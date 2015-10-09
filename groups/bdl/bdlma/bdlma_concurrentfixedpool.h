// bdlma_concurrentfixedpool.h                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLMA_CONCURRENTFIXEDPOOL
#define INCLUDED_BDLMA_CONCURRENTFIXEDPOOL

//@PURPOSE: Provide thread-safe pool of limited # of blocks of uniform size.
//
//@CLASSES:
//  bdlma::ConcurrentFixedPool: thread-safe pool of limited number of blocks
//
//@SEE_ALSO: bdlma_concurrentpool
//
//@DESCRIPTION: This component implements a *fully thread-safe* memory pool
// that allocates and manages a limited number (specified at construction) of
// memory blocks of some uniform size (also specified at construction).  A
// 'bdlma::ConcurrentFixedPool' constructed to manage up to 'N' blocks also
// provides an association between the address of each block and an index in
// the range '[ 0 .. N - 1 ]'.
//
// Other than this mapping between block and index, and the associated limit on
// the maximum number of blocks that may be simultaneously allocated, this
// component's semantics are identical to 'bdlma::ConcurrentPool'.  In
// particular, this component overloads global operator 'new' in the same
// manner, and the behaviors of 'release' and 'reserveCapacity' are equivalent
// to the corresponding methods in 'bdlma::ConcurrentPool'.
//
// Like 'bdlma::ConcurrentPool', this component is intended to be used to
// implement *out-of-place* container classes that hold elements of uniform
// size.
//
///Usage
///-----
// 'bdlma::ConcurrentFixedPool' is intended to implement *out-of-place*
// container classes that hold up to a fixed number of elements, all of uniform
// size.  Suppose we wish to implement a simple thread pool.  We want the
// equivalent of a 'bsl::deque<bsl::function<void(void)> >'.  However, to
// minimize the time spent performing operations on this deque - which must be
// carried out under a lock - we instead store just pointers in the deque, and
// manage memory efficiently using 'bdlma::ConcurrentFixedPool'.
// 'bdlma::ConcurrentFixedPool' is fully thread-safe and does not require any
// additional synchronization.
//
// The example below is just for the container portion of our simple thread
// pool.  The implementation of the worker thread, and the requisite
// synchronization, are omitted for clarity.
//..
//  class my_JobQueue {
//
//    public:
//      // PUBLIC TYPES
//      typedef bsl::function<void(void)> Job;
//
//    private:
//      // DATA
//      bslmt::Mutex                d_lock;
//      bsl::deque<Job *>           d_queue;
//      bdlma::ConcurrentFixedPool  d_pool;
//      bslma::Allocator           *d_allocator_p;
//
//      // Not implemented:
//      my_JobQueue(const my_JobQueue&);
//
//    public:
//      // CREATORS
//      my_JobQueue(int maxJobs, bslma::Allocator *basicAllocator = 0);
//      ~my_JobQueue();
//
//      // MANIPULATORS
//      void enqueueJob(const Job& job);
//
//      int tryExecuteJob();
//  };
//
//  my_JobQueue::my_JobQueue(int maxJobs, bslma::Allocator *basicAllocator)
//  : d_queue(basicAllocator)
//  , d_pool(sizeof(Job), maxJobs, basicAllocator)
//  , d_allocator_p(bslma::Default::allocator(basicAllocator))
//  {
//  }
//
//  my_JobQueue::~my_JobQueue()
//  {
//      Job *jobPtr;
//      while (!d_queue.empty()) {
//          jobPtr = d_queue.front();
//          jobPtr->~Job();
//          d_queue.pop_front();
//      }
//  }
//
//  void my_JobQueue::enqueueJob(const Job& job)
//  {
//      Job *jobPtr = new (d_pool) Job(job, d_allocator_p);
//      d_lock.lock();
//      d_queue.push_back(jobPtr);
//      d_lock.unlock();
//  }
//
//  int my_JobQueue::tryExecuteJob()
//  {
//      d_lock.lock();
//      if (d_queue.empty()) {
//          d_lock.unlock();
//          return -1;                                                // RETURN
//      }
//      Job *jobPtr = d_queue.front();
//      d_queue.pop_front();
//      d_lock.unlock();
//      (*jobPtr)();
//      d_pool.deleteObject(jobPtr);
//      return 0;
//  }
//..
// Note that in the destructor, there is no need to deallocate the individual
// job objects - the destructor of 'bdlma::ConcurrentFixedPool' will release
// any remaining allocated memory.  However, it *is* necessary to invoke the
// destructors of all these objects, as the destructor of
// 'bdlma::ConcurrentFixedPool' will not do so.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BDLMA_POOL
#include <bdlma_pool.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_DELETERHELPER
#include <bslma_deleterhelper.h>
#endif

