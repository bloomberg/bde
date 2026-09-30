// bdlma_concurrentpool.t.cpp                                         -*-C++-*-
#include <bdlma_concurrentpool.h>

#include <bdlma_infrequentdeleteblocklist.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>

#include <bslmt_barrier.h>
#include <bslmt_threadgroup.h>
#include <bslmt_threadutil.h>
#include <bslmt_timedcompletionguard.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_alignmentutil.h>
#include <bsls_blockgrowth.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_platform.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>     // `atoi`
#include <bsl_format.h>
#include <bsl_iostream.h>
#include <bsl_new.h>         // `bad_alloc`
#include <bsl_set.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#ifdef BSLS_PLATFORM_CMP_CLANG
#pragma GCC diagnostic ignored "-Wunused-private-field"
#endif
#endif

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The component under test implements a concurrent memory pool.  The primary
// manipulators are `allocate` and `deallocate`.  The basic accessors are the
// methods for obtaining the allocator (`allocator`) and the size of the
// provided memory blocks (`blockSize`).  The basic functionality of the pool
// will be verified initially with a single thread of execution, and then
// concurrency concerns will be addressed.  Effort is made to use only the
// primary manipulators and basic accessors whenever possible, thus making
// every test case independent.
//
// Global Concerns:
//  - The test driver is robust w.r.t. reuse in other, similar components.
//  - ACCESSOR methods are declared `const`.
//  - CREATOR & MANIPULATOR pointer/reference parameters are declared `const`.
//  - No memory is ever allocated from the global allocator.
//  - Any allocated memory is always from the object allocator.
//  - Injected exceptions are safely propagated during memory allocation.
//  - Precondition violations are detected in appropriate build modes.
//
// Global Assumptions:
//  - All explicit memory allocations are presumed to use the global, default,
//    or object allocator.
//  - ACCESSOR methods are `const` thread-safe.
//-----------------------------------------------------------------------------
// [ 2] bdlma::ConcurrentPool(blockSize, basicAllocator);
// [ 2] ~bdlma::ConcurrentPool();
// [ 3] void *allocate();
// [ 3] void deallocate(void *address);
// [ 4] void deleteObject(const TYPE *object);
// [ 4] void deleteObjectRaw(const TYPE *object);
// [ 6] void release();
// [ 7] void reserveCapacity(int numBlocks);
// [ 2] bsls::Types::size_type blockSize() const;
// [ 2] bslma::Allocator *allocator() const;
// [ 8] bdlma::ConcurrentPool(blockSize, strategy, basicAllocator);
// [ 8] bdlma::ConcurrentPool(blockSize, strategy, maxBlocksPerChunk, bA);
// [ 5] void *operator new(bsl::size_t size, bdlma::ConcurrentPool& pool);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
// [ 9] CONCURRENCY TEST

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

#define Q   BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P   BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_  BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_  BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_  BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)

// ============================================================================
//                   GLOBAL TYPEDEFS, CONSTANTS, AND VARIABLES
// ----------------------------------------------------------------------------

typedef bdlma::ConcurrentPool Obj;

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;
static int veryVeryVeryVerbose;

int numLeftChildren   = 0;
int numMiddleChildren = 0;
int numRightChildren  = 0;
int numMostDerived    = 0;

struct LeftChild {
    int d_li;
    LeftChild()           { ++numLeftChildren; }
    virtual ~LeftChild()  { --numLeftChildren; }
};

struct MiddleChild {    // non-polymorphic middle child
    int d_mi;
    MiddleChild()         { ++numMiddleChildren; }
    ~MiddleChild()        { --numMiddleChildren; }
};

struct RightChild {
    int d_ri;
    RightChild()          { ++numRightChildren; }
    virtual ~RightChild() { --numRightChildren; }
};

struct MostDerived : LeftChild, MiddleChild, RightChild {
    int d_md;
    MostDerived()                        { ++numMostDerived; }
    ~MostDerived() BSLS_KEYWORD_OVERRIDE { --numMostDerived; }
};

//=============================================================================
//                               USAGE EXAMPLE
//-----------------------------------------------------------------------------
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FD01  // Function doc. implied by expository text
// BDE_VERIFY pragma: -TY02  // Single letter type parameters

///Usage
///-----
// A `bdlma::ConcurrentPool` can be used by node-based containers (such as
// lists, trees, and hash tables that hold multiple elements of uniform size)
// for efficient memory allocation of new elements.  The following container
// class, `my_PooledArray`, stores templatized values "out-of-place" as nodes
// in a `vector` of pointers.  Since the size of each node is fixed and known
// *a priori*, the class uses a `bdlma::ConcurrentPool` to allocate memory for
// the nodes to improve memory allocation efficiency:
// ```
    // my_poolarray.h

    /// This class implements a container that stores `double` values
    /// out-of-place.
    template <class T>
    class my_PooledArray {

        // DATA
        bsl::vector<T *>      d_array_p;  // array of pooled elements
        bdlma::ConcurrentPool d_pool;     // memory manager for array elements

      private:
        // Not implemented:
        my_PooledArray(const my_PooledArray&);

      public:
        // CREATORS

        /// Create a pooled array that stores the parameterized values
        /// "out-of-place".  Optionally specify a `basicAllocator` used to
        /// supply memory.  If `basicAllocator` is 0, the currently
        /// installed default allocator is used.
        explicit my_PooledArray(bslma::Allocator *basicAllocator = 0);

        /// Destroy this array and all elements held by it.
        ~my_PooledArray();

        // MANIPULATORS

        /// Append the specified `value` to this array.
        void append(const T &value);

        /// Remove all elements from this array.
        void removeAll();

        // ACCESSORS

        /// Return the number of elements in this array.
        int length() const;

        /// Return a reference to the non-modifiable value at the specified
        /// `index` in this array.  The behavior is undefined unless
        /// `0 <= index < length()`.
        const T& operator[](int index) const;
    };
// ```
    // CREATORS
    template <class T>
    my_PooledArray<T>::my_PooledArray(bslma::Allocator *basicAllocator)
    : d_array_p(basicAllocator)
    , d_pool(sizeof(T), basicAllocator)
    {
    }
