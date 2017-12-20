// bdlb_cstringequalto.h                                              -*-C++-*-
#ifndef INCLUDED_BDLB_CSTRINGEQUALTO
#define INCLUDED_BDLB_CSTRINGEQUALTO

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a standard compatible equality predicate for C-strings.
//
//@CLASSES:
//  bdlb::CStringEqualTo: a standard compatible C-string equality predicate
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a 'struct', 'bdlb::CStringEqualTo',
// that defines a functor that checks two null-terminated strings for equality
// using a case-sensitive string comparison, rather than simply comparing the
// two addresses (as the 'std::equal_to' functor would do).  This comparison
// functor is suitable for supporting C-strings as keys in unordered
// associative containers.  Note that the container behavior would be undefined
// if the strings referenced by such pointers were to change value.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlb::CStringEqualTo'
/// - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how to create and use a
// 'bdlb::CStringEqualTo' object as a binary predicate for the standard library
// function 'bsl::equal' to test that two ranges of null-terminated character
// strings are equal.
//
// First, we create few sequences with null-terminated character strings,
// making sure that their elements have different memory addresses:
//..
//  const char hello1[] = { 'h', 'e', 'l', 'l', 'o', 0};
//  const char hello2[] = { 'h', 'e', 'l', 'l', 'o', 0};
//
//  const char* arrayA[3] = { "A", "B", hello1 };
//  const char* arrayB[3] = { "A", "B", hello2 };
//..
// Now, use bdlb::CStringEqualTo() as a binary predicate to compare sequences:
//..
//  bool bdlbEqualTo = bsl::equal(arrayA, arrayA+3, arrayB,
//                                bdlb::CStringEqualTo());
//  bool bslEqualTo  = bsl::equal(arrayA, arrayA+3, arrayB,
//                                bsl::equal_to<const char *>());
//..
// Finally, we observe that 'bdlb::CStringEqualTo' compares character string by
// their values, while the default comparator compares addresses:
//..
//  assert( true  == bdlbEqualTo );
//  assert( false == bslEqualTo );
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

                       // =====================
                       // struct CStringEqualTo
                       // =====================

struct CStringEqualTo {
    // This 'struct' defines a comparison functor on null-terminated character
    // strings, enabling them for use as keys in the standard unordered
    // associative containers such as 'bsl::unordered_map' and
    // 'bsl::unordered_set'.  Note that this class is an empty POD type.

    // STANDARD TYPEDEFS
    typedef const char *first_argument_type;
    typedef const char *second_argument_type;
    typedef bool        result_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CStringEqualTo, bsl::is_trivially_copyable)
    BSLMF_NESTED_TRAIT_DECLARATION(CStringEqualTo,
                                       bsl::is_trivially_default_constructible)

    // CREATORS
    //! CStringEqualTo() = default;
        // Create a 'CStringEqualTo' object.

    //! CStringEqualTo(const CStringEqualTo& original) = default;
        // Create a 'CStringEqualTo' object.  Note that as 'CStringEqualTo' is
        // an empty (stateless) type, this operation has no observable effect.

    //! ~CStringEqualTo() = default;
        // Destroy this object.

    // MANIPULATORS
    //! CStringEqualTo& operator=(const CStringEqualTo&) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'CStringEqualTo' is an empty (stateless) type, this
        // operation has no observable effect.

    // ACCESSORS
    bool operator()(const char *lhs, const char *rhs) const;
        // Return 'true' if the specified 'lhs' string has the same (case
        // sensitive) value as the specified 'rhs' string, and 'false'
        // otherwise.  The behavior is undefined unless both 'lhs' and 'rhs'
        // point to null-terminated strings.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                       // ---------------------
                       // struct CStringEqualTo
                       // ---------------------

// ACCESSORS
inline
bool CStringEqualTo::operator()(const char *lhs, const char *rhs) const
{
    BSLS_ASSERT_SAFE(lhs);
    BSLS_ASSERT_SAFE(rhs);

    return 0 == bsl::strcmp(lhs, rhs);
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
