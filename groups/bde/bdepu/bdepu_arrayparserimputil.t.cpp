// bdepu_arrayparserimputil.t.cpp              -*-C++-*-

#include <bdepu_arrayparserimputil.h>
#include <bdepu_typesparserimputil.h>
#include <bdepu_parserimputil.h>

#include <bsl_vector.h>
#include <bslma_testallocator.h>    // For testing only

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>     // strlen()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// This driver tests parsing and generating functions of 'bdepu_TypesParser'.
// Each parsing function has different constrains, which are described in the
// function documentation in 'bdepu_typesparser.h'.  However, each of the
// parsed strings representing basic types may be preceded by arbitrary
// <WHITESPACE>, which is also defined in 'bdepu_typesparser.h'.  Array types
// may have arbitrary <WHITESPACE> within the string.  We define a global set
// of representative <WHITESPACE> values, and use "orthogonal perturbation" to
// test each case with every input we choose for our test cases.
//
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
//  'bDatez      for 'bdet_DateTz'
//  'bTime'      for 'bdet_Time'
//  'bTimez'     for 'bdet_TimeTz'
//  'bDT'        for 'bdet_Datetime'
//  'bDTz'       for 'bdet_DatetimeTz'
//  'ArCh'       for 'vector<char>'
//  'ArSh'       for 'vector<short>'
//  'ArInt'      for 'vector<int>'
//  'ArInt64'    for 'vector<bsls_PlatformUtil::Int64>'
//  'ArFl'       for 'vector<float>'
//  'ArDb'       for 'vector<double>'
//  'ArStr'      for 'vector<bsl::string>'
//  'ArDate'     for 'vector<bdet_Date>'
//  'ArTime'     for 'vector<bdet_Time>'
//  'ArDT'       for 'vector<bdet_Datetime>'
//
//  'c' in front of an abbreviation stands for 'const'.
//
//-----------------------------------------------------------------------------
// [11] generateCharArray(ArCh *buffer, cArCh& val);
// [12] generateDateArray(ArCh *buffer, cArDate& val);
// [13] generateDatetimeArray(ArCh *buffer, cArDT& val);
// [26] generateDatetimeTzArray(ArCh *buffer, cArDTz& val);
// [24] generateDateTzArray(ArCh *buffer, cArDatez& val);
// [14] generateDoubleArray(ArCh *buffer, cArDb& val);
// [15] generateFloatArray(ArCh *buffer, cArFl& val);
// [16] generateIntArray(ArCh *buffer, cArInt& val);
// [17] generateInt64Array(ArCh *buffer, cArInt64& val);
// [18] generateShortArray(ArCh *buffer, cArSh& val);
// [19] generateStringArray(ArCh *buffer, cArStr& val);
// [20] generateTimeArray(ArCh *buffer, cArTime& val);
// [25] generateTimeTzArray(ArCh *buffer, cArTimez& val);
// [ 1] parseCharArray(cchar **endPos, ArCh *res, cchar *in);
// [ 2] parseDateArray(cchar **endPos, ArDate *res, cchar *in);
// [ 3] parseDatetimeArray(cchar **endPos, ArDT *res, cchar *in);
// [22] parseDatetimeTzArray(cchar **endPos, ArDTz *res, cchar *in);
// [21] parseDateTzArray(cchar **endPos, ArDatez *res, cchar *in);
// [ 4] parseDoubleArray(cchar **endPos, ArDb *res, cchar *in);
// [ 5] parseFloatArray(cchar **endPos, ArFl *res, cchar *in);
// [ 6] parseIntArray(cchar **endPos, ArInt *res, cchar *in);
// [ 7] parseInt64Array(cchar **endPos, ArInt64 *res, cchar *in);
// [ 8] parseShortArray(cchar **endPos, ArSh *res, cchar *in);
// [ 9] parseStringArray(cchar **endPos, ArStr *res, cchar *in);
// [10] parseTimeArray(cchar **endPos, ArTime *res, cchar *in);
// [23] parseTimeTzArray(cchar **endPos, ArTimez *res, cchar *in);
//-----------------------------------------------------------------------------
// [27] USAGE EXAMPLE



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
#define T_()  cout << '\t' << flush;          // Print a tab character



