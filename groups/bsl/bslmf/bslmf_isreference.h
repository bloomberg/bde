// bslmf_isreference.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_ISREFERENCE
#define INCLUDED_BSLMF_ISREFERENCE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to test reference types.
//
//@CLASSES:
//  bsl::is_reference: standard meta-function for testing reference types
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_reference',
// that may be used to query whether a type is an (lvalue or rvalue) reference
// type.
//
// 'bsl::is_reference' meets the requirements of the 'is_reference' template
// defined in the C++11 standard [meta.unary.comp].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Reference Types
///- - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are (lvalue or rvalue)
// reference types.
//
// Now, we instantiate the 'bsl::is_reference' template for a non-reference
// type, an lvalue reference type, and an rvalue reference type, and assert the
// 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_reference<int>::value);
//  assert(true  == bsl::is_reference<int&>::value);
//#if defined(BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES)
//  assert(true  == bsl::is_reference<int&&>::value);
//#endif
//..
// Note that rvalue reference is a feature introduced in the C++11 standand,
// and may not be supported by all compilers.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISLVALUEREFERENCE
#include <bslmf_islvaluereference.h>
#endif

#ifndef INCLUDED_BSLMF_ISRVALUEREFERENCE
#include <bslmf_isrvaluereference.h>
#endif

namespace bsl {

                         // ===================
                         // struct is_reference
                         // ===================

template <typename TYPE>
struct is_reference : integral_constant<bool,
                                        is_lvalue_reference<TYPE>::value
                                     || is_rvalue_reference<TYPE>::value> {
    // This 'struct' template implements the 'is_reference' meta-function
    // defined in the C++11 standard [meta.unary.comp] to determine if the
    // (template parameter) 'TYPE' is a (lvalue or rvalue) reference type.
    // This 'struct' derives from 'bsl::true_type' if the 'TYPE' is a reference
    // type, and from 'bsl::false_type' otherwise.
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
