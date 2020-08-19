// baljsn_printutil.t.cpp                                             -*-C++-*-
#include <baljsn_printutil.h>

#include <bdldfp_decimal.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>
#include <bdlt_datetz.h>
#include <bdlt_time.h>
#include <bdlt_timetz.h>

#include <bslim_testutil.h>
#include <bsls_platform.h>

#include <bsl_climits.h>
#include <bsl_cmath.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

#ifdef BSLS_PLATFORM_OS_WINDOWS
#   define copysign  _copysign
#   define copysignf _copysignf
#endif

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test implements a utility for printing 'bdeat'
// compatible simple types onto an 'bsl::ostream'.  The printing is done via
// overloaded 'printValue' functions that are overloaded for fundamental types
// and 'bdet' types.  Since the functions are independent and do not share any
// state we will test them independently.
//
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] static int printValue(bsl::ostream& s, bool                      v);
// [ 4] static int printValue(bsl::ostream& s, char                      v);
// [ 4] static int printValue(bsl::ostream& s, signed char               v);
// [ 4] static int printValue(bsl::ostream& s, unsigned char             v);
// [ 4] static int printValue(bsl::ostream& s, short                     v);
// [ 4] static int printValue(bsl::ostream& s, unsigned short            v);
// [ 4] static int printValue(bsl::ostream& s, int                       v);
// [ 4] static int printValue(bsl::ostream& s, unsigned int              v);
// [ 4] static int printValue(bsl::ostream& s, bsls::Types::Int64        v);
// [ 4] static int printValue(bsl::ostream& s, bsls::Types::Uint64       v);
// [ 4] static int printValue(bsl::ostream& s, float                     v);
// [ 4] static int printValue(bsl::ostream& s, double                    v);
// [ 3] static int printValue(bsl::ostream& s, const char               *v);
// [ 3] static int printValue(bsl::ostream& s, const bsl::string&        v);
// [ 5] static int printValue(bsl::ostream& s, const bdlt::Time&         v);
// [ 5] static int printValue(bsl::ostream& s, const bdlt::Date&         v);
// [ 5] static int printValue(bsl::ostream& s, const bdlt::Datetime&     v);
// [ 5] static int printValue(bsl::ostream& s, const bdlt::TimeTz&       v);
// [ 5] static int printValue(bsl::ostream& s, const bdlt::DateTz&       v);
// [ 5] static int printValue(bsl::ostream& s, const bdlt::DatetimeTz&   v);
// [ 5] static int printValue(bsl::ostream& s, const bdldfp::Decimal64&  v);
// [ 6] static int printValue(bsl::ostream& s, const bdlt::DatetimeInterval v);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 8] USAGE EXAMPLE

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
//                   MACROS FOR TESTING WORKAROUNDS
// ----------------------------------------------------------------------------

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
    // 'snprintf' on older Windows libraries outputs an additional '0' in the
    // exponent for scientific notation.
# define BALJSN_PRINTUTIL_EXTRA_ZERO_PADDING_FOR_EXPONENTS 1
#endif

// ============================================================================
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef baljsn::PrintUtil    Obj;
typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

template <class TYPE>
void testNumber()
{
    const struct {
        int         d_line;
        Int64       d_value;
        const char *d_result;
    } DATA[] = {
        //LINE     VALUE  RESULT
        //----     -----  ------
        { L_,         -1, "-1" },
        { L_,          0, "0" },
        { L_,          1, "1" },
        { L_,   SHRT_MIN, "-32768" },
        { L_,   SHRT_MAX, "32767" },
        { L_,  USHRT_MAX, "65535" },
        { L_,    INT_MIN, "-2147483648" },
        { L_,    INT_MAX, "2147483647" },
        { L_,   UINT_MAX, "4294967295" },
        { L_,  LLONG_MIN, "-9223372036854775808" },
        { L_,  LLONG_MAX, "9223372036854775807" }
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int  LINE  = DATA[ti].d_line;
        const TYPE VALUE = (TYPE) DATA[ti].d_value;
        const char *const EXP = DATA[ti].d_result;

        const double testValue = static_cast<double>(DATA[ti].d_value);
        if (testValue > bsl::numeric_limits<TYPE>::max()
         || testValue < bsl::numeric_limits<TYPE>::min()) {
            continue;
        }

        bsl::ostringstream oss;
        ASSERTV(LINE, 0 == Obj::printValue(oss, VALUE));

        bsl::string result = oss.str();
        ASSERTV(LINE, result, EXP, result == EXP);
    }
}

