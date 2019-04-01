// bslstl_algorithmworkaround.h                                       -*-C++-*-
#ifndef INCLUDED_BSLSTL_ALGORITHMWORKAROUND
#define INCLUDED_BSLSTL_ALGORITHMWORKAROUND

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for work arounds for the standard algorithms
//
//@CLASSES:
//
//@SEE_ALSO: bsl+stdhdrs
//
//@AUTHOR: Arthur Chiu (achiu21)
//
//@DESCRIPTION: This component is for internal use only.  Please include
// '<bsl_algorithm>' instead.  This component provides a namespace for
// work-arounds for the platform supplied algorithm header (particularly for
// the Sun Studio libCstd).
//
///Usage
///-----
// This component is for use by the 'bsl+stdhdrs' package.  Use 'algorithm'
// directly.

// Prevent 'bslstl' headers from being included directly in 'BSL_OVERRIDES_STD'
// mode.  Doing so is unsupported, and is likely to cause compilation errors.
#if defined(BSL_OVERRIDES_STD) && !defined(BSL_STDHDRS_PROLOGUE_IN_EFFECT)
#error "include <bsl_algorithm.h> instead of <bslstl_algorithmworkaround.h> in\
 BSL_OVERRIDES_STD mode"
#endif
#include <bslscm_version.h>

#include <bslstl_iterator.h>  // iterator tags

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


// DRQS 139734639: The following works around a deficiency in the STLPort
// standard library that prevents using 'lower_bound' and 'upper_bound' to
// search for a value of a different type than that held in the range being
// searched (even if an appropriate 'operator<' is defined).


#if   defined(BSLS_LIBRARYFEATURES_STDCPP_STLPORT)                            \
   && defined(_STLPORT_VERSION)                                               \
   && (_STLPORT_VERSION <= 0x452)

template<class FORWARD_IT, class TYPE>
FORWARD_IT lower_bound(FORWARD_IT first, FORWARD_IT last, const TYPE& value)
{
    typedef typename std::iterator_traits<FORWARD_IT>::difference_type
        difference_type;

    difference_type length = std::distance(first, last);

    while (length > 0) {
        difference_type half = length >> 1;
        FORWARD_IT      it   = first;

        std::advance(it, half);
        if (*it < value) {
            first = ++it;
            length -= half + 1;
        }
        else {
            length = half;
        }
    }
    return first;
}

template <class FORWARD_IT, class TYPE, class COMPARE>
FORWARD_IT lower_bound(FORWARD_IT  first,
                       FORWARD_IT  last,
                       const TYPE& value,
                       COMPARE     comp)
{
    typedef typename std::iterator_traits<FORWARD_IT>::difference_type
        difference_type;

    difference_type length = std::distance(first, last);

    while (length > 0) {
        difference_type half = length >> 1;
        FORWARD_IT       it   = first;

        std::advance(it, half);
        if (comp(*it, value)) {
            first = ++it;
            length -= half + 1;
        }
        else {
            length = half;
        }
    }
    return first;
}

template<class FORWARD_IT, class TYPE>
FORWARD_IT upper_bound(FORWARD_IT first, FORWARD_IT last, const TYPE& value)
{
    typedef typename std::iterator_traits<FORWARD_IT>::difference_type
        difference_type;

    difference_type length = std::distance(first, last);

    while (length > 0) {
        difference_type half = length >> 1;
        FORWARD_IT      it   = first;

        std::advance(it, half);
        if (!(value < *it)) {
            first = ++it;
            length -= half + 1;
        }
        else {
            length = half;
        }
    }
    return first;
}

template <class FORWARD_IT, class TYPE, class COMPARE>
FORWARD_IT upper_bound(FORWARD_IT  first,
                       FORWARD_IT  last,
                       const TYPE& value,
                       COMPARE     comp)
{
    typedef typename std::iterator_traits<FORWARD_IT>::difference_type
        difference_type;

    difference_type length = std::distance(first, last);

    while (length > 0) {
        difference_type half = length >> 1;
        FORWARD_IT      it   = first;

        std::advance(it, half);
        if (!comp(value, *it)) {
            first = ++it;
            length -= half + 1;
        }
        else {
            length = half;
        }
    }
    return first;
}

#else

// On all other platforms, use the compiler vendor supplied version of
// 'lower_bound' and 'upper_bound'.

using native_std::lower_bound;
using native_std::upper_bound;
#endif


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
