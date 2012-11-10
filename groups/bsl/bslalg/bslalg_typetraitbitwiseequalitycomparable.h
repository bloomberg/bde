// bslalg_typetraitbitwiseequalitycomparable.h                        -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE
#define INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for bit-wise eq.-comparable classes.
//
//@CLASSES:
//  bslalg::TypeTraitBitwiseEqualityComparable: bit-wise eq.-comparable trait
//
//@SEE_ALSO: bslmf_typetraits
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitBitwiseEqualityComparable'.  Two objects of a 'TYPE' that
// has the bit-wise equality comparable trait can be compared either by
// invoking the equality operator or by comparing the footprint (i.e., the
// 'sizeof(TYPE)' bytes at the respective object addresses) using 'memcmp'.
//
// This component is used by various 'bslalg' components for providing
// optimized primitives for types that have the bit-wise equality comparable
// trait.  The major benefit of this trait is not for a single object but for
// an array of such types, as a loop can be replaced by a single call to
// 'memcmp'.
//
///What constitutes bit-wise equality comparability?
///-------------------------------------------------
// TBD: A short guide on when to attach this trait to a class should follow.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE
#include <bslmf_isbitwiseequalitycomparable.h>
#endif

namespace BloombergLP {

namespace bslalg {

              //==========================================
              // struct TypeTraitBitwiseEqualityComparable
              //==========================================

struct TypeTraitBitwiseEqualityComparable {
    // Objects of a type with this trait can be compared for equality using
    // 'memcmp'.  If two objects compare equal, then 'memcmp' returns 0,
    // otherwise it returns non-zero.  Classes with this traits are assumed to
    // have no padding and to be bit-wise copyable (see the
    // 'TypeTraitBitwiseCopyable' trait).  Undefined behavior may result if
    // this trait is assigned to a type that does not have an 'operator==' or
    // whose 'operator==' implementation may return different from comparing
    // the footprints with 'memcmp' .

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslmf::IsBitwiseEqualityComparable>
    {
        // This class template ties the
        // 'bslalg::TypeTraitBitwiseEqualityComparable' trait tag to the
        // 'bslmf::IsBitwiseEqualityComparable' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bslmf::IsBitwiseEqualityComparable<TYPE>::type { };
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
