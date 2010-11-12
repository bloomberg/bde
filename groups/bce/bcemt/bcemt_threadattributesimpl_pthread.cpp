// bcemt_threadattributesimpl_pthread.cpp                             -*-C++-*-
#include <bcemt_threadattributesimpl_pthread.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_threadattributesimpl_pthread_cpp,"$Id$ $CSID$")

#include <bsls_platform.h>

#ifdef BCES_PLATFORM__POSIX_THREADS

#include <bsls_assert.h>

#include <bsl_cstdlib.h>   // 'bsl::size_t'

#include <limits.h>    // TBD why?

namespace BloombergLP {

        // -------------------------------------------------------------
        // class bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>
        // -------------------------------------------------------------

// CREATORS
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::
                                                   bcemt_ThreadAttributesImpl()
{
    pthread_attr_init(&d_imp);
    setDetachedState(BCEMT_CREATE_JOINABLE);
    setSchedulingPolicy(BCEMT_SCHED_OTHER);
    setInheritSchedule(1);

    // enforce sanity of stack size
    setStackSize(stackSize());
}

bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::
                                                    bcemt_ThreadAttributesImpl(
    const bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::
                                                    NativeAttribute& attribute)
{
    pthread_attr_init(&d_imp);

    int detachState;
    pthread_attr_getdetachstate(&attribute, &detachState);
    setDetachedState(PTHREAD_CREATE_DETACHED == detachState
                     ? BCEMT_CREATE_DETACHED
                     : BCEMT_CREATE_JOINABLE);

    int schedulingPolicy;
    pthread_attr_getschedpolicy(&attribute, &schedulingPolicy);
    switch (schedulingPolicy) {
      case SCHED_FIFO: {
        setSchedulingPolicy(BCEMT_SCHED_FIFO);
      } break;
      case SCHED_RR: {
        setSchedulingPolicy(BCEMT_SCHED_RR);
      } break;
      default: {
        setSchedulingPolicy(BCEMT_SCHED_OTHER);
      } break;
    }

    struct sched_param sched;
    pthread_attr_getschedparam(&attribute, &sched);
    setSchedulingPriority(sched.sched_priority);

    int inheritSchedule;
    pthread_attr_getinheritsched(&attribute, &inheritSchedule);
    setInheritSchedule(PTHREAD_INHERIT_SCHED == inheritSchedule ? 1 : 0);

    bsl::size_t size;

    pthread_attr_getstacksize(&attribute, &size);
    setStackSize(static_cast<int>(size));

#if !defined(BSLS_PLATFORM__OS_CYGWIN)
    pthread_attr_getguardsize(&attribute, &size);
    setGuardSize(static_cast<int>(size));
#endif
}

bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::
                                                    bcemt_ThreadAttributesImpl(
       const bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>& original)
{
    pthread_attr_init(&d_imp);
    *this = original;
}

// MANIPULATORS
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>&
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::operator=(
            const bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>& rhs)
{
    if (this != &rhs) {
        setDetachedState(rhs.detachedState());
        setSchedulingPolicy(rhs.schedulingPolicy());
        setSchedulingPriority(rhs.schedulingPriority());
        setInheritSchedule(rhs.inheritSchedule());
        setStackSize(rhs.stackSize());
        setGuardSize(rhs.guardSize());
    }
    return *this;
}

void
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::setSchedulingPolicy(
    bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::SchedulingPolicy
                                                              schedulingPolicy)
{
    switch (schedulingPolicy) {
      case BCEMT_SCHED_FIFO: {
        pthread_attr_setschedpolicy(&d_imp, SCHED_FIFO);
      } break;
      case BCEMT_SCHED_RR: {
        pthread_attr_setschedpolicy(&d_imp, SCHED_RR);
      } break;
      default: {
        pthread_attr_setschedpolicy(&d_imp, SCHED_OTHER);
      } break;
    }
}

void
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::setSchedulingPriority(
                                                        int schedulingPriority)
{
    struct sched_param sched;
    pthread_attr_getschedparam(&d_imp, &sched);
    sched.sched_priority = schedulingPriority;
    pthread_attr_setschedparam(&d_imp, &sched);
}

void bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::setStackSize(
                                                                 int stackSize)
{
    bsl::size_t newStacksize = stackSize;
#ifdef PTHREAD_STACK_MIN
    if (stackSize < PTHREAD_STACK_MIN)
#else
    if (stackSize <= 0)
#endif
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        const int ret = pthread_attr_getstacksize(&attr, &newStacksize);
        BSLS_ASSERT(0 == ret);
        pthread_attr_destroy(&attr);
#ifdef BSLS_PLATFORM__OS_SOLARIS
        // On Solaris, the default return value is 0, and setting 0 returns
        // EINVAL.  So we just set the documented default (as per
        // 'man pthread_attr_setstacksize').  Note that another approach would
        // be to reset 'd_imp', and then set all attributes except for the
        // stack size.

        if (0 == newStacksize) {
#ifdef BSLS_PLATFORM__CPU_64_BIT
            newStacksize = 2 * 1024 * 1024;
#else
            newStacksize = 1024 * 1024;
#endif
        }
#endif
    }
    const int ret = pthread_attr_setstacksize(&d_imp, newStacksize);
    BSLS_ASSERT("Could not set thread stack size" && 0 == ret);
}

