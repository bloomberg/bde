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

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1600
# define BSL_CSTDINT_NO_STDINT
#endif

#ifndef BSL_CSTDINT_NO_STDINT
# include <stdint.h>

namespace bsl {

    using ::intmax_t;
    using ::uintmax_t;

    using ::int_least8_t;
    using ::int_least16_t;
    using ::int_least32_t;
    using ::int_least64_t;

    using ::uint_least8_t;
    using ::uint_least16_t;
    using ::uint_least32_t;
    using ::uint_least64_t;

    using ::int_fast8_t;
    using ::int_fast16_t;
    using ::int_fast32_t;
    using ::int_fast64_t;

    using ::uint_fast8_t;
    using ::uint_fast16_t;
    using ::uint_fast32_t;
    using ::uint_fast64_t;

    using ::int8_t;
    using ::int16_t;
    using ::int32_t;
    using ::int64_t;

    using ::uint8_t;
    using ::uint16_t;
    using ::uint32_t;
    using ::uint64_t;

    using ::intptr_t;
    using ::uintptr_t;

}  // close package namespace

#else

namespace bsl {

    typedef long long          intmax_t;
    typedef unsigned long long uintmax_t;

    typedef signed char        int_least8_t;
    typedef short              int_least16_t;
    typedef int                int_least32_t;
    typedef long long          int_least64_t;

    typedef unsigned char      uint_least8_t;
    typedef unsigned short     uint_least16_t;
    typedef unsigned int       uint_least32_t;
    typedef unsigned long long uint_least64_t;

    typedef signed char        int_fast8_t;
    typedef int                int_fast16_t;
    typedef int                int_fast32_t;
    typedef long long          int_fast64_t;

    typedef unsigned char      uint_fast8_t;
    typedef unsigned int       uint_fast16_t;
    typedef unsigned int       uint_fast32_t;
    typedef unsigned long long uint_fast64_t;

    typedef signed char        int8_t;
    typedef short              int16_t;
    typedef int                int32_t;
    typedef long long          int64_t;

    typedef unsigned char      uint8_t;
    typedef unsigned short     uint16_t;
    typedef unsigned int       uint32_t;
    typedef unsigned long long uint64_t;

# ifdef BSLS_PLATFORM_CPU_32_BIT
    typedef int                intptr_t;
    typedef unsigned int       uintptr_t;
# else
    typedef long long          intptr_t;
    typedef unsigned long long uintptr_t;
# endif

}  // close namespace bsl

#endif

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
