// bslalg_typetraitbitwiseequalitycomparable.h                        -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE
#define INCLUDED_BSLALG_TYPETRAITBITWISEEQUALITYCOMPARABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as bit-wise eq.-comparable.
//
//@DEPRECATED: Use `bslmf_isbitwiseequalitycomparable` instead.
//
//@CLASSES:
//  bslalg::TypeTraitBitwiseEqualityComparable: bit-wise eq.-comparable trait
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a single traits class,
// `bslalg::TypeTraitBitwiseEqualityComparable`, which allows the trait
// `bslmf::IsBitwiseEqualityComparable` to be declared using the (deprecated)
// `BSLALG_DECLARE_NESTED_TRAITS` macro.  See the
// `bslmf_isbitwiseequalitycomparable` component for details of this trait.
//
///Usage
///-----
// No Usage example is provided since this component is deprecated.

#include <bslscm_version.h>

#include <bslmf_isbitwiseequalitycomparable.h>
#include <bslmf_nestedtraitdeclaration.h>

namespace BloombergLP {

namespace bslalg {

              //==========================================
              // struct TypeTraitBitwiseEqualityComparable
              //==========================================

/// Objects of a type with this trait can be compared for equality using
/// `memcmp`.  If two objects compare equal, then `memcmp` returns 0,
/// otherwise it returns non-zero.  Classes with this traits are assumed to
/// have no padding and to be bit-wise copyable (see the
/// `TypeTraitBitwiseCopyable` trait).  Undefined behavior may result if
/// this trait is assigned to a type that does not have an `operator==` or
/// whose `operator==` implementation may return different from comparing
/// the footprints with `memcmp` .
struct TypeTraitBitwiseEqualityComparable {

    /// This class template ties the
    /// `bslalg::TypeTraitBitwiseEqualityComparable` trait tag to the
    /// `bslmf::IsBitwiseEqualityComparable` trait metafunction.
    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bslmf::IsBitwiseEqualityComparable>
    {
    };

    template <class TYPE>
    struct Metafunction : bslmf::IsBitwiseEqualityComparable<TYPE>::type { };
};

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

/// This alias is defined for backward compatibility.
typedef bslalg::TypeTraitBitwiseEqualityComparable
                                     bslalg_TypeTraitBitwiseEqualityComparable;
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
