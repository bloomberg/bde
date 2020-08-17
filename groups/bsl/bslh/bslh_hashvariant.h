// bslh_hashvariant.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLH_HASHVARIANT
#define INCLUDED_BSLH_HASHVARIANT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide 'hashAppend' for 'std::variant'.
//
//@DESCRIPTION: This component provides a free function template,
// 'bslh::hashAppend', overloaded for the 'std::variant' class template.
// Including this function allows for 'std::variant' types (and types that
// contain them) to be used as keys in BDE hashed containers.
//
// Note that use of this component requires that the language standard be 2017
// or later, as that is when 'std::variant' first appears.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1:  Hashing an integer or floating-point value
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we want to maintain a value as either an integer or a floating-point
// number, and have it fit within the BDE hash framework.  We can use
// 'std::variant<int, double>' for this, and demonstrate that such a value can
// be correctly hashed.
//
// First, we set up a pair of such optional values to represent the two
// potential states we wish to represent.
//..
//  typedef std::variant<int, double> id;
//  id variantInt    = 42;
//  id variantDouble = 3.475;
//..
// Then, we create a hashing object.
//..
//  bslh::Hash<> hasher;
//..
// Next, we hash each of our values.
//..
//  size_t variantIntHash    = hasher(variantInt);
//  size_t variantDoubleHash = hasher(variantDouble);
//..
// Then we hash the underlying values.  A variant hashes as a pair of values,
// first the 'size_t' index of the held type, then the contained value, so we
// need to accumulate the hashes.
//..
//  bslh::Hash<>::HashAlgorithm haInt;
//  hashAppend(haInt, size_t(0));
//  hashAppend(haInt, 42);
//  size_t expectedIntHash = size_t(haInt.computeHash());
//
//  bslh::Hash<>::HashAlgorithm haDouble;
//  hashAppend(haDouble, size_t(1));
//  hashAppend(haDouble, 3.475);
//  size_t expectedDoubleHash = size_t(haDouble.computeHash());
//..
// Finally, we verify that the 'std::variant' hasher produces the same results
// as the underlying hashers.
//..
//  assert(expectedIntHash    == variantIntHash);
//  assert(expectedDoubleHash == variantDoubleHash);
//..

#include <bslscm_version.h>

#include <bslh_hash.h>

#include <bsls_compilerfeatures.h>
#include <bsls_platform.h>

#include <stddef.h>  // for 'size_t'

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_BASELINE_LIBRARY

#include <variant>

namespace BloombergLP {
namespace bslh {

// FREE FUNCTIONS
template <class HASH_ALGORITHM, class ... TYPE>
inline
void
hashAppend(HASH_ALGORITHM& algorithm, const std::variant<TYPE ...> &input)
    // Pass the index of the active variant of the specified 'input' to the
    // specified 'algorithm', followed by the active value itself, to be
    // combined into the internal state of the algorithm that is used to
    // produce the resulting hash value.
{
    hashAppend(algorithm, input.index());
    std::visit([&](const auto& x) { hashAppend(algorithm, x); }, input);
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
