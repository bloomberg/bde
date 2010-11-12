// bdema_multipoolallocator.h                                         -*-C++-*-
#ifndef INCLUDED_BDEMA_MULTIPOOLALLOCATOR
#define INCLUDED_BDEMA_MULTIPOOLALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide memory-pooling allocator of heterogeneous block sizes.
//
//@CLASSES:
//  bdema_MultipoolAllocator: allocator managing varying-size memory pools
//
//@SEE_ALSO: bdema_pool, bdema_multipool
//
//@AUTHOR: Shao-wei Hung (shung1)
//
//@DESCRIPTION: This component provides a general-purpose, managed allocator,
// 'bdema_MultipoolAllocator', that implements the 'bslma_ManagedAllocator'
// protocol and provides an allocator that maintains a configurable number of
// 'bdema_Pool' objects, each dispensing memory blocks of a unique size.  The
// 'bdema_Pool' objects are placed in an array, starting at index 0, with each
// successive pool managing memory blocks of a size twice that of the previous
// pool.  Each multipool allocation (deallocation) request allocates memory
// from (returns memory to) the internal pool managing memory blocks of the
// smallest size not less than the requested size, or else from a separately
// managed list of memory blocks, if no internal pool managing memory block of
// sufficient size exists.  Both the 'release' method and the destructor of a
// 'bdema_MultipoolAllocator' release all memory currently allocated via the
// object.
//..
//   ,------------------------.
//  ( bdema_MultiPoolAllocator )
//   `------------------------'
//               |         ctor/dtor
//               |         maxPooledBlockSize
//               |         numPools
//               |         reserveCapacity
//               V
//    ,----------------------.
//   ( bslma_ManagedAllocator )
//    `----------------------'
//               |         release
//               V
//       ,----------------.
//      (  bslma_Allocator )
//       `----------------'
//                        allocate
//                        deallocate
//..
// The main difference between a 'bdema_MultipoolAllocator' and a
// 'bdema_Multipool' is that, very often, a 'bdema_MultipoolAllocator' is
// managed through a 'bslma_Allocator' pointer.  Hence, every call to the
// 'allocate' method invokes a virtual function call, which is slower than
// invoking the non-virtual 'allocate' method on a 'bdema_Multipool'.  However,
// since 'bslma_Allocator *' is widely used across BDE interfaces,
// 'bdema_MultipoolAllocator' is more general purpose than a 'bdema_Multipool'.
//
///Configuration At Construction
///-----------------------------
// When creating a 'bdema_MultipoolAllocator', clients can optionally
// configure:
//
//: 1 NUMBER OF POOLS -- the number of internal pools (the block size managed
//:   by the first pool is eight bytes, with each successive pool managing
//:   block of a size twice that of the previous pool).
//: 2 GROWTH STRATEGY -- geometrically growing chunk size starting from 1 (in
//:   terms of the number of memory blocks per chunk), or fixed chunk size,
//:   specified as either:
//:   o the unique growth strategy for all pools, or
//:   o (if the number of pools is specified) an array of growth strategies
//:     corresponding to each individual pool
//:   If the growth strategy is not specified, geometric growth is used for all
//:   pools.
//: 3 MAX BLOCKS PER CHUNK -- the maximum number of memory blocks within a
//:   chunk, specified as either:
//:   o (if the number of pools is specified) the unique maximum blocks per
//:     chunk value for all the pools, or
//:   o (if the number of pools is specified) an array of maximum blocks per
//:     chunk values corresponding to each individual pool.
//:   If the maximum blocks per chunk is not specified, an
//:   implementation-defined default value is used.
//: 4 BASIC ALLOCATOR -- the allocator used to supply memory (to replenish an
//:   internal pool, or directly if the maximum block size is exceeded).  If
//:   not specified, the currently installed default allocator (see
//:   'bslma_default') is used.
//
// A default-constructed multipool allocator has a relatively small,
// implementation-defined number of pools 'N' with respective block sizes
// ranging from '2^3 = 8' to '2^(N+2)'.  By default, the initial chunk size,
// (i.e., the number of blocks of a given size allocated at once to replenish a
// pool's memory) is 1, and each pool's chunk size grows geometrically until it
// reaches an implementation-defined maximum, at which it is capped.  Finally,
// unless otherwise specified, all memory comes from the allocator that was the
// currently installed default allocator at the time the
// 'bdema_MultipoolAllocator' was created.
//
// Using the various pooling options described above, we can configure the
// number of pools maintained, whether replenishment should be adaptive (i.e.,
// geometric starting with 1) or fixed at a maximum chunk size, what that
// maximum chunk size should be (which need not be an integral power of 2), and
// the underlying allocator used to supply memory.  Note that both GROWTH
// STRATEGY and MAX BLOCKS PER CHUNK can be specified separately either as a
// single value applying to all of the maintained pools, or as an array of
// values, with the elements applying to each individually maintained pool.
//
///Usage
///-----
///Example 1: Using a 'bdema_MultipoolAllocator'
///- - - - - - - - - - - - - - - - - - - - - - -
// A 'bdema_MultipoolAllocator' can be used to supply memory to node-based data
// structures such as 'bsl::set', 'bsl::list' or 'bsl::map'.  Suppose we are
// implementing a container of named graphs data structure, where a graph is
// defined by a set of edges and nodes.  The various fixed-sized nodes can be
// efficiently allocated by a 'bdema_MultipoolAllocator'.
//
// First, the edge class, 'my_Edge', is defined as follows:
//..
//  class my_Node;
//
//  class my_Edge {
//      // This class represents an edge within a graph.  Both ends of the
//      // edge must be connected to nodes.
//
//      // DATA
//      my_Node *d_first;   // first node
//      my_Node *d_second;  // second node
//
//      // ...
//
//    public:
//      // CREATORS
//      my_Edge(my_Node *first, my_Node *second);
//          // Create an edge that connects to the specified 'first' and
//          // 'second' nodes.
//
//      // ...
//  };
//
//  // CREATORS
//  my_Edge::my_Edge(my_Node *first, my_Node *second)
//  : d_first(first), d_second(second)
//  {
//  }
//..
// Then, the node class, 'my_Node', is defined as follows:
//..
//  class my_Node {
//      // This class represents a node within a graph.  A node can be
//      // connected to any number of edges.
//
//      // DATA
//      bsl::set<my_Edge *> d_edges;  // set of edges this node connects to
//
//      // ...
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(my_Node,
//                                   bslalg_TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      explicit my_Node(bslma_Allocator *basicAllocator = 0);
//          // Create a node not connected to any other nodes.  Optionally
//          // specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//
//      // ...
//  };
//
//  // CREATORS
//  my_Node::my_Node(bslma_Allocator *basicAllocator)
//  : d_edges(basicAllocator)
//  {
//  }
//..
// Then we define the graph class, 'my_Graph', as follows:
//..
//  class my_Graph {
//      // This class represents a graph having sets of nodes and edges.
//
//      // DATA
//      bsl::set<my_Edge> d_edges;  // set of edges in this graph
//      bsl::set<my_Node> d_nodes;  // set of nodes in this graph
//
//      // ...
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(my_Graph,
//                                   bslalg_TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      explicit my_Graph(bslma_Allocator *basicAllocator = 0);
//          // Create an empty graph.  Optionally specify a 'basicAllocator'
//          // used to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      // ...
//  };
//
//  my_Graph::my_Graph(bslma_Allocator *basicAllocator)
//  : d_edges(basicAllocator)
//  , d_nodes(basicAllocator)
//  {
//  }
//..
// Then finally, the container for the collection of named graphs,
// 'my_NamedGraphContainer', is defined as follows:
//..
//  class my_NamedGraphContainer {
//      // This class stores a map that index graph names to graph objects.
//
//      // DATA
//      bsl::map<bsl::string, my_Graph> d_graphMap;  // map from graph names to
//                                                   // graph
//
//    public:
//      // TRAITS
//      BSLALG_DECLARE_NESTED_TRAITS(my_NamedGraphContainer,
//                                   bslalg_TypeTraitUsesBslmaAllocator);
//
//      // CREATORS
//      explicit my_NamedGraphContainer(bslma_Allocator *basicAllocator = 0);
//          // Create an empty named graph container.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      // ...
//  };
//
//  // CREATORS
//  my_NamedGraphContainer::my_NamedGraphContainer(
//                                             bslma_Allocator *basicAllocator)
//  : d_graphMap(basicAllocator)
//  {
//  }
//..
// Finally, in 'main', we can create a 'bdema_MultipoolAllocator' and pass it
// to our 'my_NamedGraphContainer'.  Since we know that the maximum block size
// needed is 32 (comes from 'sizeof(my_Graph)'), we can calculate the number of
// pools needed by using the formula specified in the "configuration at
// construction" section:
//..
//  largestPoolSize < 2 ^ (N + 2)'.
//..
// When solved for the above equation, the smallest 'N' that satisfies this
// relationship is 3:
//..
//  int main()
//  {
//      enum { NUM_POOLS = 3 };
//
//      bdema_MultipoolAllocator basicAllocator(NUM_POOLS);
//
//      my_NamedGraphContainer container(&basicAllocator);
//  }
//..
//
///Example 2: Performance of a 'bdema_MultipoolAllocator'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdema_MultipoolAllocator' can greatly improve efficiency when it is used
// to supply memory to multiple node-based data structure, that frequently both
// insert and remove nodes, while grows to significant size before being
// destroyed.  The following experiment will illustrate the benefits of using
// the 'bdema_MultipoolAllocator' under the these scenario by comparing the
// following 3 different allocator uses:
//
//: 1 Using the 'bslma_NewDeleteAllocator directly.
//:
//: 2 Using a 'bdema_MultipoolAllocator as a direct substitute of the
//:   'bslma_NewDeleteAllocator'.
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
//  template <int OBJECT_SIZE>
//  class my_TestObject {
//
//      // DATA
//      char d_data[OBJECT_SIZE];
//
//  };
//..
// Again, for simplicity, the size of these objects are chosen to be 20, 40,
// and 80, instead of being parameterized as part of the test data structure:
//..
//  class my_TestDataStructure {
//
//      // PRIVATE TYPES
//      typedef my_TestObject<20> Obj1;
//      typedef my_TestObject<40> Obj2;
//      typedef my_TestObject<80> Obj3;
//
//      // DATA
//      bsl::list<Obj1> d_list1;
//      bsl::list<Obj2> d_list2;
//      bsl::list<Obj3> d_list3;
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
//    public:
//      // CREATORS
//      my_TestDataStructure(bslma_Allocator *basicAllocator = 0);
//
//      // MANIPULATORS
//      void pop();
//
//      void push();
//  };
//
//  // CREATORS
//  my_TestDataStructure::my_TestDataStructure(bslma_Allocator *basicAllocator)
//  : d_list1(basicAllocator)
//  , d_list2(basicAllocator)
//  , d_list3(basicAllocator)
//  {
//  }
//..
// The 'push' method will push into the 3 'bsl::list' managed by
// 'my_TestDataStructure' sequentially.  Similarly, the 'pop' method will pop
// from the lists sequentially:
//..
//  // MANIPULATORS
//  void my_TestDataStructure::push()
//  {
//          // Push to the back of the list.
//
//          d_list1.push_back(Obj1());
//          d_list2.push_back(Obj2());
//          d_list3.push_back(Obj3());
//  }
//
//  void my_TestDataStructure::pop()
//  {
//          // Pop from the back of the list.
//
//          d_list1.pop_front();
//          d_list2.pop_front();
//          d_list3.pop_front();
//  }
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
// The 'test' method will also accept an 'bslma_Allocator *', which will be
// used as the allocator used to construct the test mechanism and its internal
// lists:
//..
//  class my_TestUtil {
//
//    public:
//      // CLASS METHODS
//      static void test(int              testLengthFactor,
//                       bslma_Allocator *basicAllocator)
//      {
//          int n          = 1;
//          int iterations = 1;
//
//          for (int i = 0; i < testLengthFactor; ++i) {
//              iterations *= 10;
//          }
//
//          for (int i = 0; i <= testLengthFactor; ++i) {
//              bsls_Stopwatch timer;
//              timer.start();
//
//              for (int j = 0; j < iterations; ++j) {
//                  my_TestDataStructure tds(basicAllocator);
//
//                  // Testing cost of insertion and deletion.
//
//                  for (int k = 0; k < n; ++k) {
//                      tds.push();
//                      tds.push();
//                      tds.pop();
//                  }
//
//                  // Testing cost of destruction.
//              }
//
//              timer.stop();
//
//              printf("%d\t%d\t%d\t%1.6f\n", testLengthFactor,
//                                            n,
//                                            iterations,
//                                            timer.elapsedTime());
//
//              n          *= 10;
//              iterations /= 10;
//          }
//      }
//..
// Next, to fully test the benefit of being able to relinquish all allocated
// memory at once, we use the 'testManaged' method, which accepts only managed
// allocators.  Instead of creating the test mechanism on the stack, the test
// mechanism will be created on the heap.  After running the test, the
// 'release' method of the allocator will reclaim all outstanding allocations
// at once, eliminating the need to run the destructor of the test mechanism:
//..
//      static void testManaged(int                     testLengthFactor,
//                              bslma_ManagedAllocator *managedAllocator)
//      {
//          int n          = 1;
//          int iterations = 1;
//
//          for (int i = 0; i < testLengthFactor; ++i) {
//              iterations *= 10;
//          }
//
//          for (int i = 0; i <= testLengthFactor; ++i) {
//              bsls_Stopwatch timer;
//              timer.start();
//
//              for (int j = 0; j < iterations; ++j) {
//                  my_TestDataStructure *tm_p = new(*managedAllocator)
//                                      my_TestDataStructure(managedAllocator);
//
//                  // Testing cost of insertion and deletion.
//
//                  for (int k = 0; k < n; ++k) {
//                      tm_p->push();
//                      tm_p->push();
//                      tm_p->pop();
//                  }
//
//                  // Testing cost of destruction.
//
//                  managedAllocator->release();
//              }
//
//              timer.stop();
//
//              printf("%d\t%d\t%d\t%1.6f\n", testLengthFactor,
//                                            n,
//                                            iterations,
//                                            timer.elapsedTime());
//
//              n          *= 10;
//              iterations /= 10;
//          }
//      }
//  };
//..
// Finally, in main, we run the test with the different allocators and
// different allocator configuration based on command line arguments:
//..
//  int main(int argc, char **argv)
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
//      bsls_BlockGrowth::Strategy strategy = growth == 'g'
//                                          ? bsls_BlockGrowth::BSLS_GEOMETRIC
//                                          : bsls_BlockGrowth::BSLS_CONSTANT;
//
//      printf("\nNew Delete Allocator:\n\n");
//      {
//          bslma_Allocator *nda = bslma_NewDeleteAllocator::allocator(0);
//          my_TestUtil::test(testLengthFactor, nda);
//      }
//
//      printf("\nMultipool Allocator with [%c], [%d]:\n\n", growth,
//                                                               maxChunkSize);
//      {
//          bdema_MultipoolAllocator ma(NUM_POOLS, strategy, maxChunkSize);
//          my_TestUtil::test(testLengthFactor, &ma);
//      }
//
//      printf("\nMultipool Allocator Managed with [%c], [%d]:\n\n", growth,
//                                                               maxChunkSize);
//      {
//          bdema_MultipoolAllocator ma(NUM_POOLS, strategy, maxChunkSize);
//          my_TestUtil::testManaged(testLengthFactor, &ma);
//      }
//
//      return 0;
//  }
//..
// An excerpt of the results of the test running on IBM, under optimized mode,
// using default constructed 'bdema_MultipoolAllocator' parameters, is shown
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
// It is clear that using a 'bdema_MultipoolAllocator' results in an
// improvement in memory allocation by a factor of about 4.  Furthermore, if
// the managed aspect of the multipool allocator is exploited, the cost of
// destruction rapidly decreases in relative terms as the list ('n') grows
// larger.

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEMA_MULTIPOOL
#include <bdema_multipool.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#include <bslma_managedallocator.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                     // ==============================
                     // class bdema_MultipoolAllocator
                     // ==============================

class bdema_MultipoolAllocator : public bslma_ManagedAllocator {
    // This class implements the 'bslma_ManagedAllocator' protocol to provide
    // an allocator that maintains a configurable number of 'bdema_Pool'
    // objects,  each dispensing memory blocks of a unique size.  The
    // 'bdema_Pool' objects are placed in an array, with each successive pool
    // managing memory blocks of size twice that of the previous pool.  Each
    // multipool allocation (deallocation) request allocates memory from
    // (returns memory to) the internal pool having the smallest block size not
    // less than the requested size, or, if no pool manages memory blocks of
    // sufficient sized, from a separately managed list of memory blocks.  Both
    // the 'release' method and the destructor of a 'bdema_MultipoolAllocator '
    // release all memory currently allocated via the object.

    // DATA
    bdema_Multipool d_multipool;  // manager for allocated memory blocks

    // NOT IMPLEMENTED
    bdema_MultipoolAllocator(const bdema_MultipoolAllocator&);
    bdema_MultipoolAllocator& operator=(const bdema_MultipoolAllocator&);

  public:
    // CREATORS
    bdema_MultipoolAllocator(
                      bslma_Allocator                  *basicAllocator = 0);
    bdema_MultipoolAllocator(
                      int                               numPools,
                      bslma_Allocator                  *basicAllocator = 0);
    bdema_MultipoolAllocator(
                      bsls_BlockGrowth::Strategy        growthStrategy,
                      bslma_Allocator                  *basicAllocator = 0);
    bdema_MultipoolAllocator(
                      int                               numPools,
                      bsls_BlockGrowth::Strategy        growthStrategy,
                      bslma_Allocator                  *basicAllocator = 0);
    bdema_MultipoolAllocator(
                      int                               numPools,
                      bsls_BlockGrowth::Strategy        growthStrategy,
                      int                               maxBlocksPerChunk,
                      bslma_Allocator                  *basicAllocator = 0);
        // Create a multipool allocator.  Optionally specify 'numPools',
        // indicating the number of internally created 'bdema_Pool' objects;
        // the block size of the first pool is 8 bytes, with the block size of
        // each additional pool successively doubling.  If 'numPools' is not
        // specified, an implementation-defined number of pools 'N' -- covering
        // memory blocks ranging in size from '2^3 = 8' to '2^(N+2)' -- are
        // created.  Optionally specify a 'growthStrategy' indicating whether
        // the number of blocks allocated at once for every internally created
        // 'bdema_Pool' should be either fixed or grow geometrically, starting
        // with 1.  If 'growthStrategy' is not specified, the allocation
        // strategy for each internally created 'bdema_Pool' object is
        // geometric, starting from 1.  If 'numPools' and 'growthStrategy' are
        // specified, optionally specify a 'maxBlocksPerChunk', indicating the
        // maximum number of blocks to be allocated at once when a pool must be
        // replenished.  If 'maxBlocksPerChunk' is not specified, an
        // implementation-defined value is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Memory
        // allocation (and deallocation) requests will be satisfied using the
        // internally maintained pool managing memory blocks of the smallest
        // size not less than the requested size, or directly from the
        // underlying allocator (supplied at construction), if no internally
        // pool managing memory block of sufficient size exists.  The behavior
        // is undefined unless '1 <= numPools' and '1 <= maxBlocksPerChunk'.
        // Note that, on platforms where
        // '8 < bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT', excess memory may be
        // allocated for pools managing smaller blocks.  Also note that
        // 'maxBlocksPerChunk' need not be an integral power of 2; if geometric
        // growth would exceed the maximum value, the chunk size is capped at
        // that value).

    bdema_MultipoolAllocator(
                      int                               numPools,
                      const bsls_BlockGrowth::Strategy *growthStrategyArray,
                      bslma_Allocator                  *basicAllocator = 0);
    bdema_MultipoolAllocator(
                      int                               numPools,
                      const bsls_BlockGrowth::Strategy *growthStrategyArray,
                      int                               maxBlocksPerChunk,
                      bslma_Allocator                  *basicAllocator = 0);
    bdema_MultipoolAllocator(
                      int                               numPools,
                      bsls_BlockGrowth::Strategy        growthStrategy,
                      const int                        *maxBlocksPerChunkArray,
                      bslma_Allocator                  *basicAllocator = 0);
    bdema_MultipoolAllocator(
                      int                               numPools,
                      const bsls_BlockGrowth::Strategy *growthStrategyArray,
                      const int                        *maxBlocksPerChunkArray,
                      bslma_Allocator                  *basicAllocator = 0);
        // Create a multipool allocator having the specified 'numPools',
        // indicating the number of internally created 'bdema_Pool' objects;
        // the block size of the first pool is 8 bytes, with the block size of
        // each additional pool successively doubling.  Optionally specify a
        // 'growthStrategy' indicating whether the number of blocks allocated
        // at once for every internally created 'bdema_Pool' should be either
        // fixed or grow geometrically, starting with 1.  If 'growthStrategy'
        // is not specified, optionally specify 'growthStrategyArray',
        // indicating the strategies for each individual 'bdema_Pool' created
        // by this object.  If neither 'growthStrategy' nor
        // 'growthStrategyArray' are specified, the allocation strategy for
        // each internally created 'bdema_Pool' object will grow geometrically,
        // starting from 1.  Optionally specify a 'maxBlocksPerChunk',
        // indicating the maximum number of blocks to be allocated at once when
        // a pool must be replenished.  If 'maxBlocksPerChunk' is not
        // specified, optionally specify 'maxBlocksPerChunkArray', indicating
        // the maximum number of blocks to allocate at once for each
        // individually created 'bdema_Pool' object.  If neither
        // 'maxBlocksPerChunk' nor 'maxBlocksPerChunkArray' are specified, an
        // implementation-defined value is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Memory
        // allocation (and deallocation) requests will be satisfied using the
        // internally maintained pool managing memory blocks of the smallest
        // size not less than the requested size, or directly from the
        // underlying allocator (supplied at construction), if no internally
        // pool managing memory block of sufficient size exists.  The behavior
        // is undefined unless '1 <= numPools', 'growthStrategyArray' has at
        // least 'numPools' strategies, '1 <= maxBlocksPerChunk' and
        // 'maxBlocksPerChunkArray' have at least 'numPools' positive values.
        // Note that, on platforms where
        // '8 < bsls_AlignmentUtil::BSLS_MAX_ALIGNMENT', excess memory may be
        // allocated for pools managing smaller blocks.  Also note that the
        // maximum need not be an integral power of 2; if geometric growth
        // would exceed a maximum value, the chunk size is capped at that
        // value).

    virtual ~bdema_MultipoolAllocator();
        // Destroy this multipool allocator.  All memory allocated from this
        // allocator is released.

    // MANIPULATORS
    void reserveCapacity(size_type size, size_type numObjects);
        // Reserve memory from this multipool allocator to satisfy memory
        // requests for at least the specified 'numObjects' having the
        // specified 'size' (in bytes) before the pool replenishes.  If 'size'
        // is 0, this method has no effect.  The behavior is undefined unless
        // '0 <= size', 'size <= maxPooledBlockSize()', and '0 <= numObjects'.

                                // Virtual Functions

    virtual void *allocate(size_type size);
        // Return the address of a contiguous block of maximally aligned memory
        // of (at least) the specified 'size' (in bytes).  If 'size' is 0, no
        // memory is allocated and 0 is returned.  If
        // 'size > maxPooledBlockSize()', the memory allocation is managed
        // directly by the underlying allocator, but will not be pooled .  The
        // behavior is undefined unless '0 <= size'.

    virtual void deallocate(void *address);
        // Return the memory block at the specified 'address' back to this
        // allocator for reuse.  If 'address' is 0, this method has no effect.
        // The behavior is undefined unless 'address' was allocated by this
        // allocator, and has not already been deallocated.

    virtual void release();
        // Release all memory currently allocated through this multipool
        // allocator.

    // ACCESSORS
    int numPools() const;
        // Return the number of pools managed by this multipool allocator.

    int maxPooledBlockSize() const;
        // Return the maximum size of memory blocks that are pooled by this
        // multipool allocator.  Note that the maximum value is defined as:
        //..
        //  2 ^ (numPools + 2)
        //..
        // where 'numPools' is either specified at construction, or an
        // implementation-defined value.
};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // ------------------------------
                     // class bdema_MultipoolAllocator
                     // ------------------------------

// CREATORS
inline
bdema_MultipoolAllocator::bdema_MultipoolAllocator(
                  bslma_Allocator                  *basicAllocator)
: d_multipool(basicAllocator)
{
}

inline
bdema_MultipoolAllocator::bdema_MultipoolAllocator(
                  int                               numPools,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools, basicAllocator)
{
}

inline
bdema_MultipoolAllocator::bdema_MultipoolAllocator(
                  bsls_BlockGrowth::Strategy        growthStrategy,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(growthStrategy, basicAllocator)
{
}

inline
bdema_MultipoolAllocator::bdema_MultipoolAllocator(
                  int                               numPools,
                  bsls_BlockGrowth::Strategy        growthStrategy,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategy, basicAllocator)
{
}

inline
bdema_MultipoolAllocator::bdema_MultipoolAllocator(
                  int                               numPools,
                  const bsls_BlockGrowth::Strategy *growthStrategyArray,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategyArray, basicAllocator)
{
}

inline
bdema_MultipoolAllocator::bdema_MultipoolAllocator(
                  int                               numPools,
                  bsls_BlockGrowth::Strategy        growthStrategy,
                  int                               maxBlocksPerChunk,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategy, maxBlocksPerChunk, basicAllocator)
{
}

inline
bdema_MultipoolAllocator::bdema_MultipoolAllocator(
                  int                               numPools,
                  const bsls_BlockGrowth::Strategy *growthStrategyArray,
                  int                               maxBlocksPerChunk,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategyArray, maxBlocksPerChunk, basicAllocator)
{
}

inline
bdema_MultipoolAllocator::bdema_MultipoolAllocator(
                  int                               numPools,
                  bsls_BlockGrowth::Strategy        growthStrategy,
                  const int                        *maxBlocksPerChunkArray,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategy, maxBlocksPerChunkArray, basicAllocator)
{
}

inline
bdema_MultipoolAllocator::bdema_MultipoolAllocator(
                  int                               numPools,
                  const bsls_BlockGrowth::Strategy *growthStrategyArray,
                  const int                        *maxBlocksPerChunkArray,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools,
              growthStrategyArray,
              maxBlocksPerChunkArray,
              basicAllocator)
{
}

// MANIPULATORS
inline
void bdema_MultipoolAllocator::release()
{
    d_multipool.release();
}

// ACCESSORS
inline
int bdema_MultipoolAllocator::numPools() const
{
    return d_multipool.numPools();
}

inline
int bdema_MultipoolAllocator::maxPooledBlockSize() const
{
    return d_multipool.maxPooledBlockSize();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
