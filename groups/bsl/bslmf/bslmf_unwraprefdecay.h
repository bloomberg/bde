// bslmf_unwraprefdecay.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMF_UNWRAPREFDECAY
#define INCLUDED_BSLMF_UNWRAPREFDECAY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to decay and unwrap reference wrappers.
//
//@CLASSES:
//  bsl::unwrap_ref_decay: standard meta-function to unwrap decayed ref wrapper
//  bsl::unwrap_ref_decay_t: alias to the return type of the meta-function
//
//@SEE_ALSO: bslmf_addreference
//
//@DESCRIPTION: This component defines a meta-function `bsl::unwrap_ref_decay`
// that may be used to unwrap a decayed (`bsl::decay`) `bsl::reference_wrapper`
// or a `std::reference_wrapper` of some type `U`, resulting in `U&`.  In case
// the specified type template argument is not an specialization of either
// `reference_wrapper` (after decaying) the result is the type itself.
//
// `bsl::unwrap_ref_decay` meets the requirements of the `unwrap_ref_decay`
// template defined in the C++20 standard [meta.trans.other].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Unwrap Reference Wrapped Argument Types
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we work in a programming environment where function argument
// types may be presented as is, or wrapped in a `bsl::reference_wrapper` or
// its `std` equivalent and we would like to create a member or local variable
// while obeying the request for reference-wrapping.  So when the argument type
// decays into a reference wrapper we would like to use a reference to the
// wrapped type, and simply use the decayed variations of the unwrapped types.
// This is a use case for `bsl::unwrap_ref_decay`.
//
// First, we create types that may be the bases for reference-wrapped, and
// normal type parameters:
// ```
//  typedef bsl::reference_wrapper<int *>  WrappedType;
//  typedef int                           *NotWrappedType;
// ```
// Next, we create types that represent possible function argument types:
// ```
//  typedef WrappedType                     WrappedTypeArray[5];
//  typedef WrappedType                    *WrappedTypePointer;
//  typedef const WrappedType               ConstWrappedType;
//  typedef volatile WrappedType            VolatileWrappedType;
//  typedef const volatile WrappedType      CvWrappedType;
//  typedef WrappedType&                    WrappedTypeRef;
//  typedef const WrappedType&              ConstWrappedTypeRef;
//  typedef volatile WrappedType&           VolatileWrappedTypeRef;
//  typedef const volatile WrappedType&     CvWrappedTypeRef;
//
//  typedef NotWrappedType                  NotWrappedTypeArray[5];
//  typedef NotWrappedType                 *NotWrappedTypePointer;
//  typedef const NotWrappedType            ConstNotWrappedType;
//  typedef volatile NotWrappedType         VolatileNotWrappedType;
//  typedef const volatile NotWrappedType   CvNotWrappedType;
//  typedef NotWrappedType&                 NotWrappedTypeRef;
//  typedef const NotWrappedType&           ConstNotWrappedTypeRef;
//  typedef volatile NotWrappedType&        VolatileNotWrappedTypeRef;
//  typedef const volatile NotWrappedType&  CvNotWrappedTypeRef;
// ```
// Finally we can verify and demonstrate how all these types turn into a
// decayed type, and only those that decay into a reference-wrapper type will
// become themselves references.  Notice that an array of reference wrappers
// decays into a pointer to a wrapper, so it will not turn into a reference.
// ```
//  assert((true == bsl::is_same<bsl::unwrap_ref_decay<WrappedTypeArray>::type,
//                               WrappedType *>::value));
//  assert((true == bsl::is_same<
//                      bsl::unwrap_ref_decay<WrappedTypePointer>::type,
//                      WrappedTypePointer>::value));
//
//  assert((true == bsl::is_same<bsl::unwrap_ref_decay<WrappedType>::type,
//                               int *&>::value));
//  assert((true == bsl::is_same<
//                      bsl::unwrap_ref_decay<ConstWrappedType>::type,
//                      int *&>::value));
//  assert((true == bsl::is_same<
//                      bsl::unwrap_ref_decay<VolatileWrappedType>::type,
//                      int *&>::value));
//  assert((true == bsl::is_same<bsl::unwrap_ref_decay<CvWrappedType>::type,
//                               int *&>::value));
//  assert((true == bsl::is_same<bsl::unwrap_ref_decay<WrappedTypeRef>::type,
//                               int *&>::value));
//  assert((true == bsl::is_same<
//                      bsl::unwrap_ref_decay<ConstWrappedTypeRef>::type,
//                      int *&>::value));
//  assert((true == bsl::is_same<
//                      bsl::unwrap_ref_decay<VolatileWrappedTypeRef>::type,
//                      int *&>::value));
//  assert((true == bsl::is_same<bsl::unwrap_ref_decay<CvWrappedTypeRef>::type,
//                               int *&>::value));
//
//  // Not wrapped types decay
//  assert((true == bsl::is_same<bsl::unwrap_ref_decay<NotWrappedType>::type,
//                               NotWrappedType>::value));
//  assert((true == bsl::is_same<
//                      bsl::unwrap_ref_decay<ConstNotWrappedType>::type,
//                      NotWrappedType>::value));
//  assert((true == bsl::is_same<
//                      bsl::unwrap_ref_decay<VolatileNotWrappedType>::type,
//                      NotWrappedType>::value));
//  assert((true == bsl::is_same<bsl::unwrap_ref_decay<CvNotWrappedType>::type,
//                               NotWrappedType>::value));
//  assert((true == bsl::is_same<
//                      bsl::unwrap_ref_decay<NotWrappedTypeRef>::type,
//                      NotWrappedType>::value));
//  assert((true == bsl::is_same<
//                      bsl::unwrap_ref_decay<ConstNotWrappedTypeRef>::type,
//                      NotWrappedType>::value));
//  assert((true == bsl::is_same<
//                      bsl::unwrap_ref_decay<VolatileNotWrappedTypeRef>::type,
//                      NotWrappedType>::value));
//  assert((true == bsl::is_same<
//                      bsl::unwrap_ref_decay<CvNotWrappedTypeRef>::type,
//                      NotWrappedType>::value));
// ```
// Note, that (when available) the `bsl::unwrap_ref_decay_t` avoids the
// `::type` suffix and `typename` prefix when we want to use the result of the
// `bsl::unwrap_ref_decay` meta-function in templates.

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
using std::unwrap_ref_decay;
using std::unwrap_ref_decay_t;
#else

/// This `struct` template implements the `unwrap_ref_decay` meta-function
/// defined in the C++20 standard [meta.trans.other], providing an alias,
/// `type`, that returns the result.  `type` has the same type as the
/// (template parameter) `t_TYPE` unless `bsl::decay<t_TYPE>::type` is a
/// specialization of `bsl::reference_wrapper` or `std::reference_wrapper<U>`
/// for a type U, in which case `type` shall be `U&`.
template <class t_TYPE>
struct unwrap_ref_decay {

    /// This `typedef` is an alias to the template parameter `t_TYPE`.
    typedef t_TYPE type;
};

#ifdef BSLS_COMPILERFEATURES_SUPPORT_ALIAS_TEMPLATES
template <class t_TYPE>
using unwrap_ref_decay_t = typename unwrap_ref_decay<t_TYPE>::type;
    // 'unwrap_ref_decay_t' is an alias to the return type of the
    // 'bsl::unwrap_ref_decay' meta-function.  Note, that the
    // 'unwrap_ref_decay_t' avoids the '::type' suffix and 'typename' prefix
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
