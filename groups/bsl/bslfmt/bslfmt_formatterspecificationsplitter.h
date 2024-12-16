// bslfmt_formatterspecificationsplitter.h                            -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERSPECIFICATIONSPLITTER
#define INCLUDED_BSLFMT_FORMATTERSPECIFICATIONSPLITTER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Private utility for use within BSL `format` spec parsers
//
//@CLASSES:
//  FormatSpecification_Splitter: Utility to tokenize format specifications.
//  FormatSpecification_SplitterBase: Base for FormatSpecification_Splitter.
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
#include <bslstl_monostate.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterunicodeutils.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'

#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
#include <format>     // for 'std::visit_format_arg'
#endif


namespace BloombergLP {
namespace bslfmt {

                   // -----------------------------------------
                   // struct FormatterSpecificationNumericValue
                   // -----------------------------------------

struct FormatterSpecificationNumericValue {
  public:
    // CLASS TYPES
    enum ValueType { e_DEFAULT, e_VALUE, e_NEXT_ARG, e_ARG_ID };

  private:
    // DATA
    int       d_value;
    ValueType d_type;

    // PRIVATE CLASS FUNCTIONS
    template <class t_ITER>
    static void BSLS_KEYWORD_CONSTEXPR_CPP20 parse(
                            FormatterSpecificationNumericValue *outValue,
                            t_ITER                             *start,
                            t_ITER                              end,
                            bool                                hasInitialDot);

    template <typename t_FORMAT_CONTEXT>
    static void finalize(FormatterSpecificationNumericValue *out,
                         const t_FORMAT_CONTEXT&             context);

    // FRIENDS
    template <class t_CHAR>
    friend class FormatterSpecificationSplitter;

    friend struct FormatterSpecificationNumericValue_Visitor;

  public:
    // CREATORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationNumericValue();
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationNumericValue(
                                                               int       value,
                                                               ValueType type);

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR_CPP20
    bool operator==(const FormatterSpecificationNumericValue& other) const;

    BSLS_KEYWORD_CONSTEXPR_CPP20 int       value() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 ValueType valueType() const;
};

                 // -------------------------------------------------
                 // struct FormatterSpecificationNumericValue_Visitor
                 // -------------------------------------------------


struct FormatterSpecificationNumericValue_Visitor {
  private:
    // DATA
    FormatterSpecificationNumericValue *d_value_p;

  public:
    // CREATORS

    FormatterSpecificationNumericValue_Visitor(
                                      FormatterSpecificationNumericValue *pc);

    // MANIPULATORS

    void operator()(bsl::monostate) const;

    template <class t_TYPE>
    typename bsl::enable_if<bsl::is_integral<t_TYPE>::value>::type operator()(
                                                               t_TYPE x) const;

    template <class t_TYPE>
    typename bsl::enable_if<!bsl::is_integral<t_TYPE>::value>::type operator()(
                                                               t_TYPE x) const;
};

