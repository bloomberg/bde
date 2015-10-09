// bslstl_simplepool.t.cpp                                            -*-C++-*-
#include <bslstl_simplepool.h>

#include <bslstl_allocator.h>

#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_alignmentutil.h>
#include <bsls_bsltestutil.h>

#include <bsltf_templatetestfacility.h>
#include <bsltf_stdtestallocator.h>

#include <algorithm>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

using namespace BloombergLP;
using namespace std;
using namespace bslstl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test implements a mechanism for allocating fixed sized
// memory blocks from a memory pool.  The main concerns are memory is allocated
// only when expected and the allocated memory is aligned correctly
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] explicit SimplePool(const ALLOCATOR& allocator);
// [ 2] ~SimplePool();

// MANIPULATORS
// [ 4] AllocatorType& allocator();
// [ 2] VALUE *allocate();
// [ 5] void deallocate(void *address);
// [ 6] void reserve(std::size_t numBlocks);
// [ 7] void release();
// [ 8] void swap(SimplePool<VALUE, ALLOCATOR>& other);
//
// ACCESSORS
// [ 4] const AllocatorType& allocator() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [10] USAGE EXAMPLE
// [ 9] CONCERN: Standard allocator can be used
// [ 3] TEST APPARATUS

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
// NOTE: THIS IS A LOW-LEVEL COMPONENT AND MAY NOT USE ANY C++ LIBRARY
// FUNCTIONS, INCLUDING IOSTREAMS.
static int testStatus = 0;

