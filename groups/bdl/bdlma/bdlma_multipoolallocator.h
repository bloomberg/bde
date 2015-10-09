// bdlma_multipoolallocator.h                                         -*-C++-*-
#ifndef INCLUDED_BDLMA_MULTIPOOLALLOCATOR
#define INCLUDED_BDLMA_MULTIPOOLALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a memory-pooling allocator of heterogeneous block sizes.
//
//@CLASSES:
//  bdlma::MultipoolAllocator: allocator managing varying-size memory pools
//
//@SEE_ALSO: bdlma_pool, bdlma_multipool
//
//@DESCRIPTION: This component provides a general-purpose, managed allocator,
// 'bdlma::MultipoolAllocator', that implements the 'bdlma::ManagedAllocator'
// protocol and provides an allocator that maintains a configurable number of
// 'bdlma::Pool' objects, each dispensing maximally-aligned memory blocks of a
// unique size.  The 'bdlma::Pool' objects are placed in an array, starting at
// index 0, with each successive pool managing memory blocks of a size twice
// that of the previous pool.  Each multipool allocation (deallocation) request
// allocates memory from (returns memory to) the internal pool managing memory
// blocks of the smallest size not less than the requested size, or else from a
// separately managed list of memory blocks, if no internal pool managing
// memory blocks of sufficient size exists.  Both the 'release' method and the
// destructor of a 'bdlma::MultipoolAllocator' release all memory currently
// allocated via the object.
//..
//   ,-------------------------.
//  ( bdlma::MultipoolAllocator )
//   `-------------------------'
//                |         ctor/dtor
//                |         maxPooledBlockSize
//                |         numPools
//                |         reserveCapacity
//                V
//    ,-----------------------.
//   ( bdlma::ManagedAllocator )
//    `-----------------------'
//                |         release
//                V
//       ,----------------.
//      ( bslma::Allocator )
//       `----------------'
//                          allocate
//                          deallocate
//..
// The main difference between a 'bdlma::MultipoolAllocator' and a
// 'bdlma::Multipool' is that, very often, a 'bdlma::MultipoolAllocator' is
// managed through a 'bslma::Allocator' pointer.  Hence, every call to the
// 'allocate' method invokes a virtual function call, which is slower than
// invoking the non-virtual 'allocate' method on a 'bdlma::Multipool'.
// However, since 'bslma::Allocator *' is widely used across BDE interfaces,
// 'bdlma::MultipoolAllocator' is more general purpose than 'bdlma::Multipool'.
//
///Configuration at Construction
///-----------------------------
// When creating a 'bdlma::MultipoolAllocator', clients can optionally
// configure:
//
//: 1 NUMBER OF POOLS -- the number of internal pools (the block size managed
//:   by the first pool is eight bytes, with each successive pool managing
//:   blocks of a size twice that of the previous pool).
//: 2 GROWTH STRATEGY -- geometrically growing chunk size starting from 1 (in
//:   terms of the number of memory blocks per chunk), or fixed chunk size,
//:   specified as either:
//:   o the unique growth strategy for all pools, or
//:   o (if the number of pools is specified) an array of growth strategies
//:     corresponding to each individual pool.
//:   If the growth strategy is not specified, geometric growth is used for all
//:   pools.
//: 3 MAX BLOCKS PER CHUNK -- the maximum number of memory blocks within a
//:   chunk, specified as either:
//:     o the unique maximum-blocks-per-chunk value for all of the pools, or
//:     o an array of maximum-blocks-per-chunk values corresponding to each
//:       individual pool.
//:   If the maximum blocks per chunk is not specified, an
//:   implementation-defined default value is used.  Note that the maximum
//:   blocks per chunk can be configured only if the number of pools is also
//:   configured.
//: 4 BASIC ALLOCATOR -- the allocator used to supply memory (to replenish an
//:   internal pool, or directly if the maximum block size is exceeded).  If
//:   not specified, the currently installed default allocator is used (see
//:   'bslma_default').
//
// A default-constructed multipool allocator has a relatively small,
// implementation-defined number of pools, 'N', with respective block sizes
// ranging from '2^3 = 8' to '2^(N+2)'.  By default, the initial chunk size,
// (i.e., the number of blocks of a given size allocated at once to replenish a
// pool's memory) is 1, and each pool's chunk size grows geometrically until it
// reaches an implementation-defined maximum, at which it is capped.  Finally,
// unless otherwise specified, all memory comes from the allocator that was the
// currently installed default allocator at the time the
// 'bdlma::MultipoolAllocator' was created.
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
// This section illustrates intended use of this component.
//
///Example 1: Using a 'bdlma::MultipoolAllocator'
/// - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::MultipoolAllocator' can be used to supply memory to node-based
// data structures such as 'bsl::set', 'bsl::list', and 'bsl::map'.  Suppose we
// are implementing a container of named graphs, where a graph is defined by a
// set of edges and a set of nodes.  The various fixed-sized nodes and edges
// can be efficiently allocated by a 'bdlma::MultipoolAllocator'.
//
// First, the edge class, 'my_Edge', is defined as follows:
//..
//  class my_Node;
//
//  class my_Edge {
//      // This class represents an edge within a graph.  Both ends of an edge
//      // must be connected to nodes.
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
//  : d_first(first)
//  , d_second(second)
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
//    private:
//      // Not implemented:
//      my_Node(const my_Node&);
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(my_Node, bslma::UsesBslmaAllocator);
//
//      // CREATORS
//      explicit my_Node(bslma::Allocator *basicAllocator = 0);
//          // Create a node not connected to any other nodes.  Optionally
//          // specify a 'basicAllocator' used to supply memory.  If
//          // 'basicAllocator' is 0, the currently installed default allocator
//          // is used.
//
//      // ...
//  };
//
//  // CREATORS
//  my_Node::my_Node(bslma::Allocator *basicAllocator)
//  : d_edges(basicAllocator)
//  {
//  }
//..
// Then, we define the graph class, 'my_Graph', as follows:
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
//    private:
//      // Not implemented:
//      my_Graph(const my_Graph&);
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(my_Graph, bslma::UsesBslmaAllocator);
//
//      // CREATORS
//      explicit my_Graph(bslma::Allocator *basicAllocator = 0);
//          // Create an empty graph.  Optionally specify a 'basicAllocator'
//          // used to supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      // ...
//  };
//
//  my_Graph::my_Graph(bslma::Allocator *basicAllocator)
//  : d_edges(basicAllocator)
//  , d_nodes(basicAllocator)
//  {
//  }
//..
// Next, the container for the collection of named graphs,
// 'my_NamedGraphContainer', is defined as follows:
//..
//  class my_NamedGraphContainer {
//      // This class stores a map that indexes graph names to graph objects.
//
//      // DATA
//      bsl::map<bsl::string, my_Graph> d_graphMap;  // map from graph name to
//                                                   // graph
//
//    private:
//      // Not implemented:
//      my_NamedGraphContainer(const my_NamedGraphContainer&);
//
//    public:
//      // TRAITS
//      BSLMF_NESTED_TRAIT_DECLARATION(my_NamedGraphContainer,
//                                     bslma::UsesBslmaAllocator);
//
//      // CREATORS
//      explicit my_NamedGraphContainer(bslma::Allocator *basicAllocator = 0);
//          // Create an empty named graph container.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      // ...
//  };
//
//  // CREATORS
//  my_NamedGraphContainer::my_NamedGraphContainer(
//                                            bslma::Allocator *basicAllocator)
//  : d_graphMap(basicAllocator)
//  {
//  }
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
///- - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
//  template <int OBJECT_SIZE>
//  class my_TestObject {
//
//      // DATA
//      char d_data[OBJECT_SIZE];
//  };
//..
// Again, for simplicity, the sizes of these objects are chosen to be 20, 40,
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
// are rarely those recently added (this also removes the possibility of noise
// from potential optimizations with relinquishing memory blocks that are most
// recently allocated).
//..
//    public:
//      // CREATORS
//      my_TestDataStructure(bslma::Allocator *basicAllocator = 0);
//
//      // MANIPULATORS
//      void pop();
//
//      void push();
//  };
//
//  // CREATORS
//  my_TestDataStructure::my_TestDataStructure(
//                                            bslma::Allocator *basicAllocator)
//  : d_list1(basicAllocator)
//  , d_list2(basicAllocator)
//  , d_list3(basicAllocator)
//  {
//  }
//..
// The 'push' method will push into the 3 'bsl::list' objects managed by
// 'my_TestDataStructure' sequentially.  Similarly, the 'pop' method will pop
// from the lists sequentially:
//..
//  // MANIPULATORS
//  void my_TestDataStructure::push()
//  {
//      // Push to the back of the 3 lists.
//
//      d_list1.push_back(Obj1());
//      d_list2.push_back(Obj2());
//      d_list3.push_back(Obj3());
//  }
//
//  void my_TestDataStructure::pop()
//  {
//      // Pop from the front of the 3 lists.
//
//      d_list1.pop_front();
//      d_list2.pop_front();
//      d_list3.pop_front();
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
// structure will grow by invoking 'push' twice and 'pop' once.  Note that "n"
// measures the effect of insertion and removal of nodes, while "iterations"
// measures the effect of construction and destruction of entire lists of
// nodes.
//
// The 'test' method also accepts a 'bslma::Allocator *' to be used as the
// allocator used to construct the test mechanism and its internal lists:
//..
//  class my_TestUtil {
//
//    public:
//      // CLASS METHODS
//      static
//      void test(int testLengthFactor, bslma::Allocator *basicAllocator)
//      {
//          int n          = 1;
//          int iterations = 1;
//
//          for (int i = 0; i < testLengthFactor; ++i) {
//              iterations *= 10;
//          }
//
//          for (int i = 0; i <= testLengthFactor; ++i) {
//              bsls::Stopwatch timer;
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
//      static
//      void testManaged(int                      testLengthFactor,
//                       bdlma::ManagedAllocator *managedAllocator)
//      {
//          int n          = 1;
//          int iterations = 1;
//
//          for (int i = 0; i < testLengthFactor; ++i) {
//              iterations *= 10;
//          }
//
//          for (int i = 0; i <= testLengthFactor; ++i) {
//              bsls::Stopwatch timer;
//              timer.start();
//
//              for (int j = 0; j < iterations; ++j) {
//                  my_TestDataStructure *tmPtr = new(*managedAllocator)
//                                      my_TestDataStructure(managedAllocator);
//
//                  // Testing cost of insertion and deletion.
//
//                  for (int k = 0; k < n; ++k) {
//                      tmPtr->push();
//                      tmPtr->push();
//                      tmPtr->pop();
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMA_MANAGEDALLOCATOR
#include <bdlma_managedallocator.h>
#endif

