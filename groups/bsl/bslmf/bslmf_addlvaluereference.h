// bslmf_addlvaluereference.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDLVALUEREFERENCE
#define INCLUDED_BSLMF_ADDLVALUEREFERENCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time type transformation to lvalue reference.
//
//@CLASSES:
//  bsl::add_lvalue_reference: standard metafunction for type transformation
//  bsl::add_lvalue_reference_t: alias to the return type of the metafunction
//
//@SEE_ALSO: bslmf_addrvaluereference, bslmf_removereference
//
//@DESCRIPTION: This component defines a metafunction,
// `bsl::add_lvalue_reference`, that may be used to transform a type to its
// corresponding lvalue reference type.  An lvalue, as defined in the C++11
// Standard [basic.lval], is an expression that designates a function or an
// object.
//
// `bsl::add_lvalue_reference` and `bsl::add_lvalue_reference_t` meet the
// requirements of the `add_lvalue_reference` template and
// `add_lvalue_reference_t` alias defined in the C++ Standard [meta.trans.ref].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Transforming Types to Lvalue Reference Types
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform a set of types to their lvalue reference
// types.
//
// First, we instantiate the `bsl::add_lvalue_reference` template for several
// types, and use the `bsl::is_same` metafunction to assert the `type` member
// of each instantiation:
// ```
// assert(true ==
//       (bsl::is_same<bsl::add_lvalue_reference<int>::type,   int&>::value));
// assert(false ==
//       (bsl::is_same<bsl::add_lvalue_reference<int>::type,   int >::value));
// assert(true ==
//       (bsl::is_same<bsl::add_lvalue_reference<int&>::type,  int&>::value));
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
// assert(true ==
//       (bsl::is_same<bsl::add_lvalue_reference<int&&>::type, int&>::value));
// #endif
// ```
// Finally, in C++11 and later, we instantiate the
// `bsl::add_lvalue_reference_t` alias template for the same set of types, and
// use the `bsl::is_same` metafunction to assert the resultant type of each
// instantiation:
// ```
// #ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//   assert(true  ==
//           (bsl::is_same<bsl::add_lvalue_reference_t<int>,   int&>::value));
//   assert(false ==
//           (bsl::is_same<bsl::add_lvalue_reference_t<int>,   int >::value));
//   assert(true  ==
//           (bsl::is_same<bsl::add_lvalue_reference_t<int&>,  int&>::value));
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//   assert(true ==
//           (bsl::is_same<bsl::add_lvalue_reference_t<int&&>, int&>::value));
// #endif
// #endif
// ```
// Note that use of the `bsl::add_lvalue_reference_t` alias avoids the need for
// the `::type` suffix and `typename` prefix when using the result of the
// `bsl::add_lvalue_reference` metafunction in templates.

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

namespace bsl {
                         // ===========================
                         // struct add_lvalue_reference
                         // ===========================

/// This `struct` template implements the `add_lvalue_reference` metafunction
/// defined in the C++11 Standard [meta.trans.ref], providing an alias, `type`,
/// that returns the result, an lvalue reference to `t_TYPE`, or to the type to
/// which `t_TYPE` refers if it is a reference type.
template <class t_TYPE>
struct add_lvalue_reference {

    /// This `typedef` defines the return type of this metafunction.
    typedef t_TYPE& type;
};

#if !defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

/// This partial specialization, for when the template argument is an lvalue
/// reference type, `t_TYPE&`, provides a `type` that is that same lvalue
/// reference type.  Note that it is required in C++03, where no reference to
/// a reference type can be formed.
template <class t_TYPE>
struct add_lvalue_reference<t_TYPE&> {

    /// This `typedef` defines the return type of this metafunction.
    typedef t_TYPE& type;
};

#endif

/// This explicit specialization of `add_lvalue_reference` provides a `type`
/// that is `void`, unchanged, as no reference to `void` can be formed.
template <>
struct add_lvalue_reference<void> {

    /// This `typedef` defines the return type of this metafunction.
    typedef void type;
};

/// This explicit specialization of `add_lvalue_reference` provides a `type`
/// that is `void const`, unchanged, as no reference to `void const` can be
/// formed.
template <>
struct add_lvalue_reference<void const> {

    /// This `typedef` defines the return type of this metafunction.
    typedef void const type;
};

/// This explicit specialization of `add_lvalue_reference` provides a `type`
/// that is `void volatile`, unchanged, as no reference to `void volatile`
/// can be formed.
template <>
struct add_lvalue_reference<void volatile> {

    /// This `typedef` defines the return type of this metafunction.
    typedef void volatile type;
};

/// This explicit specialization of `add_lvalue_reference` provides a `type`
/// that is `void const volatile`, unchanged, as no reference to
/// `void const volatile` can be formed.
template <>
struct add_lvalue_reference<void const volatile> {

    /// This `typedef` defines the return type of this metafunction.
    typedef void const volatile type;
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES

/// `add_lvalue_reference_t` is an alias to the return type of the
/// `add_lvalue_reference` metafunction; use of this alias avoids the need for
/// the `::type` suffix and `typename` prefix when using the result of the
/// metafunction in templates.
template <class t_TYPE>
using add_lvalue_reference_t = typename add_lvalue_reference<t_TYPE>::type;

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
