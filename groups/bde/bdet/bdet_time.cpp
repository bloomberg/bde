// bdet_time.cpp                                                      -*-C++-*-
#include <bdet_time.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdet_time_cpp,"$Id$ $CSID$")

#include <bdeu_print.h>

#include <bsls_assert.h>

#include <bsl_cstdio.h>    // 'sprintf'
#include <bsl_ostream.h>

// STATIC HELPER FUNCTIONS

static
int fastMod(int *number, int base)
    // Efficiently mod ('%') the specified '*number' by the specified 'base'
    // and store the result back into '*number'; return the value of the
    // original 'number' divided by 'base'.  The behavior is undefined unless
    // '1 <= base'.  Note that, for efficiency, this function uses the native
    // '%' operator which, for an initially negative '*number' may, depending
    // on the platform, have a positive or negative result (whose absolute
    // value is less then 'base'), but in either case will satisfy the
    // constraint:
    //..
    //  *number == *number % base + (*number / base) * base
    //..
{
    BSLS_ASSERT_SAFE(number);
    BSLS_ASSERT_SAFE(1 <= base);

    const int initial = *number;
    const int result  = initial / base;

    *number = initial % base;

    BSLS_ASSERT_SAFE(initial == *number + result * base);

    return result;
}

static
int modulo(int *number, int base)
    // Mod ('%') the specified '*number' by the specified 'base', and store the
    // result back into '*number'; return the value of the original '*number'
    // divided by 'base'.  If the result of the mod operation is negative,
    // adjust both the result of the mod and division such that the result of
    // the mod operation is positive and the results satisfy the constraint:
    //..
    //  *number == *number % base + (*number / base) * base
    //..
    // The behavior is undefined unless '1 <= base'.
{
    BSLS_ASSERT_SAFE(number);
    BSLS_ASSERT_SAFE(1 <= base);

    int result = *number / base;
    *number %= base;

    // Adjust modulus to a positive value, if necessary.

    if (*number < 0) {
        *number += base;
        --result;
    }

    BSLS_ASSERT_SAFE(0 <= *number);
    BSLS_ASSERT_SAFE(     *number < base);

    return result;
}

namespace BloombergLP {

