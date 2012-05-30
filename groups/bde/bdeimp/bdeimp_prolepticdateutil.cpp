// bdeimp_prolepticdateutil.cpp                                       -*-C++-*-
#include <bdeimp_prolepticdateutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdeimp_prolepticdateutil_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {

enum {
    // other useful constants

    MIN_MONTH                   = 1,
    MAX_MONTH                   = 12,
    MIN_YEAR                    = 1,
    MAX_YEAR                    = 9999,
    MAX_SERIAL_DAY              = 3652059,
    DAYS_IN_NON_LEAP_YEAR       = 365,
    DAYS_IN_LEAP_YEAR           = 366,
    DAYS_IN_4_YEARS             = DAYS_IN_NON_LEAP_YEAR * 4 + 1,  //   1,461
    DAYS_IN_100_YEARS           =  25 * DAYS_IN_4_YEARS     - 1,  //  36,524
    DAYS_IN_400_YEARS           =   4 * DAYS_IN_100_YEARS   + 1   // 146,097
};

// Note that, in each of the following arrays, the element at index position
// 0 is always the value of 0 (in order to facilitate asking questions
// involving all months up through the *previous* one).

static const int normDaysThroughMonth[] = { 0,
// Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
    31,  59,  90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};

static const int leapDaysThroughMonth[] = { 0,
// Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
    31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335, 366
};

static const int normDaysPerMonth[] = { 0,
// Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
    31,  28,  31,  30,  31,  30,  31,  31,  30,  31,  30, 31
};

static const int leapDaysPerMonth[] = { 0,
// Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
    31,  29,  31,  30,  31,  30,  31,  31,  30,  31,  30,  31
};

static inline
const int *getArrayDaysThroughMonth(int year)
    // Return the address of a static array that, for the specified 'year',
    // can be used to determine the number of days up to and including the
    // month indicated by an integer index in the range '[ 0 .. MAX_MONTH ]',
    // where an index of 0 always results in the value 0.  The behavior is
    // undefined unless 'MIN_YEAR <= year <= MAX_YEAR'.
{
    BSLS_ASSERT(MIN_YEAR <= year);
    BSLS_ASSERT(year <= MAX_YEAR);

    return bdeimp_ProlepticDateUtil::isLeapYear(year)
         ? leapDaysThroughMonth
         : normDaysThroughMonth;
}

static inline
int calendarDaysThroughMonth(int year, int month)
    // Return the number of calendar days since the start of the specified
    // 'year' and including the days in the specified 'month'.  The behavior
    // is undefined unless 'MIN_YEAR <= year <= MAX_YEAR' and
    // '0 <= month <= MAX_MONTH'.
{
    BSLS_ASSERT(MIN_YEAR <= year);
    BSLS_ASSERT(year <= MAX_YEAR);
    BSLS_ASSERT(0 <= month);
    BSLS_ASSERT(month <= MAX_MONTH);

    return getArrayDaysThroughMonth(year)[month];
}

                        // -------------------------------
                        // struct bdeimp_ProlepticDateUtil
                        // -------------------------------

// CLASS METHODS
bool bdeimp_ProlepticDateUtil::isValidCalendarDate(int year,
                                                   int month,
                                                   int day)
{
    if (year < 1 || year > 9999
     || month < 1 || month > 12
     || day < 1) {
        return false;                                                 // RETURN
    }

    const int daysInMonth = isLeapYear(year)
                          ? leapDaysPerMonth[month]
                          : normDaysPerMonth[month];
    return day <= daysInMonth;
}

bool bdeimp_ProlepticDateUtil::isValidSerialDate(int serialDay)
{
    return 1 <= serialDay && serialDay <= MAX_SERIAL_DAY;
}

int bdeimp_ProlepticDateUtil::ymd2serial(int year, int month, int day)
{
    BSLS_ASSERT(isValidCalendarDate(year, month, day));

    const int y = year - 1;

    return y * 365                         // additional days for each year
         + y / 4 - y / 100 + y / 400       // additional leap days
         + calendarDaysThroughMonth(year, month - 1)
                                           // additional days before this month
         + day;                            // additional days this month
}

void bdeimp_ProlepticDateUtil::serial2ymd(int *year,
                                          int *month,
                                          int *day,
                                          int  serialDay)
{
    BSLS_ASSERT(year);
    BSLS_ASSERT(month);
    BSLS_ASSERT(day);
    BSLS_ASSERT(isValidSerialDate(serialDay));

    int num400years = serialDay / DAYS_IN_400_YEARS;
    if (!(serialDay % DAYS_IN_400_YEARS)) {
        --num400years;
    }
    serialDay -= num400years * DAYS_IN_400_YEARS;

    int num100years = serialDay / DAYS_IN_100_YEARS;
    if (!(serialDay % DAYS_IN_400_YEARS)
     || !(serialDay % DAYS_IN_100_YEARS)) {
        --num100years;
    }

    serialDay -= num100years * DAYS_IN_100_YEARS;
    int num4years = serialDay / DAYS_IN_4_YEARS;
    if (!(serialDay % DAYS_IN_4_YEARS)) {
        --num4years;
    }
    serialDay -= num4years * DAYS_IN_4_YEARS;

    int num1years = serialDay / DAYS_IN_NON_LEAP_YEAR;
    if (!(serialDay % DAYS_IN_4_YEARS)
     || !(serialDay % DAYS_IN_NON_LEAP_YEAR)) {
        --num1years;
    }
    serialDay -= num1years * DAYS_IN_NON_LEAP_YEAR;

    const int y = num400years * 400
                + num100years * 100
                + num4years   *   4
                + num1years;

    const int *daysThroughMonth = getArrayDaysThroughMonth(y + 1);

    int m = 0;

    while (daysThroughMonth[++m] < serialDay);

    *year  = y + 1;
    *month = m;
    *day   = serialDay - daysThroughMonth[m - 1];
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2012
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
