// bdlt_formatutil.h                                                  -*-C++-*-
#ifndef INCLUDED_BDLT_FORMATUTIL
#define INCLUDED_BDLT_FORMATUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities for formatting `bdlt` types
//
//@CLASSES:
//  bdlt::FormatUtil: namespace for formatting utilities for `bdlt`
//
//@DESCRIPTION: This component provides a namespace, `bdlt::FormatUtil`,
// containing functions facilitating `bsl::format`-style formatting of `bdlt`
// value types.  The output character type (`char` or `wchar_t`) is controlled
// via the template parameter `t_CHAR`.

#include <bdlscm_version.h>

#include <bslfmt_formaterror.h>
#include <bslfmt_formatspecificationparser.h>
#include <bslfmt_formattercharutil.h>
#include <bslfmt_padutil.h>

#include <bslmf_assert.h>
#include <bslmf_issame.h>

#include <bsla_fallthrough.h>

#include <bsls_assert.h>
#include <bsls_exceptionutil.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_charconv.h>
#include <bsl_cstddef.h>

namespace BloombergLP {
namespace bdlt {

                                    // ==========
                                    // FormatUtil
                                    // ==========

/// This `class` serves as a namespace for some static functions that are
/// useful in creating specializations of `bsl::formatter` to support
/// `bsl::format`.
template<class t_CHAR>
class FormatUtil {
    BSLMF_ASSERT((bsl::is_same<t_CHAR, char>::value ||
                  bsl::is_same<t_CHAR, wchar_t>::value));

    // PRIVATE TYPES
    typedef bslfmt::PadUtil<t_CHAR>        PadUtil;

  public:
    // PUBLIC TYPES

    typedef bsl::basic_string_view<t_CHAR> StringView;

    // CLASS METHODS

    /// Write the fraction of a second from the timeCache to `d_out`, where the
    /// `precision` is the number of digits of precision wanted.  Use
    /// `decimalChar` (expected to be '.' or ',') as a decimal point, and if
    /// `precision == 0` output nothing.  If `6 < precision`, write '0'
    /// characters following the first 6 digits.  The behavior is undefined
    /// unless `0 <= millisecond < 1000`, `0 <= microsecond < 1000`, and
    /// `0 <= precision`.
    template <class t_ITERATOR>
    static t_ITERATOR writeSecondFraction(t_ITERATOR out,
                                          int        precision,
                                          t_CHAR     decimalChar,
                                          int        millisecond,
                                          int        microsecond);

    /// Write the specified `value` in decimal to the specified `out`.  Always
    /// write the specified `numDigits` digits, '0'-padding to the left if
    /// necessary.  Return the iterator after output is done.  The behavior is
    /// undefined unless `2 <= numDigits <= 6` and
    /// `0 <= value < pow(10, numDigits)`.
    template <class t_ITERATOR>
    static t_ITERATOR writeZeroPaddedDigits(t_ITERATOR out,
                                            int        value,
                                            int        numDigits = 2);
};

                                    // ----------
                                    // FormatUtil
                                    // ----------

// CLASS METHODS
template <class t_CHAR>
template <class t_ITERATOR>
t_ITERATOR FormatUtil<t_CHAR>::writeSecondFraction(t_ITERATOR out,
                                                   int        precision,
                                                   t_CHAR     decimalChar,
                                                   int        millisecond,
                                                   int        microsecond)
{
    BSLS_ASSERT(0 <= millisecond && millisecond < 1000);
    BSLS_ASSERT(0 <= microsecond && microsecond < 1000);
    BSLS_ASSERT(0 <= precision);

    if (precision == 0) {
        return out;                                                   // RETURN
    }
    *out++ = t_CHAR(decimalChar);

    char                 buffer[12] = "000000";
    bsl::to_chars_result result     = bsl::to_chars(
                                             buffer + 6,
                                             bsl::end(buffer),
                                             millisecond * 1000 + microsecond);
    BSLS_ASSERT(bsl::ErrcEnum() == result.ec);

    char *valueBegin = result.ptr - 6;
    char *valueEnd   = result.ptr;

    if (precision > 6) {
        out = bsl::copy(valueBegin, valueEnd, out);
        out = PadUtil::pad(out, precision - 6, '0');
    }
    else {
        out = bsl::copy(valueBegin, valueBegin + precision, out);
    }

    return out;
}

template <class t_CHAR>
template <class t_ITERATOR>
inline
t_ITERATOR FormatUtil<t_CHAR>::writeZeroPaddedDigits(t_ITERATOR out,
                                                     int        value,
                                                     int        numDigits)
{
    BSLS_ASSERT_SAFE(2 <= numDigits && numDigits <= 6);
    BSLS_ASSERT(0 <= value && value < 1000000);

    enum { k_BUF_LEN = 6 };

    char                 buf[k_BUF_LEN];
    bsl::to_chars_result result = bsl::to_chars(bsl::begin(buf),
                                                bsl::end(buf),
                                                value);
    BSLS_ASSERT(bsl::ErrcEnum() == result.ec);
    char * const         end = result.ptr;

    bsl::ptrdiff_t padWidth = numDigits - bsl::distance(buf, end);
    BSLS_ASSERT(0 <= padWidth);
    out = PadUtil::pad(out, padWidth, '0');

    out = bslfmt::FormatterCharUtil<t_CHAR>::outputFromChar(buf + 0, end, out);

    return out;
}

}  // close namespace bdlt
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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