#ifndef INCLUDED_BDLMA_MULTIPOOL
#include <bdlma_multipool.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {
namespace bdlma {

                         // ========================
                         // class MultipoolAllocator
                         // ========================

class MultipoolAllocator : public ManagedAllocator {
    // This class implements the 'bdlma::ManagedAllocator' protocol to provide
    // an allocator that maintains a configurable number of 'bdlma::Pool'
    // objects, each dispensing memory blocks of a unique size.  The
    // 'bdlma::Pool' objects are placed in an array, with each successive pool
    // managing memory blocks of size twice that of the previous pool.  Each
    // multipool allocation (deallocation) request allocates memory from
    // (returns memory to) the internal pool having the smallest block size not
    // less than the requested size, or, if no pool manages memory blocks of
    // sufficient sized, from a separately managed list of memory blocks.  Both
    // the 'release' method and the destructor of a 'bdlma::MultipoolAllocator'
    // release all memory currently allocated via the object.

    // DATA
    Multipool d_multipool;  // manager for allocated memory blocks

  private:
    // NOT IMPLEMENTED
    MultipoolAllocator(const MultipoolAllocator&);
    MultipoolAllocator& operator=(const MultipoolAllocator&);

  public:
    // CREATORS
    explicit
    MultipoolAllocator(bslma::Allocator *basicAllocator = 0);
    explicit
    MultipoolAllocator(int numPools, bslma::Allocator *basicAllocator = 0);
    explicit
    MultipoolAllocator(bsls::BlockGrowth::Strategy  growthStrategy,
                       bslma::Allocator            *basicAllocator = 0);
    MultipoolAllocator(int                          numPools,
                       bsls::BlockGrowth::Strategy  growthStrategy,
                       bslma::Allocator            *basicAllocator = 0);
    MultipoolAllocator(int                          numPools,
                       bsls::BlockGrowth::Strategy  growthStrategy,
                       int                          maxBlocksPerChunk,
                       bslma::Allocator            *basicAllocator = 0);
        // Create a multipool allocator.  Optionally specify 'numPools',
        // indicating the number of internally created 'bdlma::Pool' objects;
        // the block size of the first pool is 8 bytes, with the block size of
        // each additional pool successively doubling.  If 'numPools' is not
        // specified, an implementation-defined number of pools 'N' -- covering
        // memory blocks ranging in size from '2^3 = 8' to '2^(N+2)' -- are
        // created.  Optionally specify a 'growthStrategy' indicating whether
        // the number of blocks allocated at once for every internally created
        // 'bdlma::Pool' should be either fixed or grow geometrically, starting
        // with 1.  If 'growthStrategy' is not specified, the allocation
        // strategy for each internally created 'bdlma::Pool' object is
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
        // underlying allocator (supplied at construction), if no internal pool
        // managing memory blocks of sufficient size exists.  The behavior is
        // undefined unless '1 <= numPools' and '1 <= maxBlocksPerChunk'.  Note
        // that, on platforms where
        // '8 < bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT', excess memory may be
        // allocated for pools managing smaller blocks.  Also note that
        // 'maxBlocksPerChunk' need not be an integral power of 2; if geometric
        // growth would exceed the maximum value, the chunk size is capped at
        // that value.

