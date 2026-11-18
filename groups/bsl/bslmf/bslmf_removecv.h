// bslmf_removecv.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVECV
#define INCLUDED_BSLMF_REMOVECV

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a metafunction for removing top-level cv-qualifiers.
//
//@CLASSES:
//  bsl::remove_cv: metafunction for removing top-level cv-qualifiers
//  bsl::remove_cv_t: alias to the return type of metafunction `bsl::remove_cv`
//
//@SEE_ALSO: bslmf_addcv
//
//@DESCRIPTION: This component defines a metafunction, `bsl::remove_cv`, and
// declares a `bsl::remove_cv_t` alias to the return type of the metafunction
// `bsl::remove_cv`, that may be used to remove any top-level cv-qualifiers
// (`const`-qualifier and `volatile`-qualifier) from a type.
//
// `bsl::remove_cv` meets the requirements of the `remove_cv` template defined
// in the C++11 Standard [meta.trans.cv], and `bsl::remove_cv_t` meets the
// requirements of the `remove_cv_t` alias template defined in the C++14
// Standard [meta.type.synop].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing the CV-Qualifiers of a Type
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove the cv-qualifiers from a particular type.
//
// First, we create two `typedef`s -- a `const`-qualified and
// `volatile`-qualified type (`MyCvType`) and the same type without the
// cv-qualifiers (`MyType`):
// ```
// typedef int                MyType;
// typedef const volatile int MyCvType;
// ```
// Now, we remove the cv-qualifiers from `MyCvType` using `bsl::remove_cv` and
// verify that the resulting type is the same as `MyType`:
// ```
// assert(true == (bsl::is_same<bsl::remove_cv<MyCvType>::type,
//                              MyType>::value));
// ```
// Finally, in C++11 and later, we remove the cv-qualifiers from `MyCvType`
// using the alias template `bsl::remove_cv_t` and verify that the resulting
// type is the same as `MyType`:
// ```
// #if BSLS_COMPILERFEATURES_FULL_CPP11
// assert(true == (bsl::is_same<bsl::remove_cv_t<MyCvType>, MyType>::value));
// #endif
// ```
// Note that `bsl::remove_cv_t` avoids the `::type` suffix and `typename`
// prefix when we want to use the result of the `bsl::remove_cv` metafunction
// in templates.

#include <bslscm_version.h>

#include <bsls_compilerfeatures.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslmf_removeconst.h>
#include <bslmf_removevolatile.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace bsl {

                         // ================
                         // struct remove_cv
                         // ================

/// This `struct` template implements the `remove_cv` metafunction defined in
/// the C++11 Standard [meta.trans.cv], providing an alias, `type`, that
/// returns the result.  `type` has the same type as the (template parameter)
/// `t_TYPE` except that any top-level cv-qualifiers have been removed.  This
/// primary template handles a `t_TYPE` that is not cv-qualified.
template <class t_TYPE>
struct remove_cv {

    // PUBLIC TYPES

    /// This `typedef` is an alias to the (template parameter) `t_TYPE`.
    typedef t_TYPE type;
};

/// This partial specialization, for a `const`-qualified template argument,
/// `const t_TYPE`, provides a `type` that is the (template parameter)
/// `t_TYPE`.
template <class t_TYPE>
struct remove_cv<const t_TYPE> {

    // PUBLIC TYPES
    typedef t_TYPE type;
};

/// This partial specialization, for a `volatile`-qualified template argument,
/// `volatile t_TYPE`, provides a `type` that is the (template parameter)
/// `t_TYPE`.
template <class t_TYPE>
struct remove_cv<volatile t_TYPE> {

    // PUBLIC TYPES
    typedef t_TYPE type;
};

/// This partial specialization, for a `const volatile`-qualified template
/// argument, `const volatile t_TYPE`, provides a `type` that is the (template
/// parameter) `t_TYPE`.  Note that this specialization is more specialized
/// than either of those above, and so is required to avoid ambiguity.
template <class t_TYPE>
struct remove_cv<const volatile t_TYPE> {

    // PUBLIC TYPES
    typedef t_TYPE type;
};

#if BSLS_COMPILERFEATURES_FULL_CPP11

// ALIASES

/// `remove_cv_t` is an alias to the return type of the `bsl::remove_cv`
/// metafunction.  Note that `remove_cv_t` avoids the `::type` suffix and
/// `typename` prefix when we want to use the result of the metafunction in
/// templates.
template <class t_TYPE>
using remove_cv_t = typename remove_cv<t_TYPE>::type;
#endif  // BSLS_COMPILERFEATURES_FULL_CPP11

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
