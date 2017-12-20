// bslalg_typetraitpair.h                                             -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITPAIR
#define INCLUDED_BSLALG_TYPETRAITPAIR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as similar to 'std::pair'.
//
//@DEPRECATED: Use 'bslmf_ispair' instead.
//
//@CLASSES:
//  bslalg::TypeTraitPair: for 'std::pair'-like classes
//
//@SEE_ALSO: bslalg_typetraits
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitPair', which allows the trait 'bslmf::IsPair' to be
// declared using the (deprecated) 'BSLALG_DECLARE_NESTED_TRAITS' macro.  See
// the 'bslmf_ispair' component for details of this trait.
//
///Usage
///-----
// No Usage example is provided since this component is deprecated.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISPAIR
#include <bslmf_ispair.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BSLMF_DETECTNESTEDTRAIT
#include <bslmf_detectnestedtrait.h>
#endif

#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {

namespace bslalg {

                        // ====================
                        // struct TypeTraitPair
                        // ====================

struct TypeTraitPair {
    // A 'TYPE' with this trait has two data members, 'first' and 'second'
    // of types 'T::first_type' and 'T::second_type', respectively.

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
