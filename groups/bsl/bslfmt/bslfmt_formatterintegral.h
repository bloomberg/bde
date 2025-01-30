// bslfmt_formatterintegral.h                                         -*-C++-*-

#ifndef INCLUDED_BSLFMT_FORMATTERINTEGRAL
#define INCLUDED_BSLFMT_FORMATTERINTEGRAL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a formatter customization for integer types
//
//@CLASSES:
//  bsl::formatter<short, t_CHAR>: formatter specialization for `short`
//  bsl::formatter<unsigned short, t_CHAR>: specialization for `unsigned short`
//  bsl::formatter<int, t_CHAR>: formatter specialization for `int`
//  bsl::formatter<unsigned, t_CHAR>: formatter specialization for `unsigned`
//  bsl::formatter<long, t_CHAR>: formatter specialization for `long`
//  bsl::formatter<unsigned long, t_CHAR>:  specialization for `unsigned long`
//  bsl::formatter<long long, t_CHAR>: formatter specialization for `long long`
//  bsl::formatter<unsigned long long, t_CHAR>: for `unsigned long long`
//
//@CANONICAL_HEADER: bsl_format.h
//
//@DESCRIPTION: This component provides partial specializations of
// `bsl::formatter` catering for integer types.
//
// This header is not intended to be included directly.  Please include
// `<bsl_format.h>` to be able to use specializations of the `bsl::formatter`
// for integral types.
//
///Usage
///-----
// In this section we show the intended use of this component.
//
///Example: Formatting an integer
/// - - - - - - - - - - - - - - -
// We do not expect most users of `bsl::format` to interact with this type
// directly and instead use `bsl::format` or `bsl::vformat`, so this example is
// necessarily unrealistic.
//
// Suppose we want to test this formatter's ability to format an integer with
// defined alignment and padding.
//
// ```
//  bslfmt::MockParseContext<char> mpc("*<5x", 1);
//
//  bsl::formatter<int, char> f;
//  mpc.advance_to(f.parse(mpc));
//
//  int value = 42;
//
//  bslfmt::MockFormatContext<char> mfc(value, 0, 0);
//
//  mfc.advance_to(bsl::as_const(f).format(value, mfc));
//
//  assert("2a***" == mfc.finalString());
// ```
//

#include <bslscm_version.h>

#include <bslfmt_formatterbase.h>
#include <bslfmt_formatterintegralbase.h>

#include <bslalg_numericformatterutil.h>

namespace BloombergLP {
namespace bslfmt {

                        // ============================
                        // struct FormatterIntegral_Imp
                        // ============================

/// This type implements the formatter logic specific to integer types other
/// than character, boolean and pointer type.
template <class t_VALUE, class t_CHAR>
struct FormatterIntegral_Imp : public FormatterIntegralBase<t_VALUE, t_CHAR> {
  public:
    // TRAITS
    BSL_FORMATTER_PREVENT_STD_DELEGATION_TRAIT_CPP20;

    // ACCESSORS

    /// Create string representation of the specified `value`, customized in
    /// accordance with the requested format and the specified `formatContext`,
    /// and copy it to the output that the output iterator of the
    /// `formatContext` points to.
    template <class t_FORMAT_CONTEXT>
    typename t_FORMAT_CONTEXT::iterator format(
                                        t_VALUE           value,
                                        t_FORMAT_CONTEXT& formatContext) const;
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {
// FORMATTER SPECIALIZATIONS

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned char`.
template <class t_CHAR>
struct formatter<unsigned char, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral_Imp<unsigned char, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `short int`.
template <class t_CHAR>
struct formatter<short, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral_Imp<short, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned short int`.
template <class t_CHAR>
struct formatter<unsigned short, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral_Imp<unsigned short, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type `int`.
template <class t_CHAR>
struct formatter<int, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral_Imp<int, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned int`.
template <class t_CHAR>
struct formatter<unsigned, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral_Imp<unsigned, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `long int`.
template <class t_CHAR>
struct formatter<long, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral_Imp<long, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned long int`.
template <class t_CHAR>
struct formatter<unsigned long, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral_Imp<unsigned long, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `long long int`.
template <class t_CHAR>
struct formatter<long long, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral_Imp<long long, t_CHAR> {
};

/// Partial specialization of the `bsl::formatter` template for the type
/// `unsigned long long int`.
template <class t_CHAR>
struct formatter<unsigned long long, t_CHAR>
: BloombergLP::bslfmt::FormatterIntegral_Imp<unsigned long long, t_CHAR> {
};

}  // close namespace bsl


// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

namespace BloombergLP {
namespace bslfmt {

                        // ----------------------------
                        // struct FormatterIntegral_Imp
                        // ----------------------------

template <class t_VALUE, class t_CHAR>
template <class t_FORMAT_CONTEXT>
inline
typename t_FORMAT_CONTEXT::iterator
FormatterIntegral_Imp<t_VALUE, t_CHAR>::format(
                                         t_VALUE           value,
                                         t_FORMAT_CONTEXT& formatContext) const
{
    typedef BloombergLP::bslalg::NumericFormatterUtil NFUtil;

    const int  maxPrefixSize = 4;
    char       prefixBuf[maxPrefixSize];
    char      *prefixBegin = prefixBuf;
    char      *prefixEnd = this->formatPrefix(prefixBuf, maxPrefixSize, value);

    // We want to make sure that we have enough space to accommodate any
    // representation of the `value`.  Binary representation takes up the most
    // space.

    const int  maxValueSize = NFUtil::ToCharsMaxLength<t_VALUE, 2>::k_VALUE;
    t_CHAR     valueBuf[maxValueSize];
    t_CHAR    *valueBegin = valueBuf;
    t_CHAR    *valueEnd   = this->formatValue(valueBuf, maxValueSize, value);

    if (value < 0) {
        // We want to omit minus sign added by
        // `NumericFormatterUtil::toChars` since we  already added it
        // manually to the prefix.
        ++valueBegin;
    }

    return this->outputValue(prefixBegin,
                             prefixEnd,
                             valueBegin,
                             valueEnd,
                             formatContext);
}

}  // close package namespace
}  // close enterprise namespace

#endif  // INCLUDED_BSLFMT_FORMATTERINTEGRAL

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
