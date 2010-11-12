// bdema_sequentialpool.t.cpp                                         -*-C++-*-
#include <bdema_sequentialpool.h>
#include <bdema_infrequentdeleteblocklist.h>

#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignment.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// A 'bdema_SequentialPool' is a mechanism (i.e., having state but no value)
// that is used as a memory manager to manage dynamically allocated memory.
// The primary concern is that the sequential pool's internal block list grows
// at the specified growth strategy, at the specified initial size and
// constrained by the specified maximum buffer size.  These options together
// created 12 variations of the constructor, and they must all be thoroughly
// tested.
//
// Because 'bdema_SequentialPool' does not have any accessors, this test driver
// verifies the correctness of the pool's allocations indirectly through the
// use of two consecutive allocations - where the first allocation tests for
// correctness of 'allocate', and the second verifies the size of the first
// allocation and the memory alignment strategy.
//
// We make heavy use of the 'bslma_TestAllocator' to ensure that:
//
// (1) The growth rate of the internal block list matches the specified growth
//     strategy and constrained by the specified maximum buffer size.
//
// (2) When 'release' is invoked, all memory managed by the pool is
//     deallocated.
//
// (3) When the pool is destroyed, all managed memory is deallocated.
//
// Finally, the destructor of 'bdema_SequentialPool' is tested throughout the
// test driver.  At destruction, the pool should reclaim all outstanding memory
// allocation.  By setting the global allocator, default allocator and object
// allocator to different test allocators, we can determine whether all memory
// had been released by the destructor of the pool.
//-----------------------------------------------------------------------------
// // CREATORS
// [ 3] bdema_SequentialPool(bslma_Allocator *a = 0)
// [ 3] bdema_SequentialPool(GS g, bslma_Allocator *a = 0)
// [ 3] bdema_SequentialPool(AS a, bslma_Allocator *a = 0)
// [ 3] bdema_SequentialPool(GS g, AS a, bslma_Allocator *a = 0)
//
// [ 3] bdema_SequentialPool(int i, bslma_Allocator *a = 0)
// [ 3] bdema_SequentialPool(int i, GS g, bslma_Allocator *a = 0)
// [ 3] bdema_SequentialPool(int i, AS a, bslma_Allocator *a = 0)
// [ 3] bdema_SequentialPool(int i, GS g, AS a, bslma_Allocator *a = 0)
//
// [ 3] bdema_SequentialPool(int i, int m, bslma_Allocator *a = 0)
// [ 3] bdema_SequentialPool(int i, int m, GS g, bslma_Allocator *a = 0)
// [ 3] bdema_SequentialPool(int i, int m, AS a, bslma_Allocator *a = 0)
// [ 3] bdema_SequentialPool(int i, int m, GS g, AS a, bslma_Allocator *a = 0)
//
// [  ] ~bdema_SequentialPool()
//
// // MANIPULATORS
// [ 4] void *allocate(int size)
// [ 7] void *allocateAndExpand(int *size)
// [ 6] void deleteObjectRaw(const TYPE *object)
// [ 6] void deleteObject(const TYPE *object)
// [ 5] void release()
// [ 9] void reserveCapacity(int numBytes)
// [ 8] int truncate(void *address, int originalSize, int newSize)
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] HELPER FUNCTION: 'int blockSize(numBytes)'
// [10] FREE FUNCTION: 'operator new(size_t, bdema_SequentialPool)'
// [11] USAGE EXAMPLE
//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  GLOBAL VARIABLES / TYPEDEF FOR TESTING
//-----------------------------------------------------------------------------
typedef bdema_SequentialPool Obj;

enum { DEFAULT_SIZE = 256 };  // initial size of the buffer if none specified

bool globalDestructorInvoked = false;

//=============================================================================
//                          HELPER CLASS FOR TESTING
//-----------------------------------------------------------------------------

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

struct InfrequentDeleteBlock {
    // This type is copied from the 'bdema_infrequentdeleteblocklist.h' for
    // testing purposes.

    InfrequentDeleteBlock              *d_next_p;
    bsls_AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
};

//=============================================================================
//                        STATIC FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
static int blockSize(int numBytes)
    // Return the adjusted block size based on the specified 'numBytes' using
    // the calculation performed by the
    // 'bdema_InfrequentDeleteBlockList::allocate' method.
{
    ASSERT(0 <= numBytes);

    if (numBytes) {
        numBytes += sizeof(InfrequentDeleteBlock) - 1;
        numBytes &= ~(bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);
    }

    return numBytes;
}

//-----------------------------------------------------------------------------

static int calculateNextSize(int currSize, int size)
{
    ASSERT(0 < currSize);
    ASSERT(0 < size);

    if (0 == currSize) {
        return 0;
    }

    do {
        currSize *= 2;
    } while (currSize < size);

    return currSize;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------
///Example 1: Using 'bdema_SequentialPool' for Efficient Allocations
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we define a container class, 'my_IntDoubleArray', that holds both
// 'int' and 'double' values.  The class can be implemented using two parallel
// arrays: one storing the type information, and the other storing pointers to
// the 'int' and 'double' values.  For efficient memory allocation, we can use
// a 'bdema_SequentialPool' for memory allocation:
//..
///Example 1: Using 'bdema_SequentialPool' for Efficient Allocations
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we define a container class, 'my_IntDoubleArray', that holds both
// 'int' and 'double' values.  The class can be implemented using two parallel
// arrays: one storing the type information, and the other storing pointers to
// the 'int' and 'double' values.  For efficient memory allocation, we can use
// a 'bdema_SequentialPool' for memory allocation:
//..
//  // my_intdoublearray.h
//
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

        bdema_SequentialPool
               d_pool;          // sequential memory pool used to supply memory

      private:
        // PRIVATE MANIPULATORS
        void increaseSize();
            // Increase the capacity of the internal arrays used to store
            // elements added to this array by at least one element.

      public:
        // TYPES
        enum Type { MY_INT, MY_DOUBLE };

        // CREATORS
        my_IntDoubleArray(bslma_Allocator *basicAllocator = 0);
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
//  // MANIPULATORS
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
//  // my_intdoublearray.cpp
//  #include <my_intdoublearray.h>
//
    enum { INITIAL_SIZE = 1, GROWTH_FACTOR = 2 };

    // PRIVATE MANIPULATORS
    void my_IntDoubleArray::increaseSize()
    {
        // Implementation elided.
        // ...
    }

    // CREATORS
    my_IntDoubleArray::my_IntDoubleArray(bslma_Allocator *basicAllocator)
    : d_length(0)
    , d_capacity(INITIAL_SIZE)
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

        d_typeArray_p[d_length]  = static_cast<char>
                                           (my_IntDoubleArray::MY_INT);
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

        d_typeArray_p[d_length]  = static_cast<char>(
                                         my_IntDoubleArray::MY_DOUBLE);
        d_valueArray_p[d_length] = item;

        ++d_length;
    }
