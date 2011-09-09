// bcemt_threadutil.cpp                                               -*-C++-*-
#include <bcemt_threadutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_threadutil_cpp,"$Id$ $CSID$")

#include <bdema_managedptr.h>

#include <bslma_allocator.h>
#include <bslma_default.h>

#include <bsl_cmath.h>

#include <bsl_c_limits.h>

namespace BloombergLP {

extern "C" {

void *bcemt_ThreadUtil_threadFunc(void *arg)
    // extern "C" formatted routine which allows us to call a C++ functor
    // through the pthreads interface (which is written in C)
{
    typedef bcemt_ThreadUtil::Invokable Invokable;
    bdema_ManagedPtr<Invokable> functionPtr(
                                          (Invokable *) arg,
                                          ((Invokable *) arg)->getAllocator());
    (*functionPtr)();
    return 0;
}

}  // extern "C"

                            // -----------------------
                            // struct bcemt_ThreadUtil
                            // -----------------------

// CLASS METHODS
int bcemt_ThreadUtil::convertToSchedulingPriority(
         bcemt_ThreadAttributes::SchedulingPolicy policy,
         double                                   normalizedSchedulingPriority)
{
    BSLS_ASSERT_OPT(policy >= bcemt_ThreadAttributes::BCEMT_SCHED_MIN);
    BSLS_ASSERT_OPT(policy <= bcemt_ThreadAttributes::BCEMT_SCHED_MAX);

    BSLS_ASSERT_OPT(normalizedSchedulingPriority >= 0.0);
    BSLS_ASSERT_OPT(normalizedSchedulingPriority <= 1.0);

    const int minPri = getMinSchedulingPriority(policy);
    const int maxPri = getMaxSchedulingPriority(policy);

    // These two asserts should never fail -- just an internal consistency
    // check.

    BSLS_ASSERT(INT_MIN != minPri);
    BSLS_ASSERT(INT_MIN != maxPri);

    double ret = (maxPri - minPri) * normalizedSchedulingPriority +
                                                                  minPri + 0.5;
    return static_cast<int>(bsl::floor(ret));
}

int bcemt_ThreadUtil::create(bcemt_ThreadUtil::Handle           *handle,
                             const bcemt_ThreadAttributes&       attributes,
                             const bcemt_ThreadUtil::Invokable&  function)
{
    bslma_Allocator *alloc = bslma_Default::globalAllocator();

    bdema_ManagedPtr<Invokable> functionPtr(
                               new (*alloc) Invokable(function, alloc), alloc);

    const int rc = create(handle,
                          attributes,
                          bcemt_ThreadUtil_threadFunc,
                          functionPtr.ptr());
    if (0 == rc) {
        functionPtr.release();
    }
    return rc;
}

int bcemt_ThreadUtil::create(bcemt_ThreadUtil::Handle           *handle,
                             const bcemt_ThreadUtil::Invokable&  function)
{
    bslma_Allocator *alloc = bslma_Default::globalAllocator();

    bdema_ManagedPtr<Invokable> functionPtr(
                              new (*alloc) Invokable(function, alloc), alloc);

    const int rc = create(handle,
                          bcemt_ThreadUtil_threadFunc,
                          functionPtr.ptr());
    if (0 == rc) {
        functionPtr.release();
    }
    return rc;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
