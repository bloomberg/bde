// bslh_filesystem.t.cpp                                              -*-C++-*-
#include <bslh_filesystem.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>
#include <bsls_platform.h>

#include <stdio.h>

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// [ 3] operator()(const std::filesystem::path&) const
// [ 2] hash()
// [ 2] hash(const hash)
// [ 2] ~hash()
// [ 2] hash& operator=(const hash&)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] USAGE EXAMPLE 1
// [ 3] operator()(const std::filesystem::path&) const
// [ 4] typedef argument_type
// [ 4] typedef result_type
// [ 5] IsBitwiseMovable trait
// [ 5] is_trivially_copyable trait
// [ 5] is_trivially_default_constructible trait
// [ 6] QoI: Support for empty base optimization

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

// ============================================================================
//                  PRINTF FORMAT MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ZU BSLS_BSLTESTUTIL_FORMAT_ZU

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM

using namespace BloombergLP;
using namespace bsl;

#ifdef BSLS_PLATFORM_CMP_CLANG
#pragma clang diagnostic ignored "-Wunknown-warning-option"
#endif // BSLS_PLATFORM_CMP_CLANG

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended usage of this component.
//
///Example 1: Creating and Using a Hash Cross Reference
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
// We will need a hash function -- the hash function is a function that will
// take as input an object of the type stored in our array, and yield a
// 'size_t' value which will be very randomized.  Ideally, the slightest change
// in the value of the 'TYPE' object will result in a large change in the value
// returned by the hash function.  In a good hash function, typically half the
// bits of the return value will change for a 1-bit change in the hashed value.
// We then use the result of the hash function to index into our array of
// 'buckets'.  Each 'bucket' is simply a pointer to a value in our original
// array of 'TYPE' objects.  We will resolve hash collisions in our array
// through 'linear probing', where we will search consecutive buckets following
// the bucket where the collision occurred, testing occupied buckets for
// equality with the value we are searching on, and concluding that the value
// is not in the table if we encounter an empty bucket before we encounter one
// referring to an equal element.
//
// An important quality of the hash function is that if two values are
// equivalent, they must yield the same hash value.
//
// First, we define our 'HashCrossReference' template class, with the two type
// parameters 'TYPE" (the type being referenced' and 'HASHER', which defaults
// to 'bslh::Hash<TYPE>'.  This component provides the specialization of
// 'bslh::Hash' for 'std::filesystem::path':

template <class TYPE, class HASHER = bslh::Hash<TYPE> >
class HashCrossReference {
    // This class template implements a hash table providing fast lookup of an
    // external, non-owned, array of values of configurable type.
    //
    // The only requirement for 'TYPE' is that it have a transitive, symmetric
    // 'operator==' function.  There is no requirement that it have any kind of
    // creator defined.
    //
    // The 'HASHER' template parameter type must be a functor with a function
    // of the following signature:
    //..
    //  size_t operator()(const TYPE)  const; or
    //  size_t operator()(const TYPE&) const; or
    //..
    // and 'HASHER' must have a publicly available default constructor and
    // destructor.

    // DATA
    const TYPE       *d_values;             // Array of values table is to
                                            // cross-reference.  Held, not
                                            // owned.
    size_t            d_numValues;          // Length of 'd_values'.
    const TYPE      **d_bucketArray;        // Contains ptrs into 'd_values'
    size_t            d_bucketArrayMask;    // Will always be '2^N - 1'.
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
    HashCrossReference(const TYPE       *valuesArray,
                       size_t            numValues,
                       bslma::Allocator *allocator = 0)
        // Create a hash table refering to the specified 'valuesArray'
        // containing 'numValues'. Optionally specify 'allocator' or the
        // default allocator will be used.
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
                                i, (unsigned) (d_bucketArray[idx] - d_values));
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

    (void)verbose;
    (void)veryVerbose;
    (void)veryVeryVerbose;
    (void)veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 7: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 1
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERTV'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE 1\n"
                            "===============\n");
