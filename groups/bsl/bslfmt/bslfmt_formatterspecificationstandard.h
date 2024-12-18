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

/// Namespace class holding enums used by `FormatterSpecificationSplitter`
class FormatterSpecificationStandard_Enums
: public FormatterSpecificationSplitter_Enums {
  public:
    // TYPES

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

        // String presentation types
        e_STRING_DEFAULT,      // `n` (default for strings)
        e_STRING_ESCAPED,      // `?`

        // Integer presentation types
        e_INTEGRAL_BINARY,     // `b`
        e_INTEGRAL_BINARY_UC,  // `B`
        e_INTEGRAL_CHARACTER,  // `c`
        e_INTEGRAL_DECIMAL,    // `d` (default for integers)
        e_INTEGRAL_OCTAL,      // `o`
        e_INTEGRAL_HEX,        // `x` or `p` for pointers (default for ptrs)
        e_INTEGRAL_HEX_UC,     // `X` or `P` for pointers

        // Character presentation types
        e_CHARACTER_CHARACTER,  // `c` (default for characters)
        e_CHARACTER_ESCAPED,    // `?`

        // Boolean presentation type
        e_BOOLEAN_STRING,       // `s` (default for booleans)

        // Floating point types
        e_FLOATING_DEFAULT,       // none (default for floating points)
        e_FLOATING_HEX,           // `a`
        e_FLOATING_HEX_UC,        // `A`
        e_FLOATING_SCIENTIFIC,    // `e`
        e_FLOATING_SCIENTIFIC_UC, // `E`
        e_FLOATING_FIXED,         // `f`
        e_FLOATING_FIXED_UC,      // `F`
        e_FLOATING_GENERAL,       // `g`
        e_FLOATING_GENERAL_UC,    // `g`
    };
};

