// bslalg_typetraithaspointersemantics.h                              -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITHASPOINTERSEMANTICS
#define INCLUDED_BSLALG_TYPETRAITHASPOINTERSEMANTICS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as having pointer semantics.
//
//@CLASSES:
//  bslalg::TypeTraitHasPointerSemantics: has pointer semantics
//
//@SEE_ALSO:
//
//@AUTHOR: Herve Bronnimann (hbronnim)
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

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

typedef bslalg::TypeTraitHasPointerSemantics
                                           bslalg_TypeTraitHasPointerSemantics;
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
