// bcemt_threadattributes.cpp                                         -*-C++-*-
#include <bcemt_threadattributes.h>

#include <bces_platform.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_c_limits.h>

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
int bcemt_ThreadAttributes::getMaxSchedPriority(
                               bcemt_ThreadAttributes::SchedulingPolicy policy)
{
#if defined(BCES_PLATFORM__POSIX_THREADS)
    int pPolicy;

    switch (policy) {
      case BCEMT_SCHED_FIFO: {
        pPolicy = SCHED_FIFO;
      }  break;
      case BCEMT_SCHED_RR: {
        pPolicy = SCHED_RR;
      }  break;
      case BCEMT_SCHED_OTHER: {
        pPolicy = SCHED_OTHER;
      }  break;
      case BCEMT_SCHED_DEFAULT: {
#if defined(BSLS_PLATFORM__OS_HPUX)
        pPolicy = SCHED_HPUX;
#else
        pPolicy = SCHED_OTHER;
#endif
      }  break;
      default: {
        BSLS_ASSERT_OPT(0);
      }
    }

    int priority = sched_get_priority_max(pPolicy);

# if defined(BSLS_PLATFORM__OS_AIX)
    // Note max prirority returned above is 127 regardless of policy on AIX,
    // yet for non-superusers, thread creation fails if 'priority > 60'.  See
    // AIX doc "http://publib.boulder.ibm.com/infocenter/aix/v6r1/index.jsp?
    // topic=%2Fcom.ibm.aix.basetechref%2Fdoc%2Fbasetrf1%2F
    // pthread_setschedparam.htm"

    enum { MAX_AIX_NON_ROOT_PRIORITY = 60,
           MAX_AIX_PRIORITY          = 80 };

    if (0 == geteuid()) {
        // priviledged user

        // On AIX 5.3 and above, all priorities above 80 are equivalent to 80.

        if (priority > MAX_AIX_PRIORITY) {
            priority = MAX_AIX_PRIORITY;
        }
    }
    else {
        // non-priviledged user

        if (priority > MAX_AIX_NON_ROOT_PRIORITY) {
            priority = MAX_AIX_NON_ROOT_PRIORITY;
        }
    }

# endif

    return priority;
#else
    return -1;
#endif
}

int bcemt_ThreadAttributes::getMinSchedPriority(
                               bcemt_ThreadAttributes::SchedulingPolicy policy)
{
#if defined(BCES_PLATFORM__POSIX_THREADS)
    int pPolicy;

    switch (policy) {
      case BCEMT_SCHED_FIFO: {
        pPolicy = SCHED_FIFO;
      }  break;
      case BCEMT_SCHED_RR: {
        pPolicy = SCHED_RR;
      }  break;
      case BCEMT_SCHED_OTHER: {
        pPolicy = SCHED_OTHER;
      }  break;
      case BCEMT_SCHED_DEFAULT: {
#if defined(BSLS_PLATFORM__OS_HPUX)
        pPolicy = SCHED_HPUX;
#else
        pPolicy = SCHED_OTHER;
#endif
      }  break;
      default: {
        BSLS_ASSERT_OPT(0);
      }
    }

    int priority = sched_get_priority_min(pPolicy);

# if defined(BSLS_PLATFORM__OS_AIX)
    // Note on AIX all priorities below 40 are equivalent to a priority of 40.
    // See AIX doc "http://publib.boulder.ibm.com/infocenter/aix/v6r1/
    // index.jsp?topic=%2Fcom.ibm.aix.basetechref%2Fdoc%2Fbasetrf1%2F
    // pthread_setschedparam.htm"

    enum { MIN_AIX_PRIORITY = 40 };

    if (priority < MIN_AIX_PRIORITY) {
        priority = MIN_AIX_PRIORITY;
    }

# endif

    return priority;
#else
    return -1;
#endif
}

// CREATORS
bcemt_ThreadAttributes::bcemt_ThreadAttributes()
: d_detachedState(BCEMT_CREATE_JOINABLE)
, d_guardSize(BCEMT_UNSET_GUARD_SIZE)
, d_inheritScheduleFlag(true)
, d_schedulingPolicy(BCEMT_SCHED_DEFAULT)
, d_schedulingPriority(BCEMT_UNSET_PRIORITY)
, d_stackSize(BCEMT_UNSET_STACK_SIZE)
{}

// FREE OPERATORS
bool operator==(const bcemt_ThreadAttributes& lhs,
                const bcemt_ThreadAttributes& rhs)
{
    return lhs.detachedState()      == rhs.detachedState()      &&
           lhs.guardSize()          == rhs.guardSize()          &&
           lhs.inheritSchedule()    == rhs.inheritSchedule()    &&
           lhs.schedulingPolicy()   == rhs.schedulingPolicy()   &&
           lhs.schedulingPriority() == rhs.schedulingPriority() &&
           lhs.stackSize()          == rhs.stackSize();
}

bool operator!=(const bcemt_ThreadAttributes& lhs,
                const bcemt_ThreadAttributes& rhs)
{
    return lhs.detachedState()      != rhs.detachedState()      ||
           lhs.guardSize()          != rhs.guardSize()          ||
           lhs.inheritSchedule()    != rhs.inheritSchedule()    ||
           lhs.schedulingPolicy()   != rhs.schedulingPolicy()   ||
           lhs.schedulingPriority() != rhs.schedulingPriority() ||
           lhs.stackSize()          != rhs.stackSize();
}

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