              // ------------------------------------------
              // class FormatterSpecificationSplitter_Enums
              // ------------------------------------------

struct FormatterSpecificationSplitter_Enums {
  public:
    // CLASS TYPES
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

template <class t_CHAR>
class FormatterSpecificationSplitter
: public FormatterSpecificationSplitter_Enums {

  private:
    // PRIVATE CLASS TYPES
    enum { k_FILLER_BUFFER_SIZE = 5 };

    // DATA
    Sections      d_sections;
    ParsingStatus d_parsingStatus;

    t_CHAR d_filler[k_FILLER_BUFFER_SIZE];  // one filler code point
    int    d_fillerCharacters;
    int    d_fillerCodePointDisplayWidth;

    Alignment                           d_alignment;
    Sign                                d_sign;
    bool                                d_alternativeFlag;
    bool                                d_zeroPaddingFlag;
    FormatterSpecificationNumericValue  d_rawWidth;
    FormatterSpecificationNumericValue  d_rawPrecision;
    FormatterSpecificationNumericValue  d_postprocessedWidth;
    FormatterSpecificationNumericValue  d_postprocessedPrecision;
    bool                                d_localeSpecificFlag;
    bsl::basic_string_view<t_CHAR>      d_spec;

    // PRIVATE CLASS FUNCTIONS
    static BSLS_KEYWORD_CONSTEXPR_CPP20 Alignment alignmentFromChar(t_CHAR in);

    static BSLS_KEYWORD_CONSTEXPR_CPP20 Sign signFromChar(t_CHAR in);

    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseFillAndAlignment(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    static void postprocessFiller(FormatterSpecificationSplitter *outSpec);

    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseSign(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseAlternateOption(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseZeroPaddingFlag(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseRawWidth(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseRawPrecision(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void parseLocaleSpecificFlag(
                                       FormatterSpecificationSplitter *outSpec,
                                       t_ITER                         *start,
                                       t_ITER                          end);

    template <class t_ITER>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void
    rawParse(FormatterSpecificationSplitter *outSpec,  // output
             t_ITER                         *start,    // output
             t_ITER                          end,      // input
             Sections                        sections);// param

  public:

    // CLASS METHODS
    template <class t_PARSE_CONTEXT>
    static BSLS_KEYWORD_CONSTEXPR_CPP20 void
    parse(FormatterSpecificationSplitter *outSpec,  // output
          t_PARSE_CONTEXT                *pc,       // output
          Sections                        sections);// param

    template <typename t_FORMAT_CONTEXT>
    static void postprocess(FormatterSpecificationSplitter *outSpec,
                            const t_FORMAT_CONTEXT&         context);

    // CREATORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 FormatterSpecificationSplitter();

    // ACCESSORS
    BSLS_KEYWORD_CONSTEXPR_CPP20 const t_CHAR *filler() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 int           fillerCharacters() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 int       fillerCodePointDisplayWidth() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 Alignment alignment() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 Sign      sign() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool      alternativeFlag() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool      zeroPaddingFlag() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
    postprocessedWidth() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
    postprocessedPrecision() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
    rawWidth() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
                                      rawPrecision() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 bool localeSpecificFlag() const;
    BSLS_KEYWORD_CONSTEXPR_CPP20 const bsl::basic_string_view<t_CHAR>
    finalSpec() const;
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                 // -----------------------------------------
                 // struct FormatterSpecificationNumericValue
                 // -----------------------------------------

// CREATORS

inline
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatterSpecificationNumericValue::FormatterSpecificationNumericValue()
: d_value(0), d_type(e_DEFAULT)
{
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatterSpecificationNumericValue::FormatterSpecificationNumericValue(
                                                               int       value,
                                                               ValueType type)
: d_value(value)
, d_type(type)
{
}

// ACCESSORS

inline
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationNumericValue::operator==(
                        const FormatterSpecificationNumericValue& other) const
{
    return d_value == other.d_value && d_type == other.d_type;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20
int FormatterSpecificationNumericValue::value() const
{
    return d_value;
}

inline
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatterSpecificationNumericValue::ValueType
FormatterSpecificationNumericValue::valueType() const
{
    return d_type;
}

// CLASS METHODS

template <class t_ITER>
BSLS_KEYWORD_CONSTEXPR_CPP20
void FormatterSpecificationNumericValue::parse(
                           FormatterSpecificationNumericValue *outValue,
                           t_ITER                             *start,
                           t_ITER                              end,
                           bool                                needInitialDot)
{
    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return;                                                       // RETURN

    t_ITER current = *start;

    if (needInitialDot) {
        // No dot therefore no precision - early successful exit.
        if (*current != '.')
            return;                                                   // RETURN
        ++current;
        // A dot but nothing afterwards is an invalid precision string.
        if (current == end)
            BSLS_THROW(bsl::format_error(
                          "Invalid Precision (nothing after '.')"));  // RETURN
    }

    bool isArgId = false;

    if (*current == '{') {
        current++;
            // Missing matching closing brace.
        if (current == end)
            BSLS_THROW(
                    bsl::format_error("Nested arg id missing '}'"));  // RETURN
        // Early exit for a non-numbered replacement field.
        if (*current == '}') {
            outValue->d_type  = e_NEXT_ARG;
            outValue->d_value = 0;
            *start            = current + 1;
            return;                                                   // RETURN
        }
        isArgId = true;
    }

    int digitCount = 0;
    int value      = 0;

    while (*current >= '0' && *current <= '9') {
        value = (value * 10) + static_cast<int>(*current - '0');
        ++digitCount;
        ++current;
    }

    // No digits
    if (digitCount == 0) {
        // If we have either specified the "precision dot" or if we know we
        // have a numbered replacement field then digits are non-optional.
        if (needInitialDot) {
            if (current == end)
                BSLS_THROW(bsl::format_error(
                       "Invalid Precision (non-digits after '.')"));  // RETURN
        }
        if (isArgId) {
            if (current == end)
                BSLS_THROW(
                    bsl::format_error("Nested arg id non-numeric"));  // RETURN
        }
        outValue->d_value = 0;
        outValue->d_type  = e_DEFAULT;
    }
    else {
        // We know buffer holds only digits, so it is safe to call atoi. As we
        // do not allow + or - the value must be non-negative.
        outValue->d_value = value;

        if (isArgId) {
            // Relative argument references must have a closing brace.
            if (current == end || *current != '}')
                BSLS_THROW(
                    bsl::format_error("Nested arg id missing '}'"));  // RETURN
            ++current;
        }

        outValue->d_type = isArgId ? e_ARG_ID : e_VALUE;
    }

    *start = current;

    return;
}

template <typename t_FORMAT_CONTEXT>
inline
void FormatterSpecificationNumericValue::finalize(
                                 FormatterSpecificationNumericValue *out,
                                 const t_FORMAT_CONTEXT&             context)
{
    if (out->d_type != FormatterSpecificationNumericValue::e_ARG_ID)
        return;                                                       // RETURN

    FormatterSpecificationNumericValue_Visitor visitor(out);
    {
#if defined(BSLS_LIBRARYFEATURES_HAS_CPP20_FORMAT)
        // Depending on the type of t_FORMAT_CONTEXT we may need to use
        // `visit_format_arg` from `bslfmt` or from `std`.
        using namespace std;
#endif
        visit_format_arg(visitor, context.arg(out->d_value));
    }
}

              // -------------------------------------------------
              // struct FormatterSpecificationNumericValue_Visitor
              // -------------------------------------------------

inline
FormatterSpecificationNumericValue_Visitor::
    FormatterSpecificationNumericValue_Visitor(
                                       FormatterSpecificationNumericValue *pc)
: d_value_p(pc)
{
}

inline void
FormatterSpecificationNumericValue_Visitor::operator()(bsl::monostate) const
{
    BSLS_THROW(bsl::format_error("Nested argument id out of range"));
}

template <class t_TYPE>
typename bsl::enable_if<bsl::is_integral<t_TYPE>::value>::type
FormatterSpecificationNumericValue_Visitor::operator()(t_TYPE x) const
{
    d_value_p->d_value = static_cast<int>(x);
    d_value_p->d_type  = FormatterSpecificationNumericValue::e_VALUE;
}

template <class t_TYPE>
typename bsl::enable_if<!bsl::is_integral<t_TYPE>::value>::type
FormatterSpecificationNumericValue_Visitor::operator()(t_TYPE) const
{
    BSLS_THROW(bsl::format_error("Nested value args must be integral"));
}


               // --------------------------------------------
               // class FormatterSpecificationSplitter<t_CHAR>
               // --------------------------------------------

// CREATORS

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
FormatterSpecificationSplitter<t_CHAR>::FormatterSpecificationSplitter()
: d_sections(e_SECTIONS_NONE)
, d_parsingStatus(e_PARSING_UNINITIALIZED)
, d_filler(), d_fillerCharacters(1)
, d_fillerCodePointDisplayWidth(1)
, d_alignment(FormatterSpecificationSplitter::e_ALIGN_DEFAULT)
, d_sign(FormatterSpecificationSplitter::e_SIGN_DEFAULT)
, d_alternativeFlag(false)
, d_zeroPaddingFlag(false)
, d_localeSpecificFlag(false)
{
    //memset(d_filler, 0, k_FILLER_BUFFER_SIZE * sizeof(t_CHAR));
}

// ACCESSORS

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
const t_CHAR *FormatterSpecificationSplitter<t_CHAR>::filler() const
{
    if (e_PARSING_UNINITIALIZED == d_parsingStatus)
        BSLS_THROW(bsl::format_error(
            "Format specification '.parse' not called (`filler`)"));  // RETURN

    return d_filler;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
int FormatterSpecificationSplitter<t_CHAR>::fillerCharacters() const
{
    if (e_PARSING_UNINITIALIZED == d_parsingStatus)
        BSLS_THROW(
                  bsl::format_error("Format specification '.parse' not called "
                                    "(`fillerCharacters`)"));         // RETURN

    return d_fillerCharacters;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 int
FormatterSpecificationSplitter<t_CHAR>::fillerCodePointDisplayWidth() const
{
    if (e_PARSING_POSTPROCESSED != d_parsingStatus)
        BSLS_THROW(
            bsl::format_error("Format specification '.postprocess' not called "
                              "(`fillerCodePointDisplayWidth`)"));    // RETURN

    return d_fillerCodePointDisplayWidth;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
typename
FormatterSpecificationSplitter<t_CHAR>::Alignment
FormatterSpecificationSplitter<t_CHAR>::alignment() const
{
    if (e_PARSING_UNINITIALIZED == d_parsingStatus)
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
    if (e_PARSING_UNINITIALIZED == d_parsingStatus)
        BSLS_THROW(bsl::format_error(
              "Format specification '.parse' not called (`sign`)"));  // RETURN

    return d_sign;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationSplitter<t_CHAR>::alternativeFlag() const
{
    if (e_PARSING_UNINITIALIZED == d_parsingStatus)
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`alternativeFlag`)"));  // RETURN

    return d_alternativeFlag;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationSplitter<t_CHAR>::zeroPaddingFlag() const
{
    if (e_PARSING_UNINITIALIZED == d_parsingStatus)
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`zeroPaddingFlag`)"));  // RETURN

    return d_zeroPaddingFlag;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
FormatterSpecificationSplitter<t_CHAR>::postprocessedWidth() const
{
    if (e_PARSING_POSTPROCESSED != d_parsingStatus)
        BSLS_THROW(
            bsl::format_error("Format specification '.postprocess' not called "
                              "(`postprocessedWidth`)"));             // RETURN

    return d_postprocessedWidth;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
FormatterSpecificationSplitter<t_CHAR>::rawWidth() const
{
    if (e_PARSING_UNINITIALIZED == d_parsingStatus)
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`rawWidth`)"));         // RETURN

    return d_rawWidth;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
FormatterSpecificationSplitter<t_CHAR>::postprocessedPrecision() const
{
    if (e_PARSING_POSTPROCESSED != d_parsingStatus)
        BSLS_THROW(
            bsl::format_error("Format specification '.postprocess' not called "
                              "(`postprocessedPrecision`)"));         // RETURN

    return d_postprocessedPrecision;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20 const FormatterSpecificationNumericValue
FormatterSpecificationSplitter<t_CHAR>::rawPrecision() const
{
    if (e_PARSING_UNINITIALIZED == d_parsingStatus)
        BSLS_THROW(bsl::format_error("Format specification '.parse' not "
                                     "called (`rawPrecision`)"));     // RETURN

    return d_rawPrecision;
}

template <class t_CHAR>
BSLS_KEYWORD_CONSTEXPR_CPP20
bool FormatterSpecificationSplitter<t_CHAR>::localeSpecificFlag() const
{
    if (e_PARSING_UNINITIALIZED == d_parsingStatus)
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
    if (e_PARSING_UNINITIALIZED == d_parsingStatus)
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
                                     t_PARSE_CONTEXT                *pc,
                                     Sections                        sections)
{
    BSLMF_ASSERT((
        bsl::is_same<typename bsl::iterator_traits<
                         typename t_PARSE_CONTEXT::const_iterator>::value_type,
                     t_CHAR>::value));

    outSpec->d_parsingStatus = e_PARSING_PARSED;

    typename t_PARSE_CONTEXT::const_iterator current = pc->begin();
    typename t_PARSE_CONTEXT::const_iterator end     = pc->end();

    rawParse(outSpec, &current, end, sections);

    if (0 != (sections & e_SECTIONS_WIDTH)) {
        if (outSpec->rawWidth().valueType() ==
            FormatterSpecificationNumericValue::e_ARG_ID) {
            pc->check_arg_id(outSpec->rawWidth().value());
        }
        else if (outSpec->rawWidth().valueType() ==
                 FormatterSpecificationNumericValue::e_NEXT_ARG) {
            outSpec->d_rawWidth = FormatterSpecificationNumericValue(
                                static_cast<int>(pc->next_arg_id()),
                                FormatterSpecificationNumericValue::e_ARG_ID);
        }
    }

    if (0 != (sections & e_SECTIONS_PRECISION)) {
        if (outSpec->rawPrecision().valueType() ==
            FormatterSpecificationNumericValue::e_ARG_ID) {
            pc->check_arg_id(outSpec->rawPrecision().value());
        }
        else if (outSpec->rawPrecision().valueType() ==
                 FormatterSpecificationNumericValue::e_NEXT_ARG) {
            outSpec->d_rawPrecision = FormatterSpecificationNumericValue(
                                static_cast<int>(pc->next_arg_id()),
                                FormatterSpecificationNumericValue::e_ARG_ID);
        }
    }

    if (current == end || *current == '}') {
        pc->advance_to(current);
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
    outSpec->d_filler[0]                   = ' ';
    outSpec->d_fillerCharacters            = 1;
    outSpec->d_fillerCodePointDisplayWidth = 1;

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

    if (outSpec->d_rawWidth.valueType() ==
            FormatterSpecificationNumericValue::e_NEXT_ARG &&
        outSpec->d_rawPrecision.valueType() ==
            FormatterSpecificationNumericValue::e_ARG_ID) {
        BSLS_THROW(bsl::format_error(
                       "Cannot mix manual and automatic indexing"));  // RETURN
    }

    if (outSpec->d_rawWidth.valueType() ==
            FormatterSpecificationNumericValue::e_ARG_ID &&
        outSpec->d_rawPrecision.valueType() ==
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
        outSpec->d_filler[0]                   = ' ';
        outSpec->d_fillerCharacters            = 1;
        outSpec->d_fillerCodePointDisplayWidth = 1;
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

    outSpec->d_fillerCharacters = cpChars;

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
            outSpec->d_fillerCodePointDisplayWidth = -1;

            *start = aligner + 1;
            // Fill and alignment specifier given.
            return;                                                   // RETURN
        }
    }

    // If the alignment specifier is not in 2nd position, it is allowed to be
    // in the first position, in which case the spec states that a space should
    // be used as the filler character.
    outSpec->d_filler[0]                   = ' ';
    outSpec->d_fillerCharacters            = 1;
    outSpec->d_fillerCodePointDisplayWidth = 1;
    aligner                                = *start;
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
                                                outSpec->d_fillerCharacters);
      } break;
      case 2: {
        cp = Formatter_UnicodeUtils::extractCodePoint(
                                               Formatter_UnicodeUtils::e_UTF16,
                                               (const void *)outSpec->d_filler,
                                               outSpec->d_fillerCharacters * 2);
      } break;
      case 4: {
        cp = Formatter_UnicodeUtils::extractCodePoint(
                                               Formatter_UnicodeUtils::e_UTF32,
                                               (const void *)outSpec->d_filler,
                                               outSpec->d_fillerCharacters * 4);
      } break;
      default: {
        BSLS_THROW(bsl::format_error("Unsupported wchar_t size"));    // RETURN
      }
    }

    if (!cp.isValid)
        BSLS_THROW(bsl::format_error("Invalid unicode code point"));  // RETURN

    if (static_cast<size_t>(cp.numSourceBytes) != outSpec->d_fillerCharacters *
                                                      sizeof(t_CHAR))
        BSLS_THROW(bsl::format_error("Invalid unicode code point"));  // RETURN

    // `{` and `}` are invalid fill characters per the C++ spec
    if ('{' == cp.codePointValue || '}' == cp.codePointValue)
        BSLS_THROW(
          bsl::format_error("Invalid fill character ('{' or '}')"));  // RETURN

    if (outSpec->d_fillerCodePointDisplayWidth >= 0) {
        if (outSpec->d_fillerCodePointDisplayWidth != cp.codePointWidth)
            BSLS_THROW(
                     bsl::format_error("Invalid code point width"));  // RETURN
    }
    else {
        outSpec->d_fillerCodePointDisplayWidth = cp.codePointWidth;
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
        FormatterSpecificationNumericValue::finalize(
                                                    &out->d_postprocessedWidth,
                                                    context);
    }

    if (0 != (out->d_sections & e_SECTIONS_PRECISION)) {
        out->d_postprocessedPrecision = out->d_rawPrecision;
        FormatterSpecificationNumericValue::finalize(
                                                &out->d_postprocessedPrecision,
                                                context);
    }

    out->d_parsingStatus = e_PARSING_POSTPROCESSED;
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
