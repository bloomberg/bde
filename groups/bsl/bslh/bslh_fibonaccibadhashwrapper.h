// bslh_fibonaccibadhashwrapper.h                                     -*-C++-*-
#ifndef INCLUDED_BSLH_FIBONACCIBADHASHWRAPPER
#define INCLUDED_BSLH_FIBONACCIBADHASHWRAPPER

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a wrapper to improve "bad" hash algorithms.
//
//@CLASSES:
//  bslh::FibonacciBadHashWrapper: wrapper to improve "bad" hashes
//
//@DESCRIPTION: This component provides a functor type,
// 'bslh::FibonacciBadHashWrapper', that can be used to wrap a hash algorithm
// ameliorating the unwanted properties of a poor hash function.
//
// For example, the identity function is often used to hash integer values (see
// 'bsl::hash<int>'), and although the identity function has the property that
// two different input values are unlikely to result in the same hash value, it
// does not have many of the other desirable properties of a hash function
// (common input values are not evenly distributed over the range of hash
// values, a small change the input does not change the hash value extensively,
// and it is easy to determine the input from a hash value).
//
// The 'bslh::FibonacciBadHashWrapper' can be used on the output of a bad hash
// function (like the identity function) to address some (but not all) of those
// deficiencies.  Specifically, the resulting hash value is more evenly
// distributed over the range, and a small change in the input will result in a
// larger change in the hash value.
//
///Requirements Upon Wrapped Hash Function Object
///----------------------------------------------
// The wrapped hash function object must be a function object, default
// constructible, copy constructible, and destructible.  The result of
// 'operator()' must be convertable to 'size_t'.  For an instance of a hash
// functor, 'operator()' must provide the same result for each argument value
// for the lifetime of the functor.  For the 'bslh::FibonacciBadHashWrapper' to
// support the type 'KEY' in its 'operator()', the wrapped hash function's
// 'operator()' must be invokable with the type 'KEY'.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Obtaining Most-Significant and Least-Significant Bit Variation
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we are using a hash table that requires variation in the
// most-significant and least-significant bits to operate efficiently (e.g.,
// Abseil's flat hash map).  The keys we will be using are small integer
// values, and we would like to use the identity function as the hash functor
// since it is efficient.  A simple and efficient method to obtain a hash
// functor with the necessary qualities is to wrap the identity functor with
// 'bslh::FibonaccaBadHashWrapper'.
//
// First, we define our 'IdentityHash' class.
//..
//  class IdentityHash {
//      // This class provides a hash algorithm that provides the "identity"
//      // mapping from key to hash.
//
//    public:
//      size_t operator()(const int key) const
//          // Return the specified 'key'.
//      {
//          return static_cast<size_t>(key);
//      }
//  };
//..
// Then, we instantiate an instance of the identity functor and the wrapped
// functor.
//..
//  IdentityHash                                identity;
//  bslh::FibonacciBadHashWrapper<IdentityHash> wrapped;
//..
// Finally, we examine the range of values obtained from small integer values:
//..
//  if (8 == sizeof(size_t)) {
//      ASSERT(18446744073709551614ull == identity(-2));
//      ASSERT(18446744073709551615ull == identity(-1));
//      ASSERT(                      0 == identity( 0));
//      ASSERT(                      1 == identity( 1));
//      ASSERT(                      2 == identity( 2));
//
//      ASSERT(14092058508772706262ull == wrapped(-2));
//      ASSERT( 7046029254386353131ull == wrapped(-1));
//      ASSERT(                      0 == wrapped( 0));
//      ASSERT(11400714819323198485ull == wrapped( 1));
//      ASSERT( 4354685564936845354ull == wrapped( 2));
//  }
//  else {
//      ASSERT(4294967294u == identity(-2));
//      ASSERT(4294967295u == identity(-1));
//      ASSERT(          0 == identity( 0));
//      ASSERT(          1 == identity( 1));
//      ASSERT(          2 == identity( 2));
//
//      ASSERT(  23791574u == wrapped(-2));
//      ASSERT(2159379435u == wrapped(-1));
//      ASSERT(          0 == wrapped( 0));
//      ASSERT(2135587861u == wrapped( 1));
//      ASSERT(4271175722u == wrapped( 2));
//  }
//..

#include <bslscm_version.h>

#include <bsls_types.h>  // 'bsls::Types::Uint64'

#include <stddef.h>      // 'size_t'

namespace BloombergLP {
namespace bslh {

                      // =============================
                      // class FibonacciBadHashWrapper
                      // =============================

template <class HASH>
class FibonacciBadHashWrapper {
    // This class provides a hash algorithm wrapper that improves the
    // distribution of varying bits from the 'HASH' applied to the 'KEY'.

    // DATA
    HASH d_hash;  // hash functor

  public:
    // PUBLIC CLASS DATA
    static const bsls::Types::Uint64 k_FIBONACCI_HASH_MULTIPLIER
                                                     = 11400714819323198485ull;
        // The value corresponds to:
        //..
        //  2^64 / phi - 1
        //..
        // where *phi* is the Golden Ratio and 1 is subtracted to avoid the
        // value being a multiple of 2 (that would throw away one bit of
        // information from the resultant hash value).  See
        // https://en.wikipedia.org/wiki/Hash_function#Fibonacci_hashing.

    // CREATORS
    FibonacciBadHashWrapper();
        // Create a 'FibonacciBadHashWrapper' having a default constructed
        // hash.

    explicit FibonacciBadHashWrapper(const HASH& hash);
        // Create a 'FibonacciBadHashWrapper' having the specified 'hash'.

    //! FibonacciBadHashWrapper(
    //!                     const FibonacciBadHashWrapper& original) = default;
        // Create a 'FibonacciBadHashWrapper' object having the value of the
        // specified 'original' object.

    //! ~FibonacciBadHashWrapper() = default;
        // Destroy this object.

    // MANIPULATORS
    //! FibonacciBadHashWrapper& operator=(
    //!                          const FibonacciBadHashWrapper& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    // ACCESSORS
    template <class KEY>
    size_t operator()(const KEY& key) const;
        // Return the hash of the specified 'key', computed as the product
        // of the result of the hash function supplied at construction and
        // 'k_FIBONACCI_HASH_MULTIPLIER'.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                      // ------------------------------
                      // struct FibonacciBadHashWrapper
                      // ------------------------------

// CREATORS
template <class HASH>
inline
FibonacciBadHashWrapper<HASH>::FibonacciBadHashWrapper()
: d_hash()
{
}

template <class HASH>
inline
FibonacciBadHashWrapper<HASH>::FibonacciBadHashWrapper(const HASH& hash)
: d_hash(hash)
{
}

// ACCESSORS
template <class HASH>
template <class KEY>
inline
size_t FibonacciBadHashWrapper<HASH>::operator()(const KEY& key) const
{
    return static_cast<size_t>(d_hash(key) * k_FIBONACCI_HASH_MULTIPLIER);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License.  You may obtain a copy
// of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations
// under the License.
// ----------------------------- END-OF-FILE ----------------------------------
