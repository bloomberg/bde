// bsl_tuple.h                                                        -*-C++-*-
#ifndef INCLUDED_BSL_TUPLE
#define INCLUDED_BSL_TUPLE

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

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#if __cplusplus < 201103L \
    && (defined(BSLS_PLATFORM_CMP_SUN) || defined(BSLS_PLATFORM_CMP_IBM))
#   error This file requires compiler and library support for \
          the ISO C++ 2011 standard.
#endif

#include <tuple>

#define BSL_TUPLE_SUPPORTS_TUPLE 1

// Note the following logic assumes that if the preceding '#include <tuple>'
// succeeded, 'std::tuple' is generally available with the exceptions:
//  o libc++ for OSX has a C++03 version of tuple that is not complete.
//  o GCC versions prior to 4.7 are not standard conforming (do not support
//    forward_as_tuple)

#if defined(BSLS_PLATFORM_CMP_CLANG) &&                                       \
    !defined(BSLS_COMPILERFEATURES_SUPPORT_VARIADIC_TEMPLATES)
#undef BSL_TUPLE_SUPPORTS_TUPLE
#elif defined(BSLS_PLATFORM_CMP_GNU) && BSLS_PLATFORM_CMP_VERSION <= 40700
#undef BSL_TUPLE_SUPPORTS_TUPLE
#endif


#if defined(BSL_TUPLE_SUPPORTS_TUPLE)
namespace bsl {

    using native_std::tuple_size;
    using native_std::tuple_element;
    using native_std::tuple;
    using native_std::make_tuple;
    using native_std::forward_as_tuple;
    using native_std::tie;
    using native_std::tuple_cat;
    using native_std::get;
    using native_std::ignore;
}  // close namespace bsl
#endif

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

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
