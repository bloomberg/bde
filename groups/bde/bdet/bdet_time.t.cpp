// bdet_time.t.cpp                                                    -*-C++-*-

#include <bdet_time.h>

#include <bdex_byteinstream.h>                  // for testing only
#include <bdex_byteoutstream.h>                 // for testing only
#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only
#include <bdex_testoutstream.h>                 // for testing only

#include <bsls_platformutil.h>                  // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_c_time.h>
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                   TEST PLAN
// ----------------------------------------------------------------------------
// [17] bool isValid(int hour, int minute, int second, int ms);
// [ 2] bdet_Time();
// [11] bdet_Time(int hour, int minute, int second = 0, int millisecond = 0);
// [ 7] bdet_Time(const bdet_Time& original);
// [ 2] ~bdet_Time(); (nothing to get rid of)
// [ 9] bdet_Time& operator=(const bdet_Time& rhs);
// [16] bdet_Time& operator+=(const bdet_DatetimeInterval& rhs);
// [16] bdet_Time& operator-=(const bdet_DatetimeInterval& rhs);
// [ 2] void setTime(int hour, int minute = 0, int second = 0, int ms = 0);
// [18] int setTimeIfValid(int hour, int min = 0, int sec = 0, int ms = 0);
// [12] void setHour(int hour);
// [12] void setMinute(int minute);
// [12] void setSecond(int second);
// [12] void setMillisecond(int millisecond);
// [16] int addTime(int hours, int minutes, int seconds, int milliseconds);
// [16] int addHours(int hours);
// [16] int addMinutes(int minutes);
// [16] int addSeconds(int seconds);
// [16] int addMilliseconds(int milliseconds);
// [16] int addInterval(const bdet_DatetimeInterval& interval);
// [ 4] getTime(int *hour, int *minute, int *second, int *millisecond) const;
// [13] getTime(=0, =0, =0, =0);
// [ 4] int hour() const;
// [ 4] int minute() const;
// [ 4] int second() const;
// [ 4] int millisecond() const;
// [ 8] ostream& print(ostream& os, int level = 0, int spl = 4) const;
// [10] int maxSupportedBdexVersion() const;
// [15] bdet_DtInterval operator-(const bdet_Time& ls, const bdet_Time& rs);
// [ 6] bool operator==(const bdet_Time& lhs, const bdet_Time& rhs);
// [ 6] bool operator!=(const bdet_Time& lhs, const bdet_Time& rhs);
// [14] bool operator< (const bdet_Time& lhs, const bdet_Time& rhs);
// [14] bool operator<=(const bdet_Time& lhs, const bdet_Time& rhs);
// [14] bool operator> (const bdet_Time& lhs, const bdet_Time& rhs);
// [14] bool operator>=(const bdet_Time& lhs, const bdet_Time& rhs);
// [ 5] ostream& operator<<(ostream &output, const bdet_Time &time);
// [16] bdet_Time operator+(const bdet_Time&, const bdet_DatetimeInterval&);
// [16] bdet_Time operator+(const bdet_DatetimeInterval&, const bdet_Time&);
// [16] bdet_Time operator-(const bdet_Time&, const bdet_DatetimeInterval&);
// ----------------------------------------------------------------------------
// [ 1] breathing test
// [19] USAGE example
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
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

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T()   cout << "\n\t" << flush;        // Print newline then tab
#define T_()  cout << "\t" << flush;          // Print tab w/o newline

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdet_Time          Obj;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 19: {
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
// The following snippets of code illustrate how to create and use a
// 'bdet_Time' object.  First we create a default-constructed object 't1':
//..
    bdet_Time t1;                ASSERT(24 == t1.hour());
                                 ASSERT( 0 == t1.minute());
                                 ASSERT( 0 == t1.second());
                                 ASSERT( 0 == t1.millisecond());
//.
// Then we set 't1' to the value 2:34pm (14:34:00.000):
//..
    t1.setTime(14, 34);          ASSERT(14 == t1.hour());
                                 ASSERT(34 == t1.minute());
                                 ASSERT( 0 == t1.second());
                                 ASSERT( 0 == t1.millisecond());
//..
// We can also use 'setTimeIfValid' if we are not sure whether the particular
// time we want to set to is a valid 'bdet_Time'.  For example, if we want to
// set the time to 0:15am, but we mistakenly used 24 as the hour instead of 0:
//..
    int ret = t1.setTimeIfValid(24, 15);
                                 ASSERT( 0 != ret);               // "24:15" is
                                                                  // not valid

                                 ASSERT(14 == t1.hour());         // no effect
                                 ASSERT(34 == t1.minute());       // on the
                                 ASSERT( 0 == t1.second());       // object
                                 ASSERT( 0 == t1.millisecond());
//..
// Next we copy-construct 't2' from 't1':
//..
    bdet_Time t2(t1);            ASSERT(t1 == t2);
//..
// We can also add time to an existing time object:
//..
    t2.addMinutes(5);
    t2.addSeconds(7);
                                 ASSERT(14 == t2.hour());
                                 ASSERT(39 == t2.minute());
                                 ASSERT( 7 == t2.second());
                                 ASSERT( 0 == t2.millisecond());
//..
// Subtracting two 'bdet_Time' objects, 't1' from 't2', will yield a
// 'bdet_DatetimeInterval':
//..
    bdet_DatetimeInterval dt = t2 - t1;
                                 ASSERT(307 == dt.totalSeconds());
//..
// Finally we stream the value of 't2' to 'stdout':
//..
if (verbose)
    bsl::cout << t2 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  14:39:07.000
//..
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'setTimeIfValid' FUNCTION:
        //
        // Concerns:
        //   Each of the four integer fields must separately be able to
        //   cause a return of an "invalid" status, independent of other input.
        //   If the input is not invalid, the value must be correctly set.
        //   The underlying functionality is already tested, so only a few test
        //   vectors are needed.
        //
        // Plan:
        //   Construct a table of valid and invalid inputs and compare results
        //   to expected "valid" values.
        //
        // Testing
        //   int setTimeIfValid(int hour, int min, int sec, int msec);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting setTimeIfValid''"
                          << "\n========================" << endl;

        if (verbose)
            cout << "\nTesting: 'setTimeIfValid'" << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_hour;         // specification hour
                int d_minute;       // specification minute
                int d_second;       // specification second
                int d_millisecond;  // specification millisecond
                int d_valid;        // expected return value
            } DATA[] = {
                //line hour   min  sec   msec   valid
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

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int HOUR         = DATA[i].d_hour;
                const int MINUTE       = DATA[i].d_minute;
                const int SECOND       = DATA[i].d_second;
                const int MILLISECOND  = DATA[i].d_millisecond;
                const int VALID        = DATA[i].d_valid;

                if (veryVerbose) { T_(); P_(LINE);   P_(VALID);
                                         P_(HOUR);   P_(MINUTE);
                                         P_(SECOND); P(MILLISECOND);
                }
                Obj x;  const Obj& X = x;
                if (1 == VALID) {
                    const Obj R(HOUR, MINUTE, SECOND, MILLISECOND);
                    LOOP_ASSERT(LINE,
                                0 == x.setTimeIfValid(HOUR,
                                                          MINUTE,
                                                          SECOND,
                                                          MILLISECOND));
                    LOOP_ASSERT(LINE, R == X);

                    if (verbose) { P_(VALID);  P_(R);  P(X); }
                }
                else {
                    const Obj R;
                    LOOP_ASSERT(LINE,
                                -1 == x.setTimeIfValid(HOUR,
                                                           MINUTE,
                                                           SECOND,
                                                           MILLISECOND));
                    LOOP_ASSERT(LINE, R == X);

                    if (verbose) { P_(VALID);  P_(R);  P(X); }
                }
            }
        }

        if (verbose) cout << "\nTesting that 'setTimeIfValid' works "
                             "with just one argument" << endl;
        {
            Obj x; const Obj& X = x;
            x.setTimeIfValid(22);

            if (veryVerbose) P(X);
            ASSERT(22 == X.hour());
            ASSERT( 0 == X.minute());
            ASSERT( 0 == X.second());
            ASSERT( 0 == X.millisecond());
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING STATIC 'isValid' FUNCTION:
        //
        // Concerns:
        //   This function is implemented as a number of separate relational
        //   operations conjoined (appropriately, or so we shall test) by
        //   logical AND and OR operations.  We must verify the "corners" for
        //   each argument, and the special case of hours == 24.
        //
        // Plan:
        //   This test case is an abbreviated copy of the ImpUtil test.  For a
        //   set of (year, month, day) values, verify that 'isValid' returns
        //   the expected value.
        //
        // Testing
        //   static bool isValid(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'isValid'"
                          << "\n=================" << endl;

        if (verbose)
            cout << "\nTesting: 'isValid'" << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_hour;         // specification hour
                int d_minute;       // specification minute
                int d_second;       // specification second
                int d_millisecond;  // specification millisecond
                int d_valid;        // expected return value
            } DATA[] = {
                //line hour   min  sec   msec   valid
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

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int HOUR         = DATA[i].d_hour;
                const int MINUTE       = DATA[i].d_minute;
                const int SECOND       = DATA[i].d_second;
                const int MILLISECOND  = DATA[i].d_millisecond;
                const int VALID        = DATA[i].d_valid;

                if (veryVerbose) { T_(); P_(LINE);   P_(VALID);
                                         P_(HOUR);   P_(MINUTE);
                                         P_(SECOND); P(MILLISECOND);
                }
                LOOP_ASSERT(LINE,
                            VALID == bdet_Time::isValid(HOUR,   MINUTE,
                                                        SECOND, MILLISECOND));
            }
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'add' METHODS:
        //
        // Concerns:
        //   Each 'add' method relies on several numerical constants and
        //   operations to generate the modified object value and the return
        //   value, but there is only one flow-of-control path through each
        //   method.  Sufficient data must be selected to verify each constant
        //   and operation.  The default value 24:00:00.000 (a special case)
        //   must be used as an initial value, and it is sufficient to choose
        //   any one other "typical" initial value to complete the test.
        //   Since the internal representation of a time value is a single
        //   'int', the second initial value may be chosen for its convenience
        //   WLOG (without loss of generality)
        //
        // Plan:
        //   First test 'addTime' explicitly using tabulated data, and then use
        //   the tested 'addTime' as an oracle to test the other four methods
        //   in a loop-based test.
        //
        //   Specify an arbitrary (but convenient) non-default value as an
        //   initial value, and also use the default value 24:00:00.000.
        //   Specify a set of (h, m, s, ms) tuples to be used as arguments to
        //   'addTime' and verify that both the modified object value and the
        //   return value are correct for each of the two initial values.
        //
        //   Specify a (negative) start value, a (positive) stop value, and a
        //   step size for each of the four fields (h, m, s, ms).  Loop over
        //   each of these four ranges, calling the appropriate 'add' method
        //   on the default object value and a second non-default object
        //   value, and using 'addTime' as an oracle to verify the resulting
        //   object values and return values for the 'add' method under test.
        //
        // Testing
        //   int addTime(int hours, int minutes, int seconds, int msec);
        //   int addHours(int hours);
        //   int addMinutes(int minutes);
        //   int addSeconds(int seconds);
        //   int addMilliseconds(int milliseconds);
        //   int addInterval(const bdet_DatetimeInterval& interval);
        //   bdet_Time& operator+=(const bdet_DatetimeInterval& rhs);
        //   bdet_Time& operator-=(const bdet_DatetimeInterval& rhs);
        //   bT operator+(const bdet_Time&, const bdet_DatetimeInterval&);
        //   bT operator+(const bdet_DatetimeInterval&, const bdet_Time&);
        //   bT operator-(const bdet_Time&, const bdet_DatetimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'add' Methods"
                          << "\n=====================" << endl;

        if (verbose) cout <<
            "\nTesting: 'addTime' with the default initial value" << endl;
        {
            static const struct {
                int d_lineNum;       // source line number
                int d_hours;         // hours to add
                int d_minutes;       // minutes to add
                int d_seconds;       // seconds to add
                int d_msecs;         // milliseconds to add
                int d_expDays;       // expected return value (days)
                int d_expHour;       // expected hour value
                int d_expMinute;     // expected minute value
                int d_expSecond;     // expected second value
                int d_expMsec;       // expected milliseconds value
            } DATA[] = {
    //----------^
    //        - - - - - - time added - - - - - -   ---expected values---
    //line #   h       m         s           ms    days  h   m   s   ms
    //------   --      --        --          ---   ----  --  --  --  ---
    { L_,       0,      0,        0,           0,     0,  0,  0,  0,   0 },
    { L_,       0,      0,        0,           1,     0,  0,  0,  0,   1 },
    { L_,       0,      0,        0,          -1,    -1, 23, 59, 59, 999 },
    { L_,       0,      0,        0,        1000,     0,  0,  0,  1,   0 },
    { L_,       0,      0,        0,       60000,     0,  0,  1,  0,   0 },
    { L_,       0,      0,        0,     3600000,     0,  1,  0,  0,   0 },
    { L_,       0,      0,        0,    86400000,     1,  0,  0,  0,   0 },
    { L_,       0,      0,        0,   -86400000,    -1,  0,  0,  0,   0 },
    { L_,       0,      0,        0,   864000000,    10,  0,  0,  0,   0 },

    { L_,       0,      0,        1,           0,     0,  0,  0,  1,   0 },
    { L_,       0,      0,       -1,           0,    -1, 23, 59, 59,   0 },
    { L_,       0,      0,       60,           0,     0,  0,  1,  0,   0 },
    { L_,       0,      0,     3600,           0,     0,  1,  0,  0,   0 },
    { L_,       0,      0,    86400,           0,     1,  0,  0,  0,   0 },
    { L_,       0,      0,   -86400,           0,    -1,  0,  0,  0,   0 },
    { L_,       0,      0,   864000,           0,    10,  0,  0,  0,   0 },

    { L_,       0,      1,        0,           0,     0,  0,  1,  0,   0 },
    { L_,       0,     -1,        0,           0,    -1, 23, 59,  0,   0 },
    { L_,       0,     60,        0,           0,     0,  1,  0,  0,   0 },
    { L_,       0,   1440,        0,           0,     1,  0,  0,  0,   0 },
    { L_,       0,  -1440,        0,           0,    -1,  0,  0,  0,   0 },
    { L_,       0,  14400,        0,           0,    10,  0,  0,  0,   0 },

    { L_,       1,      0,        0,           0,     0,  1,  0,  0,   0 },
    { L_,      -1,      0,        0,           0,    -1, 23,  0,  0,   0 },
    { L_,      24,      0,        0,           0,     1,  0,  0,  0,   0 },
    { L_,     -24,      0,        0,           0,    -1,  0,  0,  0,   0 },
    { L_,     240,      0,        0,           0,    10,  0,  0,  0,   0 },

    { L_,      24,   1440,    86400,    86400000,     4,  0,  0,  0,   0 },
    { L_,      24,   1440,    86400,   -86400000,     2,  0,  0,  0,   0 },
    { L_,      24,   1440,   -86400,   -86400000,     0,  0,  0,  0,   0 },
    { L_,      24,  -1440,   -86400,   -86400000,    -2,  0,  0,  0,   0 },
    { L_,     -24,  -1440,   -86400,   -86400000,    -4,  0,  0,  0,   0 },
    { L_,      25,   1441,    86401,    86400001,     4,  1,  1,  1,   1 },

    //----------v
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE     = DATA[i].d_lineNum;
                const int HOURS    = DATA[i].d_hours;
                const int MINUTES  = DATA[i].d_minutes;
                const int SECONDS  = DATA[i].d_seconds;
                const int MSECS    = DATA[i].d_msecs;

                const int EXP_DAYS = DATA[i].d_expDays;
                const int EXP_HR   = DATA[i].d_expHour;
                const int EXP_MIN  = DATA[i].d_expMinute;
                const int EXP_SEC  = DATA[i].d_expSecond;
                const int EXP_MSEC = DATA[i].d_expMsec;

                Obj x;  const Obj& X = x;
                if (veryVerbose) { T_();  P_(X); }
                int RETURN_VALUE = x.addTime(HOURS, MINUTES, SECONDS, MSECS);
                const Obj EXP(EXP_HR, EXP_MIN, EXP_SEC, EXP_MSEC);
                if (veryVerbose) { P_(X);  P_(EXP);  P(RETURN_VALUE); }
                LOOP_ASSERT(LINE, EXP      == X);
                LOOP_ASSERT(LINE, EXP_DAYS == RETURN_VALUE);
            }
        }

        if (verbose) cout <<
            "\nTesting: 'addTime' with a non-default initial value" << endl;
        {
            static const struct {
                int d_lineNum;       // source line number
                int d_hours;         // hours to add
                int d_minutes;       // minutes to add
                int d_seconds;       // seconds to add
                int d_msecs;         // milliseconds to add
                int d_expDays;       // expected return value (days)
                int d_expHour;       // expected hour value
                int d_expMinute;     // expected minute value
                int d_expSecond;     // expected second value
                int d_expMsec;       // expected milliseconds value
            } DATA[] = {
    //----------^
    //        - - - - - - time added - - - - - -   ---expected values---
    //line #   h       m         s           ms    days  h   m   s   ms
    //------   --      --        --          ---   ----  --  --  --  ---
    { L_,       0,      0,        0,           0,     0, 12,  0,  0,   0 },
    { L_,       0,      0,        0,           1,     0, 12,  0,  0,   1 },
    { L_,       0,      0,        0,          -1,     0, 11, 59, 59, 999 },
    { L_,       0,      0,        0,        1000,     0, 12,  0,  1,   0 },
    { L_,       0,      0,        0,       60000,     0, 12,  1,  0,   0 },
    { L_,       0,      0,        0,     3600000,     0, 13,  0,  0,   0 },
    { L_,       0,      0,        0,    86400000,     1, 12,  0,  0,   0 },
    { L_,       0,      0,        0,   -86400000,    -1, 12,  0,  0,   0 },
    { L_,       0,      0,        0,   864000000,    10, 12,  0,  0,   0 },

    { L_,       0,      0,        1,           0,     0, 12,  0,  1,   0 },
    { L_,       0,      0,       -1,           0,     0, 11, 59, 59,   0 },
    { L_,       0,      0,       60,           0,     0, 12,  1,  0,   0 },
    { L_,       0,      0,     3600,           0,     0, 13,  0,  0,   0 },
    { L_,       0,      0,    86400,           0,     1, 12,  0,  0,   0 },
    { L_,       0,      0,   -86400,           0,    -1, 12,  0,  0,   0 },
    { L_,       0,      0,   864000,           0,    10, 12,  0,  0,   0 },

    { L_,       0,      1,        0,           0,     0, 12,  1,  0,   0 },
    { L_,       0,     -1,        0,           0,     0, 11, 59,  0,   0 },
    { L_,       0,     60,        0,           0,     0, 13,  0,  0,   0 },
    { L_,       0,   1440,        0,           0,     1, 12,  0,  0,   0 },
    { L_,       0,  -1440,        0,           0,    -1, 12,  0,  0,   0 },
    { L_,       0,  14400,        0,           0,    10, 12,  0,  0,   0 },

    { L_,       1,      0,        0,           0,     0, 13,  0,  0,   0 },
    { L_,      -1,      0,        0,           0,     0, 11,  0,  0,   0 },
    { L_,      24,      0,        0,           0,     1, 12,  0,  0,   0 },
    { L_,     -24,      0,        0,           0,    -1, 12,  0,  0,   0 },
    { L_,     240,      0,        0,           0,    10, 12,  0,  0,   0 },

    { L_,      24,   1440,    86400,    86400000,     4, 12,  0,  0,   0 },
    { L_,      24,   1440,    86400,   -86400000,     2, 12,  0,  0,   0 },
    { L_,      24,   1440,   -86400,   -86400000,     0, 12,  0,  0,   0 },
    { L_,      24,  -1440,   -86400,   -86400000,    -2, 12,  0,  0,   0 },
    { L_,     -24,  -1440,   -86400,   -86400000,    -4, 12,  0,  0,   0 },
    { L_,      25,   1441,    86401,    86400001,     4, 13,  1,  1,   1 },

    //----------v
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const Obj INITIAL(12, 0, 0, 0);  // arbitrary but convenient value

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE     = DATA[i].d_lineNum;
                const int HOURS    = DATA[i].d_hours;
                const int MINUTES  = DATA[i].d_minutes;
                const int SECONDS  = DATA[i].d_seconds;
                const int MSECS    = DATA[i].d_msecs;

                const int EXP_DAYS = DATA[i].d_expDays;
                const int EXP_HR   = DATA[i].d_expHour;
                const int EXP_MIN  = DATA[i].d_expMinute;
                const int EXP_SEC  = DATA[i].d_expSecond;
                const int EXP_MSEC = DATA[i].d_expMsec;

                Obj x(INITIAL);  const Obj& X = x;
                if (veryVerbose) { T_();  P_(X); }
                int RETURN_VALUE = x.addTime(HOURS, MINUTES, SECONDS, MSECS);
                const Obj EXP(EXP_HR, EXP_MIN, EXP_SEC, EXP_MSEC);
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

            if (verbose) cout << "\nTesting: 'addHours'" << endl;
            for (int hi = START_HOURS; hi <= STOP_HOURS; hi += STEP_HOURS) {
                Obj x1(I1);  const Obj &X1 = x1;
                Obj x2(I2);  const Obj &X2 = x2;
                Obj y1(I1);  const Obj &Y1 = y1;
                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_();  P_(X1);  P(X2); }

                const int RX1 = x1.addHours(hi);
                const int RX2 = x2.addHours(hi);
                const int RY1 = y1.addTime(hi, 0, 0, 0);
                const int RY2 = y2.addTime(hi, 0, 0, 0);

                if (veryVerbose) { T_();  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(hi, Y1 == X1);  LOOP_ASSERT(hi, RY1 == RX1);
                LOOP_ASSERT(hi, Y2 == X2);  LOOP_ASSERT(hi, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting: 'addMinutes'" << endl;
            for (int mi = START_MINS; mi <= STOP_MINS; mi += STEP_MINS) {
                Obj x1(I1);  const Obj &X1 = x1;
                Obj x2(I2);  const Obj &X2 = x2;
                Obj y1(I1);  const Obj &Y1 = y1;
                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) {T_();   P_(X1);  P(X2); }

                const int RX1 = x1.addMinutes(mi);
                const int RX2 = x2.addMinutes(mi);
                const int RY1 = y1.addTime(0, mi, 0, 0);
                const int RY2 = y2.addTime(0, mi, 0, 0);

                if (veryVerbose) { T_();  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(mi, Y1 == X1);  LOOP_ASSERT(mi, RY1 == RX1);
                LOOP_ASSERT(mi, Y2 == X2);  LOOP_ASSERT(mi, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting: 'addSeconds'" << endl;
            for (int si = START_SECS; si <= STOP_SECS; si += STEP_SECS) {
                Obj x1(I1);  const Obj &X1 = x1;
                Obj x2(I2);  const Obj &X2 = x2;
                Obj y1(I1);  const Obj &Y1 = y1;
                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_();  P_(X1);  P(X2); }

                const int RX1 = x1.addSeconds(si);
                const int RX2 = x2.addSeconds(si);
                const int RY1 = y1.addTime(0, 0, si, 0);
                const int RY2 = y2.addTime(0, 0, si, 0);

                if (veryVerbose) { T_();  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(si, Y1 == X1);  LOOP_ASSERT(si, RY1 == RX1);
                LOOP_ASSERT(si, Y2 == X2);  LOOP_ASSERT(si, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting: 'addMilliseconds'" << endl;
            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {
                Obj x1(I1);  const Obj &X1 = x1;
                Obj x2(I2);  const Obj &X2 = x2;
                Obj y1(I1);  const Obj &Y1 = y1;
                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_();  P_(X1);  P(X2); }

                const int RX1 = x1.addMilliseconds(msi);
                const int RX2 = x2.addMilliseconds(msi);
                const int RY1 = y1.addTime(0, 0, 0, msi);
                const int RY2 = y2.addTime(0, 0, 0, msi);

                if (veryVerbose) { T_();  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(msi, Y1 == X1);  LOOP_ASSERT(msi, RY1 == RX1);
                LOOP_ASSERT(msi, Y2 == X2);  LOOP_ASSERT(msi, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting: 'addInterval'" << endl;
            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {
                Obj x1(I1);  const Obj &X1 = x1;
                Obj x2(I2);  const Obj &X2 = x2;
                Obj y1(I1);  const Obj &Y1 = y1;
                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_();  P_(X1);  P(X2); }

                const bdet_DatetimeInterval INTERVAL(0, 0, 0, 0, msi);

                const int RX1 = x1.addInterval(INTERVAL);
                const int RX2 = x2.addInterval(INTERVAL);
                const int RY1 = y1.addTime(0, 0, 0, msi);
                const int RY2 = y2.addTime(0, 0, 0, msi);

                if (veryVerbose) { T_();  P_(X1);  P_(X2);  P_(RX1);  P(RX2); }

                LOOP_ASSERT(msi, Y1 == X1);  LOOP_ASSERT(msi, RY1 == RX1);
                LOOP_ASSERT(msi, Y2 == X2);  LOOP_ASSERT(msi, RY2 == RX2);
            }

            if (verbose) cout << "\nTesting: 'operator+=' and 'operator-='"
                              << endl;
            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {
                Obj x1(I1);  const Obj &X1 = x1;
                Obj x2(I2);  const Obj &X2 = x2;
                Obj y1(I1);  const Obj &Y1 = y1;
                Obj y2(I2);  const Obj &Y2 = y2;

                if (veryVerbose) { T_();  P_(X1);  P(X2); }

                const bdet_DatetimeInterval INTERVAL(0, 0, 0, 0, msi);

                x1 += INTERVAL;
                x2 += INTERVAL;
                y1.addTime(0, 0, 0, msi);
                y2.addTime(0, 0, 0, msi);

                if (veryVerbose) { T_();  P_(X1);  P(X2); }

                LOOP_ASSERT(msi, Y1 == X1);
                LOOP_ASSERT(msi, Y2 == X2);

                x1 -= INTERVAL;
                x2 -= INTERVAL;
                y1.addTime(0, 0, 0, -msi);
                y2.addTime(0, 0, 0, -msi);

                LOOP_ASSERT(msi, Y1 == X1);
                LOOP_ASSERT(msi, Y2 == X2);
            }

            if (verbose) cout << "\nTesting: 'operator+' and 'operator-'"
                              << endl;
            for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS) {

                const bdet_DatetimeInterval INTERVAL(0, 0, 0, 0, msi);

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

                    LOOP_ASSERT(msi, Y1 == X1);
                    LOOP_ASSERT(msi, Y2 == X2);
                }
            }

        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING SUBTRACTION OPERATOR (-):
        //
        // Concerns:
        //   This operator performs a straightforward subtraction on the
        //   underlying integer total-milliseconds representation and returns
        //   the bdet_DatetimeInterval initialized with the result.  Therefore,
        //   only a very few values need be tested.  The default value
        //   (24:00:00.000) is not a valid operand for this operator
        //
        // Plan:
        //   Specify a set of object value pairs S.  For each (x1, x2) in S,
        //   verify that both x2 - x1 and x1 - x2 produce the expected results.
        //
        // Testing
        //   bdet_DtI operator- (const bdet_Time&, const bdet_Time&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Subtraction Operator"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting: 'operator-'" << endl;
        {
            static const struct {
                int d_lineNum;     // source line number
                int d_hours2;      // lhs time hours
                int d_minutes2;    // lhs time minutes
                int d_seconds2;    // lhs time seconds
                int d_msecs2;      // lhs time milliseconds
                int d_hours1;      // rhs time hours
                int d_minutes1;    // rhs time minutes
                int d_seconds1;    // rhs time seconds
                int d_msecs1;      // rhs time milliseconds
                int d_expMsec;     // expected difference (msec)
            } DATA[] = {
                //        -- lhs time --    -- rhs time --
                //line #  h   m   s   ms    h   m   s   ms     Expected msec
                //------  --  --  --  ---   --  --  --  ---    -------------
                { L_,      0,  0,  0,   0,   0,  0,  0,   0,             0  },

                { L_,      0,  0,  0,   1,   0,  0,  0,   0,             1  },
                { L_,      0,  0,  0,   0,   0,  0,  0,   1,            -1  },

                { L_,      0,  0,  1,   0,   0,  0,  0,   0,          1000  },
                { L_,      0,  0,  0,   0,   0,  0,  1,   0,         -1000  },

                { L_,      0,  1,  0,   0,   0,  0,  0,   0,         60000  },
                { L_,      0,  0,  0,   0,   0,  1,  0,   0,        -60000  },

                { L_,      1,  0,  0,   0,   0,  0,  0,   0,       3600000  },
                { L_,      0,  0,  0,   0,   1,  0,  0,   0,      -3600000  },

                { L_,     24,  0,  0,   0,   0,  0,  0,   1,            -1  },
                { L_,      0,  0,  0,   1,  24,  0,  0,   0,             1  },
                { L_,     24,  0,  0,   0,  24,  0,  0,   0,             0  },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE     = DATA[i].d_lineNum;
                const int HOURS2   = DATA[i].d_hours2;
                const int MINUTES2 = DATA[i].d_minutes2;
                const int SECONDS2 = DATA[i].d_seconds2;
                const int MSECS2   = DATA[i].d_msecs2;
                const int HOURS1   = DATA[i].d_hours1;
                const int MINUTES1 = DATA[i].d_minutes1;
                const int SECONDS1 = DATA[i].d_seconds1;
                const int MSECS1   = DATA[i].d_msecs1;
                const int EXP_MSEC = DATA[i].d_expMsec;

                const Obj X2(HOURS2, MINUTES2, SECONDS2, MSECS2);
                const Obj X1(HOURS1, MINUTES1, SECONDS1, MSECS1);
                const bdet_DatetimeInterval INTERVAL1(X2 - X1);
                const bdet_DatetimeInterval INTERVAL2(X1 - X2);
                if (veryVerbose) {
                    T_();  P_(X2);  P_(X1);  P(INTERVAL1.totalMilliseconds());
                }
                LOOP_ASSERT(LINE,  EXP_MSEC == INTERVAL1.totalMilliseconds());
                LOOP_ASSERT(LINE, -EXP_MSEC == INTERVAL2.totalMilliseconds());
            }
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING RELATIONAL OPERATORS (<, <=, >=, >):
        //
        // Concerns:
        //   Each operator must invoke the corresponding operator on the
        //   underlying integer total milliseconds correctly.
        //
        // Plan:
        //   Specify an ordered set S of unique object values.  For each (u, v)
        //   in the set S x S, verify the result of u OP v for each OP in {<,
        //   <=, >=, >}.
        //
        // Testing
        //   bool operator< (const bdet_Time& lhs, const bdet_Time& rhs);
        //   bool operator<=(const bdet_Time& lhs, const bdet_Time& rhs);
        //   bool operator>=(const bdet_Time& lhs, const bdet_Time& rhs);
        //   bool operator> (const bdet_Time& lhs, const bdet_Time& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Relational Operators"
                          << "\n============================" << endl;

        if (verbose) cout <<
            "\nTesting: 'operator<', 'operator<=', 'operator>=', 'operator>'"
                          << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0,   1  },
                {  0,  0,  0, 999  },  {  0,  0,  1,   0  },
                {  0,  0, 59, 999  },  {  0,  1,  0,   0  },
                {  0,  1,  0,   1  },  {  0, 59, 59, 999  },
                {  1,  0,  0,   0  },  {  1,  0,  0,   1  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 23, 59, 59, 999  },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj v(VALUES[i].d_hour,   VALUES[i].d_minute,
                      VALUES[i].d_second, VALUES[i].d_msec);
                const Obj& V = v;
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u(VALUES[j].d_hour,   VALUES[j].d_minute,
                          VALUES[j].d_second, VALUES[j].d_msec);
                    const Obj& U = u;
                    if (veryVerbose) { T_();  P_(i);  P_(j);  P_(V);  P(U); }
                    LOOP2_ASSERT(i, j, j <  i == U <  V);
                    LOOP2_ASSERT(i, j, j <= i == U <= V);
                    LOOP2_ASSERT(i, j, j >= i == U >= V);
                    LOOP2_ASSERT(i, j, j >  i == U >  V);
                }
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'getTime' WITH NULL ARGUMENTS:
        //
        // Concerns:
        //   The accessor must respond appropriately to null-pointer arguments,
        //   specifically not affecting other (non-null) arguments to be
        //   loaded with appropriate values.
        //
        // Plan:
        //   For each of a sequence of unique object values, verify that the
        //   'getTime' method with various null arguments produces the
        //   expected behavior.
        //
        // Testing:
        //   void getTime(int *hour, int *minute, int *second, int *msec = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'getTime' WITH NULL ARGUMENTS"
                          << "\n=====================================" << endl;

        if (verbose) cout << "\nTesting: 'getTime' with null args." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 24,  0,  0,   0  },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;
                int h, m, s, ms;

                Obj x;  const Obj& X = x;

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(&h, 0, 0, 0);
                if (veryVerbose) { T_();  P_(HOUR);  P_(h);  P(X); }
                LOOP_ASSERT(i, HOUR   == h);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(&h, 0, 0);
                if (veryVerbose) { T_();  P_(HOUR);  P_(h);  P(X); }
                LOOP_ASSERT(i, HOUR   == h);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(0, &m, 0, 0);
                if (veryVerbose) { T_();  P_(MINUTE);  P_(m);  P(X); }
                LOOP_ASSERT(i, MINUTE == m);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(0, &m, 0);
                if (veryVerbose) { T_();  P_(MINUTE);  P_(m);  P(X); }
                LOOP_ASSERT(i, MINUTE == m);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(0, 0, &s, 0);
                if (veryVerbose) { T_();  P_(SECOND);  P_(s);  P(X); }
                LOOP_ASSERT(i, SECOND == s);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(0, 0, &s);
                if (veryVerbose) { T_();  P_(SECOND);  P_(s);  P(X); }
                LOOP_ASSERT(i, SECOND == s);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(0, 0, 0, &ms);
                if (veryVerbose) { T_();  P_(MSEC);  P_(ms);  P(X); }
                LOOP_ASSERT(i, MSEC   == ms);

                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(0, 0, 0);
                if (veryVerbose) { T_();  P_(0);  P(X);  cout << endl; }
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL SETTING MANIPULATORS:
        //
        // Concerns:
        //   Each method has different behavior if the initial or final value
        //   is the (special) default value, and so each case must be tested
        //   separately.  Beyond that concern, the appropriate factors must be
        //   used in modifying the internal total-milliseconds representation.
        //
        // Plan:
        //   First, for a set of independent test values not including the
        //   default value (24:00:00.000), use the default constructor to
        //   create an object and use the 'set' manipulators to set its value.
        //   Verify the value using the basic accessors.
        //
        //   Next, confirm the correct behavior of the 'set' methods when
        //   setting to and from the value 24:00:00.000.
        //
        // Testing:
        //   void setHour(int hour))
        //   void setMinute(int minute)
        //   void setSecond(int second)
        //   void setMillisecond(int millisecond)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING 'set' MANIPULATORS:"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting 'setXXX' methods." << endl;

        if (verbose) cout << "\tFor ordinary computational values." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  // 24:00:00.000 NOT tested here
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;

                Obj x;  const Obj& X = x;
                x.setHour(HOUR);
                x.setMinute(MINUTE);
                x.setSecond(SECOND);
                x.setMillisecond(MSEC);
                if (veryVerbose) {
                    T_(); P_(HOUR); P_(MINUTE); P_(SECOND); P_(MSEC); P(X);
                }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());
            }
            if (veryVerbose) cout << endl;
        }

        if (verbose) cout << "\tSetting from value 24:00:00.000." << endl;
        {
            const Obj R24;             // Reference object (24:00:00.000)

            Obj x;  const Obj& X = x;  if (veryVerbose) { T_();  P_(X); }
            x.setMinute(0);            if (veryVerbose) P(X);
            ASSERT( 0 == x.hour());    ASSERT(  0 == x.minute());
            ASSERT( 0 == x.second());  ASSERT(  0 == x.millisecond());

            x = R24;                   if (veryVerbose) { T_();  P_(X); }
            x.setMinute(59);           if (veryVerbose) P(X);
            ASSERT( 0 == x.hour());    ASSERT( 59 == x.minute());
            ASSERT( 0 == x.second());  ASSERT(  0 == x.millisecond());

            x = R24;                   if (veryVerbose) { T_();  P_(X); }
            x.setSecond(0);            if (veryVerbose) P(X);
            ASSERT( 0 == x.hour());    ASSERT(  0 == x.minute());
            ASSERT( 0 == x.second());  ASSERT(  0 == x.millisecond());

            x = R24;                   if (veryVerbose) { T_();  P_(X); }
            x.setSecond(59);           if (veryVerbose) P(X);
            ASSERT( 0 == x.hour());    ASSERT(  0 == x.minute());
            ASSERT(59 == x.second());  ASSERT(  0 == x.millisecond());

            x = R24;                   if (veryVerbose) { T_();  P_(X); }
            x.setMillisecond(0);       if (veryVerbose) P(X);
            ASSERT( 0 == x.hour());    ASSERT(  0 == x.minute());
            ASSERT( 0 == x.second());  ASSERT(  0 == x.millisecond());

            x = R24;                   if (veryVerbose) { T_();  P_(X); }
            x.setMillisecond(999);     if (veryVerbose) { P(X); cout << endl; }
            ASSERT( 0 == x.hour());    ASSERT(  0 == x.minute());
            ASSERT( 0 == x.second());  ASSERT(999 == x.millisecond());
        }

        if (verbose) cout << "\tSetting to value 24:00:00.000." << endl;
        {
            const Obj R(23, 22, 21, 209); // Reference object (21:22:21.209)

            Obj x(R); const Obj& X = x;if (veryVerbose) { T_();  P_(X); }
            x.setHour(24);             if (veryVerbose) { P(X); cout << endl; }
            ASSERT(24 == x.hour());    ASSERT(  0 == x.minute());
            ASSERT( 0 == x.second());  ASSERT(  0 == x.millisecond());

        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZING CONSTRUCTOR:
        //
        // Concerns:
        //   The two required parameters and two optional parameters must be
        //   multiplied by the appropriate factors when initializing the
        //   internal total-milliseconds integer representation.  Also, the
        //   default value 24:00:00.000 must be constructible explicitly.
        //
        // Plan:
        //   Specify a set S of times as (h, m, s, ms) tuples having widely
        //   varying values.  For each (h, m, s, ms) in S, construct an object
        //   X using all four arguments and an object Y using the first three
        //   arguments, and verify that X and Y have the expected values.
        //
        //   Test explicitly that the initializing constructor can create an
        //   object having the default value.
        //
        // Testing:
        //   bdet_Time(int hour, int minute, int second = 0, int millisec = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING INITIALIZING CONSTRUCTOR:"
                          << "\n=================================" << endl;

        if (verbose) cout << "\nFor ordinary computational values." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 23, 59, 59, 999  },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;

                Obj x(HOUR, MINUTE, SECOND, MSEC);  const Obj& X = x;
                Obj y(HOUR, MINUTE, SECOND);        const Obj& Y = y;
                Obj z(HOUR, MINUTE);                const Obj& Z = z;
                if (veryVerbose) {
                    T_(); P_(HOUR); P_(MINUTE); P_(SECOND); P_(MSEC); P_(X);
                    P_(Y)  P(Z);
                }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());
                LOOP_ASSERT(i, HOUR   == Y.hour());
                LOOP_ASSERT(i, MINUTE == Y.minute());
                LOOP_ASSERT(i, SECOND == Y.second());
                LOOP_ASSERT(i, 0      == Y.millisecond());
                LOOP_ASSERT(i, HOUR   == Z.hour());
                LOOP_ASSERT(i, MINUTE == Z.minute());
                LOOP_ASSERT(i, 0      == Z.second());
                LOOP_ASSERT(i, 0      == Z.millisecond());
            }
        }

        if (verbose) cout << "\nFor the default values." << endl;
        {
            Obj d;               const Obj& D = d;
            Obj x(24, 0, 0, 0);  const Obj& X = x;
            Obj y(24, 0, 0);     const Obj& Y = y;
            Obj z(24, 0);        const Obj& Z = z;

            if (veryVerbose) { T_();  P_(D);  P_(X);  P_(Y)  P(Z); }

            ASSERT(D == X);
            ASSERT(D == Y);
            ASSERT(D == Z);
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING 'bdex' STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   The 'bdex' streaming concerns for this component are standard.  We
        //   first test the class method 'maxSupportedBdexVersion' and then
        //   use that method to probe the member functions 'outStream' and
        //   'inStream' in the manner of a "breathing test" to verify basic
        //   functionality.  We then thoroughly test streaming functionality
        //   using the provided bdex functions, which forward appropriate calls
        //   to the member functions of this component.  We next step through
        //   the sequence of possible stream states (valid, empty, invalid,
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
        //     Use the underlying stream package to simulate of a typical valid
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
        //   int maxSupportedBdexVersion() const;
        //   BDEX STREAMING
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Streaming Functionality"
                          << "\n===============================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // scalar and array object values for various stream tests
        const Obj VA(  0,  0,  0,   0);
        const Obj VB(  0,  0,  0, 999);
        const Obj VC(  0,  0, 59,   0);
        const Obj VD(  0, 59,  0,   0);
        const Obj VE( 23,  0,  0,   0);
        const Obj VF( 23, 22, 21, 209);
        const Obj VG( 24,  0,  0,   0);

        const int NUM_VALUES = 7;
        const Obj VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF, VG };
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

        const int VERSION = Obj::maxSupportedBdexVersion();
        if (verbose) cout << "\nDirect initial trial of 'streamOut' and"
                             " (valid) 'streamIn' functionality." << endl;
        {
            const Obj X(VC);
            Out       out;
            const int VERSION = Obj::maxSupportedBdexVersion();

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
            "\nThorough test using stream operators ('<<' and '>>')." << endl;
        {
            for (int i = 0; i < NUM_VALUES; ++i) {
                const Obj X(VALUES[i]);
                Out out;
                bdex_OutStreamFunctions::streamOut(out, X, VERSION);
                const char *const OD  = out.data();
                const int         LOD = out.length();

                // Verify that each new value overwrites every old value and
                // that the input stream is emptied, but remains valid.

                for (int j = 0; j < NUM_VALUES; ++j) {
                    In in(OD, LOD);  In &testInStream = in;
                    in.setSuppressVersionCheck(1);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, !in.isEmpty());
                    Obj t(VALUES[j]);
                  BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                    LOOP2_ASSERT(i, j, X == t == (i == j));
                    bdex_InStreamFunctions::streamIn(in, t, VERSION);
                  } END_BDEX_EXCEPTION_TEST
                    LOOP2_ASSERT(i, j, X == t);
                    LOOP2_ASSERT(i, j, in);  LOOP2_ASSERT(i, j, in.isEmpty());
                }
            }
        }

        if (verbose) cout <<
            "\nTesting streamIn functionality via operator ('>>')." << endl;

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            Out out;
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                In in(OD, LOD);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);           LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj X(VALUES[i]);  Obj t(X);  LOOP_ASSERT(i, X == t);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, X == t);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
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
                In in(OD, i);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Obj t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                                         if (0 == i) LOOP_ASSERT(i, W1 == t1);
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
                                      if (LOD1 == i) LOOP_ASSERT(i, W2 == t2);
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
                                      if (LOD2 == i) LOOP_ASSERT(i, W3 == t3);
                }

                                LOOP_ASSERT(i, Y1 != t1);
                t1 = Y1;        LOOP_ASSERT(i, Y1 == t1);

                                LOOP_ASSERT(i, Y2 != t2);
                t2 = Y2;        LOOP_ASSERT(i, Y2 == t2);

                                LOOP_ASSERT(i, Y3 != t3);
                t3 = Y3;        LOOP_ASSERT(i, Y3 == t3);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W;                   // default value
        const Obj X(0, 0, 0, 0);       // original (control)
        const Obj Y(0, 0, 0, 1);       // new (streamed-out)

        const int SERIAL_Y  = 1;       // internal rep.  of Obj Y

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            const char version = 1;

            Out out;
            out.putInt32(SERIAL_Y);  // Stream out "new" value.
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(in);
                             ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out;
            out.putInt32(SERIAL_Y);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, version);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        {
            const char version = 5 ; // too large (current Time version is 1)

            Out out;
            out.putInt32(SERIAL_Y);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, version);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tValue too small." << endl;
        {
            Out out;
            out.putInt32(-1);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tValue too large." << endl;
        {
            Out out;
            out.putInt32(24*60*60*1000 + 1);  // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                int         d_hour;         // specification hour
                int         d_minute;       // specification minute
                int         d_second;       // specification second
                int         d_millisecond;  // specification millisecond
                int         d_version;      // version to stream with
                int         d_length;       // expect output length
                const char *d_fmt_p;        // expected output format
            } DATA[] = {
                //line  hour  min  sec  milli  ver  len  format
                //----  ----  ---  ---  -----  ---  ---  -------------------
                { L_,     14,  10,   2,   117,  0,   0,  ""                  },
                { L_,     20,   8,  27,   983,  0,   0,  ""                  },

                { L_,     14,  10,   2,   117,  1,   4,  "\x03\x0a\x3b\x05"  },
                { L_,     20,   8,  27,   983,  1,   4,  "\x04\x52\x62\x4f"  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int HOUR         = DATA[i].d_hour;
                const int MINUTE       = DATA[i].d_minute;
                const int SECOND       = DATA[i].d_second;
                const int MILLISECOND  = DATA[i].d_millisecond;
                const int VERSION      = DATA[i].d_version;
                const int LEN          = DATA[i].d_length;
                const char *const FMT  = DATA[i].d_fmt_p;

                Obj mX(HOUR, MINUTE, SECOND, MILLISECOND);  const Obj& X = mX;
                bdex_ByteOutStream out;  X.bdexStreamOut(out, VERSION);

                LOOP_ASSERT(LINE, LEN == out.length());
                LOOP_ASSERT(LINE, 0 == memcmp(out.data(),
                                              FMT,
                                              LEN));

                if (verbose && memcmp(out.data(), FMT, LEN)) {
                    const char *hex = "0123456789abcdef";
                    P_(LINE);
                    for (int j = 0; j < out.length(); ++j) {
                        cout << "\\x"
                             << hex[(unsigned char)*
                                          (out.data() + j) >> 4]
                             << hex[(unsigned char)*
                                       (out.data() + j) & 0x0f];
                    }
                    cout << endl;
                }

                Obj mY;  const Obj& Y = mY;
                if (LEN) { // version is supported
                    bdex_ByteInStream in(out.data(),
                                         out.length());
                    mY.bdexStreamIn(in, VERSION);
                }
                else { // version is not supported
                    mY = X;
                    bdex_ByteInStream in;
                    mY.bdexStreamIn(in, VERSION);
                    LOOP_ASSERT(LINE, !in);
                }
                LOOP_ASSERT(LINE, X == Y);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the rhs operand value.  Also, any object must
        //   be assignable to itself.
        //
        // Plan:
        //   Construct and initialize a set S of (unique) objects with
        //   substantial and varied differences in value.  Using all
        //   combinations (u, v) in the cross product S x S, assign v to u and
        //   assert that u == v.  Then test aliasing by assigning (a temporary
        //   copy of) each u to itself and verifying that its value remains
        //   unchanged.
        //
        // Testing:
        //   bdet_Time& operator=(const bdet_Time& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;

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

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int i;

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj v;  const Obj& V = v;
                v.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u;  const Obj& U = u;
                    u.setTime(VALUES[j].d_hour,    VALUES[j].d_minute,
                              VALUES[j].d_second,  VALUES[j].d_msec);
                    if (veryVerbose) { T_();  P_(V);  P_(U); }
                    Obj w(V);  const Obj &W = w;          // control
                    u = V;
                    if (veryVerbose) P(U);
                    LOOP2_ASSERT(i, j, W == U);
                    LOOP2_ASSERT(i, j, W == V);
                }
            }

            if (verbose) cout << "\nTesting assignment u = u (Aliasing)."
                              << endl;

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj u;  const Obj& U = u;
                u.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                Obj w(U);  const Obj &W = w;              // control
                u = u;
                if (veryVerbose) { T_();  P_(U);  P(W); }
                LOOP_ASSERT(i, W == U);
            }
        }

      } break;
      case 8: {
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
#define NL "\n"
            static const struct {
                int         d_lineNum;  // source line number
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  indent +/-  spaces  format                // ADJUST
                //----  ----------  ------  --------------
                { L_,    0,         -1,     "24:00:00.000"                   },

                { L_,    0,          0,     "24:00:00.000"              NL   },

                { L_,    0,          2,     "24:00:00.000"              NL   },

                { L_,    1,          1,     " 24:00:00.000"             NL   },

                { L_,    1,          2,     "  24:00:00.000"            NL   },

                { L_,   -1,          2,     "24:00:00.000"              NL   },

                { L_,   -2,          1,     "24:00:00.000"              NL   },

                { L_,    2,          1,     "  24:00:00.000"            NL   },

                { L_,    1,          3,     "   24:00:00.000"           NL   },
            };
#undef NL
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 128;  // Must be big enough to hold output string.
            const char Z1  = (char) 0xFF;  // Value 1 used for an unset char.
            const char Z2  = 0x00; // Value 2 used to represent an unset char.

            char mCtrlBuf1[SIZE];  memset(mCtrlBuf1, Z1, SIZE);
            char mCtrlBuf2[SIZE];  memset(mCtrlBuf2, Z2, SIZE);
            const char *CTRL_BUF1 = mCtrlBuf1;
            const char *CTRL_BUF2 = mCtrlBuf2;

            Obj mX;  const Obj& X = mX;  // 24:00:00.000

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
//                const int         LINE = DATA[ti].d_lineNum; // unused
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const char *const FMT  = DATA[ti].d_fmt_p;

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
        //   Specify a set S of control objects with substantial and varied
        //   differences in value.  For each object w in S, construct and
        //   initialize an identically valued object x using the primary
        //   manipulator, and copy construct an object y from x.  Use the
        //   equality operator to assert that both x and y have the same value
        //   as w.
        //
        // Testing:
        //   bdet_Time(const bdeT_Time& original);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

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

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj x;  const Obj& X = x;
                x.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                Obj w;  const Obj& W = w;
                w.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);

                Obj y(X);  const Obj &Y = y;
                if (veryVerbose) { T_();  P_(W);  P_(X);  P(Y); }
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
        //   operators.  The test data have variations in each input
        //   parameter, even though tested methods convert the input before
        //   the underlying equality operator on a single pair of 'int's is
        //   invoked.  On the other hand, the default value (24:00:00.000) is
        //   an explicit concern, and so must be tested explicitly.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences, but also including the default value.  Verify
        //   the correctness of 'operator==' and 'operator!=' using all
        //   elements (u, v) of the cross product S X S.
        //
        // Testing:
        //   operator==(const bdet_Time&, const bdet_Time&);
        //   operator!=(const bdet_Time&, const bdet_Time&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in S X S." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },
                {  0,  0,  0,   1  },  {  0,  0,  1,   0  },
                {  0,  1,  0,   0  },  {  1,  0,  0,   0  },

                { 23, 22, 21, 209  },
                { 23, 22, 21, 208  },  { 23, 22, 20, 209  },
                { 23, 21, 21, 209  },  { 22, 22, 21, 209  },

                { 24,  0,  0,   0  },  // 24:00:00.000 explicitly included
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj u;  const Obj& U = u;
                u.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj v;  const Obj& V = v;
                    v.setTime(VALUES[j].d_hour,   VALUES[j].d_minute,
                              VALUES[j].d_second, VALUES[j].d_msec);
                    bool isSame = i == j;
                    if (veryVerbose) { T_();  P_(i);  P_(j);  P_(U);  P(V); }
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
        // TESTING OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //   The output operator is implemented using the basic accessors and
        //   a format field in the 'sprintf' function.  Since the accessors are
        //   already tested, only the formatting requires verification.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the literal
        //   expected output format.
        //
        // Testing:
        //   operator<<(ostream&, const bdet_Time&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                int         d_hour;     // hour field value
                int         d_minute;   // minute field value
                int         d_second;   // second field value
                int         d_msec;     // millisecond field value
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //line  hour   min.   sec.   msec.    output format
                //----  -----  ----   ----   -----    --------------
                { L_,      0,    0,     0,      0,    "00:00:00.000"     },
                { L_,      0,    0,     0,    999,    "00:00:00.999"     },
                { L_,      0,    0,    59,      0,    "00:00:59.000"     },
                { L_,      0,   59,     0,      0,    "00:59:00.000"     },
                { L_,     23,    0,     0,      0,    "23:00:00.000"     },
                { L_,     23,   22,    21,    209,    "23:22:21.209"     },
                { L_,     23,   22,    21,    210,    "23:22:21.210"     },
                { L_,     24,    0,     0,      0,    "24:00:00.000"     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000;       // Must be able to hold output string.
            const char XX = (char) 0xFF; // Value used to for an unset char.
            char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
            const char *CTRL_BUF = mCtrlBuf; // Used for extra character check.

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int         LINE   = DATA[di].d_lineNum;
                const int         HOUR   = DATA[di].d_hour;
                const int         MINUTE = DATA[di].d_minute;
                const int         SECOND = DATA[di].d_second;
                const int         MSEC   = DATA[di].d_msec;
                const char *const FMT    = DATA[di].d_fmt_p;

                char buf[SIZE];
                memcpy(buf, CTRL_BUF, SIZE); // Preset buf to 'unset' values.

                Obj x;  const Obj& X = x;
                x.setTime(HOUR, MINUTE, SECOND, MSEC);

                if (veryVerbose) cout << "\tEXPECTED FORMAT: " << FMT << endl;
                ostrstream out(buf, SIZE);  out << X << ends;
                if (veryVerbose) cout << "\tACTUAL FORMAT:   " << buf << endl;

                const int SZ = strlen(FMT) + 1;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, XX == buf[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE,  0 == memcmp(buf, FMT, SZ));
                LOOP_ASSERT(LINE,  0 ==
                                     memcmp(buf + SZ, CTRL_BUF + SZ, SIZE-SZ));
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS:
        //
        // Concerns:
        //   Each accessor must perform the appropriate arithmetic to convert
        //   the internal total-milliseconds representation to the
        //   four-parameter (h, m, s, ms) representation.  Since we are testing
        //   "basic accessors", the case where the 'getTime' method is passed
        //   null arguments is not considered here.
        //
        // Plan:
        //   For each of a sequence of unique object values, verify that each
        //   of the basic accessors returns the correct value.
        //
        // Testing:
        //   void getTime(int *hour, int *minute, int *second, int *msec = 0);
        //   int hour();
        //   int minute();
        //   int second();
        //   int millisecond();
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING BASIC ACCESSORS"
                          << "\n=======================" << endl;

        if (verbose) cout << "\nTesting: 'getTime', 'hour()', 'minute()',"
                             "'second()', and 'millisecond()'" << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 24,  0,  0,   0  },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;
                int       h, m, s, ms;

                Obj x;  const Obj& X = x;
                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                X.getTime(&h, &m, &s, &ms);

                if (veryVerbose) {
                    T_();  P_(HOUR);    P_(h);  P_(MINUTE);  P_(m);
                           P_(SECOND);  P_(s);  P_(MSEC);    P(ms);
                    T_();  P(X);
                }

                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());

                LOOP_ASSERT(i, HOUR   == h);
                LOOP_ASSERT(i, MINUTE == m);
                LOOP_ASSERT(i, SECOND == s);
                LOOP_ASSERT(i, MSEC   == ms);
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg':
        //   Void for 'bdet_time'.
        // --------------------------------------------------------------------

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //
        // Concerns:
        //   The separate time fields must be multiplied by the appropriate
        //   factors to convert the four-parameter input representation to the
        //   internal total-milliseconds representation.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create a default object and use the primary
        //   manipulator to set its value.  Verify the value using the basic
        //   accessors.  Note that the destructor is exercised on each
        //   configuration as the object being tested leaves scope (thereby
        //   enabling assertions of internal invariants).
        //
        // Testing:
        //   bdet_Time();
        //   ~bdet_Time();
        //   void setTime(int hour, int minute = 0, int sec = 0, int msec = 0)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING PRIMARY MANIPULATOR:"
                          << "\n============================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;

        Obj x;  const Obj& X = x;

        ASSERT(24 == X.hour());          ASSERT(0 == X.minute());
        ASSERT( 0 == X.second());        ASSERT(0 == X.millisecond());

        if (verbose) cout << "\nTesting 'setTime'." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 22, 21, 210  },  { 24,  0,  0,   0  },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;

                Obj x;  const Obj& X = x;
                x.setTime(HOUR, MINUTE, SECOND, MSEC);
                if (veryVerbose) {
                    T_(); P_(HOUR); P_(MINUTE); P_(SECOND); P_(MSEC); P(X);
                }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());

                x.setTime(HOUR, MINUTE, SECOND);
                if (veryVerbose) {
                    T_(); P_(HOUR); P_(MINUTE); P_(SECOND); P(X);
                }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, 0      == X.millisecond());

                x.setTime(HOUR, MINUTE);
                if (veryVerbose) { T_();  P_(HOUR);  P_(MINUTE);  P(X); }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, 0      == X.second());
                LOOP_ASSERT(i, 0      == X.millisecond());

                x.setTime(HOUR);
                if (veryVerbose) { T_();  P_(HOUR);  P(X); }
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, 0      == X.minute());
                LOOP_ASSERT(i, 0      == X.second());
                LOOP_ASSERT(i, 0      == X.millisecond());
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
        //   denote unique, but otherwise arbitrary, object values, while 'D'
        //   denotes the valid default object value.
        //
        // 1. Create an object x1 (init. to VA).    { x1:VA }
        // 2. Create an object x2 (copy from x1).   { x1:VA x2:VA }
        // 3. Set x1 to VB.                         { x1:VB x2:VA }
        // 4. Create an object x3 (default ctor).   { x1:VB x2:VA x3:D }
        // 5. Create an object x4 (copy from x3).   { x1:VB x2:VA x3:D  x4:D }
        // 6. Set x3 to VC.                         { x1:VB x2:VA x3:VC x4:D }
        // 7. Assign x2 = x1.                       { x1:VB x2:VB x3:VC x4:D }
        // 8. Assign x2 = x3.                       { x1:VB x2:VC x3:VC x4:D }
        // 9. Assign x1 = x1 (aliasing).            { x1:VB x2:VB x3:VC x4:D }
        //
        // Testing:
        //   This Test Case exercises basic value-semantic functionality.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        const int HA = 1, MA = 2, SA = 3, mA = 4;  // h, m, s, ms values for VA
        const int HB = 5, MB = 6, SB = 7, mB = 8;  // h, m, s, ms values for VB
        const int HC = 9, MC = 9, SC = 9, mC = 9;  // h, m, s, ms values for VC
        int h, m, s, ms;                   // reusable variables for 'get' call

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(HA, MA, SA, mA);  const Obj& X1 = mX1;
        if (verbose) { T_();  P(X1); }

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
        if (verbose) { T_();  P(X2); }

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
        if (verbose) { T_();  P(X1); }

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
        if (verbose) { T_();  P(X3); }

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
        if (verbose) { T_();  P(X4); }

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
        if (verbose) { T_();  P(X3); }

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
        if (verbose) { T_();  P(X2); }

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
        if (verbose) { T_();  P(X2); }

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
        if (verbose) { T_();  P(X1); }

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
      case -1: {
        //---------------------------------------------------------------------
        // Sandbox
        //---------------------------------------------------------------------

        time_t  t1,    t2;
        tm     *ptm1, *ptm2;
        Obj     x1;  const Obj& X1 = x1;
        Obj     x2;  const Obj& X2 = x2;

        t1 = time(&t1);
        ptm1 = localtime(&t1);
        x1.setTime(ptm1->tm_hour, ptm1->tm_min, ptm1->tm_sec);
        P(X1);
        int k;
        for (int i = 0; i < 1000000; ++i) {
            for (int j = 0; j < 200; ++j) {
                k = i + j;
            }
        }
        P(k);
        t2 = time(&t2);
        ptm2 = localtime(&t2);
        x2.setTime(ptm2->tm_hour, ptm2->tm_min, ptm2->tm_sec);
        P(X2);

        P(X2 - X1);

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
