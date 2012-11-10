// bslalg_typetraitbitwisecopyable.h                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#define INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for bit-wise copyable classes.
//
//@CLASSES:
//  bslalg::TypeTraitBitwiseCopyable: bit-wise copyable trait
//
//@SEE_ALSO: bslmf_typetraits
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitBitwiseCopyable'.  An object of a 'TYPE' that has the
// bit-wise copyable trait can be copied either by invoking the copy
// constructor or by copying the footprint (i.e., the 'sizeof(TYPE)' bytes at
// the object address) using 'memcpy'.
//
// Note also that this type is a synonym for having a trivial destructor, as in
// almost every case a bitwise copyable class does not need to perform any
// operation on destruction.  It is possible to write types that are
// operationally bitwise copyable (i.e., copy constructor can be implemented in
// terms of 'memcpy') but have a non-trivial destructor, but most examples of
// such types are contrived.  In any case, this trait should not be attached to
// such types.
//
// This component is used by various 'bslalg' components for providing
// optimized primitives for types that have the bit-wise copyable trait.  The
// major benefit of this trait is not for a single object but for an array of
// such types, as a loop can be replaced by a single call to 'memcpy'.
//
///What constitutes bit-wise copyability?
///--------------------------------------
// TBD: A short guide on when to attach this trait to a class should follow.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

namespace BloombergLP {

namespace bslalg {

                        // ==============================
                        // class TypeTraitBitwiseCopyable
                        // ==============================

struct TypeTraitBitwiseCopyable {
    // Objects of a type with this trait can be copied using 'memcpy'.  After
    // such a bitwise copy, both the original and the copy are valid.  Classes
    // with this trait are assumed to have trivial (no-op) destructors and are
    // assumed to be bitwise moveable (see the 'TypeTraitBitwiseMoveable'
    // trait).  Undefined behavior may result if this trait is assigned to a
    // type that allocates memory or other resources, uses virtual inheritance,
    // or places pointers to itself within other data structures.  Also,
    // objects of a type with this trait can be destroyed by a no-op, i.e., not
    // invoking the destructor, although it is safe to write zeros into the
    // memory footprint of the object.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bsl::is_trivially_copyable>
    {
        // This class template ties the 'bslalg::TypeTraitBitwiseCopyable'
        // trait tag to the 'bsl:is_trivially_copyable' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bsl::is_trivially_copyable<TYPE>::type { };
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
