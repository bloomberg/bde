// bdepu_typesparser.t.cpp              -*-C++-*-

#include <bdepu_typesparser.h>
#include <bdepu_parserimputil.h>
#include <bdepu_arrayparserimputil.h>

#include <bsls_platform.h>
#include <bslma_testallocator.h>
#include <bsls_platformutil.h>
#include <bsl_vector.h>
#include <bdet_datetime.h>
#include <bdet_date.h>
#include <bdet_time.h>
#include <bdeimp_fuzzy.h>                // for testing only

#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
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
//      parseCharArray(cchar **endPos, ArCh *res, cchar *in);
// [ 2] parseDate(cchar **endPos, bDate *res, cchar *in);
//      parseDateArray(cchar **endPos, ArDate *res, cchar *in);
// [ 3] parseDatetime(cchar **endPos, bDT *res, cchar *in);
//      parseDatetimeArray(cchar **endPos, ArDT *res, cchar *in);
// [26] parseDatetimeTz(cchar **endPos, bDTz *res, cchar *in);
//      parseDatetimeTzArray(cchar **endPos, ArDTz *res, cchar *in);
// [24] parseDateTz(cchar **endPos, bDatez *res, cchar *in);
//      parseDateTzArray(cchar **endPos, ArDatez *res, cchar *in);
// [ 4] parseDouble(cchar **endPos, double *res, cchar *in);
//      parseDoubleArray(cchar **endPos, ArDb *res, cchar *in);
// [ 5] parseFloat(cchar **endPos, double *res, cchar *in);
//      parseFloatArray(cchar **endPos, ArFl *res, cchar *in);
// [ 6] parseInt(cchar **endPos, int *res, cchar *in, int base = 10);
//      parseIntArray(cchar **endPos, ArInt *res, cchar *in);
// [ 7] parseInt64(cchar **endPos, int64 *res, cchar *in, int base = 10);
//      parseInt64Array(cchar **endPos, ArInt64 *res, cchar *in);
// [ 8] parseIntegerLocator(cchar **endPos, int *result, cchar *inputString);
// [ 9] parseShort(cchar **endPos, short *res, cchar *in, int base = 10);
//      parseShortArray(cchar **endPos, ArSh *res, cchar *in);
// [10] parseString(cchar **endPos, bStr *res, cchar *in);
//      parseStringArray(cchar **endPos, ArStr *res, cchar *in);
// [11] parseTime(cchar **endPos, bTime *res, cchar *in);
//      parseTimeArray(cchar **endPos, ArTime *res, cchar *in);
// [25] parseTimeTz(cchar **endPos, bTimez *res, cchar *in);
//      parseTimeTzArray(cchar **endPos, ArTimez *res, cchar *in);
// [12] stripNull(ArCh *buffer);
// [13] generateChar(ArCh *buffer, char value, int level, int spaces);
//      generateCharRaw(ArCh *buffer, char value, int level, int spaces);
//      generateCharArray(ArCh *buf, cArCh& val, int lev, int sp);
//      generateCharArrayRaw(ArCh *buf, cArCh& val, int lev, int sp);
// [14] generateDate(ArCh *buffer, bDate value, int level, int spaces);
//      generateDateRaw(ArCh *buffer, bDate value, int level, int spaces);
//      generateDateArray(ArCh *buf, cArDate& val, int lev, int sp);
//      generateDateArrayRaw(ArCh *buf, cArDate& val, int lev, int sp);
// [27] generateDateTz(ArCh *buffer, bDatez value, int level, int spaces);
//      generateDateTzRaw(ArCh *buffer, bDatez value, int level, int spaces);
//      generateDateTzArray(ArCh *buf, cArDatez& val, int lev, int sp);
//      generateDateTzArrayRaw(ArCh *buf, cArDatez& val, int lev, int sp);
// [15] generateDatetime(ArCh *buffer, bDT value, int level, int spaces);
//      generateDatetimeRaw(ArCh *buffer, bDT value, int level, int spaces);
//      generateDatetimeArray(ArCh *buf, cArDT& val, int lev, int sp);
//      generateDatetimeArrayRaw(ArCh *buf, cArDT& val, int lev, int sp);
// [29] generateDatetimeTz(ArCh *buffer, bDTz value, int level, int spaces);
//      generateDatetimeTzRaw(ArCh *buffer, bDTz value, int level, int spaces);
//      generateDatetimeTzArray(ArCh *buf, cArDT& val, int lev, int sp);
//      generateDatetimeTzArrayRaw(ArCh *bu, cArDT& va, int le, int sp);
// [16] generateDouble(ArCh *buffer, double value, int level, int spaces);
//      generateDoubleRaw(ArCh *buffer, double value, int level, int spaces);
//      generateDoubleArray(ArCh *buf, cArDb& val, int lev, int sp);
//      generateDoubleArrayRaw(ArCh *buf, cArDb& val, int lev, int sp);
// [17] generateFloat(ArCh *buffer, float value, int level, int spaces);
//      generateFloatRaw(ArCh *buffer, float value, int level, int spaces);
//      generateFloatArray(ArCh *buf, cArFl& val, int lev, int sp);
//      generateFloatArrayRaw(ArCh *buf, cArFl& val, int lev, int sp);
// [18] generateInt(ArCh *buffer, int value, int level, int spaces);
//      generateIntRaw(ArCh *buffer, int value, int level, int spaces);
//      generateIntArray(ArCh *buf, cArInt& val, int lev, int sp);
//      generateIntArrayRaw(ArCh *buf, cArInt& val, int lev, int sp);
// [19] generateInt64(ArCh *buffer, int64 value, int level, int spaces);
//      generateInt64Raw(ArCh *buffer, int64 value, int level, int spaces);
//      generateInt64Array(ArCh *buf, cArInt64& val, int lev, int sp);
//      generateInt64ArrayRaw(ArCh *buf, cArInt64& val, int lev, int sp);
// [20] generateIntegerLocator(b_ca *, int val, int level, int spaces);
//      generateIntegerLocatorRaw(b_ca *, int val, int level, int spaces);
// [21] generateShort(ArCh *buffer, short value, int level, int spaces);
//      generateShortRaw(ArCh *buffer, short value, int level, int spaces);
//      generateShortArray(ArCh *buf, cArSh& val, int lev, int sp);
//      generateShortArrayRaw(ArCh *buf, cArSh& val, int lev, int sp);
// [22] generateString(ArCh *buffer, bStr value, int level, int spaces);
//      generateStringRaw(ArCh *buffer, bStr value, int level, int spaces);
//      generateStringArray(ArCh *buf, cArStr& val, int lev, int sp);
//      generateStringArrayRaw(ArCh *buf, cArStr& val, int lev, int sp);
// [23] generateTime(ArCh *buffer, bTime value, int level, int spaces);
//      generateTimeRaw(ArCh *buffer, bTime value, int level, int spaces);
//      generateTimeArray(ArCh *buf, cArTime& val, int lev, int sp);
//      generateTimeArrayRaw(ArCh *buf, cArTime& val, int lev, int sp);
// [28] generateTimeTz(ArCh *buffer, bTimez value, int level, int spaces);
//      generateTimeTzRaw(ArCh *buffer, bTimez value, int level, int spaces);
//      generateTimeTzArray(ArCh *buf, cArTimez& val, int lev, int sp);
//      generateTimeTzArrayRaw(ArCh *buf, cArTimez& val, int lev, int sp);
//-----------------------------------------------------------------------------
// TBD usage

//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;
static int verbose = 0;
static int veryVerbose = 0;
static int veryVeryVerbose = 0;

namespace {

void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//--------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) \
{ \
    if (!(X)) { \
        cout << #I << ": " << I; \
        cout << "\n"; \
        aSsErT(1, #X, __LINE__); \
    } \
}

#define LOOP2_ASSERT(I,J,X) \
{ \
    if (!(X)) { \
        cout << #I << ": " << I; \
        cout << "\t" << #J << ": " << J; \
        cout << "\n"; \
        aSsErT(1, #X, __LINE__); \
    } \
}

#define LOOP3_ASSERT(I,J,K,X) \
{ \
   if (!(X)) { \
        cout << #I << ": " << I; \
        cout << "\t" << #J << ": " << J; \
        cout << "\t" << #K << ": " << K; \
        cout << "\n"; \
        aSsErT(1, #X, __LINE__); \
   } \
}

#define LOOP4_ASSERT(I,J,K,L,X) \
{ \
   if (!(X)) { \
        cout << #I << ": " << I; \
        cout << "\t" << #J << ": " << J; \
        cout << "\t" << #K << ": " << K; \
        cout << "\t" << #L << ": " << L; \
        cout << "\n"; \
        aSsErT(1, #X, __LINE__); \
   } \
}

#define LOOP5_ASSERT(I,J,K,L,M,X) \
{ \
   if (!(X)) { \
        cout << #I << ": " << I; \
        cout << "\t" << #J << ": " << J; \
        cout << "\t" << #K << ": " << K; \
        cout << "\t" << #L << ": " << L; \
        cout << "\t" << #M << ": " << M; \
        cout << "\n"; \
        aSsErT(1, #X, __LINE__); \
   } \
}
#define TASSERT(X) LOOP_ASSERT(from,X)
#define LOOP_TASSERT(I,X) LOOP2_ASSERT(from,I,X)
#define LOOP2_TASSERT(I,J,X) LOOP3_ASSERT(from,I,J,X)
#define LOOP3_TASSERT(I,J,K,X) LOOP4_ASSERT(from,I,J,K,X)
#define LOOP4_TASSERT(I,J,K,L,X) LOOP5_ASSERT(from,I,J,K,L,X)
//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print a tab character

//=============================================================================
//                  USEFUL SPECIFIC MACROS
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls_PlatformUtil::Int64 Int64;

#define GEN_RAW(FTYPE) generate##FTYPE##Raw
#define GEN_ARRAY_RAW(FTYPE) generate##FTYPE##ArrayRaw
#define GEN_ARRAY(FTYPE) generate##FTYPE##Array
#define GEN_(FTYPE) generate##FTYPE
#define PARSE_(FTYPE) parse##FTYPE
#define PARSE_ARRAY(FTYPE) parse##FTYPE##Array

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    int         d_offset;   // expected number of parsed characters
} ErrorParseSample;

// Definitions of Sample Data for Parse Functions

typedef struct {
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
} SampleValuesParseDatetimeTz;

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    int         d_year;     // expected return year
    int         d_month;    // expected return month
    int         d_day;      // expected return day
    int         d_hour;     // expected return hour
    int         d_minute;   // expected return minute
    int         d_second;   // expected return second
    int         d_milli;    // expected return millisecond
} SampleValuesParseDatetime;

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    int         d_year;     // expected return year
    int         d_month;    // expected return month
    int         d_day;      // expected return day
    int         d_tz;       // expected return tz minutes
} SampleValuesParseDateTz;

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    int         d_year;     // expected return year
    int         d_month;    // expected return month
    int         d_day;      // expected return day
} SampleValuesParseDate;

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    int         d_hour;     // expected return hour
    int         d_minute;   // expected return minute
    int         d_second;   // expected return second
    int         d_milli;    // expected return millisecond
    int         d_tz;       // expected return tz minutes
} SampleValuesParseTimeTz;

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    int         d_hour;     // expected return hour
    int         d_minute;   // expected return minute
    int         d_second;   // expected return second
    int         d_milli;    // expected return millisecond
} SampleValuesParseTime;

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    short       d_value;    // expected return value
} SampleValuesParseShort;

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    int         d_value;    // expected return value
} SampleValuesParseInt;

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    Int64       d_value;    // expected return value
} SampleValuesParseInt64;

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    float       d_value;    // expected return value
} SampleValuesParseFloat;

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    double      d_value;    // expected return value
} SampleValuesParseDouble;

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    char        d_value;    // expected return value
} SampleValuesParseChar;

