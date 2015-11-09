// bsl_memory.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_MEMORY
#define INCLUDED_BSL_MEMORY

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

#include <memory>

// 'std::unique_ptr' is available:
//:  o GCC 4.4+, C++11 builds.
//:    https://gcc.gnu.org/gcc-4.4/changes.html
//:  o Clang, libc++, C++11 builds that have <forward_list> available
//:    http://stackoverflow.com/questions/31655462/
//:  o MSVC 2010+
//:    https://msdn.microsoft.com/en-us/library/ee410601(v=vs.100).aspx

#if defined(BSLS_PLATFORM_CMP_GNU) && defined(__GXX_EXPERIMENTAL_CXX0X__) &&  \
    BSLS_PLATFORM_CMP_VERSION >= 40400
#define BSL_MEMORY_SUPPORT_UNIQUE_PTR
#elif defined(BSLS_PLATFORM_CMP_CLANG) && __cplusplus >= 201103L
#if __has_include(<forward_list>)
#define BSL_MEMORY_SUPPORT_UNIQUE_PTR
#endif
#elif defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION >= 1600
#define BSL_MEMORY_SUPPORT_UNIQUE_PTR
#endif

namespace bsl
{
    // Import selected symbols into bsl namespace

    // These are declared in bslstl_iosfwd.h (so cannot be using declarations):
    //..
    //  using native_std::allocator;
    //..

    using native_std::auto_ptr;  // May not be available from C++17 libraries
    using native_std::get_temporary_buffer;
    using native_std::raw_storage_iterator;
    using native_std::return_temporary_buffer;
    using native_std::uninitialized_copy;
    using native_std::uninitialized_fill;
    using native_std::uninitialized_fill_n;

#if defined(BSL_MEMORY_SUPPORT_UNIQUE_PTR)
    using native_std::unique_ptr;
#endif


}  // close package namespace

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

#ifndef BSL_OVERRIDES_STD
#include <bslstl_allocator.h>
#include <bslstl_allocatortraits.h>
#include <bslstl_badweakptr.h>
#include <bslstl_ownerless.h>
#include <bslstl_sharedptr.h>
#endif

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
