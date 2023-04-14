// bslmf_removecv.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVECV
#define INCLUDED_BSLMF_REMOVECV

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing top-level cv-qualifiers.
//
//@CLASSES:
//  bsl::remove_cv: meta-function for removing top-level cv-qualifiers
//  bsl::remove_cv_t: alias to the return type of the meta-function
//
//@SEE_ALSO: bslmf_addcv
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_cv', and
// declares a 'bsl::remove_cv_t' alias to the return type of the
// 'bsl::remove_cv', that may be used to remove any top-level cv-qualifiers
// ('const'-qualifier and 'volatile'-qualifier) from a type.
//
// 'bsl::remove_cv' and  'bsl::remove_cv_t' meet the requirements of the
// 'remove_cv' template defined in the C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing the CV-Qualifiers of a Type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove the cv-qualifiers from a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified and
// 'volatile'-qualified type ('MyCvType') and the same type without the
// cv-qualifiers ('MyType'):
//..
//  typedef int                MyType;
//  typedef const volatile int MyCvType;
//..
// Now, we remove the cv-qualifiers from 'MyCvType' using 'bsl::remove_cv' and
// verify that the resulting type is the same as 'MyType':
//..
//  assert(true == (bsl::is_same<bsl::remove_cv<MyCvType>::type,
//                                                            MyType>::value));
//..
// Finally, if the current compiler supports alias templates C++11 feature, we
// remove a 'const'-qualified and 'volatile'-qualifier from 'MyCvType' using
//'bsl::remove_cv_t' and verify that the resulting type is the same as
// 'MyType':
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//  assert(true ==
//                  (bsl::is_same<bsl::remove_cv_t<MyCvType>, MyType>::value));
//#endif
//..
// Note, that the 'bsl::remove_cv_t' avoids the '::type' suffix and 'typename'
// prefix when we want to use the result of the 'bsl::remove_cv' meta-function
// in templates.

#include <bslscm_version.h>

#include <bslmf_removeconst.h>
#include <bslmf_removevolatile.h>

#include <bsls_compilerfeatures.h>

namespace bsl {

                         // ================
                         // struct remove_cv
                         // ================

template <class t_TYPE>
struct remove_cv {
    // This 'struct' template implements the 'remove_cv' meta-function defined
    // in the C++11 standard [meta.trans.cv], providing an alias, 'type', that
    // returns the result.  'type' has the same type as the (template
    // parameter) 't_TYPE' except that any top-level cv-qualifiers have been
    // removed.

    // PUBLIC TYPES
    typedef typename remove_const<typename remove_volatile<t_TYPE>::type>::type
        type;
        // This 'typedef' is an alias to the same type as the (template
        // parameter) 't_TYPE' except that any top-level cv-qualifier has been
        // removed.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES
template <class t_TYPE>
using remove_cv_t = typename remove_cv<t_TYPE>::type;
    // 'remove_cv_t' is an alias to the return type of the 'bsl::remove_cv'
    // meta-function.  Note, that the 'remove_cv_t' avoids the '::type' suffix
    // and 'typename' prefix when we want to use the result of the
    // meta-function in templates.
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
