// bdlma_bufferedsequentialpool.t.cpp                                 -*-C++-*-
#include <bdlma_bufferedsequentialpool.h>

#include <bdlma_bufferimputil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignmentutil.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// A 'bdlma::BufferedSequentialPool' is a mechanism (i.e., having state but no
// value) that is used as a memory manager to manage an external buffer and
// dynamically allocated memory (if the external buffer is full).  The primary
// concern is that 'bdlma::BufferedSequentialPool' returns memory from an
// appropriate source (the external buffer supplied at construction, or a
// dynamically allocated buffer if the external buffer is full), and respects
// the appropriate alignment strategy (also specified at construction).
//
// Because 'bdlma::BufferedSequentialPool' does not have any accessors, this
// test driver verifies the correctness of the pool's allocations *indirectly*
// through the use of two consecutive allocations -- where the first allocation
// tests for the correctness of 'allocate', and the second verifies the size of
// the first allocation and its memory alignment.
//
// Finally, we make heavy use of the 'bslma::TestAllocator' to ensure that:
//
// (1) When the external buffer supplied at construction is full, memory
//     allocation is satisfied using a dynamic buffer allocated by the
//     allocator supplied at the pool's construction.
//
// (2) When 'release' is invoked, all memory managed by the pool is
//     deallocated.
//
// (3) When the pool is destroyed, all managed memory is deallocated.
//-----------------------------------------------------------------------------
// // CREATORS
// [ 3] bdlma::BufferedSequentialPool(*buf, sz, *a = 0);
// [ 3] bdlma::BufferedSequentialPool(*buf, sz, GS, *a = 0);
// [ 3] bdlma::BufferedSequentialPool(*buf, sz, AS, *a = 0);
// [ 3] bdlma::BufferedSequentialPool(*buf, sz, GS, AS, *a = 0);
//
// [ 3] bdlma::BufferedSequentialPool(*buf, sz, max, *a = 0);
// [ 3] bdlma::BufferedSequentialPool(*buf, sz, max, GS, *a = 0);
// [ 3] bdlma::BufferedSequentialPool(*buf, sz, max, AS, *a = 0);
// [ 3] bdlma::BufferedSequentialPool(*buf, sz, max, GS, AS, *a = 0);
//
// [ 7] ~bdlma::BufferedSequentialPool();
//
// // MANIPULATORS
// [ 4] void *allocate(size_type size);
// [ 6] void deleteObjectRaw(const TYPE *object);
// [ 6] void deleteObject(const TYPE *object);
// [ 5] void release();
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] HELPER FUNCTION: 'int blockSize(numBytes)'
// [ 8] FREE FUNCTION: 'operator new(size_t, bdlma::BufferedSequentialPool)'
// [ 9] USAGE TEST

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL VARIABLES / TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlma::BufferedSequentialPool Obj;

typedef bsls::Alignment::Strategy     Strat;

enum { k_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

// On Windows, when an 8-byte aligned object is created on the stack, it
// actually gets aligned on a 4-byte boundary.  To work around this, create a
// static buffer instead.

enum { k_BUFFER_SIZE = 256 };
static bsls::AlignedBuffer<k_BUFFER_SIZE> bufferStorage;

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
///Example 1: Using 'bdlma::BufferedSequentialPool' for Efficient Allocations
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we define a container class, 'my_BufferedIntDoubleArray', that holds
// both 'int' and 'double' values.  The class can be implemented using two
// parallel arrays: one storing the type information, and the other storing
// pointers to the 'int' and 'double' values.  Furthermore, if we can
// approximate the amount of memory needed, we can use a
// 'bdlma::BufferedSequentialPool' for memory allocation for maximum
// efficiency:
//..
    // my_bufferedintdoublearray.h

    class my_BufferedIntDoubleArray {
        // This class implements an efficient container for an array that
        // stores both 'int' and 'double' values.

        // DATA
        char  *d_typeArray_p;   // array indicating the type of corresponding
                                // values stored in 'd_valueArray_p'

        void **d_valueArray_p;  // array of pointers to the values stored

        int    d_length;        // number of values stored

        int    d_capacity;      // physical capacity of the type and value
                                // arrays

        bdlma::BufferedSequentialPool
               d_pool;          // buffered sequential memory pool used to
                                // supply memory

      private:
        // PRIVATE MANIPULATORS
        void increaseCapacity();
            // Increase the capacity of the internal arrays used to store
            // elements added to this array by at least one element.

        // Not implemented:
        my_BufferedIntDoubleArray(const my_BufferedIntDoubleArray&);

      public:
        // TYPES
        enum Type { k_MY_INT, k_MY_DOUBLE };

        // CREATORS
        my_BufferedIntDoubleArray(char             *buffer,
                                  int               size,
                                  bslma::Allocator *basicAllocator = 0);
            // Create a fast 'int'-'double' array that initially allocates
            // memory sequentially from the specified 'buffer' having the
            // specified 'size' (in bytes).  Optionally specify a
            // 'basicAllocator' used to supply memory if 'buffer' capacity is
            // exceeded.  If 'basicAllocator' is 0, the currently installed
            // default allocator is used.

        ~my_BufferedIntDoubleArray();
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
// The use of a buffered sequential pool and the 'release' method allows the
// 'removeAll' method to quickly deallocate memory of all elements:
//..
    // MANIPULATORS
    inline
    void my_BufferedIntDoubleArray::removeAll()
    {
        d_pool.release();  // *very* efficient if 'd_pool' has not exhausted
                           // the buffer supplied at construction

        d_length = 0;
    }
//..
// The buffered sequential pool optimizes the allocation of memory by using a
// buffer supplied at construction.  As described in the "DESCRIPTION" section,
// the need for *all* dynamic memory allocations are eliminated provided that
// the buffer is not exhausted.  The pool provides maximal memory allocation
// efficiency:
//..
    // my_bufferedintdoublearray.cpp

    enum { k_INITIAL_SIZE = 1 };

    // PRIVATE MANIPULATORS
    void my_BufferedIntDoubleArray::increaseCapacity()
    {
        // Implementation elided.
        // ...
    }

