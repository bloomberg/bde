// bdlt_iso8601util.t.cpp                                             -*-C++-*-
#include <bdlt_iso8601util.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bsl_cctype.h>      // 'isdigit'
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#undef SEC

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a suite of static member functions
// (pure functions) that perform conversions between the values of several
// 'bdlt' vocabulary types and corresponding string representations, where the
// latter are defined by the ISO 8601 standard.  The general plan is that each
// function is to be independently tested using the table-driven technique.  A
// set of test vectors is defined globally for use in testing all functions.
// This global data is sufficient for thoroughly testing the string generating
// functions, but additional test vectors are required to address concerns
// specific to the string parsing functions.  Hence, additional test data is
// defined locally to the test cases that verify parsing.
//
// Global Concerns:
//: o No memory is ever allocated from the global allocator.
//: o Precondition violations are detected in appropriate build modes.
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 1] int generate(char *, int, const TimeInterval&);
// [ 1] int generate(char *, int, const TimeInterval&, const Config&);
// [ 2] int generate(char *, int, const Date&);
// [ 2] int generate(char *, int, const Date&, const Config&);
// [ 3] int generate(char *, int, const Time&);
// [ 3] int generate(char *, int, const Time&, const Config&);
// [ 4] int generate(char *, int, const Datetime&);
// [ 4] int generate(char *, int, const Datetime&, const Config&);
// [ 5] int generate(char *, int, const DateTz&);
// [ 5] int generate(char *, int, const DateTz&, const Config&);
// [ 6] int generate(char *, int, const TimeTz&);
// [ 6] int generate(char *, int, const TimeTz&, const Config&);
// [ 7] int generate(char *, int, const DatetimeTz&);
// [ 7] int generate(char *, int, const DatetimeTz&, const Config&);
// [12] int generate(char *, int, const DateOrDateTz&);
// [12] int generate(char *, int, const DateOrDateTz&, const Config&);
// [13] int generate(char *, int, const TimeOrTimeTz&);
// [13] int generate(char *, int, const TimeOrTimeTz&, const Config&);
// [14] int generate(char *, int, const DatetimeOrDatetimeTz&);
// [14] int generate(char*,int,const DatetimeOrDatetimeTz&,const Config&);
// [ 1] int generate(string *, const TimeInterval&);
// [ 1] int generate(string *, const TimeInterval&, const Config&);
// [ 2] int generate(string *, const Date&);
// [ 2] int generate(string *, const Date&, const Config&);
// [ 3] int generate(string *, const Time&);
// [ 3] int generate(string *, const Time&, const Config&);
// [ 4] int generate(string *, const Datetime&);
// [ 4] int generate(string *, const Datetime&, const Config&);
// [ 5] int generate(string *, const DateTz&);
// [ 5] int generate(string *, const DateTz&, const Config&);
// [ 6] int generate(string *, const TimeTz&);
// [ 6] int generate(string *, const TimeTz&, const Config&);
// [ 7] int generate(string *, const DatetimeTz&);
// [ 7] int generate(string *, const DatetimeTz&, const Config&);
// [12] int generate(string *, const DateOrDateTz&);
// [12] int generate(string *, const DateOrDateTz&, const Config&);
// [13] int generate(string *, const TimeOrTimeTz&);
// [13] int generate(string *, const TimeOrTimeTz&, const Config&);
// [14] int generate(string *, const DatetimeOrDatetimeTz&);
// [14] int generate(string*, const DatetimeOrDatetimeTz&, const Config&);
// [ 1] ostream generate(ostream&, const TimeInterval&);
// [ 1] ostream generate(ostream&, const TimeInterval&, const Config&);
// [ 2] ostream generate(ostream&, const Date&);
// [ 2] ostream generate(ostream&, const Date&, const Config&);
// [ 3] ostream generate(ostream&, const Time&);
// [ 3] ostream generate(ostream&, const Time&, const Config&);
// [ 4] ostream generate(ostream&, const Datetime&);
// [ 4] ostream generate(ostream&, const Datetime&, const Config&);
// [ 5] ostream generate(ostream&, const DateTz&);
// [ 5] ostream generate(ostream&, const DateTz&, const Config&);
// [ 6] ostream generate(ostream&, const TimeTz&);
// [ 6] ostream generate(ostream&, const TimeTz&, const Config&);
// [ 7] ostream generate(ostream&, const DatetimeTz&);
// [ 7] ostream generate(ostream&, const DatetimeTz&, const Config&);
// [12] ostream generate(ostream&, const DateOrDateTz&);
// [12] ostream generate(ostream&, const DateOrDateTz&, const Config&);
// [13] ostream generate(ostream&, const TimeOrTimeTz&);
// [13] ostream generate(ostream&, const TimeOrTimeTz&, const Config&);
// [14] ostream generate(ostream&, const DatetimeOrDatetimeTz&);
// [14] ostream generate(ostream&,const DatetimeOrDatetimeTz&,const Con&);
// [ 1] int generateRaw(char *, const TimeInterval&);
// [ 1] int generateRaw(char *, const TimeInterval&, const Config&);
// [ 2] int generateRaw(char *, const Date&);
// [ 2] int generateRaw(char *, const Date&, const Config&);
// [ 3] int generateRaw(char *, const Time&);
// [ 3] int generateRaw(char *, const Time&, const Config&);
// [ 4] int generateRaw(char *, const Datetime&);
// [ 4] int generateRaw(char *, const Datetime&, const Config&);
// [ 5] int generateRaw(char *, const DateTz&);
// [ 5] int generateRaw(char *, const DateTz&, const Config&);
// [ 6] int generateRaw(char *, const TimeTz&);
// [ 6] int generateRaw(char *, const TimeTz&, const Config&);
// [ 7] int generateRaw(char *, const DatetimeTz&);
// [ 7] int generateRaw(char *, const DatetimeTz&, const Config&);
// [12] int generateRaw(char *, const DateOrDateTz&);
// [12] int generateRaw(char *, const DateOrDateTz&, const Config&);
// [13] int generateRaw(char *, const TimeOrTimeTz&);
// [13] int generateRaw(char *, const TimeOrTimeTz&, const Config&);
// [14] int generateRaw(char *, const DatetimeOrDatetimeTz&);
// [14] int generateRaw(char*,const DatetimeOrDatetimeTz&, const Config&);
// [ 8] int parse(TimeInterval *, const char *, int);
// [ 9] int parse(Date *, const char *, int);
// [10] int parse(Time *, const char *, int);
// [11] int parse(Datetime *, const char *, int);
// [ 9] int parse(DateTz *, const char *, int);
// [10] int parse(TimeTz *, const char *, int);
// [11] int parse(DatetimeTz *, const char *, int);
// [15] int parse(DateOrDateTz *, const char *, int);
// [16] int parse(TimeOrTimeTz *, const char *, int);
// [17] int parse(DatetimeOrDatetimeTz *, const char *, int);
// [ 8] int parse(TimeInterval *result, const StringRef& string);
// [ 9] int parse(Date *result, const StringRef& string);
// [10] int parse(Time *result, const StringRef& string);
// [11] int parse(Datetime *result, const StringRef& string);
// [ 9] int parse(DateTz *result, const StringRef& string);
// [10] int parse(TimeTz *result, const StringRef& string);
// [11] int parse(DatetimeTz *result, const StringRef& string);
// [15] int parse(DateOrDateTz *result, const StringRef& string);
// [15] int parse(TimeOrTimeTz *result, const StringRef& string);
// [17] int parse(DatetimeOrDatetimeTz *result, const StringRef& string);
// [11] int parseRelaxed(Datetime *, const char *, int);
// [11] int parseRelaxed(DatetimeTz *, const char *, int);
// [17] int parseRelaxed(DatetimeOrDatetimeTz *, const char *, int);
// [11] int parseRelaxed(Datetime *, const bsl::string_view&);
// [11] int parseRelaxed(DatetimeTz *, const bsl::string_view&);
// [17] int parseRelaxed(DatetimeOrDatetimeTz *, const bsl::string_view&);
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
// [ 2] int generate(char *, const Date&, int);
// [ 3] int generate(char *, const Time&, int);
// [ 4] int generate(char *, const Datetime&, int);
// [ 5] int generate(char *, const DateTz&, int);
// [ 6] int generate(char *, const TimeTz&, int);
// [ 7] int generate(char *, const DatetimeTz&, int);
// [ 5] int generate(char *, const DateTz&, int, bool useZ);
// [ 6] int generate(char *, const TimeTz&, int, bool useZ);
// [ 7] int generate(char *, const DatetimeTz&, int, bool useZ);
// [ 5] ostream generate(ostream&, const DateTz&, bool useZ);
// [ 6] ostream generate(ostream&, const TimeTz&, bool useZ);
// [ 7] ostream generate(ostream&, const DatetimeTz&, bool useZ);
// [ 5] int generateRaw(char *, const DateTz&, bool useZ);
// [ 6] int generateRaw(char *, const TimeTz&, bool useZ);
// [ 7] int generateRaw(char *, const DatetimeTz&, bool useZ);
#endif // BDE_OMIT_INTERNAL_DEPRECATED
//-----------------------------------------------------------------------------
// [18] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

//=============================================================================
//                  GLOBALS, TYPEDEFS, CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlt::Iso8601Util              Util;
typedef bdlt::Iso8601UtilConfiguration Config;
typedef bsl::string_view               StrView;

const int k_INTERVAL_MAX_PRECISION   = 9;
const int k_DATE_MAX_PRECISION       = 3;
const int k_DATETZ_MAX_PRECISION     = 3;
const int k_DATETIME_MAX_PRECISION   = 6;
const int k_DATETIMETZ_MAX_PRECISION = 6;
const int k_TIME_MAX_PRECISION       = 6;
const int k_TIMETZ_MAX_PRECISION     = 6;

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA generally usable across 'generate' and 'parse' test
// cases.

// *** 'TimeInterval' Data ***

struct DefaultIntervalDataRow {
    int                 d_line;         // source line number
    bsls::Types::Int64  d_sec;          // seconds
    int                 d_usec;         // nanoseconds
    const char         *d_iso8601;      // ISO 8601 string
    bool                d_canonical;    // There are many strings which can be
                                        // parsed into the same TimeInterval,
                                        // but for each TimeInterval there is
                                        // only one canonical string
                                        // representation (the string returned
                                        // by the generate functions).

};

static const DefaultIntervalDataRow DEFAULT_INTERVAL_DATA[] = {
    //LINE             SECONDS             NANOSECONDS
    //---- ------------------------------  -----------
    //                                       ISO8601                CANONICAL
    //                        ------------------------------------  ---------
    { L_,                               0,         0,
                              "P0W",                                   false },
    { L_,                               0,         0,
                              "P0D",                                   false },
    { L_,                               0,         0,
                              "PT0H",                                  false },
    { L_,                               0,         0,
                              "PT0M",                                  false },
    { L_,                               0,         0,
                              "PT0S",                                  false },
    { L_,                               0,         0,
                              "PT0.000000000S",                         true },
    { L_,                               0,         0,
                              "P0DT0M",                                false },
    { L_,                               0,         0,
                              "P0DT0S",                                false },
    { L_,                               0,         0,
                              "P0W0DT0H0M0S",                          false },
    { L_,                               0,         0,
                              "P0W0DT0H0M0.0S",                        false },
    { L_,                               0,         0,
                              "P0W0DT0H0M0.000000000S",                false },
    { L_,                               0,         0,
                              "P0W0DT0H0M0.0000000000S",               false },
    { L_,                          604800,         0,
                              "P1W",                                   false },
    { L_,                          604800,         0,
                              "P1WT0.000000000S",                       true },
    { L_,                           86400,         0,
                              "P1D",                                   false },
    { L_,                           86400,         0,
                              "P1DT0.000000000S",                       true },
    { L_,                            3600,         0,
                              "PT1H",                                  false },
    { L_,                            3600,         0,
                              "PT1H0.000000000S",                       true },
    { L_,                              60,         0,
                              "PT1M",                                  false },
    { L_,                              60,         0,
                              "PT1M0.000000000S",                       true },
    { L_,                               1,         0,
                              "PT1S",                                  false },
    { L_,                               1,         0,
                              "PT1.000000000S",                         true },
    { L_,                           93600,         0,
                              "P1DT2H0.000000000S",                     true },
    { L_,                           93600,         0,
                              "PT26H",                                 false },
    { L_,                    172800 + 300,         0,
                              "P2DT5M0.000000000S",                     true },
    { L_,                    172800 + 300,         0,
                              "P2DT5M",                                false },
    { L_,                      259200 + 7,         0,
                              "P3DT7.000000000S",                       true },
    { L_, 604800 + 86400 + 7200 + 240 + 5,      5497,
                              "P1W1DT2H4M5.000005497S",                 true },
    { L_,                               0,   3000000,
                              "PT0.003000000S",                         true },
    { L_,                               0,   3000000,
                              "PT0.0030S",                             false },
    { L_,                               0,   3000000,
                              "P0W0DT0H0M0.003S",                      false },
    { L_,                               0,   3000000,
                              "P0W0DT0H0M0.0030S",                     false },
    { L_,                               0,         5,
                              "PT0.0000000045S",                       false },
    { L_,                               0,         4,
                              "PT0.00000000449S",                      false },
    { L_,           9223372036854775807LL, 999999999,
                              "PT9223372036854775807.999999999S",      false },
    { L_,           9223372036854775807LL, 999999999,
                              "P15250284452471W3DT15H30M7.999999999S",  true },
};
const int NUM_DEFAULT_INTERVAL_DATA =
static_cast<int>(sizeof DEFAULT_INTERVAL_DATA / sizeof *DEFAULT_INTERVAL_DATA);


// *** 'Date' Data ***

struct DefaultDateDataRow {
    int         d_line;     // source line number
    int         d_year;     // year (of calendar date)
    int         d_month;    // month
    int         d_day;      // day
    const char *d_iso8601;  // ISO 8601 string
};

static
const DefaultDateDataRow DEFAULT_DATE_DATA[] =
{
    //LINE   YEAR   MONTH   DAY      ISO8601
    //----   ----   -----   ---    ------------
    { L_,       1,      1,    1,   "0001-01-01" },
    { L_,       9,      9,    9,   "0009-09-09" },
    { L_,      30,     10,   20,   "0030-10-20" },
    { L_,     842,     12,   19,   "0842-12-19" },
    { L_,    1847,      5,   19,   "1847-05-19" },
    { L_,    2000,      2,   29,   "2000-02-29" },
    { L_,    9999,     12,   31,   "9999-12-31" },
};
const int NUM_DEFAULT_DATE_DATA =
        static_cast<int>(sizeof DEFAULT_DATE_DATA / sizeof *DEFAULT_DATE_DATA);

// *** 'Time' Data ***

struct DefaultTimeDataRow {
    int         d_line;     // source line number
    int         d_hour;     // hour (of day)
    int         d_min;      // minute
    int         d_sec;      // second
    int         d_msec;     // millisecond
    int         d_usec;     // microsecond
    const char *d_iso8601;  // ISO 8601 string
};

static
const DefaultTimeDataRow DEFAULT_TIME_DATA[] =
{
    //LINE   HOUR   MIN   SEC   MSEC   USEC         ISO8601
    //----   ----   ---   ---   ----   ----    -----------------
    { L_,       0,    0,    0,     0,     0,   "00:00:00.000000" },
    { L_,       1,    2,    3,     4,     5,   "01:02:03.004005" },
    { L_,      10,   20,   30,    40,    50,   "10:20:30.040050" },
    { L_,      19,   43,   27,   805,   107,   "19:43:27.805107" },
    { L_,      23,   59,   59,   999,   999,   "23:59:59.999999" },
    { L_,      24,    0,    0,     0,     0,   "24:00:00.000000" },
};
const int NUM_DEFAULT_TIME_DATA =
        static_cast<int>(sizeof DEFAULT_TIME_DATA / sizeof *DEFAULT_TIME_DATA);

// *** Zone Data ***

struct DefaultZoneDataRow {
    int         d_line;     // source line number
    int         d_offset;   // offset (in minutes) from UTC
    const char *d_iso8601;  // ISO 8601 string
};

static
const DefaultZoneDataRow DEFAULT_ZONE_DATA[] =
{
    //LINE   OFFSET   ISO8601
    //----   ------   --------
    { L_,     -1439,  "-23:59" },
    { L_,      -120,  "-02:00" },
    { L_,       -30,  "-00:30" },
    { L_,         0,  "+00:00" },
    { L_,        90,  "+01:30" },
    { L_,       240,  "+04:00" },
    { L_,      1439,  "+23:59" },
};
const int NUM_DEFAULT_ZONE_DATA =
        static_cast<int>(sizeof DEFAULT_ZONE_DATA / sizeof *DEFAULT_ZONE_DATA);

// *** Configuration Data ***

struct DefaultCnfgDataRow {
    int  d_line;       // source line number
    bool d_omitColon;  // 'omitColonInZoneDesignator' attribute
    int  d_precision;  // 'precision'                     "
    bool d_useComma;   // 'useCommaForDecimalSign'        "
    bool d_useZ;       // 'useZAbbreviationForUtc'        "
};

static
const DefaultCnfgDataRow DEFAULT_CNFG_DATA[] =
{
    //LINE   omit ':'   precision   use ','   use 'Z'
    //----   --------   ---------   -------   -------
    { L_,      false,          3,   false,    false  },
    { L_,      false,          3,   false,     true  },
    { L_,      false,          3,    true,    false  },
    { L_,      false,          3,    true,     true  },
    { L_,      false,          6,   false,    false  },
    { L_,      false,          6,   false,     true  },
    { L_,      false,          6,    true,    false  },
    { L_,      false,          6,    true,     true  },
    { L_,       true,          3,   false,    false  },
    { L_,       true,          3,   false,     true  },
    { L_,       true,          3,    true,    false  },
    { L_,       true,          3,    true,     true  },
    { L_,       true,          6,   false,    false  },
    { L_,       true,          6,   false,     true  },
    { L_,       true,          6,    true,    false  },
    { L_,       true,          6,    true,     true  },

    // additional configurations

    { L_,      false,          0,   false,    false  },
    { L_,      false,          1,   false,    false  },
    { L_,      false,          2,   false,    false  },
    { L_,      false,          4,   false,    false  },
    { L_,      false,          5,   false,    false  }
};
const int NUM_DEFAULT_CNFG_DATA =
        static_cast<int>(sizeof DEFAULT_CNFG_DATA / sizeof *DEFAULT_CNFG_DATA);

// Define BAD (invalid) DATA generally usable across 'parse' test cases.

// *** Bad 'Date' Data ***

struct BadDateDataRow {
    int         d_line;     // source line number
    const char *d_invalid;  // test string
};

static
const BadDateDataRow BAD_DATE_DATA[] =
{
    //LINE  INPUT STRING
    //----  -------------------------
    { L_,   ""                      },  // length = 0

    { L_,   "0"                     },  // length = 1
    { L_,   "-"                     },
    { L_,   "+"                     },
    { L_,   "T"                     },
    { L_,   "Z"                     },
    { L_,   ":"                     },
    { L_,   " "                     },

    { L_,   "12"                    },  // length = 2
    { L_,   "3T"                    },
    { L_,   "4-"                    },
    { L_,   "x1"                    },
    { L_,   "T:"                    },
    { L_,   "+:"                    },

    { L_,   "999"                   },  // length = 3

    { L_,   "9999"                  },  // length = 4
    { L_,   "1-9-"                  },

    { L_,   "4-5-6"                 },  // length = 5
    { L_,   "+0130"                 },

    { L_,   "1-01-1"                },  // length = 6
    { L_,   "01-1-1"                },
    { L_,   "1-1-01"                },

    { L_,   "02-02-2"               },  // length = 7
    { L_,   "03-3-03"               },
    { L_,   "4-04-04"               },

    { L_,   "05-05-05"              },  // length = 8
    { L_,   "005-05-5"              },
    { L_,   "006-6-06"              },
    { L_,   "0006-6-6"              },

    { L_,   "0007-07-7"             },  // length = 9
    { L_,   "0008-8-08"             },
    { L_,   "009-09-09"             },

    { L_,   "0001 01-01"            },  // length = 10
    { L_,   "0001-01:01"            },
    { L_,   "0000-01-01"            },
    { L_,   "0001-00-01"            },
    { L_,   "0001-13-01"            },
    { L_,   "0001-01-00"            },
    { L_,   "0001-01-32"            },
    { L_,   "0001-04-31"            },
    { L_,   "1900-02-29"            },
    { L_,   "2000-02-30"            },

    { L_,   "0001-01-010"           },  // length = 11
    { L_,   "1970-12-310"           },
};
const int NUM_BAD_DATE_DATA =
                static_cast<int>(sizeof BAD_DATE_DATA / sizeof *BAD_DATE_DATA);

// *** Bad 'Time' Data ***

struct BadTimeDataRow {
    int         d_line;     // source line number
    const char *d_invalid;  // test string
};

static
const BadTimeDataRow BAD_TIME_DATA[] =
{
    //LINE  INPUT STRING
    //----  -------------------------
    { L_,   ""                       },  // length = 0

    { L_,   "0"                      },  // length = 1
    { L_,   "-"                      },
    { L_,   "+"                      },
    { L_,   "T"                      },
    { L_,   "Z"                      },
    { L_,   ":"                      },
    { L_,   "."                      },
    { L_,   ","                      },
    { L_,   " "                      },

    { L_,   "12"                     },  // length = 2
    { L_,   "3T"                     },
    { L_,   "4-"                     },
    { L_,   "x1"                     },
    { L_,   "T:"                     },
    { L_,   "+:"                     },

    { L_,   "222"                    },  // length = 3
    { L_,   "000"                    },
    { L_,   "1:2"                    },

    { L_,   "1234"                   },  // length = 4
    { L_,   "1:19"                   },
    { L_,   "11:9"                   },

    { L_,   "12:60"                  },  // length = 5
    { L_,   "2:001"                  },
    { L_,   "24:01"                  },
    { L_,   "25:00"                  },
    { L_,   "99:00"                  },

    { L_,   "1:2:30"                 },  // length = 6
    { L_,   "1:20:3"                 },
    { L_,   "10:2:3"                 },
    { L_,   "1:2:3."                 },
    { L_,   "12:100"                 },
    { L_,   ":12:12"                 },

    { L_,   "12:00:1"                },  // length = 7
    { L_,   "12:0:01"                },
    { L_,   "2:10:01"                },
    { L_,   "24:00.1"                },

    { L_,   "12:2:001"               },  // length = 8
    { L_,   "3:02:001"               },
    { L_,   "3:2:0001"               },
    { L_,   "20:20:61"               },
    { L_,   "24:00:01"               },

    { L_,   "04:05:06."              },  // length = 9
    { L_,   "04:05:006"              },
    { L_,   "12:59:100"              },

    { L_,   "03:02:001."             },  // length = 10
    { L_,   "03:02:001,"             },
    { L_,   "03:2:001.1"             },
    { L_,   "24:00:00.1"             },

    { L_,   "24:00:00.01"            },  // length = 11

    { L_,   "24:00:00.001"           },  // length = 12
    { L_,   "24:00:00.999"           },
    { L_,   "25:00:00.000"           },

    { L_,   "24:00:00.000001"        },  // length = 15
};
const int NUM_BAD_TIME_DATA =
                static_cast<int>(sizeof BAD_TIME_DATA / sizeof *BAD_TIME_DATA);

// *** Bad Zone Data ***

struct BadZoneDataRow {
    int         d_line;     // source line number
    const char *d_invalid;  // test string
};

static
const BadZoneDataRow BAD_ZONE_DATA[] =
{
    //LINE  INPUT STRING
    //----  -------------------------
    { L_,   "0"                      },  // length = 1
    { L_,   "+"                      },
    { L_,   "-"                      },
    { L_,   "T"                      },

    { L_,   "+0"                     },  // length = 2
    { L_,   "-0"                     },
    { L_,   "Z0"                     },

    { L_,   "+01"                    },  // length = 3
    { L_,   "-01"                    },

    { L_,   "+10:"                   },  // length = 4
    { L_,   "-10:"                   },
    { L_,   "+120"                   },
    { L_,   "-030"                   },

    { L_,   "+01:1"                  },  // length = 5
    { L_,   "-01:1"                  },
    { L_,   "+1:12"                  },
    { L_,   "+12:1"                  },
    { L_,   "+2360"                  },
    { L_,   "-2360"                  },
    { L_,   "+2400"                  },
    { L_,   "-2400"                  },

    { L_,   "+12:1x"                 },  // length = 6
    { L_,   "+12:1 "                 },
    { L_,   "+1200x"                 },
    { L_,   "+23:60"                 },
    { L_,   "-23:60"                 },
    { L_,   "+24:00"                 },
    { L_,   "-24:00"                 },

    { L_,   "+123:23"                },  // length = 7
    { L_,   "+12:123"                },
    { L_,   "+011:23"                },
    { L_,   "+12:011"                },

    { L_,   "+123:123"               },  // length = 8
};
const int NUM_BAD_ZONE_DATA =
                static_cast<int>(sizeof BAD_ZONE_DATA / sizeof *BAD_ZONE_DATA);

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static
Config& gg(Config *object,
           int     fractionalSecondPrecision,
           bool    omitColonInZoneDesignatorFlag,
           bool    useCommaForDecimalSignFlag,
           bool    useZAbbreviationForUtcFlag)
    // Return, by reference, the specified '*object' with its value adjusted
    // according to the specified 'omitColonInZoneDesignatorFlag',
    // 'useCommaForDecimalSignFlag', and 'useZAbbreviationForUtcFlag'.
{
    if (fractionalSecondPrecision > 6) {
        fractionalSecondPrecision = 6;
    }

    object->setFractionalSecondPrecision(fractionalSecondPrecision);
    object->setOmitColonInZoneDesignator(omitColonInZoneDesignatorFlag);
    object->setUseCommaForDecimalSign(useCommaForDecimalSignFlag);
    object->setUseZAbbreviationForUtc(useZAbbreviationForUtcFlag);

    return *object;
}

static
void updateExpectedPerConfig(bsl::string   *expected,
                             const Config&  configuration,
                             int            maxPrecision)
    // Update the specified 'expected' ISO 8601 string as if it were generated
    // using the specified 'configuration'.  The behavior is undefined unless
    // the zone designator within 'expected' (if any) is of the form
    // "(+|-)dd:dd".
{
    ASSERT(expected);

    const bsl::string::size_type index = expected->find('.');

    if (configuration.useCommaForDecimalSign()) {
        if (index != bsl::string::npos) {
            (*expected)[index] = ',';
        }
    }

    if (index != bsl::string::npos) {
        bsl::string::size_type length = 0;
        while (isdigit((*expected)[index + length + 1])) {
            ++length;
        }

        int precision = configuration.fractionalSecondPrecision();

        if (precision > maxPrecision) {
            precision = maxPrecision;
        }

        if (0 == precision) {
            expected->erase(index, length + 1);
        }
        else if (precision < static_cast<int>(length)) {
            expected->erase(index + precision + 1,
                            length - precision);
        }
    }

    // If there aren't enough characters in 'expected', don't bother with the
    // other configuration options.

    const int ZONELEN = static_cast<int>(sizeof "+dd:dd") - 1;

    if (expected->length() < ZONELEN
     || (!configuration.useZAbbreviationForUtc()
      && !configuration.omitColonInZoneDesignator())) {
        return;                                                       // RETURN
    }

    // See if the tail of 'expected' has the pattern of a zone designator.

    const bsl::string::size_type zdx = expected->length() - ZONELEN;

    if (('+' != (*expected)[zdx] && '-' != (*expected)[zdx])
      || !isdigit(static_cast<unsigned char>((*expected)[zdx + 1]))
      || !isdigit(static_cast<unsigned char>((*expected)[zdx + 2]))
      || ':' !=   (*expected)[zdx + 3]
      || !isdigit(static_cast<unsigned char>((*expected)[zdx + 4]))
      || !isdigit(static_cast<unsigned char>((*expected)[zdx + 5]))) {
        return;                                                       // RETURN
    }

    if (configuration.useZAbbreviationForUtc()) {
        const bsl::string zone = expected->substr(
                                                 expected->length() - ZONELEN);

        if (0 == zone.compare("+00:00")) {
            expected->erase(expected->length() - ZONELEN);
            expected->push_back('Z');

            return;                                                   // RETURN
        }
    }

    if (configuration.omitColonInZoneDesignator()) {
        const bsl::string::size_type index = expected->find_last_of(':');

        if (index != bsl::string::npos) {
            expected->erase(index, 1);
        }
    }
}

static
bool containsOnlyDigits(const char *string)
    // Return 'true' if the specified 'string' contains nothing but digits, and
    // 'false' otherwise.
{
    while (*string) {
        if (!isdigit(*string)) {
            return false;                                             // RETURN
        }

        ++string;
    }

    return true;
}

static
bsl::string replaceTWithSpace(const char *buffer, int length)
    // Return copy of the specified string with all 'T' & 't' characters
    // replaced with a SPACE character.
{
    bsl::string s(buffer, static_cast<size_t>(length));
    for (bsl::string::iterator it = s.begin(); it != s.end(); ++it) {
        if(*it == 'T' || *it == 't') *it = ' ';
    }
    return s;
}

//=============================================================================
//                           OUT-OF-LINE TEST CASES
//-----------------------------------------------------------------------------
// This section includes test cases that have been moved out of the 'switch'
// block in 'main' in order to alleviate compiler crash and/or performance
// issues.

