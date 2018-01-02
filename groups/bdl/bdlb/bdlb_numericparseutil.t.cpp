// bdlb_numericparseutil.t.cpp                                        -*-C++-*-

#include <bdlb_numericparseutil.h>

#include <bslma_defaultallocatorguard.h>   // for testing only
#include <bslma_testallocator.h>           // for testing only

#include <bsls_platform.h>
#include <bsls_types.h>

#include <bslim_testutil.h>

#include <bsl_algorithm.h>
#include <bsl_cstring.h>
#include <bsl_iterator.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#include <bsl_climits.h>
#include <bsl_limits.h>

#include <bsl_c_stdlib.h>

#include <math.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script
using namespace bdlb;

// For a deprecated component, may use 'pragma' to remove valueless warnings
// in the component's test driver.

#ifdef BSLS_PLATFORM_HAS_PRAGMA_GCC_DIAGNOSTIC
#pragma GCC diagnostic ignored "-Wconversion"
#endif

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
//-----------------------------------------------------------------------------
// [ 1] characterToDigit(char character, int base)
// [ 2] parseUnsignedInteger(result, rest, in, base, maxVal)
// [ 2] parseUnsignedInteger(result, rest, in, base, maxVal, maxDigit)
// [ 2] parseUnsignedInteger(result, in, base, maxVal)
// [ 2] parseUnsignedInteger(result, in, base, maxVal, maxDigit)
// [ 3] parseSignedInteger(result, rest, input, base, minVal, maxVal)
// [ 3] parseSignedInteger(result, input, base, minVal, maxVal)
// [ 4] parseDouble(double *res, StringRef *rest, StringRef in)
// [ 4] parseDouble(double *res, StringRef in)
// [ 5] parseInt(result, rest, input, base = 10)
// [ 5] parseInt(result, input, base = 10)
// [ 6] parseInt64(result, rest, input, base = 10)
// [ 6] parseInt64(result, input, base = 10)
// [ 7] parseUint(result, rest, input, base = 10)
// [ 7] parseUint(result, input, base = 10)
// [ 8] parseUint64(result, rest, input, base = 10)
// [ 8] parseUint64(result, input, base = 10)
// [ 9] parseShort(result, rest, input, base = 10)
// [ 9] parseShort(result, input, base = 10)
// [10] parseUshort(result, rest, input, base = 10)
// [10] parseUshort(result, input, base = 10)
//-----------------------------------------------------------------------------
// [11] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BSL ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);

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

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bsls::Types::Int64  Int64;
typedef bsls::Types::Uint64 Uint64;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

