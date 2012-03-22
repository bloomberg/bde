// bdeimp_prolepticdateutil.t.cpp                                     -*-C++-*-

#include <bdeimp_prolepticdateutil.h>

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
// [ 2]  static int  isValidCalendarDate(int year, int month, int day);
// [ 3]  static int  isValidSerialDate(int serialDay);
// [ 4]  static int  ymd2serial(int year, int month, int day);
// [ 5]  static void serial2ymd(int *year, int *month, int *day, int serialDay)
//-----------------------------------------------------------------------------
// [ 7] USAGE
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
typedef bdeimp_ProlepticDateUtil Util;

namespace TestUtil {

static const int db[2][13] =
    {{-1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364},
     {-1, 30, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365}};

static const unsigned char mb[2][366] =
{
{
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2,
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3,
4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
4, 4, 4, 4,
5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
5, 5, 5, 5, 5,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8,
9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
9, 9, 9, 9,
10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
10,10,10,10,10,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
12,12,12,12,12
},
{
1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
1, 1, 1, 1, 1,
2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
2, 2, 2,
3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3,
4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
4, 4, 4, 4,
5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
5, 5, 5, 5, 5,
6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
6, 6, 6, 6,
7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
7, 7, 7, 7, 7,
8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
8, 8, 8, 8, 8,
9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
9, 9, 9, 9,
10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
10,10,10,10,10,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,
12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,
12,12,12,12,12
}
};

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

void serialToYmd(int *year,
                 int *month,
                 int *day,
                 unsigned int serialDate)
{
    BSLS_ASSERT(year);
    BSLS_ASSERT(month);
    BSLS_ASSERT(day);

    int y = (serialDate + 2) / 365;
    int z = (serialDate + 2) % 365 * 400 - 97 * y;

    if (z < 0)
        y += (z - 146096) / 146097;

    const unsigned int ym1 = y - 1;
    int doy = serialDate - ((unsigned)y*365
                          + (unsigned)y/4 - (unsigned)y/100 + (unsigned)y/400);
    const int doy1 = serialDate - (ym1*365 + ym1/4 - ym1/100 + ym1/400);
    const int N = std::numeric_limits<int>::digits - 1;

    // arithmetic rshift - not portable - but nearly universal

    const int mask1 = doy >> N;

    const int mask0 = ~mask1;
    doy = (doy & mask0) | (doy1 & mask1);
    y = (y & mask0) | (ym1 & mask1);
    y -= 32799;
    const bool leap = isLeap(y);

    *year  = y;
    *month = mb[leap][doy];
    *day   = doy - db[leap][*month-1];
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
        return bdeimp_ProlepticDateUtil::isValidCalendarDate(year, month, day);
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
        BSLS_ASSERT_SAFE(bdeimp_ProlepticDateUtil::isValidSerialDate(
                                                                d_serialDate));
        return *this;
    }

    inline
    MyDate& MyDate::operator--()
    {
        --d_serialDate;
        BSLS_ASSERT_SAFE(bdeimp_ProlepticDateUtil::isValidSerialDate(
                                                                d_serialDate));
        return *this;
    }

    inline
    MyDate& MyDate::operator+=(int numDays)
    {
        d_serialDate += numDays;
        BSLS_ASSERT_SAFE(bdeimp_ProlepticDateUtil::isValidSerialDate(
                                                                d_serialDate));
        return *this;
    }

    inline
    MyDate& MyDate::operator-=(int numDays)
    {
        d_serialDate -= numDays;
        BSLS_ASSERT_SAFE(bdeimp_ProlepticDateUtil::isValidSerialDate(
                                                                d_serialDate));
        return *this;
    }

    inline
    void MyDate::setYearMonthDay(int year, int month, int day)
    {
        d_serialDate = bdeimp_ProlepticDateUtil::ymd2serial(year, month, day);
        BSLS_ASSERT_SAFE(bdeimp_ProlepticDateUtil::isValidSerialDate(
                                                                d_serialDate));
    }

    inline
    bool MyDate::setYearMonthDayIfValid(int year, int month, int day)
    {
        const int newDate = bdeimp_ProlepticDateUtil::ymd2serial(year,
                                                                 month,
                                                                 day);
        if (bdeimp_ProlepticDateUtil::isValidSerialDate(newDate)) {
            d_serialDate = newDate;
            return true;
        }
        return false;
    }

