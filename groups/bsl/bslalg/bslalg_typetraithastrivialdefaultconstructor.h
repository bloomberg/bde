// bslalg_typetraithastrivialdefaultconstructor.h                     -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR
#define INCLUDED_BSLALG_TYPETRAITHASTRIVIALDEFAULTCONSTRUCTOR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as having a trivial default ctor.
//
//@DEPRECATED: Use 'bslmf_istriviallydefaultconstructible' instead.
//
//@CLASSES:
//  bslalg::TypeTraitHasTrivialDefaultConstructor: has trivial default ctor
//
//@SEE_ALSO:
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitHasTrivialDefaultConstructor', which allows the trait
// 'bsl::is_trivially_default_constructible' to be declared using the
// (deprecated) 'BSLALG_DECLARE_NESTED_TRAITS' macro.  See the
// 'bslmf_istriviallydefaultconstructible' component for details of this trait.
//
///Usage
///-----
// No Usage example is provided since this component is deprecated.

#include <bslscm_version.h>

#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_nestedtraitdeclaration.h>

namespace BloombergLP {

namespace bslalg {

            //=============================================
            // struct TypeTraitHasTrivialDefaultConstructor
            //=============================================

struct TypeTraitHasTrivialDefaultConstructor {
    // A 'TYPE' with this trait has a trivial default constructor.  Objects of
    // a type with this trait can be default-initialized by simply writing
    // zeros into the memory footprint of the object.  Although it is possible
    // for such a type not to be bitwise copyable, undefined behavior may
    // result if this trait is assigned to such a type.  (See the description
    // of 'TypeTraitBitwiseCopyable'.)

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE,
                                      bsl::is_trivially_default_constructible>
    {
        // This class template ties the
        // 'bslalg::TypeTraitHasTrivialDefaultConstructor' trait tag to the
        // 'bsl::is_trivially_default_constructible' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bsl::is_trivially_default_constructible<TYPE>::type
    {
    };
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

typedef bslalg::TypeTraitHasTrivialDefaultConstructor
                                  bslalg_TypeTraitHasTrivialDefaultConstructor;
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
