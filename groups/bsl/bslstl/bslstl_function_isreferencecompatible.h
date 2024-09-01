// bslstl_function_isreferencecompatible.h                            -*-C++-*-

#ifndef INCLUDED_BSLSTL_FUNCTION_ISREFERENCECOMPATIBLE
#define INCLUDED_BSLSTL_FUNCTION_ISREFERENCECOMPATIBLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a metafunction for substitutability of type references.
//
//@CLASSES:
//  bslstl::Function_IsReferenceCompatible : Boolean metafunction
//
//@SEE_ALSO: bslstl_function, bdef_function
//
//@DESCRIPTION: This private, subordinate component to `bslstl_function`
// provides a Boolean metafunction, `bslstl::Function_IsReferenceCompatible`,
// which allows generic code to determine whether a reference to the first
// template parameter type (`FROM_TYPE`) can be substituted for a reference to
// the second template parameter type (`TO_TYPE`) with no loss of information.
// By default, this metafunction yields `true_type` if `FROM_TYPE` is the same
// as `TO_TYPE`; else it yields `false_type`.  However, this template can be
// specialized to yield `true_type` for other parameters that have compatible
// references.  In practice, this metafunction is used to detect types, such as
// `bdef_Function`, that convert to `bsl::function` and wrap it with no
// additional data members; `bslstl::Function_IsReferenceCompatible` would be
// specialized to yield `true_type` for such wrapper types.  This metafunction
// is used within an `enable_if` to prevent types that are reference compatible
// with `bsl::function` from matching template parameters in `function`
// constructors and assignment operators, preferring, instead, the non-template
// overloads for copy and move construction and assignment.  Note that
// reference qualifiers on `FROM_TYPE` and `TO_TYPE` will cause instantiation
// to fail; the caller should strip reference qualifiers on `FROM_TYPE` and
// `TO_TYPE` before checking for reference compatibility.  This component
// exists solely to provide a transition from `bdef_Function` to
// `bsl::function` so that the former can eventually be deprecated and removed.
// This component should therefore also be considered deprecated and will be
// removed when `bdef_Function` is removed.
//

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_conditional.h>
#include <bslmf_issame.h>
#include <bslmf_movableref.h>

#include <bsls_compilerfeatures.h>

namespace BloombergLP {
namespace bslstl {

               // =============================================
               // class template Function_IsReferenceCompatible
               // =============================================

/// This metafunction is derived from `true_type` if a reference to the
/// specified `FROM_TYPE` parameter type can be substituted for a reference
/// to the specified `TO_TYPE` parameter type with no loss of information;
/// otherwise, it is derived from `false_type`.  By default, this
/// metafunction yields `true_type` if `FROM_TYPE` is the same as `TO_TYPE`;
/// else it yields `false_type`.  `bdef_Function` should specialize this
/// template to yield `true_type` when `FROM_TYPE` is an instantiation of
/// `bdef_Function` and `TO_TYPE` is the corresponding instantiation of
/// `bsl::function` with the same function prototype.  Instantiation will
/// fail if either `FROM_TYPE` or `TO_TYPE` are reference types.
template <class FROM_TYPE, class TO_TYPE>
struct Function_IsReferenceCompatible
: bsl::is_same<FROM_TYPE, TO_TYPE>::type {

    // Force compilation failure if 'FROM_TYPE' or 'TO_TYPE' are a reference
    // type.
    BSLMF_ASSERT(!bslmf::MovableRefUtil::IsReference<FROM_TYPE>::value);
    BSLMF_ASSERT(!bslmf::MovableRefUtil::IsReference<TO_TYPE>::value);
};

/// Partial specialization of `Function_IsReferenceCompatible` for `TO_TYPE`
/// being const.  The evaluation is forwarded to other specializations after
/// stripping the const qualifiers.  Note that if `FROM_TYPE` is const and
/// `TO_TYPE` is mutable, this partial specialization will not be selected
/// and the resulting evaluation will yield `false_type`, reflecting the
/// fact that a reference to const type cannot be substituted with a
/// reference to mutable type.
template <class FROM_TYPE, class TO_TYPE>
struct Function_IsReferenceCompatible<FROM_TYPE, const TO_TYPE>
: Function_IsReferenceCompatible<typename bsl::remove_const<FROM_TYPE>::type,
                                 TO_TYPE> {
};

}  // close package namespace
}  // close enterprise namespace

#endif  // !defined(INCLUDED_BSLSTL_FUNCTION_ISREFERENCECOMPATIBLE)

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
