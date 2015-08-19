// bslstl_bidirectionalnodepool.t.cpp                                 -*-C++-*-
#include <bslstl_bidirectionalnodepool.h>

#include <bslstl_allocator.h>

#include <bslalg_bidirectionallink.h>
#include <bslalg_bidirectionallinklistutil.h>
#include <bslalg_bidirectionalnode.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatormonitor.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_bsltestutil.h>

#include <bsltf_stdtestallocator.h>
#include <bsltf_templatetestfacility.h>
#include <bsltf_testvaluesarray.h>

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace BloombergLP;
using namespace bslstl;

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a mechanism that can be used to create and
// destroy node objects using an underlying memory pool.  The state of the
// mechanism is the list of used memory blocks containing in-use nodes, and the
// list of free blocks in the memory pool.  While that state is not exposed
// directly by the mechanism, it can be reasonablely inferred from the number
// of allocations and deallocations.
//
// Since this mechanism does not provide the copy-assignment operator, the
// copy-constructor, and the equality comparison operator, we will follow only
// the first four tests in the standard 10-case approach to testing
// value-semantic types, which includes the breathing test, and the tests for
// primary manipulators and primary accessors.
//
// One area of focus is ensuring that the memory is allocated from the correct
// allocator for varying allocator types: instances of 'bsl::allocator', and
// another standard-conforming allocator type.  On node creation, if the
// allocator type is an instance of 'bsl::allocator', then the contained
// 'bslma::Allocator' object will be used to allocate the 'value' attribute of
// the node; otherwise, the default allocator will be used.
//
// The primary manipulators and basic accessors are decided to be:
//
// Primary Manipulators:
//: o no parameter version of 'createNode'
//
// Basic Accessors:
//: o 'allocator'
//
// After the above are tested, the rest of the test driver will concentrate on
// testing the methods to return a node to the memory pool, 'deleteNode', and
// other overloads for 'createNode'.
//
// Global Concerns:
//: o Pointer/reference parameters are declared 'const'.
//: o No memory is ever allocated from the global allocator.
//: o Precondition violations are detected in appropriate build modes.
//-----------------------------------------------------------------------------
// CREATORS
// [ 2] explicit BidirectionalNodePool(const ALLOCATOR& allocator);
// [ 2] ~BidirectionalNodePool();
//
// MANIPULATORS
// [ 4] AllocatorType& allocator();
// [ 2] bslalg::BidirectionalLink *createNode();
// [ 7] bslalg::BidirectionalLink *createNode(const VALUE& value);
// [ 8] bslalg::BidirectionalLink *createNode(first, second);
// [ 9] bslalg::BidirectionalLink *cloneNode(const BidirectionalLink&);
// [ 5] void deleteNode(bslalg::BidirectionalLink *node);
// [ 6] void reserveNodes(std::size_t numNodes);
// [10] void swapRetainAllocators(other);
// [10] void swapExchangeAllocators(other);
//
// ACCESSORS
// [ 4] const AllocatorType& allocator() const;
//
// FREE FUNCTIONS
// [10] void swap(BidirectionalNodePool& a, b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ *] CONCERN: No memory is ever allocated from the global allocator.
//-----------------------------------------------------------------------------
//=============================================================================

// ============================================================================
//                          ADL SWAP TEST HELPER
// ----------------------------------------------------------------------------

template <class TYPE>
void invokeAdlSwap(TYPE& a, TYPE& b)
    // Exchange the values of the specified 'a' and 'b' objects using the
    // 'swap' method found by ADL (Argument Dependent Lookup).  The behavior
    // is undefined unless 'a' and 'b' were created with the same allocator.
{
    using namespace bsl;
    swap(a, b);
}

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
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

typedef bslalg::BidirectionalLink Link;
typedef bslalg::BidirectionalNode<int> IntNode;

namespace {

bool expectToAllocate(size_t n)
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
//                               TEST FACILITIES
//-----------------------------------------------------------------------------

namespace {

class AllocatingIntType {
    // DATA
    bslma::Allocator *d_allocator_p;
    int              *d_value_p;

  private:
    // NOT IMPLEMENTED
    AllocatingIntType(const AllocatingIntType&);

  public:

    // CREATORS
    explicit AllocatingIntType(bslma::Allocator *allocator)
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

class NonAllocatingTestType {
    // This class implements a non-allocating test type that enables the
    // tracking of the constructor that was called.

    // DATA
    bool   d_singleFlag;  // flag indicating that the one-parameter
                          // constructor has been called.

    bool   d_doubleFlag;  // flag indicating that the two-parameters
                          // constructor has been called.

    double d_arg1;        // value of the first constructor argument

    double d_arg2;        // value of the second constructor argument

  private:
    // NOT IMPLEMENTED
    NonAllocatingTestType(const NonAllocatingTestType&);

  public:
    // CREATORS
    explicit NonAllocatingTestType(const double& arg)
    : d_singleFlag(true)
    , d_doubleFlag(false)
    , d_arg1(arg)
    , d_arg2(0)
    {
    }

    NonAllocatingTestType(const double& arg1, const double& arg2)
    : d_singleFlag(false)
    , d_doubleFlag(true)
    , d_arg1(arg1)
    , d_arg2(arg2)
    {
    }

    // ACCESSORS
    bool oneParamConstructorFlag() const
    {
        return d_singleFlag;
    }

    bool twoParamsConstructorFlag() const
    {
        return d_doubleFlag;
    }

    double arg1() const { return d_arg1; }
    double arg2() const { return d_arg2; }
};

class AllocatingTestType {
    // This class implements allocating test type that enables the tracking of
    // the constructor that was called.

    // DATA
    bool              d_singleFlag;   // flag indicating that the
                                      // one-parameter constructor has been
                                      // called.

    bool              d_doubleFlag;   // flag indicating that the
                                      // two-parameter constructor has been
                                      // called.

    double           *d_arg1_p;       // address of the first constructor
                                      // argument

    double           *d_arg2_p;       // address of the second constructor
                                      // argument

