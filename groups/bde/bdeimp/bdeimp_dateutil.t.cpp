// bdeimp_dateutil.t.cpp                                              -*-C++-*-

#include <bdeimp_dateutil.h>

#include <bsl_iomanip.h>
#include <bsl_iostream.h>

#include <bsl_climits.h>
#include <bsl_cstdio.h>      // printf()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>
#include <bsl_ctime.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The component under test consists of a suite of static member functions
// (pure procedures) that perform basic validations and conversions on three
// basic representations of standard Unix dates.  Several of the methods use
// cached data to improve performance, and these methods must be exhaustively
// tested (at least against the un-cached version) to ensure an accurate cache
// is present.  The general plan is that each method is tested against a set
// of tabulated test vectors in all test modes, and additional, exhaustive
// loop-based tests are performed in 'veryVerbose' mode only.
//-----------------------------------------------------------------------------
// [ 1]  static int  isLeapYear(int year);
// [ 2]  static int  numLeapYears(int year1, int year2);
// [ 3]  static int  lastDayOfMonth(int year, int month);
// [ 4]  static int  isValidCalendarDate(int year, int month, int day);
// [ 4]  static int  isValidCalendarDateNoCache(int year, int month, int day);
// [ 4]  static int  isValidYearDayDate(int year, int dayOfYear);
// [ 4]  static int  isValidSerialDate(int serialDay);
// [ 5]  static int  ymd2serial(int year, int month, int day);
// [ 5]  static int  ymd2serialNoCache(int year, int month, int day);
// [ 8]  static int  yd2serial(int year, int dayOfYear);
// [ 9]  static void serial2yd(int *year, int *dayOfYear, int serialDay);
// [ 9]  static int  serial2dayOfYear(int serialDay);
// [ 6]  static int  ymd2dayOfYear(int year, int month, int day);
// [10]  static void serial2ymd(int *year, int *month, int *day, int serialDay)
// [10]  static void serial2ymdNoCache(int *year, int *month, int *day, int sD)
// [ 9]  static int  serial2year(int serialDay);
// [ 9]  static int  serial2yearNoCache(int serialDay);
// [10]  static int  serial2month(int serialDay);
// [10]  static int  serial2monthNoCache(int serialDay);
// [10]  static int  serial2day(int serialDay);
// [10]  static int  serial2dayNoCache(int serialDay);
// [ 7]  static void yd2md(int *month, int *day, int year, int dayOfYear);
// [ 7]  static int  yd2month(int year, int dayOfYear);
// [ 7]  static int  yd2day(int year, int dayOfYear);
// [11]  static int  serial2weekday(int serialDay);
// [11]  static int  yd2weekday(int year, int dayOfYear);
// [11]  static int  ymd2weekday(int year, int month, int day);
//-----------------------------------------------------------------------------
// [12] "y = mx + b" using static int serial2ymd(int serialDay);
// [13] Performance test: isValidCalendarDate
// [14] Performance test: ymd2serial
// [15] Performance test: serial2ymd
// [16] Cache generation code
// [17] Serial-date range calculation
// [18] USAGE
//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
# define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) {                    \
    if (!(X)) { cout << #I << ": " << I << "\t"  \
                     << #J << ": " << J << "\n";  aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                             \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"  \
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
#define P(X)  cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X)  cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                            // current Line number

//=============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
//-----------------------------------------------------------------------------
typedef bdeimp_DateUtil Util;

namespace TestUtil {

static const int db[2][13] =
    {{-1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364},
     {-1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365}};

bool isLeap(int year)
{
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

unsigned int ymdToSerial(int y, int m, int d)
{
    const int* year_data = db[isLeap(y)];
    unsigned int by = y + 32799;
    return by*365 + by/4 - by/100 + by/400 + year_data[m-1] + d;
}

}  // close namespace TestUtil

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Implementing a Value-Semantic Date Type
///- - - - - - - - - - - - - - - - - - - -
// Using the functions supplied in this component, we can easily implement a
// C++ class that represents abstract (*mathematical*) date values and performs
// a few common operations on them.  The internal representation could be any
// of the three supported by this component; in this example, we will choose to
// represent the date value internally as a "serial date":
//..
    class MyDate {
        // This class represents a valid date in the range
        // '[ 0001/01/01 .. 9999/12/31 ]'.

        // DATA
        int d_serialDate;  // 1 = 0001JAN01, 2 = 0001JAN02, ...

        // FRIENDS
        friend MyDate operator+(const MyDate&, int);
        friend MyDate operator+(int, const MyDate&);
        friend int  operator- (const MyDate&, const MyDate&);
        friend bool operator==(const MyDate&, const MyDate&);

      private:
        // PRIVATE CREATORS
        explicit MyDate(int serialDate);
//..
// Next we define the public interface of the class, with function-level
// documentation conspicuously omitted (note, however, that reference
// implementations with preconditions asserted will follow):
//..
      public:
        // TYPES
        enum Day {
            SUN = 1,
            MON = 2,
            TUE = 3,
            WED = 4,
            THU = 5,
            FRI = 6,
            SAT = 7
        };

        // CLASS METHODS
        static bool isValid(int year, int month, int day);

        // CREATORS
        MyDate();
        MyDate(const MyDate& original);
        ~MyDate();

        // MANIPULATORS
        MyDate& operator=(const MyDate& rhs);
        MyDate& operator++();
        MyDate& operator--();
        MyDate& operator+=(int numDays);
        MyDate& operator-=(int numDays);
        void setYearMonthDay(int year, int month, int day);
        bool setYearMonthDayIfValid(int year, int month, int day);

        // ACCESSORS
        void getYearMonthDay(int *year, int *month, int *day) const;
        int year() const;
        int month() const;
        int day() const;
        Day dayOfWeek() const;
        bool isLeapYear() const;
        bsl::ostream& print(bsl::ostream& stream) const;
    };

    // FREE OPERATORS
    bool operator==(const MyDate& lhs, const MyDate& rhs);
    bool operator!=(const MyDate& lhs, const MyDate& rhs);

    bsl::ostream& operator<<(bsl::ostream& stream, const MyDate& rhs);

    MyDate operator+(const MyDate& lhs, int           rhs);
    MyDate operator+(int           lhs, const MyDate& rhs);

    int operator-(const MyDate& lhs, const MyDate& rhs);

    MyDate operator++(MyDate& object, int);
    MyDate operator--(MyDate& object, int);
//..
// We now provide a reference implementation of each of the methods and free
// (operator) functions associated with the 'MyDate' class defined above (using
// 'bsls_assert' to identify preconditions and invariants where appropriate):
//..
    // PRIVATE CREATORS
    inline
    MyDate::MyDate(int serialDate)
    : d_serialDate(serialDate)
    {
    }

    // CLASS METHODS
    inline
    bool MyDate::isValid(int year, int month, int day)
    {
        return bdeimp_DateUtil::isValidCalendarDate(year, month, day);
    }

    // CREATORS
    inline
    MyDate::MyDate()
    : d_serialDate(1)
    {
    }

    inline
    MyDate::MyDate(const MyDate& original)
    : d_serialDate(original.d_serialDate)
    {
    }

    inline
    MyDate::~MyDate()
    {
    }

    // MANIPULATORS
    inline
    MyDate& MyDate::operator=(const MyDate& rhs)
    {
        d_serialDate = rhs.d_serialDate;
        return *this;
    }

    inline
    MyDate& MyDate::operator++()
    {
        ++d_serialDate;
        BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_serialDate));
        return *this;
    }

    inline
    MyDate& MyDate::operator--()
    {
        --d_serialDate;
        BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_serialDate));
        return *this;
    }

    inline
    MyDate& MyDate::operator+=(int numDays)
    {
        d_serialDate += numDays;
        BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_serialDate));
        return *this;
    }

    inline
    MyDate& MyDate::operator-=(int numDays)
    {
        d_serialDate -= numDays;
        BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_serialDate));
        return *this;
    }

    inline
    void MyDate::setYearMonthDay(int year, int month, int day)
    {
        d_serialDate = bdeimp_DateUtil::ymd2serial(year, month, day);
        BSLS_ASSERT_SAFE(bdeimp_DateUtil::isValidSerialDate(d_serialDate));
    }

    inline
    bool MyDate::setYearMonthDayIfValid(int year, int month, int day)
    {
        const int newDate = bdeimp_DateUtil::ymd2serial(year, month, day);
        if (bdeimp_DateUtil::isValidSerialDate(newDate)) {
            d_serialDate = newDate;
            return true;
        }
        return false;
    }

    // ACCESSORS
    inline
    void MyDate::getYearMonthDay(int *year, int *month, int *day) const
    {
        bdeimp_DateUtil::serial2ymd(year, month, day, d_serialDate);
    }

    inline
    int MyDate::year() const
    {
        return bdeimp_DateUtil::serial2year(d_serialDate);
    }

    inline
    int MyDate::month() const
    {
        return bdeimp_DateUtil::serial2month(d_serialDate);
    }

    inline
    int MyDate::day() const
    {
        return bdeimp_DateUtil::serial2day(d_serialDate);
    }

    inline
    MyDate::Day MyDate::dayOfWeek() const
    {
        return MyDate::Day(bdeimp_DateUtil::serial2weekday(d_serialDate));
    }

    inline
    bool MyDate::isLeapYear() const
    {
        return bdeimp_DateUtil::isLeapYear(year());
    }

    // FREE OPERATORS
    inline
    bool operator==(const MyDate& lhs, const MyDate& rhs)
    {
        return lhs.d_serialDate == rhs.d_serialDate;
    }

    inline
    bool operator!=(const MyDate& lhs, const MyDate& rhs)
    {
        return !(lhs == rhs);
    }

    inline
    bsl::ostream& operator<<(bsl::ostream& stream, const MyDate& rhs)
    {
        return rhs.print(stream);
    }

    inline
    MyDate operator+(const MyDate& lhs, int rhs)
    {
        return MyDate(lhs.d_serialDate + rhs);
    }

    inline
    MyDate operator+(int lhs, const MyDate& rhs)
    {
        return MyDate(lhs + rhs.d_serialDate);
    }

    inline
    int operator-(const MyDate& lhs, const MyDate& rhs)
    {
        return lhs.d_serialDate - rhs.d_serialDate;
    }

    inline
    MyDate operator++(MyDate& object, int)
    {
        MyDate tmp(object);
        ++object;
        return tmp;
    }

    inline
    MyDate operator--(MyDate& object, int)
    {
        MyDate tmp(object);
        --object;
        return tmp;
    }
