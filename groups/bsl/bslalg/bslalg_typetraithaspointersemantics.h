// bslalg_typetraithaspointersemantics.h                              -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITHASPOINTERSEMANTICS
#define INCLUDED_BSLALG_TYPETRAITHASPOINTERSEMANTICS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for classes with pointer semantics.
//
//@CLASSES:
//  bslalg::TypeTraitHasPointerSemantics: has pointer semantics
//
//@SEE_ALSO: bslmf_typetraits
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitHasPointerSemantics'.  A type with this trait is said to
// have "pointer semantics".  That is the type behaves as if it were a
// fundamental pointer type.  The type must define(at a minimum) 'operator*'
// and 'operator->'.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_HASPOINTERSEMANTICS
#include <bslmf_haspointersemantics.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

namespace BloombergLP {

namespace bslalg {

                 //====================================
                 // struct TypeTraitHasPointerSemantics
                 //====================================

struct TypeTraitHasPointerSemantics {
    // A type with this trait is said to have "pointer semantics".  That is the
    // type behaves as if it were a fundamental pointer type.  The type must
    // define(at a minimum) 'operator*' and 'operator->'.  Note that simply
    // providing the above operators does not imply that a type has pointer
    // semantics.  This trait is intended for "wrapper" types that behave as
    // pointers.  Some examples of such types include 'std::auto_ptr', and
    // 'bslma::ManagedPtr'.  This trait is generally used by objects that
    // invoke the wrapped type.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslmf::HasPointerSemantics>
    {
        // This class template ties the 'bslalg::TypeTraitHasPointerSemantics'
        // trait tag to the 'bslmf::HasPointerSemantics' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bslmf::HasPointerSemantics<TYPE>::type { };
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
