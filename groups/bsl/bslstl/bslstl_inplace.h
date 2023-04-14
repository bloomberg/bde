// bslstl_inplace.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLSTL_INPLACE
#define INCLUDED_BSLSTL_INPLACE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard-compliant in place construction tag types.
//
//@CLASSES:
//  bsl::in_place_t: tag type for in-place construction
//
//@DESCRIPTION: This component provides an implementation of a standard
// compliant tag type for in-place construction, 'bsl::in_place_t'.  This tag
// type is used in constructors of 'bsl::optional' to indicate that the
// contained object should be constructed in-place.  Note that the standard
// currently has two other in-place construction tag types,
// 'std::in_place_type_t' and 'std::in_place_index_t'. There is currently no
// need to provide the equivalent tag types in bsl, but if such need arises,
// the name of this header has been chosen for the purpose of being the
// designated place for all in-place construction tags.

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY
#include <utility>  // for std::in_place_t
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

namespace bsl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

using in_place_t = std::in_place_t;
using std::in_place;

#else

                              // ================
                              // class in_place_t
                              // ================
struct in_place_t {
    // This trivial tag type is passed to the constructors of types that
    // contain a single object to indicate that the contained object should be
    // constructed in-place.

    // CREATORS
    explicit BSLS_KEYWORD_CONSTEXPR in_place_t() BSLS_KEYWORD_NOEXCEPT;
        // Create an 'in_place_t' value.
};

// CREATORS
inline
BSLS_KEYWORD_CONSTEXPR in_place_t::in_place_t() BSLS_KEYWORD_NOEXCEPT
{
    // This 'constexpr' function has to be defined before initialializing the
    // 'constexpr' value, 'in_place', below.
}

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES)
inline constexpr in_place_t in_place = in_place_t();
#else
extern const in_place_t in_place;
#endif
    // Value of type 'in_place_t' used as an argument to functions that take an
    // 'in_place_t' argument.

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

}  // close namespace bsl

#endif  // INCLUDED_BSLSTL_INPLACE

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
