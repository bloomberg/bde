// baetzo_leapcorrection.t.cpp                                        -*-C++-*-

#include <baetzo_leapcorrection.h>

#include <bdet_datetime.h>
#include <bdetu_epoch.h>

#include <bdex_byteinstream.h>           // for testing only
#include <bdex_byteoutstream.h>          // for testing only
#include <bdex_instreamfunctions.h>      // for testing only
#include <bdex_outstreamfunctions.h>     // for testing only
#include <bdex_testinstream.h>           // for testing only
#include <bdex_testinstreamexception.h>  // for testing only
#include <bdex_testoutstream.h>          // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iostream.h>
#include <bsl_string.h>
#include <bsl_strstream.h>
#include <bsl_vector.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test is a value-semantic aggregate type whose state is
// identical to its value.  Given the absence of any complex operation, our
// testing concerns are safely limited to the mechanical functioning of the
// various methods and free operators.
//
// The component interface represents a leap correction as a 'bdet_Datetime'
// and an integer field.  Both of these fields will have to be verified during
// testing.
//
// The primary manipulators for 'baetzo_LeapCorrection' are the default
// constructor, 'setUtcTransitionTime' and 'setAccumulatedCorrection'.  The
// basic accessors are 'utcTime' and 'accumulatedCorrectionInSeconds'.
//
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] baetzo_LeapCorrection();
// [  ] baetzo_LeapCorrection(const bdet_Datetime& time, int correction);
// [  ] baetzo_LeapCorrection(const baetzo_LeapCorrection& original);
//
// MANIPULATORS
// [  ] operator=(const baetzo_LeapCorrection& rhs);
// [ 2] void setUtcTransitionTime(const bdet_Datetime& utcTime);
// [ 2] void setAccumulatedCorrection(int leapCorrection);
//
// ACCESSORS
// [ 4] const bdet_Datetime& utcTime() const;
// [ 4] int accumulatedCorrectionInSeconds() const;
// [ 5] print(bsl::ostream& stream, int level, int spacesPerLevel) const;
//
// FREE OPERATORS
// [ 6] operator==(const baetzo_LeapCorrection& lhs, rhs);
// [ 6] operator!=(const baetzo_LeapCorrection& lhs, rhs);
// [ 6] operator<(const baetzo_LeapCorrection& lhs, rhs);
// [ 5] operator<<(bsl::ostream& s, const baetzo_LeapCorrection& c);
//
// FREE FUNCTIONS
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// ============================================================================
//                  STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

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

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------
typedef baetzo_LeapCorrection Obj;
typedef bdex_TestInStream     In;
typedef bdex_TestOutStream    Out;

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// In this section we demonstrate how to use a series of leap-corrections to
// more accurately determine the actual time interval between two UTC times.
//
///Example 1: Accurately Computing the Whole Seconds Between Two UTC Times
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A common pattern for using 'baetzo_LeapCorrection' objects is to maintain an
// ordered sequence of corrections (sorted by 'utcTime'), where each correction
// indicates the start of a time interval where the associated leap-correction
// is applicable (and the end of that interval is indicated by the 'utcTime' of
// the subsequent leap correction in the sequence).
//
// First, we define a helper function 'determineLeapSeconds':
//..
    int determineLeapSeconds(const bdet_Datetime&         utcTime,
                             const baetzo_LeapCorrection *corrections,
                             int                          numCorrections)
        // Return the number of leap seconds that have been incorporated into
        // to the specified 'utcTime', using the ordered sequence of specified
        // 'corrections' of at least length 'numCorrections'.  The behavior is
        // undefined unless 'corrections' contains at least the specified
        // 'numCorrections', where each correction's 'utcTime' is less than the
        // subsequents correction's 'utcTime', and each correction's 'utcTime'
        // indicates the start of a time interval where the associated
        // 'accumulatedCorrectionInSeconds' is applicable (and the end of that
        // interval is indicated by the subsequent correction's 'utcTime').
    {
//..
// We now construct a temporary 'baetzo_LeapCorrection' object that we will use
// to perform a binary search of 'corrections'.
//..
        baetzo_LeapCorrection searchValue(utcTime, 0);
//..
// Next, we use 'upper_bound' to find the leap-correction that applies to
// 'utcTime' Note that 'baetzo_LeapCorrection' provides 'lessUtcTime', a static
// class method that returns 'true' if the first object's 'utcTime' attribute
// is less than the second's.
//..
        const baetzo_LeapCorrection *result =
                         bsl::upper_bound(corrections,
                                          corrections + numCorrections,
                                          searchValue,
                                          &baetzo_LeapCorrection::lessUtcTime);
//..
// Note that either 'upper_bound' or 'lower_bound' will return the correction
// immediately after the correction that applies to 'utcTime' in the interior
// of the range.  We use 'upper_bound' because, in the unlikely case that the
// search value has the same 'utcTime' as an element in the sequence, it will
// return an iterator *after* that element (rather than referring to that
// element), so we do not need to explicitly handle that edge case.
//..
        if (result == corrections) {
            // 'utcTime' precedes all corrections.

            return 0;                                                 // RETURN
        }

        --result;

        return result->accumulatedCorrectionInSeconds();
    }
