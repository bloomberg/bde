// bsls_stopwatch.cpp                                                 -*-C++-*-
#include <bsls_stopwatch.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

namespace BloombergLP {

namespace bsls {

                               // ---------------
                               // class Stopwatch
                               // ---------------

// CLASS DATA
const double Stopwatch::s_nanosecondsPerSecond = 1.0E9;

// PRIVATE MANIPULATORS
void Stopwatch::updateTimes()
{
    Types::Int64 systemTime;
    Types::Int64 userTime;
    TimeUtil::OpaqueNativeTime wallTime;
    accumulatedTimesRaw(&systemTime, &userTime, &wallTime);

    d_accumulatedSystemTime += systemTime - d_startSystemTime;
    d_accumulatedUserTime   += userTime   - d_startUserTime;
    d_accumulatedWallTime   += elapsedWallTime(wallTime);
}

// ACCESSORS
void Stopwatch::accumulatedTimes(double *systemTime,
                                 double *userTime,
                                 double *wallTime) const
{
    if (d_isRunning) {
        Types::Int64 rawSystemTime;
        Types::Int64 rawUserTime;
        TimeUtil::OpaqueNativeTime rawWallTime;
        accumulatedTimesRaw(&rawSystemTime, &rawUserTime, &rawWallTime);

        *systemTime = static_cast<double>(
                   d_accumulatedSystemTime + rawSystemTime - d_startSystemTime)
                                                      / s_nanosecondsPerSecond;
        *userTime   = static_cast<double>(
                     d_accumulatedUserTime + rawUserTime   - d_startUserTime)
                                                      / s_nanosecondsPerSecond;
        *wallTime   = static_cast<double>(
                     d_accumulatedWallTime + elapsedWallTime(rawWallTime))
                                                      / s_nanosecondsPerSecond;
    }
    else {
        *systemTime = static_cast<double>(d_accumulatedSystemTime)
                                                      / s_nanosecondsPerSecond;
        *userTime   = static_cast<double>(d_accumulatedUserTime)
                                                      / s_nanosecondsPerSecond;
        *wallTime   = static_cast<double>(d_accumulatedWallTime)
                                                      / s_nanosecondsPerSecond;
    }
}

}  // close package namespace

}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