template <class TYPE>
void testInfAndNaNAsStrings()
{
    const char *POS_INF     = "\"+inf\"";
    const char *NEG_INF     = "\"-inf\"";
    const char *POS_NAN_STR = "\"nan\"";
    const char *NEG_NAN_STR = "\"-nan\"";

    const struct {
        int         d_line;
        TYPE        d_value;
        bool        d_specifyOptions;
        bool        d_einasOptionFlag;
        int         d_retCode;
        const char *d_result;
    } DATA[] = {

 //LINE VALUE                                      OPT    EINAS   RC   RESULT
 //---- -----                                      ---    -----   --   ------

 { L_, bsl::numeric_limits<TYPE>::infinity(),      false, false, -1,  ""     },
 { L_, -bsl::numeric_limits<TYPE>::infinity(),     false, false, -1,  ""     },
 { L_, bsl::numeric_limits<TYPE>::quiet_NaN(),     false, false, -1,  ""     },
 { L_, -bsl::numeric_limits<TYPE>::quiet_NaN(),    false, false, -1,  ""     },
 { L_, bsl::numeric_limits<TYPE>::signaling_NaN(), false, false, -1,  ""     },
 { L_, -bsl::numeric_limits<TYPE>::signaling_NaN(),false, false, -1,  ""     },

 { L_, bsl::numeric_limits<TYPE>::infinity(),      true,  false, -1,  ""     },
 { L_, -bsl::numeric_limits<TYPE>::infinity(),     true,  false, -1,  ""     },
 { L_, bsl::numeric_limits<TYPE>::quiet_NaN(),     true,  false, -1,  ""     },
 { L_, -bsl::numeric_limits<TYPE>::quiet_NaN(),    true,  false, -1,  ""     },
 { L_, bsl::numeric_limits<TYPE>::signaling_NaN(), true,  false, -1,  ""     },
 { L_, -bsl::numeric_limits<TYPE>::signaling_NaN(),true,  false, -1,  ""     },

 { L_, bsl::numeric_limits<TYPE>::infinity(),      true, true, 0, POS_INF    },
 { L_, -bsl::numeric_limits<TYPE>::infinity(),     true, true, 0, NEG_INF    },
 { L_, bsl::numeric_limits<TYPE>::quiet_NaN(),     true, true, 0, POS_NAN_STR},
 { L_,-bsl::numeric_limits<TYPE>::quiet_NaN(),     true, true, 0, NEG_NAN_STR},
 { L_, bsl::numeric_limits<TYPE>::signaling_NaN(), true, true, 0, POS_NAN_STR},
 { L_,-bsl::numeric_limits<TYPE>::signaling_NaN(), true, true, 0, NEG_NAN_STR},
    };
    const int NUM_DATA = sizeof DATA / sizeof *DATA;

    for (int ti = 0; ti < NUM_DATA; ++ti) {
        const int         LINE   = DATA[ti].d_line;
        const TYPE        VALUE  = DATA[ti].d_value;
        const bool        OPT    = DATA[ti].d_specifyOptions;
        const bool        EINAS  = DATA[ti].d_einasOptionFlag;
        const int         EXP_RC = DATA[ti].d_retCode;
        const char *const EXP    = DATA[ti].d_result;

        baljsn::EncoderOptions options;
        options.setEncodeInfAndNaNAsStrings(EINAS);

        bsl::ostringstream oss;

        const int RC = OPT ? Obj::printValue(oss, VALUE, &options)
                           : Obj::printValue(oss, VALUE);

        ASSERTV(LINE, RC, EXP_RC, RC == EXP_RC);

        bsl::string result = oss.str();
        ASSERTV(LINE, result, EXP, result == EXP);
    }
}

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    bool verbose             = argc > 2;
    bool veryVerbose         = argc > 3;
    bool veryVeryVerbose     = argc > 4;

    (void)veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 8: {
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
///Example 1: Encoding a Simple 'struct' into JSON
///-----------------------------------------------
// Suppose we want to serialize some data into JSON.
//
// First, we define a struct, 'Employee', to contain the data:
//..
    struct Employee {
        const char *d_firstName;
        const char *d_lastName;
        int         d_age;
    };
//..
// Then, we create an 'Employee' object and populate it with data:
//..
    Employee john;
    john.d_firstName = "John";
    john.d_lastName = "Doe";
    john.d_age = 20;
//..
//  Now, we create an output stream and manually construct the JSON string
//  using 'baljsn::PrintUtil':
//..
    bsl::ostringstream oss;
    oss << '{' << '\n';
    baljsn::PrintUtil::printValue(oss, "firstName");
    oss << ':';
    baljsn::PrintUtil::printValue(oss, john.d_firstName);
    oss << ',' << '\n';
    baljsn::PrintUtil::printValue(oss, "lastName");
    oss << ':';
    baljsn::PrintUtil::printValue(oss, john.d_lastName);
    oss << ',' << '\n';
    baljsn::PrintUtil::printValue(oss, "age");
    oss << ':';
    baljsn::PrintUtil::printValue(oss, john.d_age);
    oss << '\n' << '}';
//..
//  Finally, we print out the JSON string:
//..
    if (verbose) {
        bsl::cout << oss.str();
    }
//..
//  The output should look like:
//..
//  {
//  "firstName":"John",
//  "lastName":"Doe",
//  "age":20
//  }
//..
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // ENCODING 'INF' AND 'NaN' FLOATING POINT VALUES
        //
        // Concerns:
        //: 1 INF and NaN floating point values can be encoded as strings by
        //:   setting the 'encodeInfAndNaNAsStrings' encoder option to 'true'.
        //:
        //: 2 Encoding INF and NaN floating point values as strings by
        //:   setting the 'encodeInfAndNaNAsStrings' encoder option to 'true'
        //:   will result in the correct encoding.
        //:
        //: 3 Encoding INF and NaN floating point values results in an error
        //:   if 'encodeInfAndNaNAsStrings' encoder option is either not
        //:   specified or set to 'false'.
        //
        // Plan:
        //: 1 Use the table-driven technique:
        //:
        //:   1 Specify a set of valid values, whether encoder options should
        //:     be used, the value for the 'encodeInfAndNaNAsStrings' option,
        //:     the expected return code and the expected output.
        //:
        //:   2 Encode these values for 'float', 'double', and
        //:     'bdldfp::Decimal64'.
        //:
        //:   3 Verify the output is as expected.
        //
        // Testing:
        //  static int printValue(bsl::ostream& s, float v, options);
        //  static int printValue(bsl::ostream& s, double v, options);
        //  static int printValue(bsl::ostream& s, Decimal64 v, options);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ENCODING 'INF' AND 'NaN' FLOATING POINT VALUES"
                          << endl
                          << "=============================================="
                          << endl;

        testInfAndNaNAsStrings<float>();
        testInfAndNaNAsStrings<double>();
        testInfAndNaNAsStrings<bdldfp::Decimal64>();

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // ENCODING DATETIMEINTERVAL TYPE
        //  Ensure that DatetimeInterval values are correctly encoded.
        //
        // Concerns:
        //: 1 DatetimeInterval is encoded in the 'bdlt::DatetimeInterval' print
        //:   format.
        //:
        // Plan:
        //: 1 Use the table-driven technique:
        //:
        //:   1 Specify a set of valid values.
        //:
        //:   2 Encode each value and verify the output is as expected.
        //:
        //
        // Testing:
        //   static int printValue(bsl::ostream& s,
        //                         const bdlt::DatetimeInterval& v);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ENCODING DATETIMEINTERVAL TYPE" << endl
                          << "==============================" << endl;

        static const struct {
            int         d_line;           // source line number
            int         d_day;
            int         d_hour;
            int         d_minute;
            int         d_second;
            int         d_millisecond;
            int         d_microsecond;
            const char *d_expected_p;
        } DATA[] = {
    //LINE D   H   M   S   MS   US   EXPECTED
    //---- --  --  --  --  ---  ---  --------
    { L_,   0,  0,  0,  0,   0,   0, "+0_00:00:00.000000"},
    { L_,   1, 23, 59, 58, 765, 432, "+1_23:59:58.765432"},
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         DAY    = DATA[ti].d_day;
                const int         HOUR   = DATA[ti].d_hour;
                const int         MINUTE = DATA[ti].d_minute;
                const int         SECOND = DATA[ti].d_second;
                const int         MSEC   = DATA[ti].d_millisecond;
                const int         USEC   = DATA[ti].d_microsecond;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_ P_(DAY) P_(HOUR) P_(MINUTE) P_(SECOND) P_(MSEC) P(USEC);
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                bdlt::DatetimeInterval x(DAY,
                                         HOUR,
                                         MINUTE,
                                         SECOND,
                                         MSEC,
                                         USEC);
                const bdlt::DatetimeInterval& X = x;

                bsl::ostringstream oss;

                oss << X;

                bsl::string result = oss.str();

                if (veryVeryVerbose) { P(result) }

                LOOP3_ASSERT(LINE, EXP, result, EXP == result);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // ENCODING DATE AND TIME TYPES
        //
        // Concerns:
        //: 1 Date/time are encoded in ISO 8601 format.
        //:
        //: 2 Output contains only information contained in the type being
        //:   encoded.  (i.e., encoding 'bdlt::Date' will not print out a time
        //:   or offset.)
        //
        // Plan:
        //: 1 Use the table-driven technique:
        //:
        //:   1 Specify a set of valid values.
        //:
        //:   2 Encode each value and verify the output is as expected.
        //:
        //: 2 Perform step one for every date/time types.
        //
        // Testing:
        //   static int printValue(bsl::ostream& s, const bdlt::Time&       v);
        //   static int printValue(bsl::ostream& s, const bdlt::Date&       v);
        //   static int printValue(bsl::ostream& s, const bdlt::Datetime&   v);
        //   static int printValue(bsl::ostream& s, const bdlt::TimeTz&     v);
        //   static int printValue(bsl::ostream& s, const bdlt::DateTz&     v);
        //   static int printValue(bsl::ostream& s, const bdlt::DatetimeTz& v);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ENCODING DATE AND TIME TYPES" << endl
                          << "============================" << endl;

        const struct {
            int d_line;
            int d_year;
            int d_month;
            int d_day;
            int d_hour;
            int d_minute;
            int d_second;
            int d_millisecond;
            int d_microsecond;
            int d_offset;
        } DATA[] = {
            //Line Year   Mon  Day  Hour  Min  Sec     ms   us   offset
            //---- ----   ---  ---  ----  ---  ---     --   --   ------

            // Valid dates and times
            { L_,     1,   1,   1,    0,   0,   0,     0,    0,      0 },
            { L_,  2005,   1,   1,    0,   0,   0,     0,    0,    -90 },
            { L_,   123,   6,  15,   13,  40,  59,     0,    0,   -240 },
            { L_,  1999,  10,  12,   23,   0,   1,     0,    0,   -720 },

            // Vary milliseconds
            { L_,  1999,  10,  12,   23,   0,   1,     0,    0,     90 },
            { L_,  1999,  10,  12,   23,   0,   1,   456,    0,    240 },
            { L_,  1999,  10,  12,   23,   0,   1,   456,  789,    240 },
            { L_,  1999,  10,  12,   23,   0,   1,   999,  789,    720 },
            { L_,  1999,  12,  31,   23,  59,  59,   999,  999,    720 }
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const char *expectedDate[] = {
            "\"0001-01-01\"",
            "\"2005-01-01\"",
            "\"0123-06-15\"",
            "\"1999-10-12\"",
            "\"1999-10-12\"",
            "\"1999-10-12\"",
            "\"1999-10-12\"",
            "\"1999-10-12\"",
            "\"1999-12-31\""
        };

        const char *expectedDateTz[] = {
            "\"0001-01-01+00:00\"",
            "\"2005-01-01-01:30\"",
            "\"0123-06-15-04:00\"",
            "\"1999-10-12-12:00\"",
            "\"1999-10-12+01:30\"",
            "\"1999-10-12+04:00\"",
            "\"1999-10-12+04:00\"",
            "\"1999-10-12+12:00\"",
            "\"1999-12-31+12:00\""
        };

        const char *expectedTime[] = {
            "\"00:00:00.000000\"",
            "\"00:00:00.000000\"",
            "\"13:40:59.000000\"",
            "\"23:00:01.000000\"",
            "\"23:00:01.000000\"",
            "\"23:00:01.456000\"",
            "\"23:00:01.456000\"",
            "\"23:00:01.999000\"",
            "\"23:59:59.999000\""
        };

        const char *expectedTimeTz[] = {
            "\"00:00:00.000000+00:00\"",
            "\"00:00:00.000000-01:30\"",
            "\"13:40:59.000000-04:00\"",
            "\"23:00:01.000000-12:00\"",
            "\"23:00:01.000000+01:30\"",
            "\"23:00:01.456000+04:00\"",
            "\"23:00:01.456000+04:00\"",
            "\"23:00:01.999000+12:00\"",
            "\"23:59:59.999000+12:00\""
        };

        const char *expectedDatetime[] = {
            "\"0001-01-01T00:00:00.000000\"",
            "\"2005-01-01T00:00:00.000000\"",
            "\"0123-06-15T13:40:59.000000\"",
            "\"1999-10-12T23:00:01.000000\"",
            "\"1999-10-12T23:00:01.000000\"",
            "\"1999-10-12T23:00:01.456000\"",
            "\"1999-10-12T23:00:01.456789\"",
            "\"1999-10-12T23:00:01.999789\"",
            "\"1999-12-31T23:59:59.999999\""
        };

        const char *expectedDatetimeTz[] = {
            "\"0001-01-01T00:00:00.000000+00:00\"",
            "\"2005-01-01T00:00:00.000000-01:30\"",
            "\"0123-06-15T13:40:59.000000-04:00\"",
            "\"1999-10-12T23:00:01.000000-12:00\"",
            "\"1999-10-12T23:00:01.000000+01:30\"",
            "\"1999-10-12T23:00:01.456000+04:00\"",
            "\"1999-10-12T23:00:01.456789+04:00\"",
            "\"1999-10-12T23:00:01.999789+12:00\"",
            "\"1999-12-31T23:59:59.999999+12:00\""
        };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_line;
            const int YEAR        = DATA[ti].d_year;
            const int MONTH       = DATA[ti].d_month;
            const int DAY         = DATA[ti].d_day;
            const int HOUR        = DATA[ti].d_hour;
            const int MINUTE      = DATA[ti].d_minute;
            const int SECOND      = DATA[ti].d_second;
            const int MILLISECOND = DATA[ti].d_millisecond;
            const int MICROSECOND = DATA[ti].d_microsecond;
            const int OFFSET      = DATA[ti].d_offset;;

            bdlt::Date       theDate(YEAR, MONTH, DAY);
            bdlt::Time       theTime(HOUR, MINUTE, SECOND,
                                     MILLISECOND);
            bdlt::Datetime   theDatetime(YEAR, MONTH, DAY,
                                         HOUR, MINUTE, SECOND,
                                         MILLISECOND, MICROSECOND);

            bdlt::DateTz     theDateTz(theDate, OFFSET);
            bdlt::TimeTz     theTimeTz(theTime, OFFSET);
            bdlt::DatetimeTz theDatetimeTz(theDatetime, OFFSET);

            baljsn::EncoderOptions opt;
            opt.setDatetimeFractionalSecondPrecision(6);
            if (verbose) cout << "Encode Date" << endl;
            {
                const char *EXP = expectedDate[ti];
                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, theDate, &opt));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode DateTz" << endl;
            {
                const char *EXP = expectedDateTz[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, theDateTz, &opt));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode Time" << endl;
            {
                const char *EXP = expectedTime[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, theTime, &opt));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode TimeTz" << endl;
            {
                const char *EXP = expectedTimeTz[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, theTimeTz, &opt));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode Datetime" << endl;
            {
                const char *EXP = expectedDatetime[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, theDatetime, &opt));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode DatetimeTz" << endl;
            {
                const char *EXP = expectedDatetimeTz[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, theDatetimeTz, &opt));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // ENCODING NUMBERS
        //
        // Concerns:
        //: 1 Encoded numbers have the expected precisions.
        //:
        //: 2 Encoded numbers used default format.
        //:
        //: 3 Encoding 'unsigned char' prints a number instead of string.
        //
        // Plan:
        //: 1 Use the table-driven technique:
        //:
        //:   1 Specify a set of valid values, including those that will test
        //:     the precision of the output.
        //:
        //:   2 Encode each value and verify the output is as expected.
        //
        // Testing:
        //  static int printValue(bsl::ostream& s, char                    v);
        //  static int printValue(bsl::ostream& s, signed char             v);
        //  static int printValue(bsl::ostream& s, unsigned char           v);
        //  static int printValue(bsl::ostream& s, short                   v);
        //  static int printValue(bsl::ostream& s, unsigned short          v);
        //  static int printValue(bsl::ostream& s, int                     v);
        //  static int printValue(bsl::ostream& s, unsigned int            v);
        //  static int printValue(bsl::ostream& s, bsls::Types::Int64      v);
        //  static int printValue(bsl::ostream& s, bsls::Types::Uint64     v);
        //  static int printValue(bsl::ostream& s, float                   v);
        //  static int printValue(bsl::ostream& s, double                  v);
        //  static int printValue(bsl::ostream& s, bdldfp::Decimal64       v);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ENCODING NUMBERS" << endl
                          << "================" << endl;

        if (verbose) cout << "Encode float" << endl;
        {
            float m0 = copysignf(0.0f, -1.0f);

            const struct {
                int         d_line;
                float       d_value;
                const char *d_result;
            } DATA[] = {
                //LINE         VALUE    RESULT
                //----         -----    ------

                { L_,            0.0f,  "0" },
                { L_,             m0,  "-0" },
                { L_,          0.125f,  "0.125" },
                { L_,            1.0f,  "1" },
                { L_,           10.0f,  "10" },
                { L_,           -1.5f,  "-1.5" },
                { L_,         -1.5e1f,  "-15" },
#if defined(BALJSN_PRINTUTIL_EXTRA_ZERO_PADDING_FOR_EXPONENTS)
                { L_,   -1.23456e-20f,  "-1.23456e-020" },
                { L_,    1.23456e-20f,  "1.23456e-020" },
#else
                { L_,   -1.23456e-20f,  "-1.23456e-20" },
                { L_,    1.23456e-20f,  "1.23456e-20" },
#endif
                { L_,         1.0e-1f,  "0.1" },
                { L_,       0.123456f,  "0.123456" },
                { L_,    0.123456789f,  "0.123457" },
                { L_,   0.1234567891f,  "0.123457" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const float       VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_result;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, VALUE));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }

        if (verbose) cout << "Encode Decimal64" << endl;
        {
            const struct {
                int                d_line;
                bdldfp::Decimal64  d_value;
                const char        *d_result;
            } DATA[] = {
                //LINE  VALUE  RESULT
                //----  -----  ------

                { L_,   BDLDFP_DECIMAL_DD(0.0),  "0.0"  },
                { L_,   BDLDFP_DECIMAL_DD(1.13), "1.13" },
                { L_,   BDLDFP_DECIMAL_DD(-9.876543210987654e307),
                  "-9.876543210987654e+307" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int               LINE  = DATA[ti].d_line;
                const bdldfp::Decimal64 VALUE = DATA[ti].d_value;
                const char *const       EXP   = DATA[ti].d_result;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, VALUE));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }

        if (verbose) cout << "Encode invalid float" << endl;
        {
            bsl::ostringstream oss;

            oss.clear();
            ASSERTV(0 != Obj::printValue(
                                      oss,
                                      bsl::numeric_limits<float>::infinity()));

            oss.clear();
            ASSERTV(0 != Obj::printValue(
                                     oss,
                                     -bsl::numeric_limits<float>::infinity()));

            oss.clear();
            ASSERTV(0 != Obj::printValue(
                                     oss,
                                     bsl::numeric_limits<float>::quiet_NaN()));

            oss.clear();
            ASSERTV(0 != Obj::printValue(
                                    oss,
                                    -bsl::numeric_limits<float>::quiet_NaN()));

            oss.clear();
            ASSERTV(0 != Obj::printValue(
                                 oss,
                                 bsl::numeric_limits<float>::signaling_NaN()));

            oss.clear();
            ASSERTV(0 != Obj::printValue(
                                oss,
                                -bsl::numeric_limits<float>::signaling_NaN()));
        }


        if (verbose) cout << "Encode float with maxFloatPrecision option"
                          << endl;
        {
            float m0 = copysignf(0.0f, -1.0f);

            const struct {
                int         d_line;
                float       d_value;
                int         d_maxFloatPrecision;
                const char *d_result;
            } DATA[] = {
                //LINE         VALUE  PRECISION RESULT
                //----         -----  --------- ------

                { L_,            0.0, 1, "0" },
                { L_,            0.0, 2, "0" },
                { L_,             m0, 1, "-0" },
                { L_,             m0, 7, "-0" },
                { L_,            1.0, 1, "1" },
                { L_,            1.0, 3, "1" },
                { L_,           10.0, 2, "10" },
                { L_,           10.0, 3, "10" },
                { L_,           -1.5, 1, "-2" },
                { L_,           -1.5, 2, "-1.5" },
                { L_,        1.0e-1f, 1, "0.1" },
                { L_,  0.1234567891f, 1, "0.1" },
                { L_,  0.1234567891f, 4, "0.1235" },
                { L_,  0.1234567891f, 9, "0.123456791" },

#if defined(BALJSN_PRINTUTIL_EXTRA_ZERO_PADDING_FOR_EXPONENTS)
                { L_,           10.0, 1, "1e+001" },
                { L_,         -1.5e1, 1, "-2e+001" },
                { L_,-1.23456789e-20, 1, "-1e-020" },
                { L_,-1.23456789e-20, 2, "-1.2e-020" },
                { L_,-1.23456789e-20, 8, "-1.2345679e-020"  },
                { L_,-1.23456789e-20, 9, "-1.23456787e-020" },
                { L_, 1.23456789e-20, 1, "1e-020" },
                { L_, 1.23456789e-20, 9, "1.23456787e-020" },
#else
                { L_,           10.0f, 1, "1e+01" },
                { L_,         -1.5e1f, 1, "-2e+01" },
                { L_,-1.23456789e-20f, 1, "-1e-20" },
                { L_,-1.23456789e-20f, 2, "-1.2e-20" },
                { L_,-1.23456789e-20f, 8, "-1.2345679e-20"  },
                { L_,-1.23456789e-20f, 9, "-1.23456787e-20" },
                { L_, 1.23456789e-20f, 1, "1e-20" },
                { L_, 1.23456789e-20f, 9, "1.23456787e-20" },
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const float       VALUE = DATA[ti].d_value;
                const int         PREC  = DATA[ti].d_maxFloatPrecision;
                const char *const EXP   = DATA[ti].d_result;

                baljsn::EncoderOptions options;
                options.setMaxFloatPrecision(PREC);

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, VALUE, &options));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }

        if (verbose) cout << "Encode double" << endl;
        {
            double m0 = copysign(0.0, -1.0);

            const struct {
                int         d_line;
                double      d_value;
                const char *d_result;
            } DATA[] = {
                //LINE              VALUE  RESULT
                //----              -----  ------

                { L_,                0.0,  "0" },
                { L_,                 m0,  "-0" },
                { L_,              0.125,  "0.125" },
                { L_,                1.0,  "1" },
                { L_,               10.0,  "10" },
                { L_,               -1.5,  "-1.5" },
                { L_,             -1.5e1,  "-15" },
                { L_,           -9.9e100,  "-9.9e+100" },
                { L_,          -3.14e300,  "-3.14e+300" },
                { L_,           3.14e300,  "3.14e+300" },
                { L_,             1.0e-1,  "0.1" },
                { L_,          2.23e-308,  "2.23e-308" },
                { L_,   0.12345678912345,  "0.12345678912345" },
                { L_,  0.12345678901234567,  "0.123456789012346" },
                { L_, 0.123456789012345678,  "0.123456789012346" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const double      VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_result;

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, VALUE));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }

        if (verbose) cout << "Encode double with maxDoublePrecision option"
                          << endl;
        {
            double m0 = copysign(0.0, -1.0);

            const struct {
                int         d_line;
                double      d_value;
                int         d_maxDoublePrecision;
                const char *d_result;
            } DATA[] = {
      //LINE                   VALUE  PRECISION RESULT
      //----                   -----  --------- ------

      { L_,                      0.0,  1, "0" },
      { L_,                      0.0,  2, "0" },
      { L_,                       m0,  1, "-0" },
      { L_,                       m0, 17, "-0" },
      { L_,                      1.0,  1, "1" },
      { L_,                      1.0,  3, "1" },
      { L_,                     10.0,  2, "10" },
      { L_,                     10.0,  3, "10" },
      { L_,                     -1.5,  1, "-2" },
      { L_,                     -1.5,  2, "-1.5" },
      { L_,                 -9.9e100,  2, "-9.9e+100" },
      { L_,                 -9.9e100, 15, "-9.9e+100" },
      { L_,                 -9.9e100, 17, "-9.9000000000000003e+100" },
      { L_,                -3.14e300, 15, "-3.14e+300" },
      { L_,                -3.14e300, 17, "-3.1400000000000001e+300" },
      { L_,                 3.14e300,  2, "3.1e+300" },
      { L_,                 3.14e300, 17, "3.1400000000000001e+300" },
      { L_,                   1.0e-1,  1, "0.1" },
      { L_,                2.23e-308,  2, "2.2e-308" },
      { L_,                2.23e-308, 17, "2.2300000000000001e-308" },
      { L_,     0.123456789012345678,  1, "0.1" },
      { L_,     0.123456789012345678,  2, "0.12" },
      { L_,     0.123456789012345678, 15, "0.123456789012346" },
      { L_,     0.123456789012345678, 16, "0.1234567890123457" },
      { L_,     0.123456789012345678, 17, "0.12345678901234568" },

#if defined(BALJSN_PRINTUTIL_EXTRA_ZERO_PADDING_FOR_EXPONENTS)
      { L_,                     10.0,  1, "1e+001" },
      { L_,                   -1.5e1,  1, "-2e+001" },
      { L_,  -1.2345678901234567e-20,  1, "-1e-020" },
      { L_,  -1.2345678901234567e-20,  2, "-1.2e-020" },
      { L_,  -1.2345678901234567e-20, 15, "-1.23456789012346e-020"  },
      { L_,  -1.2345678901234567e-20, 16, "-1.234567890123457e-020" },
      { L_,  -1.2345678901234567e-20, 17, "-1.2345678901234567e-020" },
#else
      { L_,                     10.0,  1, "1e+01" },
      { L_,                   -1.5e1,  1, "-2e+01" },
      { L_,  -1.2345678901234567e-20,  1, "-1e-20" },
      { L_,  -1.2345678901234567e-20,  2, "-1.2e-20" },
      { L_,  -1.2345678901234567e-20, 15, "-1.23456789012346e-20"  },
      { L_,  -1.2345678901234567e-20, 16, "-1.234567890123457e-20" },
      { L_,  -1.2345678901234567e-20, 17, "-1.2345678901234567e-20" },
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const double      VALUE = DATA[ti].d_value;
                const int         PREC  = DATA[ti].d_maxDoublePrecision;
                const char *const EXP   = DATA[ti].d_result;

                baljsn::EncoderOptions options;
                options.setMaxDoublePrecision(PREC);

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, VALUE, &options));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }
        }

        if (verbose) cout << "Encode invalid double" << endl;
        {
            bsl::ostringstream oss;

            oss.clear();
            ASSERTV(0 != Obj::printValue(
                                     oss,
                                     bsl::numeric_limits<double>::infinity()));

            oss.clear();
            ASSERTV(0 != Obj::printValue(
                                    oss,
                                    -bsl::numeric_limits<double>::infinity()));

            oss.clear();
            ASSERTV(0 != Obj::printValue(
                                    oss,
                                    bsl::numeric_limits<double>::quiet_NaN()));

            oss.clear();
            ASSERTV(0 != Obj::printValue(
                                   oss,
                                   -bsl::numeric_limits<double>::quiet_NaN()));

            oss.clear();
            ASSERTV(0 != Obj::printValue(
                                oss,
                                bsl::numeric_limits<double>::signaling_NaN()));

            oss.clear();
            ASSERTV(0 != Obj::printValue(
                               oss,
                               -bsl::numeric_limits<double>::signaling_NaN()));
        }