void testCase17(bool verbose,
                bool veryVerbose,
                bool veryVeryVerbose,
                bool veryVeryVeryVerbose)
{

    if (verbose) cout << endl
                      << "PARSE 'DatetimeOrDatetimeTz'" << endl
                      << "============================" << endl;

    Util::DatetimeOrDatetimeTz        mX;
    const Util::DatetimeOrDatetimeTz& X = mX;

    char buffer[Util::k_MAX_STRLEN];

    const bdlt::Date       DD(246, 8, 10);
    const bdlt::Time       TT(2, 4, 6, 8);

    const bdlt::Datetime   XX(DD, TT);  // 'XX' and 'ZZ' are controls,
    const bdlt::DatetimeTz ZZ(XX, -7);  // distinct from any test data

    const int                  NUM_DATE_DATA =       NUM_DEFAULT_DATE_DATA;
    const DefaultDateDataRow (&DATE_DATA)[NUM_DATE_DATA] =
                                                         DEFAULT_DATE_DATA;

    const int                  NUM_TIME_DATA =       NUM_DEFAULT_TIME_DATA;
    const DefaultTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] =
                                                         DEFAULT_TIME_DATA;

    const int                  NUM_ZONE_DATA =       NUM_DEFAULT_ZONE_DATA;
    const DefaultZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] =
                                                         DEFAULT_ZONE_DATA;

    const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
    const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                         DEFAULT_CNFG_DATA;

    if (verbose) cout << "\nValid ISO 8601 strings." << endl;

    for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
        const int ILINE = DATE_DATA[ti].d_line;
        const int YEAR  = DATE_DATA[ti].d_year;
        const int MONTH = DATE_DATA[ti].d_month;
        const int DAY   = DATE_DATA[ti].d_day;

        const bdlt::Date DATE(YEAR, MONTH, DAY);

        for (int tj = 0; tj < NUM_TIME_DATA; ++tj) {
            const int JLINE = TIME_DATA[tj].d_line;
            const int HOUR  = TIME_DATA[tj].d_hour;
            const int MIN   = TIME_DATA[tj].d_min;
            const int SEC   = TIME_DATA[tj].d_sec;
            const int MSEC  = TIME_DATA[tj].d_msec;
            const int USEC  = TIME_DATA[tj].d_usec;

            for (int tk = 0; tk < NUM_ZONE_DATA; ++tk) {
                const int KLINE  = ZONE_DATA[tk].d_line;
                const int OFFSET = ZONE_DATA[tk].d_offset;

                if (   bdlt::Time(HOUR, MIN, SEC, MSEC, USEC)
                                                            == bdlt::Time()
                    && OFFSET != 0) {
                    continue;  // skip invalid compositions
                }

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    int expMsec = MSEC;
                    int expUsec = USEC;
                    {
                        // adjust the expected milliseconds to account for
                        // PRECISION truncating the value generated

                        int precision = (PRECISION < 3 ? PRECISION : 3);

                        for (int i = 3; i > precision; --i) {
                            expMsec /= 10;
                        }

                        for (int i = 3; i > precision; --i) {
                            expMsec *= 10;
                        }

                        // adjust the expected microseconds to account for
                        // PRECISION truncating the value generated

                        precision = (PRECISION > 3 ? PRECISION - 3: 0);

                        for (int i = 3; i > precision; --i) {
                            expUsec /= 10;
                        }

                        for (int i = 3; i > precision; --i) {
                            expUsec *= 10;
                        }
                    }

                    const bdlt::Datetime   DATETIME(YEAR,
                                                    MONTH,
                                                    DAY,
                                                    HOUR,
                                                    MIN,
                                                    SEC,
                                                    expMsec,
                                                    expUsec);
                    const bdlt::DatetimeTz DATETIMETZ(DATETIME, OFFSET);

                    if (veryVerbose) {
                        if (0 == tc) {
                            T_ P_(ILINE) P_(JLINE) P_(KLINE)
                                                P_(DATETIME) P(DATETIMETZ);
                        }
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                      P_(USECOMMA) P(USEZ);
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    // without zone designator in parsed string
                    {
                        const int LENGTH = Util::generateRaw(buffer,
                                                             DATETIME,
                                                             C);

                        if (veryVerbose) {
                            const bsl::string STRING(buffer, LENGTH);
                            T_ T_ P(STRING)
                        }

                        mX.reset();
                        ASSERTV(ILINE, JLINE, KLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX = XX;
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                XX == X.the<bdlt::Datetime>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX = ZZ;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                ZZ == X.the<bdlt::DatetimeTz>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX.reset();
                        ASSERTV(ILINE, JLINE, KLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parse(&mX,
                                                StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX = XX;
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                XX == X.the<bdlt::Datetime>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parse(&mX,
                                                StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX = ZZ;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                ZZ == X.the<bdlt::DatetimeTz>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parse(&mX,
                                                StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        // parseRelaxed() on the same string

                        mX.reset();
                        ASSERTV(ILINE, JLINE, KLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parseRelaxed(&mX,
                                                        buffer,
                                                        LENGTH));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX = XX;
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                XX == X.the<bdlt::Datetime>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parseRelaxed(&mX,
                                                        buffer,
                                                        LENGTH));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX = ZZ;
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                ZZ == X.the<bdlt::DatetimeTz>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parseRelaxed(&mX,
                                                        buffer,
                                                        LENGTH));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX.reset();
                        ASSERTV(ILINE, JLINE, KLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parseRelaxed(&mX,
                                                 StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX = XX;
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                XX == X.the<bdlt::Datetime>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parseRelaxed(&mX,
                                                 StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX = ZZ;
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                ZZ == X.the<bdlt::DatetimeTz>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parseRelaxed(&mX,
                                                 StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        // parseRelaxed() on the string modified to have
                        // SPACE instead of 'T'
                        const bsl::string relaxed =
                                         replaceTWithSpace(buffer, LENGTH);

                        mX.reset();
                        ASSERTV(ILINE, JLINE, KLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parseRelaxed(&mX,
                                                        relaxed.data(),
                                                        relaxed.length()));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX = XX;
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                XX == X.the<bdlt::Datetime>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parseRelaxed(&mX,
                                                        relaxed.data(),
                                                        relaxed.length()));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX = ZZ;
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                ZZ == X.the<bdlt::DatetimeTz>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parseRelaxed(&mX,
                                                        relaxed.data(),
                                                        relaxed.length()));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX.reset();
                        ASSERTV(ILINE, JLINE, KLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parseRelaxed(&mX,
                                                       StrView(relaxed)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX = XX;
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                XX == X.the<bdlt::Datetime>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parseRelaxed(&mX,
                                                       StrView(relaxed)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());

                        mX = ZZ;
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                ZZ == X.the<bdlt::DatetimeTz>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parseRelaxed(&mX,
                                                       StrView(relaxed)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIME == X.the<bdlt::Datetime>());
                    }

                    // with zone designator in parsed string
                    {
                        if ((DATE == bdlt::Date() && OFFSET > 0)
                         || (DATE == bdlt::Date(9999, 12, 31)
                          && OFFSET < 0)) {
                            continue;  // skip invalid compositions
                        }

                        const int LENGTH = Util::generateRaw(buffer,
                                                             DATETIMETZ,
                                                             C);

                        if (veryVerbose) {
                            const bsl::string STRING(buffer, LENGTH);
                            T_ T_ P(STRING)
                        }

                        mX.reset();
                        ASSERTV(ILINE, JLINE, KLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX = XX;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                XX == X.the<bdlt::Datetime>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX = ZZ;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                ZZ == X.the<bdlt::DatetimeTz>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX.reset();
                        ASSERTV(ILINE, JLINE, KLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parse(&mX,
                                                StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX = XX;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                XX == X.the<bdlt::Datetime>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parse(&mX,
                                                StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX = ZZ;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                ZZ == X.the<bdlt::DatetimeTz>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parse(&mX,
                                                StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        // parseRelaxed() on the same string

                        mX.reset();
                        ASSERTV(ILINE, JLINE, KLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parseRelaxed(&mX,
                                                        buffer,
                                                        LENGTH));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX = XX;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                XX == X.the<bdlt::Datetime>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parseRelaxed(&mX,
                                                        buffer,
                                                        LENGTH));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX = ZZ;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                ZZ == X.the<bdlt::DatetimeTz>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parseRelaxed(&mX,
                                                        buffer,
                                                        LENGTH));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX.reset();
                        ASSERTV(ILINE, JLINE, KLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parseRelaxed(&mX,
                                                 StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX = XX;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                XX == X.the<bdlt::Datetime>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parseRelaxed(&mX,
                                                 StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX = ZZ;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                ZZ == X.the<bdlt::DatetimeTz>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parseRelaxed(&mX,
                                                 StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        // parseRelaxed() on the string modified to have
                        // SPACE instead of 'T'
                        const bsl::string relaxed =
                                         replaceTWithSpace(buffer, LENGTH);

                        mX.reset();
                        ASSERTV(ILINE, JLINE, KLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parseRelaxed(&mX,
                                                        relaxed.data(),
                                                        relaxed.length()));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX = XX;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                XX == X.the<bdlt::Datetime>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parseRelaxed(&mX,
                                                        relaxed.data(),
                                                        relaxed.length()));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX = ZZ;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                ZZ == X.the<bdlt::DatetimeTz>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                0 == Util::parseRelaxed(&mX,
                                                        relaxed.data(),
                                                        relaxed.length()));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX.reset();
                        ASSERTV(ILINE, JLINE, KLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parseRelaxed(&mX,
                                                       StrView(relaxed)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX = XX;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::Datetime>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                XX == X.the<bdlt::Datetime>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parseRelaxed(&mX,
                                                       StrView(relaxed)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());

                        mX = ZZ;

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                ZZ == X.the<bdlt::DatetimeTz>());

                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                               0 == Util::parseRelaxed(&mX,
                                                       StrView(relaxed)));
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                X.is<bdlt::DatetimeTz>());
                        ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                DATETIMETZ == X.the<bdlt::DatetimeTz>());
                    }
                }  // loop over 'CNFG_DATA'
            }  // loop over 'ZONE_DATA'
        }  // loop over 'TIME_DATA'
    }  // loop over 'DATE_DATA'

    {
        // verify 't' and 'z' are accepted

        const bdlt::DatetimeTz EXPECTED(bdlt::Datetime(1, 2, 3, 1, 2, 3),
                                        0);

        mX.reset();
        ASSERTV(X.isUnset());

        ASSERT(0 == Util::parse(&mX, "0001-02-03t01:02:03z", 20));
        ASSERTV( X.is<bdlt::DatetimeTz>());
        ASSERTV(EXPECTED == X.the<bdlt::DatetimeTz>());
    }

    if (verbose) cout << "\nInvalid strings." << endl;
    {
        const int              NUM_DATE_DATA =         NUM_BAD_DATE_DATA;
        const BadDateDataRow (&DATE_DATA)[NUM_DATE_DATA] = BAD_DATE_DATA;

        for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
            const int LINE = DATE_DATA[ti].d_line;

            bsl::string bad(DATE_DATA[ti].d_invalid);

            // Append a valid time.

            bad.append("T12:26:52.726");

            const char *STRING = bad.c_str();
            const int   LENGTH = static_cast<int>(bad.length());

            if (veryVerbose) { T_ P_(LINE) P(STRING) }

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            // parseRelaxed() on the same string

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(&mX,
                                                          STRING,
                                                          LENGTH));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(&mX,
                                                          STRING,
                                                          LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(&mX,
                                                          STRING,
                                                          LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(STRING, LENGTH)));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            // parseRelaxed() on the string modified to have SPACE instead
            // of 'T'
            const bsl::string relaxed = replaceTWithSpace(STRING, LENGTH);

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(
                                                        &mX,
                                                        relaxed.data(),
                                                        relaxed.length()));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(
                                                        &mX,
                                                        relaxed.data(),
                                                        relaxed.length()));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(
                                                        &mX,
                                                        relaxed.data(),
                                                        relaxed.length()));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(relaxed)));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(relaxed)));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(relaxed)));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());
        }

        const int              NUM_TIME_DATA =         NUM_BAD_TIME_DATA;
        const BadTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] = BAD_TIME_DATA;

        for (int tj = 0; tj < NUM_TIME_DATA; ++tj) {
            const int LINE = TIME_DATA[tj].d_line;

            // Initialize with a *valid* date string, then append an
            // invalid time.

            bsl::string bad("2010-08-17");

            // Ensure that 'bad' is initially valid, but only during the
            // first iteration.

            if (0 == tj) {
                const char *STRING = bad.data();
                const int   LENGTH = static_cast<int>(bad.length());

                bdlt::Date mD(DD);  const bdlt::Date& D = mD;

                ASSERT( 0 == Util::parse(&mD, STRING, LENGTH));
                ASSERT(DD != D);

                mD = DD;

                ASSERT( 0 == Util::parse(&mD, StrView(STRING, LENGTH)));
                ASSERT(DD != D);
            }

            bad.append("T");
            bad.append(TIME_DATA[tj].d_invalid);

            const char *STRING = bad.c_str();
            const int   LENGTH = static_cast<int>(bad.length());

            if (veryVerbose) { T_ P_(LINE) P(STRING) }

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            // parseRelaxed() on the same string

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(&mX,
                                                          STRING,
                                                          LENGTH));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(&mX,
                                                          STRING,
                                                          LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(&mX,
                                                          STRING,
                                                          LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(STRING, LENGTH)));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            // parseRelaxed() on the string modified to have SPACE instead
            // of 'T'
            const bsl::string relaxed = replaceTWithSpace(STRING, LENGTH);

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(
                                                        &mX,
                                                        relaxed.data(),
                                                        relaxed.length()));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(
                                                        &mX,
                                                        relaxed.data(),
                                                        relaxed.length()));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(
                                                        &mX,
                                                        relaxed.data(),
                                                        relaxed.length()));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(relaxed)));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(relaxed)));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(relaxed)));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());
        }

        const int              NUM_ZONE_DATA =         NUM_BAD_ZONE_DATA;
        const BadZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] = BAD_ZONE_DATA;

        for (int tk = 0; tk < NUM_ZONE_DATA; ++tk) {
            const int LINE = ZONE_DATA[tk].d_line;

            // Initialize with a *valid* datetime string, then append an
            // invalid zone designator.

            bsl::string bad("2010-08-17T12:26:52.726");

            // Ensure that 'bad' is initially valid, but only during the
            // first iteration.

            if (0 == tk) {
                const char *STRING = bad.data();
                const int   LENGTH = static_cast<int>(bad.length());

                mX = XX;

                ASSERT( 0 == Util::parse(&mX, STRING, LENGTH));
                ASSERT(mX.is<bdlt::Datetime>());
                ASSERT(XX != mX.the<bdlt::Datetime>());

                mX = XX;

                ASSERT( 0 == Util::parse(&mX, StrView(STRING, LENGTH)));
                ASSERT(mX.is<bdlt::Datetime>());
                ASSERT(XX != mX.the<bdlt::Datetime>());

                // parseRelaxed() on the same string

                mX = XX;

                ASSERT( 0 == Util::parseRelaxed(&mX, STRING, LENGTH));
                ASSERT(mX.is<bdlt::Datetime>());
                ASSERT(XX != mX.the<bdlt::Datetime>());

                mX = XX;

                ASSERT( 0 == Util::parseRelaxed(&mX,
                                                StrView(STRING, LENGTH)));
                ASSERT(mX.is<bdlt::Datetime>());
                ASSERT(XX != mX.the<bdlt::Datetime>());

                // parseRelaxed() on the string modified to have SPACE
                // instead of 'T'
                const bsl::string relaxed =
                                         replaceTWithSpace(STRING, LENGTH);

                mX = XX;

                ASSERT( 0 == Util::parseRelaxed(&mX,
                                                relaxed.data(),
                                                relaxed.length()));
                ASSERT(mX.is<bdlt::Datetime>());
                ASSERT(XX != mX.the<bdlt::Datetime>());

                mX = XX;

                ASSERT( 0 == Util::parseRelaxed(&mX, StrView(relaxed)));
                ASSERT(mX.is<bdlt::Datetime>());
                ASSERT(XX != mX.the<bdlt::Datetime>());
            }

            // If 'ZONE_DATA[tk].d_invalid' contains nothing but digits,
            // appending it to 'bad' simply extends the fractional second
            // (so 'bad' remains valid).

            if (containsOnlyDigits(ZONE_DATA[tk].d_invalid)) {
                continue;
            }

            bad.append(ZONE_DATA[tk].d_invalid);

            const char *STRING = bad.c_str();
            const int   LENGTH = static_cast<int>(bad.length());

            if (veryVerbose) { T_ P_(LINE) P(STRING) }

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            // parseRelaxed() on the same string

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(&mX,
                                                          STRING,
                                                          LENGTH));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(&mX,
                                                          STRING,
                                                          LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(&mX,
                                                          STRING,
                                                          LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(STRING, LENGTH)));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            // parseRelaxed() on the string modified to have SPACE instead
            // of 'T'
            const bsl::string relaxed = replaceTWithSpace(STRING, LENGTH);

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(
                                                        &mX,
                                                        relaxed.data(),
                                                        relaxed.length()));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(
                                                        &mX,
                                                        relaxed.data(),
                                                        relaxed.length()));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING, 0 != Util::parseRelaxed(
                                                        &mX,
                                                        relaxed.data(),
                                                        relaxed.length()));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(relaxed)));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(relaxed)));

            ASSERTV(LINE, STRING, X.is<bdlt::Datetime>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Datetime>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());

            ASSERTV(LINE, STRING,
                    0 != Util::parseRelaxed(&mX, StrView(relaxed)));

            ASSERTV(LINE, STRING, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DatetimeTz>());
        }
    }

    if (verbose) cout << "\nTesting leap seconds and fractional seconds."
                      << endl;
    {
        const struct {
            int         d_line;           // source line number

            const char *d_input;          // input

            int         d_year;           // year under test

            int         d_month;          // month under test

            int         d_day;            // day under test

            int         d_hour;           // hour under test

            int         d_min;            // minutes under test

            int         d_sec;            // seconds under test

            int         d_msec;           // milli seconds under test

            int         d_usec;           // micro seconds under test

            int         d_offset;         // UTC offset

            bool        d_isDatetimeTz;   // flag indicating whether the
                                          // result object is expected to
                                          // contain 'DatetimeTz' or
                                          // 'Datetime' object

        } DATA[] = {
            // leap seconds
            { L_, "0001-01-01T00:00:60.000",
                          0001, 01, 01, 00, 01, 00, 000, 000,   0, false },
            { L_, "9998-12-31T23:59:60.999",
                          9999, 01, 01, 00, 00, 00, 999, 000,   0, false },

            // fractional seconds
            { L_, "0001-01-01T00:00:00.0000001",
                          0001, 01, 01, 00, 00, 00, 000, 000,   0, false },
            { L_, "0001-01-01T00:00:00.0000009",
                          0001, 01, 01, 00, 00, 00, 000,   1,   0, false },
            { L_, "0001-01-01T00:00:00.00000001",
                          0001, 01, 01, 00, 00, 00, 000, 000,   0, false },
            { L_, "0001-01-01T00:00:00.00000049",
                          0001, 01, 01, 00, 00, 00, 000, 000,   0, false },
            { L_, "0001-01-01T00:00:00.00000050",
                          0001, 01, 01, 00, 00, 00, 000,   1,   0, false },
            { L_, "0001-01-01T00:00:00.00000099",
                          0001, 01, 01, 00, 00, 00, 000,   1,   0, false },
            { L_, "0001-01-01T00:00:00.0001",
                          0001, 01, 01, 00, 00, 00, 000, 100,   0, false },
            { L_, "0001-01-01T00:00:00.0009",
                          0001, 01, 01, 00, 00, 00, 000, 900,   0, false },
            { L_, "0001-01-01T00:00:00.00001",
                          0001, 01, 01, 00, 00, 00, 000,  10,   0, false },
            { L_, "0001-01-01T00:00:00.00049",
                          0001, 01, 01, 00, 00, 00, 000, 490,   0, false },
            { L_, "0001-01-01T00:00:00.00050",
                          0001, 01, 01, 00, 00, 00, 000, 500,   0, false },
            { L_, "0001-01-01T00:00:00.00099",
                          0001, 01, 01, 00, 00, 00, 000, 990,   0, false },
            { L_, "0001-01-01T00:00:00.9994" ,
                          0001, 01, 01, 00, 00, 00, 999, 400,   0, false },
            { L_, "0001-01-01T00:00:00.9995" ,
                          0001, 01, 01, 00, 00, 00, 999, 500,   0, false },
            { L_, "0001-01-01T00:00:00.9999" ,
                          0001, 01, 01, 00, 00, 00, 999, 900,   0, false },
            { L_, "9998-12-31T23:59:60.9999" ,
                          9999, 01, 01, 00, 00, 00, 999, 900,   0, false },
            { L_, "0001-01-01T00:00:00.9999994" ,
                          0001, 01, 01, 00, 00, 00, 999, 999,   0, false },
            { L_, "0001-01-01T00:00:00.9999995" ,
                          0001, 01, 01, 00, 00, 01, 000, 000,   0, false },
            { L_, "0001-01-01T00:00:00.9999999" ,
                          0001, 01, 01, 00, 00, 01, 000, 000,   0, false },
            { L_, "9998-12-31T23:59:60.9999999" ,
                          9999, 01, 01, 00, 00, 01, 000, 000,   0, false },

            // omit fractional seconds
            { L_, "2014-12-23T12:34:45",
                          2014, 12, 23, 12, 34, 45, 000, 000,   0, false },
            { L_, "2014-12-23T12:34:45Z",
                          2014, 12, 23, 12, 34, 45, 000, 000,   0,  true },
            { L_, "2014-12-23T12:34:45+00:30",
                          2014, 12, 23, 12, 34, 45, 000, 000,  30,  true },
            { L_, "2014-12-23T12:34:45-01:30",
                          2014, 12, 23, 12, 34, 45, 000, 000, -90,  true },

            { L_, "2014-12-23T12:34:45.9999994Z",
                          2014, 12, 23, 12, 34, 45, 999, 999,   0,  true },
            { L_, "2014-12-23T12:34:45.9999994+00:30",
                          2014, 12, 23, 12, 34, 45, 999, 999,  30,  true },
            { L_, "2014-12-23T12:34:45.9999994-01:30",
                          2014, 12, 23, 12, 34, 45, 999, 999, -90,  true },
            { L_, "2014-12-23T12:34:45.9999995Z",
                          2014, 12, 23, 12, 34, 46, 000, 000,   0,  true },
            { L_, "2014-12-23T12:34:45.9999995+00:30",
                          2014, 12, 23, 12, 34, 46, 000, 000,  30,  true },
            { L_, "2014-12-23T12:34:45.9999995-01:30",
                          2014, 12, 23, 12, 34, 46, 000, 000, -90,  true },
            { L_, "0001-01-01T00:00:00.9999999Z",
                          0001, 01, 01, 00, 00, 01, 000, 000,   0,  true },
            { L_, "0001-01-01T00:00:00.9999999+00:30",
                          0001, 01, 01, 00, 00, 01, 000, 000,  30,  true },
            { L_, "0001-01-01T00:00:00.9999999-01:30",
                          0001, 01, 01, 00, 00, 01, 000, 000, -90,  true },
            { L_, "2014-12-23T12:34:60.9999994+00:30",
                          2014, 12, 23, 12, 35, 00, 999, 999,  30,  true },
            { L_, "2014-12-23T12:34:60.9999994-01:30",
                          2014, 12, 23, 12, 35, 00, 999, 999, -90,  true },
            { L_, "9998-12-31T23:59:60.9999999+00:30",
                          9999, 01, 01, 00, 00, 01, 000, 000,  30,  true },
            { L_, "9998-12-31T23:59:60.9999999-01:30",
                          9999, 01, 01, 00, 00, 01, 000, 000, -90,  true },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE          = DATA[ti].d_line;
            const char *INPUT         = DATA[ti].d_input;
            const int   LENGTH        =
                                      static_cast<int>(bsl::strlen(INPUT));
            const int   YEAR          = DATA[ti].d_year;
            const int   MONTH         = DATA[ti].d_month;
            const int   DAY           = DATA[ti].d_day;
            const int   HOUR          = DATA[ti].d_hour;
            const int   MIN           = DATA[ti].d_min;
            const int   SEC           = DATA[ti].d_sec;
            const int   MSEC          = DATA[ti].d_msec;
            const int   USEC          = DATA[ti].d_usec;
            const int   OFFSET        = DATA[ti].d_offset;
            const bool  IS_DATETIMETZ = DATA[ti].d_isDatetimeTz;

            if (veryVerbose) { T_ P_(LINE) P(INPUT) }

            bdlt::Datetime   EXPECTED_DT(YEAR,
                                         MONTH,
                                         DAY,
                                         HOUR,
                                         MIN,
                                         SEC,
                                         MSEC,
                                         USEC);
            bdlt::DatetimeTz EXPECTED_DTTZ(EXPECTED_DT, OFFSET);


            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(LINE, INPUT, LENGTH,
                    0 == Util::parse(&mX, INPUT, LENGTH));

            if (IS_DATETIMETZ) {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::DatetimeTz>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_DTTZ == X.the<bdlt::DatetimeTz>());
            }
            else {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::Datetime>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_DT == X.the<bdlt::Datetime>());
            }

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(LINE, INPUT, LENGTH,
                    0 == Util::parse(&mX, StrView(INPUT, LENGTH)));

            if (IS_DATETIMETZ) {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::DatetimeTz>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_DTTZ == X.the<bdlt::DatetimeTz>());
            }
            else {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::Datetime>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_DT == X.the<bdlt::Datetime>());
            }

            // parseRelaxed() on the same string

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(LINE, INPUT, LENGTH,
                    0 == Util::parseRelaxed(&mX, INPUT, LENGTH));

            if (IS_DATETIMETZ) {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::DatetimeTz>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_DTTZ == X.the<bdlt::DatetimeTz>());
            }
            else {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::Datetime>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_DT == X.the<bdlt::Datetime>());
            }

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(LINE, INPUT, LENGTH,
                    0 == Util::parseRelaxed(&mX, StrView(INPUT, LENGTH)));

            if (IS_DATETIMETZ) {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::DatetimeTz>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_DTTZ == X.the<bdlt::DatetimeTz>());
            }
            else {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::Datetime>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_DT == X.the<bdlt::Datetime>());
            }

            // parseRelaxed() on the string modified to have SPACE instead
            // of 'T'
            const bsl::string relaxed = replaceTWithSpace(INPUT, LENGTH);

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(LINE, INPUT, LENGTH,
                    0 == Util::parseRelaxed(&mX,
                                            relaxed.data(),
                                            relaxed.length()));

            if (IS_DATETIMETZ) {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::DatetimeTz>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_DTTZ == X.the<bdlt::DatetimeTz>());
            }
            else {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::Datetime>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_DT == X.the<bdlt::Datetime>());
            }

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(LINE, INPUT, LENGTH,
                    0 == Util::parseRelaxed(&mX, StrView(relaxed)));

            if (IS_DATETIMETZ) {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::DatetimeTz>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_DTTZ == X.the<bdlt::DatetimeTz>());
            }
            else {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::Datetime>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_DT == X.the<bdlt::Datetime>());
            }
        }
    }

    if (verbose)
        cout << "\nTesting zone designators that overflow a 'Datetime'."
             << endl;
    {
        struct {
            int         d_line;
            const char *d_input;
            int         d_year;
            int         d_month;
            int         d_day;
            int         d_hour;
            int         d_min;
            int         d_sec;
            int         d_msec;
            int         d_offset;
        } DATA[] = {
            { L_, "0001-01-01T00:00:00.000+00:00",
                                    0001, 01, 01, 00, 00, 00, 000,     0 },
            { L_, "0001-01-01T00:00:00.000+00:01",
                                    0001, 01, 01, 00, 00, 00, 000,     1 },
            { L_, "0001-01-01T23:58:59.000+23:59",
                                    0001, 01, 01, 23, 58, 59, 000,  1439 },

            { L_, "9999-12-31T23:59:59.999+00:00",
                                    9999, 12, 31, 23, 59, 59, 999,     0 },
            { L_, "9999-12-31T23:59:59.999-00:01",
                                    9999, 12, 31, 23, 59, 59, 999,    -1 },
            { L_, "9999-12-31T00:01:00.000-23:59",
                                    9999, 12, 31, 00, 01, 00, 000, -1439 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE   = DATA[ti].d_line;
            const char *INPUT  = DATA[ti].d_input;
            const int   LENGTH = static_cast<int>(bsl::strlen(INPUT));
            const int   YEAR   = DATA[ti].d_year;
            const int   MONTH  = DATA[ti].d_month;
            const int   DAY    = DATA[ti].d_day;
            const int   HOUR   = DATA[ti].d_hour;
            const int   MIN    = DATA[ti].d_min;
            const int   SEC    = DATA[ti].d_sec;
            const int   MSEC   = DATA[ti].d_msec;
            const int   OFFSET = DATA[ti].d_offset;

            if (veryVerbose) { T_ P_(LINE) P(INPUT) }

            bdlt::DatetimeTz EXPECTED(bdlt::Datetime(YEAR, MONTH, DAY,
                                                     HOUR, MIN, SEC, MSEC),
                                      OFFSET);

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(
                LINE, INPUT, LENGTH, 0 == Util::parse(&mX, INPUT, LENGTH));

            ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, INPUT, LENGTH,
                    EXPECTED == X.the<bdlt::DatetimeTz>());

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(LINE, INPUT, LENGTH,
                    0 == Util::parse(&mX, StrView(INPUT, LENGTH)));

            ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, INPUT, LENGTH,
                    EXPECTED == X.the<bdlt::DatetimeTz>());

            // parseRelaxed() on the same string

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(
                LINE, INPUT, LENGTH, 0 == Util::parseRelaxed(&mX,
                                                             INPUT,
                                                             LENGTH));

            ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, INPUT, LENGTH,
                    EXPECTED == X.the<bdlt::DatetimeTz>());

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(LINE, INPUT, LENGTH,
                    0 == Util::parseRelaxed(&mX, StrView(INPUT, LENGTH)));

            ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, INPUT, LENGTH,
                    EXPECTED == X.the<bdlt::DatetimeTz>());

            // parseRelaxed() on the string modified to have SPACE instead
            // of 'T'
            const bsl::string relaxed = replaceTWithSpace(INPUT, LENGTH);

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(
                LINE, INPUT, LENGTH, 0 == Util::parseRelaxed(
                                                        &mX,
                                                        relaxed.data(),
                                                        relaxed.length()));

            ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, INPUT, LENGTH,
                    EXPECTED == X.the<bdlt::DatetimeTz>());

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(LINE, INPUT, LENGTH,
                    0 == Util::parseRelaxed(&mX, StrView(relaxed)));

            ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::DatetimeTz>());
            ASSERTV(LINE, INPUT, LENGTH,
                    EXPECTED == X.the<bdlt::DatetimeTz>());
        }
    }

    if (verbose) cout << "\nNegative Testing." << endl;
    {
        bsls::AssertTestHandlerGuard hG;

        const char    *INPUT  = "2013-10-23T01:23:45";
        const int      LENGTH = static_cast<int>(bsl::strlen(INPUT));
        const StrView  STRING_REF(INPUT, LENGTH);

        bdlt::Datetime   result;
        bdlt::DatetimeTz resultTz;

        if (veryVerbose) cout << "\t'Invalid result'" << endl;
        {
            Util::TimeOrTimeTz *NULL_PTR = 0;

            ASSERT_PASS(Util::parse(      &mX, INPUT, LENGTH));
            ASSERT_FAIL(Util::parse( NULL_PTR, INPUT, LENGTH));

            ASSERT_PASS(Util::parse(     &mX, STRING_REF));
            ASSERT_FAIL(Util::parse(NULL_PTR, STRING_REF));
        }

        if (veryVerbose) cout << "\t'Invalid input'" << endl;
        {
            const char    *NULL_PTR = 0;
            const StrView  NULL_REF;

            ASSERT_PASS(Util::parse(&mX,    INPUT, LENGTH));
            ASSERT_FAIL(Util::parse(&mX, NULL_PTR, LENGTH));

            ASSERT_PASS(Util::parse(&mX, STRING_REF));
            ASSERT_FAIL(Util::parse(&mX,   NULL_REF));

            ASSERT_PASS(Util::parseRelaxed(&mX,    INPUT, LENGTH));
            ASSERT_FAIL(Util::parseRelaxed(&mX, NULL_PTR, LENGTH));

            ASSERT_PASS(Util::parseRelaxed(&mX, STRING_REF));
            ASSERT_FAIL(Util::parseRelaxed(&mX,   NULL_REF));
        }

        if (veryVerbose) cout << "\t'Invalid length'" << endl;
        {
            ASSERT_PASS(Util::parse(&mX, INPUT, LENGTH));
            ASSERT_PASS(Util::parse(&mX, INPUT,      0));
            ASSERT_FAIL(Util::parse(&mX, INPUT,     -1));

            ASSERT_PASS(Util::parseRelaxed(&mX, INPUT, LENGTH));
            ASSERT_PASS(Util::parseRelaxed(&mX, INPUT,      0));
            ASSERT_FAIL(Util::parseRelaxed(&mX, INPUT,     -1));
        }
    }
}

void testCase16(bool verbose,
                bool veryVerbose,
                bool veryVeryVerbose,
                bool veryVeryVeryVerbose)
{

    if (verbose) cout << endl
                      << "PARSE 'TimeOrTimeTz'" << endl
                      << "====================" << endl;

    Util::TimeOrTimeTz        mX;
    const Util::TimeOrTimeTz& X = mX;

    char buffer[Util::k_MAX_STRLEN];

    const bdlt::Time   XX(2, 4, 6, 8);  // 'XX' and 'ZZ' are controls,
    const bdlt::TimeTz ZZ(XX, -7);      // distinct from any test data

    const int                  NUM_TIME_DATA =       NUM_DEFAULT_TIME_DATA;
    const DefaultTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] =
                                                         DEFAULT_TIME_DATA;

    const int                  NUM_ZONE_DATA =       NUM_DEFAULT_ZONE_DATA;
    const DefaultZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] =
                                                         DEFAULT_ZONE_DATA;

    const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
    const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                         DEFAULT_CNFG_DATA;

    if (verbose) cout << "\nValid ISO 8601 strings." << endl;

    for (int ti = 0; ti < NUM_TIME_DATA; ++ti) {
        const int ILINE = TIME_DATA[ti].d_line;
        const int HOUR  = TIME_DATA[ti].d_hour;
        const int MIN   = TIME_DATA[ti].d_min;
        const int SEC   = TIME_DATA[ti].d_sec;
        const int MSEC  = TIME_DATA[ti].d_msec;
        const int USEC  = TIME_DATA[ti].d_usec;

        for (int tj = 0; tj < NUM_ZONE_DATA; ++tj) {
            const int JLINE  = ZONE_DATA[tj].d_line;
            const int OFFSET = ZONE_DATA[tj].d_offset;

            if (   bdlt::Time(HOUR, MIN, SEC, MSEC, USEC) == bdlt::Time()
                && OFFSET != 0) {
                continue;  // skip invalid compositions
            }

            for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                const int  CLINE     = CNFG_DATA[tc].d_line;
                const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                const int  PRECISION = CNFG_DATA[tc].d_precision;
                const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                const bool USEZ      = CNFG_DATA[tc].d_useZ;

                int expMsec = MSEC;
                int expUsec = USEC;
                {
                    // adjust the expected milliseconds to account for
                    // PRECISION truncating the value generated

                    int precision = (PRECISION < 3 ? PRECISION : 3);

                    for (int i = 3; i > precision; --i) {
                        expMsec /= 10;
                    }

                    for (int i = 3; i > precision; --i) {
                        expMsec *= 10;
                    }

                    // adjust the expected microseconds to account for
                    // PRECISION truncating the value generated

                    precision = (PRECISION > 3 ? PRECISION - 3: 0);

                    for (int i = 3; i > precision; --i) {
                        expUsec /= 10;
                    }

                    for (int i = 3; i > precision; --i) {
                        expUsec *= 10;
                    }
                }

                const bdlt::Time TIME(HOUR, MIN, SEC, expMsec, expUsec);

                const bdlt::TimeTz TIMETZ(TIME, OFFSET);

                if (veryVerbose) {
                    if (0 == tc) {
                        T_ P_(ILINE) P_(JLINE) P_(TIME) P(TIMETZ);
                    }
                    T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                      P_(USECOMMA) P(USEZ);
                }

                Config mC;  const Config& C = mC;
                gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                // without zone designator in parsed string
                {
                    const int LENGTH = Util::generateRaw(buffer, TIME, C);

                    if (veryVerbose) {
                        const bsl::string STRING(buffer, LENGTH);
                        T_ T_ P(STRING)
                    }

                    mX.reset();
                    ASSERTV(ILINE, JLINE, CLINE, X.isUnset());

                    ASSERTV(ILINE, JLINE, CLINE,
                            0 == Util::parse(&mX, buffer, LENGTH));
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Time>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            TIME == X.the<bdlt::Time>());

                    mX = XX;
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Time>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            XX == X.the<bdlt::Time>());

                    ASSERTV(ILINE, JLINE, CLINE,
                            0 == Util::parse(&mX, buffer, LENGTH));
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Time>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            TIME == X.the<bdlt::Time>());

                    mX = ZZ;

                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::TimeTz>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            ZZ == X.the<bdlt::TimeTz>());

                    ASSERTV(ILINE, JLINE, CLINE,
                            0 == Util::parse(&mX, buffer, LENGTH));
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Time>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            TIME == X.the<bdlt::Time>());

                    mX.reset();
                    ASSERTV(ILINE, JLINE, CLINE, X.isUnset());

                    ASSERTV(ILINE, JLINE, CLINE,
                           0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Time>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            TIME == X.the<bdlt::Time>());

                    mX = XX;
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Time>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            XX == X.the<bdlt::Time>());

                    ASSERTV(ILINE, JLINE, CLINE,
                           0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Time>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            TIME == X.the<bdlt::Time>());

                    mX = ZZ;

                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::TimeTz>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            ZZ == X.the<bdlt::TimeTz>());

                    ASSERTV(ILINE, JLINE, CLINE,
                           0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Time>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            TIME == X.the<bdlt::Time>());
                }

                // with zone designator in parsed string
                {
                    const int LENGTH = Util::generateRaw(buffer,
                                                         TIMETZ,
                                                         C);

                    if (veryVerbose) {
                        const bsl::string STRING(buffer, LENGTH);
                        T_ T_ P(STRING)
                    }

                    mX.reset();
                    ASSERTV(ILINE, JLINE, CLINE, X.isUnset());

                    ASSERTV(ILINE, JLINE, CLINE,
                            0 == Util::parse(&mX, buffer, LENGTH));
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::TimeTz>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            TIMETZ == X.the<bdlt::TimeTz>());

                    mX = XX;

                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Time>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            XX == X.the<bdlt::Time>());

                    ASSERTV(ILINE, JLINE, CLINE,
                            0 == Util::parse(&mX, buffer, LENGTH));
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::TimeTz>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            TIMETZ == X.the<bdlt::TimeTz>());

                    mX = ZZ;

                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::TimeTz>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            ZZ == X.the<bdlt::TimeTz>());

                    ASSERTV(ILINE, JLINE, CLINE,
                            0 == Util::parse(&mX, buffer, LENGTH));
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::TimeTz>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            TIMETZ == X.the<bdlt::TimeTz>());

                    mX.reset();
                    ASSERTV(ILINE, JLINE, CLINE, X.isUnset());

                    ASSERTV(ILINE, JLINE, CLINE,
                           0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::TimeTz>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            TIMETZ == X.the<bdlt::TimeTz>());

                    mX = XX;

                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Time>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            XX == X.the<bdlt::Time>());

                    ASSERTV(ILINE, JLINE, CLINE,
                           0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::TimeTz>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            TIMETZ == X.the<bdlt::TimeTz>());

                    mX = ZZ;

                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::TimeTz>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            ZZ == X.the<bdlt::TimeTz>());

                    ASSERTV(ILINE, JLINE, CLINE,
                           0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                    ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::TimeTz>());
                    ASSERTV(ILINE, JLINE, CLINE,
                            TIMETZ == X.the<bdlt::TimeTz>());
                }
            }  // loop over 'CNFG_DATA'
        }  // loop over 'ZONE_DATA'
    }  // loop over 'TIME_DATA'

    {
        // verify 'z' is accepted
        const bdlt::TimeTz EXPECTED(bdlt::Time(1,2,3) , 0);

        mX.reset();
        ASSERTV(X.isUnset());

        ASSERTV(0 == Util::parse(&mX, "01:02:03z", 9));
        ASSERTV( X.is<bdlt::TimeTz>());
        ASSERTV(EXPECTED == X.the<bdlt::TimeTz>());

        mX = XX;

        ASSERTV(X.is<bdlt::Time>());
        ASSERTV(XX == X.the<bdlt::Time>());

        ASSERTV(0 == Util::parse(&mX, "01:02:03z", 9));
        ASSERTV(X.is<bdlt::TimeTz>());
        ASSERTV(EXPECTED == X.the<bdlt::TimeTz>());

        mX = ZZ;

        ASSERT(X.is<bdlt::TimeTz>());
        ASSERT(ZZ == X.the<bdlt::TimeTz>());

        ASSERT(0 == Util::parse(&mX, "01:02:03z", 9));
        ASSERT(X.is<bdlt::TimeTz>());
        ASSERT(EXPECTED == X.the<bdlt::TimeTz>());
    }

    if (verbose) cout << "\nInvalid strings." << endl;
    {
        const int              NUM_TIME_DATA =         NUM_BAD_TIME_DATA;
        const BadTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] = BAD_TIME_DATA;

        for (int ti = 0; ti < NUM_TIME_DATA; ++ti) {
            const int   LINE   = TIME_DATA[ti].d_line;
            const char *STRING = TIME_DATA[ti].d_invalid;

            if (veryVerbose) { T_ P_(LINE) P(STRING) }

            const int LENGTH = static_cast<int>(bsl::strlen(STRING));

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Time>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Time>());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::Time>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Time>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::TimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::TimeTz>());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::TimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::TimeTz>());

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Time>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Time>());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::Time>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Time>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::TimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::TimeTz>());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::TimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::TimeTz>());
        }

        const int              NUM_ZONE_DATA =         NUM_BAD_ZONE_DATA;
        const BadZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] = BAD_ZONE_DATA;

        for (int ti = 0; ti < NUM_ZONE_DATA; ++ti) {
            const int LINE = ZONE_DATA[ti].d_line;

            // Initialize with a *valid* time string, then append an
            // invalid zone designator.

            bsl::string bad("12:26:52.726");

            // Ensure that 'bad' is initially valid, but only during the
            // first iteration.

            if (0 == ti) {
                const char *STRING = bad.data();
                const int   LENGTH = static_cast<int>(bad.length());

                mX = XX;

                ASSERT( 0 == Util::parse(&mX, STRING, LENGTH));
                ASSERT(mX.is<bdlt::Time>());
                ASSERT(XX != mX.the<bdlt::Time>());

                mX = XX;

                ASSERT( 0 == Util::parse(&mX, StrView(STRING, LENGTH)));
                ASSERT(mX.is<bdlt::Time>());
                ASSERT(XX != mX.the<bdlt::Time>());
            }

            // If 'ZONE_DATA[ti].d_invalid' contains nothing but digits,
            // appending it to 'bad' simply extends the fractional second
            // (so 'bad' remains valid).

            if (containsOnlyDigits(ZONE_DATA[ti].d_invalid)) {
                continue;
            }

            bad.append(ZONE_DATA[ti].d_invalid);

            const char *STRING = bad.c_str();
            const int   LENGTH = static_cast<int>(bad.length());

            if (veryVerbose) { T_ P_(LINE) P(STRING) }

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Time>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Time>());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::Time>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Time>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::TimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::TimeTz>());

            ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

            ASSERTV(LINE, STRING, X.is<bdlt::TimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::TimeTz>());

            mX.reset();
            ASSERTV(LINE, STRING, X.isUnset());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));
            ASSERTV(LINE, STRING, X.isUnset());

            mX = XX;
            ASSERTV(LINE, STRING, X.is<bdlt::Time>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Time>());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::Time>());
            ASSERTV(LINE, STRING, XX == X.the<bdlt::Time>());

            mX = ZZ;
            ASSERTV(LINE, STRING, X.is<bdlt::TimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::TimeTz>());

            ASSERTV(LINE, STRING,
                    0 != Util::parse(&mX, StrView(STRING, LENGTH)));

            ASSERTV(LINE, STRING, X.is<bdlt::TimeTz>());
            ASSERTV(LINE, STRING, ZZ == X.the<bdlt::TimeTz>());
        }
    }

    if (verbose) cout << "\nTesting leap seconds and fractional seconds."
                      << endl;
    {
        const struct {
            int         d_line;      // source line number

            const char *d_input;     // input

            int         d_hour;      // hour under test

            int         d_min;       // minutes under test

            int         d_sec;       // seconds under test

            int         d_msec;      // milli seconds under test

            int         d_usec;      // micro seconds under test

            int         d_offset;    // UTC offset

            bool        d_isTimeTz;  // flag indicating whether the result
                                     // object is expected to contain
                                     // 'TimeTz' or 'Time'  object
        } DATA[] = {
        // leap seconds
        //LINE INPUT                    H   M   S   MS   US   OFF  DATETZ
        //---- ------------------------ --  --  --  ---  ---  ---  ------
        { L_,  "00:00:60.000",          00, 01, 00, 000, 000,   0, false },
        { L_,  "22:59:60.999",          23, 00, 00, 999, 000,   0, false },
        { L_,  "23:59:60.999",          00, 00, 00, 999, 000,   0, false },

        // fractional seconds
        { L_,  "00:00:00.0001",         00, 00, 00, 000, 100,   0, false },
        { L_,  "00:00:00.0009",         00, 00, 00, 000, 900,   0, false },
        { L_,  "00:00:00.00001",        00, 00, 00, 000,  10,   0, false },
        { L_,  "00:00:00.00049",        00, 00, 00, 000, 490,   0, false },
        { L_,  "00:00:00.00050",        00, 00, 00, 000, 500,   0, false },
        { L_,  "00:00:00.00099",        00, 00, 00, 000, 990,   0, false },
        { L_,  "00:00:00.0000001",      00, 00, 00, 000, 000,   0, false },
        { L_,  "00:00:00.0000009",      00, 00, 00, 000, 001,   0, false },
        { L_,  "00:00:00.00000001",     00, 00, 00, 000, 000,   0, false },
        { L_,  "00:00:00.00000049",     00, 00, 00, 000, 000,   0, false },
        { L_,  "00:00:00.00000050",     00, 00, 00, 000, 001,   0, false },
        { L_,  "00:00:00.00000099",     00, 00, 00, 000, 001,   0, false },
        { L_,  "00:00:00.9994",         00, 00, 00, 999, 400,   0, false },
        { L_,  "00:00:00.9995",         00, 00, 00, 999, 500,   0, false },
        { L_,  "00:00:00.9999",         00, 00, 00, 999, 900,   0, false },
        { L_,  "00:00:59.9999",         00, 00, 59, 999, 900,   0, false },
        { L_,  "23:59:59.9999",         23, 59, 59, 999, 900,   0, false },
        { L_,  "00:00:00.9999994",      00, 00, 00, 999, 999,   0, false },
        { L_,  "00:00:00.9999995",      00, 00,  1, 000, 000,   0, false },
        { L_,  "00:00:00.9999999",      00, 00,  1, 000, 000,   0, false },
        { L_,  "00:00:59.9999999",      00,  1, 00, 000, 000,   0, false },
        { L_,  "23:59:59.9999999",      00, 00, 00, 000, 000,   0, false },

        // omit fractional seconds
        { L_,  "12:34:45",              12, 34, 45, 000, 000,   0, false },
        { L_,  "12:34:45Z",             12, 34, 45, 000, 000,   0,  true },
        { L_,  "12:34:45+00:30",        12, 34, 45, 000, 000,  30,  true },
        { L_,  "00:00:00+00:30",        00, 00, 00, 000, 000,  30,  true },
        { L_,  "12:34:45-01:30",        12, 34, 45, 000, 000, -90,  true },
        { L_,  "23:59:59-01:30",        23, 59, 59, 000, 000, -90,  true },

        { L_, "12:34:45.9999994Z",      12, 34, 45, 999, 999,   0,  true },
        { L_, "12:34:45.9999994+00:30", 12, 34, 45, 999, 999,  30,  true },
        { L_, "12:34:45.9999994-01:30", 12, 34, 45, 999, 999, -90,  true },
        { L_, "12:34:45.9999995Z",      12, 34, 46, 000, 000,   0,  true },
        { L_, "12:34:45.9999995+00:30", 12, 34, 46, 000, 000,  30,  true },
        { L_, "12:34:45.9999995-01:30", 12, 34, 46, 000, 000, -90,  true },
        { L_, "00:00:00.9999999Z",      00, 00, 01, 000, 000,   0,  true },
        { L_, "00:00:00.9999999+00:30", 00, 00, 01, 000, 000,  30,  true },
        { L_, "00:00:00.9999999-01:30", 00, 00, 01, 000, 000, -90,  true },
        { L_, "12:34:60.9999994+00:30", 12, 35, 00, 999, 999,  30,  true },
        { L_, "12:34:60.9999994-01:30", 12, 35, 00, 999, 999, -90,  true },
        { L_, "23:59:60.9999999+00:30", 00, 00, 01, 000, 000,  30,  true },
        { L_, "23:59:60.9999999-01:30", 00, 00, 01, 000, 000, -90,  true },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE      = DATA[ti].d_line;
            const char *INPUT     = DATA[ti].d_input;
            const int   LENGTH    = static_cast<int>(bsl::strlen(INPUT));
            const int   HOUR      = DATA[ti].d_hour;
            const int   MIN       = DATA[ti].d_min;
            const int   SEC       = DATA[ti].d_sec;
            const int   MSEC      = DATA[ti].d_msec;
            const int   USEC      = DATA[ti].d_usec;
            const int   OFFSET    = DATA[ti].d_offset;
            const bool  IS_TIMETZ = DATA[ti].d_isTimeTz;

            if (veryVerbose) { T_ P_(LINE) P(INPUT) }

            const bdlt::Time   EXPECTED_TIME(HOUR, MIN, SEC, MSEC, USEC);
            const bdlt::TimeTz EXPECTED_TIMETZ(EXPECTED_TIME, OFFSET);

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(LINE, INPUT, LENGTH,
                    0 == Util::parse(&mX, INPUT, LENGTH));

            if (IS_TIMETZ) {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::TimeTz>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_TIMETZ == X.the<bdlt::TimeTz>());
            }
            else {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::Time>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_TIME == X.the<bdlt::Time>());
            }

            mX.reset();
            ASSERTV(LINE, INPUT, LENGTH, X.isUnset());

            ASSERTV(LINE, INPUT, LENGTH,
                    0 == Util::parse(&mX, StrView(INPUT, LENGTH)));

            if (IS_TIMETZ) {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::TimeTz>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_TIMETZ == X.the<bdlt::TimeTz>());
            }
            else {
                ASSERTV(LINE, INPUT, LENGTH, X.is<bdlt::Time>());
                ASSERTV(LINE, INPUT, LENGTH,
                        EXPECTED_TIME == X.the<bdlt::Time>());
            }
        }
    }

    if (verbose) cout << "\nNegative Testing." << endl;
    {
        bsls::AssertTestHandlerGuard hG;

        const char    *INPUT  = "01:23:45";
        const int      LENGTH = static_cast<int>(bsl::strlen(INPUT));
        const StrView  STRING_REF(INPUT, LENGTH);

        if (veryVerbose) cout << "\t'Invalid result'" << endl;
        {
            Util::TimeOrTimeTz *NULL_PTR = 0;

            ASSERT_PASS(Util::parse(     &mX, INPUT, LENGTH));
            ASSERT_FAIL(Util::parse(NULL_PTR, INPUT, LENGTH));

            ASSERT_PASS(Util::parse(     &mX, STRING_REF));
            ASSERT_FAIL(Util::parse(NULL_PTR, STRING_REF));
        }

        if (veryVerbose) cout << "\t'Invalid input'" << endl;
        {
            const char    *NULL_PTR = 0;
            const StrView  NULL_REF;

            ASSERT_PASS(Util::parse(&mX,    INPUT, LENGTH));
            ASSERT_FAIL(Util::parse(&mX, NULL_PTR, LENGTH));

            ASSERT_PASS(Util::parse(&mX, STRING_REF));
            ASSERT_FAIL(Util::parse(&mX,   NULL_REF));
        }

        if (veryVerbose) cout << "\t'Invalid length'" << endl;
        {
            ASSERT_PASS(Util::parse(&mX, INPUT, LENGTH));
            ASSERT_PASS(Util::parse(&mX, INPUT,      0));
            ASSERT_FAIL(Util::parse(&mX, INPUT,     -1));
        }
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void)veryVeryVerbose;  // eliminate unused variable warning
    (void)veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:  // Zero is always the leading case.
      case 18: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'bdlt::Iso8601Util' Usage
/// - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates basic use of one 'generate' function and two
// 'parse' functions.
//
// First, we construct a few objects that are prerequisites for this and the
// following example:
//..
    const bdlt::Date date(2005, 1, 31);     // 2005/01/31
    const bdlt::Time time(8, 59, 59, 123);  // 08:59:59.123
    const int        tzOffset = 240;        // +04:00 (four hours west of UTC)
//..
// Then, we construct a 'bdlt::DatetimeTz' object for which a corresponding ISO
// 8601-compliant string will be generated shortly:
//..
    const bdlt::DatetimeTz sourceDatetimeTz(bdlt::Datetime(date, time),
                                            tzOffset);
//..
// For comparison with the ISO 8601 string generated below, note that streaming
// the value of 'sourceDatetimeTz' to 'stdout':
//..
if (veryVerbose)
    bsl::cout << sourceDatetimeTz << bsl::endl;
//..
// produces:
//..
//  31JAN2005_08:59:59.123000+0400
//..
// Next, we use a 'generate' function to produce an ISO 8601-compliant string
// for 'sourceDatetimeTz', writing the output to a 'bsl::ostringstream', and
// assert that both the return value and the string that is produced are as
// expected:
//..
    bsl::ostringstream  oss;
    const bsl::ostream& ret =
                           bdlt::Iso8601Util::generate(oss, sourceDatetimeTz);
    ASSERT(&oss == &ret);

    const bsl::string iso8601 = oss.str();
    ASSERT(iso8601 == "2005-01-31T08:59:59.123+04:00");
//..
// For comparison, see the output that was produced by the streaming operator
// above.
//
// Now, we parse the string that was just produced, loading the result of the
// parse into a second 'bdlt::DatetimeTz' object, and assert that the parse was
// successful and that the target object has the same value as that of the
// original (i.e., 'sourceDatetimeTz'):
//..
    bdlt::DatetimeTz targetDatetimeTz;

    int rc = bdlt::Iso8601Util::parse(&targetDatetimeTz,
                                      iso8601.c_str(),
                                      static_cast<int>(iso8601.length()));
    ASSERT(               0 == rc);
    ASSERT(sourceDatetimeTz == targetDatetimeTz);
//..
// Finally, we parse the 'iso8601' string a second time, this time loading the
// result into a 'bdlt::Datetime' object (instead of a 'bdlt::DatetimeTz'):
//..
    bdlt::Datetime targetDatetime;

    rc = bdlt::Iso8601Util::parse(&targetDatetime,
                                  iso8601.c_str(),
                                  static_cast<int>(iso8601.length()));
    ASSERT(                             0 == rc);
    ASSERT(sourceDatetimeTz.utcDatetime() == targetDatetime);
//..
// Note that this time the value of the target object has been converted to
// UTC.
//
///Example 2: Configuring ISO 8601 String Generation
///- - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates use of a 'bdlt::Iso8601UtilConfiguration' object
// to influence the format of the ISO 8601 strings that are generated by this
// component by passing that configuration object to 'generate'.  We also take
// this opportunity to illustrate the flavor of the 'generate' functions that
// outputs to a 'char *' buffer of a specified length.
//
// First, we construct a 'bdlt::TimeTz' object for which a corresponding ISO
// 8601-compliant string will be generated shortly:
//..
    const bdlt::TimeTz sourceTimeTz(time, tzOffset);
//..
// For comparison with the ISO 8601 string generated below, note that streaming
// the value of 'sourceTimeTz' to 'stdout':
//..
if (veryVerbose)
    bsl::cout << sourceTimeTz << bsl::endl;
//..
// produces:
//..
//  08:59:59.123+0400
//..
// Then, we construct the 'bdlt::Iso8601UtilConfiguration' object that
// indicates how we would like to effect the generated output ISO 8601 string.
// In this case, we want to use ',' as the decimal sign (in fractional seconds)
// and omit the ':' in zone designators:
//..
    bdlt::Iso8601UtilConfiguration configuration;
    configuration.setOmitColonInZoneDesignator(true);
    configuration.setUseCommaForDecimalSign(true);
//..
// Next, we define the 'char *' buffer that will be used to stored the
// generated string.  A buffer of size 'bdlt::Iso8601Util::k_TIMETZ_STRLEN + 1'
// is large enough to hold any string generated by this component for a
// 'bdlt::TimeTz' object, including a null terminator:
//..
    const int BUFLEN = bdlt::Iso8601Util::k_TIMETZ_STRLEN + 1;
    char      buffer[BUFLEN];
//..
// Then, we use a 'generate' function that accepts our 'configuration' to
// produce an ISO 8601-compliant string for 'sourceTimeTz', this time writing
// the output to a 'char *' buffer, and assert that both the return value and
// the string that is produced are as expected.  Note that in comparing the
// return value against 'BUFLEN - 5' we account for the omission of the ':'
// from the zone designator, and also for the fact that, although a null
// terminator was generated, it is not included in the character count returned
// by 'generate'.  Also note that we use 'bsl::strcmp' to compare the resulting
// string knowing that we supplied a buffer having sufficient capacity to
// accommodate a null terminator:
//..
    rc = bdlt::Iso8601Util::generate(buffer,
                                     BUFLEN,
                                     sourceTimeTz,
                                     configuration);
    ASSERT(BUFLEN - 5 == rc);
    ASSERT(         0 == bsl::strcmp(buffer, "08:59:59,123+0400"));
//..
// For comparison, see the output that was produced by the streaming operator
// above.
//
// Next, we parse the string that was just produced, loading the result of the
// parse into a second 'bdlt::TimeTz' object, and assert that the parse was
// successful and that the target object has the same value as that of the
// original (i.e., 'sourceTimeTz').  Note that 'BUFLEN - 5' is passed and *not*
// 'BUFLEN' because the former indicates the correct number of characters in
// 'buffer' that we wish to parse:
//..
    bdlt::TimeTz targetTimeTz;

    rc = bdlt::Iso8601Util::parse(&targetTimeTz, buffer, BUFLEN - 5);

    ASSERT(           0 == rc);
    ASSERT(sourceTimeTz == targetTimeTz);
//..
// Then, we parse the string in 'buffer' a second time, this time loading the
// result into a 'bdlt::Time' object (instead of a 'bdlt::TimeTz'):
//..
    bdlt::Time targetTime;

    rc = bdlt::Iso8601Util::parse(&targetTime, buffer, BUFLEN - 5);
    ASSERT(                     0 == rc);
    ASSERT(sourceTimeTz.utcTime() == targetTime);
//..
// Note that this time the value of the target object has been converted to
// UTC.
//
// Finally, we modify the 'configuration' to display the 'bdlt::TimeTz' without
// fractional seconds:
//..
    configuration.setFractionalSecondPrecision(0);
    rc = bdlt::Iso8601Util::generate(buffer,
                                     BUFLEN,
                                     sourceTimeTz,
                                     configuration);
    ASSERT(BUFLEN - 9 == rc);
    ASSERT(         0 == bsl::strcmp(buffer, "08:59:59+0400"));
//..
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // PARSE 'DatetimeOrDatetimeTz'
        //
        // Concerns:
        //: 1 All ISO 8601 string representations supported by this component
        //:   (as documented in the header file) for 'Datetime' and
        //:   'DatetimeTz' values are parsed successfully.
        //:
        //: 2 If parsing succeeds, the result 'Datetime' or 'DatetimeTz' object
        //:   has the expected value of the expected type no matter what value
        //:   of what type was stored there previously.
        //:
        //: 3 The result object contains 'DatetimeTz' value if the optional
        //:   zone designator is present in the input string, and 'Datetime'
        //:   value otherwise.
        //:
        //: 4 The result object contains 'DatetimeTz' value if 'Z' suffix is
        //:   present in the input string, and it is assumed to be UTC.
        //:
        //: 5 If parsing succeeds, 0 is returned.
        //:
        //: 6 All strings that are not ISO 8601 representations supported by
        //:   this component for 'Datetime' and 'DatetimeTz' values are
        //:   rejected (i.e., parsing fails).
        //:
        //: 7 If parsing fails, the result object is unaffected and a non-zero
        //:   value is returned.
        //:
        //: 8 The entire extent of the input string is parsed.
        //:
        //: 9 Leap seconds, fractional seconds containing more than three
        //:   digits, and extremal values (those that can overflow a
        //:   'Datetime') are handled correctly.
        //:
        //:10 The 'parseRelaxed' functions do the same as the 'parse' functions
        //:   and additionally allow to use a SPACE characters instead of 'T'.
        //:
        //:11 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'Date' values ('D'), 'Time' values ('T'), zone designators ('Z'),
        //:   and configurations ('C').
        //:
        //: 2 Apply the (fully-tested) 'generateRaw' functions to each element
        //:   in the cross product, 'D x T x Z x C', of the test data from P-1.
        //:
        //: 3 Invoke the 'parse' functions on the strings generated in P-2 and
        //:   verify that parsing succeeds, i.e., that 0 is returned and the
        //:   result objects have the expected values.  (C-1..5)
        //:
        //: 4 Using the table-driven technique, specify a set of distinct
        //:   strings that are not ISO 8601 representations supported by this
        //:   component for 'Datetime' and 'DatetimeTz' values.
        //:
        //: 5 Invoke the 'parse' functions on the strings from P-4 and verify
        //:   that parsing fails, i.e., that a non-zero value is returned and
        //:   the result objects are unchanged.  (C-6..8)
        //:
        //: 6 Using the table-driven technique, specify a set of distinct ISO
        //:   8601 strings that specifically cover cases involving leap
        //:   seconds, fractional seconds containing more than three digits,
        //:   and extremal values.
        //:
        //: 7 Invoke the 'parse' functions on the strings from P-6 and verify
        //:   the results are as expected.  (C-9)
        //:
        //: 8 Invoke the 'parseRelaxed' functions on the same strings and on
        //:   the strings with 'T' character replaced by ' '. (C-10)
        //:
        //: 9 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-11)
        //
        // Testing:
        //   int parse(DatetimeOrDatetimeTz *, const char *, int);
        //   int parse(DatetimeOrDatetimeTz *result, const StringRef& string);
        //   int parseRelaxed(DatetimeOrDatetimeTz *, const char *, int);
        //   int parseRelaxed(DatetimeOrDatetimeTz *, const bsl::string_view&);
        // --------------------------------------------------------------------

        testCase17(verbose, veryVerbose, veryVeryVerbose, veryVeryVeryVerbose);

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // PARSE 'TimeOrTimeTz'
        //
        // Concerns:
        //: 1 All ISO 8601 string representations supported by this component
        //:   (as documented in the header file) for 'Time' and 'TimeTz' values
        //:   are parsed successfully.
        //:
        //: 2 If parsing succeeds, the result object contains the expected
        //:   value of the expected type no matter what value of what type was
        //:   stored there previously.
        //:
        //: 3 The result object contains 'TimeTz' value if the optional zone
        //:   designator is present in the input string, and 'Time' value
        //:   otherwise.
        //:
        //: 4 The result object contains 'TimeTz' value if 'Z' suffix is
        //:   present in the input string, and it is assumed to be UTC.
        //:
        //: 5 If parsing succeeds, 0 is returned.
        //:
        //: 6 All strings that are not ISO 8601 representations supported by
        //:   this component for 'Time' and 'TimeTz' values are rejected (i.e.,
        //:   parsing fails).
        //:
        //: 7 If parsing fails, the result object is unaffected and a non-zero
        //:   value is returned.
        //:
        //: 8 The entire extent of the input string is parsed.
        //:
        //: 9 Leap seconds and fractional seconds containing more than three
        //:   digits are handled correctly.
        //:
        //:10 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'Time' values ('T'), zone designators ('Z'), and configurations
        //:   ('C').
        //:
        //: 2 Apply the (fully-tested) 'generateRaw' functions to each element
        //:   in the cross product, 'T x Z x C', of the test data from P-1.
        //:
        //: 3 Invoke the 'parse' functions on the strings generated in P-2 and
        //:   verify that parsing succeeds, i.e., that 0 is returned and the
        //:   result objects have the expected values.  (C-1..5)
        //:
        //: 4 Using the table-driven technique, specify a set of distinct
        //:   strings that are not ISO 8601 representations supported by this
        //:   component for 'Time' and 'TimeTz' values.
        //:
        //: 5 Invoke the 'parse' functions on the strings from P-4 and verify
        //:   that parsing fails, i.e., that a non-zero value is returned and
        //:   the result objects are unchanged.  (C-6..8)
        //:
        //: 6 Using the table-driven technique, specify a set of distinct
        //:   ISO 8601 strings that specifically cover cases involving leap
        //:   seconds and fractional seconds containing more than three digits.
        //:
        //: 7 Invoke the 'parse' functions on the strings from P-6 and verify
        //:   the results are as expected.  (C-9)
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-10)
        //
        // Testing:
        //   int parse(TimeOrTimeTz *, const char *, int);
        //   int parse(TimeOrTimeTz *result, const StringRef& string);
        // --------------------------------------------------------------------

        testCase16(verbose, veryVerbose, veryVeryVerbose, veryVeryVeryVerbose);

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // PARSE 'DateOrDateTz'
        //
        // Concerns:
        //: 1 All ISO 8601 string representations supported by this component
        //:   (as documented in the header file) for 'Date' and 'DateTz' values
        //:   are parsed successfully.
        //:
        //: 2 If parsing succeeds, the result object contains the expected
        //:   value of the expected type no matter what value of what type was
        //:   stored there previously.
        //:
        //: 3 The result object contains 'DateTz' value if the optional zone
        //:   designator is present in the input string, and 'Date' value
        //:   otherwise.
        //:
        //: 4 The result object contains 'DateTz' value if 'Z' suffix is
        //:   present in the input string, and it is assumed to be UTC.
        //:
        //: 5 If parsing succeeds, 0 is returned.
        //:
        //: 6 All strings that are not ISO 8601 representations supported by
        //:   this component for 'Date' and 'DateTz' values are rejected (i.e.,
        //:   parsing fails).
        //:
        //: 7 If parsing fails, the result object is unaffected and a non-zero
        //:   value is returned.
        //:
        //: 8 The entire extent of the input string is parsed.
        //:
        //: 9 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'Date' values ('D'), zone designators ('Z'), and configurations
        //:   ('C').
        //:
        //: 2 Apply the (fully-tested) 'generateRaw' functions to each element
        //:   in the cross product, 'D x Z x C', of the test data from P-1.
        //:
        //: 3 Invoke the 'parse' functions on the strings generated in P-2 and
        //:   verify that parsing succeeds, i.e., that 0 is returned and the
        //:   result objects have the expected values.  (C-1..5)
        //:
        //: 4 Using the table-driven technique, specify a set of distinct
        //:   strings that are not ISO 8601 representations supported by this
        //:   component for 'Date' and 'DateTz' values.
        //:
        //: 5 Invoke the 'parse' functions on the strings from P-4 and verify
        //:   that parsing fails, i.e., that a non-zero value is returned and
        //:   the result objects are unchanged.  (C-6..8)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-9)
        //
        // Testing:
        //   int parse(DateOrDateTz *, const char *, int);
        //   int parse(DateOrDateTz *result, const StringRef& string);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PARSE 'DateOrDateTz'" << endl
                          << "====================" << endl;

        Util::DateOrDateTz        mX;
        const Util::DateOrDateTz& X = mX;

        char buffer[Util::k_MAX_STRLEN];

        const bdlt::Date   XX(246, 8, 10);  // 'XX' and 'ZZ' are controls,
        const bdlt::DateTz ZZ(XX, -7);      // distinct from any test data

        const int                  NUM_DATE_DATA =       NUM_DEFAULT_DATE_DATA;
        const DefaultDateDataRow (&DATE_DATA)[NUM_DATE_DATA] =
                                                             DEFAULT_DATE_DATA;

        const int                  NUM_ZONE_DATA =       NUM_DEFAULT_ZONE_DATA;
        const DefaultZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] =
                                                             DEFAULT_ZONE_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        if (verbose) cout << "\nValid ISO 8601 strings." << endl;

        for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
            const int ILINE = DATE_DATA[ti].d_line;
            const int YEAR  = DATE_DATA[ti].d_year;
            const int MONTH = DATE_DATA[ti].d_month;
            const int DAY   = DATE_DATA[ti].d_day;

            const bdlt::Date DATE(YEAR, MONTH, DAY);

            for (int tj = 0; tj < NUM_ZONE_DATA; ++tj) {
                const int JLINE  = ZONE_DATA[tj].d_line;
                const int OFFSET = ZONE_DATA[tj].d_offset;

                const bdlt::DateTz DATETZ(DATE, OFFSET);

                if (veryVerbose) { T_ P_(ILINE) P_(JLINE) P_(DATE) P(DATETZ) }

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    // without zone designator in parsed string
                    {
                        const int LENGTH = Util::generateRaw(buffer, DATE, C);

                        if (veryVerbose) {
                            const bsl::string STRING(buffer, LENGTH);
                            T_ T_ P(STRING)
                        }

                        mX.reset();
                        ASSERTV(ILINE, JLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Date>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                DATE == X.the<bdlt::Date>());

                        mX = XX;
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Date>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                XX == X.the<bdlt::Date>());

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Date>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                DATE == X.the<bdlt::Date>());

                        mX = ZZ;

                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::DateTz>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                ZZ == X.the<bdlt::DateTz>());

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Date>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                DATE == X.the<bdlt::Date>());

                        mX.reset();
                        ASSERTV(ILINE, JLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Date>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                DATE == X.the<bdlt::Date>());

                        mX = XX;
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Date>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                XX == X.the<bdlt::Date>());

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Date>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                DATE == X.the<bdlt::Date>());

                        mX = ZZ;

                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::DateTz>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                ZZ == X.the<bdlt::DateTz>());

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Date>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                DATE == X.the<bdlt::Date>());
                    }

                    // with zone designator in parsed string
                    {
                        const int LENGTH = Util::generateRaw(buffer,
                                                             DATETZ,
                                                             C);

                        if (veryVerbose) {
                            const bsl::string STRING(buffer, LENGTH);
                            T_ T_ P(STRING)
                        }

                        mX.reset();
                        ASSERTV(ILINE, JLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::DateTz>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                DATETZ == X.the<bdlt::DateTz>());

                        mX = XX;

                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Date>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                XX == X.the<bdlt::Date>());

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::DateTz>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                DATETZ == X.the<bdlt::DateTz>());

                        mX = ZZ;

                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::DateTz>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                ZZ == X.the<bdlt::DateTz>());

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::DateTz>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                DATETZ == X.the<bdlt::DateTz>());

                        mX.reset();
                        ASSERTV(ILINE, JLINE, CLINE, X.isUnset());

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::DateTz>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                DATETZ == X.the<bdlt::DateTz>());

                        mX = XX;

                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::Date>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                XX == X.the<bdlt::Date>());

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::DateTz>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                DATETZ == X.the<bdlt::DateTz>());

                        mX = ZZ;

                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::DateTz>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                ZZ == X.the<bdlt::DateTz>());

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, X.is<bdlt::DateTz>());
                        ASSERTV(ILINE, JLINE, CLINE,
                                DATETZ == X.the<bdlt::DateTz>());
                    }
                }  // loop over 'CNFG_DATA'
            }  // loop over 'ZONE_DATA'
        }  // loop over 'DATE_DATA'

        {
            // verify 'z' is accepted

            const bdlt::DateTz EXPECTED(bdlt::Date(1,2,3) , 0);

            mX.reset();
            ASSERTV(X.isUnset());

            ASSERTV(0 == Util::parse(&mX, "0001-02-03z", 11));
            ASSERTV( X.is<bdlt::DateTz>());
            ASSERTV(EXPECTED == X.the<bdlt::DateTz>());
        }

        if (verbose) cout << "\nInvalid strings." << endl;
        {
            const int              NUM_DATE_DATA =         NUM_BAD_DATE_DATA;
            const BadDateDataRow (&DATE_DATA)[NUM_DATE_DATA] = BAD_DATE_DATA;

            for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
                const int   LINE   = DATE_DATA[ti].d_line;
                const char *STRING = DATE_DATA[ti].d_invalid;

                if (veryVerbose) { T_ P_(LINE) P(STRING) }

                const int LENGTH = static_cast<int>(bsl::strlen(STRING));

                mX.reset();
                ASSERTV(LINE, STRING, X.isUnset());

                ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, X.isUnset());

                mX = XX;
                ASSERTV(LINE, STRING, X.is<bdlt::Date>());
                ASSERTV(LINE, STRING, XX == X.the<bdlt::Date>());

                ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

                ASSERTV(LINE, STRING, X.is<bdlt::Date>());
                ASSERTV(LINE, STRING, XX == X.the<bdlt::Date>());

                mX = ZZ;
                ASSERTV(LINE, STRING, X.is<bdlt::DateTz>());
                ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DateTz>());

                ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

                ASSERTV(LINE, STRING, X.is<bdlt::DateTz>());
                ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DateTz>());

                mX.reset();
                ASSERTV(LINE, STRING, X.isUnset());

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, X.isUnset());

                mX = XX;
                ASSERTV(LINE, STRING, X.is<bdlt::Date>());
                ASSERTV(LINE, STRING, XX == X.the<bdlt::Date>());

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));

                ASSERTV(LINE, STRING, X.is<bdlt::Date>());
                ASSERTV(LINE, STRING, XX == X.the<bdlt::Date>());

                mX = ZZ;
                ASSERTV(LINE, STRING, X.is<bdlt::DateTz>());
                ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DateTz>());

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));

                ASSERTV(LINE, STRING, X.is<bdlt::DateTz>());
                ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DateTz>());
            }

            const int              NUM_ZONE_DATA =         NUM_BAD_ZONE_DATA;
            const BadZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] = BAD_ZONE_DATA;

            for (int ti = 0; ti < NUM_ZONE_DATA; ++ti) {
                const int LINE = ZONE_DATA[ti].d_line;

                // Initialize with a *valid* date string, then append an
                // invalid zone designator.

                bsl::string bad("2010-08-17");

                // Ensure that 'bad' is initially valid, but only during the
                // first iteration.

                if (0 == ti) {
                    const char *STRING = bad.data();
                    const int   LENGTH = static_cast<int>(bad.length());

                    mX = XX;

                    ASSERT( 0 == Util::parse(&mX, STRING, LENGTH));
                    ASSERT(mX.is<bdlt::Date>());
                    ASSERT(XX != mX.the<bdlt::Date>());

                    mX = XX;

                    ASSERT( 0 == Util::parse(&mX, StrView(STRING, LENGTH)));
                    ASSERT(mX.is<bdlt::Date>());
                    ASSERT(XX != mX.the<bdlt::Date>());
                }

                bad.append(ZONE_DATA[ti].d_invalid);

                const char *STRING = bad.c_str();
                const int   LENGTH = static_cast<int>(bad.length());

                if (veryVerbose) { T_ P_(LINE) P(STRING) }

                mX.reset();
                ASSERTV(LINE, STRING, X.isUnset());

                ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, X.isUnset());

                mX = XX;
                ASSERTV(LINE, STRING, X.is<bdlt::Date>());
                ASSERTV(LINE, STRING, XX == X.the<bdlt::Date>());

                ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

                ASSERTV(LINE, STRING, X.is<bdlt::Date>());
                ASSERTV(LINE, STRING, XX == X.the<bdlt::Date>());

                mX = ZZ;
                ASSERTV(LINE, STRING, X.is<bdlt::DateTz>());
                ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DateTz>());

                ASSERTV(LINE, STRING, 0 != Util::parse(&mX, STRING, LENGTH));

                ASSERTV(LINE, STRING, X.is<bdlt::DateTz>());
                ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DateTz>());

                mX.reset();
                ASSERTV(LINE, STRING, X.isUnset());

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, X.isUnset());

                mX = XX;
                ASSERTV(LINE, STRING, X.is<bdlt::Date>());
                ASSERTV(LINE, STRING, XX == X.the<bdlt::Date>());

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));

                ASSERTV(LINE, STRING, X.is<bdlt::Date>());
                ASSERTV(LINE, STRING, XX == X.the<bdlt::Date>());

                mX = ZZ;
                ASSERTV(LINE, STRING, X.is<bdlt::DateTz>());
                ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DateTz>());

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));

                ASSERTV(LINE, STRING, X.is<bdlt::DateTz>());
                ASSERTV(LINE, STRING, ZZ == X.the<bdlt::DateTz>());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const char *INPUT  = "2013-10-23";
            const int   LENGTH = static_cast<int>(bsl::strlen(INPUT));

            const StrView stringRef(INPUT, LENGTH);
            const StrView nullRef;

            if (veryVerbose) cout << "\t'Invalid result'" << endl;
            {
                Util::DateOrDateTz *nullPtr = 0;

                ASSERT_PASS(Util::parse(&mX,     INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(nullPtr, INPUT, LENGTH));

                ASSERT_PASS(Util::parse(&mX,     stringRef));
                ASSERT_FAIL(Util::parse(nullPtr, stringRef));
            }

            if (veryVerbose) cout << "\t'Invalid input'" << endl;
            {
                ASSERT_PASS(Util::parse(&mX, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(&mX,     0, LENGTH));

                ASSERT_PASS(Util::parse(&mX, stringRef));
                ASSERT_FAIL(Util::parse(&mX, nullRef  ));
            }

            if (veryVerbose) cout << "\t'Invalid length'" << endl;
            {
                ASSERT_PASS(Util::parse(&mX, INPUT, LENGTH));
                ASSERT_PASS(Util::parse(&mX, INPUT,      0));
                ASSERT_FAIL(Util::parse(&mX, INPUT,     -1));
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // GENERATE 'DatetimeTz'
        //
        // Concerns:
        //: 1 The output generated by each method has the expected format and
        //:   contents.
        //:
        //: 2 When sufficient capacity is indicated, the method taking
        //:   'bufferLength' generates a null terminator.
        //:
        //: 3 Each method returns the expected value (the correct character
        //:   count or the supplied 'ostream', depending on the return type).
        //:
        //: 4 The value of the supplied object is unchanged.
        //:
        //: 5 The configuration that is in effect, whether user-supplied or the
        //:   process-wide default, has the desired effect on the output.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a 'Variant2<Datetime, DatetimeTz>' object, 'X'.
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   'Date' values (one per row) and their corresponding ISO 8601
        //:   string representations.
        //:
        //: 3 In a second table, specify a set of distinct 'Time' values (one
        //:   per row) and their corresponding ISO 8601 string representations.
        //:
        //: 4 For each element 'R1' in the cross product of the tables from P-2
        //:   and P-3 create a 'const' 'Datetime' object, 'DT1', from 'R1'.
        //:
        //: 5 In a third table, specify a set of distinct timezone values (one
        //:   per row) and their corresponding ISO 8601 string representations.
        //:
        //: 6 For each element 'R2' in the cross product of the tables from
        //:   P-2, P-3, and P-5:
        //:
        //:   1 Create a 'const' 'DatetimeTz' object, 'DT2', from 'R2'.
        //:
        //:   2 Assign 'DT1' to 'X'.
        //:
        //:   3 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R1' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.
        //:
        //:   4 Assign 'DT2' to 'X'.
        //:
        //:   5 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R2' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 7 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, int, const DatetimeOrDatetimeTz&);
        //   int generate(char*,int,const DatetimeOrDatetimeTz&,const Config&);
        //   int generate(string *, const DatetimeOrDatetimeTz&);
        //   int generate(string*, const DatetimeOrDatetimeTz&, const Config&);
        //   ostream generate(ostream&, const DatetimeOrDatetimeTz&);
        //   ostream generate(ostream&,const DatetimeOrDatetimeTz&,const Con&);
        //   int generateRaw(char *, const DatetimeOrDatetimeTz&);
        //   int generateRaw(char*,const DatetimeOrDatetimeTz&, const Config&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATE 'DatetimeTz'" << endl
                          << "=====================" << endl;

        typedef Util::DatetimeOrDatetimeTz TYPE;

        TYPE        mX;
        const TYPE& X = mX;

        const int OBJLEN = Util::k_DATETIMETZ_STRLEN;
        const int BUFLEN = OBJLEN + 4;

        char buffer[BUFLEN];
        char chaste[BUFLEN];  bsl::memset(chaste, '?', BUFLEN);

        const int                  NUM_DATE_DATA =       NUM_DEFAULT_DATE_DATA;
        const DefaultDateDataRow (&DATE_DATA)[NUM_DATE_DATA] =
                                                             DEFAULT_DATE_DATA;

        const int                  NUM_TIME_DATA =       NUM_DEFAULT_TIME_DATA;
        const DefaultTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] =
                                                             DEFAULT_TIME_DATA;

        const int                  NUM_ZONE_DATA =       NUM_DEFAULT_ZONE_DATA;
        const DefaultZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] =
                                                             DEFAULT_ZONE_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
            const int   ILINE   = DATE_DATA[ti].d_line;
            const int   YEAR    = DATE_DATA[ti].d_year;
            const int   MONTH   = DATE_DATA[ti].d_month;
            const int   DAY     = DATE_DATA[ti].d_day;
            const char *ISO8601 = DATE_DATA[ti].d_iso8601;

            const bdlt::Date  DATE(YEAR, MONTH, DAY);
            const bsl::string EXPECTED_DATE(ISO8601);

            for (int tj = 0; tj < NUM_TIME_DATA; ++tj) {
                const int   JLINE   = TIME_DATA[tj].d_line;
                const int   HOUR    = TIME_DATA[tj].d_hour;
                const int   MIN     = TIME_DATA[tj].d_min;
                const int   SEC     = TIME_DATA[tj].d_sec;
                const int   MSEC    = TIME_DATA[tj].d_msec;
                const int   USEC    = TIME_DATA[tj].d_usec;
                const char *ISO8601 = TIME_DATA[tj].d_iso8601;

                const bdlt::Time     TIME(HOUR, MIN, SEC, MSEC);
                const bsl::string    EXPECTED_TIME(ISO8601);
                const bdlt::Datetime DATETIME(YEAR,
                                              MONTH,
                                              DAY,
                                              HOUR,
                                              MIN,
                                              SEC,
                                              MSEC,
                                              USEC);
                const bsl::string    DATETIME_BASE_EXPECTED(
                                          EXPECTED_DATE + 'T' + EXPECTED_TIME);

                for (int tk = 0; tk < NUM_ZONE_DATA; ++tk) {
                    const int   KLINE   = ZONE_DATA[tk].d_line;
                    const int   OFFSET  = ZONE_DATA[tk].d_offset;
                    const char *ISO8601 = ZONE_DATA[tk].d_iso8601;

                    const bsl::string EXPECTED_ZONE(ISO8601);

                    if (TIME == bdlt::Time() && OFFSET != 0) {
                        continue;  // skip invalid compositions
                    }

                    const bdlt::DatetimeTz DATETIMETZ(DATETIME, OFFSET);
                    const bsl::string      DATETIMETZ_BASE_EXPECTED(
                                       DATETIME_BASE_EXPECTED + EXPECTED_ZONE);

                if (veryVerbose) {
                    T_ P_(ILINE) P_(JLINE) P_(DATETIME) P_(DATETIMETZ)
                       P_(DATETIME_BASE_EXPECTED) P(DATETIMETZ_BASE_EXPECTED)
                }

                    for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                        const int  CLINE     = CNFG_DATA[tc].d_line;
                        const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                        const int  PRECISION = CNFG_DATA[tc].d_precision;
                        const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                        const bool USEZ      = CNFG_DATA[tc].d_useZ;

                        if (veryVerbose) {
                            T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                        }

                        Config mC;  const Config& C = mC;
                        gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                        Config::setDefaultConfiguration(C);

                        bsl::string EXPECTED_DATETIME(DATETIME_BASE_EXPECTED);
                        updateExpectedPerConfig(&EXPECTED_DATETIME,
                                                C,
                                                k_DATETIME_MAX_PRECISION);
                        bsl::string EXPECTED_DATETIMETZ(
                                                     DATETIMETZ_BASE_EXPECTED);
                        updateExpectedPerConfig(&EXPECTED_DATETIMETZ,
                                                C,
                                                k_DATETIMETZ_MAX_PRECISION);

                        for (int tt = 0; tt < 2; ++tt) {
                            const int OBJ_TYPE = tt;
                            if (OBJ_TYPE) {
                                mX = DATETIMETZ;
                            }
                            else {
                                mX = DATETIME;
                            }

                            const bsl::string& EXPECTED =
                                OBJ_TYPE ? EXPECTED_DATETIMETZ
                                         : EXPECTED_DATETIME;
                            const int          OUTLEN =
                                static_cast<int>(EXPECTED.length());

                            // 'generate' taking 'bufferLength'

                            for (int k = 0; k < BUFLEN; ++k) {
                                bsl::memset(buffer, '?', BUFLEN);

                                if (veryVeryVerbose) {
                                    T_ T_ cout << "Length: ";
                                    P(k)
                                }

                                ASSERTV(ILINE,
                                        JLINE,
                                        KLINE,
                                        k,
                                        OUTLEN,
                                        OUTLEN ==
                                            Util::generate(buffer, k, X));

                                ASSERTV(
                                    ILINE,
                                    JLINE,
                                    KLINE,
                                    EXPECTED,
                                    buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                                if (k <= OUTLEN) {
                                    ASSERTV(ILINE,
                                            JLINE,
                                            KLINE,
                                            EXPECTED,
                                            buffer,
                                            0 == bsl::memcmp(chaste,
                                                             buffer + k,
                                                             BUFLEN - k));
                                }
                                else {
                                    ASSERTV(ILINE,
                                            JLINE,
                                            KLINE,
                                            k,
                                            OUTLEN,
                                            '\0' == buffer[OUTLEN]);

                                    ASSERTV(ILINE,
                                            JLINE,
                                            KLINE,
                                            EXPECTED,
                                            buffer,
                                            0 == bsl::memcmp(chaste,
                                                             buffer + k + 1,
                                                             BUFLEN - k - 1));
                                }
                            }

                            // 'generate' to a 'bsl::string'
                            {
                                bsl::string mS("qwerty");

                                ASSERTV(ILINE, JLINE, KLINE, OUTLEN,
                                        OUTLEN == Util::generate(&mS, X));

                                ASSERTV(ILINE, JLINE, KLINE, EXPECTED, mS,
                                        EXPECTED == mS);

                                if (veryVerbose) { P_(EXPECTED) P(mS); }
                            }

                            // 'generate' to an 'std::string'
                            {
                                std::string mS("qwerty");

                                ASSERTV(ILINE, JLINE, KLINE, OUTLEN,
                                        OUTLEN == Util::generate(&mS, X));

                                ASSERTV(ILINE, JLINE, KLINE, EXPECTED, mS,
                                        EXPECTED == mS);

                                if (veryVerbose) { P_(EXPECTED) P(mS); }
                            }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                            // 'generate' to an 'std::pmr::string'
                            {
                                std::pmr::string mS("qwerty");

                                ASSERTV(ILINE, JLINE, KLINE, OUTLEN,
                                        OUTLEN == Util::generate(&mS, X));

                                ASSERTV(ILINE, JLINE, KLINE, EXPECTED, mS,
                                        EXPECTED == mS);

                                if (veryVerbose) { P_(EXPECTED) P(mS); }
                            }
#endif

                            // 'generate' to an 'ostream'
                            {
                                bsl::ostringstream os;

                                ASSERTV(ILINE, JLINE, KLINE,
                                        &os == &Util::generate(os, X));

                                ASSERTV(ILINE,
                                        JLINE,
                                        KLINE,
                                        EXPECTED,
                                        os.str(),
                                        EXPECTED == os.str());

                                if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                            }

                            // 'generateRaw'
                            {
                                bsl::memset(buffer, '?', BUFLEN);

                                ASSERTV(ILINE,
                                        JLINE,
                                        KLINE,
                                        OUTLEN,
                                        OUTLEN ==
                                            Util::generateRaw(buffer, X));

                                ASSERTV(ILINE, JLINE, KLINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(EXPECTED.c_str(),
                                                         buffer,
                                                         OUTLEN));

                                ASSERTV(ILINE, JLINE, KLINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + OUTLEN,
                                                         BUFLEN - OUTLEN));
                            }
                        }  // loop over 'OBJ_TYPE' ('Datetime' or 'DatetimeTz')
                    }  // loop over 'CNFG_DATA'

                    for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                        const int  CLINE     = CNFG_DATA[tc].d_line;
                        const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                        const int  PRECISION = CNFG_DATA[tc].d_precision;
                        const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                        const bool USEZ      = CNFG_DATA[tc].d_useZ;

                        if (veryVerbose) {
                            T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                        }

                        Config mC;  const Config& C = mC;
                        gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                        // Set the default configuration to the complement of
                        // 'C'.

                        Config mDFLT;  const Config& DFLT = mDFLT;
                        gg(&mDFLT,
                           9 - PRECISION,
                           !OMITCOLON,
                           !USECOMMA,
                           !USEZ);
                        Config::setDefaultConfiguration(DFLT);

                        bsl::string EXPECTED_DATETIME(DATETIME_BASE_EXPECTED);
                        updateExpectedPerConfig(&EXPECTED_DATETIME,
                                                C,
                                                k_DATETIME_MAX_PRECISION);
                        bsl::string EXPECTED_DATETIMETZ(
                                                     DATETIMETZ_BASE_EXPECTED);
                        updateExpectedPerConfig(&EXPECTED_DATETIMETZ,
                                                C,
                                                k_DATETIMETZ_MAX_PRECISION);

                        for (int tt = 0; tt < 2; ++tt) {
                            const int OBJ_TYPE = tt;
                            if (OBJ_TYPE) {
                                mX = DATETIMETZ;
                            }
                            else {
                                mX = DATETIME;
                            }

                            const bsl::string& EXPECTED =
                                OBJ_TYPE ? EXPECTED_DATETIMETZ
                                         : EXPECTED_DATETIME;
                            const int          OUTLEN =
                                static_cast<int>(EXPECTED.length());

                            // 'generate' taking 'bufferLength'

                            for (int k = 0; k < BUFLEN; ++k) {
                                bsl::memset(buffer, '?', BUFLEN);

                                if (veryVeryVerbose) {
                                    T_ T_ cout << "Length: "; P(k)
                                }

                                ASSERTV(ILINE,
                                        k,
                                        OUTLEN,
                                        OUTLEN ==
                                            Util::generate(buffer, k, X, C));

                                ASSERTV(
                                    ILINE,
                                    EXPECTED,
                                    buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                                if (k <= OUTLEN) {
                                    ASSERTV(ILINE, EXPECTED, buffer,
                                            0 == bsl::memcmp(chaste,
                                                             buffer + k,
                                                             BUFLEN - k));
                                }
                                else {
                                    ASSERTV(ILINE, k, OUTLEN,
                                            '\0' == buffer[OUTLEN]);

                                    ASSERTV(ILINE, EXPECTED, buffer,
                                            0 == bsl::memcmp(chaste,
                                                             buffer + k + 1,
                                                             BUFLEN - k - 1));
                                }
                            }

                            // 'generate' to a 'bsl::string'
                            {
                                bsl::string mS("qwerty");

                                ASSERTV(ILINE, OUTLEN,
                                        OUTLEN == Util::generate(&mS, X, C));

                                ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                                if (veryVerbose) { P_(EXPECTED) P(mS); }
                            }

                            // 'generate' to an 'std::string'
                            {
                                std::string mS("qwerty");

                                ASSERTV(ILINE, OUTLEN,
                                        OUTLEN == Util::generate(&mS, X, C));

                                ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                                if (veryVerbose) { P_(EXPECTED) P(mS); }
                            }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                            // 'generate' to an 'std::pmr::string'
                            {
                                std::pmr::string mS("qwerty");

                                ASSERTV(ILINE, OUTLEN,
                                        OUTLEN == Util::generate(&mS, X, C));

                                ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                                if (veryVerbose) { P_(EXPECTED) P(mS); }
                            }
#endif

                            // 'generate' to an 'ostream'
                            {
                                bsl::ostringstream os;

                                ASSERTV(ILINE,
                                        &os == &Util::generate(os, X, C));

                                ASSERTV(ILINE, EXPECTED, os.str(),
                                        EXPECTED == os.str());

                                if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                            }

                            // 'generateRaw'
                            {
                                bsl::memset(buffer, '?', BUFLEN);

                                ASSERTV(ILINE,
                                        OUTLEN,
                                        OUTLEN ==
                                            Util::generateRaw(buffer, X, C));

                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(EXPECTED.c_str(),
                                                         buffer,
                                                         OUTLEN));

                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + OUTLEN,
                                                         BUFLEN - OUTLEN));
                            }
                        }  // loop over 'OBJ_TYPE' ('Datetime' or 'DatetimeTz')
                    }  // loop over 'CNFG_DATA'
                }  // loop over 'ZONE_DATA'
            }  // loop over 'TIME_DATA'
        }  // loop over 'DATE_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                TYPE                  mX;
                const TYPE&           X = mX;
                char                  buffer[OBJLEN];
                char                 *pc = 0;
                const bdlt::Datetime  DATETIME;
                mX = DATETIME;

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X));
                ASSERT_SAFE_FAIL(Util::generate(    pc, OBJLEN, X));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X));
                ASSERT_SAFE_FAIL(Util::generate(buffer,     -1, X));

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X, C));
                ASSERT_FAIL(     Util::generate(    pc, OBJLEN, X, C));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X, C));
                ASSERT_FAIL(     Util::generate(buffer,     -1, X, C));

                bsl::string mSB("qwerty");
                bsl::string *pb = 0;

                ASSERT_PASS(Util::generate(&mSB, X));
                ASSERT_FAIL(Util::generate(  pb, X));

                ASSERT_PASS(Util::generate(&mSB, X, C));
                ASSERT_FAIL(Util::generate(  pb, X, C));

                std::string mSS("qwerty");
                std::string *ps = 0;

                ASSERT_PASS(Util::generate(&mSS, X));
                ASSERT_FAIL(Util::generate(  ps, X));

                ASSERT_PASS(Util::generate(&mSS, X, C));
                ASSERT_FAIL(Util::generate(  ps, X, C));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::string mSP("qwerty");
                std::pmr::string *pp = 0;

                ASSERT_PASS(Util::generate(&mSP, X));
                ASSERT_FAIL(Util::generate(  pp, X));

                ASSERT_PASS(Util::generate(&mSP, X, C));
                ASSERT_FAIL(Util::generate(  pp, X, C));
