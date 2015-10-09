// bslmt_qlock.cpp                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_qlock.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_qlock_cpp,"$Id$ $CSID$")
#include <bsls_assert.h>
#include <bslma_default.h>
#include <bslma_newdeleteallocator.h>
#include <bsls_atomicoperations.h>
#include <bslmt_threadlocalvariable.h>
#include <bslmt_threadutil.h>
#include <bslmt_semaphore.h>

#include <bslmt_barrier.h> // for testing only

namespace {

using namespace BloombergLP;

typedef bslmt::Semaphore *SemaphorePtr;

#ifdef BSLMT_THREAD_LOCAL_VARIABLE
// The thread-local variable that caches a thread-specific semaphore used by
// the 'setFlag' and 'waitOnFlag' methods.
BSLMT_THREAD_LOCAL_VARIABLE(SemaphorePtr, s_semaphore, 0)
#endif

typedef bslmt::ThreadUtil::Key TlsKey;

// The global TLS key for the thread-specific semaphore used by the 'setFlag'
// and 'waitOnFlag' methods.
bsls::AtomicOperations::AtomicTypes::Pointer s_semaphoreKey = {0};

inline
bslma::Allocator& semaphoreAllocator()
    // Obtain an allocator object suitable for allocating memory for a thread
    // local semaphore object which potentially outlives stateful allocators
    // with static storage.
{
    return bslma::NewDeleteAllocator::singleton();
}

inline
void releaseSemaphoreObject(void *semaphore)
    // Release the specified 'semaphore' object of type 'SemaphorePtr' which
    // was allocated with the 'semaphoreAllocator()'.  If 'semaphore' is 0,
    // this operation has no effect.
{
    SemaphorePtr sema = reinterpret_cast<SemaphorePtr>(semaphore);
    semaphoreAllocator().deleteObjectRaw(sema);
}

inline
void releaseTlsKey(TlsKey *key)
    // Delete the specified TLS 'key' and release the TLS 'key' object which
    // was allocated with the 'semaphoreAllocator()'.
{
    bslmt::ThreadUtil::deleteKey(*key);
    semaphoreAllocator().deleteObjectRaw(key);
}

struct SemaphoreKeyGuard {
    // A guard class that ensures, on destruction, that the global TLS key (for
    // the thread-local semaphores) is released.  Note that a single static
    // instance of this class is created to ensure a TLS key object passed to
    // its constructor is released on program terminate.

    // DATA
    TlsKey *d_key_p;            // Pointer to the TLS key object to be released
                                // on program terminate.

    SemaphoreKeyGuard(TlsKey *key)
        // Construct a 'SemaphoreKeyGuard' object with the specified TLS 'key'
        // pointer to be released in its destructor.  The behavior is undefined
        // unless 'key' is not NULL.
    : d_key_p(key)
    {
        BSLS_ASSERT(d_key_p);
    }

    ~SemaphoreKeyGuard()
        // Release the owned TLS key object.
    {
        // Note that it's possible, though unlikely, for the returned semaphore
        // to be 0 (e.g., in the main thread calls 'pthread_exit'), in which
        // case 'releaseSemaphoreObject' is a no-op.

        releaseSemaphoreObject(bslmt::ThreadUtil::getSpecific(*d_key_p));
        releaseTlsKey(d_key_p);
    }
};

extern "C" void deleteThreadLocalSemaphore(void *semaphore)
    // Free the memory for the specified 'semaphore'.  The behavior is
    // undefined unless 'semaphore' is either the address of a valid
    // 'bslmt::Semaphore' pointer, or 0.  Note that this function is intended
    // to serve as a "destructor" callback for 'bslmt::ThreadUtil::createKey'.
    // Note that 'deleteThreadLocalSemaphore' doesn't run on the main thread.
    // The main thread deletes the semaphore object in the 'SemaphoreKeyGuard'
    // destructor.
{
    releaseSemaphoreObject(semaphore);
}

TlsKey *initializeSemaphoreTLSKey()
    // Initialize the global key, 's_semaphoreKey' with a newly created
    // thread-local storage key, if one has not previously been created, and
    // return the address of initialized global key.  This operation is
    // thread-safe: calling it multiple times simultaneously will result in the
    // initialization of a single TLS key.  Note that the returned key can be
    // used to access the thread-local semaphore for the current thread.
{
    TlsKey *key = new (semaphoreAllocator()) TlsKey;

    int rc  = bslmt::ThreadUtil::createKey(key, &deleteThreadLocalSemaphore);
    BSLS_ASSERT_OPT(rc == 0);

    void *oldKey =
               bsls::AtomicOperations::testAndSwapPtr(&s_semaphoreKey, 0, key);

    if (0 == oldKey) {
        // Create the static key-guard to ensure the resources for
        // 's_semaphoreKey' are released when this program terminates.
        // 'testAndSwapPtr' above ensures that this done only once.

        static SemaphoreKeyGuard guard(key);
        return key;                                                   // RETURN
    }
    else {
        // Another thread has already initialized 's_semaphoreKey', so release
        // the 'key' now and return the original semaphore key value.

        releaseTlsKey(key);
        return reinterpret_cast<TlsKey *>(oldKey);                    // RETURN
    }
}

inline
TlsKey *getSemaphoreTLSKey()
    // Return the address of the unique, globally-shared, thread-local storage
    // key used to access the thread-local semaphore for the current thread.
    // Create and initialize a new key if one has not been previously been
    // created.  This operation is thread-safe: calling it multiple times
    // simultaneously will return the the same address to an initialized key.
{
    TlsKey *key = reinterpret_cast<TlsKey *>(
                              bsls::AtomicOperations::getPtr(&s_semaphoreKey));

    if (!key) {
        key = initializeSemaphoreTLSKey();
    }

    return key;
}

SemaphorePtr getSemaphoreForCurrentThread()
    // Return the address of the semaphore unique to the calling thread.
{
#ifdef BSLMT_THREAD_LOCAL_VARIABLE
    // Use a thread local variable if it's supported directly.

    if (!s_semaphore) {
        s_semaphore = new (semaphoreAllocator()) bslmt::Semaphore();

        // Still need to store the object in the thread specific key to release
        // it properly on thread exit.
        bslmt::ThreadUtil::setSpecific(*getSemaphoreTLSKey(), s_semaphore);
    }

    return s_semaphore;
#else
    // Manually manipulate the thread local storage.

    TlsKey *key = getSemaphoreTLSKey();
    SemaphorePtr sema = reinterpret_cast<SemaphorePtr>(
        bslmt::ThreadUtil::getSpecific(*key));

    if (!sema) {
        sema = new (semaphoreAllocator()) bslmt::Semaphore();
        bslmt::ThreadUtil::setSpecific(*key, sema);
    }

    return sema;
#endif
}

}  // close unnamed namespace