// ACCESSORS
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::DetachedState
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::detachedState() const
{
    int detachState;
    pthread_attr_getdetachstate(&d_imp, &detachState);
    return PTHREAD_CREATE_DETACHED == detachState
           ? BCEMT_CREATE_DETACHED
           : BCEMT_CREATE_JOINABLE;
}

int bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::guardSize() const
{
#if !defined(BSLS_PLATFORM__OS_CYGWIN)
    bsl::size_t size;
    pthread_attr_getguardsize(&d_imp, &size);
    return static_cast<int>(size);
#else
    return -1;
#endif
}

int bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::
                                                        inheritSchedule() const
{
    int inheritSchedule;
    pthread_attr_getinheritsched(&d_imp, &inheritSchedule);
    return PTHREAD_INHERIT_SCHED == inheritSchedule ? 1 : 0;
}

bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::SchedulingPolicy
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::
                                                       schedulingPolicy() const
{
    int schedulingPolicy;
    pthread_attr_getschedpolicy(&d_imp, &schedulingPolicy);
    switch (schedulingPolicy) {
      case SCHED_FIFO: {
        return BCEMT_SCHED_FIFO;
      }
      case SCHED_RR: {
        return BCEMT_SCHED_RR;
      }
      default: {
        return BCEMT_SCHED_OTHER;
      }
    }
}

int
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::
                                                     schedulingPriority() const
{
    struct sched_param sched;
    pthread_attr_getschedparam(&d_imp, &sched);
    return sched.sched_priority;
}

int bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::stackSize() const
{
    bsl::size_t size;
    pthread_attr_getstacksize(&d_imp, &size);
    return static_cast<int>(size);
}

// CLASS METHODS
int
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::getMaxSchedPriority(
                                                                    int policy)
{
    if (-1 == policy) {
        policy = sched_getscheduler(0);
    }
    else {
        switch (policy) {
          case BCEMT_SCHED_FIFO: {
            policy = SCHED_FIFO;
          } break;
          case BCEMT_SCHED_RR: {
            policy = SCHED_RR;
          } break;
          case BCEMT_SCHED_OTHER: {
            policy = SCHED_OTHER;
          } break;
          default: {
            return -1;
          }
       }
    }
    return sched_get_priority_max(policy);
}

int
bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>::getMinSchedPriority(
                                                                    int policy)
{
    if (-1 == policy) {
        policy = sched_getscheduler(0);
    }
    else {
        switch (policy) {
          case BCEMT_SCHED_FIFO: {
            policy = SCHED_FIFO;
          } break;
          case BCEMT_SCHED_RR: {
            policy = SCHED_RR;
          } break;
          case BCEMT_SCHED_OTHER: {
            policy = SCHED_OTHER;
          } break;
          default: {
            return -1;
          }
       }
    }
    return sched_get_priority_min(policy);
}

// FREE OPERATORS
bool
operator==(const bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>& lhs,
           const bcemt_ThreadAttributesImpl<bces_Platform::PosixThreads>& rhs)
{
    return lhs.stackSize()          == rhs.stackSize()
        && lhs.guardSize()          == rhs.guardSize()
        && lhs.schedulingPolicy()   == rhs.schedulingPolicy()
        && lhs.schedulingPriority() == rhs.schedulingPriority()
        && lhs.inheritSchedule()    == rhs.inheritSchedule()
        && lhs.detachedState()      == rhs.detachedState();
}

}  // close namespace BloombergLP

#endif  // BCES_PLATFORM__POSIX_THREADS

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
