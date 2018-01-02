// bslh_seedgenerator.h                                               -*-C++-*-
#ifndef INCLUDED_BSLH_SEEDGENERATOR
#define INCLUDED_BSLH_SEEDGENERATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a class to generate arbitrary length seeds for algorithms.
//
//@CLASSES:
//  bslh::SeedGenerator: generator for arbitrary length seeds
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component provides a class, 'bslh::SeedGenerator', which
// utilizes a user-supplied Random Number Generator (RNG) to generate arbitrary
// length seeds.  The quality of the seeds will only be as good as the quality
// of the supplied RNG.  A cryptographically secure RNG must be supplied in
// order for 'SeedGenerator' to produce seeds suitable for a cryptographically
// secure algorithm.
//
// This class satisfies the requirements for a seed generator, defined in
// 'bslh_seededhash.h'.  More information can be found in the package level
// documentation for 'bslh' (internal users can also find information here
// {TEAM BDE:USING MODULAR HASHING<GO>})
//
///Requirements on RNG
///-------------------
// The (template parameter) type 'RANDOM_NUM_GEN' shall be a class that
// provides a type alias 'result_type' and exposes an 'operator()' that returns
// a result of type 'result_type'.  The value returned by 'operator()' shall be
// random bits, the quality of which can be defined by 'RANDOM_NUM_GEN'.
// 'RANDOM_NUM_GEN' shall also be default and copy constructible.
//
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example: Seeding Hashing Algorithms Requiring Different Seed Sizes
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a number of hashing algorithms that all require different
// length seeds.  Some require 32 bits, some require 64 bits, some even require
// 1024 bits.  We want to generate all these seeds in the same way, but we do
// not want to keep manually generating seeds of different sizes for these
// algorithms.  Moreover, we want to be able to use all these algorithms
// through a general purpose functor.  To accomplish this, we give all our
// algorithm the same interface and supply a seed generator, which can create
// any size seed that the algorithms require.
//
// First, we write our first hashing algorithm, which accepts a 32-bit seed and
// returns a 32-bit unsigned int.
//..
//  class Seeded32BitHashingAlgorithm {
//      // This class is a functor that implements a hashing algorithm seeded
//      // with 32 bits.
//
//    public:
//      typedef unsigned result_type; // Type of the hash returned
//      enum { k_SEED_LENGTH = 4 };   // Seed length in bytes
//
//    private:
//      const char *d_seed; // Seed used in the generation of hashes
//
//    public:
//      explicit Seeded32BitHashingAlgorithm(const char *seed);
//          // Construct a 'Seeded32BitHashingAlgorithm' that will use the
//          // first 4 bytes of the specified 'seed' to seed the algorithm.
//
//      result_type operator()(const char *data, size_t length);
//          // Return a hash of the specified 'length' bytes of 'data'.
//  };
//..
// Then, we define another hashing algorithm, which accepts a 64-bit seed and
// returns a 32-bit unsigned int
//..
//  class Seeded64BitHashingAlgorithm {
//      // This class is a functor that implements a hashing algorithm seeded
//      // with 64 bits.
//
//    public:
//      typedef unsigned result_type; // Type of the hash returned
//      enum { k_SEED_LENGTH = 8 };     // Seed length in bytes
//
//    private:
//      const char *d_seed; // Seed used in the generation of hashes
//
//    public:
//      explicit Seeded64BitHashingAlgorithm(const char *seed);
//          // Construct a 'Seeded64BitHashingAlgorithm' that will use the
//          // first 8 bytes of the specified 'seed' to seed the algorithm.
//
//      result_type operator()(const char *data, size_t length);
//          // Return a hash of the specified 'length' bytes of 'data'.
//  };
//..
// Next, we define a final hashing algorithm, which accepts a 1024-bit seed and
// returns a 32-bit unsigned int
//..
//  class Seeded1024BitHashingAlgorithm {
//      // This class is a functor that implements a hashing algorithm seeded
//      // with 1024 bits.
//
//    public:
//      typedef unsigned result_type; // Type of the hash returned
//      enum { k_SEED_LENGTH = 128 };   // Seed length in bytes
//
//    private:
//      const char *d_seed; // Seed used in the generation of hashes
//
//    public:
//      explicit Seeded1024BitHashingAlgorithm(const char *seed);
//          // Construct a 'Seeded1024BitHashingAlgorithm' that will use the
//          // first 128 bytes of the specified 'seed' to seed the algorithm.
//
//      result_type operator()(const char *data, size_t length);
//          // Return a hash of the specified 'length' bytes of 'data'.
//  };
//..
// Then, we declare our functor, 'SeededHash', which will take a seed
// generator, and be able to run any of our hashing algorithms by generating
// the correct size seed with the seed generator.
//..
//  template <class HASH_ALGORITHM>
//  class SeededHash {
//      // This class template implements an interface similar to 'std::hash',
//      // which will used the (template parameter) type 'SEED_GENERATOR' and
//      // 'HASH_ALGORITHM' to compute hashes.
//
//    public:
//      typedef typename HASH_ALGORITHM::result_type result_type;
//          // Type of the hash that will be returned.
//
//    private:
//      char seed[HASH_ALGORITHM::k_SEED_LENGTH];
//          // Stores the seed that will be used to run the (template
//          // parameter) type 'HASH_ALGORITHM'
//
//    public:
//      template<class SEED_GENERATOR>
//      SeededHash(SEED_GENERATOR seedGenerator);
//          //Create a 'SeededHash' and generate a seed using the specified
//          //'seedGenerator'.
//
//      result_type operator()(const char *data, size_t length) const;
//          // Returns a hash generated by the (template parameter) type
//          // 'HASH_ALGORITHM' for the specified 'length' bytes of 'data'.
//
//  };
//..
// Next, we define our constructor where we actually use 'bslh::SeedGenerator'.
// 'bslh::SeedGenerator' allows us to create arbitrary length seeds to match
// the requirements of the above declared algorithms.
//..
//  template <class HASH_ALGORITHM>
//  template<class SEED_GENERATOR>
//  SeededHash<HASH_ALGORITHM>::SeededHash(SEED_GENERATOR seedGenerator) {
//      seedGenerator.generateSeed(seed, HASH_ALGORITHM::k_SEED_LENGTH);
//  }
//
//  template <class HASH_ALGORITHM>
//  typename SeededHash<HASH_ALGORITHM>::result_type
//  SeededHash<HASH_ALGORITHM>::operator()(const char *data,
//                                         size_t length) const {
//      HASH_ALGORITHM hashAlg(seed);
//      return hashAlg(data, length);
//  }
//..
// Now, we generate some data that we want to hash.
//..
//      const char *data[] = { "asdf",
//                             "qwer",
//                             "gskgf",
//                             "ujkagad",
//                             "rwwfwe", };
//      enum { NUM_STRINGS = sizeof data / sizeof *data };
//..
// Finally, we can hash the data the same way using all of the different
// hashing algorithms.  The seed generator allows us to abstract away the
// different requirements each algorithm has on seed size.  Each algorithm will
// produce different output because it has been supplied with a different seed.
//..
//      MockRNG                                   rng;
//      SeedGenerator<MockRNG>                    seedGen(rng);
//      SeededHash<Seeded32BitHashingAlgorithm>   hashAlg32BitSeed(seedGen);
//      SeededHash<Seeded64BitHashingAlgorithm>   hashAlg64BitSeed(seedGen);
//      SeededHash<Seeded1024BitHashingAlgorithm> hashAlg1024BitSeed(seedGen);
//
//      for (int i = 0; i < NUM_STRINGS; ++i) {
//          unsigned int hash32BitSeed   = hashAlg32BitSeed(data[i],
//                                                            strlen(data[i]));
//          unsigned int hash64BitSeed   = hashAlg64BitSeed(data[i],
//                                                            strlen(data[i]));
//          unsigned int hash1024BitSeed = hashAlg1024BitSeed(data[i],
//                                                            strlen(data[i]));
//
//          if (veryVerbose) printf("Asserting hashes of %s come out"
//                                 " different\n", data[i]);
//          ASSERT(hash32BitSeed   != hash64BitSeed);
//          ASSERT(hash32BitSeed   != hash1024BitSeed);
//          ASSERT(hash1024BitSeed != hash64BitSeed);
//      }
//..

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_STRING_H
#include <string.h>  // for 'memcpy'
#define INCLUDED_STRING_H
#endif

