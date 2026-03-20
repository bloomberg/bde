// bdlt_datetimetzformatter.t.cpp                                     -*-C++-*-
#include <bdlt_datetimetzformatter.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_formattestutil.h>
#include <bdlt_time.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>     // `atoi`
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;
using bsl::size_t;
using bsl::string;
using bsl::wstring;

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number
#define PH_(exp)     (cout << (#exp ": 0x") << bsl::hex << (exp) <<           \
                                                     bsl::dec << "\t" << flush)

// ============================================================================
//                             FORMAT TEST MACROS
// ----------------------------------------------------------------------------

#define U_TEST_FORMAT(exp, format, value) do {                                \
    bsl::string message;                                                      \
    bool rc = bdlt::FormatTestUtil::testFormat(&message, exp, format, value); \
    ASSERTV(message, rc);                                                     \
} while (false)

#define U_TEST_FORMAT_ARG(exp, format, value, arg) do {                       \
    bsl::string message;                                                      \
    bool rc = bdlt::FormatTestUtil::testFormatArg(                            \
                                         &message, exp, format, value, arg);  \
    ASSERTV(message, rc);                                                     \
} while (false)

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

typedef bdlt::FormatTestUtil TU;

namespace {
namespace u {

const size_t npos = bsl::string::npos;

void trimPrecision(bsl::string *s)
{
    ASSERTV(*s, 2 <= s->length());
    ASSERTV(*s, npos != s->find('.'));

    size_t sz = s->length();
    if ('.' == (*s)[sz - 2]) {
        s->resize(sz - 2);
    }
    else {
        s->resize(sz - 1);
    }
}

void appendIso8601Date(bsl::string       *result,
                       const bdlt::Date&  date)
{
    *result += bsl::format(
                   "{:04}-{:02}-{:02}", date.year(), date.month(), date.day());
}

void appendIso8601Time(bsl::string       *result,
                       const bdlt::Time&  time)
{
    *result += bsl::format("{:02}:{:02}:{:02}.{:03}",
                           time.hour(),
                           time.minute(),
                           time.second(),
                           time.millisecond());
}

void appendIso8601Offset(bsl::string *result,
                         int          offset)
{
    ASSERT(offset <=  1440);
    ASSERT(offset >= -1440);

    const char sign = offset < 0 ? '-' : '+';
    offset = bsl::abs(offset);

    *result += sign;
    *result += bsl::format("{:02}:{:02}", offset / 60, offset % 60);
}

void setIso8601DatetimeTz(bsl::string             *result,
                          const bdlt::DatetimeTz&  datetimeTz)
{
    result->clear();
    bdlt::Datetime datetime = datetimeTz.localDatetime();
    appendIso8601Date(  result, datetime.date());
    *result += 'T';
    appendIso8601Time(  result, datetime.time());
    appendIso8601Offset(result, datetimeTz.offset());
}

static const struct DData {
    int d_line;    // line number
    int d_year;    // year of date
    int d_month;   // month of date
    int d_day;     // day of month of date
    int d_offset;  // timezone offset
} DDATA[] = {
    //LINE  YEAR  MO  DAY  OFFSET
    //----  ----  --  ---  ------
    { L_,     1,  1,   1,       0 },
    { L_,     1,  1,   2,       0 },
    { L_,     1,  2,   1,       0 },
    { L_,     2,  1,   1,    1439 },
    { L_,     1,  1,   1,      -1 },
    { L_,     1,  1,   2,       1 },
    { L_,     1,  2,   1,   -1439 },
    { L_,     2,  1,   1,    1438 },

    { L_,  9998, 12,  31,       0 },
    { L_,  9999, 11,  30,       0 },
    { L_,  9999, 12,  30,       0 },
    { L_,  9999, 12,  31,       0 },
    { L_,  9998, 12,  31,       1 },
    { L_,  9999, 11,  30,   -1439 },
    { L_,  9999, 12,  30,      -1 },
    { L_,  9999, 12,  31,    -143 },
};
enum { k_NUM_DDATA = sizeof DDATA / sizeof *DDATA };

static const struct TData {
    int d_tline;
    int d_hour;
    int d_minute;
    int d_second;
    int d_milli;
    int d_micro;
} TDATA[] = {
    { L_,  0,  0,  0,   0,   0 },
    { L_, 23, 59, 59, 999, 999 },
    { L_, 24,  0,  0,   0,   0 },
    { L_, 12, 34, 56, 123, 456 },
    { L_,  6,  7,  8, 654, 321 },
};
enum { k_NUM_TDATA = sizeof TDATA / sizeof *TDATA };

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;

