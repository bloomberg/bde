// bslmf_istriviallydefaultconstructible.h                            -*-C++-*-
#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#define INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Check for whether a type is trivially default-constructible.
//
//@CLASSES:
//  bsl::is_trivially_default_constructible: traits meta-function
//
//@SEE_ALSO: bslmf_integerconstant, bslmf_nestedtraitdeclaration
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_trivially_default_constructible', which may be used to query
// whether a type has a trivial default constructor as defined in section
// 12.1.5 of the C++11 standard [class.ctor].
//
// 'bsl::is_trivially_default_constructible' has the same syntax as the
// 'is_trivially_default_constructible' template from the C++11 standard
// [meta.unary.prop].  However, unlike the template defined in the C++11
// standard, which can determine the correct value for all types without
// requiring specialization, 'bsl::is_trivially_default_constructible' can only
// by default determine the value for the following type categories:
//..
//  Type Category        Has Trivial Default Constructor
//  -------------        -------------------------------
//  reference types      false
//  fundamental types    true
//  enums                true
//  pointers             true
//  pointers to members  true
//..
// For all other types, 'bsl::is_trivially_default_constructible' returns
// false, unless the type is explicitly specified to be trivially
// default-constructible, which can be done in 2 ways:
//
//:    1 Define a template specialization for
//:      'bsl::is_trivially_default_constructible' having the type as the
//:      template parameter that inherits directly from 'bsl::true_type'.
//:
//:    2 Use the 'BSLMF_NESTED_TRAIT_DECLARATION' macro to define
//:      'bsl::is_trivially_default_constructible' as the trait in the class
//:      definition of the type.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Whether Types are Trivially Default-Constructible
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a type is trivially
// default-constructible.
//
// First, we define a set types to evaluate:
//..
//  typedef int MyFundamentalType;
//  typedef int& MyFundamentalTypeReference;
//
//  class MyTriviallyDefaultConstructibleType {
//  };
//
//  struct MyNonTriviallyDefaultConstructibleType {
//
//      int d_data;
//
//      MyNonTriviallyDefaultConstructibleType()
//      : d_data(1)
//      {
//      }
//  };
//..
// Then, since user-defined types can not be automatically evaluated by
// 'is_trivially_default_constructible', we define a template specialization to
// specify that 'MyTriviallyDefaultConstructibleType' is trivially
// default-constructible:
//..
//  namespace bsl {
//
//  template <>
//  struct is_trivially_default_constructible<
//                      MyTriviallyDefaultConstructibleType> : bsl::true_type {
//      // This template specialization for
//      // 'is_trivially_default_constructible' indicates that
//      // 'MyTriviallyDefaultConstructibleType' is a trivially
//      // default-constructible type.
//  };
//
//  }  // close namespace bsl
//..
// Now, we verify whether each type is trivially default-constructible using
// 'bsl::is_trivially_default_constructible':
//..
//  assert(true ==
//          bsl::is_trivially_default_constructible<MyFundamentalType>::value);
//  assert(false ==
//      bsl::is_trivially_default_constructible<
//                                         MyFundamentalTypeReference>::value);
//  assert(true ==
//      bsl::is_trivially_default_constructible<
//                                MyTriviallyDefaultConstructibleType>::value);
//  assert(false ==
//      bsl::is_trivially_default_constructible<
//                             MyNonTriviallyDefaultConstructibleType>::value);
//..

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISFUNDAMENTAL
#include <bslmf_isfundamental.h>
#endif

#ifndef INCLUDED_BSLMF_ISENUM
#include <bslmf_isenum.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTER
#include <bslmf_ispointer.h>
#endif

#ifndef INCLUDED_BSLMF_ISPOINTERTOMEMBER
#include <bslmf_ispointertomember.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLMF_ISREFERENCE
#include <bslmf_isreference.h>
#endif

namespace bsl {

template <typename TYPE>
struct is_trivially_default_constructible;

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>

                         // ==========================================
                         // struct IsTriviallyDefaultConstructible_Imp
                         // ==========================================

struct IsTriviallyDefaultConstructible_Imp
: bsl::integral_constant<
                     bool,
                     !bsl::is_reference<TYPE>::value
                     && (  IsFundamental<TYPE>::value
                        || IsEnum<TYPE>::value
                        || bsl::is_pointer<TYPE>::value
                        || bslmf::IsPointerToMember<TYPE>::value
                        || DetectNestedTrait<TYPE,
                            bsl::is_trivially_default_constructible>::value)> {
    // This 'struct' template implement a meta-function to determine whether
    // the (non-cv-qualified) template parameter 'TYPE' is trivially
    // default-constructible.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // =========================================
                         // struct is_trivially_default_constructible
                         // =========================================

template <typename TYPE>
struct is_trivially_default_constructible
    : BloombergLP::bslmf::IsTriviallyDefaultConstructible_Imp<
                                              typename remove_cv<TYPE>::type> {
    // This 'struct' template implements a meta-function to determine whether
    // the (template parameter) 'TYPE' is trivially default-constructible.
    // This 'struct' derives from 'bsl::true_type', if the 'TYPE' is trivially
    // defaul-constructible, and 'bsl::false_type' otherwise.  This
    // meta-function has the same syntax as the
    // 'is_trivially_default_constructible' meta-function defined in the C++11
    // standard [meta.unary.prop]; however, this meta-function can only
    // automatically determine the value for the following types: reference
    // types, fundamental types, enums, pointers to members, and types declared
    // to have the 'bsl::is_trivially_default_constructible' trait using the
    // 'BSLMF_NESTED_TRAIT_DECLARATION' macro (and the value for other types
    // defaults to 'false').  To support other trivially default-constructible
    // types, this template must be specialized to inherit from
    // 'bsl::true_type' for them.
};

}  // close namespace bsl

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
