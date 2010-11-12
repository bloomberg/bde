// bdetu_informixdatetime.t.cpp      -*-C++-*-

#include <bdetu_informixdatetime.h>

#include <bsl_iostream.h>
#include <bsl_climits.h>
#include <bsl_cstdio.h>   // sprintf()
#include <bsl_cstdlib.h>  // atoi()
#include <bsl_cstring.h>  // memset()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// This test driver emphasizes a black-box approach, which is necessarily quite
// voluminous relative to what would be required given white-box knowledge.
// We use the standard table-based test case implementation techniques coupled
// with category partitioning to exercise these utility functions.  We also
// use loop-based, statistical methods to ensure invertibility where
// appropriate.
//-----------------------------------------------------------------------------
// [ 2] bdet_Datetime& epoch();
// [ 4] int convertToTimeT(time_t *result, const bdet_Datetime& datetime);
// [ 4] void convertFromTimeT(bdet_Datetime *result, time_t time);
// [ 3] void convertToTm(tm *result, const bdet_Datetime& datetime);
// [ 3] int convertFromTm(bdet_Datetime *result, const tm& timeStruct);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST -- (developer's sandbox)
// [ 5] USAGE EXAMPLE -- Daylight Saving Time
//-----------------------------------------------------------------------------

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
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
        << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << "\t" << flush;             // Print a tab (w/o newline)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------
typedef bdetu_InformixDatetime Util;

typedef bsls_PlatformUtil::Int64  Int64;
typedef bsls_PlatformUtil::Uint64 Uint64;

enum { VERBOSE_ARG_NUM = 2, VERY_VERBOSE_ARG_NUM, VERY_VERY_VERBOSE_ARG_NUM };

//=============================================================================
//               SEMI-STANDARD HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

#if 0
static inline int min(int a, int b) { return a < b ? a : b; }
    // Return the minimum of the specified 'a' and 'b' arguments.
#endif

static int parseAbsVerboseArg(int           *verboseFlag,
                              int            verboseArgNum,
                              char          *argv[],
                              unsigned char *switches = 0)
    // If the value addressed by the specified 'verboseFlag' is 0, return 0
    // immediately with no other effect.  Otherwise use 'atoi' to obtain the
    // integer value 'V' of the argument at the specified 'verboseArgNum'
    // position in the specified 'argv' array, and set '*verboseFlag' to 0 if
    // 'V' is negative.  Return the absolute value of 'V'.  If the optionally
    // specified 'switches' is non-zero, the character code for each character
    // in 'argv' that is not part of a leading C++-style integer value will be
    // used to increment the corresponding element in 'switches'.
    // Additionally, making 'verboseArgNumber' negative suppresses modification
    // of the *verboseFlag* and returns the argument as a signed integer value.
    // The behavior is undefined unless *verboseFlag correctly indicates the
    // existence of the 'verboseArgNum' entry >= 2 in 'argv', and 'switches'
    // (if non-zero) is large enough to accommodate all relevant character
    // codes in 'argv[verboseArgNumber]'.
{
    ASSERT(verboseFlag);

    if (!*verboseFlag) return 0;

    bool isAbsFlag = 1;
    if (verboseArgNum < 0) { // Allow test designer to return signed arg value.
        isAbsFlag = 0;
        verboseArgNum = -verboseArgNum;
    }

    ASSERT(2 <= verboseArgNum);  // Don't mess with program or test case.
    ASSERT(argv);
    const char *const input = argv[verboseArgNum];
    ASSERT(input);

    if (switches) {
        const char *in = input;

        // Skip optional leading sign provided second character is a digit.
        in += ('-' == *in || '+' == *in) && '0' <= in[1] && in[1] <= '9';

        // Skip leading decimal digits.
        while ('0' <= *in && *in <= '9') ++in;

        // Modify switches based on trailing characters of 'in'.
        for (; *in; ++in) {
            ++switches[(unsigned char) *in];
        }
    }

    // Parse leading integer and return either signed or unsigned.
    int verboseArgValue = atoi(input);
    if (isAbsFlag && verboseArgValue < 0) {
        *verboseFlag = 0;                             // Suppress this level.
        ASSERT(-verboseArgValue != verboseArgValue);  // Check for INT_MIN!
        return -verboseArgValue; // absolute value of leading integer
    }
    return verboseArgValue; // (possibly negative) leading integer argument
}

void loopMeter(unsigned index, unsigned length, unsigned size = 50)
    // Create a visual display for a computation of the specified 'length' and
    // emit updates to 'cerr' as appropriate for each 'index'.  Optionally
    // specify the 'size' of the display.  The behavior is undefined
    // unless 0 <= index, 0 <= length, 0 < size, and index <= length.  Note
    // that it is expected that indices will be presented in order from 0
    // to 'length', inclusive, without intervening output to 'stderr';
    // however, intervening output to 'stdout' may be redirected productively.
{
    ASSERT(0 <= (int)index);
    ASSERT(0 <= (int)length);
    ASSERT(0 < size);
    ASSERT(index <= length);

    if (0 == index) {           // We are at the beginning of the loop.
        cerr << "     |";
        for (unsigned i = 1; i < size; ++i) {
            cerr << (i % 5 ? '-' : '+');
        }
        cerr << "|\nBEGIN." << flush;
    }
    else {                      // We are in the middle of the loop.
        int t1 = int((double(index - 1) * size)/length + 0.5);
        int t2 = int((double(index)     * size)/length + 0.5);
        int dt = t2 - t1;       // accumulated ticks (but no accumulated error)

        for (int i = 0; i < dt; ++i) {
           cerr << '.';
        }
        cerr << flush;
    }

    if (index == length) {      // We are at the end of the loop.
        cerr << "END" << endl;
    }
}

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------


//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
//-----------------------------------------------------------------------------


//=============================================================================
//                  CLASSES FOR TESTING USAGE EXAMPLES
//-----------------------------------------------------------------------------


//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
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

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;


      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONVERT BDET_DATETIME TO/FROM TIME_T
        //
        // Concerns:
        //   To:
        //     1. All fields are converted properly.
        //     2. The millisecond field is ignored.
        //     3. Time = 24:00:00:000 converts to 00:00:00, not to 24:00:00.
        //     4. Datetime values producing out-of-range time_t values fail:
        //         a. time_t < 0
        //         b. time_t > 2^31 - 1
        //         c. internal 32-bit integer temporaries do not overflow.
        //     5. Bad status implies no change to result.
        //
        //   From:
        //     1. All relevant fields are recovered properly.
        //     2. The millisecond field is always set to 0.
        //     3. Negative inputs are treated as failures.
        //     4. Out-of-range dates are detected.
        //     5. Bad status implies no change to result.
        //
        // Plan:
        //   1. Test convertToTimeT
        //        * Table-Based Implementation
        //        * Category Partitioning Data Selection
        //        * Orthogonal Perturbation:
        //              * Unaltered Initial Values
        //
        //        Construct a table in which each input field, status, and
        //        output value are represented in separate columns.  A
        //        failure status implies that the result is not changed,
        //        which is verified within the body of the loop on two
        //        separate initial values.  Note that the body of the main
        //        loop will also be used to partially test 'convertFromTimeT'.
        //
        //   2. Test convertFromTimeT
        //        * Reuse
        //        * Table-Based Implementation
        //        * Category Partitioning Data Selection
        //        * Exploit proven inverse operation: convertToTimeT
        //        * Orthogonal Perturbation:
        //              * Unaltered Initial Values
        //
        //        A. First, REUSE the table used to test 'convertToTimeT' to
        //           reverse every successful conversion to 'time_t', and
        //           compare that result against the initial input (except in
        //           cases where milliseconds were initially non-zero).
        //
        //        B. Second, create a separate table that explicitly converts
        //           valid (non-negative) 'time_t' to 'bdet_Datetime' values
        //           with input and individual output fields represented as
        //           separate columns.  For each result, that the input is the
        //           table's expected value is verified via the proven
        //           'convertToTimeT'; 'convertFromTimeT' is then applied, and
        //           the result of that calculation is compared with the
        //           expected result values in the table.  Note that the
        //           'millisecond' field is necessarily 0, and is tested
        //           directly within the main loop.
        //
        //   3. Test convertToTimeT and then convertFromTimeT
        //        * Exploit Inverse Relationship
        //        * Loop-Based Implementation
        //        * Pseudo-Random Data Selection
        //            * Exploiting Inverse Relationship
        //
        //        Use a loop-based approach to verify that pseudo-randomly
        //        selected, non-repeating datetime values that can be
        //        converted to and then from 'struct tm' objects result in
        //        exactly the same object.  Note that the intermediate
        //        'time_t' instance is initialized to a different "garbage"
        //        value on each iteration.
        //
        // Command Line:
        //     Numeric verbose switches suppress individual test cases:
        //             verbose switch '1' suppresses block 1.
        //             verbose switch 'a' suppresses REUSE of table in block 1.
        //             verbose switch '2' suppresses block 2.
        //             verbose switch '3' suppresses block 3.
        //
        //           veryVerboseArg != 0
        //
        //           veryVerboseArg  < 0  suppresses veryVerbose mode.
        //       abs(veryVerboseArg) > 0  limits # of orthogonal perturbations.
        //
        //       veryVeryVerboseArg  < 0  suppresses veryVeryVerbose mode.
        //   abs(veryVeryVerboseArg) > 0  controls the length of pseudo-random,
        //                                potentially exhaustive generate/parse
        //                                test.
        //
        // Testing:
        //   int convertToTimeT(time_t *result, const bdet_Datetime& datetime);
        //   void convertFromTimeT(bdet_Datetime *result, time_t time);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BDET_DATETIME TO/FROM TIME_T" << endl
                          << "============================" << endl;
