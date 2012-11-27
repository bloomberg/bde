// bdetu_systemtime.t.cpp         -*-C++-*-

#include <bdetu_systemtime.h>
#include <bdet_datetimeinterval.h>
#include <bdetu_epoch.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script


// ==========================================================================
//                          TEST PLAN
// --------------------------------------------------------------------------
//                          Overview
//                          --------
//
//
//---------------------------------------------------------------------------
// [11] bdet_DatetimeInterval localTimeOffset();
// [10] bdet_Datetime nowAsDatetimeLocal();
// [ 5] bdet_Datetime nowAsDatetimeUtc();
// [ 6] bdet_Datetime nowAsDatetime();
// [12] bdet_Datetime nowAsDatetimeGMT();
// [ 5] bdet_TimeInterval now();
// [ 3] void loadCurrentTime(bdet_TimeInterval *result);
// [ 1] void loadSystemTimeDefault(bdet_TimeInterval *result);
// [ 1] loadLocalTimeOffsetDefault(bdet_TimeInterval *result);
// [ 2] setSystemTimeCallback(SystemTimeCallback callback);
// [ 2] currentSystemTimeCallback();
// [ 2] setSystemLocalTimeOffsetCallback(LocalTimeOffsetCallback callback);
// [ 2] currentLocalTimeOffsetCallback();
//---------------------------------------------------------------------------
// [13] USAGE example
// [ 9] bdet_Datetime nowAsDatetime() stress test
// [ 8] bdet_Datetime nowAsDatetimeUtc() stress test
// [ 7] bdet_TimeInterval now() stress test
// [ 4] static int inOrder();
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACROS
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------
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
   if (!(X)) { cout << #I << ": " << I << "\t"                             \
                    << #J << ": " << J << "\t" << #K << ": " << K << "\t"  \
                    << #L << ": " << L << "\t" << #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define F_(X) cout << X << flush;             // P_(X) without #X
#define L_ __LINE__                           // current Line number
#define T_()  cout << '\t' << flush;          // Print tab w/o newline
#define NL()  cout << endl;                   // Print newline
#define P64(X) printf(#X " = %lld\n", (X));   // Print 64-bit integer id & val
#define P64_(X) printf(#X " = %lld,  ", (X)); // Print 64-bit integer w/o '\n'

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------



// ==========================================================================
//                      HELPER FUNCTIONS FOR TESTING
// --------------------------------------------------------------------------

void IncrementInterval(bdet_TimeInterval *result)
{
    static int value;
    result->setInterval(value, value);
    ++value;
}

void f1(bdet_TimeInterval *result)
{
    result->setInterval(1,1);
}

void f2(bdet_TimeInterval *result)
{
    result->setInterval(1,1);
}

bdet_DatetimeInterval f3()
{
    return bdet_DatetimeInterval();
}

bdet_DatetimeInterval f4()
{
    return bdet_DatetimeInterval();
}

static int inOrder(const bdet_DatetimeInterval& lhs,
                   const bdet_TimeInterval&     rhs)
    // Return 1, 0, -1 accordingly as lhs <, ==, > rhs.
    // As a mnemonic, they return 1 if lhs,rhs are in correct order.
{
    bsls_PlatformUtil::Int64 dtm = lhs.totalMilliseconds();
    bsls_PlatformUtil::Int64  tm = rhs.totalMilliseconds();
    return dtm > tm ? -1 : dtm != tm;
}

static int inOrder(const bdet_TimeInterval&     lhs,
                   const bdet_DatetimeInterval& rhs)
    // Return 1, 0, -1 accordingly as lhs <, ==, > rhs.
    // As a mnemonic, they return 1 if lhs,rhs are in correct order.
{
    return -inOrder(rhs, lhs);
}

// ============================================================================
//                             USAGE EXAMPLES
// ----------------------------------------------------------------------------

///Usage 3
///-------
// For applications that choose to define there own mechanism for determining
// system time, the 'bdetu_SystemTime' utility provides the ability to install
// a custom system-time callback.
//
// First define the user-defined callback function 'getClientTime':
//..
void getClientTime(bdet_TimeInterval *result)
{
    result->setInterval(1,1);
}
//..

///Usage 4
///-------
// For applications that choose to define there own mechanism for determining
// the differential between local time and UTC time, the 'bdetu_SystemTime'
// utility provides the ability to install a custom local-time-offset callback.
//
// First, we define the user-defined callback function 'getLocalTimeOffset':
//..
bdet_DatetimeInterval getLocalTimeOffset()
{
    return bdet_DatetimeInterval(0, 1);  // an hour differential
}
//..

// ============================================================================
//                          MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    // int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
    case 13: {
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

        if (verbose) cout << "\nTesting Usage Example 1"
                          << "\n=======================" << endl;
        {
///Usage 1
///-------
// The following snippets of code illustrate how to use this utility component
// to obtain the system time by calling 'now', 'nowAsDatetimeUtc', or
// 'loadCurrentTime'.
//..
      bdet_TimeInterval i0;
      ASSERT(0 == i0);
//..
// Next call the utility function 'now' to obtain the system time.
//..
      i0 = bdetu_SystemTime::now();
      ASSERT(0 != i0);
//..
// Next call the utility function 'nowAsDatetimeUtc' to obtain the system time.
//..
      bdet_Datetime i1 = bdetu_SystemTime::nowAsDatetimeUtc();
      ASSERT(bdetu_Epoch::epoch() < i1);
      bdet_DatetimeInterval dti = i1 - bdetu_Epoch::epoch();
      ASSERT(i0.totalMilliseconds() <= dti.totalMilliseconds());
//..
// Now call the utility function 'loadCurrentTime' to load the system time
// into i2;
//..
      bdet_TimeInterval i2;
      ASSERT(0 == i2);
      bdetu_SystemTime::loadCurrentTime(&i2);
      ASSERT(0 != i2);
      ASSERT(dti.totalMilliseconds() <= i2.totalMilliseconds());
                                               //  Presumably, 0 < i0 < i1 < i2
//..
        }

        if (verbose) cout << "\nTesting Usage Example 2"
                          << "\n=======================" << endl;
        {
///Usage 2
///-------
// The following snippets of code illustrate how to use 'loadSystemTimeDefault'
// function (Note that 'loadSystemTimeDefault') provides a default
// implementation to retrieve system time.
//
// First create a default object 'i3' of 'bdet_TimeInterval'.
//..
      bdet_TimeInterval i3;
      ASSERT(0 == i3);
//..
// Next call the utility function 'loadSystemTimeDefault' to load the
// system time into 'i3'.
//..
      bdetu_SystemTime::loadSystemTimeDefault(&i3);
      ASSERT(0 != i3);
//..
// Create another object 'i4' of 'bdet_TimeInterval'
//..
      bdet_TimeInterval i4;
      ASSERT(0 == i4);
//..
// Then call the utility function 'loadSystemTimeDefault' again to load the
// system time into 'i4'.
//..
      bdetu_SystemTime::loadSystemTimeDefault(&i4);
      ASSERT(i4 >= i3);
//..
        }

        if (verbose) cout << "\nTesting Usage Example 3"
                          << "\n=======================" << endl;
        {
// Then, store the address of the user-defined callback function
// 'getClientTime' into 'callback_user_ptr1':
//..
      const bdetu_SystemTime::SystemTimeCallback callback_user_ptr1
                                                              = &getClientTime;
//..
// Next, call the utility function 'setSystemTimeCallback' to load the
// user-defined callback function:
//..
      bdetu_SystemTime::setSystemTimeCallback(callback_user_ptr1);
      ASSERT(callback_user_ptr1
                             == bdetu_SystemTime::currentSystemTimeCallback());
//..
// Then, create object 'i5' and 'i6' of 'bdet_TimeInterval':
//..
      bdet_TimeInterval i5;
      ASSERT(0 == i5);
      bdet_TimeInterval i6;
      ASSERT(0 == i6);
//..
// Now, call the utility function 'now' and get the system time into 'i5':
//..
      i5 = bdetu_SystemTime::now();
      ASSERT(1 == i5.seconds());
      ASSERT(1 == i5.nanoseconds());
//..
// Finally, call utility function 'loadCurrentTime' to load the system time
// into 'i6':
//..
      bdetu_SystemTime::loadCurrentTime(&i6);
      ASSERT(1 == i6.seconds());
      ASSERT(1 == i6.nanoseconds());
//..
        }

        if (verbose) cout << "\nTesting Usage Example 4"
                          << "\n=======================" << endl;
        {
// Then, store the address of the user-defined callback function
// 'getLocalTimeOffset' into 'callback_user_ptr1':
//..
      const bdetu_SystemTime::LocalTimeOffsetCallback callback_user_ptr1
                                                         = &getLocalTimeOffset;
//..
// Then call the utility function 'setLocalTimeOffsetCallback' to load the
// user-defined local-time-offset callback function:
//..
      bdetu_SystemTime::setLocalTimeOffsetCallback(callback_user_ptr1);
      ASSERT(callback_user_ptr1
             == bdetu_SystemTime::currentLocalTimeOffsetCallback());
//..
// Next, we call the utility function 'localTimeOffset' to get the current
// local time offset:
//..
      bdet_DatetimeInterval i7 = bdetu_SystemTime::localTimeOffset();
      ASSERT(0 == i7.days());
      ASSERT(1 == i7.hours());
//..
        }

    } break;
    case 12: {
      // --------------------------------------------------------------------
      // TESTING 'nowAsDatetimeGMT' METHOD
      //  This deprecated method returns a 'nowAsDatetimeUtc'
      //
      // Plan:
      //  Call 'nowAsDatetimeGMT' and compare that the results are the same as
      //  'nowAsDatetimeUtc'.
      //
      //
      // Testing:
      //  bdet_Datetime nowAsDatetimeGMT()
      // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'nowAsDatetimeGMT'" << endl;

        const bdet_Datetime EXP_RESULT = bdetu_SystemTime::nowAsDatetimeUtc();
        const bdet_Datetime result     = bdetu_SystemTime::nowAsDatetimeGMT();

        const double diffS = (EXP_RESULT - result).totalSecondsAsDouble();
        ASSERT(-0.01 < diffS);
        ASSERT( 0.01 > diffS);
    } break;
    case 11: {
      // --------------------------------------------------------------------
      // TESTING 'localTimeOffset' METHOD
      //  The 'localTimeOffset' function returns the difference between the
      //  UTC and local times.  We have to verify that 1) the offset should
      //  remain constant over time (unless the time zone changes) and 2) the
      //  offset should be very close to the difference in results returned by
      //  'nowAsDatetimeUtc' and 'nowAsDatetimeLocal'.
      //
      // Plan:
      //  First create a bdet_TimeInterval object and then load the local time
      //  offset.  Call 'localtimeOffset' several times and verify that the
      //  results are always equal to the first bdet_TimeInterval object.
      //
      //  Next, call 'nowAsDatetimeLocal' first and then 'nowAsDatetimeUtc'.
      //  Verify that the difference in results is no greater than the local
      //  time offset returned by 'localTimeOffset'.  Then call those methods
      //  in reverse order, and verify that the difference is no smaller than
      //  the local time offset.
      //
      // Testing:
      //  bdet_Datetime localTimeOffset();
      // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'localTimeOffset"
                          << "\n========================"
                          << endl;

        bdet_DatetimeInterval i1;

        i1 = bdetu_SystemTime::localTimeOffset();
        const bdet_DatetimeInterval& i2 = i1;

        ASSERT(i2 == bdetu_SystemTime::localTimeOffset());

        for (int i = 0; i < 10; ++i) {
            bdet_Datetime dt1 = bdetu_SystemTime::nowAsDatetimeLocal();
            bdet_Datetime dt2 = bdetu_SystemTime::nowAsDatetimeUtc();
            if (veryVerbose) { P_(dt1); P_(dt2); P(i2); }
            ASSERT(i2 == bdetu_SystemTime::localTimeOffset());
            ASSERT(i2 >= dt1 - dt2);
        }

        for (int i = 0; i < 10; ++i) {
            bdet_Datetime dt1 = bdetu_SystemTime::nowAsDatetimeUtc();
            bdet_Datetime dt2 = bdetu_SystemTime::nowAsDatetimeLocal();
            if (veryVerbose) { P_(dt1); P_(dt2); P(i2); }
            ASSERT(i2 == bdetu_SystemTime::localTimeOffset());
            ASSERT(i2 <= dt2 - dt1);
        }

      } break;
      case 10: {
      // --------------------------------------------------------------------
      // TESTING 'nowAsDatetimeLocal' METHOD
      //  The 'nowAsDatetimeUtc' function returns a 'bdet_DatetimeInterval'
      //  value representing the current local time.  We have to verify that
      //  each subsequent call to 'nowAsDatetimeLocal' reports a time that is
      //  non-decreasing.
      //
      // Plan:
      //  First create two bdet_Datetime objects and then load the
      //  local time.  Then verify that the local time is non-decreasing.
      //
      // Testing:
      //  bdet_Datetime nowAsDatetimeLocal();
      // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'nowAsDatetimeLocal"
                          << "\n==========================="
                          << endl;

        bdet_Datetime dt1;
        ASSERT( 1 == dt1.date().year());
        ASSERT( 1 == dt1.date().month());
        ASSERT( 1 == dt1.date().day());
        ASSERT(24 == dt1.time().hour());
        ASSERT( 0 == dt1.time().minute());
        ASSERT( 0 == dt1.time().second());
        ASSERT( 0 == dt1.time().millisecond());

        dt1 = bdetu_SystemTime::nowAsDatetimeLocal();

        bdet_Datetime dt2;
        ASSERT( 1 == dt2.date().year());
        ASSERT( 1 == dt2.date().month());
        ASSERT( 1 == dt2.date().day());
        ASSERT(24 == dt2.time().hour());
        ASSERT( 0 == dt2.time().minute());
        ASSERT( 0 == dt2.time().second());
        ASSERT( 0 == dt2.time().millisecond());

        dt2 = bdetu_SystemTime::nowAsDatetimeLocal();

        ASSERT (dt1 <= dt2);

        for (int i = 0; i < 10; ++i) {
            dt1 = dt2;
            dt2 = bdetu_SystemTime::nowAsDatetimeLocal();
            ASSERT (dt1 <= dt2);
        }

      } break;
      case 9: {
      // --------------------------------------------------------------------
      // TESTING 'nowAsDatetime' METHOD - stress testing for monotonicity
      //  The 'nowAsDatetime' function returns a 'bdet_TimeInterval' value
      //  representing the current system time using the currently installed
      //  callback function.  We have to verify that each subsequent call to
      //  'nowAsDatetime' reports a time that is non-decreasing.
      //
      // Plan:
      //  Exercise the method in a loop a large, configurable number of times,
      //  ('nowAsDatetime' runs in approximately 0.0000016 sec per iteration)
      //  verifying in each iteration that the system time is non-decreasing.
      //
      // Testing:
      //  bdet_TimeInterval nowAsDatetime()
      // --------------------------------------------------------------------
        if (verbose)
            cout << "\nStress-testing monotonic properties of 'nowAsDatetime'"
                    "\n======================================================"
                 << endl;
        {
            bdet_DatetimeInterval TOLERANCE(0, 0, 0, 0, 1); // 1ms

            enum {
                NUM_ITERATIONS = 120,
                NUM_TEST_PER_ITERATION = 1000,
                OUTPUT_WIDTH = 60,
                OUTPUT_FREQ = NUM_ITERATIONS / OUTPUT_WIDTH
            };

            int iterations = verbose ? atoi(argv[2]) : NUM_ITERATIONS;
            int output_freq = iterations < OUTPUT_WIDTH ? 1
                            : iterations / OUTPUT_WIDTH;
            int testsPerIteration = NUM_TEST_PER_ITERATION;

            if (veryVerbose) {
                testsPerIteration = atoi(argv[3]);
                const char P1[] = "0%";
                const char P2[] = "50%";
                const char P3[] = "100%";
                int hl = OUTPUT_WIDTH / 2;
                cout << P1;

                for (unsigned i=0; i < hl - sizeof(P1) - sizeof(P2) + 4; ++i) {
                    cout << "-";
                }
                cout << P2;
                for (unsigned i = 0; i < hl - sizeof(P3); ++i) {
                    cout << "-";
                }
                cout << P3 << endl;
            }

            for (int i = 0; i < iterations; ++i) {
                bdet_Datetime prev = bdetu_SystemTime::nowAsDatetime();
                for (int j = 0; j < testsPerIteration; ++j) {
                    bdet_Datetime now = bdetu_SystemTime::nowAsDatetime();
                    if (prev > now) {
                        cout << "*** Warning: system time is not "
                             << "reliably monotonic on this platform\n."
                             << "*** Allowing a tolerance of 1ms "
                             << "in test driver.\n";
                        LOOP4_ASSERT(i, j, prev, now, prev - TOLERANCE <= now);
                    }
                    LOOP4_ASSERT(i, j, prev, now, prev <= now);
                    prev = now;
                }
                if (veryVerbose && 0 == i % output_freq) {
                    if (0 == i % (OUTPUT_WIDTH / 4 * output_freq)) {
                        cout << "|" << flush;
                    }
                    else {
                        cout << "+" << flush;
                    }
                }
            }
            if (veryVerbose) cout << "|" << endl;
        }

      } break;
      case 8: {
      // --------------------------------------------------------------------
      // TESTING 'nowAsDatetimeUtc' METHOD - stress testing for monotonicity
      //  The 'nowAsDatetimeUtc' function returns a 'bdet_TimeInterval' value
      //  representing the current system time using the currently installed
      //  callback function.  We have to verify that each subsequent call to
      //  'nowAsDatetimeUtc' reports a time that is non-decreasing.
      //
      // Plan:
      //  Exercise the method in a loop a large, configurable number of times,
      //  ('nowAsDatetimeUtc' runs in approximately 0.0000016 sec per
      //  iteration) verifying in each iteration that the system time is
      //  non-decreasing.
      //
      // Testing:
      //  bdet_TimeInterval nowAsDatetimeUtc()
      // --------------------------------------------------------------------
        if (verbose)
            cout << "\nStress-testing monotonicity of 'nowAsDatetimeUtc'"
                    "\n================================================="
                 << endl;
        {
            bdet_DatetimeInterval TOLERANCE(0, 0, 0, 0, 1); // 1ms

            enum {
                NUM_ITERATIONS = 120,
                NUM_TEST_PER_ITERATION = 1000,
                OUTPUT_WIDTH = 60,
                OUTPUT_FREQ = NUM_ITERATIONS / OUTPUT_WIDTH
            };

            int iterations = verbose ? atoi(argv[2]) : NUM_ITERATIONS;
            int output_freq = iterations < OUTPUT_WIDTH ? 1
                            : iterations / OUTPUT_WIDTH;
            int testsPerIteration = NUM_TEST_PER_ITERATION;

            if (veryVerbose) {
                testsPerIteration = atoi(argv[3]);
                const char P1[] = "0%";
                const char P2[] = "50%";
                const char P3[] = "100%";
                int hl = OUTPUT_WIDTH / 2;
                cout << P1;

                for (unsigned i=0; i < hl - sizeof(P1) - sizeof(P2) + 4; ++i) {
                    cout << "-";
                }
                cout << P2;
                for (unsigned i = 0; i < hl - sizeof(P3); ++i) {
                    cout << "-";
                }
                cout << P3 << endl;
            }

            for (int i = 0; i < iterations; ++i) {
                bdet_Datetime prev = bdetu_SystemTime::nowAsDatetimeUtc();
                for (int j = 0; j < testsPerIteration; ++j) {
                    bdet_Datetime now = bdetu_SystemTime::nowAsDatetimeUtc();
                    if (prev > now) {
                        cout << "*** Warning: system time is not "
                             << "reliably monotonic on this platform\n."
                             << "*** Allowing a tolerance of 1ms "
                             << "in test driver.\n";
                        LOOP4_ASSERT(i, j, prev, now, prev - TOLERANCE <= now);
                    }
                    LOOP4_ASSERT(i, j, prev, now, prev <= now);
                    prev = now;
                }
                if (veryVerbose && 0 == i % output_freq) {
                    if (0 == i % (OUTPUT_WIDTH / 4 * output_freq)) {
                        cout << "|" << flush;
                    }
                    else {
                        cout << "+" << flush;
                    }
                }
            }
            if (veryVerbose) cout << "|" << endl;
        }

      } break;
      case 7: {
      // --------------------------------------------------------------------
      // TESTING 'now' METHOD - stress testing for monotonicity
      //  The 'now' function returns a 'bdet_TimeInterval' value representing
      //  the current system time using the currently installed callback
      //  function.  We have to verify that each subsequent call to 'now'
      //  reports a time that is non-decreasing.
      //
      // Plan:
      //  Exercise the method in a loop a large, configurable number of times,
      //  ('now' runs in approximately 0.00000045 sec per iteration) verifying
      //  in each iteration that the system time is non-decreasing.
      //
      // Testing:
      //  bdet_TimeInterval now()
      // --------------------------------------------------------------------

        if (verbose)
            cout << "\nStress-testing monotonic properties of 'now'"
                    "\n============================================" << endl;
        {
            bdet_TimeInterval TOLERANCE(0.001); // 1ms

            enum {
                NUM_ITERATIONS = 120,
                NUM_TEST_PER_ITERATION = 1000,
                OUTPUT_WIDTH = 60,
                OUTPUT_FREQ = NUM_ITERATIONS / OUTPUT_WIDTH
            };

            int iterations = verbose ? atoi(argv[2]) : NUM_ITERATIONS;
            int output_freq = iterations < OUTPUT_WIDTH ? 1
                            : iterations / OUTPUT_WIDTH;
            int testsPerIteration = NUM_TEST_PER_ITERATION;

            if (veryVerbose) {
                testsPerIteration = atoi(argv[3]);
                const char P1[] = "0%";
                const char P2[] = "50%";
                const char P3[] = "100%";
                int hl = OUTPUT_WIDTH / 2;
                cout << P1;

                for (unsigned i=0; i < hl - sizeof(P1) - sizeof(P2) + 4; ++i) {
                    cout << "-";
                }
                cout << P2;
                for (unsigned i = 0; i < hl - sizeof(P3); ++i) {
                    cout << "-";
                }
                cout << P3 << endl;
            }
            for (int i = 0; i < iterations; ++i) {
                bdet_TimeInterval prev = bdetu_SystemTime::now();
                for (int j = 0; j < testsPerIteration; ++j) {
                    bdet_TimeInterval now = bdetu_SystemTime::now();
                    if (prev > now) {
                        cout << "*** Warning: system time is not "
                             << "reliably monotonic on this platform\n."
                             << "*** Allowing a tolerance of 1ms "
                             << "in test driver.\n";
                        LOOP4_ASSERT(i, j, prev, now, prev - TOLERANCE <= now);
                    }
                    LOOP4_ASSERT(i, j, prev, now, prev <= now);
                    prev = now;
                }
                if (veryVerbose && 0 == i % output_freq) {
                    if (0 == i % (OUTPUT_WIDTH / 4 * output_freq)) {
                        cout << "|" << flush;
                    }
                    else {
                        cout << "+" << flush;
                    }
                }
            }
            if (veryVerbose) cout << "|" << endl;
        }

      } break;
      case 6: {
      // --------------------------------------------------------------------
      // TESTING 'nowAsDatetime' METHOD
      //  The 'nowAsDatetime' function returns a 'bdet_DatetimeInterval' value
      //  representing the current system time using the currently installed
      //  callback function.  We have to verify that each subsequent call to
      //  'nowAsDatetime' reports a time that is non-decreasing.
      //
      // Plan:
      //  First create several bdet_TimeInterval objects and then load the
      //  system time by using default callback function.  Then verify that
      //  the system time is non-decreasing.
      //
      //  For 'nowAsDatetime' create several bdet_DatetimeInterval objects and
      //  then load the system time by using default callback function.  Then
      //  verify that the system time is non-decreasing.
      //
      //
      // Testing:
      //  bdet_Datetime nowAsDatetime()
      // --------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting 'nowAsDatetime' with default implementation"
            "\n==================================================="
                          << endl;
        {
            bdet_TimeInterval i1;
            ASSERT( 0 == i1 );

            bdet_Datetime dt1;
            ASSERT( 1 == dt1.date().year());
            ASSERT( 1 == dt1.date().month());
            ASSERT( 1 == dt1.date().day());
            ASSERT(24 == dt1.time().hour());
            ASSERT( 0 == dt1.time().minute());
            ASSERT( 0 == dt1.time().second());
            ASSERT( 0 == dt1.time().millisecond());

            i1 = bdetu_SystemTime::now();
            ASSERT( 0 != i1 );

            dt1 = bdetu_SystemTime::nowAsDatetime();
            bdet_DatetimeInterval dti = dt1-bdetu_Epoch::epoch();
            ASSERT(0 <= inOrder(i1, dti) ); // i1 <= dt1

            bdet_TimeInterval i2;
            ASSERT( 0 == i2);

            i2 = bdetu_SystemTime::now();
            ASSERT( 0 != i2 );
            ASSERT( 0 <= inOrder(dti, i2) );    // dt1 <= i2
            ASSERT( i2 >= i1 );

            bdet_TimeInterval i3;
            ASSERT( 0 == i3);
            i3 = bdetu_SystemTime::now();
            ASSERT( 0 != i3 );

            bdet_TimeInterval i4;
            ASSERT( 0 == i4);
            i4 = bdetu_SystemTime::now();
            ASSERT( 0 != i4 );
            ASSERT( i4 >= i3 );

            bdet_TimeInterval i5;
            ASSERT( 0 == i5);
            i5 = bdetu_SystemTime::now();
            ASSERT( 0 != i5 );
            ASSERT( i5 >= i4 );
        }

        if (verbose)
            cout << "\nTesting 'nowAsDatetime' with user defined functions"
                    "\n==================================================="
                 << endl;
        {
            bdetu_SystemTime::setSystemTimeCallback(&getClientTime);
            ASSERT(&getClientTime
                             == bdetu_SystemTime::currentSystemTimeCallback());

            bdet_TimeInterval i1;
            ASSERT( 0 == i1 );

            i1 = bdetu_SystemTime::now();
            ASSERT( 1 == i1.seconds() && 1 == i1.nanoseconds() );

            bdet_Datetime dt1;
            ASSERT( 1 == dt1.date().year());
            ASSERT( 1 == dt1.date().month());
            ASSERT( 1 == dt1.date().day());
            ASSERT(24 == dt1.time().hour());
            ASSERT( 0 == dt1.time().minute());
            ASSERT( 0 == dt1.time().second());
            ASSERT( 0 == dt1.time().millisecond());

            dt1 = bdetu_SystemTime::nowAsDatetime();
            ASSERT( 1970 == dt1.date().year());
            ASSERT( 1 == dt1.date().month());
            ASSERT( 1 == dt1.date().day());
            ASSERT( 0 == dt1.time().hour());
            ASSERT( 0 == dt1.time().minute());
            ASSERT( 1 == dt1.time().second());
            ASSERT( 0 == dt1.time().millisecond());

            bdet_DatetimeInterval dti = dt1-bdetu_Epoch::epoch();

            ASSERT(0 == inOrder(i1, dti)); // i1 == dti == 1 second interval

            bdetu_SystemTime::setSystemTimeCallback(&IncrementInterval);
            ASSERT(&IncrementInterval
                             == bdetu_SystemTime::currentSystemTimeCallback());

            bdet_TimeInterval i2;
            ASSERT( 0 == i2);
            i2 = bdetu_SystemTime::now();
            ASSERT( 0 == i2 );

            bdet_Datetime dt2 = bdetu_SystemTime::nowAsDatetime();
            ASSERT( 1970 == dt2.date().year());
            ASSERT( 1 == dt2.date().month());
            ASSERT( 1 == dt2.date().day());
            ASSERT( 0 == dt2.time().hour());
            ASSERT( 0 == dt2.time().minute());
            ASSERT( 1 == dt2.time().second());
            ASSERT( 0 == dt2.time().millisecond());
            bdet_DatetimeInterval dti2 = dt2-bdetu_Epoch::epoch();
            ASSERT( 1 == inOrder(i2, dti2) );    // i2 < dt2-epoch()

            bdet_Datetime dt3 = bdetu_SystemTime::nowAsDatetime();
            ASSERT( 1970 == dt3.date().year());
            ASSERT( 1 == dt3.date().month());
            ASSERT( 1 == dt3.date().day());
            ASSERT( 0 == dt3.time().hour());
            ASSERT( 0 == dt3.time().minute());
            ASSERT( 2 == dt3.time().second());
            ASSERT( 0 == dt3.time().millisecond());
            ASSERT( dt2 < dt3);

            bdet_TimeInterval i3;
            ASSERT( 0 == i3);
            i3 = bdetu_SystemTime::now();
            bdet_DatetimeInterval dti3 = dt3-bdetu_Epoch::epoch();
            ASSERT( 1 == inOrder(dti3, i3) );    // dt3 < i2
            ASSERT( i3 >= i2 );

            bdet_TimeInterval i4;
            ASSERT( 0 == i4);
            i4 = bdetu_SystemTime::now();
            ASSERT( i4 >= i3 );

            bdet_TimeInterval i5;
            ASSERT( 0 == i5);
            i5 = bdetu_SystemTime::now();
            ASSERT( i5 >= i4 );

            bdet_TimeInterval i6;
            ASSERT( 0 == i6);
            i6 = bdetu_SystemTime::now();
            ASSERT( i6 >= i5 );

            bdetu_SystemTime::setSystemTimeCallback(&getClientTime);
            ASSERT(&getClientTime
                             == bdetu_SystemTime::currentSystemTimeCallback());

            bdet_TimeInterval i7;
            ASSERT( 0 == i7 );

            i7 = bdetu_SystemTime::now();
            ASSERT( 1 == i7.seconds() && 1 == i7.nanoseconds() );
        }
      } break;
      case 5: {
      // --------------------------------------------------------------------
      // TESTING 'now' and 'nowAsDatetimeUtc' METHODS
      //  The 'now' function returns a 'bdet_TimeInterval' value representing
      //  the current system time using the currently installed callback
      //  function.  We have to verify that each subsequent call to 'now'
      //  reports a time that is non-decreasing.
      //
      //  The 'nowAsDatetimeUtc' function returns a 'bdet_DatetimeInterval'
      //  value representing the current system time using the currently
      //  installed callback function.  We have to verify that each subsequent
      //  call to 'nowAsDatetimeUtc' reports a time that is non-decreasing.
      //
      // Plan:
      //  First create several bdet_TimeInterval objects and then load the
      //  system time by using default callback function.  Then verify that
      //  the system time is non-decreasing.
      //
      //  For 'nowAsDatetimeUtc' create several bdet_DatetimeInterval objects
      //  and then load the system time by using default callback function.
      //  Then verify that the system time is non-decreasing.
      //
      //
      // Testing:
      //  bdet_TimeInterval now()
      //  bdet_Datetime nowAsDatetimeUtc()
      // --------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting 'now' and 'nowAsDatetimeUtc' (default implementation)"
            "\n============================================================="
                          << endl;
        {
            bdet_TimeInterval i1;
            ASSERT( 0 == i1 );

            bdet_Datetime dt1;
            ASSERT( 1 == dt1.date().year());
            ASSERT( 1 == dt1.date().month());
            ASSERT( 1 == dt1.date().day());
            ASSERT(24 == dt1.time().hour());
            ASSERT( 0 == dt1.time().minute());
            ASSERT( 0 == dt1.time().second());
            ASSERT( 0 == dt1.time().millisecond());

            i1 = bdetu_SystemTime::now();
            ASSERT( 0 != i1 );

            dt1 = bdetu_SystemTime::nowAsDatetimeUtc();
            bdet_DatetimeInterval dti = dt1-bdetu_Epoch::epoch();
            ASSERT(0 <= inOrder(i1, dti) ); // i1 <= dt1

            bdet_TimeInterval i2;
            ASSERT( 0 == i2);

            i2 = bdetu_SystemTime::now();
            ASSERT( 0 != i2 );
            ASSERT( 0 <= inOrder(dti, i2) );    // dt1 <= i2
            ASSERT( i2 >= i1 );

            bdet_TimeInterval i3;
            ASSERT( 0 == i3);
            i3 = bdetu_SystemTime::now();
            ASSERT( 0 != i3 );

            bdet_TimeInterval i4;
            ASSERT( 0 == i4);
            i4 = bdetu_SystemTime::now();
            ASSERT( 0 != i4 );
            ASSERT( i4 >= i3 );

            bdet_TimeInterval i5;
            ASSERT( 0 == i5);
            i5 = bdetu_SystemTime::now();
            ASSERT( 0 != i5 );
            ASSERT( i5 >= i4 );
        }

        if (verbose)
            cout << "\nTesting 'now' and 'nowAsDatetimeUtc' with user "
                    "defined functions"
                    "\n==============================================="
                    "=================" << endl;
        {
            bdetu_SystemTime::setSystemTimeCallback(&getClientTime);
            ASSERT(&getClientTime
                             == bdetu_SystemTime::currentSystemTimeCallback());

            bdet_TimeInterval i1;
            ASSERT( 0 == i1 );

            i1 = bdetu_SystemTime::now();
            ASSERT( 1 == i1.seconds() && 1 == i1.nanoseconds() );

            bdet_Datetime dt1;
            ASSERT( 1 == dt1.date().year());
            ASSERT( 1 == dt1.date().month());
            ASSERT( 1 == dt1.date().day());
            ASSERT(24 == dt1.time().hour());
            ASSERT( 0 == dt1.time().minute());
            ASSERT( 0 == dt1.time().second());
            ASSERT( 0 == dt1.time().millisecond());

            dt1 = bdetu_SystemTime::nowAsDatetimeUtc();
            ASSERT( 1970 == dt1.date().year());
            ASSERT( 1 == dt1.date().month());
            ASSERT( 1 == dt1.date().day());
            ASSERT( 0 == dt1.time().hour());
            ASSERT( 0 == dt1.time().minute());
            ASSERT( 1 == dt1.time().second());
            ASSERT( 0 == dt1.time().millisecond());

            bdet_DatetimeInterval dti = dt1-bdetu_Epoch::epoch();

            ASSERT(0 == inOrder(i1, dti)); // i1 == dti == 1 second interval

            bdetu_SystemTime::setSystemTimeCallback(&IncrementInterval);
            ASSERT(&IncrementInterval
                             == bdetu_SystemTime::currentSystemTimeCallback());

            bdet_TimeInterval i2;
            ASSERT( 0 == i2);
            i2 = bdetu_SystemTime::now();
            ASSERT( 0 == i2 );

            bdet_Datetime dt2 = bdetu_SystemTime::nowAsDatetimeUtc();
            ASSERT( 1970 == dt2.date().year());
            ASSERT( 1 == dt2.date().month());
            ASSERT( 1 == dt2.date().day());
            ASSERT( 0 == dt2.time().hour());
            ASSERT( 0 == dt2.time().minute());
            ASSERT( 1 == dt2.time().second());
            ASSERT( 0 == dt2.time().millisecond());
            bdet_DatetimeInterval dti2 = dt2-bdetu_Epoch::epoch();
            ASSERT( 1 == inOrder(i2, dti2) );    // i2 < dt2-epoch()

            bdet_Datetime dt3 = bdetu_SystemTime::nowAsDatetimeUtc();
            ASSERT( 1970 == dt3.date().year());
            ASSERT( 1 == dt3.date().month());
            ASSERT( 1 == dt3.date().day());
            ASSERT( 0 == dt3.time().hour());
            ASSERT( 0 == dt3.time().minute());
            ASSERT( 2 == dt3.time().second());
            ASSERT( 0 == dt3.time().millisecond());
            ASSERT( dt2 < dt3);

            bdet_TimeInterval i3;
            ASSERT( 0 == i3);
            i3 = bdetu_SystemTime::now();
            bdet_DatetimeInterval dti3 = dt3-bdetu_Epoch::epoch();
            ASSERT( 1 == inOrder(dti3, i3) );    // dt3 < i2
            ASSERT( i3 >= i2 );

            bdet_TimeInterval i4;
            ASSERT( 0 == i4);
            i4 = bdetu_SystemTime::now();
            ASSERT( i4 >= i3 );

            bdet_TimeInterval i5;
            ASSERT( 0 == i5);
            i5 = bdetu_SystemTime::now();
            ASSERT( i5 >= i4 );

            bdet_TimeInterval i6;
            ASSERT( 0 == i6);
            i6 = bdetu_SystemTime::now();
            ASSERT( i6 >= i5 );

            bdetu_SystemTime::setSystemTimeCallback(&getClientTime);
            ASSERT(&getClientTime
                             == bdetu_SystemTime::currentSystemTimeCallback());

            bdet_TimeInterval i7;
            ASSERT( 0 == i7 );

            i7 = bdetu_SystemTime::now();
            ASSERT( 1 == i7.seconds() && 1 == i7.nanoseconds() );
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING static int inOrder()
        //
        // Plan:
        //   Apply the function to a set of carefully chosen challenge vectors,
        //   checking that the function returns the expected results.
        //
        // Testing:
        //   static int inOrder(const bdet_TimeInterval&     lhs,
        //                      const bdet_DatetimeInterval& rhs)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting helper function 'inOrder'"
                             "\n=================================" << endl;

        {
            enum {
                 NANOSEC_PER_MILLISEC = 1000000,
                 MILLISEC_PER_SEC     = 1000,
                 MILLISEC_PER_MIN     = 60 * MILLISEC_PER_SEC,
                 MILLISEC_PER_HOUR    = 60 * MILLISEC_PER_MIN,
                 MILLISEC_PER_DAY     = 24 * MILLISEC_PER_HOUR,
                 SEC_PER_DAY          = MILLISEC_PER_DAY / MILLISEC_PER_SEC
            };

            static const struct DtiTi {
                bdet_DatetimeInterval    dti;       // sample DatetimeInterval
                bdet_TimeInterval        ti;        // sample TimeInterval
                bsls_PlatformUtil::Int64 diff;      // ti - dti in milliseconds
                int                      inorder;   // predicted value
            } dtDiff []= {
//--------------^
{bdet_DatetimeInterval(),                   bdet_TimeInterval(0, 0),  0,  0},
{bdet_DatetimeInterval(0, 0, 0, 0, 1),      bdet_TimeInterval(0, 0), -1, -1},
{bdet_DatetimeInterval(), bdet_TimeInterval(0, NANOSEC_PER_MILLISEC),  1, 1},
{bdet_DatetimeInterval( 25000),
     bdet_TimeInterval( 25000 * (bsls_PlatformUtil::Int64)SEC_PER_DAY,
                                              NANOSEC_PER_MILLISEC),  1,  1},
{bdet_DatetimeInterval(-25000),
     bdet_TimeInterval(-25000 * (bsls_PlatformUtil::Int64)SEC_PER_DAY,
                                             -NANOSEC_PER_MILLISEC), -1, -1},
{bdet_DatetimeInterval(10000),
         bdet_TimeInterval(10000 * SEC_PER_DAY, NANOSEC_PER_MILLISEC), 1, 1},
{bdet_DatetimeInterval(1),          bdet_TimeInterval(SEC_PER_DAY, 0), 0, 0},
{bdet_DatetimeInterval(),                     bdet_TimeInterval(0, 1), 0, 0},
{bdet_DatetimeInterval(),                     bdet_TimeInterval(0,-1), 0, 0},
{bdet_DatetimeInterval(0, 0, 0, 0, 1),
                           bdet_TimeInterval(0, NANOSEC_PER_MILLISEC), 0, 0},
//--------------v
            };

            const int NUM_TESTS = sizeof(dtDiff) / sizeof(DtiTi);

            for (int i=0; i < NUM_TESTS; ++i) {
                ASSERT(inOrder(dtDiff[i].dti, dtDiff[i].ti) ==
                                                            dtDiff[i].inorder);
                ASSERT(inOrder(dtDiff[i].ti, dtDiff[i].dti) ==
                                                           -dtDiff[i].inorder);
            }
        }
    } break;
    case 3: {
      // --------------------------------------------------------------------
      // TESTING 'loadCurrentTime' METHOD
      //   The 'loadCurrentTime' function loads the current system time into
      //   specified 'result' using the currently installed callback mechanism.
      //   We have to verify that each subsequent call to 'loadCurrentTime'
      //   reports a time that is non-decreasing.
      //
      //   reports a time that is non-decreasing.
      //
      // Plan:
      //   First create several bdet_TimeInterval objects and then load the
      //   system time by using default callback function.  Then verify that
      //   system time is non-decreasing.
      //
      //
      // Testing:
      //   loadCurrentTime(bdet_TimeInterval *result)
      // --------------------------------------------------------------------

      if (verbose)
        cout << "\nTesting 'loadCurrentTime' with default implementation"
             << "\n====================================================="
             << endl;
      {
          bdet_TimeInterval i1;
          ASSERT( 0 == i1 );

          bdetu_SystemTime::loadCurrentTime(&i1);
          ASSERT( 0 != i1 );

          bdet_TimeInterval i2;
          ASSERT( 0 == i2);
          bdetu_SystemTime::loadCurrentTime(&i2);
          ASSERT( 0 != i2 );
          ASSERT( i2 >= i1 );

          bdet_TimeInterval i3;
          ASSERT( 0 == i3);
          bdetu_SystemTime::loadCurrentTime(&i3);
          ASSERT( 0 != i3 );

          bdet_TimeInterval i4;
          ASSERT( 0 == i4);
          bdetu_SystemTime::loadCurrentTime(&i4);
          ASSERT( 0 != i4 );
          ASSERT( i4 >= i3 );

          bdet_TimeInterval i5;
          ASSERT( 0 == i5);
          bdetu_SystemTime::loadCurrentTime(&i5);
          ASSERT( 0 != i5 );
          ASSERT( i5 >= i4 );
        }

        if (verbose)
          cout << "\nTesting 'loadCurrentTime' with user defined functions"
               << "\n====================================================="
               << endl;
        {
            bdetu_SystemTime::setSystemTimeCallback(&getClientTime);
            ASSERT(&getClientTime
                             == bdetu_SystemTime::currentSystemTimeCallback());

            bdet_TimeInterval i1;
            ASSERT( 0 == i1 );

            bdetu_SystemTime::loadCurrentTime(&i1);
            ASSERT( 1 == i1.seconds() && 1 == i1.nanoseconds() );

            bdetu_SystemTime::setSystemTimeCallback(&IncrementInterval);
            ASSERT(&IncrementInterval
                             == bdetu_SystemTime::currentSystemTimeCallback());

            bdet_TimeInterval i2;
            ASSERT( 0 == i2);
            bdetu_SystemTime::loadCurrentTime(&i2);
            ASSERT( 0 == i2 );

            bdet_TimeInterval i3;
            ASSERT( 0 == i3);
            bdetu_SystemTime::loadCurrentTime(&i3);
            ASSERT( i3 >= i2 );

            bdet_TimeInterval i4;
            ASSERT( 0 == i4);
            bdetu_SystemTime::loadCurrentTime(&i4);
            ASSERT( i4 >= i3 );

            bdet_TimeInterval i5;
            ASSERT( 0 == i5);
            bdetu_SystemTime::loadCurrentTime(&i5);
            ASSERT( i5 >= i4 );

            bdet_TimeInterval i6;
            ASSERT( 0 == i6);
            bdetu_SystemTime::loadCurrentTime(&i6);
            ASSERT( i6 >= i5 );

            bdetu_SystemTime::setSystemTimeCallback(&getClientTime);
            ASSERT(&getClientTime
                             == bdetu_SystemTime::currentSystemTimeCallback());

            bdet_TimeInterval i7;
            ASSERT( 0 == i7 );

            bdetu_SystemTime::loadCurrentTime(&i7);
            ASSERT( 1 == i7.seconds() && 1 == i7.nanoseconds() );
        }

      } break;
      case 2: {

        // --------------------------------------------------------------------
        // TESTING CALLBACK RELATED FUNCTIONS
        //  Install the user specified 'callback' function which will be used
        //  to actually retrieve the system time and the differential between
        //  local time and UTC time.
        //
        // Plan:
        // 1 First install default system-time callback function by calling
        //   'setSystemTimeCallback' and then verify it by calling
        //   'currentSystemTimeCallback'.  Next repeat the same process for the
        //   user defined callback function.  Apply the same test again and
        //   load some generic functions and verify them by calling
        //   'currentSystemTimeCallback'.
        //
        // 2 Then install local-time-offset default callback function by calling
        //   'setLocalTimeOffsetCallback' and then verify it by calling
        //   'currentLocalTimeOffsetCallback'.  Next repeat the same process
        //   for the user defined callback function.  Apply the same test again
        //   and load some generic functions and verify them by calling
        //   'currentLocalTimeOffsetCallback'.
        //
        // Testing:
        //   setSystemTimeCallback(SystemTimeCallback callback)
        //   SystemTimeCallback currentSystemTimeCallback()
        //   setLocalTimeOffsetCallback(SystemLocalTimeOffsetCallback callback)
        //   SystemLocalTimeOffsetCallback currentLocalTimeOffsetCallback()
        // --------------------------------------------------------------------

        if (verbose) cout <<
            "\nTesting 'setSystemTimeCallback' and 'currentSystemTimeCallback'"
            "\n==============================================================="
            << endl;
        {
           bdetu_SystemTime::SystemTimeCallback callback_default_p1;
           callback_default_p1  = &bdetu_SystemTime::loadSystemTimeDefault;

           bdetu_SystemTime::setSystemTimeCallback(callback_default_p1);

           bdetu_SystemTime::SystemTimeCallback callback_default_p2;
           callback_default_p2 = bdetu_SystemTime::currentSystemTimeCallback();
           ASSERT( callback_default_p2 == callback_default_p1);

           const bdetu_SystemTime::SystemTimeCallback callback_user_p1
                                                     = &getClientTime;
           bdetu_SystemTime::setSystemTimeCallback(callback_user_p1);
           ASSERT(callback_user_p1
                             == bdetu_SystemTime::currentSystemTimeCallback());
        }

        if (verbose)
           cout << "\nTesting 'setSystemTimeCallback' with some generic user "
                   "defined functions"
                   "\n======================================================="
                   "=================" << endl;
        {
            const bdetu_SystemTime::SystemTimeCallback user_p1 = &f1;
            bdetu_SystemTime::setSystemTimeCallback(user_p1);
            ASSERT( user_p1 == bdetu_SystemTime::currentSystemTimeCallback() );

            const bdetu_SystemTime::SystemTimeCallback user_p2 = &f2;
            bdetu_SystemTime::setSystemTimeCallback(user_p2);
            ASSERT( user_p2 == bdetu_SystemTime::currentSystemTimeCallback() );
        }

        if (verbose) cout << "\nTesting 'setLocalTimeOffsetCallback' "
                             "and 'currentLocalTimeOffsetCallback'"
                             "\n====================================="
                             "===================================="
                          << endl;
        {
           bdetu_SystemTime::LocalTimeOffsetCallback callback_default_p1
                               = &bdetu_SystemTime::loadLocalTimeOffsetDefault;

           bdetu_SystemTime::setLocalTimeOffsetCallback(callback_default_p1);

           bdetu_SystemTime::LocalTimeOffsetCallback callback_default_p2
                          = bdetu_SystemTime::currentLocalTimeOffsetCallback();
           ASSERT( callback_default_p2 == callback_default_p1);

           const bdetu_SystemTime::LocalTimeOffsetCallback callback_user_p1
                                                         = &getLocalTimeOffset;
           bdetu_SystemTime::setLocalTimeOffsetCallback(callback_user_p1);
           ASSERT(callback_user_p1
                        == bdetu_SystemTime::currentLocalTimeOffsetCallback());
        }

        if (verbose) cout
            << "\nTesting 'setLocalTimeOffsetCallback' with some generic user "
               "defined functions"
               "\n============================================================"
               "================="
            << endl;
        {
            const bdetu_SystemTime::LocalTimeOffsetCallback user_p1 = &f3;
            bdetu_SystemTime::setLocalTimeOffsetCallback(user_p1);
            ASSERT(user_p1
                        == bdetu_SystemTime::currentLocalTimeOffsetCallback());

            const bdetu_SystemTime::LocalTimeOffsetCallback user_p2 = &f4;
            bdetu_SystemTime::setLocalTimeOffsetCallback(user_p2);
            ASSERT(user_p2
                        == bdetu_SystemTime::currentLocalTimeOffsetCallback());
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING 'loadSystemTimeDefault' and 'loadLocalTimeOffsetDefault'
        //   Provides a default implementation for system time retrieval.
        //   The obtained system time is expressed as a time interval between
        //   the current time and '00:00 UTC, January 1, 1970'.  On UNIX
        //   (Solaris, LINUX and DG-UNIX) this function provides a
        //   'microseconds' resolution.  On Windows (NT, WIN2000, 95, 98 etc)
        //   it provides resolution in '100*nanoseconds'.
        //
        // Plan:
        // 1 Create 'bdet_TimeInterval' object and then load the system time
        //   into this bdet_TimeInterval object.  Repeat the process, verify
        //   the 'bdet_TimeInterval' created later always compares greater
        //   than the 'bdet_TimeInterval' object created earlier.
        //
        // 2 Call 'loadLocalTimeOffsetDefault' twice, store the return values
        //   to two 'bdet_DatetimeInterval' objects.  Verify these two objects
        //   compare equal.
        //
        // Testing:
        //   loadSystemTimeDefault(bdet_TimeInterval *result)
        //   loadLocalTimeOffsetDefault()
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'loadSystemTimeDefault' METHOD"
                          << "\n======================================"
                          << endl;
        {
            bdet_TimeInterval i1;
            ASSERT( 0 == i1 );
            bdetu_SystemTime::loadSystemTimeDefault(&i1);
            ASSERT( 0 != i1);

#if defined(BSLS_PLATFORM_OS_UNIX)  \
 && !defined(BSLS_PLATFORM_OS_AIX) \
 && !defined(BSLS_PLATFORM_OS_LINUX)
            ASSERT(0 == i1.nanoseconds() % 1000);
#endif
            bdet_TimeInterval i2;
            ASSERT( 0 == i2 );
            bdetu_SystemTime::loadSystemTimeDefault(&i2);
            ASSERT( 0 != i2);
            ASSERT( i2 >= i1 );
#if defined(BSLS_PLATFORM_OS_UNIX)  \
 && !defined(BSLS_PLATFORM_OS_AIX) \
 && !defined(BSLS_PLATFORM_OS_LINUX)
            ASSERT(0 == i2.nanoseconds() % 1000);
#endif
            bdet_TimeInterval i3;
            ASSERT( 0 == i3 );
            bdetu_SystemTime::loadSystemTimeDefault(&i3);
            ASSERT( 0 != i3);
            ASSERT( i3 >= i2 );
#if defined(BSLS_PLATFORM_OS_UNIX)  \
 && !defined(BSLS_PLATFORM_OS_AIX) \
 && !defined(BSLS_PLATFORM_OS_LINUX)
            ASSERT(0 == i3.nanoseconds() % 1000);
#endif
            bdet_TimeInterval i4;
            ASSERT( 0 == i4 );
            bdetu_SystemTime::loadSystemTimeDefault(&i4);
            ASSERT( 0 != i4);
            ASSERT( i4 >= i3 );
#if defined(BSLS_PLATFORM_OS_UNIX)  \
 && !defined(BSLS_PLATFORM_OS_AIX) \
 && !defined(BSLS_PLATFORM_OS_LINUX)
            ASSERT(0 == i4.nanoseconds() % 1000);
#endif
            bdet_TimeInterval i5;
            ASSERT( 0 == i5 );
            bdetu_SystemTime::loadSystemTimeDefault(&i5);
            ASSERT( 0 != i5);
            ASSERT( i5 >= i4 );
#if defined(BSLS_PLATFORM_OS_UNIX)  \
 && !defined(BSLS_PLATFORM_OS_AIX) \
 && !defined(BSLS_PLATFORM_OS_LINUX)
            ASSERT(0 == i5.nanoseconds() % 1000);
#endif
            if (veryVerbose) {
                P_(i1); P_(i2); P(i3);
                P_(i4); P(i5);
            }
        }

        if (verbose) cout << "\nTesting 'loadLocalTimeOffsetDefault' METHOD"
                          << "\n==========================================="
                          << endl;
        {
            bdet_DatetimeInterval i1
                              = bdetu_SystemTime::loadLocalTimeOffsetDefault();
            bdet_DatetimeInterval i2
                              = bdetu_SystemTime::loadLocalTimeOffsetDefault();
            ASSERT(i1 == i2);
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // DETERMINE RESOLUTION OF 'now()'.
        // --------------------------------------------------------------------

        for (int i = 0; i < 10; ++i) {
            bdet_TimeInterval ti1, ti2;

            ti1 = bdetu_SystemTime::now();
            do {
                ti2 = bdetu_SystemTime::now();
            } while (ti2 <= ti1);

            cout << "Resolution: " << (ti2 - ti1).totalSecondsAsDouble() <<
                                                                  " seconds\n";
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
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