#endif
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                TYPE                  mX;
                const TYPE&           X = mX;
                char                  buffer[OBJLEN];
                char                 *pc = 0;
                const bdlt::Datetime  DATETIME;
                mX = DATETIME;

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(    pc, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_FAIL(     Util::generateRaw(    pc, X, C));
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // GENERATE 'TimeTz'
        //
        // Concerns:
        //: 1 The output generated by each method has the expected format and
        //:   contents.
        //:
        //: 2 When sufficient capacity is indicated, the method taking
        //:   'bufferLength' generates a null terminator.
        //:
        //: 3 Each method returns the expected value (the correct character
        //:   count or the supplied 'ostream', depending on the return type).
        //:
        //: 4 The value of the supplied object is unchanged.
        //:
        //: 5 The configuration that is in effect, whether user-supplied or the
        //:   process-wide default, has the desired effect on the output.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a 'Variant2<Time, TimeTz>' object, 'X'.
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   'Time' values (one per row) and their corresponding ISO 8601
        //:   string representations.
        //:
        //: 3 For each row 'R1' in the table from P-2 create a 'const' 'Time'
        //:   object, 'T1', from 'R1'.
        //:
        //: 4 In a second table, specify a set of distinct timezone values (one
        //:   per row) and their corresponding ISO 8601 string representations.
        //:
        //: 5 For each element 'R2' in the cross product of the tables from P-2
        //:   and P-4:
        //:
        //:   1 Create a 'const' 'TimeTz' object, 'T2', from 'R2'.
        //:
        //:   2 Assign 'T1' to 'X'.
        //:
        //:   3 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R1' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.
        //:
        //:   4 Assign 'T2' to 'X'.
        //:
        //:   5 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R2' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, int, const TimeOrTimeTzTz&);
        //   int generate(char *, int, const TimeOrTimeTzTz&, const Config&);
        //   int generate(string *, const TimeOrTimeTzTz&);
        //   int generate(string *, const TimeOrTimeTzTz&, const Config&);
        //   ostream generate(ostream&, const TimeOrTimeTzTz&);
        //   ostream generate(ostream&, const TimeOrTimeTzTz&, const Config&);
        //   int generateRaw(char *, const TimeOrTimeTzTz&);
        //   int generateRaw(char *, const TimeOrTimeTzTz&, const Config&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATE 'TimeOrTimeTz'" << endl
                          << "=======================" << endl;

        typedef Util::TimeOrTimeTz TYPE;

        TYPE        mX;
        const TYPE& X = mX;

        const int OBJLEN = Util::k_TIMETZ_STRLEN;
        const int BUFLEN = OBJLEN + 4;

        char buffer[BUFLEN];
        char chaste[BUFLEN];  bsl::memset(chaste, '?', BUFLEN);

        const int                  NUM_TIME_DATA =       NUM_DEFAULT_TIME_DATA;
        const DefaultTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] =
                                                             DEFAULT_TIME_DATA;

        const int                  NUM_ZONE_DATA =       NUM_DEFAULT_ZONE_DATA;
        const DefaultZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] =
                                                             DEFAULT_ZONE_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        for (int ti = 0; ti < NUM_TIME_DATA; ++ti) {
            const int   ILINE   = TIME_DATA[ti].d_line;
            const int   HOUR    = TIME_DATA[ti].d_hour;
            const int   MIN     = TIME_DATA[ti].d_min;
            const int   SEC     = TIME_DATA[ti].d_sec;
            const int   MSEC    = TIME_DATA[ti].d_msec;
            const int   USEC    = TIME_DATA[ti].d_usec;
            const char *ISO8601 = TIME_DATA[ti].d_iso8601;

            const bdlt::Time  TIME(HOUR, MIN, SEC, MSEC, USEC);
            const bsl::string EXPECTED_TIME_BASE(ISO8601);

            for (int tj = 0; tj < NUM_ZONE_DATA; ++tj) {
                const int   JLINE   = ZONE_DATA[tj].d_line;
                const int   OFFSET  = ZONE_DATA[tj].d_offset;
                const char *ISO8601 = ZONE_DATA[tj].d_iso8601;

                const bsl::string EXPECTED_ZONE(ISO8601);

                if (TIME == bdlt::Time() && OFFSET != 0) {
                    continue;  // skip invalid compositions
                }

                const bdlt::TimeTz TIMETZ(TIME, OFFSET);
                const bsl::string  EXPECTED_TIMETZ_BASE(EXPECTED_TIME_BASE +
                                                        EXPECTED_ZONE);

                if (veryVerbose) {
                    T_ P_(ILINE) P_(JLINE) P_(TIME) P_(TIMETZ)
                       P_(EXPECTED_TIME_BASE) P(EXPECTED_TIMETZ_BASE)
                }

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    Config::setDefaultConfiguration(C);

                    bsl::string EXPECTED_TIME(EXPECTED_TIME_BASE);
                    updateExpectedPerConfig(&EXPECTED_TIME,
                                            C,
                                            k_TIME_MAX_PRECISION);
                    bsl::string EXPECTED_TIMETZ(EXPECTED_TIMETZ_BASE);
                    updateExpectedPerConfig(&EXPECTED_TIMETZ,
                                            C,
                                            k_TIMETZ_MAX_PRECISION);

                    for (int tt = 0; tt < 2; ++tt) {
                        const int OBJ_TYPE = tt;
                        if (OBJ_TYPE) {
                            mX = TIMETZ;
                        }
                        else {
                            mX = TIME;
                        }

                        const bsl::string& EXPECTED =
                            OBJ_TYPE ? EXPECTED_TIMETZ : EXPECTED_TIME;
                        const int          OUTLEN =
                                           static_cast<int>(EXPECTED.length());

                        // 'generate' taking 'bufferLength'

                        for (int k = 0; k < BUFLEN; ++k) {
                            bsl::memset(buffer, '?', BUFLEN);

                            if (veryVeryVerbose) {
                                T_ T_ cout << "Length: "; P(k)
                            }

                            ASSERTV(ILINE, JLINE, k, OUTLEN,
                                    OUTLEN == Util::generate(buffer, k, X));

                            ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                            if (k <= OUTLEN) {
                                ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k,
                                                         BUFLEN - k));
                            }
                            else {
                                ASSERTV(ILINE, JLINE, k, OUTLEN,
                                        '\0' == buffer[OUTLEN]);

                                ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k + 1,
                                                         BUFLEN - k - 1));
                            }
                        }

                        // 'generate' to a 'bsl::string'
                        {
                            bsl::string mS("qwerty");

                            ASSERTV(ILINE, JLINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X));

                            ASSERTV(
                                ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }

                        // 'generate' to an 'std::string'
                        {
                            std::string mS("qwerty");

                            ASSERTV(ILINE, JLINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X));

                            ASSERTV(
                                ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                        // 'generate' to a 'std::pmr::string'
                        {
                            std::pmr::string mS("qwerty");

                            ASSERTV(ILINE, JLINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X));

                            ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }
#endif

                        // 'generate' to an 'ostream'
                        {
                            bsl::ostringstream os;

                            ASSERTV(
                                ILINE, JLINE, &os == &Util::generate(os, X));

                            ASSERTV(ILINE, JLINE, EXPECTED, os.str(),
                                    EXPECTED == os.str());

                            if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                        }

                        // 'generateRaw'
                        {
                            bsl::memset(buffer, '?', BUFLEN);

                            ASSERTV(ILINE, JLINE, OUTLEN,
                                    OUTLEN == Util::generateRaw(buffer, X));

                            ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     OUTLEN));

                            ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + OUTLEN,
                                                     BUFLEN - OUTLEN));
                        }
                    }  // loop over 'OBJ_TYPE' ('Time' or 'TimeTz')
                }  // loop over 'CNFG_DATA'

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    // Set the default configuration to the complement of 'C'.

                    Config mDFLT;  const Config& DFLT = mDFLT;
                    gg(&mDFLT, 9 - PRECISION, !OMITCOLON, !USECOMMA, !USEZ);
                    Config::setDefaultConfiguration(DFLT);

                    bsl::string EXPECTED_TIME(EXPECTED_TIME_BASE);
                    updateExpectedPerConfig(&EXPECTED_TIME,
                                            C,
                                            k_TIME_MAX_PRECISION);
                    bsl::string EXPECTED_TIMETZ(EXPECTED_TIMETZ_BASE);
                    updateExpectedPerConfig(&EXPECTED_TIMETZ,
                                            C,
                                            k_TIMETZ_MAX_PRECISION);

                    for (int tt = 0; tt < 2; ++tt) {
                        const int OBJ_TYPE = tt;
                        if (OBJ_TYPE) {
                            mX = TIMETZ;
                        }
                        else {
                            mX = TIME;
                        }

                        const bsl::string& EXPECTED =
                            OBJ_TYPE ? EXPECTED_TIMETZ : EXPECTED_TIME;
                        const int          OUTLEN =
                                           static_cast<int>(EXPECTED.length());

                        // 'generate' taking 'bufferLength'

                        for (int k = 0; k < BUFLEN; ++k) {
                            bsl::memset(buffer, '?', BUFLEN);

                            if (veryVeryVerbose) {
                                T_ T_ cout << "Length: "; P(k)
                            }

                            ASSERTV(ILINE, k, OUTLEN,
                                    OUTLEN == Util::generate(buffer, k, X, C));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                            if (k <= OUTLEN) {
                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k,
                                                         BUFLEN - k));
                            }
                            else {
                                ASSERTV(
                                    ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k + 1,
                                                         BUFLEN - k - 1));
                            }
                        }

                        // 'generate' to a 'bsl::string'
                        {
                            bsl::string mS("qwerty");

                            ASSERTV(ILINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X, C));

                            ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }

                        // 'generate' to an 'std::string'
                        {
                            std::string mS("qwerty");

                            ASSERTV(ILINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X, C));

                            ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                        // 'generate' to an 'std::pmr::string'
                        {
                            std::pmr::string mS("qwerty");

                            ASSERTV(ILINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X, C));

                            ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }
#endif

                        // 'generate' to an 'ostream'
                        {
                            bsl::ostringstream os;

                            ASSERTV(ILINE, &os == &Util::generate(os, X, C));

                            ASSERTV(ILINE, EXPECTED, os.str(),
                                    EXPECTED == os.str());

                            if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                        }

                        // 'generateRaw'
                        {
                            bsl::memset(buffer, '?', BUFLEN);

                            ASSERTV(ILINE, OUTLEN,
                                    OUTLEN == Util::generateRaw(buffer, X, C));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     OUTLEN));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + OUTLEN,
                                                     BUFLEN - OUTLEN));
                        }
                    }  // loop over 'OBJ_TYPE' ('Time' or 'TimeTz')
                }  // loop over 'CNFG_DATA'
            }  // loop over 'ZONE_DATA'
        }  // loop over 'TIME_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                TYPE              mX;
                const TYPE&       X = mX;
                char              buffer[OBJLEN];
                char             *pc = 0;
                const bdlt::Time  TIME;
                mX = TIME;

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X));
                ASSERT_SAFE_FAIL(Util::generate(    pc, OBJLEN, X));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X));
                ASSERT_SAFE_FAIL(Util::generate(buffer,     -1, X));

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X, C));
                ASSERT_FAIL(     Util::generate(    pc, OBJLEN, X, C));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X, C));
                ASSERT_FAIL(     Util::generate(buffer,     -1, X, C));

                bsl::string  mB("qwerty");
                bsl::string *pb = 0;

                ASSERT_PASS(Util::generate(&mB, X));
                ASSERT_FAIL(Util::generate( pb, X));

                ASSERT_PASS(Util::generate(&mB, X, C));
                ASSERT_FAIL(Util::generate( pb, X, C));

                std::string  mS("qwerty");
                std::string *ps = 0;

                ASSERT_PASS(Util::generate(&mS, X));
                ASSERT_FAIL(Util::generate( ps, X));

                ASSERT_PASS(Util::generate(&mS, X, C));
                ASSERT_FAIL(Util::generate( ps, X, C));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::string  mP("qwerty");
                std::pmr::string *pp = 0;

                ASSERT_PASS(Util::generate(&mP, X));
                ASSERT_FAIL(Util::generate( pp, X));

                ASSERT_PASS(Util::generate(&mP, X, C));
                ASSERT_FAIL(Util::generate( pp, X, C));
#endif
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                TYPE              mX;
                const TYPE&       X = mX;
                char              buffer[OBJLEN];
                char             *pc = 0;
                const bdlt::Time  TIME;
                mX = TIME;

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(    pc, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_FAIL(     Util::generateRaw(    pc, X, C));
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // GENERATE 'DateOrDateTz'
        //
        // Concerns:
        //: 1 The output generated by each method has the expected format and
        //:   contents.
        //:
        //: 2 When sufficient capacity is indicated, the method taking
        //:   'bufferLength' generates a null terminator.
        //:
        //: 3 Each method returns the expected value (the correct character
        //:   count or the supplied 'ostream', depending on the return type).
        //:
        //: 4 The value of the supplied object is unchanged.
        //:
        //: 5 The configuration that is in effect, whether user-supplied or the
        //:   process-wide default, has the desired effect on the output.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create a 'Variant2<Date, DateTz>' object, 'X'.
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   'Date' values (one per row) and their corresponding ISO 8601
        //:   string representations.
        //:
        //: 3 For each row 'R1' in the table from P-2 create a 'const' 'Date'
        //:   object, 'D1', from 'R1'.
        //:
        //: 4 In a second table, specify a set of distinct timezone values (one
        //:   per row) and their corresponding ISO 8601 string representations.
        //:
        //: 5 For each element 'R2' in the cross product of the tables from P-2
        //:   and P-4:
        //:
        //:   1 Create a 'const' 'DateTz' object, 'D2', from 'R2'.
        //:
        //:   2 Assign 'D1' to 'X'.
        //:
        //:   3 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R1' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.
        //:
        //:   4 Assign 'D2' to 'X'.
        //:
        //:   5 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R2' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, int, const DateOrDateTz&);
        //   int generate(char *, int, const DateOrDateTz&, const Config&);
        //   int generate(string *, const DateOrDateTz&);
        //   int generate(string *, const DateOrDateTz&, const Config&);
        //   ostream generate(ostream&, const DateOrDateTz&);
        //   ostream generate(ostream&, const DateOrDateTz&, const Config&);
        //   int generateRaw(char *, const DateOrDateTz&);
        //   int generateRaw(char *, const DateOrDateTz&, const Config&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATE 'DateOrDateTz'" << endl
                          << "=======================" << endl;

        typedef Util::DateOrDateTz TYPE;

        TYPE        mX;
        const TYPE& X = mX;

        const int OBJLEN = Util::k_DATETZ_STRLEN;
        const int BUFLEN = OBJLEN + 4;

        char buffer[BUFLEN];
        char chaste[BUFLEN];  bsl::memset(chaste, '?', BUFLEN);

        const int                  NUM_DATE_DATA =       NUM_DEFAULT_DATE_DATA;
        const DefaultDateDataRow (&DATE_DATA)[NUM_DATE_DATA] =
                                                             DEFAULT_DATE_DATA;

        const int                  NUM_ZONE_DATA =       NUM_DEFAULT_ZONE_DATA;
        const DefaultZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] =
                                                             DEFAULT_ZONE_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
            const int   ILINE   = DATE_DATA[ti].d_line;
            const int   YEAR    = DATE_DATA[ti].d_year;
            const int   MONTH   = DATE_DATA[ti].d_month;
            const int   DAY     = DATE_DATA[ti].d_day;
            const char *ISO8601 = DATE_DATA[ti].d_iso8601;

            const bdlt::Date  DATE(YEAR, MONTH, DAY);
            const bsl::string EXPECTED_DATE_BASE(ISO8601);

            for (int tj = 0; tj < NUM_ZONE_DATA; ++tj) {
                const int   JLINE   = ZONE_DATA[tj].d_line;
                const int   OFFSET  = ZONE_DATA[tj].d_offset;
                const char *ISO8601 = ZONE_DATA[tj].d_iso8601;

                const bsl::string EXPECTED_ZONE(ISO8601);

                const bdlt::DateTz DATETZ(DATE, OFFSET);
                const bsl::string  EXPECTED_DATETZ_BASE(EXPECTED_DATE_BASE +
                                                        EXPECTED_ZONE);

                if (veryVerbose) {
                    T_ P_(ILINE) P_(JLINE) P_(DATE) P_(DATETZ)
                       P_(EXPECTED_DATE_BASE) P(EXPECTED_DATETZ_BASE)
                }

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    Config::setDefaultConfiguration(C);

                    bsl::string EXPECTED_DATE(EXPECTED_DATE_BASE);
                    updateExpectedPerConfig(&EXPECTED_DATE,
                                            C,
                                            k_DATE_MAX_PRECISION);
                    bsl::string EXPECTED_DATETZ(EXPECTED_DATETZ_BASE);
                    updateExpectedPerConfig(&EXPECTED_DATETZ,
                                            C,
                                            k_DATETZ_MAX_PRECISION);

                    for (int tt = 0; tt < 2; ++tt) {
                        const int OBJ_TYPE = tt;
                        if (OBJ_TYPE) {
                            mX = DATETZ;
                        }
                        else {
                            mX = DATE;
                        }

                        const bsl::string& EXPECTED =
                            OBJ_TYPE ? EXPECTED_DATETZ : EXPECTED_DATE;
                        const int          OUTLEN =
                                           static_cast<int>(EXPECTED.length());


                        // 'generate' taking 'bufferLength'

                        for (int k = 0; k < BUFLEN; ++k) {
                            bsl::memset(buffer, '?', BUFLEN);

                            if (veryVeryVerbose) {
                                T_ T_ cout << "Length: "; P(k)
                            }

                            ASSERTV(ILINE, JLINE, k, OUTLEN,
                                    OUTLEN == Util::generate(buffer, k, X));

                            ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                            if (k <= OUTLEN) {
                                ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k,
                                                         BUFLEN - k));
                            }
                            else {
                                ASSERTV(ILINE, JLINE, k, OUTLEN,
                                        '\0' == buffer[OUTLEN]);

                                ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k + 1,
                                                         BUFLEN - k - 1));
                            }

                        }

                        // 'generate' to a 'bsl::string'
                        {
                            bsl::string mS("qwerty");

                            ASSERTV(ILINE, JLINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X));

                            ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }

                        // 'generate' to an 'std::string'
                        {
                            std::string mS("qwerty");

                            ASSERTV(ILINE, JLINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X));

                            ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                        // 'generate' to an 'std::pmr::string'
                        {
                            std::pmr::string mS("qwerty");

                            ASSERTV(ILINE, JLINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X));

                            ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }
#endif

                        // 'generate' to an 'ostream'
                        {
                            bsl::ostringstream os;

                            ASSERTV(
                                ILINE, JLINE, &os == &Util::generate(os, X));

                            ASSERTV(ILINE, JLINE, EXPECTED, os.str(),
                                    EXPECTED == os.str());

                            if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                        }

                        // 'generateRaw'
                        {
                            bsl::memset(buffer, '?', BUFLEN);

                            ASSERTV(ILINE, JLINE, OUTLEN,
                                    OUTLEN == Util::generateRaw(buffer, X));

                            ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     OUTLEN));

                            ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + OUTLEN,
                                                     BUFLEN - OUTLEN));
                        }
                    }  // loop over 'OBJ_TYPE' ('Date' or 'DateTz')
                }  // loop over 'CNFG_DATA'

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    // Set the default configuration to the complement of 'C'.

                    Config mDFLT;  const Config& DFLT = mDFLT;
                    gg(&mDFLT, 9 - PRECISION, !OMITCOLON, !USECOMMA, !USEZ);
                    Config::setDefaultConfiguration(DFLT);

                    bsl::string EXPECTED_DATE(EXPECTED_DATE_BASE);
                    updateExpectedPerConfig(&EXPECTED_DATE,
                                            C,
                                            k_DATE_MAX_PRECISION);
                    bsl::string EXPECTED_DATETZ(EXPECTED_DATETZ_BASE);
                    updateExpectedPerConfig(&EXPECTED_DATETZ,
                                            C,
                                            k_DATETZ_MAX_PRECISION);

                    for (int tt = 0; tt < 2; ++tt) {
                        const int OBJ_TYPE = tt;
                        if (OBJ_TYPE) {
                            mX = DATETZ;
                        }
                        else {
                            mX = DATE;
                        }

                        const bsl::string& EXPECTED =
                            OBJ_TYPE ? EXPECTED_DATETZ : EXPECTED_DATE;
                        const int          OUTLEN =
                                           static_cast<int>(EXPECTED.length());

                        // 'generate' taking 'bufferLength'

                        for (int k = 0; k < BUFLEN; ++k) {
                            bsl::memset(buffer, '?', BUFLEN);

                            if (veryVeryVerbose) {
                                T_ T_ cout << "Length: "; P(k)
                            }

                            ASSERTV(ILINE, k, OUTLEN,
                                    OUTLEN == Util::generate(buffer, k, X, C));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                            if (k <= OUTLEN) {
                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k,
                                                         BUFLEN - k));
                            }
                            else {
                                ASSERTV(
                                    ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k + 1,
                                                         BUFLEN - k - 1));
                            }
                        }

                        // 'generate' to a 'bsl::string'
                        {
                            bsl::string mS("qwerty");

                            ASSERTV(ILINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X, C));

                            ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }

                        // 'generate' to an 'std::string'
                        {
                            std::string mS("qwerty");

                            ASSERTV(ILINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X, C));

                            ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                        // 'generate' to an 'std::pmr::string'
                        {
                            std::pmr::string mS("qwerty");

                            ASSERTV(ILINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X, C));

                            ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }
#endif

                        // 'generate' to an 'ostream'
                        {
                            bsl::ostringstream os;

                            ASSERTV(ILINE, &os == &Util::generate(os, X, C));

                            ASSERTV(ILINE, EXPECTED, os.str(),
                                    EXPECTED == os.str());

                            if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                        }

                        // 'generateRaw'
                        {
                            bsl::memset(buffer, '?', BUFLEN);

                            ASSERTV(ILINE, OUTLEN,
                                    OUTLEN == Util::generateRaw(buffer, X, C));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     OUTLEN));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + OUTLEN,
                                                     BUFLEN - OUTLEN));
                        }
                    }  // loop over 'OBJ_TYPE' ('Date' or 'DateTz')
                }  // loop over 'CNFG_DATA'
            }  // loop over 'ZONE_DATA'
        }  // loop over 'DATE_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                TYPE              mX;
                const TYPE&       X = mX;
                char              buffer[OBJLEN];
                char             *pc = 0;
                const bdlt::Date  DATE;
                mX = DATE;

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X));
                ASSERT_SAFE_FAIL(Util::generate(    pc, OBJLEN, X));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X));
                ASSERT_SAFE_FAIL(Util::generate(buffer,     -1, X));

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X, C));
                ASSERT_FAIL(     Util::generate(    pc, OBJLEN, X, C));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X, C));
                ASSERT_FAIL(     Util::generate(buffer,     -1, X, C));

                bsl::string  mB("qwerty");
                bsl::string *pb = 0;

                ASSERT_PASS(Util::generate(&mB, X));
                ASSERT_FAIL(Util::generate( pb, X));

                ASSERT_PASS(Util::generate(&mB, X, C));
                ASSERT_FAIL(Util::generate( pb, X, C));

                std::string  mS("qwerty");
                std::string *ps = 0;

                ASSERT_PASS(Util::generate(&mS, X));
                ASSERT_FAIL(Util::generate( ps, X));

                ASSERT_PASS(Util::generate(&mS, X, C));
                ASSERT_FAIL(Util::generate( ps, X, C));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::string  mP("qwerty");
                std::pmr::string *pp = 0;

                ASSERT_PASS(Util::generate(&mP, X));
                ASSERT_FAIL(Util::generate( pp, X));

                ASSERT_PASS(Util::generate(&mP, X, C));
                ASSERT_FAIL(Util::generate( pp, X, C));
