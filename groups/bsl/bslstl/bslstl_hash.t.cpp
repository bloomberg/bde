// bslstl_hash.t.cpp                                                  -*-C++-*-
#include <bslstl_hash.h>

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bsl;


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// [ 3] operator()(const VALUE_TYPE&) const
// [ 2] hash()
// [ 2] hash(const hash)
// [ 2] ~hash()
// [ 2] hash& operator=(const hash&)
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 7] STRINGTHING
// [ 8] USAGE EXAMPLE
// [ 4] Standard typedefs
// [ 5] Bitwise-movable trait
// [ 5] IsPod trait
// [ 6] QoI: Is an empty type

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

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
// array of 'TYPE' objects.  We will resovle hash collisions in our array
// through 'linear probing', where we will search consequetive buckets
// following the bucket where the collision occurred, testing occupied buckets
// for equality with the value we are searching on, and concluding that the
// value is not in the table if we encounter an empty bucket before we
// encounter one referring to an equal element.
//
// An important quality of the hash function is that if two values are
// equivalent, they must yield the same hash value.
//
// First, we define our 'HashCrossReference' template class, with the two
// type parameters 'TYPE" (the type being referenced' and 'HASHER', which
// defaults to 'bsl::hash<TYPE>'.  For common types of 'TYPE' such as 'int',
// a specialization of 'bsl::hash' is already defined:

template <class TYPE, class HASHER = bsl::hash<TYPE> >
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
        // return its index in 'd_bucketArray'.  If not found, return the
        // vacant entry in 'd_bucketArray' where it should be inserted.  Return
        // 'true' if 'value is found and 'false' otherwise.
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
        // Create a hash cross reference referring to the array of value.
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
        d_bucketArray = (const TYPE **) d_allocator_p->allocate(
                                          bucketArrayLength * sizeof(TYPE **));
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

///Example 2: Using Our Hash Cross Reference For a Custom Class
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// In Example 1, we demonstrated a hash cross reference for integers, a trivial
// example.  In Example 2, we want to demonstrate specializing 'hash' for a
// custom class.  We will re-use the 'HashCrossReference' template class
// defined in Example 1.

// First, we define a 'StringThing' class, which is basically a 'const char *'
// except that 'operator==' will do the right thing on the strings and properly
// compare them:

class StringThing {
    // This class holds a pointer to zero-terminated string.  It is implicitly
    // convertible to and from a 'const char *'.  The difference between this
    // type and a 'const char *' is that 'operator==' will properly compare two
    // objects of this type for equality of strings rather than equality of
    // pointers.

    // DATA
    const char *d_string;    // held, not owned

  public:
    // CREATOR
    StringThing(const char *string)                                 // IMPLICIT
    : d_string(string)
        // Create a 'StringThing' object out of the specified 'string'.
    {}

    // ACCESSOR
    operator const char *() const
        // Implicitly cast this 'StringThing' object to a 'const char *' that
        // refers to the same buffer.
    {
        return d_string;
    }
};

inline
bool operator==(const StringThing& lhs, const StringThing& rhs)
{
    return !strcmp(lhs, rhs);
}

inline
bool operator!=(const StringThing& lhs, const StringThing& rhs)
{
    return !(lhs == rhs);
}

// Then, we need a hash function for 'StringThing'.  We can specialize
// 'bsl::hash' for our 'StringThing' type:

namespace bsl {

template <>
struct hash<StringThing> {
    // We need to specialize 'hash' for our 'StringThing' type.  If we just
    // called 'hash<const char *>', it would just hash the pointer, so that
    // pointers to two different buffers containing the same sequence of chars
    // would hash to different values, which would not be the desired behavior.

    size_t operator()(const StringThing& st) const
        // Return the hash of the zero-terminated sequence of bytes referred to
        // by the specified 'st'.  Note that this is an ad-hoc hash function
        // thrown together in a few minutes, it has not been exhaustively
        // tested or mathematically analyzed.  Also note that even though most
        // of the default specializations of 'hash' have functions that take
        // their arguments by value, there is nothing preventing us from
        // chosing to pass it by reference in this case.
    {
        enum { SHIFT_DOWN = sizeof(size_t) * 8 - 8 };

#ifdef BSLS_PLATFORM__CPU_64_BIT
        const size_t MULTIPLIER = 0x5555555555555555ULL; // 16 '5's
#else
        const size_t MULTIPLIER = 0x55555555;            //  8 '5's
#endif

        size_t ret = 0;
        unsigned char c;
        for (const char *pc = st; (c = *pc); ++pc) {
            ret =  MULTIPLIER * (ret + c);
            ret += ret >> SHIFT_DOWN;
        }

        return ret;
    }
};

}  // close namespace bsl

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
//  bool         veryVerbose = argc > 3;
//  bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 9: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //   Extracted from component header file.
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

        if (verbose) printf("USAGE EXAMPLE 2\n"
                            "===============\n");

