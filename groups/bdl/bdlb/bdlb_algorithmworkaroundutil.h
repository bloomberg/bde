// bdlb_algorithmworkaroundutil.h                                     -*-C++-*-
#ifndef INCLUDED_BDLB_ALGORITHMWORKAROUNDUTIL
#define INCLUDED_BDLB_ALGORITHMWORKAROUNDUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a namespace for workarounds for faulty standard algorithms
//
//@CLASSES:
//  AlgorithmWorkaroundUtil: namespace for fixed faulty standard algorithms
//
//@SEE_ALSO: std::lower_bound, std::upper_bound
//
//@DESCRIPTION: This component provides a 'struct' namespace for replacement
// workarounds for some platform supplied faulty algorithms (particularly for
// the Sun Studio compiler).
//
///Usage
///-----
// Suppose that we

#include <bdlscm_version.h>

#include <bsls_nativestd.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_iterator.h>

namespace BloombergLP {
namespace bdlb {

                    // ==============================
                    // struct AlgorithmWorkaroundUtil
                    // ==============================

struct AlgorithmWorkaroundUtil {
    // A namespace for replacement workarounds for some platform supplied
    // faulty algorithms (particularly for the Sun Studio compiler).

    // CLASS METHODS
    template<class FORWARD_IT, class TYPE>
    static
    FORWARD_IT lowerBound(FORWARD_IT  first,
                          FORWARD_IT  last,
                          const TYPE& value);
        // Returns an iterator pointing to the first element in the range
        // starting at the specified 'first' iterator and ending immediately
        // before the specified 'last' iterator, that is greater than or equal
        // to the specified 'value', or 'last' if no such element is found.
        // This method works around a deficiency in the STLPort standard
        // library that prevents using 'lower_bound' to search for a value of a
        // different type than that held in the range being searched (even if
        // an appropriate 'operator<' is defined).  See DRQS 139734639.

    template <class FORWARD_IT, class TYPE, class COMPARE>
    static
    FORWARD_IT lowerBound(FORWARD_IT  first,
                          FORWARD_IT  last,
                          const TYPE& value,
                          COMPARE     comp);
        // Returns an iterator pointing to the first element in the range
        // starting at the specified 'first' iterator and ending immediately
        // before the specified 'last' iterator, that is greater than or equal
        // to the specified 'value' as determined by the specified 'comp', or
        // 'last' if no such element is found.  This method works around a
        // deficiency in the STLPort standard library that prevents using
        // 'lower_bound' to search for a value of a different type than that
        // held in the range being searched (even if an appropriate 'operator<'
        // is defined).  See DRQS 139734639.

    template<class FORWARD_IT, class TYPE>
    static
    FORWARD_IT upperBound(FORWARD_IT  first,
                          FORWARD_IT  last,
                          const TYPE& value);
        // Returns an iterator pointing to the first element in the range
        // starting at the specified 'first' iterator and ending immediately
        // before the specified 'last' iterator, that is greater than the
        // specified 'value', or 'last' if no such element is found.  This
        // method works around a deficiency in the STLPort standard library
        // that prevents using 'upper_bound' to search for a value of a
        // different type than that held in the range being searched (even if
        // an appropriate 'operator<' is defined).  See DRQS 139734639.

