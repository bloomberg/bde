// bslh_oneatatimehashalgorithm.t.cpp                                 -*-C++-*-
#include <bslh_oneatatimehashalgorithm.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_issame.h>
#include <bslmf_istriviallycopyable.h>
#include <bslmf_istriviallydefaultconstructible.h>

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
//                             TEST PLAN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 3] operator()(const T&) const
// [ 2] OneAtATimeHashAlgorithm()
// [ 2] OneAtATimeHashAlgorithm(const OneAtATimeHashAlgorithm)
// [ 2] ~OneAtATimeHashAlgorithm()
// [ 2] OneAtATimeHashAlgorithm& operator=(const OneAtATimeHashAlgorithm&)
// [ 4] typedef result_type
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 5] IsBitwiseMoveable
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
// Suppose we already have an array of unique values of type 'TYPE', for which
// 'operator==' is defined, and we want to be able to quickly look up whether
// an element is in the array, without exhaustively applying 'operator==' to
// all the elements in sequence.  The array itself is guaranteed not to change
// for the duration of our interest in it.
//
// The problem is much simpler than building a general-purpose hash table,
// because we know how many elements our cross reference will contain in
// advance, so we will never have to dynamically grow the number of 'buckets'.
// We do not need to copy the values into our own area, so we don't have to
// create storage for them, or require that a copy constructor or destructor be
// available.  We only require that they have a transitive, symmetric
// equivalence operation 'bool operator==' and that a hash function be
// provided.
//
// We will need a hash functor -- an object that uses a hashing algorithm that
// will take as input an object of the type stored in our array, and yield a
// 'size_t' value which will be very randomized. The functor can pass the
// salient attributes of the 'TYPE' into the hashing algorithm, and then return
// the hash that is produced.  Ideally, the slightest change in the value of
// the 'TYPE' object will result in a large change in the value returned by the
// hash function.  In a good hash function, typically half the bits of the
// return value will change for a 1-bit change in the hashed value.  We then
// use the result of the hash function to index into our array of 'buckets'.
// Each 'bucket' is simply a pointer to a value in our original array of 'TYPE'
// objects.  We will resolve hash collisions in our array through 'linear
// probing', where we will search consecutive buckets following the bucket
// where the collision occurred, testing occupied buckets for equality with the
// value we are searching on, and concluding that the value is not in the table
// if we encounter an empty bucket before we encounter one referring to an
// equal element.
//
// An important quality of the hash function is that if two values are
// equivalent, they must yield the same hash value.
//
// First, we define our 'HashCrossReference' template class, with the two type
// parameters: 'TYPE' (the type being referenced) and 'HASHER' (a functor that
// produces the hash).

template <class TYPE, class HASHER>
class HashCrossReference {
    // This table leverages a hash table to provide a fast lookup of an
    // external, non-owned, array of values of configurable type.
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
    const TYPE       *d_values;             // Array of values table is to
                                            // cross-reference.  Held, not
                                            // owned.
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
    HashCrossReference(const TYPE       *valuesArray,
                       size_t            numValues,
                       bslma::Allocator *allocator = 0)
        // Create a hash cross reference refering to the specified
        // 'valuesArray' containing 'numValues'. Optionally specify 'allocator'
        // or the default allocator will be used`.
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

    ~HashCrossReference()
        // Free up memory used by this cross-reference.
    {
        d_allocator_p->deallocate(d_bucketArray);
    }

    // ACCESSORS
    int count(const TYPE& value) const
        // Return 1 if the specified 'value' is found in the cross reference
        // and 0 otherwise.
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

// Then, we need a hash functor for 'Future'.  We are going to use the
// 'OneAtATimeHashAlgorithm' becuase it is a reasonably good hash function for
// hash tables, and it allows us to safely safely combine the salient
// attributes of 'Future' objects into one reasonable hash, withouth having to
// implement a hash combine ourselves.

struct HashFuture {
    // A hash functor that will apply the OneAtATimeHashAlgorithm to objects of
    // type 'Future'.

    size_t operator()(const Future& future) const
        // Return the hash of the of the specified 'future'.  Note that this
        // uses the OneAtATimeHashAlgorithm to safely combine the salient
        // attributes of 'Future' objects into one reasonable hash.
    {
        OneAtATimeHashAlgorithm hash;
        hash(future.getName(), strlen(future.getName())*sizeof(char));
        hash(future.getMonth(), sizeof(char));
        hash(future.getYear(), sizeof(short));

        return hash.getHash();
    }
};


//=============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
//-----------------------------------------------------------------------------

typedef OneAtATimeHashAlgorithm Obj;

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

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   The hashing algorithm can be used to create more powerfull
        //   components such as functors that can be used to power hash cross
        //   references and hash tables.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE\n"
                            "=============\n");

