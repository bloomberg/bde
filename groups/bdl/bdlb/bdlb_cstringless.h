// bdlb_cstringless.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLB_CSTRINGLESS
#define INCLUDED_BDLB_CSTRINGLESS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard compatible less-than predicate for C-strings.
//
//@CLASSES:
//  bdlb::CStringLess: a standard compatible less-than predicate for C-strings.
//
//@SEE_ALSO: bsl_map, bsl_set
//
//@DESCRIPTION: This component provides a 'struct', 'bdlb::CStringLess', that
// defines a functor that compares two null-terminated strings using a
// case-sensitive string comparison, rather than simply comparing the two
// addresses (as the 'std::less' functor would do).  This lexicographical
// ordering makes 'CStringLess' suitable for supporting C-strings as keys in
// associative containers.  Note that the container behavior would be undefined
// if the strings referenced by such pointers were to change value.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlb::CStringLess'
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose we need a container to store set of unique C-strings. The following
// code illustrates how to use 'bdlb::CStringLess' as a comparator for the
// standard container 'set', to create a set of unique C-string values.
//
// Note that the default comparator for 'const char *' (i.e.,
// 'bsl::less<const char *>') compares the supplied addresses, rather than the
// contents of the C-strings to which those address typically refer.  As a
// result, when using the default comparator, identical C-string values located
// at different addresses, will be successfully added to a 'set' container.
// 'bdlb::CStringLess' compares the values of the C-strings ensuring that a
// 'set', using 'CstringLess' as a comparator, is a set of unique string
// values.
//
// First, we create several C-strings:
//..
//  const char newYork[]        = "NY";
//  const char losAngeles[]     = "LA";
//  const char newJersey[]      = "NJ";
//  const char sanFrancisco[]   = "SF";
//  const char anotherNewYork[] = "NY";
//..
// Next, we create two containers, one with default comparator and another
// using 'bdlb::CstringLess' as a comparator:
//..
//  bsl::set<const char *>                    defaultComparatorContainer;
//  bsl::set<const char *, bdlb::CStringLess> userComparatorContainer;
//..
// Now, we fill containers with the same contents:
//..
//  defaultComparatorContainer.insert(newYork);
//  defaultComparatorContainer.insert(losAngeles);
//  defaultComparatorContainer.insert(newJersey);
//  defaultComparatorContainer.insert(sanFrancisco);
//  defaultComparatorContainer.insert(anotherNewYork);
//
//  userComparatorContainer.insert(newYork);
//  userComparatorContainer.insert(losAngeles);
//  userComparatorContainer.insert(newJersey);
//  userComparatorContainer.insert(sanFrancisco);
//  userComparatorContainer.insert(anotherNewYork);
//..
// Finally, we observe that the container created with 'CStringLess'
// ('userComparatorContainer') contains the correct number of unique C-string
// values (4), while the container using the default comparator does not:
//..
//  assert(5 == defaultComparatorContainer.size());
//  assert(4 == userComparatorContainer.size());
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYDEFAULTCONSTRUCTIBLE
#include <bslmf_istriviallydefaultconstructible.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_CSTRING
#include <bsl_cstring.h>
#endif

namespace BloombergLP {
namespace bdlb {

                        // ==================
                        // struct CStringLess
                        // ==================

struct CStringLess {
    // This 'struct' defines an ordering on null-terminated character strings,
    // enabling them for use as keys in the standard associative containers
    // such as 'bsl::map' and 'bsl::set'.  Note that this class is an empty
    // POD type.

    // STANDARD TYPEDEFS
    typedef const char *first_argument_type;
    typedef const char *second_argument_type;
    typedef bool result_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CStringLess, bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(CStringLess,
                                   bsl::is_trivially_default_constructible);

    // CREATORS
    //! CStringLess() = default;
        // Create a 'CStringLess' object.

    //! CStringLess(const CStringLess& original) = default;
        // Create a 'CStringLess' object.  Note that as 'CStringLess' is an
        // empty (stateless) type, this operation has no observable effect.

    //! ~CStringLess() = default;
        // Destroy this object.

    // MANIPULATORS
    //! CStringLess& operator=(const CStringLess& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.
        // Note that as 'CStringLess' is an empty (stateless) type, this
        // operation has no observable effect.

    // ACCESSORS
    bool operator()(const char *lhs, const char *rhs) const;
        // Return 'true' if the specified 'lhs' string is lexicographically
        // ordered before the specified 'rhs' string, and 'false' otherwise.
        // The behavior is undefined unless both 'lhs' and 'rhs' point to
        // null-terminated strings.
};

// ============================================================================
//                           INLINE DEFINITIONS
// ============================================================================

                          // ------------------
                          // struct CStringLess
                          // ------------------

// ACCESSORS
inline
bool CStringLess::operator()(const char *lhs, const char *rhs) const
{
    BSLS_ASSERT_SAFE(lhs);
    BSLS_ASSERT_SAFE(rhs);

    return bsl::strcmp(lhs, rhs) < 0;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