#ifndef INCLUDED_BSLS_ALIGNMENTUTIL
#include <bsls_alignmentutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

namespace BloombergLP {
namespace bdlma {

                     // ===============================
                     // struct ConcurrentFixedPool_Node
                     // ===============================

struct ConcurrentFixedPool_Node {
    // The component-private 'struct' provides a header for blocks that are
    // allocated from 'ConcurrentFixedPool' objects.

    // DATA
    unsigned d_next;  // index of next free node when on free list; otherwise,
                      // index of this node itself adjusted with a generation
                      // count
};

                        // =========================
                        // class ConcurrentFixedPool
                        // =========================

class ConcurrentFixedPool {
    // This class implements a memory pool that allocates and manages up to a
    // fixed number of memory blocks of some uniform size, with both the limit
    // on the number of blocks and the block size specified at construction.
    //
    // This class guarantees thread safety when allocating or releasing memory
    // (but see the documentation for the 'release' method).

    // PRIVATE TYPES
    typedef ConcurrentFixedPool_Node Node;  // type of memory block "header"

    // DATA
    bsls::AtomicInt      d_freeList;        // head of free list

    const unsigned       d_sizeMask;        // mask corresponding to max size
                                            // of pool; rounded up to power of
                                            // 2

    bsl::vector<Node *>  d_nodes;           // holds nodes currently being
                                            // pooled; enables index <->
                                            // address mapping

    const int            d_dataOffset;      // offset (in bytes) to memory
                                            // block within a 'Node'

    const int            d_nodeSize;        // size of blocks pooled by
                                            // 'd_nodePool'

    bslmt::Mutex         d_nodePoolMutex;   // mutex for access to 'd_nodePool'

    bdlma::Pool          d_nodePool;        // underlying memory pool

    int                  d_numNodes;        // number of nodes in 'd_nodes'
                                            // that are currently being pooled

    const int            d_objectSize;      // size of pooled objects as
                                            // specified at construction

    int                  d_backoffLevel;    // determines amount of spinning
                                            // when under contention

    bslma::Allocator    *d_allocator_p;     // memory allocator (held, not
                                            // owned)

    // NOT IMPLEMENTED
    ConcurrentFixedPool(const ConcurrentFixedPool&);
    ConcurrentFixedPool& operator=(const ConcurrentFixedPool&);

  private:
    // PRIVATE MANIPULATORS
    void *allocateNew();
        // Allocate a memory block of the 'objectSize' specified at
        // construction from the underlying pool from which this fixed pool
        // obtains memory.  Return the address of that block or 0 if this pool
        // is exhausted (i.e., 'poolSize()' memory blocks have already been
        // allocated from this pool).

  public:
    // CREATORS
    ConcurrentFixedPool(int               objectSize,
                        int               poolSize,
                        bslma::Allocator *basicAllocator = 0);
        // Create a memory pool that returns memory of the specified
        // 'objectSize' for each invocation of the 'allocate' method.
        // Configure this pool to support allocation of up to the specified
        // 'poolSize' number of memory blocks.  The largest supported
        // 'poolSize' is 33554431.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  The behavior is undefined unless
        // '0 < objectSize', '0 < poolSize', and '0x1FFFFFF >= poolSize'.

