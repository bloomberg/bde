// bslfmt_formatterspecificationstandard.h                            -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERSPECIFICATIONSTANDARD
#define INCLUDED_BSLFMT_FORMATTERSPECIFICATIONSTANDARD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide invoker adaptors for 'bsl::function'
//
//@CLASSES:
//  FormatSpecification_Splitter: Utility to tokenize format specifications.
//  FormatSpecification_Splitter_Base: Base for FormatSpecification_Splitter.
//
//@SEE_ALSO: bslfmt_format.h
//
//@DESCRIPTION: This component provides a mechanism to perform a first-pass
// split of a formatting string into its component parts in a way that is
// compatible with [format.string] and [time.format] in the Standard. No
// validation is performed by this component and further type-specific
// processing will be required prior to use.
// 
// This component is for private use only.

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
        e_BOOLEAN_STRING,     // none or `s`
        e_BOOLEAN_BINARY,     // `b`
        e_BOOLEAN_BINARY_UC,  // `B`
        e_BOOLEAN_DECIMAL,    // `d`
        e_BOOLEAN_OCTAL,      // `o`
        e_BOOLEAN_HEX,        // `x`
        e_BOOLEAN_HEX_UC,     // `X`

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

template <class t_CHAR, class t_ITER>
class FormatterSpecificationStandard
: public FormatterSpecificationStandard_Enums {
  private:
    // CLASS TYPES
    typedef FormatterSpecification_Splitter<t_CHAR, t_ITER> FSS;
    
    // DATA
    FSS        d_basicSplitter;
    FormatType d_formatType;

    // PRIVATE CLASS FUNCTIONS
    static int parseType(FormatterSpecificationStandard        *outSpec,
                         const bsl::basic_string_view<t_CHAR>&  typeString,
                         Category                               category);

  public:

    // CREATORS
    FormatterSpecificationStandard();

    // CLASS FUNCTIONS
    static int parse(FormatterSpecificationStandard *outSpec,
                     t_ITER                          *start,
                     t_ITER                           end,
                     Category                         category);

    // ACCESSORS
    const t_CHAR                   *filler();
    int                             fillerCharacters();
    Alignment                       alignment();
    Sign                            sign();
    bool                            alternativeFlag();
    bool                            zeroPaddingFlag();
    Value                           width();
    Value                           precision();
    bool                            localcSpecificFlag();
    FormatType                      formatType();
};

// PRIVATE CLASS FUNCTIONS


// CREATORS


template <class t_CHAR, class t_ITER>
FormatterSpecificationStandard<t_CHAR, t_ITER>::
    FormatterSpecificationStandard()
: d_basicSplitter()
, d_formatType(FormatterSpecificationStandard::e_TYPE_UNASSIGNED)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));
}

// ACCESSORS

template <class t_CHAR, class t_ITER>
const t_CHAR *FormatterSpecificationStandard<t_CHAR, t_ITER>::filler()
{
    return d_basicSplitter.filler();
}

template <class t_CHAR, class t_ITER>
int       FormatterSpecificationStandard<t_CHAR, t_ITER>::fillerCharacters()
{
    return d_basicSplitter.fillerCharacters();
}

template <class t_CHAR, class t_ITER>
typename
FormatterSpecificationStandard<t_CHAR, t_ITER>::Alignment
FormatterSpecificationStandard<t_CHAR, t_ITER>::alignment()
{
    return d_basicSplitter.alignment();
}

template <class t_CHAR, class t_ITER>
typename
FormatterSpecificationStandard<t_CHAR, t_ITER>::Sign
FormatterSpecificationStandard<t_CHAR, t_ITER>::sign()
{
    return d_basicSplitter.sign();
}

template <class t_CHAR, class t_ITER>
bool FormatterSpecificationStandard<t_CHAR, t_ITER>::alternativeFlag()
{
    return d_basicSplitter.alternativeFlag();
}

template <class t_CHAR, class t_ITER>
bool FormatterSpecificationStandard<t_CHAR, t_ITER>::zeroPaddingFlag()
{
    return d_basicSplitter.zeroPaddingFlag();
}

template <class t_CHAR, class t_ITER>
typename
FormatterSpecificationStandard<t_CHAR, t_ITER>::Value
FormatterSpecificationStandard<t_CHAR, t_ITER>::width()
{
    return d_basicSplitter.width();
}