//..
// Then, In 'main', we will first use our cross-reference to cross-reference a
// collection of 'std::filesystem::path' values.  Note that the '/' separator
// is equally valid on Windows and Unix-derived systems when used
// programmatically.  We define our array and take its length:
//..
    const std::filesystem::path paths[] = { "/2/3",
                                            "/4/2",
                                            "/4/7",
                                            "/5/6",
                                            "/5/7",
                                            "/6/1",
                                            "/6/2",
                                            "/6/3",
                                            "/7/0",
                                            "/7/2",
                                            "/7/9"
                                         };
    enum { NUM_PATHS = sizeof paths / sizeof *paths };
//..
// Now, we create our cross-reference 'hcri' and verify it constructed
// properly.  Note that we don't specify the second template parameter 'HASHER'
// and let it default to 'bslh::Hash<std::filesystem::path>', which is already
// defined by this component:
//..
    HashCrossReference<std::filesystem::path> hcri(paths, NUM_PATHS);
    ASSERT(hcri.isValid());
//..
// Finally, we use 'hcri' to verify numbers that were and were not in the
// collection:
//..
    ASSERTV(hcri.count("/2/3"), 1 == hcri.count("/2/3"));
    ASSERTV(hcri.count("/4/2"), 1 == hcri.count("/4/2"));
    ASSERTV(hcri.count("/4/7"), 1 == hcri.count("/4/7"));
    ASSERTV(hcri.count("/5/6"), 1 == hcri.count("/5/6"));
    ASSERTV(hcri.count("/a/3"), 0 == hcri.count("/a/3"));
    ASSERTV(hcri.count("/3/1"), 0 == hcri.count("/3/1"));
    ASSERTV(hcri.count("/3/7"), 0 == hcri.count("/3/7"));
    ASSERTV(hcri.count("/5/8"), 0 == hcri.count("/5/8"));
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING QOI: IS AN EMPTY TYPE
        //   As a quality of implementation issue, the class has no state and
        //   should support the use of the empty base class optimization on
        //   compilers that support it.
        //
        // Concerns:
        //: 1 class 'hash' does not increase the size of an
        //:   object when used as a base class.
        //
        // Plan:
        //: 1 Define a non-empty class with no padding, 'TwoInts'.
        //:
        //: 2 Assert that 'TwoInts has the expected size of 8 bytes.
        //:
        //: 3 Create a class, 'DerivedInts', with identical structure to
        //:   'TwoInts' but derived from 'hash'.
        //:
        //: 4 Assert that both classes have the same size.
        //:
        //: 5 Create a class, 'IntsWithMember', with identical structure to
        //:   'TwoInts' and an 'hash' additional data member.
        //:
        //: 6 Assert that 'IntsWithMember' is larger than the other two
        //:   classes.
        //
        // Testing:
        //   QoI: Support for empty base optimization
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING QOI: IS AN EMPTY TYPE"
                            "\n=============================\n");

        typedef std::filesystem::path TYPE;

        struct TwoInts {
            int a;
            int b;
        };

        struct DerivedInts : bslh::Hash<TYPE> {
            int a;
            int b;
        };

        struct IntsWithMember {
            bslh::Hash<TYPE> dummy;
            int              a;
            int              b;
        };

        ASSERT(8 == sizeof(TwoInts));
        ASSERT(8 == sizeof(DerivedInts));
        ASSERT(8 < sizeof(IntsWithMember));

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING BDE TYPE TRAITS
        //   The functor is an empty POD, and should have the appropriate BDE
        //   type traits to reflect this.
        //
        // Concerns:
        //: 1 The class is bitwise copyable.
        //: 2 The class is bitwise moveable.
        //: 3 The class has the trivial default constructor trait.
        //
        // Plan:
        //: 1 ASSERT the presence of each trait using the 'bslalg::HasTrait'
        //:   metafunction. (C-1..3)
        //
        // Testing:
        //   IsBitwiseMovable trait
        //   is_trivially_copyable trait
        //   is_trivially_default_constructible trait
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDE TYPE TRAITS"
                            "\n=======================\n");

        typedef std::filesystem::path TYPE;

        ASSERT(bslmf::IsBitwiseMoveable<bslh::Hash<TYPE> >::value);
        ASSERT(bsl::is_trivially_copyable<bslh::Hash<TYPE> >::value);
        ASSERT(
            bsl::is_trivially_default_constructible<bslh::Hash<TYPE> >::value);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING STANDARD TYPEDEFS
        //   Verify that the class offers the three typedefs required of a
        //   standard adaptable binary function.
        //
        // Concerns:
        //: 1 The typedef 'argument_type' is publicly accessible and an
        //:   alias for 'std::filesystem::path'.
        //:
        //: 2 The typedef 'result_type' is publicly accessible and an alias for
        //:   'std::size_t'.
        //
        // Plan:
        //: 1 ASSERT each of the typedefs has accessibly aliases the correct
        //:   type using 'bslmf::IsSame'. (C-1..3)
        //
        // Testing:
        //   typedef argument_type
        //   typedef second_argument_type
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING STANDARD TYPEDEFS"
                            "\n=========================\n");

        typedef std::filesystem::path TYPE;

        ASSERT((bslmf::IsSame<size_t, bslh::Hash<TYPE>::result_type>::VALUE));
        ASSERT((bslmf::IsSame<TYPE, bslh::Hash<TYPE>::argument_type>::VALUE));
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FUNCTION CALL OPERATOR
        //   Verify that the class offers the two typedefs required of a
        //   standard adaptable unary function, ().
        //
        // Concerns:
        //: 1 Objects of type 'hash' can be invoked as a unary predicate
        //:   returning 'std::size_t' and taking a 'std::filesystem::path'
        //:   argument.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //:
        //: 3 The function returns identical values for equal (but not
        //:   necessarily for 'std::filesystem::equivalent'-but-not-equal)
        //:   'path' objects.
        //:
        //: 4 The function is very likely to return different values for
        //:   non-equal 'path' objects.
        //:
        //: 5 No memory is allocated from the default or global allocators.
        //
        // Plan:
        //: 1 Programmatically generate many distinct 'path', generating each
        //:   in multiple '==' variations (e.g., with different numbers of
        //:   separators between equal path elements).
        //:
        //: 2 Make all 'hash' invocations on 'const &path' arguments.
        //:
        //: 3 Ensure that '==' 'path' objects result in equal hash values.
        //:
        //: 4 Ensure that '!=' 'path' objects are not likely to result in equal
        //:   hash values.
        //:
        //: 5 Use a 'DefaultAllocatorGuard' to make sure no memory is allocated
        //:   from the default allocator.  A 'main'-scope 'globalAllocator'
        //:   enusures that no memory comes from the global allocator either.
        //
        // Testing:
        //   operator()(const std::filesystem::path&) const
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTION CALL OPERATOR"
                            "\n======================\n");

        if (veryVerbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

        typedef std::filesystem::path TYPE;
        typedef TYPE::value_type      VALUE_TYPE;
        typedef TYPE::string_type     STRING_TYPE;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);


        // Portable POSIX character list from
        //     https://timsong-cpp.github.io/cppwp/n4659/fs.class.path
        // in 30.10.27.1 p 1.
        const VALUE_TYPE PORTABLE_POSIX_CHARACTERS[] = {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
            'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '_', '-'
        };
        const int NUM_PORTABLE_POSIX_CHARACTERS =
                                             sizeof PORTABLE_POSIX_CHARACTERS /
                                             sizeof *PORTABLE_POSIX_CHARACTERS;

        const int PRIME = 100003; // Arbitrarily large prime to be used in
                                  // hash-table like testing

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wlarger-than="
#endif

        unsigned char collisions[PRIME] = {};

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic pop
#endif

        STRING_TYPE pathString;           // path with single separators
        STRING_TYPE pathStringDoubleSep;  // path with doubled separators

        STRING_TYPE separator = STRING_TYPE(1, TYPE::preferred_separator);
        STRING_TYPE doubleSeparator = separator + separator;

        bslh::Hash<TYPE> hashFunctor;
        size_t           currentHash = 0;
        size_t           prevHash    = 0;

        const TYPE t;
        // C-1, C-2
        auto resultType = hashFunctor(t);
        std::size_t size_t;
        ASSERT(typeid(size_t) == typeid(resultType));

        for (int i = 0; i < PRIME; ++i) {
            pathString.clear();
            pathStringDoubleSep.clear();

            int depth = 1 + (i / NUM_PORTABLE_POSIX_CHARACTERS);

            for (int j = 0; j < depth; ++j) {
                pathString += separator;
                pathStringDoubleSep += doubleSeparator;

                pathString +=
                    PORTABLE_POSIX_CHARACTERS[i %
                                              NUM_PORTABLE_POSIX_CHARACTERS];
                pathStringDoubleSep +=
                    PORTABLE_POSIX_CHARACTERS[i %
                                              NUM_PORTABLE_POSIX_CHARACTERS];
            }

            TYPE path(pathString);
            TYPE pathDoubleSep(pathStringDoubleSep);

            const TYPE &pathRef = path;
            const TYPE &pathDoubleSepRef = pathDoubleSep;

            prevHash    = currentHash;
            currentHash = hashFunctor(pathRef);

            // Check consecutive values are not hashing to the same hash
            ASSERT(currentHash != prevHash);

            // C-3
            ASSERT(currentHash == hashFunctor(pathDoubleSepRef));

            // C-4: Check that minimal collisions are happening
            ++collisions[currentHash % PRIME];
            // Choose 11 as max number of collisions.
            ASSERTV(currentHash,
                    currentHash % PRIME,
                    collisions[currentHash % PRIME],
                    collisions[currentHash % PRIME] <= 11);

            TYPE pathCopy = pathRef;

            // Verify same hash is produced for the same value
            ASSERT(pathRef == pathCopy);
            ASSERT(currentHash == hashFunctor(pathCopy));
        }

        // C-5
        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING IMPLICITLY DEFINED OPERATIONS
        //   Ensure that the four implicitly declared and defined special
        //   member functions are publicly callable and have no unexpected side
        //   effects such as allocating memory.  As there is no observable
        //   state to inspect, there is little to verify other than that the
        //   expected expressions all compile, and
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
        //   hash()
        //   hash(const hash)
        //   ~hash()
        //   hash& operator=(const hash&)
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING IMPLICITLY DEFINED OPERATIONS"
                            "\n=====================================\n");

        typedef std::filesystem::path TYPE;

        if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) printf("Value initialization\n");
        const bslh::Hash<TYPE> obj1 = bslh::Hash<TYPE>();


        if (verbose) printf("Copy initialization\n");
        bslh::Hash<TYPE> obj2 = obj1;

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
        //: 1 Create an object 'hashFunctor' using the default ctor.
        //:
        //: 2 Call the 'hashFunctor' functor with two distinct 'path' literals
        //:   and ensure the results are different.
        //:
        //: 3 Call the 'hashFunctor' functor with two equivalent 'path'
        //:   literals and ensure the results are the same.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslh::Hash<std::filesystem::path> hashFunctor;
        std::filesystem::path             p1("a/b/c");
        std::filesystem::path             p1equivalent("a///b//c");
        std::filesystem::path             p2("d/e/f");

        ASSERT(hashFunctor(p1) != hashFunctor(p2));
        ASSERT(hashFunctor(p1) == hashFunctor(p1equivalent));
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
#else  // BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM
// BDE_VERIFY pragma: -TP11
int main()
{
    return -1;
}
#endif // BSLS_LIBRARYFEATURES_HAS_CPP17_FILESYSTEM


// ----------------------------------------------------------------------------
// Copyright 2022 Bloomberg Finance L.P.
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