namespace {

void aSsErT(bool b, const char *s, int i) {
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

#define RUN_EACH_TYPE BSLTF_TEMPLATETESTFACILITY_RUN_EACH_TYPE

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
//                       GLOBAL TEST VALUES
// ----------------------------------------------------------------------------

static bool             verbose;
static bool         veryVerbose;
static bool     veryVeryVerbose;
static bool veryVeryVeryVerbose;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

// Define various types that may cause potential alignment issues.

struct TestType1 {
    char   x1;
    double x2;
};

struct TestType2 {
    double x1;
    char   x2;
};

struct TestType3 {
    char   x1[5];
};

// Define all the types that is used for each test case.

#define TEST_TYPES  int, \
                    char, \
                    bsls::AlignmentUtil::MaxAlignedType, \
                    TestType1, \
                    TestType2, \
                    TestType3

//=============================================================================
//                               TEST FACILITIES
//-----------------------------------------------------------------------------

namespace {

bool expectToAllocate(int n)
    // Return 'true' if the container is expected to allocate memory on the
    // specified 'n'th element, and 'false' otherwise.
{
    if (n > 32) {
        return (0 == n % 32);                                         // RETURN
    }
    return (((n - 1) & n) == 0);  // Allocate when 'n' is a power of 2
}


class Stack {
    // A fixed sized stack for storing pointers allocated/deallocated by the
    // pool.

    enum { CAPACITY = 128 };  // maximum capacity of the container

    // DATA
    void  *d_data[CAPACITY];  // storage of pointers
    int    d_size;            // number of elements in the stack

  public:
    // CREATORS
    Stack() : d_size(0) {}
        // Create an empty stack.

    Stack(const Stack& original)
        // Create a stack with the same data as the specified 'original'.
    {
        d_size = original.d_size;
        memcpy(d_data, original.d_data, d_size * sizeof(*d_data));
    }

    // MANIPULATORS
    void push(void *value)
        // Add the specified 'value' to the top of the stack.
    {
        BSLS_ASSERT(CAPACITY != d_size);

        d_data[d_size] = value;
        ++d_size;
    }

    void pop()
        // Remove the top value from the stack.  The behavior is undefined
        // unless the stack is not empty.
    {
        BSLS_ASSERT(0 != d_size);

        --d_size;
    }

    // ACCESSORS
    bool empty() { return 0 == d_size; }
        // Return 'true' if the stack is not empty, and 'false' otherwise.

    int size() { return d_size; }
        // Return the number of elements in the container.

    void *top()
        // Return the top value in the stack.
    {
        BSLS_ASSERT(0 != d_size);

        return d_data[d_size - 1];
    }
};

template <class VALUE>
class TestDriver {
    // This templatized struct provide a namespace for testing the 'map'
    // container.  The parameterized 'VALUE' specifies the value type for this
    // object.  Each "testCase*" method test a specific aspect of
    // 'SimplePool<VALUE>'.  Every test cases should be invoked with various
    // parameterized type to fully test the container.

  private:
    // TYPES
    typedef bslstl::SimplePool<VALUE, bsl::allocator<VALUE> > Obj;
        // Type under testing.

    typedef bsltf::StdTestAllocator<VALUE> StlAlloc;
        // Alias for a STL compliant allocator.

  private:
    static const Obj& init(Obj   *result,
                           Stack *usedBlocks,
                           Stack *freeBlocks,
                           int    numAllocs,
                           int    numDealloc);
        // Initialize the specified 'result' object by performing a number of
        // allocations and deallocations indicated by the specified
        // 'numAllocs' and 'numDealloc', and load into the specified
        // 'usedBlocks' and 'freeBlocks' the addresses of the memory blocks
        // that was allocated and deallocated respectively.  The behavior is
        // undefined unless 'numAllocs >= numDealloc'.

    static void createFreeBlocks(Obj *result, int numBlocks);
        // Perform allocations and deallocations on the specified 'result' such
        // that it will be left with the specified 'numBlocks' number of free
        // blocks.

  public:
    // TEST CASES
    static void testCase10();
        // Test usage example.

    static void testCase9();
        // Test alignment concern.

    static void testCase8();
        // Test 'swap' member.

    static void testCase7();
        // Test 'release'.

    static void testCase6();
        // Test 'reserve'.

    static void testCase5();
        // Test 'deallocate'.

    static void testCase4();
        // Test basic accessors ('allocator').

    static void testCase3();
        // Test generator functions 'ggg', and 'gg'.

    static void testCase2();
        // Test primary manipulators.
};

template <class VALUE>
const bslstl::SimplePool<VALUE, bsl::allocator<VALUE> >&
TestDriver<VALUE>::init(Obj   *result,
                        Stack *usedBlocks,
                        Stack *freeBlocks,
                        int    numAllocs,
                        int    numDealloc)
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(usedBlocks);
    BSLS_ASSERT(freeBlocks);
    BSLS_ASSERT(numDealloc <= numAllocs);

    for (int i = 0; i < numAllocs; ++i) {
        VALUE *ptr = result->allocate();
        usedBlocks->push(ptr);
    }

    for (int i = 0; i < numDealloc; ++i) {
        void *ptr = usedBlocks->top();
        result->deallocate(ptr);
        freeBlocks->push(ptr);
        usedBlocks->pop();
    }

    return *result;
}

template <class VALUE>
void TestDriver<VALUE>::createFreeBlocks(Obj *result, int numBlocks)
{
    // Create some free blocks

    Stack blocks;

    // Allocate blocks.

    for (int i = 0; i < numBlocks; ++i) {
        VALUE *ptr = result->allocate();
        blocks.push(ptr);
    }

    // Use up all the free blocks.

    while (!expectToAllocate(blocks.size() + 1)) {
        VALUE *ptr = result->allocate();
        blocks.push(ptr);
    }

    // Free up the necessary number of blocks.

    for (int i = 0; i < numBlocks; ++i) {
        result->deallocate(blocks.top());
        blocks.pop();
    }
}

template<class VALUE>
void TestDriver<VALUE>::testCase9()
{
    // ------------------------------------------------------------------------
    // CONCERN: Standard allocator can be used
    //
    // Concerns:
    //: 1 The object can be created from a standard compliant allocator.
    //:
    //: 2 Memory allocated is correctly aligned.
    //
    // Plan:
    //: 1 Create an 'bslstl::SimplePool' using a standard compliant allocator
    //:   with minimal features.  (C-1)
    //:
    //: 2 Allocate some memory and verify the blocks are memory aligned.  (C-2)
    //
    // Testing:
    //   CONCERN: Standard allocator can be used
    // ------------------------------------------------------------------------
    typedef typename bslstl::SimplePool<VALUE, StlAlloc> Obj;

    StlAlloc A;
    Obj mX(A);

    ASSERTV(StlAlloc() == StlAlloc(mX.allocator()));

    for (int ti = 0; ti < 32; ++ti) {
        VALUE *ptr = mX.allocate();

        memset(ptr, 0xFF, sizeof(VALUE));
        std::size_t address = reinterpret_cast<std::size_t>(ptr);
        ASSERTV(ti, 0 == address % bsls::AlignmentFromType<VALUE>::VALUE);
        ASSERTV(ti, 0 == address % bsls::AlignmentFromType<void *>::VALUE);
    }
}

template<class VALUE>
void TestDriver<VALUE>::testCase8()
{
    // ------------------------------------------------------------------------
    // MANIPULATOR 'swap'
    //
    // Concerns:
    //: 1 'swap' exchange the free list and chunk list of the objects.
    //:
    //: 2 The common object allocator address held by both objects is
    //:   unchanged.
    //:
    //: 3 No memory is allocated from any allocator.
    //:
    //: 4 Swapping an object with itself does not affect the value of the
    //:   object (alias-safety).
    //:
    //: 5 Memory is deallocated on the destruction of the object.
    //:
    //: 6 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Using a table-based approach:
    //:
    //:   1 Create two objects of which memory has been allocated and
    //:     deallocated various number of times.
    //:
    //:   2 Swap the two objects, verify allocator is not changed.  (C-2)
    //:
    //:   3 Verify no memory is allocated (C-3)
    //:
    //:   4 Verify the free list of the objects have been swapped by calling
    //:     'allocate and checking the address of the allocated memory blocks.
    //:
    //:   5 Delete one of the objects and verify the memory of the other have
    //:     not been deallocated.  (C-1, 5)
    //:
    //:   6 Swap an object with itself and verify the object is unchanged.
    //:     (C-4)
    //:
    //: 2 Verify that, in appropriate build modes, defensive checks are
    //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
    //
    // Testing:
    //   void swap(bslstl_SimplePool& other);
    // ------------------------------------------------------------------------

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    struct {
        int d_line;
        int d_numAlloc;
        int d_numDealloc;
    } DATA[] = {

    //LINE  ALLOC  DEALLOC
    //----  -----  -------

    { L_,       0,       0 },
    { L_,       1,       0 },
    { L_,       1,       1 },
    { L_,       2,       0 },
    { L_,       2,       1 },
    { L_,       2,       2 },
    { L_,       3,       0 },
    { L_,       3,       1 },
    { L_,       3,       2 },
    { L_,       3,       3 },
    { L_,       4,       0 },
    { L_,       4,       1 },
    { L_,       4,       2 },
    { L_,       4,       3 },
    { L_,       4,       4 }

    };
    int NUM_DATA = sizeof DATA / sizeof *DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int LINE1     = DATA[ti].d_line;
        const int ALLOCS1   = DATA[ti].d_numAlloc;
        const int DEALLOCS1 = DATA[ti].d_numDealloc;

        for (int tj = 0; tj < NUM_DATA; ++tj) {
            const int LINE2     = DATA[tj].d_line;
            const int ALLOCS2   = DATA[tj].d_numAlloc;
            const int DEALLOCS2 = DATA[tj].d_numDealloc;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Stack usedX;
            Stack freeX;
            Obj mX(&oa);
            const Obj& X = init(&mX, &usedX, &freeX, ALLOCS1, DEALLOCS1);

            Stack usedY;
            Stack freeY;
            {
                Obj mY(&oa);
                const Obj& Y = init(&mY, &usedY, &freeY, ALLOCS2, DEALLOCS2);

                if (veryVerbose) { T_ P_(LINE1) P(LINE2) }

                bslma::TestAllocatorMonitor oam(&oa);

                mX.swap(mY);

                ASSERTV(LINE1, LINE2, &oa == X.allocator());
                ASSERTV(LINE1, LINE2, &oa == Y.allocator());
                ASSERTV(LINE1, LINE2, oam.isTotalSame());
                ASSERTV(LINE1, LINE2, oam.isInUseSame());

                // Verify the free lists are swapped

                while(!freeX.empty()) {
                    VALUE *ptr = mY.allocate();
                    ASSERTV(LINE1, LINE2, freeX.top() == ptr);
                    freeX.pop();
                    usedX.push(ptr);
                }

                while(!freeY.empty()) {
                    VALUE *ptr = mX.allocate();
                    ASSERTV(LINE1, LINE2, freeY.top() == ptr);
                    freeY.pop();
                    usedY.push(ptr);
                }
            }

            // 'Y' is now destroyed, its blocks should be deallocated.  Verify
            // Blocks in 'X' (which used to be in 'Y' before the swap) is not
            // deallocated.

            char SCRIBBLED_MEMORY[sizeof(VALUE)];
            memset(SCRIBBLED_MEMORY, 0xA5, sizeof(VALUE));
            while (!usedY.empty()) {
                char *ptr = (char *)usedY.top();
                ASSERTV(0 != strncmp(ptr, SCRIBBLED_MEMORY, sizeof(VALUE)));
                usedY.pop();
            }
        }

        if (veryVerbose) printf("Test alias safety");
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Stack usedX;
            Stack freeX;
            Obj mZ(&oa);
            init(&mZ, &usedX, &freeX, ALLOCS1, DEALLOCS1);
            Obj& mX = mZ;  const Obj& X = mX;

            if (veryVerbose) { T_ P(LINE1) }

            bslma::TestAllocatorMonitor oam(&oa);

            mX.swap(mZ);

            ASSERTV(LINE1, &oa == X.allocator());
            ASSERTV(LINE1, oam.isTotalSame());
            ASSERTV(LINE1, oam.isInUseSame());

            // Verify the free list have not change

            while(!freeX.empty()) {
                VALUE *ptr = mX.allocate();
                ASSERTV(LINE1, freeX.top() == ptr);
                freeX.pop();
                usedX.push(ptr);
            }
        }
    }

