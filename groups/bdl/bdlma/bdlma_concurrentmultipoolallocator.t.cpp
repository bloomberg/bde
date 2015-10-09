// bdlma_concurrentmultipoolallocator.t.cpp                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_concurrentmultipoolallocator.h>

#include <bdlma_bufferedsequentialallocator.h>  // for testing only

#include <bslim_testutil.h>

#include <bslma_default.h>                      // for purpose only
#include <bslma_testallocator.h>                // for purpose only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bslma_usesbslmaallocator.h>
#include <bslmf_nestedtraitdeclaration.h>
#include <bsls_alignmentutil.h>
#include <bsls_stopwatch.h>
#include <bsls_types.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'memcpy'
#include <bsl_iostream.h>
#include <bsl_list.h>
#include <bsl_map.h>
#include <bsl_set.h>
#include <bsl_string.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                                  TEST PLAN
//-----------------------------------------------------------------------------
//                                  Overview
//                                  --------
// The goals of this test suite are to verify that 1)
// 'bdlma::ConcurrentMultipoolAllocator' correctly proxies memory requests
// (except deallocation) to its 'bdlma::ConcurrentMultipool' member; and 2)
// 'deallocate' method does *not* deallocate any memory.
//
// To achieve goal 1, create a string allocator and a string pool, and supply
// each with its own instance of test allocator.  Request memory of varying
// sizes from both the string allocator and the string pool.  Verify that the
// test allocators contains the same number of bytes in use and the same total
// number of bytes requested.
//
// To achieve goal 2, create a string allocator supplied with a test allocator.
// Request memory of varying sizes and then deallocate each memory.  Verify
// that the number of bytes in use indicated by the test allocator does not
// decrease after each 'deallocate' method invocation.
//-----------------------------------------------------------------------------
// [1] bdlma::ConcurrentMultipoolAllocator(numPools, basicAllocator);
// [1] ~bdlma::ConcurrentMultipoolAllocator();
// [2] bdlma::ConcurrentMultipoolAllocator(numPools);
// [2] bdlma::ConcurrentMultipoolAllocator(numPools, Z);
// [3] bdlma::ConcurrentMultipoolAllocator(numPools, minSize);
// [3] bdlma::ConcurrentMultipoolAllocator(numPools, poolNumObjects);
// [3] bdlma::ConcurrentMultipoolAllocator(numPools, minSize, poolNumObjects);
// [3] bdlma::ConcurrentMultipoolAllocator(numPools, minSize, Z);
// [3] bdlma::ConcurrentMultipoolAllocator(numPools, poolNumObjects, Z);
// [3] bdlma::ConcurrentMultipoolAllocator(numPools, minSz, poolNumObjects, Z);
// [1] void *allocate(numBytes);
// [2] void deallocate(address);
// [1] void release();
// [3] void reserveCapacity(numBytes);
//-----------------------------------------------------------------------------
// [7] USAGE EXAMPLE

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

//=============================================================================
//                       GLOBAL TYPES AND CONSTANTS
//-----------------------------------------------------------------------------

typedef bdlma::ConcurrentMultipoolAllocator Obj;

const int MAX_ALIGN = bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT;

// Warning: keep this in sync with bdema_Multipool.h!
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

static int calcPool(int numPools, int objSize)
    // Calculate the index of the pool that should allocate objects that are of
    // the specified 'objSize' (in bytes) from a multipool allocator managing
    // the specified 'numPools' number of memory pools.
{
    ASSERT(0 < numPools);
    ASSERT(0 < objSize);

    int poolIndex        = 0;
    int pooledObjectSize = 8;

    while (objSize > pooledObjectSize) {
        pooledObjectSize *= 2;
        ++poolIndex;
    }

    if (poolIndex >= numPools) {
        poolIndex = -1;
    }

    return poolIndex;
}

inline static int recPool(char *address)
    // Return the index of the pool that allocated the memory at the specified
    // 'address'.
{
    ASSERT(address);

    Header *h = (Header *)address - 1;

    return h->d_header.d_pool;
}

inline int delta(char *address1, char *address2)
    // Return the number of bytes between the specified 'address1' and the
    // specified 'address2'.
{
    return static_cast<int>(address1 < address2
                          ? address2 - address1
                          : address1 - address2);
}

