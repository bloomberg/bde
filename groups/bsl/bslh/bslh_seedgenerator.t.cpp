// bslh_seedgenerator.t.cpp                                           -*-C++-*-
#include <bslh_seedgenerator.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <algorithm>
#include <math.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bslh;


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 2] SeedGenerator()
// [ 2] SeedGenerator(RNG& randomNumberGenerator)
// [ 2] SeedGenerator(const SeedGenerator)
// [ 2] ~SeedGenerator()
// [ 3] void generateSeed(char *seedLocation, size_t seedLength)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.

namespace {

int testStatus = 0;

void aSsErT(bool b, const char *s, int i)
{
    if (b) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", i, s);
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define Q   BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P   BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_  BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//         GLOBAL TYPEDEFS, HELPER FUNCTIONS, AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class MockRNG {
    // Predictable mock random number generator for use in testing
  public:
    typedef unsigned int result_type;
        // The type of the random data that 'operator()' will return.

  private:
    unsigned int counter;
        // Counter that provides some variance in the random numbers returned.

  public:
    MockRNG();
        // Create a 'MockRNG' that will return predictable "random" values.

    result_type operator()();
        // Return a predictable "random" number of 'result_type'.

    result_type numberOfCalls() const;
        // Return the number of times that 'operator()' has been called
};

MockRNG::MockRNG() : counter(0) { }

MockRNG::result_type MockRNG::operator()() {
    return ++counter;
}

MockRNG::result_type MockRNG::numberOfCalls() const {
    return counter;
}

void verifyResultMatchesRNG(const char *result, size_t length)
    // Compare the specified 'length' bytes of 'result' to the expected output
    // of 'MockRNG' using 'ASSERT's
{
    MockRNG rng;
    const size_t rngSize = sizeof(MockRNG::result_type);
    MockRNG::result_type rand;
    const char *randPtr = reinterpret_cast<const char *>(&rand);

    for(size_t i = 0; i < length; ++i) {
        if(i % rngSize == 0) {
            rand = rng();
        }
        ASSERT(result[i] == randPtr[i % rngSize]);
    }
}

unsigned int someSeededHash(const char *seed, size_t seedLength,
                                               const char *data, size_t length)
    // Hash the specified 'length' bytes of 'data' using the specified
    // 'seedLength' bytes of 'seed' to seed the hash. This is not a real hash
    // function. DO NOT USE FOR ACTUAL HASHING
{
    const unsigned int *castedSeed = reinterpret_cast<const unsigned int *>(
                                                                         seed);
    unsigned int hash = 0;
    for(size_t i = 0; i < seedLength/4; ++i) {
        hash ^= castedSeed[i];
    }

    for(size_t i = 0; i < length; ++i) {
        hash *= data[i];
    }
    return hash;
}

typedef bslh::SeedGenerator<MockRNG> Obj;

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example: Seeding hashing algotithms requiring different seed sizes
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have a number of hashing algorithms that all require different
// length seeds. Some require 32 bits, some require 64 bits, some even require
// 1024 bits. We want to generate all these seeds in the same way, but we do
// not want to keep manually generating seeds of different sizes for these
// algorithms. Moreover, we want to be able to use all these algorithms through
// a general purpose functor. To accomplish this, we give all our algorithsm
// the same interface and supply a seed generator, which can create any size
// seed that the algorithms require.
//
// First, we write our first hashing algorithm, which accepts a 32-bit seed and
// returns a 32-bit unsigned int.
//..
class Seeded32BitHashingAlgorithm {
  public:
    typedef unsigned result_type; // Type of the hash returned
    enum { SEED_LENGTH = 4 };     // Seed length in bytes

  private:
    const char *d_seed; // Seed used in the generation of hashes

  public:
    explicit Seeded32BitHashingAlgorithm(const char *seed);
        // Construct a 'Seeded32BitHashingAlgorithm' that will use the first 4
        // bytes of the specified 'seed' to seed the algorithm.

