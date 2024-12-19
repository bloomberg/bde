// bslfmt_formatterspecificationsplitter.h                            -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERSPECIFICATIONSPLITTER
#define INCLUDED_BSLFMT_FORMATTERSPECIFICATIONSPLITTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Tokenization utility for use within BSL `format` spec parsers
//
//@CLASSES:
//  FormatterSpecificationSplitter: Utility to tokenize format specifications.
//
//@SEE_ALSO: bslfmt_format.h
//
//@DESCRIPTION: This component provides a mechanism to perform a first-pass
// split of a formatting string into its component parts in a way that is
// compatible with [format.string] and [time.format] in the Standard. No
// validation is performed by this component and further type-specific
// processing will be required prior to use.
//
// Note a default-initialized `FormatSpecificationSplitter` has the following
// defaults:
// * sections:               e_SECTIONS_NONE
// * filler:                 zero-terminated empty string
// * numFillerCharacters:    1 (the null termination)
// * fillerDisplayWidth:     1
// * alignment:              e_ALIGN_DEFAULT
// * sign:                   e_SIGN_DEFAULT
// * alternativeFlag:        false
// * zeroPaddingFlag:        false
// * rawWidth:               e_DEFAULT
// * rawPrecision:           e_DEFAULT
// * postprocessedWidth:     e_DEFAULT
// * postprocessedPrecision: e_DEFAULT
// * localeSpecificFlag:     false
//
// This component is for use by formatters in BSL only (primarily in `bslfmt`)

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
#include <bslstl_monostate.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterspecificationnumericvalue.h>
#include <bslfmt_formatterunicodeutils.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <format>     // for 'std::visit_format_arg'
#endif

namespace BloombergLP {
namespace bslfmt {

              // ------------------------------------------
              // class FormatterSpecificationSplitter_Enums
              // ------------------------------------------

struct FormatterSpecificationSplitter_Enums {
  public:
    // TYPES
    enum ParsingStatus {
        e_PARSING_UNINITIALIZED,
        e_PARSING_PARSED,
        e_PARSING_POSTPROCESSED
    };

    enum Alignment {
        e_ALIGN_DEFAULT,
        e_ALIGN_LEFT,
        e_ALIGN_MIDDLE,
        e_ALIGN_RIGHT
    };

    enum Sign {
        e_SIGN_DEFAULT,
        e_SIGN_POSITIVE,
        e_SIGN_NEGATIVE,
        e_SIGN_SPACE
    };

    enum Sections {
        e_SECTIONS_NONE                = 0,
        e_SECTIONS_FILL_ALIGN          = 1,
        e_SECTIONS_SIGN_FLAG           = 2,
        e_SECTIONS_ALTERNATE_FLAG      = 4,
        e_SECTIONS_ZERO_PAD_FLAG       = 8,
        e_SECTIONS_WIDTH               = 16,
        e_SECTIONS_PRECISION           = 32,
        e_SECTIONS_LOCALE_FLAG         = 64,
        e_SECTIONS_NO_BRACKETS_FLAG    = 128,
        e_SECTIONS_FINAL_SPECIFICATION = 256,
        e_SECTIONS_ALL                 = 512 - 1
    };
};