// ```
// Since all memory is managed by `d_pool`, we do not have to explicitly invoke
// `deleteObject` to reclaim outstanding memory.  The destructor of the pool
// will automatically deallocate all array elements:
// ```
    template <class T>
    my_PooledArray<T>::~my_PooledArray()
    {
        // Elements are automatically deallocated when `d_pool` is destroyed.
    }
// ```
    // MANIPULATORS
// ```
// Note that the overloaded "placement" `new` is used to allocate new nodes:
// ```
    template <class T>
    void my_PooledArray<T>::append(const T& value)
    {
        T *tmp = new (d_pool) T(value);
        d_array_p.push_back(tmp);
    }
// ```
// In the `removeAll` method, all elements are deallocated by invoking the
// pool's `release` method.  This technique implies significant performance
// gain when the array contains many elements:
// ```
    template <class T>
    inline
    void my_PooledArray<T>::removeAll()
    {
        d_array_p.clear();
        d_pool.release();
    }

    // ACCESSORS
    template <class T>
    inline
    int my_PooledArray<T>::length() const
    {
        return static_cast<int>(d_array_p.size());
    }

    template <class T>
    inline
    const T& my_PooledArray<T>::operator[](int index) const
    {
        ASSERT(0 <= index);
        ASSERT(index < length());

        return *d_array_p[index];
    }
// ```

// BDE_VERIFY pragma: pop

//=============================================================================
//                CONCRETE OBJECTS FOR TESTING `deleteObject`
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
    my_LeftBase()                        { leftBaseObjectCount = 1; }
    ~my_LeftBase() BSLS_KEYWORD_OVERRIDE { leftBaseObjectCount = 0; }
};

class my_RightBase : virtual public my_VirtualBase {
    int x;
public:
    my_RightBase()                        { rightBaseObjectCount = 1; }
    ~my_RightBase() BSLS_KEYWORD_OVERRIDE { rightBaseObjectCount = 0; }
};

class my_MostDerived : public my_LeftBase, public my_RightBase {
    int x;
public:
    my_MostDerived()                        { mostDerivedObjectCount = 1; }
    ~my_MostDerived() BSLS_KEYWORD_OVERRIDE { mostDerivedObjectCount = 0; }
};

//=============================================================================
//                                HELPER FUNCTIONS
//-----------------------------------------------------------------------------

// Return the cache size, in blocks, for the specified `blockSize`,
// `chunkSize`, and `initialAllocationSize`.
unsigned computeCacheSize(unsigned blockSize,
                          unsigned chunkSize,
                          unsigned initialAllocationSize)
{
    // account for use of a header in `InfrequentDeleteBlockList`
    initialAllocationSize -= bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

    unsigned internalBlockSize =
                      (blockSize + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT - 1)
                    / bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT
                    * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

    unsigned cacheSize = initialAllocationSize / internalBlockSize;
    cacheSize += chunkSize - 2 - 1;
    cacheSize = cacheSize * chunkSize / (chunkSize - 1);

    return cacheSize;
}

//=============================================================================
//                      HELPER FUNCTION FOR CONCURRENCY TEST
//-----------------------------------------------------------------------------

enum {
    k_OBJECT_SIZE       = sizeof(bsls::Types::Uint64),
    k_OBJECT_CACHE_SIZE = 8,
    k_NUM_ALLOCATIONS   = 1000,
    k_NUM_ITERATIONS    = 10,
    k_NUM_THREADS       = 4
};

bslmt::Barrier barrier(k_NUM_THREADS);

extern "C"
void *workerThread(void *arg) {
    Obj& mX = *static_cast<Obj *>(arg);

    ASSERT(k_OBJECT_SIZE == mX.blockSize());

    bsls::Types::Uint64  selfId = bslmt::ThreadUtil::selfIdAsUint64();
    bsls::Types::Uint64 *cache[k_OBJECT_CACHE_SIZE];

    for (int i = 0; i < k_NUM_ITERATIONS; ++i) {
        barrier.wait();

        for (int j = 0; j < k_OBJECT_CACHE_SIZE; ++j) {
            cache[j] = static_cast<bsls::Types::Uint64 *>(mX.allocate());
            *cache[j] = selfId;
        }
        for (int j = k_OBJECT_CACHE_SIZE; j < k_NUM_ALLOCATIONS; ++j) {
            int jj = j % k_OBJECT_CACHE_SIZE;

            ASSERTV(i, j, selfId, *cache[jj], selfId == *cache[jj]);
            *cache[jj] = 0;
            mX.deallocate(cache[jj]);

            cache[jj] = static_cast<bsls::Types::Uint64 *>(mX.allocate());
            *cache[jj] = selfId;
        }

        mX.reserveCapacity(k_OBJECT_CACHE_SIZE);

        for (int j = 0; j < k_OBJECT_CACHE_SIZE; ++j) {
            ASSERTV(i, j, selfId, *cache[j], selfId == *cache[j]);
            mX.deallocate(cache[j]);
        }

        barrier.wait();

        // use Fibonacci Hashing to bucket the `selfId`
        if (0 == (selfId * 11400714819323198485ull) >> 63) {
            mX.release();
            mX.reserveCapacity(k_OBJECT_CACHE_SIZE);
        }
        else {
            mX.reserveCapacity(k_OBJECT_CACHE_SIZE);
            mX.release();
        }
    }
    return 0;
}