// Next, now we want to use our cross reference on a more complex type, so
// we'll use the 'StringThing' type we created.  We create an array of unique
// 'StringThing's and take its length:

        StringThing stringThings[] = { "woof",
                                       "meow",
                                       "bark",
                                       "arf",
                                       "bite",
                                       "chomp",
                                       "gnaw" };
        enum { NUM_STRINGTHINGS =
                              sizeof stringThings / sizeof *stringThings };

// Then, we create our cross-reference 'hcrsts' and verify that it constructed
// properly.  Note we don't pass a second parameter template argument and let
// 'HASHER' will define to 'bsl::hash<StringThing>', which we have defined
// above:

        HashCrossReference<StringThing> hcrsts(stringThings,
                                               NUM_STRINGTHINGS);
        ASSERT(hcrsts.isValid());

// Next, we verify that each element in our array registers with count:

        ASSERT(1 == hcrsts.count("woof"));
        ASSERT(1 == hcrsts.count("meow"));
        ASSERT(1 == hcrsts.count("bark"));
        ASSERT(1 == hcrsts.count("arf"));
        ASSERT(1 == hcrsts.count("bite"));
        ASSERT(1 == hcrsts.count("chomp"));
        ASSERT(1 == hcrsts.count("gnaw"));

// Now, we verify that strings not in our original array are correctly
// identified as not being in the set:

        ASSERT(0 == hcrsts.count("buy"));
        ASSERT(0 == hcrsts.count("beg"));
        ASSERT(0 == hcrsts.count("borrow"));
        ASSERT(0 == hcrsts.count("or"));
        ASSERT(0 == hcrsts.count("steal"));

// Finally, to make sure that our lookup is independent of string location, we
// copy some strings into a buffer and make sure that our results are as
// expected.

        char buffer[10];
        strcpy(buffer, "woof");
        ASSERT(1 == hcrsts.count(buffer));
        strcpy(buffer, "chomp");
        ASSERT(1 == hcrsts.count(buffer));
        strcpy(buffer, "buy");
        ASSERT(0 == hcrsts.count(buffer));
        strcpy(buffer, "steal");
        ASSERT(0 == hcrsts.count(buffer));
      } break;
      case 8: {
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
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE 1\n"
                            "===============\n");

// Then, In 'main', we will first use our cross-reference to cross-reference a
// collection of integer values.  We define our array and take its length:

        const int ints[] = { 23, 42, 47, 56, 57, 61, 62, 63, 70, 72, 79 };
        enum { NUM_INTS = sizeof ints / sizeof *ints };

// Now, we create our cross-reference 'hcri' and verify it constructed
// properly.  Note that we don't specify the second template parameter 'HASHER'
// and let it default to 'bsl::hash<int>', which is already defined by
// bslstl_hash:

        HashCrossReference<int> hcri(ints, NUM_INTS);
        ASSERT(hcri.isValid());

