// bslmf_removeextent.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEEXTENT
#define INCLUDED_BSLMF_REMOVEEXTENT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Metafunction removes one array dimension from an array type
//
//@CLASSES: bsl::remove_extent<TYPE>
//
//@SEE_ALSO: bslmf_decay
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provides a metafunction 'bsl::remove_extent'
// that returns the element type of an array.  The functionality is intended
// to be identical to the C++11 metafunction 'std::remove_extent'.  From the
// C++14 standard:
//
// If 'T' names a type “array of 'U'”, the member typedef 'type' shall be 'U',
// otherwise 'T'. [ *Note:* For multidimensional arrays, only the first array
// dimension is removed. For a type “array of 'const U'”, the resulting type
// is 'const U'. -- *end note* ]
//
//
///Usage
///-----
// For 'T' being a scalar, one-dimentional array of either known and
// unknown bounds, or two-dimentional array where the lower bound is
// known, we see that 'bsl::remove_extent<T>::type' is 'T' but with
// the high-order array dimension (if any), stripped:
//..
//  assert((bsl::is_same<int, bsl::remove_extent<int>::type>::value));
//  assert((bsl::is_same<int, bsl::remove_extent<int[]>::type>::value));
//  assert((bsl::is_same<int, bsl::remove_extent<int[5]>::type>::value));
//  typedef const int iarray2[][6];
//  assert((bsl::is_same<const int[6],
//                       bsl::remove_extent<iarray2>::type>::value));
//  assert((bsl::is_same<int[6],
//                       bsl::remove_extent<int[7][6]>::type>::value));
//..
// Note that if 'T' is a reference-to-array, then
// 'bsl::remove_extent<T>::type' is simply 'T', i.e., the reference
// suppresses the transformation:
//..
//  assert((bsl::is_same<int(&)[],
//                       bsl::remove_extent<int(&)[]>::type>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_CSTDLIB
#include <cstdlib>
#define INCLUDED_CSTDLIB
#endif

namespace bsl {

                        // ============================
                        // class template remove_extent
                        // ============================

template <class TYPE>
struct remove_extent {
    // From the C++14 standard: If 'T' names a type "array of 'U'", the member
    // typedef 'type' shall be 'U', otherwise 'T'. [ *Note:* For
    // multidimensional arrays, only the first array dimension is removed. For
    // a type “array of 'const U'”, the resulting type is 'const U'. --
    // *end note* ]

    typedef TYPE type;
};

template <class TYPE>
struct remove_extent<TYPE[]> {
    // Specialization of 'remove_extent' for array of unknown bound

    typedef TYPE type;
};

template <class TYPE, std::size_t SZ>
struct remove_extent<TYPE[SZ]> {
    // Specialization of 'remove_extent' for array of known bound

    typedef TYPE type;
};

}  // close namespace bsl

#endif // ! defined(INCLUDED_BSLMF_REMOVEEXTENT)

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
