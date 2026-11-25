// bslmf_addrvaluereference.h                                         -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDRVALUEREFERENCE
#define INCLUDED_BSLMF_ADDRVALUEREFERENCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time type transformation to rvalue reference.
//
//@CLASSES:
//  bsl::add_rvalue_reference: standard metafunction for transforming types
//  bsl::add_rvalue_reference_t: alias to the return type of the metafunction
//
//@SEE_ALSO: bslmf_addlvaluereference, bslmf_removereference
//
//@DESCRIPTION: This component defines a metafunction,
// `bsl::add_rvalue_reference`, that may be used to transform a type to its
// corresponding rvalue reference type.  Note that this component is available
// only in C++11 and later.
//
// `bsl::add_rvalue_reference` and `bsl::add_rvalue_reference_t` meet the
// requirements of the `add_rvalue_reference` template and
// `add_rvalue_reference_t` alias defined in the C++ Standard [meta.trans.ref].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Transforming Types to Rvalue Reference Types
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform a set of types to their rvalue reference
// types.
//
// First, we instantiate the `bsl::add_rvalue_reference` template for several
// types, and use the `bsl::is_same` metafunction to assert the `type` member
// of each instantiation:
// ```
// #if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//   assert(true  ==
//       (bsl::is_same<bsl::add_rvalue_reference<int>::type,   int&&>::value));
//   assert(false ==
//       (bsl::is_same<bsl::add_rvalue_reference<int>::type,   int  >::value));
//   assert(true  ==
//       (bsl::is_same<bsl::add_rvalue_reference<int&>::type,  int& >::value));
//   assert(true  ==
//       (bsl::is_same<bsl::add_rvalue_reference<int&&>::type, int&&>::value));
// ```
// Notice that, as a consequence of reference collapsing [dcl.ref], an lvalue
// reference type is not transformed.
//
// Finally, we instantiate the `bsl::add_rvalue_reference_t` alias template for
// the same set of types, and use the `bsl::is_same` metafunction to assert the
// resultant type of each instantiation:
// ```
// #ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//   assert(true  ==
//           (bsl::is_same<bsl::add_rvalue_reference_t<int>,   int&&>::value));
//   assert(false ==
//           (bsl::is_same<bsl::add_rvalue_reference_t<int>,   int  >::value));
//   assert(true  ==
//           (bsl::is_same<bsl::add_rvalue_reference_t<int&>,  int& >::value));
//   assert(true ==
//           (bsl::is_same<bsl::add_rvalue_reference_t<int&&>, int&&>::value));
// #endif
// #endif
// ```
// Note that rvalue references were introduced in C++11, and so this component
// is not available for earlier standards.
//
// Also note that use of the `bsl::add_rvalue_reference_t` alias avoids the
// need for the `::type` suffix and `typename` prefix when using the result of
// the `bsl::add_rvalue_reference` metafunction in templates.

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

namespace bsl {

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)

/// This `struct` template implements the `add_rvalue_reference` metafunction
/// defined in the C++11 Standard [meta.trans.ref], providing an alias, `type`,
/// that returns the result, `t_TYPE&&`, which reference collapsing reduces to
/// `t_TYPE` if `t_TYPE` is an lvalue reference type.
template <class t_TYPE>
struct add_rvalue_reference {

    // PUBLIC TYPES

    /// This `typedef` is an alias to the return type of this metafunction.
    typedef t_TYPE&& type;
};

/// This explicit specialization of `add_rvalue_reference` provides a `type`
/// that is `void`, unchanged, as no reference to `void` can be formed.
template <>
struct add_rvalue_reference<void> {

    // PUBLIC TYPES

    /// This `typedef` is an alias to the return type of this metafunction.
    typedef void type;
};

/// This explicit specialization of `add_rvalue_reference` provides a `type`
/// that is `void const`, unchanged, as no reference to `void const` can be
/// formed.
template <>
struct add_rvalue_reference<void const> {

    // PUBLIC TYPES

    /// This `typedef` is an alias to the return type of this metafunction.
    typedef void const type;
};

/// This explicit specialization of `add_rvalue_reference` provides a `type`
/// that is `void volatile`, unchanged, as no reference to `void volatile`
/// can be formed.
template <>
struct add_rvalue_reference<void volatile> {

    // PUBLIC TYPES

    /// This `typedef` is an alias to the return type of this metafunction.
    typedef void volatile type;
};

/// This explicit specialization of `add_rvalue_reference` provides a `type`
/// that is `void const volatile`, unchanged, as no reference to
/// `void const volatile` can be formed.
template <>
struct add_rvalue_reference<void const volatile> {

    // PUBLIC TYPES

    /// This `typedef` is an alias to the return type of this metafunction.
    typedef void const volatile type;
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES

/// `add_rvalue_reference_t` is an alias to the return type of the
/// `add_rvalue_reference` metafunction; use of this alias avoids the need for
/// the `::type` suffix and `typename` prefix when using the result of the
/// metafunction in templates.
template <class t_TYPE>
using add_rvalue_reference_t = typename add_rvalue_reference<t_TYPE>::type;

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
