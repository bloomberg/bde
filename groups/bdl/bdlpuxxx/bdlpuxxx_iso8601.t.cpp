// bdlpuxxx_iso8601.t.cpp                  -*-C++-*-

#include <bdlpuxxx_iso8601.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlb_chartype.h>

#include <bdlt_delegatingdateimputil.h>

#include <bsls_asserttest.h>
#include <bsls_stopwatch.h>
#include <bsls_log.h>

#include <bsl_vector.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_cmath.h>

#include <stdlib.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component can be thoroughly tested by generating a representative set
// of input values for each function and comparing the result with an expected
// result.
//-----------------------------------------------------------------------------
// [ 2] int generate(char *, const bdlt::Date&, int);
// [ 2] int generate(char *, const bdlt::Time&, int);
// [ 2] int generate(char *, const bdlt::Datetime&, int);
// [ 2] int generate(char *, const bdlt::DateTz&, int);
// [ 2] int generate(char *, const bdlt::DateTz&, int, bool);
// [ 2] int generate(char *, const bdlt::TimeTz&, int);
// [ 2] int generate(char *, const bdlt::TimeTz&, int, bool);
// [ 2] int generate(char *, const bdlt::DatetimeTz&, int);
// [ 2] int generate(char *, const bdlt::DatetimeTz&, int, bool);
// [ 2] int generate(bsl::ostream &, const bdlt::Date&, int);
// [ 2] int generate(bsl::ostream &, const bdlt::Time&, int);
// [ 2] int generate(bsl::ostream &, const bdlt::Datetime&, int);
// [ 2] int generate(bsl::ostream &, const bdlt::DateTz&, int);
// [ 2] int generate(bsl::ostream &, const bdlt::DateTz&, int, bool);
// [ 2] int generate(bsl::ostream &, const bdlt::TimeTz&, int);
// [ 2] int generate(bsl::ostream &, const bdlt::TimeTz&, int, bool);
// [ 2] int generate(bsl::ostream &, const bdlt::DatetimeTz&, int);
// [ 2] int generate(bsl::ostream &, const bdlt::DatetimeTz&, int, bool);
// [ 3] static int parse(bdlt::Date  *, const char *, int);
// [ 3] static int parse(bdlt::Time  *, const char *, int);
// [ 4] static int parse(bdlt::Datetime  *, const char *, int);
// [ 3] static int parse(bdlt::DateTz *, const char *, int);
// [ 3] static int parse(bdlt::TimeTz *, const char *, int);
// [ 4] static int parse(bdlt::DatetimeTz *, const char *, int);
// [ 7] void bdlpuxxx::Iso8601Configuration::setUseZAbbreviationForUtc(bool)
// [ 7] bool bdlpuxxx::Iso8601Configuration::useZAbbreviationForUtc()
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE
// [ 5] CONCERN: Unusual time zone offsets
// [ 6] CONCERN: parsing leap-seconds
//-----------------------------------------------------------------------------

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP1_ASSERT LOOP_ASSERT

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// The 'BSLS_BSLTESTUTIL_EXPAND' macro is required to workaround a
// pre-processor issue on windows that prevents __VA_ARGS__ to be expanded in
// the definition of 'BSLS_BSLTESTUTIL_NUM_ARGS'
#define EXPAND(X)                                            \
    X

#define NUM_ARGS_IMPL(X5, X4, X3, X2, X1, X0, N, ...)        \
    N

#define NUM_ARGS(...)                                        \
    EXPAND(NUM_ARGS_IMPL( __VA_ARGS__, 5, 4, 3, 2, 1, 0, ""))