    // CREATORS
    my_BufferedIntDoubleArray::my_BufferedIntDoubleArray(
                                              char             *buffer,
                                              int               size,
                                              bslma::Allocator *basicAllocator)
    : d_length(0)
    , d_capacity(k_INITIAL_SIZE)
    , d_pool(buffer, size, basicAllocator)
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
    my_BufferedIntDoubleArray::~my_BufferedIntDoubleArray()
    {
        ASSERT(0 <= d_length);
        ASSERT(0 <= d_capacity);
        ASSERT(d_length <= d_capacity);
    }

    // MANIPULATORS
    void my_BufferedIntDoubleArray::appendInt(int value)
    {
        if (d_length >= d_capacity) {
            increaseCapacity();
        }

        int *item = static_cast<int *>(d_pool.allocate(sizeof *item));
        *item = value;

        d_typeArray_p[d_length]  = static_cast<char>(k_MY_INT);
        d_valueArray_p[d_length] = item;

        ++d_length;
    }

    void my_BufferedIntDoubleArray::appendDouble(double value)
    {
        if (d_length >= d_capacity) {
            increaseCapacity();
        }

        double *item = static_cast<double *>(d_pool.allocate(sizeof *item));
        *item = value;

        d_typeArray_p[d_length]  = static_cast<char>(k_MY_DOUBLE);
        d_valueArray_p[d_length] = item;

        ++d_length;
    }
//..
///Example 2: Implementing an Allocator Using 'bdlma::BufferedSequentialPool'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bslma::Allocator' is used throughout the interfaces of BDE components.
// Suppose we would like to create a fast allocator, 'my_FastAllocator', that
// allocates memory from a buffer in a similar fashion to
// 'bdlma::BufferedSequentialPool'.  'bdlma::BufferedSequentialPool' can be
// used directly to implement such an allocator.
//
// Note that the documentation for this class is simplified for this usage
// example.  Please see 'bdlma_bufferedsequentialallocator' for full
// documentation of a similar class.
//..
    class my_FastAllocator : public bslma::Allocator {
        // This class implements the 'bslma::Allocator' protocol to provide a
        // fast allocator of heterogeneous blocks of memory (of varying,
        // user-specified sizes) from an external buffer whose address and size
        // are supplied at construction.

        // DATA
        bdlma::BufferedSequentialPool d_pool;  // memory manager for allocated
                                               // memory blocks

        // CREATORS
        my_FastAllocator(char             *buffer,
                         int               size,
                         bslma::Allocator *basicAllocator = 0);
            // Create an allocator for allocating memory blocks from the
            // specified external 'buffer' of the specified 'size' (in bytes).
            // Optionally specify a 'basicAllocator' used to supply memory
            // should the capacity of 'buffer' be exhausted.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.

        ~my_FastAllocator();
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
    my_FastAllocator::my_FastAllocator(char             *buffer,
                                       int               size,
                                       bslma::Allocator *basicAllocator)
    : d_pool(buffer, size, basicAllocator)
    {
    }

    inline
    my_FastAllocator::~my_FastAllocator()
    {
        d_pool.release();
    }

    // MANIPULATORS
    inline
    void *my_FastAllocator::allocate(size_type size)
    {
        return d_pool.allocate(size);
    }

    inline
    void my_FastAllocator::deallocate(void *)
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

    // Object Test Allocator
    bslma::TestAllocator objectAllocator("Object Allocator",
                                         veryVeryVeryVerbose);

