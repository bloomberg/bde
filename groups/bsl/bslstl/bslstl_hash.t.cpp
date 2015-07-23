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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

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
// [ 7] USAGE EXAMPLE 1
// [ 8] USAGE EXAMPLE 2
// [ 4] typedef argument_type
// [ 4] typedef second_argument_type
// [ 5] IsBitwiseMovable trait
// [ 5] is_trivially_copyable trait
// [ 5] is_trivially_default_constructible trait
// [ 6] QoI: Support for empty base optimization

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
// to 'bsl::hash<TYPE>'.  For common types of 'TYPE' such as 'int', a
// specialization of 'bsl::hash' is already defined:

template <class TYPE, class HASHER = bsl::hash<TYPE> >
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

///Example 2: Using 'hashAppend' from 'bslh' with 'HashCrossReference'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// We want to specialize 'bsl::hash' for a custom class. We can use the the
// modular hashing system implemented in 'bslh' rather than explicitly
// specializing 'bsl::hash'. We will re-use the 'HashCrossReference' template
// class defined in Example 1.
//
// First, we declare 'Point', a class that allows us to identify a loction on a
// two dimensional cartesian plane.
//..

    class Point {
        // This class is a value-semantic type that represents a two
        // dimensional location on a cartesian plane.

      private:
        int    d_x;
        int    d_y;
        double d_distToOrigin; // This value will be accessed a lot, so we
                               // cache it rather than recalculating every
                               // time.

      public:
        Point (int x, int y);
            // Create a 'Point' with the specified 'x' and 'y' coordinates

        double distanceToOrigin();
            // Return the distance from the origin (0, 0) to this point.

//..
// Then, we declare 'operator==' as a friend so that we will be able to compare
// two points.
//..
        friend bool operator==(const Point &left, const Point &right);

//..
// Next, we declare 'hashAppend' as a friend so that we will be able hash a
// 'Point'.
//..
        template <class HASH_ALGORITHM>
        friend
        void hashAppend(HASH_ALGORITHM &hashAlg, const Point &point);
            // Apply the specified 'hashAlg' to the specified 'point'
    };

    Point::Point(int x, int y) : d_x(x), d_y(y) {
        d_distToOrigin = sqrt(static_cast<long double>(d_x * d_x) +
                              static_cast<long double>(d_y * d_y));
    }

    double Point::distanceToOrigin() {
        return d_distToOrigin;
    }

//..
// Then, we define 'operator=='. Notice how it only checks salient attributes -
// attributes that contribute to the value of the class. We ignore
// 'd_distToOrigin' which is not required to determine equality.
//..
    bool operator==(const Point &left, const Point &right)
    {
        return (left.d_x == right.d_x) && (left.d_y == right.d_y);
    }

//..
// Next, we define 'hashAppend'. This method will allow any hashing algorithm
// to be applied to 'Point'. This is the extent of the work that needs to be
// done by type creators. They do not need to implement any algorithms, they
// just need to call out the salient attributes (which have already been
// determined by 'operator==') by calling 'hashAppend' on them.
//..
    template <class HASH_ALGORITHM>
    void hashAppend(HASH_ALGORITHM &hashAlg, const Point &point)
    {
        using ::BloombergLP::bslh::hashAppend;
        hashAppend(hashAlg, point.d_x);
        hashAppend(hashAlg, point.d_y);
    }

//..
// Then, we declare another value-semantic type, 'Box' that will have point as
// one of its salient attributes.
//..
    class Box {
        // This class is a value-semantic type that represents a box drawn on
        // to a cartesian plane.

      private:
        Point d_position;
        int d_length;
        int d_width;

      public:
        Box(Point position, int length, int width);
            // Create a box with the specified 'length' and 'width', with its
            // upper left corner at the specified 'position'

//..
// Next, we declare 'operator==' and 'hashAppend' as we did before.
//..
        friend bool operator==(const Box &left, const Box &right);

        template <class HASH_ALGORITHM>
        friend
        void hashAppend(HASH_ALGORITHM &hashAlg, const Box &box);
            // Apply the specified 'hashAlg' to the specified 'box'
    };

    Box::Box(Point position, int length, int width) : d_position(position),
                                                      d_length(length),
                                                      d_width(width) { }

