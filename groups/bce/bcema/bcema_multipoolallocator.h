// bcema_multipoolallocator.h                                         -*-C++-*-
#ifndef INCLUDED_BCEMA_MULTIPOOLALLOCATOR
#define INCLUDED_BCEMA_MULTIPOOLALLOCATOR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an allocator to manage pools of varying object sizes.
//
//@CLASSES:
//   bcema_MultipoolAllocator: allocator managing varying size memory pools
//
//@SEE_ALSO: bcema_pool, bcema_multipool
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides an allocator,
// 'bcema_MultipoolAllocator', that implements the 'bslma_ManagedAllocator'
// protocol and provides an allocator that maintains a configurable number of
// 'bcema_Pool' objects, each dispensing memory blocks of a unique size.  The
// 'bcema_Pool' objects are placed in an array, starting at index 0, with each
// successive pool managing memory blocks of a size twice that of the previous
// pool.  Each multipool allocation (deallocation) request allocates memory
// from (returns memory to) the internal pool managing memory blocks of the
// smallest size not less than the requested size, or else from a separately
// managed list of memory blocks, if no internal pool managing memory block of
// sufficient size exists.  Both the 'release' method and the destructor of a
// 'bcema_MultipoolAllocator' release all memory currently allocated via the
// object.
//..
//   ,------------------------.
//  ( bcema_MultiPoolAllocator )
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
// The main difference between a 'bcema_MultipoolAllocator' and a
// 'bcema_Multipool' is that, very often, a 'bcema_MultipoolAllocator' is
// managed through a 'bslma_Allocator' pointer.  Hence, every call to the
// 'allocate' method invokes a virtual function call, which is slower than
// invoking the non-virtual 'allocate' method on a 'bcema_Multipool'.  However,
// since 'bslma_Allocator *' is widely used across BDE interfaces,
// 'bcema_MultipoolAllocator' is more general purposed than a
// 'bcema_Multipool'.
//
///Configuration At Construction
///-----------------------------
// When creating a 'bcema_MultipoolAllocator', clients can optionally
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
// 'bcema_MultipoolAllocator' was created.
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
// A 'bcema_MultipoolAllocator' can be used to supply memory to node-based data
// structures such as 'bsl::set', 'bsl::list' or 'bsl::map'.  Suppose we are
// implementing a container of named graphs data structure, where a graph is
// defined by a set of edges and nodes.  The various fixed-sized nodes can be
// efficiently allocated by a 'bcema_MultipoolAllocator'.
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
// Finally, in 'main', we can create a 'bcema_MultipoolAllocator' and pass it
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
//      bcema_MultipoolAllocator basicAllocator(NUM_POOLS);
//
//      my_NamedGraphContainer container(&basicAllocator);
//  }
//..


#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMA_MULTIPOOL
#include <bcema_multipool.h>
#endif

#ifndef INCLUDED_BSLMA_MANAGEDALLOCATOR
#include <bslma_managedallocator.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                     // ==============================
                     // class bcema_MultipoolAllocator
                     // ==============================

class bcema_MultipoolAllocator : public bslma_ManagedAllocator {
    // This class implements the 'bslma_ManagedAllocator' protocol to provide
    // a thread-enabled allocator that maintains a configurable number of
    // 'bcema_Pool' objects,  each dispensing memory blocks of a unique size.
    // The 'bcema_Pool' objects are placed in an array, with each successive
    // pool managing memory blocks of size twice that of the previous pool.
    // Each multipool allocation (deallocation) request allocates memory from
    // (returns memory to) the internal pool having the smallest block size not
    // less than the requested size, or, if no pool manages memory blocks of
    // sufficient sized, from a separately managed list of memory blocks.  Both
    // the 'release' method and the destructor of a 'bcema_MultipoolAllocator '
    // release all memory currently allocated via the object.

    // DATA
    bcema_Multipool d_multipool;  // owned allocator

  private:
    // NOT IMPLEMENTED
    bcema_MultipoolAllocator(const bcema_MultipoolAllocator&);
    bcema_MultipoolAllocator& operator=(const bcema_MultipoolAllocator&);