    bslma::Allocator *d_allocator_p;  // address of the allocator used to
                                      // allocate memory (held, not owned)

  private:
    // NOT IMPLEMENTED
    AllocatingTestType(const AllocatingTestType&);

  public:
    // CREATORS
    AllocatingTestType(const double& arg, bslma::Allocator *basicAllocator)
    : d_singleFlag(true)
    , d_doubleFlag(false)
    , d_allocator_p(basicAllocator)
    {

        d_arg1_p  = static_cast<double *>(
                                      d_allocator_p->allocate(sizeof(double)));
        d_arg2_p  = static_cast<double *>(
                                      d_allocator_p->allocate(sizeof(double)));

        *d_arg1_p = arg;
        *d_arg2_p = 0;

    }

    AllocatingTestType(const double&     arg1,
                       const double&     arg2,
                       bslma::Allocator *basicAllocator)
    : d_singleFlag(false)
    , d_doubleFlag(true)
    , d_allocator_p(basicAllocator)
    {
        d_arg1_p  = static_cast<double *>(
                                      d_allocator_p->allocate(sizeof(double)));
        d_arg2_p  = static_cast<double *>(
                                      d_allocator_p->allocate(sizeof(double)));

        *d_arg1_p = arg1;
        *d_arg2_p = arg2;
    }

    ~AllocatingTestType()
    {
        d_allocator_p->deleteObject(d_arg1_p);
        d_allocator_p->deleteObject(d_arg2_p);
    }

    // ACCESSORS
    bool oneParamConstructorFlag() const
    {
        return d_singleFlag;
    }

    bool twoParamsConstructorFlag() const
    {
        return d_doubleFlag;
    }

    double arg1() const { return *d_arg1_p; }
    double arg2() const { return *d_arg2_p; }
};

}  // close unnamed namespace

namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<AllocatingIntType> : bsl::true_type {};

template <>
struct UsesBslmaAllocator<AllocatingTestType> : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

namespace {

class Stack
{
    enum { CAPACITY = 128 };
    Link   *d_data[CAPACITY];
    size_t  d_size;

  public:
    // CREATORS
    Stack() : d_size(0) {}

    Stack(const Stack& original)
    {
        d_size = original.d_size;
        memcpy(d_data, original.d_data, d_size * sizeof(*d_data));
    }

    // MANIPULATORS
    void push(Link *value)
    {
        BSLS_ASSERT(CAPACITY != d_size);

        d_data[d_size] = value;
        ++d_size;
    }

    void pop()
    {
        BSLS_ASSERT_SAFE(0 != d_size);

        --d_size;
    }

    // ACCESSORS
    bool empty() const { return 0 == d_size; }

    size_t size() const { return d_size; }

    Link *back() const
    {
        BSLS_ASSERT_SAFE(0 != d_size);

        return d_data[d_size - 1];
    }

    Link *operator[] (size_t index) const
    {
        BSLS_ASSERT_SAFE(index <  d_size);

        return d_data[index];
    }
};


template <class VALUE>
class TestDriver {
    // This templatized struct provide a namespace for testing.  The
    // parameterized 'VALUE' specifies the value type for this object.  Each
    // "testCase*" method test a specific aspect of 'SimplePool<VALUE>'.  Every
    // test cases should be invoked with various parameterized type to fully
    // test the container.

  private:
    // TYPES
    typedef bslstl::BidirectionalNodePool<VALUE, bsl::allocator<VALUE> > Obj;
        // Type under testing.

    typedef bsltf::StdTestAllocator<VALUE> StlAlloc;

    typedef bslalg::BidirectionalNode<VALUE> ValueNode;

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
    static void testCase11();
        // Test type traits.

    static void testCase10();
        // Test 'swap'.

    static void testCase9();
        // Test 'cloneNode'.

    static void testCase8();
        // Test 'createNode(first, second)'.

    static void testCase7();
        // Test 'createNode(value)'.

    static void testCase6();
        // Test 'reserveNode'.

    static void testCase5();
        // Test 'deleteNode'.

    static void testCase4();
        // Test basic accessors ('allocator').

    static void testCase2();
        // Test primary manipulators.
};

template <class VALUE>
const bslstl::BidirectionalNodePool<VALUE, bsl::allocator<VALUE> >&
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
        Link *ptr = result->createNode();
        usedBlocks->push(ptr);
    }

    for (int i = 0; i < numDealloc; ++i) {
        Link *ptr = usedBlocks->back();
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
        Link *ptr = result->createNode();
        usedBlocks->push(ptr);
    }

    // Use up all the free blocks.

    while (!expectToAllocate(usedBlocks->size() + 1)) {
        Link *ptr = result->createNode();
        usedBlocks->push(ptr);
    }

    // Free up the necessary number of blocks.

