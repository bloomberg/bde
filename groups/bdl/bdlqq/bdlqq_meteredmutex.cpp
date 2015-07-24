// bdlqq_meteredmutex.cpp                                             -*-C++-*-
#include <bdlqq_meteredmutex.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlqq_meteredmutex_cpp,"$Id$ $CSID$")

#include <bdlqq_barrier.h>  // for testing only

#include <bsls_types.h>

namespace BloombergLP {

namespace bdlqq {
                           // ------------------------
                           // class MeteredMutex
                           // ------------------------

// MANIPULATORS
void MeteredMutex::resetMetrics()
{
    // Loop is used here to ensure that 'd_lastResetTime' is not
    // modified (by some other thread) between the computation
    // of 't1' and the modification of d_lastResetTime by 'testAndSwap'.

    bsls::Types::Int64 t1, old;

    d_holdTime = 0;
    d_waitTime = 0;
    do {
        old = d_lastResetTime;
        t1 = bsls::TimeUtil::getTimer();
    } while(d_lastResetTime.testAndSwap(old, t1) != old);
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
