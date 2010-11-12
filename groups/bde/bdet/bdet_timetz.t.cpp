// bdet_timetz.t.cpp                                                  -*-C++-*-

#include <bdet_timetz.h>

#include <bdet_time.h>

#include <bsls_platformutil.h>           // for testing only

#include <bdex_testinstream.h>           // for testing only
#include <bdex_testoutstream.h>          // for testing only
#include <bdex_testinstreamexception.h>  // for testing only
#include <bdex_byteoutstream.h>          // for testing only
#include <bdex_byteinstream.h>           // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                   TEST PLAN
//                                   Overview
//                                   --------
//
// ----------------------------------------------------------------------------
// CLASS METHODS
// [10] static bool isValid(const bdet_Time& localTime, int offset);
// [ 7] static int maxSupportedBdexVersion() const;
//
// CREATORS
// [ 2] bdet_TimeTz();
// [ 7] bdet_TimeTz(const bdet_TimeTz& original);
// [13] bdet_TimeTz(const bdet_Time& localTime, int offset);
//
// MANIPULATORS
// [2]  void setTimeTz(const bdet_Time& localTime, int offset);
// [10] bool validateAndSetTimeTz(const bdet_Time& localTime, int offset);
//
// ACCESSORS
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [11] bdet_Time gmtTime() const;
// [ 4] bdet_Time localTime() const;
// [ 4] int offset() const;
// [ 5] bsl::ostream& print(bsl::ostream& stream, int l, int spl) const;
//
// FREE OPERATORS
// [ 6] bool operator==(const bdet_TimeTz& lhs, const bdet_TimeTz& rhs);
// [ 6] bool operator!=(const bdet_TimeTz& lhs, const bdet_TimeTz& rhs);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const bdet_TimeTz&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [12] USAGE EXAMPLE
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
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdet_TimeTz        Obj;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

///Example 2
///-------
// Let us provide a more concrete example where we use a 'bdet_TimeTz'
// object.  Suppose that we want to create a time oracle struct that returns
// the current time in a particular city.  The struct provides a simple
// function taking an enum value representing a city, and returns a
// 'bdet_TimeTz' object with the current time in that city.
//..
                          // =================
                          // struct TimeOracle
                          // =================

struct TimeOracle {
    // This struct provides a function that returns the current time in a
    // user-specified city.

    // PRIVATE CLASS METHODS
    static bdet_Time getCurrentGMTTime();
        // Return the current GMT time.

    enum { MINUTES_PER_HOUR = 60 };

  public:
    // TYPES
    enum CITY {
        // This enumeration stores the provides from GMT corresponding to
        // various cities.

        LOS_ANGELES      = -8 * MINUTES_PER_HOUR,
        LOS_ANGELES_DST  = -7 * MINUTES_PER_HOUR,
        PHOENIX          = -7 * MINUTES_PER_HOUR,
        PHOENIX_DST      = -7 * MINUTES_PER_HOUR,
        CHICAGO          = -6 * MINUTES_PER_HOUR,
        CHICAGO_DST      = -5 * MINUTES_PER_HOUR,
        NEW_YORK         = -5 * MINUTES_PER_HOUR,
        NEW_YORK_DST     = -4 * MINUTES_PER_HOUR
    };

    // CLASS METHODS
    static bdet_TimeTz getLocalTime(CITY city);
        // Return the current time in the time zone corresponding to the
        // specified 'city'.
};

                          // -----------------
                          // struct TimeOracle
                          // -----------------

#include <sys/types.h>         // type definitions
#include <bsl_ctime.h>         // various time functions

// PRIVATE CLASS METHODS
bdet_Time TimeOracle::getCurrentGMTTime()
{
    bsl::time_t currentTime = bsl::time(0);
    bsl::tm     gmtTime;

#if defined(BSLS_PLATFORM__OS_WINDOWS) || ! defined(BDE_BUILD_TARGET_MT)
    gmtTime = *bsl::gmtime(&currentTime);
#else
    gmtime_r(&currentTime, &gmtTime);
#endif

    return bdet_Time(gmtTime.tm_hour, gmtTime.tm_min, gmtTime.tm_sec);
}

