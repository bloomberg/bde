// bslmf_istriviallycopyable.h                                        -*-C++-*-
#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#define INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for determining trivially copyable types.
//
//@CLASSES:
//  bsl::is_trivially_copyable: type-traits meta-function
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_nestedtraitdeclaration
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_trivially_copyable', that may be used to query whether a type is
// trivially copyable as defined in section 3.9.3 of the C++11 standard
// [basic.types].
//
// 'bsl::is_trivially_copyable' has the same syntax as the
// 'is_trivially_copyable' template from the C++11 standard [meta.unary.prop].
// However, unlike the template defined in the C++11 standard, which can
// determine the correct value for all types without requiring specialization,
// 'bsl::is_trivially_copyable' can, by default, determine the value for the
// following type categories only:
//..
//  Type Category        Is Trivially Copyable
//  -------------        ---------------------
//  reference types      false
//  fundamental types    true
//  enumerated types     true
//  pointers             true
//  pointers to members  true
//..
// For all other types, 'bsl::is_trivially_copyable' returns 'false', unless
// the type is explicitly specified to be trivially copyable.  This can be done
// in 2 ways:
//
//: 1 Define a template specialization for 'bsl::is_trivially_copyable' having
//:   the type as the template parameter that inherits directly from
//:   'bsl::true_type'.
//:
//: 2 Use the 'BSLMF_NESTED_TRAIT_DECLARATION' macro to define
//:   'bsl::is_trivially_copyable' as a trait in the class definition of the
//:   type.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Whether Types are Trivially Copyable
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a type is trivially copyable.
//
// First, we define a set of types to evaluate:
//..
//  typedef int  MyFundamentalType;
//  typedef int& MyFundamentalTypeReference;
//
//  class MyTriviallyCopyableType {
//  };
//
//  struct MyNonTriviallyCopyableType {
//      //...
//  };
//..
// Then, since user-defined types cannot be automatically evaluated by
// 'is_trivially_copyable', we define a template specialization to specify that
// 'MyTriviallyCopyableType' is trivially copyable:
//..
//  namespace bsl {
//
//  template <>
//  struct is_trivially_copyable<MyTriviallyCopyableType> : bsl::true_type {
//      // This template specialization for 'is_trivially_copyable' indicates
//      // that 'MyTriviallyCopyableType' is a trivially copyable type.
//  };
//
//  }  // close namespace bsl
//..
// Now, we verify whether each type is trivially copyable using
// 'bsl::is_trivially_copyable':
//..
//  assert(true  == bsl::is_trivially_copyable<MyFundamentalType>::value);
//  assert(false == bsl::is_trivially_copyable<
//                                         MyFundamentalTypeReference>::value);
//  assert(true  == bsl::is_trivially_copyable<
//                                            MyTriviallyCopyableType>::value);
//  assert(false == bsl::is_trivially_copyable<
//                                         MyNonTriviallyCopyableType>::value);
//..
#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#include <bslmf_ispointertomember.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif


namespace bsl {

template <class TYPE>
struct is_trivially_copyable;

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                         // ==============================
                         // struct IsTriviallyCopyable_Imp
                         // ==============================

template <class TYPE>
struct IsTriviallyCopyable_Imp
    : bsl::integral_constant<
          bool,
          !bsl::is_reference<TYPE>::value
          && (  IsFundamental<TYPE>::value
             || IsEnum<TYPE>::value
             || bsl::is_pointer<TYPE>::value
             || IsPointerToMember<TYPE>::value
             || DetectNestedTrait<TYPE, bsl::is_trivially_copyable>::value)> {
    // This 'struct' template implements a meta-function to determine whether
    // the (non-cv-qualified) (template parameter) 'TYPE' is trivially
    // copyable.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // ============================
                         // struct is_trivially_copyable
                         // ============================

template <class TYPE>
struct is_trivially_copyable
: BloombergLP::bslmf::IsTriviallyCopyable_Imp<typename remove_cv<TYPE>::type> {
    // This 'struct' template implements a meta-function to determine whether
    // the (template parameter) 'TYPE' is trivially copyable.  This 'struct'
    // derives from 'bsl::true_type' if the 'TYPE' is trivially copyable, and
    // from 'bsl::false_type' otherwise.  This meta-function has the same
    // syntax as the 'is_trivially_copyable' meta-function defined in the C++11
    // standard [meta.unary.prop]; however, this meta-function can
    // automatically determine the value for the following types only:
    // reference types, fundamental types, enumerated types, pointers to
    // members, and types declared to have the 'bsl::is_trivially_copyable'
    // trait using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro (the value for
    // other types defaults to 'false').  To support other trivially copyable
    // types, this template must be specialized to inherit from
    // 'bsl::true_type' for them.
};


// IMPLEMENTATION NOTE: We specialize 'is_trivially_copyable' for
// 'bsls::TimeInterval' here because 'bsls' is levelized below 'bslmf'.
// Previously 'bsls_timeinterval.h' had forward declared the
// 'is_trivially_copyable' template and provided a specialization for
// 'TimeInterval' (see BDE 2.24.0 tag), but the foward declaration caused
// compilation errors with the Sun CC 5.13 compiler.
//
// This trait declaration is not needed once we migrate to a C++11 definition
// for 'is_trivially_copyable'.

template <>
struct is_trivially_copyable<BloombergLP::bsls::TimeInterval> : bsl::true_type{
    // This template specialization for 'is_trivially_copyable' indicates that
    // 'Date' is a trivially copyable type.
};

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