#ifndef INCLUDED_STDDEF_H
#include <stddef.h>  // for 'size_t'
#define INCLUDED_STDDEF_H
#endif

namespace BloombergLP {

namespace bslh {

                        // =========================
                        // class bslh::SeedGenerator
                        // =========================
template<class RANDOM_NUM_GEN>
class SeedGenerator : private RANDOM_NUM_GEN {
    // This class template implements a seed generator which takes a user
    // supplied random number generator and uses it to generate an arbitrary
    // length seed.  Note that this type inherits from the (template parameter)
    // type 'RANDOM_NUM_GEN' to take advantage of the empty-base optimization.

  private:
    // PRIVATE TYPES
    typedef typename RANDOM_NUM_GEN::result_type result_type;
        // 'result_type' is an alias for the value returned by a call to
        // 'operator()' on the (template parameter) type 'RNG'.

    // DATA
    enum { k_RNGOUTPUTSIZE = sizeof(typename RANDOM_NUM_GEN::result_type)};
        // Size in bytes of the rng's output.

  public:
    // CREATORS
    SeedGenerator();
        // Create a 'bslh::SeedGenerator' that will default construct the
        // parameterized 'RNG' and use it to generate its seeds.

    explicit SeedGenerator(const RANDOM_NUM_GEN &randomNumberGenerator);
        // Create a 'bslh::SeedGenerator' that will use the specified
        // 'randomNumberGenerator' to generate its seeds.

