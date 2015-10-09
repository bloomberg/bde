// bdlcc_fixedqueueindexmanager.h                                     -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLCC_FIXEDQUEUEINDEXMANAGER
#define INCLUDED_BDLCC_FIXEDQUEUEINDEXMANAGER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide thread-enabled state management for a fixed-size queue.
//
//@CLASSES:
//  bdlcc::FixedQueueIndexManager: state management for a queue
//
//@DESCRIPTION: This component implements a lock-free mechanism for managing
// the indices of a circular buffer of elements to facilitate the
// implementation of a fixed-size thread-enabled single-ended queue.  A
// 'bdlcc::FixedQueueIndexManager' is supplied the size of a circular buffer on
// construction, and provides the methods to reserve indices for enqueing and
// dequeing elements in that buffer.  The actual buffer is held in some other
// (external) data structure managed by the user of this component.
//
// This component is not *itself* a general-purpose queue data structure.  For
// example, no user data of any kind is stored in this data structure (it is
// not a queue of integers), and successful invocation of certain methods
// ('reservePopIndex', 'reservePushIndex') obligates the caller to invoke a
// corresponding method ('commitPopIndex', 'commitPushIndex' respectively);
// otherwise, other threads may "spin" indefinitely with severe performance
// consequences.
//
///Thread Safety
///-------------
// 'bdlcc::FixedQueueIndexManager' is fully *thread-safe*, meaning that all
// non-creator operations on an object can be safely invoked simultaneously
// from multiple threads.
//
///Exception safety
///----------------
// All methods of the 'bdlcc::FixedQueueIndexManager' provide a no-throw
// exception guarantee, except for the constructor, which is exception neutral.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a Thread-Safe Queue of Integers
///- - - - - - - - - - - - - - - - - - - - - - - - - -
// In the following example we create a simple thread-safe queue of integers
// using a 'bdlcc::FixedQueueIndexManager' to synchronize the queue operations.
//
// We start by declaring the data members of an 'IntegerQueue', a vector of
// integers, to hold the values in the queue, and an index manager to ensure
// thread-safe access to the indices of the vector:
//..
//  class IntegerQueue {
//      // This class provides a fully thread-safe queue of integers with a
//      // fixed maximum capacity.
//
//      // DATA
//      bdlcc::FixedQueueIndexManager d_indexManager;  // manages 'd_values'
//                                                     // indices
//
//      bsl::vector<int>              d_values;        // maintains values
//
//    private:
//      // Not implemented:
//      IntegerQueue(const IntegerQueue&);
//
//    public:
//..
// Then, we declare the methods of an integer queue:
//..
//      // CREATORS
//      explicit IntegerQueue(bsl::size_t       capacity,
//                            bslma::Allocator *basicAllocator = 0);
//          // Create a queue capable of holding up to the specified
//          // 'capacity' number of integer values.
//
//      ~IntegerQueue();
//          // Destroy this queue.
//
//      // MANIPULATORS
//      int tryPushBack(int value);
//          // Attempt to push the specified 'value' onto the back of this
//          // queue.  Return 0 on success, and a non-zero value if this queue
//          // is full.
//
//      int tryPopFront(int *result);
//          // Attempt to remove an element from the front of this queue and
//          // load the removed value into the specified 'result'.  Return 0
//          // on success, and a non-zero value otherwise.
//
//      // ACCESSORS
//      bsl::size_t length() const;
//          // Return a snapshot of the number of elements currently in this
//          // queue.
//
//      bsl::size_t capacity() const;
//          // Return the maximum number of elements that this queue can hold.
//  };
//..
//  Next, we define the constructor, which initializes both the index manager
//  and vector with the supplied capacity:
//..
//  // CREATORS
//  IntegerQueue::IntegerQueue(bsl::size_t       capacity,
//                             bslma::Allocator *basicAllocator)
//  : d_indexManager(capacity, basicAllocator)
//  , d_values(capacity, 0, basicAllocator)
//  {
//  }
//
//  IntegerQueue::~IntegerQueue()
//  {
//  }
//..
// Now, we define 'tryPushBack' and 'tryPopFront', which use the index manager
// to reserve an index in the vector, operate on that index, and then commit
// that index back to the index manager:
//..
//  // MANIPULATORS
//  int IntegerQueue::tryPushBack(int value)
//  {
//      unsigned int generation, index;
//      if (0 == d_indexManager.reservePushIndex(&generation, &index)) {
//          d_values[index] = value;
//          d_indexManager.commitPushIndex(generation, index);
//          return 0;                                                 // RETURN
//      }
//      return -1;
//  }
//
//  int IntegerQueue::tryPopFront(int *result)
//  {
//      unsigned int generation, index;
//      if (0 == d_indexManager.reservePopIndex(&generation, &index)) {
//          *result = d_values[index];
//          d_indexManager.commitPopIndex(generation, index);
//          return 0;                                                 // RETURN
//      }
//      return -1;
//  }
//..
// Notice that because none of these operations allocate memory, we do not need
// to add code to ensure exception safety.
//
// Then, we define the accessors to the integer queue:
//..
//  // ACCESSORS
//  bsl::size_t IntegerQueue::length() const
//  {
//      return d_indexManager.length();
//  }
//
//  bsl::size_t IntegerQueue::capacity() const
//  {
//      return d_indexManager.capacity();
//  }
//..
// Finally, we create an 'IntegerQueue', and push and pop a couple of elements
// into the queue:
//..
//  IntegerQueue intQueue(2);
//  int rc = intQueue.tryPushBack(1);
//  assert(0 == rc);
//
//  rc = intQueue.tryPushBack(2);
//  assert(0 == rc);
//
//  rc = intQueue.tryPushBack(3);
//  assert(0 != rc);
//
//  assert(2 == intQueue.length());
//
//  int result;
//
//  rc = intQueue.tryPopFront(&result);
//  assert(0 == rc);
//  assert(1 == result);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSLMT_PLATFORM
#include <bslmt_platform.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMA_USESBSLMAALLOCATOR
#include <bslma_usesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLMF_NESTEDTRAITDECLARATION
#include <bslmf_nestedtraitdeclaration.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_PERFORMANCEHINT
#include <bsls_performancehint.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_CSTDDEF
#include <bsl_cstddef.h>
#endif