    // Verify no memory is allocated from the default allocator.

    ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        if (veryVerbose) printf("\t'swap' member function\n");
        {
            bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
            bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

            Obj mA(&oa1);  Obj mB(&oa1);
            Obj mZ(&oa2);

            ASSERT_SAFE_PASS(mA.swap(mB));
            ASSERT_SAFE_FAIL(mA.swap(mZ));
        }
    }
}

template<class VALUE>
void TestDriver<VALUE>::testCase7()
{
    // ------------------------------------------------------------------------
    // MANIPULATOR 'release'
    //
    // Concerns:
    //: 1 'release' deallocate all memory whether it was in the free list or
    //:   used list.
    //:
    //: 2 No temporary memory is allocated.
    //:
    //: 3 No free memory blocks is available after a 'release'.  i.e.,
    //:   subsequent 'allocate' will need to allocate memory from the heap.
    //
    // Plan:
    //: 1 Invoke 'allocate' and 'deallocate' various number of time.
    //:
    //:   1 Call 'release' and verify all memory is deallocated.  (C-1..2)
    //:
    //:   2 Call 'allocate' and verify memory is allocated from the heap.
    //:     (C-3)
    //
    // Testing:
    //   void release();
    // ------------------------------------------------------------------------

    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard dag(&da);

    struct {
        int d_line;
        int d_numAlloc;
        int d_numDealloc;
    } DATA[] = {

    //LINE  ALLOC  DEALLOC
    //----  -----  -------

    { L_,       0,       0 },
    { L_,       1,       0 },
    { L_,       1,       1 },
    { L_,       2,       0 },
    { L_,       2,       1 },
    { L_,       2,       2 },
    { L_,       3,       0 },
    { L_,       3,       1 },
    { L_,       3,       2 },
    { L_,       3,       3 },
    { L_,       4,       0 },
    { L_,       4,       1 },
    { L_,       4,       2 },
    { L_,       4,       3 },
    { L_,       4,       4 }

    };
    int NUM_DATA = sizeof DATA / sizeof *DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int LINE     = DATA[ti].d_line;
        const int ALLOCS   = DATA[ti].d_numAlloc;
        const int DEALLOCS = DATA[ti].d_numDealloc;

        bslma::TestAllocator oa("object", veryVeryVeryVerbose);

        Stack usedX;
        Stack freeX;

        Obj mX(&oa);
        init(&mX, &usedX, &freeX, ALLOCS, DEALLOCS);

        bslma::TestAllocatorMonitor oam(&oa);

        mX.release();

        ASSERTV(LINE, oam.isTotalSame());
        ASSERTV(LINE, 0 == oa.numBlocksInUse());

        mX.allocate();

        ASSERTV(LINE, oam.isTotalUp());
        ASSERTV(LINE, 1 == oa.numBlocksInUse());

    }

    // Verify no memory is allocated from the default allocator.

    ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
}

