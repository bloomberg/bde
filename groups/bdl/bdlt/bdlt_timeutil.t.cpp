// bdlt_timeutil.t.cpp                                                -*-C++-*-
#include <bdlt_timeutil.h>

#include <bslim_testutil.h>

#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_iostream.h>

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a series of static member functions
// that translate between 'bdlt::Time' objects and 'int' values.
//-----------------------------------------------------------------------------
// [ 1] static bdlt::Time convertFromHHMM(int timeValue);
// [ 2] static bdlt::Time convertFromHHMMSS(int timeValue);
// [ 3] static bdlt::Time convertFromHHMMSSmmm(int timeValue);
// [ 4] static int convertToHHMM(const bdlt::Time& timeValue);
// [ 4] static int convertToHHMMSS(const bdlt::Time& timeValue);
// [ 4] static int convertToHHMMSSmmm(const bdlt::Time& timeValue);
// [ 1] static bdlt::Time convertFromHHMM(int value);
// [ 1] static bool isValidHHMM(int value);
// [ 2] static bdlt::Time convertFromHHMMSS(int value);
// [ 2] static bool isValidHHMMSS(int value);
// [ 3] static bdlt::Time convertFromHHMMSSmmm(int value);
// [ 3] static bool isValidHHMMSSmmm(int value);
//-----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE
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
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdlt::TimeUtil Util;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   This will test the usage examples provided in the component header
        //   file.
        //
        // Concerns:
        //: 1 The usage examples provided in the component header file must
        //:   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into driver, remove leading
        //:   comment characters, and replace 'assert' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING USAGE EXAMPLE\n"
                               "=====================\n";

///Usage
///------
// Following are examples illustrating basic use of this component.
//

if (verbose) bsl::cout << "\nUsage Example 1" << endl;

///Example 1
///- - - - -
// First, we demonstrate how to use 'bdlt::TimeUtil' to
// convert from an integer representation of time in "HHMMSSmmm" format to a
// 'bdlt::Time'.  Our first time will be around 3:45 pm.
//..
    //      format: HHMMSSmmm
    int timeValue = 154502789;

    bdlt::Time result = bdlt::TimeUtil::convertFromHHMMSSmmm(timeValue);

if (veryVerbose)
    bsl::cout << result << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  15:45:02.789
//..
// Then, we demonstrate a different time, 3:32:24.832 am.  Note that we do not
// lead the integer value with '0':
//..
    //  format: HHMMSSmmm
    timeValue =  33224832;      // Do not start with leading '0' as that would
                                // make the value octal and incorrect.

    result = bdlt::TimeUtil::convertFromHHMMSSmmm(timeValue);

if (veryVerbose)
    bsl::cout << result << bsl::endl;
//..
// The code above produces the following on 'stdout':
//..
//  03:32:24.832
//..
// Now, we demonstrate how 'bdlt::TimeUtil' provides methods that can be used
// to validate integral time values before passing them to the various
// "convert" methods.  For example:
//..
    ASSERT( bdlt::TimeUtil::isValidHHMMSSmmm(timeValue));
//..
// Finally, we demonstrate catching an invalid time value, 12:61:02.789 pm:
//..
    //         format: HHMMSSmmm
    int badTimeValue = 126102789;

    ASSERT(!bdlt::TimeUtil::isValidHHMMSSmmm(badTimeValue));
//..
//

if (verbose) bsl::cout << "\nUsage Example 2" << endl;

///Example 2
///- - - - -
// The following snippet of code demonstrates how to use 'bdlt::TimeUtil' to
// convert from a 'bdlt::Time' to an integer representation of time in "HHMM",
// "HHMMSS", and "HHMMSSmmm" formats:
//..
    bdlt::Time time(12, 45, 2, 789);
    int        timeHHMM      = bdlt::TimeUtil::convertToHHMM(time);
    int        timeHHMMSS    = bdlt::TimeUtil::convertToHHMMSS(time);
    int        timeHHMMSSmmm = bdlt::TimeUtil::convertToHHMMSSmmm(time);

