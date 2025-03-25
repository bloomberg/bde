// bslfmt_formatspecificationparser.h                                 -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATSPECIFICATIONPARSER
#define INCLUDED_BSLFMT_FORMATSPECIFICATIONPARSER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Tokenization utility for use within BSL `format` spec parsers
//
//@CLASSES:
//  bslfmt::FormatSpecificationParserEnums: namespace holding enums for parsing
//  bslfmt::FormatSpecificationParser: utility to tokenize format specs
//
//@SEE_ALSO: bslfmt_format.h
//
//@DESCRIPTION: This component provides a mechanism to perform a first-pass
// split of a formatting string into its component parts in a way that is
// compatible with [format.string] and [time.format] in the Standard.  No
// validation is performed by this component and further type-specific
// processing will be required prior to use.
//
// Note a default-initialized `FormatSpecificationParser` has the following
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
// This component is for use within `bslfmt` only.

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterspecificationnumericvalue.h>
#include <bslfmt_unicodecodepoint.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_iterator.h>

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <format>     // for 'std::visit_format_arg'
#endif

namespace BloombergLP {
namespace bslfmt {

                  // =====================================
                  // struct FormatSpecificationParserEnums
                  // =====================================

/// Namespace struct holding enums used for parsing.
struct FormatSpecificationParserEnums {
  public:
    // TYPES
    enum ProcessingState {
        e_STATE_UNPARSED,
        e_STATE_PARSED,
        e_STATE_POSTPROCESSED
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
        e_SECTIONS_REMAINING_SPEC      = 128,
        e_SECTIONS_ALL                 = 256 - 1
    };
};

