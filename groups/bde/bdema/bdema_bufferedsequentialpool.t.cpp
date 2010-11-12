// bdema_bufferedsequentialpool.t.cpp                                 -*-C++-*-
#include <bdema_bufferedsequentialpool.h>

#include <bdema_bufferimputil.h>

#include <bslma_defaultallocatorguard.h>
#include <bdema_infrequentdeleteblocklist.h>
#include <bslma_testallocator.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignment.h>
#include <bsls_alignmentutil.h>

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
// A 'bdema_BufferedSequentialPool' is a mechanism (i.e., having state but no
// value) that is used as a memory manager to manage an external buffer and
// dynamically allocated memory (if the external buffer is full).  The primary
// concern is that 'bdema_BufferedSequentialPool' returns memory from an
// appropriate source (the external buffer supplied at construction, or a
// dynamically allocated buffer if the external buffer is full), and respect
// the appropriate alignment strategy (also specified at construction).
//
// Because 'bdema_BufferedSequentialPool' does not have any accessors, this
// test driver verifies the correctness of the pool's allocations *indirectly*
// through the use of two consecutive allocations - where the first allocation
// tests for the correctness of 'allocate', and the second verifies the
// size of the first allocation and the memory alignment strategy.
//
// Finally, we make heavy use of the 'bslma_TestAllocator' to ensure that:
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
// [3] bdema_BufferedSequentialPool(char *b, int s, bslma_Allocator *a = 0)
// [3] bdema_BufferedSequentialPool(char *b, int s,
//                            AlignmentStrategy st, bslma_Allocator *a = 0)
// [7] ~bdema_BufferedSequentialPool
//
// // MANIPULATORS
// [4] void *allocate(int size)
// [6] void deleteObjectRaw(const TYPE *object)
// [6] void deleteObject(const TYPE *object)
// [5] void release()
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [2] HELPER FUNCTION: 'int blockSize(numBytes)'
// [8] FREE FUNCTION: 'operator new(size_t, bdema_BufferedSequentialPool)'
// [9] USAGE TEST
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
typedef bdema_BufferedSequentialPool Obj;

bool globalDestructorInvoked = false;

// On windows, when a 8-byte aligned object is created on the stack, it
// actually get aligned on 4-byte boundary.  To fix this, create a static
// buffer instead.
enum { BUFFER_SIZE = 256 };
static bsls_AlignedBuffer<BUFFER_SIZE> bufferStorage;

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
///Example 1: Using 'bdema_BufferedSequentialPool' for Efficient Allocations
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose we define a container class, 'my_BufferedIntDoubleArray', that holds
// both 'int' and 'double' values.  The class can be implemented using two
// parallel arrays: one storing the type information, and the other storing
// pointers to the 'int' and 'double' values.  Furthermore, if we can
// approximate the amount of memory needed, we can use a
// 'bdema_BufferedSequentialPool' for memory allocation for maximum efficiency:
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

        bdema_BufferedSequentialPool
               d_pool;          // buffered sequential memory pool used to
                                // supply memory

      private:
        // PRIVATE MANIPULATORS
        void increaseSize();
            // Increase the capacity of the internal arrays used to store
            // elements added to this array by at least one element.

      public:
        // TYPES
        enum Type { MY_INT, MY_DOUBLE };

        // CREATORS
        my_BufferedIntDoubleArray(char            *buffer,
                                  int              size,
                                  bslma_Allocator *basicAllocator = 0);
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
                           // buffer supplied at construction.

        d_length = 0;
    }
//..
// The buffered sequential pool optimizes the allocation of memory by using a
// buffer supplied at construction.  As described in the "Description" section,
// the need for *all* dynamic memory allocations are eliminated provided that
// the buffer is not exhausted.  The pool provides maximal memory allocation
// efficiency:
//..
//  // my_bufferedintdoublearray.cpp
//  #include <my_bufferedintdoublearray.h>

    enum { INITIAL_SIZE = 1, GROWTH_FACTOR = 2 };

    // CREATORS
    my_BufferedIntDoubleArray::my_BufferedIntDoubleArray(
                                               char            *buffer,
                                               int              size,
                                               bslma_Allocator *basicAllocator)
    : d_length(INITIAL_SIZE)
    , d_capacity(INITIAL_SIZE)
    , d_pool(buffer, size, basicAllocator)
    {
        d_typeArray_p  = static_cast<char *>(
                            d_pool.allocate(d_length * sizeof *d_typeArray_p));
        d_valueArray_p = static_cast<void **>(
                           d_pool.allocate(d_length * sizeof *d_valueArray_p));
    }
