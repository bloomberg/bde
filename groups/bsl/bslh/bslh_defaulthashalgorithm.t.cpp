// bslh_defaulthashalgorithm.t.cpp                                    -*-C++-*-
#include <bslh_defaulthashalgorithm.h>

#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bslh;


//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------
// TYPES
// [ 4] typedef InternalHashAlgorithm::result_type result_type;
//
// EXPLICIT CONSTRUCTORS
// [ 2] DefaultHashAlgorithm();
//
// IMPLICIT CONSTRUCTORS
// [ 2] DefaultHashAlgorithm(const DefaultHashAlgorithm);
// [ 2] ~DefaultHashAlgorithm();
// [ 2] DefaultHashAlgorithm& operator=(const DefaultHashAlgorithm&);
//
// MANIPULATORS
// [ 3] void operator()(void const* key, size_t len);
// [ 3] result_type computeHash();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] USAGE EXAMPLE
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

// ============================================================================
//                      STANDARD BDE TEST DRIVER MACROS
// ----------------------------------------------------------------------------

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
///Example: Creating and Using a Hash Table
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we have any array of types that define 'operator==', and we want a
// fast way to find out if values are contained in the array. We can create a
// 'HashTable' data structure that is capable of looking up values in O(1)
// time.
//
// Further suppose that we will be storing futures (the financial instruments)
// in this table. Since futures have standardized names, we don't have to worry
// about any malicious values causing collisions. We will want to use a general
// purpose hashing algorithm with a good hash distribution and good speed. This
// algorithm will need to be in the form of a hash functor -- an object that
// will take objects stored in our array as input, and yield an integer value.
// The functor can pass the attributes of the 'TYPE' that are salient to
// hashing into the hashing algorithm, and then return the hash that is
// produced.
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

template <class TYPE, class HASHER>
class HashTable {
    // This class template implements a hash table providing fast lookup of an
    // external, non-owned, array of values of configurable type.
    //
    // The (template parameter) 'TYPE' shall have a transitive, symmetric 'operator=='
    // function.  There is no requirement that it have any kind of creator
    // defined.
    //
    // The 'HASHER' template parameter type must be a functor with a function
    // of the following signature:
    //..
    //  size_t operator()(const TYPE)  const;
    //                   -OR-
    //  size_t operator()(const TYPE&) const;
    //..
    // and 'HASHER' must have a publicly available default constructor and
    // destructor.
    //
    // Note that the hash table implemented here has numerous simplifications
    // because we know the size of the array and never have to resize the
    // table.

    // DATA
    const TYPE       *d_values;             // Array of values we are
                                            // performing lookups on.  Held,
                                            // not owned.
    size_t            d_numValues;          // Length of 'd_values'.
    const TYPE      **d_bucketArray;        // Contains ptrs into 'd_values'
    unsigned          d_bucketArrayMask;    // Will always be '2^N - 1'.
    HASHER            d_hasher;
    bool              d_valid;              // Object was properly initialized.

  private:
    // PRIVATE ACCESSORS
    bool lookup(size_t      *idx,
                const TYPE&  value,
                size_t       hashValue) const
        // Look up the specified 'value', having hash value 'hashValue', and
        // return its index in 'd_bucketArray' using the specified 'idx'.  If
        // not found, return the vacant entry in 'd_bucketArray' where it
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
    HashTable(const TYPE *valuesArray,
              size_t      numValues)
        // Create a hash table referring to the specified 'valuesArray'
        // containing 'numValues'.
    : d_values(valuesArray)
    , d_numValues(numValues)
    , d_hasher()
    , d_valid(true)
    {
        size_t bucketArrayLength = 4;
        while (bucketArrayLength < numValues * 4) {
            bucketArrayLength *= 2;
            BSLS_ASSERT_OPT(bucketArrayLength);
        }
        d_bucketArrayMask = bucketArrayLength - 1;
        d_bucketArray = new const TYPE *[bucketArrayLength];
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
        // Free up memory used by this cross-reference.
    {
        delete [] d_bucketArray;
    }

    // ACCESSORS
    int count(const TYPE& value) const
        // Return 1 if the specified 'value' is found in the table and 0
        // otherwise.
    {
        BSLS_ASSERT_OPT(d_valid);

        size_t idx;
        return lookup(&idx, value, d_hasher(value));
    }

    bool isValid() const
        // Return 'true' if this cross reference was successfully constructed
        // and 'false' otherwise.
    {
        return d_valid;
    }
};

// Then, we define a 'Future' class, which holds a c-string 'name', char
// 'callMonth', and short 'callYear'.

class Future {
    // This class identifies a future contract.  It tracks the name, call month
    // and year of the contract it represents, and allows equality comparison.

