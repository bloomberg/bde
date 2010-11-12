// bcemt_threadimpl_pthread.cpp    -*-C++-*-

#include <bcemt_threadimpl_pthread.h>

#ifdef BCES_PLATFORM__POSIX_THREADS

namespace BloombergLP {

typedef bces_Platform::PosixThreads PT;
const pthread_t
    bcemt_ThreadUtilImpl<bces_Platform::PosixThreads>::INVALID_HANDLE
    =(pthread_t)-1;

bcemt_AttributeImpl<bces_Platform::PosixThreads>::bcemt_AttributeImpl()
{
    pthread_attr_init(&d_imp);
    setDetachedState(CREATE_JOINABLE);
    setSchedulingPolicy(BCEMT_SCHED_OTHER);
    setInheritSchedule(1);
}


bcemt_AttributeImpl<bces_Platform::PosixThreads>::bcemt_AttributeImpl(
     const bcemt_AttributeImpl<bces_Platform::PosixThreads>::NativeAttribute&
     original)
{
    int i;
    struct sched_param sched;

    pthread_attr_init(&d_imp);

    pthread_attr_getdetachstate(&original, &i);
    setDetachedState((i == PTHREAD_CREATE_DETACHED)
                     ? CREATE_DETACHED
                     : CREATE_JOINABLE);

    pthread_attr_getschedpolicy(&original, &i);
    switch(i) {
      case SCHED_FIFO:
        setSchedulingPolicy(BCEMT_SCHED_FIFO);
        break;
      case SCHED_RR:
        setSchedulingPolicy(BCEMT_SCHED_RR);
        break;
      default:
        setSchedulingPolicy(BCEMT_SCHED_OTHER);
        break;
    }

    pthread_attr_getschedparam(&original, &sched);
    setSchedulingPriority(sched.sched_priority);

    pthread_attr_getinheritsched(&original, &i);
    setInheritSchedule((i == PTHREAD_INHERIT_SCHED) ? 1 : 0);

    std::size_t size;

    pthread_attr_getstacksize(&original, &size);
    i = size;
    setStackSize(i);

#if !defined(BDES_PLATFORM__OS_DGUX) && !defined(BDES_PLATFORM__OS_CYGWIN)
    pthread_attr_getguardsize(&original, &size);
    i = size;
    setGuardSize(i);
#endif
}

bcemt_AttributeImpl<PT>&
bcemt_AttributeImpl<PT>::operator=(const bcemt_AttributeImpl<PT>& rhs)
{
    setDetachedState(rhs.detachedState());
    setSchedulingPolicy(rhs.schedulingPolicy());
    setSchedulingPriority(rhs.schedulingPriority());
    setInheritSchedule(rhs.inheritSchedule());
    setStackSize(rhs.stackSize());
    setGuardSize(rhs.guardSize());
    return *this;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
