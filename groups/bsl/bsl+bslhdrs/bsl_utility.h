// bsl_utility.h                                                      -*-C++-*-
#ifndef INCLUDED_BSL_UTILITY
#define INCLUDED_BSL_UTILITY

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

#include <utility>

namespace bsl {
    // Import selected symbols into bsl namespace.  Note that 'pair' is
    // provided by 'bslstl_pair.h' (but 'make_pair' currently is not).  Aslo
    // note that 'namespace rel_ops' is provided directly by the bsl prolog
    // header, so we provide an alias only when NOT included in a
    // 'BSL_OVERRIDES_STD' mode - the prolog header itself guards against the
    // dangers of such a (transitive) include path, and as that is one of its
    // key purposes, we do not provide additional (redundant) guards beyond
    // checking the build mode itself.

    using native_std::make_pair;

#if !defined(BSL_OVERRIDES_STD)
    namespace rel_ops = native_std::rel_ops;
#endif  // BSL_OVERRIDES_STD

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    using native_std::declval;
    using native_std::forward;
    using native_std::move;
    using native_std::move_if_noexcept;
    using native_std::swap;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR
    using native_std::piecewise_construct;
    using native_std::piecewise_construct_t;
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_PAIR_PIECEWISE_CONSTRUCTOR

}  // close package namespace

// Include Bloomberg's implementation, unless compilation is configured to
// override native types in the 'std' namespace with Bloomberg's
// implementation, in which case the implementation file will be included by
// the Bloomberg supplied standard header file.

#ifndef BSL_OVERRIDES_STD
#include <bslstl_pair.h>
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