                   // ===============================
                   // class FormatSpecificationParser
                   // ===============================

/// A general mechanism used in format specification parsers.  It supports
/// parsing syntaxes of the form `fill-and-align?sign # 0 width?precision?L`
/// and any syntax that contains a subset of the sections therein.
template <class t_CHAR>
class FormatSpecificationParser
: public FormatSpecificationParserEnums {

  private:
    // CLASS DATA
    static BSLS_KEYWORD_CONSTEXPR_MEMBER size_t s_fillerBufferSize = 5;
        // maximum required buffer size to hold a null-terminated unicode code
        // point

    // DATA
    Sections                           d_sections;
                                           // list of sections extracted

    ProcessingState                    d_processingState;
                                           // current processing state

    t_CHAR                             d_filler[s_fillerBufferSize];
                                           // the filler code point

    int                                d_numFillerCharacters;
                                           // number of characters in filler

    int                                d_fillerDisplayWidth;
                                           // display width of code point

    Alignment                          d_alignment;
                                           // type of requested alignment

    Sign                               d_sign;
                                           // type of sign representation

    bool                               d_alternativeFlag;
                                           // flag indicating whether an
                                           // alternative form of value
                                           // representation is required (`#`)

    bool                               d_zeroPaddingFlag;
                                           // flag indicating whether padding
                                           // with zeros is required (`0`)

    FormatterSpecificationNumericValue d_rawWidth;
                                           // preliminary minimum field width

    FormatterSpecificationNumericValue d_rawPrecision;
                                           // preliminary precision

    FormatterSpecificationNumericValue d_postprocessedWidth;
                                           // final minimum field width
                                           // obtained after processing the
                                           // specification parameter

    FormatterSpecificationNumericValue d_postprocessedPrecision;
                                           // final precision obtained after
                                           // processing the specification
                                           // parameter

    bool                               d_localeSpecificFlag;
                                           // flag indicating whether the
                                           // locale-specific form is required
                                           // (`L`)

    bsl::basic_string_view<t_CHAR>     d_spec;
                                           // input format specification

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


    /// Based on the value of the initial byte of a unicode code point in UTF-8
    /// representation, calculate and return the number of bytes used in that
    /// representation.  The value of the first byte is specified by
    /// `firstChar`.  If `firstChar` does not contain a valid value for the
    /// first byte of a UTF-8 codepoint representation return -1;
    static BSLS_KEYWORD_CONSTEXPR_CPP20 int codepointBytesIfValid(
                                                         const char firstChar);

    /// Based on the value of the initial byte of a unicode code point in
    /// UTF-16 (where `sizeof(wchar_t)==2`) or UTF-32 (where
    /// `sizeof(wchar_t)==4`) representation, calculate and return the number
    /// of bytes used in that representation.  The value of the first byte is
    /// specified by `firstChar`.  If `firstChar` does not contain a valid
    /// value for the first byte of a UTF-16 or UTF-32 (depending on
    /// `sizeof(wchar_t)`) codepoint representation return -1;
    static BSLS_KEYWORD_CONSTEXPR_CPP20 int codepointBytesIfValid(
                                                      const wchar_t firstChar);

    // PRIVATE MANIPULATORS

    /// Extract a filler code point and `Alignment` value from start of the
    /// string specified by the `start` and `end` random-access-iterator range,
    /// and update the corresponding attributes of this object with the result.
    /// In the event of an error throw a `format_error` exception.
    template <class t_ITER>
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseFillAndAlignment(t_ITER *start,
                                                            t_ITER  end);

    /// If this object contains a filler code point, calculate the Unicode
    /// display width of that code point and update the corresponding attribute
    /// of this object with the result.  In the event of an error throw a
    /// `format_error` exception.
    void postprocessFiller();

    /// Extract a `Sign` value from the start of the string specified by the
    /// `start` and `end` random-access-iterator range, and update the
    /// corresponding attribute of this object with the result.  In the event
    /// of an error throw a `format_error` exception.
    template <class t_ITER>
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseSign(t_ITER *start, t_ITER end);

    /// Determine whether the start of the string specified by the `start` and
    /// `end` random-access-iterator range contains a flag (`#`) indicating
    /// alternative format processing and update the corresponding attribute of
    /// this object if necessary.
    template <class t_ITER>
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseAlternateOption(t_ITER *start,
                                                           t_ITER  end);

    /// Determine whether the start of the string specified by the `start` and
    /// `end` random-access-iterator range contains a flag (`0`) indicating
    /// zero padding and update the corresponding attribute of this object if
    /// necessary.
    template <class t_ITER>
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseZeroPaddingFlag(t_ITER *start,
                                                           t_ITER  end);

    /// Extract a width from the start of the string specified by the `start`
    /// and `end` random-access-iterator range, and update the corresponding
    /// attribute of this object with the result.  In the event of an error
    /// throw a `format_error` exception.  Note that the extracted precision
    /// may be hard-coded or nested and is not itself validated at this stage.
    template <class t_ITER>
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseRawWidth(t_ITER *start, t_ITER end);

    /// Extract a precision from the start of the string specified by the
    /// `start` and `end` random-access-iterator range, and update the
    /// corresponding attribute of this object with the result.  In the event
    /// of an error throw a `format_error` exception.  Note that the extracted
    /// precision may be hard-coded or nested and is not itself validated at
    /// this stage.
    template <class t_ITER>
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseRawPrecision(t_ITER *start,
                                                        t_ITER  end);

    /// Determine whether the start of the string specified by the `start` and
    /// `end` random-access-iterator range contains a flag (`L`) indicating
    /// locale specific formatting and update the corresponding attribute of
    /// this object with the result.
    template <class t_ITER>
    BSLS_KEYWORD_CONSTEXPR_CPP20 void parseLocaleSpecificFlag(t_ITER *start,
                                                              t_ITER  end);

    /// Parse the  string specified by the `start` and `end`
    /// random-access-iterator range to extract the items indicated by the
    /// specified `sections` and update the corresponding attributes of this
    /// object with the result.  In the event of an error throw a
    /// `format_error` exception.
    template <class t_ITER>
    BSLS_KEYWORD_CONSTEXPR_CPP20 void rawParse(t_ITER   *start,      // output
                                               t_ITER    end,        // input
                                               Sections  sections);  // param

  public:
    // CREATORS

    /// Create a default instance of `FormatSpecificationParser`.
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatSpecificationParser();

    // MANIPULATORS

    /// Parse the string contained by the specified input/output `parseContext`
    /// to extract the items indicated by the specified `sections`, update this
    /// object with the parsing result and update the iterator held by
    /// `parseContext` to point to the start of the unparsed section of the
    /// string.  In the event of an error throw a `format_error` exception.
    template <class t_PARSE_CONTEXT>
    BSLS_KEYWORD_CONSTEXPR_CPP20 void
    parse(t_PARSE_CONTEXT *parseContext,  // output
          Sections         sections);     // param

    /// Update any nested width and precision values to contain actual values
    /// based on the appropriate arguments held by the specified `context`
    /// format context.  In the event of an error throw a `format_error`
    /// exception.
    template <typename t_FORMAT_CONTEXT>
    void postprocess(const t_FORMAT_CONTEXT& context);

    // ACCESSORS

    /// Return the stored filler string.  If `parse` has not previously been
    /// called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const t_CHAR *filler() const;

    /// Return the size of the stored filler string.  If `parse` has not
    /// previously been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 int numFillerCharacters() const;

    /// Return the width of the filler code point.  If `postprocess` has not
    /// previously been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 int fillerCodePointDisplayWidth() const;

    /// Return the stored alignment value.  If `parse` has not
    /// previously been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 Alignment alignment() const;

    /// Return the stored sign value.  If `parse` has not previously been
    /// called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 Sign sign() const;

    /// Return true if parsing detected an alternative processing flag in the
    /// specification, otherwise return false.  If `parse` has not previously
    /// been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool alternativeFlag() const;

    /// Return true if parsing detected a zero padding flag in the
    /// specification, otherwise return false.  If `parse` has not previously
    /// been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool zeroPaddingFlag() const;

    /// Return the modified numeric width extracted during postprocessing.  If
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

    /// Return the current processing state of this parser.
    BSLS_KEYWORD_CONSTEXPR_CPP20 ProcessingState processingState() const;

    /// Return that section of the specification string that remains unparsed
    /// after the call to `parse`.  Note that this may be a truncated version
    /// of the string held by the parse context.  If `parse` has not previously
    /// been called throw a `format_error` exception.
    BSLS_KEYWORD_CONSTEXPR_CPP20 const bsl::basic_string_view<t_CHAR>
    remainingSpec() const;
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                     // -------------------------------
                     // class FormatSpecificationParser
                     // -------------------------------

// PRIVATE CLASS FUNCTIONS

template <class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR_CPP20
typename FormatSpecificationParser<t_CHAR>::Alignment
FormatSpecificationParser<t_CHAR>::alignmentFromChar(t_CHAR in)
{
    if ('<' == in) {
        return e_ALIGN_LEFT;                                          // RETURN
    }
    if ('>' == in) {
        return e_ALIGN_RIGHT;                                         // RETURN
    }
    if ('^' == in) {
        return e_ALIGN_MIDDLE;                                        // RETURN
    }
    return e_ALIGN_DEFAULT;
}

template <class t_CHAR>
inline BSLS_KEYWORD_CONSTEXPR_CPP20
typename FormatSpecificationParser<t_CHAR>::Sign
FormatSpecificationParser<t_CHAR>::signFromChar(t_CHAR in)
{
    if ('+' == in) {
        return e_SIGN_POSITIVE;                                       // RETURN
    }
    if ('-' == in) {
        return e_SIGN_NEGATIVE;                                       // RETURN
    }
    if (' ' == in) {
        return e_SIGN_SPACE;                                          // RETURN
    }
    return e_SIGN_DEFAULT;
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20 int
FormatSpecificationParser<t_CHAR>::codepointBytesIfValid(const char firstChar)
{
    const unsigned char unsignedValue = static_cast<unsigned char>(firstChar);
    if ((unsignedValue & 0x80) == 0x00) {
        return 1;
    }
    else if ((unsignedValue & 0xe0) == 0xc0) {
        return 2;
    }
    else if ((unsignedValue & 0xf0) == 0xe0) {
        return 3;
    }
    else if ((unsignedValue & 0xf8) == 0xf0) {
        return 4;
    }
    else {
        return -1;
    }
}

template <class t_CHAR>
inline
BSLS_KEYWORD_CONSTEXPR_CPP20 int
FormatSpecificationParser<t_CHAR>::codepointBytesIfValid(
                                                       const wchar_t firstChar)
{
    switch (sizeof(wchar_t)) {
      case 2: {  // UTF-16
        if (static_cast<unsigned int>(firstChar) <
            static_cast<unsigned int>(0xd800)) {
            return 2;
        }
        else if (static_cast<unsigned int>(firstChar) <
                 static_cast<unsigned int>(0xdc00)) {
            return 4;
        }
        else {
            return -1;
        }
      } break;
      case 4: {  // UTF-32
        if (static_cast<unsigned long>(firstChar) <
            static_cast<unsigned long>(0x80000000U)) {
            return 4;
        }
        else {
            return -1;
        }
      } break;
      default: {
        return -1;  // unsuported wchar_t size.
      } break;
    }
}

// PRIVATE MANIPULATORS
template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
FormatSpecificationParser<t_CHAR>::parseFillAndAlignment(t_ITER *start,
                                                         t_ITER  end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}') {
        d_filler[0]           = ' ';
        d_numFillerCharacters = 1;
        d_fillerDisplayWidth  = 1;
        return;                                                       // RETURN
    }

    const int cpBytes = codepointBytesIfValid(**start);

    if (cpBytes < 0) {
        BSLS_THROW(
           bsl::format_error("Invalid unicode code point (`parse`"));  // THROW
    }

    const int cpChars = cpBytes / sizeof(t_CHAR);

    t_ITER tempIter = *start;
    for (int i = 0; i < cpChars; ++i) {
        if (tempIter == end) {
            BSLS_THROW(
            bsl::format_error("Unicode code point: too few bytes"));   // THROW
        }
        d_filler[i] = *tempIter;
        ++tempIter;
    }
    d_filler[cpChars] = 0;

    d_numFillerCharacters = cpChars;

    t_ITER aligner = tempIter;

    if (aligner != end) {
        if (alignmentFromChar(*aligner) != e_ALIGN_DEFAULT) {
            // `{` and `}` are invalid fill characters per the C++ spec.  As
            // they are ASCII we can check **start without decoding the code
            // point.
            if ('{' == **start || '}' == **start) {
                BSLS_THROW(bsl::format_error(
                            "Invalid fill character ('{' or '}')"));   // THROW
            }
            d_alignment = alignmentFromChar(*aligner);

            // We cannot know this yet - put in a placeholder.
            d_fillerDisplayWidth = -1;

            *start = aligner + 1;
            // Fill and alignment specifier given.
            return;                                                   // RETURN
        }
    }

    // If the alignment specifier is not in 2nd position, it is allowed to be
    // in the first position, in which case the spec states that a space should
    // be used as the filler character.
    d_filler[0]           = ' ';
    d_numFillerCharacters = 1;
    d_fillerDisplayWidth  = 1;
    aligner                        = *start;
    if (alignmentFromChar(*aligner) != e_ALIGN_DEFAULT) {
        d_alignment = alignmentFromChar(*aligner);
        *start      = aligner + 1;
        // Alignment specifier with default (ascii space) fill character
        return;                                                       // RETURN
    }

    // Otherwise we have neither fill nor alignment.
    return;                                                           // RETURN
}

template <class t_CHAR>
void
FormatSpecificationParser<t_CHAR>::postprocessFiller()
{

    UnicodeCodePoint cp;

    switch (sizeof(t_CHAR)) {
      case 1: {
        cp.extract(UnicodeCodePoint::e_UTF8,
                   (const void *)d_filler,
                   d_numFillerCharacters);
      } break;
      case 2: {
        cp.extract(UnicodeCodePoint::e_UTF16,
                   (const void *)d_filler,
                   d_numFillerCharacters * 2);
      } break;
      case 4: {
        cp.extract(UnicodeCodePoint::e_UTF32,
                   (const void *)d_filler,
                   d_numFillerCharacters * 4);
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Unsupported wchar_t size"));     // THROW
      }
    }

    if (!cp.isValid()) {
        BSLS_THROW(bsl::format_error(
                      "Invalid unicode code point (`postprocess`)"));  // THROW
    }

    if (static_cast<size_t>(cp.numSourceBytes()) != d_numFillerCharacters *
                                                        sizeof(t_CHAR)) {
        BSLS_THROW(
               bsl::format_error("Invalid unicode code point size"));  // THROW
    }

    // `{` and `}` are invalid fill characters per the C++ spec
    if ('{' == cp.codePointValue() || '}' == cp.codePointValue()) {
        BSLS_THROW(
          bsl::format_error("Invalid fill character ('{' or '}')"));   // THROW
    }

    if (d_fillerDisplayWidth >= 0) {
        if (d_fillerDisplayWidth != cp.codePointWidth()) {
            BSLS_THROW(
                     bsl::format_error("Invalid code point width"));   // THROW
        }
    }
    else {
        d_fillerDisplayWidth = cp.codePointWidth();
    }
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
FormatSpecificationParser<t_CHAR>::parseSign(t_ITER *start, t_ITER end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}') {
        return;                                                       // RETURN
    }