    for (int i = 0; i < numBlocks; ++i) {
        result->deleteNode(usedBlocks->back());
        usedBlocks->pop();
    }
}

template<class VALUE>
void TestDriver<VALUE>::testCase11()
{
    // ------------------------------------------------------------------------
    // TYPE TRAITS
    //
    // Concern:
    //: 1 The object has the necessary type traits.
    //
    // Plan:
    //: 1 Use 'BSLMF_ASSERT' to verify all the type traits exists.  (C-1)
    //
    // Testing:
    //   CONCERN: The object has the necessary type traits.
    // ------------------------------------------------------------------------

    // Verify set defines the expected traits.
    BSLMF_ASSERT((1 == bslmf::IsBitwiseMoveable<Obj>::value));
}

template<class VALUE>
void TestDriver<VALUE>::testCase10()
{
    // --------------------------------------------------------------------
    // MANIPULATOR 'swap'
    //
    // Concerns:
    //: 1 Invoking either of the swap methods or the free function exchange the
    //:  free list and chunk list of the objects.
    //:
    //: 2 The common object allocator address held by both objects is unchanged
    //:   after 'swapRetainAllocators' or the free 'swap' function is invoked.
    //:
    //: 3 The object allocator addresses of the two objects are exchanged after
    //:   'swapExchangeAllocators' is invoked.
    //:
    //: 4 No memory is allocated from any allocator.
    //:
    //: 5 Swapping an object with itself does not affect the value of the
    //:   object (alias-safety).
    //:
    //: 6 Memory is deallocated on the destruction of the object.
    //:
    //: 7 QoI: Asserted precondition violations are detected when enabled.
    //:
    //: 8 The free 'swap' function is discoverable through ADL (Argument
    //:   Dependent Lookup).
    //
    // Plan:
    //: 1 Using a table-based approach:
    //:
    //:   1 Create two objects of which memory has been allocated and
    //:     deallocated various number of times using the same allocator.
    //:
    //:   2 Using the 'swapRetainAllocators' method to swap the two objects,
    //:     verify allocator is not changed.  (C-2)
    //:
    //:   3 Verify no memory is allocated (C-4)
    //:
    //:   4 Verify the free list of the objects have been swapped by calling
    //:     'allocate' and checking the address of the allocated memory blocks.
    //:
    //:   5 Delete one of the objects and verify the memory of the other have
    //:     not been deallocated.  (C-1, 6)
    //:
    //:   6 Swap an object with itself and verify the object is unchanged.
    //:     (C-5)
    //:
    //:   7 Repeat P-1.1..P-1.6 using two object created using unequal
    //:     allocators, except this time use the 'swapExchangeAllocators'
    //:     method and verify that the allocators are exchanged.  (C-1, 3..6)
    //:
    //: 2 Verify that, in appropriate build modes, defensive checks are
    //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-7)
    //:
    //: 3 Repeat P-1..2, except this time use 'invokeAdlSwap' helper function
    //:   template instead of the 'swapRetainAllocators' method.  (C-1..2,
    //:   4..8)
    //
    // Testing:
    //   void swapRetainAllocators(other);
    //   void swapExchangeAllocators(other);
    //   void swap(BidirectionalNodePool& a, b);
    // --------------------------------------------------------------------

    if (verbose) printf("\nMANIPULATOR 'swap'"
                        "\n==================\n");

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

    // 'swap' method
    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE1     = DATA[ti].d_line;
            const int ALLOCS1   = DATA[ti].d_numAlloc;
            const int DEALLOCS1 = DATA[ti].d_numDealloc;

            for (int tj = 0; tj < NUM_DATA; ++tj) {
                const int LINE2     = DATA[tj].d_line;
                const int ALLOCS2   = DATA[tj].d_numAlloc;
                const int DEALLOCS2 = DATA[tj].d_numDealloc;

                {
                    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                    Stack usedX;
                    Stack freeX;
                    Obj mX(&oa);
                    const Obj& X = init(&mX,
                                        &usedX,
                                        &freeX,
                                        ALLOCS1,
                                        DEALLOCS1);

                    Stack usedY;
                    Stack freeY;
                    {
                        Obj mY(&oa);
                        const Obj& Y = init(&mY, &usedY, &freeY, ALLOCS2,
                                            DEALLOCS2);

                        if (veryVerbose) { T_ P_(LINE1) P(LINE2) }

                        bslma::TestAllocatorMonitor oam(&oa);

                        mX.swapRetainAllocators(mY);

                        ASSERTV(LINE1, LINE2, &oa == X.allocator());
                        ASSERTV(LINE1, LINE2, &oa == Y.allocator());
                        ASSERTV(LINE1, LINE2, oam.isTotalSame());
                        ASSERTV(LINE1, LINE2, oam.isInUseSame());

                        // Verify the free lists are swapped

                        while(!freeX.empty()) {
                            Link *ptr = mY.createNode();
                            ASSERTV(LINE1, LINE2, freeX.back() == ptr);
                            freeX.pop();
                            usedX.push(ptr);
                        }

                        while(!freeY.empty()) {
                            Link *ptr = mX.createNode();
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

                    // 'Y' is now destroyed, its blocks should be deallocated.
                    // Verify Blocks in 'X' (which used to be in 'Y' before the
                    // swap) is not deallocated.

                    char SCRIBBLED_MEMORY[sizeof(VALUE)];
                    memset(SCRIBBLED_MEMORY, 0xA5, sizeof(VALUE));
                    while (!usedY.empty()) {
                        Link *ptr = usedY.back();
                        ASSERTV(0 != strncmp((char *)ptr,
                                             SCRIBBLED_MEMORY,
                                             sizeof(VALUE)));

                        mX.deleteNode(ptr);
                        usedY.pop();
                    }
                }
                {
                    bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                    bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                    Stack usedX;
                    Stack freeX;
                    Obj mX(&oa1);
                    const Obj& X = init(&mX,
                                        &usedX,
                                        &freeX,
                                        ALLOCS1,
                                        DEALLOCS1);

                    Stack usedY;
                    Stack freeY;
                    {
                        Obj mY(&oa2);
                        const Obj& Y = init(&mY, &usedY, &freeY, ALLOCS2,
                                            DEALLOCS2);

                        if (veryVerbose) { T_ P_(LINE1) P(LINE2) }

                        bslma::TestAllocatorMonitor oam1(&oa1);
                        bslma::TestAllocatorMonitor oam2(&oa2);

                        mX.swapExchangeAllocators(mY);

                        ASSERTV(LINE1, LINE2, &oa2 == X.allocator());
                        ASSERTV(LINE1, LINE2, &oa1 == Y.allocator());
                        ASSERTV(LINE1, LINE2, oam1.isTotalSame());
                        ASSERTV(LINE1, LINE2, oam2.isInUseSame());

                        // Verify the free lists are swapped

                        while(!freeX.empty()) {
                            Link *ptr = mY.createNode();
                            ASSERTV(LINE1, LINE2, freeX.back() == ptr);
                            freeX.pop();
                            usedX.push(ptr);
                        }

                        while(!freeY.empty()) {
                            Link *ptr = mX.createNode();
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

                    // 'Y' is now destroyed, its blocks should be deallocated.
                    // Verify Blocks in 'X' (which used to be in 'Y' before the
                    // swap) is not deallocated.

                    char SCRIBBLED_MEMORY[sizeof(VALUE)];
                    memset(SCRIBBLED_MEMORY, 0xA5, sizeof(VALUE));
                    while (!usedY.empty()) {
                        Link *ptr = usedY.back();
                        ASSERTV(0 != strncmp((char *)ptr,
                                             SCRIBBLED_MEMORY,
                                             sizeof(VALUE)));

                        mX.deleteNode(ptr);
                        usedY.pop();
                    }
                }
            }
        }

        // Verify no memory is allocated from the default allocator.

        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        if (verbose) printf("\nNegative Testing.\n");
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) printf("\t'swap' member function\n");
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);  Obj mB(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(mA.swapRetainAllocators(mB));
                ASSERT_SAFE_FAIL(mA.swapRetainAllocators(mZ));
            }
        }
    }

