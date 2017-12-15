// bdlt_time.cpp                                                      -*-C++-*-
#include <bdlt_time.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_time_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsls_assert.h>

#include <bsl_cstdio.h>
#include <bsl_limits.h>
#include <bsl_ostream.h>

///IMPLEMENTATION NOTES
///--------------------
// This component implements a time type by storing the number of microseconds
// since midnight.  The previous implementation stored milliseconds.  To
// discover and correct inappropriate usage elsewhere (e.g., using 'memcpy' to
// assign a value), a versioning bit is used ('k_REP_MASK') for the in-core
// representation.  When a value in the old format is detected, logging and
// asserting occur and the value is converted from the old millisecond
// representation to the new microsecond representation.
//
// The value of 'k_REP_MASK' was chosen to avoid the low-order 37 bits needed
// for the time value, and to leave the high-order 16 bits empty in case
// storing a time value in 6 bytes is valuable (e.g., 'bdld::Datum').

namespace BloombergLP {
namespace bdlt {

// STATIC HELPER FUNCTIONS
static
bsls::Types::Int64 fastMod(int *number, int base)
    // Efficiently mod ('%') the specified 'number' by the specified 'base' and
    // store the result back into 'number'; return the value of the original
    // 'number' divided by 'base'.  The behavior is undefined unless
    // '1 <= base'.  Note that, for efficiency, this function uses the native
    // '%' operator that, for an initially negative '*number' may, depending
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
    // '%' operator that, for an initially negative '*number' may, depending
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

static
int printToBufferFormatted(char       *result,
                           int         numBytes,
                           const char *spec,
                           int         hour,
                           int         minute,
                           int         second,
                           int         microsecond,
                           int         fractionalSecondPrecision)
{

#if defined(BSLS_PLATFORM_CMP_MSVC)
    // Windows uses a different variant of snprintf that does not necessarily
    // null-terminate and returns -1 on overflow.

    int numCharsWritten;
    int rc;

    if (0 == fractionalSecondPrecision) {
        rc = _snprintf(result,
                       numBytes,
                       spec,
                       hour,
                       minute,
                       second);

        // Format of 'bdlt::Time' has 8 characters if there are no fractional
        // seconds.

        numCharsWritten = 8;
    }
    else {
        rc = _snprintf(result,
                       numBytes,
                       spec,
                       hour,
                       minute,
                       second,
                       microsecond);

        // Format of 'bdlt::Time' has 9 characters + fractional seconds.

        numCharsWritten = 9 + fractionalSecondPrecision;
    }

    if ((0 > rc || rc == numBytes) && numBytes > 0) {
        result[numBytes - 1] = '\0';  // Make sure to null-terminate on
                                      // overflow.
    }

    return numCharsWritten;

#else

    return 0 == fractionalSecondPrecision
           ? snprintf(result,
                      numBytes,
                      spec,
                      hour,
                      minute,
                      second)
           : snprintf(result,
                      numBytes,
                      spec,
                      hour,
                      minute,
                      second,
                      microsecond);
#endif
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

    bsls::Types::Int64 wholeDays = interval.totalDays();

    totalMicroseconds += interval.fractionalDayInMicroseconds();
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

int Time::printToBuffer(char *result,
                        int   numBytes,
                        int   fractionalSecondPrecision) const
{
    BSLS_ASSERT(result);
    BSLS_ASSERT(0 <= numBytes);
    BSLS_ASSERT(0 <= fractionalSecondPrecision     );
    BSLS_ASSERT(     fractionalSecondPrecision <= 6);

    int hour;
    int minute;
    int second;
    int millisecond;
    int microsecond;

    getTime(&hour, &minute, &second, &millisecond, &microsecond);

    int value;

    switch (fractionalSecondPrecision) {
      case 0: {
        char spec[] = "%02d:%02d:%02d";

        return printToBufferFormatted(result,
                                      numBytes,
                                      spec,
                                      hour,
                                      minute,
                                      second,
                                      0,
                                      0);                             // RETURN
      } break;
      case 1: {
        value = millisecond / 100;
      } break;
      case 2: {
        value = millisecond / 10 ;
      } break;
      case 3: {
        value = millisecond;
      } break;
      case 4: {
        value = millisecond * 10 + microsecond / 100;
      } break;
      case 5: {
        value = millisecond * 100 + microsecond / 10;
      } break;
      default: {
        value = millisecond * 1000 + microsecond;
      } break;
    }

    char spec[] = "%02d:%02d:%02d.%0Xd";

    const int k_PRECISION_IDX = sizeof spec - 3;

    spec[k_PRECISION_IDX] = static_cast<char>('0' + fractionalSecondPrecision);

    return printToBufferFormatted(result,
                                  numBytes,
                                  spec,
                                  hour,
                                  minute,
                                  second,
                                  value,
                                  fractionalSecondPrecision);
}

                                  // Aspects

bsl::ostream& Time::print(bsl::ostream& stream,
                          int           level,
                          int           spacesPerLevel) const
{
    // Format the output to a buffer first instead of inserting into 'stream'
    // directly to improve performance and in case the caller has done
    // something like:
    //..
    //  os << bsl::setw(20) << myTime;
    //..
    // The user-specified width will be effective when 'buffer' is written to
    // the 'stream' (below).

    const int k_BUFFER_SIZE = 32;   // Buffer sized to hold a *bad* time.
    char      buffer[k_BUFFER_SIZE];

    int rc = printToBuffer(buffer,
                           k_BUFFER_SIZE,
                           k_DEFAULT_FRACTIONAL_SECOND_PRECISION);

    (void)rc;
    BSLS_ASSERT(15 == rc);  // The time format contains 15 characters.

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);    // 'true' -> suppress '['
    stream << buffer;
    printer.end(true);      // 'true' -> suppress ']'

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