    d_sign = signFromChar(**start);

    if (e_SIGN_DEFAULT != d_sign) {
        ++*start;
    }
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
FormatSpecificationParser<t_CHAR>::parseAlternateOption(t_ITER *start,
                                                        t_ITER  end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}') {
        return;                                                       // RETURN
    }

    d_alternativeFlag = ((t_CHAR)'#' == **start);

    if (d_alternativeFlag) {
        ++*start;
    }
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
FormatSpecificationParser<t_CHAR>::parseZeroPaddingFlag(t_ITER *start,
                                                        t_ITER  end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}')  {
        return;                                                       // RETURN
    }

    d_zeroPaddingFlag = ((t_CHAR)'0' == **start);

    if (d_zeroPaddingFlag) {
        ++*start;
    }
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
FormatSpecificationParser<t_CHAR>::parseRawWidth(t_ITER *start, t_ITER end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}')  {
        return;                                                       // RETURN
    }

    typedef FormatterSpecificationNumericValue NumericValue;

    d_rawWidth.parse(start, end, false);
    // Non-relative widths must be strictly positive.
    if (d_rawWidth == NumericValue(NumericValue::e_VALUE, 0)) {
        BSLS_THROW(bsl::format_error("Field widths must be > 0"));     // THROW
    }
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
FormatSpecificationParser<t_CHAR>::parseRawPrecision(t_ITER *start, t_ITER end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}') {
        return;                                                       // RETURN
    }

    return d_rawPrecision.parse(start, end, true);
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20 void
FormatSpecificationParser<t_CHAR>::parseLocaleSpecificFlag(t_ITER *start,
                                                           t_ITER  end)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}') {
        return;                                                       // RETURN
    }

    d_localeSpecificFlag = ((t_CHAR)'L' == **start);

    if (d_localeSpecificFlag) {
        ++*start;
    }

    return;
}

