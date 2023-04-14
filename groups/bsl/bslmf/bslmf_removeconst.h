// bslmf_removeconst.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVECONST
#define INCLUDED_BSLMF_REMOVECONST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing top-level 'const'-qualifier.
//
//@CLASSES:
//  bsl::remove_const: meta-function for removing top-level 'const'-qualifier
//  bsl::remove_const_t: alias to the return type of the 'bsl::remove_const'
//
//@SEE_ALSO: bslmf_addconst
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_const' and
// declares an 'bsl::remove_const_t' alias to the return type of the
// 'bsl::remove_const', that may be used to remove any top-level
// 'const'-qualifier from a type.
//
// 'bsl::remove_const' and 'bsl::remove_const_t' meet the requirements of the
// 'remove_const' template defined in the C++11 standard [meta.trans.cv].
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
// Finally, if the current compiler supports alias templates C++11 feature, we
// remove a 'const'-qualifier from 'MyConstType' using 'bsl::remove_const_t'
// and verify that the resulting type is the same as 'MyType':
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//  assert(true ==
//            (bsl::is_same<bsl::remove_const_t<MyConstType>, MyType>::value));
//#endif
//..
// Note, that the 'bsl::remove_const_t' avoids the '::type' suffix and
// 'typename' prefix when we want to use the result of the 'bsl::remove_const'
// meta-function in templates.

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stddef.h>


// Several compiler tool-chains have problems removing the const qualifiers
// from arrays.  Additional documentation is below (where these macros are
// used).
#if defined(BSLS_PLATFORM_CMP_IBM)

# define BSLS_REMOVECONST_WORKAROUND_CONST_MULTIDIMENSIONAL_ARRAY 1
    // The IBM xlC compiler has an odd issue trying to remove 'const'
    // qualifiers from multidimensional arrays.  This workaround was last
    // verified as required for the xlC 12.1 compiler - more recent compilers
    // still need testing.

#elif (defined(BSLS_PLATFORM_CMP_SUN) && BSLS_PLATFORM_CMP_VERSION < 0x5130)  \
   || (defined(BSLS_PLATFORM_CMP_MSVC)                                        \
            && BSLS_PLATFORM_CMP_VERSION <= 1900                              \
            && _MSC_FULL_VER < 190023918)

# define BSLS_REMOVECONST_WORKAROUND_CONST_ARRAY 1
    // The Microsoft compiler does not recognize array-types as cv-qualified
    // when the element type is cv-qualified when performing matching for
    // partial template specialization, but does get the correct result when
    // performing overload resolution for functions (taking arrays by
    // reference).  Given the function dispatch behavior being correct, we
    // choose to work around this compiler bug, rather than try to report
    // compiler behavior, as the compiler itself is inconsistent depending on
    // how the trait might be used.  This also corresponds to how Microsoft
    // itself implements the trait in VC2010 and later.  Note that Microsoft
    // fixed this bug in Update 2 for MSVC2015, which requires checking the
    // '_MSC_FULL_VER' macro; the workaround below would be ambiguous when
    // trying to remove 'const' from an array of 'const' elements with a
    // conforming compiler.

#endif

namespace bsl {

                         // ===================
                         // struct remove_const
                         // ===================

template <class t_TYPE>
struct remove_const {
    // This 'struct' template implements the 'remove_const' meta-function
    // defined in the C++11 standard [meta.trans.cv], providing an alias,
    // 'type', that returns the result.  'type' has the same type as the
    // (template parameter) 't_TYPE' except that any top-level
    // 'const'-qualifier has been removed.  Note that this generic default
    // template provides a 'type' that is an alias to 't_TYPE' for when
    // 't_TYPE' is not 'const'-qualified.  A template specialization is
    // provided (below) that removes the 'const'-qualifier for when 't_TYPE' is
    // 'const'-qualified.

    // PUBLIC TYPES
    typedef t_TYPE type;
        // This 'typedef' is an alias to the (template parameter) 't_TYPE'.
};