template<class VALUE>
void TestDriver<VALUE>::testCase6()
{
    // ------------------------------------------------------------------------
    // MANIPULATOR 'reserve'
    //
    // Concerns:
    //: 1 'reserve' allocate exactly the specified number of blocks such that
    //:   subsequent 'allocate' does not get memory from the heap.
    //:
    //: 2 Free blocks that was allocated before 'reserve' is not destroyed.
    //:
    //: 3 All memory allocation comes from the object allocator.
    //:
    //: 4 Memory is deallocated on the destruction of the object.
    //
    // Plan:
    //: 1 For each different values of i from 1 to 7:
    //:
    //:   1 For each different values of j from 0 to 7:
    //:
    //:     1 Create 'j' memory blocks in the free list.
    //:
    //:     2 Call 'reserve' for 'i' blocks.
    //:
    //:     3 Invoke 'allocate' 'i + j' times, and verify no memory is
    //:       allocated.
    //:
    //:     4 Invoke 'allocate' again and verify memory is allocated from the
    //:       heap.  (C-1..3)
    //:
    //: 2 Verify all memory is deallocated on destruction.  (C-4)
    //
    // ------------------------------------------------------------------------

    if (verbose) printf("\nMANIPULATOR 'reserve'"
                        "\n======================\n");

    for (int ti = 1; ti < 8; ++ti) {
        for(int tj = 0; tj < 8; ++tj) {
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator da("default", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            {
                Obj mX(&oa);

                createFreeBlocks(&mX, tj);

                if (veryVerbose) printf("'reserve'\n");
                {
                    bslma::TestAllocatorMonitor oam(&oa);
                    mX.reserve(ti);
                    ASSERTV(oam.numBlocksInUseChange(),
                            1 == oam.numBlocksInUseChange());
                }

                if (veryVerbose) printf("Use up free blocks.\n");
                {
                    bslma::TestAllocatorMonitor oam(&oa);
                    for (int tk = 0; tk < ti + tj; ++tk) {
                        mX.allocate();
                        ASSERTV(ti, tj, tk, oam.isTotalSame());
                        ASSERTV(ti, tj, tk, oam.isInUseSame());
                    }
                    mX.allocate();
                    ASSERTV(ti, tj, oam.isInUseUp());
                }
            }

            ASSERTV(oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
            ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());
        }
    }

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        if (veryVerbose) printf("\t'reseve'\n");
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);

            ASSERT_SAFE_FAIL(mX.reserve(0));
            ASSERT_SAFE_PASS(mX.reserve(1));
        }
    }
}


