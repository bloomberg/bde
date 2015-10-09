// bslmf_isintegral.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ISINTEGRAL
#define INCLUDED_BSLMF_ISINTEGRAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for integral types.
//
//@CLASSES:
//  bsl::is_integral: standard meta-function for determining integral types
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_integral',
// that may be used to query whether a type is an integral type as defined in
// section 3.9.1.7 of the C++11 standard [basic.fundamental] (excluding those
// types that were introduced in C++11).
//
// 'bsl::is_integral' meets the requirements of the 'is_integral' template
// defined in the C++11 standard [meta.unary.cat] except that it may not
// correctly evaluate types introduced in C++11.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Integral Types
/// - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is an integral
// type.
//
// First, we create two 'typedef's -- an integral type and a non-integral type:
//..
//  typedef void MyType;
//  typedef int  MyIntegralType;
//..
// Now, we instantiate the 'bsl::is_integral' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_integral<MyType>::value);
//  assert(true  == bsl::is_integral<MyIntegralType>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bslmf {

                         // =====================
                         // struct IsIntegral_Imp
                         // =====================

template <class TYPE>
struct IsIntegral_Imp : bsl::false_type {
    // This 'struct' template implements a meta-function to determine whether
    // the (template parameter) 'TYPE' is an integral type.  This generic
    // default template derives from 'bsl::false_type'.  Template
    // specializations are provided (below) that derive from 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<bool> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'bool', derives from 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<char> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'char', derives from 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<wchar_t> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'wchar_t', derives from 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<signed char> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'signed char', derives from 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<unsigned char> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'unsigned char', derives from 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<short> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'short', derives from 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<unsigned short> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'unsigned short', derives from 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<int> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'int', derives from 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<unsigned int> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'unsigned int', derives from 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<long int> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'long int', derives from 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<unsigned long int> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'unsigned long int', derives from
     // 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<bsls::Types::Int64> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'bsls::Types::Int64', derives from
     // 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<bsls::Types::Uint64> : bsl::true_type {
     // This specialization of 'IsIntegral_Imp', for when the (template
     // parameter) 'TYPE' is 'bsls::Types::Uint64', derives from
     // 'bsl::true_type'.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // ==================
                         // struct is_integral
                         // ==================

template <class TYPE>
struct is_integral
   : BloombergLP::bslmf::IsIntegral_Imp<typename remove_cv<TYPE>::type>::type {
    // This 'struct' template implements the 'is_integral' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 'TYPE' is an integral type.  This 'struct' derives
    // from 'bsl::true_type' if the 'TYPE' is an integral type, and
    // 'bsl::false_type' otherwise.
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