// Definitions of Sample Data for Generate Functions

typedef struct {
    int         d_lineNum;  // source line number
    int         d_year;     // specification year
    int         d_month;    // specification month
    int         d_day;      // specification day
    int         d_hour;     // specification hour
    int         d_minute;   // specification minute
    int         d_second;   // specification second
    int         d_milli;    // specification millisecond
    int         d_tz;       // specification tz minutes
    const char *d_exp_p;    // expected result
} SampleValuesGenerateDatetimeTz;

typedef struct {
    int         d_lineNum;  // source line number
    int         d_year;     // specification year
    int         d_month;    // specification month
    int         d_day;      // specification day
    int         d_hour;     // specification hour
    int         d_minute;   // specification minute
    int         d_second;   // specification second
    int         d_milli;    // specification millisecond
    const char *d_exp_p;    // expected result
} SampleValuesGenerateDatetime;

typedef struct {
    int         d_lineNum;  // source line number
    int         d_year;     // specification year
    int         d_month;    // specification month
    int         d_day;      // specification day
    int         d_tz;       // specification tz minutes
    const char *d_exp_p;    // expected result
} SampleValuesGenerateDateTz;

typedef struct {
    int         d_lineNum;  // source line number
    int         d_year;     // specification year
    int         d_month;    // specification month
    int         d_day;      // specification day
    const char *d_exp_p;    // expected result
} SampleValuesGenerateDate;

typedef struct {
    int         d_lineNum;  // source line number
    int         d_hour;     // specification hour
    int         d_minute;   // specification minute
    int         d_second;   // specification second
    int         d_milli;    // specification millisecond
    int         d_tz;       // specification tz minutes
    const char *d_exp_p;    // expected result
} SampleValuesGenerateTimeTz;

typedef struct {
    int         d_lineNum;  // source line number
    int         d_hour;     // specification hour
    int         d_minute;   // specification minute
    int         d_second;   // specification second
    int         d_milli;    // specification millisecond
    const char *d_exp_p;    // expected result
} SampleValuesGenerateTime;

typedef struct {
    int         d_lineNum;  // source line number
    short       d_value;    // specification value
    const char *d_exp_p;    // expected result
} SampleValuesGenerateShort;

typedef struct {
    int         d_lineNum;  // source line number
    int         d_value;    // specification value
    const char *d_exp_p;    // expected result
} SampleValuesGenerateInt;

typedef struct {
    int         d_lineNum;  // source line number
    Int64       d_value;    // specification value
    const char *d_exp_p;    // expected result
} SampleValuesGenerateInt64;

typedef struct {
    int         d_lineNum;  // source line number
    float       d_value;    // specification value
    const char *d_exp_p;    // expected result
} SampleValuesGenerateFloat;

typedef struct {
    int         d_lineNum;  // source line number
    double      d_value;    // specification value
    const char *d_exp_p;    // expected result
} SampleValuesGenerateDouble;

typedef struct {
    int         d_lineNum;  // source line number
    const char *d_value;    // specification value
    const char *d_exp_p;    // expected result
} SampleValuesGenerateString;

typedef struct {
    int         d_lineNum;  // source line number
    char        d_value;    // specification value
    const char *d_exp_p;    // expected result
} SampleValuesGenerateChar;

//--------------------------------------------------------------
// Static declarations
//--------------------------------------------------------------

static const struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
} SPECS[] = {
    //line  spec
    //----  -----------------
    { L_,   ""                },
    { L_,   "x"               },
    { L_,   "\n"              },
};
static const int NUM_SPECS = sizeof SPECS / sizeof *SPECS;

static const struct {
    int         d_lineNum;  // source line number
    int         d_level;    // specification level
    int         d_spaces;   // spec. spaces per level
} INDENTS[] = {
    //line   LV    SPL
    //----  ---    ---
    { L_,     0,     0 },
    { L_,     0,     1 },
    { L_,     1,     0 },
    { L_,     1,     1 },
    { L_,     2,     4 },
    { L_,    -1,     1 },
    { L_,    -2,     5 },
    { L_,    -3,     0 },
};
static const int NUM_INDENTS = sizeof INDENTS / sizeof *INDENTS;

static const struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    int         d_fail;     // 1 for fail, 0 for success
    int         d_offset;   // expected number of parsed characters
} DATAWS[] = {
    //line  spec            fail  off
    //----  ---------       ----  ---
    { L_,   "",               0,    0 },
    { L_,   " ",              0,    1 },
    { L_,   "\n",             0,    1 },
    { L_,   "x",              1,    0 },
    { L_,   "\t,",            1,    1 },
    { L_,   " \n\f\t ",       0,    5 },
    { L_,   " \n]",           1,    2 },
    { L_,   "// comment\n",   0,   11 },
    { L_,   "/* comment */",  0,   13 },
};
static const int NUM_DATAWS = sizeof DATAWS / sizeof *DATAWS;

static const struct {
    int         d_lineNum;  // source line number
    const char *d_spec_p;   // specification string
    int         d_fail;     // 1 for fail, 0 for success
    int         d_offset;   // expected number of parsed characters
} EMPTY_ARRAY[] = {
    //line  spec       fail  off
    //----  ---------  ----  ---
    { L_,   "",          1,    0 },
    { L_,   " ",         1,    1 },
    { L_,   "x",         1,    0 },
    { L_,   "0",         1,    0 },
    { L_,   "  ",        1,    2 },
    { L_,   " [",        1,    2 },
    { L_,   "[",         1,    1 },
    { L_,   "[ ",        1,    2 },
    { L_,   "[ ,",       1,    2 },
    { L_,   "[]",        0,    2 },
    { L_,   "[ ]",       0,    3 },
    { L_,   "[\n ]",     0,    4 },
};
static const int NUM_EMPTY_ARRAY = sizeof EMPTY_ARRAY / sizeof *EMPTY_ARRAY;

static bslma_TestAllocator *testAllocator_p;

#define INVOKE_testParseErrorInput(TNAME, FNAME) \
    testParseErrorInput<TNAME>( \
                   bdepu_TypesParser::parse##FNAME, \
                   (int (*)(const char **, TNAME *, const char *, int))0, \
                   DATA_ERRORS, \
                   NUM_DATA_ERRORS, \
                   INITIAL_VALUE_1, \
                   INITIAL_VALUE_2, \
                   __LINE__)

#define INVOKE_testParseErrorInputB(TNAME, FNAME) \
    testParseErrorInput<TNAME>( \
                   (int (*)(const char **, TNAME *, const char *))0, \
                   bdepu_TypesParser::parse##FNAME, \
                   DATA_ERRORS, \
                   NUM_DATA_ERRORS, \
                   INITIAL_VALUE_1, \
                   INITIAL_VALUE_2, \
                   __LINE__)

template <typename PARSE_TYPE>
void testParseErrorInput(
     int (*parseFunction1)(const char **, PARSE_TYPE *, const char *),
     int (*parseFunction2)(const char **, PARSE_TYPE *, const char *, int),
     const ErrorParseSample *DATA_ERRORS,
     int NUM_DATA_ERRORS,
     const PARSE_TYPE& INITIAL_VALUE_1,
     const PARSE_TYPE& INITIAL_VALUE_2,
     int from
     )
{
    PARSE_TYPE result;
    for (int ti = 0; ti < NUM_DATA_ERRORS ; ++ti) {
        const int LINEDE         = DATA_ERRORS[ti].d_lineNum;
        const char *const SPECDE = DATA_ERRORS[ti].d_spec_p;
        const int NUMDE          = DATA_ERRORS[ti].d_offset;

        for (int wi = 0; wi < NUM_DATAWS; ++wi) {
            const int LINEWS         = DATAWS[wi].d_lineNum;
            const char *const SPECWS = DATAWS[wi].d_spec_p;
            const int FAILWS         = DATAWS[wi].d_fail;
            const int NUMWS          = DATAWS[wi].d_offset;

            const char *endPos = 0;

            const int NUM = FAILWS ? NUMWS : NUMWS + NUMDE;

            bsl::string spec(SPECWS, testAllocator_p);
            spec.append(SPECDE);

            result = INITIAL_VALUE_1;

            int rv = 0 != parseFunction1
                     ?
                     (*parseFunction1)(&endPos, &result, spec.c_str())
                     :
                     (*parseFunction2)(&endPos, &result, spec.c_str(), 10)
                     ;

            if (veryVerbose) {
                cout <<   "LINEDE = " << LINEDE
                     << ", NUMDE  = " << NUMDE
                     << ", SPECDE = " << SPECDE
                     << ", LINEWS = " << LINEWS
                     << ", SPECWS = " << SPECWS
                     << ", FAILWS = " << FAILWS
                     << ", NUMWS  = " << NUMWS
                     << ", spec   = \"" << spec << "\""
                     << ", result = " << result
                     << ", rv     = " << rv
                     << ", NUM    = " << NUM
                     << ", endPos - spec.c_str() = " << (endPos - spec.c_str())
                     << ", INITIAL_VALUE_1 = " << INITIAL_VALUE_1
                     << endl;
            }

            LOOP3_ASSERT(from, LINEWS, LINEDE, 1 == rv);
            LOOP3_ASSERT(from, LINEWS, LINEDE, INITIAL_VALUE_1 == result);
            LOOP3_ASSERT(from, LINEWS, LINEDE, NUM == endPos - spec.c_str());

            result = INITIAL_VALUE_2;

            rv = 0 != parseFunction1
                 ?
                 (*parseFunction1)(&endPos, &result, spec.c_str())
                 :
                 (*parseFunction2)(&endPos, &result, spec.c_str(), 10)
                 ;

            if (veryVerbose) {
                cout <<   "LINEDE = " << LINEDE
                     << ", NUMDE  = " << NUMDE
                     << ", SPECDE = " << SPECDE
                     << ", LINEWS = " << LINEWS
                     << ", SPECWS = " << SPECWS
                     << ", FAILWS = " << FAILWS
                     << ", NUMWS  = " << NUMWS
                     << ", spec   = \"" << spec << "\""
                     << ", result = " << result
                     << ", NUM    = " << NUM
                     << ", endPos - spec.c_str() = " << (endPos - spec.c_str())
                     << ", INITIAL_VALUE_2 = " << INITIAL_VALUE_2
                     << ", rv     = " << rv
                     << endl;
            }

            LOOP3_ASSERT(from, LINEWS, LINEDE, 1 == rv);
            LOOP3_ASSERT(from, LINEWS, LINEDE, INITIAL_VALUE_2 == result);
            LOOP3_ASSERT(from, LINEWS, LINEDE, NUM == endPos - spec.c_str());
        }
    }
}

#define INVOKE_testParseValidInput(TNAME, FNAME) \
    testParseValidInput<TNAME, SampleValuesParse##FNAME>( \
                   bdepu_TypesParser::parse##FNAME, \
                   (int (*)(const char **, TNAME *, const char *, int))0, \
                   DATA, \
                   VALUES, \
                   INITIAL_VALUE_1, \
                   INITIAL_VALUE_2, \
                   __LINE__)

