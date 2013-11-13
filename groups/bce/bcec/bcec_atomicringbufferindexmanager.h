// bcec_atomicringbufferindexmanager.h                               -*-C++-*-
#ifndef INCLUDED_ATOMICRINGBUFFERINDEXMANAGER
#define INCLUDED_ATOMICRINGBUFFERINDEXMANAGER

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide thread-enabled state management for a fixed-size queue.
//
//@CLASSES:
//  bcec_AtomicRingBufferIndexManager: state management for a queue
//
//@AUTHOR: Eric Winseman (ewinseman), Dave Schumann (dschuman)
//
//@DESCRIPTION: This component implements a lock-free mechanism for
// managing the indices of a circular buffer of elements to facilitate
// the implementation of a fixed-size thread-enabled single-ended queue.
// A 'bcec_AtomicRingBufferIndexManager' is supplied the size of an 
// circular buffer on construction, and provides the methods to reserve
// indices for enqueing and dequeing elements in that buffer.  The actual
// buffer is held in some other (external) data structure managed by the user
// of this component.  
// 
// This component is not *itself* a general-purpose queue data structure. For 
// example, no user data of any kind is stored in this data structure (it is 
// not a queue of integers), and successful invocation of certain methods
// ('acquirePopIndex', 'acquirePushIndex') obligates the caller to invoke a
// corresponding method ('releasePopReservation', 'releasePushReservation' 
// respectively); otherwise, other threads may "spin" indefinitely with 
// severe performance consequences.  
// 
///Exception safety
///----------------
// AtomicRingBufferIndexManager is exception-neutral. No method of the type
// may throw, except the constructor, which may throw only on memory 
// allocation failure. 

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
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

namespace BloombergLP {

class bcec_AtomicRingBufferIndexManager {
    // This class implements a ring buffer of atomic state variables.
    // These are intended to synchronize access to another (non-atomic) 
    // indexed data structure so that the other data structure can be used
    // as a thread-enabled producer/consumer queue.

    // PRIVATE TYPES
    enum {
        e_INDEX_STATE_EMPTY    = 0,   // cell is free (available for write)
        e_INDEX_STATE_WRITING  = 1,   // cell is reserved for write
        e_INDEX_STATE_FULL     = 2,   // cell has a value in it
        e_INDEX_STATE_READING  = 3,   // cell is reserved for read
        e_INDEX_STATE_MASK     = 0x3, // bitmap mask for state values, must be
                               // roundUpToBinaryPower(<max state value>) - 1
        e_INDEX_STATE_SHIFT    = 0x2, // left-shift to make room for state, 
                               // must be base2Log(INDEX_STATE_MASK)
        e_MAX_OP_INDEX = (1 << (8 * sizeof(int) - e_INDEX_STATE_SHIFT)) - 1,
                               // max index value given reserved bits
        e_DISABLED_STATE_MASK  = e_MAX_OP_INDEX + 1,
                               // bit that indicates queue is disabled
    };

    // DATA
    bsls::AtomicInt                 d_pushIndex;
    const char                      d_pushIndexPad[bces_Platform::e_BCEC_PAD]; 
    bsls::AtomicInt                 d_popIndex;
    const char                      d_popIndexPad[bces_Platform::e_BCEC_PAD];

    const bsl::size_t               d_capacity;  
                                              // max elements in queue
    const bsl::size_t               d_maxGeneration; 
                                              // highest possible generation
    const bsl::size_t               d_rolloverIndex; 
                                              // if maxGeneration reached
    bsls::AtomicInt                *d_states; // array of state variables
    bslma::Allocator               *d_allocator_p;

    // PRIVATE CLASS METHODS
    static bsl::size_t incrementIndex(bsl::size_t opCount, 
                                      bsl::size_t currentIndex);
        // If the specified 'opCount' is less than the maximum allowed value,
        // return 'opCount + 1'; otherwise, i.e., the opCount has rolled over
        // the maximum value, return 'currentIndex + 1'. 

    // NOT IMPLEMENTED
    bcec_AtomicRingBufferIndexManager(
                                  const bcec_AtomicRingBufferIndexManager&);
    bcec_AtomicRingBufferIndexManager& operator=(
                                  const bcec_AtomicRingBufferIndexManager&);
    
