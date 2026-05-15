// bsl_stdfloat.h                                                     -*-C++-*-
#ifndef INCLUDED_BSL_STDFLOAT
#define INCLUDED_BSL_STDFLOAT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_compilerfeatures.h>
// The `<stdfloat>` is a feature of the C++23 library, rather than C++03, so
// might not be present in all native libraries on the platforms we support.
// Detect the native header using '__has_include' where available.  If we are
// not also in C++23 mode we don't try to include the file because some
// compilers issue warnings when we do.
#if BSLS_COMPILERFEATURES_CPLUSPLUS >= 202302L                                \
 && defined(BSLS_COMPILERFEATURES_SUPPORT_HAS_INCLUDE)
# if __has_include(<stdfloat>)
#   include <stdfloat>
# endif
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)  // `using`
// Create the type aliases based directly on language support rather than
// forwarding from a native library header that might not be up to date
// with the language support of compiler.  Note that all five alias
// optional types that are available if and only if the corresponding
// Standard feature detection macro is defined.
namespace bsl {

#if defined(__STDCPP_FLOAT16_T__)
    using float16_t = decltype(0.f16);
#endif
#if defined(__STDCPP_FLOAT32_T__)
    using float32_t = decltype(0.f32);
#endif
#if defined(__STDCPP_FLOAT64_T__)
    using float64_t = decltype(0.f64);
#endif
#if defined(__STDCPP_FLOAT128_T__)
    using float128_t = decltype(0.f128);
#endif
#if defined(__STDCPP_BFLOAT16_T__)
    using bfloat16_t = decltype(0.bf16);
#endif

}  // close package namespace
#endif // type aliases

#endif

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
