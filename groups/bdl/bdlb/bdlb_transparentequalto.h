// bdlb_transparentequalto.h                                          -*-C++-*-
#ifndef INCLUDED_BDLB_TRANSPARENTEQUALTO
#define INCLUDED_BDLB_TRANSPARENTEQUALTO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a transparent equality predicate.
//
//@CLASSES:
//  bdlb::TransparentEqualTo: a transparent equality predicate.
//
//@SEE_ALSO: bdlb_transparentless, bdlb_transparenthash
//
//@DESCRIPTION: This component provides a `struct`, `bdlb::TransparentEqualTo`,
// that defines a functor that can be used as transparent equality comparator
// for heterogeneous lookup.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of `bdlb::TransparentEqualTo`
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need a container to store set of unique `bsl::string` objects.
// `bsl::unordered_set` is designed exactly for this purpose.  But imagine that
// we want to use `bsl::string_view` objects for search operations within our
// container.  `bsl::unordered_set` uses `bsl::equal_to` as default comparator.
// The problem is that even though the equality operator between `bsl::string`
// and `bsl::string_view` exists, the compiler will try to convert
// the `bsl::string_view` objects to `bsl::string` since `bsl::equal_to` is
// parameterized by `bsl::string`.  The compilation will fail because there is
// no such implicit conversion.  Even if the compilation were to succeed
// (because an implicit conversion were available), such an operation can lead
// to additional memory allocation for temporary objects.  The following code
// illustrates how to use `bdlb::TransparentEqualTo` as a comparator for the
// standard container `unordered_set`, in this case to allow a
// `bsl::unordered_set<bsl::string>` to be searched with a `bsl::string_view`.
//
// First, we create a container that uses `bdlb::TransparentEqualTo` as a
// comparator.  Note that to avoid implicit conversions we also have to use
// transparent hash functor:
// ```
// typedef bsl::unordered_set<bsl::string,
//                            bdlb::TransparentStringHash,
//                            bdlb::TransparentEqualTo>  TransparentSet;
//
// TransparentSet  transparentSet;
// ```
// Now, we fill the container with the strings:
// ```
// transparentSet.insert("NY");
// transparentSet.insert("LA");
// ```
// Finally, we observe that the container allows to use `bsl::string_view`
// objects as a key and does not make any implicit conversions:
// ```
// bsl::string_view newYork     ("NY");
// bsl::string_view losAngeles  ("LA");
// bsl::string_view sanFrancisco("SF");
//
// assert(transparentSet.end() != transparentSet.find(newYork     ));
// assert(transparentSet.end() != transparentSet.find(losAngeles  ));
// assert(transparentSet.end() == transparentSet.find(sanFrancisco));
// ```

#include <bdlscm_version.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_nestedtraitdeclaration.h>

namespace BloombergLP {
namespace bdlb {

                    // =========================
                    // struct TransparentEqualTo
                    // =========================

/// This `struct` defines an equality of objects of different types,
/// enabling them for use for heterogeneous comparison in the standard
/// associative containers such as `bsl::unordered_map`.  Note that this
/// class is an empty POD type.
struct TransparentEqualTo {

    // TYPES

    /// Type alias indicating this is a transparent comparator.
    typedef void is_transparent;

    // CREATORS

    /// Create a `TransparentEqualTo` object.
    //! TransparentEqualTo() = default;

    /// Create a `TransparentEqualTo` object.  Note that as
    /// `TransparentEqualTo` is an empty (stateless) type, this operation
    /// has no observable effect.
    //! TransparentEqualTo(const TransparentEqualTo& original) = default;

    /// Destroy this object.
    //! ~TransparentEqualTo() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    /// Note that as `TransparentEqualTo` is an empty (stateless) type, this
    /// operation has no observable effect.
    //! TransparentEqualTo& operator=(const TransparentEqualTo& rhs) = default;

    // ACCESSORS

    /// Return `true` if the specified `lhs` is equal to the specified `rhs`
    /// and `false` otherwise.
    template <class LHS, class RHS>
    bool operator()(const LHS& lhs, const RHS& rhs) const;
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                          // -------------------------
                          // struct TransparentEqualTo
                          // -------------------------

// ACCESSORS
template <class LHS, class RHS>
inline
bool TransparentEqualTo::operator()(const LHS& lhs, const RHS& rhs) const
{
    return lhs == rhs;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