if (veryVerbose) {
    bsl::cout << "Time in HHMM:      " << timeHHMM      << bsl::endl;
    bsl::cout << "Time in HHMMSS:    " << timeHHMMSS    << bsl::endl;
    bsl::cout << "Time in HHMMSSmmm: " << timeHHMMSSmmm << bsl::endl;
}
//..
// The code above produces the following on 'stdout':
//..
//  Time in HHMM:      1245
//  Time in HHMMSS:    124502
//  Time in HHMMSSmmm: 124502789
//..
// Note that the millisecond and/or second fields of 'bdlt::Time' are ignored
// depending on the conversion method that is called.

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING convertToHHMMSS, convertToHHMMSSmmm, convertToHHMM:
        //
        // Concerns:
        //: 1 Verify the 'Time' -> 'int' conversions are correct.
        //:   o Verify minimal values of each field the 'Time' value is created
        //:     with.
        //:   o Verify maximal values of each field the 'Time' value is created
        //:     with.
        //:   o Verify intermediate values of each field the 'Time' value is
        //:     created with.
        //:   o Verify the special value 24:00:00.000.
        //
        // Plan:
        //: 1 Create a table containing minimal, maximal, and intermediate
        //:   values for each field of a 'Time' c'tor, and each combination
        //:   thereof.
        //: 2 The table must also contain, for each possible time value:
        //:   o The corresponding HHMM 'int' value.
        //:   o The corresponding HHMMSS 'int' value.
        //:   o The corresponding HHMMSSmmm 'int' value.
        //: 3 Iterate through the table values:
        //:   o Created a 'Time' object based on the c'tor fields.
        //:   o Use the conversion methods to create integer 'hhmm', 'hhmmss',
        //:     and 'hhmmssmmm' values.
        //:   o Verify the correctness of the translated 'int' values.
        //
        // Testing:
        //   static int convertToHHMM(const bdlt::Time& timeValue);
        //   static int convertToHHMMSS(const bdlt::Time& timeValue);
        //   static int convertToHHMMSSmmm(const bdlt::Time& timeValue);
        // --------------------------------------------------------------------

        if (verbose) cout <<
              "\nTESTING convertToHHMMSS, convertToHHMMSSmmm, convertToHHMM\n"
                "==========================================================\n";

        static const struct {
            int d_lineNum;  // source line number
            int d_hr;       // input hour field
            int d_min;      // input min field
            int d_sec;      // input sec field
            int d_ms;       // input ms field
            int d_hhmm;     // expected hhmm output
            int d_hhmmss;   // expected hhmmss output
            int d_hhmmssmmm;// expected hhmmssmmm output

        } DATA[] = {
            //      <----- input ---->    <------- result ------>
            //lin   hr  min  sec    ms    HHMM  HHMMSS  HHMMSSmmm
            //---   --  ---  ---  ----    ----  ------  ---------
            { L_,    0,   0,   0,    0,      0,      0,         0 },
            { L_,    0,   0,   0,    1,      0,      0,         1 },
            { L_,    0,   0,   0,    2,      0,      0,         2 },
            { L_,    0,   0,   0,    9,      0,      0,         9 },
            { L_,    0,   0,   0,   10,      0,      0,        10 },
            { L_,    0,   0,   0,   11,      0,      0,        11 },
            { L_,    0,   0,   0,   99,      0,      0,        99 },
            { L_,    0,   0,   0,  100,      0,      0,       100 },
            { L_,    0,   0,   0,  101,      0,      0,       101 },
            { L_,    0,   0,   0,  998,      0,      0,       998 },
            { L_,    0,   0,   0,  999,      0,      0,       999 },
            { L_,    0,   0,   1,    0,      0,      1,      1000 },
            { L_,    0,   0,   1,    1,      0,      1,      1001 },
            { L_,    0,   0,   1,    2,      0,      1,      1002 },
            { L_,    0,   0,   1,    9,      0,      1,      1009 },
            { L_,    0,   0,   1,   10,      0,      1,      1010 },
            { L_,    0,   0,   1,   11,      0,      1,      1011 },
            { L_,    0,   0,   1,   99,      0,      1,      1099 },
            { L_,    0,   0,   1,  100,      0,      1,      1100 },
            { L_,    0,   0,   1,  101,      0,      1,      1101 },
            { L_,    0,   0,   1,  998,      0,      1,      1998 },
            { L_,    0,   0,   1,  999,      0,      1,      1999 },
            { L_,    0,   0,  59,    0,      0,     59,     59000 },
            { L_,    0,   0,  59,    1,      0,     59,     59001 },
            { L_,    0,   0,  59,    2,      0,     59,     59002 },
            { L_,    0,   0,  59,    9,      0,     59,     59009 },
            { L_,    0,   0,  59,   10,      0,     59,     59010 },
            { L_,    0,   0,  59,   11,      0,     59,     59011 },
            { L_,    0,   0,  59,   99,      0,     59,     59099 },
            { L_,    0,   0,  59,  100,      0,     59,     59100 },
            { L_,    0,   0,  59,  101,      0,     59,     59101 },
            { L_,    0,   0,  59,  998,      0,     59,     59998 },
            { L_,    0,   0,  59,  999,      0,     59,     59999 },
            { L_,    0,   1,   0,    0,      1,    100,    100000 },
            { L_,    0,   1,   0,    1,      1,    100,    100001 },
            { L_,    0,   1,   0,    2,      1,    100,    100002 },
            { L_,    0,   1,   0,    9,      1,    100,    100009 },
            { L_,    0,   1,   0,   10,      1,    100,    100010 },
            { L_,    0,   1,   0,   11,      1,    100,    100011 },
            { L_,    0,   1,   0,   99,      1,    100,    100099 },
            { L_,    0,   1,   0,  100,      1,    100,    100100 },
            { L_,    0,   1,   0,  101,      1,    100,    100101 },
            { L_,    0,   1,   0,  998,      1,    100,    100998 },
            { L_,    0,   1,   0,  999,      1,    100,    100999 },
            { L_,    0,   1,   1,    0,      1,    101,    101000 },
            { L_,    0,   1,   1,    1,      1,    101,    101001 },
            { L_,    0,   1,   1,    2,      1,    101,    101002 },
            { L_,    0,   1,   1,    9,      1,    101,    101009 },
            { L_,    0,   1,   1,   10,      1,    101,    101010 },
            { L_,    0,   1,   1,   11,      1,    101,    101011 },
            { L_,    0,   1,   1,   99,      1,    101,    101099 },
            { L_,    0,   1,   1,  100,      1,    101,    101100 },
            { L_,    0,   1,   1,  101,      1,    101,    101101 },
            { L_,    0,   1,   1,  998,      1,    101,    101998 },
            { L_,    0,   1,   1,  999,      1,    101,    101999 },
            { L_,    0,   1,  59,    0,      1,    159,    159000 },
            { L_,    0,   1,  59,    1,      1,    159,    159001 },
            { L_,    0,   1,  59,    2,      1,    159,    159002 },
            { L_,    0,   1,  59,    9,      1,    159,    159009 },
            { L_,    0,   1,  59,   10,      1,    159,    159010 },
            { L_,    0,   1,  59,   11,      1,    159,    159011 },
            { L_,    0,   1,  59,   99,      1,    159,    159099 },
            { L_,    0,   1,  59,  100,      1,    159,    159100 },
            { L_,    0,   1,  59,  101,      1,    159,    159101 },
            { L_,    0,   1,  59,  998,      1,    159,    159998 },
            { L_,    0,   1,  59,  999,      1,    159,    159999 },
            { L_,    0,  59,   0,    0,     59,   5900,   5900000 },
            { L_,    0,  59,   0,    1,     59,   5900,   5900001 },
            { L_,    0,  59,   0,    2,     59,   5900,   5900002 },
            { L_,    0,  59,   0,    9,     59,   5900,   5900009 },
            { L_,    0,  59,   0,   10,     59,   5900,   5900010 },
            { L_,    0,  59,   0,   11,     59,   5900,   5900011 },
            { L_,    0,  59,   0,   99,     59,   5900,   5900099 },
            { L_,    0,  59,   0,  100,     59,   5900,   5900100 },
            { L_,    0,  59,   0,  101,     59,   5900,   5900101 },
            { L_,    0,  59,   0,  998,     59,   5900,   5900998 },
            { L_,    0,  59,   0,  999,     59,   5900,   5900999 },
            { L_,    0,  59,   1,    0,     59,   5901,   5901000 },
            { L_,    0,  59,   1,    1,     59,   5901,   5901001 },
            { L_,    0,  59,   1,    2,     59,   5901,   5901002 },
            { L_,    0,  59,   1,    9,     59,   5901,   5901009 },
            { L_,    0,  59,   1,   10,     59,   5901,   5901010 },
            { L_,    0,  59,   1,   11,     59,   5901,   5901011 },
            { L_,    0,  59,   1,   99,     59,   5901,   5901099 },
            { L_,    0,  59,   1,  100,     59,   5901,   5901100 },
            { L_,    0,  59,   1,  101,     59,   5901,   5901101 },
            { L_,    0,  59,   1,  998,     59,   5901,   5901998 },
            { L_,    0,  59,   1,  999,     59,   5901,   5901999 },
            { L_,    0,  59,  59,    0,     59,   5959,   5959000 },
            { L_,    0,  59,  59,    1,     59,   5959,   5959001 },
            { L_,    0,  59,  59,    2,     59,   5959,   5959002 },
            { L_,    0,  59,  59,    9,     59,   5959,   5959009 },
            { L_,    0,  59,  59,   10,     59,   5959,   5959010 },
            { L_,    0,  59,  59,   11,     59,   5959,   5959011 },
            { L_,    0,  59,  59,   99,     59,   5959,   5959099 },
            { L_,    0,  59,  59,  100,     59,   5959,   5959100 },
            { L_,    0,  59,  59,  101,     59,   5959,   5959101 },
            { L_,    0,  59,  59,  998,     59,   5959,   5959998 },
            { L_,    0,  59,  59,  999,     59,   5959,   5959999 },
            { L_,   12,   0,   0,    0,   1200, 120000, 120000000 },
            { L_,   12,   0,   0,    1,   1200, 120000, 120000001 },
            { L_,   12,   0,   0,    2,   1200, 120000, 120000002 },
            { L_,   12,   0,   0,    9,   1200, 120000, 120000009 },
            { L_,   12,   0,   0,   10,   1200, 120000, 120000010 },
            { L_,   12,   0,   0,   11,   1200, 120000, 120000011 },
            { L_,   12,   0,   0,   99,   1200, 120000, 120000099 },
            { L_,   12,   0,   0,  100,   1200, 120000, 120000100 },
            { L_,   12,   0,   0,  101,   1200, 120000, 120000101 },
            { L_,   12,   0,   0,  998,   1200, 120000, 120000998 },
            { L_,   12,   0,   0,  999,   1200, 120000, 120000999 },
            { L_,   12,   0,   1,    0,   1200, 120001, 120001000 },
            { L_,   12,   0,   1,    1,   1200, 120001, 120001001 },
            { L_,   12,   0,   1,    2,   1200, 120001, 120001002 },
            { L_,   12,   0,   1,    9,   1200, 120001, 120001009 },
            { L_,   12,   0,   1,   10,   1200, 120001, 120001010 },
            { L_,   12,   0,   1,   11,   1200, 120001, 120001011 },
            { L_,   12,   0,   1,   99,   1200, 120001, 120001099 },
            { L_,   12,   0,   1,  100,   1200, 120001, 120001100 },
            { L_,   12,   0,   1,  101,   1200, 120001, 120001101 },
            { L_,   12,   0,   1,  998,   1200, 120001, 120001998 },
            { L_,   12,   0,   1,  999,   1200, 120001, 120001999 },
            { L_,   12,   0,  59,    0,   1200, 120059, 120059000 },
            { L_,   12,   0,  59,    1,   1200, 120059, 120059001 },
            { L_,   12,   0,  59,    2,   1200, 120059, 120059002 },
            { L_,   12,   0,  59,    9,   1200, 120059, 120059009 },
            { L_,   12,   0,  59,   10,   1200, 120059, 120059010 },
            { L_,   12,   0,  59,   11,   1200, 120059, 120059011 },
            { L_,   12,   0,  59,   99,   1200, 120059, 120059099 },
            { L_,   12,   0,  59,  100,   1200, 120059, 120059100 },
            { L_,   12,   0,  59,  101,   1200, 120059, 120059101 },
            { L_,   12,   0,  59,  998,   1200, 120059, 120059998 },
            { L_,   12,   0,  59,  999,   1200, 120059, 120059999 },
            { L_,   12,   1,   0,    0,   1201, 120100, 120100000 },
            { L_,   12,   1,   0,    1,   1201, 120100, 120100001 },
            { L_,   12,   1,   0,    2,   1201, 120100, 120100002 },
            { L_,   12,   1,   0,    9,   1201, 120100, 120100009 },
            { L_,   12,   1,   0,   10,   1201, 120100, 120100010 },
            { L_,   12,   1,   0,   11,   1201, 120100, 120100011 },
            { L_,   12,   1,   0,   99,   1201, 120100, 120100099 },
            { L_,   12,   1,   0,  100,   1201, 120100, 120100100 },
            { L_,   12,   1,   0,  101,   1201, 120100, 120100101 },
            { L_,   12,   1,   0,  998,   1201, 120100, 120100998 },
            { L_,   12,   1,   0,  999,   1201, 120100, 120100999 },
            { L_,   12,   1,   1,    0,   1201, 120101, 120101000 },
            { L_,   12,   1,   1,    1,   1201, 120101, 120101001 },
            { L_,   12,   1,   1,    2,   1201, 120101, 120101002 },
            { L_,   12,   1,   1,    9,   1201, 120101, 120101009 },
            { L_,   12,   1,   1,   10,   1201, 120101, 120101010 },
            { L_,   12,   1,   1,   11,   1201, 120101, 120101011 },
            { L_,   12,   1,   1,   99,   1201, 120101, 120101099 },
            { L_,   12,   1,   1,  100,   1201, 120101, 120101100 },
            { L_,   12,   1,   1,  101,   1201, 120101, 120101101 },
            { L_,   12,   1,   1,  998,   1201, 120101, 120101998 },
            { L_,   12,   1,   1,  999,   1201, 120101, 120101999 },
            { L_,   12,   1,  59,    0,   1201, 120159, 120159000 },
            { L_,   12,   1,  59,    1,   1201, 120159, 120159001 },
            { L_,   12,   1,  59,    2,   1201, 120159, 120159002 },
            { L_,   12,   1,  59,    9,   1201, 120159, 120159009 },
            { L_,   12,   1,  59,   10,   1201, 120159, 120159010 },
            { L_,   12,   1,  59,   11,   1201, 120159, 120159011 },
            { L_,   12,   1,  59,   99,   1201, 120159, 120159099 },
            { L_,   12,   1,  59,  100,   1201, 120159, 120159100 },
            { L_,   12,   1,  59,  101,   1201, 120159, 120159101 },
            { L_,   12,   1,  59,  998,   1201, 120159, 120159998 },
            { L_,   12,   1,  59,  999,   1201, 120159, 120159999 },
            { L_,   12,  59,   0,    0,   1259, 125900, 125900000 },
            { L_,   12,  59,   0,    1,   1259, 125900, 125900001 },
            { L_,   12,  59,   0,    2,   1259, 125900, 125900002 },
            { L_,   12,  59,   0,    9,   1259, 125900, 125900009 },
            { L_,   12,  59,   0,   10,   1259, 125900, 125900010 },
            { L_,   12,  59,   0,   11,   1259, 125900, 125900011 },
            { L_,   12,  59,   0,   99,   1259, 125900, 125900099 },
            { L_,   12,  59,   0,  100,   1259, 125900, 125900100 },
            { L_,   12,  59,   0,  101,   1259, 125900, 125900101 },
            { L_,   12,  59,   0,  998,   1259, 125900, 125900998 },
            { L_,   12,  59,   0,  999,   1259, 125900, 125900999 },
            { L_,   12,  59,   1,    0,   1259, 125901, 125901000 },
            { L_,   12,  59,   1,    1,   1259, 125901, 125901001 },
            { L_,   12,  59,   1,    2,   1259, 125901, 125901002 },
            { L_,   12,  59,   1,    9,   1259, 125901, 125901009 },
            { L_,   12,  59,   1,   10,   1259, 125901, 125901010 },
            { L_,   12,  59,   1,   11,   1259, 125901, 125901011 },
            { L_,   12,  59,   1,   99,   1259, 125901, 125901099 },
            { L_,   12,  59,   1,  100,   1259, 125901, 125901100 },
            { L_,   12,  59,   1,  101,   1259, 125901, 125901101 },
            { L_,   12,  59,   1,  998,   1259, 125901, 125901998 },
            { L_,   12,  59,   1,  999,   1259, 125901, 125901999 },
            { L_,   12,  59,  59,    0,   1259, 125959, 125959000 },
            { L_,   12,  59,  59,    1,   1259, 125959, 125959001 },
            { L_,   12,  59,  59,    2,   1259, 125959, 125959002 },
            { L_,   12,  59,  59,    9,   1259, 125959, 125959009 },
            { L_,   12,  59,  59,   10,   1259, 125959, 125959010 },
            { L_,   12,  59,  59,   11,   1259, 125959, 125959011 },
            { L_,   12,  59,  59,   99,   1259, 125959, 125959099 },
            { L_,   12,  59,  59,  100,   1259, 125959, 125959100 },
            { L_,   12,  59,  59,  101,   1259, 125959, 125959101 },
            { L_,   12,  59,  59,  998,   1259, 125959, 125959998 },
            { L_,   12,  59,  59,  999,   1259, 125959, 125959999 },
            { L_,   23,   0,   0,    0,   2300, 230000, 230000000 },
            { L_,   23,   0,   0,    1,   2300, 230000, 230000001 },
            { L_,   23,   0,   0,    2,   2300, 230000, 230000002 },
            { L_,   23,   0,   0,    9,   2300, 230000, 230000009 },
            { L_,   23,   0,   0,   10,   2300, 230000, 230000010 },
            { L_,   23,   0,   0,   11,   2300, 230000, 230000011 },
            { L_,   23,   0,   0,   99,   2300, 230000, 230000099 },
            { L_,   23,   0,   0,  100,   2300, 230000, 230000100 },
            { L_,   23,   0,   0,  101,   2300, 230000, 230000101 },
            { L_,   23,   0,   0,  998,   2300, 230000, 230000998 },
            { L_,   23,   0,   0,  999,   2300, 230000, 230000999 },
            { L_,   23,   0,   1,    0,   2300, 230001, 230001000 },
            { L_,   23,   0,   1,    1,   2300, 230001, 230001001 },
            { L_,   23,   0,   1,    2,   2300, 230001, 230001002 },
            { L_,   23,   0,   1,    9,   2300, 230001, 230001009 },
            { L_,   23,   0,   1,   10,   2300, 230001, 230001010 },
            { L_,   23,   0,   1,   11,   2300, 230001, 230001011 },
            { L_,   23,   0,   1,   99,   2300, 230001, 230001099 },
            { L_,   23,   0,   1,  100,   2300, 230001, 230001100 },
            { L_,   23,   0,   1,  101,   2300, 230001, 230001101 },
            { L_,   23,   0,   1,  998,   2300, 230001, 230001998 },
            { L_,   23,   0,   1,  999,   2300, 230001, 230001999 },
            { L_,   23,   0,  59,    0,   2300, 230059, 230059000 },
            { L_,   23,   0,  59,    1,   2300, 230059, 230059001 },
            { L_,   23,   0,  59,    2,   2300, 230059, 230059002 },
            { L_,   23,   0,  59,    9,   2300, 230059, 230059009 },
            { L_,   23,   0,  59,   10,   2300, 230059, 230059010 },
            { L_,   23,   0,  59,   11,   2300, 230059, 230059011 },
            { L_,   23,   0,  59,   99,   2300, 230059, 230059099 },
            { L_,   23,   0,  59,  100,   2300, 230059, 230059100 },
            { L_,   23,   0,  59,  101,   2300, 230059, 230059101 },
            { L_,   23,   0,  59,  998,   2300, 230059, 230059998 },
            { L_,   23,   0,  59,  999,   2300, 230059, 230059999 },
            { L_,   23,   1,   0,    0,   2301, 230100, 230100000 },
            { L_,   23,   1,   0,    1,   2301, 230100, 230100001 },
            { L_,   23,   1,   0,    2,   2301, 230100, 230100002 },
            { L_,   23,   1,   0,    9,   2301, 230100, 230100009 },
            { L_,   23,   1,   0,   10,   2301, 230100, 230100010 },
            { L_,   23,   1,   0,   11,   2301, 230100, 230100011 },
            { L_,   23,   1,   0,   99,   2301, 230100, 230100099 },
            { L_,   23,   1,   0,  100,   2301, 230100, 230100100 },
            { L_,   23,   1,   0,  101,   2301, 230100, 230100101 },
            { L_,   23,   1,   0,  998,   2301, 230100, 230100998 },
            { L_,   23,   1,   0,  999,   2301, 230100, 230100999 },
            { L_,   23,   1,   1,    0,   2301, 230101, 230101000 },
            { L_,   23,   1,   1,    1,   2301, 230101, 230101001 },
            { L_,   23,   1,   1,    2,   2301, 230101, 230101002 },
            { L_,   23,   1,   1,    9,   2301, 230101, 230101009 },
            { L_,   23,   1,   1,   10,   2301, 230101, 230101010 },
            { L_,   23,   1,   1,   11,   2301, 230101, 230101011 },
            { L_,   23,   1,   1,   99,   2301, 230101, 230101099 },
            { L_,   23,   1,   1,  100,   2301, 230101, 230101100 },
            { L_,   23,   1,   1,  101,   2301, 230101, 230101101 },
            { L_,   23,   1,   1,  998,   2301, 230101, 230101998 },
            { L_,   23,   1,   1,  999,   2301, 230101, 230101999 },
            { L_,   23,   1,  59,    0,   2301, 230159, 230159000 },
            { L_,   23,   1,  59,    1,   2301, 230159, 230159001 },
            { L_,   23,   1,  59,    2,   2301, 230159, 230159002 },
            { L_,   23,   1,  59,    9,   2301, 230159, 230159009 },
            { L_,   23,   1,  59,   10,   2301, 230159, 230159010 },
            { L_,   23,   1,  59,   11,   2301, 230159, 230159011 },
            { L_,   23,   1,  59,   99,   2301, 230159, 230159099 },
            { L_,   23,   1,  59,  100,   2301, 230159, 230159100 },
            { L_,   23,   1,  59,  101,   2301, 230159, 230159101 },
            { L_,   23,   1,  59,  998,   2301, 230159, 230159998 },
            { L_,   23,   1,  59,  999,   2301, 230159, 230159999 },
            { L_,   23,  59,   0,    0,   2359, 235900, 235900000 },
            { L_,   23,  59,   0,    1,   2359, 235900, 235900001 },
            { L_,   23,  59,   0,    2,   2359, 235900, 235900002 },
            { L_,   23,  59,   0,    9,   2359, 235900, 235900009 },
            { L_,   23,  59,   0,   10,   2359, 235900, 235900010 },
            { L_,   23,  59,   0,   11,   2359, 235900, 235900011 },
            { L_,   23,  59,   0,   99,   2359, 235900, 235900099 },
            { L_,   23,  59,   0,  100,   2359, 235900, 235900100 },
            { L_,   23,  59,   0,  101,   2359, 235900, 235900101 },
            { L_,   23,  59,   0,  998,   2359, 235900, 235900998 },
            { L_,   23,  59,   0,  999,   2359, 235900, 235900999 },
            { L_,   23,  59,   1,    0,   2359, 235901, 235901000 },
            { L_,   23,  59,   1,    1,   2359, 235901, 235901001 },
            { L_,   23,  59,   1,    2,   2359, 235901, 235901002 },
            { L_,   23,  59,   1,    9,   2359, 235901, 235901009 },
            { L_,   23,  59,   1,   10,   2359, 235901, 235901010 },
            { L_,   23,  59,   1,   11,   2359, 235901, 235901011 },
            { L_,   23,  59,   1,   99,   2359, 235901, 235901099 },
            { L_,   23,  59,   1,  100,   2359, 235901, 235901100 },
            { L_,   23,  59,   1,  101,   2359, 235901, 235901101 },
            { L_,   23,  59,   1,  998,   2359, 235901, 235901998 },
            { L_,   23,  59,   1,  999,   2359, 235901, 235901999 },
            { L_,   23,  59,  59,    0,   2359, 235959, 235959000 },
            { L_,   23,  59,  59,    1,   2359, 235959, 235959001 },
            { L_,   23,  59,  59,    2,   2359, 235959, 235959002 },
            { L_,   23,  59,  59,    9,   2359, 235959, 235959009 },
            { L_,   23,  59,  59,   10,   2359, 235959, 235959010 },
            { L_,   23,  59,  59,   11,   2359, 235959, 235959011 },
            { L_,   23,  59,  59,   99,   2359, 235959, 235959099 },
            { L_,   23,  59,  59,  100,   2359, 235959, 235959100 },
            { L_,   23,  59,  59,  101,   2359, 235959, 235959101 },
            { L_,   23,  59,  59,  998,   2359, 235959, 235959998 },
            { L_,   23,  59,  59,  999,   2359, 235959, 235959999 },
            { L_,   24,   0,   0,    0,   2400, 240000, 240000000 },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE        = DATA[i].d_lineNum;
            const int HOUR        = DATA[i].d_hr;
            const int MINUTE      = DATA[i].d_min;
            const int SECOND      = DATA[i].d_sec;
            const int MILLISECOND = DATA[i].d_ms;
            const int HHMM        = DATA[i].d_hhmm;
            const int HHMMSS      = DATA[i].d_hhmmss;
            const int HHMMSSmmm   = DATA[i].d_hhmmssmmm;

            if (veryVerbose) {
                T_ P_(LINE) P_(HOUR) P_(MINUTE) P_(SECOND) P_(MILLISECOND)
                   P_(HHMM) P_(HHMMSS) P(HHMMSSmmm)
            }

            const bdlt::Time timeValue(HOUR, MINUTE, SECOND, MILLISECOND);

            const int hhmm      = Util::convertToHHMM(timeValue);
            const int hhmmss    = Util::convertToHHMMSS(timeValue);
            const int hhmmssmmm = Util::convertToHHMMSSmmm(timeValue);

            if (veryVerbose) {
                T_ P_(hhmm) P_(hhmmss) P(hhmmssmmm)
            }

            ASSERT(hhmm      == HHMM);
            ASSERT(hhmmss    == HHMMSS);
            ASSERT(hhmmssmmm == HHMMSSmmm);
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'convertFromHHMMSSmmm' & 'isValidHHMMSSmmm'
        //
        // Concerns:
        //: 1 Verify 'isValidHHMMSSmmm' correctly determines the validity of
        //:   'int' representations of times.
        //:   o Verify with minimal valid values of each field.
        //:   o Verify with maximal valid values of each field.
        //:   o Verify with intermedial valid values of each field.
        //:   o Verify the special value 24:00:00.000.
        //:   o Verify with invalid values of each field (other than
        //:     milliseconds, which will just carry over into seconds).
        //: 2 Verify the conversion produces a correct result in the case of
        //:   the 'int' value being valid.
        //: o Verify the conversion detects an incorrect 'int' value.
        //
        // Plan:
        //: 1 Specify test vectors outlining the values of each field of the
        //:   int, and whether each 'int' repreents a valid 'Time' or not.
        //: 2 Iterate through the test vectors.
        //: 3 Combine the fields into an 'int' value.
        //: 4 Test the validity with 'isValidHHMMSSmmm' and verify it matches
        //:   the expected value from the table.
        //: 5 If the 'int' value is valid:
        //:   o Construct an expected 'Time' value from the fields from the
        //:     table.
        //:   o Convert the 'int' value to a 'Time' value with
        //:     'convertFromHHMMSSmmm'.
        //:   o Verify the two values are equal.
        //: 6 If the 'int' value is not valid:
        //:   o Do negative testing to verify that 'convertFromHHMMSSmmm'
        //:     catches the invalid value.
        //
        // Testing:
        //   static bdlt::Time convertFromHHMMSSmmm(int value);
        //   static bool isValidHHMMSSmmm(int value);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                    "\nTESTING 'convertFromHHMMSSmmm' & 'isValidHHMMSSmmm'\n"
                      "===================================================\n";

        static const struct {
            int d_lineNum;      // source line number
            int d_hour;         // hour field
            int d_minute;       // minute field
            int d_second;       // second field
            int d_millisecond;  // millisecond field
            int d_valid;        // are time fields valid?
        } DATA[] = {
            //line   hr   min   sec    ms   valid
            //----   --   ---   ---   ---   -----
            { L_,     0,    0,    0,    0,     1 },
            { L_,     0,    0,    0,    1,     1 },
            { L_,     0,    0,    0,    2,     1 },
            { L_,     0,    0,    0,    9,     1 },
            { L_,     0,    0,    0,   10,     1 },
            { L_,     0,    0,    0,   11,     1 },
            { L_,     0,    0,    0,   99,     1 },
            { L_,     0,    0,    0,  100,     1 },
            { L_,     0,    0,    0,  101,     1 },
            { L_,     0,    0,    0,  998,     1 },
            { L_,     0,    0,    0,  999,     1 },

            { L_,     0,    0,    1,    0,     1 },
            { L_,     0,    0,    1,    1,     1 },
            { L_,     0,    0,    1,    2,     1 },
            { L_,     0,    0,    1,    9,     1 },
            { L_,     0,    0,    1,   10,     1 },
            { L_,     0,    0,    1,   11,     1 },
            { L_,     0,    0,    1,   99,     1 },
            { L_,     0,    0,    1,  100,     1 },
            { L_,     0,    0,    1,  101,     1 },
            { L_,     0,    0,    1,  998,     1 },
            { L_,     0,    0,    1,  999,     1 },

            { L_,     0,    0,   59,    0,     1 },
            { L_,     0,    0,   59,    1,     1 },
            { L_,     0,    0,   59,    2,     1 },
            { L_,     0,    0,   59,    9,     1 },
            { L_,     0,    0,   59,   10,     1 },
            { L_,     0,    0,   59,   11,     1 },
            { L_,     0,    0,   59,   99,     1 },
            { L_,     0,    0,   59,  100,     1 },
            { L_,     0,    0,   59,  101,     1 },
            { L_,     0,    0,   59,  998,     1 },
            { L_,     0,    0,   59,  999,     1 },
            { L_,     0,    0,   60,    0,     0 },
            { L_,     0,    0,   61,   11,     0 },
            { L_,     0,    0,   99,  999,     0 },

            { L_,     0,    1,    0,    0,     1 },
            { L_,     0,    1,    0,    1,     1 },
            { L_,     0,    1,    0,    2,     1 },
            { L_,     0,    1,    0,    9,     1 },
            { L_,     0,    1,    0,   10,     1 },
            { L_,     0,    1,    0,   11,     1 },
            { L_,     0,    1,    0,   99,     1 },
            { L_,     0,    1,    0,  100,     1 },
            { L_,     0,    1,    0,  101,     1 },
            { L_,     0,    1,    0,  998,     1 },
            { L_,     0,    1,    0,  999,     1 },

            { L_,     0,    1,    1,    0,     1 },
            { L_,     0,    1,    1,    1,     1 },
            { L_,     0,    1,    1,    2,     1 },
            { L_,     0,    1,    1,    9,     1 },
            { L_,     0,    1,    1,   10,     1 },
            { L_,     0,    1,    1,   11,     1 },
            { L_,     0,    1,    1,   99,     1 },
            { L_,     0,    1,    1,  100,     1 },
            { L_,     0,    1,    1,  101,     1 },
            { L_,     0,    1,    1,  998,     1 },
            { L_,     0,    1,    1,  999,     1 },

            { L_,     0,    1,   59,    0,     1 },
            { L_,     0,    1,   59,    1,     1 },
            { L_,     0,    1,   59,    2,     1 },
            { L_,     0,    1,   59,    9,     1 },
            { L_,     0,    1,   59,   10,     1 },
            { L_,     0,    1,   59,   11,     1 },
            { L_,     0,    1,   59,   99,     1 },
            { L_,     0,    1,   59,  100,     1 },
            { L_,     0,    1,   59,  101,     1 },
            { L_,     0,    1,   59,  998,     1 },
            { L_,     0,    1,   59,  999,     1 },
            { L_,     0,    1,   60,    0,     0 },
            { L_,     0,    1,   61,   11,     0 },
            { L_,     0,    1,   99,  999,     0 },

            { L_,     0,   59,    0,    0,     1 },
            { L_,     0,   59,    0,    1,     1 },
            { L_,     0,   59,    0,    2,     1 },
            { L_,     0,   59,    0,    9,     1 },
            { L_,     0,   59,    0,   11,     1 },
            { L_,     0,   59,    0,   99,     1 },
            { L_,     0,   59,    0,  100,     1 },
            { L_,     0,   59,    0,  101,     1 },
            { L_,     0,   59,    0,  998,     1 },
            { L_,     0,   59,    0,  999,     1 },

            { L_,     0,   59,    1,    0,     1 },
            { L_,     0,   59,    1,    1,     1 },
            { L_,     0,   59,    1,    2,     1 },
            { L_,     0,   59,    1,    9,     1 },
            { L_,     0,   59,    1,   10,     1 },
            { L_,     0,   59,    1,   11,     1 },
            { L_,     0,   59,    1,   99,     1 },
            { L_,     0,   59,    1,  100,     1 },
            { L_,     0,   59,    1,  101,     1 },
            { L_,     0,   59,    1,  998,     1 },
            { L_,     0,   59,    1,  999,     1 },

            { L_,     0,   59,   59,    0,     1 },
            { L_,     0,   59,   59,    1,     1 },
            { L_,     0,   59,   59,    2,     1 },
            { L_,     0,   59,   59,    9,     1 },
            { L_,     0,   59,   59,   10,     1 },
            { L_,     0,   59,   59,   11,     1 },
            { L_,     0,   59,   59,   99,     1 },
            { L_,     0,   59,   59,  100,     1 },
            { L_,     0,   59,   59,  101,     1 },
            { L_,     0,   59,   59,  998,     1 },
            { L_,     0,   59,   59,  999,     1 },
            { L_,     0,   59,   60,    0,     0 },
            { L_,     0,   59,   61,   11,     0 },
            { L_,     0,   59,   99,  999,     0 },

            { L_,    12,    0,    0,    0,     1 },
            { L_,    12,    0,    0,    1,     1 },
            { L_,    12,    0,    0,    2,     1 },
            { L_,    12,    0,    0,    9,     1 },
            { L_,    12,    0,    0,   10,     1 },
            { L_,    12,    0,    0,   11,     1 },
            { L_,    12,    0,    0,   99,     1 },
            { L_,    12,    0,    0,  100,     1 },
            { L_,    12,    0,    0,  101,     1 },
            { L_,    12,    0,    0,  998,     1 },
            { L_,    12,    0,    0,  999,     1 },

            { L_,    12,    0,    1,    0,     1 },
            { L_,    12,    0,    1,    1,     1 },
            { L_,    12,    0,    1,    2,     1 },
            { L_,    12,    0,    1,    9,     1 },
            { L_,    12,    0,    1,   10,     1 },
            { L_,    12,    0,    1,   11,     1 },
            { L_,    12,    0,    1,   99,     1 },
            { L_,    12,    0,    1,  100,     1 },
            { L_,    12,    0,    1,  101,     1 },
            { L_,    12,    0,    1,  998,     1 },
            { L_,    12,    0,    1,  999,     1 },

            { L_,    12,    0,   59,    0,     1 },
            { L_,    12,    0,   59,    1,     1 },
            { L_,    12,    0,   59,    2,     1 },
            { L_,    12,    0,   59,    9,     1 },
            { L_,    12,    0,   59,   10,     1 },
            { L_,    12,    0,   59,   11,     1 },
            { L_,    12,    0,   59,   99,     1 },
            { L_,    12,    0,   59,  100,     1 },
            { L_,    12,    0,   59,  101,     1 },
            { L_,    12,    0,   59,  998,     1 },
            { L_,    12,    0,   59,  999,     1 },
            { L_,    12,    0,   60,    0,     0 },
            { L_,    12,    0,   61,   11,     0 },
            { L_,    12,    0,   99,  999,     0 },
            { L_,   120,    0,    0,    0,     0 },

            { L_,    12,    1,    0,    0,     1 },
            { L_,    12,    1,    0,    1,     1 },
            { L_,    12,    1,    0,    2,     1 },
            { L_,    12,    1,    0,    9,     1 },
            { L_,    12,    1,    0,   10,     1 },
            { L_,    12,    1,    0,   11,     1 },
            { L_,    12,    1,    0,   99,     1 },
            { L_,    12,    1,    0,  100,     1 },
            { L_,    12,    1,    0,  101,     1 },
            { L_,    12,    1,    0,  998,     1 },
            { L_,    12,    1,    0,  999,     1 },

            { L_,    12,    1,    1,    0,     1 },
            { L_,    12,    1,    1,    1,     1 },
            { L_,    12,    1,    1,    2,     1 },
            { L_,    12,    1,    1,    9,     1 },
            { L_,    12,    1,    1,   10,     1 },
            { L_,    12,    1,    1,   11,     1 },
            { L_,    12,    1,    1,   99,     1 },
            { L_,    12,    1,    1,  100,     1 },
            { L_,    12,    1,    1,  101,     1 },
            { L_,    12,    1,    1,  998,     1 },
            { L_,    12,    1,    1,  999,     1 },

            { L_,    12,    1,   59,    0,     1 },
            { L_,    12,    1,   59,    1,     1 },
            { L_,    12,    1,   59,    2,     1 },
            { L_,    12,    1,   59,    9,     1 },
            { L_,    12,    1,   59,   10,     1 },
            { L_,    12,    1,   59,   11,     1 },
            { L_,    12,    1,   59,   99,     1 },
            { L_,    12,    1,   59,  100,     1 },
            { L_,    12,    1,   59,  101,     1 },
            { L_,    12,    1,   59,  998,     1 },
            { L_,    12,    1,   59,  999,     1 },
            { L_,    12,    1,   60,    0,     0 },
            { L_,    12,    1,   61,   11,     0 },
            { L_,    12,    1,   99,  999,     0 },

            { L_,    12,   59,    0,    0,     1 },
            { L_,    12,   59,    0,    1,     1 },
            { L_,    12,   59,    0,    2,     1 },
            { L_,    12,   59,    0,    9,     1 },
            { L_,    12,   59,    0,   10,     1 },
            { L_,    12,   59,    0,   11,     1 },
            { L_,    12,   59,    0,   99,     1 },
            { L_,    12,   59,    0,  100,     1 },
            { L_,    12,   59,    0,  101,     1 },
            { L_,    12,   59,    0,  998,     1 },
            { L_,    12,   59,    0,  999,     1 },

            { L_,    12,   59,    1,    0,     1 },
            { L_,    12,   59,    1,    1,     1 },
            { L_,    12,   59,    1,    2,     1 },
            { L_,    12,   59,    1,    9,     1 },
            { L_,    12,   59,    1,   10,     1 },
            { L_,    12,   59,    1,   11,     1 },
            { L_,    12,   59,    1,   99,     1 },
            { L_,    12,   59,    1,  100,     1 },
            { L_,    12,   59,    1,  101,     1 },
            { L_,    12,   59,    1,  998,     1 },
            { L_,    12,   59,    1,  999,     1 },

            { L_,    12,   59,   59,    0,     1 },
            { L_,    12,   59,   59,    1,     1 },
            { L_,    12,   59,   59,    2,     1 },
            { L_,    12,   59,   59,    9,     1 },
            { L_,    12,   59,   59,   10,     1 },
            { L_,    12,   59,   59,   11,     1 },
            { L_,    12,   59,   59,   99,     1 },
            { L_,    12,   59,   59,  100,     1 },
            { L_,    12,   59,   59,  101,     1 },
            { L_,    12,   59,   59,  998,     1 },
            { L_,    12,   59,   59,  999,     1 },
            { L_,    12,   59,   60,    0,     0 },
            { L_,    12,   59,   61,   11,     0 },
            { L_,    12,   59,   99,  999,     0 },

            { L_,    23,    0,    0,    0,     1 },
            { L_,    23,    0,    0,    1,     1 },
            { L_,    23,    0,    0,    2,     1 },
            { L_,    23,    0,    0,    9,     1 },
            { L_,    23,    0,    0,   10,     1 },
            { L_,    23,    0,    0,   11,     1 },
            { L_,    23,    0,    0,   99,     1 },
            { L_,    23,    0,    0,  100,     1 },
            { L_,    23,    0,    0,  101,     1 },
            { L_,    23,    0,    0,  998,     1 },
            { L_,    23,    0,    0,  999,     1 },

            { L_,    23,    0,    1,    0,     1 },
            { L_,    23,    0,    1,    1,     1 },
            { L_,    23,    0,    1,    2,     1 },
            { L_,    23,    0,    1,    9,     1 },
            { L_,    23,    0,    1,   10,     1 },
            { L_,    23,    0,    1,   11,     1 },
            { L_,    23,    0,    1,   99,     1 },
            { L_,    23,    0,    1,  100,     1 },
            { L_,    23,    0,    1,  101,     1 },
            { L_,    23,    0,    1,  998,     1 },
            { L_,    23,    0,    1,  999,     1 },

            { L_,    23,    0,   59,    0,     1 },
            { L_,    23,    0,   59,    1,     1 },
            { L_,    23,    0,   59,    2,     1 },
            { L_,    23,    0,   59,    9,     1 },
            { L_,    23,    0,   59,   10,     1 },
            { L_,    23,    0,   59,   11,     1 },
            { L_,    23,    0,   59,   99,     1 },
            { L_,    23,    0,   59,  100,     1 },
            { L_,    23,    0,   59,  101,     1 },
            { L_,    23,    0,   59,  998,     1 },
            { L_,    23,    0,   59,  999,     1 },
            { L_,    23,    0,   60,    0,     0 },
            { L_,    23,    0,   61,   11,     0 },
            { L_,    23,    0,   99,  999,     0 },

            { L_,    23,    1,    0,    0,     1 },
            { L_,    23,    1,    0,    1,     1 },
            { L_,    23,    1,    0,    2,     1 },
            { L_,    23,    1,    0,    9,     1 },
            { L_,    23,    1,    0,   10,     1 },
            { L_,    23,    1,    0,   11,     1 },
            { L_,    23,    1,    0,   99,     1 },
            { L_,    23,    1,    0,  100,     1 },
            { L_,    23,    1,    0,  101,     1 },
            { L_,    23,    1,    0,  998,     1 },
            { L_,    23,    1,    0,  999,     1 },

            { L_,    23,    1,    1,    0,     1 },
            { L_,    23,    1,    1,    1,     1 },
            { L_,    23,    1,    1,    2,     1 },
            { L_,    23,    1,    1,    9,     1 },
            { L_,    23,    1,    1,   10,     1 },
            { L_,    23,    1,    1,   11,     1 },
            { L_,    23,    1,    1,   99,     1 },
            { L_,    23,    1,    1,  100,     1 },
            { L_,    23,    1,    1,  101,     1 },
            { L_,    23,    1,    1,  998,     1 },
            { L_,    23,    1,    1,  999,     1 },

            { L_,    23,    1,   59,    0,     1 },
            { L_,    23,    1,   59,    1,     1 },
            { L_,    23,    1,   59,    2,     1 },
            { L_,    23,    1,   59,    9,     1 },
            { L_,    23,    1,   59,   10,     1 },
            { L_,    23,    1,   59,   11,     1 },
            { L_,    23,    1,   59,   99,     1 },
            { L_,    23,    1,   59,  100,     1 },
            { L_,    23,    1,   59,  101,     1 },
            { L_,    23,    1,   59,  998,     1 },
            { L_,    23,    1,   59,  999,     1 },
            { L_,    23,    1,   60,    0,     0 },
            { L_,    23,    1,   61,   11,     0 },
            { L_,    23,    1,   99,  999,     0 },

            { L_,    23,   59,    0,    0,     1 },
            { L_,    23,   59,    0,    1,     1 },
            { L_,    23,   59,    0,    2,     1 },
            { L_,    23,   59,    0,    9,     1 },
            { L_,    23,   59,    0,   10,     1 },
            { L_,    23,   59,    0,   11,     1 },
            { L_,    23,   59,    0,   99,     1 },
            { L_,    23,   59,    0,  100,     1 },
            { L_,    23,   59,    0,  101,     1 },
            { L_,    23,   59,    0,  998,     1 },
            { L_,    23,   59,    0,  999,     1 },

            { L_,    23,   59,    1,    0,     1 },
            { L_,    23,   59,    1,    1,     1 },
            { L_,    23,   59,    1,    2,     1 },
            { L_,    23,   59,    1,    9,     1 },
            { L_,    23,   59,    1,   10,     1 },
            { L_,    23,   59,    1,   11,     1 },
            { L_,    23,   59,    1,   99,     1 },
            { L_,    23,   59,    1,  100,     1 },
            { L_,    23,   59,    1,  101,     1 },
            { L_,    23,   59,    1,  998,     1 },
            { L_,    23,   59,    1,  999,     1 },

            { L_,    23,   59,   59,    0,     1 },
            { L_,    23,   59,   59,    1,     1 },
            { L_,    23,   59,   59,    2,     1 },
            { L_,    23,   59,   59,    9,     1 },
            { L_,    23,   59,   59,   10,     1 },
            { L_,    23,   59,   59,   11,     1 },
            { L_,    23,   59,   59,   99,     1 },
            { L_,    23,   59,   59,  100,     1 },
            { L_,    23,   59,   59,  101,     1 },
            { L_,    23,   59,   59,  998,     1 },
            { L_,    23,   59,   59,  999,     1 },
            { L_,    23,   59,   60,    0,     0 },
            { L_,    23,   59,   61,   11,     0 },
            { L_,    23,   59,   99,  999,     0 },
            { L_,   230,    0,    0,    0,     0 },

            { L_,    24,    0,    0,    0,     1 },
            { L_,    24,    0,    0,    6,     0 },
            { L_,    24,    0,    6,    0,     0 },
            { L_,    24,    6,    0,    0,     0 },
            { L_,    24,    0,    6,    6,     0 },
            { L_,    24,    6,    0,    6,     0 },
            { L_,    24,    6,    6,    0,     0 },
            { L_,    24,    6,    6,    6,     0 },
            { L_,   240,    0,    0,    0,     0 },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE        = DATA[ti].d_lineNum;
            const int HOUR        = DATA[ti].d_hour;
            const int MINUTE      = DATA[ti].d_minute;
            const int SECOND      = DATA[ti].d_second;
            const int MILLISECOND = DATA[ti].d_millisecond;
            const int VALID       = DATA[ti].d_valid;

            const int VALUE =   HOUR * 10000000
                            + MINUTE * 100000
                            + SECOND * 1000
                            + MILLISECOND;

            if (veryVerbose) {
                T_ P_(LINE) P_(HOUR) P_(MINUTE) P_(SECOND) P_(MILLISECOND)
                   P(VALID) P(VALUE)
            }

            ASSERT(VALID == Util::isValidHHMMSSmmm(VALUE));

            if (VALID) {
                const bdlt::Time exp(HOUR, MINUTE, SECOND, MILLISECOND);
                bdlt::Time       result = Util::convertFromHHMMSSmmm(VALUE);

                if (veryVerbose) {
                    T_  P_(result.hour())   P_(result.minute())
                        P_(result.second())  P(result.millisecond())
                }

                ASSERT(exp == result);
            }
            else {
                bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

                ASSERT_SAFE_FAIL(Util::convertFromHHMMSSmmm(VALUE));
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'convertFromHHMMSS' & 'isValidHHMMSS'
        //
        // Concerns:
        //: 1 Verify 'isValidHHMMSS' correctly determines the validity of 'int'
        //:   representations of times.
        //:   o Verify with minimal valid values of each field.
        //:   o Verify with maximal valid values of each field.
        //:   o Verify with intermedial valid values of each field.
        //:   o Verify the special value 24:00:00.
        //:   o Verify with invalid values of each field.
        //: 2 Verify the conversion produces a correct result in the case of
        //:   the 'int' value being valid.
        //: o Verify the conversion detects an incorrect 'int' value.
        //
        // Plan:
        //: 1 Specify test vectors outlining the values of each field of the
        //:   int, and whether each 'int' repreents a valid 'Time' or not.
        //: 2 Iterate through the test vectors.
        //: 3 Combine the fields into an 'int' value.
        //: 4 Test the validity with 'isValidHHMMSS' and verify it matches the
        //:   expected value from the table.
        //: 5 If the 'int' value is valid:
        //:   o Construct an expected 'Time' value from the fields from the
        //:     table.
        //:   o Convert the 'int' value to a 'Time' value with
        //:     'convertFromHHMMSS'.
        //:   o Verify the two values are equal.
        //: 6 If the 'int' value is not valid:
        //:   o Do negative testing to verify that 'convertFromHHMMSS' catches
        //:     the invalid value.
        //
        // Testing:
        //   static bdlt::Time convertFromHHMMSS(int value);
        //   static bool isValidHHMMSS(int value);
        // --------------------------------------------------------------------

        if (verbose) cout <<
                           "\nTESTING 'convertFromHHMMSS' & 'isValidHHMMSS'\n"
                             "=============================================\n";

        static const struct {
            int d_lineNum;  // source line number
            int d_hour;     // hour field
            int d_minute;   // minute field
            int d_second;   // second field
            int d_valid;    // are time fields valid?
        } DATA[] = {
            //line   hr   min   sec   valid
            //----   --   ---   ---   -----
            { L_,     0,    0,    0,     1 },
            { L_,     0,    0,    1,     1 },
            { L_,     0,    0,   59,     1 },
            { L_,     0,    0,   60,     0 },
            { L_,     0,    0,   61,     0 },
            { L_,     0,    0,   99,     0 },

            { L_,     0,    1,    0,     1 },
            { L_,     0,    1,    1,     1 },
            { L_,     0,    1,   59,     1 },
            { L_,     0,    1,   60,     0 },
            { L_,     0,    1,   61,     0 },
            { L_,     0,    1,   99,     0 },

            { L_,     0,   59,    0,     1 },
            { L_,     0,   59,    1,     1 },
            { L_,     0,   59,    59,    1 },
            { L_,     0,   59,    60,    0 },
            { L_,     0,   59,    60,    0 },
            { L_,     0,   59,    99,    0 },

            { L_,     1,    0,    0,     1 },
            { L_,     1,    0,    1,     1 },
            { L_,     1,    0,   59,     1 },
            { L_,     1,    0,   60,     0 },
            { L_,     1,    0,   61,     0 },
            { L_,     1,    0,   99,     0 },

            { L_,     1,    1,    0,     1 },
            { L_,     1,    1,    1,     1 },
            { L_,     1,    1,   59,     1 },
            { L_,     1,    1,   60,     0 },
            { L_,     1,    1,   61,     0 },
            { L_,     1,    1,   99,     0 },

            { L_,     1,   59,    0,     1 },
            { L_,     1,   59,    1,     1 },
            { L_,     1,   59,    59,    1 },
            { L_,     1,   59,    60,    0 },
            { L_,     1,   59,    60,    0 },
            { L_,     1,   59,    99,    0 },

            { L_,    12,    0,    0,     1 },
            { L_,    12,    0,    1,     1 },
            { L_,    12,    0,   59,     1 },
            { L_,    12,    0,   60,     0 },
            { L_,    12,    0,   61,     0 },
            { L_,    12,    0,   99,     0 },

            { L_,    12,    1,    0,     1 },
            { L_,    12,    1,    1,     1 },
            { L_,    12,    1,   59,     1 },
            { L_,    12,    1,   60,     0 },
            { L_,    12,    1,   61,     0 },
            { L_,    12,    1,   99,     0 },

            { L_,    12,   59,    0,     1 },
            { L_,    12,   59,    1,     1 },
            { L_,    12,   59,    1,     1 },
            { L_,    12,   59,   59,     1 },
            { L_,    12,   59,   60,     0 },
            { L_,    12,   59,   61,     0 },
            { L_,    12,   59,   99,     0 },

            { L_,    23,    0,    0,     1 },
            { L_,    23,    0,    1,     1 },
            { L_,    23,    0,   59,     1 },
            { L_,    23,    0,   60,     0 },
            { L_,    23,    0,   61,     0 },
            { L_,    23,    0,   99,     0 },

            { L_,    23,    1,    0,     1 },
            { L_,    23,    1,    1,     1 },
            { L_,    23,    1,   59,     1 },
            { L_,    23,    1,   60,     0 },
            { L_,    23,    1,   61,     0 },
            { L_,    23,    1,   99,     0 },

            { L_,    23,   59,    0,     1 },
            { L_,    23,   59,    1,     1 },
            { L_,    23,   59,   59,     1 },
            { L_,    23,   59,   60,     0 },
            { L_,    23,   59,   61,     0 },
            { L_,    23,   59,   99,     0 },

            { L_,    24,    0,    0,     1 },
            { L_,    24,    0,    1,     0 },
            { L_,    24,    2,    0,     0 },
            { L_,    24,   24,   24,     0 },
            { L_,   240,    0,    0,     0 },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE   = DATA[ti].d_lineNum;
            const int HOUR   = DATA[ti].d_hour;
            const int MINUTE = DATA[ti].d_minute;
            const int SECOND = DATA[ti].d_second;
            const int VALID  = DATA[ti].d_valid;

            const int VALUE = HOUR * 10000 + MINUTE * 100 + SECOND;

            if (veryVerbose) {
                T_ P_(LINE) P_(HOUR) P_(MINUTE) P_(SECOND) P(VALID)
                   P(VALUE)
            }

            ASSERT(VALID == Util::isValidHHMMSS(VALUE));

            if (VALID) {
                const bdlt::Time exp(HOUR, MINUTE, SECOND);
                bdlt::Time       result = Util::convertFromHHMMSS(VALUE);

                if (veryVerbose) {
                    T_ P_(result.hour())  P_(result.minute())
                       P_(result.second()) P(result.millisecond())
                }

                ASSERT(exp == result);
            }
            else {
                bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

                ASSERT_SAFE_FAIL(Util::convertFromHHMMSS(VALUE));
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'convertFromHHMM' & 'isValidHHMM'
        //
        // Concerns:
        //: 1 Verify 'isValidHHMM' correctly determines the validity of 'int'
        //:   representations of times.
        //:   o Verify with minimal valid values of each field.
        //:   o Verify with maximal valid values of each field.
        //:   o Verify with intermedial valid values of each field.
        //:   o Verify the special value 24:00.
        //:   o Verify with invalid values of each field.
        //: 2 Verify the conversion produces a correct result in the case of
        //:   the 'int' value being valid.
        //: o Verify the conversion detects an incorrect 'int' value.
        //
        // Plan:
        //: 1 Specify test vectors outlining the values of each field of the
        //:   int, and whether each 'int' repreents a valid 'Time' or not.
        //: 2 Iterate through the test vectors.
        //: 3 Combine the fields into an 'int' value.
        //: 4 Test the validity with 'isValidHHMM' and verify it matches the
        //:   expected value from the table.
        //: 5 If the 'int' value is valid:
        //:   o Construct an expected 'Time' value from the fields from the
        //:     table.
        //:   o Convert the 'int' value to a 'Time' value with
        //:     'convertFromHHMM'.
        //:   o Verify the two values are equal.
        //: 6 If the 'int' value is not valid:
        //:   o Do negative testing to verify that 'convertFromHHMM' catches
        //:     the invalid value.
        //
        // Testing:
        //   static bdlt::Time convertFromHHMM(int value);
        //   static bool isValidHHMM(int value);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'convertFromHHMM' & 'isValidHHMM'\n"
                               "=========================================\n";

        static const struct {
            int d_lineNum;  // source line number
            int d_hour;     // hour field
            int d_minute;   // minute field
            int d_valid;    // are time fields valid?
        } DATA[] = {
            //line   hr   min   valid
            //----   --   ---   -----
            { L_,     0,    0,     1 },
            { L_,     0,    1,     1 },
            { L_,     0,    2,     1 },
            { L_,     0,   58,     1 },
            { L_,     0,   59,     1 },
            { L_,     0,   60,     0 },
            { L_,     0,   61,     0 },
            { L_,     0,   99,     0 },

            { L_,     1,    0,     1 },
            { L_,     1,    1,     1 },
            { L_,     1,    2,     1 },
            { L_,     1,   58,     1 },
            { L_,     1,   59,     1 },
            { L_,     1,   60,     0 },
            { L_,     1,   61,     0 },
            { L_,     1,   99,     0 },
            { L_,   100,    0,     0 },

            { L_,    11,    0,     1 },
            { L_,    11,    1,     1 },
            { L_,    11,    2,     1 },
            { L_,    11,   58,     1 },
            { L_,    11,   59,     1 },
            { L_,    11,   60,     0 },
            { L_,    11,   61,     0 },
            { L_,    11,   99,     0 },
            { L_,   110,    0,     0 },

            { L_,    12,    0,     1 },
            { L_,    12,    1,     1 },
            { L_,    12,    2,     1 },
            { L_,    12,   58,     1 },
            { L_,    12,   59,     1 },
            { L_,    12,   60,     0 },
            { L_,    12,   61,     0 },
            { L_,    12,   99,     0 },
            { L_,   120,    0,     0 },

            { L_,    13,    0,     1 },
            { L_,    13,    1,     1 },
            { L_,    13,    2,     1 },
            { L_,    13,   58,     1 },
            { L_,    13,   59,     1 },
            { L_,    13,   60,     0 },
            { L_,    13,   61,     0 },
            { L_,    13,   99,     0 },
            { L_,   130,    0,     0 },

            { L_,    23,    0,     1 },
            { L_,    23,    1,     1 },
            { L_,    23,    2,     1 },
            { L_,    23,   58,     1 },
            { L_,    23,   59,     1 },
            { L_,    23,   60,     0 },
            { L_,    23,   61,     0 },
            { L_,    23,   99,     0 },
            { L_,   230,    0,     0 },

            { L_,    24,    0,     1 },
            { L_,    24,    1,     0 },
            { L_,    24,   59,     0 },
            { L_,    24,   99,     0 },
            { L_,   240,    0,     0 },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE   = DATA[ti].d_lineNum;
            const int HOUR   = DATA[ti].d_hour;
            const int MINUTE = DATA[ti].d_minute;
            const int VALID  = DATA[ti].d_valid;

            const int VALUE = HOUR * 100 + MINUTE;

            if (veryVerbose) {
                T_ P_(LINE) P_(HOUR) P_(MINUTE) P(VALID)
                   P(VALUE)
            }

            ASSERT(VALID == Util::isValidHHMM(VALUE));

            if (VALID) {
                const bdlt::Time exp(HOUR, MINUTE);
                bdlt::Time       result = Util::convertFromHHMM(VALUE);

                if (veryVerbose) {
                    T_ P_(result.hour())  P_(result.minute())
                       P_(result.second()) P(result.millisecond())
                }

                ASSERT(exp == result);
            }
            else {
                bsls::AssertFailureHandlerGuard
                                          hG(bsls::AssertTest::failTestDriver);

                ASSERT_SAFE_FAIL(Util::convertFromHHMM(VALUE));
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
