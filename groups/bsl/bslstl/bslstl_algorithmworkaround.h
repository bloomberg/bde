// bslstl_algorithmworkaround.h                                       -*-C++-*-
#ifndef INCLUDED_BSLSTL_ALGORITHMWORKAROUND
#define INCLUDED_BSLSTL_ALGORITHMWORKAROUND

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for work arounds for the standard algorithms
//
//@CLASSES:
//
//@SEE_ALSO: bos+stdhdrs
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_algorithm>' instead.  This component provides a namespace for
// work-arounds for the platform supplied algorithm header (particularly for
// the Sun Studio libCstd).
//
///Usage
///-----
// This component is for use by the 'bos+stdhdrs' package.  Use 'algorithm'
// directly.

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BOS_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_algorithm.h> instead of <bslstl_algorithmworkaround.h> in\
 BSL_OVERRIDES_STD mode"
#endif
#include <bslscm_version.h>

#include <bslstl_iterator.h>  // iterator tags
#include <bslstl_pair.h>

#include <bsls_keyword.h>
#include <bsls_nativestd.h>
#include <bsls_platform.h>

#include <algorithm>          // 'copy'

namespace bsl {

#if defined(BSLSTL_ITERATOR_PROVIDE_SUN_CPP98_FIXES)

// Provide an override for 'count' since Sun only provides a 4 argument version
// while the C++ standard requires a 3 argument version.
template <class InputIter, class TYPE>
inline
typename iterator_traits<InputIter>::difference_type
count(InputIter first, InputIter last, const TYPE& value)
{
    typename iterator_traits<InputIter>::difference_type ret = 0;
    native_std::count(first, last, value, ret);
    return ret;
}

// Provide an override for 'count_if' since Sun only provides a 4 argument
// version while the C++ standard requires a 3 argument version.
template <class InputIter, class PREDICATE>
inline
typename iterator_traits<InputIter>::difference_type
count_if(InputIter first, InputIter last, PREDICATE pred)
{
    typename iterator_traits<InputIter>::difference_type ret = 0;
    native_std::count_if(first, last, pred, ret);
    return ret;
}

#else

// On all other platforms, use the compiler vendor supplied version of 'count'
// and 'count_if'.
using native_std::count;
using native_std::count_if;

#endif  // BSLS_PLATFORM_CMP_SUN && !BDE_BUILD_TARGET_STLPORT

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_ALGORITHM
    // Use definition from '<algorithm>'.
#else
template<class ForwardIt, class Searcher>
inline
BSLS_KEYWORD_CONSTEXPR_RELAXED
ForwardIt search( ForwardIt first, ForwardIt last,
                  const Searcher& searcher )
{
    bsl::pair<ForwardIt, ForwardIt> result = searcher(first, last);
    return result.first;
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_ALGORITHM

}  // close namespace bsl

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
