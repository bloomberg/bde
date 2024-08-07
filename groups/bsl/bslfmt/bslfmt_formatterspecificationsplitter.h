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
#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterunicodeutils.h>

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'


namespace BloombergLP {
namespace bslfmt {

struct FormatterSpecification_SplitterEnums {
  public:
    // CLASS TYPES
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

    struct Value {
        // CLASS TYPES
        enum ValueType { e_DEFAULT, e_VALUE, e_NEXT_ARG, e_ARG_ID };

        // PUBLIC DATA
        int       d_value;
        ValueType d_type;

        // CREATORS
        Value();
        Value(int value, ValueType type);

        // CLASS FUNCTIONS
        template <class t_ITER>
        static int parse(Value  *outValue,
                         t_ITER *start,
                         t_ITER  end,
                         bool    hasInitialDot);

        // ACCESSORS
        bool operator==(const Value& other) const;
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

template <class t_CHAR, class t_ITER>
class FormatterSpecification_Splitter
: public FormatterSpecification_SplitterEnums {

  private:
    // PRIVATE CLASS TYPES
    enum { k_FILLER_BUFFER_SIZE = 5 };
    
    // DATA
    t_CHAR d_filler[k_FILLER_BUFFER_SIZE];  // one filler code point
    int    d_fillerCharacters;

    Alignment                      d_alignment;
    Sign                           d_sign;
    bool                           d_alternativeFlag;
    bool                           d_zeroPaddingFlag;
    Value                          d_width;
    Value                          d_precision;
    bool                           d_localeSpecificFlag;
    bsl::basic_string_view<t_CHAR> d_spec;

    // PRIVATE CLASS FUNCTIONS
    static Alignment alignmentFromChar(t_CHAR in);
    
    static Sign signFromChar(t_CHAR in);
    
    static int parseFillAndAlignment(FormatterSpecification_Splitter *outSpec,
                                     t_ITER                          *start,
                                     t_ITER                           end);

    static int parseSign(FormatterSpecification_Splitter *outSpec,
                         t_ITER                          *start,
                         t_ITER                           end);

    static int parseAlternateOption(FormatterSpecification_Splitter *outSpec,
                                    t_ITER                          *start,
                                    t_ITER                           end);

    static int parseZeroPaddingFlag(FormatterSpecification_Splitter *outSpec,
                                    t_ITER                          *start,
                                    t_ITER                           end);

    static int parseWidth(FormatterSpecification_Splitter *outSpec,
                          t_ITER                          *start,
                          t_ITER                           end);

    static int parsePrecision(FormatterSpecification_Splitter *outSpec,
                              t_ITER                          *start,
                              t_ITER                           end);

    static int parseLocaleSpecificFlag(
                                     FormatterSpecification_Splitter *outSpec,
                                     t_ITER                          *start,
                                     t_ITER                           end);

  public:

    // CLASS METHODS
    static int parse(FormatterSpecification_Splitter *outSpec,  // output
                     t_ITER                          *start,     // output
                     t_ITER                           end,       // input
                     Sections                         sections); // param
    
    // CREATORS
    FormatterSpecification_Splitter();

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
    bsl::basic_string_view<t_CHAR>  finalSpec();
};



// CREATORS

inline
FormatterSpecification_SplitterEnums::Value::Value()
: d_type(e_DEFAULT), d_value(0)
{
}

inline
FormatterSpecification_SplitterEnums::Value::Value(int value, ValueType type)
: d_type(type)
, d_value(value)
{
}

template <class t_CHAR, class t_ITER>
FormatterSpecification_Splitter<t_CHAR, t_ITER>::FormatterSpecification_Splitter()
: d_fillerCharacters(0)
, d_alignment(FormatterSpecification_Splitter::e_ALIGN_DEFAULT)
, d_sign(FormatterSpecification_Splitter::e_SIGN_DEFAULT)
, d_alternativeFlag(false)
, d_zeroPaddingFlag(false)
, d_localeSpecificFlag(false)
{
    memset(d_filler, 0, k_FILLER_BUFFER_SIZE * sizeof(t_CHAR));
}

// ACCESSORS

inline
bool FormatterSpecification_SplitterEnums::Value::operator==(
                                                      const Value& other) const
{
    return d_value == other.d_value && d_type == other.d_type;
}

template <class t_CHAR, class t_ITER>
const t_CHAR *FormatterSpecification_Splitter<t_CHAR, t_ITER>::filler()
{
    return d_filler;
}

template <class t_CHAR, class t_ITER>
int       FormatterSpecification_Splitter<t_CHAR, t_ITER>::fillerCharacters()
{
    return d_fillerCharacters;
}

template <class t_CHAR, class t_ITER>
typename
FormatterSpecification_Splitter<t_CHAR, t_ITER>::Alignment
FormatterSpecification_Splitter<t_CHAR, t_ITER>::alignment()
{
    return d_alignment;
}

template <class t_CHAR, class t_ITER>
typename
FormatterSpecification_Splitter<t_CHAR, t_ITER>::Sign
FormatterSpecification_Splitter<t_CHAR, t_ITER>::sign()
{
    return d_sign;
}

template <class t_CHAR, class t_ITER>
bool FormatterSpecification_Splitter<t_CHAR, t_ITER>::alternativeFlag()
{
    return d_alternativeFlag;
}

template <class t_CHAR, class t_ITER>
bool FormatterSpecification_Splitter<t_CHAR, t_ITER>::zeroPaddingFlag()
{
    return d_zeroPaddingFlag;
}

template <class t_CHAR, class t_ITER>
typename
FormatterSpecification_Splitter<t_CHAR, t_ITER>::Value
FormatterSpecification_Splitter<t_CHAR, t_ITER>::width()
{
    return d_width;
}

template <class t_CHAR, class t_ITER>
typename
FormatterSpecification_Splitter<t_CHAR, t_ITER>::Value
FormatterSpecification_Splitter<t_CHAR, t_ITER>::precision()
{
    return d_precision;
}

template <class t_CHAR, class t_ITER>
bool  FormatterSpecification_Splitter<t_CHAR, t_ITER>::localcSpecificFlag()
{
    return d_localeSpecificFlag;
}

template <class t_CHAR, class t_ITER>
bsl::basic_string_view<t_CHAR>
FormatterSpecification_Splitter<t_CHAR, t_ITER>::finalSpec()
{
    return d_spec;
}

// PRIVATE CLASS FUNCTIONS

template <class t_CHAR, class t_ITER>
inline
typename FormatterSpecification_Splitter<t_CHAR, t_ITER>::Alignment
FormatterSpecification_Splitter<t_CHAR, t_ITER>::alignmentFromChar(t_CHAR in)
{
    if ('<' == in)
        return e_ALIGN_LEFT;                                          // RETURN

    if ('>' == in)
        return e_ALIGN_RIGHT;                                         // RETURN

    if ('^' == in)
        return e_ALIGN_MIDDLE;                                        // RETURN

    return e_ALIGN_DEFAULT;
}

template <class t_CHAR, class t_ITER>
inline
typename FormatterSpecification_Splitter<t_CHAR, t_ITER>::Sign
FormatterSpecification_Splitter<t_CHAR, t_ITER>::signFromChar(t_CHAR in)
{
    if ('+' == in)
        return e_SIGN_POSITIVE;                                       // RETURN

    if ('-' == in)
        return e_SIGN_NEGATIVE;                                       // RETURN

    if (' ' == in)
        return e_SIGN_SPACE;                                          // RETURN

    return e_SIGN_DEFAULT;
}

template <class t_CHAR, class t_ITER>
int FormatterSpecification_Splitter<t_CHAR, t_ITER>::parse(
                                     FormatterSpecification_Splitter *outSpec,
                                     t_ITER                          *start,
                                     t_ITER                           end,
                                     Sections                         sections)
{
    BSLMF_ASSERT(
               (bsl::is_same<typename bsl::iterator_traits<t_ITER>::value_type,
                             t_CHAR>::value));

    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return 0;

    if (0 != (sections & e_SECTIONS_FILL_ALIGN)) {
        int rv = parseFillAndAlignment(outSpec, start, end);
        if (rv != 0)
            return rv;

        if (*start == end || **start == '}')
            return 0;
    }

    if (0 != (sections & e_SECTIONS_SIGN_FLAG)) {
        int rv = parseSign(outSpec, start, end);
        if (rv != 0)
            return rv;

        if (*start == end || **start == '}')
            return 0;
    }

    if (0 != (sections & e_SECTIONS_ALTERNATE_FLAG)) {
        int rv = parseAlternateOption(outSpec,
                                      start,
                                      end);
        if (rv != 0)
            return rv;

        if (*start == end || **start == '}')
            return 0;
    }

    if (0 != (sections & e_SECTIONS_ZERO_PAD_FLAG)) {
        int rv = parseZeroPaddingFlag(outSpec,
                                      start,
                                      end);
        if (rv != 0)
            return rv;

        if (*start == end || **start == '}')
            return 0;
    }

    if (0 != (sections & e_SECTIONS_WIDTH)) {
        int rv = parseWidth(outSpec, start, end);
        if (rv != 0)
            return rv;

        if (*start == end || **start == '}')
            return 0;
    }

    if (0 != (sections & e_SECTIONS_PRECISION)) {
        int rv = parsePrecision(outSpec, start, end);
        if (rv != 0)
            return rv;

        if (*start == end || **start == '}')
            return 0;
    }

    if (0 != (sections & e_SECTIONS_LOCALE_FLAG)) {
        int rv = parseLocaleSpecificFlag(outSpec, start, end);
        if (rv != 0)
            return rv;

        if (*start == end || **start == '}')
            return 0;
    }

    if (0 != (sections & e_SECTIONS_FINAL_SPECIFICATION)) {
        bsl::basic_string_view<t_CHAR> temp(*start, end);

        static const t_CHAR braces[3] = {'{', '}', '\0'};

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
            *start = end;
        }

        if (*start == end || **start == '}')
            return 0;
    }

