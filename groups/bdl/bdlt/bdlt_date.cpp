// bdlt_date.cpp                                                      -*-C++-*-
#include <bdlt_date.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlt_date_cpp,"$Id$ $CSID$")

#include <bslim_printer.h>

#include <bsls_log.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>

#include <bsl_ostream.h>

#include <bsl_c_stdio.h>   // 'snprintf'

#ifndef BDE_OPENSOURCE_PUBLICATION
#include <bdlb_bitutil.h>
#endif

namespace BloombergLP {
namespace bdlt {

static const char *const months[] = {
    0,
    "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
    "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

                                  // ----------
                                  // class Date
                                  // ----------

#ifndef BDE_OPENSOURCE_PUBLICATION

// In the POSIX calendar, the first day after 1752/09/02 is 1752/09/14.  With
// 639798 for the "magic" serial date value, '>' is the appropriate comparison
// operator to use in the various 'logIfProblematicDate*' functions.

const int MAGIC_SERIAL = 639798;  // 1752/09/02 POSIX
                                  // 1752/09/15 proleptic Gregorian

// To limit spewing to 'stderr', log an occurrence of a problematic date value
// or operation only if the associated logging context count is 1, 8, or 256.

const int LOG_THROTTLE_MASK = 1 | 8 | 256;

// CLASS DATA
bool Date::s_loggingEnabledFlag = false;  // *off* by default

// PRIVATE CLASS METHODS
void Date::logIfProblematicDateAddition(const char *fileName,
                                        int         lineNumber,
                                        int         locationId,
                                        int         serialDate,
                                        int         numDays)
{
    if (!Date::isLoggingEnabled()
     || (serialDate > MAGIC_SERIAL && (serialDate + numDays) > MAGIC_SERIAL)) {
        return;                                                       // RETURN
    }

    static bsls::AtomicOperations::AtomicTypes::Int counts[32] = { 0 };

    if (locationId < 0 || locationId > 31) {
        return;                                                       // RETURN
    }

    const int tmpCount
             = bsls::AtomicOperations::addIntNvRelaxed(&counts[locationId], 1);

    if ((LOG_THROTTLE_MASK & tmpCount)
     && 1 == bdlb::BitUtil::numBitsSet(
                             static_cast<bdlb::BitUtil::uint32_t>(tmpCount))) {

        int year, month, day;
        DelegatingDateImpUtil::serialToYmd(&year, &month, &day, serialDate);

        bsls::Log::logFormattedMessage(
                              fileName, lineNumber,
                              "WARNING: bad 'Date' addition: "
                              "%d/%d/%d + %d [%d] "
                              "(see {TEAM 481627583<GO>})",
                              year, month, day, numDays,
                              tmpCount);
    }
}

void Date::logIfProblematicDateDifference(const char *fileName,
                                          int         lineNumber,
                                          int         locationId,
                                          int         lhsSerialDate,
                                          int         rhsSerialDate)
{
    if (!Date::isLoggingEnabled()
     || (lhsSerialDate > MAGIC_SERIAL && rhsSerialDate > MAGIC_SERIAL)) {
        return;                                                       // RETURN
    }

    static bsls::AtomicOperations::AtomicTypes::Int counts[32] = { 0 };

    if (locationId < 0 || locationId > 31) {
        return;                                                       // RETURN
    }

    const int tmpCount
             = bsls::AtomicOperations::addIntNvRelaxed(&counts[locationId], 1);

    if (1 == bdlb::BitUtil::numBitsSet(
                             static_cast<bdlb::BitUtil::uint32_t>(tmpCount))
     && (LOG_THROTTLE_MASK & tmpCount)) {

        int lhsYear, lhsMonth, lhsDay;
        DelegatingDateImpUtil::serialToYmd(&lhsYear, &lhsMonth, &lhsDay,
                                           lhsSerialDate);

        int rhsYear, rhsMonth, rhsDay;
        DelegatingDateImpUtil::serialToYmd(&rhsYear, &rhsMonth, &rhsDay,
                                           rhsSerialDate);

        bsls::Log::logFormattedMessage(
                            fileName, lineNumber,
                            "WARNING: bad 'Date' difference: "
                            "%d/%d/%d - %d/%d/%d [%d] "
                            "(see {TEAM 481627583<GO>})",
                            lhsYear, lhsMonth, lhsDay,
                            rhsYear, rhsMonth, rhsDay,
                            tmpCount);
    }
}

void Date::logIfProblematicDateValue(const char *fileName,
                                     int         lineNumber,
                                     int         locationId,
                                     int         serialDate)
{
    if (!Date::isLoggingEnabled()
     || (serialDate > MAGIC_SERIAL || 1 == serialDate)) {
        return;                                                       // RETURN
    }

    static bsls::AtomicOperations::AtomicTypes::Int counts[32] = { 0 };

    if (locationId < 0 || locationId > 31) {
        return;                                                       // RETURN
    }

    const int tmpCount
             = bsls::AtomicOperations::addIntNvRelaxed(&counts[locationId], 1);

    if (1 == bdlb::BitUtil::numBitsSet(
                             static_cast<bdlb::BitUtil::uint32_t>(tmpCount))
     && (LOG_THROTTLE_MASK & tmpCount)) {

        int year, month, day;
        DelegatingDateImpUtil::serialToYmd(&year, &month, &day, serialDate);

        bsls::Log::logFormattedMessage(
                                 fileName, lineNumber,
                                 "WARNING: bad 'Date' value: "
                                 "%d/%d/%d [%d] "
                                 "(see {TEAM 481627583<GO>})",
                                 year, month, day,
                                 tmpCount);
    }
}

// CLASS METHODS
void Date::disableLogging()
{
    s_loggingEnabledFlag = false;

    BSLS_LOG_SIMPLE("'bdlt::Date' logging disabled");
}

void Date::enableLogging()
{
    s_loggingEnabledFlag = true;

    BSLS_LOG_SIMPLE("'bdlt::Date' logging enabled");
}

#endif

// MANIPULATORS
int Date::addDaysIfValid(int numDays)
{
    enum { k_SUCCESS = 0, k_FAILURE = -1 };

    const int tmpSerialDate = d_serialDate + numDays;

    if (!Date::isValidSerial(tmpSerialDate)) {
        return k_FAILURE;                                             // RETURN
    }

#ifndef BDE_OPENSOURCE_PUBLICATION
    // Using maximum location 31 to minimize chance of a conflict with header
    // location values.
    enum { locationId = 31 };

    Date::logIfProblematicDateAddition(__FILE__, __LINE__,
                                       static_cast<int>(locationId),
                                       d_serialDate, numDays);
#endif

    d_serialDate = tmpSerialDate;

    return k_SUCCESS;
}

// ACCESSORS
bsl::ostream& Date::print(bsl::ostream& stream,
                          int           level,
                          int           spacesPerLevel) const
{
    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(stream.bad())) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;
        return stream;                                                // RETURN
    }