template <class t_CHAR>
template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20 void FormatSpecificationParser<t_CHAR>::rawParse(
                                                            t_ITER   *start,
                                                            t_ITER    end,
                                                            Sections  sections)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    d_sections = sections;

    // Reasonable defaults for early exit.
    d_filler[0]           = ' ';
    d_numFillerCharacters = 1;
    d_fillerDisplayWidth  = 1;

    // Handle empty string or empty specification.
    if (*start == end || **start == '}') {
        return;                                                       // RETURN
    }

    if (0 != (sections & e_SECTIONS_FILL_ALIGN)) {
        parseFillAndAlignment(start, end);

        if (*start == end || **start == '}') {
            return;                                                   // RETURN
        }
    }

    if (0 != (sections & e_SECTIONS_SIGN_FLAG)) {
        parseSign(start, end);

        if (*start == end || **start == '}') {
            return;                                                   // RETURN
        }
    }

    if (0 != (sections & e_SECTIONS_ALTERNATE_FLAG)) {
        parseAlternateOption(start, end);

        if (*start == end || **start == '}') {
            return;                                                   // RETURN
        }
    }

    if (0 != (sections & e_SECTIONS_ZERO_PAD_FLAG)) {
        parseZeroPaddingFlag(start, end);

        if (*start == end || **start == '}') {
            return;                                                   // RETURN
        }
    }

    if (0 != (sections & e_SECTIONS_WIDTH)) {
        parseRawWidth(start, end);

        if (*start == end || **start == '}') {
            return;                                                   // RETURN
        }
    }

    if (0 != (sections & e_SECTIONS_PRECISION)) {
        parseRawPrecision(start, end);

        if (*start == end || **start == '}') {
            return;                                                   // RETURN
        }
    }

    if (0 != (sections & e_SECTIONS_LOCALE_FLAG)) {
        parseLocaleSpecificFlag(start, end);

        if (*start == end || **start == '}') {
            return;                                                   // RETURN
        }
    }

    if (0 != (sections & e_SECTIONS_REMAINING_SPEC)) {
        bsl::basic_string_view<t_CHAR>                          temp(*start,
                                                                     end);
        size_t counter = 0;

        // Take anything left up to but not including last closing brace.
        int level = 1;
        while (temp.size() > counter) {
            if ('{' == temp[counter]) {
                ++level;
            }
            else if ('}' == temp[counter]) {
                --level;
            }
            if (0 == level) {
                break;                                                 // BREAK
            }
            ++counter;
        }

        d_spec = temp.substr(0, counter);
        *start += counter;
    }

    // We cannot mix specified and unspecified relative argument ids.

    if (d_rawWidth.category() ==
            FormatterSpecificationNumericValue::e_NEXT_ARG &&
        d_rawPrecision.category() ==
            FormatterSpecificationNumericValue::e_ARG_ID) {
        BSLS_THROW(bsl::format_error("Cannot mix automatic (width) and manual "
                                     "(precision) indexing"));         // THROW
    }

    if (d_rawWidth.category() ==
            FormatterSpecificationNumericValue::e_ARG_ID &&
        d_rawPrecision.category() ==
            FormatterSpecificationNumericValue::e_NEXT_ARG) {
        BSLS_THROW(bsl::format_error("Cannot mix manual (width) and automatic "
                                     "(precision) indexing"));         // THROW
    }

    if (*start == end || **start == '}') {
        return;                                                       // RETURN
    }

     BSLS_THROW(bsl::format_error(
                "Specification parse failure (invalid character)"));   // THROW
}

