// bslmf_removecvref.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVECVREF
#define INCLUDED_BSLMF_REMOVECVREF

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-func for removing reference-ness and cv-qualifiers.
//
//@CLASSES:
//  bsl::remove_cvref: meta-func for removing reference-ness and cv-qualifiers
//  bsl::remove_cvref_t: alias to the return type of the meta-function
//
//@SEE_ALSO: bslmf_removecv, bslmf_removereference
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_cvref',
// and declares a 'bsl::remove_cvref_t' alias to the return type of the
// 'bsl::remove_cvref', that may be used to strip reference-ness (including
// both lvalue and rvalue reference-ness, if the latter is supported by the
// compiler) and to remove any top-level cv-qualifiers ('const'-qualifier and
// 'volatile'-qualifier) from a type.
//
// 'bsl::remove_cvref' and  'bsl::remove_cvref_t' meet the requirements of the
// 'remove_cvref' template defined in the C++20 standard [meta.trans.other].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing the CV-Qualifiers and Reference-ness of a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove the cv-qualifiers from a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified and
// 'volatile'-qualified reference type ('MyCvRefType') and the same type
// without the cv-qualifiers and reference-ness ('MyType'):
//..
//      typedef const volatile int& MyCvRefType;
//      typedef                int  MyType;
//..
// Now, we remove the cv-qualifiers from 'MyCvRefType' and its reference-ness
// using 'bsl::remove_cvref' and verify that the resulting type is the same as
// 'MyType':
//..
//      assert(true == (bsl::is_same<bsl::remove_cvref<MyCvRefType>::type,
//                                   MyType>::value));
//..
// Finally, if the current compiler supports alias templates C++11 feature, we
// remove a 'const'-qualifier, 'volatile'-qualifier and its reference-ness from
// 'MyCvRefType' using 'bsl::remove_cvref_t' and verify that the resulting type
// is the same as 'MyType':
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//      assert(
//          true ==
//          (bsl::is_same<bsl::remove_cvref_t<MyCvRefType>, MyType>::value));
//#endif
//..

#include <bslscm_version.h>

#include <bslmf_removecv.h>
#include <bslmf_removereference.h>

#include <bsls_compilerfeatures.h>

namespace bsl {

                         // ===================
                         // struct remove_cvref
                         // ===================

template <class t_TYPE>
struct remove_cvref {
    // This 'struct' template implements the 'remove_cvref' meta-function
    // defined in the C++20 standard [meta.trans.other], providing an alias,
    // 'type', that returns the result.  'type' has the same type as the
    // (template parameter) 't_TYPE' except that its reference-ness has been
    // stripped and any top-level cv-qualifiers have been removed.

    // PUBLIC TYPES
    typedef typename bsl::remove_cv<
        typename bsl::remove_reference<t_TYPE>::type>::type type;
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 't_TYPE' except that its reference-ness has been stripped
        // and any top-level cv-qualifier has been removed.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES
template <class t_TYPE>
using remove_cvref_t = typename remove_cvref<t_TYPE>::type;
    // 'remove_cvref_t' is an alias to the return type of the
    // 'bsl::remove_cvref' meta-function.  Note, that the 'remove_cvref_t'
    // avoids the '::type' suffix and 'typename' prefix when we want to use the
    // result of the meta-function in templates.

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