static
int doubleSign(double number)
    // Return 0 if the specified 'number' is positive, and a non-zero value
    // otherwise.  This function is needed only until we have all platforms
    // updated to C++11 that supports the 'signbit' function.
{
    const unsigned char *bytes = reinterpret_cast<unsigned char *>(&number);

#ifdef BSLS_PLATFORM_IS_BIG_ENDIAN
    static bsl::size_t pos = 0;
#elif defined(BSLS_PLATFORM_IS_LITTLE_ENDIAN)
    static bsl::size_t pos = sizeof(double) - 1;
#endif
    return bytes[pos] & 0x80;
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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    using bslstl::StringRef;

    switch (test) { case 0:  // Zero is always the leading case.
      case 11: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;
///Example 1: Parsing an Integer Value from a 'StringRef'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose that we have a 'StringRef' that presumably contains a (not
// necessarily NUL terminated) string representing a 32-bit integer value and
// we want to convert that string into an 'int' (32-bit integer).
//
// First, we create the string:
//..
    bslstl::StringRef input("2017");
//..
// Then we create the output variables for the parser:
//..
    int               year;
    bslstl::StringRef rest;
//..
// Next we call the parser function:
//..
    int rv = NumericParseUtil::parseInt(&year, &rest, input);
//..
// Then we verify the results:
//..
    ASSERT(0    == rv);
    ASSERT(2017 == year);
    ASSERT(rest.empty());
//..

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING PARSE USHORT
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large as representable
        //:    3 The value is just larger than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseUshort(result, rest, input, base = 10)
        //   parseUshort(result, input, base = 10)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE USHORT" << endl
                          << "====================" << endl;

        {
            const int INITIAL_VALUE_1 = 3;  // first initial value
            const int INITIAL_VALUE_2 = 9;  // second initial value

            static const struct {
                int            d_lineNum;  // source line number
                const char    *d_spec_p;   // specification string
                int            d_base;     // specification base
                ptrdiff_t      d_offset;   // expected number of parsed chars
                int            d_fail;     // parsing expected to fail flag
                unsigned short d_value;    // expected return value
            } DATA[] = {
                //line  spec            base offset  fail  value
                //----  --------------  ---- ------  ----  -------
                { L_,   "",               10,   0,    1,       0 },

                { L_,   "a",              10,   0,    1,       0 },
                { L_,   "+",              10,   1,    1,       0 },
                { L_,   "-",              10,   0,    1,       0 },
                { L_,   "0",              10,   1,    0,       0 },
                { L_,   "a",              16,   1,    0,      10 },
                { L_,   "f",              16,   1,    0,      15 },
                { L_,   "A",              16,   1,    0,      10 },
                { L_,   "F",              16,   1,    0,      15 },
                { L_,   "g",              16,   0,    1,       0 },
                { L_,   "0",               2,   1,    0,       0 },
                { L_,   "1",               2,   1,    0,       1 },
                { L_,   "a",              11,   1,    0,      10 },
                { L_,   "A",              11,   1,    0,      10 },
                { L_,   "z",              36,   1,    0,      35 },
                { L_,   "Z",              36,   1,    0,      35 },

                { L_,   "++",             10,   1,    1,       0 },
                { L_,   "+-",             10,   1,    1,       0 },
                { L_,   "-+",             10,   0,    1,       0 },
                { L_,   "--",             10,   0,    1,       0 },
                { L_,   "+a",             10,   1,    1,       0 },
                { L_,   "-a",             10,   0,    1,       0 },
                { L_,   "+0",             10,   2,    0,       0 },
                { L_,   "+9",             10,   2,    0,       9 },
                { L_,   "-0",             10,   0,    1,       0 },
                { L_,   "-9",             10,   0,    1,       0 },
                { L_,   "0a",             10,   1,    0,       0 },
                { L_,   "9a",             10,   1,    0,       9 },
                { L_,   "00",             10,   2,    0,       0 },
                { L_,   "01",             10,   2,    0,       1 },
                { L_,   "19",             10,   2,    0,      19 },
                { L_,   "99",             10,   2,    0,      99 },
                { L_,   "+g",             16,   1,    1,       0 },
                { L_,   "+a",             16,   2,    0,      10 },
                { L_,   "+f",             16,   2,    0,      15 },
                { L_,   "ff",             16,   2,    0,     255 },
                { L_,   "FF",             16,   2,    0,     255 },
                { L_,   "+0",              2,   2,    0,       0 },
                { L_,   "+1",              2,   2,    0,       1 },
                { L_,   "00",              2,   2,    0,       0 },
                { L_,   "01",              2,   2,    0,       1 },
                { L_,   "10",              2,   2,    0,       2 },
                { L_,   "11",              2,   2,    0,       3 },
                { L_,   "+z",             36,   2,    0,      35 },
                { L_,   "0z",             36,   2,    0,      35 },
                { L_,   "0Z",             36,   2,    0,      35 },
                { L_,   "10",             36,   2,    0,      36 },
                { L_,   "z0",             36,   2,    0,    1260 },
                { L_,   "Z0",             36,   2,    0,    1260 },

                { L_,   "+0a",            10,   2,    0,       0 },
                { L_,   "+9a",            10,   2,    0,       9 },
                { L_,   "+12",            10,   3,    0,      12 },
                { L_,   "+89",            10,   3,    0,      89 },
                { L_,   "123",            10,   3,    0,     123 },
                { L_,   "789",            10,   3,    0,     789 },
                { L_,   "+fg",            16,   2,    0,      15 },
                { L_,   "+ff",            16,   3,    0,     255 },
                { L_,   "+FF",            16,   3,    0,     255 },
                { L_,   "fff",            16,   3,    0,    4095 },
                { L_,   "fFf",            16,   3,    0,    4095 },
                { L_,   "FfF",            16,   3,    0,    4095 },
                { L_,   "FFF",            16,   3,    0,    4095 },

                { L_,   "1234",           10,   4,    0,    1234 },
                { L_,   "7FFF",           16,   4,    0,   32767 },

                { L_,   "12345",          10,   5,    0,   12345 },
                { L_,   "32766",          10,   5,    0,   32766 },
                { L_,   "32767",          10,   5,    0,   32767 },
                { L_,   "32768",          10,   5,    0,   32768 },
                { L_,   "32769",          10,   5,    0,   32769 },

                { L_,   "65535",          10,   5,    0,   65535 },
                { L_,   "65536",          10,   4,    0,    6553 },
                { L_,   "123456",         10,   5,    0,   12345 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int            LINE   = DATA[ti].d_lineNum;
                const char *const    SPEC   = DATA[ti].d_spec_p;
                const int            BASE   = DATA[ti].d_base;
                const ptrdiff_t      NUM    = DATA[ti].d_offset;
                const int            FAIL   = DATA[ti].d_fail;
                const unsigned short VALUE  = DATA[ti].d_value;
                const int            curLen = strlen(SPEC);

                if (veryVerbose) {
                    P(LINE);
                    P(SPEC);
                    P(BASE);
                }

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton strings of length "
                                          << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    unsigned short result = INITIAL_VALUE_1;
                    StringRef      rest;
                    int            rv = NumericParseUtil::parseUshort(&result,
                                                                      &rest,
                                                                      SPEC,
                                                                      BASE);
                    LOOP_ASSERT(LINE, NUM == rest.data() - SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    unsigned short result = INITIAL_VALUE_2;
                    StringRef      rest;
                    int            rv = NumericParseUtil::parseUshort(&result,
                                                                      &rest,
                                                                      SPEC,
                                                                      BASE);
                    LOOP_ASSERT(LINE, NUM == rest.data() - SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }

                // Testing without 'remainder' argument.

                {  // test with first initial value
                    unsigned short result = INITIAL_VALUE_1;
                    int            rv = NumericParseUtil::parseUshort(&result,
                                                                      SPEC,
                                                                      BASE);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    unsigned short result = INITIAL_VALUE_2;
                    int            rv = NumericParseUtil::parseUshort(&result,
                                                                      SPEC,
                                                                      BASE);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING PARSE SHORT
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseShort(result, rest, input, base = 10)
        //   parseShort(result, input, base = 10)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE SHORT" << endl
                          << "===================" << endl;

        {
            const int INITIAL_VALUE_1 = -3;  // first initial value
            const int INITIAL_VALUE_2 =  9;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                ptrdiff_t   d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                short       d_value;    // expected return value
            } DATA[] = {
                //line  spec            base offset  fail  value
                //----  --------------  ---- ------  ----  -------
                { L_,   "",               10,   0,    1,       0 },

                { L_,   "a",              10,   0,    1,       0 },
                { L_,   "+",              10,   1,    1,       0 },
                { L_,   "-",              10,   1,    1,       0 },
                { L_,   "0",              10,   1,    0,       0 },
                { L_,   "a",              16,   1,    0,      10 },
                { L_,   "f",              16,   1,    0,      15 },
                { L_,   "A",              16,   1,    0,      10 },
                { L_,   "F",              16,   1,    0,      15 },
                { L_,   "g",              16,   0,    1,       0 },
                { L_,   "0",               2,   1,    0,       0 },
                { L_,   "1",               2,   1,    0,       1 },
                { L_,   "a",              11,   1,    0,      10 },
                { L_,   "A",              11,   1,    0,      10 },
                { L_,   "z",              36,   1,    0,      35 },
                { L_,   "Z",              36,   1,    0,      35 },

                { L_,   "++",             10,   1,    1,       0 },
                { L_,   "+-",             10,   1,    1,       0 },
                { L_,   "-+",             10,   1,    1,       0 },
                { L_,   "--",             10,   1,    1,       0 },
                { L_,   "+a",             10,   1,    1,       0 },
                { L_,   "-a",             10,   1,    1,       0 },
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
                { L_,   "+g",             16,   1,    1,       0 },
                { L_,   "-g",             16,   1,    1,       0 },
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
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         BASE   = DATA[ti].d_base;
                const ptrdiff_t   NUM    = DATA[ti].d_offset;
                const int         FAIL   = DATA[ti].d_fail;
                const short       VALUE  = DATA[ti].d_value;
                const int         curLen = strlen(SPEC);

                if (veryVerbose) {
                    P(LINE);
                    P(SPEC);
                    P(BASE);
                }

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton strings of length "
                                          << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    short     result = INITIAL_VALUE_1;
                    StringRef rest;
                    int       rv = NumericParseUtil::
                                        parseShort(&result, &rest, SPEC, BASE);
                    LOOP_ASSERT(LINE, NUM == rest.data() - SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    short     result = INITIAL_VALUE_2;
                    StringRef rest;
                    int       rv = NumericParseUtil::
                                        parseShort(&result, &rest, SPEC, BASE);
                    LOOP_ASSERT(LINE, NUM == rest.data() - SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }

                // Test without the 'remainder' argument

                {  // test with first initial value
                    short     result = INITIAL_VALUE_1;
                    int       rv = NumericParseUtil::
                                               parseShort(&result, SPEC, BASE);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    short     result = INITIAL_VALUE_2;
                    int       rv = NumericParseUtil::
                                               parseShort(&result, SPEC, BASE);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING PARSE UINT64
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseUint64(result, rest, input, base = 10)
        //   parseUint64(result, input, base = 10)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE UINT64" << endl
                          << "====================" << endl;

        {
            const int INITIAL_VALUE_1 = -3;  // first initial value
            const int INITIAL_VALUE_2 =  9;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                ptrdiff_t   d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                Uint64      d_value;    // expected return value
            } DATA[] = {
                //line  spec            base offset  fail  value
                //----  --------------  ---- ------  ----  -------
                { L_,   "",               10,   0,    1,       0 },

                { L_,   "a",              10,   0,    1,       0 },
                { L_,   "+",              10,   1,    1,       0 },
                { L_,   "-",              10,   0,    1,       0 },
                { L_,   "0",              10,   1,    0,       0 },
                { L_,   "a",              16,   1,    0,      10 },
                { L_,   "f",              16,   1,    0,      15 },
                { L_,   "A",              16,   1,    0,      10 },
                { L_,   "F",              16,   1,    0,      15 },
                { L_,   "g",              16,   0,    1,       0 },
                { L_,   "0",               2,   1,    0,       0 },
                { L_,   "1",               2,   1,    0,       1 },
                { L_,   "a",              11,   1,    0,      10 },
                { L_,   "A",              11,   1,    0,      10 },
                { L_,   "z",              36,   1,    0,      35 },
                { L_,   "Z",              36,   1,    0,      35 },

                { L_,   "++",             10,   1,    1,       0 },
                { L_,   "+-",             10,   1,    1,       0 },
                { L_,   "-+",             10,   0,    1,       0 },
                { L_,   "--",             10,   0,    1,       0 },
                { L_,   "+a",             10,   1,    1,       0 },
                { L_,   "+0",             10,   2,    0,       0 },
                { L_,   "+9",             10,   2,    0,       9 },
                { L_,   "-0",             10,   0,    1,       0 },
                { L_,   "-9",             10,   0,    1,       0 },
                { L_,   "0a",             10,   1,    0,       0 },
                { L_,   "9a",             10,   1,    0,       9 },
                { L_,   "00",             10,   2,    0,       0 },
                { L_,   "01",             10,   2,    0,       1 },
                { L_,   "19",             10,   2,    0,      19 },
                { L_,   "99",             10,   2,    0,      99 },
                { L_,   "+g",             16,   1,    1,       0 },
                { L_,   "-g",             16,   0,    1,       0 },
                { L_,   "+a",             16,   2,    0,      10 },
                { L_,   "+f",             16,   2,    0,      15 },
                { L_,   "ff",             16,   2,    0,     255 },
                { L_,   "FF",             16,   2,    0,     255 },
                { L_,   "+0",              2,   2,    0,       0 },
                { L_,   "+1",              2,   2,    0,       1 },
                { L_,   "00",              2,   2,    0,       0 },
                { L_,   "01",              2,   2,    0,       1 },
                { L_,   "10",              2,   2,    0,       2 },
                { L_,   "11",              2,   2,    0,       3 },
                { L_,   "+z",             36,   2,    0,      35 },
                { L_,   "0z",             36,   2,    0,      35 },
                { L_,   "0Z",             36,   2,    0,      35 },
                { L_,   "10",             36,   2,    0,      36 },
                { L_,   "z0",             36,   2,    0,    1260 },
                { L_,   "Z0",             36,   2,    0,    1260 },

                { L_,   "+0a",            10,   2,    0,       0 },
                { L_,   "+9a",            10,   2,    0,       9 },
                { L_,   "+12",            10,   3,    0,      12 },
                { L_,   "+89",            10,   3,    0,      89 },
                { L_,   "123",            10,   3,    0,     123 },
                { L_,   "789",            10,   3,    0,     789 },
                { L_,   "+fg",            16,   2,    0,      15 },
                { L_,   "+ff",            16,   3,    0,     255 },
                { L_,   "+FF",            16,   3,    0,     255 },
                { L_,   "fff",            16,   3,    0,    4095 },
                { L_,   "fFf",            16,   3,    0,    4095 },
                { L_,   "FfF",            16,   3,    0,    4095 },
                { L_,   "FFF",            16,   3,    0,    4095 },

                { L_,   "1234",           10,   4,    0,    1234 },
                { L_,   "7FFF",           16,   4,    0,   32767 },

                { L_,   "12345",          10,   5,    0,   12345 },
                { L_,   "32766",          10,   5,    0,   32766 },
                { L_,   "32767",          10,   5,    0,   32767 },
                { L_,   "32768",          10,   5,    0,   32768 },
                { L_,   "32769",          10,   5,    0,   32769 },

                { L_,   "123456",         10,   6,    0,  123456 },

                { L_,   "2147483647",     10,  10,    0,  2147483647 },
                { L_,   "2147483648",     10,  10,    0,  2147483648LL },
                { L_,   "2147483649",     10,  10,    0,  2147483649LL },

                { L_,   "ffffffffffffffff",  16,  16,    0,
                                                  0xFFFFFFFFFFFFFFFFuLL },
                { L_,   "8000000000000000",  16,  16,    0,
                                                  0x8000000000000000uLL },

                { L_,   "18446744073709551615", 10,  20,    0,
                                                18446744073709551615ull },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         BASE   = DATA[ti].d_base;
                const ptrdiff_t   NUM    = DATA[ti].d_offset;
                const int         FAIL   = DATA[ti].d_fail;
                const Uint64      VALUE  = DATA[ti].d_value;
                const int         curLen = strlen(SPEC);

                if (veryVerbose) {
                    P(LINE);
                    P(SPEC);
                    P(BASE);
                }

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton strings of length "
                                          << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    Uint64    result = INITIAL_VALUE_1;
                    StringRef rest;
                    int       rv = NumericParseUtil::
                                       parseUint64(&result, &rest, SPEC, BASE);
                    LOOP_ASSERT(LINE, NUM == rest.data() - SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? (Uint64)INITIAL_VALUE_1
                                                   : VALUE));
                }

                {  // test with second initial value
                    Uint64    result = INITIAL_VALUE_2;
                    StringRef rest;
                    int       rv = NumericParseUtil::
                                       parseUint64(&result, &rest, SPEC, BASE);
                    LOOP_ASSERT(LINE, NUM == rest.data() - SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? (Uint64)INITIAL_VALUE_2
                                                   : VALUE));
                }

                // Test without the 'remainder' argument

                {  // test with first initial value
                    Uint64    result = INITIAL_VALUE_1;
                    int       rv = NumericParseUtil::
                                              parseUint64(&result, SPEC, BASE);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? (Uint64)INITIAL_VALUE_1
                                                   : VALUE));
                }

                {  // test with second initial value
                    Uint64    result = INITIAL_VALUE_2;
                    int       rv = NumericParseUtil::
                                              parseUint64(&result, SPEC, BASE);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? (Uint64)INITIAL_VALUE_2
                                                   : VALUE));
                }
            }
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING PARSE UINT
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseUint(result, rest, input, base = 10)
        //   parseUint(result, input, base = 10)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE UINT" << endl
                          << "==================" << endl;

        {
            const int INITIAL_VALUE_1 = -3;  // first initial value
            const int INITIAL_VALUE_2 =  9;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                ptrdiff_t   d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                unsigned    d_value;    // expected return value
            } DATA[] = {
                //line  spec            base offset  fail  value
                //----  --------------  ---- ------  ----  -------
                { L_,   "",               10,   0,    1,       0 },

                { L_,   "a",              10,   0,    1,       0 },
                { L_,   "+",              10,   1,    1,       0 },
                { L_,   "-",              10,   0,    1,       0 },
                { L_,   "0",              10,   1,    0,       0 },
                { L_,   "a",              16,   1,    0,      10 },
                { L_,   "f",              16,   1,    0,      15 },
                { L_,   "A",              16,   1,    0,      10 },
                { L_,   "F",              16,   1,    0,      15 },
                { L_,   "g",              16,   0,    1,       0 },
                { L_,   "0",               2,   1,    0,       0 },
                { L_,   "1",               2,   1,    0,       1 },
                { L_,   "a",              11,   1,    0,      10 },
                { L_,   "A",              11,   1,    0,      10 },
                { L_,   "z",              36,   1,    0,      35 },
                { L_,   "Z",              36,   1,    0,      35 },

                { L_,   "++",             10,   1,    1,       0 },
                { L_,   "+-",             10,   1,    1,       0 },
                { L_,   "-+",             10,   0,    1,       0 },
                { L_,   "--",             10,   0,    1,       0 },
                { L_,   "+a",             10,   1,    1,       0 },
                { L_,   "-a",             10,   0,    1,       0 },
                { L_,   "+0",             10,   2,    0,       0 },
                { L_,   "+9",             10,   2,    0,       9 },
                { L_,   "-0",             10,   0,    1,       0 },
                { L_,   "0a",             10,   1,    0,       0 },
                { L_,   "9a",             10,   1,    0,       9 },
                { L_,   "00",             10,   2,    0,       0 },
                { L_,   "01",             10,   2,    0,       1 },
                { L_,   "19",             10,   2,    0,      19 },
                { L_,   "99",             10,   2,    0,      99 },
                { L_,   "+g",             16,   1,    1,       0 },
                { L_,   "+a",             16,   2,    0,      10 },
                { L_,   "+f",             16,   2,    0,      15 },
                { L_,   "ff",             16,   2,    0,     255 },
                { L_,   "FF",             16,   2,    0,     255 },
                { L_,   "+0",              2,   2,    0,       0 },
                { L_,   "+1",              2,   2,    0,       1 },
                { L_,   "00",              2,   2,    0,       0 },
                { L_,   "01",              2,   2,    0,       1 },
                { L_,   "10",              2,   2,    0,       2 },
                { L_,   "11",              2,   2,    0,       3 },
                { L_,   "+z",             36,   2,    0,      35 },
                { L_,   "0z",             36,   2,    0,      35 },
                { L_,   "0Z",             36,   2,    0,      35 },
                { L_,   "10",             36,   2,    0,      36 },
                { L_,   "z0",             36,   2,    0,    1260 },
                { L_,   "Z0",             36,   2,    0,    1260 },

                { L_,   "+0a",            10,   2,    0,       0 },
                { L_,   "+9a",            10,   2,    0,       9 },
                { L_,   "+12",            10,   3,    0,      12 },
                { L_,   "+89",            10,   3,    0,      89 },
                { L_,   "-12",            10,   0,    1,       0 },
                { L_,   "123",            10,   3,    0,     123 },
                { L_,   "789",            10,   3,    0,     789 },
                { L_,   "+fg",            16,   2,    0,      15 },
                { L_,   "+ff",            16,   3,    0,     255 },
                { L_,   "+FF",            16,   3,    0,     255 },
                { L_,   "fff",            16,   3,    0,    4095 },
                { L_,   "fFf",            16,   3,    0,    4095 },
                { L_,   "FfF",            16,   3,    0,    4095 },
                { L_,   "FFF",            16,   3,    0,    4095 },

                { L_,   "1234",           10,   4,    0,    1234 },
                { L_,   "7FFF",           16,   4,    0,   32767 },

                { L_,   "12345",          10,   5,    0,   12345 },
                { L_,   "32766",          10,   5,    0,   32766 },
                { L_,   "32767",          10,   5,    0,   32767 },
                { L_,   "32768",          10,   5,    0,   32768 },
                { L_,   "32769",          10,   5,    0,   32769 },

                { L_,   "123456",         10,   6,    0,  123456 },

                { L_,   "2147483647",     10,  10,    0,  2147483647 },
                { L_,   "2147483648",     10,  10,    0,   2147483648 },
                { L_,   "2147483649",     10,  10,    0,   2147483649 },

                { L_,   "4294967295",     10,  10,    0,  4294967295 },
                { L_,   "4294967296",     10,   9,    0,  429496729 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         BASE   = DATA[ti].d_base;
                const ptrdiff_t   NUM    = DATA[ti].d_offset;
                const int         FAIL   = DATA[ti].d_fail;
                const unsigned    VALUE  = DATA[ti].d_value;
                const int         curLen = strlen(SPEC);

                if (veryVerbose) {
                    P(LINE);
                    P(SPEC);
                    P(BASE);
                }

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton strings of length "
                                          << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    unsigned  result = INITIAL_VALUE_1;
                    StringRef rest;
                    int       rv = NumericParseUtil::
                                         parseUint(&result, &rest, SPEC, BASE);
                    LOOP_ASSERT(LINE, NUM == rest.data() - SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    unsigned  result = INITIAL_VALUE_2;
                    StringRef rest;
                    int       rv = NumericParseUtil::
                                         parseUint(&result, &rest, SPEC, BASE);
                    LOOP_ASSERT(LINE, NUM == rest.data() - SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }

                // Test without the 'remainder' argument

                {  // test with first initial value
                    unsigned  result = INITIAL_VALUE_1;
                    int       rv = NumericParseUtil::
                                                parseUint(&result, SPEC, BASE);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    unsigned  result = INITIAL_VALUE_2;
                    int       rv = NumericParseUtil::
                                                parseUint(&result, SPEC, BASE);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING PARSE INT64
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseInt64(result, rest, input, base = 10)
        //   parseInt64(result, input, base = 10)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE INT64" << endl
                          << "===================" << endl;

        {
            const Int64 INITIAL_VALUE_1 = -3;  // first initial value
            const Int64 INITIAL_VALUE_2 =  9;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                ptrdiff_t   d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                Int64       d_value;    // expected return value
            } DATA[] = {
                //line  spec            base offset  fail  value
                //----  --------------  ---- ------  ----  -------
                { L_,   "",               10,   0,    1,       0 },

                { L_,   "a",              10,   0,    1,       0 },
                { L_,   "+",              10,   1,    1,       0 },
                { L_,   "-",              10,   1,    1,       0 },
                { L_,   "0",              10,   1,    0,       0 },
                { L_,   "a",              16,   1,    0,      10 },
                { L_,   "f",              16,   1,    0,      15 },
                { L_,   "A",              16,   1,    0,      10 },
                { L_,   "F",              16,   1,    0,      15 },
                { L_,   "g",              16,   0,    1,       0 },
                { L_,   "0",               2,   1,    0,       0 },
                { L_,   "1",               2,   1,    0,       1 },
                { L_,   "a",              11,   1,    0,      10 },
                { L_,   "A",              11,   1,    0,      10 },
                { L_,   "z",              36,   1,    0,      35 },
                { L_,   "Z",              36,   1,    0,      35 },

                { L_,   "++",             10,   1,    1,       0 },
                { L_,   "+-",             10,   1,    1,       0 },
                { L_,   "-+",             10,   1,    1,       0 },
                { L_,   "--",             10,   1,    1,       0 },
                { L_,   "+a",             10,   1,    1,       0 },
                { L_,   "-a",             10,   1,    1,       0 },
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
                { L_,   "+g",             16,   1,    1,       0 },
                { L_,   "-g",             16,   1,    1,       0 },
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

                { L_,   "-7FFFFFFFFFFFFFFF", 16,  17,    0,
                                                       -0x7FFFFFFFFFFFFFFFLL },

                { L_,   "9223372036854775807", 10, 19, 0,
                                                       9223372036854775807ll },

                { L_,   "-9223372036854775807", 10, 20, 0,
                                                      -9223372036854775807ll },

                { L_,   "-9223372036854775809", 10, 19, 0,
                                                       -922337203685477580ll },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         BASE   = DATA[ti].d_base;
                const ptrdiff_t   NUM    = DATA[ti].d_offset;
                const int         FAIL   = DATA[ti].d_fail;
                const Int64       VALUE  = DATA[ti].d_value;
                const int         curLen = strlen(SPEC);

                if (veryVerbose) {
                    P(LINE);
                    P(SPEC);
                    P(BASE);
                }

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton strings of length "
                                          << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    Int64     result = INITIAL_VALUE_1;
                    StringRef rest;
                    int       rv = NumericParseUtil::
                                        parseInt64(&result, &rest, SPEC, BASE);
                    LOOP_ASSERT(LINE, NUM == rest.data() - SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP3_ASSERT(LINE, result, VALUE,
                                 result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    Int64     result = INITIAL_VALUE_2;
                    StringRef rest;
                    int       rv = NumericParseUtil::
                                        parseInt64(&result, &rest, SPEC, BASE);
                    LOOP_ASSERT(LINE, NUM == rest.data() - SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP3_ASSERT(LINE, result, VALUE,
                                 result == (rv ? INITIAL_VALUE_2 : VALUE));
                }

                // Test without the 'remainder' argument

                {  // test with first initial value
                    Int64     result = INITIAL_VALUE_1;
                    int       rv = NumericParseUtil::
                                               parseInt64(&result, SPEC, BASE);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP3_ASSERT(LINE, result, VALUE,
                                 result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    Int64     result = INITIAL_VALUE_2;
                    int       rv = NumericParseUtil::
                                               parseInt64(&result, SPEC, BASE);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP3_ASSERT(LINE, result, VALUE,
                                 result == (rv ? INITIAL_VALUE_2 : VALUE));
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PARSE INT
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseInt(result, rest, input, base = 10)
        //   parseInt(result, input, base = 10)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE INT" << endl
                          << "=================" << endl;

        {
            const int INITIAL_VALUE_1 = -3;  // first initial value
            const int INITIAL_VALUE_2 =  9;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                ptrdiff_t   d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                int         d_value;    // expected return value
            } DATA[] = {
                //line  spec            base offset  fail  value
                //----  --------------  ---- ------  ----  -------
                { L_,   "",               10,   0,    1,       0 },

                { L_,   "a",              10,   0,    1,       0 },
                { L_,   "+",              10,   1,    1,       0 },
                { L_,   "-",              10,   1,    1,       0 },
                { L_,   "0",              10,   1,    0,       0 },
                { L_,   "a",              16,   1,    0,      10 },
                { L_,   "f",              16,   1,    0,      15 },
                { L_,   "A",              16,   1,    0,      10 },
                { L_,   "F",              16,   1,    0,      15 },
                { L_,   "g",              16,   0,    1,       0 },
                { L_,   "0",               2,   1,    0,       0 },
                { L_,   "1",               2,   1,    0,       1 },
                { L_,   "a",              11,   1,    0,      10 },
                { L_,   "A",              11,   1,    0,      10 },
                { L_,   "z",              36,   1,    0,      35 },
                { L_,   "Z",              36,   1,    0,      35 },

                { L_,   "++",             10,   1,    1,       0 },
                { L_,   "+-",             10,   1,    1,       0 },
                { L_,   "-+",             10,   1,    1,       0 },
                { L_,   "--",             10,   1,    1,       0 },
                { L_,   "+a",             10,   1,    1,       0 },
                { L_,   "-a",             10,   1,    1,       0 },
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
                { L_,   "+g",             16,   1,    1,       0 },
                { L_,   "-g",             16,   1,    1,       0 },
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
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         BASE   = DATA[ti].d_base;
                const ptrdiff_t   NUM    = DATA[ti].d_offset;
                const int         FAIL   = DATA[ti].d_fail;
                const int         VALUE  = DATA[ti].d_value;
                const int         curLen = strlen(SPEC);

                if (veryVerbose) {
                    P(LINE);
                    P(SPEC);
                    P(BASE);
                }

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton strings of length "
                                          << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                {  // test with first initial value
                    int       result = INITIAL_VALUE_1;
                    StringRef rest;
                    int       rv = NumericParseUtil::
                                          parseInt(&result, &rest, SPEC, BASE);
                    LOOP_ASSERT(LINE, NUM == rest.data() - SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    int       result = INITIAL_VALUE_2;
                    StringRef rest;
                    int       rv = NumericParseUtil::
                                          parseInt(&result, &rest, SPEC, BASE);
                    LOOP_ASSERT(LINE, NUM == rest.data() - SPEC);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_2 : VALUE));
                }

                // Test without the 'remainder' argument

                {  // test with first initial value
                    int       result = INITIAL_VALUE_1;
                    int       rv = NumericParseUtil::
                                                 parseInt(&result, SPEC, BASE);
                    LOOP_ASSERT(LINE, FAIL == !!rv);
                    LOOP_ASSERT(LINE,result == (rv ? INITIAL_VALUE_1 : VALUE));
                }

                {  // test with second initial value
                    int       result = INITIAL_VALUE_2;
                    int       rv = NumericParseUtil::
                                                 parseInt(&result, SPEC, BASE);
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
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //:    4 "Interesting" values from "A Program for Testing IEEE
        //:      Decimal-Binary Conversions", Vern Paxson, ICIR 1991.
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseDouble(double *res, StringRef *rest, StringRef in)
        //   parseDouble(double *res, StringRef in)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE DOUBLE" << endl
                          << "====================" << endl;

        {
            const double INITIAL_VALUE_1 =  37.0;  // first initial value
            const double INITIAL_VALUE_2 = -58.0;  // second initial value

#if defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900
            const double inf = bsl::numeric_limits<double>::infinity();
            const double NaN = bsl::numeric_limits<double>::quiet_NaN();
#endif

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                ptrdiff_t   d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
            } DATA[] = {
                //line  spec                                   offset  fail
                //----  -------------------------------------  ------  ----
                { L_,   "",                                        0,    1},

                { L_,   "+",                                       0,    1},
                { L_,   "-",                                       0,    1},
                { L_,   ".",                                       0,    1},
                { L_,   "a",                                       0,    1},
                { L_,   "0",                                       1,    0},
                { L_,   "1",                                       1,    0},
                { L_,   "9",                                       1,    0},
                { L_,   "e",                                       0,    1},
                { L_,   "E",                                       0,    1},

                { L_,   "++",                                      0,    1},
                { L_,   "+-",                                      0,    1},
                { L_,   "-+",                                      0,    1},
                { L_,   "--",                                      0,    1},
                { L_,   "+1",                                      2,    0},
                { L_,   "-1",                                      2,    0},
                { L_,   "10",                                      2,    0},
                { L_,   "90",                                      2,    0},
                { L_,   ".0",                                      2,    0},
                { L_,   ".1",                                      2,    0},
                { L_,   ".9",                                      2,    0},
                { L_,   "+e",                                      0,    1},
                { L_,   "-e",                                      0,    1},
                { L_,   "+E",                                      0,    1},
                { L_,   "-E",                                      0,    1},
                { L_,   "0e",                                      1,    0},
                { L_,   "0E",                                      1,    0},

                { L_,   "-10",                                     3,    0},
                { L_,   "-90",                                     3,    0},
                { L_,   "-.0",                                     3,    0},
                { L_,   "-.1",                                     3,    0},
                { L_,   "-.9",                                     3,    0},
                { L_,   "0ee",                                     1,    0},
                { L_,   "0eE",                                     1,    0},
                { L_,   "0Ee",                                     1,    0},
                { L_,   "0EE",                                     1,    0},
                { L_,   "0e0",                                     3,    0},
                { L_,   "0e1",                                     3,    0},
                { L_,   "1e0",                                     3,    0},
                { L_,   "1e1",                                     3,    0},
                { L_,   "1e2",                                     3,    0},
                { L_,   "0E0",                                     3,    0},
                { L_,   "0E1",                                     3,    0},
                { L_,   "1E0",                                     3,    0},
                { L_,   "1E1",                                     3,    0},
                { L_,   "1E2",                                     3,    0},

                { L_,   "1.23E2E",                                 6,    0},
                { L_,   "123x",                                    3,    0},
                { L_,   "1x",                                      1,    0},

                { L_,    ".25",                                    3,    0},
                { L_,   "0.25",                                    4,    0},
                { L_,  "-0.25",                                    5,    0},
                { L_,  "00.25",                                    5,    0},
                { L_, "-00.25",                                    6,    0},
                { L_,   "0.125",                                   5,    0},
                { L_,  "-0.125",                                   6,    0},
                { L_,   "0.0625",                                  6,    0},
                { L_,  "-0.0625",                                  7,    0},
                { L_,   "0.3125",                                  6,    0},
                { L_,  "-0.3125",                                  7,    0},

                { L_,   "100",                                     3,    0},
                { L_,   "10000",                                   5,    0},
                { L_,   "1000000",                                 7,    0},
                { L_,   "100000000",                               9,    0},
                { L_,   "10000000000",                            11,    0},
                { L_,   "1000000000000",                          13,    0},
                { L_,   "100000000000000",                        15,    0},
                { L_,   "10000000000000000",                      17,    0},
                { L_,   "1000000000000000000",                    19,    0},
                { L_,   "100000000000000000000",                  21,    0},
                { L_,   "10000000000000000000000",                23,    0},
                { L_,   "1000000000000000000000000",              25,    0},
                { L_,   "100000000000000000000000000",            27,    0},
                { L_,   "10000000000000000000000000000",          29,    0},
                { L_,   "1000000000000000000000000000000",        31,    0},
                { L_,   "100000000000000000000000000000000",      33,    0},
                { L_,   "10000000000000000000000000000000000",    35,    0},
                { L_,   "1000000000000000000000000000000000000",  37,    0},

                { L_,   "-100",                                    4,    0},
                { L_,   "-10000",                                  6,    0},
                { L_,   "-1000000",                                8,    0},
                { L_,   "-100000000",                             10,    0},
                { L_,   "-10000000000",                           12,    0},
                { L_,   "-1000000000000",                         14,    0},
                { L_,   "-100000000000000",                       16,    0},
                { L_,   "-10000000000000000",                     18,    0},
                { L_,   "-1000000000000000000",                   20,    0},
                { L_,   "-100000000000000000000",                 22,    0},
                { L_,   "-10000000000000000000000",               24,    0},
                { L_,   "-1000000000000000000000000",             26,    0},
                { L_,   "-100000000000000000000000000",           28,    0},
                { L_,   "-10000000000000000000000000000",         30,    0},
                { L_,   "-1000000000000000000000000000000",       32,    0},
                { L_,   "-100000000000000000000000000000000",     34,    0},
                { L_,   "-10000000000000000000000000000000000",   36,    0},
                { L_,   "-1000000000000000000000000000000000000", 38,    0},

                { L_,   "1e500",                                   5,    0},
                { L_,   "1e-500",                                  6,    0},

                { L_,   "1e1000000000",                           12,    0},
                { L_,   "1e2147483647",                           12,    0},
                { L_,   "1e-2147483647",                          13,    0},
                { L_,   "1e-2147483648",                          13,    0},

                { L_,   "12345678000000000000",                   20,    0},

                // Boundary-value cases for 64-bit double.
                { L_,   "1e308",                                   5,    0},
                { L_,   "1.7e308",                                 7,    0},
                { L_,   "1.8e308",                                 7,    0},
                { L_,   "-1e308",                                  6,    0},
                { L_,   "-1.7e308",                                8,    0},
                { L_,   "-1.8e308",                                8,    0},

                // Stress tests for converting decimal to 64-bit double. See
                // "A Program for Testing IEEE Decimal-Binary Conversions",
                // Vern Paxson, ICIR 1991.
                { L_,   "5e125",                                   5,    0},
                { L_,   "6.9e268",                                 7,    0},
                { L_,   "9.99e-24",                                8,    0},
                { L_,   "7.861e-31",                               9,    0},
                { L_,   "7.5569e-250",                            11,    0},
                { L_,   "9.28609e-256",                           12,    0},
                { L_,   "9.210917e86",                            11,    0},
                { L_,   "8.4863171e121",                          13,    0},
                { L_,   "6.53777767e281",                         14,    0},
                { L_,   "5.232604057e-289",                       16,    0},
                { L_,   "2.7235667517e-99",                       16,    0},
                { L_,   "6.53532977297e-112",                     18,    0},
                { L_,   "3.142213164987e-282",                    19,    0},
                { L_,   "4.6202199371337e-59",                    19,    0},
                { L_,   "2.31010996856685e-59",                   20,    0},
                { L_,   "9.324754620109615e227",                  21,    0},
                { L_,   "7.8459735791271921e65",                  21,    0},
                { L_,   "2.72104041512242479e217",                23,    0},
                { L_,   "6.802601037806061975e216",               24,    0},
                { L_,   "2.0505426358836677347e-202",             26,    0},
                { L_,   "8.36168422905420598437e-214",            27,    0},
                { L_,   "4.891559871276714924261e243",            27,    0},
                // TBD more vectors

                // Test of the special values NaN and Infinity
                { L_,   "inf",                                     3,    0},
                { L_,   "-inf",                                    4,    0},
                { L_,   "INF",                                     3,    0},
                { L_,   "-INF",                                    4,    0},
                { L_,   "Inf",                                     3,    0},
                { L_,   "-Inf",                                    4,    0},
                { L_,   "InF",                                     3,    0},
                { L_,   "-InF",                                    4,    0},

                { L_,   "infinity",                                8,    0},
                { L_,   "-infinity",                               9,    0},
                { L_,   "INFINITY",                                8,    0},
                { L_,   "-INFINITY",                               9,    0},
                { L_,   "Infinity",                                8,    0},
                { L_,   "-Infinity",                               9,    0},
                { L_,   "InFiNiTy",                                8,    0},
                { L_,   "-InFiNiTy",                               9,    0},

                { L_,   "nan",                                     3,    0},
                { L_,   "-nan",                                    4,    0},
                { L_,   "NAN",                                     3,    0},
                { L_,   "-NAN",                                    4,    0},
                { L_,   "Nan",                                     3,    0},
                { L_,   "-Nan",                                    4,    0},
                { L_,   "NaN",                                     3,    0},
                { L_,   "-NaN",                                    4,    0},

                { L_,   "nan()",                                   5,    0},
                { L_,   "-nan()",                                  6,    0},
                { L_,   "NAN()",                                   5,    0},
                { L_,   "-NAN()",                                  6,    0},
                { L_,   "Nan()",                                   5,    0},
                { L_,   "-Nan()",                                  6,    0},
                { L_,   "NaN()",                                   5,    0},
                { L_,   "-NaN()",                                  6,    0},

                { L_,   "nan(ananana_batmaaan)",                  21,    0},
                { L_,   "-nan(ananana_batmaaan)",                 22,    0},
                { L_,   "NAN(ananana_batmaaan)",                  21,    0},
                { L_,   "-NAN(ananana_batmaaan)",                 22,    0},
                { L_,   "Nan(ananana_batmaaan)",                  21,    0},
                { L_,   "-Nan(ananana_batmaaan)",                 22,    0},
                { L_,   "NaN(ananana_batmaaan)",                  21,    0},
                { L_,   "-NaN(ananana_batmaaan)",                 22,    0},
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SZ = 64;  // maximum length of an input string + 2
            char      buffer[SZ];

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const char *const SPEC = DATA[ti].d_spec_p;
                const ptrdiff_t   NUM  = DATA[ti].d_offset;
                const int         FAIL = DATA[ti].d_fail;

                if (veryVerbose) {
                    P(LINE);
                    P(SPEC);
                }

                double VALUE;
#ifdef BSLS_PLATFORM_OS_AIX

                // [s]scanf is broken in the AIX standard C library.  It fails
                // to scan strings like "123e", therefore breaking the tests.
                // This code here removes trailing 'e' and 'E' characters to
                // fix the test.

                bsl::string spec(SPEC, &testAllocator);
                while (!spec.empty() &&
                       (spec.back() == 'e' || spec.back() == 'E')) {
                    spec.resize(spec.size() - 1);
                }
                sscanf(spec.c_str(), "%lf", &VALUE);
#elif defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION < 1900

                // [s]scanf is broken in the MSVC standard C library up to
                // including Visual Studio 2013 (fixed in 2015 and later).  It
                // fails to scan strings that contain the "special" values of
                // 'double': infinity and NaN.

                if (bslstl::StringRef("inf") == SPEC) {
                    VALUE = inf;
                }
                else if (bslstl::StringRef("-inf") == SPEC) {
                    VALUE = -inf;
                }
                else if (bslstl::StringRef("INF") == SPEC) {
                    VALUE = inf;
                }
                else if (bslstl::StringRef("-INF") == SPEC) {
                    VALUE = -inf;
                }
                else if (bslstl::StringRef("Inf") == SPEC) {
                    VALUE = inf;
                }
                else if (bslstl::StringRef("-Inf") == SPEC) {
                    VALUE = -inf;
                }
                else if (bslstl::StringRef("InF") == SPEC) {
                    VALUE = inf;
                }
                else if (bslstl::StringRef("-InF") == SPEC) {
                    VALUE = -inf;
                }
                else if (bslstl::StringRef("infinity") == SPEC) {
                    VALUE = inf;
                }
                else if (bslstl::StringRef("-infinity") == SPEC) {
                    VALUE = -inf;
                }
                else if (bslstl::StringRef("INFINITY") == SPEC) {
                    VALUE = inf;
                }
                else if (bslstl::StringRef("-INFINITY") == SPEC) {
                    VALUE = -inf;
                }
                else if (bslstl::StringRef("Infinity") == SPEC) {
                    VALUE = inf;
                }
                else if (bslstl::StringRef("-Infinity") == SPEC) {
                    VALUE = -inf;
                }
                else if (bslstl::StringRef("InFiNiTy") == SPEC) {
                    VALUE = inf;
                }
                else if (bslstl::StringRef("-InFiNiTy") == SPEC) {
                    VALUE = -inf;
                }
                else if (bslstl::StringRef("nan") == SPEC) {
                    VALUE = NaN;
                }
                else if (bslstl::StringRef("-nan") == SPEC) {
                    VALUE = -NaN;
                }
                else if (bslstl::StringRef("NAN") == SPEC) {
                    VALUE = NaN;
                }
                else if (bslstl::StringRef("-NAN") == SPEC) {
                    VALUE = -NaN;
                }
                else if (bslstl::StringRef("Nan") == SPEC) {
                    VALUE = NaN;
                }
                else if (bslstl::StringRef("-Nan") == SPEC) {
                    VALUE = -NaN;
                }
                else if (bslstl::StringRef("NaN") == SPEC) {
                    VALUE = NaN;
                }
                else if (bslstl::StringRef("-NaN") == SPEC) {
                    VALUE = -NaN;
                }
                else if (bslstl::StringRef("nan()") == SPEC) {
                    VALUE = NaN;
                }
                else if (bslstl::StringRef("-nan()") == SPEC) {
                    VALUE = -NaN;
                }
                else if (bslstl::StringRef("NAN()") == SPEC) {
                    VALUE = NaN;
                }
                else if (bslstl::StringRef("-NAN()") == SPEC) {
                    VALUE = -NaN;
                }
                else if (bslstl::StringRef("Nan()") == SPEC) {
                    VALUE = NaN;
                }
                else if (bslstl::StringRef("-Nan()") == SPEC) {
                    VALUE = -NaN;
                }
                else if (bslstl::StringRef("NaN()") == SPEC) {
                    VALUE = NaN;
                }
                else if (bslstl::StringRef("-NaN()") == SPEC) {
                    VALUE = -NaN;
                }
                else if (bslstl::StringRef("nan(ananana_batmaaan)") == SPEC) {
                    VALUE = NaN;
                }
                else if (bslstl::StringRef("-nan(ananana_batmaaan)") == SPEC) {
                    VALUE = -NaN;
                }
                else if (bslstl::StringRef("NAN(ananana_batmaaan)") == SPEC) {
                    VALUE = NaN;
                }
                else if (bslstl::StringRef("-NAN(ananana_batmaaan)") == SPEC) {
                    VALUE = -NaN;
                }
                else if (bslstl::StringRef("Nan(ananana_batmaaan)") == SPEC) {
                    VALUE = NaN;
                }
                else if (bslstl::StringRef("-Nan(ananana_batmaaan)") == SPEC) {
                    VALUE = -NaN;
                }
                else if (bslstl::StringRef("NaN(ananana_batmaaan)") == SPEC) {
                    VALUE = NaN;
                }
                else if (bslstl::StringRef("-NaN(ananana_batmaaan)") == SPEC) {
                    VALUE = -NaN;
                }
                else {
                    sscanf(SPEC, "%lf", &VALUE);
                }
#else
                sscanf(SPEC, "%lf", &VALUE);
#endif
                const int curLen = strlen(SPEC);

                LOOP_ASSERT(LINE, curLen + 2 < SZ);
                                 // orthogonal perturbation will fit in buffer

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton strings of length "
                                          << curLen << ':' << endl;
                    //LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // orthogonal perturbation over terminating whitespace
                for (int si = 0; si < 2; ++si) {
                    strcpy(buffer, SPEC);
                    if (veryVerbose) {
                        if (si) {
                            cout << "immediate terminating zero" << endl;
                        }
                        else {
                            cout << "terminating whitespace" << endl;
                        }
                    }
                    if (si) {
                        buffer[curLen] = ' ';
                        buffer[curLen + 1] = '\0';
                    }

                    {  // test with first initial value
                        double    result = INITIAL_VALUE_1;
                        StringRef rest;
                        int       rv = NumericParseUtil::parseDouble(&result,
                                                                     &rest,
                                                                     buffer);
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
                        LOOP4_ASSERT(LINE, si,
                                     NUM, rest.data() - buffer,
                                     NUM == rest.data() - buffer);
                        LOOP4_ASSERT(LINE, si, FAIL, rv, FAIL == !!rv);
                        const bool isNaN = VALUE != VALUE;
                        if (!isNaN) {
                            LOOP6_ASSERT(LINE, si,
                                     result, rv, INITIAL_VALUE_1, VALUE,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                        }
                        else {
                            LOOP2_ASSERT(LINE, si, result != result);
                            LOOP3_ASSERT(LINE, si, rv, 0 == rv);

                            int resultSign = doubleSign(result);
                            int valueSign  = doubleSign(VALUE);

                            LOOP4_ASSERT(LINE, si, resultSign, valueSign,
                                         resultSign == valueSign);
                        }
                    }

                    {  // test with second initial value
                        double    result = INITIAL_VALUE_2;
                        StringRef rest;
                        int       rv = NumericParseUtil::parseDouble(&result,
                                                                     &rest,
                                                                     buffer);
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

                        LOOP2_ASSERT(LINE, si, NUM == rest.data() - buffer);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        const bool isNaN = VALUE != VALUE;
                        if (!isNaN) {
                            LOOP6_ASSERT(LINE, si,
                                result, rv, INITIAL_VALUE_2, VALUE,
                                result == (rv ? INITIAL_VALUE_2 : VALUE));
                        }
                        else {
                            LOOP2_ASSERT(LINE, si, result != result);
                            LOOP3_ASSERT(LINE, si, rv, 0 == rv);

                            int resultSign = doubleSign(result);
                            int valueSign = doubleSign(VALUE);
                            LOOP4_ASSERT(LINE, si, resultSign, valueSign,
                                resultSign == valueSign);
                        }
                    }
                } // end for si....
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING PARSE SIGNED INTEGER
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseSignedInteger(result, rest, input, base, minVal, maxVal)
        //   parseSignedInteger(result, input, base, minVal, maxVal)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE SIGNED INTEGER" << endl
                          << "============================" << endl;

        {
            typedef bsls::Types::Int64 Int64;

            const Int64 INITIAL_VALUE_1 =  37;  // first initial value
            const Int64 INITIAL_VALUE_2 = -58;  // second initial value

            const Int64 int64Min = bsl::numeric_limits<Int64>::min();
            const Int64 int64Max = bsl::numeric_limits<Int64>::max();

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                Int64       d_min;      // specification min
                Int64       d_max;      // specification max
                ptrdiff_t   d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                Int64       d_value;    // expected return value
            } DATA[] = {
                //line  spec       base   min      max    off  fail  value
                //----  ---------- ---- -------- -------- ---  ----  -------
                { L_,   "",         10,  -32768,   32767,  0,    1,        0 },

                { L_,   "a",        10,  -32768,   32767,  0,    1,        0 },
                { L_,   "+",        10,  -32768,   32767,  1,    1,        0 },
                { L_,   "-",        10,  -32768,   32767,  1,    1,        0 },
                { L_,   "0",        10,  -32768,   32767,  1,    0,        0 },
                { L_,   "a",        16,  -32768,   32767,  1,    0,       10 },
                { L_,   "f",        16,  -32768,   32767,  1,    0,       15 },
                { L_,   "A",        16,  -32768,   32767,  1,    0,       10 },
                { L_,   "F",        16,  -32768,   32767,  1,    0,       15 },
                { L_,   "g",        16,  -32768,   32767,  0,    1,        0 },
                { L_,   "0",         2,  -32768,   32767,  1,    0,        0 },
                { L_,   "1",         2,  -32768,   32767,  1,    0,        1 },
                { L_,   "a",        11,  -32768,   32767,  1,    0,       10 },
                { L_,   "A",        11,  -32768,   32767,  1,    0,       10 },
                { L_,   "z",        36,  -32768,   32767,  1,    0,       35 },
                { L_,   "Z",        36,  -32768,   32767,  1,    0,       35 },

                { L_,   "++",       10,  -32768,   32767,  1,    1,        0 },
                { L_,   "+-",       10,  -32768,   32767,  1,    1,        0 },
                { L_,   "-+",       10,  -32768,   32767,  1,    1,        0 },
                { L_,   "--",       10,  -32768,   32767,  1,    1,        0 },
                { L_,   "+a",       10,  -32768,   32767,  1,    1,        0 },
                { L_,   "-a",       10,  -32768,   32767,  1,    1,        0 },
                { L_,   "+0",       10,  -32768,   32767,  2,    0,        0 },
                { L_,   "+9",       10,  -32768,   32767,  2,    0,        9 },
                { L_,   "-0",       10,  -32768,   32767,  2,    0,        0 },
                { L_,   "-9",       10,  -32768,   32767,  2,    0,       -9 },
                { L_,   "0a",       10,  -32768,   32767,  1,    0,        0 },
                { L_,   "9a",       10,  -32768,   32767,  1,    0,        9 },
                { L_,   "00",       10,  -32768,   32767,  2,    0,        0 },
                { L_,   "01",       10,  -32768,   32767,  2,    0,        1 },
                { L_,   "19",       10,  -32768,   32767,  2,    0,       19 },
                { L_,   "99",       10,  -32768,   32767,  2,    0,       99 },
                { L_,   "+g",       16,  -32768,   32767,  1,    1,        0 },
                { L_,   "-g",       16,  -32768,   32767,  1,    1,        0 },
                { L_,   "+a",       16,  -32768,   32767,  2,    0,       10 },
                { L_,   "+f",       16,  -32768,   32767,  2,    0,       15 },
                { L_,   "-a",       16,  -32768,   32767,  2,    0,      -10 },
                { L_,   "-f",       16,  -32768,   32767,  2,    0,      -15 },
                { L_,   "ff",       16,  -32768,   32767,  2,    0,      255 },
                { L_,   "FF",       16,  -32768,   32767,  2,    0,      255 },
                { L_,   "+0",        2,  -32768,   32767,  2,    0,        0 },
                { L_,   "-0",        2,  -32768,   32767,  2,    0,        0 },
                { L_,   "+1",        2,  -32768,   32767,  2,    0,        1 },
                { L_,   "-1",        2,  -32768,   32767,  2,    0,       -1 },
                { L_,   "00",        2,  -32768,   32767,  2,    0,        0 },
                { L_,   "01",        2,  -32768,   32767,  2,    0,        1 },
                { L_,   "10",        2,  -32768,   32767,  2,    0,        2 },
                { L_,   "11",        2,  -32768,   32767,  2,    0,        3 },
                { L_,   "+z",       36,  -32768,   32767,  2,    0,       35 },
                { L_,   "-z",       36,  -32768,   32767,  2,    0,      -35 },
                { L_,   "0z",       36,  -32768,   32767,  2,    0,       35 },
                { L_,   "0Z",       36,  -32768,   32767,  2,    0,       35 },
                { L_,   "10",       36,  -32768,   32767,  2,    0,       36 },
                { L_,   "z0",       36,  -32768,   32767,  2,    0,     1260 },
                { L_,   "Z0",       36,  -32768,   32767,  2,    0,     1260 },

                { L_,   "+0a",      10,  -32768,   32767,  2,    0,        0 },
                { L_,   "+9a",      10,  -32768,   32767,  2,    0,        9 },
                { L_,   "-0a",      10,  -32768,   32767,  2,    0,        0 },
                { L_,   "-9a",      10,  -32768,   32767,  2,    0,       -9 },
                { L_,   "+12",      10,  -32768,   32767,  3,    0,       12 },
                { L_,   "+89",      10,  -32768,   32767,  3,    0,       89 },
                { L_,   "-12",      10,  -32768,   32767,  3,    0,      -12 },
                { L_,   "123",      10,  -32768,   32767,  3,    0,      123 },
                { L_,   "789",      10,  -32768,   32767,  3,    0,      789 },
                { L_,   "+fg",      16,  -32768,   32767,  2,    0,       15 },
                { L_,   "-fg",      16,  -32768,   32767,  2,    0,      -15 },
                { L_,   "+ff",      16,  -32768,   32767,  3,    0,      255 },
                { L_,   "+FF",      16,  -32768,   32767,  3,    0,      255 },
                { L_,   "-ff",      16,  -32768,   32767,  3,    0,     -255 },
                { L_,   "-FF",      16,  -32768,   32767,  3,    0,     -255 },
                { L_,   "fff",      16,  -32768,   32767,  3,    0,     4095 },
                { L_,   "fFf",      16,  -32768,   32767,  3,    0,     4095 },
                { L_,   "FfF",      16,  -32768,   32767,  3,    0,     4095 },
                { L_,   "FFF",      16,  -32768,   32767,  3,    0,     4095 },

                { L_,   "1234",     10,  -32768,   32767,  4,    0,     1234 },
                { L_,   "-123",     10,  -32768,   32767,  4,    0,     -123 },
                { L_,   "7FFF",     16,  -32768,   32767,  4,    0,    32767 },

                { L_,   "12345",    10,  -32768,   32767,  5,    0,    12345 },
                { L_,   "-1234",    10,  -32768,   32767,  5,    0,    -1234 },
                { L_,   "-7FFF",    16,  -32768,   32767,  5,    0,   -32767 },
                { L_,   "32766",    10,  -32768,   32767,  5,    0,    32766 },
                { L_,   "32767",    10,  -32768,   32767,  5,    0,    32767 },
                { L_,   "32768",    10,  -32768,   32767,  4,    0,     3276 },
                { L_,   "32769",    10,  -32768,   32767,  4,    0,     3276 },
                { L_,   "-8000",    16,  -32768,   32767,  5,    0,   -32768 },
                { L_,   "32768",    10,  -32768,   32767,  4,    0,     3276 },
                { L_,   "32768",    10,
                             -2147483648LL, 2147483647LL,  5,    0,    32768 },
                { L_,   "32769",    10,
                             -2147483648LL, 2147483647LL,  5,    0,    32769 },

                { L_,   "123456",   10,  -32768,   32767,  5,    0,    12345 },
                { L_,   "-32766",   10,  -32768,   32767,  6,    0,   -32766 },
                { L_,   "-32767",   10,  -32768,   32767,  6,    0,   -32767 },
                { L_,   "-32768",   10,  -32768,   32767,  6,    0,   -32768 },
                { L_,   "-32769",   10,  -32768,   32767,  5,    0,    -3276 },

                { L_,   "-123456",  10,  -32768,   32767,  6,    0,   -12345 },

                { L_,   "2147483647", 10,
                             -2147483648LL, 2147483647LL, 10,    0,
                                                                  2147483647 },
                { L_,   "2147483648", 10,
                             -2147483648LL, 2147483647LL,  9,    0,
                                                                   214748364 },
                { L_,   "2147483649", 10,
                             -2147483648LL, 2147483647LL,  9,    0,
                                                                   214748364 },
                { L_,   "2147483648", 10,
                                  int64Min,     int64Max, 10,    0,
                                                                2147483648LL },
                { L_,   "2147483649", 10,
                  bsl::numeric_limits<Int64>::min(), 0x7FFFFFFFFFFFFFFFLL,
                                                      10,    0, 2147483649LL },

                { L_,   "-2147483647", 10,
                             -2147483648LL, 2147483647LL, 11,    0,
                                                                 -2147483647 },
                { L_,   "-2147483648", 10,
                             -2147483648LL, 2147483647LL, 11,    0,
                                                               -2147483648LL },
                { L_,   "-2147483649", 10,
                             -2147483648LL, 2147483647LL, 10,    0,
                                                                  -214748364 },
                { L_,   "-2147483648", 10,
                                  int64Min, int64Max,     11,    0,
                                                               -2147483648LL },
                { L_,   "-2147483649", 10,
                                  int64Min, int64Max,     11,    0,
                                                               -2147483649LL },

                { L_,   "7fffffffffffffff", 16,
                                  int64Min, int64Max,     16,   0,
                                                        0x7FFFFFFFFFFFFFFFLL },
                { L_,   "-8000000000000000", 16,
                                  int64Min, int64Max,     17,   0,  int64Min },
                { L_,   "-8000000000000001", 16,
                                  int64Min, int64Max,     16,   0,
                                                         -0x800000000000000LL},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SZ = 32;  // maximum length of an input string + 2
            char      buffer[SZ];

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         BASE   = DATA[ti].d_base;
                const Int64       MIN    = DATA[ti].d_min;
                const Int64       MAX    = DATA[ti].d_max;
                const ptrdiff_t   NUM    = DATA[ti].d_offset;
                const int         FAIL   = DATA[ti].d_fail;
                const Int64       VALUE  = DATA[ti].d_value;
                const int         curLen = bsl::strlen(SPEC);

                if (veryVerbose) {
                    P(LINE);
                    P(SPEC);
                    P(BASE);
                }

                LOOP_ASSERT(LINE, curLen + 2 < SZ);
                                 // orthogonal perturbation will fit in buffer

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton strings of length "
                                          << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // orthogonal perturbation over terminating whitespace
                for (int si = 0; si < 2; ++si) {
                    strcpy(buffer, SPEC);
                    if (veryVerbose) {
                        if (si) {
                            cout << "immediate terminating zero" << endl;
                        }
                        else {
                            cout << "terminating whitespace" << endl;
                        }
                    }
                    if (si) {
                        buffer[curLen] = ' ';
                        buffer[curLen + 1] = '\0';
                    }

                    {  // test with first initial value
                        Int64     result = INITIAL_VALUE_1;
                        StringRef rest;
                        int       rv = NumericParseUtil::
                                                    parseSignedInteger(&result,
                                                                       &rest,
                                                                       buffer,
                                                                       BASE,
                                                                       MIN,
                                                                       MAX);
                        LOOP2_ASSERT(LINE, si, NUM == rest.data() - buffer);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        if (veryVeryVerbose) {
                            P(result);
                            P(rv);
                            P(VALUE);
                            P(INITIAL_VALUE_1);
                        }
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                    }

                    {  // test with second initial value
                        Int64     result = INITIAL_VALUE_2;
                        StringRef rest;
                        int       rv = NumericParseUtil::
                                                    parseSignedInteger(&result,
                                                                       &rest,
                                                                       buffer,
                                                                       BASE,
                                                                       MIN,
                                                                       MAX);
                        LOOP2_ASSERT(LINE, si, NUM == rest.data() - buffer);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                   result == (rv ? INITIAL_VALUE_2 : VALUE));
                    }

                    // Test without the 'remainder' argument

                    {  // test with first initial value
                        Int64     result = INITIAL_VALUE_1;
                        int       rv = NumericParseUtil::
                                                    parseSignedInteger(&result,
                                                                       buffer,
                                                                       BASE,
                                                                       MIN,
                                                                       MAX);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        if (veryVeryVerbose) {
                            P(result);
                            P(rv);
                            P(VALUE);
                            P(INITIAL_VALUE_1);
                        }
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                    }

                    {  // test with second initial value
                        Int64     result = INITIAL_VALUE_2;
                        int       rv = NumericParseUtil::
                                                    parseSignedInteger(&result,
                                                                       buffer,
                                                                       BASE,
                                                                       MIN,
                                                                       MAX);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                   result == (rv ? INITIAL_VALUE_2 : VALUE));
                    }
                }
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PARSE UNSIGNED INTEGER
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //:
        //:  3 Corner cases work as expected
        //:
        //:    1 The string ends unexpectedly
        //:    2 The value is as large or small as representable
        //:    3 The value is just large/small than representable
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   parseUnsignedInteger(result, rest, in, base, maxVal)
        //   parseUnsignedInteger(result, rest, in, base, maxVal, maxDigit)
        //   parseUnsignedInteger(result, in, base, maxVal)
        //   parseUnsignedInteger(result, in, base, maxVal, maxDigit)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PARSE UNSIGNED INTEGER" << endl
                          << "==============================" << endl;

        typedef bsls::Types::Uint64 Uint64;

        if (verbose) cout << "\nDefault maximum number of digits." << endl;
        {
            const Uint64 INITIAL_VALUE_1 = 37;  // first initial value
            const Uint64 INITIAL_VALUE_2 = 58;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                Uint64      d_max;      // specification max
                ptrdiff_t   d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                Uint64      d_value;    // expected return value
            } DATA[] = {
                //line  spec       base   max    off  fail  value
                //----  ---------- ---- -------- ---  ----  -------
                { L_,   "",         10,   32767,  0,    1,        0 },

                { L_,   "a",        10,   32767,  0,    1,        0 },
                { L_,   "+",        10,   32767,  0,    1,        0 },
                { L_,   "-",        10,   32767,  0,    1,        0 },
                { L_,   "0",        10,   32767,  1,    0,        0 },
                { L_,   "a",        16,   32767,  1,    0,       10 },
                { L_,   "f",        16,   32767,  1,    0,       15 },
                { L_,   "A",        16,   32767,  1,    0,       10 },
                { L_,   "F",        16,   32767,  1,    0,       15 },
                { L_,   "g",        16,   32767,  0,    1,        0 },
                { L_,   "0",         2,   32767,  1,    0,        0 },
                { L_,   "1",         2,   32767,  1,    0,        1 },
                { L_,   "a",        11,   32767,  1,    0,       10 },
                { L_,   "A",        11,   32767,  1,    0,       10 },
                { L_,   "z",        36,   32767,  1,    0,       35 },
                { L_,   "Z",        36,   32767,  1,    0,       35 },

                { L_,   "+a",       10,   32767,  0,    1,        0 },
                { L_,   "-a",       10,   32767,  0,    1,        0 },
                { L_,   "+0",       10,   32767,  0,    1,        0 },
                { L_,   "+9",       10,   32767,  0,    1,        0 },
                { L_,   "-0",       10,   32767,  0,    1,        0 },
                { L_,   "-9",       10,   32767,  0,    1,        0 },
                { L_,   "0a",       10,   32767,  1,    0,        0 },
                { L_,   "9a",       10,   32767,  1,    0,        9 },
                { L_,   "00",       10,   32767,  2,    0,        0 },
                { L_,   "01",       10,   32767,  2,    0,        1 },
                { L_,   "19",       10,   32767,  2,    0,       19 },
                { L_,   "99",       10,   32767,  2,    0,       99 },
                { L_,   "ff",       16,   32767,  2,    0,      255 },
                { L_,   "FF",       16,   32767,  2,    0,      255 },
                { L_,   "00",        2,   32767,  2,    0,        0 },
                { L_,   "01",        2,   32767,  2,    0,        1 },
                { L_,   "10",        2,   32767,  2,    0,        2 },
                { L_,   "11",        2,   32767,  2,    0,        3 },
                { L_,   "0z",       36,   32767,  2,    0,       35 },
                { L_,   "0Z",       36,   32767,  2,    0,       35 },
                { L_,   "10",       36,   32767,  2,    0,       36 },
                { L_,   "z0",       36,   32767,  2,    0,     1260 },
                { L_,   "Z0",       36,   32767,  2,    0,     1260 },

                { L_,   "123",      10,   32767,  3,    0,      123 },
                { L_,   "789",      10,   32767,  3,    0,      789 },
                { L_,   "fff",      16,   32767,  3,    0,     4095 },
                { L_,   "fFf",      16,   32767,  3,    0,     4095 },
                { L_,   "FfF",      16,   32767,  3,    0,     4095 },
                { L_,   "FFF",      16,   32767,  3,    0,     4095 },

                { L_,   "1234",     10,   32767,  4,    0,     1234 },
                { L_,   "7FFF",     16,   32767,  4,    0,    32767 },

                { L_,   "12345",    10,   32767,  5,    0,    12345 },
                { L_,   "32766",    10,   32767,  5,    0,    32766 },
                { L_,   "32767",    10,   32767,  5,    0,    32767 },
                { L_,   "32768",    10,   32767,  4,    0,     3276 },
                { L_,   "32769",    10,   32767,  4,    0,     3276 },
                { L_,   "32768",    10,   32767,  4,    0,     3276 },
                { L_,   "32768",    10,
                                     2147483647,  5,    0,    32768 },
                { L_,   "32769",    10,
                                     2147483647,  5,    0,    32769 },

                { L_,   "123456",   10,   32767,  5,    0,    12345 },

                { L_,   "2147483647", 10,
                                     2147483647, 10,    0, 2147483647 },
                { L_,   "2147483648", 10,
                                     2147483647,  9,    0, 214748364 },
                { L_,   "2147483649", 10,
                                     2147483647,  9,    0, 214748364 },
                { L_,   "2147483648", 10,
                          0x7FFFFFFFFFFFFFFFuLL, 10,    0, 2147483648uLL },
                { L_,   "2147483649", 10,
                          0x7FFFFFFFFFFFFFFFuLL, 10,    0, 2147483649uLL },

                { L_,   "7fffffffffffffff", 16,
                          0x7FFFFFFFFFFFFFFFuLL, 16,    0,
                                                0x7FFFFFFFFFFFFFFFuLL },
                { L_,   "8000000000000000", 16,
                          0x7FFFFFFFFFFFFFFFuLL, 15,    0,
                                                 0x800000000000000uLL },
                { L_,   "8000000000000000", 16,
                          0x8000000000000000uLL, 16,    0,
                                                0x8000000000000000uLL },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SZ = 32;  // maximum length of an input string + 2
            char      buffer[SZ];

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE   = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         BASE   = DATA[ti].d_base;
                const Uint64      MAX    = DATA[ti].d_max;
                const ptrdiff_t   NUM    = DATA[ti].d_offset;
                const int         FAIL   = DATA[ti].d_fail;
                const Uint64      VALUE  = DATA[ti].d_value;
                const int         curLen = bsl::strlen(SPEC);

                LOOP_ASSERT(LINE, curLen + 2 < SZ);
                                 // orthogonal perturbation will fit in buffer

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton strings of length "
                                          << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // orthogonal perturbation over terminating whitespace
                for (int si = 0; si < 2; ++si) {
                    if (veryVerbose) {
                        if (si) {
                            cout << "immediate terminating zero" << endl;
                        }
                        else {
                            cout << "terminating whitespace" << endl;
                        }
                    }
                    strcpy(buffer, SPEC);
                    if (si) {
                        buffer[curLen] = ' ';
                        buffer[curLen + 1] = '\0';
                    }

                    {  // test with first initial value
                        Uint64    result = INITIAL_VALUE_1;
                        StringRef rest;
                        int       rv = NumericParseUtil::
                                                  parseUnsignedInteger(&result,
                                                                       &rest,
                                                                       buffer,
                                                                       BASE,
                                                                       MAX);
                        LOOP2_ASSERT(LINE, si, NUM == rest.data() - buffer);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                    }

                    {  // test with second initial value
                        Uint64    result = INITIAL_VALUE_2;
                        StringRef rest;
                        int       rv = NumericParseUtil::
                                                  parseUnsignedInteger(&result,
                                                                       &rest,
                                                                       buffer,
                                                                       BASE,
                                                                       MAX);
                        LOOP2_ASSERT(LINE, si, NUM == rest.data() - buffer);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                   result == (rv ? INITIAL_VALUE_2 : VALUE));
                    }

                    // Test without the 'remainder' argument

                    {  // test with first initial value
                        Uint64    result = INITIAL_VALUE_1;
                        int       rv = NumericParseUtil::
                                                  parseUnsignedInteger(&result,
                                                                       buffer,
                                                                       BASE,
                                                                       MAX);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                    }

                    {  // test with second initial value
                        Uint64    result = INITIAL_VALUE_2;
                        int       rv = NumericParseUtil::
                                                  parseUnsignedInteger(&result,
                                                                       buffer,
                                                                       BASE,
                                                                       MAX);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                   result == (rv ? INITIAL_VALUE_2 : VALUE));
                    }
                }
            }
        }


        if (verbose) cout << "\nSpecified maximum number of digits." << endl;
        {
            const Uint64 INITIAL_VALUE_1 = 37;  // first initial value
            const Uint64 INITIAL_VALUE_2 = 58;  // second initial value

            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification string
                int         d_base;     // specification base
                Uint64      d_max;      // specification max
                int         d_digit;    // specification number of digits
                ptrdiff_t   d_offset;   // expected number of parsed characters
                int         d_fail;     // parsing expected to fail indicator
                Uint64      d_value;    // expected return value
            } DATA[] = {
                //line  spec       base   max    dig off  fail  value
                //----  ---------- ---- -------- --- ---  ----  -------
                { L_,   "a",        10,   32767,  1,  0,    1,        0 },
                { L_,   "+",        10,   32767,  1,  0,    1,        0 },
                { L_,   "-",        10,   32767,  1,  0,    1,        0 },
                { L_,   "0",        10,   32767,  1,  1,    0,        0 },
                { L_,   "a",        16,   32767,  1,  1,    0,       10 },
                { L_,   "f",        16,   32767,  1,  1,    0,       15 },
                { L_,   "A",        16,   32767,  1,  1,    0,       10 },
                { L_,   "F",        16,   32767,  1,  1,    0,       15 },
                { L_,   "g",        16,   32767,  1,  0,    1,        0 },
                { L_,   "0",         2,   32767,  1,  1,    0,        0 },
                { L_,   "1",         2,   32767,  1,  1,    0,        1 },
                { L_,   "a",        11,   32767,  1,  1,    0,       10 },
                { L_,   "A",        11,   32767,  1,  1,    0,       10 },
                { L_,   "z",        36,   32767,  1,  1,    0,       35 },
                { L_,   "Z",        36,   32767,  1,  1,    0,       35 },

                { L_,   "+a",       10,   32767,  1,  0,    1,        0 },
                { L_,   "-a",       10,   32767,  1,  0,    1,        0 },
                { L_,   "+0",       10,   32767,  1,  0,    1,        0 },
                { L_,   "+9",       10,   32767,  1,  0,    1,        0 },
                { L_,   "-0",       10,   32767,  1,  0,    1,        0 },
                { L_,   "-9",       10,   32767,  1,  0,    1,        0 },
                { L_,   "0a",       10,   32767,  1,  1,    0,        0 },
                { L_,   "9a",       10,   32767,  1,  1,    0,        9 },
                { L_,   "00",       10,   32767,  1,  1,    0,        0 },
                { L_,   "01",       10,   32767,  1,  1,    0,        0 },
                { L_,   "19",       10,   32767,  1,  1,    0,        1 },
                { L_,   "99",       10,   32767,  1,  1,    0,        9 },
                { L_,   "ff",       16,   32767,  1,  1,    0,       15 },
                { L_,   "FF",       16,   32767,  1,  1,    0,       15 },
                { L_,   "00",        2,   32767,  1,  1,    0,        0 },
                { L_,   "01",        2,   32767,  1,  1,    0,        0 },
                { L_,   "10",        2,   32767,  1,  1,    0,        1 },
                { L_,   "11",        2,   32767,  1,  1,    0,        1 },
                { L_,   "0z",       36,   32767,  1,  1,    0,        0 },
                { L_,   "0Z",       36,   32767,  1,  1,    0,        0 },
                { L_,   "10",       36,   32767,  1,  1,    0,        1 },
                { L_,   "z0",       36,   32767,  1,  1,    0,       35 },
                { L_,   "Z0",       36,   32767,  1,  1,    0,       35 },

                { L_,   "+a",       10,   32767,  2,  0,    1,        0 },
                { L_,   "-a",       10,   32767,  2,  0,    1,        0 },
                { L_,   "+0",       10,   32767,  2,  0,    1,        0 },
                { L_,   "+9",       10,   32767,  2,  0,    1,        0 },
                { L_,   "-0",       10,   32767,  2,  0,    1,        0 },
                { L_,   "-9",       10,   32767,  2,  0,    1,        0 },
                { L_,   "0a",       10,   32767,  2,  1,    0,        0 },
                { L_,   "9a",       10,   32767,  2,  1,    0,        9 },
                { L_,   "00",       10,   32767,  2,  2,    0,        0 },
                { L_,   "01",       10,   32767,  2,  2,    0,        1 },
                { L_,   "19",       10,   32767,  2,  2,    0,       19 },
                { L_,   "99",       10,   32767,  2,  2,    0,       99 },
                { L_,   "ff",       16,   32767,  2,  2,    0,      255 },
                { L_,   "FF",       16,   32767,  2,  2,    0,      255 },
                { L_,   "00",        2,   32767,  2,  2,    0,        0 },
                { L_,   "01",        2,   32767,  2,  2,    0,        1 },
                { L_,   "10",        2,   32767,  2,  2,    0,        2 },
                { L_,   "11",        2,   32767,  2,  2,    0,        3 },
                { L_,   "0z",       36,   32767,  2,  2,    0,       35 },
                { L_,   "0Z",       36,   32767,  2,  2,    0,       35 },
                { L_,   "10",       36,   32767,  2,  2,    0,       36 },
                { L_,   "z0",       36,   32767,  2,  2,    0,     1260 },
                { L_,   "Z0",       36,   32767,  2,  2,    0,     1260 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;
            const int SZ = 32;  // maximum length of an input string + 2
            char      buffer[SZ];

            int oldLen = -1;
            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int         LINE         = DATA[ti].d_lineNum;
                const char *const SPEC   = DATA[ti].d_spec_p;
                const int         BASE   = DATA[ti].d_base;
                const Uint64      MAX    = DATA[ti].d_max;
                const int         DIGIT  = DATA[ti].d_digit;
                const ptrdiff_t   NUM    = DATA[ti].d_offset;
                const int         FAIL   = DATA[ti].d_fail;
                const Uint64      VALUE  = DATA[ti].d_value;
                const int         curLen = bsl::strlen(SPEC);

                if (veryVerbose) {
                    P(LINE);
                    P(SPEC);
                    P(BASE);
                }

                LOOP_ASSERT(LINE, curLen + 2 < SZ);
                                 // orthogonal perturbation will fit in buffer

                if (curLen != oldLen) {
                    if (veryVerbose) cout << "\ton strings of length "
                                          << curLen << ':' << endl;
                    LOOP_ASSERT(LINE, oldLen <= curLen);  // non-decreasing
                    oldLen = curLen;
                }

                // orthogonal perturbation over terminating whitespace
                for (int si = 0; si < 2; ++si) {
                    strcpy(buffer, SPEC);
                    if (veryVerbose) {
                        if (si) {
                            cout << "immediate terminating zero" << endl;
                        }
                        else {
                            cout << "terminating whitespace" << endl;
                        }
                    }
                    if (si) {
                        buffer[curLen] = ' ';
                        buffer[curLen + 1] = '\0';
                    }

                    {  // test with first initial value
                        Uint64    result = INITIAL_VALUE_1;
                        StringRef rest;
                        int       rv = NumericParseUtil::
                                                  parseUnsignedInteger(&result,
                                                                       &rest,
                                                                       buffer,
                                                                       BASE,
                                                                       MAX,
                                                                       DIGIT);
                        LOOP2_ASSERT(LINE, si, NUM == rest.data() - buffer);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                    }

                    {  // test with second initial value
                        Uint64    result = INITIAL_VALUE_2;
                        StringRef rest;
                        int       rv = NumericParseUtil::
                                                  parseUnsignedInteger(&result,
                                                                       &rest,
                                                                       buffer,
                                                                       BASE,
                                                                       MAX,
                                                                       DIGIT);
                        LOOP2_ASSERT(LINE, si, NUM == rest.data() - buffer);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                   result == (rv ? INITIAL_VALUE_2 : VALUE));
                    }

                    // Test without the 'remainder' argument

                    {  // test with first initial value
                        Uint64    result = INITIAL_VALUE_1;
                        int       rv = NumericParseUtil::
                                                  parseUnsignedInteger(&result,
                                                                       buffer,
                                                                       BASE,
                                                                       MAX,
                                                                       DIGIT);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                     result == (rv ? INITIAL_VALUE_1 : VALUE));
                    }

                    {  // test with second initial value
                        Uint64    result = INITIAL_VALUE_2;
                        int       rv = NumericParseUtil::
                                                  parseUnsignedInteger(&result,
                                                                       buffer,
                                                                       BASE,
                                                                       MAX,
                                                                       DIGIT);
                        LOOP2_ASSERT(LINE, si, FAIL == !!rv);
                        LOOP2_ASSERT(LINE, si,
                                   result == (rv ? INITIAL_VALUE_2 : VALUE));
                    }
                }
            }
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING CHARACTER TO DIGIT
        //
        // Concerns:
        //:  1 Correct value is returned.
        //:
        //:  2 Characters that comprise the legal set varies correctly with
        //:    'base'.
        //
        // Plan:
        //: 1 Use the table-driven approach with columns for input, base, and
        //:   expected result.  Use category partitioning to create a suite of
        //:   test vectors for an enumerated set of bases.
        //
        // Testing:
        //   characterToDigit(char character, int base)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING CHARACTER TO DIGIT" << endl
                          << "==========================" << endl;

        {
            static const struct {
                int   d_lineNum;  // source line number
                char  d_spec;     // specification character
                int   d_base;     // specification base
                short d_value;    // expected return value
            } DATA[] = {
                //line  spec  base  value
                //----  ----  ----  -----
                { L_,    '0',    2,     0 },
                { L_,    '1',    2,     1 },
                { L_,    '2',    2,    -1 },
                { L_,    '7',    2,    -1 },
                { L_,    '8',    2,    -1 },
                { L_,    '9',    2,    -1 },
                { L_,    'a',    2,    -1 },
                { L_,    'f',    2,    -1 },
                { L_,    'z',    2,    -1 },
                { L_,    'A',    2,    -1 },
                { L_,    'F',    2,    -1 },
                { L_,    'Z',    2,    -1 },
                { L_,    ' ',    2,    -1 },
                { L_,    '+',    2,    -1 },
                { L_,    '-',    2,    -1 },

                { L_,    '0',    8,     0 },
                { L_,    '1',    8,     1 },
                { L_,    '2',    8,     2 },
                { L_,    '7',    8,     7 },
                { L_,    '8',    8,    -1 },
                { L_,    '9',    8,    -1 },
                { L_,    'a',    8,    -1 },
                { L_,    'f',    8,    -1 },
                { L_,    'z',    8,    -1 },
                { L_,    'A',    8,    -1 },
                { L_,    'F',    8,    -1 },
                { L_,    'Z',    8,    -1 },
                { L_,    ' ',    8,    -1 },
                { L_,    '+',    8,    -1 },
                { L_,    '-',    8,    -1 },

                { L_,    '0',   10,     0 },
                { L_,    '1',   10,     1 },
                { L_,    '2',   10,     2 },
                { L_,    '7',   10,     7 },
                { L_,    '8',   10,     8 },
                { L_,    '9',   10,     9 },
                { L_,    'a',   10,    -1 },
                { L_,    'f',   10,    -1 },
                { L_,    'z',   10,    -1 },
                { L_,    'A',   10,    -1 },
                { L_,    'F',   10,    -1 },
                { L_,    'Z',   10,    -1 },
                { L_,    ' ',   10,    -1 },
                { L_,    '+',   10,    -1 },
                { L_,    '-',   10,    -1 },

                { L_,    '0',   16,     0 },
                { L_,    '1',   16,     1 },
                { L_,    '2',   16,     2 },
                { L_,    '7',   16,     7 },
                { L_,    '8',   16,     8 },
                { L_,    '9',   16,     9 },
                { L_,    'a',   16,    10 },
                { L_,    'f',   16,    15 },
                { L_,    'z',   16,    -1 },
                { L_,    'A',   16,    10 },
                { L_,    'F',   16,    15 },
                { L_,    'Z',   16,    -1 },
                { L_,    ' ',   16,    -1 },
                { L_,    '+',   16,    -1 },
                { L_,    '-',   16,    -1 },

                { L_,    '0',   36,     0 },
                { L_,    '1',   36,     1 },
                { L_,    '2',   36,     2 },
                { L_,    '7',   36,     7 },
                { L_,    '8',   36,     8 },
                { L_,    '9',   36,     9 },
                { L_,    'a',   36,    10 },
                { L_,    'f',   36,    15 },
                { L_,    'z',   36,    35 },
                { L_,    'A',   36,    10 },
                { L_,    'F',   36,    15 },
                { L_,    'Z',   36,    35 },
                { L_,    ' ',   36,    -1 },
                { L_,    '+',   36,    -1 },
                { L_,    '-',   36,    -1 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const char  SPEC  = DATA[ti].d_spec;
                const int   BASE  = DATA[ti].d_base;
                const short VALUE = DATA[ti].d_value;

                if (veryVerbose) {
                    P(LINE);
                    P(SPEC);
                    P(BASE);
                }

                int rv = NumericParseUtil::characterToDigit(SPEC, BASE);
                LOOP_ASSERT(LINE, VALUE == rv);
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
// Copyright 2017 Bloomberg Finance L.P.
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
