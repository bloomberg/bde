// bsl_memory.h                                                       -*-C++-*-
#ifndef INCLUDED_BSL_MEMORY
#define INCLUDED_BSL_MEMORY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: package bos+stdhdrs in the bos package group
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_libraryfeatures.h>

#include <memory>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {
    // Import selected symbols into bsl namespace

    // These are declared in bslstl_iosfwd.h (so cannot be using declarations):
    //..
    //  using std::allocator;
    //..

    using std::get_temporary_buffer;
    using std::raw_storage_iterator;
    using std::return_temporary_buffer;
    using std::uninitialized_copy;
    using std::uninitialized_fill;
    using std::uninitialized_fill_n;

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR
    using std::auto_ptr;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP98_AUTO_PTR

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    using std::addressof;
    using std::pointer_traits;
    using std::uninitialized_copy_n;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_STANDARD_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR
    using std::unique_ptr;
    using std::default_delete;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_UNIQUE_PTR

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY
    using std::make_unique;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP14_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
    using std::destroy;
    using std::destroy_at;
    using std::destroy_n;
    using std::uninitialized_default_construct;
    using std::uninitialized_default_construct_n;
    using std::uninitialized_move;
    using std::uninitialized_move_n;
    using std::uninitialized_value_construct;
    using std::uninitialized_value_construct_n;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES
    using std::align;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_MISCELLANEOUS_UTILITIES

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API
    using std::declare_no_pointers;
    using std::declare_reachable;
    using std::get_pointer_safety;
    using std::pointer_safety;
    using std::undeclare_no_pointers;
    using std::undeclare_reachable;
#endif // BSLS_LIBRARYFEATURES_HAS_CPP11_GARBAGE_COLLECTION_API

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
    // Import additional names expected by existing code, but not mandated by
    // the standard header.  This may get tricky if some standard library
    // happens to not require any of these names for its native implementation
    // of the <memory> header.

    using std::advance;
    using std::bad_alloc;
    using std::bidirectional_iterator_tag;
    using std::forward_iterator_tag;
    using std::input_iterator_tag;
    using std::iterator;
    using std::new_handler;
    using std::nothrow;
    using std::nothrow_t;
    using std::output_iterator_tag;
    using std::new_handler;
    using std::random_access_iterator_tag;
    using std::set_new_handler;
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

}  // close package namespace

// Include Bloomberg's implementation.
#include <bslma_allocatortraits.h>
#include <bslma_stdallocator.h>

#include <bslmf_allocatorargt.h>

#include <bslstl_badweakptr.h>
#include <bslstl_ownerless.h>
#include <bslstl_sharedptr.h>

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