template <class t_CHAR, class t_ITER>
typename
FormatterSpecificationStandard<t_CHAR, t_ITER>::Value
FormatterSpecificationStandard<t_CHAR, t_ITER>::precision()
{
    return d_basicSplitter.precision();
}

template <class t_CHAR, class t_ITER>
bool  FormatterSpecificationStandard<t_CHAR, t_ITER>::localcSpecificFlag()
{
    return d_basicSplitter.localcSpecificFlag();
}

template <class t_CHAR, class t_ITER>
typename FormatterSpecificationStandard<t_CHAR, t_ITER>::FormatType
FormatterSpecificationStandard<t_CHAR, t_ITER>::formatType()
{
    return d_formatType;
}

// MANIPULATORS

template <class t_CHAR, class t_ITER>
int FormatterSpecificationStandard<t_CHAR, t_ITER>::parse(
                                     FormatterSpecificationStandard *outSpec,
                                     t_ITER                          *start,
                                     t_ITER                           end,
                                     Category                         category)
{
    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return 0;

    const Sections sect = static_cast<Sections>(
            e_SECTIONS_FILL_ALIGN |
            e_SECTIONS_SIGN_FLAG |
            e_SECTIONS_ALTERNATE_FLAG |
            e_SECTIONS_ZERO_PAD_FLAG |
            e_SECTIONS_WIDTH |
            e_SECTIONS_PRECISION |
            e_SECTIONS_LOCALE_FLAG |
            e_SECTIONS_FINAL_SPECIFICATION);

    t_ITER current = *start;

    int rv = FSS::parse(&outSpec->d_basicSplitter, &current, end, sect);
    if (0 != rv)
        return rv;                                                    // RETURN

    rv = parseType(outSpec, outSpec->d_basicSplitter.finalSpec(), category);
    if (0 != rv)
        return rv;                                                    // RETURN

    if (current == end || *current == '}') {
        *start = current;
        return 0;                                                     // RETURN
    }

    return -1;
}

template <class t_CHAR, class t_ITER>
int FormatterSpecificationStandard<t_CHAR, t_ITER>::parseType(
                             FormatterSpecificationStandard        *outSpec,
                             const bsl::basic_string_view<t_CHAR>&  typeString,
                             Category                               category)
{
    // Handle empty string or empty specification.
    if (typeString.empty()) {
        switch (category) {
          case e_CATEGORY_UNASSIGNED: {
            outSpec->d_formatType = e_TYPE_UNASSIGNED;
            return -1;
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
            return -1;
          }
        }
        return 0;
    }

    // Standard format strings only allow a single type character.
    if (typeString.size() > 1) {
        outSpec->d_formatType = e_TYPE_UNASSIGNED;
        return -1;
    }

    static const std::ctype<wchar_t>& ct =
                std::use_facet<std::ctype<wchar_t> >(std::locale::classic());

    char typeChar = ct.narrow(typeString.front(), '\0');

    switch (category) {
      case e_CATEGORY_UNASSIGNED: {
        outSpec->d_formatType = e_TYPE_UNASSIGNED;
        return -1;
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
            return -1;
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
            return -1;
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
            return -1;
          }
        }
      } break;
      case e_CATEGORY_BOOLEAN: {
        switch (typeChar) {
          case 'b': {
            outSpec->d_formatType = e_BOOLEAN_BINARY;
          } break;
          case 'B': {
            outSpec->d_formatType = e_BOOLEAN_BINARY_UC;
          } break;
          case 'd': {
            outSpec->d_formatType = e_BOOLEAN_DECIMAL;
          } break;
          case 'o': {
            outSpec->d_formatType = e_BOOLEAN_OCTAL;
          } break;
          case 's': {
            outSpec->d_formatType = e_BOOLEAN_STRING;
          } break;
          case 'x': {
            outSpec->d_formatType = e_BOOLEAN_HEX;
          } break;
          case 'X': {
            outSpec->d_formatType = e_BOOLEAN_HEX_UC;
          } break;
          default: {
            outSpec->d_formatType = e_TYPE_UNASSIGNED;
            return -1;
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
            return -1;
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
            return -1;
          }
        }
      } break;
      default: {
        outSpec->d_formatType = e_TYPE_UNASSIGNED;
        return -1;
      }
    }

    return (e_TYPE_UNASSIGNED == outSpec->d_formatType) ? -1 : 0;
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
