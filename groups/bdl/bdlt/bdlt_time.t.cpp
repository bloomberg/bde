// bdlt_time.t.cpp                                                    -*-C++-*-

#include <bdlt_time.h>

#include <bdlt_timeunitratio.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bsls_asserttest.h>
#include <bsls_review.h>

#include <bslx_byteinstream.h>
#include <bslx_byteoutstream.h>
#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_testinstream.h>
#include <bslx_testinstreamexception.h>
#include <bslx_testoutstream.h>
#include <bslx_versionfunctions.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'strcmp'
#include <bsl_c_time.h>
#include <bsl_iostream.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                 TEST PLAN
// ----------------------------------------------------------------------------
//                                 Overview
//                                 --------
// The component under test implements a single, simply constrained
// (value-semantic) attribute class.  The Primary Manipulators and Basic
// Accessors are therefore, respectively, the attribute setters and getters,
// each of which follows our standard unconstrained attribute-type naming
// conventions: 'setAttributeName' and 'attributeName'.
//
// Primary Manipulator:
//: o 'setTime'
//
// Basic Accessors:
//: o 'getTime'
//: o 'hour'
//: o 'minute'
//: o 'second'
//: o 'millisecond'
//
// This particular attribute class also provides a value constructor capable of
// creating an object in any state relevant for thorough testing, obviating the
// primitive generator function, 'gg', normally used for this purpose.  We will
// therefore follow our standard 10-case approach to testing value-semantic
// types except that we will leave case 3 empty.
//-----------------------------------------------------------------------------
//
// CLASS METHODS
// [16] bool isValid(int hour, int minute, int second, int ms, int us);
// [10] static int maxSupportedBdexVersion(int versionSelector);
//
// CREATORS
// [ 2] Time();
// [11] Time(int hour, int minute, int sec = 0, int ms = 0, int us = 0);
// [ 7] Time(const Time& original);
// [ 2] ~Time();
//
// MANIPULATORS
// [ 9] Time& operator=(const Time& rhs);
// [15] Time& operator+=(const DatetimeInterval& rhs);
// [15] Time& operator-=(const DatetimeInterval& rhs);
// [15] int addHours(int hours);
// [15] int addMinutes(int minutes);
// [15] int addSeconds(int seconds);
// [15] int addMilliseconds(int milliseconds);
// [15] int addMicroseconds(int microseconds);
// [15] int addInterval(const DatetimeInterval& interval);
// [15] int addTime(int hours, int minutes, int seconds, int ms, int us);
// [12] void setHour(int hour);
// [12] void setMinute(int minute);
// [12] void setSecond(int second);
// [12] void setMillisecond(int millisecond);
// [12] void setMicrosecond(int microsecond);
// [21] int setHourIfValid(int hour);
// [21] int setMinuteIfValid(int minute);
// [21] int setSecondIfValid(int second);
// [21] int setMillisecondIfValid(int millisecond);
// [21] int setMicrosecondIfValid(int microsecond);
// [ 2] void setTime(int h, int m = 0, int s = 0, int ms = 0, int us = 0);
// [17] int setTimeIfValid(h, m = 0, s = 0, ms = 0, us = 0);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] void getTime(int *h, int *m, int *s, int *ms, int *us) const;
// [ 4] int hour() const;
// [ 4] int minute() const;
// [ 4] int second() const;
// [ 4] int millisecond() const;
// [ 4] int microsecond() const;
// [ 5] int printToBuffer(char *result, int size, int precision) const;
//
// [ 5] ostream& print(ostream& os, int level = 0, int spl = 4) const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
//
// FREE OPERATORS
// [18] void hashAppend(HASHALG&, const Time&);
// [14] DatetimeInterval operator-(const Time& lhs, const Time& rhs);
// [ 6] bool operator==(const Time& lhs, const Time& rhs);
// [ 6] bool operator!=(const Time& lhs, const Time& rhs);
// [13] bool operator< (const Time& lhs, const Time& rhs);
// [13] bool operator<=(const Time& lhs, const Time& rhs);
// [13] bool operator> (const Time& lhs, const Time& rhs);
// [13] bool operator>=(const Time& lhs, const Time& rhs);
// [ 5] ostream& operator<<(ostream& stream, const Time& time);
// [15] Time operator+(const Time&, const DatetimeInterval&);
// [15] Time operator+(const DatetimeInterval&, const Time&);
// [15] Time operator-(const Time&, const DatetimeInterval&);
#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation
// DEPRECATED
// [10] static int maxSupportedBdexVersion();
#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
// [10] static int maxSupportedVersion();
// [17] int validateAndSetTime(h, m = 0, s = 0, ms = 0);
// [ 5] ostream& streamOut(ostream& stream) const;
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] Obj& gg(Obj *object, const char *spec);
// [18] USAGE EXAMPLE
// [ 8] Reserved for 'swap' testing.

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

typedef bdlt::Time          Obj;
typedef bslx::TestInStream  In;
typedef bslx::TestOutStream Out;

