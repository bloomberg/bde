// bslh_seededhash.t.cpp                                              -*-C++-*-
#include <bslh_seededhash.h>
#include <bslh_defaultseededhashalgorithm.h>
#include <bslh_seedgenerator.h>
#include <bslh_siphashalgorithm.h>

#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>
#include <bsls_bsltestutil.h>

#include <math.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bslh;


//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test is a standards-conformant hashing algorithm
// functor.  The component will be tested for conformance to the interface
// requirements on 'std::hash', outlined in the C++ Standard.  The output of
// the component will also be tested to check that it matches the expected
// output of the underlying hashing algorithms.
//-----------------------------------------------------------------------------
// TYPEDEF
// [ 4] typedef size_t result_type;
//
// CREATORS
// [ 3] operator()(const T&) const
// [ 2] SeededHash()
// [ 2] SeededHash(SEED_GENERATOR seedGenerator)
// [ 2] SeededHash(const SeededHash)
// [ 2] ~SeededHash()
//
// MANIPULATORS
// [ 2] SeededHash& operator=(const SeededHash&)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
//-----------------------------------------------------------------------------

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BSL TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLS_BSLTESTUTIL_ASSERT
#define ASSERTV      BSLS_BSLTESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLS_BSLTESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLS_BSLTESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLS_BSLTESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLS_BSLTESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLS_BSLTESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLS_BSLTESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLS_BSLTESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLS_BSLTESTUTIL_LOOP6_ASSERT

#define Q            BSLS_BSLTESTUTIL_Q   // Quote identifier literally.
#define P            BSLS_BSLTESTUTIL_P   // Print identifier and value.
#define P_           BSLS_BSLTESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLS_BSLTESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLS_BSLTESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU
#define U64 BSLS_BSLTESTUTIL_FORMAT_U64

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------
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
// hash algorithm with a random seed.  This algorithm will need to be  in the
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

    template <class TYPE, class HASHER>
    class HashTable {
        // This class template implements a hash table providing fast lookup of
        // an external, non-owned, array of values of (template parameter)
        // 'TYPE'.
        //
        // The (template parameter) 'TYPE' shall have a transitive, symmetric
        // 'operator==' function and it will be hashable using 'bslh::Hash'.
        // Note that there is no requirement that it have any kind of creator
        // defined.
        //
        // The 'HASHER' template parameter type must be a functor with a method
        // having the following signature:
        //..
        //  size_t operator()(TYPE)  const;
        //                   -OR-
        //  size_t operator()(const TYPE&) const;
        //..
        // and 'HASHER' shall have a publicly accessible default constructor
        // and destructor.  Here we use 'bslh::Hash' as our default template
        // argument.  This allows us to hash any type for which 'hashAppend'
        // has been implemented.
        //
        // Note that this hash table has numerous simplifications because we
        // know the size of the array and never have to resize the table.

        // DATA
        const TYPE       *d_values;          // Array of values table is to
                                             // hold
        size_t            d_numValues;       // Length of 'd_values'.
        const TYPE      **d_bucketArray;     // Contains ptrs into 'd_values'
        unsigned          d_bucketArrayMask; // Will always be '2^N - 1'.
        HASHER            d_hasher;          // User supplied hashing algorithm

      private:
        // PRIVATE ACCESSORS
        bool lookup(size_t      *idx,
                    const TYPE&  value,
                    size_t       hashValue) const;
            // Look up the specified 'value', having the specified 'hashValue',
            // and load its index in 'd_bucketArray' into the specified 'idx'.
            // If not found, return the vacant entry in 'd_bucketArray' where
            // it should be inserted.  Return 'true' if 'value' is found and
            // 'false' otherwise.

      public:
        // CREATORS
        HashTable(const TYPE *valuesArray,
                  size_t      numValues,
                  HASHER      hasher);
            // Create a hash table referring to the specified 'valuesArray'
            // having length of the specified 'numValues' and using the
            // specified 'hasher' to generate hash values.  No value in
            // 'valuesArray' shall have the same value as any of the other
            // values in 'valuesArray'

        ~HashTable();
            // Free up memory used by this cross-reference.

        // ACCESSORS
        bool contains(const TYPE& value) const;
            // Return true if the specified 'value' is found in the table and
            // false otherwise.
    };

