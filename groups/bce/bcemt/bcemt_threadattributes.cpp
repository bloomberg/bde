// bcemt_threadattributes.cpp                                         -*-C++-*-
#include <bcemt_threadattributes.h>

#include <bces_platform.h>

#if defined(BCES_PLATFORM__POSIX_THREADS)
# include <pthread.h>
#elif !defined(BCES_PLATFORM__WIN32_THREADS)
# error unrecognized threading platform
#endif

#include <bsls_platform.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_threadattributes_cpp,"$Id$ $CSID$")

namespace {

namespace Local {

#if   defined(BSLS_PLATFORM__OS_SOLARIS) || defined(BSLS_PLATFORM__OS_LINUX)
    enum { DEFAULT_PRIORITY = 0 };
#elif defined(BSLS_PLATFORM__OS_AIX)
    enum { DEFAULT_PRIORITY = 1 };
#elif defined(BSLS_PLATFORM__OS_HPUX)
    enum { DEFAULT_PRIORITY = -256 };
#elif defined(BSLS_PLATFORM__OS_WINDOWS)
    enum { DEFAULT_PRIORITY = -1 };      // ignored
#else
# error unrecognized platform
#endif

}  // close namespace Local

}  // close unnamed namespace

namespace BloombergLP {

// CLASS METHODS
int bcemt_ThreadAttributes::getMaxSchedPriority(int policy)
{
#if defined(BCES_PLATFORM__POSIX_THREADS)
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
        policy = sched_getscheduler(0);
      }
    }

    return sched_get_priority_max(policy);
#else
    return -1;
#endif
}

int bcemt_ThreadAttributes::getMinSchedPriority(int policy)
{
#if defined(BCES_PLATFORM__POSIX_THREADS)
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
        policy = sched_getscheduler(0);
      }
    }

    return sched_get_priority_min(policy);
#else
    return -1;
#endif
}

// CREATORS
bcemt_ThreadAttributes::bcemt_ThreadAttributes()
: d_detachedState(BCEMT_CREATE_JOINABLE)
, d_guardSize(BCEMT_INVALID_GUARD_SIZE)
, d_inheritSchedule(1)
, d_schedulingPolicy(BCEMT_SCHED_OTHER)
, d_schedulingPriority(Local::DEFAULT_PRIORITY)
, d_stackSize(BCEMT_INVALID_STACK_SIZE)
{}

// MANIPULATORS
void bcemt_ThreadAttributes::setSchedulingPolicy(
                     bcemt_ThreadAttributes::SchedulingPolicy schedulingPolicy)
{
    d_schedulingPolicy = schedulingPolicy;

    const int maxPri = getMaxSchedPriority(schedulingPolicy);
    const int minPri = getMinSchedPriority(schedulingPolicy);
    if      (d_schedulingPriority > maxPri) {
        d_schedulingPriority = maxPri;
    }
    else if (d_schedulingPriority < minPri) {
        d_schedulingPriority = minPri;
    }
}

// FREE OPERATORS
bool operator==(const bcemt_ThreadAttributes& lhs,
                const bcemt_ThreadAttributes& rhs)
{
    return lhs.d_detachedState      == rhs.d_detachedState      &&
           lhs.d_guardSize          == rhs.d_guardSize          &&
           lhs.d_inheritSchedule    == rhs.d_inheritSchedule    &&
           lhs.d_schedulingPolicy   == rhs.d_schedulingPolicy   &&
           lhs.d_schedulingPriority == rhs.d_schedulingPriority &&
           lhs.d_stackSize          == rhs.d_stackSize;
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
