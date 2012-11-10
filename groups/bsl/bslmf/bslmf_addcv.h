// bslmf_addcv.h                                                      -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDCV
#define INCLUDED_BSLMF_ADDCV

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function for adding a top-level cv-qualifier
//
//@CLASSES:
//  bsl::add_const: meta-function for adding a top-level cv-qualifier
//
//@SEE_ALSO: bslmf_removeconst
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::add_cv', that may
// be used to add a top-level 'const'-qualifier and a 'volatile'-qualifier to a
// type if it is not a reference, a function or already 'const'-qualified and
// 'volatile'-qualified at the top-level.
//
// 'bsl::add_cv' meets the requirements of the 'add_cv' template defined in the
// C++11 standard [meta.trans.cv].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Adding The 'const'-qualifier and 'volatile'-qualifier to A Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to add the 'const'-qualifier and 'volatile'-qualifier
// to a particular type.
//
// First, we create two 'typedef's -- a 'const'-qualified and
// 'volatile'-qualified type ('MyCvType') and the same type without the
// cv-qualifiers ('MyType'):
//..
//  typedef int                MyType;
//  typedef const volatile int MyCvType;
//..
// Now, we add the the 'const'-qualifier and the 'volatile'-qualifier to
// 'MyType' using 'bsl::add_cv' and verify that the resulting type is the
// same as 'MyCvType':
//..
//  assert(true ==
//                 (bsl::is_same<bsl::add_cv<MyType>::type, MyCvType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ADDCONST
#include <bslmf_addconst.h>
#endif

#ifndef INCLUDED_BSLMF_ADDVOLATILE
#include <bslmf_addvolatile.h>
#endif

namespace bsl {

                         // =============
                         // struct add_cv
                         // =============

template <typename TYPE>
struct add_cv {
    // This 'struct' template implements the 'add_cv' meta-function defined in
    // the C++11 standard [meta.trans.cv] to provide a 'typedef' 'type'.  If
    // the (template parameter) 'TYPE' is not a reference, nor a function, nor
    // already 'const'-qualified and 'volatile'-qualified, then 'type' is an
    // alias to the same type as 'TYPE' except that the top-level
    // 'const'-qualifier and 'volatile'-qualifier has been added; otherwise,
    // 'type' is an alias to 'TYPE'.

    typedef typename add_const<typename add_volatile<TYPE>::type>::type type;
        // This 'typedef' is an alias alias to the same type as the (template
        // parameter) 'TYPE' except that the top-level 'const'-qualifier and
        // 'volatile'-qualifier has been added if 'TYPE' is not a reference,
        // nor a function, nor already 'const'-qualified and
        // 'volatile'-qualified; otherwise, 'type' is an alias to 'TYPE'.

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