//..
// Note that in the destructor, all outstanding memory blocks are deallocated
// automatically when 'd_pool' is destroyed:
//..
    my_BufferedIntDoubleArray::~my_BufferedIntDoubleArray()
    {
        ASSERT(d_typeArray_p);
        ASSERT(d_valueArray_p);
        ASSERT(0 <= d_length);
        ASSERT(0 <= d_capacity);
        ASSERT(d_length <= d_capacity);
    }

    // MANIPULATORS
    void my_BufferedIntDoubleArray::appendInt(int value)
    {
        if (d_length >= d_capacity) {
            increaseSize();
        }

        int *item = static_cast<int *>(d_pool.allocate(sizeof *item));
        *item = value;

        d_typeArray_p[d_length] = static_cast<char>
                                           (my_BufferedIntDoubleArray::MY_INT);
        d_valueArray_p[d_length] = item;

        ++d_length;
    }

    void my_BufferedIntDoubleArray::appendDouble(double value)
    {
        if (d_length >= d_capacity) {
            increaseSize();
        }

        double *item = static_cast<double *>(d_pool.allocate(sizeof *item));
        *item = value;

        d_typeArray_p[d_length] = static_cast<char>(
                                         my_BufferedIntDoubleArray::MY_DOUBLE);
        d_valueArray_p[d_length] = item;

        ++d_length;
    }

    void my_BufferedIntDoubleArray::increaseSize()
    {
        // Implementation elided.
        // ...
    }
