// bdlqq_threadutil.cpp                                               -*-C++-*-
#include <bdlqq_threadutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_threadutil_cpp,"$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslma_managedptr.h>

#include <bsls_platform.h>

#include <bsl_cmath.h>

#include <bsl_c_limits.h>

namespace BloombergLP {

extern "C" {

void *bcemt_ThreadUtil_threadFunc(void *arg)
    // extern "C" formatted routine which allows us to call a C++ functor
    // through the pthreads interface (which is written in C)
{
    typedef bdlqq::ThreadUtil::Invokable Invokable;
    bslma::ManagedPtr<Invokable> functionPtr(
                                          (Invokable *) arg,
                                          ((Invokable *) arg)->getAllocator());
    (*functionPtr)();
    return 0;
}

}  // extern "C"

namespace bdlqq {
                            // -----------------------
                            // struct ThreadUtil
                            // -----------------------

// CLASS METHODS
int ThreadUtil::convertToSchedulingPriority(
         ThreadAttributes::SchedulingPolicy policy,
         double                                   normalizedSchedulingPriority)
{
    BSLS_ASSERT_OPT((int) policy >= ThreadAttributes::BCEMT_SCHED_MIN);
    BSLS_ASSERT_OPT((int) policy <= ThreadAttributes::BCEMT_SCHED_MAX);

    BSLS_ASSERT_OPT(normalizedSchedulingPriority >= 0.0);
    BSLS_ASSERT_OPT(normalizedSchedulingPriority <= 1.0);

    const int minPri = getMinSchedulingPriority(policy);
    const int maxPri = getMaxSchedulingPriority(policy);

    if (minPri == ThreadAttributes::BCEMT_UNSET_PRIORITY ||
        maxPri == ThreadAttributes::BCEMT_UNSET_PRIORITY) {
        return ThreadAttributes::BCEMT_UNSET_PRIORITY;
    }

#if !defined(BSLS_PLATFORM_OS_CYGWIN)
    double ret = (maxPri - minPri) * normalizedSchedulingPriority +
                                                                  minPri + 0.5;
#else
    // On Cygwin, a lower numerical value implies a higher thread priority:
    //   minSchedPriority = 15, maxSchedPriority = -14

    double ret = - ((minPri - maxPri) * normalizedSchedulingPriority - minPri)
                                                                         + 0.5;
#endif

    return static_cast<int>(bsl::floor(ret));
}

int ThreadUtil::create(ThreadUtil::Handle           *handle,
                             const ThreadAttributes&       attributes,
                             const ThreadUtil::Invokable&  function,
                             bslma::Allocator                   *allocator)
{
    if (!allocator) {
        allocator = bslma::Default::globalAllocator();
    }

    bslma::ManagedPtr<Invokable> functionPtr(
                   new (*allocator) Invokable(function, allocator), allocator);

    const int rc = create(handle,
                          attributes,
                          bcemt_ThreadUtil_threadFunc,
                          functionPtr.ptr());
    if (0 == rc) {
        functionPtr.release();
    }
    return rc;
}

int ThreadUtil::create(ThreadUtil::Handle           *handle,
                             const ThreadUtil::Invokable&  function,
                             bslma::Allocator                   *allocator)
{
    if (!allocator) {
        allocator = bslma::Default::globalAllocator();
    }

    bslma::ManagedPtr<Invokable> functionPtr(
                   new (*allocator) Invokable(function, allocator), allocator);

    const int rc = create(handle,
                          bcemt_ThreadUtil_threadFunc,
                          functionPtr.ptr());
    if (0 == rc) {
        functionPtr.release();
    }
    return rc;
}
}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
