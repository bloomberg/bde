// bslmf_conditional.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_CONDITIONAL
#define INCLUDED_BSLMF_CONDITIONAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time conditional type selection
//
//@CLASSES:
//  bsl::conditional: standard meta-function for conditional type selection
//
//@SEE_ALSO: bslmf_integralconstant
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::conditional',
// which may be used to tranform into one of the two given types based on its
// (template parameter) 'bool' type value.
//
// 'bsl::conditional' meets the requirements of the 'conditional' template
// defined in the C++11 standard [meta.trans.other].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Conditional Type Transformation Based on Boolean Value
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to select between two types, 'int' and 'char', based on
// 'bool' value.  If value is 'true', type 'int' is selected.  Otherwise type
// 'char' is selected.
//
// Now, we instantiate the 'bsl::conditional' template using 'int', 'char', and
// each of the two 'bool' values.  We use the 'bsl::is_same' meta-function to
// assert the 'type' static data member of each instantiation:
//..
//  assert(true ==
//      (bsl::is_same<bsl::conditional<true,  int, char>::type, int >::value));
//  assert(true ==
//      (bsl::is_same<bsl::conditional<false, int, char>::type, char>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace bsl {

template <bool COND, typename TRUE_TYPE, typename FALSE_TYPE>
struct conditional
{
    typedef TRUE_TYPE type;
};

template <typename TRUE_TYPE, typename FALSE_TYPE>
struct conditional<false, TRUE_TYPE, FALSE_TYPE>
{
    typedef FALSE_TYPE type;
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
