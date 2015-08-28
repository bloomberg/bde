// bdlma_multipoolallocator.t.cpp                                     -*-C++-*-
#include <bdlma_multipoolallocator.h>

#include <bdlma_bufferedsequentialallocator.h>   // for testing only

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_alignmentutil.h>
#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_map.h>
#include <bsl_set.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// 'bdlma::MultipoolAllocator' adapts the 'bdlma::Multipool' mechanism to the
// 'bdlma::ManagedAllocator' protocol.  The primary concerns are: 1) that the
// constructors configure the underlying multipool as expected, and 2) that the
// manipulators correctly proxy the memory allocation requests to the multipool
// that it adapts.  The 'bslma_testallocator' component is used extensively to
// verify expected behavior.  Note that memory allocation must be tested for
// exception neutrality (also via the 'bslma_testallocator' component).
// Several small helper functions are also used to facilitate testing.
//-----------------------------------------------------------------------------
// [ 3] MultipoolAllocator(Allocator *ba = 0);
// [ 2] MultipoolAllocator(numPools, Allocator *ba = 0);
// [ 3] MultipoolAllocator(gs, Allocator *ba = 0);
// [ 3] MultipoolAllocator(numPools, gs, Allocator *ba = 0);
// [ 3] MultipoolAllocator(numPools, gs, mbpc, Allocator *ba = 0);
// [ 3] MultipoolAllocator(numPools, *gs, Allocator *ba = 0);
// [ 3] MultipoolAllocator(numPools, *gs, mbpc, Allocator *ba = 0);
// [ 3] MultipoolAllocator(numPools, gs, *mbpc, Allocator *ba = 0);
// [ 3] MultipoolAllocator(numPools, *gs, *mbpc, Allocator *ba = 0);
// [ 2] ~MultipoolAllocator();
// [ 6] void reserveCapacity(size_type size, size_type numObjects);
// [ 2] void *allocate(size);
// [ 4] void deallocate(address);
// [ 5] void release();
// [ 7] int numPools() const;
// [ 7] int maxPooledBlockSize() const;
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                       GLOBAL TYPES AND CONSTANTS
//-----------------------------------------------------------------------------

typedef bdlma::MultipoolAllocator   Obj;

typedef bdlma::Multipool            MPool;
typedef bsls::BlockGrowth::Strategy Strategy;

const int MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

// Warning: keep this in sync with bdlma_multipool.h!
struct Header {
    // Stores pool number of this item.
    union {
        int                                 d_pool;   // pool for this item
        bsls::AlignmentUtil::MaxAlignedType d_dummy;  // force max. alignment
    } d_header;
};

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static
int calcPool(int numPools, int objSize)
    // Return the index of the pool that should allocate objects that are of
    // the specified 'objSize' bytes in size from a multipool managing the
    // specified 'numPools' memory pools, or -1 if 'objSize' exceeds the size
    // of the blocks managed by all of the pools.  The behavior is undefined
    // unless '0 < numPools' and '0 < objSize'.
{
    ASSERT(0 < numPools);
    ASSERT(0 < objSize);

    int pool     = 0;
    int poolSize = 8;

    while (objSize > poolSize) {
        poolSize *= 2;
        ++pool;
    }

    if (pool >= numPools) {
        pool = -1;
    }

    return pool;
}

static inline
int recPool(char *address)
    // Return the index of the pool that allocated the memory at the specified
    // 'address', or -1 if the memory was allocated directly from the
    // underlying allocator.  The behavior is undefined unless 'address' is
    // non-null.
{
    ASSERT(address);

    Header *h = (Header *)address - 1;

    return h->d_header.d_pool;
}

void stretchRemoveAll(Obj *object, int numElements, int objSize)
    // Using only primary manipulators, extend the capacity of the specified
    // 'object' to (at least) the specified 'numElements' each of the specified
    // 'objSize' bytes, then remove all elements leaving 'object' empty.  The
    // behavior is undefined unless '0 <= numElements' and '0 <= objSize'.
{
    ASSERT(object);
    ASSERT(0 <= numElements);
    ASSERT(0 <= objSize);

    for (int i = 0; i < numElements; ++i) {
        object->allocate(objSize);
    }
    object->release();
}

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Using a 'bdlma::MultipoolAllocator'
///- - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::MultipoolAllocator' can be used to supply memory to node-based
// data structures such as 'bsl::set', 'bsl::list', and 'bsl::map'.  Suppose we
// are implementing a container of named graphs, where a graph is defined by a
// set of edges and a set of nodes.  The various fixed-sized nodes and edges
// can be efficiently allocated by a 'bdlma::MultipoolAllocator'.
//
// First, the edge class, 'my_Edge', is defined as follows:
//..
    class my_Node;

    class my_Edge {
        // This class represents an edge within a graph.  Both ends of an edge
        // must be connected to nodes.

        // DATA
        my_Node *d_first;   // first node
        my_Node *d_second;  // second node

        // ...

      public:
        // CREATORS
        my_Edge(my_Node *first, my_Node *second);
            // Create an edge that connects to the specified 'first' and
            // 'second' nodes.

        // ...
    };

    // CREATORS
    my_Edge::my_Edge(my_Node *first, my_Node *second)
    : d_first(first)
    , d_second(second)
    {
    }
//..
// Then, the node class, 'my_Node', is defined as follows:
//..
    class my_Node {
        // This class represents a node within a graph.  A node can be
        // connected to any number of edges.

        // DATA
        bsl::set<my_Edge *> d_edges;  // set of edges this node connects to

        // ...

      private:
        // Not implemented:
        my_Node(const my_Node&);

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(my_Node, bslma::UsesBslmaAllocator);

        // CREATORS
        explicit my_Node(bslma::Allocator *basicAllocator = 0);
            // Create a node not connected to any other nodes.  Optionally
            // specify a 'basicAllocator' used to supply memory.  If
            // 'basicAllocator' is 0, the currently installed default allocator
            // is used.

        // ...
    };

    // CREATORS
    my_Node::my_Node(bslma::Allocator *basicAllocator)
    : d_edges(basicAllocator)
    {
    }
//..
// Then, we define the graph class, 'my_Graph', as follows:
//..
    class my_Graph {
        // This class represents a graph having sets of nodes and edges.

        // DATA
        bsl::set<my_Edge> d_edges;  // set of edges in this graph
        bsl::set<my_Node> d_nodes;  // set of nodes in this graph

        // ...

      private:
        // Not implemented:
        my_Graph(const my_Graph&);

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(my_Graph, bslma::UsesBslmaAllocator);

        // CREATORS
        explicit my_Graph(bslma::Allocator *basicAllocator = 0);
            // Create an empty graph.  Optionally specify a 'basicAllocator'
            // used to supply memory.  If 'basicAllocator' is 0, the currently
            // installed default allocator is used.

        // ...
    };

    my_Graph::my_Graph(bslma::Allocator *basicAllocator)
    : d_edges(basicAllocator)
    , d_nodes(basicAllocator)
    {
    }
