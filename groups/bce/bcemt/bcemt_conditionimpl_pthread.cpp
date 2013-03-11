// bcemt_conditionimpl_pthread.cpp                                    -*-C++-*-
#include <bcemt_conditionimpl_pthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_conditionimpl_pthread_cpp,"$Id$ $CSID$")

#include <bcemt_saturatedtimeconversion.h>

#include <bdet_timeinterval.h>

#ifdef BCES_PLATFORM_POSIX_THREADS

namespace BloombergLP {

             // ------------------------------------------------------
             // class bcemt_ConditionImpl<bces_Platform::PosixThreads>
             // ------------------------------------------------------

// MANIPULATORS
int bcemt_ConditionImpl<bces_Platform::PosixThreads>::timedWait(
                                             bcemt_Mutex              *mutex,
                                             const bdet_TimeInterval&  timeout)
{
    timespec ts;
    bcemt_SaturatedTimeConversion::toTimeSpec(&ts, timeout);
    int status = pthread_cond_timedwait(&d_cond, &mutex->nativeMutex(), &ts);

    return 0 == status ? 0 : (ETIMEDOUT == status ? -1 : -2);
}

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