#ifndef INCLUDED_BSL_CSTDLIB
#include <bsl_cstdlib.h>
#endif

namespace BloombergLP {
namespace bdlcc {

                        // ============================
                        // class FixedQueueIndexManager
                        // ============================

class FixedQueueIndexManager {
    // This class implements a circular buffer of atomic state variables.
    // These are intended to synchronize access to another (non-atomic) indexed
    // data structure so that the other data structure can be used as a
    // thread-enabled fixed-size queue.

    // PRIVATE CONSTANTS
    enum {
        k_PADDING =
                   bslmt::Platform::e_CACHE_LINE_SIZE - sizeof(bsls::AtomicInt)
    };

    // DATA
    bsls::AtomicInt     d_pushIndex;
                           // index in circular buffer in which the next
                           // element will be pushed (see implementation note
                           // in .cpp)

    const char          d_pushIndexPad[k_PADDING];
                           // padding to prevent false sharing

    bsls::AtomicInt     d_popIndex;
                           // index in the circular buffer from which the next
                           // element will be popped (see implementation note
                           // in .cpp)

    const char          d_popIndexPad[k_PADDING];
                           // padding to prevent false sharing

    const bsl::size_t   d_capacity;
                           // maximum number of elements that can be held in
                           // the circular buffer

    const unsigned int  d_maxGeneration;
                           // maximum generation count for this object (see
                           // implementation note in the .cpp file for more
                           // detail)

    const unsigned int  d_maxCombinedIndex;
                           // maximum combination of index and generation count
                           // that can stored in 'd_pushIndex' and 'd_popIndex'
                           // of this object (see implementation note in the
                           // .cpp file for more detail)

    bsls::AtomicInt    *d_states;
                           // array of index state variables

    bslma::Allocator   *d_allocator_p;
                           // allocator, held not owned

  private:
    // NOT IMPLEMENTED
    FixedQueueIndexManager(const FixedQueueIndexManager&); // = delete;
    FixedQueueIndexManager& operator=(
                                   const FixedQueueIndexManager&); // = delete;

  private:

    // PRIVATE ACCESSORS
    unsigned int nextCombinedIndex(unsigned int combinedIndex) const;
        // Return the combined index value subsequent to the specified
        // 'combinedIndex'.  Note that a "combined index" is the combination of
        // generation count and element index held in 'd_pushIndex' and
        // 'd_popIndex', and is defined as:
        //..
        //  ('generationCount * d_capacity) + index'.
        //..
        // See the implementation note in the .cpp file for more detail.