inline static void scribble(char *address, int size)
    // Assign a non-zero value to each of the specified 'size' bytes starting
    // at the specified 'address'.
{
    memset(address, 0xff, size);
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
///Example 1: Using a 'bdlma::ConcurrentMultipoolAllocator'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::ConcurrentMultipoolAllocator' can be used to supply memory to
// node-based data structures such as 'bsl::set', 'bsl::list' or 'bsl::map'.
// Suppose we are implementing a container of named graphs data structure,
// where a graph is defined by a set of edges and nodes.  The various
// fixed-sized nodes can be efficiently allocated by a
// 'bdlma::ConcurrentMultipoolAllocator'.
//
// First, the edge class, 'my_Edge', is defined as follows:
//..
    class my_Node;

    class my_Edge {
        // This class represents an edge within a graph.  Both ends of the
        // edge must be connected to nodes.

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
    : d_first(first), d_second(second)
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
// Then we define the graph class, 'my_Graph', as follows:
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
// Then finally, the container for the collection of named graphs,
// 'my_NamedGraphContainer', is defined as follows:
//..
    class my_NamedGraphContainer {
        // This class stores a map that index graph names to graph objects.

        // DATA
        bsl::map<bsl::string, my_Graph> d_graphMap;  // map from graph names to
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
//
///Example 2: Performance of a 'bdlma::ConcurrentMultipoolAllocator'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::ConcurrentMultipoolAllocator' can greatly improve efficiency when
// it is used to supply memory to multiple node-based data structure, that
// frequently both insert and remove nodes, while grows to significant size
// before being destroyed.  The following experiment will illustrate the
// benefits of using the 'bdlma::ConcurrentMultipoolAllocator' under the these
// scenario by comparing the following 3 different allocator uses:
//
//: 1 Using the 'bslma::NewDeleteAllocator directly.
//:
//: 2 Using a 'bdlma::ConcurrentMultipoolAllocator as a direct substitute of
//:   the 'bslma::NewDeleteAllocator'.
//:
//: 3 Exploiting the managed aspect of 'bslma_MultipoolAllocator' by avoiding
//:   invocation of the destructor of the data structure; whereas, the
//:   destruction of the allocator will automatically reclaim all memory.
//
// First, we create a test data structure that contain three 'bsl::list'.  Each
// list holds a different type of object, where each type has a different size.
// For simplicity, we create these different object types as different
// instantiations of a template class, parameterized on the object size:
//..
    template <int OBJECT_SIZE>
    class my_TestObject {

        // DATA
        char d_data[OBJECT_SIZE];

    };
//..
// Again, for simplicity, the size of these objects are chosen to be 20, 40,
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
// are rarely those recently added (this also remove the possibility of noise
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
// The 'push' method will push into the 3 'bsl::list' managed by
// 'my_TestDataStructure' sequentially.  Similarly, the 'pop' method will pop
// from the lists sequentially:
//..
    // MANIPULATORS
    void my_TestDataStructure::push()
    {
            // Push to the back of the list.

            d_list1.push_back(Obj1());
            d_list2.push_back(Obj2());
            d_list3.push_back(Obj3());
    }

    void my_TestDataStructure::pop()
    {
            // Pop from the back of the list.

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
// structure will grow by invoking 'push' twice and 'pop' once.  Note that, "n"
// measures the effect of insertion and removal of nodes, while "iterations"
// measures the effect of construction and destruction of entire lists of
// nodes.
//
// The 'test' method will also accept an 'bslma::Allocator *', which will be
// used as the allocator used to construct the test mechanism and its internal
// lists:
//..
    class my_TestUtil {

      public:
        // CLASS METHODS
        static void test(int               testLengthFactor,
                         bslma::Allocator *basicAllocator)
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
        static void testManaged(int                      testLengthFactor,
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
                    my_TestDataStructure *tm_p = new(*managedAllocator)
                                        my_TestDataStructure(managedAllocator);

                    // Testing cost of insertion and deletion.

                    for (int k = 0; k < n; ++k) {
                        tm_p->push();
                        tm_p->push();
                        tm_p->pop();
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
// different allocator configuration based on command line arguments:
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
//          bdlma::ConcurrentMultipoolAllocator ma(NUM_POOLS,
//                                                 strategy,
//                                                 maxChunkSize);
//          my_TestUtil::test(testLengthFactor, &ma);
//      }
//
//      printf("\nMultipool Allocator Managed with [%c], [%d]:\n\n", growth,
//                                                               maxChunkSize);
//      {
//          bdlma::ConcurrentMultipoolAllocator ma(NUM_POOLS,
//                                                 strategy,
//                                                 maxChunkSize);
//          my_TestUtil::testManaged(testLengthFactor, &ma);
//      }
//
//      return 0;
//  }
//..
// An excerpt of the results of the test running on IBM, under optimized mode,
// using default constructed 'bdlma::ConcurrentMultipoolAllocator' parameters,
// is shown below:
//..
//  New Delete Allocator:
//
//  6       1       1000000 3.219204
//  6       10      100000  2.402481
//  6       100     10000   2.397959
//  6       1000    1000    2.427938
//  6       10000   100     2.666011
//  6       100000  10      2.959929
//  6       1000000 1       4.669094
//
//  ConcurrentMultipool Allocator with [g], [32]:
//
//  6       1       1000000 0.837499
//  6       10      100000  0.431225
//  6       100     10000   0.380504
//  6       1000    1000    0.400867
//  6       10000   100     0.580136
//  6       100000  10      1.005599
//  6       1000000 1       0.943738
//
//  ConcurrentMultipool Allocator Managed with [g], [32]:
//
//  6       1       1000000 3.974020
//  6       10      100000  0.932585
//  6       100     10000   0.446898
//  6       1000    1000    0.380909
//  6       10000   100     0.394665
//  6       100000  10      0.451662
//  6       1000000 1       0.568951
//..
// It is clear that using a 'bdlma::MultipoolAllocator' results in an
// improvement in memory allocation by a factor of about 4.  Furthermore, if
// the managed aspect of the multipool allocator is exploited, the cost of
// destruction rapidly decreases in relative terms as the list ('n') grows
// larger.

//=============================================================================
//                            OLD USAGE EXAMPLE
//-----------------------------------------------------------------------------
class my_DoubleStack {
    double *d_stack_p; // dynamically allocated array (d_size elements)
    int d_size;        // physical capacity of this stack (in elements)
    int d_length;      // logical index of next available stack element
    bslma::Allocator *d_allocator_p; // holds (but does not own) object

    friend class my_DoubleStackIter;

  private:
    void increaseSize(); // Increase the capacity by at least one element.

  public:
    // CREATORS
    my_DoubleStack(bslma::Allocator *basicAllocator = 0);
    my_DoubleStack(const my_DoubleStack&  other,
                   bslma::Allocator      *basicAllocator = 0);
    ~my_DoubleStack();

    // MANIPULATORS
    my_DoubleStack& operator=(const my_DoubleStack& rhs);
    void push(double value);
    void pop();

    // ACCESSORS
    const double& top() const;
    int isEmpty() const;
};

enum { k_INITIAL_SIZE = 1, k_GROW_FACTOR = 2 };

// ...

my_DoubleStack::my_DoubleStack(bslma::Allocator *basicAllocator)
: d_size(k_INITIAL_SIZE)
, d_length(0)
, d_allocator_p(basicAllocator)
{
    ASSERT(d_allocator_p);
    d_stack_p = (double *) d_allocator_p->allocate(d_size * sizeof *d_stack_p);
}

my_DoubleStack::~my_DoubleStack()
{
    // CLASS INVARIANTS
    ASSERT(d_allocator_p);
    ASSERT(d_stack_p);
    ASSERT(0 <= d_length);
    ASSERT(0 <= d_size);
    ASSERT(d_length <= d_size);

    d_allocator_p->deallocate(d_stack_p);
}

inline
void my_DoubleStack::push(double value)
{
    if (d_length >= d_size) {
        increaseSize();
    }
    d_stack_p[d_length++] = value;
}

static
void reallocate(double           **array,
                int                newSize,
                int                length,
                bslma::Allocator  *basicAllocator)
    // Reallocate memory in the specified 'array' to the specified 'newSize'
    // using the specified 'basicAllocator'.  The specified 'length' number of
    // leading elements are preserved.  Since the
    //  class invariant requires that the physical capacity of the
    // container may grow but never shrink; the behavior is undefined unless
    // 'length <= newSize'.
{
    ASSERT(array);
    ASSERT(1 <= newSize);
    ASSERT(0 <= length);
    ASSERT(basicAllocator);
    ASSERT(length <= newSize);        // enforce class invariant

    double *tmp = *array;             // support exception neutrality
    *array = (double *) basicAllocator->allocate(newSize * sizeof **array);

    // COMMIT POINT

    memcpy(*array, tmp, length * sizeof **array);
    basicAllocator->deallocate(tmp);
}

void my_DoubleStack::increaseSize()
{
     int proposedNewSize = d_size * k_GROW_FACTOR;    // reallocate can throw
     ASSERT(proposedNewSize > d_length);
     reallocate(&d_stack_p, proposedNewSize, d_length, d_allocator_p);
     d_size = proposedNewSize;                        // we're committed
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//           Additional Functionality Need to Complete Usage Test Case

class my_DoubleStackIter {
    const double *d_stack_p;
    int           d_index;

  private:
    // Not implemented:
    my_DoubleStackIter(const my_DoubleStackIter&);
    my_DoubleStackIter& operator=(const my_DoubleStackIter&);

  public:
    my_DoubleStackIter(const my_DoubleStack&  stack,
                       bslma::Allocator      *basicAllocator = 0)
    : d_stack_p(stack.d_stack_p)
    , d_index(stack.d_length - 1)
    {
        (void)basicAllocator;
    }

    void operator++() { --d_index; }

    operator const void *() const { return d_index >= 0 ? this : 0; }

    const double& operator()() const { return d_stack_p[d_index]; }
};

ostream& operator<<(ostream& stream, const my_DoubleStack& stack)
{
    stream << "(top) [";
    for (my_DoubleStackIter it(stack); it; ++it) {
        stream << ' ' << it();
    }
    return stream << " ] (bottom)" << flush;
}

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

    bslma::TestAllocator  testAllocator(veryVeryVerbose);
    bslma::Allocator     *Z = &testAllocator;

    switch (test) { case 0:
      case 7: {
// Finally, in 'main', we can create a 'bdlma::ConcurrentMultipoolAllocator'
// and pass it to our 'my_NamedGraphContainer'.  Since we know that the maximum
// block size needed is 32 (comes from 'sizeof(my_Graph)'), we can calculate
// the number of pools needed by using the formula specified in the
// "configuration at construction" section:
//..
//  largestPoolSize < 2 ^ (N + 2).
//..
// When solved for the above equation, the smallest 'N' that satisfies this
// relationship is 3:
//..
    enum { k_NUM_POOLS = 3 };

    bdlma::ConcurrentMultipoolAllocator basicAllocator(k_NUM_POOLS);

    my_NamedGraphContainer container(&basicAllocator);
//..

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING OLD USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   OLD USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Old Usage Example"
                          << endl << "=========================" << endl;

        {
            const int MIN_SIZE = 12;

            struct Small {
                char x[MIN_SIZE];
            };

            struct Medium {
                char x[MIN_SIZE * 2];
            };

            struct Large {  // sizeof(Large) == MIN_SIZE * 4
                Small  s1, s2;
                Medium m;
            };

            const int NUM_POOLS = 4;
            bdlma::ConcurrentMultipoolAllocator mp(NUM_POOLS);

            const int NUM_OBJS = 8;
            int objSize        = MIN_SIZE;
            for (int i = 0; i < NUM_POOLS - 1; ++i) {
                mp.reserveCapacity(objSize, NUM_OBJS);
                objSize *= 2;
            }

            Small  *pS = new(mp.allocate(sizeof(Small)))  Small;    ASSERT(pS);
            Medium *pM = new(mp.allocate(sizeof(Medium))) Medium;   ASSERT(pM);
            Large  *pL = new(mp.allocate(sizeof(Large)))  Large;    ASSERT(pL);

            char *pBuf = (char *) mp.allocate(1024);              ASSERT(pBuf);

            mp.deallocate(pS);
            mp.deallocate(pL);
            // 'pS' and 'pL' are no longer valid addresses.

            mp.release();
            // Now 'pM' and 'pBuf' are also invalid addresses.
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // RESERVECAPACITY TEST
        //   Create a string allocator and a string pool, and initialize each
        //   with its own instance of test allocator.  Reserve capacity and
        //   request memory of varying sizes from both the string allocator and
        //   the string pool.  Verify that both test allocators contain the
        //   same number of bytes in use and the same total number of bytes
        //   requested.
        //
        // Testing:
        //   void reserveCapacity(numBytes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "RESERVECAPACITY TEST" << endl
                                  << "====================" << endl;
        /* TBD
        const int DATA[] = { 0, 5, 12, 24, 32, 64, 256, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const int RES_DATA[] = { 0, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
        const int NUM_RES_DATA = sizeof RES_DATA / sizeof *RES_DATA;

        bslma::TestAllocator multiPoolAllocatorTA(veryVeryVerbose);
        bslma::TestAllocator multiPoolTA(veryVeryVerbose);

        for (int j = 0; j < NUM_RES_DATA; ++j) {
            bdlma::ConcurrentMultipoolAllocator sa(&multiPoolAllocatorTA);
            bdlma::ConcurrentMultipool sp(&multiPoolTA);

            sa.reserveCapacity(RES_DATA[j]);
            sp.reserveCapacity(RES_DATA[j]);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int SIZE = DATA[i];
                void *p = sa.allocate(SIZE);
                void *q = sp.allocate(SIZE);
                LOOP_ASSERT(i, multiPoolAllocatorTA.numBytesInUse()
                                               == multiPoolTA.numBytesInUse());
            }

            sa.release();
            sp.release();
            ASSERT(0 == multiPoolAllocatorTA.numBytesInUse());
            ASSERT(0 == multiPoolTA.numBytesInUse());
            ASSERT(multiPoolAllocatorTA.numBytesTotal()
                                               == multiPoolTA.numBytesTotal());
        }
        */
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // DEALLOCATE TEST
        //   Create a multiPooling allocator initialized with a test allocator.
        //   Request memory of varying sizes and then deallocate each memory.
        //   Verify that the number of bytes in use indicated by the test
        //   allocator does not decrease after each 'deallocate' method
        //   invocation.
        //
        // Testing:
        //   void deallocate(address);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "DEALLOCATE TEST" << endl
                                  << "===============" << endl;

        if (verbose) cout << "Testing 'deallocate'." << endl;

        const int DATA[] = { 0, 5, 12, 24, 32, 64, 256, 1000 };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        bslma::TestAllocator ta(veryVeryVerbose);
        bdlma::ConcurrentMultipoolAllocator sa(16, &ta);  // TBD

        bsls::Types::Int64 lastNumBytesInUse = ta.numBytesInUse();

        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            void *p = sa.allocate(SIZE);
            const bsls::Types::Int64 numBytesInUse = ta.numBytesInUse();
            sa.deallocate(p);
            LOOP_ASSERT(i, numBytesInUse == ta.numBytesInUse());
            LOOP_ASSERT(i, lastNumBytesInUse <= ta.numBytesInUse());
            lastNumBytesInUse = ta.numBytesInUse();
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING ALL C'TORS
        //
        // Concerns:
        //   1. That all arguments passed to the multipool allocator
        //      constructors has their proper effect.
        //
        //   2. That default growth strategy is geometric growth.
        //
        //   3. That default max blocks per chunk is 32.
        //
        // Plan:
        //   Compare allocation between a multipool and a multipool allocator.
        //
        // Testing:
        //   bdlma::ConcurrentMultipoolAllocator(bslma::Allocator *ba = 0);
        //   bdlma::ConcurrentMultipoolAllocator(int numPools, ba = 0);
        //   bdlma::ConcurrentMultipoolAllocator(Strategy gs, ba = 0);
        //   bdlma::ConcurrentMultipoolAllocator(int n,Strategy gs, ba=0);
        //   bdlma::ConcurrentMultipoolAllocator(int n,const Strat *gsa, ba=0);
        //   bdlma::ConcurrentMultipoolAllocator(int n,int mbpc, ba=0);
        //   bdlma::ConcurrentMultipoolAllocator(int n,const int *mbpc, ba=0);
        //   bdlma::ConcurrentMultipoolAllocator(int n,Strat gs,int mbpc,ba=0);
        //   bdlma::ConcurrentMultipoolAllocator(int n,gsa,int mbpc, ba=0);
        //   bdlma::ConcurrentMultipoolAllocator(int n,gs,const int *ma, ba=0);
        //   bdlma::ConcurrentMultipoolAllocator(int n,gsa,const int *ma,ba=0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'constructor' and 'destructor'"
                          << endl << "======================================"
                          << endl;

        enum { k_INITIAL_CHUNK_SIZE = 1,
               k_DEFAULT_MAX_CHUNK_SIZE = 32,
               k_DEFAULT_NUM_POOLS = 10 };

        // For pool allocation.
        char buffer[1024];
        bdlma::BufferedSequentialAllocator bsa(buffer, 1024);

        const int NUM_POOLS = 5;
        const int TEST_MAX_CHUNK_SIZE = 30;

        // Object Data
        const int ODATA[]   = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512 };
        const int NUM_ODATA = sizeof ODATA / sizeof *ODATA;

        typedef bsls::BlockGrowth::Strategy St;
        const bsls::BlockGrowth::Strategy GEO =
                                             bsls::BlockGrowth::BSLS_GEOMETRIC;
        const bsls::BlockGrowth::Strategy CON =
                                             bsls::BlockGrowth::BSLS_CONSTANT;

        // Strategy Data
        const St SDATA[][5]  = { {GEO, GEO, GEO, GEO, GEO},
                                 {CON, CON, CON, CON, CON},
                                 {GEO, CON, GEO, CON, GEO},
                                 {CON, GEO, CON, GEO, CON} };
        const int NUM_SDATA = sizeof SDATA / sizeof *SDATA;

        // MaxBlocksPerChunk Data
        const int MDATA[][5] = { { 32, 32, 32, 32, 32 },
                                 { 30, 30, 30, 30, 30 },
                                 { 30, 32, 30, 32, 30 },
                                 { 32, 30, 32, 30, 32 } };
        const int NUM_MDATA = sizeof MDATA / sizeof *MDATA;

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, const S *gsa, "
                    "const int *ma, A *ba = 0)" << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            for (int mi = 0; mi < NUM_MDATA; ++mi) {

                bslma::TestAllocator pta("pool test allocator",
                                         veryVeryVerbose);
                bslma::TestAllocator mpta("multipool test allocator",
                                          veryVeryVerbose);

                // Create the multipool allocator
                bdlma::ConcurrentMultipool pool(NUM_POOLS,
                                                SDATA[si],
                                                MDATA[mi],
                                                &pta);
                Obj mp(NUM_POOLS, SDATA[si], MDATA[mi], &mpta);

                // Allocate until we depleted the pool
                bsls::Types::Int64 poolAllocations      = pta.numAllocations();
                bsls::Types::Int64 multipoolAllocations =
                                                         mpta.numAllocations();

                // multipool should have an extra allocation for the array of
                // pools.
                LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                             poolAllocations == multipoolAllocations);

                for (int oi = 0; oi < NUM_ODATA; ++oi) {
                    const int OBJ_SIZE = ODATA[oi];
                    const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                    if (-1 == calcPoolNum) {
                        char *p = (char *) mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                        continue;
                    }

                    LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                    // Testing geometric growth
                    if (GEO == SDATA[si][calcPoolNum]) {
                        int ri = k_INITIAL_CHUNK_SIZE;
                        while (ri < MDATA[mi][calcPoolNum]) {
                            poolAllocations      = pta.numAllocations();
                            multipoolAllocations = mpta.numAllocations();

                            for (int j = 0; j < ri; ++j) {
                                char *p = (char *)mp.allocate(OBJ_SIZE);
                                const int recordPoolNum = recPool(p);

                                LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                             recordPoolNum == calcPoolNum);

                                pool.allocate(OBJ_SIZE);
                            }

                            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                         poolAllocations
                                                      == multipoolAllocations);

                            ri <<= 1;
                        }
                    }

                    // Testing constant growth (also applies to capped
                    // geometric growth).
                    const int NUM_REPLENISH = 3;
                    for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                        poolAllocations      = pta.numAllocations();
                        multipoolAllocations = mpta.numAllocations();

                        for (int j = 0; j < MDATA[mi][calcPoolNum]; ++j) {
                            char *p = (char *)mp.allocate(OBJ_SIZE);
                            const int recordPoolNum = recPool(p);

                            LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                             recordPoolNum == calcPoolNum);

                            pool.allocate(OBJ_SIZE);
                        }

                        LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                     poolAllocations == multipoolAllocations);
                    }
                }
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(bslma::Allocator *ba = 0)"
                 << endl;
        }

        {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            bdlma::ConcurrentMultipool pool(&pta);
            Obj mp(&mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(k_DEFAULT_NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < k_DEFAULT_NUM_POOLS);

                // Testing geometric growth
                int ri = k_INITIAL_CHUNK_SIZE;
                while (ri < k_DEFAULT_MAX_CHUNK_SIZE) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < ri; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations
                                              == multipoolAllocations);

                    ri <<= 1;
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations == multipoolAllocations);
                }
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int numPools,"
                 << " bslma::Allocator *ba = 0)"
                 << endl;
        }
        {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            // Create the multipool
            bdlma::ConcurrentMultipool pool(NUM_POOLS, &pta);
            Obj mp(NUM_POOLS, &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth
                int ri = k_INITIAL_CHUNK_SIZE;
                while (ri < k_DEFAULT_MAX_CHUNK_SIZE) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < ri; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                     recordPoolNum == calcPoolNum);

                        pool.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations
                                              == multipoolAllocations);

                    ri <<= 1;
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations == multipoolAllocations);
                }
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(Strategy gs,"
                 << " bslma::Allocator *ba = 0)"
                 << endl;
        }
        {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            // Create the multipool
            bdlma::ConcurrentMultipool pool(CON, &pta);
            Obj mp(CON, &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(k_DEFAULT_NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < k_DEFAULT_NUM_POOLS);

                // Testing constant growth
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations == multipoolAllocations);
                }
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, Strategy gs,"
                 << " bslma::Allocator"
                 << "*ba = 0)"
                 << endl;
        }
        {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            bdlma::ConcurrentMultipool pool(NUM_POOLS, CON, &pta);
            Obj mp(NUM_POOLS, CON, &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing constant growth
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations == multipoolAllocations);
                }
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, const Strategy *gsa,"
                 << " Allocator "
                 << "*ba = 0)"
                 << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            // Create the multipool
            bdlma::ConcurrentMultipool pool(NUM_POOLS, SDATA[si], &pta);
            Obj mp(NUM_POOLS, SDATA[si], &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth
                if (GEO == SDATA[si][calcPoolNum]) {
                    int ri = k_INITIAL_CHUNK_SIZE;
                    while (ri < k_DEFAULT_MAX_CHUNK_SIZE) {
                        poolAllocations      = pta.numAllocations();
                        multipoolAllocations = mpta.numAllocations();

                        for (int j = 0; j < ri; ++j) {
                            char *p = (char *)mp.allocate(OBJ_SIZE);
                            const int recordPoolNum = recPool(p);

                            LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                            pool.allocate(OBJ_SIZE);
                        }

                        LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                     poolAllocations == multipoolAllocations);

                        ri <<= 1;
                    }
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < k_DEFAULT_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations == multipoolAllocations);
                }
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, int mbpc,"
                 << " bslma::Allocator *ba= 0)"
                 << endl;
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, const int *mbpc,"
                 << " bslma::Allocator"
                 << "*ba = 0)"
                 << endl;
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, Strategy gs, "
                 << "int mbpc, Allocator "
                 << "*ba = 0)"
                 << endl;
        }
        {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            bdlma::ConcurrentMultipool pool(NUM_POOLS,
                                            CON,
                                            TEST_MAX_CHUNK_SIZE,
                                            &pta);
            Obj mp(NUM_POOLS, CON, TEST_MAX_CHUNK_SIZE, &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS,
                                                 OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing constant growth
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < TEST_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations == multipoolAllocations);
                }
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, const S *gsa,"
                 << " int mbpc, Alloc "
                 << "*ba = 0)"
                 << endl;
        }
        for (int si = 0; si < NUM_SDATA; ++si) {
            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            bdlma::ConcurrentMultipool pool(NUM_POOLS,
                                            SDATA[si],
                                            TEST_MAX_CHUNK_SIZE,
                                            &pta);
            Obj mp(NUM_POOLS, SDATA[si], TEST_MAX_CHUNK_SIZE, &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing geometric growth
                if (GEO == SDATA[si][calcPoolNum]) {
                    int ri = k_INITIAL_CHUNK_SIZE;
                    while (ri < TEST_MAX_CHUNK_SIZE) {
                        poolAllocations      = pta.numAllocations();
                        multipoolAllocations = mpta.numAllocations();

                        for (int j = 0; j < ri; ++j) {
                            char *p = (char *)mp.allocate(OBJ_SIZE);
                            const int recordPoolNum = recPool(p);

                            LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                            pool.allocate(OBJ_SIZE);
                        }

                        LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                     poolAllocations == multipoolAllocations);

                        ri <<= 1;
                    }
                }

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < TEST_MAX_CHUNK_SIZE; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations == multipoolAllocations);
                }
            }
        }

        if (verbose) {
            cout << "bdlma::ConcurrentMultipool(int n, Strat gs,"
                 << " const int *ma, Alloc "
                 << " *ba = 0)"
                 << endl;
        }
        for (int mi = 0; mi < NUM_MDATA; ++mi) {

            bslma::TestAllocator pta("pool test allocator",
                                     veryVeryVerbose);
            bslma::TestAllocator mpta("multipool test allocator",
                                      veryVeryVerbose);

            bdlma::ConcurrentMultipool pool(NUM_POOLS, CON, MDATA[mi], &pta);
            Obj mp(NUM_POOLS, CON, MDATA[mi], &mpta);

            // Allocate until we depleted the pool
            bsls::Types::Int64 poolAllocations      = pta.numAllocations();
            bsls::Types::Int64 multipoolAllocations = mpta.numAllocations();

            // multipool should have an extra allocation for the array of
            // pools.
            LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                         poolAllocations == multipoolAllocations);

            for (int oi = 0; oi < NUM_ODATA; ++oi) {
                const int OBJ_SIZE = ODATA[oi];
                const int calcPoolNum = calcPool(NUM_POOLS, OBJ_SIZE);

                if (-1 == calcPoolNum) {
                    char *p = (char *) mp.allocate(OBJ_SIZE);
                    const int recordPoolNum = recPool(p);

                    LOOP_ASSERT(recordPoolNum, -1 == recordPoolNum);
                    continue;
                }

                LOOP_ASSERT(calcPoolNum, calcPoolNum < NUM_POOLS);

                // Testing constant growth (also applies to capped geometric
                // growth).
                const int NUM_REPLENISH = 3;
                for (int ri = 0; ri < NUM_REPLENISH; ++ri) {
                    poolAllocations      = pta.numAllocations();
                    multipoolAllocations = mpta.numAllocations();

                    for (int j = 0; j < MDATA[mi][calcPoolNum]; ++j) {
                        char *p = (char *)mp.allocate(OBJ_SIZE);
                        const int recordPoolNum = recPool(p);

                        LOOP2_ASSERT(recordPoolNum, calcPoolNum,
                                         recordPoolNum == calcPoolNum);

                        pool.allocate(OBJ_SIZE);
                    }

                    LOOP2_ASSERT(poolAllocations, multipoolAllocations,
                                 poolAllocations == multipoolAllocations);
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING C'TOR AND D'TOR
        //
        // Concerns:
        //   We have the following concerns:
        //    1) The 'bdema::Multipool' constructor works properly:
        //       a. The constructor is exception neutral w.r.t. memory
        //          allocation.
        //       b. The internal memory management system is hooked up properly
        //          so that *all* internally allocated memory draws from a
        //          user-supplied allocator whenever one is specified.
        //       c. Objects may be allocated from the pools managed by the
        //          multi-pool.
        //    2) The destructor works properly as implicitly tested in the
        //       various scopes of this test and in the presence of exceptions.
        //   Note this test only tests one of the c'tors.
        //
        // Plan:
        //   Create a test object using the constructor: 1) without
        //   exceptions and 2) in the presence of exceptions during memory
        //   allocations using a 'bslma::TestAllocator' and varying its
        //   *allocation* *limit*.  When the object goes out of scope, verify
        //   that the destructor properly deallocates all memory that had been
        //   allocated to it.
        //
        //   Concern 2 is addressed by making use of the 'allocate' method
        //   (which is thoroughly tested in case 3).
        //
        //   At the end, include a constructor test specifying a static buffer
        //   allocator.  This tests for rudimentary correct object behavior
        //   via the destructor and Purify.
        //
        // Testing:
        //   bdema::Multipool(int numPools, bslma::Allocator *ba = 0);
        //   ~bdema::Multipool();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing 'constructor' and 'destructor'"
                          << endl << "======================================"
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
                    char *p = (char *) mX.allocate(2048);  // "overflow" pool
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
                    char *p = (char *) mX.allocate(2048);  // "overflow" pool
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
                  char *p = (char *) mX.allocate(2048);  // "overflow" pool
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
            char *p = (char *) doNotDelete->allocate(2048);  // "overflow" pool
            ASSERT(p);
            // No destructor is called; will produce memory leak in purify if
            // internal allocators are not hooked up properly.
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BASIC TEST
        //   Create a multiPooling allocator and a multiPooling pool, and
        //   initialize each with its own instance of test allocator.  Request
        //   memory of varying sizes from both the multiPooling allocator and
        //   the multipooling pool.  Verify that both test allocators contain
        //   the same number of bytes in use and the same total number of bytes
        //   requested.
        //
        // Testing:
        //   bdlma::ConcurrentMultipoolAllocator(basicAllocator);
        //   ~bdlma::ConcurrentMultipoolAllocator();
        //   void *allocate(numBytes);
        //   void release();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BASIC TEST" << endl
                                  << "==========" << endl;

        if (verbose) cout << "Testing 'allocate', 'deallocate' and 'release'."
                          << endl;

        const int DATA[] = { 5, 12, 24, 32, 64, 256, 1000 };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bslma::TestAllocator multiPoolAllocatorTA(veryVeryVerbose);
        bslma::TestAllocator multiPoolTA(veryVeryVerbose);

        bdlma::ConcurrentMultipoolAllocator sa(16, &multiPoolAllocatorTA);
        bdlma::ConcurrentMultipool sp(16, &multiPoolTA);  // TBD

        ASSERT(0 == sa.allocate(0));
        ASSERT(multiPoolAllocatorTA.numBytesInUse() ==
               multiPoolTA.numBytesInUse());

        for (int i = 0; i < NUM_DATA; ++i) {
            const int SIZE = DATA[i];
            sa.allocate(SIZE);
            sp.allocate(SIZE);
            LOOP_ASSERT(i, multiPoolAllocatorTA.numBytesInUse()
                           == multiPoolTA.numBytesInUse());
        }

        sa.release();
        sp.release();
        ASSERT(multiPoolTA.numBytesInUse() ==
                                         multiPoolAllocatorTA.numBytesInUse());
        ASSERT(multiPoolAllocatorTA.numBytesTotal() ==
                                                  multiPoolTA.numBytesTotal());

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // PERFORMANCE TEST
        // --------------------------------------------------------------------

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
            bdlma::ConcurrentMultipoolAllocator ma(NUM_POOLS,
                                                   strategy,
                                                   maxChunkSize);
            my_TestUtil::test(testLengthFactor, &ma);
        }

        printf("\nMultipool Allocator Managed with [%c], [%d]:\n\n", growth,
                                                                 maxChunkSize);
        {
            bdlma::ConcurrentMultipoolAllocator ma(NUM_POOLS,
                                                   strategy,
                                                   maxChunkSize);
            my_TestUtil::testManaged(testLengthFactor, &ma);
        }

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
