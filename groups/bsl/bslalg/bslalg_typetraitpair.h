// bslalg_typetraitpair.h                                             -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITPAIR
#define INCLUDED_BSLALG_TYPETRAITPAIR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide facilities for identifying 'std::pair'-like classes.
//
//@DEPRECATED: Do not use.
//
//@CLASSES:
//  bslalg::TypeTraitPair: for 'std::pair'-like classes
//
//@SEE_ALSO: bslalg_typetraits
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitPair'.  A 'TYPE' that has this trait fulfills the
// following requirements, where 'mX' is a modifiable object and 'X' a
// non-modifiable object of 'TYPE':
//..
//  Valid expression     Type
//  ----------------     ----
//  TYPE::first_type
//  TYPE::second_type
//
//  mX.first             first_type
//  mX.second            second
//  X.first              const first_type
//  X.second             const second_type
//..
// Note that 'first' and 'second' are *not* member functions, but data members.

//TDB #ifdef BDE_OMIT_DEPRECATED // DEPRECATED
//TBD #error "bslalg_typetraitpair is deprecated"
//TBD #endif

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISPAIR
#include <bslmf_ispair.h>
#endif

namespace BloombergLP {

namespace bslalg {

                        // ====================
                        // struct TypeTraitPair
                        // ====================

struct TypeTraitPair {
    // A type, 'T', with this trait has two data members, 'first' and 'second'
    // of types 'T::first_type' and 'T::second_type', respectively.
    // Metafunctions can be used to probe and combine the traits of the
    // individual pair members.  For example, the pair is bitwise moveable only
    // if both 'first_type' and 'second_type' have the
    // 'TypeTraitBitwiseMoveable' trait.  User-defined types will rarely need
    // this trait.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslmf::IsPair>
    {
        // This class template ties the 'bslalg::TypeTraitPair' trait tag to
        // the 'bslmf::IsPair' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bslmf::IsPair<TYPE>::type { };
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
