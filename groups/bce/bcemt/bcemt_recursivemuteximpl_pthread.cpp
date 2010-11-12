// bcemt_recursivemuteximpl_pthread.cpp                               -*-C++-*-
#include <bcemt_recursivemuteximpl_pthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_recursivemuteximpl_pthread_cpp,"$Id$ $CSID$")

#ifdef BCES_PLATFORM__POSIX_THREADS

namespace BloombergLP {

          // -----------------------------------------------------------
          // class bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>
          // -----------------------------------------------------------

// CREATORS
bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>
                                                   ::bcemt_RecursiveMutexImpl()
{
    pthread_mutexattr_t attribute;
    pthread_mutexattr_init(&attribute);
#ifdef PTHREAD_MUTEX_RECURSIVE
    pthread_mutexattr_settype(&attribute,PTHREAD_MUTEX_RECURSIVE);
#else
    d_lockCount = 0;
#endif
    pthread_mutex_init(&d_lock, &attribute);
    pthread_mutexattr_destroy(&attribute);
}

// MANIPULATORS
void bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>::lock()
{
#ifdef PTHREAD_MUTEX_RECURSIVE
    pthread_mutex_lock(&d_lock);
#else
    if (pthread_mutex_trylock(&d_lock)) {
        d_spin.lock();
        if (d_lockCount && pthread_equal(d_owner,pthread_self())) {
            ++d_lockCount;
            d_spin.unlock();
            return;                                                   // RETURN
        }
        d_spin.unlock();
        pthread_mutex_lock(&d_lock);
    }
    d_spin.lock();
    d_owner = pthread_self();
    d_lockCount = 1;
    d_spin.unlock();
#endif
}

int bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>::tryLock()
{
#ifdef PTHREAD_MUTEX_RECURSIVE
    return pthread_mutex_trylock(&d_lock);                            // RETURN
#else
    if (pthread_mutex_trylock(&d_lock)) {
        d_spin.lock();
        if (d_lockCount && pthread_equal(d_owner,pthread_self())) {
            ++d_lockCount;
            d_spin.unlock();
            return 0;                                                 // RETURN
        }
        else {
            d_spin.unlock();
            return 1;                                                 // RETURN
        }
    }
    else {
        d_spin.lock();
        d_owner = pthread_self();
        d_lockCount = 1;
        d_spin.unlock();
        return 0;                                                     // RETURN
    }
#endif
}

void bcemt_RecursiveMutexImpl<bces_Platform::PosixThreads>::unlock()
{
#ifdef PTHREAD_MUTEX_RECURSIVE
    pthread_mutex_unlock(&d_lock);
#else
    d_spin.lock();
    if (!--d_lockCount) {
        d_spin.unlock();
        pthread_mutex_unlock(&d_lock);
        return;                                                       // RETURN
    }
    d_spin.unlock();
#endif
}

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
