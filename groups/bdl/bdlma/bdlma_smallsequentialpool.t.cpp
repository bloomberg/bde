// bdlma_smallsequentialpool.t.cpp                                    -*-C++-*-
#include <bdlma_smallsequentialpool.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>

#include <bsls_alignedbuffer.h>
#include <bsls_alignment.h>
#include <bsls_alignmentutil.h>
#include <bsls_asserttest.h>
#include <bsls_blockgrowth.h>
#include <bsls_keyword.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>   // 'bsl::memcpy'
#include <bsl_iostream.h>
#include <bsl_sstream.h>   // 'bsl::ostringstream'
#include <bsl_streambuf.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                             Overview
//                             --------
// A 'bdlma::SmallSequentialPool' is a mechanism (i.e., having state but no
// value) that is used as a memory manager to manage dynamically allocated
// memory.  The primary concern is that the small sequential pool's block list
// grows using the specified growth strategy, has the specified initial size,
// and is constrained by the specified maximum buffer size.  These options
// together create 13 variations of the constructor, and they must all be
// thoroughly tested.
//
// Because 'bdlma::SmallSequentialPool' does not have any accessors, this test
// driver verifies the correctness of the pool's allocations *indirectly*
// through the use of two consecutive allocations -- where the first allocation
// tests for correctness of 'allocate', and the second verifies the size of the
// first allocation and its memory alignment.
//
// We make heavy use of the 'bslma::TestAllocator' to ensure that:
//
//: 1 The growth rate of the block list matches the specified growth strategy
//:   and is constrained by the specified maximum buffer size.
//:
//: 2 When 'release' is invoked, all memory managed by the pool is deallocated.
//:
//: 3 When the pool is destroyed, all managed memory is deallocated.
//
// Finally, the destructor of 'bdlma::SmallSequentialPool' is tested throughout
// the test driver.  At destruction, the pool should reclaim all outstanding
// allocated memory.  By setting the global allocator, default allocator, and
// object allocator to different test allocators, we can determine whether all
// memory had been released by the destructor of the pool.
// ----------------------------------------------------------------------------
// CREATORS
// [ 3] SmallSequentialPool(*a = 0);
// [ 3] SmallSequentialPool(GS g, *a = 0);
// [ 3] SmallSequentialPool(AS a, *a = 0);
// [ 3] SmallSequentialPool(GS g, AS a, *a = 0);
//
// [ 3] SmallSequentialPool(int  i);
// [ 3] SmallSequentialPool(size i, *a = 0);
// [ 3] SmallSequentialPool(size i, GS g, *a = 0);
// [ 3] SmallSequentialPool(size i, AS a, *a = 0);
// [ 3] SmallSequentialPool(size i, GS g, AS a, *a = 0);
//
// [ 3] SmallSequentialPool(size i, int m, *a = 0);
// [ 3] SmallSequentialPool(size i, int m, GS g, *a = 0);
// [ 3] SmallSequentialPool(size i, int m, AS a, *a = 0);
// [ 3] SmallSequentialPool(size i, int m, GS g, AS a, *a = 0);
//
// [ 3] SmallSequentialPool(size i, int m, GS g, AS a, bool aIB, *a = 0);
//
// [ 3] ~SmallSequentialPool();
//
// MANIPULATORS
// [ 4] void *allocate(size_type size);
// [ 7] void *allocateAndExpand(size_type *size);
// [ 6] void deleteObjectRaw(const TYPE *object);
// [ 6] void deleteObject(const TYPE *object);
// [ 5] void release();
// [ 9] void reserveCapacity(int numBytes);
// [ 5] void rewind();
// [ 8] int truncate(void *address, int originalSize, int newSize);
//
// FREE OPERATORS
// [10] operator delete(void *, bdlma::SmallSequentialPool&);'
// [10] operator new(size_t, bdlma::SmallSequentialPool&);'
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] HELPER FUNCTION: 'int blockSize(numBytes)'
// [11] CONCERN: Large allocated blocks are released by 'rewind'
// [12] CONCERN: Growth-Strategy Transitions
// [13] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

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

// ============================================================================
//                  GLOBAL VARIABLES / TYPEDEF FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlma::SmallSequentialPool Obj;

typedef bsls::Alignment::Strategy  Strat;

enum { k_MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT };

enum { k_DEFAULT_SIZE = 256 };  // initial size of the buffer if none specified
                                // (implementation detail)

// ============================================================================
//                          HELPER CLASS FOR TESTING
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------

struct Block {
    // This type was copied from 'bdlma_infrequentdeleteblocklist.h' for
    // testing purposes.

    Block                               *d_next_p;
    bsls::AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
};

// ============================================================================
//                        STATIC FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------

static int calculateNextSize(int currentSize, int size)
    // Return the next buffer size (in bytes) that is sufficiently large to
    // satisfy a memory allocation request of the specified 'size' (in bytes).
    // The next buffer size is calculated by assuming that the current buffer
    // is the specified 'currentSize' (in bytes) iteratively doubling until
    // 'size' is exceeded.
{
    ASSERT(0 < currentSize);
    ASSERT(0 < size);

    if (0 == currentSize) {
        return 0;                                                     // RETURN
    }

    do {
        currentSize *= 2;
    } while (currentSize < size);

    return currentSize;
}

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

// BDE_VERIFY pragma: -FABC01 // Functions are not in alphanumeric order.

namespace Usage {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using 'bdlma::SmallSequentialPool' for Efficient Allocations
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Some applications require the allocation of many small blocks of memory to
// hold small amounts of data, pointers, etc.  Allocating small blocks from a
// global allocator can cause problems.  For example, a thread lock may be
// required before access to the global allocator.  Also, many small
// allocations may lead to fragmentation of the global allocator.  A memory
// pool can be used to replace many small allocations (from the global
// allocator) to a smaller number of larger allocations.
//
// Suppose we define a container class, 'my_IntDoubleArray', that holds both
// 'int' and 'double' values.  The class can be implemented using two parallel
// arrays: one storing the type information, and the other storing pointers to
// the 'int' and 'double' values.  For efficient memory allocation, we can use
// a 'bdlma::SmallSequentialPool' for memory allocation:
//
// First, we define 'my_IntDoubleArray' class:
//..
    // my_intdoublearray.h

    class my_IntDoubleArray {
        // This class implements an efficient container for an array that
        // stores both 'int' and 'double' values.

        // PRIVATE TYPES
        enum  Type { k_MY_INT, k_MY_DOUBLE };

        union Data {
                     int    d_intValue;
                     double d_doubleValue;
                   };

        // DATA
        char  *d_typeArray_p;   // array indicating the type of corresponding
                                // values stored in 'd_dataArray_p'

        Data **d_dataArray_p;   // array of pointers to the values stored

        int    d_length;        // number of values stored

        bdlma::SmallSequentialPool
               d_pool;          // sequential memory pool used to supply memory

      private:
        // PRIVATE MANIPULATORS
        void incrementCapacity();
            // Increase the capacity of the internal arrays used to store
            // elements by one element.

        // NOT IMPLEMENTED
        my_IntDoubleArray(const my_IntDoubleArray&) BSLS_KEYWORD_DELETED;

      public:

        // CREATORS
        explicit my_IntDoubleArray(bslma::Allocator *basicAllocator = 0);
            // Create an 'int'-'double' array.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        // ...

        ~my_IntDoubleArray();
            // Destroy this array and all elements held by it.

        // MANIPULATORS
        void appendDouble(double value);
            // Append the specified 'double' 'value' to this array.

        void appendInt(int value);
            // Append the specified 'int' 'value' to this array.

        void removeAll();
            // Remove all elements from this array.

        // ...

        // ACCESSORS
        int numElements() const;
            // Return the number of elements in this array.

        // ...
    };
//..
// Then, we define the (straightforward) inline methods of the class.
//..
    // CREATORS
    inline
    my_IntDoubleArray::~my_IntDoubleArray()
    {
        ASSERT(0 <= d_length);
    }

    // MANIPULATORS
    inline
    void my_IntDoubleArray::removeAll()
    {
        d_pool.rewind();
        d_length = 0;
    }

    // ...

    // ACCESSORS
    inline
    int my_IntDoubleArray::numElements() const
    {
        return d_length;
    }

    // ...
//..
// Next, we define the non-inlined methods of the class.  Note that our
// implementation is *not* sparing the number of requests for memory from the
// pool.  It makes many requests for small-ish blocks of memory.  Notably:
//: o A new "type" and "data" array is allocated for each appended value, and
//: o each data item is stored in a separately allocated node.
//..
    // my_intdoublearray.cpp

    // PRIVATE MANIPULATORS
    void my_IntDoubleArray::incrementCapacity()
    {
        int    newLength    = d_length + 1;
        char  *tmpTypeArray = static_cast<char *>(
                         d_pool.allocate(newLength * sizeof *d_typeArray_p));
        Data **tmpDataArray  = static_cast<Data **>(
                         d_pool.allocate(newLength * sizeof *d_dataArray_p));

        bsl::memcpy(tmpTypeArray,
                    d_typeArray_p,
                    d_length * sizeof *d_typeArray_p);
        bsl::memcpy(tmpDataArray,
                    d_dataArray_p,
                    d_length * sizeof *d_dataArray_p);

        d_typeArray_p = tmpTypeArray;
        d_dataArray_p = tmpDataArray;

        // Note that 'd_length' is updates in the calling functions.
    }
//..
// Notice that we do nothing above clean up the old arrays because:
//: 1 The array types have trivial destructors (that need not be called).
//: 2 The memory will be recovered when the pool is destroyed by the destructor
//:   of 'my_IntDoubleArray'.  Note that the pool does *not* even provide a
//:   'deallocate' method.
//..
    // CREATORS
    my_IntDoubleArray::my_IntDoubleArray(bslma::Allocator *basicAllocator)
    : d_length(0)
    , d_pool(basicAllocator)
    {
    }