    MultipoolAllocator(int                                numPools,
                       const bsls::BlockGrowth::Strategy *growthStrategyArray,
                       bslma::Allocator                  *basicAllocator = 0);
    MultipoolAllocator(int                                numPools,
                       const bsls::BlockGrowth::Strategy *growthStrategyArray,
                       int                                maxBlocksPerChunk,
                       bslma::Allocator                  *basicAllocator = 0);
    MultipoolAllocator(int                          numPools,
                       bsls::BlockGrowth::Strategy  growthStrategy,
                       const int                   *maxBlocksPerChunkArray,
                       bslma::Allocator            *basicAllocator = 0);
    MultipoolAllocator(
                     int                                numPools,
                     const bsls::BlockGrowth::Strategy *growthStrategyArray,
                     const int                         *maxBlocksPerChunkArray,
                     bslma::Allocator                  *basicAllocator = 0);
        // Create a multipool allocator having the specified 'numPools',
        // indicating the number of internally created 'bdlma::Pool' objects;
        // the block size of the first pool is 8 bytes, with the block size of
        // each additional pool successively doubling.  Optionally specify a
        // 'growthStrategy' indicating whether the number of blocks allocated
        // at once for every internally created 'bdlma::Pool' should be either
        // fixed or grow geometrically, starting with 1.  If 'growthStrategy'
        // is not specified, optionally specify a 'growthStrategyArray',
        // indicating the strategies for each individual 'bdlma::Pool' created
        // by this object.  If neither 'growthStrategy' nor
        // 'growthStrategyArray' is specified, the allocation strategy for each
        // internally created 'bdlma::Pool' object will grow geometrically,
        // starting from 1.  Optionally specify a 'maxBlocksPerChunk',
        // indicating the maximum number of blocks to be allocated at once when
        // a pool must be replenished.  If 'maxBlocksPerChunk' is not
        // specified, optionally specify a 'maxBlocksPerChunkArray', indicating
        // the maximum number of blocks to allocate at once for each
        // individually created 'bdlma::Pool' object.  If neither
        // 'maxBlocksPerChunk' nor 'maxBlocksPerChunkArray' is specified, an
        // implementation-defined value is used.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  Memory
        // allocation (and deallocation) requests will be satisfied using the
        // internally maintained pool managing memory blocks of the smallest
        // size not less than the requested size, or directly from the
        // underlying allocator (supplied at construction), if no internal pool
        // managing memory blocks of sufficient size exists.  The behavior is
        // undefined unless '1 <= numPools', 'growthStrategyArray' has at least
        // 'numPools' strategies, '1 <= maxBlocksPerChunk', and
        // 'maxBlocksPerChunkArray' has at least 'numPools' positive values.
        // Note that, on platforms where
        // '8 < bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT', excess memory may be
        // allocated for pools managing smaller blocks.  Also note that the
        // maximum need not be an integral power of 2; if geometric growth
        // would exceed a maximum value, the chunk size is capped at that
        // value.

