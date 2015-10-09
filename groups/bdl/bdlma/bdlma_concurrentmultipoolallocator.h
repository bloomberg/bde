// bdlma_concurrentmultipoolallocator.h                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMA_CONCURRENTMULTIPOOLALLOCATOR
#define INCLUDED_BDLMA_CONCURRENTMULTIPOOLALLOCATOR

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an allocator to manage pools of varying object sizes.
//
//@CLASSES:
//   bdlma::ConcurrentMultipoolAllocator: allocator managing varying size pools
//
//@SEE_ALSO: bdlma_concurrentpool, bdlma_concurrentmultipool
//
//@DESCRIPTION: This component provides an allocator,
// 'bdlma::ConcurrentMultipoolAllocator', that implements the
// 'bdlma::ManagedAllocator' protocol and provides an allocator that maintains
// a configurable number of 'bdlma::ConcurrentPool' objects, each dispensing
// memory blocks of a unique size.  The 'bdlma::ConcurrentPool' objects are
// placed in an array, starting at index 0, with each successive pool managing
// memory blocks of a size twice that of the previous pool.  Each multipool
// allocation (deallocation) request allocates memory from (returns memory to)
// the internal pool managing memory blocks of the smallest size not less than
// the requested size, or else from a separately managed list of memory blocks,
// if no internal pool managing memory block of sufficient size exists.  Both
// the 'release' method and the destructor of a
// 'bdlma::ConcurrentMultipoolAllocator' release all memory currently allocated
// via the object.
//..
//   ,------------------------.
//  ( bcema::MultiPoolAllocator )
//   `------------------------'
//               |         ctor/dtor
//               |         maxPooledBlockSize
//               |         numPools
//               |         reserveCapacity
//               V
//    ,----------------------.
//   ( bdlma::ManagedAllocator )
//    `----------------------'
//               |         release
//               V
//       ,-----------------.
//      (  bslma::Allocator )
//       `-----------------'
//                        allocate
//                        deallocate
//..
// The main difference between a 'bdlma::ConcurrentMultipoolAllocator' and a
// 'bdlma::ConcurrentMultipool' is that, very often, a
// 'bdlma::ConcurrentMultipoolAllocator' is managed through a
// 'bslma::Allocator' pointer.  Hence, every call to the 'allocate' method
// invokes a virtual function call, which is slower than invoking the
// non-virtual 'allocate' method on a 'bdlma::ConcurrentMultipool'.  However,
// since 'bslma::Allocator *' is widely used across BDE interfaces,
// 'bdlma::ConcurrentMultipoolAllocator' is more general purposed than a
// 'bdlma::ConcurrentMultipool'.
//
///Configuration at Construction
///-----------------------------
// When creating a 'bdlma::ConcurrentMultipoolAllocator', clients can
// optionally configure:
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
//:     o the unique maximum-blocks-per-chunk value for all of the pools, or
//:     o an array of maximum-blocks-per-chunk values corresponding to each
//:       individual pool.
//:   If the maximum blocks per chunk is not specified, an
//:   implementation-defined default value is used.  Note that the maximum
//:   blocks per chunk can be configured only if the number of pools is also
//:   configured.
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
// 'bdlma::ConcurrentMultipoolAllocator' was created.
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
///Example 1: Using a 'bdlma::ConcurrentMultipoolAllocator'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlma::ConcurrentMultipoolAllocator' can be used to supply memory to
// node-based data structures such as 'bsl::set', 'bsl::list' or 'bsl::map'.
// Suppose we are implementing a container of named graphs data structure,
// where a graph is defined by a set of edges and nodes.  The various
// fixed-sized nodes can be efficiently allocated by a
// 'bdlma::ConcurrentMultipoolAllocator'.
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
//  enum { k_NUM_POOLS = 3 };
//
//  bdlma::ConcurrentMultipoolAllocator basicAllocator(k_NUM_POOLS);
//
//  my_NamedGraphContainer container(&basicAllocator);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLMA_CONCURRENTMULTIPOOL
#include <bdlma_concurrentmultipool.h>
#endif

#ifndef INCLUDED_BDLMA_MANAGEDALLOCATOR
#include <bdlma_managedallocator.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

namespace BloombergLP {
namespace bdlma {

