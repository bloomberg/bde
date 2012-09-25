// bcemt_muteximpl_pthread.cpp                                        -*-C++-*-
#include <bcemt_muteximpl_pthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_muteximpl_pthread_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_assert.h>

#ifdef BCES_PLATFORM_POSIX_THREADS

namespace BloombergLP {

// Assert 'sizeof' assumption that is made in destructor (below).

BSLMF_ASSERT(
      0 == sizeof(bcemt_MutexImpl<bces_Platform::PosixThreads>) % sizeof(int));

               // --------------------------------------------------
               // class bcemt_MutexImpl<bces_Platform::PosixThreads>
               // --------------------------------------------------

// CREATORS
bcemt_MutexImpl<bces_Platform::PosixThreads>::~bcemt_MutexImpl()
{
    const int status = pthread_mutex_destroy(&d_lock);
    BSLS_ASSERT(0 == status);

#ifdef BDE_BUILD_TARGET_SAFE
    // Since we do not want to burden 'lock' and 'unlock' with the overhead of
    // 'BSLS_ASSERT_OPT' (as their 'status' values should always be 0 in a
    // well-behaved program), we overwrite 'd_lock' with garbage so as to
    // (potentially) trigger a fault on attempts to lock or unlock following
    // destruction.

    int       *pInt = reinterpret_cast<int *>(&d_lock);
    const int *pEnd = reinterpret_cast<const int *>(&d_lock + 1);

    while (pInt < pEnd) {
        *pInt = 0xdeadbeef;
        ++pInt;
    }
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