 public:
    // CREATORS
    explicit
    bcec_AtomicRingBufferIndexManager(bsl::size_t       capacity,
                                      bslma::Allocator *basicAllocator = 0);
       // Create a ring buffer of atomic state variables having the specified
       // maximum 'capacity' using the specified 'basicAllocator' to 
       // supply memory.  The buffer is initially in the "enabled" state. 
       // If 'basicAllocator' is 0, the currently installed default
       // allocator is used.

    ~bcec_AtomicRingBufferIndexManager();
       // Destroy this object.

    // MANIPULATORS
    int acquirePushIndex(bsl::size_t *generationCount, 
                         bsl::size_t *index);
       // Reserve the next available index at which to enqueue an element
       // in an (externally managed) circular buffer; load the specified
       // 'index' with the reserved index and load the specified
       // 'generationCount' with the current generation of the circular 
       // buffer.  Return 0 on success, a negative value if the queue is
       // disabled, and a positive value if the queue is full.  If this method
       // succeeds, other threads using this object may spin on the 
       // corresponding index state until 'releasePushReservation' is called 
       // using the returned 'index' and 'generationCount' values; clients
       // should call 'releasePushReservation' quickly after this method 
       // returns, without performing any blocking operations.
       // 'generationCount' indicates the number of times the circular buffer
       // has looped around; it is necessary for invoking 
       // 'releasePushReservation' but should not otherwise be used by the
       // caller. 

    int acquirePopIndex(bsl::size_t *generation, 
                        bsl::size_t *index);
       // Reserve the next available index from which to dequeue an element
       // from an (externally managed) circular buffer; load the specified
       // 'index' with the reserved index and load the specified
       // 'generationCount' with the current generation of the circular 
       // buffer.  Return 0 on success, and a non-zero value if the queue
       // is  empty.  If this method succeeds, other threads using this 
       // object may spin on the corresponding index state until 
       // 'releasePopReservation' is called using the returned 'index' 
       // and 'generationCount' values; clients should call
       // 'releasePopReservation' quickly after this method returns, without
       // performing any blocking operations.  'generationCount' indicates
       // the number of times the circular buffer has looped around; it is
       // necessary for invoking 'releasePopReservation' but should not 
       // otherwise be used by the caller. 

    void incrementPopIndexFrom(bsl::size_t index);
       // If the current pop index is the specified 'index', increment it
       // by one position. This may be used if it is necessary to force the
       // pop index to move regardless of the state of the cell it is 
       // referencing. 

    void releasePushReservation(bsl::size_t generation, bsl::size_t index);
       // Mark the specified 'index' as occupied (full) in the specified 
       // 'generation'.  The behavior is undefined unless 'generation' and 
       // 'index' were populated by a previous call to 'acquirePushIndex'.

    void releasePopReservation(bsl::size_t currGeneration, 
                               bsl::size_t index);
       // Mark the specified 'index' as available (empty) in the generation 
       // following the specified 'currGeneration'.  The behavior is undefined
       // unless 'generation' and  'index' were populated by a previous call
       // to 'acquirePushIndex'.
    
    void disable();
       // Mark the queue as disabled.  Future calls to 'acquirePushIndex' will
       // fail.

    void enable();
       // Mark the queue as enabled.

    // ACCESSORS
    bool isEnabled() const;        
       // Return 'true' if the queue is enabled, and 'false' if it is disabled.

    bsl::size_t length() const;
       // Return a snapshot of the number of items in the queue.

    bsl::size_t capacity() const; 
       // Return the maximum number of items that may be stored in the queue.
};
    
// =====================================================================
//                        INLINE FUNCTION DEFINITIONS
// =====================================================================

                     // ----------------------------------
                     // class AtomicRingBufferIndexManager
                     // ----------------------------------

// PRIVATE CLASS METHODS
inline bsl::size_t
bcec_AtomicRingBufferIndexManager::incrementIndex(bsl::size_t opCount,
                                                  bsl::size_t currentIndex)
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(e_MAX_OP_INDEX == opCount)) {
        return currentIndex + 1;
    }
    return opCount + 1;
}

// ACCESSORS
inline
bsl::size_t bcec_AtomicRingBufferIndexManager::capacity() const {
    return d_capacity;
}

inline
bool bcec_AtomicRingBufferIndexManager::isEnabled() const {
    return (0 == (d_pushIndex.load() & e_DISABLED_STATE_MASK));
}

inline
void bcec_AtomicRingBufferIndexManager::releasePushReservation(
                                                    bsl::size_t generation,
                                                    bsl::size_t index) {
    d_states[index] = e_INDEX_STATE_FULL | 
        (generation << e_INDEX_STATE_SHIFT);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------

