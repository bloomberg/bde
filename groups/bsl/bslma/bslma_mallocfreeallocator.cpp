// bslma_mallocfreeallocator.cpp                                      -*-C++-*-
#include <bslma_mallocfreeallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_atomicoperations.h>
#include <bsls_objectbuffer.h>

#include <new>

// This allocator is simply an "adapter" connecting 'std::malloc' and
// 'std::free' to the 'bslma::Allocator' interface.  We use the reserve pool
// pattern to ensure that the returned allocator object remains valid forever
// (without leaking memory).

namespace BloombergLP {

                        // -----------------------------------------
                        // union bslma_MallocFreeAllocator_Singleton
                        // -----------------------------------------

typedef bsls::ObjectBuffer<bslma::MallocFreeAllocator>
                                           bslma_MallocFreeAllocator_Singleton;
    // 'A bslma_MallocFreeAllocator_Singleton' is a buffer with the right size
    // and alignment to hold a 'bslma::MallocFreeAllocator' object.

static bslma_MallocFreeAllocator_Singleton g_mallocFreeAllocatorSingleton;
    // A global static buffer to hold the singleton.

static bsls::AtomicOperations::AtomicTypes::Pointer
                                        g_mallocFreeAllocatorSingleton_p = {0};
    // A global static pointer to the singleton, which is *statically*
    // initialized to zero.

                        // -----------------------------
                        // static inline helper function
                        // -----------------------------

static inline
bslma::MallocFreeAllocator *initSingleton(
                                        bslma_MallocFreeAllocator_Singleton *p)
    // Construct a 'bslma::MallocFreeAllocator' at the location specified by
    // 'p' in a thread-safe way.  Return 'p'.
{
    // Thread-safe initialization of singleton.
    //
    // A 'bslma::MallocFreeAllocator' contains no data members but does contain
    // a vtbl pointer.  During construction, the vtbl pointer is first set the
    // base class's vtbl before it is set to its final, derived-class value.
    // If two threads try to initialize the same singleton, the one that
    // finishes first may be in for a rude awakening as the second thread
    // temporarily changes the vtbl ptr to point to the base class's vtbl.
    //
    // We solve this problem by initializing the singleton on the stack and
    // then bit-copying it into its final location.  The stack initialization
    // is safe because each thread has its own stack.  The bit-copy is safe
    // because the only thing that is being copied is the vtbl pointer, which
    // is the same for all threads.  (I.e., in case of a race, the second
    // thread copies the exact same data over the the results of the first
    // thread's copy.)

    bslma_MallocFreeAllocator_Singleton stackTemp;
    void *v = new(stackTemp.buffer()) bslma::MallocFreeAllocator();

    // Note that 'bsls::ObjectBuffer<T>' copy-assignment is a bit-wise copy.
    // Also, it's imperative to use 'v' here instead of the 'stackTemp' object
    // itself even though they point to the same object in memory, because that
    // creates a data dependency between the construction of the
    // 'MallocFreeAllocator' and this copy-assignment.  Without this dependency
    // the construction of the 'MallocFreeAllocator' can be reordered past the
    // copy-assignment or optimized out (as observed for Solaris optimized
    // builds)
    *p = *(static_cast<bslma_MallocFreeAllocator_Singleton *>(v));
    return &p->object();
}

namespace bslma {

                        // -------------------------
                        // class MallocFreeAllocator
                        // -------------------------

// CLASS METHODS
MallocFreeAllocator& MallocFreeAllocator::singleton()
{
    // This initialization is not guaranteed to happen once, but repeated
    // initialization will be safe (see the comment above).

    if (!bsls::AtomicOperations::getPtrAcquire(
                                          &g_mallocFreeAllocatorSingleton_p)) {
        bsls::AtomicOperations::setPtrRelease(
                &g_mallocFreeAllocatorSingleton_p,
                initSingleton(&g_mallocFreeAllocatorSingleton));
    }

    return *(bslma::MallocFreeAllocator *)
       bsls::AtomicOperations::getPtrRelaxed(&g_mallocFreeAllocatorSingleton_p);
}

}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
