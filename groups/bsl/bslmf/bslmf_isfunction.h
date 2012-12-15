// bslmf_isfunction.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ISFUNCTION
#define INCLUDED_BSLMF_ISFUNCTION

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for determining function types.
//
//@CLASSES:
//  bsl::is_function: standard meta-function for determining function types
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_function',
// that may be used to query whether a template parameter type is a function
// type.
//
// 'bsl::is_function' meets the requirements of the 'is_function' template
// defined in the C++11 standard [meta.unary.cat].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Function Types
/// - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are function types.
//
// Now, we instantiate the 'bsl::is_function' template for both a non-function
// type and a function type, and assert the 'value' static data member of each
// instantiation:
//..
//  assert(false == bsl::is_function<int>::value);
//  assert(true  == bsl::is_function<int (int)>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ADDPOINTER
#include <bslmf_addpointer.h>
#endif

#ifndef INCLUDED_BSLMF_FUNCTIONPOINTERTRAITS
#include <bslmf_functionpointertraits.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

namespace bsl {

                             // ==================
                             // struct is_function
                             // ==================

template <class TYPE>
struct is_function
    : integral_constant<
            bool,
            BloombergLP::bslmf::FunctionPointerTraits<
                      typename add_pointer<TYPE>::type>::IS_FUNCTION_POINTER> {
    // This 'struct' template implements the 'is_function' meta-function
    // defined in the C++11 standard [meta.unary.cat] to determine if the
    // (template parameter) 'TYPE' is a function type.  This 'struct' derives
    // from 'bsl::true_type' if the 'TYPE' is a function type, and
    // from 'bsl::false_type' otherwise.
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
