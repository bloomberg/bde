// bslalg_typetraitbitwisecopyable.h                                  -*-C++-*-
#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#define INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a trait to mark classes as bit-wise copyable.
//
//@CLASSES:
//  bslalg::TypeTraitBitwiseCopyable: bit-wise copyable trait
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a single traits class,
// 'bslalg::TypeTraitBitwiseCopyable'.  An object of a 'TYPE' that has the
// bit-wise copyable trait can be copied either by invoking the copy
// constructor or by copying the footprint (i.e., the 'sizeof(TYPE)' bytes at
// the object address) using 'memcpy'.
//
// Note also that this type is a synonym for having a trivial destructor, as in
// almost every case a bitwise copyable class does not need to perform any
// operation on destruction.  It is possible to write types that are
// operationally bitwise copyable (i.e., copy constructor can be implemented in
// terms of 'memcpy') but have a non-trivial destructor, but most examples of
// such types are contrived.  In any case, this trait should not be attached to
// such types.
//
// This component is used by various 'bslalg' components for providing
// optimized primitives for types that have the bit-wise copyable trait.  The
// major benefit of this trait is not for a single object but for an array of
// such types, as a loop can be replaced by a single call to 'memcpy'.
//
///What Constitutes Bit-Wise Copyability?
///--------------------------------------
// TBD: A short guide on when to attach this trait to a class should follow.
//
///Usage
///-----
// TBD

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

namespace BloombergLP {

namespace bslalg {

                        // ==============================
                        // class TypeTraitBitwiseCopyable
                        // ==============================

struct TypeTraitBitwiseCopyable {
    // Objects of a type with this trait can be copied using 'memcpy'.  After
    // such a bitwise copy, both the original and the copy are valid.  Classes
    // with this trait are assumed to have trivial (no-op) destructors and are
    // assumed to be bitwise moveable (see the 'TypeTraitBitwiseMoveable'
    // trait).  Undefined behavior may result if this trait is assigned to a
    // type that allocates memory or other resources, uses virtual inheritance,
    // or places pointers to itself within other data structures.  Also,
    // objects of a type with this trait can be destroyed by a no-op, i.e., not
    // invoking the destructor, although it is safe to write zeros into the
    // memory footprint of the object.

    template <class TYPE>
    struct NestedTraitDeclaration :
        bslmf::NestedTraitDeclaration<TYPE, bsl::is_trivially_copyable>
    {
        // This class template ties the 'bslalg::TypeTraitBitwiseCopyable'
        // trait tag to the 'bsl:is_trivially_copyable' trait metafunction.
    };

    template <class TYPE>
    struct Metafunction : bsl::is_trivially_copyable<TYPE>::type { };
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