    // Typical space usage (10 bytes): ddMMMyyyy nil.  Reserve 128 bytes for
    // possible BAD DATE result, which is sufficient space for the bad date
    // verbose message even on a 64-bit system.

    char buffer[128];

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)

    if (BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY(
                                         !Date::isValidSerial(d_serialDate))) {
        BSLS_PERFORMANCEHINT_UNLIKELY_HINT;

#if defined(BSLS_PLATFORM_CMP_MSVC)
#define snprintf _snprintf
#endif

        snprintf(buffer,
                 sizeof buffer,
                 "*BAD*DATE:%p->d_serialDate=%d",
                 this,
                 d_serialDate);

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
        BSLS_LOG("'bdlt::Date' precondition violated: %s.", buffer);
#endif
        BSLS_ASSERT_SAFE(
                 !"'bdlt::Date::print' attempted on date with invalid state.");
    }
    else {
#endif  // defined(BSLS_ASSERT_OPT_IS_ACTIVE)

        int y, m, d;
        getYearMonthDay(&y, &m, &d);

        const char *const month = months[m];

        buffer[0] = static_cast<char>(d / 10 + '0');
        buffer[1] = static_cast<char>(d % 10 + '0');
        buffer[2] = month[0];
        buffer[3] = month[1];
        buffer[4] = month[2];
        buffer[5] = static_cast<char>(  y / 1000         + '0');
        buffer[6] = static_cast<char>(((y % 1000) / 100) + '0');
        buffer[7] = static_cast<char>(((y %  100) /  10) + '0');
        buffer[8] = static_cast<char>(  y %   10         + '0');
        buffer[9] = 0;

#if defined(BSLS_ASSERT_OPT_IS_ACTIVE)
    }
#endif  // defined(BSLS_ASSERT_OPT_IS_ACTIVE)

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
