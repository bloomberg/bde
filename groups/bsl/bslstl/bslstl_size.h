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
// two overloads of each one, the first to take a container that has a 'size'
// accessor, and the second to take an array, in both cases to return the
// number of elements in the array or container.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1:
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#if 201703L <= BSLS_COMPILERFEATURES_CPLUSPLUS
# define BSLSTL_SIZE_SIZE_NATIVE 1
#else
# define BSLSTL_SIZE_SIZE_NATIVE 0
#endif

// We have observed that clang-10.0.0 does not suppose 'std::ssize' in C++20,
// and we are speculating that later releases of the compiler will support it.

#if 202002L <= BSLS_COMPILERFEATURES_CPLUSPLUS &&                             \
    (!defined(BSLS_PLATFORM_CMP_CLANG) ||                                     \
        (BSLS_PLATFORM_CMP_CLANG && 100000 < BSLS_PLATFORM_CMP_VERSION))
# define BSLSTL_SIZE_SSIZE_NATIVE 1
#else
# define BSLSTL_SIZE_SSIZE_NATIVE 0
#endif

#if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)    &&                    \
    201103L <= BSLS_COMPILERFEATURES_SUPPORT_CPLUSPLUS
# define BSLSTL_SIZE_SSIZE_ADVANCED_IMPL 1
#else
# define BSLSTL_SIZE_SSIZE_ADVANCED_IMPL 0
#endif

#if BSLSTL_SIZE_SIZE_NATIVE
# include <iterator>    // 'size' in C++17, 'ssize' in C++20
#endif

#if !BSLSTL_SIZE_SSIZE_NATIVE
# include <bsls_keyword.h>
# include <cstddef>          // 'size_t', 'ptrdiff_t'
# if BSLSTL_SIZE_SSIZE_ADVANCED_IMPL
#   include <type_traits>    // 'common_type', 'make_signed'
# endif
#endif

namespace bsl {

                                    // ====
                                    // size
                                    // ====

#if BSLSTL_SIZE_SIZE_NATIVE

using std::size;

#else

# if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)

template <class CONTAINER>
inline
BSLS_KEYWORD_CONSTEXPR auto size(const CONTAINER& container) ->
                                                     decltype(container.size())
    // Return the 'size' field from the specified 'container's 'size' accessor.
{
    return container.size();
}

# else

template <class CONTAINER>
inline
BSLS_KEYWORD_CONSTEXPR std::size_t size(const CONTAINER& container)
    // Return the 'size' field from the specified 'container's 'size' accessor.
{
    return container.size();
}

# endif

template <class TYPE, std::size_t DIMENSION>
inline
BSLS_KEYWORD_CONSTEXPR std::size_t size(const TYPE (&)[DIMENSION])
    // Return the dimension of the specified 'array'.
{
    return DIMENSION;
}

#endif

                                    // =====
                                    // ssize
                                    // =====

#if BSLSTL_SIZE_SSIZE_NATIVE

using std::ssize;

#else

# if BSLSTL_SIZE_SSIZE_ADVANCED_IMPL

template <class CONTAINER>
inline
BSLS_KEYWORD_CONSTEXPR auto ssize(const CONTAINER& container) ->
     std::common_type<std::ptrdiff_t,
                      std::make_signed<decltype(container.size())>::type>::type
    // Return the 'size' field from the specified 'container's 'size' accessor.
{
    return container.size();
}

# else

template <class CONTAINER>
inline
BSLS_KEYWORD_CONSTEXPR std::ptrdiff_t ssize(const CONTAINER& container)
    // Return the 'size' field from the specified 'container's 'size' accessor.
{
    return container.size();
}

# endif

template <class TYPE, std::ptrdiff_t DIMENSION>
inline
BSLS_KEYWORD_CONSTEXPR std::ptrdiff_t ssize(const TYPE (&)[DIMENSION])
    // Return the dimension of the specified 'array'.
{
    return DIMENSION;
}

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