    result_type operator()(const char *data, size_t length);
        // Return a hash of the specified 'length' bytes of 'data'.
};

Seeded32BitHashingAlgorithm::Seeded32BitHashingAlgorithm(const char *seed) :
                                                               d_seed(seed) { }

Seeded32BitHashingAlgorithm::result_type
Seeded32BitHashingAlgorithm::operator()(const char *data, size_t length) {
    return someSeededHash(d_seed, 4, data, length);
}

//..
// Then, we define another hashing algorithm, which accepts a 64-bit seed and
// returns a 32-bit unsigned int
//..

class Seeded64BitHashingAlgorithm {
  public:
    typedef unsigned result_type; // Type of the hash returned
    enum { SEED_LENGTH = 8 };     // Seed length in bytes

  private:
    const char *d_seed; // Seed used in the generation of hashes

  public:
    explicit Seeded64BitHashingAlgorithm(const char *seed);
        // Construct a 'Seeded64BitHashingAlgorithm' that will use the first 8
        // bytes of the specified 'seed' to seed the algorithm.

    result_type operator()(const char *data, size_t length);
        // Return a hash of the specified 'length' bytes of 'data'.
};

Seeded64BitHashingAlgorithm::Seeded64BitHashingAlgorithm(const char *seed) :
                                                               d_seed(seed) { }

Seeded64BitHashingAlgorithm::result_type
Seeded64BitHashingAlgorithm::operator()(const char *data, size_t length) {
    return someSeededHash(d_seed, 8, data, length);
}

//..
// Next, we define a final hashing algorithm, which accepts a 1024-bit seed and
// returns a 32-bit unsigned int
//..

class Seeded1024BitHashingAlgorithm {
  public:
    typedef unsigned result_type; // Type of the hash returned
    enum { SEED_LENGTH = 128 };   // Seed length in bytes

  private:
    const char *d_seed; // Seed used in the generation of hashes

  public:
    explicit Seeded1024BitHashingAlgorithm(const char *seed);
        // Construct a 'Seeded1024BitHashingAlgorithm' that will use the first
        // 128 bytes of the specified 'seed' to seed the algorithm.

    result_type operator()(const char *data, size_t length);
        // Return a hash of the specified 'length' bytes of 'data'.
};

Seeded1024BitHashingAlgorithm::Seeded1024BitHashingAlgorithm(const char *seed)
                                                             : d_seed(seed) { }

Seeded1024BitHashingAlgorithm::result_type
Seeded1024BitHashingAlgorithm::operator()(const char *data, size_t length) {
    return someSeededHash(d_seed, 128, data, length);
}

//..
// Then, we define our functor, 'SeededHash', which will take a seed generator,
// and be able to run any of our hashing algorithms by generating the correct
// size seed with the seed generator.
//..

template <class HASH_ALGORITHM>
class SeededHash
{
    // Provides an interface similar to 'std::hash', which will used the
    // paramaterized 'SEED_GENERATOR' and 'HASH_ALGORITHM' to compute hashes.
  public:
    typedef typename HASH_ALGORITHM::result_type result_type;
        // Type of the hash that will be returned.

  private:
    char seed[HASH_ALGORITHM::SEED_LENGTH];
        // Stores the seed that will be used to run the parameterized
        // 'HASH_ALGORITHM'

  public:
    template<class SEED_GENERATOR>
    SeededHash(SEED_GENERATOR seedGenerator);
        //Create a 'SeededHash' and generate a seed using the specified
        //'seedGenerator'.