//..
// Now, using the 'determineLeapSeconds' method, we can easily define a new
// function 'elapsedTime' that accurately determines the number of seconds
// between two UTC values:
//..
   bdet_DatetimeInterval elapsedTime(
                                 bdet_Datetime                startUtcTime,
                                 bdet_Datetime                endUtcTime,
                                 const baetzo_LeapCorrection *corrections,
                                 int                          numCorrections)
       // Return the elapsed time between the specified 'startUtcTime' and the
       // specified 'endUtcTime', using the specified ordered-sequence of
       // 'numCorrections' 'corrections' to account for leap seconds.  The
       // behavior is undefined unless 'startUtcTime <= endUtcTime',
       // 'corrections' contains at least the specified 'numCorrections'
       // objects, each correction's 'utcTime' is less than every subsequent
       // correction's 'utcTime', and each correction's 'utcTime' indicates the
       // start of a time period where the associated
       // 'accumulatedCorrectionInSeconds' is in effect (and the inclusive end
       // of that interval is indicated by the subsequent correction's
       // 'utcTime').
   {
       ASSERT(startUtcTime <= endUtcTime);

       bdet_DatetimeInterval interval = endUtcTime - startUtcTime;
//..
// We use the 'determineLeapSeconds' methods to find the number of leap-seconds
// that have been added between 'startUtcTime' and 'endUtcTime':
//..
        int startCorrectionSec = determineLeapSeconds(startUtcTime,
                                                      corrections,
                                                      numCorrections);
        int endCorrectionSec   = determineLeapSeconds(endUtcTime,
                                                      corrections,
                                                      numCorrections);
        int numLeapSeconds     = endCorrectionSec - startCorrectionSec;
//..
// Finally, we adjust the computed 'interval' by the number of leap-seconds
// that have been added:
//..
        interval.addSeconds(numLeapSeconds);
        return interval;
    }
//..

// ============================================================================
//                                MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
        //--------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'BSLS_ASSERT' with 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Usage Example"
                          << "\n=====================" << endl;

// To exercise our 'elapsedTime' function, we obtain an historical record:
//..
    struct {
        int d_year;
        int d_month;
        int d_day;
        int d_correction;
    } CORRECTIONS[] = {
        { 1972, 1, 1,  0 },
        { 1972, 7, 1,  1 },
        { 1973, 1, 1,  2 },
        { 1974, 1, 1,  3 },
        { 1975, 1, 1,  4 },
        { 1976, 1, 1,  5 },
        { 1977, 1, 1,  6 },
        { 1978, 1, 1,  7 },
        { 1979, 1, 1,  8 },
        { 1980, 1, 1,  9 },
        { 1981, 7, 1, 10 },
        { 1982, 7, 1, 11 },
        { 1983, 7, 1, 12 },
        { 1985, 7, 1, 13 },
        { 1988, 1, 1, 14 },
        { 1990, 1, 1, 15 },
        { 1991, 1, 1, 16 },
        { 1992, 7, 1, 17 },
        { 1993, 7, 1, 18 },
        { 1994, 7, 1, 19 },
        { 1996, 1, 1, 20 },
        { 1997, 7, 1, 21 },
        { 1999, 1, 1, 22 },
        { 2006, 1, 1, 23 },
        { 2009, 1, 1, 24 }
    };
    const int SIZE = sizeof CORRECTIONS / sizeof *CORRECTIONS;
//..
// The historical record is used to create the requisite "array" of
// 'baetzo_LeapCorrection' objects.  In this example, a 'bsl::vector' is used
// as a dynamic array:
//..
    bsl::vector<baetzo_LeapCorrection> corrections;

    for (int ci = 0; ci < SIZE; ++ci) {
        corrections.push_back(baetzo_LeapCorrection(
                                         bdet_Datetime(CORRECTIONS[ci].d_year,
                                                       CORRECTIONS[ci].d_month,
                                                       CORRECTIONS[ci].d_day),
                                         CORRECTIONS[ci].d_correction));
    }
