// bcemt_meteredmutex.cpp                                             -*-C++-*-
#include <bcemt_meteredmutex.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_meteredmutex_cpp,"$Id$ $CSID$")

#include <bcemt_barrier.h>  // for testing only

namespace BloombergLP {

                           // ------------------------
                           // class bcemt_MeteredMutex
                           // ------------------------

// MANIPULATORS
void bcemt_MeteredMutex::resetMetrics()
{
    // Loop is used here to ensure that 'd_lastResetTime' is not
    // modified (by some other thread) between the computation
    // of 't1' and the modification of d_lastResetTime by 'testAndSwap'.

    bsls_PlatformUtil::Int64 t1, old;

    d_holdTime = 0;
    d_waitTime = 0;
    do {
        old = d_lastResetTime;
        t1 = bsls_TimeUtil::getTimer();
    } while(d_lastResetTime.testAndSwap(old, t1) != old);
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
