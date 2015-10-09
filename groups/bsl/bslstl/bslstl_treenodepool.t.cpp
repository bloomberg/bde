// bslstl_treenodepool.t.cpp                                          -*-C++-*-
#include <bslstl_treenodepool.h>

#include <bslstl_allocator.h>

#include <bslalg_rbtreenode.h>
#include <bslalg_rbtreeanchor.h>
#include <bslalg_rbtreeutil.h>

#include <bslma_allocator.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_default.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsltf_templatetestfacility.h>
#include <bsltf_stdtestallocator.h>
#include <bsltf_testvaluesarray.h>

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
//
// Global Concerns:
//: o Pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated.
//: o Precondition violations are detected in appropriate build modes.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] explicit TreeNodePool(const ALLOCATOR& allocator);
//
// MANIPULATORS
// [ 4] AllocatorType& allocator();
// [ 2] bslalg::RbTreeNode *createNode();
// [ 7] bslalg::RbTreeNode *createNode(const bslalg::RbTreeNode& original);
// [ 7] bslalg::RbTreeNode *createNode(const VALUE& value);
// [ 5] void deleteNode(bslalg::RbTreeNode *node);
// [ 6] void reserveNodes(std::size_t numNodes);
// [ 8] void swap(TreeNodePool<VALUE, ALLOCATOR>& other);
//
// ACCESSORS
// [ 4] const AllocatorType& allocator() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
//-----------------------------------------------------------------------------
//=============================================================================

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
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslalg::RbTreeNode RbNode;

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

}  // close unnamed namespace

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

class AllocatingIntType {
    // DATA
    bslma::Allocator *d_allocator_p;
    int             *d_value_p;

  private:
    // NOT IMPLEMENTED
    AllocatingIntType(const AllocatingIntType&);
  public:

    // CREATORS
    AllocatingIntType(bslma::Allocator *allocator)
    : d_allocator_p(bslma::Default::allocator(allocator))
    {
        d_value_p  = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
        *d_value_p = 0xabcd;
    }

    AllocatingIntType(const AllocatingIntType&  original,
                      bslma::Allocator          *allocator)
    : d_allocator_p(bslma::Default::allocator(allocator))
    {
        d_value_p  = static_cast<int *>(d_allocator_p->allocate(sizeof(int)));
        *d_value_p = *original.d_value_p;
    }

    ~AllocatingIntType()
    {
        BSLS_ASSERT(0 != d_value_p);
        d_allocator_p->deleteObject(d_value_p);
    }

    int& value() { return *d_value_p; }

    const int& value() const { return *d_value_p; }
};

namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<AllocatingIntType> : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

//=============================================================================
//                               TEST FACILITIES
//-----------------------------------------------------------------------------

class Stack
{
    enum { CAPACITY = 128 };
    RbNode  *d_data[CAPACITY];
    int      d_size;

  public:
    // CREATORS
    Stack() : d_size(0) {}

    Stack(const Stack& original)
    {
        d_size = original.d_size;
        memcpy(d_data, original.d_data, d_size * sizeof(*d_data));
    }

    // MANIPULATORS
    void push(RbNode *value)
    {
        BSLS_ASSERT(CAPACITY != d_size);

        d_data[d_size] = value;
        ++d_size;
    }

    void pop()
    {
        BSLS_ASSERT(0 != d_size);

        --d_size;
    }

    // ACCESSORS
    bool empty() { return 0 == d_size; }

    int size() { return d_size; }

    RbNode *back()
    {
        BSLS_ASSERT(0 != d_size);

        return d_data[d_size - 1];
    }

