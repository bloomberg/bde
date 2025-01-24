// bslfmt_standardformatspecification.h                               -*-C++-*-

#ifndef INCLUDED_BSLFMT_STANDARDFORMATSPECIFICATION
#define INCLUDED_BSLFMT_STANDARDFORMATSPECIFICATION

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Private utility for use within BSL `format` standard spec parsers
//
//@CLASSES:
//  bslfmt::StandardFormatSpecification: utility to parse a format spec
//
//@DESCRIPTION: This component provides a mechanism to parse a formatting
// string that is a "standard formatting specification" as defined by
// [format.string.std].
//
// This component is for use within `bslfmt` only.

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatspecificationparser.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_keyword.h>

namespace BloombergLP {
namespace bslfmt {

                    // ==================================
                    // struct StandardFormatSpecification
                    // ==================================

/// A general mechanism to parse and validate a Standard format specification
/// string for a given argument type.
template <class t_CHAR>
class StandardFormatSpecification
: public FormatSpecificationParserEnums {

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

  private:
    // PRIVATE TYPES
    typedef FormatSpecificationParser<t_CHAR> Parser;

    // DATA
    ParsingStatus d_processingState;  // parsing state
    Parser        d_basicParser;      // parsing helper
    FormatType    d_formatType;       // requested type

    // PRIVATE MANIPULATORS

    /// Parse, from the specified `typeString`, the requested format-type and
    /// load it into the `d_formatType` of this object.  The
    /// specified `category` determines the mapping table used from character
    /// to format-type.  This method will throw a `bsl::format_error` exception
    /// in case the `typeString` is not empty or a valid, single format
    /// character.
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseType(
                              const bsl::basic_string_view<t_CHAR>& typeString,
                              Category                              category);

  public:
    // CREATORS

    /// Create an uninitialized `StandardFormatSpecification` object.
    BSLS_KEYWORD_CONSTEXPR_CPP20 StandardFormatSpecification();

    // MANIPULATORS

    /// Parse a format string using the iterator-range from the specified
    /// `context` assuming it is a Standard format specification for arguments
    /// of category `category` and if successful update this object with the
    /// parsing result, as well as set the status to `e_PARSED`; otherwise, if
    /// the format specification denoted by the `context` iterator-range is not
    /// a valid Standard format specification for arguments of category
    /// `category` throw a `bsl::format_error` exception.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parse(t_PARSE_CONTEXT *context,
                                            Category         category);

    /// Update the corresponding attributes of this object using the argument
    /// values provided by the specified `context` to fill in the values of
    /// nested width or precision parameters if such nested parameters exist
    /// and set the status of this object to `e_PARSING_POSTPROCESSED`.  By
    /// nested format parameters we mean parameters whose value comes from an
    /// argument to the formatter function, and not an literal integer value
    /// within the format string.  In case of an error throw a
    /// `bsl::format_error` exception.
    template <typename t_FORMAT_CONTEXT>
    void postprocess(const t_FORMAT_CONTEXT& context);

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
    BSLS_KEYWORD_CONSTEXPR_CPP20 int fillerCharacters() const;

    /// Return the display width of the code point represented by the array
    /// returned by `filler()` unless the status is not `e_PARSING_PARSED` or
    /// `e_PARSING_POSTPROCESSED` in which case throw a `bsl::format_error`
    /// exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 int fillerCodePointDisplayWidth() const;

    /// Return the enumerator representing the requested alignment unless the
    /// status is not at least `e_PARSING_PARSED` in which case throw a
    /// `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 Alignment alignment() const;

    /// Return the enumerator representing the requested sign-treatment option
    /// unless the status is not at least `e_PARSING_PARSED` in which case
    /// throw a `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 Sign sign() const;

    /// Return a boolean indicating if alternative formatting was requested
    /// unless the status is not at least `e_PARSING_PARSED` in which case
    /// throw a `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool alternativeFlag() const;

    /// Return a boolean indicating if zero padding was requested unless the
    /// status is not at least `e_PARSING_PARSED` in which case throw a
    /// `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool zeroPaddingFlag() const;

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
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool localeSpecificFlag() const;