    // MANIPULATORS
    void my_IntDoubleArray::appendDouble(double value)
    {
        incrementCapacity();

        Data *item = static_cast<Data *>(d_pool.allocate(sizeof *item));

        item->d_doubleValue = value;

        d_typeArray_p[d_length] = static_cast<char>(k_MY_DOUBLE);
        d_dataArray_p[d_length] = item;

        ++d_length;
    }

    void my_IntDoubleArray::appendInt(int value)
    {
        incrementCapacity();

        Data *item = static_cast<Data *>(d_pool.allocate(sizeof *item));

        item->d_intValue = value;

        d_typeArray_p[d_length] = static_cast<char>(k_MY_INT);
        d_dataArray_p[d_length] = item;

        ++d_length;
    }
//..
// Now, we can use the 'my_IntDoubleArray' class.  For purposes of
// illustration, we use the class in concert with a 'bslma::TestAllocator' so
// we can observe the interactions between our object and the global allocator.
//..
    void useMyIntDoubleArray()
        // Demonstrate some of the characteristic behaviors of the
        // 'my_IntDoubleArray' class.
    {
        bslma::TestAllocator sa("supplied");

        {
            my_IntDoubleArray obj(&sa);
            ASSERT(0 == obj.numElements());
            ASSERT(0 == sa.numBlocksInUse());

            obj.appendInt(42);
            ASSERT(1 == obj.numElements());
            ASSERT(1 == sa.numBlocksInUse());
//..
// Then, we observe that appending a value triggers the allocation of a block
// from the allocator.
//..
            obj.appendDouble(355.0/113.0);
            ASSERT(2 == obj.numElements());
            ASSERT(1 == sa.numBlocksInUse());

            obj.appendDouble(666);
            ASSERT(3 == obj.numElements());
            ASSERT(1 == sa.numBlocksInUse());
//..
// Next, we observe that appending additional elements does not require the
// allocation of additional blocks from the allocator.  The pool services the
// many subsequent data needs (new internal arrays and data nodes) out of the
// first block from the allocator.
//
// Then, we remove all of the elements from the array and observe that the pool
// retained that block for future use.  Note that our implementation of
// 'removeAll' invokes the 'rewind' (not the 'release') method of the pool.
//..
            obj.removeAll();
            ASSERT(0 == obj.numElements());
            ASSERT(1 == sa.numBlocksInUse());
//..
// Next, we observe that appending additional several elements does not require
// calls to the allocator.  Again, the pool handles these memory needs using
// the original block.
//..
            obj.appendInt   (1  );
            obj.appendDouble(2.0);
            obj.appendInt   (3  );
            obj.appendDouble(4.0);

            ASSERT(4 == obj.numElements());
            ASSERT(1 == sa.numBlocksInUse());
        }

        ASSERT(0 == sa.numBlocksInUse());
    }
//..
// Finally, we observe that destruction of the pool, a step in destroying the
// array object, deallocates the block that was allocated in the first append
// operation.
//
///Example 2: Implementing an Allocator Using 'bdlma::SmallSequentialPool'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Although 'bslma::SmallSequentialPool' provides an 'allocate' method, it
// cannot be used as an allocator (mechanism) unless it is wrapped by a class
// that provides the full 'bslma::Allocator' protocol.  The class below,
// 'my_SmallSequentialAllocator' shows how that its done.  Note that the class
// below is a simplified version of that defined in
// {'bslma_smallsequentialallocator'}.
//
// First, we define the 'my_SmallSequentialAllocator' class, taking care to
// inherit from the protocol class:
//..
    class my_SmallSequentialAllocator : public bslma::Allocator {
        // This class implements the 'bslma::Allocator' protocol to provide a
        // fast allocator of heterogeneous blocks of memory (of varying,
        // user-specified sizes) from dynamically-allocated buffers.

        // DATA
        bdlma::SmallSequentialPool d_pool;  // manager for allocated memory
                                            // blocks

      public:
        // CREATORS
        explicit my_SmallSequentialAllocator(
                                         bslma::Allocator *basicAllocator = 0);
            // Create an allocator for allocating memory blocks from
            // dynamically-allocated buffers.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        ~my_SmallSequentialAllocator();
            // Destroy this allocator.  All memory allocated from this
            // allocator is released.

        // MANIPULATORS
        void *allocate(size_type size) BSLS_KEYWORD_OVERRIDE;
            // Return the address of a contiguous block of memory of the
            // specified 'size' (in bytes).

        void deallocate(void *address) BSLS_KEYWORD_OVERRIDE;
            // This method has no effect on the memory block at the specified
            // 'address' as all memory allocated by this allocator is managed.
            // The behavior is undefined unless 'address' was allocated by this
            // allocator, and has not already been deallocated.
    };
//..
// Then, we define the methods of the class.  All are small 'inline' wrappers
// that forward to the pool data member.
//..
    // CREATORS
    inline
    my_SmallSequentialAllocator::my_SmallSequentialAllocator(
                                              bslma::Allocator *basicAllocator)
    : d_pool(basicAllocator)
    {
    }

    inline
    my_SmallSequentialAllocator::~my_SmallSequentialAllocator()
    {
        // Memory is released by the pool's destructor.
    }

    // MANIPULATORS
    inline
    void *my_SmallSequentialAllocator::allocate(size_type size)
    {
        return d_pool.allocate(size);
    }

    inline
    void my_SmallSequentialAllocator::deallocate(void *)
    {
        // A no-op: The pool does not provide a 'deallocate' method.
    }
//..
// Now, we can use our allocator class and confirm that its behavior is
// consistent with that of 'bslma::SmallSequentialPool'.  As in {Example 1}, we
// use a 'bslma::TestAllocator' to allow us to observe the interactions with
// the global allocator.
//..
    void useMySmallSequentialAllocator()
        // Demonstrate some of the characteristic behaviors of the
        // 'my_SmallSequentialAllocator' class.
    {
        bslma::TestAllocator        ta("backup");

        {
            my_SmallSequentialAllocator ssa(&ta);

            {
                ASSERT(0 == ta.numBlocksInUse());

//..
// Next, we observe that created a 'bsl::string' object that uses a
// 'my_SmallSequentialAllocator' object for memory and of length that exceeds
// the small string optimization (an implementation detail of 'bsl::string)
// triggers the allocation of a single member block from the "backup"
// allocator.
//..
                bsl::string greeting("Hello word!  How are you today?",
                                     &ssa);
                ASSERT(bsl::string().capacity() < greeting.size());
                ASSERT(1 == ta.numBlocksInUse());
//..
// Then, we observe that creating a second such string (using the same
// allocator object) does not require an additional allocation from the backup
// allocator.  The pool in our allocator object is able to satisfy this request
// from its original allocation.
//..
                bsl::string response("I am fine.  Thank you for asking.",
                                     &ssa);
                ASSERT(bsl::string().capacity() < response.size());
                ASSERT(1 == ta.numBlocksInUse());
            }
//..
// Next, we observe that destroying these strings does return the allocated
// memory to the backup allocator.  Recall that the 'deallocate' method of our
// allocator object is a no-op.
//..
            ASSERT(1 == ta.numBlocksInUse());
        }
//..
// Finally, we observe the memory being returned when our allocator object (and
// the pool it contains) is destroyed.
//..
        ASSERT(0 == ta.numBlocksInUse());
    }
//..
//
///Example 3: Iterative Pool Reuse
///- - - - - - - - - - - - - - - -
// Using a pool is more efficient for allocations a large number of small
// memory allocations than allocating directly from the global allocator.  See
// {Example 1}.  In this example, we illustrate a scenario where the 'rewind'
// method is used to make such allocations even *more* efficient.  Suppose one
// has an application that repeatedly makes a large number of small
// allocations, deallocations, and reallocations (e.g., a service responding to
// a client query).
//
// First, we define 'poolAllocationScenario', a function that represents to
// usage scenario.  For simplicity of exposition, this function is
// preternaturally regular: the same number of allocations of a uniform size
// are made in each invocation:
//..
    void poolAllocationScenario(bdlma::SmallSequentialPool *pool,
                                bsl::size_t                 allocationSize)
        // Approximate a "typical" pool usage scenario consisting of many
        // small allocations of the specified 'allocationSize' from the
        // specified 'pool'.
    {
        BSLS_ASSERT(pool);

        // Consume the first three, geometrically-allocated buffers, assuming
        // 4-byte allocations.

        for (int i = 0; i < 64 + 128 + 256; ++i) {
            pool->allocate(allocationSize);
        }
    }
//..
// Then, we create a 'bdlma::SmallSequentialPool' object to be supplied to the
// usage scenario.  We install a 'bslma::TestAllocator' object as the pool's
// upstream allocator so that the memory usage of the pool can be observed:
//..
    void iterativePoolReuse()
        // Simulate reuse of a 'bslma::SmallSequentialPool' object by
        // iteratively subjecting it to the 'poolAllocationScenario' and
        // invoking its 'rewind' method after after each iteration.
    {
        bslma::TestAllocator       sa("supplied");
        bdlma::SmallSequentialPool ssp(&sa);
//..
// Next, we iteratively invoke the usage scenario, rewinding the pool after
// each use:
//..
        for (int i = 0; i < 5; ++i) {

            poolAllocationScenario(&ssp, 4);

            cout << i                   << ": "
                 << sa.numBlocksInUse() << " "
                 << sa.numBytesInUse()  << endl;

            ssp.rewind();
        }
//..
// Now, we examine the usage pattern:
//..
//  0: 3 1816
//  1: 2 3088
//  2: 1 2056
//  3: 1 2056
//  4: 1 2056
//..
// Recall that the 'bdlma::SmallSequentialPool' class retains the latest (for
// geometric growth, the largest non-"large") allocated block.  Notice that for
// this usage pattern the pool converges to a single (contiguous) block (no
// further upstream allocations) and that block size is less than the maximum
// allocation.  Also notice that this nice behavior was achieved with *no*
// foreknowledge of the usage pattern of the repeated scenario.  (The
// implementation-specific default value was used for the initial block
// allocation.)
//
// Finally, we consider what happens if the usage scenario should change over
// time by doubling the allocation size from 4 to 8:
//..
        for (int i = 5; i < 10; ++i) {

            poolAllocationScenario(&ssp, 8);  // Increased allocation size

            cout << i                   << ": "
                 << sa.numBlocksInUse() << " "
                 << sa.numBytesInUse()  << endl;

            ssp.rewind();
        }
    }
//..
// The output shows that the pool rapidly adapted to the increased larger
// allocations:
//..
//  5: 2 6160
//  6: 1 4104
//  7: 1 4104
//  8: 1 4104
//  9: 1 4104
//..

}  // close namespace Usage