// CREATORS
template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatSpecificationParser<t_CHAR>::FormatSpecificationParser()
: d_sections(e_SECTIONS_NONE)
, d_processingState(e_STATE_UNPARSED)
, d_filler()
, d_numFillerCharacters(1)
, d_fillerDisplayWidth(1)
, d_alignment(e_ALIGN_DEFAULT)
, d_sign(e_SIGN_DEFAULT)
, d_alternativeFlag(false)
, d_zeroPaddingFlag(false)
, d_localeSpecificFlag(false)
{}

// MANIPULATORS
template <class t_CHAR>
template <class t_PARSE_CONTEXT>
BSLS_KEYWORD_CONSTEXPR_CPP20 void FormatSpecificationParser<t_CHAR>::parse(
                                                 t_PARSE_CONTEXT *parseContext,
                                                 Sections         sections)
{
    BSLMF_ASSERT((
        bsl::is_same<typename bsl::iterator_traits<
                         typename t_PARSE_CONTEXT::const_iterator>::value_type,
                     t_CHAR>::value));

    d_processingState = e_STATE_PARSED;

    typename t_PARSE_CONTEXT::const_iterator current = parseContext->begin();
    typename t_PARSE_CONTEXT::const_iterator end     = parseContext->end();

    rawParse(&current, end, sections);

    if (0 != (sections & e_SECTIONS_WIDTH)) {
        if (rawWidth().category() ==
            FormatterSpecificationNumericValue::e_ARG_ID) {
            parseContext->check_arg_id(rawWidth().value());
        }
        else if (rawWidth().category() ==
                 FormatterSpecificationNumericValue::e_NEXT_ARG) {
            d_rawWidth = FormatterSpecificationNumericValue(
                                FormatterSpecificationNumericValue::e_ARG_ID,
                                static_cast<int>(parseContext->next_arg_id()));
        }
    }

    if (0 != (sections & e_SECTIONS_PRECISION)) {
        if (rawPrecision().category() ==
            FormatterSpecificationNumericValue::e_ARG_ID) {
            parseContext->check_arg_id(rawPrecision().value());
        }
        else if (rawPrecision().category() ==
                 FormatterSpecificationNumericValue::e_NEXT_ARG) {
            d_rawPrecision = FormatterSpecificationNumericValue(
                                FormatterSpecificationNumericValue::e_ARG_ID,
                                static_cast<int>(parseContext->next_arg_id()));
        }
    }

    parseContext->advance_to(current);
}

