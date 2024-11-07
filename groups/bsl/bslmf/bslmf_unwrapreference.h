// bslmf_unwrapreference.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_UNWRAPREFERENCE
#define INCLUDED_BSLMF_UNWRAPREFERENCE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to unwrap reference wrappers.
//
//@CLASSES:
//  bsl::unwrap_reference: standard meta-function to unwrap reference wrappers
//  bsl::unwrap_reference_t: alias to the return type of the meta-function
//
//@SEE_ALSO: bslmf_addreference
//
//@DESCRIPTION: This component defines a meta-function `bsl::unwrap_reference`
// that may be used to unwrap a `bsl::reference_wrapper` or a
// `std::reference_wrapper` of some type `U`, resulting in `U&`.  In case the
// specified type template argument is not an specialization of either
// `reference_wrapper` the result is the type itself.
//
// `bsl::unwrap_reference` meets the requirements of the `unwrap_reference`
// template defined in the C++20 standard [meta.trans.other].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Unwrap Reference Wrapped Types
///- - - - - - - - - - - - - - - - - - - - -
// Suppose that we work in a programming environment where types may be
// presented wrapped in a `bsl::reference_wrapper` or its `std` equivalent.  We
// would like to use a reference to the wrapped type, but use unwrapped types
// as they are.  This is the exact use case for `bsl::unwrap_reference`.
//
// First, we create types that represent both reference-wrapped, and normal
// type parameters:
// ```
//  typedef bsl::reference_wrapper<int *>  WrappedType;
//  typedef int                           *NotWrappedType;
// ```
// Next, we create types that are references if they were wrapped:
// ```
//  typedef bsl::unwrap_reference<WrappedType>::type    UnwrappedWrapped;
//  typedef bsl::unwrap_reference<NotWrappedType>::type UnwrappedNotWrapped;
// ```
// Finally we can verify that the wrapped type became a reference, while the
// other type is unchanged:
// ```
//  assert((true == bsl::is_same<UnwrappedWrapped,    int *&>::value));
//  assert((true == bsl::is_same<UnwrappedNotWrapped, int *>::value));
// ```
// Note, that (when available) the `bsl::unwrap_reference_t` avoids the
// `::type` suffix and `typename` prefix when we want to use the result of the
// `bsl::unwrap_reference` meta-function in templates.

#include <bslscm_version.h>

#include <bslmf_referencewrapper.h>

#include <bsls_compilerfeatures.h>
#include <bsls_libraryfeatures.h>

#if BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
  #include <type_traits> // 'std::unwrap_reference', 'std::unwrap_reference_t'
#endif

                         // ======================
                         // struct uwrap_reference
                         // ======================

namespace bsl {

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY
using std::unwrap_reference;
using std::unwrap_reference_t;
#else

/// This `struct` template implements the `unwrap_reference` meta-function
/// defined in the C++20 standard [meta.trans.other], providing an alias,
/// `type`, that returns the result.  `type` has the same type as the
/// (template parameter) `t_TYPE` unless `t_TYPE` is a specialization of
/// `bsl::reference_wrapper` or `std::reference_wrapper<U>` for a type U, in
/// which case `type` shall be `U&`.
template <class t_TYPE>
struct unwrap_reference {

    /// This `typedef` is an alias to the template parameter `t_TYPE`.
    typedef t_TYPE type;
};

/// This partial specialization of `bsl::unwrap_reference`, for when the
/// template parameter `t_TYPE` is a `bsl::reference_wrapper`, provides a
/// `typedef`, `type`, that is `t_TYPE::type`.
template <class t_WRAPPED_TYPE>
struct unwrap_reference<bsl::reference_wrapper<t_WRAPPED_TYPE> > {

    typedef t_WRAPPED_TYPE& type;
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES)
// ALIASES
template <class t_TYPE>
using unwrap_reference_t = typename unwrap_reference<t_TYPE>::type;
    // 'unwrap_reference_t' is an alias to the return type of the
    // 'bsl::unwrap_reference' meta-function.  Note, that the
    // 'unwrap_reference_t' avoids the '::type' suffix and 'typename' prefix
    // when we want to use the result of the meta-function in templates.
#endif // BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES

#endif // else of BSLS_LIBRARYFEATURES_HAS_CPP20_BASELINE_LIBRARY

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright 2024 Bloomberg Finance L.P.
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
