// bdlt_datetimetz.t.cpp                                              -*-C++-*-
#include <bdlt_datetimetz.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_time.h>

#include <bslim_testutil.h>

#include <bslma_testallocator.h>

#include <bslmf_istriviallycopyable.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

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
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
// The component under test implements a single value-semantic class,
// 'bdlt::DatetimeTz', that represents a datetime value with a local time
// offset.
//
// Primary Manipulators:
//: o VALUE CONSTRUCTOR
//
// Basic Accessors:
//: o 'localDatetime'
//: o 'offset'
//
// This particular class provides a value constructor capable of creating an
// object in any state relevant for thorough testing.  The value constructor
// serves as our primary manipulator and also obviates the primitive generator
// function, 'gg', normally used for this purpose.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [12] static bool isValid(const Datetime& localDatetime, int offset);
// [10] static int maxSupportedBdexVersion(int versionSelector);
//
// CREATORS
// [11] DatetimeTz();
// [ 2] DatetimeTz(const Datetime& localDatetime, int offset);
// [ 7] DatetimeTz(const DatetimeTz& original);
// [ 2] ~DatetimeTz();
//
// MANIPULATORS
// [ 9] DatetimeTz& operator=(const DatetimeTz& rhs);
// [11] void setDatetimeTz(const Datetime& localDatetime, int offset);
// [12] int setDatetimeTzIfValid(const Datetime& localDT, int offset);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [14] DateTz dateTz() const;
// [ 4] Datetime localDatetime() const;
// [13] Datetime utcDatetime() const;
// [ 4] int offset() const;
// [14] TimeTz timeTz() const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 5] bsl::ostream& print(bsl::ostream& stream, int l, int spl) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const DatetimeTz& lhs, const DatetimeTz& rhs);
// [ 6] bool operator!=(const DatetimeTz& lhs, const DatetimeTz& rhs);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const DatetimeTz&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [15] USAGE EXAMPLE
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

typedef bdlt::DatetimeTz    Obj;
typedef bslx::TestInStream  In;
typedef bslx::TestOutStream Out;

#define VERSION_SELECTOR 20140601

//=============================================================================
//              Classes, functions, etc., needed for Usage Example
//-----------------------------------------------------------------------------
// Let us suppose that we are implementing a delivery estimation system for a
// shipping company.  The system provides estimated delivery dates and times of
// client shipments.  This information is provided in the local time zone and
// is represented as a 'bdlt::DatetimeTz' object.  Below is the definition for
// a struct that returns the estimated delivery date.
//..
                         // =====================
                         // struct DeliverySystem
                         // =====================

    struct DeliverySystem {
        // This struct provides a function that returns the estimated delivery
        // date and time for a particular shipment.

        // PRIVATE CLASS METHODS
        static bdlt::Datetime getCurrentUTCDatetime();
            // Return the current UTC date and time.

      public:
        // TYPES
        enum City {
            // This enumeration provides an identifier for the various cities.

            e_CHICAGO = 0,
            e_DUBAI,
            e_NEW_YORK,
            e_LONDON,
            e_LOS_ANGELES
        };

        // CLASS METHODS
        static bdlt::DatetimeTz getEstimatedDeliveryDatetime(City city);
            // Return the estimated delivery date and time, in local time, for
            // a shipment being sent to the specified 'city'.
    };
//..
// All the relevant data used for delivery estimation is stored in a lookup
// table as shown below:
//..
    const int k_MINUTES_PER_HOUR = 60;

    static const struct {
        int d_offset;         // time zone offset from UTC (in minutes)
        int d_deliveryTime;   // delivery time (in minutes)
    } DATA[] = {
     //    Offset                   DeliveryTime
     //    =======================  =======================
     {     -6 * k_MINUTES_PER_HOUR, 10 * k_MINUTES_PER_HOUR  },  // Chicago
     {      3 * k_MINUTES_PER_HOUR, 72 * k_MINUTES_PER_HOUR  },  // Dubai
     {     -5 * k_MINUTES_PER_HOUR,      k_MINUTES_PER_HOUR  },  // New York
     {          k_MINUTES_PER_HOUR, 36 * k_MINUTES_PER_HOUR  },  // London
     {     -8 * k_MINUTES_PER_HOUR, 24 * k_MINUTES_PER_HOUR  },  // Los Angeles
    };
//..
// And here are the function definitions:
//..
                         // ---------------------
                         // struct DeliverySystem
                         // ---------------------

    // PRIVATE CLASS METHODS
    bdlt::Datetime DeliverySystem::getCurrentUTCDatetime()
    {
        // Return a fixed datetime so that output is known a priori.
        return bdlt::Datetime(2014, 10, 17, 14, 48, 56);
    }

    // CLASS METHODS
    bdlt::DatetimeTz DeliverySystem::getEstimatedDeliveryDatetime(City city)
    {
        bdlt::Datetime localDatetime(getCurrentUTCDatetime());
        localDatetime.addMinutes(DATA[city].d_offset
                               + DATA[city].d_deliveryTime);
        return bdlt::DatetimeTz(localDatetime, DATA[city].d_offset);
    }
//..

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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.
    // CONCERN: In case 5 only does memory come from the default allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:
      case 15: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'bdlt::DatetimeTz' Usage
///- - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create and use a 'bdlt::DatetimeTz' object.
//
// First, create an object 'dt1' having the default value, and then verify that
// it contains an offset of 0, implying that the object represents a date and
// time in the UTC time zone, and the value of the datetime is the same as that
// of a default constructed 'bdlt::Datetime' object:
//..
    bdlt::DatetimeTz dt1;
    ASSERT(0                == dt1.offset());
    ASSERT(bdlt::Datetime() == dt1.localDatetime());
//..
// Then, set 'dt1' to the value 12:00 noon (12:00:00.000) on 12/31/2005 in the
// EST time zone (UTC-5):
//..
    bdlt::Datetime datetime1(2005, 12, 31, 12, 0, 0, 0);
    bdlt::Datetime datetime2(datetime1);
    int            offset1 = -5 * 60;

    dt1.setDatetimeTz(datetime1, offset1);
    ASSERT(offset1             == dt1.offset());
    ASSERT(dt1.localDatetime() != dt1.utcDatetime());
    ASSERT(datetime1           == dt1.localDatetime());
    ASSERT(datetime2           != dt1.utcDatetime());

    datetime2.addMinutes(-offset1);
    ASSERT(datetime2 == dt1.utcDatetime());
//..
// Next, create 'dt2' as a copy of 'dt1':
//..
    bdlt::DatetimeTz dt2(dt1);
    ASSERT(offset1   == dt2.offset());
    ASSERT(datetime1 == dt2.localDatetime());
    ASSERT(datetime2 == dt2.utcDatetime());
//..
// Now, create a third object, 'dt3', representing the time 10:33:25.000 on
// 01/01/2001 in the PST time zone (UTC-8):
//..
    bdlt::Datetime datetime3(2001, 1, 1, 10, 33, 25, 0);
    bdlt::Datetime datetime4(datetime3);
    int            offset2 = -8 * 60;

    bdlt::DatetimeTz dt3(datetime3, offset2);
    ASSERT(offset2             == dt3.offset());
    ASSERT(dt3.localDatetime() != dt3.utcDatetime());
    ASSERT(datetime3           == dt3.localDatetime());
    ASSERT(datetime4           != dt3.utcDatetime());

    datetime4.addMinutes(-offset2);
    ASSERT(datetime4 == dt3.utcDatetime());
//..
// Finally, stream the values of 'dt1', 'dt2', and 'dt3' to 'stdout':
//..
if (verbose)
    bsl::cout << dt1 << bsl::endl
              << dt2 << bsl::endl
              << dt3 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  31DEC2005_12:00:00.000-0500