#define INVOKE_testParseValidInputB(TNAME, FNAME) \
    testParseValidInput<TNAME, SampleValuesParse##FNAME>( \
                   (int (*)(const char **, TNAME *, const char *))0, \
                   bdepu_TypesParser::parse##FNAME, \
                   DATA, \
                   VALUES, \
                   INITIAL_VALUE_1, \
                   INITIAL_VALUE_2, \
                   __LINE__)

template <typename PARSE_TYPE, typename SPEC_TYPE>
void testParseValidInput(
     int (*parseFunction1)(const char **, PARSE_TYPE *, const char *),
     int (*parseFunction2)(const char **, PARSE_TYPE *, const char *, int),
     const SPEC_TYPE *DATA,
     const bsl::vector<PARSE_TYPE>& VALUES,
     const PARSE_TYPE& INITIAL_VALUE_1,
     const PARSE_TYPE& INITIAL_VALUE_2,
     int from
     )
{
    const int NUM_DATA = VALUES.size();
    PARSE_TYPE result;

    for (int ti = 0; ti < NUM_DATA ; ++ti) {
        const int LINE1         = DATA[ti].d_lineNum;
        const bsl::string SPEC1(DATA[ti].d_spec_p, testAllocator_p);

        const PARSE_TYPE& VALUE = VALUES[ti];

        for (int wi = 0; wi < NUM_DATAWS; ++wi) {
            const int LINEWS         = DATAWS[wi].d_lineNum;
            const char *const SPECWS = DATAWS[wi].d_spec_p;
            const int FAILWS         = DATAWS[wi].d_fail;
            const int NUMWS          = DATAWS[wi].d_offset;

            const char *endPos = 0;

            const int NUM = FAILWS ? NUMWS : NUMWS + SPEC1.length();

            bsl::string spec(SPECWS, testAllocator_p);
            spec.append(SPEC1);

            result = INITIAL_VALUE_1;

            int rv = 0 != parseFunction1
                     ?
                     (*parseFunction1)(&endPos, &result, spec.c_str())
                     :
                     (*parseFunction2)(&endPos, &result, spec.c_str(), 10)
                     ;

            if (veryVerbose) {
                cout <<   "LINE1  = " << LINE1
                     << ", LINEWS = " << LINEWS
                     << ", SPECWS = " << SPECWS
                     << ", FAILWS = " << FAILWS
                     << ", NUMWS  = " << NUMWS
                     << ", spec   = \"" << spec << "\""
                     << ", result = " << result
                     << ", INITIAL_VALUE_1 = " << INITIAL_VALUE_1
                     << endl;
            }

            LOOP3_ASSERT(from, LINEWS, LINE1, FAILWS == rv);
            LOOP3_ASSERT(from,
                         LINEWS,
                         LINE1,
                         (FAILWS ? INITIAL_VALUE_1 : VALUE) == result);
            LOOP3_ASSERT(from, LINEWS, LINE1, NUM == endPos - spec.c_str());
        }
    }
}

#define INVOKE_testParseArray(TNAME, FNAME) \
    testParseArray<TNAME, SampleValuesParse##FNAME>( \
                   bdepu_TypesParser::parse##FNAME##Array, \
                   DATA, \
                   VALUES, \
                   DATA_ERRORS, \
                   NUM_DATA_ERRORS, \
                   __LINE__)


template <typename PARSE_TYPE, typename SPEC_TYPE>
void testParseArray(
     int (*parseFunction)(const char **,
                          bsl::vector<PARSE_TYPE>*,
                          const char *),
     const SPEC_TYPE *DATA,
     const bsl::vector<PARSE_TYPE>& VALUES,
     const ErrorParseSample *DATA_ERRORS,
     int NUM_DATA_ERRORS,
     int from
     )
{
    for (int wi = 0; wi < NUM_DATAWS; ++wi) {
        const int LINEWS         = DATAWS[wi].d_lineNum;
        const char *const SPECWS = DATAWS[wi].d_spec_p;
        const int FAILWS         = DATAWS[wi].d_fail;
        const int NUMWS          = DATAWS[wi].d_offset;

        bsl::vector<PARSE_TYPE> result(testAllocator_p);

        const char *endPos = 0;

        for (int ti = 0; ti < NUM_EMPTY_ARRAY ; ++ti) {
            const int LINEEA         = EMPTY_ARRAY[ti].d_lineNum;
            const char *const SPECEA = EMPTY_ARRAY[ti].d_spec_p;
            const int FAILEA         = EMPTY_ARRAY[ti].d_fail;
            const int NUMEA          = EMPTY_ARRAY[ti].d_offset;

            const int FAIL = FAILWS || FAILEA;
            const int NUM = FAILWS ? NUMWS : NUMWS + NUMEA;

            bsl::string spec(SPECWS, testAllocator_p);
            spec.append(SPECEA);

            int rv = (*parseFunction)(&endPos, &result, spec.c_str());

            LOOP2_TASSERT(LINEWS, LINEEA, FAIL == rv);
            LOOP2_TASSERT(LINEWS, LINEEA, 0 == result.size());
            LOOP2_TASSERT(LINEWS, LINEEA, NUM == endPos - spec.c_str());
        }

        for (int tj = 0; tj < NUM_DATA_ERRORS ; ++tj) {
            const int LINEDE         = DATA_ERRORS[tj].d_lineNum;
            const char *const SPECDE = DATA_ERRORS[tj].d_spec_p;
            const int NUMDE          = DATA_ERRORS[tj].d_offset;

            bsl::string spec(SPECWS, testAllocator_p);
            spec.append("[ ");

            const int NUM = FAILWS ? NUMWS : spec.length() + NUMDE;

            spec.append(SPECDE);
            spec.append(" ]");

            int rv = (*parseFunction)(&endPos, &result, spec.c_str());

            LOOP2_TASSERT(LINEWS, LINEDE, 1 == rv);
            LOOP2_TASSERT(LINEWS, LINEDE, 0 == result.size());
            LOOP2_TASSERT(LINEWS, LINEDE, NUM == endPos - spec.c_str());
        }

        bsl::vector<bsl::string> SPECS(testAllocator_p);

        const int NUM_DATA = VALUES.size();

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int LINE1         = DATA[ti].d_lineNum;
            const bsl::string SPEC1(DATA[ti].d_spec_p, testAllocator_p);


            SPECS.push_back(SPEC1);

            bsl::string baseStr(SPECWS, testAllocator_p);
            baseStr.append("[");
            for (int tj = 0; tj <= ti ; ++tj) {
                bsl::string tmpStr(baseStr, testAllocator_p);
                tmpStr.append(" ");
                tmpStr.append(SPECS[ti - tj]);
                int rv = (*parseFunction)(&endPos, &result, tmpStr.c_str());
                LOOP3_TASSERT(LINEWS, LINE1, tj, 1 == rv);
                if (FAILWS) {
                    LOOP3_TASSERT(LINEWS, LINE1, tj, 0 == result.size());
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 tj,
                                 NUMWS == endPos - tmpStr.c_str());
                }
                else {
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 tj,
                                 result.size() == tj + 1);
                    LOOP3_TASSERT(
                        LINEWS,
                        LINE1,
                        tj,
                        tmpStr.length() == endPos - tmpStr.c_str());
                }

                tmpStr.append(" ]");
                rv = (*parseFunction)(&endPos, &result, tmpStr.c_str());

                LOOP3_TASSERT(LINEWS, LINE1, tj, FAILWS == rv);
                if (FAILWS) {
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 tj,
                                 0 == result.size());
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 tj,
                                 NUMWS == endPos - tmpStr.c_str());
                }
                else {
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 tj,
                                 result.size() == tj + 1);
                    LOOP3_TASSERT(
                        LINEWS,
                        LINE1,
                        tj,
                        tmpStr.length() == endPos - tmpStr.c_str());
                }

                baseStr.append(SPECS[ti - tj]);
                rv = (*parseFunction)(&endPos, &result, baseStr.c_str());
                LOOP3_TASSERT(LINEWS, LINE1, tj, 1 == rv);
                if (FAILWS) {
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 tj,
                                 0 == result.size());
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 tj,
                                 NUMWS == endPos - baseStr.c_str());
                }
                else {
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 tj,
                                 result.size() == tj + 1);
                    LOOP3_TASSERT(
                        LINEWS,
                        LINE1,
                        tj,
                        baseStr.length() == endPos - baseStr.c_str());
                }

                baseStr.append(" ");
                rv = (*parseFunction)(&endPos, &result, baseStr.c_str());
                LOOP3_TASSERT(LINEWS, LINE1, tj, 1 == rv);
                if (FAILWS) {
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 tj, 0 == result.size());
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 tj,
                                 NUMWS == endPos - baseStr.c_str());
                }
                else {
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 tj,
                                 result.size() == tj + 1);
                    LOOP3_TASSERT(
                        LINEWS,
                        LINE1,
                        tj,
                        baseStr.length() == endPos - baseStr.c_str());
                }
            }

            for (int tj = 0; tj < NUM_DATA_ERRORS ; ++tj) {
                const int LINEDE         = DATA_ERRORS[tj].d_lineNum;
                const char *const SPECDE = DATA_ERRORS[tj].d_spec_p;
                const int NUMDE          = DATA_ERRORS[tj].d_offset;

                bsl::string tmpStr(baseStr, testAllocator_p);
                tmpStr.append(SPECDE);
                int rv = (*parseFunction)(&endPos, &result, tmpStr.c_str());

                LOOP3_TASSERT(LINEWS, LINE1, LINEDE, 1 == rv);

                if (FAILWS) {
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 LINEDE,
                                 0 == result.size());
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 LINEDE,
                                 NUMWS == endPos - tmpStr.c_str());
                }
                else {
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 LINEDE,
                                 result.size() == ti + 1);
                    LOOP3_TASSERT(
                        LINEWS,
                        LINE1,
                        LINEDE,
                        baseStr.length() + NUMDE ==
                                                 endPos - tmpStr.c_str());
                }
            }

            baseStr.append("]");
            int rv = (*parseFunction)(&endPos, &result, baseStr.c_str());

            if (FAILWS) {
                LOOP2_TASSERT(LINEWS, LINE1, 1 == rv);
                LOOP2_TASSERT(LINEWS, LINE1, 0 == result.size());
                LOOP2_TASSERT(LINEWS,
                             LINE1,
                             NUMWS == endPos - baseStr.c_str());
            }
            else {
                LOOP2_TASSERT(LINEWS, LINE1, 0 == rv);
                LOOP2_TASSERT(LINEWS,
                             LINE1,
                             result.size() == ti + 1);
                LOOP2_TASSERT(
                    LINEWS,
                    LINE1,
                    baseStr.length() == endPos - baseStr.c_str());

                for (int tj = 0; tj <= ti ; ++tj) {
                    LOOP3_TASSERT(LINEWS,
                                 LINE1,
                                 tj,
                                 VALUES[ti - tj] == result[tj]);
                }
            }
        }
    }
}

#define INVOKE_testGenerateR(TNAME, FNAME) \
    testGenerate<TNAME, const TNAME&, SampleValuesGenerate##FNAME>( \
                   bdepu_TypesParser::generate##FNAME, \
                   bdepu_TypesParser::generate##FNAME##Raw, \
                   bdepu_TypesParser::parse##FNAME, \
                   (int (*)(const char **, TNAME *, const char *, int))0, \
                   DATA, \
                   VALUES, \
                   __LINE__)