    unsigned int nextGeneration(unsigned int generation) const;
        // Return the generation subsequent to the specified 'generation'.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(FixedQueueIndexManager,
                                   bslma::UsesBslmaAllocator);

    // CLASS METHODS
    static int circularDifference(unsigned int minuend,
                                  unsigned int subtrahend,
                                  unsigned int modulo);
        // Return the difference between the specified 'minuend' and the
        // specified 'subtrahend' (typically 'minuend - subtrahend') where
        // minuend and subtrahend are both "circular values", meaning they are
        // part of a non-euclidean number line where the value wrap around to 0
        // at the specified 'modulo'.  The difference between two circular
        // values is the minimum of either the number of increments or the
        // number of decrements to 'subtrahend' that results in 'minuend'
        // (i.e., the minimum "distance" between the points on the number
        // circle), where increments are a positive difference, and decrements
        // are a negative difference.  If the number of increments and number
        // of decrements between 'minuend' and 'subtrahend' are equal,
        // 'minuend - subtrahend' is returned.  For example, for a hypothetical
        // compass, [0, 360):
        //..
        //  circularDifference(   0, 359, 360) ==    1
        //  circularDifference( 359,   0, 360) ==   -1
        //  circularDifference( 180,   0, 360) ==  180
        //  circularDifference(   0, 180, 360) == -180
        //..
        // The behavior is undefined unless 'minuend < modulo',
        // 'subtrahend < modulo', and 'modulo <= INT_MAX + 1'.

    static unsigned int numRepresentableGenerations(bsl::size_t capacity);
        // Return the number of representable generations for a circular buffer
        // of the specified 'capacity'.

    // PUBLIC CONSTANTS
    enum {
        k_MAX_CAPACITY = 1 << ((sizeof(int) * 8) - 2)
                                    // maximum capacity of an index manager;
                                    // note that 2 bits of 'd_pushIndex' are
                                    // reserved for holding the disabled status
                                    // flag, and ensuring that the
                                    // representable number of generation
                                    // counts is at least 2 (see the
                                    // implementation note in the .cpp for more
                                    // details)

    };

    // CREATORS
    explicit
    FixedQueueIndexManager(bsl::size_t       capacity,
                           bslma::Allocator *basicAllocator = 0);
        // Create an index manager for a circular buffer having the specified
        // maximum 'capacity'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.  'isEnabled' will be 'true' for the newly
        // created index manager.  The behavior is undefined unless
        // '0 < capacity' and 'capacity <= k_MAX_CAPACITY'.

    ~FixedQueueIndexManager();
        // Destroy this object.

    // MANIPULATORS
                         // Pushing Elements

    int reservePushIndex(unsigned int *generation, unsigned int *index);
        // Reserve the next available index at which to enqueue an element in
        // an (externally managed) circular buffer; load the specified 'index'
        // with the reserved index and load the specified 'generation' with the
        // current generation of the circular buffer.  Return 0 on success, a
        // negative value if the queue is disabled, and a positive value if the
        // queue is full.  If this method succeeds, other threads using this
        // object may spin on the corresponding index state until
        // 'commitPushIndex' is called using the returned 'index' and
        // 'generation' values; clients should call 'commitPushIndex' quickly
        // after this method returns, without performing any blocking
        // operations.  If this method fails the 'generation' and 'index' will
        // be unmodified.  The behavior is undefined if the current thread is
        // already holding a reservation on either a push or pop index.  Note
        // that 'generation' is necessary for invoking 'commitPushIndex' but
        // should not otherwise be used by the caller; the value reflects the
        // number of times the 'index' in the circular buffer has been used.

    void commitPushIndex(unsigned int generation, unsigned int index);
        // Mark the specified 'index' as occupied (full) in the specified
        // 'generation'.  The behavior is undefined unless 'generation' and
        // 'index' match those returned by a previous successful call to
        // 'reservePushIndex' (that has not previously been committed).

                         // Popping Elements