    // free 'swap' function
    {
        bslma::TestAllocator         da("default", veryVeryVeryVerbose);
        bslma::DefaultAllocatorGuard dag(&da);

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
                    const Obj& Y = init(&mY, &usedY, &freeY, ALLOCS2,
                                        DEALLOCS2);

                    if (veryVerbose) { T_ P_(LINE1) P(LINE2) }

                    bslma::TestAllocatorMonitor oam(&oa);

                    invokeAdlSwap(mX, mY);

                    ASSERTV(LINE1, LINE2, &oa == X.allocator());
                    ASSERTV(LINE1, LINE2, &oa == Y.allocator());
                    ASSERTV(LINE1, LINE2, oam.isTotalSame());
                    ASSERTV(LINE1, LINE2, oam.isInUseSame());

                    // Verify the free lists are swapped

                    while(!freeX.empty()) {
                        Link *ptr = mY.createNode();
                        ASSERTV(LINE1, LINE2, freeX.back() == ptr);
                        freeX.pop();
                        usedX.push(ptr);
                    }

                    while(!freeY.empty()) {
                        Link *ptr = mX.createNode();
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

                // 'Y' is now destroyed, its blocks should be deallocated.
                // Verify Blocks in 'X' (which used to be in 'Y' before the
                // swap) is not deallocated.

                char SCRIBBLED_MEMORY[sizeof(VALUE)];
                memset(SCRIBBLED_MEMORY, 0xA5, sizeof(VALUE));
                while (!usedY.empty()) {
                    Link *ptr = usedY.back();
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

                ASSERT_SAFE_PASS(invokeAdlSwap(mA, mB));
                ASSERT_SAFE_FAIL(invokeAdlSwap(mA, mZ));
            }
        }
    }

}

template<class VALUE>
void TestDriver<VALUE>::testCase9()
{
    // -----------------------------------------------------------------------
    // MANIPULATOR 'cloneNode'
    //
    // Concerns:
    //: 1 'cloneNode' invokes the copy constructor of the (template parameter)
    //:   'VALUE' type.
    //:
    //: 2 Any memory allocation is from the object allocator.
    //:
    //: 3 There is no temporary allocation from any allocator.
    //:
    //: 4 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Create an array of distinct nodes of type 'BidirectionalLink'.  For
    //:   each node:
    //:
    //:   1 Invoke 'cloneNode' on the node.
    //:
    //:   2 Verify memory is allocated only when expected.  (C-2..3)
    //:
    //:   3 Verify the 'value' attribute of the new node compare equals to the
    //:     'value' attribute of the original node.  (C-1)
    //:
    //: 2 Verify all memory is released on destruction.  (C-4)
    //
    // Testing:
    //   bslalg::BidirectionalLink *cloneNode(
    //                              const bslalg::BidirectionalLink& original);
    // -----------------------------------------------------------------------

    if (verbose) printf("\nMANIPULATOR 'cloneNode(value)'"
                        "\n==============================\n");

    const bool TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

    bsltf::TestValuesArray<VALUE> VALUES;

    Obj mX(&scratch);
    Stack usedX;

    for (int i = 0; i < 16; ++i) {
        Link *ptr = mX.createNode(VALUES[i]);
        usedX.push(ptr);
    }

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);
    {
        Obj mY(&oa);
        Stack usedY;

        for (size_t i = 0; i < 16; ++i) {
            bslma::TestAllocatorMonitor oam(&oa);

            Link *ptr = mY.cloneNode(*usedX[i]);

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

            ValueNode *nodeX = static_cast<ValueNode *>(usedX[i]);
            ASSERTV(i, nodeX->value() == nodeY->value());
        }

        while(!usedY.empty()) {
            mY.deleteNode(usedY.back());
            usedY.pop();
        }
    }

    while(!usedX.empty()) {
        mX.deleteNode(usedX.back());
        usedX.pop();
    }

    // Verify all memory is released on object destruction.
    ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
}