template <class t_CHAR>
template <typename t_FORMAT_CONTEXT>
void FormatSpecificationParser<t_CHAR>::postprocess(
                                               const t_FORMAT_CONTEXT& context)
{
    if (0 != (d_sections & e_SECTIONS_FILL_ALIGN)) {
        FormatSpecificationParser::postprocessFiller();
    }

    if (0 != (d_sections & e_SECTIONS_WIDTH)) {
        d_postprocessedWidth = d_rawWidth;
       d_postprocessedWidth.postprocess(context);
    }

    if (0 != (d_sections & e_SECTIONS_PRECISION)) {
        d_postprocessedPrecision = d_rawPrecision;
        d_postprocessedPrecision.postprocess(context);
    }

    d_processingState = e_STATE_POSTPROCESSED;
}

// ACCESSORS

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const t_CHAR *FormatSpecificationParser<t_CHAR>::filler() const
{
    if (e_STATE_UNPARSED == d_processingState) {
        BSLS_THROW(bsl::format_error(
            "Format specification '.parse' not called (`filler`)"));   // THROW
    }

    return d_filler;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int FormatSpecificationParser<t_CHAR>::numFillerCharacters() const
{
    if (e_STATE_UNPARSED == d_processingState) {
        BSLS_THROW(
                  bsl::format_error("Format specification '.parse' not called "
                                    "(`numFillerCharacters`)"));       // THROW
    }

    return d_numFillerCharacters;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int FormatSpecificationParser<t_CHAR>::fillerCodePointDisplayWidth() const
{
    if (e_STATE_POSTPROCESSED != d_processingState) {
        BSLS_THROW(
            bsl::format_error("Format specification '.postprocess' not called "
                              "(`fillerCodePointDisplayWidth`)"));     // THROW
    }

    return d_fillerDisplayWidth;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename
FormatSpecificationParser<t_CHAR>::Alignment
FormatSpecificationParser<t_CHAR>::alignment() const
{
    if (e_STATE_UNPARSED == d_processingState) {
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`alignment`)"));         // THROW
    }

    return d_alignment;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename
FormatSpecificationParser<t_CHAR>::Sign
FormatSpecificationParser<t_CHAR>::sign() const
{
    if (e_STATE_UNPARSED == d_processingState) {
        BSLS_THROW(bsl::format_error(
              "Format specification '.parse' not called (`sign`)"));   // THROW
    }

    return d_sign;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatSpecificationParser<t_CHAR>::alternativeFlag() const
{
    if (e_STATE_UNPARSED == d_processingState) {
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`alternativeFlag`)"));   // THROW
    }

    return d_alternativeFlag;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatSpecificationParser<t_CHAR>::zeroPaddingFlag() const
{
    if (e_STATE_UNPARSED == d_processingState) {
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`zeroPaddingFlag`)"));   // THROW
    }

    return d_zeroPaddingFlag;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
FormatSpecificationParser<t_CHAR>::postprocessedWidth() const
{
    if (e_STATE_POSTPROCESSED != d_processingState) {
        BSLS_THROW(
            bsl::format_error("Format specification '.postprocess' not called "
                              "(`postprocessedWidth`)"));              // THROW
    }

    return d_postprocessedWidth;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
FormatSpecificationParser<t_CHAR>::postprocessedPrecision() const
{
    if (e_STATE_POSTPROCESSED != d_processingState) {
        BSLS_THROW(
            bsl::format_error("Format specification '.postprocess' not called "
                              "(`postprocessedPrecision`)"));          // THROW
    }

    return d_postprocessedPrecision;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
FormatSpecificationParser<t_CHAR>::rawWidth() const
{
    if (e_STATE_UNPARSED == d_processingState) {
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`rawWidth`)"));          // THROW
    }

    return d_rawWidth;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
FormatSpecificationParser<t_CHAR>::rawPrecision() const
{
    if (e_STATE_UNPARSED == d_processingState) {
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
        "called (`rawPrecision`)"));                                   // THROW
    }

    return d_rawPrecision;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatSpecificationParser<t_CHAR>::localeSpecificFlag() const
{
    if (e_STATE_UNPARSED == d_processingState) {
        BSLS_THROW(
                  bsl::format_error("Format specification '.parse' not called "
                                    "(`localeSpecificFlag`)"));        // THROW
    }

    return d_localeSpecificFlag;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatSpecificationParserEnums::ProcessingState
FormatSpecificationParser<t_CHAR>::processingState() const
{
    return d_processingState;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const bsl::basic_string_view<t_CHAR>
FormatSpecificationParser<t_CHAR>::remainingSpec() const
{
    if (e_STATE_UNPARSED == d_processingState) {
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`remainingSpec`)"));     // THROW
    }

    return d_spec;
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATSPECIFICATIONPARSER

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