    /// Return the enumerator representing the requested format type requested
    /// unless the status is not at least `e_PARSING_PARSED` in which case
    /// throw a `bsl::format_error` exception indicating that error.
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatType formatType() const;
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                     // ---------------------------------
                     // class StandardFormatSpecification
                     // ---------------------------------

// PRIVATE CLASS METHODS

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
void StandardFormatSpecification<t_CHAR>::parseType(
                             const bsl::basic_string_view<t_CHAR>&  typeString,
                             Category                               category)
{
    // Handle empty string or empty specification.
    if (typeString.empty()) {
        switch (category) {
          case e_CATEGORY_UNASSIGNED: {
            d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
                 bsl::format_error("No default type for category"));   // THROW
          } break;
          case e_CATEGORY_STRING: {
            d_formatType = e_STRING_DEFAULT;
          } break;
          case e_CATEGORY_INTEGRAL: {
            d_formatType = e_INTEGRAL_DECIMAL;
          } break;
          case e_CATEGORY_CHARACTER: {
            d_formatType = e_CHARACTER_CHARACTER;
          } break;
          case e_CATEGORY_BOOLEAN: {
            d_formatType = e_BOOLEAN_STRING;
          } break;
          case e_CATEGORY_FLOATING: {
            d_formatType = e_FLOATING_DEFAULT;
          } break;
          case e_CATEGORY_POINTER: {
            d_formatType = e_INTEGRAL_HEX;
          } break;
          default: {
            d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
                 bsl::format_error("No default type for category"));   // THROW
          }
        }
        return;                                                       // RETURN
    }

    // Standard format strings only allow a single type character.
    if (typeString.size() > 1) {
        d_formatType = e_TYPE_UNASSIGNED;
        BSLS_THROW(
          bsl::format_error("Standard types are single-character"));   // THROW
    }

    t_CHAR frontChar = typeString.front();

    // The type character can only be ascii so we can do a simple cast.
    char typeChar = (frontChar >= 0 && frontChar <= 0x7f)
                        ? static_cast<char>(frontChar)
                        : static_cast<char>(0);