    (void) veryVerbose;
    (void) veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // TESTING: `bsl::format` OPTIONS AFFECTING THE TIME ZONE
        //
        // Concerns:
        // 1. That all the format options that affect the nature of the time
        //    zone display function properly.
        //
        // Plan:
        // 1. Do nested loops iterating through the global `DDATA` and `TDATA`
        //    above to get a variety of `Date`, `Time`, and `offset` values.
        //
        // 2. Create `X`, a `DatetimeTz` built out of those `Date`, `Time`, and
        //    `offset` values.
        //
        // 3. Use `operator<<` and `setIso8601DatetimeTz` to generate strings
        //    for the default and Iso8601 forms of output of `X`.
        //
        // 4. Nest another loop counting over the range `[ 0 .. 32 )` to
        //    drive all combinations of 5 bools.
        //
        //    1. `ISO` -- "%i" -- output is to be Iso8601 format.
        //
        //    2. `COLON` -- ":" -- there is to be a ':' separating the hours
        //       and minutes of the time zone.
        //
        //    3. `NO_COLON` -- "_"  -- there is to be no ':' separating the
        //       hours and minutes of the time zone.
        //
        //    4. `Z` -- "Z" -- if the offset is 0, the time zone is to be
        //       displayed as "Z".  If the offset is not 0, this has no effect.
        //
        //    5. `COMMA` -- the decimal point in the seconds field is to be
        //       printed as ',' rather than '.'.
        //
        // 5. Use the 5 `bool`s to drive modification of the format string.
        //
        // 6. Start with the strings created in step P-3 and use the 5 bools to
        //    modify them into the expected output for format string.
        //
        // 7. Call `bsl::vformat_to` to output `X` with the format created,
        //    and check if it matches the expected output.  Repeat this with
        //    wide strings.
        //
        // Testing:
        //   `bsl::format` OPTIONS AFFECTING THE TIME ZONE
        // --------------------------------------------------------------------

        if (verbose) cout <<
                    "TESTING: `bsl::format` OPTIONS AFFECTING THE TIME ZONE\n"
                    "======================================================\n";

        bslma::TestAllocator da;
        bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);

