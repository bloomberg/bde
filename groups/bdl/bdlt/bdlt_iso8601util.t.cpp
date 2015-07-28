// bdlt_iso8601util.t.cpp                                             -*-C++-*-
#include <bdlt_iso8601util.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bdls_testutil.h>

#include <bsls_asserttest.h>

#include <bsl_cctype.h>      // 'isdigit'
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
#include <bdlt_delegatingdateimputil.h>
#include <bsls_log.h>
#endif

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
//                         ==================
//                         struct Iso8601Util
//                         ==================
// CLASS METHODS
// [ 2] int generate(char *, const Date&, int);
// [ 2] int generate(char *, const Date&, int, const Config&);
// [ 3] int generate(char *, const Time&, int);
// [ 3] int generate(char *, const Time&, int, const Config&);
// [ 4] int generate(char *, const Datetime&, int);
// [ 4] int generate(char *, const Datetime&, int, const Config&);
// [ 5] int generate(char *, const DateTz&, int);
// [ 5] int generate(char *, const DateTz&, int, const Config&);
// [ 6] int generate(char *, const TimeTz&, int);
// [ 6] int generate(char *, const TimeTz&, int, const Config&);
// [ 7] int generate(char *, const DatetimeTz&, int);
// [ 7] int generate(char *, const DatetimeTz&, int, const Config&);
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
// [ 8] int parse(Date *, const char *, int);
// [ 9] int parse(Time *, const char *, int);
// [10] int parse(Datetime *, const char *, int);
// [ 8] int parse(DateTz *, const char *, int);
// [ 9] int parse(TimeTz *, const char *, int);
// [10] int parse(DatetimeTz *, const char *, int);
//
//                         ==============================
//                         class Iso8601UtilConfiguration
//                         ==============================
// CLASS METHODS
// [ 1] static Config defaultConfiguration();
// [ 1] static void setDefaultConfiguration(const Config& config);
//
// CREATORS
// [ 1] Iso8601UtilConfiguration();
// [ 1] Iso8601UtilConfiguration(const Config& original);
// [ 1] ~Iso8601UtilConfiguration();
//
// MANIPULATORS
// [ 1] Config& operator=(const Config& rhs);
// [ 1] void setOmitColonInZoneDesignator(bool value);
// [ 1] void setUseCommaForDecimalSign(bool value);
// [ 1] void setUseZAbbreviationForUtc(bool value);
//
// ACCESSORS
// [ 1] bool omitColonInZoneDesignator() const;
// [ 1] bool useCommaForDecimalSign() const;
// [ 1] bool useZAbbreviationForUtc() const;
//
// FREE OPERATORS
// [ 1] bool operator==(const Config& lhs, const Config& rhs);
// [ 1] bool operator!=(const Config& lhs, const Config& rhs);
//-----------------------------------------------------------------------------
// [11] USAGE EXAMPLE
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

#define ASSERT       BDLS_TESTUTIL_ASSERT
#define ASSERTV      BDLS_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BDLS_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BDLS_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BDLS_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BDLS_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BDLS_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BDLS_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BDLS_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BDLS_TESTUTIL_LOOP6_ASSERT

#define Q            BDLS_TESTUTIL_Q   // Quote identifier literally.
#define P            BDLS_TESTUTIL_P   // Print identifier and value.
#define P_           BDLS_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BDLS_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BDLS_TESTUTIL_L_  // current Line number

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

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

// Define DEFAULT DATA generally usable across 'generate' and 'parse' test
// cases.

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
    const char *d_iso8601;  // ISO 8601 string
};

static
const DefaultTimeDataRow DEFAULT_TIME_DATA[] =
{
    //LINE   HOUR   MIN   SEC   MSEC      ISO8601
    //----   ----   ---   ---   ----   --------------
    { L_,       0,    0,    0,     0,  "00:00:00.000" },
    { L_,       1,    2,    3,     4,  "01:02:03.004" },
    { L_,      10,   20,   30,    40,  "10:20:30.040" },
    { L_,      19,   43,   27,   805,  "19:43:27.805" },
    { L_,      23,   59,   59,   999,  "23:59:59.999" },
    { L_,      24,    0,    0,     0,  "24:00:00.000" },
};
const int NUM_DEFAULT_TIME_DATA =
        static_cast<int>(sizeof DEFAULT_TIME_DATA / sizeof *DEFAULT_TIME_DATA);

// *** Zone Data ***

struct DefaultZoneDataRow {
    int         d_line;     // source line number
    int         d_offset;   // offset (in minutes) from GMT
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
    bool d_useComma;   // 'useCommaForDecimalSign'        "
    bool d_useZ;       // 'useZAbbreviationForUtc'        "
};

static
const DefaultCnfgDataRow DEFAULT_CNFG_DATA[] =
{
    //LINE   omit ':'   use ','   use 'Z'
    //----   --------   -------   -------
    { L_,      false,    false,    false  },
    { L_,      false,    false,     true  },
    { L_,      false,     true,    false  },
    { L_,      false,     true,     true  },
    { L_,       true,    false,    false  },
    { L_,       true,    false,     true  },
    { L_,       true,     true,    false  },
    { L_,       true,     true,     true  },
};
const int NUM_DEFAULT_CNFG_DATA =
        static_cast<int>(sizeof DEFAULT_CNFG_DATA / sizeof *DEFAULT_CNFG_DATA);

// Define BAD (invalid) DATA generally usable across 'parse' test cases.

// *** 'Date' Data ***

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
};
const int NUM_BAD_DATE_DATA =
                static_cast<int>(sizeof BAD_DATE_DATA / sizeof *BAD_DATE_DATA);

// *** 'Time' Data ***

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
};
const int NUM_BAD_TIME_DATA =
                static_cast<int>(sizeof BAD_TIME_DATA / sizeof *BAD_TIME_DATA);

// *** Zone Data ***

struct BadZoneDataRow {
    int         d_line;     // source line number
    const char *d_invalid;  // test string
};

