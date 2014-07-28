// bslh_seededhash.t.cpp                                              -*-C++-*-
#include <bslh_seededhash.h>
#include <bslh_defaulthashalgorithm.h>
#include <bslh_securehashalgorithm.h>
#include <bslh_seedgenerator.h>
#include <bslh_siphashalgorithm.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

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
// [ 3] operator()(const T&) const
// [ 2] Hash()
// [ 2] Hash(SEED_GENERATOR seedGenerator)
// [ 2] Hash(const Hash)
// [ 2] ~Hash()
// [ 2] Hash& operator=(const Hash&)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
// [ 4] typedef result_type
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
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Storing User Defined Input in a Hash Table
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have any array of user-specified nicknames, and we want a really
// fast way to find out if values are contained in the array. We can create a
// 'HashTable' data structure that is capable of looking up values in O(1)
// time. The hash table implemented here can have numerous simplifications
// because of our more relaxed specifications (it is more of a hash set), we
// know the size of the array, and never have to resize the table.
//
// Because we will be storing arbitrary user input in our table, it is possible
// that an attacker with knowledge of the hashing algorithm we are using could
// specially craft input that will cause collisions in our hash table,
// degrading performance to O(n). To avoid this we will need to use a secure
// hash algorithm with a random seed. This algorithm will need to be  in the
// form of a hash functor -- an object that will take objects stored in our
// array as input, and yield a 64-bit int value which is hard enough for an
// outside observer to predict that it appear random. 'bslh::SeededHash'
// provides a convenient functor that can wrap any seeded hashing algorithm and
// use it to produce a hash for any type them implements 'hashAppend'.
//
// We can use the result of the hash function to index into our array of
// 'buckets'.  Each 'bucket' is simply a pointer to a value in our original
// array of 'TYPE' objects.  We will resolve hash collisions in our array
// through 'linear probing', where we will search consecutive buckets following
// the bucket where the collision occurred, testing occupied buckets for
// equality with the value we are searching on, and concluding that the value
// is not in the table if we encounter an empty bucket before we encounter one
// referring to an equal element.
//
// First, we define our 'HashTable' template class, with the two type
// parameters: 'TYPE' (the type being referenced) and 'HASHER' (a functor that
// produces the hash).
//..

template <class TYPE, class HASHER>
class HashTable {
    // This hash table to provides a fast lookup of an external, non-owned,
    // array of values of configurable type.
    //
    // The only requirement for 'TYPE' is that it have a transitive, symmetric
    // 'operator==' function and that a it is hashable using 'bslh::Hash'.
    // There is no requirement that it have any kind of creator defined.
    //
    // The 'HASHER' template parameter type must be a functor with a function
    // of the following signature:
    //..
    //  size_t operator()(const TYPE)  const;
    //                   -OR-
    //  size_t operator()(const TYPE&) const;
    //..
    // and 'HASHER' must have a publicly available default constructor and
    // destructor. Here we use 'bslh::Hash' as our default value. This allows
    // us to hash any types that implement a 'hashAppend' method.

    // DATA
    const TYPE       *d_values;             // Array of values table is to hold
    size_t            d_numValues;          // Length of 'd_values'.
    const TYPE      **d_bucketArray;        // Contains ptrs into 'd_values'
    unsigned          d_bucketArrayMask;    // Will always be '2^N - 1'.
    HASHER            d_hasher;
    bool              d_valid;              // Object was properly initialized.
    bslma::Allocator *d_allocator_p;        // held, not owned

  private:
    // PRIVATE ACCESSORS
    bool lookup(size_t      *idx,
                const TYPE&  value,
                size_t       hashValue) const
        // Look up the specified 'value', having hash value 'hashValue', and
        // return its index in 'd_bucketArray' stored in the specified 'idx.
        // If not found, return the vacant entry in 'd_bucketArray' where it
        // should be inserted.  Return 'true' if 'value is found and 'false'
        // otherwise.
    {
        const TYPE *ptr;
        for (*idx = hashValue & d_bucketArrayMask; (ptr = d_bucketArray[*idx]);
                                       *idx = (*idx + 1) & d_bucketArrayMask) {
            if (value == *ptr) {
                return true;                                          // RETURN
            }
        }
        // value was not found in table

        return false;
    }

