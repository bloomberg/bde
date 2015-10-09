// bdlma_sequentialpool.t.cpp                                         -*-C++-*-
#include <bdlma_sequentialpool.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignmentutil.h>
#include <bsls_asserttest.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// A 'bdlma::SequentialPool' is a mechanism (i.e., having state but no value)
// that is used as a memory manager to manage dynamically allocated memory.
// The primary concern is that the sequential pool's internal block list grows
// using the specified growth strategy, has the specified initial size, and is
// constrained by the specified maximum buffer size.  These options together
// create 12 variations of the constructor, and they must all be thoroughly
// tested.
//
// Because 'bdlma::SequentialPool' does not have any accessors, this test
// driver verifies the correctness of the pool's allocations *indirectly*
// through the use of two consecutive allocations -- where the first allocation
// tests for correctness of 'allocate', and the second verifies the size of the
// first allocation and its memory alignment.
//
// We make heavy use of the 'bslma::TestAllocator' to ensure that:
//
// (1) The growth rate of the internal block list matches the specified growth
//     strategy and is constrained by the specified maximum buffer size.
//
// (2) When 'release' is invoked, all memory managed by the pool is
//     deallocated.
//
// (3) When the pool is destroyed, all managed memory is deallocated.
//
// Finally, the destructor of 'bdlma::SequentialPool' is tested throughout the
// test driver.  At destruction, the pool should reclaim all outstanding
// allocated memory.  By setting the global allocator, default allocator, and
// object allocator to different test allocators, we can determine whether all
// memory had been released by the destructor of the pool.
//-----------------------------------------------------------------------------
// // CREATORS
// [ 3] bdlma::SequentialPool(bslma::Allocator *a = 0);
// [ 3] bdlma::SequentialPool(GS g, bslma::Allocator *a = 0);
// [ 3] bdlma::SequentialPool(AS a, bslma::Allocator *a = 0);
// [ 3] bdlma::SequentialPool(GS g, AS a, bslma::Allocator *a = 0);
//
// [ 3] bdlma::SequentialPool(int i, bslma::Allocator *a = 0);
// [ 3] bdlma::SequentialPool(int i, GS g, bslma::Allocator *a = 0);
// [ 3] bdlma::SequentialPool(int i, AS a, bslma::Allocator *a = 0);
// [ 3] bdlma::SequentialPool(int i, GS g, AS a, bslma::Allocator *a = 0);
//
// [ 3] bdlma::SequentialPool(int i, int m, bslma::Allocator *a = 0);
// [ 3] bdlma::SequentialPool(int i, int m, GS g, bslma::Allocator *a = 0);
// [ 3] bdlma::SequentialPool(int i, int m, AS a, bslma::Allocator *a = 0);
// [ 3] bdlma::SequentialPool(int i, int m, GS g, AS a, *a = 0);
//
// [  ] ~bdlma::SequentialPool();
//
// // MANIPULATORS
// [ 4] void *allocate(size_type size);
// [ 7] void *allocateAndExpand(size_type *size);
// [ 6] void deleteObjectRaw(const TYPE *object);
// [ 6] void deleteObject(const TYPE *object);
// [ 5] void release();
// [ 9] void reserveCapacity(int numBytes);
// [ 8] int truncate(void *address, int originalSize, int newSize);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] HELPER FUNCTION: 'int blockSize(numBytes)'
// [10] FREE FUNCTION: 'operator new(size_t, bdlma::SequentialPool)'
// [11] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

}  // close unnamed namespace

//=============================================================================
//                       STANDARD BDE TEST DRIVER MACROS
//-----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

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
//                  GLOBAL VARIABLES / TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlma::SequentialPool     Obj;

typedef bsls::Alignment::Strategy Strat;

enum { k_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

enum { k_DEFAULT_SIZE = 256 };  // initial size of the buffer if none specified

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

bool globalDestructorInvoked = false;

class my_Class {
    // This object indicates when its destructor is called by setting
    // 'globalDestructorInvoked' to 'true'.

  public:
    ~my_Class()
        // Destroy this object and set 'globalDestructorInvoked' to 'true'.
    {
        globalDestructorInvoked = true;
    }
};

//-----------------------------------------------------------------------------

struct Block {
    // This type was copied from 'bdlma_infrequentdeleteblocklist.h' for
    // testing purposes.