static
const BadZoneDataRow BAD_ZONE_DATA[] =
{
    //LINE  INPUT STRING
    //----  -------------------------
    { L_,   "0"                      },
    { L_,   "+"                      },
    { L_,   "-"                      },
    { L_,   "T"                      },
    { L_,   "z"                      },

    { L_,   "+0"                     },
    { L_,   "-0"                     },
    { L_,   "Z0"                     },

    { L_,   "+01"                    },
    { L_,   "-01"                    },

    { L_,   "+10:"                   },
    { L_,   "-10:"                   },
    { L_,   "+120"                   },
    { L_,   "-030"                   },

    { L_,   "+01:1"                  },
    { L_,   "-01:1"                  },
    { L_,   "+1:12"                  },
    { L_,   "+12:1"                  },
    { L_,   "+2360"                  },
    { L_,   "-2360"                  },
    { L_,   "+2400"                  },
    { L_,   "-2400"                  },

    { L_,   "+12:1x"                 },
    { L_,   "+12:1 "                 },
    { L_,   "+1200x"                 },

    { L_,   "+23:60"                 },
    { L_,   "-23:60"                 },
    { L_,   "+24:00"                 },
    { L_,   "-24:00"                 },

    { L_,   "+123:23"                },
    { L_,   "+12:123"                },
    { L_,   "+011:23"                },
    { L_,   "+12:011"                },

    { L_,   "+123:123"               },
};
const int NUM_BAD_ZONE_DATA =
                static_cast<int>(sizeof BAD_ZONE_DATA / sizeof *BAD_ZONE_DATA);

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static
Config& gg(Config *object,
           bool    omitColonInZoneDesignatorFlag,
           bool    useCommaForDecimalSignFlag,
           bool    useZAbbreviationForUtcFlag)
    // Return, by reference, the specified '*object' with its value adjusted
    // according to the specified 'omitColonInZoneDesignatorFlag',
    // 'useCommaForDecimalSignFlag', and 'useZAbbreviationForUtcFlag'.
{
    object->setOmitColonInZoneDesignator(omitColonInZoneDesignatorFlag);
    object->setUseCommaForDecimalSign(useCommaForDecimalSignFlag);
    object->setUseZAbbreviationForUtc(useZAbbreviationForUtcFlag);

    return *object;
}

static
void updateExpectedPerConfig(bsl::string   *expected,
                             const Config&  configuration)
    // Update the specified 'expected' ISO 8601 string as if it were generated
    // using the specified 'configuration'.  The behavior is undefined unless
    // the zone designator within 'expected' (if any) is of the form
    // "(+|-)dd:dd".
{
    ASSERT(expected);

    if (configuration.useCommaForDecimalSign()) {
        const bsl::string::size_type index = expected->find('.');

        if (index != bsl::string::npos) {
            (*expected)[index] = ',';
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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
void noopLogMessageHandler(const char *, int, const char *)
    // Do nothing.
{
}
#endif

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED
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
#endif

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
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
    const bdlt::Time time(8, 59, 59, 123);  // 08::59::59.123
    const int        tzOffset = 240;        // +04:00 (four hours west of GMT)
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
//  31JAN2005_08:59:59.123+0400
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
// GMT.
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
// indicates how we would like to affect the generated output ISO 8601 string.
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
// return value against 'BUFLEN - 2' we account for the omission of the ':'
// from the zone designator, and also for the fact that, although a null
// terminator was generated, it is not included in the character count returned
// by 'generate'.  Also note that we use 'bsl::strcmp' to compare the resulting
// string knowing that we supplied a buffer having sufficient capacity to
// accommodate a null terminator:
//..
    rc = bdlt::Iso8601Util::generate(buffer,
                                     sourceTimeTz,
                                     BUFLEN,
                                     configuration);
    ASSERT(BUFLEN - 2 == rc);
    ASSERT(         0 == bsl::strcmp(buffer, "08:59:59,123+0400"));
//..
// For comparison, see the output that was produced by the streaming operator
// above.
//
// Next, we parse the string that was just produced, loading the result of the
// parse into a second 'bdlt::TimeTz'object , and assert that the parse was
// successful and that the target object has the same value as that of the
// original (i.e., 'sourceTimeTz').  Note that 'BUFLEN - 2' is passed and *not*
// 'BUFLEN' because the former indicates the correct number of characters in
// 'buffer' that we wish to parse:
//..
    bdlt::TimeTz targetTimeTz;

    rc = bdlt::Iso8601Util::parse(&targetTimeTz, buffer, BUFLEN - 2);

    ASSERT(           0 == rc);
    ASSERT(sourceTimeTz == targetTimeTz);
//..
// Finally, we parse the string in 'buffer' a second time, this time loading
// the result into a 'bdlt::Time' object (instead of a 'bdlt::TimeTz'):
//..
    bdlt::Time targetTime;

    rc = bdlt::Iso8601Util::parse(&targetTime, buffer, BUFLEN - 2);
    ASSERT(                     0 == rc);
    ASSERT(sourceTimeTz.utcTime() == targetTime);
//..
// Note that this time the value of the target object has been converted to
// GMT.

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // PARSE: DATETIME & DATETIMETZ
        //
        // Concerns:
        //: 1 'parse' accepts an ISO 8601 string having the full range of
        //:   dates.
        //:
        //: 2 'parse' accepts an ISO 8601 string having the full range of
        //:   "normal" times.
        //:
        //: 3 'parse' translates fractional seconds to milliseconds.
        //:
        //: 4 'parse' rounds fractional seconds beyond milliseconds to the
        //:    nearest millisecond.
        //:
        //: 5 'parse' accepts zone designators in the range '(-1440 .. 1440)'.
        //:
        //: 6 'parse' loads a 'bdlt::DatetimeTz' with a 'Tz' having the parsed
        //:   timezone offset.
        //:
        //: 7 'parse' loads a 'bdlt::Datetime' by converting a time with a
        //:   zone designator to its corresponding UTC time.
        //:
        //: 8 'parse' does not accept the hour 24 if minutes, seconds, or
        //:    fractional seconds is non-zero.
        //:
        //: 9 'parse' converts a seconds value of 60 (leap-second) to the first
        //:   second of the subsequent minute.
        //:
        //:10 'parse' only parses up to the supplied input length.
        //:
        //:11 'parse' returns a non-zero value if the input is not a valid ISO
        //:    8601 string.
        //:
        //:12 'parse' returns a non-zero value if the input is not in the valid
        //:   range of representable 'Datetime' or 'DatetimeTz' values.
        //:
        //:13 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For a table of valid ISO 8601 strings, call 'parse' and compare
        //:   the resulting 'bdlt::Datetime' and 'bdlt::DatetimeTz' values
        //:   against their expected value.  (C-1..4, 9).
        //:
        //: 2 For a table of valid ISO 8601 strings having zone designators,
        //:   call 'parse' and compare the resulting 'bdlt::Datetime' and
        //:   'bdlt::DatetimeTz' values against their expected value.
        //:
        //: 3 For a table of invalid ISO 8601 strings or ISO 8601 strings
        //:   outside the representable range of values, call 'parse' (for both
        //:   'Datetime' and 'DatetimeTz') and a non-zero status is returned.
        //:   (C-11)
        //:
        //: 4 For a table of ISO 8601 strings that are *within* the
        //:   representable range of 'bdlt::DatetimeTz' *but* *outside* the
        //:   representable range of 'bdlt::Datetime', call 'parse', and verify
        //:   the returned 'bdlt::DatetimeTz' has the expected value, and the
        //:   overload taking a 'bdlt::Datetime' returns a non-zero status.
        //:   (C-5..7, 12)
        //:
        //: 5 For a valid ISO 8601 string, 'INPUT', iterate over the possible
        //:   string lengths to provide to 'parse' (i.e., 0 through
        //:   'strlen(INPUT)'), and verify the returned 'parse' status against
        //:   the expected return status for that length.
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to 'parse' if provided an
        //:   invalid result object, input string, and string length
        //:   (using the 'BSLS_ASSERTTEST_*' macros).  (C-13)
        //
        // Testing:
        //   int parse(Datetime *, const char *, int);
        //   int parse(DatetimeTz *, const char *, int);
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

                const bdlt::Time TIME(HOUR, MIN, SEC, MSEC);

                for (int tk = 0; tk < NUM_ZONE_DATA; ++tk) {
                    const int KLINE  = ZONE_DATA[tk].d_line;
                    const int OFFSET = ZONE_DATA[tk].d_offset;

                    if (TIME == bdlt::Time() && OFFSET != 0) {
                        continue;  // skip invalid compositions
                    }

                    const bdlt::Datetime   DATETIME(DATE, TIME);
                    const bdlt::DatetimeTz DATETIMETZ(DATETIME, OFFSET);

                    if (veryVerbose) {
                        T_ P_(ILINE) P_(JLINE) P_(KLINE)
                        P_(DATETIME) P(DATETIMETZ)
                    }

                    for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                        const int  CLINE     = CNFG_DATA[tc].d_line;
                        const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                        const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                        const bool USEZ      = CNFG_DATA[tc].d_useZ;

                        if (veryVerbose) {
                            T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                        }

                        Config mC;  const Config& C = mC;
                        gg(&mC, OMITCOLON, USECOMMA, USEZ);

                        // without zone designator in parsed string
                        {
                            const int LENGTH = Util::generateRaw(buffer,
                                                                 DATETIME, C);

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
                        }
                    }  // loop over 'CNFG_DATA'
                }  // loop over 'ZONE_DATA'
            }  // loop over 'TIME_DATA'
        }  // loop over 'DATE_DATA'