    ~ConcurrentFixedPool();
        // Destroy this object and release all associated memory.

    // MANIPULATORS
    void *allocate();
        // Allocate a memory block of the 'objectSize' specified at
        // construction.  Return the address of that block or 0 if the pool is
        // exhausted (i.e., 'poolSize()' memory blocks have already been
        // allocated from this pool).

    void deallocate(void *address);
        // Deallocate the memory block at the specified 'address' back to this
        // pool for reuse.

    template<class TYPE>
    void deleteObject(const TYPE *object);
        // Destroy the specified 'object' based on its dynamic type and then
        // use this allocator to deallocate its memory footprint.  Do nothing
        // if 'object' is 0.  The behavior is undefined unless 'object', when
        // cast appropriately to 'void *', was allocated using this allocator
        // and has not already been deallocated.  Note that
        // 'dynamic_cast<void *>(object)' is applied if 'TYPE' is polymorphic,
        // and 'static_cast<void *>(object)' is applied otherwise.

    template <class TYPE>
    void deleteObjectRaw(const TYPE *object);
        // Destroy the specified 'object' based on its static type and then use
        // this allocator to deallocate its memory footprint.  Do nothing if
        // 'object' is 0.  The behavior is undefined if 'object' is a
        // base-class pointer to a derived type, was not allocated using this
        // allocator, or has already been deallocated.

    void release();
        // Release all memory currently allocated through this object.  Note
        // that this method should only be invoked when it is known that no
        // blocks currently allocated through this pool will be used;
        // therefore, it is not safe to use this method if any other thread may
        // be concurrently allocating memory from this pool.  Also note that
        // 'release()' is intended to free all memory without regard to the
        // contents of that memory.  Specifically, 'release()' can *not* call
        // object destructors for any allocated objects, since it has no
        // knowledge of their type.  If object destruction is required, use
        // 'ConcurrentFixedPool::deleteObject()'.

    int reserveCapacity(int numObjects);
        // Reserve memory from this pool to satisfy memory requests for at
        // least the specified 'numObjects' before the pool replenishes.  The
        // behavior is undefined unless '0 <= numObjects'.  Return 0 on success
        // and the number of objects that could not be reserved otherwise.
        // Note that this method fails if the number of memory blocks already
        // allocated plus 'numObjects' exceeds 'poolSize()'.

    void setBackoffLevel(int backoffLevel);
        // Configure this pool with the specified non-negative 'backoffLevel'
        // that controls the amount of spinning that occurs when calls to this
        // pool encounter contention.  Setting 'backoffLevel' to 0 disables
        // spinning.  Greater values of 'backoffLevel' correspond to greater
        // amounts of spinning.  The behavior is undefined unless
        // '0 <= backoffLevel'.  Note that both contention detection and
        // spinning strategy are implementation defined.

    // ACCESSORS
    int backoffLevel() const;
        // Return the non-negative 'backoffLevel' that controls the amount of
        // spinning that occurs when calls to this pool encounter contention.

    int indexFromAddress(void *address) const;
        // Return an index in the range from 0 to the maximum size of this pool
        // that uniquely identifies the memory block at the specified
        // 'address'.  The behavior is undefined unless 'address' corresponds
        // to a memory block allocated from this pool.

    int objectSize() const;
        // Return the size of the memory blocks allocated from this object.
        // Note that all blocks have the same size.

    void *addressFromIndex(int index) const;
        // Return the address of the memory block identified by the specified
        // 'index'.  The behavior is undefined unless the index has been
        // obtained through 'indexFromAddress'.

    int poolSize() const;
        // Return the maximum size of this pool.
};

}  // close package namespace
}  // close enterprise namespace

