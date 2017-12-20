// bslalg_typetraitbitwisemoveable.h                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE
#define INCLUDED_BSLALG_TYPETRAITBITWISEMOVEABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as bit-wise moveable.
//
//@DEPRECATED: Use 'bslmf_isbitwisemoveable' instead.
//
//@CLASSES:
//  bslalg::TypeTraitBitwiseMoveable: bit-wise moveable trait
//
//@SEE_ALSO: bslmf_isbitwisemoveable
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitBitwiseMoveable', which allows the trait
// 'bslma::bslmf::IsBitwiseMoveable' to be declared using the (deprecated)
// 'BSLALG_DECLARE_NESTED_TRAITS' macro.  See the 'bslmf_isbitwisemoveable'
// component for details of this trait.
//
///Usage
///-----
// No Usage example is provided since this component is deprecated.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEMOVEABLE
#include <bslmf_isbitwisemoveable.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

namespace BloombergLP {

namespace bslalg {

                        // ==============================
                        // class TypeTraitBitwiseMoveable
                        // ==============================

struct TypeTraitBitwiseMoveable {
    // Objects of a type with this trait can be "moved" from one memory
    // location to another using 'memmove' or 'memcpy'.  Although the result of
    // such a bitwise copy is two copies of the same object, this trait only
    // guarantees that one of the copies can be destroyed.  The other copy must
    // be considered invalid and its destructor must not be called.  Most
    // types, even those that contain pointers, are bitwise moveable.
    // Undefined behavior may result if this trait is assigned to a types that
    // contains pointers to its own internals, use virtual inheritance, or
    // places pointers to itself within other data structures.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslmf::IsBitwiseMoveable>
    {
        // This class template ties the 'bslalg::TypeTraitBitwiseMoveable'
        // trait tag to the 'bslmf::IsBitwiseMoveable' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bslmf::IsBitwiseMoveable<TYPE>::type { };
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