#define VERSION_SELECTOR 99991231

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int                 test = argc > 1 ? atoi(argv[1]) : 0;
    const bool             verbose = argc > 2;
    const bool         veryVerbose = argc > 3;
    const bool     veryVeryVerbose = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    // Create an object with an invalid internal representation.

    Obj mInvalid;  const Obj& INVALID = mInvalid;
    {
        memset(reinterpret_cast<char *>(&mInvalid), 0, sizeof(Obj));
    }

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: 'BSLS_REVIEW' failures should lead to test failures.
    bsls::ReviewFailureHandlerGuard reviewGuard(&bsls::Review::failByAbort);

    switch (test) { case 0:
      // --------------------------------------------------------------------
      // VERIFYING HANDLING OF INVALID INTERNAL REPRESENTATIONS
      // --------------------------------------------------------------------
      case 21: {
        // --------------------------------------------------------------------
        // TEST INDIVIDUAL TIME-'set*IfValid' MANIPULATORS
        //
        // Concerns:
        //: 1 Each of the time-only manipulators correctly forwards its
        //:   arguments to the appropriate manipulator of the constituent
        //:   'Time' object.
        //:
        //:   1 When the "time" part has a non-default value, each of the time-
        //:     setting manipulators changes it's intended time field (e.g.,
        //:     hours, milliseconds) and no other (see C-2)
        //:
        //:   2 None of the time-setting manipulators change the "date" part.
        //:
        //: 2 None of these manipulators, alters the "date" part of the object.
        //:
        //: 3 The methods have the same effect regardless of the object's
        //:   initial value.
        //:
        //: 4 These methods have no effect on the object if the supplied
        //:   "time" value is out of the valid range.
        //:
        //: 5 'set*IfValid' returns 0 on success, and a non-zero value on
        //:   failure.
        //
        // Plan:
        //: 1 For a set of independent test values that do not include the
        //:   default 'Time' value (24:00:00.000), use the default constructor
        //:   to create an object and use the time-only "set" manipulators to
        //:   set its value.  Verify the value using the basic accessors after
        //:   each individual "time" field is set.  Repeat the tests for a
        //:   series of objects that span the range of valid 'Datetime' values,
        //:   but excluding the default constructed object (see P-2).
        //:
        //: 2 Create a series of objects having a time "part" equal to 'Time()'
        //:   (24:00:00.000) and confirm using values from the valid bounding
        //:   range of each "time" field that using any of the individual
        //:   time-setting manipulators both sets the specified value (e.g.,
        //:   minute, second) *and* sets the hour field to 0.  Then create an
        //:   object having non-zero values for "time" fields and confirm that
        //:   'setHour(24)' sets that specified value *and* sets all other
        //:   fields to 0.  (C-1..2)
        //:
        //: 3 For each set of values used in testing the seven-argument value
        //:   constructor, create and compare two objects for equality.  One is
        //:   created by the value constructor (proven earlier), the other by
        //:   using the seven-argument 'setDatetime' method of a test object.
        //:   Use a series of test objects that span the range of valid
        //:   'Datetime' values, *including* the default constructed object.
        //:   (C-3)
        //:
        //: 5 Verify that, when an attempt is made to invoke methods with
        //:   arguments that are outside the valid ranges defined in the
        //:   contracts, the object is unchanged, and the return code is
        //:   non-zero.  (C-4..5)
        //
        // Testing:
        //   int setHourIfValid(int hour);
        //   int setMinuteIfValid(int minute);
        //   int setSecondIfValid(int second);
        //   int setMillisecondIfValid(int millisecond);
        //   int setMicrosecondIfValid(int microsecond);
        // --------------------------------------------------------------------

        if (verbose) cout
                  << endl
                  << "TEST INDIVIDUAL TIME-'set*IfValid' MANIPULATORS" << endl
                  << "===============================================" << endl;

        const Obj RT(23, 22, 21, 209);  // Ref time (21:22:21.209)

        Obj ARRAY1[] = {  // default value excluded
                    Obj(  0,  0,  0,   0,   0), // start of epoch
                    RT,                         // arbitrary value
                    Obj( 23, 59, 59, 999, 999)  // end of epoch
                       };
        const int NUM_ARRAY1 =
                              static_cast<int>(sizeof ARRAY1 / sizeof *ARRAY1);

        if (verbose) cout << "\nTesting time-'set*IfValid' methods." << endl;
        if (verbose) cout << "\tFor ordinary computational values." << endl;

        for (int i = 0; i < NUM_ARRAY1; ++i) {
            const Obj OBJ = ARRAY1[i];

            if (veryVerbose) { T_ P(OBJ) }

            static const struct {
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
                int d_usec;
            } VALUES[] = {
                {  0,  0,  0,   0,   0  },
                {  0,  0,  0,   0, 999  },
                {  0,  0,  0, 999,   0  },
                {  0,  0, 59,   0,   0  },
                {  0, 59,  0,   0,   0  },
                { 23,  0,  0,   0,   0  },
                { 23, 22, 21, 209,   0  },  // an ad-hoc value
                { 23, 59, 59, 999, 999  },  // 24:00:00.000 NOT tested here
            };

            const int NUM_VALUES =
                              static_cast<int>(sizeof VALUES / sizeof *VALUES);

            for (int j = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[j].d_hour;
                const int MINUTE = VALUES[j].d_minute;
                const int SECOND = VALUES[j].d_second;
                const int MSEC   = VALUES[j].d_msec;
                const int USEC   = VALUES[j].d_usec;

                Obj x(OBJ);  const Obj& X = x;

                if (veryVerbose) {
                    T_ T_ P_(HOUR) P_(MINUTE) P_(SECOND) P_(MSEC) P_(USEC) P(X)
                }

                ASSERT(0 == x.setHourIfValid(HOUR));
                LOOP2_ASSERT(i, j, HOUR              == X.hour());
                LOOP2_ASSERT(i, j, OBJ.minute()      == X.minute());
                LOOP2_ASSERT(i, j, OBJ.second()      == X.second());
                LOOP2_ASSERT(i, j, OBJ.millisecond() == X.millisecond());
                LOOP2_ASSERT(i, j, OBJ.microsecond() == X.microsecond());

                ASSERT(0 == x.setMinuteIfValid(MINUTE));
                LOOP2_ASSERT(i, j, HOUR              == X.hour());
                LOOP2_ASSERT(i, j, MINUTE            == X.minute());
                LOOP2_ASSERT(i, j, OBJ.second()      == X.second());
                LOOP2_ASSERT(i, j, OBJ.millisecond() == X.millisecond());
                LOOP2_ASSERT(i, j, OBJ.microsecond() == X.microsecond());

                ASSERT(0 == x.setSecondIfValid(SECOND));
                LOOP2_ASSERT(i, j, HOUR              == X.hour());
                LOOP2_ASSERT(i, j, MINUTE            == X.minute());
                LOOP2_ASSERT(i, j, SECOND            == X.second());
                LOOP2_ASSERT(i, j, OBJ.millisecond() == X.millisecond());
                LOOP2_ASSERT(i, j, OBJ.microsecond() == X.microsecond());

                ASSERT(0 == x.setMillisecondIfValid(MSEC));
                LOOP2_ASSERT(i, j, HOUR              == X.hour());
                LOOP2_ASSERT(i, j, MINUTE            == X.minute());
                LOOP2_ASSERT(i, j, SECOND            == X.second());
                LOOP2_ASSERT(i, j, MSEC              == X.millisecond());
                LOOP2_ASSERT(i, j, OBJ.microsecond() == X.microsecond());

                ASSERT(0 == x.setMicrosecondIfValid(USEC));
                LOOP2_ASSERT(i, j, HOUR              == X.hour());
                LOOP2_ASSERT(i, j, MINUTE            == X.minute());
                LOOP2_ASSERT(i, j, SECOND            == X.second());
                LOOP2_ASSERT(i, j, MSEC              == X.millisecond());
                LOOP2_ASSERT(i, j, USEC              == X.microsecond());
            }
        }

        if (verbose) cout << "\tSetting from value 24:00:00.000." << endl;
        {
            const Obj R24;   // Reference object, time = 24:00:00.000
            ASSERT( 24 == R24.hour());
            ASSERT(  0 == R24.minute());
            ASSERT(  0 == R24.second());
            ASSERT(  0 == R24.millisecond());
            ASSERT(  0 == R24.microsecond());

            Obj x;  const Obj& X = x;    if (veryVerbose) { T_  P_(X) }

            x = R24;                     if (veryVerbose) { T_  P_(X) }
            ASSERT(        24 == X.hour());
            ASSERT(0 == x.setMinuteIfValid(0));       if (veryVerbose) P(X);
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(         0 == X.minute());
            ASSERT(         0 == X.second());
            ASSERT(         0 == X.millisecond());
            ASSERT(         0 == X.microsecond());

            x = R24;                     if (veryVerbose) { T_  P_(X) }
            ASSERT(        24 == X.hour());
            ASSERT(0 == x.setMinuteIfValid(59));      if (veryVerbose) P(X);
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(        59 == X.minute());
            ASSERT(         0 == X.second());
            ASSERT(         0 == X.millisecond());
            ASSERT(         0 == X.microsecond());

            x = R24;                     if (veryVerbose) { T_ P_(X) }
            ASSERT(        24 == X.hour());
            ASSERT(0 == x.setSecondIfValid(0));       if (veryVerbose) P(X);
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(         0 == X.minute());
            ASSERT(         0 == X.second());
            ASSERT(         0 == X.millisecond());
            ASSERT(         0 == X.microsecond());

            x = R24;                     if (veryVerbose) { T_ P_(X) }
            ASSERT(        24 == X.hour());
            ASSERT(0 == x.setSecondIfValid(59));      if (veryVerbose) P(X);
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(         0 == X.minute());
            ASSERT(        59 == X.second());
            ASSERT(         0 == X.millisecond());
            ASSERT(         0 == X.microsecond());

            x = R24;                     if (veryVerbose) { T_ P_(X) }
            ASSERT(        24 == X.hour());
            ASSERT(0 == x.setMillisecondIfValid(0));  if (veryVerbose) P(X);
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(         0 == X.minute());
            ASSERT(         0 == X.second());
            ASSERT(         0 == X.millisecond());
            ASSERT(         0 == X.microsecond());

            x = R24;                      if (veryVerbose) { T_ P_(X) }
            ASSERT(        24 == X.hour());
            ASSERT(0 == x.setMillisecondIfValid(999)); if (veryVerbose) P(X);
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(         0 == X.minute());
            ASSERT(         0 == X.second());
            ASSERT(       999 == X.millisecond());
            ASSERT(         0 == X.microsecond());

            x = R24;                      if (veryVerbose) { T_ P_(X) }
            ASSERT(        24 == X.hour());
            ASSERT(0 == x.setMicrosecondIfValid(999)); if (veryVerbose) P(X);
            ASSERT(         0 == X.hour());     // Now 0.
            ASSERT(         0 == X.minute());
            ASSERT(         0 == X.second());
            ASSERT(         0 == X.millisecond());
            ASSERT(       999 == X.microsecond());
        }

        if (verbose) cout << "\tSetting to value 24:00:00.000." << endl;
        {
            Obj mX(RT);  const Obj& X = mX;

            if (veryVerbose) { T_;  P_(X); }
            ASSERT(0 == mX.setHourIfValid(24));
            if (veryVerbose) { P(X); cout << endl; }
            ASSERT(24   == X.hour());
            ASSERT( 0   == X.minute());
            ASSERT( 0   == X.second());
            ASSERT( 0   == X.millisecond());
            ASSERT( 0   == X.microsecond());
        }

        if (verbose) cout << "\nInvalid Testing." << endl;
        {
           if (veryVerbose) cout << "\t'setHourIfValid'" << endl;
            {
                Obj x0; ASSERT(0 != x0.setHourIfValid(-1));
                ASSERT(x0 == Obj());
                Obj x1; ASSERT(0 == x1.setHourIfValid( 0));
                Obj x2; ASSERT(0 == x2.setHourIfValid(23));
                Obj x3; ASSERT(0 == x3.setHourIfValid(24)); // Default object
                Obj x4; ASSERT(0 != x4.setHourIfValid(25));
                ASSERT(x4 == Obj());

                Obj nonDefault(1, 1, 2);  const Obj& nD = nonDefault;

                Obj y0(nD); ASSERT(0 == y0.setHourIfValid(23));
                Obj y1(nD); ASSERT(0 == y1.setHourIfValid(24));
            }

            if (veryVerbose) cout << "\t'setMinuteIfValid'" << endl;
            {
                Obj x0; ASSERT(0 != x0.setMinuteIfValid(-1));
                ASSERT(x0 == Obj());
                Obj x1; ASSERT(0 == x1.setMinuteIfValid( 0));
                Obj x2; ASSERT(0 == x2.setMinuteIfValid(59));
                Obj x4; ASSERT(0 != x4.setMinuteIfValid(60));
                ASSERT(x4 == Obj());
            }

            if (veryVerbose) cout << "\t'setSecondIfValid'" << endl;
            {
                Obj x0; ASSERT(0 != x0.setSecondIfValid(-1));
                ASSERT(x0 == Obj());
                Obj x1; ASSERT(0 == x1.setSecondIfValid( 0));
                Obj x2; ASSERT(0 == x2.setSecondIfValid(59));
                Obj x4; ASSERT(0 != x4.setSecondIfValid(60));
                ASSERT(x4 == Obj());
            }

            if (veryVerbose) cout << "\t'setMillisecondIfValid'" << endl;
            {
                Obj x0; ASSERT(0 != x0.setMillisecondIfValid(  -1));
                ASSERT(x0 == Obj());
                Obj x1; ASSERT(0 == x1.setMillisecondIfValid(   0));
                Obj x2; ASSERT(0 == x2.setMillisecondIfValid( 999));
                Obj x4; ASSERT(0 != x4.setMillisecondIfValid(1000));
                ASSERT(x4 == Obj());
            }

            if (veryVerbose) cout << "\t'setMicrosecondIfValid'" << endl;
            {
                Obj x0; ASSERT(0 != x0.setMicrosecondIfValid(  -1));
                ASSERT(x0 == Obj());
                Obj x1; ASSERT(0 == x1.setMicrosecondIfValid(   0));
                Obj x2; ASSERT(0 == x2.setMicrosecondIfValid( 999));
                Obj x4; ASSERT(0 != x4.setMicrosecondIfValid(1000));
                ASSERT(x4 == Obj());
            }
        }
      } break;
      case 20: {
        bsls::AssertTestHandlerGuard hG;

        // In SAFE build mode, verify all methods that should ASSERT do ASSERT.

        {
            int hour = 0;  (void)hour;

            ASSERT_OPT_FAIL(INVALID.getTime(&hour));
        }
        ASSERT_OPT_FAIL(INVALID.hour());
        ASSERT_OPT_FAIL(INVALID.minute());
        ASSERT_OPT_FAIL(INVALID.second());
        ASSERT_OPT_FAIL(INVALID.millisecond());
        ASSERT_OPT_FAIL(INVALID.microsecond());
        {
            const int PRECISION = 6;

            char buffer[64];  (void)buffer;
            ASSERT_OPT_FAIL(INVALID.printToBuffer(buffer,
                                                   sizeof buffer,
                                                   PRECISION));
            (void)PRECISION;  // quash compiler warning in non-safe build modes
        }

        ASSERT_OPT_FAIL(mInvalid += bdlt::DatetimeInterval());
        ASSERT_OPT_FAIL(mInvalid -= bdlt::DatetimeInterval());
        ASSERT_OPT_FAIL(mInvalid.addHours(0));
        ASSERT_OPT_FAIL(mInvalid.addMinutes(0));
        ASSERT_OPT_FAIL(mInvalid.addSeconds(0));
        ASSERT_OPT_FAIL(mInvalid.addMilliseconds(0));
        ASSERT_OPT_FAIL(mInvalid.addMicroseconds(0));
        ASSERT_OPT_FAIL(mInvalid.addInterval(bdlt::DatetimeInterval(0)));
        ASSERT_OPT_FAIL(mInvalid.addTime(0));
        ASSERT_OPT_FAIL(mInvalid.setHour(0));
        ASSERT_OPT_FAIL(mInvalid.setMinute(0));
        ASSERT_OPT_FAIL(mInvalid.setSecond(0));
        ASSERT_OPT_FAIL(mInvalid.setMillisecond(0));
        ASSERT_OPT_FAIL(mInvalid.setMicrosecond(0));

        ASSERT_OPT_FAIL(mInvalid == Obj());
        ASSERT_OPT_FAIL(mInvalid != Obj());
        ASSERT_OPT_FAIL(mInvalid <  Obj());
        ASSERT_OPT_FAIL(mInvalid <= Obj());
        ASSERT_OPT_FAIL(mInvalid >  Obj());
        ASSERT_OPT_FAIL(mInvalid >= Obj());

        ASSERT_OPT_FAIL(Obj() == mInvalid);
        ASSERT_OPT_FAIL(Obj() != mInvalid);
        ASSERT_OPT_FAIL(Obj() <  mInvalid);
        ASSERT_OPT_FAIL(Obj() <= mInvalid);
        ASSERT_OPT_FAIL(Obj() >  mInvalid);
        ASSERT_OPT_FAIL(Obj() >= mInvalid);
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'bdlt::Time' Usage
///- - - - - - - - - - - - - - - - - -
// This example demonstrates how to create and use a 'bdlt::Time' object.
//
// First, create an object 't1' having the default value, and then verify that
// it represents the value 24:00:00.000000:
//..
    bdlt::Time t1;               ASSERT(24 == t1.hour());
                                 ASSERT( 0 == t1.minute());
                                 ASSERT( 0 == t1.second());
                                 ASSERT( 0 == t1.millisecond());
                                 ASSERT( 0 == t1.microsecond());
//..
// Then, set 't1' to the value 2:34pm (14:34:00.000000):
//..
    t1.setTime(14, 34);          ASSERT(14 == t1.hour());
                                 ASSERT(34 == t1.minute());
                                 ASSERT( 0 == t1.second());
                                 ASSERT( 0 == t1.millisecond());
                                 ASSERT( 0 == t1.microsecond());
//..
// Next, use 'setTimeIfValid' to attempt to assign the invalid value 24:15 to
// 't1', then verify the method returns an error status and the value of 't1'
// is unmodified:
//..
    int ret = t1.setTimeIfValid(24, 15);
                                 ASSERT( 0 != ret);          // 24:15 is not
                                                             // valid

                                 ASSERT(14 == t1.hour());    // no effect
                                 ASSERT(34 == t1.minute());  // on the
                                 ASSERT( 0 == t1.second());  // object
                                 ASSERT( 0 == t1.millisecond());
                                 ASSERT( 0 == t1.microsecond());
//..
// Then, create 't2' as a copy of 't1':
//..
    bdlt::Time t2(t1);            ASSERT(t1 == t2);
//..
// Next, add 5 minutes and 7 seconds to the value of 't2' (in two steps), and
// confirm the value of 't2':
//..
    t2.addMinutes(5);
    t2.addSeconds(7);
                                 ASSERT(14 == t2.hour());
                                 ASSERT(39 == t2.minute());
                                 ASSERT( 7 == t2.second());
                                 ASSERT( 0 == t2.millisecond());
                                 ASSERT( 0 == t2.microsecond());
//..
// Then, subtract 't1' from 't2' to yield a 'bdlt::DatetimeInterval' 'dt'
// representing the time-interval between those two times, and verify the value
// of 'dt' is 5 minutes and 7 seconds (or 307 seconds):
//..
    bdlt::DatetimeInterval dt = t2 - t1;
                                 ASSERT(307 == dt.totalSeconds());
//..
// Finally, stream the value of 't2' to 'stdout':
//..
if (veryVerbose)
    bsl::cout << t2 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  14:39:07.000000
//..
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING: hashAppend
        //
        // Concerns:
        //: 1 Hashes different inputs differently
        //
        //: 2 Hashes equal inputs identically
        //
        //: 3 Works for 'const' and non-'const' times
        //
        // Plan:
        //: 1 Brute force test of a few hand picked values, ensuring that
        //    hashes of equivalent values match and hashes of unequal values do
        //    not.
        //
        // Testing:
        //    void hashAppend(HASHALG&, const Time&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'hashAppend'"
                          << "\n====================\n";

        if (verbose) cout << "Brute force test of several times." << endl;
        {
            typedef ::BloombergLP::bslh::Hash<> Hasher;

            bdlt::Time       t1;
            bdlt::Time       t2( 0,  1,  1,  20);
            bdlt::Time       t3( 0,  1,  1,  20);
            bdlt::Time       t4(15, 11, 30, 500);
            const bdlt::Time t5(15, 11, 30, 500);
            const bdlt::Time t6(15, 11, 30, 501);

            Hasher hasher;
            Hasher::result_type a1 = hasher(t1), a2 = hasher(t2),
                                a3 = hasher(t3), a4 = hasher(t4),
                                a5 = hasher(t5), a6 = hasher(t6);

            if (veryVerbose) {
                cout << "\tHash of " << t1 << " is " << a1 << endl;
                cout << "\tHash of " << t2 << " is " << a2 << endl;
                cout << "\tHash of " << t3 << " is " << a3 << endl;
                cout << "\tHash of " << t4 << " is " << a4 << endl;
                cout << "\tHash of " << t5 << " is " << a5 << endl;
                cout << "\tHash of " << t6 << " is " << a6 << endl;
            }

            ASSERT(a1 != a2);
            ASSERT(a1 != a3);
            ASSERT(a1 != a4);
            ASSERT(a1 != a5);
            ASSERT(a1 != a6);
            if (veryVerbose) {
                cout << "\tt1/d2: " << int(a1 != a2)
                     << ", t1/d3: " << int(a1 != a3)
                     << ", t1/d4: " << int(a1 != a4)
                     << ", t1/d5: " << int(a1 != a5)
                     << ", t1/d6: " << int(a1 != a6) << endl;
            }
            ASSERT(a2 == a3);
            ASSERT(a2 != a4);
            ASSERT(a2 != a5);
            ASSERT(a2 != a6);
            if (veryVerbose) {
                cout << "\tt2/t3: " << int(a2 != a3)
                     << ", t2/t4: " << int(a2 != a4)
                     << ", t2/t5: " << int(a2 != a5)
                     << ", t2/t6: " << int(a2 != a6) << endl;
            }
            ASSERT(a3 != a4);
            ASSERT(a3 != a5);
            ASSERT(a3 != a6);
            if (veryVerbose) {
                cout << "\tt3/t4: " << int(a3 != a4)
                     << ", t3/t5: " << int(a3 != a5)
                     << ", t3/t6: " << int(a3 != a6) << endl;
            }
            ASSERT(a4 == a5);
            ASSERT(a4 != a6);
            if (veryVerbose) {
                cout << "\tt4/t5: " << int(a4 != a5)
                     << ", t4/t6: " << int(a4 != a6) << endl;
            }
            ASSERT(a5 != a6);
            if (veryVerbose) {
                cout << "\tt5/t6: " << int(a5 != a6) << endl;
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'setTimeIfValid'
        //   Verify the method validates the arguments and, if valid, assigns
        //   the expected value.
        //
        // Concerns:
        //: 1 Each of the four integer fields are verified as valid values.
        //:
        //: 2 If the input is not valid, the value is unmodified.
        //:
        //: 3 If the input is valid, the value must be correctly set.
        //:
        //: 4 The special value 'hour == 24' is considered valid and the value
        //:   is correctly assigned.
        //
        // Plan:
        //: 1 Construct a table of valid and invalid inputs and compare results
        //:   to expected "valid" values.  (C-1..4)
        //
        // Testing:
        //   int setTimeIfValid(h, m = 0, s = 0, ms = 0, us = 0);
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
        //   int validateAndSetTime(h, m = 0, s = 0, ms = 0);
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'setTimeIfValid'" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting 'setTimeIfValid'." << endl;
        {
            static const struct {
                int d_lineNum;      // source line number
                int d_hour;         // specification hour
                int d_minute;       // specification minute
                int d_second;       // specification second
                int d_millisecond;  // specification millisecond
                int d_microsecond;  // specification microsecond
                int d_valid;        // expected return value
            } DATA[] = {
                //LINE HOUR   MIN  SEC   MSEC   USEC   VALID
                //---- ----   ---  ---   ----   ----   ------
                { L_,     0,    0,   0,     0,     0,       1 },

                { L_,     0,    0,   0,     0,    -1,       0 },
                { L_,     0,    0,   0,     0,   999,       1 },
                { L_,     0,    0,   0,     0,  1000,       0 },

                { L_,     0,    0,   0,    -1,     0,       0 },
                { L_,     0,    0,   0,   999,     0,       1 },
                { L_,     0,    0,   0,  1000,     0,       0 },

                { L_,     0,    0,  -1,     0,     0,       0 },
                { L_,     0,    0,  59,     0,     0,       1 },
                { L_,     0,    0,  60,     0,     0,       0 },

                { L_,     0,   -1,   0,     0,     0,       0 },
                { L_,     0,   59,   0,     0,     0,       1 },
                { L_,     0,   60,   0,     0,     0,       0 },

                { L_,    -1,    0,   0,     0,     0,       0 },
                { L_,    23,    0,   0,     0,     0,       1 },
                { L_,    24,    0,   0,     0,     0,       1 },
                { L_,    25,    0,   0,     0,     0,       0 },

                { L_,    24,    0,   0,     0,     1,       0 },
                { L_,    24,    0,   0,     1,     0,       0 },
                { L_,    24,    0,   1,     0,     0,       0 },
                { L_,    24,    1,   0,     0,     0,       0 },

                { L_,    23,   59,  59,   999,   999,       1 },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int HOUR         = DATA[i].d_hour;
                const int MINUTE       = DATA[i].d_minute;
                const int SECOND       = DATA[i].d_second;
                const int MILLISECOND  = DATA[i].d_millisecond;
                const int MICROSECOND  = DATA[i].d_microsecond;
                const int VALID        = DATA[i].d_valid;

                if (veryVerbose) { T_; P_(LINE);   P_(VALID);
                                       P_(HOUR);   P_(MINUTE);
                                       P_(SECOND); P_(MILLISECOND);
                                       P(MICROSECOND);
                }
                Obj x;  const Obj& X = x;
                if (1 == VALID) {
                    const Obj R(HOUR,
                                MINUTE,
                                SECOND,
                                MILLISECOND,
                                MICROSECOND);
                    LOOP_ASSERT(LINE,
                                0 == x.setTimeIfValid(HOUR,
                                                      MINUTE,
                                                      SECOND,
                                                      MILLISECOND,
                                                      MICROSECOND));
                    LOOP_ASSERT(LINE, R == X);

                    if (veryVerbose) { P_(VALID);  P_(R);  P(X); }
                }
                else {
                    const Obj R;
                    LOOP_ASSERT(LINE,
                                -1 == x.setTimeIfValid(HOUR,
                                                       MINUTE,
                                                       SECOND,
                                                       MILLISECOND,
                                                       MICROSECOND));
                    LOOP_ASSERT(LINE, R == X);

                    if (veryVerbose) { P_(VALID);  P_(R);  P(X); }
                }
            }
        }

        if (verbose) {
            cout << "\nTesting that 'setTimeIfValid' works with just one "
                 << "argument." << endl;
        }
        {
            Obj x; const Obj& X = x;
            x.setTimeIfValid(22);

            if (veryVerbose) P(X);
            ASSERT(22 == X.hour());
            ASSERT( 0 == X.minute());
            ASSERT( 0 == X.second());
            ASSERT( 0 == X.millisecond());
            ASSERT( 0 == X.microsecond());
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22

        if (verbose) cout << "\nTesting 'validateAndSetTime'." << endl;
        {
            static const struct {
                int d_lineNum;      // source line number
                int d_hour;         // specification hour
                int d_minute;       // specification minute
                int d_second;       // specification second
                int d_millisecond;  // specification millisecond
                int d_valid;        // expected return value
            } DATA[] = {
                //LINE HOUR   MIN  SEC   MSEC   VALID
                //---- ----   ---  ---   ----   ------
                { L_,     0,    0,   0,     0,    1   },

                { L_,     0,    0,   0,    -1,    0   },
                { L_,     0,    0,   0,   999,    1   },
                { L_,     0,    0,   0,  1000,    0   },

                { L_,     0,    0,  -1,     0,    0   },
                { L_,     0,    0,  59,     0,    1   },
                { L_,     0,    0,  60,     0,    0   },

                { L_,     0,   -1,   0,     0,    0   },
                { L_,     0,   59,   0,     0,    1   },
                { L_,     0,   60,   0,     0,    0   },

                { L_,    -1,    0,   0,     0,    0   },
                { L_,    23,    0,   0,     0,    1   },
                { L_,    24,    0,   0,     0,    1   },
                { L_,    25,    0,   0,     0,    0   },

                { L_,    24,    0,   0,     1,    0   },
                { L_,    24,    0,   1,     0,    0   },
                { L_,    24,    1,   0,     0,    0   },

                { L_,    23,   59,  59,   999,    1   },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int HOUR         = DATA[i].d_hour;
                const int MINUTE       = DATA[i].d_minute;
                const int SECOND       = DATA[i].d_second;
                const int MILLISECOND  = DATA[i].d_millisecond;
                const int VALID        = DATA[i].d_valid;

                if (veryVerbose) { T_; P_(LINE);   P_(VALID);
                                       P_(HOUR);   P_(MINUTE);
                                       P_(SECOND); P(MILLISECOND);
                }
                Obj x;  const Obj& X = x;
                if (1 == VALID) {
                    const Obj R(HOUR, MINUTE, SECOND, MILLISECOND);
                    LOOP_ASSERT(LINE,
                                0 == x.validateAndSetTime(HOUR,
                                                          MINUTE,
                                                          SECOND,
                                                          MILLISECOND));
                    LOOP_ASSERT(LINE, R == X);

                    if (veryVerbose) { P_(VALID);  P_(R);  P(X); }
                }
                else {
                    const Obj R;
                    LOOP_ASSERT(LINE,
                                -1 == x.validateAndSetTime(HOUR,
                                                           MINUTE,
                                                           SECOND,
                                                           MILLISECOND));
                    LOOP_ASSERT(LINE, R == X);

                    if (veryVerbose) { P_(VALID);  P_(R);  P(X); }
                }
            }
        }

        if (verbose) {
            cout << "\nTesting that 'validateAndSetTime' works with just one "
                 << "argument." << endl;
        }
        {
            Obj x; const Obj& X = x;
            x.validateAndSetTime(22);

            if (veryVerbose) P(X);
            ASSERT(22 == X.hour());
            ASSERT( 0 == X.minute());
            ASSERT( 0 == X.second());
            ASSERT( 0 == X.millisecond());
            ASSERT( 0 == X.microsecond());
        }

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'isValid'
        //   Verify the method correctly determines if the specified time is
        //   valid.  The special case of 'hour == 24' must also be verified.
        //
        // Concerns:
        //: 1 Each field is tested to ensure the specified time is valid.
        //:
        //: 2 The special value of 'hour == 24' is valid.
        //
        // Plan:
        //: 1 Construct a table of valid and invalid inputs and compare results
        //:   to the expected values.  (C-1,2)
        //
        // Testing:
        //   bool isValid(int hour, int minute, int second, int ms, int us);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'isValid'" << endl
                          << "=================" << endl;

        if (verbose) cout << "\nTesting 'isValid'." << endl;
        {
            static const struct {
                int d_lineNum;      // source line number
                int d_hour;         // specification hour
                int d_minute;       // specification minute
                int d_second;       // specification second
                int d_millisecond;  // specification millisecond
                int d_microsecond;  // specification microsecond
                int d_valid;        // expected return value
            } DATA[] = {
                //LINE HOUR   MIN  SEC   MSEC   USEC   VALID
                //---- ----   ---  ---   ----   ----   ------
                { L_,     0,    0,   0,     0,     0,       1   },

                { L_,     0,    0,   0,     0,    -1,       0   },
                { L_,     0,    0,   0,     0,   999,       1   },
                { L_,     0,    0,   0,     0,  1000,       0   },

                { L_,     0,    0,   0,    -1,     0,       0   },
                { L_,     0,    0,   0,   999,     0,       1   },
                { L_,     0,    0,   0,  1000,     0,       0   },

                { L_,     0,    0,  -1,     0,     0,       0   },
                { L_,     0,    0,  59,     0,     0,       1   },
                { L_,     0,    0,  60,     0,     0,       0   },

                { L_,     0,   -1,   0,     0,     0,       0   },
                { L_,     0,   59,   0,     0,     0,       1   },
                { L_,     0,   60,   0,     0,     0,       0   },

                { L_,    -1,    0,   0,     0,     0,       0   },
                { L_,    23,    0,   0,     0,     0,       1   },
                { L_,    24,    0,   0,     0,     0,       1   },
                { L_,    25,    0,   0,     0,     0,       0   },

                { L_,    24,    0,   0,     0,     1,       0   },
                { L_,    24,    0,   0,     1,     0,       0   },
                { L_,    24,    0,   1,     0,     0,       0   },
                { L_,    24,    1,   0,     0,     0,       0   },

                { L_,    23,   59,  59,   999,   999,       1   },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int HOUR         = DATA[i].d_hour;
                const int MINUTE       = DATA[i].d_minute;
                const int SECOND       = DATA[i].d_second;
                const int MILLISECOND  = DATA[i].d_millisecond;
                const int MICROSECOND  = DATA[i].d_microsecond;
                const int VALID        = DATA[i].d_valid;

                if (veryVerbose) { T_; P_(LINE);   P_(VALID);
                                       P_(HOUR);   P_(MINUTE);
                                       P_(SECOND); P_(MILLISECOND);
                                       P(MICROSECOND);
                }
                LOOP_ASSERT(LINE,
                            VALID == bdlt::Time::isValid(HOUR,
                                                         MINUTE,
                                                         SECOND,
                                                         MILLISECOND,
                                                         MICROSECOND));
            }
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING INTERVAL ADD METHODS
        //   Verify the 'add*' methods, addition operators, and subtraction
        //   operators for intervals work as expected.
        //
        // Concerns:
        //: 1 The numerical constants used to generate the modified object
        //:   value and the return value are correct.
        //:
        //: 2 The correct result is obtained.
        //:
        //: 3 The default value, 24:00:00.000000, is correctly handled.
        //
        // Plan:
        //: 1 Test 'addTime' explicitly using tabulated data.  Specifically,
        //:   specify an arbitrary (but convenient) non-default value as an
        //:   initial value, and also use the default value 24:00:00.000000.
        //:   Specify a set of (h, m, s, ms) tuples to be used as arguments to
        //:   'addTime' and verify that both the modified object value and the
        //:   return value are correct for each of the two initial values.
        //:
        //: 2 Use the tested 'addTime' as an oracle to test the other four
        //:   methods in a loop-based test.  Specifically, specify a (negative)
        //:   start value, a (positive) stop value, and a step size for each of
        //:   the four fields (h, m, s, ms).  Loop over each of these four
        //:   ranges, calling the appropriate 'add' method on the default
        //:   object value and a second non-default object value, and using
        //:   'addTime' as an oracle to verify the resulting object values and
        //:   return values for the 'add' method under test.  (C-1..3)
        //
        // Testing:
        //   int addHours(int hours);
        //   int addMinutes(int minutes);
        //   int addSeconds(int seconds);
        //   int addMilliseconds(int milliseconds);
        //   int addMicroseconds(int microseconds);
        //   int addInterval(const DatetimeInterval& interval);
        //   int addTime(int hours, int minutes, int seconds, int ms, int us);
        //   Time& operator+=(const DatetimeInterval& rhs);
        //   Time& operator-=(const DatetimeInterval& rhs);
        //   Time operator+(const Time&, const DatetimeInterval&);
        //   Time operator+(const DatetimeInterval&, const Time&);
        //   Time operator-(const Time&, const DatetimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING INTERVAL ADD METHODS" << endl
                          << "============================" << endl;

        if (verbose) {
            cout << "\nTesting 'addTime' with the default initial value."
                 << endl;
        }
        {
            static const struct {
                int d_lineNum;       // source line number
                int d_hours;         // hours to add
                int d_minutes;       // minutes to add
                int d_seconds;       // seconds to add
                int d_msecs;         // milliseconds to add
                int d_usecs;         // microseconds to add
                int d_expDays;       // expected return value (days)
                int d_expHour;       // expected hour value
                int d_expMinute;     // expected minute value
                int d_expSecond;     // expected second value
                int d_expMsec;       // expected milliseconds value
                int d_expUsec;       // expected microseconds value
            } DATA[] = {
//     - - - - - - - time added - - - - - - -    - - expected values - -
//LI   H      M       S         MS        US    DAYS  H   M   S   MS   US
//--   --     --      --        ---       ---   ----  --  --  --  ---  ---
{ L_,   0,     0,      0,         0,        0,     0,  0,  0,  0,   0,   0 },

{ L_,   0,     0,      0,         0,        1,     0,  0,  0,  0,   0,   1 },
{ L_,   0,     0,      0,         0,       -1,    -1, 23, 59, 59, 999, 999 },
{ L_,   0,     0,      0,         0,     1000,     0,  0,  0,  0,   1,   0 },
{ L_,   0,     0,      0,         0,    -1000,    -1, 23, 59, 59, 999,   0 },
{ L_,   0,     0,      0,         0,  1000000,     0,  0,  0,  1,   0,   0 },
{ L_,   0,     0,      0,         0, -1000000,    -1, 23, 59, 59,   0,   0 },
{ L_,   0,     0,      0,         0, 60000000,     0,  0,  1,  0,   0,   0 },

{ L_,   0,     0,      0,         1,        0,     0,  0,  0,  0,   1,   0 },
{ L_,   0,     0,      0,        -1,        0,    -1, 23, 59, 59, 999,   0 },
{ L_,   0,     0,      0,      1000,        0,     0,  0,  0,  1,   0,   0 },
{ L_,   0,     0,      0,     60000,        0,     0,  0,  1,  0,   0,   0 },
{ L_,   0,     0,      0,   3600000,        0,     0,  1,  0,  0,   0,   0 },
{ L_,   0,     0,      0,  86400000,        0,     1,  0,  0,  0,   0,   0 },
{ L_,   0,     0,      0, -86400000,        0,    -1,  0,  0,  0,   0,   0 },
{ L_,   0,     0,      0, 864000000,        0,    10,  0,  0,  0,   0,   0 },

{ L_,   0,     0,      1,         0,        0,     0,  0,  0,  1,   0,   0 },
{ L_,   0,     0,     -1,         0,        0,    -1, 23, 59, 59,   0,   0 },
{ L_,   0,     0,     60,         0,        0,     0,  0,  1,  0,   0,   0 },
{ L_,   0,     0,   3600,         0,        0,     0,  1,  0,  0,   0,   0 },
{ L_,   0,     0,  86400,         0,        0,     1,  0,  0,  0,   0,   0 },
{ L_,   0,     0, -86400,         0,        0,    -1,  0,  0,  0,   0,   0 },
{ L_,   0,     0, 864000,         0,        0,    10,  0,  0,  0,   0,   0 },

{ L_,   0,     1,      0,         0,        0,     0,  0,  1,  0,   0,   0 },
{ L_,   0,    -1,      0,         0,        0,    -1, 23, 59,  0,   0,   0 },
{ L_,   0,    60,      0,         0,        0,     0,  1,  0,  0,   0,   0 },
{ L_,   0,  1440,      0,         0,        0,     1,  0,  0,  0,   0,   0 },
{ L_,   0, -1440,      0,         0,        0,    -1,  0,  0,  0,   0,   0 },
{ L_,   0, 14400,      0,         0,        0,    10,  0,  0,  0,   0,   0 },

{ L_,   1,     0,      0,         0,        0,     0,  1,  0,  0,   0,   0 },
{ L_,  -1,     0,      0,         0,        0,    -1, 23,  0,  0,   0,   0 },
{ L_,  24,     0,      0,         0,        0,     1,  0,  0,  0,   0,   0 },
{ L_, -24,     0,      0,         0,        0,    -1,  0,  0,  0,   0,   0 },
{ L_, 240,     0,      0,         0,        0,    10,  0,  0,  0,   0,   0 },

{ L_,  24,  1440,  86400,  86400000,        0,     4,  0,  0,  0,   0,   0 },
{ L_,  24,  1440,  86400, -86400000,        0,     2,  0,  0,  0,   0,   0 },
{ L_,  24,  1440, -86400, -86400000,        0,     0,  0,  0,  0,   0,   0 },
{ L_,  24, -1440, -86400, -86400000,        0,    -2,  0,  0,  0,   0,   0 },
{ L_, -24, -1440, -86400, -86400000,        0,    -4,  0,  0,  0,   0,   0 },
{ L_,  25,  1441,  86401,  86400001,        0,     4,  1,  1,  1,   1,   0 },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE     = DATA[i].d_lineNum;
                const int HOURS    = DATA[i].d_hours;
                const int MINUTES  = DATA[i].d_minutes;
                const int SECONDS  = DATA[i].d_seconds;
                const int MSECS    = DATA[i].d_msecs;
                const int USECS    = DATA[i].d_usecs;

                const int EXP_DAYS = DATA[i].d_expDays;
                const int EXP_HR   = DATA[i].d_expHour;
                const int EXP_MIN  = DATA[i].d_expMinute;
                const int EXP_SEC  = DATA[i].d_expSecond;
                const int EXP_MSEC = DATA[i].d_expMsec;
                const int EXP_USEC = DATA[i].d_expUsec;

                Obj x;  const Obj& X = x;

                if (veryVerbose) { T_;  P_(X); }

                int RETURN_VALUE = x.addTime(HOURS,
                                             MINUTES,
                                             SECONDS,
                                             MSECS,
                                             USECS);

                const Obj EXP(EXP_HR, EXP_MIN, EXP_SEC, EXP_MSEC, EXP_USEC);

                if (veryVerbose) { P_(X);  P_(EXP);  P(RETURN_VALUE); }

                LOOP_ASSERT(LINE, EXP      == X);
                LOOP_ASSERT(LINE, EXP_DAYS == RETURN_VALUE);
            }
        }

        if (verbose) {
            cout << "\nTesting 'addTime' with a non-default initial value."
                 << endl;
        }
        {
            static const struct {
                int d_lineNum;       // source line number
                int d_hours;         // hours to add
                int d_minutes;       // minutes to add
                int d_seconds;       // seconds to add
                int d_msecs;         // milliseconds to add
                int d_usecs;         // microseconds to add
                int d_expDays;       // expected return value (days)
                int d_expHour;       // expected hour value
                int d_expMinute;     // expected minute value
                int d_expSecond;     // expected second value
                int d_expMsec;       // expected milliseconds value
                int d_expUsec;       // expected microseconds value
            } DATA[] = {
//     - - - - - - - time added - - - - - - -    - - expected values - -
//LI   H      M       S         MS        US    DAYS  H   M   S   MS   US
//--   --     --      --        ---       ---   ----  --  --  --  ---  ---
{ L_,   0,     0,      0,         0,        0,     0, 12,  0,  0,   0,   0 },

{ L_,   0,     0,      0,         0,        1,     0, 12,  0,  0,   0,   1 },
{ L_,   0,     0,      0,         0,       -1,     0, 11, 59, 59, 999, 999 },
{ L_,   0,     0,      0,         0,     1000,     0, 12,  0,  0,   1,   0 },
{ L_,   0,     0,      0,         0,    -1000,     0, 11, 59, 59, 999,   0 },
{ L_,   0,     0,      0,         0,  1000000,     0, 12,  0,  1,   0,   0 },
{ L_,   0,     0,      0,         0, -1000000,     0, 11, 59, 59,   0,   0 },
{ L_,   0,     0,      0,         0, 60000000,     0, 12,  1,  0,   0,   0 },

{ L_,   0,     0,      0,         1,        0,     0, 12,  0,  0,   1,   0 },
{ L_,   0,     0,      0,        -1,        0,     0, 11, 59, 59, 999,   0 },
{ L_,   0,     0,      0,      1000,        0,     0, 12,  0,  1,   0,   0 },
{ L_,   0,     0,      0,     60000,        0,     0, 12,  1,  0,   0,   0 },
{ L_,   0,     0,      0,   3600000,        0,     0, 13,  0,  0,   0,   0 },
{ L_,   0,     0,      0,  86400000,        0,     1, 12,  0,  0,   0,   0 },
{ L_,   0,     0,      0, -86400000,        0,    -1, 12,  0,  0,   0,   0 },
{ L_,   0,     0,      0, 864000000,        0,    10, 12,  0,  0,   0,   0 },

{ L_,   0,     0,      1,         0,        0,     0, 12,  0,  1,   0,   0 },
{ L_,   0,     0,     -1,         0,        0,     0, 11, 59, 59,   0,   0 },
{ L_,   0,     0,     60,         0,        0,     0, 12,  1,  0,   0,   0 },
{ L_,   0,     0,   3600,         0,        0,     0, 13,  0,  0,   0,   0 },
{ L_,   0,     0,  86400,         0,        0,     1, 12,  0,  0,   0,   0 },
{ L_,   0,     0, -86400,         0,        0,    -1, 12,  0,  0,   0,   0 },
{ L_,   0,     0, 864000,         0,        0,    10, 12,  0,  0,   0,   0 },

{ L_,   0,     1,      0,         0,        0,     0, 12,  1,  0,   0,   0 },
{ L_,   0,    -1,      0,         0,        0,     0, 11, 59,  0,   0,   0 },
{ L_,   0,    60,      0,         0,        0,     0, 13,  0,  0,   0,   0 },
{ L_,   0,  1440,      0,         0,        0,     1, 12,  0,  0,   0,   0 },
{ L_,   0, -1440,      0,         0,        0,    -1, 12,  0,  0,   0,   0 },
{ L_,   0, 14400,      0,         0,        0,    10, 12,  0,  0,   0,   0 },

{ L_,   1,     0,      0,         0,        0,     0, 13,  0,  0,   0,   0 },
{ L_,  -1,     0,      0,         0,        0,     0, 11,  0,  0,   0,   0 },
{ L_,  24,     0,      0,         0,        0,     1, 12,  0,  0,   0,   0 },
{ L_, -24,     0,      0,         0,        0,    -1, 12,  0,  0,   0,   0 },
{ L_, 240,     0,      0,         0,        0,    10, 12,  0,  0,   0,   0 },

{ L_,  24,  1440,  86400,  86400000,        0,     4, 12,  0,  0,   0,   0 },
{ L_,  24,  1440,  86400, -86400000,        0,     2, 12,  0,  0,   0,   0 },
{ L_,  24,  1440, -86400, -86400000,        0,     0, 12,  0,  0,   0,   0 },
{ L_,  24, -1440, -86400, -86400000,        0,    -2, 12,  0,  0,   0,   0 },
{ L_, -24, -1440, -86400, -86400000,        0,    -4, 12,  0,  0,   0,   0 },
{ L_,  25,  1441,  86401,  86400001,        0,     4, 13,  1,  1,   1,   0 },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            const Obj INITIAL(12, 0, 0, 0);  // arbitrary but convenient value

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE     = DATA[i].d_lineNum;
                const int HOURS    = DATA[i].d_hours;
                const int MINUTES  = DATA[i].d_minutes;
                const int SECONDS  = DATA[i].d_seconds;
                const int MSECS    = DATA[i].d_msecs;
                const int USECS    = DATA[i].d_usecs;

                const int EXP_DAYS = DATA[i].d_expDays;
                const int EXP_HR   = DATA[i].d_expHour;
                const int EXP_MIN  = DATA[i].d_expMinute;
                const int EXP_SEC  = DATA[i].d_expSecond;
                const int EXP_MSEC = DATA[i].d_expMsec;
                const int EXP_USEC = DATA[i].d_expUsec;

                Obj x(INITIAL);  const Obj& X = x;

                if (veryVerbose) { T_;  P_(X); }

                int RETURN_VALUE = x.addTime(HOURS,
                                             MINUTES,
                                             SECONDS,
                                             MSECS,
                                             USECS);

                const Obj EXP(EXP_HR, EXP_MIN, EXP_SEC, EXP_MSEC, EXP_USEC);

                if (veryVerbose) { P_(X);  P_(EXP);  P(RETURN_VALUE); }

                LOOP_ASSERT(LINE, EXP      == X);
                LOOP_ASSERT(LINE, EXP_DAYS == RETURN_VALUE);
            }
        }

        {
            const Obj I1;            // default initial object value
            const Obj I2(12, 0, 0);  // non-default initial object value

            const int START_HOURS = -250;
            const int STOP_HOURS  =  250;
            const int STEP_HOURS  =   25;

            const int START_MINS  = -15000;
            const int STOP_MINS   =  15000;
            const int STEP_MINS   =   1500;

            const int START_SECS  = -900000;
            const int STOP_SECS   =  900000;
            const int STEP_SECS   =   90000;

            const int START_MSECS = -900000000;
            const int STOP_MSECS  =  900000000;
            const int STEP_MSECS  =   90000000;

            const int START_USECS = -900000000;
            const int STOP_USECS  =  900000000;
            const int STEP_USECS  =   90000000;

            if (verbose) cout << "\nTesting 'addHours'." << endl;
            for (int hi = START_HOURS; hi <= STOP_HOURS; hi += STEP_HOURS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                const int RX1 = x1.addHours(hi);
                const int RX2 = x2.addHours(hi);
                const int RY1 = y1.addTime(hi, 0, 0, 0);
                const int RY2 = y2.addTime(hi, 0, 0, 0);

                if (veryVerbose) { T_;  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(hi, Y1 == X1);  LOOP_ASSERT(hi, RY1 == RX1);
                LOOP_ASSERT(hi, Y2 == X2);  LOOP_ASSERT(hi, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting 'addMinutes'." << endl;
            for (int mi = START_MINS; mi <= STOP_MINS; mi += STEP_MINS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) {T_;   P_(X1);  P(X2); }

                const int RX1 = x1.addMinutes(mi);
                const int RX2 = x2.addMinutes(mi);
                const int RY1 = y1.addTime(0, mi, 0, 0);
                const int RY2 = y2.addTime(0, mi, 0, 0);

                if (veryVerbose) { T_;  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(mi, Y1 == X1);  LOOP_ASSERT(mi, RY1 == RX1);
                LOOP_ASSERT(mi, Y2 == X2);  LOOP_ASSERT(mi, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting 'addSeconds'." << endl;
            for (int si = START_SECS; si <= STOP_SECS; si += STEP_SECS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                const int RX1 = x1.addSeconds(si);
                const int RX2 = x2.addSeconds(si);
                const int RY1 = y1.addTime(0, 0, si, 0);
                const int RY2 = y2.addTime(0, 0, si, 0);

                if (veryVerbose) { T_;  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(si, Y1 == X1);  LOOP_ASSERT(si, RY1 == RX1);
                LOOP_ASSERT(si, Y2 == X2);  LOOP_ASSERT(si, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting 'addMilliseconds'." << endl;
            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                const int RX1 = x1.addMilliseconds(msi);
                const int RX2 = x2.addMilliseconds(msi);
                const int RY1 = y1.addTime(0, 0, 0, msi);
                const int RY2 = y2.addTime(0, 0, 0, msi);

                if (veryVerbose) { T_;  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(msi, Y1 == X1);  LOOP_ASSERT(msi, RY1 == RX1);
                LOOP_ASSERT(msi, Y2 == X2);  LOOP_ASSERT(msi, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting 'addMicroseconds'." << endl;
            for (int usi = START_USECS; usi <= STOP_USECS; usi += STEP_USECS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                const int RX1 = x1.addMicroseconds(usi);
                const int RX2 = x2.addMicroseconds(usi);
                const int RY1 = y1.addTime(0, 0, 0, 0, usi);
                const int RY2 = y2.addTime(0, 0, 0, 0, usi);

                if (veryVerbose) { T_;  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(usi, Y1 == X1);  LOOP_ASSERT(usi, RY1 == RX1);
                LOOP_ASSERT(usi, Y2 == X2);  LOOP_ASSERT(usi, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting 'addInterval'." << endl;
            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                const bdlt::DatetimeInterval INTERVAL(0, 0, 0, 0, msi);

                const int RX1 = x1.addInterval(INTERVAL);
                const int RX2 = x2.addInterval(INTERVAL);
                const int RY1 = y1.addTime(0, 0, 0, msi);
                const int RY2 = y2.addTime(0, 0, 0, msi);

                if (veryVerbose) { T_;  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(msi, Y1 == X1);  LOOP_ASSERT(msi, RY1 == RX1);
                LOOP_ASSERT(msi, Y2 == X2);  LOOP_ASSERT(msi, RY2 == RX2);
            }
            for (int usi = START_USECS; usi <= STOP_USECS; usi += STEP_USECS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                const bdlt::DatetimeInterval INTERVAL(0, 0, 0, 0, 0, usi);

                const int RX1 = x1.addInterval(INTERVAL);
                const int RX2 = x2.addInterval(INTERVAL);
                const int RY1 = y1.addTime(0, 0, 0, 0, usi);
                const int RY2 = y2.addTime(0, 0, 0, 0, usi);

                if (veryVerbose) { T_;  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(usi, Y1 == X1);  LOOP_ASSERT(usi, RY1 == RX1);
                LOOP_ASSERT(usi, Y2 == X2);  LOOP_ASSERT(usi, RY2 == RX2);
            }

            if (verbose) {
                cout << "\nTesting 'operator+=' and 'operator-='." << endl;
            }
            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {
                Obj x1(I1);  const Obj &X1 = x1;

                Obj x2(I2);  const Obj &X2 = x2;

                Obj y1(I1);  const Obj &Y1 = y1;

                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                const bdlt::DatetimeInterval INTERVAL(0, 0, 0, 0, msi);

                ASSERT(&x1 == &(x1 += INTERVAL));
                ASSERT(&x2 == &(x2 += INTERVAL));
                y1.addTime(0, 0, 0, msi);
                y2.addTime(0, 0, 0, msi);

                if (veryVerbose) { T_;  P_(X1);  P(X2); }

                LOOP_ASSERT(msi, Y1 == X1);
                LOOP_ASSERT(msi, Y2 == X2);

                ASSERT(&x1 == &(x1 -= INTERVAL));
                ASSERT(&x2 == &(x2 -= INTERVAL));
                y1.addTime(0, 0, 0, -msi);
                y2.addTime(0, 0, 0, -msi);

                LOOP_ASSERT(msi, Y1 == X1);
                LOOP_ASSERT(msi, Y2 == X2);
            }

            if (verbose) {
                cout << "\nTesting 'operator+' and 'operator-'." << endl;
            }
            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {

                const bdlt::DatetimeInterval INTERVAL(0, 0, 0, 0, msi);

                {
                    Obj y1(I1);  const Obj &Y1 = y1;

                    Obj y2(I2);  const Obj &Y2 = y2;

                    Obj x1 = I1 + INTERVAL;  const Obj &X1 = x1;

                    Obj x2 = I2 + INTERVAL;  const Obj &X2 = x2;

                    y1.addTime(0, 0, 0, msi);
                    y2.addTime(0, 0, 0, msi);

                    LOOP_ASSERT(msi, Y1 == X1);
                    LOOP_ASSERT(msi, Y2 == X2);

                    Obj x3 = x1 - INTERVAL;  const Obj &X3 = x3;

                    Obj x4 = x2 - INTERVAL;  const Obj &X4 = x4;

                    y1.addTime(0, 0, 0, -msi);
                    y2.addTime(0, 0, 0, -msi);

                    if (veryVerbose) { P_(X1) P_(Y1) P_(X2) P(Y2); }

                    LOOP_ASSERT(msi, Y1 == X3);
                    LOOP_ASSERT(msi, Y2 == X4);
                }

                {
                    Obj y1(I1);  const Obj &Y1 = y1;

                    Obj y2(I2);  const Obj &Y2 = y2;

                    Obj x1 = INTERVAL + I1;  const Obj &X1 = x1;

                    Obj x2 = INTERVAL + I2;  const Obj &X2 = x2;

                    y1.addTime(0, 0, 0, msi);
                    y2.addTime(0, 0, 0, msi);

                    if (veryVerbose) { P_(X1) P_(Y1) P_(X2) P(Y2); }

                    LOOP_ASSERT(msi, Y1 == X1);
                    LOOP_ASSERT(msi, Y2 == X2);
                }
            }

        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mV;
            Obj mW;
            Obj mX;
            Obj mY;
            Obj mZ(1);

            const bdlt::DatetimeInterval INTERVAL_A(0, 0, 0, 0, 0);
            const bdlt::DatetimeInterval INTERVAL_B(
                                              bsl::numeric_limits<int>::min());
            const bdlt::DatetimeInterval INTERVAL_C(
                                               bsl::numeric_limits<int>::max(),
                                               23,
                                               59,
                                               59,
                                               999);
            const bdlt::DatetimeInterval INTERVAL_D(
                                               bsl::numeric_limits<int>::min(),
                                               0,
                                               0,
                                               0,
                                               -1);
            const bdlt::DatetimeInterval INTERVAL_E(
                                               bsl::numeric_limits<int>::max(),
                                               23);

            ASSERT_PASS(mV.addInterval(INTERVAL_A));
            ASSERT_PASS(mW.addInterval(INTERVAL_B));
            ASSERT_PASS(mX.addInterval(INTERVAL_C));
            ASSERT_FAIL(mY.addInterval(INTERVAL_D));
            ASSERT_FAIL(mZ.addInterval(INTERVAL_E));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING SUBTRACTION OPERATOR
        //   Verify the subtraction operator produces the expected result.
        //
        // Concerns:
        //: 1 Verify this operator performs a subtraction on the underlying
        //:   integer total-milliseconds representation and returns the
        //:   DatetimeInterval initialized with the result.
        //:
        //: 2 The default value, 24:00:00.000000, is correctly handled.
        //
        // Plan:
        //: 1 Specify a set of object value pairs S.  For each (x1, x2) in S,
        //:   verify that both x2 - x1 and x1 - x2 produce the expected
        //:   results.  (C-1,2)
        //
        // Testing:
        //   DatetimeInterval operator-(const Time& lhs, const Time& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING SUBTRACTION OPERATOR" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator-'." << endl;
        {
            static const struct {
                int                d_lineNum;   // source line number
                int                d_hours2;    // lhs time hours
                int                d_minutes2;  // lhs time minutes
                int                d_seconds2;  // lhs time seconds
                int                d_msecs2;    // lhs time milliseconds
                int                d_usecs2;    // lhs time microseconds
                int                d_hours1;    // rhs time hours
                int                d_minutes1;  // rhs time minutes
                int                d_seconds1;  // rhs time seconds
                int                d_msecs1;    // rhs time milliseconds
                int                d_usecs1;    // rhs time microseconds
                bsls::Types::Int64 d_expUsec;   // expected difference (usec)
            } DATA[] = {
            //    - - - lhs time - - -  - - - rhs time - - -
            //LN  H   M   S   MS   US   H   M   S   MS   US     EXP. USEC
            //--  --  --  --  ---  ---  --  --  --  ---  ---  -------------
            { L_,  0,  0,  0,   0,   0,  0,  0,  0,   0,   0,           0LL },
            { L_, 24,  0,  0,   0,   0,  0,  0,  0,   0,   0,           0LL },
            { L_,  0,  0,  0,   0,   0, 24,  0,  0,   0,   0,           0LL },
            { L_, 24,  0,  0,   0,   0, 24,  0,  0,   0,   0,           0LL },

            { L_,  0,  0,  0,   0,   1,  0,  0,  0,   0,   0,           1LL },
            { L_,  0,  0,  0,   0,   0,  0,  0,  0,   0,   1,          -1LL },
            { L_,  0,  0,  0,   0,   1, 24,  0,  0,   0,   0,           1LL },
            { L_, 24,  0,  0,   0,   0,  0,  0,  0,   0,   1,          -1LL },

            { L_,  0,  0,  0,   1,   0,  0,  0,  0,   0,   0,        1000LL },
            { L_,  0,  0,  0,   0,   0,  0,  0,  0,   1,   0,       -1000LL },
            { L_,  0,  0,  0,   1,   0, 24,  0,  0,   0,   0,        1000LL },
            { L_, 24,  0,  0,   0,   0,  0,  0,  0,   1,   0,       -1000LL },

            { L_,  0,  0,  1,   0,   0,  0,  0,  0,   0,   0,     1000000LL },
            { L_,  0,  0,  0,   0,   0,  0,  0,  1,   0,   0,    -1000000LL },
            { L_,  0,  0,  1,   0,   0, 24,  0,  0,   0,   0,     1000000LL },
            { L_, 24,  0,  0,   0,   0,  0,  0,  1,   0,   0,    -1000000LL },

            { L_,  0,  1,  0,   0,   0,  0,  0,  0,   0,   0,    60000000LL },
            { L_,  0,  0,  0,   0,   0,  0,  1,  0,   0,   0,   -60000000LL },
            { L_,  0,  1,  0,   0,   0, 24,  0,  0,   0,   0,    60000000LL },
            { L_, 24,  0,  0,   0,   0,  0,  1,  0,   0,   0,   -60000000LL },

            { L_,  1,  0,  0,   0,   0,  0,  0,  0,   0,   0,  3600000000LL },
            { L_,  0,  0,  0,   0,   0,  1,  0,  0,   0,   0, -3600000000LL },
            { L_,  1,  0,  0,   0,   0, 24,  0,  0,   0,   0,  3600000000LL },
            { L_, 24,  0,  0,   0,   0,  1,  0,  0,   0,   0, -3600000000LL },
            };

            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int                LINE     = DATA[i].d_lineNum;
                const int                HOURS2   = DATA[i].d_hours2;
                const int                MINUTES2 = DATA[i].d_minutes2;
                const int                SECONDS2 = DATA[i].d_seconds2;
                const int                MSECS2   = DATA[i].d_msecs2;
                const int                USECS2   = DATA[i].d_usecs2;
                const int                HOURS1   = DATA[i].d_hours1;
                const int                MINUTES1 = DATA[i].d_minutes1;
                const int                SECONDS1 = DATA[i].d_seconds1;
                const int                MSECS1   = DATA[i].d_msecs1;
                const int                USECS1   = DATA[i].d_usecs1;
                const bsls::Types::Int64 EXP_USEC = DATA[i].d_expUsec;

                const Obj X2(HOURS2, MINUTES2, SECONDS2, MSECS2, USECS2);
                const Obj X1(HOURS1, MINUTES1, SECONDS1, MSECS1, USECS1);

                const bdlt::DatetimeInterval INTERVAL1(X2 - X1);
                const bdlt::DatetimeInterval INTERVAL2(X1 - X2);

                if (veryVerbose) {
                    T_;  P_(X2);  P_(X1);  P(INTERVAL1.totalMicroseconds());
                }

                LOOP_ASSERT(LINE,  EXP_USEC == INTERVAL1.totalMicroseconds());
                LOOP_ASSERT(LINE, -EXP_USEC == INTERVAL2.totalMicroseconds());
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING RELATIONAL OPERATORS
        //   Verify the relational operators evaluate correctly.
        //
        // Concerns:
        //: 1 Each operator invokes the corresponding operator on the
        //:   underlying integer total milliseconds correctly.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify an ordered set 'S' of unique object values.  For each
        //:   '(u, v)' in the set 'S x S', verify the result of 'u OP v' for
        //:   each 'OP' in '{<, <=, >=, >}'.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   bool operator< (const Time& lhs, const Time& rhs);
        //   bool operator<=(const Time& lhs, const Time& rhs);
        //   bool operator>=(const Time& lhs, const Time& rhs);
        //   bool operator> (const Time& lhs, const Time& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING RELATIONAL OPERATORS" << endl
                          << "============================" << endl;

        if (verbose) {
            cout << "\nTesting 'operator<', 'operator<=', 'operator>=', "
                 << "and 'operator>'." << endl;
        }
        {
            static const struct {
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
                int d_usec;
            } VALUES[] = {
                {  0,  0,  0,   0,   0 },
                {  0,  0,  0,   0,   1 },
                {  0,  0,  0,   0, 999 },
                {  0,  0,  0,   1,   0 },
                {  0,  0,  0, 999,   0 },
                {  0,  0,  1,   0,   0 },
                {  0,  0, 59, 999,   0 },
                {  0,  1,  0,   0,   0 },
                {  0,  1,  0,   1,   0 },
                {  0, 59, 59, 999,   0 },
                {  1,  0,  0,   0,   0 },
                {  1,  0,  0,   1,   0 },
                { 23,  0,  0,   0,   0 },
                { 23, 22, 21, 209,   0 },
                { 23, 22, 21, 210,   0 },
                { 23, 22, 21, 210,   1 },
                { 23, 22, 21, 210, 102 },
                { 23, 22, 21, 211,  17 },
                { 23, 59, 59, 999, 999 },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj        v(VALUES[i].d_hour,
                             VALUES[i].d_minute,
                             VALUES[i].d_second,
                             VALUES[i].d_msec,
                             VALUES[i].d_usec);
                const Obj& V = v;

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj        u(VALUES[j].d_hour,
                                 VALUES[j].d_minute,
                                 VALUES[j].d_second,
                                 VALUES[j].d_msec,
                                 VALUES[j].d_usec);
                    const Obj& U = u;
                    if (veryVerbose) { T_;  P_(i);  P_(j);  P_(V);  P(U); }
                    LOOP2_ASSERT(i, j, (j <  i) == (U <  V));
                    LOOP2_ASSERT(i, j, (j <= i) == (U <= V));
                    LOOP2_ASSERT(i, j, (j >= i) == (U >= V));
                    LOOP2_ASSERT(i, j, (j >  i) == (U >  V));
                }
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX(1, 0, 0, 0);  const Obj X = mX;  (void)X;

            Obj mY(1, 0, 0, 0);  const Obj Y = mY;  (void)Y;

            Obj mZ;              const Obj Z = mZ;  (void)Z;

            ASSERT_SAFE_PASS(X <  Y);
            ASSERT_SAFE_FAIL(X <  Z);
            ASSERT_SAFE_FAIL(Z <  X);

            ASSERT_SAFE_PASS(X <= Y);
            ASSERT_SAFE_FAIL(X <= Z);
            ASSERT_SAFE_FAIL(Z <= X);

            ASSERT_SAFE_PASS(X >= Y);
            ASSERT_SAFE_FAIL(X >= Z);
            ASSERT_SAFE_FAIL(Z >= X);

            ASSERT_SAFE_PASS(X >  Y);
            ASSERT_SAFE_FAIL(X >  Z);
            ASSERT_SAFE_FAIL(Z >  X);
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL 'set' MANIPULATORS
        //   Verify the 'set*' methods work as expected.
        //
        // Concerns:
        //: 1 The numerical constants used to generate the modified object
        //:   value are correct.
        //:
        //: 2 The correct object value is obtained.
        //:
        //: 3 When the initial value is the default value, the methods work as
        //:   expected.
        //:
        //: 4 The special case of 'setHour(24)' sets the object to the default
        //:   value.
        //:
        //: 5 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For a set of independent test values not including the
        //:   default value (24:00:00.000000), use the default constructor to
        //:   create an object and use the 'set' manipulators to set its value.
        //:   Verify the value using the basic accessors.  (C-1,2)
        //:
        //: 2 Confirm the correct behavior of the 'set' methods when
        //:   setting from the value 24:00:00.000000.  (C-3)
        //:
        //: 3 Verify 'setHour(24)' results in the default value for the object.
        //:   (C-4)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-5)
        //
        // Testing:
        //   void setHour(int hour);
        //   void setMinute(int minute);
        //   void setSecond(int second);
        //   void setMillisecond(int millisecond);
        //   void setMillisecond(int microsecond);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ADDITIONAL 'set' MANIPULATORS" << endl
                          << "=====================================" << endl;

        if (verbose) cout << "\nTesting 'setXXX' methods." << endl;

        if (verbose) cout << "\tFor ordinary computational values." << endl;
        {
            static const struct {
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
                int d_usec;
            } VALUES[] = {
                {  0,  0,  0,   0,   0 },
                {  0,  0,  0,   0, 999 },
                {  0,  0,  0, 999,   0 },
                {  0,  0, 59,   0,   0 },
                {  0, 59,  0,   0,   0 },
                { 23,  0,  0,   0,   0 },
                { 23, 22, 21, 209,   0 },
                { 23, 22, 21, 210,   0 },
                { 23, 22, 21, 210, 117 },
                // 24:00:00.000000 NOT tested here
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;
                const int USEC   = VALUES[i].d_usec;

                Obj x;  const Obj& X = x;
                x.setHour(HOUR);
                x.setMinute(MINUTE);
                x.setSecond(SECOND);
                x.setMillisecond(MSEC);
                x.setMicrosecond(USEC);
                if (veryVerbose) {
                    T_;
                    P_(HOUR); P_(MINUTE); P_(SECOND); P_(MSEC); P_(USEC);
                    P(X);
                }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());
                LOOP_ASSERT(i, USEC   == X.microsecond());
            }
            if (veryVerbose) cout << endl;
        }

        if (verbose) cout << "\tSetting from value 24:00:00.000000." << endl;
        {
            const Obj R24;             // Reference object (24:00:00.000000)

            Obj x;  const Obj& X = x;  if (veryVerbose) { T_;  P_(X); }
            x.setMinute(0);            if (veryVerbose) P(X);
            ASSERT(  0 == x.hour());
            ASSERT(  0 == x.minute());
            ASSERT(  0 == x.second());
            ASSERT(  0 == x.millisecond());
            ASSERT(  0 == x.microsecond());

            x = R24;                   if (veryVerbose) { T_;  P_(X); }
            x.setMinute(59);           if (veryVerbose) P(X);
            ASSERT(  0 == x.hour());
            ASSERT( 59 == x.minute());
            ASSERT(  0 == x.second());
            ASSERT(  0 == x.millisecond());
            ASSERT(  0 == x.microsecond());

            x = R24;                   if (veryVerbose) { T_;  P_(X); }
            x.setSecond(0);            if (veryVerbose) P(X);
            ASSERT(  0 == x.hour());
            ASSERT(  0 == x.minute());
            ASSERT(  0 == x.second());
            ASSERT(  0 == x.millisecond());
            ASSERT(  0 == x.microsecond());

            x = R24;                   if (veryVerbose) { T_;  P_(X); }
            x.setSecond(59);           if (veryVerbose) P(X);
            ASSERT(  0 == x.hour());
            ASSERT(  0 == x.minute());
            ASSERT( 59 == x.second());
            ASSERT(  0 == x.millisecond());
            ASSERT(  0 == x.microsecond());

            x = R24;                   if (veryVerbose) { T_;  P_(X); }
            x.setMillisecond(0);       if (veryVerbose) P(X);
            ASSERT(  0 == x.hour());
            ASSERT(  0 == x.minute());
            ASSERT(  0 == x.second());
            ASSERT(  0 == x.millisecond());
            ASSERT(  0 == x.microsecond());

            x = R24;                   if (veryVerbose) { T_;  P_(X); }
            x.setMillisecond(999);     if (veryVerbose) { P(X); cout << endl; }
            ASSERT(  0 == x.hour());
            ASSERT(  0 == x.minute());
            ASSERT(  0 == x.second());
            ASSERT(999 == x.millisecond());
            ASSERT(  0 == x.microsecond());

            x = R24;                   if (veryVerbose) { T_;  P_(X); }
            x.setMicrosecond(0);       if (veryVerbose) P(X);
            ASSERT(  0 == x.hour());
            ASSERT(  0 == x.minute());
            ASSERT(  0 == x.second());
            ASSERT(  0 == x.millisecond());
            ASSERT(  0 == x.microsecond());

            x = R24;                   if (veryVerbose) { T_;  P_(X); }
            x.setMicrosecond(999);     if (veryVerbose) { P(X); cout << endl; }
            ASSERT(  0 == x.hour());
            ASSERT(  0 == x.minute());
            ASSERT(  0 == x.second());
            ASSERT(  0 == x.millisecond());
            ASSERT(999 == x.microsecond());
        }

        if (verbose) cout << "\tSetting to value 24:00:00.000000." << endl;
        {
            const Obj R(23, 22, 21, 209); // Reference object (21:22:21.209)

            Obj x(R); const Obj& X = x;if (veryVerbose) { T_;  P_(X); }
            x.setHour(24);             if (veryVerbose) { P(X); cout << endl; }
            ASSERT(24 == x.hour());
            ASSERT( 0 == x.minute());
            ASSERT( 0 == x.second());
            ASSERT( 0 == x.millisecond());
            ASSERT( 0 == x.microsecond());

        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;

            ASSERT_FAIL(mX.setHour(-1));
            ASSERT_SAFE_PASS(mX.setHour( 0));
            ASSERT_SAFE_PASS(mX.setHour(24));
            ASSERT_FAIL(mX.setHour(25));

            ASSERT_FAIL(mX.setMinute(-1));
            ASSERT_SAFE_PASS(mX.setMinute( 0));
            ASSERT_SAFE_PASS(mX.setMinute(59));
            ASSERT_FAIL(mX.setMinute(60));

            ASSERT_FAIL(mX.setSecond(-1));
            ASSERT_SAFE_PASS(mX.setSecond( 0));
            ASSERT_SAFE_PASS(mX.setSecond(59));
            ASSERT_FAIL(mX.setSecond(60));

            ASSERT_FAIL(mX.setMillisecond(  -1));
            ASSERT_SAFE_PASS(mX.setMillisecond(   0));
            ASSERT_SAFE_PASS(mX.setMillisecond( 999));
            ASSERT_FAIL(mX.setMillisecond(1000));

            ASSERT_FAIL(mX.setMicrosecond(  -1));
            ASSERT_SAFE_PASS(mX.setMicrosecond(   0));
            ASSERT_SAFE_PASS(mX.setMicrosecond( 999));
            ASSERT_FAIL(mX.setMicrosecond(1000));
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZING CONSTRUCTOR
        //   Verify the initializing constructor works as expected.
        //
        // Concerns:
        //: 1 The two required parameters and two optional parameters are
        //:   multiplied by the appropriate factors when initializing the
        //:   internal total-milliseconds integer representation.
        //:
        //: 2 The default value 24:00:00.000000 must be constructible
        //:   explicitly.
        //:
        //: 3 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Specify a set 'S' of times as '(h, m, s, ms, us)' tuples having
        //:   widely varying values.  For each '(h, m, s, ms, us)' in 'S',
        //:   construct an object 'X' using all four arguments and an object
        //:   'Y' using the first three arguments, and verify that 'X' and 'Y'
        //:   have the expected values.  (C-1)
        //:
        //: 2 Test explicitly that the initializing constructor can create an
        //:   object having the default value.  (C-2)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   Time(int hour, int minute, int sec = 0, int ms = 0, int us = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING INITIALIZING CONSTRUCTOR" << endl
                          << "================================" << endl;

        if (verbose) cout << "\nFor ordinary computational values." << endl;
        {
            static const struct {
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
                int d_usec;
            } VALUES[] = {
                {  0,  0,  0,   0,   0 },
                {  0,  0,  0,   0, 999 },
                {  0,  0,  0, 999,   0 },
                {  0,  0, 59,   0,   0 },
                {  0, 59,  0,   0,   0 },
                { 23,  0,  0,   0,   0 },
                { 23, 22, 21, 209,   0 },
                { 23, 22, 21, 210,   0 },
                { 23, 22, 21, 210, 217 },
                { 23, 59, 59, 999, 999 },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;
                const int USEC   = VALUES[i].d_usec;

                Obj        w(HOUR, MINUTE, SECOND, MSEC, USEC);
                const Obj& W = w;

                Obj x(HOUR, MINUTE, SECOND, MSEC);  const Obj& X = x;

                Obj y(HOUR, MINUTE, SECOND);        const Obj& Y = y;

                Obj z(HOUR, MINUTE);                const Obj& Z = z;

                if (veryVerbose) {
                    T_; P_(HOUR); P_(MINUTE); P_(SECOND); P_(MSEC);
                    P_(W);  P_(X);
                    P_(Y);  P(Z);
                }
                LOOP_ASSERT(i, HOUR   == W.hour());
                LOOP_ASSERT(i, MINUTE == W.minute());
                LOOP_ASSERT(i, SECOND == W.second());
                LOOP_ASSERT(i, MSEC   == W.millisecond());
                LOOP_ASSERT(i, USEC   == W.microsecond());

                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());
                LOOP_ASSERT(i, 0      == X.microsecond());

                LOOP_ASSERT(i, HOUR   == Y.hour());
                LOOP_ASSERT(i, MINUTE == Y.minute());
                LOOP_ASSERT(i, SECOND == Y.second());
                LOOP_ASSERT(i, 0      == Y.millisecond());
                LOOP_ASSERT(i, 0      == Y.microsecond());

                LOOP_ASSERT(i, HOUR   == Z.hour());
                LOOP_ASSERT(i, MINUTE == Z.minute());
                LOOP_ASSERT(i, 0      == Z.second());
                LOOP_ASSERT(i, 0      == Z.millisecond());
                LOOP_ASSERT(i, 0      == Z.microsecond());
            }
        }

        if (verbose) cout << "\nFor the default values." << endl;
        {
            Obj d;                  const Obj& D = d;

            Obj w(24, 0, 0, 0, 0);  const Obj& W = w;

            Obj x(24, 0, 0, 0);     const Obj& X = x;

            Obj y(24, 0, 0);        const Obj& Y = y;

            Obj z(24, 0);           const Obj& Z = z;

            if (veryVerbose) { T_;  P_(D);  P_(W);  P_(X);  P_(Y)  P(Z); }

            ASSERT(D == W);
            ASSERT(D == X);
            ASSERT(D == Y);
            ASSERT(D == Z);
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            ASSERT_SAFE_PASS(Obj mX(24, 0, 0, 0, 0));
            ASSERT_SAFE_FAIL(Obj mX(24, 0, 0, 0, 1));
            ASSERT_SAFE_FAIL(Obj mX(24, 0, 0, 1, 0));
            ASSERT_SAFE_FAIL(Obj mX(24, 0, 1, 0, 0));
            ASSERT_SAFE_FAIL(Obj mX(24, 1, 0, 0, 0));

            ASSERT_SAFE_FAIL(Obj mX(-1, 0, 0, 0));
            ASSERT_SAFE_PASS(Obj mX( 0, 0, 0, 0));
            ASSERT_SAFE_PASS(Obj mX(23, 0, 0, 0));
            ASSERT_SAFE_FAIL(Obj mX(25, 0, 0, 0));

            ASSERT_SAFE_FAIL(Obj mX(0, -1, 0, 0));
            ASSERT_SAFE_PASS(Obj mX(0,  0, 0, 0));
            ASSERT_SAFE_PASS(Obj mX(0, 59, 0, 0));
            ASSERT_SAFE_FAIL(Obj mX(0, 60, 0, 0));

            ASSERT_SAFE_FAIL(Obj mX(0, 0, -1, 0));
            ASSERT_SAFE_PASS(Obj mX(0, 0,  0, 0));
            ASSERT_SAFE_PASS(Obj mX(0, 0, 59, 0));
            ASSERT_SAFE_FAIL(Obj mX(0, 0, 60, 0));

            ASSERT_SAFE_FAIL(Obj mX(0, 0, 0,   -1));
            ASSERT_SAFE_PASS(Obj mX(0, 0, 0,    0));
            ASSERT_SAFE_PASS(Obj mX(0, 0, 0,  999));
            ASSERT_SAFE_FAIL(Obj mX(0, 0, 0, 1000));

            ASSERT_SAFE_FAIL(Obj mX(0, 0, 0, 0,   -1));
            ASSERT_SAFE_PASS(Obj mX(0, 0, 0, 0,    0));
            ASSERT_SAFE_PASS(Obj mX(0, 0, 0, 0,  999));
            ASSERT_SAFE_FAIL(Obj mX(0, 0, 0, 0, 1000));
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING BDEX STREAMING
        //   Verify the BDEX streaming implementation works correctly.
        //   Specific concerns include wire format, handling of stream states
        //   (valid, empty, invalid, incomplete, and corrupted), and exception
        //   neutrality.
        //
        // Concerns:
        //: 1 The class method 'maxSupportedBdexVersion' returns the correct
        //:   version to be used for the specified 'versionSelector'.
        //:
        //: 2 The 'bdexStreamOut' method is callable on a reference providing
        //:   only non-modifiable access.
        //:
        //: 3 For valid streams, externalization and unexternalization are
        //:   inverse operations.
        //:
        //: 4 For invalid streams, externalization leaves the stream invalid
        //:   and unexternalization does not alter the value of the object and
        //:   leaves the stream invalid.
        //:
        //: 5 Unexternalizing of incomplete, invalid, or corrupted data results
        //:   in a valid object of unspecified value and an invalidated stream.
        //:
        //: 6 The wire format of the object is as expected.
        //:
        //: 7 All methods are exception neutral.
        //:
        //: 8 The 'bdexStreamIn' and 'bdexStreamOut' methods return a reference
        //:   to the provided stream in all situations.
        //:
        //: 9 The initial value of the object has no affect on
        //:   unexternalization.
        //
        // Plan:
        //: 1 Test 'maxSupportedBdexVersion' explicitly.  (C-1)
        //:
        //: 2 All calls to the 'bdexStreamOut' accessor will be done from a
        //:   'const' object or reference and all calls to the 'bdexStreamOut'
        //:   free function (provided by 'bslx') will be supplied a 'const'
        //:   object or reference.  (C-2)
        //:
        //: 3 Perform a direct test of the 'bdexStreamOut' and 'bdexStreamIn'
        //:   methods (the rest of the testing will use the free functions
        //:   'bslx::OutStreamFunctions::bdexStreamOut' and
        //:   'bslx::InStreamFunctions::bdexStreamIn').
        //:
        //: 4 Define a set 'S' of test values to be used throughout the test
        //:   case.
        //:
        //: 5 For all '(u, v)' in the cross product 'S X S', stream the value
        //:   of 'u' into (a temporary copy of) 'v', 'T', and assert 'T == u'.
        //:   (C-3, 9)
        //:
        //: 6 For all 'u' in 'S', create a copy of 'u' and attempt to stream
        //:   into it from an invalid stream.  Verify after each attempt that
        //:   the object is unchanged and that the stream is invalid.  (C-4)
        //:
        //: 7 Write 3 distinct objects to an output stream buffer of total
        //:   length 'N'.  For each partial stream length from 0 to 'N - 1',
        //:   construct an input stream and attempt to read into objects
        //:   initialized with distinct values.  Verify values of objects
        //:   that are either successfully modified or left entirely
        //:   unmodified, and that the stream became invalid immediately after
        //:   the first incomplete read.  Finally, ensure that each object
        //:   streamed into is in some valid state.
        //:
        //: 8 Use the underlying stream package to simulate a typical valid
        //:   (control) stream and verify that it can be streamed in
        //:   successfully.  Then for each data field in the stream (beginning
        //:   with the version number), provide one or more similar tests with
        //:   that data field corrupted.  After each test, verify that the
        //:   object is in some valid state after streaming, and that the
        //:   input stream has become invalid.  (C-5)
        //:
        //: 9 Explicitly test the wire format.  (C-6)
        //:
        //:10 In all cases, confirm exception neutrality using the specially
        //:   instrumented 'bslx::TestInStream' and a pair of standard macros,
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN' and
        //:   'BSLX_TESTINSTREAM_EXCEPTION_TEST_END', which configure the
        //:   'bslx::TestInStream' object appropriately in a loop.  (C-7)
        //:
        //:11 In all cases, verify the return value of the tested method.
        //:   (C-8)
        //
        // Testing:
        //   static int maxSupportedBdexVersion(int versionSelector);
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation
        //   static int maxSupportedBdexVersion();
#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
        //   static int maxSupportedVersion();
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
        // --------------------------------------------------------------------

        // Allocator to use instead of the default allocator.
        bslma::TestAllocator allocator("bslx", veryVeryVeryVerbose);

        if (verbose) cout << endl
                          << "TESTING BDEX STREAMING" << endl
                          << "======================" << endl;

        // Scalar object values used in various stream tests.
        const Obj VA(  0,  0,  0,   0,   0);
        const Obj VB(  0,  0,  0,   0, 999);
        const Obj VC(  0,  0,  0, 999,   0);
        const Obj VD(  0,  0, 59,   0,   0);
        const Obj VE(  0, 59,  0,   0,   0);
        const Obj VF( 23,  0,  0,   0,   0);
        const Obj VG( 23, 22, 21, 209, 212);

        // Array object used in various stream tests.
        const Obj VALUES[]   = { VA, VB, VC, VD, VE, VF, VG };
        const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                / sizeof *VALUES);

        if (verbose) {
            cout << "\nTesting 'maxSupportedBdexVersion'." << endl;
        }
        {
            ASSERT(1 == Obj::maxSupportedBdexVersion(0));
            ASSERT(2 == Obj::maxSupportedBdexVersion(20170401));
            ASSERT(2 == Obj::maxSupportedBdexVersion(VERSION_SELECTOR));

            using bslx::VersionFunctions::maxSupportedBdexVersion;

            ASSERT(1 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                0));
            ASSERT(2 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                20170401));
            ASSERT(2 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                VERSION_SELECTOR));
        }

        const int VERSIONS[] = { 1, 2 };
        const int NUM_VERSIONS = static_cast<int>(  sizeof VERSIONS
                                                  / sizeof *VERSIONS);

        for (int versionIndex = 0;
             versionIndex < NUM_VERSIONS;
             ++versionIndex) {
            const int VERSION = VERSIONS[versionIndex];

            if (verbose) {
                cout << "\nTesting Version " << VERSION << "." << endl;
            }

            if (verbose) {
                cout << "\tDirect initial trial of 'bdexStreamOut' and "
                     << "(valid) 'bdexStreamIn'." << endl;
            }
            {
                const Obj X(VC);
                Out       out(VERSION_SELECTOR, &allocator);

                Out& rvOut = X.bdexStreamOut(out, VERSION);
                ASSERT(&out == &rvOut);
                ASSERT(out);

                const char *const OD  = out.data();
                const int         LOD = static_cast<int>(out.length());

                In in(OD, LOD);
                ASSERT(in);
                ASSERT(!in.isEmpty());

                Obj mT(VA);  const Obj& T = mT;
                ASSERT(X != T);

                In& rvIn = mT.bdexStreamIn(in, VERSION);
                ASSERT(&in == &rvIn);
                ASSERT(X == T);
                ASSERT(in);
                ASSERT(in.isEmpty());
            }

            // We will use the stream free functions provided by 'bslx', as
            // opposed to the class member functions, since the 'bslx'
            // implementation gives priority to the free function
            // implementations; we want to test what will be used.
            // Furthermore, toward making this test case more reusable in other
            // components, from here on we generally use the 'bdexStreamIn' and
            // 'bdexStreamOut' free functions that are defined in the 'bslx'
            // package rather than call the like-named member functions
            // directly.

            if (verbose) {
                cout << "\tThorough test using stream free functions."
                     << endl;
            }
            {
                for (int i = 0; i < NUM_VALUES; ++i) {
                    const Obj X(VALUES[i]);

                    Out out(VERSION_SELECTOR, &allocator);

                    using bslx::OutStreamFunctions::bdexStreamOut;
                    using bslx::InStreamFunctions::bdexStreamIn;

                    Out& rvOut = bdexStreamOut(out, X, VERSION);
                    LOOP_ASSERT(i, &out == &rvOut);
                    LOOP_ASSERT(i, out);
                    const char *const OD  = out.data();
                    const int         LOD = static_cast<int>(out.length());

                    // Verify that each new value overwrites every old value
                    // and that the input stream is emptied, but remains valid.

                    for (int j = 0; j < NUM_VALUES; ++j) {
                        In in(OD, LOD);
                        LOOP2_ASSERT(i, j, in);
                        LOOP2_ASSERT(i, j, !in.isEmpty());

                        Obj mT(VALUES[j]);  const Obj& T = mT;
                        LOOP2_ASSERT(i, j, (X == T) == (i == j));

                        BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                            in.reset();
                            In& rvIn = bdexStreamIn(in, mT, VERSION);
                            LOOP2_ASSERT(i, j, &in == &rvIn);
                        } BSLX_TESTINSTREAM_EXCEPTION_TEST_END

                        if (1 == VERSION) {
                            // Version 1 loses microseconds; replace.

                            if (24 != T.hour()) {
                                mT.setMicrosecond(X.microsecond());
                            }
                        }

                        LOOP2_ASSERT(i, j, X == T);
                        LOOP2_ASSERT(i, j, in);
                        LOOP2_ASSERT(i, j, in.isEmpty());
                    }
                }
            }

            if (verbose) {
                cout << "\tOn empty streams and non-empty, invalid streams."
                     << endl;
            }

            // Verify correct behavior for empty streams (valid and invalid).

            {
                Out               out(VERSION_SELECTOR, &allocator);
                const char *const OD  = out.data();
                const int         LOD = static_cast<int>(out.length());
                ASSERT(0 == LOD);

                for (int i = 0; i < NUM_VALUES; ++i) {
                    In in(OD, LOD);
                    LOOP_ASSERT(i, in);
                    LOOP_ASSERT(i, in.isEmpty());

                    // Ensure that reading from an empty or invalid input
                    // stream leaves the stream invalid and the target object
                    // unchanged.

                    using bslx::InStreamFunctions::bdexStreamIn;

                    const Obj  X(VALUES[i]);
                    Obj        mT(X);
                    const Obj& T = mT;
                    LOOP_ASSERT(i, X == T);

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        in.reset();

                        // Stream is valid.
                        In& rvIn1 = bdexStreamIn(in, mT, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, X == T);

                        // Stream is invalid.
                        In& rvIn2 = bdexStreamIn(in, mT, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, X == T);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                }
            }

            // Verify correct behavior for non-empty, invalid streams.

            {
                Out               out(VERSION_SELECTOR, &allocator);

                using bslx::OutStreamFunctions::bdexStreamOut;
                Out& rvOut = bdexStreamOut(out, Obj(), VERSION);
                ASSERT(&out == &rvOut);
                ASSERT(out);

                const char *const OD  = out.data();
                const int         LOD = static_cast<int>(out.length());
                ASSERT(0 < LOD);

                for (int i = 0; i < NUM_VALUES; ++i) {
                    In in(OD, LOD);
                    in.invalidate();
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, !in.isEmpty());

                    // Ensure that reading from a non-empty, invalid input
                    // stream leaves the stream invalid and the target object
                    // unchanged.

                    using bslx::InStreamFunctions::bdexStreamIn;

                    const Obj  X(VALUES[i]);
                    Obj        mT(X);
                    const Obj& T = mT;
                    LOOP_ASSERT(i, X == T);

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        in.reset();
                        in.invalidate();
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, !in.isEmpty());

                        In& rvIn = bdexStreamIn(in, mT, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn);
                        LOOP_ASSERT(i, !in);
                        LOOP_ASSERT(i, X == T);
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                }
            }

            if (verbose) {
                cout << "\tOn incomplete (but otherwise valid) data."
                     << endl;
            }
            {
                const Obj W1 = VA, X1 = VB;
                const Obj W2 = VB, X2 = VC;
                const Obj W3 = VC, X3 = VD;

                using bslx::OutStreamFunctions::bdexStreamOut;
                using bslx::InStreamFunctions::bdexStreamIn;

                Out out(VERSION_SELECTOR, &allocator);

                Out& rvOut1 = bdexStreamOut(out, X1, VERSION);
                ASSERT(&out == &rvOut1);
                ASSERT(out);
                const int         LOD1 = static_cast<int>(out.length());

                Out& rvOut2 = bdexStreamOut(out, X2, VERSION);
                ASSERT(&out == &rvOut2);
                ASSERT(out);
                const int         LOD2 = static_cast<int>(out.length());

                Out& rvOut3 = bdexStreamOut(out, X3, VERSION);
                ASSERT(&out == &rvOut3);
                ASSERT(out);
                const int         LOD3 = static_cast<int>(out.length());
                const char *const OD3  = out.data();

                for (int i = 0; i < LOD3; ++i) {
                    In in(OD3, i);

                    BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                        in.reset();
                        LOOP_ASSERT(i, in);
                        LOOP_ASSERT(i, !i == in.isEmpty());

                        Obj mT1(W1);  const Obj& T1 = mT1;
                        Obj mT2(W2);  const Obj& T2 = mT2;
                        Obj mT3(W3);  const Obj& T3 = mT3;

                        if (i < LOD1) {
                            In& rvIn1 = bdexStreamIn(in, mT1, VERSION);
                            LOOP_ASSERT(i, &in == &rvIn1);
                            LOOP_ASSERT(i, !in);
                            if (0 == i) LOOP_ASSERT(i, W1 == T1);
                            In& rvIn2 = bdexStreamIn(in, mT2, VERSION);
                            LOOP_ASSERT(i, &in == &rvIn2);
                            LOOP_ASSERT(i, !in);
                            LOOP_ASSERT(i, W2 == T2);
                            In& rvIn3 = bdexStreamIn(in, mT3, VERSION);
                            LOOP_ASSERT(i, &in == &rvIn3);
                            LOOP_ASSERT(i, !in);
                            LOOP_ASSERT(i, W3 == T3);
                        }
                        else if (i < LOD2) {
                            In& rvIn1 = bdexStreamIn(in, mT1, VERSION);
                            if (1 == VERSION) {
                                // Version 1 loses microseconds; replace.

                                if (24 != T1.hour()) {
                                    mT1.setMicrosecond(X1.microsecond());
                                }
                            }
                            LOOP_ASSERT(i, &in == &rvIn1);
                            LOOP_ASSERT(i,  in);
                            LOOP_ASSERT(i, X1 == T1);
                            In& rvIn2 = bdexStreamIn(in, mT2, VERSION);
                            LOOP_ASSERT(i, &in == &rvIn2);
                            LOOP_ASSERT(i, !in);
                            if (LOD1 <= i) LOOP_ASSERT(i, W2 == T2);
                            In& rvIn3 = bdexStreamIn(in, mT3, VERSION);
                            LOOP_ASSERT(i, &in == &rvIn3);
                            LOOP_ASSERT(i, !in);
                            LOOP_ASSERT(i, W3 == T3);
                        }
                        else {  // 'LOD2 <= i < LOD3'
                            In& rvIn1 = bdexStreamIn(in, mT1, VERSION);
                            if (1 == VERSION) {
                                // Version 1 loses microseconds; replace.

                                if (24 != T1.hour()) {
                                    mT1.setMicrosecond(X1.microsecond());
                                }
                            }
                            LOOP_ASSERT(i, &in == &rvIn1);
                            LOOP_ASSERT(i,  in);
                            LOOP_ASSERT(i, X1 == T1);
                            In& rvIn2 = bdexStreamIn(in, mT2, VERSION);
                            if (1 == VERSION) {
                                // Version 1 loses microseconds; replace.

                                if (24 != T2.hour()) {
                                    mT2.setMicrosecond(X2.microsecond());
                                }
                            }
                            LOOP_ASSERT(i, &in == &rvIn2);
                            LOOP_ASSERT(i,  in);
                            LOOP_ASSERT(i, X2 == T2);
                            In& rvIn3 = bdexStreamIn(in, mT3, VERSION);
                            LOOP_ASSERT(i, &in == &rvIn3);
                            LOOP_ASSERT(i, !in);
                            if (LOD2 <= i) LOOP_ASSERT(i, W3 == T3);
                        }

                        // Verify the objects are in a valid state.
                        LOOP_ASSERT(i,
                                    (   (      0 <= T1.hour()
                                         &&   24 >  T1.hour()
                                         &&    0 <= T1.minute()
                                         &&   60 >  T1.minute()
                                         &&    0 <= T1.second()
                                         &&   60 >  T1.second()
                                         &&    0 <= T1.millisecond()
                                         && 1000 >  T1.millisecond()
                                         &&    0 <= T1.microsecond()
                                         && 1000 >  T1.microsecond())
                                     || (     24 == T1.hour()
                                         &&    0 == T1.minute()
                                         &&    0 == T1.second()
                                         &&    0 == T1.millisecond()
                                         &&    0 == T1.microsecond())));

                        LOOP_ASSERT(i,
                                    (   (      0 <= T2.hour()
                                         &&   24 >  T2.hour()
                                         &&    0 <= T2.minute()
                                         &&   60 >  T2.minute()
                                         &&    0 <= T2.second()
                                         &&   60 >  T2.second()
                                         &&    0 <= T2.millisecond()
                                         && 1000 >  T2.millisecond()
                                         &&    0 <= T2.microsecond()
                                         && 1000 >  T2.microsecond())
                                     || (     24 == T2.hour()
                                         &&    0 == T2.minute()
                                         &&    0 == T2.second()
                                         &&    0 == T2.millisecond()
                                         &&    0 == T2.microsecond())));

                        LOOP_ASSERT(i,
                                    (   (      0 <= T3.hour()
                                         &&   24 >  T3.hour()
                                         &&    0 <= T3.minute()
                                         &&   60 >  T3.minute()
                                         &&    0 <= T3.second()
                                         &&   60 >  T3.second()
                                         &&    0 <= T3.millisecond()
                                         && 1000 >  T3.millisecond()
                                         &&    0 <= T3.microsecond()
                                         && 1000 >  T3.microsecond())
                                     || (     24 == T3.hour()
                                         &&    0 == T3.minute()
                                         &&    0 == T3.second()
                                         &&    0 == T3.millisecond()
                                         &&    0 == T3.microsecond())));

                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                }
            }
        }

        if (verbose) {
            cout << "\tOn corrupted data." << endl;
        }

        const Obj W;              // default value
        const Obj X(0, 1, 0, 0);  // original (control)
        const Obj Y(0, 0, 0, 1);  // new (streamed-out)

        // Verify the three objects are distinct.
        ASSERT(W != X);
        ASSERT(W != Y);
        ASSERT(X != Y);

        if (verbose) {
            cout << "\t\tGood stream (for control)." << endl;
        }
        {
            // Version 1.

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            out.putInt32(1);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 1);
            ASSERT(&in == &rvIn);
            ASSERT(in);
            ASSERT(Y == T);
        }
        {
            // Version 2.

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            out.putInt64(bdlt::TimeUnitRatio::k_US_PER_MS);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 2);
            ASSERT(&in == &rvIn);
            ASSERT(in);
            ASSERT(Y == T);
        }

        if (verbose) {
            cout << "\t\tBad version." << endl;
        }
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            out.putInt32(1);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, version);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            const char version = 3 ; // too large (current version is 2)

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            out.putInt32(1);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, version);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            cout << "\t\tBad value." << endl;
        }
        {
            // Version 1.  Value too small.

            Out out(VERSION_SELECTOR, &allocator);
            out.putInt32(-1);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 1);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            // Version 1.  Value too large.

            Out out(VERSION_SELECTOR, &allocator);
            out.putInt32(24*60*60*1000 + 1);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 1);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            // Version 2.  Value too small.

            Out out(VERSION_SELECTOR, &allocator);
            out.putInt64(-1LL);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 1);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }
        {
            // Version 2.  Value too large.

            Out out(VERSION_SELECTOR, &allocator);
            out.putInt64(24LL*60*60*1000*1000 + 1);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, 1);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
             cout << "\nWire format direct tests." << endl;
        }
        {
            static const struct {
                int          d_lineNum;      // source line number
                int          d_hour;         // specification hour
                int          d_minute;       // specification minute
                int          d_second;       // specification second
                int          d_millisecond;  // specification millisecond
                int          d_microsecond;  // specification microsecond
                int          d_version;      // version to stream with
                bsl::size_t  d_length;       // expect output length
                const char  *d_fmt_p;        // expected output format
            } DATA[] = {
                //LI  H   M   S   MS   US   V  L  FORMAT
                //--  --  --  --  ---  ---  -  -  ------------------
                { L_, 14, 10,  2, 117,   0, 1, 4, "\x03\x0a\x3b\x05" },
                { L_, 20,  8, 27, 983,   0, 1, 4, "\x04\x52\x62\x4f" },
                { L_, 14, 10,  2, 117,   0, 2, 8,
                                  "\x00\x00\x00\x0b\xdf\xf6\x8b\x88" },
                { L_, 20,  8, 27, 983,   0, 2, 8,
                                  "\x00\x00\x00\x10\xe1\xd0\x04\x98" },
                { L_, 14, 10,  2, 117, 212, 2, 8,
                                  "\x00\x00\x00\x0b\xdf\xf6\x8c\x5c" },
                { L_, 20,  8, 27, 983, 374, 2, 8,
                                  "\x00\x00\x00\x10\xe1\xd0\x06\x0e" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_lineNum;
                const int         HOUR        = DATA[i].d_hour;
                const int         MINUTE      = DATA[i].d_minute;
                const int         SECOND      = DATA[i].d_second;
                const int         MILLISECOND = DATA[i].d_millisecond;
                const int         MICROSECOND = DATA[i].d_microsecond;
                const int         VERSION     = DATA[i].d_version;
                const bsl::size_t LEN         = DATA[i].d_length;
                const char *const FMT         = DATA[i].d_fmt_p;

                // Test using class methods.

                {
                    Obj        mX(HOUR,
                                  MINUTE,
                                  SECOND,
                                  MILLISECOND,
                                  MICROSECOND);
                    const Obj& X = mX;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = X.bdexStreamOut(out, VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);
                    LOOP_ASSERT(LINE, out);

                    LOOP_ASSERT(LINE, LEN == out.length());
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (bsl::size_t j = 0; j < out.length(); ++j) {
                            cout << "\\x"
                                 << hex[static_cast<unsigned char>
                                            ((*(out.data() + j) >> 4) & 0x0f)]
                                 << hex[static_cast<unsigned char>
                                                   (*(out.data() + j) & 0x0f)];
                        }
                        cout << endl;
                    }

                    Obj mY;  const Obj& Y = mY;

                    bslx::ByteInStream  in(out.data(), out.length());
                    bslx::ByteInStream& rvIn = mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, &in == &rvIn);
                    LOOP_ASSERT(LINE, X == Y);
                }

                // Test using free functions.

                {
                    Obj        mX(HOUR,
                                  MINUTE,
                                  SECOND,
                                  MILLISECOND,
                                  MICROSECOND);
                    const Obj& X = mX;

                    using bslx::OutStreamFunctions::bdexStreamOut;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = bdexStreamOut(out,
                                                               X,
                                                               VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);
                    LOOP_ASSERT(LINE, out);

                    LOOP_ASSERT(LINE, LEN == out.length());
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (bsl::size_t j = 0; j < out.length(); ++j) {
                            cout << "\\x"
                                 << hex[static_cast<unsigned char>
                                            ((*(out.data() + j) >> 4) & 0x0f)]
                                 << hex[static_cast<unsigned char>
                                                   (*(out.data() + j) & 0x0f)];
                        }
                        cout << endl;
                    }

                    Obj mY;  const Obj& Y = mY;

                    using bslx::InStreamFunctions::bdexStreamIn;

                    bslx::ByteInStream  in(out.data(), out.length());
                    bslx::ByteInStream& rvIn = bdexStreamIn(in, mY, VERSION);
                    LOOP_ASSERT(LINE, &in == &rvIn);
                    LOOP_ASSERT(LINE, X == Y);
                }
            }
        }

#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation

        if (verbose) {
            cout << "\nTesting deprecated methods." << endl;
        }
        {
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
            ASSERT(Obj::maxSupportedVersion()
                                           == Obj::maxSupportedBdexVersion(0));
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
            ASSERT(Obj::maxSupportedBdexVersion()
                                           == Obj::maxSupportedBdexVersion(0));
        }

#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR
        //   Verify the assignment operator works as expected.
        //
        // Concerns:
        //: 1 Any value is assignable to an object having any initial value
        //:   without affecting the rhs operand value.
        //:
        //: 2 Any object must be assignable to itself.
        //
        // Plan:
        //: 1 Construct and initialize a set S of (unique) objects with
        //:   substantial and varied differences in value.  Using all
        //:   combinations (u, v) in the cross product S x S, assign v to u and
        //:   assert that u == v and v is unchanged.  (C-1)
        //:
        //: 2 Test aliasing by assigning (a temporary copy of) each u to
        //:   itself and verifying that its value remains unchanged.  (C-2)
        //
        // Testing:
        //   Time& operator=(const Time& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING ASSIGNMENT OPERATOR" << endl
                          << "===========================" << endl;

        if (verbose) cout << "\nTesting Assignment u = V." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 24,  0,  0,   0  },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            int i;
            for (i = 0; i < NUM_VALUES; ++i) {
                Obj v;  const Obj& V = v;
                v.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u;  const Obj& U = u;
                    u.setTime(VALUES[j].d_hour,    VALUES[j].d_minute,
                              VALUES[j].d_second,  VALUES[j].d_msec);
                    if (veryVerbose) { T_;  P_(V);  P_(U); }
                    Obj w(V);  const Obj &W = w;          // control
                    ASSERT(&u == &(u = V));
                    if (veryVerbose) P(U);
                    LOOP2_ASSERT(i, j, W == U);
                    LOOP2_ASSERT(i, j, W == V);
                }
            }

            if (verbose) {
                cout << "\nTesting assignment u = u (Aliasing)."
                     << endl;
            }

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj u;  const Obj& U = u;
                u.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                Obj w(U);  const Obj &W = w;              // control
                ASSERT(&u == &(u = u));
                if (veryVerbose) { T_;  P_(U);  P(W); }
                LOOP_ASSERT(i, W == U);
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTIONS
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //  Reserved for 'swap' testing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTIONS" << endl
                          << "==============================" << endl;

        if (verbose) cout << "Not implemented for 'bdlt::Time'." << endl;

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   Verify the copy constructor works as expected.
        //
        // Concerns:
        //: 1 Any value must be able to be copy constructed without affecting
        //:   the argument.
        //
        // Plan:
        //: 1 Specify a set S of control objects with substantial and varied
        //:   differences in value.  For each object w in S, construct and
        //:   initialize an identically valued object x using the primary
        //:   manipulator, and copy construct an object y from x.  Use the
        //:   equality operator to assert that both x and y have the same value
        //:   as w.  (C-1)
        //
        // Testing:
        //   Time(const Time& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY CONSTRUCTOR" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting copy constructor." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 24,  0,  0,   0  },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj x;  const Obj& X = x;
                x.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                Obj w;  const Obj& W = w;
                w.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);

                Obj y(X);  const Obj &Y = y;
                if (veryVerbose) { T_;  P_(W);  P_(X);  P(Y); }
                LOOP_ASSERT(i, X == W);
                LOOP_ASSERT(i, Y == W);
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //   The equality and inequality operators work as expected.
        //
        // Concerns:
        //: 1 Any subtle variation in value must be detected by the equality
        //:   operators.  The test data have variations in each input
        //:   parameter, even though tested methods convert the input before
        //:   the underlying equality operator on a single pair of 'int's is
        //:   invoked.
        //:
        //: 2 The default value (24:00:00.000000) is handled by the methods
        //:   correctly.
        //
        // Plan:
        //: 1 Specify a set S of unique object values having various minor or
        //:   subtle differences, but also including the default value.  Verify
        //:   the correctness of 'operator==' and 'operator!=' using all
        //:   elements (u, v) of the cross product S X S.  (C-1,2)
        //
        // Testing:
        //   bool operator==(const Time& lhs, const Time& rhs);
        //   bool operator!=(const Time& lhs, const Time& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING EQUALITY OPERATORS" << endl
                          << "==========================" << endl;

        if (verbose) {
            cout << "\nCompare each pair of values (u, v) in S X S." << endl;
        }
        {
            static const struct {
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
                int d_usec;
            } VALUES[] = {
                {  0,  0,  0,   0,   0 },
                {  0,  0,  0,   0,   1 },
                {  0,  0,  0,   1,   0 },
                {  0,  0,  1,   0,   0 },
                {  0,  1,  0,   0,   0 },
                {  1,  0,  0,   0,   0 },

                { 23, 22, 21, 209,   0 },
                { 23, 22, 21, 208,   0 },
                { 23, 22, 20, 209,   0 },
                { 23, 21, 21, 209,   0 },
                { 22, 22, 21, 209,   0 },
                { 23, 22, 21, 209, 102 },
                { 23, 22, 21, 208,  17 },
                { 23, 22, 20, 209, 102 },
                { 23, 21, 21, 209,  17 },
                { 22, 22, 21, 209, 102 },

                // 24:00:00.000.000000 explicitly included
                { 24,  0,  0,   0,   0 },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj u;  const Obj& U = u;
                u.setTime(VALUES[i].d_hour,
                          VALUES[i].d_minute,
                          VALUES[i].d_second,
                          VALUES[i].d_msec,
                          VALUES[i].d_usec);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj v;  const Obj& V = v;
                    v.setTime(VALUES[j].d_hour,
                              VALUES[j].d_minute,
                              VALUES[j].d_second,
                              VALUES[j].d_msec,
                              VALUES[j].d_usec);
                    bool isSame = i == j;
                    if (veryVerbose) { T_;  P_(i);  P_(j);  P_(U);  P(V); }
                    LOOP2_ASSERT(i, j,  isSame == (U == V));
                    LOOP2_ASSERT(i, j, !isSame == (U != V));
                    LOOP2_ASSERT(i, j,  isSame == (V == U));
                    LOOP2_ASSERT(i, j, !isSame == (V != U));
                }
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT, OUTPUT OPERATOR, AND 'printToBuffer'
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.  In
        //:   particular:
        //:
        //:   1 The attributes always appear on a single line.
        //:
        //:   2 A negative value of 'level' always suppresses all indentation
        //:     (since there is never a second line to indent).,
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'.
        //:
        //: 4 The 'print' method signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values.
        //:
        //: 7 The output 'operator<<' signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the supplied 'ostream'.
        //:
        //: 9 The 'printToBuffer' method:
        //:   1 Writes in the expected format.
        //:   2 Never writes more than the specified limit.
        //:   3 Writes in the specified buffer.
        //:   4 QoI: Asserted precondition violations are detected when
        //:     enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique: (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B'), having distinct values for each
        //:     corresponding salient attribute, and various values for the two
        //:     formatting parameters, along with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A } x { 0 } x { 0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A } x { 3, -3 } x { 0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B } x { 2 } x { 3 } --> 1 expected o/p
        //:     4 { A B } x { -8 } x { -8 } --> 2 expected o/ps
        //:     5 { A B } x { -9 } x { -9 } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1: (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, (-9, -9), then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //:
        //:   3 Test 'printToBuffer' using a table-driven approach.  (C-9)
        //:
        //:     1 Define an assortment of different input values and limits on
        //:       the number of bytes written.
        //:
        //:     2 For each input value, write the result into an over-sized
        //:       buffer that is pre-filled with an "unset" character.  Data is
        //:       written into the middle of the buffer.  After writing,
        //:       confirm that all characters outside the targeted range have
        //:       their initial value.
        //:
        //:     4 Verify that, in appropriate build modes, defensive checks are
        //:       triggered for invalid attribute values, but not triggered for
        //:       adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //
        // Testing:
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        //   ostream& operator<<(ostream &stream, const Time &object);
        //   int printToBuffer(char *result, int size, int precision) const;
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
        //   ostream& streamOut(ostream& stream) const;
#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
        // --------------------------------------------------------------------

        if (verbose) cout
                      << endl
                      << "PRINT, OUTPUT OPERATOR, AND 'printToBuffer'" << endl
                      << "===========================================" << endl;

        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOp  = bdlt::operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOp;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        const Obj TA( 0,  0,  0,   0);
        const Obj TZ(23, 59, 59, 999);

        static const struct {
            int         d_line;           // source line number
            int         d_level;
            int         d_spacesPerLevel;
            int         d_hour;
            int         d_minute;
            int         d_second;
            int         d_millisecond;
            int         d_microsecond;
            const char *d_expected_p;
        } DATA[] = {

#define NL "\n"

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 } x { 0, 1, -1, -8 } --> 4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  H   M   S   MS   US   EXPECTED
        //---- - ---  --  --  --  ---  ---  --------
        { L_,  0,  0,  0,  0,  0,   0,   0, "00:00:00.000000" NL },
        { L_,  0,  1,  0,  0,  0,   0,   0, "00:00:00.000000" NL },
        { L_,  0, -1,  0,  0,  0,   0,   0, "00:00:00.000000"    },

        { L_,  0, -8,  0,  0,  0,   0,   0, "00:00:00.000000" NL }, // default

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } --> 6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL  H   M   S   MS   US   EXPECTED
        //---- - ---  --  --  --  ---  ---  --------
        { L_,  3,  0,  0,  0,  0,   0,   0, "00:00:00.000000"             NL },
        { L_,  3,  2,  0,  0,  0,   0,   0, "      00:00:00.000000"       NL },
        { L_,  3, -2,  0,  0,  0,   0,   0, "      00:00:00.000000"          },

        { L_,  3, -8,  0,  0,  0,   0,   0, "            00:00:00.000000" NL },
                                                                 // default

        { L_, -3,  0,  0,  0,  0,   0,   0, "00:00:00.000000" NL },
        { L_, -3,  2,  0,  0,  0,   0,   0, "00:00:00.000000" NL },
        { L_, -3, -2,  0,  0,  0,   0,   0, "00:00:00.000000"    },

        { L_, -3, -8,  0,  0,  0,   0,   0, "00:00:00.000000" NL }, // default

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 } x { 3 } --> 1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL  H   M   S   MS   US   EXPECTED
        //---- - ---  --  --  --  ---  ---  --------
        { L_,  2,  3, 23, 59, 59, 999, 999, "      23:59:59.999999" NL },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 } x { -8 } --> 2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  H   M   S   MS   US   EXPECTED
        //---- - ---  --  --  --  ---  ---  --------
        { L_, -8, -8,  0,  0,  0,   0,   0, "00:00:00.000000" NL }, // default
        { L_, -8, -8, 23, 59, 59, 999, 999, "23:59:59.999999" NL }, // default

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 } x { -9 } --> 2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL  H   M   S   MS   US   EXPECTED
        //---- - ---  --  --  --  ---  ---  --------
        { L_, -9, -9,  0,  0,  0,   0,   0, "00:00:00.000000" },
        { L_, -9, -9, 23, 59, 59, 999, 999, "23:59:59.999999" }