// FREE OPERATORS
inline
void *operator new(bsl::size_t                              size,
                   BloombergLP::bdlma::ConcurrentFixedPool& pool);
    // Allocate memory of the specified 'size' bytes from the specified 'pool',
    // and return the address of the allocated memory.  The behavior is
    // undefined unless 'size' is the same as the 'objectSize' with which
    // 'pool' was constructed.  Note that an object may allocate additional
    // memory internally within its constructor, requiring the allocator to be
    // passed in as a constructor argument:
    //..
    //  my_Type *newMyType(bdlma::ConcurrentFixedPool  *pool,
    //                     bslma::Allocator *basicAllocator) {
    //      return new (*pool) my_Type(..., basicAllocator);
    //  }
    //..
    // Note also that the analogous version of operator 'delete' should not be
    // called directly.  Instead, this component provides a template member
    // function 'bdlma::ConcurrentFixedPool::deleteObject' parameterized by
    // 'TYPE' that performs the equivalent of the following:
    //..
    //  void deleteMyType(bdlma::ConcurrentFixedPool *pool, my_Type *t) {
    //      t->~my_Type();
    //      pool->deallocate(t);
    //  }
    //..

inline
void operator delete(void                                     *address,
                     BloombergLP::bdlma::ConcurrentFixedPool&  pool);
    // Use the specified 'pool' to deallocate the memory at the specified
    // 'address'.  The behavior is undefined unless 'address' was allocated
    // using 'pool' and has not already been deallocated.  This operator is
    // supplied solely to allow the compiler to arrange for it to be called in
    // case of an exception.  Client code should not call it; use
    // 'bdlma::ConcurrentFixedPool::deleteObject()' instead.

namespace BloombergLP {
namespace bdlma {

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class ConcurrentFixedPool
                        // -------------------------

// MANIPULATORS
template<class TYPE>
inline
void ConcurrentFixedPool::deleteObject(const TYPE *object)
{
    bslma::DeleterHelper::deleteObject(object, this);
}

template<class TYPE>
inline
void ConcurrentFixedPool::deleteObjectRaw(const TYPE *object)
{
    bslma::DeleterHelper::deleteObjectRaw(object, this);
}

inline
void ConcurrentFixedPool::setBackoffLevel(int backoffLevel)
{
    d_backoffLevel = backoffLevel;
}

// ACCESSORS
inline
void *ConcurrentFixedPool::addressFromIndex(int index) const
{
    Node * node = const_cast<Node *>(d_nodes[index]);

    BSLS_ASSERT(node);
    return (char *)node + d_dataOffset;
}

inline
int ConcurrentFixedPool::backoffLevel() const
{
    return d_backoffLevel;
}

inline
int ConcurrentFixedPool::indexFromAddress(void *address) const
{
    const Node * const node = (const Node *)(void *)
                                              ((char *)address - d_dataOffset);
    return (node->d_next & d_sizeMask) - 1;
}

inline
int ConcurrentFixedPool::objectSize() const
{
    return d_objectSize;
}

inline
int ConcurrentFixedPool::poolSize() const
{
    return static_cast<int>(d_nodes.size());
}

}  // close package namespace
}  // close enterprise namespace

inline
void *operator new(bsl::size_t                              size,
                   BloombergLP::bdlma::ConcurrentFixedPool& pool)
{
    using namespace BloombergLP;
    BSLS_ASSERT((int) size <= pool.objectSize()
        && bsls::AlignmentUtil::calculateAlignmentFromSize((int)size)
        <= bsls::AlignmentUtil::calculateAlignmentFromSize(pool.objectSize()));

    (void)size;  // suppress "unused parameter" warnings
    return pool.allocate();
}

inline
void operator delete(void                                     *address,
                     BloombergLP::bdlma::ConcurrentFixedPool&  pool)
{
    pool.deallocate(address);
}

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
