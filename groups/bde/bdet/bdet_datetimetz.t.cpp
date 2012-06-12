// bdet_datetimetz.t.cpp                                              -*-C++-*-

#include <bdet_datetimetz.h>

#include <bdet_date.h>
#include <bdet_datetime.h>
#include <bdet_time.h>

#include <bdex_testinstream.h>           // for testing only
#include <bdex_testoutstream.h>          // for testing only
#include <bdex_testinstreamexception.h>  // for testing only
#include <bdex_byteoutstream.h>          // for testing only
#include <bdex_byteinstream.h>           // for testing only

#include <bsls_assert.h>                 // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
//                                   Overview
//                                   --------
//
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE
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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdet_DatetimeTz    Obj;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

///Example 2
///---------
// Let us suppose that we are implementing a delivery estimation system for a
// shipping company.  The system provides estimated delivery dates and times
// of client shipments.  This information is provided in the local time zone
// and is represented as a 'bdet_DatetimeTz' object.  Below is the definition
// for a struct that returns the estimated delivery date.
//..
                          // =====================
                          // struct DeliverySystem
                          // =====================
//
 struct DeliverySystem {
     // This struct provides a function that returns the estimated delivery
     // date and time for a particular shipment.
//
     // PRIVATE CLASS METHODS
     static bdet_Datetime getCurrentUTCDatetime();
         // Return the current UTC date and time.
//
   public:
//
     // TYPES
     enum CITY {
         // This enumeration provides an identifier for the various cities.
//
         CHICAGO = 0,
         DUBAI,
         NEW_YORK,
         LONDON,
         LOS_ANGELES
     };
//
     // CLASS METHODS
     static bdet_DatetimeTz getEstimatedDeliveryDatetime(CITY city);
         // Return the estimated delivery date and time, in local time, for a
         // shipment being sent to the specified 'city'.
 };
//..
// All the relevant data used for delivery estimation is stored in a lookup
// table as shown below:
//..
 const int MINUTES_PER_HOUR = 60;
//
 static const struct {
     int d_offset;         // time zone offset from UTC (in minutes)
     int d_deliveryTime;   // delivery time (in minutes)
 } DATA[] = {
    //    Offset                 DeliveryTime
    //    ======                 ============
  {     -6 * MINUTES_PER_HOUR, 10 * MINUTES_PER_HOUR  },     // Chicago
  {      3 * MINUTES_PER_HOUR, 72 * MINUTES_PER_HOUR  },     // Dubai
  {     -5 * MINUTES_PER_HOUR,      MINUTES_PER_HOUR  },     // New York
  {          MINUTES_PER_HOUR, 36 * MINUTES_PER_HOUR  },     // London
  {     -8 * MINUTES_PER_HOUR, 24 * MINUTES_PER_HOUR  },     // Los Angeles
 };
//..
// And here are the function definitions:
//..

#include <sys/types.h>         // type definitions
#include <bsl_ctime.h>         // various time functions

                             // ---------------------
                             // struct DeliverySystem
                             // ---------------------
//
 // PRIVATE CLASS METHODS
 bdet_Datetime DeliverySystem::getCurrentUTCDatetime()
 {
     bsl::time_t currentTime = bsl::time(0);
     bsl::tm     gmtTime;
//
 #if defined(BSLS_PLATFORM__OS_WINDOWS) || ! defined(BDE_BUILD_TARGET_MT)
     gmtTime = *bsl::gmtime(&currentTime);
 #else
     gmtime_r(&currentTime, &gmtTime);
 #endif
//
     return bdet_Datetime(1900 + gmtTime.tm_year,
                          gmtTime.tm_mon,
                          gmtTime.tm_mday,
                          gmtTime.tm_hour,
                          gmtTime.tm_min,
                          gmtTime.tm_sec);
 }
//
 // CLASS METHODS
 bdet_DatetimeTz DeliverySystem::getEstimatedDeliveryDatetime(CITY city)
 {
     bdet_Datetime localDatetime(getCurrentUTCDatetime());
     localDatetime.addMinutes(DATA[city].d_offset
                            + DATA[city].d_deliveryTime);
     return bdet_DatetimeTz(localDatetime, DATA[city].d_offset);
 }