#if 0
        if (verbose) cout <<
                "\nProcessing case-specific command-line arguments." << endl;

        const int wasVeryVeryVerbose = veryVeryVerbose; // used in Block 3

        static unsigned char switches[128];
        const int argVerbose = parseAbsVerboseArg(&verbose,
                                             -VERBOSE_ARG_NUM, argv, switches);
        const int absVeryVerbose = parseAbsVerboseArg(&veryVerbose,
                                                   VERY_VERBOSE_ARG_NUM, argv);
        const int absVeryVeryVerbose = parseAbsVerboseArg(&veryVeryVerbose,
                                              VERY_VERY_VERBOSE_ARG_NUM, argv);

        if (veryVerbose) {
            T_ P_(verbose) T_ T_ P(argVerbose)
            T_ P_(veryVerbose) T_ P(absVeryVerbose)
            T_ P_(veryVeryVerbose) T_ P(absVeryVeryVerbose)
            T_ P(wasVeryVeryVerbose)
            for (int i = 0; i < sizeof switches / sizeof *switches; ++i) {
                if (switches[i]) {
                    cout << "\t\tverbose switch '" << char(i) << "': "
                         << (int)switches[i] << endl;
                }
            }
        }

        if (!switches['1']) {
            if (verbose) cout << "\nbdet_Datetime => time_t." << endl;

            enum { FAILURE = 1 };

            static const struct {
                int d_lineNum;      // source line number

                int d_year;         // input
                int d_month;        // input
                int d_day;          // input
                int d_hour;         // input
                int d_minute;       // input
                int d_second;       // input
                int d_millisecond;  // input

                int d_status;       // value returned by function

                time_t d_time;      // output

            } DATA[] = {
                   // <---------- input ---------->   <--expected->
                //lin year mon day hou min sec msec   s      result
                //--- ---- --- --- --- --- --- ----   -      ------

                    // *** out-of-range input values fail ***          Note:
                //lin year mon day hou min sec msec   s      result    ld =
                //--- ---- --- --- --- --- --- ----   -      ------    Leap Day
                { L_,    1,  1,  1,  0,  0,  0,   0,  FAILURE       },

                { L_, 1869, 12, 31, 23, 59, 59, 999,  FAILURE       },
                { L_, 1879, 12, 31, 23, 59, 59, 999,  FAILURE       },
                { L_, 1889, 12, 31, 23, 59, 59, 999,  FAILURE       },
                { L_, 1899, 12, 31, 23, 59, 59, 999,  FAILURE       },
                { L_, 1909, 12, 31, 23, 59, 59, 999,  FAILURE       },
                { L_, 1919, 12, 31, 23, 59, 59, 999,  FAILURE       },
                { L_, 1929, 12, 31, 23, 59, 59, 999,  FAILURE       },
                { L_, 1939, 12, 31, 23, 59, 59, 999,  FAILURE       },
                { L_, 1949, 12, 31, 23, 59, 59, 999,  FAILURE       },
                { L_, 1959, 12, 31, 23, 59, 59, 999,  FAILURE       },

                { L_, 1969, 12, 31, 23, 59, 59, 999,  FAILURE       },
                { L_, 1970,  1,  1,  0,  0,  0,   0,  0,          0 }, //  0ld

                { L_, 1980,  1,  1,  0,  0,  0,   0,  0,  315532800 }, //  2ld
                { L_, 1990,  1,  1,  0,  0,  0,   0,  0,  631152000 }, //  5ld
                { L_, 2000,  1,  1,  0,  0,  0,   0,  0,  946684800 }, //  7ld
                { L_, 2010,  1,  1,  0,  0,  0,   0,  0, 1262304000 }, // 10ld
                { L_, 2020,  1,  1,  0,  0,  0,   0,  0, 1577836800 }, // 12ld
                { L_, 2030,  1,  1,  0,  0,  0,   0,  0, 1893456000 }, // 15ld
                { L_, 2038,  1,  1,  0,  0,  0,   0,  0, 2145916800 }, // 17ld

                { L_, 2038,  1, 19,  0,  0,  0,   0,  0, 2147472000 }, // 17ld
                { L_, 2038,  1, 19,  3,  0,  0,   0,  0, 2147482800 }, // 17ld
                { L_, 2038,  1, 19,  3, 14,  0,   0,  0, 2147483640 }, // 17ld
                { L_, 2038,  1, 19,  3, 14,  7,   0,  0, 2147483647 }, // 17ld
                { L_, 2038,  1, 19,  3, 14,  8,   0,  FAILURE       },

                { L_, 2048,  1, 19,  3, 14,  8,   0,  FAILURE       },
                { L_, 2058,  1, 19,  3, 14,  8,   0,  FAILURE       },
                { L_, 2068,  1, 19,  3, 14,  8,   0,  FAILURE       },
                { L_, 2078,  1, 19,  3, 14,  8,   0,  FAILURE       },
                { L_, 2088,  1, 19,  3, 14,  8,   0,  FAILURE       },
                { L_, 2098,  1, 19,  3, 14,  8,   0,  FAILURE       },
                { L_, 2108,  1, 19,  3, 14,  8,   0,  FAILURE       },

                { L_, 9999, 12, 31, 23, 59, 59, 999,  FAILURE       },

                    // *** All fields are converted properly ***
                //lin year mon day hou min sec msec   s      result
                //--- ---- --- --- --- --- --- ----   -      ------
                { L_, 1971,  1,  1,  0,  0,  0,   0,  0,   31536000 }, // year
                { L_, 1972,  1,  1,  0,  0,  0,   0,  0,   63072000 }, // year
                { L_, 1973,  1,  1,  0,  0,  0,   0,  0,   94694400 }, // year

                { L_, 1971,  2,  1,  0,  0,  0,   0,  0,   34214400 }, // month
                { L_, 1971,  3,  1,  0,  0,  0,   0,  0,   36633600 }, // month
                { L_, 1971, 12,  1,  0,  0,  0,   0,  0,   60393600 }, // month

                { L_, 1972,  2,  1,  0,  0,  0,   0,  0,   65750400 }, // month
                { L_, 1972,  3,  1,  0,  0,  0,   0,  0,   68256000 }, // month
                { L_, 1972, 12,  1,  0,  0,  0,   0,  0,   92016000 }, // month

                { L_, 1972,  1, 30,  0,  0,  0,   0,  0,   65577600 }, // day
                { L_, 1972,  1, 31,  0,  0,  0,   0,  0,   65664000 }, // day
                { L_, 1972,  2, 29,  0,  0,  0,   0,  0,   68169600 }, // day

                { L_, 1972,  3, 31,  0,  0,  0,   0,  0,   70848000 }, // day
                { L_, 1972,  4, 30,  0,  0,  0,   0,  0,   73440000 }, // day
                { L_, 1972, 12, 31,  0,  0,  0,   0,  0,   94608000 }, // day

                { L_, 1972,  1,  1,  1,  0,  0,   0,  0,   63075600 }, // hour
                { L_, 1972,  1,  1, 23,  0,  0,   0,  0,   63154800 }, // hour

                { L_, 1972,  1,  1,  0,  1,  0,   0,  0,   63072060 }, // min
                { L_, 1972,  1,  1,  0, 59,  0,   0,  0,   63075540 }, // min

                { L_, 1972,  1,  1,  0,  0,  1,   0,  0,   63072001 }, // sec
                { L_, 1972,  1,  1,  0,  0, 59,   0,  0,   63072059 }, // sec

                    // *** The millisecond field is ignored.  ***
                //lin year mon day hou min sec msec   s      result
                //--- ---- --- --- --- --- --- ----   -      ------
                { L_, 1972,  1,  1,  0,  0,  0,   0,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,   1,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,   9,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,  10,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,  90,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0,  99,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0, 100,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0, 900,  0,   63072000 }, // msec
                { L_, 1972,  1,  1,  0,  0,  0, 999,  0,   63072000 }, // msec

                    // *** Time = 24:00:00:000 converts to 00:00:00 ***
                //lin year mon day hou min sec msec   s      result
                //--- ---- --- --- --- --- --- ----   -      ------
                { L_, 1971,  1,  1, 24,  0,  0,   0,  0,   31536000 }, // year
                { L_, 1972,  1,  1, 24,  0,  0,   0,  0,   63072000 }, // year
                { L_, 1973,  1,  1, 24,  0,  0,   0,  0,   94694400 }, // year

                { L_, 1971,  2,  1, 24,  0,  0,   0,  0,   34214400 }, // month
                { L_, 1971,  3,  1, 24,  0,  0,   0,  0,   36633600 }, // month
                { L_, 1971, 12,  1, 24,  0,  0,   0,  0,   60393600 }, // month

                { L_, 1972,  2,  1, 24,  0,  0,   0,  0,   65750400 }, // month
                { L_, 1972,  3,  1, 24,  0,  0,   0,  0,   68256000 }, // month
                { L_, 1972, 12,  1, 24,  0,  0,   0,  0,   92016000 }, // month

                { L_, 1972,  1, 30, 24,  0,  0,   0,  0,   65577600 }, // day
                { L_, 1972,  1, 31, 24,  0,  0,   0,  0,   65664000 }, // day
                { L_, 1972,  2, 29, 24,  0,  0,   0,  0,   68169600 }, // day

                { L_, 1972,  3, 31, 24,  0,  0,   0,  0,   70848000 }, // day
                { L_, 1972,  4, 30, 24,  0,  0,   0,  0,   73440000 }, // day
                { L_, 1972, 12, 31, 24,  0,  0,   0,  0,   94608000 }, // day
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // PERTURBATION: Arbitrary initial time values in order to verify
            //               "No Change" to 'result' on FAILURE.

            const time_t INITIAL_VALUES[] = { // standard b-box int partition
                INT_MIN, INT_MIN + 1, -1, 0, 1, INT_MAX
            };

            int tmp = sizeof INITIAL_VALUES / sizeof *INITIAL_VALUES;
            if (absVeryVerbose > 0) { tmp = ::min(absVeryVerbose, tmp); }
            const int NUM_INITIAL_VALUES = tmp;

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int    LINE = DATA[ti].d_lineNum;
                const int    YEAR = DATA[ti].d_year;
                const int   MONTH = DATA[ti].d_month;
                const int     DAY = DATA[ti].d_day;
                const int    HOUR = DATA[ti].d_hour;
                const int  MINUTE = DATA[ti].d_minute;
                const int  SECOND = DATA[ti].d_second;
                const int    MSEC = DATA[ti].d_millisecond;

                const int  STATUS = DATA[ti].d_status;

                const int    TIME = DATA[ti].d_time;

                if (veryVerbose) {
                    cout << "\n--------------------------------------" << endl;
                    P_(LINE) P_(YEAR) P_(MONTH) P(DAY)
                    T_ P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
                    T_ P_(STATUS) P(TIME)
                }

                const bdet_Datetime INPUT(YEAR, MONTH, DAY,
                                          HOUR, MINUTE, SECOND, MSEC);

                if (veryVerbose) P(INPUT);

                LOOP_ASSERT(LINE, !!STATUS == STATUS); // double check

                for (int vi = 0; vi < NUM_INITIAL_VALUES; ++vi) {
  //v---------------^
    const time_t CONTROL = INITIAL_VALUES[vi];
    if (veryVeryVerbose) { P(CONTROL); }
    time_t result = CONTROL;

    if (veryVerbose) { cout << "Before: "; P(result); }
    int status = bdetu_Datetime::convertToTimeT(&result, INPUT);
    if (veryVerbose) { cout << "After: "; P(result); }

    LOOP4_ASSERT(LINE, vi, STATUS, status, !STATUS == !status); // black-box

    if (STATUS) {
            // *** Bad status implies no change to result ***
        LOOP4_ASSERT(LINE, vi, CONTROL, result, CONTROL == result);
    }
    else {
        LOOP4_ASSERT(LINE, vi, TIME, result, TIME == result);
    }

        // *** REUSE THIS LOOP TO PARTIALLY TEST convertFromTimeT ***

    if (vi)            continue;  // No need to repeat more than once per row.
    if (STATUS)        continue;  // Conversion failed; nothing to reverse.
    if (switches['a']) continue;  // Turn off test for 'convertFromTimeT'

    bdet_Datetime result2(9999, 12, 31, 23, 59, 59, 999); // unreachable

    if (veryVerbose) { cout << "Before: "; P(result2); }
    bdetu_Datetime::convertFromTimeT(&result2, result);
    if (veryVerbose) { cout << "After: "; P(result2); }

        // *** The millisecond field is always set to 0 ***
    if (veryVeryVerbose) P(INPUT);
    bdet_Datetime tmp2(INPUT);
    tmp2.setMillisecond(0);

        // *** There is no equivalent representation for 24:00:00 as a time_t
    tmp2.setHour(tmp2.hour() % 24);

    const bdet_Datetime INPUT2(tmp2);
    if (veryVerbose) P(INPUT2);
    LOOP2_ASSERT(LINE, INPUT2, 0 == INPUT2.millisecond()); // double check

        // *** All relevant fields are recovered properly ***
    LOOP3_ASSERT(LINE, INPUT2, result2, INPUT2 == result2);
  //^---------------v
                } // end for vi
            } // end for ti
        } // end block

        if (!switches['2']) {
            if (verbose) cout << "\ntime_t => bdet_Datetime." << endl;


            static const struct {
                int d_lineNum; // source line number

                int d_input;   // initial time_t value

                int d_year;    // expected field of result
                int d_month;   // expected field of result
                int d_day;     // expected field of result
                int d_hour;    // expected field of result
                int d_minute;  // expected field of result
                int d_second;  // expected field of result

            } DATA[] = {           // <------- result ------->
                //lin   input value   year mon day hou min sec
                //---   -----------   ---- --- --- --- --- ---

                    // *** All relevant fields are recovered properly ***
                //lin   input value   year mon day hou min sec
                //---   -----------   ---- --- --- --- --- ---
                { L_,             1,  1970,  1,  1,  0,  0,  1 }, // sec
                { L_,            59,  1970,  1,  1,  0,  0, 59 },

                { L_,            60,  1970,  1,  1,  0,  1,  0 }, // min
                { L_,          3599,  1970,  1,  1,  0, 59, 59 }, //

                { L_,          3600,  1970,  1,  1,  1,  0,  0 }, // hour
                { L_,         86399,  1970,  1,  1, 23, 59, 59 },

                { L_,         86400,  1970,  1,  2,  0,  0,  0 }, // day
                { L_,       2678399,  1970,  1, 31, 23, 59, 59 },

                { L_,       2678400,  1970,  2,  1,  0,  0,  0 }, // mon
                { L_,      31535999,  1970, 12, 31, 23, 59, 59 },

                { L_,      31536000,  1971,  1,  1,  0,  0,  0 }, // year
                { L_,      63072000,  1972,  1,  1,  0,  0,  0 }, // lp yr.
                { L_,      94694400,  1973,  1,  1,  0,  0,  0 },
                { L_,     126230400,  1974,  1,  1,  0,  0,  0 },

                { L_,     441763200,  1984,  1,  1,  0,  0,  0 }, // decade
                { L_,     757382400,  1994,  1,  1,  0,  0,  0 },
                { L_,    1072915200,  2004,  1,  1,  0,  0,  0 },
                { L_,    1388534400,  2014,  1,  1,  0,  0,  0 },
                { L_,    1704067200,  2024,  1,  1,  0,  0,  0 },
                { L_,    2019686400,  2034,  1,  1,  0,  0,  0 },

                { L_,    2021253247,  2034,  1, 19,  3, 14,  7 }, // year
                { L_,    2052789247,  2035,  1, 19,  3, 14,  7 },
                { L_,    2084325247,  2036,  1, 19,  3, 14,  7 }, // lp yr.
                { L_,    2115947647,  2037,  1, 19,  3, 14,  7 },

                { L_,    2144805247,  2037, 12, 19,  3, 14,  7 }, // mon
                { L_,    2147480047,  2038,  1, 19,  2, 14,  7 }, // hour
                { L_,    2147483587,  2038,  1, 19,  3, 13,  7 }, // min
                { L_,    2147483646,  2038,  1, 19,  3, 14,  6 }, // sec

                { L_,    2147483647,  2038,  1, 19,  3, 14,  7 }, // top
                { L_,       INT_MAX,  2038,  1, 19,  3, 14,  7 },
             };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE = DATA[ti].d_lineNum;

                const int  INPUT = DATA[ti].d_input;

                const int   YEAR = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int    DAY = DATA[ti].d_day;
                const int   HOUR = DATA[ti].d_hour;
                const int MINUTE = DATA[ti].d_minute;
                const int SECOND = DATA[ti].d_second;

                {
                    // Double check that the table itself is right.

                    bdet_Datetime dt(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND);
                    time_t tt;
                    int s = bdetu_Datetime::convertToTimeT(&tt, dt);
                    LOOP2_ASSERT(LINE, s, 0 == s);
                    LOOP3_ASSERT(LINE, tt, INPUT, tt == INPUT);
                }

                bdet_Datetime result(1, 2, 3, 4, 5, 6, 7);

                if (veryVerbose) { cout << "Before: "; P(result) }
                bdetu_Datetime::convertFromTimeT(&result, INPUT);
                if (veryVerbose) { cout << " After: "; P_(result) }
  //v-----------^
        // *** All relevant fields are recovered properly ***
    LOOP3_ASSERT(LINE, YEAR, result.year(), YEAR == result.year());
    LOOP3_ASSERT(LINE, MONTH, result.month(), MONTH == result.month());
    LOOP3_ASSERT(LINE, DAY, result.day(), DAY == result.day());
    LOOP3_ASSERT(LINE, HOUR, result.hour(), HOUR == result.hour());
    LOOP3_ASSERT(LINE, MINUTE, result.minute(), MINUTE == result.minute());
    LOOP3_ASSERT(LINE, SECOND, result.second(), SECOND == result.second());

        // *** The millisecond field is always set to 0 ***
    LOOP2_ASSERT(LINE, result.millisecond(), 0 == result.millisecond());
  //^-----------v
            } // end for ti
        } // end block

        if (!switches['3']) {
            if (verbose) cout <<
                      "\ntime_t => bdet_Datetime => time_t." << endl;

            unsigned int SIZE = unsigned(INT_MAX) + 1;

            if (veryVerbose) P(SIZE);
                                             // to allow abs value to count!
            unsigned int numTrials = wasVeryVeryVerbose
                                     ? absVeryVeryVerbose
                                       ? absVeryVeryVerbose
                                       : SIZE   // default in v-v-verbose mode
                                     : 1000; // default with no v-v-verbose


            const int PRIME = 123456789; // at least relatively prime to size

            ASSERT(SIZE % PRIME);        // ensure SIZE not divisible by PRIME

            double percentCovered = 100 * double(numTrials) / SIZE;

            if (verbose) { T_ P_(numTrials) P(percentCovered) }

            const unsigned int STARTING_VALUE = 0;

            unsigned int pseudoRandomValue = STARTING_VALUE;

            for (unsigned int i = 0; i < numTrials; ++i) {
                if (verbose) loopMeter(i, numTrials);

                // Ensure that there is no premature repetition; ok first time.

                LOOP_ASSERT(i, STARTING_VALUE != pseudoRandomValue != !i);

                time_t currentValue = pseudoRandomValue;
                pseudoRandomValue += PRIME;
                pseudoRandomValue %= SIZE;

                const bdet_Datetime INITIAL_VALUE(
                                1 +  3 * i % 9999,
                                1 +  5 * i % 12,
                                1 +  7 * i % 28,
                                0 + 11 * i % 24,
                                0 + 13 * i % 60,
                                0 + 17 * i % 60,
                                0 + 19 * i % 1000); // pseudo random values
                if (veryVeryVerbose && absVeryVeryVerbose) P(INITIAL_VALUE);
                bdet_Datetime tmp(INITIAL_VALUE);
  //v-----------^
    if (veryVeryVerbose && absVeryVeryVerbose) { cout << "Before: "; P(tmp); }
    bdetu_Datetime::convertFromTimeT(&tmp, currentValue);
    if (veryVeryVerbose && absVeryVeryVerbose) { cout << " After: "; P(tmp); }

    time_t result = -int(i); // out of bounds

    if (veryVeryVerbose && absVeryVeryVerbose) { cout<<"Before: "; P(result); }
    LOOP_ASSERT(i, 0 == bdetu_Datetime::convertToTimeT(&result, tmp));
    if (veryVeryVerbose && absVeryVeryVerbose) { cout<<" After: "; P(result); }

    LOOP_ASSERT(i, currentValue == result);
  //^-----------v
            }
            if (verbose) loopMeter(numTrials, numTrials);
        } // end block
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONVERT BDET_DATETIME TO/FROM STRUCT TM
        //
        // Concerns:
        //   To:
        //     1. All fields are converted properly.
        //     2. The millisecond field is ignored.
        //     3. tm_isdst is always set to -1.
        //     4. Time = 24:00:00:000 converts to 00:00:00, not to 24:00:00.
        //     5. The operation never fails.
        //
        //   From:
        //     1. All relevant fields are converted properly.
        //     2. Irrelevant fields are ignored.
        //     3. The millisecond field is always set to 0.
        //     4. Time = 24:00:00 converts to 24:00:00:000, not to
        //        00:00:00:000.
        //     5. Illegal dates are detected.
        //     6. Out-of-range dates are detected.
        //     7. Leap seconds at the end of the year are treated as failures.
        //     8. Bad status implies no change to result.
        //
        // Plan:
        //   1. Test convertToTm
        //        * Table-Based Implementation
        //        * Category Partitioning Data Selection
        //
        //        Construct a table in which each input field, and each output
        //        field that can differ from its corresponding input are
        //        represented in separate columns.  Note that 'tm_isdst'
        //        is necessarily -1 and is tested directly within the loop.
        //
        //   2. Test convertFromTm
        //        * Table-Based Implementation
        //        * Category Partitioning Data Selection
        //        * Orthogonal Perturbation:
        //              * Irrelevant Input Fields
        //              * Unaltered Initial Values
        //
        //        Construct a table in which each relevant input field, status,
        //        and each output field that can differ from its corresponding
        //        input are represented in separate columns.  A failure status
        //        implies that the result is not changed, which is verified
        //        within the body of the loop on two separate initial values.
        //        Irrelevant fields are modified orthogonally within the loop.
        //        Note that 'millisecond' is necessarily 0 and is tested
        //        directly within the main loop.
        //
        //   3. Test convertToTm and then convertFromTm
        //        * Loop-Based Implementation
        //        * Pseudo-Random Data Selection:
        //            * Exploiting Inverse Relationship
        //
        //        Use a loop-based approach to verify that pseudo-randomly
        //        selected, non-repeating datetime values that can be
        //        converted to and then from 'struct tm' objects result in
        //        exactly the same object.  Note that the intermediate 'tm'
        //        instance is initialized to a different "garbage" value on
        //        each iteration.
        //
        // Command Line:
        //     Numeric verbose switches suppress individual test cases:
        //             verbose switch '1' suppresses block 1.
        //             verbose switch '2' suppresses block 2.
        //             verbose switch '3' suppresses block 3.
        //
        //           veryVerboseArg != 0
        //
        //           veryVerboseArg  < 0  suppresses veryVerbose mode.
        //       abs(veryVerboseArg) > 0  limits # of orthogonal perturbations.
        //
        //       veryVeryVerboseArg  < 0  suppresses veryVeryVerbose mode.
        //   abs(veryVeryVerboseArg) > 0  controls the length of pseudo-random,
        //                                potentially exhaustive generate/parse
        //                                test.
        //
        // Testing:
        //   void convertToTm(tm *result, const bdet_Datetime& datetime);
        //   int convertFromTm(bdet_Datetime *result, const tm& timeStruct);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BDET_DATETIME TO/FROM STRUCT TM" << endl
                          << "===============================" << endl;