#endif
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
               TYPE               mX;
                const TYPE&       X = mX;
                char              buffer[OBJLEN];
                char             *pc = 0;
                const bdlt::Date  DATE;
                mX = DATE;

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(    pc, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_FAIL(     Util::generateRaw(    pc, X, C));
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // PARSE: DATETIME & DATETIMETZ
        //
        // Concerns:
        //: 1 All ISO 8601 string representations supported by this component
        //:   (as documented in the header file) for 'Datetime' and
        //:   'DatetimeTz' values are parsed successfully.
        //:
        //: 2 If parsing succeeds, the result 'Datetime' or 'DatetimeTz' object
        //:   has the expected value.
        //:
        //: 3 If the optional zone designator is present in the input string
        //:   when parsing into a 'Datetime' object, the resulting value is
        //:   converted to the equivalent UTC datetime.
        //:
        //: 4 If the optional zone designator is *not* present in the input
        //:   string when parsing into a 'DatetimeTz' object, it is assumed to
        //:   be UTC.
        //:
        //: 5 If parsing succeeds, 0 is returned.
        //:
        //: 6 All strings that are not ISO 8601 representations supported by
        //:   this component for 'Datetime' and 'DatetimeTz' values are
        //:   rejected (i.e., parsing fails).
        //:
        //: 7 If parsing fails, the result object is unaffected and a non-zero
        //:   value is returned.
        //:
        //: 8 The entire extent of the input string is parsed.
        //:
        //: 9 Leap seconds, fractional seconds containing more than three
        //:   digits, and extremal values (those that can overflow a
        //:   'Datetime') are handled correctly.
        //:
        //:10 The 'parseRelaxed' functions do the same as the 'parse' functions
        //:   and additionally allow to use a SPACE characters instead of 'T'.
        //:
        //:11 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'Date' values ('D'), 'Time' values ('T'), zone designators ('Z'),
        //:   and configurations ('C').
        //:
        //: 2 Apply the (fully-tested) 'generateRaw' functions to each element
        //:   in the cross product, 'D x T x Z x C', of the test data from P-1.
        //:
        //: 3 Invoke the 'parse' functions on the strings generated in P-2 and
        //:   verify that parsing succeeds, i.e., that 0 is returned and the
        //:   result objects have the expected values.  (C-1..5)
        //:
        //: 4 Using the table-driven technique, specify a set of distinct
        //:   strings that are not ISO 8601 representations supported by this
        //:   component for 'Datetime' and 'DatetimeTz' values.
        //:
        //: 5 Invoke the 'parse' functions on the strings from P-4 and verify
        //:   that parsing fails, i.e., that a non-zero value is returned and
        //:   the result objects are unchanged.  (C-6..8)
        //:
        //: 6 Using the table-driven technique, specify a set of distinct ISO
        //:   8601 strings that specifically cover cases involving leap
        //:   seconds, fractional seconds containing more than three digits,
        //:   and extremal values.
        //:
        //: 7 Invoke the 'parse' functions on the strings from P-6 and verify
        //:   the results are as expected.  (C-9)
        //:
        //: 8 Invoke the 'parseRelaxed' functions on the same strings and on
        //:   the strings with 'T' character replaced by ' '. (C-10)
        //:
        //: 9 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-11)
        //
        // Testing:
        //   int parse(Datetime *, const char *, int);
        //   int parse(DatetimeTz *, const char *, int);
        //   int parse(Datetime *result, const StringRef& string);
        //   int parse(DatetimeTz *result, const StringRef& string);
        //   int parseRelaxed(Datetime *, const char *, int);
        //   int parseRelaxed(DatetimeTz *, const char *, int);
        //   int parseRelaxed(Datetime *, const bsl::string_view&);
        //   int parseRelaxed(DatetimeTz *, const bsl::string_view&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PARSE: DATETIME & DATETIMETZ" << endl
                          << "============================" << endl;

        char buffer[Util::k_MAX_STRLEN];

        const bdlt::Date       DD(246, 8, 10);
        const bdlt::Time       TT(2, 4, 6, 8);

        const bdlt::Datetime   XX(DD, TT);  // 'XX' and 'ZZ' are controls,
        const bdlt::DatetimeTz ZZ(XX, -7);  // distinct from any test data

        const int                  NUM_DATE_DATA =       NUM_DEFAULT_DATE_DATA;
        const DefaultDateDataRow (&DATE_DATA)[NUM_DATE_DATA] =
                                                             DEFAULT_DATE_DATA;

        const int                  NUM_TIME_DATA =       NUM_DEFAULT_TIME_DATA;
        const DefaultTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] =
                                                             DEFAULT_TIME_DATA;

        const int                  NUM_ZONE_DATA =       NUM_DEFAULT_ZONE_DATA;
        const DefaultZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] =
                                                             DEFAULT_ZONE_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        if (verbose) cout << "\nValid ISO 8601 strings." << endl;

        for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
            const int ILINE = DATE_DATA[ti].d_line;
            const int YEAR  = DATE_DATA[ti].d_year;
            const int MONTH = DATE_DATA[ti].d_month;
            const int DAY   = DATE_DATA[ti].d_day;

            const bdlt::Date DATE(YEAR, MONTH, DAY);

            for (int tj = 0; tj < NUM_TIME_DATA; ++tj) {
                const int JLINE = TIME_DATA[tj].d_line;
                const int HOUR  = TIME_DATA[tj].d_hour;
                const int MIN   = TIME_DATA[tj].d_min;
                const int SEC   = TIME_DATA[tj].d_sec;
                const int MSEC  = TIME_DATA[tj].d_msec;
                const int USEC  = TIME_DATA[tj].d_usec;

                for (int tk = 0; tk < NUM_ZONE_DATA; ++tk) {
                    const int KLINE  = ZONE_DATA[tk].d_line;
                    const int OFFSET = ZONE_DATA[tk].d_offset;

                    if (   bdlt::Time(HOUR, MIN, SEC, MSEC, USEC)
                                                                == bdlt::Time()
                        && OFFSET != 0) {
                        continue;  // skip invalid compositions
                    }

                    for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                        const int  CLINE     = CNFG_DATA[tc].d_line;
                        const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                        const int  PRECISION = CNFG_DATA[tc].d_precision;
                        const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                        const bool USEZ      = CNFG_DATA[tc].d_useZ;

                        int expMsec = MSEC;
                        int expUsec = USEC;
                        {
                            // adjust the expected milliseconds to account for
                            // PRECISION truncating the value generated

                            int precision = (PRECISION < 3 ? PRECISION : 3);

                            for (int i = 3; i > precision; --i) {
                                expMsec /= 10;
                            }

                            for (int i = 3; i > precision; --i) {
                                expMsec *= 10;
                            }

                            // adjust the expected microseconds to account for
                            // PRECISION truncating the value generated

                            precision = (PRECISION > 3 ? PRECISION - 3: 0);

                            for (int i = 3; i > precision; --i) {
                                expUsec /= 10;
                            }

                            for (int i = 3; i > precision; --i) {
                                expUsec *= 10;
                            }
                        }

                        const bdlt::Datetime   DATETIME(YEAR,
                                                        MONTH,
                                                        DAY,
                                                        HOUR,
                                                        MIN,
                                                        SEC,
                                                        expMsec,
                                                        expUsec);
                        const bdlt::DatetimeTz DATETIMETZ(DATETIME, OFFSET);

                        if (veryVerbose) {
                            if (0 == tc) {
                                T_ P_(ILINE) P_(JLINE) P_(KLINE)
                                                    P_(DATETIME) P(DATETIMETZ);
                            }
                            T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                          P_(USECOMMA) P(USEZ);
                        }

                        Config mC;  const Config& C = mC;
                        gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                        // without zone designator in parsed string
                        {
                            const int LENGTH = Util::generateRaw(buffer,
                                                                 DATETIME,
                                                                 C);

                            if (veryVerbose) {
                                const bsl::string STRING(buffer, LENGTH);
                                T_ T_ P(STRING)
                            }

                                  bdlt::Datetime    mX(XX);
                            const bdlt::Datetime&   X = mX;

                                  bdlt::DatetimeTz  mZ(ZZ);
                            const bdlt::DatetimeTz& Z = mZ;

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parse(&mX, buffer, LENGTH));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIME == X);

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parse(&mZ, buffer, LENGTH));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIME == Z.localDatetime());
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                           0 == Z.offset());

                            mX = XX;
                            mZ = ZZ;

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parse(&mX,
                                                     StrView(buffer, LENGTH)));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIME == X);

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parse(&mZ,
                                                     StrView(buffer, LENGTH)));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIME == Z.localDatetime());
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                           0 == Z.offset());

                            // parseRelaxed() on the same string

                            mX = XX;
                            mZ = ZZ;

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mX,
                                                            buffer,
                                                            LENGTH));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIME == X);

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mZ,
                                                            buffer,
                                                            LENGTH));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIME == Z.localDatetime());
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                           0 == Z.offset());

                            mX = XX;
                            mZ = ZZ;

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(
                                                     &mX,
                                                     StrView(buffer, LENGTH)));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIME == X);

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(
                                                     &mZ,
                                                     StrView(buffer, LENGTH)));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIME == Z.localDatetime());
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                           0 == Z.offset());

                            // parseRelaxed() on the string modified to have
                            // SPACE instead of 'T'
                            const bsl::string relaxed =
                                             replaceTWithSpace(buffer, LENGTH);

                            mX = XX;
                            mZ = ZZ;

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mX,
                                                            relaxed.data(),
                                                            relaxed.length()));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIME == X);

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mZ,
                                                            relaxed.data(),
                                                            relaxed.length()));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIME == Z.localDatetime());
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                           0 == Z.offset());

                            mX = XX;
                            mZ = ZZ;

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mX,
                                                            StrView(relaxed)));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIME == X);

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mZ,
                                                            StrView(relaxed)));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIME == Z.localDatetime());
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                           0 == Z.offset());
                        }

                        // with zone designator in parsed string
                        {
                            if ((DATE == bdlt::Date() && OFFSET > 0)
                             || (DATE == bdlt::Date(9999, 12, 31)
                              && OFFSET < 0)) {
                                continue;  // skip invalid compositions
                            }

                            const int LENGTH = Util::generateRaw(buffer,
                                                                 DATETIMETZ,
                                                                 C);

                            if (veryVerbose) {
                                const bsl::string STRING(buffer, LENGTH);
                                T_ T_ P(STRING)
                            }

                                  bdlt::Datetime    mX(XX);
                            const bdlt::Datetime&   X = mX;

                                  bdlt::DatetimeTz  mZ(ZZ);
                            const bdlt::DatetimeTz& Z = mZ;

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parse(&mX, buffer, LENGTH));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIMETZ.utcDatetime() == X);

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parse(&mZ, buffer, LENGTH));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIMETZ               == Z);

                            mX = XX;
                            mZ = ZZ;

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parse(&mX,
                                                     StrView(buffer, LENGTH)));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIMETZ.utcDatetime() == X);

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parse(&mZ,
                                                     StrView(buffer, LENGTH)));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIMETZ               == Z);

                            // parseRelaxed() on the same string

                            mX = XX;
                            mZ = ZZ;

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mX,
                                                            buffer,
                                                            LENGTH));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIMETZ.utcDatetime() == X);

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mZ,
                                                            buffer,
                                                            LENGTH));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIMETZ               == Z);

                            mX = XX;
                            mZ = ZZ;

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mX,
                                                     StrView(buffer, LENGTH)));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIMETZ.utcDatetime() == X);

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mZ,
                                                     StrView(buffer, LENGTH)));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIMETZ               == Z);

                            // parseRelaxed() on the string modified to have
                            // SPACE instead of 'T'
                            const bsl::string relaxed =
                                             replaceTWithSpace(buffer, LENGTH);

                            mX = XX;
                            mZ = ZZ;

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mX,
                                                            relaxed.data(),
                                                            relaxed.length()));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIMETZ.utcDatetime() == X);

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mZ,
                                                            relaxed.data(),
                                                            relaxed.length()));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIMETZ               == Z);

                            mX = XX;
                            mZ = ZZ;

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mX,
                                                            StrView(relaxed)));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIMETZ.utcDatetime() == X);

                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    0 == Util::parseRelaxed(&mZ,
                                                            StrView(relaxed)));
                            ASSERTV(ILINE, JLINE, KLINE, CLINE,
                                    DATETIMETZ               == Z);
                        }
                    }  // loop over 'CNFG_DATA'
                }  // loop over 'ZONE_DATA'
            }  // loop over 'TIME_DATA'
        }  // loop over 'DATE_DATA'

        {
            // verify 't' and 'z' are accepted

            bdlt::Datetime   mX(XX);  const bdlt::Datetime&   X = mX;
            bdlt::DatetimeTz mZ(ZZ);  const bdlt::DatetimeTz& Z = mZ;

            ASSERT(0 == Util::parse(&mX, "0001-02-03t01:02:03z", 20));
            ASSERT(X == bdlt::Datetime(1, 2, 3, 1, 2, 3));

            ASSERT(0 == Util::parse(&mZ, "0001-02-03t01:02:03z", 20));
            ASSERT(Z == bdlt::DatetimeTz(bdlt::Datetime(1, 2, 3, 1, 2, 3), 0));
        }

        if (verbose) cout << "\nInvalid strings." << endl;
        {
            bdlt::Datetime   mX(XX);  const bdlt::Datetime&   X = mX;
            bdlt::DatetimeTz mZ(ZZ);  const bdlt::DatetimeTz& Z = mZ;

            const int              NUM_DATE_DATA =         NUM_BAD_DATE_DATA;
            const BadDateDataRow (&DATE_DATA)[NUM_DATE_DATA] = BAD_DATE_DATA;

            for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
                const int LINE = DATE_DATA[ti].d_line;

                bsl::string bad(DATE_DATA[ti].d_invalid);

                // Append a valid time.

                bad.append("T12:26:52.726");

                const char *STRING = bad.c_str();
                const int   LENGTH = static_cast<int>(bad.length());

                if (veryVerbose) { T_ P_(LINE) P(STRING) }

                ASSERTV(LINE, STRING,  0 != Util::parse(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,  0 != Util::parse(&mZ, STRING, LENGTH));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mZ, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, ZZ == Z);

                // parseRelaxed() on the same string

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mZ, STRING, LENGTH));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mX, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mZ, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, ZZ == Z);

                // parseRelaxed() on the string modified to have SPACE instead
                // of 'T'
                const bsl::string relaxed = replaceTWithSpace(STRING, LENGTH);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mX,
                                                relaxed.data(),
                                                relaxed.length()));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mZ,
                                                relaxed.data(),
                                                relaxed.length()));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mX, StrView(relaxed)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mZ, StrView(relaxed)));
                ASSERTV(LINE, STRING, ZZ == Z);
            }

            const int              NUM_TIME_DATA =         NUM_BAD_TIME_DATA;
            const BadTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] = BAD_TIME_DATA;

            for (int tj = 0; tj < NUM_TIME_DATA; ++tj) {
                const int LINE = TIME_DATA[tj].d_line;

                // Initialize with a *valid* date string, then append an
                // invalid time.

                bsl::string bad("2010-08-17");

                // Ensure that 'bad' is initially valid.

                static bool firstFlag = true;
                if (firstFlag) {
                    const char *STRING = bad.data();
                    const int   LENGTH = static_cast<int>(bad.length());

                    bdlt::Date mD(DD);  const bdlt::Date& D = mD;

                    ASSERT( 0 == Util::parse(&mD, STRING, LENGTH));
                    ASSERT(DD != D);

                    mD = DD;

                    ASSERT( 0 == Util::parse(&mD, StrView(STRING, LENGTH)));
                    ASSERT(DD != D);
                }

                bad.append("T");
                bad.append(TIME_DATA[tj].d_invalid);

                const char *STRING = bad.c_str();
                const int   LENGTH = static_cast<int>(bad.length());

                if (veryVerbose) { T_ P_(LINE) P(STRING) }

                ASSERTV(LINE, STRING,  0 != Util::parse(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,  0 != Util::parse(&mZ, STRING, LENGTH));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mZ, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, ZZ == Z);

                // parseRelaxed() on the same string

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mZ, STRING, LENGTH));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mX, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mZ, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, ZZ == Z);

                // parseRelaxed() on the string modified to have SPACE instead
                // of 'T'
                const bsl::string relaxed = replaceTWithSpace(STRING, LENGTH);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mX,
                                                relaxed.data(),
                                                relaxed.length()));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mZ,
                                                relaxed.data(),
                                                relaxed.length()));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mX, StrView(relaxed)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mZ, StrView(relaxed)));
                ASSERTV(LINE, STRING, ZZ == Z);
            }

            const int              NUM_ZONE_DATA =         NUM_BAD_ZONE_DATA;
            const BadZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] = BAD_ZONE_DATA;

            for (int tk = 0; tk < NUM_ZONE_DATA; ++tk) {
                const int LINE = ZONE_DATA[tk].d_line;

                // Initialize with a *valid* datetime string, then append an
                // invalid zone designator.

                bsl::string bad("2010-08-17T12:26:52.726");

                // Ensure that 'bad' is initially valid.

                static bool firstFlag = true;
                if (firstFlag) {
                    const char *STRING = bad.data();
                    const int   LENGTH = static_cast<int>(bad.length());

                    bdlt::Datetime mD(XX);  const bdlt::Datetime& D = mD;

                    ASSERT( 0 == Util::parse(&mD, STRING, LENGTH));
                    ASSERT(XX != D);

                    mD = XX;

                    ASSERT( 0 == Util::parse(&mD, StrView(STRING, LENGTH)));
                    ASSERT(XX != D);

                    // parseRelaxed() on the same string

                    mD = XX;

                    ASSERT( 0 == Util::parseRelaxed(&mD, STRING, LENGTH));
                    ASSERT(XX != D);

                    mD = XX;

                    ASSERT( 0 == Util::parseRelaxed(&mD,
                                                    StrView(STRING, LENGTH)));
                    ASSERT(XX != D);

                    // parseRelaxed() on the string modified to have SPACE
                    // instead of 'T'
                    const bsl::string relaxed =
                                             replaceTWithSpace(STRING, LENGTH);

                    mD = XX;

                    ASSERT( 0 == Util::parseRelaxed(&mD,
                                                    relaxed.data(),
                                                    relaxed.length()));
                    ASSERT(XX != D);

                    mD = XX;

                    ASSERT( 0 == Util::parseRelaxed(&mD, StrView(relaxed)));
                    ASSERT(XX != D);
                }

                // If 'ZONE_DATA[tk].d_invalid' contains nothing but digits,
                // appending it to 'bad' simply extends the fractional second
                // (so 'bad' remains valid).

                if (containsOnlyDigits(ZONE_DATA[tk].d_invalid)) {
                    continue;
                }

                bad.append(ZONE_DATA[tk].d_invalid);

                const char *STRING = bad.c_str();
                const int   LENGTH = static_cast<int>(bad.length());

                if (veryVerbose) { T_ P_(LINE) P(STRING) }

                ASSERTV(LINE, STRING,  0 != Util::parse(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,  0 != Util::parse(&mZ, STRING, LENGTH));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mZ, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, ZZ == Z);

                // parseRelaxed() on the same string

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mZ, STRING, LENGTH));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mX, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mZ, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, ZZ == Z);

                // parseRelaxed() on the string modified to have SPACE instead
                // of 'T'
                const bsl::string relaxed = replaceTWithSpace(STRING, LENGTH);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mX,
                                                relaxed.data(),
                                                relaxed.length()));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mZ,
                                                relaxed.data(),
                                                relaxed.length()));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mX, StrView(relaxed)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parseRelaxed(&mZ, StrView(relaxed)));
                ASSERTV(LINE, STRING, ZZ == Z);
            }
        }

        if (verbose) cout << "\nTesting leap seconds and fractional seconds."
                          << endl;
        {
            const struct {
                int         d_line;
                const char *d_input;
                int         d_year;
                int         d_month;
                int         d_day;
                int         d_hour;
                int         d_min;
                int         d_sec;
                int         d_msec;
                int         d_usec;
                int         d_offset;
            } DATA[] = {
                // leap seconds
                { L_, "0001-01-01T00:00:60.000",
                                     0001, 01, 01, 00, 01, 00, 000, 000,   0 },
                { L_, "9998-12-31T23:59:60.999",
                                     9999, 01, 01, 00, 00, 00, 999, 000,   0 },

                // fractional seconds
                { L_, "0001-01-01T00:00:00.0000001",
                                     0001, 01, 01, 00, 00, 00, 000, 000,   0 },
                { L_, "0001-01-01T00:00:00.0000009",
                                     0001, 01, 01, 00, 00, 00, 000,   1,   0 },
                { L_, "0001-01-01T00:00:00.00000001",
                                     0001, 01, 01, 00, 00, 00, 000, 000,   0 },
                { L_, "0001-01-01T00:00:00.00000049",
                                     0001, 01, 01, 00, 00, 00, 000, 000,   0 },
                { L_, "0001-01-01T00:00:00.00000050",
                                     0001, 01, 01, 00, 00, 00, 000,   1,   0 },
                { L_, "0001-01-01T00:00:00.00000099",
                                     0001, 01, 01, 00, 00, 00, 000,   1,   0 },
                { L_, "0001-01-01T00:00:00.0001",
                                     0001, 01, 01, 00, 00, 00, 000, 100,   0 },
                { L_, "0001-01-01T00:00:00.0009",
                                     0001, 01, 01, 00, 00, 00, 000, 900,   0 },
                { L_, "0001-01-01T00:00:00.00001",
                                     0001, 01, 01, 00, 00, 00, 000,  10,   0 },
                { L_, "0001-01-01T00:00:00.00049",
                                     0001, 01, 01, 00, 00, 00, 000, 490,   0 },
                { L_, "0001-01-01T00:00:00.00050",
                                     0001, 01, 01, 00, 00, 00, 000, 500,   0 },
                { L_, "0001-01-01T00:00:00.00099",
                                     0001, 01, 01, 00, 00, 00, 000, 990,   0 },
                { L_, "0001-01-01T00:00:00.9994" ,
                                     0001, 01, 01, 00, 00, 00, 999, 400,   0 },
                { L_, "0001-01-01T00:00:00.9995" ,
                                     0001, 01, 01, 00, 00, 00, 999, 500,   0 },
                { L_, "0001-01-01T00:00:00.9999" ,
                                     0001, 01, 01, 00, 00, 00, 999, 900,   0 },
                { L_, "9998-12-31T23:59:60.9999" ,
                                     9999, 01, 01, 00, 00, 00, 999, 900,   0 },
                { L_, "0001-01-01T00:00:00.9999994" ,
                                     0001, 01, 01, 00, 00, 00, 999, 999,   0 },
                { L_, "0001-01-01T00:00:00.9999995" ,
                                     0001, 01, 01, 00, 00, 01, 000, 000,   0 },
                { L_, "0001-01-01T00:00:00.9999999" ,
                                     0001, 01, 01, 00, 00, 01, 000, 000,   0 },
                { L_, "9998-12-31T23:59:60.9999999" ,
                                     9999, 01, 01, 00, 00, 01, 000, 000,   0 },

                // omit fractional seconds
                { L_, "2014-12-23T12:34:45",
                                     2014, 12, 23, 12, 34, 45, 000, 000,   0 },
                { L_, "2014-12-23T12:34:45Z",
                                     2014, 12, 23, 12, 34, 45, 000, 000,   0 },
                { L_, "2014-12-23T12:34:45+00:30",
                                     2014, 12, 23, 12, 34, 45, 000, 000,  30 },
                { L_, "2014-12-23T12:34:45-01:30",
                                     2014, 12, 23, 12, 34, 45, 000, 000, -90 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_line;
                const char *INPUT  = DATA[ti].d_input;
                const int   LENGTH = static_cast<int>(bsl::strlen(INPUT));
                const int   YEAR   = DATA[ti].d_year;
                const int   MONTH  = DATA[ti].d_month;
                const int   DAY    = DATA[ti].d_day;
                const int   HOUR   = DATA[ti].d_hour;
                const int   MIN    = DATA[ti].d_min;
                const int   SEC    = DATA[ti].d_sec;
                const int   MSEC   = DATA[ti].d_msec;
                const int   USEC   = DATA[ti].d_usec;
                const int   OFFSET = DATA[ti].d_offset;

                if (veryVerbose) { T_ P_(LINE) P(INPUT) }

                bdlt::Datetime   mX(XX);  const bdlt::Datetime&   X = mX;
                bdlt::DatetimeTz mZ(ZZ);  const bdlt::DatetimeTz& Z = mZ;

                bdlt::DatetimeTz EXPECTED(bdlt::Datetime(YEAR,
                                                         MONTH,
                                                         DAY,
                                                         HOUR,
                                                         MIN,
                                                         SEC,
                                                         MSEC,
                                                         USEC),
                                          OFFSET);

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parse(&mX, INPUT, LENGTH));
                ASSERTV(LINE, EXPECTED, X, EXPECTED.utcDatetime() == X);

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parse(&mZ, INPUT, LENGTH));
                ASSERTV(LINE, EXPECTED, Z, EXPECTED == Z);

                mX = XX;
                mZ = ZZ;

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parse(&mX, StrView(INPUT, LENGTH)));
                ASSERTV(LINE, EXPECTED, X, EXPECTED.utcDatetime() == X);

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parse(&mZ, StrView(INPUT, LENGTH)));
                ASSERTV(LINE, EXPECTED, Z, EXPECTED == Z);

                // parseRelaxed() on the same string

                mX = XX;
                mZ = ZZ;

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parseRelaxed(&mX, INPUT, LENGTH));
                ASSERTV(LINE, EXPECTED, X, EXPECTED.utcDatetime() == X);

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parseRelaxed(&mZ, INPUT, LENGTH));
                ASSERTV(LINE, EXPECTED, Z, EXPECTED == Z);

                mX = XX;
                mZ = ZZ;

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parseRelaxed(&mX, StrView(INPUT, LENGTH)));
                ASSERTV(LINE, EXPECTED, X, EXPECTED.utcDatetime() == X);

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parseRelaxed(&mZ, StrView(INPUT, LENGTH)));
                ASSERTV(LINE, EXPECTED, Z, EXPECTED == Z);

                // parseRelaxed() on the string modified to have SPACE instead
                // of 'T'
                const bsl::string relaxed = replaceTWithSpace(INPUT, LENGTH);

                mX = XX;
                mZ = ZZ;

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parseRelaxed(&mX,
                                                relaxed.data(),
                                                relaxed.length()));
                ASSERTV(LINE, EXPECTED, X, EXPECTED.utcDatetime() == X);

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parseRelaxed(&mZ,
                                                relaxed.data(),
                                                relaxed.length()));
                ASSERTV(LINE, EXPECTED, Z, EXPECTED == Z);

                mX = XX;
                mZ = ZZ;

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parseRelaxed(&mX, StrView(relaxed)));
                ASSERTV(LINE, EXPECTED, X, EXPECTED.utcDatetime() == X);

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parseRelaxed(&mZ, StrView(relaxed)));
                ASSERTV(LINE, EXPECTED, Z, EXPECTED == Z);
            }
        }

        if (verbose)
            cout << "\nTesting zone designators that overflow a 'Datetime'."
                 << endl;
        {
            struct {
                int         d_line;
                const char *d_input;
                int         d_year;
                int         d_month;
                int         d_day;
                int         d_hour;
                int         d_min;
                int         d_sec;
                int         d_msec;
                int         d_offset;
            } DATA[] = {
                { L_, "0001-01-01T00:00:00.000+00:00",
                                        0001, 01, 01, 00, 00, 00, 000,     0 },
                { L_, "0001-01-01T00:00:00.000+00:01",
                                        0001, 01, 01, 00, 00, 00, 000,     1 },
                { L_, "0001-01-01T23:58:59.000+23:59",
                                        0001, 01, 01, 23, 58, 59, 000,  1439 },

                { L_, "9999-12-31T23:59:59.999+00:00",
                                        9999, 12, 31, 23, 59, 59, 999,     0 },
                { L_, "9999-12-31T23:59:59.999-00:01",
                                        9999, 12, 31, 23, 59, 59, 999,    -1 },
                { L_, "9999-12-31T00:01:00.000-23:59",
                                        9999, 12, 31, 00, 01, 00, 000, -1439 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_line;
                const char *INPUT  = DATA[ti].d_input;
                const int   LENGTH = static_cast<int>(bsl::strlen(INPUT));
                const int   YEAR   = DATA[ti].d_year;
                const int   MONTH  = DATA[ti].d_month;
                const int   DAY    = DATA[ti].d_day;
                const int   HOUR   = DATA[ti].d_hour;
                const int   MIN    = DATA[ti].d_min;
                const int   SEC    = DATA[ti].d_sec;
                const int   MSEC   = DATA[ti].d_msec;
                const int   OFFSET = DATA[ti].d_offset;

                if (veryVerbose) { T_ P_(LINE) P(INPUT) }

                bdlt::Datetime   mX(XX);  const bdlt::Datetime&   X = mX;
                bdlt::DatetimeTz mZ(ZZ);  const bdlt::DatetimeTz& Z = mZ;

                bdlt::DatetimeTz EXPECTED(bdlt::Datetime(YEAR, MONTH, DAY,
                                                         HOUR, MIN, SEC, MSEC),
                                          OFFSET);

                if (0 == OFFSET) {
                    ASSERTV(LINE, INPUT, 0 == Util::parse(&mX, INPUT, LENGTH));
                    ASSERTV(LINE, INPUT, EXPECTED.utcDatetime() == X);
                }
                else {
                    ASSERTV(LINE, INPUT, 0 != Util::parse(&mX, INPUT, LENGTH));
                    ASSERTV(LINE, INPUT, XX == X);
                }

                ASSERTV(LINE, INPUT, 0 == Util::parse(&mZ, INPUT, LENGTH));
                ASSERTV(LINE, INPUT, EXPECTED, Z, EXPECTED == Z);

                mX = XX;
                mZ = ZZ;

                if (0 == OFFSET) {
                    ASSERTV(LINE, INPUT,
                            0 == Util::parse(&mX, StrView(INPUT, LENGTH)));
                    ASSERTV(LINE, INPUT, EXPECTED.utcDatetime() == X);
                }
                else {
                    ASSERTV(LINE, INPUT,
                            0 != Util::parse(&mX, StrView(INPUT, LENGTH)));
                    ASSERTV(LINE, INPUT, XX == X);
                }

                ASSERTV(LINE, INPUT,
                        0 == Util::parse(&mZ, StrView(INPUT, LENGTH)));
                ASSERTV(LINE, INPUT, EXPECTED, Z, EXPECTED == Z);

                // parseRelaxed() on the same string

                mX = XX;
                mZ = ZZ;

                if (0 == OFFSET) {
                    ASSERTV(LINE, INPUT,
                            0 == Util::parseRelaxed(&mX, INPUT, LENGTH));
                    ASSERTV(LINE, INPUT, EXPECTED.utcDatetime() == X);
                }
                else {
                    ASSERTV(LINE, INPUT,
                            0 != Util::parseRelaxed(&mX, INPUT, LENGTH));
                    ASSERTV(LINE, INPUT, XX == X);
                }

                ASSERTV(LINE, INPUT,
                        0 == Util::parseRelaxed(&mZ, INPUT, LENGTH));
                ASSERTV(LINE, INPUT, EXPECTED, Z, EXPECTED == Z);

                mX = XX;
                mZ = ZZ;

                if (0 == OFFSET) {
                    ASSERTV(LINE, INPUT,
                            0 == Util::parseRelaxed(&mX,
                                                    StrView(INPUT, LENGTH)));
                    ASSERTV(LINE, INPUT, EXPECTED.utcDatetime() == X);
                }
                else {
                    ASSERTV(LINE, INPUT,
                            0 != Util::parseRelaxed(&mX,
                                                    StrView(INPUT, LENGTH)));
                    ASSERTV(LINE, INPUT, XX == X);
                }

                ASSERTV(LINE, INPUT,
                        0 == Util::parseRelaxed(&mZ, StrView(INPUT, LENGTH)));
                ASSERTV(LINE, INPUT, EXPECTED, Z, EXPECTED == Z);

                // parseRelaxed() on the string modified to have SPACE instead
                // of 'T'
                const bsl::string relaxed = replaceTWithSpace(INPUT, LENGTH);

                mX = XX;
                mZ = ZZ;

                if (0 == OFFSET) {
                    ASSERTV(LINE, INPUT,
                            0 == Util::parseRelaxed(&mX,
                                                    relaxed.data(),
                                                    relaxed.length()));
                    ASSERTV(LINE, INPUT, EXPECTED.utcDatetime() == X);
                }
                else {
                    ASSERTV(LINE, INPUT,
                            0 != Util::parseRelaxed(&mX,
                                                    relaxed.data(),
                                                    relaxed.length()));
                    ASSERTV(LINE, INPUT, XX == X);
                }

                ASSERTV(LINE, INPUT,
                        0 == Util::parseRelaxed(&mZ,
                                                relaxed.data(),
                                                relaxed.length()));
                ASSERTV(LINE, INPUT, EXPECTED, Z, EXPECTED == Z);

                mX = XX;
                mZ = ZZ;

                if (0 == OFFSET) {
                    ASSERTV(LINE, INPUT,
                            0 == Util::parseRelaxed(&mX, StrView(relaxed)));
                    ASSERTV(LINE, INPUT, EXPECTED.utcDatetime() == X);
                }
                else {
                    ASSERTV(LINE, INPUT,
                            0 != Util::parseRelaxed(&mX, StrView(relaxed)));
                    ASSERTV(LINE, INPUT, XX == X);
                }

                ASSERTV(LINE, INPUT,
                        0 == Util::parseRelaxed(&mZ, StrView(relaxed)));
                ASSERTV(LINE, INPUT, EXPECTED, Z, EXPECTED == Z);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const char *INPUT  = "2013-10-23T01:23:45";
            const int   LENGTH = static_cast<int>(bsl::strlen(INPUT));

            const StrView stringRef(INPUT, LENGTH);
            const StrView nullRef;

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

                ASSERT_PASS(Util::parse(  &result, stringRef));
                ASSERT_FAIL(Util::parse(      bad, stringRef));

                ASSERT_PASS(Util::parse(&resultTz, stringRef));
                ASSERT_FAIL(Util::parse(    badTz, stringRef));

                ASSERT_PASS(Util::parseRelaxed(  &result, INPUT, LENGTH));
                ASSERT_FAIL(Util::parseRelaxed(      bad, INPUT, LENGTH));

                ASSERT_PASS(Util::parseRelaxed(&resultTz, INPUT, LENGTH));
                ASSERT_FAIL(Util::parseRelaxed(    badTz, INPUT, LENGTH));

                ASSERT_PASS(Util::parseRelaxed(  &result, stringRef));
                ASSERT_FAIL(Util::parseRelaxed(      bad, stringRef));

                ASSERT_PASS(Util::parseRelaxed(&resultTz, stringRef));
                ASSERT_FAIL(Util::parseRelaxed(    badTz, stringRef));
            }

            if (veryVerbose) cout << "\t'Invalid input'" << endl;
            {
                ASSERT_PASS(Util::parse(  &result, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(  &result,     0, LENGTH));

                ASSERT_PASS(Util::parse(&resultTz, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(&resultTz,     0, LENGTH));

                ASSERT_PASS(Util::parse(  &result, stringRef));
                ASSERT_FAIL(Util::parse(  &result, nullRef));

                ASSERT_PASS(Util::parse(&resultTz, stringRef));
                ASSERT_FAIL(Util::parse(&resultTz, nullRef));

                ASSERT_PASS(Util::parseRelaxed(  &result, INPUT, LENGTH));
                ASSERT_FAIL(Util::parseRelaxed(  &result,     0, LENGTH));

                ASSERT_PASS(Util::parseRelaxed(&resultTz, INPUT, LENGTH));
                ASSERT_FAIL(Util::parseRelaxed(&resultTz,     0, LENGTH));

                ASSERT_PASS(Util::parseRelaxed(  &result, stringRef));
                ASSERT_FAIL(Util::parseRelaxed(  &result, nullRef));

                ASSERT_PASS(Util::parseRelaxed(&resultTz, stringRef));
                ASSERT_FAIL(Util::parseRelaxed(&resultTz, nullRef));
            }

            if (veryVerbose) cout << "\t'Invalid length'" << endl;
            {
                ASSERT_PASS(Util::parse(  &result, INPUT, LENGTH));
                ASSERT_PASS(Util::parse(  &result, INPUT,      0));
                ASSERT_FAIL(Util::parse(  &result, INPUT,     -1));

                ASSERT_PASS(Util::parse(&resultTz, INPUT, LENGTH));
                ASSERT_PASS(Util::parse(&resultTz, INPUT,      0));
                ASSERT_FAIL(Util::parse(&resultTz, INPUT,     -1));

                ASSERT_PASS(Util::parseRelaxed(  &result, INPUT, LENGTH));
                ASSERT_PASS(Util::parseRelaxed(  &result, INPUT,      0));
                ASSERT_FAIL(Util::parseRelaxed(  &result, INPUT,     -1));

                ASSERT_PASS(Util::parseRelaxed(&resultTz, INPUT, LENGTH));
                ASSERT_PASS(Util::parseRelaxed(&resultTz, INPUT,      0));
                ASSERT_FAIL(Util::parseRelaxed(&resultTz, INPUT,     -1));
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // PARSE: TIME & TIMETZ
        //
        // Concerns:
        //: 1 All ISO 8601 string representations supported by this component
        //:   (as documented in the header file) for 'Time' and 'TimeTz' values
        //:   are parsed successfully.
        //:
        //: 2 If parsing succeeds, the result 'Time' or 'TimeTz' object has the
        //:   expected value.
        //:
        //: 3 If the optional zone designator is present in the input string
        //:   when parsing into a 'Time' object, the resulting value is
        //:   converted to the equivalent UTC time.
        //:
        //: 4 If the optional zone designator is *not* present in the input
        //:   string when parsing into a 'TimeTz' object, it is assumed to be
        //:   UTC.
        //:
        //: 5 If parsing succeeds, 0 is returned.
        //:
        //: 6 All strings that are not ISO 8601 representations supported by
        //:   this component for 'Time' and 'TimeTz' values are rejected (i.e.,
        //:   parsing fails).
        //:
        //: 7 If parsing fails, the result object is unaffected and a non-zero
        //:   value is returned.
        //:
        //: 8 The entire extent of the input string is parsed.
        //:
        //: 9 Leap seconds and fractional seconds containing more than three
        //:   digits are handled correctly.
        //:
        //:10 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'Time' values ('T'), zone designators ('Z'), and configurations
        //:   ('C').
        //:
        //: 2 Apply the (fully-tested) 'generateRaw' functions to each element
        //:   in the cross product, 'T x Z x C', of the test data from P-1.
        //:
        //: 3 Invoke the 'parse' functions on the strings generated in P-2 and
        //:   verify that parsing succeeds, i.e., that 0 is returned and the
        //:   result objects have the expected values.  (C-1..5)
        //:
        //: 4 Using the table-driven technique, specify a set of distinct
        //:   strings that are not ISO 8601 representations supported by this
        //:   component for 'Time' and 'TimeTz' values.
        //:
        //: 5 Invoke the 'parse' functions on the strings from P-4 and verify
        //:   that parsing fails, i.e., that a non-zero value is returned and
        //:   the result objects are unchanged.  (C-6..8)
        //:
        //: 6 Using the table-driven technique, specify a set of distinct
        //:   ISO 8601 strings that specifically cover cases involving leap
        //:   seconds and fractional seconds containing more than three digits.
        //:
        //: 7 Invoke the 'parse' functions on the strings from P-6 and verify
        //:   the results are as expected.  (C-9)
        //:
        //: 8 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-10)
        //
        // Testing:
        //   int parse(Time *, const char *, int);
        //   int parse(TimeTz *, const char *, int);
        //   int parse(Time *result, const StringRef& string);
        //   int parse(TimeTz *result, const StringRef& string);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PARSE: TIME & TIMETZ" << endl
                          << "====================" << endl;

        char buffer[Util::k_MAX_STRLEN];

        const bdlt::Time   XX(2, 4, 6, 8);  // 'XX' and 'ZZ' are controls,
        const bdlt::TimeTz ZZ(XX, -7);      // distinct from any test data

        const int                  NUM_TIME_DATA =       NUM_DEFAULT_TIME_DATA;
        const DefaultTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] =
                                                             DEFAULT_TIME_DATA;

        const int                  NUM_ZONE_DATA =       NUM_DEFAULT_ZONE_DATA;
        const DefaultZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] =
                                                             DEFAULT_ZONE_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        if (verbose) cout << "\nValid ISO 8601 strings." << endl;

        for (int ti = 0; ti < NUM_TIME_DATA; ++ti) {
            const int ILINE = TIME_DATA[ti].d_line;
            const int HOUR  = TIME_DATA[ti].d_hour;
            const int MIN   = TIME_DATA[ti].d_min;
            const int SEC   = TIME_DATA[ti].d_sec;
            const int MSEC  = TIME_DATA[ti].d_msec;
            const int USEC  = TIME_DATA[ti].d_usec;

            for (int tj = 0; tj < NUM_ZONE_DATA; ++tj) {
                const int JLINE  = ZONE_DATA[tj].d_line;
                const int OFFSET = ZONE_DATA[tj].d_offset;

                if (   bdlt::Time(HOUR, MIN, SEC, MSEC, USEC) == bdlt::Time()
                    && OFFSET != 0) {
                    continue;  // skip invalid compositions
                }

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    int expMsec = MSEC;
                    int expUsec = USEC;
                    {
                        // adjust the expected milliseconds to account for
                        // PRECISION truncating the value generated

                        int precision = (PRECISION < 3 ? PRECISION : 3);

                        for (int i = 3; i > precision; --i) {
                            expMsec /= 10;
                        }

                        for (int i = 3; i > precision; --i) {
                            expMsec *= 10;
                        }

                        // adjust the expected microseconds to account for
                        // PRECISION truncating the value generated

                        precision = (PRECISION > 3 ? PRECISION - 3: 0);

                        for (int i = 3; i > precision; --i) {
                            expUsec /= 10;
                        }

                        for (int i = 3; i > precision; --i) {
                            expUsec *= 10;
                        }
                    }

                    const bdlt::Time TIME(HOUR, MIN, SEC, expMsec, expUsec);

                    const bdlt::TimeTz TIMETZ(TIME, OFFSET);

                    if (veryVerbose) {
                        if (0 == tc) {
                            T_ P_(ILINE) P_(JLINE) P_(TIME) P(TIMETZ);
                        }
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                          P_(USECOMMA) P(USEZ);
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    // without zone designator in parsed string
                    {
                        const int LENGTH = Util::generateRaw(buffer, TIME, C);

                        if (veryVerbose) {
                            const bsl::string STRING(buffer, LENGTH);
                            T_ T_ P(STRING)
                        }

                        bdlt::Time   mX(XX);  const bdlt::Time&   X = mX;
                        bdlt::TimeTz mZ(ZZ);  const bdlt::TimeTz& Z = mZ;

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, TIME, X, TIME == X);

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mZ, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, TIME == Z.localTime());
                        ASSERTV(ILINE, JLINE, CLINE,    0 == Z.offset());

                        mX = XX;
                        mZ = ZZ;

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, TIME == X);

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mZ, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, TIME == Z.localTime());
                        ASSERTV(ILINE, JLINE, CLINE,    0 == Z.offset());
                    }

                    // with zone designator in parsed string
                    {
                        const int LENGTH = Util::generateRaw(buffer,
                                                             TIMETZ,
                                                             C);

                        if (veryVerbose) {
                            const bsl::string STRING(buffer, LENGTH);
                            T_ T_ P(STRING)
                        }

                        bdlt::Time   mX(XX);  const bdlt::Time&   X = mX;
                        bdlt::TimeTz mZ(ZZ);  const bdlt::TimeTz& Z = mZ;

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, TIMETZ.utcTime() == X);

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mZ, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, TIMETZ           == Z);

                        mX = XX;
                        mZ = ZZ;

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, TIMETZ.utcTime() == X);

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mZ, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, TIMETZ           == Z);
                    }
                }  // loop over 'CNFG_DATA'
            }  // loop over 'ZONE_DATA'
        }  // loop over 'TIME_DATA'

        {
            // verify 'z' is accepted

            bdlt::Time   mX(XX);  const bdlt::Time&   X = mX;
            bdlt::TimeTz mZ(ZZ);  const bdlt::TimeTz& Z = mZ;

            ASSERT(0 == Util::parse(&mX, "01:02:03z", 9));
            ASSERT(X == bdlt::Time(1, 2, 3));

            ASSERT(0 == Util::parse(&mZ, "01:02:03z", 9));
            ASSERT(Z == bdlt::TimeTz(bdlt::Time(1, 2, 3), 0));
        }

        if (verbose) cout << "\nInvalid strings." << endl;
        {
            bdlt::Time   mX(XX);  const bdlt::Time&   X = mX;
            bdlt::TimeTz mZ(ZZ);  const bdlt::TimeTz& Z = mZ;

            const int              NUM_TIME_DATA =         NUM_BAD_TIME_DATA;
            const BadTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] = BAD_TIME_DATA;

            for (int ti = 0; ti < NUM_TIME_DATA; ++ti) {
                const int   LINE   = TIME_DATA[ti].d_line;
                const char *STRING = TIME_DATA[ti].d_invalid;

                if (veryVerbose) { T_ P_(LINE) P(STRING) }

                const int LENGTH = static_cast<int>(bsl::strlen(STRING));

                ASSERTV(LINE, STRING,  0 != Util::parse(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,  0 != Util::parse(&mZ, STRING, LENGTH));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mZ, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, ZZ == Z);
            }

            const int              NUM_ZONE_DATA =         NUM_BAD_ZONE_DATA;
            const BadZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] = BAD_ZONE_DATA;

            for (int ti = 0; ti < NUM_ZONE_DATA; ++ti) {
                const int LINE = ZONE_DATA[ti].d_line;

                // Initialize with a *valid* time string, then append an
                // invalid zone designator.

                bsl::string bad("12:26:52.726");

                // Ensure that 'bad' is initially valid.

                static bool firstFlag = true;
                if (firstFlag) {
                    const char *STRING = bad.data();
                    const int   LENGTH = static_cast<int>(bad.length());

                    bdlt::Time mT(XX);  const bdlt::Time& T = mT;

                    ASSERT( 0 == Util::parse(&mT, STRING, LENGTH));
                    ASSERT(XX != T);

                    mT = XX;

                    ASSERT( 0 == Util::parse(&mT, StrView(STRING, LENGTH)));
                    ASSERT(XX != T);
                }

                // If 'ZONE_DATA[ti].d_invalid' contains nothing but digits,
                // appending it to 'bad' simply extends the fractional second
                // (so 'bad' remains valid).

                if (containsOnlyDigits(ZONE_DATA[ti].d_invalid)) {
                    continue;
                }

                bad.append(ZONE_DATA[ti].d_invalid);

                const char *STRING = bad.c_str();
                const int   LENGTH = static_cast<int>(bad.length());

                if (veryVerbose) { T_ P_(LINE) P(STRING) }

                ASSERTV(LINE, STRING,  0 != Util::parse(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,  0 != Util::parse(&mZ, STRING, LENGTH));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mZ, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, ZZ == Z);
            }
        }

        if (verbose) cout << "\nTesting leap seconds and fractional seconds."
                          << endl;
        {
            const struct {
                int         d_line;
                const char *d_input;
                int         d_hour;
                int         d_min;
                int         d_sec;
                int         d_msec;
                int         d_usec;
                int         d_offset;
            } DATA[] = {
                // leap seconds
                { L_, "00:00:60.000",    00, 01, 00, 000, 000,   0 },
                { L_, "22:59:60.999",    23, 00, 00, 999, 000,   0 },
                { L_, "23:59:60.999",    00, 00, 00, 999, 000,   0 },

                // fractional seconds
                { L_, "00:00:00.0001",      00, 00, 00, 000, 100,   0 },
                { L_, "00:00:00.0009",      00, 00, 00, 000, 900,   0 },
                { L_, "00:00:00.00001",     00, 00, 00, 000,  10,   0 },
                { L_, "00:00:00.00049",     00, 00, 00, 000, 490,   0 },
                { L_, "00:00:00.00050",     00, 00, 00, 000, 500,   0 },
                { L_, "00:00:00.00099",     00, 00, 00, 000, 990,   0 },
                { L_, "00:00:00.0000001",   00, 00, 00, 000, 000,   0 },
                { L_, "00:00:00.0000009",   00, 00, 00, 000, 001,   0 },
                { L_, "00:00:00.00000001",  00, 00, 00, 000, 000,   0 },
                { L_, "00:00:00.00000049",  00, 00, 00, 000, 000,   0 },
                { L_, "00:00:00.00000050",  00, 00, 00, 000, 001,   0 },
                { L_, "00:00:00.00000099",  00, 00, 00, 000, 001,   0 },
                { L_, "00:00:00.9994",      00, 00, 00, 999, 400,   0 },
                { L_, "00:00:00.9995",      00, 00, 00, 999, 500,   0 },
                { L_, "00:00:00.9999",      00, 00, 00, 999, 900,   0 },
                { L_, "00:00:59.9999",      00, 00, 59, 999, 900,   0 },
                { L_, "23:59:59.9999",      23, 59, 59, 999, 900,   0 },
                { L_, "00:00:00.9999994",   00, 00, 00, 999, 999,   0 },
                { L_, "00:00:00.9999995",   00, 00,  1, 000, 000,   0 },
                { L_, "00:00:00.9999999",   00, 00,  1, 000, 000,   0 },
                { L_, "00:00:59.9999999",   00,  1, 00, 000, 000,   0 },
                { L_, "23:59:59.9999999",   00, 00, 00, 000, 000,   0 },

                // omit fractional seconds
                { L_, "12:34:45",        12, 34, 45, 000, 000,   0 },
                { L_, "12:34:45Z",       12, 34, 45, 000, 000,   0 },
                { L_, "12:34:45+00:30",  12, 34, 45, 000, 000,  30 },
                { L_, "00:00:00+00:30",  00, 00, 00, 000, 000,  30 },
                { L_, "12:34:45-01:30",  12, 34, 45, 000, 000, -90 },
                { L_, "23:59:59-01:30",  23, 59, 59, 000, 000, -90 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE   = DATA[ti].d_line;
                const char *INPUT  = DATA[ti].d_input;
                const int   LENGTH = static_cast<int>(bsl::strlen(INPUT));
                const int   HOUR   = DATA[ti].d_hour;
                const int   MIN    = DATA[ti].d_min;
                const int   SEC    = DATA[ti].d_sec;
                const int   MSEC   = DATA[ti].d_msec;
                const int   USEC   = DATA[ti].d_usec;
                const int   OFFSET = DATA[ti].d_offset;

                if (veryVerbose) { T_ P_(LINE) P(INPUT) }

                bdlt::Time   mX(XX);  const bdlt::Time&   X = mX;
                bdlt::TimeTz mZ(ZZ);  const bdlt::TimeTz& Z = mZ;

                bdlt::TimeTz EXPECTED(bdlt::Time(HOUR, MIN, SEC, MSEC, USEC),
                                      OFFSET);

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parse(&mX, INPUT, LENGTH));
                ASSERTV(LINE, EXPECTED, X, EXPECTED.utcTime() == X);

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parse(&mZ, INPUT, LENGTH));
                ASSERTV(LINE, EXPECTED, Z, EXPECTED == Z);

                mX = XX;
                mZ = ZZ;

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parse(&mX, StrView(INPUT, LENGTH)));
                ASSERTV(LINE, EXPECTED, X, EXPECTED.utcTime() == X);

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parse(&mZ, StrView(INPUT, LENGTH)));
                ASSERTV(LINE, EXPECTED, Z, EXPECTED == Z);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const char *INPUT  = "01:23:45";
            const int   LENGTH = static_cast<int>(bsl::strlen(INPUT));

            const StrView stringRef(INPUT, LENGTH);
            const StrView nullRef;

            bdlt::Time   result;
            bdlt::TimeTz resultTz;

            if (veryVerbose) cout << "\t'Invalid result'" << endl;
            {
                bdlt::Time   *bad   = 0;
                bdlt::TimeTz *badTz = 0;

                ASSERT_PASS(Util::parse(  &result, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(      bad, INPUT, LENGTH));

                ASSERT_PASS(Util::parse(&resultTz, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(    badTz, INPUT, LENGTH));

                ASSERT_PASS(Util::parse(  &result, stringRef));
                ASSERT_FAIL(Util::parse(      bad, stringRef));

                ASSERT_PASS(Util::parse(&resultTz, stringRef));
                ASSERT_FAIL(Util::parse(    badTz, stringRef));
            }

            if (veryVerbose) cout << "\t'Invalid input'" << endl;
            {
                ASSERT_PASS(Util::parse(  &result, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(  &result,     0, LENGTH));

                ASSERT_PASS(Util::parse(&resultTz, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(&resultTz,     0, LENGTH));

                ASSERT_PASS(Util::parse(  &result, stringRef));
                ASSERT_FAIL(Util::parse(  &result, nullRef));

                ASSERT_PASS(Util::parse(&resultTz, stringRef));
                ASSERT_FAIL(Util::parse(&resultTz, nullRef));
            }

            if (veryVerbose) cout << "\t'Invalid length'" << endl;
            {
                ASSERT_PASS(Util::parse(  &result, INPUT, LENGTH));
                ASSERT_PASS(Util::parse(  &result, INPUT,      0));
                ASSERT_FAIL(Util::parse(  &result, INPUT,     -1));

                ASSERT_PASS(Util::parse(&resultTz, INPUT, LENGTH));
                ASSERT_PASS(Util::parse(&resultTz, INPUT,      0));
                ASSERT_FAIL(Util::parse(&resultTz, INPUT,     -1));
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // PARSE: bsls::TimeInterval
        //
        // Concerns:
        //: 1 All ISO 8601 string representations supported by this component
        //:   (as documented in the header file) for 'TimeInterval' values are
        //:   parsed successfully.
        //:
        //: 2 If parsing succeeds, the result 'TimeInterval' object has the
        //:   expected value.
        //:
        //: 3 If parsing succeeds, 0 is returned.
        //:
        //: 4 All strings that are not ISO 8601 representations supported by
        //:   this component for 'TimeInterval' values are rejected (i.e.,
        //:   parsing fails).
        //:
        //: 5 If parsing fails, the result object is uneffected and a non-zero
        //:   value is returned.
        //:
        //: 6 The entire extent of the input string is parsed.
        //:
        //: 7 Fractional seconds containing more than nine digits are handled
        //:   correctly.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'TimeInterval' values.
        //:
        //: 2 Apply the (fully-tested) 'generateRaw' functions to each element
        //:   in the cross product, 'T x Z x C', of the test data from P-1.
        //:
        //: 3 Invoke the 'parse' functions on the strings generated in P-2 and
        //:   verify that parsing succeeds, i.e., that 0 is returned and the
        //:   result objects have the expected values.  (C-1..5)
        //:
        //: 4 Using the table-driven technique, specify a set of distinct
        //:   strings that are not ISO 8601 representations supported by this
        //:   component for 'Time' and 'TimeTz' values.
        //:
        //: 5 Invoke the 'parse' functions on the strings from P-4 and verify
        //:   that parsing fails, i.e., that a non-zero value is returned and
        //:   the result objects are unchanged.  (C-6..8)
        //:
        //: 6 Using the table-driven technique, specify a set of distinct
        //:   ISO 8601 strings that specifically cover cases involving leap
        //:   seconds and fractional seconds containing more than three digits.
        //:
        //: 7 Invoke the 'parse' functions on the strings from P-6 and verify
        //:   the results are as expected.  (C-9)
        //
        // Testing:
        //   int parse(Time *, const char *, int);
        //   int parse(TimeTz *, const char *, int);
        //   int parse(Time *result, const StringRef& string);
        //   int parse(TimeTz *result, const StringRef& string);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PARSE: bsls::TimeInterval" << endl
                          << "=========================" << endl;

        const bsls::TimeInterval XX(2, 4);  // A control, distinct from any
                                            // test data.

        if (verbose) cout << "\nTesting bsls::TimeInterval." << endl;
        {
            const int NUM_INTERVAL_DATA = NUM_DEFAULT_INTERVAL_DATA;
            const DefaultIntervalDataRow (&INTERVAL_DATA)[NUM_INTERVAL_DATA] =
                                                         DEFAULT_INTERVAL_DATA;

            for (int ti = 0; ti < NUM_INTERVAL_DATA; ++ti) {
                const int                 LINE   = INTERVAL_DATA[ti].d_line;
                const char               *INPUT  = INTERVAL_DATA[ti].d_iso8601;
                const int                 LENGTH =
                                          static_cast<int>(bsl::strlen(INPUT));
                const bsls::Types::Int64  SEC    = INTERVAL_DATA[ti].d_sec;
                const int                 USEC   = INTERVAL_DATA[ti].d_usec;

                if (veryVerbose) { T_ P_(LINE) P(INPUT) }

                bsls::TimeInterval mX(XX);  const bsls::TimeInterval&   X = mX;
                bsls::TimeInterval EXPECTED(SEC, USEC);

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parse(&mX, INPUT, LENGTH));
                ASSERTV(LINE, EXPECTED, X, EXPECTED == X);

                mX = XX;

                ASSERTV(LINE, INPUT, LENGTH,
                        0 == Util::parse(&mX, StrView(INPUT, LENGTH)));
                ASSERTV(LINE, EXPECTED, X, EXPECTED == X);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const struct {
                int         d_line;
                const char *d_input;
            } DATA[] = {
                { L_, "1D"         },
                { L_, "P"          },
                { L_, "P0"         },
                { L_, "P1"         },
                { L_, "PW"         },
                { L_, "PW1D"       },
                { L_, "P1D1W"      },
                { L_, "P1D1D"      },
                { L_, "P1H"        },
                { L_, "PT"         },
                { L_, "PT1"        },
                { L_, "P1.0W1D"    },
                { L_, "PT1.1H1M"   },
                { L_, "PT1.0H1.0S" },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_line;
                const char *INPUT = DATA[ti].d_input;
                const int   LENGTH = static_cast<int>(bsl::strlen(INPUT));

                if (veryVerbose) { T_ P_(LINE) P(INPUT) }

                bsls::TimeInterval mX(XX);  const bsls::TimeInterval&   X = mX;

                ASSERTV(LINE, INPUT, LENGTH,
                    0 != Util::parse(&mX, INPUT, LENGTH));
                ASSERTV(LINE, XX, X, XX == X);

                ASSERTV(LINE, INPUT, LENGTH,
                    0 != Util::parse(&mX, StrView(INPUT, LENGTH)));
                ASSERTV(LINE, XX, X, XX == X);
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PARSE: DATE & DATETZ
        //
        // Concerns:
        //: 1 All ISO 8601 string representations supported by this component
        //:   (as documented in the header file) for 'Date' and 'DateTz' values
        //:   are parsed successfully.
        //:
        //: 2 If parsing succeeds, the result 'Date' or 'DateTz' object has the
        //:   expected value.
        //:
        //: 3 If the optional zone designator is present in the input string
        //:   when parsing into a 'Date' object, it is parsed for validity but
        //:   is otherwise ignored.
        //:
        //: 4 If the optional zone designator is *not* present in the input
        //:   string when parsing into a 'DateTz' object, it is assumed to be
        //:   UTC.
        //:
        //: 5 If parsing succeeds, 0 is returned.
        //:
        //: 6 All strings that are not ISO 8601 representations supported by
        //:   this component for 'Date' and 'DateTz' values are rejected (i.e.,
        //:   parsing fails).
        //:
        //: 7 If parsing fails, the result object is unaffected and a non-zero
        //:   value is returned.
        //:
        //: 8 The entire extent of the input string is parsed.
        //:
        //: 9 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'Date' values ('D'), zone designators ('Z'), and configurations
        //:   ('C').
        //:
        //: 2 Apply the (fully-tested) 'generateRaw' functions to each element
        //:   in the cross product, 'D x Z x C', of the test data from P-1.
        //:
        //: 3 Invoke the 'parse' functions on the strings generated in P-2 and
        //:   verify that parsing succeeds, i.e., that 0 is returned and the
        //:   result objects have the expected values.  (C-1..5)
        //:
        //: 4 Using the table-driven technique, specify a set of distinct
        //:   strings that are not ISO 8601 representations supported by this
        //:   component for 'Date' and 'DateTz' values.
        //:
        //: 5 Invoke the 'parse' functions on the strings from P-4 and verify
        //:   that parsing fails, i.e., that a non-zero value is returned and
        //:   the result objects are unchanged.  (C-6..8)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-9)
        //
        // Testing:
        //   int parse(Date *, const char *, int);
        //   int parse(DateTz *, const char *, int);
        //   int parse(Date *result, const StringRef& string);
        //   int parse(DateTz *result, const StringRef& string);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PARSE: DATE & DATETZ" << endl
                          << "====================" << endl;

        char buffer[Util::k_MAX_STRLEN];

        const bdlt::Date   XX(246, 8, 10);  // 'XX' and 'ZZ' are controls,
        const bdlt::DateTz ZZ(XX, -7);      // distinct from any test data

        const int                  NUM_DATE_DATA =       NUM_DEFAULT_DATE_DATA;
        const DefaultDateDataRow (&DATE_DATA)[NUM_DATE_DATA] =
                                                             DEFAULT_DATE_DATA;

        const int                  NUM_ZONE_DATA =       NUM_DEFAULT_ZONE_DATA;
        const DefaultZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] =
                                                             DEFAULT_ZONE_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        if (verbose) cout << "\nValid ISO 8601 strings." << endl;

        for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
            const int ILINE = DATE_DATA[ti].d_line;
            const int YEAR  = DATE_DATA[ti].d_year;
            const int MONTH = DATE_DATA[ti].d_month;
            const int DAY   = DATE_DATA[ti].d_day;

            const bdlt::Date DATE(YEAR, MONTH, DAY);

            for (int tj = 0; tj < NUM_ZONE_DATA; ++tj) {
                const int JLINE  = ZONE_DATA[tj].d_line;
                const int OFFSET = ZONE_DATA[tj].d_offset;

                const bdlt::DateTz DATETZ(DATE, OFFSET);

                if (veryVerbose) { T_ P_(ILINE) P_(JLINE) P_(DATE) P(DATETZ) }

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    // without zone designator in parsed string
                    {
                        const int LENGTH = Util::generateRaw(buffer, DATE, C);

                        if (veryVerbose) {
                            const bsl::string STRING(buffer, LENGTH);
                            T_ T_ P(STRING)
                        }

                        bdlt::Date   mX(XX);  const bdlt::Date&   X = mX;
                        bdlt::DateTz mZ(ZZ);  const bdlt::DateTz& Z = mZ;

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, DATE == X);

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mZ, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, DATE == Z.localDate());
                        ASSERTV(ILINE, JLINE, CLINE,    0 == Z.offset());

                        mX = XX;
                        mZ = ZZ;

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, DATE == X);

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mZ, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, DATE == Z.localDate());
                        ASSERTV(ILINE, JLINE, CLINE,    0 == Z.offset());
                    }

                    // with zone designator in parsed string
                    {
                        const int LENGTH = Util::generateRaw(buffer,
                                                             DATETZ,
                                                             C);

                        if (veryVerbose) {
                            const bsl::string STRING(buffer, LENGTH);
                            T_ T_ P(STRING)
                        }

                        bdlt::Date   mX(XX);  const bdlt::Date&   X = mX;
                        bdlt::DateTz mZ(ZZ);  const bdlt::DateTz& Z = mZ;

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mX, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, DATE   == X);

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mZ, buffer, LENGTH));
                        ASSERTV(ILINE, JLINE, CLINE, DATETZ == Z);

                        mX = XX;
                        mZ = ZZ;

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mX, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, DATE   == X);

                        ASSERTV(ILINE, JLINE, CLINE,
                               0 == Util::parse(&mZ, StrView(buffer, LENGTH)));
                        ASSERTV(ILINE, JLINE, CLINE, DATETZ == Z);
                    }
                }  // loop over 'CNFG_DATA'
            }  // loop over 'ZONE_DATA'
        }  // loop over 'DATE_DATA'

        {
            // verify 'z' is accepted

            bdlt::Date   mX(XX);  const bdlt::Date&   X = mX;
            bdlt::DateTz mZ(ZZ);  const bdlt::DateTz& Z = mZ;

            ASSERT(0 == Util::parse(&mX, "0001-02-03z", 11));
            ASSERT(X == bdlt::Date(1, 2, 3));

            ASSERT(0 == Util::parse(&mZ, "0001-02-03z", 11));
            ASSERT(Z == bdlt::DateTz(bdlt::Date(1, 2, 3), 0));
        }

        if (verbose) cout << "\nInvalid strings." << endl;
        {
            bdlt::Date   mX(XX);  const bdlt::Date&   X = mX;
            bdlt::DateTz mZ(ZZ);  const bdlt::DateTz& Z = mZ;

            const int              NUM_DATE_DATA =         NUM_BAD_DATE_DATA;
            const BadDateDataRow (&DATE_DATA)[NUM_DATE_DATA] = BAD_DATE_DATA;

            for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
                const int   LINE   = DATE_DATA[ti].d_line;
                const char *STRING = DATE_DATA[ti].d_invalid;

                if (veryVerbose) { T_ P_(LINE) P(STRING) }

                const int LENGTH = static_cast<int>(bsl::strlen(STRING));

                ASSERTV(LINE, STRING,  0 != Util::parse(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,  0 != Util::parse(&mZ, STRING, LENGTH));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mZ, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, ZZ == Z);
            }

            const int              NUM_ZONE_DATA =         NUM_BAD_ZONE_DATA;
            const BadZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] = BAD_ZONE_DATA;

            for (int ti = 0; ti < NUM_ZONE_DATA; ++ti) {
                const int LINE = ZONE_DATA[ti].d_line;

                // Initialize with a *valid* date string, then append an
                // invalid zone designator.

                bsl::string bad("2010-08-17");

                // Ensure that 'bad' is initially valid.

                static bool firstFlag = true;
                if (firstFlag) {
                    const char *STRING = bad.data();
                    const int   LENGTH = static_cast<int>(bad.length());

                    bdlt::Date mD(XX);  const bdlt::Date& D = mD;

                    ASSERT( 0 == Util::parse(&mD, STRING, LENGTH));
                    ASSERT(XX != D);

                    mD = XX;

                    ASSERT( 0 == Util::parse(&mD, StrView(STRING, LENGTH)));
                    ASSERT(XX != D);
                }

                bad.append(ZONE_DATA[ti].d_invalid);

                const char *STRING = bad.c_str();
                const int   LENGTH = static_cast<int>(bad.length());

                if (veryVerbose) { T_ P_(LINE) P(STRING) }

                ASSERTV(LINE, STRING,  0 != Util::parse(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,  0 != Util::parse(&mZ, STRING, LENGTH));
                ASSERTV(LINE, STRING, ZZ == Z);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mX, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,
                        0 != Util::parse(&mZ, StrView(STRING, LENGTH)));
                ASSERTV(LINE, STRING, ZZ == Z);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const char *INPUT  = "2013-10-23";
            const int   LENGTH = static_cast<int>(bsl::strlen(INPUT));

            const StrView stringRef(INPUT, LENGTH);
            const StrView nullRef;

            bdlt::Date   result;
            bdlt::DateTz resultTz;

            if (veryVerbose) cout << "\t'Invalid result'" << endl;
            {
                bdlt::Date   *bad   = 0;
                bdlt::DateTz *badTz = 0;

                ASSERT_PASS(Util::parse(  &result, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(      bad, INPUT, LENGTH));

                ASSERT_PASS(Util::parse(&resultTz, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(    badTz, INPUT, LENGTH));

                ASSERT_PASS(Util::parse(  &result, stringRef));
                ASSERT_FAIL(Util::parse(      bad, stringRef));

                ASSERT_PASS(Util::parse(&resultTz, stringRef));
                ASSERT_FAIL(Util::parse(    badTz, stringRef));
            }

            if (veryVerbose) cout << "\t'Invalid input'" << endl;
            {
                ASSERT_PASS(Util::parse(  &result, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(  &result,     0, LENGTH));

                ASSERT_PASS(Util::parse(&resultTz, INPUT, LENGTH));
                ASSERT_FAIL(Util::parse(&resultTz,     0, LENGTH));

                ASSERT_PASS(Util::parse(  &result, stringRef));
                ASSERT_FAIL(Util::parse(  &result, nullRef));

                ASSERT_PASS(Util::parse(&resultTz, stringRef));
                ASSERT_FAIL(Util::parse(&resultTz, nullRef));
            }

            if (veryVerbose) cout << "\t'Invalid length'" << endl;
            {
                ASSERT_PASS(Util::parse(  &result, INPUT, LENGTH));
                ASSERT_PASS(Util::parse(  &result, INPUT,      0));
                ASSERT_FAIL(Util::parse(  &result, INPUT,     -1));

                ASSERT_PASS(Util::parse(&resultTz, INPUT, LENGTH));
                ASSERT_PASS(Util::parse(&resultTz, INPUT,      0));
                ASSERT_FAIL(Util::parse(&resultTz, INPUT,     -1));
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // GENERATE 'DatetimeTz'
        //
        // Concerns:
        //: 1 The output generated by each method has the expected format and
        //:   contents.
        //:
        //: 2 When sufficient capacity is indicated, the method taking
        //:   'bufferLength' generates a null terminator.
        //:
        //: 3 Each method returns the expected value (the correct character
        //:   count or the supplied 'ostream', depending on the return type).
        //:
        //: 4 The value of the supplied object is unchanged.
        //:
        //: 5 The configuration that is in effect, whether user-supplied or the
        //:   process-wide default, has the desired effect on the output.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'Date' values (one per row) and their corresponding ISO 8601
        //:   string representations.
        //:
        //: 2 In a second table, specify a set of distinct 'Time' values (one
        //:   per row) and their corresponding ISO 8601 string representations.
        //:
        //: 3 In a third table, specify a set of distinct timezone values (one
        //:   per row) and their corresponding ISO 8601 string representations.
        //:
        //: 4 For each element 'R' in the cross product of the tables from P-1,
        //:   P-2, and P-3:  (C-1..5)
        //:
        //:   1 Create a 'const' 'DatetimeTz' object, 'X', from 'R'.
        //:
        //:   2 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, int, const DatetimeTz&);
        //   int generate(char *, int, const DatetimeTz&, const Config&);
        //   int generate(string *, const DatetimeTz&);
        //   int generate(string *, const DatetimeTz&, const Config&);
        //   ostream generate(ostream&, const DatetimeTz&);
        //   ostream generate(ostream&, const DatetimeTz&, const Config&);
        //   int generateRaw(char *, const DatetimeTz&);
        //   int generateRaw(char *, const DatetimeTz&, const Config&);
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //   int generate(char *, const DatetimeTz&, int);
        //   int generate(char *, const DatetimeTz&, int, bool useZ);
        //   ostream generate(ostream&, const DatetimeTz&, bool useZ);
        //   int generateRaw(char *, const DatetimeTz&, bool useZ);
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATE 'DatetimeTz'" << endl
                          << "=====================" << endl;

        typedef bdlt::DatetimeTz TYPE;

        const int OBJLEN = Util::k_DATETIMETZ_STRLEN;
        const int BUFLEN = OBJLEN + 4;

        char buffer[BUFLEN];
        char chaste[BUFLEN];  bsl::memset(chaste, '?', BUFLEN);

        const int                  NUM_DATE_DATA =       NUM_DEFAULT_DATE_DATA;
        const DefaultDateDataRow (&DATE_DATA)[NUM_DATE_DATA] =
                                                             DEFAULT_DATE_DATA;

        const int                  NUM_TIME_DATA =       NUM_DEFAULT_TIME_DATA;
        const DefaultTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] =
                                                             DEFAULT_TIME_DATA;

        const int                  NUM_ZONE_DATA =       NUM_DEFAULT_ZONE_DATA;
        const DefaultZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] =
                                                             DEFAULT_ZONE_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
            const int   ILINE   = DATE_DATA[ti].d_line;
            const int   YEAR    = DATE_DATA[ti].d_year;
            const int   MONTH   = DATE_DATA[ti].d_month;
            const int   DAY     = DATE_DATA[ti].d_day;
            const char *ISO8601 = DATE_DATA[ti].d_iso8601;

            const bdlt::Date  DATE(YEAR, MONTH, DAY);
            const bsl::string EXPECTED_DATE(ISO8601);

            for (int tj = 0; tj < NUM_TIME_DATA; ++tj) {
                const int   JLINE   = TIME_DATA[tj].d_line;
                const int   HOUR    = TIME_DATA[tj].d_hour;
                const int   MIN     = TIME_DATA[tj].d_min;
                const int   SEC     = TIME_DATA[tj].d_sec;
                const int   MSEC    = TIME_DATA[tj].d_msec;
                const int   USEC    = TIME_DATA[tj].d_usec;
                const char *ISO8601 = TIME_DATA[tj].d_iso8601;

                const bdlt::Time  TIME(HOUR, MIN, SEC, MSEC);
                const bsl::string EXPECTED_TIME(ISO8601);

                for (int tk = 0; tk < NUM_ZONE_DATA; ++tk) {
                    const int   KLINE   = ZONE_DATA[tk].d_line;
                    const int   OFFSET  = ZONE_DATA[tk].d_offset;
                    const char *ISO8601 = ZONE_DATA[tk].d_iso8601;

                    const bsl::string EXPECTED_ZONE(ISO8601);

                    if (TIME == bdlt::Time() && OFFSET != 0) {
                        continue;  // skip invalid compositions
                    }

                    const TYPE        X(bdlt::Datetime(YEAR,
                                                       MONTH,
                                                       DAY,
                                                       HOUR,
                                                       MIN,
                                                       SEC,
                                                       MSEC,
                                                       USEC),
                                        OFFSET);
                    const bsl::string BASE_EXPECTED(
                          EXPECTED_DATE + 'T' + EXPECTED_TIME + EXPECTED_ZONE);

                    if (veryVerbose) {
                        T_ P_(ILINE) P_(JLINE) P_(KLINE) P_(X) P(BASE_EXPECTED)
                    }

                    for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                        const int  CLINE     = CNFG_DATA[tc].d_line;
                        const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                        const int  PRECISION = CNFG_DATA[tc].d_precision;
                        const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                        const bool USEZ      = CNFG_DATA[tc].d_useZ;

                        if (veryVerbose) {
                            T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                        }

                        Config mC;  const Config& C = mC;
                        gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                        Config::setDefaultConfiguration(C);

                        bsl::string EXPECTED(BASE_EXPECTED);
                        updateExpectedPerConfig(&EXPECTED,
                                                C,
                                                k_DATETIMETZ_MAX_PRECISION);

                        const int OUTLEN = static_cast<int>(EXPECTED.length());

                        // 'generate' taking 'bufferLength'

                        for (int k = 0; k < BUFLEN; ++k) {
                            bsl::memset(buffer, '?', BUFLEN);

                            if (veryVeryVerbose) {
                                T_ T_ cout << "Length: "; P(k)
                            }

                            ASSERTV(ILINE, JLINE, KLINE, k, OUTLEN,
                                    OUTLEN == Util::generate(buffer, k, X));

                            ASSERTV(ILINE, JLINE, KLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                            if (k <= OUTLEN) {
                                ASSERTV(ILINE, JLINE, KLINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k,
                                                         BUFLEN - k));
                            }
                            else {
                                ASSERTV(ILINE, JLINE, KLINE, k, OUTLEN,
                                        '\0' == buffer[OUTLEN]);

                                ASSERTV(ILINE, JLINE, KLINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k + 1,
                                                         BUFLEN - k - 1));
                            }
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                            // Swap order of 'k' and 'X' in call to 'generate'.
                            {
                                bsl::memset(buffer, '?', BUFLEN);

                                ASSERTV(ILINE, k, OUTLEN,
                                       OUTLEN == Util::generate(buffer, X, k));

                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(
                                                     EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                                if (k <= OUTLEN) {
                                    ASSERTV(ILINE, EXPECTED, buffer,
                                            0 == bsl::memcmp(chaste,
                                                             buffer + k,
                                                             BUFLEN - k));
                                }
                                else {
                                    ASSERTV(ILINE, k, OUTLEN,
                                            '\0' == buffer[OUTLEN]);

                                    ASSERTV(ILINE, EXPECTED, buffer,
                                            0 == bsl::memcmp(chaste,
                                                             buffer + k + 1,
                                                             BUFLEN - k - 1));
                                }
                            }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
                        }

                        // 'generate' to a 'bsl::string'
                        {
                            bsl::string mS("qwerty");

                            ASSERTV(ILINE, JLINE, KLINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X));

                            ASSERTV(ILINE, JLINE, KLINE, EXPECTED, mS,
                                    EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }

                        // 'generate' to an 'std::string'
                        {
                            std::string mS("qwerty");

                            ASSERTV(ILINE, JLINE, KLINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X));

                            ASSERTV(ILINE, JLINE, KLINE, EXPECTED, mS,
                                    EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                        // 'generate' to an 'std::pmr::string'
                        {
                            std::pmr::string mS("qwerty");

                            ASSERTV(ILINE, JLINE, KLINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X));

                            ASSERTV(ILINE, JLINE, KLINE, EXPECTED, mS,
                                    EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }
#endif

                        // 'generate' to an 'ostream'
                        {
                            bsl::ostringstream os;

                            ASSERTV(ILINE, JLINE, KLINE,
                                    &os == &Util::generate(os, X));

                            ASSERTV(ILINE, JLINE, KLINE, EXPECTED, os.str(),
                                    EXPECTED == os.str());

                            if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                        }

                        // 'generateRaw'
                        {
                            bsl::memset(buffer, '?', BUFLEN);

                            ASSERTV(ILINE, JLINE, KLINE, OUTLEN,
                                    OUTLEN == Util::generateRaw(buffer, X));

                            ASSERTV(ILINE, JLINE, KLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     OUTLEN));

                            ASSERTV(ILINE, JLINE, KLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + OUTLEN,
                                                     BUFLEN - OUTLEN));
                        }
                    }  // loop over 'CNFG_DATA'

                    for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                        const int  CLINE     = CNFG_DATA[tc].d_line;
                        const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                        const int  PRECISION = CNFG_DATA[tc].d_precision;
                        const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                        const bool USEZ      = CNFG_DATA[tc].d_useZ;

                        if (veryVerbose) {
                            T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                        }

                        Config mC;  const Config& C = mC;
                        gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                        // Set the default configuration to the complement of
                        // 'C'.

                        Config mDFLT;  const Config& DFLT = mDFLT;
                        gg(&mDFLT,
                           9 - PRECISION,
                           !OMITCOLON,
                           !USECOMMA,
                           !USEZ);
                        Config::setDefaultConfiguration(DFLT);

                        bsl::string EXPECTED(BASE_EXPECTED);
                        updateExpectedPerConfig(&EXPECTED,
                                                C,
                                                k_DATETIMETZ_MAX_PRECISION);

                        const int OUTLEN = static_cast<int>(EXPECTED.length());

                        // 'generate' taking 'bufferLength'

                        for (int k = 0; k < BUFLEN; ++k) {
                            bsl::memset(buffer, '?', BUFLEN);

                            if (veryVeryVerbose) {
                                T_ T_ cout << "Length: "; P(k)
                            }

                            ASSERTV(ILINE, k, OUTLEN,
                                    OUTLEN == Util::generate(buffer, k, X, C));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                            if (k <= OUTLEN) {
                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k,
                                                         BUFLEN - k));
                            }
                            else {
                                ASSERTV(ILINE, k, OUTLEN,
                                        '\0' == buffer[OUTLEN]);

                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k + 1,
                                                         BUFLEN - k - 1));
                            }
                        }

                        // 'generate' to a 'bsl::string'
                        {
                            bsl::string mS("qwerty");

                            ASSERTV(ILINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X, C));

                            ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }

                        // 'generate' to an 'std::string'
                        {
                            std::string mS("qwerty");

                            ASSERTV(ILINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X, C));

                            ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                        // 'generate' to an 'std::pmr::string'
                        {
                            std::pmr::string mS("qwerty");

                            ASSERTV(ILINE, OUTLEN,
                                    OUTLEN == Util::generate(&mS, X, C));

                            ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                            if (veryVerbose) { P_(EXPECTED) P(mS); }
                        }
