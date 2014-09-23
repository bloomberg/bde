// bsls_stopwatch.cpp                                                 -*-C++-*-
#include <bsls_stopwatch.h>

#include <bsls_bsltestutil.h>  // for testing only
#include <bsls_platform.h>     // for testing only

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

// ----------------------------------------------------------------------------
// Copyright 2013 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