  public:
    // CREATORS
    bcema_MultipoolAllocator(
                      bslma_Allocator                  *basicAllocator = 0);
    bcema_MultipoolAllocator(
                      int                               numPools,
                      bslma_Allocator                  *basicAllocator = 0);
    bcema_MultipoolAllocator(
                      bsls_BlockGrowth::Strategy        growthStrategy,
                      bslma_Allocator                  *basicAllocator = 0);
    bcema_MultipoolAllocator(
                      int                               numPools,
                      bsls_BlockGrowth::Strategy        growthStrategy,
                      bslma_Allocator                  *basicAllocator = 0);
    bcema_MultipoolAllocator(
                      int                               numPools,
                      bsls_BlockGrowth::Strategy        growthStrategy,
                      int                               maxBlocksPerChunk,
                      bslma_Allocator                  *basicAllocator = 0);
        // Create a multipool allocator.  Optionally specify 'numPools',
        // indicating the number of internally created 'bcema_Pool' objects;
        // the block size of the first pool is 8 bytes, with the block size of
        // each additional pool successively doubling.  If 'numPools' is not
        // specified, an implementation-defined number of pools 'N' -- covering
        // memory blocks ranging in size from '2^3 = 8' to '2^(N+2)' -- are
        // created.  Optionally specify a 'growthStrategy' indicating whether
        // the number of blocks allocated at once for every internally created
        // 'bcema_Pool' should be either fixed or grow geometrically, starting
        // with 1.  If 'growthStrategy' is not specified, the allocation
        // strategy for each internally created 'bcema_Pool' object is
        // geometric, starting from 1.  If 'numPools' is specified,
        // optionally specify a 'maxBlocksPerChunk', indicating the maximum
        // number of blocks to be allocated at once when a pool must be
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

    bcema_MultipoolAllocator(
                      int                               numPools,
                      const bsls_BlockGrowth::Strategy *growthStrategyArray,
                      bslma_Allocator                  *basicAllocator = 0);
    bcema_MultipoolAllocator(
                      int                               numPools,
                      const bsls_BlockGrowth::Strategy *growthStrategyArray,
                      int                               maxBlocksPerChunk,
                      bslma_Allocator                  *basicAllocator = 0);
    bcema_MultipoolAllocator(
                      int                               numPools,
                      bsls_BlockGrowth::Strategy        growthStrategy,
                      const int                        *maxBlocksPerChunkArray,
                      bslma_Allocator                  *basicAllocator = 0);
    bcema_MultipoolAllocator(
                      int                               numPools,
                      const bsls_BlockGrowth::Strategy *growthStrategyArray,
                      const int                        *maxBlocksPerChunkArray,
                      bslma_Allocator                  *basicAllocator = 0);
        // Create a multipool allocator having the specified 'numPools',
        // indicating the number of internally created 'bcema_Pool' objects;
        // the block size of the first pool is 8 bytes, with the block size of
        // each additional pool successively doubling.  Optionally specify a
        // 'growthStrategy' indicating whether the number of blocks allocated
        // at once for every internally created 'bcema_Pool' should be either
        // fixed or grow geometrically, starting with 1.  If 'growthStrategy'
        // is not specified, optionally specify 'growthStrategyArray',
        // indicating the strategies for each individual 'bcema_Pool' created
        // by this object.  If neither 'growthStrategy' nor
        // 'growthStrategyArray' are specified, the allocation strategy for
        // each internally created 'bcema_Pool' object will grow geometrically,
        // starting from 1.  Optionally specify a 'maxBlocksPerChunk',
        // indicating the maximum number of blocks to be allocated at once when
        // a pool must be replenished.  If 'maxBlocksPerChunk' is not
        // specified, optionally specify 'maxBlocksPerChunkArray', indicating
        // the maximum number of blocks to allocate at once for each
        // individually created 'bcema_Pool' object.  If neither
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

    virtual ~bcema_MultipoolAllocator();
        // Destroy this multipool allocator.  All memory allocated from this
        // allocator is released.

    // MANIPULATORS
    void reserveCapacity(size_type size, size_type numObjects);
        // Reserve memory from this multipool allocator to satisfy memory
        // requests for at least the specified 'numObjects' having the
        // specified 'size' (in bytes) before the pool replenishes.  If 'size'
        // is 0, this method has no effect.  The behavior is undefined unless
        // '0 <= size', 'size < maxPooledBlockSize()', and '0 <= numObjects'.

