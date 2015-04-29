// bslma_newdeleteallocator.cpp                                       -*-C++-*-
#include <bslma_newdeleteallocator.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_atomicoperations.h>
#include <bsls_objectbuffer.h>

#include <new>

namespace BloombergLP {

typedef bsls::ObjectBuffer<bslma::NewDeleteAllocator>
                                        bslma_NewDeleteAllocator_Singleton;
    // A 'bslma_NewDeleteAllocator_Singleton' is a buffer with the right size
    // and alignment to hold a 'bslma::NewDeleteAllocator' object.

static bslma_NewDeleteAllocator_Singleton g_newDeleteAllocatorSingleton;
    // 'g_newDeleteAllocatorSingleton' is a global static buffer to hold the
    // singleton.

static bsls::AtomicOperations::AtomicTypes::Pointer
                                         g_newDeleteAllocatorSingleton_p = {0};
    // 'g_newDeleteAllocatorSingleton_p' is a global static pointer to the
    // singleton, which is *statically* initialized to 0.

static inline
bslma::NewDeleteAllocator *
initSingleton(bslma_NewDeleteAllocator_Singleton *address)
    // Construct a 'bslma::NewDeleteAllocator' at the specified 'address' in a
    // thread-safe way, and return 'address'.
{
    // Thread-safe initialization of singleton:
    //
    // A 'bslma::NewDeleteAllocator' contains no data members but does contain
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
    void *v = new(stackTemp.buffer()) bslma::NewDeleteAllocator();

    // Note that 'bsls::ObjectBuffer<T>' copy-assignment is a bit-wise copy.
    // Also, it's imperative to use 'v' here instead of the 'stackTemp' object
    // itself even though they point to the same object in memory, because that
    // creates a data dependency between the construction of the
    // 'NewDeleteAllocator' and this copy-assignment.  Without this dependency
    // the construction of the 'NewDeleteAllocator' can be reordered past the
    // copy-assignment or optimized out (as observed for Solaris optimized
    // builds)
    *address = *(static_cast<bslma_NewDeleteAllocator_Singleton *>(v));
    return &address->object();
}

namespace bslma {

                        // ------------------------
                        // class NewDeleteAllocator
                        // ------------------------

// CLASS METHODS
NewDeleteAllocator& NewDeleteAllocator::singleton()
{
    // This initialization is not guaranteed to happen once, but repeated
    // initialization will be safe (see the comment above).

    if (!bsls::AtomicOperations::getPtrAcquire(
                                           &g_newDeleteAllocatorSingleton_p)) {
        bsls::AtomicOperations::setPtrRelease(
                &g_newDeleteAllocatorSingleton_p,
                initSingleton(&g_newDeleteAllocatorSingleton));
    }

    return *static_cast<bslma::NewDeleteAllocator *>(
                const_cast<void *>(
                    bsls::AtomicOperations::getPtrRelaxed(
                                           &g_newDeleteAllocatorSingleton_p)));
}

// CREATORS
NewDeleteAllocator::~NewDeleteAllocator()
{
}

// MANIPULATORS
void *NewDeleteAllocator::allocate(size_type size)
{
    return 0 == size ? 0 : ::operator new(size);
}

}  // close package namespace

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
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