//..
// Next, the container for the collection of named graphs,
// 'my_NamedGraphContainer', is defined as follows:
//..
    class my_NamedGraphContainer {
        // This class stores a map that indexes graph names to graph objects.

        // DATA
        bsl::map<bsl::string, my_Graph> d_graphMap;  // map from graph name to
                                                     // graph

      private:
        // Not implemented:
        my_NamedGraphContainer(const my_NamedGraphContainer&);

      public:
        // TRAITS
        BSLMF_NESTED_TRAIT_DECLARATION(my_NamedGraphContainer,
                                       bslma::UsesBslmaAllocator);

        // CREATORS
        explicit my_NamedGraphContainer(bslma::Allocator *basicAllocator = 0);
            // Create an empty named graph container.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        // ...
    };

    // CREATORS
    my_NamedGraphContainer::my_NamedGraphContainer(
                                              bslma::Allocator *basicAllocator)
    : d_graphMap(basicAllocator)
    {
    }
//..
// Finally, in 'main', we can create a 'bdlma::MultipoolAllocator' and pass it
// to our 'my_NamedGraphContainer'.  Since we know that the maximum block size
// needed is 32 ('sizeof(my_Graph)'), we can calculate the number of pools
// needed by using the formula given in the "Configuration at Construction"
// section:
//..
//  largestPoolSize == 2 ^ (N + 2)
//..
// When solved for the above equation, the smallest 'N' that satisfies this
// relationship is 3:
//..
//  int main()
//  {
//      enum { k_NUM_POOLS = 3 };
//
//      bdlma::MultipoolAllocator multipoolAllocator(NUM_POOLS);
//
//      my_NamedGraphContainer container(&multipoolAllocator);
//  }
//..
//
///Example 2: Performance of a 'bdlma::MultipoolAllocator'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::MultipoolAllocator' can greatly improve efficiency when it is used
// to supply memory to node-based data structures that frequently both insert
// and remove nodes, while growing to significant size before being destroyed.
// The following experiment will illustrate the benefits of using a
// 'bdlma::MultipoolAllocator' under this scenario by comparing the following 3
// different allocator uses:
//
//: 1 Using the 'bslma::NewDeleteAllocator'.
//:
//: 2 Using a 'bdlma::MultipoolAllocator' as a substitute for the
//:   'bslma::NewDeleteAllocator'.
//:
//: 3 Exploiting the managed aspect of 'bdlma::MultipoolAllocator' by avoiding
//:   invocation of the destructor of the data structure, since the
//:   destruction of the allocator will automatically reclaim all memory.
//
// First, we create a test data structure that contains three 'bsl::list's.
// Each list holds a different type of object, where each type has a different
// size.  For simplicity, we create these different object types as different
// instantiations of a template class, parameterized on the object size:
//..
    template <int OBJECT_SIZE>
    class my_TestObject {

        // DATA
        char d_data[OBJECT_SIZE];
    };
//..
// Again, for simplicity, the sizes of these objects are chosen to be 20, 40,
// and 80, instead of being parameterized as part of the test data structure:
//..
    class my_TestDataStructure {

        // PRIVATE TYPES
        typedef my_TestObject<20> Obj1;
        typedef my_TestObject<40> Obj2;
        typedef my_TestObject<80> Obj3;

        // DATA
        bsl::list<Obj1> d_list1;
        bsl::list<Obj2> d_list2;
        bsl::list<Obj3> d_list3;

      private:
        // Not implemented:
        my_TestDataStructure(const my_TestDataStructure&);
//..
// The test will consist of the following steps:
//
//: 1 Push back into 'd_list1', then 'd_list2', then 'd_list3'.
//: 2 Repeat #1.
//: 3 Pop front from 'd_list1', then 'd_list2', then 'd_list3'.
//
// The above 3 steps will be repeated 'n' times, where 'n' is a parameter
// specified by the user.  This process will both grow the list and incorporate
// a large number of node removals.  Note that nodes are removed from the front
// of the list to simulate a particular real-world usage, where nodes removed
// are rarely those recently added (this also removes the possibility of noise
// from potential optimizations with relinquishing memory blocks that are most
// recently allocated).
//..
      public:
        // CREATORS
        my_TestDataStructure(bslma::Allocator *basicAllocator = 0);

        // MANIPULATORS
        void pop();

        void push();
    };

    // CREATORS
    my_TestDataStructure::my_TestDataStructure(
                                              bslma::Allocator *basicAllocator)
    : d_list1(basicAllocator)
    , d_list2(basicAllocator)
    , d_list3(basicAllocator)
    {
    }
//..
// The 'push' method will push into the 3 'bsl::list' objects managed by
// 'my_TestDataStructure' sequentially.  Similarly, the 'pop' method will pop
// from the lists sequentially:
//..
    // MANIPULATORS
    void my_TestDataStructure::push()
    {
        // Push to the back of the 3 lists.

        d_list1.push_back(Obj1());
        d_list2.push_back(Obj2());
        d_list3.push_back(Obj3());
    }

    void my_TestDataStructure::pop()
    {
        // Pop from the front of the 3 lists.

        d_list1.pop_front();
        d_list2.pop_front();
        d_list3.pop_front();
    }
//..
// The above 'push' and 'pop' methods will allow us to evaluate the cost to add
// and remove nodes using different allocators.  To evaluate the cost of
// destruction (and hence deallocation of all allocated memory in the list
// objects), we supply a 'static' 'test' method within a 'my_TestUtil' class to
// create the test mechanism, run the test, and destroy the test mechanism.
//
// The 'test' method accepts a 'testLengthFactor' argument specified by the
// user to control the length of the test.  The effect of 'testLengthFactor' is
// shown below:
//..
//  testLengthFactor            test size           n      iterations
//  ----------------     ----------------     --------     ----------
//        4                  10^4 = 10000           1          10000
//                                                 10           1000
//                                                100            100
//                                               1000             10
//                                              10000              1
//
//        5                 10^5 = 100000           1         100000
//                                                 10          10000
//                                                100           1000
//                                               1000            100
//                                              10000             10
//                                             100000              1
//
//        6                10^6 = 1000000           1        1000000
//                                                 10         100000
//                                                100          10000
//                                               1000           1000
//                                              10000            100
//                                             100000             10
//                                            1000000              1
//
//   // ...
//..
// For each row of the specified 'testLengthFactor', a 'my_TestDataStructure'
// will be created "iterations" times, and each time the lists within the data
// structure will grow by invoking 'push' twice and 'pop' once.  Note that "n"
// measures the effect of insertion and removal of nodes, while "iterations"
// measures the effect of construction and destruction of entire lists of
// nodes.
//
// The 'test' method also accepts a 'bslma::Allocator *' to be used as the
// allocator used to construct the test mechanism and its internal lists:
//..
    class my_TestUtil {

      public:
        // CLASS METHODS
        static
        void test(int testLengthFactor, bslma::Allocator *basicAllocator)
        {
            int n          = 1;
            int iterations = 1;

            for (int i = 0; i < testLengthFactor; ++i) {
                iterations *= 10;
            }

            for (int i = 0; i <= testLengthFactor; ++i) {
                bsls::Stopwatch timer;
                timer.start();

                for (int j = 0; j < iterations; ++j) {
                    my_TestDataStructure tds(basicAllocator);

                    // Testing cost of insertion and deletion.

                    for (int k = 0; k < n; ++k) {
                        tds.push();
                        tds.push();
                        tds.pop();
                    }

                    // Testing cost of destruction.
                }

                timer.stop();

                printf("%d\t%d\t%d\t%1.6f\n", testLengthFactor,
                                              n,
                                              iterations,
                                              timer.elapsedTime());

                n          *= 10;
                iterations /= 10;
            }
        }
//..
// Next, to fully test the benefit of being able to relinquish all allocated
// memory at once, we use the 'testManaged' method, which accepts only managed
// allocators.  Instead of creating the test mechanism on the stack, the test
// mechanism will be created on the heap.  After running the test, the
// 'release' method of the allocator will reclaim all outstanding allocations
// at once, eliminating the need to run the destructor of the test mechanism:
//..
        static
        void testManaged(int                      testLengthFactor,
                         bdlma::ManagedAllocator *managedAllocator)
        {
            int n          = 1;
            int iterations = 1;

            for (int i = 0; i < testLengthFactor; ++i) {
                iterations *= 10;
            }

            for (int i = 0; i <= testLengthFactor; ++i) {
                bsls::Stopwatch timer;
                timer.start();

                for (int j = 0; j < iterations; ++j) {
                    my_TestDataStructure *tmPtr = new(*managedAllocator)
                                        my_TestDataStructure(managedAllocator);

                    // Testing cost of insertion and deletion.

                    for (int k = 0; k < n; ++k) {
                        tmPtr->push();
                        tmPtr->push();
                        tmPtr->pop();
                    }

                    // Testing cost of destruction.

                    managedAllocator->release();
                }

                timer.stop();

                printf("%d\t%d\t%d\t%1.6f\n", testLengthFactor,
                                              n,
                                              iterations,
                                              timer.elapsedTime());

                n          *= 10;
                iterations /= 10;
            }
        }
    };
