// bdlt_time.cpp                                                      -*-C++-*-
#include <bdlt_time.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_time_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsls_assert.h>

#include <bsl_cstdio.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

namespace BloombergLP {
namespace bdlt {

// STATIC HELPER FUNCTIONS

static
bsls::Types::Int64 fastMod(int *number, int base)
    // Efficiently mod ('%') the specified 'number' by the specified 'base' and
    // store the result back into 'number'; return the value of the original
    // 'number' divided by 'base'.  The behavior is undefined unless
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
bsls::Types::Int64 fastMod(bsls::Types::Int64 *number, bsls::Types::Int64 base)
    // Efficiently mod ('%') the specified 'number' by the specified 'base' and
    // store the result back into 'number'; return the value of the original
    // 'number' divided by 'base'.  The behavior is undefined unless
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

    const bsls::Types::Int64 initial = *number;
    const bsls::Types::Int64 result  = initial / base;

    *number = initial % base;

    BSLS_ASSERT_SAFE(initial == *number + result * base);

    return result;
}

static
bsls::Types::Int64 modulo(bsls::Types::Int64 *number, bsls::Types::Int64 base)
    // Mod ('%') the specified 'number' by the specified 'base', and store the
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

    bsls::Types::Int64 result = *number / base;
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

                                // ----------
                                // class Time
                                // ----------

// CLASS DATA
bsls::AtomicInt64 Time::s_invalidRepresentationCount(0);

// MANIPULATORS
int Time::addHours(int hours)
{
    bsls::Types::Int64 totalMicroseconds =
                        microsecondsFromMidnight() % TimeUnitRatio::k_US_PER_D;
                                              // Force zero if 24:00:00.000000.

    bsls::Types::Int64 wholeDays = fastMod(&hours,
                                           TimeUnitRatio::k_H_PER_D_32);

    totalMicroseconds += TimeUnitRatio::k_US_PER_H * hours;
    wholeDays         += modulo(&totalMicroseconds, TimeUnitRatio::k_US_PER_D);

    setMicrosecondsFromMidnight(totalMicroseconds);

    return static_cast<int>(wholeDays);
}

int Time::addMinutes(int minutes)
{
    bsls::Types::Int64 totalMicroseconds =
                        microsecondsFromMidnight() % TimeUnitRatio::k_US_PER_D;
                                              // Force zero if 24:00:00.000000.

    bsls::Types::Int64 wholeDays = fastMod(&minutes,
                                           TimeUnitRatio::k_M_PER_D_32);

    totalMicroseconds += TimeUnitRatio::k_US_PER_M * minutes;
    wholeDays         += modulo(&totalMicroseconds, TimeUnitRatio::k_US_PER_D);

    setMicrosecondsFromMidnight(totalMicroseconds);

    return static_cast<int>(wholeDays);
}

int Time::addSeconds(int seconds)
{
    bsls::Types::Int64 totalMicroseconds =
                        microsecondsFromMidnight() % TimeUnitRatio::k_US_PER_D;
                                              // Force zero if 24:00:00.000000.

    bsls::Types::Int64 wholeDays = fastMod(&seconds,
                                           TimeUnitRatio::k_S_PER_D_32);

    totalMicroseconds += TimeUnitRatio::k_US_PER_S * seconds;
    wholeDays         += modulo(&totalMicroseconds, TimeUnitRatio::k_US_PER_D);

    setMicrosecondsFromMidnight(totalMicroseconds);

    return static_cast<int>(wholeDays);
}

int Time::addMilliseconds(int milliseconds)
{
    bsls::Types::Int64 totalMicroseconds =
                        microsecondsFromMidnight() % TimeUnitRatio::k_US_PER_D;
                                              // Force zero if 24:00:00.000000.

    bsls::Types::Int64 wholeDays = fastMod(&milliseconds,
                                           TimeUnitRatio::k_MS_PER_D_32);

    totalMicroseconds += TimeUnitRatio::k_US_PER_MS * milliseconds;
    wholeDays         += modulo(&totalMicroseconds, TimeUnitRatio::k_US_PER_D);

    setMicrosecondsFromMidnight(totalMicroseconds);

    return static_cast<int>(wholeDays);
}

int Time::addMicroseconds(bsls::Types::Int64 microseconds)
{
    bsls::Types::Int64 totalMicroseconds =
                        microsecondsFromMidnight() % TimeUnitRatio::k_US_PER_D;
                                              // Force zero if 24:00:00.000000.

    bsls::Types::Int64 wholeDays = fastMod(&microseconds,
                                           TimeUnitRatio::k_US_PER_D);

    totalMicroseconds += microseconds;
    wholeDays         += modulo(&totalMicroseconds, TimeUnitRatio::k_US_PER_D);

    setMicrosecondsFromMidnight(totalMicroseconds);

    return static_cast<int>(wholeDays);
}

int Time::addInterval(const DatetimeInterval& interval)
{
    bsls::Types::Int64 totalMicroseconds =
                        microsecondsFromMidnight() % TimeUnitRatio::k_US_PER_D;
                                              // Force zero if 24:00:00.000000.

    bsls::Types::Int64 totalMilliseconds = interval.totalMilliseconds();
    bsls::Types::Int64 wholeDays = fastMod(&totalMilliseconds,
                                           TimeUnitRatio::k_MS_PER_D);

    totalMicroseconds += TimeUnitRatio::k_US_PER_MS * totalMilliseconds;
    wholeDays         += modulo(&totalMicroseconds, TimeUnitRatio::k_US_PER_D);

    setMicrosecondsFromMidnight(totalMicroseconds);

    BSLS_ASSERT(bsl::numeric_limits<int>::min() <= wholeDays);
    BSLS_ASSERT(bsl::numeric_limits<int>::max() >= wholeDays);

    return static_cast<int>(wholeDays);
}

int Time::addTime(int                hours,
                  int                minutes,
                  int                seconds,
                  int                milliseconds,
                  bsls::Types::Int64 microseconds)
{
    bsls::Types::Int64 totalMicroseconds =
                        microsecondsFromMidnight() % TimeUnitRatio::k_US_PER_D;
                                              // Force zero if 24:00:00.000000.

    bsls::Types::Int64 wholeDays = fastMod(&hours,
                                           TimeUnitRatio::k_H_PER_D);

    totalMicroseconds += TimeUnitRatio::k_US_PER_H * hours;

    wholeDays         += fastMod(&minutes, TimeUnitRatio::k_M_PER_D);
    totalMicroseconds += TimeUnitRatio::k_US_PER_M * minutes;

    wholeDays         += fastMod(&seconds, TimeUnitRatio::k_S_PER_D);
    totalMicroseconds += TimeUnitRatio::k_US_PER_S * seconds;

    wholeDays         += fastMod(&milliseconds, TimeUnitRatio::k_MS_PER_D);
    totalMicroseconds += TimeUnitRatio::k_US_PER_MS * milliseconds;

    wholeDays         += fastMod(&microseconds, TimeUnitRatio::k_US_PER_D);
    totalMicroseconds += microseconds;

    wholeDays         += modulo(&totalMicroseconds, TimeUnitRatio::k_US_PER_D);

    setMicrosecondsFromMidnight(totalMicroseconds);

    return static_cast<int>(wholeDays);
}

void Time::setHour(int hour)
{
    BSLS_ASSERT(0 <= hour);
    BSLS_ASSERT(     hour <= TimeUnitRatio::k_H_PER_D_32);

    bsls::Types::Int64 totalMicroseconds = microsecondsFromMidnight();

    if (hour < TimeUnitRatio::k_H_PER_D_32) {
        totalMicroseconds %= TimeUnitRatio::k_US_PER_H;
        totalMicroseconds += TimeUnitRatio::k_US_PER_H * hour;

        setMicrosecondsFromMidnight(totalMicroseconds);
    }
    else {
        setMicrosecondsFromMidnight(TimeUnitRatio::k_US_PER_D);
    }
}

void Time::setMinute(int minute)
{
    BSLS_ASSERT(0 <= minute);
    BSLS_ASSERT(     minute < TimeUnitRatio::k_M_PER_H_32);

    bsls::Types::Int64 totalMicroseconds = microsecondsFromMidnight();

    if (totalMicroseconds < TimeUnitRatio::k_US_PER_D) {
        bsls::Types::Int64 hours =
                                 totalMicroseconds / TimeUnitRatio::k_US_PER_H;

        totalMicroseconds %= TimeUnitRatio::k_US_PER_M;
        totalMicroseconds += TimeUnitRatio::k_US_PER_H * hours
                          +  TimeUnitRatio::k_US_PER_M * minute;

        setMicrosecondsFromMidnight(totalMicroseconds);
    }
    else {
        setMicrosecondsFromMidnight(TimeUnitRatio::k_US_PER_M * minute);
    }
}

void Time::setSecond(int second)
{
    BSLS_ASSERT(0 <= second);
    BSLS_ASSERT(     second < TimeUnitRatio::k_S_PER_M_32);

    bsls::Types::Int64 totalMicroseconds = microsecondsFromMidnight();

    if (totalMicroseconds < TimeUnitRatio::k_US_PER_D) {
        bsls::Types::Int64 minutes =
                                 totalMicroseconds / TimeUnitRatio::k_US_PER_M;

        totalMicroseconds %= TimeUnitRatio::k_US_PER_S;
        totalMicroseconds += TimeUnitRatio::k_US_PER_M * minutes
                          +  TimeUnitRatio::k_US_PER_S * second;

        setMicrosecondsFromMidnight(totalMicroseconds);
    }
    else {
        setMicrosecondsFromMidnight(TimeUnitRatio::k_US_PER_S * second);
    }
}

void Time::setMillisecond(int millisecond)
{
    BSLS_ASSERT(0 <= millisecond);
    BSLS_ASSERT(     millisecond < TimeUnitRatio::k_MS_PER_S_32);

    bsls::Types::Int64 totalMicroseconds = microsecondsFromMidnight();

    if (totalMicroseconds < TimeUnitRatio::k_US_PER_D) {
        bsls::Types::Int64 seconds =
                                 totalMicroseconds / TimeUnitRatio::k_US_PER_S;

        totalMicroseconds %= TimeUnitRatio::k_US_PER_MS;
        totalMicroseconds += TimeUnitRatio::k_US_PER_S  * seconds
                          +  TimeUnitRatio::k_US_PER_MS * millisecond;

        setMicrosecondsFromMidnight(totalMicroseconds);
    }
    else {
        setMicrosecondsFromMidnight(TimeUnitRatio::k_US_PER_MS * millisecond);
    }
}

void Time::setMicrosecond(int microsecond)
{
    BSLS_ASSERT(0 <= microsecond);
    BSLS_ASSERT(     microsecond < TimeUnitRatio::k_US_PER_MS_32);

    bsls::Types::Int64 totalMicroseconds = microsecondsFromMidnight();

    if (totalMicroseconds < TimeUnitRatio::k_US_PER_D) {
        bsls::Types::Int64 milliseconds =
                                totalMicroseconds / TimeUnitRatio::k_US_PER_MS;

        totalMicroseconds = TimeUnitRatio::k_US_PER_MS * milliseconds
                          + microsecond;

        setMicrosecondsFromMidnight(totalMicroseconds);
    }
    else {
        setMicrosecondsFromMidnight(microsecond);
    }
}

void Time::setTime(int hour,
                   int minute,
                   int second,
                   int millisecond,
                   int microsecond)
{
    BSLS_ASSERT(isValid(hour, minute, second, millisecond, microsecond));

    setMicrosecondsFromMidnight(  TimeUnitRatio::k_US_PER_H  * hour
                                + TimeUnitRatio::k_US_PER_M  * minute
                                + TimeUnitRatio::k_US_PER_S  * second
                                + TimeUnitRatio::k_US_PER_MS * millisecond
                                + microsecond);
}

// ACCESSORS
void Time::getTime(int *hour,
                   int *minute,
                   int *second,
                   int *millisecond,
                   int *microsecond) const
{
    bsls::Types::Int64 totalMicroseconds = microsecondsFromMidnight();

    if (hour) {
        *hour = static_cast<int>(totalMicroseconds
                                                  / TimeUnitRatio::k_US_PER_H);
    }

    if (minute) {
        totalMicroseconds %= TimeUnitRatio::k_US_PER_H;

        *minute = static_cast<int>(totalMicroseconds
                                                  / TimeUnitRatio::k_US_PER_M);
    }

    if (second) {
        totalMicroseconds %= TimeUnitRatio::k_US_PER_M;

        *second = static_cast<int>(totalMicroseconds
                                                  / TimeUnitRatio::k_US_PER_S);
    }

    if (millisecond) {
        totalMicroseconds %= TimeUnitRatio::k_US_PER_S;

        *millisecond = static_cast<int>(totalMicroseconds
                                                 / TimeUnitRatio::k_US_PER_MS);
    }

    if (microsecond) {
        *microsecond = static_cast<int>(totalMicroseconds
                                                 % TimeUnitRatio::k_US_PER_MS);
    }
}

bsl::ostream& Time::print(bsl::ostream& stream,
                          int           level,
                          int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    //      Format: HH: MM: SS. ssssss '\0'
    const int size = 3 + 3 + 3 + 6 + 1;
    char      buffer[size];

    int hour, min, sec, mSec, uSec;

    getTime(&hour, &min, &sec, &mSec, &uSec);

    bsl::sprintf(buffer,
                 "%02d:%02d:%02d.%03d%03d",
                 hour,
                 min,
                 sec,
                 mSec,
                 uSec);

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);  // 'true' -> suppress '['
    stream << buffer;
    printer.end(true);    // 'true' -> suppress ']'

    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
