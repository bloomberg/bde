// bdet_timeinterval.cpp      -*-C++-*-
#include <bdet_timeinterval.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_timeinterval_cpp,"$Id$ $CSID$")


#include <bsls_platform.h>

#include <bsl_ostream.h>

namespace BloombergLP {

struct bdet_TimeInterval_Assertions {
    char assertion1[-3 / 2 == -1];  // insure platform has the "right"
    char assertion2[-5 % 4 == -1];  // implementation
};

// CREATORS
bdet_TimeInterval::bdet_TimeInterval(bsls_PlatformUtil::Int64 seconds,
                                     int                      nanoseconds)
{
    d_seconds = seconds;
    if (nanoseconds >= BDET_NANOSECS_PER_SEC
     || nanoseconds <= -BDET_NANOSECS_PER_SEC) {
        d_seconds += nanoseconds / BDET_NANOSECS_PER_SEC;
        d_nanoseconds = int(nanoseconds % BDET_NANOSECS_PER_SEC);
    }
    else {
        d_nanoseconds = (int) nanoseconds;
    }

    if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= BDET_NANOSECS_PER_SEC;
    }
}
bdet_TimeInterval::bdet_TimeInterval(double seconds)
{
    if (seconds < 0) {
        d_seconds = -bsls_PlatformUtil::Int64(-seconds);
#ifdef BSLS_PLATFORM__OS_LINUX
        const volatile double nanoseconds =
#else
        const double nanoseconds =
#endif
                              ((double)d_seconds - seconds) * BDET_NANOSECS_PER_SEC;
        d_nanoseconds = -(int)(nanoseconds + 0.5);
        if (d_nanoseconds == -BDET_NANOSECS_PER_SEC) {
            --d_seconds;
            d_nanoseconds = 0;
        }
    }
    else {
        d_seconds = (bsls_PlatformUtil::Int64)seconds;
#ifdef BSLS_PLATFORM__OS_LINUX
        const volatile double nanoseconds =
#else
        const double nanoseconds =
#endif
                              (seconds - (double)d_seconds) * BDET_NANOSECS_PER_SEC;
        d_nanoseconds = (int)(nanoseconds + 0.5);
        if (d_nanoseconds == BDET_NANOSECS_PER_SEC) {
            ++d_seconds;
            d_nanoseconds = 0;
        }
    }
}

// MANIPULATORS
bdet_TimeInterval& bdet_TimeInterval::operator-=(const bdet_TimeInterval& rhs)
{
    d_seconds -= rhs.d_seconds;
    d_nanoseconds -= rhs.d_nanoseconds;

    if (d_nanoseconds >= BDET_NANOSECS_PER_SEC ||
        d_nanoseconds <= -BDET_NANOSECS_PER_SEC) {
        d_seconds += d_nanoseconds / BDET_NANOSECS_PER_SEC;
        d_nanoseconds %= BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= BDET_NANOSECS_PER_SEC;
    }

    return *this;
}

bdet_TimeInterval& bdet_TimeInterval::operator+=(const bdet_TimeInterval& rhs)
{
    d_seconds += rhs.d_seconds;
    d_nanoseconds += rhs.d_nanoseconds;

    if (d_nanoseconds >= BDET_NANOSECS_PER_SEC ||
        d_nanoseconds <= -BDET_NANOSECS_PER_SEC) {
        d_seconds += d_nanoseconds / BDET_NANOSECS_PER_SEC;
        d_nanoseconds %= BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= BDET_NANOSECS_PER_SEC;
    }

    return *this;
}

bdet_TimeInterval&
bdet_TimeInterval::addSeconds(bsls_PlatformUtil::Int64 seconds)
{
    d_seconds += seconds;
    if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= BDET_NANOSECS_PER_SEC;
    }

    return *this;
}

bdet_TimeInterval&
bdet_TimeInterval::addMilliseconds(bsls_PlatformUtil::Int64 milliseconds)
{
    d_seconds += milliseconds / BDET_MILLISECS_PER_SEC;
    d_nanoseconds += int(milliseconds % BDET_MILLISECS_PER_SEC) * BDET_MICROSECS_PER_SEC;

    if (d_nanoseconds >= BDET_NANOSECS_PER_SEC ||
        d_nanoseconds <= -BDET_NANOSECS_PER_SEC) {
        d_seconds += d_nanoseconds / BDET_NANOSECS_PER_SEC;
        d_nanoseconds %= BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= BDET_NANOSECS_PER_SEC;
    }

    return *this;
}

bdet_TimeInterval&
bdet_TimeInterval::addMicroseconds(bsls_PlatformUtil::Int64 microseconds)
{
    d_seconds += microseconds / BDET_MICROSECS_PER_SEC;
    d_nanoseconds += int(microseconds % BDET_MICROSECS_PER_SEC) * BDET_MILLISECS_PER_SEC;

    if (d_nanoseconds >= BDET_NANOSECS_PER_SEC ||
        d_nanoseconds <= -BDET_NANOSECS_PER_SEC) {
        d_seconds += d_nanoseconds / BDET_NANOSECS_PER_SEC;
        d_nanoseconds %= BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= BDET_NANOSECS_PER_SEC;
    }

    return *this;
}

bdet_TimeInterval&
bdet_TimeInterval::addNanoseconds(bsls_PlatformUtil::Int64 nanoseconds)
{
    d_seconds += nanoseconds / BDET_NANOSECS_PER_SEC;
    d_nanoseconds += int(nanoseconds % BDET_NANOSECS_PER_SEC);

    if (d_nanoseconds >= BDET_NANOSECS_PER_SEC ||
        d_nanoseconds <= -BDET_NANOSECS_PER_SEC) {
        d_seconds += d_nanoseconds / BDET_NANOSECS_PER_SEC;
        d_nanoseconds %= BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= BDET_NANOSECS_PER_SEC;
    }

    return *this;
}

void bdet_TimeInterval::addInterval(bsls_PlatformUtil::Int64 seconds,
                                    int                      nanoseconds)
{
    d_seconds += seconds;
    bsls_PlatformUtil::Int64 nanosecs = d_nanoseconds;
    nanosecs += nanoseconds;

    if (nanosecs >= BDET_NANOSECS_PER_SEC || nanosecs <= -BDET_NANOSECS_PER_SEC) {
        d_seconds += nanosecs / BDET_NANOSECS_PER_SEC;
        d_nanoseconds = int(nanosecs % BDET_NANOSECS_PER_SEC);
    }
    else {
        d_nanoseconds = (int) nanosecs;
    }

    if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= BDET_NANOSECS_PER_SEC;
    }
}

void bdet_TimeInterval::setInterval(bsls_PlatformUtil::Int64 seconds,
                                    int                      nanoseconds)
{
    d_seconds = seconds;
    if (nanoseconds >= BDET_NANOSECS_PER_SEC || nanoseconds <= -BDET_NANOSECS_PER_SEC) {
        d_seconds += nanoseconds / BDET_NANOSECS_PER_SEC;
        d_nanoseconds = int(nanoseconds % BDET_NANOSECS_PER_SEC);
    }
    else {
        d_nanoseconds = (int) nanoseconds;
    }

    if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += BDET_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= BDET_NANOSECS_PER_SEC;
    }
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream&            output,
                         const bdet_TimeInterval& timeinterval)
{
    output << '(' << timeinterval.seconds()     << ", "
                  << timeinterval.nanoseconds() << ')';

    return output;
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
