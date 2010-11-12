// bdepu_ntypesparser.t.cpp              -*-C++-*-

#include <bdepu_ntypesparser.h>
#include <bdepu_parserimputil.h>

#include <bsls_platform.h>
#include <bslma_testallocator.h>
#include <bsls_platformutil.h>
#include <bsl_vector.h>
#include <bdet_datetime.h>
#include <bdet_date.h>
#include <bdet_time.h>

#include <bsl_algorithm.h>

#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This driver tests parsing and generating functions of 'bdepu_NTypesParser'.
// Each parsing function has different constrains, which are described in the
// function documentation in 'bdepu_NTypesparser.h'.  However, each of the
// parsed strings representing basic types may be preceded by arbitrary
// <WHITESPACE>, which is also defined in 'bdepu_NTypesparser.h'.  Array types
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
//  'bDatez'     for 'bdet_DateTz'
//  'bTime'      for 'bdet_Time'
//  'bTimez'     for 'bdet_TimeTz'
//  'bDT'        for 'bdet_Datetime'
//  'bDTz'       for 'bdet_DatetimeTz'
//  'ArCh'       for 'bsl::vector<char>'
//  'ArSh'       for 'bsl::vector<short>'
//  'ArInt'      for 'bsl::vector<int>'
//  'ArInt64'    for 'bsl::vector<bsls_PlatformUtil::Int64>'
//  'ArFl'       for 'bsl::vector<float>'
//  'ArDb'       for 'bsl::vector<double>'
//  'ArStr'      for 'bsl::vector<bsl::string>'
//  'ArDate'     for 'bsl::vector<bdet_Date>'
//  'ArDatez'    for 'bsl::vector<bdet_DateTz>'
//  'ArTime'     for 'bsl::vector<bdet_Time>'
//  'ArTimez'    for 'bsl::vector<bdet_TimeTz>'
//  'ArDT'       for 'bsl::vector<bdet_Datetime>'
//  'ArDTz'      for 'bsl::vector<bdet_DatetimeTz>'
//
//  'c' in front of an abbreviation stands for 'const'.
//
//-----------------------------------------------------------------------------
// [ 1] parseChar(cchar **endPos, char *result, cchar *inputString);
// [ 2] parseDate(cchar **endPos, bDate *res, cchar *in);
// [ 3] parseDatetime(cchar **endPos, bDT *res, cchar *in);
// [ 4] parseDouble(cchar **endPos, double *res, cchar *in);
// [ 5] parseFloat(cchar **endPos, double *res, cchar *in);
// [ 6] parseInt(cchar **endPos, int *res, cchar *in, int base = 10);
// [ 7] parseInt64(cchar **endPos, int64 *res, cchar *in, int base = 10);
// [ 8] parseIntegerLocator(cchar **endPos, int *result, cchar *inputString);
// [ 9] parseShort(cchar **endPos, short *res, cchar *in, int base = 10);
// [10] parseString(cchar **endPos, bStr *res, cchar *in);
// [11] parseTime(cchar **endPos, bTime *res, cchar *in);
// [12] parseCharArray(cchar **endPos, ArCh *res, cchar *in);
// [13] parseDateArray(cchar **endPos, ArDate *res, cchar *in);
// [14] parseDatetimeArray(cchar **endPos, ArDT *res, cchar *in);
// [15] parseDoubleArray(cchar **endPos, ArDb *res, cchar *in);
// [16] parseFloatArray(cchar **endPos, ArFl *res, cchar *in);
// [17] parseIntArray(cchar **endPos, ArInt *res, cchar *in);
// [18] parseInt64Array(cchar **endPos, ArInt64 *res, cchar *in);
// [19] parseShortArray(cchar **endPos, ArSh *res, cchar *in);
// [20] parseStringArray(cchar **endPos, ArStr *res, cchar *in);
// [21] parseTimeArray(cchar **endPos, ArTime *res, cchar *in);
// [22] stripNull(ArCh *buffer);
// [23] generateChar(ArCh *buffer, char value, int level, int spaces);
// [23] generateCharRaw(ArCh *buffer, char value, int level, int spaces);
// [24] generateDate(ArCh *buffer, bDate value, int level, int spaces);
// [24] generateDateRaw(ArCh *buffer, bDate value, int level, int spaces);
// [25] generateDatetime(ArCh *buffer, bDT value, int level, int spaces);
// [25] generateDatetimeRaw(ArCh *buffer, bDT value, int level, int spaces);
// [26] generateDouble(ArCh *buffer, double value, int level, int spaces);
// [26] generateDoubleRaw(ArCh *buffer, double value, int level, int spaces);
// [27] generateFloat(ArCh *buffer, float value, int level, int spaces);
// [27] generateFloatRaw(ArCh *buffer, float value, int level, int spaces);
// [28] generateInt(ArCh *buffer, int value, int level, int spaces);
// [28] generateIntRaw(ArCh *buffer, int value, int level, int spaces);
// [29] generateInt64(ArCh *buffer, int64 value, int level, int spaces);
// [29] generateInt64Raw(ArCh *buffer, int64 value, int level, int spaces);
// [30] generateIntegerLocator(b_ca *, int val, int level, int spaces);
// [30] generateIntegerLocatorRaw(b_ca *, int val, int level, int spaces);
// [31] generateShort(ArCh *buffer, short value, int level, int spaces);
// [31] generateShortRaw(ArCh *buffer, short value, int level, int spaces);
// [32] generateString(ArCh *buffer, bStr value, int level, int spaces);
// [32] generateStringRaw(ArCh *buffer, bStr value, int level, int spaces);
// [33] generateTime(ArCh *buffer, bTime value, int level, int spaces);
// [33] generateTimeRaw(ArCh *buffer, bTime value, int level, int spaces);
// [34] generateCharArray(ArCh *buf, cArCh& val, int lev, int sp);
// [34] generateCharArrayRaw(ArCh *buf, cArCh& val, int lev, int sp);
// [35] generateDateArray(ArCh *buf, cArDate& val, int lev, int sp);
// [35] generateDateArrayRaw(ArCh *buf, cArDate& val, int lev, int sp);
// [36] generateDatetimeArray(ArCh *buf, cArDT& val, int lev, int sp);
// [36] generateDatetimeArrayRaw(ArCh *buf, cArDT& val, int lev, int sp);
// [37] generateDoubleArray(ArCh *buf, cArDb& val, int lev, int sp);
// [37] generateDoubleArrayRaw(ArCh *buf, cArDb& val, int lev, int sp);
// [38] generateFloatArray(ArCh *buf, cArFl& val, int lev, int sp);
// [38] generateFloatArrayRaw(ArCh *buf, cArFl& val, int lev, int sp);
// [39] generateIntArray(ArCh *buf, cArInt& val, int lev, int sp);
// [39] generateIntArrayRaw(ArCh *buf, cArInt& val, int lev, int sp);
// [40] generateInt64Array(ArCh *buf, cArInt64& val, int lev, int sp);
// [40] generateInt64ArrayRaw(ArCh *buf, cArInt64& val, int lev, int sp);
// [41] generateShortArray(ArCh *buf, cArSh& val, int lev, int sp);
// [41] generateShortArrayRaw(ArCh *buf, cArSh& val, int lev, int sp);
// [42] generateStringArray(ArCh *buf, cArStr& val, int lev, int sp);
// [42] generateStringArrayRaw(ArCh *buf, cArStr& val, int lev, int sp);
// [43] generateTimeArray(ArCh *buf, cArTime& val, int lev, int sp);
// [43] generateTimeArrayRaw(ArCh *buf, cArTime& val, int lev, int sp);
// [XX] parseDateTz(cchar **endPos, bDatez *res, cchar *in);
// [XX] parseDateTzArray(cchar **endPos, ArDatez *res, cchar *in);
// [XX] generateDateTz(ArCh *buffer, bDatez value, int level, int spaces);
// [XX] generateDateTzRaw(ArCh *buffer, bDatez value, int level, int spaces);
// [XX] generateDateTzArray(ArCh *buf, cArDatez& val, int lev, int sp);
// [XX] generateDateTzArrayRaw(ArCh *buf, cArDatez& val, int lev, int sp);
// [XX] parseDateTimeTz(cchar **endPos, bDTz *res, cchar *in);
// [XX] parseDateTimeTzArray(cchar **endPos, ArDTz *res, cchar *in);
// [XX] generateDateTimeTz(ArCh *buffer, bDTz value, int level, int spaces);
// [XX] generateDateTimeTzRaw(ArCh *buffer, bDTz value, int level, int spaces);
// [XX] generateDateTimeTzArray(ArCh *buf, cArDTz& val, int lev, int sp);
// [XX] generateDateTimeTzArrayRaw(ArCh *buf, cArDTz& val, int lev, int sp);
// [XX] parseTimeTz(cchar **endPos, bTimez *res, cchar *in);
// [XX] parseTimeTzArray(cchar **endPos, ArTimez *res, cchar *in);
// [XX] generateTimeTz(ArCh *buffer, bTimez value, int level, int spaces);
// [XX] generateTimeTzRaw(ArCh *buffer, bTimez value, int level, int spaces);
// [XX] generateTimeTzArray(ArCh *buf, cArTimez& val, int lev, int sp);
// [XX] generateTimeTzArrayRaw(ArCh *buf, cArTimez& val, int lev, int sp);
//-----------------------------------------------------------------------------
// TBD usage

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
#define LOOP_ASSERT(I,X) {                  \
    if (!(X)) { cout                        \
                << #I << ": " << I << "\n"; \
                aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {               \
    if (!(X)) { cout                        \
                << #I << ": " << I << "\t"  \
                << #J << ": " << J << "\n"; \
                aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {             \
    if (!(X)) { cout                        \
                << #I << ": " << I << "\t"  \
                << #J << ": " << J << "\t"  \
                << #K << ": " << K << "\n"; \
                aSsErT(1, #X, __LINE__); } }

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

typedef bsls_PlatformUtil::Int64 Int64;

//=============================================================================
//                  FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

template <typename TYPE>
TYPE myabs(const TYPE& x)
{
    return x < 0 ? -x : x;
}

template <typename TYPE>
bool near(const TYPE& lhs, const TYPE& rhs)
{
    return myabs(lhs - rhs) / (myabs(lhs) + myabs(rhs)) < 0.001;
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;
    bslma_TestAllocator testAllocator(veryVeryVerbose);

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 42: {
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
        //   generateTimeArray(ArCh *buf, cArTime& val, int lev, int sp);
        //   generateTimeArrayRaw(ArCh *buf, cArTime& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateTimeArray*" << endl
                          << "==========================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_spec_p;          // specification string
                const char *d_value_p;         // specification value
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  spec  value           level  SPL  exp
                //----  ----  --------------  -----  ---  ------------------
                { L_,   "",   "[ ]",              0,   0, "[\n]"             },
                { L_,   "",   "[ ]",              0,   1, "[\n]"             },
                { L_,   "",   "[ ]",              1,   0, "[\n]"             },
                { L_,   "",   "[ ]",              1,   1, " [\n ]"           },
                { L_,   "",   "[ ]",              2,   3, "      [\n      ]" },
                { L_,   "",   "[ ]",             -1,   3, "[\n   ]"          },
                { L_,   "",   "[ ]",             -2,   3, "[\n      ]"       },
                { L_,   "",   "[ 07:42:16.003 ]", 0,   0,
                                                        "[\n07:42:16.003\n]" },
                { L_,   "",   "[ 07:42:16.003 ]", 0,   1,
                                                       "[\n 07:42:16.003\n]" },
                { L_,   "",   "[ 07:42:16.003 ]", 1,   0,
                                                        "[\n07:42:16.003\n]" },
                { L_,   "",   "[ 07:42:16.003 ]", 1,   1,
                                                    " [\n  07:42:16.003\n ]" },
                { L_,   "",   "[ 07:42:16.003 ]", 2,   3,
                                   "      [\n         07:42:16.003\n      ]" },
                { L_,   "",   "[ 07:42:16.003 ]",
                                                 -1,   3,
                                               "[\n      07:42:16.003\n   ]" },
                { L_,   "",   "[ 07:42:16.003 ]",
                                                 -2,   3,
                                         "[\n         07:42:16.003\n      ]" },
                { L_,   "",   "[ 07:42:16.003 23:19:18.612 ]",
                                                  0,   0,
                                          "[\n07:42:16.003\n23:19:18.612\n]" },
                { L_,   "",   "[ 07:42:16.003 23:19:18.612 ]",
                                                  0,   1,
                                        "[\n 07:42:16.003\n 23:19:18.612\n]" },
                { L_,   "",   "[ 07:42:16.003 23:19:18.612 ]",
                                                  1,   0,
                                          "[\n07:42:16.003\n23:19:18.612\n]" },
                { L_,   "",   "[ 07:42:16.003 23:19:18.612 ]",
                                                  1,   1,
                                    " [\n  07:42:16.003\n  23:19:18.612\n ]" },
                { L_,   "",   "[ 07:42:16.003 23:19:18.612 ]",
                                                  2,   3,
            "      [\n         07:42:16.003\n         23:19:18.612\n      ]" },
                { L_,   "",   "[ 07:42:16.003 23:19:18.612 ]",
                                                 -1,   3,
                           "[\n      07:42:16.003\n      23:19:18.612\n   ]" },
                { L_,   "",   "[ 07:42:16.003 23:19:18.612 ]",
                                                 -2,   3,
                  "[\n         07:42:16.003\n         23:19:18.612\n      ]" },

                { L_,   "x",  "[ ]",              0,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              0,   1, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   1, "x [\n ]"          },
                { L_,   "x",  "[ ]",              2,   3, "x      [\n      ]"},
                { L_,   "x",  "[ ]",             -1,   3, "x[\n   ]"         },
                { L_,   "x",  "[ ]",             -2,   3, "x[\n      ]"      },
                { L_,   "x",  "[ 07:42:16.003 ]", 0,   0,
                                                       "x[\n07:42:16.003\n]" },
                { L_,   "x",  "[ 07:42:16.003 ]", 0,   1,
                                                      "x[\n 07:42:16.003\n]" },
                { L_,   "x",  "[ 07:42:16.003 ]", 1,   0,
                                                       "x[\n07:42:16.003\n]" },
                { L_,   "x",  "[ 07:42:16.003 ]", 1,   1,
                                                   "x [\n  07:42:16.003\n ]" },
                { L_,   "x",  "[ 07:42:16.003 ]", 2,   3,
                                  "x      [\n         07:42:16.003\n      ]" },
                { L_,   "x",  "[ 07:42:16.003 ]",
                                                 -1,   3,
                                              "x[\n      07:42:16.003\n   ]" },
                { L_,   "x",  "[ 07:42:16.003 ]",
                                                 -2,   3,
                                        "x[\n         07:42:16.003\n      ]" },
                { L_,   "x",  "[ 07:42:16.003 23:19:18.612 ]",
                                                  0,   0,
                                         "x[\n07:42:16.003\n23:19:18.612\n]" },
                { L_,   "x",  "[ 07:42:16.003 23:19:18.612 ]",
                                                  0,   1,
                                       "x[\n 07:42:16.003\n 23:19:18.612\n]" },
                { L_,   "x",  "[ 07:42:16.003 23:19:18.612 ]",
                                                  1,   0,
                                         "x[\n07:42:16.003\n23:19:18.612\n]" },
                { L_,   "x",  "[ 07:42:16.003 23:19:18.612 ]",
                                                  1,   1,
                                   "x [\n  07:42:16.003\n  23:19:18.612\n ]" },
                { L_,   "x",  "[ 07:42:16.003 23:19:18.612 ]",
                                                  2,   3,
           "x      [\n         07:42:16.003\n         23:19:18.612\n      ]" },
                { L_,   "x",  "[ 07:42:16.003 23:19:18.612 ]",
                                                 -1,   3,
                          "x[\n      07:42:16.003\n      23:19:18.612\n   ]" },
                { L_,   "x",  "[ 07:42:16.003 23:19:18.612 ]",
                                                 -2,   3,
                 "x[\n         07:42:16.003\n         23:19:18.612\n      ]" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char *VALUE       = DATA[ti].d_value_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<bdet_Time> val(&testAllocator);
                const char *endPos;
                bdepu_NTypesParser::parseTimeArray(&endPos, &val, VALUE);

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_NTypesParser::
                              generateTimeArrayRaw(&buffer, val, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::
                                 generateTimeArray(&buffer, val, LEVEL, SPL);
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 41: {
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
        //   generateStringArray(ArCh *buf, cArStr& val, int lev, int sp);
        //   generateStringArrayRaw(ArCh *buf, cArStr& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateStringArray*" << endl
                          << "============================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_spec_p;          // specification string
                const char *d_value_p;         // specification value
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  spec  value           level  SPL  exp
                //----  ----  --------------  -----  ---  ------------------
                { L_,   "",   "[ ]",              0,   0, "[\n]"             },
                { L_,   "",   "[ ]",              0,   1, "[\n]"             },
                { L_,   "",   "[ ]",              1,   0, "[\n]"             },
                { L_,   "",   "[ ]",              1,   1, " [\n ]"           },
                { L_,   "",   "[ ]",              2,   3, "      [\n      ]" },
                { L_,   "",   "[ ]",             -1,   3, "[\n   ]"          },
                { L_,   "",   "[ ]",             -2,   3, "[\n      ]"       },
                { L_,   "",   "[ aaa ]",          0,   0, "[\naaa\n]"        },
                { L_,   "",   "[ aaa ]",          0,   1, "[\n aaa\n]"       },
                { L_,   "",   "[ aaa ]",          1,   0, "[\naaa\n]"        },
                { L_,   "",   "[ aaa ]",          1,   1, " [\n  aaa\n ]"    },
                { L_,   "",   "[ aaa ]",          2,   3,
                                            "      [\n         aaa\n      ]" },
                { L_,   "",   "[ aaa ]",         -1,   3,
                                                        "[\n      aaa\n   ]" },
                { L_,   "",   "[ aaa ]",         -2,   3,
                                                  "[\n         aaa\n      ]" },
                { L_,   "",   "[ aaa bbb ]",      0,   0, "[\naaa\nbbb\n]"   },
                { L_,   "",   "[ aaa bbb ]",      0,   1, "[\n aaa\n bbb\n]" },
                { L_,   "",   "[ aaa bbb ]",      1,   0, "[\naaa\nbbb\n]"   },
                { L_,   "",   "[ aaa bbb ]",      1,   1,
                                                      " [\n  aaa\n  bbb\n ]" },
                { L_,   "",   "[ aaa bbb ]",      2,   3,
                              "      [\n         aaa\n         bbb\n      ]" },
                { L_,   "",   "[ aaa bbb ]",     -1,   3,
                                             "[\n      aaa\n      bbb\n   ]" },
                { L_,   "",   "[ aaa bbb ]",     -2,   3,
                                    "[\n         aaa\n         bbb\n      ]" },

                { L_,   "x",  "[ ]",              0,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              0,   1, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   1, "x [\n ]"          },
                { L_,   "x",  "[ ]",              2,   3, "x      [\n      ]"},
                { L_,   "x",  "[ ]",             -1,   3, "x[\n   ]"         },
                { L_,   "x",  "[ ]",             -2,   3, "x[\n      ]"      },
                { L_,   "x",  "[ aaa ]",          0,   0, "x[\naaa\n]"       },
                { L_,   "x",  "[ aaa ]",          0,   1, "x[\n aaa\n]"      },
                { L_,   "x",  "[ aaa ]",          1,   0, "x[\naaa\n]"       },
                { L_,   "x",  "[ aaa ]",          1,   1, "x [\n  aaa\n ]"   },
                { L_,   "x",  "[ aaa ]",          2,   3,
                                           "x      [\n         aaa\n      ]" },
                { L_,   "x",  "[ aaa ]",         -1,   3,
                                                       "x[\n      aaa\n   ]" },
                { L_,   "x",  "[ aaa ]",         -2,   3,
                                                 "x[\n         aaa\n      ]" },
                { L_,   "x",  "[ aaa bbb ]",      0,   0, "x[\naaa\nbbb\n]"  },
                { L_,   "x",  "[ aaa bbb ]",      0,   1,
                                                         "x[\n aaa\n bbb\n]" },
                { L_,   "x",  "[ aaa bbb ]",      1,   0, "x[\naaa\nbbb\n]"  },
                { L_,   "x",  "[ aaa bbb ]",      1,   1,
                                                     "x [\n  aaa\n  bbb\n ]" },
                { L_,   "x",  "[ aaa bbb ]",      2,   3,
                             "x      [\n         aaa\n         bbb\n      ]" },
                { L_,   "x",  "[ aaa bbb ]",     -1,   3,
                                            "x[\n      aaa\n      bbb\n   ]" },
                { L_,   "x",  "[ aaa bbb ]",     -2,   3,
                                   "x[\n         aaa\n         bbb\n      ]" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char *VALUE       = DATA[ti].d_value_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<bsl::string> val(&testAllocator);
                const char *endPos;
                bdepu_NTypesParser::parseStringArray(&endPos, &val, VALUE);

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_NTypesParser::
                              generateStringArrayRaw(&buffer, val, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::
                                 generateStringArray(&buffer, val, LEVEL, SPL);
                if (veryVerbose) {
                    P(&exp.front());
                    P(&buffer.front());
                    cout << endl;
                }
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 40: {
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
        //   generateShortArray(ArCh *buf, cArSh& val, int lev, int sp);
        //   generateShortArrayRaw(ArCh *buf, cArSh& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateShortArray*" << endl
                          << "===========================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_spec_p;          // specification string
                const char *d_value_p;         // specification value
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  spec  value           level  SPL  exp
                //----  ----  --------------  -----  ---  ------------------
                { L_,   "",   "[ ]",              0,   0, "[\n]"             },
                { L_,   "",   "[ ]",              0,   1, "[\n]"             },
                { L_,   "",   "[ ]",              1,   0, "[\n]"             },
                { L_,   "",   "[ ]",              1,   1, " [\n ]"           },
                { L_,   "",   "[ ]",              2,   3, "      [\n      ]" },
                { L_,   "",   "[ ]",             -1,   3, "[\n   ]"          },
                { L_,   "",   "[ ]",             -2,   3, "[\n      ]"       },
                { L_,   "",   "[ 1 ]",            0,   0, "[\n1\n]"          },
                { L_,   "",   "[ 1 ]",            0,   1, "[\n 1\n]"         },
                { L_,   "",   "[ 1 ]",            1,   0, "[\n1\n]"          },
                { L_,   "",   "[ 1 ]",            1,   1, " [\n  1\n ]"      },
                { L_,   "",   "[ 1 ]",            2,   3,
                                              "      [\n         1\n      ]" },
                { L_,   "",   "[ 1 ]",           -1,   3, "[\n      1\n   ]" },
                { L_,   "",   "[ 1 ]",           -2,   3,
                                                    "[\n         1\n      ]" },
                { L_,   "",   "[ 1 2 ]",          0,   0, "[\n1\n2\n]"       },
                { L_,   "",   "[ 1 2 ]",          0,   1, "[\n 1\n 2\n]"     },
                { L_,   "",   "[ 1 2 ]",          1,   0, "[\n1\n2\n]"       },
                { L_,   "",   "[ 1 2 ]",          1,   1, " [\n  1\n  2\n ]" },
                { L_,   "",   "[ 1 2 ]",          2,   3,
                                  "      [\n         1\n         2\n      ]" },
                { L_,   "",   "[ 1 2 ]",         -1,   3,
                                                 "[\n      1\n      2\n   ]" },
                { L_,   "",   "[ 1 2 ]",         -2,   3,
                                        "[\n         1\n         2\n      ]" },

                { L_,   "x",  "[ ]",              0,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              0,   1, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   1, "x [\n ]"          },
                { L_,   "x",  "[ ]",              2,   3, "x      [\n      ]"},
                { L_,   "x",  "[ ]",             -1,   3, "x[\n   ]"         },
                { L_,   "x",  "[ ]",             -2,   3, "x[\n      ]"      },
                { L_,   "x",  "[ 1 ]",            0,   0, "x[\n1\n]"         },
                { L_,   "x",  "[ 1 ]",            0,   1, "x[\n 1\n]"        },
                { L_,   "x",  "[ 1 ]",            1,   0, "x[\n1\n]"         },
                { L_,   "x",  "[ 1 ]",            1,   1, "x [\n  1\n ]"     },
                { L_,   "x",  "[ 1 ]",            2,   3,
                                             "x      [\n         1\n      ]" },
                { L_,   "x",  "[ 1 ]",           -1,   3,
                                                         "x[\n      1\n   ]" },
                { L_,   "x",  "[ 1 ]",           -2,   3,
                                                   "x[\n         1\n      ]" },
                { L_,   "x",  "[ 1 2 ]",          0,   0, "x[\n1\n2\n]"      },
                { L_,   "x",  "[ 1 2 ]",          0,   1, "x[\n 1\n 2\n]"    },
                { L_,   "x",  "[ 1 2 ]",          1,   0, "x[\n1\n2\n]"      },
                { L_,   "x",  "[ 1 2 ]",          1,   1,
                                                         "x [\n  1\n  2\n ]" },
                { L_,   "x",  "[ 1 2 ]",          2,   3,
                                 "x      [\n         1\n         2\n      ]" },
                { L_,   "x",  "[ 1 2 ]",         -1,   3,
                                                "x[\n      1\n      2\n   ]" },
                { L_,   "x",  "[ 1 2 ]",         -2,   3,
                                       "x[\n         1\n         2\n      ]" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char *VALUE       = DATA[ti].d_value_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<short> val(&testAllocator);
                const char *endPos;
                bdepu_NTypesParser::parseShortArray(&endPos, &val, VALUE);

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_NTypesParser::
                               generateShortArrayRaw(&buffer, val, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::
                                  generateShortArray(&buffer, val, LEVEL, SPL);
                if (veryVerbose) {
                    P(&exp.front());
                    P(&buffer.front());
                    cout << endl;
                }
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 39: {
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
        //   generateInt64Array(ArCh *buf, cArInt64& val, int lev, int sp);
        //   generateInt64ArrayRaw(ArCh *buf, cArInt64& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateInt64Array*" << endl
                          << "===========================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_spec_p;          // specification string
                const char *d_value_p;         // specification value
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  spec  value           level  SPL  exp
                //----  ----  --------------  -----  ---  ------------------
                { L_,   "",   "[ ]",              0,   0, "[\n]"             },
                { L_,   "",   "[ ]",              0,   1, "[\n]"             },
                { L_,   "",   "[ ]",              1,   0, "[\n]"             },
                { L_,   "",   "[ ]",              1,   1, " [\n ]"           },
                { L_,   "",   "[ ]",              2,   3, "      [\n      ]" },
                { L_,   "",   "[ ]",             -1,   3, "[\n   ]"          },
                { L_,   "",   "[ ]",             -2,   3, "[\n      ]"       },
                { L_,   "",   "[ 1 ]",            0,   0, "[\n1\n]"          },
                { L_,   "",   "[ 1 ]",            0,   1, "[\n 1\n]"         },
                { L_,   "",   "[ 1 ]",            1,   0, "[\n1\n]"          },
                { L_,   "",   "[ 1 ]",            1,   1, " [\n  1\n ]"      },
                { L_,   "",   "[ 1 ]",            2,   3,
                                              "      [\n         1\n      ]" },
                { L_,   "",   "[ 1 ]",           -1,   3, "[\n      1\n   ]" },
                { L_,   "",   "[ 1 ]",           -2,   3,
                                                    "[\n         1\n      ]" },
                { L_,   "",   "[ 1 2 ]",          0,   0, "[\n1\n2\n]"       },
                { L_,   "",   "[ 1 2 ]",          0,   1, "[\n 1\n 2\n]"     },
                { L_,   "",   "[ 1 2 ]",          1,   0, "[\n1\n2\n]"       },
                { L_,   "",   "[ 1 2 ]",          1,   1, " [\n  1\n  2\n ]" },
                { L_,   "",   "[ 1 2 ]",          2,   3,
                                  "      [\n         1\n         2\n      ]" },
                { L_,   "",   "[ 1 2 ]",         -1,   3,
                                                 "[\n      1\n      2\n   ]" },
                { L_,   "",   "[ 1 2 ]",         -2,   3,
                                        "[\n         1\n         2\n      ]" },

                { L_,   "x",  "[ ]",              0,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              0,   1, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   1, "x [\n ]"          },
                { L_,   "x",  "[ ]",              2,   3, "x      [\n      ]"},
                { L_,   "x",  "[ ]",             -1,   3, "x[\n   ]"         },
                { L_,   "x",  "[ ]",             -2,   3, "x[\n      ]"      },
                { L_,   "x",  "[ 1 ]",            0,   0, "x[\n1\n]"         },
                { L_,   "x",  "[ 1 ]",            0,   1, "x[\n 1\n]"        },
                { L_,   "x",  "[ 1 ]",            1,   0, "x[\n1\n]"         },
                { L_,   "x",  "[ 1 ]",            1,   1, "x [\n  1\n ]"     },
                { L_,   "x",  "[ 1 ]",            2,   3,
                                             "x      [\n         1\n      ]" },
                { L_,   "x",  "[ 1 ]",           -1,   3,
                                                         "x[\n      1\n   ]" },
                { L_,   "x",  "[ 1 ]",           -2,   3,
                                                   "x[\n         1\n      ]" },
                { L_,   "x",  "[ 1 2 ]",          0,   0, "x[\n1\n2\n]"      },
                { L_,   "x",  "[ 1 2 ]",          0,   1, "x[\n 1\n 2\n]"    },
                { L_,   "x",  "[ 1 2 ]",          1,   0, "x[\n1\n2\n]"      },
                { L_,   "x",  "[ 1 2 ]",          1,   1,
                                                         "x [\n  1\n  2\n ]" },
                { L_,   "x",  "[ 1 2 ]",          2,   3,
                                 "x      [\n         1\n         2\n      ]" },
                { L_,   "x",  "[ 1 2 ]",         -1,   3,
                                                "x[\n      1\n      2\n   ]" },
                { L_,   "x",  "[ 1 2 ]",         -2,   3,
                                       "x[\n         1\n         2\n      ]" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char *VALUE       = DATA[ti].d_value_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<Int64> val(&testAllocator);
                const char *endPos;
                bdepu_NTypesParser::parseInt64Array(&endPos, &val, VALUE);

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_NTypesParser::
                               generateInt64ArrayRaw(&buffer, val, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::
                                  generateInt64Array(&buffer, val, LEVEL, SPL);
                if (veryVerbose) {
                    P(&exp.front());
                    P(&buffer.front());
                    cout << endl;
                }
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 38: {
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
        //   generateIntArray(ArCh *buf, cArInt& val, int lev, int sp);
        //   generateIntArrayRaw(ArCh *buf, cArInt& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateIntArray*" << endl
                          << "===========================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_spec_p;          // specification string
                const char *d_value_p;         // specification value
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  spec  value           level  SPL  exp
                //----  ----  --------------  -----  ---  ------------------
                { L_,   "",   "[ ]",              0,   0, "[\n]"             },
                { L_,   "",   "[ ]",              0,   1, "[\n]"             },
                { L_,   "",   "[ ]",              1,   0, "[\n]"             },
                { L_,   "",   "[ ]",              1,   1, " [\n ]"           },
                { L_,   "",   "[ ]",              2,   3, "      [\n      ]" },
                { L_,   "",   "[ ]",             -1,   3, "[\n   ]"          },
                { L_,   "",   "[ ]",             -2,   3, "[\n      ]"       },
                { L_,   "",   "[ 1 ]",            0,   0, "[\n1\n]"          },
                { L_,   "",   "[ 1 ]",            0,   1, "[\n 1\n]"         },
                { L_,   "",   "[ 1 ]",            1,   0, "[\n1\n]"          },
                { L_,   "",   "[ 1 ]",            1,   1, " [\n  1\n ]"      },
                { L_,   "",   "[ 1 ]",            2,   3,
                                              "      [\n         1\n      ]" },
                { L_,   "",   "[ 1 ]",           -1,   3, "[\n      1\n   ]" },
                { L_,   "",   "[ 1 ]",           -2,   3,
                                                    "[\n         1\n      ]" },
                { L_,   "",   "[ 1 2 ]",          0,   0, "[\n1\n2\n]"       },
                { L_,   "",   "[ 1 2 ]",          0,   1, "[\n 1\n 2\n]"     },
                { L_,   "",   "[ 1 2 ]",          1,   0, "[\n1\n2\n]"       },
                { L_,   "",   "[ 1 2 ]",          1,   1, " [\n  1\n  2\n ]" },
                { L_,   "",   "[ 1 2 ]",          2,   3,
                                  "      [\n         1\n         2\n      ]" },
                { L_,   "",   "[ 1 2 ]",         -1,   3,
                                                 "[\n      1\n      2\n   ]" },
                { L_,   "",   "[ 1 2 ]",         -2,   3,
                                        "[\n         1\n         2\n      ]" },

                { L_,   "x",  "[ ]",              0,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              0,   1, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   1, "x [\n ]"          },
                { L_,   "x",  "[ ]",              2,   3, "x      [\n      ]"},
                { L_,   "x",  "[ ]",             -1,   3, "x[\n   ]"         },
                { L_,   "x",  "[ ]",             -2,   3, "x[\n      ]"      },
                { L_,   "x",  "[ 1 ]",            0,   0, "x[\n1\n]"         },
                { L_,   "x",  "[ 1 ]",            0,   1, "x[\n 1\n]"        },
                { L_,   "x",  "[ 1 ]",            1,   0, "x[\n1\n]"         },
                { L_,   "x",  "[ 1 ]",            1,   1, "x [\n  1\n ]"     },
                { L_,   "x",  "[ 1 ]",            2,   3,
                                             "x      [\n         1\n      ]" },
                { L_,   "x",  "[ 1 ]",           -1,   3,
                                                         "x[\n      1\n   ]" },
                { L_,   "x",  "[ 1 ]",           -2,   3,
                                                   "x[\n         1\n      ]" },
                { L_,   "x",  "[ 1 2 ]",          0,   0, "x[\n1\n2\n]"      },
                { L_,   "x",  "[ 1 2 ]",          0,   1, "x[\n 1\n 2\n]"    },
                { L_,   "x",  "[ 1 2 ]",          1,   0, "x[\n1\n2\n]"      },
                { L_,   "x",  "[ 1 2 ]",          1,   1,
                                                         "x [\n  1\n  2\n ]" },
                { L_,   "x",  "[ 1 2 ]",          2,   3,
                                 "x      [\n         1\n         2\n      ]" },
                { L_,   "x",  "[ 1 2 ]",         -1,   3,
                                                "x[\n      1\n      2\n   ]" },
                { L_,   "x",  "[ 1 2 ]",         -2,   3,
                                       "x[\n         1\n         2\n      ]" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char *VALUE       = DATA[ti].d_value_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<int> val(&testAllocator);
                const char *endPos;
                bdepu_NTypesParser::parseIntArray(&endPos, &val, VALUE);

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_NTypesParser::
                                 generateIntArrayRaw(&buffer, val, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::generateIntArray(&buffer, val, LEVEL, SPL);
                if (veryVerbose) {
                    P(&exp.front());
                    P(&buffer.front());
                    cout << endl;
                }
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 37: {
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
        //   generateFloatArray(ArCh *buf, cArFl& val, int lev, int sp);
        //   generateFloatArrayRaw(ArCh *buf, cArFl& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateFloatArray*" << endl
                          << "===========================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_spec_p;          // specification string
                const char *d_value_p;         // specification value
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  spec  value           level  SPL  exp
                //----  ----  --------------  -----  ---  ------------------
                { L_,   "",   "[ ]",              0,   0, "[\n]"             },
                { L_,   "",   "[ ]",              0,   1, "[\n]"             },
                { L_,   "",   "[ ]",              1,   0, "[\n]"             },
                { L_,   "",   "[ ]",              1,   1, " [\n ]"           },
                { L_,   "",   "[ ]",              2,   3, "      [\n      ]" },
                { L_,   "",   "[ ]",             -1,   3, "[\n   ]"          },
                { L_,   "",   "[ ]",             -2,   3, "[\n      ]"       },
                { L_,   "",   "[ 1 ]",            0,   0, "[\n1\n]"          },
                { L_,   "",   "[ 1 ]",            0,   1, "[\n 1\n]"         },
                { L_,   "",   "[ 1 ]",            1,   0, "[\n1\n]"          },
                { L_,   "",   "[ 1 ]",            1,   1, " [\n  1\n ]"      },
                { L_,   "",   "[ 1 ]",            2,   3,
                                              "      [\n         1\n      ]" },
                { L_,   "",   "[ 1 ]",           -1,   3, "[\n      1\n   ]" },
                { L_,   "",   "[ 1 ]",           -2,   3,
                                                    "[\n         1\n      ]" },
                { L_,   "",   "[ 1 2 ]",          0,   0, "[\n1\n2\n]"       },
                { L_,   "",   "[ 1 2 ]",          0,   1, "[\n 1\n 2\n]"     },
                { L_,   "",   "[ 1 2 ]",          1,   0, "[\n1\n2\n]"       },
                { L_,   "",   "[ 1 2 ]",          1,   1, " [\n  1\n  2\n ]" },
                { L_,   "",   "[ 1 2 ]",          2,   3,
                                  "      [\n         1\n         2\n      ]" },
                { L_,   "",   "[ 1 2 ]",         -1,   3,
                                                 "[\n      1\n      2\n   ]" },
                { L_,   "",   "[ 1 2 ]",         -2,   3,
                                        "[\n         1\n         2\n      ]" },

                { L_,   "x",  "[ ]",              0,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              0,   1, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   1, "x [\n ]"          },
                { L_,   "x",  "[ ]",              2,   3, "x      [\n      ]"},
                { L_,   "x",  "[ ]",             -1,   3, "x[\n   ]"         },
                { L_,   "x",  "[ ]",             -2,   3, "x[\n      ]"      },
                { L_,   "x",  "[ 1 ]",            0,   0, "x[\n1\n]"         },
                { L_,   "x",  "[ 1 ]",            0,   1, "x[\n 1\n]"        },
                { L_,   "x",  "[ 1 ]",            1,   0, "x[\n1\n]"         },
                { L_,   "x",  "[ 1 ]",            1,   1, "x [\n  1\n ]"     },
                { L_,   "x",  "[ 1 ]",            2,   3,
                                             "x      [\n         1\n      ]" },
                { L_,   "x",  "[ 1 ]",           -1,   3,
                                                         "x[\n      1\n   ]" },
                { L_,   "x",  "[ 1 ]",           -2,   3,
                                                   "x[\n         1\n      ]" },
                { L_,   "x",  "[ 1 2 ]",          0,   0, "x[\n1\n2\n]"      },
                { L_,   "x",  "[ 1 2 ]",          0,   1, "x[\n 1\n 2\n]"    },
                { L_,   "x",  "[ 1 2 ]",          1,   0, "x[\n1\n2\n]"      },
                { L_,   "x",  "[ 1 2 ]",          1,   1,
                                                         "x [\n  1\n  2\n ]" },
                { L_,   "x",  "[ 1 2 ]",          2,   3,
                                 "x      [\n         1\n         2\n      ]" },
                { L_,   "x",  "[ 1 2 ]",         -1,   3,
                                                "x[\n      1\n      2\n   ]" },
                { L_,   "x",  "[ 1 2 ]",         -2,   3,
                                       "x[\n         1\n         2\n      ]" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char *VALUE       = DATA[ti].d_value_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<float> val(&testAllocator);
                const char *endPos;
                bdepu_NTypesParser::parseFloatArray(&endPos, &val, VALUE);

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_NTypesParser::
                               generateFloatArrayRaw(&buffer, val, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::
                                  generateFloatArray(&buffer, val, LEVEL, SPL);
                if (veryVerbose) {
                    P(&exp.front());
                    P(&buffer.front());
                    cout << endl;
                }
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 36: {
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
        //   generateDoubleArray(ArCh *buf, cArDb& val, int lev, int sp);
        //   generateDoubleArrayRaw(ArCh *buf, cArDb& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDoubleArray*" << endl
                          << "==========================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_spec_p;          // specification string
                const char *d_value_p;         // specification value
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  spec  value           level  SPL  exp
                //----  ----  --------------  -----  ---  ------------------
                { L_,   "",   "[ ]",              0,   0, "[\n]"             },
                { L_,   "",   "[ ]",              0,   1, "[\n]"             },
                { L_,   "",   "[ ]",              1,   0, "[\n]"             },
                { L_,   "",   "[ ]",              1,   1, " [\n ]"           },
                { L_,   "",   "[ ]",              2,   3, "      [\n      ]" },
                { L_,   "",   "[ ]",             -1,   3, "[\n   ]"          },
                { L_,   "",   "[ ]",             -2,   3, "[\n      ]"       },
                { L_,   "",   "[ 1 ]",            0,   0, "[\n1\n]"          },
                { L_,   "",   "[ 1 ]",            0,   1, "[\n 1\n]"         },
                { L_,   "",   "[ 1 ]",            1,   0, "[\n1\n]"          },
                { L_,   "",   "[ 1 ]",            1,   1, " [\n  1\n ]"      },
                { L_,   "",   "[ 1 ]",            2,   3,
                                              "      [\n         1\n      ]" },
                { L_,   "",   "[ 1 ]",           -1,   3, "[\n      1\n   ]" },
                { L_,   "",   "[ 1 ]",           -2,   3,
                                                    "[\n         1\n      ]" },
                { L_,   "",   "[ 1 2 ]",          0,   0, "[\n1\n2\n]"       },
                { L_,   "",   "[ 1 2 ]",          0,   1, "[\n 1\n 2\n]"     },
                { L_,   "",   "[ 1 2 ]",          1,   0, "[\n1\n2\n]"       },
                { L_,   "",   "[ 1 2 ]",          1,   1, " [\n  1\n  2\n ]" },
                { L_,   "",   "[ 1 2 ]",          2,   3,
                                  "      [\n         1\n         2\n      ]" },
                { L_,   "",   "[ 1 2 ]",         -1,   3,
                                                 "[\n      1\n      2\n   ]" },
                { L_,   "",   "[ 1 2 ]",         -2,   3,
                                        "[\n         1\n         2\n      ]" },

                { L_,   "x",  "[ ]",              0,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              0,   1, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   1, "x [\n ]"          },
                { L_,   "x",  "[ ]",              2,   3, "x      [\n      ]"},
                { L_,   "x",  "[ ]",             -1,   3, "x[\n   ]"         },
                { L_,   "x",  "[ ]",             -2,   3, "x[\n      ]"      },
                { L_,   "x",  "[ 1 ]",            0,   0, "x[\n1\n]"         },
                { L_,   "x",  "[ 1 ]",            0,   1, "x[\n 1\n]"        },
                { L_,   "x",  "[ 1 ]",            1,   0, "x[\n1\n]"         },
                { L_,   "x",  "[ 1 ]",            1,   1, "x [\n  1\n ]"     },
                { L_,   "x",  "[ 1 ]",            2,   3,
                                             "x      [\n         1\n      ]" },
                { L_,   "x",  "[ 1 ]",           -1,   3,
                                                         "x[\n      1\n   ]" },
                { L_,   "x",  "[ 1 ]",           -2,   3,
                                                   "x[\n         1\n      ]" },
                { L_,   "x",  "[ 1 2 ]",          0,   0, "x[\n1\n2\n]"      },
                { L_,   "x",  "[ 1 2 ]",          0,   1, "x[\n 1\n 2\n]"    },
                { L_,   "x",  "[ 1 2 ]",          1,   0, "x[\n1\n2\n]"      },
                { L_,   "x",  "[ 1 2 ]",          1,   1,
                                                         "x [\n  1\n  2\n ]" },
                { L_,   "x",  "[ 1 2 ]",          2,   3,
                                 "x      [\n         1\n         2\n      ]" },
                { L_,   "x",  "[ 1 2 ]",         -1,   3,
                                                "x[\n      1\n      2\n   ]" },
                { L_,   "x",  "[ 1 2 ]",         -2,   3,
                                       "x[\n         1\n         2\n      ]" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char *VALUE       = DATA[ti].d_value_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<double> val(&testAllocator);
                const char *endPos;
                bdepu_NTypesParser::parseDoubleArray(&endPos, &val, VALUE);

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string SPECSTR = SPEC;
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                bdepu_NTypesParser::
                              generateDoubleArrayRaw(&buffer, val, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), SPECSTR.begin(), SPECSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::
                                 generateDoubleArray(&buffer, val, LEVEL, SPL);
                if (veryVerbose) {
                    P(&exp.front());
                    P(&buffer.front());
                    cout << endl;
                }
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 35: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DATETIME ARRAY
        //
        // Concerns:
        //   That 'generateDatetimeArray' and 'generateDatetimeArrayRaw' output
        //   values into char vectors as specced, with proper indentation and
        //   trimming of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char array, load the variable
        //   'val' by parsing a table-driven string, and output 'val' using
        //   using the two routines, checking the result against values in
        //   the table.
        //
        // Testing:
        //   generateDatetimeArray(ArCh *buf, cArDT& val, int lev, int sp);
        //   generateDatetimeArrayRaw(ArCh *buf, cArDT& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDatetimeArray*" << endl
                          << "==============================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // junk string
                const char *d_value_p;         // specification value
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk  value           level  SPL  exp
                //----  ----  --------------  -----  ---  ------------------
                { L_,   "",   "[ ]",              0,   0, "[\n]"             },
                { L_,   "",   "[ ]",              0,   1, "[\n]"             },
                { L_,   "",   "[ ]",              1,   0, "[\n]"             },
                { L_,   "",   "[ ]",              1,   1, " [\n ]"           },
                { L_,   "",   "[ ]",              2,   3, "      [\n      ]" },
                { L_,   "",   "[ ]",             -1,   3, "[\n   ]"          },
                { L_,   "",   "[ ]",             -2,   3, "[\n      ]"       },
                { L_,   "",   "[ 1993/04/12 11:33 ]",
                                                  0,   0,
                                             "[\n1993/04/12 11:33:00.000\n]" },
                { L_,   "",   "[ 1993/04/12 11:33 ]",
                                                  0,   1,
                                            "[\n 1993/04/12 11:33:00.000\n]" },
                { L_,   "",   "[ 1993/04/12 11:33 ]",
                                                  1,   0,
                                             "[\n1993/04/12 11:33:00.000\n]" },
                { L_,   "",   "[ 1993/04/12 11:33 ]",
                                                  1,   1,
                                         " [\n  1993/04/12 11:33:00.000\n ]" },
                { L_,   "",   "[ 1993/04/12 11:33 ]",
                                                  2,   3,
                        "      [\n         1993/04/12 11:33:00.000\n      ]" },
                { L_,   "",   "[ 1993/04/12 11:33 ]",
                                                 -1,   3,
                                    "[\n      1993/04/12 11:33:00.000\n   ]" },
                { L_,   "",   "[ 1993/04/12 11:33 ]",  -2,   3,
                              "[\n         1993/04/12 11:33:00.000\n      ]" },
                { L_,   "",   "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                  0,   0,
                    "[\n1993/04/12 11:33:00.000\n1712/11/17 01:29:00.000\n]" },
                { L_,   "",   "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                  0,   1,
                  "[\n 1993/04/12 11:33:00.000\n 1712/11/17 01:29:00.000\n]" },
                { L_,   "",   "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                  1,   0,
                    "[\n1993/04/12 11:33:00.000\n1712/11/17 01:29:00.000\n]" },
                { L_,   "",   "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                  1,   1,
              " [\n  1993/04/12 11:33:00.000\n  1712/11/17 01:29:00.000\n ]" },
                { L_,   "",   "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                  2,   3,
                               "      [\n         1993/04/12 11:33:00.000\n"
                                 "         1712/11/17 01:29:00.000\n      ]" },
                { L_,   "",   "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                 -1,   3,
     "[\n      1993/04/12 11:33:00.000\n      1712/11/17 01:29:00.000\n   ]" },
                { L_,   "",   "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                 -2,   3,
                                     "[\n         1993/04/12 11:33:00.000\n"
                                 "         1712/11/17 01:29:00.000\n      ]" },

                { L_,   "x",  "[ ]",              0,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              0,   1, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   0, "x[\n]"            },
                { L_,   "x",  "[ ]",              1,   1, "x [\n ]"          },
                { L_,   "x",  "[ ]",              2,   3, "x      [\n      ]"},
                { L_,   "x",  "[ ]",             -1,   3, "x[\n   ]"         },
                { L_,   "x",  "[ ]",             -2,   3, "x[\n      ]"      },
                { L_,   "x",  "[ 1993/04/12 11:33 ]",   0,   0,
                                            "x[\n1993/04/12 11:33:00.000\n]" },
                { L_,   "x",  "[ 1993/04/12 11:33 ]",   0,   1,
                                           "x[\n 1993/04/12 11:33:00.000\n]" },
                { L_,   "x",  "[ 1993/04/12 11:33 ]",   1,   0,
                                            "x[\n1993/04/12 11:33:00.000\n]" },
                { L_,   "x",  "[ 1993/04/12 11:33 ]",   1,   1,
                                        "x [\n  1993/04/12 11:33:00.000\n ]" },
                { L_,   "x",  "[ 1993/04/12 11:33 ]",   2,   3,
                       "x      [\n         1993/04/12 11:33:00.000\n      ]" },
                { L_,   "x",  "[ 1993/04/12 11:33 ]",  -1,   3,
                                   "x[\n      1993/04/12 11:33:00.000\n   ]" },
                { L_,   "x",  "[ 1993/04/12 11:33 ]",  -2,   3,
                             "x[\n         1993/04/12 11:33:00.000\n      ]" },
                { L_,   "x",  "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                  0,   0,
                   "x[\n1993/04/12 11:33:00.000\n1712/11/17 01:29:00.000\n]" },
                { L_,   "x",  "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                  0,   1,
                 "x[\n 1993/04/12 11:33:00.000\n 1712/11/17 01:29:00.000\n]" },
                { L_,   "x",  "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                  1,   0,
                   "x[\n1993/04/12 11:33:00.000\n1712/11/17 01:29:00.000\n]" },
                { L_,   "x",  "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                  1,   1,
             "x [\n  1993/04/12 11:33:00.000\n  1712/11/17 01:29:00.000\n ]" },
                { L_,   "x",  "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                  2,   3,
                              "x      [\n         1993/04/12 11:33:00.000\n"
                                 "         1712/11/17 01:29:00.000\n      ]" },
                { L_,   "x",  "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                 -1,   3,
    "x[\n      1993/04/12 11:33:00.000\n      1712/11/17 01:29:00.000\n   ]" },
                { L_,   "x",  "[ 1993/04/12 11:33 1712/11/17 01:29 ]",
                                                 -2,   3,
                                    "x[\n         1993/04/12 11:33:00.000\n"
                                 "         1712/11/17 01:29:00.000\n      ]" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const char *VALUE       = DATA[ti].d_value_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<bdet_Datetime> val(&testAllocator);
                const char *endPos;
                bdepu_NTypesParser::parseDatetimeArray(&endPos, &val, VALUE);

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::
                            generateDatetimeArrayRaw(&buffer, val, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::
                               generateDatetimeArray(&buffer, val, LEVEL, SPL);
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 34: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DATE ARRAY
        //
        // Concerns:
        //   That 'generateDateArray' and 'generateDateArrayRaw' output values
        //   into char vectors as specced, with proper indentation and trimming
        //   of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char array, load the variable
        //   'val' by parsing a table-driven string, and output 'val' using
        //   using the two routines, checking the result against values in
        //   the table.
        //
        // Testing:
        //   generateDateArray(ArCh *buf, cArDate& val, int lev, int sp);
        //   generateDateArrayRaw(ArCh *buf, cArDate& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDateArray*" << endl
                          << "==========================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // junk string
                const char *d_value_p;         // specification value
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk  value           level  SPL  exp
                //----  ----  --------------  -----  ---  ------------------
                { L_,   "",   "[ ]",              0,   0, "[\n]"             },
                { L_,   "",   "[ ]",              0,   1, "[\n]"             },
                { L_,   "",   "[ ]",              1,   0, "[\n]"             },
                { L_,   "",   "[ ]",              1,   1, " [\n ]"           },
                { L_,   "",   "[ ]",              2,   3, "      [\n      ]" },
                { L_,   "",   "[ ]",             -1,   3, "[\n   ]"          },
                { L_,   "",   "[ ]",             -2,   3, "[\n      ]" },
                { L_,   "",   "[ 1993/04/12 ]",   0,   0, "[\n1993/04/12\n]" },
                { L_,   "",   "[ 1993/04/12 ]",   0,   1,
                                                         "[\n 1993/04/12\n]" },
                { L_,   "",   "[ 1993/04/12 ]",   1,   0, "[\n1993/04/12\n]" },
                { L_,   "",   "[ 1993/04/12 ]",   1,   1,
                                                      " [\n  1993/04/12\n ]" },
                { L_,   "",   "[ 1993/04/12 ]",   2,   3,
                                     "      [\n         1993/04/12\n      ]" },
                { L_,   "",   "[ 1993/04/12 ]",  -1,   3,
                                                 "[\n      1993/04/12\n   ]" },
                { L_,   "",   "[ 1993/04/12 ]",  -2,   3,
                                           "[\n         1993/04/12\n      ]" },
                { L_,   "",   "[ 1993/04/12 1712/11/17 ]",
                                                  0,   0,
                                              "[\n1993/04/12\n1712/11/17\n]" },
                { L_,   "",   "[ 1993/04/12 1712/11/17 ]",
                                                  0,   1,
                                            "[\n 1993/04/12\n 1712/11/17\n]" },
                { L_,   "",   "[ 1993/04/12 1712/11/17 ]",
                                                  1,   0,
                                              "[\n1993/04/12\n1712/11/17\n]" },
                { L_,   "",   "[ 1993/04/12 1712/11/17 ]",
                                                  1,   1,
                                        " [\n  1993/04/12\n  1712/11/17\n ]" },
                { L_,   "",   "[ 1993/04/12 1712/11/17 ]",
                                                  2,   3,
                "      [\n         1993/04/12\n         1712/11/17\n      ]" },
                { L_,   "",   "[ 1993/04/12 1712/11/17 ]",
                                                 -1,   3,
                               "[\n      1993/04/12\n      1712/11/17\n   ]" },
                { L_,   "",   "[ 1993/04/12 1712/11/17 ]",
                                                 -2,   3,
                      "[\n         1993/04/12\n         1712/11/17\n      ]" },

                { L_,   "A@", "[ ]",              0,   0, "A@[\n]"           },
                { L_,   "A@", "[ ]",              0,   1, "A@[\n]"           },
                { L_,   "A@", "[ ]",              1,   0, "A@[\n]"           },
                { L_,   "A@", "[ ]",              1,   1, "A@ [\n ]"         },
                { L_,   "A@", "[ ]",              2,   3,"A@      [\n      ]"},
                { L_,   "A@", "[ ]",             -1,   3,"A@[\n   ]"         },
                { L_,   "A@", "[ ]",             -2,   3,"A@[\n      ]"      },
                { L_,   "A@", "[ 1993/04/12 ]",   0,   0,
                                                         "A@[\n1993/04/12\n]"},
                { L_,   "A@", "[ 1993/04/12 ]",   0,   1,
                                                        "A@[\n 1993/04/12\n]"},
                { L_,   "A@", "[ 1993/04/12 ]",   1,   0,
                                                         "A@[\n1993/04/12\n]"},
                { L_,   "A@", "[ 1993/04/12 ]",   1,   1,
                                                     "A@ [\n  1993/04/12\n ]"},
                { L_,   "A@", "[ 1993/04/12 ]",   2,   3,
                                    "A@      [\n         1993/04/12\n      ]"},
                { L_,   "A@", "[ 1993/04/12 ]",  -1,   3,
                                                "A@[\n      1993/04/12\n   ]"},
                { L_,   "A@", "[ 1993/04/12 ]",  -2,   3,
                                          "A@[\n         1993/04/12\n      ]"},
                { L_,   "A@", "[ 1993/04/12 1712/11/17 ]",
                                                  0,   0,
                                             "A@[\n1993/04/12\n1712/11/17\n]"},
                { L_,   "A@", "[ 1993/04/12 1712/11/17 ]",
                                                  0,   1,
                                           "A@[\n 1993/04/12\n 1712/11/17\n]"},
                { L_,   "A@", "[ 1993/04/12 1712/11/17 ]",
                                                  1,   0,
                                             "A@[\n1993/04/12\n1712/11/17\n]"},
                { L_,   "A@", "[ 1993/04/12 1712/11/17 ]",
                                                  1,   1,
                                       "A@ [\n  1993/04/12\n  1712/11/17\n ]"},
                { L_,   "A@", "[ 1993/04/12 1712/11/17 ]",
                                                  2,   3,
               "A@      [\n         1993/04/12\n         1712/11/17\n      ]"},
                { L_,   "A@", "[ 1993/04/12 1712/11/17 ]",
                                                 -1,   3,
                              "A@[\n      1993/04/12\n      1712/11/17\n   ]"},
                { L_,   "A@", "[ 1993/04/12 1712/11/17 ]",
                                                 -2,   3,
                     "A@[\n         1993/04/12\n         1712/11/17\n      ]"},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const char *VALUE       = DATA[ti].d_value_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<bdet_Date> val(&testAllocator);
                const char *endPos;
                bdepu_NTypesParser::parseDateArray(&endPos, &val, VALUE);

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::
                              generateDateArrayRaw(&buffer, val, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::
                                 generateDateArray(&buffer, val, LEVEL, SPL);
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 33: {
        // --------------------------------------------------------------------
        // TESTING GENERATE CHAR ARRAY
        //
        // Concerns:
        //   That 'generateCharArray' and 'generateCharArrayRaw' output chars
        //   into char vectors as specced, with proper indentation and trimming
        //   of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char array, load the variable
        //   'val' by parsing a table-driven string, and output 'val' using
        //   using the two routines, checking the result against values in
        //   the table.
        //
        // Testing:
        //   generateCharArray(ArCh *buf, cArCh& val, int lev, int sp);
        //   generateCharArrayRaw(ArCh *buf, cArCh& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateCharArray*" << endl
                          << "==========================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // junk string
                const char *d_value_p;         // specification value
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk  value           level  SPL  exp
                //----  ----  --------------  -----  ---  ------------------
                { L_,   "",   "[ ]",              0,   0, "[\n]"             },
                { L_,   "",   "[ ]",              0,   1, "[\n]"             },
                { L_,   "",   "[ ]",              1,   0, "[\n]"             },
                { L_,   "",   "[ ]",              1,   1, " [\n ]"           },
                { L_,   "",   "[ ]",              2,   3, "      [\n      ]" },
                { L_,   "",   "[ ]",             -1,   3, "[\n   ]"          },
                { L_,   "",   "[ ]",             -2,   3, "[\n      ]"       },
                { L_,   "",   "[ 'a' ]",          0,   0, "[\n'a'\n]"        },
                { L_,   "",   "[ 'a' ]",          0,   1, "[\n 'a'\n]"       },
                { L_,   "",   "[ 'a' ]",          1,   0, "[\n'a'\n]"        },
                { L_,   "",   "[ 'a' ]",          1,   1, " [\n  'a'\n ]"    },
                { L_,   "",   "[ 'a' ]",          2,   3,
                                            "      [\n         'a'\n      ]" },
                { L_,   "",   "[ 'a' ]",         -1,   3,
                                                        "[\n      'a'\n   ]" },
                { L_,   "",   "[ 'a' ]",         -2,   3,
                                                  "[\n         'a'\n      ]" },
                { L_,   "",   "[ 'a' 'b' ]",      0,   0, "[\n'a'\n'b'\n]"   },
                { L_,   "",   "[ 'a' 'b' ]",      0,   1, "[\n 'a'\n 'b'\n]" },
                { L_,   "",   "[ 'a' 'b' ]",      1,   0, "[\n'a'\n'b'\n]"   },
                { L_,   "",   "[ 'a' 'b' ]",      1,   1,
                                                      " [\n  'a'\n  'b'\n ]" },
                { L_,   "",   "[ 'a' 'b' ]",      2,   3,
                              "      [\n         'a'\n         'b'\n      ]" },
                { L_,   "",   "[ 'a' 'b' ]",     -1,   3,
                                             "[\n      'a'\n      'b'\n   ]" },
                { L_,   "",   "[ 'a' 'b' ]",     -2,   3,
                                    "[\n         'a'\n         'b'\n      ]" },

                { L_,   "#",  "[ ]",              0,   0, "#[\n]"            },
                { L_,   "#",  "[ ]",              0,   1, "#[\n]"            },
                { L_,   "#",  "[ ]",              1,   0, "#[\n]"            },
                { L_,   "#",  "[ ]",              1,   1, "# [\n ]"          },
                { L_,   "#",  "[ ]",              2,   3, "#      [\n      ]"},
                { L_,   "#",  "[ ]",             -1,   3, "#[\n   ]"         },
                { L_,   "#",  "[ ]",             -2,   3, "#[\n      ]"      },
                { L_,   "#",  "[ 'a' ]",          0,   0, "#[\n'a'\n]"       },
                { L_,   "#",  "[ 'a' ]",          0,   1, "#[\n 'a'\n]"      },
                { L_,   "#",  "[ 'a' ]",          1,   0, "#[\n'a'\n]"       },
                { L_,   "#",  "[ 'a' ]",          1,   1, "# [\n  'a'\n ]"   },
                { L_,   "#",  "[ 'a' ]",          2,   3,
                                           "#      [\n         'a'\n      ]" },
                { L_,   "#",  "[ 'a' ]",         -1,   3,
                                                       "#[\n      'a'\n   ]" },
                { L_,   "#",  "[ 'a' ]",         -2,   3,
                                                 "#[\n         'a'\n      ]" },
                { L_,   "#",  "[ 'a' 'b' ]",      0,   0, "#[\n'a'\n'b'\n]"  },
                { L_,   "#",  "[ 'a' 'b' ]",      0,   1,
                                                         "#[\n 'a'\n 'b'\n]" },
                { L_,   "#",  "[ 'a' 'b' ]",      1,   0, "#[\n'a'\n'b'\n]"  },
                { L_,   "#",  "[ 'a' 'b' ]",      1,   1,
                                                     "# [\n  'a'\n  'b'\n ]" },
                { L_,   "#",  "[ 'a' 'b' ]",      2,   3,
                             "#      [\n         'a'\n         'b'\n      ]" },
                { L_,   "#",  "[ 'a' 'b' ]",     -1,   3,
                                            "#[\n      'a'\n      'b'\n   ]" },
                { L_,   "#",  "[ 'a' 'b' ]",     -2,   3,
                                   "#[\n         'a'\n         'b'\n      ]" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const char *VALUE       = DATA[ti].d_value_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<char> val(&testAllocator);
                const char *endPos;
                bdepu_NTypesParser::parseCharArray(&endPos, &val, VALUE);

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::
                                generateCharArrayRaw(&buffer, val, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::
                                   generateCharArray(&buffer, val, LEVEL, SPL);
                if (veryVerbose) {
                    P(&exp.front());
                    P(&buffer.front());
                    cout << endl;
                }
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 32: {
        // --------------------------------------------------------------------
        // TESTING GENERATE TIME
        //
        // Concerns:
        //   That 'generateTime' and 'generateTimeRaw' output chars
        //   into char vectors as specced, with proper indentation and trimming
        //   of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char array, and use the two
        //   routines to append a value to it, and compare it against a
        //   table-driven expected value.
        //
        // Testing:
        //   generateTime(ArCh *buffer, bTime value, int level, int spaces);
        //   generateTimeRaw(ArCh *buffer, bTime value, int level, int spaces);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateTime*" << endl
                          << "=====================" << endl;

        {
            const bdet_Time VALUE( 1, 2, 3, 456);

            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // specification string
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk  level  SPL  exp
                //----  ----  -----  ---  ----------------------
                { L_,   "",       0,   0, "01:02:03.456"         },
                { L_,   "",       0,   1, "01:02:03.456"         },
                { L_,   "",       0,   1, "01:02:03.456"         },
                { L_,   "",       1,   0, "01:02:03.456"         },
                { L_,   "",       1,   1, " 01:02:03.456"        },
                { L_,   "",       2,   3, "      01:02:03.456"   },
                { L_,   "",      -1,   3, "01:02:03.456"         },
                { L_,   "+7^",    0,   0, "+7^01:02:03.456"        },
                { L_,   "+7^",    0,   1, "+7^01:02:03.456"        },
                { L_,   "+7^",    0,   1, "+7^01:02:03.456"        },
                { L_,   "+7^",    1,   0, "+7^01:02:03.456"        },
                { L_,   "+7^",    1,   1, "+7^ 01:02:03.456"       },
                { L_,   "+7^",    2,   3, "+7^      01:02:03.456"  },
                { L_,   "+7^",   -1,   3, "+7^01:02:03.456"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::
                                   generateTimeRaw(&buffer, VALUE, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::generateTime(&buffer, VALUE, LEVEL, SPL);
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 31: {
        // --------------------------------------------------------------------
        // TESTING GENERATE STRING
        //
        // Concerns:
        //   That 'generateString' and 'generateStringRaw' output chars
        //   into char vectors as specced, with proper indentation and trimming
        //   of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char array, and use the two
        //   routines to append a value to it, and compare it against a
        //   table-driven expected value.
        //
        // Testing:
        // generateString(ArCh *buffer, bStr value, int level, int spaces);
        // generateStringRaw(ArCh *buffer, bStr value, int level, int spaces);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateString*" << endl
                          << "=======================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // junk string
                const char *d_value_p;         // specification value
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk    value     level  SPL  exp
                //----  ------  --------  -----  ---  -------------
                { L_,   "",     "a",          0,   0, "a"           },
                { L_,   "",     "a",          0,   1, "a"           },
                { L_,   "",     "a",          0,   1, "a"           },
                { L_,   "",     "a",          1,   0, "a"           },
                { L_,   "",     "a",          1,   1, " a"          },
                { L_,   "",     "a",          2,   3, "      a"     },
                { L_,   "",     "a",         -1,   3, "a"           },
                { L_,   "",     "b b",       -1,   3, "\"b b\""     },
                { L_,   "#78",  "a",          0,   0, "#78a"          },
                { L_,   "#78",  "a",          0,   1, "#78a"          },
                { L_,   "#78",  "a",          0,   1, "#78a"          },
                { L_,   "#78",  "a",          1,   0, "#78a"          },
                { L_,   "#78",  "a",          1,   1, "#78 a"         },
                { L_,   "#78",  "a",          2,   3, "#78      a"    },
                { L_,   "#78",  "b b",        2,   3, "#78      \"b b\"" },
                { L_,   "#78",  "a",         -1,   3, "#78a"          },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const char *const VALUE = DATA[ti].d_value_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::
                                 generateStringRaw(&buffer, VALUE, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::generateString(&buffer, VALUE, LEVEL, SPL);

                bsl::string expStr, bufStr;
                expStr.append(exp.begin(), exp.end() - exp.begin());
                bufStr.append(buffer.begin(), buffer.end() - buffer.begin());
                LOOP3_ASSERT(LINE, expStr, bufStr, exp == buffer);
            }
        }
      } break;
      case 30: {
        // --------------------------------------------------------------------
        // TESTING GENERATE SHORT
        //
        // Concerns:
        //   That 'generateShort' and 'generateShortRaw' output chars
        //   into char vectors as specced, with proper indentation and trimming
        //   of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char array, and use the two
        //   routines to append a value to it, and compare it against a
        //   table-driven expected value.
        //
        // Testing:
        // generateShort(ArCh *buffer, short value, int level, int spaces);
        // generateShortRaw(ArCh *buffer, short value, int level, int spaces);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateShort*" << endl
                          << "======================" << endl;

        {
            const short VALUE(-1234);

            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // junk string
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk    level  SPL  exp
                //----  ------  -----  ---  -------------
                { L_,   "",         0,   0, "-1234"         },
                { L_,   "",         0,   1, "-1234"         },
                { L_,   "",         0,   1, "-1234"         },
                { L_,   "",         1,   0, "-1234"         },
                { L_,   "",         1,   1, " -1234"        },
                { L_,   "",         2,   3, "      -1234"   },
                { L_,   "",        -1,   3, "-1234"         },
                { L_,   "#@4",      0,   0, "#@4-1234"      },
                { L_,   "#@4",      0,   1, "#@4-1234"      },
                { L_,   "#@4",      0,   1, "#@4-1234"      },
                { L_,   "#@4",      1,   0, "#@4-1234"      },
                { L_,   "#@4",      1,   1, "#@4 -1234"     },
                { L_,   "#@4",      2,   3, "#@4      -1234"},
                { L_,   "#@4",     -1,   3, "#@4-1234"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::
                                  generateShortRaw(&buffer, VALUE, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::generateShort(&buffer, VALUE, LEVEL, SPL);
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 29: {
        // --------------------------------------------------------------------
        // TESTING GENERATE INT64
        //
        // Concerns:
        //   That 'generateInt64' and 'generateInt64Raw' output chars
        //   into char vectors as specced, with proper indentation and trimming
        //   of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char array, and use the two
        //   routines to append a value to it, and compare it against a
        //   table-driven expected value.
        //
        // Testing:
        // generateInt64(ArCh *buffer, int64 value, int level, int spaces);
        // generateInt64Raw(ArCh *buffer, int64 value, int level, int spaces);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateInt64*" << endl
                          << "======================" << endl;

        {
            const Int64 VALUE(-987654321054321LL);

            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // junk string
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk    level  SPL  exp
                //----  ------  -----  ---  -------------
                { L_,   "",         0,   0, "-987654321054321"         },
                { L_,   "",         0,   1, "-987654321054321"         },
                { L_,   "",         0,   1, "-987654321054321"         },
                { L_,   "",         1,   0, "-987654321054321"         },
                { L_,   "",         1,   1, " -987654321054321"        },
                { L_,   "",         2,   3, "      -987654321054321"   },
                { L_,   "",        -1,   3, "-987654321054321"         },
                { L_,   "#@4",      0,   0, "#@4-987654321054321"      },
                { L_,   "#@4",      0,   1, "#@4-987654321054321"      },
                { L_,   "#@4",      0,   1, "#@4-987654321054321"      },
                { L_,   "#@4",      1,   0, "#@4-987654321054321"      },
                { L_,   "#@4",      1,   1, "#@4 -987654321054321"     },
                { L_,   "#@4",      2,   3, "#@4      -987654321054321"},
                { L_,   "#@4",     -1,   3, "#@4-987654321054321"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::
                                  generateInt64Raw(&buffer, VALUE, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::generateInt64(&buffer, VALUE, LEVEL, SPL);
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 28: {
        // --------------------------------------------------------------------
        // TESTING GENERATE INT
        //
        // Concerns:
        //   That 'generateInt' and 'generateIntRaw' output chars
        //   into char vectors as specced, with proper indentation and trimming
        //   of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char array, and use the two
        //   routines to append a value to it, and compare it against a
        //   table-driven expected value.
        //
        // Testing:
        //   generateInt(ArCh *buffer, int value, int level, int spaces);
        //   generateIntRaw(ArCh *buffer, int value, int level, int spaces);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateInt*" << endl
                          << "====================" << endl;

        const int VALUE(-1234);

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // junk string
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk    level  SPL  exp
                //----  ------  -----  ---  -------------
                { L_,   "",         0,   0, "-1234"         },
                { L_,   "",         0,   1, "-1234"         },
                { L_,   "",         0,   1, "-1234"         },
                { L_,   "",         1,   0, "-1234"         },
                { L_,   "",         1,   1, " -1234"        },
                { L_,   "",         2,   3, "      -1234"   },
                { L_,   "",        -1,   3, "-1234"         },
                { L_,   "#@4",      0,   0, "#@4-1234"      },
                { L_,   "#@4",      0,   1, "#@4-1234"      },
                { L_,   "#@4",      0,   1, "#@4-1234"      },
                { L_,   "#@4",      1,   0, "#@4-1234"      },
                { L_,   "#@4",      1,   1, "#@4 -1234"     },
                { L_,   "#@4",      2,   3, "#@4      -1234"},
                { L_,   "#@4",     -1,   3, "#@4-1234"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::generateIntRaw(&buffer, VALUE, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::generateInt(&buffer, VALUE, LEVEL, SPL);
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 27: {
        // --------------------------------------------------------------------
        // TESTING GENERATE FLOAT
        //
        // Concerns:
        //   That 'generateFloat' and 'generateFloatRaw' output chars
        //   into char vectors as specced, with proper indentation and trimming
        //   of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char array, and use the two
        //   routines to append a value to it, and compare it against a
        //   table-driven expected value.
        //
        // Testing:
        // generateFloat(ArCh *buffer, float value, int level, int spaces);
        // generateFloatRaw(ArCh *buffer, float value, int level,int spaces);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateFloat*" << endl
                          << "=======================" << endl;

        const float VALUE(12.34e12);

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // junk string
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk    level  SPL  exp
                //----  ------  -----  ---  -------------
                { L_,   "",         0,   0, "1.234e13"           },
                { L_,   "",         0,   1, "1.234e13"           },
                { L_,   "",         0,   1, "1.234e13"           },
                { L_,   "",         1,   0, "1.234e13"           },
                { L_,   "",         1,   1, " 1.234e13"          },
                { L_,   "",         2,   3, "      1.234e13"     },
                { L_,   "",        -1,   3, "1.234e13"           },
                { L_,   "u%$",      0,   0, "u%$1.234e13"          },
                { L_,   "u%$",      0,   1, "u%$1.234e13"          },
                { L_,   "u%$",      0,   1, "u%$1.234e13"          },
                { L_,   "u%$",      1,   0, "u%$1.234e13"          },
                { L_,   "u%$",      1,   1, "u%$ 1.234e13"         },
                { L_,   "u%$",      2,   3, "u%$      1.234e13"    },
                { L_,   "u%$",     -1,   3, "u%$1.234e13"          },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::
                                  generateFloatRaw(&buffer, VALUE, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::generateFloat(&buffer, VALUE, LEVEL, SPL);
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 26: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DOUBLE
        //
        // Concerns:
        //   That 'generateDouble' and 'generateDoubleRaw' output chars
        //   into char vectors as specced, with proper indentation and trimming
        //   of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char array, and use the two
        //   routines to append a value to it, and compare it against a
        //   table-driven expected value.
        //
        // Testing:
        // generateDouble(ArCh *buffer, double value, int level, int spaces);
        // generateDoubleRaw(ArCh *buffer, double value, int level,int spaces);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDouble*" << endl
                          << "=======================" << endl;

        const double VALUE(12.34e12);

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // junk string
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk    level  SPL  exp
                //----  ------  -----  ---  -------------
                { L_,   "",         0,   0, "1.234e13"           },
                { L_,   "",         0,   1, "1.234e13"           },
                { L_,   "",         0,   1, "1.234e13"           },
                { L_,   "",         1,   0, "1.234e13"           },
                { L_,   "",         1,   1, " 1.234e13"          },
                { L_,   "",         2,   3, "      1.234e13"     },
                { L_,   "",        -1,   3, "1.234e13"           },
                { L_,   "u%$",      0,   0, "u%$1.234e13"          },
                { L_,   "u%$",      0,   1, "u%$1.234e13"          },
                { L_,   "u%$",      0,   1, "u%$1.234e13"          },
                { L_,   "u%$",      1,   0, "u%$1.234e13"          },
                { L_,   "u%$",      1,   1, "u%$ 1.234e13"         },
                { L_,   "u%$",      2,   3, "u%$      1.234e13"    },
                { L_,   "u%$",     -1,   3, "u%$1.234e13"          },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::
                                 generateDoubleRaw(&buffer, VALUE, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::generateDouble(&buffer, VALUE, LEVEL, SPL);
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 25: { // TBD datetime not date
        // --------------------------------------------------------------------
        // TESTING GENERATE DATETIME
        //
        // Concerns:
        //   That 'generateDatetime' and 'generateDatetimeRaw' output chars
        //   into char vectors as specced, with proper indentation and trimming
        //   of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char array, and use the two
        //   routines to append a value to it, and compare it against a
        //   table-driven expected value.
        //
        // Testing:
        // generateDatetime(ArCh *buffer, bDT value, int level, int spaces);
        // generateDatetimeRaw(ArCh *buffer, bDT value, int level, int spaces);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDatetime*" << endl
                          << "=========================" << endl;

        {
            const bdet_Date MATCH_DATE(   1, 2, 3);
            const bdet_Time MATCH_TIME(23, 45, 21, 789);
            const bdet_Datetime MATCH(MATCH_DATE, MATCH_TIME);

            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // junk string
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk   lvl  SPL  exp
                //----  -----  ---  ---  -------------------
                { L_,   "",      0,   0, "0001/02/03 23:45:21.789"          },
                { L_,   "",      0,   1, "0001/02/03 23:45:21.789"          },
                { L_,   "",      0,   1, "0001/02/03 23:45:21.789"          },
                { L_,   "",      1,   0, "0001/02/03 23:45:21.789"          },
                { L_,   "",      1,   1, " 0001/02/03 23:45:21.789"         },
                { L_,   "",      2,   3, "      0001/02/03 23:45:21.789"    },
                { L_,   "",     -1,   3, "0001/02/03 23:45:21.789"          },
                { L_,   "a7^",   0,   0, "a7^0001/02/03 23:45:21.789"       },
                { L_,   "a7^",   0,   1, "a7^0001/02/03 23:45:21.789"       },
                { L_,   "a7^",   0,   1, "a7^0001/02/03 23:45:21.789"       },
                { L_,   "a7^",   1,   0, "a7^0001/02/03 23:45:21.789"       },
                { L_,   "a7^",   1,   1, "a7^ 0001/02/03 23:45:21.789"      },
                { L_,   "a7^",   2,   3, "a7^      0001/02/03 23:45:21.789" },
                { L_,   "a7^",  -1,   3, "a7^0001/02/03 23:45:21.789"       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::
                               generateDatetimeRaw(&buffer, MATCH, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::generateDatetime(
                                                   &buffer, MATCH, LEVEL, SPL);
                bsl::string expStr, bufStr;
                expStr.append(exp.begin(), exp.end() - exp.begin());
                bufStr.append(buffer.begin(), buffer.end() - buffer.begin());
                LOOP3_ASSERT(LINE, expStr, bufStr, exp == buffer);
            }
        }
      } break;
      case 24: {
        // --------------------------------------------------------------------
        // TESTING GENERATE DATE
        //
        // Concerns:
        //   That 'generateDate' and 'generateDateRaw' output chars into
        //   char vectors as specced, with proper indentation and trimming
        //   of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char array, and use the two
        //   routines to append a value to it, and compare it against a
        //   table-driven expected value.
        //
        // Testing:
        //   generateDate(ArCh *buffer, bDate value, int level, int spaces);
        //   generateDateRaw(ArCh *buffer, bDate value, int level, int spaces);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDate*" << endl
                          << "=====================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // junk string
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk     level  SPL  exp
                //----  -----    -----  ---  -------------------
                { L_,   "",          0,   0, "0001/02/03"        },
                { L_,   "",          0,   1, "0001/02/03"        },
                { L_,   "",          0,   1, "0001/02/03"        },
                { L_,   "",          1,   0, "0001/02/03"        },
                { L_,   "",          1,   1, " 0001/02/03"       },
                { L_,   "",          2,   3, "      0001/02/03"  },
                { L_,   "",         -1,   3, "0001/02/03"        },
                { L_,   "x7&",       0,   0, "x7&0001/02/03"   },
                { L_,   "x7&",       0,   1, "x7&0001/02/03"   },
                { L_,   "x7&",       0,   1, "x7&0001/02/03"   },
                { L_,   "x7&",       1,   0, "x7&0001/02/03"   },
                { L_,   "x7&",       1,   1, "x7& 0001/02/03"  },
                { L_,   "x7&",       2,   3, "x7&      0001/02/03"},
                { L_,   "x7&",      -1,   3, "x7&0001/02/03"     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const bdet_Date VALUE( 1, 2, 3);

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::
                                   generateDateRaw(&buffer, VALUE, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::generateDate(&buffer, VALUE, LEVEL, SPL);
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING GENERATE CHAR
        //
        // Concerns:
        //   That 'generateChar' and 'generateCharRaw' output chars into
        //   char vectors as specced, with proper indentation and trimming
        //   of preexisting terminating nulls.
        //
        // Plan:
        //   Load some table-driven junk into a char vector, and use
        //   'generateChar' and 'generateCharRaw' to append a value to it, and
        //   compare it against a table-driven expected value.
        //
        // Testing:
        //   generateChar(ArCh *buffer, char value, int level, int spaces);
        //   generateCharRaw(ArCh *buffer, char value, int level, int spaces);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateChar*" << endl
                          << "=====================" << endl;

        {
            static const struct {
                int         d_lineNum;         // source line number
                const char *d_junk_p;          // specification string
                char        d_value;           // specification value
                int         d_level;           // specification level
                int         d_spacesPerLevel;  // spec. spaces per level
                const char *d_exp_p;           // expected result
            } DATA[] = {
                //line  junk    value     level  SPL  exp
                //----  ------  --------  -----  ---  -------------
                { L_,   "",     'a',          0,   0, "'a'"         },
                { L_,   "",     'a',          0,   1, "'a'"         },
                { L_,   "",     'a',          0,   1, "'a'"         },
                { L_,   "",     '7',          1,   0, "'7'"         },
                { L_,   "",     '7',          1,   1, " '7'"        },
                { L_,   "",     '7',          2,   3, "      '7'"   },
                { L_,   "",     'a',         -1,   3, "'a'"         },
                { L_,   "x",    'a',          0,   0, "x'a'"        },
                { L_,   "x",    '&',          0,   1, "x'&'"        },
                { L_,   "xxx",  '&',          0,   1, "xxx'&'"      },
                { L_,   "xxx",  '&',          1,   0, "xxx'&'"      },
                { L_,   "xxx",  'u',          1,   1, "xxx 'u'"     },
                { L_,   "xxx",  'U',          2,   3, "xxx      'U'"},
                { L_,   "xxx",  'U',         -1,   3, "xxx'U'"      },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE          = DATA[ti].d_lineNum;
                const char *const JUNK  = DATA[ti].d_junk_p;
                const char VALUE        = DATA[ti].d_value;
                const int LEVEL         = DATA[ti].d_level;
                const int SPL           = DATA[ti].d_spacesPerLevel;
                const char *const EXP   = DATA[ti].d_exp_p;
                const int curLen        = strlen(JUNK);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                bsl::vector<char> buffer(&testAllocator);
                const bsl::string JUNKSTR = JUNK;
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                bdepu_NTypesParser::
                                   generateCharRaw(&buffer, VALUE, LEVEL, SPL);
                const bsl::string EXPSTR = EXP;
                bsl::vector<char> exp(&testAllocator);
                exp.insert(exp.end(), EXPSTR.begin(), EXPSTR.end());
                LOOP_ASSERT(LINE, exp == buffer);

                buffer.clear();
                buffer.insert(buffer.end(), JUNKSTR.begin(), JUNKSTR.end());
                buffer.push_back('\0');
                exp.push_back('\0');
                bdepu_NTypesParser::generateChar(&buffer, VALUE, LEVEL, SPL);
                LOOP_ASSERT(LINE, exp == buffer);
            }
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING STRIP NULL
        //
        // Concerns:
        //   That strip null properly removes trailing nulls from vectors of
        //   chars, or does nothing if no trailing nulls are present.
        //
        // Plan:
        //   Load vectors with table-driven contents, apply 'stripnull', and
        //   verify that the results match a table-driven expected value.
        //
        // Testing:
        //   stripNull(ArCh *buffer);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING stripnull" << endl
                          << "=================" << endl;

        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_junk_p;   // junk to put into vector before call
                int         d_junklen;  // junk length
                const char *d_exp_p;    // expectation specification
                int         d_explen;   // expectation specification length
            } DATA[] = {
                //line  junk             length  exp              explen
                //----  ---------------  ------  ---------------  ------
                { L_,   "",                 0,   "",                 0   },
                { L_,   " ",                1,   " ",                1   },
                { L_,   "a",                1,   "a",                1   },
                { L_,   "\0",               1,   "",                 0   },
                { L_,   "  ",               2,   "  ",               2   },
                { L_,   "woof",             4,   "woof",             4   },
                { L_,   " \0",              2,   " ",                1   },
                { L_,   "\0\0",             2,   "\0",               1   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE           = DATA[ti].d_lineNum;
                const char *const JUNK   = DATA[ti].d_junk_p;
                const int         LENGTH = DATA[ti].d_junklen;
                const char *const EXP    = DATA[ti].d_exp_p;
                const int         EXPLEN = DATA[ti].d_explen;

                bsl::vector<char> result(&testAllocator);
                result.resize(LENGTH);
                for (int i = 0; i < LENGTH; ++i) {
                    result[i] = JUNK[i];
                }
                bsl::vector<char> exp(&testAllocator);
                exp.resize(EXPLEN);
                for (int i = 0; i < EXPLEN; ++i) {
                    exp[i] = EXP[i];
                }
                bdepu_NTypesParser::stripNull(&result);
                LOOP_ASSERT(LINE, exp == result);
            }
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING PARSE TIME ARRAY
        //
        // Concerns:
        //   That 'parseTimeArray' correctly parses char arrays, and detects
        //   errors properly.
        //
        // Plan:
        //   Load the input string from a table, attempt to parse it, and
        //   verify that success or failure is as expected according to
        //   table-driven 'FAIL', and if the result was success, verify that
        //   the length of the array and the values in the array were as
        //   expected.
        //
        // Testing:
        //   parseTimeArray(cchar **endPos, ArTime *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseTimeArray" << endl
                          << "======================" << endl;

        {
            const bdet_Time INIT_VALUE_1( 1, 1, 1);  // first init. value
            const bdet_Time INIT_VALUE_2(19, 7, 9);  // second init. value

            const bdet_Time MATCH_0(11,  1,  2);
            const bdet_Time MATCH_1(22, 34, 29);

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_length;   // expected length of array
            } DATA[] = {
                //line  spec             off  fail   length
                //----  ---------------  ---  ----   ----
                { L_,   "",                1,    1,    0 },
                { L_,   "[",               2,    1,    0 },
                { L_,   "  ",              3,    1,    0 },
                { L_,   "[ ]",             3,    0,    0 },
                { L_,   "[ 11:01:02 ]",   12,    0,    1 },
                { L_,   " [ 11:01:02 ]",  13,    0,    1 },
                { L_,   "  [ 11:01:02 ]", 14,    0,    1 },
                { L_,   "[ 11:01:02 22:34:29 ]",
                                          21,    0,    2 },
                { L_,   "[ 11:01:02 22:34:29 00:12:34 ]",
                                          30,    0,    3 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int LENGTH       = DATA[ti].d_length;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::vector<bdet_Time> result(&testAllocator);
                    result.resize(1, INIT_VALUE_1);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                        parseTimeArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }

                {  // test with first second value
                    bsl::vector<bdet_Time> result(&testAllocator);
                    result.resize(4, INIT_VALUE_2);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                        parseTimeArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }
            }
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING PARSE STRING ARRAY
        //
        // Concerns:
        //   That 'parseStringArray' correctly parses char arrays, and detects
        //   errors properly.
        //
        // Plan:
        //   Load the input string from a table, attempt to parse it, and
        //   verify that success or failure is as expected according to
        //   table-driven 'FAIL', and if the result was success, verify that
        //   the length of the array and the values in the array were as
        //   expected.
        //
        // Testing:
        //   parseShortArray(cchar **endPos, ArSh *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseShortArray" << endl
                          << "=======================" << endl;

        {
            const string INIT_VALUE_1 = "897";  // first initial value
            const string INIT_VALUE_2 = "'?%$"; // second initial value

            const string MATCH_0 = "abc";
            const string MATCH_1 = "&^ 87";

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_length;   // expected length of array
            } DATA[] = {
                //line  spec           off  fail   length
                //----  ----------     ---  ----   -------
                { L_,   "",              1,    1,  0       },
                { L_,   " ",             2,    1,  0       },
                { L_,   " [",            3,    1,  0       },
                { L_,   "[ ]",           3,    0,  0       },
                { L_,   "[ abc ]",       7,    0,  1       },
                { L_,   " [ abc ]",      8,    0,  1       },
                { L_,   "  [ abc ]",     9,    0,  1       },
                { L_,   "[ abc \"&^ 87\" ]", 
                                        15,    0,  2       },
                { L_,   "[ abc \"&^ 87\" 4 5 6 ]",
                                        21,    0,  5       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int LENGTH       = DATA[ti].d_length;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::vector<string> result(&testAllocator);
                    result.resize(1, INIT_VALUE_1);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                      parseStringArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }

                {  // test with first initial value
                    bsl::vector<string> result(&testAllocator);
                    result.resize(4, INIT_VALUE_2);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                      parseStringArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING PARSE SHORT ARRAY
        //
        // Concerns:
        //   That 'parseShortArray' correctly parses char arrays, and detects
        //   errors properly.
        //
        // Plan:
        //   Load the input string from a table, attempt to parse it, and
        //   verify that success or failure is as expected according to
        //   table-driven 'FAIL', and if the result was success, verify that
        //   the length of the array and the values in the array were as
        //   expected.
        //
        // Testing:
        //   parseShortArray(cchar **endPos, ArSh *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseShortArray" << endl
                          << "=======================" << endl;

        {
            const short INIT_VALUE_1 = -3;  // first initial value
            const short INIT_VALUE_2 =  9;  // second initial value

            const int MATCH_0 = 1;
            const int MATCH_1 = -8372;

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_length;   // expected length of array
            } DATA[] = {
                //line  spec          off  fail   length
                //----  ----------    ---  ----   -------
                { L_,   "",             1,    1,  0       },
                { L_,   " ",            2,    1,  0       },
                { L_,   " [",           3,    1,  0       },
                { L_,   "[ ]",          3,    0,  0       },
                { L_,   "[ 1 ]",        5,    0,  1       },
                { L_,   " [ 1 ]",       6,    0,  1       },
                { L_,   "  [ 1 ]",      7,    0,  1       },
                { L_,   "[ 1 -8372 ]", 11,    0,  2       },
                { L_,   "[ 1 -8372 4 5 6 ]",
                                       17,    0,  5       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int LENGTH       = DATA[ti].d_length;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::vector<short> result(&testAllocator);
                    result.resize(1, INIT_VALUE_1);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                       parseShortArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }

                {  // test with first initial value
                    bsl::vector<short> result(&testAllocator);
                    result.resize(4, INIT_VALUE_2);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                       parseShortArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING PARSE INT64 ARRAY
        //
        // Concerns:
        //   That 'parseInt64Array' correctly parses char arrays, and detects
        //   errors properly.
        //
        // Plan:
        //   Load the input string from a table, attempt to parse it, and
        //   verify that success or failure is as expected according to
        //   table-driven 'FAIL', and if the result was success, verify that
        //   the length of the array and the values in the array were as
        //   expected.
        //
        // Testing:
        //   parseInt64Array(cchar **endPos, ArInt64 *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseInt64Array" << endl
                          << "=======================" << endl;

        {
            const int INIT_VALUE_1 = -3;  // first initial value
            const int INIT_VALUE_2 =  9;  // second initial value

            const Int64 MATCH_0 = 1;
            const Int64 MATCH_1 = -123456789012345LL;

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_length;   // expected length of array
            } DATA[] = {
                //line  spec          off  fail   length
                //----  ----------    ---  ----   -------
                { L_,   "",             1,    1,  0       },
                { L_,   " ",            2,    1,  0       },
                { L_,   " [",           3,    1,  0       },
                { L_,   "[ ]",          3,    0,  0       },
                { L_,   "[ 1 ]",        5,    0,  1       },
                { L_,   " [ 1 ]",       6,    0,  1       },
                { L_,   "  [ 1 ]",      7,    0,  1       },
                { L_,   "[ 1 -123456789012345 ]",
                                       22,    0,  2       },
                { L_,   "[ 1 -123456789012345 3 4 5 ]",
                                       28,    0,  5       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int LENGTH       = DATA[ti].d_length;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::vector<Int64> result(&testAllocator);
                    result.resize(1, INIT_VALUE_1);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                       parseInt64Array(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }

                {  // test with first initial value
                    bsl::vector<Int64> result(&testAllocator);
                    result.resize(4, INIT_VALUE_2);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                       parseInt64Array(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING PARSE INT ARRAY
        //
        // Concerns:
        //   That 'parseIntArray' correctly parses char arrays, and detects
        //   errors properly.
        //
        // Plan:
        //   Load the input string from a table, attempt to parse it, and
        //   verify that success or failure is as expected according to
        //   table-driven 'FAIL', and if the result was success, verify that
        //   the length of the array and the values in the array were as
        //   expected.
        //
        // Testing:
        //   parseIntArray(cchar **endPos, ArInt *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseIntArray" << endl
                          << "=====================" << endl;

        {
            const int INIT_VALUE_1 = -3;  // first initial value
            const int INIT_VALUE_2 =  9;  // second initial value

            const int MATCH_0 = 1;
            const int MATCH_1 = -8372;

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_length;   // expected length of array
            } DATA[] = {
                //line  spec          off  fail   length
                //----  ----------    ---  ----   -------
                { L_,   "",             1,    1,  0       },
                { L_,   " ",            2,    1,  0       },
                { L_,   " [",           3,    1,  0       },
                { L_,   "[ ]",          3,    0,  0       },
                { L_,   "[ 1 ]",        5,    0,  1       },
                { L_,   " [ 1 ]",       6,    0,  1       },
                { L_,   "  [ 1 ]",      7,    0,  1       },
                { L_,   "[ 1 -8372 ]", 11,    0,  2       },
                { L_,   "[ 1 -8372 3 4 5]",
                                       16,    0,  5       },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int LENGTH       = DATA[ti].d_length;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::vector<int> result(&testAllocator);
                    result.resize(1, INIT_VALUE_1);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                         parseIntArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }

                {  // test with first initial value
                    bsl::vector<int> result(&testAllocator);
                    result.resize(4, INIT_VALUE_2);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                         parseIntArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING PARSE FLOAT ARRAY
        //
        // Concerns:
        //   That 'parseFloatArray' correctly parses char arrays, and detects
        //   errors properly.
        //
        // Plan:
        //   Load the input string from a table, attempt to parse it, and
        //   verify that success or failure is as expected according to
        //   table-driven 'FAIL', and if the result was success, verify that
        //   the length of the array and the values in the array were as
        //   expected.
        //
        // Testing:
        //   parseFloatArray(cchar **endPos, ArFl *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseFloatArray" << endl
                          << "=======================" << endl;

        {
            const float INIT_VALUE_1 = -3.0F;  // first initial value
            const float INIT_VALUE_2 =  9.1F;  // second initial value

            const float MATCH_0 =   1.73;
            const float MATCH_1 = -17.42e16;

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_length;   // expected length of array
                double      d_value;    // expected return value
            } DATA[] = {
                //line  spec                  off  fail   length
                //----  ----------            ---  ----   -------
                { L_,   "",                     1,    1,  0 },
                { L_,   " ",                    2,    1,  0 },
                { L_,   " [",                   3,    1,  0 },
                { L_,   "[ ]",                  3,    0,  0 },
                { L_,   "[1.73]",               6,    0,  1 },
                { L_,   "[ 1.73 ]",             8,    0,  1 },
                { L_,   " [ 1.73 ]",            9,    0,  1 },
                { L_,   "  [ 1.73 ]",          10,    0,  1 },
                { L_,   "[ 1.73 -17.42e16 ]",  18,    0,  2 },
                { L_,   "[ 1.73 -17.42e16 2.3 ]",
                                               22,    0,  3 },
                { L_,   "[   1.73   -17.42e16   ]",
                                               24,    0,  2 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int LENGTH       = DATA[ti].d_length;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::vector<float> result(&testAllocator);
                    result.resize(1, INIT_VALUE_1);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                       parseFloatArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE,
                                       LENGTH < 1 || near(result[0], MATCH_0));
                        LOOP_ASSERT(LINE,
                                       LENGTH < 2 || near(result[1], MATCH_1));
                    }
                }

                {  // test with first initial value
                    bsl::vector<float> result(&testAllocator);
                    result.resize(4, INIT_VALUE_2);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                       parseFloatArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE,
                                       LENGTH < 1 || near(result[0], MATCH_0));
                        LOOP_ASSERT(LINE,
                                       LENGTH < 2 || near(result[1], MATCH_1));
                    }
                }
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING PARSE DOUBLE ARRAY
        //
        // Concerns:
        //   That 'parseDoubleArray' correctly parses char arrays, and detects
        //   errors properly.
        //
        // Plan:
        //   Load the input string from a table, attempt to parse it, and
        //   verify that success or failure is as expected according to
        //   table-driven 'FAIL', and if the result was success, verify that
        //   the length of the array and the values in the array were as
        //   expected.
        //
        // Testing:
        //   parseDoubleArray(cchar **endPos, ArDb *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseDoubleArray" << endl
                          << "========================" << endl;

        {
            const double INIT_VALUE_1 = -3.0;  // first initial value
            const double INIT_VALUE_2 =  9.1;  // second initial value

            const double MATCH_0 =   1.73;
            const double MATCH_1 = -17.42e16;

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_length;   // expected length of array
                double      d_value;    // expected return value
            } DATA[] = {
                //line  spec                  off  fail   length
                //----  ----------            ---  ----   -------
                { L_,   "",                     1,    1,  0 },
                { L_,   " ",                    2,    1,  0 },
                { L_,   " [",                   3,    1,  0 },
                { L_,   "[ ]",                  3,    0,  0 },
                { L_,   "[1.73]",               6,    0,  1 },
                { L_,   "[ 1.73 ]",             8,    0,  1 },
                { L_,   " [ 1.73 ]",            9,    0,  1 },
                { L_,   "  [ 1.73 ]",          10,    0,  1 },
                { L_,   "[ 1.73 -17.42e16 ]",  18,    0,  2 },
                { L_,   "[ 1.73 -17.42e16 2.3 ]",
                                               22,    0,  3 },
                { L_,   "[   1.73   -17.42e16   ]",
                                               24,    0,  2 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int LENGTH       = DATA[ti].d_length;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::vector<double> result(&testAllocator);
                    result.resize(1, INIT_VALUE_1);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                      parseDoubleArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }

                {  // test with first initial value
                    bsl::vector<double> result(&testAllocator);
                    result.resize(4, INIT_VALUE_2);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                      parseDoubleArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING PARSE DATETIME ARRAY
        //
        // Concerns:
        //   That 'parseDatetimeArray' correctly parses char arrays, and
        //   detects errors properly.
        //
        // Plan:
        //   Load the input string from a table, attempt to parse it, and
        //   verify that success or failure is as expected according to
        //   table-driven 'FAIL', and if the result was success, verify that
        //   the length of the array and the values in the array were as
        //   expected.
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

        {
            const bdet_Date I1(   1, 1, 1);
            const bdet_Date I2(1923, 7, 9);
            const bdet_Date I3(1993, 1, 2);
            const bdet_Date I4(2000, 3,30);

            const bdet_Time IT1(23, 45, 21);
            const bdet_Time IT2(17, 22, 18);
            const bdet_Time IT3( 3,  4,  5);
            const bdet_Time IT4(15, 17, 28);

            const bdet_Datetime INIT_VALUE_1(I1, IT1);
            const bdet_Datetime INIT_VALUE_2(I2, IT2);
            const bdet_Datetime MATCH_0(I3, IT3);
            const bdet_Datetime MATCH_1(I4, IT4);

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_length;   // expected length of array
            } DATA[] = {
                //line spec                       off fail length
                //---- ---------------------      --- ---- ----
                { L_,  "",                          1,   1,  0 },
                { L_,  "[",                         2,   1,  0 },
                { L_,  "  ",                        3,   1,  0 },
                { L_,  "[ 1993 ]",                  8,   1,  1 },
                { L_,  "[ 1993/01 ]",              11,   1,  1 },
                { L_,  "[ 1993/01/02 ]",           14,   1,  1 },
                { L_,  "[ 1993/01/02 3 ]",         16,   1,  1 },
                { L_,  "[ 1993/01/02 3:04:05 ]",   22,   0,  1 },
                { L_,  " [ 1993/01/02 3:04:05 ]",  23,   0,  1 },
                { L_,  "  [ 1993/01/02 3:04:05 ]", 24,   0,  1 },
                { L_,  "  [  1993/01/02 3:04:05  ]",
                                                   26,   0,  1 },
                { L_,  "[ 1993/01/02 3:04:05 2000 ]",
                                                   27,   1,  2 },
                { L_,  "[ 1993/01/02 3:04:05 2000/03 ]",
                                                   30,   1,  2 },
                { L_,  "[ 1993/01/02 3:04:05 2000/03/30 15 ]",
                                                   36,   1,  2 },
                { L_,  "[ 1993/01/02 3:04:05 2000/03/30 15:17:28 ]",
                                                   42,   0,  2 },
                { L_,  "[ 1993/01/02 3:04:05 2000/03/30 15:17:28 "
                                            "2002/02/20 17:13:29 ]",
                                                   62,   0,  3 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int LENGTH       = DATA[ti].d_length;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::vector<bdet_Datetime> result(&testAllocator);
                    result.resize(1, INIT_VALUE_1);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                    parseDatetimeArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }

                {  // test with second initial value
                    bsl::vector<bdet_Datetime> result(&testAllocator);
                    result.resize(4, INIT_VALUE_2);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                    parseDatetimeArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    const int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, LENGTH < 2 || result[1] == MATCH_1);
                    }
                }
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING PARSE DATE ARRAY
        //
        // Concerns:
        //   That 'parseDateArray' correctly parses char arrays, and detects
        //   errors properly.
        //
        // Plan:
        //   Load the input string from a table, attempt to parse it, and
        //   verify that success or failure is as expected according to
        //   table-driven 'FAIL', and if the result was success, verify that
        //   the length of the array and the values in the array were as
        //   expected.
        //
        // Testing:
        //   parseDateArray(cchar **endPos, ArDate *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseDateArray" << endl
                          << "======================" << endl;

        {
            const bdet_Date INIT_VALUE_1(   1, 1, 1);  // first init. value
            const bdet_Date INIT_VALUE_2(1923, 7, 9);  // second init. value

            const bdet_Date MATCH_0(1993, 1,  2);
            const bdet_Date MATCH_1(2000, 3, 31);

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_length;   // length of the resulting array
            } DATA[] = {
                //line  spec                            off  fail   length
                //----  -------------------             ---  ----   ------
                { L_,   "",                               1,    1,  0 },
                { L_,   "[",                              2,    1,  0 },
                { L_,   "  ",                             3,    1,  0 },
                { L_,   "[ ]",                            3,    0,  0 },
                { L_,   "[ 1993]",                        7,    1,  0 },
                { L_,   "[ 1993/01]",                    10,    1,  0 },
                { L_,   "[1993/01/02]",                  12,    0,  1 },
                { L_,   "[ 1993/01/02 ]",                14,    0,  1 },
                { L_,   "[ 1993/01/32 ]",                14,    1,  0 },
                { L_,   "[ 2001/02/29 ]",                14,    1,  1 },
                { L_,   " [ 1993/01/02 ]",               15,    0,  1 },
                { L_,   "  [ 1993/01/02 ]",              16,    0,  1 },
                { L_,   "[ 1993/01/02 2000/03/31 ]",     25,    0,  2 },
                { L_,   "[ 1993/01/02 2001/02/29 ]",     25,    1,  2 },
                { L_,   "[ 1993/01/02 2000/03/31 1972/04/03 ]",
                                                         36,    0,  3 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int LENGTH       = DATA[ti].d_length;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::vector<bdet_Date> result(&testAllocator);
                    result.resize(1, INIT_VALUE_1);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                        parseDateArray(&endPos, &result, SPEC);
                    int overshoot = endPos - (SPEC + NUM);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || MATCH_0 == result[0]);
                        LOOP_ASSERT(LINE, LENGTH < 2 || MATCH_1 == result[1]);
                    }
                }

                {  // test with second initial value
                    bsl::vector<bdet_Date> result(&testAllocator);
                    result.resize(4, INIT_VALUE_2);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                        parseDateArray(&endPos, &result, SPEC);
                    int overshoot = endPos - (SPEC + NUM);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP_ASSERT(LINE, LENGTH == result.size());
                        LOOP_ASSERT(LINE, LENGTH < 1 || MATCH_0 == result[0]);
                        LOOP_ASSERT(LINE, LENGTH < 2 || MATCH_1 == result[1]);
                    }
                }
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING PARSE CHAR ARRAY
        //
        // Concerns:
        //   That 'parseCharArray' correctly parses char arrays, and detects
        //   errors properly.
        //
        // Plan:
        //   Load the input string from a table, attempt to parse it, and
        //   verify that success or failure is as expected according to
        //   table-driven 'FAIL', and if the result was success, verify that
        //   the length of the array and the values in the array were as
        //   expected.
        //
        // Testing:
        //   parseCharArray(cchar **endPos, ArCh *res, cchar *in);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING parseCharArray" << endl
                          << "======================" << endl;

        {
            const char INIT_VALUE_1 = 'q';  // first initial value
            const char INIT_VALUE_2 = 'z';  // second initial value

            const char MATCH_0 = 'a';
            const char MATCH_1 = 'k';

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_length;   // expected array length
            } DATA[] = {
                //line  spec            off  fail   length
                //----  ----------      ---  ----   ------
                { L_,   "",               1,    1,  0 },
                { L_,   " ",              2,    1,  0 },
                { L_,   " a",             2,    1,  0 },
                { L_,   " [",             3,    1,  0 },
                { L_,   "[ ]",            3,    0,  0 },
                { L_,   "[ 'a' ]",        7,    0,  1 },
                { L_,   "  ['a']",        7,    0,  1 },
                { L_,   " [ 'a' ]",       8,    0,  1 },
                { L_,   "  [ 'a' ]",      9,    0,  1 },
                { L_,   "  ['a''b']",    10,    1,  0 },
                { L_,   "  [ 'a' ] ",     9,    0,  1 },
                { L_,   "  ['a' 'k']",   11,    0,  2 },
                { L_,   "  [ 'a' 'k' ]", 13,    0,  2 },
                { L_,   "  [ 'a' 'k' '4' ]",
                                         17,    0,  3 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const int LENGTH       = DATA[ti].d_length;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    bsl::vector<char> result(&testAllocator);
                    result.resize(1, INIT_VALUE_1);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                        parseCharArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP3_ASSERT(LINE, LENGTH, result.size(),
                                                      LENGTH == result.size());
                        LOOP_ASSERT(LINE, 1 > LENGTH || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, 2 > LENGTH || result[1] == MATCH_1);
                    }
                }

                {  // test with second initial value
                    bsl::vector<char> result(&testAllocator);
                    result.resize(4, INIT_VALUE_2);
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                        parseCharArray(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    int overshoot = endPos - (SPEC + NUM);
                    if (rv) {
                        LOOP2_ASSERT(LINE, overshoot, overshoot < 0);
                    }
                    else {
                        LOOP2_ASSERT(LINE, overshoot, 0 == overshoot);
                        LOOP3_ASSERT(LINE, LENGTH, result.size(),
                                                      LENGTH == result.size());
                        LOOP_ASSERT(LINE, 1 > LENGTH || result[0] == MATCH_0);
                        LOOP_ASSERT(LINE, 2 > LENGTH || result[1] == MATCH_1);
                    }
                }
            }
        }
      } break;
      case 11: {
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
                int         d_hour;     // expected return year
                int         d_minute;   // expected return minute
                int         d_second;   // expected return second
            } DATA[] = {
                //line  spec             off  fail  hour  min  sec
                //----  ---------------  ---  ----  ----  ---  ---
                { L_,   "",                0,    1,    1,   1,   1 },
                { L_,   "1",               1,    1,    1,   1,   1 },
                { L_,   "  ",              2,    1,    1,   1,   1 },
                { L_,   "11:01:02",        8,    0,   11,   1,   2 },
                { L_,   " 11:01:02",       9,    0,   11,   1,   2 },
                { L_,   "  11:01:02",     10,    0,   11,   1,   2 },
                { L_,   "  11:01:02 ",    10,    0,   11,   1,   2 },
                { L_,   "  11:01:02  ",   10,    0,   11,   1,   2 },
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
                    int rv = bdepu_NTypesParser::
                                             parseTime(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    bdet_Time result = INITIAL_VALUE_2;
                    const char *endPos = "";
                    int rv = bdepu_NTypesParser::
                                             parseTime(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 10: {
        // TBD
// [10] parseString(cchar **endPos, bStr *res, cchar *in);
      } break;
      case 9: {
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
                //line  spec        base  off  fail  value
                //----  ----------  ----  ---  ----  -------
                { L_,   "",           10,   0,    1,  0      },
                { L_,   " ",          10,   1,    1,  0      },
                { L_,   "1",          10,   1,    0,  1      },
                { L_,   " a",         10,   1,    1,  0      },
                { L_,   " a",         16,   2,    0, 10      },
                { L_,   " 1",         10,   2,    0,  1      },
                { L_,   "  1",        10,   3,    0,  1      },
                { L_,   "  1 ",       10,   3,    0,  1      },
                { L_,   "  1  ",      10,   3,    0,  1      },
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
                    int rv = bdepu_NTypesParser::
                                      parseShort(&endPos, &result, SPEC, BASE);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    short result = INITIAL_VALUE_2;
                    const char *endPos = "";
                    int rv = bdepu_NTypesParser::
                                      parseShort(&endPos, &result, SPEC, BASE);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 8: {
        // TBD
// [ 8] parseIntegerLocator(cchar **endPos, int *result, cchar *inputString);
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
                //line  spec        base  off  fail  value
                //----  ----------  ----  ---  ----  -------
                { L_,   "",           10,   0,    1,  0      },
                { L_,   " ",          10,   1,    1,  0      },
                { L_,   "1",          10,   1,    0,  1      },
                { L_,   " a",         10,   1,    1,  0      },
                { L_,   " a",         16,   2,    0, 10      },
                { L_,   " 1",         10,   2,    0,  1      },
                { L_,   "  1",        10,   3,    0,  1      },
                { L_,   "  1 ",       10,   3,    0,  1      },
                { L_,   "  1  ",      10,   3,    0,  1      },
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
                    int rv = bdepu_NTypesParser::
                                      parseInt64(&endPos, &result, SPEC, BASE);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1
                                                   : (int)VALUE));
                }

                {  // test with second initial value
                    Int64 result = INITIAL_VALUE_2;
                    const char *endPos = "";
                    int rv = bdepu_NTypesParser::
                                      parseInt64(&endPos, &result, SPEC, BASE);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2
                                                   : (int)VALUE));
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
                //line  spec        base  off  fail  value
                //----  ----------  ----  ---  ----  -------
                { L_,   "",           10,   0,    1,  0      },
                { L_,   " ",          10,   1,    1,  0      },
                { L_,   "1",          10,   1,    0,  1      },
                { L_,   " a",         10,   1,    1,  0      },
                { L_,   " a",         16,   2,    0, 10      },
                { L_,   " 1",         10,   2,    0,  1      },
                { L_,   "  1",        10,   3,    0,  1      },
                { L_,   "  1 ",       10,   3,    0,  1      },
                { L_,   "  1  ",      10,   3,    0,  1      },
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
                    int rv = bdepu_NTypesParser::
                                        parseInt(&endPos, &result, SPEC, BASE);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    int result = INITIAL_VALUE_2;
                    const char *endPos = "";
                    int rv = bdepu_NTypesParser::
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
                //line  spec        off  fail  value
                //----  ----------  ---  ----  -------
                { L_,   "",           0,    1, 0       },
                { L_,   " ",          1,    1, 0       },
                { L_,   " a",         1,    1, 0       },
                { L_,   "1.0",        3,    0, 1.0     },
                { L_,   " 1.0",       4,    0, 1.0     },
                { L_,   "  1.0",      5,    0, 1.0     },
                { L_,   "  1.0 ",     5,    0, 1.0     },
                { L_,   "  1.0  ",    5,    0, 1.0     },
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
                    int rv = bdepu_NTypesParser::
                                            parseFloat(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    float result = INITIAL_VALUE_2;
                    const char *endPos = "";
                    int rv = bdepu_NTypesParser::
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
            const double INITIAL_VALUE_1 = -3.0;  // first initial value
            const double INITIAL_VALUE_2 =  9.1;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                double      d_value;    // expected return value
            } DATA[] = {
                //line  spec        off  fail  value
                //----  ----------  ---  ----  -------
                { L_,   "",           0,    1, 0       },
                { L_,   " ",          1,    1, 0       },
                { L_,   " a",         1,    1, 0       },
                { L_,   "1.0",        3,    0, 1.0     },
                { L_,   " 1.0",       4,    0, 1.0     },
                { L_,   "  1.0",      5,    0, 1.0     },
                { L_,   "  1.0 ",     5,    0, 1.0     },
                { L_,   "  1.0  ",    5,    0, 1.0     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE         = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const int NUM          = DATA[ti].d_offset;
                const int FAIL         = DATA[ti].d_fail;
                const double VALUE     = DATA[ti].d_value;
                const int curLen       = strlen(SPEC);

                if (curLen != oldLen) {
                    if (verbose) cout << "\ton strings of length "
                                      << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    double result = INITIAL_VALUE_1;
                    const char *endPos = 0;
                    int rv = bdepu_NTypesParser::
                                           parseDouble(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    double result = INITIAL_VALUE_2;
                    const char *endPos = "";
                    int rv = bdepu_NTypesParser::
                                           parseDouble(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 3: {
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
                { L_,  "",                    0,   1,    1,  1,   1, 1, 1, 1 },
                { L_,  "1",                   1,   1,    1,  1,   1, 1, 1, 1 },
                { L_,  "  ",                  2,   1,    1,  1,   1, 1, 1, 1 },
                { L_,  "1993/01/02 3:04:05", 18,   0, 1993,  1,   2, 3, 4, 5 },
                { L_,  " 1993/01/02 3:04:05",
                                             19,   0, 1993,  1,   2, 3, 4, 5 },
                { L_,  "  1993/01/02 3:04:05",
                                             20,   0, 1993,  1,   2, 3, 4, 5 },
                { L_,  "  1993/01/02 3:04:05 ",
                                             20,   0, 1993,  1,   2, 3, 4, 5 },
                { L_,  "  1993/01/02 3:04:05  ",
                                             20,   0, 1993,  1,   2, 3, 4, 5 },
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
                    int rv = bdepu_NTypesParser::
                                         parseDatetime(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    bdet_Datetime result = INITIAL_VALUE_2;
                    const char *endPos = "";
                    int rv = bdepu_NTypesParser::
                                         parseDatetime(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 2: {
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
                { L_,   "",                0,    1,   10,    10,  10 },
                { L_,   "1",               1,    1,   10,    10,  10 },
                { L_,   "  ",              2,    1,   10,    10,  10 },
                { L_,   "1993/01/02",     10,    0, 1993,     1,   2 },
                { L_,   " 1993/01/02",    11,    0, 1993,     1,   2 },
                { L_,   "  1993/01/02",   12,    0, 1993,     1,   2 },
                { L_,   "  1993/01/02 ",  12,    0, 1993,     1,   2 },
                { L_,   "  1993/01/02  ", 12,    0, 1993,     1,   2 },
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

                if (veryVerbose) {
                    P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P(SPEC)
                }

                bdet_Date value;
                // Avoid initializing an invalid date.
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
                    int rv = bdepu_NTypesParser::
                                             parseDate(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    bdet_Date result = INITIAL_VALUE_2;
                    const char *endPos = "";
                    int rv = bdepu_NTypesParser::
                                             parseDate(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 1: {
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
                //----  ----------  ---  ----  -------
                { L_,   "",           0,    1, 0       },
                { L_,   " ",          1,    1, 0       },
                { L_,   " a",         1,    1, 0       },
                { L_,   " '",         2,    1, 0       },
                { L_,   "'a'",        3,    0, 'a'     },
                { L_,   " 'a'",       4,    0, 'a'     },
                { L_,   "  'a'",      5,    0, 'a'     },
                { L_,   "  'a' ",     5,    0, 'a'     },
                { L_,   "  'a'  ",    5,    0, 'a'     },
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
                    int rv = bdepu_NTypesParser::
                                             parseChar(&endPos, &result, SPEC);
                    LOOP_ASSERT(LINE, SPEC + NUM == endPos);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    char result = INITIAL_VALUE_2;
                    const char *endPos = "";
                    int rv = bdepu_NTypesParser::
                                             parseChar(&endPos, &result, SPEC);
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