    // Default Test Allocator
    bslma::TestAllocator defaultAllocator("Default Allocator",
                                          veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    // Global Test Allocator
    bslma::TestAllocator globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 9: {
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
      case 8: {
        // --------------------------------------------------------------------
        // GLOBAL OPERATOR NEW TEST
        //
        // Concerns:
        //   That a 'bdlma::BufferedSequentialPool' can be used directly with
        //   'operator new'.
        //
        // Plan:
        //   Since 'bdlma::BufferedSequentialPool' is thoroughly tested at this
        //   point, we just need to make sure that 'new' forwards the memory
        //   request directly.
        //
        // Testing:
        //   operator new(size_t, bdlma::BufferedSequentialPool&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "GLOBAL OPERATOR NEW TEST" << endl
                                  << "========================" << endl;

        char *buffer = bufferStorage.buffer();

        Obj mX(buffer, k_BUFFER_SIZE);

        double *d = new(mX) double(3.0);

        ASSERT(buffer    <= (char *)d);
        ASSERT((char *)d <  buffer + k_BUFFER_SIZE);

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // DTOR TEST
        //
        // Concerns:
        //   1) That the previously managed buffer is not changed in any way
        //      after destruction of the pool.
        //
        //   2) That all memory allocated from the allocator supplied at
        //      construction is deallocated after destruction of the pool.
        //
        // Plan:
        //   For concern 1, dynamically allocate a buffer using a test
        //   allocator.  Then, set all bytes in the buffer to '0xA', and
        //   initialize a buffered sequential pool using the buffer and the
        //   same test allocator.  Finally, destroy the pool, and verify that
        //   the bytes in the first buffer remain '0xA' and the buffer is not
        //   deallocated.
        //
        //   For concern 2, construct a buffered sequential pool using a
        //   'bslma::TestAllocator', then allocate sufficient memory such that
        //   the buffer runs out and the allocator is used.  Finally, destroy
        //   the pool, and verify, using the test allocator, that there is
        //   no outstanding memory allocated.
        //
        // Testing:
        //   ~bdlma::BufferedSequentialPool();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DTOR TEST" << endl
                                  << "=========" << endl;

        if (verbose) cout << "\nTesting managed buffer is not affected after "
                             "destruction." << endl;
        {
            char bufferRef[k_BUFFER_SIZE];
            bsls::Types::Int64 total = 0;

            ASSERT(0 == objectAllocator.numBlocksInUse());
            char *buffer = (char *)objectAllocator.allocate(k_BUFFER_SIZE);
            total = objectAllocator.numBlocksInUse();

            {
                memset(buffer,    0xA, k_BUFFER_SIZE);
                memset(bufferRef, 0xA, k_BUFFER_SIZE);

                Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);

                ASSERT(total == objectAllocator.numBlocksInUse());

                // Allocate some memory.
                mX.allocate(1);
                mX.allocate(16);
                ASSERT(total == objectAllocator.numBlocksInUse());
            }
            ASSERT(total == objectAllocator.numBlocksInUse());
            ASSERT(0 == memcmp(buffer, bufferRef, k_BUFFER_SIZE));

            objectAllocator.deallocate(buffer);
            ASSERT(0 == objectAllocator.numBlocksInUse());
        }

        if (verbose) cout << "\nTesting allocated memory is deallocated after"
                             " destruction." << endl;
        {
            char buffer[k_BUFFER_SIZE];

            ASSERT(0 == objectAllocator.numBlocksInUse());

            Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksInUse());

            // Allocate some memory such that memory is allocated from the test
            // allocator.
            mX.allocate(k_BUFFER_SIZE + 1);
            mX.allocate(1);
            mX.allocate(16);
            ASSERT(0 != objectAllocator.numBlocksInUse());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());

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

        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting 'deleteObject'." << endl;
        {
            Obj mX(buffer, k_BUFFER_SIZE);

            void *addr = mX.allocate(sizeof(my_Class));
            my_Class *obj = new(addr) my_Class();

            globalDestructorInvoked = false;
            mX.deleteObject(obj);

            ASSERT(true == globalDestructorInvoked);
        }

        if (verbose) cout << "\nTesting 'deleteObjectRaw'." << endl;
        {
            Obj mX(buffer, k_BUFFER_SIZE);

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
        //   1) That 'release' has no effect on the previously managed buffer.
        //
        //   2) That all memory allocated from the allocator supplied at
        //      construction is deallocated after 'release'.
        //
        //   3) That subsequent allocation requests after invocation of the
        //      'release' method are satisfied by the buffer supplied at
        //      construction.
        //
        // Plan:
        //   For concern 1, dynamically allocate a buffer using a test
        //   allocator.  Then, set all bytes in the buffer to '0xA', and
        //   initialize a buffered sequential pool using the buffer and the
        //   same test allocator.  Finally, invoke 'release' and verify that
        //   the bytes in the first buffer remain '0xA' and the buffer is not
        //   deallocated.
        //
        //   For concerns 2 and 3, construct a buffered sequential pool using a
        //   'bslma::TestAllocator', then allocate sufficient memory such that
        //   the buffer runs out and the allocator is used.  Finally, invoke
        //   'release' and verify, using the test allocator, that there is
        //   no outstanding memory allocated.  Then, allocate memory again and
        //   verify memory comes from the buffer.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' TEST" << endl
                                  << "==============" << endl;

        if (verbose) cout << "\nTesting managed buffer is not affected after "
                             "'release'." << endl;

        {
            ASSERT(0 == objectAllocator.numBlocksInUse());

            char *buffer = (char *)objectAllocator.allocate(k_BUFFER_SIZE);
            char bufferRef[k_BUFFER_SIZE];

            bsls::Types::Int64 total = objectAllocator.numBlocksInUse();

            memset(buffer,    0xA, k_BUFFER_SIZE);
            memset(bufferRef, 0xA, k_BUFFER_SIZE);

            Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);

            ASSERT(total == objectAllocator.numBlocksInUse());

            // Allocate some memory.
            mX.allocate(1);
            mX.allocate(16);
            ASSERT(total == objectAllocator.numBlocksInUse());

            // Release all memory.
            mX.release();
            ASSERT(total == objectAllocator.numBlocksInUse());

            ASSERT(0 == memcmp(buffer, bufferRef, k_BUFFER_SIZE));

            objectAllocator.deallocate(buffer);
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());

        if (verbose) cout << "\nTesting allocated memory is deallocated after"
                             " 'release'." << endl;
        {
            char buffer[k_BUFFER_SIZE];

            ASSERT(0 == objectAllocator.numBlocksInUse());

            Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksInUse());

            // Allocate some memory such that memory is allocated from the test
            // allocator.
            mX.allocate(k_BUFFER_SIZE + 1);
            mX.allocate(1);
            mX.allocate(16);

            ASSERT(0 != objectAllocator.numBlocksInUse());

            // Release all memory.
            mX.release();

            ASSERT(0 == objectAllocator.numBlocksInUse());

            if (verbose) cout << "\nTesting subsequent allocations come"
                                 " first from the initial buffer." << endl;
            void *addr = mX.allocate(16);

            ASSERT(&buffer[0] <= addr);
            ASSERT(&buffer[0] + k_BUFFER_SIZE > addr);
            ASSERT(0 == objectAllocator.numBlocksInUse());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'allocate' TEST
        //   Note that this test alone is insufficient to thoroughly test the
        //   the 'allocate' method, as the constructors, which are not
        //   thoroughly tested yet, are used in this test.  However, a
        //   combination of both this test and test case 3 provides complete
        //   test coverage for both the constructors and the 'allocate' method.
        //
        // Concerns:
        //   1) A 'bdlma::BufferedSequentialPool' takes an external buffer
        //      supplied at construction.  This buffer is used for allocations
        //      until it runs out of memory.  Further allocations use the
        //      internal block list.
        //
        //   2) Due to the need to return aligned memory, the user supplied
        //      buffer may have sufficient additional memory to fulfill the
        //      request, but be unable to do so.  In these cases, the rest of
        //      the memory in the static buffer should be discarded and new
        //      memory supplied from the internal block list.
        //
        //   3) If the requested memory exceeds the amount of free memory in
        //      the static buffer, memory should be supplied from the internal
        //      block list.  Additional free memory in the buffer is no longer
        //      used.
        //
        //   4) The 'maxBufferSize' constructor argument caps the internal
        //      buffer growth during allocation.
        //
        //   5) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   1) Supply an aligned static buffer with a constant amount of
        //      memory (64 bytes).  Any memory requests that do not exceed 64
        //      bytes should be allocated from the static buffer.  All values
        //      near to 64 are tested.  Memory allocations less than or equal
        //      to 64 bytes will not cause the internal block list to supply
        //      memory.  All values above 64 will.
        //
        //   2) Memory is requested such that there is still free memory in the
        //      static buffer, but when memory is aligned correctly there is
        //      not enough memory to supply the requested amount of memory.
        //      Thus, this allocation should cause memory to be supplied from
        //      the internal block list.
        //
        //   3) An allocation is requested that is larger than the static
        //      buffer size.  This allocation should request memory from the
        //      internal block list.
        //
        //   4) Using the test allocator, verify the memory used no longer
        //      increases geometrically once the maximum buffer size is used.
        //
        //   5) Verify that, in appropriate build modes, defensive checks are
        //      triggered.
        //
        // Testing:
        //   void *allocate(size_type size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'allocate' TEST" << endl
                                  << "===============" << endl;

        enum {
            k_MAJOR_BUFFER_SIZE  = 1024,
            k_STATIC_BUFFER_SIZE =   64
        };

        // Align buffer to 'MaxAlignedType'.
        static union {
            char                                majorBuffer[
                                                          k_MAJOR_BUFFER_SIZE];
            bsls::AlignmentUtil::MaxAlignedType dummy;
        };

        char      *buffer;
        const int  bufferSize = k_STATIC_BUFFER_SIZE;
        char      *cBuffer;
        Strat      NAT = bsls::Alignment::BSLS_NATURAL;
        Strat      MAX = bsls::Alignment::BSLS_MAXIMUM;
        Strat      BYT = bsls::Alignment::BSLS_BYTEALIGNED;

        // Move the buffer to the interior.
        buffer = majorBuffer + 128;

        if (verbose) cout << "\nTesting allocation with buffer and allocator."
                          << endl;
        {
            {
                // Allocating 63 bytes, 1 byte under the buffer size.  This
                // allocation should happen on the static buffer.

                Obj mX(buffer, bufferSize, NAT, &objectAllocator);
                ASSERT(0       == objectAllocator.numBytesInUse());

                cBuffer =  (char *)mX.allocate(63);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());
                mX.release();

                Obj mY(buffer, bufferSize, MAX, &objectAllocator);
                ASSERT(0       == objectAllocator.numBytesInUse());

                cBuffer =  (char *)mY.allocate(63);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());
                mY.release();

                Obj mZ(buffer, bufferSize, BYT, &objectAllocator);
                ASSERT(0       == objectAllocator.numBytesInUse());

                cBuffer =  (char *)mZ.allocate(63);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());
                mZ.release();
            }
            {
                // Allocating 64 bytes, exactly the limit.  This allocation
                // should happen on the static buffer because the buffer is
                // aligned to 64 bytes exactly.

                Obj mX(buffer, bufferSize, NAT, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer =  (char *)mX.allocate(64);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());
                mX.release();

                Obj mY(buffer, bufferSize, MAX, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer =  (char *)mY.allocate(64);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());
                mY.release();

                Obj mZ(buffer, bufferSize, BYT, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer =  (char *)mZ.allocate(64);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());
                mZ.release();
            }
            {
                // Allocating 65 bytes, 1 byte over the limit.  This allocation
                // should happen through the allocator.

                Obj mX(buffer, bufferSize, NAT, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                int numBytes = 65;
                int newSize  = calculateNextSize(bufferSize, numBytes);
                cBuffer = (char *)mX.allocate(numBytes);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize)  == objectAllocator.numBytesInUse());
                mX.release();