//..
// Then, we define 'operator=='. This time all of the data members contribute
// to equality.
//..
    bool operator==(const Box &left, const Box &right)
    {
        return (left.d_position == right.d_position) &&
               (left.d_length   == right.d_length) &&
               (left.d_width    == right.d_width);
    }

//..
// Next, we define 'hashAppend' for 'Box'. Notice how as well as calling
// 'hashAppend' on fundamental types, we can also call it on our user defined
// type 'Point'. Calling 'hashAppend' on 'Point' will propogate the hashing
// algorithm functor 'hashAlg' down to the fundamental types that make up
// 'Point', and those types will then be passed into the algorithm functor.
//..
    template <class HASH_ALGORITHM>
    void hashAppend(HASH_ALGORITHM &hashAlg, const Box &box)
    {
        hashAppend(hashAlg, box.d_position);
        hashAppend(hashAlg, box.d_length);
        hashAppend(hashAlg, box.d_width);
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
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE 2
        //   Implement hashing in the new 'bslh' hashing system test that
        //   'bls::hash' picks up on it when there isn't a 'bsl::hash' template
        //   specialization.
        //
        // Concerns:
        //: 1 The usage example compiles, links, and runs as shown.
        //:
        //: 2 'bsl::hash' picks up on and used the implemented 'hashAppend'
        //
        // Plan:
        //: 1 Incorporate usage example into test driver and verify
        //:   functionality with some sample values. (C-1,2)
        //
        // Testing:
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) printf("USAGE EXAMPLE 2\n"
                            "===============\n");
//..
// Then, we want to use our cross reference on a 'Box'.  We create an array of
// unique 'Box's and take its length:
//..

        Box boxes[] = { Box(Point(0, 0), 2, 3),
                        Box(Point(1, 0), 1, 1),
                        Box(Point(0, 1), 1, 5),
                        Box(Point(1, 1), 5, 6),
                        Box(Point(2, 1), 1, 13),
                        Box(Point(0, 4), 3, 3),
                        Box(Point(3, 2), 2, 17) };
        enum { NUM_BOXES = sizeof boxes / sizeof *boxes };

//..
// Next, we create our cross-reference 'hcrsts' and verify that it constructed
// properly.  Note we don't pass a second parameter template argument and let
// 'HASHER' default to 'bsl::hash<TYPE>'. Since we have not specialized
// 'bsl::hash' for 'Box', 'bsl::hash<TYPE>' will attempt to use 'bslh::hash<>'
// to hash 'Box'.
//..

        HashCrossReference<Box> hcrsts(boxes, NUM_BOXES);
        ASSERT(hcrsts.isValid());

//..
// Now, we verify that each element in our array registers with count:
//..
        for(int i = 0; i < NUM_BOXES; ++i) {
            ASSERT(1 == hcrsts.count(boxes[i]));
        }

