// bdldfp_decimalformatutil.h                                         -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALFORMATUTIL
#define INCLUDED_BDLDFP_DECIMALFORMATUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide decimal format functions.
//
//@CLASSES:
//  bdldfp::DecimalFormatUtil: Namespace for decimal format functions
//
//@SEE_ALSO: bdldfp_decimal
//
//@DESCRIPTION: This component provides a namespace,
// 'bdldfp::DecimalFormatUtil', containing functions that format decimal types...
//

#include <bdldfp_decimal.h>
#include <bdldfp_decimalformatconfig.h>

namespace BloombergLP {
namespace bdldfp {

                        // =======================
                        // class DecimalFormatUtil
                        // =======================

class DecimalFormatUtil {
    // This 'class' provide a namespace for formatting operations on
    // 'Decimal32', 'Decimal64' and 'Decimal128' values.  Note that declaring
    // format functions in this class is a temporary solution and these
    // functions will be moved into 'DecimalUtil' class during 'bdldfp_Decimal'
    // package re-organization.

  public:
    static
    int format(char                      *buffer,
               int                        length,
               Decimal32                  value,
               const DecimalFormatConfig& cfg = DecimalFormatConfig(6));

    static
    int format(char                      *buffer,
               int                        length,
               Decimal64                  value,
               const DecimalFormatConfig& cfg = DecimalFormatConfig(15));

    static
    int format(char                      *buffer,
               int                        length,
               Decimal128                 value,
               const DecimalFormatConfig& cfg = DecimalFormatConfig(33));
        // Format the specified 'value' according to the parameters as
        // described below, placing the output in the buffer designated by the
        // specified 'buffer' and 'length', and return the length of the
        // formatted value.  If there is insufficient room in the buffer, its
        // contents will be left in an unspecified state, with the returned
        // value indicating the necessary size.  This function does not write a
        // terminating null character.  If 'length' is not positive, 'buffer'
        // is permitted to be null.  This can be used to determine the
        // necessary buffer size.
        //
        // Optionally specify 'precision' to control how many digits are
        // written after the decimal point.  If not specified, a default value
        // sufficient to produce all available digits in scientific notation is
        // used.  The behavior is undefined if 'precision' is negative.
        //
        // Optionally specify 'style' to control how the number is written.  If
        // 'style' is not given or is 'e_SCIENTIFIC', the number is written as
        // its sign (see below), then a single digit, then a decimal point (see
        // below), then 'precision' digits, then an 'e' or 'E' (see below),
        // then a '-' or '+', then an exponent with no leading zeroes (with a
        // zero exponent written as '0').  If 'style' is 'e_FIXED', the number
        // is written as its sign, then one or more digits, then a decimal
        // point, then 'precision' digits.  If the 'precision' value equals '0'
        // then precision digits and the decimal point are not written.  If
        // 'style' is 'e_NATURAL', the number is written according to the
        // description of 'to-scientific-string' found in
        // {http://speleotrove.com/decimal/decarith.pdf}.
        //
        // Optionally specify 'sign' to control how the sign is output.  If
        // 'value' has its sign bit set, a '-' is always written.  Otherwise,
        // if 'sign' is not given or is 'e_NEGATIVE_ONLY', no sign is written.
        //  If it is 'e_ALWAYS', a '+' is written, and if it is
        // 'e_POSITIVE_SPACE', a ' ' is written.
        //
        // Optionally specify 'letters' to control the case of letters.  If it
        // is not specified or 'e_LOWER', letters are written in lower-case.
        // If it is 'e_UPPER', letters are written in upper-case.  If it is
        // 'e_MIXED', letters are written as specified for
        // 'to-scientific-string' ('E' for exponents, 'Infinity', 'NaN', and
        // 'sNaN').
        //
        // Optionally specify 'point' as the character to use for decimal
        // points.  If it is not specified, '.' is used.
        //
        // Note that for some combinations of 'value' and 'precision', the
        // number being written must first be rounded to fewer digits than it
        // initially contains.  The number written must be as close as possible
        // to the initial value given the constraints on precision.  The
        // rounding should be done as "round-half-up", i.e., round up in
        // magnitude when the first of the discarded digits is between 5 and 9.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