        for (int td = 0; td < u::k_NUM_DDATA; ++td) {
            const u::DData& dData  = u::DDATA[td];
            const int       DLINE  = dData.d_line;
            const int       YEAR   = dData.d_year;
            const int       MONTH  = dData.d_month;
            const int       DAY    = dData.d_day;
            const int       OFFSET = dData.d_offset;

            const bdlt::Date  DATE(YEAR, MONTH, DAY);

            for (int ti = 0; ti < u::k_NUM_TDATA; ++ti) {
                const u::TData& tData  = u::TDATA[ti];
                const int       TLINE  = tData.d_tline;
                const int       HOUR   = tData.d_hour;
                const int       MINUTE = tData.d_minute;
                const int       SECOND = tData.d_second;
                const int       MILLI  = tData.d_milli;
                const int       MICRO  = tData.d_micro;

                const bdlt::Time TIME(HOUR, MINUTE, SECOND, MILLI, MICRO);

                if (HOUR == 24 && 0 != OFFSET) {
                    continue;
                }

                const bdlt::DatetimeTz X(bdlt::Datetime(DATE, TIME), OFFSET);

                bsl::ostringstream oss;

                oss << X;
                const string nakedObj = oss.str();

                string nakedIso8601;
                u::setIso8601DatetimeTz(&nakedIso8601, X);

                for (unsigned flags = 0; flags < 32; ++flags) {
                    const unsigned FLAGS    = flags;
                    const bool     ISO      = flags &  1;
                    const bool     COLON    = flags &  2;
                    const bool     NO_COLON = flags &  4;
                    const bool     Z        = flags &  8;
                    const bool     COMMA    = flags & 16;

                    if (COLON & NO_COLON) {
                        continue;
                    }

                    string fmts = "{:}";
                    if (ISO) {
                        fmts.insert(2, "%i");
                    }
                    if (COLON) {
                        fmts.insert(2, ":");
                    }
                    if (NO_COLON) {
                        fmts.insert(2, "_");
                    }
                    if (Z) {
                        fmts.insert(2, "Z");
                    }
                    if (COMMA) {
                        fmts.insert(2, ",");
                    }
                    const char    *FMTS = fmts.c_str();
                    wstring  fmtwString;
                    TU::widen(&fmtwString, FMTS);
                    const wchar_t *FMTW = fmtwString.c_str();

                    string exps = ISO ? nakedIso8601 : nakedObj;
                    const string& EXPS = exps;
                    if (!ISO && COLON) {
                        exps.insert(exps.length() - 2, ":");
                    }
                    if (ISO && NO_COLON) {
                        size_t idx = exps.rfind(':');
                        ASSERT(u::npos != idx);
                        exps.erase(idx, 1);
                    }
                    if (Z && 0 == OFFSET) {
                        size_t idx = exps.find_last_of("+-");;
                        ASSERT(u::npos != idx);
                        exps.resize(idx);
                        exps += 'Z';
                    }
                    if (COMMA) {
                        size_t idx = exps.find('.');
                        ASSERT(u::npos != idx);
                        exps[idx] = ',';
                    }
                    wstring expw;    const wstring& EXPW = expw;
                    TU::widen(&expw, exps);

                    if (veryVerbose) {
                        P_(X);    PH_(FLAGS);    P_(FMTS);    P(EXPS);
                    }

                    char bufS[1024];
                    char *ends = bsl::vformat_to(bufS,
                                                 FMTS,
                                                 bsl::make_format_args(X));
                    *ends = '\0';
                    string ress  = bufS;
                    ASSERTV(DLINE, TLINE, FLAGS, FMTS, EXPS, ress,
                                                                 EXPS == ress);

                    wchar_t bufW[1024];
                    wchar_t *endw = bsl::vformat_to(bufW,
                                                    FMTW,
                                                    bsl::make_wformat_args(X));
                    *endw = 0;
                    wstring resw = bufW;
                    ASSERTV(DLINE, TLINE, FLAGS, EXPS, TU::narrow(FMTW),
                             TU::narrow(resw), TU::narrow(EXPW), EXPW == resw);
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING: `bsl::format` OF DATE AND TIME PARTS
        //
        // Concerns:
        // 1. All the percent sequences that print output from a `Date`
        //    produce correct output when called on a `DatetimeTz`.
        //
        // 3. The `width` argument works properly when printing
        //    `DatetimeTz.date()`s by default or using '%D', we test left,
        //    right, and center padding.
        //
        // Plans:
        // 1. Have a function `widen` that takes a `bsl::string` and returns
        //    that string translated to a `bsl::wstring`.  All calls to
        //    `bsl::format("...", ...)` will also have a corresponding
        //    `bsl::format(L"...", ...)` with the results of both being
        //    checked.
        //
        // 2. Have a table `DEFAULT_DATA[]` driving different `Date` values.
        //    Loop through the values, initializing `Date`s.
        //
        // 3. Verify that the default and '%i' output are as expected.
        //
        // 4. Verify using other format strings that produce output equivalent
        //    to default and '%i' output, thereby exercising other '%'
        //    sequences.
        //
        // 5. Test the rest of the more obscure '%' sequences.
        //
        // 6. For default output:
        //    * Loop iterating through different values of `precision` to be
        //      passed and verify output is as expected.
        //    * Loop iterating through different values of `width` to be passed
        //      and verify output is as expected, for right, left, and center
        //      padding.
        //
        // 7. Repeat the steps of '6' for '%i' output.
        //
        // Testing:
        //   bsl::string  bsl::format( "...", datetime);
        //   bsl::wstring bsl::format(L"...", datetime);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING: `bsl::format`\n"
                             "======================\n";

        bslma::TestAllocator da;
        bslma::DefaultAllocatorGuard defaultAllocatorGuard(&da);

        for (int td = 0; td < u::k_NUM_DDATA; ++td) {
            const u::DData& dData  = u::DDATA[td];
            const int       DLINE  = dData.d_line;    (void) DLINE;
            const int       YEAR   = dData.d_year;
            const int       MONTH  = dData.d_month;
            const int       DAY    = dData.d_day;
            const int       OFFSET = dData.d_offset;

            const bdlt::Date  DATE(YEAR, MONTH, DAY);

            for (int ti = 0; ti < u::k_NUM_TDATA; ++ti) {
                const u::TData& tData  = u::TDATA[ti];
                const int       TLINE  = tData.d_tline;    (void) TLINE;
                const int       HOUR   = tData.d_hour;
                const int       MINUTE = tData.d_minute;
                const int       SECOND = tData.d_second;
                const int       MILLI  = tData.d_milli;
                const int       MICRO  = tData.d_micro;

                const bdlt::Time TIME(HOUR, MINUTE, SECOND, MILLI, MICRO);

                if (HOUR == 24 && 0 != OFFSET) {
                    continue;
                }

                const bdlt::DatetimeTz X(bdlt::Datetime(DATE, TIME), OFFSET);

                if (veryVerbose) P(X);

                bsl::ostringstream oss;
                oss << X.localDatetime().date();

                const bsl::string   nakedDate = oss.str();
                bsl::string         exps = nakedDate;
                const bsl::string&  EXPS = exps;
                bsl::wstring        expw;    const bsl::wstring& EXPW = expw;
                TU::widen(&expw, EXPS);

                if (veryVerbose) cout << "Date Part\n";

                bsl::string ress = bsl::format("{}", X.localDatetime().date());
                ASSERTV(DLINE, EXPS, ress, EXPS == ress);
                bsl::wstring resw =
                                  bsl::format(L"{}", X.localDatetime().date());
                ASSERTV(DLINE, EXPW == resw);

                U_TEST_FORMAT(EXPS, "{:%D}", X);
                U_TEST_FORMAT(EXPS, "{:%d%b%Y}", X);
                U_TEST_FORMAT(EXPS, "{:%d%h%C%y}", X);

                bsl::string fDate;
                u::appendIso8601Date(&fDate, X.localDatetime().date());
                exps = fDate;
                TU::widen(&expw, EXPS);

                ress  = bsl::format("{:%i}",
                                    X.localDatetime().date());
                ASSERTV(DLINE, EXPS, ress, EXPS == ress);
                resw = bsl::format(L"{:%i}",
                                    X.localDatetime().date());
                ASSERTV(DLINE, EXPW == resw);

                U_TEST_FORMAT(EXPS, "{:%F}", X);
                U_TEST_FORMAT(EXPS, "{:%Y-%m-%d}", X);

                oss.str("");
                {
                    const size_t width = oss.width();
                    const char   fill  = oss.fill();
                    oss.width(3);
                    oss.fill('0');
                    oss << DATE.dayOfYear();
                    oss.width(width);
                    oss.fill(fill);
                }
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%j}", X);    // 3-digit day of year

                oss.str("");
                oss << DATE.dayOfWeek();// 3-letter weekday abbrev
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%a}", X);

                int dow = X.localDatetime().dayOfWeek() - 1;     // Sunday is 0
                oss.str("");
                oss << dow;
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%w}", X);

                dow = (dow + 7 - 1) % 7 + 1;        // Monday is 1
                oss.str("");
                oss << dow;
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%u}", X);

                oss.str("");
                {
                    const size_t width = oss.width();
                    const char   fill  = oss.fill();
                    oss.width(2);
                    oss.fill('0');
                    oss << (DATE.year() % 100);    // 2-digit year
                    oss.width(width);
                    oss.fill(fill);
                }
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%y}", X);