//..
///Example 2: Implementing an Allocator Using 'bdema_BufferedSequentialPool'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bslma_Allocator' is used throughout the interfaces of BDE components.
// Suppose we would like to create a fast allocator, 'my_FastAllocator', that
// allocates memory from a buffer in a similar fashion to
// 'bdema_BufferedSequentialPool'.  This class can be used directly to
// implement such an allocator.
//
// Note that the documentation for this class is simplified for this usage
// example.  Please see 'bdema_bufferedsequentialallocator' for full
// documentation of a similar class.
//..
    class my_FastAllocator : public bslma_Allocator {
        // This class implements the 'bslma_Allocator' protocol to provide a
        // fast allocator of heterogeneous blocks of memory (of varying,
        // user-specified sizes) from an external buffer whose address and size
        // are supplied at construction.

        // DATA
        bdema_BufferedSequentialPool d_pool;  // memory manager for allocated
                                              // memory blocks

        // CREATORS
        my_FastAllocator(char            *buffer,
                         int              size,
                         bslma_Allocator *basicAllocator = 0);
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
        virtual void *allocate(int size);
            // Return the address of a contiguous block of memory of the
            // specified 'size' (in bytes).

        virtual void deallocate(void *address);
            // This method has no effect as all memory allocated by this
            // allocator is managed.
    };

    // CREATORS
    inline
    my_FastAllocator::my_FastAllocator(char            *buffer,
                                       int              size,
                                       bslma_Allocator *basicAllocator)
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
    void *my_FastAllocator::allocate(int size)
    {
        return d_pool.allocate(size);
    }

    inline
    void my_FastAllocator::deallocate(void *address)
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
      case 9: {
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
        //   USAGE TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE TEST" << endl
                                  << "==================" << endl;

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // GLOBAL OPERATOR NEW TEST
        //
        // Concerns:
        //   That a 'bdema_BufferedSequentialPool' can be used directly with
        //   'operator new'.
        //
        // Plan:
        //   Since 'bdema_BufferedSequentialPool' is thoroughly tested at this
        //   point, we just need to make sure that 'new' forwards the memory
        //   request directly.
        //
        // Testing:
        //   operator new(size_t, bdema_BufferedSequentialPool&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "GLOBAL OPERATOR NEW TEST" << endl
                                  << "========================" << endl;

        char buffer[256];
        Obj pool(buffer, 256);

        double *d = new(pool)double(3.0);

        ASSERT(&buffer[0] <= (char *)d);
        ASSERT((char *)d <= &buffer[255]);

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
        //      construction are deallocated after destruction of the pool.
        //
        // Plan:
        //   For concern 1, dynamically allocate a buffer using a test
        //   allocator.  Then, set all bytes in the buffer to '0xA', and
        //   initialize a buffered sequential pool using the buffer and the
        //   same test allocator.  Finally, destroy the pool, and verify that
        //   the bytes in the first buffer remains '0xA' and the buffer is not
        //   deallocated.
        //
        //   For concern 2, construct a buffered sequential pool using a
        //   'bslma_TestAllocator', allocate sufficient memory such that the
        //   buffer runs out and the allocator is used.  Finally, destroy
        //   the pool, and verify, using the test allocator, that there is
        //   no outstanding memory allocated.
        //
        // Testing:
        //   ~bdema_BufferedSequentialPool()
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DTOR TEST" << endl
                                  << "=========" << endl;

        enum { BUFFER_SIZE = 256 };

        if (verbose) cout << "\nTesting managed buffer is not affected after "
                             "destruction." << endl;
        {
            char bufferRef[BUFFER_SIZE];
            int total = 0;

            ASSERT(0 == objectAllocator.numBlocksInUse());
            char *buffer = (char *)objectAllocator.allocate(BUFFER_SIZE);
            total = objectAllocator.numBlocksInUse();

            {
                memset(buffer,    0xA, BUFFER_SIZE);
                memset(bufferRef, 0xA, BUFFER_SIZE);

                Obj pool(buffer, BUFFER_SIZE, &objectAllocator);

                ASSERT(total == objectAllocator.numBlocksInUse());

                // Allocate some memory.
                pool.allocate(1);
                pool.allocate(16);
                ASSERT(total == objectAllocator.numBlocksInUse());
            }
            ASSERT(total == objectAllocator.numBlocksInUse());
            ASSERT(0 == memcmp(buffer, bufferRef, BUFFER_SIZE));

            objectAllocator.deallocate(buffer);
            ASSERT(0 == objectAllocator.numBlocksInUse());
        }

        if (verbose) cout << "\nTesting allocated memory are deallocated after"
                             " destruction." << endl;
        {
            char buffer[BUFFER_SIZE];

            ASSERT(0 == objectAllocator.numBlocksInUse());

            Obj pool(buffer, BUFFER_SIZE, &objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksInUse());

            // Allocate some memory such that memory is allocated from the test
            // allocator.
            pool.allocate(BUFFER_SIZE + 1);
            pool.allocate(1);
            pool.allocate(16);
            ASSERT(0 != objectAllocator.numBlocksInUse());
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());

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
            char buf[256];
            Obj pool(buf, 256);

            void *addr = pool.allocate(sizeof(my_Class));
            my_Class *obj = new(addr)my_Class();

            globalDestructorInvoked = false;
            pool.deleteObject(obj);

            ASSERT(true == globalDestructorInvoked);
        }

        if (verbose) cout << "\nTesting 'deleteObjectRaw'." << endl;
        {
            char buf[256];
            Obj pool(buf, 256);

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
        //   1) That the previously managed buffer is not changed in any way
        //      after 'release'.
        //
        //   2) That all memory allocated from the allocator supplied at
        //      construction are deallocated after 'release'.
        //
        //   3) That subsequent allocation requests after invocation of the
        //      'release' method are satisfied by the allocator supplied at
        //      construction.
        //
        // Plan:
        //   For concern 1, dynamically allocate a buffer using a test
        //   allocator.  Then, set all bytes in the buffer to '0xA', and
        //   initialize a buffered sequential pool using the buffer and the
        //   same test allocator.  Finally, invoke 'release', and verify that
        //   the bytes in the first buffer remains '0xA' and the buffer is not
        //   deallocated.
        //
        //   For concern 2 and 3, construct a buffered sequential pool using a
        //   'bslma_TestAllocator', allocate sufficient memory such that the
        //   buffer runs out and the allocator is used.  Finally, invoke
        //   'release' and verify, using the test allocator, that there is
        //   no outstanding memory allocated.  Then, allocate memory again and
        //   verify memory comes from the test allocator.
        //
        // Testing:
        //   void release()
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' TEST" << endl
                                  << "==============" << endl;

        enum { BUFFER_SIZE = 256 };

        if (verbose) cout << "\nTesting managed buffer is not affected after "
                             "'release'." << endl;

        {
            ASSERT(0 == objectAllocator.numBlocksInUse());

            char *buffer = (char *)objectAllocator.allocate(BUFFER_SIZE);
            char bufferRef[BUFFER_SIZE];

            int total = objectAllocator.numBlocksInUse();

            memset(buffer,    0xA, BUFFER_SIZE);
            memset(bufferRef, 0xA, BUFFER_SIZE);

            Obj pool(buffer, BUFFER_SIZE, &objectAllocator);

            ASSERT(total == objectAllocator.numBlocksInUse());

            // Allocate some memory.
            pool.allocate(1);
            pool.allocate(16);
            ASSERT(total == objectAllocator.numBlocksInUse());

            // Release all memory.
            pool.release();
            ASSERT(total == objectAllocator.numBlocksInUse());

            ASSERT(0 == memcmp(buffer, bufferRef, BUFFER_SIZE));

            objectAllocator.deallocate(buffer);
        }
        ASSERT(0 == objectAllocator.numBlocksInUse());

        if (verbose) cout << "\nTesting allocated memory are deallocated after"
                             " 'release'." << endl;
        {
            char buffer[BUFFER_SIZE];

            ASSERT(0 == objectAllocator.numBlocksInUse());

            Obj pool(buffer, BUFFER_SIZE, &objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksInUse());

            // Allocate some memory such that memory is allocated from the test
            // allocator.
            pool.allocate(BUFFER_SIZE + 1);
            pool.allocate(1);
            pool.allocate(16);

            ASSERT(0 != objectAllocator.numBlocksInUse());

            // Release all memory.
            pool.release();

            ASSERT(0 == objectAllocator.numBlocksInUse());

            if (verbose) cout << "\nTesting subsequent allocations comes"
                                 " first from the initial buffer." << endl;
            void *addr = pool.allocate(16);

            ASSERT(&buffer[0] <= addr);
            ASSERT(&buffer[0] + BUFFER_SIZE > addr);
            ASSERT(0 == objectAllocator.numBlocksInUse());
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // 'allocate' TEST
        //   Note that this test alone is insufficient to thoroughly test the
        //   the 'allocate' method, as the constructor, which is not thoroughly
        //   tested yet, is used in this test.  However, a combination of both
        //   this test and test case 2 provides complete test coverage for both
        //   the constructor and the 'allocate' method.
        //
        // Concerns:
        //   1) A 'bdema_BufferedSequentialPool' takes an external buffer
        //      supplied at construction.  This buffer is used for
        //      allocations until it runs out of memory.  Further allocations
        //      use the internal block list.
        //
        //   2) Due to the need to return aligned memory, a user supplied
        //      optional buffer may have additional memory (enough to fulfill
        //      the request) but unable to do so.  In these cases, the rest of
        //      the memory in the static buffer should be discarded and new
        //      memory supplied from the internal block list.
        //
        //   3) Even if the user supplies a static buffer, if the requested
        //      memory exceeds the amount of free memory in the static buffer,
        //      memory should be supplied from the internal block list.
        //      Additional free memory in the buffer is no longer used.
        //
        //   4) 'maxBufferSize' constructor argument caps the internal buffer
        //      growth during allocation.
        //
        // Plan:
        //   1) Supply a static buffer with a constant amount of memory (64
        //      bytes).  Any memory requests that do not exceed 64 bytes should
        //      be allocated from the static buffer.  All values near to 64 are
        //      tested.  Memory allocations less than or equal to 64 bytes
        //      will not cause the internal block list to supply memory.  All
        //      values above 64 will.
        //
        //   2) A static buffer is supplied.  Memory is requested such that
        //      there is still free memory on the list, but when memory is
        //      aligned correctly there is not enough memory to supply the
        //      requested amount of memory.  Thus, this allocation should cause
        //      memory to be supplied from the internal block list.
        //
        //   3) A allocation is requested that is larger than the static buffer
        //      size.  This allocation should request memory from the internal
        //      block list.
        //
        //   4) Using the test allocator, verify the memory used no longer
        //      increases geometrically once the maximum buffer size is used.
        //
        // Testing:
        //   void *allocate(int size)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'allocate' TEST" << endl
                                  << "===============" << endl;

        enum {
            MAJOR_BUFFER_SIZE      = 1024,
            STATIC_BUFFER_SIZE     = 64
        };

        // Block copied from bdema_infrequentdeleteblocklist.h.  This is
        // the amount of additional memory used per allocation.
        struct Block {
            Block                              *d_next_p;
            bsls_AlignmentUtil::MaxAlignedType  d_memory;  // force
                                                           // alignment
        };

        // Align 'buffer' to MaxAlignedType.
        static union {
            char majorBuffer[MAJOR_BUFFER_SIZE];
            bsls_AlignmentUtil::MaxAlignedType  dummy;
        };

        char                     *buffer;
        const int                 bufferSize = STATIC_BUFFER_SIZE;
        char                     *cBuffer;
        bsls_Alignment::Strategy  nat = bsls_Alignment::BSLS_NATURAL;
        bsls_Alignment::Strategy  max = bsls_Alignment::BSLS_MAXIMUM;

        // Move the buffer to the middle.
        buffer = majorBuffer + 128;

        if (verbose) cout << "\nTesting allocation with buffer and allocator."
                          << endl;
        {
            {
                // Allocating 63 bytes, 1 byte under the buffer size.  This
                // allocation should happen on the static buffer.

                Obj mX(buffer, bufferSize, nat, &objectAllocator);
                ASSERT(0       == objectAllocator.numBytesInUse());

                cBuffer =  (char *)mX.allocate(63);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());
                mX.release();

                Obj mY(buffer, bufferSize, max, &objectAllocator);
                ASSERT(0       == objectAllocator.numBytesInUse());

                cBuffer =  (char *)mY.allocate(63);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());
                mY.release();
            }
            {
                // Allocating 64 bytes, exactly the limit.  This allocation
                // should happen on the static buffer because the buffer is
                // aligned to 64 bytes exactly.

                Obj mX(buffer, bufferSize, nat, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer =  (char *)mX.allocate(64);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());
                mX.release();

                Obj mY(buffer, bufferSize, max, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer =  (char *)mY.allocate(64);
                ASSERT(cBuffer == buffer);
                ASSERT(0       == objectAllocator.numBytesInUse());
                mY.release();
            }
            {
                // Allocating 65 bytes, 1 byte over the limit.  This
                // allocation should happen through the allocator.

                Obj mX(buffer, bufferSize, nat, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                int numBytes = 65;
                int newSize  = calculateNextSize(bufferSize, numBytes);
                cBuffer = (char *)mX.allocate(numBytes);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize)  == objectAllocator.numBytesInUse());
                mX.release();

                Obj mY(buffer, bufferSize, max, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer = (char *)mY.allocate(numBytes);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize)  == objectAllocator.numBytesInUse());
                mY.release();
            }
            {
                // Allocating 66 bytes, 2 bytes over the limit.  This
                // allocation should happen through the allocator.

                Obj mX(buffer, bufferSize, nat, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                int numBytes = 66;
                int newSize  = calculateNextSize(bufferSize, numBytes);
                cBuffer = (char *)mX.allocate(numBytes);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize)  == objectAllocator.numBytesInUse());
                mX.release();

                Obj mY(buffer, bufferSize, max, &objectAllocator);
                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer = (char *)mY.allocate(numBytes);
                ASSERT((cBuffer <= buffer) || (cBuffer >= buffer + 64));
                ASSERT(blockSize(newSize)  == objectAllocator.numBytesInUse());
                mY.release();
            }
            {
                // Allocating 63 bytes, then 1 byte, another 1 bytes, and
                // another 1 byte.

                // For natural alignment, the first two allocations should
                // happen on the static buffer, the next allocation should use
                // the allocator, and the last allocation should use the buffer
                // allocated in the previous allocation.  Thus the last
                // allocation should not increase the number of bytes used by
                // the allocator.

                Obj mX(buffer, bufferSize, nat, &objectAllocator);
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

                int   bytesUsed  = objectAllocator.numBytesInUse();
                char *tBuffer    = cBuffer;

                cBuffer        = (char *)mX.allocate(1);
                ASSERT((cBuffer   <= buffer) || (cBuffer  >= buffer + 64));
                ASSERT(cBuffer    == tBuffer + 1);
                ASSERT(bytesUsed  == objectAllocator.numBytesInUse());
                mX.release();

                // For maximum alignment, the first allocation should happen on
                // the static buffer, the next allocation should use the
                // allocator, and the last 2 allocations should use the buffer
                // allocated in the previous allocation.  Thus the last 2
                // allocations should not increase the number of bytes used by
                // the allocator.

                Obj mY(buffer, bufferSize, max, &objectAllocator);
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
                       bsls_AlignmentUtil::calculateAlignmentOffset(
                         tBuffer + 1, bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT));
                ASSERT(bytesUsed  == objectAllocator.numBytesInUse());

                tBuffer = cBuffer;
                cBuffer = (char *)mY.allocate(1);
                ASSERT((cBuffer   <= buffer) || (cBuffer  >= buffer + 64));
                ASSERT(cBuffer    == tBuffer + 1 +
                       bsls_AlignmentUtil::calculateAlignmentOffset(
                         tBuffer + 1, bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT));
                ASSERT(bytesUsed  == objectAllocator.numBytesInUse());
                mY.release();
            }
            ASSERT(0 == objectAllocator.numBytesInUse());
        }

        if (verbose) cout <<
                  "\nTesting alloation with alignment considerations." << endl;
        {
            // Testing maximal alignment
            {
                int blockSize = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

                Obj bufSeqPool(buffer, 8 * blockSize,
                               bsls_Alignment::BSLS_MAXIMUM, &objectAllocator);

                int offset   = 0; // alignment offset

                cBuffer = (char *)bufSeqPool.allocate(1);
                ASSERT(cBuffer == buffer);
                offset += blockSize;

                // BlockSize - 1 bytes wasted.
                cBuffer = (char *)bufSeqPool.allocate(blockSize);

                ASSERT(cBuffer == buffer + offset);
                offset += blockSize;

                cBuffer = (char *)bufSeqPool.allocate(1);
                ASSERT(cBuffer == buffer + offset);
                offset += blockSize;

                // BlockSize - 1 bytes wasted.
                cBuffer = (char *)bufSeqPool.allocate(blockSize);

                ASSERT(cBuffer == buffer + offset);

                bufSeqPool.release();
                ASSERT(0 == objectAllocator.numBytesInUse());
            }

            {
                // Allocate until static buffer is not completely full, but
                // is unable to allocate another block of the desired size
                // because of alignment considerations.

                int blockSize = bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT;

                buffer += blockSize / 2;
                Obj bufSeqPool(buffer, 8 * blockSize, nat, &objectAllocator);

                cBuffer = (char *)bufSeqPool.allocate(
                                            6 * blockSize + blockSize / 2 + 1);
                ASSERT(cBuffer == buffer);

                cBuffer = (char *)bufSeqPool.allocate(blockSize);
                // Block + blockSize/2 - 1  bytes wasted.

                buffer -= 4;
            }
            {
                buffer += 63;
                // Mis align the buffer such that the buffer does not
                // start aligned.  Thus, a 64 byte allocation
                // cannot be allocated off the static buffer, even though
                // there is enough space.

                Obj bufSeqPool(buffer, bufferSize, nat, &objectAllocator);

                ASSERT(0 == objectAllocator.numBytesInUse());

                cBuffer     = (char *)bufSeqPool.allocate(64);
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
            // a internal buffer with constant growth to exceed the maximum
            // buffer size.

            for (int i = 0; i < NUM_DATA; ++i) {

                const int MAXBUFFERSIZE = DATA[i];

                if (veryVerbose) {
                    T_ P_(bufferSize) P(MAXBUFFERSIZE)
                }

                bslma_TestAllocator ta(veryVeryVerbose),
                                    tb(veryVeryVerbose),
                                    tc(veryVeryVerbose);

                // 'allocate' twice before growth stops.

                Obj mX(buffer, bufferSize, MAXBUFFERSIZE, &ta);
                Obj mY(buffer, bufferSize, MAXBUFFERSIZE, nat, &tb);
                Obj mZ(buffer, bufferSize, MAXBUFFERSIZE, max, &tc);

                int nA = ta.numBytesInUse();
                int nB = tb.numBytesInUse();
                int nC = tc.numBytesInUse();

                // Loop until we exceed the max buffer size.  Within the loop,
                // the size of the internal buffers should double in size every
                // time.

                int j = 1;

                int oldNA = 0;
                int oldNB = 0;
                int oldNC = 0;

                for (; j * bufferSize + 1 <= MAXBUFFERSIZE; j <<= 2) {
                    mX.allocate(j * bufferSize + 1);
                    mY.allocate(j * bufferSize + 1);
                    mZ.allocate(j * bufferSize + 1);

                    nA = ta.numBytesInUse();
                    nB = tb.numBytesInUse();
                    nC = tc.numBytesInUse();

                    LOOP3_ASSERT(nA, oldNA, j,
                                 nA == oldNA + blockSize(bufferSize * j * 2));
                    LOOP3_ASSERT(nB, oldNB, j,
                                 nB == oldNB + blockSize(bufferSize * j * 2));
                    LOOP3_ASSERT(nC, oldNC, j,
                                 nC == oldNC + blockSize(bufferSize * j * 2));

                    oldNA = nA;
                    oldNB = nB;
                    oldNC = nC;
                }

                mX.allocate(j * bufferSize);
                mY.allocate(j * bufferSize);
                mZ.allocate(j * bufferSize);

                nA = ta.numBytesInUse();
                nB = tb.numBytesInUse();
                nC = tc.numBytesInUse();

                // Should no longer double in size - note that unlike inside
                // the loop, 'bufferSize * j' is not multiplied by 2.

                LOOP2_ASSERT(nA, oldNA,
                             nA = oldNA + blockSize(bufferSize * j));
                LOOP2_ASSERT(nA, oldNA,
                             nA = oldNA + blockSize(bufferSize * j));
                LOOP2_ASSERT(nA, oldNA,
                             nA = oldNA + bufferSize * j);
            }
        }

        if (verbose) cout << "\nTesting a large allocation request." << endl;
        {
            Obj bufSeqPool(buffer, bufferSize, nat, &objectAllocator);

            ASSERT(0 == objectAllocator.numBytesInUse());

            int numBytes = 65;
            cBuffer = (char *)bufSeqPool.allocate(numBytes);
            int newSize = calculateNextSize(bufferSize, 65);
            ASSERT(blockSize(newSize) == objectAllocator.numBytesInUse());
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
        //   1) That the external buffer supplied at construction is used to
        //      allocate memory.
        //
        //   2) That the allocator supplied at construction is used to supply
        //      memory when the external buffer is full.
        //
        //   3) That the allocator used, when not specified at construction,
        //      is defaulted to the currently installed default allocator.
        //
        //   4) That the alignments strategy, when specified, is respected
        //      during memory allocation.
        //
        //   5) That the alignment strategy, when not specified at
        //      construction, is defaulted to natural alignment.
        //
        // Plan:
        //   1) For concern 1 and 2, first create a 'bslma_TestAllocator' and
        //      a buffer on the stack.  Then create a buffered sequential pool
        //      object using the allocator and the buffer.  Verify that memory
        //      allocated from the pool first comes from the buffer, and when
        //      the buffer cannot satisfy the request, comes from the
        //      allocator.
        //
        //   2) For concern 3, install a 'bslma_TestAllocator' as the default
        //      allocator using a 'bslma_DefaultAllocatorGuard' object.  Then
        //      verify that when an allocator is not specified at construction,
        //      the currently installed default allocator is used.
        //
        //   3) For concern 4 and 5, allocate memory twice from both a buffered
        //      sequential pool and the 'bdema_BufferImpUtil::allocate' method,
        //      and verify the alignment strategy of the buffered sequential
        //      pool by comparing the address of the second allocation.
        //
        // Testing:
        //   bdema_BufferedSequentialPool(char *b,int s,bslma_Allocator *a = 0)
        //   bdema_BufferedSequentialPool(char *b,int s,
        //                        AlignmentStrategy st, bslma_Allocator *a = 0)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CTOR TEST" << endl
                                  << "=========" << endl;

        enum { ALLOC_SIZE1 = 4, ALLOC_SIZE2 = 8 };

        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting external buffer and allocator "
                             "argument." << endl;
        {
            ASSERT(0 == objectAllocator.numBlocksTotal());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            Obj pool(buffer, BUFFER_SIZE, &objectAllocator);

            ASSERT(0 == objectAllocator.numBlocksTotal());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            void *addr = pool.allocate(1);

            // Memory allocation comes from the external buffer.
            LOOP2_ASSERT((void *)&buffer[0], addr, &buffer[0] == addr);
            ASSERT(0 == objectAllocator.numBlocksTotal());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());

            addr = pool.allocate(BUFFER_SIZE);

            // Memory allocation comes from the allocator supplied at
            // construction.
            ASSERT(0 != objectAllocator.numBytesInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());
        }
        ASSERT(0 == objectAllocator.numBytesInUse());
        int total = objectAllocator.numBlocksTotal();

        if (verbose) cout << "\nTesting default allocator argument." << endl;
        {
            ASSERT(total == objectAllocator.numBlocksTotal());
            ASSERT(0     == defaultAllocator.numBlocksTotal());
            ASSERT(0     == globalAllocator.numBlocksTotal());

            Obj pool(buffer, BUFFER_SIZE);

            ASSERT(total == objectAllocator.numBlocksTotal());
            ASSERT(0     == defaultAllocator.numBlocksTotal());
            ASSERT(0     == globalAllocator.numBlocksTotal());

            void *addr = pool.allocate(1);

            // Memory allocation comes from the external buffer.
            LOOP2_ASSERT((void *)&buffer[0], addr, &buffer[0] == addr);
            ASSERT(total == objectAllocator.numBlocksTotal());
            ASSERT(0     == defaultAllocator.numBlocksTotal());
            ASSERT(0     == globalAllocator.numBlocksTotal());

            addr = pool.allocate(BUFFER_SIZE);

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
                Obj pool(buffer,
                         BUFFER_SIZE,
                         bsls_Alignment::BSLS_NATURAL);

                pool.allocate(ALLOC_SIZE1);
                void *addr = pool.allocate(ALLOC_SIZE2);

                int cursor = 0;
                bdema_BufferImpUtil::allocateFromBuffer(
                                                 &cursor,
                                                 buffer,
                                                 BUFFER_SIZE,
                                                 ALLOC_SIZE1,
                                                 bsls_Alignment::BSLS_NATURAL);
                void *addr2 = bdema_BufferImpUtil::allocateFromBuffer(
                                                 &cursor,
                                                 buffer,
                                                 BUFFER_SIZE,
                                                 ALLOC_SIZE2,
                                                 bsls_Alignment::BSLS_NATURAL);

                LOOP2_ASSERT(addr2, addr, addr2 == addr);
            }

            if (verbose) cout << "\tTesting maximum alignment." << endl;
            {
                Obj pool(buffer, BUFFER_SIZE, bsls_Alignment::BSLS_MAXIMUM);

                pool.allocate(ALLOC_SIZE1);
                void *addr = pool.allocate(ALLOC_SIZE2);

                int cursor = 0;
                bdema_BufferImpUtil::allocateFromBuffer(
                                                 &cursor,
                                                 buffer,
                                                 BUFFER_SIZE,
                                                 ALLOC_SIZE1,
                                                 bsls_Alignment::BSLS_MAXIMUM);
                void *addr2 = bdema_BufferImpUtil::allocateFromBuffer(
                                                 &cursor,
                                                 buffer,
                                                 BUFFER_SIZE,
                                                 ALLOC_SIZE2,
                                                 bsls_Alignment::BSLS_MAXIMUM);

                LOOP2_ASSERT(addr2, addr, addr2 == addr);
            }
        }

        if (verbose) cout << "\nTesting default alignment strategy." << endl;
        {
            Obj pool(buffer, BUFFER_SIZE);

            pool.allocate(ALLOC_SIZE1);
            void *addr = pool.allocate(ALLOC_SIZE2);

            int cursor = 0;
            bdema_BufferImpUtil::allocateFromBuffer(
                                                 &cursor,
                                                 buffer,
                                                 BUFFER_SIZE,
                                                 ALLOC_SIZE1,
                                                 bsls_Alignment::BSLS_NATURAL);
            void *addr2 = bdema_BufferImpUtil::allocateFromBuffer(
                                                 &cursor,
                                                 buffer,
                                                 BUFFER_SIZE,
                                                 ALLOC_SIZE2,
                                                 bsls_Alignment::BSLS_NATURAL);

            LOOP2_ASSERT(addr2, addr, addr2 == addr);
        }

        if (verbose) cout << "\nTesting growth strategy." << endl;
        {
            {
                Obj mA(buffer, BUFFER_SIZE, bsls_BlockGrowth::BSLS_GEOMETRIC);
                ASSERT(0 == defaultAllocator.numBytesInUse());

                mA.allocate(BUFFER_SIZE + 1);

                ASSERT(blockSize(BUFFER_SIZE * 2)
                                          == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mA(buffer, BUFFER_SIZE, bsls_BlockGrowth::BSLS_CONSTANT);
                ASSERT(0 == defaultAllocator.numBytesInUse());

                mA.allocate(BUFFER_SIZE);
                ASSERT(0 == defaultAllocator.numBytesInUse());

                mA.allocate(BUFFER_SIZE);
                ASSERT(blockSize(BUFFER_SIZE)
                                          == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

        if (verbose) cout << "\nTesting default growth strategy." << endl;
        {
            {
                Obj mA(buffer, BUFFER_SIZE);
                ASSERT(0 == defaultAllocator.numBytesInUse());

                mA.allocate(BUFFER_SIZE + 1);

                ASSERT(blockSize(BUFFER_SIZE * 2)
                                          == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());

            {
                Obj mA(buffer, BUFFER_SIZE, bsls_Alignment::BSLS_NATURAL);
                ASSERT(0 == defaultAllocator.numBytesInUse());

                mA.allocate(BUFFER_SIZE + 1);

                ASSERT(blockSize(BUFFER_SIZE * 2)
                                          == defaultAllocator.numBytesInUse());
            }
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }

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
            // called, thus, 'lastAllocatedSize' will return -1 instead of 0.

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
        //   1) That a 'bdema_BufferedSequentialPool' can be created and
        //      destroyed.
        //
        //   2) That 'allocate' returns a block of memory having the specified
        //      size and alignment strategy.
        //
        //   3) That 'allocate' returns a block of memory from the external
        //      buffer supplied at construction.
        //
        //   4) That 'allocate' returns a block of memory even when the
        //      the allocation request exceeds the remaining free space in the
        //      external buffer.
        //
        //   5) Destruction of the pool releases all managed memory, including
        //      memory that comes from dynamic allocation.
        //
        // Plan:
        //   For concerns 1, 2, and 3, first, create a buffer on the stack,
        //   then initialize a 'bdema_BufferedSequentialPool' with the buffer.
        //   Next, allocate a block of memory from the pool and verify that it
        //   comes from the external buffer.  Then, allocate another block of
        //   memory from the pool, and verify that the first allocation
        //   returned a block of memory of sufficient size by checking that
        //   'addr2 > addr1 + allocsize1'.  Also verify that the alignment
        //   strategy specified at construction is followed by checking the
        //   address of the second allocation.
        //
        //   For concern 4, initialize a 'bdema_BufferedSequentialPool' with a
        //   'bslma_TestAllocator'.  Then allocate a block of memory that is
        //   larger than the buffer supplied at construction of the buffered
        //   sequential pool.  Verify that memory is allocated from the test
        //   allocator.
        //
        //   For concern 5, let the pool created with the test allocator go
        //   out of scope, and verify, through the test allocator, that all
        //   allocated memory are deallocated.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATING TEST" << endl
                                  << "=============" << endl;

        enum { ALLOC_SIZE1 = 4, ALLOC_SIZE2 = 8 };

        char *buffer = bufferStorage.buffer();

        if (verbose) cout << "\nTesting constructor." << endl;
        Obj pool(buffer, BUFFER_SIZE, &objectAllocator);

        if (verbose) cout << "\nTesting allocate from buffer." << endl;
        void *addr1 = pool.allocate(ALLOC_SIZE1);

        // Allocation starts at the beginning of the aligned buffer.
        ASSERT(&buffer[0] == addr1);

        // Allocation comes from within the buffer.
        LOOP2_ASSERT((void *)&buffer[BUFFER_SIZE - 1],
                     addr1,
                     &buffer[BUFFER_SIZE - 1] >= addr1);

        void *addr2 = pool.allocate(ALLOC_SIZE2);

        // Allocation comes from within the buffer.
        ASSERT(&buffer[0]               <  addr2);
        ASSERT(&buffer[BUFFER_SIZE - 1] >= addr2);

        // Allocation respects alignment strategy.
        LOOP2_ASSERT((void *)&buffer[8],
                     addr2,
                     &buffer[8] == addr2);

        // Make sure no memory comes from the object, default and global
        // allocators.
        ASSERT(0 == objectAllocator.numBlocksTotal());
        ASSERT(0 == defaultAllocator.numBlocksTotal());
        ASSERT(0 == globalAllocator.numBlocksTotal());

        if (verbose) cout << "\nTesting allocate when buffer runs out."
                          << endl;
        {
            Obj pool2(buffer, BUFFER_SIZE, &objectAllocator);
            addr1 = pool2.allocate(BUFFER_SIZE + 1);

            // Allocation request is satisfied even when larger than the
            // supplied buffer.
            LOOP_ASSERT(addr1, 0 != addr1);

            // Allocation comes from the objectAllocator.
            ASSERT(0 != objectAllocator.numBlocksInUse());
            ASSERT(0 == defaultAllocator.numBlocksTotal());
            ASSERT(0 == globalAllocator.numBlocksTotal());
        }
        // All dynamically allocated memory are released after the pool's
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



// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