    RbNode *operator[](size_t index)
    {
        return d_data[index];
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
    typedef bslstl::TreeNodePool<VALUE, bsl::allocator<VALUE> > Obj;
        // Type under testing.

    typedef bsltf::StdTestAllocator<VALUE> StlAlloc;

    typedef bslstl::TreeNode<VALUE> ValueNode;

  private:
    // PRIVATE CLASS METHODS
    static
    const Obj& init(Obj   *result,
                    Stack *usedBlocks,
                    Stack *freeBlocks,
                    int    numAllocs,
                    int    numDealloc);

    static
    void createFreeBlocks(Obj *result, Stack *usedBlocks, int numBlocks);

  public:
    // TEST CASES
    // static void testCase10();
        // Reserved for BSLX.

    static void testCase9();
        // Test usage example.

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
const bslstl::TreeNodePool<VALUE, bsl::allocator<VALUE> >&
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
        RbNode *ptr = result->createNode();
        usedBlocks->push(ptr);
    }

    for (int i = 0; i < numDealloc; ++i) {
        RbNode *ptr = usedBlocks->back();
        result->deleteNode(ptr);
        freeBlocks->push(ptr);
        usedBlocks->pop();
    }

    return *result;
}

template <class VALUE>
void TestDriver<VALUE>::createFreeBlocks(Obj   *result,
                                         Stack *usedBlocks,
                                         int    numBlocks)
{
    // Allocate blocks.

    for (int i = 0; i < numBlocks; ++i) {
        RbNode *ptr = result->createNode();
        usedBlocks->push(ptr);
    }

    // Use up all the free blocks.

    while (!expectToAllocate(usedBlocks->size() + 1)) {
        RbNode *ptr = result->createNode();
        usedBlocks->push(ptr);
    }

    // Free up the necessary number of blocks.

    for (int i = 0; i < numBlocks; ++i) {
        result->deleteNode(usedBlocks->back());
        usedBlocks->pop();
    }
}

template<class VALUE>
void TestDriver<VALUE>::testCase8()
{
    // --------------------------------------------------------------------
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
    //   void swap(TreeNodePool<VALUE, ALLOCATOR>& other);
    // --------------------------------------------------------------------

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
                    RbNode *ptr = mY.createNode();
                    ASSERTV(LINE1, LINE2, freeX.back() == ptr);
                    freeX.pop();
                    usedX.push(ptr);
                }

                while(!freeY.empty()) {
                    RbNode *ptr = mX.createNode();
                    ASSERTV(LINE1, LINE2, freeY.back() == ptr);
                    freeY.pop();
                    usedY.push(ptr);
                }

                // Cleanup up memory used by the object in the node.

                while(!usedX.empty()) {
                    mX.deleteNode(usedX.back());
                    usedX.pop();
                }
            }

            // 'Y' is now destroyed, its blocks should be deallocated.  Verify
            // Blocks in 'X' (which used to be in 'Y' before the swap) is not
            // deallocated.

            char SCRIBBLED_MEMORY[sizeof(VALUE)];
            memset(SCRIBBLED_MEMORY, 0xA5, sizeof(VALUE));
            while (!usedY.empty()) {
                RbNode *ptr = usedY.back();
                ASSERTV(0 != strncmp((char *)ptr,
                                     SCRIBBLED_MEMORY,
                                     sizeof(VALUE)));

                mX.deleteNode(ptr);
                usedY.pop();
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
    // -----------------------------------------------------------------------
    // MANIPULATOR 'createNode'
    //
    // Concerns:
    //: 1 'createNode' invokes the copy constructor of the parameterized 'TYPE'
    //:
    //: 2 Any memory allocation is from the object allocator.
    //:
    //: 3 There is no temporary allocation from any allocator.
    //:
    //: 4 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Create an array of distinct object.  For each object in the array:
    //:
    //:   1 Invoke 'createNode' on the object.
    //:
    //:   2 Verify memory is allocated only when expected.  (C-2..3)
    //:
    //:   3 Verify the new node contains a copy of the object.
    //:
    //: 2 Create another 'TreeNodePool'.
    //:
    //: 3 For each node that was created:
    //:
    //:   1 Invoke 'createNode' on the node that was created previously
    //:
    //:   2 Verify the newly created node has the same value as the old one.
    //:     (C-1)
    //:
    //: 4 Verify all memory is released on destruction.  (C-4)
    //
    // Testing:
    //   bslalg::RbTreeNode *createNode(const bslalg::RbTreeNode& original);
    //   bslalg::RbTreeNode *createNode(const VALUE& value);
    // -----------------------------------------------------------------------

    if (verbose) printf("\nMANIPULATOR 'createNode'"
                        "\n========================\n");


    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<
                                    VALUE>::value;

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    {
        bsltf::TestValuesArray<VALUE> VALUES;

        Obj mX(&oa);

        Stack usedX;

        for (int i = 0; i < 16; ++i) {
            bslma::TestAllocatorMonitor oam(&oa);

            RbNode *ptr = mX.createNode(VALUES[i]);

            if (expectToAllocate(i + 1)) {
                ASSERTV(1 + TYPE_ALLOC == oam.numBlocksTotalChange());
                ASSERTV(1 + TYPE_ALLOC == oam.numBlocksInUseChange());
            }
            else {
                ASSERTV(TYPE_ALLOC == oam.numBlocksTotalChange());
                ASSERTV(TYPE_ALLOC == oam.numBlocksInUseChange());
            }
            usedX.push(ptr);

            ValueNode *node = static_cast<ValueNode *>(ptr);
            ASSERTV(i, VALUES[i] == node->value());
        }

        Obj mY(&oa);

        Stack usedY;

        for (int i = 0; i < 16; ++i) {
            bslma::TestAllocatorMonitor oam(&oa);

            RbNode *ptr = mY.createNode(*usedX[i]);

            if (expectToAllocate(i + 1)) {
                ASSERTV(1 + TYPE_ALLOC == oam.numBlocksTotalChange());
                ASSERTV(1 + TYPE_ALLOC == oam.numBlocksInUseChange());
            }
            else {
                ASSERTV(TYPE_ALLOC == oam.numBlocksTotalChange());
                ASSERTV(TYPE_ALLOC == oam.numBlocksInUseChange());
            }
            usedY.push(ptr);

            ValueNode *nodeY = static_cast<ValueNode *>(ptr);
            ASSERTV(i, VALUES[i] == nodeY->value());

            ValueNode *nodeX = static_cast<ValueNode *>(ptr);
            ASSERTV(i, nodeX->value() == nodeY->value());
        }

        while(!usedX.empty()) {
            mX.deleteNode(usedX.back());
            usedX.pop();
        }

        while(!usedY.empty()) {
            mY.deleteNode(usedY.back());
            usedY.pop();
        }
    }

    // Verify all memory is released on object destruction.

    ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
}