                oss.str("");
                {
                    const size_t width = oss.width();
                    const char   fill  = oss.fill();
                    oss.width(2);
                    oss.fill('0');
                    oss << (DATE.year() / 100);     // 2-digit century
                    oss.width(width);
                    oss.fill(fill);
                }
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%C}", X);

                oss.str("");
                oss << DATE.monthOfYear();          // 3-char month abbrev
                oss << ',';
                oss << DATE.monthOfYear();          // 3-char month abbrev
                exps = oss.str();

                U_TEST_FORMAT(EXPS, "{:%b,%h}", X);

                size_t nakedWidth = nakedDate.length();

                for (int rightPad = -2; rightPad < 10; ++rightPad) {
                    exps = nakedDate;
                    if (0 < rightPad) {
                        exps.append(rightPad, '*');
                    }
                    U_TEST_FORMAT_ARG(EXPS, "{:*<{}%D}", X,
                                                        nakedWidth + rightPad);
                }

                for (int leftPad = -2; leftPad < 10; ++leftPad) {
                    exps.clear();
                    if (0 < leftPad) {
                        exps.append(leftPad, '*');
                    }
                    exps += nakedDate;
                    U_TEST_FORMAT_ARG(EXPS, "{:*>{}%D}", X,
                                                         nakedWidth + leftPad);
                }

                for (int centerPad = -2; centerPad < 10; ++centerPad) {
                    exps.clear();
                    if (0 < centerPad / 2) {
                        exps.append(centerPad / 2, '*');
                    }
                    exps += nakedDate;
                    int rightPad = centerPad - centerPad / 2;
                    if (0 < rightPad) {
                        exps.append(rightPad, '*');
                    }
                    U_TEST_FORMAT_ARG(EXPS, "{:*^{}%D}", X,
                                                        nakedWidth+ centerPad);
                }

                nakedWidth = fDate.length();

                for (int rightPad = -2; rightPad < 10; ++rightPad) {
                    exps = fDate;
                    if (0 < rightPad) {
                        exps.append(rightPad, '*');
                    }
                    U_TEST_FORMAT_ARG(EXPS, "{:*<{}%F}", X,
                                                        nakedWidth + rightPad);
                }

                for (int leftPad = -2; leftPad < 10; ++leftPad) {
                    exps.clear();
                    if (0 < leftPad) {
                        exps.append(leftPad, '*');
                    }
                    exps += fDate;
                    U_TEST_FORMAT_ARG(EXPS, "{:*>{}%F}", X,
                                                         nakedWidth + leftPad);
                }

                for (int centerPad = -2; centerPad < 10; ++centerPad) {
                    exps.clear();
                    if (0 < centerPad / 2) {
                        exps.append(centerPad / 2, '*');
                    }
                    exps += fDate;
                    int rightPad = centerPad - centerPad / 2;
                    if (0 < rightPad) {
                        exps.append(rightPad, '*');
                    }
                    U_TEST_FORMAT_ARG(EXPS, "{:*^{}%F}", X,
                                                        nakedWidth +centerPad);
                }

                if (veryVerbose) cout << "Time Part\n";

                oss.str("");
                oss << TIME;

                const bsl::string nakedTime = oss.str();
                exps = nakedTime;

                nakedWidth = exps.length();

                U_TEST_FORMAT(EXPS, "{}", X.localDatetime().time());

                U_TEST_FORMAT(EXPS, "{:%T}", X);
                U_TEST_FORMAT(EXPS, "{:%H:%M:%S}", X);

                U_TEST_FORMAT(EXPS, "{:.6%i}", TIME);

                U_TEST_FORMAT(EXPS, "{:15.6%T}", X);

                exps.resize(nakedWidth - 3); // default %i precision is 3 not 6

                U_TEST_FORMAT(EXPS, "{:%i}", X.localDatetime().time());

                U_TEST_FORMAT(EXPS, "{:.3%T}", X);

                {
                    char *pc = &exps[0] + EXPS.find('.');
                    *pc = ',';
                }
                exps.insert(0, "%\n\t");

                U_TEST_FORMAT(EXPS, "{:.3,%%%n%t%T}", X);

                size_t precision = 10;
                exps = oss.str() + "0000";
                do {
                    u::trimPrecision(&exps);
                    --precision;

                    if (verbose) {
                        cout << "{:.{}%T}\t"; P_(precision); P(EXPS);
                    }

                    ASSERT((u::npos == EXPS.find('.')) == (0 == precision));

                    U_TEST_FORMAT_ARG(EXPS, "{:.{}%T}", X, precision);
                } while (0 < precision);

                for (int rightPad = -2; rightPad < 10; ++rightPad) {
                    exps = nakedTime;
                    if (0 < rightPad) {
                        exps.append(rightPad, '*');
                    }
                    U_TEST_FORMAT_ARG(EXPS, "{:*<{}%T}", X,
                                                        nakedWidth + rightPad);
                }

                for (int leftPad = -2; leftPad < 10; ++leftPad) {
                    exps.clear();
                    if (0 < leftPad) {
                        exps.append(leftPad, '*');
                    }
                    exps += nakedTime;
                    U_TEST_FORMAT_ARG(EXPS, "{:*>{}%T}", X,
                                                         nakedWidth + leftPad);
                }

                for (int centerPad = -2; centerPad < 10; ++centerPad) {
                    exps.clear();
                    if (0 < centerPad / 2) {
                        exps.append(centerPad / 2, '*');
                    }
                    exps += nakedTime;
                    int rightPad = centerPad - centerPad / 2;
                    if (0 < rightPad) {
                        exps.append(rightPad, '*');
                    }
                    U_TEST_FORMAT_ARG(EXPS, "{:*^{}%T}", X,
                                                        nakedWidth +centerPad);
                }
            }
        }
