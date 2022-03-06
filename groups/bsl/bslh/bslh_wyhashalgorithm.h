// bslh_wyhashalgorithm.h                                             -*-C++-*-
#ifndef INCLUDED_BSLH_WYHASHALGORITHM
#define INCLUDED_BSLH_WYHASHALGORITHM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an implementation of the WyHash algorithm final v3.
//
//@CLASSES:
//  bslh::WyHashAlgorithm: functor implementing the WyHash algorithm
//
//@SEE_ALSO: bslh_hash
//
//@DESCRIPTION: 'bslh::WyHashAlgorithm' implements the WyHash algorithm
// by Wang Yi et al (see implementation file for full list of authors).  This
// algorithm is known to be very fast yet have good avalanche behavior.
//
// The original algorithm was downloaded from
// https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h
// which had been updated on September 14, 2021, last commit 166f352, and
// modified to conform to BDE coding conventions with no change in the binary
// results produced.
//
///Security
///--------
// WyHash is *not* a "Cryptographically Secure" hash.  It is "Cryptographically
// Strong", but not "Cryptographically Secure".  In order to be
// cryptographically secure, an algorithm must, among other things, provide
// "Collision Resistance", described in
// https://en.wikipedia.org/wiki/Collision_resistance , meaning that it should
// be difficult to find two different messages 'm1' and 'm2' such that
// 'hash(m1) == hash(m2)'.  Because of the limited sized output (only 2**64
// possibilities) and the fast execution time of the algorithm, it is probable
// to find two such values searching only about 'sqrt(2**64) == 2**32' inputs,
// which wont take long.
//
// WyHash *is*, however, a cryptographically strong PRF (pseudo-random
// function).  This means, assuming a cryptographically secure random seed is
// given, the output of this algorithm will be indistinguishable from a uniform
// random distribution.  This property is enough for the algorithm to be able
// to protect a hash table from malicious Denial of Service (DoS) attacks.
//
///Denial of Service (DoS) Protection
/// - - - - - - - - - - - - - - - - -
// Given a cryptographically secure seed, this algorithm will produce hashes
// with a distribution that is indistinguishable from random.  This
// distribution means that there is no way for an attacker to predict which
// keys will cause collisions, meaning that this algorithm can help mitigate
// Denial of Service (DoS) attacks on a hash table.  DoS attacks occur when an
// attacker deliberately degrades the performance of the hash table by
// inserting data that will collide to the same bucket, causing an average
// constant time lookup to become a linear search.  This protection is only
// effective if the seed provided is a cryptographically secure random number
// that is not available to the attacker.
//
///Hash Distribution
///-----------------
// Output hashes will be well distributed and will avalanche, which means
// changing one bit of the input will change approximately 50% of the output
// bits.  This will prevent similar values from funneling to the same hash or
// bucket.
//
///Hash Consistency
///----------------
// This hash algorithm is endian-INDEPENDENT and returns identical hash
// values on Linux, Solaris, Aix, and Windows, 32 & 64 bit builds.
//
///Speed
///-----
// This algorithm is about 2X faster than Spooky on small objects, and about
// equal speed on objects hundreds of bytes long.  It is especially fast on
// 64-bit builds on x86 architectures.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example: Creating and Using a Hash Table
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we have any array of types that define 'operator==', and we want a
// fast way to find out if values are contained in the array.  We can create a
// 'HashTable' data structure that is capable of looking up values in O(1)
// time.
//
// Further suppose that we will be storing futures (the financial instruments)
// in this table.  Since futures have standardized names, we don't have to
// worry about any malicious values causing collisions.  We will want to use a
// general purpose hashing algorithm with a good hash distribution and good
// speed.  This algorithm will need to be in the form of a hash functor -- an
// object that will take objects stored in our array as input, and yield a
// 64-bit int value.  The functor can pass the attributes of the 'TYPE' that
// are salient to hashing into the hashing algorithm, and then return the hash
// that is produced.
//
// We can use the result of the hash function to index into our array of
// 'buckets'.  Each 'bucket' is simply a pointer to a value in our original
// array of 'TYPE' objects.
//
// First, we define our 'HashTable' template class, with the two type
// parameters: 'TYPE' (the type being referenced) and 'HASHER' (a functor that
// produces the hash).
//..
//  template <class TYPE, class HASHER>
//  class HashTable {
//..
// This 'class template' implements a hash table providing fast lookup of an
// external, non-owned, array of values of (template parameter) 'TYPE'.
//
// The (template parameter) 'TYPE' shall have a transitive, symmetric
// 'operator==' function.  There is no requirement that it have any kind of
// creator defined.
//
// The 'HASHER' template parameter type must be a functor with a method having
// the following signature:
//..
//  size_t operator()(TYPE)  const;
//                   -OR-
//  size_t operator()(const TYPE&) const;
//..
// and 'HASHER' shall have a publicly accessible default constructor and
// destructor.
//
// Note that this hash table has numerous simplifications because we know the
// size of the array and never have to resize the table.
//..
//      // DATA
//      const TYPE       *d_values;          // Array of values table is to
//                                           // hold
//      size_t            d_numValues;       // Length of 'd_values'.
//      const TYPE      **d_bucketArray;     // Contains ptrs into 'd_values'
//      size_t            d_bucketArrayMask; // Will always be '2^N - 1'.
//      HASHER            d_hasher;          // User supplied hashing algorithm
//
//    private:
//      // PRIVATE ACCESSORS
//      bool lookup(size_t      *idx,
//                  const TYPE&  value,
//                  size_t       hashValue) const;
//          // Look up the specified 'value', having the specified 'hashValue',
//          // and load its index in 'd_bucketArray' into the specified 'idx'.
//          // If not found, return the vacant entry in 'd_bucketArray' where
//          // it should be inserted.  Return 'true' if 'value' is found and
//          // 'false' otherwise.
//
//    public:
//      // CREATORS
//      HashTable(const TYPE *valuesArray,
//                size_t      numValues);
//          // Create a hash table referring to the specified 'valuesArray'
//          // having length of the specified 'numValues'.  No value in
//          // 'valuesArray' shall have the same value as any of the other
//          // values in 'valuesArray'
//
//      ~HashTable();
//          // Free up memory used by this hash table.
//
//      // ACCESSORS
//      bool contains(const TYPE& value) const;
//          // Return true if the specified 'value' is found in the table and
//          // false otherwise.
//  };
//
//  // PRIVATE ACCESSORS
//  template <class TYPE, class HASHER>
//  bool HashTable<TYPE, HASHER>::lookup(size_t      *idx,
//                                       const TYPE&  value,
//                                       size_t       hashValue) const
//  {
//      const TYPE *ptr;
//      for (*idx = hashValue & d_bucketArrayMask; (ptr = d_bucketArray[*idx]);
//                                     *idx = (*idx + 1) & d_bucketArrayMask) {
//          if (value == *ptr) {
//              return true;                                          // RETURN
//          }
//      }
//
//      // value was not found in table
//
//      return false;
//  }
//
//  // CREATORS
//  template <class TYPE, class HASHER>
//  HashTable<TYPE, HASHER>::HashTable(const TYPE *valuesArray,
//                                     size_t      numValues)
//  : d_values(valuesArray)
//  , d_numValues(numValues)
//  , d_hasher()
//  {
//      size_t bucketArrayLength = 4;
//      while (bucketArrayLength < numValues * 4) {
//          bucketArrayLength *= 2;
//
//      }
//      d_bucketArrayMask = bucketArrayLength - 1;
//      d_bucketArray = new const TYPE *[bucketArrayLength];
//      memset(d_bucketArray,  0, bucketArrayLength * sizeof(TYPE *));
//
//      for (unsigned i = 0; i < numValues; ++i) {
//          const TYPE& value = d_values[i];
//          size_t idx;
//          const bool found = lookup(&idx, value, d_hasher(value));
//          BSLS_ASSERT_OPT(!found);    (void) found;
//          d_bucketArray[idx] = &d_values[i];
//      }
//  }
//
//  template <class TYPE, class HASHER>
//  HashTable<TYPE, HASHER>::~HashTable()
//  {
//      delete [] d_bucketArray;
//  }
//
//  // ACCESSORS
//  template <class TYPE, class HASHER>
//  bool HashTable<TYPE, HASHER>::contains(const TYPE& value) const
//  {
//      size_t idx;
//      return lookup(&idx, value, d_hasher(value));
//  }
//..
// Then, we define a 'Future' class, which holds a c-string 'name', char
// 'callMonth', and short 'callYear'.
//..
//  class Future {
//..
// This 'class' identifies a future contract.  It tracks the name, call month
// and year of the contract it represents, and allows equality comparison.
//..
//      // DATA
//      const char *d_name;    // held, not owned
//      const char  d_callMonth;
//      const short d_callYear;
//
//    public:
//      // CREATORS
//      Future(const char *name, const char callMonth, const short callYear)
//      : d_name(name), d_callMonth(callMonth), d_callYear(callYear)
//          // Create a 'Future' object out of the specified 'name',
//          // 'callMonth', and 'callYear'.
//      {}
//
//      Future() : d_name(""), d_callMonth('\0'), d_callYear(0)
//          // Create a 'Future' with default values.
//      {}
//
//      // ACCESSORS
//      const char * getMonth() const
//          // Return the month that this future expires.
//      {
//          return &d_callMonth;
//      }
//
//      const char * getName() const
//          // Return the name of this future
//      {
//          return d_name;
//      }
//
//      const short * getYear() const
//          // Return the year that this future expires
//      {
//          return &d_callYear;
//      }
//
//      bool operator==(const Future& rhs) const
//          // Compare this to the specified 'other' object and return true if
//          // they are equal
//      {
//          return (!strcmp(d_name, rhs.d_name)) &&
//                                            d_callMonth == rhs.d_callMonth &&
//                                            d_callYear  == rhs.d_callYear;
//      }
//  };
//
//  bool operator!=(const Future& lhs, const Future& rhs)
//      // Compare compare the specified 'lhs' and 'rhs' objects and return
//      // true if they are not equal
//  {
//      return !(lhs == rhs);
//  }
//..
// Next, we need a hash functor for 'Future'.  We are going to use the
// 'SpookyHashAlgorithm' because it is a fast, general purpose hashing
// algorithm that will provide an easy way to combine the attributes of
// 'Future' objects that are salient to hashing into one reasonable hash that
// will distribute the items evenly throughout the hash table.
//..
//  struct HashFuture {
//      // This struct is a functor that will apply the 'SpookyHashAlgorithm'
//      // to objects of type 'Future'.
//
//      bsls::Types::Uint64 d_seed;
//
//      HashFuture()
//      {
//          // Generate random bits in 'd_seed' based on the time of day in
//          // nanoseconds.
//
//          bsls::Types::Int64 nano =
//                    bsls::SystemTime::nowMonotonicClock().totalNanoseconds();
//          const int iterations = static_cast<int>(nano & 7) + 1;
//          for (int ii = 0; ii < iterations; ++ii) {
//              nano *= bsls::SystemTime::nowMonotonicClock().
//                                                          totalNanoseconds();
//              nano += nano >> 32;
//          }
//
//          BSLMF_ASSERT(sizeof(d_seed) <= sizeof(nano));
//
//          memcpy(&d_seed, &nano, sizeof(d_seed));
//      }
//
//      // MANIPULATOR
//      size_t operator()(const Future& future) const
//          // Return the hash of the of the specified 'future'.  Note that
//          // this uses the 'SpookyHashAlgorithm' to quickly combine the
//          // attributes of 'Future' objects that are salient to hashing into
//          // a hash suitable for a hash table.
//      {
//          bslh::WyHashAlgorithm hash(d_seed);
//
//          hash(future.getName(),  strlen(future.getName()));
//          hash(future.getMonth(), sizeof(char));
//          hash(future.getYear(),  sizeof(short));
//
//          return static_cast<size_t>(hash.computeHash());
//      }
//  };
//..

