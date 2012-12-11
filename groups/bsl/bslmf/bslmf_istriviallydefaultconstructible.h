// bslmf_istriviallydefaultconstructible.h                            -*-C++-*-
#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#define INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for trivially default-constructible.
//
//@CLASSES:
//  bsl::is_trivially_default_constructible: trait meta-function
//
//@SEE_ALSO: bslmf_integerconstant, bslmf_nestedtraitdeclaration
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_trivially_default_constructible', that may be used to query whether
// a type has a trivial default constructor as defined in section 12.1.5 of the
// C++11 standard [class.ctor].
//
// 'bsl::is_trivially_default_constructible' has the same syntax as the
// 'is_trivially_default_constructible' template from the C++11 standard
// [meta.unary.prop].  However, unlike the template defined in the C++11
// standard, which can determine the correct value for all types without
// requiring specialization, 'bsl::is_trivially_default_constructible' can, by
// default, determine the value for the following type categories only:
//..
//  Type Category        Has Trivial Default Constructor
//  -------------------  -------------------------------
//  reference types      false
//  fundamental types    true
//  enums                true
//  pointers             true
//  pointers to members  true
//..
// For all other types, 'bsl::is_trivially_default_constructible' returns
// 'false', unless the type is explicitly specified to be trivially
// default-constructible, which can be done in two ways:
//
//: 1 Define a template specialization for
//:   'bsl::is_trivially_default_constructible' having the type as the template
//:   parameter that inherits directly from 'bsl::true_type'.
//:
//: 2 Use the 'BSLMF_NESTED_TRAIT_DECLARATION' macro to define
//:   'bsl::is_trivially_default_constructible' as the trait in the class
//:   definition of the type.
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
// First, we define a set of types to evaluate:
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
// Then, since user-defined types cannot be automatically evaluated by
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
//  assert(true  ==
//          bsl::is_trivially_default_constructible<MyFundamentalType>::value);
//  assert(false ==
//      bsl::is_trivially_default_constructible<
//                                         MyFundamentalTypeReference>::value);
//  assert(true  ==
//      bsl::is_trivially_default_constructible<
//                                MyTriviallyDefaultConstructibleType>::value);
//  assert(false ==
//      bsl::is_trivially_default_constructible<
//                             MyNonTriviallyDefaultConstructibleType>::value);
//..

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

namespace bsl {

template <class TYPE>
struct is_trivially_default_constructible;

}  // close namespace bsl

namespace BloombergLP {
namespace bslmf {

                         // ==========================================
                         // struct IsTriviallyDefaultConstructible_Imp
                         // ==========================================

template <class TYPE>
struct IsTriviallyDefaultConstructible_Imp
: bsl::integral_constant<
                     bool,
                     !bsl::is_reference<TYPE>::value
                     && (  IsFundamental<TYPE>::value
                        || IsEnum<TYPE>::value
                        || bsl::is_pointer<TYPE>::value
                        || IsPointerToMember<TYPE>::value
                        || DetectNestedTrait<TYPE,
                            bsl::is_trivially_default_constructible>::value)> {
    // This 'struct' template implements a meta-function to determine whether
    // the (non-cv-qualified) (template parameter) 'TYPE' is trivially
    // default-constructible.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // =========================================
                         // struct is_trivially_default_constructible
                         // =========================================

template <class TYPE>
struct is_trivially_default_constructible
    : BloombergLP::bslmf::IsTriviallyDefaultConstructible_Imp<
                                              typename remove_cv<TYPE>::type> {
    // This 'struct' template implements a meta-function to determine whether
    // the (template parameter) 'TYPE' is trivially default-constructible.
    // This 'struct' derives from 'bsl::true_type' if the 'TYPE' is trivially
    // default-constructible, and from 'bsl::false_type' otherwise.  This
    // meta-function has the same syntax as the
    // 'is_trivially_default_constructible' meta-function defined in the C++11
    // standard [meta.unary.prop]; however, this meta-function can
    // automatically determine the value for the following types only:
    // reference types, fundamental types, enums, pointers to members, and
    // types declared to have the 'bsl::is_trivially_default_constructible'
    // trait using the 'BSLMF_NESTED_TRAIT_DECLARATION' macro (and the value
    // for other types defaults to 'false').  To support other trivially
    // default-constructible types, this template must be specialized to
    // inherit from 'bsl::true_type' for them.
};

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