    int reservePopIndex(unsigned int *generation, unsigned int *index);
        // Reserve the next available index from which to dequeue an element
        // from an (externally managed) circular buffer; load the specified
        // 'index' with the reserved index and load the specified 'generation'
        // with the current generation of the circular buffer.  Return 0 on
        // success, and a non-zero value if the queue is empty.  If this method
        // succeeds, other threads using this object may spin on the
        // corresponding index state until 'commitPopIndex' is called using the
        // returned 'index' and 'generation' values; clients should call
        // 'commitPopIndex' quickly after this method returns, without
        // performing any blocking operations.  If this method fails the
        // 'generation' and 'index' will be unmodified.  The behavior is
        // undefined if the current thread is already holding a reservation on
        // either a push or pop index.  Note that 'generation' is necessary for
        // invoking 'commitPopIndex' but should not otherwise be used by the
        // caller; the value reflects the of times the 'index' in the circular
        // buffer has been used.

    void commitPopIndex(unsigned int generation, unsigned int index);
        // Mark the specified 'index' as available (empty) in the generation
        // following the specified 'generation'.  The behavior is undefined
        // unless 'generation' and index' match those returned by a previous
        // successful call to 'reservePopIndex' (that has not previously been
        // committed).

                                // Disabled State

    void disable();
        // Mark the queue as disabled.  Future calls to 'reservePushIndex' will
        // fail.

    void enable();
        // Mark the queue as enabled.

                               // Exception Safety

    int reservePopIndexForClear(unsigned int *disposedGeneration,
                                unsigned int *disposedIndex,
                                unsigned int  endGeneration,
                                unsigned int  endIndex);
        // If the next available index from which an element can be popped is
        // before the specified 'endGeneration' and 'endIndex' then reserve
        // that index for popping and load the specified 'disposedGeneration'
        // and 'disposedIndex' with the generation and index of the reserved
        // cell; otherwise this operation has no effect.  Return 0 if an index
        // was successfully reserved, and a non-zero value if the current pop
        // index is at 'endIndex' and 'endGeneration'.  The behavior is
        // undefined unless 'endGeneration' and 'endIndex' refer to a cell that
        // has been acquired for writing.  Note that this operation is used to
        // facilitate removing all the elements in a circular buffer if an
        // exception is thrown between reserving an index for pushing, and
        // committing that index -- the intended usage is to call
        // 'reservePopIndexForClear' and then 'commitPopIndex', emptying all
        // the cells up to the index that was reserved for writing, and then
        // call 'abortPushIndexReservation' on the reserved index.

    void abortPushIndexReservation(unsigned int generation,
                                   unsigned int index);
        // Release the specified 'index' and make it available for use in the
        // generation following the specified 'generation'.  The behavior is
        // undefined unless the calling thread holds a reservation on
        // 'generation' and 'index', and 'clearPopIndex' and then
        // 'commitPushIndex' have been repeatedly invoked with 'generation' and
        // 'index' as input until no indices remain to clear.  Note that this
        // operation is used to facilitate removing all the elements in a
        // circular buffer if an exception is thrown between reserving an index
        // for pushing, and committing that index.

    // ACCESSORS
    bool isEnabled() const;
        // Return 'true' if the queue is enabled, and 'false' if it is
        // disabled.

    bsl::size_t length() const;
        // Return a snapshot of the number of items in the queue.

    bsl::size_t capacity() const;
        // Return the maximum number of items that may be stored in the queue.

    bsl::ostream& print(bsl::ostream& stream) const;
        // Print a formatted string describing the current state of this object
        // to the specified 'stream'.  If 'stream' is not valid on entry, this
        // operation has no effect.  Note that this method describes the
        // internal state of the buffer and is provided purely for debugging
        // purposes.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // ----------------------------
                        // class FixedQueueIndexManager
                        // ----------------------------

// PRIVATE ACCESSORS
inline
unsigned int FixedQueueIndexManager::nextCombinedIndex(
                                              unsigned int combinedIndex) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_maxCombinedIndex ==
                                              combinedIndex)) {
        // We have reached the maximum representable combination of index and
        // generation count, so we reset the generation count to 0.

        return 0;                                                     // RETURN
    }

    return combinedIndex + 1;

}

inline
unsigned int FixedQueueIndexManager::nextGeneration(
                                                 unsigned int generation) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(d_maxGeneration == generation)) {
        return 0;                                                     // RETURN
    }
    return generation + 1;
}

// ACCESSORS
inline
bsl::size_t FixedQueueIndexManager::capacity() const
{
    return d_capacity;
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