template<class VALUE>
void TestDriver<VALUE>::testCase8()
{
    // -----------------------------------------------------------------------
    // MANIPULATOR 'createNode(first, second)'
    //
    // Concerns:
    //: 1 'createNode' forwards its arguments to the two-argument constructor
    //:   of the (template parameter) 'VALUE' type.
    //:
    //: 2 The argument passed to 'createNode' can be implicitly convertible to
    //:   to the parameter type of the constructor of the (template parameter)
    //:   'VALUE' type.
    //:
    //: 3 Any memory allocation is from the object allocator.
    //:
    //: 4 There is no temporary allocation from any allocator.
    //:
    //: 5 Every object releases any allocated memory at destruction.
    //
    // Plan:
    //: 1 Create an array of distinct object.  For each object in the array:
    //:
    //:   1 Invoke 'createNode' on the object.
    //:
    //:   2 Verify memory is allocated only when expected.  (C-3..4)
    //:
    //:   3 Verify the value of the new node was constructed properly.  (C-1)
    //:
    //: 2 Verify all memory is released on destruction.  (C-5)
    //:
    //: 3 Call the 'createNode' method passing in two arguments having types
    //:   that is implicitly convertible to the parameter type of the
    //:   constructor.  Verify that the constructor has been called.  (C-2)
    //
    // Testing:
    //   bslalg::BidirectionalLink *createNode(first, second);
    // -----------------------------------------------------------------------

    if (verbose) printf("\nMANIPULATOR 'createNode(first, second)'"
                        "\n=======================================\n");

    const bool TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    {
        Obj mX(&oa);

        Stack usedX;

        for (short unsigned i = 0; i < 16; ++i) {
            bslma::TestAllocatorMonitor oam(&oa);

            double arg1  = i;
            double arg2  = i * 2;

            Link *ptr = mX.createNode(arg1, arg2);

            if (expectToAllocate(i + 1)) {
                ASSERTV(1 + 2 * TYPE_ALLOC == oam.numBlocksTotalChange());
                ASSERTV(1 + 2 * TYPE_ALLOC == oam.numBlocksInUseChange());
            }
            else {
                ASSERTV(2 * TYPE_ALLOC == oam.numBlocksTotalChange());
                ASSERTV(2 * TYPE_ALLOC == oam.numBlocksInUseChange());
            }

            usedX.push(ptr);

            ValueNode *node = static_cast<ValueNode *>(ptr);
            ASSERTV(i, arg1 == node->value().arg1());
            ASSERTV(i, arg2 == node->value().arg2());
            ASSERTV(i, node->value().twoParamsConstructorFlag());
        }

        while(!usedX.empty()) {
            mX.deleteNode(usedX.back());
            usedX.pop();
        }
    }
    // Verify all memory is released on object destruction.
    ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());

    {
        Obj mX(&oa);
        float arg1f = 1;
        int   arg2i = 2;
        Link *ptr = mX.createNode(arg1f, arg2i);

        ValueNode *node = static_cast<ValueNode *>(ptr);
        ASSERTV(1 == node->value().arg1());
        ASSERTV(2 == node->value().arg2());
        ASSERTV(node->value().twoParamsConstructorFlag());

        mX.deleteNode(ptr);
    }
    ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
}

