// baljsn_printutil.t.cpp                                             -*-C++-*-
#include <baljsn_printutil.h>

#include <bslim_testutil.h>

#include <bsl_sstream.h>
#include <bsl_string.h>

#include <bdlt_time.h>
#include <bdlt_timetz.h>
#include <bdlt_date.h>
#include <bdlt_datetz.h>
#include <bdlt_datetime.h>
#include <bdlt_datetimetz.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>
#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;

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
// [ 2] static int printValue(bsl::ostream& s, bool                    v);
// [ 4] static int printValue(bsl::ostream& s, char                    v);
// [ 4] static int printValue(bsl::ostream& s, signed char             v);
// [ 4] static int printValue(bsl::ostream& s, unsigned char           v);
// [ 4] static int printValue(bsl::ostream& s, short                   v);
// [ 4] static int printValue(bsl::ostream& s, unsigned short          v);
// [ 4] static int printValue(bsl::ostream& s, int                     v);
// [ 4] static int printValue(bsl::ostream& s, unsigned int            v);
// [ 4] static int printValue(bsl::ostream& s, bsls::Types::Int64      v);
// [ 4] static int printValue(bsl::ostream& s, bsls::Types::Uint64     v);
// [ 4] static int printValue(bsl::ostream& s, float                   v);
// [ 4] static int printValue(bsl::ostream& s, double                  v);
// [ 3] static int printValue(bsl::ostream& s, const char             *v);
// [ 3] static int printValue(bsl::ostream& s, const bsl::string&      v);
// [ 5] static int printValue(bsl::ostream& s, const bdlt::Time&        v);
// [ 5] static int printValue(bsl::ostream& s, const bdlt::Date&        v);
// [ 5] static int printValue(bsl::ostream& s, const bdlt::Datetime&    v);
// [ 5] static int printValue(bsl::ostream& s, const bdlt::TimeTz&      v);
// [ 5] static int printValue(bsl::ostream& s, const bdlt::DateTz&      v);
// [ 5] static int printValue(bsl::ostream& s, const bdlt::DatetimeTz&  v);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 6] USAGE EXAMPLE

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

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    bool verbose             = argc > 2;
    bool veryVerbose         = argc > 3;
    bool veryVeryVerbose     = argc > 4;

    (void)veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 6: {
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
            int d_offset;
        } DATA[] = {
            //Line Year   Mon  Day  Hour  Min  Sec     ms   offset
            //---- ----   ---  ---  ----  ---  ---     --   ------

            // Valid dates and times
            { L_,     1,   1,   1,    0,   0,   0,     0,        0 },
            { L_,  2005,   1,   1,    0,   0,   0,     0,      -90 },
            { L_,   123,   6,  15,   13,  40,  59,     0,     -240 },
            { L_,  1999,  10,  12,   23,   0,   1,     0,     -720 },

            // Vary milliseconds
            { L_,  1999,  10,  12,   23,   0,   1,     0,       90 },
            { L_,  1999,  10,  12,   23,   0,   1,   456,      240 },
            { L_,  1999,  10,  12,   23,   0,   1,   999,      720 },
            { L_,  1999,  12,  31,   23,  59,  59,   999,      720 }
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
            "\"1999-12-31\""
        };

        const char *expectedDateTz[] = {
            "\"0001-01-01+00:00\"",
            "\"2005-01-01-01:30\"",
            "\"0123-06-15-04:00\"",
            "\"1999-10-12-12:00\"",
            "\"1999-10-12+01:30\"",
            "\"1999-10-12+04:00\"",
            "\"1999-10-12+12:00\"",
            "\"1999-12-31+12:00\""
        };

        const char *expectedTime[] = {
            "\"00:00:00.000\"",
            "\"00:00:00.000\"",
            "\"13:40:59.000\"",
            "\"23:00:01.000\"",
            "\"23:00:01.000\"",
            "\"23:00:01.456\"",
            "\"23:00:01.999\"",
            "\"23:59:59.999\""
        };

        const char *expectedTimeTz[] = {
            "\"00:00:00.000+00:00\"",
            "\"00:00:00.000-01:30\"",
            "\"13:40:59.000-04:00\"",
            "\"23:00:01.000-12:00\"",
            "\"23:00:01.000+01:30\"",
            "\"23:00:01.456+04:00\"",
            "\"23:00:01.999+12:00\"",
            "\"23:59:59.999+12:00\""
        };

        const char *expectedDatetime[] = {
            "\"0001-01-01T00:00:00.000\"",
            "\"2005-01-01T00:00:00.000\"",
            "\"0123-06-15T13:40:59.000\"",
            "\"1999-10-12T23:00:01.000\"",
            "\"1999-10-12T23:00:01.000\"",
            "\"1999-10-12T23:00:01.456\"",
            "\"1999-10-12T23:00:01.999\"",
            "\"1999-12-31T23:59:59.999\""
        };

        const char *expectedDatetimeTz[] = {
            "\"0001-01-01T00:00:00.000+00:00\"",
            "\"2005-01-01T00:00:00.000-01:30\"",
            "\"0123-06-15T13:40:59.000-04:00\"",
            "\"1999-10-12T23:00:01.000-12:00\"",
            "\"1999-10-12T23:00:01.000+01:30\"",
            "\"1999-10-12T23:00:01.456+04:00\"",
            "\"1999-10-12T23:00:01.999+12:00\"",
            "\"1999-12-31T23:59:59.999+12:00\""
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
            const int OFFSET      = DATA[ti].d_offset;;

            bdlt::Date       theDate(YEAR, MONTH, DAY);
            bdlt::Time       theTime(HOUR, MINUTE, SECOND, MILLISECOND);
            bdlt::Datetime   theDatetime(YEAR, MONTH, DAY,
                                        HOUR, MINUTE, SECOND, MILLISECOND);

            bdlt::DateTz     theDateTz(theDate, OFFSET);
            bdlt::TimeTz     theTimeTz(theTime, OFFSET);
            bdlt::DatetimeTz theDatetimeTz(theDatetime, OFFSET);

            if (verbose) cout << "Encode Date" << endl;
            {
                const char *EXP = expectedDate[ti];
                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, theDate));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode DateTz" << endl;
            {
                const char *EXP = expectedDateTz[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, theDateTz));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode Time" << endl;
            {
                const char *EXP = expectedTime[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, theTime));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode TimeTz" << endl;
            {
                const char *EXP = expectedTimeTz[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, theTimeTz));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode Datetime" << endl;
            {
                const char *EXP = expectedDatetime[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, theDatetime));

                bsl::string result = oss.str();
                ASSERTV(LINE, result, EXP, result == EXP);
            }

            if (verbose) cout << "Encode DatetimeTz" << endl;
            {
                const char *EXP = expectedDatetimeTz[ti];

                bsl::ostringstream oss;
                ASSERTV(LINE, 0 == Obj::printValue(oss, theDatetimeTz));

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
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "ENCODING NUMBERS" << endl
                          << "================" << endl;

        if (verbose) cout << "Encode float" << endl;
        {
            const struct {
                int         d_line;
                float       d_value;
                const char *d_result;
            } DATA[] = {
                //LINE         VALUE    RESULT
                //----         -----    ------

                { L_,            0.0f,  "0" },
                { L_,          0.125f,  "0.125" },
                { L_,            1.0f,  "1" },
                { L_,           10.0f,  "10" },
                { L_,           -1.5f,  "-1.5" },
                { L_,         -1.5e1f,  "-15" },
#if defined(BSLS_PLATFORM_OS_WINDOWS)
                { L_,   -1.23456e-20f,  "-1.23456e-020" },
                { L_,    1.23456e-20f,  "1.23456e-020" },
#else
                { L_,   -1.23456e-20f,  "-1.23456e-20" },
                { L_,    1.23456e-20f,  "1.23456e-20" },
#endif
                { L_,         1.0e-1f,  "0.1" },
                { L_,       0.123456f,  "0.123456" }
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
                                 bsl::numeric_limits<float>::signaling_NaN()));
        }

        if (verbose) cout << "Encode double" << endl;
        {
            const struct {
                int         d_line;
                double      d_value;
                const char *d_result;
            } DATA[] = {
                //LINE              VALUE  RESULT
                //----              -----  ------

                { L_,                0.0,  "0" },
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
                { L_,   0.12345678912345,  "0.12345678912345" }
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
                                bsl::numeric_limits<double>::signaling_NaN()));
        }

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
                { L_,  "\x1f", "\"\\u001f\"" },
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
            const int OFFSET      = -720;

            bdlt::Date theDate(YEAR, MONTH, DAY);
            bdlt::Time theTime(HOUR, MINUTE, SECOND, MILLISECOND);
            bdlt::Datetime theDatetime(YEAR, MONTH, DAY,
                                      HOUR, MINUTE, SECOND, MILLISECOND);

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

            Obj::printValue(oss, theDatetime);
            ASSERTV(oss.str(),"\"9999-12-31T23:59:59.999\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, theDatetimeTz);
            ASSERTV(oss.str(),
                    "\"9999-12-31T23:59:59.999-12:00\"" == oss.str());
            oss.str("");

            Obj::printValue(oss, theDatetimeTz);
            ASSERTV(oss.str(),
                    "\"9999-12-31T23:59:59.999-12:00\"" == oss.str());
            oss.str("");
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
