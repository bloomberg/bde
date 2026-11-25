// bslmf_removeconst.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVECONST
#define INCLUDED_BSLMF_REMOVECONST

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing top-level `const`-qualifier.
//
//@CLASSES:
//  bsl::remove_const: meta-function for removing top-level `const`-qualifier
//  bsl::remove_const_t: alias to the return type of the `bsl::remove_const`
//
//@SEE_ALSO: bslmf_addconst
//
//@DESCRIPTION: This component defines a meta-function, `bsl::remove_const` and
// declares an `bsl::remove_const_t` alias to the return type of the
// `bsl::remove_const`, that may be used to remove any top-level
// `const`-qualifier from a type.
//
// `bsl::remove_const` and `bsl::remove_const_t` meet the requirements of the
// `remove_const` template defined in the C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing the `const`-Qualifier of a Type
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove any top-level `const`-qualifier from a
// particular type.
//
// First, we create two `typedef`s -- a `const`-qualified type (`MyConstType`)
// and the same type without the `const`-qualifier (`MyType`):
// ```
// typedef int       MyType;
// typedef const int MyConstType;
// ```
// Now, we remove the `const`-qualifier from `MyConstType` using
// `bsl::remove_const` and verify that the resulting type is the same as
// `MyType`:
// ```
// assert(true ==
//       (bsl::is_same<bsl::remove_const<MyConstType>::type, MyType>::value));
// ```
// Finally, if the current compiler supports alias templates C++11 feature, we
// remove a `const`-qualifier from `MyConstType` using `bsl::remove_const_t`
// and verify that the resulting type is the same as `MyType`:
// ```
// #ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//   assert(true ==
//             (bsl::is_same<bsl::remove_const_t<MyConstType>, MyType>::value));
// #endif
// ```
// Note, that the `bsl::remove_const_t` avoids the `::type` suffix and
// `typename` prefix when we want to use the result of the `bsl::remove_const`
// meta-function in templates.

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
# include <stddef.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

                         // ===================
                         // struct remove_const
                         // ===================

/// This `struct` template implements the `remove_const` meta-function
/// defined in the C++11 standard [meta.trans.cv], providing an alias,
/// `type`, that returns the result.  `type` has the same type as the
/// (template parameter) `t_TYPE` except that any top-level
/// `const`-qualifier has been removed.  Note that this generic default
/// template provides a `type` that is an alias to `t_TYPE` for when
/// `t_TYPE` is not `const`-qualified.  A template specialization is
/// provided (below) that removes the `const`-qualifier for when `t_TYPE` is
/// `const`-qualified.
template <class t_TYPE>
struct remove_const {

    // PUBLIC TYPES

    /// This `typedef` is an alias to the (template parameter) `t_TYPE`.
    typedef t_TYPE type;
};

                     // =================================
                     // struct remove_const<t_TYPE const>
                     // =================================

/// This partial specialization of `bsl::remove_const`, for when the
/// (template parameter) `t_TYPE` is `const`-qualified, provides a
/// `typedef`, `type`, that has the `const`-qualifier removed.
template <class t_TYPE>
struct remove_const<t_TYPE const> {

    // PUBLIC TYPES

    /// This `typedef` is an alias to the same type as the (template
    /// parameter) `t_TYPE` except with the `const`-qualifier removed.
    typedef t_TYPE type;
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES

/// `remove_const_t` is an alias to the return type of the
/// `bsl::remove_const` meta-function.  Note, that the `remove_const_t`
/// avoids the `::type` suffix and `typename` prefix when we want to use the
/// result of the meta-function in templates.
template <class t_TYPE>
using remove_const_t = typename remove_const<t_TYPE>::type;
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
