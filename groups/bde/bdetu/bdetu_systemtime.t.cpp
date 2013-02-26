// bdetu_systemtime.t.cpp                                             -*-C++-*-

#include <bdetu_systemtime.h>
#include <bdet_datetimeinterval.h>
#include <bdetu_epoch.h>

#include <bsl_cstdlib.h>     // atoi(), getenv(), putenv(),
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

#include <bsls_stopwatch.h>          // case -2

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
// [13] int lLTOC(int *result, const bdet_Datetime&  utcDatetime);
// [13] int lLTOCDefault(int *result, const bdet_Datetime&  utcDatetime);
// [13] LLTOC setLLTOC(LLTOC callback);
// [13] LLTOC currentLLTOC();
//---------------------------------------------------------------------------
// [14] USAGE example
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

typedef bdetu_SystemTime Obj;

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

// ==========================================================================
//                      HELPER CLASSES FOR TESTING
// --------------------------------------------------------------------------

class MyLocalTimeOffsetUtility {

  // DATA
  static const int *s_status_p;  // address of status returned by callback
  static const int *s_offset_p;  // address of offset returned by callback
  static bool      *s_callbackInvoked_p;
                                 // indication of callback invocation

 public:
  // CLASS METHODS
  static int loadLocalTimeOffset(int                  *result,
                                 const bdet_Datetime&  utcDatetime);
      // Load, to the to specified 'result', value at the address specified by
      // the 'offset' argument of the last invocation of the 'setExternals'
      // method and set to 'true' the value at the address specified by the
      // 'callbackInvoked' argument of last invocation of the 'setExternals'
      // method.  Return the status value at the address specified by the
      // 'status' argument of the last invocation of the 'setExternals' method.
      // The specified 'utcDatetime' is ignored.  The behavior is undefined
      // unless the value at 'callbackInvoked' is initially 'false'.

  static void setExternals(const int *status,
                           const int *offset,
                           bool      *callbackInvoked);
      // Set the the specified addresses 'status' and 'offset' as the source of
      // values returned by the 'localLocalTimeOffset' method.
};

const int *MyLocalTimeOffsetUtility::s_status_p          = 0;
const int *MyLocalTimeOffsetUtility::s_offset_p          = 0;
bool      *MyLocalTimeOffsetUtility::s_callbackInvoked_p = 0;

void MyLocalTimeOffsetUtility::setExternals(const int *status,
                                            const int *offset,
                                            bool      *callbackInvoked)
{
    ASSERT(status);
    ASSERT(offset);

    s_status_p          = status;
    s_offset_p          = offset;
    s_callbackInvoked_p = callbackInvoked;
}

