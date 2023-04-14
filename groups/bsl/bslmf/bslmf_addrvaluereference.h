// bslmf_addrvaluereference.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDRVALUEREFERENCE
#define INCLUDED_BSLMF_ADDRVALUEREFERENCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time type transformation to rvalue reference.
//
//@CLASSES:
//  bsl::add_rvalue_reference: standard meta-function for transforming types
//  bsl::add_rvalue_reference_t: alias to the return type of the meta-function
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_addlvaluereference
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::add_rvalue_reference', that may be used to transform a type to its
// rvalue reference type.
//
// 'bsl::add_rvalue_reference' and 'bsl::add_rvalue_reference_t' meet the
// requirements of the 'add_rvalue_reference' template defined in the C++11
// standard [meta.trans.ref].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Transform to Rvalue Reference Types
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform some types to rvalue reference types.
//
// Now, for a set of types, we transform each type to the corresponding rvalue
// reference of that type using 'bsl::add_rvalue_reference' and verify the
// result:
//..
//#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//  assert(true  ==
//       (bsl::is_same<bsl::add_rvalue_reference<int>::type,   int&&>::value));
//  assert(false ==
//       (bsl::is_same<bsl::add_rvalue_reference<int>::type,   int  >::value));
//  assert(true  ==
//       (bsl::is_same<bsl::add_rvalue_reference<int&>::type,  int& >::value));
//  assert(true  ==
//       (bsl::is_same<bsl::add_rvalue_reference<int&&>::type, int&&>::value));
//..
// Finally, if the current compiler supports alias templates C++11 feature, we
// instantiate the 'bsl::add_rvalue_reference_t' template for the same set of
// types, and use the 'bsl::is_same' meta-function to assert the resultant type
// of each instantiation:
//..
//#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//  assert(true  ==
//          (bsl::is_same<bsl::add_rvalue_reference_t<int>,   int&&>::value));
//  assert(false ==
//          (bsl::is_same<bsl::add_rvalue_reference_t<int>,   int  >::value));
//  assert(true  ==
//          (bsl::is_same<bsl::add_rvalue_reference_t<int&>,  int& >::value));
//  assert(true ==
//          (bsl::is_same<bsl::add_rvalue_reference_t<int&&>, int&&>::value));
//#endif
//#endif
//..
// Note that rvalue reference was introduced in C++11 and may not be supported
// by all compilers.  Note also that according to 'reference collapsing'
// semantics [8.3.2], 'add_rvalue_reference' does not transform 't_TYPE' to
// rvalue reference type if 't_TYPE' is an lvalue reference type.
//
// Also note that the 'bsl::add_rvalue_reference_t' avoids the '::type' suffix
// and 'typename' prefix when we want to use the result of
// 'bsl::add_rvalue_reference' meta-function in templates.

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

namespace bsl {

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

template <class t_TYPE>
struct add_rvalue_reference {
    // This 'struct' template implements a meta-function to transform the
    // (template parameter) 't_TYPE' to its rvalue reference type.

    // PUBLIC TYPES
    typedef t_TYPE&& type;
        // This 'typedef' is an alias to the return type of this meta-function.
};

template <>
struct add_rvalue_reference<void> {
    // This partial specialization of 'add_rvalue_reference' defines the return
    // type when it is instantiated with the 'void' type.

    // PUBLIC TYPES
    typedef void type;
        // This 'typedef' is an alias to the return type of this meta-function.
};

template <>
struct add_rvalue_reference<void const> {
    // This partial specialization of 'add_rvalue_reference' defines the return
    // type when it is instantiated with the 'void const' type.

    // PUBLIC TYPES
    typedef void const type;
        // This 'typedef' is an alias to the return type of this meta-function.
};

template <>
struct add_rvalue_reference<void volatile> {
    // This partial specialization of 'add_rvalue_reference' defines the return
    // type when it is instantiated with the 'void volatile' type.

    // PUBLIC TYPES
    typedef void volatile type;
        // This 'typedef' is an alias to the return type of this meta-function.
};

template <>
struct add_rvalue_reference<void const volatile> {
    // This partial specialization of 'add_rvalue_reference' defines the return
    // type when it is instantiated with the 'void const volatile' type.

    // PUBLIC TYPES
    typedef void const volatile type;
        // This 'typedef' is an alias to the return type of this meta-function.
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES
template <class t_TYPE>
using add_rvalue_reference_t = typename add_rvalue_reference<t_TYPE>::type;
    // 'add_rvalue_reference_t' is an alias to the return type of
    // 'add_rvalue_reference' meta-function.  Note, that the 'add_const_t'
    // avoids the '::type' suffix and 'typename' prefix when we want to use the
    // result of the meta-function in templates.

#endif  // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
#endif  // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

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
