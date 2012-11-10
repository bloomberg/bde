// bslstl_iteratorutil.h                                              -*-C++-*-
#ifndef INCLUDED_BSLSTL_ITERATORUTIL
#define INCLUDED_BSLSTL_ITERATORUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities operating on iterators and iterator ranges.
//
//@CLASSES:
//  bslstl::IteratorUtil:
//
//@SEE_ALSO: bslstl_hashtable
//
//@DESCRIPTION: This component provides a namespace, 'bslstl::IteratorUtil',
// containing utility functions for iterator types.  In particular this
// component includes a function 'insertDistance' that returns the the
// number of elements that should be accounted for when range-inserting in a
// container, given a pair of iterator 'a' and 'b' describing a half-open range
// '[a,b)'.
//
///Usage
///-----

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSTL_ITERATOR
#include <bslstl_iterator.h>  // iterator tags
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_ALGORITHM
#include <algorithm>
#define INCLUDED_ALGORITHM
#endif

namespace BloombergLP {
namespace bslstl {

                        // ===================
                        // struct IteratorUtil
                        // ===================

struct IteratorUtil {
    // This utility struct provides a namespace for functions on iterators and
    // iterator ranges.

    // generic utility that needs a non-template hosted home
    template <class InputIterator>
    static native_std::size_t insertDistance(InputIterator first,
                                             InputIterator last);
        // Return 0 if InputIterator really is limited to the standard
        // input-iterator category, otherwise return the distance from first
        // to last.
};

// ============================================================================
//                      TEMPLATE AND INLINE FUNCTION DEFINITIONS
// ============================================================================

                    // ------------------
                    // class IteratorUtil
                    // ------------------

template <class InputIterator>
native_std::size_t IteratorUtil::insertDistance(InputIterator first,
                                                InputIterator last)
{
#if defined(BSLS_PLATFORM__CMP_SUN)
    // Need to work around Sun library broken treatment of iterator tag
    // dispatch.
    return 0;
#else
    struct impl {
        // This local class provides a utility to estimate the maximum
        // number of elements that may be inserted by a range-insert
        // operation on a standard container, by performing tag dispatch
        // on the iterator's category type.

        static native_std::size_t calc(InputIterator, // first
                                       InputIterator, // last
                                       native_std::input_iterator_tag)
        {
            return 0;
        }

        static native_std::size_t calc(InputIterator first,
                                       InputIterator last,
                                       native_std::forward_iterator_tag)
        {
            return native_std::distance(first, last);
        }
    };

    typedef typename bsl::iterator_traits<InputIterator>::iterator_category
                                                                  IterCategory;
    return impl::calc(first, last, IterCategory());
#endif
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
