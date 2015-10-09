// bdldfp_binaryintegraldecimalimputil.h                              -*-C++-*-
#ifndef INCLUDED_BDLDFP_BINARYINTEGRALDECIMALIMPUTIL
#define INCLUDED_BDLDFP_BINARYINTEGRALDECIMALIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Utilities for working with Binary Integral Decimal representation.
//
//@CLASSES:
//  bdldfp::BinaryIntegralDecimalImpUtil: namespace for BID functions.
//
//@SEE_ALSO: bdldfp_decimal, bdldfp_decimalplatform,
//  bdldfp_denselypackeddecimalimputil
//
//@DESCRIPTION: This component provides a namespace,
// 'bdldfp::BinaryIntegralDecimalImpUtil', that supplies the necessary types
// for storing a Binary Integral Decimal (BID) representation of a decimal
// floating point value.
//
///Usage
///-----
// This section shows the intended use of this component.
//
///Example 1: TBD
/// - - - - - - -

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLDFP_DECIMALPLATFORM
#include <bdldfp_decimalplatform.h>
#endif

#ifndef INCLUDED_BDLDFP_UINT128
#include <bdldfp_uint128.h>
#endif

#ifndef INCLUDED_DENSELYPACKEDDECIMALIMPUTIL
#include <bdldfp_denselypackeddecimalimputil.h>
#endif

namespace BloombergLP {
namespace bdldfp {

                        // ==================================
                        // class BinaryIntegralDecimalImpUtil
                        // ==================================

struct BinaryIntegralDecimalImpUtil {
    // This 'struct' provides a namespace for functions that provide common DPD
    // formatted decimal floating point.

    // TYPES
    struct StorageType32  { unsigned           int d_raw; };
    struct StorageType64  { unsigned long long int d_raw; };
    struct StorageType128 { bdldfp::Uint128        d_raw; };
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