    // ACCESSORS
    inline
    void MyDate::getYearMonthDay(int *year, int *month, int *day) const
    {
        bdeimp_ProlepticDateUtil::serial2ymd(year, month, day, d_serialDate);
    }

    inline
    int MyDate::year() const
    {
        int year, month, day;
        bdeimp_ProlepticDateUtil::serial2ymd(&year,
                                             &month,
                                             &day,
                                             d_serialDate);
        return year;
    }

    inline
    int MyDate::month() const
    {
        int year, month, day;
        bdeimp_ProlepticDateUtil::serial2ymd(&year,
                                             &month,
                                             &day,
                                             d_serialDate);
        return month;
    }

    inline
    int MyDate::day() const
    {
        int year, month, day;
        bdeimp_ProlepticDateUtil::serial2ymd(&year,
                                             &month,
                                             &day,
                                             d_serialDate);
        return day;
    }

    inline
    bool MyDate::isLeapYear() const
    {
        return bdeimp_ProlepticDateUtil::isLeapYear(year());
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
        bdeimp_ProlepticDateUtil::serial2ymd(&y, &m, &d, d_serialDate);

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

        Y0001_START        =      1,  // 0001/01/01 (first supported date)
        Y9999_END          = 3652059  // 9999/12/31 (last supported date)
    };

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 7: {
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
// Was the year 2000 a leap year?
//..
    ASSERT(true == bdeimp_ProlepticDateUtil::isLeapYear(2000));       // Yes, it was.
//..
// Was February 29, 1900, a valid date in history?
//..
    ASSERT(false == bdeimp_ProlepticDateUtil::isValidCalendarDate(1900,2,29));
                                                             // No, it was not.
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
      case 6: {
        // --------------------------------------------------------------------
        // Prolepic Date calculations
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting: proleptic date conversion" << endl;

        int serialDate = 0;
        const int EXTRA_SERIAL_DAYS = 11979953;
        for (int year = 1; year <= 9999; ++year) {
            for (int month = 1; month <= 12; ++month) {
                for (int day = 1; day <= 31; ++day) {
                    if (Util::isValidCalendarDate(year, month, day)) {
                        if (veryVerbose) { P_(year) P_(month) P(day) }

                        ++serialDate;

                        int x = Util::ymd2serial(year, month, day);
                        LOOP5_ASSERT(year, month, day, serialDate, x,
                                     serialDate == x);
                        int y = TestUtil::ymdToSerial(year, month, day);
                        y -= EXTRA_SERIAL_DAYS;
                        LOOP5_ASSERT(year, month, day, x, y, x == y);

                        if (veryVerbose) { P(serialDate) }

                        int ey, em, ed, ty, tm, td;
                        Util::serial2ymd(&ey, &em, &ed, serialDate);
                        TestUtil::serialToYmd(&ty, &tm, &td,
                                              serialDate + EXTRA_SERIAL_DAYS);
                        LOOP3_ASSERT(serialDate, ey, year,  ey == year);
                        LOOP2_ASSERT(ty, ey, ey == ty);
                        LOOP2_ASSERT(tm, em, em == tm);
                        LOOP2_ASSERT(td, ed, ed == td);
                    }
                }
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // Testing:
        //   static void serial2ymd(int *year, int *month, int *day,
        //                          int serialDate);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting 'serial2ymd'"
                          << "\n====================" << endl;

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

                { L_,           36523,     100,    12,   30 },
                { L_,           36524,     100,    12,   31 },
                { L_,           36525,     101,     1,    1 },

                { L_,          146096,     400,    12,   30 },
                { L_,          146097,     400,    12,   31 },
                { L_,          146098,     401,     1,    1 },

                { L_,          657071,    1799,    12,   31 },
                { L_,          657072,    1800,     1,    1 },
                { L_,          657436,    1800,    12,   31 },
                { L_,          657437,    1801,     1,    1 },
                { L_,          657801,    1801,    12,   31 },
                { L_,          657802,    1802,     1,    1 },

                { L_,          693595,    1899,    12,   31 },
                { L_,          693596,    1900,     1,    1 },
                { L_,          693960,    1900,    12,   31 },
                { L_,          693961,    1901,     1,    1 },
                { L_,          694325,    1901,    12,   31 },
                { L_,          694326,    1902,     1,    1 },

                { L_,          730119,    1999,    12,   31 },
                { L_,          730120,    2000,     1,    1 },
                { L_,          730485,    2000,    12,   31 },
                { L_,          730486,    2001,     1,    1 },
                { L_,          730850,    2001,    12,   31 },
                { L_,          730851,    2002,     1,    1 },

                { L_,          766644,    2099,    12,   31 },
                { L_,          766645,    2100,     1,    1 },
                { L_,          767009,    2100,    12,   31 },
                { L_,          767010,    2101,     1,    1 },
                { L_,          767374,    2101,    12,   31 },
                { L_,          767375,    2102,     1,    1 },

                { L_,          876216,    2399,    12,   31 },
                { L_,          876217,    2400,     1,    1 },
                { L_,          876582,    2400,    12,   31 },
                { L_,          876583,    2401,     1,    1 },
                { L_,          876947,    2401,    12,   31 },
                { L_,          876948,    2402,     1,    1 },

                { L_,         3650233,    9994,    12,   31 },
                { L_,         3650234,    9995,     1,    1 },
                { L_,         3650598,    9995,    12,   31 },
                { L_,         3650599,    9996,     1,    1 },
                { L_,         3650964,    9996,    12,   31 },
                { L_,         3650965,    9997,     1,    1 },
                { L_,         3651329,    9997,    12,   31 },
                { L_,         3651330,    9998,     1,    1 },
                { L_,         3651694,    9998,    12,   31 },
                { L_,         3651695,    9999,     1,    1 },
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
                    int year, month, day;
                    int serial = Util::ymd2serial(y, m, d);
                    Util::serial2ymd(&year, &month, &day, serial);

                    ASSERT(y == year);
                    ASSERT(m == month);
                    ASSERT(d == day);
                }
            }
        }

        if (verbose) cout << "\nserial -> (y, m, d) -> serial" << endl;

        for (int s = Y0001_START; s <= Y9999_END; ++s) {
            int year, month, day;
            if (verbose && 0 == s % 100000 ) {
                cout << "\tExhaustively testing dates " << s/100000 << "00000"
                     << " to " << s/100000 << 99999 << endl;
            }

            Util::serial2ymd(&year, &month, &day, s);

            ASSERT(s == Util::ymd2serial(year, month, day));
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // Testing:
        //   static int ymd2serial(int year, int month, int day);
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

                { L_,        100,    12,    31,        36524 },
                { L_,        101,     1,     1,        36525 },

                { L_,        400,    12,    31,       146097 },
                { L_,        401,     1,     1,       146098 },

                { L_,       1600,    12,    31,       584388 },
                { L_,       1601,     1,     1,       584389 },

                { L_,       1800,     1,     1,       657072 },
                { L_,       1800,    12,    31,       657436 },

                { L_,       1801,     1,     1,       657437 },
                { L_,       1801,    12,    31,       657801 },
                { L_,       1802,     1,     1,       657802 },

                { L_,       1899,    12,    31,       693595 },
                { L_,       1900,     1,     1,       693596 },
                { L_,       1900,    12,    31,       693960 },
                { L_,       1901,     1,     1,       693961 },
                { L_,       1901,    12,    31,       694325 },
                { L_,       1902,     1,     1,       694326 },

                { L_,       1999,    12,    31,       730119 },
                { L_,       2000,     1,     1,       730120 },
                { L_,       2000,    12,    31,       730485 },
                { L_,       2001,     1,     1,       730486 },
                { L_,       2001,    12,    31,       730850 },
                { L_,       2002,     1,     1,       730851 },

                { L_,       2099,    12,    31,       766644 },
                { L_,       2100,     1,     1,       766645 },
                { L_,       2100,    12,    31,       767009 },
                { L_,       2101,     1,     1,       767010 },
                { L_,       2101,    12,    31,       767374 },
                { L_,       2102,     1,     1,       767375 },

                { L_,       2399,    12,    31,       876216 },
                { L_,       2400,     1,     1,       876217 },
                { L_,       2400,    12,    31,       876582 },
                { L_,       2401,     1,     1,       876583 },
                { L_,       2401,    12,    31,       876947 },
                { L_,       2402,     1,     1,       876948 },

                { L_,       9995,     1,     1,      3650234 },
                { L_,       9995,    12,    31,      3650598 },
                { L_,       9996,     1,     1,      3650599 },
                { L_,       9996,    12,    31,      3650964 },
                { L_,       9997,     1,     1,      3650965 },
                { L_,       9997,    12,    31,      3651329 },
                { L_,       9998,     1,     1,      3651330 },
                { L_,       9998,    12,    31,      3651694 },
                { L_,       9999,     1,     1,      3651695 },
                { L_,       9999,    12,    31,    Y9999_END },
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int di = 0; di < NUM_DATA ; ++di) {
                const int LINE = DATA[di].d_lineNum;
                const int Y    = DATA[di].d_year;
                const int M    = DATA[di].d_month;
                const int D    = DATA[di].d_day;
                const int EXP  = DATA[di].d_exp;
                const int RES  = Util::ymd2serial(Y, M, D);
                LOOP3_ASSERT(LINE, EXP, RES, EXP == RES);
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // Testing:
        //   static bool isValidSerialDate(int serialDay);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting: 'isValidSerialDate'" << endl;
        {
            int s;
            for (s = -1500; s <= 0; ++s) {
                ASSERT(0 == Util::isValidSerialDate(s));
            }

            for (s = Y0001_START; s <= Y9999_END; ++s) {
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

                { L_,     Y0001_START-2,    0              },
                { L_,     Y0001_START-1,    0              },
                { L_,     Y0001_START  ,    1              },
                { L_,     Y0001_START+1,    1              },
                { L_,     Y0001_START+2,    1              },

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
      case 2: {
        // --------------------------------------------------------------------
        // Testing:
        //   static bool isValidCalendarDate(int year, int month, int day);
        // --------------------------------------------------------------------

        if (verbose) cout << "\nTesting: 'isValidCalendarDate'" << endl;
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
                { L_,          2,     2,    29,      0 },
                { L_,          2,     2,    30,      0 },
                { L_,          2,     3,     1,      1 },

                { L_,          4,     2,    28,      1 },
                { L_,          4,     2,    29,      1 },
                { L_,          4,     2,    30,      0 },
                { L_,          4,     3,     1,      1 },

                { L_,        100,     2,    28,      1 },
                { L_,        100,     2,    29,      0 },
                { L_,        100,     3,     1,      1 },

                { L_,        200,     2,    28,      1 },
                { L_,        200,     2,    29,      0 },
                { L_,        200,     3,     1,      1 },

                { L_,        400,     2,    28,      1 },
                { L_,        400,     2,    29,      1 },
                { L_,        400,     3,     1,      1 },

                { L_,        500,     2,    28,      1 },
                { L_,        500,     2,    29,      0 },
                { L_,        500,     3,     1,      1 },

                { L_,        800,     2,    28,      1 },
                { L_,        800,     2,    29,      1 },
                { L_,        800,     3,     1,      1 },

                { L_,       1600,     2,    29,      1 },
                { L_,       1700,     2,    29,      0 },
                { L_,       1800,     2,    29,      0 },
                { L_,       1900,     2,    29,      0 },
                { L_,       2000,     2,    29,      1 },
                { L_,       2100,     2,    29,      0 },

                { L_,       1752,     2,    28,      1 },
                { L_,       1752,     2,    29,      1 },
                { L_,       1752,     2,    30,      0 },

                { L_,       1900,     2,    28,      1 },
                { L_,       1900,     2,    29,      0 },
                { L_,       1900,     2,    30,      0 },

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
                { L_,         10,              0 },
                { L_,         96,              1 },
                { L_,         99,              0 },
                { L_,        100,              0 },
                { L_,        101,              0 },
                { L_,        104,              1 },
                { L_,        200,              0 },
                { L_,        396,              1 },
                { L_,        399,              0 },
                { L_,        400,              1 },
                { L_,        401,              0 },
                { L_,        600,              0 },
                { L_,        700,              0 },
                { L_,        799,              0 },
                { L_,        800,              1 },
                { L_,        801,              0 },
                { L_,       1000,              0 },
                { L_,       1600,              1 },
                { L_,       1700,              0 },
                { L_,       1752,              1 },
                { L_,       1800,              0 },
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
