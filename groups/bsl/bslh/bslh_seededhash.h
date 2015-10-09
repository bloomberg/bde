// bslh_seededhash.h                                                  -*-C++-*-
#ifndef INCLUDED_BSLH_SEEDEDHASH
#define INCLUDED_BSLH_SEEDEDHASH

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a struct to run seeded 'bslh' hash algorithms on types.
//
//@CLASSES:
//  bslh::SeededHash: functor that runs seeded 'bslh' hash algorithms on types
//
//@SEE_ALSO: bslh_hash, bslh_seedgenerator
//
//@DESCRIPTION: This component provides a templated 'struct',
// 'bslh::SeededHash', that defines a hash-functor that can be used with
// standard containers (a drop in replacement for 'bsl::hash'), and which
// applies the supplied (template parameter) 'HASH_ALGORITHM' to the attributes
// of the (template parameter) 'TYPE' which have been identified as salient to
// hashing.  The 'bslh::SeededHash' template parameter 'HASH_ALGORITHM' must be
// a hashing algorithm that conforms the the requirements outlined below (see
// {Requirements for Seeded 'bslh' Hashing Algorithms}).  Note that there are
// several hashing algorithms defined in 'bslh', some of which do not accept
// seeds, meaning they cannot be used with 'bslh::SeededHash'.
//
// 'bslh::SeededHash' will use the (template parameter) 'SEED_GENERATOR' to
// generate the seed used to instantiate the 'HASH_ALGORITHM'.  The
// 'bslh::SeededHash' template parameter 'SEED_GENERATOR' must be a seed
// generator that conforms the the requirements outlined below (see
// {Requirements on (template parameter) Type 'SEED_GENERATOR'}).  The seed
// will be generated once upon construction of 'bslh::SeededHash' and then held
// until destruction.
//
// A call to 'bslh::Hash::operator()' for a (template parameter) 'TYPE' will
// call the 'hashAppend' free function for 'TYPE' and provide 'hashAppend' an
// instance of the 'HASH_ALGORITHM' which has been constructed using the stored
// seed.  Clients are expected to define a free-function 'hashAppend' for each
// of the types they wish to be hashable (see 'bslh_hash.h' for details on
// 'hashAppend').  More information can be found in the package level
// documentation for 'bslh' (internal users can also find information here
// {TEAM BDE:USING MODULAR HASHING<GO>})
//
///Relationship to 'bslh::Hash'
///----------------------------
// 'bslh::SeededHash' is substantially similar to 'bslh::Hash'.
// 'bslh::SeededHash' presents a similar interface to that of 'bslh::Hash',
// however, it adds a constructor that accepts a seed generator.  Because of
// the use of seeds, 'bslh::SeededHash' stores data and therefor does not allow
// the empty base optimization like 'bslh::Hash' does.
//
///Requirements for Seeded 'bslh' Hashing Algorithms
///-------------------------------------------------
// Users of this modular hashing system are free write their own hashing
// algorithms.  In order to plug into 'bslh::SeededHash', the user-implemented
// algorithms must meet the requirements for regular 'bslh' hashing algorithms
// defined in 'bslh_hash.h', with the exception that a default constructor is
// not required.  The user-implemented algorithm must also implement the
// interface shown here:
//..
// class SomeHashAlgorithm
// {
//   public:
//     // CONSTANTS
//     enum { k_SEED_LENGTH = XXX };
//
//     // CREATORS
//     explicit SomeHashAlgorithm(const char *seed);
// };
//..
// The 'k_SEED_LENGTH' enum must be in the public interface, and 'XXX' must be
// replaced with an integer literal indicating the number of bytes of seed the
// algorithm requires.  The parameterized constructor must accept a
// 'const char *'.  This pointer will point to a seed of 'XXX' bytes in size.
//
///Requirements on (template parameter) Type 'SEED_GENERATOR'
///----------------------------------------------------------
// Users are free to write their own seed generator, which can be supplied to
// bslh::SeededHash.  The seed generator must conform to the interface shown
// here:
//..
// class SomeSeedGenerator
// {
//     // ACCESSORS
//     void generateSeed(char *seedLocation, size_t seedLength);
// };
//..
// The only mandatory piece of the seed generator interface is the generateSeed
// method, which accepts a char pointer to memory to be written and a size_t
// length in bytes.  The generateSeed method must fill the size_t bytes of the
// memory pointed to by the char pointer with a seed.  The seed generator must
// be meet one of the two requirements:
//
//: A The seed generator is default constructible.
//:
//: B The seed generator is copy constructible.
//
// Option A is preferred because it allows 'bslh::SeededHash' to be default
// constructible.  Option B is allowed, but means that 'bslh::SeededHash' must
// be passed an already-instantiated 'SEED_GENERATOR' at construction.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Storing User Defined Input in a Hash Table
///- - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have any array of user-specified nicknames, and we want a really
// fast way to find out if values are contained in the array.  We can create a
// 'HashTable' data structure that is capable of looking up values in O(1)
// time.
//
// Because we will be storing arbitrary user input in our table, it is possible
// that an attacker with knowledge of the hashing algorithm we are using could
// specially craft input that will cause collisions in our hash table,
// degrading performance to O(n).  To avoid this we will need to use a secure
// hash algorithm with a random seed.  This algorithm will need to be in the
// form of a hash functor -- an object that will take objects stored in our
// array as input, and yield a 64-bit int value which is hard enough for an
// outside observer to predict that it appear random.  'bslh::SeededHash'
// provides a convenient functor that can wrap any seeded hashing algorithm and
// use it to produce a hash for any type them implements 'hashAppend'.
//
// We can use the result of the hash function to index into our array of
// 'buckets'.  Each 'bucket' is simply a pointer to a value in our original
// array of 'TYPE' objects.
//
// First, we define our 'HashTable' template class, with the two type
// parameters: 'TYPE' (the type being referenced) and 'HASHER' (a functor that
// produces the hash).
//..
//
//  template <class TYPE, class HASHER>
//  class HashTable {
//      // This class template implements a hash table providing fast lookup of
//      // an external, non-owned, array of values of (template parameter)
//      // 'TYPE'.
//      //
//      // The (template parameter) 'TYPE' shall have a transitive, symmetric
//      // 'operator==' function and it will be hashable using 'bslh::Hash'.
//      // Note that there is no requirement that it have any kind of creator
//      // defined.
//      //
//      // The 'HASHER' template parameter type must be a functor with a method
//      // having the following signature:
//      //..
//      //  size_t operator()(TYPE)  const;
//      //                   -OR-
//      //  size_t operator()(const TYPE&) const;
//      //..
//      // and 'HASHER' shall have a publicly accessible default constructor
//      // and destructor.  Here we use 'bslh::Hash' as our default template
//      // argument.  This allows us to hash any type for which 'hashAppend'
//      // has been implemented.
//      //
//      // Note that this hash table has numerous simplifications because we
//      // know the size of the array and never have to resize the table.
//
//      // DATA
//      const TYPE       *d_values;          // Array of values table is to
//                                           // hold
//      size_t            d_numValues;       // Length of 'd_values'.
//      const TYPE      **d_bucketArray;     // Contains ptrs into 'd_values'
//      unsigned          d_bucketArrayMask; // Will always be '2^N - 1'.
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
//                size_t      numValues,
//                HASHER      hasher);
//          // Create a hash table referring to the specified 'valuesArray'
//          // having length of the specified 'numValues' and using the
//          // specified 'hasher' to generate hash values.  No value in
//          // 'valuesArray' shall have the same value as any of the other
//          // values in 'valuesArray'
//
//      ~HashTable();
//          // Free up memory used by this cross-reference.
//
//      // ACCESSORS
//      bool contains(const TYPE& value) const;
//          // Return true if the specified 'value' is found in the table and
//          // false otherwise.
//  };
//..
// Then, we will create an array of user supplied nicknames that would create
// collisions in some other hashing algorithm.
//..
//
//      const char names[6][11] = { "COLLISION!",
//                                  "COLLISION@",
//                                  "COLLISION#",
//                                  "COLLISION$",
//                                  "COLLISION%",
//                                  "COLLISION^"};
//
//      enum { NUM_NAMES = sizeof names / sizeof *names };
//
//..
// Next, we create a seed generator, with a cryptographically secure random
// number generator, that can be used to generate seeds for our secure hashing
// algorithm.  We then pass that seed generator into 'bslh::SeededHash'.  We
// use the 'bslh::SipHashAlgorithm' as our secure hashing algorithm.
//..
//      typedef SeedGenerator<CryptographicallySecureRNG> SecureSeedGenerator;
//      typedef SeededHash<SecureSeedGenerator, SipHashAlgorithm> SecureHash;
//
//      SecureSeedGenerator secureSeedGenerator;
//      SecureHash          secureHash(secureSeedGenerator);
//
//..
// Then, we create our hash table 'hashTable'.  We pass it the 'secureHash'
// hashing functor we created.  Passing it in through the functor, rather than
// just having it default constructed from the template parameter, allows us to
// pass in an algorithm with a pre-configured state if we so desire.
//..
//
//      HashTable<const char [11], SecureHash> hashTable(names,
//                                                       NUM_NAMES,
//                                                       secureHash);
//
// Now, we verify that each element in our array registers with count:
//      for ( int i = 0; i < NUM_NAMES; ++i) {
//          ASSERT(hashTable.contains(names[i]));
//      }
//
// Finally, we verify that futures not in our original array are correctly
// identified as not being in the set:
//
//      ASSERT(!hashTable.contains("asdfasdfas"));
//      ASSERT(!hashTable.contains("asdfqwerqw"));
//      ASSERT(!hashTable.contains("asdfqwerzx"));
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLH_DEFAULTSEEDEDHASHALGORITHM
#include <bslh_defaultseededhashalgorithm.h>
#endif