                    // ==================================
                    // class ConcurrentMultipoolAllocator
                    // ==================================

class ConcurrentMultipoolAllocator : public bdlma::ManagedAllocator {
    // This class implements the 'bdlma::ManagedAllocator' protocol to provide
    // a thread-safe allocator that maintains a configurable number of 'Pool'
    // objects, each dispensing memory blocks of a unique size.  The 'Pool'
    // objects are placed in an array, with each successive pool managing
    // memory blocks of size twice that of the previous pool.  Each multipool
    // allocation (deallocation) request allocates memory from (returns memory
    // to) the internal pool having the smallest block size not less than the
    // requested size, or, if no pool manages memory blocks of sufficient
    // sized, from a separately managed list of memory blocks.  Both the
    // 'release' method and the destructor of a 'ConcurrentMultipoolAllocator'
    // release all memory currently allocated via the object.

    // DATA
    ConcurrentMultipool d_multipool;  // owned allocator

  private:
    // NOT IMPLEMENTED
    ConcurrentMultipoolAllocator(const ConcurrentMultipoolAllocator&);
    ConcurrentMultipoolAllocator& operator=
                                         (const ConcurrentMultipoolAllocator&);

  public:
    // CREATORS
    ConcurrentMultipoolAllocator(bslma::Allocator *basicAllocator = 0);
    ConcurrentMultipoolAllocator(int               numPools,
                                 bslma::Allocator *basicAllocator = 0);
    ConcurrentMultipoolAllocator(
                              bsls::BlockGrowth::Strategy  growthStrategy,
                              bslma::Allocator            *basicAllocator = 0);
    ConcurrentMultipoolAllocator(
                              int                          numPools,
                              bsls::BlockGrowth::Strategy  growthStrategy,
                              bslma::Allocator            *basicAllocator = 0);
    ConcurrentMultipoolAllocator(
                              int                          numPools,
                              bsls::BlockGrowth::Strategy  growthStrategy,
                              int                          maxBlocksPerChunk,
                              bslma::Allocator            *basicAllocator = 0);
        // Create a multipool allocator.  Optionally specify 'numPools',
        // indicating the number of internally created 'Pool' objects; the
        // block size of the first pool is 8 bytes, with the block size of each
        // additional pool successively doubling.  If 'numPools' is not
        // specified, an implementation-defined number of pools 'N' -- covering
        // memory blocks ranging in size from '2^3 = 8' to '2^(N+2)' -- are
        // created.  Optionally specify a 'growthStrategy' indicating whether
        // the number of blocks allocated at once for every internally created
        // 'Pool' should be either fixed or grow geometrically, starting with
        // 1.  If 'growthStrategy' is not specified, the allocation strategy
        // for each internally created 'Pool' object is geometric, starting
        // from 1.  If 'numPools' is specified, optionally specify a
        // 'maxBlocksPerChunk', indicating the maximum number of blocks to be
        // allocated at once when a pool must be replenished.  If
        // 'maxBlocksPerChunk' is not specified, an implementation-defined
        // value is used.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.  Memory allocation (and deallocation) requests
        // will be satisfied using the internally maintained pool managing
        // memory blocks of the smallest size not less than the requested size,
        // or directly from the underlying allocator (supplied at
        // construction), if no internally pool managing memory block of
        // sufficient size exists.  The behavior is undefined unless
        // '1 <= numPools' and '1 <= maxBlocksPerChunk'.  Note that, on
        // platforms where '8 < bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT',
        // excess memory may be allocated for pools managing smaller blocks.
        // Also note that 'maxBlocksPerChunk' need not be an integral power of
        // 2; if geometric growth would exceed the maximum value, the chunk
        // size is capped at that value).