//=============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
//-----------------------------------------------------------------------------
// Assert handler to prevent premature test case failure when creating an
// invalid date to verify 'isValid' function.

static void assertHandler(const char *, const char *, int)
{
    // Do nothing.
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void) verbose;          // Eliminate unsued variable warning
    (void) veryVerbose;      // Eliminate unsued variable warning
    (void) veryVeryVerbose;  // Eliminate unsued variable warning

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 4: {
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
///Usage
///-----
///Example 1
///---------
// The following snippets of code illustrate how to create and use
// 'bdet_DateTz' objects.  First we will default construct an object
// 'datetimeTz1'.  A default constructed object contains an offset of 0,
// implying that the object represents a date and time in the UTC time zone.
// The value of the date and time is the same as that of a default constructed
// 'bdet_Datetime' object:
//..
 bdet_DatetimeTz datetimeTz1;
 ASSERT(0                           == datetimeTz1.offset());
 ASSERT(datetimeTz1.localDatetime() == bdet_Datetime());
//..
// Next we set 'datetimeTz1' to 12:00 noon (12:00:00.000) on 12/31/2005 in the
// EST time zone (UTC-5):
//..
 bdet_Datetime datetime1(2005, 12, 31, 12, 0, 0, 0);
 bdet_Datetime datetime2(datetime1);
 int           offset1 = -5 * 60;
//
 datetimeTz1.setDatetimeTz(datetime1, offset1);
 ASSERT(offset1                     == datetimeTz1.offset());
 ASSERT(datetimeTz1.localDatetime() != datetimeTz1.utcDatetime());
 ASSERT(datetimeTz1.localDatetime() == datetime1);
 ASSERT(datetimeTz1.utcDatetime()   != datetime2);
//
 datetime2.addMinutes(-offset1);
 ASSERT(datetimeTz1.utcDatetime()   == datetime2);
//..
// Then we create 'datetimeTz2' as a copy of 'datetimeTz1':
//..
 bdet_DatetimeTz datetimeTz2(datetimeTz1);
 ASSERT(offset1                     == datetimeTz2.offset());
 ASSERT(datetimeTz2.localDatetime() == datetime1);
 ASSERT(datetimeTz2.utcDatetime()   == datetime2);
//..
// We now create a third object, 'datetimeTz3', representing the time
// 10:33:25.000 on 01/01/2001 in the PST time zone (UTC-8):
//..
 bdet_Datetime  datetime3(2001, 1, 1, 10, 33, 25, 0);
 bdet_Datetime  datetime4(datetime3);
 int            offset2 = -8 * 60;
//
 bdet_DatetimeTz datetimeTz3(datetime3, offset2);
 ASSERT(offset2                     == datetimeTz3.offset());
 ASSERT(datetimeTz3.localDatetime() != datetimeTz3.utcDatetime());
 ASSERT(datetimeTz3.localDatetime() == datetime3);
 ASSERT(datetimeTz3.utcDatetime()   != datetime4);
//
 datetime4.addMinutes(-offset2);
 ASSERT(datetimeTz3.utcDatetime()   == datetime4);
//..
// Finally we stream out the values of 'datetimeTz1', 'datetimeTz2' and
// 'datetimeTz3' to 'stdout':
//..
if (veryVerbose) {
 bsl::cout << datetimeTz1 << bsl::endl
           << datetimeTz2 << bsl::endl
           << datetimeTz3 << bsl::endl;
}
//..
// The streaming operator produces the following output on 'stdout':
//..
//  31DEC2005-0500
//  31DEC2005-0500
//  01JAN2001-0800
//..
// Example 2
// ---------
//..
// When we print out the delivery times we get the following results:
//..
if (veryVerbose) {
     bsl::cout << "Estimated Delivery Time in Chicago:     "
               << DeliverySystem::getEstimatedDeliveryDatetime(
                                                    DeliverySystem::CHICAGO)
               << bsl::endl;
     bsl::cout << "Estimated Delivery Time in Dubai:       "
               << DeliverySystem::getEstimatedDeliveryDatetime(
                                                      DeliverySystem::DUBAI)
               << bsl::endl;
     bsl::cout << "Estimated Delivery Time in New York:    "
               << DeliverySystem::getEstimatedDeliveryDatetime(
                                                   DeliverySystem::NEW_YORK)
               << bsl::endl;
     bsl::cout << "Estimated Delivery Time in London:      "
               << DeliverySystem::getEstimatedDeliveryDatetime(
                                                     DeliverySystem::LONDON)
               << bsl::endl;
     bsl::cout << "Estimated Delivery Time in Los Angeles: "
               << DeliverySystem::getEstimatedDeliveryDatetime(
                                                DeliverySystem::LOS_ANGELES)
               << bsl::endl;
}
//..
//  Estimated Delivery Time in Chicago:     04JAN2008_22:45:49.000-0600
//  Estimated Delivery Time in Dubai:       07JAN2008_21:45:49.000+0300
//  Estimated Delivery Time in New York:    04JAN2008_14:45:49.000-0500
//  Estimated Delivery Time in London:      06JAN2008_07:45:49.000+0100
//  Estimated Delivery Time in Los Angeles: 05JAN2008_10:45:49.000-0800
//..
   } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING isValid and validateAndSetDatetimeTz
        //
        // Concerns:
        //   That 'isValid' and 'validateAndSetDatetimeTz' work.
        //
        // Plan:
        //   Configure valid and invalid values.  Note that the manipulators
        //   of 'bdet_Date' and 'bdet_Time' make it very difficult to give
        //   those objects invalid values.  The main thing we want to test
        //   here is the that 'offset' is not allowed to have a nonzero value
        //   when the time is null (24:00:00).
        // --------------------------------------------------------------------

        // Set the assert handler to mute error coming from construction of an
        // invalid date.

        bsls_Assert::setFailureHandler(&assertHandler);

        enum { MAX_TIMEZONE = 24 * 60 - 1,
               MTZ = MAX_TIMEZONE };

        struct {
            int d_line;
            int d_year;
            int d_month;
            int d_day;
            int d_hour;
            int d_minute;
            int d_second;
            int d_millisecond;
            int d_offset;
            int d_isValid;
        } DATA[] = {
            //    year  mo day  hr  mn sec msec   off  valid
            //    ====  == ===  ==  == === ====   ===  =
            { L_, 2005,  1,  1, 24,  0,  0,   0,     0, 1 },
            { L_, 2005,  1,  1, 24,  0,  0,   0,     1, 0 },
            { L_, 2005,  1,  1, 24,  0,  0,   0,  4*60, 0 },
            { L_, 2005,  1,  1, 24,  0,  0,   0, -4*60, 0 },
            { L_, 2005,  1,  1, 12,  1,  1,   0,   MTZ, 1 },
            { L_, 2005,  1,  1, 12,  1,  1,   0,  -MTZ, 1 },
            { L_, 2005,  1,  1, 12,  1,  1,   0, 24*60, 0 },
            { L_, 2005,  1,  1, 12,  1,  1,   0,-24*60, 0 },
            { L_, 2005,  1,  1, 12,  1,  1,   0, 99*60, 0 },
            { L_, 2005,  1,  1, 12,  1,  1,   0,-99*60, 0 },
            { L_,    1,  1,  1, 12,  1,  1,   0,     0, 1 },
            { L_,    1,  1,  1, 12,  1,  1,   0,  4*60, 1 },
            { L_,    1,  1,  1, 12,  1,  1,   0, -4*60, 1 },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE        = DATA[di].d_line;
            const int YEAR        = DATA[di].d_year;
            const int MONTH       = DATA[di].d_month;
            const int DAY         = DATA[di].d_day;
            const int HOUR        = DATA[di].d_hour;
            const int MINUTE      = DATA[di].d_minute;
            const int SECOND      = DATA[di].d_second;
            const int MILLISECOND = DATA[di].d_millisecond;
            const int OFFSET      = DATA[di].d_offset;
            const int IS_VALID    = DATA[di].d_isValid;

            // it is extremely difficult to give a bdet_Time an invalid value
            bdet_Datetime datetime;
            datetime.setYearMonthDay(YEAR, MONTH, DAY);
            datetime.setMillisecond(MILLISECOND);
            datetime.setSecond(SECOND);
            datetime.setMinute(MINUTE);
            datetime.setHour(HOUR);

            LOOP4_ASSERT(LINE, datetime, OFFSET, IS_VALID, IS_VALID ==
                                   bdet_DatetimeTz::isValid(datetime, OFFSET));

            bdet_DatetimeTz datetimeTz;
            LOOP4_ASSERT(LINE, datetime, OFFSET, IS_VALID, IS_VALID ==
                      ! datetimeTz.validateAndSetDatetimeTz(datetime, OFFSET));

            if (! IS_VALID) {
                const bdet_DatetimeTz nullDatetimeTz;
                LOOP_ASSERT(LINE, nullDatetimeTz == datetimeTz);
            }
            else {
                const bdet_Datetime datetime = datetimeTz.localDatetime();
                LOOP_ASSERT(LINE, datetime.year() == YEAR);
                LOOP_ASSERT(LINE, datetime.month() == MONTH);
                LOOP_ASSERT(LINE, datetime.day() == DAY);
                LOOP_ASSERT(LINE, datetime.hour() == HOUR);
                LOOP_ASSERT(LINE, datetime.minute() == MINUTE);
                LOOP_ASSERT(LINE, datetime.second() == SECOND);
                LOOP_ASSERT(LINE, datetime.millisecond() == MILLISECOND);

                LOOP_ASSERT(LINE, datetimeTz.offset() == OFFSET);
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRINT
        //
        // Plan:
        //   Test that the 'print' method produces the expected results for
        //   various values of 'level' and 'spacesPerLevel'.
        //
        // Testing:
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        // --------------------------------------------------------------------

        if (veryVerbose) cout << "Testing 'print'." << endl;
        {
            const int LMT = 60 * 100;
            static const struct {
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
            // indent spaces offset      format
            // ------ ------ ------      --------------
            {  0,     -1,    0,         "01JAN0001_17:00:00.000+0000"      },
            {  0,      0,    15,        "01JAN0001_17:00:00.000+0015\n"    },
            {  0,      2,    60,        "01JAN0001_17:00:00.000+0100\n"    },
            {  1,      1,    90,        " 01JAN0001_17:00:00.000+0130\n"   },
            {  1,      2,    -20,       "  01JAN0001_17:00:00.000-0020\n"  },
            { -1,      2,    -330,      "01JAN0001_17:00:00.000-0530\n"    },
            { -2,      1,    311,       "01JAN0001_17:00:00.000+0511\n"    },
            {  2,      1,    1439,      "  01JAN0001_17:00:00.000+2359\n"  },
            {  1,      3,    -1439,     "   01JAN0001_17:00:00.000-2359\n" },

// TBD
#if 0
            // The following elements test expected undefined behavior
            {  0,     -1,    1441,      "01JAN0001_17:00:00.000+1441"    },
            {  0,     -1,    -1441,     "01JAN0001_17:00:00.000-1441"    },
            {  0,     -1,    LMT-60,    "01JAN0001_17:00:00.000+9900"    },
            {  0,     -1,    -LMT+60,   "01JAN0001_17:00:00.000-9900"    },
            {  0,     -1,    LMT-1,     "01JAN0001_17:00:00.000+9959"    },
            {  0,     -1,    -LMT+1,    "01JAN0001_17:00:00.000-9959"    },
            {  0,     -1,    LMT,       "01JAN0001_17:00:00.000+XX00"    },
            {  0,     -1,    -LMT,      "01JAN0001_17:00:00.000-XX00"    },
            {  0,     -1,    LMT+11,    "01JAN0001_17:00:00.000+XX11"    },
            {  0,     -1,    -LMT-11,   "01JAN0001_17:00:00.000-XX11"    },
            {  0,     -1,    1000*LMT,  "01JAN0001_17:00:00.000+XX00"    },
            {  0,     -1,    -1000*LMT, "01JAN0001_17:00:00.000-XX00"    },
#endif
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 128;  // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Value 1 used for an unset char.
            const char Z2  = 0x00; // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const int         OFF  = DATA[ti].d_offset;
                const char *const FMT  = DATA[ti].d_fmt_p;

                bdet_Datetime datetime(1, 1, 1, 17);  // 01JAN0001_17:00:00.000
                Obj mX(datetime, OFF);  const Obj& X = mX;
                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                ostrstream out1(buf1, SIZE);  X.print(out1, IND, SPL) << ends;
                ostrstream out2(buf2, SIZE);  X.print(out2, IND, SPL) << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, 0 == strcmp(buf1, FMT));
                LOOP_ASSERT(ti, 0 == strcmp(buf2, FMT));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }

        if (verbose) cout << "\nTesting 'print' with "
                             "manipulators and left alignment." << endl;
        {
            const int LMT = 60 * 100;
            static const struct {
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
            // indent spaces offset format
            // ------ ------ ------ --------------
            {  0,     -1,    0,     "01JAN0001_17:00:00.000+0000@@@"      },
            {  0,      0,    15,    "01JAN0001_17:00:00.000+0015@@@\n"    },
            {  0,      2,    60,    "01JAN0001_17:00:00.000+0100@@@\n"    },
            {  1,      1,    90,    " 01JAN0001_17:00:00.000+0130@@@\n"   },
            {  1,      2,    -20,   "  01JAN0001_17:00:00.000-0020@@@\n"  },
            { -1,      2,    -330,  "01JAN0001_17:00:00.000-0530@@@\n"    },
            { -2,      1,    311,   "01JAN0001_17:00:00.000+0511@@@\n"    },
            {  2,      1,    1439,  "  01JAN0001_17:00:00.000+2359@@@\n"  },
            {  1,      3,    -1439, "   01JAN0001_17:00:00.000-2359@@@\n" },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 128;  // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Value 1 used for an unset char.
            const char Z2  = 0x00; // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            const char FILL_CHAR = '@'; // Used for filling whitespaces due
                                        // to 'setw'.

            const int  FORMAT_WIDTH = 30;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const int         OFF  = DATA[ti].d_offset;
                const char *const FMT  = DATA[ti].d_fmt_p;

                bdet_Datetime datetime(1, 1, 1, 17);  // 01JAN0001_17:00:00.000
                Obj mX(datetime, OFF);  const Obj& X = mX;
                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl << FMT
                                      << endl;

                ostrstream out1(buf1, SIZE);
                out1 << left << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                X.print(out1, IND, SPL);
                out1 << ends;

                ostrstream out2(buf2, SIZE);
                out2 << left << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                X.print(out2, IND, SPL);
                out2 << ends;

                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl << buf1
                                      << endl;

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, 0 == strcmp(buf1, FMT));
                LOOP_ASSERT(ti, 0 == strcmp(buf2, FMT));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }

        if (verbose) cout << "\nTesting 'print' with "
                             "manipulators and right alignment." << endl;
        {
            const int LMT = 60 * 100;
            static const struct {
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
            // indent spaces offset format
            // ------ ------ ------ --------------
            {  0,     -1,    0,     "@@@01JAN0001_17:00:00.000+0000"      },
            {  0,      0,    15,    "@@@01JAN0001_17:00:00.000+0015\n"    },
            {  0,      2,    60,    "@@@01JAN0001_17:00:00.000+0100\n"    },
            {  1,      1,    90,    " @@@01JAN0001_17:00:00.000+0130\n"   },
            {  1,      2,    -20,   "  @@@01JAN0001_17:00:00.000-0020\n"  },
            { -1,      2,    -330,  "@@@01JAN0001_17:00:00.000-0530\n"    },
            { -2,      1,    311,   "@@@01JAN0001_17:00:00.000+0511\n"    },
            {  2,      1,    1439,  "  @@@01JAN0001_17:00:00.000+2359\n"  },
            {  1,      3,    -1439, "   @@@01JAN0001_17:00:00.000-2359\n" },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 128;  // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Value 1 used for an unset char.
            const char Z2  = 0x00; // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            const char FILL_CHAR = '@'; // Used for filling whitespaces due
                                        // to 'setw'.

            const int  FORMAT_WIDTH = 30;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const int         OFF  = DATA[ti].d_offset;
                const char *const FMT  = DATA[ti].d_fmt_p;

                bdet_Datetime datetime(1, 1, 1, 17);  // 01JAN0001_17:00:00.000
                Obj mX(datetime, OFF);  const Obj& X = mX;
                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl << FMT
                                      << endl;

                ostrstream out1(buf1, SIZE);
                out1 << right << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                X.print(out1, IND, SPL);
                out1 << ends;

                ostrstream out2(buf2, SIZE);
                out2 << right << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                X.print(out2, IND, SPL);
                out2 << ends;

                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl << buf1
                                      << endl;

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, 0 == strcmp(buf1, FMT));
                LOOP_ASSERT(ti, 0 == strcmp(buf2, FMT));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }
      } break;
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

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // Values for testing

        const int YRA = 1, MOA = 2, DAA = 3;           // y, m, d for VA
        const int YRB = 4, MOB = 5, DAB = 6;           // y, m, d for VB
        const int YRC = 7, MOC = 8, DAC = 9;           // y, m, d for VC

        const int HRA = 1, MIA = 2, SCA = 3, MSA = 4;  // h, m, s, ms for VA
        const int HRB = 5, MIB = 6, SCB = 7, MSB = 8;  // h, m, s, ms for VB
        const int HRC = 9, MIC = 9, SCC = 9, MSC = 9;  // h, m, s, ms for VC

        const bdet_Date DA(YRA, MOA, DAA),
                        DB(YRB, MOB, DAB),
                        DC(YRC, MOC, DAC);

        const bdet_Time TA(HRA, MIA, SCA, MSA),
                        TB(HRB, MIB, SCB, MSB),
                        TC(HRC, MIC, SCC, MSC);

        const bdet_Datetime DTA(DA, TA),
                            DTB(DB, TB),
                            DTC(DC, TC);

        const int OA = 60, OB = -300, OC = 270;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(DTA, OA);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(DTA == X1.localDatetime());
        ASSERT( OA == X1.offset());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(DTA == X2.localDatetime());
        ASSERT( OA == X2.offset());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setDatetimeTz(DTB, OB);
        if (verbose) { cout << '\t';  P(X1); }
        ASSERT(DTB == X1.localDatetime());
        ASSERT( OB == X1.offset());

        if (verbose) cout << "\ta. Check new state of x1." << endl;

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Create a default object x3()."
                             "\t\t{ x1:VB x2:VA x3:U }" << endl;
        Obj mX3;  const Obj& X3 = mX3;
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta. Check initial state of x3." << endl;
        ASSERT(bdet_Datetime() == X3.localDatetime());
        ASSERT(              0 == X3.offset());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(0 == (X3 == X1));        ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));        ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));        ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Create an object x4 (copy from x3)."
                             "\t\t{ x1:VA x2:VA x3:U  x4:U }" << endl;
        Obj mX4(X3);  const Obj& X4 = mX4;
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        ASSERT(bdet_Datetime() == X4.localDatetime());
        ASSERT(              0 == X4.offset());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setDatetimeTz(DTC, OC);
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(DTC == X3.localDatetime());
        ASSERT( OC == X3.offset());

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
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(DTB == X2.localDatetime());
        ASSERT( OB == X2.offset());

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
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check new state of x2." << endl;
        ASSERT(DTC == X2.localDatetime());
        ASSERT( OC == X2.offset());

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
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        ASSERT(DTB == X1.localDatetime());
        ASSERT( OB == X1.offset());

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
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