template<class VALUE>
void TestDriver<VALUE>::testCase6()
{
    // --------------------------------------------------------------------
    // MANIPULATOR 'reserveNodes'
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
    //:
    //: 5 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 For each different values of i from 1 to 7:
    //:
    //:   1 For each different values of j from 0 to 7:
    //:
    //:     1 Create 'j' memory blocks in the free list.
    //:
    //:     2 Call 'reserveNode' for 'i' blocks.
    //:
    //:     3 Invoke 'createNode' 'i + j' times, and verify no memory is
    //:       allocated.
    //:
    //:     4 Invoke 'createNode' again and verify memory is allocated from the
    //:       heap.  (C-1..3)
    //:
    //: 2 Verify all memory is deallocated on destruction.  (C-4)
    //:
    //: 3 Verify that, in appropriate build modes, defensive checks are
    //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-5)
    //
    // Testing:
    //   void reserveNodes(std::size_t numNodes);
    // --------------------------------------------------------------------

    if (verbose) printf("\nMANIPULATOR 'reserve'"
                        "\n======================\n");

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<
                                    VALUE>::value;

    for (int ti = 1; ti < 8; ++ti) {
        for(int tj = 0; tj < 8; ++tj) {
            bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
            bslma::TestAllocator da("default", veryVeryVeryVerbose);

            bslma::DefaultAllocatorGuard dag(&da);

            Obj mX(&oa);

            Stack usedBlocks;
            createFreeBlocks(&mX, &usedBlocks, tj);

            mX.reserveNodes(ti);

            for (int tk = 0; tk < ti + tj; ++tk) {
                bslma::TestAllocatorMonitor oam(&oa);
                usedBlocks.push(mX.createNode());
                ASSERTV(ti, tj, tk, TYPE_ALLOC == oam.numBlocksTotalChange());
                ASSERTV(ti, tj, tk, TYPE_ALLOC == oam.numBlocksInUseChange());
            }

            {
                bslma::TestAllocatorMonitor oam(&oa);
                usedBlocks.push(mX.createNode());
                ASSERTV(ti, tj, 1 + TYPE_ALLOC == oam.numBlocksInUseChange());
            }

            while(!usedBlocks.empty()) {
                mX.deleteNode(usedBlocks.back());
                usedBlocks.pop();
            }
        }
    }

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        if (veryVerbose) printf("\t'reserve'\n");
        {
            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            Obj mX(&oa);

            ASSERT_SAFE_FAIL(mX.reserveNodes(0));
            ASSERT_SAFE_PASS(mX.reserveNodes(1));
        }
    }
}