//..
// The following definitions would appropriately appear in the implementation
// ('.cpp') file:
//..
    const char *const monthNames[] = {
        0, "JAN", "FEB", "MAR", "APR",
           "MAY", "JUN", "JUL", "AUG",
           "SEP", "OCT", "NOV", "DEC"
    };

    // MANIPULATORS
    bsl::ostream& MyDate::print(bsl::ostream& stream) const
    {
        if (!stream) {
            return stream;                                            // RETURN
        }

        // space usage: ddMMMyyyy null
        const int SIZE = 2 + 3 + 4 + 1;
        char buf[SIZE];

        int y, m, d;
        bdeimp_DateUtil::serial2ymd(&y, &m, &d, d_serialDate);

        buf[0] = d / 10 + '0';
        buf[1] = d % 10 + '0';

        bsl::memcpy(&buf[2], monthNames[m], 3);

        buf[5] = y / 1000 + '0';
        buf[6] = ((y % 1000) / 100) + '0';
        buf[7] = ((y % 100) / 10) + '0';
        buf[8] = y % 10 + '0';
        buf[9] = 0;

        stream << buf;

        return stream;
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int shortTestFlag = !(argc > 3) ; // default: short test (see 'enum' below)

    enum {
        SHORT_YEAR_START   = 1699,    // "Reasonable History"
        SHORT_YEAR_END     = 2201,    // "Reasonable Future"

        SHORT_SERIAL_START = 620195,  // 1699/1/1
        SHORT_SERIAL_END   = 803900,  // 2201/12/12

        Y9999_END          = 3652061  // 9999/12/31 (last supported date)
    };

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 19: {
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

///Usage
///-----
// This component was created primarily to support the implementation of a
// general-purpose, value-semantic (vocabulary) "Date" type, but also provides
// many low-level utility functions suitable for direct use by other clients.
//
///As a General Purpose Utility
/// - - - - - - - - - - - - - -
// Many of the functions provided by this component can be used directly by
// clients that want to ask questions about a particular date in one of the
// three supported formats.
//
// What day of the week was January 3, 2010?
//..
    ASSERT(2 == bdeimp_DateUtil::ymd2weekday(2010, 3, 1));   // 2 means Monday.
//..
// Was the year 2000 a leap year?
//..
    ASSERT(true == bdeimp_DateUtil::isLeapYear(2000));       // Yes, it was.
//..
// Was February 29, 1900, a valid date in history?
//..
    ASSERT(false == bdeimp_DateUtil::isValidCalendarDate(1900, 2, 29));
                                                             // No, it was not.
//..
// What was the last day of February in 1600?
//..
    ASSERT(29 == bdeimp_DateUtil::lastDayOfMonth(1600, 2));  // The 29th.
//..
// How many leap years occur from 1959 to 2012 inclusive?
//..
    ASSERT(14 == bdeimp_DateUtil::numLeapYears(1959, 2012)); // There are 14.
//..
// On what day of the year does February 29, 2020 fall?
//..
    ASSERT(60 == bdeimp_DateUtil::ymd2dayOfYear(2020, 2, 29));
                                                             // The 60th one.
//..
// In what month does the 120th day of 2011 fall?
//..
    ASSERT(4 == bdeimp_DateUtil::yd2month(2011, 120));       // 4 means April.
//..

  // This excerpt of the Usage is rendered above (outside of 'main').

//..
// The following snippets of code illustrate how to create and use a 'Mydate'
// object.  First create a default object, 'd1':
//..
    MyDate d1;                        ASSERT(   1 == d1.year());
                                      ASSERT(   1 == d1.month());
                                      ASSERT(   1 == d1.day());
//..
// Next, set 'd1' to July 4, 1776:
//..
    d1.setYearMonthDay(1776, 7, 4);   ASSERT(1776 == d1.year());
                                      ASSERT(   7 == d1.month());
                                      ASSERT(   4 == d1.day());
//..
// Then create 'd2' as a copy of 'd1':
//..
    MyDate d2(d1);                    ASSERT(1776 == d2.year());
                                      ASSERT(   7 == d2.month());
                                      ASSERT(   4 == d2.day());
//..
// Now, add six days to the value of 'd2':
//..
    d2 += 6;                          ASSERT(1776 == d2.year());
                                      ASSERT(   7 == d2.month());
                                      ASSERT(  10 == d2.day());
//..
// Next subtract 'd1' from 'd2', storing the difference (in days) in 'dDays':
//..
    int dDays = d2 - d1;              ASSERT(6 == dDays);
//..
// Finally, stream the value of 'd2' to 'stdout':
//..
if (veryVerbose)
    bsl::cout << d2 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  10JUL1776
//..

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // Prolepic Date calculations
        // --------------------------------------------------------------------

          cout << "\nTesting: proleptic " << endl;

//           int x = Util::ymd2ProlepticSerial(9999, 12, 31);
//           P(x)

          int ey, em, ed;
          Util::prolepticSerial2ymd(&ey, &em, &ed, 366);

          int serialDate = 0;
          for (int year = 1; year <= 9999; ++year) {
              for (int month = 1; month <= 12; ++month) {
                  for (int day = 1; day <= 31; ++day) {
                      if (Util::isValidProlepticCalendarDate(year,
                                                             month,
                                                             day)) {
                          if (veryVerbose) { P_(year) P_(month) P(day) }

                          ++serialDate;

                          int x = Util::ymd2ProlepticSerial(year, month, day);
                          LOOP5_ASSERT(year, month, day, serialDate, x,
                                       serialDate == x);
                          int y = TestUtil::ymdToSerial(year, month, day);
                          LOOP5_ASSERT(year, month, day, x, y - 11979953,
                                       x == (y - 11979953));

                          if (veryVerbose) { P(serialDate) }

                          int ey, em, ed;
                          Util::prolepticSerial2ymd(&ey, &em, &ed, serialDate);
                          LOOP3_ASSERT(serialDate, ey, year,  ey == year);
                          LOOP3_ASSERT(serialDate, em, month, em == month);
                          LOOP3_ASSERT(serialDate, ed, day,   ed == day);
                      }
                  }
              }
          }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // Serial-date short-test range calculations:
        // --------------------------------------------------------------------

        if (verbose) {
            int year1 = SHORT_YEAR_START;
            int year2 = SHORT_YEAR_END;
            cout << "serial date for SHORT_YEAR_START/1/1: "
                 << Util::ymd2serialNoCache(year1, 1, 1) << endl;
            cout << "serial date for SHORT_YEAR_END/12/31: "
                 << Util::ymd2serialNoCache(year2, 12, 31) << endl;
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // Date-Cache Code Generation:
        //
        //   Generate the C++ code that will in turn be inserted into the
        //   'bdet_dateimputil.cpp' file to implement the Date-Cache
        // --------------------------------------------------------------------

        if (verbose) {
        // everything hard-coded for now!!!

        int firstYear       = 1980;
        int lastYear        = 2040;
        int firstSerialDate = Util::ymd2serialNoCache(firstYear, 1, 1);
        int lastSerialDate  = Util::ymd2serialNoCache(lastYear, 12, 31);
        int numCachedYears  = lastYear - firstYear + 1;

        int si;

        cout << "const int bdeimp_DateUtil::s_firstCachedYear       = "
             << firstYear << ";" << endl;

        cout << "const int bdeimp_DateUtil::s_lastCachedYear        = "
             << lastYear << ";" << endl;

        cout << "const int bdeimp_DateUtil::s_firstCachedSerialDate = "
             << firstSerialDate << ";" << endl;

        cout << "const int bdeimp_DateUtil::s_lastCachedSerialDate  = "
             << lastSerialDate << ";\n" << endl;

        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

        cout << "const bdeimp_DateUtil::YearMonthDay\n"
             << "    bdeimp_DateUtil::s_cachedYearMonthDay[] = {"
             << endl;
        for (int i = firstSerialDate; i <= lastSerialDate; ++i) {
            cout << "  { "
                 << Util::serial2yearNoCache(i)  << ", " << setw(2)
                 << Util::serial2monthNoCache(i) << ", " << setw(2)
                 << Util::serial2dayNoCache(i)   << " },";
            if ((i - firstSerialDate + 1) % 4 == 0)  cout << "\n";
        }
        cout << "};\n" << endl;

        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

        cout << "const int bdeimp_DateUtil::s_cachedSerialDate["
             << numCachedYears << "][13] = {" << endl;
        for (si = firstYear; si <= lastYear; ++si) {
            cout << "  { 0, ";
            for (int sj = 1; sj <= 12; ++sj) {
                cout << setw(6)
                     << Util::ymd2serialNoCache(si, sj, 1) - 1
                     << ", ";
                if (sj % 7 == 6)  cout << "\n       ";
            }
            cout << "  }," << endl;
        }
        cout << "};\n" << endl;

        // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

        cout << "const char bdeimp_DateUtil::s_cachedDaysInMonth["
             << numCachedYears << "][13] = {" << endl;
        for (si = firstYear; si <= lastYear; ++si) {
            cout << "  { 0, 31, " << setw(2)
                 << 28 + (Util::isLeapYear(si))
                 << ", 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }," << endl;
        }
        cout << "};" << endl;
       }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // Performance Test
        //   static int serial2ymd(int serialDay);
        // --------------------------------------------------------------------

        if (verbose) {
        int year = 1900, month = 2, day = 16;
        int sDay = 730982;

        int testFlag = verbose ? atoi(argv[2]) : 0;

        switch (testFlag) {
          case 1: {
            cout << "Testing serial2ymdNoCache(&y, &m, &d, serialDay):"
                 << endl;

            //t1 =time(0);
            for (int i = 0; i < 10000000; ++i) {
                Util::serial2ymdNoCache(&year, &month, &day, sDay);
                Util::serial2ymdNoCache(&year, &month, &day, sDay);
                Util::serial2ymdNoCache(&year, &month, &day, sDay);
                Util::serial2ymdNoCache(&year, &month, &day, sDay);
                Util::serial2ymdNoCache(&year, &month, &day, sDay);

                Util::serial2ymdNoCache(&year, &month, &day, sDay);
                Util::serial2ymdNoCache(&year, &month, &day, sDay);
                Util::serial2ymdNoCache(&year, &month, &day, sDay);
                Util::serial2ymdNoCache(&year, &month, &day, sDay);
                Util::serial2ymdNoCache(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 2: {
            cout << "Testing serial2ymd(&y, &m, &d, serialDay):" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);

                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          default: {
          } break;
        }
      }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // Performance Test
        //   static int ymd2serial(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) {
        int year = 2000, month = 2, day = 16;

        int testFlag = verbose ? atoi(argv[2]) : 0;

        switch (testFlag) {
          case 1: {
            cout << "Testing ymd2serialNoCache(y, m, d):" << endl;

            //t1 =time(0);
            for (int i = 0; i < 10000000; ++i) {
                Util::ymd2serialNoCache(year, month, day);
                Util::ymd2serialNoCache(year, month, day);
                Util::ymd2serialNoCache(year, month, day);
                Util::ymd2serialNoCache(year, month, day);
                Util::ymd2serialNoCache(year, month, day);

                Util::ymd2serialNoCache(year, month, day);
                Util::ymd2serialNoCache(year, month, day);
                Util::ymd2serialNoCache(year, month, day);
                Util::ymd2serialNoCache(year, month, day);
                Util::ymd2serialNoCache(year, month, day);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 2: {
            cout << "Testing ymd2serial(y, m, d):" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::ymd2serial(year, month, day);
                Util::ymd2serial(year, month, day);
                Util::ymd2serial(year, month, day);
                Util::ymd2serial(year, month, day);
                Util::ymd2serial(year, month, day);

                Util::ymd2serial(year, month, day);
                Util::ymd2serial(year, month, day);
                Util::ymd2serial(year, month, day);
                Util::ymd2serial(year, month, day);
                Util::ymd2serial(year, month, day);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          default: {
          } break;
        }
      }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // Performance Test
        //   static bool isValidCalendarDate(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) {
        int year = 2000, month = 2, day = 16;

        int testFlag = verbose ? atoi(argv[2]) : 0;

        switch (testFlag) {
          case 1: {
            cout << "Testing isValidCalendarDateNoCache(y, m, d):" << endl;

            //t1 =time(0);
            for (int i = 0; i < 10000000; ++i) {
                Util::isValidCalendarDateNoCache(year, month, day);
                Util::isValidCalendarDateNoCache(year, month, day);
                Util::isValidCalendarDateNoCache(year, month, day);
                Util::isValidCalendarDateNoCache(year, month, day);
                Util::isValidCalendarDateNoCache(year, month, day);

                Util::isValidCalendarDateNoCache(year, month, day);
                Util::isValidCalendarDateNoCache(year, month, day);
                Util::isValidCalendarDateNoCache(year, month, day);
                Util::isValidCalendarDateNoCache(year, month, day);
                Util::isValidCalendarDateNoCache(year, month, day);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 2: {
            //t1 =time(0);
            cout << "Testing isValidCalendarDate(y, m, d):" << endl;

            for (int i = 0; i < 100000000; ++i) {
                Util::isValidCalendarDate(year, month, day);
                Util::isValidCalendarDate(year, month, day);
                Util::isValidCalendarDate(year, month, day);
                Util::isValidCalendarDate(year, month, day);
                Util::isValidCalendarDate(year, month, day);

                Util::isValidCalendarDate(year, month, day);
                Util::isValidCalendarDate(year, month, day);
                Util::isValidCalendarDate(year, month, day);
                Util::isValidCalendarDate(year, month, day);
                Util::isValidCalendarDate(year, month, day);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          default: {
          } break;
        }
      }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // Performance Test
        //   "y = mx + b" using static int serial2ymd(int serialDay);
        // --------------------------------------------------------------------

        if (verbose) {
        int year = 1900, month = 2, day = 16;
        int sDay = 730982;
        int i;

        int testFlag = verbose ? atoi(argv[2]) : 0;

        switch (testFlag) {
          case 1: {
            cout << "n = 1:" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 2: {
            cout << "n = 2:" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 3: {
            cout << "n = 3:" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 4: {
            cout << "n = 4:" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 5: {
            cout << "n = 5:" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 6: {
            cout << "n = 6:" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);

                Util::serial2ymd(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 7: {
            cout << "n = 7:" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);

                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 8: {
            cout << "n = 8:" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);

                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 9: {
            cout << "n = 9:" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);

                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 10: {
            cout << "n = 10:" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);

                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 15: {
            cout << "n = 15" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);

                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);

                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
            }
            //t2 =time(0);
            //cout << "Elapsed time = " << difftime(t2, t1) << endl;
          } break;
          case 20: {
            cout << "n = 20" << endl;

            //t1 =time(0);
            for (int i = 0; i < 100000000; ++i) {
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);

                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);

                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);

                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
                Util::serial2ymd(&year, &month, &day, sDay);
            }
          } break;
          default: {
            cout << "No such a case!" << endl;
          } break;
        }
        }
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // In this test we take advantage of the fact that all of the date
        // conversion functions have already been tested by tabulating the
        // expected results for 'ymd2weekday' and then generating the
        // corresponding test vectors from the year/month/day representation.
        //
        // Testing:
        //   static int serial2weekday(int serialDay);
        //   static int yd2weekday(int year, int dayOfYear);
        //   static int ymd2weekday(int year, int month, int day);
        // --------------------------------------------------------------------

        enum { SUN = 1, MON, TUE, WED, THU, FRI, SAT };

        if (verbose) cout << "\nTesting {ymd,yd,serial}2weekday Methods"
                          << "\n======================================="
                          << endl;

        if (verbose) cout << "\nDirect test vectors\n" << endl;

        {
            static const struct {
                int d_lineNum;    // source line number
                int d_year;       // year under test
                int d_month;      // month under test
                int d_day;        // day under test
                int d_exp;        // expected value

            } DATA[] = {
                //line no.  year   month   day     expected value
                //-------   -----  -----  -----    --------------
              //{ L_,          0,     0,     0,         0 },
              //{ L_,          1,     1,     0,         0 },
              //{ L_,          1,     0,     1,         0 },
              //{ L_,          0,     1,     1,         0 },
              //{ L_,          1,     1,    -1,         0 },
              //{ L_,          1,    -1,     1,         0 },
              //{ L_,         -1,     1,     1,         0 },

                { L_,          1,     1,     1,       SAT },
                { L_,          1,     1,     2,       SUN },
                { L_,          1,     1,     3,       MON },
                { L_,          1,     1,     4,       TUE },
                { L_,          1,     1,     5,       WED },
                { L_,          1,     1,     6,       THU },
                { L_,          1,     1,     7,       FRI },
                { L_,          1,     1,     8,       SAT },

                { L_,          1,     2,     1,       TUE },
                { L_,          2,     1,     1,       SUN },

                { L_,       1600,    12,    31,       WED },
                { L_,       1752,     1,     1,       WED },
                { L_,       1752,     9,     1,       TUE },
                { L_,       1752,     9,     2,       WED },
                { L_,       1752,     9,    14,       THU },
                { L_,       1752,     9,    15,       FRI },

                { L_,       1999,    12,    31,       FRI },
                { L_,       2000,     1,     1,       SAT },
                { L_,       2000,     2,    29,       TUE },
                { L_,       9999,    12,    31,       FRI },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int y = DATA[di].d_year;
                const int m = DATA[di].d_month;
                const int d = DATA[di].d_day;
                const int w = DATA[di].d_exp;

                int w1 = Util::ymd2weekday(y, m, d);
                int j  = Util::ymd2dayOfYear(y, m, d);
                int w2 = Util::yd2weekday(y, j);
                int s  = Util::yd2serial(y, j);
                int w3 = Util::serial2weekday(s);

                if (verbose) cout << w << ' '
                                  << w1 << ' ' << w2 << ' ' << w3 << endl;

                ASSERT(w == w1); ASSERT(w == w2); ASSERT(w == w3);
            }
        }

      } break;
      case 10: {
        // --------------------------------------------------------------------
        // Testing:
        //   static void serial2ymd(int *year, int *month, int *day,
        //                          int serialDate);
        //   static void serial2ymdNoCache(int *year, int *month, int *day,
        //                          int serialDate);
        //   static int serial2year(int serialDate);
        //   static int serial2yearNoCache(int serialDate);
        //   static int serial2month(int serialDate);
        //   static int serial2monthNoCache(int serialDate);
        //   static int serial2day(int serialDate);
        //   static int serial2dayNoCache(int serialDate);
        //
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'serial2ymd' and kin"
                          << "\n============================" << endl;

        if (verbose) cout << "\nDirect test vectors\n" << endl;

        {
            static const struct {
                int d_lineNum;    // source line number
                int d_serial;     // serial date under test
                int d_expYear;    // expected year value
                int d_expMonth;   // expected month value
                int d_expDay ;    // expected day value
            } DATA[] = {
                //                         expected values
                //line no.  serial date   year   month  day
                //-------   -----------   -----  -----  -----
                { L_,               1,       1,     1,    1 },
                { L_,               2,       1,     1,    2 },
                { L_,              32,       1,     2,    1 },
                { L_,             365,       1,    12,   31 },
                { L_,             366,       2,     1,    1 },
                { L_,             730,       2,    12,   31 },
                { L_,             731,       3,     1,    1 },
                { L_,            1095,       3,    12,   31 },
                { L_,            1096,       4,     1,    1 },
                { L_,            1460,       4,    12,   30 },
                { L_,            1461,       4,    12,   31 },
                { L_,            1462,       5,     1,    1 },

                { L_,          639188,    1751,     1,    1 },
                { L_,          639552,    1751,    12,   31 },
                { L_,          639553,    1752,     1,    1 },
                { L_,          639907,    1752,    12,   31 },
                { L_,          639908,    1753,     1,    1 },
                { L_,          640272,    1753,    12,   31 },
                { L_,          640273,    1754,     1,    1 },
                { L_,          640637,    1754,    12,   31 },
                { L_,          640638,    1755,     1,    1 },
                { L_,          641002,    1755,    12,   31 },
                { L_,          641003,    1756,     1,    1 },
                { L_,          641368,    1756,    12,   31 },

                { L_,          657073,    1799,    12,   31 },
                { L_,          657074,    1800,     1,    1 },
                { L_,          657438,    1800,    12,   31 },
                { L_,          657439,    1801,     1,    1 },
                { L_,          657803,    1801,    12,   31 },
                { L_,          657804,    1802,     1,    1 },

                { L_,          693597,    1899,    12,   31 },
                { L_,          693598,    1900,     1,    1 },
                { L_,          693962,    1900,    12,   31 },
                { L_,          693963,    1901,     1,    1 },
                { L_,          694327,    1901,    12,   31 },
                { L_,          694328,    1902,     1,    1 },

                { L_,          730121,    1999,    12,   31 },
                { L_,          730122,    2000,     1,    1 },
                { L_,          730487,    2000,    12,   31 },
                { L_,          730488,    2001,     1,    1 },
                { L_,          730852,    2001,    12,   31 },
                { L_,          730853,    2002,     1,    1 },

                { L_,          766646,    2099,    12,   31 },
                { L_,          766647,    2100,     1,    1 },
                { L_,          767011,    2100,    12,   31 },
                { L_,          767012,    2101,     1,    1 },
                { L_,          767376,    2101,    12,   31 },
                { L_,          767377,    2102,     1,    1 },

                { L_,          876218,    2399,    12,   31 },
                { L_,          876219,    2400,     1,    1 },
                { L_,          876584,    2400,    12,   31 },
                { L_,          876585,    2401,     1,    1 },
                { L_,          876949,    2401,    12,   31 },
                { L_,          876950,    2402,     1,    1 },

                { L_,         3650235,    9994,    12,   31 },
                { L_,         3650236,    9995,     1,    1 },
                { L_,         3650600,    9995,    12,   31 },
                { L_,         3650601,    9996,     1,    1 },
                { L_,         3650966,    9996,    12,   31 },
                { L_,         3650967,    9997,     1,    1 },
                { L_,         3651331,    9997,    12,   31 },
                { L_,         3651332,    9998,     1,    1 },
                { L_,         3651696,    9998,    12,   31 },
                { L_,         3651697,    9999,     1,    1 },
                { L_,       Y9999_END,    9999,    12,   31 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                int       yy, mm, dd;
                Util::serial2ymd(&yy, &mm, &dd, DATA[di].d_serial);
                if (verbose) {
                    cout << setw(15) << DATA[di].d_serial << ": "
                         << setw(4) << yy << " / " << setw(2) << mm
                         << " / " << setw(2) << dd << endl;
                }
                LOOP_ASSERT(LINE, DATA[di].d_expYear  == yy);
                LOOP_ASSERT(LINE, DATA[di].d_expMonth == mm);
                LOOP_ASSERT(LINE, DATA[di].d_expDay   == dd);
                yy = mm = dd = -1;
                Util::serial2ymdNoCache(&yy, &mm, &dd, DATA[di].d_serial);
                if (verbose) {
                    cout << setw(15) << DATA[di].d_serial << ": "
                         << setw(4) << yy << " / " << setw(2) << mm
                         << " / " << setw(2) << dd << endl;
                }
                LOOP_ASSERT(LINE, DATA[di].d_expYear  == yy);
                LOOP_ASSERT(LINE, DATA[di].d_expMonth == mm);
                LOOP_ASSERT(LINE, DATA[di].d_expDay   == dd);
            }
        }

        if (verbose)
            cout << "\n(y, m, d) -> serial -> (y, m, d)\n"
                 << "\t(in a loop)" << endl;

        int first = shortTestFlag ? SHORT_YEAR_START : -1;
        int last  = shortTestFlag ? SHORT_YEAR_END : 10000;
        for (int y = first; y <= last; ++y) {
            if (verbose && 0 == y % 100 ) {
                cout << "\tExhaustively testing years " << y/100 << "00"
                     << " to " << y/100 << 99 << endl;
            }
            for (int m = 1; m <= 12; ++m)
            for (int d = 1; d <= 31; ++d) {
                if (Util::isValidCalendarDate(y, m, d)) {
                    int year2, year2NC, month2, month2NC, day2, day2NC;
                    int serial = Util::ymd2serial(y, m, d);

                    int year1  = Util::serial2year (serial);
                    int month1 = Util::serial2month(serial);
                    int day1   = Util::serial2day  (serial);
                    Util::serial2ymd(&year2, &month2, &day2, serial);

                    int serialR = Util::ymd2serialNoCache(y, m, d);

                    int year1NC  = Util::serial2yearNoCache (serial);
                    int month1NC = Util::serial2monthNoCache(serial);
                    int day1NC   = Util::serial2dayNoCache  (serial);
                    Util::serial2ymdNoCache(&year2NC,
                                            &month2NC,
                                            &day2NC,
                                            serialR);

                    ASSERT(serial == serialR);
                    ASSERT(y == year1);     ASSERT(y == year2);
                    ASSERT(m == month1);    ASSERT(m == month2);
                    ASSERT(d == day1);      ASSERT(d == day2);
                    ASSERT(y == year1NC);   ASSERT(y == year2NC);
                    ASSERT(m == month1NC);  ASSERT(m == month2NC);
                    ASSERT(d == day1NC);    ASSERT(d == day2NC);
                }
            }
        }

        if (verbose) cout << "\nserial -> (y, m, d) -> serial" << endl;

        first = shortTestFlag ? SHORT_SERIAL_START : 1;
        last  = shortTestFlag ? SHORT_SERIAL_END : Y9999_END;

        for (int s = first; s <= last; ++s) {
            int year2, year2NC, month2, month2NC, day2, day2NC;
            if (verbose && 0 == s % 100000 ) {
                cout << "\tExhaustively testing dates " << s/100000 << "00000"
                     << " to " << s/100000 << 99999 << endl;
            }

            int year1  = Util::serial2year (s);
            int month1 = Util::serial2month(s);
            int day1   = Util::serial2day  (s);
            Util::serial2ymd(&year2, &month2, &day2, s);
            ASSERT(year1  == year2);
            ASSERT(month1 == month2);
            ASSERT(day1   == day2);

            int year1NC  = Util::serial2yearNoCache (s);
            int month1NC = Util::serial2monthNoCache(s);
            int day1NC   = Util::serial2dayNoCache  (s);
            Util::serial2ymdNoCache(&year2NC, &month2NC, &day2NC, s);
            ASSERT(year1NC  == year2NC);
            ASSERT(month1NC == month2NC);
            ASSERT(day1NC   == day2NC);

            if (s >= 1 && s <= Y9999_END) {
                ASSERT(s == Util::ymd2serial(year1, month1, day1));
                ASSERT(s == Util::ymd2serialNoCache(year1NC,
                                                    month1NC,
                                                    day1NC));
            }
            else {
                ASSERT(0 == Util::ymd2serial(year1, month1, day1));
                ASSERT(0 == Util::ymd2serialNoCache(year1NC,
                                                    month1NC,
                                                    day1NC));
            }
        }

      } break;
      case 9: {
        // --------------------------------------------------------------------
        // Testing:
        //   static void serial2yd(int *year, int *dayOfYear, int serialDate);
        //   static int serial2dayOfYear(int serialDate);
        //   static int serial2year(int serialDate);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'serial2yd'"
                          << "\n===================" << endl;

        if (verbose) cout << "\nDirect test vectors\n" << endl;

        {
            static const struct {
                int d_lineNum;  // source line number
                int d_serial;   // serial date under test
                int d_expYear;  // expected year value
                int d_expDay ;  // expected day value
            } DATA[] = {
                //                       expected value
                //line no.  serial date   year   Day
                //-------   -----------   -----  -----
                { L_,               1,       1,     1 },
                { L_,               2,       1,     2 },
                { L_,              32,       1,    32 },
                { L_,             365,       1,   365 },
                { L_,             366,       2,     1 },
                { L_,             730,       2,   365 },
                { L_,             731,       3,     1 },
                { L_,            1095,       3,   365 },
                { L_,            1096,       4,     1 },
                { L_,            1460,       4,   365 },
                { L_,            1461,       4,   366 },
                { L_,            1462,       5,     1 },

                { L_,          639188,  1751,       1 },
                { L_,          639552,  1751,     365 },
                { L_,          639553,  1752,       1 },
                { L_,          639907,  1752,     355 },
                { L_,          639908,  1753,       1 },
                { L_,          640272,  1753,     365 },
                { L_,          640273,  1754,       1 },
                { L_,          640637,  1754,     365 },
                { L_,          640638,  1755,       1 },
                { L_,          641002,  1755,     365 },
                { L_,          641003,  1756,       1 },
                { L_,          641368,  1756,     366 },

                { L_,          657073,  1799,     365 },
                { L_,          657074,  1800,       1 },
                { L_,          657438,  1800,     365 },
                { L_,          657439,  1801,       1 },
                { L_,          657803,  1801,     365 },
                { L_,          657804,  1802,       1 },

                { L_,          693597,  1899,     365 },
                { L_,          693598,  1900,       1 },
                { L_,          693962,  1900,     365 },
                { L_,          693963,  1901,       1 },
                { L_,          694327,  1901,     365 },
                { L_,          694328,  1902,       1 },

                { L_,          730121,  1999,     365 },
                { L_,          730122,  2000,       1 },
                { L_,          730487,  2000,     366 },
                { L_,          730488,  2001,       1 },
                { L_,          730852,  2001,     365 },
                { L_,          730853,  2002,       1 },

                { L_,          766646,  2099,     365 },
                { L_,          766647,  2100,       1 },
                { L_,          767011,  2100,     365 },
                { L_,          767012,  2101,       1 },
                { L_,          767376,  2101,     365 },
                { L_,          767377,  2102,       1 },

                { L_,          876218,  2399,     365 },
                { L_,          876219,  2400,       1 },
                { L_,          876584,  2400,     366 },
                { L_,          876585,  2401,       1 },
                { L_,          876949,  2401,     365 },
                { L_,          876950,  2402,       1 },

                { L_,         3650235,  9994,     365 },
                { L_,         3650236,  9995,       1 },
                { L_,         3650600,  9995,     365 },
                { L_,         3650601,  9996,       1 },
                { L_,         3650966,  9996,     366 },
                { L_,         3650967,  9997,       1 },
                { L_,         3651331,  9997,     365 },
                { L_,         3651332,  9998,       1 },
                { L_,         3651696,  9998,     365 },
                { L_,         3651697,  9999,       1 },
                { L_,       Y9999_END,  9999,     365 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE     = DATA[di].d_lineNum;
                const int EXP_YEAR = DATA[di].d_expYear;
                const int EXP_DAY  = DATA[di].d_expDay;

                int       yy, jj;
                Util::serial2yd(&yy, &jj, DATA[di].d_serial);
                if (verbose) {
                    cout << setw(15) << DATA[di].d_serial << ": "
                         << setw(4) << yy << " / " << setw(3) << jj << endl;
                }
                LOOP3_ASSERT(LINE, EXP_YEAR, yy, EXP_YEAR == yy);
                LOOP3_ASSERT(LINE, EXP_DAY, jj, EXP_DAY == jj);

            }
        }

        if (verbose) cout << "\n(y, j) -> serial -> (y, j)\n"
                          << "\t(in a long loop)" << endl;

        const int first = shortTestFlag ? SHORT_YEAR_START : -1;
        const int last  = shortTestFlag ? SHORT_YEAR_END : 10000;
        for (int y = first; y <= last; ++y) {
            if (verbose && 0 == y % 100) {
                cout << "\tExhaustively testing years " << y/100 << "00"
                     << " to " << y/100 << 99 << endl;
            }

            for (int j = -1; j <= 367; ++j) {
                if (Util::isValidYearDayDate(y, j)) {
                    int yy, jj;
                    int s = Util::yd2serial(y, j);
                    Util::serial2yd(&yy, &jj, s);

                    ASSERT(y == yy);  ASSERT(j == jj);
                    ASSERT(y == Util::serial2year(s));
                    ASSERT(j == Util::serial2dayOfYear(s));
                }
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // Testing:
        //   static int yd2serial(int year, int dayOfYear);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'yd2serial'"
                          << "\n===================" << endl;

        if (verbose) cout << "\tDirect test vectors\n" << endl;

        {
            static const struct {
                int d_lineNum;    // source line number
                int d_year;       // year under test
                int d_dayOfYear;  // day under test
                int d_exp;        // expected value
            } DATA[] = {
                //line no.  year   Day      expected value
                //-------   -----  -----    --------------
                { L_,          1,     1,           1 },
                { L_,          1,   365,         365 },
                { L_,          2,     1,         366 },
                { L_,          2,   365,         730 },
                { L_,          3,   365,        1095 },
                { L_,          4,   365,        1460 },
                { L_,          4,   366,        1461 },
                { L_,          5,   365,        1826 },

                { L_,       1751,     1,      639188 },
                { L_,       1751,   365,      639552 },
                { L_,       1752,     1,      639553 },
                { L_,       1752,   355,      639907 },
                { L_,       1753,     1,      639908 },

                { L_,       1799,   365,      657073 },
                { L_,       1800,     1,      657074 },
                { L_,       1800,   365,      657438 },
                { L_,       1801,     1,      657439 },

                { L_,       1899,   365,      693597 },
                { L_,       1900,     1,      693598 },
                { L_,       1900,   365,      693962 },
                { L_,       1901,     1,      693963 },

                { L_,       1999,   365,      730121 },
                { L_,       2000,     1,      730122 },
                { L_,       2000,   366,      730487 },
                { L_,       2001,     1,      730488 },

                { L_,       2099,   365,      766646 },
                { L_,       2100,     1,      766647 },
                { L_,       2100,   365,      767011 },
                { L_,       2101,     1,      767012 },

                { L_,       2399,   365,      876218 },
                { L_,       2400,     1,      876219 },
                { L_,       2400,   366,      876584 },
                { L_,       2401,     1,      876585 },

                { L_,       9995,   365,     3650600 },
                { L_,       9996,     1,     3650601 },
                { L_,       9996,   366,     3650966 },
                { L_,       9997,     1,     3650967 },
                { L_,       9999,   365,   Y9999_END },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                const int Y    = DATA[di].d_year;
                const int J    = DATA[di].d_dayOfYear;
                LOOP_ASSERT(LINE, DATA[di].d_exp == Util::yd2serial(Y, J));
            }
        }

        if (verbose) cout << "\n(y, m, d) -> (y, j) -> serial\n"
                          << "\t(in a long loop)" << endl;

        const int first = shortTestFlag ? SHORT_YEAR_START : 0;
        const int last  = shortTestFlag ? SHORT_YEAR_END : 9999;
        for (int y = first; y <= last; ++y) {
            if (verbose && 0 == y % 100 ) {
                cout << "\tExhaustively testing " << y/100 << "00" << " to "
                                                  << y/100 << 99 << endl;
            }

            for (int m = 1; m <= 12; ++m) {
                for (int d = 1; d <= 31; ++d) {
                    if (Util::isValidCalendarDate(y, m, d)) {
                        int j  = Util::ymd2dayOfYear(y, m, d);
                        int s1 = Util::ymd2serial(y, m, d);
                        int s2 = Util::yd2serial(y, j);
                        ASSERT(s1 == s2);
                        if (verbose && 0 == y % 100 && m == 2 && d == 29) {
                            cout << '\t';  P_(s1);  P(s2);
                        }
                    }
                }
            }
        }

      } break;
      case 7: {
        // --------------------------------------------------------------------
        // Testing:
        //   static void yd2md(int *month, int *day, int year, int dayOfYear);
        //   static int yd2month(int year, int dayOfYear);
        //   static int yd2day(int year, int dayOfYear);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'yd2XXX' Convenience Methods"
                          << "\n====================================" << endl;

        if (verbose) cout << "\nTesting: 'yd2md'" << endl;

        {
            static const struct {
                int d_lineNum;    // source line number
                int d_year;       // year under test
                int d_dayOfYear;  // day under test
                int d_expMonth;   // expected value for month
                int d_expDay;     // expected value for day
            } DATA[] = {
                //                          expected  expected
                //line no.  year   Day        month     day
                //-------   -----  -----    --------  -------
                { L_,          1,     1,       1,        1 },
                { L_,          1,   365,      12,       31 },
                { L_,          2,   365,      12,       31 },
                { L_,          3,   365,      12,       31 },
                { L_,          4,   365,      12,       30 },
                { L_,          4,   366,      12,       31 },
                { L_,          5,   365,      12,       31 },

                { L_,       1751,   365,      12,       31 },
                { L_,       1752,   355,      12,       31 },

                { L_,       1980,   365,      12,       30 },
                { L_,       1980,   366,      12,       31 },
                { L_,       1990,   365,      12,       31 },

                { L_,       1996,     1,       1,        1 },
                { L_,       1996,    31,       1,       31 },
                { L_,       1996,    32,       2,        1 },
                { L_,       1996,    59,       2,       28 },
                { L_,       1996,    60,       2,       29 },
                { L_,       1996,    61,       3,        1 },
                { L_,       1996,    91,       3,       31 },
                { L_,       1996,    92,       4,        1 },
                { L_,       1996,   121,       4,       30 },
                { L_,       1996,   122,       5,        1 },
                { L_,       1996,   152,       5,       31 },
                { L_,       1996,   153,       6,        1 },
                { L_,       1996,   182,       6,       30 },
                { L_,       1996,   183,       7,        1 },
                { L_,       1996,   213,       7,       31 },
                { L_,       1996,   214,       8,        1 },
                { L_,       1996,   244,       8,       31 },
                { L_,       1996,   245,       9,        1 },
                { L_,       1996,   274,       9,       30 },
                { L_,       1996,   275,      10,        1 },
                { L_,       1996,   305,      10,       31 },
                { L_,       1996,   306,      11,        1 },
                { L_,       1996,   335,      11,       30 },
                { L_,       1996,   336,      12,        1 },
                { L_,       1996,   366,      12,       31 },

                { L_,       2000,   365,      12,       30 },
                { L_,       2000,   366,      12,       31 },
                { L_,       2001,   365,      12,       31 },
                { L_,       2002,   365,      12,       31 },
                { L_,       2003,   365,      12,       31 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            int m, d;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                Util::yd2md(&m, &d, DATA[di].d_year, DATA[di].d_dayOfYear);
                LOOP_ASSERT(LINE, DATA[di].d_expMonth == m)
                LOOP_ASSERT(LINE, DATA[di].d_expDay   == d)
            }
        }

        if (verbose) cout << "\nTesting: 'yd2month' and 'yd2day'" << endl;

        if (verbose) cout << "\n (y, m, d) -> (y, j) -> (y, m, d)" << endl;

        const int first = shortTestFlag ? SHORT_YEAR_START : 0;
        const int last  = shortTestFlag ? SHORT_YEAR_END : 9999;
        for (int y = first; y <= last; ++y) {
            if (verbose && 0 == y % 100 ) {
                cout << "\tExhaustively testing " << y/100 << "00" << " to "
                                                  << y/100 << 99 << endl;
            }

            for (int m = 1; m <= 12; ++m) {
                for (int d = 1; d <= 31; ++d) {
                    if (Util::isValidCalendarDate(y, m, d)) {
                        int mm, dd;
                        int j     = Util::ymd2dayOfYear(y, m, d);
                        int month = Util::yd2month(y, j);
                        int day   = Util::yd2day  (y, j);
                        Util::yd2md(&mm, &dd, y, j);

                        ASSERT(j != 0);
                        LOOP4_ASSERT(y, j, m, month, m == month);
                        ASSERT(m == mm);
                        LOOP4_ASSERT(y, j, d, day, d == day);
                        ASSERT(d == dd);
                    }
                }
            }
        }

      } break;
      case 6: {
        // --------------------------------------------------------------------
        // Testing:
        //   static int ymd2dayOfYear(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'ymd2dayOfYear'"
                          << "\n====================" << endl;

        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year under test
                int d_month;    // month under test
                int d_day;      // day under test
                int d_exp;      // expected value
            } DATA[] = {
                //line no.  year   month   day     expected value
                //-------   -----  -----  -----    --------------
                { L_,          1,     1,     1,            1 },
                { L_,          1,     1,     2,            2 },
                { L_,          1,     1,    30,           30 },
                { L_,          1,     1,    31,           31 },
                { L_,          1,     2,     1,           32 },
                { L_,          1,     2,    28,           59 },
                { L_,          1,     3,     1,           60 },
                { L_,          1,     3,    31,           90 },
                { L_,          1,     4,     1,           91 },
                { L_,          1,     4,    30,          120 },
                { L_,          1,     5,     1,          121 },
                { L_,          1,     5,    31,          151 },
                { L_,          1,     6,     1,          152 },
                { L_,          1,     6,    30,          181 },
                { L_,          1,     7,     1,          182 },
                { L_,          1,     7,    31,          212 },
                { L_,          1,     8,     1,          213 },
                { L_,          1,     8,    31,          243 },
                { L_,          1,     9,     1,          244 },
                { L_,          1,     9,    30,          273 },
                { L_,          1,    10,     1,          274 },
                { L_,          1,    10,    31,          304 },
                { L_,          1,    11,     1,          305 },
                { L_,          1,    11,    30,          334 },
                { L_,          1,    12,     1,          335 },
                { L_,          1,    12,    31,          365 },

                { L_,          2,     1,     1,            1 },
                { L_,          2,    12,    31,          365 },
                { L_,          3,     1,     1,            1 },
                { L_,          3,    12,    31,          365 },

                { L_,          4,     1,     1,            1 },
                { L_,          4,     1,     2,            2 },
                { L_,          4,     1,    30,           30 },
                { L_,          4,     1,    31,           31 },
                { L_,          4,     2,     1,           32 },
                { L_,          4,     2,    28,           59 },
                { L_,          4,     2,    29,           60 },
                { L_,          4,     3,     1,           61 },
                { L_,          4,     3,    31,           91 },
                { L_,          4,     4,     1,           92 },
                { L_,          4,     4,    30,          121 },
                { L_,          4,     5,     1,          122 },
                { L_,          4,     5,    31,          152 },
                { L_,          4,     6,     1,          153 },
                { L_,          4,     6,    30,          182 },
                { L_,          4,     7,     1,          183 },
                { L_,          4,     7,    31,          213 },
                { L_,          4,     8,     1,          214 },
                { L_,          4,     8,    31,          244 },
                { L_,          4,     9,     1,          245 },
                { L_,          4,     9,    30,          274 },
                { L_,          4,    10,     1,          275 },
                { L_,          4,    10,    31,          305 },
                { L_,          4,    11,     1,          306 },
                { L_,          4,    11,    30,          335 },
                { L_,          4,    12,     1,          336 },
                { L_,          4,    12,    31,          366 },

                { L_,          5,     1,     1,            1 },
                { L_,          5,    12,    31,          365 },

                { L_,       1601,    12,    31,          365 },
                { L_,       1721,    12,    31,          365 },
                { L_,       1749,    12,    31,          365 },
                { L_,       1750,    12,    31,          365 },
                { L_,       1751,    12,    31,          365 },

                { L_,       1752,     1,     1,            1 },
                { L_,       1752,     1,     2,            2 },
                { L_,       1752,     1,    30,           30 },
                { L_,       1752,     1,    31,           31 },
                { L_,       1752,     2,     1,           32 },
                { L_,       1752,     2,    28,           59 },
                { L_,       1752,     2,    29,           60 },
                { L_,       1752,     3,     1,           61 },
                { L_,       1752,     3,    31,           91 },
                { L_,       1752,     4,     1,           92 },
                { L_,       1752,     4,    30,          121 },
                { L_,       1752,     5,     1,          122 },
                { L_,       1752,     5,    31,          152 },
                { L_,       1752,     6,     1,          153 },
                { L_,       1752,     6,    30,          182 },
                { L_,       1752,     7,     1,          183 },
                { L_,       1752,     7,    31,          213 },
                { L_,       1752,     8,     1,          214 },
                { L_,       1752,     8,    31,          244 },
                { L_,       1752,     9,     1,          245 },
                { L_,       1752,     9,     2,          246 },
                { L_,       1752,     9,    14,          247 },
                { L_,       1752,     9,    15,          248 },
                { L_,       1752,     9,    29,          262 },
                { L_,       1752,     9,    30,          263 },
                { L_,       1752,    10,     1,          264 },
                { L_,       1752,    10,    31,          294 },
                { L_,       1752,    11,     1,          295 },
                { L_,       1752,    11,    30,          324 },
                { L_,       1752,    12,     1,          325 },
                { L_,       1752,    12,    31,          355 },

                { L_,       1753,     1,     1,            1 },
                { L_,       1753,     2,    28,           59 },
                { L_,       1753,     3,     1,           60 },
                { L_,       1753,     9,     1,          244 },
                { L_,       1753,     9,    30,          273 },
                { L_,       1753,    12,    31,          365 },

                { L_,       1754,    12,    31,          365 },
                { L_,       1755,    12,    31,          365 },

                { L_,       1756,     1,     1,            1 },
                { L_,       1756,     2,    28,           59 },
                { L_,       1756,     2,    29,           60 },
                { L_,       1756,     3,     1,           61 },
                { L_,       1756,     9,     1,          245 },
                { L_,       1756,     9,    30,          274 },
                { L_,       1756,    12,    31,          366 },

                { L_,       1757,    12,    31,          365 },
                { L_,       1758,    12,    31,          365 },
                { L_,       1759,    12,    31,          365 },
                { L_,       1760,    12,    31,          366 },

                { L_,       1799,    12,    31,          365 },
                { L_,       1800,    12,    31,          365 },
                { L_,       1801,    12,    31,          365 },
                { L_,       1802,    12,    31,          365 },

                { L_,       1899,    12,    31,          365 },
                { L_,       1900,    12,    31,          365 },
                { L_,       1901,    12,    31,          365 },
                { L_,       1902,    12,    31,          365 },

                { L_,       1999,    12,    31,          365 },
                { L_,       2000,    12,    31,          366 },
                { L_,       2001,    12,    31,          365 },
                { L_,       2002,    12,    31,          365 },

                { L_,       2099,    12,    31,          365 },
                { L_,       2100,    12,    31,          365 },
                { L_,       2101,    12,    31,          365 },
                { L_,       2102,    12,    31,          365 },

                { L_,       2399,    12,    31,          365 },
                { L_,       2400,    12,    31,          366 },
                { L_,       2401,    12,    31,          365 },
                { L_,       2402,    12,    31,          365 },

                { L_,       9995,    12,    31,          365 },
                { L_,       9996,    12,    31,          366 },
                { L_,       9997,    12,    31,          365 },
                { L_,       9998,    12,    31,          365 },
                { L_,       9999,    12,    31,          365 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                const int Y    = DATA[di].d_year;
                const int M    = DATA[di].d_month;
                const int D    = DATA[di].d_day;
                LOOP_ASSERT(LINE, DATA[di].d_exp ==
                                                 Util::ymd2dayOfYear(Y, M, D));
            }
        }

      } break;
      case 5: {
        // --------------------------------------------------------------------
        // Testing:
        //   static int ymd2serial(int year, int month, int day);
        //   static int ymd2serialNoCache(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'ymd2serial'"
                          << "\n===================="
                          << "\n(Serial day #1 is 1/1/1)" << endl;

        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year under test
                int d_month;    // month under test
                int d_day;      // day under test
                int d_exp;      // expected value
            } DATA[] = {
                //line no.  year   month   day     expected value
                //-------   -----  -----  -----    --------------
                { L_,          1,     1,     1,            1 },
                { L_,          1,     1,     2,            2 },
                { L_,          1,     1,    30,           30 },
                { L_,          1,     1,    31,           31 },
                { L_,          1,     2,     1,           32 },
                { L_,          1,     2,    28,           59 },
                { L_,          1,     3,     1,           60 },
                { L_,          1,     3,    31,           90 },
                { L_,          1,     4,     1,           91 },
                { L_,          1,     4,    30,          120 },
                { L_,          1,     5,     1,          121 },
                { L_,          1,     5,    31,          151 },
                { L_,          1,     6,     1,          152 },
                { L_,          1,     6,    30,          181 },
                { L_,          1,     7,     1,          182 },
                { L_,          1,     7,    31,          212 },
                { L_,          1,     8,     1,          213 },
                { L_,          1,     8,    31,          243 },
                { L_,          1,     9,     1,          244 },
                { L_,          1,     9,    30,          273 },
                { L_,          1,    10,     1,          274 },
                { L_,          1,    10,    31,          304 },
                { L_,          1,    11,     1,          305 },
                { L_,          1,    11,    30,          334 },
                { L_,          1,    12,     1,          335 },
                { L_,          1,    12,    31,          365 },

                { L_,          2,     1,     1,          366 },
                { L_,          3,     1,     1,          731 },

                { L_,          4,     1,     1,         1096 },
                { L_,          4,     1,     2,         1097 },
                { L_,          4,     1,    30,         1125 },
                { L_,          4,     1,    31,         1126 },
                { L_,          4,     2,     1,         1127 },
                { L_,          4,     2,    28,         1154 },
                { L_,          4,     2,    29,         1155 },
                { L_,          4,     3,     1,         1156 },
                { L_,          4,     3,    31,         1186 },
                { L_,          4,     4,     1,         1187 },
                { L_,          4,     4,    30,         1216 },
                { L_,          4,     5,     1,         1217 },
                { L_,          4,     5,    31,         1247 },
                { L_,          4,     6,     1,         1248 },
                { L_,          4,     6,    30,         1277 },
                { L_,          4,     7,     1,         1278 },
                { L_,          4,     7,    31,         1308 },
                { L_,          4,     8,     1,         1309 },
                { L_,          4,     8,    31,         1339 },
                { L_,          4,     9,     1,         1340 },
                { L_,          4,     9,    30,         1369 },
                { L_,          4,    10,     1,         1370 },
                { L_,          4,    10,    31,         1400 },
                { L_,          4,    11,     1,         1401 },
                { L_,          4,    11,    30,         1430 },
                { L_,          4,    12,     1,         1431 },
                { L_,          4,    12,    31,         1461 },

                { L_,          5,     1,     1,         1462 },

                { L_,       1601,     1,     1,       584401 },
                { L_,       1721,     1,     1,       628231 },
                { L_,       1749,     1,     1,       638458 },
                { L_,       1750,     1,     1,       638823 },
                { L_,       1751,     1,     1,       639188 },

                { L_,       1752,     1,     1,       639553 },
                { L_,       1752,     1,     2,       639554 },
                { L_,       1752,     1,    30,       639582 },
                { L_,       1752,     1,    31,       639583 },
                { L_,       1752,     2,     1,       639584 },
                { L_,       1752,     2,    28,       639611 },
                { L_,       1752,     2,    29,       639612 },
                { L_,       1752,     3,     1,       639613 },
                { L_,       1752,     3,    31,       639643 },
                { L_,       1752,     4,     1,       639644 },
                { L_,       1752,     4,    30,       639673 },
                { L_,       1752,     5,     1,       639674 },
                { L_,       1752,     5,    31,       639704 },
                { L_,       1752,     6,     1,       639705 },
                { L_,       1752,     6,    30,       639734 },
                { L_,       1752,     7,     1,       639735 },
                { L_,       1752,     7,    31,       639765 },
                { L_,       1752,     8,     1,       639766 },
                { L_,       1752,     8,    31,       639796 },
                { L_,       1752,     9,     1,       639797 },
                { L_,       1752,     9,     2,       639798 },
                { L_,       1752,     9,    14,       639799 },
                { L_,       1752,     9,    15,       639800 },
                { L_,       1752,     9,    29,       639814 },
                { L_,       1752,     9,    30,       639815 },
                { L_,       1752,    10,     1,       639816 },
                { L_,       1752,    10,    31,       639846 },
                { L_,       1752,    11,     1,       639847 },
                { L_,       1752,    11,    30,       639876 },
                { L_,       1752,    12,     1,       639877 },
                { L_,       1752,    12,    31,       639907 },

                { L_,       1753,     1,     1,       639908 },
                { L_,       1753,     1,    31,       639938 },
                { L_,       1753,     2,     1,       639939 },
                { L_,       1753,     2,    28,       639966 },
                { L_,       1753,     3,     1,       639967 },
                { L_,       1753,     3,    31,       639997 },
                { L_,       1753,     4,     1,       639998 },
                { L_,       1753,     4,    30,       640027 },
                { L_,       1753,     5,     1,       640028 },
                { L_,       1753,     5,    31,       640058 },
                { L_,       1753,     6,     1,       640059 },
                { L_,       1753,     6,    30,       640088 },
                { L_,       1753,     7,     1,       640089 },
                { L_,       1753,     7,    31,       640119 },
                { L_,       1753,     8,     1,       640120 },
                { L_,       1753,     8,    31,       640150 },
                { L_,       1753,     9,     1,       640151 },
                { L_,       1753,     9,    30,       640180 },
                { L_,       1753,    10,     1,       640181 },
                { L_,       1753,    10,    31,       640211 },
                { L_,       1753,    11,     1,       640212 },
                { L_,       1753,    11,    30,       640241 },
                { L_,       1753,    12,     1,       640242 },
                { L_,       1753,    12,    31,       640272 },

                { L_,       1754,     1,     1,       640273 },
                { L_,       1755,     1,     1,       640638 },

                { L_,       1756,     1,     1,       641003 },
                { L_,       1756,     1,    31,       641033 },
                { L_,       1756,     2,     1,       641034 },
                { L_,       1756,     2,    28,       641061 },
                { L_,       1756,     2,    29,       641062 },
                { L_,       1756,     3,     1,       641063 },
                { L_,       1756,     3,    31,       641093 },
                { L_,       1756,     4,     1,       641094 },
                { L_,       1756,     4,    30,       641123 },
                { L_,       1756,     5,     1,       641124 },
                { L_,       1756,     5,    31,       641154 },
                { L_,       1756,     6,     1,       641155 },
                { L_,       1756,     6,    30,       641184 },
                { L_,       1756,     7,     1,       641185 },
                { L_,       1756,     7,    31,       641215 },
                { L_,       1756,     8,     1,       641216 },
                { L_,       1756,     8,    31,       641246 },
                { L_,       1756,     9,     1,       641247 },
                { L_,       1756,     9,    30,       641276 },
                { L_,       1756,    10,     1,       641277 },
                { L_,       1756,    10,    31,       641307 },
                { L_,       1756,    11,     1,       641308 },
                { L_,       1756,    11,    30,       641337 },
                { L_,       1756,    12,     1,       641338 },
                { L_,       1756,    12,    31,       641368 },

                { L_,       1757,     1,     1,       641369 },
                { L_,       1757,    12,    31,       641733 },
                { L_,       1758,     1,     1,       641734 },
                { L_,       1758,    12,    31,       642098 },
                { L_,       1759,     1,     1,       642099 },
                { L_,       1759,    12,    31,       642463 },
                { L_,       1760,     1,     1,       642464 },
                { L_,       1760,    12,    31,       642829 },
                { L_,       1761,     1,     1,       642830 },

                { L_,       1799,    12,    31,       657073 },
                { L_,       1800,     1,     1,       657074 },
                { L_,       1800,    12,    31,       657438 },
                { L_,       1801,     1,     1,       657439 },
                { L_,       1801,    12,    31,       657803 },
                { L_,       1802,     1,     1,       657804 },

                { L_,       1899,    12,    31,       693597 },
                { L_,       1900,     1,     1,       693598 },
                { L_,       1900,    12,    31,       693962 },
                { L_,       1901,     1,     1,       693963 },
                { L_,       1901,    12,    31,       694327 },
                { L_,       1902,     1,     1,       694328 },

                { L_,       1999,    12,    31,       730121 },
                { L_,       2000,     1,     1,       730122 },
                { L_,       2000,    12,    31,       730487 },
                { L_,       2001,     1,     1,       730488 },
                { L_,       2001,    12,    31,       730852 },
                { L_,       2002,     1,     1,       730853 },

                { L_,       2099,    12,    31,       766646 },
                { L_,       2100,     1,     1,       766647 },
                { L_,       2100,    12,    31,       767011 },
                { L_,       2101,     1,     1,       767012 },
                { L_,       2101,    12,    31,       767376 },
                { L_,       2102,     1,     1,       767377 },

                { L_,       2399,    12,    31,       876218 },
                { L_,       2400,     1,     1,       876219 },
                { L_,       2400,    12,    31,       876584 },
                { L_,       2401,     1,     1,       876585 },
                { L_,       2401,    12,    31,       876949 },
                { L_,       2402,     1,     1,       876950 },

                { L_,       9995,     1,     1,      3650236 },
                { L_,       9995,    12,    31,      3650600 },
                { L_,       9996,     1,     1,      3650601 },
                { L_,       9996,    12,    31,      3650966 },
                { L_,       9997,     1,     1,      3650967 },
                { L_,       9997,    12,    31,      3651331 },
                { L_,       9998,     1,     1,      3651332 },
                { L_,       9998,    12,    31,      3651696 },
                { L_,       9999,     1,     1,      3651697 },
                { L_,       9999,    12,    31,    Y9999_END },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                const int Y    = DATA[di].d_year;
                const int M    = DATA[di].d_month;
                const int D    = DATA[di].d_day;
                const int EXP  = DATA[di].d_exp;
                const int RES1 = Util::ymd2serial(Y, M, D);
                const int RES2 = Util::ymd2serialNoCache(Y, M, D);
                LOOP3_ASSERT(LINE, EXP, RES1, EXP == RES1);
                LOOP3_ASSERT(LINE, EXP, RES2, EXP == RES2);
            }
        }

        if (veryVerbose) {
            cout << "\nTesting: 'ymd2serial' vs 'ymd2serialNoCache'" << endl;

            for (int year  = 1; year  <= 9999; ++year)
            for (int month = 1; month <= 12;   ++month)
            for (int day   = 1; day   <= 31;   ++day) {
                if (Util::isValidCalendarDate(year, month, day)) {
                    int x1 = Util::ymd2serial(year, month, day);
                    int x2 = Util::ymd2serialNoCache(year, month,
                                                                day);
                    LOOP3_ASSERT(year, month, day, x1 == x2);
                    if (year % 100 == 0 && month == 2 && day == 29) {
                        P_(x1); P(x2);
                    }
                }
            }
        }

      } break;
      case 4: {
        // --------------------------------------------------------------------
        // Testing:
        //   static bool isValidCalendarDate(int year, int month, int day);
        //   static bool isValidYearDayDate(int year, int dayOfYear);
        //   static bool isValidserialDate(int serialDay);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'isValidXXX' Methods"
                          << "\n============================" << endl;

        if (verbose)
            cout << "\nTesting: 'isValidCalendarDate' "
                 << "and 'isValidCalendarDateNoCache'"
                 << endl;

        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year under test
                int d_month;    // month under test
                int d_day;      // day under test
                int d_exp;      // expected value
            } DATA[] = {
                //line no.  year   month   day     expected value
                //-------   -----  -----  -----    --------------
                { L_,          0,     0,     0,      0 },
                { L_,          1,     1,     0,      0 },
                { L_,          1,     0,     1,      0 },
                { L_,          0,     1,     1,      0 },
                { L_,          1,     1,    -1,      0 },
                { L_,          1,    -1,     1,      0 },
                { L_,         -1,     1,     1,      0 },

                { L_,          0,    12,    31,      0 }, // last invalid date
                { L_,          1,     1,     1,      1 }, // first valid date
                { L_,       9999,    12,    31,      1 }, // last valid date
                { L_,      10000,     1,     1,      0 }, // 1st invalid date

                { L_,          1,     1,    31,      1 },
                { L_,          2,     2,    28,      1 },
                { L_,          4,     2,    29,      1 },

                { L_,       1600,     2,    29,      1 },
                { L_,       1700,     2,    29,      1 },
                { L_,       1800,     2,    29,      0 },
                { L_,       1900,     2,    29,      0 },
                { L_,       2000,     2,    29,      1 },
                { L_,       2100,     2,    29,      0 },

                { L_,       1752,     2,    28,      1 },
                { L_,       1752,     2,    29,      1 },
                { L_,       1752,     2,    30,      0 },

                { L_,       1752,     9,     1,      1 },
                { L_,       1752,     9,     2,      1 },
                { L_,       1752,     9,     3,      0 },
                { L_,       1752,     9,     4,      0 },
                { L_,       1752,     9,    12,      0 },
                { L_,       1752,     9,    13,      0 },
                { L_,       1752,     9,    14,      1 },
                { L_,       1752,     9,    15,      1 },
                { L_,       1752,     9,    30,      1 },
                { L_,       1752,     9,    31,      0 },

                { L_,       1756,     9,     2,      1 },
                { L_,       1756,     9,     3,      1 },
                { L_,       1756,     9,     4,      1 },
                { L_,       1756,     9,    12,      1 },
                { L_,       1756,     9,    13,      1 },
                { L_,       1756,     9,    14,      1 },

                { L_,       1996,     1,     1,      1 },
                { L_,       1996,     2,     1,      1 },
                { L_,       1996,     3,     1,      1 },
                { L_,       1996,     4,     1,      1 },
                { L_,       1996,     5,     1,      1 },
                { L_,       1996,     6,     1,      1 },
                { L_,       1996,     7,     1,      1 },
                { L_,       1996,     8,     1,      1 },
                { L_,       1996,     9,     1,      1 },
                { L_,       1996,    10,     1,      1 },
                { L_,       1996,    11,     1,      1 },
                { L_,       1996,    12,     1,      1 },
                { L_,       1996,    13,     1,      0 },

                { L_,       1997,     1,     1,      1 },
                { L_,       1997,     2,     1,      1 },
                { L_,       1997,     3,     1,      1 },
                { L_,       1997,     4,     1,      1 },
                { L_,       1997,     5,     1,      1 },
                { L_,       1997,     6,     1,      1 },
                { L_,       1997,     7,     1,      1 },
                { L_,       1997,     8,     1,      1 },
                { L_,       1997,     9,     1,      1 },
                { L_,       1997,    10,     1,      1 },
                { L_,       1997,    11,     1,      1 },
                { L_,       1997,    12,     1,      1 },
                { L_,       1997,    13,     1,      0 },

                { L_,       1900,     1,    30,      1 },
                { L_,       1900,     1,    31,      1 },
                { L_,       1900,     1,    32,      0 },

                { L_,       1900,     2,    28,      1 },
                { L_,       1900,     2,    29,      0 },
                { L_,       1900,     2,    30,      0 },

                { L_,       1900,     3,    30,      1 },
                { L_,       1900,     3,    31,      1 },
                { L_,       1900,     3,    32,      0 },

                { L_,       1900,     4,    30,      1 },
                { L_,       1900,     4,    31,      0 },
                { L_,       1900,     4,    32,      0 },

                { L_,       1900,     5,    30,      1 },
                { L_,       1900,     5,    31,      1 },
                { L_,       1900,     5,    32,      0 },

                { L_,       1900,     6,    30,      1 },
                { L_,       1900,     6,    31,      0 },
                { L_,       1900,     6,    32,      0 },

                { L_,       1900,     7,    30,      1 },
                { L_,       1900,     7,    31,      1 },
                { L_,       1900,     7,    32,      0 },

                { L_,       1900,     8,    30,      1 },
                { L_,       1900,     8,    31,      1 },
                { L_,       1900,     8,    32,      0 },

                { L_,       1900,     9,    30,      1 },
                { L_,       1900,     9,    31,      0 },
                { L_,       1900,     9,    32,      0 },

                { L_,       1900,    10,    30,      1 },
                { L_,       1900,    10,    31,      1 },
                { L_,       1900,    10,    32,      0 },

                { L_,       1900,    11,    30,      1 },
                { L_,       1900,    11,    31,      0 },
                { L_,       1900,    11,    32,      0 },

                { L_,       1900,    12,    30,      1 },
                { L_,       1900,    12,    31,      1 },
                { L_,       1900,    12,    32,      0 },

                { L_,       2000,     1,    30,      1 },
                { L_,       2000,     1,    31,      1 },
                { L_,       2000,     1,    32,      0 },

                { L_,       2000,     2,    28,      1 },
                { L_,       2000,     2,    29,      1 },
                { L_,       2000,     2,    30,      0 },

                { L_,       2000,     3,    30,      1 },
                { L_,       2000,     3,    31,      1 },
                { L_,       2000,     3,    32,      0 },

                { L_,       2000,     4,    30,      1 },
                { L_,       2000,     4,    31,      0 },
                { L_,       2000,     4,    32,      0 },

                { L_,       2000,     5,    30,      1 },
                { L_,       2000,     5,    31,      1 },
                { L_,       2000,     5,    32,      0 },

                { L_,       2000,     6,    30,      1 },
                { L_,       2000,     6,    31,      0 },
                { L_,       2000,     6,    32,      0 },

                { L_,       2000,     7,    30,      1 },
                { L_,       2000,     7,    31,      1 },
                { L_,       2000,     7,    32,      0 },

                { L_,       2000,     8,    30,      1 },
                { L_,       2000,     8,    31,      1 },
                { L_,       2000,     8,    32,      0 },

                { L_,       2000,     9,    30,      1 },
                { L_,       2000,     9,    31,      0 },
                { L_,       2000,     9,    32,      0 },

                { L_,       2000,    10,    30,      1 },
                { L_,       2000,    10,    31,      1 },
                { L_,       2000,    10,    32,      0 },

                { L_,       2000,    11,    30,      1 },
                { L_,       2000,    11,    31,      0 },
                { L_,       2000,    11,    32,      0 },

                { L_,       2000,    12,    30,      1 },
                { L_,       2000,    12,    31,      1 },
                { L_,       2000,    12,    32,      0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                LOOP_ASSERT(LINE, DATA[di].d_exp ==
                                  Util::isValidCalendarDate(DATA[di].d_year,
                                                    DATA[di].d_month,
                                                    DATA[di].d_day));
                LOOP_ASSERT(LINE, DATA[di].d_exp ==
                            Util::isValidCalendarDateNoCache(DATA[di].d_year,
                                                           DATA[di].d_month,
                                                           DATA[di].d_day));
            }
        }

        if (veryVerbose) {
            cout << "\nTesting: 'isValidCalendarDate' "
                 << "vs 'isValidCalendarDateNoCache'"
                 << endl;

            for (int year  = 0; year  <= 10000; ++year)
            for (int month = 0; month <= 13;   ++month)
            for (int day   = 0; day   <= 32;   ++day) {
                int x1 = Util::isValidCalendarDate(year, month, day);
                int x2 = Util::isValidCalendarDateNoCache(year, month,day);
                LOOP3_ASSERT(year, month, day, x1 == x2);
                if (year % 100 == 0 && month == 2 && day == 29) {
                    P_(x1); P(x2);
                }
            }
        }

        if (verbose) cout << "\nTesting: 'isValidYearDayDate'" << endl;

        {
            static const struct {
                int d_lineNum;    // source line number
                int d_year;       // year under test
                int d_dayOfYear;  // day under test
                int d_exp;        // expected value
            } DATA[] = {
                //line no.  year      day       expected value
                //-------   -----  ----------   --------------
                { L_,          0,      0,           0 },
                { L_,          1,      0,           0 },
                { L_,          0,      1,           0 },
                { L_,          1,      1,           1 },
                { L_,          1,     -1,           0 },
                { L_,         -1,      1,           0 },

                { L_,          0,    366,           0 }, // last invalid date
                { L_,          1,      1,           1 }, // first valid date
                { L_,       9999,    365,           1 }, // last valid date
                { L_,       9999,    366,           0 }, // first invalid date
                { L_,      10000,    366,           0 }, // first invalid date

                { L_,          1,    365,           1 },
                { L_,          1,    366,           0 },
                { L_,          2,    365,           1 },
                { L_,          2,    366,           0 },
                { L_,          3,    365,           1 },
                { L_,          3,    366,           0 },
                { L_,          4,    365,           1 },
                { L_,          4,    366,           1 },
                { L_,          5,    365,           1 },
                { L_,          5,    366,           0 },

                { L_,       1752,    355,           1 },
                { L_,       1752,    356,           0 },
                { L_,       1753,    365,           1 },
                { L_,       1753,    366,           0 },
                { L_,       1754,    365,           1 },
                { L_,       1754,    366,           0 },
                { L_,       1755,    365,           1 },
                { L_,       1755,    366,           0 },
                { L_,       1756,    366,           1 },
                { L_,       1756,    367,           0 },

                { L_,       1899,    365,           1 },
                { L_,       1899,    366,           0 },
                { L_,       1900,    365,           1 },
                { L_,       1900,    366,           0 },
                { L_,       1901,    365,           1 },
                { L_,       1901,    366,           0 },
                { L_,       1902,    365,           1 },
                { L_,       1902,    366,           0 },
                { L_,       1903,    365,           1 },
                { L_,       1903,    366,           0 },
                { L_,       1904,    366,           1 },
                { L_,       1904,    367,           0 },
                { L_,       1905,    365,           1 },
                { L_,       1905,    366,           0 },

                { L_,       1999,    365,           1 },
                { L_,       1999,    366,           0 },
                { L_,       2000,    366,           1 },
                { L_,       2000,    367,           0 },
                { L_,       2001,    365,           1 },
                { L_,       2001,    366,           0 },
                { L_,       2002,    365,           1 },
                { L_,       2002,    366,           0 },
                { L_,       2003,    365,           1 },
                { L_,       2003,    366,           0 },
                { L_,       2004,    366,           1 },
                { L_,       2004,    367,           0 },
                { L_,       2005,    365,           1 },
                { L_,       2005,    366,           0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                const int Y    = DATA[di].d_year;
                const int J    = DATA[di].d_dayOfYear;
                LOOP_ASSERT(LINE, DATA[di].d_exp ==
                                  Util::isValidYearDayDate(Y, J));
            }
        }

        if (verbose) cout << "\tchecking years 1-9999 in double loop" << endl;

        int y, j; // loop indices

        int first = shortTestFlag ? SHORT_YEAR_START : 1;
        int last  = shortTestFlag ? SHORT_YEAR_END : 9999;
        for (y = first; y <= last; ++y) {
            ASSERT(0 == Util::isValidYearDayDate(y, -y));
            ASSERT(0 == Util::isValidYearDayDate(y, 0));
            for (j = 1; j <= 355; ++j) {
                ASSERT(1 == Util::isValidYearDayDate(y, j));
            }
            const bool isNotY1752 = 1752 != y;

            for (; j <= 365; ++j) {
                ASSERT(isNotY1752 == Util::isValidYearDayDate(y,j));
            }

            const bool isLeapYear = Util::isLeapYear(y);
            const bool isLongYear = isLeapYear && isNotY1752;

            ASSERT(isLongYear == Util::isValidYearDayDate(y, j));

            ASSERT(0 == Util::isValidYearDayDate(y, 367));
        }

        if (verbose) cout << "\nTesting: 'isValidSerialDate'" << endl;
        {
            int s;
            for (s = -1500; s <= 0; ++s) {
                ASSERT(0 == Util::isValidSerialDate(s));
            }

            first = shortTestFlag ? SHORT_SERIAL_START : 1;
            last  = shortTestFlag ? SHORT_SERIAL_END : Y9999_END;
            for (s = first; s <= last; ++s) {
                ASSERT(1 == Util::isValidSerialDate(s));
            }

            for (s = Y9999_END+1; s <= Y9999_END + 1500; ++s) {
                ASSERT(0 == Util::isValidSerialDate(s));
            }

            static const struct {
                int d_lineNum;  // source line number
                int d_serial;   // serial date
                int d_exp;      // expected value
            } DATA[] = {
                //line no.    serial        expected value
                //-------   -----------     --------------
                { L_,                 0,    0              },
                { L_,                 1,    1              },
                { L_,                 2,    1              },

                { L_,       Y9999_END-2,    1              },
                { L_,       Y9999_END-1,    1              },
                { L_,       Y9999_END  ,    1              },
                { L_,       Y9999_END+1,    0              },
                { L_,       Y9999_END+2,    0              },

                { L_,         INT_MAX-2,    0              },
                { L_,         INT_MAX-1,    0              },
                { L_,         INT_MAX  ,    0              },

                { L_,                -1,    0              },
                { L_,                -2,    0              },

                { L_,         INT_MIN+2,    0              },
                { L_,         INT_MIN+1,    0              },
                { L_,         INT_MIN  ,    0              },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                const int S    = DATA[di].d_serial;
                LOOP_ASSERT(LINE, DATA[di].d_exp ==
                                  Util::isValidSerialDate(S));
            }
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Testing:
        //   static int lastDayOfMonth(int year, int month);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'lastDayOfMonth'"
                          << "\n========================" << endl;

        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year under test
                int d_month;    // month under test
                int d_exp;      // expected value
            } DATA[] = {
                //line no.  year   month    expected value
                //--------  -----  -----    --------------
                { L_,       1999,     1,       31 },
                { L_,       1999,     2,       28 },
                { L_,       1999,     3,       31 },
                { L_,       1999,     4,       30 },
                { L_,       1999,     5,       31 },
                { L_,       1999,     6,       30 },
                { L_,       1999,     7,       31 },
                { L_,       1999,     8,       31 },
                { L_,       1999,     9,       30 },
                { L_,       1999,    10,       31 },
                { L_,       1999,    11,       30 },
                { L_,       1999,    12,       31 },

                { L_,       2000,     1,       31 },
                { L_,       2000,     2,       29 },
                { L_,       2000,     3,       31 },
                { L_,       2000,     4,       30 },
                { L_,       2000,     5,       31 },
                { L_,       2000,     6,       30 },
                { L_,       2000,     7,       31 },
                { L_,       2000,     8,       31 },
                { L_,       2000,     9,       30 },
                { L_,       2000,    10,       31 },
                { L_,       2000,    11,       30 },
                { L_,       2000,    12,       31 },

                { L_,       2001,     1,       31 },
                { L_,       2001,     2,       28 },
                { L_,       2001,     3,       31 },
                { L_,       2001,     4,       30 },
                { L_,       2001,     5,       31 },
                { L_,       2001,     6,       30 },
                { L_,       2001,     7,       31 },
                { L_,       2001,     8,       31 },
                { L_,       2001,     9,       30 },
                { L_,       2001,    10,       31 },
                { L_,       2001,    11,       30 },
                { L_,       2001,    12,       31 },

                { L_,       2096,     1,       31 },
                { L_,       2096,     2,       29 },
                { L_,       2096,     3,       31 },
                { L_,       2096,     4,       30 },
                { L_,       2096,     5,       31 },
                { L_,       2096,     6,       30 },
                { L_,       2096,     7,       31 },
                { L_,       2096,     8,       31 },
                { L_,       2096,     9,       30 },
                { L_,       2096,    10,       31 },
                { L_,       2096,    11,       30 },
                { L_,       2096,    12,       31 },

                { L_,       2100,     1,       31 },
                { L_,       2100,     2,       28 },
                { L_,       2100,     3,       31 },
                { L_,       2100,     4,       30 },
                { L_,       2100,     5,       31 },
                { L_,       2100,     6,       30 },
                { L_,       2100,     7,       31 },
                { L_,       2100,     8,       31 },
                { L_,       2100,     9,       30 },
                { L_,       2100,    10,       31 },
                { L_,       2100,    11,       30 },
                { L_,       2100,    12,       31 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                LOOP_ASSERT(LINE, DATA[di].d_exp ==
                                  Util::lastDayOfMonth(DATA[di].d_year,
                                                       DATA[di].d_month));
            }
        }

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // Testing:
        //   static int numLeapYears(int year1, int year2);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'numLeapYears'"
                          << "\n======================" << endl;

        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year1;     // 1st year in range under test
                int d_year2;     // 2nd year in range under test
                int d_exp;      // expected value
            } DATA[] = {
                //line #   year1   year2    expected value
                //------   ------  ------   --------------
                { L_,          1,      1,        0 },
                { L_,          1,      2,        0 },
                { L_,          1,      3,        0 },
                { L_,          1,      4,        1 },
                { L_,          1,      5,        1 },
                { L_,          1,      8,        2 },
                { L_,          1,     99,       24 },
                { L_,          1,    100,       25 },
                { L_,          1,    101,       25 },
                { L_,          1,    399,       99 },
                { L_,          1,    400,      100 },
                { L_,          1,    401,      100 },
                { L_,          1,   1700,      425 },
                { L_,          1,   1747,      436 },
                { L_,          1,   1748,      437 },
                { L_,          1,   1749,      437 },
                { L_,          1,   1750,      437 },
                { L_,          1,   1751,      437 },
                { L_,          1,   1752,      438 },
                { L_,          1,   1753,      438 },
                { L_,          1,   1754,      438 },
                { L_,          1,   1755,      438 },
                { L_,          1,   1756,      439 },
                { L_,          1,   1757,      439 },
                { L_,          1,   1796,      449 },
                { L_,          1,   1799,      449 },
                { L_,          1,   1800,      449 },
                { L_,          1,   1801,      449 },
                { L_,          1,   1804,      450 },
                { L_,          1,   1899,      473 },
                { L_,          1,   1900,      473 },
                { L_,          1,   1901,      473 },
                { L_,          1,   1995,      496 },
                { L_,          1,   1996,      497 },
                { L_,          1,   1997,      497 },
                { L_,          1,   1998,      497 },
                { L_,          1,   1999,      497 },
                { L_,          1,   2000,      498 },
                { L_,          1,   2001,      498 },
                { L_,          1,   2002,      498 },
                { L_,          1,   2003,      498 },
                { L_,          1,   2004,      499 },
                { L_,          1,   2005,      499 },
                { L_,          1,   2099,      522 },
                { L_,          1,   2100,      522 },
                { L_,          1,   2101,      522 },
                { L_,          1,   2399,      594 },
                { L_,          1,   2400,      595 },
                { L_,          1,   2401,      595 },
                { L_,          1,   6399,     1564 },
                { L_,          1,   6400,     1565 },
                { L_,          1,   6401,     1565 },
                { L_,          1,   9995,     2436 },
                { L_,          1,   9996,     2437 },
                { L_,          1,   9997,     2437 },
                { L_,          1,   9999,     2437 },

                { L_,       1800,   1800,        0 },
                { L_,       1800,   1801,        0 },
                { L_,       1800,   1999,       48 },
                { L_,       1800,   2000,       49 },
                { L_,       1800,   2001,       49 },
                { L_,       1800,   9999,     1988 },

                { L_,       1999,   1999,        0 },
                { L_,       1999,   2000,        1 },
                { L_,       1999,   2001,        1 },
                { L_,       1999,   9999,     1940 },

                { L_,       2000,   2000,        1 },
                { L_,       2000,   2001,        1 },
                { L_,       2000,   9999,     1940 },

                { L_,       2001,   2001,        0 },
                { L_,       2001,   9999,     1939 },

                { L_,       9999,   9999,        0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                LOOP_ASSERT(LINE, DATA[di].d_exp ==
                                  Util::numLeapYears(DATA[di].d_year1,
                                                     DATA[di].d_year2));
            }
        }

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // Testing:
        //   static bool isLeapYear(int year);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'isLeapYear'"
                          << "\n====================" << endl;

        {
            static const struct {
                int d_lineNum;  // source line number
                int d_year;     // year under test
                int d_exp;      // expected value
            } DATA[] = {
                //line no.   year    expected value
                //-------   ------   --------------
                { L_,          1,              0 },
                { L_,          2,              0 },
                { L_,          3,              0 },
                { L_,          4,              1 },
                { L_,          5,              0 },
                { L_,          8,              1 },
                { L_,         99,              0 },
                { L_,        100,              1 },
                { L_,        101,              0 },
                { L_,        399,              0 },
                { L_,        400,              1 },
                { L_,        401,              0 },
                { L_,       1600,              1 },
                { L_,       1700,              1 },
                { L_,       1747,              0 },
                { L_,       1748,              1 },
                { L_,       1749,              0 },
                { L_,       1750,              0 },
                { L_,       1751,              0 },
                { L_,       1752,              1 },
                { L_,       1753,              0 },
                { L_,       1754,              0 },
                { L_,       1755,              0 },
                { L_,       1756,              1 },
                { L_,       1757,              0 },
                { L_,       1796,              1 },
                { L_,       1799,              0 },
                { L_,       1800,              0 },
                { L_,       1801,              0 },
                { L_,       1804,              1 },
                { L_,       1899,              0 },
                { L_,       1900,              0 },
                { L_,       1901,              0 },
                { L_,       1995,              0 },
                { L_,       1996,              1 },
                { L_,       1997,              0 },
                { L_,       1998,              0 },
                { L_,       1999,              0 },
                { L_,       2000,              1 },
                { L_,       2001,              0 },
                { L_,       2002,              0 },
                { L_,       2003,              0 },
                { L_,       2004,              1 },
                { L_,       2005,              0 },
                { L_,       2099,              0 },
                { L_,       2100,              0 },
                { L_,       2101,              0 },
                { L_,       2399,              0 },
                { L_,       2400,              1 },
                { L_,       2401,              0 },
                { L_,       9995,              0 },
                { L_,       9996,              1 },
                { L_,       9997,              0 },
                { L_,       9998,              0 },
                { L_,       9999,              0 },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                LOOP_ASSERT(LINE, DATA[di].d_exp ==
                                  Util::isLeapYear(DATA[di].d_year));
            }
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
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
