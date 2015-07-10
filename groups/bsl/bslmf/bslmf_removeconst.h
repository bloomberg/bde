// bslmf_removeconst.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVECONST
#define INCLUDED_BSLMF_REMOVECONST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing top-level 'const'-qualifier.
//
//@CLASSES:
//  bsl::remove_const: meta-function for removing top-level 'const'-qualifier
//
//@SEE_ALSO: bslmf_addconst
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_const',
// that may be used to remove any top-level 'const'-qualifier from a type.
//
// 'bsl::remove_const' meets the requirements of the 'remove_const' template
// defined in the C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing the 'const'-Qualifier of a Type
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove any top-level 'const'-qualifier from a
// particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified type ('MyConstType')
// and the same type without the 'const'-qualifier ('MyType'):
//..
//  typedef int       MyType;
//  typedef const int MyConstType;
//..
// Now, we remove the 'const'-qualifier from 'MyConstType' using
// 'bsl::remove_const' and verify that the resulting type is the same as
// 'MyType':
//..
//  assert(true ==
//        (bsl::is_same<bsl::remove_const<MyConstType>::type, MyType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>
#define INCLUDED_STDDEF_H
#endif


namespace bsl {

                         // ===================
                         // struct remove_const
                         // ===================

template <class TYPE>
struct remove_const {
    // This 'struct' template implements the 'remove_const' meta-function
    // defined in the C++11 standard [meta.trans.cv], providing an alias,
    // 'type', that returns the result.  'type' has the same type as the
    // (template parameter) 'TYPE' except that any top-level 'const'-qualifier
    // has been removed.  Note that this generic default template provides a
    // 'type' that is an alias to 'TYPE' for when 'TYPE' is not
    // 'const'-qualified.  A template specialization is provided (below) that
    // removes the 'const'-qualifier for when 'TYPE' is 'const'-qualified.

    // PUBLIC TYPES
    typedef TYPE type;
        // This 'typedef' is an alias to the (template parameter) 'TYPE'.
};

                         // ===============================
                         // struct remove_const<TYPE const>
                         // ===============================

template <class TYPE>
struct remove_const<TYPE const> {
     // This partial specialization of 'bsl::remove_const', for when the
     // (template parameter) 'TYPE' is 'const'-qualified, provides a 'typedef',
     // 'type', that has the 'const'-qualifier removed.

    // PUBLIC TYPES
    typedef TYPE type;
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 'TYPE' except with the 'const'-qualifier removed.
};

#if defined(BSLS_PLATFORM_CMP_IBM)
// The IBM xlC compiler has an odd issue trying to remove 'const' qualifiers
// from multidimensional arrays.  This workaround was last verified as required
// for the xlC 12.1 compiler - more recent compilers still need testing.

template <class TYPE, size_t N>
struct remove_const<TYPE[N]> {
     // This partial specialization of 'bsl::remove_const', for when the
     // (template parameter) 'TYPE' is an array type.  On IBM compilers, it is
     // necessary to separately 'remove_const' on the element type, and then
     // reconstruct the array dimensions.

    // PUBLIC TYPES
    typedef typename remove_const<TYPE>::type type[N];
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 'TYPE[N]' except with the 'const'-qualifier removed.
};

template <class TYPE, size_t N>
struct remove_const<const TYPE[N]> {
     // This partial specialization of 'bsl::remove_const', for when the
     // (template parameter) 'TYPE' is an array type.  On IBM compilers, it is
     // necessary to separately 'remove_const' on the element type, and then
     // reconstruct the array dimensions.

    // PUBLIC TYPES
    typedef typename remove_const<const TYPE>::type type[N];
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 'TYPE[N]' except with the 'const'-qualifier removed.
};

template <class TYPE>
struct remove_const<TYPE[]> {
     // This partial specialization of 'bsl::remove_const', for when the
     // (template parameter) 'TYPE' is an array type.  On IBM compilers, it is
     // necessary to separately 'remove_const' on the element type, and then
     // reconstruct the array dimensions.

    // PUBLIC TYPES
    typedef typename remove_const<TYPE>::type type[];
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 'TYPE[]' except with the 'const'-qualifier removed.
};

template <class TYPE>
struct remove_const<const TYPE[]> {
     // This partial specialization of 'bsl::remove_const', for when the
     // (template parameter) 'TYPE' is an array type.  On IBM compilers, it is
     // necessary to separately 'remove_const' on the element type, and then
     // reconstruct the array dimensions.

    // PUBLIC TYPES
    typedef typename remove_const<const TYPE>::type type[];
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 'TYPE[]' except with the 'const'-qualifier removed.
};
#elif defined(BSLS_PLATFORM_CMP_MSVC)
// The Microsoft compiler does not recognize array-types as cv-qualified when
// the element type is cv-qualified when performing matching for partial
// template specialization, but does get the correct result when performing
// overload resolution for functions (taking arrays by reference).  Given the
// function dispatch behavior being correct, we choose to work around this
// compiler bug, rather than try to report compiler behavior, as the compiler
// itself is inconsistent depending on how the trait might be used.  This also
// corresponds to how Microsoft itself implements the trait in VC2010 and
// later.  Last tested against VC 2015 (Release Candidate).

template <class TYPE>
struct remove_const<TYPE[]> {
     // This partial specialization of 'bsl::remove_volatile', for when the
     // (template parameter) 'TYPE' is an array type.  On Microsoft compilers,
     // it is necessary to separately 'remove_volatile' on the element type,
     // and then reconstruct the array dimensions.

    // PUBLIC TYPES
    typedef typename remove_const<TYPE>::type type[];
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 'TYPE[]' except with the 'volatile'-qualifier removed.
};

template <class TYPE, size_t LENGTH>
struct remove_const<TYPE[LENGTH]> {
     // This partial specialization of 'bsl::remove_volatile', for when the
     // (template parameter) 'TYPE' is an array type.  On Microsoft compilers,
     // it is necessary to separately 'remove_volatile' on the element type,
     // and then reconstruct the array dimensions.

    // PUBLIC TYPES
    typedef typename remove_const<TYPE>::type type[LENGTH];
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 'TYPE[N]' except with the 'volatile'-qualifier removed.
};
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
