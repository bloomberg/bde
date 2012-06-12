// bcemt_sluice.h                                                     -*-C++-*-
#ifndef INCLUDED_BCEMT_SLUICE
#define INCLUDED_BCEMT_SLUICE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a "sluice" class.
//
//@CLASSES:
//  bcemt_Sluice: thread-aware sluice class
//
//@SEE_ALSO: bcemt_conditionimpl_win32
//
//@AUTHOR: Vlad Kliatchko (vkliatch)
//
//@DESCRIPTION: This component provides a "sluice" class, 'bcemt_Sluice'.
// A sluice is useful for controlling the release of threads from a common
// synchronization point.  One or more threads may "enter" a 'bcemt_Sluice'
// object (via the 'enter' method), and then wait to be released (via either
// the 'wait' or 'timedWait' method).  Either one waiting thread (via the
// 'signalOne' method), or all waiting threads (via the 'signalAll' method),
// may be signaled for release.  In either case, 'bcemt_Sluice' provides a
// guarantee against starvation; newly-entering threads will not indefinitely
// prevent threads that previously entered from being signaled.
//
///Usage
///-----
// 'bcemt_Sluice' is intended to be used to implement other synchronization
// mechanisms.  In particular, the functionality provided by 'bcemt_Sluice' is
// useful for implementing a condition variable:
//..
//  class MyCondition {
//      // This class implements a condition variable based on 'bcemt_Sluice'.
//
//      // DATA
//      bcemt_Sluice d_waitSluice;  // sluice object
//
//    public:
//      // MANIPULATORS
//      void wait(bcemt_Mutex *mutex)
//      {
//          void *token = d_waitSluice.enter();
//          mutex->unlock();
//          d_waitSluice.wait(token);
//          mutex->lock();
//      }
//
//      void signal()
//      {
//          d_waitSluice.signalOne();
//      }
//
//      void broadcast()
//      {
//          d_waitSluice.signalAll();
//      }
//  };
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_MUTEX
#include <bcemt_mutex.h>
#endif

#ifndef INCLUDED_BCEMT_TIMEDSEMAPHORE
#include <bcemt_timedsemaphore.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

                         // ==================
                         // class bcemt_Sluice
                         // ==================

class bcemt_Sluice {
    // This class controls the release of threads from a common synchronization
    // point.  One or more threads may "enter" a 'bcemt_Sluice' object, and
    // then wait to be released.  Either one waiting thread (via the
    // 'signalOne' method), or all waiting threads (via the 'signalAll'
    // method), may be signaled for release.  In any case, 'bcemt_Sluice'
    // provides a guarantee against starvation.

  private:
    // PRIVATE TYPES
    struct GenerationDescriptor {
        // This object represents one "generation" in a sluice.  A generation
        // begins when a thread enters the sluice, and ends (ceases accepting
        // new entering threads) when 'signalOne' or 'signalAll' is invoked.
        // The last thread in the generation to invoke 'wait' is responsible
        // for returning the descriptor to the pool.

        // DATA
        int                   d_numThreads;   // number of threads entered,
                                              // but not yet finished waiting

        int                   d_numSignaled;  // number of threads signaled,
                                              // but not yet finished waiting

        bcemt_TimedSemaphore  d_sema;         // semaphore on which to wait

        GenerationDescriptor *d_next;         // pointer to the next free
                                              // descriptor in the pool

        // CREATORS
        GenerationDescriptor();
            // Create a generation descriptor object.
    };

    // DATA
    bcemt_Mutex           d_mutex;               // for synchronizing access to
                                                 // data members

    GenerationDescriptor *d_signaledGeneration;  // generation in which at
                                                 // least one, but not all,
                                                 // threads have been signaled

    GenerationDescriptor *d_pendingGeneration;   // generation in which no
                                                 // threads have been signaled
                                                 // yet

    GenerationDescriptor *d_descriptorPool;      // pool of available
                                                 // generation descriptors

    bslma_Allocator      *d_allocator_p;         // memory allocator (held, not
                                                 // owned)

    // NOT IMPLEMENTED
    bcemt_Sluice(const bcemt_Sluice&);
    bcemt_Sluice& operator=(const bcemt_Sluice&);

  public:
    // CREATORS
    explicit
    bcemt_Sluice(bslma_Allocator *basicAllocator = 0);
        // Create a sluice.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If basicAllocator is 0, the currently installed
        // default allocator is used.

    ~bcemt_Sluice();
        // Destroy this sluice.

    // MANIPULATORS
    const void *enter();
        // Enter this sluice, and return the token on which the calling thread
        // must subsequently wait.  The behavior is undefined unless 'wait' or
        // 'timedWait' is invoked with the token before this sluice is
        // destroyed.

    void signalOne();
        // Signal one thread that has entered this sluice and has not yet been
        // released.

    void signalAll();
        // Signal all threads that have entered this sluice and have not yet
        // been released.

    int timedWait(const void *token, const bdet_TimeInterval& timeout);
        // Wait for the specified 'token' to be signaled, or until the
        // specified 'timeout' (expressed as the !ABSOLUTE! time from 00:00:00
        // UTC, January 1, 1970) expires.  Return 0 on success, and a non-zero
        // value on timeout.  The 'token' is released whether or not a timeout
        // occurred.  The behavior is undefined unless 'token' was obtained
        // from a call to 'enter' by this thread, and was not subsequently
        // released (via a call to 'timedWait' or 'wait').

    void wait(const void *token);
        // Wait for the specified 'token' to be signaled, and release the
        // 'token'.  The behavior is undefined unless 'token' was obtained from
        // a call to 'enter' by this thread, and was not subsequently released
        // (via a call to 'timedWait' or 'wait').
};

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
