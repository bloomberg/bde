// bslalg_typetraitbitwisemoveable.h                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#define INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for bit-wise moveable classes.
//
//@CLASSES:
//  bslalg::TypeTraitBitwiseMoveable: bit-wise moveable trait
//
//@SEE_ALSO: bslmf_typetraits
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitBitwiseMoveable'.  An object of a 'TYPE' that has the
// bit-wise moveable trait can be copied either by invoking the move
// constructor or by moving the footprint (i.e., the 'sizeof(TYPE)' bytes at
// the object address) using 'memcpy'.
//
// This component is used by various 'bslalg' components for providing
// optimized primitives for types that have the bit-wise moveable trait.  The
// major benefit of this trait is not for a single object but for an array of
// such types, as a loop can be replaced by a single call to 'memcpy'.
//
///What constitutes bit-wise movability?
///-------------------------------------
// TBD: A short guide on when to attach this trait to a class should follow.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

namespace BloombergLP {

namespace bslalg {

                        // ==============================
                        // class TypeTraitBitwiseMoveable
                        // ==============================

struct TypeTraitBitwiseMoveable {
    // Objects of a type with this trait can be "moved" from one memory
    // location to another using 'memmove' or 'memcpy'.  Although the result of
    // such a bitwise copy is two copies of the same object, this trait only
    // guarantees that one of the copies can be destroyed.  The other copy must
    // be considered invalid and its destructor must not be called.  Most
    // types, even those that contain pointers, are bitwise moveable.
    // Undefined behavior may result if this trait is assigned to a types that
    // contains pointers to its own internals, use virtual inheritance, or
    // places pointers to itself within other data structures.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslmf::IsBitwiseMoveable>
    {
        // This class template ties the 'bslalg::TypeTraitBitwiseMoveable'
        // trait tag to the 'bslmf::IsBitwiseMoveable' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bslmf::IsBitwiseMoveable<TYPE>::type { };
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