/// Utility to parse and validate a Standard format specification string for a
/// given argument type.
template <class t_CHAR>
class FormatterSpecificationStandard
: public FormatterSpecificationStandard_Enums {
  private:
    // TYPES
    typedef FormatterSpecificationSplitter<t_CHAR> Splitter;

    // DATA
    ParsingStatus d_parsingStatus;   // Parsing state
    Splitter      d_basicSplitter;   // Parsing helper
    FormatType    d_formatType;      // What type was requested
    int           d_widthArgId;      // Width argumetn number
    int           d_precisionArgId;  // Precision argument number

    // PRIVATE CLASS FUNCTIONS

    /// Parse, from the specified `typeString`, the requested format-type and
    /// load it into the `d_formatTpe` of the specified `outSpec`. The
    /// specified `category` determines the mapping table used from character
    /// to format-type. This method will throw a `bsl::format_error` exception
    /// in case the `typeString` is not empty or a valid, single format
    /// character.
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseType(
                             FormatterSpecificationStandard        *outSpec,
                             const bsl::basic_string_view<t_CHAR>&  typeString,
                             Category                               category);

  public:

    // CREATORS

    /// Create an uninitialized `FormatterSpecificationStandard` object.
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationStandard();

    // CLASS FUNCTIONS

    /// Parse a format string using the iterator-range from the specified
    /// `context` assuming it is a Standard format specification for arguments
    /// of category `category` and if successful load the results into the
    /// specified `outSpec` as well as set the status to `e_PARSED`; otherwise,
    /// if the format specification denoted by the `context` iterator-range is
    /// not a valid Standard format specification for arguments of category
    /// `category` thow a `bsl::format_error` exception.
    template <class t_PARSE_CONTEXT>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parse(
                                     FormatterSpecificationStandard *outSpec,
                                     t_PARSE_CONTEXT                *context,
                                     Category                        category);

    /// Postprocess the specified `outSpec` by using the argument values
    /// provided by the specified `context` to fill in the values of nested
    /// width or precision parameters if such nested parameters exist and set
    /// the status of `outSpec` to `e_PARSING_POSTPROCESSED`.  By nested format
    /// parameters we mean parameters whose value comes from an argument to the
    /// formatter function, and not an literal integer value within the format
    /// string.  In case of an error throw a `bsl::format_error` exception.
    template <typename t_FORMAT_CONTEXT>
    static void postprocess(FormatterSpecificationStandard *outSpec,
                            const t_FORMAT_CONTEXT&         context);

    // ACCESSORS

    /// Return a pointer to the character array that stores the parsed filler
    /// character that may be a multibyte code point or just a single character
    /// unless the status is not `e_PARSING_PARSED` or
    /// `e_PARSING_POSTPROCESSED` in which case throw a `bsl::format_error`
    /// exception indicating that error.  See also `fillerCharacters()` that
    /// provides the number of characters in the array returned by this
    /// function (at least one).
    BSLS_KEYWORD_CONSTEXPR_CPP20 const t_CHAR *filler() const;

    /// Return the number of filler characters in the array returned by
    /// `filler()` unless the status is not `e_PARSING_PARSED` or
    /// `e_PARSING_POSTPROCESSED` in which case throw a `bsl::format_error`
    /// exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 int           fillerCharacters() const;

    /// Return the display width of the code point represented by the array
    /// returned by `filler()` unless the status is not `e_PARSING_PARSED` or
    /// `e_PARSING_POSTPROCESSED` in which case throw a `bsl::format_error`
    /// exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 int           fillerCodePointDisplayWidth() const;

    /// Return the enumerator representing the requested alignment unless the
    /// status is not at least `e_PARSING_PARSED` in which case throw a
    /// `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 Alignment     alignment() const;

    /// Return the enumerator representing the requested sign-treatment option
    /// unless the status is not at least `e_PARSING_PARSED` in which case
    /// throw a `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 Sign          sign() const;

    /// Return a boolean indicating if alternative formatting was requested
    /// unless the status is not at least `e_PARSING_PARSED` in which case
    /// throw a `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool          alternativeFlag() const;

    /// Return a boolean indicating if zero padding was requested unless the
    /// status is not at least `e_PARSING_PARSED` in which case throw a
    /// `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool          zeroPaddingFlag() const;

    /// Return an optional value representing the requested width unless the
    /// status is not `e_PARSING_POSTPROCESSED` in which case throw a
    /// `bsl::format_error` exception indicating that error.  Note that the
    /// returned type is capable of representing more than just an optional
    /// integer, but after preprocessing it will have only two possible states:
    /// no value, or an integer value.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
                                               postprocessedWidth() const;

    /// Return an optional value representing the requested precision unless
    /// the status is not `e_PARSING_POSTPROCESSED` in which case throw a
    /// `bsl::format_error` exception indicating that error.  Note that the
    /// returned type is capable of representing more than just an optional
    /// integer, but after preprocessing it will have only two possible states:
    /// no value, or an integer value.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
                                               postprocessedPrecision() const;

    /// Return a boolean indicating if the locale specific flag was present in
    /// the format specification unless the status is not at least
    /// `e_PARSING_PARSED` in which case throw a `bsl::format_error` exception
    /// indicating that error.  Note that the locale specific flag is not yet
    /// supported hence the attempt to format with a specification that has
    /// this flags set will result in an exception indicating that.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool          localeSpecificFlag() const;

    /// Return the enumerator representing the requested format type requested
    /// unless the status is not at least `e_PARSING_PARSED` in which case
    /// throw a `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatType    formatType() const;
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                // --------------------------------------------
                // class FormatterSpecificationStandard<t_CHAR>
                // --------------------------------------------


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
    return d_basicSplitter.filler();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int FormatterSpecificationStandard<t_CHAR>::fillerCharacters() const
{
    return d_basicSplitter.fillerCharacters();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int FormatterSpecificationStandard<t_CHAR>::fillerCodePointDisplayWidth() const
{
    return d_basicSplitter.fillerCodePointDisplayWidth();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename
FormatterSpecificationStandard<t_CHAR>::Alignment
FormatterSpecificationStandard<t_CHAR>::alignment() const
{
    return d_basicSplitter.alignment();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename
FormatterSpecificationStandard<t_CHAR>::Sign
FormatterSpecificationStandard<t_CHAR>::sign() const
{
    return d_basicSplitter.sign();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationStandard<t_CHAR>::alternativeFlag() const
{
    return d_basicSplitter.alternativeFlag();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationStandard<t_CHAR>::zeroPaddingFlag() const
{
    return d_basicSplitter.zeroPaddingFlag();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const FormatterSpecificationNumericValue
FormatterSpecificationStandard<t_CHAR>::postprocessedWidth() const
{
    return d_basicSplitter.postprocessedWidth();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const FormatterSpecificationNumericValue
FormatterSpecificationStandard<t_CHAR>::postprocessedPrecision() const
{
    return d_basicSplitter.postprocessedPrecision();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationStandard<t_CHAR>::localeSpecificFlag() const
{
    return d_basicSplitter.localeSpecificFlag();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename FormatterSpecificationStandard<t_CHAR>::FormatType
FormatterSpecificationStandard<t_CHAR>::formatType() const
{
    if (e_PARSING_UNINITIALIZED == d_parsingStatus)
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

    typedef FormatterSpecificationNumericValue NumericValue;

    outSpec->d_parsingStatus = e_PARSING_PARSED;

    const Sections sect = static_cast<Sections>(
            e_SECTIONS_FILL_ALIGN |
            e_SECTIONS_SIGN_FLAG |
            e_SECTIONS_ALTERNATE_FLAG |
            e_SECTIONS_ZERO_PAD_FLAG |
            e_SECTIONS_WIDTH |
            e_SECTIONS_PRECISION |
            e_SECTIONS_LOCALE_FLAG |
            e_SECTIONS_FINAL_SPECIFICATION);

    FormatterSpecificationSplitter<t_CHAR>::parse(&outSpec->d_basicSplitter,
                                                   context,
                                                   sect);

    if (NumericValue::e_DEFAULT !=
            outSpec->d_basicSplitter.rawPrecision().category() &&
        e_CATEGORY_STRING != category &&
        e_CATEGORY_FLOATING != category) {
        BSLS_THROW(bsl::format_error("Standard specification parse failure "
                                     "(precision is used with inappropriate "
                                     "type)"));                       // RETURN
    }

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
    if (outSpec->d_parsingStatus == e_PARSING_UNINITIALIZED)
        BSLS_THROW(bsl::format_error(
              "Format standard specification '.parse' not called"));  // RETURN

    Splitter::postprocess(&outSpec->d_basicSplitter, context);

    switch (outSpec->d_basicSplitter.postprocessedWidth().category()) {
      case FormatterSpecificationNumericValue::e_DEFAULT: {
      } break;
      case FormatterSpecificationNumericValue::e_VALUE: {
        if (outSpec->d_basicSplitter.postprocessedWidth().value() <= 0)
            BSLS_THROW(
                 bsl::format_error("Zero or negative width value"));  // RETURN
      } break;
      default: {
        BSLS_THROW(
             bsl::format_error("Failed to find valid width value"));  // RETURN
      }
    }

    switch (outSpec->d_basicSplitter.postprocessedPrecision().category()) {
      case FormatterSpecificationNumericValue::e_DEFAULT: {
      } break;
      case FormatterSpecificationNumericValue::e_VALUE: {
        if (outSpec->d_basicSplitter.postprocessedPrecision().value() < 0)
            BSLS_THROW(
                     bsl::format_error("Negative precision value"));  // RETURN
      } break;
      default: {
        BSLS_THROW(
         bsl::format_error("Failed to find valid precision value"));  // RETURN
      }
    }

    outSpec->d_parsingStatus = e_PARSING_POSTPROCESSED;
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
            outSpec->d_formatType = e_INTEGRAL_HEX;
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
            outSpec->d_formatType = e_INTEGRAL_BINARY;
          } break;
          case 'B': {
            outSpec->d_formatType = e_INTEGRAL_BINARY_UC;
          } break;
          case 'c': {
            outSpec->d_formatType = e_CHARACTER_CHARACTER;
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
            outSpec->d_formatType = e_FLOATING_HEX;
          } break;
          case 'A': {
            outSpec->d_formatType = e_FLOATING_HEX_UC;
          } break;
          case 'e': {
            outSpec->d_formatType = e_FLOATING_SCIENTIFIC;
          } break;
          case 'E': {
            outSpec->d_formatType = e_FLOATING_SCIENTIFIC_UC;
          } break;
          case 'f': {
            outSpec->d_formatType = e_FLOATING_FIXED;
          } break;
          case 'F': {
            outSpec->d_formatType = e_FLOATING_FIXED_UC;
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
            outSpec->d_formatType = e_INTEGRAL_HEX;
          } break;
          case 'P': {
            outSpec->d_formatType = e_INTEGRAL_HEX_UC;
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
