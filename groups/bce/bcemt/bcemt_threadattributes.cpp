// bcemt_threadattributes.cpp                                         -*-C++-*-
#include <bcemt_threadattributes.h>

#include <bces_platform.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_limits.h>

#if defined(BCES_PLATFORM__POSIX_THREADS)
# include <pthread.h>
# if defined(BSLS_PLATFORM__OS_AIX)
#   include <sys/types.h>
#   include <unistd.h>
# endif
#elif !defined(BCES_PLATFORM__WIN32_THREADS)
# error unrecognized threading platform
#endif

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_threadattributes_cpp,"$Id$ $CSID$")

namespace BloombergLP {

// CLASS METHODS
int bcemt_ThreadAttributes::getMaxSchedPriority(int policy)
{
#if defined(BCES_PLATFORM__POSIX_THREADS)
    if (-1 == policy) {
        policy = sched_getscheduler(0);
    }

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
      case BCEMT_SCHED_DEFAULT: {
#if defined(BSLS_PLATFORM__OS_HPUX)
        policy = SCHED_HPUX;
#else
        policy = SCHED_OTHER;
#endif
      } break;
      default: {
        return bsl::numeric_limits<int>::min();                       // RETURN
      }
    }

    int pri = sched_get_priority_max(policy);

# if defined(BSLS_PLATFORM__OS_AIX)
    // Note max prirority returned is 127 regardless of policy on AIX

    enum { MAX_NON_SUPERUSER_PRIORITY = 60 };

    if (pri > MAX_NON_SUPERUSER_PRIORITY && 0 != geteuid()) {
        pri = MAX_NON_SUPERUSER_PRIORITY;
    }

# endif

    return pri;
#else
    return -1;
#endif
}

int bcemt_ThreadAttributes::getMinSchedPriority(int policy)
{
#if defined(BCES_PLATFORM__POSIX_THREADS)
    if (-1 == policy) {
        policy = sched_getscheduler(0);
    }

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
      case BCEMT_SCHED_DEFAULT: {
#if defined(BSLS_PLATFORM__OS_HPUX)
        policy = SCHED_HPUX;
#else
        policy = SCHED_OTHER;
#endif
      } break;
      default: {
        return bsl::numeric_limits<int>::min();                       // RETURN
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
, d_guardSize(BCEMT_UNSET_GUARD_SIZE)
, d_inheritSchedule(1)
, d_schedulingPolicy(BCEMT_SCHED_DEFAULT)
, d_schedulingPriority(BCEMT_UNSET_PRIORITY)
, d_stackSize(BCEMT_UNSET_STACK_SIZE)
{}

// FREE OPERATORS
bool operator==(const bcemt_ThreadAttributes& lhs,
                const bcemt_ThreadAttributes& rhs)
{
    return lhs.d_detachedState              == rhs.d_detachedState           &&
           lhs.d_guardSize                  == rhs.d_guardSize               &&
           lhs.d_inheritSchedule            == rhs.d_inheritSchedule         &&
           lhs.d_schedulingPolicy           == rhs.d_schedulingPolicy        &&
           lhs.d_schedulingPriority         == rhs.d_schedulingPriority      &&
           lhs.d_stackSize               == rhs.d_stackSize;
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
