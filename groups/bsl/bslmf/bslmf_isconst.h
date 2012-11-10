// bslmf_isconst.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLMF_ISCONST
#define INCLUDED_BSLMF_ISCONST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

//@PURPOSE: Provide a compile-time check for 'const'-qualified types.
//
//@CLASSES:
//  bsl::is_const: meta-function for determining 'const'-qualified types
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_const' , that
// may be used to query whether a type is 'const'-qualified as defined in
// section the C++11 standard [basic.type.qualifier].
//
// 'bsl::is_const' meets the requirements of the 'is_const' template defined in
// the C++11 standard [meta.unary.prop].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify 'Const' Types
///- - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a
// 'const'-qualified.
//
// First, we create two 'typedef's -- a 'const'-qualified type and a
// unqualified type:
//..
//  typedef int        MyType;
//  typedef const int  MyConstType;
//..
// Now, we instantiate the 'bsl::is_const' template for each of the
// 'typedef's and assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_const<MyType>::value);
//  assert(true == bsl::is_const<MyConstType>::value);
//..

namespace bsl {

template <typename TYPE>
struct is_const : false_type {
    // This 'struct' template implements the 'is_const' meta-function defined
    // in the C++11 standard [meta.unary.cat] to determine if the (template
    // parameter) 'TYPE' is 'const'-qualified.  This 'struct' derives from
    // 'bsl::true_type' if the 'TYPE' is 'const'-qualified, and
    // 'bsl::false_type' otherwise.  Note that this generic default template
    // derives from 'bsl::false_type'.  A template specialization is provided
    // (below) that derives from 'bsl::true_type'.
};

template <typename TYPE>
struct is_const<TYPE const> : true_type {
     // This partial specialization of 'is_const' derives from 'bsl::true_type'
     // for when the (template parameter) 'TYPE' is 'const'-qualified.
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