//=============================================================================
//                                MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    verbose             = argc > 2;
    veryVerbose         = argc > 3;
    veryVeryVerbose     = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    bool usesDefaultAllocator = false;

    bslmt::TimedCompletionGuard completionGuard(&defaultAllocator);
    ASSERT(0 == completionGuard.guard(bsls::TimeInterval(90, 0),
                                      bsl::format("case {}", test)));

    switch (test) { case 0:
      case 10: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //    (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        if (verbose) cout << "\nTesting `my_PooledArray<double>`." << endl;

        const double DATA[] = { 0.0, 1.2, 2.3, 3.4, 4.5, 5.6, 6.7 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator a;
        my_PooledArray<double> array(&a);

        for (int i = 0; i < NUM_DATA; ++i) {
            const double VALUE = DATA[i];
            array.append(VALUE);
            LOOP_ASSERT(i, i + 1 == array.length());
            LOOP_ASSERT(i, VALUE == array[i]);
        }
        array.removeAll();
        ASSERT(0 == array.length());
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // CONCURRENCY TEST
        //   The methods `allocate`, `deallocate`, `release`, and
        //    `reserveCapacity` are thread-safe as per the component contract.
        //
        // Concerns:
        // 1. `allocate`, `deallocate`, and `reserveCapacity` can be used
        //     concurrently.
        //
        // 2. `release` and `reserveCapacity` can be used concurrently.
        //
        // Plan:
        // 1. Stress test interleaved invocations of `allocate`, `deallocate`,
        //    and `reserveCapacity`.  (C-1)
        //
        // 2. Stress test interleaved invocations of `release` and
        //    `reserveCapacity`.  (C-2)
        //
        // Testing:
        //   CONCURRENCY TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "CONCURRENCY TEST" << endl
                          << "================" << endl;

        if (verbose) cout << "\nTesting concurrency." << endl;

        bslmt::ThreadUtil::Handle threads[k_NUM_THREADS];

        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        Obj mX(k_OBJECT_SIZE, &sa);

        for (int i = 0; i < k_NUM_THREADS; ++i) {
            int rc = bslmt::ThreadUtil::create(&threads[i],
                                               workerThread,
                                               &mX);
            LOOP_ASSERT(i, 0 == rc);
        }
        for (int i = 0; i < k_NUM_THREADS; ++i) {
            int rc = bslmt::ThreadUtil::join(threads[i]);
            LOOP_ASSERT(i, 0 == rc);
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // DEPRECATED CREATORS
        //   The deprecated constructors operate as expected.
        //
        // Concerns:
        // 1. The constructor creates the correct initial value and has the
        //    internal memory management system hooked up properly so that
        //    *all* internally allocated memory draws from the same
        //    user-supplied allocator whenever one is specified.
        //
        // 2. An allocation exception during construction results in a valid
        //    object.
        //
        // 3. The supplied `blockSize` is correctly used.
        //
        // 4. Memory is not leaked by the constructor and the destructor
        //    properly deallocates the residual allocated memory.
        //
        // 5. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Create an object using the constructor with and without passing
        //    in an allocator, verify the allocator is stored using the
        //    `allocator` accessor, and verifying all allocations are done from
        //    the allocator by using `allocate` to require additional memory.
        //    (C-1)
        //
        // 2. Create objects using the `bslma::TestAllocator`.  Vary the test
        //    allocator's allocation limit to verify behavior in the presence
        //    of exceptions.  (C-2)
        //
        // 3. Create objects with different `blockSize`, verify the value is
        //    stored correctly using the `blockSize` accessor, and verify the
        //    distance between `allocate` results is appropriate for the
        //    `blockSize`.  (C-3)
        //
        // 4. Use a supplied `bslma::TestAllocator` that goes out-of-scope
        //    at the conclusion of each test to ensure all memory is returned
        //    to the allocator.  (C-4)
        //
        // 5. Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   bdlma::ConcurrentPool(blockSize, strategy, basicAllocator);
        //   bdlma::ConcurrentPool(blockSize, strategy, maxBlocksPerChunk, bA);
        // --------------------------------------------------------------------

        usesDefaultAllocator = true;

        if (verbose) cout << endl
                          << "DEPRECATED CREATORS" << endl
                          << "===================" << endl;

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE4.33
        if (verbose) cout << "\nTesting allocator." << endl;

        // bdlma::ConcurrentPool(blockSize, strategy, basicAllocator)

        {
            {
                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                Obj mX(1, bsls::BlockGrowth::BSLS_CONSTANT);
                const Obj& X = mX;
                ASSERT(&defaultAllocator == X.allocator());
                ASSERT(allocations == defaultAllocator.numAllocations());

                mX.allocate();
                ASSERT(allocations + 1 == defaultAllocator.numAllocations());
            }
            ASSERT(defaultAllocator.numAllocations() ==
                                          defaultAllocator.numDeallocations());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            {
                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                Obj mX(1,
                       bsls::BlockGrowth::BSLS_CONSTANT,
                       reinterpret_cast<bslma::TestAllocator *>(0));
                const Obj& X = mX;
                ASSERT(&defaultAllocator == X.allocator());
                ASSERT(allocations == defaultAllocator.numAllocations());

                mX.allocate();
                ASSERT(allocations + 1 == defaultAllocator.numAllocations());
           }
            ASSERT(defaultAllocator.numAllocations() ==
                                          defaultAllocator.numDeallocations());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            {
                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();


                Obj mX(1, bsls::BlockGrowth::BSLS_CONSTANT, &sa);
                const Obj& X = mX;
                ASSERT(&sa == X.allocator());
                ASSERT(allocations == defaultAllocator.numAllocations());
                ASSERT(0 == sa.numAllocations());

                mX.allocate();
                ASSERT(allocations == defaultAllocator.numAllocations());
                ASSERT(1 == sa.numAllocations());
            }
            ASSERT(defaultAllocator.numAllocations() ==
                                          defaultAllocator.numDeallocations());
            ASSERT(0 == defaultAllocator.numBytesInUse());

            ASSERT(sa.numAllocations() == sa.numDeallocations());
            ASSERT(0 == sa.numBytesInUse());
        }

        // bdlma::ConcurrentPool(blockSize, strategy, maxBlocksPerChunk, bA);

        {
            {
                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                Obj mX(1, bsls::BlockGrowth::BSLS_CONSTANT, 1);
                const Obj& X = mX;
                ASSERT(&defaultAllocator == X.allocator());
                ASSERT(allocations == defaultAllocator.numAllocations());

                mX.allocate();
                ASSERT(allocations + 1 == defaultAllocator.numAllocations());
            }
            ASSERT(defaultAllocator.numAllocations() ==
                                          defaultAllocator.numDeallocations());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            {
                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                Obj mX(1,
                       bsls::BlockGrowth::BSLS_CONSTANT,
                       1,
                       reinterpret_cast<bslma::TestAllocator *>(0));
                const Obj& X = mX;
                ASSERT(&defaultAllocator == X.allocator());
                ASSERT(allocations == defaultAllocator.numAllocations());

                mX.allocate();
                ASSERT(allocations + 1 == defaultAllocator.numAllocations());
           }
            ASSERT(defaultAllocator.numAllocations() ==
                                          defaultAllocator.numDeallocations());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            {
                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();


                Obj mX(1, bsls::BlockGrowth::BSLS_CONSTANT, 1, &sa);
                const Obj& X = mX;
                ASSERT(&sa == X.allocator());
                ASSERT(allocations == defaultAllocator.numAllocations());
                ASSERT(0 == sa.numAllocations());

                mX.allocate();
                ASSERT(allocations == defaultAllocator.numAllocations());
                ASSERT(1 == sa.numAllocations());
            }
            ASSERT(defaultAllocator.numAllocations() ==
                                          defaultAllocator.numDeallocations());
            ASSERT(0 == defaultAllocator.numBytesInUse());

            ASSERT(sa.numAllocations() == sa.numDeallocations());
            ASSERT(0 == sa.numBytesInUse());
        }

        if (verbose) cout << "\nTesting exception behavior." << endl;

        // bdlma::ConcurrentPool(blockSize, strategy, basicAllocator)

        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                Obj mX(1, bsls::BlockGrowth::BSLS_CONSTANT, &sa);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        // bdlma::ConcurrentPool(blockSize, strategy, maxBlocksPerChunk, bA);

        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                Obj mX(1, bsls::BlockGrowth::BSLS_CONSTANT, 1, &sa);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nTesting `blockSize`." << endl;

        // bdlma::ConcurrentPool(blockSize, strategy, basicAllocator)

        {
            for (unsigned blockSize = 1;
                 blockSize <= 5 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
                 ++blockSize) {
                Obj mX(blockSize, bsls::BlockGrowth::BSLS_CONSTANT);
                const Obj& X = mX;

                ASSERT(blockSize == X.blockSize());

                ASSERT(0 != mX.allocate());

                char *p = static_cast<char *>(mX.allocate());
                ASSERT(0 != p);

                char *q = static_cast<char *>(mX.allocate());
                ASSERT(0 != q);

                unsigned diff = static_cast<unsigned>(q - p);

                ASSERT(0 == diff % bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
                ASSERT(diff >= blockSize);
                ASSERT(diff <
                          blockSize + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
            }
        }

        // bdlma::ConcurrentPool(blockSize, strategy, maxBlocksPerChunk, bA);

        {
            for (unsigned blockSize = 1;
                 blockSize <= 5 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
                 ++blockSize) {
                Obj mX(blockSize, bsls::BlockGrowth::BSLS_CONSTANT, 1);
                const Obj& X = mX;

                ASSERT(blockSize == X.blockSize());

                ASSERT(0 != mX.allocate());

                char *p = static_cast<char *>(mX.allocate());
                ASSERT(0 != p);

                char *q = static_cast<char *>(mX.allocate());
                ASSERT(0 != q);

                unsigned diff = static_cast<unsigned>(q - p);

                ASSERT(0 == diff % bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
                ASSERT(diff >= blockSize);
                ASSERT(diff <
                          blockSize + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;

        // bdlma::ConcurrentPool(blockSize, strategy, basicAllocator)

        {
            bsls::AssertTestHandlerGuard hG;

            {
                ASSERT_PASS(Obj mX(1, bsls::BlockGrowth::BSLS_CONSTANT));
            }
            {
                ASSERT_PASS(Obj mX(2, bsls::BlockGrowth::BSLS_CONSTANT));
            }
            {
                ASSERT_FAIL(Obj mX(0, bsls::BlockGrowth::BSLS_CONSTANT));
            }
        }

        // bdlma::ConcurrentPool(blockSize, strategy, maxBlocksPerChunk, bA);

        {
            bsls::AssertTestHandlerGuard hG;

            {
                ASSERT_PASS(Obj mX(1, bsls::BlockGrowth::BSLS_CONSTANT, 1));
            }
            {
                ASSERT_PASS(Obj mX(2, bsls::BlockGrowth::BSLS_CONSTANT, 1));
            }
            {
                ASSERT_FAIL(Obj mX(0, bsls::BlockGrowth::BSLS_CONSTANT, 1));
            }
        }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE4.33
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING `reserveCapacity`
        //   The `reserveCapacity` method operates as expected.
        //
        // Concerns:
        // 1. `reserveCapacity` appropriately uses the underlying allocator.
        //
        // 2. `reserveCapacity` ensures at least the specified number of blocks
        //     can be provided by the `allocate` method before another
        //     allocation from the underlying allocator.
        //
        // 3. If an exception occurs during `reserveCapacity`, the pool is in
        //    a valid state.
        //
        // 4. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Create a pool, preform a number of `allocate`, and then invoke
        //    `reservedCapacity`.  Verify the underlying allocator has been
        //    used correctly.  Compute from the number of allocations and the
        //    amount to reserve how many times `allocate` can be invoked before
        //    an underlying allocation.  Perform the allocations and verify no
        //    underlying allocation occurred.  (C-1,2)
        //
        // 2. Cause an exception during `reserveCapacity`, verify the pool
        //    can still be used to `allocate` and be destroyed.  (C-3)
        //
        // 3. Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   void reserveCapacity(int numBlocks);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING `reserveCapacity`" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nTesting `reserveCapacity`." << endl;

        int chunkSize  = 0;
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT, &sa);

            bsls::Types::Int64 allocations = sa.numAllocations();

            mX.allocate();
            ASSERT(allocations + 1 == sa.numAllocations());

            // 'allocate' until underlying allocator allocates
            while (allocations + 1 == sa.numAllocations()) {
                mX.allocate();
                ++chunkSize;
            }

            if (veryVerbose) P(chunkSize);
        }
        LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                    0 == defaultAllocator.numBlocksTotal());


        const int RESERVED[] = {
            0, 1, 2, 3, 4, 5, 15, 16, 17, 30, 32, 35, 50, 64, 90, 120, 260,
            512, 1017, 2096
        };
        const int NUM_RESERVED = sizeof RESERVED / sizeof *RESERVED;

        const int EXTENDED[] = {
            0, 1, 4, 5, 7, 16, 17, 23, 32, 40, 45, 50, 64, 80, 100, 200
        };
        const int NUM_EXTENDED = sizeof EXTENDED / sizeof *EXTENDED;

        for (int ri = 0; ri < NUM_RESERVED; ++ri) {
            for (int ei = 0; ei < NUM_EXTENDED; ++ei) {
                const int RESERVE = RESERVED[ri];  // number of blocks
                                                   // requested in
                                                   // `reserveCapacity`
                                                   // invocation

                const int EXTEND  = EXTENDED[ei];  // number of allocations and
                                                   // deallocations performed
                                                   // before `reserveCapacity`
                                                   // to provide available
                                                   // blocks

                // Load into `ALLOCATE_AFTER_RESERVE` the number of `allocate`
                // invocations that must be able to complete without causing an
                // allocation on the underlying allocator after the
                // `reserveCapacity`.
                const int ALLOCATE_AFTER_RESERVE = (RESERVE + chunkSize - 1)
                                                                   / chunkSize
                                                                   * chunkSize;

                // Load into `EXP_UNDERLYING_ALLOCATES` the expected number of
                // allocations, by the underlying allocator, caused by the
                // `reserveCapacity`.
                const int EXP_UNDERLYING_ALLOCATES =
                                     (ALLOCATE_AFTER_RESERVE > EXTEND ? 1 : 0);

                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                Obj mX(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT, &sa);

                bslma::TestAllocator aux("auxiliary", veryVeryVeryVerbose);

                bsl::vector<void *> allocated(&aux);
                allocated.resize(EXTEND);

                for (int i = 0; i < EXTEND; ++i) {
                    allocated[i] = mX.allocate();
                }

                // to simplify testing, a bogus address will be deallocated and
                // later allocated to side-step the single element reuse cache

                bsls::Types::Int64 bogusValue = 0;
                mX.deallocate(&bogusValue);  // undefined behavior

                for (int i = 0; i < EXTEND; ++i) {
                    mX.deallocate(allocated[i]);
                }

                ASSERT(&bogusValue == mX.allocate());

                bsls::Types::Int64 numAlloc   = sa.numAllocations();
                bsls::Types::Int64 numDealloc = sa.numDeallocations();

                mX.reserveCapacity(RESERVE);

                ASSERT(sa.numAllocations()   == numAlloc +
                                                     EXP_UNDERLYING_ALLOCATES);
                ASSERT(sa.numDeallocations() == numDealloc);

                for (int i = 0; i < ALLOCATE_AFTER_RESERVE; ++i) {
                    mX.allocate();
                }

                ASSERT(sa.numAllocations()   == numAlloc +
                                                     EXP_UNDERLYING_ALLOCATES);
                ASSERT(sa.numDeallocations() == numDealloc);
            }
        }
        LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                    0 == defaultAllocator.numBlocksTotal());


#ifdef BDE_BUILD_TARGET_EXC
        if (verbose) cout << "\nTesting exception behavior." << endl;
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT, &sa);

            {
                sa.setAllocationLimit(0);

                bool caught = false;

                try {
                    mX.reserveCapacity(500);
                } catch (BloombergLP::bslma::TestAllocatorException& e) {
                    caught = true;
                }
                ASSERT(caught);

                sa.setAllocationLimit(-1);
            }

            for (int i = 0; i < 1000; ++i) {
                ASSERT(0 != mX.allocate());
            }

            {
                sa.setAllocationLimit(0);

                bool caught = false;

                try {
                    mX.reserveCapacity(500);
                } catch (BloombergLP::bslma::TestAllocatorException& e) {
                    caught = true;
                }
                ASSERT(caught);

                sa.setAllocationLimit(-1);
            }

            for (int i = 0; i < 1000; ++i) {
                ASSERT(0 != mX.allocate());
            }
        }
#endif

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(1, &sa);

            ASSERT_PASS(mX.reserveCapacity( 0));
            ASSERT_PASS(mX.reserveCapacity( 1));
            ASSERT_FAIL(mX.reserveCapacity(-1));
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING `release`
        //   The `release` method operates as expected.
        //
        // Concerns:
        // 1. `release` returns all memory to the underlying allocator and
        //     correctly reinitialized the pool.
        //
        // Plan:
        // 1. Create a pool and determine its chunk size and allocator cache
        //    size.  For a large set of values `i`, perform `i` allocations
        //    followed by a `release`.  Verify memory is returned to the
        //    underlying allocator as expected.  (C-1)
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING `release`" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nTesting `release`." << endl;

        unsigned chunkSize = 0;
        unsigned cacheSize = 0;

        bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

        Obj mX(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT, &sa);

        unsigned initialAllocationSize =
                                     static_cast<unsigned>(sa.numBytesInUse());

        {
            bsls::Types::Int64 allocations = sa.numAllocations();

            mX.allocate();
            ASSERT(allocations + 1 == sa.numAllocations());

            // 'allocate' until underlying allocator allocates
            while (allocations + 1 == sa.numAllocations()) {
                mX.allocate();
                ++chunkSize;
            }

            if (veryVerbose) P(chunkSize);

            cacheSize = computeCacheSize(
                                       bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT,
                                       chunkSize,
                                       initialAllocationSize);

            mX.release();
            ASSERT(allocations + 2 == sa.numAllocations());
            ASSERT(2 == sa.numDeallocations());
            ASSERT(initialAllocationSize ==
                                    static_cast<unsigned>(sa.numBytesInUse()));
        }

        for (unsigned i = 1; i <= cacheSize; ++i) {
            for (unsigned j = 0; j < i; ++j) {
                mX.allocate();
            }
            ASSERT(sa.numAllocations() > sa.numDeallocations());
            ASSERT(initialAllocationSize <
                                    static_cast<unsigned>(sa.numBytesInUse()));
            mX.release();
            ASSERT(sa.numAllocations() == sa.numDeallocations());
            ASSERT(initialAllocationSize ==
                                    static_cast<unsigned>(sa.numBytesInUse()));
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING `operator new`
        //   The `operator new` method operates as expected.
        //
        // Concerns:
        // 1. `operator new` appropriately forwards to the supplied pool.
        //
        // 2. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Create a pool and use the pool's single element reuse cache to
        //    verify `operator new` is forwarding correctly.  (C-1)
        //
        // 2. Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   void *operator new(bsl::size_t size, bdlma::ConcurrentPool& pool);
        //   void operator delete(void *address, bdlma::ConcurrentPool& pool);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING `operator new`" << endl
                          << "======================" << endl;

        if (verbose) cout << "\nTesting forwarding." << endl;
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT, &sa);

            int *a = new (mX) int;
            int *b = new (mX) int;

            mX.deleteObject(a);
            ASSERT(a == new(mX) int);

            int *c = new (mX) int;

            mX.deleteObject(a);
            ASSERT(a == new(mX) int);
            mX.deleteObject(b);
            ASSERT(b == new(mX) int);
            mX.deleteObject(c);
            ASSERT(c == new(mX) int);
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            {
                Obj mX(sizeof(int), &sa);
                ASSERT_SAFE_PASS(new (mX) int);
            }
            {
                Obj mX(1, &sa);
                ASSERT_SAFE_FAIL(new (mX) int);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING `deleteObject` AND `deleteObjectRaw`
        //   The `deleteObject` and `deleteObjectRaw` methods operate as
        //   expected.
        //
        // Concerns:
        // 1. That `deleteObject` and `deleteObjectRaw` properly destroy and
        //    deallocate managed objects.
        //
        // Plan:
        // 1. Iterate where at the beginning of the loop, we create an object
        //    of type `mostDerived` that multiply inherits from two types with
        //    virtual destructors.  Then in the middle of the loop we switch
        //    into several ways of destroying and deallocating the object with
        //    various forms of `deleteObjectRaw` and `deleteObject`, after
        //    which we verify that the destructors have been run.  Each
        //    iteration we verify that the memory we got was the same as for
        //    the previous iteration, which shows that memory is being
        //    deallocated and recovered by the pool.
        //
        // Testing:
        //   void deleteObject(const TYPE *object);
        //   void deleteObjectRaw(const TYPE *object);
        // --------------------------------------------------------------------

        if (verbose)
                cout << endl
                     << "TESTING `deleteObject` AND `deleteObjectRaw`" << endl
                     << "============================================" << endl;

        if (verbose) cout << "\nTesting `deleteObject` and `deleteObjectRaw`."
                          << endl;

        bslma::TestAllocator alloc, *Z = &alloc;

        bool finished = false;
        const MostDerived *repeater = 0;    // verify we're re-using the memory
                                            // each time
        Obj pool(sizeof(MostDerived), bsls::BlockGrowth::BSLS_CONSTANT, 10, Z);
        for (int di = 0; !finished; ++di) {
            MostDerived *pMD = static_cast<MostDerived *>(pool.allocate());
            const MostDerived *pMDC = pMD;

            if (!repeater) {
                repeater = pMDC;
            }
            else {
                // this verifies that we are freeing the memory each iteration
                // because we get the same pointer every time we allocate, and
                // we allocate one extra time at the end
                LOOP_ASSERT(di, repeater == pMDC);
            }
            new (pMD) MostDerived();

            ASSERT(1 == numLeftChildren);
            ASSERT(1 == numMiddleChildren);
            ASSERT(1 == numRightChildren);
            ASSERT(1 == numMostDerived);

            switch (di) {
              case 0: {
                pool.deleteObjectRaw(pMDC);
              } break;
              case 1: {
                const LeftChild *pLCC = pMDC;
                ASSERT((const void*) pLCC == (const void*) pMDC);
                pool.deleteObjectRaw(pLCC);
              } break;
              case 2: {
                pool.deleteObject(pMDC);
              } break;
              case 3: {
                const LeftChild *pLCC = pMDC;
                ASSERT((const void*) pLCC == (const void*) pMDC);
                pool.deleteObject(pLCC);
              } break;
              case 4: {
                const RightChild *pRCC = pMDC;
                ASSERT((const void*) pRCC != (const void*) pMDC);
                pool.deleteObject(pRCC);
              } break;
              case 5: {
                pool.deleteObjectRaw(pMDC);    // 2nd time we do this

                finished = true;
              } break;
              default: {
                ASSERT(0);
              }
            }

            LOOP_ASSERT(di, 0 == numLeftChildren);
            LOOP_ASSERT(di, 0 == numMiddleChildren);
            LOOP_ASSERT(di, 0 == numRightChildren);
            LOOP_ASSERT(di, 0 == numMostDerived);
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `allocate` AND `deallocate`
        //   The `allocate` and `deallocate` methods operate as expected.
        //
        // Concerns:
        // 1. `allocate` appropriately uses the underlying allocator.
        //
        // 2. `deallocate` makes memory available for reuse.
        //
        // 3. If an exception occurs during an `allocate`, the appropriate
        //    number of `allocate` invocations fail.
        //
        // 4. The single element reuse cache is appropriately used.
        //
        // 5. An initial sequence of exceptions is correctly handled.
        //
        // 6. Block size does not affect correctness.
        //
        // Plan:
        // 1. Create a pool and invoke `allocate` repeatedly, storing the
        //    returned memory addresses and verifying the number of
        //    allocations performed by the underlying allocator.  Determine
        //    the implementation's chunk and allocation cache size.
        //
        // 2. Deallocate a bogus value (for use with the single element reuse
        //    cache; note this is undefined behavior), use `deallocate` to
        //    return all obtained memory, and then allocate a value and verify
        //    the value matches the supplied bogus value.  Use allocate to
        //    obtain double the allocation cache size number of blocks.
        //    Verify all deallocated values were reused, and the appropriate
        //    number of allocations were performed by the underlying allocator.
        //    (C-2)
        //
        // 3. Cause an exception during an `allocate`, verify this and an
        //    additional chunk size minus one invocations return null.  (C-3)
        //
        // 4. Repeatedly `allocate` and `deallocate` to verify the single
        //    element reuse cache is used correctly.  (C-4)
        //
        // 5. Repeat steps 1-4 with varying number of initial exceptions.
        //    (C-5)
        //
        // 6. Repeat steps 1-5 with varying block size.  (C-6)
        //
        // Testing:
        //   void *allocate();
        //   void deallocate(void *address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING `allocate` AND `deallocate`" << endl
                          << "===================================" << endl;

        const unsigned BLOCK_SIZE[] = { 1, 8192, 10000, 100000 };

        const int NUM_BLOCK_SIZE  = static_cast<int>(  sizeof BLOCK_SIZE
                                                     / sizeof *BLOCK_SIZE);

        for (int blockSizeIndex = 0;
             blockSizeIndex < NUM_BLOCK_SIZE;
             ++blockSizeIndex) {
            const unsigned blockSize = BLOCK_SIZE[blockSizeIndex];

            for (int initialExceptions = 0;
                 initialExceptions < 5;
                 ++initialExceptions) {
                unsigned chunkSize = 0;
                unsigned cacheSize = 0;

                bslma::TestAllocator aux("auxiliary", veryVeryVeryVerbose);

                bsl::set<void *> allocated(&aux);

                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                Obj mX(blockSize, &sa);
                ASSERT(0 == sa.numAllocations());

#ifdef BDE_BUILD_TARGET_EXC
                for (int i = 0; i < initialExceptions; ++i) {
                    sa.setAllocationLimit(0);

                    bool caught = false;

                    try {
                        ASSERT(0 == mX.allocate());
                    } catch (BloombergLP::bslma::TestAllocatorException& e) {
                        caught = true;
                    }
                    ASSERT(caught);

                    sa.setAllocationLimit(-1);

                    sa.stashStatistics();
                }
#endif

                if (verbose) cout << "\nTesting `allocate`." << endl;
                {
                    allocated.insert(mX.allocate());
                    ASSERT(1 == sa.numAllocations());

                    unsigned initialAllocationSize =
                                     static_cast<unsigned>(sa.numBytesInUse());

                    while (1 == sa.numAllocations()) {
                        allocated.insert(mX.allocate());
                        ++chunkSize;
                    }
                    ASSERT(2 == sa.numAllocations());

                    cacheSize = computeCacheSize(blockSize,
                                                 chunkSize,
                                                 initialAllocationSize);

                    while (allocated.size() < cacheSize) {
                        allocated.insert(mX.allocate());
                        int expected = static_cast<int>(
                                         allocated.size() - 1) / chunkSize + 1;
                        int observed = static_cast<int>(sa.numAllocations());
                        ASSERTV(allocated.size(),
                                chunkSize,
                                expected,
                                observed,
                                expected == observed);
                    }
                }

                if (verbose) cout << "\nTesting `deallocate`." << endl;
                {
                    // to simplify testing, a bogus address will be deallocated
                    // and later allocated to side-step the single element
                    // reuse cache

                    bsls::Types::Int64 bogusValue = 0;
                    mX.deallocate(&bogusValue);  // undefined behavior

                    bsl::set<void *>::const_iterator iter = allocated.cbegin();
                    while (iter != allocated.cend()) {
                        mX.deallocate(*iter);
                        ++iter;
                    }

                    ASSERT(&bogusValue == mX.allocate());

                    // the next allocations should reuse all deallocated values
                    // and require additional allocations

                    unsigned reused = 0;
                    for (unsigned i = 0 ; i < 2 * cacheSize; ++i) {
                        reused += static_cast<unsigned>(
                                               allocated.erase(mX.allocate()));
                    }
                    ASSERTV(reused, cacheSize, reused == cacheSize);
                    ASSERT(allocated.empty());
                    ASSERT(2 * cacheSize / chunkSize == sa.numAllocations());
                }

#ifdef BDE_BUILD_TARGET_EXC
                if (verbose) cout << "\nTesting exception behavior." << endl;
                {
                    sa.setAllocationLimit(0);

                    bool caught = false;

                    try {
                        ASSERT(0 == mX.allocate());
                    } catch (BloombergLP::bslma::TestAllocatorException& e) {
                        caught = true;
                    }
                    ASSERT(caught);

                    sa.setAllocationLimit(-1);

                    ASSERT(0 != mX.allocate());
                }
#endif

                if (verbose) cout << "\nTesting single element reuse cache."
                                  << endl;
                {
                    void *a = mX.allocate();
                    void *b = mX.allocate();
                    void *c = mX.allocate();

                    mX.deallocate(a);
                    ASSERT(a == mX.allocate());

                    void *d = mX.allocate();

                    mX.deallocate(a);
                    ASSERT(a == mX.allocate());
                    mX.deallocate(b);
                    ASSERT(b == mX.allocate());

                    void *e = mX.allocate();

                    mX.deallocate(a);
                    ASSERT(a == mX.allocate());
                    mX.deallocate(b);
                    ASSERT(b == mX.allocate());
                    mX.deallocate(c);
                    ASSERT(c == mX.allocate());
                    mX.deallocate(d);
                    ASSERT(d == mX.allocate());
                    mX.deallocate(e);
                    ASSERT(e == mX.allocate());
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS AND BASIC ACCESSORS TEST
        //   The constructor and basic accessors operate as expected.
        //
        // Concerns:
        // 1. The constructor creates the correct initial value and has the
        //    internal memory management system hooked up properly so that
        //    *all* internally allocated memory draws from the same
        //    user-supplied allocator whenever one is specified.
        //
        // 2. An allocation exception during construction results in a valid
        //    object.
        //
        // 3. The supplied `blockSize` is correctly used, including its effect
        //    on the chunk size and cache size.
        //
        // 4. Memory is not leaked by the constructor and the destructor
        //    properly deallocates the residual allocated memory.
        //
        // 5. QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        // 1. Create an object using the constructor with and without passing
        //    in an allocator, verify the allocator is stored using the
        //    `allocator` accessor, and verifying all allocations are done from
        //    the allocator by using `allocate` to require additional memory.
        //    (C-1)
        //
        // 2. Create objects using the `bslma::TestAllocator`.  Vary the test
        //    allocator's allocation limit to verify behavior in the presence
        //    of exceptions.  (C-2)
        //
        // 3. Create objects with different `blockSize`, verify the value is
        //    stored correctly using the `blockSize` accessor, and verify the
        //    distance between `allocate` results is appropriate for the
        //    `blockSize`.  Also compute the chunk size and cache size and
        //    compare to a table of values.  (C-3)
        //
        // 4. Use a supplied `bslma::TestAllocator` that goes out-of-scope
        //    at the conclusion of each test to ensure all memory is returned
        //    to the allocator.  (C-4)
        //
        // 5. Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   bdlma::ConcurrentPool(blockSize, basicAllocator);
        //   ~bdlma::ConcurrentPool();
        //   bsls::Types::size_type blockSize() const;
        //   bslma::Allocator *allocator() const;
        // --------------------------------------------------------------------

        usesDefaultAllocator = true;

        if (verbose) cout << endl
                          << "CREATORS AND BASIC ACCESSORS TEST" << endl
                          << "=================================" << endl;

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;
        {
            {
                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                Obj mX(1);  const Obj& X = mX;
                ASSERT(&defaultAllocator == X.allocator());
                ASSERT(allocations == defaultAllocator.numAllocations());

                mX.allocate();
                ASSERT(allocations + 1 == defaultAllocator.numAllocations());
            }
            ASSERT(defaultAllocator.numAllocations() ==
                                          defaultAllocator.numDeallocations());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            {
                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                Obj        mX(1, reinterpret_cast<bslma::TestAllocator *>(0));
                const Obj& X = mX;
                ASSERT(&defaultAllocator == X.allocator());
                ASSERT(allocations == defaultAllocator.numAllocations());

                mX.allocate();
                ASSERT(allocations + 1 == defaultAllocator.numAllocations());
           }
            ASSERT(defaultAllocator.numAllocations() ==
                                          defaultAllocator.numDeallocations());
            ASSERT(0 == defaultAllocator.numBytesInUse());
        }
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);
            {
                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();


                Obj mX(1, &sa);  const Obj& X = mX;
                ASSERT(&sa == X.allocator());
                ASSERT(allocations == defaultAllocator.numAllocations());
                ASSERT(0 == sa.numAllocations());

                mX.allocate();
                ASSERT(allocations == defaultAllocator.numAllocations());
                ASSERT(1 == sa.numAllocations());
            }
            ASSERT(defaultAllocator.numAllocations() ==
                                          defaultAllocator.numDeallocations());
            ASSERT(0 == defaultAllocator.numBytesInUse());

            ASSERT(sa.numAllocations() == sa.numDeallocations());
            ASSERT(0 == sa.numBytesInUse());
        }

        if (verbose) cout << "\nTesting exception behavior." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                Obj mX(1, &sa);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nTesting `blockSize`." << endl;
        {
            for (unsigned blockSize = 1;
                 blockSize <= 5 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;
                 ++blockSize) {
                Obj mX(blockSize);  const Obj& X = mX;

                ASSERT(blockSize == X.blockSize());

                ASSERT(0 != mX.allocate());

                char *p = static_cast<char *>(mX.allocate());
                ASSERT(0 != p);

                char *q = static_cast<char *>(mX.allocate());
                ASSERT(0 != q);

                unsigned diff = static_cast<unsigned>(q - p);

                ASSERT(0 == diff % bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
                ASSERT(diff >= blockSize);
                ASSERT(diff <
                          blockSize + bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
            }

            static const struct {
                int      d_line;          // source line number
                unsigned d_blockSize;     // block size under test
                unsigned d_expChunkSize;  // expected chunk size
                unsigned d_expCacheSize;  // expected cache size
            } DATA[] = {
                //LINE    BLOCK   CHUNK  CACHE
                //----   -------   -----  -----
                { L_,          1,     32,   256 },
                { L_,       8192,     32,   256 },
                { L_,       8193,     32,   128 },
                { L_,      16384,     32,   128 },
                { L_,      16385,     16,   128 },
                { L_,      32768,     16,   128 },
                { L_,      32769,     16,    64 },
                { L_,      65536,     16,    64 },
                { L_,      65537,      8,    64 },
                { L_,     131072,      8,    64 },
                { L_,     131073,      8,    32 },
                { L_,     262144,      8,    32 },
                { L_,     262145,      4,    32 },
                { L_,     524288,      4,    32 },
                { L_,     524289,      4,    16 },
                { L_,    1048576,      4,    16 },
                { L_,    1048577,      2,    16 },
                { L_,    2097152,      2,    16 },
                { L_,    2097153,      2,     8 },
                { L_,    4194304,      2,     8 },
                { L_,    4194305,      2,     8 },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int      LINE           = DATA[i].d_line;
                const unsigned BLOCK_SIZE     = DATA[i].d_blockSize;
                const unsigned EXP_CHUNK_SIZE = DATA[i].d_expChunkSize;
                const unsigned EXP_CACHE_SIZE = DATA[i].d_expCacheSize;

                bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                Obj mX(BLOCK_SIZE, &sa);
                ASSERT(0 == sa.numAllocations());

                mX.allocate();
                ASSERT(1 == sa.numAllocations());

                unsigned initialAllocationSize =
                                     static_cast<unsigned>(sa.numBytesInUse());

                unsigned chunkSize = 0;
                while (1 == sa.numAllocations()) {
                    mX.allocate();
                    ++chunkSize;
                }
                ASSERT(2 == sa.numAllocations());

                unsigned cacheSize = computeCacheSize(BLOCK_SIZE,
                                                      chunkSize,
                                                      initialAllocationSize);

                ASSERTV(LINE, chunkSize, EXP_CHUNK_SIZE == chunkSize);
                ASSERTV(LINE, cacheSize, EXP_CACHE_SIZE == cacheSize);
            }
        }

        if (verbose) cout << "\nNegative testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            {
                ASSERT_PASS(Obj mX(1));
            }
            {
                ASSERT_PASS(Obj mX(2));
            }
            {
                ASSERT_FAIL(Obj mX(0));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Instantiate an object and verify basic functionality.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        usesDefaultAllocator = true;

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout << "\nBreathing test." << endl;
        {
            Obj mX(1);  const Obj& X = mX;

            ASSERT(1 == X.blockSize());

            ASSERT(0 != mX.allocate());

            char *p = static_cast<char *>(mX.allocate());
            ASSERT(0 != p);

            char *q = static_cast<char *>(mX.allocate());
            ASSERT(0 != q);

            ASSERT(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT == q - p);

            mX.deallocate(p);
            mX.deallocate(q);
        }
        {
            Obj mX(2 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT);
            const Obj& X = mX;

            ASSERT(2 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT ==
                                                                X.blockSize());

            ASSERT(0 != mX.allocate());

            char *p = static_cast<char *>(mX.allocate());
            ASSERT(0 != p);

            char *q = static_cast<char *>(mX.allocate());
            ASSERT(0 != q);

            ASSERT(2 * bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT == q - p);

            mX.deallocate(p);
            mX.deallocate(q);
        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
        // Since BDE allocators do not support over-alignment, increasing the
        // alignment can cause runtime failures.

        ASSERT(bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT >= alignof(Obj));
#endif
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    // CONCERN: Memory comes from default allocator only when expected.

    if (!usesDefaultAllocator) {
        LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                    0 == defaultAllocator.numBlocksTotal());
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