#undef NL
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE   = DATA[ti].d_line;
                const int         L      = DATA[ti].d_level;
                const int         SPL    = DATA[ti].d_spacesPerLevel;
                const int         HOUR   = DATA[ti].d_hour;
                const int         MINUTE = DATA[ti].d_minute;
                const int         SECOND = DATA[ti].d_second;
                const int         MSEC   = DATA[ti].d_millisecond;
                const int         USEC   = DATA[ti].d_microsecond;
                const char *const EXP    = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_ P_(L) P_(SPL) P_(HOUR) P_(MINUTE) P_(SECOND) P_(MSEC)
                                                                       P(USEC);
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                Obj x;  const Obj& X = x;

                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);

                bslma::TestAllocator oa("scratch",  veryVeryVeryVerbose);
                stringstream ss(&oa);

                if (-9 == L && -9 == SPL) {

                    // Verify supplied stream is returned by reference.

                    LOOP_ASSERT(LINE, &ss == &(ss << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    LOOP_ASSERT(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        LOOP_ASSERT(LINE, &ss == &X.print(ss, L, SPL));
                    }
                    else if (-8 != L) {
                        LOOP_ASSERT(LINE, &ss == &X.print(ss, L));
                    }
                    else {
                        LOOP_ASSERT(LINE, &ss == &X.print(ss));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                // Avoid invoking 'ss.str()' which returns a string by value
                // and may introduce use of the default allocator.

                bsl::string result(bsl::istreambuf_iterator<char>(ss),
                                   bsl::istreambuf_iterator<char>(),
                                   &oa);

                if (veryVeryVerbose) { P(result) }

                LOOP3_ASSERT(LINE, EXP, result, EXP == result);
            }
        }

        if (verbose) cout << "\nTesting 'printToBuffer'." << endl;
        {
            static const struct {
                int         d_line;
                int         d_hour;
                int         d_minute;
                int         d_second;
                int         d_msec;
                int         d_usec;
                int         d_precision;
                int         d_numBytes;
                const char *d_expected_p;
            } DATA[] = {
        //------^
        //LN  HR  M   S   MS   US   PREC  LIMIT  EXPECTED
        //--  --  --  --  ---  ---  ----  -----  -----------------
        { L_,  0,  0,  0,   0,   0,    0,   100, "00:00:00"        },
        { L_,  0,  0,  0,   0,   0,    1,   100, "00:00:00.0"      },
        { L_,  0,  0,  0,   0,   0,    3,   100, "00:00:00.000"    },
        { L_,  0,  0,  0,   0,   0,    6,   100, "00:00:00.000000" },
        { L_,  0,  0,  0,   0,   7,    0,   100, "00:00:00"        },
        { L_,  0,  0,  0,   0,   7,    1,   100, "00:00:00.0"      },
        { L_,  0,  0,  0,   0,   7,    3,   100, "00:00:00.000"    },
        { L_,  0,  0,  0,   0,   7,    5,   100, "00:00:00.00000"  },
        { L_,  0,  0,  0,   0,   7,    6,   100, "00:00:00.000007" },
        { L_,  0,  0,  0,   0,  17,    6,   100, "00:00:00.000017" },
        { L_,  0,  0,  0,   0, 317,    3,   100, "00:00:00.000"    },
        { L_,  0,  0,  0,   0, 317,    4,   100, "00:00:00.0003"   },
        { L_,  0,  0,  0,   0, 317,    5,   100, "00:00:00.00031"  },
        { L_,  0,  0,  0,   0, 317,    6,   100, "00:00:00.000317" },
        { L_, 23, 22, 21, 209,   0,    6,   100, "23:22:21.209000" },
        { L_, 23, 22, 21, 210,   0,    6,   100, "23:22:21.210000" },
        { L_, 23, 22, 21, 211,   0,    6,   100, "23:22:21.211000" },
        { L_, 24,  0,  0,   0,   0,    6,   100, "24:00:00.000000" },
        { L_, 23, 59, 59, 999,   0,    6,   100, "23:59:59.999000" },
        { L_, 23, 59, 59, 999,   5,    6,   100, "23:59:59.999005" },
        { L_, 23, 59, 59, 999,  65,    6,   100, "23:59:59.999065" },
        { L_, 23, 59, 59, 999, 765,    0,   100, "23:59:59"        },
        { L_, 23, 59, 59, 999, 765,    1,   100, "23:59:59.9"      },
        { L_, 23, 59, 59, 999, 765,    2,   100, "23:59:59.99"     },
        { L_, 23, 59, 59, 999, 765,    3,   100, "23:59:59.999"    },
        { L_, 23, 59, 59, 999, 765,    4,   100, "23:59:59.9997"   },
        { L_, 23, 59, 59, 999, 765,    5,   100, "23:59:59.99976"  },
        { L_, 23, 59, 59, 999, 765,    6,   100, "23:59:59.999765" },
        { L_, 23, 59, 59, 999,   0,    6,     0, ""                },
        { L_, 23, 59, 59, 999,   0,    6,     1, ""                },
        { L_, 23, 59, 59, 999,   0,    6,     2, "2"               },
        { L_, 23, 59, 59, 999,   0,    6,    12, "23:59:59.99"     },
        { L_, 23, 59, 59, 999,   0,    6,    13, "23:59:59.999"    },
        { L_, 23, 59, 59, 999,   0,    6,    14, "23:59:59.9990"   },
        { L_, 23, 59, 59, 999,   0,    6,    15, "23:59:59.99900"  },
        { L_, 23, 59, 59, 999,   0,    6,    16, "23:59:59.999000" },
        //------v
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            const int  BUF_SIZE = 1000;               // Over-sized for output.
            const char XX       = static_cast<char>(0xFF);
                                                      // Used as "unset"
                                                      // character.
            char       mCtrlBuf[BUF_SIZE];
            memset(mCtrlBuf, XX, sizeof(mCtrlBuf));
            const char *const CTRL_BUF = mCtrlBuf;    // Referenced in overrun
                                                      // checks.

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE     = DATA[ti].d_line;
                const int         HOUR     = DATA[ti].d_hour;
                const int         MINUTE   = DATA[ti].d_minute;
                const int         SECOND   = DATA[ti].d_second;
                const int         MSEC     = DATA[ti].d_msec;
                const int         USEC     = DATA[ti].d_usec;
                const int         PREC     = DATA[ti].d_precision;
                const int         LIMIT    = DATA[ti].d_numBytes;
                const char *const EXPECTED = DATA[ti].d_expected_p;
                const int         EXP_LEN  = 0 == PREC ? 8 : 9 + PREC;

                if (veryVerbose) {
                    T_  P_(HOUR)
                        P_(MINUTE)
                        P_(SECOND)
                        P_(MSEC)
                        P(USEC)
                    T_  P_(PREC)
                    T_  P_(LIMIT)
                    T_  P(EXPECTED)
                }

                char buf[BUF_SIZE];

                // Preset 'buf' to "unset" values.
                memset(buf, XX, sizeof(buf));

                Obj x;  const Obj& X = x;
                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);

                char      *p = buf + sizeof(buf)/2;
                const int  RC = X.printToBuffer(p, LIMIT, PREC);

                LOOP2_ASSERT(LINE, RC, EXP_LEN == RC);

                const int LENGTH = 0 == LIMIT
                                   ? 0
                                   : static_cast<int>(strlen(p) + 1);
                LOOP_ASSERT(LINE, LENGTH <= LIMIT);

                if (veryVerbose) cout
                                   << "\tACTUAL FORMAT: "
                                   << (0 < LENGTH ? p : "<all-unset-expected>")
                                   << endl;
                LOOP_ASSERT(LINE, 0 == memcmp(buf, CTRL_BUF, p - buf));
                if (0 < LENGTH) {
                    LOOP3_ASSERT(LINE, p, EXPECTED,
                                 0 == memcmp(p, EXPECTED, LENGTH));
                }
                LOOP_ASSERT(LINE, 0 == memcmp(p + LENGTH,
                                              CTRL_BUF,
                                              (buf + sizeof(buf)) -
                                              (p   + LENGTH)));
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            if (veryVerbose) cout << "\t'printToBuffer' method" << endl;
            {
                const int SIZE = 128;
                char      buf[SIZE];

                const Obj X;

                const int PRECISION = 6;

                ASSERT_SAFE_PASS(X.printToBuffer(buf, SIZE, PRECISION));
                ASSERT_SAFE_PASS(X.printToBuffer(buf,  0  , PRECISION));
                ASSERT_SAFE_PASS(X.printToBuffer(buf, SIZE, 0));

                ASSERT_FAIL(X.printToBuffer(0,   SIZE, PRECISION));
                ASSERT_FAIL(X.printToBuffer(buf, -1  , PRECISION));
                ASSERT_FAIL(X.printToBuffer(0,   -1  , PRECISION));
                ASSERT_FAIL(X.printToBuffer(buf,  0  , -1));
                ASSERT_FAIL(X.printToBuffer(buf,  0  , PRECISION + 1));
            }
        }

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22

        if (verbose) cout << "\nTesting 'streamOut'." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                int         d_hour;     // hour field value
                int         d_minute;   // minute field value
                int         d_second;   // second field value
                int         d_msec;     // millisecond field value
                int         d_usec;     // microsecond field value
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
            //LINE  HOUR   MIN    SEC    MSEC    USEC     OUTPUT FORMAT
            //----  -----  ----   ----   -----   -----    --------------
            { L_,      0,    0,     0,      0,      0,    "00:00:00.000000"  },
            { L_,      0,    0,     0,      0,    999,    "00:00:00.000999"  },
            { L_,      0,    0,     0,    999,      0,    "00:00:00.999000"  },
            { L_,      0,    0,    59,      0,      0,    "00:00:59.000000"  },
            { L_,      0,   59,     0,      0,      0,    "00:59:00.000000"  },
            { L_,     23,    0,     0,      0,      0,    "23:00:00.000000"  },
            { L_,     23,   22,    21,    209,      0,    "23:22:21.209000"  },
            { L_,     23,   22,    21,    210,      0,    "23:22:21.210000"  },
            { L_,     23,   22,    21,    210,      1,    "23:22:21.210001"  },
            { L_,     23,   22,    21,    210,     17,    "23:22:21.210017"  },
            { L_,     23,   22,    21,    210,    412,    "23:22:21.210412"  },
            { L_,     24,    0,     0,      0,      0,    "24:00:00.000000"  },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            const int SIZE = 1000;     // Must be able to hold output string.

            const char XX = static_cast<char>(0xFF);  // Value used for an
                                                      // unset 'char'.

            char        mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
            const char *CTRL_BUF = mCtrlBuf; // Used for extra character check.

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int         LINE   = DATA[di].d_lineNum;
                const int         HOUR   = DATA[di].d_hour;
                const int         MINUTE = DATA[di].d_minute;
                const int         SECOND = DATA[di].d_second;
                const int         MSEC   = DATA[di].d_msec;
                const int         USEC   = DATA[di].d_usec;
                const char *const FMT    = DATA[di].d_fmt_p;

                Obj x;  const Obj& X = x;
                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);

                if (veryVerbose) cout << "\tEXPECTED FORMAT: " << FMT << endl;
                ostringstream out(bsl::string(CTRL_BUF, SIZE));
                X.streamOut(out);  out << ends;
                if (veryVerbose) {
                    cout << "\tACTUAL FORMAT:   " << out.str() << endl;
                }

                const int SZ = static_cast<int>(strlen(FMT)) + 1;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE,
                            XX == out.str()[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE, 0 == memcmp(out.str().c_str(),
                                              FMT,
                                              SZ));
                LOOP_ASSERT(LINE, 0 == memcmp(out.str().c_str() + SZ,
                                              CTRL_BUF + SZ,
                                              SIZE - SZ));
            }
        }

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Verify the basic accessors work as expected.
        //
        // Concerns:
        //: 1 Each accessor performs the appropriate arithmetic to convert
        //:   the internal total-milliseconds representation to the
        //:   four-parameter (h, m, s, ms) representation.
        //:
        //: 2 The 'getTime' accessor must respond appropriately to null-pointer
        //:   arguments, specifically not affecting other (non-null) arguments
        //:   to be loaded with appropriate values.
        //
        // Plan:
        //: 1 For each of a sequence of unique object values, verify that each
        //:   of the basic accessors returns the correct value.  (C-1)
        //:
        //: 2 For each of a sequence of unique object values, verify that the
        //:   'getTime' method with various null arguments produces the
        //:   expected behavior.  (C-2)
        //
        // Testing:
        //   void getTime(int *h, int *m, int *s, int *ms, int *us) const;
        //   int hour() const;
        //   int minute() const;
        //   int second() const;
        //   int millisecond() const;
        //   int microsecond() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BASIC ACCESSORS" << endl
                          << "=======================" << endl;

        if (verbose) {
            cout << "\nTesting 'getTime', 'hour()', 'minute()', 'second()', "
                 << "and 'millisecond()'." << endl;
        }
        {
            static const struct {
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
                int d_usec;
            } VALUES[] = {
                {  0,  0,  0,   0,   0 },
                {  0,  0,  0,   0, 999 },
                {  0,  0,  0, 999,   0 },
                {  0,  0, 59,   0,   0 },
                {  0, 59,  0,   0,   0 },
                { 23,  0,  0,   0,   0 },
                { 23, 22, 21, 209,   0 },
                { 23, 22, 21, 210,   0 },
                { 23, 22, 21, 210, 117 },
                { 24,  0,  0,   0,   0 },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;
                const int USEC   = VALUES[i].d_usec;
                int       h, m, s, ms, us;

                Obj x;  const Obj& X = x;
                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);
                X.getTime(&h, &m, &s, &ms, &us);

                if (veryVerbose) {
                    T_;  P_(HOUR);    P_(h);  P_(MINUTE);  P_(m);
                         P_(SECOND);  P_(s);  P_(MSEC);    P_(ms);
                         P_(USEC);    P(us);
                    T_;  P(X);
                }

                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());
                LOOP_ASSERT(i, USEC   == X.microsecond());

                LOOP_ASSERT(i, HOUR   == h);
                LOOP_ASSERT(i, MINUTE == m);
                LOOP_ASSERT(i, SECOND == s);
                LOOP_ASSERT(i, MSEC   == ms);
                LOOP_ASSERT(i, USEC   == us);
            }
        }

        if (verbose) cout << "\nTesting 'getTime' with null args." << endl;
        {
            static const struct {
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
                int d_usec;
            } VALUES[] = {
                {  0,  0,  0,   0,   0 },
                {  0,  0,  0,   0, 999 },
                {  0,  0,  0, 999,   0 },
                {  0,  0, 59,   0,   0 },
                {  0, 59,  0,   0,   0 },
                { 23,  0,  0,   0,   0 },
                { 23, 22, 21, 209,   0 },
                { 23, 22, 21, 210,   0 },
                { 23, 22, 21, 210, 117 },
                { 24,  0,  0,   0,   0 },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;
                const int USEC   = VALUES[i].d_usec;

                int h, m, s, ms, us;

                Obj x;  const Obj& X = x;

                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);
                X.getTime(&h, 0, 0, 0, 0);
                if (veryVerbose) { T_;  P_(HOUR);  P_(h);  P(X); }
                LOOP_ASSERT(i, HOUR   == h);

                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);
                X.getTime(&h, 0, 0, 0);
                if (veryVerbose) { T_;  P_(HOUR);  P_(h);  P(X); }
                LOOP_ASSERT(i, HOUR   == h);

                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);
                X.getTime(0, &m, 0, 0, 0);
                if (veryVerbose) { T_;  P_(MINUTE);  P_(m);  P(X); }
                LOOP_ASSERT(i, MINUTE == m);

                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);
                X.getTime(0, &m, 0, 0);
                if (veryVerbose) { T_;  P_(MINUTE);  P_(m);  P(X); }
                LOOP_ASSERT(i, MINUTE == m);

                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);
                X.getTime(0, 0, &s, 0, 0);
                if (veryVerbose) { T_;  P_(SECOND);  P_(s);  P(X); }
                LOOP_ASSERT(i, SECOND == s);

                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);
                X.getTime(0, 0, &s);
                if (veryVerbose) { T_;  P_(SECOND);  P_(s);  P(X); }
                LOOP_ASSERT(i, SECOND == s);

                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);
                X.getTime(0, 0, 0, &ms);
                if (veryVerbose) { T_;  P_(MSEC);  P_(ms);  P(X); }
                LOOP_ASSERT(i, MSEC   == ms);

                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);
                X.getTime(0, 0, 0, 0, &us);
                if (veryVerbose) { T_;  P_(USEC);  P_(us);  P(X); }
                LOOP_ASSERT(i, USEC   == us);

                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);
                X.getTime(0, 0, 0);
                if (veryVerbose) { T_;  P_(0);  P(X);  cout << endl; }
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg'
        //   Void for 'bdlt_time'.
        //
        // Testing:
        //   Obj& gg(Obj *object, const char *spec);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING GENERATOR FUNCTION 'gg'" << endl
                 << "===============================" << endl;
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP)
        //   Verify the primary manipulators work as expected.
        //
        // Concerns:
        //: 1 The separate time fields must be multiplied by the appropriate
        //:   factors to convert the four-parameter input representation to the
        //:   internal total-milliseconds representation.
        //:
        //: 2 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Verify the default constructor by testing the value of the
        //:   resulting object.
        //:
        //: 2 For a sequence of independent test values, use the default
        //:   constructor to create a default object and use the primary
        //:   manipulator to set its value.  Verify the value using the basic
        //:   accessors.  Note that the destructor is exercised on each
        //:   configuration as the object being tested leaves scope (thereby
        //:   enabling assertions of internal invariants).  (C-1)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   Time();
        //   ~Time();
        //   void setTime(int h, int m = 0, int s = 0, int ms = 0, int us = 0);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING PRIMARY MANIPULATORS (BOOTSTRAP)" << endl
                 << "========================================" << endl;
        }

        if (verbose) cout << "\nTesting default constructor." << endl;

        Obj x;  const Obj& X = x;

        ASSERT(24 == X.hour());          ASSERT(0 == X.minute());
        ASSERT( 0 == X.second());        ASSERT(0 == X.millisecond());
        ASSERT( 0 == X.microsecond());

        if (verbose) cout << "\nTesting 'setTime'." << endl;
        {
            static const struct {
                int d_hour;
                int d_minute;
                int d_second;
                int d_msec;
                int d_usec;
            } VALUES[] = {
                {  0,  0,  0,   0,   0 },
                {  0,  0,  0,   0, 999 },
                {  0,  0,  0, 999,   0 },
                {  0,  0, 59,   0,   0 },
                {  0, 59,  0,   0,   0 },
                { 23,  0,  0,   0,   0 },
                { 23, 22, 21, 209,   0 },
                { 23, 22, 21, 210,   0 },
                { 23, 22, 21, 210, 217 },
                { 24,  0,  0,   0,   0 },
            };

            const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                    / sizeof *VALUES);

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;
                const int USEC   = VALUES[i].d_usec;

                Obj x;  const Obj& X = x;
                x.setTime(HOUR, MINUTE, SECOND, MSEC, USEC);
                if (veryVerbose) {
                    T_;
                    P_(HOUR);
                    P_(MINUTE);
                    P_(SECOND);
                    P_(MSEC);
                    P_(USEC);
                    P(X);
                }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());
                LOOP_ASSERT(i, USEC   == X.microsecond());

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                if (veryVerbose) {
                    T_; P_(HOUR); P_(MINUTE); P_(SECOND); P_(MSEC); P(X);
                }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());
                LOOP_ASSERT(i, 0      == X.microsecond());

                x.setTime(HOUR, MINUTE, SECOND);
                if (veryVerbose) {
                    T_; P_(HOUR); P_(MINUTE); P_(SECOND); P(X);
                }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, 0      == X.millisecond());
                LOOP_ASSERT(i, 0      == X.microsecond());

                x.setTime(HOUR, MINUTE);
                if (veryVerbose) { T_;  P_(HOUR);  P_(MINUTE);  P(X); }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, 0      == X.second());
                LOOP_ASSERT(i, 0      == X.millisecond());
                LOOP_ASSERT(i, 0      == X.microsecond());

                x.setTime(HOUR);
                if (veryVerbose) { T_;  P_(HOUR);  P(X); }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, 0      == X.minute());
                LOOP_ASSERT(i, 0      == X.second());
                LOOP_ASSERT(i, 0      == X.millisecond());
                LOOP_ASSERT(i, 0      == X.microsecond());
            }
        }

        if (verbose)
            cout << "\nNegative Testing." << endl;
        {
            bsls::AssertTestHandlerGuard hG;

            Obj mX;

            ASSERT_SAFE_PASS(mX.setTime(24, 0, 0, 0, 0));
            ASSERT_FAIL(mX.setTime(24, 0, 0, 0, 1));
            ASSERT_FAIL(mX.setTime(24, 0, 0, 1, 0));
            ASSERT_FAIL(mX.setTime(24, 0, 1, 0, 0));
            ASSERT_FAIL(mX.setTime(24, 1, 0, 0, 0));

            ASSERT_FAIL(mX.setTime(-1, 0, 0, 0));
            ASSERT_SAFE_PASS(mX.setTime( 0, 0, 0, 0));
            ASSERT_SAFE_PASS(mX.setTime(23, 0, 0, 0));
            ASSERT_FAIL(mX.setTime(25, 0, 0, 0));

            ASSERT_FAIL(mX.setTime(0, -1, 0, 0));
            ASSERT_SAFE_PASS(mX.setTime(0,  0, 0, 0));
            ASSERT_SAFE_PASS(mX.setTime(0, 59, 0, 0));
            ASSERT_FAIL(mX.setTime(0, 60, 0, 0));

            ASSERT_FAIL(mX.setTime(0, 0, -1, 0));
            ASSERT_SAFE_PASS(mX.setTime(0, 0,  0, 0));
            ASSERT_SAFE_PASS(mX.setTime(0, 0, 59, 0));
            ASSERT_FAIL(mX.setTime(0, 0, 60, 0));

            ASSERT_FAIL(mX.setTime(0, 0, 0,   -1));
            ASSERT_SAFE_PASS(mX.setTime(0, 0, 0,    0));
            ASSERT_SAFE_PASS(mX.setTime(0, 0, 0,  999));
            ASSERT_FAIL(mX.setTime(0, 0, 0, 1000));

            ASSERT_FAIL(mX.setTime(0, 0, 0, 0,   -1));
            ASSERT_SAFE_PASS(mX.setTime(0, 0, 0, 0,    0));
            ASSERT_SAFE_PASS(mX.setTime(0, 0, 0, 0,  999));
            ASSERT_FAIL(mX.setTime(0, 0, 0, 0, 1000));
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Create four test objects by using the default, initializing, and
        //:   copy constructors.
        //:
        //: 2 Exercise the basic value-semantic methods and the equality
        //:   operators using these test objects.
        //:
        //: 3 Invoke the primary manipulator, copy constructor, and assignment
        //:   operator without and with aliasing.
        //:
        //: 4 Use the basic accessors to verify the expected results.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const int HA = 1, MA = 2, SA = 3, mA = 4;  // h, m, s, ms values for VA
        const int HB = 5, MB = 6, SB = 7, mB = 8;  // h, m, s, ms values for VB
        const int HC = 9, MC = 9, SC = 9, mC = 9;  // h, m, s, ms values for VC

        int h, m, s, ms;                   // reusable variables for 'get' call

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(HA, MA, SA, mA);  const Obj& X1 = mX1;
        if (verbose) { T_;  P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X1.getTime(&h, &m, &s, &ms);
        ASSERT(HA == h);  ASSERT(MA == m);  ASSERT(SA == s);  ASSERT(mA == ms);
        ASSERT(HA == X1.hour());            ASSERT(MA == X1.minute());
        ASSERT(SA == X1.second());          ASSERT(mA == X1.millisecond());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { T_;  P(X2); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X2.getTime(&h, &m, &s, &ms);
        ASSERT(HA == h);  ASSERT(MA == m);  ASSERT(SA == s);  ASSERT(mA == ms);
        ASSERT(HA == X2.hour());            ASSERT(MA == X2.minute());
        ASSERT(SA == X2.second());          ASSERT(mA == X2.millisecond());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setTime(HB, MB, SB, mB);
        if (verbose) { T_;  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X1.getTime(&h, &m, &s, &ms);
        ASSERT(HB == h);  ASSERT(MB == m);  ASSERT(SB == s);  ASSERT(mB == ms);
        ASSERT(HB == X1.hour());            ASSERT(MB == X1.minute());
        ASSERT(SB == X1.second());          ASSERT(mB == X1.millisecond());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { T_;  P(X3); }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X3.getTime(&h, &m, &s, &ms);
        ASSERT(24 == h);  ASSERT(0 == m);  ASSERT(0 == s);  ASSERT(0 == ms);

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (verbose) { T_;  P(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X4.getTime(&h, &m, &s, &ms);
        ASSERT(24 == h);  ASSERT(0 == m);  ASSERT(0 == s);  ASSERT(0 == ms);

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setTime(HC, MC, SC, mC);
        if (verbose) { T_;  P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X3.getTime(&h, &m, &s, &ms);
        ASSERT(HC == h);  ASSERT(MC == m);  ASSERT(SC == s);  ASSERT(mC == ms);
        ASSERT(HC == X3.hour());            ASSERT(MC == X3.minute());
        ASSERT(SC == X3.second());          ASSERT(mC == X3.millisecond());

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
        if (verbose) { T_;  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X2.getTime(&h, &m, &s, &ms);
        ASSERT(HB == h);  ASSERT(MB == m);  ASSERT(SB == s);  ASSERT(mB == ms);
        ASSERT(HB == X2.hour());            ASSERT(MB == X2.minute());
        ASSERT(SB == X2.second());          ASSERT(mB == X2.millisecond());

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
        if (verbose) { T_;  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X2.getTime(&h, &m, &s, &ms);
        ASSERT(HC == h);  ASSERT(MC == m);  ASSERT(SC == s);  ASSERT(mC == ms);
        ASSERT(HC == X2.hour());            ASSERT(MC == X2.minute());
        ASSERT(SC == X2.second());          ASSERT(mC == X2.millisecond());

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
        if (verbose) { T_;  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        h = 0;  m = 0;  s = 0;  ms = 0;
        X1.getTime(&h, &m, &s, &ms);
        ASSERT(HB == h);  ASSERT(MB == m);  ASSERT(SB == s);  ASSERT(mB == ms);
        ASSERT(HB == X1.hour());            ASSERT(MB == X1.minute());
        ASSERT(SB == X1.second());          ASSERT(mB == X1.millisecond());

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
