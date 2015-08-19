// bslma_autodeallocator.t.cpp                                        -*-C++-*-

#include <bslma_autodeallocator.h>

#include <bslma_allocator.h>                // for testing only
#include <bslma_deallocatorproctor.h>
#include <bslma_testallocator.h>            // for testing only
#include <bslma_testallocatorexception.h>   // for testing only

#include <bsls_bsltestutil.h>

#include <stddef.h>     // ptrdiff_t
#include <stdio.h>      // 'printf'
#include <stdlib.h>     // 'atoi'
#include <string.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// A 'bslma::AutoDeallocator' (a kind of "range proctor") is a mechanism (i.e.,
// having state but no value) that is used to conditionally deallocate a
// contiguous sequence of memory blocks.  We are mostly concerned that this
// proctor deallocates the proper sequence of contiguous memory blocks (in
// unspecified order) according to the specified 'origin' and 'length'
// supplied either at construction or through 'reset' and 'setLength'.
//
// As it happens, there are no basic accessors that can indicate whether the
// origin had been set to the expected value.  Hence, to verify that the
// proctor object is brought to the proper state, we can observe (indirectly)
// that the range of memory blocks proctored by this mechanism are properly
// deallocated.  To be able to observe the deallocation of proctored objects,
// we first have to create a test allocator class that can track each
// individual allocation.  As each memory block is deallocated, the test
// allocator decrements the count of currently allocated memory blocks, and
// marks the deallocated memory blocks as deallocated.
//
// With the test allocator defined, we can then proceed to address the proctor
// mechanism under test.  We will construct proctors to manage varying
// sequences (differing in 'origin' and 'length') of memory blocks allocated by
// the test allcoator.  Rather than using the primary manipulators ('reset' and
// 'setLength'), we will use the "state" constructor to bring the
// 'bslma::AutoDeallocator' directly to any desired state.  After each proctor
// is destroyed, we will verify that memory supplied by the test allocator are
// all deallocated.
//-----------------------------------------------------------------------------
// [3] bslma::AutoDeallocator<ALLOCATOR>(origin, allocator, length = 0);
// [3] ~bslma::AutoDeallocator<ALLOCATOR>();
// [4] void release();
// [5] void reset(origin);
// [6] void setLength(length);
// [6] int length();
// [7] void operator++();
// [7] void operator--();
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] HELPER CLASS: 'TestAllocator'
// [3] CONCERN: the 'deallocate' method for pools is also invoked
// [8] USAGE EXAMPLE
//=============================================================================

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int globalVeryVeryVeryVerbose;

//=============================================================================
//                   HELPER CLASSES AND FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

namespace {

int intMax(int x, int y)
{
    return x > y ? x : y;
}

class TestAllocator {
  public:
    enum { NUM_SEGMENTS = 100 };

  private:
    int         d_numOutstandingAllocations;
    static bool s_segmentAllocated[];

  public:
    // CREATORS
    TestAllocator();
        // Create this object.

    ~TestAllocator();
        // Destroy this object.

    // MANIPULATORS
    void *makePointer();
        // Create a pointer.  Note that the caller is not to write on the
        // memory referred to by this pointer.

    void deallocate(void *address);
        // Deallocate a pointer created by 'makePointer'.  Detect redundant
        // deallocations and deallocations of pointers not created by
        // 'makePointer'.