                Obj mY(buffer, bufferSize, MAX, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer = (char *)mY.allocate(numBytes);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize)  == objectAllocator.numBytesInUse());
                mY.release();

                Obj mZ(buffer, bufferSize, BYT, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer = (char *)mZ.allocate(numBytes);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize)  == objectAllocator.numBytesInUse());
                mZ.release();
            }
            {
                // Allocating 66 bytes, 2 bytes over the limit.  This
                // allocation should happen through the allocator.

                Obj mX(buffer, bufferSize, NAT, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                int numBytes = 66;
                int newSize  = calculateNextSize(bufferSize, numBytes);
                cBuffer = (char *)mX.allocate(numBytes);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize)  == objectAllocator.numBytesInUse());
                mX.release();

                Obj mY(buffer, bufferSize, MAX, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer = (char *)mY.allocate(numBytes);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize)  == objectAllocator.numBytesInUse());
                mY.release();

                Obj mZ(buffer, bufferSize, BYT, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer = (char *)mZ.allocate(numBytes);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize)  == objectAllocator.numBytesInUse());
                mZ.release();
            }
            {
                // Allocating 63 bytes, then 1 byte, another 1 byte, and
                // another 1 byte.

                // For natural alignment, the first two allocations should
                // happen on the static buffer, the next allocation should use
                // the allocator, and the last allocation should use the buffer
                // allocated in the previous allocation.  Thus, the last
                // allocation should not increase the number of bytes used by
                // the allocator.

                Obj mX(buffer, bufferSize, NAT, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer = (char *)mX.allocate(63);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());

                cBuffer = (char *)mX.allocate(1);
                ASSERT(cBuffer == buffer + 63);
                ASSERT(0       == objectAllocator.numBytesInUse());

                cBuffer = (char *)mX.allocate(1);
                int newSize = calculateNextSize(bufferSize, 1);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize) == objectAllocator.numBytesInUse());

                bsls::Types::Int64 bytesUsed = objectAllocator.numBytesInUse();

                char *tBuffer = cBuffer;

                cBuffer          = (char *)mX.allocate(1);
                ASSERT((cBuffer  <= buffer) || (cBuffer  >= buffer + 64));
                ASSERT(cBuffer   == tBuffer + 1);
                ASSERT(bytesUsed == objectAllocator.numBytesInUse());
                mX.release();

                // For maximum alignment, the first allocation should happen on
                // the static buffer, the next allocation should use the
                // allocator, and the last 2 allocations should use the buffer
                // allocated in the previous allocation.  Thus, the last 2
                // allocations should not increase the number of bytes used by
                // the allocator.

                Obj mY(buffer, bufferSize, MAX, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer = (char *)mY.allocate(63);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());

                cBuffer = (char *)mY.allocate(1);
                newSize = calculateNextSize(bufferSize, 1);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize) == objectAllocator.numBytesInUse());

                bytesUsed  = objectAllocator.numBytesInUse();
                tBuffer    = cBuffer;

                cBuffer = (char *)mY.allocate(1);
                ASSERT((cBuffer   <= buffer) || (cBuffer  >= buffer + 64));
                ASSERT(cBuffer    == tBuffer + 1 +
                       bsls::AlignmentUtil::calculateAlignmentOffset(
                                                                 tBuffer + 1,
                                                                 k_MAX_ALIGN));
                ASSERT(bytesUsed  == objectAllocator.numBytesInUse());

                tBuffer = cBuffer;
                cBuffer = (char *)mY.allocate(1);
                ASSERT((cBuffer   <= buffer) || (cBuffer  >= buffer + 64));
                ASSERT(cBuffer    == tBuffer + 1 +
                       bsls::AlignmentUtil::calculateAlignmentOffset(
                                                                 tBuffer + 1,
                                                                 k_MAX_ALIGN));
                ASSERT(bytesUsed  == objectAllocator.numBytesInUse());
                mY.release();

                // For 1-byte alignment, the first two allocations should
                // happen on the static buffer, the next allocation should use
                // the allocator, and the last allocation should use the buffer
                // allocated in the previous allocation.  Thus, the last
                // allocation should not increase the number of bytes used by
                // the allocator.

                Obj mZ(buffer, bufferSize, BYT, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer = (char *)mZ.allocate(63);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());

                cBuffer = (char *)mZ.allocate(1);
                ASSERT(cBuffer == buffer + 63);
                ASSERT(0       == objectAllocator.numBytesInUse());

                cBuffer = (char *)mZ.allocate(1);
                newSize = calculateNextSize(bufferSize, 1);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize) == objectAllocator.numBytesInUse());

                bytesUsed  = objectAllocator.numBytesInUse();
                tBuffer    = cBuffer;

                cBuffer          = (char *)mZ.allocate(1);
                ASSERT((cBuffer  <= buffer) || (cBuffer  >= buffer + 64));
                ASSERT(cBuffer   == tBuffer + 1);
                ASSERT(bytesUsed == objectAllocator.numBytesInUse());
                mZ.release();
            }
            ASSERT(0 == objectAllocator.numBytesInUse());
        }

        if (verbose) cout <<
                 "\nTesting allocation with alignment considerations." << endl;
        {
            // Testing maximal alignment.
            {
                int blockSize = k_MAX_ALIGN;

                Obj mX(buffer, 8 * blockSize, MAX, &objectAllocator);

                int offset   = 0; // alignment offset

                cBuffer = (char *)mX.allocate(1);
                ASSERT(cBuffer == buffer);
                offset += blockSize;

                // BlockSize - 1 bytes wasted.
                cBuffer = (char *)mX.allocate(blockSize);

                ASSERT(cBuffer == buffer + offset);
                offset += blockSize;

                cBuffer = (char *)mX.allocate(1);
                ASSERT(cBuffer == buffer + offset);
                offset += blockSize;

                // BlockSize - 1 bytes wasted.
                cBuffer = (char *)mX.allocate(blockSize);

                ASSERT(cBuffer == buffer + offset);

                mX.release();
                ASSERT(0 == objectAllocator.numBytesInUse());
            }

            {
                // Allocate until static buffer is not completely full, but is
                // unable to allocate another block of the desired size because
                // of alignment considerations.

                int blockSize = k_MAX_ALIGN;

                buffer += blockSize / 2;
                Obj mX(buffer, 8 * blockSize, NAT, &objectAllocator);

                cBuffer = (char *)mX.allocate(
                                            6 * blockSize + blockSize / 2 + 1);
                ASSERT(cBuffer == buffer);

                cBuffer = (char *)mX.allocate(blockSize);
                // Block + blockSize/2 - 1 bytes wasted.

                buffer -= blockSize / 2;
            }

            {
                buffer += 63;
                // Misalign the buffer such that the buffer does not start
                // aligned.  Thus, a 64-byte allocation cannot be allocated off
                // the static buffer, even though there is enough space.

                Obj mX(buffer, bufferSize, NAT, &objectAllocator);

                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer     = (char *)mX.allocate(64);
                int newSize = calculateNextSize(bufferSize, 64);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize) == objectAllocator.numBytesInUse());

                buffer -= 63;
                // Reset buffer to correct alignment.
            }
        }

        if (verbose) cout << "\nTesting 'maxBufferSize'." << endl;
        {

            const int DATA[]   = { bufferSize,
                                   bufferSize * 2,
                                   bufferSize * 4 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // Only need to test geometric growth, since it is impossible for
            // an internal buffer with constant growth to exceed the maximum
            // buffer size.

            for (int i = 0; i < NUM_DATA; ++i) {

                const int MAXBUFFERSIZE = DATA[i];

                if (veryVerbose) {
                    T_ P_(bufferSize) P(MAXBUFFERSIZE)
                }

                bslma::TestAllocator ta(veryVeryVerbose),
                                     tb(veryVeryVerbose),
                                     tc(veryVeryVerbose),
                                     td(veryVeryVerbose);

                Obj mV(buffer, bufferSize, MAXBUFFERSIZE,      &ta);
                Obj mW(buffer, bufferSize, MAXBUFFERSIZE, NAT, &tb);
                Obj mX(buffer, bufferSize, MAXBUFFERSIZE, MAX, &tc);
                Obj mY(buffer, bufferSize, MAXBUFFERSIZE, BYT, &td);

                // First exhaust the external buffer.

                mV.allocate(bufferSize);
                mW.allocate(bufferSize);
                mX.allocate(bufferSize);
                mY.allocate(bufferSize);

                bsls::Types::Int64 nA = ta.numBytesInUse();  ASSERT(0 == nA);
                bsls::Types::Int64 nB = tb.numBytesInUse();  ASSERT(0 == nB);
                bsls::Types::Int64 nC = tc.numBytesInUse();  ASSERT(0 == nC);
                bsls::Types::Int64 nD = td.numBytesInUse();  ASSERT(0 == nD);

                // Loop until we hit the maximum buffer size.  Within the loop,
                // the size of the internal buffers should double in size every
                // time.

                int j = 1;

                bsls::Types::Int64 oldNA = 0;
                bsls::Types::Int64 oldNB = 0;
                bsls::Types::Int64 oldNC = 0;
                bsls::Types::Int64 oldND = 0;

                for (; j * bufferSize + 1 <= MAXBUFFERSIZE; j <<= 1) {
                    mV.allocate(j * bufferSize + 1);
                    mW.allocate(j * bufferSize + 1);
                    mX.allocate(j * bufferSize + 1);
                    mY.allocate(j * bufferSize + 1);

                    nA = ta.numBytesInUse();
                    nB = tb.numBytesInUse();
                    nC = tc.numBytesInUse();
                    nD = td.numBytesInUse();

                    LOOP3_ASSERT(nA, oldNA, j,
                                 nA == oldNA + blockSize(bufferSize * j * 2));
                    LOOP3_ASSERT(nB, oldNB, j,
                                 nB == oldNB + blockSize(bufferSize * j * 2));
                    LOOP3_ASSERT(nC, oldNC, j,
                                 nC == oldNC + blockSize(bufferSize * j * 2));
                    LOOP3_ASSERT(nD, oldND, j,
                                 nD == oldND + blockSize(bufferSize * j * 2));

                    oldNA = nA;
                    oldNB = nB;
                    oldNC = nC;
                    oldND = nD;
                }

                mV.allocate(j * bufferSize);
                mW.allocate(j * bufferSize);
                mX.allocate(j * bufferSize);
                mY.allocate(j * bufferSize);

                nA = ta.numBytesInUse();
                nB = tb.numBytesInUse();
                nC = tc.numBytesInUse();
                nD = td.numBytesInUse();

                // Should no longer double in size.  Note that unlike inside
                // the loop, 'bufferSize * j' is not multiplied by 2.

                LOOP3_ASSERT(nA, oldNA, blockSize(bufferSize * j),
                             nA == oldNA + blockSize(bufferSize * j));
            }
        }

        if (verbose) cout << "\nTesting a large allocation request." << endl;
        {
            Obj mX(buffer, bufferSize, NAT, &objectAllocator);

            ASSERT(0 == objectAllocator.numBytesInUse());

            int numBytes = 65;
            cBuffer = (char *)mX.allocate(numBytes);
            int newSize = calculateNextSize(bufferSize, 65);
            ASSERT(blockSize(newSize) == objectAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            char *buffer = bufferStorage.buffer();

            Obj mX(buffer, k_BUFFER_SIZE);

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
        //   the constructors, as the untested 'allocate' method is used to
        //   verify that the buffer, alignment strategy, and allocator are
        //   properly passed to the pool at construction.  However, we cannot
        //   test 'allocate' first - as it requires the constructors.  Hence,
        //   it is a combination of both this test and test case 4 that
        //   provides complete test coverage for the constructors and the
        //   'allocate' method.
        //
        // Concerns:
        //   1) That the external buffer supplied at construction is used to
        //      allocate memory.
        //
        //   2) That the allocator supplied at construction is used to supply
        //      memory when the external buffer is full.
        //
        //   3) That the allocator used, when not specified at construction,
        //      is defaulted to the currently installed default allocator.
        //
        //   4) That the alignment strategy, when specified, is respected
        //      during memory allocation.
        //
        //   5) That the alignment strategy, when not specified at
        //      construction, defaults to natural alignment.
        //
        //   6) QoI: Asserted precondition violations are detected when
        //      enabled.
        //
        // Plan:
        //   1) For concerns 1 and 2, first create a 'bslma::TestAllocator' and
        //      a buffer on the stack.  Then create a buffered sequential pool
        //      object using the allocator and the buffer.  Verify that memory
        //      allocated from the pool first comes from the buffer, and when
        //      the buffer cannot satisfy the request, comes from the
        //      allocator.
        //
        //   2) For concern 3, install a 'bslma::TestAllocator' as the default
        //      allocator using a 'bslma::DefaultAllocatorGuard' object.  Then
        //      verify that when an allocator is not specified at construction,
        //      the currently installed default allocator is used.
        //
        //   3) For concerns 4 and 5, allocate memory twice from both a
        //      buffered sequential pool and the
        //      'bdlma::BufferImpUtil::allocate' method, and verify the
        //      alignment strategy of the buffered sequential pool by comparing
        //      the address of the second allocation.
        //
        //   4) For concern 6, verify that, in appropriate build modes,
        //      defensive checks are triggered.
        //
        // Testing:
        //   bdlma::BufferedSequentialPool(*buf, sz, *a = 0);
        //   bdlma::BufferedSequentialPool(*buf, sz, GS, *a = 0);
        //   bdlma::BufferedSequentialPool(*buf, sz, AS, *a = 0);
        //   bdlma::BufferedSequentialPool(*buf, sz, GS, AS, *a = 0);
        //   bdlma::BufferedSequentialPool(*buf, sz, max, *a = 0);
        //   bdlma::BufferedSequentialPool(*buf, sz, max, GS, *a = 0);
        //   bdlma::BufferedSequentialPool(*buf, sz, max, AS, *a = 0);
        //   bdlma::BufferedSequentialPool(*buf, sz, max, GS, AS, *a = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR TEST" << endl
                                  << "=========" << endl;

        enum { k_ALLOC_SIZE1 = 4, k_ALLOC_SIZE2 = 8 };

        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting external buffer and allocator "
                             "argument." << endl;
        {
            ASSERT(0 == objectAllocator.numBlocksTotal());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksTotal());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            void *addr = mX.allocate(1);

            // Memory allocation comes from the external buffer.
            LOOP2_ASSERT((void *)&buffer[0], addr, &buffer[0] == addr);
            ASSERT(0 == objectAllocator.numBlocksTotal());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            addr = mX.allocate(k_BUFFER_SIZE);

            // Memory allocation comes from the allocator supplied at
            // construction.
            ASSERT(0 != objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());
        bsls::Types::Int64 total = objectAllocator.numBlocksTotal();

        if (verbose) cout << "\nTesting default allocator argument." << endl;
        {
            ASSERT(total == objectAllocator.numBlocksTotal());
            ASSERT(0     == defaultAllocator.numBlocksTotal());
            ASSERT(0     == globalAllocator.numBlocksTotal());

            Obj mX(buffer, k_BUFFER_SIZE);

            ASSERT(total == objectAllocator.numBlocksTotal());
            ASSERT(0     == defaultAllocator.numBlocksTotal());
            ASSERT(0     == globalAllocator.numBlocksTotal());

            void *addr = mX.allocate(1);

            // Memory allocation comes from the external buffer.
            LOOP2_ASSERT((void *)&buffer[0], addr, &buffer[0] == addr);
            ASSERT(total == objectAllocator.numBlocksTotal());
            ASSERT(0     == defaultAllocator.numBlocksTotal());
            ASSERT(0     == globalAllocator.numBlocksTotal());

            addr = mX.allocate(k_BUFFER_SIZE);

            // Memory allocation comes from the allocator supplied at
            // construction.
            ASSERT(total == objectAllocator.numBlocksTotal());
            ASSERT(0     != defaultAllocator.numBytesInUse());
            ASSERT(0     == globalAllocator.numBlocksTotal());
        }
        ASSERT(0 == defaultAllocator.numBytesInUse());

        if (verbose) cout << "\nTesting alignment strategy." << endl;
        {
            if (verbose) cout << "\tTesting natural alignment." << endl;
            {
                Obj mX(buffer, k_BUFFER_SIZE, bsls::Alignment::BSLS_NATURAL);

                mX.allocate(k_ALLOC_SIZE1);
                void *addr = mX.allocate(k_ALLOC_SIZE2);

                int cursor = 0;
                bdlma::BufferImpUtil::allocateFromBuffer(
                                                &cursor,
                                                buffer,
                                                k_BUFFER_SIZE,
                                                k_ALLOC_SIZE1,
                                                bsls::Alignment::BSLS_NATURAL);
                void *addr2 = bdlma::BufferImpUtil::allocateFromBuffer(
                                                &cursor,
                                                buffer,
                                                k_BUFFER_SIZE,
                                                k_ALLOC_SIZE2,
                                                bsls::Alignment::BSLS_NATURAL);

                LOOP2_ASSERT(addr2, addr, addr2 == addr);
            }

            if (verbose) cout << "\tTesting maximum alignment." << endl;
            {
                Obj mX(buffer, k_BUFFER_SIZE, bsls::Alignment::BSLS_MAXIMUM);

                mX.allocate(k_ALLOC_SIZE1);
                void *addr = mX.allocate(k_ALLOC_SIZE2);

                int cursor = 0;
                bdlma::BufferImpUtil::allocateFromBuffer(
                                                &cursor,
                                                buffer,
                                                k_BUFFER_SIZE,
                                                k_ALLOC_SIZE1,
                                                bsls::Alignment::BSLS_MAXIMUM);
                void *addr2 = bdlma::BufferImpUtil::allocateFromBuffer(
                                                &cursor,
                                                buffer,
                                                k_BUFFER_SIZE,
                                                k_ALLOC_SIZE2,
                                                bsls::Alignment::BSLS_MAXIMUM);

                LOOP2_ASSERT(addr2, addr, addr2 == addr);
            }

            if (verbose) cout << "\tTesting 1-byte alignment." << endl;
            {
                Obj mX(buffer,
                       k_BUFFER_SIZE,
                       bsls::Alignment::BSLS_BYTEALIGNED);

                mX.allocate(k_ALLOC_SIZE1);
                void *addr = mX.allocate(k_ALLOC_SIZE2);

                int cursor = 0;
                bdlma::BufferImpUtil::allocateFromBuffer(
                                            &cursor,
                                            buffer,
                                            k_BUFFER_SIZE,
                                            k_ALLOC_SIZE1,
                                            bsls::Alignment::BSLS_BYTEALIGNED);
                void *addr2 = bdlma::BufferImpUtil::allocateFromBuffer(
                                            &cursor,
                                            buffer,
                                            k_BUFFER_SIZE,
                                            k_ALLOC_SIZE2,
                                            bsls::Alignment::BSLS_BYTEALIGNED);

                LOOP2_ASSERT(addr2, addr, addr2 == addr);
            }
        }

        if (verbose) cout << "\nTesting default alignment strategy." << endl;
        {
            Obj mX(buffer, k_BUFFER_SIZE);

            mX.allocate(k_ALLOC_SIZE1);
            void *addr = mX.allocate(k_ALLOC_SIZE2);

            int cursor = 0;
            bdlma::BufferImpUtil::allocateFromBuffer(
                                                &cursor,
                                                buffer,
                                                k_BUFFER_SIZE,
                                                k_ALLOC_SIZE1,
                                                bsls::Alignment::BSLS_NATURAL);
            void *addr2 = bdlma::BufferImpUtil::allocateFromBuffer(
                                                &cursor,
                                                buffer,
                                                k_BUFFER_SIZE,
                                                k_ALLOC_SIZE2,
                                                bsls::Alignment::BSLS_NATURAL);

            LOOP2_ASSERT(addr2, addr, addr2 == addr);
        }

        if (verbose) cout << "\nTesting growth strategy." << endl;
        {
            {
                Obj mX(buffer,
                       k_BUFFER_SIZE,
                       bsls::BlockGrowth::BSLS_GEOMETRIC);
                ASSERT(0 == defaultAllocator.numBytesInUse());

                mX.allocate(k_BUFFER_SIZE + 1);

                ASSERT(blockSize(k_BUFFER_SIZE * 2)
                                          == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mX(buffer,
                       k_BUFFER_SIZE,
                       bsls::BlockGrowth::BSLS_CONSTANT);
                ASSERT(0 == defaultAllocator.numBytesInUse());

                mX.allocate(k_BUFFER_SIZE);
                ASSERT(0 == defaultAllocator.numBytesInUse());

                mX.allocate(k_BUFFER_SIZE);
                ASSERT(blockSize(k_BUFFER_SIZE)
                                          == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nTesting default growth strategy." << endl;
        {
            {
                Obj mX(buffer, k_BUFFER_SIZE);
                ASSERT(0 == defaultAllocator.numBytesInUse());

                mX.allocate(k_BUFFER_SIZE + 1);

                ASSERT(blockSize(k_BUFFER_SIZE * 2)
                                          == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mX(buffer, k_BUFFER_SIZE, bsls::Alignment::BSLS_NATURAL);
                ASSERT(0 == defaultAllocator.numBytesInUse());

                mX.allocate(k_BUFFER_SIZE + 1);

                ASSERT(blockSize(k_BUFFER_SIZE * 2)
                                          == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

#define GEO bsls::BlockGrowth::BSLS_GEOMETRIC
#define CON bsls::BlockGrowth::BSLS_CONSTANT
#define NAT bsls::Alignment::BSLS_NATURAL
#define MAX bsls::Alignment::BSLS_MAXIMUM
#define BYT bsls::Alignment::BSLS_BYTEALIGNED

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'Obj(buf, sz, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, GS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2, CON));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2, CON));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0, CON));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1, CON));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, GS, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2, CON, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2, CON, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0, CON, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1, CON, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, max, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2,  8));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2,  8));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0,  8));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1,  8));

                ASSERT_SAFE_PASS(    Obj(buffer,  2,  2));

                ASSERT_SAFE_FAIL(    Obj(buffer,  2,  1));
                ASSERT_SAFE_FAIL(    Obj(buffer,  2, -2));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, max, GS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2,  8, CON));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2,  8, CON));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0,  8, CON));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1,  8, CON));

                ASSERT_SAFE_PASS(    Obj(buffer,  2,  2, CON));

                ASSERT_SAFE_FAIL(    Obj(buffer,  2,  1, CON));
                ASSERT_SAFE_FAIL(    Obj(buffer,  2, -2, CON));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, max, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2,  8, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2,  8, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0,  8, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1,  8, MAX));

                ASSERT_SAFE_PASS(    Obj(buffer,  2,  2, MAX));

                ASSERT_SAFE_FAIL(    Obj(buffer,  2,  1, MAX));
                ASSERT_SAFE_FAIL(    Obj(buffer,  2, -2, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(buf, sz, max, GS, AS, *ba)'"
                                  << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj(buffer,  2,  8, CON, MAX));

                ASSERT_SAFE_FAIL_RAW(Obj(0,       2,  8, CON, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer,  0,  8, CON, MAX));
                ASSERT_SAFE_FAIL_RAW(Obj(buffer, -1,  8, CON, MAX));

                ASSERT_SAFE_PASS(    Obj(buffer,  2,  2, CON, MAX));

                ASSERT_SAFE_FAIL(    Obj(buffer,  2,  1, CON, MAX));
                ASSERT_SAFE_FAIL(    Obj(buffer,  2, -2, CON, MAX));
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
            // called; thus, 'lastAllocatedSize' will return -1 instead of 0.

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
        //   1) That a 'bdlma::BufferedSequentialPool' can be created and
        //      destroyed.
        //
        //   2) That 'allocate' returns a block of memory having the specified
        //      size and expected alignment.
        //
        //   3) That 'allocate' returns a block of memory from the external
        //      buffer supplied at construction.
        //
        //   4) That 'allocate' returns a block of memory even when the
        //      allocation request exceeds the remaining free space in the
        //      external buffer.
        //
        //   5) Destruction of the pool releases all managed memory, including
        //      memory that comes from dynamic allocation.
        //
        // Plan:
        //   For concerns 1, 2, and 3, first, create a
        //   'bdlma::BufferedSequentialPool' with an aligned static buffer.
        //   Next, allocate a block of memory from the pool and verify that it
        //   comes from the external buffer.  Then, allocate another block of
        //   memory from the pool, and verify that the first allocation
        //   returned a block of memory of sufficient size by checking that
        //   'addr2 >= addr1 + k_ALLOC_SIZE1'.  Also verify that the alignment
        //   strategy indicated at construction is followed by checking the
        //   address of the second allocation.
        //
        //   For concern 4, initialize a 'bdlma::BufferedSequentialPool' with a
        //   'bslma::TestAllocator'.  Then allocate a block of memory that is
        //   larger than the buffer supplied at construction of the buffered
        //   sequential pool.  Verify that memory is allocated from the test
        //   allocator.
        //
        //   For concern 5, let the pool created with the test allocator go
        //   out of scope, and verify, through the test allocator, that all
        //   allocated memory is deallocated.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        enum { k_ALLOC_SIZE1 = 4, k_ALLOC_SIZE2 = 8 };

        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting constructor." << endl;
        Obj mX(buffer, k_BUFFER_SIZE, &objectAllocator);

        if (verbose) cout << "\nTesting allocate from buffer." << endl;
        void *addr1 = mX.allocate(k_ALLOC_SIZE1);

        // Allocation starts at the beginning of the aligned buffer.
        ASSERT(&buffer[0] == addr1);

        // Allocation comes from within the buffer.
        LOOP2_ASSERT((void *)&buffer[k_BUFFER_SIZE - 1],
                     addr1,
                     &buffer[k_BUFFER_SIZE - 1] >= addr1);

        void *addr2 = mX.allocate(k_ALLOC_SIZE2);

        // Allocation comes from within the buffer.
        ASSERT(&buffer[0]               <  addr2);
        ASSERT(&buffer[k_BUFFER_SIZE - 1] >= addr2);

        // Allocation respects the alignment strategy.
        LOOP2_ASSERT((void *)&buffer[8],
                     addr2,
                     &buffer[8] == addr2);

        // First allocation is of sufficient size.
        ASSERT((char *)addr2 >= (char *)addr1 + k_ALLOC_SIZE1);

        // Make sure no memory comes from the object, default, and global
        // allocators.
        ASSERT(0 == objectAllocator.numBlocksTotal());
        ASSERT(0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nTesting allocate when buffer runs out."
                          << endl;
        {
            Obj mY(buffer, k_BUFFER_SIZE, &objectAllocator);
            addr1 = mY.allocate(k_BUFFER_SIZE + 1);

            // Allocation request is satisfied even when larger than the
            // supplied buffer.
            LOOP_ASSERT(addr1, 0 != addr1);

            // Allocation comes from the objectAllocator.
            ASSERT(0 != objectAllocator.numBlocksInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());
        }

        // All dynamically allocated memory is released after the pool's
        // destruction.
        ASSERT(0 == objectAllocator.numBlocksInUse());

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
