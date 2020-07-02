// bslstl_size.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLSTL_SIZE
#define INCLUDED_BSLSTL_SIZE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide 'size' and 'ssize' free functions
//
//@FUNCTIONS:
//  size_t bsl::size(<array>)
//  size_t bsl::size(<container>)
//  ptrdiff_t bslstl::ssize(<array>)
//  ptrdiff_t bslstl::ssize(<container>)
//
//@DESCRIPTION: This component provides the free functions 'size' and 'ssize',
// two overloads of each one, the first to take a contain that has a 'size'
// accessor, and the second
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

#if 201703L <= BSLS_COMPILERFEATURES_CPLUSPLUS
# include <string>    // 'size', 'ssize'
#endif

#if BSLS_COMPILERFEATURES_CPLUSPLUS <= 201703L
  // This is considered pre-C++20 code.  At the time of this writing, the exact
  // number of '__cplusplus' to check for the 2020 release was unknown.

# include <bsls_nativestd.h>

# include <cstddef>    // 'size_t'

#endif

namespace bsl {

#if BSLS_COMPILERFEATURES_CPLUSPLUS < 201703L

                                    // ----
                                    // size
                                    // ----

template <class CONTAINER>
inline
native_std::size_t size(const CONTAINER& container)
    // Return the 'size' field from the specified 'container's 'size' accessor.
{
    return container.size();
}

template <class TYPE, native_std::size_t DIMENSION>
inline
native_std::size_t size(const TYPE (&array)[DIMENSION])
    // Return the dimension of the specified 'array'.
{
    (void) array;

    return DIMENSION;
}

#else

using native_std::size;

#endif

#if BSLS_COMPILERFEATURES_CPLUSPLUS <= 201703L
// This is considered pre-C++20 code.  At the time of this writing, the exact
// number of '__cplusplus' to check for the 2020 release was unknown.

template <class CONTAINER>
inline
native_std::ptrdiff_t ssize(const CONTAINER& container)
    // Return the 'size' field from the specified 'container's 'size' accessor.
{
    return container.size();
}

template <class TYPE, native_std::ptrdiff_t DIMENSION>
inline
native_std::ptrdiff_t ssize(const TYPE (&array)[DIMENSION])
    // Return the dimension of the specified 'array'.
{
    (void) array;

    return DIMENSION;
}

#else

using native_std::ssize;

#endif

}  // close namespace bsl

#endif

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