    // ACCESSORS
    int numOutstandingAllocations() const;
};

bool TestAllocator::s_segmentAllocated[NUM_SEGMENTS];

TestAllocator::TestAllocator()
: d_numOutstandingAllocations(0) {}

TestAllocator::~TestAllocator() {
    ASSERT(0 == d_numOutstandingAllocations);
}

void *TestAllocator::makePointer()
{
    ++d_numOutstandingAllocations;

    for (int i = 0; i < NUM_SEGMENTS; ++i) {
        if (!s_segmentAllocated[i]) {
            if (globalVeryVeryVeryVerbose) {
                printf("Allocating: %d\n", i);
            }
            s_segmentAllocated[i] = true;
            return s_segmentAllocated + i;                            // RETURN
        }
    }

    ASSERT(0 && "Exhausted segments");

    return 0;
}

void TestAllocator::deallocate(void *address)
{
    bool *segment = (bool *) address;
    if (globalVeryVeryVeryVerbose) {
        long long int printVal = (segment - s_segmentAllocated);
        printf("Deallocating: %lld\n", printVal);
    }
    if (segment >= s_segmentAllocated &&
                     segment < s_segmentAllocated + NUM_SEGMENTS && *segment) {
        *segment = false;

        --d_numOutstandingAllocations;
    }
    else {
        LOOP_ASSERT(segment - s_segmentAllocated,
                    0 && "Unallocated segment freed");
    }
}

int TestAllocator::numOutstandingAllocations() const
{
    return d_numOutstandingAllocations;
}

}  // close unnamed namespace

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
// The 'bslma::AutoDeallocator' proctor object can be used to achieve
// *exception* *safety* in an *exception* *neutral* way during manipulation of
// "out-of-place" arrays of raw resources or memory.  Since there are no
// destructor calls, this component is more efficient compared to the
// 'bslma::AutoRawDeleter'.  The following illustrates the insertion operation
// for an "out-of-place" array of raw character sequences.  Assume that an
// array initially contains 5 character sequences as its elements:
//..
//     0     1     2     3     4
//   _____ _____ _____ _____ _____
//  |  o  |  o  |  o  |  o  |  o  |
//  `==|==^==|==^==|==^==|==^==|=='
//     |    _V___  |   __V___  |
//     |   |"Bye"| |  |"berg"| |
//     |   `=====' |  `======' |
//    _V_____     _V_____     _V__
//   |"Hello"|   |"Bloom"|   |"LP"|
//   `======='   `======='   `===='
//..
// To insert two more character sequences at index position 2, the array is
// first reallocated if it is not big enough, and then the existing elements
// at index position 2 - 4 are shifted:
//..
//     0     1     2     3     4     5     6
//   _____ _____ _____ _____ _____ _____ _____
//  |  o  |  o  |xxxxx|xxxxx|  o  |  o  |  o  |
//  `==|==^==|==^=====^=====^==|==^==|==^==|=='
//     |    _V___              |   __V___  |
//     |   |"Bye"|             |  |"berg"| |
//     |   `====='             |  `======' |
//    _V_____                 _V_____     _V__
//   |"Hello"|               |"Bloom"|   |"LP"|
//   `======='               `======='   `===='
//
//  Note: "xxxxx" denotes undefined value.
//..
// Next, two new memory blocks must be allocated to position 2 and 3.  If,
// one of the two allocations fails and an exception is thrown, the array will
// be left in an invalid state because the addresses contained at index
// positions 2 and 3 may be duplicates of those at index positions 4 and 5, or,
// if a resize occurred, invalid altogether.  We can restore exception
// neutrality by setting the array's length to 2 before attempting to create
// the string objects, but there is still a problem: the character sequences
// "Bloom", "berg", and "LP" (at index positions 4 - 6) are "orphaned" and will
// never be deallocated -- a memory leak.  To prevent this potential memory
// leak, we can additionally create an instance of 'bslma::AutoDeallocator' to
// manage (temporarily) the memory at index positions 4 - 6 prior to allocating
// the new memory:
//..
//      0     1     2     3     4     5     6
//    _____ _____ _____ _____ _____ _____ _____
//   |  o  |  o  |xxxxx|xxxxx|  o  |  o  |  o  |
//   `==|==^==|==^=====^=====^==|==^==|==^==|=='
//      |    _V___              |   __V___  |
//      |   |"Bye"|             |  |"berg"| |
//      |   `====='             |  `======' |
//     _V_____                 _V_____     _V__
//    |"Hello"|               |"Bloom"|   |"LP"|
//    `======='               `======='   `===='
//   my_StrArray              ^---------------bslma::AutoDeallocator
//   (length = 2)                          (origin = 4, length = 3)
//
//  Note: Configuration after initializing the proctor.
//..
// If an exception occurs, the array (now of length 2) is in a perfectly valid
// state, while the proctor is responsible for deallocating the orphaned memory
// at index positions 4 - 6.  If no exception is thrown, the length is set to 7
// and the proctor's 'release()' method is called, releasing its control over
// the (temporarily) managed memory.
//
// The following example illustrates the use of 'bslma::AutoDeallocator' to
// manage temporarily an "out-of-place" array of character sequences during the
// array's insertion operation.
//
// First we define a 'my_StrArray' class which stores an array of character
// sequences.
//..
    template <class ALLOCATOR>
    class my_StrArray {
        // This class is a container that stores an array of character
        // sequences.  Memory will be supplied by the parameterized 'ALLOCATOR'
        // type provided at construction (which must remain valid throughout
        // the lifetime of this guard object).  Note that memory is managed by
        // a parametrized 'ALLCOATOR' type, instead of a 'bslma::Allocator', to
        // enable clients to pass in a pool (such as a sequential pool)
        // optimized for allocations of character sequences.

        // DATA
        char      **d_array_p;      // dynamically allocated array of character
                                    // sequence

        int         d_length;       // logical length of this array

        int         d_size;         // physical capacity of this array

        ALLOCATOR  *d_allocator_p;  // allocator or pool (held, not owned)

      public:
        // CREATORS
        my_StrArray(ALLOCATOR *basicAllocator);
            // Create a 'my_StrArray' object using the specified
            // 'basicAllocator' used to supply memory.

        ~my_StrArray();
            // Destroy this 'my_StrArray' object and all elements currently
            // stored.

        // MANIPULATORS
        void append(const char *src);
            // Append to this array the string 'src'.

        void insert(int dstIndex, const my_StrArray& srcArray);
            // Insert into this array at the specified 'dstIndex', the
            // character sequences in the specified 'srcArray'.  All values
            // with initial indices at or above 'dstIndex' are shifted up by
            // unless '0 <= dstIndex' and 'dstIndex <= length()'.  Note that
            // this method is functionally the same as 'insert2', but has a
            // different implementation to facilitate the usage example.

        void insert2(int dstIndex, const my_StrArray& srcArray);
            // Insert into this array at the specified 'dstIndex', the
            // character sequences in the specified 'srcArray'.  All values
            // with initial indices at or above 'dstIndex' are shifted up by
            // 'srcArray.length()' index positions.  The behavior is undefined
            // unless '0 <= dstIndex' and 'dstIndex <= length()'.  Note that
            // this method is functionally the same as 'insert', but has a
            // different implementation to facilitate the usage example.

        // ...

        // ACCESSORS
        int length() const;
            // Return the logical length of this array.

        const char *operator[](int index) const;
            // Return a pointer to the 'index'th string in the array.

        // ...
    };
//..
// Next, we define the 'insert' method of 'my_StrArray':
//..
    template <class ALLOCATOR>
    void my_StrArray<ALLOCATOR>::insert(int                           dstIndex,
                                        const my_StrArray<ALLOCATOR>& srcArray)
    {
        int srcLength  = srcArray.d_length;
        int newLength  = d_length + srcLength;
        int numShifted = d_length - dstIndex;

        if (newLength > d_size) {
            while (d_size < newLength) {
                d_size = ! d_size ? 1 : 2 * d_size;
            }

            char ** newArray =
                    (char **) d_allocator_p->allocate(d_size * sizeof(char *));
            memcpy(newArray, d_array_p, d_length * sizeof(char *));
            if (d_array_p) {
                d_allocator_p->deallocate(d_array_p);
            }
            d_array_p = newArray;
        }

        char **tmpSrc = srcArray.d_array_p;
        if (this == &srcArray) {
            // self-alias
            size_t size = srcLength * sizeof(char *);
            tmpSrc = (char **) d_allocator_p->allocate(size);
            memcpy(tmpSrc, d_array_p, size);
        }
        bslma::DeallocatorProctor<ALLOCATOR> guard(this == &srcArray ? tmpSrc
                                                                     : 0,
                                                  d_allocator_p);

        // First shift the elements to the back of the array.
        memmove(d_array_p + dstIndex + srcLength,
                d_array_p + dstIndex,
                numShifted * sizeof *d_array_p);

        // Shorten 'd_length' and use 'bslma::AutoDeallocator' to proctor the
        // memory shifted.
        d_length = dstIndex;

        //*******************************************************
        // Note use of auto deallocator on tail memory (below). *
        //*******************************************************

        bslma::AutoDeallocator<ALLOCATOR> tailDeallocator(
                                    (void **) d_array_p + dstIndex + srcLength,
                                    d_allocator_p,
                                    numShifted);
//..
// Now, if any allocation for the inserted character sequences throws, the
// memory used for the character sequences that had been moved to the end of
// array will be deallocated automatically by the 'bslma::AutoDeallocator'.
//..
        // Copy the character sequences from the 'srcArray'.
        for (int i = 0; i < srcLength; ++i, ++d_length) {
            size_t size = strlen(tmpSrc[i]) + 1;
            d_array_p[dstIndex + i] = (char *) d_allocator_p->allocate(size);
            memcpy(d_array_p[dstIndex + i], tmpSrc[i], size);
        }

        //*********************************************
        // Note that the proctor is released (below). *
        //*********************************************

        tailDeallocator.release();
        d_length = newLength;
    }