#ifndef INCLUDED_BSLH_HASH
#include <bslh_hash.h>
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>  // for 'size_t'
#define INCLUDED_STDDEF_H
#endif

namespace BloombergLP {

namespace bslh {

                        // ======================
                        // class bslh::SeededHash
                        // ======================

template <class SEED_GENERATOR, class HASH_ALGORITHM
                                            = bslh::DefaultSeededHashAlgorithm>
struct SeededHash {
    // This class wraps the (template parameter) 'HASH_ALGORITHM', which
    // requires a seed, in an interface that satisfies the 'hash' requirements
    // of the C++11 standard.  The (template parameter) type 'SEED_GENERATOR'
    // will be used to generate the seed required by 'HASH_ALGORITHM'.

  private:
    // DATA
    char seed[HASH_ALGORITHM::k_SEED_LENGTH];
        // Stores the seed generated by the (template parameter) type
        // 'SEED_GENERATOR'.

  public:
    // TYPES
    typedef size_t result_type;
        // The type of the hash value that will be returned by the
        // function-call operator.

    // CREATORS
    SeededHash();
        // Create a 'bslh::SeededHash' which will default construct the
        // (template parameter) type 'SEED_GENERATOR' to initialize the seed
        // that will be passed to the (template parameter) type
        // 'HASH_ALGORITHM' when it is used.  'SEED_GENERATOR' must be default
        // constructible to use this constructor.

