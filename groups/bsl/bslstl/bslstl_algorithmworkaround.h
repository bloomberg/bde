// bslstl_algorithmworkaround.h                                       -*-C++-*-
#ifndef INCLUDED_BSLSTL_ALGORITHMWORKAROUND
#define INCLUDED_BSLSTL_ALGORITHMWORKAROUND

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for work arounds for the standard algorithms
//
//@CLASSES:
//
//@CANONICAL_HEADER: bsl_algorithm.h
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

#include <bslscm_version.h>

#include <bslstl_iterator.h>  // iterator tags
#include <bslstl_pair.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <algorithm>          // 'copy'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

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
    std::count(first, last, value, ret);
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
    std::count_if(first, last, pred, ret);
    return ret;
}

#else

// On all other platforms, use the compiler vendor supplied version of 'count'
// and 'count_if'.
using std::count;
using std::count_if;

#endif  // BSLS_PLATFORM_CMP_SUN && !BDE_BUILD_TARGET_STLPORT

#if defined (BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD)
    // Use definition from '<algorithm>'.
#else
template<class ForwardIt, class Searcher>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14
ForwardIt search( ForwardIt first, ForwardIt last,
                  const Searcher& searcher )
    // Return the position in the specified range '[first, last)' of the first
    // occurrence of the pattern sought by the specified 'searcher' if found,
    // and 'last' otherwise.  See [alg.search].
{
    bsl::pair<ForwardIt, ForwardIt> result = searcher(first, last);
    return result.first;
}
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_SEARCH_OVERLOAD

#if !defined(BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY)
#if defined(BSLS_LIBRARYFEATURES_STDCPP_MSVC)
    // Visual Studio (the versions we support) provides 'copy_if'.
    using std::copy_if;
#else
#define BSLSTL_ALGORITHMWORKAROUND_IMPLEMENTS_COPY_IF                         1
    // C++03 standard libraries do not provide 'std::copy_if' (as it was not
    // part of the C++03 standard), but it is actually implementable in C++03,
    // so we inject it here.

template <class InputIt, class OutputIt, class UnaryPred>
OutputIt copy_if(InputIt first, InputIt last, OutputIt result, UnaryPred pred)
    // Copy all elements in the half-open range of the specified 'first', and
    // 'last' ('[first, last)') input iterators for which the specified 'pred'
    // unary predicate is 'true' to the specified 'result' output iterator,
    // incrementing result after each copied element, keeping the element order
    // stable.  The behavior is undefined if the ranges '[first, last)' and
    // '[result, advance(result, distance(firt, last)))' overlap.  The behavior
    // is also undefined if 'pred' attempts to invoke any non-constant
    // functions of its argument.  See also [alg.copy] in the C++11 standard.
{
    while(first != last) {
        if (pred(*first)) {
            *result++ = *first;
        }
        ++first;
    }
    return result;
}
#endif
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