  public:
    // CREATORS
    HashTable(const TYPE       *valuesArray,
              size_t            numValues,
              HASHER            hasher,
              bslma::Allocator *allocator = 0)
        // Create a hash table referring to the specified 'valuesArray',
        // containing 'numValues', and generating hashes with the specified
        // 'hasher'.  Optionally specify 'allocator' or the default allocator
        // will be used`.
    : d_values(valuesArray)
    , d_numValues(numValues)
    , d_hasher(hasher)
    , d_valid(true)
    , d_allocator_p(bslma::Default::allocator(allocator))
    {
        size_t bucketArrayLength = 4;
        while (bucketArrayLength < numValues * 4) {
            bucketArrayLength *= 2;
            BSLS_ASSERT_OPT(bucketArrayLength);
        }
        d_bucketArrayMask = bucketArrayLength - 1;
        d_bucketArray = static_cast<const TYPE **>(d_allocator_p->allocate(
                                         bucketArrayLength * sizeof(TYPE **)));
        memset(d_bucketArray,  0, bucketArrayLength * sizeof(TYPE *));

        for (unsigned i = 0; i < numValues; ++i) {
            const TYPE& value = d_values[i];
            size_t idx;
            if (lookup(&idx, value, d_hasher(value))) {
                // Duplicate value.  Fail.

                printf("Error: entries %u and %u have the same value\n",
                       i,
                       static_cast<unsigned>((d_bucketArray[idx] - d_values)));
                d_valid = false;

                // don't return, continue reporting other redundant entries.
            }
            else {
                d_bucketArray[idx] = &d_values[i];
            }
        }
    }

    ~HashTable()
        // Free up memory used by this hash table.
    {
        d_allocator_p->deallocate(d_bucketArray);
    }

    // ACCESSORS
    int count(const TYPE& value) const
        // Return 1 if the specified 'value' is found in the hash table and 0
        // otherwise.
    {
        BSLS_ASSERT_OPT(d_valid);

        size_t idx;
        return lookup(&idx, value, d_hasher(value));
    }

    bool isValid() const
        // Return 'true' if this hash table was successfully constructed and
        // 'false' otherwise.
    {
        return d_valid;
    }
};

//=============================================================================
//          GLOBAL TYPEDEFS, HELPER FUNCTIONS, AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class MockRNG {
    // Predictable mock random number generator for use in testing
  public:
    typedef unsigned long long result_type;
        // The type of the random data that 'operator()' will return.

  private:
    unsigned long long counter;
        // Counter that provides some variance in the random numbers returned.

  public:
    MockRNG();
        // Create a 'MockRNG' that will return predictable "random" values.

    result_type operator()();
        // Return a predictable "random" number of 'result_type'.
};

MockRNG::MockRNG() : counter(0) { }

MockRNG::result_type MockRNG::operator()() {
    return ++counter;
}

typedef MockRNG CryptographicallySecureRNG;
    // Not actually cryptographically secure!

typedef bslh::SeedGenerator<MockRNG> SeedGen;
typedef bslh::SeededHash<SeedGen, SpookyHashAlgorithm>    Obj;

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
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The 'bslh::SeededHash' can be used with seeded algorithms to
        //   secure more powerful components such as hash tables.
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
// Then, we will create an array of user supplied nicknames that would create
// collisions in some other hashing algorithm.
//..

        const char names[6][11] = { "COLLISION!",
                                    "COLLISION@",
                                    "COLLISION#",
                                    "COLLISION$",
                                    "COLLISION%",
                                    "COLLISION^"};

        enum { NUM_NAMES = sizeof names / sizeof *names };

//..
// Next, we create a seed generator, with a cryptographically secure random
// number generator, that can be used to generate seeds for our secure hashing
// algorithm. We then pass that seed generator into 'bslh::SeededHash'. We use
// the 'bslh::SecureHashAlgorithm' typedef to pick our secure hashing algorithm
// for us.
//..
        typedef SeedGenerator<CryptographicallySecureRNG> SecureSeedGenerator;
        typedef SeededHash<SecureSeedGenerator, SecureHashAlgorithm>
                                                                    SecureHash;

        SecureSeedGenerator secureSeedGenerator;
        SecureHash          secureHash(secureSeedGenerator);

//..
// Then, we create our hash table and verify that it constructed properly.  We
// pass it the 'secureHash' hashing functor we created. Passing it in through
// the functor, rather than just having it default constructed from the
// template parameter, allows us to pass in an algorithm with a pre-configured
// state if we so desire.
//..

        HashTable<const char [11], SecureHash> hashTable(names,
                                                         NUM_NAMES,
                                                         secureHash);
        ASSERT(hashTable.isValid());