#if 0
        if (verbose) cout <<
                "\nProcessing case-specific command-line arguments." << endl;

        static unsigned char switches[128];
        const int argVerbose = parseAbsVerboseArg(&verbose,
                                             -VERBOSE_ARG_NUM, argv, switches);
        const int absVeryVerbose = parseAbsVerboseArg(&veryVerbose,
                                                   VERY_VERBOSE_ARG_NUM, argv);
        const int absVeryVeryVerbose = parseAbsVerboseArg(&veryVeryVerbose,
                                              VERY_VERY_VERBOSE_ARG_NUM, argv);

        if (veryVerbose) {
            T_ P_(verbose) T_ T_ P(argVerbose)
            T_ P_(veryVerbose) T_ P(absVeryVerbose)
            T_ P_(veryVeryVerbose) T_ P(absVeryVeryVerbose)
            for (int i = 0; i < sizeof switches / sizeof *switches; ++i) {
                if (switches[i]) {
                    cout << "\t\tverbose switch '" << char(i) << "': "
                         << (int)switches[i] << endl;
                }
            }
        }

        if (!switches['1']) {
            if (verbose) cout << "\nbdet_Datetime => struct tm." << endl;

            static const struct {
                int d_lineNum;      // source line number

                int d_year;         // input
                int d_month;        // input
                int d_day;          // input
                int d_hour;         // input
                int d_minute;       // input
                int d_second;       // input
                int d_millisecond;  // input

                int d_tm_year;      // output (different from input)
                int d_tm_mon;       // output (different from input)
                int d_tm_hour;      // output (different from input)
                int d_tm_wday;      // output (calculated)
                int d_tm_yday;      // output (calculated)
            } DATA[] = {                            // <- expected output ->
                   // <--------- input ----------->    <----- result ------>
                //lin year mon day hou min sec msec    year mon hou wda yday
                //--- ---- --- --- --- --- --- ----    ---- --- --- --- ----

                    // *** Day, Day-Of-Week, Day-Of-Year, Month, Year ***
                //lin year mon day hou min sec msec    year mon hou wda yday
                //--- ---- --- --- --- --- --- ----    ---- --- --- --- ----
                { L_, 1999, 12, 31,  0,  0,  0,   0,     99, 11,  0,  5, 364 },
                { L_, 2000,  1,  1,  0,  0,  0,   0,    100,  0,  0,  6,   0 },
                { L_, 2000,  1,  2,  0,  0,  0,   0,    100,  0,  0,  0,   1 },
                { L_, 2000,  1,  3,  0,  0,  0,   0,    100,  0,  0,  1,   2 },

                { L_, 2000,  1, 31,  0,  0,  0,   0,    100,  0,  0,  1,  30 },
                { L_, 2000,  2,  1,  0,  0,  0,   0,    100,  1,  0,  2,  31 },
                { L_, 2000,  2, 29,  0,  0,  0,   0,    100,  1,  0,  2,  59 },
                { L_, 2000,  3,  1,  0,  0,  0,   0,    100,  2,  0,  3,  60 },

                { L_, 2000, 11, 30,  0,  0,  0,   0,    100, 10,  0,  4, 334 },
                { L_, 2000, 12,  1,  0,  0,  0,   0,    100, 11,  0,  5, 335 },
                { L_, 2000, 12, 31,  0,  0,  0,   0,    100, 11,  0,  0, 365 },
                { L_, 2001,  1,  1,  0,  0,  0,   0,    101,  0,  0,  1,   0 },

                       // *** Hour, Minute, Second, Milliseconds ***
                //lin year mon day hou min sec msec    year mon hou wda yday
                //--- ---- --- --- --- --- --- ----    ---- --- --- --- ----
                { L_, 2024,  2, 29,  0,  0,  0,   0,    124,  1,  0,  4,  59 },
                { L_, 2024,  2, 29,  1,  0,  0,   0,    124,  1,  1,  4,  59 },
                { L_, 2024,  2, 29, 23,  0,  0,   0,    124,  1, 23,  4,  59 },
                { L_, 2024,  2, 29, 24,  0,  0,   0,    124,  1,  0,  4,  59 },

                { L_, 2024,  2, 29,  0,  1,  0,   0,    124,  1,  0,  4,  59 },
                { L_, 2024,  2, 29,  0, 59,  0,   0,    124,  1,  0,  4,  59 },

                { L_, 2024,  2, 29,  0,  0,  1,   0,    124,  1,  0,  4,  59 },
                { L_, 2024,  2, 29,  0,  0, 59,   0,    124,  1,  0,  4,  59 },

                { L_, 2024,  2, 29,  0,  0,  1,   0,    124,  1,  0,  4,  59 },
                { L_, 2024,  2, 29,  0,  0, 59,   0,    124,  1,  0,  4,  59 },

                { L_, 2024,  2, 29,  0,  0,  0,   1,    124,  1,  0,  4,  59 },
                { L_, 2024,  2, 29,  0,  0,  0,  60,    124,  1,  0,  4,  59 },
                { L_, 2024,  2, 29,  0,  0,  0, 999,    124,  1,  0,  4,  59 },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                const int    LINE = DATA[ti].d_lineNum;
                const int    YEAR = DATA[ti].d_year;
                const int   MONTH = DATA[ti].d_month;
                const int     DAY = DATA[ti].d_day;
                const int    HOUR = DATA[ti].d_hour;
                const int  MINUTE = DATA[ti].d_minute;
                const int  SECOND = DATA[ti].d_second;
                const int    MSEC = DATA[ti].d_millisecond;

                const int TM_YEAR = DATA[ti].d_tm_year;
                const int  TM_MON = DATA[ti].d_tm_mon;
                const int TM_HOUR = DATA[ti].d_tm_hour;
                const int TM_WDAY = DATA[ti].d_tm_wday;
                const int TM_YDAY = DATA[ti].d_tm_yday;

  //v-----------^
    if (veryVerbose) {
        cout << "\n--------------------------------------------------" << endl;
        P_(LINE) P_(YEAR) P_(MONTH) P(DAY)
        T_ P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
        T_ P_(TM_YEAR) P_(TM_MON) P_(TM_WDAY) P(TM_YDAY)
    }
    const bdet_Date DATE(YEAR, MONTH, DAY);
    const bdet_Time TIME(HOUR, MINUTE, SECOND, MSEC);

    const bdet_Datetime INPUT(DATE, TIME);
    if (veryVerbose) P(INPUT);

    tm result;
    memset(&result, char(ti), sizeof result);   // junk

    if (veryVeryVerbose) { cout << "Before: "; P(result.tm_isdst); }
    bdetu_Datetime::convertToTm(&result, INPUT);
    if (veryVeryVerbose) { cout << "After: "; P(result.tm_isdst); }
    if (veryVerbose) dump(cout, result);

    LOOP_ASSERT(LINE, MONTH - 1 == TM_MON);     // double check
    LOOP_ASSERT(LINE, HOUR % 24 == TM_HOUR);    // double check
    LOOP_ASSERT(LINE, YEAR - 1900 == TM_YEAR);  // double check

    LOOP3_ASSERT(LINE, SECOND, result.tm_sec, SECOND == result.tm_sec);
    LOOP3_ASSERT(LINE, MINUTE, result.tm_min, MINUTE == result.tm_min);
    LOOP3_ASSERT(LINE, TM_HOUR, result.tm_hour, TM_HOUR == result.tm_hour);
    LOOP3_ASSERT(LINE, DAY, result.tm_mday, DAY == result.tm_mday);
    LOOP3_ASSERT(LINE, TM_MON, result.tm_mon, TM_MON == result.tm_mon);
    LOOP3_ASSERT(LINE, TM_YEAR, result.tm_year, TM_YEAR == result.tm_year);
    LOOP3_ASSERT(LINE, TM_WDAY, result.tm_wday, TM_WDAY == result.tm_wday);
    LOOP3_ASSERT(LINE, TM_YDAY, result.tm_yday, TM_YDAY == result.tm_yday);

    LOOP2_ASSERT(LINE, result.tm_isdst, -1 == result.tm_isdst);
  //^-----------v
            } // end for ti
        } // end block

        if (!switches['2']) {
            if (verbose) cout << "\nstruct tm => bdet_Datetime." << endl;

            enum { FAILURE = 1 }; // in which case output data is ignored

            static const struct {
                int d_lineNum;      // source line number

                int d_tm_year;      // input
                int d_tm_mon;       // input
                int d_tm_mday;      // input
                int d_tm_hour;      // input
                int d_tm_min;       // input
                int d_tm_sec;       // input

                int d_status;       // value returned by function

                int d_year;         // output (different from input)
                int d_month;        // output (different from input)
                int d_hour;         // output (different from input)


            } DATA[] = {                       // <-- expected ->
                     // <--------- input ------>     <- result ->
                //lin   year mon day hou min sec  s  year mon hou
                //---   ---- --- --- --- --- ---  -  ---- --- ---

                    // *** All relevant fields are converted properly ***
                //lin   year mon day hou min sec  s  year mon hou
                //---   ---- --- --- --- --- ---  -  ---- --- ---
                { L_,    100,  5, 15, 12, 45, 56, 0, 2000,  6, 12 }, // base

                { L_,    101,  5, 15, 12, 45, 56, 0, 2001,  6, 12 }, // year
                { L_,    100,  6, 15, 12, 45, 56, 0, 2000,  7, 12 }, // mon
                { L_,    100,  5, 16, 12, 45, 56, 0, 2000,  6, 12 }, // day
                { L_,    100,  5, 15, 13, 45, 56, 0, 2000,  6, 13 }, // hour
                { L_,    100,  5, 15, 12, 46, 56, 0, 2000,  6, 12 }, // min
                { L_,    100,  5, 15, 12, 45, 57, 0, 2000,  6, 12 }, // sec

                    // *** Time = 24:00:00 converts to 24:00:00:000 ***
                //lin   year mon day hou min sec  s  year mon hou
                //---   ---- --- --- --- --- ---  -  ---- --- ---
                { L_,    100,  0,  1, 24,  0,  0, 0, 2000,  1, 24 }, // year
                { L_,    101,  1,  2, 24,  0,  0, 0, 2001,  2, 24 }, // year
                { L_,    102,  2,  3, 24,  0,  0, 0, 2002,  3, 24 }, // year
                { L_,    103,  3,  4, 24,  0,  0, 0, 2003,  4, 24 }, // year
                { L_,    104, 10, 11, 24,  0,  0, 0, 2004, 11, 24 }, // year
                { L_,    105, 11, 11, 24,  0,  0, 0, 2005, 12, 24 }, // year

                    // *** Out-of-range dates are detected ***
                //lin   year mon day hou min sec  s  year mon hou
                //---   ---- --- --- --- --- ---  -  ---- --- ---
                { L_,  -1900, 11, 31, 24,  0,  0, FAILURE         }, // year
                { L_,  -1900, 11, 31, 23, 59, 59, FAILURE         },
                { L_,  -1900,  0,  1,  0,  0,  0, FAILURE         },

                { L_,  -1899,  0,  1,  0,  0,  0, 0,    1,  1,  0 }, // year
                { L_,  -1899,  0,  1, 24,  0,  0, 0,    1,  1, 24 },
                { L_,  -1899,  0,  1, 23, 59, 59, 0,    1,  1, 23 },

                { L_,   8099,  0,  1,  0,  0,  0, 0, 9999,  1,  0 }, // year
                { L_,   8099, 11, 31, 23, 59, 59, 0, 9999, 12, 23 },
                { L_,   8099, 11, 31, 24,  0,  0, 0, 9999, 12, 24 },

                { L_,   8100, 11, 31, 24,  0,  0, FAILURE         }, // year
                { L_,   8100, 11, 31, 23, 59, 59, FAILURE         },
                { L_,   8100,  0,  1,  0,  0,  0, FAILURE         },

                    // *** Illegal dates are detected ***
                //lin   year mon day hou min sec  s  year mon hou
                //---   ---- --- --- --- --- ---  -  ---- --- ---
                { L_,    100, -1, 15,  0,  0,  0, FAILURE,        }, // month
                { L_,    100,  0, 15,  0,  0,  0, 0, 2000,  1,  0 },
                { L_,    100, 11, 15,  0,  0,  0, 0, 2000, 12,  0 },
                { L_,    100, 12, 15,  0,  0,  0, FAILURE         },

                { L_,    100,  2,  0,  0,  0,  0, FAILURE,        }, // day in
                { L_,    100,  2,  1,  0,  0,  0, 0, 2000,  3,  0 }, // March
                { L_,    100,  2, 31,  0,  0,  0, 0, 2000,  3,  0 },
                { L_,    100,  2, 32,  0,  0,  0, FAILURE,        },

                { L_,    100,  3, 30,  0,  0,  0, 0, 2000,  4,  0 }, // day in
                { L_,    100,  3, 31,  0,  0,  0, FAILURE,        }, // April

                { L_,    103,  1, 28,  0,  0,  0, 0, 2003,  2,  0 }, // Feb.
                { L_,    103,  1, 29,  0,  0,  0, FAILURE         }, // no-leap

                { L_,    104,  1, 29,  0,  0,  0, 0, 2004,  2,  0 }, // Feb.
                { L_,    100,  1, 30,  0,  0,  0, FAILURE         }, // leap-yr

                { L_,      0,  1, 28,  0,  0,  0, 0, 1900,  2,  0 }, // Feb.
                { L_,      0,  1, 29,  0,  0,  0, FAILURE         }, // century

                { L_,    500,  1, 29,  0,  0,  0, 0, 2400,  2,  0 }, // Feb.
                { L_,    500,  1, 30,  0,  0,  0, FAILURE         }, // 400-yr

                { L_,    100,  6, 15, -1, 12, 34, FAILURE         }, // hour
                { L_,    100,  6, 15,  0, 12, 34, 0, 2000,  7,  0 },
                { L_,    100,  6, 15, 23, 12, 34, 0, 2000,  7, 23 },
                { L_,    100,  6, 15, 24,  0, 34, FAILURE         },
                { L_,    100,  6, 15, 24,  0,  0, 0, 2000,  7, 24 },
                { L_,    100,  6, 15, 25,  0,  0, FAILURE         },

                { L_,    100,  6, 15, 12, -1, 34, FAILURE         }, // min
                { L_,    100,  6, 15, 12,  0, 34, 0, 2000,  7, 12 },
                { L_,    100,  6, 15, 12, 59, 34, 0, 2000,  7, 12 },
                { L_,    100,  6, 15, 12, 60, 34, FAILURE         },

                { L_,    100,  6, 15, 12, 34, -1, FAILURE         }, // sec
                { L_,    100,  6, 15, 12, 34,  0, 0, 2000,  7, 12 },
                { L_,    100,  6, 15, 12, 34, 59, 0, 2000,  7, 12 },
                { L_,    100,  6, 15, 12, 34, 60, FAILURE         },

                    // *** Leap seconds at the end of the year fail ***
                //lin   year mon day hou min sec  s  year mon hou
                //---   ---- --- --- --- --- ---  -  ---- --- ---
                { L_,    100, 11, 31, 23, 59, 59, 0, 2000, 12, 23 }, // sec
                { L_,    100, 11, 31, 23, 59, 60, FAILURE         },
                { L_,    100, 11, 31, 23, 59, 61, FAILURE         },
                { L_,    100, 11, 31, 23, 59, 62, FAILURE         },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // PERTURBATION 1: Various values for the ignored fields
            //                 of a tm struct

            static const struct {
                int d_lineNum;      // source line number

                int d_tm_wday;      // ignored
                int d_tm_yday;      // ignored
                int d_tm_isdst;     // ignored

            } IGNORED_FIELDS[] = {

                     // <-------- input --------->
                //lin   tm_wday  tm_yday  tm_isdst
                //---   -------  -------  --------
                { L_,         0,       0,        0 }, // base-line

                { L_,   INT_MIN,       0,        0 }, // tm_wday candidates
                { L_,        -1,       0,        0 },
                { L_,         0,       0,        0 },
                { L_,         1,       0,        0 },
                { L_,         2,       0,        0 },
                { L_,         3,       0,        0 },
                { L_,         4,       0,        0 },
                { L_,         5,       0,        0 },
                { L_,         6,       0,        0 },
                { L_,         7,       0,        0 },
                { L_,   INT_MAX,       0,        0 },

                { L_,         0, INT_MIN,        0 }, // tm_yday candidates
                { L_,         0,      -1,        0 },
                { L_,         0,       0,        0 },
                { L_,         0,       1,        0 },
                { L_,         0,     364,        0 },
                { L_,         0,     365,        0 },
                { L_,         0,     366,        0 },
                { L_,         0, INT_MAX,        0 },

                { L_,         0,       0,  INT_MIN }, // tm_isdst candidates
                { L_,         0,       0,       -2 },
                { L_,         0,       0,       -1 },
                { L_,         0,       0,        0 },
                { L_,         0,       0,        1 },
                { L_,         0,       0,        2 },
                { L_,         0,       0,  INT_MAX },
            };

            int tmp = sizeof IGNORED_FIELDS / sizeof *IGNORED_FIELDS;
            if (absVeryVerbose > 0) { tmp = ::min(absVeryVerbose, tmp); }
            const int NUM_IGNORED_FIELDS = tmp;

            // PERTURBATION 2: Arbitrary initial datetime values in order to
            //                 verify "No Change" to 'result' on FAILURE.

            const bdet_Date D_1(1234, 11, 13);
            const bdet_Date D_2(5678, 12, 14);

            const bdet_Time T_1(1, 2, 3, 4);
            const bdet_Time T_2(5, 6, 7, 8);

            const bdet_Datetime INITIAL_VALUE_1(D_1, T_1);
            const bdet_Datetime INITIAL_VALUE_2(D_2, T_2);

            const bdet_Datetime *const INITIAL_VALUES[] = {
                &INITIAL_VALUE_1, &INITIAL_VALUE_2
            };
            tmp = sizeof INITIAL_VALUES / sizeof *INITIAL_VALUES;
            if (absVeryVerbose > 0) { tmp = ::min(absVeryVerbose, tmp); }
            const int NUM_INITIAL_VALUES = tmp;

            // MAIN TEST-TABLE LOOP

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int    LINE = DATA[ti].d_lineNum;

                const int TM_YEAR = DATA[ti].d_tm_year;
                const int  TM_MON = DATA[ti].d_tm_mon;
                const int TM_MDAY = DATA[ti].d_tm_mday;
                const int TM_HOUR = DATA[ti].d_tm_hour;
                const int  TM_MIN = DATA[ti].d_tm_min;
                const int  TM_SEC = DATA[ti].d_tm_sec;

                const int  STATUS = DATA[ti].d_status;

                const int    YEAR = DATA[ti].d_year;
                const int   MONTH = DATA[ti].d_month;
                const int     DAY = TM_MDAY;            // same
                const int    HOUR = DATA[ti].d_hour;
                const int  MINUTE = TM_MIN;             // same
                const int  SECOND = TM_SEC;             // same

                // *** The millisecond field is always set to 0 ***

                const int    MSEC = 0;                  // zero


                // PERTURBATION 1: some data fields are irrelevant.

                for (int pi = 0; pi < NUM_IGNORED_FIELDS; ++pi) {
                    const int        L2 = IGNORED_FIELDS[pi].d_lineNum;

                    // *** Irrelevant fields are ignored ***

                    const int  TM_WDAY = IGNORED_FIELDS[pi].d_tm_wday;
                    const int  TM_YDAY = IGNORED_FIELDS[pi].d_tm_wday;
                    const int TM_ISDST = IGNORED_FIELDS[pi].d_tm_isdst;

                    if (veryVerbose) {
                        cout << "\n------------------------------------------"
                                "-----------------------------------" << endl;
                        P_(LINE) P_(L2) P_(TM_YEAR) P_(TM_MON) P(TM_MDAY)
                        T_ T_ P_(TM_HOUR) P_(TM_MIN) P(TM_SEC)
                        T_ T_ P_(TM_WDAY) P_(TM_YDAY) P(TM_ISDST)
                        T_ T_ P_(STATUS) P_(YEAR) P_(MONTH) P(DAY)
                        T_ T_ P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
                    }

                    tm in;
                    in.tm_sec   = TM_SEC;
                    in.tm_min   = TM_MIN;
                    in.tm_hour  = TM_HOUR;
                    in.tm_mday  = TM_MDAY;
                    in.tm_mon   = TM_MON;
                    in.tm_year  = TM_YEAR;
                    in.tm_wday  = TM_WDAY;
                    in.tm_yday  = TM_YDAY;
                    in.tm_isdst = TM_ISDST;

                    const tm INPUT(in);
                    if (veryVerbose) P(INPUT);

                    // PERTURBATION 2: have multiple initial result values.

                    for (int vi = 0; vi < NUM_INITIAL_VALUES; ++vi) {
  //v-------------------^
    if (veryVeryVerbose) { P_(vi) P(*INITIAL_VALUES[vi]); }

    bdet_Datetime result = *INITIAL_VALUES[vi];

    if (veryVerbose) { cout << "Before: "; P(result) }

    int status = bdetu_Datetime::convertFromTm(&result, INPUT);

    if (veryVerbose) { cout << " After: "; P_(result) P(status) }

    LOOP4_ASSERT(LINE, L2, vi, status, !STATUS == !status); // black-box

    if (0 != STATUS) {
        LOOP3_ASSERT(LINE, L2, vi, 0 == YEAR);              // double check
        LOOP3_ASSERT(LINE, L2, vi, 0 == MONTH);             // double check
        LOOP3_ASSERT(LINE, L2, vi, 0 == HOUR);              // double check

        // *** Bad status implies no change to result ***

        LOOP4_ASSERT(LINE, L2, vi, result, *INITIAL_VALUES[vi] == result);
        continue;
    }

    LOOP3_ASSERT(LINE, L2, vi, TM_YEAR + 1900 == YEAR);     // double check
    LOOP3_ASSERT(LINE, L2, vi, TM_MON + 1 == MONTH);        // double check
    LOOP3_ASSERT(LINE, L2, vi, TM_HOUR == HOUR);            // double check

    LOOP4_ASSERT(LINE, L2, vi, result.year(), YEAR == result.year());
    LOOP4_ASSERT(LINE, L2, vi, result.month(), MONTH == result.month());
    LOOP4_ASSERT(LINE, L2, vi, result.day(), DAY == result.day());
    LOOP4_ASSERT(LINE, L2, vi, result.hour(), HOUR == result.hour());
    LOOP4_ASSERT(LINE, L2, vi, result.minute(), MINUTE == result.minute());
    LOOP4_ASSERT(LINE, L2, vi, result.second(), SECOND == result.second());
    LOOP4_ASSERT(LINE, L2, vi, result.millisecond(),MSEC==result.millisecond())
  //^-------------------v
                    } // end for vi
                } // end for pi
            } // end for ti
        } // end block

        if (!switches['3']) {
            if (verbose) cout <<
                      "\nbdet_Datetime => struct tm => bdet_Datetime." << endl;

            int numTrials = absVeryVeryVerbose ? absVeryVeryVerbose : 1000;

            const Uint64 SIZE = Uint64(9999) * 12 * 31 * 24 * 60 * 60 + 1;

            if (veryVerbose) P(SIZE);

            const Uint64 Z = 1234567;

            const Uint64 PRIME = Z * Z ; // i.e., relatively prime to SIZE

            ASSERT(SIZE % PRIME);        // ensure SIZE not divisible by PRIME

            double percentCovered = 100 * double(numTrials) / SIZE;

            if (verbose) { T_ P_(numTrials) P(percentCovered) }

            const Uint64 STARTING_VALUE = 0;

            Uint64 pseudoRandomValue = STARTING_VALUE;

            int numInvalid = 0;

            for (int i = 0; i < numTrials; ++i) {
                if (verbose) loopMeter(i, numTrials);

                // Ensure that there is no premature repetition; ok first time.

                LOOP_ASSERT(i, STARTING_VALUE != pseudoRandomValue != !i);

                const Uint64 currentValue = pseudoRandomValue;
                pseudoRandomValue += PRIME;
                pseudoRandomValue %= SIZE;

                Uint64 s  = currentValue;
                Uint64 m  = s  / 60;
                Uint64 h  = m  / 60;
                Uint64 d  = h  / 24;
                Uint64 mo = d  / 31;
                Uint64 y  = mo / 12;

                 s %= 60;
                 m %= 60;
                 h %= 24;
                 d %= 31;
                mo %= 12;
                 y %= 9999;

                ++d;
                ++mo;
                ++y;

                bool isValid = bdet_Datetime::isValid(y, mo, d, h, m, s, 0);
                if (veryVeryVerbose) {
                    P_(isValid) P_(y) P_(mo) P_(d) P_(h) P_(m) P(s)
                }
                numInvalid += !isValid;
                if (!isValid) continue;
                const bdet_Datetime INITIAL_VALUE(y, mo, d, h, m, s);

                tm tmp;
                memset(&tmp, i, sizeof tmp);    // junk
  //v-----------^
    bdetu_Datetime::convertToTm(&tmp, INITIAL_VALUE);

    bdet_Datetime result(1, 2, 3, 4, 5, 6, 7);

    LOOP_ASSERT(i, 0 == bdetu_Datetime::convertFromTm(&result, tmp));

    LOOP_ASSERT(i, INITIAL_VALUE == result);
  //^-----------v
            }
            if (verbose) loopMeter(numTrials, numTrials);

            double percentInvalid = 100 * double(numInvalid) / numTrials;

            if (verbose) { T_ P_(numInvalid) P(percentInvalid)}

        } // end block
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // EPOCH
        //   Test the function that returns the Unix epoch 'bdet_Datetime'.
        //
        // Concerns:
        //   That this function returns the epoch 'bdet_Datetime' in a
        //   thread-safe fashion.
        //
        // Plan:
        //   We know that the implementation is thread-safe by design.
        //   We want to verify only that the correct value is returned.
        //
        // Command Line:
        //   Nothing extra.
        //
        // Testing:
        //   bdet_Datetime epoch();
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "UNIX EPOCH DATETIME" << endl
                          << "===================" << endl;