#endif

                        // 'generate' to an 'ostream'
                        {
                            bsl::ostringstream os;

                            ASSERTV(ILINE, &os == &Util::generate(os, X, C));

                            ASSERTV(ILINE, EXPECTED, os.str(),
                                    EXPECTED == os.str());

                            if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                        }

                        // 'generateRaw'
                        {
                            bsl::memset(buffer, '?', BUFLEN);

                            ASSERTV(ILINE, OUTLEN,
                                    OUTLEN == Util::generateRaw(buffer, X, C));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     OUTLEN));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + OUTLEN,
                                                     BUFLEN - OUTLEN));
                        }
                    }  // loop over 'CNFG_DATA'

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                    // Test methods taking (legacy)
                    // 'bool useZAbbreviationForUtc'.

                    const bool USEZ_CNFG_DATA[] = { false, true };

                    for (int tc = 0; tc < 2; ++tc) {
                        const bool OMITCOLON = true;
                        const int  PRECISION = 3;
                        const bool USECOMMA  = true;
                        const bool USEZ      = USEZ_CNFG_DATA[tc];

                        if (veryVerbose) {
                            T_ P_(OMITCOLON) P_(PRECISION) P_(USECOMMA) P(USEZ)
                        }

                        Config mC;  const Config& C = mC;
                        gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                        // Set the default configuration to use the complement
                        // of 'USEZ'.

                        Config mDFLT;  const Config& DFLT = mDFLT;
                        gg(&mDFLT, PRECISION, OMITCOLON, USECOMMA, !USEZ);
                        Config::setDefaultConfiguration(DFLT);

                        bsl::string EXPECTED(BASE_EXPECTED);
                        updateExpectedPerConfig(&EXPECTED,
                                                C,
                                                k_DATETIMETZ_MAX_PRECISION);

                        const int OUTLEN = static_cast<int>(EXPECTED.length());

                        // 'generate' taking 'bufferLength'

                        for (int k = 0; k < BUFLEN; ++k) {
                            bsl::memset(buffer, '?', BUFLEN);

                            if (veryVeryVerbose) {
                                T_ T_ cout << "Length: "; P(k)
                            }

                            ASSERTV(ILINE, k, OUTLEN,
                                 OUTLEN == Util::generate(buffer, X, k, USEZ));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                            if (k <= OUTLEN) {
                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k,
                                                         BUFLEN - k));
                            }
                            else {
                                ASSERTV(ILINE, k, OUTLEN,
                                        '\0' == buffer[OUTLEN]);

                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k + 1,
                                                         BUFLEN - k - 1));
                            }
                        }

                        // 'generate' to an 'ostream'
                        {
                            bsl::ostringstream os;

                            ASSERTV(ILINE,
                                    &os == &Util::generate(os, X, USEZ));

                            ASSERTV(ILINE, EXPECTED, os.str(),
                                    EXPECTED == os.str());

                            if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                        }

                        // 'generateRaw'
                        {
                            bsl::memset(buffer, '?', BUFLEN);

                            ASSERTV(ILINE, OUTLEN,
                                 OUTLEN == Util::generateRaw(buffer, X, USEZ));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     OUTLEN));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + OUTLEN,
                                                     BUFLEN - OUTLEN));
                        }
                    }  // loop over 'USEZ_CNFG_DATA'
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

                }  // loop over 'ZONE_DATA'
            }  // loop over 'TIME_DATA'
        }  // loop over 'DATE_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN], *pc = 0;

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X));
                ASSERT_SAFE_FAIL(Util::generate(    pc, OBJLEN, X));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X));
                ASSERT_SAFE_FAIL(Util::generate(buffer,     -1, X));

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X, C));
                ASSERT_FAIL(Util::generate(     pc, OBJLEN, X, C));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X, C));
                ASSERT_FAIL(Util::generate(buffer,     -1, X, C));

                bsl::string mSB("qwerty");
                bsl::string *pb = 0;

                ASSERT_PASS(Util::generate(&mSB, X));
                ASSERT_FAIL(Util::generate(  pb, X));

                ASSERT_PASS(Util::generate(&mSB, X, C));
                ASSERT_FAIL(Util::generate(  pb, X, C));

                std::string mSS("qwerty");
                std::string *ps = 0;

                ASSERT_PASS(Util::generate(&mSS, X));
                ASSERT_FAIL(Util::generate(  ps, X));

                ASSERT_PASS(Util::generate(&mSS, X, C));
                ASSERT_FAIL(Util::generate(  ps, X, C));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::string mSP("qwerty");
                std::pmr::string *pp = 0;

                ASSERT_PASS(Util::generate(&mSP, X));
                ASSERT_FAIL(Util::generate(  pp, X));

                ASSERT_PASS(Util::generate(&mSP, X, C));
                ASSERT_FAIL(Util::generate(  pp, X, C));
#endif
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_FAIL(Util::generateRaw(     0, X, C));
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // GENERATE 'TimeTz'
        //
        // Concerns:
        //: 1 The output generated by each method has the expected format and
        //:   contents.
        //:
        //: 2 When sufficient capacity is indicated, the method taking
        //:   'bufferLength' generates a null terminator.
        //:
        //: 3 Each method returns the expected value (the correct character
        //:   count or the supplied 'ostream', depending on the return type).
        //:
        //: 4 The value of the supplied object is unchanged.
        //:
        //: 5 The configuration that is in effect, whether user-supplied or the
        //:   process-wide default, has the desired effect on the output.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'Time' values (one per row) and their corresponding ISO 8601
        //:   string representations.
        //:
        //: 2 In a second table, specify a set of distinct timezone values (one
        //:   per row) and their corresponding ISO 8601 string representations.
        //:
        //: 3 For each element 'R' in the cross product of the tables from P-1
        //:   and P-2:  (C-1..5)
        //:
        //:   1 Create a 'const' 'TimeTz' object, 'X', from 'R'.
        //:
        //:   2 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, int, const TimeTz&);
        //   int generate(char *, int, const TimeTz&, const Config&);
        //   int generate(string *, const TimeTz&);
        //   int generate(string *, const TimeTz&, const Config&);
        //   ostream generate(ostream&, const TimeTz&);
        //   ostream generate(ostream&, const TimeTz&, const Config&);
        //   int generateRaw(char *, const TimeTz&);
        //   int generateRaw(char *, const TimeTz&, const Config&);
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //   int generate(char *, const TimeTz&, int);
        //   int generate(char *, const TimeTz&, int, bool useZ);
        //   ostream generate(ostream&, const TimeTz&, bool useZ);
        //   int generateRaw(char *, const TimeTz&, bool useZ);
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATE 'TimeTz'" << endl
                          << "=================" << endl;

        typedef bdlt::TimeTz TYPE;

        const int OBJLEN = Util::k_TIMETZ_STRLEN;
        const int BUFLEN = OBJLEN + 4;

        char buffer[BUFLEN];
        char chaste[BUFLEN];  bsl::memset(chaste, '?', BUFLEN);

        const int                  NUM_TIME_DATA =       NUM_DEFAULT_TIME_DATA;
        const DefaultTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] =
                                                             DEFAULT_TIME_DATA;

        const int                  NUM_ZONE_DATA =       NUM_DEFAULT_ZONE_DATA;
        const DefaultZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] =
                                                             DEFAULT_ZONE_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        for (int ti = 0; ti < NUM_TIME_DATA; ++ti) {
            const int   ILINE   = TIME_DATA[ti].d_line;
            const int   HOUR    = TIME_DATA[ti].d_hour;
            const int   MIN     = TIME_DATA[ti].d_min;
            const int   SEC     = TIME_DATA[ti].d_sec;
            const int   MSEC    = TIME_DATA[ti].d_msec;
            const int   USEC    = TIME_DATA[ti].d_usec;
            const char *ISO8601 = TIME_DATA[ti].d_iso8601;

            const bdlt::Time  TIME(HOUR, MIN, SEC, MSEC, USEC);
            const bsl::string EXPECTED_TIME(ISO8601);

            for (int tj = 0; tj < NUM_ZONE_DATA; ++tj) {
                const int   JLINE   = ZONE_DATA[tj].d_line;
                const int   OFFSET  = ZONE_DATA[tj].d_offset;
                const char *ISO8601 = ZONE_DATA[tj].d_iso8601;

                const bsl::string EXPECTED_ZONE(ISO8601);

                if (TIME == bdlt::Time() && OFFSET != 0) {
                    continue;  // skip invalid compositions
                }

                const TYPE        X(TIME, OFFSET);
                const bsl::string BASE_EXPECTED(EXPECTED_TIME + EXPECTED_ZONE);

                if (veryVerbose) {
                    T_ P_(ILINE) P_(JLINE) P_(X) P(BASE_EXPECTED)
                }

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    Config::setDefaultConfiguration(C);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED,
                                            C,
                                            k_TIMETZ_MAX_PRECISION);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, JLINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, k, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 k < OUTLEN ? k : OUTLEN));

                        if (k <= OUTLEN) {
                            ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k,
                                                     BUFLEN - k));
                        }
                        else {
                            ASSERTV(ILINE, JLINE, k, OUTLEN,
                                    '\0' == buffer[OUTLEN]);

                            ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k + 1,
                                                     BUFLEN - k - 1));
                        }
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                        // Swap order of 'k' and 'X' in call to 'generate'.
                        {
                            bsl::memset(buffer, '?', BUFLEN);

                            ASSERTV(ILINE, k, OUTLEN,
                                    OUTLEN == Util::generate(buffer, X, k));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                            if (k <= OUTLEN) {
                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k,
                                                         BUFLEN - k));
                            }
                            else {
                                ASSERTV(ILINE, k, OUTLEN,
                                        '\0' == buffer[OUTLEN]);

                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k + 1,
                                                         BUFLEN - k - 1));
                            }
                        }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
                    }

                    // 'generate' to a 'bsl::string'
                    {
                        bsl::string mS("qwerty");

                        ASSERTV(ILINE, JLINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }

                    // 'generate' to an 'std::string'
                    {
                        std::string mS("qwerty");

                        ASSERTV(ILINE, JLINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                    // 'generate' to an 'std::pmr::string'
                    {
                        std::pmr::string mS("qwerty");

                        ASSERTV(ILINE, JLINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }
#endif

                    // 'generate' to an 'ostream'
                    {
                        bsl::ostringstream os;

                        ASSERTV(ILINE, JLINE, &os == &Util::generate(os, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, os.str(),
                                EXPECTED == os.str());

                        if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                    }

                    // 'generateRaw'
                    {
                        bsl::memset(buffer, '?', BUFLEN);

                        ASSERTV(ILINE, JLINE, OUTLEN,
                                OUTLEN == Util::generateRaw(buffer, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 OUTLEN));

                        ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + OUTLEN,
                                                 BUFLEN - OUTLEN));
                    }
                }  // loop over 'CNFG_DATA'

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    // Set the default configuration to the complement of 'C'.

                    Config mDFLT;  const Config& DFLT = mDFLT;
                    gg(&mDFLT, 9 - PRECISION, !OMITCOLON, !USECOMMA, !USEZ);
                    Config::setDefaultConfiguration(DFLT);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED,
                                            C,
                                            k_TIMETZ_MAX_PRECISION);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, k, X, C));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 k < OUTLEN ? k : OUTLEN));

                        if (k <= OUTLEN) {
                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k,
                                                     BUFLEN - k));
                        }
                        else {
                            ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k + 1,
                                                     BUFLEN - k - 1));
                        }
                    }

                    // 'generate' to a 'bsl::string'
                    {
                        bsl::string mS("qwerty");

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X, C));

                        ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }

                    // 'generate' to an 'std::string'
                    {
                       std::string mS("qwerty");

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X, C));

                        ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                    // 'generate' to an 'std::pmr::string'
                    {
                        std::pmr::string mS("qwerty");

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X, C));

                        ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }
#endif

                    // 'generate' to an 'ostream'
                    {
                        bsl::ostringstream os;

                        ASSERTV(ILINE, &os == &Util::generate(os, X, C));

                        ASSERTV(ILINE, EXPECTED, os.str(),
                                EXPECTED == os.str());

                        if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                    }

                    // 'generateRaw'
                    {
                        bsl::memset(buffer, '?', BUFLEN);

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generateRaw(buffer, X, C));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 OUTLEN));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + OUTLEN,
                                                 BUFLEN - OUTLEN));
                    }
                }  // loop over 'CNFG_DATA'

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                // Test methods taking (legacy) 'bool useZAbbreviationForUtc'.

                const bool USEZ_CNFG_DATA[] = { false, true };

                for (int tc = 0; tc < 2; ++tc) {
                    const bool OMITCOLON = true;
                    const int  PRECISION = 3;
                    const bool USECOMMA  = true;
                    const bool USEZ      = USEZ_CNFG_DATA[tc];

                    if (veryVerbose) {
                        T_ P_(OMITCOLON) P_(PRECISION) P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    // Set the default configuration to use the complement of
                    // 'USEZ'.

                    Config mDFLT;  const Config& DFLT = mDFLT;
                    gg(&mDFLT, PRECISION, OMITCOLON, USECOMMA, !USEZ);
                    Config::setDefaultConfiguration(DFLT);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED,
                                            C,
                                            k_TIMETZ_MAX_PRECISION);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, X, k, USEZ));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 k < OUTLEN ? k : OUTLEN));

                        if (k <= OUTLEN) {
                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k,
                                                     BUFLEN - k));
                        }
                        else {
                            ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k + 1,
                                                     BUFLEN - k - 1));
                        }
                    }

                    // 'generate' to an 'ostream'
                    {
                        bsl::ostringstream os;

                        ASSERTV(ILINE, &os == &Util::generate(os, X, USEZ));

                        ASSERTV(ILINE, EXPECTED, os.str(),
                                EXPECTED == os.str());

                        if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                    }

                    // 'generateRaw'
                    {
                        bsl::memset(buffer, '?', BUFLEN);

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generateRaw(buffer, X, USEZ));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 OUTLEN));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + OUTLEN,
                                                 BUFLEN - OUTLEN));
                    }
                }  // loop over 'USEZ_CNFG_DATA'
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

            }  // loop over 'ZONE_DATA'
        }  // loop over 'TIME_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN], *pc = 0;

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X));
                ASSERT_SAFE_FAIL(Util::generate(    pc, OBJLEN, X));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X));
                ASSERT_SAFE_FAIL(Util::generate(buffer,     -1, X));

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X, C));
                ASSERT_FAIL(Util::generate(    pc, OBJLEN, X, C));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X, C));
                ASSERT_FAIL(Util::generate(buffer,     -1, X, C));

                bsl::string mB("qwerty"), *pb = 0;

                ASSERT_PASS(Util::generate(&mB, X));
                ASSERT_FAIL(Util::generate( pb, X));

                ASSERT_PASS(Util::generate(&mB, X, C));
                ASSERT_FAIL(Util::generate( pb, X, C));

                std::string mS("qwerty"), *ps = 0;

                ASSERT_PASS(Util::generate(&mS, X));
                ASSERT_FAIL(Util::generate( ps, X));

                ASSERT_PASS(Util::generate(&mS, X, C));
                ASSERT_FAIL(Util::generate( ps, X, C));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::string mP("qwerty"), *pp = 0;

                ASSERT_PASS(Util::generate(&mP, X));
                ASSERT_FAIL(Util::generate( pp, X));

                ASSERT_PASS(Util::generate(&mP, X, C));
                ASSERT_FAIL(Util::generate( pp, X, C));
#endif
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_FAIL(Util::generateRaw(     0, X, C));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // GENERATE 'DateTz'
        //
        // Concerns:
        //: 1 The output generated by each method has the expected format and
        //:   contents.
        //:
        //: 2 When sufficient capacity is indicated, the method taking
        //:   'bufferLength' generates a null terminator.
        //:
        //: 3 Each method returns the expected value (the correct character
        //:   count or the supplied 'ostream', depending on the return type).
        //:
        //: 4 The value of the supplied object is unchanged.
        //:
        //: 5 The configuration that is in effect, whether user-supplied or the
        //:   process-wide default, has the desired effect on the output.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'Date' values (one per row) and their corresponding ISO 8601
        //:   string representations.
        //:
        //: 2 In a second table, specify a set of distinct timezone values (one
        //:   per row) and their corresponding ISO 8601 string representations.
        //:
        //: 3 For each element 'R' in the cross product of the tables from P-1
        //:   and P-2:  (C-1..5)
        //:
        //:   1 Create a 'const' 'DateTz' object, 'X', from 'R'.
        //:
        //:   2 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, int, const DateTz&);
        //   int generate(char *, int, const DateTz&, const Config&);
        //   int generate(string *, const DateTz&);
        //   int generate(string *, const DateTz&, const Config&);
        //   ostream generate(ostream&, const DateTz&);
        //   ostream generate(ostream&, const DateTz&, const Config&);
        //   int generateRaw(char *, const DateTz&);
        //   int generateRaw(char *, const DateTz&, const Config&);
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //   int generate(char *, const DateTz&, int);
        //   int generate(char *, const DateTz&, int, bool useZ);
        //   ostream generate(ostream&, const DateTz&, bool useZ);
        //   int generateRaw(char *, const DateTz&, bool useZ);
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATE 'DateTz'" << endl
                          << "=================" << endl;

        typedef bdlt::DateTz TYPE;

        const int OBJLEN = Util::k_DATETZ_STRLEN;
        const int BUFLEN = OBJLEN + 4;

        char buffer[BUFLEN];
        char chaste[BUFLEN];  bsl::memset(chaste, '?', BUFLEN);

        const int                  NUM_DATE_DATA =       NUM_DEFAULT_DATE_DATA;
        const DefaultDateDataRow (&DATE_DATA)[NUM_DATE_DATA] =
                                                             DEFAULT_DATE_DATA;

        const int                  NUM_ZONE_DATA =       NUM_DEFAULT_ZONE_DATA;
        const DefaultZoneDataRow (&ZONE_DATA)[NUM_ZONE_DATA] =
                                                             DEFAULT_ZONE_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
            const int   ILINE   = DATE_DATA[ti].d_line;
            const int   YEAR    = DATE_DATA[ti].d_year;
            const int   MONTH   = DATE_DATA[ti].d_month;
            const int   DAY     = DATE_DATA[ti].d_day;
            const char *ISO8601 = DATE_DATA[ti].d_iso8601;

            const bdlt::Date  DATE(YEAR, MONTH, DAY);
            const bsl::string EXPECTED_DATE(ISO8601);

            for (int tj = 0; tj < NUM_ZONE_DATA; ++tj) {
                const int   JLINE   = ZONE_DATA[tj].d_line;
                const int   OFFSET  = ZONE_DATA[tj].d_offset;
                const char *ISO8601 = ZONE_DATA[tj].d_iso8601;

                const bsl::string EXPECTED_ZONE(ISO8601);

                const TYPE        X(DATE, OFFSET);
                const bsl::string BASE_EXPECTED(EXPECTED_DATE + EXPECTED_ZONE);

                if (veryVerbose) {
                    T_ P_(ILINE) P_(JLINE) P_(X) P(BASE_EXPECTED)
                }

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    Config::setDefaultConfiguration(C);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED,
                                            C,
                                            k_DATETZ_MAX_PRECISION);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, JLINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, k, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 k < OUTLEN ? k : OUTLEN));

                        if (k <= OUTLEN) {
                            ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k,
                                                     BUFLEN - k));
                        }
                        else {
                            ASSERTV(ILINE, JLINE, k, OUTLEN,
                                    '\0' == buffer[OUTLEN]);

                            ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k + 1,
                                                     BUFLEN - k - 1));
                        }
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                        // Swap order of 'k' and 'X' in call to 'generate'.
                        {
                            bsl::memset(buffer, '?', BUFLEN);

                            ASSERTV(ILINE, k, OUTLEN,
                                    OUTLEN == Util::generate(buffer, X, k));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                            if (k <= OUTLEN) {
                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k,
                                                         BUFLEN - k));
                            }
                            else {
                                ASSERTV(ILINE, k, OUTLEN,
                                        '\0' == buffer[OUTLEN]);

                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k + 1,
                                                         BUFLEN - k - 1));
                            }
                        }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
                    }

                    // 'generate' to a 'bsl::string'
                    {
                        bsl::string mS("qwerty");

                        ASSERTV(ILINE, JLINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }

                    // 'generate' to an 'std::string'
                    {
                        std::string mS("qwerty");

                        ASSERTV(ILINE, JLINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                    // 'generate' to an 'std::pmr::string'
                    {
                        std::pmr::string mS("qwerty");

                        ASSERTV(ILINE, JLINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }
#endif

                    // 'generate' to an 'ostream'
                    {
                        bsl::ostringstream os;

                        ASSERTV(ILINE, JLINE, &os == &Util::generate(os, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, os.str(),
                                EXPECTED == os.str());

                        if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                    }

                    // 'generateRaw'
                    {
                        bsl::memset(buffer, '?', BUFLEN);

                        ASSERTV(ILINE, JLINE, OUTLEN,
                                OUTLEN == Util::generateRaw(buffer, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 OUTLEN));

                        ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + OUTLEN,
                                                 BUFLEN - OUTLEN));
                    }
                }  // loop over 'CNFG_DATA'

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    // Set the default configuration to the complement of 'C'.

                    Config mDFLT;  const Config& DFLT = mDFLT;
                    gg(&mDFLT, 9 - PRECISION, !OMITCOLON, !USECOMMA, !USEZ);
                    Config::setDefaultConfiguration(DFLT);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED,
                                            C,
                                            k_DATETZ_MAX_PRECISION);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, k, X, C));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 k < OUTLEN ? k : OUTLEN));

                        if (k <= OUTLEN) {
                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k,
                                                     BUFLEN - k));
                        }
                        else {
                            ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k + 1,
                                                     BUFLEN - k - 1));
                        }
                    }

                    // 'generate' to a 'bsl::string'
                    {
                        bsl::string mS("qwerty");

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X, C));

                        ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }

                    // 'generate' to an 'std::string'
                    {
                        std::string mS("qwerty");

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X, C));

                        ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                    // 'generate' to an 'std::pmr::string'
                    {
                        std::pmr::string mS("qwerty");

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X, C));

                        ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }
#endif

                    // 'generate' to an 'ostream'
                    {
                        bsl::ostringstream os;

                        ASSERTV(ILINE, &os == &Util::generate(os, X, C));

                        ASSERTV(ILINE, EXPECTED, os.str(),
                                EXPECTED == os.str());

                        if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                    }

                    // 'generateRaw'
                    {
                        bsl::memset(buffer, '?', BUFLEN);

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generateRaw(buffer, X, C));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 OUTLEN));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + OUTLEN,
                                                 BUFLEN - OUTLEN));
                    }
                }  // loop over 'CNFG_DATA'

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                // Test methods taking (legacy) 'bool useZAbbreviationForUtc'.

                const bool USEZ_CNFG_DATA[] = { false, true };

                for (int tc = 0; tc < 2; ++tc) {
                    const bool OMITCOLON = true;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = true;
                    const bool USEZ      = USEZ_CNFG_DATA[tc];

                    if (veryVerbose) {
                        T_ P_(OMITCOLON) P_(PRECISION) P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    // Set the default configuration to use the complement of
                    // 'USEZ'.

                    Config mDFLT;  const Config& DFLT = mDFLT;
                    gg(&mDFLT, PRECISION, OMITCOLON, USECOMMA, !USEZ);
                    Config::setDefaultConfiguration(DFLT);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED,
                                            C,
                                            k_DATETZ_MAX_PRECISION);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, X, k, USEZ));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 k < OUTLEN ? k : OUTLEN));

                        if (k <= OUTLEN) {
                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k,
                                                     BUFLEN - k));
                        }
                        else {
                            ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k + 1,
                                                     BUFLEN - k - 1));
                        }
                    }

                    // 'generate' to an 'ostream'
                    {
                        bsl::ostringstream os;

                        ASSERTV(ILINE, &os == &Util::generate(os, X, USEZ));

                        ASSERTV(ILINE, EXPECTED, os.str(),
                                EXPECTED == os.str());

                        if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                    }

                    // 'generateRaw'
                    {
                        bsl::memset(buffer, '?', BUFLEN);

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generateRaw(buffer, X, USEZ));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 OUTLEN));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + OUTLEN,
                                                 BUFLEN - OUTLEN));
                    }
                }  // loop over 'USEZ_CNFG_DATA'
#endif  // BDE_OMIT_INTERNAL_DEPRECATED

            }  // loop over 'ZONE_DATA'
        }  // loop over 'DATE_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN], *pc = 0;

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X));
                ASSERT_SAFE_FAIL(Util::generate(    pc, OBJLEN, X));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X));
                ASSERT_SAFE_FAIL(Util::generate(buffer,     -1, X));

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X, C));
                ASSERT_FAIL(Util::generate(    pc, OBJLEN, X, C));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X, C));
                ASSERT_FAIL(Util::generate(buffer,     -1, X, C));

                bsl::string mB("qwerty"), *pb = 0;

                ASSERT_PASS(Util::generate(&mB, X));
                ASSERT_FAIL(Util::generate( pb, X));

                ASSERT_PASS(Util::generate(&mB, X, C));
                ASSERT_FAIL(Util::generate( pb, X, C));

                std::string mS("qwerty"), *ps = 0;

                ASSERT_PASS(Util::generate(&mS, X));
                ASSERT_FAIL(Util::generate( ps, X));

                ASSERT_PASS(Util::generate(&mS, X, C));
                ASSERT_FAIL(Util::generate( ps, X, C));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::string mP("qwerty"), *pp = 0;

                ASSERT_PASS(Util::generate(&mP, X));
                ASSERT_FAIL(Util::generate( pp, X));

                ASSERT_PASS(Util::generate(&mP, X, C));
                ASSERT_FAIL(Util::generate( pp, X, C));
