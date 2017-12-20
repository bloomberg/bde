// bsl_chrono.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_CHRONO
#define INCLUDED_BSL_CHRONO

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

#include <chrono>

namespace bsl {

    namespace chrono {
        using native_std::chrono::duration;
        using native_std::chrono::time_point;
        using native_std::chrono::system_clock;
        using native_std::chrono::steady_clock;
        using native_std::chrono::high_resolution_clock;
        using native_std::chrono::treat_as_floating_point;
        using native_std::chrono::duration_values;
        using native_std::chrono::duration_cast;
        using native_std::chrono::time_point_cast;
        using native_std::chrono::hours;
        using native_std::chrono::minutes;
        using native_std::chrono::seconds;
        using native_std::chrono::milliseconds;
        using native_std::chrono::microseconds;
        using native_std::chrono::nanoseconds;
    }  // close namespace chrono
}  // close package namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