// CLASS METHODS
bdet_TimeTz TimeOracle::getLocalTime(CITY city)
{
    bdet_Time localTime(getCurrentGMTTime());
    localTime.addMinutes((int) city);
    return bdet_TimeTz(localTime, (int) city);
}
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{

    int test = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;
    bool veryVerbose = argc > 3;
    bool veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;
    bool veryVeryVeryVeryVerbose = argc > 6;

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
// 'bdet_TimeTz' objects.  First we will default construct an object 'timeTz'.
// A default constructed object contains an offset of 0, implying that the
// object represents time in the GMT time zone.  The value of the time is the
// same as that of a default constructed 'bdet_Time' object:
//..
  bdet_TimeTz timeTz1;
  ASSERT(0                   == timeTz1.offset());
  ASSERT(timeTz1.localTime() == bdet_Time());
//..
// Next we set 'timeTz1' to 12:00 noon (12:00:00.000) in the EST time zone
// (GMT-5):
//..
  bdet_Time time1(12, 0, 0, 0);
  bdet_Time time2(time1);
  int       offset1 = -5 * 60;

  timeTz1.setTimeTz(time1, offset1);
  ASSERT(offset1             == timeTz1.offset());
  ASSERT(timeTz1.localTime() != timeTz1.gmtTime());
  ASSERT(timeTz1.localTime() == time1);
  ASSERT(timeTz1.gmtTime()   != time2);

  time2.addMinutes(-offset1);
  ASSERT(timeTz1.gmtTime()   == time2);
//..
// Then we create 'timeTz2' as a copy of 'timeTz1':
//..
  bdet_TimeTz timeTz2(timeTz1);
  ASSERT(offset1             == timeTz2.offset());
  ASSERT(timeTz2.localTime() == time1);
  ASSERT(timeTz2.gmtTime()   == time2);
//..
// We now create a third object, 'timeTz3', representing the time 10:33:25.000
// in the PST time zone (GMT-8):
//..
  bdet_Time time3(10, 33, 25, 0);
  bdet_Time time4(time3);
  int       offset2 = -8 * 60;

  bdet_TimeTz timeTz3(time3, offset2);
  ASSERT(offset2             == timeTz3.offset());
  ASSERT(timeTz3.localTime() != timeTz3.gmtTime());
  ASSERT(timeTz3.localTime() == time3);
  ASSERT(timeTz3.gmtTime()   != time4);

  time4.addMinutes(-offset2);
  ASSERT(timeTz3.gmtTime()   == time4);
//..
// Finally we stream the values of 'timeTz1', 'timeTz2' and 'timeTz3' to
// 'stdout':
//..
if (veryVerbose)
  bsl::cout << timeTz1 << bsl::endl
            << timeTz2 << bsl::endl
            << timeTz3 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  12:00:00.000-0500
//  12:00:00.000-0500
//  10:33:25.000-0800
//..
///Example 2
///---------
//..
// Finally, the following statements produce the time in various time zones:
//..
if (veryVerbose) {
  bsl::cout << "Current New York Time:    "
            << TimeOracle::getLocalTime(TimeOracle::NEW_YORK)
            << bsl::endl;
  bsl::cout << "Current Chicago DST Time: "
            << TimeOracle::getLocalTime(TimeOracle::CHICAGO_DST)
            << bsl::endl;
  bsl::cout << "Current Phoenix Time:     "
            << TimeOracle::getLocalTime(TimeOracle::PHOENIX)
            << bsl::endl;
  bsl::cout << "Current Los Angeles Time: "
            << TimeOracle::getLocalTime(TimeOracle::LOS_ANGELES)
            << bsl::endl;
}
//..
// The result produced is as follows (the actual result might vary based on
// the current time at which the example is run):
//..
// Current New York Time:        10:19:44.000-0500
// Current Chicago DST Time:     10:19:44.000-0500
// Current Phoenix Time:         08:19:44.000-0700
// Current Los Angeles Time:     07:19:44.000-0800
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'isValid' and 'validateAndSetTimeTz'
        //
        // Concerns:
        //   That 'isValid' and 'validateAndSetTimeTz' work.
        //
        // Plan:
        //   The c'tors and manipulators of 'bdet_Time' make it very hard to
        //   build one that is invalid, so we'll mostly trust that
        //   'bdet_Time::isValid' works.  Configure 'bdet_Time' objects with
        //   various values, and try the two routines under test on those
        //   values combined with values of 'offset'.
        // --------------------------------------------------------------------

        enum { MAX_TIMEZONE = 24 * 60 - 1,
               MTZ = MAX_TIMEZONE };

        struct {
            int d_line;
            int d_hour;
            int d_minute;
            int d_second;
            int d_millisecond;
            int d_offset;
            int d_isValid;
        } DATA[] = {
            //    hr  mn sec msec    off  valid
            //    ==  == === ====    ===  =
            { L_,  2,  0,  0,   0,  -4*60, 1 },
            { L_,  0,  2,  0,   0,  -4*60, 1 },
            { L_,  0,  0,  2,   0,  -4*60, 1 },
            { L_,  0,  0,  0,   0,  -4*60, 1 },
            { L_,  2,  0,  0,   0,   -MTZ, 1 },
            { L_,  2,  0,  0,   0,    MTZ, 1 },
            { L_, 24,  0,  0,   0,      0, 1 },
            { L_, 24,  0,  0,   0,      1, 0 },
            { L_, 24,  0,  0,   0,   4*60, 0 },
            { L_, 24,  0,  0,   0,  -4*60, 0 },
            { L_,  2,  0,  0,   0,  24*60, 0 },
            { L_,  2,  0,  0,   0, -24*60, 0 },
            { L_,  2,  0,  0,   0,  99*60, 0 },
            { L_,  2,  0,  0,   0, -99*60, 0 },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int di = 0; di < NUM_DATA; ++di) {
            const int LINE        = DATA[di].d_line;
            const int HOUR        = DATA[di].d_hour;
            const int MINUTE      = DATA[di].d_minute;
            const int SECOND      = DATA[di].d_second;
            const int MILLISECOND = DATA[di].d_millisecond;
            const int OFFSET      = DATA[di].d_offset;
            const int IS_VALID    = DATA[di].d_isValid;

            bdet_Time time;
            time.setTime(HOUR, MINUTE, SECOND, MILLISECOND);

            LOOP_ASSERT(LINE,
                        !IS_VALID == !bdet_TimeTz::isValid(time, OFFSET));

            bdet_TimeTz timeTz;
            LOOP_ASSERT(LINE,
                        IS_VALID == 
                                   !timeTz.validateAndSetTimeTz(time, OFFSET));

            if (!IS_VALID) {
                const bdet_TimeTz nullTimeTz;
                LOOP_ASSERT(LINE, nullTimeTz == timeTz);
            }
            else {
                const bdet_Time time = timeTz.localTime();
                LOOP_ASSERT(LINE, time.hour() == HOUR);
                LOOP_ASSERT(LINE, time.minute() == MINUTE);
                LOOP_ASSERT(LINE, time.second() == SECOND);
                LOOP_ASSERT(LINE, time.millisecond() == MILLISECOND);

                LOOP_ASSERT(LINE, timeTz.offset() == OFFSET);
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING PRINT
        //
        // Plan:
        //   Test that the 'print' method produces the expected results for
        //   various values of 'level' and 'spacesPerLevel'.  The time format
        //   is already extensively tested in 'bdet_Time', so it will not be
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
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //indent  spaces  offset      format
                //------  ------  ------      --------------
                {  0,      -1,     0,         "17:00:00.000+0000"    },
                {  0,       0,     15,        "17:00:00.000+0015"    },
                {  0,       2,     60,        "17:00:00.000+0100"    },
                {  1,       1,     90,        " 17:00:00.000+0130"   },
                {  1,       2,     -20,       "  17:00:00.000-0020"  },
                { -1,       2,     -330,      "17:00:00.000-0530"    },
                { -2,       1,     311,       "17:00:00.000+0511"    },
                {  2,       1,     1439,      "  17:00:00.000+2359"  },
                {  1,       3,     -1439,     "   17:00:00.000-2359" },

// TBD
#if 0
                // The following elements test expected undefined behavior
                {  0,      -1,     1440,      "24:00:00.000+2400"    },
                {  0,      -1,     -1440,     "24:00:00.000-2400"    },
                {  0,      -1,     1441,      "24:00:00.000+2401"    },
                {  0,      -1,     -1441,     "24:00:00.000-2401"    },
                {  0,      -1,     LMT-60,    "24:00:00.000+9900"    },
                {  0,      -1,     -LMT+60,   "24:00:00.000-9900"    },
                {  0,      -1,     LMT-1,     "24:00:00.000+9959"    },
                {  0,      -1,     -LMT+1,    "24:00:00.000-9959"    },
                {  0,      -1,     LMT,       "24:00:00.000+XX00"    },
                {  0,      -1,     -LMT,      "24:00:00.000-XX00"    },
                {  0,      -1,     LMT+11,    "24:00:00.000+XX11"    },
                {  0,      -1,     -LMT-11,   "24:00:00.000-XX11"    },
                {  0,      -1,     1000*LMT,  "24:00:00.000+XX00"    },
                {  0,      -1,     -1000*LMT, "24:00:00.000-XX00"    },
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

                bdet_Time time(17);  // 17:00:00.000
                Obj mX(time, OFF);  const Obj& X = mX;
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
            static const struct {
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                int         d_offset;   // tz offset
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //ind. spc. offset      format
                //---- ---- ------  --------------
                {  0,   -1,  0,     "17:00:00.000+0000@@@@@@@@@@@@@"    },
                {  0,    0,  15,    "17:00:00.000+0015@@@@@@@@@@@@@"    },
                {  0,    2,  60,    "17:00:00.000+0100@@@@@@@@@@@@@"    },
                {  1,    1,  90,    " 17:00:00.000+0130@@@@@@@@@@@@@"   },
                {  1,    2,  -20,   "  17:00:00.000-0020@@@@@@@@@@@@@"  },
                { -1,    2,  -330,  "17:00:00.000-0530@@@@@@@@@@@@@"    },
                { -2,    1,  311,   "17:00:00.000+0511@@@@@@@@@@@@@"    },
                {  2,    1,  1439,  "  17:00:00.000+2359@@@@@@@@@@@@@"  },
                {  1,    3,  -1439, "   17:00:00.000-2359@@@@@@@@@@@@@" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 128;  // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Value 1 used for an unset char.
            const char Z2  = 0x00; // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);

            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            const char    FILL_CHAR = '@'; // Used for filling whitespaces due
                                           // to 'setw'.
            const int  FORMAT_WIDTH = 30;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const int         OFF  = DATA[ti].d_offset;
                const char *const FMT  = DATA[ti].d_fmt_p;

                bdet_Time time(17);  // 17:00:00.000
                Obj mX(time, OFF);  const Obj& X = mX;
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
                //ind. spc. offset format
                //---- ---- ------ --------------
                {  0,   -1,  0,    "@@@@@@@@@@@@@17:00:00.000+0000"    },
                {  0,    0,  15,   "@@@@@@@@@@@@@17:00:00.000+0015"    },
                {  0,    2,  60,   "@@@@@@@@@@@@@17:00:00.000+0100"    },
                {  1,    1,  90,   " @@@@@@@@@@@@@17:00:00.000+0130"   },
                {  1,    2,  -20,  "  @@@@@@@@@@@@@17:00:00.000-0020"  },
                { -1,    2,  -330, "@@@@@@@@@@@@@17:00:00.000-0530"    },
                { -2,    1,  311,  "@@@@@@@@@@@@@17:00:00.000+0511"    },
                {  2,    1,  1439, "  @@@@@@@@@@@@@17:00:00.000+2359"  },
                {  1,    3,  -1439,"   @@@@@@@@@@@@@17:00:00.000-2359" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 128;  // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Value 1 used for an unset char.
            const char Z2  = 0x00; // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            const char    FILL_CHAR = '@'; // Used for filling whitespaces due
                                           // to 'setw'.

            const int  FORMAT_WIDTH = 30;

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const int         OFF  = DATA[ti].d_offset;
                const char *const FMT  = DATA[ti].d_fmt_p;

                bdet_Time time(17);  // 17:00:00.000
                Obj mX(time, OFF);  const Obj& X = mX;
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
                //offset      format
                //------      --------------
                {  0,         "17:00:00.000+0000" },
                {  15,        "17:00:00.000+0015" },
                {  60,        "17:00:00.000+0100" },
                {  90,        "17:00:00.000+0130" },
                {  -20,       "17:00:00.000-0020" },
                {  -330,      "17:00:00.000-0530" },
                {  311,       "17:00:00.000+0511" },
                {  1439,      "17:00:00.000+2359" },
                {  -1439,     "17:00:00.000-2359" },
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

                bdet_Time time(17);  // 17:00:00.000
                Obj mX(time, OFF);  const Obj& X = mX;
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

        const int HRA = 1, MIA = 2, SCA = 3;  // h, m, s, ms for VA
        const int HRB = 5, MIB = 6, SCB = 7;  // h, m, s, ms for VB
        const int HRC = 9, MIC = 9, SCC = 9;  // h, m, s, ms for VC

        const bdet_Time TA(HRA, MIA, SCA),
                        TB(HRB, MIB, SCB),
                        TC(HRC, MIC, SCC);

        const int OA = 60, OB = -300, OC = 270;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(TA, OA);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        ASSERT(TA == X1.localTime());
        ASSERT(OA == X1.offset());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        ASSERT(TA == X2.localTime());
        ASSERT(OA == X2.offset());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setTimeTz(TB, OB);
        if (verbose) { cout << '\t';  P(X1); }
        ASSERT(TB == X1.localTime());
        ASSERT(OB == X1.offset());

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
        ASSERT(bdet_Time() == X3.localTime());
        ASSERT(          0 == X3.offset());

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
        ASSERT(bdet_Time() == X4.localTime());
        ASSERT(          0 == X4.offset());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setTimeTz(TC, OC);
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        ASSERT(TC == X3.localTime());
        ASSERT(OC == X3.offset());

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
        ASSERT(TB == X2.localTime());
        ASSERT(OB == X2.offset());

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
        ASSERT(TC == X2.localTime());
        ASSERT(OC == X2.offset());

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
        ASSERT(TB == X1.localTime());
        ASSERT(OB == X1.offset());

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