template<class VALUE>
void TestDriver<VALUE>::testCase7()
{
    // -----------------------------------------------------------------------
    // MANIPULATOR 'createNode(value)'
    //
    // Concerns:
    //: 1 'createNode' forwards its argument to a single argument constructor
    //:   of the (template parameter) 'VALUE' type.
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
    //:   3 Verify the new node contains a copy of the object.  (C-1)
    //:
    //: 2 Verify all memory is released on destruction.  (C-4)
    //
    // Testing:
    //   bslalg::BidirectionalLink *createNode(const VALUE& value);
    // -----------------------------------------------------------------------

    if (verbose) printf("\nMANIPULATOR 'createNode(value)'"
                        "\n===============================\n");

    const bool TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

    bslma::TestAllocator oa("object", veryVeryVeryVerbose);

    {
        bsltf::TestValuesArray<VALUE> VALUES;

        Obj mX(&oa);

        Stack usedX;

        for (size_t i = 0; i < 16; ++i) {
            bslma::TestAllocatorMonitor oam(&oa);

            Link *ptr = mX.createNode(VALUES[i]);

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

        while(!usedX.empty()) {
            mX.deleteNode(usedX.back());
            usedX.pop();
        }
    }

    // Verify all memory is released on object destruction.
    ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
}

template <>
void TestDriver<NonAllocatingTestType>::testCase7()
{
    // -----------------------------------------------------------------------
    // MANIPULATOR 'createNode(value)'
    //
    // Concerns:
    //: 1 The single-argument overload of the 'createNode' method forwards its
    //:   argument to a single-argument constructor of the (template parameter)
    //:   'VALUE' type.
    //:
    //: 2 The argument passed to 'createNode' can be implicitly convertible to
    //:   to the parameter type of the constructor of the (template parameter)
    //:   'VALUE' type.
    //
    // Plan:
    //: 1 Call the 'createNode' method and verify that the single-argument
    //:   constructor of a custom test type has been called called.  (C-1)
    //:
    //: 2 Call the 'createNode' method passing in an argument having a type
    //:   that is implicitly convertible to the parameter type of the
    //:   constructor.  Verify that the constructor has been called.  (C-2)
    //
    // Testing:
    //   bslalg::BidirectionalLink *createNode(const VALUE& value);
    // -----------------------------------------------------------------------

    if (verbose) printf("\nMANIPULATOR 'createNode(value)'"
                        "\n===============================\n");

    bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);
    Obj mX(&scratch);

    double arg1 = 1.0;
    Link *ptr = mX.createNode(arg1);

    ASSERT(static_cast<ValueNode*>(ptr)->value().oneParamConstructorFlag());
    mX.deleteNode(ptr);

    float arg1f = 1.0;
    ptr = mX.createNode(arg1f);
    ASSERT(static_cast<ValueNode*>(ptr)->value().oneParamConstructorFlag());
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
    //: 2 Free blocks that was allocated before 'reserve' are not destroyed.
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

    const bool TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

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
    //: 3 'createNode' retrieve the last node that was deleted.
    //:
    //: 4 'deleteNode' does not allocate or release any memory other than those
    //:   caused by the destructor of the value.
    //:
    //: 5 QoI: Asserted precondition violations are detected when enabled.
    //
    // Plan:
    //: 1 Create a list of sequences to allocate and deallocate memory.  For
    //:   each sequence:
    //:
    //:   1 Invoke 'createNode' and 'deleteNode' according to the sequence.
    //:
    //:   2 Verify that each 'createNode' returns the last block that was
    //:     deallocated if 'deleteNode' was called.  (C-1..3)
    //:
    //:   3 Verify no memory was allocated from the heap on 'deleteNode'.
    //:     (C-4)
    //:
    //:   4 Verify 'createNode' will get memory from the heap only when
    //:     expected.
    //:
    //: 2 Verify that, in appropriate build modes, defensive checks are
    //:   triggered (using the 'BSLS_ASSERTTEST_*' macros).  (C-5)
    //
    // Testing:
    //   void deleteNode(bslalg::BidirectionalLink *node);
    // --------------------------------------------------------------------

    if (verbose) printf("\nMANIPULATOR 'deleteNode'"
                        "\n========================");

    const int TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;

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
                Link *ptr = mX.createNode();

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
                Link *ptr = usedBlocks.back();

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

            Link *ptr = mX.createNode();

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
    //:   1 Create a 'bslstl::BidirectionalNodePool' with an allocator.
    //:
    //:   2 Use the basic accessor to verify the allocator is installed
    //:     properly.  (C-1..2)
    //:
    //:   3 Verify no memory is allocated from any allocator.  (C-3)
    //
    // Testing:
    //   AllocatorType& allocator();
    //   const AllocatorType& allocator() const;
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
        ASSERTV(CONFIG, &oa == mX.allocator());

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
    //: 2 if the (template parameter) 'ALLOCATOR' is an instance of the
    //:   'bsl::allocator' template, then any memory allocation is from the
    //:   supplied allocator .
    //:
    //: 3 If the default allocator if the (template parameter) 'ALLOCATOR' is
    //:   another standard-compliant allocator, then memory allocated for the
    //:   nodes is from the supplied allocator, while memory allocated for the
    //:   constructor of the node's 'value' attribute is from the default
    //:   allocator.
    //:
    //: 4 There is no temporary allocation from any allocator.
    //:
    //: 5 Every object releases any allocated memory at destruction.
    //:
    //: 6 Allocation starts at one block, up to a maximum of 32 blocks.
    //:
    //: 7 Constructor allocates no memory.
    //:
    //: 8 Any memory allocation is exception neutral.
    //:
    // Plan:
    //: 1 For each 'bsl::allocator' configuration:
    //:
    //:   1 Create a pool object and verify no memory is allocated.  (C-1, 7)
    //:
    //:   2 Call 'allocate' 96 times in the presence of exception, for each
    //:     time:
    //:
    //:     1 Verify memory is only allocated from object allocator and only
    //:       when expected.  (C-2, 4, 6..7)
    //:
    //:     2 If memory is not allocated, the address is the max of
    //:       'sizeof(VALUE)' and 'sizeof(void *) larger than the previous
    //:       address.  (C-8)
    //:
    //:   3 Delete the object and verify all memory is deallocated.  (C-5)
    //:
    //: 2 Repeat P1, except using another standard-compliant allocator.
    //    (C-1..8)
    //
    // Testing:
    //   explicit BidirectionalNodePool(const ALLOCATOR& allocator);
    //   ~BidirectionalNodePool();
    //   VALUE *createNode();
    // --------------------------------------------------------------------

    if (verbose) printf(
                 "\nDEFAULT CTOR, PRIMARY MANIPULATORS, & DTOR"
                 "\n==========================================\n");

    const bool TYPE_ALLOC = bslma::UsesBslmaAllocator<VALUE>::value;


    // Testing various 'bsl::allocator' configuration.

    for (char cfg = 'a'; cfg <= 'b'; ++cfg) {

        const char CONFIG = cfg;  // how we specify the allocator

        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);

        Obj                  *objPtr;
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

        for (size_t i = 0; i < 96; ++i) {
            bslma::TestAllocatorMonitor oam(&oa);

            Link *ptr = mX.createNode();

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

            if (TYPE_ALLOC) {
                ASSERTV(oam.numBlocksInUseChange(),
                        -1 == oam.numBlocksInUseChange());
            }
            else {
                ASSERTV(oam.numBlocksInUseChange(),
                         0 == oam.numBlocksInUseChange());
            }

            usedBlocks.pop();
        }

        // Reclaim dynamically allocated object under test.

        fa.deleteObject(objPtr);

        // Verify all memory is released on object destruction.

        ASSERTV(fa.numBlocksInUse(),  0 ==  fa.numBlocksInUse());
        ASSERTV(oa.numBlocksInUse(),  0 ==  oa.numBlocksInUse());
        ASSERTV(noa.numBlocksTotal(), 0 == noa.numBlocksTotal());
    }

    // Testing standard-compliant allocator.
    {
        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied",   veryVeryVeryVerbose);

        bslma::DefaultAllocatorGuard dag(&da);
        bsltf::StdTestAllocatorConfigurationGuard stag(&sa);

        typedef bslstl::BidirectionalNodePool<VALUE, StlAlloc> ObjStlAlloc;

        StlAlloc     stlOA;
        ObjStlAlloc *objPtr = new (fa) ObjStlAlloc(stlOA);

        ObjStlAlloc& mX = *objPtr;

        // ---------------------------------------
        // Verify allocator is installed properly.
        // ---------------------------------------

        ASSERTV(stlOA == mX.allocator());

        // Verify no allocation from the object/non-object allocators.

        ASSERTV(sa.numBlocksTotal(), 0 == sa.numBlocksTotal());
        ASSERTV(da.numBlocksTotal(), 0 == da.numBlocksTotal());

        Stack usedBlocks;

        for (size_t i = 0; i < 96; ++i) {
            bslma::TestAllocatorMonitor dam(&da);
            bslma::TestAllocatorMonitor sam(&sa);

            Link *ptr = mX.createNode();

            if (expectToAllocate(i + 1)) {
                ASSERTV(1 == sam.numBlocksTotalChange());
                ASSERTV(1 == sam.numBlocksInUseChange());
                ASSERTV(TYPE_ALLOC == dam.numBlocksTotalChange());
                ASSERTV(TYPE_ALLOC == dam.numBlocksInUseChange());
            }
            else {
                ASSERTV(TYPE_ALLOC == dam.numBlocksTotalChange());
                ASSERTV(TYPE_ALLOC == dam.numBlocksInUseChange());
            }

            usedBlocks.push(ptr);
        }

        // Verify no temporary memory is allocated from the object
        // allocator.
        ASSERTV(da.numBlocksTotal(), da.numBlocksInUse(),
                sa.numBlocksTotal() == sa.numBlocksInUse());

        // Free up used blocks.
        for (int i = 0; i < 96; ++i) {
            bslma::TestAllocatorMonitor dam(&da);

            mX.deleteNode(usedBlocks.back());

            ASSERTV(-TYPE_ALLOC == dam.numBlocksInUseChange());

            usedBlocks.pop();
        }

        // Reclaim dynamically allocated object under test.

        fa.deleteObject(objPtr);

        // Verify all memory is released on object destruction.

        ASSERTV(fa.numBlocksInUse(), 0 == fa.numBlocksInUse());
        ASSERTV(da.numBlocksInUse(), 0 == da.numBlocksInUse());
        ASSERTV(sa.numBlocksInUse(), 0 == sa.numBlocksInUse());
    }
}

}  // close unnamed namespace

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

