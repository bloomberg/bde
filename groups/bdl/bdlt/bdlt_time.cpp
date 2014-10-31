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
int fastMod(int *number, int base)
    // Efficiently mod ('%') the specified 'number' by the specified 'base'
    // and store the result back into 'number'; return the value of the
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

                        // ----------
                        // class Time
                        // ----------

// MANIPULATORS
int Time::addHours(int hours)
{
    d_milliseconds %= bdlt::TimeUnitRatio::k_MS_PER_D_32;
                                                 // Force zero if 24:00:00.000.

    int wholeDays = fastMod(&hours, bdlt::TimeUnitRatio::k_H_PER_D_32);
    d_milliseconds += hours * bdlt::TimeUnitRatio::k_MS_PER_H_32;
    return wholeDays + modulo(&d_milliseconds,
                              bdlt::TimeUnitRatio::k_MS_PER_D_32);
}

int Time::addMinutes(int minutes)
{
    d_milliseconds %= bdlt::TimeUnitRatio::k_MS_PER_D_32;
                                                 // Force zero if 24:00:00.000.

    int wholeDays = fastMod(&minutes,
                            bdlt::TimeUnitRatio::k_M_PER_D_32);
    d_milliseconds += minutes * bdlt::TimeUnitRatio::k_MS_PER_M_32;
    return wholeDays + modulo(&d_milliseconds,
                              bdlt::TimeUnitRatio::k_MS_PER_D_32);
}

int Time::addSeconds(int seconds)
{
    d_milliseconds %= bdlt::TimeUnitRatio::k_MS_PER_D_32;
                                                 // Force zero if 24:00:00.000.

    int wholeDays = fastMod(&seconds,
                            bdlt::TimeUnitRatio::k_S_PER_D_32);
    d_milliseconds += seconds * bdlt::TimeUnitRatio::k_MS_PER_S_32;
    return wholeDays + modulo(&d_milliseconds,
                              bdlt::TimeUnitRatio::k_MS_PER_D_32);
}

int Time::addMilliseconds(int milliseconds)
{
    d_milliseconds %= bdlt::TimeUnitRatio::k_MS_PER_D_32;
                                                 // Force zero if 24:00:00.000.

    int wholeDays = fastMod(&milliseconds,
                            bdlt::TimeUnitRatio::k_MS_PER_D_32);
    d_milliseconds += milliseconds;
    return wholeDays + modulo(&d_milliseconds,
                              bdlt::TimeUnitRatio::k_MS_PER_D_32);
}

int Time::addInterval(const DatetimeInterval& interval)
{
    d_milliseconds %= bdlt::TimeUnitRatio::k_MS_PER_D_32;
                                                 // Force zero if 24:00:00.000.

    // We must perform the 64-bit normalization with temporaries before
    // adjusting the 'int' 'd_milliseconds' value to avoid integer overflow.

    const bsls::Types::Int64 totalMsec = interval.totalMilliseconds();

    const int normMsec  = static_cast<int>(totalMsec
                                            % bdlt::TimeUnitRatio::k_MS_PER_D);

    const bsls::Types::Int64 wholeDays = totalMsec
                                             / bdlt::TimeUnitRatio::k_MS_PER_D;

    const bsls::Types::Int64 totalWholeDays = wholeDays
                                                   + addMilliseconds(normMsec);

    BSLS_ASSERT(bsl::numeric_limits<int>::min() <= totalWholeDays);
    BSLS_ASSERT(bsl::numeric_limits<int>::max() >= totalWholeDays);

    return static_cast<int>(totalWholeDays);
}

int Time::addTime(int hours, int minutes, int seconds, int milliseconds)
{
    d_milliseconds %= bdlt::TimeUnitRatio::k_MS_PER_D_32;
                                                 // Force zero if 24:00:00.000.

    int wholeDays = fastMod(&hours, bdlt::TimeUnitRatio::k_H_PER_D_32);
    d_milliseconds += hours * bdlt::TimeUnitRatio::k_MS_PER_H_32;

    wholeDays += fastMod(&minutes, bdlt::TimeUnitRatio::k_M_PER_D_32);
    d_milliseconds += minutes * bdlt::TimeUnitRatio::k_MS_PER_M_32;

    wholeDays += fastMod(&seconds, bdlt::TimeUnitRatio::k_S_PER_D_32);
    d_milliseconds += seconds * bdlt::TimeUnitRatio::k_MS_PER_S_32;

    wholeDays += fastMod(&milliseconds, bdlt::TimeUnitRatio::k_MS_PER_D_32);
    d_milliseconds += milliseconds;

    return wholeDays + modulo(&d_milliseconds,
                              bdlt::TimeUnitRatio::k_MS_PER_D_32);
}

void Time::setHour(int hour)
{
    BSLS_ASSERT(0 <= hour);
    BSLS_ASSERT(     hour <= bdlt::TimeUnitRatio::k_H_PER_D_32);

    if (hour < 24) {
        d_milliseconds %= bdlt::TimeUnitRatio::k_MS_PER_H_32;
        d_milliseconds += hour * bdlt::TimeUnitRatio::k_MS_PER_H_32;
    }
    else {
        d_milliseconds = bdlt::TimeUnitRatio::k_MS_PER_D_32;
    }

    BSLS_ASSERT_SAFE(0              <= d_milliseconds);
    BSLS_ASSERT_SAFE(d_milliseconds <= bdlt::TimeUnitRatio::k_MS_PER_D_32);
}

