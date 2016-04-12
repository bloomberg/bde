// bdlt_datetimeimputil.h                                             -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIMEIMPUTIL
#define INCLUDED_BDLT_DATETIMEIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide constants useful for encoding datetimes.
//
//@CLASSES:
//  bdlt::DatetimeImpUtil: namespace for datetime encoding constants
//
//@DESCRIPTION: This component implements a utility 'struct',
// 'bdlt::DatetimeImpUtil', that defines a namespace for obtaining constants
// and 'bdlt::Datetime *' to constant values that are useful to encoding
// datetimes at static-initilaization time.  The assumptions for this encoding
// are that a collection of unset values, one per day over the valid range of
// 'bdlt::Date', are encoded in the lowest values, and then microsecond
// resolution over the entire 'bdlt::Date' range.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Syntax
///- - - - - - - - - - - -
// To obtain the 'bdlt::Datetime' internal value for 0001/01/01 at
// static-initialization time:
//..
//  static const bdlt::Datetime *firstDatetime =
//                                   bdlt::DatetimeImpUtil::epoch_0001_01_01();
//
//  assert(reinterpret_cast<const bdlt::Datetime *>(
//               &bdlt::DatetimeImpUtil::k_0001_01_01_VALUE) == firstDatetime);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bdlt {

class Datetime;

                          // ======================
                          // struct DatetimeImpUtil
                          // ======================

struct DatetimeImpUtil {
    // This 'struct' provides a namespace for datetime encoding constants.

    // CLASS DATA
    static const bsls::Types::Uint64 k_0001_01_01_VALUE;
    static const bsls::Types::Uint64 k_1970_01_01_VALUE;
    static const bsls::Types::Uint64 k_MAX_VALUE;

    // CLASS METHODS
    static const Datetime *epoch_0001_01_01();
        // Return a pointer to a 'bdlt::Datetime' with value
        // 'bdlt::Datetime(1, 1, 1)' suitable for use during static
        // initialization.

    static const Datetime *epoch_1970_01_01();
        // Return a pointer to a 'bdlt::Datetime' with value
        // 'bdlt::Datetime(1970, 1, 1)' suitable for use during static
        // initialization.

    static const Datetime *epoch_max();
        // Return a pointer to a 'bdlt::Datetime' with value
        // 'bdlt::Datetime(9999, 12, 31, 24)' suitable for use during static
        // initialization.
};

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
