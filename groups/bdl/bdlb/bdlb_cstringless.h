// bdlb_cstringless.h                                                 -*-C++-*-
#ifndef INCLUDED_BDLB_CSTRINGLESS
#define INCLUDED_BDLB_CSTRINGLESS

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a functor enabling C strings as associative container keys.
//
//@CLASSES:
//  CStringLess: functor enabling C strings as associative container keys
//
//@SEE_ALSO: bsl_map, bsl_set
//
//@DESCRIPTION: This component provides a functor to compare two
// null-terminated strings using a case-sensitive string comparison, rather
// than simply comparing the two addresses (as the 'std::less' functor would
// do).  This lexicographical ordering makes 'CStringLess' suitable for
// supporting C strings as keys in associative containers.  Note that the
// container behavior would be undefined if the strings referenced by such
// pointers were to change value.
//
///Usage
///-----
// Suppose we need a container to store set of unique c-strings. The following
// code illustrates how to use 'bdlb::CStringLess' as a comparator for standard
// container 'set'. Default comparator compares pointer's values, so identical
// strings, placed in different memory sectors, will be added to 'set'
// container in spite of expected uniqueness of container's contents. Whereas
// 'bdlb::CStringLess' will discern no difference between such strings.
//
// First, we create several c-strings:
//..
//  const char newYork[]= "NY";
//  const char losAngeles[] = "LA";
//  const char newJersey[] = "NJ";
//  const char sanFrancisco[] = "SF";
//  const char anotherNewYork[] = "NY";
//..
// Next, we create two containers, one with default comparator and another with
// an 'bdlb::CStringLess' used as comparator:
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
// Finally, we observe different container sizes:
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
    BSLMF_NESTED_TRAIT_DECLARATION(CStringLess,
                                   bsl::is_trivially_default_constructible);
    BSLMF_NESTED_TRAIT_DECLARATION(CStringLess,
                                   bsl::is_trivially_copyable);

    // CREATORS
    //! CStringLess() = default;
        // Create a 'CStringLess' object.

    //! CStringLess(const CStringLess& original) = default;
        // Create a 'CStringLess' object.  Note that as 'CStringLess' is an
        // empty (stateless) type, this operation havs no observable effect.

        //! ~CStringLess() = default;
        // Destroy this object.

    // MANIPULATORS
    //! CStringLess& operator=(const CStringLess& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // a return a reference providing modifiable access to this object.
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
//                      INLINE FUNCTION DEFINITIONS
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