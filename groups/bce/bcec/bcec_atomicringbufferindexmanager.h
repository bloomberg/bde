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
// corresponding method ('releasePopIndex', 'releasePushIndex' 
// respectively); otherwise, other threads may "spin" indefinitely with 
// severe performance consequences.  
// 
///Exception safety
///----------------
// All methods of the 'bcec_AtomicRingBufferIndexManager' provide a no-throw
// exception guarantee, except for the constructor, which is exception neutral.

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
    // This class implements a circular buffer of atomic state variables.
    // These are intended to synchronize access to another (non-atomic) 
    // indexed data structure so that the other data structure can be used
    // as a thread-enabled producer/consumer queue.

    // PRIVATE CONSTANTS
    enum {    
        e_PADDING = bces_Platform::e_CACHE_LINE_SIZE - sizeof(bsls::AtomicInt)
    };


    // DATA
    bsls::AtomicInt    d_pushIndex;
                           // index in circular buffer in which the next
                           // element will be pushed
        
    const char         d_pushIndexPad[e_PADDING]; 
                           // padding to prevent false sharing

    bsls::AtomicInt    d_popIndex;
                           // index in the circular buffer from which the next
                           // element will be popped 

    const char         d_popIndexPad[e_PADDING];
                           // padding to prevent false sharing

    const bsl::size_t  d_capacity;  
                           // maximum number of elements that can be held in
                           // the circular buffer

    const bsl::size_t  d_maxGeneration; 
                           // highest possible generation count

    const bsl::size_t  d_rolloverIndex; 
                           // if maxGeneration reached

    bsls::AtomicInt   *d_states; 
                           // array of index state variables 

    bslma::Allocator  *d_allocator_p;
                           // allocator, held not owned


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
       // Create an index manager for a circular buffer having the specified
       // maximum 'capacity'.  Optionally specify a 'basicAllocator' used to
       // supply memory.  If 'basicAllocator' is 0, the currently installed
       // default allocator is used.  'isEnabled' will be 'true' for the
       // newly created index manager.

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
       // corresponding index state until 'releasePushIndex' is called 
       // using the returned 'index' and 'generationCount' values; clients
       // should call 'releasePushIndex' quickly after this method 
       // returns, without performing any blocking operations.  Note that
       // 'generationCount' is necessary for invoking
       // 'releasePusedReservation' but should not otherwise be used by the
       // caller;  the value reflects the of times the 'index' in the circular
       // buffer has been used.

    int acquirePopIndex(bsl::size_t *generation, 
                        bsl::size_t *index);
       // Reserve the next available index from which to dequeue an element
       // from an (externally managed) circular buffer; load the specified
       // 'index' with the reserved index and load the specified
       // 'generationCount' with the current generation of the circular 
       // buffer.  Return 0 on success, and a non-zero value if the queue
       // is  empty.  If this method succeeds, other threads using this 
       // object may spin on the corresponding index state until 
       // 'releasePopIndex' is called using the returned 'index' 
       // and 'generationCount' values; clients should call
       // 'releasePopIndex' quickly after this method returns, without
       // performing any blocking operations.  Note that 'generationCount' is
       // necessary for invoking 'releasePopIndex' but should not  
       // otherwise be used by the caller;  the value reflects the of times
       // the 'index' in the circular buffer has been used.

    void incrementPopIndexFrom(bsl::size_t index);
       // If the current pop index is the specified 'index', increment it
       // by one position. This may be used if it is necessary to force the
       // pop index to move regardless of the state of the cell it is 
       // referencing. 

    void releasePushIndex(bsl::size_t generation, bsl::size_t index);
       // Mark the specified 'index' as occupied (full) in the specified 
       // 'generation'.  The behavior is undefined unless 'generation' and 
       // 'index' were populated by a previous call to 'acquirePushIndex'.

    void releasePopIndex(bsl::size_t currGeneration, bsl::size_t index);
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

// ACCESSORS
inline
bsl::size_t bcec_AtomicRingBufferIndexManager::capacity() const {
    return d_capacity;
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