template<class VALUE>
void TestDriver<VALUE>::testCase5()
{
    // ------------------------------------------------------------------------
    // MANIPULATOR 'deallocate'
    //
    // Concerns:
    //: 1 Deallocating returns the memory to a free list.
    //:
    //: 2 'allocate' does not allocate from the heap when there are still
    //:   blocks in the free list.
    //:
    //: 3 'allocate' retrieve the last block that was deallocated.
    //:
    //: 4 'allocate' will retrieve memory from the heap if the free list is
    //:   empty.
    //:
    //: 5 'deallocate' does not allocate or release any memory.
    //:
    //: 6 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create a list of sequences to allocate and deallocate memory.  For
    //:   each sequence:
    //:
    //:   1 Invoke 'allocate' and 'deallocate' according to the sequence.
    //:
    //:   2 Verify that each allocate returns the last block that was
    //:     deallocated if 'deallocate' was called.  (C-1..3)
    //:
    //:   3 Verify no memory was allocated from the heap on 'deallocate'.
    //:     (C-5)
    //:
    //:   4 Verify 'allocate' will get memory from the heap only when expected.
    //:
    //: 2 Verify that, in appropriate build modes, defensive checks are
    //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
    // ------------------------------------------------------------------------

    if (verbose) printf("\nMANIPULATOR 'deallocate'"
                        "\n========================");

    struct {
        int         d_line;
        const char *d_sequence;
    } DATA[] = {

    //LINE  SEQUENCE
    //----  --------
    { L_,   "ADA" },
    { L_,   "AADDAA" },
    { L_,   "ADADA" },
    { L_,   "ADAADDAAA" },
    { L_,   "AAADDDAAA" },
    { L_,   "AADADA" }

    };
    int NUM_DATA = sizeof DATA / sizeof *DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE     = DATA[ti].d_line;
        const char *const SEQUENCE = DATA[ti].d_sequence;
        const int         LENGTH   = (int)strlen(SEQUENCE);

        Stack usedBlocks;
        Stack freeBlocks;

        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(&oa);

        for (int tj = 0; tj < LENGTH; ++tj) {
            bslma::TestAllocatorMonitor oam(&oa);

            if (SEQUENCE[tj] == 'A') {
                VALUE *ptr = mX.allocate();

                usedBlocks.push(ptr);

                if (!freeBlocks.empty()) {
                    ASSERTV(LINE, tj, freeBlocks.top() == ptr);
                    freeBlocks.pop();
                }
                else {
                    if (expectToAllocate(usedBlocks.size())) {
                        ASSERTV(LINE, tj, oam.isTotalUp());
                        ASSERTV(LINE, tj, oam.isInUseUp());
                    }
                    else {
                        ASSERTV(LINE, tj, oam.isTotalSame());
                        ASSERTV(LINE, tj, oam.isInUseSame());
                    }
                }
            }
            else {
                ASSERTV(LINE, !usedBlocks.empty());
                void *ptr = usedBlocks.top();

                mX.deallocate(ptr);
                freeBlocks.push(ptr);
                usedBlocks.pop();

                ASSERTV(LINE, tj, oam.isTotalSame());
                ASSERTV(LINE, tj, oam.isInUseSame());
            }
        }
        ASSERTV(LINE, 0 == da.numBlocksTotal());
    }

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        if (veryVerbose) printf("\t'allocate' member function\n");
        {
            Obj mX(0);

            VALUE *ptr = mX.allocate();

            ASSERT_SAFE_FAIL(mX.deallocate(0));
            ASSERT_SAFE_PASS(mX.deallocate(ptr));
        }
    }
}

