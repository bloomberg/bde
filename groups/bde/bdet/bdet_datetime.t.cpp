// bdet_datetime.t.cpp                                                -*-C++-*-

#include <bdet_datetime.h>

#include <bdet_date.h>
#include <bdet_time.h>
#include <bdex_byteoutstream.h>          // for testing only
#include <bdex_byteinstream.h>           // for testing only
#include <bdex_instreamfunctions.h>      // for testing only
#include <bdex_outstreamfunctions.h>     // for testing only
#include <bdex_testinstream.h>           // for testing only
#include <bdex_testoutstream.h>          // for testing only
#include <bdex_testinstreamexception.h>  // for testing only

#include <bsls_types.h>

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
// The component under test is implemented using contained 'bdet_date' and
// 'bdet_time' objects.  As such, many concerns of a typical value-semantic
// component may be relaxed.  In particular, aside from 'bdex' streaming,
// there are only two accessors, 'date' and 'time', which permit the user to
// invoke the underlying accessors directly; minimal testing is required of
// these.  Similarly, most of the datetime manipulators are implemented using
// the underlying contained-object manipulators; only the collaboration
// between time and date addition exposes new functionality.
//
// Note that if the implementation is ever changed, this test driver should be
// re-analized -- and perhaps rewritten -- BEFORE the implementation is
// altered.
//
// ----------------------------------------------------------------------------
// [17] bool isValid(int, int, int, int, int, int, int);
// [ 2] bdet_Datetime();
// [11] bdet_Datetime(const bdet_Date& date);
// [11] bdet_Datetime(const bdet_Date& date, const bdet_Time& time);
// [11] bdet_Datetime(int y, int m, int d, int h , int m , int s, int ms);
// [ 7] bdet_Datetime(const bdet_Datetime& original);
// [ 2] ~bdet_Datetime();   (via purify)
// [ 9] bdet_Datetime& operator=(const bdet_Datetime& rhs);
// [15] bdet_Datetime& operator+=(const bdet_Datetime& rhs);
// [15] bdet_Datetime& operator-=(const bdet_Datetime& rhs);
// [ 2] bdet_Date& date();
// [12] void setDatetime(int y, int m, int d, int h , int m , int s, int ms);
// [18] int setDatetimeIfValid(int, int, int, int, int, int, int);
// [20] void setDate(const bdet_Date& date);
// [ 2] setTime(int hour =0, int minute =0, int second =0, int millisecond=0);
// [22] void setTime(const bdet_Time& time);
// [12] void setHour(int hour);
// [12] void setMinute(int minute);
// [12] void setSecond(int second);
// [12] void setMillisecond(int millisecond);
// [23] void setYearMonthDay(int year, int month, int day);
// [19] void setYearDay(int year, int dayOfYear);
// [14] addTime(int hours, int minutes=0, int seconds=0, int milliseconds=0);
// [21] void addDays(int days);
// [14] void addHours(int hours);
// [14] void addMinutes(int minutes);
// [14] void addSeconds(int seconds);
// [14] void addMilliseconds(int milliseconds);
// [ 4] const bdet_Date& date() const;
// [ 4] const bdet_Time& time() const;
// [ 4] int year() const;
// [ 4] int month() const;
// [ 4] int day() const;
// [ 4] int dayOfYear() const;
// [ 4] int dayOfWeek() const;
// [ 4] int hour() const;
// [ 4] int minute() const;
// [ 4] int second() const;
// [ 4] int millisecond() const;
// [ 8] ostream& print(ostream& os, int level = 0, int spl = 4) const;
// [10] int maxSupportedBdexVersion() const;
// [ 6] bool operator==(const bdet_Datetime& lhs, const bdet_Datetime& rhs);
// [ 6] bool operator!=(const bdet_Datetime& lhs, const bdet_Datetime& rhs);
// [13] bool operator< (const bdet_Datetime& lhs, const bdet_Datetime& rhs);
// [13] bool operator<=(const bdet_Datetime& lhs, const bdet_Datetime& rhs);
// [13] bool operator> (const bdet_Datetime& lhs, const bdet_Datetime& rhs);
// [13] bool operator>=(const bdet_Datetime& lhs, const bdet_Datetime& rhs);
// [15] bdet_DTInterval operator-(const bdet_Datetime&, const bdet_Datetime&);
// [ 5] ostream& operator<<(ostream &output, const bdet_Datetime &date);
// ----------------------------------------------------------------------------
// [ 1] breathing test
// [24] USAGE example
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