// Next, we want to actually use our cross reference on 'Future' objects.  We
// create an array of unique 'Future's and take its length:

        Future futures[] = { Future("Swiss Franc", 'F', 2014),
                             Future("US Dollar", 'G', 2015),
                             Future("Canadian Dollar", 'Z', 2014),
                             Future("British Pound", 'M', 2015),
                             Future("Deutsche Mark", 'X', 2016),
                             Future("Eurodollar", 'Q', 2017)};
        enum { NUM_FUTURES =
                              sizeof futures / sizeof *futures };

// Then, we create our cross-reference 'hcr' and verify that it constructed
// properly.  We pass the functor that we defined above as the second argument:

        HashCrossReference<Future, HashFuture> hashCrossRef(futures,
                                                            NUM_FUTURES);
        ASSERT(hashCrossRef.isValid());

// Now, we verify that each element in our array registers with count:
        for( int i = 0; i < 6; ++i) {
            ASSERT(1 == hashCrossRef.count(futures[i]));
        }

// Finally, we verify that futures not in our original array are correctly
// identified as not being in the set:

        ASSERT(0 == hashCrossRef.count(Future("French Franc", 'N', 2019)));
        ASSERT(0 == hashCrossRef.count(Future("Swiss Franc", 'X', 2014)));
        ASSERT(0 == hashCrossRef.count(Future("US Dollar", 'F', 2014)));

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING BDE TYPE TRAITS
        //   The functor contains only a single 'size_t' and so should be
        //   bitwise movable.
        //
        // Concerns:
        //: 1 The class is bitwise moveable.
        //
        // Plan:
        //: 1 ASSERT the presence of the trait using the 'bslalg::HasTrait'
        //:   metafunction. (C-1)
        //
        // Testing:
        //   IsBitwiseMoveable
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDE TYPE TRAITS"
                            "\n=======================\n");

        ASSERT(bslmf::IsBitwiseMoveable<OneAtATimeHashAlgorithm>::value);


      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING STANDARD TYPEDEF
        //   Verify that the class offers the result_type typedef that needs to
        //   be exposed by all 'bslh' hashing algorithms
        //
        // Concerns:
        //: 1 The typedef 'result_type' is publicly accessible and an alias for
        //:   'size_t'.
        //
        // Plan:
        //: 1 ASSERT each of the typedef is accessable and is the correct type
        //:   using 'bslmf::IsSame'. (C-1)
        //
        // Testing:
        //   typedef result_type
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STANDARD TYPEDEF"
                            "\n========================\n");

        ASSERT((bslmf::IsSame<size_t,
                              OneAtATimeHashAlgorithm::result_type>::VALUE));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING FUNCTION CALL OPERATOR
        //   Verify that the class offers the ability to invike it with some
        //   bytes and a length, and that it return a hash.
        //
        // Concerns:
        //: 1 The function call operator will return the expected vaulue
        //:   regardless of type
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //:
        //: 3 No memory is allocated from the default or global allocators.
        //
        // Plan:
        //: 1 Cast test values to different types and test the hashes returned.
        //:   (C-1,2)
        //: 2 Use a test allocator to ensure no memory is used
        //
        // Testing:
        //   operator()(const T&) const
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING FUNCTION CALL OPERATOR"
                            "\n==============================\n");

        if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

        typedef signed char TYPE;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        static const struct {
            int        d_line;
            TYPE       d_value;
            size_t     d_oneByteHash;
            size_t     d_twoByteHash;
            size_t     d_fourByteHash;
            size_t     d_eightByteHash;
        } DATA[] = {
            // LINE VALUE ONEBYTEHASH  TWOBYTEHASH FOURBYTEHASH EIGHTBYTEHASH
         {  L_,  0,              0U,           0U,           0U,           0U},
         {  L_,  5,     1534473963U,  3869591463U,  1718633450U,  2828949013U},
         {  L_,  13,    3993033726U,  2148495725U,  3165706846U,  3063670407U},
         {  L_,  42,    4281892453U,  1900720968U,  3861518388U,  1055756050U},
         {  L_,  127,   3941848516U,  4156547249U,    24383023U,  2189952278U},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int         LINE    = DATA[i].d_line;
            const signed char VALUE   = DATA[i].d_value;
            const size_t      HASH [] = { 0,                       // 0 bytes
                                          DATA[i].d_oneByteHash,   // 1 byte
                                          DATA[i].d_twoByteHash,   // 2 bytes
                                          0,                       // 3 bytes
                                          DATA[i].d_fourByteHash,  // 4 bytes
                                          0,                       // 5 bytes
                                          0,                       // 6 bytes
                                          0,                       // 7 bytes
                                          DATA[i].d_eightByteHash};// 8 bytes
            if(veryVerbose) printf("Testing hashes of %i\n", VALUE);

            Obj charHasher;
            char charValue = VALUE;
            charHasher(&charValue, sizeof(char));
            LOOP_ASSERT(LINE, charHasher.getHash() == HASH[sizeof(char)]);

            Obj unsignedcharHasher;
            unsigned char unsignedcharValue = VALUE;
            unsignedcharHasher(&unsignedcharValue, sizeof(unsigned char));
            LOOP_ASSERT(LINE, unsignedcharHasher.getHash() ==
                                                  HASH[sizeof(unsigned char)]);

            Obj signedcharHasher;
            signed char signedcharValue = VALUE;
            signedcharHasher(&signedcharValue, sizeof(signed char));
            LOOP_ASSERT(LINE, signedcharHasher.getHash() ==
                                                    HASH[sizeof(signed char)]);

            Obj wcharHasher;
            wchar_t wcharValue = VALUE;
            wcharHasher(&wcharValue, sizeof(wchar_t));
            LOOP_ASSERT(LINE, wcharHasher.getHash() == HASH[sizeof(wchar_t)]);

            Obj shortHasher;
            short shortValue = VALUE;
            shortHasher(&shortValue, sizeof(short));
            LOOP_ASSERT(LINE, shortHasher.getHash() == HASH[sizeof(short)]);

            Obj unsignedshortHasher;
            unsigned short unsignedshortValue = VALUE;
            unsignedshortHasher(&unsignedshortValue, sizeof(unsigned short));
            LOOP_ASSERT(LINE, unsignedshortHasher.getHash() ==
                                                 HASH[sizeof(unsigned short)]);

            Obj signedshortHasher;
            signed short signedshortValue = VALUE;
            signedshortHasher(&signedshortValue, sizeof(signed short));
            LOOP_ASSERT(LINE, signedshortHasher.getHash() ==
                                                   HASH[sizeof(signed short)]);

            Obj intHasher;
            int intValue = VALUE;
            intHasher(&intValue, sizeof(int));
            LOOP_ASSERT(LINE, intHasher.getHash() == HASH[sizeof(int)]);

            Obj signedintHasher;
            signed int signedintValue = VALUE;
            signedintHasher(&signedintValue, sizeof(signed int));
            LOOP_ASSERT(LINE, signedintHasher.getHash() ==
                                                     HASH[sizeof(signed int)]);

            Obj unsignedintHasher;
            unsigned int unsignedintValue = VALUE;
            unsignedintHasher(&unsignedintValue, sizeof(unsigned int));
            LOOP_ASSERT(LINE, unsignedintHasher.getHash() ==
                                                   HASH[sizeof(unsigned int)]);

            Obj longHasher;
            long longValue = VALUE;
            longHasher(&longValue, sizeof(long));
            LOOP_ASSERT(LINE, longHasher.getHash() == HASH[sizeof(long)]);

            Obj signedlongHasher;
            signed long signedlongValue = VALUE;
            signedlongHasher(&signedlongValue, sizeof(signed long));
            LOOP_ASSERT(LINE, signedlongHasher.getHash() ==
                                                    HASH[sizeof(signed long)]);

            Obj unsignedlongHasher;
            unsigned long unsignedlongValue = VALUE;
            unsignedlongHasher(&unsignedlongValue, sizeof(unsigned long));
            LOOP_ASSERT(LINE, unsignedlongHasher.getHash() ==
                                                  HASH[sizeof(unsigned long)]);

            Obj longlongHasher;
            long long longlongValue = VALUE;
            longlongHasher(&longlongValue, sizeof(long long));
            LOOP_ASSERT(LINE, longlongHasher.getHash() ==
                                                      HASH[sizeof(long long)]);

            Obj signedlonglongHasher;
            signed long long signedlonglongValue = VALUE;
            signedlonglongHasher(&signedlonglongValue,
                                  sizeof(signed long long));
            LOOP_ASSERT(LINE, signedlonglongHasher.getHash() ==
                                               HASH[sizeof(signed long long)]);

            Obj unsignedlonglongHasher;
            unsigned long long unsignedlonglongValue = VALUE;
            unsignedlonglongHasher(&unsignedlonglongValue,
                                    sizeof(unsigned long long));
            LOOP_ASSERT(LINE, unsignedlonglongHasher.getHash() ==
                                             HASH[sizeof(unsigned long long)]);
        }

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        if(veryVerbose) printf("Testing hash for strings");

        // special test for hash<const char *>
        const char STRING_1[] = "Hello World";
        const char STRING_2[] = "Hello World";

        const char *C_STRING_1 = STRING_1;
        const char *C_STRING_2 = STRING_2;
        ASSERT(C_STRING_1 != C_STRING_2);

        Obj string1Hasher;
        string1Hasher(C_STRING_1, strlen(C_STRING_1)*sizeof(char));
        Obj string2Hasher;
        string2Hasher(C_STRING_2, strlen(C_STRING_2)*sizeof(char));

        ASSERT(string1Hasher.getHash() == string2Hasher.getHash());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING C'TORS, D'TOR, AND ASSIGNMENT OPERATOR
        //   Ensure that the three implicitly declared and defined special
        //   member functions and the one explicitly defined constructor are
        //   publicly callable and have no unexpected side effects such as
        //   allocating memory.  Verify that the internal state, as accessable
        //   through the 'getHash()' function, is as expected after
        //   construction.
        //
        // Concerns:
        //: 1 Objects can be created using the default constructor.
        //: 2 Objects can be created using the copy constructor.
        //: 3 The copy constructor is not declared as explicit.
        //: 4 Objects can be assigned to from constant objects.
        //: 5 Assignments operations can be chained.
        //: 6 Objects can be destroyed.
        //: 7 No memory is allocated by the default and global allocators.
        //
        // Plan:
        //: 1 Install a test allocator as the default allocator.  Then install
        //:   an 'AllocatorGuard' to verify no memory is allocated during the
        //:   execution of this test case.  Memory from the global allocator is
        //:   tested as a global concern. (C-7)
        //:
        //: 2 Verify the default constructor exists and is publicly accessible
        //:   by default-constructing a 'const hash'
        //:   object. (C-1)
        //:
        //: 3 Verify the copy constructor is publicly accessible and not
        //:   'explicit' by using the copy-initialization syntax to create a
        //:   second 'hash' from the first. (C-2,3)
        //:
        //: 4 Assign the value of the first ('const') object to the second.
        //:   (C-4)
        //:
        //: 5 Chain the assignment of the value of the first ('const') object
        //:   to the second, into a self-assignment of the second object to
        //:   itself. (C-5)
        //:
        //: 6 Verify the destructor is publicly accessible by allowing the two
        //:   'hash' object to leave scope and be
        //:    destroyed. (C-6)
        //
        // Testing:
        //   OneAtATimeHashAlgorithm()
        //   OneAtATimeHashAlgorithm(const OneAtATimeHashAlgorithm)
        //   ~OneAtATimeHashAlgorithm()
        //   OneAtATimeHashAlgorithm& operator=(const OneAtATimeHashAlgorithm&)
        // --------------------------------------------------------------------

        if (verbose)
            printf("\nTESTING C'TORS, D'TOR, AND ASSIGNMENT OPERATOR"
                   "\n==============================================\n");

        typedef int TYPE;

        if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) printf("Value initialization\n");
        const Obj obj1 = OneAtATimeHashAlgorithm();
        ASSERT(obj1.getHash() == 0);

        if (verbose) printf("Copy initialization\n");
        Obj obj2 = obj1;

        if (verbose) printf("Copy assignment\n");
        obj2 = obj1;
        obj2 = obj2 = obj1;


        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
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
        //: 1 Create an object 'hash' using 'bsl::OneAtATimeHashAlgorithm'.
        //:
        //: 2 Verify for a few char literals that different hashes are
        //:   produced.
        //:
        //: 3 Verify for different char literals that different hashes are
        //:   produced.
        //:
        //: 4 Repeat steps 1-4 with 'int' literals.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            OneAtATimeHashAlgorithm hash1;
            OneAtATimeHashAlgorithm hash2;
            char data = 'A';
            hash1(&data, sizeof(char));
            hash2(&data, sizeof(char));
            ASSERT(hash1.getHash() == hash2.getHash());
            data = 'a';
            hash1(&data, sizeof(char));
            hash2(&data, sizeof(char));
            ASSERT(hash1.getHash() == hash2.getHash());
            data = '\0';
            hash1(&data, sizeof(char));
            hash2(&data, sizeof(char));
            ASSERT(hash1.getHash() == hash2.getHash());
        }

        {
            OneAtATimeHashAlgorithm hash1;
            OneAtATimeHashAlgorithm hash2;
            int data = 0;
            hash1(&data, sizeof(int));
            hash2(&data, sizeof(int));
            ASSERT(hash1.getHash() == hash2.getHash());
            data = 1;
            hash1(&data, sizeof(int));
            hash2(&data, sizeof(int));
            ASSERT(hash1.getHash() == hash2.getHash());
            data = 42;
            hash1(&data, sizeof(int));
            hash2(&data, sizeof(int));
            ASSERT(hash1.getHash() == hash2.getHash());
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

