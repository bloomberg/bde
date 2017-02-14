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

#ifndef INCLUDED_BSLS_LIBRARYFEATURES
#include <bsls_libraryfeatures.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#include <memory>

namespace bsl {
    // Import selected symbols into bsl namespace

    // These are declared in bslstl_iosfwd.h (so cannot be using declarations):
    //..
    //  using native_std::allocator;
    //..

    using native_std::get_temporary_buffer;
    using native_std::raw_storage_iterator;
    using native_std::return_temporary_buffer;
    using native_std::uninitialized_copy;
    using native_std::uninitialized_fill;
    using native_std::uninitialized_fill_n;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
    using native_std::auto_ptr;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    using native_std::addressof;
    using native_std::pointer_traits;
    using native_std::uninitialized_copy_n;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_STANDARD_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
    using native_std::unique_ptr;
    using native_std::default_delete;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    using native_std::align;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
    using native_std::declare_no_pointers;
    using native_std::declare_reachable;
    using native_std::get_pointer_safety;
    using native_std::pointer_safety;
    using native_std::undeclare_no_pointers;
    using native_std::undeclare_reachable;
#endif // BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API


}  // close package namespace

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

#ifndef BSL_OVERRIDES_STD
#include <bslma_allocatortraits.h>
#include <bslma_stdallocator.h>
#include <bslmf_allocatorargt.h>
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
