// bdlt_datetimeimputil.h                                            -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIMEIMPUTIL
#define INCLUDED_BDLT_DATETIMEIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE:
//
//@CLASSES:
//
//@DESCRIPTION: This component implements a utility class,
// 'bdlt::DatetimeImpUtil', that provides
//
///Usage
///-----

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
    // This 'struct' provides

  public:
    // CLASS DATA

    static const bsls::Types::Uint64 k_1970_01_01_TOTAL_SECONDS;
    static const bsls::Types::Uint64 k_2470_01_01_TOTAL_SECONDS;

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
