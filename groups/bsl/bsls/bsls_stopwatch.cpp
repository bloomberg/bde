// bsls_stopwatch.cpp                                                 -*-C++-*-
#include <bsls_stopwatch.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

                           // --------------------
                           // class bsls_Stopwatch
                           // --------------------

// CLASS DATA
const double bsls_Stopwatch::s_nanosecondsPerSecond = 1.0E9;

// PRIVATE MANIPULATORS
void bsls_Stopwatch::updateTimes()
{
    bsls_Types::Int64 systemTime;
    bsls_Types::Int64 userTime;
    bsls_Types::Int64 wallTime;
    accumulatedTimesRaw(&systemTime, &userTime, &wallTime);

    d_accumulatedSystemTime += systemTime - d_startSystemTime;
    d_accumulatedUserTime   += userTime   - d_startUserTime;
    d_accumulatedWallTime   += wallTime   - d_startWallTime;
}

// ACCESSORS
void bsls_Stopwatch::accumulatedTimes(double *systemTime,
                                      double *userTime,
                                      double *wallTime) const
{
    if (d_isRunning) {
        bsls_Types::Int64 rawSystemTime;
        bsls_Types::Int64 rawUserTime;
        bsls_Types::Int64 rawWallTime;
        accumulatedTimesRaw(&rawSystemTime, &rawUserTime, &rawWallTime);

        *systemTime = (d_accumulatedSystemTime + rawSystemTime
                                 - d_startSystemTime) / s_nanosecondsPerSecond;
        *userTime   = (d_accumulatedUserTime   + rawUserTime
                                 - d_startUserTime)   / s_nanosecondsPerSecond;
        *wallTime   = (d_accumulatedWallTime   + rawWallTime
                                 - d_startWallTime)   / s_nanosecondsPerSecond;
    }
    else {
        *systemTime = d_accumulatedSystemTime / s_nanosecondsPerSecond;
        *userTime   = d_accumulatedUserTime   / s_nanosecondsPerSecond;
        *wallTime   = d_accumulatedWallTime   / s_nanosecondsPerSecond;
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