//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdepu_ParserImpUtil ImpUtil;
typedef bsls_PlatformUtil::Int64 Int64;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 27: {
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

        vector<int> initial;
        initial.push_back(1);
        initial.push_back(2);
        initial.push_back(3);
        initial.push_back(4);

        vector<char> buffer;
        bdepu_ArrayParserImpUtil::generateIntArray(&buffer, initial);
        ASSERT(0 == strncmp("[ 1 2 3 4 ]", &buffer.front(), 11));

        vector<int> final;
        const char *pos;
        bdepu_ArrayParserImpUtil::parseIntArray(&pos, &final, &buffer.front());
        ASSERT(&buffer.front() + 11 == pos);
        ASSERT(initial == final);
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DATETIMETZ ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateDatetimeTzArray(ArCh *buffer, cArDTz& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDatetimeTzArray" << endl
                          << "========================== =====" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            int         d_year;            // specification year
            int         d_month;           // specification month
            int         d_day;             // specification day
            int         d_hour;            // specification hour
            int         d_minute;          // specification minute
            int         d_second;          // specification second
            int         d_millisecond;     // specification millisecond
            int         d_tz;              // TZ offset
            const char *d_exp_p;           // expected result
        } DATA[] = {
            // LN year MM  DD  HH  MM  SS  MS       tz  exp
            //--- ---- --  --  --  --  --  ---   -----
            { L_,    1,  2,  3,  1,  2,  3,   4,     0,
                                        "0001/02/03 01:02:03.004+0000" },
            { L_,   10,  2,  3, 12, 21, 32,  40,     0,
                                        "0010/02/03 12:21:32.040+0000" },
            { L_,  100,  2,  3, 12, 21, 32, 417,     0,
                                        "0100/02/03 12:21:32.417+0000" },
            { L_, 1000,  2,  3, 23, 59, 59,   0,     0,
                                        "1000/02/03 23:59:59.000+0000" },
            { L_, 9999,  2,  3, 12, 21, 32,  40,     0,
                                        "9999/02/03 12:21:32.040+0000" },
            { L_, 9999, 12, 31,  1,  2,  3,   4,     0,
                                        "9999/12/31 01:02:03.004+0000" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_DatetimeTz> VALUES(&testAllocator);
        bsl::vector<bsl::string> EXPECTED(&testAllocator);

        vector<char> expected(&testAllocator);
        expected.push_back('[');
        expected.push_back(' ');
        expected.push_back(']');

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE          = DATA[ti].d_lineNum;
            const int YEAR          = DATA[ti].d_year;
            const int MONTH         = DATA[ti].d_month;
            const int DAY           = DATA[ti].d_day;
            const int HOUR          = DATA[ti].d_hour;
            const int MINUTE        = DATA[ti].d_minute;
            const int SECOND        = DATA[ti].d_second;
            const int MILLISECOND   = DATA[ti].d_millisecond;
            const int OFFSET        = DATA[ti].d_tz;
            const char *const EXP   = DATA[ti].d_exp_p;

            const bdet_Datetime VALUEDT(YEAR,
                                        MONTH,
                                        DAY,
                                        HOUR,
                                        MINUTE,
                                        SECOND,
                                        MILLISECOND);
            const bdet_DatetimeTz VALUE(VALUEDT, OFFSET);
            VALUES.push_back(VALUE);

            const bsl::string EXPSTR(EXP, &testAllocator);
            expected.pop_back();
            expected.insert(expected.end(), EXPSTR.begin(), EXPSTR.end());
            expected.push_back(' ');
            expected.push_back(']');

            vector<char> buffer(&testAllocator);
            bdepu_ArrayParserImpUtil::generateDatetimeTzArray(&buffer, VALUES);

            LOOP_ASSERT(LINE, expected == buffer);

            bsl::vector<bdet_DatetimeTz> verify(&testAllocator);
            const char *endPos = 0;
            int rv = bdepu_ArrayParserImpUtil::
                     parseDatetimeTzArray(&endPos, &verify, &buffer.front());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, verify == VALUES);
        }

        bsl::string PREF = "xyz";
        expected.insert(expected.begin(), PREF.begin(), PREF.end());
        vector<char> buffer(&testAllocator);
        buffer.insert(buffer.end(), PREF.begin(), PREF.end());
        bdepu_ArrayParserImpUtil::generateDatetimeTzArray(&buffer, VALUES);
        ASSERT(expected == buffer);
      } break;
      case 25: {
        // --------------------------------------------------------------------
        // TESTING GENERATE TIMETZ ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateTimeTzArray(ArCh *buffer, cArTimez& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateTimeTzArray" << endl
                          << "===========================" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            int         d_hour;            // specification hour
            int         d_minute;          // specification minute
            int         d_second;          // specification second
            int         d_millisecond;     // specification millisecond
            int         d_tz;              // TZ offset
            const char *d_exp_p;           // expected result
        } DATA[] = {
            //line  hour  min  sec  milli      tz  exp
            //----  ----  ---  ---  -----  ------  ---------------
            { L_,    1,   2,   3,     4,        0, "01:02:03.004+0000"    },
            { L_,   12,  21,  32,    40,        0, "12:21:32.040+0000"    },
            { L_,   12,  21,  32,   417,        0, "12:21:32.417+0000"    },
            { L_,   23,  59,  59,     0,        0, "23:59:59.000+0000"    },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_TimeTz> VALUES(&testAllocator);
        bsl::vector<bsl::string> EXPECTED(&testAllocator);

        vector<char> expected(&testAllocator);
        expected.push_back('[');
        expected.push_back(' ');
        expected.push_back(']');

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE          = DATA[ti].d_lineNum;
            const int HOUR          = DATA[ti].d_hour;
            const int MINUTE        = DATA[ti].d_minute;
            const int SECOND        = DATA[ti].d_second;
            const int MILLISECOND   = DATA[ti].d_millisecond;
            const int OFFSET        = DATA[ti].d_tz;
            const char *const EXP   = DATA[ti].d_exp_p;

            const bdet_Time VALUED(HOUR, MINUTE, SECOND, MILLISECOND);
            const bdet_TimeTz VALUE(VALUED, OFFSET);
            VALUES.push_back(VALUE);

            const bsl::string EXPSTR(EXP, &testAllocator);
            expected.pop_back();
            expected.insert(expected.end(), EXPSTR.begin(), EXPSTR.end());
            expected.push_back(' ');
            expected.push_back(']');

            vector<char> buffer(&testAllocator);
            bdepu_ArrayParserImpUtil::generateTimeTzArray(&buffer, VALUES);

            LOOP_ASSERT(LINE, expected == buffer);

            bsl::vector<bdet_TimeTz> verify(&testAllocator);
            const char *endPos = 0;
            int rv = bdepu_ArrayParserImpUtil::
                     parseTimeTzArray(&endPos, &verify, &buffer.front());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, verify == VALUES);
        }

        bsl::string PREF = "xyz";
        expected.insert(expected.begin(), PREF.begin(), PREF.end());
        vector<char> buffer(&testAllocator);
        buffer.insert(buffer.end(), PREF.begin(), PREF.end());
        bdepu_ArrayParserImpUtil::generateTimeTzArray(&buffer, VALUES);
        ASSERT(expected == buffer);
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DATETZ ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateDateTzArray(ArCh *buffer, cArDatez& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDateTzArray" << endl
                          << "===========================" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            int         d_year;            // specification year
            int         d_month;           // specification month
            int         d_day;             // specification day
            int         d_tz;              // TZ offset
            const char *d_exp_p;           // expected result
        } DATA[] = {
            //line  year  month  day     tz  exp
            //----  ----  -----  ---  -----  -----------------
            { L_,   1989,    11,  23,     0, "1989/11/23+0000" },
            { L_,   1998,     6,  18,     0, "1998/06/18+0000" },
            { L_,   2000,    10,   7,     0, "2000/10/07+0000" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_DateTz> VALUES(&testAllocator);
        bsl::vector<bsl::string> EXPECTED(&testAllocator);

        vector<char> expected(&testAllocator);
        expected.push_back('[');
        expected.push_back(' ');
        expected.push_back(']');

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE          = DATA[ti].d_lineNum;
            const int YEAR          = DATA[ti].d_year;
            const int MONTH         = DATA[ti].d_month;
            const int DAY           = DATA[ti].d_day;
            const int OFFSET        = DATA[ti].d_tz;
            const char *const EXP   = DATA[ti].d_exp_p;

            const bdet_Date VALUED(YEAR, MONTH, DAY);
            const bdet_DateTz VALUE(VALUED, OFFSET);
            VALUES.push_back(VALUE);

            const bsl::string EXPSTR(EXP, &testAllocator);
            expected.pop_back();
            expected.insert(expected.end(), EXPSTR.begin(), EXPSTR.end());
            expected.push_back(' ');
            expected.push_back(']');

            vector<char> buffer(&testAllocator);
            bdepu_ArrayParserImpUtil::generateDateTzArray(&buffer, VALUES);

            LOOP_ASSERT(LINE, expected == buffer);

            bsl::vector<bdet_DateTz> verify(&testAllocator);
            const char *endPos = 0;
            int rv = bdepu_ArrayParserImpUtil::
                     parseDateTzArray(&endPos, &verify, &buffer.front());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, verify == VALUES);
        }

        bsl::string PREF = "xyz";
        expected.insert(expected.begin(), PREF.begin(), PREF.end());
        vector<char> buffer(&testAllocator);
        buffer.insert(buffer.end(), PREF.begin(), PREF.end());
        bdepu_ArrayParserImpUtil::generateDateTzArray(&buffer, VALUES);
        ASSERT(expected == buffer);
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING PARSE TIMETZ ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseTimeTzArray(cchar **endPos, ArTimez *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseTimeTzArray" << endl
                          << "========================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_fail;     // 1 for fail, 0 for success
            int         d_offset;   // expected number of parsed characters
        } DATA0[] = {
            //line  spec       fail  off
            //----  ---------  ----  ---
            { L_,   "",          1,    0 },
            { L_,   " ",         1,    0 },
            { L_,   "x",         1,    0 },
            { L_,   "0",         1,    0 },
            { L_,   "  ",        1,    0 },
            { L_,   " [",        1,    0 },
            { L_,   "[",         1,    1 },
            { L_,   "[ ",        1,    2 },
            { L_,   "[ 1",       1,    3 },
            { L_,   "[]",        0,    2 },
            { L_,   "[ ]",       0,    3 },
            { L_,   "[  ]",      0,    4 },
        };
        const int NUM_DATA0 = sizeof DATA0 / sizeof *DATA0;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_hour;     // expected return hour
            int         d_minute;   // expected return minute
            int         d_second;   // expected return second
            int         d_milli;    // expected return millisecond
            int         d_tz;       // expected return tz minutes
        } DATA1[] = {
            //LN   spec                hours   mins secs    ms      tz
            //---  --------------      -----   ---- ----  ----   -----
            { L_,  "7:01:32-1430",         7,     1,  32,    0,   -870 },
            { L_,  "08:24+0000",           8,    24,   0,    0,      0 },
            { L_,  "8:24+0330",            8,    24,   0,    0,    210 },
            { L_,  "8:24:00-1100",         8,    24,   0,    0,   -660 },
            { L_,  "8:24:00.0+0830",       8,    24,   0,    0,    510 },
            { L_,  "8:24:29.1-0000",       8,    24,  29,  100,      0 },
            { L_,  "08:24:29.1+0900",      8,    24,  29,  100,    540 },
            { L_,  "8:24:29.10+1045",      8,    24,  29,  100,    645 },
            { L_,  "04:31:00.100-0030",    4,    31,   0,  100,    -30 },
        };
        const int NUM_DATA1 = sizeof DATA1 / sizeof *DATA1;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_offset;   // expected number of parsed characters
        } DATA2[] = {
            //line  spec               off
            //----  ---------------    ---
            { L_,   "x",                 0 },
            { L_,   "1",                 1 },
            { L_,   "01",                2 },
            { L_,   "1:",                2 },
            { L_,   "08:",               3 },
            { L_,   "08: ",              3 },
            { L_,   "08:2",              4 },
            { L_,   "8:2",               3 },
            { L_,   "08:2 ",             4 },
            { L_,   "08:24:",            6 },
            { L_,   "08:2:24",           4 },
            { L_,   "08:24:2.",          7 },
            { L_,   "08:24:2.x",         7 },
            { L_,   "08:24:20.",         9 },
            { L_,   "08:24:20.?",        9 },
            { L_,   "28:44:20",          0 },
            { L_,   "08:64:20",          3 },
            { L_,   "08:24:20.2",       10 },
            { L_,   "08:24:20.2 ",      10 },
            { L_,   "08:24:20.2+",      11 },
            { L_,   "08:24:20.2-1",     12 },
            { L_,   "08:24:20.2-10",    13 },
            { L_,   "08:24:20.2+103",   14 },
            { L_,   "08:24:20.2+103x",  14 },
            { L_,   "08:24:20.2+8998",  11 },
        };
        const int NUM_DATA2 = sizeof DATA2 / sizeof *DATA2;

        bsl::vector<bsl::string> SPECS(&testAllocator);
        bsl::vector<bdet_TimeTz> VALUES(&testAllocator);

        bsl::vector<bdet_TimeTz> result(&testAllocator);

        const char *endPos = 0;

        for (int ti = 0; ti < NUM_DATA0 ; ++ti) {
            const int LINE         = DATA0[ti].d_lineNum;
            const char *const SPEC = DATA0[ti].d_spec_p;
            const int FAIL         = DATA0[ti].d_fail;
            const int NUM          = DATA0[ti].d_offset;

            int rv = bdepu_ArrayParserImpUtil::
                     parseTimeTzArray(&endPos, &result, SPEC);
            LOOP_ASSERT(LINE, FAIL == rv);
            LOOP_ASSERT(LINE, result.size() == 0);
            LOOP_ASSERT(LINE, SPEC + NUM == endPos);
        }

        for (int ti = 0; ti < NUM_DATA2 ; ++ti) {
            const int LINE         = DATA2[ti].d_lineNum;
            const char *const SPEC = DATA2[ti].d_spec_p;
            const int NUM          = DATA2[ti].d_offset;

            bsl::string tmpStr("[" , &testAllocator);
            tmpStr.append(SPEC);
            int rv = bdepu_ArrayParserImpUtil::
                     parseTimeTzArray(&endPos, &result, tmpStr.c_str());
            LOOP_ASSERT(LINE, 1 == rv);
            LOOP_ASSERT(LINE, result.size() == 0);
            LOOP_ASSERT(LINE,
                        tmpStr.c_str() + NUM + 1 == endPos);
        }

        for (int ti = 0; ti < NUM_DATA1 ; ++ti) {
            const int LINE         = DATA1[ti].d_lineNum;
            const char *const SPEC = DATA1[ti].d_spec_p;
            const int HOUR         = DATA1[ti].d_hour;
            const int MINS         = DATA1[ti].d_minute;
            const int SECS         = DATA1[ti].d_second;
            const int MILLI        = DATA1[ti].d_milli;
            const int OFFSET       = DATA1[ti].d_tz;
            const bdet_Time VALUET(HOUR, MINS, SECS, MILLI);
            const bdet_TimeTz VALUE(VALUET, OFFSET);

            SPECS.push_back(bsl::string(SPEC, &testAllocator));
            VALUES.push_back(VALUE);

            bsl::string baseStr("[", &testAllocator);
            for (int tj = 0; tj <= ti ; ++tj) {
                bsl::string tmpStr(baseStr, &testAllocator);
                tmpStr.append(" ");
                tmpStr.append(SPECS[ti - tj]);
                int rv = bdepu_ArrayParserImpUtil::
                         parseTimeTzArray(&endPos, &result, tmpStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE, tmpStr.c_str() + tmpStr.length() == endPos);

                baseStr.append(SPECS[ti - tj]);
                rv = bdepu_ArrayParserImpUtil::
                         parseTimeTzArray(&endPos, &result, baseStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE,
                            baseStr.c_str() + baseStr.length() == endPos);

                baseStr.append(" ");
                rv = bdepu_ArrayParserImpUtil::
                         parseTimeTzArray(&endPos, &result, baseStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE,
                            baseStr.c_str() + baseStr.length() == endPos);
            }

            for (int tj = 0; tj < NUM_DATA2 ; ++tj) {
                const int LINE2         = DATA2[tj].d_lineNum;
                const char *const SPEC2 = DATA2[tj].d_spec_p;
                const int NUM2          = DATA2[tj].d_offset;

                bsl::string tmpStr(baseStr, &testAllocator);
                tmpStr.append(SPEC2);
                int rv = bdepu_ArrayParserImpUtil::
                         parseTimeTzArray(&endPos, &result, tmpStr.c_str());
                LOOP2_ASSERT(LINE, LINE2, 1 == rv);
                LOOP2_ASSERT(LINE, LINE2, result.size() == ti + 1);
                LOOP2_ASSERT(LINE, LINE2,
                           tmpStr.c_str() + baseStr.length() + NUM2 == endPos);
            }

            baseStr.append("]");
            int rv = bdepu_ArrayParserImpUtil::
                         parseTimeTzArray(&endPos, &result, baseStr.c_str());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, baseStr.c_str() + baseStr.length() == endPos);
            for (int tj = 0; tj <= ti ; ++tj) {
                LOOP2_ASSERT(LINE, tj, VALUES[ti - tj] == result[tj]);
            }
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING PARSE DATETIMETZ ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseDatetimeTzArray(cchar **endPos, ArDTz *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseDatetimeTzArray" << endl
                          << "============================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_fail;     // 1 for fail, 0 for success
            int         d_offset;   // expected number of parsed characters
        } DATA0[] = {
            //line  spec       fail  off
            //----  ---------  ----  ---
            { L_,   "",          1,    0 },
            { L_,   " ",         1,    0 },
            { L_,   "x",         1,    0 },
            { L_,   "0",         1,    0 },
            { L_,   "  ",        1,    0 },
            { L_,   " [",        1,    0 },
            { L_,   "[",         1,    1 },
            { L_,   "[ ",        1,    2 },
            { L_,   "[ 1",       1,    3 },
            { L_,   "[]",        0,    2 },
            { L_,   "[ ]",       0,    3 },
            { L_,   "[  ]",      0,    4 },
        };
        const int NUM_DATA0 = sizeof DATA0 / sizeof *DATA0;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_year;     // expected return year
            int         d_month;    // expected return month
            int         d_day;      // expected return day
            int         d_hour;     // expected return hour
            int         d_minute;   // expected return minute
            int         d_second;   // expected return second
            int         d_milli;    // expected return millisecond
            int         d_tz;       // expected return tz minutes
        } DATA1[] = {
            //LN  spec                          year month  day
            //                                 hours  mins secs    ms     tz
            //--- ---------------               ---- -----  ---   ---   ----
            { L_, "19/1/2 7:01:32+0000",          19,    1,   2,
                                                   7,    1,  32,    0,     0 },
            { L_, "19/01/02_08:24-1430",          19,    1,   2,
                                                   8,   24,   0,    0,  -870 },
            { L_, "199/01/02 8:24-0100",         199,    1,   2,
                                                   8,   24,   0,    0,   -60 },
            { L_, "1993/1/02_8:24:29.1+1015",   1993,    1,   2,
                                                   8,   24,  29,  100,   615 },
            { L_, "1993/01/2 18:57:09.98+0400", 1993,    1,   2,
                                                  18,   57,   9,  980,   240 },
            { L_, "1993/1/2_04:31:00.100-0000", 1993,    1,   2,
                                                   4,   31,   0,  100,     0 },
            { L_, "2003/11/22_18:24:29+1234",   2003,   11,  22,
                                                  18,   24,  29,    0,   754 },
        };
        const int NUM_DATA1 = sizeof DATA1 / sizeof *DATA1;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_offset;   // expected number of parsed characters
        } DATA2[] = {
            //line  spec                             off
            //----  ---------------                  ---
            { L_,   "0",                               0 },
            { L_,   "1",                               1 },
            { L_,   "19",                              2 },
            { L_,   "199",                             3 },
            { L_,   "1993",                            4 },
            { L_,   "1993/",                           5 },
            { L_,   "1993/0",                          5 },
            { L_,   "1993/01",                         7 },
            { L_,   "1993/13",                         6 },
            { L_,   "1993/01/",                        8 },
            { L_,   "1993/01/0",                       8 },
            { L_,   "1993/01/32",                      9 },
            { L_,   "1993/02/31",                      9 },
            { L_,   "2011/05/03",                     10 },
            { L_,   "2011/05/03 ",                    11 },
            { L_,   "2011/05/03 x",                   11 },
            { L_,   "2011/05/03 1",                   12 },
            { L_,   "2011/05/03 01",                  13 },
            { L_,   "2011/05/03 1:",                  13 },
            { L_,   "2011/05/03 08:",                 14 },
            { L_,   "2011/05/03 08: ",                14 },
            { L_,   "2011/05/03 08:2",                15 },
            { L_,   "2011/05/03 8:2",                 14 },
            { L_,   "2011/05/03 08:2 ",               15 },
            { L_,   "2011/05/03 08:24:",              17 },
            { L_,   "2011/05/03 08:2:24",             15 },
            { L_,   "2011/05/03 08:24:2.",            18 },
            { L_,   "2011/05/03 08:24:2.x",           18 },
            { L_,   "2011/05/03 08:24:20.",           20 },
            { L_,   "2011/05/03 08:24:20.?",          20 },
            { L_,   "2011/05/03 28:44:20",            11 },
            { L_,   "2011/05/03 08:64:20",            14 },
            { L_,   "2011/05/03 08:24:20.2",          21 },
            { L_,   "2011/05/03 08:24:20.2 ",         21 },
            { L_,   "2011/05/03 08:24:20.2+",         22 },
            { L_,   "2011/05/03 08:24:20.2+0",        23 },
            { L_,   "2011/05/03 08:24:20.2-01",       24 },
            { L_,   "2011/05/03 08:24:20.2-010",      25 },
            { L_,   "2011/05/03 08:24:20.2-010 ",     25 },
        };
        const int NUM_DATA2 = sizeof DATA2 / sizeof *DATA2;

        bsl::vector<bsl::string> SPECS(&testAllocator);
        bsl::vector<bdet_DatetimeTz> VALUES(&testAllocator);

        bsl::vector<bdet_DatetimeTz> result(&testAllocator);

        const char *endPos = 0;

        for (int ti = 0; ti < NUM_DATA0 ; ++ti) {
            const int LINE         = DATA0[ti].d_lineNum;
            const char *const SPEC = DATA0[ti].d_spec_p;
            const int FAIL         = DATA0[ti].d_fail;
            const int NUM          = DATA0[ti].d_offset;

            int rv = bdepu_ArrayParserImpUtil::
                     parseDatetimeTzArray(&endPos, &result, SPEC);
            LOOP_ASSERT(LINE, FAIL == rv);
            LOOP_ASSERT(LINE, result.size() == 0);
            LOOP_ASSERT(LINE, SPEC + NUM == endPos);
        }

        for (int ti = 0; ti < NUM_DATA2 ; ++ti) {
            const int LINE         = DATA2[ti].d_lineNum;
            const char *const SPEC = DATA2[ti].d_spec_p;
            const int NUM          = DATA2[ti].d_offset;

            bsl::string tmpStr("[" , &testAllocator);
            tmpStr.append(SPEC);
            int rv = bdepu_ArrayParserImpUtil::
                     parseDatetimeTzArray(&endPos, &result, tmpStr.c_str());
            LOOP_ASSERT(LINE, 1 == rv);
            LOOP_ASSERT(LINE, result.size() == 0);
            LOOP_ASSERT(LINE,
                        tmpStr.c_str() + NUM + 1 == endPos);
        }

        for (int ti = 0; ti < NUM_DATA1 ; ++ti) {
            const int LINE         = DATA1[ti].d_lineNum;
            const char *const SPEC = DATA1[ti].d_spec_p;
            const int YEAR         = DATA1[ti].d_year;
            const int MONTH        = DATA1[ti].d_month;
            const int DAY          = DATA1[ti].d_day;
            const int HOUR         = DATA1[ti].d_hour;
            const int MINS         = DATA1[ti].d_minute;
            const int SECS         = DATA1[ti].d_second;
            const int MILLI        = DATA1[ti].d_milli;
            const bdet_Date VALUED(YEAR, MONTH, DAY);
            const bdet_Time VALUET(HOUR, MINS, SECS, MILLI);
            const int OFFSET       = DATA1[ti].d_tz;
            const bdet_Datetime VALUEDT(VALUED, VALUET);
            const bdet_DatetimeTz VALUE(VALUEDT, OFFSET);

            SPECS.push_back(bsl::string(SPEC, &testAllocator));
            VALUES.push_back(VALUE);

            bsl::string baseStr("[", &testAllocator);
            for (int tj = 0; tj <= ti ; ++tj) {
                bsl::string tmpStr(baseStr, &testAllocator);
                tmpStr.append(" ");
                tmpStr.append(SPECS[ti - tj]);
                int rv = bdepu_ArrayParserImpUtil::
                         parseDatetimeTzArray(&endPos,
                                              &result,
                                              tmpStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE, tmpStr.c_str() + tmpStr.length() == endPos);

                baseStr.append(SPECS[ti - tj]);
                rv = bdepu_ArrayParserImpUtil::
                      parseDatetimeTzArray(&endPos, &result, baseStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE,
                            baseStr.c_str() + baseStr.length() == endPos);

                baseStr.append(" ");
                rv = bdepu_ArrayParserImpUtil::
                       parseDatetimeTzArray(&endPos, &result, baseStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE,
                            baseStr.c_str() + baseStr.length() == endPos);
            }

            for (int tj = 0; tj < NUM_DATA2 ; ++tj) {
                const int LINE2         = DATA2[tj].d_lineNum;
                const char *const SPEC2 = DATA2[tj].d_spec_p;
                const int NUM2          = DATA2[tj].d_offset;

                bsl::string tmpStr(baseStr, &testAllocator);
                tmpStr.append(SPEC2);
                int rv = bdepu_ArrayParserImpUtil::
                         parseDatetimeTzArray(&endPos,
                                              &result,
                                              tmpStr.c_str());
                LOOP2_ASSERT(LINE, LINE2, 1 == rv);
                LOOP2_ASSERT(LINE, LINE2, result.size() == ti + 1);
                LOOP2_ASSERT(LINE, LINE2,
                           tmpStr.c_str() + baseStr.length() + NUM2 == endPos);
            }

            baseStr.append("]");
            int rv = bdepu_ArrayParserImpUtil::
                     parseDatetimeTzArray(&endPos, &result, baseStr.c_str());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, baseStr.c_str() + baseStr.length() == endPos);
            for (int tj = 0; tj <= ti ; ++tj) {
                LOOP2_ASSERT(LINE, tj, VALUES[ti - tj] == result[tj]);
            }
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING PARSE DATETZ ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseDateTzArray(cchar **endPos, ArDatez *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseDateTzArray" << endl
                          << "========================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_fail;     // 1 for fail, 0 for success
            int         d_offset;   // expected number of parsed characters
        } DATA0[] = {
            //line  spec       fail  off
            //----  ---------  ----  ---
            { L_,   "",          1,    0 },
            { L_,   " ",         1,    0 },
            { L_,   "x",         1,    0 },
            { L_,   "0",         1,    0 },
            { L_,   "  ",        1,    0 },
            { L_,   " [",        1,    0 },
            { L_,   "[",         1,    1 },
            { L_,   "[ ",        1,    2 },
            { L_,   "[ 1",       1,    3 },
            { L_,   "[]",        0,    2 },
            { L_,   "[ ]",       0,    3 },
            { L_,   "[  ]",      0,    4 },
        };
        const int NUM_DATA0 = sizeof DATA0 / sizeof *DATA0;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_year;     // expected return year
            int         d_month;    // expected return month
            int         d_day;      // expected return day
            int         d_tz;       // expected return tz minutes
        } DATA1[] = {
            //line  spec                       year  month  day     tz
            //----  -------------------        ----  -----  ---   ----
            { L_,   "19/1/2+0000",               19,     1,   2,     0 },
            { L_,   "19/01/02-1430",             19,     1,   2,  -870 },
            { L_,   "199/01/02-0100",           199,     1,   2,   -60 },
            { L_,   "1993/1/02+1015",          1993,     1,   2,   615 },
            { L_,   "1993/01/2+0400",          1993,     1,   2,   240 },
            { L_,   "1993/01/02-0000",         1993,     1,   2,     0 },
        };
        const int NUM_DATA1 = sizeof DATA1 / sizeof *DATA1;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_offset;   // expected number of parsed characters
        } DATA2[] = {
            //line  spec             off
            //----  ---------------  ---
            { L_,   "0",               0 },
            { L_,   "1",               1 },
            { L_,   "19",              2 },
            { L_,   "199",             3 },
            { L_,   "1993",            4 },
            { L_,   "1993/",           5 },
            { L_,   "1993/0",          5 },
            { L_,   "1993/01",         7 },
            { L_,   "1993/13",         6 },
            { L_,   "1993/01/",        8 },
            { L_,   "1993/01/0",       8 },
            { L_,   "1993/01/32",      9 },
            { L_,   "1993/02/31",      9 },
            { L_,   "1993/01/08 ",    10 },
            { L_,   "1993/01/08+",    11 },
            { L_,   "1993/01/08-",    11 },
            { L_,   "1993/01/08+0",   12 },
            { L_,   "1993/01/08-0",   12 },
            { L_,   "1993/01/08+04",  13 },
            { L_,   "1993/01/08-04",  13 },
            { L_,   "1993/01/08+040", 14 },
            { L_,   "1993/01/08-040", 14 },
        };
        const int NUM_DATA2 = sizeof DATA2 / sizeof *DATA2;

        bsl::vector<bsl::string> SPECS(&testAllocator);
        bsl::vector<bdet_DateTz> VALUES(&testAllocator);

        bsl::vector<bdet_DateTz> result(&testAllocator);

        const char *endPos = 0;

        for (int ti = 0; ti < NUM_DATA0 ; ++ti) {
            const int LINE         = DATA0[ti].d_lineNum;
            const char *const SPEC = DATA0[ti].d_spec_p;
            const int FAIL         = DATA0[ti].d_fail;
            const int NUM          = DATA0[ti].d_offset;

            int rv = bdepu_ArrayParserImpUtil::
                     parseDateTzArray(&endPos, &result, SPEC);
            LOOP_ASSERT(LINE, FAIL == rv);
            LOOP_ASSERT(LINE, result.size() == 0);
            LOOP_ASSERT(LINE, SPEC + NUM == endPos);
        }

        for (int ti = 0; ti < NUM_DATA2 ; ++ti) {
            const int LINE         = DATA2[ti].d_lineNum;
            const char *const SPEC = DATA2[ti].d_spec_p;
            const int NUM          = DATA2[ti].d_offset;

            bsl::string tmpStr("[" , &testAllocator);
            tmpStr.append(SPEC);
            int rv = bdepu_ArrayParserImpUtil::
                     parseDateTzArray(&endPos, &result, tmpStr.c_str());
            LOOP_ASSERT(LINE, 1 == rv);
            LOOP_ASSERT(LINE, result.size() == 0);
            LOOP_ASSERT(LINE, tmpStr.c_str() + NUM + 1 == endPos);
        }

        for (int ti = 0; ti < NUM_DATA1 ; ++ti) {
            const int LINE         = DATA1[ti].d_lineNum;
            const char *const SPEC = DATA1[ti].d_spec_p;
            const int YEAR         = DATA1[ti].d_year;
            const int MONTH        = DATA1[ti].d_month;
            const int DAY          = DATA1[ti].d_day;
            const bdet_Date VALUED(YEAR, MONTH, DAY);
            const int OFFSET       = DATA1[ti].d_tz;
            const bdet_DateTz VALUE(VALUED, OFFSET);

            SPECS.push_back(bsl::string(SPEC, &testAllocator));
            VALUES.push_back(VALUE);

            bsl::string baseStr("[", &testAllocator);
            for (int tj = 0; tj <= ti ; ++tj) {
                bsl::string tmpStr(baseStr, &testAllocator);
                tmpStr.append(" ");
                tmpStr.append(SPECS[ti - tj]);
                int rv = bdepu_ArrayParserImpUtil::
                         parseDateTzArray(&endPos, &result, tmpStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE, tmpStr.c_str() + tmpStr.length() == endPos);

                baseStr.append(SPECS[ti - tj]);
                rv = bdepu_ArrayParserImpUtil::
                         parseDateTzArray(&endPos, &result, baseStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE,
                            baseStr.c_str() + baseStr.length() == endPos);

                baseStr.append(" ");
                rv = bdepu_ArrayParserImpUtil::
                         parseDateTzArray(&endPos, &result, baseStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE,
                            baseStr.c_str() + baseStr.length() == endPos);
            }

            for (int tj = 0; tj < NUM_DATA2 ; ++tj) {
                const int LINE2         = DATA2[tj].d_lineNum;
                const char *const SPEC2 = DATA2[tj].d_spec_p;
                const int NUM2          = DATA2[tj].d_offset;

                bsl::string tmpStr(baseStr, &testAllocator);
                tmpStr.append(SPEC2);
                int rv = bdepu_ArrayParserImpUtil::
                         parseDateTzArray(&endPos, &result, tmpStr.c_str());
                LOOP2_ASSERT(LINE, LINE2, 1 == rv);
                LOOP2_ASSERT(LINE, LINE2, result.size() == ti + 1);
                LOOP2_ASSERT(LINE, LINE2,
                           tmpStr.c_str() + baseStr.length() + NUM2 == endPos);
            }

            baseStr.append("]");
            int rv = bdepu_ArrayParserImpUtil::
                         parseDateTzArray(&endPos, &result, baseStr.c_str());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, baseStr.c_str() + baseStr.length() == endPos);
            for (int tj = 0; tj <= ti ; ++tj) {
                LOOP2_ASSERT(LINE, tj, VALUES[ti - tj] == result[tj]);
            }
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING GENERATE TIME ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateTimeArray(ArCh *buffer, cArTime& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDatetimeArray" << endl
                          << "=============================" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            int         d_hour;            // specification hour
            int         d_minute;          // specification minute
            int         d_second;          // specification second
            int         d_millisecond;     // specification millisecond
            const char *d_exp_p;           // expected result
        } DATA[] = {
            //line  hour  min  sec  milli  exp
            //----  ----  ---  ---  -----  ----------------------
            { L_,    1,   2,   3,     4, "01:02:03.004"         },
            { L_,   12,  21,  32,    40, "12:21:32.040"         },
            { L_,   12,  21,  32,   417, "12:21:32.417"         },
            { L_,   23,  59,  59,     0, "23:59:59.000"         },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_Time> VALUES(&testAllocator);
        bsl::vector<bsl::string> EXPECTED(&testAllocator);

        vector<char> expected(&testAllocator);
        expected.push_back('[');
        expected.push_back(' ');
        expected.push_back(']');

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE          = DATA[ti].d_lineNum;
            const int HOUR          = DATA[ti].d_hour;
            const int MINUTE        = DATA[ti].d_minute;
            const int SECOND        = DATA[ti].d_second;
            const int MILLISECOND   = DATA[ti].d_millisecond;
            const char *const EXP   = DATA[ti].d_exp_p;

            const bdet_Time VALUE(HOUR, MINUTE, SECOND, MILLISECOND);
            VALUES.push_back(VALUE);

            const bsl::string EXPSTR(EXP, &testAllocator);
            expected.pop_back();
            expected.insert(expected.end(), EXPSTR.begin(), EXPSTR.end());
            expected.push_back(' ');
            expected.push_back(']');

            vector<char> buffer(&testAllocator);
            bdepu_ArrayParserImpUtil::generateTimeArray(&buffer, VALUES);

            LOOP_ASSERT(LINE, expected == buffer);

            bsl::vector<bdet_Time> verify(&testAllocator);
            const char *endPos = 0;
            int rv = bdepu_ArrayParserImpUtil::
                     parseTimeArray(&endPos, &verify, &buffer.front());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, verify == VALUES);
        }

        bsl::string PREF = "xyz";
        expected.insert(expected.begin(), PREF.begin(), PREF.end());
        vector<char> buffer(&testAllocator);
        buffer.insert(buffer.end(), PREF.begin(), PREF.end());
        bdepu_ArrayParserImpUtil::generateTimeArray(&buffer, VALUES);
        ASSERT(expected == buffer);
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING GENERATE STRING ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateStringArray(ArCh *buffer, cArStr& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateStringArray" << endl
                          << "===========================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_init_p;   // specification initial result value
                const char *d_result_p; // expected result
            } DATA[] = {
                //line  spec                   init    result
                //----  ---------------------  ------  ---------------
                { L_,   "[]",                  "",     "[ ]"           },
                { L_,   "[]",                  "x",    "x[ ]"          },
                { L_,   "[ a ]",               "",     "[ a ]"         },
                { L_,   "[ a ]",               "x",    "x[ a ]"        },
                { L_,   "[ a b ]",             "",     "[ a b ]"       },
                { L_,   "[ a b ]",             "x",    "x[ a b ]"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const INIT = DATA[ti].d_init_p;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct specification value
                const char *endPos;
                vector<bsl::string> spec(&testAllocator);
                LOOP_ASSERT(LINE, 0 == bdepu_ArrayParserImpUtil::
                                       parseStringArray(&endPos, &spec, SPEC));

                // construct result value
                const bsl::string INITSTR = INIT;
                vector<char> result(&testAllocator);
                result.insert(result.end(), INITSTR.begin(), INITSTR.end());
                bdepu_ArrayParserImpUtil::generateStringArray(&result, spec);
                LOOP_ASSERT(LINE, strlen(EXP) == result.size());
                LOOP_ASSERT(LINE,
                            0 == strncmp(EXP, &result.front(), result.size()));
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING GENERATE SHORT ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateShortArray(ArCh *buffer, cArSh& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateShortArray" << endl
                          << "==========================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_init_p;   // specification initial result value
                const char *d_result_p; // expected result
            } DATA[] = {
                //line  spec                   init    result
                //----  ---------------------  ------  ---------------
                { L_,   "[]",                  "",     "[ ]"           },
                { L_,   "[]",                  "x",    "x[ ]"          },
                { L_,   "[ 1 ]",               "",     "[ 1 ]"         },
                { L_,   "[ 1 ]",               "x",    "x[ 1 ]"        },
                { L_,   "[ 1 2 ]",             "",     "[ 1 2 ]"       },
                { L_,   "[ 1 2 ]",             "x",    "x[ 1 2 ]"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const INIT = DATA[ti].d_init_p;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct specification value
                const char *endPos;
                vector<short> spec(&testAllocator);
                LOOP_ASSERT(LINE, 0 == bdepu_ArrayParserImpUtil::
                                        parseShortArray(&endPos, &spec, SPEC));

                // construct result value
                const bsl::string INITSTR = INIT;
                vector<char> result(&testAllocator);
                result.insert(result.end(), INITSTR.begin(), INITSTR.end());
                bdepu_ArrayParserImpUtil::generateShortArray(&result, spec);
                LOOP_ASSERT(LINE, strlen(EXP) == result.size());
                LOOP_ASSERT(LINE,
                            0 == strncmp(EXP, &result.front(), result.size()));
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING GENERATE INT64 ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateInt64Array(ArCh *buffer, cArInt64& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateInt64Array" << endl
                          << "==========================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_init_p;   // specification initial result value
                const char *d_result_p; // expected result
            } DATA[] = {
                //line  spec                   init    result
                //----  ---------------------  ------  ---------------
                { L_,   "[]",                  "",     "[ ]"           },
                { L_,   "[]",                  "x",    "x[ ]"          },
                { L_,   "[ 1 ]",               "",     "[ 1 ]"         },
                { L_,   "[ 1 ]",               "x",    "x[ 1 ]"        },
                { L_,   "[ 1 2 ]",             "",     "[ 1 2 ]"       },
                { L_,   "[ 1 2 ]",             "x",    "x[ 1 2 ]"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const INIT = DATA[ti].d_init_p;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct specification value
                const char *endPos;
                vector<bsls_PlatformUtil::Int64> spec(&testAllocator);
                LOOP_ASSERT(LINE, 0 == bdepu_ArrayParserImpUtil::
                                        parseInt64Array(&endPos, &spec, SPEC));

                // construct result value
                const bsl::string INITSTR = INIT;
                vector<char> result(&testAllocator);
                result.insert(result.end(), INITSTR.begin(), INITSTR.end());
                bdepu_ArrayParserImpUtil::generateInt64Array(&result, spec);
                LOOP_ASSERT(LINE, strlen(EXP) == result.size());
                LOOP_ASSERT(LINE,
                            0 == strncmp(EXP, &result.front(), result.size()));
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING GENERATE INT ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateIntArray(ArCh *buffer, cArInt& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateIntArray" << endl
                          << "========================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_init_p;   // specification initial result value
                const char *d_result_p; // expected result
            } DATA[] = {
                //line  spec                   init    result
                //----  ---------------------  ------  ---------------
                { L_,   "[]",                  "",     "[ ]"           },
                { L_,   "[]",                  "x",    "x[ ]"          },
                { L_,   "[ 1 ]",               "",     "[ 1 ]"         },
                { L_,   "[ 1 ]",               "x",    "x[ 1 ]"        },
                { L_,   "[ 1 2 ]",             "",     "[ 1 2 ]"       },
                { L_,   "[ 1 2 ]",             "x",    "x[ 1 2 ]"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const INIT = DATA[ti].d_init_p;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct specification value
                const char *endPos;
                vector<int> spec(&testAllocator);
                LOOP_ASSERT(LINE, 0 == bdepu_ArrayParserImpUtil::
                                          parseIntArray(&endPos, &spec, SPEC));

                // construct result value
                const bsl::string INITSTR = INIT;
                vector<char> result(&testAllocator);
                result.insert(result.end(), INITSTR.begin(), INITSTR.end());
                bdepu_ArrayParserImpUtil::generateIntArray(&result, spec);
                LOOP_ASSERT(LINE, strlen(EXP) == result.size());
                LOOP_ASSERT(LINE,
                            0 == strncmp(EXP, &result.front(), result.size()));
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING GENERATE FLOAT ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateFloatArray(ArCh *buffer, cArFl& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateFloatArray" << endl
                          << "==========================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_init_p;   // specification initial result value
                const char *d_result_p; // expected result
            } DATA[] = {
                //line  spec                   init    result
                //----  ---------------------  ------  ---------------
                { L_,   "[]",                  "",     "[ ]"           },
                { L_,   "[]",                  "x",    "x[ ]"          },
                { L_,   "[ 1 ]",               "",     "[ 1 ]"         },
                { L_,   "[ 1 ]",               "x",    "x[ 1 ]"        },
                { L_,   "[ 1 2 ]",             "",     "[ 1 2 ]"       },
                { L_,   "[ 1 2 ]",             "x",    "x[ 1 2 ]"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const INIT = DATA[ti].d_init_p;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct specification value
                const char *endPos;
                vector<float> spec(&testAllocator);
                LOOP_ASSERT(LINE, 0 == bdepu_ArrayParserImpUtil::
                                        parseFloatArray(&endPos, &spec, SPEC));

                // construct result value
                const bsl::string INITSTR = INIT;
                vector<char> result(&testAllocator);
                result.insert(result.end(), INITSTR.begin(), INITSTR.end());
                bdepu_ArrayParserImpUtil::generateFloatArray(&result, spec);
                LOOP_ASSERT(LINE, strlen(EXP) == result.size());
                LOOP_ASSERT(LINE,
                            0 == strncmp(EXP, &result.front(), result.size()));
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DOUBLE ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateDoubleArray(ArCh *buffer, cArDb& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDoubleArray" << endl
                          << "===========================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_init_p;   // specification initial result value
                const char *d_result_p; // expected result
            } DATA[] = {
                //line  spec                   init    result
                //----  ---------------------  ------  ---------------
                { L_,   "[]",                  "",     "[ ]"           },
                { L_,   "[]",                  "x",    "x[ ]"          },
                { L_,   "[ 1 ]",               "",     "[ 1 ]"         },
                { L_,   "[ 1 ]",               "x",    "x[ 1 ]"        },
                { L_,   "[ 1 2 ]",             "",     "[ 1 2 ]"       },
                { L_,   "[ 1 2 ]",             "x",    "x[ 1 2 ]"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const INIT = DATA[ti].d_init_p;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct specification value
                const char *endPos;
                vector<double> spec(&testAllocator);
                LOOP_ASSERT(LINE, 0 == bdepu_ArrayParserImpUtil::
                                       parseDoubleArray(&endPos, &spec, SPEC));

                // construct result value
                const bsl::string INITSTR = INIT;
                vector<char> result(&testAllocator);
                result.insert(result.end(), INITSTR.begin(), INITSTR.end());
                bdepu_ArrayParserImpUtil::generateDoubleArray(&result, spec);
                LOOP_ASSERT(LINE, strlen(EXP) == result.size());
                LOOP_ASSERT(LINE,
                            0 == strncmp(EXP, &result.front(), result.size()));
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DATETIME ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateDatetimeArray(ArCh *buffer, cArDT& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDatetimeArray" << endl
                          << "=============================" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            int         d_year;            // specification year
            int         d_month;           // specification month
            int         d_day;             // specification day
            int         d_hour;            // specification hour
            int         d_minute;          // specification minute
            int         d_second;          // specification second
            int         d_millisecond;     // specification millisecond
            const char *d_exp_p;           // expected result
        } DATA[] = {
            // LN year MM  DD  HH  MM  SS  MS  exp
            //--- ---- --  --  --  --  --  ---
            { L_,    1,  2,  3,  1,  2,  3,   4,
                                        "0001/02/03 01:02:03.004" },
            { L_,   10,  2,  3, 12, 21, 32,  40,
                                        "0010/02/03 12:21:32.040" },
            { L_,  100,  2,  3, 12, 21, 32, 417,
                                        "0100/02/03 12:21:32.417" },
            { L_, 1000,  2,  3, 23, 59, 59,   0,
                                        "1000/02/03 23:59:59.000" },
            { L_, 9999,  2,  3, 12, 21, 32,  40,
                                        "9999/02/03 12:21:32.040" },
            { L_, 9999, 12, 31,  1,  2,  3,   4,
                                        "9999/12/31 01:02:03.004" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_Datetime> VALUES(&testAllocator);
        bsl::vector<bsl::string> EXPECTED(&testAllocator);

        vector<char> expected(&testAllocator);
        expected.push_back('[');
        expected.push_back(' ');
        expected.push_back(']');

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE          = DATA[ti].d_lineNum;
            const int YEAR          = DATA[ti].d_year;
            const int MONTH         = DATA[ti].d_month;
            const int DAY           = DATA[ti].d_day;
            const int HOUR          = DATA[ti].d_hour;
            const int MINUTE        = DATA[ti].d_minute;
            const int SECOND        = DATA[ti].d_second;
            const int MILLISECOND   = DATA[ti].d_millisecond;
            const char *const EXP   = DATA[ti].d_exp_p;

            const bdet_Datetime VALUE(YEAR,
                                      MONTH,
                                      DAY,
                                      HOUR,
                                      MINUTE,
                                      SECOND,
                                      MILLISECOND);
            VALUES.push_back(VALUE);

            const bsl::string EXPSTR(EXP, &testAllocator);
            expected.pop_back();
            expected.insert(expected.end(), EXPSTR.begin(), EXPSTR.end());
            expected.push_back(' ');
            expected.push_back(']');

            vector<char> buffer(&testAllocator);
            bdepu_ArrayParserImpUtil::generateDatetimeArray(&buffer, VALUES);

            LOOP_ASSERT(LINE, expected == buffer);

            bsl::vector<bdet_Datetime> verify(&testAllocator);
            const char *endPos = 0;
            int rv = bdepu_ArrayParserImpUtil::
                     parseDatetimeArray(&endPos, &verify, &buffer.front());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, verify == VALUES);
        }

        bsl::string PREF = "xyz";
        expected.insert(expected.begin(), PREF.begin(), PREF.end());
        vector<char> buffer(&testAllocator);
        buffer.insert(buffer.end(), PREF.begin(), PREF.end());
        bdepu_ArrayParserImpUtil::generateDatetimeArray(&buffer, VALUES);
        ASSERT(expected == buffer);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DATE ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateDateArray(ArCh *buffer, cArDate& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDateArray" << endl
                          << "=========================" << endl;

        static const struct {
            int         d_lineNum;         // source line number
            int         d_year;            // specification year
            int         d_month;           // specification month
            int         d_day;             // specification day
            const char *d_exp_p;           // expected result
        } DATA[] = {
            //line  year  month  day  exp
            //----  ----  -----  ---  -----------------------
            { L_,   1989,    11,  23, "1989/11/23"            },
            { L_,   1998,     6,  18, "1998/06/18"            },
            { L_,   2000,    10,   7, "2000/10/07"            },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_Date> VALUES(&testAllocator);
        bsl::vector<bsl::string> EXPECTED(&testAllocator);

        vector<char> expected(&testAllocator);
        expected.push_back('[');
        expected.push_back(' ');
        expected.push_back(']');

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE          = DATA[ti].d_lineNum;
            const int YEAR          = DATA[ti].d_year;
            const int MONTH         = DATA[ti].d_month;
            const int DAY           = DATA[ti].d_day;
            const char *const EXP   = DATA[ti].d_exp_p;

            const bdet_Date VALUE(YEAR, MONTH, DAY);
            VALUES.push_back(VALUE);

            const bsl::string EXPSTR(EXP, &testAllocator);
            expected.pop_back();
            expected.insert(expected.end(), EXPSTR.begin(), EXPSTR.end());
            expected.push_back(' ');
            expected.push_back(']');

            vector<char> buffer(&testAllocator);
            bdepu_ArrayParserImpUtil::generateDateArray(&buffer, VALUES);

            LOOP_ASSERT(LINE, expected == buffer);

            bsl::vector<bdet_Date> verify(&testAllocator);
            const char *endPos = 0;
            int rv = bdepu_ArrayParserImpUtil::
                     parseDateArray(&endPos, &verify, &buffer.front());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, verify == VALUES);
        }

        bsl::string PREF = "xyz";
        expected.insert(expected.begin(), PREF.begin(), PREF.end());
        vector<char> buffer(&testAllocator);
        buffer.insert(buffer.end(), PREF.begin(), PREF.end());
        bdepu_ArrayParserImpUtil::generateDateArray(&buffer, VALUES);
        ASSERT(expected == buffer);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING GENERATE CHAR ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateCharArray(ArCh *buffer, cArCh& val);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateCharArray" << endl
                          << "=========================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                const char *d_init_p;   // specification initial result value
                const char *d_result_p; // expected result
            } DATA[] = {
                //line  spec                   init    result
                //----  ---------------------  ------  ---------------
                { L_,   "[]",                  "",     "[ ]"           },
                { L_,   "[]",                  "x",    "x[ ]"          },
                { L_,   "[ 'a' ]",             "",     "[ 'a' ]"       },
                { L_,   "[ 'a' ]",             "x",    "x[ 'a' ]"      },
                { L_,   "[ 'a' 'b' ]",         "",     "[ 'a' 'b' ]"   },
                { L_,   "[ 'a' 'b' ]",         "x",    "x[ 'a' 'b' ]"  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const char *const INIT = DATA[ti].d_init_p;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct specification value
                const char *endPos;
                vector<char> spec(&testAllocator);
                LOOP_ASSERT(LINE, 0 == bdepu_ArrayParserImpUtil::
                                         parseCharArray(&endPos, &spec, SPEC));

                // construct result value
                const bsl::string INITSTR = INIT;
                vector<char> result(&testAllocator);
                result.insert(result.end(), INITSTR.begin(), INITSTR.end());
                bdepu_ArrayParserImpUtil::generateCharArray(&result, spec);
                LOOP_ASSERT(LINE, strlen(EXP) == result.size());
                LOOP_ASSERT(LINE,
                            0 == strncmp(EXP, &result.front(), result.size()));
            }
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING PARSE TIME ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseTimeArray(cchar **endPos, ArTime *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseTimeArray" << endl
                          << "======================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_fail;     // 1 for fail, 0 for success
            int         d_offset;   // expected number of parsed characters
        } DATA0[] = {
            //line  spec       fail  off
            //----  ---------  ----  ---
            { L_,   "",          1,    0 },
            { L_,   " ",         1,    0 },
            { L_,   "x",         1,    0 },
            { L_,   "0",         1,    0 },
            { L_,   "  ",        1,    0 },
            { L_,   " [",        1,    0 },
            { L_,   "[",         1,    1 },
            { L_,   "[ ",        1,    2 },
            { L_,   "[ 1",       1,    3 },
            { L_,   "[]",        0,    2 },
            { L_,   "[ ]",       0,    3 },
            { L_,   "[  ]",      0,    4 },
        };
        const int NUM_DATA0 = sizeof DATA0 / sizeof *DATA0;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_hour;     // expected return hour
            int         d_minute;   // expected return minute
            int         d_second;   // expected return second
            int         d_milli;    // expected return millisecond
        } DATA1[] = {
            //line  spec            hours   mins secs    ms
            //----  --------------  -----   ---- ----  ----
            { L_,   "7:01:32",          7,     1,  32,    0 },
            { L_,   "08:24",            8,    24,   0,    0 },
            { L_,   "8:24",             8,    24,   0,    0 },
            { L_,   "8:24:00",          8,    24,   0,    0 },
            { L_,   "8:24:00.0",        8,    24,   0,    0 },
            { L_,   "8:24:29.1",        8,    24,  29,  100 },
            { L_,   "08:24:29.1",       8,    24,  29,  100 },
            { L_,   "8:24:29.10",       8,    24,  29,  100 },
            { L_,   "04:31:00.100",     4,    31,   0,  100 },
        };
        const int NUM_DATA1 = sizeof DATA1 / sizeof *DATA1;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_offset;   // expected number of parsed characters
        } DATA2[] = {
            //line  spec             off
            //----  ---------------  ---
            { L_,   "x",               0 },
            { L_,   "1",               1 },
            { L_,   "01",              2 },
            { L_,   "1:",              2 },
            { L_,   "08:",             3 },
            { L_,   "08: ",            3 },
            { L_,   "08:2",            4 },
            { L_,   "8:2",             3 },
            { L_,   "08:2 ",           4 },
            { L_,   "08:24:",          6 },
            { L_,   "08:2:24",         4 },
            { L_,   "08:24:2.",        7 },
            { L_,   "08:24:2.x",       7 },
            { L_,   "08:24:20.",       9 },
            { L_,   "08:24:20.?",      9 },
            { L_,   "28:44:20",        0 },
            { L_,   "08:64:20",        3 },
        };
        const int NUM_DATA2 = sizeof DATA2 / sizeof *DATA2;

        bsl::vector<bsl::string> SPECS(&testAllocator);
        bsl::vector<bdet_Time> VALUES(&testAllocator);

        bsl::vector<bdet_Time> result(&testAllocator);

        const char *endPos = 0;

        for (int ti = 0; ti < NUM_DATA0 ; ++ti) {
            const int LINE         = DATA0[ti].d_lineNum;
            const char *const SPEC = DATA0[ti].d_spec_p;
            const int FAIL         = DATA0[ti].d_fail;
            const int NUM          = DATA0[ti].d_offset;

            int rv = bdepu_ArrayParserImpUtil::
                     parseTimeArray(&endPos, &result, SPEC);
            LOOP_ASSERT(LINE, FAIL == rv);
            LOOP_ASSERT(LINE, result.size() == 0);
            LOOP_ASSERT(LINE, SPEC + NUM == endPos);
        }

        for (int ti = 0; ti < NUM_DATA2 ; ++ti) {
            const int LINE         = DATA2[ti].d_lineNum;
            const char *const SPEC = DATA2[ti].d_spec_p;
            const int NUM          = DATA2[ti].d_offset;

            bsl::string tmpStr("[" , &testAllocator);
            tmpStr.append(SPEC);
            int rv = bdepu_ArrayParserImpUtil::
                     parseTimeArray(&endPos, &result, tmpStr.c_str());
            LOOP_ASSERT(LINE, 1 == rv);
            LOOP_ASSERT(LINE, result.size() == 0);
            LOOP_ASSERT(LINE,
                        tmpStr.c_str() + NUM + 1 == endPos);
        }

        for (int ti = 0; ti < NUM_DATA1 ; ++ti) {
            const int LINE         = DATA1[ti].d_lineNum;
            const char *const SPEC = DATA1[ti].d_spec_p;
            const int HOUR         = DATA1[ti].d_hour;
            const int MINS         = DATA1[ti].d_minute;
            const int SECS         = DATA1[ti].d_second;
            const int MILLI        = DATA1[ti].d_milli;
            const bdet_Time VALUE(HOUR, MINS, SECS, MILLI);

            SPECS.push_back(bsl::string(SPEC, &testAllocator));
            VALUES.push_back(VALUE);

            bsl::string baseStr("[", &testAllocator);
            for (int tj = 0; tj <= ti ; ++tj) {
                bsl::string tmpStr(baseStr, &testAllocator);
                tmpStr.append(" ");
                tmpStr.append(SPECS[ti - tj]);
                int rv = bdepu_ArrayParserImpUtil::
                         parseTimeArray(&endPos, &result, tmpStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE, tmpStr.c_str() + tmpStr.length() == endPos);

                baseStr.append(SPECS[ti - tj]);
                rv = bdepu_ArrayParserImpUtil::
                         parseTimeArray(&endPos, &result, baseStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE,
                            baseStr.c_str() + baseStr.length() == endPos);

                baseStr.append(" ");
                rv = bdepu_ArrayParserImpUtil::
                         parseTimeArray(&endPos, &result, baseStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE,
                            baseStr.c_str() + baseStr.length() == endPos);
            }

            for (int tj = 0; tj < NUM_DATA2 ; ++tj) {
                const int LINE2         = DATA2[tj].d_lineNum;
                const char *const SPEC2 = DATA2[tj].d_spec_p;
                const int NUM2          = DATA2[tj].d_offset;

                bsl::string tmpStr(baseStr, &testAllocator);
                tmpStr.append(SPEC2);
                int rv = bdepu_ArrayParserImpUtil::
                         parseTimeArray(&endPos, &result, tmpStr.c_str());
                LOOP2_ASSERT(LINE, LINE2, 1 == rv);
                LOOP2_ASSERT(LINE, LINE2, result.size() == ti + 1);
                LOOP2_ASSERT(LINE, LINE2,
                           tmpStr.c_str() + baseStr.length() + NUM2 == endPos);
            }

            baseStr.append("]");
            int rv = bdepu_ArrayParserImpUtil::
                         parseTimeArray(&endPos, &result, baseStr.c_str());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, baseStr.c_str() + baseStr.length() == endPos);
            for (int tj = 0; tj <= ti ; ++tj) {
                LOOP2_ASSERT(LINE, tj, VALUES[ti - tj] == result[tj]);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING PARSE STRING ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseStringArray(cchar **endPos, ArStr *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseStringArray" << endl
                          << "========================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_fail;     // parsing expected to fail indicator
                int         d_offset;   // expected number of parsed characters
                const char *d_result_p; // expected result specification string
            } DATA[] = {
                //line  spec                   fail  offset  result
                //----  ---------------------  ----  ------  ---------------
                { L_,   "",                       1,      0, ""              },
                { L_,   "[",                      1,      1, ""              },
                { L_,   "]",                      1,      0, ""              },
                { L_,   "[]",                     0,      2, ""              },
                { L_,   "[ ]",                    0,      3, ""              },
                { L_,   "[a]",                    1,      3, "a]"            },
                { L_,   "[  ]",                   0,      4, ""              },
                { L_,   "[ a]",                   1,      4, "a]"            },
                { L_,   "[a ]",                   0,      4, "a"             },
                { L_,   "[  a]",                  1,      5, "a]"            },
                { L_,   "[ a ]",                  0,      5, "a"             },
                { L_,   "[a  ]",                  0,      5, "a"             },
                { L_,   "[a b]",                  1,      5, "a b]"          },
                { L_,   "[ a b]",                 1,      6, "a b]"          },
                { L_,   "[a  b]",                 1,      6, "a b]"          },
                { L_,   "[a b ]",                 0,      6, "a b"           },
                { L_,   "[ a b ]",                0,      7, "a b"           },
                { L_,   "[ a b c ]",              0,      9, "a b c"         },

                // TBD need vectors to test internal parse failure
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct expected value
                vector<bsl::string> expResult(&testAllocator);
                {
                    const char *endPos = EXP;
                    bsl::string value;
                    while (*endPos) {
                        LOOP_ASSERT(LINE,
                                    0 == bdepu_TypesParserImpUtil::
                                         parseString(&endPos, &value, endPos));
                        expResult.push_back(value);
                        if (' ' == *endPos) {
                            ++endPos;
                        }
                    }
                }

                vector<bsl::string> result(&testAllocator);
                const char *endPos = 0;
                int rv = bdepu_ArrayParserImpUtil::
                                      parseStringArray(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE, expResult == result);
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING PARSE SHORT ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseShortArray(cchar **endPos, ArSh *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseShortArray" << endl
                          << "=======================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_fail;     // parsing expected to fail indicator
                int         d_offset;   // expected number of parsed characters
                const char *d_result_p; // expected result specification string
            } DATA[] = {
                //line  spec                   fail  offset  result
                //----  ---------------------  ----  ------  ---------------
                { L_,   "",                       1,      0, ""              },
                { L_,   "[",                      1,      1, ""              },
                { L_,   "]",                      1,      0, ""              },
                { L_,   "[]",                     0,      2, ""              },
                { L_,   "[ ]",                    0,      3, ""              },
                { L_,   "[1]",                    0,      3, "1"             },
                { L_,   "[  ]",                   0,      4, ""              },
                { L_,   "[ 1]",                   0,      4, "1"             },
                { L_,   "[1 ]",                   0,      4, "1"             },
                { L_,   "[  1]",                  0,      5, "1"             },
                { L_,   "[ 1 ]",                  0,      5, "1"             },
                { L_,   "[1  ]",                  0,      5, "1"             },
                { L_,   "[1 2]",                  0,      5, "1 2"           },
                { L_,   "[ 1 2]",                 0,      6, "1 2"           },
                { L_,   "[1  2]",                 0,      6, "1 2"           },
                { L_,   "[1 2 ]",                 0,      6, "1 2"           },
                { L_,   "[ 1 2 ]",                0,      7, "1 2"           },
                { L_,   "[ 1 2 3 ]",              0,      9, "1 2 3"         },

                // TBD need vectors to test internal parse failure
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct expected value
                vector<short> expResult(&testAllocator);
                {
                    const char *endPos = EXP;
                    short value = 0;
                    while (*endPos) {
                        LOOP_ASSERT(LINE,
                                    0 == bdepu_TypesParserImpUtil::
                                         parseShort(&endPos, &value, endPos));
                        expResult.push_back(value);
                        if (' ' == *endPos) {
                            ++endPos;
                        }
                    }
                }

                vector<short> result(&testAllocator);
                const char *endPos = 0;
                int rv = bdepu_ArrayParserImpUtil::
                                      parseShortArray(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE, expResult == result);
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING PARSE INT64 ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseInt64Array(cchar **endPos, ArInt64 *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseInt64Array" << endl
                          << "=======================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_fail;     // parsing expected to fail indicator
                int         d_offset;   // expected number of parsed characters
                const char *d_result_p; // expected result specification string
            } DATA[] = {
                //line  spec                   fail  offset  result
                //----  ---------------------  ----  ------  ---------------
                { L_,   "",                       1,      0, ""              },
                { L_,   "[",                      1,      1, ""              },
                { L_,   "]",                      1,      0, ""              },
                { L_,   "[]",                     0,      2, ""              },
                { L_,   "[ ]",                    0,      3, ""              },
                { L_,   "[1]",                    0,      3, "1"             },
                { L_,   "[  ]",                   0,      4, ""              },
                { L_,   "[ 1]",                   0,      4, "1"             },
                { L_,   "[1 ]",                   0,      4, "1"             },
                { L_,   "[  1]",                  0,      5, "1"             },
                { L_,   "[ 1 ]",                  0,      5, "1"             },
                { L_,   "[1  ]",                  0,      5, "1"             },
                { L_,   "[1 2]",                  0,      5, "1 2"           },
                { L_,   "[ 1 2]",                 0,      6, "1 2"           },
                { L_,   "[1  2]",                 0,      6, "1 2"           },
                { L_,   "[1 2 ]",                 0,      6, "1 2"           },
                { L_,   "[ 1 2 ]",                0,      7, "1 2"           },
                { L_,   "[ 1 2 3 ]",              0,      9, "1 2 3"         },

                // TBD need vectors to test internal parse failure
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct expected value
                vector<Int64> expResult(&testAllocator);
                {
                    const char *endPos = EXP;
                    Int64 value;
                    while (*endPos) {
                        LOOP_ASSERT(LINE,
                                    0 == bdepu_TypesParserImpUtil::
                                         parseInt64(&endPos, &value, endPos));
                        expResult.push_back(value);
                        if (' ' == *endPos) {
                            ++endPos;
                        }
                    }
                }

                vector<Int64> result(&testAllocator);
                const char *endPos = 0;
                int rv = bdepu_ArrayParserImpUtil::
                                      parseInt64Array(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE, expResult == result);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING PARSE INT ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseIntArray(cchar **endPos, ArInt *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseIntArray" << endl
                          << "=====================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_fail;     // parsing expected to fail indicator
                int         d_offset;   // expected number of parsed characters
                const char *d_result_p; // expected result specification string
            } DATA[] = {
                //line  spec                   fail  offset  result
                //----  ---------------------  ----  ------  ---------------
                { L_,   "",                       1,      0, ""              },
                { L_,   "[",                      1,      1, ""              },
                { L_,   "]",                      1,      0, ""              },
                { L_,   "[]",                     0,      2, ""              },
                { L_,   "[ ]",                    0,      3, ""              },
                { L_,   "[1]",                    0,      3, "1"             },
                { L_,   "[  ]",                   0,      4, ""              },
                { L_,   "[ 1]",                   0,      4, "1"             },
                { L_,   "[1 ]",                   0,      4, "1"             },
                { L_,   "[  1]",                  0,      5, "1"             },
                { L_,   "[ 1 ]",                  0,      5, "1"             },
                { L_,   "[1  ]",                  0,      5, "1"             },
                { L_,   "[1 2]",                  0,      5, "1 2"           },
                { L_,   "[ 1 2]",                 0,      6, "1 2"           },
                { L_,   "[1  2]",                 0,      6, "1 2"           },
                { L_,   "[1 2 ]",                 0,      6, "1 2"           },
                { L_,   "[ 1 2 ]",                0,      7, "1 2"           },
                { L_,   "[ 1 2 3 ]",              0,      9, "1 2 3"         },

                // TBD need vectors to test internal parse failure
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct expected value
                vector<int> expResult(&testAllocator);
                {
                    const char *endPos = EXP;
                    int value = 0;
                    while (*endPos) {
                        LOOP_ASSERT(LINE,
                                    0 == bdepu_TypesParserImpUtil::
                                            parseInt(&endPos, &value, endPos));
                        expResult.push_back(value);
                        if (' ' == *endPos) {
                            ++endPos;
                        }
                    }
                }

                vector<int> result(&testAllocator);
                const char *endPos = 0;
                int rv = bdepu_ArrayParserImpUtil::
                                         parseIntArray(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE, expResult == result);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PARSE FLOAT ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseFloatArray(cchar **endPos, ArFl *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseFloatArray" << endl
                          << "=======================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_fail;     // parsing expected to fail indicator
                int         d_offset;   // expected number of parsed characters
                const char *d_result_p; // expected result specification string
            } DATA[] = {
                //line  spec                   fail  offset  result
                //----  ---------------------  ----  ------  ---------------
                { L_,   "",                       1,      0, ""              },
                { L_,   "[",                      1,      1, ""              },
                { L_,   "]",                      1,      0, ""              },
                { L_,   "[]",                     0,      2, ""              },
                { L_,   "[ ]",                    0,      3, ""              },
                { L_,   "[1]",                    0,      3, "1"             },
                { L_,   "[  ]",                   0,      4, ""              },
                { L_,   "[ 1]",                   0,      4, "1"             },
                { L_,   "[1 ]",                   0,      4, "1"             },
                { L_,   "[  1]",                  0,      5, "1"             },
                { L_,   "[ 1 ]",                  0,      5, "1"             },
                { L_,   "[1  ]",                  0,      5, "1"             },
                { L_,   "[1 2]",                  0,      5, "1 2"           },
                { L_,   "[ 1 2]",                 0,      6, "1 2"           },
                { L_,   "[1  2]",                 0,      6, "1 2"           },
                { L_,   "[1 2 ]",                 0,      6, "1 2"           },
                { L_,   "[ 1 2 ]",                0,      7, "1 2"           },
                { L_,   "[ 1 2 3 ]",              0,      9, "1 2 3"         },

                // TBD need vectors to test internal parse failure
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct expected value
                vector<float> expResult(&testAllocator);
                {
                    const char *endPos = EXP;
                    float value;
                    while (*endPos) {
                        LOOP_ASSERT(LINE,
                                    0 == bdepu_TypesParserImpUtil::
                                         parseFloat(&endPos, &value, endPos));
                        expResult.push_back(value);
                        if (' ' == *endPos) {
                            ++endPos;
                        }
                    }
                }

                vector<float> result(&testAllocator);
                const char *endPos = 0;
                int rv = bdepu_ArrayParserImpUtil::
                                      parseFloatArray(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE, expResult == result);
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING PARSE DOUBLE ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseDoubleArray(cchar **endPos, ArDb *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseDoubleArray" << endl
                          << "========================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_fail;     // parsing expected to fail indicator
                int         d_offset;   // expected number of parsed characters
                const char *d_result_p; // expected result specification string
            } DATA[] = {
                //line  spec                   fail  offset  result
                //----  ---------------------  ----  ------  ---------------
                { L_,   "",                       1,      0, ""              },
                { L_,   "[",                      1,      1, ""              },
                { L_,   "]",                      1,      0, ""              },
                { L_,   "[]",                     0,      2, ""              },
                { L_,   "[ ]",                    0,      3, ""              },
                { L_,   "[1]",                    0,      3, "1"             },
                { L_,   "[  ]",                   0,      4, ""              },
                { L_,   "[ 1]",                   0,      4, "1"             },
                { L_,   "[1 ]",                   0,      4, "1"             },
                { L_,   "[  1]",                  0,      5, "1"             },
                { L_,   "[ 1 ]",                  0,      5, "1"             },
                { L_,   "[1  ]",                  0,      5, "1"             },
                { L_,   "[1 2]",                  0,      5, "1 2"           },
                { L_,   "[ 1 2]",                 0,      6, "1 2"           },
                { L_,   "[1  2]",                 0,      6, "1 2"           },
                { L_,   "[1 2 ]",                 0,      6, "1 2"           },
                { L_,   "[ 1 2 ]",                0,      7, "1 2"           },
                { L_,   "[ 1 2 3 ]",              0,      9, "1 2 3"         },

                // TBD need vectors to test internal parse failure
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct expected value
                vector<double> expResult(&testAllocator);
                {
                    const char *endPos = EXP;
                    double value;
                    while (*endPos) {
                        LOOP_ASSERT(LINE,
                                    0 == bdepu_TypesParserImpUtil::
                                         parseDouble(&endPos, &value, endPos));
                        expResult.push_back(value);
                        if (' ' == *endPos) {
                            ++endPos;
                        }
                    }
                }

                vector<double> result(&testAllocator);
                const char *endPos = 0;
                int rv = bdepu_ArrayParserImpUtil::
                                      parseDoubleArray(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE, expResult == result);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PARSE DATETIME ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseDatetimeArray(cchar **endPos, ArDT *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseDatetimeArray" << endl
                          << "==========================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_fail;     // 1 for fail, 0 for success
            int         d_offset;   // expected number of parsed characters
        } DATA0[] = {
            //line  spec       fail  off
            //----  ---------  ----  ---
            { L_,   "",          1,    0 },
            { L_,   " ",         1,    0 },
            { L_,   "x",         1,    0 },
            { L_,   "0",         1,    0 },
            { L_,   "  ",        1,    0 },
            { L_,   " [",        1,    0 },
            { L_,   "[",         1,    1 },
            { L_,   "[ ",        1,    2 },
            { L_,   "[ 1",       1,    3 },
            { L_,   "[]",        0,    2 },
            { L_,   "[ ]",       0,    3 },
            { L_,   "[  ]",      0,    4 },
        };
        const int NUM_DATA0 = sizeof DATA0 / sizeof *DATA0;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_year;     // expected return year
            int         d_month;    // expected return month
            int         d_day;      // expected return day
            int         d_hour;     // expected return hour
            int         d_minute;   // expected return minute
            int         d_second;   // expected return second
            int         d_milli;    // expected return millisecond
        } DATA1[] = {
            //line  spec                     year  month  day
            //                              hours   mins secs    ms
            //----  ---------------          ----  -----  ---   ---
            { L_,   "19/1/2 7:01:32",          19,     1,   2,
                                                7,     1,  32,    0 },
            { L_,   "19/01/02_08:24",          19,     1,   2,
                                                8,    24,   0,    0 },
            { L_,   "199/01/02 8:24",         199,     1,   2,
                                                8,    24,   0,    0 },
            { L_,   "1993/1/02_8:24:29.1",   1993,     1,   2,
                                                8,    24,  29,  100 },
            { L_,   "1993/01/2 18:57:09.98", 1993,     1,   2,
                                               18,    57,   9,  980 },
            { L_,   "1993/1/2_04:31:00.100", 1993,     1,   2,
                                                4,    31,   0,  100 },
            { L_,   "2003/11/22_18:24:29",   2003,    11,  22,
                                               18,    24,  29,    0 },
        };
        const int NUM_DATA1 = sizeof DATA1 / sizeof *DATA1;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_offset;   // expected number of parsed characters
        } DATA2[] = {
            //line  spec                       off
            //----  ---------------            ---
            { L_,   "0",                         0 },
            { L_,   "1",                         1 },
            { L_,   "19",                        2 },
            { L_,   "199",                       3 },
            { L_,   "1993",                      4 },
            { L_,   "1993/",                     5 },
            { L_,   "1993/0",                    5 },
            { L_,   "1993/01",                   7 },
            { L_,   "1993/13",                   6 },
            { L_,   "1993/01/",                  8 },
            { L_,   "1993/01/0",                 8 },
            { L_,   "1993/01/32",                9 },
            { L_,   "1993/02/31",                9 },
            { L_,   "2011/05/03",               10 },
            { L_,   "2011/05/03 ",              11 },
            { L_,   "2011/05/03 x",           11+0 },
            { L_,   "2011/05/03 1",           11+1 },
            { L_,   "2011/05/03 01",          11+2 },
            { L_,   "2011/05/03 1:",          11+2 },
            { L_,   "2011/05/03 08:",         11+3 },
            { L_,   "2011/05/03 08: ",        11+3 },
            { L_,   "2011/05/03 08:2",        11+4 },
            { L_,   "2011/05/03 8:2",         11+3 },
            { L_,   "2011/05/03 08:2 ",       11+4 },
            { L_,   "2011/05/03 08:24:",      11+6 },
            { L_,   "2011/05/03 08:2:24",     11+4 },
            { L_,   "2011/05/03 08:24:2.",    11+7 },
            { L_,   "2011/05/03 08:24:2.x",   11+7 },
            { L_,   "2011/05/03 08:24:20.",   11+9 },
            { L_,   "2011/05/03 08:24:20.?",  11+9 },
            { L_,   "2011/05/03 28:44:20",    11+0 },
            { L_,   "2011/05/03 08:64:20",    11+3 },
        };
        const int NUM_DATA2 = sizeof DATA2 / sizeof *DATA2;

        bsl::vector<bsl::string> SPECS(&testAllocator);
        bsl::vector<bdet_Datetime> VALUES(&testAllocator);

        bsl::vector<bdet_Datetime> result(&testAllocator);

        const char *endPos = 0;

        for (int ti = 0; ti < NUM_DATA0 ; ++ti) {
            const int LINE         = DATA0[ti].d_lineNum;
            const char *const SPEC = DATA0[ti].d_spec_p;
            const int FAIL         = DATA0[ti].d_fail;
            const int NUM          = DATA0[ti].d_offset;

            int rv = bdepu_ArrayParserImpUtil::
                     parseDatetimeArray(&endPos, &result, SPEC);
            LOOP_ASSERT(LINE, FAIL == rv);
            LOOP_ASSERT(LINE, result.size() == 0);
            LOOP_ASSERT(LINE, SPEC + NUM == endPos);
        }

        for (int ti = 0; ti < NUM_DATA2 ; ++ti) {
            const int LINE         = DATA2[ti].d_lineNum;
            const char *const SPEC = DATA2[ti].d_spec_p;
            const int NUM          = DATA2[ti].d_offset;

            bsl::string tmpStr("[" , &testAllocator);
            tmpStr.append(SPEC);
            int rv = bdepu_ArrayParserImpUtil::
                     parseDatetimeArray(&endPos, &result, tmpStr.c_str());
            LOOP_ASSERT(LINE, 1 == rv);
            LOOP_ASSERT(LINE, result.size() == 0);
            LOOP_ASSERT(LINE,
                        tmpStr.c_str() + NUM + 1 == endPos);
        }

        for (int ti = 0; ti < NUM_DATA1 ; ++ti) {
            const int LINE         = DATA1[ti].d_lineNum;
            const char *const SPEC = DATA1[ti].d_spec_p;
            const int YEAR         = DATA1[ti].d_year;
            const int MONTH        = DATA1[ti].d_month;
            const int DAY          = DATA1[ti].d_day;
            const int HOUR         = DATA1[ti].d_hour;
            const int MINS         = DATA1[ti].d_minute;
            const int SECS         = DATA1[ti].d_second;
            const int MILLI        = DATA1[ti].d_milli;
            const bdet_Date VALUED(YEAR, MONTH, DAY);
            const bdet_Time VALUET(HOUR, MINS, SECS, MILLI);
            const bdet_Datetime VALUE(VALUED, VALUET);

            SPECS.push_back(bsl::string(SPEC, &testAllocator));
            VALUES.push_back(VALUE);

            bsl::string baseStr("[", &testAllocator);
            for (int tj = 0; tj <= ti ; ++tj) {
                bsl::string tmpStr(baseStr, &testAllocator);
                tmpStr.append(" ");
                tmpStr.append(SPECS[ti - tj]);
                int rv = bdepu_ArrayParserImpUtil::
                         parseDatetimeArray(&endPos, &result, tmpStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE, tmpStr.c_str() + tmpStr.length() == endPos);

                baseStr.append(SPECS[ti - tj]);
                rv = bdepu_ArrayParserImpUtil::
                         parseDatetimeArray(&endPos, &result, baseStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE,
                            baseStr.c_str() + baseStr.length() == endPos);

                baseStr.append(" ");
                rv = bdepu_ArrayParserImpUtil::
                         parseDatetimeArray(&endPos, &result, baseStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE,
                            baseStr.c_str() + baseStr.length() == endPos);
            }

            for (int tj = 0; tj < NUM_DATA2 ; ++tj) {
                const int LINE2         = DATA2[tj].d_lineNum;
                const char *const SPEC2 = DATA2[tj].d_spec_p;
                const int NUM2          = DATA2[tj].d_offset;

                bsl::string tmpStr(baseStr, &testAllocator);
                tmpStr.append(SPEC2);
                int rv = bdepu_ArrayParserImpUtil::
                         parseDatetimeArray(&endPos, &result, tmpStr.c_str());
                LOOP2_ASSERT(LINE, LINE2, 1 == rv);
                LOOP2_ASSERT(LINE, LINE2, result.size() == ti + 1);
                LOOP2_ASSERT(LINE, LINE2,
                           tmpStr.c_str() + baseStr.length() + NUM2 == endPos);
            }

            baseStr.append("]");
            int rv = bdepu_ArrayParserImpUtil::
                         parseDatetimeArray(&endPos, &result, baseStr.c_str());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, baseStr.c_str() + baseStr.length() == endPos);
            for (int tj = 0; tj <= ti ; ++tj) {
                LOOP2_ASSERT(LINE, tj, VALUES[ti - tj] == result[tj]);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PARSE DATE ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseDateArray(cchar **endPos, ArDate *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseDateArray" << endl
                          << "======================" << endl;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_fail;     // 1 for fail, 0 for success
            int         d_offset;   // expected number of parsed characters
        } DATA0[] = {
            //line  spec       fail  off
            //----  ---------  ----  ---
            { L_,   "",          1,    0 },
            { L_,   " ",         1,    0 },
            { L_,   "x",         1,    0 },
            { L_,   "0",         1,    0 },
            { L_,   "  ",        1,    0 },
            { L_,   " [",        1,    0 },
            { L_,   "[",         1,    1 },
            { L_,   "[ ",        1,    2 },
            { L_,   "[ 1",       1,    3 },
            { L_,   "[]",        0,    2 },
            { L_,   "[ ]",       0,    3 },
            { L_,   "[  ]",      0,    4 },
        };
        const int NUM_DATA0 = sizeof DATA0 / sizeof *DATA0;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_year;     // expected return year
            int         d_month;    // expected return month
            int         d_day;      // expected return day
        } DATA1[] = {
            //line  spec             year  month  day
            //----  ---------------  ----  -----  ---
            { L_,   "19/1/2",          19,     1,   2 },
            { L_,   "19/01/02",        19,     1,   2 },
            { L_,   "199/01/02",      199,     1,   2 },
            { L_,   "1993/1/02",     1993,     1,   2 },
            { L_,   "1993/01/2",     1993,     1,   2 },
            { L_,   "1993/01/02",    1993,     1,   2 },
        };
        const int NUM_DATA1 = sizeof DATA1 / sizeof *DATA1;

        static const struct {
            int         d_lineNum;  // source line number
            const char *d_spec_p;   // specification string
            int         d_offset;   // expected number of parsed characters
        } DATA2[] = {
            //line  spec             off
            //----  ---------------  ---
            { L_,   "0",               0 },
            { L_,   "1",               1 },
            { L_,   "19",              2 },
            { L_,   "199",             3 },
            { L_,   "1993",            4 },
            { L_,   "1993/",           5 },
            { L_,   "1993/0",          5 },
            { L_,   "1993/01",         7 },
            { L_,   "1993/13",         6 },
            { L_,   "1993/01/",        8 },
            { L_,   "1993/01/0",       8 },
            { L_,   "1993/01/32",      9 },
            { L_,   "1993/02/31",      9 },
        };
        const int NUM_DATA2 = sizeof DATA2 / sizeof *DATA2;

        bsl::vector<bsl::string> SPECS(&testAllocator);
        bsl::vector<bdet_Date> VALUES(&testAllocator);

        bsl::vector<bdet_Date> result(&testAllocator);

        const char *endPos = 0;

        for (int ti = 0; ti < NUM_DATA0 ; ++ti) {
            const int LINE         = DATA0[ti].d_lineNum;
            const char *const SPEC = DATA0[ti].d_spec_p;
            const int FAIL         = DATA0[ti].d_fail;
            const int NUM          = DATA0[ti].d_offset;

            int rv = bdepu_ArrayParserImpUtil::
                     parseDateArray(&endPos, &result, SPEC);
            LOOP_ASSERT(LINE, FAIL == rv);
            LOOP_ASSERT(LINE, result.size() == 0);
            LOOP_ASSERT(LINE, SPEC + NUM == endPos);
        }

        for (int ti = 0; ti < NUM_DATA2 ; ++ti) {
            const int LINE         = DATA2[ti].d_lineNum;
            const char *const SPEC = DATA2[ti].d_spec_p;
            const int NUM          = DATA2[ti].d_offset;

            bsl::string tmpStr("[" , &testAllocator);
            tmpStr.append(SPEC);
            int rv = bdepu_ArrayParserImpUtil::
                     parseDateArray(&endPos, &result, tmpStr.c_str());
            LOOP_ASSERT(LINE, 1 == rv);
            LOOP_ASSERT(LINE, result.size() == 0);
            LOOP_ASSERT(LINE,
                        tmpStr.c_str() + NUM + 1 == endPos);
        }

        for (int ti = 0; ti < NUM_DATA1 ; ++ti) {
            const int LINE         = DATA1[ti].d_lineNum;
            const char *const SPEC = DATA1[ti].d_spec_p;
            const int YEAR         = DATA1[ti].d_year;
            const int MONTH        = DATA1[ti].d_month;
            const int DAY          = DATA1[ti].d_day;
            const bdet_Date VALUE(YEAR, MONTH, DAY);

            SPECS.push_back(bsl::string(SPEC, &testAllocator));
            VALUES.push_back(VALUE);

            bsl::string baseStr("[", &testAllocator);
            for (int tj = 0; tj <= ti ; ++tj) {
                bsl::string tmpStr(baseStr, &testAllocator);
                tmpStr.append(" ");
                tmpStr.append(SPECS[ti - tj]);
                int rv = bdepu_ArrayParserImpUtil::
                         parseDateArray(&endPos, &result, tmpStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE, tmpStr.c_str() + tmpStr.length() == endPos);

                baseStr.append(SPECS[ti - tj]);
                rv = bdepu_ArrayParserImpUtil::
                         parseDateArray(&endPos, &result, baseStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE,
                            baseStr.c_str() + baseStr.length() == endPos);

                baseStr.append(" ");
                rv = bdepu_ArrayParserImpUtil::
                         parseDateArray(&endPos, &result, baseStr.c_str());
                LOOP_ASSERT(LINE, 1 == rv);
                LOOP_ASSERT(LINE, result.size() == tj + 1);
                LOOP_ASSERT(LINE,
                            baseStr.c_str() + baseStr.length() == endPos);
            }

            for (int tj = 0; tj < NUM_DATA2 ; ++tj) {
                const int LINE2         = DATA2[tj].d_lineNum;
                const char *const SPEC2 = DATA2[tj].d_spec_p;
                const int NUM2          = DATA2[tj].d_offset;

                bsl::string tmpStr(baseStr, &testAllocator);
                tmpStr.append(SPEC2);
                int rv = bdepu_ArrayParserImpUtil::
                         parseDateArray(&endPos, &result, tmpStr.c_str());
                LOOP2_ASSERT(LINE, LINE2, 1 == rv);
                LOOP2_ASSERT(LINE, LINE2, result.size() == ti + 1);
                LOOP2_ASSERT(LINE, LINE2,
                           tmpStr.c_str() + baseStr.length() + NUM2 == endPos);
            }

            baseStr.append("]");
            int rv = bdepu_ArrayParserImpUtil::
                         parseDateArray(&endPos, &result, baseStr.c_str());
            LOOP_ASSERT(LINE, 0 == rv);
            LOOP_ASSERT(LINE, baseStr.c_str() + baseStr.length() == endPos);
            for (int tj = 0; tj <= ti ; ++tj) {
                LOOP2_ASSERT(LINE, tj, VALUES[ti - tj] == result[tj]);
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING PARSE CHAR ARRAY
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   parseCharArray(cchar **endPos, ArCh *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseCharArray" << endl
                          << "======================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_fail;     // parsing expected to fail indicator
                int         d_offset;   // expected number of parsed characters
                const char *d_result_p; // expected result specification string
            } DATA[] = {
                //line  spec                   fail  offset  result
                //----  ---------------------  ----  ------  ---------------
                { L_,   "",                       1,      0, ""              },
                { L_,   "[",                      1,      1, ""              },
                { L_,   "]",                      1,      0, ""              },
                { L_,   "[]",                     0,      2, ""              },
                { L_,   "[ ]",                    0,      3, ""              },
                { L_,   "[  ]",                   0,      4, ""              },
                { L_,   "['a']",                  0,      5, "'a'"           },
                { L_,   "[ 'a']",                 0,      6, "'a'"           },
                { L_,   "['a' ]",                 0,      6, "'a'"           },
                { L_,   "[  'a']",                0,      7, "'a'"           },
                { L_,   "[ 'a' ]",                0,      7, "'a'"           },
                { L_,   "['a'  ]",                0,      7, "'a'"           },
                { L_,   "['a''b']",               1,      4, "'a'"           },
                { L_,   "[ 'a''b']",              1,      5, "'a'"           },
                { L_,   "['a' 'b']",              0,      9, "'a' 'b'"       },
                { L_,   "['a''b' ]",              1,      4, "'a'"           },
                { L_,   "[  'a''b']",             1,      6, "'a'"           },
                { L_,   "[ 'a' 'b']",             0,     10, "'a' 'b'"       },
                { L_,   "[ 'a''b' ]",             1,      5, "'a'"           },
                { L_,   "['a'  'b']",             0,     10, "'a' 'b'"       },
                { L_,   "['a' 'b' ]",             0,     10, "'a' 'b'"       },
                { L_,   "['a''b'  ]",             1,      4, "'a'"           },
                { L_,   "[ 'a' 'b' ]",            0,     11, "'a' 'b'"       },
                { L_,   "[ 'a' 'b' 'c' ]",        0,     15, "'a' 'b' 'c'"   },

                // TBD need vectors to test internal parse failure
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const char *const EXP  = DATA[ti].d_result_p;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // construct expected value
                vector<char> expResult(&testAllocator);
                {
                    const char *endPos = EXP;
                    char value;
                    while (*endPos) {
                        LOOP_ASSERT(LINE,
                                    0 == bdepu_TypesParserImpUtil::
                                           parseChar(&endPos, &value, endPos));
                        expResult.push_back(value);
                        if (' ' == *endPos) {
                            ++endPos;
                        }
                    }
                }

                vector<char> result(&testAllocator);
                const char *endPos = 0;
                int rv = bdepu_ArrayParserImpUtil::
                                        parseCharArray(&endPos, &result, SPEC);
                LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                LOOP_ASSERT(LINE, FAIL == !!rv);
                LOOP_ASSERT(LINE, expResult == result);
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
