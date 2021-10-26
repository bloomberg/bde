// bslh_hashpair.h                                                    -*-C++-*-
#ifndef INCLUDED_BSLH_HASHPAIR
#define INCLUDED_BSLH_HASHPAIR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide 'hashAppend' for 'std::pair'.
//
//@DESCRIPTION: This component provides a free function template,
// 'bslh::hashAppend', overloaded for the 'std::pair' class template.
// Including this function allows for 'std::pair' types (and types that contain
// them) to be used as keys in BDE hashed containers.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Hashing a Pair of Integer Values
/// - - - - - - - - - - - - - - - - - - - - - -
// Suppose one must compute has that combines the hashes of several integer
// values, each of a different type:
//..
//  char  c = 'a';
//  short s = static_cast<short>(1);
//..
// First, we can make that calculation by repeated invocations of a
// 'bslh::DefaultHashAlogorithm object:
//..
//  bslh::DefaultHashAlgorithm hasherS;
//  hasherS(&c, sizeof(char));
//  hasherS(&s, sizeof(short));
//
//  bslh::DefaultHashAlgorithm::result_type hashS = hasherS.computeHash();
//..
// Now, the same calculation can also be be made if those same values are
// contained in a single 'std::pair' object.
//..
//  std::pair<char, short> t = std::make_pair(c, s);
//
//  bslh::DefaultHashAlgorithm hasherT;
//  bslh::hashAppend(hasherT, t);
//
//  bslh::DefaultHashAlgorithm::result_type hashT = hasherT.computeHash();
//..
// Finally, we confirm that we computed the same result.
//..
//  assert(hashS == hashT);
//..

#include <bslscm_version.h>

#include <bslh_hash.h>

#include <utility> // 'std::pair'

namespace BloombergLP {
namespace bslh {

// FREE FUNCTIONS
template <class HASH_ALGORITHM, class TYPE1, class TYPE2>
void
hashAppend(HASH_ALGORITHM& algorithm, const std::pair<TYPE1, TYPE2>& input);
    // Invoke the (appropriate) 'hashAppend' function, with the specified
    // 'algorithm' on the 'first' and 'second' members, in that order, of the
    // specified 'input' pair.

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

// FREE FUNCTIONS
template <class HASH_ALGORITHM, class TYPE1, class TYPE2>
inline
void
hashAppend(HASH_ALGORITHM& algorithm, const std::pair<TYPE1, TYPE2>& input)
{
    hashAppend(algorithm, input.first);
    hashAppend(algorithm, input.second);
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
