// bslmf_removepointer.h                                              -*-C++-*-
#ifndef INCLUDED_BSLMF_REMOVEPOINTER
#define INCLUDED_BSLMF_REMOVEPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Transform a pointer type to type pointed by pointer type
//
//@CLASSES:
//  bsl::remove_pointer: transform a pointer type to type pointed by pointer
//
//@SEE_ALSO: bslmf_addpointer
//
//@DESCRIPTION: This component defines a meta-function, 'bsl::remove_pointer',
// that may be used to get the type pointed to by a pointer type.
//
// 'bsl::remove_pointer' meets the requirements of the 'remove_pointer'
// template defined in the C++11 standard [meta.trans.ptr].
//
///Usage
///-----
// In this section we show intended use of this component.
//
///Example 1: Get the Type Pointed to by a Pointer Type
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to get the type pointed to by a poiner type.
//
// First, we create two 'typedef's -- a pointer type ('MyPtrType')
// and the type pointed to by the pointer type ('MyType'):
//..
//  typedef int   MyType;
//  typedef int * MyPtrType;
//..
// Now, we get the type pointed to by 'MyPtrType' using 'bsl::remove_pointer'
// and verify that the resulting type is the same as 'MyType':
//..
//  assert((bsl::is_same<bsl::remove_pointer<MyPtrType>::type,
//                       MyType>::value));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_REMOVECVQ
#include <bslmf_removecvq.h>
#endif

namespace BloombergLP {
namespace bslmf {

                         // ========================
                         // struct RemovePointer_Imp
                         // ========================

template <typename TYPE>
struct RemovePointer_Imp {
    // This 'struct' template provides an alias 'Type' that refers to the type
    // pointed to by the (template parameter) 'TYPE' if 'TYPE' is a
    // (non-cv-qualified) pointer type; otherwise, 'Type' refers to 'TYPE'.
    // This generic default template's 'Type' always refers to 'TYPE'.  A
    // template specialization (below) handles when 'TYPE' is a pointer type.

    typedef TYPE Type;
        // This 'typedef' is an alias to the (template parameter) 'TYPE'.
};

                         // ================================
                         // struct RemovePointer_Imp<TYPE *>
                         // ================================

template <typename TYPE>
struct RemovePointer_Imp<TYPE *> {
     // This partial specialization of 'RemovePointer_Imp', for when the
     // (template parameter) 'TYPE' is a pointer type, provides an alias 'TYPE'
     // that refers to the type pointed to by 'TYPE'.

    typedef TYPE Type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 'TYPE'.
};

}  // close package namespace
}  // close enterprise namespace

namespace bsl {

                         // =====================
                         // struct remove_pointer
                         // =====================

template <typename TYPE>
struct remove_pointer {
    // This 'struct' template implements the 'remove_pointer' meta-function
    // defined in the C++11 standard [meta.trans.ptr], providing an alias,
    // 'type' that returns the result.  If the (template parameter) 'TYPE' is a
    // (possibly cv-qualified) pointer type, then 'type' is an alias to the
    // type pointed to by 'TYPE'; otherwise, 'type' is an alias to 'TYPE'.

    typedef typename BloombergLP::bslmf::RemovePointer_Imp<
                typename remove_cv<TYPE>::type>::Type
                                                                          type;
        // This 'typedef' is an alias to the type pointed to by the (template
        // parameter) 'TYPE' if 'TYPE' is a (possibly cv-qualified) pointer
        // type; otherwise, 'type' is an alias to 'TYPE'.
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
