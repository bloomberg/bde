// bdlma_pool.t.cpp                                                   -*-C++-*-
#include <bdlma_pool.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_blockgrowth.h>
#include <bsls_types.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The goals of this 'bdlma::Pool' test driver are to verify that: 1) the
// 'allocate' method dispenses memory blocks of the correct (uniform) size, 2)
// the pool replenishes correctly according to the 'growthStrategy' and
// 'maxBlocksPerChunk' constructor parameters, 3) the 'deallocate' method
// returns the memory to the pool, and 4) the 'release' method and the
// destructor releases all memory allocated through the pool.
//
// To achieve goal 1, initialize pools of varying block sizes.  Invoke
// 'allocate' repeatedly and verify that the difference between the returned
// memory addresses of two consecutive requests is equal to the specified block
// size for the current pool.  To achieve goal 2, initialize a pool with a test
// allocator and varying 'growthStrategy' and 'maxBlocksPerChunk'.  Invoke
// 'allocate' repeatedly and verify that the pool requests memory blocks of the
// expected sizes from the allocator.  To achieve goal 3, allocate multiple
// memory blocks from the pool and store the returned addresses in an array.
// Deallocate the memory in reverse order, then allocate memory again and
// verify that the allocated memory are in the same order as those stored in
// the array.  Note that this test depends on the implementation details of
// 'deallocate' in which a deallocated memory block is placed at the beginning
// of the free memory list.  To achieve goal 4, initialize two pools, each
// supplied with its own test allocator.  Invoke 'allocate' repeatedly.  Invoke
// 'release' on one pool, and allow the other pool to go out of scope.  Verify
// that both test allocators indicate that all memory is released.
//-----------------------------------------------------------------------------
// [ 4] Pool(bs, basicAllocator = 0);
// [ 4] Pool(bs, gs, basicAllocator = 0);
// [ 3] Pool(bs, gs, maxBlocksPerChunk, basicAllocator = 0);
// [ 6] ~Pool();
// [ 4] void *allocate();
// [ 5] void deallocate(address);
// [ 9] template <class TYPE> void deleteObject(const TYPE *object);
// [10] template <class TYPE> void deleteObjectRaw(const TYPE *object);
// [ 6] void release();
// [11] void reserveCapacity(numBlocks);
// [ 2] int blockSize() const;
// [ 7] void *operator new(bsl::size_t size, bdlma::Pool& pool);
// [ 8] void operator delete(void *address, bdlma::Pool& pool);
//-----------------------------------------------------------------------------
// [12] USAGE EXAMPLE
// [ 2] 'allocate' returns memory of the correct block size.
// [ 1] int blockSize(numBytes);
// [ 1] int poolBlockSize(size);
// [ 1] int growNumBlocks(numBlocks, maxNumBlocks);
// [ *] CONCERN: Precondition violations are detected when enabled.

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
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

//=============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
//-----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                               GLOBAL TYPEDEF
//-----------------------------------------------------------------------------

typedef bdlma::Pool                 Obj;

typedef bsls::BlockGrowth::Strategy Strategy;

// This type was copied from 'bdlma_infrequentdeleteblocklist.h' for testing
// purposes only.

struct InfrequentDeleteBlock {
    InfrequentDeleteBlock               *d_next_p;
    bsls::AlignmentUtil::MaxAlignedType  d_memory;  // force alignment
};

// The following enumerator values must be kept in sync with 'bdlma_pool.cpp'.

enum {
    k_INITIAL_NUM_BLOCKS =  1,
    k_MAX_NUM_BLOCKS     = 32,

    k_INITIAL_CHUNK_SIZE =  1,  // from 'bdlma_pool.cpp'
    k_MAX_CHUNK_SIZE     = 32   // from 'bdlma_pool.cpp'
};

// ============================================================================
//                      FILE-STATIC FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

static
int blockSize(int numBytes)
    // Return the adjusted block size based on the specified 'numBytes' using
    // the calculation performed by 'bdlma::InfrequentDeleteBlockList's
    // 'allocate' method.  The behavior is undefined unless '0 <= numBytes'.
{
    ASSERT(0 <= numBytes);

    if (numBytes) {
        numBytes += static_cast<int>(sizeof(InfrequentDeleteBlock)) - 1;
        numBytes &= ~(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1);
    }

    return numBytes;
}

static inline
int myAbs(int n)
    // Return the absolute value of the specified 'n'.  The behavior is
    // undefined unless 'INT_MIN < n'.
{
    return n >= 0 ? n : -n;
}

static inline
int roundUp(int x, int y)
    // Round up the specified 'x' to the nearest multiple of the specified 'y'.
    // The behavior is undefined unless '0 <= x' and '0 < y';
{
    ASSERT(0 <= x);
    ASSERT(0 <  y);

    return (x + y - 1) / y * y;
}