                 // --------------------------------------------
                 // class FormatterSpecificationSplitter<t_CHAR>
                 // --------------------------------------------

/// A general mechanism used in format specification parsers.  It supports
/// parsing syntaxes of the form `fill-and-align?sign # 0 width?precision?L`
/// and any syntax that contains a subset of the sections therein.
template <class t_CHAR>
class FormatterSpecificationSplitter
: public FormatterSpecificationSplitter_Enums {

  private:
    // PRIVATE CLASS DATA
    static BSLS_KEYWORD_CONSTEXPR_MEMBER size_t k_FILLER_BUFFER_SIZE = 5;
        // Maximum required buffer size to hold a null-terminated unicode code
        // point.

    // DATA
    Sections      d_sections;           // List of sections extracted
    ParsingStatus d_processingState;    // Current state

    t_CHAR d_filler[k_FILLER_BUFFER_SIZE];  // The filler code point
    int    d_numFillerCharacters;           // number of characters in filler
    int    d_fillerDisplayWidth;   // display width of code point

    Alignment                           d_alignment;              // alignment
    Sign                                d_sign;                   // sign
    bool                                d_alternativeFlag;        // alt (#)
    bool                                d_zeroPaddingFlag;        // zero (0)
    FormatterSpecificationNumericValue  d_rawWidth;               // raw wdith
    FormatterSpecificationNumericValue  d_rawPrecision;           // raw prec
    FormatterSpecificationNumericValue  d_postprocessedWidth;     // width
    FormatterSpecificationNumericValue  d_postprocessedPrecision; // precisions
    bool                                d_localeSpecificFlag;     // locale (L)
    bsl::basic_string_view<t_CHAR>      d_spec;                   // input spec

    // PRIVATE CLASS METHODS

    /// Determine and return the value of the Alignment enum represented by the
    /// specified `in`, determined by the mapping given by the Standard C++
    /// formatting specification.  In the event of an error throw a
    /// `format_error` exception.
    static BSLS_KEYWORD_CONSTEXPR_CPP20 Alignment alignmentFromChar(t_CHAR in);

    /// Determine and return the value of the Sign enum represented by the
    /// specified `in`, determined by the mapping given by the Standard C++
    /// formatting specification.  In the event of an error throw a
    /// `format_error` exception.
    static BSLS_KEYWORD_CONSTEXPR_CPP20 Sign signFromChar(t_CHAR in);

    /// Extract a filler code point and `Alignment` value from start of the
    /// string specified by the `start` and `end` random-access-iterator range,
    /// and update the specified output `outSpec` with the result.  In the
    /// event of an error throw a `format_error` exception.
    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseFillAndAlignment(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    /// Where the specified output `outSpec` contains a filler code point,
    /// calculate the Unicode display width of that code point and update
    /// `outSpec` with the result.  In the event of an error throw a
    /// `format_error` exception.
    static void postprocessFiller(FormatterSpecificationSplitter *outSpec);

    /// Extract a `Sign` value from the start of the string specified by the
    /// `start` and `end` random-access-iterator range, and update the
    /// specified output `outSpec` with the result.  In the event of an error
    /// throw a `format_error` exception.
    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseSign(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    /// Determine whether the start of the string specified by the `start` and
    /// `end` random-access-iterator range contains a flag (`#`) indicating
    /// alternative format processing and if so return true.  Otherwise return
    /// false.
    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseAlternateOption(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    /// Determine whether the start of the string specified by the `start` and
    /// `end` random-access-iterator range contains a flag (`0`) indicating
    /// zero padding and if so return true.  Otherwise return false.
    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseZeroPaddingFlag(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    /// Extract a width from the start of the string specified by the `start`
    /// and `end` random-access-iterator range, and update the specified output
    /// `outSpec` with the result.  In the event of an error throw a
    /// `format_error` exception.  Note that the extracted precision may be
    /// hard-coded or nested and is not itself validated at this stage.
    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseRawWidth(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    /// Extract a precision from the start of the string specified by the
    /// `start` and `end` random-access-iterator range, and update the
    /// specified output `outSpec` with the result.  In the event of an error
    /// throw a `format_error` exception.  Note that the extracted precision
    /// may be hard-coded or nested and is not itself validated at this stage.
    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseRawPrecision(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    /// Determine whether the start of the string specified by the `start` and
    /// `end` random-access-iterator range contains a flag (`L`) indicating
    /// locale specific formatting and update the specified output `outSpec`
    /// with the result.
    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseLocaleSpecificFlag(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    /// Parse the  string specified by the `start` and `end`
    /// random-access-iterator range to extract the items indicated by the
    /// specified `sections` and update the specified output `outSpec` with the
    /// result.  In the event of an error throw a `format_error` exception.
    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void
    rawParse(FormatterSpecificationSplitter *outSpec,  // output
             t_ITER                         *start,    // output
             t_ITER                          end,      // input
             Sections                        sections);                       // param

  public:

    // CLASS METHODS

    /// Parse the string contained by the specified input/output `parseContext`
    /// to extract the items indicated by the specified `sections` and update
    /// the specified output `outSpec` with the result.  Update the iterator
    /// held by `parseContext` to point to the start of the unparsed section of
    /// the string.  In the event of an error throw a `format_error` exception.
    template <class t_PARSE_CONTEXT>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void
    parse(FormatterSpecificationSplitter *outSpec,      // output
          t_PARSE_CONTEXT                *parseContext, // output
          Sections                        sections);    // param

    /// Update any nested width and precision values held by the specified
    /// `outSpec` to contain actual values based on the appropriate arguments
    /// held by the specified `context` format context.  In the event of an
    /// error throw a `format_error` exception.
    template <typename t_FORMAT_CONTEXT>
    static void postprocess(FormatterSpecificationSplitter *outSpec,
                            const t_FORMAT_CONTEXT&         context);

    // CREATORS

    /// Construct a default instance of `FormatterSpecificationSplitter`.
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationSplitter();

    // ACCESSORS

    /// Return the stored filler string.  If `parse` has not previously been
    /// called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const t_CHAR *filler() const;

    /// Return the size of the stored filler string.  If `parse` has not
    /// previously been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 int           fillerCharacters() const;

    BSLS_KEYWORD_CONSTEXPR_CPP20 int       fillerCodePointDisplayWidth() const;

    /// Return the stored alignment value.  If `parse` has not
    /// previously been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 Alignment alignment() const;

    /// Return the stored sign value.  If `parse` has not previously been
    /// called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 Sign      sign() const;

    /// Return true if parsing detected an alternative processing flag in the
    /// specification, otherwise return false.  If `parse` has not previously
    /// been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool      alternativeFlag() const;

    /// Return true if parsing detected a zero padding flag in the
    /// specification, otherwise return false.  If `parse` has not previously
    /// been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool      zeroPaddingFlag() const;

    /// Return the modified numeric width extracted during postprocessing. If
    /// `postprocess` has not previously been called throw a `format_error`
    /// exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
    postprocessedWidth() const;

    /// Return the modified numeric precision extracted during postprocessing.
    /// If `postprocess` has not previously been called throw a `format_error`
    /// exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
    postprocessedPrecision() const;

    /// Return the unmodified width extracted during parsing.  If `parse` has
    /// not previously been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
    rawWidth() const;

    /// Return the unmodified precision extracted during parsing.  If `parse`
    /// has not previously been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
                                      rawPrecision() const;

    /// Return true if parsing detected a locale-specific formatting flag in
    /// the specification, otherwise return false.  If `parse` has not
    /// previously been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool localeSpecificFlag() const;

    /// Return that section of the specification string parsed during the call
    /// to `parse`.  Note that this may be a truncated version of the string
    /// held by the parse context.  If `parse` has not previously been called
    /// throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const bsl::basic_string_view<t_CHAR>
    finalSpec() const;
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================


               // --------------------------------------------
               // class FormatterSpecificationSplitter<t_CHAR>
               // --------------------------------------------

// CREATORS

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatterSpecificationSplitter<t_CHAR>::FormatterSpecificationSplitter()
: d_sections(e_SECTIONS_NONE)
, d_processingState(e_PARSING_UNINITIALIZED)
, d_filler()
, d_numFillerCharacters(1)
, d_fillerDisplayWidth(1)
, d_alignment(FormatterSpecificationSplitter::e_ALIGN_DEFAULT)
, d_sign(FormatterSpecificationSplitter::e_SIGN_DEFAULT)
, d_alternativeFlag(false)
, d_zeroPaddingFlag(false)
, d_rawWidth(0, FormatterSpecificationNumericValue::e_DEFAULT)
, d_rawPrecision(0, FormatterSpecificationNumericValue::e_DEFAULT)
, d_postprocessedWidth(0, FormatterSpecificationNumericValue::e_DEFAULT)
, d_postprocessedPrecision(0, FormatterSpecificationNumericValue::e_DEFAULT)
, d_localeSpecificFlag(false)
{}

// ACCESSORS

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const t_CHAR *FormatterSpecificationSplitter<t_CHAR>::filler() const
{
    if (e_PARSING_UNINITIALIZED == d_processingState)
        BSLS_THROW(bsl::format_error(
            "Format specification '.parse' not called (`filler`)"));  // RETURN

    return d_filler;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int FormatterSpecificationSplitter<t_CHAR>::fillerCharacters() const
{
    if (e_PARSING_UNINITIALIZED == d_processingState)
        BSLS_THROW(
                  bsl::format_error("Format specification '.parse' not called "
                                    "(`fillerCharacters`)"));         // RETURN

    return d_numFillerCharacters;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 int
FormatterSpecificationSplitter<t_CHAR>::fillerCodePointDisplayWidth() const
{
    if (e_PARSING_POSTPROCESSED != d_processingState)
        BSLS_THROW(
            bsl::format_error("Format specification '.postprocess' not called "
                              "(`fillerCodePointDisplayWidth`)"));    // RETURN

    return d_fillerDisplayWidth;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename
FormatterSpecificationSplitter<t_CHAR>::Alignment
FormatterSpecificationSplitter<t_CHAR>::alignment() const
{
    if (e_PARSING_UNINITIALIZED == d_processingState)
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`alignment`)"));        // RETURN

    return d_alignment;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename
FormatterSpecificationSplitter<t_CHAR>::Sign
FormatterSpecificationSplitter<t_CHAR>::sign() const
{
    if (e_PARSING_UNINITIALIZED == d_processingState)
        BSLS_THROW(bsl::format_error(
              "Format specification '.parse' not called (`sign`)"));  // RETURN

    return d_sign;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationSplitter<t_CHAR>::alternativeFlag() const
{
    if (e_PARSING_UNINITIALIZED == d_processingState)
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`alternativeFlag`)"));  // RETURN

    return d_alternativeFlag;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationSplitter<t_CHAR>::zeroPaddingFlag() const
{
    if (e_PARSING_UNINITIALIZED == d_processingState)
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`zeroPaddingFlag`)"));  // RETURN

    return d_zeroPaddingFlag;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
FormatterSpecificationSplitter<t_CHAR>::postprocessedWidth() const
{
    if (e_PARSING_POSTPROCESSED != d_processingState)
        BSLS_THROW(
            bsl::format_error("Format specification '.postprocess' not called "
                              "(`postprocessedWidth`)"));             // RETURN

    return d_postprocessedWidth;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
FormatterSpecificationSplitter<t_CHAR>::rawWidth() const
{
    if (e_PARSING_UNINITIALIZED == d_processingState)
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`rawWidth`)"));         // RETURN

    return d_rawWidth;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
FormatterSpecificationSplitter<t_CHAR>::postprocessedPrecision() const
{
    if (e_PARSING_POSTPROCESSED != d_processingState)
        BSLS_THROW(
            bsl::format_error("Format specification '.postprocess' not called "
                              "(`postprocessedPrecision`)"));         // RETURN

    return d_postprocessedPrecision;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
FormatterSpecificationSplitter<t_CHAR>::rawPrecision() const
{
    if (e_PARSING_UNINITIALIZED == d_processingState)
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`rawPrecision`)"));     // RETURN

    return d_rawPrecision;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationSplitter<t_CHAR>::localeSpecificFlag() const
{
    if (e_PARSING_UNINITIALIZED == d_processingState)
        BSLS_THROW(
                  bsl::format_error("Format specification '.parse' not called "
                                    "(`localeSpecificFlag`)"));       // RETURN

    return d_localeSpecificFlag;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const bsl::basic_string_view<t_CHAR>
FormatterSpecificationSplitter<t_CHAR>::finalSpec() const
{
    if (e_PARSING_UNINITIALIZED == d_processingState)
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`finalSpec`)"));        // RETURN

    return d_spec;
}

// PRIVATE CLASS FUNCTIONS

template <class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR_CPP20
typename FormatterSpecificationSplitter<t_CHAR>::Alignment
FormatterSpecificationSplitter<t_CHAR>::alignmentFromChar(t_CHAR in)
{
    if ('<' == in)
        return e_ALIGN_LEFT;                                          // RETURN

    if ('>' == in)
        return e_ALIGN_RIGHT;                                         // RETURN

    if ('^' == in)
        return e_ALIGN_MIDDLE;                                        // RETURN

    return e_ALIGN_DEFAULT;
}

template <class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR_CPP20
typename FormatterSpecificationSplitter<t_CHAR>::Sign
FormatterSpecificationSplitter<t_CHAR>::signFromChar(t_CHAR in)
{
    if ('+' == in)
        return e_SIGN_POSITIVE;                                       // RETURN

    if ('-' == in)
        return e_SIGN_NEGATIVE;                                       // RETURN

    if (' ' == in)
        return e_SIGN_SPACE;                                          // RETURN

    return e_SIGN_DEFAULT;
}

template <class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
FormatterSpecificationSplitter<t_CHAR>::parse(
                                     FormatterSpecificationSplitter *outSpec,
                                     t_PARSE_CONTEXT                *parseContext,
                                     Sections                        sections)
{
    BSLMF_ASSERT((
        bsl::is_same<typename bsl::iterator_traits<
                         typename t_PARSE_CONTEXT::const_iterator>::value_type,
                     t_CHAR>::value));

    outSpec->d_processingState = e_PARSING_PARSED;

    typename t_PARSE_CONTEXT::const_iterator current = parseContext->begin();
    typename t_PARSE_CONTEXT::const_iterator end     = parseContext->end();

    rawParse(outSpec, &current, end, sections);

    if (0 != (sections & e_SECTIONS_WIDTH)) {
        if (outSpec->rawWidth().category() ==
            FormatterSpecificationNumericValue::e_ARG_ID) {
            parseContext->check_arg_id(outSpec->rawWidth().value());
        }
        else if (outSpec->rawWidth().category() ==
                 FormatterSpecificationNumericValue::e_NEXT_ARG) {
            outSpec->d_rawWidth = FormatterSpecificationNumericValue(
                                static_cast<int>(parseContext->next_arg_id()),
                                FormatterSpecificationNumericValue::e_ARG_ID);
        }
    }

    if (0 != (sections & e_SECTIONS_PRECISION)) {
        if (outSpec->rawPrecision().category() ==
            FormatterSpecificationNumericValue::e_ARG_ID) {
            parseContext->check_arg_id(outSpec->rawPrecision().value());
        }
        else if (outSpec->rawPrecision().category() ==
                 FormatterSpecificationNumericValue::e_NEXT_ARG) {
            outSpec->d_rawPrecision = FormatterSpecificationNumericValue(
                                static_cast<int>(parseContext->next_arg_id()),
                                FormatterSpecificationNumericValue::e_ARG_ID);
        }
    }

    if (current == end || *current == '}') {
        parseContext->advance_to(current);
        return;                                                       // RETURN
    }

    BSLS_THROW(bsl::format_error("Standard specification parse failure "
                                 "(invalid character)"));             // RETURN
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
FormatterSpecificationSplitter<t_CHAR>::rawParse(
                                     FormatterSpecificationSplitter *outSpec,
                                     t_ITER                         *start,
                                     t_ITER                          end,
                                     Sections                        sections)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    outSpec->d_sections = sections;

    // Reasonable defaults for early exit.
    outSpec->d_filler[0]           = ' ';
    outSpec->d_numFillerCharacters = 1;
    outSpec->d_fillerDisplayWidth  = 1;

    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return;                                                       // RETURN

    if (0 != (sections & e_SECTIONS_FILL_ALIGN)) {
        parseFillAndAlignment(outSpec, start, end);

        if (*start == end || **start == '}')
            return;                                                   // RETURN
    }

    if (0 != (sections & e_SECTIONS_SIGN_FLAG)) {
        parseSign(outSpec, start, end);

        if (*start == end || **start == '}')
            return;                                                   // RETURN
    }

    if (0 != (sections & e_SECTIONS_ALTERNATE_FLAG)) {
        parseAlternateOption(outSpec, start, end);

        if (*start == end || **start == '}')
            return;                                                   // RETURN
    }

    if (0 != (sections & e_SECTIONS_ZERO_PAD_FLAG)) {
        parseZeroPaddingFlag(outSpec, start, end);

        if (*start == end || **start == '}')
            return;                                                   // RETURN
    }

    if (0 != (sections & e_SECTIONS_WIDTH)) {
        parseRawWidth(outSpec, start, end);

        if (*start == end || **start == '}')
            return;                                                   // RETURN
    }

    if (0 != (sections & e_SECTIONS_PRECISION)) {
        parseRawPrecision(outSpec, start, end);

        if (*start == end || **start == '}')
            return;                                                   // RETURN
    }

    if (0 != (sections & e_SECTIONS_LOCALE_FLAG)) {
        parseLocaleSpecificFlag(outSpec, start, end);

        if (*start == end || **start == '}')
            return;                                                   // RETURN
    }

    if (0 != (sections & e_SECTIONS_FINAL_SPECIFICATION)) {
        bsl::basic_string_view<t_CHAR> temp(*start, end);

        const BSLS_KEYWORD_CONSTEXPR t_CHAR braces[3] = {'{', '}', '\0'};

        // Take anything left up to but not including any brace.
        size_t brace_pos = temp.find_first_of(braces);
        if (brace_pos != bsl::basic_string_view<t_CHAR>::npos) {
            // Brace found
            outSpec->d_spec = temp.substr(0, brace_pos);
            *start += brace_pos;
        }
        else {
            // No brace found
            outSpec->d_spec = temp;
            *start          = end;
        }
    }

    // We cannot mix specified and unspecified relative argument ids.

    if (outSpec->d_rawWidth.category() ==
            FormatterSpecificationNumericValue::e_NEXT_ARG &&
        outSpec->d_rawPrecision.category() ==
            FormatterSpecificationNumericValue::e_ARG_ID) {
        BSLS_THROW(bsl::format_error(
                       "Cannot mix manual and automatic indexing"));  // RETURN
    }

    if (outSpec->d_rawWidth.category() ==
            FormatterSpecificationNumericValue::e_ARG_ID &&
        outSpec->d_rawPrecision.category() ==
            FormatterSpecificationNumericValue::e_NEXT_ARG) {
        BSLS_THROW(bsl::format_error(
                       "Cannot mix manual and automatic indexing"));  // RETURN
    }

    if (*start == end || **start == '}')
        return;                                                       // RETURN

     BSLS_THROW(bsl::format_error(
                "Specification parse failure (invalid character)"));  // RETURN
}


template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20
void FormatterSpecificationSplitter<t_CHAR>::parseFillAndAlignment(
                                     FormatterSpecificationSplitter *outSpec,
                                     t_ITER                         *start,
                                     t_ITER                          end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}') {
        outSpec->d_filler[0]           = ' ';
        outSpec->d_numFillerCharacters = 1;
        outSpec->d_fillerDisplayWidth  = 1;
        return;                                                       // RETURN
    }

    int cpBytes = Formatter_UnicodeUtils::codepointBytesIfValid(**start);

    if (cpBytes < 0)
        BSLS_THROW(bsl::format_error("Invalid unicode code point"));  // RETURN

    int cpChars = cpBytes / sizeof(t_CHAR);

    t_ITER tempIter = *start;
    for (int i = 0; i < cpChars; ++i) {
        if (tempIter == end)
            BSLS_THROW(
            bsl::format_error("Unicode code point: too few bytes"));  // RETURN
        outSpec->d_filler[i] = *tempIter;
        ++tempIter;
    }
    outSpec->d_filler[cpChars] = 0;

    outSpec->d_numFillerCharacters = cpChars;

    t_ITER aligner = tempIter;

    if (aligner != end) {
        if (alignmentFromChar(*aligner) != e_ALIGN_DEFAULT) {
            // `{` and `}` are invalid fill characters per the C++ spec. As
            // they are ASCII we can check **start without decoding the code
            // point.
            if ('{' == **start || '}' == **start)
                BSLS_THROW(bsl::format_error(
                            "Invalid fill character ('{' or '}')"));  // RETURN

            outSpec->d_alignment = alignmentFromChar(*aligner);

            // We cannot know this yet - put in a placeholder.
            outSpec->d_fillerDisplayWidth = -1;

            *start = aligner + 1;
            // Fill and alignment specifier given.
            return;                                                   // RETURN
        }
    }

    // If the alignment specifier is not in 2nd position, it is allowed to be
    // in the first position, in which case the spec states that a space should
    // be used as the filler character.
    outSpec->d_filler[0]           = ' ';
    outSpec->d_numFillerCharacters = 1;
    outSpec->d_fillerDisplayWidth  = 1;
    aligner                        = *start;
    if (alignmentFromChar(*aligner) != e_ALIGN_DEFAULT) {
        outSpec->d_alignment = alignmentFromChar(*aligner);
        *start               = aligner + 1;
        // Alignment specifier with default (ascii space) fill character
        return;                                                       // RETURN
    }

    // Otherwise we have neither fill nor alignment.
    return;                                                           // RETURN
}

template <class t_CHAR>
void
FormatterSpecificationSplitter<t_CHAR>::postprocessFiller(
                                      FormatterSpecificationSplitter *outSpec)
{
    bslfmt::Formatter_UnicodeUtils::CodePointExtractionResult cp;
    switch (sizeof(t_CHAR)) {
      case 1: {
        cp = Formatter_UnicodeUtils::extractCodePoint(
                                               Formatter_UnicodeUtils::e_UTF8,
                                               (const void *)outSpec->d_filler,
                                               outSpec->d_numFillerCharacters);
      } break;
      case 2: {
        cp = Formatter_UnicodeUtils::extractCodePoint(
                                           Formatter_UnicodeUtils::e_UTF16,
                                           (const void *)outSpec->d_filler,
                                           outSpec->d_numFillerCharacters * 2);
      } break;
      case 4: {
        cp = Formatter_UnicodeUtils::extractCodePoint(
                                           Formatter_UnicodeUtils::e_UTF32,
                                           (const void *)outSpec->d_filler,
                                           outSpec->d_numFillerCharacters * 4);
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Unsupported wchar_t size"));    // RETURN
      }
    }

    if (!cp.isValid)
        BSLS_THROW(bsl::format_error("Invalid unicode code point"));  // RETURN

    if (static_cast<size_t>(cp.numSourceBytes) !=
        outSpec->d_numFillerCharacters * sizeof(t_CHAR)) {
        BSLS_THROW(bsl::format_error("Invalid unicode code point"));  // RETURN
    }

    // `{` and `}` are invalid fill characters per the C++ spec
    if ('{' == cp.codePointValue || '}' == cp.codePointValue) {
        BSLS_THROW(
          bsl::format_error("Invalid fill character ('{' or '}')"));  // RETURN
    }

    if (outSpec->d_fillerDisplayWidth >= 0) {
        if (outSpec->d_fillerDisplayWidth != cp.codePointWidth) {
            BSLS_THROW(
                     bsl::format_error("Invalid code point width"));  // RETURN
        }
    }
    else {
        outSpec->d_fillerDisplayWidth = cp.codePointWidth;
    }

    return;
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20
void FormatterSpecificationSplitter<t_CHAR>::parseSign(
                                     FormatterSpecificationSplitter *outSpec,
                                     t_ITER                         *start,
                                     t_ITER                          end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return;                                                       // RETURN

    outSpec->d_sign = signFromChar(**start);

    if (e_SIGN_DEFAULT != outSpec->d_sign) {
        ++*start;
    }

    return;
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20
void FormatterSpecificationSplitter<t_CHAR>::parseAlternateOption(
                                     FormatterSpecificationSplitter *outSpec,
                                     t_ITER                         *start,
                                     t_ITER                          end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return;                                                       // RETURN

    outSpec->d_alternativeFlag = ((t_CHAR)'#' == **start);

    if (outSpec->d_alternativeFlag) {
        ++*start;
    }

    return;
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20
void FormatterSpecificationSplitter<t_CHAR>::parseZeroPaddingFlag(
                                     FormatterSpecificationSplitter *outSpec,
                                     t_ITER                         *start,
                                     t_ITER                          end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return;                                                       // RETURN

    outSpec->d_zeroPaddingFlag = ((t_CHAR)'0' == **start);

    if (outSpec->d_zeroPaddingFlag) {
        ++*start;
    }

    return;
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20
void FormatterSpecificationSplitter<t_CHAR>::parseRawWidth(
                                     FormatterSpecificationSplitter *outSpec,
                                     t_ITER                         *start,
                                     t_ITER                          end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return;                                                       // RETURN

    typedef FormatterSpecificationNumericValue     FSNValue;

    FSNValue::parse(&outSpec->d_rawWidth, start, end, false);
    // Non-relative widths must be strictly positive.
    if (outSpec->d_rawWidth == FSNValue(0, FSNValue::e_VALUE))
        BSLS_THROW(bsl::format_error("Field widths must be > 0."));   // RETURN
    return;
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20
void FormatterSpecificationSplitter<t_CHAR>::parseRawPrecision(
                                     FormatterSpecificationSplitter *outSpec,
                                     t_ITER                         *start,
                                     t_ITER                          end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return;                                                       // RETURN

    typedef FormatterSpecificationNumericValue FSNValue;

    return FSNValue::parse(&outSpec->d_rawPrecision, start, end, true);
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20
void FormatterSpecificationSplitter<t_CHAR>::parseLocaleSpecificFlag(
                                     FormatterSpecificationSplitter *outSpec,
                                     t_ITER                         *start,
                                     t_ITER                          end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return;                                                       // RETURN

    outSpec->d_localeSpecificFlag = ((t_CHAR)'L' == **start);

    if (outSpec->d_localeSpecificFlag) {
        ++*start;
    }

    return;
}


template <class t_CHAR>
template <typename t_FORMAT_CONTEXT>
void FormatterSpecificationSplitter<t_CHAR>::postprocess(
                                      FormatterSpecificationSplitter *out,
                                      const t_FORMAT_CONTEXT&         context)
{
    if (0 != (out->d_sections & e_SECTIONS_FILL_ALIGN)) {
        FormatterSpecificationSplitter::postprocessFiller(out);
    }

    if (0 != (out->d_sections & e_SECTIONS_WIDTH)) {
        out->d_postprocessedWidth = out->d_rawWidth;
        FormatterSpecificationNumericValue::postprocess(
                                                    &out->d_postprocessedWidth,
                                                    context);
    }

    if (0 != (out->d_sections & e_SECTIONS_PRECISION)) {
        out->d_postprocessedPrecision = out->d_rawPrecision;
        FormatterSpecificationNumericValue::postprocess(
                                                &out->d_postprocessedPrecision,
                                                context);
    }

    out->d_processingState = e_PARSING_POSTPROCESSED;
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
