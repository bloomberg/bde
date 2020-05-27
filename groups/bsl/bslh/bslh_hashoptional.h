// bslh_hashoptional.h                                                -*-C++-*-
#ifndef INCLUDED_BSLH_HASHOPTIONAL
#define INCLUDED_BSLH_HASHOPTIONAL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide 'hashAppend' for 'std::optional'.
//
//@DESCRIPTION: This component provides a free function template,
// 'bslh::hashAppend', overloaded for the 'std::optional' class template.
// Including this function allows for 'std::optional' types (and types that
// contain them) to be used as keys in BDE hashed containers.
//
// Note that use of this component requires that the language standard be 2017
// or later, as that is when 'std::optional' first appears.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1:  Hashing an optional Boolean value
///- - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to maintain a boolean condition as either true, false, or
// unspecified, and have it fit within the BDE hash framework.  We can use
// 'std::optional<bool>' for this, and demonstrate that such a value can be
// correctly hashed.
//
// First, we set up three such optional values to represent the three possible
// states we wish to represent.
//..
//  std::optional<bool> optionalTrue  = true;
//  std::optional<bool> optionalFalse = false;
//  std::optional<bool> optionalUnset;
//..
// Then, we create a hashing object.
//..
//  bslh::Hash<> hasher;
//..
// Next, we hash each of our values.
//..
//  size_t optionalTrueHash  = hasher(optionalTrue);
//  size_t optionalFalseHash = hasher(optionalFalse);
//  size_t optionalUnsetHash = hasher(optionalUnset);
//..
// Then we hash the underlying values.
//..
//  size_t expectedTrueHash  = hasher(true);
//  size_t expectedFalseHash = hasher(false);
//..
// Finally, we verify that the 'std::optional' hasher produces the same results
// as the underlying hashers.  For the disengaged hash, we will just check that
// the value differs from either engaged value.
//..
//  assert(expectedTrueHash  == optionalTrueHash);
//  assert(expectedFalseHash == optionalFalseHash);
//  assert(expectedTrueHash  != optionalUnsetHash);
//  assert(expectedFalseHash != optionalUnsetHash);
//..

#include <bslscm_version.h>

#include <bslh_hash.h>

#include <bslmf_removeconst.h>

#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>

#include <stddef.h>  // for 'size_t'
#include <string.h>  // for 'strlen'

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#include <optional>

namespace BloombergLP {
namespace bslh {

// FREE FUNCTIONS
template <class HASH_ALGORITHM, class TYPE>
inline
void
hashAppend(HASH_ALGORITHM& algorithm, const std::optional<TYPE> &input)
    // If the specified 'input' is engaged, pass its contained value into the
    // specified 'algorithm' to be combined into the internal state of the
    // algorithm that is used to produce the resulting hash value.  Otherwise,
    // pass an arbitrary 'size_t' value instead.  Note that this behavior (for
    // engaged values) is required by the C++ Standard.
{
    if (input) {
        hashAppend(algorithm, input.value());
    } else {
        size_t disengaged = 0xB01DFACE;
        hashAppend(algorithm, disengaged);
    }
}

}  // close package namespace
}  // close enterprise namespace

#endif  // BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