    template <class FORWARD_IT, class TYPE, class COMPARE>
    static
    FORWARD_IT upperBound(FORWARD_IT  first,
                           FORWARD_IT  last,
                           const TYPE& value,
                           COMPARE     comp);
        // Returns an iterator pointing to the first element in the range
        // starting at the specified 'first' iterator and ending immediately
        // before the specified 'last' iterator, that is greater than the
        // specified 'value' as determined by the specified 'comp', or 'last'
        // if no such element is found.  This method works around a deficiency
        // in the STLPort standard library that prevents using 'upper_bound' to
        // search for a value of a different type than that held in the range
        // being searched (even if an appropriate 'operator<' is defined).  See
        // DRQS 139734639.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // ------------------------------
                    // struct AlgorithmWorkaroundUtil
                    // ------------------------------

template<class FORWARD_IT, class TYPE>
inline
FORWARD_IT AlgorithmWorkaroundUtil::lowerBound(FORWARD_IT first,
                                               FORWARD_IT last,
                                               const TYPE& value)
{
#if   defined(BSLS_LIBRARYFEATURES_STDCPP_STLPORT)                            \
    && defined(_STLPORT_VERSION)                                              \
    && (_STLPORT_VERSION <= 0x452)

    typedef typename bsl::iterator_traits<FORWARD_IT>::difference_type
        difference_type;

    difference_type length = bsl::distance(first, last);

    while (length > 0) {
        difference_type half = length >> 1;
        FORWARD_IT      it   = first;

        bsl::advance(it, half);
        if (*it < value) {
            first = ++it;
            length -= half + 1;
        }
        else {
            length = half;
        }
    }
    return first;
#else
    return native_std::lower_bound(first, last, value);
#endif
}

template <class FORWARD_IT, class TYPE, class COMPARE>
inline
FORWARD_IT AlgorithmWorkaroundUtil::lowerBound(FORWARD_IT  first,
                                               FORWARD_IT  last,
                                               const TYPE& value,
                                               COMPARE     comp)
{
#if   defined(BSLS_LIBRARYFEATURES_STDCPP_STLPORT)                            \
    && defined(_STLPORT_VERSION)                                              \
    && (_STLPORT_VERSION <= 0x452)

    typedef typename bsl::iterator_traits<FORWARD_IT>::difference_type
        difference_type;

    difference_type length = bsl::distance(first, last);

    while (length > 0) {
        difference_type half = length >> 1;
        FORWARD_IT       it   = first;

        bsl::advance(it, half);
        if (comp(*it, value)) {
            first = ++it;
            length -= half + 1;
        }
        else {
            length = half;
        }
    }
    return first;
#else
    return native_std::lower_bound(first, last, value, comp);
#endif
}

template<class FORWARD_IT, class TYPE>
inline
FORWARD_IT AlgorithmWorkaroundUtil::upperBound(FORWARD_IT  first,
                                               FORWARD_IT  last,
                                               const TYPE& value)
{
#if   defined(BSLS_LIBRARYFEATURES_STDCPP_STLPORT)                            \
    && defined(_STLPORT_VERSION)                                              \
    && (_STLPORT_VERSION <= 0x452)

    typedef typename bsl::iterator_traits<FORWARD_IT>::difference_type
        difference_type;

    difference_type length = bsl::distance(first, last);

    while (length > 0) {
        difference_type half = length >> 1;
        FORWARD_IT      it   = first;

        bsl::advance(it, half);
        if (!(value < *it)) {
            first = ++it;
            length -= half + 1;
        }
        else {
            length = half;
        }
    }
    return first;
#else
    return native_std::upper_bound(first, last, value);
#endif
}

template <class FORWARD_IT, class TYPE, class COMPARE>
inline
FORWARD_IT AlgorithmWorkaroundUtil::upperBound(FORWARD_IT  first,
                                               FORWARD_IT  last,
                                               const TYPE& value,
                                               COMPARE     comp)
{
#if   defined(BSLS_LIBRARYFEATURES_STDCPP_STLPORT)                            \
    && defined(_STLPORT_VERSION)                                              \
    && (_STLPORT_VERSION <= 0x452)

    typedef typename bsl::iterator_traits<FORWARD_IT>::difference_type
        difference_type;

    difference_type length = bsl::distance(first, last);

    while (length > 0) {
        difference_type half = length >> 1;
        FORWARD_IT      it   = first;

        bsl::advance(it, half);
        if (!comp(value, *it)) {
            first = ++it;
            length -= half + 1;
        }
        else {
            length = half;
        }
    }
    return first;
#else
    return native_std::upper_bound(first, last, value, comp);
#endif
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2019 Bloomberg Finance L.P.
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
