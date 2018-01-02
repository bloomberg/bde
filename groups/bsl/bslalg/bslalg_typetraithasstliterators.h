// bslalg_typetraithasstliterators.h                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS
#define INCLUDED_BSLALG_TYPETRAITHASSTLITERATORS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
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
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitHasStlIterators', which allows the trait
// 'bslalg::HasStlIterators' to be declared using the (deprecated)
// 'BSLALG_DECLARE_NESTED_TRAITS' macro.  See the 'bslalg_hasstliterators'
// component for details of this trait.
//
///Usage
///-----
// No Usage example is provided since this component is deprecated.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLALG_HASSTLITERATORS
#include <bslalg_hasstliterators.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

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