    //! SeedGenerator(const SeedGenerator& original) = default;
        // Create a 'bslh::SeedGenerator' object with a copy of the random
        // number generator used by the specified 'original'.

    //! ~SeedGenerator() = default;
        // Destroy this object.

    // MANIPULATORS
    //! SeedGenerator& operator=(const SeedGenerator& rhs) = default;
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void generateSeed(char *seedLocation, size_t seedLength);
        // Generate a seed of the specified 'seedLength' bytes and store it at
        // the specified 'seedLocation'.  The seed will be generated with bytes
        // from the random number generator supplied at construction.  All of
        // the returned bytes will come from the RNG, meaning if the requested
        // seed is larger than the return type of the RNG, the RNG will be
        // called multiple times.  The behaviour is undefined unless the memory
        // at 'seedLocation' can store 'seedLength' bytes.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

// CREATORS
template<class RANDOM_NUM_GEN>
inline
SeedGenerator<RANDOM_NUM_GEN>::SeedGenerator()
: RANDOM_NUM_GEN()
{
}

template<class RANDOM_NUM_GEN>
inline
SeedGenerator<RANDOM_NUM_GEN>::SeedGenerator(
                                   const RANDOM_NUM_GEN &randomNumberGenerator)
: RANDOM_NUM_GEN(randomNumberGenerator)
{
}

// MANIPULATORS
template<class RANDOM_NUM_GEN>
inline
void SeedGenerator<RANDOM_NUM_GEN>::generateSeed(char *seedLocation,
                                                 size_t seedLength)
{
    BSLS_ASSERT(seedLocation || !seedLength);

    size_t numChunks = seedLength / k_RNGOUTPUTSIZE;
    size_t remainder = seedLength % k_RNGOUTPUTSIZE;

    for (size_t i = 0; i != numChunks; ++i) {
        result_type rand = RANDOM_NUM_GEN::operator()();
        memcpy(seedLocation + i * sizeof(rand), &rand, sizeof(rand));
    }

    if (remainder) {
        result_type rand = RANDOM_NUM_GEN::operator()();
        memcpy(seedLocation + numChunks * sizeof(rand), &rand, remainder);
    }
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
