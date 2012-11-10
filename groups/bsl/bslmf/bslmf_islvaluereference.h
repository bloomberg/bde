// bslmf_islvaluereference.h                                          -*-C++-*-
#ifndef INCLUDED_BSLMF_ISLVALUEREFERENCE
#define INCLUDED_BSLMF_ISLVALUEREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for lvalue reference types.
//
//@CLASSES:
//  bsl::is_lvalue_reference: standard meta-function for lvalue reference types
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a meta-function,
// 'bsl::is_lvalue_reference', which may be used to query whether a type is a
// lvalue reference type.
//
// 'bsl::is_lvalue_reference' meets the requirements of the
// 'is_lvalue_reference' template defined in the C++11 standard
// [meta.unary.cat].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Lvalue Reference Types
/// - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a particular type is a lvalue
// reference type.
//
// Now, we instantiate the 'bsl::is_lvalue_reference' template for both a
// non-reference type and a reference type, and assert the 'value' static data
// member of each instantiation:
//..
//  assert(false == bsl::is_lvalue_reference<int>::value);
//  assert(true  == bsl::is_lvalue_reference<int&>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

namespace bsl {

template <typename TYPE>
struct is_lvalue_reference : false_type
{
    // This 'struct' template provides a meta-function to determine whether the
    // (template parameter) 'TYPE' is a (cv-qualified) lvalue reference type.
    // This generic default template derives from 'bsl::false_type'.  A
    // template specialization is provided (below) that derives from
    // 'bsl::true_type'.
};

template <typename TYPE>
struct is_lvalue_reference<TYPE &> : true_type
{
    // This partial specialization of 'is_lvalue_reference' derives from
    // 'bsl::true_type' for when the (template parameter) 'TYPE' is a lvalue
    // reference type.
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
