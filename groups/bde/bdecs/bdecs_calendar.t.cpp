// bdecs_calendar.t.cpp                                               -*-C++-*-

#include <bdecs_calendar.h>

#include <bdema_bufferedsequentialallocator.h>  // for testing only
#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bsls_platformutil.h>                  // for testing only

#include <bsl_hash_map.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_utility.h>

#include <bsl_cstdlib.h>      // atoi()
#include <bsl_cstring.h>      // strcmp()

#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <crtdbg.h>  // '_CrtSetReportMode', to suppress popups
#endif

using namespace BloombergLP;
using bsl::cerr;
using bsl::cout;
using bsl::endl;
using bsl::flush;

//==========================================================================
//                             TEST PLAN
//--------------------------------------------------------------------------
//                              Overview
//                              --------
// A 'bdecs_Calendar' is a value-semantic type that represents a collection of
// weekend days and holidays over a given range of dates, and with each holiday
// associated with a possibly empty list of unique (integer) holiday codes).
// Two calendars have the same 'value' if they (1) designate the same
// days-of-the-week as "weekend" days, (2) have the same valid range, (3) have
// the same days within that range designated as holidays, and (4) for each
// holiday, have the same set of associated integer codes.
//
// We have chosen the primary manipulators for 'bdecs_Calendar' to be 'addDay',
// 'addHoliday', 'addHolidayCode', 'addWeekendDay', 'addWeekendDaysTransition',
// and 'removeAll'.  We have chosen the basic accessors for 'bdecs_Calendar' to
// be 'length', 'isInRange', 'firstDate', 'lastDate', 'isNonBusinessDay',
// 'isHoliday', 'isWeekendDay(bdet_DayOfWeek::Day)',
// 'beginWeekendDaysTransitions', 'endWeekendDaysTransitions',
// 'numWeekendDaysTransitions', 'numBusinessDays', 'numNonBusinessDays',
// 'packedCalendar' and all the iterators except the business day iterators.
//--------------------------------------------------------------------------
// ITERATORS
// [15] BusinessDayConstIterator;
// [ 4] HolidayConstIterator;
// [ 4] HolidayCodeConstIterator;
// [15] BusinessDayConstReverseIterator;
// [ 4] HolidayConstReverseIterator;
// [ 4] HolidayCodeConstReverseIterator;
// [ 4] WeekendDayTransitionConstReverseIterator;
//
// CLASS METHODS
// [10] static int maxSupportedBdexVersion();
//
// CREATORS
// [ 2] bdecs_Calendar(bslma_Allocator *basicAllocator = 0);
// [11] bdecs_Calendar(const bdet_Date& firstDate, lastDate, ba = 0);
// [11] bdecs_Calendar(const bdecs_PackedCalendar& packedCalendar, ba = 0);
// [ 7] bdecs_Calendar(const bdecs_Calendar& original, ba = 0);
// [ 2] ~bdecs_Calendar();
//
// MANIPULATORS
// [ 9] bdecs_Calendar& operator=(const bdecs_Calendar& rhs);
// [ 9] bdecs_Calendar& operator=(const bdecs_PackedCalendar& rhs);
// [16] void setValidRange(const bdet_Date& fd, const bdet_Date& ld);
// [ 2] void addDay(const bdet_Date& date);
// [ 2] void addHoliday(const bdet_Date& date);
// [ 2] void addHolidayCode(const bdet_Date& date, int holidayCode);
// [ 2] void addWeekendDay(bdet_DayOfWeek::Day weekendDay);
// [17] void addWeekendDays(const bdec_DayOfWeekSet& weekendDays);
// [ 2] void addWeekendDaysTransition(date, weekendDays);
// [20] void intersectBusinessDays(const bdecs_Calendar& calendar);
// [21] void intersectNonBusinessDays(const bdecs_Calendar& calendar);
// [21] void unionBusinessDays(const bdecs_Calendar& calendar);
// [20] void unionNonBusinessDays(const bdecs_Calendar& calendar);
// [19] void removeHoliday(const bdet_Date& date);
// [19] void removeHolidayCode(const bdet_Date& date, int holidayCode);
// [ 2] void removeAll();
// [22] void swap(bdecs_Calendar *rhs);
// [22] void swap(bdecs_Calendar& other);
// [22] void swap(bdecs_Calendar& lhs, bdecs_Calendar& rhs);
// [10] template <class STREAM> STREAM& bdexStreamIn(STREAM& s, int version);
//
// ACCESSORS
// [ 4] WeekendDaysTransitionConstIterator beginWeekendDaysTransitions() const;
// [ 4] WeekendDaysTransitionConstIterator endWeekendDaysTransitions() const;
// [ 4] int numWeekendDaysTransitions() const;
// [15] BusinessDayConstIterator beginBusinessDays() const;
// [15] BusinessDayConstIterator beginBusinessDays(const bdet_Date&) const;
// [ 4] HolidayConstIterator beginHolidays() const;
// [ 4] HolidayConstIterator beginHolidays(const bdet_Date& date) const;
// [ 4] HolidayCodeConstIterator beginHolidayCodes(const iter&) const;
// [ 4] HolidayCodeConstIterator beginHolidayCodes(const bdet_Date&) const;
// [15] BusinessDayConstIterator rbeginBusinessDays() const;
// [15] BusinessDayConstIterator rbeginBusinessDays(const bdet_Date&) const;
// [ 4] HolidayConstIterator rbeginHolidays() const;
// [ 4] HolidayConstIterator rbeginHolidays(const bdet_Date& date) const;
// [ 4] HolidayCodeConstIterator rbeginHolidayCodes(const iter&) const;
// [ 4] HolidayCodeConstIterator rbeginHolidayCodes(const bdet_Date&) const;
// [15] BusinessDayIterator endBusinessDays() const;
// [15] BusinessDayIterator endBusinessDays(const bdet_Date& date) const;
// [ 4] HolidayIterator endHolidays() const;
// [ 4] HolidayIterator endHolidays(const bdet_Date& date) const;
// [ 4] HolidayCodeIterator endHolidayCodes(const HolidayIterator&) const;
// [ 4] HolidayCodeIterator endHolidayCodes(const bdet_Date& date) const;
// [15] BusinessDayIterator rendBusinessDays() const;
// [15] BusinessDayIterator rendBusinessDays(const bdet_Date& date) const;
// [ 4] HolidayIterator rendHolidays() const;
// [ 4] HolidayIterator rendHolidays(const bdet_Date& date) const;
// [ 4] HolidayCodeIterator rendHolidayCodes(const HolidayIterator&) const;
// [ 4] HolidayCodeIterator rendHolidayCodes(const bdet_Date& date) const;
// [ 4] const bdet_Date& firstDate() const;
// [18] bdet_Date getNextBusinessDay(const bdet_Date& initialDate) const;
// [18] bdet_Date getNextBusinessDay(const bdet_Date& initialDate,int n) const;
// [ 4] bool isInRange(const bdet_Date& date) const;
// [14] bool isBusinessDay(const bdet_Date& date) const;
// [ 4] bool isNonBusinessDay(const bdet_Date& date) const;
// [ 4] bool isHoliday(const bdet_Date& date) const;
// [ 4] bool isWeekendDay(const bdet_Date& date) const;
// [ 4] bool isWeekendDay(bdet_DayOfWeek::Day dayOfWeek) const;
// [ 4] const bdet_Date& lastDate() const;
// [ 4] int length() const;
// [ 4] int numBusinessDays() const;
// [ 4] int numNonBusinessDays() const;
// [13] int numHolidays() const;
// [13] int numHolidayCodes(const bdet_Date& date) const;
// [12] int numWeekendDaysInRange() const;
// [23] const bdecs_PackedCalendar& packedCalendar() const;
// [10] template <class STREAM> STREAM& bdexStreamOut(STREAM&, int) const;
// [ 5] ostream& print(ostream& stream, int level = 0, int spl = 4) const;
//
// FREE OPERATORS
// [ 6] operator==(const bdecs_Calendar& lhs, rhs);
// [ 6] operator!=(const bdecs_Calendar& lhs, rhs);
// [ 5] operator<<(ostream&, const bdecs_Calendar&);
//--------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [24] USAGE EXAMPLE
// [ 3] PRIMITIVE GENERATOR FUNCTIONS 'gg' and 'ggg'
// [ 8] TESTING GENERATOR FUNCTION, 'g'
//==========================================================================
//                      STANDARD BDE ASSERT TEST MACRO
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
//                  EXCEPTION SAFETY TEST MACROS
//
//  These macros are very similar to the standard bdema exception test macros.
//  The only significant difference is 'END_EXCEPTION_SAFE_TEST' takes a
//  parameter which it calls 'ASSERT' to verify its non-zero value.
//-----------------------------------------------------------------------------

#ifdef BDE_BUILD_TARGET_EXC
#define BEGIN_EXCEPTION_SAFE_TEST {                                        \
    {                                                                      \
        static int firstTime = 1;                                          \
        if (veryVerbose && firstTime) cout <<                              \
            "\t\tEXCEPTION SAFETY TEST -- (ENABLED) --" << endl;           \
        firstTime = 0;                                                     \
    }                                                                      \
    if (veryVeryVerbose) cout <<                                           \
        "\t\tBegin exception safety test." << endl;                        \
    int bdemaExceptionCounter = 0;                                         \
    testAllocator.setAllocationLimit(bdemaExceptionCounter);               \
    do {                                                                   \
        try {

#define END_EXCEPTION_SAFE_TEST(x)                                         \
        } catch (bslma_TestAllocatorException& e) {                        \
            if (veryVeryVerbose) cout << endl << "\t*** BEDMA_EXCEPTION: " \
                << "alloc limit = " << bdemaExceptionCounter << ", "       \
                << "last alloc size = " << e.numBytes() << " ***" << endl; \
            ASSERT(x);                                                     \
            testAllocator.setAllocationLimit(++bdemaExceptionCounter);     \
            continue;                                                      \
        }                                                                  \
        testAllocator.setAllocationLimit(-1);                              \
        break;                                                             \
    } while (1);                                                           \
    if (veryVeryVerbose) cout <<                                           \
        "\t\tEnd exception safety test." << endl;                          \
}
#else
#define BEGIN_EXCEPTION_SAFE_TEST                                          \
{                                                                          \
    static int firstTime = 1;                                              \
    if (verbose && firstTime) { cout <<                                    \
        "\t\tEXCEPTION SAFETY TEST -- (NOT ENABLED) --" << endl;           \
        firstTime = 0;                                                     \
    }                                                                      \
}
#define END_EXCEPTION_SAFE_TEST(x)
#endif

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define T_ cout << '\t' << flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdecs_Calendar      Obj;

//=============================================================================
// ITERATOR TEST UTILITY FUNCTIONS
//-----------------------------------------------------------------------------

template <typename U, typename V>
struct IsSame
{
    enum { VALUE = 0 };
};

template <typename U>
struct IsSame<U, U>
{
    enum { VALUE = 1 };
};

//=============================================================================
// TEST UTILITY FUNCTIONS
//-----------------------------------------------------------------------------

// The function 'isEqualWithCache' below compares two 'bdecs_Calendar' objects.
// It compares the two objects using 'operator==' first, then verifies that
// their cached non-business day information also matches.  This is necessary
// because 'operator==' only compares the value of their internal packed
// calendar objects.  We want to verify that these objects are also in
// consistent states by examining their cached non-business day information.

namespace {

bool isEqualWithCache(const Obj& lhs, const Obj& rhs)
{
    if (lhs != rhs) {
        return false;
    }

    bdet_Date tempDate = lhs.firstDate();
    while (tempDate <= lhs.lastDate()) {
        if (lhs.isNonBusinessDay(tempDate) != rhs.isNonBusinessDay(tempDate)) {
            return false;
        }
        if (tempDate >= lhs.lastDate()) {
            break;
        }
        ++tempDate;
    }

    return true;
}

bool testCase10VerifyInvalidDataTest(const Obj Xs[], const Obj YYs[],
                                     const int LODs[],
                                     const int NUM_DATA,
                                     const Obj& XX, const int i)
{
    for (int di = 0; di < NUM_DATA; ++di) {
        if (i < LODs[di]) {
            if (!isEqualWithCache(Xs[di], XX)) return false;
        }
        else {
            if (!isEqualWithCache(Xs[di], YYs[di])) return false;
        }
    }
    return true;
}

int numWeekendDaysInFirstTransition(const Obj& calendar)
    // Return the number of weekend days in the weekend-days transition at
    // January 1, 0001, if the transition exist; otherwise, return 0.
{

    if (1 <= calendar.numWeekendDaysTransitions() &&
        bdet_Date(1,1,1) == calendar.beginWeekendDaysTransitions()->first) {
        return calendar.beginWeekendDaysTransitions()->second.length();
    }
    return 0;
}

bool sameWeekendDaysTransition(const Obj::WeekendDaysTransition& transition,
                               const bdet_Date& date,
                               const char *weekendDays)
    // Return 'true' if the specified weekend-days 'transition' has a starting
    // date the same as the specified 'date' and has the same set of weekend
    // days in specified 'weekendDays' string, which represents each day of the
    // week as a lower case character ({u, m, t, w, r, f, a} ==
    // {Sunday...Satruday}).  Return 'false' is the said condition is not true
    // or if 'weekendDays' is not valid.
{

    bdec_DayOfWeekSet wdSet;

    while (*weekendDays) {
        bdet_DayOfWeek::Day dow;
        switch (*weekendDays) {
          case 'u': dow = bdet_DayOfWeek::BDET_SUN; break;
          case 'm': dow = bdet_DayOfWeek::BDET_MON; break;
          case 't': dow = bdet_DayOfWeek::BDET_TUE; break;
          case 'w': dow = bdet_DayOfWeek::BDET_WED; break;
          case 'r': dow = bdet_DayOfWeek::BDET_THU; break;
          case 'f': dow = bdet_DayOfWeek::BDET_FRI; break;
          case 'a': dow = bdet_DayOfWeek::BDET_SAT; break;
          default: return false;
        }
        wdSet.add(dow);
        ++weekendDays;
    };

    return Obj::WeekendDaysTransition(date, wdSet) == transition;
}

}  // close unnamed namespace

//=============================================================================
//              FLEXIBLE GENERATOR FUNCTIONS 'g' and 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The function 'g' and 'gg' interpret a specified 'spec' in order from left to
// right according to a complex custom language to bring the calendar to a
// range of possible states relevant for testing.
//
///LANGUAGE SPECIFICATION
//-----------------------
// <SPEC>       ::= <ABS_DATE>[<DOW_LIST>|<EMPTY>] [<DATE>|<EMPTY>]
//                                                                 <DATEM_LIST>
//                      // Represents the first date of the calendar (along
//                      // with an associated list of weekend days in the
//                      // default weekend-days transition at January 1, 0001),
//                      // and optionally the last date of the calendar as well
//                      // as a list of holiday dates along with their holiday
//                      // codes, and weekend-days transitions.
//
// <DATE>       ::= <ABS_DATE> | <REL_DATE>
//
// <ABS_DATE>   ::= @yyyy/mm/dd
//
// <REL_DATE>   ::= integer value
//
// <DATEM_LIST> ::= <DATEM> | <DATEM> <DATEM_LIST> | <EMPTY>
//                      // Represents a list of a combination of holidays and
//                      // weekend-days transitions.
//
// <DATEM>      ::= <DATE>[<DATE_EMBEL>|<EMPTY>]
//                      // Represents either a holiday if 'DATE' is
//                      // unembellished or is proceeded by a list of holiday
//                      // codes, or a weekend-days transition if 'DATE' is
//                      // proceeded by a list of days of week.
//
// <EMPTY>      ::=
//
// <DATE_EMBEL> ::= <DOW_LIST> | <HCODE_LIST>
//                      // Represents either a list of days of week or a list
//                      // of holiday codes.
//
// <DOW_LIST>   ::= <DOW> | <DOW> <DOW_LIST>
//
// <DOW>        ::= 'u' | 'm' | 't' | 'w' | 'r' | 'f' | 'a'
//                      // Represents days of the week from Sunday to Saturday.
//
// <HCODE_LIST> ::= <HCODE> | <HCODE> <HCODE_LIST>
//
// <HCODE>      ::= 'A' | 'B' | 'C' | 'D' | 'E'
//                      // Represents unique but otherwise arbitrary
//                      // holidaycodes.
//
// These functions enables the explict specification of dates representing the
// first and last date in a calendar, holiday dates, and weekend-days
// transition dates.  An absolute date will be represented as @yyyy/mm/dd, (but
// leading 0's may be omitted).  A relative date will be represented as an
// unsigned integer offset from the start of the current range.  Note that
// relative dates should not be used on an empty calendar.
//
// Holiday codes are represented symbolically as uppercase letters (from 'A' to
// 'E').  Days of the week to be considered weekend days are identifier by
// lowercase letters ('u', 'm', 't', 'w', 'r', 'f', 'a').
//
// The first date in a spec represents one end of the range and must be
// absolute.  If the first date is embellished by weekend-day identifiers, use
// the legacy 'addWeekendDay' method to add the corresponding weekend days to
// the calendar.  The second date, if present, represents the other end of the
// range.  A subsequent date represents either a holiday or a weekend-days
// transition -- if the date is proceeded by holiday codes or is unembellished,
// then the date represents a holiday; otherwise, if the date is proceeded
// weekend-day identifiers, then the date represents the starting date of a
// weekend-day transition; otherwise, if the date is proceeded weekend-day
// identifiers or the lower case letter 'e' (representing an empty transition),
// then the date represents the starting date of a weekend-day transition.
//
// Any associated holiday codes should be unique for that holiday.  Whitespace
// is used to delimit integer fields with no intervening holiday codes (notice
// that absolute dates and holiday codes are self-delimiting).
//
//
// This more powerful, but also more complex and less concise notation is used
// as follows:
//
//   Spec                 Description
//   ----                -----------
//   ""                   Calendar is Empty (no weekend days, empty range).
//
//   "m"                  [ MON ] is a weekend day, empty range.
//
//   "ua"                 [ SUN SAT ] are weekend days, empty range.
//
//   "@2000"              Ill-formed: improper absolute date format
//
//   "@2000/1/1"          No weekend days; range is 2000/1/1..2000/1/1.
//
//   "u@2000/1/1a"        [ SUN SAT ]; range is 2001/1/1..2000/1/1.
//
//   "@2000/1/1sa"        Ill formed: unrecognized character 's' at position 9.
//
//   "u@2000/1/1 @2000/1/10 @2000/1/1mt"
//                       Range: 2000/1/1...2000/1//10
//                       W/e Trans: [ 1/1/1: [ SUN ], 2000/1/1: [ MON TUE ] ]
//
//   "rf@2000/1/1 @2000/1/3 @2000/1/1"
//                        W/e Days: [ THU FRI ], Range: 2000/1/1..2000/1/3,
//                        Holidays: 2000/1/1
//
//   "rf@2000/1/1 2 0"    Same as above.
//
//   "w@2000/1/1 59 0A 14B 30C 45 59DE"
//                        W/e Days: [ WED ], Range: 2000/1/1..2000/2/29,
//                        Holidays(codes): 2000/1/1(0), 2000/1/15(1),
//                        2000/1/31(2), 2000/2/15(), 2000/2/29(100,1000).
//
// An implementation of a primitive generator interpreting this more elaborate
// and flexible notation is as follows:

// Holiday codes

int VA = 0, VB = 1, VC = 2, VD = 100, VE = 1000;

enum { SUCCESS = 1, FAILURE = 0 };

// Utility macros to make the code more readable.

#define SUCCEED        { *endPos = input; return SUCCESS; }
#define FAIL           { *endPos = input; return FAILURE; }
#define FAIL_UNLESS(c) { if (!(c)) FAIL; }

void skipOptionalWhiteSpace(const char **endPos, const char *input)
{
    ASSERT(endPos); ASSERT(input);
    while(*input && isspace(*input)) {
        ++input;
    }
    *endPos = input;
}

int skipRequiredChar(const char **endPos, const char *input, char token)
{
    ASSERT(endPos); ASSERT(input); ASSERT(token);
    FAIL_UNLESS(*input == token);
    *endPos = input + 1;
    return SUCCESS; // cannot use SUCCEED here because of the previous line
}

int parseUint(const char **endPos, int *result, const char *input)
{
    ASSERT(endPos); ASSERT(result); ASSERT(input);
    FAIL_UNLESS(isdigit(*input));
    int tmp = 0;
    while (isdigit(*input)) {
        tmp *= 10;
        tmp += *input - '0';
        ++input;
    }
    *result = tmp;
    SUCCEED;
}

int parseAbsoluteDate(const char **endPos,
                      bdet_Date   *result,
                      const char  *input)
{
    ASSERT(endPos); ASSERT(result); ASSERT(input);
    FAIL_UNLESS(skipRequiredChar(endPos, input, '@'));  input = *endPos;

    int y, m, d;
    skipOptionalWhiteSpace(endPos, input);              input = *endPos;
    FAIL_UNLESS(parseUint(endPos, &y, input));
    FAIL_UNLESS(1 <= y && y <= 9999);                   input = *endPos;

    skipOptionalWhiteSpace(endPos, input);              input = *endPos;
    FAIL_UNLESS(skipRequiredChar(endPos, input, '/'));  input = *endPos;

    skipOptionalWhiteSpace(endPos, input);              input = *endPos;
    FAIL_UNLESS(parseUint(endPos, &m, input));
    FAIL_UNLESS(1 <= m && m <= 12);                     input = *endPos;

    skipOptionalWhiteSpace(endPos, input);              input = *endPos;
    FAIL_UNLESS(skipRequiredChar(endPos, input, '/'));  input = *endPos;

    skipOptionalWhiteSpace(endPos, input);              input = *endPos;
    FAIL_UNLESS(parseUint(endPos, &d, input));
    FAIL_UNLESS(bdet_Date::isValid(y, m, d));           input = *endPos;

    result->setYearMonthDay(y, m, d);
    SUCCEED;
}

int parseRelativeDate(const char **endPos,
                      bdet_Date *result,
                      const char *input,
                      const bdet_Date& base)
{
    ASSERT(endPos); ASSERT(result); ASSERT(input);
    int offset;
    FAIL_UNLESS(parseUint(endPos, &offset, input));        input = *endPos;
    *result = base + offset;
    SUCCEED;
}

int loadWeekendDay(const char **endPosAddr,
                   bdecs_Calendar *result,
                   char dayOfWeek)
{
    ASSERT(result);
    bdet_DayOfWeek::Day dow;
    switch (dayOfWeek) {
      case 'u': dow = bdet_DayOfWeek::BDET_SUN; break;
      case 'm': dow = bdet_DayOfWeek::BDET_MON; break;
      case 't': dow = bdet_DayOfWeek::BDET_TUE; break;
      case 'w': dow = bdet_DayOfWeek::BDET_WED; break;
      case 'r': dow = bdet_DayOfWeek::BDET_THU; break;
      case 'f': dow = bdet_DayOfWeek::BDET_FRI; break;
      case 'a': dow = bdet_DayOfWeek::BDET_SAT; break;
      default: return FAILURE;
    };
    int before = numWeekendDaysInFirstTransition(*result);
    result->addWeekendDay(dow);

    // check for duplicates
    if (before >= numWeekendDaysInFirstTransition(*result)) {
        return FAILURE;
    }
    ++*endPosAddr;
    return SUCCESS;
}

int loadWeekendDaysTransition(const char **endPosAddr,
                              bdecs_Calendar *result,
                              const bdet_Date& transitionDate)
{
    ASSERT(result);
    bdec_DayOfWeekSet weekendDays;

    while (islower(**endPosAddr))
    {
        bdet_DayOfWeek::Day dow;
        bool emptyFlag = false;
        switch (**endPosAddr) {
          case 'u': dow = bdet_DayOfWeek::BDET_SUN; break;
          case 'm': dow = bdet_DayOfWeek::BDET_MON; break;
          case 't': dow = bdet_DayOfWeek::BDET_TUE; break;
          case 'w': dow = bdet_DayOfWeek::BDET_WED; break;
          case 'r': dow = bdet_DayOfWeek::BDET_THU; break;
          case 'f': dow = bdet_DayOfWeek::BDET_FRI; break;
          case 'a': dow = bdet_DayOfWeek::BDET_SAT; break;
          case 'e': emptyFlag = true; break;
          default: return FAILURE;
        };
        if (emptyFlag) {
            ++*endPosAddr;
            weekendDays.removeAll();
            break;
        }

        int before = weekendDays.length();
        weekendDays.add(dow);

        // Check for duplicates.
        if (before >= weekendDays.length()) {
            return FAILURE;
        }
        ++*endPosAddr;
    }

    result->addWeekendDaysTransition(transitionDate, weekendDays);
    return SUCCESS;
}

int loadHolidayCode(const char **endPosAddr,
                    bdecs_Calendar *result,
                    const bdet_Date& holiday,
                    char holidayCode)
{
    ASSERT(result); ASSERT(0 != result->length());
    int code;
    switch (holidayCode) {
      case 'A': code = VA; break;
      case 'B': code = VB; break;
      case 'C': code = VC; break;
      case 'D': code = VD; break;
      case 'E': code = VE; break;
      default: return FAILURE;
    };
    int before = result->numHolidayCodes(holiday);    // check for duplicates
    result->addHolidayCode(holiday, code);
    if (before >= result->numHolidayCodes(holiday)) { // check for duplicates
        return FAILURE;                               // check for duplicates
    }                                                 // check for duplicates
    ++*endPosAddr;
    return SUCCESS;
}

int ggg(bdecs_Calendar *object, const char *spec, bool verboseFlag =true)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulators 'addDay', 'addHoliday', and
    // 'addHolidayCode'.  Optionally specify a 'false' 'verboseFlag' to
    // suppress 'spec' syntax error messages.  Return the index of the first
    // invalid character, and a negative value otherwise.  Note that this
    // function is used to implement 'gg' as well as allow for verification of
    // syntax error detection.
{
    ASSERT(object);
    ASSERT(spec);
    enum { SUCCESS = -1 };
    const char *input = spec;

    bdet_Date lastDate;   // Reference date for holiday codes; initially unset.
    bool lastDateIsSet = false;
    bdet_Date lastAbsDate;// Reference date for relative dates; initially
                          // unset.

    int numDays = 0;
    const char *endPos = input;

    while (*input) {
        if (isspace(*input)) {                      // WHITE SPACE
            skipOptionalWhiteSpace(&endPos, input);
        }
        else if (islower(*input)) {                // WEEKEND DAYS
            if (numDays <= 1) {
                if (loadWeekendDay(&endPos, object, *input) == FAILURE) {
                    if (verboseFlag) {
                        cout << "Error: bad weekend day." << endl;
                    }
                    break;
                }
            }
            else {
                if (loadWeekendDaysTransition(&endPos, object, lastDate)
                                                                  == FAILURE) {
                    if (verboseFlag) {
                        cout << "Error: bad weekend-day transition." << endl;
                    }
                    break;
                }
            }
        }
        else if (isupper(*input)) {               // HOLIDAY CODES
            if (!lastDateIsSet) {
                if (verboseFlag) {
                    cout << "Error: no dates have been specified." << endl;
                }
                break;
            }

            if (loadHolidayCode(&endPos, object, lastDate, *input) == FAILURE){
                if (verboseFlag) {
                    cout << "Error: '"<< *input << "' bad holiday code."
                         << endl;
                }
                break;
            }
        }
        else if ('@' == *input ||
                   (isdigit(*input) && lastDateIsSet)) { // DAYS/HOLIDAYS
            bdet_Date date;

            if ('@' == *input) {                        // absolute date
                if (parseAbsoluteDate(&endPos, &date, input) == FAILURE) {
                    if (verboseFlag) {
                        cout << "Error: bad absolute date." << endl;
                    }
                    break;
                }
                lastDate = date;      // Set reference date for holiday codes.
                lastDateIsSet = true; // We can now set holiday codes (above).
                lastAbsDate = date;  // Set reference date for relative dates.
            }
            else {                                      // relative date
                if (parseRelativeDate(&endPos, &date, input, lastAbsDate) ==
                        FAILURE) {
                    if (verboseFlag) {
                        cout << "Error: bad relative date." << endl;
                    }
                    break;
                }
                lastDate = date;      // Set reference date for holiday codes.

                // 'lastDateIsSet' is already set if we are here.
            }

            if (numDays < 2) {
                object->addDay(date);
            }
            else {
                // Look ahead to see the next character to see if this date
                // represents a holiday or a weekend-days transition.
                if (*endPos == 0 || isupper(*endPos) || isspace(*endPos)) {
                    object->addHoliday(date);
                }
            }
            ++numDays;                   // Keep track of dates parsed:
                                         // day/holiday.
        }
        else if ('~' == *input) { // Undocumented Feature!!!
            endPos = input + 1;
            object->removeAll();

            // Reset the initial state.

            lastDateIsSet = false;
            numDays = 0;
        }
        else {
            if (verboseFlag) {
                   cout << "Error: unrecognised character." << endl;
            }
            break;
        }

        input = endPos; // So far so good; update input and continue.
    } // end while not done

    if (*input) {
            int idx = endPos - spec;
        if (verboseFlag) {
            cout << "An error occurred near character ('" << spec[idx]
                 << "') in spec \"" << spec << "\" at position " << idx
                 << '.' << endl;
        }
        return idx;  // Discontinue processing this spec.
    }

    return -1; // All input was consumed.
}

