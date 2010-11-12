// bdet_date.t.cpp                                                    -*-C++-*-

#include <bdet_date.h>

#include <bdex_byteinstream.h>           // for testing only
#include <bdex_byteoutstream.h>          // for testing only
#include <bdex_instreamfunctions.h>      // for testing only
#include <bdex_outstreamfunctions.h>     // for testing only
#include <bdex_testinstream.h>           // for testing only
#include <bdex_testinstreamexception.h>  // for testing only
#include <bdex_testoutstream.h>          // for testing only

#include <bsls_platformutil.h>           // for testing only

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strcmp()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test is a value-semantic scalar whose state is
// identically its value.  Moreover, most of the complex functionality is
// implemented using a fully-tested implementation utility component
// ('bdeimp_dateutil'), and there is no allocator involved.  As such, our
// testing concerns are (safely) limited to the mechanical functioning of the
// various methods and free operators, and exception neutrality during 'bdex'
// streaming.
//
// The component interface represents a date value as three integer fields, and
// so it is necessary always to verify that each of these parameters is
// correctly forwarded to the (fully-tested) conversion utilities; the
// underlying representation is a single "serial date" integer.
//
// The primary manipulators for 'bdet_Date' are the default constructor and
// 'setYearMonthDay'.  The basic accessors are 'year' , 'month', 'day',
// and 'getYearMonthDay'.
// ----------------------------------------------------------------------------
// CLASS METHODS
// [11] static bool isValid(int year, int month, int day);
// [10] static int maxSupportedBdexVersion() const;
//
// CREATORS
// [ 2] bdet_Date();
// [12] bdet_Date(int year, int month, int day);
// [ 7] bdet_Date(const bdet_Date& original);
//
// MANIPULATORS
// [ 9] bdet_Date& operator=(const bdet_Date& rhs);
// [13] bdet_Date& operator++();
// [13] bdet_Date& operator--();
// [13] bdet_Date operator++(int);
// [13] bdet_Date operator--(int);
// [14] bdet_Date& operator+=(int numDays);
// [14] bdet_Date& operator-=(int numDays);
// [ 2] void setYearMonthDay(int year, int month, int day);
// [17] int setYearMonthDayIfValid(int year, int month, int day);
// [18] void setYearDay(int year, int dayOfYear);
// [18] int setYearDayIfValid(int year, int dayOfYear);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
//
// ACCESSORS
// [ 4] void getYearMonthDay(int *year, int *month, int *day) const;
// [ 4] int day() const;
// [ 4] int month() const;
// [ 4] int year() const;
// [18] int dayOfYear() const;
// [16] bdet_DayOfWeek::Day dayOfWeek() const;
// [ 8] bsl::ostream& print(bsl::ostream& stream, int lvl, int sp) const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
//
// FREE OPERATORS
// [14] bdet_Date operator+(const bdet_Date& date, int numDays);
// [14] bdet_Date operator+(int numDays, const bdet_Date& date);
// [14] bdet_Date operator-(const bdet_Date& date, int numDays);
// [14] int operator-(const bdet_Date& lhs, const bdet_Date& rhs);
// [ 6] bool operator==(const bdet_Date& lhs, const bdet_Date& rhs);
// [ 6] bool operator!=(const bdet_Date& lhs, const bdet_Date& rhs);
// [15] bool operator< (const bdet_Date& lhs, const bdet_Date& rhs);
// [15] bool operator<=(const bdet_Date& lhs, const bdet_Date& rhs);
// [15] bool operator> (const bdet_Date& lhs, const bdet_Date& rhs);
// [15] bool operator>=(const bdet_Date& lhs, const bdet_Date& rhs);
// [ 5] bsl::ostream& operator<<(bsl::ostream&, const bdet_Date&);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [-1] PERFORMANCE: 'getYearMonthDay'
// [-2] PERFORMANCE: 'month()'
// [19] USAGE EXAMPLE
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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
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
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\n"; \
       aSsErT(1, #X, __LINE__); } }

#define LOOP6_ASSERT(I,J,K,L,M,N,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" << \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" << \
       #M << ": " << M << "\t" << #N << ": " << N << "\n"; \
       aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline
#define L_ __LINE__                           // current Line number

//=============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
//-----------------------------------------------------------------------------
typedef bdet_Date          Obj;
typedef bdex_TestInStream  In;
typedef bdex_TestOutStream Out;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 21: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE TEST
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

        if (verbose) cout << endl << "USAGE EXAMPLE TEST" << endl
                                  << "==================" << endl;

        if (verbose) cout << "\nTesting usage example." << endl;

///Usage
///-----
// The following snippets of code illustrate how to create and use a
// 'bdet_date' object.  First create a default date 'd1':
//..
    bdet_Date d1;                     ASSERT(   1 == d1.year());
                                      ASSERT(   1 == d1.month());
                                      ASSERT(   1 == d1.day());
//..
// Next, set 'd1' to July 4, 1776:
//..
    d1.setYearMonthDay(1776, 7, 4);   ASSERT(1776 == d1.year());
                                      ASSERT(   7 == d1.month());
                                      ASSERT(   4 == d1.day());
//..
// We can also use 'setYearMonthDayIfValid' to set the date if we are not sure
// whether the particular day we want to set to is valid.  For example, is year
// 1900 a leap year or not:
//..
    int ret = d1.setYearMonthDayIfValid(1900, 2, 29);
                                      ASSERT(   0 != ret);         // not leap
                                                                   // year

                                      ASSERT(1776 == d1.year());   // no effect
                                      ASSERT(   7 == d1.month());  // on the
                                      ASSERT(   4 == d1.day());    // object
//..
// From the date object, we can tell the day of year and day of week:
//..
    int dayOfYear = d1.dayOfYear();   ASSERT( 186 == dayOfYear);

    bdet_DayOfWeek::Day dayOfWeek = d1.dayOfWeek();
                                      ASSERT(bdet_DayOfWeek::BDET_THU
                                                                 == dayOfWeek);
//..
// Finally, we can also create a date object 'd2' using the year and day of
// year:
//..
    bdet_Date d2(1776, dayOfYear);    ASSERT(1776 == d2.year());
                                      ASSERT(   7 == d2.month());
                                      ASSERT(   4 == d2.day());
//..
// Now, add six days to the value of 'd2'.
//..
    d2 += 6;                          ASSERT(1776 == d2.year());
                                      ASSERT(   7 == d2.month());
                                      ASSERT(  10 == d2.day());
//..
// Next subtract 'd1' from 'd2', storing the difference (in days) in
// 'daysDiff':
//..
    int daysDiff = d2 - d1;           ASSERT(   6 == daysDiff);
//..
// Finally, stream the value of 'd2' to 'stdout'.
//..
if (verbose)
    bsl::cout << d2 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  10JUL1776