void Time::setMinute(int minute)
{
    BSLS_ASSERT(0 <= minute);
    BSLS_ASSERT(     minute < bdlt::TimeUnitRatio::k_M_PER_H_32);

    if (bdlt::TimeUnitRatio::k_MS_PER_D_32 != d_milliseconds) {
        int hours = d_milliseconds / bdlt::TimeUnitRatio::k_MS_PER_H_32;
        d_milliseconds %= bdlt::TimeUnitRatio::k_MS_PER_M_32;
        d_milliseconds += hours  * bdlt::TimeUnitRatio::k_MS_PER_H_32
                        + minute * bdlt::TimeUnitRatio::k_MS_PER_M_32;
    }
    else {
        d_milliseconds = minute * bdlt::TimeUnitRatio::k_MS_PER_M_32;
    }

    BSLS_ASSERT_SAFE(0              <= d_milliseconds);
    BSLS_ASSERT_SAFE(d_milliseconds <= bdlt::TimeUnitRatio::k_MS_PER_D_32);
}

void Time::setSecond(int second)
{
    BSLS_ASSERT(0 <= second);
    BSLS_ASSERT(     second < bdlt::TimeUnitRatio::k_S_PER_M_32);

    if (bdlt::TimeUnitRatio::k_MS_PER_D_32 != d_milliseconds) {
        int minutes = d_milliseconds / bdlt::TimeUnitRatio::k_MS_PER_M_32;
        d_milliseconds %= bdlt::TimeUnitRatio::k_MS_PER_S_32;
        d_milliseconds += minutes * bdlt::TimeUnitRatio::k_MS_PER_M_32
                       +  second  * bdlt::TimeUnitRatio::k_MS_PER_S_32;
    }
    else {
        d_milliseconds = bdlt::TimeUnitRatio::k_MS_PER_S_32 * second;
    }

    BSLS_ASSERT_SAFE(0              <= d_milliseconds);
    BSLS_ASSERT_SAFE(d_milliseconds <= bdlt::TimeUnitRatio::k_MS_PER_D_32);
}

void Time::setMillisecond(int millisecond)
{
    BSLS_ASSERT(0 <= millisecond);
    BSLS_ASSERT(     millisecond < bdlt::TimeUnitRatio::k_MS_PER_S_32);

    if (bdlt::TimeUnitRatio::k_MS_PER_D_32 != d_milliseconds) {
        d_milliseconds /= bdlt::TimeUnitRatio::k_MS_PER_S_32;
        d_milliseconds *= bdlt::TimeUnitRatio::k_MS_PER_S_32;
        d_milliseconds += millisecond;
    }
    else {
        d_milliseconds = millisecond;
    }

    BSLS_ASSERT_SAFE(0              <= d_milliseconds);
    BSLS_ASSERT_SAFE(d_milliseconds <= bdlt::TimeUnitRatio::k_MS_PER_D_32);
}

void Time::setTime(int hour, int minute, int second, int millisecond)
{
    BSLS_ASSERT(isValid(hour, minute, second, millisecond));

    d_milliseconds = millisecond
                   + second * bdlt::TimeUnitRatio::k_MS_PER_S_32
                   + minute * bdlt::TimeUnitRatio::k_MS_PER_M_32
                   + hour * bdlt::TimeUnitRatio::k_MS_PER_H_32;

    BSLS_ASSERT_SAFE(0              <= d_milliseconds);
    BSLS_ASSERT_SAFE(d_milliseconds <= bdlt::TimeUnitRatio::k_MS_PER_D_32);
}

// ACCESSORS
void Time::getTime(int *hour,
                   int *minute,
                   int *second,
                   int *millisecond) const
{
    int ms = d_milliseconds;
    int hr = ms / bdlt::TimeUnitRatio::k_MS_PER_H_32;
    ms %= bdlt::TimeUnitRatio::k_MS_PER_H_32;

    if (hour) {
        *hour = hr;
    }

    int min = ms / bdlt::TimeUnitRatio::k_MS_PER_M_32;
    ms %= bdlt::TimeUnitRatio::k_MS_PER_M_32;

    if (minute) {
        *minute = min;
    }

    int sec = ms / bdlt::TimeUnitRatio::k_MS_PER_S_32;

    if (second) {
        *second = sec;
    }

    if (millisecond) {
        *millisecond = ms % bdlt::TimeUnitRatio::k_MS_PER_S_32;
    }
}

bsl::ostream& Time::print(bsl::ostream& stream,
                          int           level,
                          int           spacesPerLevel) const
{
    if (stream.bad()) {
        return stream;                                                // RETURN
    }

    //      Format: HH: MM: SS. mmm '\0'
    const int size = 3 + 3 + 3 + 3 + 1;
    char      buffer[size];

    int hour, min, sec, mSec;

    getTime(&hour, &min, &sec, &mSec);

    //              Format: HH:  MM:  SS. mmm '\0'
    bsl::sprintf(buffer, "%02d:%02d:%02d.%03d", hour, min, sec, mSec);

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);  // 'true' -> suppress '['
    stream << buffer;
    printer.end(true);    // 'true' -> suppress ']'

    return stream;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
