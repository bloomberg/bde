// bslmf_addconst.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDCONST
#define INCLUDED_BSLMF_ADDCONST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding a top-level 'const'-qualifier.
//
//@CLASSES:
//  bsl::add_const: meta-function for adding a top-level 'const'-qualifier
//  bsl::add_const_t: alias to the return type of the meta-function
//
//@SEE_ALSO: bslmf_removeconst
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::add_const' and
// declares an 'bsl::add_const_t' alias to the return type of the
// 'bsl::add_const', that may be used to add a top-level 'const'-qualifier to a
// type if it is not a reference type, nor a function type, nor already
// 'const'-qualified at the top-level.
//
// 'bsl::add_const' and 'bsl::add_const_t' meet the requirements of the
// 'add_const' template defined in the C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding a 'const'-qualifier to a Type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add a 'const'-qualifier to a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified type ('MyConstType')
// and the same type without the 'const'-qualifier ('MyType'):
//..
//  typedef int       MyType;
//  typedef const int MyConstType;
//..
// Now, we add a 'const'-qualifier to 'MyType' using 'bsl::add_const' and
// verify that the resulting type is the same as 'MyConstType':
//..
//  assert(true ==
//           (bsl::is_same<bsl::add_const<MyType>::type, MyConstType>::value));
//..
// Finally, if the current compiler supports alias templates C++11 feature, we
// add a 'const'-qualifier to 'MyType' using 'bsl::add_const_t' and verify that
// the resulting type is the same as 'MyConstType':
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//  assert(true ==
//               (bsl::is_same<bsl::add_const_t<MyType>, MyConstType>::value));
//#endif
//..
// Note, that the 'bsl::add_const_t' avoids the '::type' suffix and 'typename'
// prefix when we want to use the result of the 'bsl::add_const' meta-function
// in templates.

#include <bslscm_version.h>

#include <bslmf_isconst.h>
#include <bslmf_isfunction.h>
#include <bslmf_isreference.h>

#include <bsls_compilerfeatures.h>

namespace BloombergLP {
namespace bslmf {

                         // ===================
                         // struct AddConst_Imp
                         // ===================

template <class t_TYPE, bool t_ADD_CONST_FLAG>
struct AddConst_Imp {
    // This 'struct' template provides an alias 'Type' that adds a
    // 'const'-qualifier to the (template parameter) 't_TYPE' if the (template
    // parameter) 't_ADD_CONST_FLAG' is 'true'.  This generic default template
    // adds the 'const'-qualifier to 't_TYPE' in the 'Type' alias.  A template
    // specialization (below) leaves 't_TYPE' as-is in its 'Type' alias.

    // PUBLIC TYPES
    typedef t_TYPE const Type;
        // This 'typedef' is an alias to a type that is the same as the
        // (template parameter) 't_TYPE' except that a top-level
        // 'const'-qualifier has been added.
};

                     // ==================================
                     // struct AddConst_Imp<t_TYPE, false>
                     // ==================================

template <class t_TYPE>
struct AddConst_Imp<t_TYPE, false> {
    // This partial specialization of 'AddConst_Imp', for when the (template
    // parameter) 't_ADD_CONST_FLAG' is 'false', provides an alias 'Type' that
    // has the same type as the (template parameter) 't_TYPE'.

    // PUBLIC TYPES
    typedef t_TYPE Type;
        // This 'typedef' is an alias to the (template parameter) 't_TYPE'.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // ================
                         // struct add_const
                         // ================

template <class t_TYPE>
struct add_const {
    // This 'struct' template implements the 'add_const' meta-function defined
    // in the C++11 standard [meta.trans.cv], providing an alias, 'type', that
    // returns the result.  If the (template parameter) 't_TYPE' is not a
    // reference type, nor a function type, nor already 'const'-qualified at
    // the top-level, then 'type' is an alias to 't_TYPE' with a top-level
    // 'const'-qualifier added; otherwise, 'type' is an alias to 't_TYPE'.

    // PUBLIC TYPES
    typedef typename BloombergLP::bslmf::AddConst_Imp<
        t_TYPE,
        !is_reference<t_TYPE>::value && !is_function<t_TYPE>::value &&
            !is_const<t_TYPE>::value>::Type type;
        // This 'typedef' is an alias to the (template parameter) 't_TYPE' with
        // a top-level 'const'-qualifier added if 't_TYPE' is not a reference,
        // nor a function, nor already 'const'-qualified; otherwise, 'type' is
        // an alias to 't_TYPE'.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES
template <class t_TYPE>
using add_const_t = typename add_const<t_TYPE>::type;
    // 'add_const_t' is an alias to the return type of the 'add_const'
    // meta-function.  Note, that the 'add_const_t' avoids the '::type' suffix
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
