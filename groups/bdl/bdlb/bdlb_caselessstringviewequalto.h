// bdlb_caselessstringviewequalto.h                                   -*-C++-*-
#ifndef INCLUDED_BDLB_CASELESSSTRINGVIEWEQUALTO
#define INCLUDED_BDLB_CASELESSSTRINGVIEWEQUALTO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a case-insensitive equality predicate for string views.
//
//@CLASSES:
//  bdlb::CaselessStringViewEqualTo: case-insensitive string view comparison.
//
//@SEE_ALSO: bdlb_caselessstringviewhash, bdlb_caselessstringviewless
//
//@DESCRIPTION: This component provides a 'struct',
// 'bdlb::CaselessStringViewEqualTo', that defines a functor that checks two
// string views for equality using a case-insensitive string comparison.  This
// comparison functor is suitable for supporting 'bsl::string's or
// 'bsl::string_view's as keys in unordered associative containers.
//
// Note that using this component to do comparisons of keys in a container is
// less efficient than converting all the keys to the same case prior to
// insertion and then just doing straight string comparisons.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlb::CaselessStringViewEqualTo'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have some strings that compare equal, some that don't, and some
// of which are equal except for the cases of some of the letters.
//..
//  const bsl::string_view a = "To be or not to be, that is the question.";
//  const bsl::string_view b = "To be or not to be, THAT IS THE QUESTION.";
//  const bsl::string_view c = "Sein oder nicht sein, das ist die frage.";
//..
// Now, we create an object of type 'bdlb::CaselessStringViewEqualTo' to do the
// comparisons:
//..
//  bdlb::CaselessStringViewEqualTo        eq;
//  const bdlb::CaselessStringViewEqualTo& equals = eq;
//..
// Finally, we observe that 'a' matches 'b', but neither matches 'c':
//..
//  assert( equals(a, b));
//  assert( equals(b, a));
//  assert(!equals(a, c));
//  assert(!equals(c, a));
//  assert(!equals(b, c));
//  assert(!equals(c, b));
//..

#include <bdlscm_version.h>

#include <bdlb_stringviewutil.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlb {

                       // ================================
                       // struct CaselessStringViewEqualTo
                       // ================================

struct CaselessStringViewEqualTo {
    // This 'struct' defines a case-insensitive equality comparison functor on
    // string views, enabling them for use as keys in the standard unordered
    // associative containers such as 'bsl::unordered_map' and
    // 'bsl::unordered_set'.  Note that this 'class' is an empty POD type.

    // PUBLIC TYPES
    typedef bsl::string_view first_argument_type;
    typedef bsl::string_view second_argument_type;
    typedef bool             result_type;
    typedef void             is_transparent;
        // Type alias indicating this is a transparent hash functor.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CaselessStringViewEqualTo,
                                                    bsl::is_trivially_copyable)
    BSLMF_NESTED_TRAIT_DECLARATION(CaselessStringViewEqualTo,
                                       bsl::is_trivially_default_constructible)

    // CREATORS
    //! CaselessStringViewEqualTo() = default;
        // Create a 'CaselessStringViewEqualTo' object.

    //! CaselessStringViewEqualTo(const CaselessStringViewEqualTo& original) =
    //                                                                 default;
        // Create a 'CaselessStringViewEqualTo' object.  Note that as
        // 'CaselessStringViewEqualTo' is an empty (stateless) type, this
        // operation has no observable effect.

    //! ~CaselessStringViewEqualTo() = default;
        // Destroy this object.

    // MANIPULATORS
    //! CaselessStringViewEqualTo& operator=(const CaselessStringViewEqualTo&)
    //                                                               = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.  Note
        // that as 'CaselessStringViewEqualTo' is an empty (stateless) type,
        // this operation has no observable effect.

    // ACCESSORS
    bool operator()(bsl::string_view lhs, bsl::string_view rhs) const;
        // Return 'true' if the specified 'lhs' string view has the same (case
        // insensitive) value as the specified 'rhs' string view, and 'false'
        // otherwise.
};

// ============================================================================
//                              INLINE DEFINITIONS
// ============================================================================

                     // --------------------------------
                     // struct CaselessStringViewEqualTo
                     // --------------------------------

// ACCESSORS
inline
bool CaselessStringViewEqualTo::operator()(bsl::string_view lhs,
                                           bsl::string_view rhs) const
{
    return bdlb::StringViewUtil::areEqualCaseless(lhs, rhs);
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