#define INVOKE_testGenerateC(TNAME, FNAME) \
    testGenerate<TNAME, TNAME, SampleValuesGenerate##FNAME>( \
                   bdepu_TypesParser::generate##FNAME, \
                   bdepu_TypesParser::generate##FNAME##Raw, \
                   bdepu_TypesParser::parse##FNAME, \
                   (int (*)(const char **, TNAME *, const char *, int))0, \
                   DATA, \
                   VALUES, \
                   __LINE__)

#define INVOKE_testGenerateB(TNAME, FNAME) \
    testGenerate<TNAME, TNAME, SampleValuesGenerate##FNAME>( \
                   bdepu_TypesParser::generate##FNAME, \
                   bdepu_TypesParser::generate##FNAME##Raw, \
                   (int (*)(const char **, TNAME *, const char *))0, \
                   bdepu_TypesParser::parse##FNAME, \
                   DATA, \
                   VALUES, \
                   __LINE__)

template <typename GEN_TYPE1, typename GEN_TYPE2, typename SPEC_TYPE>
void testGenerate(
     void (*generateFunction)(bsl::vector<char> *, GEN_TYPE2, int, int),
     void (*generateFunctionRaw)(bsl::vector<char> *, GEN_TYPE2, int, int),
     int (*parseFunction1)(const char **, GEN_TYPE1 *, const char *),
     int (*parseFunction2)(const char **, GEN_TYPE1 *, const char *, int),
     const SPEC_TYPE *DATA,
     const bsl::vector<GEN_TYPE1>& VALUES,
     int from
     )
{
    const int NUM_DATA = VALUES.size();

    for (int i = 0; i < NUM_SPECS; ++i) {
        const int   LINE1 = SPECS[i].d_lineNum;
        const bsl::string SPEC(SPECS[i].d_spec_p, testAllocator_p);

        for (int j = 0; j < NUM_INDENTS; ++j) {
            const int LINE2  = INDENTS[j].d_lineNum;
            const int LEVEL  = INDENTS[j].d_level;
            const int SPACES = INDENTS[j].d_spaces;

            LOOP2_TASSERT(LINE1, LINE2, SPACES >= 0);

            bsl::string singleElemPref(SPEC, testAllocator_p);
            singleElemPref.append((LEVEL >= 0 ? LEVEL : 0) * SPACES, ' ');

            for (int k = 0; k < NUM_DATA; ++k) {
                const int         LINE3  = DATA[k].d_lineNum;
                const bsl::string EXP(DATA[k].d_exp_p, testAllocator_p);

                const GEN_TYPE1& VALUE = VALUES[k];

                bsl::vector<char> expected(testAllocator_p);
                expected.insert(expected.end(),
                                singleElemPref.begin(),
                                singleElemPref.end());
                expected.insert(expected.end(), EXP.begin(), EXP.end());

                bsl::vector<char> buffer(testAllocator_p);
                buffer.insert(buffer.end(), SPEC.begin(), SPEC.end());

                (*generateFunctionRaw)(&buffer, VALUE, LEVEL, SPACES);

                LOOP3_TASSERT(LINE1, LINE2, LINE3, buffer == expected);

                {   // Parse back generated string
                    GEN_TYPE1 verify;
                    buffer.push_back('\0');
                    const char *endPos = &buffer.front() + SPEC.length();
                    int rv =
                        0 != parseFunction1
                        ?
                        (*parseFunction1)(&endPos, &verify, endPos)
                        :
                        (*parseFunction2)(&endPos, &verify, endPos, 10)
                        ;

                    LOOP3_TASSERT(LINE1, LINE2, LINE3, 0 == rv);
                    LOOP3_TASSERT(LINE1, LINE2, LINE3, verify == VALUE);
                }
                expected.push_back('\0');

                buffer.clear();
                buffer.insert(buffer.end(), SPEC.begin(), SPEC.end());
                (*generateFunction)(&buffer, VALUE, LEVEL, SPACES);

                LOOP3_TASSERT(LINE1, LINE2, LINE3, buffer == expected);

                buffer.clear();
                buffer.insert(buffer.end(), SPEC.begin(), SPEC.end());
                buffer.push_back('\0');
                (*generateFunction)(&buffer, VALUE, LEVEL, SPACES);

                LOOP3_TASSERT(LINE1, LINE2, LINE3, buffer == expected);
            }
        }
    }
}

#define INVOKE_testGenerateArray(TNAME, FNAME) \
    testGenerateArray<TNAME, SampleValuesGenerate##FNAME>( \
                   bdepu_TypesParser::generate##FNAME##Array, \
                   bdepu_TypesParser::generate##FNAME##ArrayRaw, \
                   bdepu_TypesParser::parse##FNAME##Array, \
                   DATA, \
                   VALUES, \
                   __LINE__)

