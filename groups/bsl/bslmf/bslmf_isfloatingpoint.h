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
// 'bsl::is_floating_point' , which may be used to query whether a type is a
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
///- - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a floating-point
// type.
//
// First, we create two 'typedef's -- a floating-point type and a
// non-floating-point type:
//..
//  typedef void MyType;
//  typedef float  MyFloatingPointType;
//..
// Now, we instantiate the 'bsl::is_floating_point' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_floating_point<MyType>::value);
//  assert(true == bsl::is_floating_point<MyFloatingPointType>::value);
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

template <typename TYPE>
struct IsFloatingPoint_Imp : bsl::false_type {
    // This 'struct' template implements a meta-function to determine whether
    // the (template parameter) 'TYPE' is a floating-point type.  This generic
    // default template derives from 'bsl::false_type'.  Template
    // specializations are provided (below) that derives from 'bsl::true_type'.
};

template <>
struct IsFloatingPoint_Imp<float> : bsl::true_type {
     // This specialization of 'IsFloatingPoint_Imp', for when the (template
     // parameter) 'TYPE' is a 'float', derives from 'bsl::true_type'.
};

template <>
struct IsFloatingPoint_Imp<double> : bsl::true_type {
     // This specialization of 'IsFloatingPoint_Imp', for when the (template
     // parameter) 'TYPE' is a 'double', derives from 'bsl::true_type'.
};

template <>
struct IsFloatingPoint_Imp<long double> : bsl::true_type {
     // This specialization of 'IsFloatingPoint_Imp', for when the (template
     // parameter) 'TYPE' is a 'long double', derives from 'bsl::true_type'.
};

}
}

namespace bsl {

                         // ========================
                         // struct is_floating_point
                         // ========================

template <typename TYPE>
struct is_floating_point
    : BloombergLP::bslmf::IsFloatingPoint_Imp<
                                typename remove_cv<TYPE>::type>::type {
    // This 'struct' template implements the 'is_floating_point' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 'TYPE' is a floating-point type.  This 'struct'
    // derives from 'bsl::true_type' if the 'TYPE' is a floating-point type,
    // and 'bsl::false_type' otherwise.
};

}

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