typedef bdet_Datetime      Obj;
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
      case 24: {
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
// Values represented by objects of type 'bdet_Datetime' are used widely in
// practice.  The values of the individual fields resulting from a
// default-constructed 'bdet_Datetime' object, 'dt', are
// "0001/01/01_24:00:00.000":
//..
    bdet_Datetime dt;           ASSERT( 1 == dt.date().year());
                                ASSERT( 1 == dt.date().month());
                                ASSERT( 1 == dt.date().day());
                                ASSERT(24 == dt.time().hour());
                                ASSERT( 0 == dt.time().minute());
                                ASSERT( 0 == dt.time().second());
                                ASSERT( 0 == dt.time().millisecond());
//..
// We can then set 'dt' to have a specific value, say, 8:43pm on January 6,
// 2013:
//..
    dt.setDatetime(2013, 1, 6, 20, 43);
                                ASSERT(2013 == dt.date().year());
                                ASSERT(   1 == dt.date().month());
                                ASSERT(   6 == dt.date().day());
                                ASSERT(  20 == dt.time().hour());
                                ASSERT(  43 == dt.time().minute());
                                ASSERT(   0 == dt.time().second());
                                ASSERT(   0 == dt.time().millisecond());
//..
// Now suppose we add 6 hours and 9 seconds to this value.  There is more
// than one way to do it:
//..
    bdet_Datetime dt2(dt);
    dt2.addHours(6);
    dt2.addSeconds(9);
                                ASSERT(2013 == dt2.date().year());
                                ASSERT(   1 == dt2.date().month());
                                ASSERT(   7 == dt2.date().day());
                                ASSERT(   2 == dt2.time().hour());
                                ASSERT(  43 == dt2.time().minute());
                                ASSERT(   9 == dt2.time().second());
                                ASSERT(   0 == dt2.time().millisecond());

    bdet_Datetime dt3(dt);
    dt3.addTime(6, 0, 9);
                                ASSERT(dt2 == dt3);
//..
// Notice that (in both cases) the date changed as a result of adding time;
// however, changing just the date never affects the time:
//..
    dt3.addDays(10);
                                ASSERT(2013 == dt3.date().year());
                                ASSERT(   1 == dt3.date().month());
                                ASSERT(  17 == dt3.date().day());
                                ASSERT(   2 == dt3.time().hour());
                                ASSERT(  43 == dt3.time().minute());
                                ASSERT(   9 == dt3.time().second());
                                ASSERT(   0 == dt3.time().millisecond());
//..
// We can also add more than a day's worth of time:
//..
    dt2.addHours(240);
                                ASSERT(dt3 == dt2);
//..
// The individual arguments can also be negative:
//..
    dt2.addTime(-246, 0, -10, 1000);  // -246 h, -10 s, +1000 ms
                                ASSERT(dt == dt2);
//..
// Finally, we stream the value of 'dt2' to 'stdout':
//..
if (verbose)
    bsl::cout << dt2 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  06JAN2013_20:43:00.000
//..

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING SETYEARMONTHDAY:
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the parameter value.  Also, any object must be
        //   settable to itself.
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
        //   void setYearMonthDay(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting setYearMonthDay"
                          << "\n=======================" << endl;

        if (verbose) cout << "\nTesting setTime u = V." << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day;
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {    1,  1,  1,  0,  0,  0,   0 },
                {   10,  4,  5,  0,  0,  0, 999 },
                {  100,  6,  7,  0,  0, 59,   0 },
                { 1000,  8,  9,  0, 59,  0,   0 },
                { 2000,  2, 29, 23,  0,  0,   0 },
                { 2002,  7,  4, 21, 22, 23, 209 },
                { 2003,  8,  5, 21, 22, 23, 210 },
                { 2004,  9,  3, 22, 44, 55, 888 },
                { 9999, 12, 31, 23, 59, 59, 999 },

                {    1,  1,  1, 24,  0,  0,   0 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int i;

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj v;  const Obj& V = v;
                v.setYearMonthDay(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
                v.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u;  const Obj& U = u;
                    u.setYearMonthDay(VALUES[j].d_year,
                                      VALUES[j].d_month,
                                      VALUES[j].d_day);
                    u.setTime(VALUES[j].d_hour,    VALUES[j].d_minute,
                              VALUES[j].d_second,  VALUES[j].d_msec);
                    if (veryVerbose) { T_();  P_(V);  P_(U); }
                    Obj w(V);  const Obj &W = w;          // control
                    Obj e;  const Obj &E = e;             // expected

                    e.setYearMonthDay(VALUES[i].d_year,
                                      VALUES[i].d_month,
                                      VALUES[i].d_day);
                    e.setTime(VALUES[j].d_hour,    VALUES[j].d_minute,
                              VALUES[j].d_second,  VALUES[j].d_msec);

                    u.setYearMonthDay(V.year(), V.month(), V.day());

                    if (veryVerbose) P(U);
                    LOOP2_ASSERT(i, j, E == U);
                    LOOP2_ASSERT(i, j, W == V);
                }
            }

            if (verbose) cout << "\nTesting setDate u = u (Aliasing)."
                              << endl;

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj u;  const Obj& U = u;
                u.setYearMonthDay(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
                u.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                Obj w(U);  const Obj &W = w;              // control
                u.setYearMonthDay(u.year(), u.month(), u.day());
                if (veryVerbose) { T_();  P_(U);  P(W); }
                LOOP_ASSERT(i, W == U);
            }
        }
      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING SETTIME:
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the parameter value.  Also, any object must be
        //   settable to itself.
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
        //   void setTime(const bdet_Time& time);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting setTime"
                          << "\n===============" << endl;

        if (verbose) cout << "\nTesting setTime u = V." << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day;
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {    1,  1,  1,  0,  0,  0,   0 },
                {   10,  4,  5,  0,  0,  0, 999 },
                {  100,  6,  7,  0,  0, 59,   0 },
                { 1000,  8,  9,  0, 59,  0,   0 },
                { 2000,  2, 29, 23,  0,  0,   0 },
                { 2002,  7,  4, 21, 22, 23, 209 },
                { 2003,  8,  5, 21, 22, 23, 210 },
                { 2004,  9,  3, 22, 44, 55, 888 },
                { 9999, 12, 31, 23, 59, 59, 999 },

                {    1,  1,  1, 24,  0,  0,   0 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int i;

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj v;  const Obj& V = v;
                v.setYearMonthDay(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
                v.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u;  const Obj& U = u;
                    u.setYearMonthDay(VALUES[j].d_year,
                                      VALUES[j].d_month,
                                      VALUES[j].d_day);
                    u.setTime(VALUES[j].d_hour,    VALUES[j].d_minute,
                              VALUES[j].d_second,  VALUES[j].d_msec);
                    if (veryVerbose) { T_();  P_(V);  P_(U); }
                    Obj w(V);  const Obj &W = w;          // control
                    Obj e;  const Obj &E = e;             // expected

                    e.setYearMonthDay(VALUES[j].d_year,
                                      VALUES[j].d_month,
                                      VALUES[j].d_day);
                    e.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                              VALUES[i].d_second,  VALUES[i].d_msec);

                    u.setTime(V.time());

                    if (veryVerbose) P(U);
                    LOOP2_ASSERT(i, j, E == U);
                    LOOP2_ASSERT(i, j, W == V);
                }
            }

            if (verbose) cout << "\nTesting setDate u = u (Aliasing)."
                              << endl;

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj u;  const Obj& U = u;
                u.setYearMonthDay(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
                u.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                Obj w(U);  const Obj &W = w;              // control
                u.setTime(u.time());
                if (veryVerbose) { T_();  P_(U);  P(W); }
                LOOP_ASSERT(i, W == U);
            }
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING ADDDAYS:
        //
        // Concerns:
        // TBD doc
        //
        // Plan:
        // TBD doc
        //
        // Testing:
        //   void addDays(int days);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting setDate"
                          << "\n===============" << endl;

        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year under test
                int d_day;      // day-of-year under test
                int d_days;     // days to add
                int d_expYear;  // expected Year
                int d_expDay;   // expected day
            } DATA[] = {
                //line  year   dayOfYr  days  exp  year  exp  dayOfYr
                //----  -----  -------  ----  ---------  ------------
                { L_,       1,       1,    1,         1,            2 },
                { L_,       1,       1,    3,         1,            4 },
                { L_,    9999,       1,    1,      9999,            2 },
                { L_,    9999,       1,    3,      9999,            4 },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // TBD need to verify time is not modified

            for (int di = 0; di < NUM_DATA; ++di) {
                const int LINE      = DATA[di].d_lineNum;
                const int YEAR      = DATA[di].d_year;
                const int DAY       = DATA[di].d_day;
                const int DAYS      = DATA[di].d_days;
                const int EXP_YEAR  = DATA[di].d_expYear;
                const int EXP_DAY   = DATA[di].d_expDay;
                if (veryVerbose) { T_();  P_(LINE);
                    P_(YEAR);  P_(DAY);   P_(DAYS);  P_(EXP_YEAR) P(EXP_DAY); }

                bdet_Date r;
                const bdet_Date& R = r;
                r.setYearDay(EXP_YEAR, EXP_DAY);

                Obj             x;
                const Obj&      X = x;

                x.setYearDay(YEAR, DAY);
                x.addDays(DAYS);
                if (veryVerbose) { T_();  P(X); }

                LOOP_ASSERT(LINE, R   == X.date());
            }
        }

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING SETDATE:
        //
        // Concerns:
        //   Any value must be assignable to an object having any initial value
        //   without affecting the parameter value.  Also, any object must be
        //   settable to itself.
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
        //   void setDate(const bdet_Date& date);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting setDate"
                          << "\n===============" << endl;

        if (verbose) cout << "\nTesting setDate u = V." << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day;
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {    1,  1,  1,  0,  0,  0,   0 },
                {   10,  4,  5,  0,  0,  0, 999 },
                {  100,  6,  7,  0,  0, 59,   0 },
                { 1000,  8,  9,  0, 59,  0,   0 },
                { 2000,  2, 29, 23,  0,  0,   0 },
                { 2002,  7,  4, 21, 22, 23, 209 },
                { 2003,  8,  5, 21, 22, 23, 210 },
                { 2004,  9,  3, 22, 44, 55, 888 },
                { 9999, 12, 31, 23, 59, 59, 999 },

                {    1,  1,  1, 24,  0,  0,   0 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int i;

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj v;  const Obj& V = v;
                v.setYearMonthDay(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
                v.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u;  const Obj& U = u;
                    u.setYearMonthDay(VALUES[j].d_year,
                                      VALUES[j].d_month,
                                      VALUES[j].d_day);
                    u.setTime(VALUES[j].d_hour,    VALUES[j].d_minute,
                              VALUES[j].d_second,  VALUES[j].d_msec);
                    if (veryVerbose) { T_();  P_(V);  P_(U); }
                    Obj w(V);  const Obj &W = w;          // control
                    Obj e;  const Obj &E = e;             // expected

                    e.setYearMonthDay(VALUES[i].d_year,
                                      VALUES[i].d_month,
                                      VALUES[i].d_day);
                    e.setTime(VALUES[j].d_hour,    VALUES[j].d_minute,
                              VALUES[j].d_second,  VALUES[j].d_msec);

                    u.setDate(V.date());

                    if (veryVerbose) P(U);
                    LOOP2_ASSERT(i, j, E == U);
                    LOOP2_ASSERT(i, j, W == V);
                }
            }

            if (verbose) cout << "\nTesting setDate u = u (Aliasing)."
                              << endl;

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj u;  const Obj& U = u;
                u.setYearMonthDay(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
                u.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                Obj w(U);  const Obj &W = w;              // control
                u.setDate(u.date());
                if (veryVerbose) { T_();  P_(U);  P(W); }
                LOOP_ASSERT(i, W == U);
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING  setYearDay  FUNCTION:
        //
        // Concerns:
        //   This method is implemented using tested functionality from
        //   'bdet_date'.  Therefore, only a few test vectors are needed.
        //
        //   We are concerned that the arguments are passed in the correct
        //   order (to the correct function).
        //
        // Plan:
        //   Construct a table of valid inputs and compare results with the
        //   expected values.  The test is taken directly from 'bdet_date'
        //   for convenience.
        //
        // Testing:
        //   int setYearDay(int year, int dayOfYear);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'setYearDay' Function"
                          << "\n=============================" << endl;

        if (verbose)
            cout << "\nTesting: 'setYearDay'" << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year under test
                int d_day;      // day-of-year under test
                int d_expMonth; // expected month
                int d_expDay;   // expected day
            } DATA[] = {
                //line no.  year   dayOfYr     exp  month   exp  day
                //-------   -----  -------     ----------   --------
                { L_,          1,       1,          1,         1 },
                { L_,          1,       2,          1,         2 },
                { L_,          1,      32,          2,         1 },
                { L_,          1,     365,         12,        31 },

                { L_,       1996,       1,          1,         1 },
                { L_,       1996,       2,          1,         2 },
                { L_,       1996,      32,          2,         1 },
                { L_,       1996,     365,         12,        30 },
                { L_,       1996,     366,         12,        31 },

                { L_,       9999,       1,          1,         1 },
                { L_,       9999,       2,          1,         2 },
                { L_,       9999,      32,          2,         1 },
                { L_,       9999,     365,         12,        31 },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA; ++di) {
                const int LINE      = DATA[di].d_lineNum;
                const int YEAR      = DATA[di].d_year;
                const int DAY       = DATA[di].d_day;
                const int EXP_MONTH = DATA[di].d_expMonth;
                const int EXP_DAY   = DATA[di].d_expDay;
                if (veryVerbose) { T_();  P_(LINE);
                    P_(YEAR);  P_(DAY);   P_(EXP_MONTH) P(EXP_DAY); }

                const bdet_Date R(YEAR, EXP_MONTH, EXP_DAY);
                Obj             x;
                const Obj&      X = x;

                x.setYearDay(YEAR, DAY);   if (veryVerbose) { T_();  P(X); }

                LOOP_ASSERT(LINE, R   == X.date());
            }
        }

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING setDatetimeIfValid :
        //
        // Concerns:
        //   Each of the seven integer fields must separately be able to
        //   cause a return of an "invalid" status, independent of other input.
        //   If the input is not invalid, the value must be correctly set.
        //   The underlying functionality is already tested.
        //
        // Plan:
        //   Construct a table of valid and invalid inputs and compare results
        //   to expected "valid" values.
        //
        // Testing:
        //   int setDatetimeIfValid(int, int, int, int, int, int, int);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'setDatetimeIfValid'"
                          << "\n============================"
                          << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                int         d_year;         // specification year
                int         d_month;        // specification month
                int         d_day;          // specification day
                int         d_hour;         // specification hour
                int         d_minute;       // specification minute
                int         d_second;       // specification second
                int         d_millisecond;  // specification millisecond
                int         d_valid;        // expected return value
            } DATA[] = {
                //line year   mo   day   hr   min  sec   msec   valid
                //---- ----   --   ---   --   ---  ---   ----   ------
                { L_,     1,   1,   1,    0,    0,   0,     0,    1   },

                { L_,     1,   1,   1,    0,    0,   0,   999,    1   },
                { L_,     1,   1,   1,    0,    0,  59,     0,    1   },
                { L_,     1,   1,   1,    0,   59,   0,     0,    1   },
                { L_,     1,   1,   1,   23,    0,   0,     0,    1   },
                { L_,     1,   1,  31,    0,    0,   0,     0,    1   },
                { L_,     1,  12,   1,    0,    0,   0,     0,    1   },
                { L_,  9999,   1,   1,    0,    0,   0,     0,    1   },

                { L_,     1,   1,   1,   24,    0,   0,     0,    1   },

                { L_,  1400,  10,   2,    7,   31,   2,    22,    1   },
                { L_,  2002,   8,  27,   14,    2,  48,   976,    1   },

                { L_,     1,   1,   1,    0,    0,   0,    -1,    0   },
                { L_,     1,   1,   1,    0,    0,  -1,     0,    0   },
                { L_,     1,   1,   1,    0,   -1,   0,     0,    0   },
                { L_,     1,   1,   1,   -1,    0,   0,     0,    0   },
                { L_,     1,   1,   0,    0,    0,   0,     0,    0   },
                { L_,     1,   0,   1,    0,    0,   0,     0,    0   },
                { L_,     0,   1,   1,    0,    0,   0,     0,    0   },

                { L_,     1,   1,   1,    0,    0,   0,  1000,    0   },
                { L_,     1,   1,   1,    0,    0,  60,     0,    0   },
                { L_,     1,   1,   1,    0,   60,   0,     0,    0   },
                { L_,     1,   1,   1,   25,    0,   0,     0,    0   },
                { L_,     1,   1,  32,    0,    0,   0,     0,    0   },
                { L_,     1,  13,   1,    0,    0,   0,     0,    0   },
                { L_, 10000,   1,   1,    0,    0,   0,     0,    0   },

                { L_,     1,   1,   1,   24,    0,   0,     1,    0   },
                { L_,     1,   1,   1,   24,    0,   1,     0,    0   },
                { L_,     1,   1,   1,   24,    1,   0,     0,    0   },

                { L_,     0,   0,   0,   -1,   -1,  -1,    -1,    0   },

                { L_,  1401,   2,  29,    7,   31,   2,    22,    0   },
                { L_,  2002,   2,  29,   14,    2,  48,    976,   0   },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int YEAR         = DATA[i].d_year;
                const int MONTH        = DATA[i].d_month;
                const int DAY          = DATA[i].d_day;
                const int HOUR         = DATA[i].d_hour;
                const int MINUTE       = DATA[i].d_minute;
                const int SECOND       = DATA[i].d_second;
                const int MILLISECOND  = DATA[i].d_millisecond;
                const int VALID        = DATA[i].d_valid;

                Obj x;  const Obj& X = x;
                if (1 == VALID) {
                    const Obj R(YEAR, MONTH, DAY,
                                HOUR, MINUTE, SECOND, MILLISECOND);
                    LOOP_ASSERT(LINE,
                                0 == x.setDatetimeIfValid(YEAR,
                                                          MONTH,
                                                          DAY,
                                                          HOUR,
                                                          MINUTE,
                                                          SECOND,
                                                          MILLISECOND));
                    LOOP_ASSERT(LINE, R == X);

                    if (verbose) { P_(VALID);  P_(R);  P(X); }
                }
                else {
                    const Obj R;
                    LOOP_ASSERT(LINE,
                                -1 == x.setDatetimeIfValid(YEAR,
                                                           MONTH,
                                                           DAY,
                                                           HOUR,
                                                           MINUTE,
                                                           SECOND,
                                                           MILLISECOND));
                    LOOP_ASSERT(LINE, R == X);

                    if (verbose) { P_(VALID);  P_(R);  P(X); }
                }
            }
        }

        if (verbose) cout << "\nTesting that 'setDatetimeIfValid' works "
                             "with just four arguments" << endl;
        {
            const int YEAR  = 2020;
            const int MONTH = 7;
            const int DAY   = 4;
            const int HOUR  = 8;
            const Obj R(YEAR, MONTH, DAY, HOUR);

            Obj x; const Obj& X = x;
            x.setDatetimeIfValid(YEAR, MONTH, DAY, HOUR);

            if (veryVerbose) P(X);
            ASSERT(R == X);
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING isValid
        //
        // Concerns:
        //   Each of the seven integer fields must separately be able to
        //   invalidate the whole (i.e., in the presence of otherwise good
        //   values).  The underlying methods (for date and time) are already
        //   tested.
        //
        // Plan:
        //   Construct a table of valid and invalid inputs and compare results
        //   to expected "valid" values.
        //
        // Testing:
        //   bool isValid(int y, int m, int d, int h, int m, int s, int ms);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'isValid'"
                          << "\n================="
                          << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                int         d_year;         // specification year
                int         d_month;        // specification month
                int         d_day;          // specification day
                int         d_hour;         // specification hour
                int         d_minute;       // specification minute
                int         d_second;       // specification second
                int         d_millisecond;  // specification millisecond
                int         d_valid;        // expected return value
            } DATA[] = {
                //line year   mo   day   hr   min  sec   msec   valid
                //---- ----   --   ---   --   ---  ---   ----   ------
                { L_,     1,   1,   1,    0,    0,   0,     0,    1   },

                { L_,     1,   1,   1,    0,    0,   0,   999,    1   },
                { L_,     1,   1,   1,    0,    0,  59,     0,    1   },
                { L_,     1,   1,   1,    0,   59,   0,     0,    1   },
                { L_,     1,   1,   1,   23,    0,   0,     0,    1   },
                { L_,     1,   1,  31,    0,    0,   0,     0,    1   },
                { L_,     1,  12,   1,    0,    0,   0,     0,    1   },
                { L_,  9999,   1,   1,    0,    0,   0,     0,    1   },

                { L_,     1,   1,   1,   24,    0,   0,     0,    1   },

                { L_,  1400,  10,   2,    7,   31,   2,    22,    1   },
                { L_,  2002,   8,  27,   14,    2,  48,   976,    1   },

                { L_,     1,   1,   1,    0,    0,   0,    -1,    0   },
                { L_,     1,   1,   1,    0,    0,  -1,     0,    0   },
                { L_,     1,   1,   1,    0,   -1,   0,     0,    0   },
                { L_,     1,   1,   1,   -1,    0,   0,     0,    0   },
                { L_,     1,   1,   0,    0,    0,   0,     0,    0   },
                { L_,     1,   0,   1,    0,    0,   0,     0,    0   },
                { L_,     0,   1,   1,    0,    0,   0,     0,    0   },

                { L_,     1,   1,   1,    0,    0,   0,  1000,    0   },
                { L_,     1,   1,   1,    0,    0,  60,     0,    0   },
                { L_,     1,   1,   1,    0,   60,   0,     0,    0   },
                { L_,     1,   1,   1,   25,    0,   0,     0,    0   },
                { L_,     1,   1,  32,    0,    0,   0,     0,    0   },
                { L_,     1,  13,   1,    0,    0,   0,     0,    0   },
                { L_, 10000,   1,   1,    0,    0,   0,     0,    0   },

                { L_,     1,   1,   1,   24,    0,   0,     1,    0   },
                { L_,     1,   1,   1,   24,    0,   1,     0,    0   },
                { L_,     1,   1,   1,   24,    1,   0,     0,    0   },

                { L_,     0,   0,   0,   -1,   -1,  -1,    -1,    0   },

                { L_,  1401,   2,  29,    7,   31,   2,    22,    0   },
                { L_,  2002,   2,  29,   14,    2,  48,    976,   0   },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int YEAR         = DATA[i].d_year;
                const int MONTH        = DATA[i].d_month;
                const int DAY          = DATA[i].d_day;
                const int HOUR         = DATA[i].d_hour;
                const int MINUTE       = DATA[i].d_minute;
                const int SECOND       = DATA[i].d_second;
                const int MILLISECOND  = DATA[i].d_millisecond;
                const int VALID        = DATA[i].d_valid;

                LOOP_ASSERT(LINE,
                            VALID == bdet_Datetime::isValid(YEAR,
                                                            MONTH,
                                                            DAY,
                                                            HOUR,
                                                            MINUTE,
                                                            SECOND,
                                                            MILLISECOND));
            }
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING ADDITION AND SUBTRACTION OPERATORS :
        //
        // Concerns:
        //   One operator performs a subtraction on the underlying date and
        //   time components of the datetime and returns a
        //   'bdet_DatetimeInterval' initialized with the sum of the
        //   differences between the date and time components of two datetimes.
        //   The other three operators perform addition and subtraction on one
        //   datetime and one interval value, and return a datetime value.
        //
        //   The subtraction methods of 'date' and 'time' are tested in their
        //   corresponding components.  We need only to test that the
        //   resulting sum is computed correctly.  We can then use the
        //   tested subtraction method and its data (along with basic algebra)
        //   to test the remaining three operators.
        //
        // Plan:
        //   Specify a set of object value pairs for the objects of types
        //   bdet_Date and bdet_Time.  Use these pairs to construct
        //   bdet_DateTime objects.  Verify that subtraction of the resulting
        //   objects produces the expected results.  Then use algebraic
        //   combinations of the inputs and results to test the remaining
        //   three operators taking a datetime and an interval.
        //
        // Testing:
        //   bdet_DtI operator-(const bdet_DateTime&, const bdet_DateTime&);
        //   bdet_DateTime operator-(const bdet_DateTime&, const bdet_DtI&);
        //   bdet_DateTime operator-(const bdet_DtI&, const bdet_DateTime&);
        //   bdet_DateTime operator-(const bdet_DateTime&, const bdet_DtI&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Subtraction Operator"
                          << "\n============================" << endl;
        {
            static const struct {
                int d_lineNum;   // source line number
                int d_year1;     // operand/result date1 year
                int d_month1;    // operand/result date1 month
                int d_day1;      // operand/result date1 day
                int d_numDays;   // operand/result 'int' number of days
                int d_year2;     // operand/result date2 year
                int d_month2;    // operand/result date2 month
                int d_day2;      // operand/result date2 day
            } DATE_DATA[] = {
                //          - - - -first- - - -           - - - second - - -
                //line no.  year   month   day   numDays  year   month   day
                //-------   -----  -----  -----  -------  -----  -----  -----
                { L_,          1,     1,     1,      1,      1,     1,     2 },
                { L_,         10,     2,    28,      1,     10,     3,     1 },
                { L_,        100,     3,    31,      2,    100,     4,     2 },

                { L_,       1000,     4,    30,      4,   1000,     5,     4 },
                { L_,       1000,     6,     1,    -31,   1000,     5,     1 },
                { L_,       1001,     1,     1,   -366,   1000,     1,     1 },
                { L_,       1100,     5,    31,     30,   1100,     6,    30 },
                { L_,       1200,     6,    30,     32,   1200,     8,     1 },

                { L_,       1996,     2,    28,    367,   1997,     3,     1 },
                { L_,       1997,     2,    28,    366,   1998,     3,     1 },
                { L_,       1998,     2,    28,    365,   1999,     2,    28 },
                { L_,       1999,     2,    28,    364,   2000,     2,    27 },
                { L_,       1999,     2,    28,   1096,   2002,     2,    28 },
                { L_,       2002,     2,    28,  -1096,   1999,     2,    28 },

            };

            const int NUM_DATE_DATA = sizeof DATE_DATA / sizeof *DATE_DATA;

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
            } TIME_DATA[] = {
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

            };

            const int NUM_TIME_DATA = sizeof TIME_DATA / sizeof *TIME_DATA;

            for (int di = 0; di < NUM_DATE_DATA ; ++di) {
                bdet_Date date1(DATE_DATA[di].d_year1, DATE_DATA[di].d_month1,
                      DATE_DATA[di].d_day1);
                bdet_Date date2(DATE_DATA[di].d_year2, DATE_DATA[di].d_month2,
                      DATE_DATA[di].d_day2);

                bsls_Types::Int64 dateDiff = date2 - date1;
                dateDiff = dateDiff * (24 * 60 * 60 * 1000);

                for (int i = 0; i < NUM_TIME_DATA; ++i) {
                    const int LINE     = TIME_DATA[i].d_lineNum;
                    const int HOURS2   = TIME_DATA[i].d_hours2;
                    const int MINUTES2 = TIME_DATA[i].d_minutes2;
                    const int SECONDS2 = TIME_DATA[i].d_seconds2;
                    const int MSECS2   = TIME_DATA[i].d_msecs2;
                    const int HOURS1   = TIME_DATA[i].d_hours1;
                    const int MINUTES1 = TIME_DATA[i].d_minutes1;
                    const int SECONDS1 = TIME_DATA[i].d_seconds1;
                    const int MSECS1   = TIME_DATA[i].d_msecs1;
                    const int EXP_MSEC = TIME_DATA[i].d_expMsec;

                    const bdet_Time time1(HOURS2, MINUTES2, SECONDS2, MSECS2);
                    const bdet_Time time2(HOURS1, MINUTES1, SECONDS1, MSECS1);

                    const Obj X1(date1, time1);
                    const Obj X2(date2, time2);

                    const Obj X3(date1, time2);
                    const Obj X4(date2, time1);

                    const bdet_DatetimeInterval INTERVAL1(X1 - X2);
                    const bdet_DatetimeInterval INTERVAL2(X2 - X1);
                    const bdet_DatetimeInterval INTERVAL3(X3 - X4);
                    const bdet_DatetimeInterval INTERVAL4(X4 - X3);

                    if (veryVerbose) {
                        T_();  P(INTERVAL1.totalMilliseconds());
                        T_();  P(INTERVAL2.totalMilliseconds());
                        T_();  P(INTERVAL3.totalMilliseconds());
                        T_();  P(INTERVAL4.totalMilliseconds());
                    }

                    LOOP_ASSERT(LINE,
                        EXP_MSEC - dateDiff == INTERVAL1.totalMilliseconds());

                    LOOP_ASSERT(LINE,
                        -EXP_MSEC + dateDiff == INTERVAL2.totalMilliseconds());

                    LOOP_ASSERT(LINE,
                        -EXP_MSEC - dateDiff == INTERVAL3.totalMilliseconds());

                    LOOP_ASSERT(LINE,
                        EXP_MSEC + dateDiff == INTERVAL4.totalMilliseconds());

                    if (veryVerbose) {
                        T_(); P_(X3); P_(X4); P(X4 - INTERVAL4); }
                    LOOP_ASSERT(LINE, X1 == INTERVAL1 + X2);
                    LOOP_ASSERT(LINE, X1 == X2 + INTERVAL1);
                    LOOP_ASSERT(LINE, X1 == X2 - INTERVAL2);

                    LOOP_ASSERT(LINE, X2 == INTERVAL2 + X1);
                    LOOP_ASSERT(LINE, X2 == X1 + INTERVAL2);
                    LOOP_ASSERT(LINE, X2 == X1 - INTERVAL1);

                    LOOP_ASSERT(LINE, X3 == INTERVAL3 + X4);
                    LOOP_ASSERT(LINE, X3 == X4 + INTERVAL3);
                    LOOP_ASSERT(LINE, X3 == X4 - INTERVAL4);

                    LOOP_ASSERT(LINE, X4 == INTERVAL4 + X3);
                    LOOP_ASSERT(LINE, X4 == X3 + INTERVAL4);
                    LOOP_ASSERT(LINE, X4 == X3 - INTERVAL3);

                }
            }
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING ADDITION AND SUBTRACTION ASSIGNMENT OPERATORS :
        //
        // Concerns:
        //   We want to be able to add/subtract arbitrary intervals to
        //   arbitrary datetime values, subject to the overflow constraints.
        //   We are not here too concerned with the "corners" of the
        //   constraints.  A later test can explore those.
        //
        // Plan:
        //   Specify a set S of initial datetime values and a set T of
        //   datetime *intervals*.  For each (s, t) in S x T, use the operators
        //   under test to add/subtract t from a local copy of s.  Use the
        //   tested 'addMilliseconds' on t.totalMilliseconds to confirm
        //   the results.
        //
        // Testing:
        //   bdet_DateTime& operator+=(const bdet_DatetimeInterval&);
        //   bdet_DateTime& operator-=(const bdet_DatetimeInterval&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Arithmetic Assignment Operators"
                          << "\n======================================="
                          << endl;
        {
            static const struct {
                int d_year;  int d_month;   int d_day;
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } INITIAL_VALUES[] = {
                // {    5,  1,  1,   0,  0,  0,   0 },
                {   10,  4,  5,   0,  0,  0,   1 },
                {  100,  6,  7,   0,  0,  0, 999},
                { 1000,  8,  9,   0,  0,  1,   0 },
                { 2000,  1, 31,   0,  0, 59, 999},
                { 2002,  7,  4,   0,  1,  0,   0 },
                { 2002, 12, 31,   0,  1,  0,   1 },
                { 2003,  1,  1,   0, 59, 59, 999 },
                { 2003,  1,  2,   1,  0,  0,   0},
                { 2003,  8,  5,   1,  0,  0,   1},
                { 2003,  8,  6,  23,  0,  0,   0},
                { 2003,  8,  7,  23, 22, 21, 209},
                { 2004,  9,  3,  23, 22, 21, 210},
                { 2004,  9,  4,  23, 22, 21, 211},
                { 9990, 12, 31,  23, 59, 59, 999 },
             };

            const int NUM_INIT_VALUES =
                sizeof INITIAL_VALUES / sizeof *INITIAL_VALUES;

            static const struct {
                int d_days;
                int d_hours;
                int d_minutes;
                int d_seconds;
                int d_msecs;
            } INTERVAL_VALUES[] = {
                { -1001,    0,   0,   0,    0 },
                { -1000,  -23, -59, -59, -999 },
                { -1000,  -23, -59, -59, -998 },
                {   -10,   25,  80,  70,   -1 },
                {   -10,   25,  80,  70,    0 },
                {   -10,   25,  80,  70,    1 },
                {    -1,    0,  -1,   0,    0 },
                {    -1,    0,   0,   0,    0 },
                {    -1,    0,   1,   0,    0 },
                {     0,    0,   0,   0,    0 },
                {     1,    0,  -1,   0,    0 },
                {     1,    0,   0,   0,    0 },
                {     1,    0,   1,   0,    0 },
                {    10,   25,  80,  70,   -1 },
                {    10,   25,  80,  70,    0 },
                {    10,   25,  80,  70,    1 },
                {  1000,   23,  59,  59,  998 },
                {  1000,   23,  59,  59,  999 },
                {  1001,    0,   0,   0,    0 },
             };

            const int NUM_INTERVAL_VALUES =
                sizeof INTERVAL_VALUES / sizeof *INTERVAL_VALUES;

            for (int i = 0; i < NUM_INIT_VALUES; ++i) {
                const Obj R(INITIAL_VALUES[i].d_year,
                            INITIAL_VALUES[i].d_month,
                            INITIAL_VALUES[i].d_day,
                            INITIAL_VALUES[i].d_hour,
                            INITIAL_VALUES[i].d_minute,
                            INITIAL_VALUES[i].d_second,
                            INITIAL_VALUES[i].d_msec);

                for (int j = 0; j < NUM_INTERVAL_VALUES; ++j) {
                    Obj x(R);  const Obj& X = x;
                    Obj y(R);  const Obj& Y = y;
                    bdet_DatetimeInterval u;
                    u.setInterval(INTERVAL_VALUES[j].d_days,
                                  INTERVAL_VALUES[j].d_hours,
                                  INTERVAL_VALUES[j].d_minutes,
                                  INTERVAL_VALUES[j].d_seconds,
                                  INTERVAL_VALUES[j].d_msecs);

                    const bsls_Types::Int64 INTERVAL_MSEC =
                                                         u.totalMilliseconds();

                    if (veryVerbose) { P(INTERVAL_MSEC); }

                    x += u;
                    y.addMilliseconds(INTERVAL_MSEC);

                    if (veryVerbose) { P_(X);  P(Y); }

                    LOOP2_ASSERT(i, j, Y == X);

                    // 'operator-=' should just undo the change, yielding 'R'
                    x -= u;
                    LOOP2_ASSERT(i, j, R == X);
                }
            }
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'add' METHODS:
        //
        // Concerns:
        //   Each 'add' method forwards its arguments to the corresponding
        //   method of the contained 'bdet_Time' object, and uses its return
        //   value to augment the contained 'bdet_Date' value via 'bdet_Date's
        //   'operator+=' method.  The concerns are only that methods and
        //   arguments match as intended.  A very few test vectors for each
        //   method will suffice for a thorough test.  Because all of the
        //   underlying methods are tested, a single, convenient, initial
        //   datetime value is sufficient.
        //
        // Plan:
        //   First test 'addTime' explicitly using tabulated data, and then use
        //   the tested 'addTime' as an oracle to test the other four methods
        //   in a loop-based test.
        //
        //   Specify as a convenient initial value a datetime having a day
        //   field value of 15, a month that has 31 days, and a time field
        //   value of 00:00:00.000.  Specify a set S of (h, m, s, ms) tuples
        //   such that | h + m + s + ms | < 14 days (so that at most the day
        //   field value of the initial datetime will change).  Use each s in
        //   S as the arguments to 'addTime' and verify that the modified
        //   object value is correct.  In particular, tabulate the expected
        //   change in days, construct an "expected 'bdet_Date' value" from
        //   that tabulated value and the initial "reference" 'bdet_Date'
        //   value, and use that 'bdet_Date' value to construct an expected
        //   datetime value to compare to the initial object value modified by
        //   the 'addTime' method.
        //
        //   Specify a (negative) start value, a (positive) stop value, and a
        //   step size for each of the four fields (h, m, s, ms).  Loop over
        //   each of these four ranges, calling the appropriate 'add' method
        //   on the default object value and a second non-default object value,
        //   and using 'addTime' as an oracle to verify the resulting object
        //   values and return values for the 'add' method under test.
        //
        // Testing:
        //   void addTime(int hours, int minutes, int seconds, int msec);
        //   void addHours(int hours);
        //   void addMinutes(int minutes);
        //   void addSeconds(int seconds);
        //   void addMilliseconds(int milliseconds);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'add' Methods"
                          << "\n=====================" << endl;

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

            if (verbose) cout << "\nTesting: 'addTime'" << endl;
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int REFERENCE_YEAR  = 2000;
            const int REFERENCE_MONTH = 1;
            const int REFERENCE_DAY   = 15;

            const bdet_Date INITIAL_DATE(REFERENCE_YEAR, REFERENCE_MONTH,
                                         REFERENCE_DAY);
            const bdet_Time INITIAL_TIME;

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

                Obj x(INITIAL_DATE, INITIAL_TIME);  const Obj& X = x;
                if (veryVerbose) { T_();  P_(X); }
                x.addTime(HOURS, MINUTES, SECONDS, MSECS);
                const bdet_Date EXP_D(REFERENCE_YEAR, REFERENCE_MONTH,
                                      REFERENCE_DAY + EXP_DAYS);
                const bdet_Time EXP_T(EXP_HR, EXP_MIN, EXP_SEC, EXP_MSEC);
                const Obj EXP(EXP_D, EXP_T);
                if (veryVerbose) { P_(X);  P(EXP); }
                LOOP_ASSERT(LINE, EXP      == X);
            }
        }

        {
            const int REFERENCE_YEAR  = 2000;
            const int REFERENCE_MONTH = 1;
            const int REFERENCE_DAY   = 15;

            const bdet_Date ID(REFERENCE_YEAR, REFERENCE_MONTH, REFERENCE_DAY);
            const bdet_Time IT;

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
                Obj x(ID, IT), y(ID, IT);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addHours(hi);
                y.addTime(hi, 0, 0, 0);
                if (veryVerbose) { P_(X);  P_(Y);  P(hi); }

                LOOP_ASSERT(hi, Y == X);
            }

            if (verbose) cout << "\nTesting: 'addMinutes'" << endl;
            for (int mi = START_MINS; mi <= STOP_MINS; mi += STEP_MINS) {
                Obj x(ID, IT), y(ID, IT);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addMinutes(mi);
                y.addTime(0, mi, 0, 0);
                if (veryVerbose) { P_(X);  P_(Y);  P(mi); }

                LOOP_ASSERT(mi, Y == X);
            }

            if (verbose) cout << "\nTesting: 'addSeconds'" << endl;
            for (int si = START_SECS; si <= STOP_SECS; si += STEP_SECS) {
                Obj x(ID, IT), y(ID, IT);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addSeconds(si);
                y.addTime(0, 0, si, 0);
                if (veryVerbose) { P_(X);  P_(Y);  P(si); }

                LOOP_ASSERT(si, Y == X);
            }

            if (verbose) cout << "\nTesting: 'addMilliseconds'" << endl;
             for (int msi = START_MSECS; msi <= STOP_MSECS; msi += STEP_MSECS){
                Obj x(ID, IT), y(ID, IT);  const Obj &X = x, &Y = y;
                if (veryVerbose) { T_();  P_(X); }

                x.addMilliseconds(msi);
                y.addTime(0, 0, 0, msi);
                if (veryVerbose) { P_(X);  P_(Y);  P(msi); }

                LOOP_ASSERT(msi, Y == X);
            }

        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING RELATIONAL OPERATORS (<, <=, >=, >):
        //
        // Concerns:
        //   Each operator implements a logical expression on the contained
        //   'bdet_Date' and 'bdet_Time' values, which must be verified.
        //
        // Plan:
        //   Specify an ordered set S of unique object values.  For each (u, v)
        //   in the set S x S, verify the result of u OP v for each OP in
        //   {<, <=, >=, >}.
        //
        // Testing:
        //  bool operator< (const bdet_Datetime& lhs, const bdet_Datetime& rhs)
        //  bool operator<=(const bdet_Datetime& lhs, const bdet_Datetime& rhs)
        //  bool operator>=(const bdet_Datetime& lhs, const bdet_Datetime& rhs)
        //  bool operator> (const bdet_Datetime& lhs, const bdet_Datetime& rhs)
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Relational Operators"
                          << "\n============================" << endl;

        if (verbose) cout <<
            "\nTesting: 'operator<', 'operator<=', 'operator>=', 'operator>'"
                          << endl;
        {
            static const struct {
                int d_year;  int d_month;   int d_day;
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {    1,  1,  1,   0,  0,  0,   0 },
                {   10,  4,  5,   0,  0,  0,   1 },
                {  100,  6,  7,   0,  0,  0, 999},
                { 1000,  8,  9,   0,  0,  1,   0 },
                { 2000,  1, 31,   0,  0, 59, 999},
                { 2002,  7,  4,   0,  1,  0,   0 },
                { 2002, 12, 31,   0,  1,  0,   1 },
                { 2003,  1,  1,   0, 59, 59, 999 },
                { 2003,  1,  2,   1,  0,  0,   0},
                { 2003,  8,  5,   1,  0,  0,   1},
                { 2003,  8,  6,  23,  0,  0,   0},
                { 2003,  8,  7,  23, 22, 21, 209},
                { 2004,  9,  3,  23, 22, 21, 210},
                { 2004,  9,  4,  23, 22, 21, 211},
                { 9999, 12, 31,  23, 59, 59, 999 },
             };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj v;  const Obj& V = v;
                v.setYearMonthDay(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
                v.setTime(VALUES[i].d_hour,   VALUES[i].d_minute,
                          VALUES[i].d_second, VALUES[i].d_msec);

                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u;  const Obj& U = u;
                    u.setYearMonthDay(VALUES[j].d_year,
                                      VALUES[j].d_month,
                                      VALUES[j].d_day);
                    u.setTime(VALUES[j].d_hour,   VALUES[j].d_minute,
                              VALUES[j].d_second, VALUES[j].d_msec);
                    if (veryVerbose) { T_();  P_(i);  P_(j);  P_(V);  P(U); }
                    LOOP2_ASSERT(i, j, j <  i == U <  V);
                    LOOP2_ASSERT(i, j, j <= i == U <= V);
                    LOOP2_ASSERT(i, j, j >= i == U >= V);
                    LOOP2_ASSERT(i, j, j >  i == U >  V);
                }
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING ADDITIONAL SETTING MANIPULATORS:
        //
        // Concerns:
        //   Each method forwards its parameter(s) to the underlying method(s)
        //   on the contained 'bdet_Date' and 'bdet_Time' objects.  The only
        //   concern is that all methods and arguments match correctly.
        //
        // Plan:
        //   For the "time-only" methods:
        //   First, for a set of independent test values not including the
        //   default value (24:00:00.000), use the default constructor to
        //   create an object and use the 'set' manipulators to set its value.
        //   Verify the value using the basic accessors.
        //
        //   For the seven-integer-arguments method:
        //   Use the data set from the tested 7-arc c'tor, and use that c'tor
        //   as an oracle to verify that the 'setDatetime' method can set the
        //   default object to the correct value.
        //
        // Testing:
        //   void setHour(int hour);
        //   void setMinute(int minute);
        //   void setSecond(int second);
        //   void setMillisecond(int millisecond);
        //   void setDatetime(int, int, int, int, int, int, int);
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
                { 23, 59, 59, 999  },  // 24:00:00.000 NOT tested here
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
                LOOP_ASSERT(i, HOUR   == X.time().hour());
                LOOP_ASSERT(i, MINUTE == X.time().minute());
                LOOP_ASSERT(i, SECOND == X.time().second());
                LOOP_ASSERT(i, MSEC   == X.time().millisecond());
            }
            if (veryVerbose) cout << endl;
        }

        if (verbose) cout << "\tSetting from value 24:00:00.000." << endl;
        {
            const Obj R24;             // Reference object, time = 24:00:00.000

            Obj x;  const Obj& X = x;  if (veryVerbose) { T_();  P_(X); }
            x.setMinute(0);            if (veryVerbose) P(X);
            ASSERT(0 == X.time().hour());
            ASSERT(0 == X.time().minute());
            ASSERT(0 == X.time().second());
            ASSERT(0 == X.time().millisecond());

            x = R24;                   if (veryVerbose) { T_();  P_(X); }
            x.setMinute(59);           if (veryVerbose) P(X);
            ASSERT( 0 == X.time().hour());
            ASSERT(59 == X.time().minute());
            ASSERT( 0 == X.time().second());
            ASSERT( 0 == X.time().millisecond());

            x = R24;                   if (veryVerbose) { T_();  P_(X); }
            x.setSecond(0);            if (veryVerbose) P(X);
            ASSERT(0 == X.time().hour());
            ASSERT(0 == X.time().minute());
            ASSERT(0 == X.time().second());
            ASSERT(0 == X.time().millisecond());

            x = R24;                   if (veryVerbose) { T_();  P_(X); }
            x.setSecond(59);           if (veryVerbose) P(X);
            ASSERT( 0 == X.time().hour());
            ASSERT( 0 == X.time().minute());
            ASSERT(59 == X.time().second());
            ASSERT( 0 == X.time().millisecond());

            x = R24;                   if (veryVerbose) { T_();  P_(X); }
            x.setMillisecond(0);       if (veryVerbose) P(X);
            ASSERT(0 == X.time().hour());
            ASSERT(0 == X.time().minute());
            ASSERT(0 == X.time().second());
            ASSERT(0 == X.time().millisecond());

            x = R24;                   if (veryVerbose) { T_();  P_(X); }
            x.setMillisecond(999);     if (veryVerbose) { P(X); cout << endl; }
            ASSERT(  0 == X.time().hour());
            ASSERT(  0 == X.time().minute());
            ASSERT(  0 == X.time().second());
            ASSERT(999 == X.time().millisecond());
        }

        if (verbose) cout << "\tSetting to value 24:00:00.000." << endl;
        {
            const bdet_Date RD(2000, 1, 1);      // Ref date (01JAN2000)
            const bdet_Time RT(23, 22, 21, 209); // Ref time (21:22:21.209)
            const Obj R(RD, RT);                 // Reference object

            Obj x(R); const Obj& X = x;if (veryVerbose) { T_();  P_(X); }
            x.setHour(24);             if (veryVerbose) { P(X); cout << endl; }
            ASSERT(24 == X.time().hour());
            ASSERT( 0 == X.time().minute());
            ASSERT( 0 == X.time().second());
            ASSERT( 0 == X.time().millisecond());

        }

        if (verbose)
            cout << "\nTesting: 'setDatetime(7 x int)'" << endl;
        {
            static const struct {
                int d_year;  int d_month;   int d_day;
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {    1,  1,  1,   0,  0,  0,   0 },
                {   10,  4,  5,   0,  0,  0, 999 },
                {  100,  6,  7,   0,  0, 59,   0 },
                { 1000,  8,  9,   0, 59,  0,   0 },
                { 1100,  1, 31,  23,  0,  0,   0 },
                { 2000, 10, 31,  23, 22, 21, 209 },
                { 2100, 11, 30,  23, 22, 21, 210 },
                { 2200, 12, 31,  23, 22, 21, 211 },
                { 2400, 12, 31,  23, 22, 21, 211 },
                { 9999, 12, 31,  23, 59, 59, 999 },

                {    1,  1,  1,  24,  0,  0,   0 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES ; ++i) {
                const Obj R(VALUES[i].d_year,
                            VALUES[i].d_month,
                            VALUES[i].d_day,
                            VALUES[i].d_hour,
                            VALUES[i].d_minute,
                            VALUES[i].d_second,
                            VALUES[i].d_msec); // Reference object

                Obj x;  const Obj& X = x;

                x.setDatetime(VALUES[i].d_year,
                              VALUES[i].d_month,
                              VALUES[i].d_day,
                              VALUES[i].d_hour,
                              VALUES[i].d_minute,
                              VALUES[i].d_second,
                              VALUES[i].d_msec);

                if (veryVerbose) { T_();  P_(R);  P(X); }
                LOOP_ASSERT(i, R == X);
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIALIZATION CONSTRUCTORS:
        //
        // Concerns:
        //   The constructor must correctly forward its parameters to
        //   appropriate tested constructors thereby correctly initializing the
        //   data members.
        //
        // Plan:
        //   For the constructor taking a date:
        //   Specify a set S of dates as (y, m, d) triplets having widely
        //   varying values.  For each (y, m, d) in S, construct a date object,
        //   use that to construct a datetime X, and verify that X has the
        //   expected value.
        //
        //   For the constructor taking a date and a time:
        //   Specify a set S of datetime values as (y, m, d, h, m, s, ms)
        //   7- tuples having widely varying values.  For each element in S,
        //   construct a datetime object X and verify that X has the
        //   expected value.
        //
        //   For the constructor taking seven integers:
        //   Specify a set S of datetime values as (y, m, d, h, m, s, ms)
        //   7- tuples having widely varying values.  For each element in S,
        //   construct a datetime object X and verify that X has the
        //   expected value.
        //
        // Testing:
        //   bdet_Datetime(const bdet_Date& date);
        //   bdet_Datetime(const bdet_Date& date, const bdet_Time& time);
        //   bdet_Datetime(int y, int m, int d, int h , int m , int s, int ms);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Initialization Constructors"
                          << "\n===================================" << endl;

        if (verbose)
            cout << "\nTesting: 'bdet_Datetime(date)'" << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day;
            } VALUES[] = {
                {    1,  1,  1 }, {   10,  4,  5 }, {  100,  6,  7 },
                { 1000,  8,  9 }, { 1100,  1, 31 }, { 1200,  2, 29 },
                { 1300,  3, 31 }, { 1400,  4, 30 }, { 1500,  5, 31 },
                { 1600,  6, 30 }, { 1700,  7, 31 }, { 1800,  8, 31 },
                { 1900,  9, 30 }, { 2000, 10, 31 }, { 2100, 11, 30 },
                { 2200, 12, 31 }, { 2400, 12, 31 }, { 9999, 12, 31 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES ; ++i) {
                const bdet_Date D(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
                const Obj X(D);
                if (veryVerbose) { T_();  P(X); }
                LOOP_ASSERT(i, VALUES[i].d_year  == X.date().year());
                LOOP_ASSERT(i, VALUES[i].d_month == X.date().month());
                LOOP_ASSERT(i, VALUES[i].d_day   == X.date().day());
                LOOP_ASSERT(i, 0                 == X.time().hour());
                LOOP_ASSERT(i, 0                 == X.time().minute());
                LOOP_ASSERT(i, 0                 == X.time().second());
                LOOP_ASSERT(i, 0                 == X.time().millisecond());
            }
        }

        if (verbose)
            cout << "\nTesting: 'bdet_Datetime(date, time)'" << endl;
        {
            static const struct {
                int d_year;  int d_month;   int d_day;
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {    1,  1,  1,   0,  0,  0,   0 },
                {   10,  4,  5,   0,  0,  0, 999 },
                {  100,  6,  7,   0,  0, 59,   0 },
                { 1000,  8,  9,   0, 59,  0,   0 },
                { 1100,  1, 31,  23,  0,  0,   0 },
                { 2000, 10, 31,  23, 22, 21, 209 },
                { 2100, 11, 30,  23, 22, 21, 210 },
                { 2200, 12, 31,  23, 22, 21, 211 },
                { 2400, 12, 31,  23, 22, 21, 211 },
                { 9999, 12, 31,  23, 59, 59, 999 },

                {    1,  1,  1,  24,  0,  0,   0 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES ; ++i) {
                const bdet_Date D(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
                const bdet_Time T(VALUES[i].d_hour, VALUES[i].d_minute,
                                  VALUES[i].d_second, VALUES[i].d_msec);
                const Obj X(D, T);
                if (veryVerbose) { T_();  P(X); }
                LOOP_ASSERT(i, VALUES[i].d_year   == X.date().year());
                LOOP_ASSERT(i, VALUES[i].d_month  == X.date().month());
                LOOP_ASSERT(i, VALUES[i].d_day    == X.date().day());
                LOOP_ASSERT(i, VALUES[i].d_hour   == X.time().hour());
                LOOP_ASSERT(i, VALUES[i].d_minute == X.time().minute());
                LOOP_ASSERT(i, VALUES[i].d_second == X.time().second());
                LOOP_ASSERT(i, VALUES[i].d_msec   == X.time().millisecond());
            }
        }

        if (verbose)
            cout << "\nTesting: 'bdet_Datetime(7 x int)'" << endl;
        {
            static const struct {
                int d_year;  int d_month;   int d_day;
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {    1,  1,  1,   0,  0,  0,   0 },
                {   10,  4,  5,   0,  0,  0, 999 },
                {  100,  6,  7,   0,  0, 59,   0 },
                { 1000,  8,  9,   0, 59,  0,   0 },
                { 1100,  1, 31,  23,  0,  0,   0 },
                { 2000, 10, 31,  23, 22, 21, 209 },
                { 2100, 11, 30,  23, 22, 21, 210 },
                { 2200, 12, 31,  23, 22, 21, 211 },
                { 2400, 12, 31,  23, 22, 21, 211 },
                { 9999, 12, 31,  23, 59, 59, 999 },

                {    1,  1,  1,  24,  0,  0,   0 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES ; ++i) {
                const Obj X(VALUES[i].d_year,
                            VALUES[i].d_month,
                            VALUES[i].d_day,
                            VALUES[i].d_hour,
                            VALUES[i].d_minute,
                            VALUES[i].d_second,
                            VALUES[i].d_msec);

                const Obj Y(VALUES[i].d_year,
                            VALUES[i].d_month,
                            VALUES[i].d_day,
                            VALUES[i].d_hour);

                if (veryVerbose) { T_();  P(X); }
                LOOP_ASSERT(i, VALUES[i].d_year   == X.date().year());
                LOOP_ASSERT(i, VALUES[i].d_month  == X.date().month());
                LOOP_ASSERT(i, VALUES[i].d_day    == X.date().day());
                LOOP_ASSERT(i, VALUES[i].d_hour   == X.time().hour());
                LOOP_ASSERT(i, VALUES[i].d_minute == X.time().minute());
                LOOP_ASSERT(i, VALUES[i].d_second == X.time().second());
                LOOP_ASSERT(i, VALUES[i].d_msec   == X.time().millisecond());

                LOOP_ASSERT(i, VALUES[i].d_year   == Y.date().year());
                LOOP_ASSERT(i, VALUES[i].d_month  == Y.date().month());
                LOOP_ASSERT(i, VALUES[i].d_day    == Y.date().day());
                LOOP_ASSERT(i, VALUES[i].d_hour   == Y.time().hour());
                LOOP_ASSERT(i, 0                  == Y.time().minute());
                LOOP_ASSERT(i, 0                  == Y.time().second());
                LOOP_ASSERT(i, 0                  == Y.time().millisecond());

            }
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
        //   int maxSupportedBdexVersion() const;
        //   BDEX STREAMING
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Streaming Functionality"
                          << "\n===============================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // scalar and array object values for various stream tests
        typedef bdet_Date Date;         typedef bdet_Time Time;

        const Date DA(   1,  1,  1);    const Time TA(  0,  0,  0,   0);
        const Date DB(1776,  7,  4);    const Time TB(  0,  0,  0, 999);
        const Date DC(1956,  4, 30);    const Time TC(  0,  0, 59,   0);
        const Date DD(1958,  4, 30);    const Time TD(  0, 59,  0,   0);
        const Date DE(2002,  3, 25);    const Time TE( 23,  0,  0,   0);
        const Date DF(9999, 12, 31);    const Time TF( 23, 22, 21, 209);

        const Obj VA(DA, TA);
        const Obj VB(DB, TB);
        const Obj VC(DC, TC);
        const Obj VD(DD, TD);
        const Obj VE(DE, TE);
        const Obj VF(DF, TF);

        const int NUM_VALUES = 6;
        const Obj VALUES[NUM_VALUES] = { VA, VB, VC, VD, VE, VF };
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

                // Verify that each new value overwrites every old value
                // and that the input stream is emptied, but remains valid.

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

        const Obj W;               // default value (serial date = 1, time = 0)
        Obj temp;
        temp.addMilliseconds(1);
        const Obj X(temp);         // control value (serial date = 1, time = 1)
        temp.addMilliseconds(1);
        const Obj Y(temp);         // new value (serial date = 1, time = 2)

        const int DATE_Y = 1;      // internal rep of Y.d_date
        const int TIME_Y = 2;      // internal rep of Y.d_time

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            const char version = 1;

            Out out;
            out.putInt24(DATE_Y);
            out.putInt32(TIME_Y);
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(in);
                             ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        if (verbose) cout << "\t\t\t'bdet_Datetime' version too small." <<endl;
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out;
            out.putInt24(DATE_Y);
            out.putInt32(TIME_Y);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, version);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tBad value (date too small)." << endl;
        {
            Out out;
            out.putInt24(0);
            out.putInt32(TIME_Y);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tBad value (date too large)." << endl;
        {
            const int  MAX_DATE = bdeimp_DateUtil::ymd2serial(9999, 12, 31);

            Out out;
            out.putInt24(MAX_DATE + 1);
            out.putInt32(TIME_Y);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tBad value (time too small)." << endl;
        {
            Out out;
            out.putInt24(DATE_Y);
            out.putInt32(-1);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tBad value (time too large)." << endl;
        {
            Out out;
            out.putInt24(DATE_Y);
            out.putInt32(24*60*60*1000 + 1);

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setQuiet(!veryVerbose);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);  ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int         d_lineNum;      // source line number
                int         d_year;         // specification year
                int         d_month;        // specification month
                int         d_day;          // specification day
                int         d_hour;         // specification hour
                int         d_minute;       // specification minute
                int         d_second;       // specification second
                int         d_millisecond;  // specification millisecond
                int         d_version;      // version to stream with
                int         d_length;       // expect output length
                const char *d_fmt_p;        // expected output format
            } DATA[] = {
                //line year  mo  d   h   mi  s   ms   ver len format
                //---- ----  --  --  --  --  --  ---  --- --- --------------
                { L_,  1400, 10,  2,  7, 31,  2,  22,  0,  0, ""             },
                { L_,  2002,  8, 27, 14,  2, 48, 976,  0,  0, ""             },

                { L_,  1400, 10,  2,  7, 31,  2,  22,  1,  7,
                                              "\x07\xcd\x1c\x01\x9c\xef\x06" },
                { L_,  2002,  8, 27, 14,  2, 48, 976,  1,  7,
                                              "\x0b\x27\xd3\x03\x03\x9f\x10" },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE         = DATA[i].d_lineNum;
                const int YEAR         = DATA[i].d_year;
                const int MONTH        = DATA[i].d_month;
                const int DAY          = DATA[i].d_day;
                const int HOUR         = DATA[i].d_hour;
                const int MINUTE       = DATA[i].d_minute;
                const int SECOND       = DATA[i].d_second;
                const int MILLISECOND  = DATA[i].d_millisecond;
                const int VERSION      = DATA[i].d_version;
                const int LEN          = DATA[i].d_length;
                const char *const FMT  = DATA[i].d_fmt_p;

                const bdet_Date D(YEAR, MONTH, DAY);
                const bdet_Time T(HOUR, MINUTE, SECOND, MILLISECOND);
                Obj mX(D, T);  const Obj& X = mX;
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
        //   bdet_Datetime& operator=(const bdet_Datetime& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Assignment Operator"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting Assignment u = V." << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day;
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {    1,  1,  1,  0,  0,  0,   0 },
                {   10,  4,  5,  0,  0,  0, 999 },
                {  100,  6,  7,  0,  0, 59,   0 },
                { 1000,  8,  9,  0, 59,  0,   0 },
                { 2000,  2, 29, 23,  0,  0,   0 },
                { 2002,  7,  4, 21, 22, 23, 209 },
                { 2003,  8,  5, 21, 22, 23, 210 },
                { 2004,  9,  3, 22, 44, 55, 888 },
                { 9999, 12, 31, 23, 59, 59, 999 },

                {    1,  1,  1, 24,  0,  0,   0 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;
            int i;

            for (i = 0; i < NUM_VALUES; ++i) {
                Obj v;  const Obj& V = v;
                v.setYearMonthDay(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
                v.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj u;  const Obj& U = u;
                    u.setYearMonthDay(VALUES[j].d_year,
                                       VALUES[j].d_month,
                                       VALUES[j].d_day);
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
                u.setYearMonthDay(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
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
                { L_,    0,         -1,     "01JAN0001_24:00:00.000"         },

                { L_,    0,          0,     "01JAN0001_24:00:00.000"      NL },

                { L_,    0,          2,     "01JAN0001_24:00:00.000"      NL },

                { L_,    1,          1,     " 01JAN0001_24:00:00.000"     NL },

                { L_,    1,          2,     "  01JAN0001_24:00:00.000"    NL },

                { L_,   -1,          2,     "01JAN0001_24:00:00.000"      NL },

                { L_,   -2,          1,     "01JAN0001_24:00:00.000"      NL },

                { L_,    2,          1,     "  01JAN0001_24:00:00.000"    NL },

                { L_,    1,          3,     "   01JAN0001_24:00:00.000"   NL },
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

            const bdet_Date D;  // 01JAN0001
            const bdet_Time T;  // 24:00:00.000

            Obj mX(D, T);  const Obj& X = mX;  // 01JAN0001_24:00:00.000

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

        if (verbose) cout << "\nTesting 'print' with "
                             "manipulators and left alignment." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                int         d_year;     // year field value
                int         d_month;    // month field value
                int         d_day;      // day field value
                int         d_hour;     // hour field value
                int         d_minute;   // minute field value
                int         d_second;   // second field value
                int         d_msec;     // millisecond field value
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
            //--^
            //line year mon day  hr min sec msec      output format
            //---- ---- --- ---  -- --- --- ----  ------------------------
            { L_,    1,  1,  1,   0,  0,  0,   0,
                                        "01JAN0001_00:00:00.000@@@@@@@@" },
            { L_, 1999,  1,  1,  23, 22, 21, 209,
                                        "01JAN1999_23:22:21.209@@@@@@@@" },
            { L_, 2000,  2,  1,  23, 22, 21, 210,
                                        "01FEB2000_23:22:21.210@@@@@@@@" },
            { L_, 2001,  3,  1,  23, 22, 21, 211,
                                        "01MAR2001_23:22:21.211@@@@@@@@" },
            { L_, 9999, 12, 31,  23, 59, 59, 999,
                                        "31DEC9999_23:59:59.999@@@@@@@@" },
            //--v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char XX = (char) 0xFF;  // Value used for an unset char.
            char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
            const char *CTRL_BUF = mCtrlBuf; // Used for extra character check.

            const char    FILL_CHAR = '@'; // Used for filling whitespaces due
                                           // to 'setw'.

            const int  FORMAT_WIDTH = 30;

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int         LINE   = DATA[di].d_lineNum;
                const int         YEAR   = DATA[di].d_year;
                const int         MONTH  = DATA[di].d_month;
                const int         DAY    = DATA[di].d_day;
                const int         HOUR   = DATA[di].d_hour;
                const int         MINUTE = DATA[di].d_minute;
                const int         SECOND = DATA[di].d_second;
                const int         MSEC   = DATA[di].d_msec;
                const char *const FMT    = DATA[di].d_fmt_p;

                char buf[SIZE];
                memcpy(buf, CTRL_BUF, SIZE); // Preset buf to 'unset' values.

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(YEAR, MONTH, DAY);
                x.setTime(HOUR, MINUTE, SECOND, MSEC);

                if (veryVerbose) cout << "\tEXPECTED FORMAT: " << FMT << endl;
                ostrstream out(buf, SIZE);

                out << left << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                X.print(out, 0, -1);
                out << ends;

                if (veryVerbose) cout << "\tACTUAL FORMAT:   " << buf << endl;

                const int SZ = strlen(FMT) + 1;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, XX == buf[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE,  0 == memcmp(buf, FMT, SZ));
                LOOP_ASSERT(LINE,
                            0 == memcmp(buf + SZ, CTRL_BUF + SZ, SIZE-SZ));
            }
        }

        if (verbose) cout << "\nTesting 'print' "
                             "with manipulators and right alignment" << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                int         d_year;     // year field value
                int         d_month;    // month field value
                int         d_day;      // day field value
                int         d_hour;     // hour field value
                int         d_minute;   // minute field value
                int         d_second;   // second field value
                int         d_msec;     // millisecond field value
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
            //--^
            //line year mon day  hr min sec msec      output format
            //---- ---- --- ---  -- --- --- ----  ------------------------
            { L_,    1,  1,  1,   0,  0,  0,   0,
                                        "@@@@@@@@01JAN0001_00:00:00.000" },
            { L_, 1999,  1,  1,  23, 22, 21, 209,
                                        "@@@@@@@@01JAN1999_23:22:21.209" },
            { L_, 2000,  2,  1,  23, 22, 21, 210,
                                        "@@@@@@@@01FEB2000_23:22:21.210" },
            { L_, 2001,  3,  1,  23, 22, 21, 211,
                                        "@@@@@@@@01MAR2001_23:22:21.211" },
            { L_, 9999, 12, 31,  23, 59, 59, 999,
                                        "@@@@@@@@31DEC9999_23:59:59.999" },
            //--v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char XX = (char) 0xFF;  // Value used for an unset char.
            char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
            const char *CTRL_BUF = mCtrlBuf; // Used for extra character check.

            const char    FILL_CHAR = '@'; // Used for filling whitespaces due
                                           // to 'setw'.

            const int  FORMAT_WIDTH = 30;

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int         LINE   = DATA[di].d_lineNum;
                const int         YEAR   = DATA[di].d_year;
                const int         MONTH  = DATA[di].d_month;
                const int         DAY    = DATA[di].d_day;
                const int         HOUR   = DATA[di].d_hour;
                const int         MINUTE = DATA[di].d_minute;
                const int         SECOND = DATA[di].d_second;
                const int         MSEC   = DATA[di].d_msec;
                const char *const FMT    = DATA[di].d_fmt_p;

                char buf[SIZE];
                memcpy(buf, CTRL_BUF, SIZE); // Preset buf to 'unset' values.

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(YEAR, MONTH, DAY);
                x.setTime(HOUR, MINUTE, SECOND, MSEC);

                if (veryVerbose) cout << "\tEXPECTED FORMAT: " << FMT << endl;
                ostrstream out(buf, SIZE);

                out << right << setfill(FILL_CHAR) << setw(FORMAT_WIDTH);
                X.print(out, 0, -1);
                out << ends;

                if (veryVerbose) cout << "\tACTUAL FORMAT:   " << buf << endl;

                const int SZ = strlen(FMT) + 1;
                LOOP_ASSERT(LINE, SZ < SIZE);  // Check buffer is large enough.
                LOOP_ASSERT(LINE, XX == buf[SIZE - 1]);  // Check for overrun.
                LOOP_ASSERT(LINE,  0 == memcmp(buf, FMT, SZ));
                LOOP_ASSERT(LINE,
                            0 == memcmp(buf + SZ, CTRL_BUF + SZ, SIZE-SZ));
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
        //   bdet_Datetime(const bdet_Datetime& original);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Copy Constructor"
                          << "\n========================" << endl;

        if (verbose) cout << "\nTesting copy constructor." << endl;
        {
            static const struct {
                int d_year;  int d_month;   int d_day;
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {    1,  1,  1,  0,  0,  0,   0 },
                {   10,  4,  5,  0,  0,  0, 999 },
                {  100,  6,  7,  0,  0, 59,   0 },
                { 1000,  8,  9,  0, 59,  0,   0 },
                { 2000,  2, 29, 23,  0,  0,   0 },
                { 2002,  7,  4, 21, 22, 23, 209 },
                { 2003,  8,  5, 21, 22, 23, 210 },
                { 2004,  9,  3, 22, 44, 55, 888 },
                { 9999, 12, 31, 23, 59, 59, 999 },

                {    1,  1,  1, 24,  0,  0,   0 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj w;  const Obj& W = w;           // control
                w.setYearMonthDay(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
                w.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(VALUES[i].d_year, VALUES[i].d_month,
                                  VALUES[i].d_day);
                x.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec)   ;

                Obj y(X);  const Obj &Y = y;
                if (veryVerbose) { T_();  P_(W);  P_(X);  P(Y); }
                LOOP_ASSERT(i, X == W);  LOOP_ASSERT(i, Y == W);
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
        //   even though tested operators are used on the date and time fields
        //   atomically.
        //
        // Plan:
        //   Specify a set S of unique object values having various minor or
        //   subtle differences.  Verify the correctness of 'operator==' and
        //   'operator!=' using all elements (u, v) of the cross product
        //    S X S.
        //
        // Testing:
        //   operator==(const bdet_Datetime&, const bdet_Datetime&);
        //   operator!=(const bdet_Datetime&, const bdet_Datetime&);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Equality Operators"
                          << "\n==========================" << endl;

        if (verbose) cout <<
            "\nCompare each pair of values (u, v) in S X S." << endl;
        {
            static const struct {
                int d_year;  int d_month;   int d_day;
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {    1,  1,  1,   1,  1,  1,   1 },
                {    1,  1,  1,   1,  1,  1,   2 },
                {    1,  1,  1,   1,  1,  2,   1 },
                {    1,  1,  1,   1,  2,  1,   1 },
                {    1,  1,  1,   2,  1,  1,   1 },
                {    1,  1,  2,   1,  1,  1,   1 },
                {    1,  2,  1,   1,  1,  1,   1 },
                {    2,  1,  1,   1,  1,  1,   1 },

                { 9999, 12, 31,  23, 59, 59, 999 },
                { 9999, 12, 31,  23, 59, 59, 998 },
                { 9999, 12, 31,  23, 59, 58, 999 },
                { 9999, 12, 31,  23, 58, 59, 999 },
                { 9999, 12, 31,  22, 59, 59, 999 },
                { 9999, 12, 30,  23, 59, 59, 999 },
                { 9999, 11, 30,  23, 59, 59, 999 },
                { 9998, 12, 31,  23, 59, 59, 999 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                Obj u;  const Obj& U = u;
                u.setYearMonthDay(VALUES[i].d_year,
                                  VALUES[i].d_month,
                                  VALUES[i].d_day);
                u.setTime(VALUES[i].d_hour,    VALUES[i].d_minute,
                          VALUES[i].d_second,  VALUES[i].d_msec);
                for (int j = 0; j < NUM_VALUES; ++j) {
                    Obj v;  const Obj& V = v;
                    v.setYearMonthDay(VALUES[j].d_year,
                                      VALUES[j].d_month,
                                      VALUES[j].d_day);
                    v.setTime(VALUES[j].d_hour,   VALUES[j].d_minute,
                              VALUES[j].d_second, VALUES[j].d_msec);
                    bool isSame = i == j;
                    if (veryVerbose) { P_(i); P_(j); P_(U); P(V); }
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
        //   The output operator is trivially implemented using the
        //   fully-tested date and time output operators; a very few test
        //   vectors can thoroughly test this functionality.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   operator<<(ostream&, const bdet_Datetime&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                int         d_year;     // year field value
                int         d_month;    // month field value
                int         d_day;      // day field value
                int         d_hour;     // hour field value
                int         d_minute;   // minute field value
                int         d_second;   // second field value
                int         d_msec;     // millisecond field value
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
            //--^
            //line year mon day  hr min sec msec      output format
            //---- ---- --- ---  -- --- --- ----  ------------------------
            { L_,    1,  1,  1,   0,  0,  0,   0, "01JAN0001_00:00:00.000" },
            { L_, 1999,  1,  1,  23, 22, 21, 209, "01JAN1999_23:22:21.209" },
            { L_, 2000,  2,  1,  23, 22, 21, 210, "01FEB2000_23:22:21.210" },
            { L_, 2001,  3,  1,  23, 22, 21, 211, "01MAR2001_23:22:21.211" },
            { L_, 9999, 12, 31,  23, 59, 59, 999, "31DEC9999_23:59:59.999" },
            //--v
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000; // Must be big enough to hold output string.
            const char XX = (char) 0xFF;  // Value used for an unset char.
            char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
            const char *CTRL_BUF = mCtrlBuf; // Used for extra character check.

            for (int di = 0; di < NUM_DATA;  ++di) {
                const int         LINE   = DATA[di].d_lineNum;
                const int         YEAR   = DATA[di].d_year;
                const int         MONTH  = DATA[di].d_month;
                const int         DAY    = DATA[di].d_day;
                const int         HOUR   = DATA[di].d_hour;
                const int         MINUTE = DATA[di].d_minute;
                const int         SECOND = DATA[di].d_second;
                const int         MSEC   = DATA[di].d_msec;
                const char *const FMT    = DATA[di].d_fmt_p;

                char buf[SIZE];
                memcpy(buf, CTRL_BUF, SIZE); // Preset buf to 'unset' values.

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(YEAR, MONTH, DAY);
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
        //   The two basic accessors must provide appropriate references to the
        //   fully-tested contained 'bdet_Date' and 'bdet_Time' data members.
        //
        // Plan:
        //   For each of a sequence of unique object values, verify that each
        //   of the basic accessors returns a reference to its associated
        //   contained data member.  Use the respective 'bdet_Date' and
        //   'bdet_Time' accessors to confirm the datetime object values.
        //
        // Testing:
        //   const bdet_Date& date() const;
        //   const bdet_Time& time() const;
        //   int year() const;
        //   int month() const;
        //   int day() const;
        //   int dayOfYear() const;
        //   int dayOfWeek() const;
        //   int hour() const;
        //   int minute() const;
        //   int second() const;
        //   int millisecond() const;
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTESTING BASIC ACCESSORS"
                          << "\n=======================" << endl;

        if (verbose) cout << "\nTesting: 'date', 'time'" << endl;
        {
            static const struct {
                int d_year;  int d_month;   int d_day;
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {    1,  1,  1,     0,  0,  0,   0  },
                { 1700,  7, 31,    23, 22, 21, 206  },
                { 1800,  8, 31,    23, 22, 21, 207  },
                { 1900,  9, 30,    23, 22, 21, 208  },
                { 2000, 10, 31,    23, 22, 21, 209  },
                { 2100, 11, 30,    23, 22, 21, 210  },
                { 9999, 12, 31,    23, 59, 59, 999  },

                {    1,  1,  1,    24,  0,  0,   0  },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int YEAR   = VALUES[i].d_year;
                const int MONTH  = VALUES[i].d_month;
                const int DAY    = VALUES[i].d_day;
                const int HOUR   = VALUES[i].d_hour;
                const int MINUTE = VALUES[i].d_minute;
                const int SECOND = VALUES[i].d_second;
                const int MSEC   = VALUES[i].d_msec;

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(YEAR, MONTH, DAY);
                x.setTime(HOUR, MINUTE, SECOND, MSEC);

                if (veryVerbose) {
                    T_();  P_(YEAR);  P_(MONTH);   P(DAY);
                    T_();  P_(HOUR);  P_(MINUTE);  P_(SECOND);  P(MSEC);
                    T_();  P(X);
                }

                LOOP_ASSERT(i, YEAR   == X.date().year());
                LOOP_ASSERT(i, MONTH  == X.date().month());
                LOOP_ASSERT(i, DAY    == X.date().day());
                LOOP_ASSERT(i, HOUR   == X.time().hour());
                LOOP_ASSERT(i, MINUTE == X.time().minute());
                LOOP_ASSERT(i, SECOND == X.time().second());
                LOOP_ASSERT(i, MSEC   == X.time().millisecond());

                LOOP_ASSERT(i, YEAR   == X.year());
                LOOP_ASSERT(i, MONTH  == X.month());
                LOOP_ASSERT(i, DAY    == X.day());
                LOOP_ASSERT(i, HOUR   == X.hour());
                LOOP_ASSERT(i, MINUTE == X.minute());
                LOOP_ASSERT(i, SECOND == X.second());
                LOOP_ASSERT(i, MSEC   == X.millisecond());

                LOOP_ASSERT(i, X.date().dayOfYear() == X.dayOfYear());
                LOOP_ASSERT(i, X.date().dayOfWeek() == X.dayOfWeek());
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg':
        //   Void for 'bdet_datetime'.
        // --------------------------------------------------------------------

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS (BOOTSTRAP):
        //
        // Concerns:
        //   All of the date field manipulation is accomplished through the
        //   'date' method.  The time manipulator forwards its input to the
        //   fully-tested 'bdet_time' method, so only minimal testing is needed
        //   to guarantee correct parameter-forwarding.
        //
        // Plan:
        //   First, verify the default constructor by testing the value of the
        //   resulting object.
        //
        //   Next, for a sequence of independent test values, use the default
        //   constructor to create an object and use the primary manipulator
        //   to set its value.  Verify that value using the basic accessors.
        //   Note that the destructor is exercised on each configuration as the
        //   object being tested leaves scope.
        //
        // Testing:
        //   bdet_Datetime();
        //   ~bdet_Datetime();
        //   bdet_Date& date();
        //   void setTime(int hour = 0, int min = 0, int sec = 0, int ms = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting Primary Manipulator"
                          << "\n===========================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;

        Obj x;  const Obj& X = x;        if (veryVerbose) P(X);

        ASSERT( 1 == X.date().year());    ASSERT(1 == X.date().month());
        ASSERT( 1 == X.date().day());
        ASSERT(24 == X.time().hour());    ASSERT(0 == X.time().minute());
        ASSERT( 0 == X.time().second());  ASSERT(0 == X.time().millisecond());

        if (verbose) cout << "\nTesting 'setTime'." << endl;
        {
            static const struct {
                int d_hour;  int d_minute;  int d_second;  int d_msec;
            } VALUES[] = {
                {  0,  0,  0,   0  },  {  0,  0,  0, 999  },
                {  0,  0, 59,   0  },  {  0, 59,  0,   0  },
                { 23,  0,  0,   0  },  { 23, 22, 21, 209  },
                { 23, 59, 59, 999  },  { 24,  0,  0,   0  },
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
                LOOP_ASSERT(i, HOUR   == X.time().hour());
                LOOP_ASSERT(i, MINUTE == X.time().minute());
                LOOP_ASSERT(i, SECOND == X.time().second());
                LOOP_ASSERT(i, MSEC   == X.time().millisecond());

                x.setTime(HOUR, MINUTE, SECOND);
                if (veryVerbose) {
                    T_();  P_(HOUR);  P_(MINUTE);  P_(SECOND);  P(X); }
                LOOP_ASSERT(i, HOUR   == X.time().hour());
                LOOP_ASSERT(i, MINUTE == X.time().minute());
                LOOP_ASSERT(i, SECOND == X.time().second());
                LOOP_ASSERT(i, 0      == X.time().millisecond());

                x.setTime(HOUR, MINUTE);
                if (veryVerbose) { T_();  P_(HOUR);  P_(MINUTE);  P(X); }
                LOOP_ASSERT(i, HOUR   == X.time().hour());
                LOOP_ASSERT(i, MINUTE == X.time().minute());
                LOOP_ASSERT(i, 0      == X.time().second());
                LOOP_ASSERT(i, 0      == X.time().millisecond());

                x.setTime(HOUR);
                if (veryVerbose) { T_();  P_(HOUR);  P(X); }
                LOOP_ASSERT(i, HOUR   == X.time().hour());
                LOOP_ASSERT(i, 0      == X.time().minute());
                LOOP_ASSERT(i, 0      == X.time().second());
                LOOP_ASSERT(i, 0      == X.time().millisecond());

            }
        }

        if (verbose) cout << "\nTesting 'date'." << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day;
            } VALUES[] = {
                {    1,  1,  1 }, {   10,  4,  5 }, {  100,  6,  7 },
                { 1000,  8,  9 }, { 1100,  1, 31 }, { 1200,  2, 29 },
                { 1300,  3, 31 }, { 1400,  4, 30 }, { 1500,  5, 31 },
                { 1600,  6, 30 }, { 1700,  7, 31 }, { 1800,  8, 31 },
                { 1900,  9, 30 }, { 2000, 10, 31 }, { 2100, 11, 30 },
                { 2200, 12, 31 }, { 2400, 12, 31 }, { 9999, 12, 31 },
            };

            const int NUM_VALUES = sizeof VALUES / sizeof *VALUES;

            for (int i = 0; i < NUM_VALUES; ++i) {
                const int YEAR  = VALUES[i].d_year;
                const int MONTH = VALUES[i].d_month;
                const int DAY   = VALUES[i].d_day;

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(YEAR, MONTH, DAY);
                if (veryVerbose) { T_(); P_(YEAR); P_(MONTH); P_(DAY); P(X) }

                LOOP_ASSERT(i, YEAR  == X.date().year());
                LOOP_ASSERT(i, MONTH == X.date().month());
                LOOP_ASSERT(i, DAY   == X.date().day());
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
        // 1. Create an object x1 (init to VA).     { x1:VA }
        // 2. Create an object x2 (copy from x1).   { x1:VA x2:VA }
        // 3. Set x1 to VB.                         { x1:VB x2:VA }
        // 4. Create an object x3 (default ctor).   { x1:VB x2:VA x3:U }
        // 5. Create an object x4 (copy from x3).   { x1:VB x2:VA x3:U  x4:U }
        // 6. Set x3 to VC.                         { x1:VB x2:VA x3:VC x4:U }
        // 7. Assign x2 = x1.                       { x1:VB x2:VB x3:VC x4:U }
        // 8. Assign x2 = x3.                       { x1:VB x2:VC x3:VC x4:U }
        // 9. Assign x1 = x1 (aliasing).            { x1:VB x2:VB x3:VC x4:U }
        //
        //   In step 10, exercise the constructor that takes a 'bdet_Date'
        //   object, but sets the time to the default value.  Confirm that the
        //   time can subsequently be set to a new value.
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

        int year, month, day;              // reusable variables for 'get' call
        int h, m, s, ms;                   // reusable variables for 'get' call

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(DA, TA);  const Obj& X1 = mX1;
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X1.date().getYearMonthDay(&year, &month, &day);
        X1.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRA == year);     ASSERT(MOA == month);     ASSERT(DAA == day);
        ASSERT(HRA == h);        ASSERT(MIA == m);
        ASSERT(SCA == s);        ASSERT(MSA == ms);
        ASSERT(YRA == X1.date().year());
        ASSERT(MOA == X1.date().month());
        ASSERT(DAA == X1.date().day());
        ASSERT(HRA == X1.time().hour());
        ASSERT(MIA == X1.time().minute());
        ASSERT(SCA == X1.time().second());
        ASSERT(MSA == X1.time().millisecond());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (verbose) { cout << '\t';  P(X2); }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X2.date().getYearMonthDay(&year, &month, &day);
        X2.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRA == year);     ASSERT(MOA == month);     ASSERT(DAA == day);
        ASSERT(HRA == h);        ASSERT(MIA == m);
        ASSERT(SCA == s);        ASSERT(MSA == ms);
        ASSERT(YRA == X2.date().year());
        ASSERT(MOA == X2.date().month());
        ASSERT(DAA == X2.date().day());
        ASSERT(HRA == X2.time().hour());
        ASSERT(MIA == X2.time().minute());
        ASSERT(SCA == X2.time().second());
        ASSERT(MSA == X2.time().millisecond());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setDate(DB);
        mX1.setTime(HRB, MIB, SCB, MSB);
        if (verbose) { cout << '\t';  P(X1); }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X1.date().getYearMonthDay(&year, &month, &day);
        X1.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRB == year);     ASSERT(MOB == month);     ASSERT(DAB == day);
        ASSERT(HRB == h);        ASSERT(MIB == m);
        ASSERT(SCB == s);        ASSERT(MSB == ms);
        ASSERT(YRB == X1.date().year());
        ASSERT(MOB == X1.date().month());
        ASSERT(DAB == X1.date().day());
        ASSERT(HRB == X1.time().hour());
        ASSERT(MIB == X1.time().minute());
        ASSERT(SCB == X1.time().second());
        ASSERT(MSB == X1.time().millisecond());

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
        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X3.date().getYearMonthDay(&year, &month, &day);
        X3.time().getTime(&h, &m, &s, &ms);
        ASSERT(1 == year);     ASSERT(1 == month);     ASSERT(1 == day);
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
        if (verbose) { cout << '\t';  P(X4); }

        if (verbose) cout << "\ta. Check initial state of x4." << endl;
        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X4.date().getYearMonthDay(&year, &month, &day);
        X4.time().getTime(&h, &m, &s, &ms);
        ASSERT(1 == year);     ASSERT(1 == month);     ASSERT(1 == day);
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
        mX3.setDate(DC);
        mX3.setTime(HRC, MIC, SCC, MSC);
        if (verbose) { cout << '\t';  P(X3); }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X3.date().getYearMonthDay(&year, &month, &day);
        X3.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRC == year);     ASSERT(MOC == month);     ASSERT(DAC == day);
        ASSERT(HRC == h);        ASSERT(MIC == m);
        ASSERT(SCC == s);        ASSERT(MSC == ms);
        ASSERT(YRC == X3.date().year());
        ASSERT(MOC == X3.date().month());
        ASSERT(DAC == X3.date().day());
        ASSERT(HRC == X3.time().hour());
        ASSERT(MIC == X3.time().minute());
        ASSERT(SCC == X3.time().second());
        ASSERT(MSC == X3.time().millisecond());

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
        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X2.date().getYearMonthDay(&year, &month, &day);
        X2.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRB == year);     ASSERT(MOB == month);     ASSERT(DAB == day);
        ASSERT(HRB == h);        ASSERT(MIB == m);
        ASSERT(SCB == s);        ASSERT(MSB == ms);
        ASSERT(YRB == X2.date().year());
        ASSERT(MOB == X2.date().month());
        ASSERT(DAB == X2.date().day());
        ASSERT(HRB == X2.time().hour());
        ASSERT(MIB == X2.time().minute());
        ASSERT(SCB == X2.time().second());
        ASSERT(MSB == X2.time().millisecond());

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
        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X2.date().getYearMonthDay(&year, &month, &day);
        X2.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRC == year);     ASSERT(MOC == month);     ASSERT(DAC == day);
        ASSERT(HRC == h);        ASSERT(MIC == m);
        ASSERT(SCC == s);        ASSERT(MSC == ms);
        ASSERT(YRC == X2.date().year());
        ASSERT(MOC == X2.date().month());
        ASSERT(DAC == X2.date().day());
        ASSERT(HRC == X2.time().hour());
        ASSERT(MIC == X2.time().minute());
        ASSERT(SCC == X2.time().second());
        ASSERT(MSC == X2.time().millisecond());

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
        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X1.date().getYearMonthDay(&year, &month, &day);
        X1.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRB == year);     ASSERT(MOB == month);     ASSERT(DAB == day);
        ASSERT(HRB == h);        ASSERT(MIB == m);
        ASSERT(SCB == s);        ASSERT(MSB == ms);
        ASSERT(YRB == X1.date().year());
        ASSERT(MOB == X1.date().month());
        ASSERT(DAB == X1.date().day());
        ASSERT(HRB == X1.time().hour());
        ASSERT(MIB == X1.time().minute());
        ASSERT(SCB == X1.time().second());
        ASSERT(MSB == X1.time().millisecond());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 10. Create an object x5 initialized to a "
                             "date, but with the default time value." << endl;

        Obj mX5(DB);  const Obj &X5 = mX5;
        if (verbose) { cout << '\t';  P(X5); }

        if (verbose) cout << "\ta. Check the state of x5." << endl;
        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X5.date().getYearMonthDay(&year, &month, &day);
        X5.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRB == year);     ASSERT(MOB == month);     ASSERT(DAB == day);
        ASSERT(  0 == h);        ASSERT(  0 == m);
        ASSERT(  0 == s);        ASSERT(  0 == ms);
        ASSERT(YRB == X5.date().year());
        ASSERT(MOB == X5.date().month());
        ASSERT(DAB == X5.date().day());
        ASSERT(  0 == X5.time().hour());
        ASSERT(  0 == X5.time().minute());
        ASSERT(  0 == X5.time().second());
        ASSERT(  0 == X5.time().millisecond());

        if (verbose) cout << "\tb. Modify the time field of x5." << endl;
        mX5.setTime(HRB, MIB, SCB, MSB);
        if (verbose) { cout << '\t';  P(X5); }
        year = 0;  month = 0;  day = 0;  h = 0;  m = 0;  s = 0;  ms = 0;
        X5.date().getYearMonthDay(&year, &month, &day);
        X5.time().getTime(&h, &m, &s, &ms);
        ASSERT(YRB == year);     ASSERT(MOB == month);     ASSERT(DAB == day);
        ASSERT(HRB == h);        ASSERT(MIB == m);
        ASSERT(SCB == s);        ASSERT(MSB == ms);
        ASSERT(YRB == X5.date().year());
        ASSERT(MOB == X5.date().month());
        ASSERT(DAB == X5.date().day());
        ASSERT(HRB == X5.time().hour());
        ASSERT(MIB == X5.time().minute());
        ASSERT(SCB == X5.time().second());
        ASSERT(MSB == X5.time().millisecond());

        ASSERT(1 == (X5 == X1));        ASSERT(0 == (X5 != X1));
        ASSERT(0 == (X5 == X2));        ASSERT(1 == (X5 != X2));
        ASSERT(0 == (X5 == X3));        ASSERT(1 == (X5 != X3));
        ASSERT(0 == (X5 == X4));        ASSERT(1 == (X5 != X4));
        ASSERT(1 == (X5 == X5));        ASSERT(0 == (X5 != X5));

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