bdecs_Calendar gg(bdecs_Calendar * object, const char *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec' according to the custom language
    // described above (in the test driver).
{
    ASSERT(object); ASSERT(spec);
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

bdecs_Calendar g(const char *spec)
    // Return, by value, an object with its value adjusted according to the
    // specified 'spec' according to the custom language described above (in
    // the test driver).
{
    ASSERT(spec);
    bdecs_Calendar object;
    return gg(&object, spec);
}

//=============================================================================
//                             USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// The two subsections below illustrate various aspects of populating and
// using calendars.
//
///Populating Calendars
///- - - - - - - - - -
// 'bdecs_Calendars' can be populated directly, but are often more efficiently
// created by first creating a corresponding 'bdecs_PackedCalendar', and then
// using that object to construct the calendar.  As an example, suppose we
// want to provide efficient access to a (high-performance) 'bdecs_Calendar'
// for a variety of locales, whose raw information comes from a database.  The
// latency associated with fetching data for individual calendars on demand
// from a typical database can be prohibitively expensive, so it may make
// sense to acquire data for all calendars in a single query at start-up.
//
// Imagine we have a 'MyPackedCalendarCache' which, internally, is just a
// mapping from (typically short) character string names (such as "NYB",
// representing New York Bank settlement days) to instances of
// 'bdecs_PackedCalendar' objects, containing densely packed calendar data:
//..
    class MyPackedCalendarCache {
        // This class maintains a space-efficient repository of calendar data
        // associated with a (typically short) name.

        // DATA
        bsl::hash_map<bsl::string, bdecs_PackedCalendar *>  d_map;
        bslma_Allocator                                    *d_allocator_p;

      public:
         // CREATORS
        MyPackedCalendarCache(bslma_Allocator *basicAllocator = 0);
            // Create an empty 'MyPackedCalendarCache'.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.

        // ...

         // ACCESSORS
        const bdecs_PackedCalendar *lookup(const bsl::string& name) const;
            // Return the address of calendar data associated with the
            // specified 'name', or 0 if no such association exists.
    };

    MyPackedCalendarCache::MyPackedCalendarCache(
                                               bslma_Allocator *basicAllocator)
    : d_map(basicAllocator)
    {
    }

    const bdecs_PackedCalendar *
    MyPackedCalendarCache::lookup(const bsl::string& name) const
    {
        typedef bsl::hash_map<bsl::string, bdecs_PackedCalendar *> Cache;
        Cache::const_iterator itr = d_map.find(name);

        if (itr == d_map.end()) {
            return 0;                                                 // RETURN
        }
        return itr->second;
    }
//..
// Now imagine an application function 'loadMyPackedCalendarCache' which takes
// the address of a 'MyPackedCalendarCache' and populates it with up-to-date
// calendar data for all known locales from a well-known database location:
//..
//  int loadMyPackedCalendarCache(MyPackedCalendarCache *result);
//      // Load into the specified 'result', up-to-date calendar information
//      // for every known locale.  Return 0 on success, and an non-zero
//      // value otherwise.
//..
// We can imagine that there might be dozens, even hundreds of different
// locales, and that most applications will not need efficient access to
// calendar data from many, let alone every locale; however, many long-running
// applications may well need to obtain efficient access to the same calendar
// data repeatedly.  We therefore create a second-level of cache,
// 'MyCalendarCache', that maintains a repository of the more
// runtime-efficient, but also more space-intensive 'bdecs_Calendar' objects,
// which are instantiated on demand from a packed-calendar-based data source:
//..
    class MyCalendarCache {
        // This class maintains a cache of runtime-efficient calendar objects
        // created on demand from a compact packed-calendar-based data source,
        // whose address is supplied at construction.

        // DATA
        MyPackedCalendarCache                      *d_datasource_p;
        bsl::hash_map<bsl::string, bdecs_Calendar>  d_map;

      public:
        // CREATORS
        MyCalendarCache(MyPackedCalendarCache *dataSource,
                        bslma_Allocator       *basicAllocator = 0);
            // Create an empty 'MyCalendarCache' associated with the specified
            // 'dataSource'.  Optionally specify a 'basicAllocator' used to
            // supply memory.  If 'basicAllocator' is 0, the currently installed
            // default allocator is used.

        // ...

        // MANIPULATORS
        const bdecs_Calendar *lookup(const bsl::string& name);
            // Return the address of calendar data associated with the
            // specified name, or 0 if no such association exists in
            // the data source whose address was supplied at construction.
            // Note that this method may alter the physical state of this
            // object (and is therefore deliberately declared non-'const').

        // ...

    };

    MyCalendarCache::MyCalendarCache(MyPackedCalendarCache *dataSource,
                                     bslma_Allocator       *basicAllocator)
    : d_datasource_p(dataSource)
    , d_map(basicAllocator)
    {
    }

    const bdecs_Calendar *MyCalendarCache::lookup(const bsl::string& name)
    {
        if (d_map.find(name) != d_map.end()) {
            const bdecs_PackedCalendar *pc = d_datasource_p->lookup(name);
            if (!pc) {

                // No such name in the data source.

                return 0;                                             // RETURN
            }

            // Create new entry in calendar cache.

            d_map.insert(bsl::make_pair(name, *pc));
        }

        // An efficient calendar either already existed or was created.

        return &d_map[name];
    }
//
///Using Calendars
///- - - - - - - -
// What makes a 'bdecs_Calendar' substantially different from a
// 'bdecs_PackedCalendar' is the speed with which the 'bdecs_Calendar' can
// report whether a given date is or is not a business day.  An important
// use of high-performance calendar objects in financial applications is to
// quickly determine the settlement date of a financial instrument.  In some
// applications (e.g., those that explore the cross product of various
// portfolios over several horizons and scenarios), the settlement date may
// need to be calculated literally billions of times.  The settlement date
// will often be determined from a periodic target date, such as the 15th
// or 30th of the month, which is then perturbed in some way to arrive at
// a valid settlement date.
//
// One very common algorithm a security may prescribe for finding a valid
// settlement date is known as *modified* *following*: Given a target day,
// the settlement date for that month is defined as the first valid, business
// day at or after the given target day in the same month; if no such date
// exists, then the settlement date is the day closest valid business day
// before the target day in that month:
//..
    struct MyCalendarUtil {

        static bdet_Date modifiedFollowing(int                   targetDay,
                                           int                   month,
                                           int                   year,
                                           const bdecs_Calendar& calendar)
            // Return the date of the first business day at or after the
            // specified 'targetDay' in the specified 'month' and 'year'
            // according to the specified 'calendar', unless the resulting
            // date would not fall within 'month', in which case return
            // instead the date of the first business day before 'targetDay'
            // in 'month'.  The behavior is undefined unless all candidate
            // dates applied to 'calendar' are within its valid range and
            // there exists at least one business day within 'month'.
        {
            BSLS_ASSERT(bdet_Date::isValid(year, month, targetDay));

            // Efficiency is important so we will avoid converting to and
            // from year/month/day to 'bdet_date' objects more than we must.

            bdet_Date date(year, month, targetDay);
            bdet_Date date2(date);

            while (calendar.isNonBusinessDay(date)) {
                ++date;
            }
            if (month == date.month()) {
                return date;                                          // RETURN
            }
            do {
                --date2;
            } while (calendar.isNonBusinessDay(date2));
            return date2;
       }
   };

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
#ifdef BSLS_PLATFORM__OS_WINDOWS
    // Suppress all windows debugging popups
    _CrtSetReportMode(_CRT_ASSERT,0);
    _CrtSetReportMode(_CRT_ERROR, 0);
    _CrtSetReportMode(_CRT_WARN,  0);
#endif

    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 24: {
        // --------------------------------------------------------------------
        //  USAGE EXAMPLE TEST
        //
        // Concerns:
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.
        //
        // Plan:
        //   Since none of the functions introduced in the usage example are
        //   templated functions, no specific calls are necessary to verify
        //   they compile and link.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE"
                          << endl << "=============" << endl;

      } break;
      case 23: {
        // --------------------------------------------------------------------
        // TESTING 'packedCalendar':
        //
        // Concerns:
        //   1.  The non-modifiable reference to the 'bdecs_PackedCalendar'
        //       returned must be an accurate representation of the
        //       'bdecs_Calendar' in question.
        //
        // Plans:
        //   To address concern 1, we create a custom set of calendars and
        //   for each of them we obtain a 'bdecs_PackedCalendar' that
        //   represents it.  We are then creating a new calendar from its
        //   packed representation and we make sure they are equal comparing
        //   them.  Then we are going to modify the original calendar and,
        //   obtaining a new 'bdecs_PackedCalendar' that represents it, we
        //   create a new calendar from its new packed representation and make
        //   sure they are equal comparing them.
        //
        // Testing:
        //   const bdecs_PackedCalendar& packedCalendar() const
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'packedCalendar'" << endl
                          << "==============" << endl;

        static const char *SPECS[] = {
            "", "a", "@1/1/1", "@9999/12/31",

            "@2004/1/2 20 1 2A 3 4B 5 10",
            "au@2004/7/1 30 1 3 5 10",
            "mtwfau@2005/1/1 800 11 23B 38C 406A",
            "u@2000/1/1 200 10A 5mt 100wr 150f",
            "m@1990/3/1 100 20tr 50rf 100f",
            "m@1990/3/1 100 0mf 20mt 60rf",
        0};

        for (int i = 0; SPECS[i]; ++i) {
            Obj mX(&testAllocator); const Obj& X = mX;
            gg(&mX, SPECS[i]);
            const Obj XX(mX);

            const bdecs_PackedCalendar& packedCal = mX.packedCalendar();

            Obj mY(packedCal, &testAllocator); const Obj& Y = mY;

            if (veryVerbose) {
                T_; T_; P(X);
            }

            if (veryVeryVerbose) {
                T_; T_; P(Y);
            }

            LOOP_ASSERT(i, isEqualWithCache(X,XX));
            LOOP_ASSERT(i, isEqualWithCache(X, Y));

            if (veryVerbose)
                cout << "Modifying calendar adding a unique holiday"
                     << endl;

            mX.addHoliday(X.lastDate() + 2);

            // Need too rebuild a calendar from the modified reference.

            Obj mZ(packedCal, &testAllocator); const Obj& Z = mZ;

            if (veryVerbose) {
                T_; T_; P(X);
            }

            if (veryVeryVerbose) {
                T_; T_; P(Z);
            }

            LOOP_ASSERT(i, isEqualWithCache(X,Z));
        }

      } break;
      case 22: {
        // --------------------------------------------------------------------
        // TESTING 'swap':
        //
        // Concerns:
        //   1. This method properly swaps all data members of the two
        //      calendars, including first date, last day, weekend days,
        //      holidays, holiday codes, and the non-business day cache.
        //   2. This method is exception-safe with a guarantee of rollback.
        //
        // Plans:
        //   To address concern 1, we use a set of calendars of arbitrarily
        //   determined values and exercise 'swap' on them.  Then we verify
        //   their values are properly swapped by checking both their internal
        //   packed calendar objects and their non-business day caches.
        //
        //   To address concern 2, we use the 'BEGIN_EXCEPTION_SAFE_TEST' and
        //   'END_EXCEPTION_SAFE_TEST' macros to generate memory exceptions and
        //   verify that the value of the calendar object is rolled back when
        //   an exception is thrown.
        //
        // Testing:
        //   void swap(bdecs_Calendar *);
        //   void swap(bdecs_Calendar&);
        //   void swap(bdecs_Calendar&, bdecs_Calendar&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'swap'" << endl
                          << "==============" << endl;

        struct {
            int         d_line;
            const char *d_spec;
        } DATA[] = {
            { L_, "" },
            { L_, "a" },
            { L_, "@1/1/1" },
            { L_, "@9999/12/31" },
            { L_, "@2004/1/2 20 1 2A 3 4B 5 10" },
            { L_, "au@2004/7/1 30 1 3 5 10" },
            { L_, "mtwfau@2005/1/1 800 11 23B 38C 406A" },
            { L_, "u@2000/1/1 200 10A 5mt 100wr 150f" },
            { L_, "m@1990/3/1 100 20tr 50rf 100f" },
            { L_, "m@1990/3/1 100 0mf 20mt 60rf" },
        };
        enum { NUM_DATA = sizeof DATA / sizeof *DATA };

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE_I   = DATA[i].d_line;
            const char *SPEC_I = DATA[i].d_spec;

            for (int j = 0; j < NUM_DATA; ++j) {
                const int LINE_J   = DATA[j].d_line;
                const char *SPEC_J = DATA[j].d_spec;

                Obj mX(&testAllocator); const Obj& X = mX;
                gg(&mX, SPEC_I);
                const Obj XX(mX);

                Obj mY(&testAllocator); const Obj& Y = mY;
                gg(&mY, SPEC_J);
                const Obj YY(mY);

                // 'swap' taking pointer (DEPRECATED)
                BEGIN_EXCEPTION_SAFE_TEST {
                    mX.swap(&mY);
                } END_EXCEPTION_SAFE_TEST(   isEqualWithCache(X, XX)
                                          && isEqualWithCache(Y, YY));

                LOOP2_ASSERT(i, j, isEqualWithCache(X, YY));
                LOOP2_ASSERT(i, j, isEqualWithCache(Y, XX));

                // 'swap' taking reference
                mX = XX;
                mY = YY;

                BEGIN_EXCEPTION_SAFE_TEST {
                    mX.swap(mY);
                } END_EXCEPTION_SAFE_TEST(   isEqualWithCache(X, XX)
                                          && isEqualWithCache(Y, YY));

                LOOP2_ASSERT(i, j, isEqualWithCache(X, YY));
                LOOP2_ASSERT(i, j, isEqualWithCache(Y, XX));

                // 'swap' free function
                mX = XX;
                mY = YY;

                BEGIN_EXCEPTION_SAFE_TEST {
                    swap(mX, mY);
                } END_EXCEPTION_SAFE_TEST(   isEqualWithCache(X, XX)
                                          && isEqualWithCache(Y, YY));

                LOOP2_ASSERT(i, j, isEqualWithCache(X, YY));
                LOOP2_ASSERT(i, j, isEqualWithCache(Y, XX));
            }
        }
      } break;
      case 21: {
        // --------------------------------------------------------------------
        // TESTING 'intersectNonBusinessDays' and 'unionBusinessDays':
        //
        // Concerns:
        //   1. Both methods are properly hooked up with the corresponding
        //      methods of the internal packed calendar object.
        //   2. Both methods are exception-safe with a guarantee of rollback.
        //
        // Plans:
        //   To address concern 1, We first create a set of *packed* calendar
        //   objects.  For each pair of these objects (P1, P2), we create a
        //   pair of calendar objects (C1, C2) from them and exercise each of
        //   the two methods to obtain the resulting calendar object C3.  Then
        //   we exercise the corresponding method of the packed calendar class
        //   on (P1, P2) to obtain the resulting packed calendar object P3.
        //   Finally we create a calendar object C3' from P3 and verify that C3
        //   and C3' have the same value.
        //
        //   To address concern 2, we use the 'BEGIN_EXCEPTION_SAFE_TEST' and
        //   'END_EXCEPTION_SAFE_TEST' macros to generate memory exceptions and
        //   verify that the value of the calendar object is rolled back when
        //   an exception is thrown.
        //
        // Testing:
        //   void intersectNonBusinessDays(const bdecs_Calendar&)
        //   void intersectNonBusinessDays(const bdecs_PackedCalendar&)
        //   void unionBusinessDays(const bdecs_Calendar&)
        //   void unionBusinessDays(const bdecs_PackedCalendar&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'intersectNonBusinessDays' and" << endl
                          << "        'unionBusinessDays'"            << endl
                          << "======================================" << endl;

        // Constructing the packed calendar objects.  We have to create them
        // manually because 'gg' cannot be used to set up packed calendars
        // here.

        bdecs_PackedCalendar packedCals[4];
        const int NUM_DATA = sizeof(packedCals) / sizeof(bdecs_PackedCalendar);

        packedCals[0].removeAll();  // calendar 0 is an empty calendar.
        packedCals[1].setValidRange(bdet_Date(2005,1,1), bdet_Date(2005,7,1));
        packedCals[1].addWeekendDay(bdet_DayOfWeek::BDET_SUN);
        packedCals[1].addWeekendDay(bdet_DayOfWeek::BDET_SAT);
        packedCals[1].addHoliday(bdet_Date(2005, 1, 1));
        packedCals[1].addHoliday(bdet_Date(2005, 6, 1));
        packedCals[1].addHolidayCode(bdet_Date(2005, 1, 1), 0);
        packedCals[1].addHolidayCode(bdet_Date(2005, 1, 1), 1000);
        packedCals[2].setValidRange(bdet_Date(2004,1,1), bdet_Date(2005,2,1));
        packedCals[2].addWeekendDay(bdet_DayOfWeek::BDET_FRI);
        bdec_DayOfWeekSet weekendDays;
        weekendDays.add(bdet_DayOfWeek::BDET_MON);
        packedCals[1].addWeekendDaysTransition(bdet_Date(2004,5,2),
                                               weekendDays);
        packedCals[2].addHoliday(bdet_Date(2004, 9, 10));
        packedCals[2].addHoliday(bdet_Date(2005, 1, 1));
        packedCals[2].addHolidayCode(bdet_Date(2005, 1, 1), 0);
        packedCals[2].addHolidayCode(bdet_Date(2005, 1, 1), 1);
        packedCals[2].addHolidayCode(bdet_Date(2005, 1, 1), 2);
        packedCals[3].setValidRange(bdet_Date(2004,12,1), bdet_Date(2005,1,1));
        weekendDays.removeAll();
        weekendDays.add(bdet_DayOfWeek::BDET_SAT);
        weekendDays.add(bdet_DayOfWeek::BDET_MON);
        packedCals[3].addWeekendDaysTransition(bdet_Date(2004,12,5),
                                               weekendDays);
        packedCals[3].addHoliday(bdet_Date(2004, 12, 25));
        packedCals[3].addHoliday(bdet_Date(2004, 12, 31));
        packedCals[3].addHolidayCode(bdet_Date(2004, 12, 31), 0);
        packedCals[3].addHolidayCode(bdet_Date(2004, 12, 31), 100);

        for (int i = 0; i < NUM_DATA; ++i) {
            for (int j = 0; j < NUM_DATA; ++j) {
                bdecs_PackedCalendar p1(packedCals[i]), p2(packedCals[j]);
                Obj cal1(p1, &testAllocator), cal2(p2, &testAllocator);
                const Obj CAL1(cal1);

                // Testing 'intersectNonBusinessDays'.
                p1.intersectNonBusinessDays(p2);
                const Obj CAL3(p1);
                BEGIN_EXCEPTION_SAFE_TEST {
                    cal1.intersectNonBusinessDays(cal2);
                } END_EXCEPTION_SAFE_TEST(isEqualWithCache(CAL1, cal1));
                LOOP2_ASSERT(i, j, isEqualWithCache(CAL3, cal1));

                // Replicate test for 'bdecs_PackedCalendar'.
                cal1 = CAL1;
                BEGIN_EXCEPTION_SAFE_TEST {
                    cal1.intersectNonBusinessDays(p2);
                } END_EXCEPTION_SAFE_TEST(isEqualWithCache(CAL1, cal1));
                LOOP2_ASSERT(i, j, isEqualWithCache(CAL3, cal1));

                // Testing 'unionBusinessDays'.
                p1 = packedCals[i];
                cal1 = CAL1;
                p1.unionBusinessDays(p2);
                const Obj CAL4(p1);
                BEGIN_EXCEPTION_SAFE_TEST {
                    cal1.unionBusinessDays(cal2);
                } END_EXCEPTION_SAFE_TEST(isEqualWithCache(CAL1, cal1));
                LOOP2_ASSERT(i, j, isEqualWithCache(CAL4, cal1));

                // Replicate test for 'bdecs_PackedCalendar'.
                cal1 = CAL1;
                BEGIN_EXCEPTION_SAFE_TEST {
                    cal1.unionBusinessDays(p2);
                } END_EXCEPTION_SAFE_TEST(isEqualWithCache(CAL1, cal1));
                LOOP2_ASSERT(i, j, isEqualWithCache(CAL4, cal1));
            }
        }
      } break;
      case 20: {
        // --------------------------------------------------------------------
        // TESTING 'intersectBusinessDays' and 'unionNonBusinessDays':
        //
        // Concerns:
        //   1. Both methods are properly hooked up with the corresponding
        //      methods of the internal packed calendar object.
        //   2. Both methods are exception-safe with a guarantee of rollback.
        //
        // Plans:
        //   To address concern 1, We first create a set of *packed* calendar
        //   objects.  For each pair of these objects (P1, P2), we create a
        //   pair of calendar objects (C1, C2) from them and exercise each of
        //   the two methods to obtain the resulting calendar object C3.  Then
        //   we exercise the corresponding method of the packed calendar class
        //   on (P1, P2) to obtain the resulting packed calendar object P3.
        //   Finally we create a calendar object C3' from P3 and verify that C3
        //   and C3' have the same value.
        //
        //   To address concern 2, we use the 'BEGIN_EXCEPTION_SAFE_TEST' and
        //   'END_EXCEPTION_SAFE_TEST' macros to generate memory exceptions and
        //   verify that the value of the calendar object is rolled back when
        //   an exception is thrown.
        //
        // Testing:
        //   void intersectBusinessDays(const bdecs_Calendar&)
        //   void unionNonBusinessDays(const bdecs_Calendar&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'intersectBusinessDays' and" << endl
                          << "        'unionNonBusinessDays'"      << endl
                          << "===================================" << endl;

        // Constructing the packed calendar objects.  We have to create them
        // manually because 'gg' cannot be used to set up packed calendars
        // here.

        bdecs_PackedCalendar packedCals[4];
        const int NUM_DATA = sizeof(packedCals) / sizeof(bdecs_PackedCalendar);

        packedCals[0].removeAll();  // calendar 0 is an empty calendar.
        packedCals[1].setValidRange(bdet_Date(2005,1,1), bdet_Date(2005,7,1));
        packedCals[1].addWeekendDay(bdet_DayOfWeek::BDET_SUN);
        packedCals[1].addWeekendDay(bdet_DayOfWeek::BDET_SAT);
        packedCals[1].addHoliday(bdet_Date(2005, 1, 1));
        packedCals[1].addHoliday(bdet_Date(2005, 6, 1));
        packedCals[1].addHolidayCode(bdet_Date(2005, 1, 1), 0);
        packedCals[1].addHolidayCode(bdet_Date(2005, 1, 1), 1000);
        packedCals[2].setValidRange(bdet_Date(2004,1,1), bdet_Date(2005,2,1));
        packedCals[2].addWeekendDay(bdet_DayOfWeek::BDET_FRI);
        bdec_DayOfWeekSet weekendDays;
        weekendDays.add(bdet_DayOfWeek::BDET_MON);
        packedCals[1].addWeekendDaysTransition(bdet_Date(2004,5,2),
                                               weekendDays);
        packedCals[2].addHoliday(bdet_Date(2004, 9, 10));
        packedCals[2].addHoliday(bdet_Date(2005, 1, 1));
        packedCals[2].addHolidayCode(bdet_Date(2005, 1, 1), 0);
        packedCals[2].addHolidayCode(bdet_Date(2005, 1, 1), 1);
        packedCals[2].addHolidayCode(bdet_Date(2005, 1, 1), 2);
        packedCals[3].setValidRange(bdet_Date(2004,12,1), bdet_Date(2005,1,1));
        weekendDays.removeAll();
        weekendDays.add(bdet_DayOfWeek::BDET_SAT);
        weekendDays.add(bdet_DayOfWeek::BDET_MON);
        packedCals[3].addWeekendDaysTransition(bdet_Date(2004,12,5),
                                               weekendDays);
        packedCals[3].addHoliday(bdet_Date(2004, 12, 25));
        packedCals[3].addHoliday(bdet_Date(2004, 12, 31));
        packedCals[3].addHolidayCode(bdet_Date(2004, 12, 31), 0);
        packedCals[3].addHolidayCode(bdet_Date(2004, 12, 31), 100);

        for (int i = 0; i < NUM_DATA; ++i) {
            for (int j = 0; j < NUM_DATA; ++j) {
                bdecs_PackedCalendar p1(packedCals[i]), p2(packedCals[j]);
                Obj cal1(p1, &testAllocator), cal2(p2, &testAllocator);
                const Obj CAL1(cal1);

                // Testing 'intersectBusinessDays'.

                p1.intersectBusinessDays(p2);
                const Obj CAL3(p1);
                BEGIN_EXCEPTION_SAFE_TEST {
                    cal1.intersectBusinessDays(cal2);
                } END_EXCEPTION_SAFE_TEST(isEqualWithCache(CAL1, cal1));
                LOOP2_ASSERT(i, j, isEqualWithCache(CAL3, cal1));

                // Replicate test for 'bdecs_PackedCalendar'.
                cal1 = CAL1;
                BEGIN_EXCEPTION_SAFE_TEST {
                    cal1.intersectBusinessDays(p2);
                } END_EXCEPTION_SAFE_TEST(isEqualWithCache(CAL1, cal1));
                LOOP2_ASSERT(i, j, isEqualWithCache(CAL3, cal1));

                // Testing 'unionNonBusinessDays'.

                p1 = packedCals[i];
                cal1 = CAL1;
                p1.unionNonBusinessDays(p2);
                const Obj CAL4(p1);
                BEGIN_EXCEPTION_SAFE_TEST {
                    cal1.unionNonBusinessDays(cal2);
                } END_EXCEPTION_SAFE_TEST(isEqualWithCache(CAL1, cal1));
                LOOP2_ASSERT(i, j, isEqualWithCache(CAL4, cal1));

                // Replicate test for 'bdecs_PackedCalendar'.
                cal1 = CAL1;
                BEGIN_EXCEPTION_SAFE_TEST {
                    cal1.unionNonBusinessDays(p2);
                } END_EXCEPTION_SAFE_TEST(isEqualWithCache(CAL1, cal1));
                LOOP2_ASSERT(i, j, isEqualWithCache(CAL4, cal1));
            }
        }
      } break;
      case 19: {
        // --------------------------------------------------------------------
        // TESTING 'removeHoliday' and 'removeHolidayCode':
        //
        // Concerns:
        //   1. These methods are properly hooked up with the corresponding
        //      methods of the internal packed calendar object.
        //   2. 'removeHoliday' properly updates the internal non-business day
        //      cache.
        //
        // Plan:
        //   To address concern 1, create a calendar object with a few holidays
        //   and holiday codes and exercise these two methods on the calendar.
        //
        //   To address concern 2, verify the internal cache using
        //   'isNonBusinessDay' each time after 'removeHoliday" is called.
        //
        // Testing:
        //   void removeHoliday(const bdet_Date&)
        //   void removeHolidayCode(const bdet_Date&, int)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'removeHoliday' and 'removeHolidayCode'"
                          << endl
                          << "==============================================="
                          << endl;

        // Call 'removeHoliday' and 'removeHolidayCode' on a non-weekend day
        // and verify that it is marked as a business day afterwards.

        Obj mX(&testAllocator); const Obj& X = mX;
        gg(&mX, "au@2005/1/1 30 0au 0A 1 2 10 15A 20BDE 25");

        bdet_Date holiday(2005, 1, 21);  // Friday
        mX.removeHolidayCode(holiday, 1);
        Obj::HolidayCodeConstIterator hcit = X.beginHolidayCodes(holiday);
        ASSERT(100 == *hcit);
        ++hcit;
        ASSERT(1000 == *hcit);
        ++hcit;
        ASSERT(X.endHolidayCodes(holiday) == hcit);
        mX.removeHoliday(holiday);
        for (Obj::HolidayConstIterator hit = X.beginHolidays(); hit !=
                                                      X.endHolidays(); ++hit) {
            LOOP_ASSERT(*hit, *hit != holiday);
        }
        ASSERT(false == X.isNonBusinessDay(holiday));

        // Call 'removeHoliday' and 'removeHolidayCode' on a weekend day and
        // verify that it is still marked as a non-business day afterwards.

        holiday.setYearMonthDay(2005, 1, 16);  // Sunday
        mX.removeHolidayCode(holiday, 0);
        hcit = X.beginHolidayCodes(holiday);
        ASSERT(X.endHolidayCodes(holiday) == hcit);
        mX.removeHoliday(holiday);
        for (Obj::HolidayConstIterator hit = X.beginHolidays(); hit !=
                                                      X.endHolidays(); ++hit) {
            LOOP_ASSERT(*hit, *hit != holiday);
        }
        ASSERT(true == X.isNonBusinessDay(holiday));  // 'holiday' is a weekend
                                                      // day.

        // Verify that if 'removeHoliday' is called with an out-of-range date,
        // the non-business day cache is unaffected.

        holiday.setYearMonthDay(2005, 12, 31);  // out-of-range date
        mX.removeHoliday(holiday);
        for (bdet_Date tempDate = X.firstDate(); tempDate <= X.lastDate();
                                                                  ++tempDate) {
            LOOP_ASSERT(tempDate, X.isNonBusinessDay(tempDate) ==
                          (X.isWeekendDay(tempDate) || X.isHoliday(tempDate)));
        }

      } break;
      case 18: {
        // --------------------------------------------------------------------
        // TESTING 'getNextBusinessDay':
        //
        // Concerns:
        //   1. Both 'getNextBusinessDay' methods return the expected results
        //      if the specified initial date is within the valid range of the
        //      calendar.
        //   2. Both 'getNextBusinessDay' methods return the expected results
        //      if the specified initial date is outside of the valid range of
        //      the calendar.
        //
        // Plan:
        //   In order to show that 'getNextBusinessDay(const bdet_Date&)' works
        //   for all valid inputs, we set up a few calendar objects with
        //   various weekend days and holidays.  Then we use all the dates that
        //   are between 7 days prior to the first date and 7 days after the
        //   last dates as test inputs for this method.  We then verify the
        //   results by either using 'isBusinessDay' if the results are within
        //   the valid range or finding the next weekend day if the result is
        //   outside of the valid range.
        //
        //   After we have verified that 'getNextBusinessDay(const bdet_Date&)'
        //   works, we apply the same test to 'getNextBusinessDay(const
        //   bdet_Date& initialDate, int n)' where we let 'n' to be from 2 to
        //   8.  Then we verify its results by calling
        //   'getNextBusinessDay(const bdet_Date&)' 'n' times.
        //
        // Testing:
        //   bdet_Date getNextBusinessDay(const bdet_Date&) const
        //   bdet_Date getNextBusinessDay(const bdet_Date&, int) const
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'getNextBusinessDay'" << endl
                          << "============================" << endl;

        struct {
            int d_line;
            const char *d_spec;
        } DATA[] = {
            { L_, "" },
            { L_, "auw" },
            { L_, "@2005/1/1" },
            { L_, "a@2005/1/1" },
            { L_, "@2005/1/1 30" },
            { L_, "au@1/1/1 30 7 9 27 28 29 30" },
            { L_, "mwf@2005/3/1 30 7 8 9 22 27 29" },
            { L_, "@2005/3/1 30 7 8 9 10 22 27 29" },
            { L_, "twrfau@2005/3/1 30 6 13 20 27" },
            { L_, "au@2005/8/1 30 0m 30w" },
            { L_, "au@2005/8/1 30 0mt 29tw" },
            { L_, "m@2005/8/1 30 1t 29tw" } ,
            { L_, "@2005/8/1 10 0mtwrfa" },
            { L_, "@1/1/1 100 0mt 30rf 60au" },
        };
        enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

        // Testing 'getNextBusinessDay(const bdet_Date&)'.

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE   = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;

            Obj X(g(SPEC), &testAllocator);
            bdet_Date startDate, endDate;

            // Setting up the test range.

            if (X.length() > 0) {
                startDate =   X.firstDate() > bdet_Date(1, 1, 7)
                            ? X.firstDate() - 7
                            : bdet_Date(1, 1, 1);
                endDate   =   X.lastDate() < bdet_Date(9999, 12, 25)
                            ? X.lastDate() + 7
                            : bdet_Date(9999, 12, 31);
            }
            else {
                startDate.setYearMonthDay(2005, 1, 1);
                endDate.setYearMonthDay(2005, 1, 1);
            }

            // Testing all dates within the test range.

            for (bdet_Date tempDate = startDate; tempDate <= endDate;
                                                                  ++tempDate) {
                bdet_Date result(tempDate);

                // Finding the next business day.

                ++result;
                while (   X.isInRange(result)  && X.isNonBusinessDay(result)
                       || !X.isInRange(result) && X.isWeekendDay(result)) {
                    ++result;
                }

                LOOP2_ASSERT(i, tempDate,
                                     X.getNextBusinessDay(tempDate) == result);
            }
        }

        // Testing 'getNextBusinessDay(const bdet_Date&, int)'.

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE   = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;

            Obj X(g(SPEC), &testAllocator);
            bdet_Date startDate, endDate;

            // Setting up the test range.

            if (X.length() > 0) {
                startDate =   X.firstDate() > bdet_Date(1, 1, 7)
                            ? X.firstDate() - 7
                            : bdet_Date(1, 1, 1);
                endDate   =   X.lastDate() < bdet_Date(9999, 12, 25)
                            ? X.lastDate() + 7
                            : bdet_Date(9999, 12, 31);
            }
            else {
                startDate.setYearMonthDay(2005, 1, 1);
                endDate.setYearMonthDay(2005, 1, 1);
            }

            // Testing all dates within the test range.

            for (bdet_Date tempDate = startDate; tempDate <= endDate;
                                                                  ++tempDate) {
                for (int n = 2; n <= 8; ++n) {
                    bdet_Date result(tempDate);

                    // Call 'getNextBusinessDay(const& bdet_Date)' 'n' times
                    // and use the result to verify
                    // 'getNextBusinessDay(const& bdet_Date, int)'.

                    for (int nn = 0; nn < n; ++nn) {
                        result = X.getNextBusinessDay(result);
                    }
                    LOOP2_ASSERT(i, tempDate,
                                  X.getNextBusinessDay(tempDate, n) == result);
                }
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING 'addWeekendDays'
        //
        // Concerns:
        //   1. 'addWeekendDays' properly adds to the set weekend days in the
        //      'bdec_DayOfWeekSet' of the default weekend-days transition
        //      maintained by the calendar object.  If the calendar object
        //      already had weekend days in the default weekend-days transition
        //      before, it should now have the union of its original weekend
        //      days and the new weekend days after this method is invoked.
        //
        // Plan:
        //   To address concern 1, we use the first 7 bits of two integers to
        //   represent all possible combinations of the weekend days that exist
        //   in the calendar and the weekend days that are being added,
        //   respectively.  For each of the 128x128 cases, we generate the
        //   weekend days set, add it to the calendar object, and verify the
        //   resulting weekend days are the combined set of old and new weekend
        //   days.
        //
        // Testing:
        //   void addWeekendDays(const bdec_DayOfWeekSet& weekendDays)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'addWeekendDays' and"       << endl
                          << "        'isWeekendDay(bdet_Date&)'" << endl
                          << "==================================" << endl;

        static const bdet_DayOfWeek::Day DAYS[] = {
            bdet_DayOfWeek::BDET_SUN,
            bdet_DayOfWeek::BDET_MON,
            bdet_DayOfWeek::BDET_TUE,
            bdet_DayOfWeek::BDET_WED,
            bdet_DayOfWeek::BDET_THU,
            bdet_DayOfWeek::BDET_FRI,
            bdet_DayOfWeek::BDET_SAT,
        };

        for (int i = 0; i <= 0x7F; ++i) {
            bdec_DayOfWeekSet set;

            // Generating the weekend days set

            for (int d = 0; d < 7; ++d) {
                if (i & (1 << d)) {
                    set.add(DAYS[d]);
                }
            }

            for (int j = 0; j <= 0x7F; ++j) {
                Obj mX(&testAllocator); const Obj& X = mX;


                ASSERT(0 == X.numWeekendDaysTransitions());

                // Set up the current weekend days before adding new weekend
                // days
                bdec_DayOfWeekSet compareSet(set);
                for (int d = 0; d < 7; ++d) {
                    if (j & (1 << d)) {
                        mX.addWeekendDay(DAYS[d]);
                        compareSet.add(DAYS[d]);
                    }
                }

                mX.addWeekendDays(set);

                // Verify that the first transition at 1/1/1 has been modified
                ASSERT(X.numWeekendDaysTransitions() == 1);
                ASSERT(X.beginWeekendDaysTransitions()->first ==
                                                           bdet_Date(1, 1, 1));
                ASSERT(X.beginWeekendDaysTransitions()->second == compareSet);

                // Verify the weekend days are properly added.

                int ij = i | j;
                for (int d = 0; d < 7; ++d) {
                    LOOP3_ASSERT(i, j, d, X.isWeekendDay(DAYS[d]) ==
                                             ((ij & (1 << d)) ? true : false));
                }

                mX.addDay(bdet_Date(2004, 1,  1));
                mX.addDay(bdet_Date(2004, 1, 31));
                for (int d = 1; d <= 31; ++d) {
                    bdet_Date date(2004, 1, d);

                    // Verify the result of 'isWeekendDay(bdet_Date&)' using
                    // 'isWeekendDay(bdet_DayOfWeek::Day)'.

                    LOOP3_ASSERT(i, j, d,
                     X.isWeekendDay(date) == X.isWeekendDay(date.dayOfWeek()));
                }
            }
        }
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'setValidRange':
        //
        // Concerns:
        //   1. 'setValidRange' is properly hooked up with the corresponding
        //      method of the internal packed calendar object.  Since this
        //      method is based on 'synchronizeCache' and packed calendar's
        //      'setValidRange', and 'synchronizeCache' has been tested in the
        //      test case for streaming, no exhaustive testing is necessary for
        //      this method.
        //   2. 'setValidRange' is exception-safe with a guarantee of rollback.
        //
        // Plan:
        //   To address concern 1, create a calendar object and exercise
        //   'setValidRange' a few times on this object.  Verify the range of
        //   the object each time after 'setValidRange' is called.
        //
        //   To address concern 2, we use the 'BEGIN_EXCEPTION_SAFE_TEST' and
        //   'END_EXCEPTION_SAFE_TEST' macros to generate memory exceptions and
        //   verify that the value of the calendar object is rolled back when
        //   an exception is thrown.
        //
        // Testing:
        //   void setValidRange(const bdet_Date&, const bdet_Date&)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'setValidRange'" << endl
                          << "=======================" << endl;

        Obj mX(&testAllocator); const Obj& X = mX;
        Obj savedCalendar;

        savedCalendar = X;
        BEGIN_EXCEPTION_SAFE_TEST {
            mX.setValidRange(bdet_Date(2005, 7, 1), bdet_Date(2005, 7, 31));
        } END_EXCEPTION_SAFE_TEST(savedCalendar == X);
        ASSERT(bdet_Date(2005, 7, 1) == X.firstDate());
        ASSERT(bdet_Date(2005, 7, 31) == X.lastDate());

        savedCalendar = X;
        BEGIN_EXCEPTION_SAFE_TEST {
            mX.setValidRange(bdet_Date(2004, 7, 1), bdet_Date(2006, 7, 31));
        } END_EXCEPTION_SAFE_TEST(savedCalendar == X);
        ASSERT(bdet_Date(2004, 7, 1) == X.firstDate());
        ASSERT(bdet_Date(2006, 7, 31) == X.lastDate());

        savedCalendar = X;
        BEGIN_EXCEPTION_SAFE_TEST {
            mX.setValidRange(bdet_Date(9999, 12, 31), bdet_Date(1, 1, 1));
        } END_EXCEPTION_SAFE_TEST(savedCalendar == X);
        ASSERT(bdet_Date(9999, 12, 31) == X.firstDate());
        ASSERT(bdet_Date(1, 1, 1) == X.lastDate());

      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING BUSINESS DAY ITERATORS
        //
        // Concerns:
        //  1. 'beginBusinessDays' return the first business day if it exists
        //     and 'endBusinessDays' returns one element past the last one.  If
        //     there is no business day, 'beginBusinessDays' must be equal to
        //     'endBusinessDays'.  In addition, 'beginBusinessDays(const
        //     bdet_Date&)' and 'endBusinessDays(const bdet_Date&)' return the
        //     first and one past the last element within the range specified
        //     by the date.
        //  2. 'operator++' and 'operator--' move the iterator forward/backward
        //     by one element.  'operator=' assigns an iterator to another one
        //     of the same type and returns a reference to the iterator.
        //  3. 'operator*' and 'operator->' return the actual value associated
        //     with the iterator.
        //  4. 'operator==' and 'operator!=' compare the values of the two
        //     iterators and return the expected result.
        //  5. The reverse iterator is working properly.  Also
        //     'rbeginBusinessDays(const bdet_Date&)' and
        //     'rendBusinessDays(const bdet_Date&)' are working properly.
        //
        // Plan:
        //  To address concern 1, we first create a set of calendar objects
        //  based on an arbitrarily selected data and exercise
        //  'beginBusinessDays' and 'endBusinessDays'.  Next, for each of these
        //  calendars objects, we remove all its holidays and weekends.
        //  Starting from the two ends of its range and progress towards the
        //  middle, we add the first and last business days in the calendar as
        //  holidays and verify the results of these two methods.  We keep
        //  doing until we have set all the days in the calendar to be
        //  holidays.  To test 'beginBusinessDays(const bdet_Date&)' and
        //  'endBusinessDays(const bdet_Date&)', we set up a calendar object
        //  with a fixed set of holidays and weekend days.  We then
        //  arbitrarily specify a set of dates (which include holidays, weekend
        //  days, and business days) as the input for these two method and
        //  verify they return the expected results.
        //
        //  To address concern 2, we call the iterator's 'operator++' and
        //  'operator--' methods and make sure it moves forward/backward as
        //  expected.  To ensure that 'operator++' allows the iterator to
        //  properly iterate over all business days, we verify, after each call
        //  to 'operator++', that the date currently being referenced is a
        //  business day and is greater than the previous business day.  We
        //  also verify that the total number of business days counted using
        //  'operator++' matches that returned by 'numBusinessDays'.  Since
        //  'beginBusinessDays' and 'endBusinessDays' have been tested
        //  previously and is proven to return correct results, all business
        //  days returned are in increasing order, and the total number of
        //  business days are correct, we know that the iterator has iterated
        //  over the correct set of business days using 'operator++'.
        //
        //  To address concern 3, we call 'operator*' and 'operator->' on an
        //  iterator pointing to a valid entry and verify that the return value
        //  matches the value of the entry.
        //
        //  To address concern 4, we first compare an iterator to itself.  Then
        //  we will compare it with another iterator that points to the same
        //  entry and make sure they are equal.  Finally we will compare the
        //  iterator with an iterator that points to a different entry and make
        //  sure they are not equal.
        //
        //  To address concern 5, we compare the results returned by the
        //  reverse iterators with the results returned by the forward
        //  iterators moving backwards and make sure they are identical.  It is
        //  not necessary to apply all the tests for the forward iterators to
        //  these reverse iterators because they are implemented as the
        //  'bsl::reverse_iterator<>' version of the forward iterators.  To
        //  test 'rbeginBusinessDays(const bdet_Date&)' and
        //  'rendBusinessDays(const bdet_Date&)', we set up a calendar object
        //  with a fixed set of holidays and weekend days.  We then arbitrarily
        //  specify a set of dates (which include holidays, weekend days, and
        //  business days) as the input for these two method and verify they
        //  return the expected results.
        //
        //  Tactics:
        //      - Ad Hoc test data selection method
        //      - Brute-Force/Loop-based/Table-Driven test case implementation
        //        techniques
        //
        //  Testing:
        //      BusinessDayConstIterator
        // --------------------------------------------------------------------
        bslma_TestAllocator testAllocator(veryVeryVerbose);

        {
            if (verbose) cout << endl
                              << "Testing business day iterators" << endl
                              << "==============================" << endl;

            typedef Obj::BusinessDayConstIterator Iterator;

            typedef bdet_Date                              IteratorValue;
                // iterator's value_type typedef
            typedef int                                    IteratorDifference;
                // iterator's difference_type typedef
            typedef bdecs_PackedCalendar_IteratorDateProxy IteratorPointer;
                // iterator's pointer typedef
            typedef bdecs_PackedCalendar_DateRef           IteratorReference;
                // iterator's reference typedef
            typedef bsl::bidirectional_iterator_tag        IteratorCategory;
                // iterator's iterator_category typedef

            // *** TYPEDEF TESTS ***

            if (veryVerbose) cout << "\ttypedef test" << endl;
            {
                typedef bsl::iterator_traits<Iterator> IterTraits;
                ASSERT((IsSame<IterTraits::value_type, IteratorValue>::VALUE));
                ASSERT((IsSame<IterTraits::difference_type,
                                                  IteratorDifference>::VALUE));
                ASSERT((IsSame<IterTraits::pointer, IteratorPointer>::VALUE));
                ASSERT((IsSame<IterTraits::reference,
                                                   IteratorReference>::VALUE));
                ASSERT((IsSame<IterTraits::iterator_category,
                                                    IteratorCategory>::VALUE));
            }
            {
                // Testing 'beginBusinessDays' and 'endBusinessDays'.

                static const struct  {
                    int d_lineNumber;
                    const char *d_input;
                    const int d_begin;
                    const int d_end;   // A value of 0 indicates an 'end'
                                       // iterator for both 'd_begin' and
                                       // 'd_end'.
                } DATA[] = {
                    //LINE, INPUT,                     BEGIN,    END
                    {L_,    "",                               0,        0},
                    {L_,    "a",                              0,        0},
                    {L_,    "@1/1/1 30",                  10101,    10131},
                    {L_,    "au@2005/7/2",                    0,        0},
                    {L_,    "au@2005/8/1 30",          20050801, 20050831},
                    {L_,    "aut@2005/5/1 30",         20050502, 20050530},
                    {L_,    "@2005/2/1 27 26",         20050201, 20050228},
                    {L_,    "au@2005/1/1 30 0 17 30",  20050103, 20050128},
                    {L_,    "au@2005/4/1 29 26 27 28", 20050401, 20050426},
                    {L_,    "fau@2005/6/3 7 3 4 5 6",         0,        0},
                    {L_,    "au@2005/8/1 30 0m 30w",   20050802, 20050830},
                    {L_,    "au@2005/8/1 30 0mt 29tw", 20050803, 20050828},
                    {L_,    "m@2005/8/1 30 1t 29tw",   20050803, 20050829},
                    {L_,    "@2005/8/1 10 0mtwrfau",          0,        0},
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int   LINE  = DATA[ti].d_lineNumber;
                    const char *INPUT = DATA[ti].d_input;
                    const int   BEGIN = DATA[ti].d_begin;
                    const int   END   = DATA[ti].d_end;

                    if (veryVeryVerbose) {
                        T_; T_; P(INPUT);
                        T_; T_; P(BEGIN);
                        T_; T_; P(END);
                    }

                    Obj mX(&testAllocator); const Obj& X = mX;
                    gg(&mX, INPUT);

                    if (BEGIN) {
                        bdet_Date mDBEGIN = bdet_Date(BEGIN / 10000,
                                                      BEGIN / 100 % 100,
                                                      BEGIN % 100);
                        bdet_Date mDEND   = bdet_Date(END   / 10000,
                                                      END   / 100 % 100,
                                                      END   % 100);
                        const bdet_Date& DBEGIN = mDBEGIN;
                        const bdet_Date& DEND   = mDEND;
                        LOOP3_ASSERT(ti, DBEGIN, *X.beginBusinessDays(),
                                             DBEGIN == *X.beginBusinessDays());
                        Iterator it = X.endBusinessDays();
                        --it;
                        LOOP3_ASSERT(ti, DEND, *it, DEND == *it);

                        // Reset the calendar object.  Then, start from the two
                        // ends and progress towards the middle of the
                        // calendar, make each day a holiday and verify the
                        // results of 'beginBusinessDays' and
                        // 'endBusinessDays'.

                        mX.removeAll();
                        mX.addDay(DBEGIN);
                        mX.addDay(DEND);
                        while (mDEND >= mDBEGIN) {
                            mX.addHoliday(DBEGIN);
                            if (DBEGIN != DEND) {
                                mX.addHoliday(DEND);
                            }
                            if (DEND - DBEGIN > 1) {
                                LOOP3_ASSERT(ti, DBEGIN, DEND,
                                          ++mDBEGIN == *X.beginBusinessDays());
                                Iterator it = X.endBusinessDays();
                                --it;
                                LOOP3_ASSERT(ti, DBEGIN, DEND, --mDEND == *it);
                            }
                            else {
                                LOOP3_ASSERT(ti, DBEGIN, DEND,
                                 X.beginBusinessDays() == X.endBusinessDays());
                                ++mDBEGIN;
                                --mDEND;
                            }
                        }
                    }
                    else {
                        LOOP_ASSERT(ti,
                                 X.beginBusinessDays() == X.endBusinessDays());
                    }
                }
            }
            {
                // TESTING 'beginBusinessDays(const bdet_Date&)' and
                // 'endBusinessDays(const bdet_Date&)'

                static const char SPEC[] =
                    "au@2005/1/1 89 5 27 30 31 32 33 34 88 89";

                static const struct  {
                    int d_lineNumber;
                    const int d_date;
                    const int d_begin;
                    const int d_end;   // A value of 0 indicates an 'end'
                                       // iterator for both 'd_begin' and
                                       // 'd_end'.
                } DATA[] = {
                    //LINE, DATE,      BEGIN,     END
                    {L_,    20050104, 20050104, 20050105},
                    {L_,    20050105, 20050105, 20050107},
                    {L_,    20050106, 20050107, 20050107},
                    {L_,    20050114, 20050114, 20050117},
                    {L_,    20050128, 20050207, 20050207},
                    {L_,    20050202, 20050207, 20050207},
                    {L_,    20050206, 20050207, 20050207},
                    {L_,    20050207, 20050207, 20050208},
                    {L_,    20050329, 20050329,        0},
                    {L_,    20050330,        0,        0},
                    {L_,    20050331,        0,        0},
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                Obj mX(&testAllocator);
                const Obj& X = mX;
                ASSERT(X.beginBusinessDays() == X.beginBusinessDays());
                ASSERT(X.endBusinessDays() == X.endBusinessDays());
                ASSERT(X.beginBusinessDays() == X.endBusinessDays());
                gg(&mX, SPEC);

                for (int nb = 0; nb < NUM_DATA; ++nb) {
                    const bdet_Date DATE = bdet_Date(
                                                   DATA[nb].d_date / 10000,
                                                   DATA[nb].d_date / 100 % 100,
                                                   DATA[nb].d_date % 100);

                    if (DATA[nb].d_begin != 0) {
                        const bdet_Date BZDAY = bdet_Date(
                                                  DATA[nb].d_begin / 10000,
                                                  DATA[nb].d_begin / 100 % 100,
                                                  DATA[nb].d_begin % 100);
                        LOOP3_ASSERT(nb, *X.beginBusinessDays(DATE), BZDAY,
                                         *X.beginBusinessDays(DATE) == BZDAY);
                    }
                    else {
                        LOOP_ASSERT(nb, X.beginBusinessDays(DATE) ==
                                                         X.endBusinessDays());
                    }

                    if (DATA[nb].d_end != 0) {
                        const bdet_Date BZDAY2 = bdet_Date(
                                                    DATA[nb].d_end / 10000,
                                                    DATA[nb].d_end / 100 % 100,
                                                    DATA[nb].d_end % 100);
                        LOOP3_ASSERT(nb, *X.endBusinessDays(DATE), BZDAY2,
                                         *X.endBusinessDays(DATE) == BZDAY2);
                    }
                    else {
                        LOOP_ASSERT(nb, X.endBusinessDays(DATE) ==
                                                         X.endBusinessDays());
                    }
                }
            }
            {
                // TESTING 'rbeginBusinessDays(const bdet_Date&)' and
                // 'rendBusinessDays(const bdet_Date&)'

                static const char SPEC[] =
                    "au@2005/1/1 89 5 27 30 31 32 33 34 88 89";

                static const struct  {
                    int d_lineNumber;
                    const int d_date;
                    const int d_begin;
                    const int d_end;  // A value of 0 indicates an 'end'
                                      // iterator for both 'd_begin' and
                                      // 'd_end'.
                } DATA[] = {
                    //LINE, DATE,     BEGIN,    END
                    {L_,    20050101,        0,        0},
                    {L_,    20050102,        0,        0},
                    {L_,    20050103, 20050103,        0},
                    {L_,    20050104, 20050104, 20050103},
                    {L_,    20050106, 20050105, 20050105},
                    {L_,    20050107, 20050107, 20050105},
                    {L_,    20050117, 20050117, 20050114},
                    {L_,    20050128, 20050127, 20050127},
                    {L_,    20050202, 20050127, 20050127},
                    {L_,    20050206, 20050127, 20050127},
                    {L_,    20050207, 20050207, 20050127},
                    {L_,    20050208, 20050208, 20050207},
                    {L_,    20050329, 20050329, 20050328},
                    {L_,    20050330, 20050329, 20050329},
                    {L_,    20050331, 20050329, 20050329},
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                Obj mX(&testAllocator);
                const Obj& X = mX;
                ASSERT(X.rbeginBusinessDays() == X.rbeginBusinessDays());
                ASSERT(X.rendBusinessDays() == X.rendBusinessDays());
                ASSERT(X.rbeginBusinessDays() == X.rendBusinessDays());
                gg(&mX, SPEC);

                for (int nb = 0; nb < NUM_DATA; ++nb) {
                    const bdet_Date DATE = bdet_Date(
                                                   DATA[nb].d_date / 10000,
                                                   DATA[nb].d_date / 100 % 100,
                                                   DATA[nb].d_date % 100);

                    if (DATA[nb].d_begin != 0) {
                        const bdet_Date BZDAY = bdet_Date(
                                                  DATA[nb].d_begin / 10000,
                                                  DATA[nb].d_begin / 100 % 100,
                                                  DATA[nb].d_begin % 100);
                        LOOP3_ASSERT(nb, *X.rbeginBusinessDays(DATE), BZDAY,
                                         *X.rbeginBusinessDays(DATE) == BZDAY);
                    }
                    else {
                        LOOP_ASSERT(nb, X.rbeginBusinessDays(DATE) ==
                                                         X.rendBusinessDays());
                    }

                    if (DATA[nb].d_end != 0) {
                        const bdet_Date BZDAY2 = bdet_Date(
                                                    DATA[nb].d_end / 10000,
                                                    DATA[nb].d_end / 100 % 100,
                                                    DATA[nb].d_end % 100);
                        LOOP3_ASSERT(nb, *X.rendBusinessDays(DATE), BZDAY2,
                                         *X.rendBusinessDays(DATE) == BZDAY2);
                    }
                    else {
                        LOOP_ASSERT(nb, X.rendBusinessDays(DATE) ==
                                                         X.rendBusinessDays());
                    }
                }
            }
            {
                static const char *SPECS[] = {
                    "", "a", "@1/1/1 30", "@9999/12/1 30",

                    "@2005/1/1",               // one business day in calendar
                    "@2005/1/1 0 0",           // one holiday in calendar
                    "a@2005/1/1",              // one weekend day in calendar

                    "@2005/1/1 1",             // two business day in calendar
                    "@2005/1/1 1 0 1",         // two holiday in calendar
                    "au@2005/1/1 1",           // two weekend day in calendar
                    "@2005/1/1 1 0",           // one holiday, one business day
                    "@2005/1/1 1 1",           // one business day, one holiday
                    "a@2005/1/1 1",            // one weekend day, one business
                                               // day
                    "u@2005/1/1 1",            // one business day, one weekend
                                               // day
                    "a@2005/1/1 1 1",          // one weekend day, one holiday
                    "u@2005/1/1 1 0",          // one holiday, one weekend day

                    "a@2005/1/1 6",            // first and last days are
                                               // weekend days
                    "@2005/1/1 9 0 9",         // first and last days are
                                               // holidays
                    "@2005/1/1 9 1 8",         // first and last days are
                                               // business days

                    "au@2005/1/1 7",           // first 2 days and last 2 days
                                               // are weekend days
                    "@2005/1/1 9 0 1 8 9",     // first 2 days and last 2 days
                                               // are holidays
                    "@2005/1/1 9 2 7",         // first 2 days and last 2 days
                                               // are business days

                    "aum@2005/1/1 9 3 4 5 6",  // only holidays and weekends
                    "@2005/1/1 5 0 1 2 3 4 5", // all holidays

                    "wau@2005/12/1 60 4 5 28 29 30 31 32 33",
                        // a calendar with contiguous weekend days and holidays

                    "mtwfau@1997/7/1 60 5 6A 19B 20 21 34 35C 36 37D 40 58E",
                        // a calendar with only one week day in a week

                    // some calendars with more than one weekend-days
                    // transition
                    "mu@2005/1/1 30 2t 15wr 30fa",
                    "au@2005/1/1 20 2wt 5f 9u",
                0};

                for (int i = 0; SPECS[i]; ++i) {
                    Obj mX(&testAllocator); const Obj& X = mX;
                    gg(&mX, SPECS[i]);

                    {
                        int count = 0;
                        Iterator it = X.beginBusinessDays();
                        while (it != X.endBusinessDays()) {
                            ++count;

                            LOOP2_ASSERT(i, *it, X.isInRange(*it));
                            LOOP2_ASSERT(i, *it, X.isBusinessDay(*it));

                            // copy construction

                            Iterator it2(it);
                            LOOP_ASSERT(i, it2 != X.endBusinessDays());
                            LOOP3_ASSERT(i, *it, *it2, *it == *it2);

                            // 'operator++'

                            Iterator it3(++it);
                            if (it != X.endBusinessDays()) {
                                LOOP3_ASSERT(i, *it, *it3, *it == *it3);

                                // Make sure the business days returned are in
                                // chronological order.

                                LOOP3_ASSERT(i, *it, *it2, *it >  *it2);
                            }
                            else {
                                LOOP_ASSERT(i, it == it3);
                            }
                        }
                        LOOP3_ASSERT(i, count, X.numBusinessDays(),
                                                 count == X.numBusinessDays());
                    }

                    // 'operator==' and 'operator!='

                    int nb = X.numBusinessDays();

                    // Note that in the double loop below, each loop runs
                    // 'nb' + 1 times because we compare the 'nb' iterators
                    // referencing the 'nb' business days plus the end
                    // iterator.

                    {
                        Iterator it = X.beginBusinessDays();
                        for (int c = 0; c <= nb; ++c) {
                            Iterator it2 = X.beginBusinessDays();
                            for (int d = 0; d <= nb; ++d) {

                                // The two tests are necessary because
                                // 'operator!=' is NOT implemented as "return
                                // !(lhs == rhs);".

                                if (c == d) {
                                    LOOP3_ASSERT(i, c, d,   it == it2);
                                    LOOP3_ASSERT(i, c, d, !(it != it2));
                                }
                                else {
                                    LOOP3_ASSERT(i, c, d,   it != it2);
                                    LOOP3_ASSERT(i, c, d, !(it == it2));
                                }
                                if (d < nb) {
                                    ++it2;
                                }
                            }
                            if (c < nb) {
                                ++it;
                            }
                        }
                    }

                    // assignment operator

                    {
                        Iterator it = X.beginBusinessDays();
                        for (int c = 0; c < nb; ++c) {
                            Iterator it2 = X.beginBusinessDays();
                            for (int d = 0; d < nb; ++d) {
                                Iterator it3(it);

                                it = it;
                                LOOP3_ASSERT(i, c, d, it3 == it);

                                it3 = it2;
                                LOOP3_ASSERT(i, c, d, it3 == it2);

                                ++it2;
                            }
                            ++it;
                        }
                    }

                    // 'operator->'

                    Iterator it = X.beginBusinessDays();
                    for (; it != X.endBusinessDays(); ++it) {
                        LOOP_ASSERT(i, (*it).month() == it->month());
                    }

                    // post-increment operator test

                    it = X.beginBusinessDays();
                    for (int c = 0; c < nb; ++c) {
                        LOOP2_ASSERT(i, c, it != X.endBusinessDays());
                        LOOP3_ASSERT(i, c, *it, X.isInRange(*it));
                        LOOP3_ASSERT(i, c, *it, X.isBusinessDay(*(it++)));
                    }
                    LOOP_ASSERT(i, it == X.endBusinessDays());

                    // *** NON-MUTATING BIDIRECTIONAL ITERATOR TESTS ***

                    if (veryVerbose) cout << "\tnon-mutating bidirectional "
                                          << "iterator tests" << endl;

                    // pre-decrement operator test

                    int count = 0;
                    it = X.endBusinessDays();
                    while (it != X.beginBusinessDays()) {

                        // 'operator--'

                        Iterator it2(--it);
                        LOOP3_ASSERT(i, *it, *it2, *it == *it2);

                        ++count;
                        LOOP2_ASSERT(i, *it, X.isInRange(*it));
                        LOOP2_ASSERT(i, *it, X.isBusinessDay(*it));
                    }
                    LOOP3_ASSERT(i, count, X.numBusinessDays(),
                                                 count == X.numBusinessDays());

                    // post-decrement operator test

                    it = X.endBusinessDays();
                    // Note that the loop below runs 'nb' + 1 times because we
                    // compare the 'nb' iterators referencing the 'nb' business
                    // days plus the end iterator.

                    for (int c = 0; c <= nb; ++c) {
                        Iterator it2(it);
                        if (it != X.beginBusinessDays()) {
                            Iterator it3(it--);
                            LOOP2_ASSERT(i, c, it3 == it2);
                        }
                        if (it2 != X.endBusinessDays()) {
                            LOOP3_ASSERT(i, c, *it2, X.isInRange(*it2));
                            LOOP3_ASSERT(i, c, *it2, X.isBusinessDay(*it2));
                        }
                    }
                    LOOP_ASSERT(i, it == X.beginBusinessDays());

                    // *** REVERSE ITERATOR TESTS ***
                    // The following tests are very simple because
                    // 'BusinessDayConstReverseIterator' is implemented using a
                    // TESTED bsl::reserse_iterator<Iterator>-like template.

                    if (veryVerbose) cout << "\treverse iterator tests"
                                          << endl;

                    typedef Obj::BusinessDayConstReverseIterator
                                                               ReverseIterator;

                    if (i == 0) {
                        LOOP_ASSERT(i, X.rbeginBusinessDays() ==
                                                         X.rendBusinessDays());
                    }
                    else {
                        Iterator        it = X.endBusinessDays();
                        ReverseIterator ri = X.rbeginBusinessDays();

                        for (; ri != X.rendBusinessDays(); ++ri) {
                            --it;
                            LOOP_ASSERT(i, *it == *ri);
                            LOOP_ASSERT(i, it->year() == ri->year());
                        }
                        LOOP_ASSERT(i, X.beginBusinessDays() == it);

                        for (ri = X.rendBusinessDays(),
                             it = X.beginBusinessDays();
                                             it != X.endBusinessDays(); ++it) {
                            --ri;
                            LOOP_ASSERT(i, *it == *ri);
                            LOOP_ASSERT(i, it->year() == ri->year());
                        }
                        LOOP_ASSERT(i, X.rbeginBusinessDays() == ri);
                    }
                }
            }
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'isBusinessDay' and 'isNonBusinessDay':
        //
        // Concern:
        //   1. We wnat to verify that 'isNonBusinessDay' returns the correct
        //      value for all possible dates, including boundary dates.
        //
        //   2. We want to verify that 'isBusinessDay' is properly implemented
        //      using 'isNonBusinessDay'.
        //
        // Plan:
        //   To address concern 1, we create a list of calendars including
        //   those that have ranges with a first date of 1/1/1 and those with a
        //   last date of 9999/12/31.  Verify, for each date in the each
        //   calendar's valid range, the result of 'isNonBusinessDay' is
        //   consistent with those of 'isWeekendDay' and 'isHoliday'.
        //
        //   To address concern 2, we create a calendar with a few holidays
        //   and weekend days and verify that 'isBusinessDay' returns the
        //   expected results.
        //
        // Testing:
        //    bool isNonBusinessDay(const bdet_Date& date) const
        //    bool isBusinessDay(const bdet_Date& date) const
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'isBusinessDay'" << endl
                          << "=======================" << endl;

        struct {
            int d_line;
            const char *d_spec;
        } DATA[] = {
            { L_, "" },
            { L_, "auw" },
            { L_, "@2005/1/1" },
            { L_, "a@2005/1/1" },
            { L_, "@2005/1/1 30" },
            { L_, "au@1/1/1 30 7 9 27 28 29 30" },
            { L_, "mwf@2005/3/1 30 7 8 9 22 27 29" },
            { L_, "@2005/3/1 30 7 8 9 10 22 27 29" },
            { L_, "twrfau@2005/3/1 30 6 13 20 27" },
            { L_, "au@2005/8/1 30 0m 30w" },
            { L_, "au@2005/8/1 30 0mt 29tw" },
            { L_, "m@2005/8/1 30 1t 29tw" } ,
            { L_, "@2005/8/1 10 0mtwrfa" },
            { L_, "@1/1/1 100 0au 30rf 60au" },
            { L_, "@9999/10/1 @9999/12/31 @9999/10/1mt 30fu" },
        };
        enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

        // Testing 'getNextBusinessDay(const bdet_Date&)'.

        for (int i = 0; i < NUM_DATA; ++i) {
            const int LINE   = DATA[i].d_line;
            const char *SPEC = DATA[i].d_spec;

            Obj X(g(SPEC), &testAllocator);

            for (bdet_Date tempDate = X.firstDate(); tempDate <= X.lastDate();
                                                                  ++tempDate) {
                bool weekendDayFlag = X.isWeekendDay(tempDate);
                bool holidayFlag = X.isHoliday(tempDate);

                bool nonBusinessDayFlag = X.isNonBusinessDay(tempDate);

                if (veryVerbose) {
                    P(X);
                    P_(tempDate); P_(weekendDayFlag); P_(holidayFlag);
                    P(nonBusinessDayFlag);
                }

                LOOP3_ASSERT(weekendDayFlag, holidayFlag, nonBusinessDayFlag,
                             nonBusinessDayFlag == weekendDayFlag ||
                                                                  holidayFlag);
            }
        }

        Obj mX(bdet_Date(2005,1,1), bdet_Date(2005,1,31), &testAllocator);
        const Obj& X = mX;
        mX.addHoliday(bdet_Date(2005, 1, 7));
        mX.addWeekendDay(bdet_DayOfWeek::BDET_SAT);
        ASSERT(false == mX.isBusinessDay(bdet_Date(2005, 1, 7)));
        ASSERT(false == mX.isBusinessDay(bdet_Date(2005, 1, 8)));
        ASSERT(true  == mX.isBusinessDay(bdet_Date(2005, 1, 9)));

      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'numHolidays' and 'numHolidayCodes':
        //
        // Concern:
        //   We want to verify that these methods have been properly hooked up
        //   with the corresponding methods of the internal packed calendar
        //   object.
        //
        // Plan:
        //   To address this concern, we create a calendar object and add a
        //   few holidays with some holiday codes and verify the results
        //   returned by these two methods.
        //
        // Testing:
        //    int numHolidays() const
        //    int numHolidayCodes(const bdet_Date& date) const
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing numHolidays and numHolidayCodes" << endl
                          << "=======================================" << endl;

        Obj mX(bdet_Date(2005,1,1), bdet_Date(2005,1,31), &testAllocator);
        const Obj& X = mX;

        ASSERT(0 == X.numHolidays());
        bdet_Date holiday(2005,1,1);
        mX.addHoliday(holiday);
        ASSERT(1 == X.numHolidays());
        ASSERT(0 == X.numHolidayCodes(holiday));
        mX.addHolidayCode(holiday, 1);
        ASSERT(1 == X.numHolidayCodes(holiday));
        mX.addHolidayCode(holiday, 9);
        ASSERT(2 == X.numHolidayCodes(holiday));
        holiday.setYearMonthDay(2005,1,15);
        mX.addHoliday(holiday);
        ASSERT(2 == X.numHolidays());
        ASSERT(0 == X.numHolidayCodes(holiday));
        mX.addHolidayCode(holiday, 5);
        ASSERT(1 == X.numHolidayCodes(holiday));

      } break;
      case 12: {
        // --------------------------------------------------------------------
        // TESTING 'numWeekendDaysInRange':
        //
        // Concern:
        //   We want to verify that the method have been properly hooked up
        //   with the corresponding method of the internal packed calendar
        //   object.
        //
        // Plan:
        //   To address this concern, we set up a calendar with a few
        //   weekend-days transitions and verify the results returned by the
        //   method.
        //
        // Testing:
        //    int numWeekendDaysInRange() const
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing numWeekendDaysInRange" << endl
                          << "=============================" << endl;

        Obj mX(bdet_Date(2005, 1, 1), bdet_Date(2005, 1, 31), &testAllocator);
        const Obj& X = mX;

        ASSERT(0 == X.numWeekendDaysInRange());
        ASSERT(0 == X.numWeekendDaysTransitions());

        mX.addWeekendDay(bdet_DayOfWeek::BDET_SUN);
        ASSERT(1 == numWeekendDaysInFirstTransition(X));
        ASSERT(1 == X.numWeekendDaysTransitions());

        bdec_DayOfWeekSet weekendDays;
        weekendDays.add(bdet_DayOfWeek::BDET_MON);
        weekendDays.add(bdet_DayOfWeek::BDET_TUE);
        mX.addWeekendDaysTransition(bdet_Date(2005, 1, 7), weekendDays);
        ASSERT(2 == X.numWeekendDaysTransitions());

        int weekendDayCount = 0;
        for (bdet_Date tempDate = X.firstDate(); tempDate <= X.lastDate();
                                                                  ++tempDate) {
            if (X.isWeekendDay(tempDate)) {
                ++weekendDayCount;
            }
        }
        ASSERT(X.numWeekendDaysInRange() == weekendDayCount);

        weekendDays.removeAll();
        weekendDays.add(bdet_DayOfWeek::BDET_FRI);
        weekendDays.add(bdet_DayOfWeek::BDET_SAT);
        mX.addWeekendDaysTransition(bdet_Date(2005, 1, 19), weekendDays);
        ASSERT(3 == X.numWeekendDaysTransitions());

        weekendDayCount = 0;
        for (bdet_Date tempDate = X.firstDate(); tempDate <= X.lastDate();
                                                                  ++tempDate) {
            if (X.isWeekendDay(tempDate)) {
                ++weekendDayCount;
            }
        }
        ASSERT(X.numWeekendDaysInRange() == weekendDayCount);
      } break;
      case 11: {
        // --------------------------------------------------------------------
        // TESTING INITIAL-VALUE CONSTRUCTORS:
        //
        // Concerns:
        //   1. The initial value is correct.
        //   2. The constructor is exception neutral w.r.t. memory allocation.
        //   3. The internal memory management system is hooked up properly so
        //      that *all* internally allocated memory (if any) draws from a
        //      user-supplied allocator whenever one is specified.
        //
        // Plan:
        //   For each constructor we will create objects
        //     - Without passing in an allocator.  We will install an allocator
        //       as the default and verify that memory is drawn from it.
        //     - With an allocator.  We will install an allocator as the
        //       default and verify that memory is drawn from the allocator
        //       passed in and not from the default allocator.
        //     - In the presence of exceptions during memory allocations using
        //       a 'bslma_TestAllocator' and varying its *allocation* *limit*.
        //     - Where the object is constructed entirely in static memory
        //       (using a 'bslma_BufferAllocator') and never destroyed.
        //
        //   We will use basic accessors to verify the first and last date of
        //   this object.  We will also verify that this object has no holiday
        //   or weekend day.
        //
        // Testing:
        //   bdecs_Calendar::bdecs_Calendar(const bdet_Date&,
        //                                  const bdet_Date&,
        //                                  bslma_Allocator *)
        //   bdecs_Calendar::bdecs_Calendar(const bdecs_PackedCalendar&,
        //                                  bslma_Allocator *)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Initial-Value Constructors" << endl
                          << "==================================" << endl;

        static const struct {
            int d_lineNumber;
            int d_first;      // first day of the calendar in the format
                              // of YYYYMMDD
            int d_last;       // last day of the calendar in the format
                              // of YYYYMMDD
        } DATA[] = {
            //LINE, FIRST,    LAST
            { L_,   20050102, 20050101},
            { L_,   20050102, 20050102},
            { L_,      10101, 99991231},
            { L_,   20000102, 20000229},
            { L_,   20050101, 20060101},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\tWithout passing in an allocator." << endl;
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int DFIRST = DATA[i].d_first;
                const int DLAST  = DATA[i].d_last;

                bdet_Date dateFirst(DFIRST / 10000, DFIRST / 100 % 100,
                                    DFIRST % 100);
                bdet_Date dateLast (DLAST  / 10000, DLAST  / 100 % 100,
                                    DLAST  % 100);

                bslma_TestAllocator da; // default allocator
                const bslma_DefaultAllocatorGuard DAG(&da);
                {
                    // Testing bdecs_Calendar(const bdet_Date&,
                    //                        const bdet_Date&,
                    //                        bslma_Allocator *).

                    int blocks = da.numBlocksTotal();
                    const Obj X(dateFirst, dateLast);

                    // Verify that memory is drawn from the default allocator.

                    LOOP_ASSERT(i, blocks <= da.numBlocksTotal());

                    if (veryVerbose) P(X);
                    if (dateFirst > dateLast) {
                        LOOP_ASSERT(i, X.firstDate() ==
                                                      bdet_Date(9999, 12, 31));
                        LOOP_ASSERT(i, X.lastDate()  ==
                                                      bdet_Date(   1,  1,  1));
                    }
                    else {
                        LOOP_ASSERT(i, X.firstDate() == dateFirst);
                        LOOP_ASSERT(i, X.lastDate()  == dateLast );
                    }
                    LOOP_ASSERT(i, X.beginHolidays() == X.endHolidays());
                    LOOP_ASSERT(i, 0 == X.numWeekendDaysTransitions());
                }
                {
                    // Testing bdecs_Calendar(const bdecs_PackedCalendar&,
                    //                        bslma_Allocator *).

                    int blocks = da.numBlocksTotal();
                    bdecs_PackedCalendar packedCal(dateFirst, dateLast);
                    const Obj X(packedCal);

                    // Verify that memory is drawn from the default allocator.

                    LOOP_ASSERT(i, blocks <= da.numBlocksTotal());

                    if (veryVerbose) P(X);
                    if (dateFirst > dateLast) {
                        LOOP_ASSERT(i, X.firstDate() ==
                                                      bdet_Date(9999, 12, 31));
                        LOOP_ASSERT(i, X.lastDate()  ==
                                                      bdet_Date(   1,  1,  1));
                    }
                    else {
                        LOOP_ASSERT(i, X.firstDate() == dateFirst);
                        LOOP_ASSERT(i, X.lastDate()  == dateLast );
                    }
                    LOOP_ASSERT(i, X.beginHolidays() == X.endHolidays());
                    LOOP_ASSERT(i, 0 == X.numWeekendDaysTransitions());
                }
            }
        }
        if (verbose) cout << "\tPassing in an allocator." << endl;

        if (verbose) cout << "\t\tWith no exceptions." << endl;
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int DFIRST = DATA[i].d_first;
                const int DLAST  = DATA[i].d_last;

                bdet_Date dateFirst(DFIRST / 10000, DFIRST / 100 % 100,
                                    DFIRST % 100);
                bdet_Date dateLast (DLAST  / 10000, DLAST  / 100 % 100,
                                    DLAST  % 100);

                bslma_TestAllocator da; // default allocator
                const bslma_DefaultAllocatorGuard DAG(&da);
                {
                    // Testing bdecs_Calendar(const bdet_Date&,
                    //                        const bdet_Date&,
                    //                        bslma_Allocator *).

                    int blocks = testAllocator.numBlocksTotal();
                    int blocksDefault = da.numBlocksTotal();
                    const Obj X(dateFirst, dateLast, &testAllocator);

                    // Verify that memory is drawn from the specified allocator
                    // and not from the default allocator.

                    LOOP_ASSERT(i, blocks <= testAllocator.numBlocksTotal());
                    LOOP_ASSERT(i, blocksDefault == da.numBlocksTotal());

                    if (veryVerbose) P(X);
                    if (dateFirst > dateLast) {
                        LOOP_ASSERT(i, X.firstDate() ==
                                                      bdet_Date(9999, 12, 31));
                        LOOP_ASSERT(i, X.lastDate()  ==
                                                      bdet_Date(   1,  1,  1));
                    }
                    else {
                        LOOP_ASSERT(i, X.firstDate() == dateFirst);
                        LOOP_ASSERT(i, X.lastDate()  == dateLast );
                    }
                    LOOP_ASSERT(i, X.beginHolidays() == X.endHolidays());
                    LOOP_ASSERT(i, 0 == X.numWeekendDaysTransitions());
                }
                {
                    // Testing bdecs_Calendar(const bdecs_PackedCalendar&,
                    //                        bslma_Allocator *).

                    int blocks = testAllocator.numBlocksTotal();
                    int blocksDefault = da.numBlocksTotal();
                    bdecs_PackedCalendar packedCal(dateFirst, dateLast,
                                                               &testAllocator);
                    const Obj X(packedCal, &testAllocator);

                    // Verify that memory is drawn from the specified allocator
                    // and not from the default allocator.

                    LOOP_ASSERT(i, blocks <= testAllocator.numBlocksTotal());
                    LOOP_ASSERT(i, blocksDefault == da.numBlocksTotal());

                    if (veryVerbose) P(X);
                    if (dateFirst > dateLast) {
                        LOOP_ASSERT(i, X.firstDate() ==
                                                      bdet_Date(9999, 12, 31));
                        LOOP_ASSERT(i, X.lastDate()  ==
                                                      bdet_Date(   1,  1,  1));
                    }
                    else {
                        LOOP_ASSERT(i, X.firstDate() == dateFirst);
                        LOOP_ASSERT(i, X.lastDate()  == dateLast );
                    }
                    LOOP_ASSERT(i, X.beginHolidays() == X.endHolidays());
                    LOOP_ASSERT(i, 0 == X.numWeekendDaysTransitions());
                }
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            for (int i = 0; i < NUM_DATA; ++i) {
                const int DFIRST = DATA[i].d_first;
                const int DLAST  = DATA[i].d_last;

                bdet_Date dateFirst(DFIRST / 10000, DFIRST / 100 % 100,
                                    DFIRST % 100);
                bdet_Date dateLast (DLAST  / 10000, DLAST  / 100 % 100,
                                    DLAST  % 100);
                // Testing bdecs_Calendar(const bdet_Date&,
                //                        const bdet_Date&,
                //                        bslma_Allocator *).

                BEGIN_BSLMA_EXCEPTION_TEST {
                    Obj mX(dateFirst, dateLast, &testAllocator);
                    const Obj& X = mX;
                    if (veryVerbose) P(X);
                    if (dateFirst > dateLast) {
                        LOOP_ASSERT(i, X.firstDate() ==
                                                      bdet_Date(9999, 12, 31));
                        LOOP_ASSERT(i, X.lastDate()  ==
                                                      bdet_Date(   1,  1,  1));
                    }
                    else {
                        LOOP_ASSERT(i, X.firstDate() == dateFirst);
                        LOOP_ASSERT(i, X.lastDate()  == dateLast );
                    }
                    LOOP_ASSERT(i, X.beginHolidays() == X.endHolidays());
                    LOOP_ASSERT(i, 0 == X.numWeekendDaysTransitions());
                } END_BSLMA_EXCEPTION_TEST

                // Testing bdecs_Calendar(const bdecs_PackedCalendar&,
                //                        bslma_Allocator *).

                BEGIN_BSLMA_EXCEPTION_TEST {
                    bdecs_PackedCalendar packedCal(dateFirst, dateLast,
                                                               &testAllocator);
                    const Obj X(packedCal, &testAllocator);
                    if (veryVerbose) P(X);
                    if (dateFirst > dateLast) {
                        LOOP_ASSERT(i, X.firstDate() ==
                                                      bdet_Date(9999, 12, 31));
                        LOOP_ASSERT(i, X.lastDate()  ==
                                                      bdet_Date(   1,  1,  1));
                    }
                    else {
                        LOOP_ASSERT(i, X.firstDate() == dateFirst);
                        LOOP_ASSERT(i, X.lastDate()  == dateLast );
                    }
                    LOOP_ASSERT(i, X.beginHolidays() == X.endHolidays());
                    LOOP_ASSERT(i, 0 == X.numWeekendDaysTransitions());
                } END_BSLMA_EXCEPTION_TEST
            }
        }
        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[32 * 1024];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
            for (int i = 0; i < NUM_DATA; ++i) {
                const int DFIRST = DATA[i].d_first;
                const int DLAST  = DATA[i].d_last;

                bdet_Date dateFirst(DFIRST / 10000, DFIRST / 100 % 100,
                                    DFIRST % 100);
                bdet_Date dateLast (DLAST  / 10000, DLAST  / 100 % 100,
                                    DLAST  % 100);

                // Skip over the calendar(s) that may require a large amount of
                // memory.

                if ((dateLast - dateFirst)/8 > sizeof(memory)/(2 * NUM_DATA)) {
                    continue;
                }

                // Testing bdecs_Calendar(const bdet_Date&,
                //                        const bdet_Date&,
                //                        bslma_Allocator *).

                {
                    Obj *doNotDelete = new(a.allocate(sizeof(Obj)))
                                                  Obj(dateFirst, dateLast, &a);
                    const Obj& X = *doNotDelete;
                    if (veryVerbose) P(X);

                    if (dateFirst > dateLast) {
                        LOOP_ASSERT(i, X.firstDate() ==
                                                      bdet_Date(9999, 12, 31));
                        LOOP_ASSERT(i, X.lastDate()  ==
                                                      bdet_Date(   1,  1,  1));
                    }
                    else {
                        LOOP_ASSERT(i, X.firstDate() == dateFirst);
                        LOOP_ASSERT(i, X.lastDate()  == dateLast );
                    }
                    LOOP_ASSERT(i, X.beginHolidays() == X.endHolidays());
                    LOOP_ASSERT(i, 0 == X.numWeekendDaysTransitions());
                }

                // Testing bdecs_Calendar(const bdecs_PackedCalendar&,
                //                        bslma_Allocator *).

                {
                    bdecs_PackedCalendar *doNotDeletePackedCal =
                                  new(a.allocate(sizeof(bdecs_PackedCalendar)))
                                 bdecs_PackedCalendar(dateFirst, dateLast, &a);
                    Obj *doNotDelete = new(a.allocate(sizeof(Obj)))
                                                Obj(*doNotDeletePackedCal, &a);
                    const Obj& X = *doNotDelete;
                    if (veryVerbose) P(X);

                    if (dateFirst > dateLast) {
                        LOOP_ASSERT(i, X.firstDate() ==
                                                      bdet_Date(9999, 12, 31));
                        LOOP_ASSERT(i, X.lastDate()  ==
                                                      bdet_Date(   1,  1,  1));
                    }
                    else {
                        LOOP_ASSERT(i, X.firstDate() == dateFirst);
                        LOOP_ASSERT(i, X.lastDate()  == dateLast );
                    }
                    LOOP_ASSERT(i, X.beginHolidays() == X.endHolidays());
                    LOOP_ASSERT(i, 0 == X.numWeekendDaysTransitions());
                }
            }

            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }
      } break;
      case 10: {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //   1. The bdex stream functions are implemented using the respective
        //      member functions 'bdexStreamOut' and 'bdexStreamIn'.
        //   2. Streaming must be exception safe with a guarantee of rollback
        //      with respect to memory allocation or streaming in values.
        //   3. Ensure that streaming works under the following conditions:
        //       VALID - may contain any sequence of valid values.
        //       EMPTY - valid, but contains no data.
        //       INVALID - may or may not be empty.
        //       INCOMPLETE - the stream is truncated, but otherwise valid.
        //       CORRUPTED - the data contains explicitly inconsistent fields.
        //   4. both version 1 and version 2 of the streaming format are
        //      available. Version 2 supports calendars with more than one
        //      weekend-days transition,
        //   5. Verify that 'synchronizeCache' works properly.
        //      'synchronizeCache' is a private method which updates the
        //      non-business day cache with information from the internal
        //      packed calendar object.  It is used in several public methods
        //      of the calendar class.  Since the implementation of
        //      'bdexStreamIn' uses 'synchronizeCache' to update the internal
        //      non-business day cache, by verifying that the non-business day
        //      cache is correctly updated, we can conclude that
        //      'synchronizeCache' is working properly.
        //
        // Plan:
        //  To address concern 1 and 4, perform a trivial direct (breathing)
        //  test of the 'bdexStreamOut' and 'bdexStreamIn' methods for both
        //  version streaming format version.  Note that the rest of the
        //  testing will use the stream operators.
        //
        //  To address concerns 2 and 4, for a set of possible calendar values
        //  with and without weekend-days transitions, verify that bdex
        //  streaming works for both version 1 and version 2 of stream format
        //  when a calendar has only one weekend-days transition.  Verify that
        //  version 2 of the stream format supports calendars with more than
        //  one weekend-days transition.  Surround the stream in function with
        //  the macros 'BEGIN_EXCEPTION_SAFE_TEST' and
        //  'END_EXCEPTION_SAFE_TEST' to generate exceptions and verify that
        //  the destination object is rolled back in the case of an exception.
        //  These macros first set the allocation limit of the test allocator
        //  to 0 and gradually increase it while exercising the streaming
        //  functionality.  When an exception occurred, the exception handler
        //  verifies that the destination object is unchanged.
        //
        //  To address concern 3 and 4, specify a set S of unique object values
        //  with substantial and varied differences (with some only fully
        //  representable with version 2 of the streaming format), ordered by
        //  increasing complexity.
        //
        //  VALID STREAMS (and exceptions)
        //    Using all combinations of (u, v) in S X S, stream-out the value
        //    of u into a buffer and stream it back into (an independent
        //    instance of) v, and assert that u == v.
        //
        //  EMPTY AND INVALID STREAMS
        //    For each x in S, attempt to stream into (a temporary copy of) x
        //    from an empty and then invalid stream.  Verify after each try
        //    that the object is unchanged and that the stream is invalid.
        //
        //  INCOMPLETE (BUT OTHERWISE VALID) DATA
        //    Write 3 distinct objects to an output stream buffer of total
        //    length N.  For each partial stream length from 0 to N - 1,
        //    construct a truncated input stream and attempt to read into
        //    objects initialized with distinct values.  Verify values of
        //    objects that are either successfully modified or left entirely
        //    unmodified,  and that the stream became invalid immediately
        //    after the first incomplete read.
        //
        //  CORRUPTED DATA
        //    We will assume that the incomplete test fails every field,
        //    including a char (multi-byte representation).  Hence we need to
        //    produce values that are inconsistent with a valid value and
        //    verify that they are detected.  Use the underlying stream package
        //    to simulate an instance of a typical valid (control) stream and
        //    verify that it can be streamed in successfully.  Then for each
        //    data field in the stream (beginning with the version number),
        //    provide one or more similar tests with that data field corrupted.
        //    After each test, verify that the object is unchanged after
        //    streaming.
        //
        //   To address concern 5, we must not only rely on 'operator==' when
        //   verifying the result of 'bdexStreamIn'.  We need to also verify
        //   that the content of the non-business days cache matches that of
        //   the original calendar object.
        //
        // Testing:
        //   int maxSupportedBdexVersion() const
        //   STREAM& bdexStreamIn(STREAM&, int)
        //   STREAM& bdexStreamOut(STREAM&, int) const
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing Streaming Functionality" << endl
                          << "===============================" << endl;

        if (verbose) cout << "\nTesting 'maxSupportedBdexVersion()'." << endl;
        {
            if (verbose) cout << "\tusing object syntax:" << endl;
            const Obj X;
            ASSERT(1 == X.maxSupportedBdexVersion());
            if (verbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(1 == Obj::maxSupportedBdexVersion());
        }

        // Set the maximum supported version number to 2.  Note that
        // 'maxSupportedBdexVersion()' is kept at 1 for backwards
        // compatiblility reasons.

        const int VERSION = 2; // Obj::maxSupportedBdexVersion();

        if (verbose) cout << "\nDirect initial trial of 'bdexStreamOut' and"
                             " (valid) 'bdexStreamIn' functionality." << endl;

        {
            for (int version = 1; version <= VERSION; ++version) {
                if (veryVerbose) { P(version); }

                Obj mX(&testAllocator);
                const Obj& X = gg(&mX, "au@2005/5/1 30");

                if (veryVerbose) {
                    cout << "\t   Value being streamed: "; P(X); }

                bdex_TestOutStream out;  X.bdexStreamOut(out, version);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                bdex_TestInStream in(OD, LOD);  ASSERT(in);
                ASSERT(!in.isEmpty());
                in.setSuppressVersionCheck(1);

                Obj t(&testAllocator);  gg(&t, "u@2004/7/1 30 0");

                if (veryVerbose) {
                    cout << "\tValue being overwritten: "; P(t); }
                ASSERT(X != t);

                t.bdexStreamIn(in, version);
                ASSERT(in);  ASSERT(in.isEmpty());

                if (veryVerbose) {
                    cout << "\t  Value after overwrite: "; P(t); }

                ASSERT(isEqualWithCache(X, t));
            }
        }

        static const struct {
            int         d_line;     // source line number

            int         d_version;  // mimum supported version of the streaming
                                    // format

            const char *d_spec;     // spec to generate the calendar
        } DATA[] = {
            //LINE  VER  SPEC
            //----  ---  ----
            { L_,     1, ""                                                 },
            { L_,     1, "a"                                                },
            { L_,     1, "@2000/1/1"                                        },
            { L_,     1, "@2000/1/1 a"                                      },
            { L_,     1, "@2000/1/1 2"                                      },
            { L_,     1, "wt@2000/1/1 2"                                    },
            { L_,     1, "au@2000/1/1 59 3 4 5 6 7"                         },
            { L_,     1, "au@2000/1/1 365 2 20A 30 40AB 50DE"               },
            { L_,     2, "@2000/1/1 100 0u 50au 200fau"                     },
            { L_,     2, "u@2000/1/1 365 0au 50wr 200mt 2 20A 30 40AB 50DE" },
        };

        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << "\tOn valid, non-empty stream data." << endl;
        {
            for (int ui = 0; ui < NUM_DATA; ++ui) {
                const int         U_LINE = DATA[ui].d_line;
                const int         U_VERS = DATA[ui].d_version;
                const char *const U_SPEC = DATA[ui].d_spec;
                const Obj UU = g(U_SPEC);

                for (int vi = 0; vi < NUM_DATA; ++vi) {
                    const int         V_LINE = DATA[vi].d_line;
                    const int         V_VERS = DATA[vi].d_version;
                    const char *const V_SPEC = DATA[vi].d_spec;
                    const Obj VV = g(V_SPEC);


                    for (int version = U_VERS; version <= VERSION; ++version) {
                        if (veryVerbose) { P(version); }
                        Obj mU; const Obj& U = mU; gg(&mU, U_SPEC);
                        bdex_TestOutStream out;

                        // Testing stream-out operator here.
                        bdex_OutStreamFunctions::streamOut(out, U, version);

                        const char *const OD  = out.data();
                        const int         LOD = out.length();

                        bdex_TestInStream testInStream(OD, LOD);
                        testInStream.setSuppressVersionCheck(1);
                        LOOP_ASSERT(U_SPEC, testInStream);
                        LOOP_ASSERT(U_SPEC, !testInStream.isEmpty());

                        Obj mV; const Obj& V = mV;
                        BEGIN_EXCEPTION_SAFE_TEST {
                            testInStream.reset();
                            LOOP2_ASSERT(U_SPEC, V_SPEC, testInStream);
                            LOOP2_ASSERT(U_SPEC,
                                         V_SPEC,
                                         !testInStream.isEmpty());
                            gg(&mV, V_SPEC);

                            if (veryVerbose) { cout << "\t |"; P_(U); P(V); }


                            LOOP2_ASSERT(U_SPEC, V_SPEC,
                                         isEqualWithCache(UU, U));
                            LOOP2_ASSERT(U_SPEC, V_SPEC,
                                         isEqualWithCache(VV, V));

                            LOOP2_ASSERT(
                                        U_SPEC,
                                        V_SPEC,
                                        (ui == vi) == isEqualWithCache(U, V));

                            // Test stream-in operator here.
                            bdex_InStreamFunctions::streamIn(testInStream,
                                                             mV,
                                                             version);

                            LOOP2_ASSERT(U_SPEC, V_SPEC,
                                         isEqualWithCache(UU, U));
                            LOOP4_ASSERT(U_SPEC, V_SPEC, UU, V,
                                         isEqualWithCache(UU, V));
                            LOOP2_ASSERT(U_SPEC, V_SPEC,
                                         isEqualWithCache(U, V));
                        } END_EXCEPTION_SAFE_TEST(isEqualWithCache(VV, V));
                    }
                }
            }
        }

        if (verbose) cout << "\tOn empty and invalid streams." << endl;
        {
            bdex_TestInStream testInStream("", 0);
            testInStream.setSuppressVersionCheck(1);
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const char *const SPEC = DATA[ti].d_spec;
                const int         VERS = DATA[ti].d_version;

                for (int version = VERS; version <= VERSION; ++version) {
                    if (veryVerbose) { cout << "\t\t\t"; P_(version) P(SPEC); }

                    // Create control object X.

                    Obj mX; gg(&mX, SPEC); const Obj& X = mX;
                    Obj t; const Obj EMPTYCAL(t);
                    BEGIN_EXCEPTION_SAFE_TEST {
                        testInStream.reset();
                        gg(&t, SPEC);

                        // Ensure that reading from an empty or invalid input
                        // stream leaves the stream invalid and the target
                        // object unchanged.

                        LOOP_ASSERT(ti, testInStream);
                        LOOP_ASSERT(ti, isEqualWithCache(X, t));

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         t,
                                                         version);
                        LOOP_ASSERT(ti, !testInStream);
                        LOOP_ASSERT(ti, isEqualWithCache(X, t));

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         t,
                                                         version);
                        LOOP_ASSERT(ti, !testInStream);
                        LOOP_ASSERT(ti, isEqualWithCache(X, t));

                    } END_EXCEPTION_SAFE_TEST(isEqualWithCache(t, EMPTYCAL));
                }
            }
        }

        if (verbose) cout <<
            "\tOn incomplete (but otherwise valid) data." << endl;
        {

            for (int version = 1; version <= VERSION; ++version) {
                static const struct {
                    int         d_line;         // source line number

                    int         d_version;      // mimum supported version of
                                                // the streaming format

                    const char *d_spec;         // spec to generate the
                                                // calendar
                } DATA[] = {
                    //LINE  VER  SPEC
                    //----  ---  ----
                    { L_,     1, "@2005/12/1 60 4A 9BC 60DE"    },
                    { L_,     1, "u@1949/10/1 30 0ABCDE"        },
                    { L_,     1, "m@1/1/1"                      },
                    { L_,     2, "@2000/1/1 100 0u 50au 200fau" },
                };

                const int NUM_DATA = sizeof DATA / sizeof *DATA;
                Obj YYs[NUM_DATA];
                int LODs[NUM_DATA];

                bdex_TestOutStream out;
                for (int di = 0; di < NUM_DATA; ++di) {
                    if (DATA[di].d_version > version) {
                        continue;
                    }

                    YYs[di] = g(DATA[di].d_spec);

                    bdex_OutStreamFunctions::streamOut(out,
                                                       YYs[di],
                                                       version);
                    LODs[di] = out.length();
                }

                const char *const OD = out.data();
                const int LOD = out.length();
                const char *const CONTROL_SPEC = "au@1/1/1 365";
                const Obj XX = g(CONTROL_SPEC);


                for (int i = 1; i < LOD; ++i) {
                    bdex_TestInStream testInStream(OD, i);
                    bdex_TestInStream& in = testInStream;
                    in.setSuppressVersionCheck(1);
                    LOOP_ASSERT(i, in);  LOOP_ASSERT(i, !i == in.isEmpty());


                    if (veryVerbose) { cout << "\t\t"; P(i); }

                    Obj Xs[NUM_DATA];
                    BEGIN_EXCEPTION_SAFE_TEST {
                        in.reset();

                        LOOP_ASSERT(i, in);
                        LOOP_ASSERT(i, !i == in.isEmpty());

                        for (int di = 0; di < NUM_DATA; ++di) {
                            Xs[di] = g(CONTROL_SPEC);
                        }

                        for (int di = 0; di < NUM_DATA; ++di) {
                            if (DATA[di].d_version > version) {
                                continue;
                            }

                            bdex_InStreamFunctions::streamIn(in,
                                                             Xs[di],
                                                             version);
                            if (i < LODs[di]) {
                                LOOP2_ASSERT(i, di, !in);
                                LOOP4_ASSERT(i, di, Xs[di], XX,
                                             isEqualWithCache(Xs[di], XX));
                            }
                            else {
                                LOOP2_ASSERT(i, di, in);
                                LOOP4_ASSERT(i, di, Xs[di], YYs[di],
                                            isEqualWithCache(Xs[di], YYs[di]));
                            }
                        }
                    } END_EXCEPTION_SAFE_TEST(
                        (testCase10VerifyInvalidDataTest(Xs, YYs, LODs,
                                                         NUM_DATA, XX, i)));
                }
            }
        }

        if (verbose) cout << "\tOn corrupted data." << endl;

        const Obj W = g("");                 // default value
        const Obj X = g("au@2005/5/1 30");   // original value
        const Obj Y = g("u@2004/7/1 30 0");  // new value

        if (verbose) cout << "\t\tGood stream (for control)." << endl;
        {
            for (int version = 1; version <= VERSION; ++version) {
                bdex_TestOutStream out;

                Obj z(Y);
                bdex_OutStreamFunctions::streamOut(out, z, version);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); ASSERT(in);
                in.setSuppressVersionCheck(1);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(in);
                ASSERT(W != t);
                ASSERT(X != t);
                ASSERT(isEqualWithCache(Y, t));
            }
        }

        if (verbose) cout << "\t\tBad version." << endl;
        {
            for (int version = 0; version <= VERSION; ++version) {
                const char streamInVersion = 0; // too small
                bdex_TestOutStream out;

                Obj z(Y);
                bdex_OutStreamFunctions::streamOut(out, z, version);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);
                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, streamInVersion);
                ASSERT(!in);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);
            }
        }

        {
            for (int version = 0; version <= VERSION; ++version) {
                const char streamInVersion = 3; // too large
                bdex_TestOutStream out;

                Obj z(Y);
                bdex_OutStreamFunctions::streamOut(out, z, version);

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, streamInVersion);
                ASSERT(!in);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);
            }
        }

        if (verbose) cout << "\t\tBad offsets length'." << endl;
        {

            for (int version = 1; version <= VERSION; ++version) {
                bdex_TestOutStream out;

                // Writing first date, last date, and weekend days into the
                // stream.

                bdet_Date tempFirstDate(2005, 1,  1);
                bdet_Date tempLastDate (2005, 1, 31);

                tempFirstDate.bdexStreamOut(out, 1);
                tempLastDate.bdexStreamOut(out, 1);

                if (version == 1) {
                    bdec_DayOfWeekSet days;
                    days.add(bdet_DayOfWeek::BDET_SUN);
                    days.bdexStreamOut(out, 1);
                }
                else if (version == 2) {
                    int weekendDaysTransitionsLength = 0;
                    out.putLength(weekendDaysTransitionsLength);
                }

                // Writing offsets length and codes length into the stream.

                int OffsetsLength = 5;
                int codesLength   = 0;

                out.putLength(OffsetsLength);
                out.putLength(codesLength);

                // Writing less offsets into the stream than 'OffsetsLength'.

                for (int i = 0; i < OffsetsLength - 1; ++i) {
                    out.putInt32(i);
                }

                // Writing holiday code indices into the stream.

                for (int i = 0; i < OffsetsLength - 1; ++i) {
                    out.putInt32(0);
                }

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);
            }
        }


        if (verbose) cout << "\t\tBad offset values." << endl;
        {

            for (int version = 1; version <= VERSION; ++version) {
                bdex_TestOutStream out;

                // Writing first date, last date, and weekend days into the
                // stream.

                bdet_Date tempFirstDate(2005, 1,  1);
                bdet_Date tempLastDate (2005, 1, 31);

                tempFirstDate.bdexStreamOut(out, 1);
                tempLastDate.bdexStreamOut(out, 1);

                if (version == 1) {
                    bdec_DayOfWeekSet days;
                    days.add(bdet_DayOfWeek::BDET_SUN);
                    days.bdexStreamOut(out, 1);
                }
                else if (version == 2) {
                    int weekendDaysTransitionsLength = 0;
                    out.putLength(weekendDaysTransitionsLength);
                }

                // Writing offsets length and codes length into the stream.

                int OffsetsLength = 5;
                int codesLength   = 0;

                out.putLength(OffsetsLength);
                out.putLength(codesLength);

                // Writing offsets into the stream.  Note that we purposely
                // "corrupt" the second to the last offset to make it greater
                // than the last offset.

                for (int i = 0; i < OffsetsLength; ++i) {
                    if (i == OffsetsLength - 2) {
                        out.putInt32(i * 10);
                    }
                    else {
                        out.putInt32(i);
                    }
                }

                // Writing holiday code indices into the stream.

                for (int i = 0; i < OffsetsLength; ++i) {
                    out.putInt32(0);
                }

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);
            }
        }

        if (verbose) cout << "\t\tOffsets and code indices mismatch." << endl;
        {

            for (int version = 1; version <= VERSION; ++version) {
                bdex_TestOutStream out;

                // Writing first date, last date, and weekend days into the
                // stream.

                bdet_Date tempFirstDate(2005, 1,  1);
                bdet_Date tempLastDate (2005, 1, 31);

                tempFirstDate.bdexStreamOut(out, 1);
                tempLastDate.bdexStreamOut(out, 1);

                if (version == 1) {
                    bdec_DayOfWeekSet days;
                    days.add(bdet_DayOfWeek::BDET_SUN);
                    days.bdexStreamOut(out, 1);
                }
                else if (version == 2) {
                    int weekendDaysTransitionsLength = 0;
                    out.putLength(weekendDaysTransitionsLength);
                }

                // Writing offsets length and codes length into the stream.

                int OffsetsLength = 5;
                int codesLength   = 0;

                out.putLength(OffsetsLength);
                out.putLength(codesLength);

                // Writing offsets into the stream.

                for (int i = 0; i < OffsetsLength; ++i) {
                    out.putInt32(i);
                }

                // Writing less holiday code indices into the stream.

                for (int i = 0; i < OffsetsLength - 1; ++i) {
                    out.putInt32(0);
                }

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);
            }
        }

        if (verbose) cout << "\t\tBad holiday code length." << endl;
        {

            for (int version = 1; version <= VERSION; ++version) {
                bdex_TestOutStream out;

                // Writing first date, last date, and weekend days into the
                // stream.

                bdet_Date tempFirstDate(2005, 1,  1);
                bdet_Date tempLastDate (2005, 1, 31);

                tempFirstDate.bdexStreamOut(out, 1);
                tempLastDate.bdexStreamOut(out, 1);

                if (version == 1) {
                    bdec_DayOfWeekSet days;
                    days.add(bdet_DayOfWeek::BDET_SUN);
                    days.bdexStreamOut(out, 1);
                }
                else if (version == 2) {
                    int weekendDaysTransitionsLength = 0;
                    out.putLength(weekendDaysTransitionsLength);
                }

                // Writing offsets length and codes length into the stream.

                int OffsetsLength = 5;
                int codesLength   = 3;

                out.putLength(OffsetsLength);
                out.putLength(codesLength);

                // Writing offsets length into the stream.

                for (int i = 0; i < OffsetsLength; ++i) {
                    out.putInt32(i);
                }

                // Writing holiday code indices into the stream.  Note that the
                // last code index is greater than 'codesLength'.

                for (int i = 0; i < OffsetsLength; ++i) {
                    out.putInt32(i);
                }

                // Writing holiday codes into the stream.

                for (int i = 0; i < codesLength; ++i) {
                    out.putInt32(1);
                }

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);
            }
        }

        if (verbose)
            cout << "\t\tBad weekend-days transitions length." << endl;
        {
            // only version 2 of the streaming format supports weekend-days
            // transitions
            for (int version = 2; version <= VERSION; ++version) {
                bdex_TestOutStream out;

                // Writing first date, last date, and weekend days into the
                // stream.

                bdet_Date tempFirstDate(2005, 1,  1);
                bdet_Date tempLastDate (2005, 1, 31);

                tempFirstDate.bdexStreamOut(out, 1);
                tempLastDate.bdexStreamOut(out, 1);

                int weekendDaysTransitionsLength = 1;
                out.putLength(weekendDaysTransitionsLength);

                // Writing offsets length and codes length into the stream.

                int OffsetsLength = 5;
                int codesLength   = 0;

                out.putLength(OffsetsLength);
                out.putLength(codesLength);


                // Write more weekend-days transitions than the specified
                // length
                {
                    bdet_Date date(1, 1, 1);
                    date.bdexStreamOut(out, 1);
                    bdec_DayOfWeekSet days;
                    days.add(bdet_DayOfWeek::BDET_SUN);
                    days.bdexStreamOut(out, 1);
                }
                {
                    bdet_Date date(1, 1, 2);
                    date.bdexStreamOut(out, 1);
                    bdec_DayOfWeekSet days;
                    days.add(bdet_DayOfWeek::BDET_MON);
                    days.bdexStreamOut(out, 1);
                }

                // Writing offsets into the stream.

                for (int i = 0; i < OffsetsLength; ++i) {
                    out.putInt32(i);
                }

                for (int i = 0; i < OffsetsLength; ++i) {
                    out.putInt32(0);
                }

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);
            }
        }
        if (verbose) cout << "\t\tInvalid weekend-days transition order."
                          << endl;
        {
            // only version 2 of the streaming format supports weekend-days
            // transitions
            for (int version = 2; version <= VERSION; ++version) {
                bdex_TestOutStream out;

                // Writing first date, last date, and weekend days into the
                // stream.

                bdet_Date tempFirstDate(2005, 1,  1);
                bdet_Date tempLastDate (2005, 1, 31);

                tempFirstDate.bdexStreamOut(out, 1);
                tempLastDate.bdexStreamOut(out, 1);

                int weekendDaysTransitionsLength = 3;
                out.putLength(weekendDaysTransitionsLength);

                // Writing offsets length and codes length into the stream.

                int OffsetsLength = 5;
                int codesLength   = 0;

                out.putLength(OffsetsLength);
                out.putLength(codesLength);


                // Write more weekend-days transitions in some random
                // (non-ascending) time order
                {
                    bdet_Date date(1, 1, 1);
                    date.bdexStreamOut(out, 1);
                    bdec_DayOfWeekSet days;
                    days.add(bdet_DayOfWeek::BDET_MON);
                    days.bdexStreamOut(out, 1);
                }
                {
                    bdet_Date date(1, 1, 10);
                    date.bdexStreamOut(out, 1);
                    bdec_DayOfWeekSet days;
                    days.add(bdet_DayOfWeek::BDET_SUN);
                    days.bdexStreamOut(out, 1);
                }
                {
                    bdet_Date date(1, 1, 2);
                    date.bdexStreamOut(out, 1);
                    bdec_DayOfWeekSet days;
                    days.add(bdet_DayOfWeek::BDET_SUN);
                    days.bdexStreamOut(out, 1);
                }

                // Writing offsets into the stream.

                for (int i = 0; i < OffsetsLength; ++i) {
                    out.putInt32(i);
                }

                for (int i = 0; i < OffsetsLength; ++i) {
                    out.putInt32(0);
                }

                const char *const OD  = out.data();
                const int         LOD = out.length();

                Obj t(X);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);
                ASSERT(isEqualWithCache(X, t));
                ASSERT(Y != t);
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns:
        //   1. The value represented by any instance can be assigned to any
        //      other instance.  And the assignment operator returns a
        //      reference to the destination object.
        //   2. The 'rhs' value must not be affected by the operation.
        //   3. 'rhs' going out of scope has no effect on the value of 'lhs'
        //      after the assignment.
        //   4. Aliasing (x = x): The assignment operator must always work --
        //      even when the 'lhs' and 'rhs' are identically the same object.
        //   5. The allocator value is not part of the assignment.
        //   6. The assignment operator must be exception safe with a guarantee
        //      of rollback with respect to memory allocation.
        //
        // Plan:
        //   To address concerns 1, 2, and 3, specify a set S of unique object
        //   values with substantial and varied differences.  Construct tests
        //   X = Y for all (X, Y) in S x S.  For each of these test, generate Y
        //   and YY using the same 'SPEC'.  After the assignment, assert that
        //   X, Y, and the value returned by the assignment operator all equal
        //   YY.  Let Y go out of scope and confirm that YY equals X.
        //
        //   To address concern 4, we perform Y = Y and verify that both the
        //   return value of the assignment and Y itself equal YY.
        //
        //   To address concern 5, we create an object with the default
        //   allocator and assigned to it an object with a test allocator.
        //   Then we verify that the memory of the new object is drawn from the
        //   default allocator.
        //
        //   To address concern 6, we use a test allocator and initially set
        //   the allocation limit to 0.  Then we perform assignments while
        //   gradually increasing the allocation limit.  This will trigger
        //   exceptions when the assignments need to obtain memory.  Then in
        //   the exception handler we verify that the values of the objects
        //   involved in the assignment are unchanged.
        //
        // Testing:
        //   bdecs_Calendar& operator=(const bdecs_Calendar& rhs)
        //   bdecs_Calendar& operator=(const bdecs_PackedCalendar& rhs)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Assignment Operator" << endl
                          << "===========================" << endl;

        static const char *SPECS[] = {
            "",       "a",      "@2000/1/1",          "@2000/1/1 a",
            "@2000/1/1 2",      "@2000/1/1 2 2au",    "au@2000/1/1 2 1",
            "au@2000/1/1 1 1A", "au@2000/1/1 2 1A 2", "@2000/1/1 2 2A",
            "@2000/1/1 100",    "au@2000/1/1 365 2 20A 30 40AB 50DE",
            "au@2000/1/1 2 0au",  "@2000/1/1 150 0au 30tw 100rf",
        0}; // Null string required as last element.

        for (int i = 0; SPECS[i]; ++i) {
            for (int j = 0; SPECS[j]; ++j) {
                const Obj XX(g(SPECS[i]), &testAllocator);
                const Obj YY(g(SPECS[j]), &testAllocator);

                // In the block below, objects 'mX' and 'mZ' are assigned the
                // value of 'mY'.  After 'mY' goes out of scope, we verify that
                // 'mX' still has the value of 'mY'.  And when 'mZ' goes out of
                // scope, its destructor will assert its invariants.

                {
                    Obj mX(&testAllocator);  const Obj& X = mX;
                    gg(&mX, SPECS[i]);
                    {
                        Obj mY(&testAllocator); const Obj& Y = mY;
                        Obj mZ(&testAllocator); const Obj& Z = mZ;
                        gg(&mY, SPECS[j]);
                        gg(&mZ, SPECS[i]);

                        LOOP_ASSERT(i, isEqualWithCache(YY, (mY = Y)));
                        LOOP_ASSERT(i, isEqualWithCache(YY, Y));
                        LOOP2_ASSERT(i, j, (i == j) == isEqualWithCache(X, Y));
                        LOOP2_ASSERT(i, j, isEqualWithCache(YY, (mX = Y)));
                        LOOP2_ASSERT(i, j, isEqualWithCache(YY, (mZ = Y)));
                        LOOP2_ASSERT(i, j, isEqualWithCache(YY, Y));
                        LOOP2_ASSERT(i, j, isEqualWithCache(YY, X));
                        LOOP2_ASSERT(i, j, isEqualWithCache(YY, Z));
                    }
                    LOOP2_ASSERT(i, j, isEqualWithCache(YY, X));
                }

                // Replicate for 'operator=(const bdecs_PackedCalendar&)'
                {
                    Obj mX(&testAllocator);  const Obj& X = mX;
                    gg(&mX, SPECS[i]);
                    {
                        Obj mY(&testAllocator); const Obj& Y = mY;
                        Obj mZ(&testAllocator); const Obj& Z = mZ;
                        gg(&mY, SPECS[j]);
                        gg(&mZ, SPECS[i]);

                        LOOP_ASSERT(i, isEqualWithCache(YY, (mY = Y)));
                        LOOP_ASSERT(i, isEqualWithCache(YY, Y));
                        LOOP2_ASSERT(i, j, (i == j) == isEqualWithCache(X, Y));
                        LOOP2_ASSERT(i, j,
                           isEqualWithCache(YY, (mX = Y.packedCalendar())));
                        LOOP2_ASSERT(i, j,
                           isEqualWithCache(YY, (mZ = Y.packedCalendar())));
                        LOOP2_ASSERT(i, j, isEqualWithCache(YY, Y));
                        LOOP2_ASSERT(i, j, isEqualWithCache(YY, X));
                        LOOP2_ASSERT(i, j, isEqualWithCache(YY, Z));
                    }
                    LOOP2_ASSERT(i, j, isEqualWithCache(YY, X));
                }

                // Verify that the allocator value is not part of the
                // assignment.

                {
                    // Compute the number of blocks needed for this
                    // assignment.

                    Obj mX(&testAllocator);
                    gg(&mX, SPECS[i]);
                    int blocks = testAllocator.numBlocksTotal();
                    mX = YY;
                    blocks = testAllocator.numBlocksTotal() - blocks;

                    // Assign 'YY', which uses a test allocator, to 'mY', which
                    // uses the default allocator.  The allocator value of 'mY'
                    // should not be affected.

                    bslma_TestAllocator da; // default allocator
                    const bslma_DefaultAllocatorGuard DAG(&da);
                    Obj mY;
                    gg(&mY, SPECS[i]);
                    int defaultBlocks = da.numBlocksTotal();
                    int testBlocks = testAllocator.numBlocksTotal();
                    mY = YY;
                    defaultBlocks = da.numBlocksTotal() - defaultBlocks;

                    // Verify 'mY' still uses the default allocator to obtain
                    // memory and the test allocator used by 'YY' is not used,
                    // thus proving the allocator value of 'mY' is unaffected
                    // by the assignment.

                    LOOP3_ASSERT(i, blocks, defaultBlocks,
                                                      blocks == defaultBlocks);
                    LOOP3_ASSERT(i, testBlocks, testAllocator.numBlocksTotal(),
                                 testBlocks == testAllocator.numBlocksTotal());
                }

                // Testing exception safety.

                #ifdef BDE_BUILD_TARGET_EXC
                {
                    Obj mX(&testAllocator); const Obj& X = mX;
                    Obj mY(&testAllocator); const Obj& Y = mY;
                    gg(&mX, SPECS[i]);
                    gg(&mY, SPECS[j]);
                    const int limit = testAllocator.allocationLimit();
                    int bdemaExceptionCounter = 0;
                    testAllocator.setAllocationLimit(bdemaExceptionCounter);
                    do {
                        try {
                            mX = Y;
                        } catch (bslma_TestAllocatorException& e) {
                            LOOP2_ASSERT(i, j, isEqualWithCache(XX, X));
                            LOOP2_ASSERT(i, j, isEqualWithCache(YY, Y));
                            testAllocator.setAllocationLimit(
                                                      ++bdemaExceptionCounter);
                            continue;
                        }
                        break;
                    } while (1);

                    bdemaExceptionCounter = 0;
                    testAllocator.setAllocationLimit(bdemaExceptionCounter);
                    do {
                        try {
                            mY = Y;
                        } catch (bslma_TestAllocatorException& e) {
                            LOOP2_ASSERT(i, j, isEqualWithCache(YY, Y));
                            testAllocator.setAllocationLimit(
                                                      ++bdemaExceptionCounter);
                            continue;
                        }
                        testAllocator.setAllocationLimit(limit);
                        break;
                    } while (1);
                }

                // Replicate for 'operator=(const bdecs_PackedCalendar&)'
                {
                    Obj mX(&testAllocator); const Obj& X = mX;
                    Obj mY(&testAllocator); const Obj& Y = mY;
                    gg(&mX, SPECS[i]);
                    gg(&mY, SPECS[j]);
                    const int limit = testAllocator.allocationLimit();
                    int bdemaExceptionCounter = 0;
                    testAllocator.setAllocationLimit(bdemaExceptionCounter);
                    do {
                        try {
                            mX = Y.packedCalendar();
                        } catch (bslma_TestAllocatorException& e) {
                            LOOP2_ASSERT(i, j, isEqualWithCache(XX, X));
                            LOOP2_ASSERT(i, j, isEqualWithCache(YY, Y));
                            testAllocator.setAllocationLimit(
                                                      ++bdemaExceptionCounter);
                            continue;
                        }
                        break;
                    } while (1);

                    bdemaExceptionCounter = 0;
                    testAllocator.setAllocationLimit(bdemaExceptionCounter);
                    do {
                        try {
                            mY = Y.packedCalendar();
                        } catch (bslma_TestAllocatorException& e) {
                            LOOP2_ASSERT(i, j, isEqualWithCache(YY, Y));
                            testAllocator.setAllocationLimit(
                                                      ++bdemaExceptionCounter);
                            continue;
                        }
                        testAllocator.setAllocationLimit(limit);
                        break;
                    } while (1);
                }
                #endif
            }
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, 'g':
        //
        // Concerns:
        //   Since 'g' is implemented almost entirely using 'gg', we need to
        //   verify only that the arguments are properly forwarded, that 'g'
        //   does not affect the test allocator, and that 'g' returns an
        //   object by value.
        //
        // Plan:
        //   For each SPEC in a short list of specifications, compare the
        //   object returned (by value) from the generator function, 'g(SPEC)'
        //   with the value of a newly constructed OBJECT configured using
        //   'gg(&OBJECT, SPEC)'.  Compare the results of calling the
        //   allocator's 'numBlocksTotal' and 'numBytesInUse' methods before
        //   and after calling 'g' in order to demonstrate that 'g' has no
        //   effect on the test allocator.  Finally, compare the address of the
        //   temporary object returned by 'g' to show that 'g' returns an
        //   object by value.
        //
        // Testing:
        //   bdecs_Calendar g(const char *spec)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Generator Function 'g'" << endl
                          << "==============================" << endl;

        static const char *SPECS[] = {
            "",              "au",                   "@2005/1/1 0",
            "w@1900/3/4 30", "au@1/1/1 30 2 4A 30B", "fau@9999/12/31 0 0A",
            "au@2005/5/1 30 1A 3B 11C 12D 13E 29",
            "au@2008/1/1 365 15 40A 120B 280C 300D 365E",
            "au@2000/1/1 2 0au",  "@2000/1/1 150 0au 30tw 100rf",
        0}; // Null string required as last element.

        if (verbose) cout <<
            "\nCompare values produced by 'g' and 'gg' for various inputs."
                                                                       << endl;
        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *spec = SPECS[ti];
            if (veryVerbose) { P_(ti);  P(spec); }
            Obj mX(&testAllocator);  gg(&mX, spec);  const Obj& X = mX;
            if (veryVerbose) {
                cout << "\t g = " << g(spec) << endl;
                cout << "\tgg = " << X       << endl;
            }
            const int TOTAL_BLOCKS_BEFORE = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_BEFORE = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, X == g(spec));
            const int TOTAL_BLOCKS_AFTER = testAllocator.numBlocksTotal();
            const int IN_USE_BYTES_AFTER = testAllocator.numBytesInUse();
            LOOP_ASSERT(ti, TOTAL_BLOCKS_BEFORE == TOTAL_BLOCKS_AFTER);
            LOOP_ASSERT(ti, IN_USE_BYTES_BEFORE == IN_USE_BYTES_AFTER);
        }

        if (verbose) cout << "\nConfirm return-by-value." << endl;
        {
            const char *spec = "au@2008/1/1 365 15 40A 120B 280C 300D 365E";

            ASSERT(sizeof(Obj) == sizeof g(spec));      // compile-time fact

            const Obj& r1 = g(spec);
            const Obj& r2 = g(spec);
            ASSERT(&r2 != &r1);
        }
      } break;
      case 7: {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //   1. The new object's value is the same as that of the original
        //      object.
        //   2. The value of the original object is left unaffected.
        //   3. Subsequent changes in or destruction of the source object have
        //      no effect on the copy-constructed object.
        //   4. The object has its internal memory management system hooked up
        //      properly so that *all* internally allocated memory draws from a
        //      user-supplied allocator whenever one is specified, and
        //      otherwise the default one.
        //
        // Plan:
        //   To address concern 1 - 3, specify a set S of object values with
        //   substantial and varied differences.  For each value in S,
        //   initialize objects W and X, copy construct Y from X and use
        //   'operator==()' to verify that both X and Y subsequently have the
        //   same value as W.  Let X go out of scope and again verify that
        //   W == Y.
        //
        //   To address concern 4, we will install a test allocator as the
        //   default and also supply a separate test allocator explicitly.  We
        //   will measure the total usage of both allocators before and after
        //   calling the copy constructor to ensure that no memory is allocated
        //   from the default allocator.  Then we will call the copy
        //   constructor to create another object using the default allocator
        //   and verify that the amount of memory used is the same as that with
        //   the supplied test allocator.
        //
        //   It is not necessary to perform exception-safety test for the copy
        //   constructor because it is using only the class members' copy
        //   constructors in the initialization list to create the object.
        //
        // Testing:
        //   bdecs_Calendar::bdecs_Calendar(const bdecs_Calendar&  original,
        //                                  bslma_Allocator *basicAllocator)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Copy Constructor" << endl
                          << "========================" << endl;

        static const char *SPECS[] = {
            "",       "a",      "@2000/1/1",          "@2000/1/1 a",
            "@2000/1/1 2",      "@2000/1/1 2 2au",    "au@2000/1/1 2 1",
            "au@2000/1/1 1 1A", "au@2000/1/1 2 1A 2", "@2000/1/1 2 1 2A",
            "@2000/1/1 100",    "au@2000/1/1 365 2 20A 30 40AB 50DE",
            "au@2000/1/1 2 0au",  "@2000/1/1 150 0au 30tw 100rf",
        0}; // Null string required as last element.

        for (int i = 0; SPECS[i]; ++i) {
            Obj *pX = new Obj(&testAllocator);  // Dynamic allocation allows us
                                                // to easily control the
                                                // destruction of this object.
            Obj& mX = *pX;
            const Obj& X = mX;
            Obj W(&testAllocator);
            gg(&mX, SPECS[i]);
            gg(&W, SPECS[i]);

            bslma_TestAllocator da; // default allocator
            const bslma_DefaultAllocatorGuard DAG(&da);
            bslma_TestAllocator a;  // specified allocator
            LOOP2_ASSERT(i, da.numBlocksTotal(), 0 == da.numBlocksTotal());
            int blocks = a.numBlocksTotal();
            const Obj Y(X, &a);
            blocks = a.numBlocksTotal() - blocks;
            LOOP2_ASSERT(i, da.numBlocksTotal(), 0 == da.numBlocksTotal());

            int defaultBlocks = da.numBlocksTotal();
            const Obj Z(X);
            defaultBlocks = da.numBlocksTotal() - defaultBlocks;
            LOOP3_ASSERT(i, blocks, defaultBlocks, blocks == defaultBlocks);

            LOOP_ASSERT(i, W == X);
            LOOP_ASSERT(i, W == Y);
            if (bsl::strcmp(SPECS[i], "") != 0)
            {
                mX.removeAll();
                LOOP_ASSERT(i, !(X == Y));
                LOOP_ASSERT(i,   W == Y);
            }
            delete pX;  // X is now out of scope.
            LOOP_ASSERT(i, W == Y);
        }
      } break;
      case 6: {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS: '==' and '!='
        //
        // Concerns:
        //   We want to verify that these two functions have been properly
        //   hooked with the corresponding functions for the packed calendar
        //   class.
        //
        // Plan:
        //   To address the concern, we set up a small set of calendar objects
        //   and use these two functions to compare these calendars against
        //   each other.
        //
        // Testing:
        //   bool operator==(const bdecs_Calendar& lhs,
        //                   const bdecs_Calendar& rhs)
        //   bool operator!=(const bdecs_Calendar& lhs,
        //                   const bdecs_Calendar& rhs)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing Equality Operators" << endl
                          << "==========================" << endl;

        static const char *SPECS[] = {
            "", "a", "fau@2004/2/28 3", "au@2000/1/1 365 2 20A 30 40AB 50DE",
            "@2000/1/1 0 0au 30tw 100rf",
        0}; // Null string required as last element.

        for (int i = 0; SPECS[i]; ++i) {
            for (int j = 0; SPECS[j]; ++j) {

                if (veryVerbose) { P_(SPECS[i]) P(SPECS[j]) }
                Obj mX(&testAllocator); const Obj& X = mX;
                gg(&mX, SPECS[i]);

                // Perform a self comparison first.

                LOOP_ASSERT(i,   X == X);
                LOOP_ASSERT(i, !(X != X));

                // Compare calendars against each other.

                Obj mY(&testAllocator); const Obj& Y = mY;
                gg(&mY, SPECS[j]);
                LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                LOOP2_ASSERT(i, j, (i == j) != (X != Y));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR and 'print' method:
        //
        // Concerns:
        //   We want to ensure that the 'print' method correctly formats a
        //   'bdecs_Calendar' object for output with any valid 'level' and
        //   'spacesPerLevel' values and returns the specified stream.
        //
        // Plan:
        //   We will verify that 'print' has been properly hooked up with the
        //   'print' method of the internal packed calendar object by using a
        //   small set of calendar objects combined with some 'level' and
        //   'spacesPerLevel' values as test vectors.  Since 'operator<<' is
        //   implemented based upon the 'print' method, we will check its
        //   output against the 'print' method when the values of 'level' and
        //   'spacesPerLevel' match those used to implement 'operator<<'.  We
        //   will also verify that 'operator<<' returns the specified stream.
        //
        // Testing:
        //   bsl::ostream& bdecs_Calendar::print(
        //                                 bsl::ostream& stream,
        //                                 int           level,
        //                                 int           spacesPerLevel) const;
        //    bsl::ostream& operator<<(bsl::ostream&         stream,
        //                             const bdecs_Calendar& calendar);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Testing 'print' and 'operator<<'." << endl
                          << "=================================" << endl;

        char R0[]="{ [ 01JAN0001, 09FEB0001 ] [ 01JAN0001 : [ SUN SAT ] ] "
                  "02JAN0001 03JAN0001 { 0 } 04JAN0001 { 1 } "
                  "08JAN0001 { 0 1 2 100 } }";
        char R1[]="  {\n"
                  "    [ 01JAN0001, 13JAN0001 ]\n"
                  "    [ 01JAN0001 : [ SUN SAT ] ]\n"
                  "    13JAN0001 {\n"
                  "      0\n"
                  "      1\n"
                  "      2\n"
                  "      100\n"
                  "      1000\n"
                  "    }\n"
                  "  }\n";
        char R2[]="{ [ 01JAN0001, 11JAN0001 ] "
                  "[ 01JAN0001 : [ SUN FRI SAT ], "
                  "11JAN0001 : [ TUE ], "
                  "13JAN0001 : [ WED THU ] ] "
                  "02JAN0001 { 0 } 03JAN0001 { 1 } }";

        static const struct {
            const char *d_spec;
            int   d_level;
            int   d_spacesPerLevel;
            const char *d_result;
        } DATA[] = {
            //SPEC                        LEVEL SPACE PER LEVEL   RESULT
            {"au@1/1/1 39 1 2A 3B 7ABCD", 0,    -1,               R0},
            {"au@1/1/1 12 12ABCDE",       1,     2,               R1},
            {"fau@1/1/1 10t 1A 12wr 2B", -1,    -2,               R2},
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *SPEC = DATA[ti].d_spec;
            const int LEVEL = DATA[ti].d_level;
            const int SPACES = DATA[ti].d_spacesPerLevel;
            const bsl::string RESULT = DATA[ti].d_result;

            bsl::ostringstream printStream;
            bsl::ostringstream operatorStream;
            Obj mX(&testAllocator); const Obj& X = mX;
            gg(&mX, SPEC);

            LOOP_ASSERT(ti, printStream == X.print(printStream, LEVEL,
                                                                      SPACES));
            LOOP_ASSERT(ti, operatorStream == (operatorStream << X));

            bsl::string cmp = printStream.str();
            LOOP3_ASSERT(ti, cmp.size(), RESULT.size(),
                                                  cmp.size() == RESULT.size());
            for (int i = 0; i < cmp.size(); ++i) {
                LOOP4_ASSERT(ti, i, cmp[i], RESULT[i], cmp[i] == RESULT[i]);
            }
            if (LEVEL == 0 && SPACES == -1) {
                bsl::string cmp2 = operatorStream.str();
                LOOP3_ASSERT(ti, cmp.size(), cmp2.size(),
                                                    cmp.size() == cmp2.size());
                for (int i = 0; i < cmp.size(); ++i) {
                    LOOP4_ASSERT(ti, i, cmp[i], cmp2[i], cmp[i] == cmp2[i]);
                }
            }
        }
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // We want to verify that the 'beginXXXX', 'rbeginXXXX', 'endXXXX', and
        // 'rendXXXX' methods for 'HolidayConstIterator',
        // 'HolidayCodeConstIterator' and 'WeekendDaysTransitionConstIterator',
        // along with 'isInRange', 'firstDate', 'lastDate', 'isHoliday',
        // 'isWeekendDay(bdet_DayOfWeek::Day)', and 'weekendDays' are properly
        // hooked up with the internal packed calendar object.  We also want to
        // test the other basic accessors including 'length',
        // 'isNonBusinessDay', 'numBusinessDays', 'numWeekendDaysTransitions',
        // 'numNonBusinessDays', and 'packedCalendar'.
        //
        // Concerns:
        //   1. The 'beginXXXX', 'rbeginXXXX', 'endXXXX', and 'rendXXXX'
        //      methods for 'HolidayIterator', 'HolidayCodeIterator' and
        //      'WeekendDaysTransitionConstIterator' return the proper
        //      iterators.
        //   2. 'length', 'isInRange', 'firstDate', and 'lastDate' return the
        //      expected results for various type of calendar objects.
        //   3. 'isHoliday', and 'isWeekendDay(bdet_DayOfWeek::Day)' properly
        //      identify holidays and weekend days.]
        //   4. 'weekendDays' properly returns the set of weekend days
        //      associated with this calendar.
        //   5. 'isNonBusinessDay' properly identifies non-business days.
        //   6. 'numBusinessDays' and 'numNonBusinessDays' returns the correct
        //      information about number of business days in the calendar.
        //
        // Plan:
        //   To address concerns 1, 2, 3 and 4, we create a calendar object
        //   and use it to test these methods to verify that they have been
        //   properly hooked up with the methods which they are implemented
        //   based upon.
        //
        //   To address concern 5, we call 'isNonBusinessDay' with every day of
        //   the calendar and verify the results using 'isHoliday' and
        //   'isWeekendDay'.
        //
        //   To address concern 6, we first count the number of business days
        //   in a calendar by examining every day of the calendar.  We then use
        //   this number to verify the results returned by 'numBusinessDays'
        //   and 'numNonBusinessDays'.
        //
        // Testing:
        //   HolidayIterator beginHolidays() const
        //   HolidayIterator beginHolidays(const bdet_Date&) const
        //   HolidayIterator endHolidays() const
        //   HolidayIterator endHolidays(const bdet_Date&) const
        //   HolidayReverseIterator rbeginHolidays() const
        //   HolidayReverseIterator rbeginHolidays(const bdet_Date&) const
        //   HolidayReverseIterator rendHolidays() const
        //   HolidayReverseIterator rendHolidays(const bdet_Date&) const
        //   HolidayCodeIterator beginHolidayCodes(
        //                                     const HolidayIterator&) const
        //   HolidayCodeIterator beginHolidayCodes(const bdet_Date&) const
        //   HolidayCodeIterator endHolidayCodes(
        //                                        const HolidayIterator&) const
        //   HolidayCodeIterator endHolidayCodes(const bdet_Date&) const
        //   HolidayCodeReverseIterator rbeginHolidayCodes(
        //                                        const HolidayIterator&) const
        //   HolidayCodeReverseIterator rbeginHolidayCodes(
        //                                              const bdet_Date&) const
        //   HolidayCodeReverseIterator rendHolidayCodes(
        //                                        const HolidayIterator&) const
        //   HolidayCodeReverseIterator rendHolidayCodes(
        //                                              const bdet_Date&) const
        //   beginWeekendDaysTransitions() const
        //   endWeekendDaysTransitions() const
        //   int numWeekendDaysTransitions() const
        //   int length()
        //   bool isInRange(const bdet_Date&)
        //   const bdet_Date& firstDate()
        //   const bdet_Date& lastDate()
        //   const bdec_DayOfWeekSet& weekendDays() const
        //   bool isHoliday(const bdet_Date&)
        //   bool isWeekendDay(bdet_DayOfWeek::Day)
        //   bool isWeekendDay(bdet_Date)
        //   bool isNonBusinessDay(const bdet_Date& date) const
        //   int numBusinessDays() const;
        //   int numNonBusinessDays() const;
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING BASIC ACCESSORS" << endl
                          << "=======================" << endl;

        Obj mX(&testAllocator); const Obj& X = mX;
        gg(&mX, "fau@2005/1/1 365 @2005/3/1mt @2005/6/10wr "
                "@2005/1/1AB 1ABCDE 30DE 90 365E");

        bdet_Date start(2005,1,1);

        // Testing 'packedCalendar'.
        bdecs_PackedCalendar packedCal;
        bdec_DayOfWeekSet weekendDays;
        packedCal.setValidRange(start, start + 365);
        packedCal.addWeekendDay(bdet_DayOfWeek::BDET_FRI);

        weekendDays.removeAll();
        weekendDays.add(bdet_DayOfWeek::BDET_SAT);
        weekendDays.add(bdet_DayOfWeek::BDET_SUN);
        packedCal.addWeekendDays(weekendDays);

        weekendDays.removeAll();
        weekendDays.add(bdet_DayOfWeek::BDET_MON);
        weekendDays.add(bdet_DayOfWeek::BDET_TUE);
        packedCal.addWeekendDaysTransition(bdet_Date(2005, 3, 1), weekendDays);

        weekendDays.removeAll();
        weekendDays.add(bdet_DayOfWeek::BDET_WED);
        weekendDays.add(bdet_DayOfWeek::BDET_THU);
        packedCal.addWeekendDaysTransition(bdet_Date(2005, 6, 10),
                                           weekendDays);

        packedCal.addHoliday(start + 0);
        packedCal.addHolidayCode(start + 0, VA);
        packedCal.addHolidayCode(start + 0, VB);
        packedCal.addHoliday(start + 1);
        packedCal.addHolidayCode(start + 1, VA);
        packedCal.addHolidayCode(start + 1, VB);
        packedCal.addHolidayCode(start + 1, VC);
        packedCal.addHolidayCode(start + 1, VD);
        packedCal.addHolidayCode(start + 1, VE);
        packedCal.addHoliday(start + 30);
        packedCal.addHolidayCode(start + 30, VD);
        packedCal.addHolidayCode(start + 30, VE);
        packedCal.addHoliday(start + 90);
        packedCal.addHoliday(start + 365);
        packedCal.addHolidayCode(start + 365, VE);

        LOOP2_ASSERT(packedCal,
                     X.packedCalendar(),
                     packedCal == X.packedCalendar());

        // Testing 'beginHolidays', 'endHolidays', 'rbeginHolidays', and
        // 'rendHolidays'.

        ASSERT(*X.beginHolidays() == bdet_Date(2005,1,1));
        ASSERT(*X.beginHolidays(bdet_Date(2005,1,15)) == bdet_Date(2005,1,31));
        ASSERT(*X.beginHolidays(bdet_Date(2005,4,1)) == bdet_Date(2005,4,1));
        Obj::HolidayConstIterator hit = X.endHolidays(); --hit;
        ASSERT(*hit == bdet_Date(2006,1,1));
        ASSERT(*X.endHolidays(bdet_Date(2005,3,31)) == bdet_Date(2005,4,1));
        ASSERT(*X.endHolidays(bdet_Date(2005,4,1)) == bdet_Date(2006,1,1));
        ASSERT(X.endHolidays(bdet_Date(2006,1,1)) == X.endHolidays());
        ASSERT(*X.rbeginHolidays() == bdet_Date(2006,1,1));
        ASSERT(*X.rbeginHolidays(bdet_Date(2005,4,2)) == bdet_Date(2005,4,1));
        ASSERT(*X.rbeginHolidays(bdet_Date(2005,4,1)) == bdet_Date(2005,4,1));
        Obj::HolidayConstReverseIterator hrit = X.rendHolidays(); --hrit;
        ASSERT(*hrit == bdet_Date(2005,1,1));
        ASSERT(*X.rendHolidays(bdet_Date(2005,1,31)) == bdet_Date(2005,1,2));
        ASSERT(*X.rendHolidays(bdet_Date(2005,1,3)) == bdet_Date(2005,1,2));
        ASSERT(X.rendHolidays(bdet_Date(2005,1,1)) == X.rendHolidays());

        // Testing 'beginHolidayCodes', 'endHolidayCodes',
        // 'rbeginHolidayCodes', and 'rendHolidayCodes'.

        hit = X.beginHolidays(); ++hit;
        ASSERT(bdet_Date(2005,1,2) == *hit);
        ASSERT(*X.beginHolidayCodes(hit) == 0);
        Obj::HolidayCodeConstIterator hcit = X.endHolidayCodes(hit); --hcit;
        ASSERT(*hcit == 1000);
        hcit = X.endHolidayCodes(*hit); --hcit;
        ASSERT(*hcit == 1000);
        ASSERT(*X.rbeginHolidayCodes(hit) == 1000);
        ASSERT(*X.rbeginHolidayCodes(*hit) == 1000);
        Obj::HolidayCodeConstReverseIterator hcrit = X.rendHolidayCodes(hit);
        --hcrit;
        ASSERT(*hcrit == 0);
        hcrit = X.rendHolidayCodes(*hit);
        --hcrit;
        ASSERT(*hcrit == 0);

        // Testing 'beginWeekendDaysTransitions', 'endWeekendDaysTransitions'

        Obj::WeekendDaysTransitionConstIterator wdIt =
                                               X.beginWeekendDaysTransitions();

        weekendDays.removeAll();
        weekendDays.add(bdet_DayOfWeek::BDET_FRI);
        weekendDays.add(bdet_DayOfWeek::BDET_SAT);
        weekendDays.add(bdet_DayOfWeek::BDET_SUN);
        ASSERT(wdIt->first == bdet_Date(1,1,1));
        ASSERT(wdIt->second == weekendDays);

        ++wdIt;
        weekendDays.removeAll();
        weekendDays.add(bdet_DayOfWeek::BDET_MON);
        weekendDays.add(bdet_DayOfWeek::BDET_TUE);
        ASSERT(wdIt->first == bdet_Date(2005,3,1));
        ASSERT(wdIt->second == weekendDays);

        ++wdIt;
        weekendDays.removeAll();
        weekendDays.add(bdet_DayOfWeek::BDET_WED);
        weekendDays.add(bdet_DayOfWeek::BDET_THU);
        ASSERT(wdIt->first == bdet_Date(2005,6,10));
        ASSERT(wdIt->second == weekendDays);

        ++wdIt;
        ASSERT(wdIt == X.endWeekendDaysTransitions());

        ASSERT(X.length() == 366);
        ASSERT(X.firstDate() == bdet_Date(2005,1,1));
        ASSERT(X.lastDate()  == bdet_Date(2006,1,1));
        ASSERT(true  == X.isInRange(X.firstDate()));
        ASSERT(true  == X.isInRange(X.lastDate()));
        ASSERT(true  == X.isInRange(X.firstDate()+1));
        ASSERT(true  == X.isInRange(X.lastDate()-1));
        ASSERT(false == X.isInRange(X.firstDate()-1));
        ASSERT(false == X.isInRange(X.lastDate()+1));

        mX.addDay(bdet_Date(2004,12,30));
        mX.addDay(bdet_Date(2006,1,3));
        ASSERT(X.length() == 370);
        ASSERT(X.firstDate() == bdet_Date(2004,12,30));
        ASSERT(X.lastDate()  == bdet_Date(2006,1,3));

        // Testing 'isHoliday'.

        ASSERT(true  == X.isHoliday(bdet_Date(2005,1,2)));
        ASSERT(true  == X.isHoliday(bdet_Date(2005,4,1)));
        ASSERT(false == X.isHoliday(bdet_Date(2005,4,2)));

        Obj::WeekendDaysTransitionConstIterator nextTransIt =
                                               X.beginWeekendDaysTransitions();
        Obj::WeekendDaysTransitionConstIterator curTransIt = nextTransIt++;
        for (bdet_Date tempDate = X.firstDate();
             tempDate <= X.lastDate();
             ++tempDate) {
            if (nextTransIt != X.endWeekendDaysTransitions() &&
                nextTransIt->first <= tempDate) {
                curTransIt = nextTransIt++;
            }
            bdet_DayOfWeek::Day dow = tempDate.dayOfWeek();
            LOOP_ASSERT(tempDate,
                        X.isWeekendDay(tempDate) ==
                        curTransIt->second.isMember(tempDate.dayOfWeek()));
        }


        // Testing 'isNonBusinessDay', 'numBusinessDays', and
        // 'numNonBusinessDays'.

        int nonBusinessDayCount = 0;
        for (bdet_Date tempDate = X.firstDate(); tempDate <= X.lastDate();
                                                                  ++tempDate) {
            if (veryVerbose) { P(tempDate) }

            bool nonBusinessDayFlag = X.isNonBusinessDay(tempDate);
            bool weekendDayFlag = X.isWeekendDay(tempDate);
            bool holidayFlag = X.isHoliday(tempDate);

            LOOP4_ASSERT(tempDate, nonBusinessDayFlag,
                         weekendDayFlag, holidayFlag,
                         nonBusinessDayFlag ==
                                              (holidayFlag || weekendDayFlag));


            if (nonBusinessDayFlag) ++nonBusinessDayCount;
        }
        ASSERT(X.numNonBusinessDays() == nonBusinessDayCount);
        ASSERT(X.numBusinessDays() == X.length() - nonBusinessDayCount);

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMITIVE GENERATOR FUNCTIONS 'gg' and 'ggg':
        //
        // Developing a generator also means developing a test case to verify
        // it.  We will need to select an appropriate suite of inputs specs
        // and, using basic accessors, verify that valid syntax drives the
        // object to its desired state:
        //
        // We will also want to verify that simple typo's are detected and
        // reported reliably.  In order to test that aspect, we will need to
        // supply invalid syntax.  But supplying invalid syntax to 'gg' will,
        // by design, result in a diagnostic message and a test failure.  To
        // address this testing issue, notice that the implementation of the
        // primitive generator 'gg' in Figure 9-45 has been broken into two
        // functions 'ggg' and 'gg' with the latter implemented trivially in
        // terms of the former.  The primitive generator *implementation* 'ggg'
        // exposes one additional parameter that can be used to suppress output
        // during what is sometimes called "negative testing" -- i.e., making
        // sure that the function "breaks" when it should.  Instead of
        // returning a reference, the 'ggg' function returns the index of the
        // character that caused the error and a negative value on success.
        // This extra information (useful only when testing the generator
        // itself) ensures that spec-parsing failed at the expected location
        // and not accidentally for some other reason.
        //
        // Concerns:
        //
        // We need to ensure that
        //
        //  1. The parsing stops at the first incorrect character of expression
        //  2. Absolute date parsing function accepts only valid dates
        //  3. a. Weekend days can be specified anywhere and set properly
        //     b. A duplicate weekend day will return a failure
        //     c. Only the 7 accepted lowercase letters are recognized and
        //        accepted
        //  4. The first end of the range is specified as absolute
        //  5. The second end of the range can be properly specified as an
        //     absolute or relative date
        //  6. a. Holidays can be specified as absolute or relative dates
        //     b. Relative dates are calculated against the last absolute date
        //  7. a. The 5 defined holidays codes can refer to any last set
        //        holiday
        //     b. returns a failure when trying to set a holiday which has
        //        already been attributed
        //  8. All examples in the documentation are parsed as expected
        //  9. The removeAll facility works as expected
        // 10. Optional spaces have no consequences on the parsing
        // 11. All examples in the documentation for 'hh' with no syntax errors
        //     are parsed successfully
        // 12. a. Weekend-days transition can be specified with absolute or
        //        relative dates.
        //     b. Weekend-days transtions only accepts the 7 accepted lower
        //        case letters for days considered to be weekend days
        //
        // Plan:
        //
        // To address concern 1, supply invalid vectors and verify parsing
        // stopped at the specified offset.  This concern will also be
        // implicitly tested by the vectors supplied to test the other
        // concerns.
        //
        // To address concern 2, supply vectors with a unique absolute date.
        // The dates will exercise the code in 'parseAbsoluteDate'.
        //
        // To address concern 3, supply vectors setting weekend days at
        // different positions in the test vectors and verify the results are
        // similar.  Verify that duplicates are handled correctly and no other
        // lowercase letters are accepted.
        //
        // To address concern 4, supply invalid vectors with a "relative" date
        // but no absolute date.
        //
        // To address concern 5, verify that the range is properly set when
        // supplying different absolute and relative dates.
        //
        // To address concern 6, specify a number of holidays either as
        // absolute or relative dates and verify the results are correct.  By
        // using the fact that 'addHoliday' extends the range, verify that the
        // relative dates are properly used.
        //
        // To address concern 7, set different holidays codes to specific
        // holidays and checks if duplicates are handled correctly.
        //
        // To address concern 8, supply a set of example vectors and verify
        // they are parsed correctly.
        //
        // To address concern 9, append a "remove all' command (i.e., '~') to
        // all the valid test vectors and verify that 'removeAll' is indeed
        // called.  Also supply a test vector which has '~' in the middle of
        // the string and verify the resulting calendar object matches the
        // object described by the string after the '~' symbol.
        //
        // To address concern 10, supply complex test vectors and check that
        // spaces are ignored when they should be.
        //
        // To address concern 11, generate an object using 'gg' which has the
        // same value as the object generated by 'hh' for each 'spec'.  Verify
        // these two objects are equal.
        //
        // To address concern 12, verify 'gg' with with a variety of (manually
        // selected) specs containing weekend-day transitions.  Include specs
        // that use relative and absolute dates for transition start dates, and
        // ensure that the entire set of specs covers the use of all possible
        // weekend day identifiers.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Table driven implementation method
        //
        // Testing:
        //      int ggg(bdecs_PackedCalendar *, const char *, bool=true)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'gg', and 'ggg'" << endl
                          << "=======================" << endl;

        const bdet_Date xD1(2000, 1, 2); const bdet_Date *D1 = &xD1;
        const bdet_Date xD2(2000, 1, 3); const bdet_Date *D2 = &xD2;
        const bdet_Date xD3(2000, 1, 1); const bdet_Date *D3 = &xD3;
        const bdet_Date xD4(2000, 1,15); const bdet_Date *D4 = &xD4;
        const bdet_Date xD5(2000, 1,31); const bdet_Date *D5 = &xD5;
        const bdet_Date xD6(2000, 2,15); const bdet_Date *D6 = &xD6;
        const bdet_Date xD7(2000, 2,29); const bdet_Date *D7 = &xD7;
        static struct {
            int d_lineNumber;
            const char *d_input;      // input command
            int d_retCode;            // return code of ggg()
            int d_length;             // length of the calendar
            int d_businessDays;       // number of the business days in the
                                      // calendar
            int d_weekendDaysInWeek;  // number of WE days per week in the
                                      // calendar
            int d_holidays;           // number of holidays in the calendar
            const bdet_Date *d_date_p;// date to test holidays codes for
            int d_holidayCodes;       // the number of holiday codes for d_date
        } DATA[] = {
//----------^
//Lin Input                                        RC  Len  BDs  WE Hol  D HC
// *** concern 1 ***
{ L_, "2000/1/1 3 2"                             ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "@2000/1/Asa"                              ,  8,   0,   0, 0,  0, D1, 0},
{ L_, "@2000/1/1sa"                              ,  9,   1,   1, 0,  0, D1, 0},
// *** concern 2 ***
{ L_, "0/0/0"                                    ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "@0/0/0"                                   ,  1,   0,   0, 0,  0, D1, 0},
{ L_, "@/"                                       ,  1,   0,   0, 0,  0, D1, 0},
{ L_, "@2000"                                    ,  5,   0,   0, 0,  0, D1, 0},
{ L_, "@2000/1/"                                 ,  8,   0,   0, 0,  0, D1, 0},
{ L_, "@1/0/0"                                   ,  3,   0,   0, 0,  0, D1, 0},
{ L_, "@1/1/0"                                   ,  5,   0,   0, 0,  0, D1, 0},
{ L_, "@1/1/1"                                   , -1,   1,   1, 0,  0, D1, 0},
{ L_, "@01/01/01"                                , -1,   1,   1, 0,  0, D1, 0},
{ L_, "@10000/1/1"                               ,  1,   0,   0, 0,  0, D1, 0},
{ L_, "@9999/13/1"                               ,  6,   0,   0, 0,  0, D1, 0},
{ L_, "@9999/12/35"                              ,  9,   0,   0, 0,  0, D1, 0},
{ L_, "@9999/12/31"                              , -1,   1,   1, 0,  0, D1, 0},
{ L_, "@-9999/-12/-31"                           ,  1,   0,   0, 0,  0, D1, 0},
{ L_, "@2000/2/30"                               ,  8,   0,   0, 0,  0, D1, 0},
// *** concern 3a ***
{ L_, "umtwrfa"                                  , -1,   0,   0, 7,  0, D1, 0},
{ L_, "@1/1/1umtwrfa"                            , -1,   1,   0, 7,  0, D1, 0},
{ L_, "afr@1/1/1wtmu"                            , -1,   1,   0, 7,  0, D1, 0},
{ L_, "umtwrfa@1/1/1"                            , -1,   1,   0, 7,  0, D1, 0},
// *** concern 3b ***
{ L_, "uu"                                       ,  1,   0,   0, 1,  0, D1, 0},
{ L_, "uut"                                      ,  1,   0,   0, 1,  0, D1, 0},
{ L_, "utt"                                      ,  2,   0,   0, 2,  0, D1, 0},
{ L_, "utu"                                      ,  2,   0,   0, 2,  0, D1, 0},
{ L_, "a@2000/1/1a@2001/1/1 u"                   , 10,   1,   0, 1,  0, D1, 0},
// *** concern 3c ***
{ L_, "zaf"                                      ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "azf"                                      ,  1,   0,   0, 1,  0, D1, 0},
{ L_, "afz"                                      ,  2,   0,   0, 2,  0, D1, 0},
{ L_, "z@2000/1/1 af"                            ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "a@2000/1/1  @2000/1/2zf"                  , 21,   2,   1, 1,  0, D1, 0},
{ L_, "@2000/1/1@2000/1/2 afz"                   , 21,   2,   2, 0,  0, D1, 0},
// *** concern 4 ***
{ L_, "1"                                        ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "5"                                        ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "arf15"                                    ,  3,   0,   0, 3,  0, D1, 0},
{ L_, "15arf"                                    ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "arf15"                                    ,  3,   0,   0, 3,  0, D1, 0},
// *** concern 5 ***
{ L_, "@2000/1/1 @2000/1/1"                      , -1,   1,   1, 0,  0, D1, 0},
{ L_, "@2000/1/1 0"                              , -1,   1,   1, 0,  0, D1, 0},
{ L_, "@2000/1/1 @2000/1/3"                      , -1,   3,   3, 0,  0, D1, 0},
{ L_, "@2000/1/3 @2000/1/1"                      , -1,   3,   3, 0,  0, D1, 0},
{ L_, "@2000/1/1 2"                              , -1,   3,   3, 0,  0, D1, 0},
// *** concern 6 ***
{ L_, "@2000/1/1 30 14"                          , -1,  31,  30, 0,  1, D1, 0},
{ L_, "@2000/1/1 30 @2000/1/15"                  , -1,  31,  30, 0,  1, D1, 0},
{ L_, "@2000/1/1 30 @2000/1/15 30"               , -1,  45,  43, 0,  2, D1, 0},
{ L_, "@2000/1/1 30 @2001/2/29"                  , 21,  31,  31, 0,  0, D1, 0},
{ L_, "@2000/1/1 30 14 @2001/2/29"               , 24,  31,  30, 0,  1, D1, 0},
// *** concern 7 ***
{ L_, "@2000/1/1 30 1A"                          , -1,  31,  30, 0,  1, D1, 1},
{ L_, "@2000/1/1 30 2 1A"                        , -1,  31,  29, 0,  2, D1, 1},
{ L_, "@2000/1/1 30 1AA"                         , 15,  31,  30, 0,  1, D1, 1},
{ L_, "@2000/1/1 30 2 1ABDCE 3"                  , -1,  31,  28, 0,  3, D1, 5},
{ L_, "@2000/1/1 30 1AB 2DCE"                    , -1,  31,  29, 0,  2, D1, 2},
{ L_, "@2000/1/1 30 1AB 2DCE"                    , -1,  31,  29, 0,  2, D2, 3},
{ L_, "@2000/1/1 30 @2000/1/2AB @2000/1/3DCE"    , -1,  31,  29, 0,  2, D1, 2},
{ L_, "@2000/1/1 30 @2000/1/2AB @2000/1/3DCE"    , -1,  31,  29, 0,  2, D2, 3},
{ L_, "@2000/1/1 30 @2000/1/2AB 1DCE"            , -1,  31,  29, 0,  2, D1, 2},
{ L_, "@2000/1/1 30 1AB @2000/1/3DCE"            , -1,  31,  29, 0,  2, D2, 3},
{ L_, "@2000/1/1 30 @2000/1/2AB @2000/1/32DC"    , 33,  31,  30, 0,  1, D1, 2},
// *** concern 8 ***
{ L_, ""                                         , -1,   0,   0, 0,  0, D1, 0},
{ L_, "m"                                        , -1,   0,   0, 1,  0, D1, 0},
{ L_, "ua"                                       , -1,   0,   0, 2,  0, D1, 0},
{ L_, "u@2000/1/1a"                              , -1,   1,   0, 2,  0, D1, 0},
{ L_, "@2000/1/1sa"                              ,  9,   1,   1, 0,  0, D1, 0},
{ L_, "rf@2000/1/1 @2000/1/3 @2000/1/1"          , -1,   3,   2, 2,  1, D1, 0},
{ L_, "rf@2000/1/1 2 0"                          , -1,   3,   2, 2,  1, D1, 0},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D1, 0},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D2, 0},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D3, 1},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D4, 1},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D5, 1},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D6, 0},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D7, 2},
// *** concern 9 ***
{ L_, "au@2000/1/1 30 1AB 2DCE~au@2000/1/1 30 1A", -1,  31,  21, 2,  1, D1, 1},
// *** concern 10 ***
{ L_, " @2000  /   1    /    1  30 1 A  B2  D CE", -1,  31,  29, 0,  2, D1, 2},
{ L_, " a  @   2000 /1 / 1 30 u1 AB2D C E"       , -1,  31,  24, 1,  2, D2, 3},
};
//----------v
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        if (verbose) cout << endl
                          << "\tTesting PRIMITIVE GENERATOR 'ggg'" << endl
                          << "\t=================================" << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            if (veryVerbose) { T_; P(ti); }
            const int LINE = DATA[ti].d_lineNumber;
            const char *INPUT = DATA[ti].d_input;
            const int RC = DATA[ti].d_retCode;
            const int LEN = DATA[ti].d_length;
            const int BDAY = DATA[ti].d_businessDays;
            const int WDAY = DATA[ti].d_weekendDaysInWeek;
            const int HOLI = DATA[ti].d_holidays;
            const bdet_Date DCOD = *DATA[ti].d_date_p;
            const int HCOD = DATA[ti].d_holidayCodes;

            if (veryVeryVerbose) {
                T_; T_; P(INPUT);
                T_; T_; P(RC);
                T_; T_; P(LEN);
                T_; T_; P(BDAY);
                T_; T_; P(WDAY);
                T_; T_; P(HOLI);
                T_; T_; P(DCOD);
                T_; T_; P(HCOD);
            }

            Obj mX; const Obj& X = mX;

            int retCode = ggg(&mX, INPUT, false);

            LOOP3_ASSERT(LINE, RC, retCode, RC == retCode);
            LOOP3_ASSERT(LINE, LEN, X.length(), LEN == X.length());
            LOOP3_ASSERT(LINE, BDAY, X.numBusinessDays(),
                         BDAY == X.numBusinessDays());
            LOOP3_ASSERT(LINE, WDAY, numWeekendDaysInFirstTransition(X),
                         WDAY == numWeekendDaysInFirstTransition(X))
            LOOP3_ASSERT(LINE, HOLI, X.numHolidays(),
                         HOLI == X.numHolidays());
            if (X.isInRange(DCOD)) {
                LOOP4_ASSERT(LINE, DCOD, HCOD, X.numHolidayCodes(DCOD),
                             HCOD == X.numHolidayCodes(DCOD));
            }

            // Testing the "remove all" facility

            if (retCode == -1) {
                bsl::string strInput(INPUT);
                strInput.push_back('~');

                Obj mY; const Obj& Y = mY;

                int ret = ggg(&mY, strInput.c_str(), false);
                LOOP_ASSERT(LINE, ret == -1);
                LOOP_ASSERT(LINE, 0 == Y.length());
                LOOP_ASSERT(LINE, 0 == Y.numBusinessDays());
                LOOP_ASSERT(LINE, 0 == numWeekendDaysInFirstTransition(Y));
                LOOP_ASSERT(LINE, 0 == Y.numHolidays());
            }
        }
        {
            if (verbose)
                cout <<
                    endl << "\tTesting 'gg' for Weekend-Days Transitions" <<
                    endl << "\t=========================================" <<
                    endl;

            {
                const char* spec = "@2000/1/1 30 0rw @2000/1/10mt 20";
                Obj mX; const Obj& X = mX;
                int retCode = ggg(&mX, spec, false);
                ASSERT(-1 == retCode);

                Obj::WeekendDaysTransitionConstIterator iter =
                                               X.beginWeekendDaysTransitions();
                ASSERT(sameWeekendDaysTransition(*iter,
                                                 bdet_Date(2000,1,1), "rw"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdet_Date(2000,1,10), "mt"));
                ASSERT(++iter == X.endWeekendDaysTransitions());
            }

            {
                const char* spec = "w@2000/1/1 60 0wu 15mt 30rfa";
                Obj mX; const Obj& X = mX;
                int retCode = ggg(&mX, spec, false);
                ASSERT(-1 == retCode);

                Obj::WeekendDaysTransitionConstIterator iter =
                                               X.beginWeekendDaysTransitions();
                ASSERT(sameWeekendDaysTransition(*iter,
                                                 bdet_Date(1,1,1), "w"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdet_Date(2000,1,1), "wu"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdet_Date(2000,1,16), "mt"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdet_Date(2000,1,31), "rfa"));
                ASSERT(++iter == X.endWeekendDaysTransitions());
            }

            {
                const char* spec = "w@2000/1/1 @2000/3/1 @2000/1/1wu "
                                   "@2000/1/16mt @2000/1/18e @2000/1/31rfa";
                Obj mX; const Obj& X = mX;
                int retCode = ggg(&mX, spec, false);
                ASSERT(-1 == retCode);

                Obj::WeekendDaysTransitionConstIterator iter =
                                               X.beginWeekendDaysTransitions();
                ASSERT(sameWeekendDaysTransition(*iter,
                                                 bdet_Date(1,1,1), "w"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdet_Date(2000,1,1), "wu"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdet_Date(2000,1,16), "mt"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdet_Date(2000,1,18), ""));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdet_Date(2000,1,31), "rfa"));
                ASSERT(++iter == X.endWeekendDaysTransitions());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // We want to exercise the set of primary manipulators, which can put
        // the object in any state.  We also test 'addWeekendDay'
        //
        // Concerns:
        //   1. Default constructor
        //       a. creates an object with the expected value
        //       b. is exception-neutral
        //       c. properly wires the optionally-specified allocator
        //
        //   2. That 'addDay'
        //       a. properly extends the object range if needed
        //       b. properly handles duplicates
        //       c. is exception-safe with a guarantee of rollback
        //       d. all the new days added that fall on weekend days are marked
        //          as non-business days
        //
        //   3. That 'addWeekendDay'
        //       a. properly sets the day as a weekend day
        //       b. properly handles duplicates
        //       c. marks all days that fall on the specified weekend day as
        //          non-business day
        //
        //   4. That 'addHoliday'
        //       a. properly sets the specified day as a holiday
        //       b. properly handles duplicates
        //       c. marks the specified day as non-business day
        //
        //   5. That 'addHolidayCode'
        //       a. sets the specified date as a holiday if needed
        //       b. properly sets a code for the specified holiday
        //       c. properly handles multiple codes for one holiday
        //       d. properly handles duplicates
        //       e. marks the specified day as non-business day if needed
        //
        //   6. That 'removeAll'
        //       a. produces the expected value (empty)
        //       b. leaves the object in a consistent state
        //
        //   7. That 'addWeekendDaysTransition'
        //       a. properly adds a weekend-days transition
        //       b. properly handles duplicates
        //
        //   Note that there is no "stretching" in this object.  We are
        //   adopting a black-box attitude while testing this function with
        //   regard to the containers used by the object.
        //
        // Plan:
        //   Each 'bdecs_Calendar' object contains a 'bdecs_PackedCalendar'
        //   object and a non-business day cache.  The information contained in
        //   the packed calendar and the cache must be consistent.  When new
        //   holidays or weekend days are added to the packed calendar object,
        //   they must also be marked in the cache as non-business days.
        //   Therefore, when testing methods such as 'addHoliday',
        //   'addHolidayCode', and 'addWeekendDay', it is necessary to first
        //   use the iterators to verify that the value is added, then use
        //   'isNonBusinessDay' to verify that the cache has been properly
        //   updated.
        //
        //   To address concerns for 1, create an object using the default
        //   constructor
        //     - without passing an allocator, in which case the object will
        //       allocate memory using the default allocator.
        //     - with an allocator, in which case the object will allocate
        //       memory using the specified allocator.
        //     - in the presence of exceptions during memory allocations
        //       using a 'bslma_TestAllocator' and varying its allocation
        //       limit.
        //   Use 'length' to verify that the newly-created calendar is empty.
        //   Use 'numBlocksTotal' of the default allocator and the specified
        //   allocator to verify that the constructor allocates memory from the
        //   correct source.
        //
        //   To address concerns for 2, create an object, use 'addDay' to add
        //   several days including duplicates and check if the range has been
        //   properly extended with 'isInRange' and 'length'.  Use the
        //   exception safety test macros to verify that when a memory
        //   exception is thrown while calling 'addDay', this object is rolled
        //   back.  Finally, after some weekend days are added, call 'addDay'
        //   to add a few more weeks to the calendar and verify that the
        //   weekend days among the new days being added are identified as
        //   non-business days.  The last test will be done along with the
        //   tests for concerns for 3.
        //
        //   To address concerns for 3, create an object, exercise
        //   'addWeekendDay' to set weekend days and check if it is working as
        //   expected using 'WeekendDayConstIterator'.  Also verify that all
        //   the existing weekend days in the calendar are marked as
        //   non-business days.
        //
        //   To address concerns for 4, create an object, use 'addHoliday' to
        //   add several days including duplicates and check if the holidays
        //   were correctly added using 'HolidayConstIterator'.  Also verify
        //   that the holiday being added is marked as a non-business day.
        //
        //   To address concerns for 5, create an object, use 'addHolidayCode'
        //   to add several days and codes including duplicates and check if
        //   the holiday codes were correctly added using
        //   'HolidayCodeConstIterator'.  Also verify that the holiday
        //   specified is marked as a non-business day if necessary.
        //
        //   To address concerns for 6, create an object, exercise 'addDay',
        //   'addWeekendDay' 'addHoliday', 'addHolidayCode' at will, then call
        //   'removeAll', check the value and then exercise 'addDay',
        //   'addWeekendDay', 'addHoliday' and 'addHolidayCode' to check
        //   consistency.
        //
        //   To address concerns for 7, create an object, add a set of
        //   weekend-days transitions using the 'addWeekendDayTransition'
        //   method.  Verify using the 'numWeekendDaysTransitions',
        //   'beginWeekendDaysTransitions', 'endWeekendDaysTransitions' methods
        //   that the transitions have been added correctly.  Also verify that
        //   all the existing weekend days in the calendar are marked as
        //   non-business days.
        //
        //  Testing:
        //    bdecs_Calendar(bslma_Allocator *basicAllocator = 0)
        //    void addDay(const bdet_Date& date)
        //    void addWeekendDay(bdet_DayOfWeek::Day weekendDay)
        //    void addWeekendDaysTransition(date, weekendDays)
        //    void addHoliday(const bdet_Date& date)
        //    void addHolidayCode(const bdet_Date& date, int holidayCode)
        //    void removeAll()
        //    ~bdecs_Calendar()
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING PRIMARY MANIPULATORS" << endl
                          << "============================" << endl;

        if (verbose) cout << "\nTesting default constructor." << endl;

        {
            if (verbose) cout << "\twith default allocator" << endl;

            // Temporarily install a test allocator as the default allocator in
            // order to verify the default allocator is used when no allocator
            // is specified.

            bslma_TestAllocator da; // default allocator
            const bslma_DefaultAllocatorGuard DAG(&da);
            const int previousTotal = da.numBlocksTotal();
            const Obj X, Y(0);
            ASSERT(0 == X.length());
            ASSERT(0 == Y.length());
            ASSERT(da.numBlocksTotal() >= previousTotal);
        }

        {
            if (verbose) cout << "\twith a specified allocator" << endl;
            const int previousTotal = testAllocator.numBlocksTotal();
            const Obj X(&testAllocator);
            ASSERT(0 == X.length());
            ASSERT(testAllocator.numBlocksTotal() >= previousTotal);
        }

        {
            if (verbose) cout << "\twith a specified allocator and exceptions"
                              << endl;
            BEGIN_BSLMA_EXCEPTION_TEST {
                const int previousTotal = testAllocator.numBlocksTotal();
                const Obj X(&testAllocator);
                ASSERT(0 == X.length());
                ASSERT(testAllocator.numBlocksTotal() >= previousTotal);
            } END_BSLMA_EXCEPTION_TEST
        }

        if (verbose) cout << "\nTesting 'addDay'." << endl;
        {
            Obj mX;
            const Obj& X = mX;
            ASSERT(0 == X.isInRange(bdet_Date(2000,1,1)));

            mX.addDay(bdet_Date(2000,1,2));
            ASSERT(1 == X.length());
            ASSERT(0 == X.isInRange(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1,2)));
            ASSERT(0 == X.isInRange(bdet_Date(2000,2,1)));
            ASSERT(0 == X.isInRange(bdet_Date(2001,2,1)));

            mX.addDay(bdet_Date(2000,1,2));
            ASSERT(1 == X.length());
            ASSERT(0 == X.isInRange(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1,2)));
            ASSERT(0 == X.isInRange(bdet_Date(2000,2,1)));
            ASSERT(0 == X.isInRange(bdet_Date(2001,2,1)));

            mX.addDay(bdet_Date(2000,2,1));
            ASSERT(31 == X.length());
            ASSERT(0 == X.isInRange(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1,2)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,2,1)));
            ASSERT(0 == X.isInRange(bdet_Date(2001,2,1)));

            mX.addDay(bdet_Date(2000,1,1));
            ASSERT(32 == X.length());
            ASSERT(1 == X.isInRange(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,2,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,2,1)));
            ASSERT(0 == X.isInRange(bdet_Date(2001,2,1)));

            mX.addDay(bdet_Date(2001,2,1));
            ASSERT(398 == X.length());
            ASSERT(1 == X.isInRange(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1,2)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,2,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2001,2,1)));

            mX.addDay(bdet_Date(2000,1,15));
            ASSERT(398 == X.length());
            ASSERT(1 == X.isInRange(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1,2)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,2,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2001,2,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1,15)));

            BEGIN_EXCEPTION_SAFE_TEST {
                mX.addDay(bdet_Date(2002,1,1));
            } END_EXCEPTION_SAFE_TEST(   398 == X.length()
                                      && !X.isInRange(bdet_Date(2002,1,1)))
            ASSERT(732 == X.length());
            ASSERT(1 == X.isInRange(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1,2)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,2,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2001,2,1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1,15)));
            ASSERT(1 == X.isInRange(bdet_Date(2002,1,1)));
        }
        if (verbose) cout << "\nTesting 'addWeekendDay'." << endl;
        {
            Obj mX;
            const Obj& X = mX;
            bdec_DayOfWeekSet expected;
            ASSERT(0 == X.numWeekendDaysTransitions());

            mX.addWeekendDay(bdet_DayOfWeek::BDET_SAT);
            expected.add(bdet_DayOfWeek::BDET_SAT);
            ASSERT(1 == X.numWeekendDaysTransitions());
            ASSERT(X.beginWeekendDaysTransitions()->first == bdet_Date(1,1,1));
            ASSERT(X.beginWeekendDaysTransitions()->second == expected);

            // Verify 'addDay' marks all the new weekend days as non-business
            // days.

            mX.addDay(bdet_Date(2000, 1,  1));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 1)));
            mX.addDay(bdet_Date(2000, 1, 15));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 15)));
            for (bdet_Date tempDate = X.firstDate(); tempDate <= X.lastDate();
                                                                  ++tempDate) {
                bdet_DayOfWeek::Day dow = tempDate.dayOfWeek();
                LOOP_ASSERT(tempDate, X.isNonBusinessDay(tempDate) ==
                                            (dow == bdet_DayOfWeek::BDET_SAT));
            }

            mX.addWeekendDay(bdet_DayOfWeek::BDET_MON);
            expected.add(bdet_DayOfWeek::BDET_MON);
            ASSERT(1 == X.numWeekendDaysTransitions());
            ASSERT(X.beginWeekendDaysTransitions()->first == bdet_Date(1,1,1));
            ASSERT(X.beginWeekendDaysTransitions()->second == expected);

            mX.addWeekendDay(bdet_DayOfWeek::BDET_SAT);
            ASSERT(1 == X.numWeekendDaysTransitions());
            ASSERT(X.beginWeekendDaysTransitions()->first == bdet_Date(1,1,1));
            ASSERT(X.beginWeekendDaysTransitions()->second == expected);

            // Verify that 'addWeekendDay' marks all the existing weekend days
            // in the calendar as non-business days.

            for (bdet_Date tempDate = X.firstDate(); tempDate <= X.lastDate();
                                                                  ++tempDate) {
                bdet_DayOfWeek::Day dow = tempDate.dayOfWeek();
                LOOP_ASSERT(tempDate, X.isNonBusinessDay(tempDate) ==
                                         (   dow == bdet_DayOfWeek::BDET_SAT
                                          || dow == bdet_DayOfWeek::BDET_MON));
            }
        }
        if (verbose) cout << "\nTesting 'addWeekendDaysTransition'." << endl;
        {
            Obj mX; const Obj& X = mX;
            bdet_Date date;
            bdec_DayOfWeekSet weekendDays;

            // default transition at 1/1/1
            Obj::WeekendDaysTransitionConstIterator iter =
                                               X.beginWeekendDaysTransitions();
            ASSERT(iter == X.endWeekendDaysTransitions());


            // new transition
            date.setYearMonthDay(2, 2, 2);
            weekendDays.removeAll();
            weekendDays.add(bdet_DayOfWeek::BDET_MON);
            mX.addWeekendDaysTransition(date, weekendDays);

            iter = X.beginWeekendDaysTransitions();
            ASSERT(sameWeekendDaysTransition(*iter,
                                             bdet_Date(2,2,2), "m"));
            ASSERT(++iter == X.endWeekendDaysTransitions());

            // overwrite defualt transition
            date.setYearMonthDay(1, 1, 1);
            weekendDays.removeAll();
            weekendDays.add(bdet_DayOfWeek::BDET_TUE);
            weekendDays.add(bdet_DayOfWeek::BDET_SUN);;
            mX.addWeekendDaysTransition(date, weekendDays);

            iter = X.beginWeekendDaysTransitions();
            ASSERT(sameWeekendDaysTransition(*iter,
                                             bdet_Date(1,1,1), "tu"));
            ASSERT(sameWeekendDaysTransition(*(++iter),
                                             bdet_Date(2,2,2), "m"));
            ASSERT(++iter == X.endWeekendDaysTransitions());

            // new transition
            date.setYearMonthDay(1, 2, 1);
            weekendDays.removeAll();
            weekendDays.add(bdet_DayOfWeek::BDET_WED);
            weekendDays.add(bdet_DayOfWeek::BDET_SUN);
            mX.addWeekendDaysTransition(date, weekendDays);

            iter = X.beginWeekendDaysTransitions();
            ASSERT(sameWeekendDaysTransition(*iter,
                                             bdet_Date(1,1,1), "tu"));
            ASSERT(sameWeekendDaysTransition(*(++iter),
                                             bdet_Date(1,2,1), "wu"));
            ASSERT(sameWeekendDaysTransition(*(++iter),
                                             bdet_Date(2,2,2), "m"));
            ASSERT(++iter == X.endWeekendDaysTransitions());

            // overwritting new transition
            date.setYearMonthDay(1, 2, 1);
            weekendDays.removeAll();
            weekendDays.add(bdet_DayOfWeek::BDET_MON);
            weekendDays.add(bdet_DayOfWeek::BDET_THU);
            mX.addWeekendDaysTransition(date, weekendDays);
            iter = X.beginWeekendDaysTransitions();
            ASSERT(sameWeekendDaysTransition(*iter,
                                             bdet_Date(1,1,1), "tu"));
            ASSERT(sameWeekendDaysTransition(*(++iter),
                                             bdet_Date(1,2,1), "mr"));
            ASSERT(sameWeekendDaysTransition(*(++iter),
                                             bdet_Date(2,2,2), "m"));
            ASSERT(++iter == X.endWeekendDaysTransitions());

            // Verify that 'addWeekendDayTransition' marks weekend days as
            // non-business days.

            Obj::WeekendDaysTransitionConstIterator nextTransIt =
                                               X.beginWeekendDaysTransitions();
            Obj::WeekendDaysTransitionConstIterator curTransIt = nextTransIt++;
            for (bdet_Date tempDate = X.firstDate(); tempDate <= X.lastDate();
                                                                  ++tempDate) {

                if (nextTransIt != X.endWeekendDaysTransitions() &&
                    nextTransIt->first <= tempDate) {
                    curTransIt = nextTransIt++;
                }
                bdet_DayOfWeek::Day dow = tempDate.dayOfWeek();
                LOOP_ASSERT(tempDate,
                            X.isNonBusinessDay(tempDate) ==
                            curTransIt->second.isMember(tempDate.dayOfWeek()));
            }
        }
        if (verbose) cout << "\nTesting 'addHoliday'." << endl;
        {
            Obj mX;
            const Obj& X = mX;

            mX.addDay(bdet_Date(1999,1,1));
            mX.addDay(bdet_Date(2001,1,1));

            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,7,15)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2001,1,1)));

            mX.addHoliday(bdet_Date(2000,1,1));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,7,15)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2001,1,1)));
            Obj::HolidayConstIterator i = X.beginHolidays();
            ASSERT(i != X.endHolidays());
            ASSERT(bdet_Date(2000,1,1) == *i);
            ASSERT(++i == X.endHolidays());

            mX.addHoliday(bdet_Date(2000,1,1));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,7,15)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2001,1,1)));
            i = X.beginHolidays();
            ASSERT(i != X.endHolidays());
            ASSERT(2000 == i->year());
            ASSERT(bdet_Date(2000,1,1) == *i);
            ASSERT(++i == X.endHolidays());

            mX.addHoliday(bdet_Date(2001,1,1));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,7,15)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2001,1,1)));
            i = X.beginHolidays();
            ASSERT(bdet_Date(2000,1,1) == *i);
            ASSERT(bdet_Date(2001,1,1) == *(++i));
            ASSERT(++i == X.endHolidays());

            mX.addHoliday(bdet_Date(2000,7,15));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,7,15)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2001,1,1)));
            i = X.beginHolidays();
            ASSERT(bdet_Date(2000,1, 1) == *i);
            ASSERT(bdet_Date(2000,7,15) == *(++i));
            ASSERT(bdet_Date(2001,1, 1) == *(++i));
            ASSERT(++i == X.endHolidays());

            mX.addHoliday(bdet_Date(2000,7,15));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2001,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,7,15)));
            i = X.beginHolidays();
            ASSERT(bdet_Date(2000,1, 1) == *i);
            ASSERT(bdet_Date(2000,7,15) == *(++i));
            ASSERT(bdet_Date(2001,1, 1) == *(++i));
            ASSERT(++i == X.endHolidays());

            mX.addHoliday(bdet_Date(1999,1,1));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2001,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,7,15)));
            i = X.beginHolidays();
            ASSERT(i != X.endHolidays());
            ASSERT(bdet_Date(1999,1, 1) == *i);
            ASSERT(bdet_Date(2000,1, 1) == *(++i));
            ASSERT(bdet_Date(2000,7,15) == *(++i));
            ASSERT(bdet_Date(2001,1, 1) == *(++i));
            ASSERT(++i == X.endHolidays());
        }

        if (verbose) cout << "\nTesting 'addHolidayCode'." << endl;
        {
            Obj mX;
            const Obj& X = mX;

            mX.addDay(bdet_Date(1999,1,1));
            mX.addDay(bdet_Date(2000,2,1));

            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,2,1)));

            mX.addHoliday(bdet_Date(2000,1,1));
            mX.addHolidayCode(bdet_Date(2000,1,1), 1);
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            Obj::HolidayCodeConstIterator k = X.beginHolidayCodes(
                                                 bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdet_Date(2000,1,1), 1);
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdet_Date(2000,1,1), 2);
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdet_Date(2000,1,1), 2);
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdet_Date(2000,2,1), 3);
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            Obj::HolidayConstIterator i = X.beginHolidays();
            ASSERT(bdet_Date(2000,2,1) == *(++i));
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);
            k = X.beginHolidayCodes(bdet_Date(2000,2,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) != k);
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) == ++k);

            mX.addHolidayCode(bdet_Date(1999,1,1), 1);
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            mX.addHolidayCode(bdet_Date(1999,1,1), 1);
            mX.addHolidayCode(bdet_Date(1999,1,1), 2);
            mX.addHolidayCode(bdet_Date(1999,1,1), 2);
            mX.addHolidayCode(bdet_Date(1999,1,1), 3);
            mX.addHolidayCode(bdet_Date(1999,1,1), 3);
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            i = X.beginHolidays();
            ASSERT(bdet_Date(1999,1,1) == *i);
            k = X.beginHolidayCodes(bdet_Date(1999,1,1));
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(3 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(1999,1,1)) == ++k);
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);
            k = X.beginHolidayCodes(bdet_Date(2000,2,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) != k);
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) == ++k);

            mX.addHolidayCode(bdet_Date(1999,1,1), INT_MAX);
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            mX.addHolidayCode(bdet_Date(1999,1,1), INT_MIN);
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            i = X.beginHolidays();
            ASSERT(bdet_Date(1999,1,1) == *i);
            k = X.beginHolidayCodes(bdet_Date(1999,1,1));
            ASSERT(INT_MIN == *k);
            ASSERT(1       == *(++k));
            ASSERT(2       == *(++k));
            ASSERT(3       == *(++k));
            ASSERT(INT_MAX == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(1999,1,1)) == ++k);
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);
            k = X.beginHolidayCodes(bdet_Date(2000,2,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) != k);
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) == ++k);
        }

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            Obj mX;
            const Obj& X = mX;

            mX.addWeekendDay(bdet_DayOfWeek::BDET_SUN);

            mX.addDay(bdet_Date(1999,1,1));
            mX.addDay(bdet_Date(2000,2,1));

            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,2,1)));

            mX.addHoliday(bdet_Date(2000,1,1));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            mX.addHolidayCode(bdet_Date(2000,1,1), 1);
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            Obj::HolidayCodeConstIterator k = X.beginHolidayCodes(
                                                 bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdet_Date(2000,1,1), 1);
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdet_Date(2000,1,1), 2);
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdet_Date(2000,1,1), 2);
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);

            mX.addWeekendDay(bdet_Date(2000,2,1).dayOfWeek());
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            mX.addHolidayCode(bdet_Date(2000,2,1), 3);
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            Obj::HolidayConstIterator i = X.beginHolidays();
            ASSERT(bdet_Date(2000,2,1) == *(++i));
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);
            k = X.beginHolidayCodes(bdet_Date(2000,2,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) != k);
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) == ++k);

            mX.addHolidayCode(bdet_Date(1999,1,1), 3);
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            ASSERT(1 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            i = X.beginHolidays();
            ASSERT(bdet_Date(1999,1,1) == *i);
            k = X.beginHolidayCodes(bdet_Date(1999,1,1));
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(1999,1,1)) == ++k);
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);
            k = X.beginHolidayCodes(bdet_Date(2000,2,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) != k);
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) == ++k);

            {
                bdec_DayOfWeekSet weekendDays;
                weekendDays.add(bdet_DayOfWeek::BDET_MON);
                weekendDays.add(bdet_DayOfWeek::BDET_TUE);
                mX.addWeekendDaysTransition(bdet_Date(2000,2,2), weekendDays);
            }

            mX.removeAll();

            // 'numBusinessDays' and 'numNonBusinessDays' returns the
            // numbers of 0- and 1-bits in the non-business day cache.  So
            // if both of them return 0, it means all the non-business day
            // flags in the cache have been cleared and the length of the cache
            // is 0 (which is correct).

            ASSERT(0 == X.numBusinessDays());
            ASSERT(0 == X.numNonBusinessDays());

            ASSERT(0 == X.length());
            ASSERT(0 == X.isInRange(bdet_Date(1999, 1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(2000, 1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(2000, 2, 1)));
            ASSERT(X.endHolidays() == X.beginHolidays());
            ASSERT(0 == X.numWeekendDaysTransitions());
            ASSERT(X.beginWeekendDaysTransitions() ==
                                                X.endWeekendDaysTransitions());

            mX.addWeekendDay(bdet_DayOfWeek::BDET_SUN);

            mX.addDay(bdet_Date(2000,1,1));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,1,1)));
            mX.addHoliday(bdet_Date(2000,1,1));
            mX.addHolidayCode(bdet_Date(2000,1,1), 1);
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdet_Date(2000,1,1), 1);
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdet_Date(2000,1,1), 2);
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdet_Date(2000,1,1), 2);
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);

            mX.addDay(bdet_Date(2000,2,1));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(2000,2,1)));
            mX.addWeekendDay(bdet_Date(2000,2,1).dayOfWeek());
            mX.addHolidayCode(bdet_Date(2000,2,1), 3);
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 2, 1)));
            i = X.beginHolidays();
            ASSERT(bdet_Date(2000,2,1) == *(++i));
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);
            k = X.beginHolidayCodes(bdet_Date(2000,2,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) != k);
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) == ++k);

            mX.addDay(bdet_Date(1999,1,1));
            ASSERT(0 == X.isNonBusinessDay(bdet_Date(1999,1,1)));
            mX.addHolidayCode(bdet_Date(1999,1,1), 3);
            ASSERT(1 == X.isInRange(bdet_Date(1999, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 2, 1)));
            i = X.beginHolidays();
            ASSERT(bdet_Date(1999,1,1) == *i);
            k = X.beginHolidayCodes(bdet_Date(1999,1,1));
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(1999,1,1)) == ++k);
            k = X.beginHolidayCodes(bdet_Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,1,1)) == ++k);
            k = X.beginHolidayCodes(bdet_Date(2000,2,1));
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) != k);
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdet_Date(2000,2,1)) == ++k);
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //   Developers' Sandbox.
        //
        // Concerns:
        //   We want to exercise basic value-semantic functionality.  In
        //   particular we want to demonstrate a base-line level of correct
        //   operation of the following methods and operators:
        //      - default and copy constructors (and also the destructor)
        //      - the assignment operator (including aliasing)
        //      - equality operators: 'operator==' and 'operator!='
        //      - the (test-driver supplied) output operator: 'operator<<'
        //      - primary manipulators: 'push_back' and 'clear' methods
        //      - basic accessors: 'size' and 'operator[]'
        //   In addition we would like to exercise objects with potentially
        //   different internal organizations representing the same value.
        //
        // Plan:
        //   Create four objects using both the default and copy constructors.
        //   Exercise these objects using primary manipulators, basic
        //   accessors, equality operators, and the assignment operator.
        //   Invoke the primary (black box) manipulator [3&5], copy constructor
        //   [2&8], and assignment operator [10&9] in situations where the
        //   internal data (i) does *not* and (ii) *does* have to resize.  Try
        //   aliasing with assignment for a non-empty instance [11] and allow
        //   the result to leave scope, enabling the destructor to assert
        //   internal object invariants.  Display object values frequently in
        //   verbose mode:
        //    1. Create an object x1 (default ctor).        x1:
        //    2. Create a second object x2 (copy from x1).  x1: x2:
        //    3. Append an element value A to x1).          x1:A x2:
        //    4. Append the same element value A to x2).    x1:A x2:A
        //    5. Append another element value B to x2).     x1:A x2:AB
        //    6. Remove all elements from x1.               x1: x2:AB
        //    7. Create a third object x3 (default ctor).   x1: x2:AB x3:
        //    8. Create a fourth object x4 (copy of x2).    x1: x2:AB x3: x4:AB
        //    9. Assign x2 = x1 (non-empty becomes empty).  x1: x2: x3: x4:AB
        //   10. Assign x3 = x4 (empty becomes non-empty).  x1: x2: x3:AB x4:AB
        //   11. Assign x4 = x4 (aliasing).                 x1: x2: x3:AB x4:AB
        //
        // Testing:
        //   This "test" *exercises* basic functionality, but *tests* nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const bdet_Date VA(2000,  1,  1);
        const bdet_Date VX(2000,  1,  2);
        const bdet_Date VB(2000,  1,  3);
        const bdet_Date FE(9999, 12, 31);        // First Empty
        const bdet_Date LE(   1,  1,  1);        // Last empty

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (default ctor).       "
                             "          { x1: }" << endl;
        Obj mX1; const Obj& X1 = mX1;
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout <<
            "\ta. Check initial state of x1." << endl;
        ASSERT( 0 == X1.length());
        ASSERT( 0 == X1.numHolidays());
        ASSERT(FE == X1.firstDate());
        ASSERT(LE == X1.lastDate());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1." << endl;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 2. Create a second object x2 (copy from x1). "
                             "          { x1: x2: }" << endl;
        Obj mX2(X1); const Obj& X2 = mX2;
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout <<
            "\ta. Check the initial state of x2." << endl;
        ASSERT( 0 == X1.length());
        ASSERT( 0 == X1.numHolidays());
        ASSERT(FE == X1.firstDate());
        ASSERT(LE == X1.lastDate());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));          ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));          ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 3. Append an element value A to x1).         "
                             "          { x1:A x2: }" << endl;
        mX1.addDay(VA);
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout <<
            "\ta. Check new state of x1." << endl;
        ASSERT( 1 == X1.length());
        ASSERT( 0 == X1.numHolidays());
        ASSERT( 0 == X1.isHoliday(VA));
        ASSERT(VA == X1.firstDate());
        ASSERT(VA == X1.lastDate());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));          ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 4. Append the same element value A to x2).   "
                             "          { x1:A x2:A }" << endl;
        mX2.addDay(VA);
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT( 1 == X2.length());
        ASSERT( 0 == X2.numHolidays());
        ASSERT( 0 == X2.isHoliday(VA));
        ASSERT(VA == X2.firstDate());
        ASSERT(VA == X2.lastDate());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(1 == (X2 == X1));          ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));          ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 5. Append another element value B to x2).    "
                             "          { x1:A x2:AB }" << endl;

        mX2.addDay(VB);
        mX2.addHoliday(VB);
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT( 3 == X2.length());
        ASSERT( 1 == X2.numHolidays());
        ASSERT( 0 == X2.isHoliday(VA));
        ASSERT( 0 == X2.isHoliday(VX));
        ASSERT( 1 == X2.isHoliday(VB));
        ASSERT(VA == X2.firstDate());
        ASSERT(VB == X2.lastDate());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2." << endl;
        ASSERT(0 == (X2 == X1));          ASSERT(1 == (X2 != X1));
        ASSERT(1 == (X2 == X2));          ASSERT(0 == (X2 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 6. Remove all elements from x1.              "
                             "          { x1: x2:AB }" << endl;
        mX1.removeAll();
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout <<
            "\ta. Check new state of x1." << endl;
        ASSERT( 0 == X1.length());
        ASSERT( 0 == X1.numHolidays());
        ASSERT(FE == X1.firstDate());
        ASSERT(LE == X1.lastDate());

        if (verbose) cout <<
            "\tb. Try equality operators: x1 <op> x1, x2." << endl;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == X2));          ASSERT(1 == (X1 != X2));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 7. Create a third object x3 (default ctor).  "
                             "          { x1: x2:AB x3: }" << endl;

        Obj mX3; const Obj& X3 = mX3;
        if (verbose) { cout << '\t'; P(X3); }

        if (verbose) cout <<
            "\ta. Check new state of x3." << endl;
        ASSERT( 0 == X3.length());
        ASSERT( 0 == X3.numHolidays());
        ASSERT(FE == X3.firstDate());
        ASSERT(LE == X3.lastDate());

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3." << endl;
        ASSERT(1 == (X3 == X1));          ASSERT(0 == (X3 != X1));
        ASSERT(0 == (X3 == X2));          ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));          ASSERT(0 == (X3 != X3));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 8. Create a fourth object x4 (copy of x2).   "
                             "          { x1: x2:AB x3: x4:AB }" << endl;

        Obj mX4(X2); const Obj& X4 = mX4;
        if (verbose) { cout << '\t'; P(X4); }

        if (verbose) cout <<
            "\ta. Check new state of x4." << endl;
        ASSERT( 3 == X4.length());
        ASSERT( 1 == X4.numHolidays());
        ASSERT( 0 == X4.isHoliday(VA));
        ASSERT( 0 == X4.isHoliday(VX));
        ASSERT( 1 == X4.isHoliday(VB));
        ASSERT(VA == X4.firstDate());
        ASSERT(VB == X4.lastDate());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));          ASSERT(1 == (X4 != X1));
        ASSERT(1 == (X4 == X2));          ASSERT(0 == (X4 != X2));
        ASSERT(0 == (X4 == X3));          ASSERT(1 == (X4 != X3));
        ASSERT(1 == (X4 == X4));          ASSERT(0 == (X4 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 9. Assign x2 = x1 (non-empty becomes empty). "
                             "          { x1: x2: x3: x4:AB }" << endl;
        mX2 = X1;
        if (verbose) { cout << '\t'; P(X2); }

        if (verbose) cout <<
            "\ta. Check new state of x2." << endl;
        ASSERT( 0 == X2.length());
        ASSERT( 0 == X2.numHolidays());
        ASSERT(FE == X2.firstDate());
        ASSERT(LE == X2.lastDate());

        if (verbose) cout <<
            "\tb. Try equality operators: x2 <op> x1, x2, x3, x4." << endl;
        ASSERT(1 == (X2 == X1));          ASSERT(0 == (X2 != X1));
        ASSERT(1 == (X2 == X2));          ASSERT(0 == (X2 != X2));
        ASSERT(1 == (X2 == X3));          ASSERT(0 == (X2 != X3));
        ASSERT(0 == (X2 == X4));          ASSERT(1 == (X2 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n10. Assign x3 = x4 (empty becomes non-empty). "
                             "          { x1: x2: x3:AB x4:AB }" << endl;
        mX3 = X4;
        if (verbose) { cout << '\t'; P(X3); }

        if (verbose) cout <<
            "\ta. Check new state of x3." << endl;
        ASSERT(3 == X4.length());
        ASSERT(1 == X4.numHolidays());
        ASSERT(0 == X4.isHoliday(VA));
        ASSERT(0 == X4.isHoliday(VX));
        ASSERT(1 == X4.isHoliday(VB));

        if (verbose) cout <<
            "\tb. Try equality operators: x3 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X3 == X1));          ASSERT(1 == (X3 != X1));
        ASSERT(0 == (X3 == X2));          ASSERT(1 == (X3 != X2));
        ASSERT(1 == (X3 == X3));          ASSERT(0 == (X3 != X3));
        ASSERT(1 == (X3 == X4));          ASSERT(0 == (X3 != X4));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n11. Assign x4 = x4 (aliasing).                "
                             "          { x1: x2: x3:AB x4:AB }" << endl;
        mX4 = X4;
        if (verbose) { cout << '\t'; P(X4); }

        if (verbose) cout <<
            "\ta. Check new state of x4." << endl;
        ASSERT( 3 == X4.length());
        ASSERT( 1 == X4.numHolidays());
        ASSERT( 0 == X4.isHoliday(VA));
        ASSERT( 0 == X4.isHoliday(VX));
        ASSERT(VA == X4.firstDate());
        ASSERT(VB == X4.lastDate());

        if (verbose) cout <<
            "\tb. Try equality operators: x4 <op> x1, x2, x3, x4." << endl;
        ASSERT(0 == (X4 == X1));          ASSERT(1 == (X4 != X1));
        ASSERT(0 == (X4 == X2));          ASSERT(1 == (X4 != X2));
        ASSERT(1 == (X4 == X3));          ASSERT(0 == (X4 != X3));
        ASSERT(1 == (X4 == X4));          ASSERT(0 == (X4 != X4));

        // --------------------------------------------------------------------
        // Additional functions exercised include:
        //
        // MANIPULATORS
        //  - void addDay(const bdet_Date& date);
        //  - void addHoliday(const bdet_Date& date);
        //  - int addHolidayIfInRange(const bdet_Date& date);
        //  - void addHolidayCode(const bdet_Date& date, int holidayCode);
        //
        //  ACCESSORS
        //  - const bdet_Date& firstDate() const;
        //  - const bdet_Date& lastDate() const;
        //  - bool isInRange(const bdet_Date& date) const;
        //  - bool isBusinessDay(const bdet_Date& date) const;
        //  - bool isNonBusinessDay(const bdet_Date& date) const;
        //  - bool isHoliday(const bdet_Date& date) const;
        //  - bool isWeekendDay(const bdet_Date& date) const;
        //  - bool isWeekendDay(bdet_DayOfWeek::Day dayOfWeek) const;
        //  - int length() const;
        //  - int numBusinessDays() const;
        //  - int numNonBusinessDays() const;
        //  - int numHolidays() const;
        //  - int numHolidayCodes(const bdet_Date& date) const;
        //  - int numWeekendDaysInRange() const;
        //  - int numWeekendDaysInWeek() const;
        //  - ostream& print(ostream& stream, int lev = 0, int spl = 4) const;
        //  - const bdec_DayOfWeekSet& weekendDays() const;
        //
        // Plan:
        //   This test case *exercises* basic functionality of a calendar in
        //   the following range:
        //                              (n) - indicates a holiday (W-end)
        //          Dec 1999/Jan 2000
        //         S  M Tu  W Th  F  S
        //        26 27 28 29 30 31 (1)  // Happy New Year!!!
        //         2  3  4  5  6  7  8
        //
        //   Create a calendar, add dates, holidays, and holiday codes
        //   in, after, and before the current range.
        //
        // Tactics:
        //   Ad Hoc test data selection method
        //   Brute Force test case implementation technique
        //
        // Testing:
        //   This test *exercises* basic functionality, but "tests" nothing.
        // --------------------------------------------------------------------

        if (verbose) cout << "\nCreate an empty calendar." << endl;
        Obj cal;
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //         S  M Tu  W Th  F  S

        ASSERT(FE == cal.firstDate());
        ASSERT(LE == cal.lastDate());
        ASSERT( 0 == cal.length());
        ASSERT( 0 == cal.numBusinessDays());
        ASSERT( 0 == cal.numNonBusinessDays());
        ASSERT( 0 == cal.numHolidays());
        ASSERT( 0 == cal.numWeekendDaysInRange());
        ASSERT( 0 == cal.numWeekendDaysTransitions());

        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_WED));
        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SAT));

        ASSERT( 0 == cal.isInRange(bdet_Date(   1, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdet_Date(1999,12,31)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isInRange(bdet_Date(9999,12,31)));

        // --------------------------------------------------------------------

        if (verbose) cout << "\nAdd weekend day: Sunday" << endl;
        cal.addWeekendDay(bdet_DayOfWeek::BDET_SAT);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //         S  M Tu  W Th  F (S)

        ASSERT(FE == cal.firstDate());
        ASSERT(LE == cal.lastDate());
        ASSERT( 0 == cal.length());
        ASSERT( 0 == cal.numBusinessDays());
        ASSERT( 0 == cal.numNonBusinessDays());
        ASSERT( 0 == cal.numHolidays());
        ASSERT( 0 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_WED));
        ASSERT( 1 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SAT));

        ASSERT( 0 == cal.isInRange(bdet_Date(   1, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdet_Date(1999,12,31)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isInRange(bdet_Date(9999,12,31)));

        // --------------------------------------------------------------------

        if (verbose) cout << "\nAdd Day: Jan 1, 2000." << endl;
        bdet_Date temp(2000, 1, 1);
        temp.year();
        temp.month();
        temp.day();
        cal.addDay(temp);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //         S  M Tu  W Th  F (S)
        //                           1

        ASSERT(bdet_Date(2000, 1, 1) == cal.firstDate());
        ASSERT(bdet_Date(2000, 1, 1) == cal.lastDate());
        ASSERT( 1 == cal.length());
        ASSERT( 0 == cal.numBusinessDays());
        ASSERT( 1 == cal.numNonBusinessDays());
        ASSERT( 0 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_WED));
        ASSERT( 1 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SAT));

        ASSERT( 0 == cal.isInRange(bdet_Date(   1, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isInRange(bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isInRange(bdet_Date(9999,12,31)));

        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 1)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 1)));
        ASSERT( 1 == cal.isWeekendDay(    bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 1)));

        // --------------------------------------------------------------------

        if (verbose) cout << "\nExtend the range and add Holiday: Jan 4, 2000."
                          << endl;
        cal.addDay(bdet_Date(2000, 1, 4));
        cal.addHoliday(bdet_Date(2000, 1, 4));
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //         S  M Tu  W Th  F (S)
        //                           1
        //         2  3 (4)

        ASSERT(bdet_Date(2000, 1, 1) == cal.firstDate());
        ASSERT(bdet_Date(2000, 1, 4) == cal.lastDate());
        ASSERT( 4 == cal.length());
        ASSERT( 2 == cal.numBusinessDays());
        ASSERT( 2 == cal.numNonBusinessDays());
        ASSERT( 1 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_WED));
        ASSERT( 1 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SAT));

        ASSERT( 0 == cal.isInRange(bdet_Date(   1, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isInRange(bdet_Date(2000, 1, 1)));
        ASSERT( 1 == cal.isInRange(bdet_Date(2000, 1, 2)));
        ASSERT( 1 == cal.isInRange(bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isInRange(bdet_Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 5)));
        ASSERT( 0 == cal.isInRange(bdet_Date(9999,12,31)));

        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 1)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.isWeekendDay(    bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 4)));

        if (verbose) cout << "\nAdd Day: Dec 30, 1999." << endl;
        cal.addDay(bdet_Date(1999, 12, 30));
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //         S  M Tu  W Th  F (S)
        //                    30 31  1
        //         2  3 (4)

        ASSERT(bdet_Date(1999,12,30) == cal.firstDate());
        ASSERT(bdet_Date(2000, 1, 4) == cal.lastDate());
        ASSERT( 6 == cal.length());
        ASSERT( 4 == cal.numBusinessDays());
        ASSERT( 2 == cal.numNonBusinessDays());
        ASSERT( 1 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_WED));
        ASSERT( 1 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SAT));

        ASSERT( 0 == cal.isInRange(bdet_Date(1999,12,29)));
        ASSERT( 1 == cal.isInRange(bdet_Date(1999,12,30)));
        ASSERT( 1 == cal.isInRange(bdet_Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 5)));

        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,30)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,31)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 1)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,31)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isWeekendDay(    bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,31)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 4)));

        if (verbose) cout << "\nAdd Holiday Code: Jan 1, 2000, 123." << endl;
        cal.addHolidayCode(bdet_Date(2000, 1, 1), 123);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //         S  M Tu  W Th  F (S) ,123
        //                    30 31 (1)'
        //         2  3 (4)

        ASSERT(bdet_Date(1999,12,30) == cal.firstDate());
        ASSERT(bdet_Date(2000, 1, 4) == cal.lastDate());
        ASSERT( 6 == cal.length());
        ASSERT( 4 == cal.numBusinessDays());
        ASSERT( 2 == cal.numNonBusinessDays());
        ASSERT( 2 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_WED));
        ASSERT( 1 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SAT));

        ASSERT( 0 == cal.isInRange(bdet_Date(1999,12,29)));
        ASSERT( 1 == cal.isInRange(bdet_Date(1999,12,30)));
        ASSERT( 1 == cal.isInRange(bdet_Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 5)));

        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,30)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,31)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 1)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isHoliday(       bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isWeekendDay(    bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.numHolidayCodes( bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 4)));

        if (verbose) cout << "\nAdd Holiday Code: Dec 26, 1999, 45." << endl;
        cal.addDay(bdet_Date(1999, 12, 26));
        cal.addHolidayCode(bdet_Date(1999, 12, 26), 45);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //   45.   S  M Tu  W Th  F (S) ,123
        //      `(26)27 28 29 30 31 (1)'
        //         2  3 (4)

        ASSERT(bdet_Date(1999,12,26) == cal.firstDate());
        ASSERT(bdet_Date(2000, 1, 4) == cal.lastDate());
        ASSERT(10 == cal.length());
        ASSERT( 7 == cal.numBusinessDays());
        ASSERT( 3 == cal.numNonBusinessDays());
        ASSERT( 3 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_WED));
        ASSERT( 1 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SAT));

        ASSERT( 0 == cal.isInRange(bdet_Date(1999,12,25)));
        ASSERT( 1 == cal.isInRange(bdet_Date(1999,12,26)));
        ASSERT( 1 == cal.isInRange(bdet_Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 5)));

        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(1999,12,26)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,27)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,28)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,29)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,30)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,31)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 1)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.isHoliday(       bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isHoliday(       bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isWeekendDay(    bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.numHolidayCodes( bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.numHolidayCodes( bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 4)));

        if (verbose) cout << "\nAdd Holiday Code: Jan 1, 2000, 6." << endl;
        cal.addHolidayCode(bdet_Date(2000, 1, 1), 6);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //   45.   S  M Tu  W Th  F (S) ,123,6
        //      `(26)27 28 29 30 31 (1)'
        //         2  3 (4)

        ASSERT(bdet_Date(1999,12,26) == cal.firstDate());
        ASSERT(bdet_Date(2000, 1, 4) == cal.lastDate());
        ASSERT(10 == cal.length());
        ASSERT( 7 == cal.numBusinessDays());
        ASSERT( 3 == cal.numNonBusinessDays());
        ASSERT( 3 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_WED));
        ASSERT( 1 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SAT));

        ASSERT( 0 == cal.isInRange(bdet_Date(1999,12,25)));
        ASSERT( 1 == cal.isInRange(bdet_Date(1999,12,26)));
        ASSERT( 1 == cal.isInRange(bdet_Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 5)));

        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(1999,12,26)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,27)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,28)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,29)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,30)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,31)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 1)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.isHoliday(       bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isHoliday(       bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isWeekendDay(    bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.numHolidayCodes( bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,31)));
        ASSERT( 2 == cal.numHolidayCodes( bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 4)));

        if (verbose) cout << "\nAdd Holiday Code: Jan 1, 2000, 6. (again)"
                                                                       << endl;
        cal.addHolidayCode(bdet_Date(2000, 1, 1), 6);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //   45.   S  M Tu  W Th  F (S) ,123,6!
        //      `(26)27 28 29 30 31 (1)'
        //         2  3 (4)

        ASSERT(bdet_Date(1999,12,26) == cal.firstDate());
        ASSERT(bdet_Date(2000, 1, 4) == cal.lastDate());
        ASSERT(10 == cal.length());
        ASSERT( 7 == cal.numBusinessDays());
        ASSERT( 3 == cal.numNonBusinessDays());
        ASSERT( 3 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_WED));
        ASSERT( 1 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SAT));

        ASSERT( 0 == cal.isInRange(bdet_Date(1999,12,25)));
        ASSERT( 1 == cal.isInRange(bdet_Date(1999,12,26)));
        ASSERT( 1 == cal.isInRange(bdet_Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 5)));

        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(1999,12,26)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,27)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,28)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,29)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,30)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,31)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 1)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.isHoliday(       bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isHoliday(       bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdet_Date(2000, 1, 4)));

        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isWeekendDay(    bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.numHolidayCodes( bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,31)));

        ASSERT( 2 == cal.numHolidayCodes( bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 4)));

        if (verbose) cout << "\nAdd Weekend Day: Sunday." << endl;

        cal.addWeekendDay(bdet_DayOfWeek::BDET_SUN);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //   45.  (S) M Tu  W Th  F (S) ,123,6!
        //      `(26)27 28 29 30 31 (1)'
        //         2  3 (4)

        ASSERT(bdet_Date(1999,12,26) == cal.firstDate());
        ASSERT(bdet_Date(2000, 1, 4) == cal.lastDate());
        ASSERT(10 == cal.length());
        ASSERT( 6 == cal.numBusinessDays());
        ASSERT( 4 == cal.numNonBusinessDays());
        ASSERT( 3 == cal.numHolidays());
        ASSERT( 3 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 2 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 1 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdet_DayOfWeek::BDET_WED));
        ASSERT( 1 == cal.isWeekendDay(bdet_DayOfWeek::BDET_SAT));

        ASSERT( 0 == cal.isInRange(bdet_Date(1999,12,25)));
        ASSERT( 1 == cal.isInRange(bdet_Date(1999,12,26)));
        ASSERT( 1 == cal.isInRange(bdet_Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdet_Date(2000, 1, 5)));

        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(1999,12,26)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,27)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,28)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,29)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,30)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(1999,12,31)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 1)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.isHoliday(       bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isHoliday(       bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdet_Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.isWeekendDay(    bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(1999,12,31)));
        ASSERT( 1 == cal.isWeekendDay(    bdet_Date(2000, 1, 1)));
        ASSERT( 1 == cal.isWeekendDay(    bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdet_Date(2000, 1, 4)));

        ASSERT( 1 == cal.numHolidayCodes( bdet_Date(1999,12,26)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,27)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,28)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,29)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,30)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(1999,12,31)));

        ASSERT( 2 == cal.numHolidayCodes( bdet_Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdet_Date(2000, 1, 4)));

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
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