// TBD RETAIN - special cases of interest
        if (verbose) cout << "\nTesting valid datetime values." << endl;
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
                { L_, "0001-01-01T00:00:00.000"  ,
                                               0001, 01, 01, 00, 00, 00, 000 },
                { L_, "9999-12-31T23:59:59.999"  ,
                                               9999, 12, 31, 23, 59, 59, 999 },

                // Test random dates
                { L_, "1234-02-23T12:34:45.123"  ,
                                               1234, 02, 23, 12, 34, 45, 123 },
                { L_, "2014-12-15T17:03:56.243"  ,
                                               2014, 12, 15, 17, 03, 56, 243 },

                // Test fractional millisecond rounding
                { L_, "0001-01-01T00:00:00.00001",
                                               0001, 01, 01, 00, 00, 00, 000 },
                { L_, "0001-01-01T00:00:00.00049",
                                               0001, 01, 01, 00, 00, 00, 000 },
                { L_, "0001-01-01T00:00:00.00050",
                                               0001, 01, 01, 00, 00, 00, 001 },
                { L_, "0001-01-01T00:00:00.00099",
                                               0001, 01, 01, 00, 00, 00, 001 },

                // Test fractional millisecond rounding to 1000
                { L_, "0001-01-01T00:00:00.9994" ,
                                               0001, 01, 01, 00, 00, 00, 999 },
                { L_, "0001-01-01T00:00:00.9995" ,
                                               0001, 01, 01, 00, 00, 01, 000 },

                // Test without fractional seconds
                { L_, "1234-02-23T12:34:45"      ,
                                               1234, 02, 23, 12, 34, 45, 000 },
                { L_, "2014-12-15T17:03:56"      ,
                                               2014, 12, 15, 17, 03, 56, 000 },

                // Test leap-seconds
                { L_, "0001-01-01T00:00:60.000"  ,
                                               0001, 01, 01, 00, 01, 00, 000 },
                { L_, "9998-12-31T23:59:60.999"  ,
                                               9999, 01, 01, 00, 00, 00, 999 },

                // Test special case 24:00:00 (midnight) values
                { L_, "0001-01-01T24:00:00.000"  ,
                                               0001, 01, 01, 24, 00, 00, 000 },
                { L_, "2001-01-01T24:00:00.000"  ,
                                               2001, 01, 01, 24, 00, 00, 000 },
                { L_, "0001-01-01T24:00:00"      ,
                                               0001, 01, 01, 24, 00, 00, 000 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *INPUT = DATA[i].d_input;

                if (veryVerbose) { T_ P_(LINE) P(INPUT) }

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

                ASSERTV(LINE,
                        0 == Util::parse(&result,
                                         INPUT,
                                         static_cast<int>(strlen(INPUT))));
                ASSERTV(LINE, EXPECTED, result, EXPECTED == result);

                ASSERTV(LINE,
                        0 == Util::parse(&resultTz,
                                        INPUT,
                                        static_cast<int>(strlen(INPUT))));
                ASSERTV(LINE, EXPECTEDTZ, resultTz, EXPECTEDTZ == resultTz);
            }
        }

