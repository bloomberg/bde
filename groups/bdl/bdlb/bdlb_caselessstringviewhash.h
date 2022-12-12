// bdlb_caselessstringviewhash.h                                      -*-C++-*-
#ifndef INCLUDED_BDLB_CASELESSSTRINGVIEWHASH
#define INCLUDED_BDLB_CASELESSSTRINGVIEWHASH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a case-insensitive hash functor for string views.
//
//@CLASSES:
//  bdlb::CaselessStringViewHash: case-insensive hash functor
//
//@SEE_ALSO: bdlb_caselessstringviewequalto, bdlb_caselessstringviewless
//
//@DESCRIPTION: This component provides a 'struct',
// 'bdlb::CaselessStringViewHash', that defines a functor to generate a
// case-insensitivee hash code for a string view.  This hash functor is
// suitable for supporting 'bsl::string's and 'bsl::string_view's as keys in
// unordered associative containers.
//
// Note that using this component to hash keys into a container and then using
// the 'bdlb_caselessstringviewequalto' component to compare them is less
// efficient than converting all the keys to the same case prior to insertion
// and then just doing straight hashes and string compares.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Use of 'bdlb::CaselessStringViewHash':
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we need an associative container to store, for any stock name, the
// number of shares of that stock we own:
//..
//  typedef bsl::unordered_map<bsl::string, int> SecuritiesOwnedCS;
//                                                      // case sensitive
//
//
//  typedef bsl::unordered_map<bsl::string,
//                             int,
//                             bdlb::CaselessStringViewHash,
//                             bdlb::CaselessStringViewEqualTo>
//                                                           SecuritiesOwnedCI;
//                                                      // case insensitive
//..
// This type of container stores quantities of shares and allows access to them
// by their names, in a case-insensitive manner.
//
// First, we create a container for securities holdings, case-sensitive, and
// fill it:
//..
//  SecuritiesOwnedCS securitiesOwnedCS;
//
//  securitiesOwnedCS["IBM"]       = 616;
//  securitiesOwnedCS["Microsoft"] = 6160000;
//
//  assert(2 == securitiesOwnedCS.size());
//..
// Then, we create a container for securities holdings, case-insensitive, and
// fill it:
//..
//  SecuritiesOwnedCI securitiesOwnedCI;
//
//  securitiesOwnedCI["IBM"]       = 616;
//  securitiesOwnedCI["Microsoft"] = 6160000;
//
//  assert(2 == securitiesOwnedCI.size());
//..
// Now, we try accessing the case-sensitive 'securitiesc':
//..
//  assert(1   == securitiesOwnedCS.count("IBM"));
//  assert(616 == securitiesOwnedCS[      "IBM"]);
//
//  assert(0   == securitiesOwnedCS.count("ibm"));
//  assert(0   == securitiesOwnedCS.count("Ibm"));
//  assert(0   == securitiesOwnedCS.count("iBm"));
//
//  assert(1       == securitiesOwnedCS.count("Microsoft"));
//  assert(6160000 == securitiesOwnedCS[      "Microsoft"]);
//
//  assert(0       == securitiesOwnedCS.count("MICROSOFT"));
//  assert(0       == securitiesOwnedCS.count("microsoft"));
//  assert(0       == securitiesOwnedCS.count("MICROSOFT"));
//
//  assert(0 == securitiesOwnedCS.count("Google"));
//..
// Finally, we access the case-insensitive 'securitiesci':
//..
//  assert(1   == securitiesOwnedCI.count("IBM"));
//  assert(616 == securitiesOwnedCI[      "IBM"]);
//  assert(1   == securitiesOwnedCI.count("ibm"));
//  assert(616 == securitiesOwnedCI[      "ibm"]);
//  assert(1   == securitiesOwnedCI.count("Ibm"));
//  assert(616 == securitiesOwnedCI[      "Ibm"]);
//  assert(1   == securitiesOwnedCI.count("iBm"));
//  assert(616 == securitiesOwnedCI[      "iBm"]);
//
//  assert(1       == securitiesOwnedCI.count("Microsoft"));
//  assert(6160000 == securitiesOwnedCI[      "Microsoft"]);
//  assert(1       == securitiesOwnedCI.count("MICROSOFT"));
//  assert(6160000 == securitiesOwnedCI[      "MICROSOFT"]);
//  assert(1       == securitiesOwnedCI.count("microsoft"));
//  assert(6160000 == securitiesOwnedCI[      "microsoft"]);
//  assert(1       == securitiesOwnedCI.count("MICROSOFT"));
//  assert(6160000 == securitiesOwnedCI[      "MICROSOFT"]);
//
//  assert(0 == securitiesOwnedCI.count("Google"));
//..

#include <bdlscm_version.h>

#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsl_cstddef.h>
#include <bsl_string_view.h>

namespace BloombergLP {
namespace bdlb {

                       // =============================
                       // struct CaselessStringViewHash
                       // =============================

struct CaselessStringViewHash {
    // This 'struct' defines a hash operation for string views enabling
    // 'bsl::string's or 'bsl::string_view's to be used as keys in the standard
    // unordered associative containers such as 'bsl::unordered_map' and
    // 'bsl::unordered_set'.  Note that this class is an empty POD type.

    // PUBLIC TYPES
    typedef bsl::string_view  argument_type;
    typedef bsl::size_t       result_type;
    typedef void              is_transparent;
        // Type alias indicating this is a transparent hash functor.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(CaselessStringViewHash,
                                                   bsl::is_trivially_copyable);
    BSLMF_NESTED_TRAIT_DECLARATION(CaselessStringViewHash,
                                      bsl::is_trivially_default_constructible);

    //! CaselessStringViewHash() = default;
        // Create a 'CaselessStringViewHash' object.

    //! CaselessStringViewHash(const CaselessStringViewHash& original) =
    //                                                                 default;
        // Create a 'CaselessStringViewHash' object.  Note that as
        // 'CaselessStringViewHash' is an empty (stateless) type, this
        // operation will have no observable effect.

    //! ~CaselessStringViewHash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! CaselessStringViewHash& operator=(const CaselessStringViewHash& rhs) =
    //                                                                 default;
        // Assign to this object the value of the specified 'rhs' object, and a
        // return a reference providing modifiable access to this object.  Note
        // that as 'CaselessStringViewHash' is an empty (stateless) type, this
        // operation will have no observable effect.

    // ACCESSORS
    bsl::size_t operator()(bsl::string_view argument) const;
        // Return a hash code generated from the contents of the specified
        // null-terminated 'argument' string, cast to lower case.  The behavior
        // is undefined unless 'argument' points to a null-terminated string.
};

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