//..
// Finally, in main, we run the test with the different allocators and
// different allocator configurations based on command line arguments:
//..
//  {
//      int testLengthFactor = 5;
//      const int NUM_POOLS  = 10;
//
//      if (argc > 2) {
//          testLengthFactor = bsl::atoi(argv[2]);
//      }
//
//      char growth = 'g';
//      if (argc > 3) {
//          growth = argv[3][0];
//          if (growth != 'g' && growth != 'c') {
//              printf("[g]eometric or [c]onstant growth must be used\n");
//              return -1;
//          }
//      }
//
//      int maxChunkSize = 32;
//      if (argc > 4) {
//          maxChunkSize = bsl::atoi(argv[4]);
//          if (maxChunkSize <= 0) {
//              printf("maxChunkSize must be >= 1");
//          }
//      }
//
//      bsls::BlockGrowth::Strategy strategy = growth == 'g'
//                                          ? bsls::BlockGrowth::BSLS_GEOMETRIC
//                                          : bsls::BlockGrowth::BSLS_CONSTANT;
//
//      printf("\nNew Delete Allocator:\n\n");
//      {
//          bslma::Allocator *nda = bslma::NewDeleteAllocator::allocator(0);
//          my_TestUtil::test(testLengthFactor, nda);
//      }
//
//      printf("\nMultipool Allocator with [%c], [%d]:\n\n", growth,
//                                                               maxChunkSize);
//      {
//          bdlma::MultipoolAllocator ma(NUM_POOLS, strategy, maxChunkSize);
//          my_TestUtil::test(testLengthFactor, &ma);
//      }
//
//      printf("\nMultipool Allocator Managed with [%c], [%d]:\n\n", growth,
//                                                               maxChunkSize);
//      {
//          bdlma::MultipoolAllocator ma(NUM_POOLS, strategy, maxChunkSize);
//          my_TestUtil::testManaged(testLengthFactor, &ma);
//      }
//
//      return 0;
//  }
//..
// An excerpt of the results of the test running on IBM under optimized mode,
// using default constructed 'bdlma::MultipoolAllocator' parameters, is shown
// below:
//..
//  New Delete Allocator:
//
//  6       1       1000000 3.006253
//  6       10      100000  2.369734
//  6       100     10000   2.598567
//  6       1000    1000    2.604546
//  6       10000   100     2.760319
//  6       100000  10      3.085765
//  6       1000000 1       4.465030
//
//  Multipool Allocator with [g], [32]:
//
//  6       1       1000000 0.766064
//  6       10      100000  0.408509
//  6       100     10000   0.357019
//  6       1000    1000    0.436448
//  6       10000   100     0.643206
//  6       100000  10      0.932662
//  6       1000000 1       0.938906
//
//  Multipool Allocator Managed with [g], [32]:
//
//  6       1       1000000 1.958663
//  6       10      100000  0.463185
//  6       100     10000   0.371201
//  6       1000    1000    0.357816
//  6       10000   100     0.368082
//  6       100000  10      0.388422
//  6       1000000 1       0.529167
//..
// It is clear that using a 'bdlma::MultipoolAllocator' results in an
// improvement in memory allocation by a factor of about 4.  Furthermore, if
// the managed aspect of the multipool allocator is exploited, the cost of
// destruction rapidly decreases in relative terms as the list grows larger
// (increasing 'n').

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

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator     *Z = &testAllocator;

    switch (test) { case 0:
      case 8: {
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

// Finally, in 'main', we can create a 'bdlma::MultipoolAllocator' and pass it
// to our 'my_NamedGraphContainer'.  Since we know that the maximum block size
// needed is 32 ('sizeof(my_Graph)'), we can calculate the number of pools
// needed by using the formula given in the "Configuration at Construction"
// section:
//..
//  largestPoolSize == 2 ^ (N + 2)
//..
// When solved for the above equation, the smallest 'N' that satisfies this
// relationship is 3:
//..
//  int main()
//  {
        enum { k_NUM_POOLS = 3 };

        bdlma::MultipoolAllocator multipoolAllocator(k_NUM_POOLS);

        my_NamedGraphContainer container(&multipoolAllocator);
//  }
//..

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING 'numPools' and 'maxPooledBlockSize'
        //
        // Concerns:
        //   1) That 'numPools' returns the number of pools managed by the
        //      multipool allocator.
        //
        //   2) That 'maxPooledBlockSize' returns the correct maximum block
        //      size managed by the multipool allocator given the specified
        //      'numPools'.
        //
        // Plan:
        //   Since the constructors are thoroughly tested at this point, simply
        //   construct a multipool allocator passing in different 'numPools'
        //   arguments specified in a test array, and verify that 'numPools'
        //   and 'maxPooledBlockSize' return the expected values.
        //
        // Testing:
        //   int numPools() const;
        //   int maxPooledBlockSize() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                      << "TESTING 'numPools' and 'maxPooledBlockSize'" << endl
                      << "===========================================" << endl;

        static const struct {
            int d_lineNum;       // line number
            int d_numPools;      // number of pools
            int d_maxBlockSize;  // maximum block size
        } DATA[] = {
            //LINE  # POOLS     MAXIMUM BLOCK SIZE
            //----  -------     ------------------
            { L_,   1,          8                  },
            { L_,   2,          16                 },
            { L_,   3,          32                 },
            { L_,   4,          64                 },
            { L_,   5,          128                },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE         = DATA[i].d_lineNum;
            const int NUMPOOLS     = DATA[i].d_numPools;
            const int MAXBLOCKSIZE = DATA[i].d_maxBlockSize;

            if (veryVerbose) {
                P_(LINE) P_(NUMPOOLS) P(MAXBLOCKSIZE)
            }

            Obj mX(NUMPOOLS, &testAllocator);  const Obj& X = mX;

            LOOP2_ASSERT(NUMPOOLS, X.numPools(),
                         NUMPOOLS == X.numPools());

            LOOP2_ASSERT(MAXBLOCKSIZE, X.maxPooledBlockSize(),
                         MAXBLOCKSIZE == X.maxPooledBlockSize());
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // RESERVECAPACITY TEST
        //
        // Concerns:
        //   Our primary concern is that 'reserveCapacity(sz, n)' reserves
        //   sufficient memory to satisfy 'n' allocation requests from the
        //   pool managing objects of size 'sz'.
        //
        //   QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //   To test 'reserveCapacity', specify a table of capacities to
        //   reserve.  Construct an object managing three pools and call
        //   'reserveCapacity' for each of the three pools with the tabulated
        //   number of elements.  Allocate as many objects as required to
        //   bring the size of the pool under test to the specified number of
        //   elements and use 'bslma::TestAllocator' to verify that no
        //   additional allocations have occurred.  Perform each test in the
        //   standard 'bslma' exception-testing macro block.
        //
        // Testing:
        //   void reserveCapacity(size_type size, size_type numObjects);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RESERVECAPACITY TEST" << endl
                                  << "====================" << endl;

        if (verbose) cout << "Testing 'reserveCapacity'." << endl;

        {
            const int DATA[] = {
                0, 1, 2, 3, 4, 5, 15, 16, 17
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            static const int EXTEND[] = {
                0, 1, 4, 5, 7, 17, 23, 100
            };
            const int NUM_EXTEND = sizeof EXTEND / sizeof *EXTEND;

            static const int OSIZE[] = {  // enough for 3 pools
                MAX_ALIGN, MAX_ALIGN * 2, MAX_ALIGN * 4
            };
            const int NUM_OSIZE = sizeof OSIZE / sizeof *OSIZE;

            const int NUM_POOLS = 4;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int NE = DATA[ti];
                if (veryVerbose) { cout << "\t\t"; P(NE); }

                for (int zi = 0; zi < NUM_OSIZE; ++zi) {
                    const int OBJ_SIZE = OSIZE[zi];
                    for (int ei = 0; ei < NUM_EXTEND; ++ei) {
                      BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                        Obj mX(NUM_POOLS, Z);
                        stretchRemoveAll(&mX, EXTEND[ei], OBJ_SIZE);
                        mX.reserveCapacity(OBJ_SIZE, 0);
                        mX.reserveCapacity(OBJ_SIZE, NE);
                        const bsls::Types::Int64 NUM_BLOCKS =
                                                testAllocator.numBlocksTotal();
                        const bsls::Types::Int64 NUM_BYTES  =
                                                 testAllocator.numBytesInUse();
                        for (int i = 0; i < NE; ++i) {
                            mX.allocate(OBJ_SIZE);
                        }
                        LOOP3_ASSERT(ti, ei, zi, NUM_BLOCKS ==
                                               testAllocator.numBlocksTotal());
                        LOOP3_ASSERT(ti, ei, zi, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                      } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                    }
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX(2);

            {
                ASSERT_SAFE_PASS_RAW(mX.reserveCapacity(16,  1));

                ASSERT_SAFE_FAIL_RAW(mX.reserveCapacity(17,  1));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING RELEASE
        //
        // Concerns:
        //   Our primary concern is that 'release' relinquishes all outstanding
        //   memory to the allocator that is in use by the multipool allocator.
        //
        // Plan:
        //   Create multipool allocators that manage a varying number of pools
        //   and make many allocation requests from each of the pools, as well
        //   as from the "overflow" block list.  Make use of the facilities
        //   available in 'bslma::TestAllocator' to monitor memory usage.
        //   Verify with appropriate assertions that all memory is indeed
        //   relinquished to the memory allocator following each 'release'.
        //
        // Testing:
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING RELEASE"
                          << endl << "===============" << endl;

        const int MAX_POOLS     = 3;
        const int POOL_QUANTA[] = { MAX_ALIGN, MAX_ALIGN * 2, MAX_ALIGN * 4 };
        const int OVERFLOW_SIZE = MAX_ALIGN * 5;
        const int NITERS        = MAX_ALIGN * 256;

        for (int i = 1; i <= MAX_POOLS; ++i) {
            if (veryVerbose) { T_ cout << "# pools: "; P(i); }
            Obj mX(i, Z);
            const bsls::Types::Int64 NUM_BLOCKS =
                                                testAllocator.numBlocksInUse();
            const bsls::Types::Int64 NUM_BYTES  =
                                                 testAllocator.numBytesInUse();
            int its              = NITERS;
            while (its-- > 0) {  // exercise each pool, as well as "overflow"
                char *p;
                for (int j = 0; j < i; ++j) {
                    const int OBJ_SIZE = POOL_QUANTA[j];
                    p = (char *) mX.allocate(OBJ_SIZE);  // garbage-collected
                                                         //  by 'release'
                    LOOP3_ASSERT(i, j, its, p);
                }

                p = (char *) mX.allocate(OVERFLOW_SIZE);
                LOOP2_ASSERT(i, its, p);

                mX.release();
                LOOP2_ASSERT(i, its, NUM_BLOCKS ==
                                               testAllocator.numBlocksInUse());
                LOOP2_ASSERT(i, its, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING DEALLOCATE
        //
        // Concerns:
        //   Our primary concern is that 'deallocate' returns the block of
        //   memory to the underlying pool making it available for future
        //   allocation.
        //
        // Plan:
        //   Create multipool allocators that manage a varying number of pools
        //   and make many allocation requests from each of the pools, as well
        //   as from the "overflow" block list.  Make use of the facilities
        //   available in 'bslma::TestAllocator' to monitor memory usage.
        //   Verify with appropriate assertions that no demands are put on the
        //   memory allocation beyond those attributable to start-up.
        //
        // Testing:
        //   void deallocate(void *address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING DEALLOCATE"
                          << endl << "==================" << endl;

        const int MAX_POOLS     = 3;
        const int POOL_QUANTA[] = { MAX_ALIGN, MAX_ALIGN * 2, MAX_ALIGN * 4 };
        const int OVERFLOW_SIZE = MAX_ALIGN * 5;
        const int NITERS        = MAX_ALIGN * 256;

        bsls::Types::Int64 numBlocks;
        bsls::Types::Int64 numBytes;

        for (int i = 1; i <= MAX_POOLS; ++i) {
            if (veryVerbose) { T_ cout << "# pools: "; P(i); }
            Obj mX(i + 1, Z);
            for (int j = 0; j < i; ++j) {
                if (veryVerbose) { T_ T_ cout << "pool: "; P(j); }
                const int OBJ_SIZE = POOL_QUANTA[j];
                int its            = NITERS;
                int firstTime      = 1;
                while (its-- > 0) {  // exercise each pool
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    LOOP2_ASSERT(i, j, p);
                    if (firstTime) {
                        numBlocks = testAllocator.numBlocksTotal();
                        numBytes  = testAllocator.numBytesInUse();
                        firstTime = 0;
                    }
                    else {
                        LOOP2_ASSERT(i, j, numBlocks ==
                                               testAllocator.numBlocksTotal());
                        LOOP2_ASSERT(i, j, numBytes  ==
                                                testAllocator.numBytesInUse());
                    }
                    mX.deallocate(p);
                }
            }

            if (veryVerbose) { T_ T_ cout << "overflow\n"; }
            int its       = NITERS;
            int firstTime = 1;
            while (its-- > 0) {  // exercise "overflow" block list
                char *p = (char *) mX.allocate(OVERFLOW_SIZE);
                LOOP2_ASSERT(i, its, p);
                if (firstTime) {
                    numBlocks = testAllocator.numBlocksInUse();
                    numBytes  = testAllocator.numBytesInUse();
                    firstTime = 0;
                }
                else {
                    LOOP2_ASSERT(i, its, numBlocks ==
                                               testAllocator.numBlocksInUse());
                    LOOP2_ASSERT(i, its, numBytes ==
                                                testAllocator.numBytesInUse());
                }
                mX.deallocate(p);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ALL CTORS
        //
        // Concerns:
        //   1. That all arguments passed to the multipool allocator
        //      constructors have the expected effect.
        //
        //   2. That the default growth strategy is geometric growth.
        //
        //   3. That the default maximum blocks per chunk is 32.
        //
        //   QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //   Compare allocations between a multipool and a multipool allocator
        //   that are constructed with identical arguments.
        //
        // Testing:
        //   MultipoolAllocator(Allocator *ba = 0);
        //   MultipoolAllocator(gs, Allocator *ba = 0);
        //   MultipoolAllocator(numPools, gs, Allocator *ba = 0);
        //   MultipoolAllocator(numPools, gs, mbpc, Allocator *ba = 0);
        //   MultipoolAllocator(numPools, *gs, Allocator *ba = 0);
        //   MultipoolAllocator(numPools, *gs, mbpc, Allocator *ba = 0);
        //   MultipoolAllocator(numPools, gs, *mbpc, Allocator *ba = 0);
        //   MultipoolAllocator(numPools, *gs, *mbpc, Allocator *ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING ALL CTORS" << endl
                                  << "=================" << endl;

        enum {
            k_INITIAL_CHUNK_SIZE     =  1,
            k_DEFAULT_MAX_CHUNK_SIZE = 32,
            k_DEFAULT_NUM_POOLS      = 10
        };

        const int NUM_POOLS           =  5;
        const int TEST_MAX_CHUNK_SIZE = 30;

        // Object Data
        const int ODATA[]   = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
        const int NUM_ODATA = sizeof ODATA / sizeof *ODATA;

        const Strategy GEO = bsls::BlockGrowth::BSLS_GEOMETRIC;
        const Strategy CON = bsls::BlockGrowth::BSLS_CONSTANT;

        // Strategy Data
        const Strategy SDATA[][5] = {
            { GEO, GEO, GEO, GEO, GEO },
            { CON, CON, CON, CON, CON },
            { GEO, CON, GEO, CON, GEO },
            { CON, GEO, CON, GEO, CON }
        };
        const int NUM_SDATA = sizeof SDATA / sizeof *SDATA;

        // MaxBlocksPerChunk Data
        const int MDATA[][5] = {
            { 32, 32, 32, 32, 32 },
            { 30, 30, 30, 30, 30 },
            { 30, 32, 30, 32, 30 },
            { 32, 30, 32, 30, 32 }
        };
        const int NUM_MDATA = sizeof MDATA / sizeof *MDATA;

        if (verbose) {
            cout << "'bdlma::MultipoolAllocator(numPools, *gs, *mbpc, *ba)'"
                 << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            for (int mi = 0; mi < NUM_MDATA; ++mi) {

                bslma::TestAllocator mpta("multipool", veryVeryVerbose);
                bslma::TestAllocator   oa("object",    veryVeryVerbose);

                MPool mp(NUM_POOLS, SDATA[si], MDATA[mi], &mpta);

                Obj mX(NUM_POOLS, SDATA[si], MDATA[mi], &oa);

                bsls::Types::Int64 multipoolAllocations =
                                                         mpta.numAllocations();
                bsls::Types::Int64 objectAllocations    = oa.numAllocations();

                LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                             multipoolAllocations == objectAllocations);

                for (int oi = 0; oi < NUM_ODATA; ++oi) {
                    const int OBJ_SIZE    = ODATA[oi];
                    const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                    if (-1 == calcPoolNum) {
                        char *p = (char *) mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                        continue;
                    }

                    LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                    // Testing geometric growth.
                    if (GEO == SDATA[si][calcPoolNum]) {
                        int ri = k_INITIAL_CHUNK_SIZE;
                        while (ri < MDATA[mi][calcPoolNum]) {
                            multipoolAllocations = mpta.numAllocations();
                            objectAllocations    =   oa.numAllocations();

                            for (int j = 0; j < ri; ++j) {
                                char *p = (char *)mX.allocate(OBJ_SIZE);
                                const int recordPoolNum = recPool(p);

                                LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                             recordPoolNum == calcPoolNum);

                                mp.allocate(OBJ_SIZE);
                            }

                            LOOP2_ASSERT(multipoolAllocations,
                                         objectAllocations,
                                         multipoolAllocations
                                                         == objectAllocations);

                            ri <<= 1;
                        }
                    }

                    // Testing constant growth (also applies to capped
                    // geometric growth).
                    const int NUM_REPLENISH = 3;
                    for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                        multipoolAllocations = mpta.numAllocations();
                        objectAllocations    =   oa.numAllocations();

                        for (int j = 0; j < MDATA[mi][calcPoolNum]; ++j) {
                            char *p = (char *)mX.allocate(OBJ_SIZE);
                            const int recordPoolNum = recPool(p);

                            LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                            mp.allocate(OBJ_SIZE);
                        }

                        LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                    multipoolAllocations == objectAllocations);
                    }
                }
            }
        }

        if (verbose) {
            cout << "'bdlma::MultipoolAllocator(*ba)'" << endl;
        }

        {
            bslma::TestAllocator mpta("multipool", veryVeryVerbose);
            bslma::TestAllocator   oa("object",    veryVeryVerbose);

            MPool mp(&mpta);

            Obj mX(&oa);

            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();
            bsls::Types::Int64 objectAllocations    =   oa.numAllocations();

            LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                         multipoolAllocations == objectAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(k_DEFAULT_NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < k_DEFAULT_NUM_POOLS);

                // Testing geometric growth.
                int ri = k_INITIAL_CHUNK_SIZE;
                while (ri < k_DEFAULT_MAX_CHUNK_SIZE) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < ri; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);

                    ri <<= 1;
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);
                }
            }
        }

        if (verbose) {
            cout << "'bdlma::MultipoolAllocator(numPools, *ba)'" << endl;
        }
        {
            bslma::TestAllocator mpta("multipool", veryVeryVerbose);
            bslma::TestAllocator   oa("object",    veryVeryVerbose);

            MPool mp(NUM_POOLS, &mpta);

            Obj mX(NUM_POOLS, &oa);

            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();
            bsls::Types::Int64 objectAllocations    =   oa.numAllocations();

            LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                         multipoolAllocations == objectAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth.
                int ri = k_INITIAL_CHUNK_SIZE;
                while (ri < k_DEFAULT_MAX_CHUNK_SIZE) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < ri; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);

                    ri <<= 1;
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);
                }
            }
        }

        if (verbose) {
            cout << "'bdlma::MultipoolAllocator(gs, *ba)'" << endl;
        }
        {
            bslma::TestAllocator mpta("multipool", veryVeryVerbose);
            bslma::TestAllocator   oa("object",    veryVeryVerbose);

            MPool mp(CON, &mpta);

            Obj mX(CON, &oa);

            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();
            bsls::Types::Int64 objectAllocations    =   oa.numAllocations();

            LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                         multipoolAllocations == objectAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(k_DEFAULT_NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < k_DEFAULT_NUM_POOLS);

                // Testing constant growth.
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);
                }
            }
        }

        if (verbose) {
            cout << "'bdlma::MultipoolAllocator(numPools, gs, *ba)'" << endl;
        }
        {
            bslma::TestAllocator mpta("multipool", veryVeryVerbose);
            bslma::TestAllocator   oa("object",    veryVeryVerbose);

            MPool mp(NUM_POOLS, CON, &mpta);

            Obj mX(NUM_POOLS, CON, &oa);

            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();
            bsls::Types::Int64 objectAllocations    =   oa.numAllocations();

            LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                         multipoolAllocations == objectAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing constant growth.
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);
                }
            }
        }

        if (verbose) {
            cout << "'bdlma::MultipoolAllocator(numPools, *gs, *ba)'" << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            bslma::TestAllocator mpta("multipool", veryVeryVerbose);
            bslma::TestAllocator   oa("object",    veryVeryVerbose);

            MPool mp(NUM_POOLS, SDATA[si], &mpta);

            Obj mX(NUM_POOLS, SDATA[si], &oa);

            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();
            bsls::Types::Int64 objectAllocations    =   oa.numAllocations();

            LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                         multipoolAllocations == objectAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth.
                if (GEO == SDATA[si][calcPoolNum]) {
                    int ri = k_INITIAL_CHUNK_SIZE;
                    while (ri < k_DEFAULT_MAX_CHUNK_SIZE) {
                        multipoolAllocations = mpta.numAllocations();
                        objectAllocations    =   oa.numAllocations();

                        for (int j = 0; j < ri; ++j) {
                            char *p = (char *)mX.allocate(OBJ_SIZE);
                            const int recordPoolNum = recPool(p);

                            LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                            mp.allocate(OBJ_SIZE);
                        }

                        LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                    multipoolAllocations == objectAllocations);

                        ri <<= 1;
                    }
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);
                }
            }
        }

        if (verbose) {
            cout << "'bdlma::MultipoolAllocator(numPools, gs, mbpc, *ba)'"
                 << endl;
        }
        {
            bslma::TestAllocator mpta("multipool", veryVeryVerbose);
            bslma::TestAllocator   oa("object",    veryVeryVerbose);

            MPool mp(NUM_POOLS, GEO, TEST_MAX_CHUNK_SIZE, &mpta);

            Obj mX(NUM_POOLS, GEO, TEST_MAX_CHUNK_SIZE, &oa);

            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();
            bsls::Types::Int64 objectAllocations    =   oa.numAllocations();

            LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                         multipoolAllocations == objectAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth.
                int ri = k_INITIAL_CHUNK_SIZE;
                while (ri < TEST_MAX_CHUNK_SIZE) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < ri; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);

                    ri <<= 1;
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < TEST_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);
                }
            }
        }

        if (verbose) {
            cout << "'bdlma::MultipoolAllocator(numPools, gs, *mbpc, *ba)'"
                 << endl;
        }
        for (int mi = 0; mi < NUM_MDATA; ++mi) {

            bslma::TestAllocator mpta("multipool", veryVeryVerbose);
            bslma::TestAllocator   oa("object",    veryVeryVerbose);

            MPool mp(NUM_POOLS, GEO, MDATA[mi], &mpta);

            Obj mX(NUM_POOLS, GEO, MDATA[mi], &oa);

            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();
            bsls::Types::Int64 objectAllocations    =   oa.numAllocations();

            LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                         multipoolAllocations == objectAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth.
                int ri = k_INITIAL_CHUNK_SIZE;
                while (ri < MDATA[mi][calcPoolNum]) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < ri; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);

                    ri <<= 1;
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < MDATA[mi][calcPoolNum]; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);
                }
            }
        }

        if (verbose) {
            cout << "'bdlma::MultipoolAllocator(numPools, gs, mbpc, *ba)'"
                 << endl;
        }
        {
            bslma::TestAllocator mpta("multipool", veryVeryVerbose);
            bslma::TestAllocator   oa("object",    veryVeryVerbose);

            MPool mp(NUM_POOLS, CON, TEST_MAX_CHUNK_SIZE, &mpta);

            Obj mX(NUM_POOLS, CON, TEST_MAX_CHUNK_SIZE, &oa);

            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();
            bsls::Types::Int64 objectAllocations    =   oa.numAllocations();

            LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                         multipoolAllocations == objectAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing constant growth.
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < TEST_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);
                }
            }
        }

        if (verbose) {
            cout << "'bdlma::MultipoolAllocator(numPools, *gs, mbpc, *ba)'"
                 << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            bslma::TestAllocator mpta("multipool", veryVeryVerbose);
            bslma::TestAllocator   oa("object",    veryVeryVerbose);

            MPool mp(NUM_POOLS, SDATA[si], TEST_MAX_CHUNK_SIZE, &mpta);

            Obj mX(NUM_POOLS, SDATA[si], TEST_MAX_CHUNK_SIZE, &oa);

            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();
            bsls::Types::Int64 objectAllocations    =   oa.numAllocations();

            LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                         multipoolAllocations == objectAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth.
                if (GEO == SDATA[si][calcPoolNum]) {
                    int ri = k_INITIAL_CHUNK_SIZE;
                    while (ri < TEST_MAX_CHUNK_SIZE) {
                        multipoolAllocations = mpta.numAllocations();
                        objectAllocations    =   oa.numAllocations();

                        for (int j = 0; j < ri; ++j) {
                            char *p = (char *)mX.allocate(OBJ_SIZE);
                            const int recordPoolNum = recPool(p);

                            LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                            mp.allocate(OBJ_SIZE);
                        }

                        LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                    multipoolAllocations == objectAllocations);

                        ri <<= 1;
                    }
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < TEST_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);
                }
            }
        }

        if (verbose) {
            cout << "'bdlma::MultipoolAllocator(numPools, gs, *mbpc, *ba)'"
                 << endl;
        }
        for (int mi = 0; mi < NUM_MDATA; ++mi) {

            bslma::TestAllocator mpta("multipool", veryVeryVerbose);
            bslma::TestAllocator   oa("object",    veryVeryVerbose);

            MPool mp(NUM_POOLS, CON, MDATA[mi], &mpta);

            Obj mX(NUM_POOLS, CON, MDATA[mi], &oa);

            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();
            bsls::Types::Int64 objectAllocations    =   oa.numAllocations();

            LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                         multipoolAllocations == objectAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE    = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    multipoolAllocations = mpta.numAllocations();
                    objectAllocations    =   oa.numAllocations();

                    for (int j = 0; j < MDATA[mi][calcPoolNum]; ++j) {
                        char *p = (char *)mX.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        mp.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(multipoolAllocations, objectAllocations,
                                 multipoolAllocations == objectAllocations);
                }
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose)
                cout << "\t'MultipoolAllocator(numPools, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1));

                ASSERT_SAFE_FAIL_RAW(Obj( 0));
                ASSERT_SAFE_FAIL_RAW(Obj(-1));
            }

            if (veryVerbose)
                cout << "\t'MultipoolAllocator(numPools, gs, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1, CON));

                ASSERT_SAFE_FAIL_RAW(Obj( 0, CON));
                ASSERT_SAFE_FAIL_RAW(Obj(-1, CON));
            }

            if (veryVerbose)
                cout << "\t'MultipoolAllocator(numPools, gs, mbpc, *ba)'"
                     << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1, CON,  1));

                ASSERT_SAFE_FAIL_RAW(Obj( 0, CON,  1));
                ASSERT_SAFE_FAIL_RAW(Obj(-1, CON,  1));

                ASSERT_SAFE_FAIL_RAW(Obj( 1, CON,  0));
                ASSERT_SAFE_FAIL_RAW(Obj( 1, CON, -1));
            }

            if (veryVerbose)
                cout << "\t'MultipoolAllocator(numPools, *gs, *ba)'" << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1, SDATA[0]));

                ASSERT_SAFE_FAIL_RAW(Obj( 0, SDATA[0]));
                ASSERT_SAFE_FAIL_RAW(Obj(-1, SDATA[0]));

                ASSERT_SAFE_FAIL_RAW(Obj( 1, (Strategy *)0));
            }

            if (veryVerbose)
                cout << "\t'MultipoolAllocator(numPools, *gs, mbpc, *ba)'"
                     << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1, SDATA[0],       1));

                ASSERT_SAFE_FAIL_RAW(Obj( 0, SDATA[0],       1));
                ASSERT_SAFE_FAIL_RAW(Obj(-1, SDATA[0],       1));

                ASSERT_SAFE_FAIL_RAW(Obj( 1, (Strategy *)0,  1));

                ASSERT_SAFE_FAIL_RAW(Obj( 1, SDATA[0],       0));
                ASSERT_SAFE_FAIL_RAW(Obj( 1, SDATA[0],      -1));
            }

            if (veryVerbose)
                cout << "\t'MultipoolAllocator(numPools, gs, *mbpc, *ba)'"
                     << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1, CON, MDATA[0]));

                ASSERT_SAFE_FAIL_RAW(Obj( 0, CON, MDATA[0]));
                ASSERT_SAFE_FAIL_RAW(Obj(-1, CON, MDATA[0]));

                ASSERT_SAFE_FAIL_RAW(Obj( 1, CON, (int *)0));
            }

            if (veryVerbose)
                cout << "\t'MultipoolAllocator(numPools, *gs, *mbpc, *ba)'"
                     << endl;
            {
                ASSERT_SAFE_PASS_RAW(Obj( 1, SDATA[0],      MDATA[0]));

                ASSERT_SAFE_FAIL_RAW(Obj( 0, SDATA[0],      MDATA[0]));
                ASSERT_SAFE_FAIL_RAW(Obj(-1, SDATA[0],      MDATA[0]));

                ASSERT_SAFE_FAIL_RAW(Obj( 1, (Strategy *)0, MDATA[0]));

                ASSERT_SAFE_FAIL_RAW(Obj( 1, SDATA[0],      (int *)0));
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'NUMPOOLS' CTOR AND DTOR
        //
        // Concerns:
        //   We have the following concerns:
        //    1) The 'bdlma::MultipoolAllocator' constructor works properly:
        //       a. The constructor is exception neutral w.r.t. memory
        //          allocation.
        //       b. The internal memory management system is hooked up properly
        //          so that *all* internally allocated memory draws from a
        //          user-supplied allocator whenever one is specified.
        //       c. Objects may be allocated from the pools managed by the
        //          multipool allocator.
        //    2) The destructor works properly as implicitly tested in the
        //       various scopes of this test and in the presence of exceptions.
        //
        //   Note that this test case only tests one of the constructors.
        //
        // Plan:
        //   Create a test object using the constructor: 1) without exceptions
        //   and 2) in the presence of exceptions during memory allocations
        //   using a 'bslma::TestAllocator' and varying its *allocation*
        //   *limit*.  When the object goes out of scope, verify that the
        //   destructor properly deallocates all memory that had been allocated
        //   to it.
        //
        //   Concern 2 is addressed by making use of the 'allocate' method
        //   (which is thoroughly tested elsewhere).
        //
        //   At the end, include a constructor test specifying a static buffer
        //   allocator.  This tests for rudimentary correct object behavior
        //   via the destructor and Purify.
        //
        // Testing:
        //   MultipoolAllocator(numPools, Allocator *ba = 0);
        //   ~MultipoolAllocator();
        //   void *allocate(size);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING NUMPOOLS CTOR AND DTOR"
                          << endl << "=============================="
                          << endl;

        // number of pools to manage
        const int PDATA[]   = { 1, 2, 3, 4, 5 };
        const int NUM_PDATA = sizeof PDATA / sizeof *PDATA;

        // sizes of objects to allocate
        const int ODATA[]   = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
        const int NUM_ODATA = sizeof ODATA / sizeof *ODATA;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            for (int i = 0; i < NUM_PDATA; ++i) {
                const int NUM_POOLS = PDATA[i];
                if (veryVerbose) { P(NUM_POOLS); }
                for (int j = 0; j < NUM_ODATA; ++j) {
                    Obj mX(NUM_POOLS);
                    const int OBJ_SIZE = ODATA[j];
                    if (veryVerbose) { T_; P(OBJ_SIZE); }
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    LOOP2_ASSERT(i, j, p);
                    const int pCalculatedPool = calcPool(NUM_POOLS, OBJ_SIZE);
                    const int pRecordedPool   = recPool(p);
                    if (veryVerbose) {
                        T_; P_((void *)p);
                        P_(pCalculatedPool); P(pRecordedPool);
                    }
                    LOOP2_ASSERT(i, j, pCalculatedPool == pRecordedPool);
                }

                {
                    Obj mX(NUM_POOLS);
                    char *p = (char *) mX.allocate(2048);  // "overflow" block
                                                           // list
                    LOOP_ASSERT(i, p);
                    const int pRecordedPool = recPool(p);
                    if (veryVerbose) { T_; P_((void *)p); P(pRecordedPool); }
                    LOOP_ASSERT(i, -1 == pRecordedPool);
                }
            }
        }

        if (verbose) cout << "\tPassing in an allocator." << endl;
        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            for (int i = 0; i < NUM_PDATA; ++i) {
                const int NUM_POOLS = PDATA[i];
                if (veryVerbose) { P(NUM_POOLS); }
                const bsls::Types::Int64 NUM_BLOCKS =
                                                testAllocator.numBlocksInUse();
                const bsls::Types::Int64 NUM_BYTES  =
                                                 testAllocator.numBytesInUse();
                for (int j = 0; j < NUM_ODATA; ++j) {
                    {
                        Obj mX(NUM_POOLS, Z);
                        const int OBJ_SIZE = ODATA[j];
                        if (veryVerbose) { T_; P(OBJ_SIZE); }
                        char *p = (char *) mX.allocate(OBJ_SIZE);
                        LOOP2_ASSERT(i, j, p);
                        const int pCalculatedPool =
                                                 calcPool(NUM_POOLS, OBJ_SIZE);
                        const int pRecordedPool   = recPool(p);
                        if (veryVerbose) {
                            T_; P_((void *)p);
                            P_(pCalculatedPool); P(pRecordedPool);
                        }
                        LOOP2_ASSERT(i, j, pCalculatedPool == pRecordedPool);
                    }
                    LOOP2_ASSERT(i, j, NUM_BLOCKS ==
                                               testAllocator.numBlocksInUse());
                    LOOP2_ASSERT(i, j, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                }

                {
                    Obj mX(NUM_POOLS, Z);
                    char *p = (char *) mX.allocate(2048);  // "overflow" block
                                                           // list
                    LOOP_ASSERT(i, p);
                    const int pRecordedPool = recPool(p);
                    if (veryVerbose) { T_; P_((void *)p); P(pRecordedPool); }
                    LOOP_ASSERT(i, -1 == pRecordedPool);
                }
                LOOP_ASSERT(i, NUM_BLOCKS == testAllocator.numBlocksInUse());
                LOOP_ASSERT(i, NUM_BYTES  == testAllocator.numBytesInUse());
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            for (int i = 0; i < NUM_PDATA; ++i) {
                const int NUM_POOLS = PDATA[i];
                if (veryVerbose) { P(NUM_POOLS); }
                const bsls::Types::Int64 NUM_BLOCKS =
                                                testAllocator.numBlocksInUse();
                const bsls::Types::Int64 NUM_BYTES  =
                                                 testAllocator.numBytesInUse();
                for (int j = 0; j < NUM_ODATA; ++j) {
                  BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mX(NUM_POOLS, Z);
                    const int OBJ_SIZE = ODATA[j];
                    if (veryVerbose) { T_; P(OBJ_SIZE); }
                    char *p = (char *) mX.allocate(OBJ_SIZE);
                    LOOP2_ASSERT(i, j, p);
                    const int pCalculatedPool = calcPool(NUM_POOLS, OBJ_SIZE);
                    const int pRecordedPool   = recPool(p);
                    if (veryVerbose) {
                        T_; P_((void *)p);
                        P_(pCalculatedPool); P(pRecordedPool);
                    }
                    LOOP2_ASSERT(i, j, pCalculatedPool == pRecordedPool);
                  } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                  LOOP2_ASSERT(i, j, NUM_BLOCKS ==
                                               testAllocator.numBlocksInUse());
                  LOOP2_ASSERT(i, j, NUM_BYTES  ==
                                                testAllocator.numBytesInUse());
                }

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                  Obj mX(NUM_POOLS, Z);
                  char *p = (char *) mX.allocate(2048);  // "overflow" block
                                                         // list
                  LOOP_ASSERT(i, p);
                  const int pRecordedPool = recPool(p);
                  if (veryVerbose) { T_; P_((void *)p); P(pRecordedPool); }
                  LOOP_ASSERT(i, -1 == pRecordedPool);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
                LOOP_ASSERT(i, NUM_BLOCKS == testAllocator.numBlocksInUse());
                LOOP_ASSERT(i, NUM_BYTES  == testAllocator.numBytesInUse());
            }
        }

        if (verbose)
            cout << "\tWith a buffer allocator (exercise only)." << endl;
        {
            char memory[4096];
            bdlma::BufferedSequentialAllocator a(memory, sizeof memory);
            Obj *doNotDelete = new(a.allocate(sizeof(Obj))) Obj(1, &a);
            char *p = (char *) doNotDelete->allocate(2048);  // "overflow"
                                                             // block list
            ASSERT(p);
            // No destructor is called; will produce memory leak in purify if
            // internal allocators are not hooked up properly.
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //   That the basic functionality of 'bdlma::MultipoolAllocator' works
        //   properly.
        //
        // Plan:
        //   Create a multipool allocator that manages three pools.  Allocate
        //   memory from the first two pools, as well as from the "overflow"
        //   block list.  Then 'deallocate' or 'release' the allocated blocks.
        //   Finally, let the multipool allocator go out of scope to exercise
        //   the destructor.
        //
        // Testing:
        //   This "test" exercises basic functionality, but tests nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST"
                          << endl << "==============" << endl;

        {
            char *p, *q, *r;

            // 1.
            if (verbose)
                cout << "1. Create a multipool allocator that manages three "
                        "pools."
                     << endl;

            Obj mX(3);

            // 2.
            if (verbose)
                cout << "2. Allocate an object from the small object pool."
                     << endl;

            p = (char *)mX.allocate(8);                   ASSERT(p);

            // 3.
            if (verbose) cout << "3. Deallocate the small object." << endl;

            mX.deallocate(p);

            // Address 'p' is no longer valid.

            // 4.
            if (verbose)
                cout << "4. Reserve capacity in the medium object pool for "
                        "at least two objects."
                     << endl;

            mX.reserveCapacity(8 * 2, 2);

            // 5.
            if (verbose)
                cout << "5. Allocate two objects from the medium object pool."
                     << endl;

            p = (char *)mX.allocate(8 * 2);               ASSERT(p);
            q = (char *)mX.allocate(8 * 2 - 1);           ASSERT(q);

            // 6.
            if (verbose)
                cout << "6. Allocate an object from the \"overflow\" block "
                        "list."
                     << endl;

            r = (char *)mX.allocate(1024);                ASSERT(r);

            // 7.
            if (verbose)
                cout << "7. Deallocate all outstanding objects." << endl;

            mX.release();

            // Addresses 'p', 'q', 'r' are no longer valid.

            // 8.
            if (verbose)
                cout << "8. Let the multipool allocator go out of scope."
                     << endl;
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // --------------------------------------------------------------------

// Finally, in main, we run the test with the different allocators and
// different allocator configurations based on command line arguments:
//..
    {
        int testLengthFactor = 5;
        const int NUM_POOLS  = 10;

        if (argc > 2) {
            testLengthFactor = bsl::atoi(argv[2]);
        }

        char growth = 'g';
        if (argc > 3) {
            growth = argv[3][0];
            if (growth != 'g' && growth != 'c') {
                printf("[g]eometric or [c]onstant growth must be used\n");
                return -1;                                            // RETURN
            }
        }

        int maxChunkSize = 32;
        if (argc > 4) {
            maxChunkSize = bsl::atoi(argv[4]);
            if (maxChunkSize <= 0) {
                printf("maxChunkSize must be >= 1");
            }
        }

        bsls::BlockGrowth::Strategy strategy = growth == 'g'
                                            ? bsls::BlockGrowth::BSLS_GEOMETRIC
                                            : bsls::BlockGrowth::BSLS_CONSTANT;

        printf("\nNew Delete Allocator:\n\n");
        {
            bslma::Allocator *nda = bslma::NewDeleteAllocator::allocator(0);
            my_TestUtil::test(testLengthFactor, nda);
        }

        printf("\nMultipool Allocator with [%c], [%d]:\n\n", growth,
                                                                 maxChunkSize);
        {
            bdlma::MultipoolAllocator ma(NUM_POOLS, strategy, maxChunkSize);
            my_TestUtil::test(testLengthFactor, &ma);
        }

        printf("\nMultipool Allocator Managed with [%c], [%d]:\n\n", growth,
                                                                 maxChunkSize);
        {
            bdlma::MultipoolAllocator ma(NUM_POOLS, strategy, maxChunkSize);
            my_TestUtil::testManaged(testLengthFactor, &ma);
        }
    }
//..

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