    // DATA
    const char *d_name;    // held, not owned
    const char  d_callMonth;
    const short d_callYear;

  public:
    // CREATORS
    Future(const char *name, const char callMonth, const short callYear)
    : d_name(name), d_callMonth(callMonth), d_callYear(callYear)
        // Create a 'Future' object out of the specified 'name', 'callMonth',
        // and 'callYear'.
    {}

    Future() : d_name(""), d_callMonth('\0'), d_callYear(0)
        // Create a 'Future' with default values.
    {}

    // ACCESSORS
    const char * getMonth() const
        // Return the month that this future expires.
    {
        return &d_callMonth;
    }

    const char * getName() const
        // Return the name of this future
    {
        return d_name;
    }

    const short * getYear() const
        // Return the year that this future expires
    {
        return &d_callYear;
    }

    bool operator==(const Future& other) const
        // Compare this to the specified 'other' object and return true if they
        // are equal
    {
        return (!strcmp(d_name, other.d_name))  &&
           d_callMonth == other.d_callMonth &&
           d_callYear  == other.d_callYear;
    }
};

bool operator!=(const Future& lhs, const Future& rhs)
    // Compare compare the specified 'lhs' and 'rhs' objects and return true if
    // they are not equal
{
    return !(lhs == rhs);
}

// Next, we need a hash functor for 'Future'.  We are going to use the
// 'DefaultHashAlgorithm' because it is a fast, general purpose hashing
// algorithm that will provide an easy way to combine the attributes of
// 'Future' objects that are salient to hashing into one reasonable hash that
// will distribute the items evenly throughout the hash table. Moreover, when a
// new hashing algorithm is discovered to be a better default, we can be
// automatically be upgraded to use it as soon as 'bslh::DefaultHashAlgorithm'
// is updated.

struct HashFuture {
    // This struct is a functor that will apply the 'DefaultHashAlgorithm' to
    // objects of type 'Future'.

    size_t operator()(const Future& future) const
        // Return the hash of the of the specified 'future'.  Note that this
        // uses the 'DefaultHashAlgorithm' to quickly combine the attributes of
        // 'Future' objects that are salient to hashing into a hash suitable
        // for a hash table.
    {
        DefaultHashAlgorithm hash;

        hash(future.getName(),  strlen(future.getName())*sizeof(char));
        hash(future.getMonth(), sizeof(char));
        hash(future.getYear(),  sizeof(short));

        return static_cast<size_t>(hash.computeHash());
    }
};

//=============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
//-----------------------------------------------------------------------------

typedef DefaultHashAlgorithm Obj;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
//  bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The hashing algorithm can be used to create more powerful
        //   components such as functors that can be used to power hash tables.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

// Then, we want to actually use our hash table on 'Future' objects.  We
// create an array of 'Future's based on data that was originally from some
// external source:

        Future futures[] = { Future("Swiss Franc", 'F', 2014),
                             Future("US Dollar", 'G', 2015),
                             Future("Canadian Dollar", 'Z', 2014),
                             Future("British Pound", 'M', 2015),
                             Future("Deutsche Mark", 'X', 2016),
                             Future("Eurodollar", 'Q', 2017)};
        enum { NUM_FUTURES =
                              sizeof futures / sizeof *futures };

// Next, we create our HashTable 'hashTable' and verify that it constructed
// properly.  We pass the functor that we defined above as the second argument:

        HashTable<Future, HashFuture> hashTable(futures, NUM_FUTURES);
        ASSERT(hashTable.isValid());

// Now, we verify that each element in our array registers with count:
        for ( int i = 0; i < 6; ++i) {
            ASSERT(1 == hashTable.count(futures[i]));
        }

// Finally, we verify that futures not in our original array are correctly
// identified as not being in the set:

