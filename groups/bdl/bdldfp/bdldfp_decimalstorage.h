// bdldfp_decimalstorage.h                                            -*-C++-*-
#ifndef INCLUDED_BDLDFP_DECIMALSTORAGE
#define INCLUDED_BDLDFP_DECIMALSTORAGE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id$")

//@PURPOSE: Utilities for working with Decimal Storage types.
//
//@CLASSES:
//  bdldfp::DecimalStorage: namespace for Decimal Storage types.
//
//@SEE_ALSO: bdldfp_decimal, bdldfp_decimalplatform,
//
//@DESCRIPTION: This component provides a namespace, 'bdldfp::DecimalStorage',
// that supplies the necessary types for storing a Binary Integral Decimal
// (BID) representation of a decimal floating point value.
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

#ifndef INCLUDED_BDLDFP_INTELIMPWRAPPER
#include <bdldfp_intelimpwrapper.h>
#endif

namespace BloombergLP {
namespace bdldfp {

                        // ====================
                        // class DecimalStorage
                        // ====================

struct DecimalStorage {
    // This 'struct' provides a namespace for types for storing Binary Integral
    // Decimal representation of a decimal floating point value.

    // TYPES
    typedef BID_UINT32  Type32;
    typedef BID_UINT64  Type64;
    typedef BID_UINT128 Type128;
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
