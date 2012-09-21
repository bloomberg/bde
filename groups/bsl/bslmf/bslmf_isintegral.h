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
//  bsl::is_integeral: standard meta-function for determining integral types
//
//@SEE_ALSO: bslmf_integralconstant
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_intgeral' ,
// which may be used to query whether a type is a integeral type as defined in
// section 3.9.1.7 of the C++11 standard [basic.fundamental] (excluding those
// types that are only available in C++11).
//
// 'bsl::is_integral' meets the requirements of the 'is_integral' template
// defined in the C++11 standard [meta.unary.cat].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Integral Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a integral type.
//
// First, we create two 'typedef's -- a integral type and a non-integral type:
//..
//  typedef void MyType;
//  typedef int  MyIntegralType;
//..
// Now, we instantiate the 'bsl::is_integral' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_integral<MyType>::value);
//  assert(true == bsl::is_integral<MyIntegralType>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECV
#include <bslmf_removecv.h>
#endif

namespace BloombergLP {
namespace bslmf {

template <typename TYPE>
struct IsIntegral_Imp : bsl::false_type {
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 'TYPE' is a integral type.  This generic default
    // template derives from 'bsl::false_type'.  Template specializations are
    // provided (below) that derives from 'bsl::true_type'.
};

template <>
struct IsIntegral_Imp<bool> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

template <>
struct IsIntegral_Imp<char> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

template <>
struct IsIntegral_Imp<wchar_t> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

template <>
struct IsIntegral_Imp<signed char> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

template <>
struct IsIntegral_Imp<unsigned char> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

template <>
struct IsIntegral_Imp<short> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

template <>
struct IsIntegral_Imp<unsigned short> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

template <>
struct IsIntegral_Imp<int> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

template <>
struct IsIntegral_Imp<unsigned int> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

template <>
struct IsIntegral_Imp<long int> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

template <>
struct IsIntegral_Imp<unsigned long int> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

template <>
struct IsIntegral_Imp<bsls::Types::Int64> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

template <>
struct IsIntegral_Imp<bsls::Types::Uint64> : bsl::true_type {
     // This partial specialization of 'IsIntegral_Imp' derives from
     // 'bsl::true_type' for when the (template parameter) 'TYPE' is a integral
     // type.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

template <typename TYPE>
struct is_integral
   : BloombergLP::bslmf::IsIntegral_Imp<typename remove_cv<TYPE>::type>::type {
    // This 'struct' template implements the 'is_integral' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 'TYPE' is a integral type.  This 'struct' derives
    // from 'bsl::true_type' if the 'TYPE' is a integral type and
    // 'bsl::false_type' otherwise.
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
