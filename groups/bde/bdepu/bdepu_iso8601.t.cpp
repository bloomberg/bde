// bdepu_iso8601.t.cpp                  -*-C++-*-

#include <bdepu_iso8601.h>

#include <bdet_date.h>
#include <bdet_datetime.h>
#include <bdet_datetimetz.h>
#include <bdet_datetz.h>
#include <bdet_time.h>
#include <bdeu_chartype.h>

#include <bsls_stopwatch.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_cmath.h>

#include <stdlib.h>

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This component can be thoroughly tested by generating a representative set
// of input values for each function and comparing the result with an expected
// result.
//-----------------------------------------------------------------------------
// [2]   static int generate(char             *buffer,
//                           const bdet_Date&  object,
//                           int               bufferLength);
// [2]   static int generate(char                 *buffer,
//                           const bdet_Datetime&  object,
//                           int                   bufferLength);
// [2]   static int generate(char                   *buffer,
//                           const bdet_DatetimeTz&  object,
//                           int                     bufferLength);
// [2]   static int generate(char               *buffer,
//                           const bdet_DateTz&  object,
//                           int                 bufferLength);
// [2]   static int generate(char             *buffer,
//                           const bdet_Time&  object,
//                           int               bufferLength);
// [2]   static int generate(char               *buffer,
//                           const bdet_TimeTz&  object,
//                           int                 bufferLength);
//
// [2] template <typename DATE_TYPE>
//     static bsl::ostream& generate(bsl::ostream&    stream,
//                                   const DATE_TYPE& object);
//
// [3] static int parse(bdet_Date  *result,
//                      const char *input,
//                      int         inputLength);
// [3] static int parse(bdet_Datetime *result,
//                      const char    *input,
//                      int            inputLength);
// [3] static int parse(bdet_DatetimeTz *result,
//                      const char      *input,
//                      int              inputLength);
// [3] static int parse(bdet_DateTz *result,
//                      const char  *input,
//                      int          inputLength);
// [3] static int parse(bdet_Time  *result,
//                      const char *input,
//                      int         inputLength);
// [3] static int parse(bdet_TimeTz *result,
//                      const char  *input,
//                      int          inputLength);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [4] USAGE EXAMPLE
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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) {                 \
   if (!(X)) { bsl::cout                   \
               << #I << ": " << I << "\n"; \
               aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) {              \
   if (!(X)) { bsl::cout                   \
               << #I << ": " << I << "\t"  \
               << #J << ": " << J << "\n"; \
               aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {            \
   if (!(X)) { bsl::cout                   \
               << #I << ": " << I << "\t"  \
               << #J << ": " << J << "\t"  \
               << #K << ": " << K << "\n"; \
               aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {          \
   if (!(X)) { bsl::cout                   \
               << #I << ": " << I << "\t"  \
               << #J << ": " << J << "\t"  \
               << #K << ": " << K << "\t"  \
               << #L << ": " << L << "\n"; \
               aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {        \
   if (!(X)) { bsl::cout                   \
               << #I << ": " << I << "\t"  \
               << #J << ": " << J << "\t"  \
               << #K << ": " << K << "\t"  \
               << #L << ": " << L << "\t"  \
               << #M << ": " << M << "\n"; \
               aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) {      \
   if (!(X)) { bsl::cout                   \
               << #I << ": " << I << "\t"  \
               << #J << ": " << J << "\t"  \
               << #K << ": " << K << "\t"  \
               << #L << ": " << L << "\t"  \
               << #M << ": " << M << "\t"  \
               << #N << ": " << N << "\n"; \
               aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
    // Print identifier and value.
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
    // Quote identifier literally.
#define P_(X) bsl::cout << #X " = " << (X) << ", "<< bsl::flush;
    // P(X) without '\n'
#define T_    bsl::cout << "\t" << bsl::flush;  // print Tab without '\n'
#define L_ __LINE__                             // current Line number

//=============================================================================
//                  GLOBALS, TYPEDEFS, CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose;
static int veryVerbose;
static int veryVeryVerbose;

typedef bdepu_Iso8601 Util;

static char *cloneStr(const char *str, int len)
{
    char *ret = (char *) malloc(len);
#ifdef BSLS_PLATFORM__OS_AIX
    // 'malloc(0) return 0 on AIX, which complicates life later

    if (0 == len) {
        ASSERT(0 == ret);
        ret = (char *) malloc(4) + 4;
    }
#endif

    bsl::memcpy(ret, str, len);
    return ret;
}

static void freeStr(char *str, int len)
{
#ifdef BSLS_PLATFORM__OS_AIX
    if (0 == len) {
        str -= 4;
    }
#else
    ++len;    // quiet 'unused' warnings
#endif

    free(str);
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
    static const bsl::string dateStr = "2000-01-02";
    static const bsl::string timeStr = "12:34:56";
    static const bsl::string datetimeStr = "2001-02-03T14:21:34";

    static const bdet_Date       initDate( 3,  3,  3);
    static const bdet_DateTz     initDateTz(initDate,-120);
    static const bdet_Time       initTime(11, 11, 11);
    static const bdet_TimeTz     initTimeTz(initTime, 120);
    static const bdet_Datetime   initDatetime(  initDate, initTime);
    static const bdet_DatetimeTz initDatetimeTz(initDatetime, 180);

    int ret;
    bdet_Date date(initDate);
    const bsl::string& dateTzStr = dateStr + tzStr;
    ret = !Util::parse(&date, dateTzStr.c_str(),
                              dateTzStr.length());
    LOOP2_ASSERT(dateTzStr, ret, valid == ret);
    LOOP_ASSERT(dateTzStr, valid || initDate == date);

    bdet_DateTz dateTz(initDateTz);
    ret = !Util::parse(&dateTz, dateTzStr.c_str(),
                                dateTzStr.length());
    LOOP2_ASSERT(dateTzStr, ret, valid == ret);
    LOOP_ASSERT(dateTzStr, valid || initDateTz == dateTz);
    LOOP3_ASSERT(dateTzStr, offset, dateTz.offset(),
                      !valid || offset == dateTz.offset());

    bdet_Time time(initTime);
    const bsl::string& timeTzStr = timeStr + tzStr;
    ret = !Util::parse(&time, timeTzStr.c_str(),
                              timeTzStr.length());
    LOOP2_ASSERT(timeTzStr, ret, valid == ret);
    LOOP_ASSERT(timeTzStr, valid || initTime == time);

    bdet_TimeTz timeTz(initTimeTz);
    ret = !Util::parse(&timeTz, timeTzStr.c_str(),
                                timeTzStr.length());
    LOOP2_ASSERT(timeTzStr, ret, valid == ret);
    LOOP_ASSERT(timeTzStr, valid || initTimeTz == timeTz);
    LOOP3_ASSERT(timeTzStr, offset, timeTz.offset(),
                      !valid || offset == timeTz.offset());

    bdet_Datetime datetime(initDatetime);
    const bsl::string& datetimeTzStr = datetimeStr + tzStr;
    ret = !Util::parse(&datetime, datetimeTzStr.c_str(),
                                  datetimeTzStr.length());
    LOOP2_ASSERT(dateTzStr, ret, valid == ret);
    LOOP_ASSERT(dateTzStr,
                        valid || initDatetime == datetime);

    bdet_DatetimeTz datetimeTz(initDatetimeTz);
    ret = !Util::parse(&datetimeTz,datetimeTzStr.c_str(),
                                   datetimeTzStr.length());
    LOOP2_ASSERT(dateTzStr, ret, valid == ret);
    LOOP_ASSERT(dateTzStr,
                    valid || initDatetimeTz == datetimeTz);
    LOOP3_ASSERT(datetimeTzStr, offset,datetimeTz.offset(),
                  !valid || offset == datetimeTz.offset());
}

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
        { L_, 2005,  1,  1, 24,   0,   0,     0, "",           1,  1 },
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
    static const int NUM_UTC_OFFSETS = sizeof UTC_OFFSETS / sizeof *UTC_OFFSETS;

    static const char *JUNK[] = { "xyz", "?1234", "*zbc", "*", "01", "+",
                                                                     "-" };
    static const int NUM_JUNK = sizeof JUNK / sizeof *JUNK;

    const bdet_Date       initDate( 3,  3,  3);
    const bdet_Time       initTime(11, 11, 11);
    const bdet_DateTz     initDateTz(initDate,-120);
    const bdet_TimeTz     initTimeTz(initTime, 120);
    const bdet_Datetime   initDatetime(  initDate, initTime);
    const bdet_DatetimeTz initDatetimeTz(initDatetime, 180);

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
                                  && bdeu_CharType::isDigit(FRAC_SECOND[4])
                                  && FRAC_SECOND[4] - '0' >= 5;
        LOOP_ASSERT(LINE, carrySecond == (MILLISECOND == 1000));

        bdet_Date theDate;
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

                bdet_DateTz     theDateTz;
                if (isValidDate) {
                    theDateTz.setDateTz(theDate, UTC_OFFSET);
                }

                bdet_Time theTime;
                bool isValidTime =
                              0 == theTime.setTimeIfValid(HOUR, MINUTE, SECOND);
                if (isValidTime && MILLISECOND) {
                    if (HOUR >= 24) {
                        isValidTime = false;
                        theTime = bdet_Time();
                    }
                    else {
                        theTime.addMilliseconds(MILLISECOND);
                    }
                }
                LOOP_ASSERT(LINE, TIME_VALID == isValidTime);
                isValidTime &= !(24 == HOUR && UTC_OFFSET);

                bdet_TimeTz     theTimeTz;
                bool isValidTimeTz = isValidTime
                   && 0 == theTimeTz.validateAndSetTimeTz(theTime, UTC_OFFSET);

                bool isValidDatetime = (isValidDate && isValidTime);
                bdet_Datetime theDatetime;
                if (isValidDatetime) {
                    theDatetime.setDatetime(YEAR, MONTH, DAY,
                                            HOUR, MINUTE, SECOND);
                    if (MILLISECOND) {
                        theDatetime.addMilliseconds(MILLISECOND);
                    }
                }

                bdet_DatetimeTz theDatetimeTz;
                bool isValidDatetimeTz = isValidDatetime
                                 && 0 == theDatetimeTz.validateAndSetDatetimeTz(
                                                       theDatetime, UTC_OFFSET);

                char dateStr[25], timeStr[25], offsetStr[10];
                bsl::sprintf(dateStr, "%04d-%02d-%02d", YEAR, MONTH, DAY);
                bsl::sprintf(timeStr, "%02d:%02d:%02d%s",
                             HOUR, MINUTE, SECOND, FRAC_SECOND);
                if (UTC_OFFSETS[j] >= UTC_EMPTY_OFFSET) {
                    // Create empty string, "Z", or "z"
                    offsetStr[0] = UTC_OFFSETS[j] - UTC_EMPTY_OFFSET;
                    offsetStr[1] = '\0';
                }
                else {
                    bsl::sprintf(offsetStr, "%+03d:%02d",
                                    UTC_OFFSET / 60, bsl::abs(UTC_OFFSET) % 60);
                }

                const bool trailFrac = bsl::strlen(FRAC_SECOND) > 1
                                           && 0 == bsl::strlen(offsetStr)
                                           && bdeu_CharType::isDigit(*JUNK_STR);

                char input[200];
                int ret;

                {
                    bdet_DatetimeTz datetime = initDatetimeTz;
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
                            const bdet_Datetime& dt = datetime.localDatetime();

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
                    const bdet_Datetime EXP_DATETIME = isValidDatetime ?
                                  theDatetimeTz.gmtDatetime() : bdet_Datetime();

                    bdet_Datetime datetime = initDatetime;
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
                        ret = Util::parse(&datetime, input, bsl::strlen(input));
                        LOOP3_ASSERT(LINE, input, ret, 0 != ret);
                        LOOP_ASSERT(initDatetime,initDatetime == datetime);
                    }
                }

                {
                    bdet_DateTz date = initDateTz;
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
                    bdet_Date date = initDate;
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
                    bdet_TimeTz time = initTimeTz;
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
                    const bdet_Time EXP_TIME = isValidTime ?
                                             theTimeTz.gmtTime() : bdet_Time();

                    bdet_Time time = initTime;
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
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    (void) veryVeryVerbose;    // eliminate unused variable warning

    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 6: {
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
// string from a 'bdet_DatetimeTz' value:
//..
    const bdet_DatetimeTz theDatetime(bdet_Datetime(2005, 1, 31,
                                                    8, 59, 59, 123), 240);
    bsl::stringstream ss;
    bdepu_Iso8601::generate(ss, theDatetime);
    ASSERT(ss.str() == "2005-01-31T08:59:59.123+04:00");
//..
// The following example illustrates how to parse an ISO 8601-compliant string
// into a 'bdet_DatetimeTz' object:
//..
    bdet_DatetimeTz dateTime;
    const char dtStr[] = "2005-01-31T08:59:59.1226-04:00";
    int ret = bdepu_Iso8601::parse(&dateTime, dtStr, bsl::strlen(dtStr));
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
      case 5: {
        // --------------------------------------------------------------------
        // TESTING TIME ZONES
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
        // PURIFY TESTING PARSE
        //
        // Concerns:
        //   That parse must never access past the end of segment.
        //
        // Plan:
        //   Repeat test 3, only with segments malloc'ed to be all lengths
        //   in the range [ 0, inputLen ].  This test must be run under purify
        //   for the full effect.
        //
        // Testing (again):
        //     static int parse(bdet_Date  *result,
        //                      const char *input,
        //                      int         inputLength);
        //     static int parse(bdet_Datetime *result,
        //                      const char    *input,
        //                      int            inputLength);
        //     static int parse(bdet_DatetimeTz *result,
        //                      const char      *input,
        //                      int              inputLength);
        //     static int parse(bdet_DateTz *result,
        //                      const char  *input,
        //                      int          inputLength);
        //     static int parse(bdet_Time  *result,
        //                      const char *input,
        //                      int         inputLength);
        //     static int parse(bdet_TimeTz *result,
        //                      const char  *input,
        //                      int          inputLength);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING PARSE"
                               << "\n=============" << bsl::endl;

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
            bool        d_partialTest;
        } DATA[] = {
            //                                                  Dat Tim Tst
            //Ln  Year  Mo  Day Hr  Min  Sec    ms Frac         Vld Vld Prt
            //==  ====  ==  === ==  ===  ===    == ====         === === ===

            // Invalid Dates
            { L_, 0000,  0,  0,  0,   0,   0,    0, "",           0,  1, 1 },
            { L_, 2005,  0,  1,  0,   0,   0,    0, "",           0,  1, 1 },
            { L_, 2005, 13,  1,  0,   0,   0,    0, "",           0,  1, 1 },
            { L_, 2005, 99,  1,  0,   0,   0,    0, "",           0,  1, 1 },
            { L_, 2005,  1,  0,  0,   0,   0,    0, "",           0,  1, 1 },
            { L_, 2005,  1, 32,  0,   0,   0,    0, "",           0,  1, 1 },
            { L_, 2005,  1, 99,  0,   0,   0,    0, "",           0,  1, 1 },

            // Invalid Times
            { L_, 2005, 12, 31, 25,   0,   0,    0, "",           1,  0, 1 },
            { L_, 2005, 12, 31, 99,   0,   0,    0, "",           1,  0, 1 },
            { L_, 2005, 12, 31, 12,  60,   0,    0, "",           1,  0, 1 },
            { L_, 2005, 12, 31, 12, 100,   0,    0, "",           1,  0, 1 },
            { L_, 2005, 12, 31, 12,  59,  62,    0, "",           1,  0, 1 },
            { L_, 2005, 12, 31, 12,  59, 101,    0, "",           1,  0, 0 },

            { L_, 2005,  1,  1, 24,   1,   0,    0, "",           1,  0, 1 },
            { L_, 2005,  1,  1, 24,   0,   1,    0, "",           1,  0, 1 },
            { L_, 2005,  1,  1, 24,   0,   0, 1000, ".9991",      1,  0, 0 },

            // Valid dates and times
            { L_, 2005,  1,  1, 24,   0,   0,    0, "",           1,  1, 0 },
            { L_, 2005,  1,  1,  0,   0,   0,    0, "",           1,  1, 1 },
            { L_, 0123,  6, 15, 13,  40,  59,    0, "",           1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    0, "",           1,  1, 1 },

            // Vary fractions of a second.
            { L_, 1999, 10, 12, 23,   0,   1,    0, ".",          1,  0, 0 },
            { L_, 1999, 10, 12, 23,   0,   1,    0, ".0",         1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    0, ".00",        1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    0, ".000",       1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    0, ".0000",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    0, ".00000",     1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    0, ".000000",    1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    0, ".0000000",   1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    0, ".0004",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    0, ".00045",     1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    0, ".000456",    1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    0, ".0004567",   1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    1, ".0005",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    1, ".0006",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    1, ".0009",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    2, ".002",       1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    2, ".0020",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  200, ".2",         1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  200, ".20",        1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  200, ".200",       1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  200, ".2000",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  200, ".20000",     1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  200, ".200000",    1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,    3, ".0025",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,   34, ".034",       1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,   34, ".0340",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,   35, ".0345",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  456, ".456",       1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  456, ".4560",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  457, ".4567",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  999, ".9994",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  999, ".99945",     1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  999, ".999456",    1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1,  999, ".9994567",   1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1, 1000, ".9995",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1, 1000, ".99956",     1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1, 1000, ".999567",    1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1, 1000, ".9999",      1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1, 1000, ".99991",     1,  1, 1 },
            { L_, 1999, 10, 12, 23,   0,   1, 1000, ".999923",    1,  1, 1 },
            { L_, 1999, 12, 31, 23,  59,  59, 1000, ".9995",      1,  1, 1 },
        };

        static const int NUM_DATA = sizeof DATA / sizeof *DATA;

        static const int UTC_EMPTY_OFFSET = 0x70000000;
        static const int UTC_UCZ_OFFSET   = UTC_EMPTY_OFFSET + 'Z';
        static const int UTC_LCZ_OFFSET   = UTC_EMPTY_OFFSET + 'z';

        static const int UTC_OFFSETS[] = {
            0, -90, -240, -720, 90, 240, 720,
            UTC_EMPTY_OFFSET, UTC_UCZ_OFFSET, UTC_LCZ_OFFSET
        };
        static const int NUM_UTC_OFFSETS =
            sizeof UTC_OFFSETS / sizeof *UTC_OFFSETS;

        const bdet_Date       initDate( 3,  3,  3);
        const bdet_Time       initTime(11, 11, 11);
        const bdet_DateTz     initDateTz(initDate,-120);
        const bdet_TimeTz     initTimeTz(initTime, 120);
        const bdet_Datetime   initDatetime(  initDate, initTime);
        const bdet_DatetimeTz initDatetimeTz(initDatetime, 180);

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
            const bool        PARTIAL_TEST= DATA[i].d_partialTest;

            const int         FRAC_LEN    = bsl::strlen(FRAC_SECOND);

            if (verbose) {
                P_(LINE); P_(YEAR); P_(MONTH); P(DAY);
                P_(HOUR); P_(MINUTE); P_(SECOND); P_(MILLISECOND);
                P_(FRAC_SECOND); P_(DATE_VALID); P(TIME_VALID);
            }

            bdet_Date theDate;
            const bool isValidDate =
                0 == theDate.setYearMonthDayIfValid(YEAR, MONTH, DAY) &&
                DATE_VALID;
            LOOP_ASSERT(LINE, DATE_VALID == isValidDate);

            for (int j = 0; j < NUM_UTC_OFFSETS; ++j) {
                if (verbose) { T_; P_(NUM_UTC_OFFSETS); P(j); }

                const int UTC_OFFSET = (UTC_OFFSETS[j] >= UTC_EMPTY_OFFSET ?
                                        0 : UTC_OFFSETS[j]);

                bdet_DateTz     theDateTz;
                if (isValidDate) {
                    theDateTz.setDateTz(theDate, UTC_OFFSET);
                }

                bdet_Time theTime;
                bool isValidTime =
                         0 == theTime.setTimeIfValid(HOUR, MINUTE, SECOND) &&
                         TIME_VALID;
                if (isValidTime && MILLISECOND) {
                    if (HOUR >= 24) {
                        isValidTime = false;
                        theTime = bdet_Time();
                    }
                    else {
                        theTime.addMilliseconds(MILLISECOND);
                        LOOP_ASSERT(LINE, TIME_VALID == isValidTime);
                    }
                }

                bdet_TimeTz     theTimeTz;
                bool isValidTimeTz = isValidTime
                   && 0 == theTimeTz.validateAndSetTimeTz(theTime, UTC_OFFSET);

                bool isValidDatetime = (isValidDate && isValidTime);
                bdet_Datetime theDatetime;
                if (isValidDatetime) {
                    theDatetime.setDatetime(YEAR, MONTH, DAY,
                                            HOUR, MINUTE, SECOND);
                    if (MILLISECOND) {
                        theDatetime.addMilliseconds(MILLISECOND);
                    }
                }

                bdet_DatetimeTz theDatetimeTz;
                bool isValidDatetimeTz = isValidDatetime
                                && 0 == theDatetimeTz.validateAndSetDatetimeTz(
                                                      theDatetime, UTC_OFFSET);

                char dateStr[25], timeStr[25], offsetStr[10];
                bsl::sprintf(dateStr, "%04d-%02d-%02d", YEAR, MONTH, DAY);
                bsl::sprintf(timeStr, "%02d:%02d:%02d%s",
                             HOUR, MINUTE, SECOND, FRAC_SECOND);
                if (UTC_OFFSETS[j] >= UTC_EMPTY_OFFSET) {
                    // Create empty string, "Z", or "z"
                    offsetStr[0] = UTC_OFFSETS[j] - UTC_EMPTY_OFFSET;
                    offsetStr[1] = '\0';
                }
                else {
                    bsl::sprintf(offsetStr, "%+03d:%02d",
                                 UTC_OFFSET / 60, bsl::abs(UTC_OFFSET) % 60);
                }
                const int OFFSET_LEN = bsl::strlen(offsetStr);

                char input[200];
                int ret;

                {
                    bsl::memset(input, 0, sizeof(input));
                    bsl::strcpy(input, dateStr);
                    bsl::strcat(input, "T");
                    bsl::strcat(input, timeStr);
                    bsl::strcat(input, offsetStr);
                    int inputLen = bsl::strlen(input);
                    bdet_DatetimeTz datetime;
                    for (int k = 0; k <= inputLen; ++k) {
                        char *segment = cloneStr(input, k);
                        datetime = initDatetimeTz;
                        ret = !Util::parse(&datetime, segment, k);
                        bool pass = 19 == k;
                        pass |= k >= 21 && k <= 19 + FRAC_LEN;
                        pass |= k == 19 + FRAC_LEN + OFFSET_LEN;
                        if (PARTIAL_TEST || k == inputLen) {
                            LOOP5_ASSERT(LINE, ret, k, isValidDatetimeTz,input,
                                           ret == (isValidDatetimeTz && pass));
                        }
                        freeStr(segment, k);
                    }
                    LOOP5_ASSERT(LINE, input, ret, isValidDatetimeTz,
                                         UTC_OFFSET, isValidDatetimeTz == ret);
                    if (isValidDatetimeTz) {
                        LOOP3_ASSERT(LINE, input, datetime,
                                                    datetime == theDatetimeTz);
                        if (bsl::strncmp(FRAC_SECOND, ".999", 4)) {
                            LOOP3_ASSERT(LINE, datetime, SECOND,
                                  datetime.localDatetime().second() == SECOND);
                            LOOP3_ASSERT(LINE, datetime, MINUTE,
                                  datetime.localDatetime().minute() == MINUTE);
                            LOOP3_ASSERT(LINE, datetime, HOUR,
                                  datetime.localDatetime().hour() == HOUR);
                            LOOP3_ASSERT(LINE, datetime, DAY,
                                  datetime.localDatetime().day() == DAY);
                            LOOP3_ASSERT(LINE, datetime, MONTH,
                                  datetime.localDatetime().month() == MONTH);
                            LOOP4_ASSERT(LINE, input, datetime, YEAR,
                                  datetime.localDatetime().year() == YEAR);
                        }
                    }
                    else {
                        LOOP_ASSERT(LINE, initDatetimeTz == datetime);
                    }
                    if (veryVerbose) { T_; P(datetime); }
                }

                {
                    const bdet_Datetime EXP_DATETIME = isValidDatetime ?
                                 theDatetimeTz.utcDatetime() : bdet_Datetime();

                    bsl::memset(input, 0, sizeof(input));
                    bsl::strcpy(input, dateStr);
                    bsl::strcat(input, "T");
                    bsl::strcat(input, timeStr);
                    bsl::strcat(input, offsetStr);
                    int inputLen = bsl::strlen(input);
                    bdet_Datetime datetime;
                    for (int k = 0; k <= inputLen; ++k) {
                        char *segment = cloneStr(input, k);
                        datetime = initDatetime;
                        ret = !Util::parse(&datetime, input, k);
                        bool pass = 19 == k;
                        pass |= k >= 21 && k <= 19 + FRAC_LEN;
                        pass |= k == 19 + FRAC_LEN + OFFSET_LEN;
                        if (PARTIAL_TEST || k == inputLen) {
                            LOOP5_ASSERT(LINE, ret, k, isValidDatetimeTz,input,
                                           ret == (isValidDatetimeTz && pass));
                        }
                        freeStr(segment, k);
                    }
                    if (isValidDatetimeTz) {
                        LOOP4_ASSERT(LINE, input, datetime, EXP_DATETIME,
                                                     EXP_DATETIME == datetime);
                        if (bsl::strncmp(FRAC_SECOND, ".999", 4) && 0 == j) {
                            LOOP3_ASSERT(LINE, datetime, SECOND,
                                                  datetime.second() == SECOND);
                            LOOP3_ASSERT(LINE, datetime, MINUTE,
                                                  datetime.minute() == MINUTE);
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
                    else {
                        LOOP_ASSERT(LINE, initDatetime == datetime);
                    }
                    if (veryVerbose) { T_; P(datetime); }
                }

                {
                    bsl::memset(input, 0, sizeof(input));
                    bsl::strcpy(input, dateStr);
                    bsl::strcat(input, offsetStr);
                    int inputLen = bsl::strlen(input);
                    bdet_DateTz date;
                    for (int k = 0; k <= inputLen; ++k) {
                        char *segment = cloneStr(input, k);
                        date = initDateTz;
                        ret = !Util::parse(&date, input, k);
                        bool pass = 10 == k;
                        pass |= k == 10 + OFFSET_LEN;
                        if (PARTIAL_TEST || k == inputLen) {
                            LOOP5_ASSERT(LINE, ret, k, DATE_VALID, input,
                                                  ret == (DATE_VALID && pass));
                        }
                        freeStr(segment, k);
                    }
                    LOOP3_ASSERT(LINE, input, ret, isValidDate == ret);
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
                }

                {
                    bsl::memset(input, 0, sizeof(input));
                    bsl::strcpy(input, dateStr);
                    bsl::strcat(input, offsetStr);
                    int inputLen = bsl::strlen(input);
                    bdet_Date date;
                    for (int k = 0; k <= inputLen; ++k) {
                        char *segment = cloneStr(input, k);
                        date = initDate;
                        ret = !Util::parse(&date, input, k);
                        bool pass = 10 == k;
                        pass |= k == 10 + OFFSET_LEN;
                        if (PARTIAL_TEST || k == inputLen) {
                            LOOP5_ASSERT(LINE, ret, k, DATE_VALID, input,
                                                  ret == (DATE_VALID && pass));
                        }
                        freeStr(segment, k);
                    }
                    LOOP3_ASSERT(LINE, input, ret, isValidDate == ret);
                    if (isValidDate) {
                        LOOP3_ASSERT(LINE, input, date, date == theDate);
                        LOOP3_ASSERT(LINE, date, DAY,   date.day()   == DAY);
                        LOOP3_ASSERT(LINE, date, MONTH, date.month() == MONTH);
                        LOOP3_ASSERT(LINE, date, YEAR,  date.year()  == YEAR);

                    }
                    else {
                        LOOP_ASSERT(LINE, initDate == date);
                    }
                    if (veryVerbose) { T_; P(date); }
                }

                {
                    bsl::memset(input, 0, sizeof(input));
                    bsl::strcpy(input, timeStr);
                    bsl::strcat(input, offsetStr);
                    int inputLen = bsl::strlen(input);
                    bdet_TimeTz time;
                    for (int k = 0; k <= inputLen; ++k) {
                        char *segment = cloneStr(input, k);
                        time = initTimeTz;
                        ret = !Util::parse(&time, input, k);
                        bool pass = 8 == k;
                        if (FRAC_LEN > 0) {
                            pass |= k >= 10 && k <= 8 + FRAC_LEN;
                        }
                        if (OFFSET_LEN > 0) {
                            pass |= k == 8 + FRAC_LEN + OFFSET_LEN;
                        }
                        if (PARTIAL_TEST || k == inputLen) {
                            LOOP5_ASSERT(LINE, ret, k, isValidTimeTz, input,
                                               ret == (isValidTimeTz && pass));
                        }
                        freeStr(segment, k);
                    }
                    LOOP5_ASSERT(LINE, input, ret, isValidTimeTz, UTC_OFFSET,
                                                         isValidTimeTz == ret);
                    if (isValidTimeTz) {
                        LOOP3_ASSERT(LINE, input, time, time == theTimeTz);
                        if (bsl::strncmp(FRAC_SECOND, ".999", 4)) {
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
                }

                {
                    const bdet_Time EXP_TIME = isValidTime ?
                                             theTimeTz.utcTime() : bdet_Time();

                    bsl::memset(input, 0, sizeof(input));
                    bsl::strcpy(input, timeStr);
                    bsl::strcat(input, offsetStr);
                    int inputLen = bsl::strlen(input);
                    bdet_Time time;
                    for (int k = 0; k <= inputLen; ++k) {
                        char *segment = cloneStr(input, k);
                        time = initTime;
                        ret = !Util::parse(&time, input, k);
                        bool pass = 8 == k;
                        pass |= k >= 10 && k <= 8 + FRAC_LEN;
                        pass |= k == 8 + FRAC_LEN + OFFSET_LEN;
                        if (PARTIAL_TEST || k == inputLen) {
                            if (8 + FRAC_LEN < k) {
                                LOOP5_ASSERT(LINE, ret, k, isValidTimeTz,
                                             input,
                                               ret == (isValidTimeTz && pass));
                            }
                            else {
                                LOOP5_ASSERT(LINE, ret, k, TIME_VALID, input,
                                                  ret == (TIME_VALID && pass));
                            }
                        }
                        freeStr(segment, k);
                    }
                    if (ret) {
                        LOOP4_ASSERT(LINE, input, time, EXP_TIME,
                                                             EXP_TIME == time);
                        if (bsl::strncmp(FRAC_SECOND, ".999", 4) && 0 == j) {
                            LOOP3_ASSERT(LINE, time, SECOND,
                                                      time.second() == SECOND);
                            LOOP3_ASSERT(LINE, time, MINUTE,
                                                      time.minute() == MINUTE);
                            LOOP3_ASSERT(LINE, time, HOUR,
                                                      time.hour() == HOUR);
                        }
                    }
                    else {
                        LOOP_ASSERT(LINE, initTime == time);
                    }
                    if (veryVerbose) { T_; P(time); }
                }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PARSE
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
        //     static int parse(bdet_Date  *result,
        //                      const char *input,
        //                      int         inputLength);
        //     static int parse(bdet_Datetime *result,
        //                      const char    *input,
        //                      int            inputLength);
        //     static int parse(bdet_DatetimeTz *result,
        //                      const char      *input,
        //                      int              inputLength);
        //     static int parse(bdet_DateTz *result,
        //                      const char  *input,
        //                      int          inputLength);
        //     static int parse(bdet_Time  *result,
        //                      const char *input,
        //                      int         inputLength);
        //     static int parse(bdet_TimeTz *result,
        //                      const char  *input,
        //                      int          inputLength);
        // --------------------------------------------------------------------

        if (verbose) bsl::cout << "\nTESTING PARSE"
                               << "\n=============" << bsl::endl;

        testCase3TestingParse();
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING GENERATE
        //
        // Concerns:
        //     - All six types generate correct strings for a variety of
        //       values.
        //     - The Tz types add an offset from UTC to the end of the
        //       generated string; the other types do not.
        //     - The Tz value can be positive, negative, or zero.
        //     - All time and datetime classes generate a 3-digits for
        //       fractions of a second.
        //     - Correct behavior in case of short output buffers
        //
        //   - Prepare a set of test vectors with the following values:
        //     + Valid dates and times
        //     + Fractional seconds from 0 to 999 requiring 0, 1, 2, or 3
        //       digits to represent, with and without trailing zeros.
        //   - Apply the following orthogonal perturbation:
        //     + A set of timezone offsets, including zero, positive, and
        //       negative offsets.
        //   - Compose expected date, time, and datetime strings from each
        //     test vector.
        //   - Construct each of the six date, time, and datetime types from
        //     the test vector and stream them to an output string.
        //   - Compare the generated strings against expected strings.
        //
        // Testing:
        //      static int generate(char             *buffer,
        //                          const bdet_Date&  object,
        //                          int               bufferLength);
        //      static int generate(char                 *buffer,
        //                          const bdet_Datetime&  object,
        //                          int                   bufferLength);
        //      static int generate(char                   *buffer,
        //                          const bdet_DatetimeTz&  object,
        //                          int                     bufferLength);
        //      static int generate(char               *buffer,
        //                          const bdet_DateTz&  object,
        //                          int                 bufferLength);
        //      static int generate(char             *buffer,
        //                          const bdet_Time&  object,
        //                          int               bufferLength);
        //      static int generate(char               *buffer,
        //                          const bdet_TimeTz&  object,
        //                          int                 bufferLength);
        // --------------------------------------------------------------------

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

        for (int i = 0; i < NUM_DATA; ++i) {
            const int         LINE        = DATA[i].d_line;
            const int         YEAR        = DATA[i].d_year;
            const int         MONTH       = DATA[i].d_month;
            const int         DAY         = DATA[i].d_day;
            const int         HOUR        = DATA[i].d_hour;
            const int         MINUTE      = DATA[i].d_minute;
            const int         SECOND      = DATA[i].d_second;
            const int         MILLISECOND = DATA[i].d_millisecond;

            bdet_Date theDate(YEAR, MONTH, DAY);
            bdet_Time theTime(HOUR, MINUTE, SECOND, MILLISECOND);
            bdet_Datetime theDatetime(YEAR, MONTH, DAY,
                                      HOUR, MINUTE, SECOND, MILLISECOND);

            bsl::stringstream output;
            char outbuf[Util::BDEPU_MAX_DATETIME_STRLEN + 3];
            int  rc;

            for (int j = 0; j < NUM_UTC_OFFSETS; ++j) {

                const int UTC_OFFSET = UTC_OFFSETS[j];

                bdet_DateTz     theDateTz(theDate, UTC_OFFSET);
                bdet_TimeTz     theTimeTz(theTime, UTC_OFFSET);
                bdet_DatetimeTz theDatetimeTz(theDatetime, UTC_OFFSET);

                char dateStr[25], timeStr[25], offsetStr[10];
                bsl::sprintf(dateStr, "%04d-%02d-%02d", YEAR, MONTH, DAY);
                bsl::sprintf(timeStr, "%02d:%02d:%02d.%03d",
                             HOUR, MINUTE, SECOND, MILLISECOND);
                bsl::sprintf(offsetStr, "%+03d:%02d",
                             UTC_OFFSET / 60, bsl::abs(UTC_OFFSET) % 60);

                char expected[100];

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
            bsls_Stopwatch  watch;
            bdet_DatetimeTz datetime;
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
            bdet_DatetimeTz dateTime;
            const char str[] = "2005-01-31T08:59:59.12345678901234-04:00";
            int ret = Util::parse(&dateTime, str, sizeof(str)-1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(dateTime,
                        dateTime == bdet_DatetimeTz(bdet_Datetime(2005,1,31,
                                                                  8,59,59,123),
                                                    -240));
            if (verbose) P(dateTime);
        }

        {
            bdet_Datetime dateTime;
            const char str[] = "2005-01-31T08:59:59.1234567890123-04:00";
            int ret = Util::parse(&dateTime, str, sizeof(str)-1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(dateTime,
                        dateTime == bdet_Datetime(2005,1,31,12,59,59,123));
            if (verbose) P(dateTime);
        }

        {
            bdet_DateTz date;
            const char str[] = "2005-01-31-04:00";
            int ret = Util::parse(&date, str, sizeof(str)-1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(date,
                        date == bdet_DateTz(bdet_Date(2005,1,31), -240));
            if (verbose) P(date);
        }

        {
            bdet_Date date;
            const char str[] = "2005-01-31-04:00";
            int ret = Util::parse(&date, str, sizeof(str)-1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(date, date == bdet_Date(2005,1,31))
            if (verbose) P(date);
        }

        {
            bdet_TimeTz time;
            const char str[] = "08:59:59.1225678901234-04:00";
            int ret = Util::parse(&time, str, sizeof(str)-1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(time,
                        time == bdet_TimeTz(bdet_Time(8,59,59,123), -240));
            if (verbose) P(time);
        }

        {
            bdet_Time time;
            const char str[] = "08:59:59.1225678901234-04:00";
            int ret = Util::parse(&time, str, sizeof(str)-1);
            LOOP_ASSERT(ret, 0 == ret);
            LOOP_ASSERT(time, time == bdet_Time(12,59,59,123));
            if (verbose) P(time);
        }

        {
            bdet_DatetimeTz dateTime(bdet_Datetime(2005,1,31,8,59,59,123),
                                     -240);
            bsl::stringstream ss;
            bdepu_Iso8601::generate(ss, dateTime);
            LOOP_ASSERT(ss.str(), ss.str() == "2005-01-31T08:59:59.123-04:00");
            if (verbose) P(ss.str());
        }

        {
            bdet_Datetime dateTime(2005,1,31,8,59,59,123);
            bsl::stringstream ss;
            bdepu_Iso8601::generate(ss, dateTime);
            LOOP_ASSERT(ss.str(), ss.str() == "2005-01-31T08:59:59.123");
            if (verbose) P(ss.str());
        }

        {
            bdet_DateTz date(bdet_Date(2005,1,31), -240);
            bsl::stringstream ss;
            bdepu_Iso8601::generate(ss, date);
            LOOP_ASSERT(ss.str(), ss.str() == "2005-01-31-04:00");
            if (verbose) P(ss.str());
        }

        {
            bdet_Date date(2005,1,31);
            bsl::stringstream ss;
            bdepu_Iso8601::generate(ss, date);
            LOOP_ASSERT(ss.str(), ss.str() == "2005-01-31");
            if (verbose) P(ss.str());
        }

        {
            bdet_TimeTz time(bdet_Time(8,59,59,120), -240);
            bsl::stringstream ss;
            bdepu_Iso8601::generate(ss, time);
            LOOP_ASSERT(ss.str(), ss.str() == "08:59:59.120-04:00");
            if (verbose) P(ss.str());
        }

        {
            bdet_Time time(8,59,59,120);
            bsl::stringstream ss;
            bdepu_Iso8601::generate(ss, time);
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