#include <bslscm_version.h>

#include <bslmf_assert.h>
#include <bslmf_isbitwisemoveable.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <stddef.h>  // for 'size_t'

namespace BloombergLP {

namespace bslh {


                          // ===========================
                          // class bslh::WyHashAlgorithm
                          // ===========================

class WyHashAlgorithm {
    // This class wraps an implementation of the "SpookyHash" hash algorithm in
    // an interface that is usable in the modular hashing system in 'bslh'.

  private:
    // PRIVATE TYPES
    typedef bsls::Types::Uint64 Uint64;
        // Typedef for a 64-bit integer type used in the hashing algorithm.

    // DATA
    Uint64 d_data;

  private:
    // NOT IMPLEMENTED
    WyHashAlgorithm(const WyHashAlgorithm&); // = delete;
    WyHashAlgorithm& operator=(const WyHashAlgorithm&); // = delete;

  public:
    // TYPES
    typedef bsls::Types::Uint64 result_type;
        // Typedef indicating the value type returned by this algorithm.

    // CREATORS
    WyHashAlgorithm();
        // Create a 'WyHashAlgorithm' using a default initial seed.

    explicit WyHashAlgorithm(const Uint64 seed);
        // Create a 'bslh::WyHashAlgorithm', seeded with the specified 'seed'.

