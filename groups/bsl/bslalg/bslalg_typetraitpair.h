// bslalg_typetraitpair.h                                             -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITPAIR
#define INCLUDED_BSLALG_TYPETRAITPAIR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as similar to 'std::pair'.
//
//@DEPRECATED: Do not use.
//
//@CLASSES:
//  bslalg::TypeTraitPair: for 'std::pair'-like classes
//
//@SEE_ALSO: bslalg_typetraits
//
//@AUTHOR: Herve Bronnimann (hbronnim)
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
//  mX.second            second_type
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

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

typedef bslalg::TypeTraitPair bslalg_TypeTraitPair;
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
