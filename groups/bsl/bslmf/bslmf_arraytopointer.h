// bslmf_arraytopointer.h                                             -*-C++-*-
#ifndef INCLUDED_BSLMF_ARRAYTOPOINTER
#define INCLUDED_BSLMF_ARRAYTOPOINTER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a meta-function to convert array types to pointer types.
//
//@CLASSES:
//  bslmf::ArrayToPointer: convert an array type to a pointer type
//  bslmf::ArrayToConstPointer: convert an array type to a 'const' pointer type
//
//@SEE_ALSO: bslmf_isarray, bslmf_forwardingtype
//
//@DESCRIPTION: This component provides a meta function for converting array
// types to pointer types.  The utility is generally used for in templates that
// require forwarding or storage of arguments that are passed a arrays(e.g.,
// "string literals").  For non array types, the type is left unmodified.  Note
// that 'bslmf::ArrayToPointer' and 'bslmf::ArrayToConstPointer' retain the CV
// qualifiers of the original type.  In other words, if the original array type
// was 'const' or 'volatile', or 'const volatile', the converted pointer type
// will also be 'const', 'volatile', or 'const volatile' respectively.
//
// When an explicit const pointer pointer type is needed(such as when accepting
// as argument, then 'bslmf::ArrayToConstPointer' should be used.
//
///Usage
///-----
// For example:
//..
//  assert(1 == bsl::is_same<bslmf::ArrayToPointer<int[5]>::Type
//                          , int *>::value);
//  assert(1 == bsl::is_same<bslmf::ArrayToPointer<int *>::Type
//                          , int *>::value);
//  assert(0 == bsl::is_same<bslmf::ArrayToPointer<int (*)[5]>::Type]
//                          , int **>::value);
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_CSTDDEF
#include <cstddef>       // for 'std::size_t'
#define INCLUDED_CSTDDEF
#endif

namespace BloombergLP {

namespace bslmf {

                            // =====================
                            // struct ArrayToPointer
                            // =====================

template <typename TYPE, typename ORIGINAL_TYPE>
struct ArrayToPointer_Imp;

template <typename TYPE>
struct ArrayToPointer {
    typedef typename ArrayToPointer_Imp<TYPE, TYPE>::Type Type;
};

template <typename TYPE>
struct ArrayToPointer<TYPE &> {
    typedef typename ArrayToPointer_Imp<TYPE, TYPE &>::Type Type;
};

                          // ==========================
                          // struct ArrayToConstPointer
                          // ==========================

template <typename TYPE>
struct ArrayToConstPointer {
    typedef typename ArrayToPointer_Imp<const TYPE, TYPE>::Type Type;
};

template <typename TYPE>
struct ArrayToConstPointer<TYPE &> {
    typedef typename ArrayToPointer_Imp<const TYPE, TYPE &>::Type Type;
};

                         // =========================
                         // struct ArrayToPointer_Imp
                         // =========================

template <typename TYPE, typename ORIGINAL_TYPE>
struct ArrayToPointer_Imp {
    typedef ORIGINAL_TYPE Type;
};

template <typename TYPE, std::size_t NUM_ELEMENTS, typename UNUSED>
struct ArrayToPointer_Imp<TYPE [NUM_ELEMENTS], UNUSED> {
    typedef TYPE *Type;
};

template <typename TYPE, typename UNUSED>
struct ArrayToPointer_Imp<TYPE [], UNUSED> {
    typedef TYPE *Type;
};

}  // close package namespace


}  // close enterprise namespace

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
