// bslalg_typetraithastrivialdefaultconstructor.h                     -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR
#define INCLUDED_BSLALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a primitive type trait for class with trivial default ctor.
//
//@CLASSES:
//  bslalg::TypeTraitHasTrivialDefaultConstructor: has trivial default ctor
//
//@SEE_ALSO: bslmf_typetraits
//
//@DESCRIPTION: This component provides a single traits class.
// 'bslalg::TypeTraitHasTrivialDefaultConstructor'.  An object of a 'TYPE' that
// has the trivial default constructor trait can be initialized either by
// invoking the default constructor or by filling the footprint (i.e., the
// 'sizeof(TYPE)' bytes at the object address) with 0 using 'memset'.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#include <bslmf_istriviallydefaultconstructible.h>
#endif

namespace BloombergLP {

namespace bslalg {

            //=============================================
            // struct TypeTraitHasTrivialDefaultConstructor
            //=============================================

struct TypeTraitHasTrivialDefaultConstructor {
    // Objects of a type with this trait can be default-initialized by simply
    // writing zeros into the memory footprint of the object.  Although it is
    // possible for such a type not to be bitwise copyable, undefined behavior
    // may result if this trait is assigned to such a type.  (See the
    // description of 'TypeTraitBitwiseCopyable'.)

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE,
                                      bsl::is_trivially_default_constructible>
    {
        // This class template ties the
        // 'bslalg::TypeTraitBitwiseEqualityComparable'
        // trait tag to the 'bslmf::IsBitwiseEqualityComparable'
        // trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bsl::is_trivially_default_constructible<TYPE>::type
    {
    };
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