#define LOOPN_ASSERT_IMPL(N, ...)                            \
    EXPAND(LOOP ## N ## _ASSERT(__VA_ARGS__))

#define LOOPN_ASSERT(N, ...)                                 \
    LOOPN_ASSERT_IMPL(N, __VA_ARGS__)

#define ASSERTV(...)                                         \
    LOOPN_ASSERT(NUM_ARGS(__VA_ARGS__), __VA_ARGS__)

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBALS, TYPEDEFS, CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

typedef bdlpuxxx::Iso8601 Util;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

void noopLogMessageHandler(const char *file, int line, const char *message)
    // Do nothing.
{
}

template <typename TYPE>
int myParse(TYPE *dst, const char *str)
{
    const int len = bsl::strlen(str);
    return Util::parse(dst, str, len);
}


//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------

//=============================================================================
//                      HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static
void testTimezone(const char *tzStr,
                  const bool  valid,
                  const int   offset)
{
    const bsl::string dateStr = "2000-01-02";
    const bsl::string timeStr = "12:34:56";
    const bsl::string datetimeStr = "2001-02-03T14:21:34";

    const bdlt::Date       initDate( 3,  3,  3);
    const bdlt::DateTz     initDateTz(initDate,-120);
    const bdlt::Time       initTime(11, 11, 11);
    const bdlt::TimeTz     initTimeTz(initTime, 120);
    const bdlt::Datetime   initDatetime(  initDate, initTime);
    const bdlt::DatetimeTz initDatetimeTz(initDatetime, 180);

    int ret;
    bdlt::Date date(initDate);
    const bsl::string& dateTzStr = dateStr + tzStr;
    ret = !Util::parse(&date, dateTzStr.c_str(),
                              dateTzStr.length());
    LOOP2_ASSERT(dateTzStr, ret, valid == ret);
    LOOP_ASSERT(dateTzStr, valid || initDate == date);

    bdlt::DateTz dateTz(initDateTz);
    ret = !Util::parse(&dateTz, dateTzStr.c_str(),
                                dateTzStr.length());
    LOOP2_ASSERT(dateTzStr, ret, valid == ret);
    LOOP_ASSERT(dateTzStr, valid || initDateTz == dateTz);
    LOOP3_ASSERT(dateTzStr, offset, dateTz.offset(),
                      !valid || offset == dateTz.offset());

    bdlt::Time time(initTime);
    const bsl::string& timeTzStr = timeStr + tzStr;
    ret = !Util::parse(&time, timeTzStr.c_str(),
                              timeTzStr.length());
    LOOP2_ASSERT(timeTzStr, ret, valid == ret);
    LOOP_ASSERT(timeTzStr, valid || initTime == time);

    bdlt::TimeTz timeTz(initTimeTz);
    ret = !Util::parse(&timeTz, timeTzStr.c_str(),
                                timeTzStr.length());
    LOOP2_ASSERT(timeTzStr, ret, valid == ret);
    LOOP_ASSERT(timeTzStr, valid || initTimeTz == timeTz);
    LOOP3_ASSERT(timeTzStr, offset, timeTz.offset(),
                      !valid || offset == timeTz.offset());

    bdlt::Datetime datetime(initDatetime);
    const bsl::string& datetimeTzStr = datetimeStr + tzStr;
    ret = !Util::parse(&datetime, datetimeTzStr.c_str(),
                                  datetimeTzStr.length());
    LOOP2_ASSERT(dateTzStr, ret, valid == ret);
    LOOP_ASSERT(dateTzStr,
                        valid || initDatetime == datetime);

    bdlt::DatetimeTz datetimeTz(initDatetimeTz);
    ret = !Util::parse(&datetimeTz,datetimeTzStr.c_str(),
                                   datetimeTzStr.length());
    LOOP2_ASSERT(dateTzStr, ret, valid == ret);
    LOOP_ASSERT(dateTzStr,
                    valid || initDatetimeTz == datetimeTz);
    LOOP3_ASSERT(datetimeTzStr, offset,datetimeTz.offset(),
                  !valid || offset == datetimeTz.offset());
}

//=============================================================================
//                      TEST CASE 3
//-----------------------------------------------------------------------------

// Note that the following test case is factored into function to avoid
// internal compiler errors building optimized versions of the test driver.

void testCase3TestingParse()
{
    const struct {
        int         d_line;
        int         d_year;
        int         d_month;
        int         d_day;
        int         d_hour;
        int         d_minute;
        int         d_second;
        int         d_millisecond;
        const char *d_fracSecond;
        bool        d_dateValid;
        bool        d_timeValid;
    } DATA[] = {
        //                                                   Dat Tim
        //Ln  Year  Mo  Day Hr  Min  Sec     ms Frac         Vld Vld
        //==  ====  ==  === ==  ===  ===     == ====         === ===

        // Invalid Dates
        { L_, 0000,  0,  0,  0,   0,   0,     0, "",           0,  1 },
        { L_, 2005,  0,  1,  0,   0,   0,     0, "",           0,  1 },
        { L_, 2005, 13,  1,  0,   0,   0,     0, "",           0,  1 },
        { L_, 2005, 99,  1,  0,   0,   0,     0, "",           0,  1 },
        { L_, 2005,  1,  0,  0,   0,   0,     0, "",           0,  1 },
        { L_, 2005,  1, 32,  0,   0,   0,     0, "",           0,  1 },
        { L_, 2005,  1, 99,  0,   0,   0,     0, "",           0,  1 },

        // Invalid Times
        { L_, 2005, 12, 31, 25,   0,   0,     0, "",           1,  0 },
        { L_, 2005, 12, 31, 99,   0,   0,     0, "",           1,  0 },
        { L_, 2005, 12, 31, 12,  60,   0,     0, "",           1,  0 },
        { L_, 2005, 12, 31, 12, 100,   0,     0, "",           1,  0 },
        { L_, 2005, 12, 31, 12,  59,  62,     0, "",           1,  0 },
        { L_, 2005, 12, 31, 12,  59, 101,     0, "",           1,  0 },

        { L_, 2005,  1,  1, 24,   1,   0,     0, "",           1,  0 },
        { L_, 2005,  1,  1, 24,   0,   1,     0, "",           1,  0 },
        { L_, 2005,  1,  1, 24,   0,   0,   999, ".9991",      1,  0 },

        // Valid dates and times
// The behavior for 24:00:00.000 changed (DRQS 60307287)
//      { L_, 2005,  1,  1, 24,   0,   0,     0, "",           1,  1 },

        { L_, 2005,  1,  1,  0,   0,   0,     0, "",           1,  1 },
        { L_, 0123,  6, 15, 13,  40,  59,     0, "",           1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     0, "",           1,  1 },

        // Vary fractions of a second.
        { L_, 1999, 10, 12, 23,   0,   1,     0, ".0",         1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     0, ".00",        1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     0, ".000",       1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     0, ".0000",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     0, ".00000",     1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     0, ".000000",    1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     0, ".0000000",   1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     0, ".0004",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     0, ".00045",     1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     0, ".000456",    1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     0, ".0004567",   1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     1, ".0005",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     1, ".0006",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     1, ".0009",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     2, ".002",       1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     2, ".0020",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   200, ".2",         1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   200, ".20",        1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   200, ".200",       1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   200, ".2000",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   200, ".20000",     1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   200, ".200000",    1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,     3, ".0025",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,    34, ".034",       1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,    34, ".0340",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,    35, ".0345",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   456, ".456",       1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   456, ".4560",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   457, ".4567",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   999, ".9994",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   999, ".99945",     1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   999, ".999456",    1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,   999, ".9994567",   1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,  1000, ".9995",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,  1000, ".99956",     1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,  1000, ".999567",    1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,  1000, ".9999",      1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,  1000, ".99991",     1,  1 },
        { L_, 1999, 10, 12, 23,   0,   1,  1000, ".999923",    1,  1 },
        { L_, 1999, 12, 31, 23,  59,  59,  1000, ".9995",      1,  1 },
    };

    static const int NUM_DATA = sizeof DATA / sizeof *DATA;

    static const int UTC_EMPTY_OFFSET = 0x70000000;
    static const int UTC_UCZ_OFFSET   = UTC_EMPTY_OFFSET + 'Z';
    static const int UTC_LCZ_OFFSET   = UTC_EMPTY_OFFSET + 'z';

    static const int UTC_OFFSETS[] = {
        0, -90, -240, -720, -1439, 90, 240, 720, 1439,
        UTC_EMPTY_OFFSET, UTC_UCZ_OFFSET, UTC_LCZ_OFFSET
    };
    static const int NUM_UTC_OFFSETS = sizeof UTC_OFFSETS /sizeof *UTC_OFFSETS;

    static const char *JUNK[] = { "xyz", "?1234", "*zbc", "*", "01", "+",
                                                                     "-" };
    static const int NUM_JUNK = sizeof JUNK / sizeof *JUNK;

    const bdlt::Date       initDate( 3,  3,  3);
    const bdlt::Time       initTime(11, 11, 11);
    const bdlt::DateTz     initDateTz(initDate,-120);
    const bdlt::TimeTz     initTimeTz(initTime, 120);
    const bdlt::Datetime   initDatetime(  initDate, initTime);
    const bdlt::DatetimeTz initDatetimeTz(initDatetime, 180);

    for (int i = 0; i < NUM_DATA; ++i) {
        const int         LINE        = DATA[i].d_line;
        const int         YEAR        = DATA[i].d_year;
        const int         MONTH       = DATA[i].d_month;
        const int         DAY         = DATA[i].d_day;
        const int         HOUR        = DATA[i].d_hour;
        const int         MINUTE      = DATA[i].d_minute;
        const int         SECOND      = DATA[i].d_second;
        const int         MILLISECOND = DATA[i].d_millisecond;
        const char *const FRAC_SECOND = DATA[i].d_fracSecond;
        const bool        DATE_VALID  = DATA[i].d_dateValid;
        const bool        TIME_VALID  = DATA[i].d_timeValid;

        if (verbose) {
            P_(LINE); P_(YEAR); P_(MONTH); P(DAY);
            P_(HOUR); P_(MINUTE); P_(SECOND); P_(MILLISECOND);
            P_(FRAC_SECOND); P_(DATE_VALID); P(TIME_VALID);
        }

        const bool carrySecond = bsl::strlen(FRAC_SECOND) >= 5
                                  && !bsl::strncmp(FRAC_SECOND, ".999", 4)
                                  && bdlb::CharType::isDigit(FRAC_SECOND[4])
                                  && FRAC_SECOND[4] - '0' >= 5;
        LOOP_ASSERT(LINE, carrySecond == (MILLISECOND == 1000));

        bdlt::Date theDate;
        const bool isValidDate =
            0 == theDate.setYearMonthDayIfValid(YEAR, MONTH, DAY);
        LOOP_ASSERT(LINE, DATE_VALID == isValidDate);

        for (int j = 0; j < NUM_UTC_OFFSETS; ++j) {
            if (verbose) { T_; P_(NUM_UTC_OFFSETS); P(j); }

            const int UTC_OFFSET = (UTC_OFFSETS[j] >= UTC_EMPTY_OFFSET ?
                                    0 : UTC_OFFSETS[j]);

            for (int k = 0; k < NUM_JUNK; ++k) {
                // Select a semi-random piece of junk to append to the
                // end of the string.

                const char* const JUNK_STR = JUNK[k];

                bdlt::DateTz     theDateTz;
                if (isValidDate) {
                    theDateTz.setDateTz(theDate, UTC_OFFSET);
                }

                bdlt::Time theTime;
                bool isValidTime =
                             0 == theTime.setTimeIfValid(HOUR, MINUTE, SECOND);
                if (isValidTime && MILLISECOND) {
                    if (HOUR >= 24) {
                        isValidTime = false;
                        theTime = bdlt::Time();
                    }
                    else {
                        theTime.addMilliseconds(MILLISECOND);
                    }
                }
                LOOP_ASSERT(LINE, TIME_VALID == isValidTime);
                isValidTime &= !(24 == HOUR && UTC_OFFSET);

                bdlt::TimeTz     theTimeTz;
                bool isValidTimeTz = isValidTime
                   && 0 == theTimeTz.validateAndSetTimeTz(theTime, UTC_OFFSET);

                bool isValidDatetime = (isValidDate && isValidTime);
                bdlt::Datetime theDatetime;
                if (isValidDatetime) {
                    theDatetime.setDatetime(YEAR, MONTH, DAY,
                                            HOUR, MINUTE, SECOND);
                    if (MILLISECOND) {
                        theDatetime.addMilliseconds(MILLISECOND);
                    }
                }

                bdlt::DatetimeTz theDatetimeTz;
                bool isValidDatetimeTz = isValidDatetime
                                && 0 == theDatetimeTz.validateAndSetDatetimeTz(
                                                      theDatetime, UTC_OFFSET);

                char dateStr[25], timeStr[25], offsetStr[10];
                bsl::sprintf(dateStr, "%04d-%02d-%02d", YEAR, MONTH, DAY);
                bsl::sprintf(timeStr, "%02d:%02d:%02d%s",
                             HOUR, MINUTE, SECOND, FRAC_SECOND);
                if (UTC_OFFSETS[j] >= UTC_EMPTY_OFFSET) {
                    // Create empty string, "Z", or "z"
                    offsetStr[0] = static_cast<char>(UTC_OFFSETS[j] -
                                                     UTC_EMPTY_OFFSET);
                    offsetStr[1] = '\0';
                }
                else {
                    bsl::sprintf(offsetStr, "%+03d:%02d",
                                   UTC_OFFSET / 60, bsl::abs(UTC_OFFSET) % 60);
                }

                const bool trailFrac = bsl::strlen(FRAC_SECOND) > 1
                                          && 0 == bsl::strlen(offsetStr)
                                          && bdlb::CharType::isDigit(*JUNK_STR);

                char input[200];
                int ret;

                {
                    bdlt::DatetimeTz datetime = initDatetimeTz;
                    bsl::strcpy(input, dateStr);
                    bsl::strcat(input, "T");
                    bsl::strcat(input, timeStr);
                    bsl::strcat(input, offsetStr);
                    int inputLen = bsl::strlen(input);
                    bsl::strcat(input, JUNK_STR); // not included in length
                    ret = Util::parse(&datetime, input, inputLen);
                    LOOP5_ASSERT(LINE, input, ret, isValidDatetimeTz,
                                        UTC_OFFSET, isValidDatetimeTz == !ret);
                    if (ret) {
                        LOOP_ASSERT(datetime, initDatetimeTz == datetime);
                    }
                    else {
                        LOOP3_ASSERT(LINE, input, datetime,
                                                    datetime == theDatetimeTz);
                        if (carrySecond) {
                            LOOP3_ASSERT(LINE, datetime, SECOND,
                                  datetime.localDatetime().second() ==
                                                            (SECOND + 1) % 60);
                            LOOP3_ASSERT(LINE, datetime, SECOND,
                                  datetime.localDatetime().millisecond() == 0);
                        }
                        else {
                            const bdlt::Datetime& dt = datetime.localDatetime();

                            LOOP3_ASSERT(LINE, datetime, MILLISECOND,
                                              dt.millisecond() == MILLISECOND);
                            LOOP3_ASSERT(LINE, datetime, SECOND,
                                                        dt.second() == SECOND);
                            LOOP3_ASSERT(LINE, datetime, MINUTE,
                                                        dt.minute() == MINUTE);
                            LOOP3_ASSERT(LINE, datetime, HOUR,
                                                            dt.hour() == HOUR);
                            LOOP3_ASSERT(LINE, datetime, DAY,
                                                              dt.day() == DAY);
                            LOOP3_ASSERT(LINE, datetime, MONTH,
                                                          dt.month() == MONTH);
                            LOOP4_ASSERT(LINE, input, datetime, YEAR,
                                                            dt.year() == YEAR);
                        }
                    }
                    if (veryVerbose) { T_; P(datetime); }

                    // with junk on end should fail

                    if (!trailFrac) {
                        datetime = initDatetimeTz;
                        ret = Util::parse(&datetime, input,
                                                           bsl::strlen(input));
                        LOOP5_ASSERT(LINE, input, ret, isValidDatetimeTz,
                                                         UTC_OFFSET, 0 != ret);
                        LOOP_ASSERT(datetime, initDatetimeTz == datetime);
                    }
                }

                {
                    const bdlt::Datetime EXP_DATETIME = isValidDatetime ?
                                 theDatetimeTz.gmtDatetime() : bdlt::Datetime();

                    bdlt::Datetime datetime = initDatetime;
                    bsl::strcpy(input, dateStr);
                    bsl::strcat(input, "T");
                    bsl::strcat(input, timeStr);
                    bsl::strcat(input, offsetStr);
                    int inputLen = bsl::strlen(input);
                    bsl::strcat(input, JUNK_STR); // not included in length
                    ret = Util::parse(&datetime, input, inputLen);
                    LOOP3_ASSERT(LINE, input, ret, isValidDatetime == !ret);
                    if (ret) {
                        LOOP_ASSERT(datetime, initDatetime == datetime);
                    }
                    if (isValidDatetime) {
                        LOOP4_ASSERT(LINE, input, datetime, EXP_DATETIME,
                                                     EXP_DATETIME == datetime);
                        if (carrySecond) {
                            LOOP3_ASSERT(LINE, datetime, SECOND,
                                       datetime.second() == (SECOND + 1) % 60);
                            LOOP3_ASSERT(LINE, datetime, SECOND,
                                                  datetime.millisecond() == 0);
                        }
                        else {
                            LOOP3_ASSERT(LINE, datetime, SECOND,
                                                  datetime.second() == SECOND);
                            if (0 == UTC_OFFSET % 60) {
                                LOOP3_ASSERT(LINE, datetime, MINUTE,
                                                  datetime.minute() == MINUTE);
                            }
                            if (0 == UTC_OFFSET) {
                                LOOP3_ASSERT(LINE, datetime, HOUR,
                                                      datetime.hour() == HOUR);
                                LOOP3_ASSERT(LINE, datetime, DAY,
                                                        datetime.day() == DAY);
                                LOOP3_ASSERT(LINE, datetime, MONTH,
                                                    datetime.month() == MONTH);
                                LOOP3_ASSERT(LINE, datetime, YEAR,
                                                      datetime.year() == YEAR);
                            }
                        }
                    }
                    else {
                        LOOP_ASSERT(LINE, initDatetime == datetime);
                    }
                    if (veryVerbose) { T_; P(datetime); }

                    // with junk on end should fail

                    if (!trailFrac) {
                        datetime = initDatetime;
                        ret = Util::parse(&datetime,
                                          input,
                                          bsl::strlen(input));
                        LOOP3_ASSERT(LINE, input, ret, 0 != ret);
                        LOOP_ASSERT(initDatetime,initDatetime == datetime);
                    }
                }

                {
                    bdlt::DateTz date = initDateTz;
                    bsl::strcpy(input, dateStr);
                    bsl::strcat(input, offsetStr);
                    int inputLen = bsl::strlen(input);
                    bsl::strcat(input, JUNK_STR); // not included in length
                    ret = Util::parse(&date, input, inputLen);
                    LOOP3_ASSERT(LINE, input, ret, isValidDate == !ret);
                    if (ret) {
                        LOOP_ASSERT(date, initDateTz == date);
                    }
                    if (isValidDate) {
                        LOOP3_ASSERT(LINE, input, date, date == theDateTz);
                        LOOP3_ASSERT(LINE, date, DAY,
                                                date.localDate().day() == DAY);
                        LOOP3_ASSERT(LINE, date, MONTH,
                                            date.localDate().month() == MONTH);
                        LOOP3_ASSERT(LINE, date, YEAR,
                                              date.localDate().year() == YEAR);
                    }
                    else {
                        LOOP_ASSERT(LINE, initDateTz == date);
                    }
                    if (veryVerbose) { T_; P(date); }

                    // with junk on end should fail

                    if (!trailFrac) {
                        date = initDateTz;
                        ret = Util::parse(&date, input, bsl::strlen(input));
                        LOOP3_ASSERT(LINE, input, ret, 0 != ret);
                        LOOP_ASSERT(date, initDateTz == date);
                    }
                }

                {
                    bdlt::Date date = initDate;
                    bsl::strcpy(input, dateStr);
                    bsl::strcat(input, offsetStr);
                    int inputLen = bsl::strlen(input);
                    bsl::strcat(input, JUNK_STR); // not included in length
                    ret = Util::parse(&date, input, inputLen);
                    LOOP3_ASSERT(LINE, input, ret, isValidDate == !ret);
                    if (ret) {
                        LOOP_ASSERT(date, initDate == date);
                    }
                    if (isValidDate) {
                        LOOP3_ASSERT(LINE, input, date, date == theDate);
                        LOOP3_ASSERT(LINE, date, DAY, date.day() == DAY);
                        LOOP3_ASSERT(LINE, date, MONTH,
                                                        date.month() == MONTH);
                        LOOP3_ASSERT(LINE, date, YEAR,
                                                         date.year()  == YEAR);
                    }
                    else {
                        LOOP_ASSERT(LINE, initDate == date);
                    }
                    if (veryVerbose) { T_; P(date); }

                    // with junk on end should fail

                    if (!trailFrac) {
                        date = initDate;
                        ret = Util::parse(&date, input,bsl::strlen(input));
                        LOOP3_ASSERT(LINE, input, ret, 0 != ret);
                        LOOP_ASSERT(date, initDate == date);
                    }
                }

                {
                    bdlt::TimeTz time = initTimeTz;
                    bsl::strcpy(input, timeStr);
                    bsl::strcat(input, offsetStr);
                    int inputLen = bsl::strlen(input);
                    bsl::strcat(input, JUNK_STR); // not included in length
                    ret = Util::parse(&time, input, inputLen);
                    LOOP5_ASSERT(LINE, input, ret, isValidTimeTz,
                                        UTC_OFFSET, isValidTimeTz == !ret);
                    if (ret) {
                        LOOP_ASSERT(time, initTimeTz == time);
                    }
                    if (isValidTimeTz) {
                        LOOP3_ASSERT(LINE, input, time, time == theTimeTz);
                        if (carrySecond) {
                            LOOP3_ASSERT(LINE, time, SECOND,
                                        time.localTime().second() ==
                                                            (SECOND + 1) % 60);
                            LOOP3_ASSERT(LINE, time, SECOND,
                                          time.localTime().millisecond() == 0);
                        }
                        else {
                            LOOP3_ASSERT(LINE, time, SECOND,
                                          time.localTime().second() == SECOND);
                            LOOP3_ASSERT(LINE, time, MINUTE,
                                          time.localTime().minute() == MINUTE);
                            LOOP3_ASSERT(LINE, time, HOUR,
                                              time.localTime().hour() == HOUR);
                        }
                    }
                    else if (! isValidTimeTz) {
                        LOOP_ASSERT(LINE, initTimeTz == time);
                    }
                    if (veryVerbose) { T_; P(time); }

                    // with junk on end should fail

                    if (!trailFrac) {
                        time = initTimeTz;
                        ret = Util::parse(&time, input,bsl::strlen(input));
                        LOOP5_ASSERT(LINE, input, ret, isValidTimeTz,
                                                         UTC_OFFSET, 0 != ret);
                        LOOP_ASSERT(time, initTimeTz == time);
                    }
                }

                {
                    const bdlt::Time EXP_TIME = isValidTime ?
                                             theTimeTz.gmtTime() : bdlt::Time();

                    bdlt::Time time = initTime;
                    bsl::strcpy(input, timeStr);
                    bsl::strcat(input, offsetStr);
                    int inputLen = bsl::strlen(input);
                    bsl::strcat(input, JUNK_STR); // not included in length
                    ret = Util::parse(&time, input, inputLen);
                    LOOP3_ASSERT(LINE, input, ret, isValidTime == !ret);
                    if (ret) {
                        LOOP_ASSERT(time, initTime == time);
                    }
                    if (isValidTime) {
                        LOOP4_ASSERT(LINE, input, time, EXP_TIME,
                                                             EXP_TIME == time);
                        if (carrySecond) {
                            LOOP3_ASSERT(LINE, time, SECOND,
                                           time.second() == (SECOND + 1) % 60);
                            LOOP3_ASSERT(LINE, time, SECOND,
                                                      time.millisecond() == 0);
                        }
                        else {
                            LOOP3_ASSERT(LINE, time, SECOND,
                                                      time.second() == SECOND);
                            if (0 == UTC_OFFSET % 60) {
                                LOOP3_ASSERT(LINE, time, MINUTE,
                                                      time.minute() == MINUTE);
                            }
                            if (0 == UTC_OFFSET) {
                                LOOP3_ASSERT(LINE, time, HOUR,
                                                          time.hour() == HOUR);
                            }
                        }
                    }
                    else if (! isValidTime) {
                        LOOP_ASSERT(LINE, initTime == time);
                    }
                    if (veryVerbose) { T_; P(time); }

                    // with junk on end should fail

                    if (!trailFrac) {
                        time = initTime;
                        ret = Util::parse(&time, input, bsl::strlen(input));
                        LOOP3_ASSERT(LINE, input, ret, 0 != ret);
                        LOOP_ASSERT(time, initTime == time);
                    }
                }
            }
        }
    }
}

//=============================================================================
//                      TEST CASE 2
//-----------------------------------------------------------------------------

// Note that the following test case is factored into function to avoid
// internal compiler errors building optimized versions of the test driver.

void testCase2TestingGenerate()
{
    if (verbose) bsl::cout << "\nTESTING GENERATE"
                           << "\n================" << bsl::endl;


    const struct {
            int         d_line;
            int         d_year;
            int         d_month;
            int         d_day;
            int         d_hour;
            int         d_minute;
            int         d_second;
            int         d_millisecond;
    } DATA[] = {
        //Line Year   Mon  Day  Hour  Min  Sec     ms
        //==== ====   ===  ===  ====  ===  ===     ==

        // Valid dates and times
        { L_,  0001,   1,   1,    0,   0,   0,     0 },
        { L_,  2005,   1,   1,    0,   0,   0,     0 },
        { L_,  0123,   6,  15,   13,  40,  59,     0 },
        { L_,  1999,  10,  12,   23,   0,   1,     0 },

        // Vary milliseconds
        { L_,  1999,  10,  12,   23,   0,   1,     0 },
        { L_,  1999,  10,  12,   23,   0,   1,     1 },
        { L_,  1999,  10,  12,   23,   0,   1,     2 },
        { L_,  1999,  10,  12,   23,   0,   1,     3 },
        { L_,  1999,  10,  12,   23,   0,   1,    30 },
        { L_,  1999,  10,  12,   23,   0,   1,    34 },
        { L_,  1999,  10,  12,   23,   0,   1,    35 },
        { L_,  1999,  10,  12,   23,   0,   1,   200 },
        { L_,  1999,  10,  12,   23,   0,   1,   456 },
        { L_,  1999,  10,  12,   23,   0,   1,   457 },
        { L_,  1999,  10,  12,   23,   0,   1,   999 },
        { L_,  1999,  12,  31,   23,  59,  59,   999 },
    };

    static const int NUM_DATA = sizeof DATA / sizeof *DATA;

    static const int UTC_OFFSETS[] = {
        0, -90, -240, -720, 90, 240, 720
    };
    static const int NUM_UTC_OFFSETS =
        sizeof UTC_OFFSETS / sizeof *UTC_OFFSETS;

    if (verbose) cout << "\tLoop based test of test-data.\n";

    for (int i = 0; i < NUM_DATA; ++i) {
        const int         LINE        = DATA[i].d_line;
        const int         YEAR        = DATA[i].d_year;
        const int         MONTH       = DATA[i].d_month;
        const int         DAY         = DATA[i].d_day;
        const int         HOUR        = DATA[i].d_hour;
        const int         MINUTE      = DATA[i].d_minute;
        const int         SECOND      = DATA[i].d_second;
        const int         MILLISECOND = DATA[i].d_millisecond;

        bdlt::Date theDate(YEAR, MONTH, DAY);
        bdlt::Time theTime(HOUR, MINUTE, SECOND, MILLISECOND);
        bdlt::Datetime theDatetime(YEAR, MONTH, DAY,
                                  HOUR, MINUTE, SECOND, MILLISECOND);

        bsl::stringstream output;
        char outbuf[Util::BDEPU_MAX_DATETIME_STRLEN + 3];
        int  rc;

        for (int j = 0; j < NUM_UTC_OFFSETS; ++j) {

            const int UTC_OFFSET = UTC_OFFSETS[j];

            bdlt::DateTz     theDateTz(theDate, UTC_OFFSET);
            bdlt::TimeTz     theTimeTz(theTime, UTC_OFFSET);
            bdlt::DatetimeTz theDatetimeTz(theDatetime, UTC_OFFSET);

            char dateStr[25], timeStr[25], offsetStr[10];
            bsl::sprintf(dateStr, "%04d-%02d-%02d", YEAR, MONTH, DAY);
            bsl::sprintf(timeStr, "%02d:%02d:%02d.%03d",
                         HOUR, MINUTE, SECOND, MILLISECOND);
            bsl::sprintf(offsetStr, "%+03d:%02d",
                         UTC_OFFSET / 60, bsl::abs(UTC_OFFSET) % 60);

            char expected[100];

            // Testing bdlt::DatetimeTz
            {
                output.str("");

                bsl::strcpy(expected, dateStr);
                bsl::strcat(expected, "T");
                bsl::strcat(expected, timeStr);
                bsl::strcat(expected, offsetStr);

                int TEST_LENGTH = Util::BDEPU_DATETIMETZ_STRLEN;
                int k;
                // test short output buffers
                for (k=0; k < TEST_LENGTH + 1; ++k) {

                    bsl::memset(outbuf, '*', sizeof(outbuf));
                    outbuf[sizeof(outbuf)-1] = 0;

                    rc = Util::generate(outbuf, theDatetimeTz, k);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 rc == TEST_LENGTH);

                    rc = bsl::memcmp(outbuf, expected, k);
                    LOOP3_ASSERT(LINE, expected, outbuf, rc == 0);

                    // check that "outbuf" bytes started from "k"
                    // have not been changed
                    for (int m = k; m <  TEST_LENGTH + 2; ++m) {
                        LOOP3_ASSERT(LINE,
                                     expected,
                                     outbuf,
                                     outbuf[m] == '*');
                    }
                }

                // test long enough output buffers
                for (; k < TEST_LENGTH + 5; ++k) {

                    bsl::memset(outbuf, '*', sizeof(outbuf));
                    outbuf[sizeof(outbuf)-1] = 0;

                    rc = Util::generate(outbuf, theDatetimeTz, k);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 rc == TEST_LENGTH);

                    rc = bsl::memcmp(outbuf, expected, TEST_LENGTH);
                    LOOP3_ASSERT(LINE, expected, outbuf, rc == 0);

                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 outbuf[TEST_LENGTH] == 0);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 outbuf[TEST_LENGTH + 1] == '*');
                }

                Util::generate(output, theDatetimeTz);
                LOOP3_ASSERT(LINE, expected, output.str(),
                             output.str() == expected);
                if (veryVerbose) { P_(expected); P(output.str()); }
            }

            // Testing bdlt::Datetime
            {
                output.str("");

                bsl::strcpy(expected, dateStr);
                bsl::strcat(expected, "T");
                bsl::strcat(expected, timeStr);

                int TEST_LENGTH = Util::BDEPU_DATETIME_STRLEN;
                int k;
                // test short output buffers
                for (k=0; k < TEST_LENGTH + 1; ++k) {

                    bsl::memset(outbuf, '*', sizeof(outbuf));
                    outbuf[sizeof(outbuf)-1] = 0;

                    rc = Util::generate(outbuf, theDatetime, k);
                    LOOP3_ASSERT(LINE,
                                 expected,
                                 outbuf,
                                 rc == TEST_LENGTH);

                    rc = bsl::memcmp(outbuf, expected, k);
                    LOOP3_ASSERT(LINE, expected, outbuf, rc == 0);

                    // check that "outbuf" bytes started from "k"
                    // have not been changed
                    for (int m = k; m <  TEST_LENGTH + 2; ++m) {

                        LOOP3_ASSERT(LINE,
                                     expected,
                                     outbuf,
                                     outbuf[m] == '*');
                    }
                }

                // test long enough output buffers
                for (; k < TEST_LENGTH + 5; ++k) {

                    bsl::memset(outbuf, '*', sizeof(outbuf));
                    outbuf[sizeof(outbuf)-1] = 0;

                    rc = Util::generate(outbuf, theDatetime, k);
                    LOOP3_ASSERT(LINE,
                                 expected,
                                 outbuf,
                                 rc == TEST_LENGTH);

                    rc = bsl::memcmp(outbuf, expected, TEST_LENGTH);
                    LOOP3_ASSERT(LINE, expected, outbuf, rc == 0);

                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 outbuf[TEST_LENGTH] == 0);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 outbuf[TEST_LENGTH + 1] == '*');
                }

                Util::generate(output, theDatetime);
                LOOP3_ASSERT(LINE, expected, output.str(),
                             output.str() == expected);
                if (veryVerbose) { P_(expected); P(output.str()); }
            }

            // Testing bdlt::DateTz
            {
                output.str("");

                bsl::strcpy(expected, dateStr);
                bsl::strcat(expected, offsetStr);
                int TEST_LENGTH = Util::BDEPU_DATETZ_STRLEN;
                int k;
                // test short output buffers
                for (k=0; k < TEST_LENGTH + 1; ++k) {

                    bsl::memset(outbuf, '*', sizeof(outbuf));
                    outbuf[sizeof(outbuf)-1] = 0;

                    rc = Util::generate(outbuf, theDateTz, k);
                    LOOP3_ASSERT(LINE,
                                 expected,
                                 outbuf,
                                 rc == TEST_LENGTH);

                    rc = bsl::memcmp(outbuf, expected, k);
                    LOOP3_ASSERT(LINE, expected, outbuf, rc == 0);

                    // check that "outbuf" bytes started from "k"
                    // have not been changed
                    for (int m = k; m <  TEST_LENGTH + 2; ++m) {

                        LOOP3_ASSERT(LINE,
                                     expected,
                                     outbuf,
                                     outbuf[m] == '*');
                    }
                }

                // test long enough output buffers
                for (; k < TEST_LENGTH + 5; ++k) {

                    bsl::memset(outbuf, '*', sizeof(outbuf));
                    outbuf[sizeof(outbuf)-1] = 0;

                    rc = Util::generate(outbuf, theDateTz, k);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 rc == TEST_LENGTH);

                    rc = bsl::memcmp(outbuf, expected, TEST_LENGTH);
                    LOOP3_ASSERT(LINE, expected, outbuf, rc == 0);

                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 outbuf[TEST_LENGTH] == 0);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 outbuf[TEST_LENGTH + 1] == '*');
                }

                Util::generate(output, theDateTz);
                LOOP3_ASSERT(LINE, expected, output.str(),
                             output.str() == expected);
                if (veryVerbose) { P_(expected); P(output.str()); }
            }

            // Testing bdlt::Date
            {
                output.str("");

                bsl::strcpy(expected, dateStr);

                int TEST_LENGTH = Util::BDEPU_DATE_STRLEN;
                int k;
                // test short output buffers
                for (k=0; k < TEST_LENGTH + 1; ++k) {

                    bsl::memset(outbuf, '*', sizeof(outbuf));
                    outbuf[sizeof(outbuf)-1] = 0;

                    rc = Util::generate(outbuf, theDate, k);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 rc == TEST_LENGTH);

                    rc = bsl::memcmp(outbuf, expected, k);
                    LOOP3_ASSERT(LINE, expected, outbuf, rc == 0);

                    // check that "outbuf" bytes started from "k"
                    // have not been changed
                    for (int m = k; m <  TEST_LENGTH + 2; ++m) {

                        LOOP3_ASSERT(LINE,
                                     expected,
                                     outbuf,
                                     outbuf[m] == '*');
                    }
                }

                // test long enough output buffers
                for (; k < TEST_LENGTH + 5; ++k) {

                    bsl::memset(outbuf, '*', sizeof(outbuf));
                    outbuf[sizeof(outbuf)-1] = 0;

                    rc = Util::generate(outbuf, theDate, k);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 rc == TEST_LENGTH);

                    rc = bsl::memcmp(outbuf, expected, TEST_LENGTH);
                    LOOP3_ASSERT(LINE, expected, outbuf, rc == 0);

                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 outbuf[TEST_LENGTH] == 0);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 outbuf[TEST_LENGTH + 1] == '*');
                }

                Util::generate(output, theDate);
                LOOP3_ASSERT(LINE, expected, output.str(),
                             output.str() == expected);
                if (veryVerbose) { P_(expected); P(output.str()); }
            }

            // Testing bdlt::TimeTz
            {
                output.str("");

                bsl::strcpy(expected, timeStr);
                bsl::strcat(expected, offsetStr);
                int TEST_LENGTH = Util::BDEPU_TIMETZ_STRLEN;
                int k;
                // test short output buffers
                for (k=0; k < TEST_LENGTH + 1; ++k) {

                    bsl::memset(outbuf, '*', sizeof(outbuf));
                    outbuf[sizeof(outbuf)-1] = 0;

                    rc = Util::generate(outbuf, theTimeTz, k);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 rc == TEST_LENGTH);

                    rc = bsl::memcmp(outbuf, expected, k);
                    LOOP3_ASSERT(LINE, expected, outbuf, rc == 0);

                    // check that "outbuf" bytes started from "k"
                    // have not been changed
                    for (int m = k; m <  TEST_LENGTH + 2; ++m) {

                        LOOP3_ASSERT(LINE,
                                     expected,
                                     outbuf,
                                     outbuf[m] == '*');
                    }
                }

                // test long enough output buffers
                for (; k < TEST_LENGTH + 5; ++k) {

                    bsl::memset(outbuf, '*', sizeof(outbuf));
                    outbuf[sizeof(outbuf)-1] = 0;

                    rc = Util::generate(outbuf, theTimeTz, k);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 rc == TEST_LENGTH);

                    rc = bsl::memcmp(outbuf, expected, TEST_LENGTH);
                    LOOP3_ASSERT(LINE, expected, outbuf, rc == 0);

                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 outbuf[TEST_LENGTH] == 0);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 outbuf[TEST_LENGTH + 1] == '*');
                }

                Util::generate(output, theTimeTz);
                LOOP3_ASSERT(LINE, expected, output.str(),
                             output.str() == expected);
                if (veryVerbose) { P_(expected); P(output.str()); }
            }

            // Testing bdlt::Time
            {
                output.str("");

                bsl::strcpy(expected, timeStr);
                int TEST_LENGTH = Util::BDEPU_TIME_STRLEN;
                int k;
                // test short output buffers
                for (k=0; k < TEST_LENGTH + 1; ++k) {

                    bsl::memset(outbuf, '*', sizeof(outbuf));
                    outbuf[sizeof(outbuf)-1] = 0;

                    rc = Util::generate(outbuf, theTime, k);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 rc == TEST_LENGTH);

                    // check that "outbuf" bytes started from "k"
                    // have not been changed
                    rc = bsl::memcmp(outbuf, expected, k);
                    LOOP3_ASSERT(LINE, expected, outbuf, rc == 0);

                    for (int m = k; m <  TEST_LENGTH + 2; ++m) {

                        LOOP3_ASSERT(LINE,
                                     expected,
                                     outbuf,
                                     outbuf[m] == '*');
                    }
                }

                // test long enough output buffers
                for (; k < TEST_LENGTH + 5; ++k) {

                    bsl::memset(outbuf, '*', sizeof(outbuf));
                    outbuf[sizeof(outbuf)-1] = 0;

                    rc = Util::generate(outbuf, theTime, k);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 rc == TEST_LENGTH);

                    rc = bsl::memcmp(outbuf, expected, TEST_LENGTH);
                    LOOP3_ASSERT(LINE, expected, outbuf, rc == 0);

                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 outbuf[TEST_LENGTH] == 0);
                    LOOP3_ASSERT(LINE, expected, outbuf,
                                 outbuf[TEST_LENGTH + 1] == '*');
                }

                Util::generate(output, theTime);
                LOOP3_ASSERT(LINE, expected, output.str(),
                             output.str() == expected);
                if (veryVerbose) { P_(expected); P(output.str()); }
            }

        }
    }

    if (verbose)
        cout << "\tTest 'useZAbbreviationForUtc' optional argument.\n";

    {
        bdlt::Datetime datetime(2013, 8, 23, 11, 30, 5, 1);
        bsl::string dateStr("2013-08-23");
        bsl::string timeStr("11:30:05.001");
        bsl::string datetimeStr("2013-08-23T11:30:05.001");

        for (int i = 0; i < NUM_UTC_OFFSETS; ++i ) {
            const int UTC_OFFSET = UTC_OFFSETS[i];
            bdlt::DateTz dateTz(datetime.date(), UTC_OFFSET);
            bdlt::TimeTz timeTz(datetime.time(), UTC_OFFSET);
            bdlt::DatetimeTz datetimeTz(datetime, UTC_OFFSET);

            char offsetBuffer[10];
            bsl::sprintf(offsetBuffer, "%+03d:%02d",
                         UTC_OFFSET / 60, bsl::abs(UTC_OFFSET) % 60);
            bsl::string offsetStr(offsetBuffer);
            for (int useZ = 0; useZ <= 1; ++useZ) {
                bsl::string expectedDate(dateStr);
                bsl::string expectedTime(timeStr);
                bsl::string expectedDatetime(datetimeStr);

                if (useZ && 0 == UTC_OFFSET) {
                    expectedDate     += "Z";
                    expectedTime     += "Z";
                    expectedDatetime += "Z";
                }
                else {
                    expectedDate     += offsetStr;
                    expectedTime     += offsetStr;
                    expectedDatetime += offsetStr;
                }

                bsl::vector<char> dateOutput(100, '*');
                bsl::vector<char> timeOutput(100, '*');
                bsl::vector<char> datetimeOutput(100, '*');

                bsl::ostringstream dateStream;
                bsl::ostringstream timeStream;
                bsl::ostringstream datetimeStream;

                unsigned int dateLen =
                    Util::generate(dateOutput.data(), dateTz, 100, useZ);
                unsigned int timeLen =
                    Util::generate(timeOutput.data(), timeTz, 100, useZ);
                unsigned int datetimeLen =
                    Util::generate(
                        datetimeOutput.data(), datetimeTz, 100, useZ);

                Util::generate(dateStream, dateTz, useZ);
                Util::generate(timeStream, timeTz, useZ);
                Util::generate(datetimeStream, datetimeTz, useZ);

                ASSERTV(dateLen, bsl::strlen(dateOutput.data()),
                        dateLen == bsl::strlen(dateOutput.data()));
                ASSERTV(timeLen, bsl::strlen(timeOutput.data()),
                        timeLen == bsl::strlen(timeOutput.data()));
                ASSERTV(datetimeLen, bsl::strlen(datetimeOutput.data()),
                        datetimeLen == bsl::strlen(datetimeOutput.data()));
                if (veryVeryVerbose) {
                    P_(dateOutput.data());
                    P_(timeOutput.data());
                    P(datetimeOutput.data());
                }
                ASSERTV(expectedDate, dateOutput.data(),
                        expectedDate == dateOutput.data());
                ASSERTV(expectedTime, timeOutput.data(),
                        expectedTime == timeOutput.data());
                ASSERTV(expectedDatetime, datetimeOutput.data(),
                        expectedDatetime == datetimeOutput.data());

                ASSERTV(expectedDate, dateStream.str(),
                        expectedDate == dateStream.str());
                ASSERTV(expectedTime, timeStream.str(),
                        expectedTime == timeStream.str());
                ASSERTV(expectedDatetime, datetimeStream.str(),
                        expectedDatetime == datetimeStream.str());

            }
        }
    }

    if (verbose)
        cout << "\tTest 'setUseZAbbreviationForUtc' configuration.\n";

    for (int i = 0; i < NUM_UTC_OFFSETS; ++i ) {
        const int UTC_OFFSET = UTC_OFFSETS[i];

        bdlt::Date date(2013, 8, 23);
        bdlt::Time time(11, 30, 5, 1);
        bdlt::Datetime datetime(date, time);

        bdlt::DateTz dateTz(date, UTC_OFFSET);
        bdlt::TimeTz timeTz(time, UTC_OFFSET);
        bdlt::DatetimeTz datetimeTz(datetime, UTC_OFFSET);

        char offsetBuffer[10];
        bsl::sprintf(offsetBuffer, "%+03d:%02d",
                     UTC_OFFSET / 60, bsl::abs(UTC_OFFSET) % 60);

        bsl::string dateStr("2013-08-23");
        bsl::string timeStr("11:30:05.001");
        bsl::string datetimeStr("2013-08-23T11:30:05.001");
        bsl::string offsetStr(offsetBuffer);

        bsl::string expectedDateNoZ(dateStr + offsetStr);
        bsl::string expectedTimeNoZ(timeStr + offsetStr);
        bsl::string expectedDatetimeNoZ(datetimeStr + offsetStr);

        bsl::string expectedDateWithZ(dateStr);
        bsl::string expectedTimeWithZ(timeStr);
        bsl::string expectedDatetimeWithZ(datetimeStr);

        if (0 != UTC_OFFSET) {
            expectedDateWithZ     += offsetStr;
            expectedTimeWithZ     += offsetStr;
            expectedDatetimeWithZ += offsetStr;
        }
        else {
            expectedDateWithZ     += "Z";
            expectedTimeWithZ     += "Z";
            expectedDatetimeWithZ += "Z";
        }

        // Test generated vs expected values with Z enabled.
        bdlpuxxx::Iso8601Configuration::setUseZAbbreviationForUtc(true);
        {
            bsl::vector<char> dateOutput(100, '*');
            bsl::vector<char> timeOutput(100, '*');
            bsl::vector<char> datetimeOutput(100, '*');
            bsl::ostringstream dateStream;
            bsl::ostringstream timeStream;
            bsl::ostringstream datetimeStream;


            unsigned int dateLen =
                Util::generate(dateOutput.data(), dateTz, 100);
            unsigned int timeLen =
                Util::generate(timeOutput.data(), timeTz, 100);
            unsigned int datetimeLen =
                Util::generate(datetimeOutput.data(), datetimeTz, 100);

            Util::generate(dateStream, dateTz);
            Util::generate(timeStream, timeTz);
            Util::generate(datetimeStream, datetimeTz);


            ASSERTV(dateLen, bsl::strlen(dateOutput.data()),
                    dateLen == bsl::strlen(dateOutput.data()));
            ASSERTV(timeLen, bsl::strlen(timeOutput.data()),
                    timeLen == bsl::strlen(timeOutput.data()));
            ASSERTV(datetimeLen, bsl::strlen(datetimeOutput.data()),
                    datetimeLen == bsl::strlen(datetimeOutput.data()));

            if (veryVeryVerbose) {
                P_(dateOutput.data());
                P_(timeOutput.data());
                P(datetimeOutput.data());
            }
            ASSERTV(expectedDateWithZ, dateOutput.data(),
                    expectedDateWithZ == dateOutput.data());
            ASSERTV(expectedTimeWithZ, timeOutput.data(),
                    expectedTimeWithZ == timeOutput.data());
            ASSERTV(expectedDatetimeWithZ, datetimeOutput.data(),
                    expectedDatetimeWithZ == datetimeOutput.data());

            ASSERTV(expectedDateWithZ, dateStream.str(),
                    expectedDateWithZ == dateStream.str());
            ASSERTV(expectedTimeWithZ, timeStream.str(),
                    expectedTimeWithZ == timeStream.str());
            ASSERTV(expectedDatetimeWithZ, datetimeStream.str(),
                    expectedDatetimeWithZ == datetimeStream.str());
        }
        // Test generated vs expected values with Z disabled.
        bdlpuxxx::Iso8601Configuration::setUseZAbbreviationForUtc(false);
        {
            bsl::vector<char> dateOutput(100, '*');
            bsl::vector<char> timeOutput(100, '*');
            bsl::vector<char> datetimeOutput(100, '*');
            bsl::ostringstream dateStream;
            bsl::ostringstream timeStream;
            bsl::ostringstream datetimeStream;

            unsigned int dateLen =
                Util::generate(dateOutput.data(), dateTz, 100);
            unsigned int timeLen =
                Util::generate(timeOutput.data(), timeTz, 100);
            unsigned int datetimeLen =
                Util::generate(datetimeOutput.data(), datetimeTz, 100);

            Util::generate(dateStream, dateTz);
            Util::generate(timeStream, timeTz);
            Util::generate(datetimeStream, datetimeTz);

            ASSERTV(dateLen, bsl::strlen(dateOutput.data()),
                    dateLen == bsl::strlen(dateOutput.data()));
            ASSERTV(timeLen, bsl::strlen(timeOutput.data()),
                    timeLen == bsl::strlen(timeOutput.data()));
            ASSERTV(datetimeLen, bsl::strlen(datetimeOutput.data()),
                    datetimeLen == bsl::strlen(datetimeOutput.data()));

            if (veryVeryVerbose) {
                P_(dateOutput.data());
                P_(timeOutput.data());
                P(datetimeOutput.data());
            }

            ASSERTV(expectedDateNoZ, dateOutput.data(),
                    expectedDateNoZ == dateOutput.data());
            ASSERTV(expectedTimeNoZ, timeOutput.data(),
                    expectedTimeNoZ == timeOutput.data());
            ASSERTV(expectedDatetimeNoZ, datetimeOutput.data(),
                    expectedDatetimeNoZ == datetimeOutput.data());


            ASSERTV(expectedDateNoZ, dateStream.str(),
                    expectedDateNoZ == dateStream.str());
            ASSERTV(expectedTimeNoZ, timeStream.str(),
                    expectedTimeNoZ == timeStream.str());
            ASSERTV(expectedDatetimeNoZ, datetimeStream.str(),
                    expectedDatetimeNoZ == datetimeStream.str());
        }
    }
}
//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    (void) veryVeryVerbose;    // eliminate unused variable warning

    BSLS_ASSERT(!bdlt::DelegatingDateImpUtil::isProlepticGregorianMode());
    if (!veryVeryVerbose) {
        // Except when in 'veryVeryVerbose' mode, suppress logging performed by
        // the (private) 'bdlt::Date::logIfProblematicDate*' methods.  When
        // those methods are removed, the use of a log message handler should
        // be removed.

        bsls::Log::setLogMessageHandler(&noopLogMessageHandler);
    }

    if (veryVerbose) {
        cout << "The calendar mode in effect is "
             << (bdlt::DelegatingDateImpUtil::isProlepticGregorianMode()
                 ? "proleptic Gregorian"
                 : "Gregorian (POSIX)")
             << endl;
    }

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 8: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern: that the usage example in the header compile and
        //    run as expected.
        //
        // Plan:
        //    Copy the usage example verbatim, but substitute 'ASSERT' for
        //    each 'assert'.
        //
        // Testing:
        //    USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nUSAGE EXAMPLE"
                               << "\n=============" << bsl::endl;