//..
// We can then compare the values returned from our 'elapsedTime' function with
// values obtained from inspection of the historical data.  For an interval
// that spans the entire history of leap-seconds, we expect the correction to
// match the largest accumulated offset (i.e., 24).
//..
    bdet_Datetime start1(1970, 1, 1);
    bdet_Datetime   end1(2010, 1, 1);

    bdet_DatetimeInterval uncorrectedInterval1 = end1 - start1;
    bdet_DatetimeInterval  calculatedInterval1 = elapsedTime(
                                                       start1,
                                                       end1,
                                                       &corrections.front(),
                                                       corrections.size());
    ASSERT(uncorrectedInterval1 + bdet_DatetimeInterval(0, 0, 0, 24)
         == calculatedInterval1);
//..
// For an interval spanning the last three leap-seconds, we expect the
// correction to match the number of leap-seconds in that period (i.e., 3).
//..
    bdet_Datetime start2(1998, 1, 1);
    bdet_Datetime   end2(2010, 1, 1);

    bdet_DatetimeInterval uncorrectedInterval2 = end2 - start2;
    bdet_DatetimeInterval  calculatedInterval2 = elapsedTime(
                                                          start2,
                                                          end2,
                                                          &corrections.front(),
                                                          corrections.size());
    ASSERT(uncorrectedInterval2 + bdet_DatetimeInterval(0, 0, 0, 3)
         == calculatedInterval2);
//..
      } break;
/////////// BEGIN: IGNORE THESE TWO TEST CASES FOR NOW ////////////////////////
      case 3: {
//      // --------------------------------------------------------------------
//      // DEFAULT CONSTRUCTOR TEST:
//      //
//      // Concerns:
//      //   The default constructor ...
//      //
//      // Plan:
//      //
//      // Testing:
//      //   baetzo_LeapCorrection();
//      //   const bdet_Datetime& utcTime() const;
//      //   int accumulatedCorrectionInSeconds() const;
//      // --------------------------------------------------------------------
//      if (verbose) cout << endl << "DEFAULT CONSTRUCTOR TEST" << endl
//                                << "========================" << endl;

//      Obj x;
//      ASSERT(bdet_Datetime() == x.utcTime());
//      ASSERT(0 == x.accumulatedCorrectionInSeconds());

      } break;
      case 2: {
//      // --------------------------------------------------------------------
//      // BASIC MANIPULATOR TEST:
//      //
//      // Concerns:
//      //
//      // Plan:
//      //
//      // Testing:
//      //   void setUtcTransitionTime(const bdet_Datetime& utcTime);
//      //   void setAccumulatedCorrection(int leapCorrection);
//      // --------------------------------------------------------------------
//      if (verbose) cout << endl << "DEFAULT CONSTRUCTOR TEST" << endl
//                                << "========================" << endl;

//      Obj x;
//      ASSERT(bdet_Datetime() == x.utcTime());
//      ASSERT(0 == x.accumulatedCorrectionInSeconds());

//      {
//          static const struct {
//              int d_line;  int d_year;  int d_month;   int d_day;
//              int d_hour;  int d_minute;  int d_second;  int d_msec;
//          } DATA[] = {
//              { L_,    1,  1,  1,  0,  0,  0,   0, },
//              { L_, 1700,  7, 31, 23, 22, 21, 206, },
//              { L_, 1800,  8, 31, 23, 22, 21, 207, },
//              { L_, 1900,  9, 30, 23, 22, 21, 208, },
//              { L_, 2000, 10, 31, 23, 22, 21, 209, },
//              { L_, 2100, 11, 30, 23, 22, 21, 210, },
//              { L_, 9999, 12, 31, 23, 59, 59, 999, }
//          };

//          const int SIZE = sizeof DATA / sizeof *DATA;

//          for (int ti = 0; ti < SIZE; ++ti) {
//              const bdet_Datetime DATETIME(DATA[ti].d_year,
//                                           DATA[ti].d_month,
//                                           DATA[ti].d_day,
//                                           DATA[ti].d_hour,
//                                           DATA[ti].d_minute,
//                                           DATA[ti].d_second,
//                                           DATA[ti].d_msec);

//              Obj x;
//              const Obj& X = x;

//              x.setUtcTransitionTime(DATETIME);

//              LOOP_ASSERT(ti, DATETIME == X.utcTime());
//          }
//      }
//      {
//          static const struct {
//              int d_line; int d_correction;
//          } DATA[] = {
//              { L_, INT_MIN, },
//              { L_,      -1, },
//              { L_,       0, },
//              { L_,       1, },
//              { L_, INT_MAX, },
//          };

//          const int SIZE = sizeof DATA / sizeof *DATA;

//          for (int ti = 0; ti < SIZE; ++ti) {
//              const int LINE       = DATA[ti].d_line;
//              const int CORRECTION = DATA[ti].d_correction;

//              Obj x;
//              const Obj& X = x;

//              x.setAccumulatedCorrection(CORRECTION);

//              LOOP_ASSERT(LINE,
//                          CORRECTION == X.accumulatedCorrectionInSeconds());
//          }
//      }
      } break;