namespace BloombergLP {

                          // ---------------------
                          // class QLock_EventFlag
                          // ---------------------

#define FLAG_SET_WITHOUT_SEMAPHORE (reinterpret_cast<SemaphorePtr>(-1L))

// MANIPULATORS
void bslmt::QLock_EventFlag::set()
{
    // If the flag is unset and not (yet) associated with a semaphore, simply
    // set the flag.
    Semaphore *sem = d_status.testAndSwap(0, FLAG_SET_WITHOUT_SEMAPHORE);

    if (sem) {
        // If this flag has been associated with a semaphore, then we must
        // signal the waiting thread via that semaphore.

        // This flag permits only one set at a time, so the flag should not
        // have previously been set.
        BSLS_ASSERT(sem !=  FLAG_SET_WITHOUT_SEMAPHORE);

        sem->post();
    }
}

void bslmt::QLock_EventFlag::waitUntilSet(int spinRetryCount)
{
    Semaphore *flagValue = 0;

    // Read flag value with memory barrier, and if it was not set, spin a
    // little bit.
    int i = 0;
    do {
        flagValue = d_status;
    } while (!flagValue && ++i < spinRetryCount);

    if (0 == flagValue) {
        // This flag is still unset, use a semaphore to more efficiently wait
        // for the flag.

        Semaphore *sem = getSemaphoreForCurrentThread();

        flagValue = d_status.testAndSwap(0, sem);

        if (0 == flagValue ) {
            // A semaphore handle has been successfully stored, now wait until
            // it is signaled.

            sem->wait();

            return;                                                   // RETURN
        }
    }

    // If we did not wait on the semaphore (and return above), then the flag
    // must have been set without a semaphore.
    BSLS_ASSERT(flagValue == FLAG_SET_WITHOUT_SEMAPHORE);
}

                             // ----------------
                             // class QLockGuard
                             // ----------------

// MANIPULATORS
void bslmt::QLockGuard::unlockRaw()
{
    enum { k_SPIN = 1000 };

    BSLS_ASSERT(this != 0);

    QLockGuard *tail = (QLockGuard *)bsls::AtomicOperations::testAndSwapPtr(
                                        &d_qlock_p->d_guardQueueTail, this, 0);

    if (this == tail) {
        // There is no successor; the lock is now free.
        BSLS_ASSERT(d_next == 0);
        return;                                                       // RETURN
    }

    // Wait for successor to set the 'd_next' pointer.
    d_nextFlag.waitUntilSet(k_SPIN);

    BSLS_ASSERT(d_next != 0);

    // Pass the lock to successor.
    d_next->d_readyFlag.set();
}

void bslmt::QLockGuard::lock()
{
    enum { k_SPIN = 100 };

    BSLS_ASSERT(d_qlock_p);
    BSLS_ASSERT(!d_locked);

    // Insert 'this' at head of the queue.
    QLockGuard *pred = (QLockGuard *)
        bsls::AtomicOperations::swapPtr(&d_qlock_p->d_guardQueueTail, this);

    if (pred)  {
        // The lock was not free.  Link behind predecessor.
        pred->d_next = this;

        // Notify predecessor that 'd_next' is set
        pred->d_nextFlag.set();

        d_readyFlag.waitUntilSet(k_SPIN);
    }

    d_locked = true;
}

int bslmt::QLockGuard::tryLock()
{
    BSLS_ASSERT(d_qlock_p);

    if (d_locked) {
        return -1;                                                    // RETURN
    }

    // Grab the qlock if it is free
    QLockGuard *pred = (QLockGuard *)bsls::AtomicOperations::testAndSwapPtr(
                                        &d_qlock_p->d_guardQueueTail, 0, this);

    if (pred != 0) {
        // The lock was not free.  Do not wait.
        return 1;                                                     // RETURN
    }

    // The lock was free.  We are now at the head of the queue and own the
    // lock.
    d_locked = true;

    return 0;
}

}  // close enterprise namespace

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