// The following example illustrates how to generate an ISO 8601-compliant
// string from a 'bdlt::DatetimeTz' value:
//..
    const bdlt::DatetimeTz theDatetime(bdlt::Datetime(2005, 1, 31,
                                                    8, 59, 59, 123), 240);
    bsl::stringstream ss;
    bdlpuxxx::Iso8601::generate(ss, theDatetime);
    ASSERT(ss.str() == "2005-01-31T08:59:59.123+04:00");
//..
// The following example illustrates how to parse an ISO 8601-compliant string
// into a 'bdlt::DatetimeTz' object:
//..
    bdlt::DatetimeTz dateTime;
    const char dtStr[] = "2005-01-31T08:59:59.1226-04:00";
    int ret = bdlpuxxx::Iso8601::parse(&dateTime, dtStr, bsl::strlen(dtStr));
    ASSERT(0 == ret);
    ASSERT(2005 == dateTime.localDatetime().year());
    ASSERT(   1 == dateTime.localDatetime().month());
    ASSERT(  31 == dateTime.localDatetime().day());
    ASSERT(   8 == dateTime.localDatetime().hour());
    ASSERT(  59 == dateTime.localDatetime().minute());
    ASSERT(  59 == dateTime.localDatetime().second());
    ASSERT( 123 == dateTime.localDatetime().millisecond());
    ASSERT(-240 == dateTime.offset());
