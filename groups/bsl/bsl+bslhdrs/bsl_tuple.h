// bsl_tuple.h                                                        -*-C++-*-
#ifndef INCLUDED_BSL_TUPLE
#define INCLUDED_BSL_TUPLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsl_cstddef.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>
#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

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

    using std::tuple_size;
    using std::tuple_element;
    using std::tuple;
    using std::make_tuple;
    using std::forward_as_tuple;
    using std::tie;
    using std::tuple_cat;
    using std::get;
    using std::ignore;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
template <std::size_t I, class TYPE> using tuple_element_t =
                             typename std::tuple_element<I, TYPE>::type;
        // 'tuple_element_t' is an alias to the return type of the
        // 'std::tuple_element' meta-function.  Note, that the
        // 'tuple_element_t' avoids the '::type' suffix and 'typename' prefix
        // when we want to use the result of the meta-function in templates.
#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    using std::tuple_size_v;
#elif defined BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    template <class TYPE>
    BSLS_KEYWORD_INLINE_VARIABLE
    constexpr bsl::size_t tuple_size_v = std::tuple_size<TYPE>::value;
        // This template variable represents the result value of the
        // 'std::tuple_size' meta-function.

#endif

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    using std::apply;
    using std::make_from_tuple;
#endif

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
