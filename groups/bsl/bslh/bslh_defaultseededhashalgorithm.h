// bslh_defaultseededhashalgorithm.h                                  -*-C++-*-
#ifndef INCLUDED_BSLH_DEFAULTSEEDEDHASHALGORITHM
#define INCLUDED_BSLH_DEFAULTSEEDEDHASHALGORITHM

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a reasonable seeded hashing algorithm for default use.
//
//@CLASSES:
//  bslh::DefaultSeededHashAlgorithm: a default seeded hashing algorithm
//
//@SEE_ALSO: bslh_hash, bslh_siphashalgorithm, bslh_defaulthashalgorithm
//
//@DESCRIPTION: 'bslh::DefaultSeededHashAlgorithm' provides an unspecified
// default seeded hashing algorithm.  The supplied algorithm is suitable for
// general purpose use in a hash table.  The underlying algorithm is subject to
// change in future releases.
//
// This class satisfies the requirements for seeded 'bslh' hashing algorithms,
// defined in 'bslh_seededhash.h'.  More information can be found in the
// package level documentation for 'bslh' (internal users can also find
// information here {TEAM BDE:USING MODULAR HASHING<GO>})
//
///Security
///--------
// In this context "security" refers to the ability of the algorithm to produce
// hashes that are not predictable by an attacker.  Security is a concern when
// an attacker may be able to provide malicious input into a hash table,
// thereby causing hashes to collide to buckets, which degrades performance.
// There are *no* security guarantees made by 'bslh::DefaultHashAlgorithm',
// meaning attackers may be able to engineer keys that will cause a Denial of
// Service (DoS) attack in hash tables using this algorithm.  Note that even if
// an attacker does not know the seed used to initialize this algorithm, they
// may still be able to produce keys that will cause a DoS attack in hash
// tables using this algorithm.  If security is required, an algorithm that
// documents better secure properties should be used, such as
// 'bslh::SipHashAlgorithm'.
//
///Speed
///-----
// The default hash algorithm will compute a hash on the order of O(n) where
// 'n' is the length of the input data.  Note that this algorithm will produce
// hashes fast enough to be used to hash keys in a hash table.  The chosen
// algorithm will be quicker than specialized algorithms such as SipHash, but
// not as fast as hashing using the identity function.
//
///Hash Distribution
///-----------------
// The default hash algorithm will distribute hashes in a pseudo-random
// distribution across the key space.  The hash function will exhibit avalanche
// behavior, meaning changing one bit of input will result in a 50% chance of
// each output bit changing.  Avalanche behavior is enough to guarantee good
// key distribution, even when values are consecutive.
//
///Hash Consistency
///----------------
// The default hash algorithm guarantees only that hashes will remain
// consistent within a single process, meaning different hashes may be produced
// on machines of different endianness or even between runs on the same
// machine.  Therefor it is not recommended to send hashes from
// 'bslh::DefaultSeededHashAlgorithm' over a network.  It is also not
// recommended to write hashes from 'bslh::DefaultSeededHashAlgorithm' to any
// memory accessible by multiple machines.
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
// object that will take objects stored in our array as input, and yield an
// integer value.  The functor can pass the attributes of 'TYPE' that are
// salient to hashing into the hashing algorithm, and then return the hash that
// is produced.
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
//      // This class template implements a hash table providing fast lookup of
//      // an external, non-owned, array of values of (template parameter)
//      // 'TYPE'.
//      //
//      // The (template parameter) 'TYPE' shall have a transitive, symmetric
//      // 'operator==' function.  There is no requirement that it have any
//      // kind of creator defined.
//      //
//      // The 'HASHER' template parameter type must be a functor with a method
//      // having the following signature:
//      //..
//      //  size_t operator()(TYPE)  const;
//      //                   -OR-
//      //  size_t operator()(const TYPE&) const;
//      //..
//      // and 'HASHER' shall have a publicly accessible default constructor
//      // and destructor.
//      //
//      // Note that this hash table has numerous simplifications because we
//      // know the size of the array and never have to resize the table.
//
//      // DATA
//      const TYPE       *d_values;          // Array of values table is to
//                                           // hold
//      size_t            d_numValues;       // Length of 'd_values'.
//      const TYPE      **d_bucketArray;     // Contains ptrs into d_values'
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
//..
// Then, we define a 'Future' class, which holds a c-string 'name', char
// 'callMonth', and short 'callYear'.
//..
//  class Future {
//      // This class identifies a future contract.  It tracks the name, call
//      // month and year of the contract it represents, and allows equality
//      // comparison.
//
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
//      bool operator==(const Future& other) const
//          // Compare this to the specified 'other' object and return true if
//          // they are equal
//      {
//          return (!strcmp(d_name, other.d_name))  &&
//             d_callMonth == other.d_callMonth &&
//             d_callYear  == other.d_callYear;
//      }
//  };
//
//  bool operator!=(const Future& lhs, const Future& rhs)
//      // Compare compare the specified 'lhs' and 'rhs' objects and return
//      // true if they are not equal
//  {
//      return !(lhs == rhs);
//  }
//
//..
// Next, we need a hash functor for 'Future'.  We are going to use the
// 'DefaultSeededHashAlgorithm' because it is a fast, general purpose hashing
// algorithm that will provide an easy way to combine the attributes of
// 'Future' objects that are salient to hashing into one reasonable hash that
// will distribute the items evenly throughout the hash table.  Moreover, when
// a new hashing algorithm is discovered to be a better default, we can be
// automatically be upgraded to use it as soon as
// 'bslh::DefaultSeededHashAlgorithm' is updated.
//..
//
//  struct HashFuture {
//      // This struct is a functor that will apply the
//      // 'DefaultSeededHashAlgorithm' to objects of type 'Future', using a
//      // generated seed.
//
//      size_t operator()(const Future& future) const
//          // Return the hash of the of the specified 'future'.  Note that
//          // this uses the 'DefaultSeededHashAlgorithm' to quickly combine
//          // the attributes of 'Future' objects that are salient to hashing
//          // into a hash suitable for a hash table.
//      {
//..
// Then, we use a 'bslh::SeedGenerator' combined with a RNG (implementation not
// shown), to generate the seeds for our algorithm.
//..
//          char seed[DefaultSeededHashAlgorithm::k_SEED_LENGTH];
//          SeedGenerator<SomeRNG> seedGenerator;
//          seedGenerator.generateSeed(seed,
//                                  DefaultSeededHashAlgorithm::k_SEED_LENGTH);
//
//          DefaultSeededHashAlgorithm hash(seed);
//..
// Next, after seeding our algorithm, we pass data into it and operate on it
// just as easily as for a non-seeded algorithm
//..
//
//          hash(future.getName(),  strlen(future.getName()));
//          hash(future.getMonth(), sizeof(char));
//          hash(future.getYear(),  sizeof(short));
//
//          return static_cast<size_t>(hash.computeHash());
//      }
//  };
//..
// Then, we want to actually use our hash table on 'Future' objects.  We create
// an array of 'Future's based on data that was originally from some external
// source:
//..
//      Future futures[] = { Future("Swiss Franc", 'F', 2014),
//                           Future("US Dollar", 'G', 2015),
//                           Future("Canadian Dollar", 'Z', 2014),
//                           Future("British Pound", 'M', 2015),
//                           Future("Deutsche Mark", 'X', 2016),
//                           Future("Eurodollar", 'Q', 2017)};
//      enum { NUM_FUTURES = sizeof futures / sizeof *futures };
//..
// Next, we create our HashTable 'hashTable'.  We pass the functor that we
// defined above as the second argument:
//..
//      HashTable<Future, HashFuture> hashTable(futures, NUM_FUTURES);
//..
// Now, we verify that each element in our array registers with count:
//..
//      for ( int i = 0; i < 6; ++i) {
//          ASSERT(hashTable.contains(futures[i]));
//      }
//..
// Finally, we verify that futures not in our original array are correctly
// identified as not being in the set:
//..
//      ASSERT(!hashTable.contains(Future("French Franc", 'N', 2019)));
//      ASSERT(!hashTable.contains(Future("Swiss Franc", 'X', 2014)));
//      ASSERT(!hashTable.contains(Future("US Dollar", 'F', 2014)));
//..