    result_type operator()(const char *data, size_t length) const;
        // Returns a hash generated by the parameterized 'HASH_ALGORITHM' for
        // the specified 'length' bytes of 'data'.

};

template <class HASH_ALGORITHM>
template<class SEED_GENERATOR>
SeededHash<HASH_ALGORITHM>::SeededHash(SEED_GENERATOR seedGenerator) {
    seedGenerator.generateSeed(seed, HASH_ALGORITHM::SEED_LENGTH);
}

template <class HASH_ALGORITHM>
typename SeededHash<HASH_ALGORITHM>::result_type
SeededHash<HASH_ALGORITHM>::operator()(const char *data, size_t length) const {
    HASH_ALGORITHM hashAlg(seed);
    return hashAlg(data, length);
}


// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
//  bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The seed generator can be used to ease the creation and usage of
        //   more complicated components such as hashing algorithms.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Run the usage example (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

//..
// Now, we generate some data that we want to hash.
//..

        const char *data[] = { "asdf",
                               "qwer",
                               "gskgf",
                               "ujkagad",
                               "rwwfwe", };
        enum { NUM_STRINGS = sizeof data / sizeof *data };

//..
// Finally, we can hash the data the same way using all of the different
// hashing algorithms. The seed generator allows us to abstract away the
// different requirements each algorithm has on seed size. Each algorithm will
// produce different output because it has been supplied with a different seed.
//..

        MockRNG rng;
        SeedGenerator<MockRNG> seedGen(rng);
        SeededHash<Seeded32BitHashingAlgorithm>   hashAlg32BitSeed(seedGen);
        SeededHash<Seeded64BitHashingAlgorithm>   hashAlg64BitSeed(seedGen);
        SeededHash<Seeded1024BitHashingAlgorithm> hashAlg1024BitSeed(seedGen);

        for(int i = 0; i < NUM_STRINGS; ++i) {
            unsigned int hash32BitSeed = hashAlg32BitSeed(data[i],
                                                              strlen(data[i]));
            unsigned int hash64BitSeed = hashAlg64BitSeed(data[i],
                                                              strlen(data[i]));
            unsigned int hash1024BitSeed = hashAlg1024BitSeed(data[i],
                                                              strlen(data[i]));

            if(veryVerbose) printf("Asserting hashes of %s come out"
                                   " different\n", data[i]);
            ASSERT(hash32BitSeed != hash64BitSeed);
            ASSERT(hash32BitSeed != hash1024BitSeed);
            ASSERT(hash1024BitSeed != hash64BitSeed);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATESEED
        //   Ensure that the 'generateSeed' method is publicly callable,
        //   returns the expected values, and has no unexpected side effects
        //   such as allocating memory.
        //
        // Concerns:
        //: 1 The method exists and is callable with a pointer and a length.
        //:
        //: 2 The supplied RNG is used to fill memory.
        //:
        //: 3 The method writes to every byte specified.
        //:
        //: 4 The method does not write more memory than specified.
        //:
        //: 5 A size of zero results in no calls to the RNG and no writes to
        //:   the supplied memory
        //:
        //: 6 The method behaves as expected when input length is not a
        //:   multiple of the supplied RNG 'result_type'.
        //:
        //: 7 No memory is allocated by the default and global allocators.
        //
        // Plan:
        //: 1 Install a test allocator as the default allocator.  Then install
        //:   an 'AllocatorGuard' to verify no memory is allocated during the
        //:   execution of this test case.  Memory from the global allocator is
        //:   tested as a global concern. (C-7)
        //:
        //: 2 Create a 'SeedGenerator' with a predictable RNG and test that the
        //:   values written to memory match the output of the RNG. (C-1,2)
        //:
        //: 3 Preload memory with known data, and test that it is all
        //:   overwritten after a call to 'generateSeed'. (C-3)
        //:
        //: 4 Preload memory with known data, and test that memory beyond the
        //:   end of the specified memory is not overwritten after a call to
        //:  'generateSeed' (C-4)
        //:
        //: 5 Call 'generateSeed' with a length of zero and verify the RNG was
        //:   not called, and the supplied memory was not written. (C-6)
        //:
        //: 6 Call 'generateSeed' with lengths that are not multiples of the
        //:   size of 'MockRNG::result_type'. (C-6)
        //:
        //: 7 Verify no memory was used. (C-7)
        //
        // Testing:
        //   void generateSeed(char *seedLocation, size_t seedLength)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING GENERATESEED"
                   "\n====================\n");

