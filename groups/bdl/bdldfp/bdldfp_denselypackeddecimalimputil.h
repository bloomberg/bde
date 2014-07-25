// bdldfp_denselypackeddecimalimputil.h                               -*-C++-*-
#ifndef INCLUDED_BDLDFP_DENSELYPACKEDDECIMALIMPUTIL
#define INCLUDED_BDLDFP_DENSELYPACKEDDECIMALIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Provide utilities to help implement decimal floating-point types.
//
//@CLASSES:
//  bdldfp::DenselyPackedDecimalImpUtil: Namespace for DPD functions.
//
//@MACROS:
//  BDLDFP_DECIMALIMPLUTIL_DF: ValueType32  from literal
//  BDLDFP_DECIMALIMPLUTIL_DD: ValueType64  from literal
//  BDLDFP_DECIMALIMPLUTIL_DL: ValueType128 from literal
//
//@SEE ALSO: bdldfp_decimal, bdldfp_decimalplatform
//
//@DESCRIPTION: This component is for internal use only by the
// 'bdldfp_decimal*' components.  Direct use of any names declared in this
// component by any other code invokes undefined behavior.  In other words:
// this code may change, disappear, break, move without notice, and no support
// whatsoever will ever be provided for it.
//
// This component contains:
//
//: o portable decimal floating-point macros that create the implementation
//:   type (C99 or 'decNumber' library)
//:
//: o the decimal floating-point environment/context for 'decNumber' library
//:   (when it is used)
//:
//: o the definitions of the implementation types (to be used, e.g., as members
//:   in the 'DecimalN' types)
//:
//: o the parsing functions that turn literals into values on platforms that do
//:   not yet support decimal floating-point literals, and possibly to use by
//:   stream input operators.
//
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: Get the DPD representation of a 10 digit binary number
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//
//..
//  int value = 842;
//  int asDeclet = bdldfp::DenselyPackedDecimalImpUtil::encodeDeclet(value);
//  assert(
//       bdldfp::DenselyPackedDecimalImpUtil::decodeDeclet(asDeclet) == value);
//..



#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#endif

#ifndef INCLUDED_BDLDFP_UINT128
#include <bdldfp_uint128.h>
#endif

namespace BloombergLP {
namespace bdldfp {

                          // =================================
                          // class DenselyPackedDecimalImpUtil
                          // =================================

struct DenselyPackedDecimalImpUtil {
    // This 'struct' provides a namespace for implementation functions that
    // provide common DPD formatted decimal floating point support.

    // TYPES

    typedef unsigned           int StorageType32;
    typedef unsigned long long int StorageType64;
    typedef bdldfp::Uint128        StorageType128;

    // CLASS METHODS

    static unsigned encodeDeclet(unsigned digits);
        // Return an unsigned integer which represents an encoding of the
        // specified 'digits' in Densely Packed Decimal (DPD) format.  The
        // behavior is undefined unless '0 <= digits < 1000'.  Note that the
        // result will be a bit pattern whose binary representation corresponds
        // to a value in the range '[0, 1024]'.  Also note that this function
        // accepts and returns 'unsigned int' type values, as it is intended
        // for use with raw bit manipulation functions.

    static unsigned decodeDeclet(unsigned declet);
        // Return an unsigned integer containing the digits represented by the
        // specified 'declet'.  The behavior is undefined unless the binary
        // value of 'declet < 1024', and 'declet' is encoded in Densely Packed
        // Decimal (DPD), and every unused bit in the densely packed state is
        // set to 0.  Note that the result will be an integer in the range
        // '[0, 999]'.  Also note that this function accepts and returns
        // 'unsigned int' type values, as it is intended for use with raw bit
        // manipulation functions.

    static StorageType32 makeDecimalRaw32(int mantissa, int exponent);
        // Return a 'StorageType32' object representing a decimal floating
        // point number consisting of the specified 'mantissa' and 'exponent',
        // with the sign given by the 'mantissa'.  If 'mantissa' is 0, the
        // result is 0 but the quanta of the result is unspecified.  The
        // behavior is undefined unless 'abs(mantissa) <= 9,999,999' and
        // '-101 <= exponent <= 90'.

    static StorageType64 makeDecimalRaw64(unsigned long long int mantissa,
                                                             int exponent);
    static StorageType64 makeDecimalRaw64(         long long int mantissa,
                                                             int exponent);
    static StorageType64 makeDecimalRaw64(unsigned           int mantissa,
                                                             int exponent);
    static StorageType64 makeDecimalRaw64(                   int mantissa,
                                                             int exponent);
        // Create a 'StorageType64' object representing a decimal floating
        // point number consisting of the specified 'mantissa' and 'exponent',
        // with the sign given by the 'mantissa'.  If 'mantissa' is 0, the
        // result is 0 but the quanta of the result is unspecified.  The
        // behavior is undefined unless
        // 'abs(mantissa) <= 9,999,999,999,999,999' and
        // '-398 <= exponent <= 369'.

    static StorageType128 makeDecimalRaw128(unsigned long long int mantissa,
                                                               int exponent);
    static StorageType128 makeDecimalRaw128(         long long int mantissa,
                                                               int exponent);
    static StorageType128 makeDecimalRaw128(unsigned           int mantissa,
                                                               int exponent);
    static StorageType128 makeDecimalRaw128(                   int mantissa,
                                                               int exponent);
        // Create a 'StorageType128' object representing a decimal floating
        // point number consisting of the specified 'mantissa' and 'exponent',
        // with the sign given by the 'mantissa'.  If 'mantissa' is 0, the
        // result is 0 but the quanta of the result is unspecified.  The
        // behavior is undefined unless '-6176 <= exponent <= 6111'.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2014 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
