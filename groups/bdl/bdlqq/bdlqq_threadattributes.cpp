// bdlqq_threadattributes.cpp                                         -*-C++-*-
#include <bdlqq_threadattributes.h>

#include <bdlqq_platform.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_c_limits.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_threadattributes_cpp,"$Id$ $CSID$")

namespace BloombergLP {

namespace bdlqq {
// Implementation note:  'get[Min|Max]SchedPriority' have been deprecated for
// at least 1 release, and the 4 users have been notified twice.  There appear
// to be no remaining users, but I'm leaving these methods to ensure BDE 2.23
// does not break compilation, they should be removed in BDE 2.24.

// CLASS METHODS
int ThreadAttributes::getMaxSchedPriority(ThreadAttributes::SchedulingPolicy)
{
    return BCEMT_UNSET_PRIORITY;
}

int ThreadAttributes::getMinSchedPriority(ThreadAttributes::SchedulingPolicy)
{
    return BCEMT_UNSET_PRIORITY;
}

// CREATORS
ThreadAttributes::ThreadAttributes()
: d_detachedState(BCEMT_CREATE_JOINABLE)
, d_guardSize(BCEMT_UNSET_GUARD_SIZE)
, d_inheritScheduleFlag(true)
, d_schedulingPolicy(BCEMT_SCHED_DEFAULT)
, d_schedulingPriority(BCEMT_UNSET_PRIORITY)
, d_stackSize(BCEMT_UNSET_STACK_SIZE)
{
}
}  // close package namespace

// FREE OPERATORS
bool bdlqq::operator==(const ThreadAttributes& lhs,
                const ThreadAttributes& rhs)
{
    return lhs.detachedState()      == rhs.detachedState()      &&
           lhs.guardSize()          == rhs.guardSize()          &&
           lhs.inheritSchedule()    == rhs.inheritSchedule()    &&
           lhs.schedulingPolicy()   == rhs.schedulingPolicy()   &&
           lhs.schedulingPriority() == rhs.schedulingPriority() &&
           lhs.stackSize()          == rhs.stackSize();
}

bool bdlqq::operator!=(const ThreadAttributes& lhs,
                const ThreadAttributes& rhs)
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
