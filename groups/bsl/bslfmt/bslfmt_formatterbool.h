// bslfmt_formatterbase.h                                             -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERBOOL
#define INCLUDED_BSLFMT_FORMATTERBOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter customization for bool type
//
//@CLASSES:
//
//@DESCRIPTION: This component provides partial specializations of
// `bsl::formatter` catering for bool type.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Formatting a bool
/// - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test this formatter's ability to format a boolean with
// defined alignment and padding.
//
// ```
//  bslfmt::MockParseContext<char> mpc("*<6s", 1);
//
//  bsl::formatter<bool, char> f;
//  mpc.advance_to(f.parse(mpc));
//
//  bool value = false;
//
//  bslfmt::MockFormatContext<char> mfc(value, 0, 0);
//
//  mfc.advance_to(bsl::as_const(f).format(value, mfc));
//
//  assert("false*" == mfc.finalString());
// ```
//

#include <bslscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterintegral.h>
#include <bslfmt_formatterspecificationstandard.h>
#include <bslfmt_formatterunicodeutils.h>

#include <bslalg_numericformatterutil.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>
#include <bslmf_isarithmetic.h>
#include <bslmf_issame.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>
#include <bsls_keyword.h>

#include <bslstl_algorithm.h>
#include <bslstl_iterator.h>
#include <bslstl_string.h>
#include <bslstl_stringview.h>


#include <locale>     // for 'std::ctype', 'locale'
#include <string>     // for 'std::char_traits'

#include <stdio.h>    // for 'snprintf'

namespace BloombergLP {
namespace bslfmt {

                         // ========================
                         // struct FormatterBool_Imp
                         // ========================

/// This type implements the formatter logic specific to the boolean type.
template <class t_CHAR>
struct FormatterBool_Imp : public FormatterIntegralBase<bool, t_CHAR> {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESORS

    /// Create string representation of the specified `value`, customized in
    /// accordance with the requested format and the specified `formatContext`,
    /// and copy it to the output that the output iterator of the
    /// `formatContext` points to.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                        bool              value,
                                        t_FORMAT_CONTEXT& formatContext) const;
};

}  // close namespace bslfmt
}  // close enterprise namespace

namespace bsl {
// FORMATTER SPECIALIZATIONS

                         // ==============================
                         // struct formatter<bool, t_CHAR>
                         // ==============================

/// Partial specialization of the `bsl::formatter` template for the type
/// `bool`.
template <class t_CHAR>
struct formatter<bool, t_CHAR>
: BloombergLP::bslfmt::FormatterBool_Imp<t_CHAR> {
};

}

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslfmt {

                         // --------------------
                         // struct FormatterBool
                         // --------------------

template <class t_CHAR>
template <class t_FORMAT_CONTEXT>
inline
typename t_FORMAT_CONTEXT::iterator FormatterBool_Imp<t_CHAR>::format(
                                         bool              value,
                                         t_FORMAT_CONTEXT& formatContext) const
{
    typedef FormatterSpecificationStandard<t_CHAR>  FSS;
    typedef bslalg::NumericFormatterUtil            NFUtil;
    typedef Formatter_CharUtils<t_CHAR>             CharUtils;

    const FSS& parsedSpec = this->specification();

    const int  maxPrefixSize = 4;
    char       prefixBuf[maxPrefixSize];
    char      *prefixBegin = prefixBuf;
    char      *prefixEnd = prefixBuf;

    // We want to make sure that we have enough space to accommodate any
    // representation of the `value`.  In case of `unsigned char`
    // it should be large enough for binary representation, in case of string
    // we should be able to fit word `false`, which is 5 characters long.

    const int maxValueSize =
                      NFUtil::ToCharsMaxLength<unsigned char, 2>::k_VALUE > 5
                          ? NFUtil::ToCharsMaxLength<unsigned char, 2>::k_VALUE
                          : 5;
    t_CHAR     valueBuf[maxValueSize];
    t_CHAR    *valueBegin = valueBuf;
    t_CHAR    *valueEnd = valueBuf;

    if (FSS::e_BOOLEAN_STRING == parsedSpec.formatType()) {
        // String representation.

        const char *stringRepresentation       = value ? "true" : "false";
        const int   stringRepresentationLength = value ? 4      : 5;
        valueEnd = CharUtils::outputFromChar(
                             stringRepresentation,
                             stringRepresentation + stringRepresentationLength,
                             valueBegin);
    }
    else {
        unsigned char intRepresentation = static_cast<unsigned char>(value);
        prefixEnd = this->formatPrefix(prefixBuf,
                                       maxPrefixSize,
                                       intRepresentation);
        valueEnd = this->formatValue(valueBuf,
                                     maxValueSize,
                                     intRepresentation);
    }
    return this->outputValue(prefixBegin,
                             prefixEnd,
                             valueBegin,
                             valueEnd,
                             formatContext);
}

}  // close namespace bslfmt
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATTERBOOL

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
