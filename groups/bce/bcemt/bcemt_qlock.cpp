// bcemt_qlock.cpp                  -*-C++-*-
#include <bcemt_qlock.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_qlock_cpp,"$Id$ $CSID$")
#include <bsls_assert.h>
#include <bslma_default.h>
#include <bces_atomicutil.h>
#include <bces_threadlocalvariable.h>
#include <bcemt_threadutil.h>
#include <bcemt_semaphore.h>

#include <bcemt_barrier.h> // for testing only

namespace {

using namespace BloombergLP;

typedef bcemt_Semaphore *SemaphorePtr;

#ifdef BCES_THREAD_LOCAL_VARIABLE
// The thread-local variable that caches a thread-specific semaphore
// used by the 'setFlag' and 'waitOnFlag' methods.
BCES_THREAD_LOCAL_VARIABLE(SemaphorePtr, s_semaphore, 0)
#endif

typedef bcemt_ThreadUtil::Key TlsKey;

// The global TLS key for the thread-specific semaphore used by the 'setFlag'
// and 'waitOnFlag' methods.
bces_AtomicUtil::Pointer s_semaphoreKey = {0};

class SemaphoreKeyGuard
{
    // A guard class that ensures, on destruction, that the global TLS key
    // (for the thread-local semaphores) is released.  Note that a static
    // instance of this class may be created to ensure the global TLS
    // key is released at program termination.

  public:

    ~SemaphoreKeyGuard();
        // Release the resources for the global TLS key 's_semaphoreKey', or do
        // nothing if that key has not been initialized or has previously been
        // released.
};

SemaphoreKeyGuard::~SemaphoreKeyGuard()
{
    TlsKey *key = reinterpret_cast<TlsKey*>(
                               bces_AtomicUtil::swapPtr(&s_semaphoreKey, 0));
    if (key) {
        bcemt_ThreadUtil::deleteKey(*key);
        bslma_Default::globalAllocator()->deleteObjectRaw(key);
    }
}

SemaphoreKeyGuard *initializeSemaphoreKeyGuard()
    // Create a static instance of 'SemaphoreKeyGuard' to ensure the global
    // TLS key 's_semaphoreKey' is released on program termination, and return
    // the address of that static key guard.
{
    static SemaphoreKeyGuard guard;
    return &guard;
}

extern "C" void deleteThreadLocalSemaphore(void *semaphore)
    // Free the memory for the specified 'semaphore' using the default global
    // allocator.  The behavior is undefined unless 'semaphore' is either the
    // address of a valid 'bcemt_Semaphore' pointer, or 0.  Note that this
    // function is intended to serve as a "destructor" callback for
    // 'bcemt_ThreadUtil::createKey'.
{
    SemaphorePtr sema = reinterpret_cast<SemaphorePtr>(semaphore);
    bslma_Default::globalAllocator()->deleteObjectRaw(sema);
}

TlsKey *initializeSemaphoreTLSKey()
    // Initialize the global key, 's_semaphoreKey' with a newly created
    // thread-local storage key, if one has not previously been created, and
    // return the address of initialized global key.  This operation is
    // thread-safe: calling it multiple times simultaneously will result in the
    // initialization of a single TLS key.  Note that the returned key can be
    // used to access the thread-local semaphore for the current thread.
{
    TlsKey *key = new (*bslma_Default::globalAllocator()) TlsKey;
    int rc  = bcemt_ThreadUtil::createKey(key, &deleteThreadLocalSemaphore);
    BSLS_ASSERT_OPT(rc == 0);

    void *oldKey = bces_AtomicUtil::testAndSwapPtr(&s_semaphoreKey, 0, key);


    if (0 == oldKey) {
        // Create the static key-guard to ensure the resources for
        // 's_semaphoreKey' are released when this program terminates.

        initializeSemaphoreKeyGuard();
    }
    else {
        // Another thread has already initialized 's_semaphoreKey', so release
        // the resources for 'key'.

        bcemt_ThreadUtil::deleteKey(*key);
        bslma_Default::globalAllocator()->deleteObjectRaw(key);
        key = reinterpret_cast<TlsKey*>(oldKey);
    }
    return key;
}

TlsKey *getSemaphoreTLSKey()
    // Return the address of the unique, globally-shared, thread-local storage
    // key used to access the thread-local semaphore for the current thread.
    // Create and initialize a new key if one has not been previously been
    // created.  This operation is thread-safe: calling it multiple times
    // simultaneously will return the the same address to an initialized key.
{
    TlsKey *key =
        reinterpret_cast<TlsKey *>(bces_AtomicUtil::getPtr(s_semaphoreKey));

    if (!key) {
        key = initializeSemaphoreTLSKey();
    }
    return key;
}

SemaphorePtr getSemaphoreForCurrentThread()
    // Return the address of the semaphore unique to the calling thread.
{
    SemaphorePtr sema;

#ifdef BCES_THREAD_LOCAL_VARIABLE
    sema = s_semaphore;
    if (sema) {
        return sema;                                                  // RETURN
    }
#endif

    TlsKey *key = getSemaphoreTLSKey();
    sema = reinterpret_cast<SemaphorePtr>(bcemt_ThreadUtil::getSpecific(*key));
    if (0 == sema) {
        sema = new (*bslma_Default::globalAllocator()) bcemt_Semaphore;
        bcemt_ThreadUtil::setSpecific(*key, sema);
    }

#ifdef BCES_THREAD_LOCAL_VARIABLE
    s_semaphore = sema;
#endif

    return sema;
}

}  // close unnamed namespace

