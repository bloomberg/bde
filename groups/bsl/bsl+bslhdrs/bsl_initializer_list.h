// bsl_initializer_list.h                                             -*-C++-*-
#ifndef INCLUDED_BSL_INITIALIZER_LIST
#define INCLUDED_BSL_INITIALIZER_LIST

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

#ifndef INCLUDED_BSLS_COMPILERFEATURES
#include <bsls_compilerfeatures.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#include <initializer_list>

namespace bsl {

// Libc++ for osx has a c++ 03 version of initializer_list that implements
// nothing, but can still be included.  Similarly, gcc versions prior to 4.7.
#if !(defined(BSLS_PLATFORM_OS_DARWIN) &&                                     \
     !defined(BSLS_COMPILERFEATURES_SUPPORT_GENERALIZED_INITIALIZERS))        \
 && !(defined(BSLS_PLATFORM_CMP_GNU) && (BSLS_PLATFORM_CMP_VERSION < 40700    \
             || defined(__GXX_EXPERIMENTAL_CXX0X__)))
using native_std::initializer_list;
#endif

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
