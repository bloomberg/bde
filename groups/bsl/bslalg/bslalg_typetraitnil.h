// bslalg_typetraitnil.h                                              -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITNIL
#define INCLUDED_BSLALG_TYPETRAITNIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as having no other traits.
//
//@CLASSES:
//  bslalg::TypeTraitBitwiseCopyable: bit-wise copyable trait
//
//@SEE_ALSO:
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitNil'.  This trait is assigned by default to any class that
// does not have any other trait.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_NIL
#include <bslmf_nil.h>
#endif

namespace BloombergLP {

namespace bslalg {

                            //=============
                            // TypeTraitNil
                            //=============

struct TypeTraitNil
{
    // Nil trait -- every type has this trait.

    template <class TYPE>
    struct NestedTraitDeclaration
    {
        // This metafunction returns 'true_type' for any class that is queried
        // for the nil trait.
    };

    template <class TYPE>
    struct Metafunction : bsl::true_type { };
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ===========================================================================
//                           BACKWARD COMPATIBILITY
// ===========================================================================

#ifdef bslalg_TypeTraitNil
#undef bslalg_TypeTraitNil
#endif
#define bslalg_TypeTraitNil bslalg::TypeTraitNil
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