// BDE_VERIFY pragma: +FABC01 // Functions are not in alphanumeric order.

// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                test = argc > 1 ? atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // As part of our overall allocator testing strategy, we will create three
    // test allocators.

    // Object Test Allocator.
    bslma::TestAllocator objectAllocator("Object Allocator",
                                         veryVeryVeryVerbose);

    // Default Test Allocator.
    bslma::TestAllocator         defaultAllocator("Default Allocator",
                                                   veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard guard(&defaultAllocator);

    // Global Test Allocator.
    bslma::TestAllocator globalAllocator("Global Allocator",
                                         veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 13: {
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

        if (veryVerbose) cout << "Example 1" << endl;
        Usage::useMyIntDoubleArray();

        if (veryVerbose) cout << "Example 2" << endl;
        Usage::useMySmallSequentialAllocator();

        if (veryVerbose) cout << "Example 3" << endl;

        bsl::streambuf     *coutStreambuf = bsl::cout.rdbuf();
        bsl::ostringstream  ss;
        bsl::cout.rdbuf(ss.rdbuf());    // Redirect 'cout' to the 'streambuf'.

        Usage::iterativePoolReuse();    // ACTION

        bsl::cout.rdbuf(coutStreambuf); // Undo the output redirection.

        if (veryVeryVerbose) {
            cout << ss.str() << endl;
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TEST GROWTH-STRATEGY TRANSITIONS
        //   The pool constant-growth strategy lapses to geometric growth if a
        //   request exceeds the constant-growth size but is within the maximum
        //   buffer size.  Requests larger than the maximum buffer size are
        //   handled as "large" blocks.  Each policy is used only when needed.
        //
        // Concerns:
        //: 1 The pool growth strategy switches from constant-growth to
        //:   geometric growth when an allocation request exceeds the constant
        //:   growth size.
        //:
        //: 2 The initial growth strategy used can be either constant or
        //:   geometric.
        //:
        //: 3 The geometric growth factor is retained across episodes of
        //:   constant growth.
        //:
        //: 4 A maximum buffer size limits geometric growth initiated from
        //:   constant growth.
        //:
        //: 5 Blocks acquired by constant-growth and (forced) geometric-growth
        //:   strategies are retained across 'rewind' whereas "large" blocks
        //:   are not.
        //:
        //: 6 The geometric growth factor is retained across calls to 'rewind'.
        //:
        //: 7 The 'release' method resets the geometric growth factor.
        //
        // Plan:
        //: 1 The different transitions will be explored by an ad-hoc series of
        //:   tests.  (C-1..7)
        //
        // Testing:
        //   CONCERN: Growth-Strategy Transitions
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST GROWTH-STRATEGY TRANSITIONS" << endl
                          << "================================" << endl;

        const bsls::BlockGrowth::Strategy CS =
                                             bsls::BlockGrowth::BSLS_CONSTANT;

        const int ISZ = 1024;     // initial buffer size
        const int MBS = ISZ * 8;  // maximum buffer size

        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

        bsls::Types::Int64 priorBlocksInUse = sa.numBlocksInUse();
        bsls::Types::Int64 priorBytesInUse  = sa. numBytesInUse();

        ASSERT(0 == priorBlocksInUse);
        ASSERT(0 == priorBytesInUse);

        if (veryVerbose) { Q(scenario: constant-growth first) }

        Obj mX(ISZ, MBS, CS, &sa);                                    // ACTION

        ASSERT(1              == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(ISZ) == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Consume block allocated on construction.
        mX.allocate(ISZ);                                             // ACTION

        ASSERT(0              == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(0              == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Add a block to the pool under the constant growth strategy
        mX.allocate(ISZ);                                             // ACTION

        ASSERT(1              == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(ISZ) == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Force geometric allocation.
        mX.allocate(ISZ + 1);                                         // ACTION

        ASSERT(1                  == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(2 * ISZ) == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Service a request via constant-growth strategy (second time).
        mX.allocate(ISZ);                                             // ACTION

        ASSERT(1              == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(ISZ) == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Force geometric allocation again.
        mX.allocate(ISZ + 1);                                         // ACTION

        ASSERT(1                  == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(4 * ISZ) == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Force "large" block allocation
        mX.allocate(MBS + 1);                                         // ACTION

        ASSERT(1                  == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(MBS + 1) == sa.numBytesInUse()  - priorBytesInUse);

        mX.rewind();                                                  // ACTION

        ASSERT(1                  == sa.numBlocksInUse());
        ASSERT(blockSize(4 * ISZ) == sa.numBytesInUse());  // last non-large
                                                           // allocation
                                                           // (geometric)

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Consume block allocated retained across 'rewind'.
        mX.allocate(4 * ISZ);                                         // ACTION

        ASSERT(0              == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(0              == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Confirm that geometric growth is based retained block.
        mX.allocate(ISZ + 1);                                         // ACTION

        ASSERT(1                  == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(8 * ISZ) == sa.numBytesInUse()  - priorBytesInUse);

        if (veryVerbose) { Q(release) }

        mX.release();                                                 // ACTION

        ASSERT(0                  == sa.numBlocksInUse());
        ASSERT(0                  == sa.numBytesInUse());

        if (veryVerbose) { Q(scenario: geometric-growth first) }

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Force geometric allocation.
        mX.allocate(ISZ + 1);                                         // ACTION

        ASSERT(1                  == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(2 * ISZ) == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Add a block to the pool under the constant growth strategy
        mX.allocate(ISZ);                                             // ACTION

        ASSERT(1              == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(ISZ) == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Force geometric allocation again.
        mX.allocate(ISZ + 1);                                         // ACTION

        ASSERT(1                  == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(4 * ISZ) == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Consume remaining space of last allocation
        mX.allocate(4 * ISZ - (ISZ + 1));                             // ACTION

        ASSERT(0                  == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(0                  == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Service a request via constant-growth strategy (second time).
        mX.allocate(ISZ);                                             // ACTION

        ASSERT(1              == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(ISZ) == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Force "large" block allocation
        mX.allocate(MBS + 1);                                         // ACTION

        ASSERT(1                  == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(MBS + 1) == sa.numBytesInUse()  - priorBytesInUse);

        mX.rewind();                                                  // ACTION

        ASSERT(1              == sa.numBlocksInUse());
        ASSERT(blockSize(ISZ) == sa.numBytesInUse());  // last non-large
                                                       // allocation (constant)

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Consume block allocated retained across 'rewind'.
        mX.allocate(ISZ);                                             // ACTION

        ASSERT(0              == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(0              == sa.numBytesInUse()  - priorBytesInUse);

        priorBlocksInUse = sa.numBlocksInUse();
        priorBytesInUse  = sa.numBytesInUse();

        // Confirm that geometric growth is based retained block.
        mX.allocate(ISZ + 1);                                         // ACTION

        ASSERT(1                  == sa.numBlocksInUse() - priorBlocksInUse);
        ASSERT(blockSize(2 * ISZ) == sa.numBytesInUse()  - priorBytesInUse);

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TEST LARGE BLOCK MANAGEMENT
        //
        // Concerns:
        //: 1 "Large" blocks are not retained by 'rewind' operations.
        //:
        //: 2 Constructors:
        //:
        //:   1 Constructors that define a "maximum" buffer size and a
        //:     geometric block growth strategy create objects that manage
        //:     allocations that exceed that maximum as "large" blocks.  Note
        //:     that these constructors always specify an initial size.
        //:
        //:   2 Constructors that specify a constant growth strategy create
        //:     objects that manage allocations that exceed the initial size as
        //:     "large" blocks.
        //:
        //:   3 Other constructors create objects that do not distinguish
        //:     allocations as "large" blocks.
        //:
        //: 3 "Large" blocks are allocated from the object allocator.
        //:
        //: 4 The 'rewind' method is idempotent.
        //:
        //: 5 Each request requiring a "large" block triggers one allocation of
        //:   the needed size.
        //:
        //: 6 When the constant growth strategy is used and there is no initial
        //:   allocation, allocations in excess of 'k_DEFAULT_SIZE' are "large"
        //:   blocks.  Note that 'k_DEFAULT_SIZE' is 256 and that is an
        //:   implementation detail.
        //
        // Plan:
        //: 1 As the class under tests provides no accessors to object state,
        //:   the behavior or each object must be inferred from the changing
        //:   state of its supplied allocator, an instance of
        //:   'bslma::TestAllocator'.
        //
        //: 2 Use the "footprint" idiom to exercise each of the constructors to
        //:   create a series of test objects.  Constructors that allow
        //:   specification of growth strategy are invoked twice so that we
        //:   test the object behavior under both geometric and constant growth
        //:   strategies.  (C-2)
        //:
        //: 2 Categorize each created object based on the constructor and (in
        //:   some cases) the constructor arguments.  Object categories
        //:   determine the expected behavior and is stored in boolean flags
        //:   (e.g., 'canHaveLargeBlocks').
        //
        //: 3 Confirm and save allocator state after construction.  Note that
        //:   constructor preconditions assure us that any memory allocated as
        //:   part of construction is not classified as a "large" block.
        //:
        //: 4 Explicitly call 'allocate' using a size value that results in a
        //:   "large" block if the object distinguishes large blocks.
        //:
        //: 5 Confirm and save allocator state after the allocation.  (C-3)
        //:
        //: 6 Invoke the 'rewind' method.
        //:
        //: 7 Confirm that the allocator state matches the expected values for
        //:   category of the test object.  (C-1)
        //:
        //: 8 Use a 'bslma::TestAllocatorMonitor' object to confirm no change
        //:   of allocator state after a second invocation of 'rewind' (to
        //:   confirm that the method is idempotent).  (C-4)
        //:
        //: 9 In a separate test (from the "footprint" idiom above):
        //:
        //:   1 Create a pool using the constant growth strategy and having no
        //:     initial allocation.
        //:
        //:   2 Confirm that that each request in excess of 'k_DEFAULT_SIZE'
        //:     triggers an allocation of the required size (and some fixed
        //:     overhead).  (C-5)
        //:
        //:   3 Confirm such requests are "large" blocks (i.e., not retained
        //:     past rewind).  (C-6)
        //
        // Testing:
        //   CONCERN: Large allocated blocks are released by 'rewind'
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TEST LARGE BLOCK MANAGEMENT" << endl
                          << "===========================" << endl;

        const bsls::BlockGrowth::Strategy GS =
                                             bsls::BlockGrowth::BSLS_GEOMETRIC;
        const bsls::BlockGrowth::Strategy CS =
                                             bsls::BlockGrowth::BSLS_CONSTANT;
        const bsls::Alignment::Strategy   AS =
                                             bsls::Alignment::BSLS_NATURAL;

        if (verbose) {
            cout << endl << "Test each constructor" << endl;
        }

        int   hasInitialAllocationAvecLargeBlocks = 0;
        int   hasInitialAllocationSansLargeBlocks = 0;
        int hasNoInitialAllocationAvecLargeBlocks = 0;
        int hasNoInitialAllocationSansLargeBlocks = 0;

        const int ISZ = 1024;          // initial size
        const int MBS = ISZ;           // maximum size

        ASSERT(k_DEFAULT_SIZE < ISZ);  // Exceeds default size of constant
                                       // growth strategy.

        for (char cfg = 'a'; cfg <= 'v'; ++cfg) {
            const char CONFIG = cfg;

            if (veryVerbose) {
                cout << endl;
                T_ P(CONFIG)
            }

            bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            Obj *objPtr = // ACTION
                   // Geometric Growth Strategy, 'GS'
                     'a' == CONFIG ? new (fa) Obj(                         &sa)
                   : 'b' == CONFIG ? new (fa) Obj(          GS,            &sa)
                   : 'c' == CONFIG ? new (fa) Obj(              AS,        &sa)
                   : 'd' == CONFIG ? new (fa) Obj(          GS, AS,        &sa)

                   : 'e' == CONFIG ? new (fa) Obj(ISZ,                     &sa)
                   : 'f' == CONFIG ? new (fa) Obj(ISZ,      GS,            &sa)
                   : 'g' == CONFIG ? new (fa) Obj(ISZ,          AS,        &sa)
                   : 'h' == CONFIG ? new (fa) Obj(ISZ,      GS, AS,        &sa)

                   : 'i' == CONFIG ? new (fa) Obj(ISZ, MBS,                &sa)
                   : 'j' == CONFIG ? new (fa) Obj(ISZ, MBS, GS,            &sa)
                   : 'k' == CONFIG ? new (fa) Obj(ISZ, MBS,     AS,        &sa)
                   : 'l' == CONFIG ? new (fa) Obj(ISZ, MBS, GS, AS,        &sa)

                   // Constant Growth Strategy, 'CS'
                   : 'm' == CONFIG ? new (fa) Obj(          CS,            &sa)
                   : 'n' == CONFIG ? new (fa) Obj(          CS, AS,        &sa)

                   : 'o' == CONFIG ? new (fa) Obj(ISZ,      CS,            &sa)
                   : 'p' == CONFIG ? new (fa) Obj(ISZ,      CS, AS,        &sa)

                   : 'q' == CONFIG ? new (fa) Obj(ISZ, MBS, CS,            &sa)
                   : 'r' == CONFIG ? new (fa) Obj(ISZ, MBS, CS, AS,        &sa)

                   // Conditional
                   : 's' == CONFIG ? new (fa) Obj(ISZ, MBS, GS, AS, true,  &sa)
                   : 't' == CONFIG ? new (fa) Obj(ISZ, MBS, GS, AS, false, &sa)
                   : 'u' == CONFIG ? new (fa) Obj(ISZ, MBS, CS, AS, true,  &sa)
                   : 'v' == CONFIG ? new (fa) Obj(ISZ, MBS, CS, AS, false, &sa)

                   : /* error */     0;
            ASSERTV(CONFIG, objPtr);

            Obj& mX = *objPtr;

            const bool hasInitialAllocation = ('e' <= CONFIG && CONFIG <= 'l')
                                           || ('o' <= CONFIG && CONFIG <= 'r')
                                           ||  's' == CONFIG
                                           ||  'u' == CONFIG;

            const bool hasMaximumBlockSize  = ('i' <= CONFIG && CONFIG <= 'l')
                                           || ('q' <= CONFIG && CONFIG <= 'v');

            const bool usesConstantGrowthStrategy =
                                              ('m' <= CONFIG && CONFIG <= 'r')
                                           ||  'u' == CONFIG
                                           ||  'v' == CONFIG;

            const bool canHaveLargeBlocks   = hasMaximumBlockSize;

            if (veryVerbose) {
                cout << "Classification" << ": ";
                P(hasInitialAllocation)
                P(usesConstantGrowthStrategy)
                P(hasMaximumBlockSize)
                P(canHaveLargeBlocks)
            }

            const bsls::Types::Int64 blocksInitialAlloc = sa.numBlocksInUse();
            const bsls::Types::Int64  bytesInitialAlloc = sa. numBytesInUse();

            if (veryVerbose) {
                cout << "Post CTOR" << ": ";
                P_(blocksInitialAlloc) P(bytesInitialAlloc)
            }

            if (hasInitialAllocation) {
                ASSERTV(CONFIG, 1              == blocksInitialAlloc);
                ASSERTV(CONFIG, blockSize(ISZ) ==  bytesInitialAlloc);
            } else {
                ASSERTV(CONFIG, 0              == blocksInitialAlloc);
                ASSERTV(CONFIG, 0              ==  bytesInitialAlloc);
            }

            bsl::size_t explicitAllocationSize = ISZ + 1;

            if (veryVerbose) {
                P(explicitAllocationSize)
            }

            ASSERTV(CONFIG, mX.allocate(explicitAllocationSize));  // ACTION

            const bsls::Types::Int64 blocksExplicitAlloc = sa.numBlocksInUse()
                                                         - blocksInitialAlloc;

            const bsls::Types::Int64  bytesExplicitAlloc = sa. numBytesInUse()
                                                         - bytesInitialAlloc;

            if (veryVerbose) {
                cout << "Post 'allocate'" << ": ";
                P_(blocksExplicitAlloc) P(bytesExplicitAlloc)
            }

            // Note: True irrespective of 'hasInitialAllocation'.

            ASSERTV(CONFIG, 1                  == blocksExplicitAlloc);
            ASSERTV(CONFIG, bytesExplicitAlloc == (hasMaximumBlockSize
                                                   ? blockSize(ISZ + 1)
                                                   : blockSize(ISZ * 2)));

            mX.rewind();  // ACTION

            const bsls::Types::Int64 blocksPostRewind = sa.numBlocksInUse();
            const bsls::Types::Int64  bytesPostRewind = sa. numBytesInUse();

            if (veryVerbose) {
                cout << "Post 'rewind'" << ": ";
                P_(blocksPostRewind) P(bytesPostRewind)
            }

            if (hasInitialAllocation) {
                ASSERTV(CONFIG, 1                 == sa.numBlocksInUse());
                ASSERTV(CONFIG, bytesInitialAlloc <  bytesExplicitAlloc);

                if (canHaveLargeBlocks) {  // Fewer bytes retained.
                    ++hasInitialAllocationAvecLargeBlocks;

                    ASSERTV(CONFIG, bytesInitialAlloc  == bytesPostRewind);
                } else {
                    ++hasInitialAllocationSansLargeBlocks;

                    ASSERTV(CONFIG, bytesExplicitAlloc == bytesPostRewind);
                }
            } else {
                if (canHaveLargeBlocks) {
                    ++hasNoInitialAllocationAvecLargeBlocks;

                    ASSERTV(CONFIG, 0                  == sa.numBlocksInUse());
                    ASSERTV(CONFIG, 0                  == bytesPostRewind);
                } else {
                    ++hasNoInitialAllocationSansLargeBlocks;

                    ASSERTV(CONFIG, 1                  == sa.numBlocksInUse());
                    ASSERTV(CONFIG, bytesExplicitAlloc == bytesPostRewind);
                }
            }

            // Confirm that 'rewind' is idempotent.

            bslma::TestAllocatorMonitor sam(&sa);

            mX.rewind();  // ACTION

            ASSERTV(CONFIG, sam.isInUseSame());

            // Clean up

            fa.deleteObject(objPtr);
            ASSERTV(CONFIG, 0 == sa.numBlocksInUse());
        }

        if (veryVerbose) {
            cout << endl;
            P(  hasInitialAllocationAvecLargeBlocks);
            P(  hasInitialAllocationSansLargeBlocks);
            P(hasNoInitialAllocationAvecLargeBlocks);
            P(hasNoInitialAllocationSansLargeBlocks);
        }

        ASSERT(0 <   hasInitialAllocationAvecLargeBlocks);
        ASSERT(0 <   hasInitialAllocationSansLargeBlocks);
        ASSERT(0 < hasNoInitialAllocationAvecLargeBlocks);
        ASSERT(0 < hasNoInitialAllocationSansLargeBlocks);

        if (verbose) {
            cout << endl
                 << "Additional tests for constant growth strategy" << endl;
        }

        const int LIMIT = 5; // arbitrary small limit

        for (int i = 1; i <= LIMIT; ++i ) {

            if (veryVerbose) { T_ P(i) }

            bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

            // Create a pool object that uses the constant-growth strategy and
            // that has an initial buffer sized to be insufficient for the
            // allocation size.  Set the maximum buffer size to disallow the
            // use of geometric-growth as a fall-back.  Thus, each allocation
            // will be handled as a "large" block.

            Obj mX(k_DEFAULT_SIZE, k_DEFAULT_SIZE, CS, &sa);

            const bsls::Types::Int64 blocksInitialAlloc = sa.numBlocksInUse();
            const bsls::Types::Int64  bytesInitialAlloc = sa. numBytesInUse();

            ASSERT(1                         == blocksInitialAlloc);
            ASSERT(blockSize(k_DEFAULT_SIZE) ==  bytesInitialAlloc);

            for (int j = 1; j <= i; ++j) {

                if (veryVerbose) { T_ T_ P(j) }

                bslma::TestAllocatorMonitor sam(&sa);

                ASSERT(mX.allocate(k_DEFAULT_SIZE + 1));  // ACTION

                static const bsls::Types::Int64 bytesPerAllocation =
                                                            sa.numBytesInUse()
                                                          - bytesInitialAlloc;
                    // This 'static' value is fixed on the first pass.

                if (veryVerbose) {
                    T_ T_ P_(bytesPerAllocation)
                          P_(sa.numBlocksInUse())
                          P(sa.numBytesInUse())

                }

                ASSERTV(i, j, j                      == sa.numBlocksInUse()
                                                      - blocksInitialAlloc);
                ASSERTV(i, j, j * bytesPerAllocation == sa.numBytesInUse()
                                                      - bytesInitialAlloc);

                if (veryVerbose) {
                    cout << endl;
                }
            }

            mX.rewind();  // ACTION

            ASSERTV(i, 1 == sa.numBlocksInUse());

            for (int j = 1; j <= i; ++j) {

                if (veryVerbose) { T_ T_ P(j) }

                ASSERT(mX.allocate(k_DEFAULT_SIZE));  // ACTION

                static const bsls::Types::Int64 bytesPerAllocation =
                                                            sa.numBytesInUse();

                ASSERTV(i, j, j                      == sa.numBlocksInUse());
                ASSERTV(i, j, j * bytesPerAllocation == sa. numBytesInUse());
            }

            mX.rewind();  // ACTION

            ASSERTV(i, 1 == sa.numBlocksInUse());
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // GLOBAL OPERATOR NEW TEST
        //
        // Concerns:
        //: 1 That a 'bdlma::SmallSequentialPool' can be used directly with
        //:   'operator new'.
        //
        // Plan:
        //: 1 Since 'bdlma::SmallSequentialPool' is thoroughly tested at this
        //:   point, we just need to make sure that 'new' forwards the memory
        //:   request directly.  (C-1)
        //
        // Testing:
        //   operator delete(void *, bdlma::SmallSequentialPool&);'
        //   operator new(size_t, bdlma::SmallSequentialPool&);'
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "GLOBAL OPERATOR NEW TEST" << endl
                                  << "========================" << endl;

        Obj mX(&objectAllocator);

        double *d = new(mX) double(3.0);
        (void)d;

        ASSERT(0 !=  objectAllocator.numBytesInUse());
        ASSERT(0 == defaultAllocator.numBytesInUse());
        ASSERT(0 ==  globalAllocator.numBytesInUse());

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // 'reserveCapacity' TEST
        //
        // Concerns:
        //: 1 That if there is sufficient memory within the buffer,
        //:   'reserveCapacity' should not trigger dynamic allocation.
        //:
        //: 2 That we can allocate at least the amount of bytes specified in
        //:   'reserveCapacity' before triggering another dynamic allocation.
        //:
        //: 3 That 'reserveCapacity' can override the maximum buffer size
        //:   parameter supplied to the pool at construction.
        //:
        //: 4 That 'reserveCapacity' has no effect when invoked with 0.
        //
        // Plan:
        //: 1 Create a 'bdlma::SmallSequentialPool' using a test allocator and
        //:   specify an initial size and maximum buffer size.
        //:
        //: 2 Invoke 'reserveCapacity' with a size less than the initial size.
        //:   Allocate the same amount of memory, and verify, using the test
        //:   allocator, that no new dynamic allocation is triggered.  (C-1)
        //:
        //: 3 Invoke 'reserveCapacity' with a size larger than the initial
        //:   size.  Verify that the call triggers dynamic allocation, and that
        //:   we can allocate the same amount of memory without triggering
        //:   further dynamic allocation.  (C-2)
        //:
        //: 4 Invoke 'reserveCapacity' with a size larger than the maximum
        //:   buffer size.  Repeat verification done in P-2.  (C-3)
        //:
        //: 5 Invoke 'reserveCapacity' with an argument of 0.  Confirm that
        //:   there no dynamic allocation is triggered.  (C-4)
        //
        // Testing:
        //   void reserveCapacity(int numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'reserveCapacity' TEST" << endl
                                  << "======================" << endl;

        enum { k_INITIAL_SIZE = 64, k_MAX_BUFFER = k_INITIAL_SIZE * 4 };

        if (verbose) cout << "\nTesting that 'reserveCapacity' triggers "
                             "dynamic memory allocation only when needed."
                          << endl;

        {
            Obj                mX(k_INITIAL_SIZE,
                                  k_MAX_BUFFER,
                                  &objectAllocator);
            bsls::Types::Int64 numBytesUsed = objectAllocator.numBytesInUse();

            mX.reserveCapacity(k_INITIAL_SIZE / 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            mX.allocate(k_INITIAL_SIZE / 2);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            mX.reserveCapacity(0);
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
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());
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

            mX.reserveCapacity(0);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());

            mX.reserveCapacity(k_DEFAULT_SIZE * 4);
            ASSERT(numBytesUsed < objectAllocator.numBytesInUse());
            numBytesUsed = objectAllocator.numBytesInUse();

            mX.allocate(k_DEFAULT_SIZE * 4);
            ASSERT(numBytesUsed == objectAllocator.numBytesInUse());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // 'truncate' TEST
        //
        // Concerns:
        //: 1 The 'truncate' method reduces the amount of memory allocated to
        //:   the specified 'newSize'.
        //:
        //: 2 When 'truncate' fails, 'originalSize' is returned.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, create test vectors having the
        //:   alignment strategy, initial allocation size, the new size, and
        //:   expected offset.  First allocate memory of the initial allocation
        //:   size, then truncate to the new size and allocate memory (1 byte)
        //:   again.  Verify that the latest allocation matches the expected
        //:   offset.  (C-1)
        //:
        //: 2 Truncate the memory returned by the initial allocation, and
        //:   verify that the return value is 'originalSize'.  (C-2)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered.  (C-3)
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
                             static_cast<char *>(addr1) + EXPOFFSET
                          == static_cast<char *>(addr2));
            }
            else {
                ASSERT(used < objectAllocator.numBytesInUse());
            }

            // Truncating previously allocated address should fail.
            if (EXPOFFSET >= 0
             && (static_cast<char *>(addr1) + INITIALSIZE)
             != (static_cast<char *>(addr2) + 1)) {
                bsl::size_t ret = mX.truncate(addr1, INITIALSIZE, NEWSIZE);
                LOOP2_ASSERT(INITIALSIZE, ret,
                             static_cast<bsl::size_t>(INITIALSIZE) == ret);
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
        //: 1 The 'allocateAndExpand' method returns the maximum amount of
        //:   memory available for use without triggering another allocation.
        //:
        //: 2 The 'allocateAndExpand' method returns the updated size of memory
        //:   used.
        //:
        //: 3 The 'allocateAndExpand' method performs as expected for
        //:   zero-sized allocations.
        //
        // Plan:
        //: 1 Using the table-driven technique, create test vectors having the
        //:   alignment strategy, initial memory offset, and expected memory
        //:   used.  First allocate memory necessary for the initial memory
        //:   offset, then allocate 1 byte using 'allocateAndExpand'.  Verify
        //:   the updated size is the same as the expected memory used.
        //:   Finally, invoke 'allocate' again and verify it triggers new
        //:   dynamic memory allocation -- meaning 'allocateAndExpand' did use
        //:   up all available memory in the buffer.  (C-1..2)
        //:
        //: 2 Call the 'allocateAndExpand' method with 0 and non-zero arguments
        //:   and confirm that the returned addresses are 0 and non-zero,
        //:   respectively.  (C-3)
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
            {  L_,      NAT,           1,   k_DEFAULT_SIZE * 1 - 1           },
            {  L_,      NAT,           2,   k_DEFAULT_SIZE * 1 - 2           },
            {  L_,      NAT,           3,   k_DEFAULT_SIZE * 1 - 3           },
            {  L_,      NAT,           4,   k_DEFAULT_SIZE * 1 - 4           },
            {  L_,      NAT,           7,   k_DEFAULT_SIZE * 1 - 7           },
            {  L_,      NAT,           8,   k_DEFAULT_SIZE * 1 - 8           },
            {  L_,      NAT,          15,   k_DEFAULT_SIZE * 1 - 15          },
            {  L_,      NAT,          16,   k_DEFAULT_SIZE * 1 - 16          },
            {  L_,      NAT,         100,   k_DEFAULT_SIZE * 1 - 100         },
            {  L_,      NAT,         510,   k_DEFAULT_SIZE * 2 - 510         },
            {  L_,      NAT,         511,   k_DEFAULT_SIZE * 2 - 511         },

            // MAXIMUM ALIGNMENT
            {  L_,      MAX,           1,   k_DEFAULT_SIZE * 1 - k_MAX_ALIGN },
            {  L_,      MAX,           2,   k_DEFAULT_SIZE * 1 - k_MAX_ALIGN },
            {  L_,      MAX,           3,   k_DEFAULT_SIZE * 1 - k_MAX_ALIGN },
            {  L_,      MAX,           4,   k_DEFAULT_SIZE * 1 - k_MAX_ALIGN },
            {  L_,      MAX,           7,   k_DEFAULT_SIZE * 1 - k_MAX_ALIGN },
            {  L_,      MAX,           8,   k_DEFAULT_SIZE * 1 - k_MAX_ALIGN },
            {  L_,      MAX,          15,   k_DEFAULT_SIZE * 1 - 16          },
            {  L_,      MAX,          16,   k_DEFAULT_SIZE * 1 - 16          },
            {  L_,      MAX,         108,   k_DEFAULT_SIZE * 1 - 112         },

            // 1-BYTE ALIGNMENT
            {  L_,      BYT,           1,   k_DEFAULT_SIZE * 1 - 1           },
            {  L_,      BYT,           2,   k_DEFAULT_SIZE * 1 - 2           },
            {  L_,      BYT,           3,   k_DEFAULT_SIZE * 1 - 3           },
            {  L_,      BYT,           4,   k_DEFAULT_SIZE * 1 - 4           },
            {  L_,      BYT,           7,   k_DEFAULT_SIZE * 1 - 7           },
            {  L_,      BYT,           8,   k_DEFAULT_SIZE * 1 - 8           },
            {  L_,      BYT,          15,   k_DEFAULT_SIZE * 1 - 15          },
            {  L_,      BYT,          16,   k_DEFAULT_SIZE * 1 - 16          },
            {  L_,      BYT,         100,   k_DEFAULT_SIZE * 1 - 100         },
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

            void      *addr1  = mX.allocate(INITIALSIZE);
            const int  GROWTH = INITIALSIZE/k_DEFAULT_SIZE + 1;
            ASSERTV(blockSize(k_DEFAULT_SIZE * GROWTH),
                    objectAllocator.numBytesInUse(),
                    blockSize(k_DEFAULT_SIZE * GROWTH) ==
                                              objectAllocator.numBytesInUse());
            ASSERT(1 == objectAllocator.numBlocksInUse());

            bsls::Types::size_type  size  = 1;
            void                   *addr2 = mX.allocateAndExpand(&size);

            // Check for correct memory address.
            if (NAT == STRAT || BYT == STRAT) {
                ASSERT((char *)addr1 + INITIALSIZE == (char *)addr2);
            }
            else {
                int offset = bsls::AlignmentUtil::calculateAlignmentOffset(
                                      static_cast<char *>(addr1) + INITIALSIZE,
                                      k_MAX_ALIGN);
                ASSERT(static_cast<char *>(addr1) + INITIALSIZE + offset
                    == static_cast<char *>(addr2));
            }

#undef NAT
#undef MAX
#undef BYT

            // Check 'size' is updated correctly.
            ASSERT(EXPUSED == (int)size);

            // Check for no new allocations.
            ASSERT(blockSize(k_DEFAULT_SIZE * GROWTH) ==
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
            ASSERT(blockSize(k_DEFAULT_SIZE * 1) ==
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
                Obj                    mX;
                bsls::Types::size_type size = 1;

                ASSERT_SAFE_PASS(mX.allocateAndExpand(&size));

                ASSERT_SAFE_FAIL(mX.allocateAndExpand(    0));
            }

        }

        if (veryVerbose) cout << "\t'0 <= *size'" << endl;
        {
            Obj                     mX;
            bsls::Types::size_type  size = 1;

            ASSERT(0 != mX.allocateAndExpand(&size));

            size = 0;

            ASSERT(0 == mX.allocateAndExpand(&size));
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // DELETEOBJECT TEST
        //
        // Concerns:
        //: 1 That both the 'deleteObject' and 'deleteObjectRaw' methods invoke
        //:   the destructor of the object passed in.
        //
        // Plan:
        //: 2 Since 'deleteObject' and 'deleteObjectRaw' do not deallocate
        //:   memory, we just need to ensure that the destructor of the object
        //:   passed in is invoked.
        //
        // Testing:
        //   void deleteObjectRaw(const TYPE *object);
        //   void deleteObject(const TYPE *object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DELETEOBJECT TEST" << endl
                                  << "=================" << endl;

        if (verbose) cout << "\nTesting 'deleteObject'." << endl;
        {
            Obj       mX;
            void     *addr = mX.allocate(sizeof(my_Class));
            my_Class *obj  = new(addr) my_Class();

            globalDestructorInvoked = false;
            mX.deleteObject(obj);

            ASSERT(true == globalDestructorInvoked);
        }

        if (verbose) cout << "\nTesting 'deleteObjectRaw'." << endl;
        {
            Obj       mX;
            void     *addr = mX.allocate(sizeof(my_Class));
            my_Class *obj = new(addr) my_Class();

            globalDestructorInvoked = false;
            mX.deleteObjectRaw(obj);

            ASSERT(true == globalDestructorInvoked);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // 'release' AND 'rewind' TEST
        //
        // Concerns:
        //: 1 All memory allocated from the allocator supplied at construction
        //:   is deallocated after 'release'.
        //:
        //: 2 Subsequent allocation requests after invocation of the 'release'
        //:   method follow the specified growth and alignment strategies.
        //:
        //: 3 The 'rewind' method retains the most recently allocated block
        //:   when the constant-growth strategy is used and when then
        //:   constant-growth strategy lapses into geometric growth.
        //
        // Plan:
        //: 1 Using the table-driven technique, create test vectors having an
        //:   initial buffer size, an allocation request size, and a number of
        //:   allocations to request.  For each alignment strategy, construct a
        //:   sequential pool using a 'bslma::TestAllocator', and specifying
        //:   the initial buffer size from the table and constant buffer
        //:   growth.  Then allocate a sequence of memory blocks, the number
        //:   and size of which are specified in the table.  Then invoke
        //:   'release' and verify, using the test allocator, that there is no
        //:   outstanding memory allocated.  Finally, allocate memory again and
        //:   verify the alignment and growth strategies.  (C-1..2)
        //:
        //: 2 Issue a series a memory requests designed so that the final
        //:   request triggers the allocation of a new block then 'rewind' and
        //:   allocate a single byte.  The address of the allocated byte should
        //:   match that of the last allocation before rewind.  Confirm this in
        //:   scenarios using the constant-growth strategy and when
        //:   constant-growth lapses into geometric growth.  (C-3)
        //
        // Testing:
        //   void release();
        //   void rewind();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'release' AND 'rewind' TEST" << endl
                                  << "===========================" << endl;

#define CON bsls::BlockGrowth::BSLS_CONSTANT

        const int TH = 64;  // threshold

        static const struct {
            int  d_line;         // line number
            int  d_bufSize;      // buffer size
            int  d_requestSize;  // request size
            int  d_numRequests;  // number of requests

            bool d_isGeometric;  // expect geometric growth
        } DATA[] = {
            //LINE      BUFSIZE     REQUEST SIZE    # REQUESTS        IS_GEO
            //----      -------     ------------    ----------        ------

            {  L_,      1,          1,              2               , false  },
            {  L_,      1,          5,              2               , true   },
            {  L_,      1,          TH - 1,         2               , true   },
            {  L_,      1,          TH,             2               , true   },
            {  L_,      1,          TH + 1,         2               , true   },

            {  L_,      TH - 1,     1,              TH              , false  },
            {  L_,      TH - 1,     5,              1 + (TH - 1) / 5, false  },
            {  L_,      TH - 1,     TH - 2,         2               , false  },
            {  L_,      TH - 1,     TH - 1,         2               , false  },
            {  L_,      TH - 1,     TH,             2               , true   },

            {  L_,      TH,         1,              TH + 1          , false  },
            {  L_,      TH,         5,              1 + TH / 5      , false  },
            {  L_,      TH,         TH - 1,         2               , false  },
            {  L_,      TH,         TH,             2               , false  },
            {  L_,      TH,         TH + 1,         2               , true   },

            {  L_,      TH + 1,     1,              TH + 2          , false  },
            {  L_,      TH + 1,     5,              1 + (TH + 1) / 5, false  },
            {  L_,      TH + 1,     TH,             2               , false  },
            {  L_,      TH + 1,     TH + 1,         2               , false  },
            {  L_,      TH + 1,     TH + 2,         2               , true   },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int  LINE    = DATA[i].d_line;
            const int  BUFSIZE = DATA[i].d_bufSize;
            const int  REQSIZE = DATA[i].d_requestSize;
            const int  NUMREQ  = DATA[i].d_numRequests;
            const bool IS_GEO  = DATA[i].d_isGeometric;

            const int MAXNUMREQ  = TH + 2;
            LOOP2_ASSERT(MAXNUMREQ, NUMREQ, MAXNUMREQ >= NUMREQ);
            if (MAXNUMREQ < NUMREQ) continue;

            if (veryVerbose) {
                P_(i) P_(LINE) P_(BUFSIZE) P_(REQSIZE) P_(NUMREQ) P(IS_GEO)
            }

            // Try each test using maximum, natural, and 1-byte alignment.

            for (Strat strategy  = bsls::Alignment::BSLS_MAXIMUM;
                       strategy <= bsls::Alignment::BSLS_BYTEALIGNED;
                       strategy = (Strat)(strategy + 1)) {

                if (veryVerbose) {
                    T_ P(strategy)
                }

                bslma::TestAllocator ta(veryVeryVeryVerbose);

                Obj mX(BUFSIZE, CON, strategy, &ta);
                ASSERT(1 == ta.numBlocksInUse());
                ASSERT(blockSize(BUFSIZE) == ta.numBytesInUse());

                // Make 'NUMREQ' requests for memory, recording how the
                // allocator was used after each request.

                for (int reqNum = 0; reqNum < NUMREQ; ++reqNum) {
                    bsl::size_t  requestSize = IS_GEO
                                             ? REQSIZE * (1 << reqNum)
                                             : REQSIZE;
                    void        *returnAddr = mX.allocate(requestSize);
                    LOOP2_ASSERT(LINE, reqNum, returnAddr);
                    LOOP2_ASSERT(LINE, reqNum, ta.numBlocksInUse());

                    if (veryVerbose) {
                        T_ T_ P_(reqNum) P_(requestSize) P(returnAddr);
                    }
                }

                // Now call 'release' and verify that all memory is returned.

                if (veryVerbose) {
                    T_ T_ Q(release)
                }

                mX.release();  // ACTION
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(0 == ta.numBytesInUse());

                // Again, make 'NUMREQ' requests for memory, recording how the
                // allocator was used after each request.

                for (int reqNum = 0; reqNum < NUMREQ; ++reqNum) {
                    bsl::size_t  requestSize = IS_GEO
                                             ? REQSIZE * (1 << reqNum)
                                             : REQSIZE;
                    void        *returnAddr = mX.allocate(requestSize);
                    LOOP2_ASSERT(LINE, reqNum, returnAddr);
                    LOOP2_ASSERT(LINE, reqNum, ta.numBlocksInUse());

                    if (veryVerbose) {
                        T_ T_ P_(reqNum) P_(requestSize) P(returnAddr);
                    }
                }

                bslma::TestAllocatorMonitor tam(&ta);

                bsl::size_t additionalRequestSize
                                                = IS_GEO
                                                ? REQSIZE * (1 << (NUMREQ + 1))
                                                : BUFSIZE;
                if (veryVerbose) {
                    T_ T_ P(additionalRequestSize)
                }
                void *addrPre =  mX.allocate(additionalRequestSize);

                if (veryVerbose) {
                    T_ T_ P(addrPre)
                }

                ASSERT(1 == tam.numBlocksInUseChange());

                // Now release all the allocations, but keep the last buffer
                // to use again.

                if (veryVerbose) {
                    T_ T_ Q(rewind)
                }

                mX.rewind();  // ACTION
                void *addrPost = mX.allocate(1);
                ASSERTV(addrPre,   addrPost,
                        addrPre == addrPost);
                ASSERT(1 == ta.numBlocksInUse());

                mX.release();
                ASSERT(0 == ta.numBlocksInUse());
                ASSERT(0 == ta.numBytesInUse());

                if (1 == BUFSIZE) continue;

                if (veryVerbose) cout << "\nTesting alignment and growth"
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
        // 'allocate' TEST
        //   Note that this test alone is insufficient to thoroughly test the
        //   the 'allocate' method, as the constructors, which are not
        //   thoroughly tested yet, are used in this test.  However, a
        //   combination of both this test and test case 3 provides complete
        //   test coverage for both the constructors and the 'allocate' method.
        //
        // Concerns:
        //: 1 If 'initialSize' is specified then all requests up to
        //:   'initialSize' require no additional allocations.  Subsequent
        //:   buffer resizes are constant or geometric based on the specified
        //:   growth strategy.
        //:
        //: 2 All requests over a specified THRESHOLD are satisfied directly
        //:   from the block list if the they cannot be satisfied by the pool's
        //:   buffer.
        //:
        //: 3 The 'allocate' method performs as expected for zero-sized
        //:   allocations.
        //
        // Plan:
        //: 1 Construct objects 'mV', 'mW', 'mX', and 'mY' with default,
        //:   maximum, natural, and 1-byte alignment allocation strategies
        //:   using the default constructor and the constructor taking an
        //:   'initialSize' and an alignment strategy.  Additionally pass a
        //:   test allocator to the constructor to monitor the memory
        //:   allocations by the pool.  Confirm the bytes allocated by the
        //:   objects are as expected.  (C-1..2)
        //:
        //: 2 Call the 'allocate' method with 0 and non-zero arguments and
        //:   confirm that the returned addresses are 0 and non-zero,
        //:   respectively.  (C-3)
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

                if (veryVerbose) { T_ P_(i) P(SIZE) }

                bslma::TestAllocator ta(veryVeryVeryVerbose),
                                     tb(veryVeryVeryVerbose),
                                     tc(veryVeryVeryVerbose),
                                     td(veryVeryVeryVerbose);

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

                if (SIZE <= k_DEFAULT_SIZE) {
                    LOOP2_ASSERT(i, SIZE,
                          blockSize(k_DEFAULT_SIZE) == ta.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                          blockSize(k_DEFAULT_SIZE) == tb.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                          blockSize(k_DEFAULT_SIZE) == tc.numBytesInUse());
                    LOOP2_ASSERT(i, SIZE,
                          blockSize(k_DEFAULT_SIZE) == td.numBytesInUse());
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

                if (veryVerbose) {
                    T_ P_(i) P(SIZE)
                }

                for (int j = 0; j < NUM_INITIAL_SIZES; ++j) {
                    const int                         INITIAL_SIZE =
                                                              INITIAL_SIZES[j];
                    const bsls::BlockGrowth::Strategy STRATEGY     =  STRAT[j];

                    bslma::TestAllocator ta(veryVeryVeryVerbose),
                                         tb(veryVeryVeryVerbose),
                                         tc(veryVeryVeryVerbose),
                                         td(veryVeryVeryVerbose);

                    if (veryVerbose) {
                        T_ T_ P_(j) P_(STRATEGY) P(INITIAL_SIZE)
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
                    else {  // constant-growth strategy
                        if (0 == INITIAL_SIZE && SIZE < k_DEFAULT_SIZE) {
                            BSLS_ASSERT(!"Reached");

                            LOOP_ASSERT(i, NA + blockSize(k_DEFAULT_SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, NB + blockSize(k_DEFAULT_SIZE)
                                                        == tb.numBytesInUse());
                            LOOP_ASSERT(i, NC + blockSize(k_DEFAULT_SIZE)
                                                        == tc.numBytesInUse());
                            LOOP_ASSERT(i, ND + blockSize(k_DEFAULT_SIZE)
                                                        == td.numBytesInUse());
                        }
                        else if (SIZE <= INITIAL_SIZE) {
                            LOOP_ASSERT(i, NA + blockSize(SIZE)
                                                        == ta.numBytesInUse());
                            LOOP_ASSERT(i, NB + blockSize(SIZE)
                                                        == tb.numBytesInUse());
                            LOOP_ASSERT(i, NC + blockSize(SIZE)
                                                        == tc.numBytesInUse());
                            LOOP_ASSERT(i, ND + blockSize(SIZE)
                                                        == td.numBytesInUse());
                        }
                        else {  // constant-growth going geometric
                            int nextSize = calculateNextSize(INITIAL_SIZE,
                                                             SIZE);
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
                    T_ P_(i) P(SIZE)
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
                    const int                         INITIAL_SIZE =
                                                              INITIAL_SIZES[j];
                    const bsls::BlockGrowth::Strategy STRATEGY = STRATEGIES[j];

                    bslma::TestAllocator ta(veryVeryVeryVerbose),
                                         tb(veryVeryVeryVerbose),
                                         tc(veryVeryVeryVerbose),
                                         td(veryVeryVeryVerbose);

                    if (veryVerbose) {
                        T_ T_ P_(j) P_(INITIAL_SIZE) P(SIZE)
                    }

                    const int NUM_MAX_SIZES = 3;
                    int       MAX_SIZES[NUM_MAX_SIZES];
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
                            else {  // constant-growth strategy
                                if (ALLOC_SIZE <= INITIAL_SIZE // pool  block
                                 || ALLOC_SIZE >  MAX_SIZE     // large block
                                                ) {
                                    LOOP_ASSERT(i, NA + blockSize(ALLOC_SIZE)
                                                        == ta.numBytesInUse());
                                    LOOP_ASSERT(i, NB + blockSize(ALLOC_SIZE)
                                                        == tb.numBytesInUse());
                                    LOOP_ASSERT(i, NC + blockSize(ALLOC_SIZE)
                                                        == tc.numBytesInUse());
                                    LOOP_ASSERT(i, ND + blockSize(ALLOC_SIZE)
                                                        == td.numBytesInUse());
                                }
                                else { // constant-growth going geometric
                                    int nextSize = calculateNextSize(
                                                                  INITIAL_SIZE,
                                                                  ALLOC_SIZE);
                                    if (veryVeryVerbose) {
                                        T_ T_ T_ T_ P_(nextSize)
                                                    P(blockSize(nextSize))
                                        typedef bsls::Types::Int64 Int64;
                                        Int64 diffA = ta.numBytesInUse() - NA;
                                        Int64 diffB = tb.numBytesInUse() - NB;
                                        Int64 diffC = tc.numBytesInUse() - NC;
                                        Int64 diffD = td.numBytesInUse() - ND;
                                        T_ T_ T_ T_ P_(diffA)
                                                    P_(diffB)
                                                    P_(diffC)
                                                    P(diffD)
                                    }
                                    LOOP_ASSERT(i, NA + blockSize(nextSize)
                                                        == ta.numBytesInUse());
                                    LOOP_ASSERT(i, NB + blockSize(nextSize)
                                                        == tb.numBytesInUse());
                                    LOOP_ASSERT(i, NC + blockSize(nextSize)
                                                        == tc.numBytesInUse());
                                    LOOP_ASSERT(i, ND + blockSize(nextSize)
                                                        == td.numBytesInUse());
                                }
                            }
                        }
                    }
                }
            }
        }

        if (veryVerbose) cout << "\t'allocate(0 <= size)'" << endl;
        {
            Obj mX;

            ASSERT(0 == mX.allocate(0));
            ASSERT(0 != mX.allocate(1));
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONSTRUCTOR TEST
        //   Note that this test alone is insufficient to thoroughly test the
        //   constructors, as the untested 'allocate' method is used to verify
        //   that the buffer, alignment strategy, and allocator are properly
        //   passed to the pool at construction.  However, we cannot test
        //   'allocate' first -- as it requires the constructors.  Hence, it is
        //   a combination of both this test and test case 4 that provides
        //   complete test coverage for the constructor and the 'allocate'
        //   method.
        //
        // Concerns:
        //: 1 That when an allocator is not supplied, the currently installed
        //:   default allocator is used.
        //:
        //: 2 That when an alignment strategy is not specified, natural
        //:   alignment is used.
        //:
        //: 3 That when a growth strategy is not specified, geometric growth is
        //:   used.
        //:
        //: 4 That when an initial size is not specified, the
        //:   'bdlma::SmallSequentialPool' does not allocate memory at
        //:   construction.
        //:
        //: 5 That if an initial size is specified, the pool allocates the
        //:   specified amount of memory at construction.
        //:
        //: 6 That the proper growth strategy, alignment strategy, and
        //:   allocator are used if specified at construction.
        //:
        //: 7 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 In a series of individual tests, invoke each constructor and
        //:   confirm that the object has the expected attributes.
        //:
        //: 2 Verify that in appropriate build modes, defensive checks are
        //:   triggered.  (C-7)
        //
        // Testing:
        //   SmallSequentialPool(*a = 0);
        //   SmallSequentialPool(GS g, *a = 0);
        //   SmallSequentialPool(AS a, *a = 0);
        //   SmallSequentialPool(GS g, AS a, *a = 0);
        //
        //   SmallSequentialPool(int  i);
        //   SmallSequentialPool(size i, *a = 0);
        //   SmallSequentialPool(size i, GS g, *a = 0);
        //   SmallSequentialPool(size i, AS a, *a = 0);
        //   SmallSequentialPool(size i, GS g, AS a, *a = 0);
        //
        //   SmallSequentialPool(size i, int m, *a = 0);
        //   SmallSequentialPool(size i, int m, GS g, *a = 0);
        //   SmallSequentialPool(size i, int m, AS a, *a = 0);
        //   SmallSequentialPool(size i, int m, GS g, AS a, *a = 0);
        //
        //   SmallSequentialPool(size i, int m, GS g, AS a, bool aIB, *a = 0);
        //
        //   ~SmallSequentialPool();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "CONSTRUCTOR TEST" << endl
                                  << "================" << endl;

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
                void *addr1 = 0;
                addr1 = mA.allocate(k_DEFAULT_SIZE + 1);
                addr1 = mA.allocate(k_DEFAULT_SIZE + 1); // triggers allocation
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
                void *addr1 = 0;
                addr1 = mB.allocate(k_DEFAULT_SIZE + 1);
                addr1 = mB.allocate(k_DEFAULT_SIZE + 1); // triggers allocation
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

                if (veryVerbose) {
                    P_(LINE) P_(STRAT) P_(ALLOCSIZE) P(EXPOFFSET)
                }

                ASSERT(0 == objectAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numBytesInUse());
                ASSERT(0 == globalAllocator.numBytesInUse());

                Obj mX(STRAT, &objectAllocator);

                ASSERT(0 == objectAllocator.numBytesInUse());
                ASSERT(0 == defaultAllocator.numBytesInUse());
                ASSERT(0 == globalAllocator.numBytesInUse());

                char *addr1 = static_cast<char *>(mX.allocate(1));
                char *addr2 = static_cast<char *>(mX.allocate(ALLOCSIZE));

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

        if (verbose) {
            cout << "\nTesting specification of 'allocateInitialBuffer'."
                 << endl;
        }
        {
            ASSERT(0 == objectAllocator.numBlocksInUse());
            ASSERT(0 == defaultAllocator.numBlocksInUse());
            ASSERT(0 == globalAllocator.numBlocksInUse());

            {
                Obj mX(128, 128, CON, NAT, false);
                ASSERT(0 == objectAllocator.numBlocksInUse());
                ASSERT(0 == defaultAllocator.numBlocksInUse());
                ASSERT(0 == globalAllocator.numBlocksInUse());
            }

            {
                Obj mX(128, 128, CON, NAT, true);
                ASSERT(0 == objectAllocator.numBlocksInUse());
                ASSERT(1 == defaultAllocator.numBlocksInUse());
                ASSERT(0 == globalAllocator.numBlocksInUse());
            }

            {
                Obj mX(128, 128, GEO, NAT, false);
                ASSERT(0 == objectAllocator.numBlocksInUse());
                ASSERT(0 == defaultAllocator.numBlocksInUse());
                ASSERT(0 == globalAllocator.numBlocksInUse());
            }

            {
                Obj mX(128, 128, GEO, NAT, true);
                ASSERT(0 == objectAllocator.numBlocksInUse());
                ASSERT(1 == defaultAllocator.numBlocksInUse());
                ASSERT(0 == globalAllocator.numBlocksInUse());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'Obj(i, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1));

                ASSERT_SAFE_FAIL(Obj( 0));
                ASSERT_SAFE_FAIL(Obj(-1));  // 'i' is type 'int'.
            }

            if (veryVerbose) cout << "\t'Obj(i, GS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, CON));
                ASSERT_SAFE_FAIL(Obj( 0, CON));
            }

            if (veryVerbose) cout << "\t'Obj(i, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, MAX));
                ASSERT_SAFE_FAIL(Obj( 0, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(i, GS, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, CON, MAX));
                ASSERT_SAFE_FAIL(Obj( 0, CON, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1,  8));
                ASSERT_SAFE_FAIL(Obj( 0,  8));
                ASSERT_SAFE_FAIL(Obj( 9,  8));

                ASSERT_SAFE_PASS(Obj( 2,  2));
                ASSERT_SAFE_FAIL(Obj( 2,  1));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, GS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1,  8, CON));
                ASSERT_SAFE_FAIL(Obj( 0,  8, CON));
                ASSERT_SAFE_FAIL(Obj( 9,  8, CON));

                ASSERT_SAFE_PASS(Obj( 2,  2, CON));
                ASSERT_SAFE_FAIL(Obj( 2,  1, CON));
                ASSERT_SAFE_FAIL(Obj( 2,  0, CON));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1,  8, MAX));
                ASSERT_SAFE_FAIL(Obj( 0,  8, MAX));
                ASSERT_SAFE_FAIL(Obj( 9,  8, MAX));

                ASSERT_SAFE_PASS(Obj( 2,  2, MAX));
                ASSERT_SAFE_FAIL(Obj( 2,  1, MAX));
                ASSERT_SAFE_FAIL(Obj( 2,  0, MAX));
            }

            if (veryVerbose) cout << "\t'Obj(i, m, GS, AS, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1,  8, CON, MAX));
                ASSERT_SAFE_FAIL(Obj( 0,  8, CON, MAX));
                ASSERT_SAFE_FAIL(Obj( 9,  8, CON, MAX));

                ASSERT_SAFE_PASS(Obj( 2,  2, CON, MAX));
                ASSERT_SAFE_FAIL(Obj( 2,  1, CON, MAX));
                ASSERT_SAFE_FAIL(Obj( 2,  0, CON, MAX));
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
        //: 1 The size returned by 'blockSize' matches the amount of bytes
        //:   allocated by the 'bdlma::InfrequentDeleteBlockList::allocate'
        //:   method.
        //
        // Plan:
        //: 1 Create a 'bdlma::InfrequentDeleteBlockList' object initialized
        //:   with a test allocator.  Invoke both the 'blockSize' function and
        //:   the 'bdlma::InfrequentDeleteBlockList::allocate' method with
        //:   varying memory sizes, and verify that the sizes returned by
        //:   'blockSize' are equal to the memory request sizes recorded by the
        //:   allocator.  (C-1)
        //
        // Testing:
        //   HELPER FUNCTION: 'int blockSize(numBytes)'
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER FUNCTION TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "\nTesting 'blockSize'." << endl;

        const int DATA[] = { 0, 1, 5, 12, 24, 64, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator             a(veryVeryVeryVerbose);
        bdlma::InfrequentDeleteBlockList bl(&a);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            int       blkSize = blockSize(SIZE);
            bl.allocate(SIZE);

            // If the first 'SIZE' is 0, the 'allocate' method of the
            // (upstream) allocator is never called; thus, the
            // 'lastAllocatedNumBytes' method returns -1 instead of 0.

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
        //: 1 A 'bdlma::SmallSequentialPool' can be created and destroyed.
        //:
        //: 2 The 'allocate' method returns a block of memory having the
        //:   specified size and expected alignment.
        //:
        //: 3 That 'allocate' method does not always causes dynamic allocation
        //:   (i.e., the pool manages an buffer of memory)
        //:
        //: 4 The 'allocate' method returns a block of memory even when the the
        //:   allocation request exceeds the initial size of the buffer.
        //:
        //: 5 Destruction of the pool releases all managed memory.
        //
        // Plan:
        //: 1 Initialize a 'bdlma::SmallSequentialPool' with a
        //:   'bslma::TestAllocator' (C-1).  Then, allocate a block of memory,
        //:   and verify that it comes from the test allocator.  Allocate
        //:   another block of memory, and verify that no dynamic allocation is
        //:   triggered (C-3).  Verify the alignment and size of the first
        //:   allocation by checking the address of the second allocation
        //:   (C-2).
        //:
        //: 2 Allocate a large block of memory and verify the memory comes from
        //:   the test allocator (C-4).  Finally, destroy the pool and check
        //:   that all allocated memory is deallocated (C-5).
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

            if (verbose) cout << "\nTesting buffering." << endl;
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

            mX.rewind();
            ASSERT(1 == objectAllocator.numBlocksInUse());
        }

        if (verbose) cout << "\nTesting destruction." << endl;
        ASSERT(0 == objectAllocator.numBlocksInUse());
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
// Copyright 2020 Bloomberg Finance L.P.
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