//  31DEC2005_12:00:00.000-0500
//  01JAN2001_10:33:25.000-0800
//..
///Example 2: Delivery Estimation System
///- - - - - - - - - - - - - - - - - - -
// When we print out the delivery times:
//..
if (verbose) {
    bsl::cout << "Estimated Delivery Time in Chicago:     "
              << DeliverySystem::getEstimatedDeliveryDatetime(
                                                     DeliverySystem::e_CHICAGO)
              << bsl::endl;
    bsl::cout << "Estimated Delivery Time in Dubai:       "
              << DeliverySystem::getEstimatedDeliveryDatetime(
                                                       DeliverySystem::e_DUBAI)
              << bsl::endl;
    bsl::cout << "Estimated Delivery Time in New York:    "
              << DeliverySystem::getEstimatedDeliveryDatetime(
                                                    DeliverySystem::e_NEW_YORK)
              << bsl::endl;
    bsl::cout << "Estimated Delivery Time in London:      "
              << DeliverySystem::getEstimatedDeliveryDatetime(
                                                      DeliverySystem::e_LONDON)
              << bsl::endl;
    bsl::cout << "Estimated Delivery Time in Los Angeles: "
              << DeliverySystem::getEstimatedDeliveryDatetime(
                                                 DeliverySystem::e_LOS_ANGELES)
              << bsl::endl;
}
//..
// We get the following results:
//..
//  Estimated Delivery Time in Chicago:     17OCT2014_18:48:56.000-0600
//  Estimated Delivery Time in Dubai:       20OCT2014_17:48:56.000+0300
//  Estimated Delivery Time in New York:    17OCT2014_10:48:56.000-0500
//  Estimated Delivery Time in London:      19OCT2014_03:48:56.000+0100
//  Estimated Delivery Time in Los Angeles: 18OCT2014_06:48:56.000-0800
//..
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'dateTz' AND 'timeTz' METHODS
        //
        // Concerns:
        //: 1 'dateTz' and 'timeTz' produce the correct values.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of datetimes and
        //:   their associated offsets.  Verify the methods produce the
        //:   expected values by comparing the returned value to ones directly
        //:   constructed.  (C-1)
        //
        // Testing:
        //   DateTz dateTz() const;
        //   TimeTz timeTz() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'dateTz' AND 'timeTz' METHODS" << endl
                          << "=====================================" << endl;

        if (verbose) cout << "\nDirect test of 'dateTz' and 'timeTz'." << endl;
        {
            static const struct {
                int d_year;    // year of datetime
                int d_month;   // month of datetime
                int d_day;     // day of month of datetime
                int d_hour;    // hour of datetime
                int d_min;     // minute of datetime
                int d_sec;     // second of datetime
                int d_msec;    // millisecond of datetime
                int d_offset;  // timezone offset
            } DATA[] = {
            //    YEAR  MO  DAY  HOUR  MIN  SEC  MSEC   OFFSET
            //    ----  --  ---  ----  ---  ---  ----  -------
                {    1,  1,   1,   24,   0,   0,    0,       0 },
                {   10,  4,   5,    1,   0,   0,    0,       1 },
                {  100,  6,   7,    7,   1,   0,    0,   -1439 },
                { 1000,  8,   9,   22,   0,   1,    0,    1439 },
                { 2000,  2,  29,   12,   0,   0,    1,      -1 },
                { 2002,  7,   4,   18,  59,   0,    0,    1380 },
                { 2003,  8,   5,    9,   0,  59,    0,   -1380 },
                { 2004,  9,   3,   11,   0,   0,  999,    5*60 },
                { 9999, 12,  31,   18,  17,  42,  103,   -5*60 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int HOUR   = DATA[i].d_hour;
                const int MIN    = DATA[i].d_min;
                const int SECS   = DATA[i].d_sec;
                const int MSEC   = DATA[i].d_msec;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(YEAR) P_(MONTH) P_(DAY)
                    P_(HOUR) P_(MIN) P_(SECS) P_(MSEC)
                    P(OFFSET)
                }

                const bdlt::Datetime TEMP_DATETIME(YEAR,
                                                   MONTH,
                                                   DAY,
                                                   HOUR,
                                                   MIN,
                                                   SECS,
                                                   MSEC);

                const Obj X(TEMP_DATETIME, OFFSET);

                const bdlt::DateTz DATETZ = bdlt::DateTz(TEMP_DATETIME.date(),
                                                         OFFSET);

                const bdlt::TimeTz TIMETZ = bdlt::TimeTz(TEMP_DATETIME.time(),
                                                         OFFSET);

                if (veryVeryVerbose) { T_ T_ P(X) }

                LOOP_ASSERT(i, DATETZ == X.dateTz());
                LOOP_ASSERT(i, TIMETZ == X.timeTz());
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'utcDatetime' METHOD
        //
        // Concerns:
        //: 1 'utcDatetime' computes the correct UTC datetime of the local
        //:   datetime accordingly to its timezone.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of datetimes and
        //:   their associated offsets. Verify that 'utcDatetime' returns the
        //:   expected value as per the invariant:
        //:   'utcDatetime() == localDatetime() - offset()'.  (C-1)
        //
        // Testing:
        //   Datetime utcDatetime() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'utcDatetime' METHOD" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nDirect test of 'utcDatetime'." << endl;
        {
            static const struct {
                int d_year;    // year of datetime
                int d_month;   // month of datetime
                int d_day;     // day of month of datetime
                int d_hour;    // hour of datetime
                int d_min;     // minute of datetime
                int d_sec;     // second of datetime
                int d_msec;    // millisecond of datetime
                int d_offset;  // timezone offset
            } DATA[] = {
            //    YEAR  MO  DAY  HOUR  MIN  SEC  MSEC   OFFSET
            //    ----  --  ---  ----  ---  ---  ----  -------
                {    1,  1,   1,   24,   0,   0,    0,       0 },
                {   10,  4,   5,    1,   0,   0,    0,       1 },
                {  100,  6,   7,    7,   1,   0,    0,   -1439 },
                { 1000,  8,   9,   22,   0,   1,    0,    1439 },
                { 2000,  2,  29,   12,   0,   0,    1,      -1 },
                { 2002,  7,   4,   18,  59,   0,    0,    1380 },
                { 2003,  8,   5,    9,   0,  59,    0,   -1380 },
                { 2004,  9,   3,   11,   0,   0,  999,    5*60 },
                { 9999, 12,  31,   18,  17,  42,  103,   -5*60 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int HOUR   = DATA[i].d_hour;
                const int MIN    = DATA[i].d_min;
                const int SECS   = DATA[i].d_sec;
                const int MSEC   = DATA[i].d_msec;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(YEAR) P_(MONTH) P_(DAY)
                    P_(HOUR) P_(MIN) P_(SECS) P_(MSEC)
                    P(OFFSET)
                }

                const bdlt::Datetime LOCAL_DATETIME(YEAR,
                                                    MONTH,
                                                    DAY,
                                                    HOUR,
                                                    MIN,
                                                    SECS,
                                                    MSEC);

                bdlt::Datetime        utc_datetime(LOCAL_DATETIME);
                const bdlt::Datetime& UTC_DATETIME = utc_datetime;
                if (OFFSET) utc_datetime.addMinutes(-OFFSET);

                const Obj X(LOCAL_DATETIME, OFFSET);

                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, UTC_DATETIME == X.utcDatetime());

            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'isValid' AND 'setDatetimeTzIfValid' METHODS
        //
        // Concerns:
        //: 1 'isValid' correctly determines whether a datetime and an
        //:   associated offset are valid attributes of a 'DatetimeTz' object.
        //:
        //: 2 'setDatetimeTzIfValid' sets the date and offset of a 'DatetimeTz'
        //:   object to the specified values only if they are valid.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of valid and
        //:   invalid object values. For each value verify that 'isValid'
        //:   returns the correct result and 'setDatetimeTzIfValid' behaves
        //:   correctly.  (C-1..2)
        //
        // Testing:
        //   static bool isValid(const Datetime& localDatetime, int offset);
        //   int setDatetimeTzIfValid(const Datetime& localDT, int offset);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'isValid' AND 'setDatetimeTzIfValid' METHODS"
                 << endl
                 << "===================================================="
                 << endl;
        }

        if (verbose) {
            cout << "\nDirect test of 'isValid' & 'setDatetimeTzIfValid'."
                 << endl;
        }
        {
            static const struct {
                int d_lineNum;      // source line number
                int d_year;         // year of datetime
                int d_month;        // month of datetime
                int d_day;          // day of month of datetime
                int d_hour;         // hour of datetime
                int d_minute;       // minute of datetime
                int d_second;       // second of datetime
                int d_millisecond;  // millisecond of datetime
                int d_offset;       // timezone offset
                int d_isValid;      // is the implied value valid
            } DATA[] = {
            //    LINE  YEAR  MO  DAY  HOUR  MIN  SEC  MSEC   OFFSET  VALID
            //    ----  ----  --  ---  ----  ---  ---  ----  -------  -----
                {   L_,    1,  1,   1,   24,   0,   0,    0,       0,     1 },
                {   L_,    2,  1,   1,    3,   0,   0,    0,       0,     1 },
                {   L_,    2,  1,   1,    0,   3,   0,    0,       0,     1 },
                {   L_,    2,  1,   1,    0,   0,   3,    0,       0,     1 },
                {   L_,    2,  1,   1,    0,   0,   0,    3,       0,     1 },
                {   L_,    2,  1,   1,    0,   0,   0,    0,       3,     1 },
                {   L_,   10,  4,   5,    1,   0,   0,    0,       1,     1 },
                {   L_,  100,  6,   7,    7,   1,   0,    0,   -1439,     1 },
                {   L_, 1000,  8,   9,   22,   0,   1,    0,    1439,     1 },
                {   L_, 2000,  2,  29,   12,   0,   0,    1,      -1,     1 },
                {   L_, 2000,  2,  29,   12,   0,   0,    1,       0,     1 },
                {   L_, 2000,  2,  29,   12,   0,   0,    1,       1,     1 },
                {   L_, 2002,  7,   4,   18,  59,   0,    0,    1380,     1 },
                {   L_, 2003,  8,   5,    9,   0,  59,    0,   -1380,     1 },
                {   L_, 2004,  9,   3,   11,   0,   0,  999,    5*60,     1 },
                {   L_, 9999, 12,  31,   18,  17,  42,  103,   -5*60,     1 },

                {   L_,    1,  1,   1,   24,   0,   0,    0,      -1,     0 },
                {   L_,    1,  1,   1,   24,   0,   0,    0,       1,     0 },
                {   L_,  100,  6,   7,    7,   1,   0,    0,   -1440,     0 },
                {   L_, 1000,  8,   9,   22,   0,   1,    0,    1440,     0 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int di = 0; di < NUM_DATA; ++di) {
                const int LINE        = DATA[di].d_lineNum;
                const int YEAR        = DATA[di].d_year;
                const int MONTH       = DATA[di].d_month;
                const int DAY         = DATA[di].d_day;
                const int HOUR        = DATA[di].d_hour;
                const int MINUTE      = DATA[di].d_minute;
                const int SECOND      = DATA[di].d_second;
                const int MILLISECOND = DATA[di].d_millisecond;
                const int OFFSET      = DATA[di].d_offset;
                const int IS_VALID    = DATA[di].d_isValid;

                if (veryVerbose) {
                    T_ P_(YEAR) P_(MONTH) P_(DAY)
                    P_(HOUR) P_(MINUTE) P_(SECOND) P_(MILLISECOND)
                    P_(OFFSET) P(IS_VALID)
                }

                const bdlt::Datetime SRC_DATETIME(YEAR,
                                                  MONTH,
                                                  DAY,
                                                  HOUR,
                                                  MINUTE,
                                                  SECOND,
                                                  MILLISECOND);
                LOOP_ASSERT(LINE,
                  IS_VALID == bdlt::DatetimeTz::isValid(SRC_DATETIME, OFFSET));

                const int            OFFSET2 = 5 * 60;
                const bdlt::Datetime DEST_DATETIME(2003, 3, 18);
                bdlt::DatetimeTz     t(DEST_DATETIME, OFFSET2);

                int sts = t.setDatetimeTzIfValid(SRC_DATETIME, OFFSET);
                LOOP_ASSERT(LINE, IS_VALID == !sts);

                if (IS_VALID) {
                    LOOP_ASSERT(LINE, SRC_DATETIME == t.localDatetime());
                    LOOP_ASSERT(LINE, OFFSET       == t.offset());
                }
                else {
                    // verify t was unchanged.

                    LOOP_ASSERT(LINE, DEST_DATETIME == t.localDatetime());
                    LOOP_ASSERT(LINE,  OFFSET2      == t.offset());
                }
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING DEFAULT CTOR AND 'setDatetimeTz' METHOD
        //
        // Concerns:
        //: 1 An object created with the default constructor has the
        //:   contractually specified default value.
        //:
        //: 2 The primary manipulator 'setDatetimeTz' sets the date and local
        //:   offset of the object to the specified values.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Default construct an object and verify that the object has the
        //:   expected value.  (C-1)
        //:
        //: 2 Using the table-driven technique, specify a set 'S' of datetimes
        //:   and offsets as '(y, m, d, h, m, s, ms, o)' tuples having widely
        //:   varying values.  For each '(y, m, d, h, m, s, ms, o)' in 'S',
        //:   default construct a 'DatetimeTz' object 'X', use 'setDatetimeTz'
        //:   to modify 'X', and verify the value of 'X' by comparing it to a
        //:   value constructed object with the same parameters.  (C-2)
        //:
        //: 3 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-3)
        //
        // Testing:
        //   DatetimeTz();
        //   void setDatetimeTz(const Datetime& localDatetime, int offset);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING DEFAULT CTOR AND 'setDatetimeTz' METHOD" << endl
                 << "===============================================" << endl;
        }

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            Obj mX;  const Obj& X = mX;

            ASSERT(bdlt::Datetime() == X.localDatetime());
            ASSERT(               0 == X.offset());
        }

        if (verbose) cout << "\nTesting 'setDatetimeTz'." << endl;
        {
            static const struct {
                int d_year;    // year of datetime
                int d_month;   // month of datetime
                int d_day;     // day of month of datetime
                int d_hour;    // hour of datetime
                int d_min;     // minute of datetime
                int d_sec;     // second of datetime
                int d_msec;    // millisecond of datetime
                int d_offset;  // timezone offset
            } DATA[] = {
            //    YEAR  MO  DAY  HOUR  MIN  SEC  MSEC   OFFSET
            //    ----  --  ---  ----  ---  ---  ----  -------
                {    1,  1,   1,   24,   0,   0,    0,       0 },
                {   10,  4,   5,    1,   0,   0,    0,       1 },
                {  100,  6,   7,    7,   1,   0,    0,   -1439 },
                { 1000,  8,   9,   22,   0,   1,    0,    1439 },
                { 2000,  2,  29,   12,   0,   0,    1,      -1 },
                { 2002,  7,   4,   18,  59,   0,    0,    1380 },
                { 2003,  8,   5,    9,   0,  59,    0,   -1380 },
                { 2004,  9,   3,   11,   0,   0,  999,    5*60 },
                { 9999, 12,  31,   18,  17,  42,  103,   -5*60 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int HOUR   = DATA[i].d_hour;
                const int MIN    = DATA[i].d_min;
                const int SECS   = DATA[i].d_sec;
                const int MSEC   = DATA[i].d_msec;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(YEAR) P_(MONTH) P_(DAY)
                    P_(HOUR) P_(MIN) P_(SECS) P_(MSEC)
                    P(OFFSET)
                }

                const bdlt::Datetime TEMP_DATETIME(YEAR,
                                                   MONTH,
                                                   DAY,
                                                   HOUR,
                                                   MIN,
                                                   SECS,
                                                   MSEC);

                Obj mX;  const Obj& X = mX;
                mX.setDatetimeTz(TEMP_DATETIME, OFFSET);

                const Obj Y(TEMP_DATETIME, OFFSET);

                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, Y == X);
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;

            ASSERT_SAFE_FAIL(mX.setDatetimeTz(bdlt::Datetime(), -1));
            ASSERT_SAFE_PASS(mX.setDatetimeTz(bdlt::Datetime(),  0));
            ASSERT_SAFE_FAIL(mX.setDatetimeTz(bdlt::Datetime(),  1));

            ASSERT_SAFE_FAIL(mX.setDatetimeTz(bdlt::Datetime(1, 1, 1, 1),
                                              1440));
            ASSERT_SAFE_PASS(mX.setDatetimeTz(bdlt::Datetime(1, 1, 1, 1),
                                              1439));
            ASSERT_SAFE_FAIL(mX.setDatetimeTz(bdlt::Datetime(1, 1, 1, 1),
                                              -1440));
            ASSERT_SAFE_PASS(mX.setDatetimeTz(bdlt::Datetime(1, 1, 1, 1),
                                              -1439));
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
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BDEX STREAMING" << endl
                          << "======================" << endl;

        // Allocator to use instead of the default allocator.
        bslma::TestAllocator allocator("bslx", veryVeryVeryVerbose);

        // Scalar object values used in various stream tests.
        const bdlt::Datetime A(   1,  1,  1, 24,  0,  0,   0);
        const bdlt::Datetime B(   1,  1,  1,  0,  0,  0,   0);
        const bdlt::Datetime C(   3,  4,  7,  9, 12, 24, 102);
        const bdlt::Datetime D(2012,  4,  7,  9, 20, 30, 206);
        const bdlt::Datetime E(2014,  6, 14, 17, 34, 52, 503);
        const bdlt::Datetime F(2014, 10, 22, 23, 56, 57, 702);
        const bdlt::Datetime G(9999, 12, 31, 18, 59, 59, 999);

        const Obj VA(A,     0);
        const Obj VB(B,     1);
        const Obj VC(C,    -1);
        const Obj VD(D,   203);
        const Obj VE(E,  -507);
        const Obj VF(F,  1000);
        const Obj VG(G, -1100);

        // Array object used in various stream tests.
        const Obj VALUES[]   = { VA, VB, VC, VD, VE, VF, VG };
        const int NUM_VALUES = static_cast<int>(sizeof VALUES
                                                / sizeof *VALUES);

        if (verbose) {
            cout << "\nTesting 'maxSupportedBdexVersion'." << endl;
        }
        {
            ASSERT(1 == Obj::maxSupportedBdexVersion(0));
            ASSERT(1 == Obj::maxSupportedBdexVersion(VERSION_SELECTOR));

            using bslx::VersionFunctions::maxSupportedBdexVersion;

            ASSERT(1 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                0));
            ASSERT(1 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                VERSION_SELECTOR));
        }

        const int VERSION = Obj::maxSupportedBdexVersion(0);

        if (verbose) {
            cout << "\nDirect initial trial of 'bdexStreamOut' and (valid) "
                 << "'bdexStreamIn' functionality." << endl;
        }
        {
            const Obj X(VC);
            Out       out(VERSION_SELECTOR, &allocator);

            Out& rvOut = X.bdexStreamOut(out, VERSION);
            ASSERT(&out == &rvOut);

            const char *const OD  = out.data();
            const int         LOD = out.length();

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

        // We will use the stream free functions provided by 'bslx', as opposed
        // to the class member functions, since the 'bslx' implementation gives
        // priority to the free function implementations; we want to test what
        // will be used.  Furthermore, toward making this test case more
        // reusable in other components, from here on we generally use the
        // 'bdexStreamIn' and 'bdexStreamOut' free functions that are defined
        // in the 'bslx' package rather than call the like-named member
        // functions directly.

        if (verbose) {
            cout << "\nThorough test using stream free functions."
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
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

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
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                LOOP_ASSERT(i, in);
                LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

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

            const char *const OD  = out.data();
            const int         LOD = out.length();
            ASSERT(0 < LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);
                in.invalidate();
                LOOP_ASSERT(i, !in);
                LOOP_ASSERT(i, !in.isEmpty());

                // Ensure that reading from a non-empty, invalid input stream
                // leaves the stream invalid and the target object unchanged.

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
            const int         LOD1 = out.length();

            Out& rvOut2 = bdexStreamOut(out, X2, VERSION);
            ASSERT(&out == &rvOut2);
            const int         LOD2 = out.length();

            Out& rvOut3 = bdexStreamOut(out, X3, VERSION);
            ASSERT(&out == &rvOut3);
            const int         LOD3 = out.length();
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
                        LOOP_ASSERT(i, &in == &rvIn1);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X1 == T1);
                        In& rvIn2 = bdexStreamIn(in, mT2, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn2);
                        LOOP_ASSERT(i,  in);
                        LOOP_ASSERT(i, X2 == T2);
                        In& rvIn3 = bdexStreamIn(in, mT3, VERSION);
                        LOOP_ASSERT(i, &in == &rvIn3);
                        LOOP_ASSERT(i, !in);
                        if (LOD2 <= i) LOOP_ASSERT(i, W3 == T3);
                    }

                    // Verify the objects are in a valid state.

                    LOOP_ASSERT(i, (   -1440 < T1.offset()
                                    &&  1440 > T1.offset()
                                    && (   bdlt::Time()
                                                   != T1.localDatetime().time()
                                        || 0 == T1.offset())));

                    LOOP_ASSERT(i, (   -1440 < T2.offset()
                                    &&  1440 > T2.offset()
                                    && (   bdlt::Time()
                                                   != T2.localDatetime().time()
                                        || 0 == T2.offset())));

                    LOOP_ASSERT(i, (   -1440 < T3.offset()
                                    &&  1440 > T3.offset()
                                    && (   bdlt::Time()
                                                   != T3.localDatetime().time()
                                        || 0 == T3.offset())));

                } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
            }
        }

        if (verbose) {
            cout << "\tOn corrupted data." << endl;
        }

        const Obj W;                                  // default value
        const Obj X(bdlt::Datetime(1, 1, 1, 0), -3);  // original (control)
        const Obj Y(bdlt::Datetime(1, 1, 1, 0),  1);  // new (streamed-out)

        // Verify the three objects are distinct.
        ASSERT(W != X);
        ASSERT(W != Y);
        ASSERT(X != Y);

        const int SERIAL_Y = 1;       // internal rep. of 'Y.offset()'

        if (verbose) {
            cout << "\t\tGood stream (for control)." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Datetime(1, 1, 1, 0), VERSION);
            out.putInt32(SERIAL_Y);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, VERSION);
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

            bdexStreamOut(out, bdlt::Datetime(1, 1, 1, 0), VERSION);
            out.putInt32(SERIAL_Y);

            const char *const OD  = out.data();
            const int         LOD = out.length();

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
            const char version = 2 ; // too large (current version is 1)

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Datetime(1, 1, 1, 0), VERSION);
            out.putInt32(SERIAL_Y);

            const char *const OD  = out.data();
            const int         LOD = out.length();

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
            cout << "\t\tOffset too small." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Datetime(1, 1, 1, 0), VERSION);
            out.putInt32(-1440);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            cout << "\t\tOffset too large." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Datetime(1, 1, 1, 0), VERSION);
            out.putInt32(1440);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
            cout << "\t\tOffset non-zero when must be zero." << endl;
        }
        {
            Out out(VERSION_SELECTOR, &allocator);
            using bslx::OutStreamFunctions::bdexStreamOut;

            bdexStreamOut(out, bdlt::Datetime(1, 1, 1, 24), VERSION);
            out.putInt32(1);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj mT(X);  const Obj& T = mT;
            ASSERT(X == T);

            In in(OD, LOD);
            ASSERT(in);
            in.setQuiet(!veryVerbose);

            using bslx::InStreamFunctions::bdexStreamIn;

            In& rvIn = bdexStreamIn(in, mT, VERSION);
            ASSERT(&in == &rvIn);
            ASSERT(!in);
            ASSERT(X == T);
        }

        if (verbose) {
             cout << "\nWire format direct tests." << endl;
        }
        {
            static const struct {
                int         d_lineNum;      // source line number
                int         d_offset;       // specification offset
                int         d_version;      // version to stream with
                int         d_length;       // expect output length
                const char *d_fmt_p;        // expected output format
            } DATA[] = {
    //LINE  OFFSET  VER  LEN  FORMAT
    //----  ------  ---  ---  ----------------------------------------------
    { L_,       -1,   1,  11, "\x00\x00\x01\x00\x00\x00\x00\xff\xff\xff\xff" },
    { L_,        0,   1,  11, "\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00" },
    { L_,        1,   1,  11, "\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x01" }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE        = DATA[i].d_lineNum;
                const int         OFFSET      = DATA[i].d_offset;
                const int         VERSION     = DATA[i].d_version;
                const int         LEN         = DATA[i].d_length;
                const char *const FMT         = DATA[i].d_fmt_p;

                // Test using class methods.

                {
                    Obj        mX(bdlt::Datetime(1, 1, 1, 0), OFFSET);
                    const Obj& X = mX;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = X.bdexStreamOut(out, VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    LOOP_ASSERT(LINE, LEN == out.length());
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0; j < out.length(); ++j) {
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
                    Obj        mX(bdlt::Datetime(1, 1, 1, 0), OFFSET);
                    const Obj& X = mX;

                    using bslx::OutStreamFunctions::bdexStreamOut;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = bdexStreamOut(out,
                                                               X,
                                                               VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    LOOP_ASSERT(LINE, LEN == out.length());
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0; j < out.length(); ++j) {
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

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any modifiable
        //:   target object to that of any source object.
        //:
        //: 2 The signature and return type are standard.
        //:
        //: 3 The reference returned is to the target object (i.e., '*this').
        //:
        //: 4 The value of the source object is not modified.
        //:
        //: 5 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a member-function
        //:   pointer having the appropriate signature and return type for the
        //:   copy-assignment operator defined in this component.  (C-2)
        //:
        //: 2 Using the table-driven technique, specify a set 'S' of (unique)
        //:   objects with substantial and varied differences in value.
        //:   Construct and initialize all combinations '(U, V)' in the cross
        //:   product 'S x S', assign 'U' from 'V', and verify the remaining
        //:   concerns.  (C-1, C-3..5)
        //
        // Testing:
        //   DatetimeTz& operator=(const DatetimeTz& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING COPY-ASSIGNMENT OPERATOR" << endl
                          << "================================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            (void)operatorAssignment;  // quash potential compiler warning
        }

        if (verbose) cout << "\nTesting Assignment u = V." << endl;
        {
            static const struct {
                int d_year;    // year of datetime
                int d_month;   // month of datetime
                int d_day;     // day of month of datetime
                int d_hour;    // hour of datetime
                int d_min;     // minute of datetime
                int d_sec;     // second of datetime
                int d_msec;    // millisecond of datetime
                int d_offset;  // timezone offset
            } DATA[] = {
            //    YEAR  MO  DAY  HOUR  MIN  SEC  MSEC   OFFSET
            //    ----  --  ---  ----  ---  ---  ----  -------
                {    1,  1,   1,   24,   0,   0,    0,       0 },
                {    2,  1,   1,    3,   0,   0,    0,       0 },
                {    2,  1,   1,    0,   3,   0,    0,       0 },
                {    2,  1,   1,    0,   0,   3,    0,       0 },
                {    2,  1,   1,    0,   0,   0,    3,       0 },
                {    2,  1,   1,    0,   0,   0,    0,       3 },
                {   10,  4,   5,    1,   0,   0,    0,       1 },
                {  100,  6,   7,    7,   1,   0,    0,   -1439 },
                { 1000,  8,   9,   22,   0,   1,    0,    1439 },
                { 2000,  2,  29,   12,   0,   0,    1,      -1 },
                { 2000,  2,  29,   12,   0,   0,    1,       0 },
                { 2000,  2,  29,   12,   0,   0,    1,       1 },
                { 2002,  7,   4,   18,  59,   0,    0,    1380 },
                { 2003,  8,   5,    9,   0,  59,    0,   -1380 },
                { 2004,  9,   3,   11,   0,   0,  999,    5*60 },
                { 9999, 12,  31,   18,  17,  42,  103,   -5*60 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int V_YEAR   = DATA[i].d_year;
                const int V_MONTH  = DATA[i].d_month;
                const int V_DAY    = DATA[i].d_day;
                const int V_HOUR   = DATA[i].d_hour;
                const int V_MIN    = DATA[i].d_min;
                const int V_SEC    = DATA[i].d_sec;
                const int V_MSEC   = DATA[i].d_msec;
                const int V_OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(V_YEAR) P_(V_MONTH) P_(V_DAY)
                    P_(V_HOUR) P_(V_MIN) P_(V_SEC) P_(V_MSEC)
                    P(V_OFFSET)
                }

                const bdlt::Datetime V_DATETIME(V_YEAR,
                                                V_MONTH,
                                                V_DAY,
                                                V_HOUR,
                                                V_MIN,
                                                V_SEC,
                                                V_MSEC);

                const Obj V(V_DATETIME, V_OFFSET);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int U_YEAR   = DATA[j].d_year;
                    const int U_MONTH  = DATA[j].d_month;
                    const int U_DAY    = DATA[j].d_day;
                    const int U_HOUR   = DATA[j].d_hour;
                    const int U_MIN    = DATA[j].d_min;
                    const int U_SEC    = DATA[j].d_sec;
                    const int U_MSEC   = DATA[j].d_msec;
                    const int U_OFFSET = DATA[j].d_offset;

                    if (veryVerbose) {
                        T_ T_ P_(U_YEAR) P_(U_MONTH) P_(U_DAY)
                        P_(U_HOUR) P_(U_MIN) P_(U_SEC) P_(U_MSEC)
                        P(U_OFFSET)
                    }

                    const bdlt::Datetime U_DATETIME(U_YEAR,
                                                    U_MONTH,
                                                    U_DAY,
                                                    U_HOUR,
                                                    U_MIN,
                                                    U_SEC,
                                                    U_MSEC);

                    Obj mU(U_DATETIME, U_OFFSET);  const Obj& U = mU;

                    const Obj ZZ(V);

                    if (veryVeryVerbose) { T_ T_ T_ P_(V) P_(U) P(ZZ) }

                    Obj *mR = &(mU = V);

                    if (veryVeryVerbose) { T_ T_ T_ P_(V) P_(U) P(ZZ) }
                    LOOP2_ASSERT(i, j, mR == &U);
                    LOOP2_ASSERT(i, j, ZZ == U);
                    LOOP2_ASSERT(i, j, ZZ == V);
                }
            }

            if (verbose) cout << "Testing self-assignment" << endl;
            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int HOUR   = DATA[i].d_hour;
                const int MIN    = DATA[i].d_min;
                const int SECS   = DATA[i].d_sec;
                const int MSEC   = DATA[i].d_msec;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(YEAR) P_(MONTH) P_(DAY)
                    P_(HOUR) P_(MIN) P_(SECS) P_(MSEC)
                    P(OFFSET)
                }

                const bdlt::Datetime TEMP_DATETIME(YEAR,
                                                   MONTH,
                                                   DAY,
                                                   HOUR,
                                                   MIN,
                                                   SECS,
                                                   MSEC);

                Obj mU(TEMP_DATETIME, OFFSET);  const Obj& U = mU;

                const Obj ZZ(U);

                if (veryVeryVerbose) { T_ T_ P_(U) P(ZZ) }
                LOOP_ASSERT(i, ZZ == U);

                Obj *mR = &(mU = U);

                if (veryVeryVerbose) { T_ T_ P_(U) P(ZZ) }
                LOOP_ASSERT(i, mR == &U);
                LOOP_ASSERT(i, ZZ == U);
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

        if (verbose) cout << "Not implemented for 'bdlt::DatetimeTz'." << endl;

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor creates an object having the same value as
        //:   that of the supplied original object.
        //:
        //: 2 The original object is passed as a reference providing
        //:   non-modifiable access to that object.
        //:
        //: 3 The value of the original object is unchanged.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row) in terms of their attributes.
        //:   Specify a set 'S' whose elements have substantial and varied
        //:   differences in value.  For each element in 'S', copy construct a
        //:   new object from 'S' and verify the concerns. (C-1..3)
        //
        // Testing:
        //   DatetimeTz(const DatetimeTz& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING COPY CONSTRUCTOR" << endl
                                  << "========================" << endl;

        if (verbose) cout << "\nTesting copy constructor." << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year of datetime
                int d_month;    // month of datetime
                int d_day;      // day of month of datetime
                int d_hour;     // hour of datetime
                int d_min;      // minute of datetime
                int d_sec;      // second of datetime
                int d_msec;     // millisecond of datetime
                int d_offset;   // timezone offset
            } DATA[] = {
            //    LINE  YEAR  MO  DAY  HOUR  MIN  SEC  MSEC   OFFSET
            //    ----  ----  --  ---  ----  ---  ---  ----  -------
                {   L_,    1,  1,   1,   24,   0,   0,    0,       0 },
                {   L_,   10,  4,   5,    1,   0,   0,    0,       1 },
                {   L_,  100,  6,   7,    7,   1,   0,    0,   -1439 },
                {   L_, 1000,  8,   9,   22,   0,   1,    0,    1439 },
                {   L_, 2000,  2,  29,   12,   0,   0,    1,      -1 },
                {   L_, 2002,  7,   4,   18,  59,   0,    0,    1380 },
                {   L_, 2003,  8,   5,    9,   0,  59,    0,   -1380 },
                {   L_, 2004,  9,   3,   11,   0,   0,  999,    5*60 },
                {   L_, 9999, 12,  31,   18,  17,  42,  103,   -5*60 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE   = DATA[i].d_lineNum;
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int HOUR   = DATA[i].d_hour;
                const int MIN    = DATA[i].d_min;
                const int SECS   = DATA[i].d_sec;
                const int MSEC   = DATA[i].d_msec;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(YEAR) P_(MONTH) P_(DAY)
                    P_(HOUR) P_(MIN) P_(SECS) P_(MSEC)
                    P(OFFSET)
                }

                const bdlt::Datetime TEMP_DATETIME(YEAR,
                                                   MONTH,
                                                   DAY,
                                                   HOUR,
                                                   MIN,
                                                   SECS,
                                                   MSEC);

                const Obj X(TEMP_DATETIME, OFFSET);
                const Obj ZZ(TEMP_DATETIME, OFFSET);
                const Obj Y(X);

                if (veryVerbose) { T_ T_ P_(X) P_(ZZ) P(Y) }
                ASSERTV(LINE, X == Y);
                ASSERTV(LINE, Y == ZZ);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if each of
        //:   their corresponding salient attributes respectively compares
        //:   equal.
        //:
        //: 2 'true  == (X == X)'  (i.e., identity)
        //:
        //: 3 'false == (X != X)'  (i.e., identity)
        //:
        //: 4 'X == Y' if and only if 'Y == X'  (i.e., commutativity)
        //:
        //: 5 'X != Y' if and only if 'Y != X'  (i.e., commutativity)
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'
        //:
        //: 7 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 8 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 9 The equality operator's signature and return type are standard.
        //:
        //:10 The inequality operator's signature and return type are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.
        //:   (C-7..10)
        //:
        //: 2 Using the table-driven technique, specify a set 'S' of unique
        //:   object values having various minor or subtle differences.  Verify
        //:   the correctness of 'operator==' and 'operator!=' using all
        //:   elements '(u, v)' of the cross product 'S X S'.  (C-1..6)
        //
        // Testing:
        //   bool operator==(const DatetimeTz& lhs, const DatetimeTz& rhs);
        //   bool operator!=(const DatetimeTz& lhs, const DatetimeTz& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING EQUALITY OPERATORS" << endl
                                  << "==========================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each operator to a variable." << endl;
        {
            typedef bool (*operatorPtr)(const Obj&, const Obj&);

            // Verify that the signatures and return types are standard.

            operatorPtr operatorEq = bdlt::operator==;
            operatorPtr operatorNe = bdlt::operator!=;

            (void)operatorEq;  // quash potential compiler warnings
            (void)operatorNe;
        }

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in S X S." << endl;
        {
            static const struct {
                int d_year;    // year of datetime
                int d_month;   // month of datetime
                int d_day;     // day of month of datetime
                int d_hour;    // hour of datetime
                int d_min;     // minute of datetime
                int d_sec;     // second of datetime
                int d_msec;    // millisecond of datetime
                int d_offset;  // timezone offset
            } DATA[] = {
            //    YEAR  MO  DAY  HOUR  MIN  SEC  MSEC   OFFSET
            //    ----  --  ---  ----  ---  ---  ----  -------
                {    1,  1,   1,   24,   0,   0,    0,       0 },
                {    2,  1,   1,    3,   0,   0,    0,       0 },
                {    2,  1,   1,    0,   3,   0,    0,       0 },
                {    2,  1,   1,    0,   0,   3,    0,       0 },
                {    2,  1,   1,    0,   0,   0,    3,       0 },
                {    2,  1,   1,    0,   0,   0,    0,       3 },
                {   10,  4,   5,    1,   0,   0,    0,       1 },
                {  100,  6,   7,    7,   1,   0,    0,   -1439 },
                { 1000,  8,   9,   22,   0,   1,    0,    1439 },
                { 2000,  2,  29,   12,   0,   0,    1,      -1 },
                { 2000,  2,  29,   12,   0,   0,    1,       0 },
                { 2000,  2,  29,   12,   0,   0,    1,       1 },
                { 2002,  7,   4,   18,  59,   0,    0,    1380 },
                { 2003,  8,   5,    9,   0,  59,    0,   -1380 },
                { 2004,  9,   3,   11,   0,   0,  999,    5*60 },
                { 9999, 12,  31,   18,  17,  42,  103,   -5*60 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int U_YEAR   = DATA[i].d_year;
                const int U_MONTH  = DATA[i].d_month;
                const int U_DAY    = DATA[i].d_day;
                const int U_HOUR   = DATA[i].d_hour;
                const int U_MIN    = DATA[i].d_min;
                const int U_SEC    = DATA[i].d_sec;
                const int U_MSEC   = DATA[i].d_msec;
                const int U_OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(U_YEAR) P_(U_MONTH) P_(U_DAY)
                    P_(U_HOUR) P_(U_MIN) P_(U_SEC) P_(U_MSEC)
                    P(U_OFFSET)
                }

                const bdlt::Datetime U_DATETIME(U_YEAR,
                                                U_MONTH,
                                                U_DAY,
                                                U_HOUR,
                                                U_MIN,
                                                U_SEC,
                                                U_MSEC);

                const Obj U(U_DATETIME, U_OFFSET);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int V_YEAR   = DATA[j].d_year;
                    const int V_MONTH  = DATA[j].d_month;
                    const int V_DAY    = DATA[j].d_day;
                    const int V_HOUR   = DATA[j].d_hour;
                    const int V_MIN    = DATA[j].d_min;
                    const int V_SEC    = DATA[j].d_sec;
                    const int V_MSEC   = DATA[j].d_msec;
                    const int V_OFFSET = DATA[j].d_offset;

                    if (veryVerbose) {
                        T_ T_ P_(V_YEAR) P_(V_MONTH) P_(V_DAY)
                        P_(V_HOUR) P_(V_MIN) P_(V_SEC) P_(V_MSEC)
                        P(V_OFFSET)
                    }

                    const bdlt::Datetime V_DATETIME(V_YEAR,
                                                    V_MONTH,
                                                    V_DAY,
                                                    V_HOUR,
                                                    V_MIN,
                                                    V_SEC,
                                                    V_MSEC);

                    const Obj V(V_DATETIME, V_OFFSET);

                    bool isSame = i == j;

                    if (veryVeryVerbose) { T_ T_ T_ P_(i) P_(j) P_(U) P(V) }
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
        // TESTING PRINT AND OUTPUT OPERATOR
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)',
        //:   but with each "attributeName = " elided.
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
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique, define set of distinct
        //:   formatting parameters and the corresponding expected output
        //:   string for the 'print' method.  If the value of the formatting
        //:   parameters, 'level' or 'spacesPerLevel' is -8, then the
        //:   parameters will be omitted in the method invocation.
        //:
        //:   1 Invoke the 'print' method passing the formatting parameters and
        //:     a 'const' object.
        //:
        //:   2 Verify the address of what is returned is that of the supplied
        //:     stream.  (C-5)
        //:
        //:   3 Verify that the output string has the expected value.
        //:     (C-1..2, 6)
        //:
        //: 3 Using the table-driven technique, define a set of distinct object
        //:   values and the expected string output of 'operator<<'.
        //:
        //:   1 Invoke 'operator<<' passing a 'const' object.
        //:
        //:   2 Verify the address of what is returned is that of the supplied
        //:     stream.  (C-8)
        //:
        //:   3 Verify that the output string has the expected value.  (C-3)
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream, int l, int spl) const;
        //   bsl::ostream& operator<<(bsl::ostream&, const DatetimeTz&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRINT AND OUTPUT OPERATOR" << endl
                          << "=================================" << endl;

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

        static const struct {
            int         d_lineNum;         // source line number
            int         d_level;           // indentation level
            int         d_spacesPerLevel;  // spaces per indentation level
            int         d_offset;          // tz offset
            const char *d_expected_p;      // expected output format
        } DATA[] = {
            //line  lvl  spaces  offset  format
            //----  ---  ------  ------  -----------------------------
            { L_,    0,     -1,      0,  "01JAN0001_00:00:00.000+0000"      },
            { L_,    0,      0,     15,  "01JAN0001_00:00:00.000+0015\n"    },
            { L_,    0,      2,     60,  "01JAN0001_00:00:00.000+0100\n"    },
            { L_,   -8,     -8,     60,  "01JAN0001_00:00:00.000+0100\n"    },
            { L_,    1,      1,     90,  " 01JAN0001_00:00:00.000+0130\n"   },
            { L_,    1,      2,    -20,  "  01JAN0001_00:00:00.000-0020\n"  },
            { L_,    1,      0,    -20,  "01JAN0001_00:00:00.000-0020\n"    },
            { L_,    1,     -1,    -20,  " 01JAN0001_00:00:00.000-0020"     },
            { L_,   -1,      2,   -330,  "01JAN0001_00:00:00.000-0530\n"    },
            { L_,   -1,      0,   -330,  "01JAN0001_00:00:00.000-0530\n"    },
            { L_,   -2,      1,    311,  "01JAN0001_00:00:00.000+0511\n"    },
            { L_,    2,      1,   1439,  "  01JAN0001_00:00:00.000+2359\n"  },
            { L_,    1,      3,  -1439,  "   01JAN0001_00:00:00.000-2359\n" },
            { L_,   -9,     -9,      0,  "01JAN0001_00:00:00.000+0000"      },
            { L_,   -9,     -9,     15,  "01JAN0001_00:00:00.000+0015"      },
            { L_,   -9,     -9,     60,  "01JAN0001_00:00:00.000+0100"      },
            { L_,   -9,     -9,     90,  "01JAN0001_00:00:00.000+0130"      },
            { L_,   -9,     -9,    -20,  "01JAN0001_00:00:00.000-0020"      },
            { L_,   -9,     -9,   -330,  "01JAN0001_00:00:00.000-0530"      },
            { L_,   -9,     -9,    311,  "01JAN0001_00:00:00.000+0511"      },
            { L_,   -9,     -9,   1439,  "01JAN0001_00:00:00.000+2359"      },
            { L_,   -9,     -9,  -1439,  "01JAN0001_00:00:00.000-2359"      },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const int         L    = DATA[ti].d_level;
                const int         SPL  = DATA[ti].d_spacesPerLevel;
                const int         OFF  = DATA[ti].d_offset;
                const char *const EXP  = DATA[ti].d_expected_p;

                if (veryVerbose) { T_ P_(L) P_(SPL) P_(OFF) }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                bdlt::Datetime datetime(1, 1, 1); // 01JAN0001_00:00:00.000
                const Obj      X(datetime, OFF);

                ostringstream os;

                // Verify supplied stream is returned by reference.

                if (-9 == L && -9 == SPL) {
                    LOOP_ASSERT(LINE, &os == &(os << X));

                    if (veryVeryVerbose) { T_ T_ Q(operator<<) }
                }
                else {
                    LOOP_ASSERT(LINE, -8 == SPL || -8 != L);

                    if (-8 != SPL) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L, SPL));
                    }
                    else if (-8 != L) {
                        LOOP_ASSERT(LINE, &os == &X.print(os, L));
                    }
                    else {
                        LOOP_ASSERT(LINE, &os == &X.print(os));
                    }

                    if (veryVeryVerbose) { T_ T_ Q(print) }
                }

                // Verify output is formatted as expected.

                if (veryVeryVerbose) { P(os.str()) }

                LOOP3_ASSERT(LINE, EXP, os.str(), EXP == os.str());
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //   Verify the basic accessors work as expected.
        //
        // Concerns:
        //: 1 Each accessor returns the value of the corresponding attribute
        //:   of the object.
        //:
        //: 2 Each accessor method is declared 'const'.
        //
        // Plan:
        //: 1 Use the table-driven technique, specify a range of distinct
        //:   object values. For each row 'R1' in the table:  (C-1..2)
        //:
        //:   1 Value construct an object to that of 'R1'.
        //:
        //:   2 Invoke each basic accessor from a reference providing
        //:     non-modifiable access to the object created in P-1.1 and verify
        //:     that the return value is correct.  (C-1..2)
        //
        // Testing:
        //   Datetime localDatetime() const;
        //   int offset() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING BASIC ACCESSORS" << endl
                                  << "=======================" << endl;

        if (verbose) cout << "\n'localDatetime()', 'offset()'" << endl;
        {
            static const struct {
                int d_year;    // year of datetime
                int d_month;   // month of datetime
                int d_day;     // day of month of datetime
                int d_hour;    // hour of datetime
                int d_min;     // minute of datetime
                int d_sec;     // second of datetime
                int d_msec;    // millisecond of datetime
                int d_offset;  // timezone offset
            } DATA[] = {
            //    YEAR  MO  DAY  HOUR  MIN  SEC  MSEC   OFFSET
            //    ----  --  ---  ----  ---  ---  ----  -------
                {    1,  1,   1,   24,   0,   0,    0,       0 },
                {   10,  4,   5,    1,   0,   0,    0,       1 },
                {  100,  6,   7,    7,   1,   0,    0,   -1439 },
                { 1000,  8,   9,   22,   0,   1,    0,    1439 },
                { 2000,  2,  29,   12,   0,   0,    1,      -1 },
                { 2002,  7,   4,   18,  59,   0,    0,    1380 },
                { 2003,  8,   5,    9,   0,  59,    0,   -1380 },
                { 2004,  9,   3,   11,   0,   0,  999,    5*60 },
                { 9999, 12,  31,   18,  17,  42,  103,   -5*60 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int HOUR   = DATA[i].d_hour;
                const int MIN    = DATA[i].d_min;
                const int SECS   = DATA[i].d_sec;
                const int MSEC   = DATA[i].d_msec;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(YEAR) P_(MONTH) P_(DAY)
                    P_(HOUR) P_(MIN) P_(SECS) P_(MSEC)
                    P(OFFSET)
                }

                const bdlt::Datetime TEMP_DATETIME(YEAR,
                                                   MONTH,
                                                   DAY,
                                                   HOUR,
                                                   MIN,
                                                   SECS,
                                                   MSEC);

                const Obj X(TEMP_DATETIME, OFFSET);

                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, TEMP_DATETIME == X.localDatetime());
                LOOP_ASSERT(i, OFFSET        == X.offset());
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTIONS 'gg' AND 'ggg'
        //   Void for 'bdlt_datetimetz'.
        //
        // Concerns:
        //   N/A
        //
        // Plan:
        //   N/A
        //
        // Testing:
        //   Reserved for testing of primitive generator functions.
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING GENERATOR FUNCTIONS 'gg' AND 'ggg'" << endl
                 << "==========================================" << endl;
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATOR - VALUE CONSTRUCTOR
        //   Verify the primary manipulators work as expected.
        //
        // Concerns:
        //: 1 The constructor must correctly set its value to the value
        //:   indicated by its two parameters.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Using the table-driven technique, specify a set 'S' of datetimes
        //:   and offsets as '(y, m, d, h, m, s, ms, o)' tuples having widely
        //:   varying values.  For each '(y, m, d, h, m, s, ms, o)' in 'S',
        //:   construct a 'DatetimeTz' object 'X' and verify that 'X' has the
        //:   expected value.  (C-1)
        //:
        //: 2 Verify that, in appropriate build modes, defensive checks are
        //:   triggered for invalid attribute values, but not triggered for
        //:   adjacent valid ones (using the 'BSLS_ASSERTTEST_*' macros).
        //:   (C-2)
        //
        // Testing:
        //   DatetimeTz(const Datetime& localDatetime, int offset);
        //   ~DatetimeTz();
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING PRIMARY MANIPULATOR - VALUE CONSTRUCTOR" << endl
                 << "===============================================" << endl;
        }

        if (verbose) cout << "\nTesting value constructor." << endl;
        {
            static const struct {
                int d_year;    // year of datetime
                int d_month;   // month of datetime
                int d_day;     // day of month of datetime
                int d_hour;    // hour of datetime
                int d_min;     // minute of datetime
                int d_sec;     // second of datetime
                int d_msec;    // millisecond of datetime
                int d_offset;  // timezone offset
            } DATA[] = {
            //    YEAR  MO  DAY  HOUR  MIN  SEC  MSEC   OFFSET
            //    ----  --  ---  ----  ---  ---  ----  -------
                {    1,  1,   1,   24,   0,   0,    0,       0 },
                {   10,  4,   5,    1,   0,   0,    0,       1 },
                {  100,  6,   7,    7,   1,   0,    0,   -1439 },
                { 1000,  8,   9,   22,   0,   1,    0,    1439 },
                { 2000,  2,  29,   12,   0,   0,    1,      -1 },
                { 2002,  7,   4,   18,  59,   0,    0,    1380 },
                { 2003,  8,   5,    9,   0,  59,    0,   -1380 },
                { 2004,  9,   3,   11,   0,   0,  999,    5*60 },
                { 9999, 12,  31,   18,  17,  42,  103,   -5*60 }
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int HOUR   = DATA[i].d_hour;
                const int MIN    = DATA[i].d_min;
                const int SECS   = DATA[i].d_sec;
                const int MSEC   = DATA[i].d_msec;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(YEAR) P_(MONTH) P_(DAY)
                    P_(HOUR) P_(MIN) P_(SECS) P_(MSEC)
                    P(OFFSET)
                }

                const bdlt::Datetime TEMP_DATETIME(YEAR,
                                                   MONTH,
                                                   DAY,
                                                   HOUR,
                                                   MIN,
                                                   SECS,
                                                   MSEC);

                const Obj X(TEMP_DATETIME, OFFSET);

                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, TEMP_DATETIME == X.localDatetime());
                LOOP_ASSERT(i, OFFSET        == X.offset());
            }
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_SAFE_FAIL(Obj(bdlt::Datetime(), -1));
            ASSERT_SAFE_PASS(Obj(bdlt::Datetime(),  0));
            ASSERT_SAFE_FAIL(Obj(bdlt::Datetime(),  1));

            ASSERT_SAFE_FAIL(Obj(bdlt::Datetime(1, 1, 1, 1),  1440));
            ASSERT_SAFE_PASS(Obj(bdlt::Datetime(1, 1, 1, 1),  1439));
            ASSERT_SAFE_FAIL(Obj(bdlt::Datetime(1, 1, 1, 1), -1440));
            ASSERT_SAFE_PASS(Obj(bdlt::Datetime(1, 1, 1, 1), -1439));
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
        //: 1 Execute each methods to verify functionality for simple case.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        const int YRA = 1, MOA = 2, DAA = 3;           // y, m, d for VA
        const int YRB = 4, MOB = 5, DAB = 6;           // y, m, d for VB
        const int YRC = 7, MOC = 8, DAC = 9;           // y, m, d for VC

        const bdlt::Datetime DA(YRA, MOA, DAA),
                             DB(YRB, MOB, DAB),
                             DC(YRC, MOC, DAC);

        const int OA = 60, OB = -300, OC = 270;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1.  Create an object x1 (init.  to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(DA, OA);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta.  Check initial state of x1." << endl;
        ASSERT(DA == X1.localDatetime());
        ASSERT(OA == X1.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2.  Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta.  Check the initial state of x2." << endl;
        ASSERT(DA == X2.localDatetime());
        ASSERT(OA == X2.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3.  Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setDatetimeTz(DB, OB);
        if (verbose) { cout << '\t';  P(X1); }
        ASSERT(DB == X1.localDatetime());
        ASSERT(OB == X1.offset());

        if (verbose) cout << "\ta.  Check new state of x1." << endl;

        if (verbose) cout <<
            "\tb.  Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4.  Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta.  Check initial state of x3." << endl;
        ASSERT(bdlt::Datetime() == X3.localDatetime());
        ASSERT(               0 == X3.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5.  Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout << "\ta.  Check initial state of x4." << endl;
        ASSERT(bdlt::Datetime() == X4.localDatetime());
        ASSERT(               0 == X4.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6.  Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setDatetimeTz(DC, OC);
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta.  Check new state of x3." << endl;
        ASSERT(DC == X3.localDatetime());
        ASSERT(OC == X3.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));
        ASSERT(0 == (X3 == X4));        ASSERT(1 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7.  Assign x2 = x1."
                             "\t\t\t\t{ x1:VB x2:VB x3:VC x4:U }" << endl;
        mX2 = X1;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta.  Check new state of x2." << endl;
        ASSERT(DB == X2.localDatetime());
        ASSERT(OB == X2.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(0 == (X2 == X3));        ASSERT(1 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8.  Assign x2 = x3."
                             "\t\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;
        mX2 = X3;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta.  Check new state of x2." << endl;
        ASSERT(DC == X2.localDatetime());
        ASSERT(OC == X2.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X2 == X1));        ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));        ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));        ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9.  Assign x1 = x1 (aliasing)."
                             "\t\t\t{ x1:VB x2:VC x3:VC x4:U }" << endl;

        mX1 = X1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta.  Check new state of x1." << endl;
        ASSERT(DB == X1.localDatetime());
        ASSERT(OB == X1.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
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

    // CONCERN: In no case does memory come from the global allocator.
    // CONCERN: In case 5 only does memory come from the default allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    LOOP_ASSERT(defaultAllocator.numBlocksTotal(),
                5 == test || 0 == defaultAllocator.numBlocksTotal());

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
