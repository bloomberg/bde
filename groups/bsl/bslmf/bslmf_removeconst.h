// bslmf_removeconst.h                                                -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVECONST
#define INCLUDED_BSLMF_REMOVECONST

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for removing top-level 'const'-qualifier
//
//@CLASSES:
//  bsl::remove_const: meta-function for removing top-level 'const'-qualifier
//
//@SEE_ALSO: bslmf_addconst
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_const',
// that may be used to remove any top-level 'const'-qualifier from a type.
//
// 'bsl::remove_const' meets the requirements of the 'remove_const' template
// defined in the C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Removing the 'const'-qualifier of a Type
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to remove any top-level 'const'-qualifier from a
// particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified type ('MyConstType')
// and the same type without the 'const'-qualifier ('MyType'):
//..
//  typedef int       MyType;
//  typedef const int MyConstType;
//..
// Now, we remove the 'const'-qualifier from 'MyConstType' using
// 'bsl::remove_const' and verify that the resulting type is the same as
// 'MyType':
//..
//  assert(true ==
//        (bsl::is_same<bsl::remove_const<MyConstType>::type, MyType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

namespace bsl {

                         // ===================
                         // struct remove_const
                         // ===================

template <typename TYPE>
struct remove_const {
    // This 'struct' template implements the 'remove_const' meta-function
    // defined in the C++11 standard [meta.trans.cv], providing an alias,
    // 'type', that returns the result.  'type' has the same type as the
    // (template parameter) 'TYPE' except that any top-level 'const'-qualifier
    // has been removed.  Note that this generic default template provides a
    // 'type' that is an alias to 'TYPE' for when 'TYPE' is not
    // 'const'-qualified.  A template specialization is provided (below) that
    // removes the 'const'-qualifier for when 'TYPE' is 'const'-qualified.

    typedef TYPE type;
        // This 'typedef' is an alias to the (template parameter) 'TYPE'.
};

                         // ===============================
                         // struct remove_const<TYPE const>
                         // ===============================

template <typename TYPE>
struct remove_const<TYPE const> {
     // This partial specialization of 'bsl::remove_const', for when the
     // (template parameter) 'TYPE' is 'const'-qualified, provides a 'typedef'
     // 'type' that has the 'const'-qualifier removed.

    typedef TYPE type;
        // This 'typedef' to a type that is the same as the (template
        // parameter) 'TYPE' except with the 'const'-qualifier removed.

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
