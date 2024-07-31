// bslfmt_formatterspecificationsplitter.h                            -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERSPECIFICATIONSPLITTER
#define INCLUDED_BSLFMT_FORMATTERSPECIFICATIONSPLITTER

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

#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'


namespace BloombergLP {
namespace bslfmt {

template <class t_CHAR, class t_ITER>
class Formatter_SpecificationSplitter {
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
        enum ValueType { e_DEFAULT, e_VALUE, e_ARG_ID };
        // CREATORS
        Value();
        // DATA
        int       d_value;
        ValueType d_type;
    };

  private:

    // DATA
    unsigned char                  d_filler[5];  // one filler code point
    Alignment                      d_alignment;
    Sign                           d_sign;
    bool                           d_alternativeFlag;
    bool                           d_zeroPaddingFlag;
    Value                          d_width;
    Value                          d_precision;
    bool                           d_localeSpecificFlag;
    bsl::basic_string_view<t_CHAR> d_spec;

    // PRIVATE CLASS FUNCTIONS
    Alignment alignmentFromChar(t_CHAR in);
    
    Sign signFromChar(t_CHAR in);
    
    // PRIVATE MANIPULATORS
    int parseFillAndAlignment(t_ITER *start, t_ITER end);

    int parseSign(t_ITER *start, t_ITER end);

    int parseAlternateOption(t_ITER *start, t_ITER end);

    int parseZeroPaddingFlag(t_ITER *start, t_ITER end);

    int parseWidth(t_ITER *start, t_ITER end);

    int parsePrecision(t_ITER *start, t_ITER end);

    int parseLocaleSpecificFlag(t_ITER *start, t_ITER end);

  public:

    // CREATORS
    Formatter_SpecificationSplitter();

    // MANIPULATORS
    int parse(t_ITER start, t_ITER end);
};

template <class t_CHAR, class t_ITER>
Formatter_SpecificationSplitter<t_CHAR, t_ITER>::Value::Value()
: d_type(e_DEFAULT), d_value(0)
{
}

template <class t_CHAR, class t_ITER>
Formatter_SpecificationSplitter<t_CHAR, t_ITER>::Formatter_SpecificationSplitter()
: d_alignment(Formatter_SpecificationSplitter::e_ALIGN_DEFAULT)
, d_sign(Formatter_SpecificationSplitter::e_SIGN_DEFAULT)
, d_alternativeFlag(false)
, d_zeroPaddingFlag(false)
, d_localeSpecificFlag(false)
{
    memset(d_filler, 0, 5);
}

template <class t_CHAR, class t_ITER>
int Formatter_SpecificationSplitter<t_CHAR, t_ITER>::parse(t_ITER start,
                                                           t_ITER end)
{
    BSLMF_ASSERT((
        bsl::is_same<bsl::iterator_traits<t_ITER>::value_type, t_CHAR>::value));

    // Handle empty string or empty specification.
    if (start == end || *start == '}')
        return 0;

    int rv = parseFillAndAlignment(&start, end);
    if (rv != 0)
        return rv;

    rv = parseSign(&start, end);
    if (rv != 0)
        return rv;

    rv = parseAlternateOption(&start, end);
    if (rv != 0)
        return rv;

    rv = parseZeroPaddingFlag(&start, end);
    if (rv != 0)
        return rv;

    rv = parseWidth(&start, end);
    if (rv != 0)
        return rv;

    rv = parsePrecision(&start, end);
    if (rv != 0)
        return rv;

    rv = parseLocaleSpecificFlag(&start, end);
    if (rv != 0)
        return rv;

    d_spec = bsl::basic_string_view<t_CHAR>(start, end);
    if (0 == d_spec.size())
        return -1;

    return 0;
}

template <class t_CHAR, class t_ITER>
int Formatter_SpecificationSplitter<t_CHAR, t_ITER>::parseFillAndAlignment(
                                                                 t_ITER *start,
                                                                 t_ITER  end)
{
    // Handle empty string or empty specification.
    if (start == end || *start == '}')
        return 0;

    t_ITER filler = *start;

    t_CHAR buffer[4];
    for (int cp_pos = 0, t_ITER cp_iter = filler;
         cp_pos < 4 && cp_iter != last;
         ++cp_pos, ++cp_iter)
    {
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
                                                Formatter_UnicodeUtils::e_UTF8,
                                                (const void *)buffer,
                                                cp_pos * sizeof(t_CHAR));
      } break;
      case 4: {
        cp = Formatter_UnicodeUtils::extractCodePoint(
                                                Formatter_UnicodeUtils::e_UTF8,
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

    t_ITER aligner = start + cp.numSourceBytes / sizeof(t_CHAR);

    // Handle the case where we have a single code point only in the string.
    // This must always be invalid as we need to have space for a closing `}`.
    if (aligner == end)
        return -1;                                                    // RETURN

    // Handle the case where the character after the first code point is a
    // valid alignment specifier.
    if (alignmentFromChar(*aligner) != e_ALIGN_DEFAULT) {
        // Handle invalid fill characters
        if ('{' == cp.codePointValue || '}' == cp.codePointValue)
            return -1;                                                // RETURN
        d_alignment = alignmentFromChar(*aligner);
        memcpy((void *)d_filler, buffer, cp.numSourceBytes);
        *start = aligner + 1;
        return 0;
    }

    // If the alignment specifier is not in 2nd position, it is allowed to be
    // in the first position.
    aligner = start;
    if (alignmentFromChar(*aligner) != e_ALIGN_DEFAULT) {
        d_alignment = alignmentFromChar(*aligner);
        d_filler[0] = ' ';
        *start = aligner + 1;
        return 0;
    }

    // Otherwise we have neither fill nor alignment.
    return 0;
}

template <class t_CHAR, class t_ITER>
int Formatter_SpecificationSplitter<t_CHAR, t_ITER>::parseSign(t_ITER *start,
                                                               t_ITER  end)
{
    // Handle empty string or empty specification.
    if (start == end || *start == '}')
        return 0;

    d_sign = signFromChar(*start);

    if (e_SIGN_DEFAULT != d_sign) {
        ++*start;
    }

    return 0;
}

template <class t_CHAR, class t_ITER>
int Formatter_SpecificationSplitter<t_CHAR, t_ITER>::parseAlternateOption(
                                                                 t_ITER *start,
                                                                 t_ITER  end)
{
    // Handle empty string or empty specification.
    if (start == end || *start == '}')
        return 0;

    d_alternativeFlag = ((t_CHAR)'#' == *start);

    if (d_alternativeFlag) {
        ++*start;
    }

    return 0;
}

template <class t_CHAR, class t_ITER>
int Formatter_SpecificationSplitter<t_CHAR, t_ITER>::parseZeroPaddingFlag(
                                                                 t_ITER *start,
                                                                 t_ITER  end)
{
    // Handle empty string or empty specification.
    if (start == end || *start == '}')
        return 0;

    d_zeroPaddingFlag = ((t_CHAR)'0' == *start);

    if (d_zeroPaddingFlag) {
        ++*start;
    }

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
