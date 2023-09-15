// bslmf_isbitwisecopyable.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISBITWISECOPYABLE
#define INCLUDED_BSLMF_ISBITWISECOPYABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for determining bitwise copyable types.
//
//@CLASSES:
//  IsBitwiseCopyable: type-traits meta-function
//  IsBitwiseCopyable_v: the result value of the meta-function (post-C++14)
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_nestedtraitdeclaration
//
//@DESCRIPTION: This component defines a meta-function,
// 'bslmf::IsBitwiseCopyable' and a template variable
// 'bslmf::IsBitwiseCopyable_v', that represents the result value of the
// 'bslmf::IsBitwiseCopyable' meta-function, that may be used to query
// whether a type is deemed by the author to be trivially copyable, but does
// *NOT* have the property 'bsl::is_trivially_copyable'.
//
// 'bslmf::IsBitwiseCopyable' has the same syntax as the
// 'is_trivially_copyable' template from the C++11 standard [meta.unary.prop],
// but is used exclusively to identify types deemed by the author to be
// trivially copyable, but for which 'std::is_trivially_copyable' is not 'true'
// by default.  Typically such types might have a destructor, or non-trivial
// creators, so that copying them via 'memcpy' may be theoretically undefined
// behavior and cause problems on some future compilers.
//..
//  Type Category                 IsBitwiseCopyable by Default
//  -------------                 ----------------------------
//  fundamental types             true
//  enumerated types              true
//  pointers to data              true
//  pointers to functions         true
//  pointers to member data       true
//  pointers to member functions  true
//  reference types               false
//  rvalue reference types        false
//..
// Types that are 'bsl::is_trivially_copyable' are 'IsBitwiseCopyable' by
// default -- otherwise, the only way a type acquires the 'IsBitwiseCopyable'
// trait is by explicit specialization or by the
// 'BSLMF_NESTED_TRAIT_DECLARATION' macro.
//
// Note that the template variable 'IsBitwiseCopyable_v' is defined in the
// C++17 standard as an inline variable.  If the current compiler supports the
// inline variable C++17 compiler feature, 'bslmf::IsBitwiseCopyable_v' is
// defined as an 'inline constexpr bool' variable.  Otherwise, if the compiler
// supports the variable templates C++14 compiler feature,
// 'bslmf::IsBitwiseCopyable_v' is defined as a non-inline 'constexpr bool'
// variable.  See 'BSLS_COMPILERFEATURES_SUPPORT_INLINE_VARIABLES' and
// 'BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES' macros in
// bsls_compilerfeatures component for details.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Whether Types are Trivially Copyable
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a type is trivially copyable, and/or
// bitwise copyable.
//
// First, we define a set of types to evaluate:
//..
//  struct MyTriviallyCopyableType {
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(MyTriviallyCopyableType,
//                                     bsl::is_trivially_copyable);
//  };
//
//  struct MyNonTriviallyCopyableType {
//      // Because this 'struct' has constructors declared, the C++11 compiler
//      // will not automatically declare it 'std::is_trivially_copyable'.  But
//      // since it has no data, we know it can be 'memcpy'ed around so we will
//      // give it the 'IsBitwiseCopyable' trait.
//
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(MyNonTriviallyCopyableType,
//                                     bslmf::IsBitwiseCopyable);
//
//      // CREATORS
//      MyNonTriviallyCopyableType() {}
//      MyNonTriviallyCopyableType(const MyNonTriviallyCopyableType&) {}
//      ~MyNonTriviallyCopyableType() {}
//          // Explicitly supply creators that do nothing, to ensure that this
//          // class has no trivial traits detected with a conforming C++11
//          // library implementation.
//  };
//
//  class MyNonBitwiseCopyableType {
//      // This 'class' allocates memory and cannot be copied around with
//      // 'memcpy', so it should have neither the 'is_trivially_copyable' nor
//      // the 'IsBitwiseCopyable' traits.
//
//      // DATA
//      char *d_string;
//
//    public:
//      // CREATORS
//      MyNonBitwiseCopyableType(const char *string)
//      : d_string(::strdup(string))
//      {}
//
//      MyNonBitwiseCopyableType(const MyNonBitwiseCopyableType& original)
//      : d_string(::strdup(original.d_string))
//      {}
//
//      ~MyNonBitwiseCopyableType()
//      {
//          free(d_string);
//      }
//
//      bool operator==(const MyNonBitwiseCopyableType& rhs) const
//      {
//          return !::strcmp(d_string, rhs.d_string);
//      }
//  };
//..
// Then, the following 5 types are automatically interpreted by
// 'bsl::is_trivially_copyable' to be trivially copyable without our having to
// declare them as such, and therefore, as 'IsBitwiseCopyable'.
//..
//  typedef int MyFundamentalType;
//      // fundamental type
//
//  typedef int *DataPtrTestType;
//      // data pointer
//
//  typedef void (*FunctionPtrTestType)();
//      // function ptr
//
//  typedef int MyNonBitwiseCopyableType::*DataMemberPtrTestType;
//      // non-static data member ptr
//
//  typedef int (MyNonBitwiseCopyableType::*MethodPtrTestType)();
//      // non-static function member ptr
//..
// The following 2 types are neither trivially nor bitwise copyable:
///..
//  typedef int& MyFundamentalTypeRef;
//      // reference (not bitwise copyable)
//
//  #if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//      typedef int&& MyFundamentalTypeRvalueRef;
//          // rvalue reference (not bitwise copyable)
//  #endif
//..
// Now, we verify whether each type is trivially copyable using
// 'std::is_trivially_copyable':
//..
//  assert( std::is_trivially_copyable<MyTriviallyCopyableType>::value);
//  assert(!std::is_trivially_copyable<MyNonTriviallyCopyableType>::value);
//  assert(!std::is_trivially_copyable<MyNonBitwiseCopyableType>::value);
//
//  assert( std::is_trivially_copyable<MyFundamentalType>::value);
//  assert( std::is_trivially_copyable<DataPtrTestType>::value);
//  assert( std::is_trivially_copyable<FunctionPtrTestType>::value);
//  assert( std::is_trivially_copyable<DataMemberPtrTestType>::value);
//  assert( std::is_trivially_copyable<MethodPtrTestType>::value);
//  assert(!std::is_trivially_copyable<MyFundamentalTypeRef>::value);
//  #if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//      assert(!std::is_trivially_copyable<MyFundamentalTypeRvalueRef>::value);
//  #endif
//..
// Now, we verify whether each type is bitwise copyable using
// 'bslmf::IsBitwiseCopyable':
//..
//  assert( bslmf::IsBitwiseCopyable<MyTriviallyCopyableType>::value);
//  assert( bslmf::IsBitwiseCopyable<MyNonTriviallyCopyableType>::value);
//  assert(!bslmf::IsBitwiseCopyable<MyNonBitwiseCopyableType>::value);
//
//  assert( bslmf::IsBitwiseCopyable<MyFundamentalType>::value);
//  assert( bslmf::IsBitwiseCopyable<DataPtrTestType>::value);
//  assert( bslmf::IsBitwiseCopyable<FunctionPtrTestType>::value);
//  assert( bslmf::IsBitwiseCopyable<DataMemberPtrTestType>::value);
//  assert( bslmf::IsBitwiseCopyable<MethodPtrTestType>::value);
//  assert(!bslmf::IsBitwiseCopyable<MyFundamentalTypeRef>::value);
//  #if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//      assert(!bslmf::IsBitwiseCopyable<MyFundamentalTypeRvalueRef>::value);
//  #endif
//..
// Finally, note that if the current compiler supports the variable templates
// C++14 feature, then we can re-write the two snippets of code above as
// follows:
//..
//  #ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
//      // trivially copyable:
//
//      assert( std::is_trivially_copyable_v<MyTriviallyCopyableType>);
//      assert(!std::is_trivially_copyable_v<MyNonTriviallyCopyableType>);
//      assert(!std::is_trivially_copyable_v<MyNonBitwiseCopyableType>);
//
//      assert( std::is_trivially_copyable_v<MyFundamentalType>);
//      assert( std::is_trivially_copyable_v<DataPtrTestType>);
//      assert( std::is_trivially_copyable_v<FunctionPtrTestType>);
//      assert( std::is_trivially_copyable_v<DataMemberPtrTestType>);
//      assert( std::is_trivially_copyable_v<MethodPtrTestType>);
//      assert(!std::is_trivially_copyable_v<MyFundamentalTypeRef>);
//      assert(!std::is_trivially_copyable_v<MyFundamentalTypeRvalueRef>);
//
//      // bitwise copyable:
//
//      assert( bslmf::IsBitwiseCopyable_v<MyTriviallyCopyableType>);
//      assert( bslmf::IsBitwiseCopyable_v<MyNonTriviallyCopyableType>);
//      assert(!bslmf::IsBitwiseCopyable_v<MyNonBitwiseCopyableType>);
//
//      assert( bslmf::IsBitwiseCopyable_v<MyFundamentalType>);
//      assert( bslmf::IsBitwiseCopyable_v<DataPtrTestType>);
//      assert( bslmf::IsBitwiseCopyable_v<FunctionPtrTestType>);
//      assert( bslmf::IsBitwiseCopyable_v<DataMemberPtrTestType>);
//      assert( bslmf::IsBitwiseCopyable_v<MethodPtrTestType>);
//      assert(!bslmf::IsBitwiseCopyable_v<MyFundamentalTypeRef>);
//      assert(!bslmf::IsBitwiseCopyable_v<MyFundamentalTypeRvalueRef>);
//  #endif
//..

#include <bslscm_version.h>

#include <bslmf_detectnestedtrait.h>
#include <bslmf_integralconstant.h>
#include <bslmf_istriviallycopyable.h>

#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_platform.h>

#include <stddef.h>

#ifdef BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER
# include <type_traits>
#endif // BSLS_COMPILERFEATURES_SUPPORT_TRAITS_HEADER

namespace BloombergLP {
namespace bslmf {

                           // ========================
                           // struct IsBitwiseCopyable
                           // ========================

template <class t_TYPE>
struct IsBitwiseCopyable;

#ifdef BSLS_COMPILERFEATURES_SUPPORT_VARIABLE_TEMPLATES
template <class t_TYPE>
BSLS_KEYWORD_INLINE_VARIABLE constexpr bool IsBitwiseCopyable_v =
                                              IsBitwiseCopyable<t_TYPE>::value;
    // This template variable represents the result value of the
    // 'bslmf::IsBitwiseCopyable' meta-function.
#endif

template <class t_TYPE>
struct IsBitwiseCopyable : bsl::integral_constant<
                       bool,
                       DetectNestedTrait<t_TYPE, IsBitwiseCopyable>::value
                          || bsl::is_trivially_copyable<t_TYPE>::value>::type {
};

template <class t_TYPE>
struct IsBitwiseCopyable<t_TYPE&> : bsl::false_type {
    // This partial specialization optimizes away a number of nested template
    // instantiations to ensure that reference types are never bitwise
    // copyable.
};

#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
template <class t_TYPE>
struct IsBitwiseCopyable<t_TYPE&&> : bsl::false_type {
    // This partial specialization optimizes away a number of nested template
    // instantiations to ensure that rvalue reference types are never bitwise
    // copyable.
};
#endif

template <class t_TYPE>
struct IsBitwiseCopyable<const t_TYPE> : IsBitwiseCopyable<t_TYPE> {};

template <class t_TYPE>
struct IsBitwiseCopyable<volatile t_TYPE> : IsBitwiseCopyable<t_TYPE> {};

template <class t_TYPE>
struct IsBitwiseCopyable<const volatile t_TYPE> : IsBitwiseCopyable<t_TYPE> {};

template <class t_TYPE, int t_LEN>
struct IsBitwiseCopyable<t_TYPE[t_LEN]> : IsBitwiseCopyable<t_TYPE> {};

template <class t_TYPE, int t_LEN>
struct IsBitwiseCopyable<const t_TYPE[t_LEN]> : IsBitwiseCopyable<t_TYPE> {};

template <class t_TYPE, int t_LEN>
struct IsBitwiseCopyable<volatile t_TYPE[t_LEN]> :
                                                  IsBitwiseCopyable<t_TYPE> {};

template <class t_TYPE, int t_LEN>
struct IsBitwiseCopyable<const volatile t_TYPE[t_LEN]> :
                                                  IsBitwiseCopyable<t_TYPE> {};

template <class t_TYPE>
struct IsBitwiseCopyable<t_TYPE[]> : IsBitwiseCopyable<t_TYPE>::type {};
template <class t_TYPE>
struct IsBitwiseCopyable<const t_TYPE[]> : IsBitwiseCopyable<t_TYPE>::type {};

template <class t_TYPE>
struct IsBitwiseCopyable<volatile t_TYPE[]> :
                                            IsBitwiseCopyable<t_TYPE>::type {};
template <class t_TYPE>
struct IsBitwiseCopyable<const volatile t_TYPE[]>
                                          : IsBitwiseCopyable<t_TYPE>::type {};
    // These partial specializations ensures that array-of-unknown-bound types
    // have the same result as their element type.

}  // close namespace bslmf
}  // close enterprise namespace

#endif // ! defined(INCLUDED_BSLMF_ISBITWISECOPYABLE)

// ----------------------------------------------------------------------------
// Copyright 2023 Bloomberg Finance L.P.
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
