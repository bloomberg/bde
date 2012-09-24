// bcemt_conditionimpl_win32.cpp                                      -*-C++-*-
#include <bcemt_conditionimpl_win32.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_conditionimpl_win32_cpp,"$Id$ $CSID$")

#include <bcemt_mutex.h>

#ifdef BCES_PLATFORM_WIN32_THREADS

// #define BCEMT_WIN64_DEBUG

namespace BloombergLP {

             // ------------------------------------------------------
             // class bcemt_ConditionImpl<bces_Platform::Win32Threads>
             // ------------------------------------------------------

// MANIPULATORS
int bcemt_ConditionImpl<bces_Platform::Win32Threads>::timedWait(
                                             bcemt_Mutex              *mutex,
                                             const bdet_TimeInterval&  timeout)
{
    const void *sluiceToken = d_waitSluice.enter();
    mutex->unlock();
    const int rc = d_waitSluice.timedWait(sluiceToken, timeout);
    mutex->lock();

    return 0 == rc ? 0 : -1;
}

int bcemt_ConditionImpl<bces_Platform::Win32Threads>::wait(bcemt_Mutex *mutex)
{
    const void *sluiceToken = d_waitSluice.enter();
    mutex->unlock();
    d_waitSluice.wait(sluiceToken);
    mutex->lock();

    return 0;
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM_WIN32_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