// Finally, we use 'hcri' to verify numbers that were and were not in the
// collection:

        ASSERT(1 == hcri.count(23));
        ASSERT(1 == hcri.count(42));
        ASSERT(1 == hcri.count(47));
        ASSERT(1 == hcri.count(56));
        ASSERT(0 == hcri.count( 3));
        ASSERT(0 == hcri.count(31));
        ASSERT(0 == hcri.count(37));
        ASSERT(0 == hcri.count(58));
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'StringThing'
        //
        // Concern:
        //   That the 'StringThing' type use in the usage example functions
        //   properly.
        // --------------------------------------------------------------------

        const char *a = "woof";
        char b[5];
        strcpy(b, a);
        ASSERT(a != b);

        ASSERT(StringThing(a) == StringThing(b));
        b[0] = '*';
        ASSERT(StringThing(a) != StringThing(b));
        ASSERT(StringThing(a) != StringThing("meow"));
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // QoI: Is an empty type
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

        if (verbose) printf("\nTESTING QoI: Is an empty type"
                            "\n=============================\n");

        typedef int TYPE;

        struct TwoInts {
            int a;
            int b;
        };

        struct DerivedInts : hash<TYPE> {
            int a;
            int b;
        };

        struct IntsWithMember {
            hash<TYPE> dummy;
            int              a;
            int              b;
        };

        ASSERT(8 == sizeof(TwoInts));
        ASSERT(8 == sizeof(DerivedInts));
        ASSERT(8 < sizeof(IntsWithMember));

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // BDE TYPE TRAITS
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
        //   BDE Traits
        // --------------------------------------------------------------------

        if (verbose) printf("\nTESTING BDE TRAITS"
                            "\n==================\n");

        typedef int TYPE;

        ASSERT(bslmf::IsBitwiseMoveable<hash<TYPE> >::value);
        ASSERT(bsl::is_trivially_copyable<hash<TYPE> >::value);
        ASSERT(bsl::is_trivially_default_constructible<hash<TYPE> >::value);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // STANDARD TYPEDEFS
        //   Verify that the class offers the three typedefs required of a
        //   standard adaptable binary function.
        //
        // Concerns:
        //: 1 The typedef 'first_argument_type' is publicly accessible and an
        //:   alias for 'const char '.
        //:
        //: 2 The typedef 'second_argument_type' is publicly accessible and an
        //:   alias for 'const char '.
        //:
        //: 3 The typedef 'result_type' is publicly accessible and an alias for
        //:   'bool'.
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

        typedef int TYPE;

        ASSERT((bslmf::IsSame<size_t, hash<TYPE>::result_type>::VALUE));
        ASSERT((bslmf::IsSame<TYPE, hash<TYPE>::argument_type>::VALUE));

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // FUNCTION CALL OPERATOR
        //   Verify that the class offers the three typedefs required of a
        //   standard adaptable binary function, ().
        //
        // Concerns:
        //: 1 Objects of type 'hash' can be invokes as a binary
        //:   predicate returning 'bool' and taking two 'const char *'
        //:   arguments.
        //:
        //: 2 The function call operator can be invoked on constant objects.
        //:
        //: 3 The function call returns 'true' or 'false' indicating whether
        //:   the two supplied string arguments have the same string value.
        //:
        //: 4 No memory is allocated from the default or global allocators.
        //
        // Plan:
        //: 1
        //: 2
        //: 3
        //
        // Testing:
        //   operator()(const char*, const char *) const
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTION CALL OPERATOR"
                            "\n======================\n");

        if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

        typedef int TYPE;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        static const struct {
            int        d_line;
            TYPE       d_value;
            size_t     d_hashCode;
        } DATA[] = {
            // LINE    VALUE   HASHCODE
            {  L_,     0,      bslalg::HashUtil::computeHash(TYPE( 0)) },
            {  L_,     13,     bslalg::HashUtil::computeHash(TYPE(13)) },
            {  L_,     42,     bslalg::HashUtil::computeHash(TYPE(42)) },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const hash<int> callHash = hash<int>();

        for (int i = 0; i != NUM_DATA; ++i) {
            const int     LINE      = DATA[i].d_line;
            const int     VALUE     = DATA[i].d_value;
            const size_t  HASHCODE  = DATA[i].d_hashCode;

            LOOP_ASSERT(LINE, callHash(VALUE) == HASHCODE);
        }

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // C'TORS, D'TOR AND ASSIGNMENT OPERATOR (IMPLICITLY DEFINED)
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

        if (verbose) printf("\nIMPLICITLY DEFINED OPERATIONS"
                            "\n=============================\n");

        typedef int TYPE;

        if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        if (verbose) printf("Value initialization\n");
        const hash<TYPE> obj1 = hash<TYPE>();


        if (verbose) printf("Copy initialization\n");
        hash<TYPE> obj2 = obj1;

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
        //: 1 Create an object 'compare' using the default ctor.
        //:
        //: 2 Call the 'compare' functor with two 'char' literals in lexical
        //:   order.
        //:
        //: 3 Call the 'compare' functor with two 'char' literals in reverse
        //:   lexical order.
        //:
        //: 4 Call the 'compare' functor with two identical 'char' literals.
        //:
        //: 5 Repeat steps 1-4 for 'hash<const int>' using 'int' literals.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            hash<char> func;
            ASSERT(bslalg::HashUtil::computeHash('A') == func('A'));
            ASSERT(bslalg::HashUtil::computeHash('a') == func('a'));
            ASSERT(bslalg::HashUtil::computeHash('Z') == func('Z'));
        }

        {
            hash<const int> func;
            ASSERT(bslalg::HashUtil::computeHash( 0) == func( 0));
            ASSERT(bslalg::HashUtil::computeHash( 1) == func( 1));
            ASSERT(bslalg::HashUtil::computeHash(42) == func(42));
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
// Copyright (C) 2012 Bloomberg L.P.
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
