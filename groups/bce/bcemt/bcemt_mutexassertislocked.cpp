// bcemt_mutexassertislocked.cpp                                      -*-C++-*-
#include <bcemt_mutexassertislocked.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_mutex_cpp,"$Id$ $CSID$")

#include <bcemt_mutex.h>
#include <bcemt_threadutil.h>     // for testing only

#include <bces_platform.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#endif

namespace BloombergLP {

void bcemt_MutexAssertIsLocked::assertIsLockedImpl(bcemt_Mutex *mutex,
                                                   const char  *text,
                                                   const char  *file,
                                                   int          line)
{
#if   defined(BCES_PLATFORM_POSIX_THREADS)

    if (0 == mutex->tryLock()) {    // mutex isn't recursive in pthreads
        // The mutex was unlocked.

        mutex->unlock();    // restore mutex to the state it was originally in
        bsls::Assert::invokeHandler(text, file, line);
    }

#elif defined(BCES_PLATFORM_WIN32_THREADS)

    if (-1 == mutex->nativeMutex().LockCount) {
        // The mutex was unlocked.

        bsls::Assert::invokeHandler(text, file, line);
    }

#else
# error unrecognized platform
#endif
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