//=============================================================================
//                     ELIDED USAGE EXAMPLE IMPLEMENTATIONS
//-----------------------------------------------------------------------------

// PRIVATE ACCESSORS
template <class TYPE, class HASHER>
bool HashTable<TYPE, HASHER>::lookup(size_t      *idx,
                                     const TYPE&  value,
                                     size_t       hashValue) const
{
    const TYPE *ptr;
    for (*idx = hashValue & d_bucketArrayMask; (ptr = d_bucketArray[*idx]);
                                   *idx = (*idx + 1) & d_bucketArrayMask) {
        if (value == *ptr) {
            return true;                                              // RETURN
        }
    }
    // value was not found in table

    return false;
}

// CREATORS
template <class TYPE, class HASHER>
HashTable<TYPE, HASHER>::HashTable(const TYPE *valuesArray,
                                   size_t      numValues,
                                   HASHER      hasher)
: d_values(valuesArray)
, d_numValues(numValues)
, d_hasher(hasher)
{
    size_t bucketArrayLength = 4;
    while (bucketArrayLength < numValues * 4) {
        bucketArrayLength *= 2;

    }
    d_bucketArrayMask = bucketArrayLength - 1;
    d_bucketArray = new const TYPE *[bucketArrayLength];
    memset(d_bucketArray,  0, bucketArrayLength * sizeof(TYPE *));

    for (unsigned i = 0; i < numValues; ++i) {
        const TYPE& value = d_values[i];
        size_t idx;
        BSLS_ASSERT_OPT(!lookup(&idx, value, d_hasher(value)));
        d_bucketArray[idx] = &d_values[i];
    }
}

template <class TYPE, class HASHER>
HashTable<TYPE, HASHER>::~HashTable()
{
    delete [] d_bucketArray;
}

// ACCESSORS
template <class TYPE, class HASHER>
bool HashTable<TYPE, HASHER>::contains(const TYPE& value) const
{
    size_t idx;
    return lookup(&idx, value, d_hasher(value));
}

//=============================================================================
//          GLOBAL TYPEDEFS, HELPER FUNCTIONS, AND CLASSES FOR TESTING
//-----------------------------------------------------------------------------

class MockRNG {
    // This class implements a predictable mock random number generator for use
    // in testing.

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

template<class EXPECTED_TYPE>
class TypeChecker {
    // Provides a member function to determine if passed data is of the same
    // type as the (template parameter) 'EXPECTED_TYPE'
  public:
      static bool isCorrectType(EXPECTED_TYPE type);
      template<class OTHER_TYPE>
      static bool isCorrectType(OTHER_TYPE type);
          // Return true if the specified 'type' is of the same type as the
          // (template parameter) 'EXPECTED_TYPE'.
};

template<class EXPECTED_TYPE>
bool TypeChecker<EXPECTED_TYPE>::isCorrectType(EXPECTED_TYPE) {
    return true;
}

template<class EXPECTED_TYPE>
template<class OTHER_TYPE>
bool TypeChecker<EXPECTED_TYPE>::isCorrectType(OTHER_TYPE type) {
    return false;
}

typedef MockRNG CryptographicallySecureRNG;
    // Not actually cryptographically secure!

typedef bslh::SeedGenerator<MockRNG> SeedGen;
typedef bslh::SeededHash<SeedGen, DefaultSeededHashAlgorithm> Obj;

typedef bsls::Types::Uint64 u64;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVerbose;      // suppress warning
    (void)veryVeryVeryVerbose;  // suppress warning

