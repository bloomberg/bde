// bdlb_caselessstringviewless.h                                      -*-C++-*-
#ifndef INCLUDED_BDLB_CASELESSSTRINGVIEWLESS
#define INCLUDED_BDLB_CASELESSSTRINGVIEWLESS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a case-insensitive less-than predicate for string views.
//
//@CLASSES:
//  bdlb::CaselessStringViewLess: a case-insensitive less for 'string_view's.
//
//@SEE_ALSO: bsl_map, bsl_set
//
//@DESCRIPTION: This component provides a 'struct',
// 'bdlb::CaselessStringViewLess', that defines a functor that compares two
// string views using a case-insensitive string comparison.  This
// lexicographical ordering makes 'CaselessStringViewLess' suitable for
// supporting 'bsl::string's or 'bsl::string_view's as keys in case-insensitive
// ordered associative containers.
//
// Note that using this component to compare keys in a container is less
// efficient than converting them all to the same case prior to insertion and
// then just doing straight string compares.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlb::CaselessStringViewLess'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need a container to store set of unique strings.  The following
// code illustrates how to use 'bdlb::CaselessStringViewLess' as a comparator
// for the standard container 'set', to create a set of unique case-insensitive
// string values.
//
// First, we create several strings:
//..
//  const bsl::string newYork        = "NY";
//  const bsl::string losAngeles     = "LA";
//  const bsl::string newJersey      = "NJ";
//  const bsl::string sanFrancisco   = "SF";
//  const bsl::string anotherNewYork = "ny";
//..
// Next, we create two containers, one with default comparator and another
// using 'bdlb::CstringLess' as a comparator:
//..
//  bsl::set<bsl::string>                               caseSensitiveSet;
//  bsl::set<bsl::string, bdlb::CaselessStringViewLess> caseInsensitiveSet;
//..
// Then, we fill containers with the same contents:
//..
//  caseSensitiveSet.insert(newYork);
//  caseSensitiveSet.insert(losAngeles);
//  caseSensitiveSet.insert(newJersey);
//  caseSensitiveSet.insert(sanFrancisco);
//  caseSensitiveSet.insert(anotherNewYork);
//
//  caseInsensitiveSet.insert(newYork);
//  caseInsensitiveSet.insert(losAngeles);
//  caseInsensitiveSet.insert(newJersey);
//  caseInsensitiveSet.insert(sanFrancisco);
//  caseInsensitiveSet.insert(anotherNewYork);
//..
// Next, we observe that the container created with 'CaselessStringViewLess'
// ('caseInsensitiveSet') contains the correct number of unique string values
// (4), while the container using the default comparator does not:
//..
//  assert(5 == caseSensitiveSet.size());
//  assert(4 == caseInsensitiveSet.size());
//..
// Now, we observe the members of the case-sensitive set:
//..
//  assert( caseSensitiveSet.count("NY"));
//  assert(!caseSensitiveSet.count("nY"));
//  assert(!caseSensitiveSet.count("Ny"));
//  assert( caseSensitiveSet.count("ny"));
//
//  assert( caseSensitiveSet.count("SF"));
//  assert(!caseSensitiveSet.count("sF"));
//  assert(!caseSensitiveSet.count("Sf"));
//  assert(!caseSensitiveSet.count("sf"));
//..
// Finally, we observe that we can do case-insensitive access to
// 'caseInsensiveSet':
//..
//  assert( caseInsensitiveSet.count("NY"));
//  assert( caseInsensitiveSet.count("nY"));
//  assert( caseInsensitiveSet.count("Ny"));
//  assert( caseInsensitiveSet.count("ny"));
//
//  assert( caseInsensitiveSet.count("LA"));
//  assert( caseInsensitiveSet.count("lA"));
//  assert( caseInsensitiveSet.count("La"));
//  assert( caseInsensitiveSet.count("la"));
//
//  assert( caseInsensitiveSet.count("nj"));
//  assert( caseInsensitiveSet.count("nJ"));
//  assert( caseInsensitiveSet.count("Nj"));
//  assert( caseInsensitiveSet.count("NJ"));
//
//  assert( caseInsensitiveSet.count("sf"));
//  assert( caseInsensitiveSet.count("sF"));
//  assert( caseInsensitiveSet.count("Sf"));
//  assert( caseInsensitiveSet.count("SF"));
//
//  assert(!caseInsensitiveSet.count("GA"));
//  assert(!caseInsensitiveSet.count("gA"));
//  assert(!caseInsensitiveSet.count("Ga"));
//  assert(!caseInsensitiveSet.count("ga"));
//..

#include <bdlscm_version.h>

#include <bdlb_stringviewutil.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlb {

                        // =============================
                        // struct CaselessStringViewLess
                        // =============================

struct CaselessStringViewLess {
    // This 'struct' defines an ordering on string views, enabling them for use
    // as keys in the standard associative containers such as 'bsl::map' and
    // 'bsl::set'.  Note that this class is an empty POD type.

    // PUBLIC TYPES
    typedef bsl::string_view first_argument_type;
    typedef bsl::string_view second_argument_type;
    typedef bool             result_type;
    typedef void             is_transparent;
        // Type alias indicating this is a transparent hash functor.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CaselessStringViewLess,
                                   bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(CaselessStringViewLess,
                                   bsl::is_trivially_default_constructible);

    // CREATORS
    //! CaselessStringViewLess() = default;
        // Create a 'CaselessStringViewLess' object.

    //! CaselessStringViewLess(const CaselessStringViewLess& original) =
    //                                                                 default;
        // Create a 'CaselessStringViewLess' object.  Note that as
        // 'CaselessStringViewLess' is an empty (stateless) type, this
        // operation has no observable effect.

    //! ~CaselessStringViewLess() = default;
        // Destroy this object.

    // MANIPULATORS
    //! CaselessStringViewLess& operator=(const CaselessStringViewLess& rhs) =
    //                                                                 default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'CaselessStringViewLess' is an empty (stateless) type, this
        // operation has no observable effect.

    // ACCESSORS
    bool operator()(bsl::string_view lhs, bsl::string_view rhs) const;
        // Return 'true' if the specified 'lhs' is lexicographically ordered
        // before the specified 'rhs', and 'false' otherwise.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                        // -----------------------------
                        // struct CaselessStringViewLess
                        // -----------------------------

// ACCESSORS
inline
bool CaselessStringViewLess::operator()(bsl::string_view lhs,
                                        bsl::string_view rhs) const
{
    return bdlb::StringViewUtil::lowerCaseCmp(lhs, rhs) < 0;
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