namespace {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Linked List Container
///- - - - - - - - - - - - - - - - - - - - - -
// Suppose that we want to define a bidirectional linked list that can hold
// elements of a template parameter type.  'bslstl::BidirectionalNodePool' can
// be used to create and destroy nodes that make up a linked list.
//
// First, we create an elided definition of the class template 'MyList':
//..
// #include <bslalg_bidirectionallinklistutil.h>

template <class VALUE, class ALLOCATOR>
class MyList {
    // This class template implements a bidirectional linked list of element of
    // the (template parameter) type 'VALUE'.  The memory used will be
    // allocated from an allocator of the (template parameter) type 'ALLOCATOR'
    // specified at construction.

  public:
    // TYPES
    typedef bslalg::BidirectionalNode<VALUE> Node;
        // This 'typedef' is an alias to the type of the linked list node.

  private:
    // TYPES
    typedef bslstl::BidirectionalNodePool<VALUE, ALLOCATOR> Pool;
        // This 'typedef' is an alias to the type of the memory pool.

    typedef bslalg::BidirectionalLinkListUtil               Util;
        // This 'typedef' is an alias to the utility 'struct' providing
        // functions for constructing and manipulating linked lists.

    typedef bslalg::BidirectionalLink                       Link;
        // This 'typedef' is an alis to the type of the linked list link.

    // DATA
    Node *d_head_p;  // pointer to the head of the linked list
    Node *d_tail_p;  // pointer to the tail of the linked list
    Pool  d_pool;    // memory pool used to allocate memory


  public:
    // CREATORS
    explicit MyList(const ALLOCATOR& allocator = ALLOCATOR());
        // Create an empty linked list that allocate memory using the specified
        // 'allocator'.

    ~MyList();
        // Destroy this linked list by calling destructor for each element and
        // deallocate all allocated storage.

    // MANIPULATORS
    void pushFront(const VALUE& value);
        // Insert the specified 'value' at the front of this linked list.

    void pushBack(const VALUE& value);
        // Insert the specified 'value' at the end of this linked list.

    // ACCESSORS
    const Node *head() const
        // Return the node at the beginning of this linked list.
    {
        return d_head_p;
    }

