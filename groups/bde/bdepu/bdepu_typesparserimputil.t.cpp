// bdepu_typesparserimputil.t.cpp              -*-C++-*-

#include <bdepu_typesparserimputil.h>
#include <bdepu_parserimputil.h>

#include <bdet_datetime.h>
#include <bdet_date.h>
#include <bdet_time.h>

#include <bslma_defaultallocatorguard.h>   // for testing only
#include <bslma_testallocator.h>           // for testing only

#include <bsls_platform.h>
#include <bsls_platformutil.h>

#include <bsl_algorithm.h>
#include <bsl_iterator.h>
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_c_stdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We use standard table-based approach to testing where we put both input and
// expected output in the same table row and verify that the actual result
// matches the expected value.
//
// For each test case the test vectors include both cases where parsable
// string is (a) the only string in the input and (b) followed by a suffix
// string.
//
// We use the following abbreviations: (for documentation purposes only)
//  'cchar'      for 'const char'
//  'int64'      for 'bsls_PlatformUtil::Int64;
//  'bStr'       for 'bsl::string'
//  'bDate'      for 'bdet_Date'
//  'bDatez'     for 'bdet_DateTz'
//  'bDT'        for 'bdet_Datetime'
//  'bDTz'       for 'bdet_DatetimeTz'
//  'bTime'      for 'bdet_Time'
//  'bTimez'     for 'bdet_TimeTz'
//  'ArCh'       for 'vector<char>'
//
//  'c' in front of an abbreviation stands for 'const'.
//
//-----------------------------------------------------------------------------
// [ 2] parseChar(cchar **endPos, char *result, cchar *inputString);
// [ 1] parseCharRaw(cchar **endPos, char *result, cchar *inputString);
// [ 1] parseCharRaw(cchar **endPos, char *result, cchar *input, int *);
// [ 3] parseDate(cchar **endPos, bDate *res, cchar *in);
// [10] parseDatetime(cchar **endPos, bDT *res, cchar *in);
// [29] parseDatetimeTz(cchar **endPos, bDTz *res, cchar *in);
// [33] parseDateTz(cchar **endPos, bDatez *res, cchar *in);
// [ 4] parseDouble(cchar **endPos, double *res, cchar *in);
// [ 5] parseFloat(cchar **endPos, double *res, cchar *in);
// [ 6] parseInt(cchar **endPos, int *res, cchar *in, int base = 10);
// [ 7] parseInt64(cchar **endPos, int64 *res, cchar *in, int base = 10);
// [ 8] parseShort(cchar **endPos, short *res, cchar *in, int base = 10);
// [ 9] parseTime(cchar **endPos, bTime *res, cchar *in);
// [31] parseTimeTz(cchar **endPos, bTimez *res, cchar *in);
// [27] parseTz(cchar **endPos, int *res, cchar *in);
//
// [11] parseDelimitedString(cchar **, bStr *, cchar *, cchar, cchar);
// [12] parseQuotedString(cchar **endPos, bStr *res, cchar *in);
// [13] parseSpaceDelimitedString(cchar **endPos, bStr *res, cchar *in);
// [15] parseString(cchar **endPos, bStr *res, cchar *in);
// [14] parseUnquotedString(cchar **endPos, bStr *res, cchar *in);
//
// [19] generateLongDouble(cchar *buffer, long double value, int length);
// [19] generateDouble(cchar *buffer, double value, int length);
// [20] generateFloat(cchar *buffer, float value, int length);
// [21] generateInt(cchar *buffer, int value, int length, int base);
// [22] generateInt64(cchar *buffer, int64 value, int length, int base);
// [23] generateShort(cchar *buffer, short value, int length, int base);
//
// [19] generateLongDoubleRaw(cchar *buffer, long double value, int length);
// [19] generateDoubleRaw(cchar *buffer, double value, int length);
// [20] generateFloatRaw(cchar *buffer, float value, int length);
// [21] generateIntRaw(cchar *buffer, int value, int length, int base);
// [22] generateInt64Raw(cchar *buffer, int64 value, int length, int base);
// [23] generateShortRaw(cchar *buffer, short value, int length, int base);
//
// [17] generateChar(ArCh *buffer, char value);
// [16] generateCharRaw(ArCh *buffer, char value);
// [18] generateDate(ArCh *buffer, bDate value);
// [26] generateDatetime(ArCh *buffer, bDT value);
// [34] generateDatetimeTz(ArCh *buffer, const bDT& value);
// [30] generateDateTz(ArCh *buffer, const bDatez& value);
// [19] generateDouble(ArCh *buffer, double value);
// [20] generateFloat(ArCh *buffer, float value);
// [21] generateInt(ArCh *buffer, int value, int base);
// [22] generateInt64(ArCh *buffer, int64 value, int base);
// [23] generateShort(ArCh *buffer, short value, int base);
// [24] generateString(ArCh *buffer, cchar *value);
// [25] generateTime(ArCh *buffer, bTime value);
// [32] generateTimeTz(ArCh *buffer, const bTimez& value);
// [28] generateTz(ArCh *buffer, int value);
//-----------------------------------------------------------------------------
// [35] USAGE EXAMPLE

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls_PlatformUtil::Int64 Int64;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 35: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Usage Example" << endl
                          << "=====================" << endl;

        int initial = 37;

        vector<char> buffer;
        bdepu_TypesParserImpUtil::generateInt(&buffer, initial);
        buffer.push_back('\0');
        ASSERT(0 == strncmp("37", &buffer.front(), 2));

        int final = 0;
        const char *pos;
        bdepu_TypesParserImpUtil::parseInt(&pos, &final, &buffer.front());
        ASSERT(&buffer[2] == pos);
        ASSERT(initial    == final);
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DATETIMETZ
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateDatetimeTz(ArCh *buffer, const bDTz& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDatetimeTz" << endl
                          << "==========================" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            const char *d_spec_p;          // specification string
            int         d_year;            // specification year
            int         d_month;           // specification month
            int         d_day;             // specification day
            int         d_hour;            // specification hour
            int         d_minute;          // specification minute
            int         d_second;          // specification second
            int         d_millisecond;     // specification millisecond
            const char *d_exp_p;           // expected result
        } DATA1[] = {
            // LN spec  year MM  DD  HH  MM  SS  MS  exp
            //--- ----  ---- --  --  --  --  --  ---
            { L_, "",     1,  2,  3,  1,  2,  3,   4,
                                        "0001/02/03 01:02:03.004" },
            { L_, "",    10,  2,  3, 12, 21, 32,  40,
                                        "0010/02/03 12:21:32.040" },
            { L_, "",   100,  2,  3, 12, 21, 32, 417,
                                        "0100/02/03 12:21:32.417" },
            { L_, "",  1000,  2,  3, 23,  0,  0,   0,
                                        "1000/02/03 23:00:00.000" },
            { L_, "",  9999,  2,  3, 12, 21, 32,  40,
                                        "9999/02/03 12:21:32.040" },
            { L_, "",  9999, 12, 31,  1,  2,  3,   4,
                                        "9999/12/31 01:02:03.004" },

            { L_, "x",     1,  2,  3,  1,  2,  3,   4,
                                        "x0001/02/03 01:02:03.004" },
            { L_, "x",    10,  2,  3, 12, 21, 32,  40,
                                        "x0010/02/03 12:21:32.040" },
            { L_, "x",   100,  2,  3, 12, 21, 32, 417,
                                        "x0100/02/03 12:21:32.417" },
            { L_, "x",  1000,  2,  3, 23,  0,  0,   0,
                                        "x1000/02/03 23:00:00.000" },
            { L_, "x",  9999,  2,  3, 12, 21, 32,  40,
                                        "x9999/02/03 12:21:32.040" },
            { L_, "x",  9999, 12, 31,  1,  2,  3,   4,
                                        "x9999/12/31 01:02:03.004" },
        };
        const int NUM_DATA1 = sizeof DATA1 / sizeof *DATA1;

        static const struct {
            int         d_lineNum;  // source line number
            int         d_offset;   // TZ offset
            const char *d_exp;      // expected result
        } DATA2[] = {
            //line  offset   result
            //----  -------  -----------------
            { L_,      0,    "+0000" },
            { L_,     -0,    "+0000" },
            { L_,     60,    "+0100" },
            { L_,    -60,    "-0100" },
            { L_,    180,    "+0300" },
            { L_,    270,    "+0430" },
            { L_,   -299,    "-0459" },
            { L_,   -300,    "-0500" },
            { L_,   -301,    "-0501" },
            { L_,   -675,    "-1115" },
            { L_,    705,    "+1145" },
            { L_,   -900,    "-1500" },
            { L_,    915,    "+1515" },
            { L_,   1020,    "+1700" },
            { L_,  -1065,    "-1745" },
        };
        const int NUM_DATA2 = sizeof DATA2 / sizeof *DATA2;

        for (int ti = 0; ti < NUM_DATA1; ++ti) {
            const int LINE1         = DATA1[ti].d_lineNum;
            const char *const SPEC  = DATA1[ti].d_spec_p;
            const int YEAR          = DATA1[ti].d_year;
            const int MONTH         = DATA1[ti].d_month;
            const int DAY           = DATA1[ti].d_day;
            const int HOUR          = DATA1[ti].d_hour;
            const int MINUTE        = DATA1[ti].d_minute;
            const int SECOND        = DATA1[ti].d_second;
            const int MILLISECOND   = DATA1[ti].d_millisecond;
            const char *const EXP1  = DATA1[ti].d_exp_p;
            const int curLen        = strlen(SPEC);
            const bdet_Datetime VALUE1(YEAR,
                                       MONTH,
                                       DAY,
                                       HOUR,
                                       MINUTE,
                                       SECOND,
                                       MILLISECOND);

            for (int tj = 0; tj < NUM_DATA2 ; ++tj) {
                const int LINE2        = DATA2[tj].d_lineNum;
                const int OFFSET       = DATA2[tj].d_offset;
                const bsl::string EXP2 = DATA2[tj].d_exp;

                const bdet_DatetimeTz VALUE(VALUE1, OFFSET);

                vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_TypesParserImpUtil::generateDatetimeTz(&buffer, VALUE);
                buffer.push_back(0);

                bsl::string EXPSTR = EXP1;
                EXPSTR += EXP2;
                vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                exp.push_back(0);

                LOOP2_ASSERT(LINE1, LINE2, exp == buffer);

                if (0 == curLen) {
                    bdet_DatetimeTz result;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                            parseDatetimeTz(&endPos, &result, &buffer.front());
                    LOOP2_ASSERT(LINE1, LINE2, 0 == rv);
                    LOOP2_ASSERT(LINE1, LINE2, VALUE == result);
                }
            }
        }
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING PARSE DATETIMETZ
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseDatetimeTz(cchar **endPos, bDTz *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseDatetimeTz" << endl
                          << "=======================" << endl;

        const bdet_Date D1(   1, 1, 1);
        const bdet_Date D2(1923, 7, 9);
        const bdet_Datetime DT1(D1);
        const bdet_Datetime DT2(D2);
        const bdet_DatetimeTz INITIAL_VALUE_1(DT1, 0);
        const bdet_DatetimeTz INITIAL_VALUE_2(DT2, -180);

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_offset;   // expected number of parsed characters
            int         d_fail;     // parsing expected to fail indicator
            int         d_year;     // expected return year
            int         d_month;    // expected return month
            int         d_day;      // expected return day
            int         d_hour;     // expected return hour
            int         d_minute;   // expected return minute
            int         d_second;   // expected return second
            int         d_tz;       // expected return tz minutes
        } DATA[] = {
            //line spec                     off fail
            //                             year mon day  hh  mm  ss offset
            //---- ------------------------ --- --- ---- --- --- -- -- -- ----
            { L_, "",                         0,  1                       },
            { L_, "1993/01/02",              10,  1                       },
            { L_, "1993/01/02 3:04",         15,  1                       },
            { L_, "1993/01/02_3:04",         15,  1                       },
            { L_, "1993/01/02_3:04 ",        15,  1                       },
            { L_, "1993/01/02_3:04-",        16,  1                       },
            { L_, "1993/01/02_3:04+",        16,  1                       },
            { L_, "1993/01/02 3:04+0300",    20,  0,
                                           1993,  1,  2,  3,  4,  0,  180 },
            { L_, "1993/01/02 3:04-1015",    20,  0,
                                           1993,  1,  2,  3,  4,  0, -615 },
            { L_, "1993/1/02 03:14+0015",    20,  0,
                                           1993,  1,  2,  3, 14,  0,   15 },
            { L_, "1993/01/02_3:04-0500",    20,  0,
                                           1993,  1,  2,  3,  4,  0, -300 },
            { L_, "1993/01/02 3:04:10",      18,  1,                      },
            { L_, "1993/01/02 3:04:10 ",     18,  1,                      },
            { L_, "1993/01/02 3:04:10+",     19,  1,                      },
            { L_, "1993/01/02 3:04:10-",     19,  1,                      },
            { L_, "1993/01/2_3:04:10-0500",  22,  0,
                                           1993,  1,  2,  3,  4, 10, -300 },
            { L_, "1993/01/02_3:05:10+0000", 23,  0,
                                           1993,  1,  2,  3,  5, 10,    0 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int LINE         = DATA[ti].d_lineNum;
            const char *const SPEC = DATA[ti].d_spec_p;
            const int NUM          = DATA[ti].d_offset;
            const int FAIL         = DATA[ti].d_fail;
            const int YEAR         = DATA[ti].d_year;
            const int MONTH        = DATA[ti].d_month;
            const int DAY          = DATA[ti].d_day;
            const int HOUR         = DATA[ti].d_hour;
            const int MINUTE       = DATA[ti].d_minute;
            const int SECOND       = DATA[ti].d_second;
            const int OFFSET       = DATA[ti].d_tz;

            bdet_DatetimeTz value;
            if (!FAIL) {
                const bdet_Date DATE(YEAR, MONTH, DAY);
                const bdet_Time TIME(HOUR, MINUTE, SECOND);
                const bdet_Datetime DATETIME(DATE, TIME);
                value.setDatetimeTz(DATETIME, OFFSET);
            }

            const bdet_DatetimeTz& VALUE = value;

            {  // test with first initial value
                bdet_DatetimeTz result = INITIAL_VALUE_1;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                                     parseDatetimeTz(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
            }

            {  // test with second initial value
                bdet_DatetimeTz result = INITIAL_VALUE_2;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                                     parseDatetimeTz(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
            }
        }
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING GENERATE TIMETZ
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateTimeTz(ArCh *buffer, const bTimez& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateTimeTz" << endl
                          << "======================" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            const char *d_spec_p;          // specification string
            int         d_hour;            // specification hour
            int         d_minute;          // specification minute
            int         d_second;          // specification second
            int         d_millisecond;     // specification millisecond
            const char *d_exp_p;           // expected result
        } DATA1[] = {
            //line  spec   hour  min  sec  milli  exp
            //----  -----  ----  ---  ---  -----  ----------------------
            { L_,   "",       1,   2,   3,     4, "01:02:03.004"         },
            { L_,   "",      12,  21,  32,    40, "12:21:32.040"         },
            { L_,   "",      12,  21,  32,   417, "12:21:32.417"         },
            { L_,   "",      23,   0,   0,     0, "23:00:00.000"         },

            { L_,   "x",      1,   2,   3,     4, "x01:02:03.004"        },
            { L_,   "x",     12,  21,  32,    40, "x12:21:32.040"        },
            { L_,   "x",     12,  21,  32,   417, "x12:21:32.417"        },
            { L_,   "x",     23,   0,   0,     0, "x23:00:00.000"        },
        };
        const int NUM_DATA1 = sizeof DATA1 / sizeof *DATA1;

        static const struct {
            int         d_lineNum;  // source line number
            int         d_offset;   // TZ offset
            const char *d_exp;      // expected result
        } DATA2[] = {
            //line  offset   result
            //----  -------  -----------------
            { L_,      0,    "+0000" },
            { L_,     -0,    "+0000" },
            { L_,     60,    "+0100" },
            { L_,    -60,    "-0100" },
            { L_,    180,    "+0300" },
            { L_,    270,    "+0430" },
            { L_,   -299,    "-0459" },
            { L_,   -300,    "-0500" },
            { L_,   -301,    "-0501" },
            { L_,   -675,    "-1115" },
            { L_,    705,    "+1145" },
            { L_,   -900,    "-1500" },
            { L_,    915,    "+1515" },
            { L_,   1020,    "+1700" },
            { L_,  -1065,    "-1745" },
        };
        const int NUM_DATA2 = sizeof DATA2 / sizeof *DATA2;

        for (int ti = 0; ti < NUM_DATA1; ++ti) {
            const int LINE1         = DATA1[ti].d_lineNum;
            const char *const SPEC  = DATA1[ti].d_spec_p;
            const int HOUR          = DATA1[ti].d_hour;
            const int MINUTE        = DATA1[ti].d_minute;
            const int SECOND        = DATA1[ti].d_second;
            const int MILLISECOND   = DATA1[ti].d_millisecond;
            const char *const EXP1  = DATA1[ti].d_exp_p;
            const int curLen        = strlen(SPEC);
            const bdet_Time VALUE1(HOUR, MINUTE, SECOND, MILLISECOND);

            for (int tj = 0; tj < NUM_DATA2 ; ++tj) {
                const int LINE2        = DATA2[tj].d_lineNum;
                const int OFFSET       = DATA2[tj].d_offset;
                const bsl::string EXP2 = DATA2[tj].d_exp;

                const bdet_TimeTz VALUE(VALUE1, OFFSET);

                vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_TypesParserImpUtil::generateTimeTz(&buffer, VALUE);
                buffer.push_back(0);

                bsl::string EXPSTR = EXP1;
                EXPSTR += EXP2;
                vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                exp.push_back(0);

                LOOP2_ASSERT(LINE1, LINE2, exp == buffer);

                if (0 == curLen) {
                    bdet_TimeTz result;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                               parseTimeTz(&endPos, &result, &buffer.front());
                    LOOP2_ASSERT(LINE1, LINE2, 0 == rv);
                    LOOP2_ASSERT(LINE1, LINE2, VALUE == result);
                }
            }
        }
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING PARSE TIMETZ
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseTimeTz(cchar **endPos, bTimez *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseTimeTz" << endl
                          << "===================" << endl;

        const bdet_Time INITIAL_TIME_1( 1, 1, 1);  // 1st init. time
        const bdet_Time INITIAL_TIME_2(19, 7, 9);  // 2nd init. time

        const int INITIAL_OFFSET_1 = 180;          // 1st init. offset
        const int INITIAL_OFFSET_2 = -90;          // 2nd init. offset

        const bdet_TimeTz INITIAL_VALUE_1(INITIAL_TIME_1, INITIAL_OFFSET_1);
        const bdet_TimeTz INITIAL_VALUE_2(INITIAL_TIME_2, INITIAL_OFFSET_2);

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_offset;   // expected number of parsed characters
            int         d_fail;     // parsing expected to fail indicator
            int         d_hour;     // expected return year
            int         d_minute;   // expected return minute
            int         d_second;   // expected return second
            int         d_milli;    // expected return millisecond
            int         d_tz;       // expected return tz minutes
        } DATA[] = {
            //line spec               off  fail  hour  min  sec  milli  offset
            //---- ---------------    ---  ----  ----  ---  ---  -----  ------
            { L_, "",                  0,    1,                             },
            { L_, " ",                 0,    1,                             },
            { L_, "1",                 1,    1,                             },
            { L_, "11",                2,    1,                             },
            { L_, "11:",               3,    1,                             },
            { L_, "011:",              2,    1,                             },
            { L_, "11:0",              4,    1,                             },
            { L_, "11:1:",             4,    1,                             },
            { L_, "11:01",             5,    1,                             },
            { L_, "11:01 ",            5,    1,                             },
            { L_, "11:01+",            6,    1,                             },
            { L_, "11:01-",            6,    1,                             },
            { L_, "11:01+1800",       10,    0,   11,   1,   0,     0, 1080 },
            { L_, "11:60-0100",        3,    1,                             },
            { L_, "11:01:",            6,    1,                             },
            { L_, "11:01:0",           7,    1,                             },
            { L_, "11:01:2",           7,    1,                             },
            { L_, "11:01:2+",          7,    1,                             },
            { L_, "11:01:02",          8,    1,                             },
            { L_, "11:01:02 ",         8,    1,                             },
            { L_, "11:01:02+",         9,    1,                             },
            { L_, "11:01:02-023",     12,    1,                             },
            { L_, "11:01:02-0030",    13,    0,   11,   1,   2,     0,  -30 },
            { L_, "02:34:56+1145",    13,    0,    2,  34,  56,     0,  705 },
            { L_, "11:01:60+0000",     6,    1,                             },
            { L_, "23:00:00+0300",    13,    0,   23,   0,   0,     0,  180 },
            { L_, "24:01:00+0845",     4,    1,                             },
            { L_, "11:01:02.23+0215", 16,    0,   11,   1,   2,   230,  135 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        int oldLen = -1;
        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int LINE         = DATA[ti].d_lineNum;
            const char *const SPEC = DATA[ti].d_spec_p;
            const int NUM          = DATA[ti].d_offset;
            const int FAIL         = DATA[ti].d_fail;
            const int HOUR         = DATA[ti].d_hour;
            const int MINUTE       = DATA[ti].d_minute;
            const int SECOND       = DATA[ti].d_second;
            const int MILLI        = DATA[ti].d_milli;
            const int OFFSET       = DATA[ti].d_tz;
            const bdet_Time VALUE1(HOUR, MINUTE, SECOND, MILLI);
            const bdet_TimeTz VALUE(VALUE1, OFFSET);

            {  // test with first initial value
                bdet_TimeTz result = INITIAL_VALUE_1;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                               parseTimeTz(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE, result == (rv ? INITIAL_VALUE_1 : VALUE));
            }

            {  // test with second initial value
                bdet_TimeTz result = INITIAL_VALUE_2;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                                parseTimeTz(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE, result == (rv ? INITIAL_VALUE_2 : VALUE));
            }
        }
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DATETZ
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateDateTz(ArCh *buffer, const bDateTz& value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDateTz" << endl
                          << "======================" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            const char *d_spec_p;          // specification string
            int         d_year;            // specification year
            int         d_month;           // specification month
            int         d_day;             // specification day
            const char *d_exp_p;           // expected result
        } DATA1[] = {
            //line  spec   year  month  day  exp
            //----  -----  ----  -----  ---  -----------------------
            { L_,   "",       1,     2,   3, "0001/02/03"            },
            { L_,   "",      10,     2,   3, "0010/02/03"            },
            { L_,   "",     100,     2,   3, "0100/02/03"            },
            { L_,   "",    1000,     2,   3, "1000/02/03"            },
            { L_,   "",    9999,     2,   3, "9999/02/03"            },
            { L_,   "",    9999,    12,  31, "9999/12/31"            },

            { L_,   "x",      1,     2,   3, "x0001/02/03"           },
            { L_,   "x",     10,     2,   3, "x0010/02/03"           },
            { L_,   "x",    100,     2,   3, "x0100/02/03"           },
            { L_,   "x",   1000,     2,   3, "x1000/02/03"           },
            { L_,   "x",   9999,     2,   3, "x9999/02/03"           },
            { L_,   "x",   9999,    12,  31, "x9999/12/31"           },
        };
        const int NUM_DATA1 = sizeof DATA1 / sizeof *DATA1;

        static const struct {
            int         d_lineNum;  // source line number
            int         d_offset;   // TZ offset
            const char *d_exp;      // expected result
        } DATA2[] = {
            //line  offset   result
            //----  -------  -----------------
            { L_,      0,    "+0000" },
            { L_,     -0,    "+0000" },
            { L_,     60,    "+0100" },
            { L_,    -60,    "-0100" },
            { L_,    180,    "+0300" },
            { L_,    270,    "+0430" },
            { L_,   -299,    "-0459" },
            { L_,   -300,    "-0500" },
            { L_,   -301,    "-0501" },
            { L_,   -675,    "-1115" },
            { L_,    705,    "+1145" },
            { L_,   -900,    "-1500" },
            { L_,    915,    "+1515" },
            { L_,   1020,    "+1700" },
            { L_,  -1065,    "-1745" },
        };
        const int NUM_DATA2 = sizeof DATA2 / sizeof *DATA2;

        for (int ti = 0; ti < NUM_DATA1; ++ti) {
            const int LINE1         = DATA1[ti].d_lineNum;
            const char *const SPEC  = DATA1[ti].d_spec_p;
            const int YEAR          = DATA1[ti].d_year;
            const int MONTH         = DATA1[ti].d_month;
            const int DAY           = DATA1[ti].d_day;
            const char *const EXP1  = DATA1[ti].d_exp_p;
            const int curLen        = strlen(SPEC);
            const bdet_Date VALUE1(YEAR, MONTH, DAY);

            for (int tj = 0; tj < NUM_DATA2 ; ++tj) {
                const int LINE2        = DATA2[tj].d_lineNum;
                const int OFFSET       = DATA2[tj].d_offset;
                const bsl::string EXP2 = DATA2[tj].d_exp;

                const bdet_DateTz VALUE(VALUE1, OFFSET);

                vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_TypesParserImpUtil::generateDateTz(&buffer, VALUE);
                buffer.push_back(0);

                bsl::string EXPSTR = EXP1;
                EXPSTR += EXP2;
                vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                exp.push_back(0);

                LOOP2_ASSERT(LINE1, LINE2, exp == buffer);

                if (0 == curLen) {
                    bdet_DateTz result;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                               parseDateTz(&endPos, &result, &buffer.front());
                    LOOP2_ASSERT(LINE1, LINE2, 0 == rv);
                    LOOP2_ASSERT(LINE1, LINE2, VALUE == result);
                }
            }
        }
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING PARSE DATETZ
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseDateTz(cchar **endPos, bDatez *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseDateTz" << endl
                          << "===================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_offset;   // expected number of parsed characters
            int         d_fail;     // parsing expected to fail indicator
            int         d_year;     // expected return year
            int         d_month;    // expected return month
            int         d_day;      // expected return day
            int         d_tz;       // expected return tz minutes
        } DATA[] = {
            //line  spec                 off  fail  year  month  day  offset
            //----  ---------------      ---  ----  ----  -----  ---  -------
            { L_,   "",                   0,    1,                          },
            { L_,   " ",                  0,    1,                          },
            { L_,   "+0200",              0,    1,                          },
            { L_,   "1",                  1,    1,                          },
            { L_,   "19",                 2,    1,                          },
            { L_,   "199-0000",           3,    1,                          },
            { L_,   "1993",               4,    1,                          },
            { L_,   "1993/",              5,    1,                          },
            { L_,   "1993/0",             5,    1,                          },
            { L_,   "19/1/2",             6,    1,                          },
            { L_,   "1993/01",            7,    1,                          },
            { L_,   "1993/13",            6,    1,                          },
            { L_,   "1993/01/",           8,    1,                          },
            { L_,   "1993/01/0",          8,    1,                          },
            { L_,   "1993/01/32",         9,    1,                          },
            { L_,   "1993/02/31",         9,    1,                          },
            { L_,   "199/01/02",          9,    1,                          },
            { L_,   "1993/01/2",          9,    1,                          },
            { L_,   "1993/01/02",        10,    1,                          },
            { L_,   "1993/01/02 ",       10,    1,                          },
            { L_,   "1993/01/02-",       11,    1,                          },
            { L_,   "1993/01/02+",       11,    1,                          },
            { L_,   "1993/01/02+01",     13,    1,                          },
            { L_,   "1993/01/02-013",    14,    1,                          },
            { L_,   "1993/01/02+0130",   15,    0, 1993,     1,   2,    90  },
            { L_,   "1993/01/02-1100",   15,    0, 1993,     1,   2,  -660  },
            { L_,   "1993/01/02+1800",   15,    0, 1993,     1,   2,  1080  },
            { L_,   "1993/01/02+0000",   15,    0, 1993,     1,   2,     0  },
            { L_,   "1993/01/02-0000",   15,    0, 1993,     1,   2,     0  },
            { L_,   "1993/01/02+0130,",  15,    0, 1993,     1,   2,    90  },
            { L_,   "1993/01/02 +0130,", 10,    1,                          },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        const bdet_Date INITIAL_DATE_1(   1, 1, 1);  // 1st init. date
        const bdet_Date INITIAL_DATE_2(1923, 7, 9);  // 2nd init. date

        const int INITIAL_OFFSET_1 = 180;            // 1st init. offset
        const int INITIAL_OFFSET_2 = -90;            // 2nd init. offset

        const bdet_DateTz INITIAL_VALUE_1(INITIAL_DATE_1, INITIAL_OFFSET_1);
        const bdet_DateTz INITIAL_VALUE_2(INITIAL_DATE_2, INITIAL_OFFSET_2);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int LINE         = DATA[ti].d_lineNum;
            const char *SPEC       = DATA[ti].d_spec_p;
            const int NUM          = DATA[ti].d_offset;
            const int FAIL         = DATA[ti].d_fail;
            const int YEAR         = DATA[ti].d_year;
            const int MONTH        = DATA[ti].d_month;
            const int DAY          = DATA[ti].d_day;
            const int OFFSET       = DATA[ti].d_tz;

            bdet_DateTz value;
            if (!FAIL) {
                const bdet_Date VALUE1(YEAR, MONTH, DAY);
                value.setDateTz(VALUE1, OFFSET);
            }

            const bdet_DateTz& VALUE = value;

            {  // test with first initial value
                bdet_DateTz result = INITIAL_VALUE_1;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                               parseDateTz(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE, result == (rv ? INITIAL_VALUE_1 : value));
            }

            {  // test with second initial value
                bdet_DateTz result = INITIAL_VALUE_2;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                                parseDateTz(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE, result == (rv ? INITIAL_VALUE_2 : value));
            }
        }
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING GENERATE TZ
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateTz(ArCh *buffer, int value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateTz" << endl
                          << "==================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            int         d_spec;     // specification string
            const char *d_expected; // expected value
        } DATA[] = {
            //line  spec     result
            //----  -------  -----------------
            { L_,      0,    "+0000" },
            { L_,     -0,    "+0000" },
            { L_,     60,    "+0100" },
            { L_,    -60,    "-0100" },
            { L_,    180,    "+0300" },
            { L_,    270,    "+0430" },
            { L_,   -299,    "-0459" },
            { L_,   -300,    "-0500" },
            { L_,   -301,    "-0501" },
            { L_,   -675,    "-1115" },
            { L_,    705,    "+1145" },
            { L_,   -900,    "-1500" },
            { L_,    915,    "+1515" },
            { L_,   1020,    "+1700" },
            { L_,  -1065,    "-1745" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int LINE         = DATA[ti].d_lineNum;
            const int SPEC         = DATA[ti].d_spec;
            const bsl::string RES  = DATA[ti].d_expected;

            vector<char> buffer(&testAllocator);
            bdepu_TypesParserImpUtil::generateTz(&buffer, SPEC);
            buffer.push_back(0);

            vector<char> exp(&testAllocator);
            exp.insert(exp.end(), RES.begin(), RES.end());
            exp.push_back(0);
            LOOP_ASSERT(LINE, exp == buffer);

            int result = -999999;
            const char *endPos = 0;
            int rv = bdepu_TypesParserImpUtil::
                        parseTz(&endPos, &result, &buffer.front());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, SPEC == result);
        }
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING PARSE TZ
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseTz(cchar **endPos, int *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseTz" << endl
                          << "===============" << endl;

        {
            const int INITIAL_VALUE_1 = 74712;            // first init. value
            const int INITIAL_VALUE_2 = -984332;          // second init. value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_result;   // expected result value
            } DATA[] = {
                //line  spec             off  fail  result
                //----  ---------------  ---  ----  ---
                { L_,   "",                0,    1,      },
                { L_,   " ",               0,    1,      },
                { L_,   "3",               0,    1,      },
                { L_,   "-",               1,    1,      },
                { L_,   "+",               1,    1,      },
                { L_,   "+1",              2,    1,      },
                { L_,   "-7",              2,    1,      },
                { L_,   "-72",             1,    1,      },
                { L_,   "+023",            4,    1,      },
                { L_,   "-117",            4,    1,      },
                { L_,   "+000",            4,    1,      },
                { L_,   "+0000",           5,    0,    0 },
                { L_,   "-0000",           5,    0,    0 },
                { L_,   "+0741",           5,    0,  461 },
                { L_,   "-0741",           5,    0, -461 },
                { L_,   " 0430",           0,    1,      },
                { L_,   "+0030",           5,    0,   30 },
                { L_,   "-0030",           5,    0,  -30 },
                { L_,   "+0500",           5,    0,  300 },
                { L_,   "-0545",           5,    0, -345 },
                { L_,   "+0901",           5,    0,  541 },
                { L_,   "-1115",           5,    0, -675 },
                { L_,   "+1200",           5,    0,  720 },
                { L_,   "-1200",           5,    0, -720 },
                { L_,   "+1800",           5,    0, 1080 },
                { L_,   "-2000",           5,    0,-1200 },
                { L_,   "-07412",          5,    0, -461 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int VALUE        = DATA[ti].d_result;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    int result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                             parseTz(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    int result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                           parseTz(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DATETIME
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateDatetime(ArCh *buffer, bDT value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDatetime" << endl
                          << "========================" << endl;

        {
            {
                bdet_Date DATE(1993, 12, 2);
                bdet_Time TIME(15, 3, 7, 23);
                bdet_Datetime VALUE(DATE, TIME);
                const bsl::string EXP = "1993/12/02 15:03:07.023";

                vector<char> buffer(&testAllocator);
                bdepu_TypesParserImpUtil::generateDatetime(&buffer, VALUE);
                buffer.push_back(0);
                vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXP.begin(), EXP.end());
                exp.push_back(0);
                ASSERT(exp == buffer);

                bdet_Datetime result;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                              parseDatetime(&endPos, &result, &buffer.front());
                ASSERT(0 == rv);
                ASSERT(VALUE == result);
            }
        }
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING GENERATE TIME
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateTime(ArCh *buffer, bTime value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateTime" << endl
                          << "====================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_spec_p;          // specification string
                int         d_hour;            // specification hour
                int         d_minute;          // specification minute
                int         d_second;          // specification second
                int         d_millisecond;     // specification millisecond
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  spec   hour  min  sec  milli  exp
                //----  -----  ----  ---  ---  -----  ----------------------
                { L_,   "",       1,   2,   3,     4, "01:02:03.004"         },
                { L_,   "",      12,  21,  32,    40, "12:21:32.040"         },
                { L_,   "",      12,  21,  32,   417, "12:21:32.417"         },
                { L_,   "",      24,   0,   0,     0, "24:00:00.000"         },

                { L_,   "x",      1,   2,   3,     4, "x01:02:03.004"        },
                { L_,   "x",     12,  21,  32,    40, "x12:21:32.040"        },
                { L_,   "x",     12,  21,  32,   417, "x12:21:32.417"        },
                { L_,   "x",     24,   0,   0,     0, "x24:00:00.000"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const int HOUR          = DATA[ti].d_hour;
                const int MINUTE        = DATA[ti].d_minute;
                const int SECOND        = DATA[ti].d_second;
                const int MILLISECOND   = DATA[ti].d_millisecond;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(SPEC);
                const bdet_Time VALUE(HOUR, MINUTE, SECOND, MILLISECOND);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_TypesParserImpUtil::generateTime(&buffer, VALUE);
                buffer.push_back(0);
                const bsl::string EXPSTR = EXP;
                vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                exp.push_back(0);
                LOOP_ASSERT(LINE, exp == buffer);

                if (0 == curLen) {
                    bdet_Time result;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                  parseTime(&endPos, &result, &buffer.front());
                    LOOP_ASSERT(LINE, 0 == rv);
                    LOOP_ASSERT(LINE, VALUE == result);
                }
            }
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING GENERATE STRING
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateString(ArCh *buffer, cchar *value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateString" << endl
                          << "======================" << endl;

        {
            vector<char> buffer(&testAllocator);

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_text_p;   // input string
                const char *d_value_p;  // specification value
                  const char *d_exp_p;    // expected value
            } DATA[] = {
                //line  text      value         exp
                //----  --------  ------------  --------------
                { L_,   "",       "",           "\"\""         },
                { L_,   "",       "a",          "a"            },
                { L_,   "",       "]",          "\"]\""        },
                { L_,   "",       "ab",         "ab"           },
                { L_,   "",       "//",          "\"//\""      },
                { L_,   "",       "/*",          "\"/*\""      },
                { L_,   "",       "a b",        "\"a b\""      },

                // TBD need more vectors
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const TEXT  = DATA[ti].d_text_p;
                const char *const VALUE = DATA[ti].d_value_p;
                const char *const EXP   = DATA[ti].d_exp_p;

                buffer.clear();
                const bsl::string TEXTSTR = TEXT;
                buffer.insert(buffer.end(), TEXTSTR.begin(), TEXTSTR.end());

                if (veryVerbose) {
                    cout << "\t";
                    P_(TEXT);
                    P_(VALUE);
                    P_(EXP);
                }

                bdepu_TypesParserImpUtil::generateString(&buffer, VALUE);

                if (veryVerbose) {
                    cout << "\tresult = ";
                    copy(buffer.begin(), buffer.end(),
                         ostream_iterator<char>(cout));
                    cout << endl;
                }

                LOOP_ASSERT(LINE, strlen(EXP) == buffer.size());
                LOOP_ASSERT(LINE,
                            0 == strncmp(EXP, &buffer.front(), buffer.size()));

                // Parse the data and verify that the value is recovered.
                buffer.push_back('\0');
                bsl::string result;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                                   parseString(&endPos,
                                               &result,
                                               &buffer.front() + strlen(TEXT));
                LOOP_ASSERT(LINE, 0 == rv);
                LOOP_ASSERT(LINE,
                            &buffer.front() + buffer.size() - 1 == endPos);
                LOOP_ASSERT(LINE, VALUE == result);
            }
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING GENERATE SHORT
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateShort(ArCh *buffer, short value, int base);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateShort" << endl
                          << "=====================" << endl;

        {
            vector<char> buffer(&testAllocator);

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_text_p;   // input string
                short       d_value;    // expected value
                int         d_base;     // base
                  const char *d_exp_p;    // expected value
            } DATA[] = {
                //line  text      value   base  exp
                //----  --------  ------  ----  --------------
                  { L_,   "",       -32768,   10, "-32768"       },
                  { L_,   "",           -1,   10, "-1"           },
                  { L_,   "",            0,   10, "0"            },
                  { L_,   "",            1,   10, "1"            },
                  { L_,   "",        32767,   10, "32767"        },

                  { L_,   "",       -32768,   16, "-8000"        },
                  { L_,   "",           -1,   16, "-1"           },
                  { L_,   "",            0,   16, "0"            },
                  { L_,   "",            1,   16, "1"            },
                  { L_,   "",        32767,   16, "7FFF"         },

                  { L_,   "abc",    -32768,   10, "abc-32768"    },
                  { L_,   "abc",        -1,   10, "abc-1"        },
                  { L_,   "abc",         0,   10, "abc0"         },
                  { L_,   "abc",         1,   10, "abc1"         },
                  { L_,   "abc",     32767,   10, "abc32767"     },

                  { L_,   "abc",    -32768,   16, "abc-8000"     },
                  { L_,   "abc",        -1,   16, "abc-1"        },
                  { L_,   "abc",         0,   16, "abc0"         },
                  { L_,   "abc",         1,   16, "abc1"         },
                  { L_,   "abc",     32767,   16, "abc7FFF"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const TEXT = DATA[ti].d_text_p;
                const short VALUE      = DATA[ti].d_value;
                const int BASE         = DATA[ti].d_base;
                const char *const EXP  = DATA[ti].d_exp_p;

                buffer.clear();
                const bsl::string TEXTSTR = TEXT;
                buffer.insert(buffer.end(), TEXTSTR.begin(), TEXTSTR.end());

                if (veryVerbose) {
                    cout << "\t";
                    P_(TEXT);
                    P_(VALUE);
                    P_(EXP);
                }

                bdepu_TypesParserImpUtil::generateShort(&buffer, VALUE, BASE);

                if (veryVerbose) {
                    cout << "\tresult = ";
                    copy(buffer.begin(), buffer.end(),
                         ostream_iterator<char>(cout));
                    cout << endl;
                }

                LOOP_ASSERT(LINE, strlen(EXP) == buffer.size());
                LOOP_ASSERT(LINE,
                            0 == strncmp(EXP, &buffer.front(), buffer.size()));

                // Parse the data and verify that the value is recovered.
                buffer.push_back('\0');
                short result = 0;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                                     parseShort(&endPos,
                                                &result,
                                                &buffer.front() + strlen(TEXT),
                                                BASE);
                LOOP_ASSERT(LINE, 0 == rv);
                LOOP_ASSERT(LINE,
                            &buffer.front() + buffer.size() - 1 == endPos);
                LOOP_ASSERT(LINE, VALUE == result);
            }
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING GENERATE INT64
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateInt64(ArCh *buffer, int64 value, int base);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateInt64" << endl
                          << "=====================" << endl;

        {
            vector<char> buffer(&testAllocator);
            const char *UNFILLED =
 "???????????????????????????????????????????????????????????????????????????";

            static const struct {
                int                       d_lineNum;  // source line number
                const char               *d_text_p;   // specification string
                bsls_PlatformUtil::Int64  d_value;    // specification value
                int                       d_base;     // specification base
                const char               *d_exp_p;    // expected result string
            } DATA[] = {
                //line  text      value             base  exp
                //----  --------  -----------       ----  ----------------
                { L_,   "", -9223372036854775808LL, 2,
         "-1000000000000000000000000000000000000000000000000000000000000000" },
                { L_,   "", -9223372036854775807LL, 2,
          "-111111111111111111111111111111111111111111111111111111111111111" },
                { L_,   "",     -123456789012345LL, 2,
                          "-11100000100100010000110000011011101111101111001" },
                { L_,   "",          -2147483648LL, 2,
                                         "-10000000000000000000000000000000" },
                { L_,   "",                 -32768, 2,   "-1000000000000000" },
                { L_,   "",                     -1, 2,                  "-1" },
                { L_,   "",                      0, 2,                   "0" },
                { L_,   "",                      1, 2,                   "1" },
                { L_,   "",                  32767, 2,     "111111111111111" },
                { L_,   "",              123456789, 2,
                                               "111010110111100110100010101" },
                { L_,   "",             2147483647, 2,
                                          "1111111111111111111111111111111"  },
                { L_,   "",      123456789012345LL, 2,
                           "11100000100100010000110000011011101111101111001" },
                { L_,   "",  9223372036854775807LL, 2,
           "111111111111111111111111111111111111111111111111111111111111111" },

                { L_,   "", -9223372036854775808LL, 10,
                                                      "-9223372036854775808" },
                { L_,   "", -9223372036854775807LL, 10,
                                                      "-9223372036854775807" },
                { L_,   "",     -123456789012345LL, 10, "-123456789012345"   },
                { L_,   "",          -2147483648LL, 10, "-2147483648"        },
                { L_,   "",                 -32768, 10, "-32768"             },
                { L_,   "",                     -1, 10, "-1"                 },
                { L_,   "",                      0, 10, "0"                  },
                { L_,   "",                      1, 10, "1"                  },
                { L_,   "",                  32767, 10, "32767"              },
                { L_,   "",             2147483647, 10, "2147483647"         },
                { L_,   "",      123456789012345LL, 10, "123456789012345"    },
                { L_,   "",  9223372036854775807LL, 10,
                                                       "9223372036854775807" },

                { L_,   "", -9223372036854775808LL, 16, "-8000000000000000"  },
                { L_,   "",     -123456789012345LL, 16, "-7048860DDF79"      },
                { L_,   "",          -2147483648LL, 16, "-80000000"          },
                { L_,   "",                 -32768, 16, "-8000"              },
                { L_,   "",                     -1, 16, "-1"                 },
                { L_,   "",                      0, 16, "0"                  },
                { L_,   "",                      1, 16, "1"                  },
                { L_,   "",                  32767, 16, "7FFF"               },
                { L_,   "",             2147483647, 16, "7FFFFFFF"           },
                { L_,   "",      123456789012345LL, 16, "7048860DDF79"       },
                { L_,   "",  9223372036854775807LL, 16, "7FFFFFFFFFFFFFFF"   },

                { L_,   "", -9223372036854775808LL, 36, "-1Y2P0IJ32E8E8"     },
                { L_,   "", -9223372036854775807LL, 36, "-1Y2P0IJ32E8E7"     },
                { L_,   "",          -2147483648LL, 36, "-ZIK0ZK"            },
                { L_,   "",            -2147483647, 36, "-ZIK0ZJ"            },
                { L_,   "",            -1234567890, 36, "-KF12OI"            },
                { L_,   "",                 -32768, 36, "-PA8"               },
                { L_,   "",                 -32767, 36, "-PA7"               },
                { L_,   "",                     -1, 36, "-1"                 },
                { L_,   "",                      0, 36, "0"                  },
                { L_,   "",                      1, 36, "1"                  },
                { L_,   "",                  32767, 36, "PA7"                },
                { L_,   "",                  32768, 36, "PA8"                },
                { L_,   "",             1234567890, 36, "KF12OI"             },
                { L_,   "",             2147483647, 36, "ZIK0ZJ"             },
                { L_,   "",  9223372036854775807LL, 36, "1Y2P0IJ32E8E7"      },

                { L_,   "abc", -9223372036854775808LL, 2,
      "abc-1000000000000000000000000000000000000000000000000000000000000000" },
                { L_,   "abc",  -123456789012345LL, 2,
                       "abc-11100000100100010000110000011011101111101111001" },
                { L_,   "abc",       -2147483648LL, 2,
                                      "abc-10000000000000000000000000000000" },
                { L_,   "abc",          -123456789, 2,
                                           "abc-111010110111100110100010101" },
                { L_,   "abc",              -32768, 2,"abc-1000000000000000" },
                { L_,   "abc",                  -1, 2,               "abc-1" },
                { L_,   "abc",                   0, 2,                "abc0" },
                { L_,   "abc",                   1, 2,                "abc1" },
                { L_,   "abc",               32767, 2,  "abc111111111111111" },
                { L_,   "abc",           123456789, 2,
                                            "abc111010110111100110100010101" },
                { L_,   "abc",          2147483647, 2,
                                        "abc1111111111111111111111111111111" },
                { L_,   "abc", 9223372036854775807LL, 2,
        "abc111111111111111111111111111111111111111111111111111111111111111" },

                { L_,   "abc", -9223372036854775808LL, 10,
                                                   "abc-9223372036854775808" },
                { L_,   "abc",       -2147483648LL, 10, "abc-2147483648"     },
                { L_,   "abc",              -32768, 10, "abc-32768"          },
                { L_,   "abc",                  -1, 10, "abc-1"              },
                { L_,   "abc",                   0, 10, "abc0"               },
                { L_,   "abc",                   1, 10, "abc1"               },
                { L_,   "abc",               32767, 10, "abc32767"           },
                { L_,   "abc",          2147483647, 10, "abc2147483647"      },
                { L_,   "abc", 9223372036854775807LL, 10,
                                                    "abc9223372036854775807" },

                { L_,   "abc", -9223372036854775808LL, 16,
                                                      "abc-8000000000000000" },
                { L_,   "abc", -9223372036854775807LL, 16,
                                                      "abc-7FFFFFFFFFFFFFFF" },
                { L_,   "abc",       -2147483648LL, 16, "abc-80000000"       },
                { L_,   "abc",       -2147483647LL, 16, "abc-7FFFFFFF"       },
                { L_,   "abc",              -32768, 16, "abc-8000"           },
                { L_,   "abc",                  -1, 16, "abc-1"              },
                { L_,   "abc",                   0, 16, "abc0"               },
                { L_,   "abc",                   1, 16, "abc1"               },
                { L_,   "abc",               32767, 16, "abc7FFF"            },
                { L_,   "abc",          2147483647, 16, "abc7FFFFFFF"        },
                { L_,   "abc", 9223372036854775807LL, 16,
                                                       "abc7FFFFFFFFFFFFFFF" },

                { L_,   "abc", -9223372036854775808LL, 36,
                                                        "abc-1Y2P0IJ32E8E8"  },
                { L_,   "abc", -9223372036854775807LL, 36,
                                                        "abc-1Y2P0IJ32E8E7"  },
                { L_,   "abc",       -2147483648LL, 36, "abc-ZIK0ZK"         },
                { L_,   "abc",         -2147483647, 36, "abc-ZIK0ZJ"         },
                { L_,   "abc",         -1234567890, 36, "abc-KF12OI"         },
                { L_,   "abc",              -32768, 36, "abc-PA8"            },
                { L_,   "abc",              -32767, 36, "abc-PA7"            },
                { L_,   "abc",                  -1, 36, "abc-1"              },
                { L_,   "abc",                   0, 36, "abc0"               },
                { L_,   "abc",                   1, 36, "abc1"               },
                { L_,   "abc",               32767, 36, "abcPA7"             },
                { L_,   "abc",               32768, 36, "abcPA8"             },
                { L_,   "abc",          1234567890, 36, "abcKF12OI"          },
                { L_,   "abc",          2147483647, 36, "abcZIK0ZJ"          },
                { L_,   "abc", 9223372036854775807LL, 36,
                                                        "abc1Y2P0IJ32E8E7"   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE                       = DATA[ti].d_lineNum;
                const char *const TEXT               = DATA[ti].d_text_p;
                const bsls_PlatformUtil::Int64 VALUE = DATA[ti].d_value;
                const int BASE                       = DATA[ti].d_base;
                const char *const EXP                = DATA[ti].d_exp_p;

                const int PREFIX = strlen(TEXT);
                const int LEN    = strlen(EXP);

                buffer.clear();
                const bsl::string TEXTSTR = TEXT;
                buffer.insert(buffer.end(), TEXTSTR.begin(), TEXTSTR.end());

                if (veryVerbose) {
                    cout << "\t";
                    P_(TEXT);
                    P_(VALUE);
                    P_(BASE);
                    P(EXP);
                    cout << "\t";
                    P_(PREFIX);
                    P(LEN);
                    cout << "\tTesting generateInt64"
                         << " with '(vector<char> *) buffer'." << endl;
                }

                bdepu_TypesParserImpUtil::generateInt64(&buffer, VALUE, BASE);

                if (veryVerbose) {
                    cout << "\t\tresult = ";
                    copy(buffer.begin(), buffer.end(),
                         ostream_iterator<char>(cout));
                    cout << endl;
                }

                LOOP_ASSERT(LINE, LEN == buffer.size());
                LOOP_ASSERT(LINE,
                            0 == strncmp(EXP, &buffer.front(), buffer.size()));

                // parse the data and verify the value is recovered
                buffer.push_back('\0');
                bsls_PlatformUtil::Int64 result = 0;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                                     parseInt64(&endPos,
                                                &result,
                                                &buffer.front() + strlen(TEXT),
                                                BASE);
                LOOP_ASSERT(LINE, 0 == rv);
                LOOP_ASSERT(LINE,
                            &buffer.front() + buffer.size() - 1 == endPos);
                LOOP_ASSERT(LINE, VALUE == result);

                if (veryVerbose) {
                    cout << "\tTesting generateInt64 with '(char *) buffer'."
                         << endl;
                }

                for (int j = 0; j <= LEN + 2 - PREFIX; ++j) {
                    buffer.assign(TEXTSTR.begin(), TEXTSTR.end());
                    buffer.resize(buffer.size() + LEN + 2, UNFILLED[0]);

                    const int RET = bdepu_TypesParserImpUtil::generateInt64(
                                                             &(buffer[PREFIX]),
                                                             VALUE,
                                                             j,
                                                             BASE);

                    if (veryVerbose) {
                        cout << "\t\t";
                        P_(RET);
                        cout << "result = ";
                        copy(buffer.begin(), buffer.end(),
                                ostream_iterator<char>(cout));
                        cout << endl;
                    }

                    LOOP2_ASSERT(LINE, j, LEN == PREFIX + RET);
                    LOOP2_ASSERT(LINE, j, 0 == strncmp(EXP,
                                                   &buffer.front(),
                                                   bsl::min(PREFIX + j, LEN)));
                    if (j >= LEN - PREFIX + 1) {
                        LOOP2_ASSERT(LINE, j, '\0' == buffer[LEN]);
                        LOOP2_ASSERT(LINE, j, UNFILLED[0] == buffer[LEN + 1]);
                    } else {
                        LOOP2_ASSERT(LINE, j,
                                     0 == strncmp(UNFILLED,
                                                  &buffer.front() + PREFIX + j,
                                                  LEN + 2 - PREFIX - j));
                    }

                }

                if (verbose) {
                    cout << "\tTesting generateInt64Raw." << endl;
                }

                buffer.assign(TEXTSTR.begin(), TEXTSTR.end());
                buffer.resize(buffer.size() + LEN + 2, UNFILLED[0]);

                const int EXPRET = LEN - PREFIX;
                const int RET = bdepu_TypesParserImpUtil::generateInt64Raw(
                                                             &(buffer[PREFIX]),
                                                             VALUE,
                                                             BASE);

                if (veryVerbose) {
                    cout << "\t\t";
                    P_(EXPRET);
                    P_(RET);
                    cout << "result = ";
                    copy(buffer.begin(), buffer.end(),
                            ostream_iterator<char>(cout));
                    cout << endl;
                }
                LOOP_ASSERT(LINE, EXPRET == RET);
                LOOP_ASSERT(LINE, 0 == strncmp(EXP,
                                               &buffer.front(),
                                               PREFIX + EXPRET));
                LOOP_ASSERT(LINE, 0 == strncmp(
                                             UNFILLED,
                                             &buffer.front() + PREFIX + EXPRET,
                                             LEN + 2 - PREFIX - EXPRET));
            }
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING GENERATE INT
        //
        // Concerns:
        //   o That it handles INT_MIN properly.
        //   o That all digits are generated properly, for both signed and
        //     unsigned integers, up to the maximum width.
        //   o That it correctly handles bases 2, 10, and 16, as well as the
        //     maximum base 36.
        //
        // Plan:
        //   Apply the tabular method on inputs selected carefully
        //   to exhibit the concerns above, with both flavors of generateInt.
        //   Also use the perturbation method (with and without prefix) and all
        //   possible truncations for the 'generateInt(const char* ...)'
        //   flavor.
        //
        // Testing:
        //   generateInt(cchar *buffer, int value, int base);
        //   generateInt(ArCh *buffer, int value, int base);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateInt and generateIntRaw" << endl
                          << "======================================" << endl;

        {
            vector<char> buffer(&testAllocator);
            const char *UNFILLED = "?????????????????????????????????????????";

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_text_p;   // specification string for init.
                int         d_value;    // specification value
                int         d_base;     // specification base
                const char *d_exp_p;    // expected result string
            } DATA[] = {
                  //line  text      value        base  exp
                  //----  --------  -----------  ----  ----------------
                  { L_,   "",     -2147483648LL,   2,
                                         "-10000000000000000000000000000000" },
                  { L_,   "",       -2147483647,   2,
                                          "-1111111111111111111111111111111" },
                  { L_,   "",       -1234567890,   2,
                                          "-1001001100101100000001011010010" },
                  { L_,   "",            -32768,   2,    "-1000000000000000" },
                  { L_,   "",            -32767,   2,     "-111111111111111" },
                  { L_,   "",                -1,   2,                   "-1" },
                  { L_,   "",                 0,   2,                    "0" },
                  { L_,   "",                 1,   2,                    "1" },
                  { L_,   "",             32767,   2,      "111111111111111" },
                  { L_,   "",             32768,   2,     "1000000000000000" },
                  { L_,   "",        1234567890,   2,
                                           "1001001100101100000001011010010" },
                  { L_,   "",        2147483647,   2,
                                           "1111111111111111111111111111111" },

                  { L_,   "",     -2147483648LL,  10, "-2147483648"          },
                  { L_,   "",       -2147483647,  10, "-2147483647"          },
                  { L_,   "",       -1234567890,  10, "-1234567890"          },
                  { L_,   "",            -32768,  10, "-32768"               },
                  { L_,   "",                -1,  10, "-1"                   },
                  { L_,   "",                 0,  10, "0"                    },
                  { L_,   "",                 1,  10, "1"                    },
                  { L_,   "",             32767,  10, "32767"                },
                  { L_,   "",        1234567890,  10, "1234567890"           },
                  { L_,   "",        2147483647,  10, "2147483647"           },

                  { L_,   "",     -2147483648LL,  16, "-80000000"            },
                  { L_,   "",       -2147483647,  16, "-7FFFFFFF"            },
                  { L_,   "",        -162254319,  16, "-9ABCDEF"             },
                  { L_,   "",            -32768,  16, "-8000"                },
                  { L_,   "",                -1,  16, "-1"                   },
                  { L_,   "",                 0,  16, "0"                    },
                  { L_,   "",                 1,  16, "1"                    },
                  { L_,   "",             32767,  16, "7FFF"                 },
                  { L_,   "",         162254319,  16, "9ABCDEF"              },
                  { L_,   "",        2147483647,  16, "7FFFFFFF"             },

                  { L_,   "",     -2147483648LL,  36, "-ZIK0ZK"              },
                  { L_,   "",       -2147483647,  36, "-ZIK0ZJ"              },
                  { L_,   "",       -1234567890,  36, "-KF12OI"              },
                  { L_,   "",            -32768,  36, "-PA8"                 },
                  { L_,   "",            -32767,  36, "-PA7"                 },
                  { L_,   "",                -1,  36, "-1"                   },
                  { L_,   "",                 0,  36, "0"                    },
                  { L_,   "",                 1,  36, "1"                    },
                  { L_,   "",             32767,  36, "PA7"                  },
                  { L_,   "",             32768,  36, "PA8"                  },
                  { L_,   "",        1234567890,  36, "KF12OI"               },
                  { L_,   "",        2147483647,  36, "ZIK0ZJ"               },

                  { L_,   "abc",  -2147483648LL,   2,
                                      "abc-10000000000000000000000000000000" },
                  { L_,   "abc",    -2147483647,   2,
                                       "abc-1111111111111111111111111111111" },
                  { L_,   "abc",    -1234567890,   2,
                                       "abc-1001001100101100000001011010010" },
                  { L_,   "abc",         -32768,   2, "abc-1000000000000000" },
                  { L_,   "abc",             -1,   2,                "abc-1" },
                  { L_,   "abc",              0,   2,                 "abc0" },
                  { L_,   "abc",              1,   2,                 "abc1" },
                  { L_,   "abc",          32767,   2,   "abc111111111111111" },
                  { L_,   "abc",     1234567890,   2,
                                        "abc1001001100101100000001011010010" },
                  { L_,   "abc",     2147483647,   2,
                                        "abc1111111111111111111111111111111" },

                  { L_,   "abc",  -2147483648LL,  10, "abc-2147483648"       },
                  { L_,   "abc",    -2147483647,  10, "abc-2147483647"       },
                  { L_,   "abc",    -1234567890,  10, "abc-1234567890"       },
                  { L_,   "abc",         -32768,  10, "abc-32768"            },
                  { L_,   "abc",             -1,  10, "abc-1"                },
                  { L_,   "abc",              0,  10, "abc0"                 },
                  { L_,   "abc",              1,  10, "abc1"                 },
                  { L_,   "abc",          32767,  10, "abc32767"             },
                  { L_,   "abc",     1234567890,  10, "abc1234567890"        },
                  { L_,   "abc",     2147483647,  10, "abc2147483647"        },

                  { L_,   "abc",  -2147483648LL,  16, "abc-80000000"         },
                  { L_,   "abc",    -2147483647,  16, "abc-7FFFFFFF"         },
                  { L_,   "abc",     -162254319,  16, "abc-9ABCDEF"         },
                  { L_,   "abc",         -32768,  16, "abc-8000"             },
                  { L_,   "abc",             -1,  16, "abc-1"                },
                  { L_,   "abc",              0,  16, "abc0"                 },
                  { L_,   "abc",              1,  16, "abc1"                 },
                  { L_,   "abc",          32767,  16, "abc7FFF"              },
                  { L_,   "abc",      162254319,  16, "abc9ABCDEF"           },
                  { L_,   "abc",     2147483647,  16, "abc7FFFFFFF"          },

                  { L_,   "abc",  -2147483648LL,  36, "abc-ZIK0ZK"           },
                  { L_,   "abc",    -2147483647,  36, "abc-ZIK0ZJ"           },
                  { L_,   "abc",    -1234567890,  36, "abc-KF12OI"           },
                  { L_,   "abc",         -32768,  36, "abc-PA8"              },
                  { L_,   "abc",         -32767,  36, "abc-PA7"              },
                  { L_,   "abc",             -1,  36, "abc-1"                },
                  { L_,   "abc",              0,  36, "abc0"                 },
                  { L_,   "abc",              1,  36, "abc1"                 },
                  { L_,   "abc",          32767,  36, "abcPA7"               },
                  { L_,   "abc",          32768,  36, "abcPA8"               },
                  { L_,   "abc",     1234567890,  36, "abcKF12OI"            },
                  { L_,   "abc",     2147483647,  36, "abcZIK0ZJ"            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const TEXT = DATA[ti].d_text_p;
                const int VALUE        = DATA[ti].d_value;
                const int BASE         = DATA[ti].d_base;
                const char *const EXP  = DATA[ti].d_exp_p;

                const int PREFIX = strlen(TEXT);
                const int LEN    = strlen(EXP);

                buffer.clear();
                const bsl::string TEXTSTR = TEXT;
                buffer.insert(buffer.end(), TEXTSTR.begin(), TEXTSTR.end());

                if (veryVerbose) {
                    cout << "\t";
                    P_(TEXT);
                    P_(VALUE);
                    P_(BASE);
                    P(EXP);
                    cout << "\t";
                    P_(PREFIX);
                    P(LEN);
                    cout << "\tTesting generateInt"
                         << " with '(vector<char> *) buffer'." << endl;
                }

                bdepu_TypesParserImpUtil::generateInt(&buffer, VALUE, BASE);

                if (veryVerbose) {
                    cout << "\t\tresult = ";
                    copy(buffer.begin(), buffer.end(),
                         ostream_iterator<char>(cout));
                    cout << endl;
                }

                LOOP_ASSERT(LINE, LEN == buffer.size());
                LOOP_ASSERT(LINE,
                            0 == strncmp(EXP, &buffer.front(), buffer.size()));

                // parse the data and verify the value is recovered
                buffer.push_back('\0');
                int result = 0;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                                       parseInt(&endPos,
                                                &result,
                                                &buffer.front() + strlen(TEXT),
                                                BASE);
                LOOP_ASSERT(LINE, 0 == rv);
                LOOP_ASSERT(LINE,
                            &buffer.front() + buffer.size() - 1 == endPos);
                LOOP_ASSERT(LINE, VALUE == result);

                if (veryVerbose) {
                    cout << "\tTesting generateInt with '(char *) buffer'."
                         << endl;
                }

                for (int j = 0; j <= LEN + 2 - PREFIX; ++j) {
                    buffer.assign(TEXTSTR.begin(), TEXTSTR.end());
                    buffer.resize(buffer.size() + LEN + 2, UNFILLED[0]);

                    const int RET = bdepu_TypesParserImpUtil::generateInt(
                                                             &(buffer[PREFIX]),
                                                             VALUE,
                                                             j,
                                                             BASE);

                    if (veryVerbose) {
                        cout << "\t\t";
                        P_(RET);
                        cout << "result = ";
                        copy(buffer.begin(), buffer.end(),
                                ostream_iterator<char>(cout));
                        cout << endl;
                    }

                    LOOP2_ASSERT(LINE, j, LEN == PREFIX + RET);
                    LOOP2_ASSERT(LINE, j, 0 == strncmp(EXP,
                                                   &buffer.front(),
                                                   bsl::min(PREFIX + j, LEN)));
                    if (j >= LEN - PREFIX + 1) {
                        LOOP2_ASSERT(LINE, j, '\0' == buffer[LEN]);
                        LOOP2_ASSERT(LINE, j, UNFILLED[0] == buffer[LEN + 1]);
                    } else {
                        LOOP2_ASSERT(LINE, j,
                                     0 == strncmp(UNFILLED,
                                                  &buffer.front() + PREFIX + j,
                                                  LEN + 2 - PREFIX - j));
                    }

                }

                if (verbose) {
                    cout << "\tTesting generateIntRaw." << endl;
                }

                buffer.assign(TEXTSTR.begin(), TEXTSTR.end());
                buffer.resize(buffer.size() + LEN + 2, UNFILLED[0]);

                const int EXPRET = LEN - PREFIX;
                const int RET = bdepu_TypesParserImpUtil::generateIntRaw(
                                                             &(buffer[PREFIX]),
                                                             VALUE,
                                                             BASE);

                if (veryVerbose) {
                    cout << "\t\t";
                    P_(EXPRET);
                    P_(RET);
                    cout << "result = ";
                    copy(buffer.begin(), buffer.end(),
                            ostream_iterator<char>(cout));
                    cout << endl;
                }
                LOOP_ASSERT(LINE, EXPRET == RET);
                LOOP_ASSERT(LINE, 0 == strncmp(EXP,
                                               &buffer.front(),
                                               PREFIX + EXPRET));
                LOOP_ASSERT(LINE, 0 == strncmp(
                                             UNFILLED,
                                             &buffer.front() + PREFIX + EXPRET,
                                             LEN + 2 - PREFIX - EXPRET));
            }
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING GENERATE FLOAT
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateFloat(ArCh *buffer, float value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateFloat" << endl
                          << "=====================" << endl;

        {
            vector<char> buffer(&testAllocator);

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_text_p;   // input string
                float       d_value;    // specification value
                const char *d_exp_p;    // expected string
            } DATA[] = {
                //line  text   value                  exp
                //----  ----   --------------------   --------------
                { L_,   "",    0.0F,                  "0.0"             },
                { L_,   "",    0.1F,                  "0.1"             },
                { L_,   "",    0.01F,                 "0.01"            },
                { L_,   "",    0.001F,                "1e-3"            },
                { L_,   "",    0.0001F,               "1e-4"            },
                { L_,   "",    0.00001F,              "1e-5"            },
                { L_,   "",    0.000001F,             "1e-6"            },
                { L_,   "",    1.0F,                  "1"               },
                { L_,   "",    10.0F,                 "10"              },
                { L_,   "",    100.0F,                "100"             },
                { L_,   "",    1000.0F,               "1e3"             },
                { L_,   "",    10000.0F,              "1e4"             },
                { L_,   "",    100000.0F,             "1e5"             },
                { L_,   "",    1000000.0F,            "1e6"             },
                { L_,   "",    0.0012345F,            "1.2345e-3"       },
                { L_,   "",    0.012345F,             "0.012345"        },
                { L_,   "",    0.12345F,              "0.12345"         },
                { L_,   "",    1.2345F,               "1.2345"          },
                { L_,   "",    12.345F,               "12.345"          },
                { L_,   "",    123.45F,               "123.45"          },
                { L_,   "",    1234.5F,               "1.2345e3"        },
                { L_,   "",    12345.0F,              "1.2345e4"        },

                // For generateFloat(char *, ...) only
                // { L_,   "",    1234567890.12345F,     "1.23457e9"       },
                // { L_,   "",    .123456789012345E30F,  "1.23457e29"      },
                // { L_,   "",    .123456789012345E-30F, "1.23457e-31"     },

                { L_,   "abc", 0.0F,                  "abc0.0"          },
                { L_,   "abc", 0.1F,                  "abc0.1"          },
                { L_,   "abc", 0.01F,                 "abc0.01"         },
                { L_,   "abc", 0.001F,                "abc1e-3"         },
                { L_,   "abc", 0.0001F,               "abc1e-4"         },
                { L_,   "abc", 0.00001F,              "abc1e-5"         },
                { L_,   "abc", 0.000001F,             "abc1e-6"         },
                { L_,   "abc", 1.0F,                  "abc1"            },
                { L_,   "abc", 10.0F,                 "abc10"           },
                { L_,   "abc", 100.0F,                "abc100"          },
                { L_,   "abc", 1000.0F,               "abc1e3"          },
                { L_,   "abc", 10000.0F,              "abc1e4"          },
                { L_,   "abc", 100000.0F,             "abc1e5"          },
                { L_,   "abc", 1000000.0F,            "abc1e6"          },
                { L_,   "abc", 0.0012345F,            "abc1.2345e-3"    },
                { L_,   "abc", 0.012345F,             "abc0.012345"     },
                { L_,   "abc", 0.12345F,              "abc0.12345"      },
                { L_,   "abc", 1.2345F,               "abc1.2345"       },
                { L_,   "abc", 12.345F,               "abc12.345"       },
                { L_,   "abc", 123.45F,               "abc123.45"       },
                { L_,   "abc", 1234.5F,               "abc1.2345e3"     },
                { L_,   "abc", 12345.0F,              "abc1.2345e4"     },

                // For generateFloat(char *, ...) only
                // { L_,   "abc", 1234567890.12345F,     "abc1.23457e9"    },
                // { L_,   "abc", .123456789012345E30F,  "abc1.23457e29"   },
                // { L_,   "abc", .123456789012345E-30F, "abc1.23457e-31"  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const TEXT  = DATA[ti].d_text_p;
                const float       VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_exp_p;

                buffer.clear();
                const bsl::string TEXTSTR = TEXT;
                buffer.insert(buffer.end(), TEXTSTR.begin(), TEXTSTR.end());

                if (veryVerbose) {
                    cout << "\t";
                    P_(TEXT);
                    P_(VALUE);
                    P(EXP);
                }

                bdepu_TypesParserImpUtil::generateFloat(&buffer, VALUE);

                if (veryVerbose) {
                    cout << "\tresult = ";
                    copy(buffer.begin(), buffer.end(),
                         ostream_iterator<char>(cout));
                    cout << endl;
                }

                LOOP_ASSERT(LINE, strlen(EXP) == buffer.size());
                LOOP_ASSERT(LINE,
                            0 == strncmp(EXP, &buffer.front(), buffer.size()));

                // Parse the data and verify that the value is recovered.
                buffer.push_back('\0');
                float result;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                                    parseFloat(&endPos,
                                               &result,
                                               &buffer.front() + strlen(TEXT));
                LOOP_ASSERT(LINE, 0 == rv);
                LOOP_ASSERT(LINE,
                            &buffer.front() + buffer.size() - 1 == endPos);
                LOOP_ASSERT(LINE, VALUE == result);
            }
        }

        if (verbose) {
            cout << endl
                 << "TESTING generateFloat and generateFloatRaw" << endl
                 << "==========================================" << endl;
        }

        bslma_DefaultAllocatorGuard defaultAllocGuard(&testAllocator);
        {
            vector<char> buffer(&testAllocator);
            const char *UNFILLED = "?????????????????????????????????????????";

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define ZERO   "0"
#else
#define ZERO
#endif

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_text_p;   // input string
                float       d_value;    // specification value
                const char *d_exp_p;    // expected string
            } DATA[] = {
                //line  text    value                 exp
                //----  ------  --------------------  --------------
                { L_,   "abc",  0.0F,                 "abc0"                 },
                { L_,   "abc",  0.1F,                 "abc0.1"               },
                { L_,   "abc",  0.01F,                "abc0.01"              },
                { L_,   "abc",  0.001F,               "abc0.001"             },
#ifndef BSLS_PLATFORM__OS_LINUX
    // On Linux, for some reason, the statement 'printf("%.6g", 0.0001)' prints
    // 1e-04.
                { L_,   "abc",  0.0001F,              "abc0.0001"            },
#endif
                { L_,   "abc",  0.00001F,             "abc1e-" ZERO "05"     },
                { L_,   "abc",  0.000001F,            "abc1e-" ZERO "06"     },
                { L_,   "abc",  1.0F,                 "abc1"                 },
                { L_,   "abc",  10.0F,                "abc10"                },
                { L_,   "abc",  100.0F,               "abc100"               },
                { L_,   "abc",  1000.0F,              "abc1000"              },
                { L_,   "abc",  10000.0F,             "abc10000"             },
                { L_,   "abc",  100000.0F,            "abc100000"            },
                { L_,   "abc",  1000000.0F,           "abc1e+" ZERO "06"     },
                { L_,   "abc",  0.0012345F,           "abc0.0012345"         },
                { L_,   "abc",  0.012345F,            "abc0.012345"          },
                { L_,   "abc",  0.12345F,             "abc0.12345"           },
                { L_,   "abc",  1.2345F,              "abc1.2345"            },
                { L_,   "abc",  12.345F,              "abc12.345"            },
                { L_,   "abc",  123.45F,              "abc123.45"            },
                { L_,   "abc",  1234.5F,              "abc1234.5"            },
                { L_,   "abc",  12345.0F,             "abc12345"             },

                { L_,   "abc",  1234567890.12345F,
                                                    "abc1.23457e+" ZERO "09" },
                { L_,   "abc",  .123456789012345E30F,
                                                    "abc1.23457e+" ZERO "29" },
                { L_,   "abc",  .123456789012345E-30F,
                                                    "abc1.23457e-" ZERO "31" }
            };
#undef ZERO
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const TEXT  = DATA[ti].d_text_p;
                const float       VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_exp_p;

                const int PREFIX = strlen(TEXT);
                const int LEN    = strlen(EXP);

                if (veryVerbose) {
                    cout << "\tTesting generateFloat with '(char *) buffer'."
                         << endl;
                }

                for (int j = 0; j <= LEN + 5 - PREFIX; ++j) {
                    buffer.assign(TEXT, TEXT + PREFIX);
                    buffer.resize(LEN + 5, UNFILLED[0]);

                    if (veryVerbose) {
                        cout << "\t";
                        P_(TEXT);
                        P_(VALUE);
                        P(EXP);
                    }

                    int numAllocation = testAllocator.numAllocations();
                    const int RET = bdepu_TypesParserImpUtil::generateFloat(
                                                             &(buffer[PREFIX]),
                                                             VALUE,
                                                             j);
                    LOOP_ASSERT(LINE,
                             numAllocation == testAllocator.numAllocations());

                    if (veryVerbose) {
                        cout << "\tBDEPU_MAX_FLOAT_STRLEN10 = "
                         << bdepu_TypesParserImpUtil::BDEPU_MAX_FLOAT_STRLEN10
                         << ", result = ";
                        copy(buffer.begin(), buffer.end(),
                             ostream_iterator<char>(cout));
                        cout << endl;
                    }

                    LOOP2_ASSERT(LINE, j, LEN == PREFIX + RET);
                    LOOP2_ASSERT(LINE, j,
                                 0 == strncmp(EXP,
                                              &buffer[0],
                                              bsl::min(PREFIX + j, LEN)));
                    if (LEN < PREFIX + j) {
                        LOOP2_ASSERT(LINE, j, '\0' == buffer[LEN]);
                        LOOP2_ASSERT(LINE, j, UNFILLED[0] == buffer[LEN + 1]);
                    } else {
                        LOOP2_ASSERT(LINE, j,
                                     0 == strncmp(UNFILLED,
                                                  &buffer[0] + PREFIX + j,
                                                  LEN + 5 - PREFIX - j));
                    }

                }

                if (verbose) {
                    cout << "\tTesting generateFloatRaw." << endl;
                }

                buffer.assign(TEXT, TEXT + PREFIX);
                buffer.resize(PREFIX + LEN + 5, UNFILLED[0]);

                const int EXPRET = LEN - PREFIX;
                const int RET = bdepu_TypesParserImpUtil::generateFloatRaw(
                                                             &(buffer[PREFIX]),
                                                             VALUE);

                if (veryVerbose) {
                    cout << "\t";
                    P_(EXPRET);
                    P_(RET);
                    cout << "result = ";
                    copy(buffer.begin(), buffer.end(),
                            ostream_iterator<char>(cout));
                    cout << endl;
                }
                LOOP_ASSERT(LINE, LEN == PREFIX + RET);
                LOOP_ASSERT(LINE, 0 == strncmp(EXP, &buffer[0], LEN));
                LOOP_ASSERT(LINE, 0 == strncmp(UNFILLED,
                                               &buffer[0] + LEN + 1,
                                               4));
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DOUBLE
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateDouble(ArCh *buffer, double value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDouble" << endl
                          << "======================" << endl;

        {
            vector<char> buffer(&testAllocator);

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_text_p;   // input string
                double      d_value;    // specification value
                const char *d_exp_p;    // expected string
            } DATA[] = {
                //line  text      value                exp
                //----  --------  ---------            --------------
                { L_,   "",       0.0,                 "0.0"             },
                { L_,   "",       0.1,                 "0.1"             },
                { L_,   "",       0.01,                "0.01"            },
                { L_,   "",       0.001,               "1e-3"            },
                { L_,   "",       0.0001,              "1e-4"            },
                { L_,   "",       0.00001,             "1e-5"            },
                { L_,   "",       0.000001,            "1e-6"            },
                { L_,   "",       1.0,                 "1"               },
                { L_,   "",       10.0,                "10"              },
                { L_,   "",       100.0,               "100"             },
                { L_,   "",       1000.0,              "1e3"             },
                { L_,   "",       10000.0,             "1e4"             },
                { L_,   "",       100000.0,            "1e5"             },
                { L_,   "",       1000000.0,           "1e6"             },
                { L_,   "",       0.0012345,           "1.2345e-3"       },
                { L_,   "",       0.012345,            "0.012345"        },
                { L_,   "",       0.12345,             "0.12345"         },
                { L_,   "",       1.2345,              "1.2345"          },
                { L_,   "",       12.345,              "12.345"          },
                { L_,   "",       123.45,              "123.45"          },
                { L_,   "",       1234.5,              "1.2345e3"        },
                { L_,   "",       12345.0,             "1.2345e4"        },
                // For generateDouble(char *, ...) only
                // { L_,   "",       1234567890.1234567890123456789,
                //                                        "1.23457e9"       },
                // { L_,   "",       .12345678901234567890123456789E300,
                //                                        "1.23457e299"     },
                // { L_,   "",       .12345678901234567890123456789E-300,
                //                                        "1.23457e-301"    },

                { L_,   "abc",    0.0,                 "abc0.0"          },
                { L_,   "abc",    0.1,                 "abc0.1"          },
                { L_,   "abc",    0.01,                "abc0.01"         },
                { L_,   "abc",    0.001,               "abc1e-3"         },
                { L_,   "abc",    0.0001,              "abc1e-4"         },
                { L_,   "abc",    0.00001,             "abc1e-5"         },
                { L_,   "abc",    0.000001,            "abc1e-6"         },
                { L_,   "abc",    1.0,                 "abc1"            },
                { L_,   "abc",    10.0,                "abc10"           },
                { L_,   "abc",    100.0,               "abc100"          },
                { L_,   "abc",    1000.0,              "abc1e3"          },
                { L_,   "abc",    10000.0,             "abc1e4"          },
                { L_,   "abc",    100000.0,            "abc1e5"          },
                { L_,   "abc",    1000000.0,           "abc1e6"          },
                { L_,   "abc",    0.0012345,           "abc1.2345e-3"    },
                { L_,   "abc",    0.012345,            "abc0.012345"     },
                { L_,   "abc",    0.12345,             "abc0.12345"      },
                { L_,   "abc",    1.2345,              "abc1.2345"       },
                { L_,   "abc",    12.345,              "abc12.345"       },
                { L_,   "abc",    123.45,              "abc123.45"       },
                { L_,   "abc",    1234.5,              "abc1.2345e3"     },
                { L_,   "abc",    12345.0,             "abc1.2345e4"     },

                // For generateDouble(char *, ...) only
                // { L_,   "abc",    1234567890.1234567890123456789,
                //                                     "abc1.23457e9"    },
                // { L_,   "abc",    .12345678901234567890123456789E300,
                //                                     "abc1.23457e299"  },
                // { L_,   "abc",    .12345678901234567890123456789E-300,
                //                                     "abc1.23457e-301" }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const TEXT  = DATA[ti].d_text_p;
                const double      VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_exp_p;

                const int PREFIX = strlen(TEXT);
                const int LEN    = strlen(EXP);

                buffer.assign(TEXT, TEXT + PREFIX);

                if (veryVerbose) {
                    cout << "\t";
                    P_(TEXT);
                    P_(VALUE);
                    P(EXP);
                }

                bdepu_TypesParserImpUtil::generateDouble(&buffer, VALUE);

                if (veryVerbose) {
                    cout << "\tresult = ";
                    copy(buffer.begin(), buffer.end(),
                         ostream_iterator<char>(cout));
                    cout << endl;
                }

                LOOP_ASSERT(LINE, LEN == buffer.size());
                LOOP_ASSERT(LINE, 0 == strncmp(EXP,
                                               &buffer[0], buffer.size()));

                // Parse the data and verify that the value is recovered.
                buffer.push_back('\0');
                double result;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                                              parseDouble(&endPos,
                                                          &result,
                                                          &buffer[0] + PREFIX);
                LOOP_ASSERT(LINE, 0 == rv);
                LOOP_ASSERT(LINE, &buffer[0] + buffer.size() - 1 == endPos);
                LOOP_ASSERT(LINE, VALUE == result);
            }
        }

        if (verbose) {
            cout << endl
                 << "TESTING generateDouble and generateDoubleRaw" << endl
                 << "============================================" << endl;
        }

        bslma_DefaultAllocatorGuard defaultAllocGuard(&testAllocator);
        {
            vector<char> buffer(&testAllocator);
            const char *UNFILLED = "?????????????????????????????????????????";

#if defined(BSLS_PLATFORM__CMP_MSVC)
#define ZERO   "0"
#else
#define ZERO
#endif

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_text_p;   // input string
                double      d_value;    // specification value
                const char *d_exp_p;    // expected string
            } DATA[] = {
                //line  text    value                exp
                //----  ------  ---------            --------------
                { L_,   "abc",  0.0,                 "abc0"                  },
                { L_,   "abc",  0.1,                 "abc0.1"                },
                { L_,   "abc",  0.01,                "abc0.01"               },
                { L_,   "abc",  0.001,               "abc0.001"              },
                { L_,   "abc",  0.0001,              "abc0.0001"             },
                { L_,   "abc",  0.00001,             "abc1e-" ZERO "05"      },
                { L_,   "abc",  0.000001,            "abc1e-" ZERO "06"      },
                { L_,   "abc",  1.0,                 "abc1"                  },
                { L_,   "abc",  10.0,                "abc10"                 },
                { L_,   "abc",  100.0,               "abc100"                },
                { L_,   "abc",  1000.0,              "abc1000"               },
                { L_,   "abc",  10000.0,             "abc10000"              },
                { L_,   "abc",  100000.0,            "abc100000"             },
                { L_,   "abc",  1000000.0,           "abc1e+" ZERO "06"      },
                { L_,   "abc",  0.0012345,           "abc0.0012345"          },
                { L_,   "abc",  0.012345,            "abc0.012345"           },
                { L_,   "abc",  0.12345,             "abc0.12345"            },
                { L_,   "abc",  1.2345,              "abc1.2345"             },
                { L_,   "abc",  12.345,              "abc12.345"             },
                { L_,   "abc",  123.45,              "abc123.45"             },
                { L_,   "abc",  1234.5,              "abc1234.5"             },
                { L_,   "abc",  12345.0,             "abc12345"              },

                { L_,   "abc",  1234567890.1234567890123456789,
                                                    "abc1.23457e+" ZERO "09" },
                { L_,   "abc",  .12345678901234567890123456789E300,
                                                     "abc1.23457e+299"       },
                { L_,   "abc",  .12345678901234567890123456789E-300,
                                                     "abc1.23457e-301"       }
            };
#undef ZERO
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const char *const TEXT  = DATA[ti].d_text_p;
                const double      VALUE = DATA[ti].d_value;
                const char *const EXP   = DATA[ti].d_exp_p;

                const int PREFIX = strlen(TEXT);
                const int LEN    = strlen(EXP);

                if (veryVerbose) {
                    cout << "\tTesting generateDouble with '(char *) buffer'."
                         << endl;
                }

                for (int j = 0; j <= LEN + 5 - PREFIX; ++j) {
                    buffer.assign(TEXT, TEXT + PREFIX);
                    buffer.resize(LEN + 5, UNFILLED[0]);

                    if (veryVerbose) {
                        cout << "\t";
                        P_(TEXT);
                        P_(VALUE);
                        P(EXP);
                    }

                    int numAllocation = testAllocator.numAllocations();
                    const int RET = bdepu_TypesParserImpUtil::generateDouble(
                                                             &(buffer[PREFIX]),
                                                             VALUE,
                                                             j);
                    LOOP_ASSERT(LINE,
                             numAllocation == testAllocator.numAllocations());

                    if (veryVerbose) {
                        cout << "\tBDEPU_MAX_DOUBLE_STRLEN10 = "
                         << bdepu_TypesParserImpUtil::BDEPU_MAX_DOUBLE_STRLEN10
                         << ", result = ";
                        copy(buffer.begin(), buffer.end(),
                             ostream_iterator<char>(cout));
                        cout << endl;
                    }

                    LOOP2_ASSERT(LINE, j, LEN == PREFIX + RET);
                    LOOP2_ASSERT(LINE, j,
                                 0 == strncmp(EXP,
                                              &buffer[0],
                                              bsl::min(PREFIX + j, LEN)));
                    if (LEN < PREFIX + j) {
                        LOOP2_ASSERT(LINE, j, '\0' == buffer[LEN]);
                        LOOP2_ASSERT(LINE, j, UNFILLED[0] == buffer[LEN + 1]);
                    } else {
                        LOOP2_ASSERT(LINE, j,
                                     0 == strncmp(UNFILLED,
                                                  &buffer[0] + PREFIX + j,
                                                  LEN + 5 - PREFIX - j));
                    }

                }

                if (verbose) {
                    cout << "\tTesting generateDoubleRaw." << endl;
                }

                buffer.assign(TEXT, TEXT + PREFIX);
                buffer.resize(PREFIX + LEN + 5, UNFILLED[0]);

                const int EXPRET = LEN - PREFIX;
                const int RET = bdepu_TypesParserImpUtil::generateDoubleRaw(
                                                             &(buffer[PREFIX]),
                                                             VALUE);

                if (veryVerbose) {
                    cout << "\t";
                    P_(EXPRET);
                    P_(RET);
                    cout << "result = ";
                    copy(buffer.begin(), buffer.end(),
                            ostream_iterator<char>(cout));
                    cout << endl;
                }
                LOOP_ASSERT(LINE, LEN == PREFIX + RET);
                LOOP_ASSERT(LINE, 0 == strncmp(EXP, &buffer[0], LEN));
                LOOP_ASSERT(LINE, 0 == strncmp(UNFILLED,
                                               &buffer[0] + LEN + 1,
                                               4));
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DATE
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateDate(ArCh *buffer, bDate value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDate" << endl
                          << "====================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_spec_p;          // specification string
                int         d_year;            // specification year
                int         d_month;           // specification month
                int         d_day;             // specification day
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  spec   year  month  day  exp
                //----  -----  ----  -----  ---  -----------------------
                { L_,   "",       1,     2,   3, "0001/02/03"            },
                { L_,   "",      10,     2,   3, "0010/02/03"            },
                { L_,   "",     100,     2,   3, "0100/02/03"            },
                { L_,   "",    1000,     2,   3, "1000/02/03"            },
                { L_,   "",    9999,     2,   3, "9999/02/03"            },
                { L_,   "",    9999,    12,  31, "9999/12/31"            },

                { L_,   "x",      1,     2,   3, "x0001/02/03"           },
                { L_,   "x",     10,     2,   3, "x0010/02/03"           },
                { L_,   "x",    100,     2,   3, "x0100/02/03"           },
                { L_,   "x",   1000,     2,   3, "x1000/02/03"           },
                { L_,   "x",   9999,     2,   3, "x9999/02/03"           },
                { L_,   "x",   9999,    12,  31, "x9999/12/31"           },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const int YEAR          = DATA[ti].d_year;
                const int MONTH         = DATA[ti].d_month;
                const int DAY           = DATA[ti].d_day;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(SPEC);
                const bdet_Date VALUE(YEAR, MONTH, DAY);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_TypesParserImpUtil::generateDate(&buffer, VALUE);
                buffer.push_back(0);
                const bsl::string EXPSTR = EXP;
                vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                exp.push_back(0);
                LOOP_ASSERT(LINE, exp == buffer);

                if (0 == curLen) {
                    bdet_Date result;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                  parseDate(&endPos, &result, &buffer.front());
                    LOOP_ASSERT(LINE, 0 == rv);
                    LOOP_ASSERT(LINE, VALUE == result);
                }
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING GENERATE CHAR
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateChar(ArCh *buffer, char value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateChar" << endl
                          << "====================" << endl;

        {
            vector<char> buffer(&testAllocator);
            for (int i = 0; i < 256; ++i) {
                buffer.clear();
                bdepu_TypesParserImpUtil::generateChar(&buffer, (char)i);
                char result;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                                  parseChar(&endPos, &result, &buffer.front());
                LOOP_ASSERT(i, 0 == rv);
                LOOP_ASSERT(i, result == (char)i);

                buffer.clear();
                buffer.resize(3);
                bdepu_TypesParserImpUtil::generateChar(&buffer, (char)i);
                rv = bdepu_TypesParserImpUtil::
                              parseChar(&endPos, &result, &buffer.front() + 3);
                LOOP_ASSERT(i, 0 == rv);
                LOOP_ASSERT(i, result == (char)i);
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING GENERATE CHAR RAW
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateCharRaw(ArCh *buffer, char value);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateCharRaw" << endl
                          << "=======================" << endl;

        {
            vector<char> buffer(&testAllocator);
            for (int i = 1; i < 256; ++i) { // generateCharRaw(b, 0) undefined
                buffer.clear();
                bdepu_TypesParserImpUtil::generateCharRaw(&buffer, (char)i);
                buffer.push_back(0);
                char result;
                const char *endPos = 0;
                int rv = bdepu_TypesParserImpUtil::
                               parseCharRaw(&endPos, &result, &buffer.front());
                LOOP_ASSERT(i, 0 == rv);
                LOOP_ASSERT(i, result == (char)i);

                buffer.clear();
                buffer.resize(3);
                bdepu_TypesParserImpUtil::generateCharRaw(&buffer, (char)i);
                buffer.push_back(0);
                rv = bdepu_TypesParserImpUtil::
                           parseCharRaw(&endPos, &result, &buffer.front() + 3);
                LOOP_ASSERT(i, 0 == rv);
                LOOP_ASSERT(i, result == (char)i);
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING PARSE STRING
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseString(cchar **endPos, bStr *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseString" << endl
                          << "===================" << endl;

        {
            const char *INITIAL_VALUE_1 = "-3";  // first initial value
            const char *INITIAL_VALUE_2 = " 9";  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                const char *d_value_p;  // expected return value
            } DATA[] = {
                //line  spec                 offset  fail  value
                //----  -------------------  ------  ----  --------------
                { L_,   "",                       0,    1,                },

                { L_,   " ",                      0,    1,                },
                { L_,   "\"",                     1,    1,                },
                { L_,   "a",                      1,    0, "a"            },
                { L_,   "b",                      1,    0, "b"            },

                { L_,   "a ",                     1,    0, "a"            },
                { L_,   "a\"",                    1,    0, "a"            },
                { L_,   "\\\"",                   2,    0, "\""           },
                { L_,   "ab",                     2,    0, "ab"           },
                { L_,   "\"a",                    2,    1,                },

                { L_,   "abc",                    3,    0, "abc"          },
                { L_,   "\"a\"",                  3,    0,  "a"           },

                { L_,   "\"\\\"\"",               4,    0,  "\""          },

                // TBD more vectors
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char *VALUE      = DATA[ti].d_value_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::string result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                           parseString(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    bsl::string result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                           parseString(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING PARSE UNQUOTED STRING
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseUnquotedString(cchar **endPos, bStr *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseUnquotedString" << endl
                          << "===========================" << endl;

        {
            const char *INITIAL_VALUE_1 = "-3";  // first initial value
            const char *INITIAL_VALUE_2 = " 9";  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                const char *d_value_p;  // expected return value
            } DATA[] = {
                //line  spec                 offset  fail  value
                //----  -------------------  ------  ----  --------------
                { L_,   "",                       0,    1,                },

                { L_,   " ",                      0,    1,                },
                { L_,   "\"",                     0,    1,                },
                { L_,   "a",                      1,    0, "a"            },
                { L_,   "b",                      1,    0, "b"            },

                { L_,   "a ",                     1,    0, "a"            },
                { L_,   "a\"",                    1,    0, "a"            },
                { L_,   "\\\"",                   2,    0, "\""           },
                { L_,   "ab",                     2,    0, "ab"           },
                { L_,   "\\x",                    2,    1,                },

                { L_,   "abc",                    3,    0, "abc"          },

                // TBD more vectors
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char *VALUE      = DATA[ti].d_value_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::string result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                   parseUnquotedString(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    bsl::string result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                   parseUnquotedString(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING PARSE SPACE DELIMITED STRING
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseSpaceDelimitedString(cchar **endPos, bStr *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseSpaceDelimitedString" << endl
                          << "=================================" << endl;

        {
            const char *INITIAL_VALUE_1 = "-3";  // first initial value
            const char *INITIAL_VALUE_2 = " 9";  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                const char *d_value_p;  // expected return value
            } DATA[] = {
                //line  spec                 offset  fail  value
                //----  -------------------  ------  ----  --------------
                { L_,   "",                       0,    1,                },

                { L_,   " ",                      0,    1,                },
                { L_,   "\"",                     1,    0, "\""           },
                { L_,   "a",                      1,    0, "a"            },
                { L_,   "b",                      1,    0, "b"            },

                { L_,   "a ",                     1,    0, "a"            },
                { L_,   "a\"",                    2,    0, "a\""          },
                { L_,   "\\\"",                   2,    0, "\""           },
                { L_,   "ab",                     2,    0, "ab"           },
                { L_,   "\\x",                    2,    1,                },

                { L_,   "abc",                    3,    0, "abc"          },

                // TBD more vectors
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char *VALUE      = DATA[ti].d_value_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::string result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                             parseSpaceDelimitedString(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    bsl::string result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                             parseSpaceDelimitedString(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING PARSE QUOTED STRING
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseQuotedString(cchar **endPos, bStr *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseQuotedString" << endl
                          << "===========================" << endl;

        {
            const char *INITIAL_VALUE_1 = "-3";  // first initial value
            const char *INITIAL_VALUE_2 = " 9";  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                const char *d_value_p;  // expected return value
            } DATA[] = {
                //line  spec                 offset  fail  value
                //----  -------------------  ------  ----  --------------
                { L_,   "",                       0,    1,                },

                { L_,   " ",                      0,    1,                },
                { L_,   "\"",                     1,    1,                },
                { L_,   "a",                      0,    1,                },
                { L_,   "b",                      0,    1,                },

                { L_,   "\"a",                    2,    1,                },

                { L_,   "\"a\"",                  3,    0,  "a"           },
                { L_,   "\"\\x",                  3,    1,                },

                { L_,   "\"\\\"\"",               4,    0,  "\""          },

                // TBD more vectors
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char *VALUE      = DATA[ti].d_value_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::string result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                   parseQuotedString(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    bsl::string result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                   parseQuotedString(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING PARSE DELIMITED STRING
        //   This test case verifies that the 'parseDelimitedString' properly
        //   handles the parsing of various kinds of strings given two
        //   delimiters.
        //
        // Concerns:
        //   1) Handles the case with 2 delimiters.
        //   2) Handles the case with only a left delimiter.  The function
        //      should parse from the second character till the end of the
        //      string.
        //   3) Handles the case with only a right delimiter.  This should
        //      parse from the beginning of the string till the first instance
        //      of right delimiter.
        //   4) Handles the case with no delimiters.  This should parse from
        //      the beginning of the string till the end of the string.
        //   5) Should return failure if the left delimiter is specified but
        //      not found.
        //   6) Should return failure if the right delimiter is specified but
        //      not found.
        //   7) Should return failure if right delimiter is part of an escape
        //      character, as specified from the document.
        //
        // Plan:
        //   Since the 'parseDelimitedString' method uses the 'parseCharRaw'
        //   method within its implementation, we need only to verify that the
        //   delimiters are are properly handled (concerns 1 - 6) and that the
        //   positions returned are correct.  We will do so by specifying the
        //   expected endpos and starting position values as offsets from the
        //   beginning of the string, then assert the returned value with the
        //   expected.
        //
        //   For concern 7, we will assert for failure (return value of 0) for
        //   the test case stated in the document.
        //
        // Testing:
        //   parseDelimitedString(cchar **, bStr *, cchar *, cchar, cchar);
        // --------------------------------------------------------------------
        if (verbose) cout << endl
                          << "TESTING parseDatetime" << endl
                          << "=====================" << endl;

        {
            static const struct {
                int         d_lineNum;          // source line number
                const char *d_spec_p;           // specification string
                char        d_leftDelimiter;    // left delimiter
                char        d_rightDelimiter;   // right delimiter
                int         d_expectedStartPos; // expected starting position
                int         d_expectedEndPos;   // expected ending position
                int         d_ret;              // expected return value
            } DATA[] = {
                //                                 rightdelim             ret
                //                                 |                      |
                //line spec             leftdelim  v    startpos  endpos  v
                //---- ---------------- --------- ----  --------  ------ ----
                { L_,  "ABCAB",         'A',      'A',     1,       4,    0  },
                { L_,  "ABCDB",         'A',      'D',     1,       4,    0  },
                { L_,  "ABC\0B",        'A',       0 ,     1,       4,    0  },
                { L_,  "BBCAB",          0 ,      'A',     0,       4,    0  },
                { L_,  "ABCAB",          0 ,      'A',     0,       1,    0  },
                { L_,  "BBCD\0",         0 ,       0 ,     0,       5,    0  },
                { L_,  "BCDEF",         'A',       0 ,    -1,      -1,    1  },
                { L_,  "ABCD\0",        '0',      'A',    -1,      -1,    1  },
                { L_,  "A_some_other_stuff_and_then_\x05A",
                                        'A',      'A',    -1,      -1,    1  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int LINE             = DATA[i].d_lineNum;
                const char *const SPEC     = DATA[i].d_spec_p;
                const char LEFTDELIMITER   = DATA[i].d_leftDelimiter;
                const char RIGHTDELIMITER  = DATA[i].d_rightDelimiter;
                const int EXPECTEDSTARTPOS = DATA[i].d_expectedStartPos;
                const int EXPECTEDENDPOS   = DATA[i].d_expectedEndPos;
                const int RET              = DATA[i].d_ret;

                if (veryVerbose) {
                    T_(); T_(); P(LINE);
                    T_(); T_(); P(SPEC);
                    T_(); T_(); P(LEFTDELIMITER);
                    T_(); T_(); P(RIGHTDELIMITER);
                    T_(); T_(); P(EXPECTEDSTARTPOS);
                    T_(); T_(); P(EXPECTEDENDPOS);
                    T_(); T_(); P(RET);
                }

                const char *endPos;
                bsl::string result;
                int rv = bdepu_TypesParserImpUtil::
                                          parseDelimitedString(&endPos,
                                                               &result,
                                                               SPEC,
                                                               LEFTDELIMITER,
                                                               RIGHTDELIMITER);

                if (RET != 0) {
                    ASSERT(rv != 0);
                }
                else {
                    bsl::string expectedString(SPEC + EXPECTEDSTARTPOS,
                                               SPEC + EXPECTEDENDPOS - 1);
                    if (veryVeryVerbose) {
                        T_(); T_(); T_(); P(expectedString);
                        T_(); T_(); T_(); P(result);
                        T_(); T_(); T_(); P(endPos[0]);
                    }

                    ASSERT(endPos == SPEC + EXPECTEDENDPOS);
                    ASSERT(0 == result.compare(expectedString));
                    ASSERT(rv == RET);
                }
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING PARSE DATETIME
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseDatetime(cchar **endPos, bDT *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseDatetime" << endl
                          << "=====================" << endl;

        {
            const bdet_Date I1(   1, 1, 1);
            const bdet_Date I2(1923, 7, 9);
            const bdet_Datetime INITIAL_VALUE_1(I1);
            const bdet_Datetime INITIAL_VALUE_2(I2);

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_year;     // expected return year
                int         d_month;    // expected return month
                int         d_day;      // expected return day
                int         d_hour;     // expected return hour
                int         d_minute;   // expected return minute
                int         d_second;   // expected return second
            } DATA[] = {
                //line spec                  off fail year  mon  day hh mm ss
                //---- --------------------- --- ---- ----  ---  --- -- -- --
                { L_,  "",                    0,   1,                        },
                { L_,  " ",                   0,   1,                        },
                { L_,  "1",                   1,   1,                        },
                { L_,  "1993/01/02",         10,   1,                        },
                { L_,  "1993/01/02 ",        11,   1,                        },
                { L_,  "1993/01/02_",        11,   1,                        },
                { L_,  "1993/01/02 3",       12,   1,                        },
                { L_,  "1993/01/02_3",       12,   1,                        },
                { L_,  "1993/01/02_3:",      13,   1,                        },
                { L_,  "1993/01/02 3:",      13,   1,                        },
                { L_,  "1993/01/02_3:0",     14,   1,                        },
                { L_,  "1993/01/02 3:0",     14,   1,                        },
                { L_,  "1993/01/02_3:04",    15,   0, 1993,  1,   2, 3, 4, 0 },
                { L_,  "1993/01/02 3:04",    15,   0, 1993,  1,   2, 3, 4, 0 },
                { L_,  "1993/01/02_3:04:",   16,   1,                        },
                { L_,  "1993/01/02 3:04:",   16,   1,                        },
                { L_,  "1993/01/02_3:21:0",  17,   1,                        },
                { L_,  "1993/01/02 3:21:0",  17,   1,                        },
                { L_,  "1993/01/02 3:04:05", 18,   0, 1993,  1,   2, 3, 4, 5 },
                { L_,  "1993/01/02_3:04:05", 18,   0, 1993,  1,   2, 3, 4, 5 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int YEAR         = DATA[ti].d_year;
                const int MONTH        = DATA[ti].d_month;
                const int DAY          = DATA[ti].d_day;
                const int HOUR         = DATA[ti].d_hour;
                const int MINUTE       = DATA[ti].d_minute;
                const int SECOND       = DATA[ti].d_second;
                const int curLen       = strlen(SPEC);

                bdet_Datetime value;
                if (!FAIL) {
                    const bdet_Date DATE(YEAR, MONTH, DAY);
                    const bdet_Time TIME(HOUR, MINUTE, SECOND);
                    value.setDate(DATE);
                    value.setTime(TIME);
                }
                const bdet_Datetime& VALUE = value;

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bdet_Datetime result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                         parseDatetime(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    bdet_Datetime result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                         parseDatetime(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING PARSE TIME
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseTime(cchar **endPos, bTime *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseTime" << endl
                          << "=================" << endl;

        {
            const bdet_Time INITIAL_VALUE_1( 1, 1, 1);  // first init. value
            const bdet_Time INITIAL_VALUE_2(19, 7, 9);  // second init. value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_hour;     // expected return hour
                int         d_minute;   // expected return minute
                int         d_second;   // expected return second
                int         d_milli;    // expected return millisecond
            } DATA[] = {
                //line  spec             off  fail  hour  min  sec  milli
                //----  ---------------  ---  ----  ----  ---  ---  -----
                { L_,   "",                0,    1,                       },
                { L_,   " ",               0,    1,                       },
                { L_,   "1",               1,    1,                       },
                { L_,   "11",              2,    1,                       },
                { L_,   "11:",             3,    1,                       },
                { L_,   "011:",            2,    1,                       },
                { L_,   "11:0",            4,    1,                       },
                { L_,   "11:1:",           4,    1,                       },
                { L_,   "11:01",           5,    0,   11,   1,   0,     0 },
                { L_,   "11:60",           3,    1,                       },
                { L_,   "11:01:",          6,    1,                       },
                { L_,   "11:01:0",         7,    1,                       },
                { L_,   "11:01:2",         7,    1,                       },
                { L_,   "11:01:02",        8,    0,   11,   1,   2,     0 },
                { L_,   "02:34:56",        8,    0,    2,  34,  56,     0 },
                { L_,   "11:01:60",        6,    1,                       },
                { L_,   "24:00:00",        8,    0,   24,   0,   0,     0 },
                { L_,   "24:01:00",        4,    1,                       },
                { L_,   "24:00:01",        7,    1,                       },
                { L_,   "24:10:00",        3,    1,                       },
                { L_,   "24:00:10",        6,    1,                       },
                { L_,   "25:00:00",        0,    1,                       },
                { L_,   "02:34:56.",       9,    1,                       },
                { L_,   "24:00:00.1",      9,    1,                       },
                { L_,   "24:00:00.01",    10,    1,                       },
                { L_,   "24:00:00.001",   11,    1,                       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int HOUR         = DATA[ti].d_hour;
                const int MINUTE       = DATA[ti].d_minute;
                const int SECOND       = DATA[ti].d_second;
                const int curLen       = strlen(SPEC);
                const bdet_Time VALUE(HOUR, MINUTE, SECOND);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bdet_Time result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                             parseTime(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    bdet_Time result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                             parseTime(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING PARSE SHORT
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseShort(cchar **endPos, short *res, cchar *in, int base = 10);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseShort" << endl
                          << "==================" << endl;

        {
            const int INITIAL_VALUE_1 = -3;  // first initial value
            const int INITIAL_VALUE_2 =  9;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                short       d_value;    // expected return value
            } DATA[] = {
                //line  spec            base offset  fail  value
                //----  --------------  ---- ------  ----  -------
                { L_,   "",               10,   0,    1,         },

                { L_,   "a",              10,   0,    1,         },
                { L_,   "+",              10,   1,    1,         },
                { L_,   "-",              10,   1,    1,         },
                { L_,   "0",              10,   1,    0,       0 },
                { L_,   "a",              16,   1,    0,      10 },
                { L_,   "f",              16,   1,    0,      15 },
                { L_,   "A",              16,   1,    0,      10 },
                { L_,   "F",              16,   1,    0,      15 },
                { L_,   "g",              16,   0,    1,         },
                { L_,   "0",               2,   1,    0,       0 },
                { L_,   "1",               2,   1,    0,       1 },
                { L_,   "a",              11,   1,    0,      10 },
                { L_,   "A",              11,   1,    0,      10 },
                { L_,   "z",              36,   1,    0,      35 },
                { L_,   "Z",              36,   1,    0,      35 },

                { L_,   "++",             10,   1,    1,         },
                { L_,   "+-",             10,   1,    1,         },
                { L_,   "-+",             10,   1,    1,         },
                { L_,   "--",             10,   1,    1,         },
                { L_,   "+a",             10,   1,    1,         },
                { L_,   "-a",             10,   1,    1,         },
                { L_,   "+0",             10,   2,    0,       0 },
                { L_,   "+9",             10,   2,    0,       9 },
                { L_,   "-0",             10,   2,    0,       0 },
                { L_,   "-9",             10,   2,    0,      -9 },
                { L_,   "0a",             10,   1,    0,       0 },
                { L_,   "9a",             10,   1,    0,       9 },
                { L_,   "00",             10,   2,    0,       0 },
                { L_,   "01",             10,   2,    0,       1 },
                { L_,   "19",             10,   2,    0,      19 },
                { L_,   "99",             10,   2,    0,      99 },
                { L_,   "+g",             16,   1,    1,         },
                { L_,   "-g",             16,   1,    1,         },
                { L_,   "+a",             16,   2,    0,      10 },
                { L_,   "+f",             16,   2,    0,      15 },
                { L_,   "-a",             16,   2,    0,     -10 },
                { L_,   "-f",             16,   2,    0,     -15 },
                { L_,   "ff",             16,   2,    0,     255 },
                { L_,   "FF",             16,   2,    0,     255 },
                { L_,   "+0",              2,   2,    0,       0 },
                { L_,   "-0",              2,   2,    0,       0 },
                { L_,   "+1",              2,   2,    0,       1 },
                { L_,   "-1",              2,   2,    0,      -1 },
                { L_,   "00",              2,   2,    0,       0 },
                { L_,   "01",              2,   2,    0,       1 },
                { L_,   "10",              2,   2,    0,       2 },
                { L_,   "11",              2,   2,    0,       3 },
                { L_,   "+z",             36,   2,    0,      35 },
                { L_,   "-z",             36,   2,    0,     -35 },
                { L_,   "0z",             36,   2,    0,      35 },
                { L_,   "0Z",             36,   2,    0,      35 },
                { L_,   "10",             36,   2,    0,      36 },
                { L_,   "z0",             36,   2,    0,    1260 },
                { L_,   "Z0",             36,   2,    0,    1260 },

                { L_,   "+0a",            10,   2,    0,       0 },
                { L_,   "+9a",            10,   2,    0,       9 },
                { L_,   "-0a",            10,   2,    0,       0 },
                { L_,   "-9a",            10,   2,    0,      -9 },
                { L_,   "+12",            10,   3,    0,      12 },
                { L_,   "+89",            10,   3,    0,      89 },
                { L_,   "-12",            10,   3,    0,     -12 },
                { L_,   "123",            10,   3,    0,     123 },
                { L_,   "789",            10,   3,    0,     789 },
                { L_,   "+fg",            16,   2,    0,      15 },
                { L_,   "-fg",            16,   2,    0,     -15 },
                { L_,   "+ff",            16,   3,    0,     255 },
                { L_,   "+FF",            16,   3,    0,     255 },
                { L_,   "-ff",            16,   3,    0,    -255 },
                { L_,   "-FF",            16,   3,    0,    -255 },
                { L_,   "fff",            16,   3,    0,    4095 },
                { L_,   "fFf",            16,   3,    0,    4095 },
                { L_,   "FfF",            16,   3,    0,    4095 },
                { L_,   "FFF",            16,   3,    0,    4095 },

                { L_,   "1234",           10,   4,    0,    1234 },
                { L_,   "-123",           10,   4,    0,    -123 },
                { L_,   "7FFF",           16,   4,    0,   32767 },

                { L_,   "12345",          10,   5,    0,   12345 },
                { L_,   "-1234",          10,   5,    0,   -1234 },
                { L_,   "-7FFF",          16,   5,    0,  -32767 },
                { L_,   "32766",          10,   5,    0,   32766 },
                { L_,   "32767",          10,   5,    0,   32767 },
                { L_,   "32768",          10,   4,    0,    3276 },
                { L_,   "32769",          10,   4,    0,    3276 },
                { L_,   "-8000",          16,   5,    0,  -32768 },

                { L_,   "123456",         10,   5,    0,   12345 },
                { L_,   "-32766",         10,   6,    0,  -32766 },
                { L_,   "-32767",         10,   6,    0,  -32767 },
                { L_,   "-32768",         10,   6,    0,  -32768 },
                { L_,   "-32769",         10,   5,    0,   -3276 },

                { L_,   "-123456",        10,   6,    0,  -12345 }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int BASE         = DATA[ti].d_base;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const short VALUE      = DATA[ti].d_value;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    short result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                      parseShort(&endPos, &result, SPEC, BASE);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    short result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                      parseShort(&endPos, &result, SPEC, BASE);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING PARSE INT64
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseInt64(cchar **endPos, int64 *res, cchar *in, int base = 10);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseInt64" << endl
                          << "==================" << endl;

        {
            const int INITIAL_VALUE_1 = -3;  // first initial value
            const int INITIAL_VALUE_2 =  9;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                Int64       d_value;    // expected return value
            } DATA[] = {
                //line  spec            base offset  fail  value
                //----  --------------  ---- ------  ----  -------
                { L_,   "",               10,   0,    1,         },

                { L_,   "a",              10,   0,    1,         },
                { L_,   "+",              10,   1,    1,         },
                { L_,   "-",              10,   1,    1,         },
                { L_,   "0",              10,   1,    0,       0 },
                { L_,   "a",              16,   1,    0,      10 },
                { L_,   "f",              16,   1,    0,      15 },
                { L_,   "A",              16,   1,    0,      10 },
                { L_,   "F",              16,   1,    0,      15 },
                { L_,   "g",              16,   0,    1,         },
                { L_,   "0",               2,   1,    0,       0 },
                { L_,   "1",               2,   1,    0,       1 },
                { L_,   "a",              11,   1,    0,      10 },
                { L_,   "A",              11,   1,    0,      10 },
                { L_,   "z",              36,   1,    0,      35 },
                { L_,   "Z",              36,   1,    0,      35 },

                { L_,   "++",             10,   1,    1,         },
                { L_,   "+-",             10,   1,    1,         },
                { L_,   "-+",             10,   1,    1,         },
                { L_,   "--",             10,   1,    1,         },
                { L_,   "+a",             10,   1,    1,         },
                { L_,   "-a",             10,   1,    1,         },
                { L_,   "+0",             10,   2,    0,       0 },
                { L_,   "+9",             10,   2,    0,       9 },
                { L_,   "-0",             10,   2,    0,       0 },
                { L_,   "-9",             10,   2,    0,      -9 },
                { L_,   "0a",             10,   1,    0,       0 },
                { L_,   "9a",             10,   1,    0,       9 },
                { L_,   "00",             10,   2,    0,       0 },
                { L_,   "01",             10,   2,    0,       1 },
                { L_,   "19",             10,   2,    0,      19 },
                { L_,   "99",             10,   2,    0,      99 },
                { L_,   "+g",             16,   1,    1,         },
                { L_,   "-g",             16,   1,    1,         },
                { L_,   "+a",             16,   2,    0,      10 },
                { L_,   "+f",             16,   2,    0,      15 },
                { L_,   "-a",             16,   2,    0,     -10 },
                { L_,   "-f",             16,   2,    0,     -15 },
                { L_,   "ff",             16,   2,    0,     255 },
                { L_,   "FF",             16,   2,    0,     255 },
                { L_,   "+0",              2,   2,    0,       0 },
                { L_,   "-0",              2,   2,    0,       0 },
                { L_,   "+1",              2,   2,    0,       1 },
                { L_,   "-1",              2,   2,    0,      -1 },
                { L_,   "00",              2,   2,    0,       0 },
                { L_,   "01",              2,   2,    0,       1 },
                { L_,   "10",              2,   2,    0,       2 },
                { L_,   "11",              2,   2,    0,       3 },
                { L_,   "+z",             36,   2,    0,      35 },
                { L_,   "-z",             36,   2,    0,     -35 },
                { L_,   "0z",             36,   2,    0,      35 },
                { L_,   "0Z",             36,   2,    0,      35 },
                { L_,   "10",             36,   2,    0,      36 },
                { L_,   "z0",             36,   2,    0,    1260 },
                { L_,   "Z0",             36,   2,    0,    1260 },

                { L_,   "+0a",            10,   2,    0,       0 },
                { L_,   "+9a",            10,   2,    0,       9 },
                { L_,   "-0a",            10,   2,    0,       0 },
                { L_,   "-9a",            10,   2,    0,      -9 },
                { L_,   "+12",            10,   3,    0,      12 },
                { L_,   "+89",            10,   3,    0,      89 },
                { L_,   "-12",            10,   3,    0,     -12 },
                { L_,   "123",            10,   3,    0,     123 },
                { L_,   "789",            10,   3,    0,     789 },
                { L_,   "+fg",            16,   2,    0,      15 },
                { L_,   "-fg",            16,   2,    0,     -15 },
                { L_,   "+ff",            16,   3,    0,     255 },
                { L_,   "+FF",            16,   3,    0,     255 },
                { L_,   "-ff",            16,   3,    0,    -255 },
                { L_,   "-FF",            16,   3,    0,    -255 },
                { L_,   "fff",            16,   3,    0,    4095 },
                { L_,   "fFf",            16,   3,    0,    4095 },
                { L_,   "FfF",            16,   3,    0,    4095 },
                { L_,   "FFF",            16,   3,    0,    4095 },

                { L_,   "1234",           10,   4,    0,    1234 },
                { L_,   "-123",           10,   4,    0,    -123 },
                { L_,   "7FFF",           16,   4,    0,   32767 },

                { L_,   "12345",          10,   5,    0,   12345 },
                { L_,   "-1234",          10,   5,    0,   -1234 },
                { L_,   "-7FFF",          16,   5,    0,  -32767 },
                { L_,   "32766",          10,   5,    0,   32766 },
                { L_,   "32767",          10,   5,    0,   32767 },
                { L_,   "32768",          10,   5,    0,   32768 },
                { L_,   "32769",          10,   5,    0,   32769 },
                { L_,   "-8000",          16,   5,    0,  -32768 },

                { L_,   "123456",         10,   6,    0,  123456 },
                { L_,   "-32766",         10,   6,    0,  -32766 },
                { L_,   "-32767",         10,   6,    0,  -32767 },
                { L_,   "-32768",         10,   6,    0,  -32768 },
                { L_,   "-32769",         10,   6,    0,  -32769 },

                { L_,   "-123456",        10,   7,    0, -123456 },

                { L_,   "2147483647",     10,  10,    0,  2147483647 },
                { L_,   "2147483648",     10,  10,    0,  2147483648LL },
                { L_,   "2147483649",     10,  10,    0,  2147483649LL },

                { L_,   "-2147483647",    10,  11,    0, -2147483647LL },
                { L_,   "-2147483648",    10,  11,    0, -2147483648LL },
                { L_,   "-2147483649",    10,  11,    0, -2147483649LL },

                { L_,   "7fffffffffffffff",  16,  16,    0,
                                                 0x7FFFFFFFFFFFFFFFLL },
                { L_,   "8000000000000000",  16,  15,    0,
                                                  0x800000000000000LL },

                { L_,   "-8000000000000000", 16,  17,    0,
                                                -0x8000000000000000LL },
                { L_,   "-8000000000000001", 16,  16,    0,
                                                 -0x800000000000000LL },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int BASE         = DATA[ti].d_base;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const Int64 VALUE      = DATA[ti].d_value;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    Int64 result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                      parseInt64(&endPos, &result, SPEC, BASE);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? (Int64)INITIAL_VALUE_1
                                                   : VALUE));
                }

                {  // test with second initial value
                    Int64 result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                      parseInt64(&endPos, &result, SPEC, BASE);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? (Int64)INITIAL_VALUE_2
                                                   : VALUE));
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING PARSE INT
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseInt(cchar **endPos, int *res, cchar *in, int base = 10);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseInt" << endl
                          << "================" << endl;

        {
            const int INITIAL_VALUE_1 = -3;  // first initial value
            const int INITIAL_VALUE_2 =  9;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_value;    // expected return value
            } DATA[] = {
                //line  spec            base offset  fail  value
                //----  --------------  ---- ------  ----  -------
                { L_,   "",               10,   0,    1,         },

                { L_,   "a",              10,   0,    1,         },
                { L_,   "+",              10,   1,    1,         },
                { L_,   "-",              10,   1,    1,         },
                { L_,   "0",              10,   1,    0,       0 },
                { L_,   "a",              16,   1,    0,      10 },
                { L_,   "f",              16,   1,    0,      15 },
                { L_,   "A",              16,   1,    0,      10 },
                { L_,   "F",              16,   1,    0,      15 },
                { L_,   "g",              16,   0,    1,         },
                { L_,   "0",               2,   1,    0,       0 },
                { L_,   "1",               2,   1,    0,       1 },
                { L_,   "a",              11,   1,    0,      10 },
                { L_,   "A",              11,   1,    0,      10 },
                { L_,   "z",              36,   1,    0,      35 },
                { L_,   "Z",              36,   1,    0,      35 },

                { L_,   "++",             10,   1,    1,         },
                { L_,   "+-",             10,   1,    1,         },
                { L_,   "-+",             10,   1,    1,         },
                { L_,   "--",             10,   1,    1,         },
                { L_,   "+a",             10,   1,    1,         },
                { L_,   "-a",             10,   1,    1,         },
                { L_,   "+0",             10,   2,    0,       0 },
                { L_,   "+9",             10,   2,    0,       9 },
                { L_,   "-0",             10,   2,    0,       0 },
                { L_,   "-9",             10,   2,    0,      -9 },
                { L_,   "0a",             10,   1,    0,       0 },
                { L_,   "9a",             10,   1,    0,       9 },
                { L_,   "00",             10,   2,    0,       0 },
                { L_,   "01",             10,   2,    0,       1 },
                { L_,   "19",             10,   2,    0,      19 },
                { L_,   "99",             10,   2,    0,      99 },
                { L_,   "+g",             16,   1,    1,         },
                { L_,   "-g",             16,   1,    1,         },
                { L_,   "+a",             16,   2,    0,      10 },
                { L_,   "+f",             16,   2,    0,      15 },
                { L_,   "-a",             16,   2,    0,     -10 },
                { L_,   "-f",             16,   2,    0,     -15 },
                { L_,   "ff",             16,   2,    0,     255 },
                { L_,   "FF",             16,   2,    0,     255 },
                { L_,   "+0",              2,   2,    0,       0 },
                { L_,   "-0",              2,   2,    0,       0 },
                { L_,   "+1",              2,   2,    0,       1 },
                { L_,   "-1",              2,   2,    0,      -1 },
                { L_,   "00",              2,   2,    0,       0 },
                { L_,   "01",              2,   2,    0,       1 },
                { L_,   "10",              2,   2,    0,       2 },
                { L_,   "11",              2,   2,    0,       3 },
                { L_,   "+z",             36,   2,    0,      35 },
                { L_,   "-z",             36,   2,    0,     -35 },
                { L_,   "0z",             36,   2,    0,      35 },
                { L_,   "0Z",             36,   2,    0,      35 },
                { L_,   "10",             36,   2,    0,      36 },
                { L_,   "z0",             36,   2,    0,    1260 },
                { L_,   "Z0",             36,   2,    0,    1260 },

                { L_,   "+0a",            10,   2,    0,       0 },
                { L_,   "+9a",            10,   2,    0,       9 },
                { L_,   "-0a",            10,   2,    0,       0 },
                { L_,   "-9a",            10,   2,    0,      -9 },
                { L_,   "+12",            10,   3,    0,      12 },
                { L_,   "+89",            10,   3,    0,      89 },
                { L_,   "-12",            10,   3,    0,     -12 },
                { L_,   "123",            10,   3,    0,     123 },
                { L_,   "789",            10,   3,    0,     789 },
                { L_,   "+fg",            16,   2,    0,      15 },
                { L_,   "-fg",            16,   2,    0,     -15 },
                { L_,   "+ff",            16,   3,    0,     255 },
                { L_,   "+FF",            16,   3,    0,     255 },
                { L_,   "-ff",            16,   3,    0,    -255 },
                { L_,   "-FF",            16,   3,    0,    -255 },
                { L_,   "fff",            16,   3,    0,    4095 },
                { L_,   "fFf",            16,   3,    0,    4095 },
                { L_,   "FfF",            16,   3,    0,    4095 },
                { L_,   "FFF",            16,   3,    0,    4095 },

                { L_,   "1234",           10,   4,    0,    1234 },
                { L_,   "-123",           10,   4,    0,    -123 },
                { L_,   "7FFF",           16,   4,    0,   32767 },

                { L_,   "12345",          10,   5,    0,   12345 },
                { L_,   "-1234",          10,   5,    0,   -1234 },
                { L_,   "-7FFF",          16,   5,    0,  -32767 },
                { L_,   "32766",          10,   5,    0,   32766 },
                { L_,   "32767",          10,   5,    0,   32767 },
                { L_,   "32768",          10,   5,    0,   32768 },
                { L_,   "32769",          10,   5,    0,   32769 },
                { L_,   "-8000",          16,   5,    0,  -32768 },

                { L_,   "123456",         10,   6,    0,  123456 },
                { L_,   "-32766",         10,   6,    0,  -32766 },
                { L_,   "-32767",         10,   6,    0,  -32767 },
                { L_,   "-32768",         10,   6,    0,  -32768 },
                { L_,   "-32769",         10,   6,    0,  -32769 },

                { L_,   "-123456",        10,   7,    0, -123456 },

                { L_,   "2147483647",     10,  10,    0,  2147483647 },
                { L_,   "2147483648",     10,   9,    0,   214748364 },
                { L_,   "2147483649",     10,   9,    0,   214748364 },

                { L_,   "-2147483647",    10,  11,    0, -2147483647 },
                { L_,   "-2147483648",    10,  11,    0, -2147483648LL },
                { L_,   "-2147483649",    10,  10,    0,  -214748364 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int BASE         = DATA[ti].d_base;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int VALUE        = DATA[ti].d_value;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    int result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                        parseInt(&endPos, &result, SPEC, BASE);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    int result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                        parseInt(&endPos, &result, SPEC, BASE);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PARSE FLOAT
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseFloat(cchar **endPos, double *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseFloat" << endl
                          << "==================" << endl;

        {
            const float INITIAL_VALUE_1 = -3.0F;  // first initial value
            const float INITIAL_VALUE_2 =  9.1F;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                float       d_value;    // expected return value
            } DATA[] = {
                //line  spec            offset  fail  value
                //----  --------------  ------  ----  -------
                { L_,   "",                  0,    1,         },

                { L_,   "+",                 1,    1,         },
                { L_,   "-",                 1,    1,         },
                { L_,   ".",                 1,    1,         },
                { L_,   "a",                 0,    1,         },
                { L_,   "0",                 1,    0, 0.0F    },
                { L_,   "1",                 1,    0, 1.0F    },
                { L_,   "9",                 1,    0, 9.0F    },
                { L_,   "e",                 0,    1,         },
                { L_,   "E",                 0,    1,         },

                { L_,   "++",                1,    1,         },
                { L_,   "+-",                1,    1,         },
                { L_,   "-+",                1,    1,         },
                { L_,   "--",                1,    1,         },
                { L_,   "+1",                2,    0, 1.0F    },
                { L_,   "-1",                2,    0, -1.0F   },
                { L_,   "10",                2,    0, 10.0F   },
                { L_,   "90",                2,    0, 90.0F   },
                { L_,   ".0",                2,    0, 0.0F    },
                { L_,   ".1",                2,    0, 0.1F    },
                { L_,   ".9",                2,    0, 0.9F    },
                { L_,   "+e",                1,    1,         },
                { L_,   "-e",                1,    1,         },
                { L_,   "+E",                1,    1,         },
                { L_,   "-E",                1,    1,         },
                { L_,   "0e",                2,    1,         },
                { L_,   "0E",                2,    1,         },

                { L_,   "-10",               3,    0, -10.0F  },
                { L_,   "-90",               3,    0, -90.0F  },
                { L_,   "-.0",               3,    0, 0.0F    },
                { L_,   "-.1",               3,    0, -0.1F   },
                { L_,   "-.9",               3,    0, -0.9F   },
                { L_,   "0ee",               2,    1,         },
                { L_,   "0eE",               2,    1,         },
                { L_,   "0Ee",               2,    1,         },
                { L_,   "0EE",               2,    1,         },
                { L_,   "0e0",               3,    0, 0.0F    },
                { L_,   "0e1",               3,    0, 0.0F    },
                { L_,   "1e0",               3,    0, 1.0F    },
                { L_,   "1e1",               3,    0, 10.0F   },
                { L_,   "1e2",               3,    0, 100.0F  },
                { L_,   "0E0",               3,    0, 0.0F    },
                { L_,   "0E1",               3,    0, 0.0F    },
                { L_,   "1E0",               3,    0, 1.0F    },
                { L_,   "1E1",               3,    0, 10.0F   },
                { L_,   "1E2",               3,    0, 100.0F  },

                { L_,   "1e500",             5,    1,         },

                { L_,   "1e-500",            6,    0, 0.0F    },

                { L_,   "1e1000000000",     12,    1,         },
                { L_,   "1e2147483647",     12,    1,         },
                { L_,   "1e-2147483647",    13,    0, 0.0F    },
                { L_,   "1e-2147483648",    13,    1,         },

                // TBD more vectors
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const float VALUE      = DATA[ti].d_value;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    float result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                            parseFloat(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    float result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                            parseFloat(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PARSE DOUBLE
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseDouble(cchar **endPos, double *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseDouble" << endl
                          << "===================" << endl;

        {
            const double INITIAL_VALUE_1 =  37.0;  // first initial value
            const double INITIAL_VALUE_2 = -58.0;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                double      d_value;    // expected return value
            } DATA[] = {
                //line  spec            offset  fail  value
                //----  --------------  ------  ----  -------
                { L_,   "",                  0,    1,         },

                { L_,   "+",                 1,    1,         },
                { L_,   "-",                 1,    1,         },
                { L_,   ".",                 1,    1,         },
                { L_,   "a",                 0,    1,         },
                { L_,   "0",                 1,    0, 0.0     },
                { L_,   "1",                 1,    0, 1.0     },
                { L_,   "9",                 1,    0, 9.0     },
                { L_,   "e",                 0,    1,         },
                { L_,   "E",                 0,    1,         },

                { L_,   "++",                1,    1,         },
                { L_,   "+-",                1,    1,         },
                { L_,   "-+",                1,    1,         },
                { L_,   "--",                1,    1,         },
                { L_,   "+1",                2,    0, 1.0     },
                { L_,   "-1",                2,    0, -1.0    },
                { L_,   "10",                2,    0, 10.0    },
                { L_,   "90",                2,    0, 90.0    },
                { L_,   ".0",                2,    0, 0.0     },
                { L_,   ".1",                2,    0, 0.1     },
                { L_,   ".9",                2,    0, 0.9     },
                { L_,   "+e",                1,    1,         },
                { L_,   "-e",                1,    1,         },
                { L_,   "+E",                1,    1,         },
                { L_,   "-E",                1,    1,         },
                { L_,   "0e",                2,    1,         },
                { L_,   "0E",                2,    1,         },

                { L_,   "-10",               3,    0, -10.0   },
                { L_,   "-90",               3,    0, -90.0   },
                { L_,   "-.0",               3,    0, 0.0     },
                { L_,   "-.1",               3,    0, -0.1    },
                { L_,   "-.9",               3,    0, -0.9    },
                { L_,   "0ee",               2,    1,         },
                { L_,   "0eE",               2,    1,         },
                { L_,   "0Ee",               2,    1,         },
                { L_,   "0EE",               2,    1,         },
                { L_,   "0e0",               3,    0, 0.0     },
                { L_,   "0e1",               3,    0, 0.0     },
                { L_,   "1e0",               3,    0, 1.0     },
                { L_,   "1e1",               3,    0, 10.0    },
                { L_,   "1e2",               3,    0, 100.0   },
                { L_,   "0E0",               3,    0, 0.0     },
                { L_,   "0E1",               3,    0, 0.0     },
                { L_,   "1E0",               3,    0, 1.0     },
                { L_,   "1E1",               3,    0, 10.0    },
                { L_,   "1E2",               3,    0, 100.0   },

                { L_,   "1e500",             5,    1,         },

                { L_,   "1e-500",            6,    0, 0.0     },

                { L_,   "1e1000000000",     12,    1,         },
                { L_,   "1e2147483647",     12,    1,         },
                { L_,   "1e-2147483647",    13,    0, 0.0     },
                { L_,   "1e-2147483648",    13,    1,         },

                { L_,   "12345678000000000000",
                                            20,    0,
                                                 1.2345678e19 },

                { L_,   "100",               3,    0, 100.0   },
                { L_,   "10000",             5,    0, 10000.0 },
                { L_,   "1000000",           7,    0,
                                              1000000.0       },
                { L_,   "100000000",         9,    0,
                                              100000000.0     },
                { L_,   "10000000000",      11,    0,
                                              10000000000.0   },
                { L_,   "1000000000000",    13,    0,
                                              1000000000000.0 },
                { L_,   "100000000000000",  15,    0, 1e14    },
                { L_,   "10000000000000000",
                                            17,    0, 1e16    },
                { L_,   "1000000000000000000",
                                            19,    0, 1e18    },
                { L_,   "100000000000000000000",
                                            21,    0, 1e20    },
                { L_,   "10000000000000000000000",
                                            23,    0, 1e22    },
                { L_,   "1000000000000000000000000",
                                            25,    0, 1e24    },
                { L_,   "100000000000000000000000000",
                                            27,    0, 1e26    },
                { L_,   "10000000000000000000000000000",
                                            29,    0, 1e28    },
                { L_,   "1000000000000000000000000000000",
                                            31,    0, 1e30    },
                { L_,   "100000000000000000000000000000000",
                                            33,    0, 1e32    },
                { L_,   "10000000000000000000000000000000000",
                                            35,    0, 1e34    },
                { L_,   "1000000000000000000000000000000000000",
                                            37,    0, 1e36    },

                { L_,   "-100",              4,    0, -100.0  },
                { L_,   "-10000",            6,    0, -10000.0},
                { L_,   "-1000000",          8,    0,
                                              -1000000.0      },
                { L_,   "-100000000",       10,    0,
                                              -100000000.0    },
                { L_,   "-10000000000",     12,    0,
                                              -10000000000.0  },
                { L_,   "-1000000000000",   14,    0,
                                              -1000000000000.0},
                { L_,   "-100000000000000", 16,    0, -1e14   },
                { L_,   "-10000000000000000",
                                            18,    0, -1e16   },
                { L_,   "-1000000000000000000",
                                            20,    0, -1e18   },
                { L_,   "-100000000000000000000",
                                            22,    0, -1e20   },
                { L_,   "-10000000000000000000000",
                                            24,    0, -1e22   },
                { L_,   "-1000000000000000000000000",
                                            26,    0, -1e24   },
                { L_,   "-100000000000000000000000000",
                                            28,    0, -1e26   },
                { L_,   "-10000000000000000000000000000",
                                            30,    0, -1e28   },
                { L_,   "-1000000000000000000000000000000",
                                            32,    0, -1e30   },
                { L_,   "-100000000000000000000000000000000",
                                            34,    0, -1e32   },
                { L_,   "-10000000000000000000000000000000000",
                                            36,    0, -1e34   },
                { L_,   "-1000000000000000000000000000000000000",
                                            38,    0, -1e36   },

                // TBD more vectors
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SZ = 64;  // maximum length of an input string + 2
            char buffer[SZ];

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const double VALUE     = DATA[ti].d_value;
                const int curLen       = strlen(SPEC);

                LOOP_ASSERT(LINE, curLen + 2 < SZ);
                                 // orthogonal perturbation will fit in buffer

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    //LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // orthogonal perturbation over terminating whitespace
                for (int si = 0; si < 2; ++si) {
                    strcpy(buffer, SPEC);
                    if (si) {
                      buffer[curLen] = ' ';
                      buffer[curLen + 1] = '\0';
                    }

                    {  // test with first initial value
                        double result = INITIAL_VALUE_1;
                        const char *endPos = 0;
                        int rv = bdepu_TypesParserImpUtil::
                                         parseDouble(&endPos, &result, buffer);
                        if (veryVerbose) {
                            P_(SPEC);
                            P_(rv);
                            P_(result);
                            if (rv) {
                                P(INITIAL_VALUE_1);
                            }
                            else {
                                P(VALUE);
                            }
                        }
                        LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                    }

                    {  // test with second initial value
                        double result = INITIAL_VALUE_2;
                        const char *endPos = 0;
                        int rv = bdepu_TypesParserImpUtil::
                                         parseDouble(&endPos, &result, buffer);
                        if (veryVerbose) {
                            P_(SPEC);
                            P_(rv);
                            P_(result);
                            if (rv) {
                                P(INITIAL_VALUE_2);
                            }
                            else {
                                P(VALUE);
                            }
                        }

                        LOOP2_ASSERT(LINE, si, buffer + NUM == endPos);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                   result == (rv ? INITIAL_VALUE_2 : VALUE));
                    }
                } // end for si....
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PARSE DATE
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseDate(cchar **endPos, bDate *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseDate" << endl
                          << "=================" << endl;

        {
            const bdet_Date INITIAL_VALUE_1(   1, 1, 1);  // first init. value
            const bdet_Date INITIAL_VALUE_2(1923, 7, 9);  // second init. value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_year;     // expected return year
                int         d_month;    // expected return month
                int         d_day;      // expected return day
            } DATA[] = {
                //line  spec             off  fail  year  month  day
                //----  ---------------  ---  ----  ----  -----  ---
                { L_,   "",                0,    1,                  },
                { L_,   " ",               0,    1,                  },
                { L_,   "0",               0,    1,                  },
                { L_,   "1",               1,    1,                  },
                { L_,   "19",              2,    1,                  },
                { L_,   "199",             3,    1,                  },
                { L_,   "1993",            4,    1,                  },
                { L_,   "1993/",           5,    1,                  },
                { L_,   "1993/0",          5,    1,                  },
                { L_,   "19/1/2",          6,    0,   19,     1,   2 },
                { L_,   "1993/01",         7,    1,                  },
                { L_,   "1993/13",         6,    1,                  },
                { L_,   "1993/01/",        8,    1,                  },
                { L_,   "19/01/02",        8,    0,   19,     1,   2 },
                { L_,   "1993/01/0",       8,    1,                  },
                { L_,   "199/01/02",       9,    0,  199,     1,   2 },
                { L_,   "1993/1/02",       9,    0, 1993,     1,   2 },
                { L_,   "1993/01/2",       9,    0, 1993,     1,   2 },
                { L_,   "1993/01/02",     10,    0, 1993,     1,   2 },
                { L_,   "1993/01/32",      9,    1,                  },
                { L_,   "1993/02/31",      9,    1,                  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int YEAR         = DATA[ti].d_year;
                const int MONTH        = DATA[ti].d_month;
                const int DAY          = DATA[ti].d_day;
                const int curLen       = strlen(SPEC);

                bdet_Date value;
                if (!FAIL) {
                    value.setYearMonthDay(YEAR, MONTH, DAY);
                }
                const bdet_Date& VALUE = value;

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bdet_Date result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                             parseDate(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : value));
                }

                {  // test with second initial value
                    bdet_Date result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                             parseDate(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : value));
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PARSE CHAR
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseChar(cchar **endPos, char *result, cchar *inputString);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseChar" << endl
                          << "=================" << endl;

        {
            const char INITIAL_VALUE_1 = 'q';  // first initial value
            const char INITIAL_VALUE_2 = 'z';  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                char        d_value;    // expected return value
            } DATA[] = {
                //line  spec        off  fail  value
                //----  ----------  ---  ----  ---------
                { L_,   "",           0,    1,           },
                { L_,   " ",          0,    1,           },
                { L_,   "a",          0,    1,           },
                { L_,   "'",          1,    1,           },
                { L_,   "''",         1,    1,           },
                { L_,   "'a",         2,    1,           },
                { L_,   "'\\",        2,    1,           },
                { L_,   "'\\0",       3,    1,           },
                { L_,   "'\\x",       3,    1,           },
                { L_,   "'a'",        3,    0, 'a'       },
                { L_,   "'b'",        3,    0, 'b'       },
                { L_,   "'c'",        3,    0, 'c'       },
                { L_,   "'\\0'",      4,    0, '\0'      },
                { L_,   "'\\1'",      4,    0, (char)1   },
                { L_,   "'\\t'",      4,    0, '\t'      },
                { L_,   "'\\11'",     5,    0, (char)9   },
                { L_,   "'\\18'",     3,    1,           },
                { L_,   "'\\x1'",     5,    0, (char)1   },
                { L_,   "'\\xf'",     5,    0, (char)15  },
                { L_,   "'\\xF'",     5,    0, (char)15  },
                { L_,   "'\\xFF'",    6,    0, (char)255 },
                { L_,   "'\\xFFF'",   5,    1            },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char VALUE       = DATA[ti].d_value;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    char result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                             parseChar(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    char result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                             parseChar(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING PARSE CHAR RAW
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseCharRaw(cchar **endPos, char *result, cchar *inputString);
        //   parseCharRaw(cchar **endPos, char *result, cchar *input, int *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseChar" << endl
                          << "=================" << endl;

        {
            const char INITIAL_VALUE_1 = 'q';  // first initial value
            const char INITIAL_VALUE_2 = 'z';  // second initial value
            const char INITIAL_FLAG_1 = -3;  // first initial value
            const char INITIAL_FLAG_2 =  5;  // second initial value

            const int FV = bdepu_TypesParserImpUtil::BDEPU_HAS_ESCAPE;

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                char        d_value;    // expected return value
                int         d_flag;     // parsing expected to flag indicator
            } DATA[] = {
                //line  spec        off  fail  value      flag
                //----  ----------  ---  ----  ---------  ----
                { L_,   "",           0,    1,                 },
                { L_,   " ",          1,    0, ' ',         0  },
                { L_,   "'",          1,    0, '\'',        0  },
                { L_,   "a",          1,    0, 'a',         0  },
                { L_,   "b",          1,    0, 'b',         0  },
                { L_,   "c",          1,    0, 'c',         0  },
                { L_,   "\a",         1,    0, '\a',        0  },
                { L_,   "\b",         1,    0, '\b',        0  },
                { L_,   "\f",         1,    0, '\f',        0  },
                { L_,   "\n",         1,    0, '\n',        0  },
                { L_,   "\r",         1,    0, '\r',        0  },
                { L_,   "\t",         1,    0, '\t',        0  },
                { L_,   "\v",         1,    0, '\v',        0  },
                { L_,   "\'",         1,    0, '\'',        0  },
                { L_,   "\"",         1,    0, '"',         0  },
                { L_,   "\?",         1,    0, '?',         0  },
                { L_,   "\\0",        2,    0, '\0',        FV },
                { L_,   "\\x",        2,    1,                 },
                { L_,   "\\1",        2,    0, (char)1,     FV },
                { L_,   "\\t",        2,    0, '\t',        FV },
                { L_,   "\\11",       3,    0, (char)9,     FV },
                { L_,   "\\18",       2,    0, (char)1,     FV },
                { L_,   "\\x1",       3,    0, (char)1,     FV },
                { L_,   "\\xf",       3,    0, (char)15,    FV },
                { L_,   "\\xF",       3,    0, (char)15,    FV },
                { L_,   "\\xFF",      4,    0, (char)255,   FV },
                { L_,   "\\xFFF",     4,    0, (char)255,   FV },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char VALUE       = DATA[ti].d_value;
                const int FLAG         = DATA[ti].d_flag;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    char result = INITIAL_VALUE_1;
                    int flag = INITIAL_FLAG_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                   parseCharRaw(&endPos, &result, SPEC, &flag);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                    LOOP_ASSERT(LINE, flag == (rv ? INITIAL_FLAG_1 : FLAG));
                }

                {  // test with second initial value
                    char result = INITIAL_VALUE_2;
                    int flag = INITIAL_FLAG_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                   parseCharRaw(&endPos, &result, SPEC, &flag);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                    LOOP_ASSERT(LINE, flag == (rv ? INITIAL_FLAG_2 : FLAG));
                }

                {  // test with first initial value
                    char result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                          parseCharRaw(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    char result = INITIAL_VALUE_2;
                    const char *endPos = 0;
                    int rv = bdepu_TypesParserImpUtil::
                                          parseCharRaw(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