#include <bslscm_version.h>

#include <bslh_wyhashincrementalalgorithm.h>

#include <bslmf_assert.h>

#include <bsls_assert.h>

namespace BloombergLP {

namespace bslh {

class DefaultSeededHashAlgorithm {
    // This class wraps an unspecified default hashing algorithm, which takes a
    // seed, that is appropriate for general purpose use such as generating
    // hashes for a hash table.

  private:
    // PRIVATE TYPES
    typedef bslh::WyHashIncrementalAlgorithm InternalHashAlgorithm;
        // Typedef indicating the algorithm currently being used by
        // 'bslh::DefualtHashAlgorithm' to compute hashes.  This algorithm is
        // subject to change.

    // DATA
    InternalHashAlgorithm d_state;
        // Object storing the state of the chosen 'InternalHashAlgorithm'.

    // NOT IMPLEMENTED
    DefaultSeededHashAlgorithm(const DefaultSeededHashAlgorithm& original);
                                                                   // = delete;
        // Do not allow copy construction.

    DefaultSeededHashAlgorithm& operator=(
                           const DefaultSeededHashAlgorithm& rhs); // = delete;
        // Do not allow assignment.

  public:
    // TYPES
    typedef InternalHashAlgorithm::result_type result_type;
        // Typedef indicating the value type returned by this algorithm.