//..

      } break;
      case 20: {
        // --------------------------------------------------------------------
        // 'addDaysIfValid' TEST
        //
        // Concerns:
        //   1) Functionally equivalent to 'operator+=' when the resulting date
        //      is a valid.
        //   2) Returns 0 on success, and a non-zero value otherwise with no
        //      change to the original date.
        //
        // Plan:
        //   Using the table-driven approach, create a table with valid start
        //   dates, the number of days to add, the expected resulting date, and
        //   the expected return value.  Add the number of days to the start
        //   date, and verify that the resulting date and return value is as
        //   expected.
        //
        // Testing:
        //   int addDaysIfValid(int numDays);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'addDaysIfValid' TEST" << endl
                                  << "=====================" << endl;

        static const struct {
            int d_lineNum;   // source line number
            int d_syear;     // start year
            int d_smonth;    // start month
            int d_sday;      // start day
            int d_numDays;   // number of days
            int d_expYear;   // expected year
            int d_expMonth;  // expected month
            int d_expDay;    // expected day
            int d_retCode;   // expected return value
        } DATA[] = {
        //LINE   SYEAR  SMONTH  SDAY  NUMDAYS  EXPYEAR  EXPMONTH  EXPDAY  EXPRC
        //----   -----  ------  ----  -------  -------  --------  ------  -----
        // Valid results
        { L_,       1,     1,     1,      1,        1,       1,       2,   0 },
        { L_,      10,     2,    28,      1,       10,       3,       1,   0 },
        { L_,     100,     3,    31,      2,      100,       4,       2,   0 },
        { L_,    1000,     4,    30,      4,     1000,       5,       4,   0 },
        { L_,    1000,     6,     1,    -31,     1000,       5,       1,   0 },
        { L_,    1001,     1,     1,   -366,     1000,       1,       1,   0 },
        { L_,    1100,     5,    31,     30,     1100,       6,      30,   0 },
        { L_,    1200,     6,    30,     32,     1200,       8,       1,   0 },
        { L_,    1996,     2,    28,    367,     1997,       3,       1,   0 },
        { L_,    1997,     2,    28,    366,     1998,       3,       1,   0 },
        { L_,    1998,     2,    28,    365,     1999,       2,      28,   0 },
        { L_,    1999,     2,    28,    364,     2000,       2,      27,   0 },
        { L_,    1999,     2,    28,   1096,     2002,       2,      28,   0 },
        { L_,    2002,     2,    28,  -1096,     1999,       2,      28,   0 },
        { L_,    9999,    12,    31,      0,     9999,      12,      31,   0 },

        // Invalid results
        { L_,    9999,    12,    31,      1,     9999,      12,      31,  -1 },
        { L_,    9999,    12,    31,      2,     9999,      12,      31,  -1 },
        { L_,    9999,    12,    30,      2,     9999,      12,      30,  -1 },
        { L_,    9999,    11,    30,     32,     9999,      11,      30,  -1 },
        { L_,    9999,     1,     1,    365,     9999,       1,       1,  -1 },
        { L_,    9998,    12,    31,    366,     9998,      12,      31,  -1 },
        { L_,       1,     1,     1,     -1,        1,       1,       1,  -1 },
        { L_,       1,     1,     2,     -2,        1,       1,       2,  -1 },
        { L_,       1,     1,    31,    -31,        1,       1,      31,  -1 },
        { L_,       1,     2,     1,    -32,        1,       2,       1,  -1 },
        { L_,       1,    12,    31,   -365,        1,      12,      31,  -1 },
        { L_,       2,     1,     1,   -366,        2,       1,       1,  -1 },

        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE     = DATA[ti].d_lineNum;
            const int SYEAR    = DATA[ti].d_syear;
            const int SMONTH   = DATA[ti].d_smonth;
            const int SDAY     = DATA[ti].d_sday;
            const int NUMDAYS  = DATA[ti].d_numDays;
            const int EXPYEAR  = DATA[ti].d_expYear;
            const int EXPMONTH = DATA[ti].d_expMonth;
            const int EXPDAY   = DATA[ti].d_expDay;
            const int EXPRC    = DATA[ti].d_retCode;

            if (veryVerbose) {
                T_ P_(LINE)    P_(SYEAR)    P_(SMONTH) P_(SDAY) P(NUMDAYS)
                T_ P_(EXPYEAR) P_(EXPMONTH) P_(EXPDAY) P(EXPRC)
            }

            Obj mX(SYEAR, SMONTH, SDAY);    const Obj& X = mX;
            const Obj EXP(EXPYEAR, EXPMONTH, EXPDAY);

            const int RC = mX.addDaysIfValid(NUMDAYS);

            LOOP3_ASSERT(LINE, EXP,    X, EXP   == X);
            LOOP3_ASSERT(LINE, EXPRC, RC, EXPRC == RC);
        }

      } break;
      case 19: {
        // --------------------------------------------------------------------
        // 'getYearDay' TEST
        //
        // Concerns:
        //   Each individual date field must be correctly forwarded to the
        //   fully-tested underlying utility function.  'year' and 'dayOfYear'
        //   must be populated with the correct values.
        //
        // Plan:
        //   Construct a table of valid 'year' and 'dayOfYear' values, and
        //   create a 'bdet_Date' with them.  Then, invoke 'getYearDay' and
        //   verify the values returned are as expected.
        //
        // Testing:
        //   void getYearDay(int *year, int *dayOfYear) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'getYearDay' TEST" << endl
                                  << "=================" << endl;

        if (verbose) cout << "\nTesting: 'getYearDay'." << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year under test
                int d_day;      // day-of-year under test
            } DATA[] = {
                //LINE      YEAR      DAY
                //-------   -----  -------
                { L_,          1,       1  },
                { L_,          1,       2  },
                { L_,          1,      32  },
                { L_,          1,     365  },

                { L_,       1996,       1  },
                { L_,       1996,       2  },
                { L_,       1996,      32  },
                { L_,       1996,     365  },
                { L_,       1996,     366  },

                { L_,       9999,       1  },
                { L_,       9999,       2  },
                { L_,       9999,      32  },
                { L_,       9999,     365  },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE      = DATA[ti].d_lineNum;
                const int YEAR      = DATA[ti].d_year;
                const int DAY       = DATA[ti].d_day;
                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR) P(DAY)
                }

                const Obj X(YEAR, DAY);

                int year      = 0;
                int dayOfYear = 0;

                X.getYearDay(&year, &dayOfYear);

                LOOP2_ASSERT(YEAR, year,     YEAR == year);
                LOOP2_ASSERT(DAY,  dayOfYear, DAY == dayOfYear);
            }
        }
      } break;
      case 18: {
        // --------------------------------------------------------------------
        // 'setYearDay' TEST
        //
        // Concerns:
        //   Each of these methods is implemented using tested functionality
        //   from 'bdeimp_dateutil'.  Therefore, only a few test vectors are
        //   needed.  For 'dayOfYear', the concern is simply that the right
        //   utility method is used.
        //
        //   For 'setYearDay', we are concerned that the arguments are passed
        //   in the correct order (to the correct function).
        //
        //   For 'setYearDayIfValid', each of the two integer fields must
        //   separately be able to cause a return of an "invalid" status,
        //   independent of other input.  If the input is not invalid, the
        //   value must be correctly set.
        //
        // Plan:
        //   For 'setYearDay', construct a table of valid inputs and compare
        //   results with the expected values.
        //
        //   For 'dayOfYear', use the 'set'-method table of valid inputs to
        //   confirm the expected results.
        //
        //   For 'setYearDayIfValid', construct a table of valid and
        //   invalid inputs and use the now-tested 'setYearDay' to confirm the
        //   correct results for valid values.
        //
        // Testing:
        //   int dayOfYear();
        //   int setYearDay(int year, int dayOfYear);
        //   int setYearDayIfValid(int year, int dayOfYear);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'setYearDay' TEST" << endl
                                  << "=================" << endl;

        if (verbose)
            cout << "\nTesting: 'setYearDay' and 'dayOfYear'." << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year under test
                int d_day;      // day-of-year under test
                int d_expMonth; // expected month
                int d_expDay;   // expected day
            } DATA[] = {
                //LINE      YEAR      DAY      EXP MONTH    EXP DAY
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

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE      = DATA[ti].d_lineNum;
                const int YEAR      = DATA[ti].d_year;
                const int DAY       = DATA[ti].d_day;
                const int EXP_MONTH = DATA[ti].d_expMonth;
                const int EXP_DAY   = DATA[ti].d_expDay;
                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR) P_(DAY)
                       P_(EXP_MONTH) P(EXP_DAY)
                }

                const Obj  R(YEAR, EXP_MONTH, EXP_DAY);
                Obj        x;
                const Obj& X = x;

                x.setYearDay(YEAR, DAY);

                LOOP_ASSERT(LINE, R   == X);
                LOOP_ASSERT(LINE, DAY == X.dayOfYear());
            }
        }

        if (verbose)
            cout << "\nTesting: 'setYearDayIfValid'" << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year under test
                int d_day;      // day-of-year under test
                int d_exp;      // expected status
            } DATA[] = {
                //LINE      YEAR      DAY       EXP
                //-------   -----  -------     ------
                { L_,          1,       1,       1    },

                { L_,          1,      -1,       0    },
                { L_,          1,       0,       0    },
                { L_,          1,     365,       1    },
                { L_,          1,     366,       0    },
                { L_,          1,     367,       0    },

                { L_,          0,       1,       0    },
                { L_,       9999,       1,       1    },
                { L_,      10000,       1,       0    },

                { L_,          0,       0,       0    },
                { L_,         -1,      -1,       0    },

                { L_,       1996,       1,       1    },
                { L_,       1996,       2,       1    },
                { L_,       1996,      32,       1    },
                { L_,       1996,     365,       1    },
                { L_,       1996,     366,       1    },
                { L_,       1996,     367,       0    },

                { L_,       9999,       1,       1    },
                { L_,       9999,       2,       1    },
                { L_,       9999,      32,       1    },
                { L_,       9999,     365,       1    },
                { L_,       9999,     366,       0    },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int YEAR  = DATA[ti].d_year;
                const int DAY   = DATA[ti].d_day;
                const int EXP   = DATA[ti].d_exp;
                if (veryVerbose) { T_ P_(LINE) P_(EXP) P_(YEAR) P(DAY) }

                Obj x;  const Obj& X = x;
                if (1 == EXP) {
                    Obj r;  const Obj& R = r;
                    r.setYearDay(YEAR, DAY);

                    LOOP_ASSERT(LINE, 0 == x.setYearDayIfValid(YEAR, DAY));
                    LOOP_ASSERT(LINE, R == X);

                    if (veryVeryVerbose) { T_ T_ P_(EXP) P_(R) P(X) }
                }
                else {
                    const Obj R;
                    LOOP_ASSERT(LINE, -1 == x.setYearDayIfValid(YEAR, DAY));
                    LOOP_ASSERT(LINE, R == X);

                    if (veryVeryVerbose) { T_ T_ P_(EXP) P_(R) P(X) }
                }
            }
        }

      } break;
      case 17: {
        // --------------------------------------------------------------------
        // 'setYearMonthDayIfValid' TEST
        //
        // Concerns:
        //   Each of the three integer fields must separately be able to
        //   cause a return of an "invalid" status, independent of other input.
        //   If the input is not invalid, the value must be correctly set.
        //   The underlying functionality is already tested, so only a few test
        //   vectors are needed.
        //
        // Plan:
        //   Construct a table of valid and invalid inputs and compare results
        //   to expected "valid" values.
        //
        // Testing:
        //   int setYearMonthDayIfValid(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'setYearMonthDayIfValid' TEST" << endl
                          << "=============================" << endl;

        if (verbose)
            cout << "\nTesting: 'setYearMonthDayIfValid'." << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year under test
                int d_month;    // month under test
                int d_day;      // day under test
                int d_exp;      // expected value
            } DATA[] = {
                //LINE      YEAR   MONTH   DAY     EXP
                //-------   -----  -----  -----    -----
                { L_,          0,     0,     0,      0 },
                { L_,          1,     1,     0,      0 },
                { L_,          1,     0,     1,      0 },
                { L_,          0,     1,     1,      0 },
                { L_,          1,     1,    -1,      0 },
                { L_,          1,    -1,     1,      0 },
                { L_,         -1,     1,     1,      0 },

                { L_,          0,    12,    31,      0 },
                { L_,          1,     1,     1,      1 },
                { L_,       2010,     1,     2,      1 },
                { L_,       2011,     2,     5,      1 },
                { L_,       2012,     3,    10,      1 },
                { L_,       2013,     4,    17,      1 },
                { L_,       2014,     5,    23,      1 },
                { L_,       9999,    12,    31,      1 },
                { L_,      10000,     1,     1,      0 },

                { L_,       1600,     2,    29,      1 },
                { L_,       1700,     2,    29,      1 },
                { L_,       1800,     2,    29,      0 },
                { L_,       1900,     2,    29,      0 },
                { L_,       2000,     2,    29,      1 },
                { L_,       2100,     2,    29,      0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int YEAR  = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int DAY   = DATA[ti].d_day;
                const int EXP   = DATA[ti].d_exp;
                if (veryVerbose) {
                    T_ P_(LINE) P_(EXP) P_(YEAR)
                       P_(MONTH) P(DAY)
                }

                Obj x;  const Obj& X = x;
                if (1 == EXP) {
                    const Obj R(YEAR, MONTH, DAY);
                    LOOP_ASSERT(LINE,
                                0 == x.setYearMonthDayIfValid(YEAR,
                                                              MONTH,
                                                              DAY));
                    LOOP_ASSERT(LINE, R == X);

                    if (veryVeryVerbose) { T_ T_ P_(EXP) P_(R) P(X) }
                }
                else {
                    const Obj R;
                    LOOP_ASSERT(LINE,
                                -1 == x.setYearMonthDayIfValid(YEAR,
                                                               MONTH,
                                                               DAY));
                    LOOP_ASSERT(LINE, R == X);

                    if (veryVeryVerbose) { T_ T_ P_(EXP) P_(R) P(X) }
                }

                LOOP_ASSERT(LINE, EXP == bdet_Date::isValid(YEAR, MONTH, DAY));
            }
        }

      } break;
      case 16: {
        // --------------------------------------------------------------------
        // 'dayOfWeek' TESTING
        //
        // Concerns:
        //   The method must forward its underlying value to the correctly
        //   composed pair of tested functions.
        //
        // Plan:
        //   Specify a set S of dates.  For each d in S construct a date x
        //   having the value d and verify that x.dayOfWeek() returns the
        //   expected 'bdet_DayOfWeek::Day' value.
        //
        // Testing:
        //   bdet_DayOfWeek::Day dayOfWeek() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Day-of-Week Functionality" << endl
                          << "=================================" << endl;

        if (verbose) cout << "\nTesting: 'dayOfWeek()'." << endl;
        {
            typedef bdet_DayOfWeek DOW;

            static const struct {
                int d_lineNum;      // source line number
                int d_year;         // year under test
                int d_month;        // month under test
                int d_day;          // day under test
                DOW::Day d_expDay;  // number of days to be added
            } DATA[] = {
                //LINE      YEAR   MONTH   DAY     EXP
                //-------   -----  -----  -----   -------
                { L_,       1600,   1,       1,   DOW::BDET_TUESDAY   },
                { L_,       1600,   1,       2,   DOW::BDET_WEDNESDAY },
                { L_,       1600,   1,       3,   DOW::BDET_THURSDAY  },
                { L_,       1600,   1,       4,   DOW::BDET_FRIDAY    },
                { L_,       1600,   1,       5,   DOW::BDET_SATURDAY  },
                { L_,       1600,   1,       6,   DOW::BDET_SUNDAY    },
                { L_,       1600,   1,       7,   DOW::BDET_MONDAY    },
                { L_,       1600,   1,       8,   DOW::BDET_TUESDAY   },

                { L_,       1752,   9,       1,   DOW::BDET_TUESDAY   },
                { L_,       1752,   9,       2,   DOW::BDET_WEDNESDAY },
                { L_,       1752,   9,      14,   DOW::BDET_THURSDAY  },
                { L_,       1752,   9,      15,   DOW::BDET_FRIDAY    },
                { L_,       1752,   9,      16,   DOW::BDET_SATURDAY  },
                { L_,       1752,   9,      17,   DOW::BDET_SUNDAY    },
                { L_,       1752,   9,      18,   DOW::BDET_MONDAY    },
                { L_,       1752,   9,      19,   DOW::BDET_TUESDAY   },

                { L_,       1999,  12,      28,   DOW::BDET_TUESDAY   },
                { L_,       1999,  12,      29,   DOW::BDET_WEDNESDAY },
                { L_,       1999,  12,      30,   DOW::BDET_THURSDAY  },
                { L_,       1999,  12,      31,   DOW::BDET_FRIDAY    },
                { L_,       2000,   1,       1,   DOW::BDET_SATURDAY  },
                { L_,       2000,   1,       2,   DOW::BDET_SUNDAY    },
                { L_,       2000,   1,       3,   DOW::BDET_MONDAY    },
                { L_,       2000,   1,       4,   DOW::BDET_TUESDAY   },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int YEAR  = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int DAY   = DATA[ti].d_day;
                const int EXP   = DATA[ti].d_expDay;
                if (veryVerbose) { T_ P_(LINE) P_(YEAR) P_(MONTH)
                                      P_(DAY)  P(EXP) }

                Obj x(YEAR, MONTH, DAY);
                const Obj& X = x;

                if (veryVeryVerbose) { T_ T_ P_(X) P(X.dayOfWeek()) }

                LOOP_ASSERT(LINE, EXP == X.dayOfWeek());
            }
        }

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // RELATIONAL OPERATORS (<, <=, >=, >) TEST
        //
        // Concerns:
        //   Each operator must invoke the corresponding operator on the
        //   underlying integer serial date correctly.
        //
        // Plan:
        //   Specify an ordered set S of unique object values.  For each (u, v)
        //   in the set S x S, verify the result of u OP v for each OP in
        //   {<, <=, >=, >}.
        //
        // Testing:
        //   bool operator< (const bdet_Date& lhs, const bdet_Date& rhs);
        //   bool operator<=(const bdet_Date& lhs, const bdet_Date& rhs);
        //   bool operator>=(const bdet_Date& lhs, const bdet_Date& rhs);
        //   bool operator> (const bdet_Date& lhs, const bdet_Date& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                     << "RELATIONAL OPERATORS (<, <=, >=, >) TEST" << endl
                     << "========================================" << endl;

        if (verbose) cout <<
            "\nTesting: 'operator<', 'operator<=', 'operator>=', 'operator>'"
                          << endl;
        {
            static const struct {
                int d_year;
                int d_month;
                int d_day;
            } DATA[] = {
                {    1,  1,  1 }, {   10,  4,  5 }, {  100,  6,  7 },
                { 1000,  8,  9 }, { 2000,  1, 31 }, { 2002,  7,  4 },
                { 2002, 12, 31 }, { 2003,  1,  1 }, { 2003,  1,  2 },
                { 2003,  8,  5 }, { 2003,  8,  6 }, { 2003,  8,  7 },
                { 2004,  9,  3 }, { 2004,  9,  4 }, { 9999, 12, 31 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int V_YEAR  = DATA[i].d_year;
                const int V_MONTH = DATA[i].d_month;
                const int V_DAY   = DATA[i].d_day;
                if (veryVerbose) { T_ P_(V_YEAR) P_(V_MONTH) P(V_DAY) }

                Obj v;  const Obj& V = v;
                v.setYearMonthDay(V_YEAR, V_MONTH, V_DAY);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int U_YEAR  = DATA[j].d_year;
                    const int U_MONTH = DATA[j].d_month;
                    const int U_DAY   = DATA[j].d_day;
                    if (veryVerbose) { T_ T_ P_(U_YEAR) P_(U_MONTH) P(U_DAY) }

                    Obj u;  const Obj& U = u;
                    u.setYearMonthDay(U_YEAR, U_MONTH, U_DAY);

                    if (veryVeryVerbose) { T_ T_ T_ P_(i) P_(j) P_(V) P(U) }

                    LOOP2_ASSERT(i, j, j <  i == U <  V);
                    LOOP2_ASSERT(i, j, j <= i == U <= V);
                    LOOP2_ASSERT(i, j, j >= i == U >= V);
                    LOOP2_ASSERT(i, j, j >  i == U >  V);
                }
            }
        }

      } break;
      case 14: {
        // --------------------------------------------------------------------
        // ARITHMETIC OPERATORS TEST
        //
        // Concerns:
        //   Each function must return the expected value (consistent with its
        //   nominal primitive-operator counterpart).  Assignment operators
        //   must also modify the lhs operand as expected.
        //
        // Plan:
        //   Specify a set S of {pairs of dates (d1, d2) and their difference
        //   in days D}.  For each of the operators under test, in a loop over
        //   the elements of S, construct a date x having the appropriate value
        //   from (d1, d2), apply the operator (possibly with D as an operand),
        //   and confirm the results using an additional date y as needed and
        //   the values (d1, d2, D).  Note that, depending on the specific
        //   operator, the values d1 and d2 will serve variously as initial
        //   values or as results, and the integer D will serve as an operand
        //   as a result.  Note also that, for convenience, the two overloaded
        //   'operator+' functions are tested in the same loop, using dates y
        //   and z to accumulate the results.
        //
        // Testing:
        //   bdet_Date& operator+=(int numDays);
        //   bdet_Date& operator-=(int numDays);
        //   bdet_Date& operator+(const bdet_Date& lhs, int numDays);
        //   bdet_Date& operator+(int numDays, const bdet_Date& rhs);
        //   bdet_Date& operator-(const bdet_Date& lhs, int numDays);
        //   int operator-(const bdet_Date& lhs, const bdet_Date& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "ARITHMETIC OPERATORS TEST" << endl
                                  << "=========================" << endl;

        if (verbose) cout << "Testing arithmetic operators." << endl;
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
            } DATA[] = {
                //          - - - -first- - - -           - - - second - - -
                //LINE      YEAR1  MONTH1  DAY1  NUM DAYS  YEAR2  MONTH2  DAY2
                //-------   -----  ------  ----  --------  -----  ------  ----
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
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose) cout <<
                    "\nTesting: 'bdet_Date& operator+=(int numDays)'." << endl;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE     = DATA[ti].d_lineNum;
                const int YEAR1    = DATA[ti].d_year1;
                const int MONTH1   = DATA[ti].d_month1;
                const int DAY1     = DATA[ti].d_day1;
                const int NUM_DAYS = DATA[ti].d_numDays;
                const int YEAR2    = DATA[ti].d_year2;
                const int MONTH2   = DATA[ti].d_month2;
                const int DAY2     = DATA[ti].d_day2;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR1) P_(MONTH1) P(DAY1)
                    T_ P_(NUM_DAYS) P_(YEAR2) P_(MONTH2) P(DAY2)
                }

                Obj x(YEAR1, MONTH1, DAY1);

                const Obj& X = x;

                if (veryVerbose) { T_ P_(X) P_(NUM_DAYS) }
                x += NUM_DAYS;

                if (veryVerbose) { cout << "\t\t --> "; P(X) }
                LOOP_ASSERT(LINE, YEAR2  == X.year());
                LOOP_ASSERT(LINE, MONTH2 == X.month());
                LOOP_ASSERT(LINE, DAY2   == X.day());
            }

            if (verbose) cout <<
                    "\nTesting: 'bdet_Date& operator-=(int numDays)'." << endl;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE     = DATA[ti].d_lineNum;
                const int YEAR1    = DATA[ti].d_year1;
                const int MONTH1   = DATA[ti].d_month1;
                const int DAY1     = DATA[ti].d_day1;
                const int NUM_DAYS = DATA[ti].d_numDays;
                const int YEAR2    = DATA[ti].d_year2;
                const int MONTH2   = DATA[ti].d_month2;
                const int DAY2     = DATA[ti].d_day2;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR1) P_(MONTH1) P(DAY1)
                    T_ P_(NUM_DAYS) P_(YEAR2) P_(MONTH2) P(DAY2)
                }

                Obj x(YEAR2, MONTH2, DAY2);
                const Obj& X = x;

                if (veryVerbose) { T_ P_(X) P_(NUM_DAYS) }
                x -= NUM_DAYS;

                if (veryVerbose) { cout << "\t\t --> "; P(X) }
                LOOP_ASSERT(LINE, YEAR1  == X.year());
                LOOP_ASSERT(LINE, MONTH1 == X.month());
                LOOP_ASSERT(LINE, DAY1   == X.day());
            }

            if (verbose) cout <<
                "\nTesting: 'bdet_Date& operator+(const bdet_Date& date, int)'"
                "\n         'bdet_Date& operator+(int, const bdet_Date& date)'"
                << endl;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE     = DATA[ti].d_lineNum;
                const int YEAR1    = DATA[ti].d_year1;
                const int MONTH1   = DATA[ti].d_month1;
                const int DAY1     = DATA[ti].d_day1;
                const int NUM_DAYS = DATA[ti].d_numDays;
                const int YEAR2    = DATA[ti].d_year2;
                const int MONTH2   = DATA[ti].d_month2;
                const int DAY2     = DATA[ti].d_day2;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR1) P_(MONTH1) P(DAY1)
                    T_ P_(NUM_DAYS) P_(YEAR2) P_(MONTH2) P(DAY2)
                }

                Obj x(YEAR1, MONTH1, DAY1);

                const Obj& X = x;
                if (veryVerbose) { T_ P_(X) P_(NUM_DAYS); }

                const Obj Y = X + NUM_DAYS;
                const Obj Z = NUM_DAYS + X;
                const Obj EXP(YEAR2, MONTH2, DAY2);

                if (veryVerbose) { cout << "\t\t --> "; P_(Y) P(Z) }
                LOOP_ASSERT(LINE, EXP == Y);   LOOP_ASSERT(LINE, Y == Z);
            }

            if (verbose) cout << "\nTesting: "
                "'bdet_Date& operator-(const bdet_Date& date, int)'" << endl;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE     = DATA[ti].d_lineNum;
                const int YEAR1    = DATA[ti].d_year1;
                const int MONTH1   = DATA[ti].d_month1;
                const int DAY1     = DATA[ti].d_day1;
                const int NUM_DAYS = DATA[ti].d_numDays;
                const int YEAR2    = DATA[ti].d_year2;
                const int MONTH2   = DATA[ti].d_month2;
                const int DAY2     = DATA[ti].d_day2;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR1) P_(MONTH1) P(DAY1)
                    T_ P_(NUM_DAYS) P_(YEAR2) P_(MONTH2) P(DAY2)
                }

                Obj x(YEAR2, MONTH2, DAY2);

                const Obj& X = x;
                if (veryVerbose) { T_ P_(X) P_(NUM_DAYS) }

                const Obj Y = x - NUM_DAYS;
                const Obj EXP(YEAR1, MONTH1, DAY1);

                if (veryVerbose) { cout << "\t\t --> "; P(Y) }
                LOOP_ASSERT(LINE, EXP == Y);
            }

            if (verbose) cout <<  "\nTesting: "
                "'int operator-(const bdet_Date& date, const bdet_Date&)'"
                << endl;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE     = DATA[ti].d_lineNum;
                const int YEAR1    = DATA[ti].d_year1;
                const int MONTH1   = DATA[ti].d_month1;
                const int DAY1     = DATA[ti].d_day1;
                const int NUM_DAYS = DATA[ti].d_numDays;
                const int YEAR2    = DATA[ti].d_year2;
                const int MONTH2   = DATA[ti].d_month2;
                const int DAY2     = DATA[ti].d_day2;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR1) P_(MONTH1) P(DAY1)
                    P_(NUM_DAYS) P_(YEAR2) P_(MONTH2) P(DAY2)
                }

                Obj x(YEAR1, MONTH1, DAY1);
                const Obj& X = x;

                Obj y(YEAR2, MONTH2, DAY2);
                const Obj& Y = y;

                if (veryVerbose) { T_ P_(X) P_(Y) }
                const int RESULT = Y - X;

                if (veryVerbose) { cout << "\t\t --> "; P(RESULT) }
                LOOP_ASSERT(LINE, DATA[ti].d_numDays == RESULT);
            }
        }

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // INCREMENT AND DECREMENT OPERATORS TEST
        //
        // Concerns:
        //   Each operator must modify its argument (the operand) as expected
        //   and also return the expected value, consistent with its nominal
        //   primitive-operator counterpart.
        //
        // Plan:
        //   Specify a set S of pairs of dates (d1, d2) differing by one day.
        //   For each of the operators under test, in a loop over the elements
        //   of S, construct a date x having an appropriate value from (d1, d2)
        //   apply the operator, assign the result to an independent date y,
        //   and verify that x and y have the expected values from (d1, d2).
        //   Note that, depending on the specific operator, d1 or d2 may serve
        //   as the initial or final value for x, and as the result for y.
        //
        // Testing:
        //   bdet_Date& operator++();    // prefix
        //   bdet_Date& operator--();    // prefix
        //   bdet_Date operator++(int);  // postfix
        //   bdet_Date operator--(int);  // postfix
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "INCREMENT AND DECREMENT OPERATORS TEST" << endl
                          << "======================================" << endl;

        {
            static const struct {
                int d_lineNum;   // source line number
                int d_year1;     // (first) date year
                int d_month1;    // (first) date month
                int d_day1;      // (first) date day
                int d_year2;     // (second) date year
                int d_month2;    // (second) date month
                int d_day2;      // (second) date day
            } DATA[] = {
                //          - - - -first- - - -    - - - second - - -
                //LINE      YEAR1  MONTH1  DAY1    YEAR2  MONTH2  DAY2
                //-------   -----  ------  -----   -----  ------  -----
                { L_,          1,     1,     1,       1,     1,     2 },
                { L_,         10,     2,    28,      10,     3,     1 },
                { L_,        100,     3,    31,     100,     4,     1 },
                { L_,       1000,     4,    30,    1000,     5,     1 },
                { L_,       1100,     5,    31,    1100,     6,     1 },
                { L_,       1200,     6,    30,    1200,     7,     1 },
                { L_,       1300,     7,    31,    1300,     8,     1 },
                { L_,       1400,     8,    31,    1400,     9,     1 },
                { L_,       1500,     9,    30,    1500,    10,     1 },
                { L_,       1600,    10,    31,    1600,    11,     1 },
                { L_,       1700,    11,    30,    1700,    12,     1 },
                { L_,       1800,    12,    31,    1801,     1,     1 },
                { L_,       1996,     2,    28,    1996,     2,    29 },
                { L_,       1997,     2,    28,    1997,     3,     1 },
                { L_,       1998,     2,    28,    1998,     3,     1 },
                { L_,       1999,     2,    28,    1999,     3,     1 },
                { L_,       2000,     2,    28,    2000,     2,    29 },
                { L_,       2001,     2,    28,    2001,     3,     1 },
                { L_,       2004,     2,    28,    2004,     2,    29 },
                { L_,       2100,     2,    28,    2100,     3,     1 },
                { L_,       2400,     2,    28,    2400,     2,    29 },

            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            if (verbose) cout << "\nTesting: 'operator++()'" << endl;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE   = DATA[ti].d_lineNum;
                const int YEAR1  = DATA[ti].d_year1;
                const int MONTH1 = DATA[ti].d_month1;
                const int DAY1   = DATA[ti].d_day1;
                const int YEAR2  = DATA[ti].d_year2;
                const int MONTH2 = DATA[ti].d_month2;
                const int DAY2   = DATA[ti].d_day2;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR1) P_(MONTH1) P(DAY1)
                    T_ P_(YEAR2) P_(MONTH2) P(DAY2)
                }

                Obj x(YEAR1, MONTH1, DAY1);
                const Obj& X = x;
                Obj y = ++x;  const Obj& Y = y;

                if (veryVerbose) { T_ T_ P_(X) P(Y) }
                LOOP_ASSERT(LINE, YEAR2  == X.year());
                LOOP_ASSERT(LINE, MONTH2 == X.month());
                LOOP_ASSERT(LINE, DAY2   == X.day());

                LOOP_ASSERT(LINE, YEAR2  == Y.year());
                LOOP_ASSERT(LINE, MONTH2 == Y.month());
                LOOP_ASSERT(LINE, DAY2   == Y.day());
            }

            if (verbose) cout << "\nTesting: 'operator++(int)'" << endl;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE   = DATA[ti].d_lineNum;
                const int YEAR1  = DATA[ti].d_year1;
                const int MONTH1 = DATA[ti].d_month1;
                const int DAY1   = DATA[ti].d_day1;
                const int YEAR2  = DATA[ti].d_year2;
                const int MONTH2 = DATA[ti].d_month2;
                const int DAY2   = DATA[ti].d_day2;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR1) P_(MONTH1) P(DAY1)
                    T_ P_(YEAR2) P_(MONTH2) P(DAY2)
                }

                Obj x(YEAR1, MONTH1, DAY1);
                const Obj& X = x;
                Obj y = x++;  const Obj& Y = y;

                if (veryVerbose) { T_ T_ P_(X) P(Y) }
                LOOP_ASSERT(LINE, YEAR2  == X.year());
                LOOP_ASSERT(LINE, MONTH2 == X.month());
                LOOP_ASSERT(LINE, DAY2   == X.day());

                LOOP_ASSERT(LINE, YEAR1  == Y.year());
                LOOP_ASSERT(LINE, MONTH1 == Y.month());
                LOOP_ASSERT(LINE, DAY1   == Y.day());
            }

            if (verbose) cout << "\nTesting: 'operator--()'" << endl;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE   = DATA[ti].d_lineNum;
                const int YEAR1  = DATA[ti].d_year1;
                const int MONTH1 = DATA[ti].d_month1;
                const int DAY1   = DATA[ti].d_day1;
                const int YEAR2  = DATA[ti].d_year2;
                const int MONTH2 = DATA[ti].d_month2;
                const int DAY2   = DATA[ti].d_day2;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR1) P_(MONTH1) P(DAY1)
                    T_ P_(YEAR2) P_(MONTH2) P(DAY2)
                }

                Obj x(YEAR2, MONTH2, DAY2);
                const Obj& X = x;
                Obj y = --x;  Obj Y = y;

                if (veryVerbose) { T_ T_ P_(X) P(Y) }
                LOOP_ASSERT(LINE, YEAR1  == X.year());
                LOOP_ASSERT(LINE, MONTH1 == X.month());
                LOOP_ASSERT(LINE, DAY1   == X.day());

                LOOP_ASSERT(LINE, YEAR1  == Y.year());
                LOOP_ASSERT(LINE, MONTH1 == Y.month());
                LOOP_ASSERT(LINE, DAY1   == Y.day());
            }

            if (verbose) cout << "\nTesting: 'operator--(int)'" << endl;

            for (int ti = 0; ti < NUM_DATA ; ++ti) {
                const int LINE   = DATA[ti].d_lineNum;
                const int YEAR1  = DATA[ti].d_year1;
                const int MONTH1 = DATA[ti].d_month1;
                const int DAY1   = DATA[ti].d_day1;
                const int YEAR2  = DATA[ti].d_year2;
                const int MONTH2 = DATA[ti].d_month2;
                const int DAY2   = DATA[ti].d_day2;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR1) P_(MONTH1) P(DAY1)
                    T_ P_(YEAR2) P_(MONTH2) P(DAY2)
                }

                Obj x(YEAR2, MONTH2, DAY2);
                const Obj& X = x;
                Obj y = x--;  Obj Y = y;

                if (veryVerbose) { T_ T_ P_(X) P(Y) }
                LOOP_ASSERT(LINE, YEAR1  == X.year());
                LOOP_ASSERT(LINE, MONTH1 == X.month());
                LOOP_ASSERT(LINE, DAY1   == X.day());

                LOOP_ASSERT(LINE, YEAR2  == Y.year());
                LOOP_ASSERT(LINE, MONTH2 == Y.month());
                LOOP_ASSERT(LINE, DAY2   == Y.day());
            }
        }

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // STATE CONSTRUCTOR TEST
        //
        // Concerns:
        //   The constructor must correctly forward its three parameters to a
        //   tested utility function and correctly initialize the serial date
        //   integer with the returned result.
        //
        // Plan:
        //   Specify a set S of dates as (y, m, d) triplets having widely
        //   varying values.  For each (y, m, d) in S, construct a date object
        //   X and verify that X has the expected value.
        //
        // Testing:
        //   bdet_Date(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "STATE CONSTRUCTOR TEST" << endl
                                  << "======================" << endl;

        if (verbose)
            cout << "\nTesting: 'bdet_Date(year, month, day)'" << endl;
        {
            static const struct {
                int d_year;
                int d_month;
                int d_day;
            } DATA[] = {
                {    1,  1,  1 }, {   10,  4,  5 }, {  100,  6,  7 },
                { 1000,  8,  9 }, { 1100,  1, 31 }, { 1200,  2, 29 },
                { 1300,  3, 31 }, { 1400,  4, 30 }, { 1500,  5, 31 },
                { 1600,  6, 30 }, { 1700,  7, 31 }, { 1800,  8, 31 },
                { 1900,  9, 30 }, { 2000, 10, 31 }, { 2100, 11, 30 },
                { 2200, 12, 31 }, { 2400, 12, 31 }, { 9999, 12, 31 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA ; ++i) {
                const int YEAR  = DATA[i].d_year;
                const int MONTH = DATA[i].d_month;
                const int DAY   = DATA[i].d_day;

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P(DAY) }

                const Obj X(YEAR, MONTH, DAY);

                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, YEAR  == X.year());
                LOOP_ASSERT(i, MONTH == X.month());
                LOOP_ASSERT(i, DAY   == X.day());
            }
        }

      } break;
      case 11: {
        // --------------------------------------------------------------------
        // 'isValid' TEST
        //
        // Concerns:
        //   Since this function is implemented as a direct call to
        //   'bdeimp_DateUtil::isValidDate', we are concerned only that the
        //   arguments are correctly forwarded.
        //
        // Plan:
        //   This test case is an abbreviated copy of the ImpUtil test.  For a
        //   set of (year, month, day) values, verify that 'isValid' returns
        //   the expected value.
        //
        // Testing:
        //   static bool isValid(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'isValid' TEST" << endl
                                  << "==============" << endl;

        if (verbose) cout << "\nTesting: 'isValid'" << endl;
        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year under test
                int d_month;    // month under test
                int d_day;      // day under test
                int d_exp;      // expected value
            } DATA[] = {
                //LINE      YEAR   MONTH  DAY      EXP
                //-------   -----  -----  -----    ---
                { L_,          0,     0,     0,      0 },
                { L_,          1,     1,     0,      0 },
                { L_,          1,     0,     1,      0 },
                { L_,          0,     1,     1,      0 },
                { L_,          1,     1,    -1,      0 },
                { L_,          1,    -1,     1,      0 },
                { L_,         -1,     1,     1,      0 },

                { L_,          0,    12,    31,      0 },
                { L_,          1,     1,     1,      1 },
                { L_,       9999,    12,    31,      1 },
                { L_,      10000,     1,     1,      0 },

                { L_,       1600,     2,    29,      1 },
                { L_,       1700,     2,    29,      1 },
                { L_,       1800,     2,    29,      0 },
                { L_,       1900,     2,    29,      0 },
                { L_,       2000,     2,    29,      1 },
                { L_,       2100,     2,    29,      0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int LINE  = DATA[ti].d_lineNum;
                const int YEAR  = DATA[ti].d_year;
                const int MONTH = DATA[ti].d_month;
                const int DAY   = DATA[ti].d_day;
                const int EXP   = DATA[ti].d_exp;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P(EXP)
                }

                LOOP_ASSERT(LINE, EXP == bdet_Date::isValid(YEAR, MONTH, DAY));
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
        //   relying on the bdex functions which forward appropriate calls
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
        //   static int maxSupportedBdexVersion() const;
        //   STREAM& bdexStreamIn(STREAM& stream, int version);
        //   STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'bdex' STREAMING FUNCTIONALITY" << endl
                          << "======================================" << endl;

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        // scalar and array object values for various stream tests
        const Obj VA(   1,  1,  1);
        const Obj VB(1776,  7,  4);
        const Obj VC(1956,  4, 30);
        const Obj VD(1958,  4, 30);
        const Obj VE(2002,  3, 25);
        const Obj VF(9999, 12, 31);

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
            "\nTesting streamIn functionality via bdex functions." << endl;

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            Out out;
            const char *const  OD = out.data();
            const int         LOD = out.length();
            ASSERT(0 == LOD);

            for (int i = 0; i < NUM_VALUES; ++i) {
                if (veryVerbose) { T_ P(i) }

                In in(OD, LOD);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
                LOOP_ASSERT(i, in);           LOOP_ASSERT(i, in.isEmpty());

                // Ensure that reading from an empty or invalid input stream
                // leaves the stream invalid and the target object unchanged.

                const Obj X(VALUES[i]);  Obj t(X);  LOOP_ASSERT(i, X == t);
                const Obj ERR(1, 1, 2);
              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, ERR == t);
                bdex_InStreamFunctions::streamIn(in, t, VERSION);
                LOOP_ASSERT(i, !in);     LOOP_ASSERT(i, ERR == t);
              } END_BDEX_EXCEPTION_TEST
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {
            const Obj W1 = VA, X1 = VB, Y1 = VC;
            const Obj W2 = VB, X2 = VC, Y2 = VD;
            const Obj W3 = VC, X3 = VD, Y3 = VE;
            const Obj E(1, 1, 2);  // error value for invalidated stream

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

                In in(OD, i);  In &testInStream = in;
                in.setSuppressVersionCheck(1);
                const Obj ERR(1, 1, 2);

              BEGIN_BDEX_EXCEPTION_TEST { in.reset();
                LOOP_ASSERT(i, in); LOOP_ASSERT(i, !i == in.isEmpty());
                Obj t1(W1), t2(W2), t3(W3);

                if (i < LOD1) {
                    bdex_InStreamFunctions::streamIn(in, t1, VERSION);
                    LOOP_ASSERT(i, !in);
                                       if (0 == i) { LOOP_ASSERT(i,
                                                                 ERR == t1);}
                                       if (2 <= i) { LOOP_ASSERT(i, E  == t1);}
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
                                 if (LOD1 == i)    { LOOP_ASSERT(i,
                                                                 ERR == t2);}
                                 if (LOD1 + 1 < i) { LOOP_ASSERT(i, E  == t2);}
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
                                 if (LOD2 == i)    { LOOP_ASSERT(i,
                                                                 ERR == t3);}
                                 if (LOD2 + 1 < i) { LOOP_ASSERT(i, E  == t3);}
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

        const Obj W;                // default value            (serial date 1)
        const Obj X(1, 1, 2);       // original (control) value (serial date 2)
        const Obj Y(1, 1, 3);       // new (streamed-out) value (serial date 3)

        const int SERIAL_Y  = 3;    // internal rep.  of Obj Y

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            const char version = 1;

            Out out;
            out.putInt24(SERIAL_Y);            // Stream out "new" value.
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, VERSION);
            ASSERT(in);
                             ASSERT(W != t);  ASSERT(X != t);  ASSERT(Y == t);
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out;
            out.putInt24(SERIAL_Y);            // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }
        {
            const char version = 5 ; // too large (current Date version is 1)

            Out out;
            out.putInt24(SERIAL_Y);            // Stream out "new" value.

            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            in.setQuiet(!veryVerbose);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                             ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
        }

        if (verbose) cout << "\t\tGood 'int' but bad date." << endl;
        {
            const char version = 1;

            Out out;
            out.putInt24(0);             // Stream out invalid date value.
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                             ASSERT(Obj::isValid(t.year(), t.month(),t.day()));
                             ASSERT(Obj(1, 1, 3) == t);  // imp testing value
        }

        if (verbose) cout << "\t\tGood 'int' but bad date." << endl;
        {
            const char version  = 1;
            const int  MAX_DATE = bdeimp_DateUtil::ymd2serial(9999, 12, 31);

            Out out;
            out.putInt24(MAX_DATE + 1);       // Stream out invalid date value.
            const char *const OD  = out.data();
            const int         LOD = out.length();

            Obj t(X);        ASSERT(W != t);  ASSERT(X == t);  ASSERT(Y != t);
            In in(OD, LOD);  ASSERT(in);
            in.setSuppressVersionCheck(1);
            bdex_InStreamFunctions::streamIn(in, t, version);
            ASSERT(!in);
                             ASSERT(Obj::isValid(t.year(), t.month(),t.day()));
                             ASSERT(Obj(1, 1, 3) == t);  // imp testing value
        }

        if (verbose) cout << "\nWire format direct tests." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                int         d_year;     // specification year
                int         d_month;    // specification month
                int         d_day;      // specification day
                int         d_version;  // version to stream with
                int         d_length;   // expect output length
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LINE  YEAR  MONTH  DAY  VER  LEN  FMT
                //----  ----  -----  ---  ---  ---  ---------------------
                { L_,   1400,    10,   2,   0,   0, ""                    },
                { L_,   2002,     8,  27,   0,   0, ""                    },

                { L_,   1400,    10,   2,   1,   3, "\x07\xcd\x1c"        },
                { L_,   2002,     8,  27,   1,   3, "\x0b\x27\xd3"        },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int   LINE  = DATA[ti].d_lineNum;
                const int   YEAR  = DATA[ti].d_year;
                const int   MONTH = DATA[ti].d_month;
                const int   DAY   = DATA[ti].d_day;
                const int   VER   = DATA[ti].d_version;
                const int   LEN   = DATA[ti].d_length;
                const char *FMT   = DATA[ti].d_fmt_p;

                if (veryVerbose) { T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY)
                                      P_(VER) P_(LEN) P(FMT) }

                Obj mX(YEAR, MONTH, DAY);  const Obj& X = mX;
                bdex_ByteOutStream out;  X.bdexStreamOut(out, VER);

                LOOP_ASSERT(LINE, LEN == out.length());
                LOOP_ASSERT(LINE, 0 == memcmp(out.data(),
                                              FMT,
                                              LEN));

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
        //   bdet_Date& operator=(const bdet_Date& rhs);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING ASSIGNMENT OPERATOR" << endl
                                  << "===========================" << endl;

        if (verbose) cout << "\nTesting Assignment u = V." << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day;
            } DATA[] = {
                {    1,  1,  1 }, {   10,  4,  5 }, {  100,  6,  7 },
                { 1000,  8,  9 }, { 2000,  2, 29 }, { 2002,  7,  4 },
                { 2003,  8,  5 }, { 2004,  9,  3 }, { 9999, 12, 31 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int V_YEAR  = DATA[i].d_year;
                const int V_MONTH = DATA[i].d_month;
                const int V_DAY   = DATA[i].d_day;

                if (veryVerbose) { T_ P_(V_YEAR) P_(V_MONTH) P(V_DAY) }

                Obj v;  const Obj& V = v;
                v.setYearMonthDay(V_YEAR, V_MONTH, V_DAY);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int U_YEAR  = DATA[j].d_year;
                    const int U_MONTH = DATA[j].d_month;
                    const int U_DAY   = DATA[j].d_day;

                    if (veryVerbose) { T_ T_ P_(U_YEAR) P_(U_MONTH) P(U_DAY) }

                    Obj u;  const Obj& U = u;
                    u.setYearMonthDay(U_YEAR, U_MONTH, U_DAY);

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
                const int YEAR  = DATA[i].d_year;
                const int MONTH = DATA[i].d_month;
                const int DAY   = DATA[i].d_day;

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P(DAY) }

                Obj u;  const Obj& U = u;
                u.setYearMonthDay(YEAR, MONTH, DAY);

                Obj w(U);  const Obj &W = w;              // control

                if (veryVeryVerbose) { T_ T_ P_(U) P(W) }
                LOOP_ASSERT(i, W == U);

                u = u;

                if (veryVeryVerbose) { T_ T_ P_(U) P(W) }
                LOOP_ASSERT(i, W == U);
            }
        }

      } break;
      case 8: {
        // --------------------------------------------------------------------
        // 'print' TEST
        //
        // Concerns:
        //   That 'print' method should print to the specified 'stream' with
        //   the appropriate indentation and new line characters.
        //
        // Plan:
        //   Test that the 'print' method produces the expected results for
        //   various values of 'level' and 'spacesPerLevel'.
        //
        // Testing:
        //   bsl::ostream& print(bsl::ostream& stream, int lvl, int sp) const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'print' TEST" << endl
                                  << "============" << endl;

        if (verbose) cout << "\nTesting the 'print' method." << endl;
        {
#define NL "\n"
            static const struct {
                int         d_lineNum;  // source line number
                int         d_indent;   // indentation level
                int         d_spaces;   // spaces per indentation level
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LINE  IND    +/-  SPL     FMT                   // ADJUST
                //----  ----------  ------  --------------
                { L_,    0,         -1,     "01JAN0001"                      },

                { L_,    0,          0,     "01JAN0001"                  NL  },

                { L_,    0,          2,     "01JAN0001"                  NL  },

                { L_,    1,          1,     " 01JAN0001"                 NL  },

                { L_,    1,          2,     "  01JAN0001"                NL  },

                { L_,   -1,          2,     "01JAN0001"                  NL  },

                { L_,   -2,          1,     "01JAN0001"                  NL  },

                { L_,    2,          1,     "  01JAN0001"                NL  },

                { L_,    1,          3,     "   01JAN0001"               NL  },
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

            Obj mX;  const Obj& X = mX;  // 01JAN0001

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE = DATA[ti].d_lineNum;
                const int         IND  = DATA[ti].d_indent;
                const int         SPL  = DATA[ti].d_spaces;
                const char *const FMT  = DATA[ti].d_fmt_p;

                if (veryVerbose) { T_ P_(LINE) P_(IND) P_(SPL) P(FMT) }

                char buf1[SIZE], buf2[SIZE];
                memcpy(buf1, CTRL_BUF1, SIZE); // preset buf1 to Z1 values
                memcpy(buf2, CTRL_BUF2, SIZE); // preset buf2 to Z2 values

                if (veryVeryVerbose) cout << "\t\tEXPECTED FORMAT:" << endl
                                          << FMT <<endl;
                ostrstream out1(buf1, SIZE);  X.print(out1, IND, SPL) << ends;
                ostrstream out2(buf2, SIZE);  X.print(out2, IND, SPL) << ends;
                if (veryVeryVerbose) cout << "\t\tACTUAL FORMAT:" << endl
                                          << buf1 <<endl;

                const int SZ = strlen(FMT) + 1;
                const int REST = SIZE - SZ;
                LOOP_ASSERT(LINE, SZ < SIZE);  // check buffer is large enough
                LOOP_ASSERT(LINE, Z1 == buf1[SIZE - 1]);  // check for overrun
                LOOP_ASSERT(LINE, Z2 == buf2[SIZE - 1]);  // check for overrun
                LOOP_ASSERT(LINE, 0 == strcmp(buf1, FMT));
                LOOP_ASSERT(LINE, 0 == strcmp(buf2, FMT));
                LOOP_ASSERT(LINE, 0 == memcmp(buf1 + SZ, CTRL_BUF1 + SZ,REST));
                LOOP_ASSERT(LINE, 0 == memcmp(buf2 + SZ, CTRL_BUF2 + SZ,REST));
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
        //   bdet_Date(const bdet_Date& original);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "Testing Copy Constructor" << endl
                                  << "========================" << endl;

        if (verbose) cout << "\nTesting copy constructor." << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day;
            } DATA[] = {
                {    1,  1,  1 }, {   10,  4,  5 }, {  100,  6,  7 },
                { 1000,  8,  9 }, { 2000,  2, 29 }, { 2002,  7,  4 },
                { 2003,  8,  5 }, { 2004,  9,  3 }, { 9999, 12, 31 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR  = DATA[i].d_year;
                const int MONTH = DATA[i].d_month;
                const int DAY   = DATA[i].d_day;

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P(DAY) }

                Obj w;  const Obj& W = w;           // control
                w.setYearMonthDay(YEAR, MONTH, DAY);

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(YEAR, MONTH, DAY);

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
                int d_year;  int d_month;  int d_day;
            } DATA[] = {
                {    1,  1,  1 },
                {    1,  1,  2 }, {    1,  2,  1 }, {    2,  1,  1 },

                { 9998, 12, 31 },
                { 9999, 11, 30 }, { 9999, 12, 30 }, { 9999, 12, 31 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int U_YEAR  = DATA[i].d_year;
                const int U_MONTH = DATA[i].d_month;
                const int U_DAY   = DATA[i].d_day;

                if (veryVerbose) { T_ P_(U_YEAR) P_(U_MONTH) P(U_DAY) }

                Obj u;  const Obj& U = u;
                u.setYearMonthDay(U_YEAR, U_MONTH, U_DAY);

                for (int j = 0; j < NUM_DATA; ++j) {
                    const int V_YEAR  = DATA[j].d_year;
                    const int V_MONTH = DATA[j].d_month;
                    const int V_DAY   = DATA[j].d_day;

                    if (veryVerbose) { T_ T_ P_(V_YEAR) P_(V_MONTH) P(V_DAY) }

                    Obj v;  const Obj& V = v;
                    v.setYearMonthDay(V_YEAR, V_MONTH, V_DAY);

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
        // TESTING OUTPUT (<<) OPERATOR:
        //
        // Concerns:
        //   The "month" field is formatted using tabulated data; all 12 months
        //   must be verified.  Other fields use tested accessors, so only
        //   their correct forwarding requires verification.
        //
        // Plan:
        //   For each of a small representative set of object values, use
        //   'ostrstream' to write that object's value to a character buffer
        //   and then compare the contents of that buffer with the expected
        //   output format.
        //
        // Testing:
        //   bsl::ostream& operator<<(bsl::ostream&, const bdet_Date&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Output (<<) Operator" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting 'operator<<' (ostream)." << endl;
        {
            static const struct {
                int         d_lineNum;  // source line number
                int         d_year;     // year under test
                int         d_month;    // month under test
                int         d_day;      // day under test
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LINE  YEAR   MONTH   DAY     FMT
                //----  -----  -----  -----   ---------------
                { L_,      1,     1,     1,   "01JAN0001"     },

                { L_,   1999,     1,     1,   "01JAN1999"     },
                { L_,   1999,     2,     1,   "01FEB1999"     },
                { L_,   1999,     3,     1,   "01MAR1999"     },
                { L_,   1999,     4,     1,   "01APR1999"     },
                { L_,   1999,     5,     1,   "01MAY1999"     },
                { L_,   1999,     6,     1,   "01JUN1999"     },
                { L_,   1999,     7,     1,   "01JUL1999"     },
                { L_,   1999,     8,     1,   "01AUG1999"     },
                { L_,   1999,     9,     1,   "01SEP1999"     },
                { L_,   1999,    10,     1,   "01OCT1999"     },
                { L_,   1999,    11,     1,   "01NOV1999"     },
                { L_,   1999,    12,     1,   "01DEC1999"     },

                { L_,   9999,    12,    31,   "31DEC9999"     },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            const int SIZE = 1000;        // max length of output string
            const char XX = (char) 0xFF;  // value representing an unset 'char'
            char mCtrlBuf[SIZE];  memset(mCtrlBuf, XX, SIZE);
            const char *CTRL_BUF = mCtrlBuf; // used for extra character check

            for (int ti = 0; ti < NUM_DATA;  ++ti) {
                const int         LINE  = DATA[ti].d_lineNum;
                const int         YEAR  = DATA[ti].d_year;
                const int         MONTH = DATA[ti].d_month;
                const int         DAY   = DATA[ti].d_day;
                const char *const FMT   = DATA[ti].d_fmt_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P(FMT)
                }

                char buf[SIZE];
                memcpy(buf, CTRL_BUF, SIZE); // preset buf to 'unset' values

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(YEAR, MONTH, DAY);

                if (veryVeryVerbose) cout << "\t\tEXPECTED FORMAT: " << FMT
                                          << endl;
                ostrstream out(buf, SIZE);  out << X << ends;
                if (veryVeryVerbose) cout << "\t\tACTUAL FORMAT:   " << buf
                                          << endl;

                const int SZ = strlen(FMT) + 1;
                LOOP_ASSERT(LINE, SZ < SIZE);  // check buffer is large enough
                LOOP_ASSERT(LINE, XX == buf[SIZE - 1]);  // check for overrun
                LOOP_ASSERT(LINE, 0  == memcmp(buf, FMT, SZ));
                LOOP_ASSERT(LINE, 0  ==
                                    memcmp(buf + SZ, CTRL_BUF + SZ, SIZE-SZ));
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
        //   void getYearMonthDay(int *year, int *month, int *day)
        //   int year();
        //   int month();
        //   int day();
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING BASIC ACCESSORS" << endl
                                  << "=======================" << endl;

        if (verbose) cout << "\nTesting: 'getYearMonthDay', "
                             "'year()', 'month()', 'day()'" << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day;
            } DATA[] = {
                {    1,  1,  1 }, {   10,  4,  5 }, {  100,  6,  7 },
                { 1000,  8,  9 }, { 1100,  1, 31 }, { 1200,  2, 29 },
                { 1300,  3, 31 }, { 1400,  4, 30 }, { 1500,  5, 31 },
                { 1600,  6, 30 }, { 1700,  7, 31 }, { 1800,  8, 31 },
                { 1900,  9, 30 }, { 2000, 10, 31 }, { 2100, 11, 30 },
                { 2200, 12, 31 }, { 2400, 12, 31 }, { 9999, 12, 31 },
            };

            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR  = DATA[i].d_year;
                const int MONTH = DATA[i].d_month;
                const int DAY   = DATA[i].d_day;
                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P(DAY) }

                int       y, m, d;

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(YEAR, MONTH, DAY);
                X.getYearMonthDay(&y, &m, &d);

                if (veryVeryVerbose) { T_ T_ P_(y) P_(m) P_(d) P(X) }
                LOOP_ASSERT(i, YEAR  == X.year());  LOOP_ASSERT(i, YEAR  == y);
                LOOP_ASSERT(i, MONTH == X.month()); LOOP_ASSERT(i, MONTH == m);
                LOOP_ASSERT(i, DAY   == X.day());   LOOP_ASSERT(i, DAY   == d);
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION 'gg':
        //   Void for 'bdet_date'.
        // --------------------------------------------------------------------

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS:
        //
        // Concerns:
        //   The separate date fields must be correctly forwarded to the
        //   fully-tested underlying utility function.
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
        //   bdet_Date();
        //   void setYearMonthDay(int year, int month, int day)
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING PRIMARY MANIPULATOR" << endl
                                  << "===========================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;

        Obj x;  const Obj& X = x;

        ASSERT(1 == X.year());  ASSERT(1 == X.month());  ASSERT(1 == X.day());

        if (verbose) cout << "\nTesting 'setYearMonthDay'." << endl;
        {
            static const struct {
                int d_year;  int d_month;  int d_day;
            } DATA[] = {
                {    1,  1,  1 }, {   10,  4,  5 }, {  100,  6,  7 },
                { 1000,  8,  9 }, { 1100,  1, 31 }, { 1200,  2, 29 },
                { 1300,  3, 31 }, { 1400,  4, 30 }, { 1500,  5, 31 },
                { 1600,  6, 30 }, { 1700,  7, 31 }, { 1800,  8, 31 },
                { 1900,  9, 30 }, { 2000, 10, 31 }, { 2100, 11, 30 },
                { 2200, 12, 31 }, { 2400, 12, 31 }, { 9999, 12, 31 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int YEAR  = DATA[i].d_year;
                const int MONTH = DATA[i].d_month;
                const int DAY   = DATA[i].d_day;

                if (veryVerbose) { T_ P_(YEAR) P_(MONTH) P(DAY) }

                Obj x;  const Obj& X = x;
                x.setYearMonthDay(YEAR, MONTH, DAY);

                if (veryVeryVerbose) { T_ T_ P(X) }
                LOOP_ASSERT(i, YEAR  == X.year());
                LOOP_ASSERT(i, MONTH == X.month());
                LOOP_ASSERT(i, DAY   == X.day());
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

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        const int YA = 1, MA = 2, DA = 3;  // y, m, d values for VA
        const int YB = 4, MB = 5, DB = 6;  // y, m, d values for VB
        const int YC = 7, MC = 8, DC = 9;  // y, m, d values for VC
        int year, month, day;              // reusable variables for 'get' call

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (init. to VA)."
                             "\t\t{ x1:VA }" << endl;
        Obj mX1(YA, MA, DA);  const Obj& X1 = mX1;
        if (veryVerbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check initial state of x1." << endl;
        year = 0;  month = 0;  day = 0;
        X1.getYearMonthDay(&year, &month, &day);
        ASSERT(YA == year);     ASSERT(MA == month);     ASSERT(DA == day);
        ASSERT(YA == X1.year());ASSERT(MA == X1.month());ASSERT(DA ==X1.day());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create an object x2 (copy from x1)."
                             "\t\t{ x1:VA x2:VA }" << endl;
        Obj mX2(X1);  const Obj& X2 = mX2;
        if (veryVerbose) { T_ P(X2) }

        if (verbose) cout << "\ta. Check the initial state of x2." << endl;
        year = 0;  month = 0;  day = 0;
        X2.getYearMonthDay(&year, &month, &day);
        ASSERT(YA == year);     ASSERT(MA == month);     ASSERT(DA == day);
        ASSERT(YA == X2.year());ASSERT(MA == X2.month());ASSERT(DA ==X2.day());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));        ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));        ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Set x1 to a new value VB."
                             "\t\t\t{ x1:VB x2:VA }" << endl;
        mX1.setYearMonthDay(YB, MB, DB);
        if (veryVerbose) { T_ P(X1) }

        if (verbose) cout << "\ta. Check new state of x1." << endl;
        year = 0;  month = 0;  day = 0;
        X1.getYearMonthDay(&year, &month, &day);
        ASSERT(YB == year);     ASSERT(MB == month);     ASSERT(DB == day);
        ASSERT(YB == X1.year());ASSERT(MB == X1.month());ASSERT(DB ==X1.day());

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
        year = 0;  month = 0;  day = 0;
        X3.getYearMonthDay(&year, &month, &day);
        ASSERT(1 == year);     ASSERT(1 == month);     ASSERT(1 == day);

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
        year = 0;  month = 0;  day = 0;
        X4.getYearMonthDay(&year, &month, &day);
        ASSERT(1 == year);     ASSERT(1 == month);     ASSERT(1 == day);

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));        ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));        ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));        ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));        ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Set x3 to a new value VC."
                             "\t\t\t{ x1:VB x2:VA x3:VC x4:U }" << endl;
        mX3.setYearMonthDay(YC, MC, DC);
        if (veryVerbose) { T_ P(X3) }

        if (verbose) cout << "\ta. Check new state of x3." << endl;
        year = 0;  month = 0;  day = 0;
        X3.getYearMonthDay(&year, &month, &day);
        ASSERT(YC == year);     ASSERT(MC == month);     ASSERT(DC == day);
        ASSERT(YC == X3.year());ASSERT(MC == X3.month());ASSERT(DC ==X3.day());

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
        year = 0;  month = 0;  day = 0;
        X2.getYearMonthDay(&year, &month, &day);
        ASSERT(YB == year);     ASSERT(MB == month);     ASSERT(DB == day);
        ASSERT(YB == X2.year());ASSERT(MB == X2.month());ASSERT(DB ==X2.day());

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
        year = 0;  month = 0;  day = 0;
        X2.getYearMonthDay(&year, &month, &day);
        ASSERT(YC == year);     ASSERT(MC == month);     ASSERT(DC == day);
        ASSERT(YC == X2.year());ASSERT(MC == X2.month());ASSERT(DC ==X2.day());

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
        year = 0;  month = 0;  day = 0;
        X1.getYearMonthDay(&year, &month, &day);
        ASSERT(YB == year);     ASSERT(MB == month);     ASSERT(DB == day);
        ASSERT(YB == X1.year());ASSERT(MB == X1.month());ASSERT(DB ==X1.day());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X1 == X1));        ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));        ASSERT(1 == (X1 != X2));
        ASSERT(0 == (X1 == X3));        ASSERT(1 == (X1 != X3));
        ASSERT(0 == (X1 == X4));        ASSERT(1 == (X1 != X4));

      } break;
      case -1: {
        // --------------------------------------------------------------------
        // 'getYearMonthDay' PERFORMANCE TEST:
        //
        // Concerns:
        //   The method should run very efficiently for date values within the
        //   cache of 'bdeimp_dateutil'.
        //
        // Plan:
        //   Invoke the 'getYearMonthDay' method in a loop for the default
        //   number of times or else for an integral multiple thereof as
        //   specified in the second argument ('argv[2]') to the executable.
        //
        // Testing:
        //   PERFORMANCE: 'getYearMonthDay'
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'getYearMonthDay' PERFORMANCE TEST" << endl
                          << "==================================" << endl;

        if (verbose) cout << "\nTesting performance of 'getYearMonthDay'."
                          << endl;

        const int DEFAULT = 0;   // keep default standard
        int SIZE = verbose && atoi(argv[2]) > DEFAULT ? atoi(argv[2]) :DEFAULT;

        if (veryVerbose) { T_ P(SIZE) }

        //                       vvvv Adjust Here to change default runtime.
        const int size = SIZE * 1000000;

        const int NUM_FEEDBACKS = 50;
        int feedback = size / NUM_FEEDBACKS;
        if (feedback <= 0) feedback = 1;

        if (veryVerbose) cout << "\tPerformance test for 'getYearMonthDay' "
                              << size * 10 << " times. " << endl;
        {
            if (veryVerbose) cerr << "\t"
                "----+----+----+----+----+----+----+----+----+----+" << endl;
            if (veryVerbose) cerr << "\tBEGIN";

            bdet_Date date(2000, 1, 1);
            int y, m, d;

            for (int i = size - 1; i >= 0; --i)  {
                if (veryVerbose && 0 == i % feedback) cerr << "\t.";
                date.getYearMonthDay(&y, &m, &d);
                date.getYearMonthDay(&y, &m, &d);
                date.getYearMonthDay(&y, &m, &d);
                date.getYearMonthDay(&y, &m, &d);
                date.getYearMonthDay(&y, &m, &d);
                date.getYearMonthDay(&y, &m, &d);
                date.getYearMonthDay(&y, &m, &d);
                date.getYearMonthDay(&y, &m, &d);
                date.getYearMonthDay(&y, &m, &d);
                date.getYearMonthDay(&y, &m, &d);
            }
            if (veryVerbose) cerr << "\tEND" << endl;
        }

      } break;
      case -2: {
        // --------------------------------------------------------------------
        // 'month' PERFORMANCE TEST:
        //
        // Concerns:
        //   The method should run very efficiently for date values within the
        //   cache of 'bdeimp_dateutil'.
        //
        // Plan:
        //   Invoke the 'month()' method in a loop for the default number of
        //   times or else for an integral multiple thereof as specified in
        //   the second argument ('argv[2]') to the executable.
        //
        // Testing:
        //   PERFORMANCE: 'month()'
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "'month' PERFORMANCE TEST" << endl
                                  << "========================" << endl;

        if (verbose) cout << "\nTesting performance of 'month()'." << endl;

        const int DEFAULT = 0;   // keep default standard
        int SIZE = verbose && atoi(argv[2]) > DEFAULT ? atoi(argv[2]) :DEFAULT;

        if (veryVerbose) { T_ P(SIZE) }

        //                       vvvv Adjust Here to change default runtime.
        const int size = SIZE * 1000000;

        const int NUM_FEEDBACKS = 50;
        int feedback = size / NUM_FEEDBACKS;
        if (feedback <= 0) feedback = 1;

        if (veryVerbose) cout << "\tPerformance test for 'month()' "
                              << size * 10 << " times. " << endl;
        {
            if (veryVerbose) cerr << "\t"
                "----+----+----+----+----+----+----+----+----+----+" << endl;
            if (veryVerbose) cerr << "\tBEGIN";

            bdet_Date date(2000, 1, 1);

            for (int i = size - 1; i >= 0; --i)  {
                if (veryVerbose && 0 == i % feedback) cerr << '.';
                date.month();
                date.month();
                date.month();
                date.month();
                date.month();
                date.month();
                date.month();
                date.month();
                date.month();
                date.month();
            }
            if (veryVerbose) cerr << "\tEND" << endl;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
