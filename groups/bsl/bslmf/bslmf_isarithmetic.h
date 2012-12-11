// bslmf_isarithmetic.h                                               -*-C++-*-
#ifndef INCLUDED_BSLMF_ISARITHMETIC
#define INCLUDED_BSLMF_ISARITHMETIC

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time check for determining arithmetic types.
//
//@CLASSES:
//  bsl::is_arithmetic: standard meta-function for determining arithmetic types
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::is_arithmetic',
// that may be used to query whether a template parameter type is an arithmetic
// type.
//
// 'bsl::is_arithmetic' meets the requirements of the 'is_arithmetic' template
// defined in the C++11 standard [meta.unary.comp].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Verify Arithmetic Types
/// - - - - - - - - - - - - - - - - -
// Suppose that we want to assert whether a set of types are arithmetic types.
//
// Now, we instantiate the 'bsl::is_arithmetic' template for these types, and
// assert the 'value' static data member of each instantiation:
//..
//  assert(false == bsl::is_arithmetic<int&>::value);
//  assert(false == bsl::is_arithmetic<int*>::value);
//  assert(true  == bsl::is_arithmetic<int >::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISFLOATINGPOINT
#include <bslmf_isfloatingpoint.h>
#endif

#ifndef INCLUDED_BSLMF_ISINTEGRAL
#include <bslmf_isintegral.h>
#endif

namespace bsl {
                         // ====================
                         // struct is_arithmetic
                         // ====================

template <class TYPE>
struct is_arithmetic
    : integral_constant<bool,
                        is_integral<TYPE>::value
                        || is_floating_point<TYPE>::value> {
    // This 'struct' template implements the 'is_arithmetic' meta-function
    // defined in the C++11 standard [meta.unary.comp] to determine if the
    // (template parameter) 'TYPE' is an arithmetic type.  This 'struct'
    // derives from 'bsl::true_type' if the 'TYPE' is an arithmetic type,
    // and from 'bsl::false_type' otherwise.
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