    explicit SeededHash(SEED_GENERATOR& seedGenerator);
        // Create a 'bslh::SeededHash' which will use the specified
        // 'seedGenerator' to initialize the seed that will be passed to the
        // (template parameter) type 'HASH_ALGORITHM' when it is used.
        // 'SEED_GENERATOR' must be copy-constructible to use this constructor.

    //! SeededHash(const SeededHash& original) = default;
        // Create a 'bslh::SeededHash' object having the same internal state as
        // the specified 'original'.

    //! ~SeededHash() = default;
        // Destroy this object.

    // MANIPULATORS
    //! SeededHash& operator=(const SeededHash& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    // ACCESSORS
    template <class TYPE>
    result_type operator()(const TYPE& type) const;
        // Returns a hash generated by the (template parameter) type
        // 'HASH_ALGORITHM' for the specified 'type'.  The value returned by
        // the 'HASH_ALGORITHM' is cast to 'size_t' before returning.

};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CREATORS
template <class SEED_GENERATOR, class HASH_ALGORITHM>
inline
SeededHash<SEED_GENERATOR, HASH_ALGORITHM>::SeededHash()
{
    SEED_GENERATOR seedGenerator = SEED_GENERATOR();
    seedGenerator.generateSeed(seed, HASH_ALGORITHM::k_SEED_LENGTH);
}

template <class SEED_GENERATOR, class HASH_ALGORITHM>
inline
SeededHash<SEED_GENERATOR, HASH_ALGORITHM>::SeededHash(
                                                 SEED_GENERATOR& seedGenerator)
{
    seedGenerator.generateSeed(seed, HASH_ALGORITHM::k_SEED_LENGTH);
}

// ACCESSORS
template <class SEED_GENERATOR, class HASH_ALGORITHM>
template <class TYPE>
inline
typename SeededHash<SEED_GENERATOR, HASH_ALGORITHM>::result_type
SeededHash<SEED_GENERATOR, HASH_ALGORITHM>::operator()(TYPE const& key) const
{
    HASH_ALGORITHM hashAlg(seed);
    hashAppend(hashAlg, key);
    return static_cast<result_type>(hashAlg.computeHash());
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
