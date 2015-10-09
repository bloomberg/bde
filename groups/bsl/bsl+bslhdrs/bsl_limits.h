// bsl_limits.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_LIMITS
#define INCLUDED_BSL_LIMITS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bsl+stdhdrs
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <limits>

namespace bsl
{
    // Import selected symbols into bsl namespace

    using native_std::denorm_absent;
    using native_std::denorm_indeterminate;
    using native_std::denorm_present;
    using native_std::float_denorm_style;
    using native_std::float_round_style;
    using native_std::numeric_limits;
    using native_std::round_indeterminate;
    using native_std::round_to_nearest;
    using native_std::round_toward_infinity;
    using native_std::round_toward_neg_infinity;
    using native_std::round_toward_zero;
}  // close package namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
