// bslmf_conditional.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_CONDITIONAL
#define INCLUDED_BSLMF_CONDITIONAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compile-time conditional type selector.
//
//@CLASSES:
//  bsl::conditional: standard meta-function for conditional type selection
//
//@SEE_ALSO: bslmf_enableif
//
//@AUTHOR:
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::conditional',
// that may be used to conditionally select one of its two (template parameter)
// types based on a 'bool' (template parameter) value.
//
// 'bsl::conditional' meets the requirements of the 'conditional' template
// defined in the C++11 standard [meta.trans.other], providing a 'typedef'
// 'type' that is an alias to the first (template parameter) type if the
// (template parameter) value is 'true'; otherwise, 'type' is an alias to the
// second (template parameter) type.
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Conditionally Select From Two Types
/// - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to select between two types based on a 'bool' value.
//
// Now, we use 'bsl::conditional' to select between two types, 'int' and
// 'char', with a 'bool' value.  When the 'bool' is 'true', we select 'int';
// otherwise, we select 'char'.  We verify that our code behaves correctly by
// asserting the result of 'bsl::conditional' with the expected type using
// 'bsl::is_same':
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
    // This 'struct' template implements the 'conditional' meta-function
    // defined in the C++ standard [meta.trans.other], providing an alias,
    // 'type', that returns the result.  If the (template parameter) value
    // 'COND' is 'true', then 'type' has the same type as the (template
    // parameter) type 'TRUE_TYPE'; otherwise, 'type' has the same type as the
    // (template parameter) type 'FALSE_TYPE'.  Note that this generic default
    // template defines 'type' to be an alias to 'TRUE_TYPE' for when 'COND' is
    // 'true'.  A template specialization is provided (below) handles the case
    // for when 'COND' is 'false'.

    typedef TRUE_TYPE type;
        // This 'typedef' is an alias to the (template parameter) type
        // 'TRUE_TYPE'.
};

template <class TRUE_TYPE, class FALSE_TYPE>
struct conditional<false, TRUE_TYPE, FALSE_TYPE> {
    // This partial specialization of 'bsl::conditional', for when the
    // (template parameter) value 'COND' is 'false', provides a 'typedef'
    // 'type' that is an alias to the (template parameter) type 'FALSE_TYPE'.

    typedef FALSE_TYPE type;
        // This 'typedef' is an alias to the (template parameter) type
        // 'FALSE_TYPE'.
};

}  // close namespace bsl

#endif

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg L.P.
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