//..
///Example 2: Implementing an Allocator Using 'bdema_SequentialPool'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bslma_Allocator' is used throughout the interfaces of BDE components.
// Suppose we would like to create a fast allocator, 'my_FastAllocator', that
// allocates memory from a buffer in a similar fashion to
// 'bdema_SequentialPool'.  This class can be used directly to implement such
// an allocator.
//
// Note that the documentation for this class is simplified for this usage
// example.  Please see 'bdema_sequentialallocator' for full documentation of a
// similar class.
//..
    class my_SequentialAllocator : public bslma_Allocator {
        // This class implements the 'bslma_Allocator' protocol to provide a
        // fast allocator of heterogeneous blocks of memory (of varying,
        // user-specified sizes) from dynamically-allocated internal buffers.

        // DATA
        bdema_SequentialPool d_pool;  // memory manager for allocated memory
                                      // blocks

      public:
        // CREATORS
        my_SequentialAllocator(bslma_Allocator *basicAllocator = 0);
            // Create an allocator for allocating memory blocks from
            // dynamically-allocated internal buffers.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        ~my_SequentialAllocator();
            // Destroy this allocator.  All memory allocated from this
            // allocator is released.

        // MANIPULATORS
        virtual void *allocate(int size);
            // Return the address of a contiguous block of memory of the
            // specified 'size' (in bytes).

        virtual void deallocate(void *address);
            // This method has no effect as all memory allocated by this
            // allocator is managed.
    };

    // CREATORS
    inline
    my_SequentialAllocator::my_SequentialAllocator(
                                               bslma_Allocator *basicAllocator)
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
    void *my_SequentialAllocator::allocate(int size)
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

    // As part of our overall allocator testing strategy, we will create
    // three test allocators.

    // Object Test Allocator.
    bslma_TestAllocator objectAllocator("Object Allocator",
                                        veryVeryVeryVerbose);

    // Default Test Allocator.
    bslma_TestAllocator defaultAllocator("Default Allocator",
                                         veryVeryVeryVerbose);
    bslma_DefaultAllocatorGuard guard(&defaultAllocator);

    // Global Test Allocator.
    bslma_TestAllocator  globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma_Allocator *originalGlobalAllocator =
                           bslma_Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 11: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Copy usage example from the header file, uncomment the code and
        //   change all 'assert' to 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // GLOBAL OPERATOR NEW TEST
        //
        // Concerns:
        //   That a 'bdema_SequentialPool' can be used directly with
        //   'operator new'.
        //
        // Plan:
        //   Since 'bdema_SequentialPool' is thoroughly tested at this point,
        //   we just need to make sure that 'new' forwards the memory request
        //   directly.
        //
        // Testing:
        //   operator new(size_t, bdema_SequentialPool&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "GLOBAL OPERATOR NEW TEST" << endl
                                  << "========================" << endl;

        Obj pool(&objectAllocator);

        double *d = new(pool)double(3.0);

        ASSERT(0 != objectAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == globalAllocator.numBytesInUse());

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // 'reserveCapacity' TEST
        //
        // Concerns:
        //   1) That if there are sufficient memory within the internal buffer,
        //      'reserveCapacity' should not trigger dynamic allocation.
        //
        //   2) That we can allocate at least the amount of bytes specified in
        //      'reserveCapacity' before triggering another dynamic allocation.
        //
        //   3) That 'reserveCapacity' can override the maximum buffer size
        //      parameter supplied to the pool at construction.
        //
        // Plan:
        //   Create a 'bdema_SequentialPool' using a test allocator and specify
        //   an initial size and maximum buffer size.
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
        //   Finally, for concern 3, invoke 'reserveCapacity' with a size
        //   larger than the maximum buffer size.  Repeat verification for
        //   concern 2.
        //
        // Testing:
        //   void reserveCapacity(int numBytes)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'reserveCapacity' TEST" << endl
                                  << "======================" << endl;

        enum { INITIAL_SIZE = 64, MAX_BUFFER = INITIAL_SIZE * 4 };

        if (verbose) cout << "\nTesting that 'reserveCapacity' does not "
                             "trigger dynamic memory allocation." << endl;

        {
            Obj pool(INITIAL_SIZE, MAX_BUFFER, &objectAllocator);
            int numBytesUsed = objectAllocator.numBytesInUse();

            pool.reserveCapacity(INITIAL_SIZE / 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            pool.allocate(INITIAL_SIZE / 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            pool.reserveCapacity(INITIAL_SIZE * 2);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            pool.allocate(INITIAL_SIZE * 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            pool.reserveCapacity(MAX_BUFFER * 2);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            pool.allocate(MAX_BUFFER * 2);
            ASSERT(numBytesUsed = objectAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nTesting 'reserveCapacity' on a default "
                             "constructed pool." << endl;
        {
            Obj pool(&objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());

            pool.reserveCapacity(DEFAULT_SIZE);
            int numBytesUsed = objectAllocator.numBytesInUse();

            pool.allocate(DEFAULT_SIZE);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            pool.reserveCapacity(DEFAULT_SIZE * 4);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            pool.allocate(DEFAULT_SIZE * 4);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());
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
        // Plan:
        //   For concern 1, using the table-driven technique, create test
        //   vectors having the alignment strategy, initial allocation size,
        //   the new size, and expected offset.  First allocate memory of
        //   initial allocation size, then truncate to the new size, and
        //   allocate memory (1 byte) again.  Verify that the latest allocation
        //   matches the expected offset.
        //
        //   For concern 2, truncate the memory returned by the initial
        //   allocation, and verify that the return value is 'originalSize'.
        //
        // Testing:
        //   int truncate(void *address, int originalSize, int newSize)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TRUNCATE TEST" << endl
                                  << "=============" << endl;

        typedef bsls_Alignment::Strategy St;

        enum { MAX_ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT };

#define NAT bsls_Alignment::BSLS_NATURAL
#define MAX bsls_Alignment::BSLS_MAXIMUM

        if (verbose) cout << "\nTesting 'truncate'." << endl;

        static const struct {
            int d_line;         // line number
            St  d_strategy;     // alignment strategy
            int d_initialSize;  // size of initial allocation request
            int d_newSize;      // truncate size
            int d_expOffset;    // expected memory offset
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
            {  L_,      MAX,           1,           1,   MAX_ALIGN },
            {  L_,      MAX,           2,           0,           0 },
            {  L_,      MAX,           2,           1,   MAX_ALIGN },
            {  L_,      MAX,           2,           2,   MAX_ALIGN },
            {  L_,      MAX,           3,           0,           0 },
            {  L_,      MAX,           3,           1,   MAX_ALIGN },
            {  L_,      MAX,           3,           2,   MAX_ALIGN },
            {  L_,      MAX,           3,           3,   MAX_ALIGN },
            {  L_,      MAX,           8,           4,   MAX_ALIGN },
            {  L_,      MAX,         511,         511,          -1 },  // *
            {  L_,      MAX,         512,           0,           0 },
            {  L_,      MAX,         512,           1,   MAX_ALIGN },
            {  L_,      MAX,         512,         511,          -1 },  // *
            {  L_,      MAX,         512,         512,          -1 },  // *
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_line;         // line number
            const St  STRAT       = DATA[ti].d_strategy;     // alignment strat
            const int INITIALSIZE = DATA[ti].d_initialSize;  // initial size
            const int NEWSIZE     = DATA[ti].d_newSize;      // new size
            const int EXPOFFSET   = DATA[ti].d_expOffset;    // expected offset

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL_ALIGNMENT, ";
                }
                else {
                    cout << "STRAT = MAXIMUM_ALIGNMENT, ";
                }
                P_(INITIALSIZE) P_(NEWSIZE) P(EXPOFFSET)
            }

            Obj pool(STRAT, &objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());

            void *addr1 = pool.allocate(INITIALSIZE);
            ASSERT(0 != objectAllocator.numBytesInUse());

            int used = objectAllocator.numBytesInUse();

            pool.truncate(addr1, INITIALSIZE, NEWSIZE);
            ASSERT(used == objectAllocator.numBytesInUse());

            void *addr2 = pool.allocate(1);
            if (EXPOFFSET >= 0) {
                ASSERT(used == objectAllocator.numBytesInUse());
                LOOP4_ASSERT(LINE, EXPOFFSET, addr1, addr2,
                             (char *)addr1 + EXPOFFSET == (char *)addr2);
            }
            else {
                ASSERT(used < objectAllocator.numBytesInUse());
            }

            // Truncating previously allocated address should fail.
            if (EXPOFFSET >= 0 &&
                        ((char *)addr1 + INITIALSIZE) != ((char *)addr2 + 1)) {
                int ret = pool.truncate(addr1, INITIALSIZE, NEWSIZE);
                LOOP2_ASSERT(INITIALSIZE, ret, INITIALSIZE == ret);
            }
        }

#undef MAX
#undef NAT

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
        // Plan:
        //   For both concern, using the table-driven technique, create test
        //   vectors having the alignment strategy, initial memory offset and
        //   expected memory used.  First allocate memory necessary for the
        //   initial memory offset, then allocate 1 byte using
        //   'allocateAndExpand'.  Verify the updated size is the same as the
        //   expected memory used.  Finally, invoke 'allocate' again and verify
        //   it triggers new dynamic memory allocation - meaning
        //   'allocateAndExpand' did use up all available memory in the
        //   internal buffer.
        //
        // Testing:
        //   void *allocateAndExpand(int *size)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'allocateAndExpand' TEST" << endl
                                  << "========================" << endl;

        typedef bsls_Alignment::Strategy St;

        enum { MAX_ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT };

#define NAT bsls_Alignment::BSLS_NATURAL
#define MAX bsls_Alignment::BSLS_MAXIMUM

        if (verbose) cout << "\nTesting 'expand'." << endl;

        static const struct {
            int d_line;         // line number
            St  d_strategy;     // alignment strategy
            int d_initialSize;  // size of initial allocation request
            int d_expused;      // expected memory used after 'expand'
        } DATA[] = {
            // LINE     STRAT       INITIALSIZE       EXPUSED
            // ----     -----       -----------       -------

            // NATURAL ALIGNMENT
            {  L_,      NAT,           1,     DEFAULT_SIZE * 2 - 1           },
            {  L_,      NAT,           2,     DEFAULT_SIZE * 2 - 2           },
            {  L_,      NAT,           3,     DEFAULT_SIZE * 2 - 3           },
            {  L_,      NAT,           4,     DEFAULT_SIZE * 2 - 4           },
            {  L_,      NAT,           7,     DEFAULT_SIZE * 2 - 7           },
            {  L_,      NAT,           8,     DEFAULT_SIZE * 2 - 8           },
            {  L_,      NAT,          15,     DEFAULT_SIZE * 2 - 15          },
            {  L_,      NAT,          16,     DEFAULT_SIZE * 2 - 16          },
            {  L_,      NAT,         100,     DEFAULT_SIZE * 2 - 100         },
            {  L_,      NAT,         510,     DEFAULT_SIZE * 2 - 510         },
            {  L_,      NAT,         511,     DEFAULT_SIZE * 2 - 511         },

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,           1,     DEFAULT_SIZE * 2 - MAX_ALIGN   },
            {  L_,      MAX,           2,     DEFAULT_SIZE * 2 - MAX_ALIGN   },
            {  L_,      MAX,           3,     DEFAULT_SIZE * 2 - MAX_ALIGN   },
            {  L_,      MAX,           4,     DEFAULT_SIZE * 2 - MAX_ALIGN   },
            {  L_,      MAX,           7,     DEFAULT_SIZE * 2 - MAX_ALIGN   },
            {  L_,      MAX,           8,     DEFAULT_SIZE * 2 - MAX_ALIGN   },
            {  L_,      MAX,          15,     DEFAULT_SIZE * 2 - 16          },
            {  L_,      MAX,          16,     DEFAULT_SIZE * 2 - 16          },
            {  L_,      MAX,         108,     DEFAULT_SIZE * 2 - 112         },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_line;
            const St  STRAT       = DATA[ti].d_strategy;
            const int INITIALSIZE = DATA[ti].d_initialSize;
            const int EXPUSED     = DATA[ti].d_expused;

            if (veryVerbose) {
                T_ P_(LINE)
                if (STRAT == NAT) {
                    cout << "STRAT = NATURAL_ALIGNMENT, ";
                }
                else {
                    cout << "STRAT = MAXIMUM_ALIGNMENT, ";
                }
                P_(INITIALSIZE) P(EXPUSED)
            }

            Obj pool(STRAT, &objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == objectAllocator.numBlocksInUse());

            void *addr1 = pool.allocate(INITIALSIZE);
            ASSERT(blockSize(DEFAULT_SIZE * 2) ==
                                              objectAllocator.numBytesInUse());
            ASSERT(1 == objectAllocator.numBlocksInUse());

            bsls_PlatformUtil::size_type size = 1;
            void *addr2 = pool.allocateAndExpand(&size);

            // Check for correct memory address.
            if (bsls_Alignment::BSLS_NATURAL == STRAT) {
                ASSERT((char *)addr1 + INITIALSIZE == (char *)addr2);
            }
            else {
                int offset = bsls_AlignmentUtil::calculateAlignmentOffset(
                                       (char *)addr1 + INITIALSIZE,
                                       bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT);
                ASSERT((char *)addr1 + INITIALSIZE + offset == (char *)addr2);
            }

            // Check 'size' is updated correctly.
            ASSERT(EXPUSED == size);

            // Check for no new allocations.
            ASSERT(blockSize(DEFAULT_SIZE * 2) ==
                                              objectAllocator.numBytesInUse());
            ASSERT(1 == objectAllocator.numBlocksInUse());

            // Check that new allocations causes dynamic memory allocations.
            pool.allocate(1);
            ASSERT(2 == objectAllocator.numBlocksInUse());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());

        {
            // No initial allocation, just allocate and expand directly.
            Obj pool(&objectAllocator);
            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == objectAllocator.numBlocksInUse());

            bsls_PlatformUtil::size_type size = 1;
            pool.allocateAndExpand(&size);

            ASSERT(blockSize(DEFAULT_SIZE * 2) ==
                                              objectAllocator.numBytesInUse());
            ASSERT(1 == objectAllocator.numBlocksInUse());

            pool.allocate(1);
            ASSERT(2 == objectAllocator.numBlocksInUse());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());

#undef MAX
#undef NAT

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // DELETE TEST
        //
        // Concerns:
        //   1) Both 'deleteObject' and 'deleteObjectRaw' methods invokes the
        //      destructor of the object passed in.
        //
        // Plan:
        //   Since 'deleteObject' and 'deleteObjectRaw' does not deallocate
        //   memory, we just need to ensure that the destructor of the object
        //   passed in is invoked.
        //
        // Testing:
        //   void deleteObjectRaw(const TYPE *object)
        //   void deleteObject(const TYPE *object)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DELETE TEST" << endl
                                  << "===========" << endl;

        if (verbose) cout << "\nTesting 'deleteObject'." << endl;
        {
            Obj pool;

            void *addr = pool.allocate(sizeof(my_Class));
            my_Class *obj = new(addr)my_Class();

            globalDestructorInvoked = false;
            pool.deleteObject(obj);

            ASSERT(true == globalDestructorInvoked);
        }

        if (verbose) cout << "\nTesting 'deleteObjectRaw'." << endl;
        {
            Obj pool;

            void *addr = pool.allocate(sizeof(my_Class));
            my_Class *obj = new(addr)my_Class();

            globalDestructorInvoked = false;
            pool.deleteObjectRaw(obj);

            ASSERT(true == globalDestructorInvoked);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'release' TEST
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   void release()
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' TEST" << endl
                                  << "==============" << endl;

        const int TH = 64;
        static const struct {
            int d_line;         // line number
            int d_bufSize;      // buffer size
            int d_requestSize;  // request size
            int d_numRequests;  // number of requests
        } DATA[] = {
            // LINE     BUFSIZE     REQSIZE         NUMREQ
            // ----     -------     -------         ------

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
        const int DATA_SIZE = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < DATA_SIZE; ++i) {
            const int LINE    = DATA[i].d_line;
            const int BUFSIZE = DATA[i].d_bufSize;
            const int REQSIZE = DATA[i].d_requestSize;
            const int NUMREQ  = DATA[i].d_numRequests;

            const int MAXNUMREQ  = TH + 2;
            LOOP2_ASSERT(MAXNUMREQ, NUMREQ, MAXNUMREQ >= NUMREQ);
            if (MAXNUMREQ < NUMREQ) continue;

            // Try each test using both maximum and natural alignment

            typedef bsls_Alignment::Strategy AlignmentStrategy;
            AlignmentStrategy strategy;
            for (strategy = bsls_Alignment::BSLS_MAXIMUM;
                 strategy <= bsls_Alignment::BSLS_NATURAL;
                 strategy = (AlignmentStrategy) (strategy + 1)) {

                bslma_TestAllocator ta(veryVeryVerbose);

                Obj pool(BUFSIZE, strategy, &ta);
                ASSERT(1 == ta.numBlocksInUse());
                ASSERT(blockSize(BUFSIZE) == ta.numBytesInUse());

                // Make NUMREQ requests for memory, recording how the allocator
                // was used after each request.

                for (int reqNum = 0; reqNum < NUMREQ; ++reqNum) {
                    void *returnAddr = pool.allocate(REQSIZE);
                    LOOP2_ASSERT(LINE, reqNum, returnAddr);
                    LOOP2_ASSERT(LINE, reqNum, ta.numBlocksInUse());

                    if (veryVerbose) {
                        P_(reqNum) P(returnAddr);
                    }
                } // end for (each request)

                // Now reset and verify that all memory is returned.

                pool.release();
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(0 == ta.numBytesInUse());

            } // end for (each alignment strategy)
        } // end for (each test vector)
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'allocate' TEST:
        //   Note that this test alone is insufficient to thoroughly test the
        //   the 'allocate' method, as the constructor, which is not thoroughly
        //   tested yet, is used in this test.  However, a combination of both
        //   this test and test case 2 provides complete test coverage for both
        //   the constructor and the 'allocate' method.
        //
        // Concerns:
        //   1) If 'initialSize' is specified then all requests up to
        //      'initialSize' require no additional allocations.  Subsequent
        //      buffer resizes are constant or geometric based on the specified
        //      growth strategy.
        //
        //   2) All requests over a specified THRESHOLD are satisfied directly
        //      from the blockList if the they cannot be satisfied by the
        //      pool's internal buffer.
        //
        // Plan:
        //   For concerns 1 and 2 construct objects mX, mY and mZ with default,
        //   maximum and natural alignment allocation strategy using the two
        //   different constructors, namely the default constructor and the
        //   constructor taking an 'initialSize'.  Additionally pass a test
        //   allocator to the constructor to monitor the memory allocations by
        //   the pool.  Confirm the bytes allocated by the constructor are as
        //   expected.
        //
        // Testing:
        //   void *allocate(int size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'allocate' TEST" << endl
                                  << "===============" << endl;

        const int DATA[]   = { 2, 5, 7, 8, 15, 16, 24, 31, 32, 33, 48,
                               63, 64, 65, 66, 127, 128, 129, 255, 256,
                               511, 512, 1023, 1024, 1025 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsls_Alignment::Strategy MAX = bsls_Alignment::BSLS_MAXIMUM;
        bsls_Alignment::Strategy NAT = bsls_Alignment::BSLS_NATURAL;

        if (verbose) cout << "\nTesting constructor without allocation."
                          << endl;
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];

                bslma_TestAllocator ta(veryVeryVerbose),
                                    tb(veryVeryVerbose),
                                    tc(veryVeryVerbose);
                Obj mX(&ta);
                Obj mY(MAX, &tb);
                Obj mZ(NAT, &tc);

                mX.allocate(SIZE);
                mY.allocate(SIZE);
                mZ.allocate(SIZE);

                if (0 == SIZE) {
                    continue;
                }

                if (SIZE <= DEFAULT_SIZE * 2) {
                    LOOP2_ASSERT(i, SIZE,
                            blockSize(DEFAULT_SIZE * 2) == ta.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                            blockSize(DEFAULT_SIZE * 2) == tb.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                            blockSize(DEFAULT_SIZE * 2) == tc.numBytesInUse());
                }
                else {
                    int nextSize = calculateNextSize(DEFAULT_SIZE, SIZE);
                    LOOP4_ASSERT(i, SIZE, blockSize(nextSize),
                                 ta.numBytesInUse(),
                                 blockSize(nextSize) == ta.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                                 blockSize(nextSize) == tb.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                                 blockSize(nextSize) == tc.numBytesInUse());
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

#define GEO bsls_BlockGrowth::BSLS_GEOMETRIC
#define CON bsls_BlockGrowth::BSLS_CONSTANT

                const bsls_BlockGrowth::Strategy STRAT[] = { GEO, GEO, GEO,
                                                             CON, CON, CON };

#undef CON
#undef GEO

                const int NUM_INITIAL_SIZES = sizeof  INITIAL_SIZES
                                            / sizeof *INITIAL_SIZES;

                for (int j = 0; j < NUM_INITIAL_SIZES; ++j) {
                    const int INITIAL_SIZE = INITIAL_SIZES[j];
                    const bsls_BlockGrowth::Strategy STRATEGY = STRAT[j];

                    bslma_TestAllocator ta(veryVeryVerbose),
                                        tb(veryVeryVerbose),
                                        tc(veryVeryVerbose);

                    if (veryVerbose) {
                        P(INITIAL_SIZE) P(SIZE)
                    }

                    Obj mX(INITIAL_SIZE, STRATEGY, &ta);
                    Obj mY(INITIAL_SIZE, STRATEGY, MAX, &tb);
                    Obj mZ(INITIAL_SIZE, STRATEGY, NAT, &tc);

                    const int NA = ta.numBytesInUse();
                    const int NB = tb.numBytesInUse();
                    const int NC = tc.numBytesInUse();

                    mX.allocate(SIZE);
                    mY.allocate(SIZE);
                    mZ.allocate(SIZE);

                    if (SIZE <= bsl::abs(INITIAL_SIZE)) {
                        LOOP_ASSERT(i, NA == ta.numBytesInUse());
                        LOOP_ASSERT(i, NB == tb.numBytesInUse());
                        LOOP_ASSERT(i, NC == tc.numBytesInUse());
                    }
                    else if (bsls_BlockGrowth::BSLS_GEOMETRIC == STRAT[j]) {
                        int nextSize = calculateNextSize(INITIAL_SIZE, SIZE);
                        LOOP3_ASSERT(i, NA + blockSize(nextSize),
                                     ta.numBytesInUse(),
                                     NA + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                        LOOP_ASSERT(i, NB + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                        LOOP_ASSERT(i, NC + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                    }
                    else {
                        if (0 == INITIAL_SIZE && SIZE < DEFAULT_SIZE) {
                            LOOP_ASSERT(i, NA + blockSize(DEFAULT_SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, NB + blockSize(DEFAULT_SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, NC + blockSize(DEFAULT_SIZE)
                                                        == ta.numBytesInUse());
                        }
                        else {
                            LOOP_ASSERT(i, NA + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, NB + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, NC + blockSize(SIZE)
                                                        == ta.numBytesInUse());
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

#define GEO bsls_BlockGrowth::BSLS_GEOMETRIC
#define CON bsls_BlockGrowth::BSLS_CONSTANT

                const bsls_BlockGrowth::Strategy STRATEGIES[] = {
                                                              GEO, GEO, GEO,
                                                              CON, CON, CON };

#undef CON
#undef GEO


                const int NUM_INITIAL_SIZES = sizeof  INITIAL_SIZES
                                            / sizeof *INITIAL_SIZES;

                for (int j = 0; j < NUM_INITIAL_SIZES; ++j) {
                    const int INITIAL_SIZE = INITIAL_SIZES[j];
                    const bsls_BlockGrowth::Strategy STRATEGY = STRATEGIES[j];

                    bslma_TestAllocator ta(veryVeryVerbose),
                                        tb(veryVeryVerbose),
                                        tc(veryVeryVerbose);

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

                            Obj mX(INITIAL_SIZE, MAX_SIZE, STRATEGY, &ta);
                            Obj mY(INITIAL_SIZE, MAX_SIZE, STRATEGY, MAX, &tb);
                            Obj mZ(INITIAL_SIZE, MAX_SIZE, STRATEGY, NAT, &tc);

                            const int NA = ta.numBytesInUse();
                            const int NB = tb.numBytesInUse();
                            const int NC = tc.numBytesInUse();

                            mX.allocate(ALLOC_SIZE);
                            mY.allocate(ALLOC_SIZE);
                            mZ.allocate(ALLOC_SIZE);

                            if (ALLOC_SIZE <= INITIAL_SIZE) {
                                LOOP_ASSERT(i, NA == ta.numBytesInUse());
                                LOOP_ASSERT(i, NB == tb.numBytesInUse());
                                LOOP_ASSERT(i, NC == tc.numBytesInUse());
                            }
                            else if (bsls_BlockGrowth::BSLS_GEOMETRIC ==
                                                                    STRATEGY) {
                                if (ALLOC_SIZE < MAX_SIZE) {
                                    int nextSize = calculateNextSize(
                                                     INITIAL_SIZE, ALLOC_SIZE);
                                    LOOP_ASSERT(i, NA + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                                    LOOP_ASSERT(i, NB + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                                    LOOP_ASSERT(i, NC + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                                }
                                else {
                                    LOOP_ASSERT(i, NA + blockSize(ALLOC_SIZE)
                                                        == ta.numBytesInUse());
                                    LOOP_ASSERT(i, NB + blockSize(ALLOC_SIZE)
                                                        == ta.numBytesInUse());
                                    LOOP_ASSERT(i, NC + blockSize(ALLOC_SIZE)
                                                        == ta.numBytesInUse());
                                }
                            }
                            else {
                                LOOP_ASSERT(i, NA + blockSize(ALLOC_SIZE)
                                                    == ta.numBytesInUse());
                                LOOP_ASSERT(i, NB + blockSize(ALLOC_SIZE)
                                                    == ta.numBytesInUse());
                                LOOP_ASSERT(i, NC + blockSize(ALLOC_SIZE)
                                                    == ta.numBytesInUse());
                            }
                        }
                    }
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CTOR TEST
        //   Note that this test alone is insufficient to thoroughly test the
        //   the constructor, as the untested 'allocate' method is used to
        //   verify that the buffer, alignment strategy and allocator are
        //   properly passed to the pool at construction.  However, we cannot
        //   test 'allocate' first - as it requires the constructor.  Hence,
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
        //   4) That when initial size is not specified, the
        //      'bdema_SequentialPool' does not allocate memory at
        //      construction.
        //
        //   5) That if initial size is specified, the pool allocate the
        //      specified amount of memory at construction.
        //
        //   6) That the proper growth strategy, alignment strategy and
        //      allocator are used if specified at construction.
        //
        // Plan:
        //
        // Testing:
        //  *bdema_SequentialPool(bslma_Allocator *a = 0)
        //   bdema_SequentialPool(GS g, bslma_Allocator *a = 0)
        //  *bdema_SequentialPool(AS a, bslma_Allocator *a = 0)
        //   bdema_SequentialPool(GS g, AS a, bslma_Allocator *a = 0)
        //
        //  *bdema_SequentialPool(int i, bslma_Allocator *a = 0)
        //   bdema_SequentialPool(int i, GS g, bslma_Allocator *a = 0)
        //   bdema_SequentialPool(int i, AS a, bslma_Allocator *a = 0)
        //  *bdema_SequentialPool(int i, GS g, AS a, bslma_Allocator *a = 0)
        //
        //  *bdema_SequentialPool(int i, int m, bslma_Allocator *a = 0)
        //   bdema_SequentialPool(int i, int m, GS g, bslma_Allocator *a = 0)
        //   bdema_SequentialPool(int i, int m, AS a, bslma_Allocator *a = 0)
        //  *bdema_SequentialPool(int, int, GS g, AS a, bslma_Allocator *a = 0)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR TEST" << endl
                                  << "=========" << endl;

#define GEO bsls_BlockGrowth::BSLS_GEOMETRIC
#define CON bsls_BlockGrowth::BSLS_CONSTANT
#define NAT bsls_Alignment::BSLS_NATURAL
#define MAX bsls_Alignment::BSLS_MAXIMUM
        const int INITIAL_SIZE = 64;
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
                void *addr1 = mA.allocate(DEFAULT_SIZE + 1);
                addr1 = mA.allocate(DEFAULT_SIZE + 1);  // triggers allocation

                ASSERT(blockSize(DEFAULT_SIZE * 2) +
                       blockSize(calculateNextSize(DEFAULT_SIZE * 2,
                                                   DEFAULT_SIZE + 1))
                                        == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mB(NAT);

                ASSERT(0 == defaultAllocator.numBytesInUse());
                void *addr1 = mB.allocate(DEFAULT_SIZE + 1);
                addr1 = mB.allocate(DEFAULT_SIZE + 1);  // triggers allocation

                ASSERT(blockSize(DEFAULT_SIZE * 2) +
                       blockSize(calculateNextSize(DEFAULT_SIZE * 2,
                                                   DEFAULT_SIZE + 1))
                                        == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mC(INITIAL_SIZE);

                // Check initial memory allocation:
                ASSERT(blockSize(INITIAL_SIZE) ==
                                             defaultAllocator.numBytesInUse());

                void *addr1 = mC.allocate(INITIAL_SIZE + 1);  // trigger

                ASSERT(blockSize(INITIAL_SIZE) +
                       blockSize(calculateNextSize(INITIAL_SIZE,
                                                   INITIAL_SIZE + 1))
                                        == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mD(INITIAL_SIZE, NAT);

                // Check initial memory allocation:
                ASSERT(blockSize(INITIAL_SIZE) ==
                                             defaultAllocator.numBytesInUse());

                void *addr1 = mD.allocate(INITIAL_SIZE + 1);  // trigger

                ASSERT(blockSize(INITIAL_SIZE) +
                       blockSize(calculateNextSize(INITIAL_SIZE,
                                                   INITIAL_SIZE + 1))
                                        == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mE(INITIAL_SIZE, MAX_BUFFER);

                // Check initial memory allocation:
                ASSERT(blockSize(INITIAL_SIZE) ==
                                             defaultAllocator.numBytesInUse());

                void *addr1 = mE.allocate(INITIAL_SIZE + 1);  // trigger

                ASSERT(blockSize(INITIAL_SIZE) +
                       blockSize(calculateNextSize(INITIAL_SIZE,
                                                   INITIAL_SIZE + 1))
                                        == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mF(INITIAL_SIZE, MAX_BUFFER, MAX);

                // Check initial memory allocation:
                ASSERT(blockSize(INITIAL_SIZE) ==
                                             defaultAllocator.numBytesInUse());

                void *addr1 = mF.allocate(INITIAL_SIZE + 1);  // trigger

                ASSERT(blockSize(INITIAL_SIZE) +
                       blockSize(calculateNextSize(INITIAL_SIZE,
                                                   INITIAL_SIZE + 1))
                                        == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nTesting that when initial size not specified, "
                        "there will be no allocation at construction." << endl;
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
            typedef bsls_Alignment::Strategy St;
            enum { MAX_ALIGN = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT };

            static const struct {
                int d_line;       // line number
                St  d_strategy;   // alignment strategy
                int d_allocSize;  // size of allocation request
                int d_expOffset;  // expected address of second allocation
            } DATA[] = {
                // LINE     STRAT       ALLOCSIZE       EXPOFFSET
                // ----     -----       ---------       ---------

                // NATURAL ALIGNMENT
                {  L_,      NAT,         1,             1         },
                {  L_,      NAT,         2,             2         },
                {  L_,      NAT,         3,             1         },
                {  L_,      NAT,         4,             4         },
                {  L_,      NAT,         7,             1         },
                {  L_,      NAT,         8,             8         },
                {  L_,      NAT,        15,             1         },
                {  L_,      NAT,        16,             MAX_ALIGN },

                // MAXIMUM ALIGNMENT
                {  L_,      MAX,         1,             MAX_ALIGN },
                {  L_,      MAX,         2,             MAX_ALIGN },
                {  L_,      MAX,         3,             MAX_ALIGN },
                {  L_,      MAX,         4,             MAX_ALIGN },
                {  L_,      MAX,         7,             MAX_ALIGN },
                {  L_,      MAX,         8,             MAX_ALIGN },
                {  L_,      MAX,        15,             MAX_ALIGN },
                {  L_,      MAX,        16,             MAX_ALIGN },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE      = DATA[ti].d_line;
                const St  STRAT     = DATA[ti].d_strategy;
                const int ALLOCSIZE = DATA[ti].d_allocSize;
                const int EXPOFFSET = DATA[ti].d_expOffset;

                ASSERT(0 == objectAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numBytesInUse());
                ASSERT(0 == globalAllocator.numBytesInUse());

                Obj pool(STRAT, &objectAllocator);

                ASSERT(0 == objectAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numBytesInUse());
                ASSERT(0 == globalAllocator.numBytesInUse());

                char *addr1 = (char *)pool.allocate(1);
                char *addr2 = (char *)pool.allocate(ALLOCSIZE);

                ASSERT(0 != objectAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numBytesInUse());
                ASSERT(0 == globalAllocator.numBytesInUse());

                LOOP2_ASSERT(addr1 + EXPOFFSET, addr2,
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

            Obj pool(GEO, &objectAllocator);

            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());

            pool.allocate(DEFAULT_SIZE + 1);
            pool.allocate(DEFAULT_SIZE + 1);  // triggers allocation

            ASSERT(0 != objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());

            int totalSize = blockSize(DEFAULT_SIZE * 2) +
                            blockSize(calculateNextSize(DEFAULT_SIZE * 2,
                                                        DEFAULT_SIZE + 1));

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

            Obj pool(CON, &objectAllocator);

            ASSERT(0 == objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());

            pool.allocate(DEFAULT_SIZE / 2 + 1);
            pool.allocate(DEFAULT_SIZE / 2 + 1);  // triggers allocation.

            ASSERT(0 != objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBytesInUse());
            ASSERT(0 == globalAllocator.numBytesInUse());

            int totalSize = blockSize(DEFAULT_SIZE) +
                            blockSize(DEFAULT_SIZE);

            LOOP2_ASSERT(totalSize, objectAllocator.numBytesInUse(),
                                 totalSize == objectAllocator.numBytesInUse());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 == globalAllocator.numBytesInUse());

#undef GEO
#undef CON
#undef NAT
#undef MAX

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // HELPER FUNCTION TEST
        //
        // Concerns:
        //   That the size returned matches the amount of bytes allocated by
        //   the 'bdema_InfrequentDeleteBlockList::allocate' method.
        //
        // Plan:
        //   Create a 'bslma_BlockList' object initialized with a test
        //   allocator.  Invoke both the 'blockSize' function and the
        //   'bslma_BlockList::allocate' method with varying memory sizes, and
        //   verify that the sizes returned by 'blockSize' are equal to the
        //   memory request sizes recorded by the allocator.
        //
        // Testing:
        //   HELPER FUNCTION: 'int blockSize(numBytes)'
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER FUNCTION TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "\nTesting 'blockSize'." << endl;

        const int DATA[] = { 0, 1, 5, 12, 24, 64, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma_TestAllocator a(veryVeryVerbose);
        bdema_InfrequentDeleteBlockList bl(&a);
        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            int blkSize = blockSize(SIZE);
            bl.allocate(SIZE);

            // If the first 'SIZE' is 0, the allocator's 'allocate' is never
            // called, thus, 'lastAllocateddSize' will return -1 instead of 0.

            const int EXP = i || SIZE ? a.lastAllocatedNumBytes() : 0;

            if (veryVerbose) { T_ P_(SIZE); T_ P_(blkSize); T_ P(EXP); }
            LOOP_ASSERT(i, EXP == blkSize);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   1) That a 'bdema_SequentialPool' can be created and destroyed.
        //
        //   2) That 'allocate' returns a block of memory having the specified
        //      size and alignment strategy.
        //
        //   2) That 'allocate' does not always causes dynamic allocation
        //      (i.e., the pool manages an internal buffer of memory)
        //
        //   3) That 'allocate' returns a block of memory even when the
        //      the allocation request exceeds the initial size of the internal
        //      buffer.
        //
        //   4) Destruction of the pool releases all managed memory.
        //
        // Plan:
        //   First, initialize a 'bdema_SequentialPool' with a
        //   'bslma_TestAllocator' (concern 1).  Then, allocate a block of
        //   memory, and verify that it comes from the test allocator.
        //   Allocate another block of memory, and verify that no dynamic
        //   allocation is triggered (concern 3).  Verify the alignment and
        //   size of the first allocation by checking the address of the
        //   second allocation (concern 2).
        //
        //   Then, allocate a large block of memory and verify the memory comes
        //   from the test allocator (concern 4).  Finally, destroy the pool
        //   and check that all allocated memory are deallocated (concern 5).
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATING TEST" << endl
                                  << "=============" << endl;

        enum { ALLOC_SIZE1 = 4, ALLOC_SIZE2 = 8, ALLOC_SIZE3 = 1024 };

        {
            if (verbose) cout << "\nTesting construction of pool." << endl;
            Obj pool(&objectAllocator);

            // Make sure no memory comes from the object, default and global
            // allocators.
            ASSERT(0 == objectAllocator.numBlocksTotal());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            if (verbose) cout << "\nTesting allocation." << endl;
            void *addr1 = pool.allocate(ALLOC_SIZE1);

            ASSERT(0 != objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            int oldNumBytesInUse = objectAllocator.numBytesInUse();

            if (verbose) cout << "\nTesting internal buffering." << endl;
            void *addr2 = pool.allocate(ALLOC_SIZE2);

            ASSERT(oldNumBytesInUse == objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            if (verbose) cout << "\nTesting alignment strategy." << endl;

            // Check for alignment and size of first allocation)
            ASSERT((char *)addr1 + 8 == (char *)addr2);

            if (verbose) cout << "\nTesting large allocation." << endl;
            void *addr3 = pool.allocate(ALLOC_SIZE3);

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