//..
// Finally, we verify that elements not in our original array are correctly
// identified as not being in the set:
//..
        ASSERT(0 == hcrsts.count(Box(Point(3, 3), 3, 3)));
        ASSERT(0 == hcrsts.count(Box(Point(3, 2), 1, 0)));
        ASSERT(0 == hcrsts.count(Box(Point(1, 2), 3, 4)));
        ASSERT(0 == hcrsts.count(Box(Point(33, 23), 13, 3)));
        ASSERT(0 == hcrsts.count(Box(Point(30, 37), 34, 13)));

      } break;
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

        typedef int TYPE;

        ASSERT(bslmf::IsBitwiseMoveable<hash<TYPE> >::value);
        ASSERT(bsl::is_trivially_copyable<hash<TYPE> >::value);
        ASSERT(bsl::is_trivially_default_constructible<hash<TYPE> >::value);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING STANDARD TYPEDEFS
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
        //   operator()(const VALUE_TYPE&) const
        // --------------------------------------------------------------------

        if (verbose) printf("\nFUNCTION CALL OPERATOR"
                            "\n======================\n");

        if (verbose) printf(
                 "\nCreate a test allocator and install it as the default.\n");

        typedef signed char TYPE;

        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        static const struct {
            int        d_line;
            TYPE       d_value;
            size_t     d_hashCode;
        } DATA[] = {
            // LINE   VALUE  HASHCODE
            {  L_,       0,     0 },
            {  L_,       5,     5 },
            {  L_,      13,    13 },
            {  L_,      42,    42 },
            {  L_,     127,   127 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i != NUM_DATA; ++i) {
            const int         LINE      = DATA[i].d_line;
            const signed char VALUE     = DATA[i].d_value;
            const size_t      HASHCODE  = DATA[i].d_hashCode;

            if(veryVerbose) {
                printf("Testing that %c hashes to %u", VALUE, HASHCODE);
            }

            LOOP_ASSERT(LINE, bsl::hash<char>()(VALUE) == HASHCODE);
            LOOP_ASSERT(LINE, bsl::hash<unsigned char>()(VALUE) == HASHCODE);
            LOOP_ASSERT(LINE, bsl::hash<signed char>()(VALUE) == HASHCODE);

            LOOP_ASSERT(LINE, bsl::hash<wchar_t>()(VALUE) == HASHCODE);

            LOOP_ASSERT(LINE, bsl::hash<unsigned short>()(VALUE) == HASHCODE);
            LOOP_ASSERT(LINE, bsl::hash<signed short>()(VALUE) == HASHCODE);

            LOOP_ASSERT(LINE, bsl::hash<unsigned int>()(VALUE) == HASHCODE);
            LOOP_ASSERT(LINE, bsl::hash<signed int>()(VALUE) == HASHCODE);

            LOOP_ASSERT(LINE, bsl::hash<unsigned long>()(VALUE) == HASHCODE);
            LOOP_ASSERT(LINE, bsl::hash<signed long>()(VALUE) == HASHCODE);

            if (sizeof (unsigned long long) <= sizeof (std::size_t))
            {
                LOOP_ASSERT(LINE, bsl::hash<unsigned long long>()(VALUE)
                        == (unsigned long long) HASHCODE);
                LOOP_ASSERT(LINE, bsl::hash<signed long long>()(VALUE)
                        == (signed long long) HASHCODE);
            }
            else
            {
                LOOP_ASSERT(LINE, bsl::hash<unsigned long long>()(VALUE)
                        == ((std::size_t)
                                    ((unsigned long long) HASHCODE) ^
                                   (((unsigned long long) HASHCODE) >> 32)));
                LOOP_ASSERT(LINE, bsl::hash<signed long long>()(VALUE)
                        == ((std::size_t)
                                    ((signed long long) HASHCODE) ^
                                   (((signed long long) HASHCODE) >> 32)));
            }
        }

        LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        // special test for hash<const char *>
        const char STRING_1[] = "Hello World";
        const char STRING_2[] = "Hello World";

        const char *C_STRING_1 = STRING_1;
        const char *C_STRING_2 = STRING_2;
        ASSERT(C_STRING_1 != C_STRING_2);


        const ::bsl::hash<const char *> C_STRING_HASH =
                                                   ::bsl::hash<const char *>();

        ASSERT(C_STRING_HASH(C_STRING_1) != C_STRING_HASH(C_STRING_2));

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
            ASSERT('A' == func('A'));
            ASSERT('a' == func('a'));
            ASSERT('Z' == func('Z'));
        }

        {
            hash<const int> func;
            ASSERT( 0 == func( 0));
            ASSERT( 1 == func( 1));
            ASSERT(42 == func(42));
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
