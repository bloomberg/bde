// bslmf_decay.h                  -*-C++-*-
#ifndef INCLUDED_BSLMF_DECAY
#define INCLUDED_BSLMF_DECAY

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Convert a type to the type used for pass-by-value
//
//@CLASSES: bsl::decay<TYPE>
//
//@SEE_ALSO: bslmf_removeextent
//
//@AUTHOR: Pablo Halpern (phalpern)
//
//@DESCRIPTION: This component provies a metafunction that converts a type
// into the used for pass-by-value by applying array-to-pointer and
// function-to-pointer conversion and cv-qualification removal. 'bsl::decay'
// provides identical functionality to the C++11 standard metafunction
// 'std::decay'. From the C++14, standard description of 'std::decay':
//
// Let 'U' be 'remove_reference_t<T>'. If 'is_array<U>::value' is 'true', the
// member typedef 'type' shall equal 'remove_extent_t<U>*'. If
// 'is_function<U>::value' is 'true', the member typedef 'type' shall equal
// 'add_pointer_t<U>'. Otherwise the member typedef 'type' equals
// 'remove_cv_t<U>'. [ *Note*: This behavior is similar to the
// lvalue-to-rvalue (4.1), array-to-pointer (4.2), and function-to-pointer
// (4.3) conversions applied when an lvalue expression is used as an rvalue,
// but also strips cv-qualifiers from class types in order to more closely
// model by-value argument passing. - *end note* ]
//
///Usage
///-----
//
/// Usage Example 1
/// - - - - - - - -
// When an array is passed as a function argument, it decays to a
// pointer to its element type. The result of 'bsl::decay' applied
// to array types is, therefore, a pointer to the element type of the
// array:
//..
//  typedef const int A1[];
//  typedef double A2[3][4];
//  assert((bsl::is_same<const int*,   bsl::decay<A1>::type>::value));
//  assert((bsl::is_same<double(*)[4], bsl::decay<A2>::type>::value));
//..
//
/// Usage Example 2
/// - - - - - - - -
// When an function is passed as a function argument, it decays to a
// pointer to the function type. This transformation is reflected in
// result of 'bsl::decay', when applied to function types
//..
//  typedef void F1(int);
//  typedef int (&F2)();
//  assert((bsl::is_same<void (*)(int), bsl::decay<F1>::type>::value));
//  assert((bsl::is_same<int (*)(),     bsl::decay<F2>::type>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ADDPOINTER
#include <bslmf_addpointer.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISARRAY
#include <bslmf_isarray.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNCTION
#include <bslmf_isfunction.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEEXTENT
#include <bslmf_removeextent.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

namespace bsl {

// Forward declaration
template <class TYPE, bool ISARRAY, bool ISFUNC> struct decay_imp;

                        // ====================
                        // class template decay
                        // ====================

template <class TYPE>
class decay
{
    // Metafunction to return the variant of the specified parameter 'TYPE'
    // used for pass-by-reference, e.g., by applying array-to-pointer and
    // function-to-pointer conversion.

    typedef typename bsl::remove_reference<TYPE>::type U;
    enum {
        ISARRAY = is_array<typename bsl::remove_reference<U>::type>::value,
        ISFUNC  = is_function<typename bsl::remove_reference<U>::type>::value
    };

  public:
    typedef typename decay_imp<U, ISARRAY, ISFUNC>::type type;
};

// ===========================================================================
//                CLASS TEMPLATE IMPLEMENTATION
// ===========================================================================

template <class TYPE, bool ISARRAY, bool ISFUNC>
struct decay_imp : remove_cv<TYPE>
{
    BSLMF_ASSERT( ! (ISARRAY || ISFUNC));
};

template <class TYPE>
struct decay_imp<TYPE, true /* ISARRAY */, false /* ISFUNC */>
{
    typedef typename remove_extent<TYPE>::type *type;
};

template <class TYPE>
struct decay_imp<TYPE, false /* ISARRAY */, true /* ISFUNC */>
{
    typedef typename add_pointer<TYPE>::type type;
};

}  // close namespace bsl

#endif // ! defined(INCLUDED_BSLMF_DECAY)

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