template <typename GEN_TYPE, typename SPEC_TYPE>
void testGenerateArray(
     void (*generateFunction)(bsl::vector<char> *,
                              const bsl::vector<GEN_TYPE>&,
                              int,
                              int),
     void (*generateFunctionRaw)(bsl::vector<char> *,
                                 const bsl::vector<GEN_TYPE>&,
                                 int,
                                 int),
     int (*parseFunction)(const char **,
                          bsl::vector<GEN_TYPE> *,
                          const char *),
     const SPEC_TYPE              *DATA,
     const bsl::vector<GEN_TYPE>&  SAVED_VALUES,
     int                           from)
{
    const int NUM_DATA = SAVED_VALUES.size();

    for (int i = 0; i < NUM_SPECS; ++i) {
        const int   LINE1 = SPECS[i].d_lineNum;
        const bsl::string SPEC(SPECS[i].d_spec_p, testAllocator_p);

        for (int j = 0; j < NUM_INDENTS; ++j) {
            const int LINE2  = INDENTS[j].d_lineNum;
            const int LEVEL  = INDENTS[j].d_level;
            const int SPACES = INDENTS[j].d_spaces;

            LOOP2_TASSERT(LINE1, LINE2, SPACES >= 0);

            bsl::vector<GEN_TYPE> VALUES(testAllocator_p);
            VALUES.reserve(NUM_DATA);

            bsl::string oBracketStr("", testAllocator_p);
            bsl::string cBracketStr("\n", testAllocator_p);
            bsl::string arrayElemPref("\n", testAllocator_p);

            {
                int level = LEVEL;

                if (level < 0) {
                    level = -level;
                }
                else {
                    oBracketStr.append(level * SPACES, ' ');
                }
                oBracketStr.append(1, '[');

                cBracketStr.append(level * SPACES, ' ');
                cBracketStr.append(1, ']');

                arrayElemPref.append((level + 1) * SPACES, ' ');
            }

            bsl::vector<char> expected(testAllocator_p);
            expected.insert(expected.end(), SPEC.begin(), SPEC.end());
            expected.insert(expected.end(),
                            oBracketStr.begin(),
                            oBracketStr.end());

            expected.insert(expected.end(),
                            cBracketStr.begin(),
                            cBracketStr.end());

            bsl::vector<char> buffer(testAllocator_p);
            buffer.insert(buffer.end(), SPEC.begin(), SPEC.end());

            (*generateFunctionRaw)(&buffer, VALUES, LEVEL, SPACES);

            LOOP2_TASSERT(LINE1, LINE2, buffer == expected);

            {   // Parse back generated string
                bsl::vector<GEN_TYPE> verify(testAllocator_p);
                buffer.push_back('\0');
                const char *endPos = &buffer.front() + SPEC.length();

                int rv = (*parseFunction)(&endPos, &verify, endPos);

                LOOP2_TASSERT(LINE1, LINE2, 0 == rv);
                LOOP2_TASSERT(LINE1, LINE2, VALUES == verify);
            }
            expected.push_back('\0');

            buffer.clear();
            buffer.insert(buffer.end(), SPEC.begin(), SPEC.end());

            (*generateFunction)(&buffer, VALUES, LEVEL, SPACES);

            LOOP2_TASSERT(LINE1, LINE2, buffer == expected);

            buffer.clear();
            buffer.insert(buffer.end(), SPEC.begin(), SPEC.end());
            buffer.push_back('\0');

            (*generateFunction)(&buffer, VALUES, LEVEL, SPACES);

            LOOP2_TASSERT(LINE1, LINE2, buffer == expected);

            for (int k = 0; k < NUM_DATA; ++k) {
                const int         LINE3  = DATA[k].d_lineNum;
                const bsl::string EXP(DATA[k].d_exp_p, testAllocator_p);

                VALUES.push_back(SAVED_VALUES[k]);

                expected.erase(expected.end() - (cBracketStr.length() + 1),
                               expected.end());
                expected.insert(expected.end(),
                                arrayElemPref.begin(),
                                arrayElemPref.end());
                expected.insert(expected.end(),
                                EXP.begin(),
                                EXP.end());
                expected.insert(expected.end(),
                                cBracketStr.begin(),
                                cBracketStr.end());

                bsl::vector<char> buffer(testAllocator_p);
                buffer.insert(buffer.end(), SPEC.begin(), SPEC.end());

                (*generateFunctionRaw)(&buffer, VALUES, LEVEL, SPACES);

                LOOP3_TASSERT(LINE1, LINE2, LINE3, buffer == expected);

                {   // Parse back generated string
                    bsl::vector<GEN_TYPE> verify(testAllocator_p);
                    buffer.push_back('\0');
                    const char *endPos = &buffer.front() + SPEC.length();

                    int rv = (*parseFunction)(&endPos, &verify, endPos);

                    LOOP3_TASSERT(LINE1, LINE2, LINE3, 0 == rv);
                    LOOP3_TASSERT(LINE1, LINE2, LINE3, VALUES == verify);
                }
                expected.push_back('\0');

                buffer.clear();
                buffer.insert(buffer.end(), SPEC.begin(), SPEC.end());

                (*generateFunction)(&buffer, VALUES, LEVEL, SPACES);

                LOOP3_TASSERT(LINE1, LINE2, LINE3, buffer == expected);

                buffer.clear();
                buffer.insert(buffer.end(), SPEC.begin(), SPEC.end());
                buffer.push_back('\0');

                (*generateFunction)(&buffer, VALUES, LEVEL, SPACES);

                LOOP3_TASSERT(LINE1, LINE2, LINE3, buffer == expected);
            }
        }
    }
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    verbose = argc > 2;
    veryVerbose = argc > 3;
    veryVeryVerbose = argc > 4;

    bslma_TestAllocator testAllocator(veryVeryVerbose);
    testAllocator_p = &testAllocator;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 29: {
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
        // generateDatetimeTz(ArCh *buffer, bDTz value, int level, int spaces);
        // generateDatetimeTzRaw(ArCh *buffer,
        //                       bDTz value,
        //                       int level,
        //                       int spaces);
        //   generateDatetimeTzArray(ArCh *buf, cArDT& val, int lev, int sp);
        //   generateDatetimeTzArrayRaw(ArCh *bu, cArDT& va, int le, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDatetimeTz*" << endl
                          << "===========================" << endl;

        static const
        SampleValuesGenerateDatetimeTz DATA[] = {
            //LN  YY    MM  DD  hour min  sec   ms  exp
            //--- ----  --  --  ---- ---  ---  ---  ----------------------
            { L_, 1978,  8, 27,   1,   2,   3,   4,    60,
                                     "1978/08/27 01:02:03.004+0100" },
            { L_, 2000, 11,  4,  12,  21,  32,  40,  -210,
                                     "2000/11/04 12:21:32.040-0330" },
            { L_, 2003,  4, 18,   7,  49,  56, 417,     0,
                                     "2003/04/18 07:49:56.417+0000" },
            { L_, 2008,  9, 30,  23,   0,   0,   0,  -660,
                                     "2008/09/30 23:00:00.000-1100" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_DatetimeTz> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const int YEAR   = DATA[k].d_year;
            const int MONTH  = DATA[k].d_month;
            const int DAY    = DATA[k].d_day;
            const int HOUR   = DATA[k].d_hour;
            const int MINUTE = DATA[k].d_minute;
            const int SECOND = DATA[k].d_second;
            const int MILLI  = DATA[k].d_milli;
            const int OFFSET = DATA[k].d_tz;

            const bdet_Datetime VALUEDT(YEAR,
                                        MONTH,
                                        DAY,
                                        HOUR,
                                        MINUTE,
                                        SECOND,
                                        MILLI);
            const bdet_DatetimeTz VALUE(VALUEDT, OFFSET);

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateR(bdet_DatetimeTz, DatetimeTz);
        INVOKE_testGenerateArray(bdet_DatetimeTz, DatetimeTz);
      } break;
      case 28: {
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
        //   generateTimeTz(ArCh *buffer, bTimez value, int level, int spaces);
        //   generateTimeTzRaw(ArCh *buffer,
        //                     bTimez value,
        //                     int level,
        //                     int spaces);
        //   generateTimeTzArray(ArCh *buf, cArTimez& val, int lev, int sp);
        //   generateTimeTzArrayRaw(ArCh *buf, cArTimez& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateTimeTz*" << endl
                          << "=======================" << endl;

        static const
        SampleValuesGenerateTimeTz DATA[] = {
            //line  hour  min  sec  milli  exp
            //----  ----  ---  ---  -----  ----------------------
            { L_,    1,   2,   3,     4,   420, "01:02:03.004+0700" },
            { L_,   12,  21,  32,    40,   310, "12:21:32.040+0510" },
            { L_,    7,  49,  56,   417,   -75, "07:49:56.417-0115" },
            { L_,   23,   0,   0,     0,  -120, "23:00:00.000-0200" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_TimeTz> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const int HOUR   = DATA[k].d_hour;
            const int MINUTE = DATA[k].d_minute;
            const int SECOND = DATA[k].d_second;
            const int MILLI  = DATA[k].d_milli;
            const int OFFSET = DATA[k].d_tz;

            const bdet_Time VALUET(HOUR, MINUTE, SECOND, MILLI);
            const bdet_TimeTz VALUE(VALUET, OFFSET);

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateR(bdet_TimeTz, TimeTz);
        INVOKE_testGenerateArray(bdet_TimeTz, TimeTz);
      } break;
      case 27: {
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
        //   generateDateTz(ArCh *buffer, bDatez value, int level, int spaces);
        //   generateDateTzRaw(ArCh *buffer,
        //                     bDatez value,
        //                     int level,
        //                     int spaces);
        //   generateDateTzArray(ArCh *buf, cArDatez& val, int lev, int sp);
        //   generateDateTzArrayRaw(ArCh *buf, cArDatez& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDateTz*" << endl
                          << "=======================" << endl;

        static const
        SampleValuesGenerateDateTz DATA[] = {
            //LN  YY    MM  DD  exp
            //--- ----  --  --  ----------------------
            { L_, 1978,  8, 27,   -90, "1978/08/27-0130" },
            { L_, 2000, 11,  4,   240, "2000/11/04+0400" },
            { L_, 2003,  4, 18,    45, "2003/04/18+0045" },
            { L_, 2008,  9, 30,  -540, "2008/09/30-0900" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_DateTz> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const int         YEAR   = DATA[k].d_year;
            const int         MONTH  = DATA[k].d_month;
            const int         DAY    = DATA[k].d_day;
            const int         OFFSET = DATA[k].d_tz;

            const bdet_Date VALUED(YEAR, MONTH, DAY);
            const bdet_DateTz VALUE(VALUED, OFFSET);

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateR(bdet_DateTz, DateTz);
        INVOKE_testGenerateArray(bdet_DateTz, DateTz);
      } break;
      case 26: {
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

        static const
        ErrorParseSample DATA_ERRORS[] = {
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
            ///{ L_,   "2011/05/03",                     10 },
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
        const int NUM_DATA_ERRORS = sizeof DATA_ERRORS / sizeof *DATA_ERRORS;

        static const
        SampleValuesParseDatetimeTz DATA[] = {
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
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_DatetimeTz> VALUES(&testAllocator);
        VALUES.reserve(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int YEAR          = DATA[ti].d_year;
            const int MONTH         = DATA[ti].d_month;
            const int DAY           = DATA[ti].d_day;
            const int HOUR          = DATA[ti].d_hour;
            const int MINS          = DATA[ti].d_minute;
            const int SECS          = DATA[ti].d_second;
            const int MILLI         = DATA[ti].d_milli;
            const int OFFSET        = DATA[ti].d_tz;

            const bdet_Datetime VALUEDT(YEAR,
                                        MONTH,
                                        DAY,
                                        HOUR,
                                        MINS,
                                        SECS,
                                        MILLI);
            const bdet_DatetimeTz VALUE(VALUEDT, OFFSET);

            VALUES.push_back(VALUE);
        }
        ASSERT(NUM_DATA == VALUES.size());

        INVOKE_testParseErrorInput(bdet_DatetimeTz, DatetimeTz);

        INVOKE_testParseValidInput(bdet_DatetimeTz, DatetimeTz);

        INVOKE_testParseArray(bdet_DatetimeTz, DatetimeTz);
      } break;
      case 25: {
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

        static const
        ErrorParseSample DATA_ERRORS[] = {
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
        const int NUM_DATA_ERRORS = sizeof DATA_ERRORS / sizeof *DATA_ERRORS;

        static const
        SampleValuesParseTimeTz DATA[] = {
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
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_TimeTz> VALUES(&testAllocator);
        VALUES.reserve(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int HOUR          = DATA[ti].d_hour;
            const int MINS          = DATA[ti].d_minute;
            const int SECS          = DATA[ti].d_second;
            const int MILLI         = DATA[ti].d_milli;
            const int OFFSET        = DATA[ti].d_tz;

            const bdet_Time VALUET(HOUR, MINS, SECS, MILLI);
            const bdet_TimeTz VALUE(VALUET, OFFSET);

            VALUES.push_back(VALUE);
        }
        ASSERT(NUM_DATA == VALUES.size());

        INVOKE_testParseErrorInput(bdet_TimeTz, TimeTz);

        INVOKE_testParseValidInput(bdet_TimeTz, TimeTz);

        INVOKE_testParseArray(bdet_TimeTz, TimeTz);
      } break;
      case 24: {
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

        const bdet_Date INITIAL_DATE_1(   1, 1, 1);  // 1st init. date
        const bdet_Date INITIAL_DATE_2(1923, 7, 9);  // 2nd init. date

        const int INITIAL_OFFSET_1 = 180;            // 1st init. offset
        const int INITIAL_OFFSET_2 = -90;            // 2nd init. offset

        const bdet_DateTz INITIAL_VALUE_1(INITIAL_DATE_1, INITIAL_OFFSET_1);
        const bdet_DateTz INITIAL_VALUE_2(INITIAL_DATE_2, INITIAL_OFFSET_2);

        static const
        ErrorParseSample DATA_ERRORS[] = {
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
        const int NUM_DATA_ERRORS = sizeof DATA_ERRORS / sizeof *DATA_ERRORS;

        static const
        SampleValuesParseDateTz DATA[] = {
            //line  spec                       year  month  day     tz
            //----  -------------------        ----  -----  ---   ----
            { L_,   "19/1/2+0000",               19,     1,   2,     0 },
            { L_,   "19/01/02-1430",             19,     1,   2,  -870 },
            { L_,   "199/01/02-0100",           199,     1,   2,   -60 },
            { L_,   "1993/1/02+1015",          1993,     1,   2,   615 },
            { L_,   "1993/01/2+0400",          1993,     1,   2,   240 },
            { L_,   "1993/01/02-0000",         1993,     1,   2,     0 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_DateTz> VALUES(&testAllocator);
        VALUES.reserve(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int YEAR          = DATA[ti].d_year;
            const int MONTH         = DATA[ti].d_month;
            const int DAY           = DATA[ti].d_day;
            const int OFFSET        = DATA[ti].d_tz;

            const bdet_Date VALUED(YEAR, MONTH, DAY);
            const bdet_DateTz VALUE(VALUED, OFFSET);

            VALUES.push_back(VALUE);
        }
        ASSERT(NUM_DATA == VALUES.size());

        INVOKE_testParseErrorInput(bdet_DateTz, DateTz);

        INVOKE_testParseValidInput(bdet_DateTz, DateTz);

        INVOKE_testParseArray(bdet_DateTz, DateTz);
      } break;
      case 23: {
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
        //   generateTime(ArCh *buffer, bTime value, int level, int spaces);
        //   generateTimeRaw(ArCh *buffer, bTime value, int level, int spaces);
        //   generateTime(ArCh *buffer, bTime value, int level, int spaces);
        //   generateTimeRaw(ArCh *buffer, bTime value, int level, int spaces);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateTime*" << endl
                          << "=====================" << endl;

        static const
        SampleValuesGenerateTime DATA[] = {
            //line  hour  min  sec  milli  exp
            //----  ----  ---  ---  -----  ----------------------
            { L_,    1,   2,   3,     4, "01:02:03.004"         },
            { L_,   12,  21,  32,    40, "12:21:32.040"         },
            { L_,    7,  49,  56,   417, "07:49:56.417"         },
            { L_,   24,   0,   0,     0, "24:00:00.000"         },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_Time> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const int         HOUR   = DATA[k].d_hour;
            const int         MINUTE = DATA[k].d_minute;
            const int         SECOND = DATA[k].d_second;
            const int         MILLI  = DATA[k].d_milli;

            const bdet_Time VALUE(HOUR, MINUTE, SECOND, MILLI);

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateR(bdet_Time, Time);
        INVOKE_testGenerateArray(bdet_Time, Time);
      } break;
      case 22: {
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
        // generateString(ArCh *buffer, bStr value, int level, int spaces);
        // generateStringRaw(ArCh *buffer, bStr value, int level, int spaces);
        // generateStringArray(ArCh *buf, cArStr& val, int lev, int sp);
        // generateStringArrayRaw(ArCh *buf, cArStr& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateString*" << endl
                          << "=======================" << endl;

        static const
        SampleValuesGenerateString DATA[] = {
            //LN    char       expected
            //--- ----------- ------------------
            { L_, "abc",      "abc"              },
            { L_, "3(x)",     "3(x)"             },
            { L_, "?[\n",     "\"\\?[\\n\""      },
            { L_, "<int>",    "<int>"            },
            { L_, "<in t>",   "\"<in t>\""       },
            { L_, "\\n\\\t",  "\"\\\\n\\\\\\t\"" },
            { L_, "\"ab c\"", "\"\\\"ab c\\\"\"" },
            { L_, "\"abc\"",  "\\\"abc\\\""      },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bsl::string> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const char * VALUE =DATA[k].d_value;

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateR(bsl::string, String);
        INVOKE_testGenerateArray(bsl::string, String);
      } break;
      case 21: {
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
        // generateShort(ArCh *buffer, short value, int level, int spaces);
        // generateShortRaw(ArCh *buffer, short value, int level, int spaces);
        // generateShortArray(ArCh *buf, cArSh& val, int lev, int sp);
        // generateShortArrayRaw(ArCh *buf, cArSh& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateShort*" << endl
                          << "======================" << endl;

        static const
        SampleValuesGenerateShort DATA[] = {
            //LN   short
            //---  ----
            { L_,  1978,  "1978" },
            { L_,     0,  "0"    },
            { L_,   -12,  "-12"  },
            { L_,   257,  "257"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<short> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const short VALUE = DATA[k].d_value;

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateB(short, Short);
        INVOKE_testGenerateArray(short, Short);
      } break;
      case 20: {
        // TBD
// [20] generateIntegerLocator(b_ca *, int val, int level, int spaces);
// [20] generateIntegerLocatorRaw(b_ca *, int val, int level, int spaces);
      } break;
      case 19: {
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
        // generateInt64(ArCh *buffer, int64 value, int level, int spaces);
        // generateInt64Raw(ArCh *buffer, int64 value, int level, int spaces);
        // generateInt64Array(ArCh *buf, cArInt64& val, int lev, int sp);
        // generateInt64ArrayRaw(ArCh *buf, cArInt64& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateInt64*" << endl
                          << "======================" << endl;

        static const
        SampleValuesGenerateInt64 DATA[] = {
            //LN   Int64
            //---  ----
            { L_,  1978,  "1978" },
            { L_,     0,  "0"    },
            { L_,   -12,  "-12"  },
            { L_,   257,  "257"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<Int64> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const Int64 VALUE = DATA[k].d_value;

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateB(Int64, Int64);
        INVOKE_testGenerateArray(Int64, Int64);
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING GENERATE INT
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   generateInt(ArCh *buffer, int value, int level, int spaces);
        //   generateIntRaw(ArCh *buffer, int value, int level, int spaces);
        //   generateIntArray(ArCh *buf, cArInt& val, int lev, int sp);
        //   generateIntArrayRaw(ArCh *buf, cArInt& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateInt*" << endl
                          << "====================" << endl;

        static const
        SampleValuesGenerateInt DATA[] = {
            //LN   Int64
            //---  ----
            { L_,  1978,  "1978" },
            { L_,     0,  "0"    },
            { L_,   -12,  "-12"  },
            { L_,   257,  "257"  },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<int> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const int VALUE = DATA[k].d_value;

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateB(int, Int);
        INVOKE_testGenerateArray(int, Int);
      } break;
      case 17: {
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
        // generateFloat(ArCh *buffer, float value, int level, int spaces);
        // generateFloatRaw(ArCh *buffer, float value, int level,int spaces);
        // generateFloatArray(ArCh *buf, cArFl& val, int lev, int sp);
        // generateFloatArrayRaw(ArCh *buf, cArFl& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateFloat*" << endl
                          << "=======================" << endl;

        static const
        SampleValuesGenerateFloat DATA[] = {
            //LN   float
            //---  ----
            { L_,   1978,  "1.978e3" },
            { L_,      0,  "0.0"     },
            { L_, -12.50,  "-12.5"   },
            { L_,    257,  "257"     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<float> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const float VALUE = DATA[k].d_value;

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateC(float, Float);
        INVOKE_testGenerateArray(float, Float);
      } break;
      case 16: {
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
        // generateDouble(ArCh *buffer, double value, int level, int spaces);
        // generateDoubleRaw(ArCh *buffer, double value, int level,int spaces);
        // generateDoubleArray(ArCh *buf, cArDb& val, int lev, int sp);
        // generateDoubleArrayRaw(ArCh *buf, cArDb& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDouble*" << endl
                          << "=======================" << endl;

        static const
        SampleValuesGenerateDouble DATA[] = {
            //LN   double  expected
            //---  -----   ---------
            { L_,   1978,  "1.978e3" },
            { L_,      0,  "0.0"     },
            { L_, -12.50,  "-12.5"   },
            { L_,    257,  "257"     },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<double> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const double VALUE = DATA[k].d_value;

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateC(double, Double);
        INVOKE_testGenerateArray(double, Double);
      } break;
      case 15: {
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
        // generateDatetime(ArCh *buffer, bDT value, int level, int spaces);
        // generateDatetimeRaw(ArCh *buffer, bDT value, int level, int spaces);
        // generateDatetimeArray(ArCh *buf, cArDT& val, int lev, int sp);
        // generateDatetimeArrayRaw(ArCh *buf, cArDT& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDatetime*" << endl
                          << "=========================" << endl;


        static const
        SampleValuesGenerateDatetime DATA[] = {
            //LN  YY    MM  DD  hr  min  sec   ms  exp
            //--- ----  --  --  --- ---  ---  ---  ----------------------
            { L_, 1978,  8, 27,  1,   2,   3,   4, "1978/08/27 01:02:03.004" },
            { L_, 2000, 11,  4, 12,  21,  32,  40, "2000/11/04 12:21:32.040" },
            { L_, 2003,  4, 18,  7,  49,  56, 417, "2003/04/18 07:49:56.417" },
            { L_, 2008,  9, 30, 24,   0,   0,   0, "2008/09/30 24:00:00.000" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_Datetime> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const int         YEAR   = DATA[k].d_year;
            const int         MONTH  = DATA[k].d_month;
            const int         DAY    = DATA[k].d_day;
            const int         HOUR   = DATA[k].d_hour;
            const int         MINUTE = DATA[k].d_minute;
            const int         SECOND = DATA[k].d_second;
            const int         MILLI  = DATA[k].d_milli;

            const bdet_Datetime VALUE(YEAR,
                                      MONTH,
                                      DAY,
                                      HOUR,
                                      MINUTE,
                                      SECOND,
                                      MILLI);

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateR(bdet_Datetime, Datetime);
        INVOKE_testGenerateArray(bdet_Datetime, Datetime);
      } break;
      case 14: {
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
        //   generateDate(ArCh *buffer, bDate value, int level, int spaces);
        //   generateDateRaw(ArCh *buffer, bDate value, int level, int spaces);
        //   generateDateArray(ArCh *buf, cArDate& val, int lev, int sp);
        //   generateDateArrayRaw(ArCh *buf, cArDate& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateDate*" << endl
                          << "=====================" << endl;

        static const
        SampleValuesGenerateDate DATA[] = {
            //LN  YY    MM  DD  exp
            //--- ----  --  --  ----------------------
            { L_, 1978,  8, 27, "1978/08/27" },
            { L_, 2000, 11,  4, "2000/11/04" },
            { L_, 2003,  4, 18, "2003/04/18" },
            { L_, 2008,  9, 30, "2008/09/30" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_Date> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const int         YEAR   = DATA[k].d_year;
            const int         MONTH  = DATA[k].d_month;
            const int         DAY    = DATA[k].d_day;

            const bdet_Date VALUE(YEAR, MONTH, DAY);

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateR(bdet_Date, Date);
        INVOKE_testGenerateArray(bdet_Date, Date);
      } break;
      case 13: {
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
        //   generateChar(ArCh *buffer, char value, int level, int spaces);
        //   generateCharRaw(ArCh *buffer, char value, int level, int spaces);
        //   generateCharArray(ArCh *buf, cArCh& val, int lev, int sp);
        //   generateCharArrayRaw(ArCh *buf, cArCh& val, int lev, int sp);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING generateChar*" << endl
                          << "=====================" << endl;
        static const
        SampleValuesGenerateChar DATA[] = {
            //LN    char   expected
            //---  -----   --------
            { L_,    'a',  "'a'"    },
            { L_,    '3',  "'3'"    },
            { L_,    '?',  "'\\?'"  },
            { L_,   '\t',  "'\\t'"  },
            { L_,    ')',  "')'"    },
            { L_,    '"',  "'\\\"'" },
            { L_,    '\n',  "'\\n'" },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<char> VALUES(testAllocator_p);
        VALUES.reserve(NUM_DATA);

        for (int k = 0; k < NUM_DATA; ++k) {
            const char VALUE = DATA[k].d_value;

            VALUES.push_back(VALUE);
        }

        INVOKE_testGenerateC(char, Char);
        INVOKE_testGenerateArray(char, Char);
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING STRIP NULL
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   stripNull(ArCh *buffer);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING stripNull" << endl
                          << "=================" << endl;

        {
            static const struct SampleValues {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_length;   // specification string length
                const char *d_exp_p;    // expectation specification
                int         d_explen;   // expectation specification length
            } DATA[] = {
                //line  spec             length  exp              explen
                //----  ---------------  ------  ---------------  ------
                { L_,   "",                 0,   "",                 0   },
                { L_,   " ",                1,   " ",                1   },
                { L_,   "\0",               1,   "",                 0   },
                { L_,   "  ",               2,   "  ",               2   },
                { L_,   " \0",              2,   " ",                1   },
                { L_,   "\0\0",             2,   "\0",               1   },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE           = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         LENGTH = DATA[ti].d_length;
                const char *const EXP    = DATA[ti].d_exp_p;
                const int         EXPLEN = DATA[ti].d_explen;

                bsl::vector<char> result(&testAllocator);
                result.resize(LENGTH);
                for (int i = 0; i < LENGTH; ++i) {
                    result[i] = SPEC[i];
                }
                bsl::vector<char> exp(&testAllocator);
                exp.resize(EXPLEN);
                for (int i = 0; i < EXPLEN; ++i) {
                    exp[i] = EXP[i];
                }
                bdepu_TypesParser::stripNull(&result);
                LOOP_ASSERT(LINE, exp == result);
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

        const bdet_Time INITIAL_VALUE_1( 1, 1, 1);  // first init. value
        const bdet_Time INITIAL_VALUE_2(19, 7, 9);  // second init. value

        static const
        ErrorParseSample DATA_ERRORS[] = {
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
        };
        const int NUM_DATA_ERRORS = sizeof DATA_ERRORS / sizeof *DATA_ERRORS;

        static const
        SampleValuesParseTime DATA[] = {
            //LN   spec                hours   mins secs    ms
            //---  --------------      -----   ---- ----  ----
            { L_,  "7:01:32",              7,     1,  32,    0 },
            { L_,  "08:24",                8,    24,   0,    0 },
            { L_,  "8:24",                 8,    24,   0,    0 },
            { L_,  "8:24:00",              8,    24,   0,    0 },
            { L_,  "8:24:00.0",            8,    24,   0,    0 },
            { L_,  "8:24:29.1",            8,    24,  29,  100 },
            { L_,  "08:24:29.1",           8,    24,  29,  100 },
            { L_,  "8:24:29.10",           8,    24,  29,  100 },
            { L_,  "04:31:00.100",         4,    31,   0,  100 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_Time> VALUES(&testAllocator);
        VALUES.reserve(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int HOUR          = DATA[ti].d_hour;
            const int MINS          = DATA[ti].d_minute;
            const int SECS          = DATA[ti].d_second;
            const int MILLI         = DATA[ti].d_milli;

            const bdet_Time VALUE(HOUR, MINS, SECS, MILLI);
            VALUES.push_back(VALUE);
        }
        ASSERT(NUM_DATA == VALUES.size());

        INVOKE_testParseErrorInput(bdet_Time, Time);

        INVOKE_testParseValidInput(bdet_Time, Time);

        INVOKE_testParseArray(bdet_Time, Time);
      } break;
      case 10: {
        // TBD
// [10] parseString(cchar **endPos, bStr *res, cchar *in);
// [10] parseStringArray(cchar **endPos, ArStr *res, cchar *in);
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

        const short INITIAL_VALUE_1 = -3;  // first initial value
        const short INITIAL_VALUE_2 =  9;  // second initial value

        static const
        ErrorParseSample DATA_ERRORS[] = {
            //line  spec             off
            //----  ---------------  ---
            { L_,   "?3",              0 },
            { L_,   "-",               1 },
            { L_,   "x42e#",           0 },
            { L_,   "+e2",             1 },
        };
        const int NUM_DATA_ERRORS = sizeof DATA_ERRORS / sizeof *DATA_ERRORS;

        static const
        SampleValuesParseShort DATA[] = {
            //line  spec             value
            //----  --------------   -----
            { L_,   "12",               12 },
            { L_,   "0",                 0 },
            { L_,   "-39",             -39 },
            { L_,   "3125",           3125 },
            { L_,   "-9843",         -9843 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<short> VALUES(&testAllocator);
        VALUES.reserve(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int VALUE         = DATA[ti].d_value;

            VALUES.push_back(VALUE);
        }
        ASSERT(NUM_DATA == VALUES.size());

        INVOKE_testParseErrorInputB(short, Short);

        INVOKE_testParseValidInputB(short, Short);

        INVOKE_testParseArray(short, Short);
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

        const Int64 INITIAL_VALUE_1 = -3;  // first initial value
        const Int64 INITIAL_VALUE_2 =  9;  // second initial value

        static const
        ErrorParseSample DATA_ERRORS[] = {
            //line  spec             off
            //----  ---------------  ---
            { L_,   "?3",              0 },
            { L_,   "-",               1 },
            { L_,   "x42e#",           0 },
            { L_,   "+e2",             1 },
        };
        const int NUM_DATA_ERRORS = sizeof DATA_ERRORS / sizeof *DATA_ERRORS;

        static const
        SampleValuesParseInt64 DATA[] = {
            //line  spec             value
            //----  --------------   -----
            { L_,   "12",               12 },
            { L_,   "0",                 0 },
            { L_,   "-39",             -39 },
            { L_,   "3125",           3125 },
            { L_,   "-9843",         -9843 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<Int64> VALUES(&testAllocator);
        VALUES.reserve(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const Int64    VALUE      = DATA[ti].d_value;

            VALUES.push_back(VALUE);
        }
        ASSERT(NUM_DATA == VALUES.size());

        INVOKE_testParseErrorInputB(Int64, Int64);

        INVOKE_testParseValidInputB(Int64, Int64);

        INVOKE_testParseArray(Int64, Int64);
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

        const int INITIAL_VALUE_1 = -3;  // first initial value
        const int INITIAL_VALUE_2 =  9;  // second initial value

        static const
        ErrorParseSample DATA_ERRORS[] = {
            //line  spec             off
            //----  ---------------  ---
            { L_,   "?3",              0 },
            { L_,   "-",               1 },
            { L_,   "x42e#",           0 },
            { L_,   "+e2",             1 },
        };
        const int NUM_DATA_ERRORS = sizeof DATA_ERRORS / sizeof *DATA_ERRORS;

        static const
        SampleValuesParseInt DATA[] = {
            //line  spec             value
            //----  --------------   -----
            { L_,   "12",               12 },
            { L_,   "0",                 0 },
            { L_,   "-39",             -39 },
            { L_,   "3125",           3125 },
            { L_,   "-9843",         -9843 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<int> VALUES(&testAllocator);
        VALUES.reserve(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int    VALUE      = DATA[ti].d_value;

            VALUES.push_back(VALUE);
        }
        ASSERT(NUM_DATA == VALUES.size());

        INVOKE_testParseErrorInputB(int, Int);

        INVOKE_testParseValidInputB(int, Int);

        INVOKE_testParseArray(int, Int);
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

        const float INITIAL_VALUE_1 = -3.0F;  // first initial value
        const float INITIAL_VALUE_2 =  9.1F;  // second initial value

        static const
        ErrorParseSample DATA_ERRORS[] = {
            //line  spec             off
            //----  ---------------  ---
            { L_,   "?3",              0 },
            { L_,   "-",               1 },
            { L_,   "42e#",            3 },
        };
        const int NUM_DATA_ERRORS = sizeof DATA_ERRORS / sizeof *DATA_ERRORS;

        static const
        SampleValuesParseFloat DATA[] = {
            //line  spec             value
            //----  --------------   -----
            { L_,   "12",               12 },
            { L_,   "0.75",           0.75 },
            { L_,   "3.125e2",       312.5 },
            { L_,   "-39",             -39 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<float> VALUES(&testAllocator);
        VALUES.reserve(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const float VALUE      = DATA[ti].d_value;

            VALUES.push_back(VALUE);
        }
        ASSERT(NUM_DATA == VALUES.size());

        INVOKE_testParseErrorInput(float, Float);

        INVOKE_testParseValidInput(float, Float);

        INVOKE_testParseArray(float, Float);
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

        const double INITIAL_VALUE_1 = -3.0;  // first initial value
        const double INITIAL_VALUE_2 =  9.1;  // second initial value

        static const
        ErrorParseSample DATA_ERRORS[] = {
            //line  spec             off
            //----  ---------------  ---
            { L_,   "?3",              0 },
            { L_,   "-",               1 },
            { L_,   "42e#",            3 },
        };
        const int NUM_DATA_ERRORS = sizeof DATA_ERRORS / sizeof *DATA_ERRORS;

        static const
        SampleValuesParseDouble DATA[] = {
            //line  spec             value
            //----  --------------   -----
            { L_,   "12",               12 },

            // DRQS 13577520 - add more integer test cases
            { L_,                      "1",                1LL },
            { L_,                     "-1",               -1LL },
            { L_,                      "2",                2LL },
            { L_,                     "-2",               -2LL },
            { L_,                      "3",                3LL },
            { L_,                     "-3",               -3LL },
            { L_,                      "4",                4LL },
            { L_,                     "-4",               -4LL },
            { L_,                      "5",                5LL },
            { L_,                     "-5",               -5LL },
            { L_,                      "8",                8LL },
            { L_,                     "-8",               -8LL },
            { L_,                      "9",                9LL },
            { L_,                     "-9",               -9LL },
            { L_,                     "16",               16LL },
            { L_,                    "-16",              -16LL },
            { L_,                     "17",               17LL },
            { L_,                    "-17",              -17LL },
            { L_,                     "32",               32LL },
            { L_,                    "-32",              -32LL },
            { L_,                     "33",               33LL },
            { L_,                    "-33",              -33LL },
            { L_,                     "64",               64LL },
            { L_,                    "-64",              -64LL },
            { L_,                     "65",               65LL },
            { L_,                    "-65",              -65LL },
            { L_,                    "128",              128LL },
            { L_,                   "-128",             -128LL },
            { L_,                    "129",              129LL },
            { L_,                   "-129",             -129LL },
            { L_,                    "256",              256LL },
            { L_,                   "-256",             -256LL },
            { L_,                    "257",              257LL },
            { L_,                   "-257",             -257LL },
            { L_,                    "512",              512LL },
            { L_,                   "-512",             -512LL },
            { L_,                    "513",              513LL },
            { L_,                   "-513",             -513LL },
            { L_,                   "1024",             1024LL },
            { L_,                  "-1024",            -1024LL },
            { L_,                   "1025",             1025LL },
            { L_,                  "-1025",            -1025LL },
            { L_,                   "2048",             2048LL },
            { L_,                  "-2048",            -2048LL },
            { L_,                   "2049",             2049LL },
            { L_,                  "-2049",            -2049LL },
            { L_,                   "4096",             4096LL },
            { L_,                  "-4096",            -4096LL },
            { L_,                   "4097",             4097LL },
            { L_,                  "-4097",            -4097LL },
            { L_,                   "8192",             8192LL },
            { L_,                  "-8192",            -8192LL },
            { L_,                   "8193",             8193LL },
            { L_,                  "-8193",            -8193LL },
            { L_,                  "16384",            16384LL },
            { L_,                 "-16384",           -16384LL },
            { L_,                  "16385",            16385LL },
            { L_,                 "-16385",           -16385LL },
            { L_,                  "32768",            32768LL },
            { L_,                 "-32768",           -32768LL },
            { L_,                  "32769",            32769LL },
            { L_,                 "-32769",           -32769LL },
            { L_,                  "65536",            65536LL },
            { L_,                 "-65536",           -65536LL },
            { L_,                  "65537",            65537LL },
            { L_,                 "-65537",           -65537LL },
            { L_,                 "131072",           131072LL },
            { L_,                "-131072",          -131072LL },
            { L_,                 "131073",           131073LL },
            { L_,                "-131073",          -131073LL },
            { L_,                 "262144",           262144LL },
            { L_,                "-262144",          -262144LL },
            { L_,                 "262145",           262145LL },
            { L_,                "-262145",          -262145LL },
            { L_,                 "524288",           524288LL },
            { L_,                "-524288",          -524288LL },
            { L_,                 "524289",           524289LL },
            { L_,                "-524289",          -524289LL },
            { L_,                "1048576",          1048576LL },
            { L_,               "-1048576",         -1048576LL },
            { L_,                "1048577",          1048577LL },
            { L_,               "-1048577",         -1048577LL },
            { L_,                "2097152",          2097152LL },
            { L_,               "-2097152",         -2097152LL },
            { L_,                "2097153",          2097153LL },
            { L_,               "-2097153",         -2097153LL },
            { L_,                "4194304",          4194304LL },
            { L_,               "-4194304",         -4194304LL },
            { L_,                "4194305",          4194305LL },
            { L_,               "-4194305",         -4194305LL },
            { L_,                "8388608",          8388608LL },
            { L_,               "-8388608",         -8388608LL },
            { L_,                "8388609",          8388609LL },
            { L_,               "-8388609",         -8388609LL },
            { L_,               "16777216",         16777216LL },
            { L_,              "-16777216",        -16777216LL },
            { L_,               "16777217",         16777217LL },
            { L_,              "-16777217",        -16777217LL },
            { L_,               "33554432",         33554432LL },
            { L_,              "-33554432",        -33554432LL },
            { L_,               "33554433",         33554433LL },
            { L_,              "-33554433",        -33554433LL },
            { L_,               "67108864",         67108864LL },
            { L_,              "-67108864",        -67108864LL },
            { L_,               "67108865",         67108865LL },
            { L_,              "-67108865",        -67108865LL },
            { L_,              "134217728",        134217728LL },
            { L_,             "-134217728",       -134217728LL },
            { L_,              "134217729",        134217729LL },
            { L_,             "-134217729",       -134217729LL },
            { L_,              "268435456",        268435456LL },
            { L_,             "-268435456",       -268435456LL },
            { L_,              "268435457",        268435457LL },
            { L_,             "-268435457",       -268435457LL },
            { L_,              "536870912",        536870912LL },
            { L_,             "-536870912",       -536870912LL },
            { L_,              "536870913",        536870913LL },
            { L_,             "-536870913",       -536870913LL },
            { L_,             "1073741824",       1073741824LL },
            { L_,            "-1073741824",      -1073741824LL },
            { L_,             "1073741825",       1073741825LL },
            { L_,            "-1073741825",      -1073741825LL },
            { L_,             "2147483648",       2147483648LL },
            { L_,            "-2147483648",      -2147483648LL },
            { L_,             "2147483649",       2147483649LL },
            { L_,            "-2147483649",      -2147483649LL },
            { L_,             "4294967296",       4294967296LL },
            { L_,            "-4294967296",      -4294967296LL },
            { L_,             "4294967297",       4294967297LL },
            { L_,            "-4294967297",      -4294967297LL },
            { L_,             "8589934592",       8589934592LL },
            { L_,            "-8589934592",      -8589934592LL },
            { L_,             "8589934593",       8589934593LL },
            { L_,            "-8589934593",      -8589934593LL },
            { L_,            "17179869184",      17179869184LL },
            { L_,           "-17179869184",     -17179869184LL },
            { L_,            "17179869185",      17179869185LL },
            { L_,           "-17179869185",     -17179869185LL },
            { L_,            "34359738368",      34359738368LL },
            { L_,           "-34359738368",     -34359738368LL },
            { L_,            "34359738369",      34359738369LL },
            { L_,           "-34359738369",     -34359738369LL },
            { L_,            "68719476736",      68719476736LL },
            { L_,           "-68719476736",     -68719476736LL },
            { L_,            "68719476737",      68719476737LL },
            { L_,           "-68719476737",     -68719476737LL },
            { L_,           "137438953472",     137438953472LL },
            { L_,          "-137438953472",    -137438953472LL },
            { L_,           "137438953473",     137438953473LL },
            { L_,          "-137438953473",    -137438953473LL },
            { L_,           "274877906944",     274877906944LL },
            { L_,          "-274877906944",    -274877906944LL },
            { L_,           "274877906945",     274877906945LL },
            { L_,          "-274877906945",    -274877906945LL },
            { L_,           "549755813888",     549755813888LL },
            { L_,          "-549755813888",    -549755813888LL },
            { L_,           "549755813889",     549755813889LL },
            { L_,          "-549755813889",    -549755813889LL },
            { L_,          "1099511627776",    1099511627776LL },
            { L_,         "-1099511627776",   -1099511627776LL },
            { L_,          "1099511627777",    1099511627777LL },
            { L_,         "-1099511627777",   -1099511627777LL },
            { L_,          "2199023255552",    2199023255552LL },
            { L_,         "-2199023255552",   -2199023255552LL },
            { L_,          "2199023255553",    2199023255553LL },
            { L_,         "-2199023255553",   -2199023255553LL },
            { L_,          "4398046511104",    4398046511104LL },
            { L_,         "-4398046511104",   -4398046511104LL },
            { L_,          "4398046511105",    4398046511105LL },
            { L_,         "-4398046511105",   -4398046511105LL },
            { L_,          "8796093022208",    8796093022208LL },
            { L_,         "-8796093022208",   -8796093022208LL },
            { L_,          "8796093022209",    8796093022209LL },
            { L_,         "-8796093022209",   -8796093022209LL },
            { L_,         "17592186044416",   17592186044416LL },
            { L_,        "-17592186044416",  -17592186044416LL },
            { L_,         "17592186044417",   17592186044417LL },
            { L_,        "-17592186044417",  -17592186044417LL },
            { L_,         "35184372088832",   35184372088832LL },
            { L_,        "-35184372088832",  -35184372088832LL },
            { L_,         "35184372088833",   35184372088833LL },
            { L_,        "-35184372088833",  -35184372088833LL },
            { L_,         "70368744177664",   70368744177664LL },
            { L_,        "-70368744177664",  -70368744177664LL },
            { L_,         "70368744177665",   70368744177665LL },
            { L_,        "-70368744177665",  -70368744177665LL },
            { L_,        "140737488355328",  140737488355328LL },
            { L_,       "-140737488355328", -140737488355328LL },
            { L_,        "140737488355329",  140737488355329LL },
            { L_,       "-140737488355329", -140737488355329LL },
            { L_,        "281474976710656",  281474976710656LL },
            { L_,       "-281474976710656", -281474976710656LL },
            { L_,        "281474976710657",  281474976710657LL },
            { L_,       "-281474976710657", -281474976710657LL },
            { L_,        "562949953421312",  562949953421312LL },
            { L_,       "-562949953421312", -562949953421312LL },
            { L_,        "562949953421313",  562949953421313LL },
            { L_,       "-562949953421313", -562949953421313LL },
            { L_,       "1125899906842624", 1125899906842624LL },
            { L_,      "-1125899906842624",-1125899906842624LL },
            { L_,       "1125899906842635", 1125899906842635LL },
            { L_,      "-1125899906842635",-1125899906842635LL },
            { L_,       "2251799813685248", 2251799813685248LL },
            { L_,      "-2251799813685248",-2251799813685248LL },
            { L_,       "2251799813685270", 2251799813685270LL },
            { L_,      "-2251799813685270",-2251799813685270LL },
            { L_,       "4503599627370496", 4503599627370496LL },
            { L_,      "-4503599627370496",-4503599627370496LL },
            { L_,       "4503599627370541", 4503599627370541LL },
            { L_,      "-4503599627370541",-4503599627370541LL },
            { L_,       "9007199254740992", 9007199254740992LL },
            { L_,      "-9007199254740992",-9007199254740992LL },
            { L_,       "9007199254741082", 9007199254741082LL },
            { L_,      "-9007199254741082",-9007199254741082LL },

            { L_,   "0.75",           0.75 },
            { L_,  "-0.75",          -0.75 },
            { L_,   "3.125e2",       312.5 },
            { L_,  "-3.125e2",      -312.5 },
            { L_,   "-39",             -39 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<double> VALUES(&testAllocator);
        VALUES.reserve(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const double VALUE      = DATA[ti].d_value;

            VALUES.push_back(VALUE);
        }
        ASSERT(NUM_DATA == VALUES.size());

        INVOKE_testParseErrorInput(double, Double);

        INVOKE_testParseValidInput(double, Double);

        INVOKE_testParseArray(double, Double);
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

        const bdet_Datetime INITIAL_VALUE_1(2000, 1, 1, 1, 1, 1, 1);
        const bdet_Datetime INITIAL_VALUE_2(1000, 1, 1, 1, 1, 0, 0);

        static const
        ErrorParseSample DATA_ERRORS[] = {
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
            ///{ L_,   "2011/05/03",                     10 },
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
        };
        const int NUM_DATA_ERRORS = sizeof DATA_ERRORS / sizeof *DATA_ERRORS;

        static const
        SampleValuesParseDatetime DATA[] = {
            //LN  spec                          year month  day
            //                                 hours  mins secs    ms
            //--- ---------------               ---- -----  ---   ---
            { L_, "19/1/2 7:01:32",               19,    1,   2,
                                                   7,    1,  32,    0 },
            { L_, "19/01/02_08:24",               19,    1,   2,
                                                   8,   24,   0,    0 },
            { L_, "199/01/02 8:24",              199,    1,   2,
                                                   8,   24,   0,    0 },
            { L_, "1993/1/02_8:24:29.1",        1993,    1,   2,
                                                   8,   24,  29,  100 },
            { L_, "1993/01/2 18:57:09.98",      1993,    1,   2,
                                                  18,   57,   9,  980 },
            { L_, "1993/1/2_04:31:00.100",      1993,    1,   2,
                                                   4,   31,   0,  100 },
            { L_, "2003/11/22_18:24:29",        2003,   11,  22,
                                                  18,   24,  29,    0 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_Datetime> VALUES(&testAllocator);
        VALUES.reserve(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int YEAR          = DATA[ti].d_year;
            const int MONTH         = DATA[ti].d_month;
            const int DAY           = DATA[ti].d_day;
            const int HOUR          = DATA[ti].d_hour;
            const int MINS          = DATA[ti].d_minute;
            const int SECS          = DATA[ti].d_second;
            const int MILLI         = DATA[ti].d_milli;

            const bdet_Datetime VALUE(YEAR,
                                      MONTH,
                                      DAY,
                                      HOUR,
                                      MINS,
                                      SECS,
                                      MILLI);
            VALUES.push_back(VALUE);
        }
        ASSERT(NUM_DATA == VALUES.size());

        INVOKE_testParseErrorInput(bdet_Datetime, Datetime);

        INVOKE_testParseValidInput(bdet_Datetime, Datetime);

        INVOKE_testParseArray(bdet_Datetime, Datetime);
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

        const bdet_Date INITIAL_VALUE_1(   1, 1, 1);  // first init. value
        const bdet_Date INITIAL_VALUE_2(1923, 7, 9);  // second init. value

        static const
        ErrorParseSample DATA_ERRORS[] = {
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
        const int NUM_DATA_ERRORS = sizeof DATA_ERRORS / sizeof *DATA_ERRORS;

        static const
        SampleValuesParseDate DATA[] = {
            //line  spec             year  month  day
            //----  --------------   ----  -----  ---
            { L_,   "19/1/2",          19,     1,   2 },
            { L_,   "19/01/02",        19,     1,   2 },
            { L_,   "199/01/02",      199,     1,   2 },
            { L_,   "1993/1/02",     1993,     1,   2 },
            { L_,   "1993/01/2",     1993,     1,   2 },
            { L_,   "1993/01/02",    1993,     1,   2 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bdet_Date> VALUES(&testAllocator);
        VALUES.reserve(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const int YEAR          = DATA[ti].d_year;
            const int MONTH         = DATA[ti].d_month;
            const int DAY           = DATA[ti].d_day;

            const bdet_Date VALUE(YEAR, MONTH, DAY);
            VALUES.push_back(VALUE);
        }
        ASSERT(NUM_DATA == VALUES.size());

        INVOKE_testParseErrorInput(bdet_Date, Date);

        INVOKE_testParseValidInput(bdet_Date, Date);

        INVOKE_testParseArray(bdet_Date, Date);
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

        const char INITIAL_VALUE_1 = 'q';  // first initial value
        const char INITIAL_VALUE_2 = 'z';  // second initial value

        static const
        ErrorParseSample DATA_ERRORS[] = {
            //line  spec             off
            //----  ---------------  ---
            { L_,   "a",               0 },
            ///{ L_,   "'",               1 },
            { L_,   "''",              1 },
            { L_,   "x'",              0 },
        };
        const int NUM_DATA_ERRORS = sizeof DATA_ERRORS / sizeof *DATA_ERRORS;

        static const
        SampleValuesParseChar DATA[] = {
            //line  spec             value
            //----  --------------   -----
            { L_,   "'a'",            'a' },
            { L_,   "'\n'",           '\n' },
            { L_,   "','",            ',' },
            { L_,   "' '",            ' ' },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<char> VALUES(&testAllocator);
        VALUES.reserve(NUM_DATA);

        for (int ti = 0; ti < NUM_DATA ; ++ti) {
            const char VALUE        = DATA[ti].d_value;

            VALUES.push_back(VALUE);
        }
        ASSERT(NUM_DATA == VALUES.size());

        INVOKE_testParseErrorInput(char, Char);

        INVOKE_testParseValidInput(char, Char);

        INVOKE_testParseArray(char, Char);
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