    return -1;
}

template <class t_CHAR, class t_ITER>
int FormatterSpecification_Splitter<t_CHAR, t_ITER>::parseFillAndAlignment(
                                     FormatterSpecification_Splitter *outSpec,
                                     t_ITER                          *start,
                                     t_ITER                           end)
{
    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return 0;

    t_CHAR buffer[k_FILLER_BUFFER_SIZE] = {};
    t_ITER cp_iter                   = *start;
    int    cp_pos                    = 0;
    for (; cp_pos < k_FILLER_BUFFER_SIZE && cp_iter != end;
         ++cp_pos, ++cp_iter) {
        buffer[cp_pos] = *cp_iter;
    }

    bslfmt::Formatter_UnicodeUtils::CodePointExtractionResult cp;
    switch (sizeof(t_CHAR)) {
      case 1: {
        cp = Formatter_UnicodeUtils::extractCodePoint(
                                                Formatter_UnicodeUtils::e_UTF8,
                                                (const void *)buffer,
                                                cp_pos * sizeof(t_CHAR));
      } break;
      case 2: {
        cp = Formatter_UnicodeUtils::extractCodePoint(
                                                Formatter_UnicodeUtils::e_UTF16,
                                                (const void *)buffer,
                                                cp_pos * sizeof(t_CHAR));
      } break;
      case 4: {
        cp = Formatter_UnicodeUtils::extractCodePoint(
                                                Formatter_UnicodeUtils::e_UTF32,
                                                (const void *)buffer,
                                                cp_pos * sizeof(t_CHAR));
      } break;
      default: {
        BSLS_ASSERT_INVOKE_NORETURN("unreachable");
      }
    }

    if (!cp.isValid)
        return -1;                                                    // RETURN

    if (cp.numSourceBytes > cp_pos * sizeof(t_CHAR))
        return -1;                                                    // RETURN

    t_ITER aligner = *start + (cp.numSourceBytes / sizeof(t_CHAR));

    // Handle the case where the character after the first code point is a
    // valid alignment specifier.
    if (aligner != end) {
        if (alignmentFromChar(*aligner) != e_ALIGN_DEFAULT) {
            // '{' and '}' are invalid fill characters per the C++ spec
            if ('{' == cp.codePointValue || '}' == cp.codePointValue)
                return -1;                                            // RETURN
            outSpec->d_alignment = alignmentFromChar(*aligner);
            memcpy((void *)outSpec->d_filler, buffer, cp.numSourceBytes);
            outSpec->d_fillerCharacters = cp.numSourceBytes / sizeof(t_CHAR);

            *start = aligner + 1;
            return 0;
        }
    }

    // If the alignment specifier is not in 2nd position, it is allowed to be
    // in the first position, in which case the spec states that a space should
    // be used as the filler character.
    aligner = *start;
    if (alignmentFromChar(*aligner) != e_ALIGN_DEFAULT) {
        outSpec->d_alignment        = alignmentFromChar(*aligner);
        outSpec->d_filler[0]        = ' ';
        outSpec->d_fillerCharacters = 1;

        *start = aligner + 1;
        return 0;
    }

    // Otherwise we have neither fill nor alignment.
    return 0;
}

template <class t_CHAR, class t_ITER>
int FormatterSpecification_Splitter<t_CHAR, t_ITER>::parseSign(
                                     FormatterSpecification_Splitter *outSpec,
                                     t_ITER                          *start,
                                     t_ITER                           end)
{
    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return 0;

    outSpec->d_sign = signFromChar(**start);

    if (e_SIGN_DEFAULT != outSpec->d_sign) {
        ++*start;
    }

    return 0;
}

template <class t_CHAR, class t_ITER>
int FormatterSpecification_Splitter<t_CHAR, t_ITER>::parseAlternateOption(
                                     FormatterSpecification_Splitter *outSpec,
                                     t_ITER                          *start,
                                     t_ITER                           end)
{
    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return 0;

    outSpec->d_alternativeFlag = ((t_CHAR)'#' == **start);

    if (outSpec->d_alternativeFlag) {
        ++*start;
    }

    return 0;
}

template <class t_CHAR, class t_ITER>
int FormatterSpecification_Splitter<t_CHAR, t_ITER>::parseZeroPaddingFlag(
                                     FormatterSpecification_Splitter *outSpec,
                                     t_ITER                          *start,
                                     t_ITER                           end)
{
    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return 0;

    outSpec->d_zeroPaddingFlag = ((t_CHAR)'0' == **start);

    if (outSpec->d_zeroPaddingFlag) {
        ++*start;
    }

    return 0;
}

template <class t_CHAR, class t_ITER>
int FormatterSpecification_Splitter<t_CHAR, t_ITER>::parseWidth(
                                     FormatterSpecification_Splitter *outSpec,
                                     t_ITER                          *start,
                                     t_ITER                           end)
{
    typedef FormatterSpecification_Splitter<t_CHAR, t_ITER> FS;

    int rv = Value::parse(&outSpec->d_width, start, end, false);
    // Widths must be strictly positive.
    if (outSpec->d_width == FS::Value(0, FS::Value::e_VALUE))
        rv = -1;
    return rv;
}

template <class t_CHAR, class t_ITER>
int FormatterSpecification_Splitter<t_CHAR, t_ITER>::parsePrecision(
                                     FormatterSpecification_Splitter *outSpec,
                                     t_ITER                          *start,
                                     t_ITER                           end)
{
    return Value::parse(&outSpec->d_precision, start, end, true);
}

template <class t_CHAR, class t_ITER>
int FormatterSpecification_Splitter<t_CHAR, t_ITER>::parseLocaleSpecificFlag(
                                     FormatterSpecification_Splitter *outSpec,
                                     t_ITER                          *start,
                                     t_ITER                           end)
{
    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return 0;

    outSpec->d_localeSpecificFlag = ((t_CHAR)'L' == **start);

    if (outSpec->d_localeSpecificFlag) {
        ++*start;
    }

    return 0;
}

template <class t_ITER>
int FormatterSpecification_SplitterEnums::Value::parse(Value  *outValue,
                                                      t_ITER *start,
                                                      t_ITER  end,
                                                      bool    needInitialDot)
{
    // Handle empty string or empty specification.
    if (*start == end || **start == '}')
        return 0;                                                     // RETURN

    t_ITER current = *start;

    if (needInitialDot) {
        // No dot therefore no precision - early successful exit.
        if (*current != '.')
            return 0;                                                 // RETURN
        ++current;
        // A dot but nothing afterwards is an invalid precision string.
        if (current == end)
            return -1;                                                // RETURN
    }

    bool isArgId = false;

    if (*current == '{') {
        current++;
        // Missing matching closing brace.
        if (current == end)
            return -1;                                                // RETURN
        // Early exit for a non-numbered replacement field.
        if (*current == '}') {
            outValue->d_type = e_NEXT_ARG;
            outValue->d_value = 0;
            *start = current + 1;
            return 0;                                                 // RETURN
        }
        isArgId = true;
    }

    enum { k_VALUE_BUFFER_SIZE = 64 };

    char buffer[k_VALUE_BUFFER_SIZE + 1] = {};  // note char not t_CHAR
    int  cp_pos                          = 0;
    for (; cp_pos < k_VALUE_BUFFER_SIZE && current != end;
         ++cp_pos, ++current) {

        if (!isdigit(static_cast<int>(*current)))
            break;

        buffer[cp_pos] = static_cast<char>(*current);
    }

    // Buffer overrun
    if (cp_pos == k_VALUE_BUFFER_SIZE)
        return -1;                                                    // RETURN

    // No digits
    if (cp_pos == 0) {
        // If we have either specified the "precision dot" or if we know we
        // have a numbered replacement field then digits are non-optional.
        if (needInitialDot || isArgId) {
            return -1;                                                // RETURN
        }
    }

    // We know buffer holds only digits, so it is safe to call atoi. As we do
    // not allow + or - the value must be non-negative.
    outValue->d_value = atoi(buffer);

    if (isArgId) {
        // Relative argument references must have a closing brace.
        if (current == end || *current != '}')
            return -1;                                                // RETURN
        ++current;
    }

    outValue->d_type = isArgId ? e_ARG_ID : e_VALUE;

    *start = current;

    return 0;
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