// Now, we verify that each element in our array registers with count:
        for( int i = 0; i < NUM_NAMES; ++i) {
            ASSERT(1 == hashTable.count(names[i]));
        }

// Finally, we verify that futures not in our original array are correctly
// identified as not being in the set:

        ASSERT(0 == hashTable.count("asdfasdfas"));
        ASSERT(0 == hashTable.count("asdfqwerqw"));
        ASSERT(0 == hashTable.count("asdfqwerzx"));

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING STANDARD TYPEDEFS
        //   Verify that the struct properly forwards the typedefs of the
        //   algorithm it is passed.
        //
        // Concerns:
        //: 1 The typedef 'result_type' is publicly accessible and an alias for
        //:   'size_t'.
        //
        // Plan:
        //: 1 ASSERT the typedef accessibly aliases the correct type using
        //:   'bslmf::IsSame'. (C-1)
        //
        // Testing:
        //   typedef result_type
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STANDARD TYPEDEFS"
                            "\n=========================\n");

        if (verbose) printf("ASSERT the typedef accessibly aliases the correct"
                            " type using 'bslmf::IsSame'. (C-1)\n");
        {

            ASSERT((bslmf::IsSame<size_t, Obj::result_type>::VALUE));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION CALL OPERATOR
        //   Verify that the struct offers the ability to invoke it with some
        //   bytes and a length, and that it return a hash.
        //
        // Concerns:
        //: 1 The function call operator will return the expected value
        //:   according to the canonical implementation of the algorithm being
        //:   used.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //:
        //: 3 No memory is allocated from the default or global allocators.
        //
        // Plan:
        //: 1 Install a test allocator as the default allocator.  Then install
        //:   an 'AllocatorGuard' to verify no memory is allocated during the
        //:   execution of this test case.  Memory from the global allocator is
        //:   tested as a global concern. (C-3)
        //:
        //: 2 Create 'const' ints and hash them. Compare the results against
        //:   known good values. (C-1,2)
        //:
        //: 3 Verify no memory was used. (C-3)
        //
        // Testing:
        //   operator()(const T&) const
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FUNCTION CALL OPERATOR"
                            "\n==============================\n");

        static const struct {
            int                  d_line;
            const int            d_value;
            bsls::Types::Uint64  d_hash;
        } DATA[] = {
        // LINE    DATA              HASH
         {  L_,        1,  9778072230994240314ULL,},
         {  L_,        3, 16874605512690156844ULL,},
         {  L_,        9,  6609278684846086166ULL,},
         {  L_,       27, 14610053422485613907ULL,},
         {  L_,       81,  4473763709117720193ULL,},
         {  L_,      243,  6469189993869193617ULL,},
         {  L_,      729, 18245170745653607298ULL,},
         {  L_,     2187,  4418771231001558887ULL,},
         {  L_,     6561,  8361494415593539480ULL,},
         {  L_,    19683,  8034516711244389554ULL,},
         {  L_,    59049, 15257840606198213647ULL,},
         {  L_,   177147,  9838846006369268307ULL,},
         {  L_,   531441,  2891007685366740764ULL,},
         {  L_,  1594323,  3005240762459740192ULL,},
         {  L_,  4782969,  3383268391725748969ULL,},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;


        if (verbose) printf("Install a test allocator as the default"
                            " allocator.  Then install an 'AllocatorGuard' to"
                            " verify no memory is allocated during the"
                            " execution of this test case.  Memory from the"
                            " global allocator is tested as a global concern."
                            " (C-3)\n");
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);


        if (verbose) printf("Create 'const' strings and hash them. Compare the"
                            " results against known good values. (C-1,2)\n");
        {
            for (int i = 0; i != NUM_DATA; ++i) {
                const int     LINE  = DATA[i].d_line;
                const int     VALUE = DATA[i].d_value;
                const size_t  HASH  = static_cast<size_t>(DATA[i].d_hash);

                if (veryVerbose) printf("Hashing: %i, Expecting: %lu\n",
                                        VALUE,
                                        HASH);

                Obj hash = Obj();
                LOOP_ASSERT(LINE, hash(VALUE) == HASH);
            }
        }

        if (verbose) printf("Verify no memory was used. (C-3)\n");
        {
            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //   Ensure that the implicitly defined copy construct, destructor, and
        //   assignment operator, as well as the explicitly defined default and
        //   parameterized constructors are publicly callable and have no
        //   unexpected side effects such as allocating memory.  As there is no
        //   observable state to inspect, there is little to verify other than
        //   that the expected expressions all compile, and:
        //
        // Concerns:
        //: 1 Objects can be created using the default constructor.
        //:
        //: 2 Objects can be created using the parameterized constructor.
        //:
        //: 3 Objects can be created using the copy constructor.
        //:
        //: 4 The copy constructor is not declared as explicit.
        //:
        //: 5 Objects can be assigned to from constant objects.
        //:
        //: 6 Assignments operations can be chained.
        //:
        //: 7 Objects can be destroyed.
        //:
        //: 8 No memory is allocated by the default and global allocators.
        //
        // Plan:
        //: 1 Install a test allocator as the default allocator.  Then install
        //:   an 'AllocatorGuard' to verify no memory is allocated during the
        //:   execution of this test case.  Memory from the global allocator is
        //:   tested as a global concern. (C-8)
        //:
        //: 2 Create a default constructed 'SeededHash'. (C-1)
        //:
        //: 3 Construct a 'SeededHash' using the parameterized constructor.
        //:   (C-2)
        //:
        //: 4 Use the copy-initialization syntax to create a new instance of
        //:   'SeededHash' from an existing instance. (C-3,4)
        //:
        //: 5 Assign the value of the one (const) instance of 'SeededHash' to a
        //:   second. (C-5)
        //:
        //: 6 Chain the assignment of the value of the one instance of
        //:   'SeededHash' to a second instance of 'SeededHash', into a
        //:   self-assignment of the second object. (C-6)
        //:
        //: 7 Create an instance of 'SeededHash' and allow it to leave scope to
        //:   be destroyed. (C-7)
        //:
        //: 8 Verify no memory was used. (C-8)
        //
        // Testing:
        //   Hash()
        //   Hash(SEED_GENERATOR seedGenerator)
        //   Hash(const Hash)
        //   ~Hash()
        //   Hash& operator=(const Hash&)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING CREATORS"
                   "\n================\n");

        if (verbose) printf("Install a test allocator as the default"
                            " allocator.  Then install an 'AllocatorGuard' to"
                            " verify no memory is allocated during the"
                            " execution of this test case.  Memory from the"
                            " global allocator is tested as a global concern."
                            " (C-8)\n");
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);


        if (verbose) printf("Create a default constructed 'SeededHash'."
                            " (C-1)\n");
        {
            Obj alg1;
        }

        if (verbose) printf("Construct a 'SeededHash' using the parameterized"
                            " constructor. (C-2)\n");
        {
            SeedGen seedGen;
            Obj alg1(seedGen);
        }

        if (verbose) printf("Use the copy-initialization syntax to create a"
                            " new instance of 'SeededHash' from an existing"
                            " instance. (C-3,4)\n");
        {
            Obj alg1;
            Obj alg2 = alg1;
        }

        if (verbose) printf("Assign the value of the one (const) instance of"
                            " 'SeededHash' to a second. (C-5)\n");
        {
            const Obj alg1 = Obj();
            Obj alg2 = alg1;
        }

        if (verbose) printf("Chain the assignment of the value of the one"
                            " instance of 'SeededHash' to a second instance of"
                            " 'SeededHash', into a self-assignment of the"
                            " second object. (C-6)\n");
        {
            Obj alg1;
            Obj alg2 = alg1;
            alg2 = alg2 = alg1;
        }

        if (verbose) printf("Create an instance of 'SeededHash' and allow it"
                            " to leave scope to be destroyed. (C-7)\n");
        {
            Obj alg1;
        }

        if (verbose) printf("Verify no memory was used. (C-8)\n");
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
        //: 1 Create an instance of 'bslh::SecureHash'. (C-1)
        //:
        //: 2 Verify different hashes are produced for different ints. (C-1)
        //:
        //: 3 Verify the same hashes are produced for the same ints. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("Create an instance of 'bslh::SecureHash'."
                            " (C-1)\n");
        {
            Obj hashAlg;
        }

        if (verbose) printf("Verify different hashes are produced for"
                            " different ints. (C-1)\n");
        {
            Obj hashAlg;
            int int1 = 123456;
            int int2 = 654321;
            ASSERT(hashAlg(int1) != hashAlg(int2));
        }

        if (verbose) printf("Verify the same hashes are produced for the same"
                            " ints. (C-1)\n");
        {
            Obj hashAlg;
            int int1 = 123456;
            int int2 = 123456;
            ASSERT(hashAlg(int1) == hashAlg(int2));
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
