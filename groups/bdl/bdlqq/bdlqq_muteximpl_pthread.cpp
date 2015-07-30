// bdlqq_muteximpl_pthread.cpp                                        -*-C++-*-
#include <bdlqq_muteximpl_pthread.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_muteximpl_pthread_cpp,"$Id$ $CSID$")

#include <bslmf_assert.h>
#include <bsls_assert.h>

#ifdef BDLQQ_PLATFORM_POSIX_THREADS

namespace BloombergLP {

// Assert 'sizeof' assumption that is made in destructor (below).

BSLMF_ASSERT(
      0 == sizeof(bdlqq::MutexImpl<bdlqq::Platform::PosixThreads>) % sizeof(int));

namespace bdlqq {
               // --------------------------------------------------
               // class MutexImpl<bdlqq::Platform::PosixThreads>
               // --------------------------------------------------

// CREATORS
MutexImpl<bdlqq::Platform::PosixThreads>::~MutexImpl()
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
}  // close package namespace

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