    const Node *tail() const
        // Return the node at the end of this linked list.
    {
        return d_tail_p;
    }
};
//..
// Now, we define the methods of 'MyMatrix':
//..
// CREATORS
template <class VALUE, class ALLOCATOR>
MyList<VALUE, ALLOCATOR>::MyList(const ALLOCATOR& allocator)
: d_head_p(0)
, d_tail_p(0)
, d_pool(allocator)
{
}

template <class VALUE, class ALLOCATOR>
MyList<VALUE, ALLOCATOR>::~MyList()
{
    Link *link = d_head_p;
    while (link) {
        Link *next = link->nextLink();
//..
// Here, we call the memory pool's 'deleteNode' method to destroy the 'value'
// attribute of the node and return its memory footprint back to the pool:
//..
        d_pool.deleteNode(static_cast<Node*>(link));
        link = next;
    }
}

// MANIPULATORS
template <class VALUE, class ALLOCATOR>
void
MyList<VALUE, ALLOCATOR>::pushFront(const VALUE& value)
{
//..
// Here, we call the memory pool's 'createNode' method to allocate a node and
// copy-construct the specified 'value' at the 'value' attribute of the node:
//..
    Node *node = static_cast<Node *>(d_pool.createNode(value));
//..
// Note that the memory pool will allocate the footprint of the node using the
// allocator specified at construction.  If the (template parameter) type
// 'ALLOCATOR' is an instance of 'bsl::allocator' and the (template parameter)
// type 'VALUE' has the 'bslma::UsesBslmaAllocator' trait, then the allocator
// specified at construction will also be supplied to the copy-constructor of
// 'VALUE'.
//..
    if (!d_head_p) {
        d_tail_p = node;
        node->setNextLink(0);
        node->setPreviousLink(0);
    }
    else {
        Util::insertLinkBeforeTarget(node, d_head_p);
    }
    d_head_p = node;
}

template <class VALUE, class ALLOCATOR>
void
MyList<VALUE, ALLOCATOR>::pushBack(const VALUE& value)
{
//..
// Here, just like how we implemented the 'pushFront' method, we call the
// pool's 'createNode' method to allocate a node and copy-construct the
// specified 'value' at the 'value' attribute of the node:
//..
    Node *node = static_cast<Node *>(d_pool.createNode(value));
    if (!d_head_p) {
        d_head_p = node;
        node->setNextLink(0);
        node->setPreviousLink(0);
    }
    else {
        Util::insertLinkAfterTarget(node, d_tail_p);
    }
    d_tail_p = node;
}
//..

}  // close unnamed namespace

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

    // CONCERN: No memory is ever allocated from the global allocator.
    bslma::TestAllocator ga("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&ga);
    bslma::TestAllocatorMonitor gam(&ga);

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

        if (verbose) printf("\nUsage Example"
                            "\n=============\n");


        // Do some ad-hoc breathing test for 'MyVector' type in the usage
        // example.

        bslma::TestAllocator oa("oa", veryVeryVeryVerbose);
        MyList<int, bsl::allocator<int> > list(&oa);

        int DATA[] =  { 3, 2, 1, 10, 5 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            list.pushBack(DATA[i]);
        }
        ASSERT(oa.numBlocksInUse() > 0);

        typedef bslalg::BidirectionalNode<int> Node;
        typedef bslalg::BidirectionalLink      Link;

        const Link *link = list.head();
        int ti = 0;
        while (link)
        {
            const Node * node = static_cast<const Node *>(link);
            ASSERT(node->value() == DATA[ti++]);
            link = link->nextLink();
        }
        ASSERT(NUM_DATA == ti);

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TYPE TRAITS
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase11,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // MANIPULATOR 'swap'
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase10,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // MANIPULATOR 'cloneNode'
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase9,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // MANIPULATOR 'createNode(first, second)'
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase8,
                      NonAllocatingTestType, AllocatingTestType);
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // MANIPULATOR 'createNode'
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase7,
                      NonAllocatingTestType,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // MANIPULATOR 'reserveNodes'
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase6,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // MANIPULATOR 'deleteNode'
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase5,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // BASIC ACCESSORS
        // --------------------------------------------------------------------
        RUN_EACH_TYPE(TestDriver,
                      testCase4,
                      BSLTF_TEMPLATETESTFACILITY_TEST_TYPES_REGULAR);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TEST APPARATUS
        //
        // Concerns:
        //: 1 Objects of 'NonAllocatingTestType' and 'AllocatingTestType'
        //:   correctly keep track of whether they are created with the
        //:   one-parameter constructor or two-parameter constructor.
        //:
        //: 2 The arguments used to construct objects of
        //:   'NonAllocatingTestType' and 'AllocatingTestType' can be accessed
        //:   from the 'arg1' and 'arg2' methods.
        //:
        //: 3 'NonAllocatingTestType' does not allocate any memory.
        //:
        //: 4 'AllocatingTestType' correctly allocate memory on construction
        //:   and releases memory on destruction.
        //:
        //: 5 'AllocatingTestType' has the 'bslma::UsesBslmaAllocator' type
        //:   trait.
        //
        // Plan:
        //: 1 Construct two 'NonAllocatingTestType' objects using the
        //:   one-parameter constructor and two-parameter constructor
        //:   respectively:
        //:
        //:   1 Verify that no memory has been allocated.  (C-3)
        //:
        //:   2 Verify that calling 'oneParamConstructorFlag' and
        //:     'twoParamsConstructorFlag' correctly identifies which
        //:     constructor has been called.  (C-1)
        //:
        //:   3 Verify that 'arg1' and 'arg2' methods return the arguments used
        //:     when calling the constructors.  (C-2)
        //:
        //: 2 Verify that 'AllocatingTestType' has the
        //:   'bslma::UsesBslmaAllocator' trait.  (C-5)
        //:
        //: 3 Construct two 'AllocatingTestType' objects using the
        //:   one-parameter constructor and two-parameter constructor
        //:   respectively:
        //:
        //:   1 Verify that memory has been allocated by only the object
        //:     allocator specified at construction.  (C-4)
        //:
        //:   2 Verify that calling 'oneParamConstructorFlag' and
        //:     'twoParamsConstructorFlag' correctly identifies which
        //:     constructor has been called.  (C-1)
        //:
        //:   3 Verify that 'arg1' and 'arg2' methods return the arguments used
        //:     when calling the constructors.  (C-2)
        //:
        //:   4 Verify after the objects are destroyed that memory used will
        //:     have been released.  (C-3)
        // --------------------------------------------------------------------

        const double ARG1 = 10;
        const double ARG2 = 10;
        {
            NonAllocatingTestType X(ARG1);
            NonAllocatingTestType Y(ARG1, ARG2);

            ASSERT( X.oneParamConstructorFlag());
            ASSERT(!X.twoParamsConstructorFlag());

            ASSERT(!Y.oneParamConstructorFlag());
            ASSERT( Y.twoParamsConstructorFlag());

            ASSERT(ARG1 == X.arg1());
            ASSERT(ARG1 == Y.arg1());
            ASSERT(ARG2 == Y.arg2());
        }

        ASSERT(bslma::UsesBslmaAllocator<AllocatingTestType>::value);


        bslma::TestAllocator da("default",   veryVeryVeryVerbose);
        bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

        {
            AllocatingTestType X(ARG1, &sa);

            ASSERT(0 == da.numBlocksTotal());
            ASSERT(2 == sa.numBlocksTotal());
            ASSERT(2 == sa.numBlocksInUse());

            AllocatingTestType Y(ARG1, ARG2, &sa);

            ASSERT(0 == da.numBlocksTotal());
            ASSERT(4 == sa.numBlocksTotal());
            ASSERT(4 == sa.numBlocksInUse());

            ASSERT( X.oneParamConstructorFlag());
            ASSERT(!X.twoParamsConstructorFlag());

            ASSERT(!Y.oneParamConstructorFlag());
            ASSERT( Y.twoParamsConstructorFlag());

            ASSERT(ARG1 == X.arg1());
            ASSERT(ARG1 == Y.arg1());
            ASSERT(ARG2 == Y.arg2());
        }

        ASSERT(0 == da.numBlocksTotal());
        ASSERT(0 == sa.numBlocksInUse());

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CTOR, PRIMARY MANIPULATORS, & DTOR
        // --------------------------------------------------------------------

        RUN_EACH_TYPE(TestDriver,
                      testCase2,
                      bsltf::AllocTestType);

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

            typedef BidirectionalNodePool<int, bsl::allocator<int> > Obj;

            bslma::TestAllocator da, ta;
            bslma::DefaultAllocatorGuard daGuard(&da);

            Obj x(&ta);
            IntNode *value = (IntNode *)x.createNode();
            ASSERT(0 != value);
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(1 == ta.numBlocksInUse());
            x.deleteNode(value);
            ASSERT(0 == da.numBlocksInUse());
            ASSERT(1 == ta.numBlocksInUse());

            value = (IntNode *)x.createNode(0xabcd);
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
            typedef BidirectionalNodePool<AllocType,
                                          bsl::allocator<AllocType> > Obj;
            typedef bslalg::BidirectionalNode<AllocatingIntType> Node;

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

    // CONCERN: No memory is ever allocated from the global allocator.
    ASSERTV(gam.isTotalSame());

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
