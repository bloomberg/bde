// bslma_mallocfreeallocator.cpp                                      -*-C++-*-
#include <bslma_mallocfreeallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

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

static bslma::MallocFreeAllocator *g_mallocFreeAllocatorSingleton_p = 0;
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
    void *v = new(&stackTemp) bslma::MallocFreeAllocator;

    // 'bsls::ObjectBuffer<T>' assignment is a bit-wise copy.

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

    if (!g_mallocFreeAllocatorSingleton_p) {
        g_mallocFreeAllocatorSingleton_p =
                                initSingleton(&g_mallocFreeAllocatorSingleton);
    }

    // In case the singleton has been previously initialized on another thread,
    // the data dependency between 'g_mallocFreeAllocator_p' and the singleton
    // itself will guarantee visibility of singleton updates on most modern
    // architectures.  This is the best we can do here in the absence of access
    // to any kind of atomic operations.

    return *g_mallocFreeAllocatorSingleton_p;
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
