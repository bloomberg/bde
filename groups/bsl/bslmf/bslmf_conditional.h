// bslmf_conditional.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_CONDITIONAL
#define INCLUDED_BSLMF_CONDITIONAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time conditional type transformation.
//
//@CLASSES:
//  bsl::conditional: standard meta-function for conditional type transforming
//
//@SEE_ALSO: bslmf_integralconstant, bslmf_enableif
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::conditional',
// that may be used to transform into one of the two given types based on its
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
// 'bool' value.  If the 'bool' value is 'true', the 'int' type is returned;
// otherwise the 'char' type is returned.
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
                           // ==================
                           // struct conditional
                           // ==================

template <bool COND, class TRUE_TYPE, class FALSE_TYPE>
struct conditional {
    // This 'struct' template implements a meta-function, 'bsl::conditional',
    // that transforms into one of the two given (template parameter) types,
    // 'TRUE_TYPE' and 'FALSE_TYPE', depending on value of the (template
    // parameter) 'COND'.  If 'COND' is 'true', this meta-function returns the
    // 'TRUE_TYPE', and returns the 'FALSE_TYPE' otherwise.  This generic
    // default template is instantiated when 'COND' is true.  There is another
    // partial specialization when 'COND' is false.

    typedef TRUE_TYPE type;
        // This 'typedef' defines the return type of this meta function.
};

template <class TRUE_TYPE, class FALSE_TYPE>
struct conditional<false, TRUE_TYPE, FALSE_TYPE> {
    // This partial specialization of 'bsl::conditional' defines a return type
    // to 'FALSE_TYPE' when it is instantiated to have the 'false' template
    // parameter value.

    typedef FALSE_TYPE type;
        // This 'typedef' defines the return type of this meta function.
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