#endif
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN], *pc = 0;

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(    pc, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_FAIL(Util::generateRaw(    pc, X, C));
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // GENERATE 'Datetime'
        //
        // Concerns:
        //: 1 The output generated by each method has the expected format and
        //:   contents.
        //:
        //: 2 When sufficient capacity is indicated, the method taking
        //:   'bufferLength' generates a null terminator.
        //:
        //: 3 Each method returns the expected value (the correct character
        //:   count or the supplied 'ostream', depending on the return type).
        //:
        //: 4 The value of the supplied object is unchanged.
        //:
        //: 5 The configuration that is in effect, whether user-supplied or the
        //:   process-wide default, has the desired effect on the output.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'Date' values (one per row) and their corresponding ISO 8601
        //:   string representations.
        //:
        //: 2 In a second table, specify a set of distinct 'Time' values (one
        //:   per row) and their corresponding ISO 8601 string representations.
        //:
        //: 3 For each element 'R' in the cross product of the tables from P-1
        //:   and P-2:  (C-1..5)
        //:
        //:   1 Create a 'const' 'Datetime' object, 'X', from 'R'.
        //:
        //:   2 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, int, const Datetime&);
        //   int generate(char *, int, const Datetime&, const Config&);
        //   int generate(string *, const Datetime&);
        //   int generate(string *, const Datetime&, const Config&);
        //   ostream generate(ostream&, const Datetime&);
        //   ostream generate(ostream&, const Datetime&, const Config&);
        //   int generateRaw(char *, const Datetime&);
        //   int generateRaw(char *, const Datetime&, const Config&);
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //   int generate(char *, const Datetime&, int);
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATE 'Datetime'" << endl
                          << "===================" << endl;

        typedef bdlt::Datetime TYPE;

        const int OBJLEN = Util::k_DATETIME_STRLEN;
        const int BUFLEN = OBJLEN + 4;

        char buffer[BUFLEN];
        char chaste[BUFLEN];  bsl::memset(chaste, '?', BUFLEN);

        const int                  NUM_DATE_DATA =       NUM_DEFAULT_DATE_DATA;
        const DefaultDateDataRow (&DATE_DATA)[NUM_DATE_DATA] =
                                                             DEFAULT_DATE_DATA;

        const int                  NUM_TIME_DATA =       NUM_DEFAULT_TIME_DATA;
        const DefaultTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] =
                                                             DEFAULT_TIME_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
            const int   ILINE   = DATE_DATA[ti].d_line;
            const int   YEAR    = DATE_DATA[ti].d_year;
            const int   MONTH   = DATE_DATA[ti].d_month;
            const int   DAY     = DATE_DATA[ti].d_day;
            const char *ISO8601 = DATE_DATA[ti].d_iso8601;

            const bdlt::Date  DATE(YEAR, MONTH, DAY);
            const bsl::string EXPECTED_DATE(ISO8601);

            for (int tj = 0; tj < NUM_TIME_DATA; ++tj) {
                const int   JLINE   = TIME_DATA[tj].d_line;
                const int   HOUR    = TIME_DATA[tj].d_hour;
                const int   MIN     = TIME_DATA[tj].d_min;
                const int   SEC     = TIME_DATA[tj].d_sec;
                const int   MSEC    = TIME_DATA[tj].d_msec;
                const int   USEC    = TIME_DATA[tj].d_usec;
                const char *ISO8601 = TIME_DATA[tj].d_iso8601;

                const bsl::string EXPECTED_TIME(ISO8601);

                const TYPE        X(YEAR,
                                    MONTH,
                                    DAY,
                                    HOUR,
                                    MIN,
                                    SEC,
                                    MSEC,
                                    USEC);
                const bsl::string BASE_EXPECTED(
                                          EXPECTED_DATE + 'T' + EXPECTED_TIME);

                if (veryVerbose) {
                    T_ P_(ILINE) P_(JLINE) P_(X) P(BASE_EXPECTED)
                }

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    Config::setDefaultConfiguration(C);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED,
                                            C,
                                            k_DATETIME_MAX_PRECISION);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, JLINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, k, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 k < OUTLEN ? k : OUTLEN));

                        if (k <= OUTLEN) {
                            ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k,
                                                     BUFLEN - k));
                        }
                        else {
                            ASSERTV(ILINE, JLINE, k, OUTLEN,
                                    '\0' == buffer[OUTLEN]);

                            ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k + 1,
                                                     BUFLEN - k - 1));
                        }
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                        // Swap order of 'k' and 'X' in call to 'generate'.
                        {
                            bsl::memset(buffer, '?', BUFLEN);

                            ASSERTV(ILINE, k, OUTLEN,
                                    OUTLEN == Util::generate(buffer, X, k));

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(EXPECTED.c_str(),
                                                     buffer,
                                                     k < OUTLEN ? k : OUTLEN));

                            if (k <= OUTLEN) {
                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k,
                                                         BUFLEN - k));
                            }
                            else {
                                ASSERTV(ILINE, k, OUTLEN,
                                        '\0' == buffer[OUTLEN]);

                                ASSERTV(ILINE, EXPECTED, buffer,
                                        0 == bsl::memcmp(chaste,
                                                         buffer + k + 1,
                                                         BUFLEN - k - 1));
                            }
                        }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
                    }

                    // 'generate' to a 'bsl::string'
                    {
                        bsl::string mS("qwerty");

                        ASSERTV(ILINE, JLINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }

                    // 'generate' to an 'std::string'
                    {
                        std::string mS("qwerty");

                        ASSERTV(ILINE, JLINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                    // 'generate' to an 'std::pmr::string'
                    {
                        std::pmr::string mS("qwerty");

                        ASSERTV(ILINE, JLINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }
#endif

                    // 'generate' to an 'ostream'
                    {
                        bsl::ostringstream os;

                        ASSERTV(ILINE, JLINE, &os == &Util::generate(os, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, os.str(),
                                EXPECTED == os.str());

                        if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                    }

                    // 'generateRaw'
                    {
                        bsl::memset(buffer, '?', BUFLEN);

                        ASSERTV(ILINE, JLINE, OUTLEN,
                                OUTLEN == Util::generateRaw(buffer, X));

                        ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 OUTLEN));

                        ASSERTV(ILINE, JLINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + OUTLEN,
                                                 BUFLEN - OUTLEN));
                    }
                }  // loop over 'CNFG_DATA'

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const int  PRECISION = CNFG_DATA[tc].d_precision;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                    // Set the default configuration to the complement of 'C'.

                    Config mDFLT;  const Config& DFLT = mDFLT;
                    gg(&mDFLT, 9 - PRECISION, !OMITCOLON, !USECOMMA, !USEZ);
                    Config::setDefaultConfiguration(DFLT);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED,
                                            C,
                                            k_DATETIME_MAX_PRECISION);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, k, X, C));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 k < OUTLEN ? k : OUTLEN));

                        if (k <= OUTLEN) {
                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k,
                                                     BUFLEN - k));
                        }
                        else {
                            ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k + 1,
                                                     BUFLEN - k - 1));
                        }
                    }

                    // 'generate' to a 'bsl::string'
                    {
                        bsl::string mS("qwerty");

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X, C));

                        ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }

                    // 'generate' to an 'std::string'
                    {
                        std::string mS("qwerty");

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X, C));

                        ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                    // 'generate' to an 'std::pmr::string'
                    {
                        std::pmr::string mS("qwerty");

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generate(&mS, X, C));

                        ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                        if (veryVerbose) { P_(EXPECTED) P(mS); }
                    }
#endif

                    // 'generate' to an 'ostream'
                    {
                        bsl::ostringstream os;

                        ASSERTV(ILINE, &os == &Util::generate(os, X, C));

                        ASSERTV(ILINE, EXPECTED, os.str(),
                                EXPECTED == os.str());

                        if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                    }

                    // 'generateRaw'
                    {
                        bsl::memset(buffer, '?', BUFLEN);

                        ASSERTV(ILINE, OUTLEN,
                                OUTLEN == Util::generateRaw(buffer, X, C));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 OUTLEN));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + OUTLEN,
                                                 BUFLEN - OUTLEN));
                    }
                }  // loop over 'CNFG_DATA'
            }  // loop over 'TIME_DATA'
        }  // loop over 'DATE_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN], *pc = 0;

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X));
                ASSERT_SAFE_FAIL(Util::generate(    pc, OBJLEN, X));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X));
                ASSERT_SAFE_FAIL(Util::generate(buffer,     -1, X));

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X, C));
                ASSERT_FAIL(Util::generate(    pc, OBJLEN, X, C));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X, C));
                ASSERT_FAIL(Util::generate(buffer,     -1, X, C));

                bsl::string mB("qwerty"), *pb = 0;

                ASSERT_PASS(Util::generate(&mB, X));
                ASSERT_FAIL(Util::generate( pb, X));

                ASSERT_PASS(Util::generate(&mB, X, C));
                ASSERT_FAIL(Util::generate( pb, X, C));

                std::string mS("qwerty"), *ps = 0;

                ASSERT_PASS(Util::generate(&mS, X));
                ASSERT_FAIL(Util::generate( ps, X));

                ASSERT_PASS(Util::generate(&mS, X, C));
                ASSERT_FAIL(Util::generate( ps, X, C));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::string mP("qwerty"), *pp = 0;

                ASSERT_PASS(Util::generate(&mP, X));
                ASSERT_FAIL(Util::generate( pp, X));

                ASSERT_PASS(Util::generate(&mP, X, C));
                ASSERT_FAIL(Util::generate( pp, X, C));
#endif
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_FAIL(Util::generateRaw(     0, X, C));
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // GENERATE 'Time'
        //
        // Concerns:
        //: 1 The output generated by each method has the expected format and
        //:   contents.
        //:
        //: 2 When sufficient capacity is indicated, the method taking
        //:   'bufferLength' generates a null terminator.
        //:
        //: 3 Each method returns the expected value (the correct character
        //:   count or the supplied 'ostream', depending on the return type).
        //:
        //: 4 The value of the supplied object is unchanged.
        //:
        //: 5 The configuration that is in effect, whether user-supplied or the
        //:   process-wide default, has the desired effect on the output.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'Time' values (one per row) and their corresponding ISO 8601
        //:   string representations.
        //:
        //: 2 For each row 'R' in the table from P-1:  (C-1..5)
        //:
        //:   1 Create a 'const' 'Time' object, 'X', from 'R'.
        //:
        //:   2 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, int, const Time&);
        //   int generate(char *, int, const Time&, const Config&);
        //   int generate(string *, const Time&);
        //   int generate(string *, const Time&, const Config&);
        //   ostream generate(ostream&, const Time&);
        //   ostream generate(ostream&, const Time&, const Config&);
        //   int generateRaw(char *, const Time&);
        //   int generateRaw(char *, const Time&, const Config&);
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //   int generate(char *, const Time&, int);
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATE 'Time'" << endl
                          << "===============" << endl;

        typedef bdlt::Time TYPE;

        const int OBJLEN = Util::k_TIME_STRLEN;
        const int BUFLEN = OBJLEN + 4;

        char buffer[BUFLEN];
        char chaste[BUFLEN];  bsl::memset(chaste, '?', BUFLEN);

        const int                  NUM_TIME_DATA =       NUM_DEFAULT_TIME_DATA;
        const DefaultTimeDataRow (&TIME_DATA)[NUM_TIME_DATA] =
                                                             DEFAULT_TIME_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        for (int ti = 0; ti < NUM_TIME_DATA; ++ti) {
            const int   ILINE   = TIME_DATA[ti].d_line;
            const int   HOUR    = TIME_DATA[ti].d_hour;
            const int   MIN     = TIME_DATA[ti].d_min;
            const int   SEC     = TIME_DATA[ti].d_sec;
            const int   MSEC    = TIME_DATA[ti].d_msec;
            const int   USEC    = TIME_DATA[ti].d_usec;
            const char *ISO8601 = TIME_DATA[ti].d_iso8601;

            const TYPE        X(HOUR, MIN, SEC, MSEC, USEC);
            const bsl::string BASE_EXPECTED(ISO8601);

            if (veryVerbose) { T_ P_(ILINE) P_(X) P(BASE_EXPECTED) }

            for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                const int  CLINE     = CNFG_DATA[tc].d_line;
                const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                const int  PRECISION = CNFG_DATA[tc].d_precision;
                const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                const bool USEZ      = CNFG_DATA[tc].d_useZ;

                if (veryVerbose) {
                    T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                }

                Config mC;  const Config& C = mC;
                gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                Config::setDefaultConfiguration(C);

                bsl::string EXPECTED(BASE_EXPECTED);
                updateExpectedPerConfig(&EXPECTED,
                                        C,
                                        k_TIME_MAX_PRECISION);

                const int OUTLEN = static_cast<int>(EXPECTED.length());

                // 'generate' taking 'bufferLength'

                for (int k = 0; k < BUFLEN; ++k) {
                    bsl::memset(buffer, '?', BUFLEN);

                    if (veryVeryVerbose) {
                        T_ T_ cout << "Length: "; P(k)
                    }

                    ASSERTV(ILINE, k, OUTLEN,
                            OUTLEN == Util::generate(buffer, k, X));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(EXPECTED.c_str(),
                                             buffer,
                                             k < OUTLEN ? k : OUTLEN));

                    if (k <= OUTLEN) {
                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + k,
                                                 BUFLEN - k));
                    }
                    else {
                        ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + k + 1,
                                                 BUFLEN - k - 1));
                    }
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                    // Swap order of 'k' and 'X' in call to 'generate'.
                    {
                        bsl::memset(buffer, '?', BUFLEN);

                        ASSERTV(ILINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, X, k));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 k < OUTLEN ? k : OUTLEN));

                        if (k <= OUTLEN) {
                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k,
                                                     BUFLEN - k));
                        }
                        else {
                            ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k + 1,
                                                     BUFLEN - k - 1));
                        }
                    }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
                }

                // 'generate' to a 'bsl::string'
                {
                    bsl::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN, OUTLEN == Util::generate(&mS, X));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }

                // 'generate' to an 'std::string'
                {
                    std::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN, OUTLEN == Util::generate(&mS, X));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                // 'generate' to an 'std::pmr::string'
                {
                    std::pmr::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN, OUTLEN == Util::generate(&mS, X));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }
#endif

                // 'generate' to an 'ostream'
                {
                    bsl::ostringstream os;

                    ASSERTV(ILINE, &os == &Util::generate(os, X));

                    ASSERTV(ILINE, EXPECTED, os.str(), EXPECTED == os.str());

                    if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                }

                // 'generateRaw'
                {
                    bsl::memset(buffer, '?', BUFLEN);

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generateRaw(buffer, X));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(EXPECTED.c_str(),
                                             buffer,
                                             OUTLEN));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(chaste,
                                             buffer + OUTLEN,
                                             BUFLEN - OUTLEN));
                }
            }  // loop over 'CNFG_DATA'

            for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                const int  CLINE     = CNFG_DATA[tc].d_line;
                const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                const int  PRECISION = CNFG_DATA[tc].d_precision;
                const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                const bool USEZ      = CNFG_DATA[tc].d_useZ;

                if (veryVerbose) {
                    T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                }

                Config mC;  const Config& C = mC;
                gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                // Set the default configuration to the complement of 'C'.

                Config mDFLT;  const Config& DFLT = mDFLT;
                gg(&mDFLT, 9 - PRECISION, !OMITCOLON, !USECOMMA, !USEZ);
                Config::setDefaultConfiguration(DFLT);

                bsl::string EXPECTED(BASE_EXPECTED);
                updateExpectedPerConfig(&EXPECTED,
                                        C,
                                        k_TIME_MAX_PRECISION);

                const int OUTLEN = static_cast<int>(EXPECTED.length());

                // 'generate' taking 'bufferLength'

                for (int k = 0; k < BUFLEN; ++k) {
                    bsl::memset(buffer, '?', BUFLEN);

                    if (veryVeryVerbose) {
                        T_ T_ cout << "Length: "; P(k)
                    }

                    ASSERTV(ILINE, k, OUTLEN,
                            OUTLEN == Util::generate(buffer, k, X, C));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(EXPECTED.c_str(),
                                             buffer,
                                             k < OUTLEN ? k : OUTLEN));

                    if (k <= OUTLEN) {
                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + k,
                                                 BUFLEN - k));
                    }
                    else {
                        ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + k + 1,
                                                 BUFLEN - k - 1));
                    }
                }

                // 'generate' to a 'bsl::string'
                {
                    bsl::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generate(&mS, X, C));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }

                // 'generate' to an 'std::string'
                {
                    std::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generate(&mS, X, C));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                // 'generate' to an 'std::pmr::string'
                {
                    std::pmr::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generate(&mS, X, C));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }
#endif

                // 'generate' to an 'ostream'
                {
                    bsl::ostringstream os;

                    ASSERTV(ILINE, &os == &Util::generate(os, X, C));

                    ASSERTV(ILINE, EXPECTED, os.str(), EXPECTED == os.str());

                    if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                }

                // 'generateRaw'
                {
                    bsl::memset(buffer, '?', BUFLEN);

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generateRaw(buffer, X, C));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(EXPECTED.c_str(),
                                             buffer,
                                             OUTLEN));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(chaste,
                                             buffer + OUTLEN,
                                             BUFLEN - OUTLEN));
                }
            }  // loop over 'CNFG_DATA'
        }  // loop over 'TIME_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN], *pc = 0;

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X));
                ASSERT_SAFE_FAIL(Util::generate(    pc, OBJLEN, X));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X));
                ASSERT_SAFE_FAIL(Util::generate(buffer,     -1, X));

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X, C));
                ASSERT_FAIL(Util::generate(    pc, OBJLEN, X, C));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X, C));
                ASSERT_FAIL(Util::generate(buffer,     -1, X, C));

                bsl::string mB("qwerty"), *pb = 0;

                ASSERT_PASS(Util::generate(&mB, X));
                ASSERT_FAIL(Util::generate( pb, X));

                ASSERT_PASS(Util::generate(&mB, X, C));
                ASSERT_FAIL(Util::generate( pb, X, C));

                std::string mS("qwerty"), *ps = 0;

                ASSERT_PASS(Util::generate(&mS, X));
                ASSERT_FAIL(Util::generate( ps, X));

                ASSERT_PASS(Util::generate(&mS, X, C));
                ASSERT_FAIL(Util::generate( ps, X, C));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::string mP("qwerty"), *pp = 0;

                ASSERT_PASS(Util::generate(&mP, X));
                ASSERT_FAIL(Util::generate( pp, X));

                ASSERT_PASS(Util::generate(&mP, X, C));
                ASSERT_FAIL(Util::generate( pp, X, C));
#endif
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN], *pc = 0;

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(    pc, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_FAIL(Util::generateRaw(    pc, X, C));
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // GENERATE 'Date'
        //
        // Concerns:
        //: 1 The output generated by each method has the expected format and
        //:   contents.
        //:
        //: 2 When sufficient capacity is indicated, the method taking
        //:   'bufferLength' generates a null terminator.
        //:
        //: 3 Each method returns the expected value (the correct character
        //:   count or the supplied 'ostream', depending on the return type).
        //:
        //: 4 The value of the supplied object is unchanged.
        //:
        //: 5 The configuration that is in effect, whether user-supplied or the
        //:   process-wide default, has the desired effect on the output.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'Date' values (one per row) and their corresponding ISO 8601
        //:   string representations.
        //:
        //: 2 For each row 'R' in the table from P-1:  (C-1..5)
        //:
        //:   1 Create a 'const' 'Date' object, 'X', from 'R'.
        //:
        //:   2 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, int, const Date&);
        //   int generate(char *, int, const Date&, const Config&);
        //   int generate(string *, const Date&);
        //   int generate(string *, const Date&, const Config&);
        //   ostream generate(ostream&, const Date&);
        //   ostream generate(ostream&, const Date&, const Config&);
        //   int generateRaw(char *, const Date&);
        //   int generateRaw(char *, const Date&, const Config&);
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
        //   int generate(char *, const Date&, int);
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATE 'Date'" << endl
                          << "===============" << endl;

        typedef bdlt::Date TYPE;

        const int OBJLEN = Util::k_DATE_STRLEN;
        const int BUFLEN = OBJLEN + 4;

        char buffer[BUFLEN];
        char chaste[BUFLEN];  bsl::memset(chaste, '?', BUFLEN);

        const int                  NUM_DATE_DATA =       NUM_DEFAULT_DATE_DATA;
        const DefaultDateDataRow (&DATE_DATA)[NUM_DATE_DATA] =
                                                             DEFAULT_DATE_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        for (int ti = 0; ti < NUM_DATE_DATA; ++ti) {
            const int   ILINE   = DATE_DATA[ti].d_line;
            const int   YEAR    = DATE_DATA[ti].d_year;
            const int   MONTH   = DATE_DATA[ti].d_month;
            const int   DAY     = DATE_DATA[ti].d_day;
            const char *ISO8601 = DATE_DATA[ti].d_iso8601;

            const TYPE        X(YEAR, MONTH, DAY);
            const bsl::string BASE_EXPECTED(ISO8601);

            if (veryVerbose) { T_ P_(ILINE) P_(X) P(BASE_EXPECTED) }

            for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                const int  CLINE     = CNFG_DATA[tc].d_line;
                const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                const int  PRECISION = CNFG_DATA[tc].d_precision;
                const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                const bool USEZ      = CNFG_DATA[tc].d_useZ;

                if (veryVerbose) {
                    T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                }

                Config mC;  const Config& C = mC;
                gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                Config::setDefaultConfiguration(C);

                bsl::string EXPECTED(BASE_EXPECTED);
                updateExpectedPerConfig(&EXPECTED,
                                        C,
                                        k_DATE_MAX_PRECISION);

                const int OUTLEN = static_cast<int>(EXPECTED.length());

                // 'generate' taking 'bufferLength'

                for (int k = 0; k < BUFLEN; ++k) {
                    bsl::memset(buffer, '?', BUFLEN);

                    if (veryVeryVerbose) {
                        T_ T_ cout << "Length: "; P(k)
                    }

                    ASSERTV(ILINE, k, OUTLEN,
                            OUTLEN == Util::generate(buffer, k, X));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(EXPECTED.c_str(),
                                             buffer,
                                             k < OUTLEN ? k : OUTLEN));

                    if (k <= OUTLEN) {
                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + k,
                                                 BUFLEN - k));
                    }
                    else {
                        ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + k + 1,
                                                 BUFLEN - k - 1));
                    }
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
                    // Swap order of 'k' and 'X' in call to 'generate'.
                    {
                        bsl::memset(buffer, '?', BUFLEN);

                        ASSERTV(ILINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, X, k));

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(EXPECTED.c_str(),
                                                 buffer,
                                                 k < OUTLEN ? k : OUTLEN));

                        if (k <= OUTLEN) {
                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k,
                                                     BUFLEN - k));
                        }
                        else {
                            ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                            ASSERTV(ILINE, EXPECTED, buffer,
                                    0 == bsl::memcmp(chaste,
                                                     buffer + k + 1,
                                                     BUFLEN - k - 1));
                        }
                    }
#endif  // BDE_OMIT_INTERNAL_DEPRECATED
                }

                // 'generate' to a 'bsl::string'
                {
                    bsl::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN, OUTLEN == Util::generate(&mS, X));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }

                // 'generate' to an 'std::string'
                {
                    std::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN, OUTLEN == Util::generate(&mS, X));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                // 'generate' to an 'std::pmr::string'
                {
                    std::pmr::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN, OUTLEN == Util::generate(&mS, X));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }
#endif

                // 'generate' to an 'ostream'
                {
                    bsl::ostringstream os;

                    ASSERTV(ILINE, &os == &Util::generate(os, X));

                    ASSERTV(ILINE, EXPECTED, os.str(), EXPECTED == os.str());

                    if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                }

                // 'generateRaw'
                {
                    bsl::memset(buffer, '?', BUFLEN);

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generateRaw(buffer, X));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(EXPECTED.c_str(),
                                             buffer,
                                             OUTLEN));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(chaste,
                                             buffer + OUTLEN,
                                             BUFLEN - OUTLEN));
                }
            }  // loop over 'CNFG_DATA'

            for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                const int  CLINE     = CNFG_DATA[tc].d_line;
                const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                const int  PRECISION = CNFG_DATA[tc].d_precision;
                const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                const bool USEZ      = CNFG_DATA[tc].d_useZ;

                if (veryVerbose) {
                    T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                }

                Config mC;  const Config& C = mC;
                gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                // Set the default configuration to the complement of 'C'.

                Config mDFLT;  const Config& DFLT = mDFLT;
                gg(&mDFLT, 9 - PRECISION, !OMITCOLON, !USECOMMA, !USEZ);
                Config::setDefaultConfiguration(DFLT);

                bsl::string EXPECTED(BASE_EXPECTED);
                updateExpectedPerConfig(&EXPECTED,
                                        C,
                                        k_DATE_MAX_PRECISION);

                const int OUTLEN = static_cast<int>(EXPECTED.length());

                // 'generate' taking 'bufferLength'

                for (int k = 0; k < BUFLEN; ++k) {
                    bsl::memset(buffer, '?', BUFLEN);

                    if (veryVeryVerbose) {
                        T_ T_ cout << "Length: "; P(k)
                    }

                    ASSERTV(ILINE, k, OUTLEN,
                            OUTLEN == Util::generate(buffer, k, X, C));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(EXPECTED.c_str(),
                                             buffer,
                                             k < OUTLEN ? k : OUTLEN));

                    if (k <= OUTLEN) {
                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + k,
                                                 BUFLEN - k));
                    }
                    else {
                        ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + k + 1,
                                                 BUFLEN - k - 1));
                    }
                }

                // 'generate' to a 'bsl::string'
                {
                    bsl::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generate(&mS, X, C));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }

                // 'generate' to an 'std::string'
                {
                    std::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generate(&mS, X, C));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                // 'generate' to an 'std::pmr::string'
                {
                    std::pmr::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generate(&mS, X, C));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }
#endif

                // 'generate' to an 'ostream'
                {
                    bsl::ostringstream os;

                    ASSERTV(ILINE, &os == &Util::generate(os, X, C));

                    ASSERTV(ILINE, EXPECTED, os.str(), EXPECTED == os.str());

                    if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                }

                // 'generateRaw'
                {
                    bsl::memset(buffer, '?', BUFLEN);

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generateRaw(buffer, X, C));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(EXPECTED.c_str(),
                                             buffer,
                                             OUTLEN));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(chaste,
                                             buffer + OUTLEN,
                                             BUFLEN - OUTLEN));
                }
            }  // loop over 'CNFG_DATA'
        }  // loop over 'DATE_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN], *pc = 0;

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X));
                ASSERT_SAFE_FAIL(Util::generate(    pc, OBJLEN, X));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X));
                ASSERT_SAFE_FAIL(Util::generate(buffer,     -1, X));

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X, C));
                ASSERT_FAIL(Util::generate(    pc, OBJLEN, X, C));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X, C));
                ASSERT_FAIL(Util::generate(buffer,     -1, X, C));

                bsl::string mB("qwerty"), *pb = 0;

                ASSERT_PASS(Util::generate(&mB, X));
                ASSERT_FAIL(Util::generate( pb, X));

                ASSERT_PASS(Util::generate(&mB, X, C));
                ASSERT_FAIL(Util::generate( pb, X, C));

                bsl::string mS("qwerty"), *ps = 0;

                ASSERT_PASS(Util::generate(&mS, X));
                ASSERT_FAIL(Util::generate( ps, X));

                ASSERT_PASS(Util::generate(&mS, X, C));
                ASSERT_FAIL(Util::generate( ps, X, C));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::string mP("qwerty"), *pp = 0;

                ASSERT_PASS(Util::generate(&mP, X));
                ASSERT_FAIL(Util::generate( pp, X));

                ASSERT_PASS(Util::generate(&mP, X, C));
                ASSERT_FAIL(Util::generate( pp, X, C));
#endif
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_FAIL(Util::generateRaw(     0, X, C));
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // GENERATE 'TimeInterval'
        //
        // Concerns:
        //: 1 The output generated by each method has the expected format and
        //:   contents.
        //:
        //: 2 When sufficient capacity is indicated, the method taking
        //:   'bufferLength' generates a null terminator.
        //:
        //: 3 Each method returns the expected value (the correct character
        //:   count or the supplied 'ostream', depending on the return type).
        //:
        //: 4 The value of the supplied object is unchanged.
        //:
        //: 5 The configuration that is in effect, whether user-supplied or the
        //:   process-wide default, has the desired effect on the output.
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   'TimeInterval' values (one per row) and their corresponding ISO
        //:   8601 string representations.
        //:
        //: 2 For each row 'R' in the table from P-1:  (C-1..5)
        //:
        //:   1 Create a 'const' 'TimeInterval' object, 'X', from 'R'.
        //:
        //:   2 Invoke the six methods under test on 'X' for all possible
        //:     configurations.  Also exercise the method taking 'bufferLength'
        //:     for all buffer lengths in the range '[0 .. L]', where 'L'
        //:     provides sufficient capacity for a null terminator and a few
        //:     extra characters.  For each call, verify that the generated
        //:     output matches the string from 'R' (taking the effect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, int, const TimeInterval&);
        //   int generate(char *, int, const TimeInterval&, const Config&);
        //   int generate(string *, const TimeInterval&);
        //   int generate(string *, const TimeInterval&, const Config&);
        //   ostream generate(ostream&, const TimeInterval&);
        //   ostream generate(ostream&, const TimeInterval&, const Config&);
        //   int generateRaw(char *, const TimeInterval&);
        //   int generateRaw(char *, const TimeInterval&, const Config&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "GENERATE 'TimeInterval'" << endl
                          << "=======================" << endl;

        typedef bsls::TimeInterval TYPE;

        const int OBJLEN = Util::k_TIMEINTERVAL_STRLEN;
        const int BUFLEN = OBJLEN + 4;

        char buffer[BUFLEN];
        char chaste[BUFLEN];  bsl::memset(chaste, '?', BUFLEN);

        const int                  NUM_INTERVAL_DATA =
                                                     NUM_DEFAULT_INTERVAL_DATA;
        const DefaultIntervalDataRow (&INTERVAL_DATA)[NUM_INTERVAL_DATA] =
                                                         DEFAULT_INTERVAL_DATA;

        const int                  NUM_CNFG_DATA =       NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&CNFG_DATA)[NUM_CNFG_DATA] =
                                                             DEFAULT_CNFG_DATA;

        for (int ti = 0; ti < NUM_INTERVAL_DATA; ++ti) {
            if (!INTERVAL_DATA[ti].d_canonical) {
                continue;
            }
            const int                 ILINE   = INTERVAL_DATA[ti].d_line;
            const bsls::Types::Int64  SEC     = INTERVAL_DATA[ti].d_sec;
            const int                 USEC    = INTERVAL_DATA[ti].d_usec;
            const char               *ISO8601 = INTERVAL_DATA[ti].d_iso8601;

            const TYPE        X(SEC, USEC);
            const bsl::string BASE_EXPECTED(ISO8601);

            if (veryVerbose) { T_ P_(ILINE) P_(X) P(BASE_EXPECTED) }

            for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                const int  CLINE     = CNFG_DATA[tc].d_line;
                const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                const int  PRECISION = CNFG_DATA[tc].d_precision;
                const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                const bool USEZ      = CNFG_DATA[tc].d_useZ;

                if (veryVerbose) {
                    T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                }

                Config mC;  const Config& C = mC;
                gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                Config::setDefaultConfiguration(C);

                bsl::string EXPECTED(BASE_EXPECTED);
                updateExpectedPerConfig(&EXPECTED,
                                        C,
                                        k_INTERVAL_MAX_PRECISION);

                const int OUTLEN = static_cast<int>(EXPECTED.length());

                // 'generate' taking 'bufferLength'

                for (int k = 0; k < BUFLEN; ++k) {
                    bsl::memset(buffer, '?', BUFLEN);

                    if (veryVeryVerbose) {
                        T_ T_ cout << "Length: "; P(k)
                    }

                    ASSERTV(ILINE, k, OUTLEN,
                            OUTLEN == Util::generate(buffer, k, X));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(EXPECTED.c_str(),
                                             buffer,
                                             k < OUTLEN ? k : OUTLEN));

                    if (k <= OUTLEN) {
                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + k,
                                                 BUFLEN - k));
                    }
                    else {
                        ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + k + 1,
                                                 BUFLEN - k - 1));
                    }
                }

                // 'generate' to a 'bsl::string'
                {
                    bsl::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN, OUTLEN == Util::generate(&mS, X));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }

                // 'generate' to an 'std::string'
                {
                    std::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN, OUTLEN == Util::generate(&mS, X));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                // 'generate' to an 'std::pmr::string'
                {
                    std::pmr::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN, OUTLEN == Util::generate(&mS, X));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }
#endif

                // 'generate' to an 'ostream'
                {
                    bsl::ostringstream os;

                    ASSERTV(ILINE, &os == &Util::generate(os, X));

                    ASSERTV(ILINE, EXPECTED, os.str(), EXPECTED == os.str());

                    if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                }

                // 'generateRaw'
                {
                    bsl::memset(buffer, '?', BUFLEN);

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generateRaw(buffer, X));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(EXPECTED.c_str(),
                                             buffer,
                                             OUTLEN));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(chaste,
                                             buffer + OUTLEN,
                                             BUFLEN - OUTLEN));
                }
            }  // loop over 'CNFG_DATA'

            for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                const int  CLINE     = CNFG_DATA[tc].d_line;
                const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                const int  PRECISION = CNFG_DATA[tc].d_precision;
                const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                const bool USEZ      = CNFG_DATA[tc].d_useZ;

                if (veryVerbose) {
                    T_ P_(CLINE) P_(OMITCOLON) P_(PRECISION)
                                                           P_(USECOMMA) P(USEZ)
                }

                Config mC;  const Config& C = mC;
                gg(&mC, PRECISION, OMITCOLON, USECOMMA, USEZ);

                // Set the default configuration to the complement of 'C'.

                Config mDFLT;  const Config& DFLT = mDFLT;
                gg(&mDFLT, 9 - PRECISION, !OMITCOLON, !USECOMMA, !USEZ);
                Config::setDefaultConfiguration(DFLT);

                bsl::string EXPECTED(BASE_EXPECTED);
                updateExpectedPerConfig(&EXPECTED,
                                        C,
                                        k_TIME_MAX_PRECISION);

                const int OUTLEN = static_cast<int>(EXPECTED.length());

                // 'generate' taking 'bufferLength'

                for (int k = 0; k < BUFLEN; ++k) {
                    bsl::memset(buffer, '?', BUFLEN);

                    if (veryVeryVerbose) {
                        T_ T_ cout << "Length: "; P(k)
                    }

                    ASSERTV(ILINE, k, OUTLEN,
                            OUTLEN == Util::generate(buffer, k, X, C));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(EXPECTED.c_str(),
                                             buffer,
                                             k < OUTLEN ? k : OUTLEN));

                    if (k <= OUTLEN) {
                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + k,
                                                 BUFLEN - k));
                    }
                    else {
                        ASSERTV(ILINE, k, OUTLEN, '\0' == buffer[OUTLEN]);

                        ASSERTV(ILINE, EXPECTED, buffer,
                                0 == bsl::memcmp(chaste,
                                                 buffer + k + 1,
                                                 BUFLEN - k - 1));
                    }
                }

                // 'generate' to a 'bsl::string'
                {
                    bsl::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generate(&mS, X, C));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }

                // 'generate' to an 'std::string'
                {
                    std::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generate(&mS, X, C));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                // 'generate' to an 'std::pmr::string'
                {
                    std::pmr::string mS("qwerty");

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generate(&mS, X, C));

                    ASSERTV(ILINE, EXPECTED, mS, EXPECTED == mS);

                    if (veryVerbose) { P_(EXPECTED) P(mS); }
                }
#endif

                // 'generate' to an 'ostream'
                {
                    bsl::ostringstream os;

                    ASSERTV(ILINE, &os == &Util::generate(os, X, C));

                    ASSERTV(ILINE, EXPECTED, os.str(), EXPECTED == os.str());

                    if (veryVerbose) { P_(EXPECTED) P(os.str()); }
                }

                // 'generateRaw'
                {
                    bsl::memset(buffer, '?', BUFLEN);

                    ASSERTV(ILINE, OUTLEN,
                            OUTLEN == Util::generateRaw(buffer, X, C));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(EXPECTED.c_str(),
                                             buffer,
                                             OUTLEN));

                    ASSERTV(ILINE, EXPECTED, buffer,
                            0 == bsl::memcmp(chaste,
                                             buffer + OUTLEN,
                                             BUFLEN - OUTLEN));
                }
            }  // loop over 'CNFG_DATA'
        }  // loop over 'TIME_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN], *pc = 0;

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X));
                ASSERT_SAFE_FAIL(Util::generate(    pc, OBJLEN, X));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X));
                ASSERT_SAFE_FAIL(Util::generate(buffer,     -1, X));

                ASSERT_SAFE_PASS(Util::generate(buffer, OBJLEN, X, C));
                ASSERT_FAIL(Util::generate(     pc, OBJLEN, X, C));

                ASSERT_SAFE_PASS(Util::generate(buffer,      0, X, C));
                ASSERT_FAIL(Util::generate(buffer,     -1, X, C));

                bsl::string mB("qwerty"), *pb = 0;

                ASSERT_PASS(Util::generate(&mB, X));
                ASSERT_FAIL(Util::generate( pb, X));

                ASSERT_PASS(Util::generate(&mB, X, C));
                ASSERT_FAIL(Util::generate( pb, X, C));

                std::string mS("qwerty"), *ps = 0;

                ASSERT_PASS(Util::generate(&mS, X));
                ASSERT_FAIL(Util::generate( ps, X));

                ASSERT_PASS(Util::generate(&mS, X, C));
                ASSERT_FAIL(Util::generate( ps, X, C));

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
                std::pmr::string mP("qwerty"), *pp = 0;

                ASSERT_PASS(Util::generate(&mP, X));
                ASSERT_FAIL(Util::generate( pp, X));

                ASSERT_PASS(Util::generate(&mP, X, C));
                ASSERT_FAIL(Util::generate( pp, X, C));
#endif
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN], *pc = 0;

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(    pc, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_FAIL(Util::generateRaw(    pc, X, C));
            }
        }

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
// Copyright 2016 Bloomberg Finance L.P.
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
