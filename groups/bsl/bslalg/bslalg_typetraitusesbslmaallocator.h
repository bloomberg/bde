// bslalg_typetraitusesbslmaallocator.h                               -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#define INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark types that use 'bslma' allocators.
//
//@DEPRECATED: Use 'bslma_usesbslmaallocator' instead.
//
//@CLASSES:
//  bslalg::TypeTraitUsesBslmaAllocator: uses 'bslma' allocators
//
//@SEE_ALSO: bslalg_constructorproxy
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitUsesBslmaAllocator', which allows the trait
// 'bslma::UsesBslmaAllocator' to be declared using the (deprecated)
// 'BSLALG_DECLARE_NESTED_TRAITS' macro.  See the 'bslma_usesbslmaallocator'
// component for details of this trait.
//
///Usage
///-----
// No Usage example is provided since this component is deprecated.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

namespace BloombergLP {

namespace bslalg {

                 //===================================
                 // struct TypeTraitUsesBslmaAllocator
                 //===================================

struct TypeTraitUsesBslmaAllocator {
    // A 'TYPE' with this trait uses an allocator derived from
    // 'bslma::Allocator' to supply memory.  See documentation on the
    // 'bslma_usesbslmaallocator' trait for the requirements on a 'TYPE' that
    // has this trait.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslma::UsesBslmaAllocator>
    {
        // This class template ties the 'bslalg::TypeTraitUsesBslmaAllocator'
        // trait tag to the 'bslma::UsesBslmaAllocator' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bslma::UsesBslmaAllocator<TYPE> { };
};

}  // close package namespace


#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

                 //==========================================
                 // struct bdealg_TypeTraitUsesBdemaAllocator
                 //==========================================

typedef bslalg::TypeTraitUsesBslmaAllocator bdealg_TypeTraitUsesBdemaAllocator;
    // This alias is defined for backward compatibility.

#endif // BDE_OMIT_INTERNAL_DEPRECATED


typedef bslalg::TypeTraitUsesBslmaAllocator bslalg_TypeTraitUsesBslmaAllocator;
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
