// bslalg_typetraithasstliterators.h                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#define INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as having STL-like iterators.
//
//@DEPRECATED: Use 'bslalg_hasstliterators' instead.
//
//@CLASSES:
//  bslalg::TypeTraitHasStlIterators: has STL-like iterators
//
//@SEE_ALSO:
//
//@AUTHOR: Herve Bronnimann (hbronnim)
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitHasStlIterators', which allows the trait
// 'bslalg::HasStlIterators' to be declared using the (deprecated)
// 'BSLALG_DECLARE_NESTED_TRAITS' macro.  See the 'bslalg_hasstliterators'
// component for details of this trait.
//
///Usage
///-----
// No Usage example is provided since this component is deprecated.

#include <bslscm_version.h>

#include <bslalg_hasstliterators.h>

#include <bslmf_nestedtraitdeclaration.h>

namespace BloombergLP {

namespace bslalg {

                        //================================
                        // struct TypeTraitHasStlIterators
                        //================================

struct TypeTraitHasStlIterators {
    // A 'TYPE' with this trait defines (at minimum) the nested types
    // 'iterator' and 'const_iterator' and the functions 'begin()' and 'end()'
    // having the standard STL semantics.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, HasStlIterators>
    {
        // This class template ties the 'bslalg::TypeTraitHasStlIterators'
        // trait tag to the 'bslmf::HasStlIterators' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : HasStlIterators<TYPE>::type { };
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

typedef bslalg::TypeTraitHasStlIterators bslalg_TypeTraitHasStlIterators;
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