//..
// The above method copies the source elements (visually) from left to right.
// Another (functionally equivalent) implementation copies the source elements
// from right to left, and makes use of the 'operator--()' of the
// 'bslma::AutoDeallocator' interface:
//..
    template <class ALLOCATOR>
    void my_StrArray<ALLOCATOR>::insert2(
                                        int                           dstIndex,
                                        const my_StrArray<ALLOCATOR>& srcArray)
    {
        int srcLength  = srcArray.d_length;
        int newLength  = d_length + srcLength;
        int numShifted = d_length - dstIndex;

        if (newLength > d_size) {
            while (d_size < newLength) {
                d_size = ! d_size ? 1 : 2 * d_size;
            }

            char ** newArray =
                    (char **) d_allocator_p->allocate(d_size * sizeof(char *));
            memcpy(newArray, d_array_p, d_length * sizeof(char *));
            if (d_array_p) {
                d_allocator_p->deallocate(d_array_p);
            }
            d_array_p = newArray;
        }

        char **tmpSrc = srcArray.d_array_p;
        if (this == &srcArray) {
            // self-alias
            size_t size = srcLength * sizeof(char *);
            tmpSrc = (char **) d_allocator_p->allocate(size);
            memcpy(tmpSrc, d_array_p, size);
        }
        bslma::DeallocatorProctor<ALLOCATOR> guard(this == &srcArray ? tmpSrc
                                                                     : 0,
                                                  d_allocator_p);

        // First shift the elements to the back of the array.
        memmove(d_array_p + dstIndex + srcLength,
                d_array_p + dstIndex,
                numShifted * sizeof *d_array_p);

        // Shorten 'd_length' and use 'bslma::AutoDeallocator' to proctor the
        // memory shifted.
        d_length = dstIndex;

        //********************************************
        //* Note the use of auto deallocator on tail *
        //* memory with negative length (below).     *
        //********************************************

        bslma::AutoDeallocator<ALLOCATOR> tailDeallocator(
                       (void **) d_array_p + d_length + srcLength + numShifted,
                       d_allocator_p,
                       -numShifted);
//..
// Since we have decided to copy the source elements from right to left, we
// set the origin of the 'bslma::AutoDeallocator' to the end of the array, and
// decrement the (signed) length on each copy to extend the proctor range by
// 1.
//..
        // Copy the character sequences from the 'srcArray'.  Note that the
        // 'tailDeallocator' has to be decremented to cover the newly
        // created object.

        for (int i = srcLength - 1; i >= 0; --i, --tailDeallocator) {
            size_t size = strlen(tmpSrc[i]) + 1;
            d_array_p[dstIndex + i] = (char *)d_allocator_p->allocate(size);
            memcpy(d_array_p[dstIndex + i], tmpSrc[i], size);
        }

        //*********************************************
        // Note that the proctor is released (below). *
        //*********************************************

        tailDeallocator.release();
        d_length = newLength;
    }
//..
// Note that though the two implementations are functionally equivalent, they
// are logically different.  First of all, the second implementation will be
// slightly slower because it is accessing memory backwards when compared to
// the normal forward sequential access.  Secondly, in case of an exception,
// the first implementation will retain all the elements copied prior to the
// exception, whereas the second implementation will remove them.

//=============================================================================
//           Additional Functionality Needed to Complete Usage Test Case
//-----------------------------------------------------------------------------

// CREATORS
template <class ALLOCATOR>
inline
my_StrArray<ALLOCATOR>::my_StrArray(ALLOCATOR *basicAllocator)
: d_array_p(0)
, d_length(0)
, d_size(0)
, d_allocator_p(basicAllocator)
{
}

template <class ALLOCATOR>
my_StrArray<ALLOCATOR>::~my_StrArray()
{
    for (int i = 0; i < d_length; ++i) {
        d_allocator_p->deallocate(d_array_p[i]);
    }
    d_allocator_p->deallocate(d_array_p);
}

// MANIPULATORS
template <class ALLOCATOR>
void my_StrArray<ALLOCATOR>::append(const char *src)
{
    if (d_length == d_size) {
        d_size = ! d_size ? 1 : 2 * d_size;

        char ** newArray =
                 (char **) d_allocator_p->allocate(d_size * sizeof(char*));
        memcpy(newArray, d_array_p, d_length * sizeof(char *));
        if (d_array_p) {
            d_allocator_p->deallocate(d_array_p);
        }
        d_array_p = newArray;
    }
    size_t length = strlen(src) + 1;
    d_array_p[d_length] = (char *) d_allocator_p->allocate(length);
    memcpy(d_array_p[d_length], src, length);
    ++d_length;
}

// ACCESSORS
template <class ALLOCATOR>
inline
int my_StrArray<ALLOCATOR>::length() const
{
    return d_length;
}

