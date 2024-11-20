// bdlb_transparenthash.h                                             -*-C++-*-
#ifndef INCLUDED_BDLB_TRANSPARENTHASH
#define INCLUDED_BDLB_TRANSPARENTHASH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a transparent hash functor.
//
//@CLASSES:
//  bdlb::TransparentHash: a transparent hash functor
//
//@SEE_ALSO: bsl_map, bsl_set
//
//@DESCRIPTION: This component provides a `struct`, `bdlb::TransparentHash`,
// that defines a functor to generate a hash code for different types and can
// be used as transparent hash functor for heterogeneous lookup.
//
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of `bdlb::TransparentHash`
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need a container to store set of `bsl::string` unique objects.
// `bsl::unordered_set` is designed exactly for this purpose.  But imagine that
// we want to use `bsl::string_view` objects for search operations within our
// container.  `bsl::unordered_set` uses `bsl::hash` as default hash functor.
// The problem is that even though the hash function for `bsl::string_view`
// exists, compiler tries to convert `bsl::string_view` objects to the
// `bsl::string` since `bsl::hash` is parameterized by `bsl::string`.  And
// compilation fails, because there is no such implicit conversion.  In
// addition, implicit conversions where they are available, may lead to
// additional memory allocation for temporary objects.  The following code
// illustrates how to use `bdlb::TransparentHash` as a hash functor for the
// standard container `unordered_set`, in this case to allow a
// `bsl::unordered_set<bsl::string>` to be searched with a `bsl::string_view`.
//
// First, we define a transparent equality predicate, that is required by the
// `bsl::unordered_set` along with the transparent hash:
// ```
//                  // =============================
//                  // struct TestTransparentEqualTo
//                  // =============================
//
// /// This `struct` defines an equality of objects of different types,
// /// enabling them for use for heterogeneous comparison in the standard
// /// associative containers such as `bsl::unordered_map`.  Note that this
// /// class is an empty POD type.
// struct TestTransparentEqualTo {
//
//     // TYPES
//
//     // Type alias indicating this is a transparent comparator.
//     typedef void is_transparent;
//
//     // ACCESSORS
//
//     /// Return `true` if the specified `lhs` is equal to the specified
//     /// `rhs` and `false` otherwise.
//     template <class LHS, class RHS>
//     bool operator()(const LHS& lhs, const RHS& rhs) const
//     {
//         return lhs == rhs;
//     }
// };
// ```
// Note that this struct is defined only to avoid cycle dependencies between
// BDE components.  In real code for these purposes it is recommended to use
// `bdlb::TransparentEqualTo`.
//
// Then, we create a container that uses `bdlb::TransparentHash`.  We use the
// transparent comparator defined above to avoid implicit conversions:
// ```
// typedef bsl::unordered_set<bsl::string,
//                            bdlb::TransparentHash,
//                            TestTransparentEqualTo> TransparentHashSet;
//
// TransparentHashSet transparentSet;
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

#include <bslstl_string.h>
#include <bslstl_stringview.h>

#include <bsl_functional.h>

namespace BloombergLP {
namespace bdlb {

                    // ======================
                    // struct TransparentHash
                    // ======================

/// This `struct` defines a hash operation for different types, enabling
/// them for use for heterogeneous lookup in the standard associative
/// containers such as `bsl::unordered_map`.  Note that this class is an
/// empty POD type.
struct TransparentHash {

    // TYPES

    /// Type alias indicating this is a transparent hash functor.
    typedef void is_transparent;

    // CREATORS

    /// Create a `TransparentHash` object.
    //! TransparentHash() = default;

    /// Create a `TransparentHash` object.  Note that as `TransparentHash`
    /// is an empty (stateless) type, this operation has no observable
    /// effect.
    //! TransparentHash(const TransparentHash& original) = default;

    /// Destroy this object.
    //! ~TransparentHash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    /// Note that as `TransparentHash` is an empty (stateless) type, this
    /// operation has no observable effect.
    //! TransparentHash& operator=(const TransparentHash& rhs) = default;

    // ACCESSORS

    /// Return a hash code generated from the contents of the specified
    /// `value`.
    template <class TYPE>
    std::size_t operator()(const TYPE &value) const;
};

                    // ============================
                    // struct TransparentStringHash
                    // ============================

/// This `struct` defines a hash operation for different string types, enabling
/// them for use for heterogeneous lookup in the standard associative
/// containers such as `bsl::unordered_map`.  Note that this class is an
/// empty POD type.
struct TransparentStringHash {

    // TYPES

    /// Type alias indicating this is a transparent hash functor.
    typedef void is_transparent;

    // CREATORS

    /// Create a `TransparentStringHash` object.
    //! TransparentStringHash() = default;

    /// Create a `TransparentStringHash` object.  Note that as
    /// `TransparentStringHash` is an empty (stateless) type, this operation
    /// has no observable effect.
    //! TransparentStringHash(const TransparentStringHash& original) = default;

    /// Destroy this object.
    //! ~TransparentStringHash() = default;

    // MANIPULATORS

    /// Assign to this object the value of the specified `rhs` object, and
    /// return a reference providing modifiable access to this object.
    /// Note that as `TransparentStringHash` is an empty (stateless) type, this
    /// operation has no observable effect.
    //! TransparentStringHash& operator=(const TransparentStringHash& rhs) = default;

    // ACCESSORS

    /// Return a hash code generated from the contents of the specified
    /// `value`.
    std::size_t operator()(const char *            str) const;
    std::size_t operator()(const bsl::string&      str) const;
    std::size_t operator()(const bsl::string_view& str) const;
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                          // ---------------------
                          // struct TransparentHash
                          // ---------------------

// ACCESSORS
template <class TYPE>
inline
std::size_t TransparentHash::operator()(const TYPE& value) const
{
    return bsl::hash<TYPE>().operator()(value);
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