static inline
int poolBlockSize(int size)
    // Return the actual block size used by the pool when given the specified
    // 'size'.  The behavior is undefined unless '1 <= size'.
{
    return roundUp(size, bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
}

static
int growNumBlocks(int numBlocks, int maxNumBlocks)
    // Simulate the pool's growth behavior using the specified 'numBlocks' and
    // 'maxNumBlocks', and return the new value for 'numBlocks'.  The behavior
    // is undefined unless '1 <= numBlocks' and '1 <= maxNumBlocks'.
{
    ASSERT(1 <= numBlocks);
    ASSERT(1 <= maxNumBlocks);

    if (numBlocks < maxNumBlocks) {
        numBlocks *= 2;
        if (numBlocks > maxNumBlocks) {
            numBlocks = maxNumBlocks;
        }
    }
    return numBlocks;
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using a 'bdlma::Pool' for Efficient Memory Allocation
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::Pool' can be used by node-based containers (such as lists, trees,
// and hash tables that hold multiple elements of uniform size) for efficient
// memory allocation of new elements.  The following container template class,
// 'my_PooledArray', stores values of (template parameter) 'TYPE'
// "out-of-place" as nodes in a 'vector' of pointers.  Since the size of each
// node is fixed and known *a priori*, the class uses a 'bdlma::Pool' to
// allocate memory for the nodes to improve memory allocation efficiency.  Note
// that for simplicity, we assume that 'TYPE' does not require an allocator,
// and that calls to the destructor of 'TYPE' can be elided.
//
// First, we define the interface of our 'my_PooledArray' template class:
//..
    // my_poolarray.h

    template <class TYPE>
    class my_PooledArray {
        // This class implements a container that stores values of (template
        // parameter) 'TYPE' out-of-place.  It is assumed that 'TYPE' does not
        // require an allocator, and that calls to the destructor of 'TYPE' can
        // be elided.

        // DATA
        bsl::vector<TYPE *> d_array_p;  // array of pooled elements
        bdlma::Pool         d_pool;     // memory manager for array elements

      private:
        // Not implemented:
        my_PooledArray(const my_PooledArray&);

      public:
        // CREATORS
        explicit my_PooledArray(bslma::Allocator *basicAllocator = 0);
            // Create a pooled array that stores the 'TYPE' element values
            // "out-of-place".  Optionally specify a 'basicAllocator' used to
            // supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        ~my_PooledArray();
            // Destroy this array and all elements held by it.

        // MANIPULATORS
        void append(const TYPE& value);
            // Append the specified 'value' to this array.

        void removeAll();
            // Remove all elements from this array.

        // ACCESSORS
        bsl::size_t length() const;
            // Return the number of elements in this array.

        const TYPE& operator[](int index) const;
            // Return a reference providing non-modifiable access to the value
            // at the specified 'index' in this array.  The behavior is
            // undefined unless '0 <= index < length()'.
    };
//..
// Next, we provide the implementation of the 'my_PooledArray' methods that are
// defined 'inline'.
//
// Note that in the 'removeAll' method, all elements are deallocated by simply
// invoking the pool's 'release' method.  This technique implies significant
// performance gain when the array contains many elements:
//..
    // MANIPULATORS
    template <class TYPE>
    inline
    void my_PooledArray<TYPE>::removeAll()
    {
        d_array_p.clear();
        d_pool.release();
    }

    // ACCESSORS
    template <class TYPE>
    inline
    bsl::size_t my_PooledArray<TYPE>::length() const
    {
        return d_array_p.size();
    }

    template <class TYPE>
    inline
    const TYPE& my_PooledArray<TYPE>::operator[](int index) const
    {
        ASSERT(0     <= index);
        ASSERT(index <  static_cast<int>(length()));

        return *d_array_p[index];
    }
//..
// Next, we provide the implementation of the 'my_PooledArray' methods that are
// defined in the '.cpp' file.
//
// Note that the growth strategy and maximum chunk size of the pool defaults to
// those provided by 'bdlma::Pool':
//..
    // my_poolarray.cpp

    // CREATORS
    template <class TYPE>
    my_PooledArray<TYPE>::my_PooledArray(bslma::Allocator *basicAllocator)
    : d_array_p(basicAllocator)
    , d_pool(sizeof(TYPE), basicAllocator)
    {
    }
//..
// Since all memory is managed by 'd_pool', we do not have to explicitly invoke
// 'deleteObject' to reclaim outstanding memory.  The destructor of the pool
// will automatically deallocate all array elements:
//..
    template <class TYPE>
    my_PooledArray<TYPE>::~my_PooledArray()
    {
        // Elements are automatically deallocated when 'd_pool' is destroyed.
    }
//..
// Finally, note that the overloaded "placement" 'new' is used to allocate new
// nodes in the 'append' method:
//..
    // MANIPULATORS
    template <class TYPE>
    void my_PooledArray<TYPE>::append(const TYPE& value)
    {
        TYPE *tmp = new (d_pool) TYPE(value);
        d_array_p.push_back(tmp);
    }
//..

//=============================================================================
//           Additional Functionality Needed to Complete Usage Test Case
//-----------------------------------------------------------------------------

template <class TYPE>
ostream& operator<<(ostream& stream, const my_PooledArray<TYPE>& array)
{
    stream << "[ ";
    for (int i = 0; i < static_cast<int>(array.length()); ++i) {
        stream << array[i] << " ";
    }
    return stream << ']' << flush;
}

//-----------------------------------------------------------------------------
//                      OVERLOADED OPERATOR NEW USAGE EXAMPLE
//-----------------------------------------------------------------------------

class my_Type {

    // DATA
    char             *d_stuff_p;
    bslma::Allocator *d_allocator_p;

  private:
    // Not implemented:
    my_Type(const my_Type&);

  public:
    // CREATORS
    my_Type(int size, bslma::Allocator *basicAllocator)
    : d_allocator_p(basicAllocator)
    {
        d_stuff_p = (char *)d_allocator_p->allocate(size);
    }

    ~my_Type()
    {
        d_allocator_p->deallocate(d_stuff_p);
    }
};

my_Type *newMyType(bdlma::Pool *pool, bslma::Allocator *basicAllocator)
{
    return new (*pool) my_Type(5, basicAllocator);
}

void deleteMyType(bdlma::Pool *pool, my_Type *p)
{
    p->~my_Type();
    pool->deallocate(p);
}

//-----------------------------------------------------------------------------
//  HELPER CLASS FOR TESTING EXCEPTION SAFETY OF OVERLOADED OPERATOR NEW
//-----------------------------------------------------------------------------

class my_ClassThatMayThrowFromConstructor {
    char d_c;
  public:
    my_ClassThatMayThrowFromConstructor()
    {
#ifdef BDE_BUILD_TARGET_EXC
        throw int(13);
#endif
    }

    ~my_ClassThatMayThrowFromConstructor()
    {
    }
};

//=============================================================================
// CONCRETE OBJECTS FOR TESTING 'operator new' and 'deleteObject'
//-----------------------------------------------------------------------------

static int my_ClassCode = 0;

class my_Class1 {
  public:
    my_Class1()  { my_ClassCode = 1; }
    ~my_Class1() { my_ClassCode = 2; }
};

class my_Class2 {
  public:
    my_Class2()  { my_ClassCode = 3; }
    ~my_Class2() { my_ClassCode = 4; }
};

// The "dreaded diamond".

static int virtualBaseObjectCount = 0;
static int leftBaseObjectCount    = 0;
static int rightBaseObjectCount   = 0;
static int mostDerivedObjectCount = 0;

class my_VirtualBase {
    int x;
  public:
    my_VirtualBase()          { virtualBaseObjectCount = 1; }
    virtual ~my_VirtualBase() { virtualBaseObjectCount = 0; }
};

class my_LeftBase : virtual public my_VirtualBase {
    int x;
  public:
    my_LeftBase()             { leftBaseObjectCount = 1; }
    virtual ~my_LeftBase()    { leftBaseObjectCount = 0; }
};

class my_RightBase : virtual public my_VirtualBase {
    int x;
  public:
    my_RightBase()            { rightBaseObjectCount = 1; }
    virtual ~my_RightBase()   { rightBaseObjectCount = 0; }
};

class my_MostDerived : public my_LeftBase, public my_RightBase {
    int x;
  public:
    my_MostDerived()          { mostDerivedObjectCount = 1; }
    ~my_MostDerived()         { mostDerivedObjectCount = 0; }
};

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator(veryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    switch (test) { case 0:
      case 12: {
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

        if (verbose) cout << "\nTesting 'my_PooledArray'." << endl;
        {
            const double DATA[] = { 0.0, 1.2, 2.3, 3.4, 4.5, 5.6, 6.7 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma::TestAllocator a(veryVeryVerbose);
            my_PooledArray<double> array(&a);

            for (int i = 0; i < NUM_DATA; ++i) {
                const double VALUE = DATA[i];
                array.append(VALUE);
                LOOP_ASSERT(i, i + 1 == static_cast<int>(array.length()));
                LOOP_ASSERT(i, VALUE == array[i]);
            }
            if (veryVerbose) { cout << '\t' << array << endl; }
            array.removeAll();
            ASSERT(0 == array.length());
        }

        if (verbose) cout << "\nUsage test for 'new' operator." << endl;
        {
            bslma::TestAllocator a(veryVeryVerbose);
            const int BLOCK_SIZE = sizeof(my_Type);
            const int CHUNK_SIZE = 10;
            Obj mX(BLOCK_SIZE,
                   bsls::BlockGrowth::BSLS_GEOMETRIC,
                   CHUNK_SIZE,
                   &a);

            my_Type *t = newMyType(&mX, &a);
            deleteMyType(&mX, t);
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // RESERVECAPACITY TEST
        //
        // Concerns:
        //   1. That when the capacity of a pool has been reserved, calls to
        //      'allocate' do not result in memory allocations.
        //
        //   2. That 'reserveCapacity' behaves correctly when 'numBlocks' is 0,
        //      less than the current capacity, the same as the current
        //      capacity, and larger than the current capacity.
        //
        //   QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //   Reserve various capacities as described above and check the number
        //   of memory allocations.
        //
        // Testing:
        //   void reserveCapacity(numBlocks);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RESERVECAPACITY TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "\nTesting 'reserveCapacity'." << endl;

        bslma::TestAllocator a(veryVeryVerbose);
        const bslma::TestAllocator& A = a;
        {
            const int BLOCK_SIZE = 5;
            const int CHUNK_SIZE = 10;
            Obj mX(BLOCK_SIZE,
                   bsls::BlockGrowth::BSLS_CONSTANT,
                   CHUNK_SIZE,
                   &a);

            for (int i = 0; i < CHUNK_SIZE / 2; ++i) {
                mX.allocate();
            }

            bsls::Types::Int64 numAllocations = A.numAllocations();
            mX.reserveCapacity(CHUNK_SIZE / 2);
            ASSERT(A.numAllocations() == numAllocations);

            for (int ii = 0; ii < CHUNK_SIZE / 2; ++ii) {
                mX.allocate();
            }

            mX.reserveCapacity(0);
            ASSERT(A.numAllocations() == numAllocations);

            mX.reserveCapacity(CHUNK_SIZE);
            ++numAllocations;
            ASSERT(A.numAllocations() == numAllocations);

            for (int j = 0; j < CHUNK_SIZE; ++j) {
                mX.allocate();
            }
            ASSERT(A.numAllocations() == numAllocations);
        }
        ASSERT(0 == A.numBytesInUse());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX(8);

            if (veryVerbose) cout << "\t'reserveCapacity(0 <= numBlocks)'"
                                  << endl;
            {
                ASSERT_SAFE_PASS(mX.reserveCapacity( 1));
                ASSERT_SAFE_PASS(mX.reserveCapacity( 0));

                ASSERT_SAFE_FAIL(mX.reserveCapacity(-1));
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // 'deleteObjectRaw' TEST
        //
        // Concerns:
        //   That when 'deleteObjectRaw' is used, both the destructor and
        //   'deallocate' are invoked.
        //
        // Plan:
        //   Using a pool and the placement 'new' operator, construct objects
        //   of two different classes.  Invoke 'deleteObjectRaw' to delete the
        //   constructed objects and check that both the destructor and
        //   'deallocate' have been called.
        //
        // Testing:
        //   template <class TYPE> void deleteObjectRaw(const TYPE *object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'deleteObjectRaw' TEST" << endl
                                  << "======================" << endl;

        if (verbose) cout << "\nTesting 'deleteObjectRaw':" << endl;
        {
            bslma::TestAllocator a(veryVeryVerbose);
            const bslma::TestAllocator& A = a;

            const int BLOCK_SIZE = sizeof(my_Class1);
            ASSERT(sizeof(my_Class2) == BLOCK_SIZE);
            const int CHUNK_SIZE = 1;
            Obj mX(BLOCK_SIZE,
                   bsls::BlockGrowth::BSLS_CONSTANT,
                   CHUNK_SIZE,
                   &a);

            if (verbose) cout << "\tWith a 'my_Class1' object." << endl;

            my_ClassCode=0;

            my_Class1 *pC1 = (my_Class1 *) mX.allocate();
            new(pC1) my_Class1;
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(1 == my_ClassCode);
            ASSERT(1 == A.numAllocations());

            mX.deleteObjectRaw(pC1);
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(2 == my_ClassCode);
            ASSERT(1 == A.numAllocations());
            mX.allocate();
            ASSERT(1 == A.numAllocations());
                // By observing that the number of allocations remains at one,
                // we confirm that the memory obtained from the pool has been
                // returned by 'deleteObjectRaw'.  Had it not been returned,
                // the call to 'allocate' would have required another
                // allocation from the test allocator.

            if (verbose) cout << "\tWith a 'my_Class2' object." << endl;

            my_Class2 *pC2 = (my_Class2 *) mX.allocate();
            new(pC2) my_Class2;
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(3 == my_ClassCode);
            ASSERT(2 == A.numAllocations());

            mX.deleteObjectRaw(pC2);
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(4 == my_ClassCode);
            ASSERT(2 == A.numAllocations());
            mX.allocate();
            ASSERT(2 == A.numAllocations());
        }

        if (verbose) cout <<"\nTesting 'deleteObjectRaw' on polymorphic types:"
                          << endl;
        {
            bslma::TestAllocator a(veryVeryVerbose);

            const int BLOCK_SIZE = sizeof(my_MostDerived);
            ASSERT(sizeof(my_MostDerived) == BLOCK_SIZE);
            const int CHUNK_SIZE = 1;
            Obj mX(BLOCK_SIZE,
                   bsls::BlockGrowth::BSLS_CONSTANT,
                   CHUNK_SIZE,
                   &a);

            if (verbose) cout << "\t'deleteObjectRaw(my_MostDerived *)'"
                              << endl;

            my_MostDerived *pMost = (my_MostDerived *) mX.allocate();
            const my_MostDerived *pMostCONST = pMost;

            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
            new(pMost) my_MostDerived;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObjectRaw(pMostCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tWith a null pointer." << endl;

            pMost = 0;
            mX.deleteObjectRaw(pMost);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // 'deleteObject' TEST
        //
        // Concerns:
        //   That when 'deleteObject' is used, both the destructor and
        //   'deallocate' are invoked.
        //
        // Plan:
        //   Using a pool and the placement 'new' operator, construct objects
        //   of two different classes.  Invoke 'deleteObject' to delete the
        //   constructed objects and check that both the destructor and
        //   'deallocate' have been called.
        //
        // Testing:
        //   template <class TYPE> void deleteObject(const TYPE *object);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'deleteObject' TEST" << endl
                                  << "===================" << endl;

        if (verbose) cout << "\nTesting 'deleteObject':" << endl;
        {
            bslma::TestAllocator a(veryVeryVerbose);
            const bslma::TestAllocator& A = a;

            const int BLOCK_SIZE = sizeof(my_Class1);
            ASSERT(sizeof(my_Class2) == BLOCK_SIZE);
            const int CHUNK_SIZE = 1;
            Obj mX(BLOCK_SIZE,
                   bsls::BlockGrowth::BSLS_CONSTANT,
                   CHUNK_SIZE,
                   &a);

            if (verbose) cout << "\tWith a 'my_Class1' object." << endl;

            my_ClassCode=0;

            my_Class1 *pC1 = (my_Class1 *) mX.allocate();
            new(pC1) my_Class1;
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(1 == my_ClassCode);
            ASSERT(1 == A.numAllocations());

            mX.deleteObject(pC1);
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(2 == my_ClassCode);
            ASSERT(1 == A.numAllocations());
            mX.allocate();
            ASSERT(1 == A.numAllocations());
                // By observing that the number of allocations remains at one,
                // we confirm that the memory obtained from the pool has been
                // returned by 'deleteObject'.  Had it not been returned, the
                // call to 'allocate' would have required another allocation
                // from the test allocator.

            if (verbose) cout << "\tWith a 'my_Class2' object." << endl;

            my_Class2 *pC2 = (my_Class2 *) mX.allocate();
            new(pC2) my_Class2;
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(3 == my_ClassCode);
            ASSERT(2 == A.numAllocations());

            mX.deleteObject(pC2);
            if (verbose) { T_  T_  P(my_ClassCode); }
            ASSERT(4 == my_ClassCode);
            ASSERT(2 == A.numAllocations());
            mX.allocate();
            ASSERT(2 == A.numAllocations());
        }

        if (verbose) cout << "\nTesting 'deleteObject' on polymorphic types:"
                          << endl;
        {
            bslma::TestAllocator a(veryVeryVerbose);

            const int BLOCK_SIZE = sizeof(my_MostDerived);
            ASSERT(sizeof(my_MostDerived) == BLOCK_SIZE);
            const int CHUNK_SIZE = 1;
            Obj mX(BLOCK_SIZE,
                   bsls::BlockGrowth::BSLS_CONSTANT,
                   CHUNK_SIZE,
                   &a);

            if (verbose) cout << "\t'deleteObject(my_MostDerived *)'" << endl;

            my_MostDerived *pMost = (my_MostDerived *) mX.allocate();
            const my_MostDerived *pMostCONST = pMost;

            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
            new(pMost) my_MostDerived;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObject(pMostCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\t'deleteObject(my_LeftBase *)'" << endl;

            pMost = (my_MostDerived *) mX.allocate();

            new(pMost) my_MostDerived;
            const my_LeftBase *pLeftCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObject(pLeftCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\t'deleteObject(my_RightBase *)'" << endl;

            pMost = (my_MostDerived *) mX.allocate();

            new(pMost) my_MostDerived;
            const my_RightBase *pRightCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObject(pRightCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\t'deleteObject(my_VirtualBase *)'" << endl;

            pMost = (my_MostDerived *) mX.allocate();

            new(pMost) my_MostDerived;
            const my_VirtualBase *pVirtualCONST = pMost;
            ASSERT(1 == mostDerivedObjectCount);
            ASSERT(1 == rightBaseObjectCount);
            ASSERT(1 == leftBaseObjectCount);
            ASSERT(1 == virtualBaseObjectCount);

            mX.deleteObject(pVirtualCONST);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);

            if (verbose) cout << "\tWith a null pointer." << endl;

            pMost = 0;
            mX.deleteObject(pMost);
            ASSERT(0 == mostDerivedObjectCount);
            ASSERT(0 == rightBaseObjectCount);
            ASSERT(0 == leftBaseObjectCount);
            ASSERT(0 == virtualBaseObjectCount);
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // EXCEPTION SAFETY OF OPERATOR NEW TEST
        //
        // Concerns:
        //   That when the overloaded 'operator new' is invoked and the
        //   constructor of the new object throws an exception, the overloaded
        //   'delete' operator is invoked automatically to deallocate the
        //   memory.
        //
        // Plan:
        //   Invoke 'operator new' for an object of a class that throws an
        //   exception from its constructor.  Catch the exception and verify
        //   that deallocation was performed automatically.
        //
        //   QoI: Asserted precondition violations are detected when enabled.
        //
        // Testing:
        //   void operator delete(void *address, bdlma::Pool& pool);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EXCEPTION SAFETY OF OPERATOR NEW TEST" << endl
                          << "=====================================" << endl;

        bslma::TestAllocator a(veryVeryVerbose);
        const bslma::TestAllocator& A = a;
        {
            const int BLOCK_SIZE = sizeof(my_ClassThatMayThrowFromConstructor);
            const int CHUNK_SIZE = 1;
            Obj mX(BLOCK_SIZE,
                   bsls::BlockGrowth::BSLS_CONSTANT,
                   CHUNK_SIZE,
                   &a);

            my_ClassThatMayThrowFromConstructor *p = 0;
#ifdef BDE_BUILD_TARGET_EXC
            try
#endif
            {
                p = new (mX) my_ClassThatMayThrowFromConstructor;
                p->~my_ClassThatMayThrowFromConstructor();
                mX.deallocate(p);
                p = 0;
            }
#ifdef BDE_BUILD_TARGET_EXC
            catch(int n)
            {
                if (verbose) cout << "\nCaught exception." << endl;
                ASSERT(13 == n);
            }
#endif
            ASSERT(!p);
            mX.allocate();
            ASSERT(1 == A.numAllocations());
                // By observing that the number of allocations remains at one,
                // we confirm that the memory obtained from the pool by
                // operator 'new' has been returned to the pool on exception.
                // Had it not been returned, the call to 'allocate' would have
                // required another allocation from the test allocator.
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX(8);
            char *p = (char *)mX.allocate();

            if (veryVerbose) cout << "\t'operator delete'" << endl;
            {
                ASSERT_SAFE_PASS(operator delete(p, mX));

                ASSERT_SAFE_FAIL(operator delete(0, mX));
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // OPERATOR NEW TEST
        //
        // Concerns:
        //   That the correct constructor is called based on the type of the
        //   object allocated via the overloaded 'new' operator.
        //
        // Plan:
        //   Invoke 'operator new' for various types.  Verify that the
        //   correct constructor is called in each case.
        //
        //   QoI: Asserted precondition violations are detected when enabled.
        //
        // Testing:
        //   void *operator new(bsl::size_t size, bdlma::Pool& pool);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "OPERATOR NEW TEST" << endl
                                  << "=================" << endl;

        bslma::TestAllocator a(veryVeryVerbose);

        const int BLOCK_SIZE = sizeof(my_Class1);
        ASSERT(sizeof(my_Class2)==BLOCK_SIZE);
        const int CHUNK_SIZE = 10;
        Obj mX(BLOCK_SIZE, bsls::BlockGrowth::BSLS_CONSTANT, CHUNK_SIZE, &a);

        my_ClassCode=0;
        my_Class1 *c1 = new (mX) my_Class1;
        ASSERT(c1);  ASSERT(1 == my_ClassCode);
        my_Class2 *c2 = new (mX) my_Class2;
        ASSERT(c2);  ASSERT(3 == my_ClassCode);

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX(8);

            if (veryVerbose) cout << "\t'operator new'" << endl;
            {
                ASSERT_SAFE_PASS(operator new( 1, mX));
                ASSERT_SAFE_PASS(operator new( 7, mX));
                ASSERT_SAFE_PASS(operator new( 8, mX));

                ASSERT_SAFE_FAIL(operator new( 9, mX));
                ASSERT_SAFE_FAIL(operator new(16, mX));
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // DTOR AND RELEASE TEST
        //
        // Concerns:
        //   That both the destructor and the 'release' method free all memory
        //   used by the pool.
        //
        // Plan:
        //   Initialize two pools with varying block sizes, growth strategies,
        //   and 'maxBlocksPerChunk', and supply each with its own test
        //   allocator.  Invoke 'allocate' repeatedly.  Invoke 'release' on one
        //   pool, and allow the other pool to go out of scope.  Verify that
        //   both allocators indicate all memory has been released by the
        //   pools.
        //
        // Testing:
        //   ~Pool();
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DTOR AND RELEASE TEST" << endl
                                  << "=====================" << endl;

        if (verbose) cout << "\nTesting destructor and 'release'." << endl;

        Strategy GEO = bsls::BlockGrowth::BSLS_GEOMETRIC;
        Strategy CON = bsls::BlockGrowth::BSLS_CONSTANT;

        struct {
            int      d_line;
            int      d_blockSize;
            int      d_numBlocks;
            Strategy d_strategy;
        } DATA[] = {
            //line    block
            //no.     size      max chunk size    growth strategy
            //----    ------    ----------------  ---------------
            { L_,       1,                    5,            CON },
            { L_,       5,                   10,            CON },
            { L_,      12,                    1,            GEO },
            { L_,      24,                    5,            GEO },
            { L_,      32,     k_MAX_CHUNK_SIZE,            GEO }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int NUM_REQUESTS = 100;
        bslma::TestAllocator taX(veryVeryVerbose);
        const bslma::TestAllocator& TAX = taX;
        bslma::TestAllocator taY(veryVeryVerbose);
        const bslma::TestAllocator& TAY = taY;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int      LINE       = DATA[ti].d_line;
            const int      BLOCK_SIZE = DATA[ti].d_blockSize;
            const int      CHUNK_SIZE = DATA[ti].d_numBlocks;
            const Strategy STRATEGY   = DATA[ti].d_strategy;
            {
                Obj mX(BLOCK_SIZE, STRATEGY, CHUNK_SIZE, &taX);
                Obj mY(BLOCK_SIZE, STRATEGY, CHUNK_SIZE, &taY);

                for (int ai = 0; ai < NUM_REQUESTS; ++ai) {
                    mX.allocate();
                    mY.allocate();
                }

                if (veryVerbose) { T_ P_(TAX.numBytesInUse()); }
                mX.release();
                if (veryVerbose) { T_ P(TAX.numBytesInUse()); }

                if (veryVerbose) { T_ P_(TAY.numBytesInUse()); }
                // Let 'mY' go out of scope.
            }
            if (veryVerbose) { T_ P(TAY.numBytesInUse()); }

            LOOP2_ASSERT(LINE, ti, 0 == TAX.numBytesInUse());
            LOOP2_ASSERT(LINE, ti, 0 == TAY.numBytesInUse());
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // DEALLOCATE TEST
        //
        // Concerns:
        //   That memory returned to a pool by 'deallocate' is "pooled" for
        //   future allocations.
        //
        // Plan:
        //   Initialize a pool with varying block sizes and
        //   'maxBlocksPerChunk'.  Invoke 'allocate' repeatedly and store the
        //   returned memory addresses in an array.  Then deallocate the
        //   allocated memory in reverse order.  Finally, allocate memory again
        //   and verify that the returned memory addresses are in the same
        //   order as those stored in the array.  Also verify that no
        //   additional memory requests to the allocator occurred.
        //
        //   QoI: Asserted precondition violations are detected when enabled.
        //
        // Testing:
        //   void deallocate(address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DEALLOCATE TEST" << endl
                                  << "===============" << endl;

        if (verbose) cout << "\nTesting 'deallocate'." << endl;

        Strategy GEO = bsls::BlockGrowth::BSLS_GEOMETRIC;
        Strategy CON = bsls::BlockGrowth::BSLS_CONSTANT;

        const int NUM_REQUESTS = 100;

        // Note that after the NUM_REQUESTS allocations are performed, there
        // will be un-allocated blocks that will be used before the free list;
        // 'd_availBlocks' provides the expected number of these un-allocated
        // blocks.

        struct {
            int      d_line;
            int      d_blockSize;
            int      d_numBlocks;
            Strategy d_strategy;
            int      d_availBlocks;
        } DATA[] = {
            //    block                              avail
            //LN  size    max chunk size   strategy  blocks
            //--  -----  ----------------  --------  ------
            { L_,     1,                5,      CON,      0 },
            { L_,     5,               10,      CON,      0 },
            { L_,    12,                1,      GEO,      0 },
            { L_,    24,                5,      GEO,      2 },
            { L_,    32, k_MAX_CHUNK_SIZE,      GEO,     27 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        void *p[NUM_REQUESTS];
        bslma::TestAllocator ta(veryVeryVerbose);
        const bslma::TestAllocator& TA = ta;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int      LINE       = DATA[ti].d_line;
            const int      BLOCK_SIZE = DATA[ti].d_blockSize;
            const int      CHUNK_SIZE = DATA[ti].d_numBlocks;
            const Strategy STRATEGY   = DATA[ti].d_strategy;
            const int      AVAIL      = DATA[ti].d_availBlocks;

            Obj mX(BLOCK_SIZE, STRATEGY, CHUNK_SIZE, &ta);

            for (int ai = 0; ai < NUM_REQUESTS; ++ai) {
                p[ai] = mX.allocate();
            }

            bsls::Types::Int64 numAllocations = TA.numAllocations();

            for (int dd = NUM_REQUESTS - 1; dd >= 0; --dd) {
                mX.deallocate(p[dd]);
            }

            if (veryVerbose) { T_ P_(CHUNK_SIZE); P(numAllocations); }

            // Ensure memory was deallocated in the expected sequence.
            for (int aj = 0; aj < AVAIL; ++aj) {
                mX.allocate();
            }
            for (int aj = AVAIL; aj < NUM_REQUESTS; ++aj) {
                LOOP3_ASSERT(LINE, ti, aj, p[aj - AVAIL] == mX.allocate());
            }

            // Ensure no additional memory requests to the allocator occurred.
            LOOP2_ASSERT(LINE, ti, TA.numAllocations() == numAllocations);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX(8);
            char *p = (char *)mX.allocate();

            if (veryVerbose) cout << "\t'deallocate(0)'" << endl;
            {
                ASSERT_SAFE_PASS(mX.deallocate(p));

                ASSERT_SAFE_FAIL(mX.deallocate(0));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // DEFAULT GROWTH TEST
        //
        // Concerns:
        //   1. That the pool replenishes with the proper growth strategy when
        //      'growthStrategy' is not supplied.
        //
        //   2. That the pool replenishes with the proper maximum blocks per
        //      chunk when 'maxBlocksPerChunk' is not supplied.
        //
        //   3. That the default allocator is used to supply memory when
        //      'basicAllocator' is not supplied.
        //
        //   QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //   Initialize a pool with a chosen block size, default
        //   'maxBlocksPerChunk', and a test allocator.  Initialize a second
        //   pool as a reference with the same block size,
        //   'k_INITIAL_NUM_BLOCKS' for 'maxBlocksPerChunk', and a second test
        //   allocator.  Invoke 'allocate' repeatedly on both pools so that the
        //   pools deplete and replenish until the pools stop growing in size.
        //   Verify that for each replenishment the allocator for the pool
        //   under test contains the same number of memory requests and the
        //   same request size as the allocator for the reference pool.
        //
        // Testing:
        //   Pool(bs, basicAllocator = 0);
        //   Pool(bs, gs, basicAllocator = 0);
        //   void *allocate();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DEFAULT GROWTH TEST" << endl
                                  << "===================" << endl;

        const int BLOCK_SIZE = 4;

        if (verbose) cout << "\nTesting default growth strategy." << endl;
        {
            bslma::TestAllocator taX(veryVeryVerbose);
            const bslma::TestAllocator& TAX = taX;
            Obj mX(BLOCK_SIZE, &taX);

            bslma::TestAllocator taY(veryVeryVerbose);
            const bslma::TestAllocator& TAY = taY;
            Obj mY(BLOCK_SIZE,
                   bsls::BlockGrowth::BSLS_GEOMETRIC,
                   k_MAX_CHUNK_SIZE,
                   &taY);

            int exceedMaxNumBlocksCount = 0;
            int chunkSize = k_INITIAL_NUM_BLOCKS;
            while (1) {
                // Allocate until current pool is depleted.
                for (int oi = 0; oi < myAbs(chunkSize); ++oi) {
                    mX.allocate();
                    mY.allocate();
                }

                bsls::Types::Int64 numAllocations = TAX.numAllocations();
                bsls::Types::Int64 numBytes = TAX.lastAllocatedNumBytes();
                if (veryVerbose) { T_ P_(numAllocations); T_ P(numBytes); }
                LOOP_ASSERT(chunkSize,
                            TAY.numAllocations() == numAllocations);
                LOOP_ASSERT(chunkSize,
                            (int)TAY.lastAllocatedNumBytes() == numBytes);

                int newChunkSize = growNumBlocks(chunkSize, k_MAX_CHUNK_SIZE);
                if (newChunkSize == chunkSize) ++exceedMaxNumBlocksCount;
                if (2 < exceedMaxNumBlocksCount) break;
                chunkSize = newChunkSize;
            }
        }

        if (verbose) cout << "\nTesting default max blocks per chunk." << endl;
        {
            bslma::TestAllocator taX(veryVeryVerbose);
            const bslma::TestAllocator& TAX = taX;
            Obj mX(BLOCK_SIZE, bsls::BlockGrowth::BSLS_GEOMETRIC, &taX);

            bslma::TestAllocator taY(veryVeryVerbose);
            const bslma::TestAllocator& TAY = taY;
            Obj mY(BLOCK_SIZE,
                   bsls::BlockGrowth::BSLS_GEOMETRIC,
                   k_MAX_CHUNK_SIZE,
                   &taY);

            int exceedMaxNumBlocksCount = 0;
            int chunkSize = k_INITIAL_NUM_BLOCKS;
            while (1) {
                // Allocate until current pool is depleted.
                for (int oi = 0; oi < myAbs(chunkSize); ++oi) {
                    mX.allocate();
                    mY.allocate();
                }

                bsls::Types::Int64 numAllocations = TAX.numAllocations();
                bsls::Types::Int64 numBytes = TAX.lastAllocatedNumBytes();
                if (veryVerbose) { T_ P_(numAllocations); T_ P(numBytes); }
                LOOP_ASSERT(chunkSize, TAY.numAllocations() == numAllocations);
                LOOP_ASSERT(chunkSize, (int)TAY.lastAllocatedNumBytes()
                                                                  == numBytes);

                int newChunkSize = growNumBlocks(chunkSize, k_MAX_CHUNK_SIZE);
                if (newChunkSize == chunkSize) ++exceedMaxNumBlocksCount;
                if (2 < exceedMaxNumBlocksCount) break;
                chunkSize = newChunkSize;
            }
        }

        if (verbose) cout << "\nTesting default allocator." << endl;
        {
            // Make sure memory is allocated from the default allocator.
            bslma::TestAllocator ta;
            bslma::DefaultAllocatorGuard dag(&ta);
            ASSERT(0 == ta.numBytesInUse());

            Obj mX(BLOCK_SIZE);
            ASSERT(0 == ta.numBytesInUse());

            mX.allocate();
            ASSERT(0 != ta.numBytesInUse());
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\tTwo argument constructor." << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1));

                ASSERT_SAFE_FAIL(Obj( 0));
                ASSERT_SAFE_FAIL(Obj(-1));
            }

            if (veryVerbose) cout << "\tThree argument constructor." << endl;
            {
                Strategy CON = bsls::BlockGrowth::BSLS_CONSTANT;

                ASSERT_SAFE_PASS(Obj( 1, CON));

                ASSERT_SAFE_FAIL(Obj( 0, CON));
                ASSERT_SAFE_FAIL(Obj(-1, CON));
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // 'growthStrategy' TEST
        //
        // Concerns:
        //   1. That when constant growth is used, the pool replenishes with
        //      the specified 'maxBlocksPerChunk'.
        //
        //   2. That when geometric growth is used, the pool replenishes with
        //      a geometrically increasing chunk size up to the specified
        //      'maxBlocksPerChunk'.
        //
        //   QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //   Using the table driven technique, specify the block size, growth
        //   strategy, and 'maxBlocksPerChunk'.  Create a pool with the
        //   parameters from the table, along with a test allocator.  Invoke
        //   'allocate' repeatedly so that the pool depletes and replenishes.
        //   Verify that for each replenishment the pool requests memory of the
        //   expected size.
        //
        // Testing:
        //   Pool(bs, gs, maxBlocksPerChunk, basicAllocator);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'growthStrategy' TEST" << endl
                                  << "=====================" << endl;

        if (verbose) cout << "\nTesting constructor and 'allocate' w/varying "
                             "'maxBlocksPerChunk'." << endl;

        Strategy GEO = bsls::BlockGrowth::BSLS_GEOMETRIC;
        Strategy CON = bsls::BlockGrowth::BSLS_CONSTANT;

        static const struct {
            int      d_line;
            int      d_blockSize;
            Strategy d_strategy;
            int      d_maxBlocksPerChunk;
        } DATA[] = {
            // LINE     BLOCKSIZE    STRATEGY     MAXBLOCKS
            // ----     ---------    --------     ---------

            // Constant growth
            {  L_,              1,       CON,           16 },
            {  L_,              1,       CON,           30 },
            {  L_,              1,       CON,           32 },
            {  L_,              1,       CON,           48 },
            {  L_,              1,       CON,           64 },

            {  L_,              2,       CON,           16 },
            {  L_,              2,       CON,           30 },
            {  L_,              2,       CON,           32 },
            {  L_,              2,       CON,           48 },
            {  L_,              2,       CON,           64 },

            {  L_,              4,       CON,           16 },
            {  L_,              4,       CON,           30 },
            {  L_,              4,       CON,           32 },
            {  L_,              4,       CON,           48 },
            {  L_,              4,       CON,           64 },

            // Geometric growth
            {  L_,              1,       GEO,           16 },
            {  L_,              1,       GEO,           31 },
            {  L_,              1,       GEO,           32 },
            {  L_,              1,       GEO,           33 },
            {  L_,              1,       GEO,           48 },

            {  L_,              2,       GEO,           16 },
            {  L_,              2,       GEO,           31 },
            {  L_,              2,       GEO,           32 },
            {  L_,              2,       GEO,           33 },
            {  L_,              2,       GEO,           48 },

            {  L_,              4,       GEO,           16 },
            {  L_,              4,       GEO,           31 },
            {  L_,              4,       GEO,           32 },
            {  L_,              4,       GEO,           33 },
            {  L_,              4,       GEO,           48 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int      LINE       = DATA[ti].d_line;
            const int      BLOCK_SIZE = DATA[ti].d_blockSize;
            const Strategy STRATEGY   = DATA[ti].d_strategy;
            const int      MAXBLOCKS  = DATA[ti].d_maxBlocksPerChunk;

            if (veryVerbose) {
                T_ P_(LINE) P_(BLOCK_SIZE)
                if (STRATEGY == GEO) {
                    cout << "STRATEGY = GEOMETRIC_GROWTH, ";
                }
                else {
                    cout << "STRATEGY = CONSTANT_GROWTH, ";
                }
                P(MAXBLOCKS)
            }

            bslma::TestAllocator testAllocator(veryVeryVerbose);
            const bslma::TestAllocator& TA = testAllocator;

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                Obj mX(BLOCK_SIZE, STRATEGY, MAXBLOCKS, &testAllocator);

                bsls::Types::Int64 numAllocations = 0;

                // If geometric, grow till constant size first.
                if (GEO == STRATEGY) {
                    int ri = k_INITIAL_CHUNK_SIZE;

                    while (ri < MAXBLOCKS) {
                        numAllocations = TA.numAllocations();

                        // Use all memory blocks from current chunk.
                        char *lastAddr = 0;
                        for (int j = 0; j < ri; ++j) {
                            char *addr = (char *)mX.allocate();
                            if (j) {
                                int EXP  = poolBlockSize(BLOCK_SIZE);
                                int size = static_cast<int>(addr - lastAddr);
                                LOOP2_ASSERT(EXP, size, EXP == size);
                            }
                            lastAddr = addr;
                        }

                        LOOP_ASSERT(ri, TA.numAllocations()
                                                        == numAllocations + 1);
                        ri <<= 1;
                    }
                }

                // Test for constant growth.

                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    numAllocations = TA.numAllocations();

                    // Use all memory blocks from current chunk.
                    char *lastAddr = 0;
                    for (int j = 0; j < MAXBLOCKS; ++j) {
                        char *addr = (char *)mX.allocate();
                        if (j) {
                            bsls::Types::Int64 EXP  =
                                                     poolBlockSize(BLOCK_SIZE);
                            bsls::Types::Int64 size = addr - lastAddr;
                            LOOP2_ASSERT(EXP, size, EXP == size);
                        }
                        lastAddr = addr;
                    }

                    LOOP_ASSERT(ri, TA.numAllocations() == numAllocations + 1);
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\tFour argument constructor." << endl;
            {
                ASSERT_SAFE_PASS(Obj( 1, CON,  1));

                ASSERT_SAFE_FAIL(Obj( 0, CON,  1));
                ASSERT_SAFE_FAIL(Obj(-1, CON,  1));

                ASSERT_SAFE_FAIL(Obj( 1, CON,  0));
                ASSERT_SAFE_FAIL(Obj( 1, CON, -1));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ALLOCATE TEST
        //
        // Concerns:
        //   That 'allocate' returns memory blocks having the 'blockSize'
        //   specified at construction.
        //
        // Plan:
        //   Initialize a pool with a fixed 'maxBlocksPerChunk', constant
        //   growth strategy, and varying block sizes.  Invoke 'allocate'
        //   repeatedly and verify that the difference between the memory
        //   addresses of two consecutive requests is equal to the expected
        //   block size (taking alignment considerations into account).
        //
        // Testing:
        //   int blockSize() const;
        //   'allocate' returns memory of the correct block size.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "ALLOCATE TEST" << endl
                                  << "=============" << endl;

        if (verbose) cout << "\nTesting 'allocate' with varying block sizes."
                          << endl;

        const int DATA[] = { 1, 2, 5, 6, 12, 24, 32 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;
        const int NUM_BLOCKS = 5;
        bslma::TestAllocator testAllocator(veryVeryVerbose);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                const int BLOCK_SIZE = DATA[ti];
                Obj mX(BLOCK_SIZE,
                       bsls::BlockGrowth::BSLS_CONSTANT,
                       NUM_BLOCKS,
                       &testAllocator);
                const Obj& X = mX;

                ASSERT(BLOCK_SIZE == X.blockSize());

                char *lastP = 0;
                for (int oi = 0; oi < NUM_BLOCKS; ++oi) {
                    char *p = (char *) mX.allocate();
                    if (oi) {
                        ASSERT(lastP);

                        bsls::Types::Int64       size = p - lastP;
                        const bsls::Types::Int64 EXP  =
                                                     poolBlockSize(BLOCK_SIZE);

                        if (veryVerbose) { T_ P_(size) T_ P(EXP) }
                        LOOP2_ASSERT(ti, oi, EXP == size);
                    }
                    lastP = p;
                }
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // HELPER FUNCTIONS
        //
        // Concerns:
        //   That the static functions 'blockSize', 'poolBlockSize', and
        //   'growNumBlocks' are implemented consistently with the
        //   implementation of the pool itself.
        //
        // Plan:
        //   To test 'blockSize', create a 'bdlma::InfrequentDeleteBlockList'
        //   object initialized with a test allocator.  Invoke both the
        //   'blockSize' function and the 'bdlma::InfrequentDeleteBlockList's
        //   'allocate' method with varying memory sizes, and verify that the
        //   sizes returned by 'blockSize' are equal to the sizes recorded by
        //   the allocator.
        //
        //   To test 'poolBlockSize', invoke the function with varying sizes,
        //   and verify that the returned value is equal to the difference
        //   between the memory addresses returned by two consecutive
        //   'allocate' requests from a suitably configured pool (i.e., the
        //   difference is the size of each returned memory block).
        //
        //   To test 'growNumBlocks', iterate over a set of table-based test
        //   vectors.  Invoke 'growNumBlocks' with the parameters specified in
        //   each test vector and verify that the function returns the expected
        //   value.
        //
        // Testing:
        //   int blockSize(numBytes);
        //   int poolBlockSize(size);
        //   int growNumBlocks(numBlocks, maxNumBlocks);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "HELPER FUNCTIONS" << endl
                                  << "================" << endl;

        if (verbose) cout << "\nTesting 'blockSize'." << endl;
        {
            const int DATA[] = { 0, 1, 5, 12, 24, 64, 1000 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            bslma::TestAllocator a(veryVeryVerbose);
            bdlma::InfrequentDeleteBlockList bl(&a);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];
                bsls::Types::Int64 blkSize = blockSize(SIZE);
                bl.allocate(SIZE);

                const bsls::Types::Int64 EXP = a.lastAllocatedNumBytes();

                if (veryVerbose) {T_ P_(SIZE); P_(blkSize); P(EXP);}
                LOOP_ASSERT(i, EXP == blkSize);
            }
        }

        if (verbose) cout << "\nTesting 'poolBlockSize'." << endl;
        {
            const int DATA[] = { 1, 2, 5, 6, 12, 24, 32 };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int SIZE = DATA[ti];
                Obj mX(SIZE, bsls::BlockGrowth::BSLS_CONSTANT, 2);
                char *p = (char *)mX.allocate();
                char *q = (char *)mX.allocate();
                bsls::Types::Int64 EXP = q - p;
                bsls::Types::Int64 blockSize = poolBlockSize(SIZE);
                if (veryVerbose) { T_ P_(blockSize); P(EXP); }
                LOOP_ASSERT(ti, EXP == blockSize);
            }
        }

        if (verbose) cout << "\nTesting 'growNumBlocks'." << endl;
        {
            struct {
                int d_line;
                int d_numBlocks;
                int d_maxNumBlocks;
                int d_exp;
            } DATA[] = {
                //line               Maximum         Expected
                //no.     # Blocks   # Blocks        Value
                //----    --------   --------        --------
                { L_,       1,          1,             1      },
                { L_,       1,          2,             2      },

                { L_,       2,          2,             2      },
                { L_,       2,          3,             3      },
                { L_,       2,          4,             4      },

                { L_,       4,          4,             4      },
                { L_,       4,          5,             5      },
                { L_,       4,          6,             6      },
                { L_,       4,          7,             7      },
                { L_,       4,          8,             8      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE           = DATA[ti].d_line;
                const int NUM_BLOCKS     = DATA[ti].d_numBlocks;
                const int MAX_NUM_BLOCKS = DATA[ti].d_maxNumBlocks;
                const int EXP            = DATA[ti].d_exp;

                int numBlocks = growNumBlocks(NUM_BLOCKS, MAX_NUM_BLOCKS);

                if (veryVerbose) { T_ P_(numBlocks); T_ P(EXP); }
                LOOP2_ASSERT(LINE, ti, EXP == numBlocks);
            }
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

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