                     // =================================
                     // struct remove_const<t_TYPE const>
                     // =================================

template <class t_TYPE>
struct remove_const<t_TYPE const> {
    // This partial specialization of 'bsl::remove_const', for when the
    // (template parameter) 't_TYPE' is 'const'-qualified, provides a
    // 'typedef', 'type', that has the 'const'-qualifier removed.

    // PUBLIC TYPES
    typedef t_TYPE type;
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 't_TYPE' except with the 'const'-qualifier removed.
};

#if defined(BSLS_REMOVECONST_WORKAROUND_CONST_MULTIDIMENSIONAL_ARRAY)
template <class t_TYPE, size_t N>
struct remove_const<t_TYPE[N]> {
    // This partial specialization of 'bsl::remove_const', for when the
    // (template parameter) 't_TYPE' is an array type.  On IBM compilers, it is
    // necessary to separately 'remove_const' on the element type, and then
    // reconstruct the array dimensions.

    // PUBLIC TYPES
    typedef typename remove_const<t_TYPE>::type type[N];
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 't_TYPE[N]' except with the 'const'-qualifier removed.
};

template <class t_TYPE, size_t N>
struct remove_const<const t_TYPE[N]> {
    // This partial specialization of 'bsl::remove_const', for when the
    // (template parameter) 't_TYPE' is an array type.  On IBM compilers, it is
    // necessary to separately 'remove_const' on the element type, and then
    // reconstruct the array dimensions.

    // PUBLIC TYPES
    typedef typename remove_const<const t_TYPE>::type type[N];
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 't_TYPE[N]' except with the 'const'-qualifier removed.
};

template <class t_TYPE>
struct remove_const<t_TYPE[]> {
    // This partial specialization of 'bsl::remove_const', for when the
    // (template parameter) 't_TYPE' is an array type.  On IBM compilers, it is
    // necessary to separately 'remove_const' on the element type, and then
    // reconstruct the array dimensions.

    // PUBLIC TYPES
    typedef typename remove_const<t_TYPE>::type type[];
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 't_TYPE[]' except with the 'const'-qualifier removed.
};

template <class t_TYPE>
struct remove_const<const t_TYPE[]> {
    // This partial specialization of 'bsl::remove_const', for when the
    // (template parameter) 't_TYPE' is an array type.  On IBM compilers, it is
    // necessary to separately 'remove_const' on the element type, and then
    // reconstruct the array dimensions.

    // PUBLIC TYPES
    typedef typename remove_const<const t_TYPE>::type type[];
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 't_TYPE[]' except with the 'const'-qualifier removed.
};
#elif defined(BSLS_REMOVECONST_WORKAROUND_CONST_ARRAY)
template <class t_TYPE>
struct remove_const<t_TYPE[]> {
    // This partial specialization of 'bsl::remove_const', for when the
    // (template parameter) 't_TYPE' is an array type.  On Microsoft compilers,
    // it is necessary to separately 'remove_const' on the element type, and
    // then reconstruct the array dimensions.

    // PUBLIC TYPES
    typedef typename remove_const<t_TYPE>::type type[];
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 't_TYPE[]' except with the 'const'-qualifier removed.
};

template <class t_TYPE, size_t LENGTH>
struct remove_const<t_TYPE[LENGTH]> {
    // This partial specialization of 'bsl::remove_const', for when the
    // (template parameter) 't_TYPE' is an array type.  On Microsoft compilers,
    // it is necessary to separately 'remove_const' on the element type, and
    // then reconstruct the array dimensions.

    // PUBLIC TYPES
    typedef typename remove_const<t_TYPE>::type type[LENGTH];
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 't_TYPE[N]' except with the 'const'-qualifier removed.
};
#endif

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES
template <class t_TYPE>
using remove_const_t = typename remove_const<t_TYPE>::type;
    // 'remove_const_t' is an alias to the return type of the
    // 'bsl::remove_const' meta-function.  Note, that the 'remove_const_t'
    // avoids the '::type' suffix and 'typename' prefix when we want to use the
    // result of the meta-function in templates.
#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES


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
