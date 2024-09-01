// bdlb_transparentless.h                                             -*-C++-*-
#ifndef INCLUDED_BDLB_TRANSPARENTLESS
#define INCLUDED_BDLB_TRANSPARENTLESS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a transparent less-than predicate.
//
//@CLASSES:
//  bdlb::TransparentLess: a transparent less-than predicate.
//
//@SEE_ALSO: bsl_map, bsl_set
//
//@DESCRIPTION: This component provides a `struct`, `bdlb::TransparentLess`,
// that defines a functor that can be used as transparent less-than comparator
// for heterogeneous lookup.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of `bdlb::TransparentLess`
///- - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need a container to store set of unique `bsl::strings` objects.
// We use `bsl::set` for our container, which uses `bsl::less` as default
// comparator.  `bsl::less` is suitable if we want to search an entry using a
// `bsl::string` object as a key.  However, if we were to try and search using
// a `bsl::string_view` object the code would not compile, even though the
// comparison operator between `bsl::string` and `bsl::string_view` exists.  In
// addition, implicit conversions where they are available, may lead to
// additional memory allocation for temporary objects.  The following code
// illustrates how to use `bdlb::TransparentLess` as a comparator for the
// standard container `set`, in this case to allow a `bsl::set<bsl::string>` to
// be searched with a `bsl::string_view`.
//
// First, we create several C-strings:
// ```
// const char *newYork          = "NY";
// const char *losAngeles       = "LA";
// const char *cityWithLongName = "The abbreviation of really really long "
//                                "name of the city. The name is so long "
//                                "that even its abbreviation definitely "
//                                "exceeds Small String Optimization limit";
// const char *sanFrancisco     = "SF";
// ```
// Next, we create several allocators to precisely control memory allocation:
// ```
// bslma::TestAllocator         ca("container", veryVeryVeryVerbose);
// bslma::TestAllocator         sa("strings",   veryVeryVeryVerbose);
// ```
// And set one of them as default:
// ```
// bslma::TestAllocator         da("default",   veryVeryVeryVerbose);
// bslma::DefaultAllocatorGuard dag(&da);
// ```
// Then, we create two containers, one with default comparator and another
// using `bdlb::TransparentLess` as a comparator:
// ```
// bsl::set<bsl::string>                        defaultSet(&ca);
// bsl::set<bsl::string, bdlb::TransparentLess> userSet(&ca);
// ```
// Now, we fill the containers with the strings:
// ```
// bsl::string newYorkStr         (newYork,           &sa);
// bsl::string losAngelesStr      (losAngeles,        &sa);
// bsl::string cityWithLongNameStr(cityWithLongName,  &sa);
// bsl::string sanFranciscoStr    (sanFrancisco,      &sa);
//
// defaultSet.insert(newYorkStr         );
// defaultSet.insert(losAngelesStr      );
// defaultSet.insert(cityWithLongNameStr);
//
// userSet.insert(newYorkStr         );
// userSet.insert(losAngelesStr      );
// userSet.insert(cityWithLongNameStr);
// ```
// Finally, we observe that the container created with `TransparentLess`
// container (`userSet`) allows to use `string_view` object as a key and does
// not make any implicit conversions:
// ```
// bsl::string_view newYorkStrView         (newYork         );
// bsl::string_view cityWithLongNameStrView(cityWithLongName);
// bsl::string_view sanFranciscoStrView    (sanFrancisco    );
//
// assert(userSet.end() != userSet.find(newYorkStrView         ));
// assert(userSet.end() != userSet.find(cityWithLongNameStrView));
// assert(userSet.end() == userSet.find(sanFranciscoStrView    ));
//
// assert(0 == da.numBytesTotal());
//
// assert(userSet.end() != userSet.find(newYork         ));
// assert(userSet.end() != userSet.find(cityWithLongName));
// assert(userSet.end() == userSet.find(sanFrancisco    ));
//
// assert(0 == da.numBytesTotal());
// ```
// While the container using the default comparator implicitly converts
// C-strings to `bsl::string` objects:
// ```
// assert(defaultSet.end() != defaultSet.find(newYork         ));
// assert(defaultSet.end() != defaultSet.find(cityWithLongName));
// assert(defaultSet.end() == defaultSet.find(sanFrancisco    ));
//
// assert(0 < da.numBytesTotal());
// ```

#include <bdlscm_version.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_nestedtraitdeclaration.h>

namespace BloombergLP {
namespace bdlb {

                    // ======================
                    // struct TransparentLess
                    // ======================

/// This `struct` defines an ordering on objects of different types,
/// enabling them for use for heterogeneous comparison in the standard
/// associative containers such as `bsl::map` and `bsl::set`.  Note that
/// this class is an empty POD type.
struct TransparentLess {

    // TYPES

    /// Type alias indicating this is a transparent comparator.
    typedef void is_transparent;

    // CREATORS
    //! TransparentLess() = default;
        // Create a 'TransparentLess' object.

    //! TransparentLess(const TransparentLess& original) = default;
        // Create a 'TransparentLess' object.  Note that as 'TransparentLess'
        // is an empty (stateless) type, this operation has no observable
        // effect.

    //! ~TransparentLess() = default;
        // Destroy this object.

    // MANIPULATORS
    //! TransparentLess& operator=(const TransparentLess& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.
        // Note that as 'TransparentLess' is an empty (stateless) type, this
        // operation has no observable effect.

    // ACCESSORS

    /// Return `true` if the specified `lhs` is less than the specified
    /// `rhs` and `false` otherwise.
    template <class LHS, class RHS>
    bool operator()(const LHS& lhs, const RHS& rhs) const;
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                          // ----------------------
                          // struct TransparentLess
                          // ----------------------

// ACCESSORS
template <class LHS, class RHS>
inline
bool TransparentLess::operator()(const LHS& lhs, const RHS& rhs) const
{
    return lhs < rhs;
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
