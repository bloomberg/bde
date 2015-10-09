// bdlc_packedintarrayutil.h                                          -*-C++-*-
#ifndef INCLUDED_BDLC_PACKEDINTARRAYUTIL
#define INCLUDED_BDLC_PACKEDINTARRAYUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide common non-primitive operations on 'bdlc::PackedIntArray'.
//
//@CLASSES:
//  bdlc::PackedIntArrayUtil: non-primitive 'bdlc::PackedIntArray' operations
//
//@SEE_ALSO: bdlc_packedintarray
//
//@DESCRIPTION: This component provides a 'struct', 'bdlc::PackedIntArrayUtil',
// that serves as a namespace for utility functions that operate on
// 'bdlc::PackedIntArray' objects.
//
// The following list of methods are provided by 'bdlc::PackedIntArrayUtil':
//..
//  'isSorted'         Returns 'true' if the range from a
//                     'bdlc::PackedIntArray' is sorted, and 'false' otherwise.
//
//  'lowerBound'       Returns an iterator to the first element in a sorted
//                     range from a 'bdlc::PackedIntArray' that compares
//                     greater than or equal to a specified value.
//
//  'upperBound'       Returns an iterator to the first element in a sorted
//                     range from a 'bdlc::PackedIntArray' that compares
//                     greater than a specified value.
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: 'lowerBound'
///- - - - - - - - - - - -
// Suppose that given a sorted 'bdlc::PackedIntArray', we want to find the
// first value greater than or equal to the value 17.  First, create and
// populate with sorted data the 'bdlc::PackedIntArray' to be searched:
//..
//  bdlc::PackedIntArray<int> array;
//
//  array.push_back( 5);
//  array.push_back( 9);
//  array.push_back(15);
//  array.push_back(19);
//  array.push_back(23);
//  array.push_back(36);
//  assert(6 == array.length());
//..
// Then, verify the array's data has sorted values:
//..
//  assert(bdlc::PackedIntArrayUtil::isSorted(array.begin(), array.end()));
//..
// Finally, use 'bdlc::PackedIntArrayUtil::lowerBound' to find the desired
// value:
//..
//  bdlc::PackedIntArrayConstIterator<int> iterator =
//                          bdlc::PackedIntArrayUtil::lowerBound(array.begin(),
//                                                               array.end(),
//                                                               17);
//  assert(iterator != array.end() && 19 == *iterator);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLC_PACKEDINTARRAY
#include <bdlc_packedintarray.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace bdlc {

                      // =========================
                      // struct PackedIntArrayUtil
                      // =========================

struct PackedIntArrayUtil {
    // This 'struct' provides a namespace for utility functions that provide
    // non-primitive operations on 'bdlc::PackedIntArray'.

  public:
    // CLASS METHODS
    template <class TYPE>
    static bool isSorted(PackedIntArrayConstIterator<TYPE> first,
                         PackedIntArrayConstIterator<TYPE> last);
        // Return 'true' if the range from the specified 'first' (inclusive) to
        // the specified 'last' (exclusive) is sorted or empty, and 'false'
        // otherwise.  The behavior is undefined unless 'first <= last'.

    template <class TYPE>
    static PackedIntArrayConstIterator<TYPE> lowerBound(
                                      PackedIntArrayConstIterator<TYPE> first,
                                      PackedIntArrayConstIterator<TYPE> last,
                                      TYPE                              value);
        // Return an iterator to the first element in the sorted range from the
        // specified 'first' (inclusive) to the specified 'last' (exclusive)
        // that compares greater than or equal to the specified 'value', and
        // 'last' if no such element exists.  The behavior is undefined unless
        // 'first <= last' and the range is sorted.

    template <class TYPE>
    static PackedIntArrayConstIterator<TYPE> upperBound(
                                      PackedIntArrayConstIterator<TYPE> first,
                                      PackedIntArrayConstIterator<TYPE> last,
                                      TYPE                              value);
        // Return an iterator to the first element in the sorted range from the
        // specified 'first' (inclusive) to the specified 'last' (exclusive)
        // that compares greater than the specified 'value', and 'last' if no
        // such element exists.  The behavior is undefined unless
        // 'first <= last' and the range is sorted.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                      // -------------------------
                      // struct PackedIntArrayUtil
                      // -------------------------

// CLASS METHODS
template <class TYPE>
bool PackedIntArrayUtil::isSorted(PackedIntArrayConstIterator<TYPE> first,
                                  PackedIntArrayConstIterator<TYPE> last)
{
    BSLS_ASSERT(first <= last);

    PackedIntArrayConstIterator<TYPE> at   = first;
    PackedIntArrayConstIterator<TYPE> prev = first;

    while (at < last) {
        if (*prev > *at) {
            return false;                                             // RETURN
        }
        prev = at++;
    }

    return true;
}

template <class TYPE>
PackedIntArrayConstIterator<TYPE> PackedIntArrayUtil::lowerBound(
                                       PackedIntArrayConstIterator<TYPE> first,
                                       PackedIntArrayConstIterator<TYPE> last,
                                       TYPE                              value)
{
    BSLS_ASSERT(first <= last);
    BSLS_ASSERT_SAFE(isSorted(first, last));

    typedef typename PackedIntArrayConstIterator<TYPE>::difference_type
                                                               difference_type;

    difference_type count = last - first;

    while (count > 0) {
        difference_type                   step = count / 2;
        PackedIntArrayConstIterator<TYPE> it   = first + step;

        if (*it < value) {
            first = ++it;
            count -= step + 1;
        }
        else {
            count = step;
        }
    }

    return first;
}

template <class TYPE>
PackedIntArrayConstIterator<TYPE> PackedIntArrayUtil::upperBound(
                                       PackedIntArrayConstIterator<TYPE> first,
                                       PackedIntArrayConstIterator<TYPE> last,
                                       TYPE                              value)
{
    BSLS_ASSERT(first <= last);
    BSLS_ASSERT_SAFE(isSorted(first, last));

    typedef typename PackedIntArrayConstIterator<TYPE>::difference_type
                                                               difference_type;

    difference_type count = last - first;

    while (count > 0) {
        difference_type                   step = count / 2;
        PackedIntArrayConstIterator<TYPE> it   = first + step;

        if (*it <= value) {
            first = ++it;
            count -= step + 1;
        }
        else {
            count = step;
        }
    }

    return first;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
