// bdet_datetz.t.cpp                                                  -*-C++-*-

#include <bdet_datetz.h>

#include <bdet_date.h>
#include <bdet_datetime.h>

#include <bdex_instreamfunctions.h>      // for testing only
#include <bdex_outstreamfunctions.h>     // for testing only
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
//
//                                   Overview
//                                   --------
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// [10] static bool isValid(const bdet_Date& localDate, int offset);
// [ 7] static int maxSupportedBdexVersion() const;
//
// CREATORS
// [ 2] bdet_DateTz();
// [ 7] bdet_DateTz(const bdet_DateTz& original);
// [12] bdet_DateTz(const bdet_Date& localDate, int offset);
//
// MANIPULATORS
// [2]  void setDateTz(const bdet_Date& localDate, int offset);
// [10] bool validateAndSetDateTz(const bdet_Date& localDate, int offset);
//
// ACCESSORS
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [11] bdet_Datetime utcStartTime() const;
// [13] bdet_Datetime gmtStartTime() const;
// [ 4] bdet_Date localDate() const;
// [ 4] int offset() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream, int l, int spl) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const bdet_DateTz& lhs, const bdet_DateTz& rhs);
// [ 6] bool operator!=(const bdet_DateTz& lhs, const bdet_DateTz& rhs);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const bdet_DateTz&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [14] USAGE EXAMPLE
//-----------------------------------------------------------------------------

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
#define T_  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdet_DateTz        Obj;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

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
      case 14: {
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
// The following snippets of code illustrate how to create and use
// 'bdet_DateTz' objects.  First we will default construct an object 'date1'.
// A default constructed object contains an offset of 0, implying that the
// object represents a date in the UTC time zone.  The value of the date is the
// same as that of a default constructed 'bdet_Date' object:
//..
  bdet_DateTz dateTz1;
  ASSERT(0                   == dateTz1.offset());
  ASSERT(dateTz1.localDate() == dateTz1.utcStartTime().date());
  ASSERT(dateTz1.localDate() == bdet_Date());
//..
// Next we set 'dateTz1' to 12/31/2005 in the EST time zone (UTC-5):
//..
  bdet_Date     date1(2005, 12, 31);
  bdet_Datetime datetime1(date1, bdet_Time(0, 0, 0, 0));
  int           offset1 = -5 * 60;

  dateTz1.setDateTz(date1, offset1);
  ASSERT(offset1                == dateTz1.offset());
  ASSERT(dateTz1.localDate()    == dateTz1.utcStartTime().date());
  ASSERT(dateTz1.localDate()    == date1);
  ASSERT(dateTz1.utcStartTime() != datetime1);

  datetime1.addMinutes(-offset1);
  ASSERT(dateTz1.utcStartTime() == datetime1);
//..
// Then we create 'dateTz2' as a copy of 'dateTz1':
//..
  bdet_DateTz dateTz2(dateTz1);
  ASSERT(offset1                == dateTz2.offset());
  ASSERT(dateTz2.localDate()    == dateTz2.utcStartTime().date());
  ASSERT(dateTz2.localDate()    == date1);
  ASSERT(dateTz2.utcStartTime() == datetime1);
//..
// We now create a third object, 'dateTz3', representing a date 01/01/2001 in
// the PST time zone (UTC-8):
//..
  bdet_Date     date2(2001, 01, 01);
  bdet_Datetime datetime2(date2, bdet_Time(0, 0, 0, 0));
  int           offset2 = -8 * 60;

  bdet_DateTz dateTz3(date2, offset2);
  ASSERT(offset2                == dateTz3.offset());
  ASSERT(dateTz3.localDate()    == dateTz3.utcStartTime().date());
  ASSERT(dateTz3.localDate()    == date2);
  ASSERT(dateTz3.utcStartTime() != datetime2);

  datetime2.addMinutes(-offset2);
  ASSERT(dateTz3.utcStartTime() == datetime2);
//..
// Finally we stream out the values of 'dateTz1', 'dateTz2' and 'dateTz3' to
// 'stdout':
//..
if (veryVerbose)
  bsl::cout << dateTz1 << bsl::endl
            << dateTz2 << bsl::endl
            << dateTz3 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  31DEC2005-0500
//  31DEC2005-0500
//  01JAN2001-0800
//..
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'gmtStartTime'
        //
        // Concerns:
        //   That the deprecated 'gmtStartTime' computes the correct UTC start
        //   time of the local date accordingly to its timezone.
        //
        // Plan:
        //   Given a table of possible dates their offset and their starting
        //   UTC time verify that 'utcStartTime' computation corresponds to the
        //   tabulated data and that it also holds that
        //   'utcStartTime() == localDate() - offset()'
        //
        // Testing:
        //   bdet_Datetime gmtStartTime() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'gmtStartTime'" << endl
                          << "======================" << endl;
        struct {
            int d_line;
            int d_year;
            int d_month;
            int d_day;
            int d_offset;
            int d_gmtYear;
            int d_gmtMonth;
            int d_gmtDay;
            int d_gmtHour;
            int d_gmtMinute;
        } DATA[] = {
            //LINE YR MO D  OFF G_Y G_M G_D G_H G_M
            //---- -- -- -- --- --- --- --- --- ---
            { L_,   1, 1, 1,  0,  1,  1,  1,  0,  0 },
            { L_,   1, 1, 1, -1,  1,  1,  1,  0,  1 },
            { L_,   1, 1, 2,  1,  1,  1,  1, 23, 59 }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE       = DATA[i].d_line;
            const int YEAR       = DATA[i].d_year;
            const int MONTH      = DATA[i].d_month;
            const int DAY        = DATA[i].d_day;
            const int OFFSET     = DATA[i].d_offset;
            const int GMT_YEAR   = DATA[i].d_gmtYear;
            const int GMT_MONTH  = DATA[i].d_gmtMonth;
            const int GMT_DAY    = DATA[i].d_gmtDay;
            const int GMT_HOUR   = DATA[i].d_gmtHour;
            const int GMT_MINUTE = DATA[i].d_gmtMinute;

            if(veryVerbose) {
                T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P_(OFFSET) P_(GMT_YEAR)
                          P_(GMT_MONTH) P_(GMT_DAY) P_(GMT_HOUR) P(GMT_MINUTE)
            }

            const bdet_Date TEMP_DATE(YEAR, MONTH, DAY);
            Obj x; const Obj& X = x;
            x.setDateTz(TEMP_DATE, OFFSET);

            const bdet_Datetime EXP1(GMT_YEAR,
                                     GMT_MONTH,
                                     GMT_DAY,
                                     GMT_HOUR,
                                     GMT_MINUTE);

            bdet_Datetime exp2(bdet_Datetime(X.localDate()));
            const bdet_Datetime& EXP2 = exp2;
            exp2.addMinutes(-X.offset());

            if (veryVerbose) {
                T_  cout << "GMT START TIME: " << X.gmtStartTime() << endl;
            }
            LOOP_ASSERT(i, EXP1 == X.gmtStartTime());
            LOOP_ASSERT(i, EXP2 == X.gmtStartTime());
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING STATE CONSTRUCTOR:
        //
        //
        // Concerns:
        //   The constructor must correctly forward its three parameters to a
        //   tested utility function and correctly initialize the serial date
        //   integer with the returned result.
        //
        // Plan:
        //   Specify a set S of dates and offsets as (y, m, d, o) quadruplets
        //   having widely varying values.  For each (y, m, d, o) in S,
        //   construct a date object X and verify that X has the expected
        //   value.
        //
        // Testing:
        //   bdet_DateTz(const bdet_Date& localDate, int offset);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Copy Constructor" << endl
                                  << "========================" << endl;

        if (verbose) cout << "\nTesting copy constructor." << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day; int d_offset;
            } DATA[] = {
                //    year  mo day      off
                //    ====  == ===    =======
                    {    1,  1,  1,    0     },
                    {   10,  4,  5,    1     },
                    {  100,  6,  7,   -1439  },
                    { 1000,  8,  9,    1439  },
                    { 2000,  2, 29,   -1     },
                    { 2002,  7,  4,    1380  },
                    { 2003,  8,  5,   -1380  },
                    { 2004,  9,  3,    5*60  },
                    { 9999, 12, 31,   -5*60  }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P_(DAY) P(OFFSET) }

                const bdet_Date TEMP_DATE(YEAR, MONTH, DAY);
                Obj x;  const Obj& X = x;
                x.setDateTz(TEMP_DATE, OFFSET);

                const Obj Y(TEMP_DATE, OFFSET);

                if (veryVeryVerbose) { T_ T_ P_(X) P(Y) }
                LOOP_ASSERT(i, X == Y);
            }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING 'utcStartTime'
        //
        // Concerns:
        //   That 'utcStartTime' computes the correct UTC start time of the
        //   local date accordingly to its timezone.
        //
        // Plan:
        //   Given a table of possible dates their offset and their starting
        //   UTC time verify that 'utcStartTime' computation corresponds to the
        //   tabulated data and that it also holds that
        //   'utcStartTime() == localDate() - offset()'
        //
        // Testing:
        //   bdet_Datetime utcStartTime() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'utcStartTime'" << endl
                          << "======================" << endl;
        struct {
            int d_line;
            int d_year;
            int d_month;
            int d_day;
            int d_offset;
            int d_utcYear;
            int d_utcMonth;
            int d_utcDay;
            int d_utcHour;
            int d_utcMinute;
        } DATA[] = {
            //LINE YR MO D  OFF G_Y G_M G_D G_H G_M
            //---- -- -- -- --- --- --- --- --- ---
            { L_,   1, 1, 1,  0,  1,  1,  1,  0,  0 },
            { L_,   1, 1, 1, -1,  1,  1,  1,  0,  1 },
            { L_,   1, 1, 2,  1,  1,  1,  1, 23, 59 }
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE       = DATA[i].d_line;
            const int YEAR       = DATA[i].d_year;
            const int MONTH      = DATA[i].d_month;
            const int DAY        = DATA[i].d_day;
            const int OFFSET     = DATA[i].d_offset;
            const int UTC_YEAR   = DATA[i].d_utcYear;
            const int UTC_MONTH  = DATA[i].d_utcMonth;
            const int UTC_DAY    = DATA[i].d_utcDay;
            const int UTC_HOUR   = DATA[i].d_utcHour;
            const int UTC_MINUTE = DATA[i].d_utcMinute;

            if(veryVerbose) {
                T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P_(OFFSET) P_(UTC_YEAR)
                          P_(UTC_MONTH) P_(UTC_DAY) P_(UTC_HOUR) P(UTC_MINUTE)
            }

            const bdet_Date TEMP_DATE(YEAR, MONTH, DAY);
            Obj x; const Obj& X = x;
            x.setDateTz(TEMP_DATE, OFFSET);

            const bdet_Datetime EXP1(UTC_YEAR,
                                     UTC_MONTH,
                                     UTC_DAY,
                                     UTC_HOUR,
                                     UTC_MINUTE);

            bdet_Datetime exp2(bdet_Datetime(X.localDate()));
            const bdet_Datetime& EXP2 = exp2;
            exp2.addMinutes(-X.offset());

            if (veryVerbose) {
                T_  cout << "UTC START TIME: " << X.utcStartTime() << endl;
            }
            LOOP_ASSERT(i, EXP1 == X.utcStartTime());
            LOOP_ASSERT(i, EXP2 == X.utcStartTime());
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'isValid' AND 'validateAndSetDateTz'
        //
        // Concerns:
        //   That 'isValid' and 'validateAndSetDateTz' perform correctly.
        //
        // Plan:
        //   Given a table of valid and invalid values, test the routines
        //   and make sure they determine validity correctly, and determine
        //   whether the value of the DateTz is correct after
        //   validateAndSetDateTz.
        //
        // Testing:
        //   static bool isValid(const bdet_Date& date, int offset);
        //   int validateAndSetDateTz(const bdet_Date& date, int offset);
        // --------------------------------------------------------------------

        // Set the assert handler to mute error coming from construction of an
        // invalid date.

        if (verbose) cout << endl
                          << "TESTING 'isValid' and 'validateAndSetDateTz"
                          << endl
                          << "==========================================="
                          << endl;

        bsls_Assert::setFailureHandler(&assertHandler);

        enum { MAX_TIMEZONE = 24 * 60 - 1,
               MTZ = MAX_TIMEZONE };

        struct {
            int d_line;
            int d_year;
            int d_month;
            int d_day;
            int d_offset;
            int d_isValid;
        } DATA[] = {
            //    year  mo day    off  valid
            //    ====  == ===    ===  =====
            { L_, 1776,  7,  4,     0,     1 },
            { L_, 1776,  7,  4,  4*60,     1 },
            { L_, 1776,  7,  4, -4*60,     1 },
            { L_, 1776,  7,  4,   MTZ,     1 },
            { L_, 1776,  7,  4,  -MTZ,     1 },
            { L_, 1776,  7,  4, 24*60,     0 },
            { L_, 1776,  7,  4,-24*60,     0 },
            { L_, 1776,  7,  4, 99*60,     0 },
            { L_, 1776,  7,  4,-99*60,     0 },
            { L_,    1,  1,  1,     0,     1 },
            { L_,    1,  1,  1,   MTZ,     1 },
            { L_,    1,  1,  1,  -MTZ,     1 },
            { L_,    1,  1,  1, 24*60,     0 },
            { L_,    1,  1,  1,-24*60,     0 },
            { L_,    1,  1,  1, 99*60,     0 },
            { L_,    1,  1,  1,-99*60,     0 } };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE     = DATA[di].d_line;
            const int YEAR     = DATA[di].d_year;
            const int MONTH    = DATA[di].d_month;
            const int DAY      = DATA[di].d_day;
            const int OFFSET   = DATA[di].d_offset;
            const int IS_VALID = DATA[di].d_isValid;

            const bdet_Date SRC_DATE(YEAR, MONTH, DAY);
            LOOP_ASSERT(LINE,
                        IS_VALID == bdet_DateTz::isValid(SRC_DATE, OFFSET));

            const bdet_Date DEST_DATE(2003,  3, 18);
            bdet_DateTz t(DEST_DATE, 5*60);

            int sts = t.validateAndSetDateTz(SRC_DATE, OFFSET);
            LOOP_ASSERT(LINE, IS_VALID == !sts);

            if (IS_VALID) {
                LOOP_ASSERT(LINE, t.localDate().year()  == YEAR);
                LOOP_ASSERT(LINE, t.localDate().month() == MONTH);
                LOOP_ASSERT(LINE, t.localDate().day()   == DAY);
                LOOP_ASSERT(LINE, t.offset() == OFFSET);
            }
            else {
                // verify t was unchanged.

                LOOP_ASSERT(LINE, t.localDate().year()  == 2003);
                LOOP_ASSERT(LINE, t.localDate().month() == 3);
                LOOP_ASSERT(LINE, t.localDate().day()   == 18);
                LOOP_ASSERT(LINE, t.offset() == 5*60);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   The 'bdex' streaming concerns for this component are standard.  We
        //   first test the class method 'maxSupportedBdexVersion' and then use
        //   that method to probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality.  We then thoroughly test streaming functionality
        //   relying on the bdex functions which forward appropriate calls to
        //   the member functions of this component.  We next step through the
        //   sequence of possible stream states (valid, empty, invalid,
        //   incomplete, and corrupted), appropriately selecting data sets as
        //   described below.  In all cases, exception neutrality is confirmed
        //   using the specially instrumented 'bdex_TestInStream' and a pair of
        //   standard macros, 'BEGIN_BDEX_EXCEPTION_TEST' and
        //   'END_BDEX_EXCEPTION_TEST', which configure the
        //   'bdex_TestInStream' object appropriately in a loop.
        //
        // Plan:
        //   PRELIMINARY MEMBER FUNCTION TEST
        //     First test 'maxSupportedBdexVersion' explicitly, and then
        //     perform a trivial direct test of the 'outStream' and 'inStream'
        //     methods (the rest of the testing will use the stream operators).
        //
        //   VALID STREAMS
        //     For the set S of globally-defined test values, use all
        //     combinations (u, v) in the cross product S X S, stream the
        //     value of v into (a temporary copy of) u and assert u == v.
        //
        //   EMPTY AND INVALID STREAMS
        //     For each u in S, create a copy and attempt to stream into it
        //     from an empty and then invalid stream.  Verify after each try
        //     that the object is unchanged and that the stream is invalid.
        //
        //   INCOMPLETE (BUT OTHERWISE VALID) DATA
        //     Write 3 distinct objects to an output stream buffer of total
        //     length N.  For each partial stream length from 0 to N - 1,
        //     construct an input stream and attempt to read into objects
        //     initialized with distinct values.  Verify values of objects
        //     that are either successfully modified or left entirely
        //     unmodified, and that the stream became invalid immediately after
        //     the first incomplete read.  Finally ensure that each object
        //     streamed into is in some valid state by assigning it a distinct
        //     new value and testing for equality.
        //
        //   CORRUPTED DATA
        //     Use the underlying stream package to simulate a typical valid
        //     (control) stream and verify that it can be streamed in
        //     successfully.  Then for each data field in the stream (beginning
        //     with the version number), provide one or more similar tests with
        //     that data field corrupted.  After each test, verify that the
        //     object is in some valid state after streaming, and that the
        //     input stream has gone invalid.
        //
        //   Finally, tests of the explicit wire format will be performed.
        //
        // Testing:
        //   static int maxSupportedBdexVersion() const;
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdex' STREAMING FUNCTIONALITY" << endl
                          << "======================================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // scalar and array object values for various stream tests

        const bdet_Date V_DATE_A(   1,  1,  1);
        const bdet_Date V_DATE_B(1776,  7,  4);
        const bdet_Date V_DATE_C(1956,  4, 30);
        const bdet_Date V_DATE_D(1958,  4, 30);
        const bdet_Date V_DATE_E(2002,  3, 25);
        const bdet_Date V_DATE_F(9999, 12, 31);


        Obj va; const Obj& VA = va;
        va.setDateTz(V_DATE_A, -1439);
        Obj vb; const Obj& VB = vb;
        vb.setDateTz(V_DATE_B, -1*60);
        Obj vc; const Obj& VC = vc;
        vc.setDateTz(V_DATE_C, 0);
        Obj vd; const Obj& VD = vd;
        vd.setDateTz(V_DATE_D, 1*60);
        Obj ve; const Obj& VE = ve;
        ve.setDateTz(V_DATE_E, 1380);
        Obj vf; const Obj& VF = vf;
        vf.setDateTz(V_DATE_F, 1439);

        const int NUM_VALUES = 6;
        const Obj VALUES[NUM_VALUES] = { va, vb, vc, vd, ve, vf };
        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout <<
            "\nTesting 'streamOut' and (valid) 'streamIn' functionality."
                          << endl;

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            if (verbose) cout << "\tusing object syntax:" << endl;
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
            if (verbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion());
        }

        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const int VERSION = Obj::maxSupportedBdexVersion();

            if (veryVerbose) { T_ P(VC) }

            const Obj X(VC);
            Out       out;

            X.bdexStreamOut(out, VERSION);

            const char *const OD  = out.data();
            const int         LOD = out.length();
            In                in(OD, LOD);              ASSERT(in);
                                                        ASSERT(!in.isEmpty());
            Obj               t(VA);                    ASSERT(X != t);

            in.setSuppressVersionCheck(1);  // needed for direct method test
            t.bdexStreamIn(in, VERSION);                ASSERT(X == t);
            ASSERT(in);                                 ASSERT(in.isEmpty());
        }

        if (verbose) cout <<
            "\nThorough test using stream functions." << endl;
        {
            const int VERSION = Obj::maxSupportedBdexVersion();

            for (int i = 0; i < NUM_VALUES; ++i) {
                if (veryVerbose) { T_ P(i) }

                const Obj X(VALUES[i]);
                Out out;
                bdex_OutStreamFunctions::streamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    if (veryVerbose) { T_ T_ P(j) }

                    In in(OD, LOD);
                    In &testInStream = in;  // Alias needed by
                                            // BEGIN_BDEX_EXCEPTION_TEST

                    in.setSuppressVersionCheck(1);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());

                    Obj t(VALUES[j]);
                    if (veryVerbose) { T_ T_ P_(X) P(t) }
                    BEGIN_BDEX_EXCEPTION_TEST {
                      in.reset();
                      LOOP2_ASSERT(i, j, X == t == (i == j));
                      bdex_InStreamFunctions::streamIn(in, t, VERSION);
                    } END_BDEX_EXCEPTION_TEST
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) cout <<
            "\nTesting streamIn functionality via bdex functions." << endl;

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            const int VERSION = Obj::maxSupportedBdexVersion();

            Out out;
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                if (veryVerbose) { T_ P(i) }

                In in(OD, LOD);
                In &testInStream = in;  // Alias needed by
                                        // BEGIN_BDEX_EXCEPTION_TEST

                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);           LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty stream leaves the stream
                // invalid and the object set to a valid value.

                const Obj X(VALUES[i]);  Obj t1(X);  LOOP_ASSERT(i, X == t1);
                BEGIN_BDEX_EXCEPTION_TEST {
                  in.reset();
                  bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                  LOOP_ASSERT(i, !in);

                  LOOP_ASSERT(i, t1.localDate() == bdet_Date(1, 1, 3));
                  LOOP_ASSERT(i, t1.offset() == -3);

                  // At this point the stream is invalid.  Streaming from an
                  // invalid stream should leave the target object unaltered.

                  Obj t2(t1);

                  bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                  LOOP_ASSERT(i, !in);
                  LOOP_ASSERT(i, t1 == t2);
                } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const int VERSION = Obj::maxSupportedBdexVersion();

            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;

            Out out;
            bdex_OutStreamFunctions::streamOut(out, X1, VERSION);
            const int LOD1 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X2, VERSION);
            const int LOD2 = out.length();
            bdex_OutStreamFunctions::streamOut(out, X3, VERSION);
            const int LOD  = out.length();
            const char *const OD = out.data();

            for (int i = 0; i < LOD; ++i) {
                if (veryVerbose) { T_ P(i) }

                In in(OD, i);
                In &testInStream = in; // Alias needed by
                                       // BEGIN_BDEX_EXCEPTION_TEST

                in.setSuppressVersionCheck(1);

                BEGIN_BDEX_EXCEPTION_TEST {
                  in.reset();
                  LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                  Obj t1(W1), t2(W2), t3(W3);

                  if (i < LOD1) {
                      bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                      LOOP_ASSERT(i, !in);

                      // Stream was valid, became invalid because tried to read
                      // over the end.  Object has undetermined valid value by
                      // contract (03JAN0001+00:03 by implementation).

                      LOOP_ASSERT(i, t1.localDate() == bdet_Date(1, 1, 3))
                      LOOP_ASSERT(i, t1.offset() == -3);

                      // Stream is invalid.  Target object will be unmodified
                      // after streaming operation.

                      bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                      LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W2 == t2);
                      bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                      LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                  }
                  else if (i < LOD2) {
                      bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                      LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                      bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                      LOOP_ASSERT(i, !in);

                      // Stream was valid, became invalid because tried to read
                      // over the end.  Object has undetermined valid value by
                      // contract (03JAN0001+00:03 by implementation).


                      LOOP_ASSERT(i, t2.localDate() == bdet_Date(1, 1, 3))
                      LOOP_ASSERT(i, t2.offset() == -3);

                      // Stream is invalid.  Target object will be unmodified
                      // after streaming operation.

                      bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                      LOOP_ASSERT(i, !in);  LOOP_ASSERT(i, W3 == t3);
                  }
                  else {
                      bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                      LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X1 == t1);
                      bdex_InStreamFunctions::streamIn(in, t2, VERSION);
                      LOOP_ASSERT(i,  in);  LOOP_ASSERT(i, X2 == t2);
                      bdex_InStreamFunctions::streamIn(in, t3, VERSION);
                      LOOP_ASSERT(i, !in);

                      // Stream was valid, became invalid because tried to read
                      // over the end.  Object has undetermined valid value by
                      // contract (03JAN0001+00:03 by implementation).


                      LOOP_ASSERT(i, t3.localDate() == bdet_Date(1, 1, 3))
                      LOOP_ASSERT(i, t3.offset() == -3);
                  }
                } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;
        {
            const bdet_Date ORIG_DATE(1, 1, 2);
            const bdet_Date  NEW_DATE(1, 1, 3);

            const int ORIG_OFFSET = 1;
            const int  NEW_OFFSET = 2;

            const Obj W;                         // default value
            const Obj X(ORIG_DATE, ORIG_OFFSET); // original (control) value
            const Obj Y(NEW_DATE, NEW_OFFSET);   // new (streamed-out) value

            const int SERIAL_Y = 3;    // internal rep.of 'bdet_Date'

            if (verbose) cout << "\t\tGood stream (for control)." << endl;
            {
                const char VERSION = 1;

                Out out;

                bdex_OutStreamFunctions::streamOut(out, Y, VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
                In in(OD, LOD);  ASSERT(in);
                in.setSuppressVersionCheck(1);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                ASSERT(in);
                ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
            }

            if (verbose) cout << "\t\tBad version (too small) ." << endl;
            {
                const char VERSION = 0; // too small ('version' must be >= 1)

                Out out;
                bdex_OutStreamFunctions::streamOut(out, Y, VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
                In in(OD, LOD);  ASSERT(in);
                in.setSuppressVersionCheck(1);
                in.setQuiet(!veryVerbose);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                ASSERT(!in);
                ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            }
            if (verbose) cout << "\t\tBad version (too large) ." << endl;
            {
                const char VERSION = 5 ; // too large (max DateTz version is 1)

                Out out;
                bdex_OutStreamFunctions::streamOut(out, Y, VERSION);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
                In in(OD, LOD);  ASSERT(in);
                in.setSuppressVersionCheck(1);
                in.setQuiet(!veryVerbose);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                ASSERT(!in);
                ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            }
            if (verbose) cout << "\t\tBad 'date'." << endl;
            {
                const char VERSION = 1;

                const bdet_Date TEMP_DATE(1, 1, 1);  // valid date

                Out out;
                out.putInt24(0);
                out.putInt32(1339);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t); ASSERT(X == t);  ASSERT(Y != t);
                In in(OD, LOD);  ASSERT(in);
                in.setSuppressVersionCheck(1);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                ASSERT(!in);

                if(veryVerbose) { T_ P(t) }

                ASSERT(Obj::isValid(t.localDate(), t.offset()));
            }

            if (verbose) cout << "\t\tGood 'date' but bad offset." << endl;
            {
                const char VERSION = 1;

                const bdet_Date TEMP_DATE(1, 1, 1);  // valid date

                Out out;
                bdex_OutStreamFunctions::streamOut(out, TEMP_DATE, VERSION);

                out.putInt32(-1441);  // Stream out invalid offset value.

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t); ASSERT(X == t);  ASSERT(Y != t);
                In in(OD, LOD);  ASSERT(in);
                in.setSuppressVersionCheck(1);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                ASSERT(!in);

                if(veryVerbose) { T_ P(t) }

                ASSERT(-3 == t.offset());  // Implementation defined value
                ASSERT(Obj::isValid(t.localDate(), t.offset()));
            }

            if (verbose) cout << "\t\tGood 'date' but bad offset." << endl;
            {
                const char VERSION = 1;

                const bdet_Date TEMP_DATE(1, 1, 1);  // valid date

                Out out;
                bdex_OutStreamFunctions::streamOut(out, TEMP_DATE, VERSION);

                out.putInt32(1441);  // Stream out invalid offset value.

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t); ASSERT(X == t);  ASSERT(Y != t);
                In in(OD, LOD);  ASSERT(in);
                in.setSuppressVersionCheck(1);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                ASSERT(!in);

                if(veryVerbose) { T_ P(t) }

                ASSERT(-3 == t.offset());  // Implementation defined value
                ASSERT(Obj::isValid(t.localDate(), t.offset()));
            }


            if (verbose) cout << "\nWire format direct tests." << endl;
            {
                static const struct {
                    int         d_lineNum;  // source line number
                    int         d_year;     // specification year
                    int         d_month;    // specification month
                    int         d_day;      // specification day
                    int         d_offset;   // specification offset
                    int         d_version;  // version to stream with
                    int         d_length;   // expect output length
                    const char *d_fmt_p;    // expected output format
                } DATA[] = {
                    //LINE YEAR MONTH DAY OFFSET VER LEN FMT
                    //---- ---- ----- --- ------ --- --- ---------------
                    { L_,  1400,   10,  2,    0,   0,  0,""             },
                    { L_,  2002,    8, 27,    1,   0,  0,""             },

                    { L_,  1400,   10,  2,    0,   1,  7,
                                              "\x07\xcd\x1c\x00\x00\x00\x00" },

                    { L_,  2002,    8, 27, 1439,   1,  7,
                                              "\x0b\x27\xd3\x00\x00\x05\x9f" }
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int   LINE   = DATA[ti].d_lineNum;
                    const int   YEAR   = DATA[ti].d_year;
                    const int   MONTH  = DATA[ti].d_month;
                    const int   DAY    = DATA[ti].d_day;
                    const int   OFFSET = DATA[ti].d_offset;
                    const int   VER    = DATA[ti].d_version;
                    const int   LEN    = DATA[ti].d_length;
                    const char *FMT    = DATA[ti].d_fmt_p;

                    if (veryVerbose) { T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY)
                                            P_(OFFSET) P_(VER) P_(LEN) P(FMT) }

                    const bdet_Date TEMP_DATE(YEAR, MONTH, DAY);

                    Obj mX;  const Obj& X = mX;
                    mX.setDateTz(TEMP_DATE, OFFSET);
                    bdex_ByteOutStream out;  X.bdexStreamOut(out, VER);


                    LOOP2_ASSERT(LINE, out.length(), LEN == out.length());

                    if (veryVeryVerbose && memcmp(out.data(), FMT, LEN)) {
                        T_ T_

                        const char *hex = "0123456789abcdef";
                        for (int j = 0; j < out.length(); ++j) {
                            cout << "\\x"
                                 << hex[(unsigned char)*
                                              (out.data() + j) >> 4]
                                 << hex[(unsigned char)*
                                           (out.data() + j) & 0x0f];
                        }
                        cout << endl;
                    }

                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(),
                                                  FMT,
                                                  LEN));

                    Obj mY;  const Obj& Y = mY;
                    if (LEN) {  // version is supported
                        bdex_ByteInStream in(out.data(),
                                             out.length());
                        mY.bdexStreamIn(in, VER);
                    }
                    else {  // version is not supported
                        mY = X;
                        bdex_ByteInStream in;
                        mY.bdexStreamIn(in, VER);
                        LOOP_ASSERT(LINE, !in);
                    }
                    LOOP_ASSERT(LINE, X == Y);
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must be
        //   assignable to itself.
        //
        // Plan:
        //   Specify a set S of (unique) objects with substantial and varied
        //   differences in value.  Construct and initialize all combinations
        //   (u, v) in the cross product S x S, copy construct a control w from
        //   v, assign v to u, and assert that w == u and w == v.  Then test
        //   aliasing by copy constructing a control w from each u in S,
        //   assigning u to itself, and verifying that w == u.
        //
        // Testing:
        //   bdet_Date& operator=(const bdet_Date& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING ASSIGNMENT OPERATOR" << endl
                                  << "===========================" << endl;

        if (verbose) cout << "\nTesting Assignment u = V." << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day; int d_offset;
            } DATA[] = {
                // year  mo day      off
                // ====  == ===    =======
                {    1,  1,  1,    0     }, {   10,  4,  5,    1     },
                {  100,  6,  7,   -1439  }, { 1000,  8,  9,    1439  },
                { 2000,  2, 29,   -1     }, { 2002,  7,  4,    1380  },
                { 2003,  8,  5,   -1380  }, { 2004,  9,  3,    5*60  },
                { 2020,  9,  9,   -5*60  }, { 9999, 12, 31,   -5*60  }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int V_YEAR   = DATA[i].d_year;
                const int V_MONTH  = DATA[i].d_month;
                const int V_DAY    = DATA[i].d_day;
                const int V_OFFSET = DATA[i].d_offset;

                if (veryVerbose) {
                    T_ P_(V_YEAR) P_(V_MONTH) P_(V_DAY) P(V_OFFSET)
                }

                bdet_Date tempDate(V_YEAR, V_MONTH, V_DAY);
                const bdet_Date& V_TEMP_DATE = tempDate;

                Obj v;  const Obj& V = v;
                v.setDateTz(V_TEMP_DATE, V_DAY);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int U_YEAR   = DATA[j].d_year;
                    const int U_MONTH  = DATA[j].d_month;
                    const int U_DAY    = DATA[j].d_day;
                    const int U_OFFSET = DATA[j].d_offset;

                    if (veryVerbose) {
                        T_ T_ P_(U_YEAR) P_(U_MONTH) P_(U_DAY) P(U_OFFSET)
                    }

                    bdet_Date tempDate(U_YEAR, U_MONTH, U_DAY);
                    const bdet_Date& U_TEMP_DATE = tempDate;

                    Obj u;  const Obj& U = u;
                    u.setDateTz(U_TEMP_DATE, U_OFFSET);

                    Obj w(V);  const Obj &W = w;          // control

                    if (veryVeryVerbose) { T_ T_ T_ P_(V) P_(U) P(W) }

                    u = V;

                    if (veryVeryVerbose) { T_ T_ T_ P_(V) P_(U) P(W) }
                    LOOP2_ASSERT(i, j, W == U);
                    LOOP2_ASSERT(i, j, W == V);
                }
            }

            if (verbose) cout << "\nTesting assignment u = u (Aliasing)."
                              << endl;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P_(DAY) P(OFFSET) }

                bdet_Date tempDate(YEAR, MONTH, DAY);
                const bdet_Date& TEMP_DATE = tempDate;

                Obj u;  const Obj& U = u;
                u.setDateTz(TEMP_DATE, OFFSET);

                Obj w(U);  const Obj &W = w;              // control

                if (veryVeryVerbose) { T_ T_ P_(U) P(W) }
                LOOP_ASSERT(i, W == U);

                u = u;

                if (veryVeryVerbose) { T_ T_ P_(U) P(W) }
                LOOP_ASSERT(i, W == U);
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //   Any value must be able to be copy constructed without affecting
        //   its argument.
        //
        // Plan:
        //   Specify a set S whose elements have substantial and varied
        //   differences in value.  For each element in S, construct and
        //   initialize identically valued objects w and x using tested
        //   methods.  Then copy construct an object y from x, and use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   bdet_DateTz(const bdet_DateTz& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Copy Constructor" << endl
                                  << "========================" << endl;

        if (verbose) cout << "\nTesting copy constructor." << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day; int d_offset;
            } DATA[] = {
                //    year  mo day      off
                //    ====  == ===    =======
                    {    1,  1,  1,    0     },
                    {   10,  4,  5,    1     },
                    {  100,  6,  7,   -1439  },
                    { 1000,  8,  9,    1439  },
                    { 2000,  2, 29,   -1     },
                    { 2002,  7,  4,    1380  },
                    { 2003,  8,  5,   -1380  },
                    { 2004,  9,  3,    5*60  },
                    { 9999, 12, 31,   -5*60  }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P_(DAY) P(OFFSET) }

                bdet_Date tempDate(YEAR, MONTH, DAY);
                Obj w;  const Obj& W = w;           // control

                int status = w.validateAndSetDateTz(tempDate, OFFSET);
                LOOP_ASSERT(i, 0 == status);  // success

                Obj x;  const Obj& X = x;
                status = x.validateAndSetDateTz(tempDate, OFFSET);

                Obj y(X);  const Obj &Y = y;

                if (veryVeryVerbose) { T_ T_ P_(W) P_(X) P(Y) }
                LOOP_ASSERT(i, X == W);
                LOOP_ASSERT(i, Y == W);
            }
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS:
        //
        // Concerns:
        //   Any subtle variation in value must be detected by the equality
        //   operators.  The test data have variations in each input parameter,
        //   even though tested methods convert the input before the underlying
        //   equality operators are invoked.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        // Testing:
        //   operator==(const bdet_Date&, const bdet_Date&);
        //   operator!=(const bdet_Date&, const bdet_Date&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Equality Operators" << endl
                                  << "==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in S X S." << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day;  int d_offset;
            } DATA[] = {
                {    1,  1,  1, 0     }, {    1,  1,  2, 0     },
                {    1,  2,  1, 0     }, {    2,  1,  1, 1439  },
                {    1,  1,  1, -1    }, {    1,  1,  2, 1     },
                {    1,  2,  1, -1439 }, {    2,  1,  1, 1438  },

                { 9998, 12, 31, 0     }, { 9999, 11, 30, 0     },
                { 9999, 12, 30, 0     }, { 9999, 12, 31, 0     },
                { 9998, 12, 31, 1     }, { 9999, 11, 30, -1439 },
                { 9999, 12, 30, -1    }, { 9999, 12, 31, -143  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int U_YEAR   = DATA[i].d_year;
                const int U_MONTH  = DATA[i].d_month;
                const int U_DAY    = DATA[i].d_day;
                const int U_OFFSET = DATA[i].d_offset;

                if (veryVerbose) { T_ P_(U_YEAR) P_(U_MONTH) P_(U_DAY)
                                                                  P(U_OFFSET) }

                const bdet_Date U_DATE(U_YEAR, U_MONTH, U_DAY);
                Obj u;  const Obj& U = u;
                u.setDateTz(U_DATE, U_OFFSET);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int V_YEAR   = DATA[j].d_year;
                    const int V_MONTH  = DATA[j].d_month;
                    const int V_DAY    = DATA[j].d_day;
                    const int V_OFFSET = DATA[j].d_offset;

                    if (veryVerbose) { T_ T_ P_(V_YEAR) P_(V_MONTH) P_(V_DAY)
                                                                  P(V_OFFSET) }

                    const bdet_Date V_DATE(V_YEAR, V_MONTH, V_DAY);
                    Obj v;  const Obj& V = v;
                    v.setDateTz(V_DATE, V_OFFSET);

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
        // TESTING PRINT
        //
        // Plan:
        //   Test that the 'print' method produces the expected results for
        //   various values of 'level' and 'spacesPerLevel'.  The date format
        //   is already extensively tested in 'bdet_Date', so it will not be
        //   exhausted here.  Testing the 'operator<<' using the 'print' method
        //   as an oracle.
        //
        // Testing:
        //   ostream& print(ostream& os, int level = 0, int spl = 4) const;
        //   bsl::ostream& operator<<(bsl::ostream&, const bdet_Date&);
        // --------------------------------------------------------------------

        if (veryVerbose) cout << "Testing 'print'." << endl;
        {
            const int LMT = 60 * 100;
            static const struct {
                int         d_lineNum;  // source line number
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  indent  spaces  offset      format
                //----  ------  ------  ------      --------------
                { L_,    0,      -1,     0,         "01JAN0001+0000"      },
                { L_,    0,       0,     15,        "01JAN0001+0015\n"    },
                { L_,    0,       2,     60,        "01JAN0001+0100\n"    },
                { L_,    1,       1,     90,        " 01JAN0001+0130\n"   },
                { L_,    1,       2,     -20,       "  01JAN0001-0020\n"  },
                { L_,   -1,       2,     -330,      "01JAN0001-0530\n"    },
                { L_,   -2,       1,     311,       "01JAN0001+0511\n"    },
                { L_,    2,       1,     1439,      "  01JAN0001+2359\n"  },
                { L_,    1,       3,     -1439,     "   01JAN0001-2359\n" },

// TBD
#if 0
                // The following elements test expected undefined behavior
                { L_,    0,      -1,     1440,      "01JAN0001+2400"    },
                { L_,    0,      -1,     -1440,     "01JAN0001-2400"    },
                { L_,    0,      -1,     1441,      "01JAN0001+2401"    },
                { L_,    0,      -1,     -1441,     "01JAN0001-2401"    },
                { L_,    0,      -1,     LMT-60,    "01JAN0001+9900"    },
                { L_,    0,      -1,     -LMT+60,   "01JAN0001-9900"    },
                { L_,    0,      -1,     LMT-1,     "01JAN0001+9959"    },
                { L_,    0,      -1,     -LMT+1,    "01JAN0001-9959"    },
                { L_,    0,      -1,     LMT,       "01JAN0001+XX00"    },
                { L_,    0,      -1,     -LMT,      "01JAN0001-XX00"    },
                { L_,    0,      -1,     LMT+11,    "01JAN0001+XX11"    },
                { L_,    0,      -1,     -LMT-11,   "01JAN0001-XX11"    },
                { L_,    0,      -1,     1000*LMT,  "01JAN0001+XX00"    },
                { L_,    0,      -1,     -1000*LMT, "01JAN0001-XX00"    },
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
                const int         LI  = DATA[ti].d_lineNum;
                const int         IND = DATA[ti].d_indent;
                const int         SPL = DATA[ti].d_spaces;
                const int         OFF = DATA[ti].d_offset;
                const char *const FMT = DATA[ti].d_fmt_p;

                bdet_Date date; // 01JAN0001
                Obj mX(date, OFF);  const Obj& X = mX;
                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (veryVerbose) cout << "EXPECTED FORMAT:" << endl<<FMT<<endl;
                ostrstream out1(buf1, SIZE);  X.print(out1, IND, SPL) << ends;
                ostrstream out2(buf2, SIZE);  X.print(out2, IND, SPL) << ends;
                if (veryVerbose) cout << "ACTUAL FORMAT:" << endl<<buf1<<endl;

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LI, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LI, Z1 == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LI, Z2 == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LI, 0 == strcmp(buf1, FMT));
                LOOP_ASSERT(LI, 0 == strcmp(buf2, FMT));
                LOOP_ASSERT(LI, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ, REST));
                LOOP_ASSERT(LI, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ, REST));
            }
        }

        if (verbose) cout << "\nTesting 'print' with "
                             "manipulators and left alignment." << endl;
        {
            static const struct {
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
               //indent spaces offset  format
               //------ ------ ------  --------------
               {  0,     -1,    0,     "01JAN0001+0000@@@@@@@@@@@@@@@@"      },
               {  0,      0,    15,    "01JAN0001+0015@@@@@@@@@@@@@@@@\n"    },
               {  0,      2,    60,    "01JAN0001+0100@@@@@@@@@@@@@@@@\n"    },
               {  1,      1,    90,    " 01JAN0001+0130@@@@@@@@@@@@@@@@\n"   },
               {  1,      2,    -20,   "  01JAN0001-0020@@@@@@@@@@@@@@@@\n"  },
               { -1,      2,    -330,  "01JAN0001-0530@@@@@@@@@@@@@@@@\n"    },
               { -2,      1,    311,   "01JAN0001+0511@@@@@@@@@@@@@@@@\n"    },
               {  2,      1,    1439,  "  01JAN0001+2359@@@@@@@@@@@@@@@@\n"  },
               {  1,      3,    -1439, "   01JAN0001-2359@@@@@@@@@@@@@@@@\n" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 128;  // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Value 1 used for an unset char.
            const char Z2  = 0x00; // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);

            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            const char    FILL_CHAR = '@'; // Used for filling white spaces due
                                           // to 'setw'.
            const int  FORMAT_WIDTH = 30;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const int         OFF  = DATA[ti].d_offset;
                const char *const FMT  = DATA[ti].d_fmt_p;

                bdet_Date date; // 01JAN0001
                Obj mX(date, OFF);  const Obj& X = mX;
                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (veryVerbose) cout << "EXPECTED FORMAT: " << endl << FMT
                                                                       << endl;
                ostrstream out1(buf1, SIZE);
                out1 << left << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                X.print(out1, IND, SPL);
                out1 << ends;

                ostrstream out2(buf2, SIZE);
                out2 << left << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                X.print(out2, IND, SPL);
                out2 << ends;
                if (veryVerbose) cout << "  ACTUAL FORMAT: " << endl << buf1
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
            static const struct {
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
               //indent spaces offset  format
               //------ ------ ------  --------------
               {  0,     -1,    0,     "@@@@@@@@@@@@@@@@01JAN0001+0000"      },
               {  0,      0,    15,    "@@@@@@@@@@@@@@@@01JAN0001+0015\n"    },
               {  0,      2,    60,    "@@@@@@@@@@@@@@@@01JAN0001+0100\n"    },
               {  1,      1,    90,    " @@@@@@@@@@@@@@@@01JAN0001+0130\n"   },
               {  1,      2,    -20,   "  @@@@@@@@@@@@@@@@01JAN0001-0020\n"  },
               { -1,      2,    -330,  "@@@@@@@@@@@@@@@@01JAN0001-0530\n"    },
               { -2,      1,    311,   "@@@@@@@@@@@@@@@@01JAN0001+0511\n"    },
               {  2,      1,    1439,  "  @@@@@@@@@@@@@@@@01JAN0001+2359\n"  },
               {  1,      3,    -1439, "   @@@@@@@@@@@@@@@@01JAN0001-2359\n" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 128;  // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Value 1 used for an unset char.
            const char Z2  = 0x00; // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            const char    FILL_CHAR = '@'; // Used for filling white spaces due
                                           // to 'setw'.

            const int  FORMAT_WIDTH = 30;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const int         OFF  = DATA[ti].d_offset;
                const char *const FMT  = DATA[ti].d_fmt_p;

                bdet_Date date; // 01JAN0001
                Obj mX(date, OFF);  const Obj& X = mX;
                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // Preset buf1 to Z1 values.
                memcpy(buf2, CTRL_BUF2, SIZE); // Preset buf2 to Z2 values.

                if (veryVerbose) cout << "EXPECTED FORMAT: " << endl<< FMT
                                                                        <<endl;
                ostrstream out1(buf1, SIZE);
                out1 << right << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                X.print(out1, IND, SPL);
                out1 << ends;

                ostrstream out2(buf2, SIZE);
                out2 << right << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                X.print(out2, IND, SPL);
                out2 << ends;
                if (veryVerbose) cout << "  ACTUAL FORMAT: " << endl<< buf1
                                                                        <<endl;

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
        if (verbose) cout << "\nTesting 'operator<<'" << endl;
        {
            static const struct {
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                // offset   format
                // ------   --------------
                {  0,      "01JAN0001+0000" },
                {  15,     "01JAN0001+0015" },
                {  60,     "01JAN0001+0100" },
                {  90,     "01JAN0001+0130" },
                {  -20,    "01JAN0001-0020" },
                {  -330,   "01JAN0001-0530" },
                {  311,    "01JAN0001+0511" },
                {  1439,   "01JAN0001+2359" },
                {  -1439,  "01JAN0001-2359" },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 128;  // Must be big enough to hold output string.
            const char Z   = (char) 0xFF;  // Value 1 used for an unset char.

            char mCtrlBuf[SIZE];  memset(mCtrlBuf, Z, SIZE);
            const char *CTRL_BUF = mCtrlBuf;

            const int IND = 0;
            const int SPL = -1;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         OFF  = DATA[ti].d_offset;
                const char *const FMT  = DATA[ti].d_fmt_p;

                bdet_Date date; // 01JAN0001
                Obj mX(date, OFF);  const Obj& X = mX;
                char buf1[SIZE]; char buf2[SIZE];
                memcpy(buf1, CTRL_BUF, SIZE); // Preset buf1 to Z values.
                memcpy(buf2, CTRL_BUF, SIZE); // Preset buf2 to Z values.

                if (veryVerbose) cout << "EXPECTED FORMAT: " << FMT << endl;

                ostrstream out1(buf1, SIZE);
                out1 << X << ends;

                ostrstream out2(buf2, SIZE);
                X.print(out2, IND, SPL);
                out2 << ends;

                if (veryVerbose) cout << "  ACTUAL FORMAT: " << buf1 <<endl;

                if (veryVerbose) cout << "  ORACLE FORMAT: " << buf2 << endl;

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(ti, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(ti, Z == buf1[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, Z == buf2[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(ti, 0 == strcmp(buf1, buf2));
                LOOP_ASSERT(ti, 0 == strcmp(buf1, FMT));
                LOOP_ASSERT(ti, 0 == memcmp(buf1 + SZ, CTRL_BUF + SZ, REST));
                LOOP_ASSERT(ti, 0 == memcmp(buf2 + SZ, CTRL_BUF + SZ, REST));
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //
        // Concerns:
        //   Each individual date field must be correctly forwarded from the
        //   fully-tested underlying utility functions.
        //
        // Plan:
        //   For each of a sequence of unique object values, verify that each
        //   of the direct accessors returns the correct value.
        //
        // Testing:
        //   bdet_Date localDate() const;
        //   int offset() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING BASIC ACCESSORS" << endl
                                  << "=======================" << endl;

        if (verbose) cout << "\n'localDate()', 'offset()'" << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day, d_offset;
            } DATA[] = {
                {    1,  1,  1, -1439 }, {   10,  4,  5,  1439 },
                {  100,  6,  1, -5*60 }, { 1000,  8,  9, -1438 },
                { 1100,  1, 31,  1380 }, { 1200,  2, 15, -1380 },
                { 1300,  3, 31,    -1 }, { 1400,  4, 30,  5*60 },
                { 1600,  6, 30,     0 }, { 1500,  5, 15, -1439 },
                { 1700,  7, 31,  1*60 }, { 1900,  9, 30,     1 },
                { 2000, 10, 31,     0 }, { 2200, 12, 31,  1438 },
                { 2400, 12,  1, -1*60 }, { 9999, 12, 31,  1439 }
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P_(DAY) P(OFFSET) }

                int       y, m, d;

                bdet_Date tempDate(YEAR, MONTH, DAY);
                const bdet_Date& TEMP_DATE = tempDate;

                Obj x;  const Obj& X = x;
                x.setDateTz(TEMP_DATE, OFFSET);

                if (veryVeryVerbose) { T_ P(X) }
                LOOP_ASSERT(i, TEMP_DATE == X.localDate());
                LOOP_ASSERT(i, OFFSET    == X.offset());
            }
        }
      } break;
      case 3: {
        // No generator function testing.

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS:
        //
        // Concerns:
        //   The default constructor must set the correct default values for
        //   each member of the class.  The primary manipulator 'setDateTz'
        //   must correctly invoke 'bdet_Date' constructor and offset provided
        //   must be correctly set.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the basic manipulator
        //   to set its value.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   bdet_DateTz();
        //   void setDateTz(const bdet_Date& localDate, int offset)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING PRIMARY MANIPULATOR" << endl
                                  << "===========================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;
        {
            Obj x;  const Obj& X = x;
            const bdet_Date& LOCAL_DATE = X.localDate();

            ASSERT(1 == LOCAL_DATE.year());
            ASSERT(1 == LOCAL_DATE.month());
            ASSERT(1 == LOCAL_DATE.day());
            ASSERT(0 == X.offset());
        }

        if (verbose) cout << "\nTesting how 'setDateTz' forwards 'bdet_Date'."
                                                                       << endl;
        {
            // First let's make sure that the 'bdet_Date' object is passed down
            // correctly, let's use a 3 different offsets.

            static const struct {
                int d_year;  int d_month;  int d_day; int d_offset;
            } DATA[] = {
                {   1,   1,  1,     0 },
                { 1600,  6, 30,  5*60 },
                { 9999, 12, 31, 23*60 }
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = DATA[i].d_year;
                const int MONTH  = DATA[i].d_month;
                const int DAY    = DATA[i].d_day;
                const int OFFSET = DATA[i].d_offset;

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P_(DAY) P(OFFSET) }

                Obj x;  const Obj& X = x;
                bdet_Date TEMP_DATE(YEAR, MONTH, DAY);
                x.setDateTz(TEMP_DATE, OFFSET);

                if (veryVeryVerbose) { T_ T_ P(X) }

                const bdet_Date& LOCAL_DATE = X.localDate();
                LOOP_ASSERT(i, YEAR   == LOCAL_DATE.year());
                LOOP_ASSERT(i, MONTH  == LOCAL_DATE.month());
                LOOP_ASSERT(i, DAY    == LOCAL_DATE.day());
                LOOP_ASSERT(i, OFFSET == X.offset());
            }
        }

        if (verbose) cout << "\nTesting 'setDateTz' with different offsets."
                                                                       << endl;
        {
            // Now we trust that the 'bdet_Date' construction part is reliable.
            // Let's verify that the offset part works fine.

            const int DATA[] = { -1439, -1339, -60, -1 ,0 ,1, 60, 1339, 1439 };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR   = 1;
                const int MONTH  = 1;
                const int DAY    = 1;
                const int OFFSET = DATA[i];

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P_(DAY) P(OFFSET) }

                Obj x;  const Obj& X = x;
                bdet_Date TEMP_DATE(YEAR, MONTH, DAY);
                x.setDateTz(TEMP_DATE, OFFSET);

                if (veryVeryVerbose) { T_ T_ P(X) }

                const bdet_Date& LOCAL_DATE = X.localDate();
                LOOP_ASSERT(i, YEAR   == LOCAL_DATE.year());
                LOOP_ASSERT(i, MONTH  == LOCAL_DATE.month());
                LOOP_ASSERT(i, DAY    == LOCAL_DATE.day());
                LOOP_ASSERT(i, OFFSET == X.offset());
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
        // 1. Create an object x1 (init.  to VA).   { x1:VA }
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

        const bdet_Date DA(YRA, MOA, DAA),
                        DB(YRB, MOB, DAB),
                        DC(YRC, MOC, DAC);

        const int OA = 60, OB = -300, OC = 270;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1.  Create an object x1 (init.  to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(DA, OA);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta.  Check initial state of x1." << endl;
        ASSERT(DA == X1.localDate());
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
        ASSERT(DA == X2.localDate());
        ASSERT(OA == X2.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3.  Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setDateTz(DB, OB);
        if (verbose) { cout << '\t';  P(X1); }
        ASSERT(DB == X1.localDate());
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
        ASSERT(bdet_Date() == X3.localDate());
        ASSERT(          0 == X3.offset());

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
        ASSERT(bdet_Date() == X4.localDate());
        ASSERT(          0 == X4.offset());

        if (verbose) cout <<
            "\tb.  Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6.  Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setDateTz(DC, OC);
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta.  Check new state of x3." << endl;
        ASSERT(DC == X3.localDate());
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
        ASSERT(DB == X2.localDate());
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
        ASSERT(DC == X2.localDate());
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
        ASSERT(DB == X1.localDate());
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