template<class VALUE>
void TestDriver<VALUE>::testCase4()
{
    // ------------------------------------------------------------------------
    // BASIC ACCESSORS
    //
    // Concerns:
    //: 1 'allocator' returns the allocator that was supplied on construction.
    //:
    //: 2 The accessor is declared 'const'.
    //:
    //: 3 The accessor does not allocate any memory from any allocator.
    //
    // Plan:
    //: 1 For each allocator configuration:
    //:
    //:   1 Create a 'bslstl_SimplePool' with an allocator.
    //:
    //:   2 Use the basic accessor to verify the allocator is installed
    //:     properly.  (C-1..2)
    //:
    //:   3 Verify no memory is allocated from any allocator.  (C-3)
    //
    // Testing:
    //   AllocatorType& allocator();
    // ------------------------------------------------------------------------

    for (char cfg = 'a'; cfg <= 'c'; ++cfg) {
        const char CONFIG = cfg;

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
        bslma::TestAllocator sa1("supplied1",  veryVeryVeryVerbose);
        bslma::TestAllocator sa2("supplied2",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj                 *objPtr;
        bslma::TestAllocator *objAllocatorPtr;

        switch (CONFIG) {
          case 'a': {
              objPtr = new (fa) Obj(0);
              objAllocatorPtr = &da;
          } break;
          case 'b': {
              objPtr = new (fa) Obj(&sa1);
              objAllocatorPtr = &sa1;
          } break;
          case 'c': {
              objPtr = new (fa) Obj(&sa2);
              objAllocatorPtr = &sa2;
          } break;
          default: {
              ASSERTV(CONFIG, !"Bad allocator config.");
              return;                                                 // RETURN
          } break;
        }

        Obj& mX = *objPtr;  const Obj& X = mX;
        bslma::TestAllocator& oa = *objAllocatorPtr;

        // --------------------------------------------------------

        // Verify basic accessor

        bslma::TestAllocatorMonitor oam(&oa);

        ASSERTV(CONFIG, &oa == X.allocator());

        ASSERT(oam.isTotalSame());

        // --------------------------------------------------------

        // Reclaim dynamically allocated object under test.

        fa.deleteObject(objPtr);

        // Verify all memory is released on object destruction.

        ASSERTV(CONFIG, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        ASSERTV(CONFIG, fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
        ASSERTV(CONFIG, sa1.numBlocksInUse(), 0 == sa1.numBlocksInUse());
        ASSERTV(CONFIG, sa2.numBlocksInUse(), 0 == sa2.numBlocksInUse());
    }
}

template<class VALUE>
void TestDriver<VALUE>::testCase3()
{
    // ------------------------------------------------------------------------
    // RESERVED FOR TEST APPARATUS TESTING
    // ------------------------------------------------------------------------
}

template<class VALUE>
void TestDriver<VALUE>::testCase2()
{
    // ------------------------------------------------------------------------
    // CTOR, PRIMARY MANIPULATORS, & DTOR
    //   Ensure that we can use the default constructor to create an
    //   object (having the default-constructed value), use the primary
    //   manipulators to put that object into any state relevant for
    //   thorough testing, and use the destructor to destroy it safely.
    //
    // Concerns:
    //: 1 An object created with the constructor has the specified
    //:   allocator.
    //:
    //: 2 Any memory allocation is from the object allocator.
    //:
    //: 3 There is no temporary allocation from any allocator.
    //:
    //: 4 Every object releases any allocated memory at destruction.
    //:
    //: 5 Pointer returned by 'allocate' is aligned correctly.
    //:
    //: 6 Allocation starts at one block, up to a maximum of 32 blocks.
    //:
    //: 7 Every allocation allocate contiguous blocks.
    //:
    //: 8 Constructor allocates no memory.
    //:
    //: 9 Any memory allocation is exception neutral.
    //
    // Plan:
    //: 1 For each allocator configuration:
    //:
    //:   1 Create a pool object and verify no memory is allocated.  (C-1, 8)
    //:
    //:   2 Call 'allocate' 96 times in the presence of exception, for each
    //:     time:
    //:
    //:     1 Verify memory is only allocated from object allocator and only
    //:       when expected.  (C-2..3, 6, 9)
    //:
    //:     2 Verify address returned is aligned.  (C-5)
    //:
    //:     3 If memory is not allocated, the address is the max of
    //:       'sizeof(VALUE)' and 'sizeof(void *) larger than the previous
    //:       address.  (C-7)
    //:
    //:   3 Delete the object and verify all memory is deallocated.  (C-4)
    //
    // Testing:
    //   explicit SimplePool(const ALLOCATOR& allocator);
    //   ~SimplePool();
    //   VALUE *allocate();
    // ------------------------------------------------------------------------

    if (verbose) printf(
                 "\nDEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR"
                 "\n==========================================\n");

    if (verbose) printf("\nTesting with various allocator configurations.\n");

    for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

        const char CONFIG = cfg;  // how we specify the allocator

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj                 *objPtr;
        bslma::TestAllocator *objAllocatorPtr;

        switch (CONFIG) {
          case 'a': {
            objPtr = new (fa) Obj(0);
            objAllocatorPtr = &da;
          } break;
          case 'b': {
            objPtr = new (fa) Obj(&sa);
            objAllocatorPtr = &sa;
          } break;
          default: {
            ASSERTV(CONFIG, !"Bad allocator config.");
            return;                                                   // RETURN
          } break;
        }

        Obj&                   mX = *objPtr;  const Obj& X = mX;
        bslma::TestAllocator&  oa = *objAllocatorPtr;
        bslma::TestAllocator& noa = 'b' != CONFIG ? sa : da;

        // ---------------------------------------
        // Verify allocator is installed properly.
        // ---------------------------------------

        ASSERTV(CONFIG, &oa == X.allocator());

        // Verify no allocation from the object/non-object allocators.

        ASSERTV(CONFIG,  oa.numBlocksTotal(), 0 ==  oa.numBlocksTotal());
        ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

        VALUE *prevPtr = 0;
        for (int ti = 0; ti < 96; ++ti) {
            bslma::TestAllocatorMonitor oam(&oa);

            VALUE *ptr;
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                ptr = mX.allocate();
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            if (expectToAllocate(ti + 1)) {
                ASSERTV(oam.isInUseUp());
            }
            else {
                ptrdiff_t offset = sizeof(VALUE);
                offset += bsls::AlignmentUtil::calculateAlignmentOffset(
                                       (void *) offset,
                                       bsls::AlignmentFromType<void *>::VALUE);
                ASSERTV(oam.isTotalSame());
                ASSERTV(prevPtr, ptr, (char *)prevPtr + offset == (char*)ptr);
            }
            memset(ptr, 0xff, sizeof(VALUE));

            std::size_t address = reinterpret_cast<std::size_t>(ptr);
            ASSERTV(ti, 0 == address % bsls::AlignmentFromType<VALUE>::VALUE);
            ASSERTV(ti, 0 == address % bsls::AlignmentFromType<void *>::VALUE);

            prevPtr = ptr;
        }

        // Verify no temporary memory is allocated from the object
        // allocator.

        ASSERTV(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                oa.numBlocksTotal() == oa.numBlocksInUse());

        // Reclaim dynamically allocated object under test.

        fa.deleteObject(objPtr);

        // Verify all memory is released on object destruction.

        ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
        ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
        ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
    }

}

}  // close unnamed namespace

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use for this component.
//
///Example 1: Creating a Node-Based Stack
/// - - - - - - - - - - - - - - - - - - -
// Suppose that we want to implement a stack with a linked list.  It is
// expensive to allocate memory every time a node is inserted.  Therefore, we
// can use 'SimplePool' to efficiently manage the memory for the list.
//
// First, we define the class that implements the stack:
//..
    template <class ALLOCATOR = bsl::allocator<int> >
    class my_Stack {
        // This class defines a node-based stack of integers.
//
        // PRIVATE TYPES
        struct Node {
            // This 'struct' implements a link data structure containing a
            // value and a pointer to the next node.
//
            int   d_value;   // payload value
            Node *d_next_p;  // pointer to the next node
        };
//
        typedef bslstl::SimplePool<Node, ALLOCATOR> Pool;
            // Alias for memory pool.
//
      private:
        // DATA
        Node *d_head_p;  // pointer to the first node
        int   d_size;    // size of the stack
        Pool  d_pool;    // memory manager for the stack
//
      public:
        // CREATORS
        my_Stack(const ALLOCATOR& allocator = ALLOCATOR());
            // Create an empty 'my_Stack' object.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.
//
        // MANIPULATORS
        void push(int value);
            // Insert an element with the specified value to the top of this
            // stack.
//
        void pop();
            // Remove the top element from this stack.  The behavior is
            // undefined unless '1 <= size()'.
//
        // ACCESSORS
        int top();
            // Return the value of the element on the top of this stack.  The
            // behavior is undefined unless '1 <= size()'.
//
        std::size_t size();
            // Return the number of elements in this stack.
    };
//..
// Now, we define the implementation of the stack.  Notice how
// 'bslstl::SimplePool' is used to allocate memory in 'push' and deallocate
// memory in 'pop':
//..
    // CREATORS
    template <class ALLOCATOR>
    my_Stack<ALLOCATOR>::my_Stack(const ALLOCATOR& allocator)
    : d_head_p(0)
    , d_size(0)
    , d_pool(allocator)
    {
    }
//
    // MANIPULATORS
    template <class ALLOCATOR>
    void my_Stack<ALLOCATOR>::push(int value)
    {
        Node *newNode = d_pool.allocate();
//
        newNode->d_value = value;
        newNode->d_next_p = d_head_p;
        d_head_p = newNode;
//
        ++d_size;
    }
//
    template <class ALLOCATOR>
    void my_Stack<ALLOCATOR>::pop()
    {
        BSLS_ASSERT(0 != size());
//
        Node *n = d_head_p;
        d_head_p = d_head_p->d_next_p;
        d_pool.deallocate(n);
        --d_size;
    }
//
    // ACCESSORS
    template <class ALLOCATOR>
    int my_Stack<ALLOCATOR>::top()
    {
        BSLS_ASSERT(0 != size());
//
        return d_head_p->d_value;
    }
//
    template <class ALLOCATOR>
    std::size_t my_Stack<ALLOCATOR>::size()
    {
        return d_size;
    }
//..

//=============================================================================
//                                 MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int  test = argc > 1 ? atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;
    veryVeryVeryVerbose = argc > 5;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:
      case 10: {
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

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Finally, we test our stack by pushing and popping some elements:
//..
    my_Stack<> stack;
    stack.push(1);
    stack.push(2);
    stack.push(3);
    stack.push(4);
    stack.push(5);
    ASSERT(5 == stack.size());

    ASSERT(5 == stack.top());
    stack.pop();
    ASSERT(4 == stack.top());
    stack.pop();
    ASSERT(3 == stack.top());
    stack.pop();
    ASSERT(2 == stack.top());
    stack.pop();
    ASSERT(1 == stack.top());
    stack.pop();

    ASSERT(0 == stack.size());
//..

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // ALIGNMENT TEST
        //
        // Concern:
        //  Every byte of an allocated block is usable and aligned correctly.
        //
        // --------------------------------------------------------------------
          RUN_EACH_TYPE(TestDriver, testCase9, TEST_TYPES);
      } break;
      case 8: {
          RUN_EACH_TYPE(TestDriver, testCase8, TEST_TYPES);
      } break;
      case 7: {
          RUN_EACH_TYPE(TestDriver, testCase7, TEST_TYPES);
      } break;
      case 6: {
          RUN_EACH_TYPE(TestDriver, testCase6, TEST_TYPES);
      } break;
      case 5: {
          RUN_EACH_TYPE(TestDriver, testCase5, TEST_TYPES);
      } break;
      case 4: {
          RUN_EACH_TYPE(TestDriver, testCase4, TEST_TYPES);
      } break;
      case 3: {
          RUN_EACH_TYPE(TestDriver, testCase3, TEST_TYPES);
      } break;
      case 2: {
          RUN_EACH_TYPE(TestDriver, testCase2, TEST_TYPES);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Plan:
        //   Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        {
            if (veryVerbose) {
                printf("\tTest int-node w/ bslma::Allocator\n");
            }

            typedef SimplePool<int, bsl::allocator<int> > Obj;

            bslma::TestAllocator da, ta;
            bslma::DefaultAllocatorGuard daGuard(&da);

            Obj x(&ta);
            int *value = (int *)x.allocate();
            ASSERTV(0 != value);
            ASSERTV(0 == da.numBlocksInUse());
            ASSERTV(1 == ta.numBlocksInUse());
            x.deallocate(value);
            ASSERTV(0 == da.numBlocksInUse());
            ASSERTV(1 == ta.numBlocksInUse());

            value = (int *)x.allocate();
            ASSERTV(0 != value);
            ASSERTV(0 == da.numBlocksInUse());
            ASSERTV(1 == ta.numBlocksInUse());
            value = (int *)x.allocate();
            ASSERTV(0 == da.numBlocksInUse());
            ASSERTV(2 == ta.numBlocksInUse());
            value = (int *)x.allocate();
            ASSERTV(0 == da.numBlocksInUse());
            ASSERTV(2 == ta.numBlocksInUse());
            value = (int *)x.allocate();
            ASSERTV(0 == da.numBlocksInUse());
            ASSERTV(3 == ta.numBlocksInUse());
        }
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
