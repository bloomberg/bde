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
// 'bdlt::DatetimeImpUtil', that defines a namespace for constants that are
// useful to encoding datetimes.  The assumptions for this encoding are that a
// collection of unset values, one per day over the valid range of
// 'bdlt::Date', are encoded in the lowest values, then microsecond resolution
// from the start of the 'bdlt::Date' range to 1970/01/01, then nanosecond
// resolution throughy 2470/01/01, and finally microsecond resolution over the
// residual of the 'bdlt::Date' range.
//
///Usage
///-----
// This component is not meant to be used directly and hence no usage example
// is provided.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bdlt {

                          // ======================
                          // struct DatetimeImpUtil
                          // ======================

struct DatetimeImpUtil {
    // This 'struct' provides a namespace for datetime encoding constants.

    // CLASS DATA

    static const bsls::Types::Int64  k_1970_01_01_TOTAL_SECONDS;
    static const bsls::Types::Int64  k_2470_01_01_TOTAL_SECONDS;

    static const bsls::Types::Uint64 k_0001_01_01_VALUE;
    static const bsls::Types::Uint64 k_1970_01_01_VALUE;
    static const bsls::Types::Uint64 k_2470_01_01_VALUE;
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