#undef U_TEST_FORMAT
#undef U_TEST_FORMAT_ARG
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING bsl::format
        //
        // Concerns:
        // 1. That `bsl:format` compiles and produces correct output when
        //    outputting `DatetimeTz`s.
        //
        // Plan:
        // 1. Call with a wide variety of format strings and check the output.
        //
        // Testing:
        //   bsl::format(format_string, DatetimeTz);
        // --------------------------------------------------------------------

        if (verbose) cout << "Demo: bdlt::DatetimeTz\n"
                             "======================\n";

        bsl::ostringstream dummy;
        bsl::ostream&      out = veryVerbose ? cout : dummy;

#define U_DO(stmt)                                                            \
        stmt;                                                                 \
        out << #stmt << ';' << endl

        U_DO(bdlt::Datetime   startDatetime(1500, 5, 17, 15, 5, 23, 456, 789));
        U_DO(bdlt::DatetimeTz datetimeTz(startDatetime, 5 * 60));
        U_DO(bsl::string      result);
        U_DO(bsl::wstring     wResult);
        U_DO(bsl::wstring     wExpected);
        U_DO(bsl::ostringstream oss);

#define U_FMT_DATETIMETZ(fmt, expected)                                       \
        if (veryVerbose) cout << fmt << endl;                                 \
        result = bsl::format(fmt, datetimeTz);                                \
        ASSERTV(result, expected == result);                                  \
        TU::widen(&wExpected, expected);                                      \
        wResult = bsl::format(L ## fmt, datetimeTz);                          \
        ASSERTV(expected, TU::narrow(wResult), wExpected == wResult);         \
        out << "result = bsl::format(" #fmt ", datetimeTz);" << endl;         \
        out << "result: \"" << result << "\"\n";

        oss  << datetimeTz;
        result = oss.str();
        out << "result = (oss << datetimeTz, oss.str());" << endl;        \
        out << "result: \"" << result << "\"\n";
        ASSERTV(result ==              "17MAY1500_15:05:23.456789+0500");

        out << "3 Ways of using bsl::format the object, all yield the\n"
               "same output as `operator<<`.  The third method is just\n"
                "explicity specifying the default format\n";

        U_FMT_DATETIMETZ("{}",         "17MAY1500_15:05:23.456789+0500");
        U_FMT_DATETIMETZ("{:}",        "17MAY1500_15:05:23.456789+0500");
        U_FMT_DATETIMETZ("{:%%}",      "%");
        U_FMT_DATETIMETZ("{:%% }",     "% ");
        U_FMT_DATETIMETZ("{:%D_%T%z}", "17MAY1500_15:05:23.456789+0500");

        out << "Use '%i' for Iso8601 output (different from `operator<<`\n"
               "note that default precision is 3 for Iso8601, 6 for others,\n"
               "and note that the default for '%i' is that the colon in\n"
               "the time zone is displayed\n";

        U_FMT_DATETIMETZ("{:%i}",      "1500-05-17T15:05:23.456+05:00");

        out << "Specifying a precision overrides both Iso8610 and\n"
               "non-Iso8601 output\n";

        U_FMT_DATETIMETZ("{:.4}",      "17MAY1500_15:05:23.4567+0500");
        U_FMT_DATETIMETZ("{:.4%i}",    "1500-05-17T15:05:23.4567+05:00");

        U_FMT_DATETIMETZ("{:.0}",      "17MAY1500_15:05:23+0500");
        U_FMT_DATETIMETZ("{:.0%i}",    "1500-05-17T15:05:23+05:00");

        out << "Specifying '%_' indicates that the ':' in the time\n"
               "zone is suppressed.  Note that this only affects output\n"
               "done later in the format than the '%_'\n";

        U_FMT_DATETIMETZ("{:_}",   "17MAY1500_15:05:23.456789+0500");
        U_FMT_DATETIMETZ("{:_%i}", "1500-05-17T15:05:23.456+0500");

        out << "Specifying '%:' indicates that the ':' in the time\n"
               "zone is displayed.  Note that this only affects output\n"
               "done later in the format than the '%_'\n";

        U_FMT_DATETIMETZ("{::}",   "17MAY1500_15:05:23.456789+05:00");
        U_FMT_DATETIMETZ("{::%i}", "1500-05-17T15:05:23.456+05:00");

        out << "Specifying '%,' indicates that the decimal is to be"
               "comma, not a period\n";

        U_FMT_DATETIMETZ("{:,}",   "17MAY1500_15:05:23,456789+0500");
        U_FMT_DATETIMETZ("{:,%i}", "1500-05-17T15:05:23,456+05:00");

        out << "Specifying 'Z' before the first '%' indicates that if the\n"
               "the time zone is UTC, it is just to be output as\n"
               "'Z'.  In this case, the time zone is not UTC, so the\n"
               "'Z' option is ignored.\n";

        U_FMT_DATETIMETZ("{:Z}",   "17MAY1500_15:05:23.456789+0500");
        U_FMT_DATETIMETZ("{:Z%i}", "1500-05-17T15:05:23.456+05:00");

        out << "Set the time zone to UTC\n";

        U_DO(datetimeTz.setDatetimeTz(datetimeTz.localDatetime(), 0));

        U_FMT_DATETIMETZ("{:}",   "17MAY1500_15:05:23.456789+0000");
        U_FMT_DATETIMETZ("{:%i}", "1500-05-17T15:05:23.456+00:00");

        U_FMT_DATETIMETZ("{:Z}",   "17MAY1500_15:05:23.456789Z");
        U_FMT_DATETIMETZ("{:Z%i}", "1500-05-17T15:05:23.456Z");

#undef U_DO
#undef U_FMT_DATETIMETZ
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }

    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
