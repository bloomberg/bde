// bslalg_typetraitbitwiseequalitycomparable.h                        -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE
#define INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as bit-wise eq.-comparable.
//
//@CLASSES:
//  bslalg::TypeTraitBitwiseEqualityComparable: bit-wise eq.-comparable trait
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitBitwiseEqualityComparable'.  Two objects of a 'TYPE' that
// has the bit-wise equality comparable trait can be compared either by
// invoking the equality operator or by comparing the footprint (i.e., the
// 'sizeof(TYPE)' bytes at the respective object addresses) using 'memcmp'.
//
// This component is used by various 'bslalg' components for providing
// optimized primitives for types that have the bit-wise equality comparable
// trait.  The major benefit of this trait is not for a single object but for
// an array of such types, as a loop can be replaced by a single call to
// 'memcmp'.
//
///What Constitutes Bit-Wise Equality Comparability?
///-------------------------------------------------
// TBD: A short guide on when to attach this trait to a class should follow.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISBITWISEEQUALITYCOMPARABLE
#include <bslmf_isbitwiseequalitycomparable.h>
#endif

namespace BloombergLP {

namespace bslalg {

              //==========================================
              // struct TypeTraitBitwiseEqualityComparable
              //==========================================

struct TypeTraitBitwiseEqualityComparable {
    // Objects of a type with this trait can be compared for equality using
    // 'memcmp'.  If two objects compare equal, then 'memcmp' returns 0,
    // otherwise it returns non-zero.  Classes with this traits are assumed to
    // have no padding and to be bit-wise copyable (see the
    // 'TypeTraitBitwiseCopyable' trait).  Undefined behavior may result if
    // this trait is assigned to a type that does not have an 'operator==' or
    // whose 'operator==' implementation may return different from comparing
    // the footprints with 'memcmp' .

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslmf::IsBitwiseEqualityComparable>
    {
        // This class template ties the
        // 'bslalg::TypeTraitBitwiseEqualityComparable' trait tag to the
        // 'bslmf::IsBitwiseEqualityComparable' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bslmf::IsBitwiseEqualityComparable<TYPE>::type { };
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