    switch (category) {
      case e_CATEGORY_UNASSIGNED: {
        d_formatType = e_TYPE_UNASSIGNED;
        BSLS_THROW(
                 bsl::format_error("Invalid type for unassigned."));   // THROW
      } break;
      case e_CATEGORY_STRING: {
        switch (typeChar) {
          case 's': {
            d_formatType = e_STRING_DEFAULT;
          } break;
          case '?': {
            d_formatType = e_STRING_ESCAPED;
          } break;
          default: {
            d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
                     bsl::format_error("Invalid type for string."));   // THROW
          }
        }
      } break;
      case e_CATEGORY_INTEGRAL: {
        switch (typeChar) {
          case 'b': {
            d_formatType = e_INTEGRAL_BINARY;
          } break;
          case 'B': {
            d_formatType = e_INTEGRAL_BINARY_UC;
          } break;
          case 'c': {
            d_formatType = e_INTEGRAL_CHARACTER;
          } break;
          case 'd': {
            d_formatType = e_INTEGRAL_DECIMAL;
          } break;
          case 'o': {
            d_formatType = e_INTEGRAL_OCTAL;
          } break;
          case 'x': {
            d_formatType = e_INTEGRAL_HEX;
          } break;
          case 'X': {
            d_formatType = e_INTEGRAL_HEX_UC;
          } break;
          default: {
            d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
                   bsl::format_error("Invalid type for integers."));   // THROW
          }
        }
      } break;
      case e_CATEGORY_CHARACTER: {
        switch (typeChar) {
          case 'b': {
            d_formatType = e_INTEGRAL_BINARY;
          } break;
          case 'B': {
            d_formatType = e_INTEGRAL_BINARY_UC;
          } break;
          case 'c': {
            d_formatType = e_CHARACTER_CHARACTER;
          } break;
          case 'd': {
            d_formatType = e_INTEGRAL_DECIMAL;
          } break;
          case 'o': {
            d_formatType = e_INTEGRAL_OCTAL;
          } break;
          case 'x': {
            d_formatType = e_INTEGRAL_HEX;
          } break;
          case 'X': {
            d_formatType = e_INTEGRAL_HEX_UC;
          } break;
          case '?': {
            d_formatType = e_CHARACTER_ESCAPED;
          } break;
          default: {
            d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
            bsl::format_error("Invalid type for character types."));   // THROW
          }
        }
      } break;
      case e_CATEGORY_BOOLEAN: {
        switch (typeChar) {
          case 'b': {
            d_formatType = e_INTEGRAL_BINARY;
          } break;
          case 'B': {
            d_formatType = e_INTEGRAL_BINARY_UC;
          } break;
          case 'd': {
            d_formatType = e_INTEGRAL_DECIMAL;
          } break;
          case 'o': {
            d_formatType = e_INTEGRAL_OCTAL;
          } break;
          case 's': {
            d_formatType = e_BOOLEAN_STRING;
          } break;
          case 'x': {
            d_formatType = e_INTEGRAL_HEX;
          } break;
          case 'X': {
            d_formatType = e_INTEGRAL_HEX_UC;
          } break;
          default: {
            d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
                bsl::format_error("Invalid type for booleans."));      // THROW
          }
        }
      } break;
      case e_CATEGORY_FLOATING: {
        switch (typeChar) {
          case 'a': {
            d_formatType = e_FLOATING_HEX;
          } break;
          case 'A': {
            d_formatType = e_FLOATING_HEX_UC;
          } break;
          case 'e': {
            d_formatType = e_FLOATING_SCIENTIFIC;
          } break;
          case 'E': {
            d_formatType = e_FLOATING_SCIENTIFIC_UC;
          } break;
          case 'f': {
            d_formatType = e_FLOATING_FIXED;
          } break;
          case 'F': {
            d_formatType = e_FLOATING_FIXED_UC;
          } break;
          case 'g': {
            d_formatType = e_FLOATING_GENERAL;
          } break;
          case 'G': {
            d_formatType = e_FLOATING_GENERAL_UC;
          } break;
          default: {
            d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
            bsl::format_error("Invalid type for floating points."));   // THROW
          }
        }
      } break;
      case e_CATEGORY_POINTER: {
        switch (typeChar) {
          case 'p': {
            d_formatType = e_INTEGRAL_HEX;
          } break;
          case 'P': {
            d_formatType = e_INTEGRAL_HEX_UC;
          } break;
          default: {
            d_formatType = e_TYPE_UNASSIGNED;
            BSLS_THROW(
                   bsl::format_error("Invalid type for pointers."));   // THROW
          }
        }
      } break;
      default: {
        d_formatType = e_TYPE_UNASSIGNED;
        BSLS_THROW(
           bsl::format_error("Invalid type for default category."));   // THROW
      }
    }

    if (e_TYPE_UNASSIGNED == d_formatType) {
        BSLS_THROW(
        bsl::format_error("Failed to parse type (reason unknown)"));   // THROW
    }
}

// CLASS METHODS

template <class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 void StandardFormatSpecification<t_CHAR>::parse(
                                                     t_PARSE_CONTEXT *context,
                                                     Category         category)
{
    BSLMF_ASSERT((
        bsl::is_same<typename bsl::iterator_traits<
                         typename t_PARSE_CONTEXT::const_iterator>::value_type,
                     t_CHAR>::value));

    typedef FormatterSpecificationNumericValue NumericValue;

    d_processingState = e_PARSING_PARSED;

    const Sections sect = static_cast<Sections>(
            e_SECTIONS_FILL_ALIGN |
            e_SECTIONS_SIGN_FLAG |
            e_SECTIONS_ALTERNATE_FLAG |
            e_SECTIONS_ZERO_PAD_FLAG |
            e_SECTIONS_WIDTH |
            e_SECTIONS_PRECISION |
            e_SECTIONS_LOCALE_FLAG |
            e_SECTIONS_FINAL_SPECIFICATION);

    d_basicParser.parse(context, sect);

    if (NumericValue::e_DEFAULT != d_basicParser.rawPrecision().category() &&
        e_CATEGORY_STRING != category && e_CATEGORY_FLOATING != category) {
        BSLS_THROW(bsl::format_error("Standard specification parse failure "
                                     "(precision is used with inappropriate "
                                     "type)"));                        // THROW
    }

    parseType(d_basicParser.finalSpec(), category);

    if (context->begin() == context->end() || *context->begin() == '}') {
        return;                                                       // RETURN
    }

    BSLS_THROW(bsl::format_error("Standard specification parse failure "
                                 "(invalid character)"));              // THROW
}

