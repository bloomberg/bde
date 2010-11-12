// bdetu_epoch.t.cpp      -*-C++-*-

#include <bdetu_epoch.h>

#include <bsls_platform.h>
#include <bsls_platformutil.h>

#include <bsl_climits.h>
#include <bsl_cstdlib.h>  // atoi()
#include <bsl_ctime.h>
#include <bsl_iostream.h>
#include <bsl_new.h>
#include <bsl_climits.h>
#include <bsl_cstdlib.h>  // atoi()

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
// use loop-based, statistical methods to ensure inversion property where
// appropriate.
//-----------------------------------------------------------------------------
// [ 1] bdet_Datetime& epoch();
// [ 2] int convertToTimeT(time_t *result, const bdet_Datetime& datetime);
// [ 2] bsl::time_t convertToTimeT(const bdet_Datetime& datetime);
// [ 2] void convertFromTimeT(bdet_Datetime *result, time_t time);
// [ 2] bdet_Datetime convertFromTimeT(bsl::time_t time);
// [ 3] int convertToTimeInterval(bdet_TimeInterval    *result,
//                                const bdet_Datetime&  datetime);
// [ 3] bdet_TimeInterval convertToTimeInterval(const bdet_Datetime& datetime);
// [ 3] void convertFromTimeInterval(bdet_Datetime            *result,
//                                   const bdet_TimeInterval&  timeInterval);
// [ 3] bdet_Datetime convertFromTimeInterval(
//                                      const bdet_TimeInterval& timeInterval);
// [ 4] int convertToDatetimeInterval(bdet_DatetimeInterval *result,
//                                    const bdet_Datetime&   datetime);
// [ 4] bdet_DatetimeInterval convertToDatetimeInterval(
//                                              const bdet_Datetime& datetime);
// [ 4] void convertFromDatetimeInterval(
//                             bdet_Datetime                *result,
//                             const bdet_DatetimeInterval&  datetimeInterval);
// [ 4] bdet_Datetime convertFromDatetimeInterval(
//                              const bdet_DatetimeInterval& datetimeInterval);
//-----------------------------------------------------------------------------
// [ 5] USAGE EXAMPLE
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

typedef bsls_PlatformUtil::Int64 Int64;
typedef bsls_PlatformUtil::Uint64 Uint64;

int epochAddressIsNotZero = 0;
int epochBuffer[10] = {0,0,0,0,0,0,0,0,0,0}; // some zeroes

class EarlyEpochCopier {
  public:
    EarlyEpochCopier() {
        const bdet_Datetime &epoch = bdetu_Epoch::epoch();
        if (0 != &epoch) {  // THIS IS NOT A TYPO!
            epochAddressIsNotZero = 1;
            new((void *)epochBuffer) bdet_Datetime(bdetu_Epoch::epoch());
        }
    }
    static const bdet_Datetime& copiedValue() {
        return *(bdet_Datetime *)epochBuffer;
    }
};

// Direct compilers to move initialization earlier so we can verify the
// access actually works from ALL dynamic initializers.

#define INITATTR

#if defined(BSLS_PLATFORM__CMP_MSVC)
#pragma init_seg(compiler)
#elif defined(BSLS_PLATFORM__CMP_GNU) && defined(BSLS_PLATFORM__OS_LINUX)
#undef INITATTR
#define INITATTR __attribute__((init_priority(101)))
#elif defined(BSLS_PLATFORM__CMP_IBM)
#pragma priority(-2147482623)
#endif

EarlyEpochCopier earlyEpochCopier INITATTR;

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------

