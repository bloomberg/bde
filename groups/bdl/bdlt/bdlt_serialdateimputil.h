// bdlt_serialdateimputil.h                                           -*-C++-*-
#ifndef INCLUDED_BDLT_SERIALDATEIMPUTIL
#define INCLUDED_BDLT_SERIALDATEIMPUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide low-level support functions for date-value manipulation.
//
//@CLASSES:
// bdlt::SerialDateImpUtil: suite of low-level date-related stateless functions
//
//@SEE_ALSO: bdlt_posixdateimputil, bdlt_prolepticdateimputil
//
//@DESCRIPTION: This component defines a 'typedef' to a class that implements
// the (documentation-only) protocol for serial date implementation utilities
// -- each class implements a common set of methods, each having the same
// signature, and each having the same contract.  Currently, two
// implementations are allowed: 'bdlt_posixdateimputil' (required in Bloomberg
// code) and 'bdlt_prolepticdateimputil' (an option for open source code).

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif


#ifdef BDE_USE_PROLEPTIC_DATES
#ifndef INCLUDED_BDLT_PROLEPTICDATEIMPUTIL
#include <bdlt_prolepticdateimputil.h>
#endif
#else
#ifndef INCLUDED_BDLT_POSIXDATEIMPUTIL
#include <bdlt_posixdateimputil.h>
#endif
#endif

namespace BloombergLP {
namespace bdlt {

                           // ========================
                           // struct SerialDateImpUtil
                           // ========================

#ifdef BDE_USE_PROLEPTIC_DATES
    typedef ProlepticDateImpUtil SerialDateImpUtil;
#else
    typedef     PosixDateImpUtil SerialDateImpUtil;

    // DEPRECATED: typedef to temporarily support residual uses of
    // 'DelegatingDateImpUtil'.
    typedef     PosixDateImpUtil DelegatingDateImpUtil;
#endif

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================


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