#define DEC(X) BDLDFP_DECIMAL_DD(X)

        if (verbose) cout << "Encode Decimal64" << endl;
        {
            const struct {
                int                d_line;
                bdldfp::Decimal64  d_value;
                bool               d_quoted;
                const char        *d_result;
            } DATA[] = {
  //---------------------------------------------------------------------------
  // LINE |            VALUE          | QUOTED |         RESULT
  //---------------------------------------------------------------------------
   { L_, DEC( 0.0),                     false,    "0.0"                      },
   { L_, DEC(-0.0),                     false,   "-0.0"                      },
   { L_, DEC( 1.13),                    false,   "1.13"                      },
   { L_, DEC(-9.8765432109876548e307),  false,   "-9.876543210987655e+307"   },
   { L_, DEC(-9.87654321098765482e307), false,   "-9.876543210987655e+307"   },
   { L_, DEC( 0.0),                     true,   "\"0.0\""                    },
   { L_, DEC(-0.0),                     true,  "\"-0.0\""                    },
   { L_, DEC( 1.13),                    true,  "\"1.13\""                    },
   { L_, DEC(-9.8765432109876548e307),  true,  "\"-9.876543210987655e+307\"" },
   { L_, DEC(-9.87654321098765482e307), true,  "\"-9.876543210987655e+307\"" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int               LINE   = DATA[ti].d_line;
                const bdldfp::Decimal64 VALUE  = DATA[ti].d_value;
                const bool              QUOTED = DATA[ti].d_quoted;
                const char *const       EXP    = DATA[ti].d_result;

                baljsn::EncoderOptions opt;
                opt.setEncodeQuotedDecimal64(QUOTED);
                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, VALUE, &opt));
                bsl::string result = oss.str();
                ASSERTV(LINE, QUOTED, result, EXP, result == EXP);

                if (!QUOTED) {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 == Obj::printValue(oss, VALUE));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, QUOTED, result, EXP, result == EXP);
                }
            }
        }

        if (verbose) cout << "Encode Decimal64 Inf and NaN" << endl;
        {
            typedef bdldfp::Decimal64 Type;

            const Type NAN_P = bsl::numeric_limits<Type>::quiet_NaN();
            const Type NAN_N = -NAN_P;
            const Type INF_P = bsl::numeric_limits<Type>::infinity();
            const Type INF_N = -INF_P;

            const struct {
                int         d_line;
                Type        d_value;
                bool        d_encodeAsString;
                const char *d_expected;
                int         d_result;
            } DATA[] = {
               //-----------------------------------------------
               // LINE | VALUE | AS_STRING | EXPECTED  | RESULT
               //-----------------------------------------------
                { L_,    NAN_P,    true,     "\"nan\"",     0  },
                { L_,    NAN_N,    true,     "\"-nan\"",    0  },
                { L_,    INF_P,    true,     "\"+inf\"",    0  },
                { L_,    INF_N,    true,     "\"-inf\"",    0  },
                { L_,    NAN_P,    false,    "",           -1  },
                { L_,    NAN_N,    false,    "",           -1  },
                { L_,    INF_P,    false,    "",           -1  },
                { L_,    INF_N,    false,    "",           -1  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int          LINE   = DATA[ti].d_line;
                const Type         VALUE  = DATA[ti].d_value;
                const bool         AS_STR = DATA[ti].d_encodeAsString;
                const char *const  EXP    = DATA[ti].d_expected;
                const int          RESULT = DATA[ti].d_result;

                baljsn::EncoderOptions opt;
                opt.setEncodeInfAndNaNAsStrings(AS_STR);
                bsl::ostringstream oss;
                int result = Obj::printValue(oss, VALUE, &opt);
                ASSERTV(LINE, RESULT, result, RESULT == result);
                if (0 == result) {
                    bsl::string output = oss.str();
                    ASSERTV(LINE, AS_STR, EXP, output, EXP == output);
                }
            }
        }
#undef DEC

        if (verbose) cout << "Encode int" << endl;
        {
            testNumber<char>();
            testNumber<short>();
            testNumber<int>();
            testNumber<Int64>();
            testNumber<unsigned char>();
            testNumber<unsigned short>();
            testNumber<unsigned int>();
            testNumber<Uint64>();
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // ENCODING STRINGS
        //
        // Concerns:
        //: 1 Character are encoded as a single character string.
        //:
        //: 2 All escape characters are encoded corrected.
        //:
        //: 3 Control characters are encoded as hex.
        //:
        //: 4 Invalid UTF-8 strings are rejected.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of values that include all escaped characters and
        //:     some control characters.
        //:
        //:   2 Encode the value and verify the results.
        //:
        //: 2 Repeat for strings and Customized type.
        //
        // Testing:
        //  static int printValue(bsl::ostream& s, const char             *v);
        //  static int printValue(bsl::ostream& s, const bsl::string&      v);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ENCODING STRINGS" << endl
                          << "================" << endl;

        if (verbose) cout << "Encode string" << endl;
        {
            const struct {
                int         d_line;
                const char *d_value;
                const char *d_result;
            } DATA[] = {
                //LINE    VAL  RESULT
                //----    ---  ------
                { L_,  "",     "\"\"" },
                { L_,  " ",    "\" \"" },
                { L_,  "~",    "\"~\"" },
                { L_,  "test", "\"test\"" },
                { L_,  "A quick brown fox jump over a lazy dog!",
                               "\"A quick brown fox jump over a lazy dog!\"" },
                { L_,  "\"",   "\"\\\"\"" },
                { L_,  "\\",   "\"\\\\\"" },
                { L_,  "/",    "\"\\/\"" },
                { L_,  "\b",   "\"\\b\"" },
                { L_,  "\f",   "\"\\f\"" },
                { L_,  "\n",   "\"\\n\"" },
                { L_,  "\r",   "\"\\r\"" },
                { L_,  "\t",   "\"\\t\"" },
                { L_, "\xc2\x80", "\"\xc2\x80\""},
                { L_, "\xdf\xbf", "\"\xdf\xbf\""},
                { L_, "\xe0\xa0\x80", "\"\xe0\xa0\x80\""},
                { L_, "\xef\xbf\xbf", "\"\xef\xbf\xbf\""},
                { L_, "\xf0\x90\x80\x80", "\"\xf0\x90\x80\x80\""},
                { L_, "\xf4\x8f\xbf\xbf", "\"\xf4\x8f\xbf\xbf\""},
                { L_,  "\x01", "\"\\u0001\"" },
                { L_,  "\x02", "\"\\u0002\"" },
                { L_,  "\x03", "\"\\u0003\"" },
                { L_,  "\x04", "\"\\u0004\"" },
                { L_,  "\x05", "\"\\u0005\"" },
                { L_,  "\x06", "\"\\u0006\"" },
                { L_,  "\x07", "\"\\u0007\"" },
                // Back space
                { L_,  "\x08", "\"\\b\""     },
                // Horizontal tab
                { L_,  "\x09", "\"\\t\""     },
                // New line
                { L_,  "\x0A", "\"\\n\""     },
                { L_,  "\x0B", "\"\\u000b\"" },
                // Form feed
                { L_,  "\x0C", "\"\\f\""     },
                { L_,  "\x0D", "\"\\r\""     },
                { L_,  "\x0E", "\"\\u000e\"" },
                { L_,  "\x0F", "\"\\u000f\"" },
                { L_,  "\x10", "\"\\u0010\"" },
                { L_,  "\x11", "\"\\u0011\"" },
                { L_,  "\x12", "\"\\u0012\"" },
                { L_,  "\x13", "\"\\u0013\"" },
                { L_,  "\x14", "\"\\u0014\"" },
                { L_,  "\x15", "\"\\u0015\"" },
                { L_,  "\x16", "\"\\u0016\"" },
                { L_,  "\x17", "\"\\u0017\"" },
                { L_,  "\x18", "\"\\u0018\"" },
                { L_,  "\x19", "\"\\u0019\"" },
                { L_,  "\x1A", "\"\\u001a\"" },
                { L_,  "\x1B", "\"\\u001b\"" },
                { L_,  "\x1C", "\"\\u001c\"" },
                { L_,  "\x1D", "\"\\u001d\"" },
                { L_,  "\x1E", "\"\\u001e\"" },
                { L_,  "\x1F", "\"\\u001f\"" },
                { L_,  "\\/\b\f\n\r\t",   "\"\\\\\\/\\b\\f\\n\\r\\t\"" }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const char *const VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_result;

                if (veryVeryVerbose) cout << "Test 'char *'" << endl;
                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 == Obj::printValue(oss, VALUE));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }

                if (veryVeryVerbose) cout << "Test 'string'" << endl;
                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 == Obj::printValue(oss,
                                                       bsl::string(VALUE)));

                    bsl::string result = oss.str();
                    ASSERTV(LINE, result, EXP, result == EXP);
                }
            }
        }

        if (verbose) cout << "Encode invalid UTF-8 string" << endl;
        {
            const struct {
                int         d_line;
                const char *d_value;
            } DATA[] = {
                //LINE  VALUE
                //----  -----
                { L_, "\x80" },
                { L_, "\xc2\x00" },
                { L_, "\xc2\xff" },
                { L_, "\xc1\xbf" },
                { L_, "\xe0\x9f\xbf" },
                { L_, "\xf0\x8f\xbf\xbf" },
                { L_, "\xf4\x9f\xbf\xbf" }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const char *const VALUE = DATA[ti].d_value;

                if (veryVeryVerbose) cout << "Test 'char *'" << endl;
                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 != Obj::printValue(oss, VALUE));
                }

                if (veryVeryVerbose) cout << "Test 'string'" << endl;
                {
                    bsl::ostringstream oss;
                    ASSERTV(LINE, 0 != Obj::printValue(oss,
                                                       bsl::string(VALUE)));
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // ENCODING BOOLEAN
        //
        // Concerns:
        //: 1 'true' is encoded into "true" and 'false' is encoded into
        //:   "false".
        //
        // Plan:
        //: 1 Use a brute force approach to test both cases.
        //
        // Testing:
        //   static int printValue(bsl::ostream& s, bool                    v);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ENCODING BOOLEAN" << endl
                          << "================" << endl;

        if (verbose) cout << "Encode 'true'" << endl;
        {
            bsl::ostringstream oss;
            ASSERTV(0 == Obj::printValue(oss, true));

            bsl::string result = oss.str();
            ASSERTV(result, result == "true");
        }

        if (verbose) cout << "Encode 'false'" << endl;
        {
            bsl::ostringstream oss;
            ASSERTV(0 == Obj::printValue(oss, false));

            bsl::string result = oss.str();
            ASSERTV(result, result == "false");
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Call 'printValue' on each value type.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        bsl::ostringstream oss;

        if (verbose) cout << "Test boolean" << endl;
        {
            Obj::printValue(oss, true);
            ASSERTV("true" == oss.str());
            oss.str("");

            Obj::printValue(oss, false);
            ASSERTV("false" == oss.str());
            oss.str("");
        }

        if (verbose) cout << "Test unsigned integers" << endl;
        {
            Obj::printValue(oss, (unsigned char) 1);
            ASSERTV("1" == oss.str());
            oss.str("");

            Obj::printValue(oss, (unsigned short) 2);
            ASSERTV("2" == oss.str());
            oss.str("");

            Obj::printValue(oss, (unsigned int) 3);
            ASSERTV("3" == oss.str());
            oss.str("");

            Obj::printValue(oss, (bsls::Types::Uint64) 4);
            ASSERTV("4" == oss.str());
            oss.str("");
        }

        if (verbose) cout << "Test signed integers" << endl;
        {
            Obj::printValue(oss, (char) -2);
            ASSERTV(oss.str(), "-2" == oss.str());
            oss.str("");

            Obj::printValue(oss, (short) -2);
            ASSERTV("-2" == oss.str());
            oss.str("");

            Obj::printValue(oss, (int) -3);
            ASSERTV("-3" == oss.str());
            oss.str("");

            Obj::printValue(oss, (bsls::Types::Int64) -4);
            ASSERTV("-4" == oss.str());
            oss.str("");
        }

        if (verbose) cout << "Test decimal" << endl;
        {
            Obj::printValue(oss, 3.14159f);
            ASSERTV("3.14159" == oss.str());
            oss.str("");

            Obj::printValue(oss, 3.1415926535);
            ASSERTV("3.1415926535" == oss.str());
            oss.str("");
        }

        if (verbose) cout << "Test string" << endl;
        {
            Obj::printValue(oss, "Hello");
            ASSERTV("\"Hello\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, bsl::string("World"));
            ASSERTV("\"World\"" == oss.str());
            oss.str("");
        }

        if (verbose) cout << "Test date/time" << endl;
        {
            const int YEAR        = 9999;
            const int MONTH       = 12;
            const int DAY         = 31;
            const int HOUR        = 23;
            const int MINUTE      = 59;
            const int SECOND      = 59;
            const int MILLISECOND = 999;
            const int MICROSECOND = 999;
            const int OFFSET      = -720;

            bdlt::Date theDate(YEAR, MONTH, DAY);
            bdlt::Time theTime(HOUR, MINUTE, SECOND, MILLISECOND,
                               MICROSECOND);
            bdlt::Datetime theDatetime(YEAR, MONTH, DAY,
                                       HOUR, MINUTE, SECOND,
                                       MILLISECOND, MICROSECOND);

            bdlt::DateTz     theDateTz(theDate, OFFSET);
            bdlt::TimeTz     theTimeTz(theTime, OFFSET);
            bdlt::DatetimeTz theDatetimeTz(theDatetime, OFFSET);

            Obj::printValue(oss, theDate);
            ASSERTV(oss.str(), "\"9999-12-31\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, theDateTz);
            ASSERTV(oss.str(),"\"9999-12-31-12:00\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, theTime);
            ASSERTV(oss.str(),"\"23:59:59.999\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, theTimeTz);
            ASSERTV(oss.str(),"\"23:59:59.999-12:00\"" == oss.str());
            oss.str("");

            {
                Obj::printValue(oss, theDatetime);
                ASSERTV(oss.str(),
                        "\"9999-12-31T23:59:59.999\"" == oss.str());
                oss.str("");
            }
            {
                baljsn::EncoderOptions opt;
                opt.setDatetimeFractionalSecondPrecision(6);
                Obj::printValue(oss, theDatetime, &opt);
                ASSERTV(oss.str(),"\"9999-12-31T23:59:59.999999\"" ==
                        oss.str());
                oss.str("");
            }
            {
                baljsn::EncoderOptions opt;
                opt.setDatetimeFractionalSecondPrecision(6);
                Obj::printValue(oss, theDatetimeTz, &opt);
                ASSERTV(oss.str(),
                        "\"9999-12-31T23:59:59.999999-12:00\"" == oss.str());
                oss.str("");
            }
            {
                baljsn::EncoderOptions opt;
                opt.setDatetimeFractionalSecondPrecision(6);
                Obj::printValue(oss, theTime, &opt);
                ASSERTV(oss.str(),"\"23:59:59.999999\"" == oss.str());
                oss.str("");
            }
            {
                baljsn::EncoderOptions opt;
                opt.setDatetimeFractionalSecondPrecision(6);
                Obj::printValue(oss, theTimeTz, &opt);
                ASSERTV(oss.str(),"\"23:59:59.999999-12:00\"" == oss.str());
                oss.str("");
            }
            {
                Obj::printValue(oss, theDatetimeTz);
                ASSERTV(oss.str(),
                        "\"9999-12-31T23:59:59.999-12:00\"" == oss.str());
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
// Copyright 2015 Bloomberg Finance L.P.
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