    //! ~WyHashAlgorithm() = default;
        // Destroy this object.

    // MANIPULATORS
    void operator()(const void *data, size_t numBytes);
        // Incorporate the specified 'data', of at least the specified
        // 'numBytes', into the internal state of the hashing algorithm.  Every
        // bit of data incorporated into the internal state of the algorithm
        // will contribute to the final hash produced by 'computeHash()'.  The
        // same hash value will be produced regardless of whether a sequence of
        // bytes is passed in all at once or through multiple calls to this
        // member function.  Input where 'numBytes' is 0 will have no effect on
        // the internal state of the algorithm.  The behaviour is undefined
        // unless 'data' points to a valid memory location with at least
        // 'numBytes' bytes of initialized memory or 'numBytes' is zero.

    result_type computeHash();
        // Return the finalized version of the hash that has been accumulated.
        // Note that this changes the internal state of the object, so calling
        // 'computeHash()' multiple times in a row will return different
        // results, and only the first result returned will match the expected
        // result of the algorithm.  Also note that a value will be returned,
        // even if data has not been passed into 'operator()'
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CREATORS
inline
WyHashAlgorithm::WyHashAlgorithm()
: d_data(0x50defacedfacade5ULL)
{
}

inline
WyHashAlgorithm::WyHashAlgorithm(const Uint64 seed)
: d_data(seed)
{}

inline
WyHashAlgorithm::result_type WyHashAlgorithm::computeHash()
{
    BSLMF_ASSERT(sizeof(result_type) == sizeof(d_data));

    return d_data;
}

}  // close package namespace

// ============================================================================
//                                TYPE TRAITS
// ============================================================================

namespace bslmf {
template <>
struct IsBitwiseMoveable<bslh::WyHashAlgorithm>
    : bsl::true_type {};
}  // close namespace bslmf

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
