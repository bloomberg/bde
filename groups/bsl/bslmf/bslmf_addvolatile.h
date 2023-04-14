// bslmf_addvolatile.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDVOLATILE
#define INCLUDED_BSLMF_ADDVOLATILE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding a 'volatile'-qualifier.
//
//@CLASSES:
//  bsl::add_volatile: meta-function for adding top-level 'volatile'-qualifier
//  bsl::add_volatile_t: alias to the return type of the 'bsl::add_volatile'
//
//@SEE_ALSO: bslmf_removevolatile
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::add_volatile'
// and declares an 'bsl::add_volatile_t' alias to the return type of the
// 'bsl::add_volatile', that may be used to add a top-level
// 'volatile'-qualifier to a type if it is not a reference type, nor a function
// type, nor already 'volatile'-qualified at the top-level.
//
// 'bsl::add_volatile' and 'bsl::add_volatile_t' meet the requirements of the
// 'add_volatile' template defined in the C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding a 'volatile'-Qualifier to a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add a 'volatile'-qualifier to a particular type.
//
// First, we create two 'typedef's -- a 'volatile'-qualified type
// ('MyVolatileType') and the same type without the 'volatile'-qualifier
// ('MyType'):
//..
//  typedef int          MyType;
//  typedef volatile int MyVolatileType;
//..
// Now, we add a 'volatile'-qualifier to 'MyType' using 'bsl::add_volatile' and
// verify that the resulting type is the same as 'MyVolatileType':
//..
//  assert(true ==
//     (bsl::is_same<bsl::add_volatile<MyType>::type, MyVolatileType>::value));
//..
// Finally, if the current compiler supports alias templates C++11 feature, we
// add a 'volatile'-qualifier to 'MyType' using 'bsl::add_volatile_t' and
// verify that the resulting type is the same as 'MyVolatileType':
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//  assert(true ==
//         (bsl::is_same<bsl::add_volatile_t<MyType>, MyVolatileType>::value));
//#endif
//..
// Note, that the 'bsl::add_volatile_t' avoids the '::type' suffix and
// 'typename' prefix when we want to use the result of the 'bsl::add_volatile'
// meta-function in templates.

#include <bslscm_version.h>

#include <bslmf_isfunction.h>
#include <bslmf_isreference.h>
#include <bslmf_isvolatile.h>

#include <bsls_compilerfeatures.h>


namespace BloombergLP {
namespace bslmf {

                         // ======================
                         // struct AddVolatile_Imp
                         // ======================

template <class t_TYPE, bool t_ADD_VOLATILE_FLAG>
struct AddVolatile_Imp {
    // This 'struct' template provides an alias, 'Type', that adds a
    // 'volatile'-qualifier to the (template parameter) 't_TYPE' if the
    // (template parameter) 't_ADD_VOLATILE_FLAG' is 'true'.  This generic
    // default template adds the 'volatile'-qualifier to 't_TYPE'.  A template
    // specialization (below) leaves 't_TYPE' as-is in its 'Type' alias.

    // PUBLIC TYPES
    typedef t_TYPE volatile Type;
        // This 'typedef' is an alias to a type that is the same as the
        // (template parameter) 't_TYPE' except that a top-level
        // 'volatile'-qualifier has been added.
};

                   // =====================================
                   // struct AddVolatile_Imp<t_TYPE, false>
                   // =====================================

template <class t_TYPE>
struct AddVolatile_Imp<t_TYPE, false> {
    // This partial specialization of 'AddVolatile_Imp', for when the (template
    // parameter) 't_ADD_VOLATILE_FLAG' is 'false', provides an alias 'Type'
    // that has the same type as the (template parameter) 't_TYPE'.

    // PUBLIC TYPES
    typedef t_TYPE Type;
        // This 'typedef' is an alias to the (template parameter) 't_TYPE'.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // ===================
                         // struct add_volatile
                         // ===================

template <class t_TYPE>
struct add_volatile {
    // This 'struct' template implements the 'add_volatile' meta-function
    // defined in the C++11 standard [meta.trans.cv], providing an alias,
    // 'type', that returns the result.  If the (template parameter) 't_TYPE'
    // is not a reference type, nor a function type, nor already
    // 'volatile'-qualified at the top-level, then 'type' is an alias to
    // 't_TYPE' with a top-level 'volatile'-qualifier added; otherwise, 'type'
    // is an alias to 't_TYPE'.

    typedef typename BloombergLP::bslmf::AddVolatile_Imp<
        t_TYPE,
        !is_reference<t_TYPE>::value && !is_function<t_TYPE>::value &&
            !is_volatile<t_TYPE>::value>::Type type;
        // This 'typedef' is an alias to the (template parameter) 't_TYPE' with
        // a top-level 'volatile'-qualifier added if 't_TYPE' is not a
        // reference type, nor a function type, nor already
        // 'volatile'-qualified at the top-level; otherwise, 'type' is an alias
        // to 't_TYPE'.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES
template <class t_TYPE>
using add_volatile_t = typename add_volatile<t_TYPE>::type;
    // 'add_volatile_t' is an alias to the return type of the
    // 'bsl::add_volatile' meta-function.  Note, that the 'bsl::add_volatile_t'
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
