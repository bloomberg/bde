// bdlqq_conditionimpl_win32.cpp                                      -*-C++-*-
#include <bdlqq_conditionimpl_win32.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_conditionimpl_win32_cpp,"$Id$ $CSID$")

#include <bdlqq_mutex.h>

#ifdef BDLQQ_PLATFORM_WIN32_THREADS

// #define BCEMT_WIN64_DEBUG

namespace BloombergLP {

namespace bdlqq {
             // ------------------------------------------------------
             // class ConditionImpl<bdlqq::Platform::Win32Threads>
             // ------------------------------------------------------

// MANIPULATORS
int ConditionImpl<bdlqq::Platform::Win32Threads>::timedWait(
                                             Mutex              *mutex,
                                             const bsls::TimeInterval&  timeout)
{
    const void *sluiceToken = d_waitSluice.enter();
    mutex->unlock();
    const int rc = d_waitSluice.timedWait(sluiceToken, timeout);
    mutex->lock();

    return 0 == rc ? 0 : -1;
}

int ConditionImpl<bdlqq::Platform::Win32Threads>::wait(Mutex *mutex)
{
    const void *sluiceToken = d_waitSluice.enter();
    mutex->unlock();
    d_waitSluice.wait(sluiceToken);
    mutex->lock();

    return 0;
}
}  // close package namespace

}  // close namespace BloombergLP

#endif  // BDLQQ_PLATFORM_WIN32_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
