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
//@DESCRIPTION: This component implements a thread-enabled fixed-size ring
// buffer containing state variables. This class is intended to provide 
// synchronization for some other data structure representing a queue of
// values for multithreaded communication in a producer-consumer model.
// 
// This is not a general-purpose queue data structure. For example, no user
// data of any kind is stored in this data structure (it is not a queue of
// integers), and successful invocation of certain methods ('acquirePopIndex', 
// 'acquirePushIndex') obligates the caller to invoke a corresponding method
// ('releaseElement', 'stopWriting' respectively); otherwise, other threads
// may "spin" indefinitely with severe performance consequences.  
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
    int  acquirePushIndex(bsl::size_t *generation, 
                          bsl::size_t *index);
       // Mark the next available index as "writing" and load that index 
       // into the specified 'index'.  Load the current generation count into
       // the specified 'generation'.  Return 0 on success, and a nonzero
       // value if the queue is disabled or full.  If this method succeeds, 
       // the caller is obligated to call 'stopWriting' with the 
       // 'generation' and 'index' values; otherwise, other threads using
       // this data structure may "spin" indefinitely. 

    int  acquirePopIndex(bsl::size_t *generation, 
                         bsl::size_t *index);
       // Mark the next occupied index (one having the specified
       // 'currentState' in the current generation) as "reading" and load
       // that index  into the specified 'index'.  Load the current 
       // generation count into the specified 'generation'.  Return 0 on 
       // success, and a nonzero value if the queue is empty.  If this method
       // succeeds, the caller is obligated to call 'releaseElement' with the 
       // 'generation' and 'index' values; otherwise, other threads using
       // this data structure may "spin" indefinitely. 

    void incrementPopIndexFrom(bsl::size_t index);
       // If the current pop index is the specified 'index', increment it
       // by one position. This may be used if it is necessary to force the
       // pop index to move regardless of the state of the cell it is 
       // referencing. 

    void releaseElement(bsl::size_t currGeneration, 
                        bsl::size_t index);
       // Mark the specified 'index' as available (empty) in the generation 
       // following the specified 'currGeneration'. 

    void stopWriting(bsl::size_t generation, bsl::size_t index);
       // Mark the specified 'index' as occupied (full) in the specified 
       // 'generation'.
    
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
void bcec_AtomicRingBufferIndexManager::stopWriting(bsl::size_t generation,
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

