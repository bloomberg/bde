// bslmf_removevolatile.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEVOLATILE
#define INCLUDED_BSLMF_REMOVEVOLATILE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing `volatile`-qualifier.
//
//@CLASSES:
//  bsl::remove_volatile: meta-function for removing `volatile`-qualifier
//  bsl::remove_volatile_t: alias to the return type of the meta-function
//
//@SEE_ALSO: bslmf_addvolatile
//
//@DESCRIPTION: This component defines a meta-function, `bsl::remove_volatile`
//  and declares an `bsl::remove_volatile_t` alias to the return type of the
// `bsl::remove_volatile`, that may be used to remove any top-level
// `volatile`-qualifier from a type.
//
// `bsl::remove_volatile` and `bsl::remove_volatile_t` meet the requirements of
// the `remove_volatile` template defined in the C++11 standard
// [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing the `volatile`-Qualifier of a Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove any `volatile`-qualifier from a particular
// type.
//
// First, we create two `typedef`s -- a `volatile`-qualified type
// (`MyVolatileType`) and the same type without the `volatile`-qualifier
// (`MyType`):
// ```
// typedef int          MyType;
// typedef volatile int MyVolatileType;
// ```
// Now, we remove the `volatile`-qualifier from `MyVolatileType` using
// `bsl::remove_volatile` and verify that the resulting type is the same as
// `MyType`:
// ```
// assert(true == (bsl::is_same<bsl::remove_volatile<MyVolatileType>::type,
//                                                           MyType>::value));
// ```
// Finally, if the current compiler supports alias templates C++11 feature, we
// remove a `volatile`-qualifier from `MyVolatileType` using
//`bsl::remove_volatile_t` and verify that the resulting type is the same as
// `MyType`:
// ```
// #ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
//   assert(true ==
//       (bsl::is_same<bsl::remove_volatile_t<MyVolatileType>, MyType>::value));
// #endif
// ```
// Note, that the `bsl::remove_volatile_t` avoids the `::type` suffix and
// `typename` prefix when we want to use the result of the  `bsl::remove_const`
// meta-function in templates.

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
# include <stddef.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

                        // ======================
                        // struct remove_volatile
                        // ======================

/// This `struct` template implements the `remove_volatile` meta-function
/// defined in the C++11 standard [meta.trans.cv], providing an alias,
/// `type`, that returns the result.  `type` has the same type as the
/// (template parameter) `t_TYPE` except that any top-level
/// `volatile`-qualifier has been removed.  Note that this generic default
/// template provides a `type` that is an alias to `t_TYPE` for when
/// `t_TYPE` is not `volatile`-qualified.  A template specialization is
/// provided (below) that removes the `volatile`-qualifier for when `t_TYPE`
/// is `volatile`-qualified.
template <class t_TYPE>
struct remove_volatile {

    // PUBLIC TYPES

    /// This `typedef` is an alias to the (template parameter) `t_TYPE`.
    typedef t_TYPE type;
};

                  // =======================================
                  // struct remove_volatile<t_TYPE volatile>
                  // =======================================

/// This partial specialization of `bsl::remove_volatile`, for when the
/// (template parameter) `t_TYPE` is `volatile`-qualified, provides a
/// `typedef`, `type`, that has the `volatile`-qualifier removed.
template <class t_TYPE>
struct remove_volatile<t_TYPE volatile> {

    // PUBLIC TYPES

    /// This `typedef` is an alias to the same type as the (template
    /// parameter) `t_TYPE` except with the `volatile`-qualifier removed.
    typedef t_TYPE type;
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

// ALIASES

/// `remove_volatile_t` is an alias to the return type of the
/// `bsl::remove_volatile` meta-function.  Note, that the
/// `remove_volatile_t` avoids the `::type` suffix and `typename` prefix
/// when we want to use the result of the meta-function in templates.
template <class t_TYPE>
using remove_volatile_t = typename remove_volatile<t_TYPE>::type;
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