    Block                               *d_next_p;
    bsls::AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
};

//=============================================================================
//                        STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static int blockSize(int numBytes)
    // Return the adjusted block size based on the specified 'numBytes' using
    // the calculation performed by the
    // 'bdlma::InfrequentDeleteBlockList::allocate' method.
{
    ASSERT(0 <= numBytes);

    if (numBytes) {
        numBytes += static_cast<int>(sizeof(Block)) - 1;
        numBytes &= ~(k_MAX_ALIGN - 1);
    }

    return numBytes;
}

//-----------------------------------------------------------------------------

static int calculateNextSize(int currSize, int size)
{
    ASSERT(0 < currSize);
    ASSERT(0 < size);

    if (0 == currSize) {
        return 0;                                                     // RETURN
    }

    do {
        currSize *= 2;
    } while (currSize < size);

    return currSize;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
///Example 1: Using 'bdlma::SequentialPool' for Efficient Allocations
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we define a container class, 'my_IntDoubleArray', that holds both
// 'int' and 'double' values.  The class can be implemented using two parallel
// arrays: one storing the type information, and the other storing pointers to
// the 'int' and 'double' values.  For efficient memory allocation, we can use
// a 'bdlma::SequentialPool' for memory allocation:
//..
    // my_intdoublearray.h

    class my_IntDoubleArray {
        // This class implements an efficient container for an array that
        // stores both 'int' and 'double' values.

        // DATA
        char  *d_typeArray_p;   // array indicating the type of corresponding
                                // values stored in 'd_valueArray_p'

        void **d_valueArray_p;  // array of pointers to the values stored

        int    d_length;        // number of values stored

        int    d_capacity;      // physical capacity of the type and value
                                // arrays

        bdlma::SequentialPool
               d_pool;          // sequential memory pool used to supply memory

      private:
        // PRIVATE MANIPULATORS
        void increaseSize();
            // Increase the capacity of the internal arrays used to store
            // elements added to this array by at least one element.

        // Not implemented:
        my_IntDoubleArray(const my_IntDoubleArray&);

      public:
        // TYPES
        enum Type { k_MY_INT, k_MY_DOUBLE };

        // CREATORS
        explicit my_IntDoubleArray(bslma::Allocator *basicAllocator = 0);
            // Create an 'int'-'double' array.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        ~my_IntDoubleArray();
            // Destroy this array and all elements held by it.

        // ...

        // MANIPULATORS
        void appendInt(int value);
            // Append the specified 'int' 'value' to this array.

        void appendDouble(double value);
            // Append the specified 'double' 'value' to this array.

        void removeAll();
            // Remove all elements from this array.

        // ...
    };
//..
// The use of a sequential pool and the 'release' method allows the 'removeAll'
// method to quickly deallocate memory of all elements:
//..
    // MANIPULATORS
    inline
    void my_IntDoubleArray::removeAll()
    {
        d_pool.release();
        d_length = 0;
    }
//..
// The sequential pool optimizes the allocation of memory by using
// dynamically-allocated buffers to supply memory.  This greatly reduces the
// amount of dynamic allocation needed:
//..
    // my_intdoublearray.cpp

    enum { k_INITIAL_SIZE = 1 };

    // PRIVATE MANIPULATORS
    void my_IntDoubleArray::increaseSize()
    {
        // Implementation elided.
        // ...
    }

    // CREATORS
    my_IntDoubleArray::my_IntDoubleArray(bslma::Allocator *basicAllocator)
    : d_length(0)
    , d_capacity(k_INITIAL_SIZE)
    , d_pool(basicAllocator)
    {
        d_typeArray_p  = static_cast<char *>(
                         d_pool.allocate(d_capacity * sizeof *d_typeArray_p));
        d_valueArray_p = static_cast<void **>(
                         d_pool.allocate(d_capacity * sizeof *d_valueArray_p));
    }
//..
// Note that in the destructor, all outstanding memory blocks are deallocated
// automatically when 'd_pool' is destroyed:
//..
    my_IntDoubleArray::~my_IntDoubleArray()
    {
        ASSERT(0 <= d_length);
        ASSERT(0 <= d_capacity);
        ASSERT(d_length <= d_capacity);
    }

    // MANIPULATORS
    void my_IntDoubleArray::appendInt(int value)
    {
        if (d_length >= d_capacity) {
            increaseSize();
        }

        int *item = static_cast<int *>(d_pool.allocate(sizeof *item));
        *item = value;

        d_typeArray_p[d_length]  = static_cast<char>(k_MY_INT);
        d_valueArray_p[d_length] = item;

        ++d_length;
    }

    void my_IntDoubleArray::appendDouble(double value)
    {
        if (d_length >= d_capacity) {
            increaseSize();
        }

        double *item = static_cast<double *>(d_pool.allocate(sizeof *item));
        *item = value;

        d_typeArray_p[d_length]  = static_cast<char>(k_MY_DOUBLE);
        d_valueArray_p[d_length] = item;

        ++d_length;
    }
//..
///Example 2: Implementing an Allocator Using 'bdlma::SequentialPool'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bslma::Allocator' is used throughout the interfaces of BDE components.
// Suppose we would like to create a fast allocator, 'my_FastAllocator', that
// allocates memory from a buffer in a similar fashion to
// 'bdlma::SequentialPool'.  'bdlma::SequentialPool' can be used directly to
// implement such an allocator.
//
// Note that the documentation for this class is simplified for this usage
// example.  Please see 'bdlma_sequentialallocator' for full documentation of a
// similar class.
//..
    class my_SequentialAllocator : public bslma::Allocator {
        // This class implements the 'bslma::Allocator' protocol to provide a
        // fast allocator of heterogeneous blocks of memory (of varying,
        // user-specified sizes) from dynamically-allocated internal buffers.

        // DATA
        bdlma::SequentialPool d_pool;  // memory manager for allocated memory
                                       // blocks

      public:
        // CREATORS
        explicit my_SequentialAllocator(bslma::Allocator *basicAllocator = 0);
            // Create an allocator for allocating memory blocks from
            // dynamically-allocated internal buffers.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        ~my_SequentialAllocator();
            // Destroy this allocator.  All memory allocated from this
            // allocator is released.

        // MANIPULATORS
        virtual void *allocate(size_type size);
            // Return the address of a contiguous block of memory of the
            // specified 'size' (in bytes).

        virtual void deallocate(void *address);
            // This method has no effect on the memory block at the specified
            // 'address' as all memory allocated by this allocator is managed.
            // The behavior is undefined unless 'address' was allocated by this
            // allocator, and has not already been deallocated.
    };

    // CREATORS
    inline
    my_SequentialAllocator::my_SequentialAllocator(
                                              bslma::Allocator *basicAllocator)
    : d_pool(basicAllocator)
    {
    }

    inline
    my_SequentialAllocator::~my_SequentialAllocator()
    {
        d_pool.release();
    }

    // MANIPULATORS
    inline
    void *my_SequentialAllocator::allocate(size_type size)
    {
        return d_pool.allocate(size);
    }

    inline
    void my_SequentialAllocator::deallocate(void *)
    {
    }
//..

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // As part of our overall allocator testing strategy, we will create three
    // test allocators.

    // Object Test Allocator.
    bslma::TestAllocator objectAllocator("Object Allocator",
                                         veryVeryVeryVerbose);

    // Default Test Allocator.
    bslma::TestAllocator defaultAllocator("Default Allocator",
                                          veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    // Global Test Allocator.
    bslma::TestAllocator globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // GLOBAL OPERATOR NEW TEST
        //
        // Concerns:
        //   That a 'bdlma::SequentialPool' can be used directly with
        //   'operator new'.
        //
        // Plan:
        //   Since 'bdlma::SequentialPool' is thoroughly tested at this point,
        //   we just need to make sure that 'new' forwards the memory request
        //   directly.
        //
        // Testing:
        //   operator new(size_t, bdlma::SequentialPool&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "GLOBAL OPERATOR NEW TEST" << endl
                                  << "========================" << endl;

        Obj mX(&objectAllocator);

        double *d = new(mX) double(3.0);
        (void)d;

        ASSERT(0 != objectAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == globalAllocator.numBytesInUse());

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // 'reserveCapacity' TEST
        //
        // Concerns:
        //   1) That if there is sufficient memory within the internal buffer,
        //      'reserveCapacity' should not trigger dynamic allocation.
        //
        //   2) That we can allocate at least the amount of bytes specified in
        //      'reserveCapacity' before triggering another dynamic allocation.
        //
        //   3) That 'reserveCapacity' can override the maximum buffer size
        //      parameter supplied to the pool at construction.
        //
        //   4) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   Create a 'bdlma::SequentialPool' using a test allocator and
        //   specify an initial size and maximum buffer size.
        //
        //   First, for concern 1, invoke 'reserveCapacity' with a size less
        //   than the initial size.  Allocate the same amount of memory, and
        //   verify, using the test allocator, that no new dynamic allocation
        //   is triggered.
        //
        //   Then, for concern 2, invoke 'reserveCapacity' with a size larger
        //   than the initial size.  Verify that the call triggers dynamic
        //   allocation, and that we can allocate the same amount of memory
        //   without triggering further dynamic allocation.
        //
        //   Then, for concern 3, invoke 'reserveCapacity' with a size
        //   larger than the maximum buffer size.  Repeat verification for
        //   concern 2.
        //
        //   Finally, for concern 4, verify that in appropriate build modes,
        //   defensive checks are triggered.
        //
        // Testing:
        //   void reserveCapacity(int numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'reserveCapacity' TEST" << endl
                                  << "======================" << endl;

        enum { k_INITIAL_SIZE = 64, k_MAX_BUFFER = k_INITIAL_SIZE * 4 };

        if (verbose) cout << "\nTesting that 'reserveCapacity' does not "
                             "trigger dynamic memory allocation." << endl;

        {
            Obj mX(k_INITIAL_SIZE, k_MAX_BUFFER, &objectAllocator);
            bsls::Types::Int64 numBytesUsed = objectAllocator.numBytesInUse();

            mX.reserveCapacity(k_INITIAL_SIZE / 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            mX.allocate(k_INITIAL_SIZE / 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            mX.reserveCapacity(k_INITIAL_SIZE * 2);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            mX.allocate(k_INITIAL_SIZE * 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            mX.reserveCapacity(k_MAX_BUFFER * 2);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            mX.allocate(k_MAX_BUFFER * 2);
            ASSERT(numBytesUsed = objectAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nTesting 'reserveCapacity' on a default "
                             "constructed pool." << endl;
        {
            Obj mX(&objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());

            mX.reserveCapacity(k_DEFAULT_SIZE);
            bsls::Types::Int64 numBytesUsed = objectAllocator.numBytesInUse();

            mX.allocate(k_DEFAULT_SIZE);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            mX.reserveCapacity(k_DEFAULT_SIZE * 4);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            mX.allocate(k_DEFAULT_SIZE * 4);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;

            {
                ASSERT_SAFE_PASS(mX.reserveCapacity( 1));

                ASSERT_SAFE_FAIL(mX.reserveCapacity( 0));
                ASSERT_SAFE_FAIL(mX.reserveCapacity(-1));
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // 'truncate' TEST
        //
        // Concerns:
        //   1. That 'truncate' reduces the amount of memory allocated to the
        //      specified 'newSize'.
        //
        //   2. That when 'truncate' fails, 'originalSize' is returned.
        //
        //   3) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   For concern 1, using the table-driven technique, create test
        //   vectors having the alignment strategy, initial allocation size,
        //   the new size, and expected offset.  First allocate memory of the
        //   initial allocation size, then truncate to the new size and
        //   allocate memory (1 byte) again.  Verify that the latest allocation
        //   matches the expected offset.
        //
        //   For concern 2, truncate the memory returned by the initial
        //   allocation, and verify that the return value is 'originalSize'.
        //
        //   For concern 3, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   int truncate(void *address, int originalSize, int newSize);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'truncate' TEST" << endl
                                  << "===============" << endl;

#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        if (verbose) cout << "\nTesting 'truncate'." << endl;

        static const struct {
            int   d_line;         // line number
            Strat d_strategy;     // alignment strategy
            int   d_initialSize;  // size of initial allocation request
            int   d_newSize;      // truncate size
            int   d_expOffset;    // expected memory offset
        } DATA[] = {
            // LINE     STRAT       INITIALSIZE   NEWSIZE   EXPOFFSET
            // ----     -----       -----------   -------   ---------

            // * - 'allocate' triggers dynamic memory allocation

            // NATURAL ALIGNMENT
            {  L_,      NAT,           1,           0,           0 },
            {  L_,      NAT,           1,           1,           1 },
            {  L_,      NAT,           2,           0,           0 },
            {  L_,      NAT,           2,           1,           1 },
            {  L_,      NAT,           2,           2,           2 },
            {  L_,      NAT,           3,           0,           0 },
            {  L_,      NAT,           3,           1,           1 },
            {  L_,      NAT,           3,           2,           2 },
            {  L_,      NAT,           3,           3,           3 },
            {  L_,      NAT,           8,           4,           4 },
            {  L_,      NAT,         511,         511,         511 },
            {  L_,      NAT,         512,           0,           0 },
            {  L_,      NAT,         512,         511,         511 },
            {  L_,      NAT,         512,         512,          -1 },  // *

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,           1,           0,           0 },
            {  L_,      MAX,           1,           1, k_MAX_ALIGN },
            {  L_,      MAX,           2,           0,           0 },
            {  L_,      MAX,           2,           1, k_MAX_ALIGN },
            {  L_,      MAX,           2,           2, k_MAX_ALIGN },
            {  L_,      MAX,           3,           0,           0 },
            {  L_,      MAX,           3,           1, k_MAX_ALIGN },
            {  L_,      MAX,           3,           2, k_MAX_ALIGN },
            {  L_,      MAX,           3,           3, k_MAX_ALIGN },
            {  L_,      MAX,           8,           4, k_MAX_ALIGN },
            {  L_,      MAX,         511,         511,          -1 },  // *
            {  L_,      MAX,         512,           0,           0 },
            {  L_,      MAX,         512,           1, k_MAX_ALIGN },
            {  L_,      MAX,         512,         511,          -1 },  // *
            {  L_,      MAX,         512,         512,          -1 },  // *

            // 1-BYTE ALIGNMENT
            {  L_,      BYT,           1,           0,           0 },
            {  L_,      BYT,           1,           1,           1 },
            {  L_,      BYT,           2,           0,           0 },
            {  L_,      BYT,           2,           1,           1 },
            {  L_,      BYT,           2,           2,           2 },
            {  L_,      BYT,           3,           0,           0 },
            {  L_,      BYT,           3,           1,           1 },
            {  L_,      BYT,           3,           2,           2 },
            {  L_,      BYT,           3,           3,           3 },
            {  L_,      BYT,           4,           0,           0 },
            {  L_,      BYT,           4,           1,           1 },
            {  L_,      BYT,           4,           2,           2 },
            {  L_,      BYT,           4,           3,           3 },
            {  L_,      BYT,           8,           4,           4 },
            {  L_,      BYT,         511,         511,         511 },
            {  L_,      BYT,         512,           0,           0 },
            {  L_,      BYT,         512,         511,         511 },
            {  L_,      BYT,         512,         512,          -1 },  // *
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE        = DATA[ti].d_line;         // line number
            const Strat STRAT       = DATA[ti].d_strategy;     // align. strat.
            const int   INITIALSIZE = DATA[ti].d_initialSize;  // initial size
            const int   NEWSIZE     = DATA[ti].d_newSize;      // new size
            const int   EXPOFFSET   = DATA[ti].d_expOffset;    // exp. offset

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL ALIGNMENT, ";
                }
                else if (STRAT == MAX) {
                    cout << "STRAT = MAXIMUM ALIGNMENT, ";
                }
                else {  // STRAT == BYTE
                    cout << "STRAT = 1-BYTE ALIGNMENT, ";
                }
                P_(INITIALSIZE) P_(NEWSIZE) P(EXPOFFSET)
            }

            Obj mX(STRAT, &objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());

            void *addr1 = mX.allocate(INITIALSIZE);
            ASSERT(0 != objectAllocator.numBytesInUse());

            bsls::Types::Int64 used = objectAllocator.numBytesInUse();

            mX.truncate(addr1, INITIALSIZE, NEWSIZE);
            ASSERT(used == objectAllocator.numBytesInUse());

            void *addr2 = mX.allocate(1);
            if (EXPOFFSET >= 0) {
                ASSERT(used == objectAllocator.numBytesInUse());
                LOOP4_ASSERT(LINE, EXPOFFSET, addr1, addr2,
                             (char *)addr1 + EXPOFFSET == (char *)addr2);
            }
            else {
                ASSERT(used < objectAllocator.numBytesInUse());
            }

            // Truncating previously allocated address should fail.
            if (EXPOFFSET >= 0
             && ((char *)addr1 + INITIALSIZE) != ((char *)addr2 + 1)) {
                int ret = mX.truncate(addr1, INITIALSIZE, NEWSIZE);
                LOOP2_ASSERT(INITIALSIZE, ret, INITIALSIZE == ret);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'0 != address'" << endl;
            {
                Obj mX;

                void *addr = mX.allocate(2);

                ASSERT_SAFE_PASS(mX.truncate(addr, 2, 1));

                ASSERT_SAFE_FAIL(mX.truncate(   0, 1, 0));
            }

            if (veryVerbose) cout << "\t'0 <= newSize'" << endl;
            {
                Obj mX;

                void *addr = mX.allocate(2);

                ASSERT_SAFE_PASS(mX.truncate(addr, 2,  1));
                ASSERT_SAFE_PASS(mX.truncate(addr, 1,  0));

                ASSERT_SAFE_FAIL(mX.truncate(addr, 0, -1));
            }

            if (veryVerbose) cout << "\t'newSize <= originalSize'" << endl;
            {
                Obj mX;

                void *addr = mX.allocate(2);

                ASSERT_SAFE_PASS(mX.truncate(addr, 2, 2));

                ASSERT_SAFE_FAIL(mX.truncate(addr, 2, 3));
            }
        }

#undef NAT
#undef MAX
#undef BYT

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // 'allocateAndExpand' TEST
        //
        // Concerns:
        //   1) That 'allocateAndExpand' returns the maximum amount of memory
        //      available for use without triggering another allocation.
        //
        //   2) That 'allocateAndExpand' returns the updated size of memory
        //      used.
        //
        //   3) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   For concerns 1-2, using the table-driven technique, create test
        //   vectors having the alignment strategy, initial memory offset, and
        //   expected memory used.  First allocate memory necessary for the
        //   initial memory offset, then allocate 1 byte using
        //   'allocateAndExpand'.  Verify the updated size is the same as the
        //   expected memory used.  Finally, invoke 'allocate' again and verify
        //   it triggers new dynamic memory allocation -- meaning
        //   'allocateAndExpand' did use up all available memory in the
        //   internal buffer.
        //
        //   For concern 4, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   void *allocateAndExpand(size_type *size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'allocateAndExpand' TEST" << endl
                                  << "========================" << endl;

#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        if (verbose) cout << "\nTesting 'expand'." << endl;

        static const struct {
            int   d_line;         // line number
            Strat d_strategy;     // alignment strategy
            int   d_initialSize;  // size of initial allocation request
            int   d_expused;      // expected memory used after 'expand'
        } DATA[] = {
            // LINE     STRAT       INITIALSIZE       EXPUSED
            // ----     -----       -----------       -------

            // NATURAL ALIGNMENT
            {  L_,      NAT,           1,   k_DEFAULT_SIZE * 2 - 1           },
            {  L_,      NAT,           2,   k_DEFAULT_SIZE * 2 - 2           },
            {  L_,      NAT,           3,   k_DEFAULT_SIZE * 2 - 3           },
            {  L_,      NAT,           4,   k_DEFAULT_SIZE * 2 - 4           },
            {  L_,      NAT,           7,   k_DEFAULT_SIZE * 2 - 7           },
            {  L_,      NAT,           8,   k_DEFAULT_SIZE * 2 - 8           },
            {  L_,      NAT,          15,   k_DEFAULT_SIZE * 2 - 15          },
            {  L_,      NAT,          16,   k_DEFAULT_SIZE * 2 - 16          },
            {  L_,      NAT,         100,   k_DEFAULT_SIZE * 2 - 100         },
            {  L_,      NAT,         510,   k_DEFAULT_SIZE * 2 - 510         },
            {  L_,      NAT,         511,   k_DEFAULT_SIZE * 2 - 511         },

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,           1,   k_DEFAULT_SIZE * 2 - k_MAX_ALIGN },
            {  L_,      MAX,           2,   k_DEFAULT_SIZE * 2 - k_MAX_ALIGN },
            {  L_,      MAX,           3,   k_DEFAULT_SIZE * 2 - k_MAX_ALIGN },
            {  L_,      MAX,           4,   k_DEFAULT_SIZE * 2 - k_MAX_ALIGN },
            {  L_,      MAX,           7,   k_DEFAULT_SIZE * 2 - k_MAX_ALIGN },
            {  L_,      MAX,           8,   k_DEFAULT_SIZE * 2 - k_MAX_ALIGN },
            {  L_,      MAX,          15,   k_DEFAULT_SIZE * 2 - 16          },
            {  L_,      MAX,          16,   k_DEFAULT_SIZE * 2 - 16          },
            {  L_,      MAX,         108,   k_DEFAULT_SIZE * 2 - 112         },

            // 1-BYTE ALIGNMENT
            {  L_,      BYT,           1,   k_DEFAULT_SIZE * 2 - 1           },
            {  L_,      BYT,           2,   k_DEFAULT_SIZE * 2 - 2           },
            {  L_,      BYT,           3,   k_DEFAULT_SIZE * 2 - 3           },
            {  L_,      BYT,           4,   k_DEFAULT_SIZE * 2 - 4           },
            {  L_,      BYT,           7,   k_DEFAULT_SIZE * 2 - 7           },
            {  L_,      BYT,           8,   k_DEFAULT_SIZE * 2 - 8           },
            {  L_,      BYT,          15,   k_DEFAULT_SIZE * 2 - 15          },
            {  L_,      BYT,          16,   k_DEFAULT_SIZE * 2 - 16          },
            {  L_,      BYT,         100,   k_DEFAULT_SIZE * 2 - 100         },
            {  L_,      BYT,         510,   k_DEFAULT_SIZE * 2 - 510         },
            {  L_,      BYT,         511,   k_DEFAULT_SIZE * 2 - 511         },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE        = DATA[ti].d_line;
            const Strat STRAT       = DATA[ti].d_strategy;
            const int   INITIALSIZE = DATA[ti].d_initialSize;
            const int   EXPUSED     = DATA[ti].d_expused;

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL ALIGNMENT, ";
                }
                else if (STRAT == MAX) {
                    cout << "STRAT = MAXIMUM ALIGNMENT, ";
                }
                else {  // STRAT == BYTE
                    cout << "STRAT = 1-BYTE ALIGNMENT, ";
                }
                P_(INITIALSIZE) P(EXPUSED)
            }

            Obj mX(STRAT, &objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == objectAllocator.numBlocksInUse());

            void *addr1 = mX.allocate(INITIALSIZE);
            ASSERT(blockSize(k_DEFAULT_SIZE * 2) ==
                                              objectAllocator.numBytesInUse());
            ASSERT(1 == objectAllocator.numBlocksInUse());

            bsls::Types::size_type size = 1;
            void *addr2 = mX.allocateAndExpand(&size);

            // Check for correct memory address.
            if (NAT == STRAT || BYT == STRAT) {
                ASSERT((char *)addr1 + INITIALSIZE == (char *)addr2);
            }
            else {
                int offset = bsls::AlignmentUtil::calculateAlignmentOffset(
                                                   (char *)addr1 + INITIALSIZE,
                                                   k_MAX_ALIGN);
                ASSERT((char *)addr1 + INITIALSIZE + offset == (char *)addr2);
            }

            // Check 'size' is updated correctly.
            ASSERT(EXPUSED == (int)size);

            // Check for no new allocations.
            ASSERT(blockSize(k_DEFAULT_SIZE * 2) ==
                                              objectAllocator.numBytesInUse());
            ASSERT(1 == objectAllocator.numBlocksInUse());

            // Check that new allocations causes dynamic memory allocations.
            mX.allocate(1);
            ASSERT(2 == objectAllocator.numBlocksInUse());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());

        {
            // No initial allocation, just allocate and expand directly.
            Obj mX(&objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == objectAllocator.numBlocksInUse());

            bsls::Types::size_type size = 1;
            mX.allocateAndExpand(&size);
            ASSERT(blockSize(k_DEFAULT_SIZE * 2) ==
                                              objectAllocator.numBytesInUse());
            ASSERT(1 == objectAllocator.numBlocksInUse());

            mX.allocate(1);
            ASSERT(2 == objectAllocator.numBlocksInUse());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'0 != size'" << endl;
            {
                Obj mX;
                bsls::Types::size_type size = 1;

                ASSERT_SAFE_PASS(mX.allocateAndExpand(&size));

                ASSERT_SAFE_FAIL(mX.allocateAndExpand(    0));
            }

            if (veryVerbose) cout << "\t'0 < *size'" << endl;
            {
                Obj mX;
                bsls::Types::size_type size = 1;

                ASSERT_SAFE_PASS(mX.allocateAndExpand(&size));

                size = 0;

                ASSERT_SAFE_FAIL(mX.allocateAndExpand(&size));
            }
        }

#undef NAT
#undef MAX
#undef BYT

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // DELETEOBJECT TEST
        //
        // Concerns:
        //   1) That both the 'deleteObject' and 'deleteObjectRaw' methods
        //      invoke the destructor of the object passed in.
        //
        // Plan:
        //   Since 'deleteObject' and 'deleteObjectRaw' do not deallocate
        //   memory, we just need to ensure that the destructor of the object
        //   passed in is invoked.
        //
        // Testing:
        //   void deleteObjectRaw(const TYPE *object);
        //   void deleteObject(const TYPE *object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DELETEOBJECT TEST" << endl
                                  << "=================" << endl;

        if (verbose) cout << "\nTesting 'deleteObject'." << endl;
        {
            Obj mX;

            void *addr = mX.allocate(sizeof(my_Class));
            my_Class *obj = new(addr) my_Class();

            globalDestructorInvoked = false;
            mX.deleteObject(obj);

            ASSERT(true == globalDestructorInvoked);
        }

        if (verbose) cout << "\nTesting 'deleteObjectRaw'." << endl;
        {
            Obj mX;

            void *addr = mX.allocate(sizeof(my_Class));
            my_Class *obj = new(addr) my_Class();

            globalDestructorInvoked = false;
            mX.deleteObjectRaw(obj);

            ASSERT(true == globalDestructorInvoked);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //   1) That all memory allocated from the allocator supplied at
        //      construction is deallocated after 'release'.
        //
        //   2) That subsequent allocation requests after invocation of the
        //      'release' method follow the specified growth and alignment
        //      strategies.
        //
        // Plan:
        //   Using the table-driven technique, create test vectors having an
        //   initial buffer size, an allocation request size, and a number of
        //   allocations to request.  For each alignment strategy, construct a
        //   sequential pool using a 'bslma::TestAllocator', and specifying the
        //   initial buffer size from the table and constant buffer growth.
        //   Then allocate a sequence of memory blocks, the number and size of
        //   which are specified in the table.  Then invoke 'release' and
        //   verify, using the test allocator, that there is no outstanding
        //   memory allocated.  Finally, allocate memory again and verify the
        //   alignment and growth strategies.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' TEST" << endl
                                  << "==============" << endl;

#define CON bsls::BlockGrowth::BSLS_CONSTANT

        const int TH = 64;  // threshold

        static const struct {
            int d_line;         // line number
            int d_bufSize;      // buffer size
            int d_requestSize;  // request size
            int d_numRequests;  // number of requests
        } DATA[] = {
            //LINE      BUFSIZE     REQUEST SIZE    # REQUESTS
            //----      -------     ------------    ----------

            {  L_,      1,          1,              2                 },
            {  L_,      1,          5,              2                 },
            {  L_,      1,          TH - 1,         2                 },
            {  L_,      1,          TH,             2                 },
            {  L_,      1,          TH + 1,         2                 },

            {  L_,      TH - 1,     1,              TH                },
            {  L_,      TH - 1,     5,              1 + (TH - 1) / 5  },
            {  L_,      TH - 1,     TH - 2,         2                 },
            {  L_,      TH - 1,     TH - 1,         2                 },
            {  L_,      TH - 1,     TH,             2                 },

            {  L_,      TH,         1,              TH + 1            },
            {  L_,      TH,         5,              1 + TH / 5        },
            {  L_,      TH,         TH - 1,         2                 },
            {  L_,      TH,         TH,             2                 },
            {  L_,      TH,         TH + 1,         2                 },

            {  L_,      TH + 1,     1,              TH + 2            },
            {  L_,      TH + 1,     5,              1 + (TH + 1) / 5  },
            {  L_,      TH + 1,     TH,             2                 },
            {  L_,      TH + 1,     TH + 1,         2                 },
            {  L_,      TH + 1,     TH + 2,         2                 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE    = DATA[i].d_line;
            const int BUFSIZE = DATA[i].d_bufSize;
            const int REQSIZE = DATA[i].d_requestSize;
            const int NUMREQ  = DATA[i].d_numRequests;

            const int MAXNUMREQ  = TH + 2;
            LOOP2_ASSERT(MAXNUMREQ, NUMREQ, MAXNUMREQ >= NUMREQ);
            if (MAXNUMREQ < NUMREQ) continue;

            // Try each test using maximum, natural, and 1-byte alignment.

            for (Strat strategy = bsls::Alignment::BSLS_MAXIMUM;
                       strategy <= bsls::Alignment::BSLS_BYTEALIGNED;
                       strategy = (Strat)(strategy + 1)) {

                bslma::TestAllocator ta(veryVeryVerbose);

                Obj mX(BUFSIZE, CON, strategy, &ta);
                ASSERT(1 == ta.numBlocksInUse());
                ASSERT(blockSize(BUFSIZE) == ta.numBytesInUse());

                // Make 'NUMREQ' requests for memory, recording how the
                // allocator was used after each request.

                for (int reqNum = 0; reqNum < NUMREQ; ++reqNum) {
                    void *returnAddr = mX.allocate(REQSIZE);
                    LOOP2_ASSERT(LINE, reqNum, returnAddr);
                    LOOP2_ASSERT(LINE, reqNum, ta.numBlocksInUse());

                    if (veryVerbose) {
                        P_(reqNum) P(returnAddr);
                    }
                }

                // Now call 'release' and verify that all memory is returned.

                mX.release();
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(0 == ta.numBytesInUse());

                if (1 == BUFSIZE) continue;

                if (verbose) cout << "\nTesting alignment and growth"
                                     " strategies." << endl;

                // Testing alignment.
                void *addr1 = mX.allocate(1);
                void *addr2 = mX.allocate(2);

                if (bsls::Alignment::BSLS_NATURAL == strategy) {
                    ASSERT((char *)addr1 +         2 == (char *)addr2);
                }
                else if (bsls::Alignment::BSLS_MAXIMUM == strategy) {
                    ASSERT((char *)addr1 + k_MAX_ALIGN == (char *)addr2);
                }
                else {  // bsls::Alignment::BSLS_BYTEALIGNED == strategy
                    ASSERT((char *)addr1 +         1 == (char *)addr2);
                }

                ASSERT(                     1 == ta.numBlocksInUse());
                ASSERT(    blockSize(BUFSIZE) == ta.numBytesInUse());
                mX.allocate(BUFSIZE);
                ASSERT(                     2 == ta.numBlocksInUse());
                ASSERT(2 * blockSize(BUFSIZE) == ta.numBytesInUse());
            }
        }

#undef CON

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'allocate' TEST:
        //   Note that this test alone is insufficient to thoroughly test the
        //   the 'allocate' method, as the constructors, which are not
        //   thoroughly tested yet, are used in this test.  However, a
        //   combination of both this test and test case 3 provides complete
        //   test coverage for both the constructors and the 'allocate' method.
        //
        // Concerns:
        //   1) If 'initialSize' is specified then all requests up to
        //      'initialSize' require no additional allocations.  Subsequent
        //      buffer resizes are constant or geometric based on the specified
        //      growth strategy.
        //
        //   2) All requests over a specified THRESHOLD are satisfied directly
        //      from the internal block list if the they cannot be satisfied by
        //      the pool's internal buffer.
        //
        //   3) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   For concerns 1 and 2, construct objects 'mV', 'mW', 'mX', and 'mY'
        //   with default, maximum, natural, and 1-byte alignment allocation
        //   strategies using the default constructor and the constructor
        //   taking an 'initialSize' and an alignment strategy.  Additionally
        //   pass a test allocator to the constructor to monitor the memory
        //   allocations by the pool.  Confirm the bytes allocated by the
        //   objects are as expected.
        //
        //   For concern 3, verify that, in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   void *allocate(size_type size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'allocate' TEST" << endl
                                  << "===============" << endl;

        const int DATA[]   = { 2, 5, 7, 8, 15, 16, 24, 31, 32, 33, 48,
                               63, 64, 65, 66, 127, 128, 129, 255, 256,
                               511, 512, 1023, 1024, 1025 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        Strat MAX = bsls::Alignment::BSLS_MAXIMUM;
        Strat NAT = bsls::Alignment::BSLS_NATURAL;
        Strat BYT = bsls::Alignment::BSLS_BYTEALIGNED;

        if (verbose) cout << "\nTesting constructor without initial size."
                          << endl;
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                bslma::TestAllocator ta(veryVeryVerbose),
                                     tb(veryVeryVerbose),
                                     tc(veryVeryVerbose),
                                     td(veryVeryVerbose);
                Obj mV(     &ta);
                Obj mW(MAX, &tb);
                Obj mX(NAT, &tc);
                Obj mY(BYT, &td);

                mV.allocate(SIZE);
                mW.allocate(SIZE);
                mX.allocate(SIZE);
                mY.allocate(SIZE);

                if (0 == SIZE) {
                    continue;
                }

                if (SIZE <= k_DEFAULT_SIZE * 2) {
                    LOOP2_ASSERT(i, SIZE,
                          blockSize(k_DEFAULT_SIZE * 2) == ta.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                          blockSize(k_DEFAULT_SIZE * 2) == tb.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                          blockSize(k_DEFAULT_SIZE * 2) == tc.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                          blockSize(k_DEFAULT_SIZE * 2) == td.numBytesInUse());
                }
                else {
                    int nextSize = calculateNextSize(k_DEFAULT_SIZE, SIZE);
                    LOOP4_ASSERT(i, SIZE, blockSize(nextSize),
                                 ta.numBytesInUse(),
                                 blockSize(nextSize) == ta.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                                 blockSize(nextSize) == tb.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                                 blockSize(nextSize) == tc.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                                 blockSize(nextSize) == td.numBytesInUse());
                }
            }
        }

        if (verbose) {
            cout << "\nTesting 'initialSize' constructor." << endl;
        }
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                const int INITIAL_SIZES[] = { SIZE - 1, SIZE, SIZE + 1,
                                              SIZE - 1, SIZE, SIZE + 1 };

#define GEO bsls::BlockGrowth::BSLS_GEOMETRIC
#define CON bsls::BlockGrowth::BSLS_CONSTANT

                const bsls::BlockGrowth::Strategy STRAT[] = { GEO, GEO, GEO,
                                                              CON, CON, CON };

#undef CON
#undef GEO

                const int NUM_INITIAL_SIZES = sizeof  INITIAL_SIZES
                                            / sizeof *INITIAL_SIZES;

                for (int j = 0; j < NUM_INITIAL_SIZES; ++j) {
                    const int INITIAL_SIZE = INITIAL_SIZES[j];
                    const bsls::BlockGrowth::Strategy STRATEGY = STRAT[j];

                    bslma::TestAllocator ta(veryVeryVerbose),
                                         tb(veryVeryVerbose),
                                         tc(veryVeryVerbose),
                                         td(veryVeryVerbose);

                    if (veryVerbose) {
                        P(INITIAL_SIZE) P(SIZE)
                    }

                    Obj mV(INITIAL_SIZE, STRATEGY,      &ta);
                    Obj mW(INITIAL_SIZE, STRATEGY, MAX, &tb);
                    Obj mX(INITIAL_SIZE, STRATEGY, NAT, &tc);
                    Obj mY(INITIAL_SIZE, STRATEGY, BYT, &td);

                    const bsls::Types::Int64 NA = ta.numBytesInUse();
                    const bsls::Types::Int64 NB = tb.numBytesInUse();
                    const bsls::Types::Int64 NC = tc.numBytesInUse();
                    const bsls::Types::Int64 ND = td.numBytesInUse();

                    mV.allocate(SIZE);
                    mW.allocate(SIZE);
                    mX.allocate(SIZE);
                    mY.allocate(SIZE);

                    if (SIZE <= bsl::abs(INITIAL_SIZE)) {
                        LOOP_ASSERT(i, NA == ta.numBytesInUse());
                        LOOP_ASSERT(i, NB == tb.numBytesInUse());
                        LOOP_ASSERT(i, NC == tc.numBytesInUse());
                        LOOP_ASSERT(i, ND == td.numBytesInUse());
                    }
                    else if (bsls::BlockGrowth::BSLS_GEOMETRIC == STRAT[j]) {
                        int nextSize = calculateNextSize(INITIAL_SIZE, SIZE);
                        LOOP3_ASSERT(i, NA + blockSize(nextSize),
                                     ta.numBytesInUse(),
                                     NA + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                        LOOP_ASSERT(i, NB + blockSize(nextSize)
                                                        == tb.numBytesInUse());
                        LOOP_ASSERT(i, NC + blockSize(nextSize)
                                                        == tc.numBytesInUse());
                        LOOP_ASSERT(i, ND + blockSize(nextSize)
                                                        == td.numBytesInUse());
                    }
                    else {
                        if (0 == INITIAL_SIZE && SIZE < k_DEFAULT_SIZE) {
                            LOOP_ASSERT(i, NA + blockSize(k_DEFAULT_SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, NB + blockSize(k_DEFAULT_SIZE)
                                                        == tb.numBytesInUse());
                            LOOP_ASSERT(i, NC + blockSize(k_DEFAULT_SIZE)
                                                        == tc.numBytesInUse());
                            LOOP_ASSERT(i, ND + blockSize(k_DEFAULT_SIZE)
                                                        == td.numBytesInUse());
                        }
                        else {
                            LOOP_ASSERT(i, NA + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, NB + blockSize(SIZE)
                                                        == tb.numBytesInUse());
                            LOOP_ASSERT(i, NC + blockSize(SIZE)
                                                        == tc.numBytesInUse());
                            LOOP_ASSERT(i, ND + blockSize(SIZE)
                                                        == td.numBytesInUse());
                        }
                    }
                }
            }
        }

        if (verbose)
            cout << "\nTesting 'initialSize' and 'maxBufferSize' constructor."
                 << endl;
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                if (veryVerbose) {
                    T_ P(SIZE)
                }

                const int INITIAL_SIZES[] = { SIZE - 1, SIZE, SIZE + 1,
                                              SIZE - 1, SIZE, SIZE + 1 };

#define GEO bsls::BlockGrowth::BSLS_GEOMETRIC
#define CON bsls::BlockGrowth::BSLS_CONSTANT

                const bsls::BlockGrowth::Strategy STRATEGIES[] = {
                                                              GEO, GEO, GEO,
                                                              CON, CON, CON };

#undef CON
#undef GEO

                const int NUM_INITIAL_SIZES = sizeof  INITIAL_SIZES
                                            / sizeof *INITIAL_SIZES;

                for (int j = 0; j < NUM_INITIAL_SIZES; ++j) {
                    const int INITIAL_SIZE = INITIAL_SIZES[j];
                    const bsls::BlockGrowth::Strategy STRATEGY = STRATEGIES[j];

                    bslma::TestAllocator ta(veryVeryVerbose),
                                         tb(veryVeryVerbose),
                                         tc(veryVeryVerbose),
                                         td(veryVeryVerbose);

                    if (veryVerbose) {
                        T_ T_ P_(INITIAL_SIZE) P(SIZE)
                    }

                    const int NUM_MAX_SIZES = 3;
                    int MAX_SIZES[NUM_MAX_SIZES];
                    MAX_SIZES[0] = INITIAL_SIZE * 2;
                    MAX_SIZES[1] = INITIAL_SIZE * 4;
                    MAX_SIZES[2] = INITIAL_SIZE * 8;

                    for (int k = 0; k < NUM_MAX_SIZES; ++k) {
                        const int MAX_SIZE = MAX_SIZES[k];

                        if (veryVerbose) {
                            T_ T_ T_ P(MAX_SIZE)
                        }

                        for (int m = 0; m < NUM_DATA; ++m) {
                            const int ALLOC_SIZE = DATA[m];

                            if (veryVerbose) {
                                T_ T_ T_ T_ P(ALLOC_SIZE)
                            }

                            Obj mV(INITIAL_SIZE, MAX_SIZE, STRATEGY,      &ta);
                            Obj mW(INITIAL_SIZE, MAX_SIZE, STRATEGY, MAX, &tb);
                            Obj mX(INITIAL_SIZE, MAX_SIZE, STRATEGY, NAT, &tc);
                            Obj mY(INITIAL_SIZE, MAX_SIZE, STRATEGY, BYT, &td);

                            const bsls::Types::Int64 NA = ta.numBytesInUse();
                            const bsls::Types::Int64 NB = tb.numBytesInUse();
                            const bsls::Types::Int64 NC = tc.numBytesInUse();
                            const bsls::Types::Int64 ND = td.numBytesInUse();

                            mV.allocate(ALLOC_SIZE);
                            mW.allocate(ALLOC_SIZE);
                            mX.allocate(ALLOC_SIZE);
                            mY.allocate(ALLOC_SIZE);

                            if (ALLOC_SIZE <= INITIAL_SIZE) {
                                LOOP_ASSERT(i, NA == ta.numBytesInUse());
                                LOOP_ASSERT(i, NB == tb.numBytesInUse());
                                LOOP_ASSERT(i, NC == tc.numBytesInUse());
                                LOOP_ASSERT(i, ND == td.numBytesInUse());
                            }
                            else if (bsls::BlockGrowth::BSLS_GEOMETRIC ==
                                                                    STRATEGY) {
                                if (ALLOC_SIZE < MAX_SIZE) {
                                    int nextSize = calculateNextSize(
                                                     INITIAL_SIZE, ALLOC_SIZE);
                                    LOOP_ASSERT(i, NA + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                                    LOOP_ASSERT(i, NB + blockSize(nextSize)
                                                        == tb.numBytesInUse());
                                    LOOP_ASSERT(i, NC + blockSize(nextSize)
                                                        == tc.numBytesInUse());
                                    LOOP_ASSERT(i, ND + blockSize(nextSize)
                                                        == td.numBytesInUse());
                                }
                                else {
                                    LOOP_ASSERT(i, NA + blockSize(ALLOC_SIZE)
                                                        == ta.numBytesInUse());
                                    LOOP_ASSERT(i, NB + blockSize(ALLOC_SIZE)
                                                        == tb.numBytesInUse());
                                    LOOP_ASSERT(i, NC + blockSize(ALLOC_SIZE)
                                                        == tc.numBytesInUse());
                                    LOOP_ASSERT(i, ND + blockSize(ALLOC_SIZE)
                                                        == td.numBytesInUse());
                                }
                            }
                            else {
                                LOOP_ASSERT(i, NA + blockSize(ALLOC_SIZE)
                                                        == ta.numBytesInUse());
                                LOOP_ASSERT(i, NB + blockSize(ALLOC_SIZE)
                                                        == tb.numBytesInUse());
                                LOOP_ASSERT(i, NC + blockSize(ALLOC_SIZE)
                                                        == tc.numBytesInUse());
                                LOOP_ASSERT(i, ND + blockSize(ALLOC_SIZE)
                                                        == td.numBytesInUse());
                            }
                        }
                    }
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;

            if (veryVerbose) cout << "\t'allocate(0 < size)'" << endl;
            {
                ASSERT_SAFE_PASS(mX.allocate(1));

                ASSERT_SAFE_FAIL(mX.allocate(0));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CTOR TEST
        //   Note that this test alone is insufficient to thoroughly test the
        //   constructors, as the untested 'allocate' method is used to
        //   verify that the buffer, alignment strategy, and allocator are
        //   properly passed to the pool at construction.  However, we cannot
        //   test 'allocate' first - as it requires the constructors.  Hence,
        //   it is a combination of both this test and test case 4 that
        //   provides complete test coverage for the constructor and the
        //   'allocate' method.
        //
        // Concerns:
        //   1) That when an allocator is not supplied, the currently installed
        //      default allocator is used.
        //
        //   2) That when an alignment strategy is not specified, natural
        //      alignment is used.
        //
        //   3) That when a growth strategy is not specified, geometric growth
        //      is used.
        //
        //   4) That when an initial size is not specified, the
        //      'bdlma::SequentialPool' does not allocate memory at
        //      construction.
        //
        //   5) That if an initial size is specified, the pool allocates the
        //      specified amount of memory at construction.
        //
        //   6) That the proper growth strategy, alignment strategy, and
        //      allocator are used if specified at construction.
        //
        //   7) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   TBD
        //
        //   In addition, verify that in appropriate build modes, defensive
        //   checks are triggered.
        //
        // Testing:
        //   bdlma::SequentialPool(bslma::Allocator *a = 0);
        //   bdlma::SequentialPool(GS g, bslma::Allocator *a = 0);
        //   bdlma::SequentialPool(AS a, bslma::Allocator *a = 0);
        //   bdlma::SequentialPool(GS g, AS a, bslma::Allocator *a = 0);
        //
        //   bdlma::SequentialPool(int i, bslma::Allocator *a = 0);
        //   bdlma::SequentialPool(int i, GS g, bslma::Allocator *a = 0);
        //   bdlma::SequentialPool(int i, AS a, bslma::Allocator *a = 0);
        //   bdlma::SequentialPool(int i, GS g, AS a, bslma::Allocator *a = 0);
        //
        //   bdlma::SequentialPool(int i, int m, bslma::Allocator *a = 0);
        //   bdlma::SequentialPool(int i, int m, GS g, bslma::Allocator *a= 0);
        //   bdlma::SequentialPool(int i, int m, AS a, bslma::Allocator *a= 0);
        //   bdlma::SequentialPool(int, int, GS g, AS a, *a= 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR TEST" << endl
                                  << "=========" << endl;

#define GEO bsls::BlockGrowth::BSLS_GEOMETRIC
#define CON bsls::BlockGrowth::BSLS_CONSTANT
#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        const int INITIAL_SIZE =  64;
        const int MAX_BUFFER   = 256;

        if (verbose) cout << "\nTesting default allocator." << endl;
        {
            int x = 0;

            ASSERT(0 == objectAllocator.numBlocksTotal());
            ASSERT(x == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            Obj mA;                                         mA.allocate(1);
            ASSERT(0   == objectAllocator.numBlocksTotal());
            ASSERT(++x == defaultAllocator.numBlocksTotal());
            ASSERT(0   == globalAllocator.numBlocksTotal());

            Obj mB(GEO);                                    mB.allocate(1);
            ASSERT(0   == objectAllocator.numBlocksTotal());
            ASSERT(++x == defaultAllocator.numBlocksTotal());
            ASSERT(0   == globalAllocator.numBlocksTotal());

            Obj mC(NAT);                                    mC.allocate(1);
            ASSERT(0   == objectAllocator.numBlocksTotal());
            ASSERT(++x == defaultAllocator.numBlocksTotal());
            ASSERT(0   == globalAllocator.numBlocksTotal());

            Obj mD(CON, MAX);                               mD.allocate(1);
            ASSERT(0   == objectAllocator.numBlocksTotal());
            ASSERT(++x == defaultAllocator.numBlocksTotal());
            ASSERT(0   == globalAllocator.numBlocksTotal());

            Obj mE(INITIAL_SIZE);                           mE.allocate(1);
            ASSERT(0   == objectAllocator.numBlocksTotal());
            ASSERT(++x == defaultAllocator.numBlocksTotal());
            ASSERT(0   == globalAllocator.numBlocksTotal());

            Obj mF(INITIAL_SIZE, CON);                      mF.allocate(1);
            ASSERT(0   == objectAllocator.numBlocksTotal());
            ASSERT(++x == defaultAllocator.numBlocksTotal());
            ASSERT(0   == globalAllocator.numBlocksTotal());

            Obj mG(INITIAL_SIZE, MAX);                      mG.allocate(1);
            ASSERT(0   == objectAllocator.numBlocksTotal());
            ASSERT(++x == defaultAllocator.numBlocksTotal());
            ASSERT(0   == globalAllocator.numBlocksTotal());

            Obj mH(INITIAL_SIZE, GEO, NAT);                 mH.allocate(1);
            ASSERT(0   == objectAllocator.numBlocksTotal());
            ASSERT(++x == defaultAllocator.numBlocksTotal());
            ASSERT(0   == globalAllocator.numBlocksTotal());

            Obj mI(INITIAL_SIZE, MAX_BUFFER);               mI.allocate(1);
            ASSERT(0   == objectAllocator.numBlocksTotal());
            ASSERT(++x == defaultAllocator.numBlocksTotal());
            ASSERT(0   == globalAllocator.numBlocksTotal());

            Obj mJ(INITIAL_SIZE, MAX_BUFFER, GEO);          mJ.allocate(1);
            ASSERT(0   == objectAllocator.numBlocksTotal());
            ASSERT(++x == defaultAllocator.numBlocksTotal());
            ASSERT(0   == globalAllocator.numBlocksTotal());

            Obj mK(INITIAL_SIZE, MAX_BUFFER, MAX);          mK.allocate(1);
            ASSERT(0   == objectAllocator.numBlocksTotal());
            ASSERT(++x == defaultAllocator.numBlocksTotal());
            ASSERT(0   == globalAllocator.numBlocksTotal());

            Obj mL(INITIAL_SIZE, MAX_BUFFER, CON, NAT);     mL.allocate(1);
            ASSERT(0   == objectAllocator.numBlocksTotal());
            ASSERT(++x == defaultAllocator.numBlocksTotal());
            ASSERT(0   == globalAllocator.numBlocksTotal());

            ASSERT(12 == x);  // 12 variations
        }
        ASSERT(0 == objectAllocator.numBlocksTotal());
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nTesting default alignment strategy." << endl;
        {
            void *addr1, *addr2;

            Obj mA;                                 addr1 = mA.allocate(1);
            addr2 = mA.allocate(2);     ASSERT((char *)addr1 + 2 == addr2);

            Obj mB(GEO);                            addr1 = mB.allocate(1);
            addr2 = mB.allocate(2);     ASSERT((char *)addr1 + 2 == addr2);

            Obj mC(INITIAL_SIZE);                   addr1 = mC.allocate(1);
            addr2 = mC.allocate(2);     ASSERT((char *)addr1 + 2 == addr2);

            Obj mD(INITIAL_SIZE, CON);              addr1 = mD.allocate(1);
            addr2 = mD.allocate(2);     ASSERT((char *)addr1 + 2 == addr2);

            Obj mE(INITIAL_SIZE, MAX_BUFFER);       addr1 = mE.allocate(1);
            addr2 = mE.allocate(2);     ASSERT((char *)addr1 + 2 == addr2);

            Obj mF(INITIAL_SIZE, MAX_BUFFER, GEO);  addr1 = mF.allocate(1);
            addr2 = mF.allocate(2);     ASSERT((char *)addr1 + 2 == addr2);
        }

        if (verbose) cout << "\nTesting default growth strategy." << endl;
        {
            {
                Obj mA;

                ASSERT(0 == defaultAllocator.numBytesInUse());
                void *addr1 = mA.allocate(k_DEFAULT_SIZE + 1);
                addr1 = mA.allocate(k_DEFAULT_SIZE + 1);  // triggers alloc.
                (void)addr1;

                ASSERT(blockSize(k_DEFAULT_SIZE * 2) +
                       blockSize(calculateNextSize(k_DEFAULT_SIZE * 2,
                                                   k_DEFAULT_SIZE + 1))
                                        == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mB(NAT);

                ASSERT(0 == defaultAllocator.numBytesInUse());
                void *addr1 = mB.allocate(k_DEFAULT_SIZE + 1);
                addr1 = mB.allocate(k_DEFAULT_SIZE + 1);  // triggers alloc.
                (void)addr1;

                ASSERT(blockSize(k_DEFAULT_SIZE * 2) +
                       blockSize(calculateNextSize(k_DEFAULT_SIZE * 2,
                                                   k_DEFAULT_SIZE + 1))
                                        == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mC(INITIAL_SIZE);

                // Check initial memory allocation.
                ASSERT(blockSize(INITIAL_SIZE) ==
                                             defaultAllocator.numBytesInUse());

                void *addr1 = mC.allocate(INITIAL_SIZE + 1);  // trigger
                (void)addr1;

                ASSERT(blockSize(INITIAL_SIZE) +
                       blockSize(calculateNextSize(INITIAL_SIZE,
                                                   INITIAL_SIZE + 1))
                                        == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mD(INITIAL_SIZE, NAT);

                // Check initial memory allocation.
                ASSERT(blockSize(INITIAL_SIZE) ==
                                             defaultAllocator.numBytesInUse());

                void *addr1 = mD.allocate(INITIAL_SIZE + 1);  // trigger
                (void)addr1;

                ASSERT(blockSize(INITIAL_SIZE) +
                       blockSize(calculateNextSize(INITIAL_SIZE,
                                                   INITIAL_SIZE + 1))
                                        == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mE(INITIAL_SIZE, MAX_BUFFER);

                // Check initial memory allocation.
                ASSERT(blockSize(INITIAL_SIZE) ==
                                             defaultAllocator.numBytesInUse());

                void *addr1 = mE.allocate(INITIAL_SIZE + 1);  // trigger
                (void)addr1;

                ASSERT(blockSize(INITIAL_SIZE) +
                       blockSize(calculateNextSize(INITIAL_SIZE,
                                                   INITIAL_SIZE + 1))
                                        == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mF(INITIAL_SIZE, MAX_BUFFER, MAX);

                // Check initial memory allocation.
                ASSERT(blockSize(INITIAL_SIZE) ==
                                             defaultAllocator.numBytesInUse());

                void *addr1 = mF.allocate(INITIAL_SIZE + 1);  // trigger
                (void)addr1;

                ASSERT(blockSize(INITIAL_SIZE) +
                       blockSize(calculateNextSize(INITIAL_SIZE,
                                                   INITIAL_SIZE + 1))
                                        == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nTesting that when initial size is not "
            " specified, there will be no allocation at construction." << endl;
        {
            ASSERT(0 == defaultAllocator.numBytesInUse());

            Obj mA;
            ASSERT(0 == defaultAllocator.numBytesInUse());

            Obj mB(MAX);
            ASSERT(0 == defaultAllocator.numBytesInUse());

            Obj mD(GEO);
            ASSERT(0 == defaultAllocator.numBytesInUse());

            Obj mF(GEO, MAX);
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nTesting specification of alignment strategy."
                          << endl;
        {
            static const struct {
                int   d_line;       // line number
                Strat d_strategy;   // alignment strategy
                int   d_allocSize;  // size of allocation request
                int   d_expOffset;  // expected address of second allocation
            } DATA[] = {
                // LINE     STRAT       ALLOCSIZE       EXPOFFSET
                // ----     -----       ---------       ---------

                // NATURAL ALIGNMENT
                {  L_,      NAT,         1,             1           },
                {  L_,      NAT,         2,             2           },
                {  L_,      NAT,         3,             1           },
                {  L_,      NAT,         4,             4           },
                {  L_,      NAT,         7,             1           },
                {  L_,      NAT,         8,             8           },
                {  L_,      NAT,        15,             1           },
                {  L_,      NAT,        16,             k_MAX_ALIGN },

                // MAXIMUM ALIGNMENT
                {  L_,      MAX,         1,             k_MAX_ALIGN },
                {  L_,      MAX,         2,             k_MAX_ALIGN },
                {  L_,      MAX,         3,             k_MAX_ALIGN },
                {  L_,      MAX,         4,             k_MAX_ALIGN },
                {  L_,      MAX,         7,             k_MAX_ALIGN },
                {  L_,      MAX,         8,             k_MAX_ALIGN },
                {  L_,      MAX,        15,             k_MAX_ALIGN },
                {  L_,      MAX,        16,             k_MAX_ALIGN },

                // 1-BYTE ALIGNMENT
                {  L_,      BYT,         1,             1           },
                {  L_,      BYT,         2,             1           },
                {  L_,      BYT,         3,             1           },
                {  L_,      BYT,         4,             1           },
                {  L_,      BYT,         7,             1           },
                {  L_,      BYT,         8,             1           },
                {  L_,      BYT,        15,             1           },
                {  L_,      BYT,        16,             1           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE      = DATA[ti].d_line;
                const Strat STRAT     = DATA[ti].d_strategy;
                const int   ALLOCSIZE = DATA[ti].d_allocSize;
                const int   EXPOFFSET = DATA[ti].d_expOffset;

                ASSERT(0 == objectAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numBytesInUse());
                ASSERT(0 == globalAllocator.numBytesInUse());

                Obj mX(STRAT, &objectAllocator);

                ASSERT(0 == objectAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numBytesInUse());
                ASSERT(0 == globalAllocator.numBytesInUse());

                char *addr1 = (char *)mX.allocate(1);
                char *addr2 = (char *)mX.allocate(ALLOCSIZE);

                ASSERT(0 != objectAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numBytesInUse());
                ASSERT(0 == globalAllocator.numBytesInUse());

                LOOP3_ASSERT(LINE, addr1 + EXPOFFSET, addr2,
                                                   addr1 + EXPOFFSET == addr2);
            }
            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nTesting specification of growth strategy."
                          << endl;
        {

            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());

            Obj mX(GEO, &objectAllocator);

            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());

            mX.allocate(k_DEFAULT_SIZE + 1);
            mX.allocate(k_DEFAULT_SIZE + 1);  // triggers allocation

            ASSERT(0 != objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());

            int totalSize = blockSize(k_DEFAULT_SIZE * 2) +
                            blockSize(calculateNextSize(k_DEFAULT_SIZE * 2,
                                                        k_DEFAULT_SIZE + 1));

            LOOP2_ASSERT(totalSize, objectAllocator.numBytesInUse(),
                                 totalSize == objectAllocator.numBytesInUse());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == globalAllocator.numBytesInUse());

        {
            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());

            Obj mX(CON, &objectAllocator);

            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());

            mX.allocate(k_DEFAULT_SIZE / 2 + 1);
            mX.allocate(k_DEFAULT_SIZE / 2 + 1);  // triggers allocation

            ASSERT(0 != objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());

            int totalSize = blockSize(k_DEFAULT_SIZE) +
                            blockSize(k_DEFAULT_SIZE);

            LOOP2_ASSERT(totalSize, objectAllocator.numBytesInUse(),
                                 totalSize == objectAllocator.numBytesInUse());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == globalAllocator.numBytesInUse());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'Obj(i, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1));

                ASSERT_SAFE_FAIL(Obj( 0));
                ASSERT_SAFE_FAIL(Obj(-1));
            }

            if (veryVerbose) cout << "\t'Obj(i, GS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, CON));

                ASSERT_SAFE_FAIL(Obj( 0, CON));
                ASSERT_SAFE_FAIL(Obj(-1, CON));
            }

            if (veryVerbose) cout << "\t'Obj(i, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, MAX));

                ASSERT_SAFE_FAIL(Obj( 0, MAX));
                ASSERT_SAFE_FAIL(Obj(-1, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(i, GS, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, CON, MAX));

                ASSERT_SAFE_FAIL(Obj( 0, CON, MAX));
                ASSERT_SAFE_FAIL(Obj(-1, CON, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1,  8));

                ASSERT_SAFE_FAIL(Obj( 0,  8));
                ASSERT_SAFE_FAIL(Obj(-1,  8));

                ASSERT_SAFE_PASS(Obj( 2,  2));

                ASSERT_SAFE_FAIL(Obj( 2,  1));
                ASSERT_SAFE_FAIL(Obj( 2, -2));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, GS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1,  8, CON));

                ASSERT_SAFE_FAIL(Obj( 0,  8, CON));
                ASSERT_SAFE_FAIL(Obj(-1,  8, CON));

                ASSERT_SAFE_PASS(Obj( 2,  2, CON));

                ASSERT_SAFE_FAIL(Obj( 2,  1, CON));
                ASSERT_SAFE_FAIL(Obj( 2, -2, CON));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1,  8, MAX));

                ASSERT_SAFE_FAIL(Obj( 0,  8, MAX));
                ASSERT_SAFE_FAIL(Obj(-1,  8, MAX));

                ASSERT_SAFE_PASS(Obj( 2,  2, MAX));

                ASSERT_SAFE_FAIL(Obj( 2,  1, MAX));
                ASSERT_SAFE_FAIL(Obj( 2, -2, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, GS, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1,  8, CON, MAX));

                ASSERT_SAFE_FAIL(Obj( 0,  8, CON, MAX));
                ASSERT_SAFE_FAIL(Obj(-1,  8, CON, MAX));

                ASSERT_SAFE_PASS(Obj( 2,  2, CON, MAX));

                ASSERT_SAFE_FAIL(Obj( 2,  1, CON, MAX));
                ASSERT_SAFE_FAIL(Obj( 2, -2, CON, MAX));
            }
        }

#undef GEO
#undef CON
#undef NAT
#undef MAX
#undef BYT

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // HELPER FUNCTION TEST
        //
        // Concerns:
        //   That the size returned by 'blockSize' matches the amount of bytes
        //   allocated by the 'bdlma::InfrequentDeleteBlockList::allocate'
        //   method.
        //
        // Plan:
        //   Create a 'bdlma::InfrequentDeleteBlockList' object initialized
        //   with a test allocator.  Invoke both the 'blockSize' function and
        //   the 'bdlma::InfrequentDeleteBlockList::allocate' method with
        //   varying memory sizes, and verify that the sizes returned by
        //   'blockSize' are equal to the memory request sizes recorded by the
        //   allocator.
        //
        // Testing:
        //   HELPER FUNCTION: 'int blockSize(numBytes)'
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER FUNCTION TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "\nTesting 'blockSize'." << endl;

        const int DATA[] = { 0, 1, 5, 12, 24, 64, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator a(veryVeryVerbose);
        bdlma::InfrequentDeleteBlockList bl(&a);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            int blkSize = blockSize(SIZE);
            bl.allocate(SIZE);

            // If the first 'SIZE' is 0, the allocator's 'allocate' is never
            // called; thus, 'lastAllocateddSize' will return -1 instead of 0.

            const int EXP = i || SIZE
                          ? static_cast<int>(a.lastAllocatedNumBytes())
                          : 0;

            if (veryVerbose) { T_ P_(SIZE); T_ P_(blkSize); T_ P(EXP); }
            LOOP_ASSERT(i, EXP == blkSize);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) That a 'bdlma::SequentialPool' can be created and destroyed.
        //
        //   2) That 'allocate' returns a block of memory having the specified
        //      size and expected alignment.
        //
        //   3) That 'allocate' does not always causes dynamic allocation
        //      (i.e., the pool manages an internal buffer of memory)
        //
        //   4) That 'allocate' returns a block of memory even when the
        //      the allocation request exceeds the initial size of the internal
        //      buffer.
        //
        //   5) Destruction of the pool releases all managed memory.
        //
        // Plan:
        //   First, initialize a 'bdlma::SequentialPool' with a
        //   'bslma::TestAllocator' (concern 1).  Then, allocate a block of
        //   memory, and verify that it comes from the test allocator.
        //   Allocate another block of memory, and verify that no dynamic
        //   allocation is triggered (concern 3).  Verify the alignment and
        //   size of the first allocation by checking the address of the
        //   second allocation (concern 2).
        //
        //   Then, allocate a large block of memory and verify the memory comes
        //   from the test allocator (concern 4).  Finally, destroy the pool
        //   and check that all allocated memory is deallocated (concern 5).
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        enum { k_ALLOC_SIZE1 = 4, k_ALLOC_SIZE2 = 8, k_ALLOC_SIZE3 = 1024 };

        {
            if (verbose) cout << "\nTesting construction of pool." << endl;
            Obj mX(&objectAllocator);

            // Make sure no memory comes from the object, default, and global
            // allocators.
            ASSERT(0 == objectAllocator.numBlocksTotal());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            if (verbose) cout << "\nTesting allocation." << endl;
            void *addr1 = mX.allocate(k_ALLOC_SIZE1);

            ASSERT(0 != objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            bsls::Types::Int64 oldNumBytesInUse =
                                               objectAllocator.numBytesInUse();

            if (verbose) cout << "\nTesting internal buffering." << endl;
            void *addr2 = mX.allocate(k_ALLOC_SIZE2);

            ASSERT(oldNumBytesInUse == objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            if (verbose) cout << "\nTesting alignment strategy." << endl;

            // Check for alignment and size of first allocation.
            ASSERT((char *)addr1 + 8 == (char *)addr2);

            if (verbose) cout << "\nTesting large allocation." << endl;
            void *addr3 = mX.allocate(k_ALLOC_SIZE3);
            (void)addr3;

            ASSERT(oldNumBytesInUse < objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());
        }

        if (verbose) cout << "\nTesting destruction." << endl;
        ASSERT(0 == objectAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

    } break;
    default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
