// bslh_siphashalgorithm.t.cpp                                        -*-C++-*-
#include <bslh_siphashalgorithm.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_issame.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <wchar.h>
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
// TYPEDEF
// [ 4] typedef bsls::Types::Uint64 result_type;
//
// EXPLICIT CONSTRUCTORS
// [ 2] explicit SipHashAlgorithm(uint64 k0, uint64 k1 = 0);
//
// IMPLICIT CONSTRUCTORS
// [ 2] SipHashAlgorithm(const SipHashAlgorithm);
// [ 2] ~SipHashAlgorithm();
// [ 2] SipHashAlgorithm& operator=(const SipHashAlgorithm&);
//
// MANIPULATOR
// [ 3] void operator()(void const* key, size_t len);
//
// ACCESSOR
// [ 3] result_type computeHash();
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] Trait IsBitwiseMoveable
// [ 6] USAGE EXAMPLE
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
///Example: Creating and Using a Hash Cross Reference
/// - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we have any array of values that define 'operator==', and we want a
// really fast way to find out if values are contained in the array. We can
// create a 'HashTable' data structure that is capable of looking up values in
// O(1) time. The hash table implemented here can have numerous simplifications
// becuse we know the size of the array and never have to resize the table.
//
// Further suppose that we will be storing arbitrary user input in our table.
// It is possible that an attacker with knowledge of the hashing algorithm we
// are using could specially craft input that will cause collisions in our hash
// table, degrading performance to O(n). To avoid this we will need to use a
// secure hash algorithm with a random seed. This algorithm will need to be  in
// the form of a hash functor -- an object that will take objects stored in our
// array as input, and yield a 64-bit int value which is hard enough for an
// outside observer to predict that it appear random. The functor can pass the
// salient attributes of the 'TYPE' into the hashing algorithm, and then return
// the hash that is produced.
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
    // Implements a hash table to provide a fast lookup of an external,
    // non-owned, array of values of configurable type.
    //
    // The only requirement for 'TYPE' is that it have a transitive, symmetric
    // 'operator==' function.  There is no requirement that it have any kind of
    // creator defined.
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

    // DATA
    const TYPE       *d_values;             // Array of values we are
                                            // performing lookups on.  Held,
                                            // not owned.
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
    HashTable(const TYPE       *valuesArray,
              size_t            numValues,
              bslma::Allocator *allocator = 0)
        // Create a hash table refering to the specified 'valuesArray'
        // containing 'numValues'. Optionally specify 'allocator' or the
        // default allocator will be used`.
    : d_values(valuesArray)
    , d_numValues(numValues)
    , d_hasher()
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
        // Free up memory used by this cross-reference.
    {
        d_allocator_p->deallocate(d_bucketArray);
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

// Then, we define a 'Future' class, which holds a cstring 'name', char
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

// Next, we define a function that will give us cryptographically secure random
// numbers that will be required to make our hashing algorithm secure.
unsigned long long getCryptographicallySecureNumber();
    // Return a number that is indistinguishable from random and will not be
    // able to be predicted by an attacker

// Then, we need a hash functor for 'Future'.  We are going to use the
// 'SipHashAlgorithm' becuase, it is a secure hash algorithm that will provide
// a way to securely combine the salient attributes of 'Future' objects into
// one reasonable hash that an malicious user will not be able to predict.

struct HashFuture {
    // A hash functor that will apply the SipHashAlgorithm to objects of type
    // 'Future'.

    size_t operator()(const Future& future) const
        // Return the hash of the of the specified 'future'.  Note that this
        // uses the 'SipHashAlgorithm' to safely combine the salient attributes
        // of 'Future' objects into a hash that is not predictable by an
        // attacker.
    {
        SipHashAlgorithm hash(getCryptographicallySecureNumber(),
                              getCryptographicallySecureNumber());

        hash(future.getName(),  strlen(future.getName())*sizeof(char));
        hash(future.getMonth(), sizeof(char));
        hash(future.getYear(),  sizeof(short));

        return hash.computeHash();
    }
};

//=============================================================================
//                    USAGE EXAMPLE IMPLEMENTATIONS
//-----------------------------------------------------------------------------

unsigned long long getCryptographicallySecureNumber() {
    return 49872534ULL; // NOTE: not a real crypto secure random number
}

//=============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
//-----------------------------------------------------------------------------

typedef SipHashAlgorithm Obj;

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

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The hashing algorithm can be used to create more powerfull
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

// Next, we want to actually use our hash table on 'Future' objects.  We
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

// Then, we create our HashTable 'hashTable' and verify that it constructed
// properly.  We pass the functor that we defined above as the second argument:

        HashTable<Future, HashFuture> hashTable(futures, NUM_FUTURES);
        ASSERT(hashTable.isValid());

// Now, we verify that each element in our array registers with count:
        for( int i = 0; i < 6; ++i) {
            ASSERT(1 == hashTable.count(futures[i]));
        }

// Finally, we verify that futures not in our original array are correctly
// identified as not being in the set:

        ASSERT(0 == hashTable.count(Future("French Franc", 'N', 2019)));
        ASSERT(0 == hashTable.count(Future("Swiss Franc", 'X', 2014)));
        ASSERT(0 == hashTable.count(Future("US Dollar", 'F', 2014)));

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING BDE TYPE TRAITS
        //   The class is bitwise movable and should have a trait that
        //   indicates that.
        //
        // Concerns:
        //: 1 The class is marked as 'IsBitwiseMoveable'.
        //
        // Plan:
        //: 1 ASSERT the presence of the trait using the 'bslalg::HasTrait'
        //:   metafunction. (C-1)
        //
        // Testing:
        //   Trait IsBitwiseMoveable
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDE TYPE TRAITS"
                            "\n=======================\n");

        if (verbose) printf("ASSERT the presence of the trait using the"
                            " 'bslalg::HasTrait' metafunction. (C-1)\n");
        {
            ASSERT(bslmf::IsBitwiseMoveable<SipHashAlgorithm>::value);
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING STANDARD TYPEDEF
        //   Verify that the class offers the result_type typedef that needs to
        //   be exposed by all 'bslh' hashing algorithms
        //
        // Concerns:
        //: 1 The typedef 'result_type' is publicly accessible and an alias for
        //:   'bsls::Types::Uint64'.
        //
        // Plan:
        //: 1 ASSERT the typedef is accessable and is the correct type using
        //:   'bslmf::IsSame'. (C-1)
        //
        // Testing:
        //   typedef bsls::Types::Uint64 result_type;
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STANDARD TYPEDEF"
                            "\n========================\n");

        if (verbose) printf("ASSERT the typedef is accessable and is the"
                            " correct type using 'bslmf::IsSame'. (C-1)\n");
        {
            ASSERT((bslmf::IsSame<bsls::Types::Uint64,
                                       SipHashAlgorithm::result_type>::VALUE));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION CALL OPERATOR AND COMPUTEHASH
        //   Verify that the class offers the ability to invoke it via
        //   'operator()' with some bytes and a length. Verify that calling
        //   'operator()' will permute the algorithm's internal state as
        //   specified by SipHash. Verify that 'computeHash' returns the final
        //   value by SipHash specifications.
        //
        // Concerns:
        //: 1 The function call operator exists.
        //:
        //: 2 Given the same bytes, the function call operator will permute the
        //:   internal state of the algorithm in the same way, regardless of
        //:   whether the bytes are passed in all at once or in pieces.
        //:
        //: 3 'computeHash()' exists and returns the appropriate value
        //:   according to the SipHash specification.
        //:
        //: 4 No memory is allocated from the default or global allocators.
        //
        // Plan:
        //: 1 Install a test allocator as the default allocator.  Then install
        //:   an 'AllocatorGuard' to verify no memory is allocated during the
        //:   execution of this test case.  Memory from the global allocator is
        //:   tested as a global concern. (C-4)
        //:
        //: 2 Insert various lengths of c-strings into the algorithm both all
        //:   at once and char by char using 'operator()'. Assert that the
        //:   algorithm produces the same result in both cases. (C-1,2)
        //:
        //: 3 Check the output of 'computeHash()' against the expected results
        //:   from a known good version of the algorithm. (C-3)
        //:
        //: 4 Verify no memory was used. (C-4)
        //
        // Testing:
        //   void operator()(void const* key, size_t len);
        //   result_type computeHash();
        // --------------------------------------------------------------------

        if (verbose) printf(
                         "\nTESTING FUNCTION CALL OPERATOR AND COMPUTEHASH"
                         "\n==============================================\n");

        static const struct {
            int                  d_line;
            const char           d_value [21];
            bsls::Types::Uint64  d_hash;
        } DATA[] = {
        // LINE DATA               HASH
         {  L_,                     "1",  5003827105613308882ULL,},
         {  L_,                    "12", 15843726386018726000ULL,},
         {  L_,                   "123", 10015141997369857857ULL,},
         {  L_,                  "1234",  5657644571716055698ULL,},
         {  L_,                 "12345",  1506984125008183380ULL,},
         {  L_,                "123456",   840043121118509737ULL,},
         {  L_,               "1234567", 17835334163700308593ULL,},
         {  L_,              "12345678",  6766637294834280856ULL,},
         {  L_,             "123456789",   620596589959453183ULL,},
         {  L_,            "1234567890", 15570569622333497593ULL,},
         {  L_,           "12345678901", 13530909564418392529ULL,},
         {  L_,          "123456789012", 13894509901335244589ULL,},
         {  L_,         "1234567890123",  1226840757467028471ULL,},
         {  L_,        "12345678901234",    50576561989623266ULL,},
         {  L_,       "123456789012345",  7596366566405954022ULL,},
         {  L_,      "1234567890123456", 12358386444182647117ULL,},
         {  L_,     "12345678901234567", 16434479071585321762ULL,},
         {  L_,    "123456789012345678",  1230685661702586319ULL,},
         {  L_,   "1234567890123456789",  8034362604589000460ULL,},
         {  L_,  "12345678901234567890",   544989015912666071ULL,},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;


        if (verbose) printf("Install a test allocator as the default"
                            " allocator.  Then install an 'AllocatorGuard' to"
                            " verify no memory is allocated during the"
                            " execution of this test case.  Memory from the"
                            " global allocator is tested as a global concern."
                            " (C-4)\n");
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);


        if (verbose) printf("Insert various lengths of c-strings into the"
                            " algorithm both all at once and char by char"
                            " using 'operator()'. Assert that the algorithm"
                            " produces the same result in both cases. (C-1,2)"
                            "\n");
        {
            for (int i = 0; i != NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *VALUE = DATA[i].d_value;

                if (veryVerbose) printf("Hashing: %s\n", VALUE);

                Obj contiguousHash = Obj(0);
                Obj dispirateHash  = Obj(0);

                contiguousHash(VALUE, strlen(VALUE));
                for(unsigned int j = 0; j < strlen(VALUE); ++j){
                    if (veryVeryVerbose) printf("Hashing by char: %c\n",
                                                                     VALUE[j]);
                    dispirateHash(&VALUE[j], sizeof(char));
                }

                LOOP_ASSERT(LINE, contiguousHash.computeHash() ==
                                                  dispirateHash.computeHash());
            }
        }

        if (verbose) printf("Check the output of 'computeHash()' against the"
                            " expected results from a known good version of"
                            " the algorithm. (C-3)\n");
        {
            for (int i = 0; i != NUM_DATA; ++i) {
                const int                LINE  = DATA[i].d_line;
                const char              *VALUE = DATA[i].d_value;
                const unsigned long long HASH  = DATA[i].d_hash;

                if (veryVerbose) printf("Hashing: %s\n, Expecting: %llu",
                                        VALUE,
                                        HASH);

                Obj hash = Obj(0);
                hash(VALUE, strlen(VALUE));
                LOOP_ASSERT(LINE, hash.computeHash() == HASH);
            }
        }

        if (verbose) printf("Verify no memory was used\n");
        {
            LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING C'TORS, D'TOR, AND ASSIGNMENT OPERATOR
        //   Ensure that the three implicitly declared and defined special
        //   member functions and the one explicitly defined constructor are
        //   publicly callable and have no unexpected side effects such as
        //   allocating memory.  Verify that the algorithm does not have a
        //   default constructor and can only be instantiated with a seed.
        //
        // Concerns:
        //: 1 Objects can not be created using the default constructor.
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
        //: 2 Assert the algorithm does not have the is_default_constructable
        //:   trait. (C-1)
        //:
        //: 3 Call the parameterized constructor using one and two seeds. (C-2)
        //:
        //: 4 Use the copy-initialization syntax to create a new instance of
        //:   'SipHashAlgorithm' from an existing instance. (C-3,4)
        //:
        //: 5 Assign the value of the one (const) instance of
        //:   'SipHashAlgorithm' to a second. (C-5)
        //:
        //: 6 Chain the assignment of the value of the one instance of
        //:   'SipHashAlgorithm' to a second instance of 'SipHashAlgorithm',
        //:   into a self-assignment of the second object. (C-6)
        //:
        //: 7 Create an instance of 'SipHashAlgorithm' and allow it to leave
        //:   scope to be destroyed. (C-7)
        //:
        //: 8 Verify no memory was used. (C-8)
        //
        // Testing:
        //   explicit SipHashAlgorithm(uint64 k0, uint64 k1 = 0);
        //   SipHashAlgorithm(const SipHashAlgorithm);
        //   ~SipHashAlgorithm();
        //   SipHashAlgorithm& operator=(const SipHashAlgorithm&);
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING C'TORS, D'TOR, AND ASSIGNMENT OPERATOR"
                   "\n==============================================\n");

        if (verbose) printf("Install a test allocator as the default"
                            " allocator.  Then install an 'AllocatorGuard' to"
                            " verify no memory is allocated during the"
                            " execution of this test case.  Memory from the"
                            " global allocator is tested as a global concern."
                            " (C-8)\n");
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);


        if (verbose) printf("Assert the algorithm does not have the"
                            " is_default_constructable trait. (C-1)\n");
        {
            //ASSERT(!std::is_default_constructible<SipHashAlgorithm>::value);  //TODO
        }

        if (verbose) printf("Call the parameterized constructor using one and"
                            " two seeds. (C-2)\n");
        {
            Obj alg1 = Obj(0);
            Obj alg2 = Obj(0, 0);
        }

        if (verbose) printf("Use the copy-initialization syntax to create a"
                            " new instance of 'SipHashAlgorithm' from an"
                            " existing instance. (C-3,4)\n");
        {
            Obj alg1 = Obj(0);
            Obj alg2 = alg1;
        }

        if (verbose) printf("Assign the value of the one (const) instance of"
                            " 'SipHashAlgorithm' to a second. (C-5)\n");
        {
            const Obj alg1 = Obj(0);
            Obj alg2 = alg1;
        }

        if (verbose) printf("Chain the assignment of the value of the one"
                            " instance of 'SipHashAlgorithm' to a second"
                            " instance of 'SipHashAlgorithm', into a"
                            " self-assignment of the second object. (C-6)\n");
        {
            Obj alg1 = Obj(0);
            Obj alg2 = alg1;
            alg2 = alg2 = alg1;
        }

        if (verbose) printf("Create an instance of 'SipHashAlgorithm' and"
                            " allow it to leave scope to be destroyed. (C-7)"
                            "\n");
        {
            Obj alg1 = Obj(0);
        }

        if (verbose) printf("Verify no memory was used\n");
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
        //: 1 Create an instance of 'bslh::SipHashAlgorithm'. (C-1)
        //:
        //: 2 Verify different hashes are produced for different c-strings.
        //:   (C-1)
        //:
        //: 3 Verify the same hashes are produced for the same c-strings. (C-1)
        //:
        //: 4 Verify different hashes are produced for different ints. (C-1)
        //:
        //: 5 Verify the same hashes are produced for the same ints. (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        if (verbose) printf("Instantiate 'bsl::SipHashAlgorithm'\n");
        {
            SipHashAlgorithm hashAlg(1);
        }

        if (verbose) printf("Verify different hashes are produced for"
                            " different c-strings.\n");
        {
            SipHashAlgorithm hashAlg1(1);
            SipHashAlgorithm hashAlg2(1);
            const char * str1 = "Hello World";
            const char * str2 = "Goodbye World";
            hashAlg1(str1, strlen(str1));
            hashAlg2(str2, strlen(str2));
            ASSERT(hashAlg1.computeHash() != hashAlg2.computeHash());
        }

        if (verbose) printf("Verify the same hashes are produced for the same"
                            " c-strings.\n");
        {
            SipHashAlgorithm hashAlg1(1);
            SipHashAlgorithm hashAlg2(1);
            const char * str1 = "Hello World";
            const char * str2 = "Hello World";
            hashAlg1(str1, strlen(str1));
            hashAlg2(str2, strlen(str2));
            ASSERT(hashAlg1.computeHash() == hashAlg2.computeHash());
        }

        if (verbose) printf("Verify different hashes are produced for"
                            " different ints.\n");
        {
            SipHashAlgorithm hashAlg1(1);
            SipHashAlgorithm hashAlg2(1);
            int int1 = 123456;
            int int2 = 654321;
            hashAlg1(&int1, sizeof(int));
            hashAlg2(&int2, sizeof(int));
            ASSERT(hashAlg1.computeHash() != hashAlg2.computeHash());
        }

        if (verbose) printf("Verify the same hashes are produced for the same"
                            " ints.\n");
        {
            SipHashAlgorithm hashAlg1(1);
            SipHashAlgorithm hashAlg2(1);
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