        ASSERT(0 == hashTable.count(Future("French Franc", 'N', 2019)));
        ASSERT(0 == hashTable.count(Future("Swiss Franc", 'X', 2014)));
        ASSERT(0 == hashTable.count(Future("US Dollar", 'F', 2014)));

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING STANDARD TYPEDEF
        //   Verify that the class offers the result_type typedef that needs to
        //   be exposed by all 'bslh' hashing algorithms
        //
        // Concerns:
        //: 1 The typedef 'result_type' is publicly accessible and an alias for
        //:   'bslh::SpookyHashAlgorithm::result_type'.
        //
        // Plan:
        //: 1 ASSERT the typedef is accessible and is the correct type using
        //:   'bslmf::IsSame'. (C-1)
        //
        // Testing:
        //   typedef InternalHashAlgorithm::result_type result_type;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STANDARD TYPEDEF"
                            "\n========================\n");

        if (verbose) printf("ASSERT the typedef is accessible and is the"
                            " correct type using 'bslmf::IsSame'. (C-1)\n");
        {
            ASSERT((bslmf::IsSame<Obj::result_type,
                                  SpookyHashAlgorithm::result_type>::VALUE));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION CALL OPERATOR AND 'COMPUTEHASH'
        //   Verify that the class offers the ability to invoke it via
        //   'operator()' with some bytes and a length. Verify that calling
        //   'operator()' will permute the algorithm's internal state as
        //   specified by the underlying hashing algorithm
        //   (bslh::SpookyHashAlgorithm). Verify that 'computeHash' returns the
        //   final value specified by the canonical implementation of the
        //   underlying hashing algorithm.
        //
        // Concerns:
        //: 1 The function call operator exists and is callable.
        //:
        //: 2 The 'computeHash' function exists and is callable.
        //:
        //: 3 The output of calling 'operator()' and then 'computeHash()'
        //:   matches the output of the underlying hashing algorithm.
        //
        // Plan:
        //: 1 Hash a number of values with 'bslh::DefaultHashAlgorithm' and
        //:   'bslh::SpookyHashAlgorithm' and verify that the outputs match.
        //:   (C-1,2,3)
        //
        // Testing:
        //   void operator()(void const* key, size_t len);
        //   result_type computeHash();
        // --------------------------------------------------------------------

        if (verbose) printf(
                       "\nTESTING FUNCTION CALL OPERATOR AND 'COMPUTEHASH'"
                       "\n================================================\n");

        static const struct {
            int                  d_line;
            const char           d_value [21];
        } DATA[] = {
           // LINE           DATA
            {  L_,                     "1",},
            {  L_,                    "12",},
            {  L_,                   "123",},
            {  L_,                  "1234",},
            {  L_,                 "12345",},
            {  L_,                "123456",},
            {  L_,               "1234567",},
            {  L_,              "12345678",},
            {  L_,             "123456789",},
            {  L_,            "1234567890",},
            {  L_,           "12345678901",},
            {  L_,          "123456789012",},
            {  L_,         "1234567890123",},
            {  L_,        "12345678901234",},
            {  L_,       "123456789012345",},
            {  L_,      "1234567890123456",},
            {  L_,     "12345678901234567",},
            {  L_,    "123456789012345678",},
            {  L_,   "1234567890123456789",},
            {  L_,  "12345678901234567890",},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) printf("Hash a number of values with"
                            " 'bslh::DefaultHashAlgorithm' and"
                            " 'bslh::SpookyHashAlgorithm' and verify that the"
                            " outputs match. (C-1,2,3)\n");
        {
            for (int i = 0; i != NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *VALUE = DATA[i].d_value;

                if (veryVerbose) printf("Hashing: %s\n with"
                                        " 'bslh::DefaultHashAlgorithm' and"
                                        " 'bslh::SpookyHashAlgorithm'", VALUE);

                Obj                 contiguousHash;
                Obj                 dispirateHash;
                SpookyHashAlgorithm cannonicalHashAlgorithm;

                cannonicalHashAlgorithm(VALUE, strlen(VALUE));
                contiguousHash(VALUE, strlen(VALUE));

                for (unsigned int j = 0; j < strlen(VALUE); ++j){
                    if (veryVeryVerbose) printf("Hashing by char: %c\n",
                                                                     VALUE[j]);
                    dispirateHash(&VALUE[j], sizeof(char));
                }

                SpookyHashAlgorithm::result_type hash =
                                         cannonicalHashAlgorithm.computeHash();

                LOOP_ASSERT(LINE, hash == contiguousHash.computeHash());
                LOOP_ASSERT(LINE, hash == dispirateHash.computeHash());
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING C'TORS, D'TOR, AND ASSIGNMENT OPERATOR
        //   Ensure that the implicit copy constructor, destructor, and
        //   assignment operator as well as the explicit default constructor
        //   are publicly callable.
        //
        // Concerns:
        //: 1 Objects can be created using the parameterized constructor.
        //:
        //: 2 Objects can be created using the copy constructor.
        //:
        //: 3 The copy constructor is not declared as explicit.
        //:
        //: 4 Objects can be assigned to from constant objects.
        //:
        //: 5 Assignments operations can be chained.
        //:
        //: 6 Objects can be destroyed.
        //
        // Plan:
        //: 1 Instantiate the algorithm using the parameterized constructor.
        //:   (C-1)
        //:
        //: 2 Use the copy-initialization syntax to create a new instance of
        //:   'DefaultHashAlgorithm' from an existing instance. (C-2,3)
        //:
        //: 3 Assign the value of the one (const) instance of
        //:   'DefaultHashAlgorithm' to a second. (C-4)
        //:
        //: 4 Chain the assignment of the value of the one instance of
        //:   'DefaultHashAlgorithm' to a second instance of
        //:   'DefaultHashAlgorithm', into a self-assignment of the second
        //:   object. (C-5)
        //:
        //: 5 Create an instance of 'DefaultHashAlgorithm' and allow it to
        //:   leave scope to be destroyed. (C-6)
        //
        // Testing:
        //   DefaultHashAlgorithm();
        //   DefaultHashAlgorithm(const DefaultHashAlgorithm);
        //   ~DefaultHashAlgorithm();
        //   DefaultHashAlgorithm& operator=(const DefaultHashAlgorithm&);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING C'TORS, D'TOR, AND ASSIGNMENT OPERATOR"
                   "\n==============================================\n");

        if (verbose) printf("Instantiate the algorithm using a default"
                            " constructor. (C-1)\n");
        {
            Obj alg1 = Obj();
        }

        if (verbose) printf("Use the copy-initialization syntax to create a"
                            " new instance of 'DefaultHashAlgorithm' from an"
                            " existing instance. (C-2,3)\n");
        {
            Obj alg1 = Obj();
            Obj alg2 = alg1;
        }

        if (verbose) printf("Assign the value of the one (const) instance of"
                            " 'DefaultHashAlgorithm' to a second. (C-4)\n");
        {
            const Obj alg1 = Obj();
            Obj alg2 = alg1;
        }

        if (verbose) printf("Chain the assignment of the value of the one"
                            " instance of 'DefaultHashAlgorithm' to a second"
                            " instance of 'DefaultHashAlgorithm', into a"
                            " self-assignment of the second object. (C-5)\n");
        {
            Obj alg1 = Obj();
            Obj alg2 = alg1;
            alg2 = alg2 = alg1;
        }

        if (verbose) printf("Create an instance of 'DefaultHashAlgorithm' and"
                            " allow it to leave scope to be destroyed. (C-6)"
                            "\n");
        {
            Obj alg1 = Obj();
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
        //: 1 Create an instance of 'bsl::DefaultHashAlgorithm'. (C-1)
        //:
        //: 2 Verify different hashes are produced for different c-strings.
        //:   (C-1)
        //:
        //: 3 Verify the same hashes are produced for the same c-strings. (C-1)
        //:
        //: 4 Verify different hashes are produced for different 'int's. (C-1)
        //:
        //: 5 Verify the same hashes are produced for the same 'int's. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("Instantiate 'bsl::DefaultHashAlgorithm'\n");
        {
            Obj hashAlg;
        }

        if (verbose) printf("Verify different hashes are produced for"
                            " different c-strings.\n");
        {
            Obj hashAlg1;
            Obj hashAlg2;
            const char * str1 = "Hello World";
            const char * str2 = "Goodbye World";
            hashAlg1(str1, strlen(str1));
            hashAlg2(str2, strlen(str2));
            ASSERT(hashAlg1.computeHash() != hashAlg2.computeHash());
        }

        if (verbose) printf("Verify the same hashes are produced for the same"
                            " c-strings.\n");
        {
            Obj hashAlg1;
            Obj hashAlg2;
            const char * str1 = "Hello World";
            const char * str2 = "Hello World";
            hashAlg1(str1, strlen(str1));
            hashAlg2(str2, strlen(str2));
            ASSERT(hashAlg1.computeHash() == hashAlg2.computeHash());
        }

        if (verbose) printf("Verify different hashes are produced for"
                            " different 'int's.\n");
        {
            Obj hashAlg1;
            Obj hashAlg2;
            int int1 = 123456;
            int int2 = 654321;
            hashAlg1(&int1, sizeof(int));
            hashAlg2(&int2, sizeof(int));
            ASSERT(hashAlg1.computeHash() != hashAlg2.computeHash());
        }

        if (verbose) printf("Verify the same hashes are produced for the same"
                            " 'int's.\n");
        {
            Obj hashAlg1;
            Obj hashAlg2;
            int int1 = 123456;
            int int2 = 123456;
            hashAlg1(&int1, sizeof(int));
            hashAlg2(&int2, sizeof(int));
            ASSERT(hashAlg1.computeHash() == hashAlg2.computeHash());
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
// Copyright (C) 2014 Bloomberg Finance L.P.
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