    virtual ~MultipoolAllocator();
        // Destroy this multipool allocator.  All memory allocated from this
        // allocator is released.

    // MANIPULATORS
    void reserveCapacity(size_type size, size_type numObjects);
        // Reserve memory from this multipool allocator to satisfy memory
        // requests for at least the specified 'numObjects' having the
        // specified 'size' (in bytes) before the pool replenishes.  If 'size'
        // is 0, this method has no effect.  The behavior is undefined unless
        // 'size <= maxPooledBlockSize()'.

                                // Virtual Functions

    virtual void *allocate(size_type size);
        // Return the address of a contiguous block of maximally-aligned memory
        // of (at least) the specified 'size' (in bytes).  If 'size' is 0, no
        // memory is allocated and 0 is returned.  If
        // 'size > maxPooledBlockSize()', the memory allocation is managed
        // directly by the underlying allocator, but will not be pooled .

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
//                             INLINE DEFINITIONS
// ============================================================================

                         // ------------------------
                         // class MultipoolAllocator
                         // ------------------------

// CREATORS
inline
MultipoolAllocator::MultipoolAllocator(
                     bslma::Allocator                  *basicAllocator)
: d_multipool(basicAllocator)
{
}

inline
MultipoolAllocator::MultipoolAllocator(
                     int                                numPools,
                     bslma::Allocator                  *basicAllocator)
: d_multipool(numPools, basicAllocator)
{
}

inline
MultipoolAllocator::MultipoolAllocator(
                     bsls::BlockGrowth::Strategy        growthStrategy,
                     bslma::Allocator                  *basicAllocator)
: d_multipool(growthStrategy, basicAllocator)
{
}

inline
MultipoolAllocator::MultipoolAllocator(
                     int                                numPools,
                     bsls::BlockGrowth::Strategy        growthStrategy,
                     bslma::Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategy, basicAllocator)
{
}

inline
MultipoolAllocator::MultipoolAllocator(
                     int                                numPools,
                     const bsls::BlockGrowth::Strategy *growthStrategyArray,
                     bslma::Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategyArray, basicAllocator)
{
}

inline
MultipoolAllocator::MultipoolAllocator(
                     int                                numPools,
                     bsls::BlockGrowth::Strategy        growthStrategy,
                     int                                maxBlocksPerChunk,
                     bslma::Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategy, maxBlocksPerChunk, basicAllocator)
{
}

inline
MultipoolAllocator::MultipoolAllocator(
                     int                                numPools,
                     const bsls::BlockGrowth::Strategy *growthStrategyArray,
                     int                                maxBlocksPerChunk,
                     bslma::Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategyArray, maxBlocksPerChunk, basicAllocator)
{
}

inline
MultipoolAllocator::MultipoolAllocator(
                     int                                numPools,
                     bsls::BlockGrowth::Strategy        growthStrategy,
                     const int                         *maxBlocksPerChunkArray,
                     bslma::Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategy, maxBlocksPerChunkArray, basicAllocator)
{
}

inline
MultipoolAllocator::MultipoolAllocator(
                     int                                numPools,
                     const bsls::BlockGrowth::Strategy *growthStrategyArray,
                     const int                         *maxBlocksPerChunkArray,
                     bslma::Allocator                  *basicAllocator)
: d_multipool(numPools,
              growthStrategyArray,
              maxBlocksPerChunkArray,
              basicAllocator)
{
}

// MANIPULATORS
inline
void MultipoolAllocator::release()
{
    d_multipool.release();
}

// ACCESSORS
inline
int MultipoolAllocator::numPools() const
{
    return d_multipool.numPools();
}

inline
int MultipoolAllocator::maxPooledBlockSize() const
{
    return d_multipool.maxPooledBlockSize();
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
