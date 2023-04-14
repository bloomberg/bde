// bslmf_removeextent.h                                               -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEEXTENT
#define INCLUDED_BSLMF_REMOVEEXTENT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a metafunction to return an array type's element type.
//
//@CLASSES:
//  bsl::remove_extent: type trait that returns the element type of an array
//  bsl::remove_extent_t: alias to the return type of the 'bsl::remove_extent'
//
//@SEE_ALSO: bslmf_decay
//
//@DESCRIPTION: This component provides a metafunction 'bsl::remove_extent'
// that returns the element type of an array.  The functionality is intended to
// be identical to the C++11 metafunction 'std::remove_extent'.  From the C++14
// standard:
//
// If 'T' names a type "array of 'U'", the member typedef 'type' shall be 'U',
// otherwise 'T'.  [ *Note:* For multidimensional arrays, only the first array
// dimension is removed.  For a type "array of 'const U'", the resulting type
// is 'const U'.  -- *end note* ]
//
///Usage
///-----
// The class template 'Traverser' is used to traverse an array and perform some
// operation.  In order to do its job in the case of two-dimensional arrays,
// 'Traverser' must hold on to an entire row of the array at a time in order to
// process it correctly.  The row type is determined from the array type using
// 'remove_extent':
//..
//  template <class ARRAY_TYPE>
//  class Traverser {
//  public:
//
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//..
// Note that if the current compiler supports alias templates C++11 feature, we
// can use 'bsl::remove_extent_t' alias to the "result" type of the
// 'bsl::remove_extent' meta-function, that avoids the '::type' suffix and
// 'typename' prefix in the declaration of the function return type:
//..
//      using RowType = bsl::remove_extent_t<ARRAY_TYPE>;
//#else
//      typedef typename bsl::remove_extent<ARRAY_TYPE>::type RowType;
//#endif
//
//  private:
//      RowType d_row;  // Might be scalar
//      // ...
//  };
//..
// Now we can see that the row type is the type of the array after having
// striped off the high-order dimension:
//..
//  int main()
//  {
//      assert((bsl::is_same<int, Traverser<int>::RowType>::value));
//      assert((bsl::is_same<int, Traverser<int[]>::RowType>::value));
//      assert((bsl::is_same<int, Traverser<int[5]>::RowType>::value));
//      typedef const int MyRow[6];
//      assert((bsl::is_same<MyRow, Traverser<MyRow[]>::RowType>::value));
//      assert((bsl::is_same<int[6], Traverser<int[7][6]>::RowType>::value));
//
//      return 0;
//  }
//..

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

#include <cstddef>

namespace bsl {

                        // ============================
                        // class template remove_extent
                        // ============================

template <class t_TYPE>
struct remove_extent {
    // From the C++14 standard: If 'T' names a type "array of 'U'", the member
    // typedef 'type' shall be 'U', otherwise 'T'.  [ *Note:* For
    // multidimensional arrays, only the first array dimension is removed.  For
    // a type "array of 'const U'", the resulting type is 'const U'.  -- *end
    // note* ]

    typedef t_TYPE type;
};

template <class t_TYPE>
struct remove_extent<t_TYPE[]> {
    // Specialization of 'remove_extent' for array of unknown bound

    typedef t_TYPE type;
};

template <class t_TYPE, std::size_t t_SZ>
struct remove_extent<t_TYPE[t_SZ]> {
    // Specialization of 'remove_extent' for array of known bound

    typedef t_TYPE type;
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES
template <class t_TYPE>
using remove_extent_t = typename remove_extent<t_TYPE>::type;
    // 'remove_extent_t' is an alias to the return type of the
    // 'bsl::remove_extent' meta-function.  Note, that the 'enable_if_t' avoids
    // the '::type' suffix and 'typename' prefix when we want to use the result
    // of the meta-function in templates.

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

}  // close namespace bsl

#endif // ! defined(INCLUDED_BSLMF_REMOVEEXTENT)

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