    // CONSTANTS
    enum { k_SEED_LENGTH = InternalHashAlgorithm::k_SEED_LENGTH };
        // Seed length in bytes.

    BSLMF_ASSERT(0 < k_SEED_LENGTH);

    // CREATORS
    explicit DefaultSeededHashAlgorithm(const char *seed);
        // Create a 'bslh::DefaultSeededHashAlgorithm', seeded with the
        // ('k_SEED_LENGTH' bytes) seed pointed to by the specified 'seed'.
        // Each bit of the supplied seed will contribute to the final hash
        // produced by 'computeHash()'.  The behaviour is undefined unless
        // 'seed' points to at least 'k_SEED_LENGTH' bytes of initialized
        // memory.

    //! ~DefaultSeededHashAlgorithm() = default;
        // Destroy this object.

    // MANIPULATORS
    void operator()(const void *data, size_t numBytes);
        // Incorporate the specified 'data', of at least the specified
        // 'numBytes', into the internal state of the hashing algorithm.  Every
        // bit of data incorporated into the internal state of the algorithm
        // will contribute to the final hash produced by 'computeHash()'.  The
        // same hash will be produced regardless of whether a sequence of bytes
        // is passed in all at once or through multiple calls to this member
        // function.  Input where 'numBytes' is 0 will have no effect on the
        // internal state of the algorithm.  The behaviour is undefined unless
        // 'data' points to a valid memory location with at least 'numBytes'
        // bytes of initialized memory or 'numBytes' is zero.

    result_type computeHash();
        // Return the finalized version of the hash that has been accumulated.
        // Note that this changes the internal state of the object, so calling
        // 'computeHash()' multiple times in a row will return different
        // results, and only the first result returned will match the expected
        // result of the algorithm.  Also note that a value will be returned,
        // even if data has not been passed into 'operator()'.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CREATORS
inline
DefaultSeededHashAlgorithm::DefaultSeededHashAlgorithm(const char *seed)
: d_state(seed)
{
    BSLS_ASSERT(seed);
}

// MANIPULATORS
inline
void DefaultSeededHashAlgorithm::operator()(const void *data, size_t numBytes)
{
    BSLS_ASSERT(0 != data || 0 == numBytes);
    d_state(data, numBytes);
}

inline
DefaultSeededHashAlgorithm::result_type
DefaultSeededHashAlgorithm::computeHash()
{
    return d_state.computeHash();
}

}  // close package namespace

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
