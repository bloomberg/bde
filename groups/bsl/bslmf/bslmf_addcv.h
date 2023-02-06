// bslmf_addcv.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDCV
#define INCLUDED_BSLMF_ADDCV

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding top-level cv-qualifiers.
//
//@CLASSES:
//  bsl::add_cv: meta-function for adding top-level cv-qualifiers
//  bsl::add_cv_t: alias to the return type of the 'bsl::add_cv' meta-function
//
//@SEE_ALSO: bslmf_removecv
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::add_cv' and
// declares an 'bsl::add_cv_t' alias to the return type of the 'bsl::add_cv',
// that may be used to add a top-level 'const'-qualifier and a top-level
// 'volatile'-qualifier to a type if it is not a reference type, nor a function
// type, nor already 'const'-qualified and 'volatile'-qualified at the
// top-level.
//
// 'bsl::add_cv' and 'bsl::add_cv_t' meet the requirements of the 'add_cv'
// template defined in the C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding a 'const'-Qualifier and a 'volatile'-Qualifier to a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add a 'const'-qualifier and a 'volatile'-qualifier
// to a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified and
// 'volatile'-qualified type ('MyCvType') and the same type without the
// cv-qualifiers ('MyType'):
//..
//  typedef int                MyType;
//  typedef const volatile int MyCvType;
//..
// Now, we add a 'const'-qualifier and a 'volatile'-qualifier to 'MyType' using
// 'bsl::add_cv' and verify that the resulting type is the same as 'MyCvType':
//..
//  assert(true == (bsl::is_same<bsl::add_cv<MyType>::type, MyCvType>::value));
//..
// Finally, if the current compiler supports alias templates C++11 feature, we
// add a 'const'-qualifier and a 'volatile'-qualifier to 'MyType' using
// 'bsl::add_cv_t' and verify that the resulting type is the same as
// 'MyCvType':
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//  assert(true == (bsl::is_same<bsl::add_cv_t<MyType>, MyCvType>::value));
//#endif
//..
// Note, that the 'bsl::add_cv_t' avoids the '::type' suffix and 'typename'
// prefix when we want to use the result of the 'bsl::add_cv' meta-function in
// templates.

#include <bslscm_version.h>

#include <bslmf_addconst.h>
#include <bslmf_addvolatile.h>

#include <bsls_compilerfeatures.h>

namespace bsl {

                         // =============
                         // struct add_cv
                         // =============

template <class t_TYPE>
struct add_cv {
    // This 'struct' template implements the 'add_cv' meta-function defined in
    // the C++11 standard [meta.trans.cv], providing an alias, 'type', that
    // returns the result.  If the (template parameter) 't_TYPE' is not a
    // reference type, nor a function type, nor already 'const'-qualified and
    // 'volatile'-qualified at the top-level, then 'type' is an alias to
    // 't_TYPE' with a top-level 'const'-qualifier and a 'volatile'-qualifier
    // added; otherwise, 'type' is an alias to 't_TYPE'.

    // PUBLIC TYPES
    typedef typename add_const<typename add_volatile<t_TYPE>::type>::type type;
        // This 'typedef' is an alias to the (template parameter) 't_TYPE' with
        // a top-level 'const'-qualifier and 'volatile'-qualifier added if
        // 't_TYPE' is not a reference type, nor a function type, nor already
        // 'const'-qualified and 'volatile'-qualified at the top-level;
        // otherwise, 'type' is an alias to 't_TYPE'.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES
template <class t_TYPE>
using add_cv_t = typename add_cv<t_TYPE>::type;
    // 'add_cv_t' is an alias to the return type of the 'bsl::add_cv'
    // meta-function.  Note, that the 'add_cv_t' avoids the '::type' suffix and
    // 'typename' prefix when we want to use the result of the 'bsl::add_cv' in
    // templates.
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
