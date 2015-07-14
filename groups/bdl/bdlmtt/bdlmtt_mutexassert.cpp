// bdlmtt_mutexassert.cpp                                              -*-C++-*-
#include <bdlmtt_mutexassert.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmtt_mutex_cpp,"$Id$ $CSID$")

#include <bdlmtt_mutex.h>
#include <bdlmtt_threadutil.h>     // for testing only

#include <bdlmtt_platform.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#endif

namespace BloombergLP {

namespace bdlmtt {
void MutexAssert_Imp::assertIsLockedImpl(Mutex *mutex,
                                               const char  *text,
                                               const char  *file,
                                               int          line)
{
#if   defined(BDLMTT_PLATFORM_POSIX_THREADS)

    if (0 == mutex->tryLock()) {    // mutex isn't recursive in pthreads
        // The mutex was unlocked.

        mutex->unlock();    // restore mutex to the state it was originally in
        bsls::Assert::invokeHandler(text, file, line);
    }

#elif defined(BDLMTT_PLATFORM_WIN32_THREADS)

    if (-1 == mutex->nativeMutex().LockCount) {
        // The mutex was unlocked.

        bsls::Assert::invokeHandler(text, file, line);
    }

#else
# error unrecognized platform
#endif
}
}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