// TBD RETAIN - extremal values
        if (verbose) cout
            << "\nTesting timezone offsets that cannot be converted to UTC"
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
                { L_, "0001-01-01T00:00:00.000+00:01",
                                         0001, 01, 01, 00, 00, 00, 000,    1 },
                { L_, "0001-01-01T23:58:59.000+23:59",
                                         0001, 01, 01, 23, 58, 59, 000, 1439 },
                { L_, "9999-12-31T23:59:59.999-00:01",
                                         9999, 12, 31, 23, 59, 59, 999,   -1 },
                { L_, "9999-12-31T00:01:00.000-23:59",
                                         9999, 12, 31, 00, 01, 00, 000,-1439 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *INPUT = DATA[i].d_input;

                if (veryVerbose) { T_ P_(i) P(INPUT) }

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
                        0 == Util::parse(&resultTz,
                                         INPUT,
                                         static_cast<int>(strlen(INPUT))));

                ASSERTV(LINE, INPUT, EXPECTED, resultTz,
                        EXPECTED == resultTz);

                ASSERTV(LINE, INPUT, EXPECTED,
                        0 != Util::parse(&result,
                                         INPUT,
                                         static_cast<int>(strlen(INPUT))));
            }
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
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const char *INPUT  = "2013-10-23T01:23:45";
            const int   LENGTH = static_cast<int>(bsl::strlen(INPUT));

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
        // PARSE: TIME & TIMETZ
        //
        // Concerns:
        //: 1 TBD
        //
        // Plan:
        //: 1 TBD
        //
        // Testing:
        //   int parse(Time *, const char *, int);
        //   int parse(TimeTz *, const char *, int);
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

            const bdlt::Time TIME(HOUR, MIN, SEC, MSEC);

            for (int tj = 0; tj < NUM_ZONE_DATA; ++tj) {
                const int JLINE  = ZONE_DATA[tj].d_line;
                const int OFFSET = ZONE_DATA[tj].d_offset;

                if (TIME == bdlt::Time() && OFFSET != 0) {
                    continue;  // skip invalid compositions
                }

                const bdlt::TimeTz TIMETZ(TIME, OFFSET);

                if (veryVerbose) { T_ P_(ILINE) P_(JLINE) P_(TIME) P(TIMETZ) }

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, OMITCOLON, USECOMMA, USEZ);

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
                        ASSERTV(ILINE, JLINE, CLINE, TIME == X);

                        ASSERTV(ILINE, JLINE, CLINE,
                                0 == Util::parse(&mZ, buffer, LENGTH));
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
                    }
                }  // loop over 'CNFG_DATA'
            }  // loop over 'ZONE_DATA'
        }  // loop over 'TIME_DATA'

        // TBD special cases (24:00, leap seconds, fractional seconds)

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
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const char *INPUT  = "01:23:45";
            const int   LENGTH = static_cast<int>(bsl::strlen(INPUT));

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
                ASSERT_PASS(Util::parse(  &result, INPUT,      0));
                ASSERT_FAIL(Util::parse(  &result, INPUT,     -1));

                ASSERT_PASS(Util::parse(&resultTz, INPUT, LENGTH));
                ASSERT_PASS(Util::parse(&resultTz, INPUT,      0));
                ASSERT_FAIL(Util::parse(&resultTz, INPUT,     -1));
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // PARSE: DATE & DATETZ
        //
        // Concerns:
        //: 1 TBD
        //
        // Plan:
        //: 1 TBD
        //
        // Testing:
        //   int parse(Date *, const char *, int);
        //   int parse(DateTz *, const char *, int);
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
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, OMITCOLON, USECOMMA, USEZ);

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
                    }
                }  // loop over 'CNFG_DATA'
            }  // loop over 'ZONE_DATA'
        }  // loop over 'DATE_DATA'

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
                }

                bad.append(ZONE_DATA[ti].d_invalid);

                const char *STRING = bad.c_str();
                const int   LENGTH = static_cast<int>(bad.length());

                if (veryVerbose) { T_ P_(LINE) P(STRING) }

                ASSERTV(LINE, STRING,  0 != Util::parse(&mX, STRING, LENGTH));
                ASSERTV(LINE, STRING, XX == X);

                ASSERTV(LINE, STRING,  0 != Util::parse(&mZ, STRING, LENGTH));
                ASSERTV(LINE, STRING, ZZ == Z);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const char *INPUT  = "2013-10-23";
            const int   LENGTH = static_cast<int>(bsl::strlen(INPUT));

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
        //:   process-wide default, has the desired affect on the output.
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
        //:     output matches the string from 'R' (taking the affect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, const DatetimeTz&, int);
        //   int generate(char *, const DatetimeTz&, int, const Config&);
        //   ostream generate(ostream&, const DatetimeTz&);
        //   ostream generate(ostream&, const DatetimeTz&, const Config&);
        //   int generateRaw(char *, const DatetimeTz&);
        //   int generateRaw(char *, const DatetimeTz&, const Config&);
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

                    const TYPE        X(bdlt::Datetime(DATE, TIME), OFFSET);
                    const bsl::string BASE_EXPECTED(
                          EXPECTED_DATE + 'T' + EXPECTED_TIME + EXPECTED_ZONE);

                    if (veryVerbose) {
                        T_ P_(ILINE) P_(JLINE) P_(KLINE) P_(X) P(BASE_EXPECTED)
                    }

                    for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                        const int  CLINE     = CNFG_DATA[tc].d_line;
                        const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                        const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                        const bool USEZ      = CNFG_DATA[tc].d_useZ;

                        if (veryVerbose) {
                            T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                        }

                        Config mC;  const Config& C = mC;
                        gg(&mC, OMITCOLON, USECOMMA, USEZ);

                        Config::setDefaultConfiguration(C);

                        bsl::string EXPECTED(BASE_EXPECTED);
                        updateExpectedPerConfig(&EXPECTED, C);

                        const int OUTLEN = static_cast<int>(EXPECTED.length());

                        // 'generate' taking 'bufferLength'

                        for (int k = 0; k < BUFLEN; ++k) {
                            bsl::memset(buffer, '?', BUFLEN);

                            if (veryVeryVerbose) {
                                T_ T_ cout << "Length: "; P(k)
                            }

                            ASSERTV(ILINE, JLINE, KLINE, k, OUTLEN,
                                    OUTLEN == Util::generate(buffer, X, k));

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
                        }

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

                    Config::setDefaultConfiguration(Config());

                    for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                        const int  CLINE     = CNFG_DATA[tc].d_line;
                        const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                        const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                        const bool USEZ      = CNFG_DATA[tc].d_useZ;

                        if (veryVerbose) {
                            T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                        }

                        Config mC;  const Config& C = mC;
                        gg(&mC, OMITCOLON, USECOMMA, USEZ);

                        bsl::string EXPECTED(BASE_EXPECTED);
                        updateExpectedPerConfig(&EXPECTED, C);

                        const int OUTLEN = static_cast<int>(EXPECTED.length());

                        // 'generate' taking 'bufferLength'

                        for (int k = 0; k < BUFLEN; ++k) {
                            bsl::memset(buffer, '?', BUFLEN);

                            if (veryVeryVerbose) {
                                T_ T_ cout << "Length: "; P(k)
                            }

                            ASSERTV(ILINE, k, OUTLEN,
                                    OUTLEN == Util::generate(buffer, X, k, C));

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
                }  // loop over 'ZONE_DATA'
            }  // loop over 'TIME_DATA'
        }  // loop over 'DATE_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generate(buffer, X, OBJLEN));
                ASSERT_SAFE_FAIL(Util::generate(     0, X, OBJLEN));

                ASSERT_SAFE_PASS(Util::generate(buffer, X,      0));
                ASSERT_SAFE_FAIL(Util::generate(buffer, X,     -1));

                ASSERT_SAFE_PASS(Util::generate(buffer, X, OBJLEN, C));
                ASSERT_SAFE_FAIL(Util::generate(     0, X, OBJLEN, C));

                ASSERT_SAFE_PASS(Util::generate(buffer, X,      0, C));
                ASSERT_SAFE_FAIL(Util::generate(buffer, X,     -1, C));
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X, C));
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
        //:   process-wide default, has the desired affect on the output.
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
        //:     output matches the string from 'R' (taking the affect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, const TimeTz&, int);
        //   int generate(char *, const TimeTz&, int, const Config&);
        //   ostream generate(ostream&, const TimeTz&);
        //   ostream generate(ostream&, const TimeTz&, const Config&);
        //   int generateRaw(char *, const TimeTz&);
        //   int generateRaw(char *, const TimeTz&, const Config&);
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
            const char *ISO8601 = TIME_DATA[ti].d_iso8601;

            const bdlt::Time  TIME(HOUR, MIN, SEC, MSEC);
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
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, OMITCOLON, USECOMMA, USEZ);

                    Config::setDefaultConfiguration(C);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED, C);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, JLINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, X, k));

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

                Config::setDefaultConfiguration(Config());

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, OMITCOLON, USECOMMA, USEZ);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED, C);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, X, k, C));

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
            }  // loop over 'ZONE_DATA'
        }  // loop over 'TIME_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generate(buffer, X, OBJLEN));
                ASSERT_SAFE_FAIL(Util::generate(     0, X, OBJLEN));

                ASSERT_SAFE_PASS(Util::generate(buffer, X,      0));
                ASSERT_SAFE_FAIL(Util::generate(buffer, X,     -1));

                ASSERT_SAFE_PASS(Util::generate(buffer, X, OBJLEN, C));
                ASSERT_SAFE_FAIL(Util::generate(     0, X, OBJLEN, C));

                ASSERT_SAFE_PASS(Util::generate(buffer, X,      0, C));
                ASSERT_SAFE_FAIL(Util::generate(buffer, X,     -1, C));
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X, C));
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
        //:   process-wide default, has the desired affect on the output.
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
        //:     output matches the string from 'R' (taking the affect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, const DateTz&, int);
        //   int generate(char *, const DateTz&, int, const Config&);
        //   ostream generate(ostream&, const DateTz&);
        //   ostream generate(ostream&, const DateTz&, const Config&);
        //   int generateRaw(char *, const DateTz&);
        //   int generateRaw(char *, const DateTz&, const Config&);
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
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, OMITCOLON, USECOMMA, USEZ);

                    Config::setDefaultConfiguration(C);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED, C);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, JLINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, X, k));

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

                Config::setDefaultConfiguration(Config());

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, OMITCOLON, USECOMMA, USEZ);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED, C);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, X, k, C));

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
            }  // loop over 'ZONE_DATA'
        }  // loop over 'DATE_DATA'

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generate(buffer, X, OBJLEN));
                ASSERT_SAFE_FAIL(Util::generate(     0, X, OBJLEN));

                ASSERT_SAFE_PASS(Util::generate(buffer, X,      0));
                ASSERT_SAFE_FAIL(Util::generate(buffer, X,     -1));

                ASSERT_SAFE_PASS(Util::generate(buffer, X, OBJLEN, C));
                ASSERT_SAFE_FAIL(Util::generate(     0, X, OBJLEN, C));

                ASSERT_SAFE_PASS(Util::generate(buffer, X,      0, C));
                ASSERT_SAFE_FAIL(Util::generate(buffer, X,     -1, C));
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X, C));
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
        //:   process-wide default, has the desired affect on the output.
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
        //:     output matches the string from 'R' (taking the affect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, const Datetime&, int);
        //   int generate(char *, const Datetime&, int, const Config&);
        //   ostream generate(ostream&, const Datetime&);
        //   ostream generate(ostream&, const Datetime&, const Config&);
        //   int generateRaw(char *, const Datetime&);
        //   int generateRaw(char *, const Datetime&, const Config&);
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
                const char *ISO8601 = TIME_DATA[tj].d_iso8601;

                const bdlt::Time  TIME(HOUR, MIN, SEC, MSEC);
                const bsl::string EXPECTED_TIME(ISO8601);

                const TYPE        X(DATE, TIME);
                const bsl::string BASE_EXPECTED(
                                          EXPECTED_DATE + 'T' + EXPECTED_TIME);

                if (veryVerbose) {
                    T_ P_(ILINE) P_(JLINE) P_(X) P(BASE_EXPECTED)
                }

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, OMITCOLON, USECOMMA, USEZ);

                    Config::setDefaultConfiguration(C);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED, C);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, JLINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, X, k));

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

                Config::setDefaultConfiguration(Config());

                for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                    const int  CLINE     = CNFG_DATA[tc].d_line;
                    const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                    const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                    const bool USEZ      = CNFG_DATA[tc].d_useZ;

                    if (veryVerbose) {
                        T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                    }

                    Config mC;  const Config& C = mC;
                    gg(&mC, OMITCOLON, USECOMMA, USEZ);

                    bsl::string EXPECTED(BASE_EXPECTED);
                    updateExpectedPerConfig(&EXPECTED, C);

                    const int OUTLEN = static_cast<int>(EXPECTED.length());

                    // 'generate' taking 'bufferLength'

                    for (int k = 0; k < BUFLEN; ++k) {
                        bsl::memset(buffer, '?', BUFLEN);

                        if (veryVeryVerbose) {
                            T_ T_ cout << "Length: "; P(k)
                        }

                        ASSERTV(ILINE, k, OUTLEN,
                                OUTLEN == Util::generate(buffer, X, k, C));

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
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generate(buffer, X, OBJLEN));
                ASSERT_SAFE_FAIL(Util::generate(     0, X, OBJLEN));

                ASSERT_SAFE_PASS(Util::generate(buffer, X,      0));
                ASSERT_SAFE_FAIL(Util::generate(buffer, X,     -1));

                ASSERT_SAFE_PASS(Util::generate(buffer, X, OBJLEN, C));
                ASSERT_SAFE_FAIL(Util::generate(     0, X, OBJLEN, C));

                ASSERT_SAFE_PASS(Util::generate(buffer, X,      0, C));
                ASSERT_SAFE_FAIL(Util::generate(buffer, X,     -1, C));
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X, C));
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
        //:   process-wide default, has the desired affect on the output.
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
        //:     output matches the string from 'R' (taking the affect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, const Time&, int);
        //   int generate(char *, const Time&, int, const Config&);
        //   ostream generate(ostream&, const Time&);
        //   ostream generate(ostream&, const Time&, const Config&);
        //   int generateRaw(char *, const Time&);
        //   int generateRaw(char *, const Time&, const Config&);
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
            const char *ISO8601 = TIME_DATA[ti].d_iso8601;

            const TYPE        X(HOUR, MIN, SEC, MSEC);
            const bsl::string BASE_EXPECTED(ISO8601);

            if (veryVerbose) { T_ P_(ILINE) P_(X) P(BASE_EXPECTED) }

            for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                const int  CLINE     = CNFG_DATA[tc].d_line;
                const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                const bool USEZ      = CNFG_DATA[tc].d_useZ;

                if (veryVerbose) {
                    T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                }

                Config mC;  const Config& C = mC;
                gg(&mC, OMITCOLON, USECOMMA, USEZ);

                Config::setDefaultConfiguration(C);

                bsl::string EXPECTED(BASE_EXPECTED);
                updateExpectedPerConfig(&EXPECTED, C);

                const int OUTLEN = static_cast<int>(EXPECTED.length());

                // 'generate' taking 'bufferLength'

                for (int k = 0; k < BUFLEN; ++k) {
                    bsl::memset(buffer, '?', BUFLEN);

                    if (veryVeryVerbose) {
                        T_ T_ cout << "Length: "; P(k)
                    }

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

            Config::setDefaultConfiguration(Config());

            for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                const int  CLINE     = CNFG_DATA[tc].d_line;
                const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                const bool USEZ      = CNFG_DATA[tc].d_useZ;

                if (veryVerbose) {
                    T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                }

                Config mC;  const Config& C = mC;
                gg(&mC, OMITCOLON, USECOMMA, USEZ);

                bsl::string EXPECTED(BASE_EXPECTED);
                updateExpectedPerConfig(&EXPECTED, C);

                const int OUTLEN = static_cast<int>(EXPECTED.length());

                // 'generate' taking 'bufferLength'

                for (int k = 0; k < BUFLEN; ++k) {
                    bsl::memset(buffer, '?', BUFLEN);

                    if (veryVeryVerbose) {
                        T_ T_ cout << "Length: "; P(k)
                    }

                    ASSERTV(ILINE, k, OUTLEN,
                            OUTLEN == Util::generate(buffer, X, k, C));

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
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generate(buffer, X, OBJLEN));
                ASSERT_SAFE_FAIL(Util::generate(     0, X, OBJLEN));

                ASSERT_SAFE_PASS(Util::generate(buffer, X,      0));
                ASSERT_SAFE_FAIL(Util::generate(buffer, X,     -1));

                ASSERT_SAFE_PASS(Util::generate(buffer, X, OBJLEN, C));
                ASSERT_SAFE_FAIL(Util::generate(     0, X, OBJLEN, C));

                ASSERT_SAFE_PASS(Util::generate(buffer, X,      0, C));
                ASSERT_SAFE_FAIL(Util::generate(buffer, X,     -1, C));
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X, C));
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
        //:   process-wide default, has the desired affect on the output.
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
        //:     output matches the string from 'R' (taking the affect of the
        //:     configuration into account), a null terminator is appended when
        //:     expected, and the return value is correct.  (C-1..5)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid arguments, but not triggered for adjacent
        //:   valid ones (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   int generate(char *, const Date&, int);
        //   int generate(char *, const Date&, int, const Config&);
        //   ostream generate(ostream&, const Date&);
        //   ostream generate(ostream&, const Date&, const Config&);
        //   int generateRaw(char *, const Date&);
        //   int generateRaw(char *, const Date&, const Config&);
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
                const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                const bool USEZ      = CNFG_DATA[tc].d_useZ;

                if (veryVerbose) {
                    T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                }

                Config mC;  const Config& C = mC;
                gg(&mC, OMITCOLON, USECOMMA, USEZ);

                Config::setDefaultConfiguration(C);

                bsl::string EXPECTED(BASE_EXPECTED);
                updateExpectedPerConfig(&EXPECTED, C);

                const int OUTLEN = static_cast<int>(EXPECTED.length());

                // 'generate' taking 'bufferLength'

                for (int k = 0; k < BUFLEN; ++k) {
                    bsl::memset(buffer, '?', BUFLEN);

                    if (veryVeryVerbose) {
                        T_ T_ cout << "Length: "; P(k)
                    }

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

            Config::setDefaultConfiguration(Config());

            for (int tc = 0; tc < NUM_CNFG_DATA; ++tc) {
                const int  CLINE     = CNFG_DATA[tc].d_line;
                const bool OMITCOLON = CNFG_DATA[tc].d_omitColon;
                const bool USECOMMA  = CNFG_DATA[tc].d_useComma;
                const bool USEZ      = CNFG_DATA[tc].d_useZ;

                if (veryVerbose) {
                    T_ P_(CLINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                }

                Config mC;  const Config& C = mC;
                gg(&mC, OMITCOLON, USECOMMA, USEZ);

                bsl::string EXPECTED(BASE_EXPECTED);
                updateExpectedPerConfig(&EXPECTED, C);

                const int OUTLEN = static_cast<int>(EXPECTED.length());

                // 'generate' taking 'bufferLength'

                for (int k = 0; k < BUFLEN; ++k) {
                    bsl::memset(buffer, '?', BUFLEN);

                    if (veryVeryVerbose) {
                        T_ T_ cout << "Length: "; P(k)
                    }

                    ASSERTV(ILINE, k, OUTLEN,
                            OUTLEN == Util::generate(buffer, X, k, C));

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
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            const Config C;

            if (verbose) cout << "\t'generate'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generate(buffer, X, OBJLEN));
                ASSERT_SAFE_FAIL(Util::generate(     0, X, OBJLEN));

                ASSERT_SAFE_PASS(Util::generate(buffer, X,      0));
                ASSERT_SAFE_FAIL(Util::generate(buffer, X,     -1));

                ASSERT_SAFE_PASS(Util::generate(buffer, X, OBJLEN, C));
                ASSERT_SAFE_FAIL(Util::generate(     0, X, OBJLEN, C));

                ASSERT_SAFE_PASS(Util::generate(buffer, X,      0, C));
                ASSERT_SAFE_FAIL(Util::generate(buffer, X,     -1, C));
            }

            if (verbose) cout << "\t'generateRaw'" << endl;
            {
                const TYPE X;
                char       buffer[OBJLEN];

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X));

                ASSERT_SAFE_PASS(Util::generateRaw(buffer, X, C));
                ASSERT_SAFE_FAIL(Util::generateRaw(     0, X, C));
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'Iso8601UtilConfiguration'
        //
        // Concerns:
        //: 1 Our concerns include those that apply to unconstrained attribute
        //:   types, with the simplification that the class under test lacks a
        //:   'print' method and free 'operator<<'.
        //:
        //: 2 The two class methods that manage the process-wide configuration
        //:   work correctly.
        //
        // Plan:
        //: 1 Follow the standard regimen for testing an unconstrained
        //:   attribute type.  (C-1)
        //:
        //: 2 Exercise the class methods on all possible configurations and
        //:   verify that the behavior is as expected.  (C-2)
        //
        // Testing:
        //   static Config defaultConfiguration();
        //   static void setDefaultConfiguration(const Config& config);
        //   Iso8601UtilConfiguration();
        //   Iso8601UtilConfiguration(const Config& original);
        //   ~Iso8601UtilConfiguration();
        //   Config& operator=(const Config& rhs);
        //   void setOmitColonInZoneDesignator(bool value);
        //   void setUseCommaForDecimalSign(bool value);
        //   void setUseZAbbreviationForUtc(bool value);
        //   bool omitColonInZoneDesignator() const;
        //   bool useCommaForDecimalSign() const;
        //   bool useZAbbreviationForUtc() const;
        //   bool operator==(const Config& lhs, const Config& rhs);
        //   bool operator!=(const Config& lhs, const Config& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'Iso8601UtilConfiguration'" << endl
                          << "==================================" << endl;

        const int                  NUM_DATA        = NUM_DEFAULT_CNFG_DATA;
        const DefaultCnfgDataRow (&DATA)[NUM_DATA] = DEFAULT_CNFG_DATA;

        // Testing:
        //   Iso8601UtilConfiguration();
        //   ~Iso8601UtilConfiguration();
        {
            const Config X;

            ASSERT(!X.omitColonInZoneDesignator());
            ASSERT(!X.useCommaForDecimalSign());
            ASSERT(!X.useZAbbreviationForUtc());
        }

        // Testing:
        //   void setOmitColonInZoneDesignator(bool value);
        //   void setUseCommaForDecimalSign(bool value);
        //   void setUseZAbbreviationForUtc(bool value);
        //   bool omitColonInZoneDesignator() const;
        //   bool useCommaForDecimalSign() const;
        //   bool useZAbbreviationForUtc() const;
        {
            {
                Config mX;  const Config& X = mX;

                mX.setOmitColonInZoneDesignator(true);

                ASSERT( X.omitColonInZoneDesignator());
                ASSERT(!X.useCommaForDecimalSign());
                ASSERT(!X.useZAbbreviationForUtc());
            }

            {
                Config mX;  const Config& X = mX;

                mX.setUseCommaForDecimalSign(true);

                ASSERT(!X.omitColonInZoneDesignator());
                ASSERT( X.useCommaForDecimalSign());
                ASSERT(!X.useZAbbreviationForUtc());
            }

            {
                Config mX;  const Config& X = mX;

                mX.setUseZAbbreviationForUtc(true);

                ASSERT(!X.omitColonInZoneDesignator());
                ASSERT(!X.useCommaForDecimalSign());
                ASSERT( X.useZAbbreviationForUtc());
            }

            Config mX;  const Config& X = mX;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE      = DATA[ti].d_line;
                const bool OMITCOLON = DATA[ti].d_omitColon;
                const bool USECOMMA  = DATA[ti].d_useComma;
                const bool USEZ      = DATA[ti].d_useZ;

                if (veryVerbose) {
                    T_ P_(LINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                }

                mX.setOmitColonInZoneDesignator(OMITCOLON);
                mX.setUseCommaForDecimalSign(USECOMMA);
                mX.setUseZAbbreviationForUtc(USEZ);

                ASSERTV(LINE, OMITCOLON == X.omitColonInZoneDesignator());
                ASSERTV(LINE, USECOMMA  == X.useCommaForDecimalSign());
                ASSERTV(LINE, USEZ      == X.useZAbbreviationForUtc());
            }
        }

        // Testing:
        //   bool operator==(const Config& lhs, const Config& rhs);
        //   bool operator!=(const Config& lhs, const Config& rhs);
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  ILINE      = DATA[ti].d_line;
                const bool IOMITCOLON = DATA[ti].d_omitColon;
                const bool IUSECOMMA  = DATA[ti].d_useComma;
                const bool IUSEZ      = DATA[ti].d_useZ;

                if (veryVerbose) {
                    T_ P_(ILINE) P_(IOMITCOLON) P_(IUSECOMMA) P(IUSEZ)
                }

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int  JLINE      = DATA[tj].d_line;
                    const bool JOMITCOLON = DATA[tj].d_omitColon;
                    const bool JUSECOMMA  = DATA[tj].d_useComma;
                    const bool JUSEZ      = DATA[tj].d_useZ;

                    if (veryVerbose) {
                        T_ P_(JLINE) P_(JOMITCOLON) P_(JUSECOMMA) P(JUSEZ)
                    }

                    Config mX;  const Config& X = mX;
                    gg(&mX, IOMITCOLON, IUSECOMMA, IUSEZ);

                    Config mY;  const Config& Y = mY;
                    gg(&mY, JOMITCOLON, JUSECOMMA, JUSEZ);

                    const bool EXP = ti == tj;  // expected for '==' comparison

                    ASSERTV(ILINE, JLINE,  EXP == (X == Y));
                    ASSERTV(ILINE, JLINE,  EXP == (Y == X));

                    ASSERTV(ILINE, JLINE, !EXP == (X != Y));
                    ASSERTV(ILINE, JLINE, !EXP == (Y != X));
                }
            }
        }

        // Testing:
        //   Iso8601UtilConfiguration(const Config& original);
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE      = DATA[ti].d_line;
                const bool OMITCOLON = DATA[ti].d_omitColon;
                const bool USECOMMA  = DATA[ti].d_useComma;
                const bool USEZ      = DATA[ti].d_useZ;

                if (veryVerbose) {
                    T_ P_(LINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                }

                Config mX;  const Config& X = mX;
                gg(&mX, OMITCOLON, USECOMMA, USEZ);

                const Config Y(X);

                ASSERTV(LINE, X == Y);
            }
        }

        // Testing:
        //   Config& operator=(const Config& rhs);
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  ILINE      = DATA[ti].d_line;
                const bool IOMITCOLON = DATA[ti].d_omitColon;
                const bool IUSECOMMA  = DATA[ti].d_useComma;
                const bool IUSEZ      = DATA[ti].d_useZ;

                if (veryVerbose) {
                    T_ P_(ILINE) P_(IOMITCOLON) P_(IUSECOMMA) P(IUSEZ)
                }

                Config mZ;  const Config& Z = mZ;
                gg(&mZ, IOMITCOLON, IUSECOMMA, IUSEZ);

                for (int tj = 0; tj < NUM_DATA; ++tj) {
                    const int  JLINE      = DATA[tj].d_line;
                    const bool JOMITCOLON = DATA[tj].d_omitColon;
                    const bool JUSECOMMA  = DATA[tj].d_useComma;
                    const bool JUSEZ      = DATA[tj].d_useZ;

                    if (veryVerbose) {
                        T_ P_(JLINE) P_(JOMITCOLON) P_(JUSECOMMA) P(JUSEZ)
                    }

                    Config mX;  const Config& X = mX;
                    gg(&mX, JOMITCOLON, JUSECOMMA, JUSEZ);

                    ASSERTV(ILINE, JLINE, (Z == X) == (ILINE == JLINE));

                    Config *mR = &(mX = Z);

                    ASSERTV(ILINE, JLINE,  Z == X);
                    ASSERTV(ILINE, JLINE, mR == &mX);
                }

                // self-assignment

                {
                    Config mX;
                    gg(&mX, IOMITCOLON, IUSECOMMA, IUSEZ);

                    Config mZZ;  const Config& ZZ = mZZ;
                    gg(&mZZ, IOMITCOLON, IUSECOMMA, IUSEZ);

                    const Config& Z = mX;

                    ASSERTV(ILINE, ZZ == Z);

                    Config *mR = &(mX = Z);

                    ASSERTV(ILINE, mR == &mX);
                    ASSERTV(ILINE, ZZ == Z);
                }
            }
        }

        // Testing:
        //   static void setDefaultConfiguration(const Config& config);
        //   static Config defaultConfiguration();
        {
            ASSERT(Config() == Config::defaultConfiguration());

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int  LINE      = DATA[ti].d_line;
                const bool OMITCOLON = DATA[ti].d_omitColon;
                const bool USECOMMA  = DATA[ti].d_useComma;
                const bool USEZ      = DATA[ti].d_useZ;

                if (veryVerbose) {
                    T_ P_(LINE) P_(OMITCOLON) P_(USECOMMA) P(USEZ)
                }

                Config mX;  const Config& X = mX;
                gg(&mX, OMITCOLON, USECOMMA, USEZ);

                {
                    Config mY;  const Config& Y = mY;
                    gg(&mY, OMITCOLON, USECOMMA, USEZ);

                    Config::setDefaultConfiguration(Y);
                }

                ASSERTV(LINE, X == Config::defaultConfiguration());
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