int MyLocalTimeOffsetUtility::loadLocalTimeOffset(
                                             int                  *result,
                                             const bdet_Datetime&  )
{
    ASSERT(result);
    ASSERT(!*s_callbackInvoked_p);

    *result              = *s_offset_p;
    *s_callbackInvoked_p = true;
    return *s_status_p;
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

namespace UsageExample4 {

///Example 4: Using the Local Time Offset Callback
///-----------------------------------------------
// Suppose one has to provide time stamp values that always reflect local time
// for a given location, even when local time transitions into and out of
// daylight saving time.  Further suppose that one must do this quite often
// (e.g., for every record in a high frequency log), so the performance of the
// default method for calculating local time offset is not adequate.  Creation
// and installation of a specialized user-defined callback for local time
// offset allows one to solve this problem.
// 
// First, create a utility class that provides a method of type
// 'bdetu_SystemTime::LoadLocalTimeOffsetCallback' that is valid for the
// location of interest (New York) for the period of interest (the year 2013).
// Note that the transition times into and out of daylight savings for
// New York are given in UTC.
//..
    struct MyLocalTimeOffsetUtilNewYork2013 {

      private:
        // DATA
        static int           s_useCount;
        static bdet_Datetime s_startOfDaylightSavingTime;  // UTC Datetime
        static bdet_Datetime s_resumptionOfStandardTime;   // UTC Datetime

      public:
        // CLASS METHODS
        static int loadLocalTimeOffset(int                  *result,
                                      const bdet_Datetime&  utcDatetime);
            // Load, into the specified 'result', the offset between the local
            // time for the "America/New_York" timezone and UTC at the
            // specified 'utcDatetime'.  The behavior is undefined unless
            // '2013 == utcDatetime.date().year()'.

        static int useCount();
            // Return the number of invocations of the 'loadLocalTimeOffset'
            // since the start of the process.
    };

    // DATA
    int MyLocalTimeOffsetUtilNewYork2013::s_useCount = 0;

    bdet_Datetime
    MyLocalTimeOffsetUtilNewYork2013::s_startOfDaylightSavingTime(2013,
                                                                     3,
                                                                    10,
                                                                     7);
    bdet_Datetime
    MyLocalTimeOffsetUtilNewYork2013::s_resumptionOfStandardTime(2013,
                                                                   11,
                                                                    3,
                                                                    6);
    // CLASS METHODS
    int MyLocalTimeOffsetUtilNewYork2013::loadLocalTimeOffset(
                                             int                  *result,
                                             const bdet_Datetime&  utcDatetime)
    {
        ASSERT(result);
        ASSERT(2013 == utcDatetime.date().year());

        *result = utcDatetime < s_startOfDaylightSavingTime ? -18000:
                  utcDatetime < s_resumptionOfStandardTime  ? -14400:
                                                              -18000;
        ++s_useCount;
        return 0;
    }

    int MyLocalTimeOffsetUtilNewYork2013::useCount()
    {
        return s_useCount;
    }
//..
// Notice that we do not attempt to make the 'loadLocalTimeOffset' method
// 'inline', since we must take its address to install it as the callback.
//..
void main4()
{
//..
// Next, we install this 'loadLocalTimeOffset' as the local time offset
// callback.
//..

    bdetu_SystemTime::LoadLocalTimeOffsetCallback defaultCallback =
                              bdetu_SystemTime::setLoadLocalTimeOffsetCallback(
                                         &MyLocalTimeOffsetUtilNewYork2013::
                                                          loadLocalTimeOffset);

    ASSERT(bdetu_SystemTime::loadLocalTimeOffsetDefault == defaultCallback);
    ASSERT(&MyLocalTimeOffsetUtilNewYork2013::loadLocalTimeOffset
         == bdetu_SystemTime::currentLoadLocalTimeOffsetCallback());
//..
// Now, we can use the 'bdetu_SystemTime::loadLocalTimeOffset' method to obtain
// the local time offsets in New York on several dates of interest.  The
// increasing values from our 'useCount' method assures us that the callback we
// defined is indeed being used.
//..
    ASSERT(0 == MyLocalTimeOffsetUtilNewYork2013::useCount());

    int offset;
    bdet_Datetime     newYearsDay(2013,  1,  1);
    bdet_Datetime independenceDay(2013,  7,  4);
    bdet_Datetime     newYearsEve(2013, 12, 31);

    bdetu_SystemTime::loadLocalTimeOffset(&offset, newYearsDay);
    ASSERT(-5 * 3600 == offset);
    ASSERT(        1 == MyLocalTimeOffsetUtilNewYork2013::useCount());

    bdetu_SystemTime::loadLocalTimeOffset(&offset, independenceDay);
    ASSERT(-4 * 3600 == offset);
    ASSERT(        2 == MyLocalTimeOffsetUtilNewYork2013::useCount());
    
    bdetu_SystemTime::loadLocalTimeOffset(&offset, newYearsEve);
    ASSERT(-5 * 3600 == offset);
    ASSERT(        3 == MyLocalTimeOffsetUtilNewYork2013::useCount());
//..
// Finally, to be neat, we restore the local time offset callback to the
// default callback:
//..
    bdetu_SystemTime::setLoadLocalTimeOffsetCallback(defaultCallback);
    ASSERT(&bdetu_SystemTime::loadLocalTimeOffsetDefault
        == bdetu_SystemTime::currentLoadLocalTimeOffsetCallback());
//..
}
}  // end usage example

// ============================================================================
//                          MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{

    int            test = argc > 1 ? atoi(argv[1]) : 0;
    int         verbose = argc > 2;
    int     veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4; (void) veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
    case 14: {
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

#if 0
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
//
        }
#endif

        if (verbose) cout << "\nTesting Usage Example 4"
                          << "\n=======================" << endl;

        // Restore state changed in Example 3.

        Obj::setSystemTimeCallback(Obj::loadSystemTimeDefault);
        ASSERT(&Obj::loadSystemTimeDefault
            == Obj::currentSystemTimeCallback());

        UsageExample4::main4();

    } break;
    case 13: {
      // --------------------------------------------------------------------
      // LOCAL TIME OFFSET CALLBACK FACILITY
      //
      // Concerns:
      //: 1 The 'loadSystemTimeDefault' method is installed as the default
      //:   callback.
      //:
      //: 2 The 'currentLoadLocalTimeOffsetCallback' method shows the currently
      //:   installed callback.
      //:
      //: 3 The 'setLoadLocalTimeOffsetCallback' returns the previously
      //:   installed callback, and sets the specified callback.
      //:
      //: 4 The installed callback is called by each of the methods that must
      //:   do so, that that the value generated by the callback is reflected
      //:   in the values returned by these methods, and, when relevant, the
      //:   status value provided by the callback is passed through.  The
      //:   relevant methods are:
      //:   1 'nowAsDatetimeLocal'
      //:   2 'localTimeOffset'
      //:   3 'loadLocalTimeOffset'
      //:
      //: 5 The user-defined callback used in this test operates correctly.
      //:
      //: 6 The 'loadSystemTimeDefault' returns expected values.
      //
      // Plan:
      //: 1 Use the 'setLoadLocalTimeOffsetCallback' and
      //:   'currentLoadLocalTimeOffsetCallback' methods to check, change to a
      //:   user-defined callback, and restore initial callback.  Confirm that
      //:   the expected values are returned by each method at each stage.
      //:   (C-1..3)
      //:
      //: 2 Create a user-defined callback that provides an external indication
      //:   when called, and provides user-specified offset and status to its
      //:   caller.  Using an array-drive test, demonstrate that the callback
      //:   works as designed for several unambiguous combinations of status
      //:   and offset values (C-5), and, when the user-defined callback is
      //:   installed as the local time offset callback, demonstrate that the
      //:   'loadLocalTimeOffset' method passes through those same status and
      //:   offset values exactly (C-4.3).
      //:
      //: 3 Install the user-defined callback, and then call the
      //:   'nowAsDatetimeLocal' and 'localTimeOffset' methods, and confirm
      //:   that their results match the values expected given the known values
      //:   coming from the user-defined callback.  (C-4.1, C-4.2)
      //:
      //:   o The test of 'nowAsDatetimeLocal' requires two measurements UTC
      //:     time (one implicit in the method calls the other explicit to
      //:     obtain a value for comparison).  Since time changes between the
      //:     two calls, the results cannot be expected to always compare
      //:     exactly; some positive variance is accepted.
      //:
      //:   o Also, we assume that there is no change in offset (e.g., the
      //:     start of daylight saving time) between the two measures of UTC
      //:     time. 
      //:
      //: 4 Compare the results of the 'loadSystemTimeDefault' method with an
      //:   independent source for local time offset.  (C-6)
      //
      // Testing:
      //  int lLTOC(int *result, const bdet_Datetime&  utcDatetime);
      //  int lLTOCDefault(int *result, const bdet_Datetime&  utcDatetime);
      //  LLTOC setLLTOC(LLTOC callback);
      //  LLTOC currentLLTOC();
      // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "LOCAL TIME OFFSET CALLBACK FACILITY" << endl
                          << "===================================" << endl;

        if (verbose) cout << "\n'currentLoadLocalTimeOffsetCallback' and "
                             "'setLoadLocalTimeOffsetCallback'" << endl;
        {
            if (veryVerbose) { T_() Q(Check for Default Initially) }

            ASSERT(Obj::loadLocalTimeOffsetDefault
                == Obj::currentLoadLocalTimeOffsetCallback());

            if (veryVerbose) { T_() Q(Set User-Defined Callback) }

            struct MyLocalTimeOffsetUtil {

                // CLASS METHODS
                static int loadLocalTimeOffset(
                                           int                  *result,
                                           const bdet_Datetime&  ) {
                    ASSERT(result);
                    *result = 0;
                    return 0;
                }
            };

            Obj::LoadLocalTimeOffsetCallback defaultCallback =
                   Obj::setLoadLocalTimeOffsetCallback(&MyLocalTimeOffsetUtil::
                                                          loadLocalTimeOffset);
            ASSERT(Obj::loadLocalTimeOffsetDefault == defaultCallback);
            ASSERT(&MyLocalTimeOffsetUtil::loadLocalTimeOffset
                == Obj::currentLoadLocalTimeOffsetCallback());
        
            if (veryVerbose) { T_() Q(Restore Default Callback) }

            Obj::LoadLocalTimeOffsetCallback userCallback =
                   Obj::setLoadLocalTimeOffsetCallback(
                                             &Obj::loadLocalTimeOffsetDefault);

            ASSERT(&MyLocalTimeOffsetUtil::loadLocalTimeOffset
                == userCallback);
            ASSERT(Obj::loadLocalTimeOffsetDefault
                == Obj::currentLoadLocalTimeOffsetCallback());
        }

        if (verbose) cout << "\n'MyLocalTimeOffsetUtility' and"
                             "'loadLocalTimeOffset'" << endl;
        {
            const int INPUT[]   = { INT_MIN, -1, 0, 1, INT_MAX};
            const int NUM_INPUT = sizeof(INPUT)/sizeof(*INPUT);

            int resultHelper;
            int resultMethod;

            int  expectedStatus;
            int  expectedOffset;
            bool callbackInvoked;

            MyLocalTimeOffsetUtility::setExternals(&expectedStatus,
                                                   &expectedOffset, 
                                                   &callbackInvoked);

            Obj::LoadLocalTimeOffsetCallback defaultCallback =
                              Obj::setLoadLocalTimeOffsetCallback(
                               &MyLocalTimeOffsetUtility::loadLocalTimeOffset);

            ASSERT(Obj::loadLocalTimeOffsetDefault == defaultCallback);
            ASSERT(&MyLocalTimeOffsetUtility::loadLocalTimeOffset
                == Obj::currentLoadLocalTimeOffsetCallback());

            for (int i = 0; i < NUM_INPUT; ++i) {
                expectedStatus = INPUT[i];

                if (veryVerbose) { P(expectedStatus) }

                for (int j = 0; j < NUM_INPUT; ++j) {
                    expectedOffset  = INPUT[j];

                    if (veryVerbose) { T_() P(expectedOffset) }

                    callbackInvoked = false;
                    ASSERT(!callbackInvoked);
                    int statusHelper = MyLocalTimeOffsetUtility::
                                                          loadLocalTimeOffset(
                                                              &resultHelper,
                                                              bdet_Datetime());
                    ASSERT(callbackInvoked);
                    ASSERT(expectedStatus == statusHelper);
                    ASSERT(expectedOffset == resultHelper);

                    callbackInvoked = false;
                    ASSERT(!callbackInvoked);
                    int statusMethod = Obj::loadLocalTimeOffset(
                                                              &resultMethod,
                                                              bdet_Datetime());
                    ASSERT(callbackInvoked);
                    ASSERT(expectedStatus == statusMethod);
                    ASSERT(expectedOffset == resultMethod);

                    ASSERT(statusHelper == statusMethod);
                    ASSERT(resultHelper == resultMethod);
                }
            }

            // Restore default local time callback

            Obj::setLoadLocalTimeOffsetCallback(defaultCallback);
            ASSERT(&Obj::loadLocalTimeOffsetDefault
                == Obj::currentLoadLocalTimeOffsetCallback());
        }

        if (verbose) cout << "\n'MyLocalTimeOffsetUtility' and"
                             "'loadLocalTimeOffset'" << endl;
        {
            const int OFFSETS[]   = {  -48 * 3600, 0, 72 * 3600 };
            const int NUM_OFFSETS = sizeof(OFFSETS)/sizeof(*OFFSETS);

            int resultHelper;
            int resultMethod;

            const int expectedStatus = 0;
            int       expectedOffset;
            bool      callbackInvoked;

            MyLocalTimeOffsetUtility::setExternals(&expectedStatus,
                                                   &expectedOffset, 
                                                   &callbackInvoked);

            Obj::LoadLocalTimeOffsetCallback defaultCallback =
                              Obj::setLoadLocalTimeOffsetCallback(
                               &MyLocalTimeOffsetUtility::loadLocalTimeOffset);

            ASSERT(Obj::loadLocalTimeOffsetDefault == defaultCallback);
            ASSERT(&MyLocalTimeOffsetUtility::loadLocalTimeOffset
                == Obj::currentLoadLocalTimeOffsetCallback());

            for (int i = 0; i < NUM_OFFSETS; ++i) {
                expectedOffset = OFFSETS[i];

                if (veryVerbose) { P(expectedOffset) }

                bdet_Datetime utcDatetime;
                double        measuredOffset;
                double        delta;

                // test 'nowAsDatetimeLocal'

                callbackInvoked = false;
                bdet_Datetime  lclDatetime = Obj::nowAsDatetimeLocal();
                ASSERT(callbackInvoked);

                // Race condition:  UTC time is advancing, effectively
                // lengthening the offet between local and GMT time.

                utcDatetime    = Obj::nowAsDatetimeUtc();
                measuredOffset = (lclDatetime - utcDatetime).
                                                        totalSecondsAsDouble();
                delta = measuredOffset - static_cast<double>(expectedOffset);

                if (veryVeryVerbose) { T_()
                                       P_(utcDatetime)
                                       P_(lclDatetime)
                                       P_(expectedOffset)
                                       P_(measuredOffset)
                                       P(delta)
                                     }

                LOOP3_ASSERT(lclDatetime, utcDatetime, delta, 0.0 <= delta);
                LOOP3_ASSERT(lclDatetime, utcDatetime, delta, 1.1 >  delta);

                // test 'localTimeOffset'

                callbackInvoked = false;
                bdet_DatetimeInterval reportedOffset = Obj::localTimeOffset();
                ASSERT(callbackInvoked);

                delta = reportedOffset.totalSecondsAsDouble()
                      - static_cast<double>(expectedOffset);

                if (veryVeryVerbose) { T_()
                                       P_(reportedOffset)
                                       P_(expectedOffset)
                                       P(delta)
                                     }

                LOOP3_ASSERT(reportedOffset,
                             expectedOffset,
                             delta,
                             0.0 == delta);

            }
        }

        if (verbose) cout << "\nCompare 'loadLocalTimeOffsetDefault' "
                             "to Alternate Implementation" << endl;
        {
            static const char *INPUT[]   = {  "America/New_York",
                                              "Utc/GMT",
                                              "Europe/Berlin"
                                           };
            static const int NUM_INPUT = sizeof(INPUT)/sizeof(*INPUT);

            static char tzEquals[] = "TZ=";

            for (int i = 0; i < NUM_INPUT; ++i) {
                const char *const TZ = INPUT[i];

                if (verbose) { T_() P(TZ) }

                char buffer[80];
                ASSERT(sizeof(buffer) > sizeof(tzEquals) -1 + strlen(TZ) + 1);
                snprintf(buffer, sizeof(buffer), "%s%s", tzEquals, TZ);
        
                if (veryVeryVerbose) { T_() P(buffer) }

                putenv(buffer);

                ASSERT(0 == strcmp(TZ,
                                   const_cast<const char *>(getenv("TZ"))));
                tzset();

                bdet_Datetime now = bdetu_SystemTime:: nowAsDatetimeUtc();
                int offset;
                bdetu_SystemTime::loadLocalTimeOffsetDefault(&offset, now);
    
                if (veryVeryVerbose) { T_() P_(now) P(offset) }
    
                time_t    currentTime;
                bdetu_Datetime::convertToTimeT(&currentTime, now);
    
                struct tm gmtTM =    *gmtime(&currentTime);
                struct tm lclTM = *localtime(&currentTime);
                time_t    gmt   = mktime(&gmtTM);
                time_t    lcl   = mktime(&lclTM);
                double    diff  = difftime(lcl, gmt);
    
                if (veryVeryVerbose) { T_() P_(currentTime) P(diff) }
    
                ASSERT(static_cast<int>(difftime(lcl, gmt)) == offset);
            }
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

#if 0
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
#endif

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

#if 0
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
#endif
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
#if 0
      case -2: {
        // --------------------------------------------------------------------
        // PERFORMANCE 'localTimeOffset'
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PERFORMANCE 'localTimeOffset'" << endl
                          << "=============================" << endl;


        int count = argc > 2 ? atoi(argv[2]) : 100;

        if (verbose) P(count);

#if   defined(TEST_EMPTY_LOOP)
            Q(TEST_EMPTY_LOOP)
#elif defined(TEST_BASELINE)
            Q(TEST_BASELINE)
#elif defined(TEST_SET_CALLBACK)
            Q(TEST_SET_CALLBACK)
#elif defined(TEST_NEW_GETLOCALTIMEOFFSET)
            Q(TEST_NEW_GETLOCALTIMEOFFSET)
#else
#error "UNKNOWN BUILD FLAG"
#endif

#ifdef TEST_SET_CALLBACK
        Q(USE! alternate callback)

        struct MyUtil {

            static bdet_DatetimeInterval getLocalTimeOffset()
            {
                return bdet_DatetimeInterval();
            }
        };

        bdetu_SystemTime::LocalTimeOffsetCallback oldCallback;
        oldCallback = bdetu_SystemTime::setLocalTimeOffsetCallback(
                                                  &MyUtil::getLocalTimeOffset);
#else
        Q(USE! default callback)
#endif

        bsls::Stopwatch stopwatch;

        stopwatch.start(true);

        for (int i = 0; i < count; ++i) {
#ifdef TEST_EMPTY_LOOP
#else
            bdet_DatetimeInterval dti;
            dti = bdetu_SystemTime::localTimeOffset();
#endif
            //if (veryVerbose) { T_() P_(i) P(dti) }
        }
        stopwatch.stop();

#ifdef TEST_SET_CALLBACK
        Q(USED alternate callback)
        bdetu_SystemTime::setLocalTimeOffsetCallback(oldCallback);
#else
        Q(USED default callback)
#endif

        if (verbose) {
            double systemTime, userTime, wallTime;
            stopwatch.accumulatedTimes(&systemTime,
                                       &userTime,
                                       &wallTime);
            P_(systemTime) P_(userTime) P(wallTime);
        }
      } break;
      case -3: {
        // --------------------------------------------------------------------
        // BUILD EXPERIMENT
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BUILD EXPERIMENT" << endl
                          << "================" << endl;

//#if   defined(BUILD_OPTION1)
#ifdef BUILD_OPTION1
            Q(BUILD_OPTION1)
#endif

      } break;
      case -4: {
        // --------------------------------------------------------------------
        // NEW INTERFACES
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "NEW INTERFACES" << endl
                          << "==============" << endl;

        struct MyTimeoffsetUtil
        {
            static bdet_DatetimeInterval getTimeOffset(bdet_TimeInterval  now,
                                                       void              *arg)
            {
                P_(now) P((const char *)arg)
                return bdet_DatetimeInterval();
            }
        };

        char localTimezone[] = "America/New_York";

        bdetu_SystemTime::GetLocalTimeOffsetCallbackSpec callbackSpec = {
                                               MyTimeoffsetUtil::getTimeOffset,
                                               &localTimezone
                                             };

         bdetu_SystemTime::GetLocalTimeOffsetCallbackSpec *oldCallbackSpec =
                bdetu_SystemTime::setGetLocalTimeOffsetCallback(&callbackSpec);
         ASSERT(0 == oldCallbackSpec);

         bdetu_SystemTime::GetLocalTimeOffsetCallbackSpec *currentCallbackSpec
                   = bdetu_SystemTime::currentGetLocalTimeOffsetCallbackSpec();
         ASSERT(&callbackSpec == currentCallbackSpec);

         bdet_DatetimeInterval offset = bdetu_SystemTime::getLocalTimeOffset();
         ASSERT(bdet_DatetimeInterval() == offset);
       
      } break;
#if defined(TEST_NEW_GETLOCALTIMEOFFSET)
      case -5: {
        // --------------------------------------------------------------------
        // PERFORMANCE 'localTimeOffset(bdet_Datetime utcDatetime)'
        // --------------------------------------------------------------------

        if (verbose) cout
         << endl
         << "PERFORMANCE 'localTimeOffset(bdet_Datetime utcDatetime)'" << endl
         << "========================================================" << endl;

        int count = argc > 2 ? atoi(argv[2]) : 100;

        if (verbose) P(count);

        bsls::Stopwatch stopwatch;

        bdet_Datetime utcDatetime(2013, 2, 13, 18, 30);

        stopwatch.start(true);
        for (int i = 0; i < count; ++i) {
            bdet_DatetimeInterval dti;
            dti = bdetu_SystemTime::getLocalTimeOffset(utcDatetime);
            //if (veryVerbose) { T_() P_(i) P(dti) }
        }
        stopwatch.stop();

        if (verbose) {
            double systemTime, userTime, wallTime;
            stopwatch.accumulatedTimes(&systemTime,
                                       &userTime,
                                       &wallTime);
            P_(systemTime) P_(userTime) P(wallTime);
        }
      } break;
#endif
#endif
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
