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

#if 201703L <= BSLS_COMPILERFEATURES_CPLUSPLUS
# include <iterator>    // 'size' in C++17, 'ssize' in C++20
#endif

#if BSLS_COMPILERFEATURES_CPLUSPLUS < 202002L
# include <bsls_keyword.h>
# include <cstddef>          // 'size_t', 'ptrdiff_t'

# if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)    &&                    \
     201103L <= BSLS_COMPILERFEATURES_SUPPORT_CPLUSPLUS

#   include <type_traits>    // 'common_type', 'make_signed'

# endif
#endif

namespace bsl {

                                    // ====
                                    // size
                                    // ====

#if BSLS_COMPILERFEATURES_CPLUSPLUS < 201703L

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

#else

using std::size;

#endif

                                    // =====
                                    // ssize
                                    // =====

#if BSLS_COMPILERFEATURES_CPLUSPLUS < 202002L
# if defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE)    &&                    \
     201103L <= BSLS_COMPILERFEATURES_SUPPORT_CPLUSPLUS

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

#else

using std::ssize;

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