                        // ---------------
                        // class bdet_Time
                        // ---------------

// MANIPULATORS
void bdet_Time::setTime(int hour, int minute, int second, int millisecond)
{
    BSLS_ASSERT(0           <= hour);
    BSLS_ASSERT(hour        <= BDET_HOURS_PER_DAY);
    BSLS_ASSERT(0           <= minute);
    BSLS_ASSERT(minute      <  BDET_MINUTES_PER_HOUR);
    BSLS_ASSERT(0           <= second);
    BSLS_ASSERT(second      <  BDET_SECONDS_PER_MINUTE);
    BSLS_ASSERT(0           <= millisecond);
    BSLS_ASSERT(millisecond <  BDET_MILLISECONDS_PER_SECOND);

    BSLS_ASSERT(hour < BDET_HOURS_PER_DAY
             || (0 == minute && 0 == second && 0 == millisecond));

    d_milliseconds = millisecond + BDET_MILLISECONDS_PER_SECOND *
                       (second   + BDET_SECONDS_PER_MINUTE      *
                         (minute + BDET_MINUTES_PER_HOUR        *
                            hour
                         )
                       );

    BSLS_ASSERT_SAFE(0              <= d_milliseconds);
    BSLS_ASSERT_SAFE(d_milliseconds <= BDET_MILLISECONDS_PER_DAY);
}

void bdet_Time::setHour(int hour)
{
    BSLS_ASSERT(0 <= hour);
    BSLS_ASSERT(     hour <= BDET_HOURS_PER_DAY);

    if (hour < 24) {
        d_milliseconds %= BDET_MILLISECONDS_PER_HOUR;
        d_milliseconds += BDET_MILLISECONDS_PER_HOUR * hour;
    }
    else {
        d_milliseconds = BDET_MILLISECONDS_PER_DAY;
    }

    BSLS_ASSERT_SAFE(0              <= d_milliseconds);
    BSLS_ASSERT_SAFE(d_milliseconds <= BDET_MILLISECONDS_PER_DAY);
}

void bdet_Time::setMinute(int minute)
{
    BSLS_ASSERT(0 <= minute);
    BSLS_ASSERT(     minute < BDET_MINUTES_PER_HOUR);

    if (BDET_MILLISECONDS_PER_DAY == d_milliseconds) {
        d_milliseconds = BDET_MILLISECONDS_PER_MINUTE * minute;
    }
    else {
        int hours = d_milliseconds / BDET_MILLISECONDS_PER_HOUR;
        d_milliseconds %= BDET_MILLISECONDS_PER_MINUTE;
        d_milliseconds += BDET_MILLISECONDS_PER_MINUTE *
                                      (minute + BDET_MINUTES_PER_HOUR * hours);
    }

    BSLS_ASSERT_SAFE(0              <= d_milliseconds);
    BSLS_ASSERT_SAFE(d_milliseconds <= BDET_MILLISECONDS_PER_DAY);
}

void bdet_Time::setSecond(int second)
{
    BSLS_ASSERT(0 <= second);
    BSLS_ASSERT(     second < BDET_SECONDS_PER_MINUTE);

    if (BDET_MILLISECONDS_PER_DAY == d_milliseconds) {
        d_milliseconds = BDET_MILLISECONDS_PER_SECOND * second;
    }
    else {
        int minutes = d_milliseconds / BDET_MILLISECONDS_PER_MINUTE;
        d_milliseconds %= BDET_MILLISECONDS_PER_SECOND;
        d_milliseconds += BDET_MILLISECONDS_PER_SECOND *
                                  (second + BDET_SECONDS_PER_MINUTE * minutes);
    }

    BSLS_ASSERT_SAFE(0              <= d_milliseconds);
    BSLS_ASSERT_SAFE(d_milliseconds <= BDET_MILLISECONDS_PER_DAY);
}

void bdet_Time::setMillisecond(int millisecond)
{
    BSLS_ASSERT(0 <= millisecond);
    BSLS_ASSERT(     millisecond < BDET_MILLISECONDS_PER_SECOND);

    if (BDET_MILLISECONDS_PER_DAY == d_milliseconds) {
        d_milliseconds = millisecond;
    }
    else {
        d_milliseconds /= BDET_MILLISECONDS_PER_SECOND;
        d_milliseconds *= BDET_MILLISECONDS_PER_SECOND;
        d_milliseconds += millisecond;
    }

    BSLS_ASSERT_SAFE(0              <= d_milliseconds);
    BSLS_ASSERT_SAFE(d_milliseconds <= BDET_MILLISECONDS_PER_DAY);
}

int bdet_Time::addTime(int hours, int minutes, int seconds, int milliseconds)
{
    d_milliseconds %= BDET_MILLISECONDS_PER_DAY; // Force zero if 24:00:00.000.

    int wholeDays = fastMod(&hours, BDET_HOURS_PER_DAY);
    d_milliseconds += hours * BDET_MILLISECONDS_PER_HOUR;

    wholeDays += fastMod(&minutes, BDET_MINUTES_PER_DAY);
    d_milliseconds += minutes * BDET_MILLISECONDS_PER_MINUTE;

    wholeDays += fastMod(&seconds, BDET_SECONDS_PER_DAY);
    d_milliseconds += seconds * BDET_MILLISECONDS_PER_SECOND;

    wholeDays += fastMod(&milliseconds, BDET_MILLISECONDS_PER_DAY);
    d_milliseconds += milliseconds;

    return wholeDays + modulo(&d_milliseconds, BDET_MILLISECONDS_PER_DAY);
}

int bdet_Time::addInterval(const bdet_DatetimeInterval& interval)
{
    d_milliseconds %= BDET_MILLISECONDS_PER_DAY; // Force zero if 24:00:00.000.

    // We must perform the 64-bit normalization with temporaries before
    // adjusting the 'int' 'd_milliseconds' value to avoid integer overflow.

    const bsls_PlatformUtil::Int64 totalMsec = interval.totalMilliseconds();

    const int normMsec  =
                       static_cast<int>(totalMsec % BDET_MILLISECONDS_PER_DAY);
    const int wholeDays =
                       static_cast<int>(totalMsec / BDET_MILLISECONDS_PER_DAY);

    return wholeDays + addMilliseconds(normMsec);
}

int bdet_Time::addHours(int hours)
{
    d_milliseconds %= BDET_MILLISECONDS_PER_DAY; // Force zero if 24:00:00.000.

    int wholeDays = fastMod(&hours, BDET_HOURS_PER_DAY);
    d_milliseconds += hours * BDET_MILLISECONDS_PER_HOUR;
    return wholeDays + modulo(&d_milliseconds, BDET_MILLISECONDS_PER_DAY);
}

int bdet_Time::addMinutes(int minutes)
{
    d_milliseconds %= BDET_MILLISECONDS_PER_DAY; // Force zero if 24:00:00.000.

    int wholeDays = fastMod(&minutes, BDET_MINUTES_PER_DAY);
    d_milliseconds += minutes * BDET_MILLISECONDS_PER_MINUTE;
    return wholeDays + modulo(&d_milliseconds, BDET_MILLISECONDS_PER_DAY);
}

int bdet_Time::addSeconds(int seconds)
{
    d_milliseconds %= BDET_MILLISECONDS_PER_DAY; // Force zero if 24:00:00.000.

    int wholeDays = fastMod(&seconds, BDET_SECONDS_PER_DAY);
    d_milliseconds += seconds * BDET_MILLISECONDS_PER_SECOND;
    return wholeDays + modulo(&d_milliseconds, BDET_MILLISECONDS_PER_DAY);
}

int bdet_Time::addMilliseconds(int milliseconds)
{
    d_milliseconds %= BDET_MILLISECONDS_PER_DAY; // Force zero if 24:00:00.000.

    int wholeDays = fastMod(&milliseconds, BDET_MILLISECONDS_PER_DAY);
    d_milliseconds += milliseconds;
    return wholeDays + modulo(&d_milliseconds, BDET_MILLISECONDS_PER_DAY);
}

// ACCESSORS
void bdet_Time::getTime(int *hour,
                        int *minute,
                        int *second,
                        int *millisecond) const
{
    int ms = d_milliseconds;
    int hr = ms / BDET_MILLISECONDS_PER_HOUR;
    ms %= BDET_MILLISECONDS_PER_HOUR;

    if (hour) {
        *hour = hr;
    }

    int min = ms / BDET_MILLISECONDS_PER_MINUTE;
    ms %= BDET_MILLISECONDS_PER_MINUTE;

    if (minute) {
        *minute = min;
    }

    int sec = ms / BDET_MILLISECONDS_PER_SECOND;

    if (second) {
        *second = sec;
    }

    if (millisecond) {
        *millisecond = ms % BDET_MILLISECONDS_PER_SECOND;
    }
}

bsl::ostream& bdet_Time::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;
    }

    //      Format: HH: MM: SS. mmm '\0'
    const int SIZE = 3 + 3 + 3 + 3 + 1;
    char buffer[SIZE];

    int hour, min, sec, mSec;

    getTime(&hour, &min, &sec, &mSec);

    //              Format: HH:  MM:  SS. mmm '\0'
    bsl::sprintf(buffer, "%02d:%02d:%02d.%03d", hour, min, sec, mSec);

    if (level < 0) {
        level = -level;
    }
    else {
        bdeu_Print::indent(stream, level, spacesPerLevel);
    }

    stream << buffer;

    if (spacesPerLevel >= 0) {
        stream << '\n';
    }

    return stream;
}

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const bdet_Time& time)
{
    return time.print(stream, 0, -1);
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