    printf("TEST " __FILE__ " CASE %d\n", test);

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
// algorithm.  We then pass that seed generator into 'bslh::SeededHash'.  We
// use the 'bslh::SipHashAlgorithm' as our secure hashing algorithm.
//..
        typedef SeedGenerator<CryptographicallySecureRNG> SecureSeedGenerator;
        typedef SeededHash<SecureSeedGenerator, SipHashAlgorithm> SecureHash;

        SecureSeedGenerator secureSeedGenerator;
        SecureHash          secureHash(secureSeedGenerator);

//..
// Then, we create our hash table 'hashTable'.  We pass it the 'secureHash'
// hashing functor we created.  Passing it in through the functor, rather than
// just having it default constructed from the template parameter, allows us to
// pass in an algorithm with a pre-configured state if we so desire.
//..

        HashTable<const char [11], SecureHash> hashTable(names,
                                                         NUM_NAMES,
                                                         secureHash);

// Now, we verify that each element in our array registers with count:
        for ( int i = 0; i < NUM_NAMES; ++i) {
            ASSERT(hashTable.contains(names[i]));
        }

// Finally, we verify that futures not in our original array are correctly
// identified as not being in the set:

        ASSERT(!hashTable.contains("asdfasdfas"));
        ASSERT(!hashTable.contains("asdfqwerqw"));
        ASSERT(!hashTable.contains("asdfqwerzx"));

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING STANDARD TYPEDEFS
        //   Verify that the struct hashes the proper 'typedef's.
        //
        // Concerns:
        //: 1 The typedef 'result_type' is publicly accessible and an alias for
        //:   'size_t'.
        //:
        //: 2 'result_type' is 'size_t' even when the algorithm returns a
        //:   'result_type' of a different size
        //:
        //: 3 'operator()' returns 'result_type'
        //
        //
        // Plan:
        //: 1 ASSERT the 'typedef' accessibly aliases the correct type using
        //:   'bslmf::IsSame' for a number of algorithms of different result
        //:   types. (C-1,2)
        //:
        //: 2 Invoke 'operator()' and verify the return type is 'result_type'.
        //:   (C-3)
        //
        // Testing:
        //   typedef size_t result_type;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STANDARD TYPEDEFS"
                            "\n=========================\n");

        if (verbose) printf("ASSERT the 'typedef' accessibly aliases the"
                            " correct type using 'bslmf::IsSame' for a number"
                            " of algorithms of different result types."
                            " (C-1,2)\n");
        {

            ASSERT((bslmf::IsSame<size_t, Obj::result_type>::VALUE));
            ASSERT((bslmf::IsSame<size_t,
                                  SeededHash<SeedGen,
                                             DefaultSeededHashAlgorithm>
                                                      ::result_type >::VALUE));
            ASSERT((bslmf::IsSame<size_t,
                                  SeededHash<SeedGen, SipHashAlgorithm>
                                                       ::result_type>::VALUE));
            ASSERT((bslmf::IsSame<size_t,
                                  SeededHash<SeedGen, SpookyHashAlgorithm>
                                                       ::result_type>::VALUE));
        }

        if (verbose) printf("Invoke 'operator()' and verify the return type is"
                            " 'result_type'. (C-3)\n");
        {
            typedef SeededHash<SeedGen, DefaultSeededHashAlgorithm> S1;
            typedef SeededHash<SeedGen, SipHashAlgorithm>           S2;
            typedef SeededHash<SeedGen, SpookyHashAlgorithm>        S3;

            ASSERT(TypeChecker<S1::result_type>::isCorrectType(S1()(1)));

            ASSERT(TypeChecker<S1::result_type>::isCorrectType(S2()(1)));

            ASSERT(TypeChecker<S1::result_type>::isCorrectType(S3()(1)));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'operator()'
        //   Verify that the struct offers the ability to invoke it with some
        //   bytes and a length, and that it return a hash.
        //
        // Concerns:
        //: 1 The function call operator will return the expected value
        //:   according to the canonical implementation of the algorithm being
        //:   used.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //
        // Plan:
        //: 1 Create 'const' ints and hash them.  Compare the results against
        //:   known good values.  (C-1,2)
        //
        // Testing:
        //   operator()(const T&) const
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING 'operator()'"
                            "\n====================\n");

        static const struct {
            int d_line;
            int d_value;
            u64 d_expectedHash;
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
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) printf("Create 'const' strings and hash them.  Compare"
                            " the results against known good values."
                            "  (C-1,2)\n");
        {
            for (int i = 0; i != NUM_DATA; ++i) {
                const int     LINE  = DATA[i].d_line;
                const int     VALUE = DATA[i].d_value;
                const u64 HASH  = DATA[i].d_expectedHash;

                Obj hash = Obj();
                const u64 result = hash(VALUE);
                size_t truncResult = size_t(result);
                size_t truncExpect = size_t(HASH);
                if (veryVerbose) printf(
                             "Hashing: %i, Expecting: " U64 ", Got: " U64 "\n",
                             VALUE, u64(truncExpect), u64(truncResult));
                LOOP_ASSERT(LINE, truncResult == truncExpect);

                const Obj constHash = Obj();
                size_t constTruncResult = size_t(constHash(VALUE));
                LOOP_ASSERT(LINE, constTruncResult == truncExpect);
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING CREATORS
        //   Ensure that the implicitly defined copy construct, destructor, and
        //   assignment operator, as well as the explicitly defined default and
        //   parameterized constructors are publicly callable.  As there is no
        //   observable state to inspect, there is little to verify other than
        //   that the expected expressions all compile.
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
        //
        // Plan:
        //: 1 Create a default constructed 'SeededHash' and allow it to leave
        //:   scope to be destroyed. (C-1,7)
        //:
        //: 2 Construct a 'SeededHash' using the parameterized constructor.
        //:   (C-2)
        //:
        //: 3 Use the copy-initialization syntax to create a new instance of
        //:   'SeededHash' from an existing instance. (C-3,4)
        //:
        //: 4 Assign the value of the one (const) instance of 'SeededHash' to a
        //:   second. (C-5)
        //:
        //: 5 Chain the assignment of the value of the one instance of
        //:   'SeededHash' to a second instance of 'SeededHash', into a
        //:   self-assignment of the second object. (C-6)
        //
        // Testing:
        //   SeededHash()
        //   SeededHash(SEED_GENERATOR seedGenerator)
        //   SeededHash(const SeededHash)
        //   ~SeededHash()
        //   SeededHash& operator=(const SeededHash&)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING CREATORS"
                   "\n================\n");

        if (verbose) printf("Create a default constructed 'SeededHash' and"
                            " allow it to leave scope to be destroyed."
                            " (C-1,7)\n");
        {
            Obj alg1 = Obj();
            (void) alg1;
        }

        if (verbose) printf("Construct a 'SeededHash' using the parameterized"
                            " constructor. (C-2)\n");
        {
            SeedGen seedGen;
            Obj alg1(seedGen);
            (void) alg1;
        }

        if (verbose) printf("Use the copy-initialization syntax to create a"
                            " new instance of 'SeededHash' from an existing"
                            " instance. (C-3,4)\n");
        {
            Obj alg1;
            Obj alg2 = alg1;
            (void) alg2;
        }

        if (verbose) printf("Assign the value of the one (const) instance of"
                            " 'SeededHash' to a second. (C-5)\n");
        {
            const Obj alg1 = Obj();
            Obj alg2 = alg1;
            (void) alg2;
        }

        if (verbose) printf("Chain the assignment of the value of the one"
                            " instance of 'SeededHash' to a second instance of"
                            " 'SeededHash', into a self-assignment of the"
                            " second object. (C-6)\n");
        {
            Obj alg1;
            Obj alg2 = alg1;
            alg2 = alg2 = alg1;
            (void) alg2;
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

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