template <class ALLOCATOR>
inline
const char *my_StrArray<ALLOCATOR>::operator[](int index) const
{
    return d_array_p[index];
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    globalVeryVeryVeryVerbose = veryVeryVeryVerbose;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Run the usage example and exercise the creators and manipulators
        //   of 'myQueue' using a 'bslma::TestAllocator' to verify that memory
        //   is allocated and deallocated properly.
        //
        // Testing:
        //   Usage example
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        bslma::TestAllocator testAllocator(veryVeryVeryVerbose);
        const bslma::TestAllocator& TA = testAllocator;

#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) printf("\nTesting with exceptions\n");
#endif

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_StrArray<bslma::Allocator> arrayA(&testAllocator);

            arrayA.append("Hello");
            arrayA.append("Bye");
            arrayA.append("Bloom");
            arrayA.append("berg");
            arrayA.append("LP");

            my_StrArray<bslma::Allocator> arrayB(&testAllocator);

            arrayB.append("BDE");
            arrayB.append("101");

            arrayA.insert(2, arrayB);

            ASSERT(7 == arrayA.length());
            ASSERT(!strcmp(arrayA[0], "Hello"));
            ASSERT(!strcmp(arrayA[1], "Bye"));
            ASSERT(!strcmp(arrayA[2], "BDE"));
            ASSERT(!strcmp(arrayA[3], "101"));
            ASSERT(!strcmp(arrayA[4], "Bloom"));
            ASSERT(!strcmp(arrayA[5], "berg"));
            ASSERT(!strcmp(arrayA[6], "LP"));

            ASSERT(arrayB.length() == 2);
            ASSERT(!strcmp(arrayB[0], "BDE"));
            ASSERT(!strcmp(arrayB[1], "101"));
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERT(0 == TA.numBytesInUse());

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_StrArray<bslma::Allocator> arrayA(&testAllocator);

            arrayA.append("Hello");
            arrayA.append("Bye");
            arrayA.append("Bloom");
            arrayA.append("berg");
            arrayA.append("LP");

            my_StrArray<bslma::Allocator> arrayB(&testAllocator);

            arrayB.append("BDE");
            arrayB.append("101");

            arrayA.insert2(2, arrayB);

            ASSERT(7 == arrayA.length());
            ASSERT(!strcmp(arrayA[0], "Hello"));
            ASSERT(!strcmp(arrayA[1], "Bye"));
            ASSERT(!strcmp(arrayA[2], "BDE"));
            ASSERT(!strcmp(arrayA[3], "101"));
            ASSERT(!strcmp(arrayA[4], "Bloom"));
            ASSERT(!strcmp(arrayA[5], "berg"));
            ASSERT(!strcmp(arrayA[6], "LP"));

            ASSERT(2 == arrayB.length());
            ASSERT(!strcmp(arrayB[0], "BDE"));
            ASSERT(!strcmp(arrayB[1], "101"));
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERT(0 == TA.numBytesInUse());

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_StrArray<bslma::Allocator> arrayA(&testAllocator);

            arrayA.append("Hello");
            arrayA.append("Bye");
            arrayA.append("Bloom");
            arrayA.append("berg");
            arrayA.append("LP");

            // aliasing
            arrayA.insert(4, arrayA);

            ASSERT(10 == arrayA.length());
            ASSERT(!strcmp(arrayA[0], "Hello"));
            ASSERT(!strcmp(arrayA[1], "Bye"));
            ASSERT(!strcmp(arrayA[2], "Bloom"));
            ASSERT(!strcmp(arrayA[3], "berg"));
            ASSERT(!strcmp(arrayA[4], "Hello"));
            ASSERT(!strcmp(arrayA[5], "Bye"));
            ASSERT(!strcmp(arrayA[6], "Bloom"));
            ASSERT(!strcmp(arrayA[7], "berg"));
            ASSERT(!strcmp(arrayA[8], "LP"));
            ASSERT(!strcmp(arrayA[9], "LP"));
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERT(0 == TA.numBytesInUse());

        BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
            my_StrArray<bslma::Allocator> arrayA(&testAllocator);

            arrayA.append("Hello");
            arrayA.append("Bye");
            arrayA.append("Bloom");
            arrayA.append("berg");
            arrayA.append("LP");

            // aliasing
            arrayA.insert2(4, arrayA);

            ASSERT(10 == arrayA.length());
            ASSERT(!strcmp(arrayA[0], "Hello"));
            ASSERT(!strcmp(arrayA[1], "Bye"));
            ASSERT(!strcmp(arrayA[2], "Bloom"));
            ASSERT(!strcmp(arrayA[3], "berg"));
            ASSERT(!strcmp(arrayA[4], "Hello"));
            ASSERT(!strcmp(arrayA[5], "Bye"));
            ASSERT(!strcmp(arrayA[6], "Bloom"));
            ASSERT(!strcmp(arrayA[7], "berg"));
            ASSERT(!strcmp(arrayA[8], "LP"));
            ASSERT(!strcmp(arrayA[9], "LP"));
        } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

        ASSERT(0 == TA.numBytesInUse());
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 'operator++' AND 'operator--' TEST
        //
        // Concerns:
        //   That both 'operator++' and 'operator--' respectively increment and
        //   decrement the (signed) length of the array of memory blocks
        //   protected by the range proctor.
        //
        // Plan:
        //   Since the 'length' method is thoroughly tested in case 7, we can
        //   simply verify that 'operator++' and 'operator--' properly set
        //   the length by checking the values returned by 'length()' for
        //   positive, negative, and zero values of length.
        //
        // Testing:
        //   void operator++();
        //   void operator--();
        // --------------------------------------------------------------------

        if (verbose) printf("\n'operator++' AND 'operator--' TEST"
                            "\n==================================\n");

        enum { NUM_BLOCKS = 10};
        void *pointers[NUM_BLOCKS];  // dummy pointer passed to the proctor
        bslma::TestAllocator ta(veryVeryVeryVerbose); // dummy allocator passed
                                                      // to the proctor

        if (verbose) printf("\nTesting 'operator++' with positive length\n");

        bslma::AutoDeallocator<bslma::Allocator> proctor(pointers, &ta, 0);
        for (int i = 0; i < NUM_BLOCKS; ++i) {
            LOOP_ASSERT(i, i == proctor.length());
            ++proctor;
        }
        ASSERT(NUM_BLOCKS == proctor.length());

        if (verbose) printf("\nTesting 'operator--' with positive length\n");

        for (int i = NUM_BLOCKS - 1; i >= 0; --i) {
            --proctor;
            LOOP_ASSERT(i, i == proctor.length());
        }
        ASSERT(0 == proctor.length());

        if (verbose) printf("\nTesting 'operator--' with negative length\n");

        for (int i = 0; i < NUM_BLOCKS; ++i) {
            LOOP_ASSERT(i, -i == proctor.length());
            --proctor;
        }
        ASSERT(-NUM_BLOCKS == proctor.length());

        if (verbose) printf("\nTesting 'operator++' with negative length\n");

        for (int i = NUM_BLOCKS - 1; i >= 0; --i) {
            ++proctor;
            LOOP_ASSERT(i, -i == proctor.length());
        }
        ASSERT(0 == proctor.length());
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // 'setLength' AND 'length' TEST
        //
        // Concerns:
        //   1) That 'setLength' properly sets the length of the sequence of
        //      memory blocks protected by the 'TestAllocator'.
        //   2) That the 'length' method properly returns the number of memory
        //      blocks currently protected by a 'const'
        //      'bslma::AutoDeallocator'.
        //
        // Plan:
        //   For concern 1, allocate a sequence of memory blocks using a
        //   'TestAllocator'.  Then, initialize a 'bslma::AutoDeallocator'
        //   object with the sequence of memory blocks, the
        //   'TestAllocator' and a fixed length.  Invoke 'setLength' to
        //   modify the number of elements covered by the range proctor before
        //   the proctor goes out of scope.  Once the proctor goes out
        //   of scope, verify that only the range of memory blocks covered by
        //   the proctor is deallocated.
        //
        //   For concern 2, allocate a sequence of memory blocks of various
        //   lengths using a 'TestAllocator'.  Then, initialize a
        //   'bslma::AutoDeallocator' object with the sequence of memory
        //   blocks.  Verify that 'length' returns the number of memory blocks
        //   currently protected by the range proctor.
        //
        // Testing:
        //   void setLength(length);
        //   int length();
        // --------------------------------------------------------------------

        if (verbose) printf("\n'setLength' AND 'length' TEST"
                            "\n=============================\n");

        TestAllocator t;
        const TestAllocator& T = t;

        enum { NUM_BLOCKS = 30 };

        if (verbose) printf("\nTesting 'setLength'\n");

        for (int i = -NUM_BLOCKS / 2; i < NUM_BLOCKS / 2 + 1; ++i) {
            void *pointers[NUM_BLOCKS];

            for (int j = 0; j < NUM_BLOCKS; ++j) {
                LOOP_ASSERT(j, T.numOutstandingAllocations() == j);
                pointers[j] = t.makePointer();
            }

            if (veryVerbose) { T_ P_(i) P(T.numOutstandingAllocations()) }
            ASSERT(NUM_BLOCKS == T.numOutstandingAllocations());

            {
                bslma::AutoDeallocator<TestAllocator> proctor(
                                                       &pointers[NUM_BLOCKS/2],
                                                       &t,
                                                       0);

                if (veryVerbose) { T_ P_(i) P(T.numOutstandingAllocations()) }
                ASSERT(NUM_BLOCKS == T.numOutstandingAllocations());

                proctor.setLength(i);

                if (veryVerbose) { T_ P_(i) P(T.numOutstandingAllocations()) }
                ASSERT(NUM_BLOCKS == T.numOutstandingAllocations());
            }
            if (i < 0) {
                if (veryVerbose) { T_ P_(i) P(T.numOutstandingAllocations()) }
                ASSERT(NUM_BLOCKS + i == T.numOutstandingAllocations());

                // Clean up
                for (int j = 0; j < NUM_BLOCKS/2+i; ++j) {
                        t.deallocate(pointers[j]);
                }
                for (int j = NUM_BLOCKS/2; j < NUM_BLOCKS; ++j) {
                        t.deallocate(pointers[j]);
                }
            }
            else {
                if (veryVerbose) { T_ P_(i) P(T.numOutstandingAllocations()) }
                ASSERT(NUM_BLOCKS - i == T.numOutstandingAllocations());

                // Clean up
                for (int j = 0; j < NUM_BLOCKS/2; ++j) {
                        t.deallocate(pointers[j]);
                }
                for (int j = NUM_BLOCKS/2 + i; j < NUM_BLOCKS; ++j) {
                        t.deallocate(pointers[j]);
                }
            }

            ASSERT(0 == T.numOutstandingAllocations());
        }

        if (verbose) printf("\nTesting 'length'\n");

        for (int i = -NUM_BLOCKS / 2; i < NUM_BLOCKS / 2 + 1; ++i) {
            void *pointers[NUM_BLOCKS];  // dummy

            {
                bslma::AutoDeallocator<TestAllocator> proctor(pointers, &t, i);

                if (veryVerbose) { T_ P_(i) P(proctor.length()) }
                ASSERT(i == proctor.length());

                proctor.release();  // do nothing
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'reset' TEST
        //
        // Concerns:
        //   That when the 'reset' method is called, the range proctor object
        //   properly manages a different sequence of memory blocks of the same
        //   length.
        //
        // Plan:
        //   Allocate a sequence of memory blocks using a
        //   'bslma::TestAllocator'.  Next allocate another sequence of memory
        //   blocks of the same length, but different size.  Finally,
        //   initialize a 'bslma::AutoDeallocator' object with the first
        //   sequence of memory blocks and the 'bslma::TestAllocator'.  Call
        //   'reset' on the proctor with the second sequence of memory blocks
        //   before the proctor goes out of scope.  Once the proctor goes out
        //   of scope, verify that only the second sequence of memory blocks
        //   are deallocated (by checking the expected number of bytes in use).
        //
        // Testing:
        //   void reset(origin);
        // --------------------------------------------------------------------

        if (verbose) printf("\n'reset' TEST"
                            "\n============\n");

        if (verbose) printf("\nTesting the 'reset' method\n");

        enum { NUM_TEST = 20, SIZE1 = sizeof(int), SIZE2 = sizeof(char) };

        bslma::TestAllocator ta1(veryVeryVeryVerbose);
        bslma::TestAllocator ta2(veryVeryVeryVerbose);
        const bslma::TestAllocator& TA1 = ta1;
        const bslma::TestAllocator& TA2 = ta2;

        static void *pointers1a[NUM_TEST];
        static void *pointers1b[NUM_TEST];
        static void *pointers2a[NUM_TEST];
        static void *pointers2b[NUM_TEST];

        for (int i = 0; i < NUM_TEST; ++i) {
            LOOP_ASSERT(i, (SIZE1 + SIZE2) * i == TA1.numBytesInUse());
            LOOP_ASSERT(i, (SIZE1 + SIZE2) * i == TA2.numBytesInUse());

            pointers1a[i] = ta1.allocate(SIZE1);
            pointers1b[i] = ta1.allocate(SIZE2);
            pointers2a[i] = ta2.allocate(SIZE1);
            pointers2b[i] = ta2.allocate(SIZE2);
        }

        if (veryVerbose) { T_ P_(TA1.numBytesInUse()) P(TA2.numBytesInUse()) }
        ASSERT((SIZE1 + SIZE2) * NUM_TEST == TA1.numBytesInUse());
        ASSERT((SIZE1 + SIZE2) * NUM_TEST == TA2.numBytesInUse());

        {
            bslma::AutoDeallocator<bslma::Allocator> proctor1(pointers1a,
                                                              &ta1,
                                                              NUM_TEST);
            bslma::AutoDeallocator<bslma::Allocator> proctor2(pointers2a,
                                                              &ta2,
                                                              NUM_TEST);

            if (veryVerbose) { T_ P_(TA1.numBytesInUse())
                               P(TA2.numBytesInUse()) }
            ASSERT((SIZE1 + SIZE2) * NUM_TEST == TA1.numBytesInUse());
            ASSERT((SIZE1 + SIZE2) * NUM_TEST == TA2.numBytesInUse());

            proctor2.reset(pointers2b);

            if (veryVerbose) { T_ P_(TA1.numBytesInUse())
                               P(TA2.numBytesInUse()) }
            ASSERT((SIZE1 + SIZE2) * NUM_TEST == TA1.numBytesInUse());
            ASSERT((SIZE1 + SIZE2) * NUM_TEST == TA2.numBytesInUse());
        }

        if (veryVerbose) { T_ P_(TA1.numBytesInUse())
                           P(TA2.numBytesInUse()) }
        ASSERT(SIZE2 * NUM_TEST == TA1.numBytesInUse());
        ASSERT(SIZE1 * NUM_TEST == TA2.numBytesInUse());

        for (int i = NUM_TEST - 1; i >= 0; --i) {
            ta1.deallocate(pointers1b[i]);
            ta2.deallocate(pointers2a[i]);
            LOOP_ASSERT(i, SIZE2 * i == TA1.numBytesInUse());
            LOOP_ASSERT(i, SIZE1 * i == TA2.numBytesInUse());
        }
        ASSERT(TA1.numBytesInUse() == 0);
        ASSERT(TA2.numBytesInUse() == 0);

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //   That when the 'release' method is called, the range proctor object
        //   properly releases from management the sequence of memory blocks
        //   currently managed by this proctor.
        //
        // Plan:
        //   Allocate two array of memory blocks of equal length using
        //   'bslma::TestAllocator'.  Next initialize two
        //   'bslma::AutoDeallocator' proctors, one with the first array and
        //   the other with the second.  Call 'release' on the first proctor
        //   before it goes out of scope.  Verify that only the memory blocks
        //   managed by the second proctor are deallocated.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) printf("\n'release' TEST"
                            "\n==============\n");

        if (verbose) printf("\nTesting the 'release' method.\n");

        bslma::TestAllocator ta1(veryVeryVeryVerbose);
        bslma::TestAllocator ta2(veryVeryVeryVerbose);
        const bslma::TestAllocator& TA1 = ta1;
        const bslma::TestAllocator& TA2 = ta2;

        enum { NUM_TEST = 20, SIZE = sizeof(int) };

        static void *pointers1[NUM_TEST];  // guaranteed to init to 0
        static void *pointers2[NUM_TEST];  // guaranteed to init to 0

        for (int i = 0; i < NUM_TEST; ++i) {
            LOOP_ASSERT(i, SIZE * i == TA1.numBytesInUse());
            LOOP_ASSERT(i, SIZE * i == TA2.numBytesInUse());
            pointers1[i] = ta1.allocate(SIZE);
            pointers2[i] = ta2.allocate(SIZE);
        }

        if (veryVerbose) { T_ P(TA1.numBytesInUse()) }
        if (veryVerbose) { T_ P(TA2.numBytesInUse()) }
        ASSERT(SIZE * NUM_TEST == TA1.numBytesInUse());
        ASSERT(SIZE * NUM_TEST == TA2.numBytesInUse());

        {
            bslma::AutoDeallocator<bslma::Allocator> proctor1(pointers1,
                                                              &ta1,
                                                              NUM_TEST);
            bslma::AutoDeallocator<bslma::Allocator> proctor2(pointers2,
                                                              &ta2,
                                                              NUM_TEST);

            if (veryVerbose) { T_ P(TA1.numBytesInUse()) }
            if (veryVerbose) { T_ P(TA2.numBytesInUse()) }
            ASSERT(SIZE * NUM_TEST == TA1.numBytesInUse());
            ASSERT(SIZE * NUM_TEST == TA2.numBytesInUse());

            proctor1.release();

            if (veryVerbose) { T_ P(TA1.numBytesInUse()) }
            if (veryVerbose) { T_ P(TA2.numBytesInUse()) }
            ASSERT(SIZE * NUM_TEST == TA1.numBytesInUse());
            ASSERT(SIZE * NUM_TEST == TA2.numBytesInUse());
        }

        if (veryVerbose) { T_ P(TA1.numBytesInUse()) }
        if (veryVerbose) { T_ P(TA2.numBytesInUse()) }
        ASSERT(SIZE * NUM_TEST == TA1.numBytesInUse());
        ASSERT(0               == TA2.numBytesInUse());

        for (int i = NUM_TEST - 1; i >= 0; --i) {
            ta1.deallocate(pointers1[i]);
            LOOP_ASSERT(i, SIZE * i == TA1.numBytesInUse());
        }
        ASSERT(0 == TA1.numBytesInUse());
        ASSERT(0 == TA2.numBytesInUse());

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CTOR / DTOR TEST
        //
        // Concerns:
        //   1) That the 'bslma::AutoDeallocator' automatically guards a
        //      sequence of memory blocks of the provided length (positive).
        //   2) That the 'bslma::AutoDeallocator' automatically guards a
        //      sequence of memory blocks of the provided length (negative).
        //   3) That the 'bslma::AutoDeallocator' guards nothing when supplied
        //      a zero length at construction.
        //   4) That we can construct a 'bslma::AutoDeallocator' with zero
        //      length and a null pointer.
        //   5) That when an allocator (or pool) not inherited from
        //      'bslma::Allocator' is supplied to the 'bslma::AutoDeallocator',
        //      the 'deallocate' method of the allocator (or pool) supplied is
        //      still invoked.
        //
        // Plan:
        //   Allocate a sequence of memory blocks using 'bslma::TestAllocator'.
        //   Next create a 'bslma::AutoDeallocator' to proctor the array of
        //   memory blocks.  When the range proctor goes out of scope, verify
        //   that the allocated memory blocks are deallocated.  Repeat for the
        //   different lengths outlined under concerns 1 - 4.
        //
        //   Finally, repeat the all of the above using the 'TestAllocator',
        //   which does not inherit from 'bslma::Allocator'.
        //
        // Testing:
        //   bslma::AutoDeallocator<ALLOCATOR>(origin, allocator, length = 0);
        //   ~bslma::AutoDeallocator<ALLOCATOR>();
        //   CONCERN: the 'deallocate' method for pools is also invoked
        // --------------------------------------------------------------------

        if (verbose) printf("\nCTOR / DOTR TEST"
                            "\n================\n");

        if (verbose) printf("\nTesting with bslma_TestAllocator.\n");

        enum { NUM_TEST = 20, SIZE = sizeof(int) };
        bslma::TestAllocator ta(veryVeryVeryVerbose);
        const bslma::TestAllocator &TA = ta;

        if (verbose) printf("\tTesting positive length.\n");

        static void *pointers[NUM_TEST];  // guaranteed to init to 0

        for (int i = 1; i < NUM_TEST; ++i) {

            // First allocate memory.
            for (int j = 0; j < NUM_TEST; ++j) {
                pointers[j] = ta.allocate(SIZE);
                LOOP2_ASSERT(i, j, (j + 1) * SIZE == TA.numBytesInUse());
            }

            if (veryVerbose) { T_ T_ P_(i) P(TA.numBytesInUse()); }
            ASSERT(NUM_TEST * SIZE == TA.numBytesInUse());

            // Test the range proctor.
            {
                bslma::AutoDeallocator<bslma::Allocator> ad(pointers,
                                                            &ta,
                                                            i);

                if (veryVerbose) { T_ T_ P_(i) P(TA.numBytesInUse()); }
                ASSERT(NUM_TEST * SIZE == TA.numBytesInUse());
            }

            if (veryVerbose) { T_ T_ P_(i) P(TA.numBytesInUse()); }
            ASSERT((NUM_TEST - i) * SIZE == TA.numBytesInUse());

            // Deallocate the remaining memory not guarded by the range
            // proctor.
            for (int j = i; j <  NUM_TEST; ++j) {
                LOOP2_ASSERT(i, j,
                             (NUM_TEST - j) * SIZE == TA.numBytesInUse());
                ta.deallocate(pointers[j]);
            }

            ASSERT(0 == TA.numBytesInUse());
        }

        memset(pointers, 0, NUM_TEST * sizeof(void *));

        if (verbose) printf("\tTesting negative length.\n");

        for (int i = 1; i < NUM_TEST; ++i) {

            // First allocate memory.
            for (int j = 0; j < NUM_TEST; ++j) {
                pointers[j] = ta.allocate(SIZE);
                LOOP2_ASSERT(i, j, (j + 1) * SIZE == TA.numBytesInUse());
            }

            if (veryVerbose) { T_ T_ P_(i) P(TA.numBytesInUse()); }
            ASSERT(NUM_TEST * SIZE == TA.numBytesInUse());

            // Test the range proctor.
            {
                bslma::AutoDeallocator<bslma::Allocator> ad(
                                                           pointers + NUM_TEST,
                                                           &ta,
                                                           -i);
                if (veryVerbose) { T_ T_ P_(i) P(TA.numBytesInUse()); }
                ASSERT(NUM_TEST * SIZE == TA.numBytesInUse());
            }

            if (veryVerbose) { T_ T_ P_(i) P(TA.numBytesInUse()); }
            ASSERT((NUM_TEST - i) * SIZE == TA.numBytesInUse());

            // Deallocate the remaining memory not guarded by the range
            // proctor.
            for (int j = 0; j < NUM_TEST - i; ++j) {
                LOOP2_ASSERT(i, j,
                             (NUM_TEST - j - i) * SIZE == TA.numBytesInUse());
                ta.deallocate(pointers[j]);
            }

            ASSERT(0 == TA.numBytesInUse());
        }

        memset(pointers, 0, NUM_TEST * sizeof(void *));

        if (verbose) printf("\tTesting default length.\n");

        for (int i = 0; i < NUM_TEST; ++i) {
            pointers[i] = ta.allocate(SIZE);
            LOOP_ASSERT(i, (i + 1) * SIZE == TA.numBytesInUse());
        }

        if (veryVerbose) { T_ T_ P(TA.numBytesInUse()); }
        ASSERT(NUM_TEST * SIZE == TA.numBytesInUse());

        {
            bslma::AutoDeallocator<bslma::Allocator> ad(pointers, &ta);

            if (veryVerbose) { T_ T_ P(TA.numBytesInUse()); }
            ASSERT(NUM_TEST * SIZE == TA.numBytesInUse());
        }

        if (veryVerbose) { T_ T_ P(TA.numBytesInUse()); }
        ASSERT(NUM_TEST * SIZE == TA.numBytesInUse());

        for (int i = 0; i < NUM_TEST; ++i) {
            LOOP_ASSERT(i, (NUM_TEST - i) * SIZE == TA.numBytesInUse());
            ta.deallocate(pointers[i]);
        }

        ASSERT(0 == TA.numBytesInUse());

        if (verbose) printf("\tTesting Construction with null pointer.\n");

        {
            bslma::AutoDeallocator<bslma::Allocator> ad(0, &ta);
        }

        if (verbose) printf("\nTesting with TestAllocator.\n");

        TestAllocator t;
        const TestAllocator &T = t;

        if (verbose) printf("\tTesting positive length.\n");

        for (int i = 1; i < NUM_TEST; ++i) {

            // First allocate memory.
            for (int j = 0; j < NUM_TEST; ++j) {
                pointers[j] = t.makePointer();
                LOOP2_ASSERT(i, j, j + 1 == T.numOutstandingAllocations());
            }

            if (veryVeryVerbose) { T_ T_ P(T.numOutstandingAllocations()); }
            ASSERT(NUM_TEST == T.numOutstandingAllocations());

            // Test the range proctor.
            {
                bslma::AutoDeallocator<TestAllocator> ad(pointers,
                                                         &t,
                                                         i);

                if (veryVeryVerbose) { T_ T_ P(T.numOutstandingAllocations());}
                ASSERT(NUM_TEST == T.numOutstandingAllocations());
            }

            if (veryVeryVerbose) { T_ T_ P(T.numOutstandingAllocations()); }
            ASSERT(NUM_TEST - i == T.numOutstandingAllocations());

            // Deallocate the remaining memory not guarded by the range
            // proctor.
            for (int j = i; j <  NUM_TEST; ++j) {
                LOOP2_ASSERT(i, j,
                             NUM_TEST - j == T.numOutstandingAllocations());
                t.deallocate(pointers[j]);
            }

            ASSERT(0 == T.numOutstandingAllocations());
        }

        memset(pointers, 0, NUM_TEST * sizeof(void *));

        if (verbose) printf("\tTesting negative length.\n");

        for (int i = 1; i < NUM_TEST; ++i) {

            // First allocate memory.
            for (int j = 0; j < NUM_TEST; ++j) {
                pointers[j] = t.makePointer();
                LOOP2_ASSERT(i, j, j + 1 == T.numOutstandingAllocations());
            }

            if (veryVerbose) { T_ T_ P_(i) P(T.numOutstandingAllocations()); }
            ASSERT(NUM_TEST == T.numOutstandingAllocations());

            // Test the range proctor.
            {
                bslma::AutoDeallocator<TestAllocator> ad(pointers + NUM_TEST,
                                                         &t,
                                                         -i);

                if (veryVerbose) { T_ T_ P_(i)
                                   P(T.numOutstandingAllocations());}
                ASSERT(NUM_TEST == T.numOutstandingAllocations());
            }

            if (veryVerbose) { T_ T_ P_(i) P(T.numOutstandingAllocations()); }
            ASSERT(NUM_TEST - i == T.numOutstandingAllocations());

            // Deallocate the remaining memory not guarded by the range
            // proctor.
            for (int j = 0; j < NUM_TEST - i; ++j) {
                LOOP2_ASSERT(
                            i, j,
                            NUM_TEST - i - j == T.numOutstandingAllocations());
                t.deallocate(pointers[j]);
            }

            ASSERT(0 == T.numOutstandingAllocations());
        }

        memset(pointers, 0, NUM_TEST * sizeof(void *));

        if (verbose) printf("\tTesting default length.\n");

        for (int i = 0; i < NUM_TEST; ++i) {
            pointers[i] = t.makePointer();
            LOOP_ASSERT(i, i + 1 == T.numOutstandingAllocations());
        }

        if (veryVerbose) { T_ T_ P(T.numOutstandingAllocations()); }
        ASSERT(NUM_TEST == T.numOutstandingAllocations());

        {
            bslma::AutoDeallocator<TestAllocator> ad(pointers, &t);

            if (veryVerbose) { T_ T_ P(T.numOutstandingAllocations()); }
            ASSERT(NUM_TEST == T.numOutstandingAllocations());
        }

        if (veryVerbose) { T_ T_ P(T.numOutstandingAllocations()); }
        ASSERT(NUM_TEST == T.numOutstandingAllocations());

        for (int i = 0; i < NUM_TEST; ++i) {
            LOOP_ASSERT(i, NUM_TEST - i == T.numOutstandingAllocations());
            t.deallocate(pointers[i]);
        }

        ASSERT(0 == T.numOutstandingAllocations());

        if (verbose) printf("\tTesting Construction with null pointer.\n");

        {
            bslma::AutoDeallocator<TestAllocator> ad(0, &t);
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // HELPER CLASS TEST
        //
        // Concerns:
        //   1) The helper class 'TestAllocator' properly keeps track of
        //      addresses allocated and the number of outstanding allocations.
        //
        // Plan:
        //   Randomly intervene calls to 'allocate' and 'deallocate' of a
        //   'TestAllocator'.  Verify that number of outstanding allocations
        //   are properly kept track of.
        //
        // Testing:
        //   Helper Class: 'TestAllocator'
        // --------------------------------------------------------------------

        if (verbose) printf("\nHELPER CLASS TEST"
                            "\n=================\n");

        if (verbose) printf("\nTesting 'TestAllocator'.\n");

        enum { MAX_NUM_POINTERS = 30 };
        static void *pointers[MAX_NUM_POINTERS];  // guaranteed to init to 0
        int numAllocated    = 0;    const int& NUMALLOCATED = numAllocated;
        int maxNumAllocated = 0;

        TestAllocator mX;    const TestAllocator &X = mX;
        (void) X;
        for (int i = 0; i < MAX_NUM_POINTERS; ++i) {
            if (rand() & 0x10) {  // allocate

                // Find empty slot.
                int j;
                for (j = 0; j < MAX_NUM_POINTERS; ++j) {
                    if (!pointers[j]) {
                        break;
                    }
                }

                if (j < MAX_NUM_POINTERS) {
                    pointers[j] = mX.makePointer();
                    ASSERT(pointers[j]);
                    ++numAllocated;
                }
            }
            else {  // deallocate

                // Find allocated pointer.
                int j;
                for (j = 0; j < MAX_NUM_POINTERS; ++j) {
                    if (pointers[j]) {
                        break;
                    }
                }

                if (j < MAX_NUM_POINTERS) {
                    mX.deallocate(pointers[j]);
                    pointers[j] = 0;
                    --numAllocated;
                }
            }

            if (veryVerbose) printf("Net allocated: %d\n", numAllocated);

            maxNumAllocated = intMax(maxNumAllocated, numAllocated);

            LOOP2_ASSERT(mX.numOutstandingAllocations(), numAllocated,
                         NUMALLOCATED == mX.numOutstandingAllocations());
        }

        if (verbose) printf("Max num allocated: %d\n", maxNumAllocated);

        for (int j = 0; j < MAX_NUM_POINTERS; ++j) {
            if (pointers[j]) {
                mX.deallocate(pointers[j]);
                --numAllocated;
            }
            ASSERT(NUMALLOCATED == mX.numOutstandingAllocations());
        }

        ASSERT(0 == mX.numOutstandingAllocations());
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) The 'bslma::AutoDeallocator' can be constructed and destructed
        //      gracefully with both 'void **' and 'char **'.
        //   2) The allocator's 'deallocate' method is invoked.
        //
        // Plan:
        //   Allocate a sequence of memory with a 'bslma::TestAllocator' and
        //   guard it with 'bslma::AutoDeallocator' to show that the
        //   'deallocate' method is called (by verifying all memory is returned
        //   to the 'bslma::TestAllocator').
        //
        // Testing:
        //   Breathing Test
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        bslma::TestAllocator ta(veryVeryVeryVerbose);
        const bslma::TestAllocator& TA = ta;

        enum {NUM_TEST = 10};

        if (verbose) printf("\nTesting with void **.\n");

        void *memory[NUM_TEST];

        for (int i = 0; i < NUM_TEST; ++i) {
            memory[i] = ta.allocate(sizeof(int) * (i + 1));
            ASSERT(static_cast<ptrdiff_t>(sizeof(int)) * (i + 1) * (i + 2) / 2
                                                        == TA.numBytesInUse());
        }

        ASSERT(sizeof(int) * NUM_TEST * (NUM_TEST + 1) / 2
                                                        == TA.numBytesInUse());

        {
            bslma::AutoDeallocator<bslma::Allocator> proctor(&memory[0],
                                                             &ta,
                                                             NUM_TEST);
        }

        ASSERT(0 == TA.numBytesInUse());

        if (verbose) printf("\nTesting with char **.\n");

        char *cmemory[NUM_TEST];
        for (int i = 0; i < NUM_TEST; ++i) {
            cmemory[i] = (char *)ta.allocate(sizeof(int) * (i + 1));
            ASSERT(static_cast<ptrdiff_t>(sizeof(int)) * (i + 1) * (i + 2) / 2
                                                        == TA.numBytesInUse());
        }

        ASSERT(sizeof(int) * NUM_TEST * (NUM_TEST + 1) / 2
                                                        == TA.numBytesInUse());

        {
            bslma::AutoDeallocator<bslma::Allocator> proctor(&cmemory[0],
                                                             &ta,
                                                             NUM_TEST);
        }

        ASSERT(0 == TA.numBytesInUse());

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