//=============================================================================
//                  GLOBAL HELPER CLASSES FOR TESTING
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
        //   This will test the usage example provided in the component header
        //   file.
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
        //   Usage example.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

        {
            if (verbose) cout << "\nUsage Example 1A" << endl;

            bsl::time_t  myStdTime = 946684800;  // January 1st, 2000
            bdet_Datetime datetime;

            bdetu_Epoch::convertFromTimeT(&datetime, myStdTime);

            if (verbose) bsl::cout << datetime << bsl::endl;
        }

        {
            if (verbose) cout << "\nUsage Example 1B" << endl;

            bsl::time_t  myStdTime = 946684800;  // January 1st, 2000

            if (verbose) bsl::cout << bdetu_Epoch::convertFromTimeT(myStdTime)
                                   << bsl::endl;
        }

        {
            if (verbose) cout << "\nUsage Example 2" << endl;

            bdet_Datetime datetime(2000, 1, 1, 0, 0, 0, 0);
            bsl::time_t   myStdTime;

            int status = bdetu_Epoch::convertToTimeT(&myStdTime, datetime);

            if (0 != status) {
                if (verbose) bsl::cout << "Conversion Failure" << bsl::endl;
            }
            else {
                if (verbose) bsl::cout << myStdTime << bsl::endl;
            }
        }

        {
            if (verbose) cout << "\nUsage Example 3" << endl;

            bdet_TimeInterval timeInterval(946684800, 900999999);
                // January 1st, 2000, 900 milliseconds

            bdet_Datetime datetime;

            bdetu_Epoch::convertFromTimeInterval(&datetime, timeInterval);

            if (verbose) bsl::cout << datetime << bsl::endl;
        }

        {
            if (verbose) cout << "\nUsage Example 4" << endl;

            bdet_Datetime     datetime(2000, 1, 1, 0, 0, 0, 900);
            bdet_TimeInterval timeInterval;

            // Works when 'datetime > epoch'.
            int status = bdetu_Epoch::convertToTimeInterval(&timeInterval,
                                                            datetime);

            if (0 != status) {
                if (verbose) bsl::cout << "Conversion Failure" << bsl::endl;
            }
            else {
                if (verbose) bsl::cout << timeInterval << bsl::endl;
            }
        }

        {
            if (verbose) cout << "\nUsage Example 5" << endl;

            bdet_DatetimeInterval datetimeInterval(0, 0, 0, 0, 946684800999LL);
                // January 1st, 2000, 999 milliseconds

            bdet_Datetime datetime;

            bdetu_Epoch::convertFromDatetimeInterval(&datetime,
                                                     datetimeInterval);

            if (verbose) bsl::cout << datetime << bsl::endl;
        }

        {
            if (verbose) cout << "\nUsage Example 6" << endl;

            bdet_Datetime         datetime(2000, 1, 1, 0, 0, 0, 999);
            bdet_DatetimeInterval datetimeInterval;

            // Works when 'datetime > epoch'.
            int status = bdetu_Epoch::convertToDatetimeInterval(
                                                             &datetimeInterval,
                                                             datetime);

            if (0 != status) {
                if (verbose) bsl::cout << "Conversion Failure" << bsl::endl;
            }
            else {
                if (verbose) bsl::cout << datetimeInterval << bsl::endl;
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // CONVERT BDET_DATETIME TO/FROM BDET_DATETIMEINTERVAL
        //
        // Concerns:
        //   To:
        //     1) All fields are converted properly.
        //     2) Time = 24:00:00.000 converts to 00:00:00, not to 24:00:00.
        //     3) Limit values of 'bdet_Datetime' converts properly.
        //     4) Datetime values producing negative 'bdet_DatetimeInterval'
        //        values will fail.
        //     5) Non-zero status value implies no change to result
        //
        //   From:
        //     1) All relevant fields are recovered properly.
        //
        // Plan:
        //   1) Test convertToDatetimeInterval
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
        //        loop will also be used to partially test
        //        'convertFromDatetimeInterval'.
        //
        //   2) Test convertFromDatetimeInterval
        //        * Reuse
        //        * Table-Based Implementation
        //        * Category Partitioning Data Selection
        //        * Exploit proven inverse operation: 'convertToTimeInterval'
        //        * Orthogonal Perturbation:
        //              * Unaltered Initial Values
        //
        //        A) First, REUSE the table used to test
        //           'convertToDatetimeInterval' to reverse every successful
        //           conversion to 'bdet_DatetimeInterval', and compare that
        //           result against the initial input.
        //
        //        B) Second, create a separate table that explicitly converts
        //           valid (non-negative) 'bdet_DatetimeInterval' to
        //           'bdet_Datetime' values with input and individual output
        //           fields represented as separate columns.  For each result,
        //           that the input is the table's expected value is verified
        //           via the proven 'convertToDatetimeInterval';
        //           'convertFromDatetimeInterval' is then applied, and the
        //           result of that calculation is compared with the expected
        //           result values in the table.
        //
        //   3) Test 'convertToDatetimeInterval' and then
        //      'convertFromDatetimeInterval'
        //        * Exploit Inverse Relationship
        //        * Loop-Based Implementation]
        //        * Pseudo-Random Data Selection
        //            * Exploiting Inverse Relationship
        //
        //        Use a loop-based approach to verify that pseudo-randomly
        //        selected, non-repeating datetime values that can be converted
        //        to and then from 'bdet_DatetimeInterval' objects result in
        //        exactly the same object.  Note that the immediate
        //        'bdet_DatetimeInterval' object is initialized to a different
        //        "garbage" value on each iteration.
        //
        // Testing:
        //
        // int convertToDatetimeInterval(bdet_DatetimeInterval *result,
        //                               const bdet_Datetime&   datetime);
        // bdet_DatetimeInterval convertToDatetimeInterval(
        //                                      const bdet_Datetime& datetime);
        // void convertFromDatetimeInterval(
        //                     bdet_Datetime                *result,
        //                     const bdet_DatetimeInterval&  datetimeInterval);
        // bdet_Datetime convertFromDatetimeInterval(
        //                      const bdet_DatetimeInterval& datetimeInterval);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BDET_DATETIME TO/FROM BDET_DATETIMEINTERVAL"
                          << endl
                          << "==========================================="
                          << endl;

        if (verbose) cout << "\nbdet_Datetime => bdet_DatetimeInterval."
                          << endl;

        enum { FAILURE = 1 };

        {

        static const struct {
            int                      d_lineNum;      // source line number

            int                      d_year;         // input
            int                      d_month;        // input
            int                      d_day;          // input
            int                      d_hour;         // input
            int                      d_minute;       // input
            int                      d_second;       // input
            int                      d_millisecond;  // input

            int                      d_status;       // value returned by
                                                     // function

            bsls_PlatformUtil::Int64 d_retmsec;      // output

        } DATA[] = {
               // <---------- input ---------->   <--expected--------->
            //lin year mon day hou min sec msec   s      msec
            //--- ---- --- --- --- --- --- ----   -      ----

                // *** out-of-range input values fail ***      Note:
            //lin year mon day hou min sec msec   s      msec   ld =
            //--- ---- --- --- --- --- --- ----   -      ----   Leap Day
            { L_,    1,  1,  1,  0,  0,  0,   0,  FAILURE, 0 },

            { L_, 1869, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
            { L_, 1879, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
            { L_, 1883, 10, 20, 12, 49, 20, 123,  FAILURE, 0 },
            { L_, 1889, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
            { L_, 1899, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
            { L_, 1909, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
            { L_, 1919, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
            { L_, 1925,  5, 28,  5,  9, 40, 321,  FAILURE, 0 },
            { L_, 1929, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
            { L_, 1939, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
            { L_, 1944,  7,  8, 13, 18, 33, 951,  FAILURE, 0 },
            { L_, 1949, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
            { L_, 1959, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },

            { L_, 1969, 12, 31, 23, 59, 59, 999,  FAILURE, 0 },
            { L_, 1970,  1,  1,  0,  0,  0,   0,  0,       0 }, //  0

            { L_, 1980,  1,  1,  0,  0,  0,   0,  0,
                                              315532800000LL }, //  2
            { L_, 1990,  1,  1,  0,  0,  0,   0,  0,
                                              631152000000LL }, //  5
            { L_, 2000,  1,  1,  0,  0,  0,   0,  0,
                                              946684800000LL }, //  7
            { L_, 2010,  1,  1,  0,  0,  0,   0,  0,
                                             1262304000000LL }, // 10
            { L_, 2020,  1,  1,  0,  0,  0,   0,  0,
                                             1577836800000LL }, // 12
            { L_, 2030,  1,  1,  0,  0,  0,   0,  0,
                                             1893456000000LL }, // 15
            { L_, 2038,  1,  1,  0,  0,  0,   0,  0,
                                             2145916800000LL }, // 17

            { L_, 2038,  1, 19,  0,  0,  0,   0,  0,
                                             2147472000000LL }, // 17
            { L_, 2038,  1, 19,  3,  0,  0,   0,  0,
                                             2147482800000LL }, // 17
            { L_, 2038,  1, 19,  3, 14,  0,   0,  0,
                                             2147483640000LL }, // 17
            { L_, 2038,  1, 19,  3, 14,  7,   0,  0,
                                             2147483647000LL }, // 17
            { L_, 2038,  1, 19,  3, 14,  8,   0,  0,
                                             2147483648000LL }, // 17

            { L_, 2048,  1, 19,  3, 14,  8,   0,  0,
                                             2463016448000LL }, // 19
            { L_, 2058,  1, 19,  3, 14,  8,   0,  0,
                                             2778635648000LL }, // 22
            { L_, 2068,  1, 19,  3, 14,  8,   0,  0,
                                             3094168448000LL }, // 24
            { L_, 2078,  1, 19,  3, 14,  8,   0,  0,
                                             3409787648000LL }, // 27
            { L_, 2088,  1, 19,  3, 14,  8,   0,  0,
                                             3725320448000LL }, // 29
            { L_, 2098,  1, 19,  3, 14,  8,   0,  0,
                                             4040939648000LL }, // 32
            { L_, 2108,  1, 19,  3, 14,  8,   0,  0,
                                             4356386048000LL }, // 33

            { L_, 9999, 12, 31,  0,  0,  0,   0,  0,
                                           253402214400000LL }, // 1947
            { L_, 9999, 12, 31, 23,  0,  0,   0,  0,
                                           253402297200000LL }, // 1947
            { L_, 9999, 12, 31, 23, 59,  0,   0,  0,
                                           253402300740000LL }, // 1947
            { L_, 9999, 12, 31, 23, 59, 59,   0,  0,
                                           253402300799000LL }, // 1947
            { L_, 9999, 12, 31, 23, 59, 59, 999,  0,
                                           253402300799999LL }, // 1947

                // *** All fields are converted properly. ***
            //lin year mon day hou min sec msec   s      msec
            //--- ---- --- --- --- --- --- ----   -      ----
            { L_, 1971,  1,  1,  0,  0,  0,  0,  0, 31536000000LL }, // yr
            { L_, 1972,  1,  1,  0,  0,  0,  0,  0, 63072000000LL }, // yr
            { L_, 1973,  1,  1,  0,  0,  0,  0,  0, 94694400000LL }, // yr

            { L_, 1971,  2,  1,  0,  0,  0,  0,  0, 34214400000LL }, // mon
            { L_, 1971,  3,  1,  0,  0,  0,  0,  0, 36633600000LL }, // mon
            { L_, 1971, 12,  1,  0,  0,  0,  0,  0, 60393600000LL }, // mon

            { L_, 1972,  2,  1,  0,  0,  0,  0,  0, 65750400000LL }, // mon
            { L_, 1972,  3,  1,  0,  0,  0,  0,  0, 68256000000LL }, // mon
            { L_, 1972, 12,  1,  0,  0,  0,  0,  0, 92016000000LL }, // mon

            { L_, 1972,  1, 30,  0,  0,  0,  0,  0, 65577600000LL }, // day
            { L_, 1972,  1, 31,  0,  0,  0,  0,  0, 65664000000LL }, // day
            { L_, 1972,  2, 29,  0,  0,  0,  0,  0, 68169600000LL }, // day

            { L_, 1972,  3, 31,  0,  0,  0,  0,  0, 70848000000LL }, // day
            { L_, 1972,  4, 30,  0,  0,  0,  0,  0, 73440000000LL }, // day
            { L_, 1972, 12, 31,  0,  0,  0,  0,  0, 94608000000LL }, // day

            { L_, 1972,  1,  1,  1,  0,  0,  0,  0, 63075600000LL }, // hr
            { L_, 1972,  1,  1, 23,  0,  0,  0,  0, 63154800000LL }, // hr

            { L_, 1972,  1,  1,  0,  1,  0,  0,  0, 63072060000LL }, // min
            { L_, 1972,  1,  1,  0, 59,  0,  0,  0, 63075540000LL }, // min

            { L_, 1972,  1,  1,  0,  0,  1,  0,  0, 63072001000LL }, // sec
            { L_, 1972,  1,  1,  0,  0, 59,  0,  0, 63072059000LL }, // sec

                // *** The millisecond field is converted properly.  ***
            //lin year mon day hou min sec msec   s      msec
            //--- ---- --- --- --- --- --- ----   -      ----
            { L_, 1972,  1,  1,  0,  0,  0,   0,  0, 63072000000LL }, // ms
            { L_, 1972,  1,  1,  0,  0,  0,   1,  0, 63072000001LL }, // ms
            { L_, 1972,  1,  1,  0,  0,  0,   9,  0, 63072000009LL }, // ms
            { L_, 1972,  1,  1,  0,  0,  0,  10,  0, 63072000010LL }, // ms
            { L_, 1972,  1,  1,  0,  0,  0,  90,  0, 63072000090LL }, // ms
            { L_, 1972,  1,  1,  0,  0,  0,  99,  0, 63072000099LL }, // ms
            { L_, 1972,  1,  1,  0,  0,  0, 100,  0, 63072000100LL }, // ms
            { L_, 1972,  1,  1,  0,  0,  0, 900,  0, 63072000900LL }, // ms
            { L_, 1972,  1,  1,  0,  0,  0, 999,  0, 63072000999LL }, // ms

                // *** Time = 24:00:00:000 converts to 00:00:00 ***
            //lin year mon day hou min sec msec   s      msec
            //--- ---- --- --- --- --- --- ----   -      ----
            { L_, 1970,  1,  1, 24,  0,  0,   0, 0,           0LL }, // limit
            { L_, 1971,  1,  1, 24,  0,  0,   0, 0, 31536000000LL }, // yr
            { L_, 1972,  1,  1, 24,  0,  0,   0, 0, 63072000000LL }, // yr
            { L_, 1973,  1,  1, 24,  0,  0,   0, 0, 94694400000LL }, // yr

            { L_, 1971,  2,  1, 24,  0,  0,   0, 0, 34214400000LL }, // mon
            { L_, 1971,  3,  1, 24,  0,  0,   0, 0, 36633600000LL }, // mon
            { L_, 1971, 12,  1, 24,  0,  0,   0, 0, 60393600000LL }, // mon

            { L_, 1972,  2,  1, 24,  0,  0,   0, 0, 65750400000LL }, // mon
            { L_, 1972,  3,  1, 24,  0,  0,   0, 0, 68256000000LL }, // mon
            { L_, 1972, 12,  1, 24,  0,  0,   0, 0, 92016000000LL }, // mon

            { L_, 1972,  1, 30, 24,  0,  0,   0, 0, 65577600000LL }, // day
            { L_, 1972,  1, 31, 24,  0,  0,   0, 0, 65664000000LL }, // day
            { L_, 1972,  2, 29, 24,  0,  0,   0, 0, 68169600000LL }, // day

            { L_, 1972,  3, 31, 24,  0,  0,   0, 0, 70848000000LL }, // day
            { L_, 1972,  4, 30, 24,  0,  0,   0, 0, 73440000000LL }, // day
            { L_, 1972, 12, 31, 24,  0,  0,   0, 0, 94608000000LL }, // day
            { L_, 9999, 12, 31, 24,  0,  0,   0, 0,
                                                253402214400000LL }, // limit
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // PERTURBATION: Arbitrary initial time values in order to verify
        //               "No Change" to 'result' on FAILURE.

        const bsl::time_t INITIAL_VALUES[] = { // standard b-box int partition
            INT_MIN, INT_MIN + 1, -1, 0, 1, INT_MAX
        };

        int tmp = sizeof INITIAL_VALUES / sizeof *INITIAL_VALUES;

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

            const bsls_PlatformUtil::Int64  RETMSEC = DATA[ti].d_retmsec;

            if (veryVerbose) {
                cout << "\n--------------------------------------" << endl;
                P_(LINE) P_(YEAR) P_(MONTH) P(DAY)
                T_ P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
                T_ P_(STATUS) P(RETMSEC)
            }

            const bdet_Datetime INPUT(YEAR, MONTH, DAY,
                                      HOUR, MINUTE, SECOND, MSEC);
            const bdet_DatetimeInterval OUTPUT(0, 0, 0, 0, RETMSEC);

            if (veryVerbose) P(INPUT);

            LOOP_ASSERT(LINE, !!STATUS == STATUS); // double check

            for (int vi = 0; vi < NUM_INITIAL_VALUES; ++vi) {
  //v-----------^
    const int CONTROL = INITIAL_VALUES[vi];
    if (veryVeryVerbose) { P(CONTROL); }
    bdet_DatetimeInterval result(0, 0, 0, 0, CONTROL);
    const bdet_DatetimeInterval ORIGINAL(0, 0, 0, 0, CONTROL);

    if (veryVerbose) { cout << "Before: "; P(result); }
    int status = bdetu_Epoch::convertToDatetimeInterval(&result, INPUT);// TEST
    if (veryVerbose) { cout << "After: "; P(result); }

    LOOP4_ASSERT(LINE, vi, STATUS, status, !STATUS == !status); // black-box

    bdet_DatetimeInterval resultDup(0, 0, 0, 0, CONTROL);
    if (veryVerbose) { cout << "Before: "; P(resultDup); }
    // This is necessary since the version of 'convertToDatetimeInterval' does
    // not do any error checking.
    if (!STATUS) {
        resultDup = bdetu_Epoch::convertToDatetimeInterval(INPUT);      // TEST
    }
    if (veryVerbose) { cout << "After: "; P(resultDup); }

    if (STATUS) {
            // *** Bad status implies no change to result. ***
        LOOP4_ASSERT(LINE, vi, CONTROL, result, ORIGINAL == result);
        LOOP4_ASSERT(LINE, vi, CONTROL, result, ORIGINAL == resultDup);
    }
    else {
        LOOP4_ASSERT(LINE, vi, OUTPUT, result, OUTPUT == result);
        LOOP4_ASSERT(LINE, vi, OUTPUT, result, OUTPUT == resultDup);
    }

       // *** REUSE THIS LOOP TO PARTIALLY TEST convertFromDatetimeInterval ***

    if (vi)                 continue; // Don't repeat more than once per row.

    if (STATUS)             continue; // Conversion failed; nothing to reverse.

    bdet_Datetime    result2(1, 1, 1, 0, 0, 0, 0); // unreachable
    bdet_Datetime result2Dup(1, 1, 1, 0, 0, 0, 0); // unreachable

    if (veryVerbose) { cout << "Before: "; P(result2); }
    bdetu_Epoch::convertFromDatetimeInterval(&result2, result);         // TEST
    if (veryVerbose) { cout << "After: "; P(result2); }

    if (veryVerbose) { cout << "Before: "; P(result2Dup); }
    result2Dup = bdetu_Epoch::convertFromDatetimeInterval(resultDup);   // TEST
    if (veryVerbose) { cout << "After: "; P(result2Dup); }

    if (veryVeryVerbose) P(INPUT);
    bdet_Datetime tmp2(INPUT);
    tmp2.setHour(tmp2.hour() % 24);

    const bdet_Datetime INPUT2(tmp2);
    if (veryVerbose) P(INPUT2);

        // *** All relevant fields are recovered properly. ***
    LOOP3_ASSERT(LINE, INPUT2, result2, INPUT2 == result2);
    LOOP3_ASSERT(LINE, INPUT2, result2Dup, INPUT2 == result2Dup);
  //^-----------v
            } // end for vi
        } // end for ti

        }

        {

        if (verbose) cout << "\nbdet_DatetimeInterval => bdet_Datetime."
                          << endl;

        static const struct {
            int d_lineNum;  // source line number

            bsls_PlatformUtil::Int64 d_input_ms;   // initial time_t value

            int d_year;    // expected field of result
            int d_month;   // expected field of result
            int d_day;     // expected field of result
            int d_hour;    // expected field of result
            int d_minute;  // expected field of result
            int d_second;  // expected field of result
            int d_msec;    // expected field of result

        } DATA[] = {           // <------- result ------->
            //lin   input value   year mon day hou min sec
            //---   -----------   ---- --- --- --- --- ---

                // *** All relevant fields are recovered properly. ***
            //lin       input (ms)  year mon day hou min sec  ms
            //---   --------------  ---- --- --- --- --- --- ---
            { L_,             1000, 1970,  1,  1,  0,  0,  1, 0 }, // sec
            { L_,            59000, 1970,  1,  1,  0,  0, 59, 0 },

            { L_,            60000, 1970,  1,  1,  0,  1,  0, 0 }, // min
            { L_,          3599000, 1970,  1,  1,  0, 59, 59, 0 }, //

            { L_,          3600000, 1970,  1,  1,  1,  0,  0, 0 }, // hour
            { L_,         86399000, 1970,  1,  1, 23, 59, 59, 0 },

            { L_,         86400000, 1970,  1,  2,  0,  0,  0, 0 }, // day
            { L_,       2147483647, 1970,  1, 25, 20, 31, 23,
                                                            647 }, // int
            { L_,          INT_MAX, 1970,  1, 25, 20, 31, 23,
                                                            647 },
            { L_,     2678399000LL, 1970,  1, 31, 23, 59, 59, 0 },

            { L_,     2678400000LL, 1970,  2,  1,  0,  0,  0, 0 }, // mon
            { L_,    31535999000LL, 1970, 12, 31, 23, 59, 59, 0 },

            { L_,    31536000000LL, 1971,  1,  1,  0,  0,  0, 0 }, // year
            { L_,    63072000000LL, 1972,  1,  1,  0,  0,  0, 0 }, // leap
            { L_,    94694400000LL, 1973,  1,  1,  0,  0,  0, 0 },
            { L_,   126230400000LL, 1974,  1,  1,  0,  0,  0, 0 },

            { L_,   441763200000LL, 1984,  1,  1,  0,  0,  0, 0 }, // dec.
            { L_,   757382400000LL, 1994,  1,  1,  0,  0,  0, 0 },
            { L_,  1072915200000LL, 2004,  1,  1,  0,  0,  0, 0 },
            { L_,  1388534400000LL, 2014,  1,  1,  0,  0,  0, 0 },
            { L_,  1704067200000LL, 2024,  1,  1,  0,  0,  0, 0 },
            { L_,  2019686400000LL, 2034,  1,  1,  0,  0,  0, 0 },

            { L_,  2021253247000LL, 2034,  1, 19,  3, 14,  7, 0 }, // year
            { L_,  2052789247000LL, 2035,  1, 19,  3, 14,  7, 0 },
            { L_,  2084325247000LL, 2036,  1, 19,  3, 14,  7, 0 }, // leap
            { L_,  2115947647000LL, 2037,  1, 19,  3, 14,  7, 0 },

            { L_,  2144805247000LL, 2037, 12, 19,  3, 14,  7, 0 }, // mon
            { L_,  2147480047000LL, 2038,  1, 19,  2, 14,  7, 0 }, // hour
            { L_,  2147483587000LL, 2038,  1, 19,  3, 13,  7, 0 }, // min
            { L_,  2147483646000LL, 2038,  1, 19,  3, 14,  6, 0 }, // sec

            { L_,  2463016448000LL, 2048,  1, 19,  3, 14,  8, 0 },
            { L_,  2778635648000LL, 2058,  1, 19,  3, 14,  8, 0 },
            { L_,  3094168448000LL, 2068,  1, 19,  3, 14,  8, 0 },
            { L_,  3409787648000LL, 2078,  1, 19,  3, 14,  8, 0 },
            { L_,  3725320448000LL, 2088,  1, 19,  3, 14,  8, 0 },
            { L_,  4040939648000LL, 2098,  1, 19,  3, 14,  8, 0 },
            { L_,  4356386048000LL, 2108,  1, 19,  3, 14,  8, 0 },
            { L_,253402214400000LL, 9999, 12, 31,  0,  0,  0, 0 },
            { L_,253402297200000LL, 9999, 12, 31, 23,  0,  0, 0 },
            { L_,253402300740000LL, 9999, 12, 31, 23, 59,  0, 0 },
            { L_,253402300799000LL, 9999, 12, 31, 23, 59, 59, 0 }, // MAX

            { L_,    63072000000LL, 1972,  1,  1,  0,  0,  0, 0 }, // ms
            { L_,    63072000001LL, 1972,  1,  1,  0,  0,  0, 1 },
            { L_,    63072000009LL, 1972,  1,  1,  0,  0,  0, 9 },
            { L_,    63072000010LL, 1972,  1,  1,  0,  0,  0, 10 },
            { L_,    63072000090LL, 1972,  1,  1,  0,  0,  0, 90 },
            { L_,    63072000100LL, 1972,  1,  1,  0,  0,  0, 100 },
            { L_,    63072000900LL, 1972,  1,  1,  0,  0,  0, 900 },
            { L_,    63072000999LL, 1972,  1,  1,  0,  0,  0, 999 },
         };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // MAIN TEST-TABLE LOOP

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;

            const bsls_PlatformUtil::Int64 INPUT_MS = DATA[ti].d_input_ms;

            const int                         YEAR = DATA[ti].d_year;
            const int                        MONTH = DATA[ti].d_month;
            const int                          DAY = DATA[ti].d_day;
            const int                         HOUR = DATA[ti].d_hour;
            const int                       MINUTE = DATA[ti].d_minute;
            const int                       SECOND = DATA[ti].d_second;
            const int                         MSEC = DATA[ti].d_msec;

            if (veryVerbose) {
                T_ P(INPUT_MS)
            }

            bdet_DatetimeInterval INPUT(0, 0, 0, 0, INPUT_MS);

            {
                // Double check that the table itself is right.
                bdet_Datetime dt(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);
                bdet_DatetimeInterval ti;
                int s = bdetu_Epoch::convertToDatetimeInterval(&ti, dt);
                LOOP2_ASSERT(LINE, s, 0 == s);
                LOOP3_ASSERT(LINE, ti, INPUT, ti == INPUT);
            }

            bdet_Datetime result(1, 2, 3, 4, 5, 6, 7);
            bdet_Datetime resultDup(1, 2, 3, 4, 5, 6, 7);

            if (veryVerbose) { cout << "Before: "; P(result) }
            bdetu_Epoch::convertFromDatetimeInterval(&result, INPUT);   // TEST
            if (veryVerbose) { cout << " After: "; P_(result) }

            if (veryVerbose) { cout << "Before: "; P(resultDup) }
            resultDup = bdetu_Epoch::convertFromDatetimeInterval(INPUT);// TEST
            if (veryVerbose) { cout << " After: "; P_(resultDup) }
  //v-------^
        // *** All relevant fields are recovered properly. ***
    LOOP3_ASSERT(LINE, YEAR, result.year(), YEAR == result.year());
    LOOP3_ASSERT(LINE, MONTH, result.month(), MONTH == result.month());
    LOOP3_ASSERT(LINE, DAY, result.day(), DAY == result.day());
    LOOP3_ASSERT(LINE, HOUR, result.hour(), HOUR == result.hour());
    LOOP3_ASSERT(LINE, MINUTE, result.minute(), MINUTE == result.minute());
    LOOP3_ASSERT(LINE, SECOND, result.second(), SECOND == result.second());
    LOOP3_ASSERT(LINE, MSEC, result.millisecond(),
                                                 MSEC == result.millisecond());

    LOOP3_ASSERT(LINE, YEAR, resultDup.year(), YEAR == resultDup.year());
    LOOP3_ASSERT(LINE, MONTH, resultDup.month(), MONTH == resultDup.month());
    LOOP3_ASSERT(LINE, DAY, resultDup.day(), DAY == resultDup.day());
    LOOP3_ASSERT(LINE, HOUR, resultDup.hour(), HOUR == resultDup.hour());
    LOOP3_ASSERT(LINE, MINUTE, resultDup.minute(),
                                                 MINUTE == resultDup.minute());
    LOOP3_ASSERT(LINE, SECOND, resultDup.second(),
                                                 SECOND == resultDup.second());
    LOOP3_ASSERT(LINE, MSEC, resultDup.millisecond(),
                                              MSEC == resultDup.millisecond());
  //^-------v
        } // end for ti

        }

        {

        if (verbose) cout <<
                  "\nbdet_DatetimeInterval => bdet_Datetime" <<
                  "=> bdet_DatetimeInterval." << endl;

        unsigned int SIZE = unsigned(INT_MAX) + 1;

        if (veryVerbose) P(SIZE);

        unsigned int numTrials = 1000; // default with no v-v-verbose

        const int PRIME = 123456789; // at least relatively prime to size

        ASSERT(SIZE % PRIME);        // ensure SIZE not divisible by PRIME

        double percentCovered = 100 * double(numTrials) / SIZE;

        if (verbose) { T_ P_(numTrials) P(percentCovered) }

        const unsigned int STARTING_VALUE = 0;

        unsigned int pseudoRandomValue = STARTING_VALUE;

        for (unsigned int i = 0; i < numTrials; ++i) {

            // Ensure that there is no premature repetition; ok first time.

            LOOP_ASSERT(i, STARTING_VALUE != pseudoRandomValue != !i);

            bdet_DatetimeInterval currentValue(0, 0, 0, 0,
                                               pseudoRandomValue);
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
            if (veryVeryVerbose) P(INITIAL_VALUE);
            bdet_Datetime tmp(INITIAL_VALUE);
            bdet_Datetime tmpDup(INITIAL_VALUE);
  //v-------^
    if (veryVeryVerbose) { cout << "Before: "; P(tmp); }
    bdetu_Epoch::convertFromDatetimeInterval(&tmp, currentValue);
    if (veryVeryVerbose) { cout << " After: "; P(tmp); }

    if (veryVeryVerbose) { cout << "Before: "; P(tmpDup); }
    tmpDup = bdetu_Epoch::convertFromDatetimeInterval(currentValue);
    if (veryVeryVerbose) { cout << " After: "; P(tmpDup); }

    bdet_DatetimeInterval result(-1, -1, -1, -1, -1);    // out of bounds
    bdet_DatetimeInterval resultDup(-1, -1, -1, -1, -1); // out of bounds

    if (veryVeryVerbose) { cout<<"Before: "; P(result); }
    LOOP_ASSERT(i, 0 == bdetu_Epoch::convertToDatetimeInterval(&result, tmp));
    if (veryVeryVerbose) { cout<<" After: "; P(result); }

    if (veryVeryVerbose) { cout<<"Before: "; P(resultDup); }
    resultDup = bdetu_Epoch::convertToDatetimeInterval(tmpDup);
    if (veryVeryVerbose) { cout<<" After: "; P(resultDup); }

    LOOP_ASSERT(i, currentValue == result);
    LOOP_ASSERT(i, currentValue == resultDup);
  //^-------v
        }

        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONVERT BDET_DATETIME TO/FROM BDET_TIMEINTERVAL
        //
        // Concerns:
        //   To:
        //     1) All fields are converted properly.
        //     2) Time = 24:00:00:000 converts to 00:00:00, not to 24:00:00.
        //     3) Limit values of 'bdet_Datetime' converts properly.
        //     4) Datetime values producing negative 'bdet_TimeInterval'
        //        values will fail.
        //     5) Non-zero status value implies no change to result
        //
        //   From:
        //     1) All relevant fields are recovered properly.
        //
        // Plan:
        //   1) Test convertToTimeInterval
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
        //        loop will also be used to partially test
        //        'convertFromTimeInterval'.
        //
        //   2) Test convertFromTimeInterval
        //        * Reuse
        //        * Table-Based Implementation
        //        * Category Partitioning Data Selection
        //        * Exploit proven inverse operation: 'convertToTimeInterval'
        //        * Orthogonal Perturbation:
        //              * Unaltered Initial Values
        //
        //        A) First, REUSE the table used to test
        //           'convertToTimeInterval' to reverse every successful
        //           conversion to 'time_t', and compare that result against
        //           the initial input.
        //
        //        B) Second, create a separate table that explicitly converts
        //           valid (non-negative) 'bdet_TimeInterval' to
        //           'bdet_Datetime' values with input and individual output
        //           fields represented as separate columns.  For each result,
        //           that the input is the table's expected value is verified
        //           via the proven 'convertToTimeInterval';
        //           'convertFromTimeInterval' is then applied, and the result
        //           of that calculation is compared with the expected result
        //           values in the table.
        //
        //   3) Test 'convertToTimeInterval' and then 'convertFromTimeInterval'
        //        * Exploit Inverse Relationship
        //        * Loop-Based Implementation
        //        * Pseudo-Random Data Selection
        //            * Exploiting Inverse Relationship
        //
        //        Use a loop-based approach to verify that pseudo-randomly
        //        selected, non-repeating datetime values that can be
        //        converted to and then from 'bdet_TimeInterval' objects result
        //        in exactly the same object.  Note that the intermediate
        //        'bdet_TimeInterval' instance is initialized to a different
        //        "garbage" value on each iteration.
        //
        // Testing:
        //
        // void convertToTimeInterval(bdet_TimeInterval    *result,
        //                            const bdet_Datetime&  datetime)
        // bdet_TimeInterval convertToTimeInterval(
        //                                       const bdet_Datetime& datetime)
        // void convertFromTimeInterval(bdet_Datetime            *result,
        //                              const bdet_TimeInterval&  timeInterval)
        // bdet_Datetime convertFromTimeInterval(
        //                               const bdet_TimeInterval& timeInterval)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BDET_DATETIME TO/FROM BDET_TIMEINTERVAL" << endl
                          << "=======================================" << endl;

        enum { FAILURE = 1 };

        {

        if (verbose) cout << "\nbdet_Datetime => bdet_TimeInterval." << endl;

        static const struct {
            int                      d_lineNum;      // source line number

            int                      d_year;         // input
            int                      d_month;        // input
            int                      d_day;          // input
            int                      d_hour;         // input
            int                      d_minute;       // input
            int                      d_second;       // input
            int                      d_millisecond;  // input

            int                      d_status;       // value returned by
                                                     // function

            bsls_PlatformUtil::Int64 d_retsec;       // output
            int                      d_retnsec;      // output

        } DATA[] = {
               // <---------- input ---------->   <--expected------------>
            //lin year mon day hou min sec msec   s      sec  nsec
            //--- ---- --- --- --- --- --- ----   -      ---  ----

                // *** out-of-range input values fail ***          Note:
            //lin year mon day hou min sec msec   s      sec  nsec ld =
            //--- ---- --- --- --- --- --- ----   -      ---  ---- Leap Day
            { L_,    1,  1,  1,  0,  0,  0,   0,  FAILURE, 0,    0 },

            { L_, 1869, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
            { L_, 1879, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
            { L_, 1883, 10, 20, 12, 49, 20, 123,  FAILURE, 0,    0 },
            { L_, 1889, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
            { L_, 1899, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
            { L_, 1909, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
            { L_, 1919, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
            { L_, 1925,  5, 28,  5,  9, 40, 321,  FAILURE, 0,    0 },
            { L_, 1929, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
            { L_, 1939, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
            { L_, 1944,  7,  8, 13, 18, 33, 951,  FAILURE, 0,    0 },
            { L_, 1949, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
            { L_, 1959, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },

            { L_, 1969, 12, 31, 23, 59, 59, 999,  FAILURE, 0,    0 },
            { L_, 1970,  1,  1,  0,  0,  0,   0,  0,       0,    0 }, //  0

            { L_, 1980,  1,  1,  0,  0,  0,   0,  0,  315532800, 0 }, //  2
            { L_, 1990,  1,  1,  0,  0,  0,   0,  0,  631152000, 0 }, //  5
            { L_, 2000,  1,  1,  0,  0,  0,   0,  0,  946684800, 0 }, //  7
            { L_, 2010,  1,  1,  0,  0,  0,   0,  0, 1262304000, 0 }, // 10
            { L_, 2020,  1,  1,  0,  0,  0,   0,  0, 1577836800, 0 }, // 12
            { L_, 2030,  1,  1,  0,  0,  0,   0,  0, 1893456000, 0 }, // 15
            { L_, 2038,  1,  1,  0,  0,  0,   0,  0, 2145916800, 0 }, // 17

            { L_, 2038,  1, 19,  0,  0,  0,   0,  0, 2147472000, 0 }, // 17
            { L_, 2038,  1, 19,  3,  0,  0,   0,  0, 2147482800, 0 }, // 17
            { L_, 2038,  1, 19,  3, 14,  0,   0,  0, 2147483640, 0 }, // 17
            { L_, 2038,  1, 19,  3, 14,  7,   0,  0, 2147483647, 0 }, // 17
            { L_, 2038,  1, 19,  3, 14,  8,   0,  0, 2147483648, 0 }, // 17

            { L_, 2048,  1, 19,  3, 14,  8,   0,  0, 2463016448LL,
                                                                 0}, // 19
            { L_, 2058,  1, 19,  3, 14,  8,   0,  0, 2778635648LL,
                                                                 0}, // 22
            { L_, 2068,  1, 19,  3, 14,  8,   0,  0, 3094168448LL,
                                                                 0}, // 24
            { L_, 2078,  1, 19,  3, 14,  8,   0,  0, 3409787648LL,
                                                                 0}, // 27
            { L_, 2088,  1, 19,  3, 14,  8,   0,  0, 3725320448LL,
                                                                 0}, // 29
            { L_, 2098,  1, 19,  3, 14,  8,   0,  0, 4040939648LL,
                                                                 0}, // 32
            { L_, 2108,  1, 19,  3, 14,  8,   0,  0, 4356386048LL,
                                                                 0}, // 33

            { L_, 9999, 12, 31,  0,  0,  0,   0,  0, 253402214400LL,
                                                               0 }, // 1947
            { L_, 9999, 12, 31, 23,  0,  0,   0,  0, 253402297200LL,
                                                               0 }, // 1947
            { L_, 9999, 12, 31, 23, 59,  0,   0,  0, 253402300740LL,
                                                               0 }, // 1947
            { L_, 9999, 12, 31, 23, 59, 59,   0,  0, 253402300799LL,
                                                               0 }, // 1947
            { L_, 9999, 12, 31, 23, 59, 59, 999,  0, 253402300799LL,
                                                       999000000 }, // 1947

                // *** All fields are converted properly. ***
            //lin year mon day hou min sec msec  s      sec  nsec
            //--- ---- --- --- --- --- --- ----  -      ---  ----
            { L_, 1971,  1,  1,  0,  0,  0,  0,  0, 31536000, 0 }, // year
            { L_, 1972,  1,  1,  0,  0,  0,  0,  0, 63072000, 0 }, // year
            { L_, 1973,  1,  1,  0,  0,  0,  0,  0, 94694400, 0 }, // year

            { L_, 1971,  2,  1,  0,  0,  0,  0,  0, 34214400, 0 }, // month
            { L_, 1971,  3,  1,  0,  0,  0,  0,  0, 36633600, 0 }, // month
            { L_, 1971, 12,  1,  0,  0,  0,  0,  0, 60393600, 0 }, // month

            { L_, 1972,  2,  1,  0,  0,  0,  0,  0, 65750400, 0 }, // month
            { L_, 1972,  3,  1,  0,  0,  0,  0,  0, 68256000, 0 }, // month
            { L_, 1972, 12,  1,  0,  0,  0,  0,  0, 92016000, 0 }, // month

            { L_, 1972,  1, 30,  0,  0,  0,  0,  0, 65577600, 0 }, // day
            { L_, 1972,  1, 31,  0,  0,  0,  0,  0, 65664000, 0 }, // day
            { L_, 1972,  2, 29,  0,  0,  0,  0,  0, 68169600, 0 }, // day

            { L_, 1972,  3, 31,  0,  0,  0,  0,  0, 70848000, 0 }, // day
            { L_, 1972,  4, 30,  0,  0,  0,  0,  0, 73440000, 0 }, // day
            { L_, 1972, 12, 31,  0,  0,  0,  0,  0, 94608000, 0 }, // day

            { L_, 1972,  1,  1,  1,  0,  0,  0,  0, 63075600, 0 }, // hour
            { L_, 1972,  1,  1, 23,  0,  0,  0,  0, 63154800, 0 }, // hour

            { L_, 1972,  1,  1,  0,  1,  0,  0,  0, 63072060, 0 }, // min
            { L_, 1972,  1,  1,  0, 59,  0,  0,  0, 63075540, 0 }, // min

            { L_, 1972,  1,  1,  0,  0,  1,  0,  0, 63072001, 0 }, // sec
            { L_, 1972,  1,  1,  0,  0, 59,  0,  0, 63072059, 0 }, // sec

                // *** The millisecond field is converted properly.  ***
            //lin year mon day hou min sec msec   s       sec  nsec
            //--- ---- --- --- --- --- --- ----   -       ---  ----
            { L_, 1972,  1,  1,  0,  0,  0,   0,  0, 63072000, 0 }, // msec
            { L_, 1972,  1,  1,  0,  0,  0,   1,  0, 63072000,
                                                         1000000 }, // msec
            { L_, 1972,  1,  1,  0,  0,  0,   9,  0, 63072000,
                                                         9000000 }, // msec
            { L_, 1972,  1,  1,  0,  0,  0,  10,  0, 63072000,
                                                        10000000 }, // msec
            { L_, 1972,  1,  1,  0,  0,  0,  90,  0, 63072000,
                                                        90000000 }, // msec
            { L_, 1972,  1,  1,  0,  0,  0,  99,  0, 63072000,
                                                        99000000 }, // msec
            { L_, 1972,  1,  1,  0,  0,  0, 100,  0, 63072000,
                                                       100000000 }, // msec
            { L_, 1972,  1,  1,  0,  0,  0, 900,  0, 63072000,
                                                       900000000 }, // msec
            { L_, 1972,  1,  1,  0,  0,  0, 999,  0, 63072000,
                                                       999000000 }, // msec

                // *** Time = 24:00:00:000 converts to 00:00:00 ***
            //lin year mon day hou min sec msec  s       sec  nsec
            //--- ---- --- --- --- --- --- ----  -       ---  ----
            { L_, 1970,  1,  1, 24,  0,  0,   0, 0,        0, 0 }, // year
            { L_, 1971,  1,  1, 24,  0,  0,   0, 0, 31536000, 0 }, // year
            { L_, 1972,  1,  1, 24,  0,  0,   0, 0, 63072000, 0 }, // year
            { L_, 1973,  1,  1, 24,  0,  0,   0, 0, 94694400, 0 }, // year

            { L_, 1971,  2,  1, 24,  0,  0,   0, 0, 34214400, 0 }, // month
            { L_, 1971,  3,  1, 24,  0,  0,   0, 0, 36633600, 0 }, // month
            { L_, 1971, 12,  1, 24,  0,  0,   0, 0, 60393600, 0 }, // month

            { L_, 1972,  2,  1, 24,  0,  0,   0, 0, 65750400, 0 }, // month
            { L_, 1972,  3,  1, 24,  0,  0,   0, 0, 68256000, 0 }, // month
            { L_, 1972, 12,  1, 24,  0,  0,   0, 0, 92016000, 0 }, // month

            { L_, 1972,  1, 30, 24,  0,  0,   0, 0, 65577600, 0 }, // day
            { L_, 1972,  1, 31, 24,  0,  0,   0, 0, 65664000, 0 }, // day
            { L_, 1972,  2, 29, 24,  0,  0,   0, 0, 68169600, 0 }, // day

            { L_, 1972,  3, 31, 24,  0,  0,   0, 0, 70848000, 0 }, // day
            { L_, 1972,  4, 30, 24,  0,  0,   0, 0, 73440000, 0 }, // day
            { L_, 1972, 12, 31, 24,  0,  0,   0, 0, 94608000, 0 }, // day
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // PERTURBATION: Arbitrary initial time values in order to verify
        //               "No Change" to 'result' on FAILURE.

        const bsl::time_t INITIAL_VALUES[] = { // standard b-box int partition
            INT_MIN, INT_MIN + 1, -1, 0, 1, INT_MAX
        };

        int tmp = sizeof INITIAL_VALUES / sizeof *INITIAL_VALUES;

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

            const bsls_PlatformUtil::Int64  RETSEC = DATA[ti].d_retsec;
            const int                      RETNSEC = DATA[ti].d_retnsec;

            if (veryVerbose) {
                cout << "\n--------------------------------------" << endl;
                P_(LINE) P_(YEAR) P_(MONTH) P(DAY)
                T_ P_(HOUR) P_(MINUTE) P_(SECOND) P(MSEC)
                T_ P_(STATUS) P_(RETSEC) P(RETNSEC)
            }

            const bdet_Datetime INPUT(YEAR, MONTH, DAY,
                                      HOUR, MINUTE, SECOND, MSEC);
            const bdet_TimeInterval OUTPUT(RETSEC, RETNSEC);

            if (veryVerbose) P(INPUT);

            LOOP_ASSERT(LINE, !!STATUS == STATUS); // double check

            for (int vi = 0; vi < NUM_INITIAL_VALUES; ++vi) {
  //v-----------^
    const int CONTROL = INITIAL_VALUES[vi];
    if (veryVeryVerbose) { P(CONTROL); }
    bdet_TimeInterval result(CONTROL, 0);
    const bdet_TimeInterval ORIGINAL(CONTROL, 0);

    if (veryVerbose) { cout << "Before: "; P(result); }
    int status = bdetu_Epoch::convertToTimeInterval(&result, INPUT);
    if (veryVerbose) { cout << "After: "; P(result); }

    LOOP4_ASSERT(LINE, vi, STATUS, status, !STATUS == !status); // black-box

    bdet_TimeInterval resultDup(CONTROL, 0);
    if (veryVerbose) { cout << "Before: "; P(resultDup); }
    // This is necessary since the version of 'convertToTimeInterval' does not
    // do any error checking.
    if (!STATUS) {
        resultDup = bdetu_Epoch::convertToTimeInterval(INPUT);
    }
    if (veryVerbose) { cout << "After: "; P(resultDup); }

    if (STATUS) {
            // *** Bad status implies no change to result. ***
        LOOP4_ASSERT(LINE, vi, CONTROL, result, ORIGINAL == result);
        LOOP4_ASSERT(LINE, vi, CONTROL, result, ORIGINAL == resultDup);
    }
    else {
        LOOP4_ASSERT(LINE, vi, OUTPUT, result, OUTPUT == result);
        LOOP4_ASSERT(LINE, vi, OUTPUT, result, OUTPUT == resultDup);
    }

        // *** REUSE THIS LOOP TO PARTIALLY TEST convertFromTimeInterval ***

    if (vi)                 continue; // Don't repeat more than once per row.

    if (STATUS)             continue; // Conversion failed; nothing to reverse.

    bdet_Datetime    result2(1, 1, 1, 0, 0, 0, 0); // unreachable
    bdet_Datetime result2Dup(1, 1, 1, 0, 0, 0, 0); // unreachable

    if (veryVerbose) { cout << "Before: "; P(result2); }
    bdetu_Epoch::convertFromTimeInterval(&result2, result);
    if (veryVerbose) { cout << "After: "; P(result2); }

    if (veryVerbose) { cout << "Before: "; P(result2Dup); }
    result2Dup = bdetu_Epoch::convertFromTimeInterval(resultDup);
    if (veryVerbose) { cout << "After: "; P(result2Dup); }

    if (veryVeryVerbose) P(INPUT);
    bdet_Datetime tmp2(INPUT);
    tmp2.setHour(tmp2.hour() % 24);

    const bdet_Datetime INPUT2(tmp2);
    if (veryVerbose) P(INPUT2);

        // *** All relevant fields are recovered properly. ***
    LOOP3_ASSERT(LINE, INPUT2, result2, INPUT2 == result2);
    LOOP3_ASSERT(LINE, INPUT2, result2Dup, INPUT2 == result2Dup);
  //^-----------v
            } // end for vi
        } // end for ti

        }

        {

        if (verbose) cout << "\nbdet_TimeInterval => bdet_Datetime." << endl;

        static const struct {
            int d_lineNum;  // source line number

            bsls_PlatformUtil::Int64 d_input_s;   // initial time_t value
            int                      d_input_ns;  // initial time_t value

            int d_year;    // expected field of result
            int d_month;   // expected field of result
            int d_day;     // expected field of result
            int d_hour;    // expected field of result
            int d_minute;  // expected field of result
            int d_second;  // expected field of result
            int d_msec;    // expected field of result

        } DATA[] = {           // <------- result ------->
            //lin   input value   year mon day hou min sec
            //---   -----------   ---- --- --- --- --- ---

                // *** All relevant fields are recovered properly. ***
            //lin     input (s) (ns) year mon day hou min sec  ms
            //---   ----------- ---- ---- --- --- --- --- --- ---
            { L_,             1,  0, 1970,  1,  1,  0,  0,  1, 0 }, // sec
            { L_,            59,  0, 1970,  1,  1,  0,  0, 59, 0 },

            { L_,            60,  0, 1970,  1,  1,  0,  1,  0, 0 }, // min
            { L_,          3599,  0, 1970,  1,  1,  0, 59, 59, 0 }, //

            { L_,          3600,  0, 1970,  1,  1,  1,  0,  0, 0 }, // hour
            { L_,         86399,  0, 1970,  1,  1, 23, 59, 59, 0 },

            { L_,         86400,  0, 1970,  1,  2,  0,  0,  0, 0 }, // day
            { L_,       2678399,  0, 1970,  1, 31, 23, 59, 59, 0 },

            { L_,       2678400,  0, 1970,  2,  1,  0,  0,  0, 0 }, // mon
            { L_,      31535999,  0, 1970, 12, 31, 23, 59, 59, 0 },

            { L_,      31536000,  0, 1971,  1,  1,  0,  0,  0, 0 }, // year
            { L_,      63072000,  0, 1972,  1,  1,  0,  0,  0, 0 }, // leap
            { L_,      94694400,  0, 1973,  1,  1,  0,  0,  0, 0 },
            { L_,     126230400,  0, 1974,  1,  1,  0,  0,  0, 0 },

            { L_,     441763200,  0, 1984,  1,  1,  0,  0,  0, 0 }, // dec.
            { L_,     757382400,  0, 1994,  1,  1,  0,  0,  0, 0 },
            { L_,    1072915200,  0, 2004,  1,  1,  0,  0,  0, 0 },
            { L_,    1388534400,  0, 2014,  1,  1,  0,  0,  0, 0 },
            { L_,    1704067200,  0, 2024,  1,  1,  0,  0,  0, 0 },
            { L_,    2019686400,  0, 2034,  1,  1,  0,  0,  0, 0 },

            { L_,    2021253247,  0, 2034,  1, 19,  3, 14,  7, 0 }, // year
            { L_,    2052789247,  0, 2035,  1, 19,  3, 14,  7, 0 },
            { L_,    2084325247,  0, 2036,  1, 19,  3, 14,  7, 0 }, // leap
            { L_,    2115947647,  0, 2037,  1, 19,  3, 14,  7, 0 },

            { L_,    2144805247,  0, 2037, 12, 19,  3, 14,  7, 0 }, // mon
            { L_,    2147480047,  0, 2038,  1, 19,  2, 14,  7, 0 }, // hour
            { L_,    2147483587,  0, 2038,  1, 19,  3, 13,  7, 0 }, // min
            { L_,    2147483646,  0, 2038,  1, 19,  3, 14,  6, 0 }, // sec

            { L_,    2147483647,  0, 2038,  1, 19,  3, 14,  7, 0 }, // int
            { L_,       INT_MAX,  0, 2038,  1, 19,  3, 14,  7, 0 },

            { L_,  2463016448LL,  0, 2048,  1, 19,  3, 14,  8, 0 },
            { L_,  2778635648LL,  0, 2058,  1, 19,  3, 14,  8, 0 },
            { L_,  3094168448LL,  0, 2068,  1, 19,  3, 14,  8, 0 },
            { L_,  3409787648LL,  0, 2078,  1, 19,  3, 14,  8, 0 },
            { L_,  3725320448LL,  0, 2088,  1, 19,  3, 14,  8, 0 },
            { L_,  4040939648LL,  0, 2098,  1, 19,  3, 14,  8, 0 },
            { L_,  4356386048LL,  0, 2108,  1, 19,  3, 14,  8, 0 },
            { L_,253402214400LL,  0, 9999, 12, 31,  0,  0,  0, 0 },
            { L_,253402297200LL,  0, 9999, 12, 31, 23,  0,  0, 0 },
            { L_,253402300740LL,  0, 9999, 12, 31, 23, 59,  0, 0 },
            { L_,253402300799LL,  0, 9999, 12, 31, 23, 59, 59, 0 }, // MAX

            { L_,      63072000,  1,
                                     1972,  1,  1,  0,  0,  0, 0 }, // ms
            { L_,      63072000, 10,
                                     1972,  1,  1,  0,  0,  0, 0 },
            { L_,      63072000, 100,
                                     1972,  1,  1,  0,  0,  0, 0 },
            { L_,      63072000, 1000,
                                     1972,  1,  1,  0,  0,  0, 0 },
            { L_,      63072000, 10000,
                                     1972,  1,  1,  0,  0,  0, 0 },
            { L_,      63072000, 100000,
                                     1972,  1,  1,  0,  0,  0, 0 },
            { L_,      63072000, 1000000,
                                     1972,  1,  1,  0,  0,  0, 1 },
            { L_,      63072000, 9000000,
                                     1972,  1,  1,  0,  0,  0, 9 },
            { L_,      63072000, 10000000,
                                     1972,  1,  1,  0,  0,  0, 10 },
            { L_,      63072000, 90000000,
                                     1972,  1,  1,  0,  0,  0, 90 },
            { L_,      63072000, 100000000,
                                     1972,  1,  1,  0,  0,  0, 100 },
            { L_,      63072000, 900000000,
                                     1972,  1,  1,  0,  0,  0, 900 },
            { L_,      63072000, 999000000,
                                     1972,  1,  1,  0,  0,  0, 999 },
            { L_,      63072000, 999123456,
                                     1972,  1,  1,  0,  0,  0, 999 },
            { L_,      63072000, 999654321,
                                     1972,  1,  1,  0,  0,  0, 999 },
            { L_,      63072000, 998999999,
                                     1972,  1,  1,  0,  0,  0, 998 },
            { L_,      63072000, 999999999,
                                     1972,  1,  1,  0,  0,  0, 999 },
         };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        // MAIN TEST-TABLE LOOP

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int   LINE  = DATA[ti].d_lineNum;

            const bsls_PlatformUtil::Int64 INPUT_S = DATA[ti].d_input_s;
            const int                     INPUT_NS = DATA[ti].d_input_ns;

            const int                         YEAR = DATA[ti].d_year;
            const int                        MONTH = DATA[ti].d_month;
            const int                          DAY = DATA[ti].d_day;
            const int                         HOUR = DATA[ti].d_hour;
            const int                       MINUTE = DATA[ti].d_minute;
            const int                       SECOND = DATA[ti].d_second;
            const int                         MSEC = DATA[ti].d_msec;

            if (veryVerbose) {
                T_ P_(INPUT_S) P(INPUT_NS)
            }

            bdet_TimeInterval INPUT(INPUT_S, INPUT_NS);
            bdet_TimeInterval INPUT2(INPUT_S, (INPUT_NS/1000000)*1000000);

            {

            // Double check that the table itself is right.
            bdet_Datetime dt(YEAR, MONTH, DAY, HOUR, MINUTE, SECOND, MSEC);
            bdet_TimeInterval ti;
            int s = bdetu_Epoch::convertToTimeInterval(&ti, dt);
            LOOP2_ASSERT(LINE, s, 0 == s);
            LOOP3_ASSERT(LINE, ti, INPUT2, ti == INPUT2);

            }

            bdet_Datetime result(1, 2, 3, 4, 5, 6, 7);
            bdet_Datetime resultDup(1, 2, 3, 4, 5, 6, 7);

            if (veryVerbose) { cout << "Before: "; P(result) }
            bdetu_Epoch::convertFromTimeInterval(&result, INPUT);
            if (veryVerbose) { cout << " After: "; P_(result) }

            if (veryVerbose) { cout << "Before: "; P(resultDup) }
            resultDup = bdetu_Epoch::convertFromTimeInterval(INPUT);
            if (veryVerbose) { cout << " After: "; P_(resultDup) }
  //v-------^
        // *** All relevant fields are recovered properly. ***
    LOOP3_ASSERT(LINE, YEAR, result.year(), YEAR == result.year());
    LOOP3_ASSERT(LINE, MONTH, result.month(), MONTH == result.month());
    LOOP3_ASSERT(LINE, DAY, result.day(), DAY == result.day());
    LOOP3_ASSERT(LINE, HOUR, result.hour(), HOUR == result.hour());
    LOOP3_ASSERT(LINE, MINUTE, result.minute(), MINUTE == result.minute());
    LOOP3_ASSERT(LINE, SECOND, result.second(), SECOND == result.second());
    LOOP3_ASSERT(LINE, MSEC, result.millisecond(),
                                                 MSEC == result.millisecond());

    LOOP3_ASSERT(LINE, YEAR, resultDup.year(), YEAR == resultDup.year());
    LOOP3_ASSERT(LINE, MONTH, resultDup.month(), MONTH == resultDup.month());
    LOOP3_ASSERT(LINE, DAY, resultDup.day(), DAY == resultDup.day());
    LOOP3_ASSERT(LINE, HOUR, resultDup.hour(), HOUR == resultDup.hour());
    LOOP3_ASSERT(LINE, MINUTE, resultDup.minute(),
                                                 MINUTE == resultDup.minute());
    LOOP3_ASSERT(LINE, SECOND, resultDup.second(),
                                                 SECOND == resultDup.second());
    LOOP3_ASSERT(LINE, MSEC, resultDup.millisecond(),
                                              MSEC == resultDup.millisecond());
  //^-------v
        } // end for ti

        }

        {

        if (verbose) cout <<
                  "\nbdet_TimeInterval => bdet_Datetime" <<
                  "=> bdet_TimeInterval." << endl;

        unsigned int SIZE = unsigned(INT_MAX) + 1;

        if (veryVerbose) P(SIZE);
                                         // to allow abs value to count!
        unsigned int numTrials = 1000; // default with no v-v-verbose

        const int PRIME = 123456789; // at least relatively prime to size

        ASSERT(SIZE % PRIME);        // ensure SIZE not divisible by PRIME

        double percentCovered = 100 * double(numTrials) / SIZE;

        if (verbose) { T_ P_(numTrials) P(percentCovered) }

        const unsigned int STARTING_VALUE = 0;

        unsigned int pseudoRandomValue = STARTING_VALUE;

        for (unsigned int i = 0; i < numTrials; ++i) {

            // Ensure that there is no premature repetition; ok first time.

            LOOP_ASSERT(i, STARTING_VALUE != pseudoRandomValue != !i);

            bdet_TimeInterval currentValue(pseudoRandomValue,
                                           0);
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
            if (veryVeryVerbose) P(INITIAL_VALUE);
            bdet_Datetime tmp(INITIAL_VALUE);
            bdet_Datetime tmpDup(INITIAL_VALUE);
  //v-------^
    if (veryVeryVerbose) { cout << "Before: "; P(tmp); }
    bdetu_Epoch::convertFromTimeInterval(&tmp, currentValue);
    if (veryVeryVerbose) { cout << " After: "; P(tmp); }

    if (veryVeryVerbose) { cout << "Before: "; P(tmpDup); }
    tmpDup = bdetu_Epoch::convertFromTimeInterval(currentValue);
    if (veryVeryVerbose) { cout << " After: "; P(tmpDup); }

    bdet_TimeInterval result(-1, -1);    // out of bounds
    bdet_TimeInterval resultDup(-1, -1); // out of bounds

    if (veryVeryVerbose) { cout<<"Before: "; P(result); }
    LOOP_ASSERT(i, 0 == bdetu_Epoch::convertToTimeInterval(&result, tmp));
    if (veryVeryVerbose) { cout<<" After: "; P(result); }

    if (veryVeryVerbose) { cout<<"Before: "; P(resultDup); }
    resultDup = bdetu_Epoch::convertToTimeInterval(tmpDup);
    if (veryVeryVerbose) { cout<<" After: "; P(resultDup); }

    LOOP_ASSERT(i, currentValue == result);
    LOOP_ASSERT(i, currentValue == resultDup);
  //^-------v
        }

        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CONVERT BDET_DATETIME TO/FROM TIME_T
        //
        // Concerns:
        //   To:
        //     1) All fields are converted properly.
        //     2) The millisecond field is ignored.
        //     3) Time = 24:00:00:000 converts to 00:00:00, not to 24:00:00.
        //     4) Datetime values producing out-of-range time_t values fail:
        //         a) time_t < 0
        //         b) time_t > 2^31 - 1
        //         c) internal 32-bit integer temporaries do not overflow.
        //     5) Bad status implies no change to result.
        //
        //   From:
        //     1) All relevant fields are recovered properly.
        //     2) The millisecond field is always set to 0.
        //
        // Plan:
        //   1) Test convertToTimeT
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
        //   2) Test convertFromTimeT
        //        * Reuse
        //        * Table-Based Implementation
        //        * Category Partitioning Data Selection
        //        * Exploit proven inverse operation: convertToTimeT
        //        * Orthogonal Perturbation:
        //              * Unaltered Initial Values
        //
        //        A) First, REUSE the table used to test 'convertToTimeT' to
        //           reverse every successful conversion to 'time_t', and
        //           compare that result against the initial input (except in
        //           cases where milliseconds were initially non-zero).
        //
        //        B) Second, create a separate table that explicitly converts
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
        //   3) Test convertToTimeT and then convertFromTimeT
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
        // Testing:
        //   int convertToTimeT(time_t *result, const bdet_Datetime& datetime);
        //   bsl::time_t convertToTimeT(const bdet_Datetime& datetime);
        //   void convertFromTimeT(bdet_Datetime *result, time_t time);
        //   bdet_Datetime convertFromTimeT(bsl::time_t time);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BDET_DATETIME TO/FROM TIME_T" << endl
                          << "============================" << endl;

        enum { FAILURE = 1 };

        {

        if (verbose) cout << "\nbdet_Datetime => time_t." << endl;

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

            bsl::time_t d_time;      // output

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
            { L_, 1883, 10, 20, 12, 49, 20, 123,  FAILURE       },
            { L_, 1889, 12, 31, 23, 59, 59, 999,  FAILURE       },
            { L_, 1899, 12, 31, 23, 59, 59, 999,  FAILURE       },
            { L_, 1909, 12, 31, 23, 59, 59, 999,  FAILURE       },
            { L_, 1919, 12, 31, 23, 59, 59, 999,  FAILURE       },
            { L_, 1925,  5, 28,  5,  9, 40, 321,  FAILURE       },
            { L_, 1929, 12, 31, 23, 59, 59, 999,  FAILURE       },
            { L_, 1939, 12, 31, 23, 59, 59, 999,  FAILURE       },
            { L_, 1944,  7,  8, 13, 18, 33, 951,  FAILURE       },
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

                // *** All fields are converted properly. ***
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
            { L_, 1970,  1,  1, 24,  0,  0,   0,  0,          0 }, // year
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

        const bsl::time_t INITIAL_VALUES[] = { // standard b-box int partition
            INT_MIN, INT_MIN + 1, -1, 0, 1, INT_MAX
        };

        int tmp = sizeof INITIAL_VALUES / sizeof *INITIAL_VALUES;

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
  //v-----------^
    const bsl::time_t CONTROL = INITIAL_VALUES[vi];
    if (veryVeryVerbose) { P(CONTROL); }
    bsl::time_t result = CONTROL;

    if (veryVerbose) { cout << "Before: "; P(result); }
    int status = bdetu_Epoch::convertToTimeT(&result, INPUT);
    if (veryVerbose) { cout << "After: "; P(result); }

    LOOP4_ASSERT(LINE, vi, STATUS, status, !STATUS == !status); // black-box

    bsl::time_t resultDup = CONTROL;
    if (veryVerbose) { cout << "Before: "; P(resultDup); }
    // This is necessary since the version of 'convertToTimeT' does not do any
    // error checking.
    if (!STATUS) {
        resultDup = bdetu_Epoch::convertToTimeT(INPUT);
    }
    if (veryVerbose) { cout << "After: "; P(resultDup); }

    if (STATUS) {
            // *** Bad status implies no change to result. ***
        LOOP4_ASSERT(LINE, vi, CONTROL, result, CONTROL == result);
        LOOP4_ASSERT(LINE, vi, CONTROL, result, CONTROL == resultDup);
    }
    else {
        LOOP4_ASSERT(LINE, vi, TIME, result, TIME == result);
        LOOP4_ASSERT(LINE, vi, TIME, result, TIME == resultDup);
    }

        // *** REUSE THIS LOOP TO PARTIALLY TEST convertFromTimeT ***

    if (vi)                 continue; // Don't repeat more than once per row.
    if (STATUS)             continue; // Conversion failed; nothing to reverse.

    bdet_Datetime result2(9999, 12, 31, 23, 59, 59, 999); // unreachable
    bdet_Datetime result2Dup(9999, 12, 31, 23, 59, 59, 999); // unreachable

    if (veryVerbose) { cout << "Before: "; P(result2); }
    bdetu_Epoch::convertFromTimeT(&result2, result);
    if (veryVerbose) { cout << "After: "; P(result2); }

    if (veryVerbose) { cout << "Before: "; P(result2Dup); }
    result2Dup = bdetu_Epoch::convertFromTimeT(resultDup);
    if (veryVerbose) { cout << "After: "; P(result2Dup); }

        // *** The millisecond field is always set to 0. ***
    if (veryVeryVerbose) P(INPUT);
    bdet_Datetime tmp2(INPUT);
    tmp2.setMillisecond(0);

        // *** There is no equivalent representation for 24:00:00 as a time_t
    tmp2.setHour(tmp2.hour() % 24);

    const bdet_Datetime INPUT2(tmp2);
    if (veryVerbose) P(INPUT2);
    LOOP2_ASSERT(LINE, INPUT2, 0 == INPUT2.millisecond()); // double check

        // *** All relevant fields are recovered properly. ***
    LOOP3_ASSERT(LINE, INPUT2, result2, INPUT2 == result2);
    LOOP3_ASSERT(LINE, INPUT2, result2Dup, INPUT2 == result2Dup);
  //^-----------v
            } // end for vi
        } // end for ti

        }

        {

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

                // *** All relevant fields are recovered properly. ***
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
            { L_,      63072000,  1972,  1,  1,  0,  0,  0 }, // lp. yr.
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
            { L_,    2084325247,  2036,  1, 19,  3, 14,  7 }, // lp. yr.
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
                bsl::time_t tt;
                int s = bdetu_Epoch::convertToTimeT(&tt, dt);
                LOOP2_ASSERT(LINE, s, 0 == s);
                LOOP3_ASSERT(LINE, tt, INPUT, tt == INPUT);
            }

            bdet_Datetime result(1, 2, 3, 4, 5, 6, 7);
            bdet_Datetime resultDup(1, 2, 3, 4, 5, 6, 7);

            if (veryVerbose) { cout << "Before: "; P(result) }
            bdetu_Epoch::convertFromTimeT(&result, INPUT);
            if (veryVerbose) { cout << " After: "; P_(result) }

            if (veryVerbose) { cout << "Before: "; P(resultDup) }
            resultDup = bdetu_Epoch::convertFromTimeT(INPUT);
            if (veryVerbose) { cout << " After: "; P_(resultDup) }
  //v-------^
        // *** All relevant fields are recovered properly. ***
    LOOP3_ASSERT(LINE, YEAR, result.year(), YEAR == result.year());
    LOOP3_ASSERT(LINE, MONTH, result.month(), MONTH == result.month());
    LOOP3_ASSERT(LINE, DAY, result.day(), DAY == result.day());
    LOOP3_ASSERT(LINE, HOUR, result.hour(), HOUR == result.hour());
    LOOP3_ASSERT(LINE, MINUTE, result.minute(), MINUTE == result.minute());
    LOOP3_ASSERT(LINE, SECOND, result.second(), SECOND == result.second());

    LOOP3_ASSERT(LINE, YEAR, resultDup.year(), YEAR == resultDup.year());
    LOOP3_ASSERT(LINE, MONTH, resultDup.month(), MONTH == resultDup.month());
    LOOP3_ASSERT(LINE, DAY, resultDup.day(), DAY == resultDup.day());
    LOOP3_ASSERT(LINE, HOUR, resultDup.hour(), HOUR == resultDup.hour());
    LOOP3_ASSERT(LINE, MINUTE, resultDup.minute(),
                                                 MINUTE == resultDup.minute());
    LOOP3_ASSERT(LINE, SECOND, resultDup.second(),
                                                 SECOND == resultDup.second());

        // *** The millisecond field is always set to 0. ***
    LOOP2_ASSERT(LINE, result.millisecond(), 0 == result.millisecond());
    LOOP2_ASSERT(LINE, resultDup.millisecond(), 0 == resultDup.millisecond());
  //^-----------v
        } // end for ti

        }

        {

        if (verbose) cout <<
                  "\ntime_t => bdet_Datetime => time_t." << endl;

        unsigned int SIZE = unsigned(INT_MAX) + 1;

        if (veryVerbose) P(SIZE);
                                         // to allow abs value to count!
        unsigned int numTrials = 1000; // default with no v-v-verbose

        const int PRIME = 123456789; // at least relatively prime to size

        ASSERT(SIZE % PRIME);        // ensure SIZE not divisible by PRIME

        double percentCovered = 100 * double(numTrials) / SIZE;

        if (verbose) { T_ P_(numTrials) P(percentCovered) }

        const unsigned int STARTING_VALUE = 0;

        unsigned int pseudoRandomValue = STARTING_VALUE;

        for (unsigned int i = 0; i < numTrials; ++i) {

            // Ensure that there is no premature repetition; ok first time.

            LOOP_ASSERT(i, STARTING_VALUE != pseudoRandomValue != !i);

            bsl::time_t currentValue = pseudoRandomValue;
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
            if (veryVeryVerbose) P(INITIAL_VALUE);
            bdet_Datetime tmp(INITIAL_VALUE);
            bdet_Datetime tmpDup(INITIAL_VALUE);
  //v-------^
    if (veryVeryVerbose) { cout << "Before: "; P(tmp); }
    bdetu_Epoch::convertFromTimeT(&tmp, currentValue);
    if (veryVeryVerbose) { cout << " After: "; P(tmp); }

    if (veryVeryVerbose) { cout << "Before: "; P(tmpDup); }
    tmpDup = bdetu_Epoch::convertFromTimeT(currentValue);
    if (veryVeryVerbose) { cout << " After: "; P(tmpDup); }

    bsl::time_t result = -int(i); // out of bounds
    bsl::time_t resultDup = -int(i); // out of bounds

    if (veryVeryVerbose) { cout<<"Before: "; P(result); }
    LOOP_ASSERT(i, 0 == bdetu_Epoch::convertToTimeT(&result, tmp));
    if (veryVeryVerbose) { cout<<" After: "; P(result); }

    if (veryVeryVerbose) { cout<<"Before: "; P(resultDup); }
    resultDup = bdetu_Epoch::convertToTimeT(tmpDup);
    if (veryVeryVerbose) { cout<<" After: "; P(resultDup); }

    LOOP_ASSERT(i, currentValue == result);
    LOOP_ASSERT(i, currentValue == resultDup);
  //^-------v
        }

        }
      } break;
      case 1: {
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

        const bdet_Datetime EPOCH(1970, 1, 1, 0);

        ASSERT(EPOCH == bdetu_Epoch::epoch());
        ASSERT(epochAddressIsNotZero);
        ASSERT(EPOCH == EarlyEpochCopier::copiedValue());

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
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