//..
// Note that fractions of a second was rounded up to 123 milliseconds and that
// the offset from UTC was converted to minutes.

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING: bdlpuxxx::Iso8601Configuration
        //
        // Concerns:
        //: 1 The default value for 'useZAbbreviationForUtc' is 'false'.
        //:
        //: 2 Calling 'setUseZAbbreviationForUtc' sets
        //:   'useZAbbreviationForUtc'  to the indicated status.
        //
        // Plan:
        //: 1 Verify that if 'setUseZAbbreviationForUtc' is not called,
        //:   'useZAbbreviationForUtc' returns 'false'. (C-1)
        //:
        //: 2 Call 'setUseZAbbreviationForUtc' with both 'true' and 'false'
        //:   and verify that 'useZAbbreviationForUtc' returns the
        //:   corresponding state. (C-2)
        //
        // Testing:
        //  void setUseZAbbreviationForUtc(bool);
        //  bool useZAbbreviationForUtc();
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING: bdlpuxxx::Iso8601Configuration"
                               << "\n==================================="
                               << bsl::endl;

        ASSERT(false == bdlpuxxx::Iso8601Configuration::useZAbbreviationForUtc());

        bdlpuxxx::Iso8601Configuration::setUseZAbbreviationForUtc(false);
        ASSERT(false == bdlpuxxx::Iso8601Configuration::useZAbbreviationForUtc());

        bdlpuxxx::Iso8601Configuration::setUseZAbbreviationForUtc(true);
        ASSERT(true == bdlpuxxx::Iso8601Configuration::useZAbbreviationForUtc());

        bdlpuxxx::Iso8601Configuration::setUseZAbbreviationForUtc(false);
        ASSERT(false == bdlpuxxx::Iso8601Configuration::useZAbbreviationForUtc());
      }
      case 6: {
        // --------------------------------------------------------------------
        // CONCERN: parsing leap-seconds
        //
        // Concerns:
        //   That leap seconds are correctly parsed.
        //
        // Plan:
        //   Parse pairs of objects, one with the expected time not parsed as
        //   a leap second, one as a leap second, and compare them for
        //   equality.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "TESTING LEAP SECONDS\n"
                                  "====================\n";

        int rc;

        if (verbose) Q(bdlt::Time);
        {
            bdlt::Time garbage(17, 42, 37, 972), expected(0), parsed;

            rc = myParse(&parsed, "23:59:60");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsed, expected == parsed);

            rc = expected.setTimeIfValid(15, 48, 0, 345);
            ASSERT(0 == rc);

            parsed = garbage;
            rc = myParse(&parsed, "15:47:60.345");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsed, expected == parsed);

            rc = expected.setTimeIfValid(15, 48, 1);
            ASSERT(0 == rc);

            parsed = garbage;
            rc = myParse(&parsed, "15:47:60.9996");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsed, expected == parsed);

            rc = expected.setTimeIfValid(15, 48, 0, 345);
            ASSERT(0 == rc);

            if (verbose) Q(bdlt::TimeTz);

            const bdlt::TimeTz garbageTz(garbage, 274);
            bdlt::TimeTz expectedTz(bdlt::Time(0), 0), parsedTz(garbageTz);

            rc = myParse(&parsedTz, "23:59:60");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsedTz, expectedTz == parsedTz);

            expectedTz.setTimeTz(expected, 0);

            parsedTz = garbageTz;
            rc = myParse(&parsedTz, "15:47:60.345");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsedTz, expectedTz == parsedTz);

            expected.addHours(3);
            expectedTz.setTimeTz(expected, 240);

            parsedTz = garbageTz;
            rc = myParse(&parsedTz, "18:47:60.345+04:00");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsedTz, expectedTz == parsedTz);
        }

        if (verbose) Q(bdlt::Datetime);
        {
            const bdlt::Datetime garbage(1437, 7, 6, 19, 41, 12, 832);
            bdlt::Datetime expected(1, 1, 1, 1, 0, 0, 0), parsed(garbage);

            rc = myParse(&parsed, "0001-01-01T00:59:60");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsed, expected == parsed);

            expected.addTime(0, 0, 1);

            parsed = garbage;
            rc = myParse(&parsed, "0001-01-01T00:59:60.9996");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsed, expected == parsed);

            expected.addTime(0, 0, -1, 345);

            rc = myParse(&parsed, "0001-01-01T00:59:60.345");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsed, expected == parsed);

            expected.setDatetime(1, 1, 2, 0);

            parsed = garbage;
            rc = myParse(&parsed, "0001-01-01T23:59:60");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsed, expected == parsed);

            expected.setDatetime(1, 1, 2, 0, 0, 1);

            rc = myParse(&parsed, "0001-01-01T23:59:60.9996");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsed, expected == parsed);

            expected.setDatetime(1963, 11, 22, 12, 31, 0);

            parsed = garbage;
            rc = myParse(&parsed, "1963-11-22T12:30:60");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsed, expected == parsed);

            expected.setDatetime(1, 1, 2, 0, 0, 0);

            if (verbose) Q(bdlt::Datetime);

            bdlt::DatetimeTz garbageTz(garbage, 281);
            bdlt::DatetimeTz expectedTz(expected, 0), parsedTz(garbageTz);

            rc = myParse(&parsedTz, "0001-01-01T23:59:60");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsedTz, expectedTz == parsedTz);

            expected.addTime(0, 0, 0, 345);
            expectedTz.setDatetimeTz(expected, 120);

            parsedTz = garbageTz;
            rc = myParse(&parsedTz, "0001-01-01T23:59:60.345+02:00");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsedTz, expectedTz == parsedTz);

            expected.setDatetime(1, 1, 2, 0, 0, 1, 0);
            expectedTz.setDatetimeTz(expected, -720);

            parsedTz = garbageTz;
            rc = myParse(&parsedTz, "0001-01-01T23:59:60.99985-12:00");
            ASSERT(0 == rc);
            LOOP_ASSERT(parsedTz, expectedTz == parsedTz);
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // CONCERN: Unusual time zone offsets
        //
        // Concern:
        //   Test that time zones accept all valid inputs and reject
        //   appropriately.
        //
        // Plan:
        //   Exhaustively test all values of hh and mm, then in a separate
        //   loop test some other values.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "TESTING TIME ZONES\n"
                                  "==================\n";

        if (verbose) Q(Exhaustive test);
        {
            for (int sign = 0; sign <= 1; ++sign) {
                for (int hh = 0; hh <= 99; ++hh) {
                    for (int mm = 0; mm <= 99; ++mm) {
                        char tzBuf[10];
                        sprintf(tzBuf, "%c%02d:%02d", (sign ? '-' : '+'),
                                                      hh, mm);
                        const bool VALID = hh < 24 && mm <= 59;
                        const int OFFSET = (sign ? -1 : 1) * (hh * 60 + mm);

                        testTimezone(tzBuf, VALID, OFFSET);
                    }
                }
            }
        }

        if (verbose) Q(Table test);
        {
            static const struct {
                const char *d_tzStr;
                bool        d_valid;
                int         d_offset;    // note only examined if 'valid' is
                                         // true
            } DATA[] = {
                // tzStr      valid  offset
                // ---------  -----  ------
                { "+123:123",     0,      0 },
                { "+12:12",       1,    732 },
                { "-12:12",       1,   -732 },
                { "z",            1,      0 },
                { "Z",            1,      0 },
                { "",             1,      0 },
                { "+123:23",      0,      0 },
                { "+12:123",      0,      0 },
                { "+011:23",      0,      0 },
                { "+12:011",      0,      0 },
                { "+1:12",        0,      0 },
                { "+12:1",        0,      0 },
                { "+a1:12",       0,      0 },
                { "z0",           0,      0 },
                { "0",            0,      0 },
                { "T",            0,      0 },
                { "+",            0,      0 },
                { "-",            0,      0 },
                { "+0",           0,      0 },
                { "-0",           0,      0 },
                { "+01",          0,      0 },
                { "-01",          0,      0 },
                { "+01:",         0,      0 },
                { "-01:",         0,      0 },
                { "+01:1",        0,      0 },
                { "-01:1",        0,      0 },
            };
            enum { NUM_DATA = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < NUM_DATA; ++i) {
                const char *TZ_STR = DATA[i].d_tzStr;
                const bool  VALID  = DATA[i].d_valid;
                const int   OFFSET = DATA[i].d_offset;

                testTimezone(TZ_STR, VALID, OFFSET);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PARSE
        //
        // Concerns:
        //  1 'parse' will correctly accept a iso8601 value having the full
        //     range of dates.
        //
        //  2 'parse' will correctly accept a iso8601 value having the full
        //     range of normal times.
        //
        //  3 'parse' will translate fractional seconds to milliseconds
        //
        //  4 'parse' will round fractional seconds beyond milliseconds to the
        //     nearest millisecond.
        //
        //  5 'parse' will round fractional seconds beyond milliseconds to the
        //     nearest millisecond.
        //
        //  6 'parse' will accept a time zone value in the range of
        //    (-1440, 1440).
        //
        //  7 'parse' will load a 'bdlt::DatetimTz' with a Tz having the parse
        //    time zone offset.
        //
        //  8 'parse' will load a 'bdlt::Datetime' by converting a time with
        //    a time-zone to its corresponding UTC time.
        //
        //  9 'parse' will return a time of 24:00:00.000 as 00:00:00.000 of the
        //     same day (see 'Note Regarding the Time 24:00' in
        //     bdlpuxxx_iso8601.h).
        //
        // 10 'parse' will not accept hour 24 if minutes, seconds, or
        //     fractional seconds is non-zero.
        //
        // 11 'parse' will convert a seconds value of 60 (leap-second) to the
        //    first second of the subsequent minute.
        //
        // 12 'parse' will only parse up to the supplied input length
        //
        // 13 'parse' will return a non-zero value if the input is not a valid
        //     ISO-8601 string.
        //
        // 14 'parse' will return a non-zero value if the input is not in the
        //    valid range of representable Datetime or DatetimeTz values.
        //
        // 15 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //  1 For a table of valid ISO-8601 strings, call 'parse' and compare
        //    the resulting 'bdlt::Datetime' and 'bdlt::DatetimeTz' values
        //    against their expected value. (C-1..5, 9, 11).
        //
        //  2 For a table of valid ISO-8601 strings having time zones, call
        //    'parse' and compare the resulting 'bdlt::Datetime' and
        //    'bdlt::DatetimeTz' values against their expected value. (C-6..8)
        //
        //  3 For a table of invalid ISO-8601 strings or ISO-8601 strings
        //    outside the representable range of values, call 'parse' (for both
        //    datetime and datetimetz) and verify they return a non-zero
        //    status (C13..14)
        //
        //  4 For a table of ISO-8601 strings that are *within* the
        //    representable range of 'bdlt::DatetimeTz' *but* *outside* the
        //    representable range of 'bdlt::Datetime', call 'parse', and verify
        //    the returned 'bdlt::DatetimeTz' has the expected value, and the
        //    overload taking a 'bdlt::Datetime' returns a non-zero status.
        //    (C6..8, 14)
        //
        //  5 For a valid ISO-8601 string, 'INPUT', iterate over the possible
        //    string lengths to provide to 'parse' (i.e., 0 through
        //    'strlen(INPUT)'), and verify the 'parse' status returned by
        //    parse against the expected return status for that length.
        //
        //  6 Verify that, in appropriate build modes, defensive checks are
        //    triggered when an attempt is made to 'parse' if provided an
        //    invalid result object, input string, and string length
        //    (using the 'BSLS_ASSERTTEST_*' macros).  (C-15)
        //
        // Testing:
        //   static int parse(bdlt::Datetime *, const char *, int);
        //   static int parse(bdlt::DatetimeTz *, const char *, int);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING PARSE"
                               << "\n=============" << bsl::endl;

        if (verbose) cout << "\nTesting valid datetime values."
                          << endl;
        {
            const struct {
                int         d_line;
                const char *d_input;
                int         d_year;
                int         d_month;
                int         d_day;
                int         d_hour;
                int         d_minute;
                int         d_second;
                int         d_millisecond;
            } DATA[] = {

            // Test range end points
  { L_, "0001-01-01T00:00:00.000"       , 0001, 01, 01, 00, 00, 00, 000 },
  { L_, "9999-12-31T23:59:59.999"       , 9999, 12, 31, 23, 59, 59, 999 },

            // Test random dates
  { L_, "1234-02-23T12:34:45.123"       , 1234, 02, 23, 12, 34, 45, 123 },
  { L_, "2014-12-15T17:03:56.243"       , 2014, 12, 15, 17, 03, 56, 243 },

            // Test fractional millisecond rounding
  { L_, "0001-01-01T00:00:00.00001"     , 0001, 01, 01, 00, 00, 00, 000 },
  { L_, "0001-01-01T00:00:00.00049"     , 0001, 01, 01, 00, 00, 00, 000 },
  { L_, "0001-01-01T00:00:00.00050"     , 0001, 01, 01, 00, 00, 00, 001 },
  { L_, "0001-01-01T00:00:00.00099"     , 0001, 01, 01, 00, 00, 00, 001 },

            // Test fractional millisecond rounding to 1000
  { L_, "0001-01-01T00:00:00.9994"      , 0001, 01, 01, 00, 00, 00, 999 },
  { L_, "0001-01-01T00:00:00.9995"      , 0001, 01, 01, 00, 00, 01, 000 },

            // Test without fractional seconds
  { L_, "1234-02-23T12:34:45"           , 1234, 02, 23, 12, 34, 45, 000 },
  { L_, "2014-12-15T17:03:56"           , 2014, 12, 15, 17, 03, 56, 000 },

            // Test leap-seconds
  { L_, "0001-01-01T00:00:60.000"       , 0001, 01, 01, 00, 01, 00, 000 },
  { L_, "9998-12-31T23:59:60.999"       , 9999, 01, 01, 00, 00, 00, 999 },

            // Test special case 24:00:00 (midnight) values
  { L_, "0001-01-01T24:00:00.000"       , 0001, 01, 01, 24, 00, 00, 000 },
  { L_, "2001-01-01T24:00:00.000"       , 2001, 01, 01, 24, 00, 00, 000 },
  { L_, "0001-01-01T24:00:00"           , 0001, 01, 01, 24, 00, 00, 000 },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const char       *INPUT       = DATA[i].d_input;

                bdlt::Datetime   result(4321,1,2,3,4,5,6);
                bdlt::DatetimeTz resultTz(result, -123);

                bdlt::Datetime   EXPECTED(DATA[i].d_year,
                                         DATA[i].d_month,
                                         DATA[i].d_day,
                                         DATA[i].d_hour,
                                         DATA[i].d_minute,
                                         DATA[i].d_second,
                                         DATA[i].d_millisecond);
                bdlt::DatetimeTz EXPECTEDTZ(EXPECTED, 0);

                ASSERTV(LINE, 0 == Util::parse(&result, INPUT, strlen(INPUT)));
                ASSERTV(LINE, EXPECTED, result, EXPECTED == result);

                ASSERTV(LINE,
                        0 == Util::parse(&resultTz, INPUT, strlen(INPUT)));
                ASSERTV(LINE, EXPECTEDTZ, resultTz, EXPECTEDTZ == resultTz);
            }
        }

        if (verbose) cout << "\nTesting valid datetime values w/ time zone."
                          << endl;
        {
            const struct {
                int         d_line;
                const char *d_input;
                int         d_year;
                int         d_month;
                int         d_day;
                int         d_hour;
                int         d_minute;
                int         d_second;
                int         d_millisecond;
                int         d_tzOffset;
            } DATA[] = {

            // Test with time zone
  { L_, "1000-01-01T00:00:00.000+00:00" , 1000, 01, 01, 00, 00, 00, 000,  0 },
  { L_, "1000-01-01T00:00:00.000+00:01" , 1000, 01, 01, 00, 00, 00, 000,  1 },
  { L_, "1000-01-01T00:00:00.000-00:01" , 1000, 01, 01, 00, 00, 00, 000, -1 },

  { L_, "2000-01-01T00:00:00.000+23:59" , 2000, 01, 01, 00, 00, 00, 000, 1439},
  { L_, "2000-01-01T00:00:00.000-23:59" , 2000, 01, 01, 00, 00, 00, 000,-1439},

  { L_, "0001-01-01T00:00:00.000Z"      , 0001, 01, 01, 00, 00, 00, 000,  0 },
  { L_, "9999-12-31T23:59:59.999Z"      , 9999, 12, 31, 23, 59, 59, 999,  0 },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const char       *INPUT       = DATA[i].d_input;

                bdlt::Datetime   result(4321,1,2,3,4,5,6);
                bdlt::DatetimeTz resultTz(result, -123);
                bdlt::DatetimeTz EXPECTEDTZ(
                                  bdlt::Datetime(DATA[i].d_year,
                                                DATA[i].d_month,
                                                DATA[i].d_day,
                                                DATA[i].d_hour,
                                                DATA[i].d_minute,
                                                DATA[i].d_second,
                                                DATA[i].d_millisecond),
                                  DATA[i].d_tzOffset);
                bdlt::Datetime   EXPECTED(EXPECTEDTZ.utcDatetime());

                ASSERTV(LINE, 0 == Util::parse(&result, INPUT, strlen(INPUT)));
                ASSERTV(LINE, EXPECTED, result, EXPECTED == result);

                ASSERTV(LINE,
                        0 == Util::parse(&resultTz, INPUT, strlen(INPUT)));
                ASSERTV(LINE, EXPECTEDTZ, resultTz, EXPECTEDTZ == resultTz);
            }
        }

        if (verbose) cout << "\nTesting invalid datetime values."
                          << endl;
        {
            const char *DATA[] = {
                // garbage
                "",
                "asdajksad",

                // Invalid dates
                "0000-01-01T00:00:00.000",
               "10000-01-01T00:00:00.000",
                "2000-00-01T00:00:00.000",
                "2000-13-01T00:00:00.000",
                "2000-01-00T00:00:00.000",
                "2000-01-32T00:00:00.000",
                "2000-02-31T00:00:00.000",
                "2000-2-31T00:00:00.000",
                "2000-02-3T00:00:00.000",

                // Invalid Times
                "2000-01-01T24:01:00.000",
                "2000-01-01T24:00:01.000",
                "2000-01-01T24:00:00.001",
                "2000-01-01T25:00:00.000",
                "2000-01-01T00:60:00.000",
                "2000-01-01T00:00:61.000",

                // Invalid Separators
                "2000/01-01T12:01:00.000",
                "2000-01/01T12:01:00.000",
                "2000-01-01:12:01:00.000",
                "2000-01-01T12 01:00.000",
                "2000-01-01T12:01 00.000",
                "2000-01-01T12:01 00/000",

                // Invalid Time zones
                "2000-01-01T12:01:00.000+23:60",
                "2000-01-01T12:01:00.000+24:00",
                "2000-01-01T12:01:00.000-23:60",
                "2000-01-01T12:01:00.000-24:00",

                // Out-of range values
                "9999-12-31T23:59:60.000+00:00"
                "9999-12-31T23:59:59.9996+00:00"
                "9999-12-31T24:00:00.000+00:00"
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const char *INPUT = DATA[i];

                bdlt::Datetime   result(4321,1,2,3,4,5,6);
                bdlt::DatetimeTz resultTz(result, -213);
                ASSERTV(INPUT, result,
                        0 != Util::parse(&result, INPUT, strlen(INPUT)));
                ASSERTV(INPUT, resultTz,
                        0 != Util::parse(&resultTz, INPUT, strlen(INPUT)));
            }

        }

        if (verbose) cout
            << "\nTesting time zone offsets that cannot be converted to utc"
            << endl;
        {
            struct {
                int         d_line;
                const char *d_input;
                int         d_year;
                int         d_month;
                int         d_day;
                int         d_hour;
                int         d_minute;
                int         d_second;
                int         d_millisecond;
                int         d_tzOffset;
            } DATA[] = {
  { L_, "0001-01-01T00:00:00.000+00:01" , 0001, 01, 01, 00, 00, 00, 000,    1},
  { L_, "0001-01-01T23:58:59.000+23:59" , 0001, 01, 01, 23, 58, 59, 000, 1439},
  { L_, "9999-12-31T23:59:59.999-00:01" , 9999, 12, 31, 23, 59, 59, 999,   -1},
  { L_, "9999-12-31T00:01:00.000-23:59" , 9999, 12, 31, 00, 01, 00, 000,-1439},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;


            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_line;
                const char       *INPUT       = DATA[i].d_input;

                bdlt::Datetime   result(4321,1,2,3,4,5,6);
                bdlt::DatetimeTz resultTz(result, -213);

                bdlt::DatetimeTz EXPECTED(bdlt::Datetime(DATA[i].d_year,
                                                       DATA[i].d_month,
                                                       DATA[i].d_day,
                                                       DATA[i].d_hour,
                                                       DATA[i].d_minute,
                                                       DATA[i].d_second,
                                                       DATA[i].d_millisecond),
                                         DATA[i].d_tzOffset);

                ASSERTV(LINE, INPUT, EXPECTED,
                        0 == Util::parse(&resultTz, INPUT, strlen(INPUT)));

                ASSERTV(LINE, INPUT, EXPECTED, resultTz,
                        EXPECTED == resultTz);

                ASSERTV(LINE, INPUT, EXPECTED,
                        0 != Util::parse(&result, INPUT, strlen(INPUT)));
            }
        }

        if (verbose) cout << "\nTesting length parameter"
                          << endl;
        {
            // Create a test input string 'INPUT', and a set of expected
            // return statuses for 'parse', 'VALID'.  Notice that:
            //..
            // '0 == parse(&out, INPUT, length)' IFF ''V' == VALID[length]'
            //..

            const char *INPUT =  "2013-10-23T01:23:45.678901+12:34111";
            const char *VALID = "IIIIIIIIIIIIIIIIIIIVIVVVVVVIIIIIVIII";

            for (unsigned int len = 0; len < strlen(INPUT) + 1; ++len) {
                bdlt::Datetime   result(4321,1,2,3,4,5,6);
                bdlt::DatetimeTz resultTz(result, -213);

                bool EXPECTED = 'V' == VALID[len];

                ASSERTV(INPUT, len, result, VALID[len],
                        EXPECTED == (0 == Util::parse(&result, INPUT, len)));
                ASSERTV(INPUT, len, resultTz, VALID[len],
                        EXPECTED == (0 == Util::parse(&resultTz, INPUT, len)));
            }
        }
        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const char *INPUT  = "2013-10-23T01:23:45";
            const int   LENGTH = bsl::strlen(INPUT);

            bdlt::Datetime   result;
            bdlt::DatetimeTz resultTz;

            if (veryVerbose) cout << "\t'Invalid result'" << endl;
            {
                bdlt::Datetime   *bad   = 0;
                bdlt::DatetimeTz *badTz = 0;

                ASSERT_PASS(Util::parse(  &result, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(      bad, INPUT, LENGTH));

                ASSERT_PASS(Util::parse(&resultTz, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(    badTz, INPUT, LENGTH));
            }

            if (veryVerbose) cout << "\t'Invalid input'" << endl;
            {
                ASSERT_PASS(Util::parse(  &result, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(  &result,     0, LENGTH));

                ASSERT_PASS(Util::parse(&resultTz, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(&resultTz,     0, LENGTH));
            }

            if (veryVerbose) cout << "\t'Invalid length'" << endl;
            {
                ASSERT_PASS(Util::parse(  &result, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(  &result, INPUT,     -1));

                ASSERT_PASS(Util::parse(&resultTz, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(&resultTz, INPUT,     -1));
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PARSE
        //
        //   Note that the testing for 'parse' for 'Datetime' and 'DatetimeTz'
        //   values has been superseded by test-case 4.  This test case has
        //   been maintained for completeness.
        //
        // Concerns:
        //   The following concerns apply to each of the six types that
        //   can be parsed by this component.
        //   - Valid input strings will produce the correct parse object.
        //   - Invalid input strings return a non-zero return value and do not
        //     modify the object.
        //   - Fractional seconds can be absent.
        //   - If fractional seconds are present, they may be any length and
        //     are rounded to the nearest millisecond.
        //   - If fractional seconds round to 1000 milliseconds, then an
        //     entire second is added to the parsed time object.
        //   - Timezone offset is parsed and applied correctly.
        //   - If timezone offset is absent, it is treated as "+00:00"
        //   - Characters after the end of the parsed string do
        //     not affect the parse.
        //
        // Plan:
        //   - Prepare a set of test vectors with the following values:
        //     + Invalid dates
        //     + Invalid times
        //     + Valid dates and times
        //     + Empty fractional seconds
        //     + Fractional seconds of 1 to 10 fractional digits.  Note that
        //       a large 10-digit number would overflow a 32-bit integer.  Our
        //       test shows that only the first 4 fractional digits are
        //       evaluated, but that the rest do not cause a parse failure.
        //     + Fractional seconds of 4 digits or more where the 4th digit
        //       would cause round-up when converted to milliseconds.
        //     + Fractional seconds of .9995 or more, which would cause
        //       round-up to the next whole second.
        //   - Compose date, time, and datetime strings from each test vector.
        //   - Apply the following orthogonal perturbations:
        //     + A set of timezone offsets, including an empty offset and the
        //       characters "z" and "Z", which are equivalent to "+00:00".
        //     + A superfluous "X" after the end of the parsed string.
        //   - Construct each of the six date, time, and datetime types from
        //     the appropriate input strings.
        //   - Compare the return code and parsed value against expected
        //     values.
        //
        // Testing:
        //     static int parse(bdlt::Date  *result,
        //                      const char *input,
        //                      int         inputLength);
        //     static int parse(bdlt::Datetime *result,
        //                      const char    *input,
        //                      int            inputLength);
        //     static int parse(bdlt::DatetimeTz *result,
        //                      const char      *input,
        //                      int              inputLength);
        //     static int parse(bdlt::DateTz *result,
        //                      const char  *input,
        //                      int          inputLength);
        //     static int parse(bdlt::Time  *result,
        //                      const char *input,
        //                      int         inputLength);
        //     static int parse(bdlt::TimeTz *result,
        //                      const char  *input,
        //                      int          inputLength);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING PARSE"
                               << "\n=============" << bsl::endl;

        testCase3TestingParse();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING: generate
        //
        // Concerns:
        //: 1 All six types generate correct strings for a variety of
        //:   values.
        //:
        //: 2 The Tz types add an offset from UTC to the end of the
        //:   generated string; the other types do not.
        //:
        //: 3 The Tz value can be positive, negative, or zero.
        //:
        //: 4 All time and datetime classes generate a 3-digits for
        //:   fractions of a second.
        //:
        //: 5 Correct behavior in case of short output buffers
        //:
        //: 6 If the optional argument 'useZAbbreviationForUtc' is 'true' the
        //:   output for the TZ offset of a UTC value is 'Z', and '00:00'
        //:   otherwise.
        //:
        //: 7 If the optional argument 'useZAbbreviationForUtc' is not supplied
        //:   the default configuration value is used.
        //
        // Plan:
        //: 1 User a loop-based test over a range of valid date & time values
        //:   selected to test various formatting properties: (C1-5)
        //:   1 For each value, perform an orthogonal perturbation for timezone
        //:     offsets
        //:
        //:   2 For each value, perform an orthogonal perturbation for the
        //:     output buffer length, testing buffers to short for the
        //:     resulting formatted value.
        //:
        //: 2 For a UTC value generate a formatted value of each of the 3 'Tz"
        //:   types with the optional 'useZAbbreviationForUtc' as both 'true'
        //:   and 'false' (C-6)
        //:
        //: 3 For a UTC value generate a formatted value of each of the 3 'Tz"
        //:   types both with, and without, the default useZAbbreviationForUtc
        //:   option enabled. (C-7)
        //
        // Testing:
        //  int generate(char *, const bdlt::Date&, int);
        //  int generate(char *, const bdlt::Time&, int);
        //  int generate(char *, const bdlt::Datetime&, int);
        //  int generate(char *, const bdlt::DateTz&, int);
        //  int generate(char *, const bdlt::DateTz&, int, bool);
        //  int generate(char *, const bdlt::TimeTz&, int);
        //  int generate(char *, const bdlt::TimeTz&, int, bool);
        //  int generate(char *, const bdlt::DatetimeTz&, int);
        //  int generate(char *, const bdlt::DatetimeTz&, int, bool);
        //  int generate(bsl::ostream &, const bdlt::Date&, int);
        //  int generate(bsl::ostream &, const bdlt::Time&, int);
        //  int generate(bsl::ostream &, const bdlt::Datetime&, int);
        //  int generate(bsl::ostream &, const bdlt::DateTz&, int);
        //  int generate(bsl::ostream &, const bdlt::DateTz&, int, bool);
        //  int generate(bsl::ostream &, const bdlt::TimeTz&, int);
        //  int generate(bsl::ostream &, const bdlt::TimeTz&, int, bool);
        //  int generate(bsl::ostream &, const bdlt::DatetimeTz&, int);
        //  int generate(bsl::ostream &, const bdlt::DatetimeTz&, int, bool);
        // --------------------------------------------------------------------

        testCase2TestingGenerate();
      } break;

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns: Every function is callable and produces expected results
        //    under simple circumstances.
        //
        // Plan:
        //    Call each static function with a "representative" input and
        //    verify the expected results.
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nBREATHING TEST"
                               << "\n==============" << bsl::endl;

        if (verbose) {
            bsl::cout << "Performance test:" << bsl::endl;
            bsls::Stopwatch watch;
            bdlt::DatetimeTz datetime;
            const char INPUT[] = "2005-01-31T08:59:59.123-04:00";
            watch.start();
            for (int i = 0; i < 500000; ++i) {
                Util::parse(&datetime, INPUT, sizeof(INPUT));
            }
            watch.stop();
            bsl::cout << "** Time taken = " << watch.elapsedTime() << " s."
                      << bsl::endl;
        }

        {
            bdlt::DatetimeTz dateTime;
            const char str[] = "2005-01-31T08:59:59.12345678901234-04:00";
            int ret = Util::parse(&dateTime, str, sizeof(str)-1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(dateTime,
                        dateTime == bdlt::DatetimeTz(bdlt::Datetime(2005,1,31,
                                                                  8,59,59,123),
                                                    -240));
            if (verbose) P(dateTime);
        }

        {
            bdlt::Datetime dateTime;
            const char str[] = "2005-01-31T08:59:59.1234567890123-04:00";
            int ret = Util::parse(&dateTime, str, sizeof(str)-1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(dateTime,
                        dateTime == bdlt::Datetime(2005,1,31,12,59,59,123));
            if (verbose) P(dateTime);
        }

        {
            bdlt::DateTz date;
            const char str[] = "2005-01-31-04:00";
            int ret = Util::parse(&date, str, sizeof(str)-1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(date,
                        date == bdlt::DateTz(bdlt::Date(2005,1,31), -240));
            if (verbose) P(date);
        }

        {
            bdlt::Date date;
            const char str[] = "2005-01-31-04:00";
            int ret = Util::parse(&date, str, sizeof(str)-1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(date, date == bdlt::Date(2005,1,31))
            if (verbose) P(date);
        }

        {
            bdlt::TimeTz time;
            const char str[] = "08:59:59.1225678901234-04:00";
            int ret = Util::parse(&time, str, sizeof(str)-1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(time,
                        time == bdlt::TimeTz(bdlt::Time(8,59,59,123), -240));
            if (verbose) P(time);
        }

        {
            bdlt::Time time;
            const char str[] = "08:59:59.1225678901234-04:00";
            int ret = Util::parse(&time, str, sizeof(str)-1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(time, time == bdlt::Time(12,59,59,123));
            if (verbose) P(time);
        }

        {
            bdlt::DatetimeTz dateTime(bdlt::Datetime(2005,1,31,8,59,59,123),
                                     -240);
            bsl::stringstream ss;
            bdlpuxxx::Iso8601::generate(ss, dateTime);
            LOOP_ASSERT(ss.str(), ss.str() == "2005-01-31T08:59:59.123-04:00");
            if (verbose) P(ss.str());
        }

        {
            bdlt::Datetime dateTime(2005,1,31,8,59,59,123);
            bsl::stringstream ss;
            bdlpuxxx::Iso8601::generate(ss, dateTime);
            LOOP_ASSERT(ss.str(), ss.str() == "2005-01-31T08:59:59.123");
            if (verbose) P(ss.str());
        }

        {
            bdlt::DateTz date(bdlt::Date(2005,1,31), -240);
            bsl::stringstream ss;
            bdlpuxxx::Iso8601::generate(ss, date);
            LOOP_ASSERT(ss.str(), ss.str() == "2005-01-31-04:00");
            if (verbose) P(ss.str());
        }

        {
            bdlt::Date date(2005,1,31);
            bsl::stringstream ss;
            bdlpuxxx::Iso8601::generate(ss, date);
            LOOP_ASSERT(ss.str(), ss.str() == "2005-01-31");
            if (verbose) P(ss.str());
        }

        {
            bdlt::TimeTz time(bdlt::Time(8,59,59,120), -240);
            bsl::stringstream ss;
            bdlpuxxx::Iso8601::generate(ss, time);
            LOOP_ASSERT(ss.str(), ss.str() == "08:59:59.120-04:00");
            if (verbose) P(ss.str());
        }

        {
            bdlt::Time time(8,59,59,120);
            bsl::stringstream ss;
            bdlpuxxx::Iso8601::generate(ss, time);
            LOOP_ASSERT(ss.str(), ss.str() == "08:59:59.120");
            if (verbose) P(ss.str());
        }

      } break;
      default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
