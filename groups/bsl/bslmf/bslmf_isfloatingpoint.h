// bslmf_isfloatingpoint.h                                            -*-C++-*-
#ifndef INCLUDED_BSLMF_ISFLOATINGPOINT
#define INCLUDED_BSLMF_ISFLOATINGPOINT

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for floating-point types.
//
//@CLASSES:
//  bsl::is_floating_point: meta-function for determining floating-point types
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_floating_point', that may be used to query whether a type is a
// (possibly cv-qualified) floating-point type as defined in section 3.9.1.8 of
// the C++11 standard [basic.fundamental].
//
// 'bsl::is_floating_point' meets the requirements of the 'is_floating_point'
// template defined in the C++11 standard [meta.unary.cat].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Floating-Point Types
/// - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a floating-point
// type.
//
// First, we create two 'typedef's -- a floating-point type and a
// non-floating-point type:
//..
//  typedef void  MyType;
//  typedef float MyFloatingPointType;
//..
// Now, we instantiate the 'bsl::is_floating_point' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_floating_point<MyType>::value);
//  assert(true  == bsl::is_floating_point<MyFloatingPointType>::value);
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

namespace BloombergLP {
namespace bslmf {

                         // ==========================
                         // struct IsFloatingPoint_Imp
                         // ==========================

template <class TYPE>
struct IsFloatingPoint_Imp : bsl::false_type {
    // This 'struct' template implements a meta-function to determine whether
    // the (template parameter) 'TYPE' is a floating-point type.  This generic
    // default template derives from 'bsl::false_type'.  Template
    // specializations are provided (below) that derive from 'bsl::true_type'.
};

template <>
struct IsFloatingPoint_Imp<float> : bsl::true_type {
     // This specialization of 'IsFloatingPoint_Imp', for when the (template
     // parameter) 'TYPE' is 'float', derives from 'bsl::true_type'.
};

template <>
struct IsFloatingPoint_Imp<double> : bsl::true_type {
     // This specialization of 'IsFloatingPoint_Imp', for when the (template
     // parameter) 'TYPE' is 'double', derives from 'bsl::true_type'.
};

template <>
struct IsFloatingPoint_Imp<long double> : bsl::true_type {
     // This specialization of 'IsFloatingPoint_Imp', for when the (template
     // parameter) 'TYPE' is 'long double', derives from 'bsl::true_type'.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // ========================
                         // struct is_floating_point
                         // ========================

template <class TYPE>
struct is_floating_point
    : BloombergLP::bslmf::IsFloatingPoint_Imp<
                                typename remove_cv<TYPE>::type>::type {
    // This 'struct' template implements the 'is_floating_point' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 'TYPE' is a floating-point type.  This 'struct'
    // derives from 'bsl::true_type' if the 'TYPE' is a floating-point type,
    // and 'bsl::false_type' otherwise.
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
