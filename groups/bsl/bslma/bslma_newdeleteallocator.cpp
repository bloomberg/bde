// bslma_newdeleteallocator.cpp                                       -*-C++-*-
#include <bslma_newdeleteallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_objectbuffer.h>

#include <new>

namespace BloombergLP {

typedef bsls_ObjectBuffer<bslma_NewDeleteAllocator>
                                        bslma_NewDeleteAllocator_Singleton;
    // A 'bslma_NewDeleteAllocator_Singleton' is a buffer with the right size
    // and alignment to hold a 'bslma_NewDeleteAllocator' object.

static bslma_NewDeleteAllocator_Singleton g_newDeleteAllocatorSingleton;
    // 'g_newDeleteAllocatorSingleton' is a global static buffer to hold the
    // singleton.

static bslma_NewDeleteAllocator *g_newDeleteAllocatorSingleton_p = 0;
    // 'g_newDeleteAllocatorSingleton_p' is a global static pointer to the
    // singleton, which is *statically* initialized to 0.

static inline
bslma_NewDeleteAllocator *
initSingleton(bslma_NewDeleteAllocator_Singleton *address)
    // Construct a 'bslma_NewDeleteAllocator' at the specified 'address'
    // in a thread-safe way, and return 'address'.
{
    // Thread-safe initialization of singleton:
    //
    // A 'bslma_NewDeleteAllocator' contains no data members but does contain
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
    // is the same for all threads -- i.e., in case of a race, the second
    // thread copies identically the same data over the results of the first
    // thread's copy.

    bslma_NewDeleteAllocator_Singleton stackTemp;

    void *v = new(&stackTemp) bslma_NewDeleteAllocator;

    // Note that 'bsls_ObjectBuffer<T>' assignment is a bit-wise copy.

    *address = *(static_cast<bslma_NewDeleteAllocator_Singleton *>(v));

    return &address->object();
}

                        // ------------------------------
                        // class bslma_NewDeleteAllocator
                        // ------------------------------

// CLASS METHODS
bslma_NewDeleteAllocator& bslma_NewDeleteAllocator::singleton()
{
    // This initialization is not guaranteed to happen once, but repeated
    // initialization will be safe (see the comment above).

    if (!g_newDeleteAllocatorSingleton_p) {
        g_newDeleteAllocatorSingleton_p =
                                 initSingleton(&g_newDeleteAllocatorSingleton);
    }

    // In case the singleton has been previously initialized on another thread,
    // the data dependency between 'g_newDeleteAllocator_p' and the singleton
    // itself will guarantee visibility of singleton updates on most modern
    // architectures.  This is the best we can do here in the absence of access
    // to any kind of atomic operations.

    return *g_newDeleteAllocatorSingleton_p;
}

// CREATORS
bslma_NewDeleteAllocator::~bslma_NewDeleteAllocator()
{
}

// MANIPULATORS
void *bslma_NewDeleteAllocator::allocate(size_type size)
{
    BSLS_ASSERT_SAFE(0 <= size);

    return 0 == size ? 0 : ::operator new(size);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