template <class t_CHAR>
template <typename t_FORMAT_CONTEXT>
void StandardFormatSpecification<t_CHAR>::postprocess(
                                               const t_FORMAT_CONTEXT& context)
{
    if (d_processingState == e_PARSING_UNINITIALIZED)
        BSLS_THROW(bsl::format_error(
              "Format standard specification '.parse' not called"));   // THROW

    d_basicParser.postprocess(context);

    switch (d_basicParser.postprocessedWidth().category()) {
      case FormatterSpecificationNumericValue::e_DEFAULT: {
      } break;
      case FormatterSpecificationNumericValue::e_VALUE: {
        if (d_basicParser.postprocessedWidth().value() <= 0)
            BSLS_THROW(
                 bsl::format_error("Zero or negative width value"));   // THROW
      } break;
      default: {
        BSLS_THROW(
             bsl::format_error("Failed to find valid width value"));   // THROW
      }
    }

    switch (d_basicParser.postprocessedPrecision().category()) {
      case FormatterSpecificationNumericValue::e_DEFAULT: {
      } break;
      case FormatterSpecificationNumericValue::e_VALUE: {
        if (d_basicParser.postprocessedPrecision().value() < 0)
            BSLS_THROW(
                     bsl::format_error("Negative precision value"));   // THROW
      } break;
      default: {
        BSLS_THROW(
         bsl::format_error("Failed to find valid precision value"));   // THROW
      }
    }

    d_processingState = e_PARSING_POSTPROCESSED;
}

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
StandardFormatSpecification<t_CHAR>::StandardFormatSpecification()
: d_processingState(StandardFormatSpecification::e_PARSING_UNINITIALIZED)
, d_basicParser()
, d_formatType(StandardFormatSpecification::e_TYPE_UNASSIGNED)
{
}

// ACCESSORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const t_CHAR *StandardFormatSpecification<t_CHAR>::filler() const
{
    return d_basicParser.filler();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int StandardFormatSpecification<t_CHAR>::fillerCharacters() const
{
    return d_basicParser.fillerCharacters();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int StandardFormatSpecification<t_CHAR>::fillerCodePointDisplayWidth() const
{
    return d_basicParser.fillerCodePointDisplayWidth();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename
StandardFormatSpecification<t_CHAR>::Alignment
StandardFormatSpecification<t_CHAR>::alignment() const
{
    return d_basicParser.alignment();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename
StandardFormatSpecification<t_CHAR>::Sign
StandardFormatSpecification<t_CHAR>::sign() const
{
    return d_basicParser.sign();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool StandardFormatSpecification<t_CHAR>::alternativeFlag() const
{
    return d_basicParser.alternativeFlag();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool StandardFormatSpecification<t_CHAR>::zeroPaddingFlag() const
{
    return d_basicParser.zeroPaddingFlag();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const FormatterSpecificationNumericValue
StandardFormatSpecification<t_CHAR>::postprocessedWidth() const
{
    return d_basicParser.postprocessedWidth();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const FormatterSpecificationNumericValue
StandardFormatSpecification<t_CHAR>::postprocessedPrecision() const
{
    return d_basicParser.postprocessedPrecision();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool StandardFormatSpecification<t_CHAR>::localeSpecificFlag() const
{
    return d_basicParser.localeSpecificFlag();
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename StandardFormatSpecification<t_CHAR>::FormatType
StandardFormatSpecification<t_CHAR>::formatType() const
{
    if (e_PARSING_UNINITIALIZED == d_processingState)
        BSLS_THROW(bsl::format_error(
              "Format standard specification '.parse' not called"));   // THROW

    return d_formatType;
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_STANDARDFORMATSPECIFICATION

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
