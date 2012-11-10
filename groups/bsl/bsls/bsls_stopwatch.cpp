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

// ----------------------------------------------------------------------------
// Copyright (C) 2012 Bloomberg L.P.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------- END-OF-FILE ----------------------------------
