// bslmf_addpointer.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMF_ADDPOINTER
#define INCLUDED_BSLMF_ADDPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Transform a type to a pointer to that type
//
//@CLASSES:
//  bsl::add_pointer: transform a type to a pointer to that type
//
//@SEE_ALSO: bslmf_removepointer
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::add_pointer',
// that may be used to transform a type to a pointer to that type.
//
// 'bsl::add_pointer' meets the requirements of the 'add_pointer' template
// defined in the C++11 standard [meta.trans.ptr].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Transform Type to Pointer to the Type
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to transform a type to a pointer to that type.
//
// First, we create two 'typedef's -- a pointer type ('MyPtrType')
// and the type pointed to by the pointer type ('MyType'):
//..
//  typedef int   MyType;
//  typedef int * MyPtrType;
//..
// Now, we transform 'MyType' to a pointer to 'MyType' using 'bsl::add_pointer'
// and verify that the resulting type is the same as 'MyPtrType'.  and verify
// that the resulting type is the same as 'MyType':
//..
//  assert((bsl::is_same<bsl::add_pointer<MyType>::type,
//                       MyPtrType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVEREFERENCE
#include <bslmf_removereference.h>
#endif

namespace bsl {

                         // ==================
                         // struct add_pointer
                         // ==================

template <typename TYPE>
struct add_pointer {
    // This 'struct' template implements the 'add_pointer' meta-function
    // defined in the C++11 standard [meta.trans.ptr] to provide a 'typedef'
    // 'type'.  'type' is an alias to a pointer type that points to the
    // (template parameter) 'TYPE' if it's not a reference.  Otherwise, 'type'
    // is an alias to a pointer type that points to the type referred to by the
    // reference 'TYPE'.

    typedef typename remove_reference<TYPE>::type *type;
        // This 'typedef' is an alias to a pointer type that points to the
        // (template parameter) 'TYPE' if it's not a reference.  Otherwise,
        // this 'typedef' is an alias to a pointer type that points to the type
        // referred to by the reference 'TYPE'.
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