    ConcurrentMultipoolAllocator(
                        int                                numPools,
                        const bsls::BlockGrowth::Strategy *growthStrategyArray,
                        bslma::Allocator                  *basicAllocator = 0);
    ConcurrentMultipoolAllocator(
                        int                                numPools,
                        const bsls::BlockGrowth::Strategy *growthStrategyArray,
                        int                                maxBlocksPerChunk,
                        bslma::Allocator                  *basicAllocator = 0);
    ConcurrentMultipoolAllocator(
                           int                          numPools,
                           bsls::BlockGrowth::Strategy  growthStrategy,
                           const int                   *maxBlocksPerChunkArray,
                           bslma::Allocator            *basicAllocator = 0);
    ConcurrentMultipoolAllocator(
                     int                                numPools,
                     const bsls::BlockGrowth::Strategy *growthStrategyArray,
                     const int                         *maxBlocksPerChunkArray,
                     bslma::Allocator                  *basicAllocator = 0);
        // Create a multipool allocator having the specified 'numPools',
        // indicating the number of internally created 'Pool' objects; the
        // block size of the first pool is 8 bytes, with the block size of each
        // additional pool successively doubling.  Optionally specify a
        // 'growthStrategy' indicating whether the number of blocks allocated
        // at once for every internally created 'Pool' should be either fixed
        // or grow geometrically, starting with 1.  If 'growthStrategy' is not
        // specified, optionally specify 'growthStrategyArray', indicating the
        // strategies for each individual 'Pool' created by this object.  If
        // neither 'growthStrategy' nor 'growthStrategyArray' are specified,
        // the allocation strategy for each internally created 'Pool' object
        // will grow geometrically, starting from 1.  Optionally specify a
        // 'maxBlocksPerChunk', indicating the maximum number of blocks to be
        // allocated at once when a pool must be replenished.  If
        // 'maxBlocksPerChunk' is not specified, optionally specify
        // 'maxBlocksPerChunkArray', indicating the maximum number of blocks to
        // allocate at once for each individually created 'Pool' object.  If
        // neither 'maxBlocksPerChunk' nor 'maxBlocksPerChunkArray' are
        // specified, an implementation-defined value is used.  Optionally
        // specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  Memory allocation (and deallocation) requests will be
        // satisfied using the internally maintained pool managing memory
        // blocks of the smallest size not less than the requested size, or
        // directly from the underlying allocator (supplied at construction),
        // if no internally pool managing memory block of sufficient size
        // exists.  The behavior is undefined unless '1 <= numPools',
        // 'growthStrategyArray' has at least 'numPools' strategies,
        // '1 <= maxBlocksPerChunk' and 'maxBlocksPerChunkArray' have at least
        // 'numPools' positive values.  Note that, on platforms where
        // '8 < bsls::AlignmentUtil::BSLS_MAX_ALIGNMENT', excess memory may be
        // allocated for pools managing smaller blocks.  Also note that the
        // maximum need not be an integral power of 2; if geometric growth
        // would exceed a maximum value, the chunk size is capped at that
        // value).

    virtual ~ConcurrentMultipoolAllocator();
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
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                    // ----------------------------------
                    // class ConcurrentMultipoolAllocator
                    // ----------------------------------

// CREATORS
inline
ConcurrentMultipoolAllocator::ConcurrentMultipoolAllocator(
                  bslma::Allocator                 *basicAllocator)
: d_multipool(basicAllocator)
{
}

inline
ConcurrentMultipoolAllocator::ConcurrentMultipoolAllocator(
                  int                               numPools,
                  bslma::Allocator                 *basicAllocator)
: d_multipool(numPools, basicAllocator)
{
}

inline
ConcurrentMultipoolAllocator::ConcurrentMultipoolAllocator(
                  bsls::BlockGrowth::Strategy       growthStrategy,
                  bslma::Allocator                 *basicAllocator)
: d_multipool(growthStrategy, basicAllocator)
{
}

inline
ConcurrentMultipoolAllocator::ConcurrentMultipoolAllocator(
                  int                               numPools,
                  bsls::BlockGrowth::Strategy       growthStrategy,
                  bslma::Allocator                 *basicAllocator)
: d_multipool(numPools, growthStrategy, basicAllocator)
{
}

inline
ConcurrentMultipoolAllocator::ConcurrentMultipoolAllocator(
                  int                                numPools,
                  const bsls::BlockGrowth::Strategy *growthStrategyArray,
                  bslma::Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategyArray, basicAllocator)
{
}

inline
ConcurrentMultipoolAllocator::ConcurrentMultipoolAllocator(
                  int                               numPools,
                  bsls::BlockGrowth::Strategy       growthStrategy,
                  int                               maxBlocksPerChunk,
                  bslma::Allocator                 *basicAllocator)
: d_multipool(numPools, growthStrategy, maxBlocksPerChunk, basicAllocator)
{
}

inline
ConcurrentMultipoolAllocator::ConcurrentMultipoolAllocator(
                  int                                numPools,
                  const bsls::BlockGrowth::Strategy *growthStrategyArray,
                  int                                maxBlocksPerChunk,
                  bslma::Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategyArray, maxBlocksPerChunk, basicAllocator)
{
}

inline
ConcurrentMultipoolAllocator::ConcurrentMultipoolAllocator(
                  int                                numPools,
                  bsls::BlockGrowth::Strategy        growthStrategy,
                  const int                         *maxBlocksPerChunkArray,
                  bslma::Allocator                  *basicAllocator)
: d_multipool(numPools, growthStrategy, maxBlocksPerChunkArray, basicAllocator)
{
}

inline
ConcurrentMultipoolAllocator::ConcurrentMultipoolAllocator(
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

// ACCESSORS
inline
int ConcurrentMultipoolAllocator::numPools() const
{
    return d_multipool.numPools();
}

inline
int ConcurrentMultipoolAllocator::maxPooledBlockSize() const
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