template<class VALUE>
void TestDriver<VALUE>::testCase5()
{
    // --------------------------------------------------------------------
    // MANIPULATOR 'deleteNode'
    //
    // Concerns:
    //: 1 'deleteNode' invokes the destructor of the value in the node.
    //:
    //: 2 'createNode' does not allocate from the heap when there are still
    //:   blocks in the free list.
    //:
    //: 3 'createNode' retrieve the last block that was deallocated.
    //:
    //: 4 'allocate' will retrieve memory from the heap once so that the next
    //:   allocation will not allocate from the heap.
    //:
    //: 5 'deleteNode' does not allocate or release any memory other than those
    //:   caused by the destructor of the value.
    //:
    //: 6 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create a list of sequences to allocate and deallocate memory.  For
    //:   each sequence:
    //:
    //:   1 Invoke 'createNode' and 'deleteNode' according to the sequence.
    //:
    //:   2 Verify that each allocate returns the last block that was
    //:     deallocated if 'deleteNode' was called.  (C-1..3)
    //:
    //:   3 Verify no memory was allocated from the heap on 'deleteNode'.
    //:     (C-5)
    //:
    //:   4 Verify 'createNode' will get memory from the heap only when
    //:     expected.
    //:
    //: 2 Verify that, in appropriate build modes, defensive checks are
    //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
    // --------------------------------------------------------------------

    if (verbose) printf("\nMANIPULATOR 'deleteNode'"
                        "\n========================");

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<
                                    VALUE>::value;

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
        const size_t      LENGTH   = strlen(SEQUENCE);

        Stack usedBlocks;
        Stack freeBlocks;

        bslma::TestAllocator oa("object",  veryVeryVeryVerbose);
        bslma::TestAllocator da("default", veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj mX(&oa);

        for (size_t tj = 0; tj < LENGTH; ++tj) {
            bslma::TestAllocatorMonitor oam(&oa);

            if (SEQUENCE[tj] == 'A') {
                RbNode *ptr = mX.createNode();

                usedBlocks.push(ptr);

                if (!freeBlocks.empty()) {
                    ASSERTV(LINE, tj, freeBlocks.back() == ptr);
                    freeBlocks.pop();
                }
                else {
                    if (expectToAllocate(usedBlocks.size())) {
                        ASSERTV(1 + TYPE_ALLOC == oam.numBlocksTotalChange());
                        ASSERTV(1 + TYPE_ALLOC == oam.numBlocksInUseChange());
                    }
                    else {
                        ASSERTV(TYPE_ALLOC == oam.numBlocksTotalChange());
                        ASSERTV(TYPE_ALLOC == oam.numBlocksInUseChange());
                    }
                }
            }
            else {
                ASSERTV(LINE, !usedBlocks.empty());
                RbNode *ptr = usedBlocks.back();

                mX.deleteNode(ptr);
                freeBlocks.push(ptr);
                usedBlocks.pop();

                ASSERTV(LINE, tj, oam.isTotalSame());
                ASSERTV(LINE, tj,
                        -TYPE_ALLOC == oam.numBlocksInUseChange());
            }
        }
        ASSERTV(LINE, 0 == da.numBlocksTotal());

        // Cleanup up memory used by the object in the node.

        while(!usedBlocks.empty()) {
            mX.deleteNode(usedBlocks.back());
            usedBlocks.pop();
        }
    }

    if (verbose) printf("\nNegative Testing.\n");
    {
        bsls::AssertFailureHandlerGuard hG(bsls::AssertTest::failTestDriver);

        if (veryVerbose) printf("\t'allocate' member function\n");
        {
            Obj mX(0);

            RbNode *ptr = mX.createNode();

            ASSERT_SAFE_FAIL(mX.deleteNode(0));
            ASSERT_SAFE_PASS(mX.deleteNode(ptr));
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
    //:   1 Create a 'bslstl_TreeNodePool' with an allocator.
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
    // --------------------------------------------------------------------
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
    //: 5 Allocation starts at one block, up to a maximum of 32 blocks.
    //:
    //: 6 Constructor allocates no memory.
    //:
    //: 7 Any memory allocation is exception neutral.
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
    //:     2 If memory is not allocated, the address is the max of
    //:       'sizeof(VALUE)' and 'sizeof(void *) larger than the previous
    //:       address.  (C-7)
    //:
    //:   3 Delete the object and verify all memory is deallocated.  (C-4)
    //
    // Testing:
    //   explicit TreeNodePool(const ALLOCATOR& allocator);
    //   ~TreeNodePool();
    //   VALUE *createNode();
    // --------------------------------------------------------------------

    if (verbose) printf(
                 "\nDEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR"
                 "\n==========================================\n");

    if (verbose) printf("\nTesting with various allocator configurations.\n");

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

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

        Obj&                  mX = *objPtr;  const Obj& X = mX;
        bslma::TestAllocator&  oa = *objAllocatorPtr;
        bslma::TestAllocator& noa = 'b' != CONFIG ? sa : da;

        // ---------------------------------------
        // Verify allocator is installed properly.
        // ---------------------------------------

        ASSERTV(CONFIG, &oa == X.allocator());

        // Verify no allocation from the object/non-object allocators.

        ASSERTV(CONFIG,  oa.numBlocksTotal(), 0 ==  oa.numBlocksTotal());
        ASSERTV(CONFIG, noa.numBlocksTotal(), 0 == noa.numBlocksTotal());

        Stack usedBlocks;

        for (int i = 0; i < 96; ++i) {
            bslma::TestAllocatorMonitor oam(&oa);

            RbNode *ptr = mX.createNode();

            if (expectToAllocate(i + 1)) {
                ASSERTV(1 + TYPE_ALLOC == oam.numBlocksTotalChange());
                ASSERTV(1 + TYPE_ALLOC == oam.numBlocksInUseChange());
            }
            else {
                ASSERTV(TYPE_ALLOC == oam.numBlocksTotalChange());
                ASSERTV(TYPE_ALLOC == oam.numBlocksInUseChange());
            }
            usedBlocks.push(ptr);
        }

        // Verify no temporary memory is allocated from the object
        // allocator.

        ASSERTV(CONFIG, oa.numBlocksTotal(), oa.numBlocksInUse(),
                oa.numBlocksTotal() == oa.numBlocksInUse());

        // Free up used blocks.
        for (int i = 0; i < 96; ++i) {
            bslma::TestAllocatorMonitor oam(&oa);

            mX.deleteNode(usedBlocks.back());

            ASSERTV(-TYPE_ALLOC == oam.numBlocksInUseChange());

            usedBlocks.pop();
        }

        // Reclaim dynamically allocated object under test.

        fa.deleteObject(objPtr);

        // Verify all memory is released on object destruction.

        ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
        ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
        ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
    }
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a 'IntSet' Container
/// - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create a container type, 'IntSet' using
// 'bslalg::RbTreeUtil'.
//
// First, we define a comparison functor for comparing a
// 'bslstl::RbTreeNode<int>' object and an 'int' value.  This functor conforms
// to the requirements of 'bslalg::RbTreeUtil':
//..
    struct IntNodeComparator {
        // This class defines a comparator providing comparison operations
        // between 'bslstl::TreeNode<int>' objects, and 'int' values.
//
      private:
        // PRIVATE TYPES
        typedef bslstl::TreeNode<int> Node;
            // Alias for a node type containing an 'int' value.
//
      public:
        // CLASS METHODS
        bool operator()(const bslalg::RbTreeNode& lhs, int rhs) const
        {
            return static_cast<const Node&>(lhs).value() < rhs;
        }
//
        bool operator()(int lhs, const bslalg::RbTreeNode& rhs) const
        {
            return lhs < static_cast<const Node&>(rhs).value();
        }
    };
//..
// Then, we define the public interface of 'IntSet'.  Note that it contains a
// 'TreeNodePool' that will be used by 'bslalg::RbTreeUtil' as a 'FACTORY' to
// create and delete nodes.  Also note that a number of simplifications have
// been made for the purpose of illustration.  For example, this implementation
// provides only a minimal set of critical operations, and it does not use the
// empty base-class optimization for the comparator.
//..
    template <class ALLOCATOR = bsl::allocator<int> >
    class IntSet {
//      // This class implements a set of (unique) 'int' values.
//
        // PRIVATE TYPES
        typedef bslstl::TreeNodePool<int, ALLOCATOR> TreeNodePool;
//
        // DATA
        bslalg::RbTreeAnchor d_tree;      // tree of node objects
        TreeNodePool         d_nodePool;  // allocator for node objects
//
        // NOT IMPLEMENTED
        IntSet(const IntSet&);
        IntSet& operator=(const IntSet&);
//
      public:
        // CREATORS
        IntSet(const ALLOCATOR& allocator = ALLOCATOR());
            // Create an empty set.  Optionally specify an 'allocator' used to
            // supply memory.  If 'allocator' is not specified, a default
            // constructed 'ALLOCATOR' object is used.
//
        //! ~IntSet() = 0;
            // Destroy this object.
//
        // MANIPULATORS
        void insert(int value);
            // Insert the specified 'value' into this set.
//
        bool remove(int value);
            // If 'value' is a member of this set, then remove it from the set
            // and return 'true'.  Otherwise, return 'false' with no effect.
//
        // ACCESSORS
        bool isElement(int value) const;
            // Return 'true' if the specified 'value' is a member of this set,
            // and 'false' otherwise.
//
        int numElements() const;
            // Return the number of elements in this set.
    };
//..
// Now, we implement the methods of 'IntSet' using 'RbTreeUtil'.
//..
    // CREATORS
    template <class ALLOCATOR>
    inline
    IntSet<ALLOCATOR>::IntSet(const ALLOCATOR& allocator)
    : d_tree()
    , d_nodePool(allocator)
    {
    }
//
    // MANIPULATORS
    template <class ALLOCATOR>
    void IntSet<ALLOCATOR>::insert(int value)
    {
        int comparisonResult;
        IntNodeComparator comp;

        bslalg::RbTreeNode *parent =
            bslalg::RbTreeUtil::findUniqueInsertLocation(&comparisonResult,
                                                         &d_tree,
                                                         comp,
                                                         value);
//..
// Here we use the 'TreeNodePool' object, 'd_nodePool', to create the node that
// was inserted into the set.
//..
        if (0 != comparisonResult) {
            bslalg::RbTreeNode *node = d_nodePool.createNode(value);
            bslalg::RbTreeUtil::insertAt(&d_tree,
                                         parent,
                                         comparisonResult < 0,
                                         node);
        }
    }
//
    template <class ALLOCATOR>
    bool IntSet<ALLOCATOR>::remove(int value)
    {
        IntNodeComparator comparator;
        bslalg::RbTreeNode *node =
                  bslalg::RbTreeUtil::find(d_tree, comparator, value);
//..
// Here we use the 'TreeNodePool' object, 'd_nodePool', to delete a node that
// was removed from the set.
//..
        if (node) {
            bslalg::RbTreeUtil::remove(&d_tree, node);
            d_nodePool.deleteNode(node);
        }
        return node;
    }
//
    // ACCESSORS
    template <class ALLOCATOR>
    inline
    bool IntSet<ALLOCATOR>::isElement(int value) const
    {
        IntNodeComparator comp;
        return bslalg::RbTreeUtil::find(d_tree, comp, value);
    }
//
    template <class ALLOCATOR>
    inline
    int IntSet<ALLOCATOR>::numElements() const
    {
        return d_tree.numNodes();
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
      case 9: {
        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

// Finally, we create a sample 'IntSet' object and insert 3 values into the
// 'IntSet'.  We verify the attributes of the 'Set' before and after each
// insertion.
//..
    bslma::TestAllocator defaultAllocator("defaultAllocator");
    bslma::DefaultAllocatorGuard defaultGuard(&defaultAllocator);
//
    bslma::TestAllocator objectAllocator("objectAllocator");
//
    IntSet<bsl::allocator<int> > set(&objectAllocator);
    ASSERT(0 == defaultAllocator.numBytesInUse());
    ASSERT(0 == objectAllocator.numBytesInUse());
    ASSERT(0 == set.numElements());
//
    set.insert(1);
    ASSERT(set.isElement(1));
    ASSERT(1 == set.numElements());
//
    set.insert(1);
    ASSERT(set.isElement(1));
    ASSERT(1 == set.numElements());
//
    set.insert(2);
    ASSERT(set.isElement(1));
    ASSERT(set.isElement(2));
    ASSERT(2 == set.numElements());
//
    ASSERT(0 == defaultAllocator.numBytesInUse());
    ASSERT(0 <  objectAllocator.numBytesInUse());
//..
      } break;
      case 8: {
        TestDriver<bsltf::AllocTestType>::testCase8();
      } break;
      case 7: {
        TestDriver<bsltf::AllocTestType>::testCase7();
      } break;
      case 6: {
        TestDriver<bsltf::AllocTestType>::testCase6();
      } break;
      case 5: {
        TestDriver<bsltf::AllocTestType>::testCase5();
      } break;
      case 4: {
        TestDriver<bsltf::AllocTestType>::testCase4();
      } break;
      case 3: {
        TestDriver<bsltf::AllocTestType>::testCase3();
      } break;
      case 2: {
        TestDriver<bsltf::AllocTestType>::testCase2();
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

            typedef TreeNodePool<int, bsl::allocator<int> > Obj;
            typedef TreeNode<int>                           Node;

            bslma::TestAllocator da, ta;
            bslma::DefaultAllocatorGuard daGuard(&da);

            Obj x(&ta);
            Node *value = (Node *)x.createNode();
            ASSERT(0 != value);
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(1 == ta.numBlocksInUse());
            x.deleteNode(value);
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(1 == ta.numBlocksInUse());

            value = (Node *)x.createNode(0xabcd);
            ASSERT(0 != value);
            ASSERT(0xabcd == value->value());
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(1 == ta.numBlocksInUse());
            x.deleteNode(value);
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(1 == ta.numBlocksInUse());
        }
        {
            if (veryVerbose) {
                printf("\tTest allocating-node w/ bslma::Allocator\n");
            }

            typedef AllocatingIntType AllocType;
            typedef TreeNodePool<AllocType, bsl::allocator<AllocType> > Obj;
            typedef TreeNode<AllocatingIntType> Node;

            bslma::TestAllocator da, ta;
            bslma::DefaultAllocatorGuard daGuard(&da);


            Obj x(&ta);

            Node *value = (Node *)x.createNode();
            ASSERT(0 != value);
            ASSERT(0xabcd == value->value().value());
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(2 == ta.numBlocksInUse());
            x.deleteNode(value);
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(1 == ta.numBlocksInUse());

            bslma::TestAllocator ta2;
            AllocType myInt(&ta2);
            myInt.value() = 0xdbca;

            ASSERT(0 == da.numBlocksInUse());
            ASSERT(1 == ta.numBlocksInUse());

            value = (Node *)x.createNode(myInt);
            ASSERT(0 != value);
            ASSERT(0xdbca == value->value().value());
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(2 == ta.numBlocksInUse());
            x.deleteNode(value);
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(1 == ta.numBlocksInUse());
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