namespace BloombergLP {

                           // ---------------------------
                           // class bcemt_QLock_EventFlag
                           // ---------------------------

#define FLAG_SET_WITHOUT_SEMAPHORE (reinterpret_cast<SemaphorePtr>(-1L))

// MANIPULATORS
void bcemt_QLock_EventFlag::set()
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

void bcemt_QLock_EventFlag::waitUntilSet(int spinRetryCount)
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

                           // ----------------------
                           // class bcemt_QLockGuard
                           // ----------------------

// MANIPULATORS
void bcemt_QLockGuard::unlockRaw()
{
    enum { SPIN = 1000 };

    BSLS_ASSERT(this != 0);

    bcemt_QLockGuard *tail = (bcemt_QLockGuard *)
        bces_AtomicUtil::testAndSwapPtr(&d_qlock_p->d_guardQueueTail, this, 0);

    if (this == tail) {
        // There is no successor; the lock is now free.
        BSLS_ASSERT(d_next == 0);
        return;                                                       // RETURN
    }

    // Wait for successor to set the 'd_next' pointer.
    d_nextFlag.waitUntilSet(SPIN);

    BSLS_ASSERT(d_next != 0);

    // Pass the lock to successor.
    d_next->d_readyFlag.set();
}

void bcemt_QLockGuard::lock()
{
    enum { SPIN = 100 };

    BSLS_ASSERT(d_qlock_p);
    BSLS_ASSERT(!d_locked);

    // Insert 'this' at head of the queue.
    bcemt_QLockGuard *pred = (bcemt_QLockGuard *)
        bces_AtomicUtil::swapPtr(&d_qlock_p->d_guardQueueTail, this);

    if (pred)  {
        // The lock was not free.  Link behind predecessor.
        pred->d_next = this;

        // Notify predecessor that 'd_next' is set
        pred->d_nextFlag.set();

        d_readyFlag.waitUntilSet(SPIN);
    }

    d_locked = true;
}

int bcemt_QLockGuard::tryLock()
{
    BSLS_ASSERT(d_qlock_p);

    if (d_locked) {
        return -1;                                                    // RETURN
    }

    // Grab the qlock if it is free
    bcemt_QLockGuard *pred = (bcemt_QLockGuard *)
        bces_AtomicUtil::testAndSwapPtr(&d_qlock_p->d_guardQueueTail, 0, this);

    if (pred != 0) {
        // The lock was not free.  Do not wait.
        return 1;                                                     // RETURN
    }

    // The lock was free.  We are now at the head of the queue and own the
    // lock.
    d_locked = true;

    return 0;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