/////// END ////////////////////////////////////////////////////////////////

      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Concerns:
        //   Exercise a broad cross-section of value-semantic functionality
        //   before beginning testing in earnest.  Probe that functionality
        //   systematically and incrementally to discover basic errors in
        //   isolation.
        //
        // Plan:
        //   Create four test objects by using the default, initializing, and
        //   copy constructors.  Exercise the basic value-semantic methods and
        //   the equality operators using these test objects.  Invoke the
        //   primary manipulator [3, 6], copy constructor [2, 5], and
        //   assignment operator without [7, 8] and with [9] aliasing.  Use the
        //   basic accessors to verify the expected results.  Display object
        //   values frequently in verbose mode.  Note that 'VA', 'VB', and 'VC'
        //   denote unique, but otherwise arbitrary, object values, while 'U'
        //   denotes the valid, but "unknown", default object value.
        //
        // 1. Create an object x1 (init. to VA).    { x1:VA }
        // 2. Create an object x2 (copy from x1).   { x1:VA x2:VA }
        // 3. Set x1 to VB.                         { x1:VB x2:VA }
        // 4. Create an object x3 (default ctor).   { x1:VB x2:VA x3:U }
        // 5. Create an object x4 (copy from x3).   { x1:VB x2:VA x3:U  x4:U }
        // 6. Set x3 to VC.                         { x1:VB x2:VA x3:VC x4:U }
        // 7. Assign x2 = x1.                       { x1:VB x2:VB x3:VC x4:U }
        // 8. Assign x2 = x3.                       { x1:VB x2:VC x3:VC x4:U }
        // 9. Assign x1 = x1 (aliasing).            { x1:VB x2:VB x3:VC x4:U }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing
        const int LSA = 0;    // Leaps seconds for VA
        const int LSB = 100;  // Leaps seconds for VB
        const int LSC = 1000; // Leaps seconds for VC

        const bdet_Datetime TTA(1971,  1,  1,  0,  0,  0);
        const bdet_Datetime TTB(2000,  3, 15,  3, 10, 15);
        const bdet_Datetime TTC(2010, 12, 31, 23, 59, 59);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(TTA, LSA);  const Obj& X1 = mX1;
        if (veryVerbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(LSA == X1.accumulatedCorrectionInSeconds());
        ASSERT(TTA == X1.utcTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(LSA == X2.accumulatedCorrectionInSeconds());
        ASSERT(TTA == X2.utcTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setAccumulatedCorrection(LSB);
        mX1.setUtcTime(TTB);
        if (veryVerbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(LSB == X1.accumulatedCorrectionInSeconds());
        ASSERT(TTB == X1.utcTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (veryVerbose) { T_ P(X3) }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;

        ASSERT(bdetu_Epoch::epoch() == X3.utcTime());
        ASSERT(0                    == X3.accumulatedCorrectionInSeconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (veryVerbose) { T_ P(X4) }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;

        ASSERT(bdetu_Epoch::epoch() == X4.utcTime());
        ASSERT(0                    == X4.accumulatedCorrectionInSeconds());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setAccumulatedCorrection(LSC);
        mX3.setUtcTime(TTC);
        if (veryVerbose) { T_ P(X3) }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(LSC == X3.accumulatedCorrectionInSeconds());
        ASSERT(TTC == X3.utcTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (veryVerbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(LSB == X2.accumulatedCorrectionInSeconds());
        ASSERT(TTB == X2.utcTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;
        if (veryVerbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(LSC == X2.accumulatedCorrectionInSeconds());
        ASSERT(TTC == X2.utcTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;
        if (veryVerbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(LSB == X1.accumulatedCorrectionInSeconds());
        ASSERT(TTB == X1.utcTime());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

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
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