        if (verbose) printf("Install a test allocator as the default"
                            " allocator.  Then install an 'AllocatorGuard' to"
                            " verify no memory is allocated during the"
                            " execution of this test case.  Memory from the"
                            " global allocator is tested as a global concern."
                            " (C-7)\n");
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);


        if (verbose) printf("Create a 'SeedGenerator' with a predictable RNG"
                            " and test that the values written to memory match"
                            " the output of the RNG. (C-1,2)\n");
        {
            MockRNG rng;
            Obj generator(rng);

            char seed[8];
            generator.generateSeed(seed, 8);

            verifyResultMatchesRNG(seed, 8);
        }

        if (verbose) printf("Preload memory with known data, and test that it"
                            " is all overwritten after a call to"
                            " 'generateSeed'. (C-3)\n");
        {
            MockRNG rng;
            Obj generator(rng);

            char seed[8];
            std::fill_n(seed, 8, 255);

            generator.generateSeed(seed, 8);

            verifyResultMatchesRNG(seed, 8);
        }

        if (verbose) printf("Preload memory with known data, and test that"
                            " memory beyond the end of the specified memory is"
                            " not overwritten after a call to 'generateSeed'"
                            " (C-4)\n");
        {
            MockRNG rng;
            Obj generator(rng);

            char seed[24];
            const char maxChar = static_cast<char>(255);
            std::fill_n(seed, 24, maxChar);

            generator.generateSeed(&seed[8], 8);

            for(int i = 0; i < 8; ++i) {
                if(veryVerbose) printf("Asserting seed[%i]: %hhu from"
                                       " generated seed is inchanged\n",
                                       i ,
                                       seed[i]);
                ASSERT(seed[i] == maxChar);
            }

            verifyResultMatchesRNG(&seed[8], 8);

            for(int i = 16; i < 24; ++i) {
                if(veryVerbose) printf("Asserting seed[%i]: %hhu from"
                                       " generated seed is inchanged\n",
                                       i ,
                                       seed[i]);
                ASSERT(seed[i] == maxChar);
            }
        }

        if (verbose) printf("Call 'generateSeed' with a length of zero and"
                            " verify the RNG was not called, and the supplied"
                            " memory was not written. (C-6)\n");
        {
            MockRNG rng;
            Obj generator(rng);

            char seed[8];
            const char maxChar = static_cast<char>(255);
            std::fill_n(seed, 8, maxChar);

            generator.generateSeed(&seed[4], 0);

            for(int i = 0; i < 8; ++i) {
                if(veryVerbose) printf("Asserting seed[%i]: %hhu from"
                                       " generated seed is inchanged\n",
                                       i ,
                                       seed[i]);
                ASSERT(seed[i] == maxChar);
            }

            ASSERT(rng.numberOfCalls() == 0);
        }

        if (verbose) printf("Call 'generateSeed' with lengths that are not"
                            " multiples of the size of 'MockRNG::result_type'."
                            " (C-6)\n");
        {
            char seed[24];
            const char maxChar = static_cast<char>(255);
            std::fill_n(seed, 24, maxChar);

            for(int i = 0; i < 24; ++i) {
                if(veryVerbose) printf("Testing seeds of length %i\n", i);
                MockRNG rng;
                Obj generator(rng);
                generator.generateSeed(seed, i);
                verifyResultMatchesRNG(seed, i);
            }
        }

        if (verbose) printf("Verify no memory was used. (C-7)\n");
        {
            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //   Ensure that the implicitly declared and defined copy constructor
        //   and destructor, as well as the explicitly defined default and
        //   parameterized constructors, are publicly callable and have no
        //   unexpected side effects such as allocating memory.  As there is no
        //   observable state to inspect, there is little to verify other than
        //   that the expected expressions all compile, and:
        //
        // Concerns:
        //: 1 Objects can be created using the user defined default
        //:   constructor.
        //:
        //: 2 Objects can be created using the user defined patameterized
        //:   constructor.
        //:
        //: 3 Objects can be created using the copy constructor.
        //:
        //: 4 The copy constructor is not declared as explicit.
        //:
        //: 5 Objects can be copy constructed from constant objects.
        //:
        //: 6 Objects can be destroyed.
        //:
        //: 7 No memory is allocated by the default and global allocators.
        //
        // Plan:
        //: 1 Install a test allocator as the default allocator.  Then install
        //:   an 'AllocatorGuard' to verify no memory is allocated during the
        //:   execution of this test case.  Memory from the global allocator is
        //:   tested as a global concern. (C-6)
        //:
        //: 2 Create a 'SeedGenerator' with the user defined default
        //:   constuctor. (C-1)
        //:
        //: 3 Create a 'SeedGenerator' with the user defined parameterized
        //:   constuctor. (C-2)
        //:
        //: 4 Use the copy-initialization syntax to create a new instance of
        //:   'SeedGenerator' from an existing instance. (C-3,4)
        //:
        //: 5 Copy the value of the one (const) instance of 'SeedGenerator'
        //:   to a second non-const one. (C-5)
        //:
        //: 6 Create an instance of 'SeedGenerator' and allow it to leave scope
        //:   to be destroyed. (C-6)
        //:
        //: 7 Verify no memory was used. (C-7)
        //
        // Testing:
        //   SeedGenerator()
        //   SeedGenerator(RNG& randomNumberGenerator)
        //   SeedGenerator(const SeedGenerator)
        //   ~SeedGenerator()
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING CREATORS"
                   "\n================\n");

        if (verbose) printf("Install a test allocator as the default"
                            " allocator.  Then install an 'AllocatorGuard' to"
                            " verify no memory is allocated during the"
                            " execution of this test case.  Memory from the"
                            " global allocator is tested as a global concern."
                            " (C-6)\n");
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) printf("Create a 'SeedGenerator' with the user defined"
                            " default constuctor. (C-1)\n");
        {
            Obj generator;
        }

        if (verbose) printf("Create a 'SeedGenerator' with the user defined"
                            " parameterized constuctor. (C-2)\n");
        {
            MockRNG rng;
            Obj generator(rng);
        }

        if (verbose) printf("Use the copy-initialization syntax to create a"
                            " new instance of 'SeedGenerator' from an existing"
                            " instance. (C-3,4)\n");
        {
            MockRNG rng;
            Obj generator1(rng);
            Obj generator2(generator1);
            Obj generator3 = generator1;
        }

        if (verbose) printf("Copy the value of the one (const) instance of"
                            " 'SeedGenerator' to a second non-const one."
                            " (C-5)\n");
        {
            MockRNG rng;
            const Obj generator1 = Obj(rng);
            Obj generator2(generator1);
            Obj generator3 = generator1;
        }

        if (verbose) printf("Create an instance of 'SeedGenerator' and allow"
                            " it to leave scope to be destroyed. (C-6)\n");
        {
            MockRNG rng;
            Obj generator(rng);
        }

        if (verbose) printf("Verify no memory was used. (C-7)\n");
        {
            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create an instance of 'bslh::SeedGenerator<>'. (C-1)
        //:
        //: 2 Verify 'generateSeed' returns a value. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("Create an instance of 'bslh::SeedGenerator<>'."
                            " (C-1)\n");
        {
            MockRNG mockRNG;
            Obj seedGenerator(mockRNG);
        }

        if (verbose) printf("Verify 'generateSeed' returns a value. (C-1)\n");
        {
            MockRNG mockRNG;
            Obj seedGenerator(mockRNG);

            char seed[8];
            seedGenerator.generateSeed(seed, 8);

            ASSERT(seed[0] == 1);
        }
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright (C) 2013 Bloomberg Finance L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
