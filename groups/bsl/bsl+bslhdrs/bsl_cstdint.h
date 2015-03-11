// bsl_cstdint.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_CSTDINT
#define INCLUDED_BSL_CSTDINT

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

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#include <cstdint>

namespace bsl {
    using native_std::intmax_t;
    using native_std::uintmax_t;
    using native_std::int_least8_t;
    using native_std::int_least16_t;
    using native_std::int_least32_t;
    using native_std::int_least64_t;
    using native_std::uint_least8_t;
    using native_std::uint_least16_t;
    using native_std::uint_least32_t;
    using native_std::uint_least64_t;
    using native_std::int_fast8_t;
    using native_std::int_fast16_t;
    using native_std::int_fast32_t;
    using native_std::int_fast64_t;
    using native_std::uint_fast8_t;
    using native_std::uint_fast16_t;
    using native_std::uint_fast32_t;
    using native_std::uint_fast64_t;

#ifdef optional_cstdint_features
    using native_std::int8_t;
    using native_std::int16_t;
    using native_std::int32_t;
    using native_std::int64_t;
    using native_std::uint8_t;
    using native_std::uint16_t;
    using native_std::uint32_t;
    using native_std::uint64_t;
    using native_std::intptr_t;
    using native_std::uintptr_t;
#endif

}  // close namespace bsl

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
