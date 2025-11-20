// bslh_hashthreadid.h                                                -*-C++-*-
#ifndef INCLUDED_BSLH_HASHTHREADID
#define INCLUDED_BSLH_HASHTHREADID

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide `hashAppend` for `std::thread::id`.
//
//@DESCRIPTION: This component provides a free function template,
// `bslh::hashAppend`, overloaded for the `std::thread::id` type.
// Including this function allows for `std::thread::id` (and types that contain
// members of that type) to be used as keys in BDE hashed containers.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Hashing a Thread ID
/// - - - - - - - - - - - - - - -
// Suppose we have a struct with an `int` member and a `std::thread::id`
// member:
// ```
// struct S {
//     int             d_x;
//     std::thread::id d_tid;
// };
// ```
// To implement `hashAppend` for this struct, we would feed its first member,
// then its second, to `hashAppend`.  This is made possible by the fact that
// this component provides an overload that accepts `std::thread::id`.
// ```
// template <class t_HASH_ALG>
// void hashAppend(t_HASH_ALG& algorithm, const S& s)
// {
//     using bslh::hashAppend;
//     hashAppend(algorithm, s.d_x);
//     hashAppend(algorithm, s.d_tid);
// }
// ```
// We can now use our new `hashAppend` overload:
// ```
// void usageExample()
// {
//     using bslh::hashAppend;
//
//     bslh::DefaultHashAlgorithm h1;
//     bslh::DefaultHashAlgorithm h2;
//
//     S s1;
//     s.d_x   = 1;
//     s.d_tid = std::this_thread::get_id();
//     hashAppend(h1, s1);
//
//     S s2 = s1;
//     hashAppend(h2, s2);
//
//     assert(h1.computeHash() == h2.computeHash());
// }
// ```

#include <bslscm_version.h>

#include <bslh_hash.h>

#include <bsls_libraryfeatures.h>

#include <cstddef>
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
# include <thread>
#endif

namespace BloombergLP {
namespace bslh {
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
// FREE FUNCTIONS

/// Invoke `hashAppend` with the specified `algorithm` as the first argument
/// and the result of applying an unspecified function to the specified `tid`
/// as the second argument.
template <class t_HASH_ALG>
void hashAppend(t_HASH_ALG& algorithm, const std::thread::id& tid);

/// This component-private struct is used to implement `hashAppend`.
struct HashThreadId_ImpUtil {
    static std::size_t getHashInput(const std::thread::id& tid);
};

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

// FREE FUNCTIONS

template <class t_HASH_ALG>
void hashAppend(t_HASH_ALG& algorithm, const std::thread::id& tid)
{
    hashAppend(algorithm, HashThreadId_ImpUtil::getHashInput(tid));
}
#endif  // C++11
}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
