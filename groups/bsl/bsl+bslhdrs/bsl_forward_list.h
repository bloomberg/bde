// bsl_forward_list.h                                                 -*-C++-*-
#ifndef INCLUDED_BSL_FORWARD_LIST
#define INCLUDED_BSL_FORWARD_LIST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide functionality of the corresponding C++ Standard header.
//
//@SEE_ALSO: bslim_bslstandardheadertest
//
//@DESCRIPTION: Provide types, in the 'bsl' namespace, equivalent to those
// defined in the corresponding C++ standard header.  Include the native
// compiler-provided standard header, and also directly include Bloomberg's
// implementation of the C++ standard type (if one exists).  Finally, place the
// included symbols from the 'std' namespace (if any) into the 'bsl' namespace.

#include <bsls_libraryfeatures.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
# include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
# include <forward_list>

// According to C++11 Standard (24.6.5 range access) some functions ('begin',
// 'cbegin', etc.) must be available not only via inclusion of the <iterator>
// header, but also when <forward_list> is included.  To satisfy this
// requirement the following inclusion is added.
# include <bslstl_iterator.h>

# include <bslma_bslallocator.h>

namespace bsl {
// BDE does not provide its own implementation of an allocator-aware
// 'forward_list'.  Instead, it relies on the native implementation functioning
// correctly when 'bsl::allocator' is installed by specializations in the 'bsl'
// namespace.  This requires the native implementation to have a conforming
// implementation of 'std::allocator_traits', and for the native
// implementation of 'foward_list' to defer all allocation and element
// construction to the traits.

    template <class TYPE>
    using forward_list = std::forward_list<TYPE, bsl::allocator<TYPE>>;
}  // close package namespace
#endif  // C++11

#endif  // INCLUDED_BSL_FORWARD_LIST

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