#if 0
        const bdet_Datetime EPOCH(1970, 1, 1, 0);

        ASSERT(EPOCH == bdetu_Datetime::epoch());
#endif
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case is available to be used as a developers sandbox.
        //
        // Concerns:
        //    None.
        //
        // Plan:
        //    Ad hoc.
        //
        // Command Line:
        //           verbose switch '1' suppresses first breathing test block.
        //
        //             veryVerbose < 0  suppresses veryVerbose mode.
        //        abs(veryVerbose) > 0
        //
        //         veryVeryVerbose < 0  suppresses veryVeryVerbose mode.
        //    abs(veryVeryVerbose) > 0
        //
        // Testing:
        //     BREATHING TEST -- (developer's sandbox)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        if (verbose) cout <<
                "\nProcessing case-specific command-line arguments." << endl;

        static unsigned char switches[128];
        parseAbsVerboseArg(&veryVerbose, -VERBOSE_ARG_NUM, argv, switches);
        parseAbsVerboseArg(&veryVerbose, VERY_VERBOSE_ARG_NUM, argv);
        parseAbsVerboseArg(&veryVeryVerbose, VERY_VERY_VERBOSE_ARG_NUM, argv);
        if (!switches[(int)'1']) {
            if (verbose) cout << "\nFirst breathing test block." << endl;

            const bdet_Datetime  DT1(2004,  1, 31, 12, 30, 59, 0);
            const bdet_Datetime  DT2(2005, 12, 01,  8, 25, 02, 0);

            const char          *IFMX1 = "2004-01-31 12:30:59.00";
            const char          *IFMX2 = "2005-12-01 08:25:02.00";

            {
                char dt[23];

                Util::convertToInformixTime(dt, DT1);
                if (verbose) { T_;  P_(IFMX1)  P(dt); }
                ASSERT(0 == strcmp(dt, IFMX1));

                Util::convertToInformixTime(dt, DT2);
                if (verbose) { T_;  P_(IFMX2)  P(dt); }
                ASSERT(0 == strcmp(dt, IFMX2));
            }

            {
                bsl::string dt;

                Util::convertToInformixTime(&dt, DT1);
                if (verbose) { T_; P(dt); }
                ASSERT(dt == IFMX1);

                Util::convertToInformixTime(&dt, DT2);
                if (verbose) { T_; P(dt); }
                ASSERT(dt == IFMX2);
            }

            {
                bdet_Datetime dt;

                ASSERT(DT1 != dt);
                ASSERT(0 == Util::convertFromInformixTime(&dt, IFMX1));
                if (verbose) { T_; P(dt); }
                ASSERT(DT1 == dt);

                ASSERT(DT2 != dt);
                ASSERT(0 == Util::convertFromInformixTime(&dt, IFMX2));
                if (verbose) { T_; P(dt); }
                ASSERT(DT2 == dt);
            }

            {
                bdet_Datetime dt;

                ASSERT(0 == Util::convertFromInformixTime(&dt, IFMX1));
                if (verbose) { T_; P(dt); }
                ASSERT(DT1 == dt);

                ASSERT(0 == Util::convertFromInformixTime(&dt, IFMX2));
                if (verbose) { T_; P(dt); }
                ASSERT(DT2 == dt);
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