                                // Virtual Functions

    virtual void *allocate(size_type size);
        // Return the address of a contiguous block of maximally aligned memory
        // of (at least) the specified 'size' (in bytes).  If 'size' is 0, no
        // memory is allocated and 0 is returned.  If
        // 'size > maxPooledBlockSize()', the memory allocation is managed
        // directly by the underlying allocator, but will not be pooled .  The
        // behavior is undefined unless '0 <= size'.

    virtual void deallocate(void *address);
        // Relinquish the memory block at the specified 'address' back to this
        // allocator for reuse.  If 'address' is 0, this method has no effect.
        // The behavior is undefined unless 'address' was allocated by this
        // allocator, and has not already been deallocated.

    virtual void release();
        // Relinquish all memory currently allocated through this multipool
        // allocator.

    // ACCESSORS
    int numPools() const;
        // Return the number of pools managed by this multipool allocator.

    int maxPooledBlockSize() const;
        // Return the maximum size of memory blocks that are pooled by this
        // multipool object.  Note that the maximum value is defined as:
        //..
        //  2 ^ (numPools + 2)
        //..
        // where 'numPools' is either specified at construction, or an
        // implementation-defined value.

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    int maxPooledObjectSize() const;
        // [!DEPRECATED!] Return the maximum size of memory blocks that are
        // pooled by this multipool object.  Note that the maximum value is
        // defined as:
        //..
        //  2 ^ (numPools + 2)
        //..
        // where 'numPools' is either specified at construction, or an
        // implementation-defined value.
#endif

};

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                     // ------------------------------
                     // class bcema_MultipoolAllocator
                     // ------------------------------

// CREATORS
inline
bcema_MultipoolAllocator::bcema_MultipoolAllocator(
                  bslma_Allocator                  *basicAllocator)
: d_multipool(basicAllocator)
{
}

inline
bcema_MultipoolAllocator::bcema_MultipoolAllocator(
                  int                               numPools,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools, basicAllocator)
{
}

inline
bcema_MultipoolAllocator::bcema_MultipoolAllocator(
                  bsls_BlockGrowth::Strategy        growthStrategy,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(growthStrategy, basicAllocator)
{
}

inline
bcema_MultipoolAllocator::bcema_MultipoolAllocator(
                  int                               numPools,
                  bsls_BlockGrowth::Strategy        growthStrategy,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategy, basicAllocator)
{
}

inline
bcema_MultipoolAllocator::bcema_MultipoolAllocator(
                  int                               numPools,
                  const bsls_BlockGrowth::Strategy *growthStrategyArray,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategyArray, basicAllocator)
{
}

inline
bcema_MultipoolAllocator::bcema_MultipoolAllocator(
                  int                               numPools,
                  bsls_BlockGrowth::Strategy        growthStrategy,
                  int                               maxBlocksPerChunk,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategy, maxBlocksPerChunk, basicAllocator)
{
}

inline
bcema_MultipoolAllocator::bcema_MultipoolAllocator(
                  int                               numPools,
                  const bsls_BlockGrowth::Strategy *growthStrategyArray,
                  int                               maxBlocksPerChunk,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategyArray, maxBlocksPerChunk, basicAllocator)
{
}

inline
bcema_MultipoolAllocator::bcema_MultipoolAllocator(
                  int                               numPools,
                  bsls_BlockGrowth::Strategy        growthStrategy,
                  const int                        *maxBlocksPerChunkArray,
                  bslma_Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategy, maxBlocksPerChunkArray, basicAllocator)
{
}

inline
bcema_MultipoolAllocator::bcema_MultipoolAllocator(
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

// ACCESSORS
inline
int bcema_MultipoolAllocator::numPools() const
{
    return d_multipool.numPools();
}

inline
int bcema_MultipoolAllocator::maxPooledBlockSize() const
{
    return d_multipool.maxPooledBlockSize();
}

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
inline
int bcema_MultipoolAllocator::maxPooledObjectSize() const
{
    return d_multipool.maxPooledBlockSize();
}
#endif


}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
