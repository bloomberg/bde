// bdlt_packedcalendar.t.cpp                                          -*-C++-*-
#include <bdlt_packedcalendar.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslma_testallocatorexception.h>
#include <bslma_testallocatormonitor.h>

#include <bslmf_issame.h>

#include <bsls_asserttest.h>

#include <bslx_byteinstream.h>
#include <bslx_byteoutstream.h>
#include <bslx_instreamfunctions.h>
#include <bslx_outstreamfunctions.h>
#include <bslx_testinstream.h>
#include <bslx_testinstreamexception.h>
#include <bslx_testoutstream.h>
#include <bslx_versionfunctions.h>

#include <bsl_cctype.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

using namespace BloombergLP;
using namespace bsl;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                              Overview
//                              --------
// A 'bdlt::PackedCalendar' is a value-semantic type that represents a
// collection of weekend days and holidays over a given range of dates, and
// with each holiday associated with a possibly empty list of unique (integer)
// holiday codes.  Two calendars have the same value if they (1) designate the
// same set of days as "weekend" days, (2) have the same valid range, (3) have
// the same days within that range designated as holidays, and (4) for each
// holiday, have the same set of associated integer codes.
//
// We have chosen the primary manipulators for 'bdlt::PackedCalendar' to be
// 'addDay', 'addHoliday', 'addHolidayCode', 'addWeekendDay',
// 'addWeekendDaysTransition', and 'removeAll'.
//
// We have chosen the basic accessors for 'bdlt::PackedCalendar' to be
// 'allocator', 'firstDate', 'holiday', 'holidayCode', 'isHoliday',
// 'isInRange', 'isWeekendDay', 'lastDate', 'numHolidayCodes', 'numHolidays',
// 'numWeekendDaysTransitions', and 'weekendDaysTransition'.
// ----------------------------------------------------------------------------
// ITERATORS
// [23] BusinessDayConstIterator
// [24] BusinessDayConstReverseIterator
// [21] HolidayCodeConstIterator
// [22] HolidayCodeConstReverseIterator
// [19] HolidayConstIterator
// [20] HolidayConstReverseIterator
// [25] WeekendDaysTransitionConstIterator
// [26] WeekendDaysTransitionConstReverseIterator
//
// CLASS METHODS
// [10] static int maxSupportedBdexVersion(int versionSelector);
//
// CREATORS
// [ 2] PackedCalendar(bslma::Allocator *basicAllocator = 0);
// [12] PackedCalendar(const Date& firstDate, lastDate, ba = 0);
// [ 7] PackedCalendar(const PackedCalendar& original, ba = 0);
// [ 2] ~PackedCalendar();
//
// MANIPULATORS
// [ 9] PackedCalendar& operator=(const PackedCalendar& rhs);
// [ 2] void addDay(const Date& date);
// [ 2] void addHoliday(const Date& date);
// [13] int addHolidayIfInRange(const Date& date);
// [ 2] void addHolidayCode(const Date& date, int holidayCode);
// [13] int addHolidayCodeIfInRange(const Date& date, int holidayCode);
// [ 2] void addWeekendDay(DayOfWeek::Enum weekendDay);
// [14] void addWeekendDays(const DayOfWeekSet& weekendDays);
// [ 2] void addWeekendDaysTransition(date, weekendDays);
// [18] void intersectBusinessDays(const PackedCalendar& calendar);
// [18] void intersectNonBusinessDays(const PackedCalendar& calendar);
// [ 2] void removeAll();
// [16] void removeHoliday(const Date& date);
// [16] void removeHolidayCode(const Date& date, int holidayCode);
// [17] void reserveHolidayCapacity(int numHolidays);
// [17] void reserveHolidayCodeCapacity(int numHolidayCodes);
// [15] void setValidRange(const Date& fd, const Date& ld);
// [18] void unionBusinessDays(const PackedCalendar& calendar);
// [18] void unionNonBusinessDays(const PackedCalendar& calendar);
// [10] STREAM& bdexStreamIn(STREAM& stream, int version);
// [ 8] void swap(PackedCalendar& other);
//
// ACCESSORS
// [23] BusinessDayConstIterator beginBusinessDays() const;
// [23] BusinessDayConstIterator beginBusinessDays(const Date&) const;
// [21] HolidayCodeConstIterator beginHolidayCodes(const Date&) const;
// [21] HolidayCodeConstIterator beginHolidayCodes(const HCI&) const;
// [19] HolidayConstIterator beginHolidays() const;
// [19] HolidayConstIterator beginHolidays(const Date& date) const;
// [25] WDTCI beginWeekendDaysTransitions() const;
// [23] BusinessDayConstIterator endBusinessDays() const;
// [23] BusinessDayConstIterator endBusinessDays(const Date&) const;
// [21] HolidayCodeConstIterator endHolidayCodes(const HCI&) const;
// [21] HolidayCodeConstIterator endHolidayCodes(const Date& date) const;
// [19] HolidayConstIterator endHolidays() const;
// [19] HolidayConstIterator endHolidays(const Date& date) const;
// [25] WDTCI endWeekendDaysTransitions() const;
// [ 4] const Date& firstDate() const;
// [27] int getNextBusinessDay(Date *nextBusinessDay, const Date& date);
// [27] int getNextBusinessDay(Date *nBD, const Date& date, int nth);
// [ 4] bdlt::Date holiday(int index) const;
// [ 4] int holidayCode(const Date& date, int index) const;
// [11] bool isBusinessDay(const Date& date) const;
// [ 4] bool isHoliday(const Date& date) const;
// [ 4] bool isInRange(const Date& date) const;
// [11] bool isNonBusinessDay(const Date& date) const;
// [ 4] bool isWeekendDay(const Date& date) const;
// [ 4] bool isWeekendDay(DayOfWeek::Enum dayOfWeek) const;
// [ 4] const Date& lastDate() const;
// [11] int length() const;
// [11] int numBusinessDays() const;
// [28] int numBusinessDays(beginDate, endDate) const;
// [ 4] int numHolidayCodes(const Date& date) const;
// [11] int numHolidayCodesTotal() const;
// [ 4] int numHolidays() const;
// [11] int numNonBusinessDays() const;
// [11] int numWeekendDaysInRange() const;
// [ 4] int numWeekendDaysTransitions() const;
// [24] BusinessDayConstReverseIterator rbeginBusinessDays() const;
// [24] BusinessDayConstReverseIterator rbeginBusinessDays(date) const;
// [22] HCCRI rbeginHolidayCodes(const Date&) const;
// [22] HCCRI rbeginHolidayCodes(const HCI&) const;
// [20] HolidayConstReverseIterator rbeginHolidays() const;
// [20] HolidayConstReverseIterator rbeginHolidays(const Date&) const;
// [26] WDTCRI rbeginWeekendDaysTransitions() const;
// [24] BusinessDayConstReverseIterator rendBusinessDays() const;
// [24] BusinessDayConstReverseIterator rendBusinessDays(date) const;
// [22] HCCRI rendHolidayCodes(const Date&) const;
// [22] HCCRI rendHolidayCodes(const HCI&) const;
// [20] HolidayConstReverseIterator rendHolidays() const;
// [20] HolidayConstReverseIterator rendHolidays(const Date& date) const;
// [26] WDTCRI rendWeekendDaysTransitions() const;
// [ 4] WeekendDaysTransition weekendDaysTransition(int index) const;
// [ 4] bslma::Allocator *allocator() const;
// [10] STREAM& bdexStreamOut(STREAM& stream, int version) const;
// [ 5] ostream& print(ostream& stream, int level = 0, int sPL = 4) const;
//
//
// FREE OPERATORS
// [ 6] bool operator==(lhs, rhs);
// [ 6] bool operator!=(lhs, rhs);
// [ 5] ostream& operator<<(ostream&, const PackedCalendar&);
//
// FREE FUNCTIONS
// [ 8] void swap(PackedCalendar& a, PackedCalendar& b);
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [29] USAGE EXAMPLE
// [ 3] bdlt::PackedCalendar& gg(bdlt::PackedCalendar *o, const char *s);
// [ 3] int ggg(bdlt::PackedCalendar *obj, const char *spec, bool vF);
// ============================================================================

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

#define ASSERT_SAFE_PASS_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS_RAW(EXPR)
#define ASSERT_SAFE_FAIL_RAW(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL_RAW(EXPR)
#define ASSERT_PASS_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS_RAW(EXPR)
#define ASSERT_FAIL_RAW(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)
#define ASSERT_OPT_PASS_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS_RAW(EXPR)
#define ASSERT_OPT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL_RAW(EXPR)

// ============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlt::PackedCalendar Obj;
typedef bslx::TestInStream   In;
typedef bslx::TestOutStream  Out;

#define VERSION_SELECTOR 20150612

// ============================================================================
//                             GLOBAL TEST DATA
// ----------------------------------------------------------------------------

static const char *DEFAULT_SPECS[] = {
    "",
    "@2000/1/1",
    "@2000/1/1 2",
    "@2000/1/1 2 2",
    "@2000/1/1 2au",
    "@2000/1/1 100",
    "@1999/12/31 1",
    "@0001/1/1 364",
    "@9999/01/01 364",
    "@9999/12/31",
    "@2000/1/1 0au 30tw 100rf",
    "@2012/06/03 45 5 19BD",
    "@2012/06/03 45 5 19BD 45",
    "@2014/01/01 90 10CE 14DE 17A 23ABE",
    "@2012/01/01 30 5A 19",
    "@2000/1/1 32 1ABC 2 3CDE 4ABCDE 5C",
    0 // Null string required as last element.
};

// ============================================================================
//                          TEST UTILITY FUNCTIONS
// ----------------------------------------------------------------------------

namespace {

int numWeekendDaysInFirstTransition(const Obj& calendar)
    // Return the number of weekend days in the weekend-days transition of the
    // specified 'calendar' at January 1, 0001, if the transition exists;
    // otherwise, return 0.
{

    if (1 <= calendar.numWeekendDaysTransitions() &&
        bdlt::Date(1,1,1) == calendar.beginWeekendDaysTransitions()->first) {
        return calendar.beginWeekendDaysTransitions()->second.length();
                                                                      // RETURN
    }
    return 0;
}

bool sameWeekendDaysTransition(const Obj::WeekendDaysTransition&  transition,
                               const bdlt::Date&                  date,
                               const char                        *weekendDays)
    // Return 'true' if the specified weekend-days 'transition' has a starting
    // date the same as the specified 'date' and has the same set of weekend
    // days in the specified 'weekendDays' string, which represents each day of
    // the week as a lowercase character ({u, m, t, w, r, f, a} ==
    // {Sunday..Saturday}).  Return 'false' if the said condition is not true
    // or if 'weekendDays' is not valid.
{

    bdlt::DayOfWeekSet wdSet;

    while (*weekendDays) {
        bdlt::DayOfWeek::Enum dow;
        switch (*weekendDays) {
          case 'u': dow = bdlt::DayOfWeek::e_SUN; break;
          case 'm': dow = bdlt::DayOfWeek::e_MON; break;
          case 't': dow = bdlt::DayOfWeek::e_TUE; break;
          case 'w': dow = bdlt::DayOfWeek::e_WED; break;
          case 'r': dow = bdlt::DayOfWeek::e_THU; break;
          case 'f': dow = bdlt::DayOfWeek::e_FRI; break;
          case 'a': dow = bdlt::DayOfWeek::e_SAT; break;
          default: return false;                                      // RETURN
        }
        wdSet.add(dow);
        ++weekendDays;
    };

    return Obj::WeekendDaysTransition(date, wdSet) == transition;
}

}  // close unnamed namespace

int VA = 0, VB = 1, VC = 2, VD = 100, VE = 1000; // Holiday codes.

// ============================================================================
//                FLEXIBLE GENERATOR FUNCTION 'gg' FOR TESTING
// ----------------------------------------------------------------------------
// The function 'gg' interprets a specified 'spec' in order from left to right
// according to a complex custom language to bring the calendar to a range of
// possible states relevant for testing.
//
///LANGUAGE SPECIFICATION
//-----------------------
// <SPEC>       ::= <EMPTY>
//                  | <DOW_LIST>
//                  | <START> [<DATE>|<EMPTY>] <DATEM_LIST>
//                      // Represents the first date of the calendar (along
//                      // with an associated list of weekend days in the
//                      // default weekend-days transition at January 1, 0001),
//                      // and optionally the last date of the calendar as well
//                      // as a list of holiday dates along with their holiday
//                      // codes, and weekend-days transitions.
//
// <START>      ::= [<DOW_LIST>|<EMPTY>]<ABS_DATE>[<DOW_LIST>|<EMPTY>]
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
//                      // unembellished or is followed by a list of holiday
//                      // codes, or a weekend-days transition if 'DATE' is
//                      // followed by a list of days of the week.
//
// <EMPTY>      ::=
//
// <DATE_EMBEL> ::= <DOW_LIST> | <HCODE_LIST>
//                      // Represents either a list of days of the week or a
//                      // list of holiday codes.
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
//                      // holiday codes.
//
// This language enables the explicit specification of dates representing the
// first and last date in a calendar, holiday dates, and weekend-days
// transition dates.  An absolute date is represented as @yyyy/mm/dd (but
// leading 0's may be omitted).  A relative date is represented as an unsigned
// integer offset from the start of the current range.  Note that relative
// dates cannot be used on an empty calendar.
//
// Holiday codes are represented symbolically as uppercase letters (from 'A' to
// 'E').  Days of the week to be considered weekend days are identified by
// lowercase letters ('u', 'm', 't', 'w', 'r', 'f', 'a').
//
// The first date in a spec represents one end of the range and must be
// absolute.  If the first date is embellished by weekend-day identifiers, use
// the legacy 'addWeekendDay' method to add the corresponding weekend days to
// the calendar.  The second date, if present, represents the other end of the
// range.  A subsequent date represents either a holiday or a weekend-days
// transition -- if the date is followed by holiday codes or is unembellished,
// then the date represents a holiday; otherwise, if the date is followed by
// weekend-day identifiers or the lowercase letter 'e' (representing an empty
// transition), then the date represents the starting date of a weekend-days
// transition.
//
// Any associated holiday codes should be unique for that holiday.  Whitespace
// is used to delimit integer fields with no intervening holiday codes (note
// that absolute dates and holiday codes are self-delimiting).
//
// This notation is used as follows:
//
//   Spec                 Description
//   ----                 -----------
//   ""                   Calendar is empty (no weekend days, empty range).
//
//   "m"                  [ MON ] is a weekend day, empty range.
//
//   "ua"                 [ SUN SAT ] are weekend days, empty range.
//
//   "@2000"              Ill-formed: improper absolute date format.
//
//   "@2000/1/1"          No weekend days; range is 2000/1/1..2000/1/1.
//
//   "u@2000/1/1a"        [ SUN SAT ]; range is 2001/1/1..2000/1/1.
//
//   "@2000/1/1sa"        Ill-formed: unrecognized character 's' at position 9.
//
//   "u@2000/1/1 @2000/1/10 @2000/1/1mt"
//                        Range: 2000/1/1..2000/1/10
//                        W/e Trans: [ 1/1/1: [ SUN ], 2000/1/1: [ MON TUE ] ]
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
// An implementation of a primitive generator interpreting this notation
// follows.

enum { SUCCESS = 0, FAILURE = 1 };

// Utility macros to make the code more readable.

void skipOptionalWhitespace(const char **endPos, const char *input)
    // Load into the specified 'endPos' the first position that is '!isspace',
    // or the end of the string if no such position is found, starting from
    // the specified 'input'.
{
    ASSERT(endPos);
    ASSERT(input);

    while (*input && isspace(*input)) {
        ++input;
    }

    *endPos = input;
}

int skipRequiredChar(const char **endPos, const char *input, char token)
    // If the value at the specified 'input' is equal to the specified 'token',
    // load into the specified 'endPos' the first position after 'input' and
    // return 0.  Otherwise, return a non-zero value.
{
    ASSERT(endPos);
    ASSERT(input);
    ASSERT(token);

    if (*input != token) {
        *endPos = input;
        return FAILURE;                                               // RETURN
    }
    ++input;

    *endPos = input;
    return SUCCESS;
}

int parseUint(const char **endPos, int *result, const char *input)
    // Load into the specified 'result' the value of the unsigned integer
    // starting at the specified 'input' position.  Load into the specified
    // 'endPos' the first position that is after the parsed value.  Return 0
    // if the parse succeeds, and a non-zero value otherwise.
{
    ASSERT(endPos);
    ASSERT(result);
    ASSERT(input);

    if (!isdigit(*input)) {
        *endPos = input;
        return FAILURE;                                               // RETURN
    }

    int tmp = 0;
    while (isdigit(*input)) {
        tmp *= 10;
        tmp += *input - '0';
        ++input;
    }
    *result = tmp;

    *endPos = input;
    return SUCCESS;
}

int parseAbsoluteDate(const char **endPos,
                      bdlt::Date  *result,
                      const char  *input)
    // Load into the specified 'result' the value of the 'bdlt::Date' starting
    // at the specified 'input' position.  Load into the specified 'endPos' the
    // first position that is after the parsed value.  Return 0 if the parse
    // succeeds, and a non-zero value otherwise.
{
    ASSERT(endPos);
    ASSERT(result);
    ASSERT(input);

    if (skipRequiredChar(endPos, input, '@')) {
        *endPos = input;
        return FAILURE;                                               // RETURN
    }
    input = *endPos;

    int y, m, d;

    skipOptionalWhitespace(endPos, input);
    input = *endPos;

    if (parseUint(endPos, &y, input)) {
        *endPos = input;
        return FAILURE;                                               // RETURN
    }
    if (y < 1 || y > 9999) {
        *endPos = input;
        return FAILURE;                                               // RETURN
    }
    input = *endPos;

    skipOptionalWhitespace(endPos, input);
    input = *endPos;

    if (skipRequiredChar(endPos, input, '/')) {
        *endPos = input;
        return FAILURE;                                               // RETURN
    }
    input = *endPos;

    skipOptionalWhitespace(endPos, input);
    input = *endPos;

    if (parseUint(endPos, &m, input)) {
        *endPos = input;
        return FAILURE;                                               // RETURN
    }
    if (m < 1 || m > 12) {
        *endPos = input;
        return FAILURE;                                               // RETURN
    }
    input = *endPos;

    skipOptionalWhitespace(endPos, input);
    input = *endPos;

    if (skipRequiredChar(endPos, input, '/')) {
        *endPos = input;
        return FAILURE;                                               // RETURN
    }
    input = *endPos;

    skipOptionalWhitespace(endPos, input);
    input = *endPos;

    if (parseUint(endPos, &d, input)) {
        *endPos = input;
        return FAILURE;                                               // RETURN
    }
    if (!bdlt::Date::isValidYearMonthDay(y, m, d)) {
        *endPos = input;
        return FAILURE;                                               // RETURN
    }
    input = *endPos;

    result->setYearMonthDay(y, m, d);

    *endPos = input;
    return SUCCESS;
}

int parseRelativeDate(const char        **endPos,
                      bdlt::Date         *result,
                      const char         *input,
                      const bdlt::Date&   base)
    // Load into the specified 'result' the 'bdlt::Date' equivalent to the
    // result of adding the value of the unsigned integer starting at the
    // specified 'input' position to the specified 'base' date.  Load into the
    // specified 'endPos' the first position that is after the parsed value.
    // Return 0 if the parse succeeds, and a non-zero value otherwise.
{
    ASSERT(endPos);
    ASSERT(result);
    ASSERT(input);

    int offset;
    if (parseUint(endPos, &offset, input)) {
        *endPos = input;
        return FAILURE;                                               // RETURN
    }
    input = *endPos;
    *endPos = input;

    bdlt::Date rv(base);
    if (rv.addDaysIfValid(offset)) {
        return FAILURE;                                               // RETURN
    }

    *result = rv;

    return SUCCESS;
}

int loadWeekendDay(const char           **endPosAddr,
                   bdlt::PackedCalendar  *result,
                   char                   dayOfWeek)
    // Modify the specified 'result' to include the value implied by the
    // specified 'dayOfWeek' as a weekend day.  Return a non-zero value if the
    // 'dayOfWeek' is not a valid value or if the implied 'dayOfWeek' is
    // already a weekend day in 'result', and return 0 and increment the
    // specified 'endPosAddr' otherwise.
{
    ASSERT(result);

    bdlt::DayOfWeek::Enum dow;
    switch (dayOfWeek) {
      case 'u': dow = bdlt::DayOfWeek::e_SUN; break;
      case 'm': dow = bdlt::DayOfWeek::e_MON; break;
      case 't': dow = bdlt::DayOfWeek::e_TUE; break;
      case 'w': dow = bdlt::DayOfWeek::e_WED; break;
      case 'r': dow = bdlt::DayOfWeek::e_THU; break;
      case 'f': dow = bdlt::DayOfWeek::e_FRI; break;
      case 'a': dow = bdlt::DayOfWeek::e_SAT; break;
      default: return FAILURE;                                        // RETURN
    }

    // Check for duplicates.

    int before = numWeekendDaysInFirstTransition(*result);
    result->addWeekendDay(dow);
    if (before >= numWeekendDaysInFirstTransition(*result)) {
        return FAILURE;                                               // RETURN
    }

    ++*endPosAddr;
    return SUCCESS;
}

int loadWeekendDaysTransition(const char           **endPosAddr,
                              bdlt::PackedCalendar  *result,
                              const bdlt::Date&      transitionDate)
    // Parse the characters from the specified 'endPosAddr' to produce a
    // 'bdlt::DayOfWeekSet'.  If the parse is successful, return 0, modify the
    // specified 'result' to include this 'bdlt::DayOfWeekSet' as a
    // weekend-days transition on the specified 'transitionDate', and load the
    // position past the last parsed character into 'endPosAddr'; otherwise,
    // return a non-zero value.
{
    ASSERT(result);

    bdlt::DayOfWeekSet weekendDays;

    while (islower(**endPosAddr)) {
        bdlt::DayOfWeek::Enum dow;
        bool                  emptyFlag = false;
        switch (**endPosAddr) {
          case 'u': dow = bdlt::DayOfWeek::e_SUN; break;
          case 'm': dow = bdlt::DayOfWeek::e_MON; break;
          case 't': dow = bdlt::DayOfWeek::e_TUE; break;
          case 'w': dow = bdlt::DayOfWeek::e_WED; break;
          case 'r': dow = bdlt::DayOfWeek::e_THU; break;
          case 'f': dow = bdlt::DayOfWeek::e_FRI; break;
          case 'a': dow = bdlt::DayOfWeek::e_SAT; break;
          case 'e': emptyFlag = true; break;
          default: return FAILURE;                                    // RETURN
        }
        if (emptyFlag) {
            ++*endPosAddr;
            weekendDays.removeAll();
            break;
        }

        int before = weekendDays.length();
        weekendDays.add(dow);

        // Check for duplicates.
        if (before >= weekendDays.length()) {
            return FAILURE;                                           // RETURN
        }
        ++*endPosAddr;
    }

    result->addWeekendDaysTransition(transitionDate, weekendDays);
    return SUCCESS;
}

int loadHolidayCode(const char           **endPosAddr,
                    bdlt::PackedCalendar  *result,
                    const bdlt::Date&      holiday,
                    char                   holidayCode)
    // Modify the specified 'result' to include the value implied by the
    // specified 'holidayCode' as a holiday code for the specified 'holiday'.
    // Return a non-zero value if the 'holidayCode' is not a valid value or if
    // the implied 'holidayCode' is already present for the 'holiday', and
    // return 0 and increment the specified 'endPosAddr' otherwise.
{
    ASSERT(result);
    ASSERT(0 != result->length());

    int code;
    switch (holidayCode) {
      case 'A': code = VA; break;
      case 'B': code = VB; break;
      case 'C': code = VC; break;
      case 'D': code = VD; break;
      case 'E': code = VE; break;
      default: return FAILURE;                                        // RETURN
    }

    // Check for duplicates.

    int before = result->numHolidayCodes(holiday);
    result->addHolidayCode(holiday, code);
    if (before >= result->numHolidayCodes(holiday)) {
        return FAILURE;                                               // RETURN
    }

    ++*endPosAddr;
    return SUCCESS;
}

int ggg(bdlt::PackedCalendar *object,
        const char           *spec,
        bool                  verboseFlag = true)
    // Configure the specified 'object' according to the specified 'spec',
    // using only the primary manipulators 'addDay', 'addHoliday',
    // 'addHolidayCode', 'addWeekendDay', and 'addWeekendDaysTransition'.
    // Optionally specify a 'false' 'verboseFlag' to suppress 'spec' syntax
    // error messages.  Return the index of the first invalid character, and a
    // negative value otherwise.  Note that this function is used to implement
    // 'gg' as well as allowing for verification of syntax error detection.
{
    ASSERT(object);
    ASSERT(spec);

    enum { SUCCESS = -1 };

    const char *input = spec;

    bdlt::Date lastDate;  // Reference date for holiday codes; initially
                          // assigned the default 'bdlt::Date' value.

    bool lastDateIsSet = false;

    bdlt::Date lastAbsDate;  // Reference date for relative dates; initially
                             // assigned the default 'bdlt::Date' value.

    int         numDays = 0;
    const char *endPos = input;

    while (*input) {
        if (isspace(*input)) {                     // WHITESPACE
            skipOptionalWhitespace(&endPos, input);
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
                        cout << "Error: bad weekend-days transition." << endl;
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
            bdlt::Date date;

            if ('@' == *input) {                        // absolute date
                if (parseAbsoluteDate(&endPos, &date, input)) {
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
                if (parseRelativeDate(&endPos, &date, input, lastAbsDate)) {
                    if (verboseFlag) {
                        cout << "Error: bad relative date." << endl;
                    }
                    break;
                }
                lastDate = date;      // Set reference date for holiday codes.

                // 'lastDateIsSet' is already 'true' if we are here.
            }

            if (numDays < 2) {
                object->addDay(date);
            }
            else {
                // Look ahead to see the next character to see if this date
                // represents a holiday or a weekend-days transition.
                if (0 == *endPos || isupper(*endPos) || isspace(*endPos)) {
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
                cout << "Error: unrecognized character." << endl;
            }
            break;
        }

        input = endPos; // So far so good; update input and continue.
    } // end while not done

    if (*input) {
        int idx = static_cast<int>(endPos - spec);
        if (verboseFlag) {
            cout << "An error occurred near character ('" << spec[idx]
                 << "') in spec \"" << spec << "\" at position " << idx
                 << '.' << endl;
        }

        // Discontinue processing this spec.

        return idx;                                                   // RETURN
    }

    // All input was consumed.

    return -1;
}

bdlt::PackedCalendar& gg(bdlt::PackedCalendar *object, const char *spec)
    // Return, by reference, the specified 'object' with its value adjusted
    // according to the specified 'spec' according to the custom language
    // described above.
{
    ASSERT(object);
    ASSERT(spec);
    ASSERT(ggg(object, spec) < 0);

    return *object;
}

// ============================================================================
//                                USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// The two subsections below illustrate various aspects of populating and using
// packed calendars.
//
///Example 1: Populating Packed Calendars
///- - - - - - - - - - - - - - - - - - -
// Packed calendars will typically be populated from a database or flat file.
// The user should employ an appropriate population mechanism that provides
// the desired holiday dates and associated holiday codes within some desired
// range.  For example, suppose we have created the following flat-file format
// that encodes calendar information, including holidays and holiday codes (we
// assume, for the simplicity of this example, that "Weekend Days" (i.e.,
// recurring non-business days) are always just Saturdays and Sundays):
//..
//  // HOLIDAY DATE   HOLIDAY CODES
//  // ------------   -------------
//  // Year Mon Day    #    Codes     Comments, separated by Semicolons (;)
//  // ---- --- ---   --- ---------   -------------------------------------
//     2010  1  18     1   57         ;Martin Luther King, Jr. Day
//     2010  2  15     1   51         ;Presidents Day
//     2010  4   2     2   9 105      ;Easter Sunday (Observed); Good Friday
//     2010  5  31     1   16         ;Memorial Day
//     2010  7   5     1   28         ;Independence Day (Observed)
//     2010  9   6     1   44         ;Labor Day
//     2010 10  11     1   19         ;Columbus Day
//     2010 11   2     0              ;Election Day
//     2010 11  25     1   14         ;Thanksgiving Day
//     2010 12  25     1    4         ;Christmas Day (Observed)
//     2010 12  31     1   22         ;New Year's Day (Observed)
//..
// Let's now create a couple of primitive helper functions to extract holiday
// and holiday-code counts from a given input stream.
//
// First, we'll create a helper function to get a holiday record:
//..
    int getNextHoliday(bsl::istream& input, bdlt::Date *holiday, int *numCodes)
        // Load into the specified 'holiday' the date of the next holiday, and
        // into the specified 'numCodes' the associated number of holiday codes
        // for the holiday read from the specified 'input' stream.  Return 0 on
        // success, and a non-zero value (with no effect on '*holiday' and
        // '*numCodes') otherwise.
    {
        enum { SUCCESS = 0, FAILURE = 1 };

        int year, month, day, codes;

        if (input.good()) {
            input >> year;
        }
        if (input.good()) {
            input >> month;
        }
        if (input.good()) {
            input >> day;
        }
        if (input.good()) {
            input >> codes;
        }

        if (input.good()
         && bdlt::Date::isValidYearMonthDay(year, month, day)) {
            *holiday  = bdlt::Date(year, month, day);
            *numCodes = codes;
            return SUCCESS;                                           // RETURN
        }

        return FAILURE;                                               // RETURN
    }
//..
// Then, we'll write a function that gets us an integer holiday code, or
// invalidates the stream if it cannot (note that negative holiday codes are
// not supported by this function but negative holiday codes *are* supported
// by 'bdlt::PackedCalendar'):
//..
    void getNextHolidayCode(bsl::istream& input, int *result)
        // Load, into the specified 'result', the value read from the specified
        // 'input' stream.  If the next token is not an integer, invalidate the
        // stream with no effect on 'result'.
    {
        int holidayCode;

        if (input.good()) {
            input >> holidayCode;
        }

        if (input.good()) {
            *result = holidayCode;
        }
    }
//..
// Now, with these helper functions, it is a simple matter to write a calendar
// loader function, 'load', that populates a given calendar with data in this
// "proprietary" format:
//..
    void load(bsl::istream& input, bdlt::PackedCalendar *calendar)
        // Populate the specified 'calendar' with holidays and corresponding
        // codes read from the specified 'input' stream in our "proprietary"
        // format (see above).  On success, 'input' will be empty, but valid;
        // otherwise 'input' will be invalid.
    {
        bdlt::Date holiday;
        int        numCodes;

        while (0 == getNextHoliday(input, &holiday, &numCodes)) {
            calendar->addHoliday(holiday);                       // add date
            for (int i = 0; i < numCodes; ++i) {
                int holidayCode;
                getNextHolidayCode(input, &holidayCode);
                if (input.good()) {
                    // add codes

                    calendar->addHolidayCode(holiday, holidayCode);
                }
            }
            input.ignore(256, '\n');  // skip comments
        }
    }
//..
// Finally, we load a 'bdlt::PackedCalendar' and verify some values from the
// calendar.
//..

// ***--- moved into Case 29 ---***

//..
// Note that different formats can easily be accommodated, while still using
// the same basic population strategy.  Also note that it may be substantially
// more efficient to populate calendars in increasing date order, compared to
// either reverse or random order.
//
///Example 2: Using Packed Calendars
///- - - - - - - - - - - - - - - - -
// Higher-level clients (e.g., a GUI) may need to extract the holiday codes
// for a particular date, use them to look up their corresponding string names
// in a separate repository (e.g., a vector of strings), and to display these
// names to end users.
//
// First, let's create a function that prints the names of holidays for a
// given date:
//..
    void
    printHolidayNamesForGivenDate(bsl::ostream&                   output,
                                  const bdlt::PackedCalendar&     calendar,
                                  const bdlt::Date&               date,
                                  const bsl::vector<bsl::string>& holidayNames)
        // Write, to the specified 'output' stream, the elements in the
        // specified 'holidayNames' associated, via holiday codes in the
        // specified 'calendar', to the specified 'date'.  Each holiday name
        // emitted is followed by a newline ('\n').  The behavior is undefined
        // unless 'date' is within the valid range of 'calendar'.
    {
        for (bdlt::PackedCalendar::HolidayCodeConstIterator
                                         it = calendar.beginHolidayCodes(date);
                                         it != calendar.endHolidayCodes(date);
                                       ++it) {
            output << holidayNames[*it] << bsl::endl;
        }
    }
//..
// Then, since we can write the names of holidays for a given date, let's
// write a function that can write out all of the names associated with each
// holiday in the calendar:
//..
    void
    printHolidayDatesAndNames(bsl::ostream&                   output,
                              const bdlt::PackedCalendar&     calendar,
                              const bsl::vector<bsl::string>& holidayNames)
        // Write, to the specified 'output' stream, each date associated with
        // a holiday in the specified 'calendar' followed by any elements in
        // the specified 'holidayNames' (associated via holiday codes in
        // 'calendar') corresponding to that date.  Each date emitted is
        // preceded and followed by a newline ('\n').  Each holiday name
        // emitted is followed by a newline ('\n').
    {
        for (bdlt::PackedCalendar::HolidayConstIterator
                          it = calendar.beginHolidays();
                                        it != calendar.endHolidays(); ++it) {
            output << '\n' << *it << '\n';
            printHolidayNamesForGivenDate(output,
                                          calendar,
                                          *it,
                                          holidayNames);
        }
    }
//..
// Next, we populate the 'holidayNames' vector:
//..

// ***--- moved into Case 29 ---***

// ============================================================================
//                              MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    int             verbose = argc > 2;
    int         veryVerbose = argc > 3;
    int     veryVeryVerbose = argc > 4;
    int veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    bslma::Default::setDefaultAllocator(&defaultAllocator);

    switch (test) { case 0:  // Zero is always the leading case.
      case 29: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// Example 1

    bsl::stringstream stream;
    {
        stream << "2010  9   6     1   44         ;Labor Day\n"
               << "2010 10  11     1   19         ;Columbus Day\n"
               << "2010 11   2     0              ;Election Day\n"
               << "2010 11  25     1   14         ;Thanksgiving Day\n";
    }

    bdlt::PackedCalendar calendar;
    load(stream, &calendar);

    ASSERT(bdlt::Date(2010,  9,  6) == calendar.firstDate());
    ASSERT(bdlt::Date(2010, 11, 25) == calendar.lastDate());
    ASSERT(true  == calendar.isBusinessDay(bdlt::Date(2010, 10, 12)));
    ASSERT(false == calendar.isBusinessDay(bdlt::Date(2010, 11,  2)));

// Example 2

    bsl::vector<bsl::string> holidayNames;
    {
        holidayNames.resize(45);

        holidayNames[44] = "Labor Day";         // holiday code 44 is for
                                                // Labor Day

        holidayNames[14] = "Thanksgiving Day";  // holiday code 14 is for
                                                // Thanksgiving Day
    }
//..
// Now, using the 'calendar' populated in the previous example, we print the
// holiday information to a new 'bsl::stringstream':
//..
    bsl::stringstream printStream;
//
    printHolidayDatesAndNames(printStream, calendar, holidayNames);
//..
// Finally, we verify the output:
//..
    ASSERT(printStream.str() == "\n06SEP2010\nLabor Day\n\n11OCT2010\n\n\n"
                                 "02NOV2010\n\n25NOV2010\nThanksgiving Day\n");
//..
      } break;
      case 28: {
        // -------------------------------------------------------------------
        // TESTING 'numBusinessDays(beginDate, endDate)'
        //   Ensure the non-basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 The non-basic accessor returns the expected value.
        //:
        //: 2 The non-basic accessor method is declared 'const'.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly test that the non-basic accessor, invoked on a set of
        //:   'const' objects created with the generator function, returns the
        //:   expected value using the table-driven technique.  (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   int numBusinessDays(beginDate, endDate) const;
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'numBusinessDays(beginDate, endDate)'"
                          << endl
                          << "============================================="
                          << endl;

        static const struct {
            int         d_line;    // source line number
            const char *d_spec_p;  // initial specification
            int         d_year1;   // input year1
            int         d_month1;  // input month1
            int         d_day1;    // input day1
            int         d_year2;   // input year2
            int         d_month2;  // input month2
            int         d_day2;    // input day2
            int         d_exp;     // expected number of business days
        } DATA[] = {
            //LN          SPEC            Y1   M1  D1   Y2   M2  D2  EXP
            //--  ---------------------  ----  --  --  ----  --  --  ---
            { L_,      "ua@2015/1/1 30", 2015,  1,  3, 2015,  1,  4,   0 },
            { L_,      "ua@2015/1/1 30", 2015,  1,  1, 2015,  1,  1,   1 },
            { L_,      "ua@2015/1/1 30", 2015,  1,  1, 2015,  1,  2,   2 },
            { L_,      "ua@2015/1/1 30", 2015,  1,  1, 2015,  1,  5,   3 },
            { L_,      "ua@2015/1/1 30", 2015,  1, 12, 2015,  1, 16,   5 },
            { L_,      "ua@2015/1/1 30", 2015,  1,  1, 2015,  1, 31,  22 },
            { L_,   "ua@2015/1/1 30 14", 2015,  1, 12, 2015,  1, 16,   4 },
            { L_,   "ua@2015/1/1 30 14", 2015,  1,  1, 2015,  1, 31,  21 },
            { L_, "ua@2015/1/1 30 9 14", 2015,  1,  1, 2015,  1, 31,  21 },
            { L_,   "@2015/1/1 30 14ua", 2015,  1,  1, 2015,  1, 31,  26 },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_line;
            const char *SPEC   = DATA[i].d_spec_p;
            const int   YEAR1  = DATA[i].d_year1;
            const int   MONTH1 = DATA[i].d_month1;
            const int   DAY1   = DATA[i].d_day1;
            const int   YEAR2  = DATA[i].d_year2;
            const int   MONTH2 = DATA[i].d_month2;
            const int   DAY2   = DATA[i].d_day2;
            const int   EXP    = DATA[i].d_exp;

            const bdlt::Date DATE1(YEAR1, MONTH1, DAY1);
            const bdlt::Date DATE2(YEAR2, MONTH2, DAY2);

            Obj mX;  const Obj& X = gg(&mX, SPEC);

            int rv = X.numBusinessDays(DATE1, DATE2);
            LOOP2_ASSERT(LINE, X, EXP == rv);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = gg(&mX, "@2015/1/1 30 14");

            ASSERT_PASS(X.numBusinessDays(bdlt::Date(2015,  1,  1),
                                          bdlt::Date(2015,  1, 31)));
            ASSERT_FAIL(X.numBusinessDays(bdlt::Date(2014, 12, 31),
                                          bdlt::Date(2015,  1, 31)));
            ASSERT_FAIL(X.numBusinessDays(bdlt::Date(2015,  1,  1),
                                          bdlt::Date(2015,  2,  1)));

            ASSERT_PASS(X.numBusinessDays(bdlt::Date(2015,  1, 10),
                                          bdlt::Date(2015,  1, 10)));
            ASSERT_PASS(X.numBusinessDays(bdlt::Date(2015,  1, 10),
                                          bdlt::Date(2015,  1, 11)));
            ASSERT_FAIL(X.numBusinessDays(bdlt::Date(2015,  1, 11),
                                          bdlt::Date(2015,  1, 10)));
        }
      } break;
      case 27: {
        // -------------------------------------------------------------------
        // 'nextBusinessDay' ACCESSORS
        //   Ensure both of these non-basic accessors properly interpret
        //   object state.
        //
        // Concerns:
        //: 1 Both of these non-basic accessors returns the expected value and
        //:   correctly loads the supplied 'nextBusinessDay'.
        //:
        //: 2 Each non-basic accessor method is declared 'const'.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For a set of 'const' objects created with the generator function,
        //:   compute and store all business days for the calendar.
        //:   Exhaustively verify the return value and loaded 'nextBusinessDay'
        //:   using the stored business days.  (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   int getNextBusinessDay(Date *nextBusinessDay, const Date& date);
        //   int getNextBusinessDay(Date *nBD, const Date& date, int nth);
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "'nextBusinessDay' ACCESSORS" << endl
                          << "===========================" << endl;

        const char **SPECS = DEFAULT_SPECS;

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];

            Obj mX;  const Obj& X = gg(&mX, SPEC);

            if (0 < X.length()) {
                bsl::vector<bdlt::Date> businessDay;

                // Note that the below avoids incrementing
                // 'bdlt::Date(9999, 12, 31)'.

                for (bdlt::Date date = X.firstDate();
                     date < X.lastDate();
                     ++date) {
                    if (X.isBusinessDay(date)) {
                        businessDay.push_back(date);
                    }
                }
                if (X.isBusinessDay(X.lastDate())) {
                    businessDay.push_back(X.lastDate());
                }

                int previousBusinessDayIndex = -1;

                for (bdlt::Date date = X.firstDate();
                     date < X.lastDate();
                     ++date) {
                    if (X.isBusinessDay(date)) {
                        ++previousBusinessDayIndex;
                    }

                    bdlt::Date rv;

                    if (previousBusinessDayIndex + 1 <
                                        static_cast<int>(businessDay.size())) {
                        const bdlt::Date EXP =
                                     businessDay[previousBusinessDayIndex + 1];

                        ASSERTV(ti,
                                X,
                                date,
                                0 == X.getNextBusinessDay(&rv, date));
                        ASSERTV(ti, date, EXP == rv);
                    }
                    else {
                        ASSERTV(ti,
                                X,
                                date,
                                0 != X.getNextBusinessDay(&rv, date));
                    }

                    for (int tj = 1;
                         previousBusinessDayIndex + tj <
                                          static_cast<int>(businessDay.size());
                         ++tj) {
                        const bdlt::Date EXP =
                                    businessDay[previousBusinessDayIndex + tj];

                        ASSERTV(ti,
                                X,
                                date,
                                tj,
                                0 == X.getNextBusinessDay(&rv, date, tj));
                        ASSERTV(ti, date, EXP == rv);
                    }

                    const int nthTooLarge =
                                           static_cast<int>(businessDay.size())
                                         - previousBusinessDayIndex;
                    ASSERTV(ti,
                            X,
                            date,
                            0 != X.getNextBusinessDay(&rv, date, nthTooLarge));
                }
            }
        }

        // Negative testing.

        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = gg(&mX, "@2014/1/1 30 14");

            bdlt::Date date;

            ASSERT_SAFE_FAIL(X.getNextBusinessDay(&date, X.firstDate() - 2));
            ASSERT_SAFE_PASS(X.getNextBusinessDay(&date, X.firstDate() - 1));
            ASSERT_SAFE_PASS(X.getNextBusinessDay(&date, X.lastDate() - 1));
            ASSERT_SAFE_FAIL(X.getNextBusinessDay(&date, X.lastDate()));
            ASSERT_SAFE_FAIL(X.getNextBusinessDay(&date,
                                                  bdlt::Date(9999, 12, 31)));
            ASSERT_SAFE_FAIL(X.getNextBusinessDay(0, X.firstDate() - 1));

            ASSERT_FAIL(X.getNextBusinessDay(&date, X.firstDate() - 2, 1));
            ASSERT_PASS(X.getNextBusinessDay(&date, X.firstDate() - 1, 1));
            ASSERT_PASS(X.getNextBusinessDay(&date, X.lastDate() - 1, 1));
            ASSERT_FAIL(X.getNextBusinessDay(&date, X.lastDate(), 1));
            ASSERT_FAIL(X.getNextBusinessDay(&date,
                                             bdlt::Date(9999, 12, 31),
                                             1));
            ASSERT_FAIL(X.getNextBusinessDay(&date, X.firstDate() - 1, 0));
            ASSERT_FAIL(X.getNextBusinessDay(0, X.firstDate() - 1, 1));
        }
      } break;
      case 26: {
        // -------------------------------------------------------------------
        // TESTING 'WeekendDaysTransitionConstReverseIterator'
        //   Ensure that this iterator and the associated 'rbegin' and 'rend'
        //   methods behave as expected.
        //
        // Concerns:
        //: 1 The iterator's type traits are appropriately defined.
        //:
        //: 2 The iterator can be used to iterate from 'rbegin' to 'rend' using
        //:   'operator++'.
        //:
        //: 3 The iterator can be used to iterate from 'rend' to 'rbegin' using
        //:   'operator--'.
        //:
        //: 4 The methods 'operator*' and 'operator->' work as expected.
        //:
        //: 5 The equality operators work as expected.
        //:
        //: 6 The constructors and assignment operator work as expected.
        //
        // Plan:
        //: 1 Directly verify the type traits of the iterator.  (C-1)
        //:
        //: 2 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'rbegin' to the 'rend' of each calendar and verify the value
        //:   referenced by the iterator with the 'weekendDaysTransition'
        //:   accessor from 'bdlt::PackedCalendar'.  (C-2)
        //:
        //: 3 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'rend' to the 'rbegin' of each calendar and verify the value
        //:   referenced by the iterator with the 'weekendDaysTransition'
        //:   accessor from 'bdlt::PackedCalendar'.  (C-3..4)
        //:
        //: 4 For a set of 'bdlt::PackedCalendar' values, test the equality
        //:   operators with all possible pairs of iterator values from each
        //:   calendar value.  Also test the constructors and assignment
        //:   operator.  (C-5..6)
        //
        // Testing:
        //   WeekendDaysTransitionConstReverseIterator
        //   WDTCRI rbeginWeekendDaysTransitions() const;
        //   WDTCRI rendWeekendDaysTransitions() const;
        // -------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'WeekendDaysTransitionConstReverseIterator'"
                 << endl
                 << "==================================================="
                 << endl;
        }

        typedef bdlt::PackedCalendar::WeekendDaysTransitionConstReverseIterator
                                                                      Iterator;

        const char **SPECS = DEFAULT_SPECS;

        // Verify the type trait values.

        {
            typedef bsl::iterator_traits<Iterator> IterTraits;

            ASSERT((bsl::is_same<IterTraits::value_type,
                         bdlt::PackedCalendar::WeekendDaysTransition>::value));

            ASSERT((bsl::is_same<IterTraits::difference_type,
                                 bsl::ptrdiff_t>::value));

            ASSERT((bsl::is_same<IterTraits::pointer,
                 const bdlt::PackedCalendar::WeekendDaysTransition *>::value));

            ASSERT((bsl::is_same<IterTraits::reference,
                  const bdlt::PackedCalendar::WeekendDaysTransition&>::value));
        }

        // Verify iterator functionality.

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];

            Obj mX;  const Obj& X = gg(&mX, SPEC);

            // Bootstrap testing.

            {
                Iterator        mA = X.rbeginWeekendDaysTransitions();
                const Iterator& A  = mA;

                Iterator        mB = X.rendWeekendDaysTransitions();
                const Iterator& B  = mB;

                ASSERTV(ti,
                        X,
                        (0 == X.numWeekendDaysTransitions()) == (A == B));

                if (X.numWeekendDaysTransitions()) {
                    Obj mY;  const Obj& Y = gg(&mY, SPEC);

                    Iterator        mC = Y.rbeginWeekendDaysTransitions();
                    const Iterator& C  = mC;

                    Iterator        mD = Y.rendWeekendDaysTransitions();
                    const Iterator& D  = mD;

                    ASSERTV(ti, X, A != C);
                    ASSERTV(ti, X, B != D);
                }
            }

            // Test iterating in the forward direction, 'operator*' and
            // 'operator->'.

            {
                Iterator        mA = X.rbeginWeekendDaysTransitions();
                const Iterator& A  = mA;

                Iterator        mB = X.rendWeekendDaysTransitions();
                const Iterator& B  = mB;

                Iterator        mC(mA);
                const Iterator& C = mC;

                int index = X.numWeekendDaysTransitions() - 1;
                while (A != B) {
                    ASSERTV(ti,
                            X,
                            index,
                            X.weekendDaysTransition(index) == *A);

                    ASSERTV(ti,
                            X,
                            index,
                            X.weekendDaysTransition(index).first == A->first);

                    ASSERTV(ti,
                            X,
                            index,
                            X.weekendDaysTransition(index).second
                                                                 == A->second);

                    --index;
                    ++mA;

                    mC++;
                    ASSERT(C == A);
                }
                ASSERTV(ti, X, -1 == index);
            }

            // Test iterating in the reverse direction, 'operator*' and
            // 'operator->'.

            if (X.numWeekendDaysTransitions()) {
                Iterator        mA = X.rbeginWeekendDaysTransitions();
                const Iterator& A  = mA;

                Iterator        mB = X.rendWeekendDaysTransitions();
                const Iterator& B  = mB;

                Iterator        mC(mB);
                const Iterator& C = mC;

                int index = -1;
                while (B != A) {
                    --mB;
                    ++index;

                    ASSERTV(ti,
                            X,
                            index,
                            X.weekendDaysTransition(index) == *B);

                    ASSERTV(ti,
                            X,
                            index,
                            X.weekendDaysTransition(index).first == B->first);

                    ASSERTV(ti,
                            X,
                            index,
                            X.weekendDaysTransition(index).second
                                                                 == B->second);

                    mC--;
                    ASSERT(C == B);
                }
                ASSERTV(ti, X, X.numWeekendDaysTransitions() - 1 == index);
            }

            // Test assignment and equality operators.

            Iterator        mA = X.rbeginWeekendDaysTransitions();
            const Iterator& A  = mA;

            for (int tj = 0; tj < X.numWeekendDaysTransitions(); ++tj, ++mA) {
                Iterator        mB = X.rbeginWeekendDaysTransitions();
                const Iterator& B  = mB;
                for (int tk = 0;
                     tk < X.numWeekendDaysTransitions();
                     ++tk, ++mB) {
                    ASSERTV(X, tj, tk, (tj == tk) == (A == B));
                    ASSERTV(X, tj, tk, (tj == tk) == (B == A));
                    ASSERTV(X, tj, tk, (tj != tk) == (A != B));
                    ASSERTV(X, tj, tk, (tj != tk) == (B != A));

                    Iterator mC(mA);  const Iterator& C = mC;
                    ASSERTV(X, tj, C == A);

                    mC = B;
                    ASSERTV(X, tj, C == B);
                }

                ASSERTV(X, tj, false == (A == X.rendWeekendDaysTransitions()));
                ASSERTV(X, tj, true  == (A != X.rendWeekendDaysTransitions()));
                ASSERTV(X, tj, false == (X.rendWeekendDaysTransitions() == A));
                ASSERTV(X, tj, true  == (X.rendWeekendDaysTransitions() != A));
            }
            ASSERTV(X, true  == (X.rendWeekendDaysTransitions()
                                           == X.rendWeekendDaysTransitions()));
            ASSERTV(X, false == (X.rendWeekendDaysTransitions()
                                           != X.rendWeekendDaysTransitions()));
        }
      } break;
      case 25: {
        // -------------------------------------------------------------------
        // TESTING 'WeekendDaysTransitionConstIterator'
        //   Ensure that this iterator and the associated 'begin' and 'end'
        //   methods behave as expected.
        //
        // Concerns:
        //: 1 The iterator's type traits are appropriately defined.
        //:
        //: 2 The iterator can be used to iterate from 'begin' to 'end' using
        //:   'operator++'.
        //:
        //: 3 The iterator can be used to iterate from 'end' to 'begin' using
        //:   'operator--'.
        //:
        //: 4 The methods 'operator*' and 'operator->' work as expected.
        //:
        //: 5 The equality operators work as expected.
        //:
        //: 6 The constructors and assignment operator work as expected.
        //
        // Plan:
        //: 1 Directly verify the type traits of the iterator.  (C-1)
        //:
        //: 2 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'begin' to the 'end' of each calendar and verify the value
        //:   referenced by the iterator with the 'weekendDaysTransition'
        //:   accessor from 'bdlt::PackedCalendar'.  (C-2)
        //:
        //: 3 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'end' to the 'begin' of each calendar and verify the value
        //:   referenced by the iterator with the 'weekendDaysTransition'
        //:   accessor from 'bdlt::PackedCalendar'.  (C-3..4)
        //:
        //: 4 For a set of 'bdlt::PackedCalendar' values, test the equality
        //:   operators with all possible pairs of iterator values from each
        //:   calendar value.  Also test the constructors and assignment
        //:   operator.  (C-5..6)
        //
        // Testing:
        //   WeekendDaysTransitionConstIterator
        //   WDTCI beginWeekendDaysTransitions() const;
        //   WDTCI endWeekendDaysTransitions() const;
        // -------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'WeekendDaysTransitionConstIterator'" << endl
                 << "============================================" << endl;
        }

        typedef bdlt::PackedCalendar::WeekendDaysTransitionConstIterator
                                                                      Iterator;

        const char **SPECS = DEFAULT_SPECS;

        // Verify the type trait values.

        {
            typedef bsl::iterator_traits<Iterator> IterTraits;

            ASSERT((bsl::is_same<IterTraits::value_type,
                         bdlt::PackedCalendar::WeekendDaysTransition>::value));

            ASSERT((bsl::is_same<IterTraits::difference_type,
                                 bsl::ptrdiff_t>::value));

            ASSERT((bsl::is_same<IterTraits::pointer,
                 const bdlt::PackedCalendar::WeekendDaysTransition *>::value));

            ASSERT((bsl::is_same<IterTraits::reference,
                  const bdlt::PackedCalendar::WeekendDaysTransition&>::value));
        }

        // Verify iterator functionality.

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];

            Obj mX;  const Obj& X = gg(&mX, SPEC);

            // Bootstrap testing.

            {
                Iterator        mA = X.beginWeekendDaysTransitions();
                const Iterator& A  = mA;
                Iterator        mB = X.endWeekendDaysTransitions();
                const Iterator& B  = mB;

                ASSERTV(ti,
                        X,
                        (0 == X.numWeekendDaysTransitions()) == (A == B));

                if (X.numWeekendDaysTransitions()) {
                    Obj mY;  const Obj& Y = gg(&mY, SPEC);

                    Iterator        mC = Y.beginWeekendDaysTransitions();
                    const Iterator& C  = mC;
                    Iterator        mD = Y.endWeekendDaysTransitions();
                    const Iterator& D  = mD;

                    ASSERTV(ti, X, A != C);
                    ASSERTV(ti, X, B != D);
                }
            }

            // Test iterating in the forward direction, 'operator*' and
            // 'operator->'.

            {
                Iterator        mA = X.beginWeekendDaysTransitions();
                const Iterator& A  = mA;

                Iterator        mB = X.endWeekendDaysTransitions();
                const Iterator& B  = mB;

                Iterator        mC(mA);
                const Iterator& C  = mC;

                int index = 0;
                while (A != B) {
                    ASSERTV(ti,
                            X,
                            index,
                            X.weekendDaysTransition(index) == *A);

                    ASSERTV(ti,
                            X,
                            index,
                            X.weekendDaysTransition(index).first == A->first);

                    ASSERTV(ti,
                            X,
                            index,
                            X.weekendDaysTransition(index).second
                                                                 == A->second);

                    ++index;
                    ++mA;

                    mC++;
                    ASSERT(C == A);
                }
                ASSERTV(ti, X, X.numWeekendDaysTransitions() == index);
            }

            // Test iterating in the reverse direction, 'operator*' and
            // 'operator->'.

            if (X.numWeekendDaysTransitions()) {
                Iterator        mA = X.beginWeekendDaysTransitions();
                const Iterator& A  = mA;

                Iterator        mB = X.endWeekendDaysTransitions();
                const Iterator& B  = mB;

                Iterator        mC(mB);
                const Iterator& C  = mC;

                int index = X.numWeekendDaysTransitions();
                while (B != A) {
                    --mB;
                    --index;

                    ASSERTV(ti,
                            X,
                            index,
                            X.weekendDaysTransition(index) == *B);

                    ASSERTV(ti,
                            X,
                            index,
                            X.weekendDaysTransition(index).first == B->first);

                    ASSERTV(ti,
                            X,
                            index,
                            X.weekendDaysTransition(index).second
                                                                 == B->second);

                    mC--;
                    ASSERT(C == B);
                }
                ASSERTV(ti, X, 0 == index);
            }

            // Test assignment and equality operators.

            Iterator        mA = X.beginWeekendDaysTransitions();
            const Iterator& A  = mA;
            for (int tj = 0; tj < X.numWeekendDaysTransitions(); ++tj, ++mA) {
                Iterator        mB = X.beginWeekendDaysTransitions();
                const Iterator& B  = mB;
                for (int tk = 0;
                     tk < X.numWeekendDaysTransitions();
                     ++tk, ++mB) {
                    ASSERTV(X, tj, tk, (tj == tk) == (A == B));
                    ASSERTV(X, tj, tk, (tj == tk) == (B == A));
                    ASSERTV(X, tj, tk, (tj != tk) == (A != B));
                    ASSERTV(X, tj, tk, (tj != tk) == (B != A));

                    Iterator mC(mA);  const Iterator& C = mC;
                    ASSERTV(X, tj, C == A);

                    mC = B;
                    ASSERTV(X, tj, C == B);
                }

                ASSERTV(X, tj, false == (A == X.endWeekendDaysTransitions()));
                ASSERTV(X, tj, true  == (A != X.endWeekendDaysTransitions()));
                ASSERTV(X, tj, false == (X.endWeekendDaysTransitions() == A));
                ASSERTV(X, tj, true  == (X.endWeekendDaysTransitions() != A));
            }
            ASSERTV(X, true  == (X.endWeekendDaysTransitions()
                                            == X.endWeekendDaysTransitions()));
            ASSERTV(X, false == (X.endWeekendDaysTransitions()
                                            != X.endWeekendDaysTransitions()));
        }
      } break;
      case 24: {
        // -------------------------------------------------------------------
        // TESTING 'BusinessDayConstReverseIterator'
        //   Ensure that this iterator and the associated 'rbegin' and 'rend'
        //   methods behave as expected.
        //
        // Concerns:
        //: 1 The iterator's type traits are appropriately defined.
        //:
        //: 2 The iterator can be used to iterate from 'rbegin' to 'rend' using
        //:   'operator++'.
        //:
        //: 3 The iterator can be used to iterate from 'rend' to 'rbegin' using
        //:   'operator--'.
        //:
        //: 4 The methods 'operator*' and 'operator->' work as expected.
        //:
        //: 5 The equality operators work as expected.
        //:
        //: 6 The constructors and assignment operator work as expected.
        //:
        //: 7 The methods 'rbeginBusinessDays(date)' and
        //:   'rendBusinessDays(date)' work as expected.
        //:
        //: 8 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly verify the type traits of the iterator.  (C-1)
        //:
        //: 2 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'rbegin' to the 'rend' of each calendar and verify the value
        //:   referenced by the iterator with the 'isBusinessDay' accessor from
        //:   'bdlt::PackedCalendar'.  (C-2)
        //:
        //: 3 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'rend' to the 'rbegin' of each calendar and verify the value
        //:   referenced by the iterator with the 'isBusinessDay' accessor from
        //:   'bdlt::PackedCalendar'.  (C-3..4)
        //:
        //: 4 For a set of 'bdlt::PackedCalendar' values, test the equality
        //:   operators with all possible pairs of iterator values from each
        //:   calendar value.  Also test the constructors and assignment
        //:   operator.  (C-5..6)
        //:
        //: 5 For a set of 'bdlt::PackedCalendar' values, for all dates within
        //:   the range of the calendar verify the return values of
        //:   'rbeginBusinessDays(date)' and 'rendBusinessDays(date)'.  (C-7)
        //:
        //: 6 Verify defensive checks are triggered for invalid values.  (C-8)
        //
        // Testing:
        //   BusinessDayConstReverseIterator
        //   BusinessDayConstReverseIterator rbeginBusinessDays() const;
        //   BusinessDayConstReverseIterator rbeginBusinessDays(date) const;
        //   BusinessDayConstReverseIterator rendBusinessDays() const;
        //   BusinessDayConstReverseIterator rendBusinessDays(date) const;
        // -------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'BusinessDayConstReverseIterator'" << endl
                 << "=========================================" << endl;
        }

        typedef bdlt::PackedCalendar::BusinessDayConstReverseIterator Iterator;

        const char **SPECS = DEFAULT_SPECS;

        // Verify the type trait values.
        {
            ASSERT((bsl::is_same<Iterator::value_type, bdlt::Date>::value));

            ASSERT((bsl::is_same<Iterator::difference_type, int>::value));

            ASSERT((bsl::is_same<Iterator::pointer,
                                      bdlt::PackedCalendar_DateProxy>::value));

            ASSERT((bsl::is_same<Iterator::reference,
                                 bdlt::PackedCalendar_DateRef>::value));
        }

        // Verify iterator functionality.

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];

            Obj mX;  const Obj& X = gg(&mX, SPEC);

            // Bootstrap testing.

            {
                Iterator mA = X.rbeginBusinessDays();  const Iterator& A = mA;

                Iterator mB = X.rendBusinessDays();    const Iterator& B = mB;

                ASSERTV(ti, X, (0 == X.numBusinessDays()) == (A == B));
            }

            // Test iterating in the forward direction, 'operator*' and
            // 'operator->'.

            {
                Iterator mA = X.rbeginBusinessDays();  const Iterator& A = mA;

                Iterator mB = X.rendBusinessDays();    const Iterator& B = mB;

                Iterator mC(mA);                       const Iterator& C = mC;

                int offset = 0;
                int count  = 0;
                while (A != B) {
                    ASSERTV(ti, X, offset, X.isBusinessDay(*A));
                    while (X.lastDate() - offset > *A) {
                        ASSERTV(ti,
                                X,
                                offset,
                                !X.isBusinessDay(X.lastDate() - offset));
                        ++offset;
                    }

                    int year;
                    int month;
                    int day;
                    A->getYearMonthDay(&year, &month, &day);
                    ASSERTV(ti,
                            X,
                            offset,
                            X.lastDate() - offset == bdlt::Date(year,
                                                                month,
                                                                day));

                    ++offset;
                    ++count;
                    ++mA;

                    mC++;
                    ASSERT(C == A);
                }

                while (offset < X.length()) {
                    ASSERTV(ti,
                            X,
                            offset,
                            !X.isBusinessDay(X.lastDate() - offset));
                    ++offset;
                }

                ASSERTV(ti, X, X.numBusinessDays() == count);
            }

            // Test iterating in the reverse direction, 'operator*' and
            // 'operator->'.

            if (X.numBusinessDays()) {
                Iterator mA = X.rbeginBusinessDays();  const Iterator& A = mA;

                Iterator mB = X.rendBusinessDays();    const Iterator& B = mB;

                Iterator mC(mB);                       const Iterator& C = mC;

                int offset = 0;
                int count  = 0;
                while (B != A) {
                    --mB;

                    ASSERTV(ti, X, offset, X.isBusinessDay(*B));
                    while (X.firstDate() + offset < *B) {
                        ASSERTV(ti,
                                X,
                                offset,
                                !X.isBusinessDay(X.firstDate() + offset));
                        ++offset;
                    }

                    int year;
                    int month;
                    int day;
                    B->getYearMonthDay(&year, &month, &day);
                    ASSERTV(ti,
                            X,
                            offset,
                            X.firstDate() + offset == bdlt::Date(year,
                                                                 month,
                                                                 day));

                    ++offset;
                    ++count;

                    mC--;
                    ASSERT(C == B);
                }

                while (offset < X.length()) {
                    ASSERTV(ti,
                            X,
                            offset,
                            !X.isBusinessDay(X.firstDate() + offset));
                    ++offset;
                }

                ASSERTV(ti, X, X.numBusinessDays() == count);
            }

            // Test assignment and equality operators.

            Iterator mA = X.rbeginBusinessDays();  const Iterator& A = mA;
            for (int tj = 0; tj < X.numBusinessDays(); ++tj, ++mA) {
                Iterator mB = X.rbeginBusinessDays();  const Iterator& B = mB;
                for (int tk = 0; tk < X.numBusinessDays(); ++tk, ++mB) {
                    ASSERTV(X, tj, tk, (tj == tk) == (A == B));
                    ASSERTV(X, tj, tk, (tj == tk) == (B == A));
                    ASSERTV(X, tj, tk, (tj != tk) == (A != B));
                    ASSERTV(X, tj, tk, (tj != tk) == (B != A));

                    Iterator mC(mA);  const Iterator& C = mC;
                    ASSERTV(X, tj, C == A);

                    mC = B;
                    ASSERTV(X, tj, C == B);
                }

                ASSERTV(X, tj, false == (A == X.rendBusinessDays()));
                ASSERTV(X, tj, true  == (A != X.rendBusinessDays()));
                ASSERTV(X, tj, false == (X.rendBusinessDays() == A));
                ASSERTV(X, tj, true  == (X.rendBusinessDays() != A));
            }
            ASSERTV(X,
                    true  == (X.rendBusinessDays() == X.rendBusinessDays()));
            ASSERTV(X,
                    false == (X.rendBusinessDays() != X.rendBusinessDays()));

            // Testing 'rbeginBusinessDays(date)' and 'rendBusinessDays(date)'.

            for (int offset = 0; offset < X.length(); ++offset) {
                bdlt::Date date = X.lastDate() - offset;

                Iterator        mA = X.rbeginBusinessDays(date);
                const Iterator& A  = mA;
                Iterator        mB = X.rendBusinessDays(date);
                const Iterator& B = mB;

                if (X.isBusinessDay(date)) {
                    ASSERTV(X, date, *A == date);
                    ++mA;
                    ASSERTV(X, date, A == B);
                }
                else {
                    ASSERTV(X, date, A == X.rendBusinessDays() || *A < date);
                    ASSERTV(X, date, A == B);

                    if (A != X.rbeginBusinessDays()) {
                        --mA;
                        ASSERTV(X, date, *A > date);
                    }
                }
            }
        }

        // Negative testing.

        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = gg(&mX, "@2014/1/1 30 14");

            ASSERT_SAFE_PASS(X.rbeginBusinessDays());
            ASSERT_SAFE_FAIL(--X.rbeginBusinessDays());

            Iterator mA = X.rbeginBusinessDays();
            ASSERT_SAFE_FAIL(mA--);

            ASSERT_SAFE_PASS(X.rendBusinessDays());
            ASSERT_SAFE_FAIL(++X.rendBusinessDays());

            mA = X.rendBusinessDays();
            ASSERT_SAFE_FAIL(mA++);

            ASSERT_SAFE_PASS(X.rbeginBusinessDays(bdlt::Date(2014, 1, 6)));
            ASSERT_SAFE_FAIL(X.rbeginBusinessDays(bdlt::Date(2014, 3, 6)));

            ASSERT_SAFE_PASS(X.rendBusinessDays(bdlt::Date(2014, 1, 6)));
            ASSERT_SAFE_FAIL(X.rendBusinessDays(bdlt::Date(2014, 3, 6)));

            Obj mY;  const Obj& Y = gg(&mY, "@2014/1/1 30 14");

            Iterator  mB = Y.rbeginBusinessDays();
            ASSERT_SAFE_FAIL_RAW(mA == mB);
            ASSERT_SAFE_FAIL_RAW(mA != mB);
        }
      } break;
      case 23: {
        // -------------------------------------------------------------------
        // TESTING 'BusinessDayConstIterator'
        //   Ensure that this iterator and the associated 'begin' and 'end'
        //   methods behave as expected.
        //
        // Concerns:
        //: 1 The iterator's type traits are appropriately defined.
        //:
        //: 2 The iterator can be used to iterate from 'begin' to 'end' using
        //:   'operator++'.
        //:
        //: 3 The iterator can be used to iterate from 'end' to 'begin' using
        //:   'operator--'.
        //:
        //: 4 The methods 'operator*' and 'operator->' work as expected.
        //:
        //: 5 The equality operators work as expected.
        //:
        //: 6 The constructors and assignment operator work as expected.
        //:
        //: 7 The methods 'beginBusinessDays(date)' and 'endBusinessDays(date)'
        //:   work as expected.
        //:
        //: 8 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly verify the type traits of the iterator.  (C-1)
        //:
        //: 2 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'begin' to the 'end' of each calendar and verify the value
        //:   referenced by the iterator with the 'isBusinessDay' accessor from
        //:   'bdlt::PackedCalendar'.  (C-2)
        //:
        //: 3 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'end' to the 'begin' of each calendar and verify the value
        //:   referenced by the iterator with the 'isBusinessDay' accessor from
        //:   'bdlt::PackedCalendar'.  (C-3..4)
        //:
        //: 4 For a set of 'bdlt::PackedCalendar' values, test the equality
        //:   operators with all possible pairs of iterator values from each
        //:   calendar value.  Also test the constructors and assignment
        //:   operator.  (C-5..6)
        //:
        //: 5 For a set of 'bdlt::PackedCalendar' values, for all dates within
        //:   the range of the calendar verify the return values of
        //:   'beginBusinessDays(date)' and 'endBusinessDays(date)'.  (C-7)
        //:
        //: 6 Verify defensive checks are triggered for invalid values.  (C-8)
        //
        // Testing:
        //   BusinessDayConstIterator
        //   BusinessDayConstIterator beginBusinessDays() const;
        //   BusinessDayConstIterator beginBusinessDays(const Date&) const;
        //   BusinessDayConstIterator endBusinessDays() const;
        //   BusinessDayConstIterator endBusinessDays(const Date&) const;
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'BusinessDayConstIterator'" << endl
                          << "==================================" << endl;

        typedef bdlt::PackedCalendar::BusinessDayConstIterator Iterator;

        const char **SPECS = DEFAULT_SPECS;

        // Verify the type trait values.
        {
            ASSERT((bsl::is_same<Iterator::value_type, bdlt::Date>::value));

            ASSERT((bsl::is_same<Iterator::difference_type, int>::value));

            ASSERT((bsl::is_same<Iterator::pointer,
                                      bdlt::PackedCalendar_DateProxy>::value));

            ASSERT((bsl::is_same<Iterator::reference,
                                 bdlt::PackedCalendar_DateRef>::value));
        }

        // Verify iterator functionality.

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];

            Obj mX;  const Obj& X = gg(&mX, SPEC);

            // Bootstrap testing.

            {
                Iterator mA = X.beginBusinessDays();  const Iterator& A = mA;

                Iterator mB = X.endBusinessDays();    const Iterator& B = mB;

                ASSERTV(ti, X, (0 == X.numBusinessDays()) == (A == B));
            }

            // Test iterating in the forward direction, 'operator*' and
            // 'operator->'.

            {
                Iterator mA = X.beginBusinessDays();  const Iterator& A = mA;

                Iterator mB = X.endBusinessDays();    const Iterator& B = mB;

                Iterator mC(mA);                      const Iterator& C = mC;

                int offset = 0;
                int count  = 0;
                while (A != B) {
                    ASSERTV(ti, X, offset, X.isBusinessDay(*A));
                    while (X.firstDate() + offset < *A) {
                        ASSERTV(ti,
                                X,
                                offset,
                                !X.isBusinessDay(X.firstDate() + offset));
                        ++offset;
                    }

                    int year;
                    int month;
                    int day;
                    A->getYearMonthDay(&year, &month, &day);
                    ASSERTV(ti,
                            X,
                            offset,
                            X.firstDate() + offset == bdlt::Date(year,
                                                                 month,
                                                                 day));

                    ++offset;
                    ++count;
                    ++mA;

                    mC++;
                    ASSERT(C == A);
                }

                while (offset < X.length()) {
                    ASSERTV(ti,
                            X,
                            offset,
                            !X.isBusinessDay(X.firstDate() + offset));
                    ++offset;
                }

                ASSERTV(ti, X, X.numBusinessDays() == count);
            }

            // Test iterating in the reverse direction, 'operator*' and
            // 'operator->'.

            if (X.numBusinessDays()) {
                Iterator mA = X.beginBusinessDays();  const Iterator& A = mA;

                Iterator mB = X.endBusinessDays();    const Iterator& B = mB;

                Iterator mC(mB);                      const Iterator& C = mC;

                int offset = 0;
                int count  = 0;
                while (B != A) {
                    --mB;

                    ASSERTV(ti, X, offset, X.isBusinessDay(*B));
                    while (X.lastDate() - offset > *B) {
                        ASSERTV(ti,
                                X,
                                offset,
                                !X.isBusinessDay(X.lastDate() - offset));
                        ++offset;
                    }

                    int year;
                    int month;
                    int day;
                    B->getYearMonthDay(&year, &month, &day);
                    ASSERTV(ti,
                            X,
                            offset,
                            X.lastDate() - offset == bdlt::Date(year,
                                                                month,
                                                                day));

                    ++offset;
                    ++count;

                    mC--;
                    ASSERT(C == B);
                }

                while (offset < X.length()) {
                    ASSERTV(ti,
                            X,
                            offset,
                            !X.isBusinessDay(X.lastDate() - offset));
                    ++offset;
                }

                ASSERTV(ti, X, X.numBusinessDays() == count);
            }

            // Test assignment and equality operators.

            Iterator mA = X.beginBusinessDays();  const Iterator& A = mA;
            for (int tj = 0; tj < X.numBusinessDays(); ++tj, ++mA) {
                Iterator mB = X.beginBusinessDays();  const Iterator& B = mB;
                for (int tk = 0; tk < X.numBusinessDays(); ++tk, ++mB) {
                    ASSERTV(X, tj, tk, (tj == tk) == (A == B));
                    ASSERTV(X, tj, tk, (tj == tk) == (B == A));
                    ASSERTV(X, tj, tk, (tj != tk) == (A != B));
                    ASSERTV(X, tj, tk, (tj != tk) == (B != A));

                    Iterator mC(mA);  const Iterator& C = mC;
                    ASSERTV(X, tj, C == A);

                    mC = B;
                    ASSERTV(X, tj, C == B);
                }

                ASSERTV(X, tj, false == (A == X.endBusinessDays()));
                ASSERTV(X, tj, true  == (A != X.endBusinessDays()));
                ASSERTV(X, tj, false == (X.endBusinessDays() == A));
                ASSERTV(X, tj, true  == (X.endBusinessDays() != A));
            }
            ASSERTV(X, true  == (X.endBusinessDays() == X.endBusinessDays()));
            ASSERTV(X, false == (X.endBusinessDays() != X.endBusinessDays()));

            // Testing 'beginBusinessDays(date)' and 'endBusinessDays(date)'.

            for (int offset = 0; offset < X.length(); ++offset) {
                bdlt::Date date = X.firstDate() + offset;

                Iterator        mA = X.beginBusinessDays(date);
                const Iterator& A  = mA;
                Iterator        mB = X.endBusinessDays(date);
                const Iterator& B = mB;

                if (X.isBusinessDay(date)) {
                    ASSERTV(X, date, *A == date);
                    ++mA;
                    ASSERTV(X, date, A == B);
                }
                else {
                    ASSERTV(X, date, A == X.endBusinessDays() || *A > date);
                    ASSERTV(X, date, A == B);

                    if (A != X.beginBusinessDays()) {
                        --mA;
                        ASSERTV(X, date, *A < date);
                    }
                }
            }
        }

        // Negative testing.

        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = gg(&mX, "@2014/1/1 30 14");

            Iterator mA = X.beginBusinessDays();

            ASSERT_SAFE_PASS(X.beginBusinessDays());
            ASSERT_SAFE_FAIL(--X.beginBusinessDays());

            mA = X.beginBusinessDays();
            ASSERT_SAFE_FAIL(mA--);

            ASSERT_SAFE_PASS(X.endBusinessDays());
            ASSERT_SAFE_FAIL(++X.endBusinessDays());

            mA = X.endBusinessDays();
            ASSERT_SAFE_FAIL(mA++);

            ASSERT_SAFE_PASS(X.beginBusinessDays(bdlt::Date(2014, 1, 6)));
            ASSERT_SAFE_FAIL(X.beginBusinessDays(bdlt::Date(2014, 3, 6)));

            ASSERT_SAFE_PASS(X.endBusinessDays(bdlt::Date(2014, 1, 6)));
            ASSERT_SAFE_FAIL(X.endBusinessDays(bdlt::Date(2014, 3, 6)));

            Obj mY;  const Obj& Y = gg(&mY, "@2014/1/1 30 14");

            Iterator  mB = Y.beginBusinessDays();
            ASSERT_SAFE_FAIL(mA == mB);
            ASSERT_SAFE_FAIL(mA != mB);
        }
      } break;
      case 22: {
        // -------------------------------------------------------------------
        // TESTING 'HolidayCodeConstReverseIterator'
        //   Ensure that this iterator and the associated 'rbegin' and 'rend'
        //   methods behave as expected.
        //
        // Concerns:
        //: 1 The iterator's type traits are appropriately defined.
        //:
        //: 2 The iterator can be used to iterate from 'rbegin' to 'rend' using
        //:   'operator++'.
        //:
        //: 3 The iterator can be used to iterate from 'rend' to 'rbegin' using
        //:   'operator--'.
        //:
        //: 4 The method 'operator*' works as expected.
        //:
        //: 5 The equality operators work as expected.
        //:
        //: 6 The constructors and assignment operator work as expected.
        //:
        //: 7 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly verify the type traits of the iterator.  (C-1)
        //:
        //: 2 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'rbegin' to the 'rend' of each calendar and verify the value
        //:   referenced by the iterator with the 'holidayCode' accessor from
        //:   'bdlt::PackedCalendar'.  (C-2)
        //:
        //: 3 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'rend' to the 'rbegin' of each calendar and verify the value
        //:   referenced by the iterator with the 'holidayCode' accessor from
        //:   'bdlt::PackedCalendar'.  (C-3..4)
        //:
        //: 4 For a set of 'bdlt::PackedCalendar' values, test the equality
        //:   operators with all possible pairs of iterator values from each
        //:   calendar value.  Also test the constructors and assignment
        //:   operator.  (C-5..6)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-7)
        //
        // Testing:
        //   HolidayCodeConstReverseIterator
        //   HCCRI rbeginHolidayCodes(const Date&) const;
        //   HCCRI rbeginHolidayCodes(const HCI&) const;
        //   HCCRI rendHolidayCodes(const Date&) const;
        //   HCCRI rendHolidayCodes(const HCI&) const;
        // -------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'HolidayCodeConstReverseIterator'" << endl
                 << "=========================================" << endl;
        }

        typedef bdlt::PackedCalendar::HolidayCodeConstReverseIterator Iterator;

        const char **SPECS = DEFAULT_SPECS;

        // Verify the type trait values.

        {
            ASSERT((bsl::is_same<Iterator::value_type, int>::value));

            ASSERT((bsl::is_same<Iterator::difference_type, int>::value));

            ASSERT((bsl::is_same<Iterator::reference, int>::value));
        }

        // Verify iterator functionality.

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];

            Obj mX;  const Obj& X = gg(&mX, SPEC);

            for (int offset = 0; offset < X.length(); ++offset) {
                bdlt::Date date = X.firstDate() + offset;

                if (X.isHoliday(date)) {
                    // Bootstrap testing.

                    {
                        Iterator        mA = X.rbeginHolidayCodes(date);
                        const Iterator& A = mA;
                        Iterator        mB = X.rendHolidayCodes(date);
                        const Iterator& B = mB;

                        ASSERTV(ti,
                                X,
                                date,
                                (0 == X.numHolidayCodes(date)) == (A == B));

                        bdlt::PackedCalendar::HolidayConstIterator holidayIter
                                                       = X.beginHolidays(date);

                        ASSERTV(ti,
                                X,
                                date,
                                A == X.rbeginHolidayCodes(holidayIter));

                        ASSERTV(ti,
                                X,
                                date,
                                B == X.rendHolidayCodes(holidayIter));

                        Obj mY;  const Obj& Y = gg(&mY, SPEC);

                        Iterator        mC = Y.rbeginHolidayCodes(date);
                        const Iterator& C = mC;
                        Iterator        mD = Y.rendHolidayCodes(date);
                        const Iterator& D = mD;

                        ASSERTV(ti, X, A != C);
                        ASSERTV(ti, X, B != D);
                    }

                    // Test iterating in the forward direction, and
                    // 'operator*'.

                    {
                        Iterator        mA = X.rbeginHolidayCodes(date);
                        const Iterator& A = mA;
                        Iterator        mB = X.rendHolidayCodes(date);
                        const Iterator& B = mB;
                        Iterator        mC(mA);
                        const Iterator& C = mC;

                        int index = X.numHolidayCodes(date) - 1;
                        while (A != B) {
                            ASSERTV(ti,
                                    X,
                                    date,
                                    index,
                                    X.holidayCode(date, index) == *A);

                            --index;
                            ++mA;

                            mC++;
                            ASSERT(C == A);
                        }
                        ASSERTV(ti, X, -1 == index);
                    }

                    // Test iterating in the reverse direction, and
                    // 'operator*'.

                    if (X.numHolidayCodes(date)) {
                        Iterator        mA = X.rbeginHolidayCodes(date);
                        const Iterator& A = mA;
                        Iterator        mB = X.rendHolidayCodes(date);
                        const Iterator& B = mB;
                        Iterator        mC(mB);
                        const Iterator& C = mC;

                        int index = -1;
                        while (B != A) {
                            --mB;
                            ++index;

                            ASSERTV(ti,
                                    X,
                                    date,
                                    index,
                                    X.holidayCode(date, index) == *B);

                            mC--;
                            ASSERT(C == B);
                        }
                        ASSERTV(ti, X, X.numHolidayCodes(date) - 1 == index);
                    }

                    // Test assignment and equality operators.

                    Iterator        mA = X.rbeginHolidayCodes(date);
                    const Iterator& A  = mA;
                    for (int tj = 0;
                         tj < X.numHolidayCodes(date);
                         ++tj, ++mA) {
                        Iterator        mB = X.rbeginHolidayCodes(date);
                        const Iterator& B  = mB;
                        for (int tk = 0;
                             tk < X.numHolidayCodes(date);
                             ++tk, ++mB) {
                            ASSERTV(X, tj, tk, (tj == tk) == (A == B));
                            ASSERTV(X, tj, tk, (tj == tk) == (B == A));
                            ASSERTV(X, tj, tk, (tj != tk) == (A != B));
                            ASSERTV(X, tj, tk, (tj != tk) == (B != A));

                            Iterator mC(mA);  const Iterator& C = mC;
                            ASSERTV(X, tj, C == A);

                            mC = B;
                            ASSERTV(X, tj, C == B);
                        }

                        ASSERTV(X,
                                tj,
                                false == (A == X.rendHolidayCodes(date)));
                        ASSERTV(X,
                                tj,
                                true  == (A != X.rendHolidayCodes(date)));
                        ASSERTV(X,
                                tj,
                                false == (X.rendHolidayCodes(date) == A));
                        ASSERTV(X,
                                tj,
                                true  == (X.rendHolidayCodes(date) != A));
                    }
                    ASSERTV(X, true  == (X.rendHolidayCodes(date)
                                                 == X.rendHolidayCodes(date)));
                    ASSERTV(X, false == (X.rendHolidayCodes(date)
                                                 != X.rendHolidayCodes(date)));
                }
            }
        }

        // Negative testing.

        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = gg(&mX, "@2014/1/1 30 14");

            Obj mY;  const Obj& Y = gg(&mY, "@2012/1/1 30 14");

            ASSERT_SAFE_PASS(X.rbeginHolidayCodes(X.beginHolidays()));
            ASSERT_SAFE_FAIL(X.rbeginHolidayCodes(Y.beginHolidays()));
            ASSERT_SAFE_FAIL_RAW(--X.rbeginHolidayCodes(X.beginHolidays()));

            Iterator mA = X.rbeginHolidayCodes(X.beginHolidays());
            ASSERT_SAFE_FAIL_RAW(mA--);

            ASSERT_SAFE_PASS(X.rendHolidayCodes(X.beginHolidays()));
            ASSERT_SAFE_FAIL(X.rendHolidayCodes(Y.beginHolidays()));
            ASSERT_SAFE_FAIL_RAW(++X.rendHolidayCodes(X.beginHolidays()));

            mA = X.rendHolidayCodes(X.beginHolidays());
            ASSERT_SAFE_FAIL_RAW(mA++);

            ASSERT_SAFE_PASS(X.rbeginHolidayCodes(bdlt::Date(2014, 1, 15)));
            ASSERT_SAFE_FAIL(X.rbeginHolidayCodes(bdlt::Date(2014, 1, 16)));

            ASSERT_SAFE_PASS(X.rendHolidayCodes(bdlt::Date(2014, 1, 15)));
            ASSERT_SAFE_FAIL(X.rendHolidayCodes(bdlt::Date(2014, 1, 16)));
        }
      } break;
      case 21: {
        // -------------------------------------------------------------------
        // TESTING 'HolidayCodeConstIterator'
        //   Ensure that this iterator and the associated 'begin' and 'end'
        //   methods behave as expected.
        //
        // Concerns:
        //: 1 The iterator's type traits are appropriately defined.
        //:
        //: 2 The iterator can be used to iterate from 'begin' to 'end' using
        //:   'operator++'.
        //:
        //: 3 The iterator can be used to iterate from 'end' to 'begin' using
        //:   'operator--'.
        //:
        //: 4 The method 'operator*' works as expected.
        //:
        //: 5 The equality operators work as expected.
        //:
        //: 6 The constructors and assignment operator work as expected.
        //:
        //: 7 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly verify the type traits of the iterator.  (C-1)
        //:
        //: 2 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'begin' to the 'end' of each calendar holiday and verify the
        //:   value referenced by the iterator with the 'holidayCode' accessor
        //:   from 'bdlt::PackedCalendar'.  (C-2)
        //:
        //: 3 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'end' to the 'begin' of each calendar holiday and verify the
        //:   value referenced by the iterator with the 'holidayCode' accessor
        //:   from 'bdlt::PackedCalendar'.  (C-3..4)
        //:
        //: 4 For a set of 'bdlt::PackedCalendar' values, test the equality
        //:   operators with all possible pairs of iterator values from each
        //:   calendar value.  Also test the constructors and assignment
        //:   operator.  (C-5..6)
        //:
        //: 5 Verify defensive checks are triggered for invalid values.  (C-7)
        //
        // Testing:
        //   HolidayCodeConstIterator
        //   HolidayCodeConstIterator beginHolidayCodes(const Date&) const;
        //   HolidayCodeConstIterator beginHolidayCodes(const HCI&) const;
        //   HolidayCodeConstIterator endHolidayCodes(const HCI&) const;
        //   HolidayCodeConstIterator endHolidayCodes(const Date& date) const;
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'HolidayCodeConstIterator'" << endl
                          << "==================================" << endl;

        typedef bdlt::PackedCalendar::HolidayCodeConstIterator Iterator;

        const char **SPECS = DEFAULT_SPECS;

        // Verify the type trait values.

        {
            ASSERT((bsl::is_same<Iterator::value_type, int>::value));

            ASSERT((bsl::is_same<Iterator::difference_type, int>::value));

            ASSERT((bsl::is_same<Iterator::reference, int>::value));
        }

        // Verify iterator functionality.

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];

            Obj mX;  const Obj& X = gg(&mX, SPEC);

            for (int offset = 0; offset < X.length(); ++offset) {
                bdlt::Date date = X.firstDate() + offset;

                if (X.isHoliday(date)) {
                    // Bootstrap testing.

                    {
                        Iterator        mA = X.beginHolidayCodes(date);
                        const Iterator& A = mA;
                        Iterator        mB = X.endHolidayCodes(date);
                        const Iterator& B = mB;

                        ASSERTV(ti,
                                X,
                                date,
                                (0 == X.numHolidayCodes(date)) == (A == B));

                        bdlt::PackedCalendar::HolidayConstIterator holidayIter
                                                       = X.beginHolidays(date);

                        ASSERTV(ti,
                                X,
                                date,
                                A == X.beginHolidayCodes(holidayIter));

                        ASSERTV(ti,
                                X,
                                date,
                                B == X.endHolidayCodes(holidayIter));

                        Obj mY;  const Obj& Y = gg(&mY, SPEC);

                        Iterator        mC = Y.beginHolidayCodes(date);
                        const Iterator& C = mC;
                        Iterator        mD = Y.endHolidayCodes(date);
                        const Iterator& D = mD;

                        ASSERTV(ti, X, A != C);
                        ASSERTV(ti, X, B != D);
                    }

                    // Test iterating in the forward direction, 'operator*',
                    // and 'operator-'.

                    {
                        Iterator        mA = X.beginHolidayCodes(date);
                        const Iterator& A = mA;
                        Iterator        mB = X.endHolidayCodes(date);
                        const Iterator& B = mB;
                        Iterator        mC(mA);
                        const Iterator& C = mC;

                        int index = 0;
                        while (A != B) {
                            ASSERTV(ti,
                                    X,
                                    date,
                                    index,
                                    X.holidayCode(date, index) == *A);

                            ASSERTV(ti,
                                    X,
                                    date,
                                    index,
                                    index == A - X.beginHolidayCodes(date));

                            ++index;
                            ++mA;

                            mC++;
                            ASSERT(C == A);
                        }
                        ASSERTV(ti, X, X.numHolidayCodes(date) == index);
                    }

                    // Test iterating in the reverse direction, and
                    // 'operator*'.

                    if (X.numHolidayCodes(date)) {
                        Iterator        mA = X.beginHolidayCodes(date);
                        const Iterator& A = mA;
                        Iterator        mB = X.endHolidayCodes(date);
                        const Iterator& B = mB;
                        Iterator        mC(mB);
                        const Iterator& C = mC;

                        int index = X.numHolidayCodes(date);
                        while (B != A) {
                            --mB;
                            --index;

                            ASSERTV(ti,
                                    X,
                                    date,
                                    index,
                                    X.holidayCode(date, index) == *B);

                            mC--;
                            ASSERT(C == B);
                        }
                        ASSERTV(ti, X, 0 == index);
                    }

                    // Test assignment and equality operators.

                    Iterator        mA = X.beginHolidayCodes(date);
                    const Iterator& A  = mA;
                    for (int tj = 0;
                         tj < X.numHolidayCodes(date);
                         ++tj, ++mA) {
                        Iterator        mB = X.beginHolidayCodes(date);
                        const Iterator& B  = mB;
                        for (int tk = 0;
                             tk < X.numHolidayCodes(date);
                             ++tk, ++mB) {
                            ASSERTV(X, tj, tk, (tj == tk) == (A == B));
                            ASSERTV(X, tj, tk, (tj == tk) == (B == A));
                            ASSERTV(X, tj, tk, (tj != tk) == (A != B));
                            ASSERTV(X, tj, tk, (tj != tk) == (B != A));

                            Iterator mC(mA);  const Iterator& C = mC;
                            ASSERTV(X, tj, C == A);

                            mC = B;
                            ASSERTV(X, tj, C == B);
                        }

                        ASSERTV(X,
                                tj,
                                false == (A == X.endHolidayCodes(date)));
                        ASSERTV(X,
                                tj,
                                true  == (A != X.endHolidayCodes(date)));
                        ASSERTV(X,
                                tj,
                                false == (X.endHolidayCodes(date) == A));
                        ASSERTV(X,
                                tj,
                                true  == (X.endHolidayCodes(date) != A));
                    }
                    ASSERTV(X, true  == (X.endHolidayCodes(date)
                                                  == X.endHolidayCodes(date)));
                    ASSERTV(X, false == (X.endHolidayCodes(date)
                                                  != X.endHolidayCodes(date)));
                }
            }
        }

        // Negative testing.

        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = gg(&mX, "@2014/1/1 30 14");

            Obj mY;  const Obj& Y = gg(&mY, "@2012/1/1 30 14");

            ASSERT_SAFE_PASS(X.beginHolidayCodes(X.beginHolidays()));
            ASSERT_SAFE_FAIL(X.beginHolidayCodes(Y.beginHolidays()));
            ASSERT_SAFE_FAIL_RAW(--X.beginHolidayCodes(X.beginHolidays()));

            Iterator mA = X.beginHolidayCodes(X.beginHolidays());
            ASSERT_SAFE_FAIL_RAW(mA--);

            ASSERT_SAFE_PASS(X.endHolidayCodes(X.beginHolidays()));
            ASSERT_SAFE_FAIL(X.endHolidayCodes(Y.beginHolidays()));
            ASSERT_SAFE_FAIL_RAW(++X.endHolidayCodes(X.beginHolidays()));

            mA = X.endHolidayCodes(X.beginHolidays());
            ASSERT_SAFE_FAIL_RAW(mA++);

            ASSERT_PASS(X.rbeginHolidayCodes(bdlt::Date(2014, 1, 15)));
            ASSERT_FAIL(X.rbeginHolidayCodes(bdlt::Date(2014, 1, 16)));

            ASSERT_PASS(X.rendHolidayCodes(bdlt::Date(2014, 1, 15)));
            ASSERT_FAIL(X.rendHolidayCodes(bdlt::Date(2014, 1, 16)));
        }
      } break;
      case 20: {
        // -------------------------------------------------------------------
        // TESTING 'HolidayConstReverseIterator'
        //   Ensure that this iterator and the associated 'rbegin' and 'rend'
        //   methods behave as expected.
        //
        // Concerns:
        //: 1 The iterator's type traits are appropriately defined.
        //:
        //: 2 The iterator can be used to iterate from 'rbegin' to 'rend' using
        //:   'operator++'.
        //:
        //: 3 The iterator can be used to iterate from 'rend' to 'rbegin' using
        //:   'operator--'.
        //:
        //: 4 The methods 'operator*' and 'operator->' work as expected.
        //:
        //: 5 The equality operators work as expected.
        //:
        //: 6 The constructors and assignment operator work as expected.
        //:
        //: 7 The methods 'beginHolidays(date)' and 'endHolidays(date)' work as
        //:   expected.
        //:
        //: 8 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly verify the type traits of the iterator.  (C-1)
        //:
        //: 2 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'rbegin' to the 'rend' of each calendar and verify the value
        //:   referenced by the iterator with the 'holiday' accessor from
        //:   'bdlt::PackedCalendar'.  (C-2)
        //:
        //: 3 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'rend' to the 'rbegin' of each calendar and verify the value
        //:   referenced by the iterator with the 'holiday' accessor from
        //:   'bdlt::PackedCalendar'.  (C-3..4)
        //:
        //: 4 For a set of 'bdlt::PackedCalendar' values, test the equality
        //:   operators with all possible pairs of iterator values from each
        //:   calendar value.  Also test the constructors and assignment
        //:   operator.  (C-5..6)
        //:
        //: 5 For a set of 'bdlt::PackedCalendar' values, for all dates within
        //:   the range of the calendar verify the return values of
        //:   'beginHolidays(date)' and 'endHolidays(date)'.  (C-7)
        //:
        //: 6 Verify defensive checks are triggered for invalid values.  (C-8)
        //
        // Testing:
        //   HolidayConstReverseIterator
        //   HolidayConstReverseIterator rbeginHolidays() const;
        //   HolidayConstReverseIterator rbeginHolidays(const Date&) const;
        //   HolidayConstReverseIterator rendHolidays() const;
        //   HolidayConstReverseIterator rendHolidays(const Date& date) const;
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'HolidayConstReverseIterator'" << endl
                          << "=====================================" << endl;

        typedef bdlt::PackedCalendar::HolidayConstReverseIterator Iterator;

        const char **SPECS = DEFAULT_SPECS;

        // Verify the type trait values.

        {
            ASSERT((bsl::is_same<Iterator::value_type, bdlt::Date>::value));

            ASSERT((bsl::is_same<Iterator::difference_type, int>::value));

            ASSERT((bsl::is_same<Iterator::pointer,
                                      bdlt::PackedCalendar_DateProxy>::value));

            ASSERT((bsl::is_same<Iterator::reference,
                                 bdlt::PackedCalendar_DateRef>::value));
        }

        // Verify iterator functionality.

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];

            Obj mX;  const Obj& X = gg(&mX, SPEC);

            // Bootstrap testing.

            {
                Iterator mA = X.rbeginHolidays();  const Iterator& A = mA;

                Iterator mB = X.rendHolidays();    const Iterator& B = mB;

                ASSERTV(ti, X, (0 == X.numHolidays()) == (A == B));

                Obj mY;  const Obj& Y = gg(&mY, SPEC);

                Iterator mC = Y.rbeginHolidays();  const Iterator& C = mC;

                Iterator mD = Y.rendHolidays();    const Iterator& D = mD;

                ASSERTV(ti, X, A != C);
                ASSERTV(ti, X, B != D);
            }

            // Test iterating in the forward direction, 'operator*' and
            // 'operator->'.

            {
                Iterator mA = X.rbeginHolidays();  const Iterator& A = mA;

                Iterator mB = X.rendHolidays();    const Iterator& B = mB;

                Iterator mC(mA);                   const Iterator& C = mC;

                int index = X.numHolidays() - 1;
                while (A != B) {
                    ASSERTV(ti, X, index, X.holiday(index) == *A);

                    int year;
                    int month;
                    int day;
                    A->getYearMonthDay(&year, &month, &day);
                    ASSERTV(ti, X, index, X.holiday(index) == bdlt::Date(year,
                                                                         month,
                                                                         day));

                    --index;
                    ++mA;

                    mC++;
                    ASSERT(C == A);
                }
                ASSERTV(ti, X, -1 == index);
            }

            // Test iterating in the reverse direction, 'operator*' and
            // 'operator->'.

            if (X.numHolidays()) {
                Iterator mA = X.rbeginHolidays();  const Iterator& A = mA;

                Iterator mB = X.rendHolidays();    const Iterator& B = mB;

                Iterator mC(mB);                   const Iterator& C = mC;

                int index = -1;
                while (B != A) {
                    --mB;
                    ++index;

                    ASSERTV(ti, X, index, X.holiday(index) == *B);

                    int year;
                    int month;
                    int day;
                    B->getYearMonthDay(&year, &month, &day);
                    ASSERTV(ti,
                            X,
                            index,
                            X.holiday(index) == bdlt::Date(year,
                                                           month,
                                                           day));

                    mC--;
                    ASSERT(C == B);
                }
                ASSERTV(ti, X, X.numHolidays() - 1 == index);
            }

            // Test assignment and equality operators.

            Iterator mA = X.rbeginHolidays();  const Iterator& A = mA;
            for (int tj = 0; tj < X.numHolidays(); ++tj, ++mA) {
                Iterator mB = X.rbeginHolidays();  const Iterator& B = mB;
                for (int tk = 0; tk < X.numHolidays(); ++tk, ++mB) {
                    ASSERTV(X, tj, tk, (tj == tk) == (A == B));
                    ASSERTV(X, tj, tk, (tj == tk) == (B == A));
                    ASSERTV(X, tj, tk, (tj != tk) == (A != B));
                    ASSERTV(X, tj, tk, (tj != tk) == (B != A));

                    Iterator mC(mA);  const Iterator& C = mC;
                    ASSERTV(X, tj, C == A);

                    mC = B;
                    ASSERTV(X, tj, C == B);
                }

                ASSERTV(X, tj, false == (A == X.rendHolidays()));
                ASSERTV(X, tj, true  == (A != X.rendHolidays()));
                ASSERTV(X, tj, false == (X.rendHolidays() == A));
                ASSERTV(X, tj, true  == (X.rendHolidays() != A));
            }
            ASSERTV(X, true  == (X.rendHolidays() == X.rendHolidays()));
            ASSERTV(X, false == (X.rendHolidays() != X.rendHolidays()));

            // Testing 'rbeginHolidays(date)' and 'rendHolidays(date)'.

            for (int offset = 0; offset < X.length(); ++offset) {
                bdlt::Date date = X.firstDate() + offset;

                Iterator mA = X.rbeginHolidays(date);  const Iterator& A = mA;

                Iterator mB = X.rendHolidays(date);    const Iterator& B = mB;

                if (X.isHoliday(date)) {
                    ASSERTV(X, date, *A == date);
                    ++mA;
                    ASSERTV(X, date, A == B);
                }
                else {
                    ASSERTV(X, date, A == X.rendHolidays() || *A < date);
                    ASSERTV(X, date, A == B);

                    if (A != X.rbeginHolidays()) {
                        --mA;
                        ASSERTV(X, date, *A > date);
                    }
                }
            }
        }

        // Negative testing.

        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = gg(&mX, "@2014/1/1 30 14");

            ASSERT_SAFE_PASS(X.rbeginHolidays());
            ASSERT_SAFE_FAIL_RAW(--X.rbeginHolidays());

            Iterator mA = X.rbeginHolidays();
            ASSERT_SAFE_FAIL_RAW(mA--);

            ASSERT_SAFE_PASS(X.rendHolidays());
            ASSERT_SAFE_FAIL_RAW(++X.rendHolidays());

            mA = X.rendHolidays();
            ASSERT_SAFE_FAIL_RAW(mA++);

            ASSERT_SAFE_PASS(X.rbeginHolidays(bdlt::Date(2014, 1, 6)));
            ASSERT_SAFE_FAIL(X.rbeginHolidays(bdlt::Date(2014, 3, 6)));

            ASSERT_SAFE_PASS(X.rendHolidays(bdlt::Date(2014, 1, 6)));
            ASSERT_SAFE_FAIL(X.rendHolidays(bdlt::Date(2014, 3, 6)));
        }
      } break;
      case 19: {
        // -------------------------------------------------------------------
        // TESTING 'HolidayConstIterator'
        //   Ensure that this iterator and the associated 'begin' and 'end'
        //   methods behave as expected.
        //
        // Concerns:
        //: 1 The iterator's type traits are appropriately defined.
        //:
        //: 2 The iterator can be used to iterate from 'begin' to 'end' using
        //:   'operator++'.
        //:
        //: 3 The iterator can be used to iterate from 'end' to 'begin' using
        //:   'operator--'.
        //:
        //: 4 The methods 'operator*' and 'operator->' work as expected.
        //:
        //: 5 The equality operators work as expected.
        //:
        //: 6 The constructors and assignment operator work as expected.
        //:
        //: 7 The methods 'beginHolidays(date)' and 'endHolidays(date)' work as
        //:   expected.
        //:
        //: 8 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly verify the type traits of the iterator.  (C-1)
        //:
        //: 2 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'begin' to the 'end' of each calendar and verify the value
        //:   referenced by the iterator with the 'holiday' accessor from
        //:   'bdlt::PackedCalendar'.  (C-2)
        //:
        //: 3 For a set of 'bdlt::PackedCalendar' values, iterate from the
        //:   'end' to the 'begin' of each calendar and verify the value
        //:   referenced by the iterator with the 'holiday' accessor from
        //:   'bdlt::PackedCalendar'.  (C-3..4)
        //:
        //: 4 For a set of 'bdlt::PackedCalendar' values, test the equality
        //:   operators with all possible pairs of iterator values from each
        //:   calendar value.  Also test the constructors and assignment
        //:   operator.  (C-5..6)
        //:
        //: 5 For a set of 'bdlt::PackedCalendar' values, for all dates within
        //:   the range of the calendar verify the return values of
        //:   'beginHolidays(date)' and 'endHolidays(date)'.  (C-7)
        //:
        //: 6 Verify defensive checks are triggered for invalid values.  (C-8)
        //
        // Testing:
        //   HolidayConstIterator
        //   HolidayConstIterator beginHolidays() const;
        //   HolidayConstIterator beginHolidays(const Date& date) const;
        //   HolidayConstIterator endHolidays() const;
        //   HolidayConstIterator endHolidays(const Date& date) const;
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'HolidayConstIterator'" << endl
                          << "==============================" << endl;

        typedef bdlt::PackedCalendar::HolidayConstIterator Iterator;

        const char **SPECS = DEFAULT_SPECS;

        // Verify the type trait values.
        {
            ASSERT((bsl::is_same<Iterator::value_type, bdlt::Date>::value));

            ASSERT((bsl::is_same<Iterator::difference_type, int>::value));

            ASSERT((bsl::is_same<Iterator::pointer,
                                      bdlt::PackedCalendar_DateProxy>::value));

            ASSERT((bsl::is_same<Iterator::reference,
                                 bdlt::PackedCalendar_DateRef>::value));
        }

        // Verify iterator functionality.

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];

            Obj mX;  const Obj& X = gg(&mX, SPEC);

            // Bootstrap testing.

            {
                Iterator mA = X.beginHolidays();  const Iterator& A = mA;

                Iterator mB = X.endHolidays();    const Iterator& B = mB;

                ASSERTV(ti, X, (0 == X.numHolidays()) == (A == B));

                Obj mY;  const Obj& Y = gg(&mY, SPEC);

                Iterator mC = Y.beginHolidays();  const Iterator& C = mC;

                Iterator mD = Y.endHolidays();    const Iterator& D = mD;

                ASSERTV(ti, X, A != C);
                ASSERTV(ti, X, B != D);
            }

            // Test iterating in the forward direction, 'operator*' and
            // 'operator->'.

            {
                Iterator mA = X.beginHolidays();  const Iterator& A = mA;

                Iterator mB = X.endHolidays();    const Iterator& B = mB;

                Iterator mC(mA);                  const Iterator& C = mC;

                int index = 0;
                while (A != B) {
                    ASSERTV(ti, X, index, X.holiday(index) == *A);

                    int year;
                    int month;
                    int day;
                    A->getYearMonthDay(&year, &month, &day);
                    ASSERTV(ti, X, index, X.holiday(index) == bdlt::Date(year,
                                                                         month,
                                                                         day));

                    ++index;
                    ++mA;

                    mC++;
                    ASSERT(C == A);
                }
                ASSERTV(ti, X, X.numHolidays() == index);
            }

            // Test iterating in the reverse direction, 'operator*' and
            // 'operator->'.

            if (X.numHolidays()) {
                Iterator mA = X.beginHolidays();  const Iterator& A = mA;

                Iterator mB = X.endHolidays();    const Iterator& B = mB;

                Iterator mC(mB);                  const Iterator& C = mC;

                int index = X.numHolidays();
                while (B != A) {
                    --mB;
                    --index;

                    ASSERTV(ti, X, index, X.holiday(index) == *B);

                    int year;
                    int month;
                    int day;
                    B->getYearMonthDay(&year, &month, &day);
                    ASSERTV(ti,
                            X,
                            index,
                            X.holiday(index) == bdlt::Date(year,
                                                           month,
                                                           day));

                    mC--;
                    ASSERT(C == B);
                }
                ASSERTV(ti, X, 0 == index);
            }

            // Test assignment and equality operators.

            Iterator mA = X.beginHolidays();  const Iterator& A = mA;
            for (int tj = 0; tj < X.numHolidays(); ++tj, ++mA) {
                Iterator mB = X.beginHolidays();  const Iterator& B = mB;
                for (int tk = 0; tk < X.numHolidays(); ++tk, ++mB) {
                    ASSERTV(X, tj, tk, (tj == tk) == (A == B));
                    ASSERTV(X, tj, tk, (tj == tk) == (B == A));
                    ASSERTV(X, tj, tk, (tj != tk) == (A != B));
                    ASSERTV(X, tj, tk, (tj != tk) == (B != A));

                    Iterator mC(mA);  const Iterator& C = mC;
                    ASSERTV(X, tj, C == A);

                    mC = B;
                    ASSERTV(X, tj, C == B);
                }

                ASSERTV(X, tj, false == (A == X.endHolidays()));
                ASSERTV(X, tj, true  == (A != X.endHolidays()));
                ASSERTV(X, tj, false == (X.endHolidays() == A));
                ASSERTV(X, tj, true  == (X.endHolidays() != A));
            }
            ASSERTV(X, true  == (X.endHolidays() == X.endHolidays()));
            ASSERTV(X, false == (X.endHolidays() != X.endHolidays()));

            // Testing 'beginHolidays(date)' and 'endHolidays(date)'.

            for (int offset = 0; offset < X.length(); ++offset) {
                bdlt::Date date = X.firstDate() + offset;

                Iterator mA = X.beginHolidays(date);  const Iterator& A = mA;

                Iterator mB = X.endHolidays(date);    const Iterator& B = mB;

                if (X.isHoliday(date)) {
                    ASSERTV(X, date, *A == date);
                    ++mA;
                    ASSERTV(X, date, A == B);
                }
                else {
                    ASSERTV(X, date, A == X.endHolidays() || *A > date);
                    ASSERTV(X, date, A == B);

                    if (A != X.beginHolidays()) {
                        --mA;
                        ASSERTV(X, date, *A < date);
                    }
                }
            }
        }

        // Negative testing.

        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = gg(&mX, "@2014/1/1 30 14");

            Iterator  mA = X.beginHolidays();

            ASSERT_SAFE_PASS(X.beginHolidays());
            ASSERT_SAFE_FAIL_RAW(--X.beginHolidays());

            mA = X.beginHolidays();
            ASSERT_SAFE_FAIL_RAW(mA--);

            ASSERT_SAFE_PASS(X.endHolidays());
            ASSERT_SAFE_FAIL_RAW(++X.endHolidays());

            mA = X.endHolidays();
            ASSERT_SAFE_FAIL_RAW(mA++);

            ASSERT_SAFE_PASS(X.beginHolidays(bdlt::Date(2014, 1, 6)));
            ASSERT_SAFE_FAIL(X.beginHolidays(bdlt::Date(2014, 3, 6)));

            ASSERT_SAFE_PASS(X.endHolidays(bdlt::Date(2014, 1, 6)));
            ASSERT_SAFE_FAIL(X.endHolidays(bdlt::Date(2014, 3, 6)));
        }
      } break;
      case 18: {
        // -------------------------------------------------------------------
        // TESTING UNION AND INTERSECTION METHODS
        //   Ensure that these manipulators produce the expected results.
        //
        // Concerns:
        //: 1 That 'intersectBusinessDays', 'intersectNonBusinessDays',
        //:   'unionBusinessDays', and 'unionNonBusinessDays' correctly modify
        //:   the value of the object.
        //:
        //: 2 That the values of the object and the method parameter can be
        //:   reversed ('X.foo(Y)' to 'Y.foo(X)') without changing the
        //:   resultant value.
        //:
        //: 3 Any memory allocation is from the specified allocator.
        //:
        //: 4 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 The table-driven technique is used to test each manipulator
        //:   independently.  For each row in a table of test vectors, three
        //:   specifications are provided; one for the object, 'X', that will
        //:   be used to invoke the method, one for the object, 'Y', that will
        //:   be the parameter to the method, and one for the expected result
        //:   object 'EXP'.  (C-1)
        //:
        //: 2 The values for 'X' and 'Y' are reversed and the method tested
        //:   again.  (C-2)
        //:
        //: 3 In all cases, confirm the allocator used and exception
        //:   neutrality by using the specially instrumented
        //:   'bslma::TestAllocator', its accessor methods, and a pair of
        //:   standard macros, 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END', which configure the
        //:   'bslma::TestAllocator' object appropriately in a loop.  (C-3..4)
        //
        // Testing:
        //   void intersectBusinessDays(const PackedCalendar& calendar);
        //   void intersectNonBusinessDays(const PackedCalendar& calendar);
        //   void unionBusinessDays(const PackedCalendar& calendar);
        //   void unionNonBusinessDays(const PackedCalendar& calendar);
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING UNION AND INTERSECTION METHODS" << endl
                          << "======================================" << endl;

        if (verbose) cout << "\nTesting 'intersectBusinessDays'." << endl;
        {
            static const struct {
                int         d_line;     // source line number
                const char *d_spec1_p;  // specification for an argument
                const char *d_spec2_p;  // specification for an argument
                const char *d_exp_p;    // expected specification
            } DATA[] = {
                //LN              SPEC1                 SPEC2        EXP
                //--  ------------------------------  ----------  ----------
                { L_,                             "",         "",         "" },

                { L_,                 "@2012/3/1 30",         "",         "" },

                { L_,                 "@2012/3/1 30",
                                                  "@2012/3/1 15",
                                                              "@2012/3/1 15" },
                { L_,                 "@2012/3/1 30",
                                                  "@2012/4/1 15",         "" },

                { L_,                 "@2012/3/1 30",
                                       "@2012/3/1 30 1mw 3ua 7f",
                                                   "@2012/3/1 30 1mw 3ua 7f" },

                { L_,         "@2012/3/1 30 0ua 3fa",
                                       "@2012/3/1 30 1mw 3ua 7f",
                                           "@2012/3/1 30 0ua 1uamw 3ufa 7fa" },

                { L_,         "@2012/3/1 30 0ua 5fa",
                                       "@2012/3/1 30 1mw 3ua 7f",
                                       "@2012/3/1 30 0ua 1uamw 3ua 5ufa 7fa" },

                { L_,           "@2012/3/1 30 0A 5B",
                                     "@2012/3/1 30 1D 3A 5ABC 7",
                                              "@2012/3/1 30 0A 1D 3A 5ABC 7" },

                { L_,          "@2012/3/1 30 0A 5BC",
                                     "@2012/3/1 30 1D 3A 5ABC 7",
                                              "@2012/3/1 30 0A 1D 3A 5ABC 7" },

                { L_,          "@2012/3/1 30 0A 5BC",
                                       "@2012/3/1 30 1D 3A 5A 7",
                                              "@2012/3/1 30 0A 1D 3A 5ABC 7" },

                { L_,  "@2012/3/1 30 0ua 5fa 0A 5BC",
                          "@2012/3/1 30 1mw 3ua 7f 1D 3A 5ABC 7",
                       "@2012/3/1 30 0ua 1uamw 3ua 5ufa 7fa 0A 1D 3A 5ABC 7" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE     = DATA[i].d_line;
                const char *SPEC1    = DATA[i].d_spec1_p;
                const char *SPEC2    = DATA[i].d_spec2_p;
                const char *EXP_SPEC = DATA[i].d_exp_p;

                Obj mX;    const Obj X   = gg(&mX, SPEC1);

                Obj mY;    const Obj Y   = gg(&mY, SPEC2);

                Obj mEXP;  const Obj EXP = gg(&mEXP, EXP_SPEC);

                if (veryVerbose) {
                    T_ P_(LINE) P_(X) P_(Y) P(EXP);
                }

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                bslma::TestAllocator testAllocator("supplied",
                                                   veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mZ(X, &testAllocator);  const Obj& Z = mZ;
                    mZ.intersectBusinessDays(Y);
                    ASSERTV(LINE, EXP, Z, EXP == Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(LINE, allocations ==
                                            defaultAllocator.numAllocations());

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mZ(Y, &testAllocator);  const Obj& Z = mZ;
                    mZ.intersectBusinessDays(X);
                    ASSERTV(LINE, EXP, Z, EXP == Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(LINE, allocations ==
                                            defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nTesting 'intersectNonBusinessDays'." << endl;
        {
            static const struct {
                int         d_line;     // source line number
                const char *d_spec1_p;  // specification for an argument
                const char *d_spec2_p;  // specification for an argument
                const char *d_exp_p;    // expected specification
            } DATA[] = {
                //LN              SPEC1                 SPEC2        EXP
                //--  ------------------------------  ----------  ----------
                { L_,                             "",         "",         "" },

                { L_,                 "@2012/3/1 30",         "",         "" },

                { L_,                 "@2012/3/1 30",
                                                  "@2012/3/1 15",
                                                              "@2012/3/1 15" },
                { L_,                 "@2012/3/1 30",
                                                  "@2012/4/1 15",         "" },

                { L_,                 "@2012/3/1 30",
                                       "@2012/3/1 30 1mw 3ua 7f",
                                                              "@2012/3/1 30" },

                { L_,         "@2012/3/1 30 0ua 3fa",
                                       "@2012/3/1 30 1mw 3ua 7f",
                                                        "@2012/3/1 30 3a 7f" },

                { L_,         "@2012/3/1 30 0ua 5fa",
                                       "@2012/3/1 30 1mw 3ua 7f",
                                                    "@2012/3/1 30 3ua 5a 7f" },

                { L_,           "@2012/3/1 30 0A 5B",
                                     "@2012/3/1 30 1D 3A 5ABC 7",
                                                         "@2012/3/1 30 5ABC" },

                { L_,          "@2012/3/1 30 0A 5BC",
                                     "@2012/3/1 30 1D 3A 5ABC 7",
                                                         "@2012/3/1 30 5ABC" },

                { L_,          "@2012/3/1 30 0A 5BC",
                                       "@2012/3/1 30 1D 3A 5A 7",
                                                         "@2012/3/1 30 5ABC" },

                { L_,  "@2012/3/1 30 0ua 5fa 0A 5BC",
                          "@2012/3/1 30 1mw 3ua 7f 1D 3A 5ABC 7",
                                               "@2012/3/1 30 3ua 5a 7f 5ABC" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE     = DATA[i].d_line;
                const char *SPEC1    = DATA[i].d_spec1_p;
                const char *SPEC2    = DATA[i].d_spec2_p;
                const char *EXP_SPEC = DATA[i].d_exp_p;

                Obj mX;    const Obj X   = gg(&mX, SPEC1);

                Obj mY;    const Obj Y   = gg(&mY, SPEC2);

                Obj mEXP;  const Obj EXP = gg(&mEXP, EXP_SPEC);

                if (veryVerbose) {
                    T_ P_(LINE) P_(X) P_(Y) P(EXP);
                }

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                bslma::TestAllocator testAllocator("supplied",
                                                   veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mZ(X, &testAllocator);  const Obj& Z = mZ;
                    mZ.intersectNonBusinessDays(Y);
                    ASSERTV(LINE, EXP, Z, EXP == Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(LINE, allocations ==
                                            defaultAllocator.numAllocations());

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mZ(Y, &testAllocator);  const Obj& Z = mZ;
                    mZ.intersectNonBusinessDays(X);
                    ASSERTV(LINE, EXP, Z, EXP == Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(LINE, allocations ==
                                            defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nTesting 'unionBusinessDays'." << endl;
        {
            static const struct {
                int         d_line;     // source line number
                const char *d_spec1_p;  // specification for an argument
                const char *d_spec2_p;  // specification for an argument
                const char *d_exp_p;    // expected specification
            } DATA[] = {
                //LN              SPEC1                 SPEC2        EXP
                //--  ------------------------------  ----------  ----------
                { L_,                             "",         "",         "" },

                { L_,                 "@2012/3/1 30",         "",
                                                              "@2012/3/1 30" },

                { L_,                 "@2012/3/1 30",
                                                  "@2012/3/1 15",
                                                              "@2012/3/1 30" },
                { L_,                 "@2012/3/1 30",
                                                  "@2012/4/1 15",
                                                              "@2012/3/1 46" },

                { L_,                 "@2012/3/1 30",
                                       "@2012/3/1 30 1mw 3ua 7f",
                                                              "@2012/3/1 30" },

                { L_,         "@2012/3/1 30 0ua 3fa",
                                       "@2012/3/1 30 1mw 3ua 7f",
                                                        "@2012/3/1 30 3a 7f" },

                { L_,         "@2012/3/1 30 0ua 5fa",
                                       "@2012/3/1 30 1mw 3ua 7f",
                                                    "@2012/3/1 30 3ua 5a 7f" },

                { L_,           "@2012/3/1 30 0A 5B",
                                     "@2012/3/1 30 1D 3A 5ABC 7",
                                                         "@2012/3/1 30 5ABC" },

                { L_,          "@2012/3/1 30 0A 5BC",
                                     "@2012/3/1 30 1D 3A 5ABC 7",
                                                         "@2012/3/1 30 5ABC" },

                { L_,          "@2012/3/1 30 0A 5BC",
                                       "@2012/3/1 30 1D 3A 5A 7",
                                                         "@2012/3/1 30 5ABC" },

                { L_,  "@2012/3/1 30 0ua 5fa 0A 5BC",
                          "@2012/3/1 30 1mw 3ua 7f 1D 3A 5ABC 7",
                                               "@2012/3/1 30 3ua 5a 7f 5ABC" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE     = DATA[i].d_line;
                const char *SPEC1    = DATA[i].d_spec1_p;
                const char *SPEC2    = DATA[i].d_spec2_p;
                const char *EXP_SPEC = DATA[i].d_exp_p;

                Obj mX;    const Obj X   = gg(&mX, SPEC1);

                Obj mY;    const Obj Y   = gg(&mY, SPEC2);

                Obj mEXP;  const Obj EXP = gg(&mEXP, EXP_SPEC);

                if (veryVerbose) {
                    T_ P_(LINE) P_(X) P_(Y) P(EXP);
                }

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                bslma::TestAllocator testAllocator("supplied",
                                                   veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mZ(X, &testAllocator);  const Obj& Z = mZ;
                    mZ.unionBusinessDays(Y);
                    ASSERTV(LINE, EXP, Z, EXP == Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(LINE, allocations ==
                                            defaultAllocator.numAllocations());

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mZ(Y, &testAllocator);  const Obj& Z = mZ;
                    mZ.unionBusinessDays(X);
                    ASSERTV(LINE, EXP, Z, EXP == Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(LINE, allocations ==
                                            defaultAllocator.numAllocations());
            }
        }

        if (verbose) cout << "\nTesting 'unionNonBusinessDays'." << endl;
        {
            static const struct {
                int         d_line;     // source line number
                const char *d_spec1_p;  // specification for an argument
                const char *d_spec2_p;  // specification for an argument
                const char *d_exp_p;    // expected specification
            } DATA[] = {
                //LN              SPEC1                 SPEC2        EXP
                //--  ------------------------------  ----------  ----------
                { L_,                             "",         "",         "" },

                { L_,                 "@2012/3/1 30",         "",
                                                              "@2012/3/1 30" },

                { L_,                 "@2012/3/1 30",
                                                  "@2012/3/1 15",
                                                              "@2012/3/1 30" },
                { L_,                 "@2012/3/1 30",
                                                  "@2012/4/1 15",
                                                              "@2012/3/1 46" },

                { L_,                 "@2012/3/1 30",
                                       "@2012/3/1 30 1mw 3ua 7f",
                                                   "@2012/3/1 30 1mw 3ua 7f" },

                { L_,         "@2012/3/1 30 0ua 3fa",
                                       "@2012/3/1 30 1mw 3ua 7f",
                                           "@2012/3/1 30 0ua 1uamw 3ufa 7fa" },

                { L_,         "@2012/3/1 30 0ua 5fa",
                                       "@2012/3/1 30 1mw 3ua 7f",
                                       "@2012/3/1 30 0ua 1uamw 3ua 5ufa 7fa" },

                { L_,           "@2012/3/1 30 0A 5B",
                                     "@2012/3/1 30 1D 3A 5ABC 7",
                                              "@2012/3/1 30 0A 1D 3A 5ABC 7" },

                { L_,          "@2012/3/1 30 0A 5BC",
                                     "@2012/3/1 30 1D 3A 5ABC 7",
                                              "@2012/3/1 30 0A 1D 3A 5ABC 7" },

                { L_,          "@2012/3/1 30 0A 5BC",
                                       "@2012/3/1 30 1D 3A 5A 7",
                                              "@2012/3/1 30 0A 1D 3A 5ABC 7" },

                { L_,  "@2012/3/1 30 0ua 5fa 0A 5BC",
                          "@2012/3/1 30 1mw 3ua 7f 1D 3A 5ABC 7",
                       "@2012/3/1 30 0ua 1uamw 3ua 5ufa 7fa 0A 1D 3A 5ABC 7" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE     = DATA[i].d_line;
                const char *SPEC1    = DATA[i].d_spec1_p;
                const char *SPEC2    = DATA[i].d_spec2_p;
                const char *EXP_SPEC = DATA[i].d_exp_p;

                Obj mX;    const Obj X   = gg(&mX, SPEC1);

                Obj mY;    const Obj Y   = gg(&mY, SPEC2);

                Obj mEXP;  const Obj EXP = gg(&mEXP, EXP_SPEC);

                if (veryVerbose) {
                    T_ P_(LINE) P_(X) P_(Y) P(EXP);
                }

                bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                bslma::TestAllocator testAllocator("supplied",
                                                   veryVeryVeryVerbose);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mZ(X, &testAllocator);  const Obj& Z = mZ;
                    mZ.unionNonBusinessDays(Y);
                    ASSERTV(LINE, EXP, Z, EXP == Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(LINE, allocations ==
                                            defaultAllocator.numAllocations());

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                    Obj mZ(Y, &testAllocator);  const Obj& Z = mZ;
                    mZ.unionNonBusinessDays(X);
                    ASSERTV(LINE, EXP, Z, EXP == Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP_ASSERT(LINE, allocations ==
                                            defaultAllocator.numAllocations());
            }
        }
      } break;
      case 17: {
        // --------------------------------------------------------------------
        // TESTING RESERVE CAPACITY METHODS
        //   The 'reserveHolidayCapacity' and 'reserveHolidayCodeCapacity'
        //   methods operate as expected.
        //
        // Concerns:
        //: 1 The methods are exception neutral with respect to memory
        //:   allocation.
        //:
        //: 2 Any memory allocation is from the specified allocator.
        //:
        //: 3 The methods produce the expected state without affecting the
        //:   value.
        //:
        //: 4 QoI: asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 In all cases, confirm the allocator used and exception
        //:   neutrality by using the specially instrumented
        //:   'bslma::TestAllocator', its accessor methods, and a pair of
        //:   standard macros, 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN' and
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END', which configure the
        //:   'bslma::TestAllocator' object appropriately in a loop.  (C-1..2)
        //:
        //: 2 Directly verify the methods by using the method, verifying the
        //:   value has not changed, and then adding elements to ensure no
        //:   allocations are performed until the reserved capacity is
        //:   exhausted.  (C-3)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-4)
        //
        // Testing:
        //   void reserveHolidayCapacity(int numHolidays);
        //   void reserveHolidayCodeCapacity(int numHolidayCodes);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING RESERVE CAPACITY METHODS" << endl
                          << "================================" << endl;

        // Note that the current implementation will reserve 8 bytes per
        // requested item; the number of elements added may have to be (much)
        // larger than the requested capacity to force an allocation.

        Obj mZ;  const Obj& Z = gg(&mZ, "@2012/01/01 30 0");

        bsls::Types::Int64 defaultAllocations =
                                             defaultAllocator.numAllocations();

        bslma::TestAllocator testAllocator("supplied",
                                           veryVeryVeryVerbose);

        const int SIZE = 100;

        for (int num = 0; num <= SIZE; ++num) {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                int offset = 0;

                Obj mX(Z, &testAllocator);  const Obj& X = mX;

                const Obj Y(X, &testAllocator);

                mX.reserveHolidayCapacity(num);

                ASSERT(X == Y);

                bsls::Types::Int64 allocations =
                                                testAllocator.numAllocations();

                while (allocations == testAllocator.numAllocations()) {
                    mX.addHoliday(mX.firstDate() + offset++);
                }

                ASSERTV(num, offset, offset >= num);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        for (int num = 0; num <= SIZE; ++num) {
            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(testAllocator) {
                int offset = 0;

                Obj mX(Z, &testAllocator);  const Obj& X = mX;

                const Obj Y(X, &testAllocator);

                mX.reserveHolidayCodeCapacity(num);

                ASSERT(X == Y);

                bsls::Types::Int64 allocations =
                                                testAllocator.numAllocations();

                while (allocations == testAllocator.numAllocations()) {
                    mX.addHolidayCode(mX.firstDate(), offset++);
                }

                ASSERTV(num, offset, offset >= num);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END
        }

        ASSERT(defaultAllocations == defaultAllocator.numAllocations());
      } break;
      case 16: {
        // --------------------------------------------------------------------
        // TESTING 'removeHoliday' AND 'removeHolidayCode'
        //   The manipulators operate as expected.
        //
        // Concerns:
        //: 1 That 'removeHoliday' and 'removeHolidayCode' correctly modify
        //:   the value of the object.
        //
        // Plan:
        //: 1 For each row in a table, create two objects using the generator
        //:   function; 'X' representing the initial value and 'Z' the
        //:   expected value after modifying 'X' with the method under test
        //:   and the parameters specified in the row.  Modify 'X' with the
        //:   method under test and parameters from the row and verify the
        //:   result and return value against the expected return value
        //:   supplied in the row.  (C-1)
        //
        // Testing:
        //   void removeHoliday(const Date& date);
        //   void removeHolidayCode(const Date& date, int holidayCode);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'removeHoliday' AND 'removeHolidayCode'" << endl
                 << "===============================================" << endl;
        }

        if (verbose) cout << "\nTesting 'removeHoliday'." << endl;
        {
            static const struct {
                int         d_line;    // source line number
                const char *d_spec_p;  // initial specification
                int         d_year;    // input year
                int         d_month;   // input month
                int         d_day;     // input day
                const char *d_exp_p;   // expected specification
            } DATA[] = {
                //LN            SPEC            YEAR  M   D        EXP
                //--  ------------------------  ----  --  --  --------------
                { L_,                       "", 2000,  1,  1,             "" },

                { L_,         "@2012/3/1 30 4", 2012,  3,  4,
                                                            "@2012/3/1 30 4" },
                { L_,         "@2012/3/1 30 4", 2012,  3,  5, "@2012/3/1 30" },
                { L_,         "@2012/3/1 30 4", 2012,  3,  6,
                                                            "@2012/3/1 30 4" },

                { L_,        "@2012/3/1 30 4C", 2012,  3,  4,
                                                           "@2012/3/1 30 4C" },
                { L_,        "@2012/3/1 30 4C", 2012,  3,  5, "@2012/3/1 30" },
                { L_,        "@2012/3/1 30 4C", 2012,  3,  6,
                                                           "@2012/3/1 30 4C" },

                { L_,  "@2012/3/1 30 4BC 6 8A", 2012,  3,  5,
                                                         "@2012/3/1 30 6 8A" },
                { L_,  "@2012/3/1 30 4BC 6 8A", 2012,  3,  7,
                                                       "@2012/3/1 30 4BC 8A" },
                { L_,  "@2012/3/1 30 4BC 6 8A", 2012,  3,  9,
                                                        "@2012/3/1 30 4BC 6" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_spec_p;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const char *EXP   = DATA[i].d_exp_p;

                const bdlt::Date DATE(YEAR, MONTH, DAY);

                Obj mX;  const Obj& X = gg(&mX, SPEC);

                Obj mZ;  const Obj& Z = gg(&mZ, EXP);

                mX.removeHoliday(DATE);

                LOOP3_ASSERT(LINE, X, Z, X == Z);
            }
        }

        if (verbose) cout << "\nTesting 'removeHolidayCode'." << endl;
        {
            static const struct {
                int         d_line;    // source line number
                const char *d_spec_p;  // initial specification
                int         d_year;    // input year
                int         d_month;   // input month
                int         d_day;     // input day
                int         d_code;    // input holiday code
                const char *d_exp_p;   // expected specification
            } DATA[] = {
                //LN            SPEC            YEAR  M   D   CODE    EXP
                //--  ------------------------  ----  --  --  ----  --------
                { L_,                       "", 2000,  1,  1,   VA,       "" },

                { L_,         "@2012/3/1 30 4", 2012,  3,  4,   VA,
                                                            "@2012/3/1 30 4" },
                { L_,         "@2012/3/1 30 4", 2012,  3,  5,   VA,
                                                            "@2012/3/1 30 4" },
                { L_,         "@2012/3/1 30 4", 2012,  3,  6,   VA,
                                                            "@2012/3/1 30 4" },

                { L_,        "@2012/3/1 30 4C", 2012,  3,  4,   VA,
                                                           "@2012/3/1 30 4C" },
                { L_,        "@2012/3/1 30 4C", 2012,  3,  5,   VA,
                                                           "@2012/3/1 30 4C" },
                { L_,        "@2012/3/1 30 4C", 2012,  3,  5,   VC,
                                                            "@2012/3/1 30 4" },
                { L_,        "@2012/3/1 30 4C", 2012,  3,  6,   VA,
                                                           "@2012/3/1 30 4C" },

                { L_,  "@2012/3/1 30 4BC 6 8A", 2012,  3,  5,   VA,
                                                     "@2012/3/1 30 4BC 6 8A" },
                { L_,  "@2012/3/1 30 4BC 6 8A", 2012,  3,  5,   VB,
                                                      "@2012/3/1 30 4C 6 8A" },
                { L_,  "@2012/3/1 30 4BC 6 8A", 2012,  3,  5,   VC,
                                                      "@2012/3/1 30 4B 6 8A" },
                { L_,  "@2012/3/1 30 4BC 6 8A", 2012,  3,  5,   VD,
                                                     "@2012/3/1 30 4BC 6 8A" },
                { L_,  "@2012/3/1 30 4BC 6 8A", 2012,  3,  7,   VA,
                                                     "@2012/3/1 30 4BC 6 8A" },
                { L_,  "@2012/3/1 30 4BC 6 8A", 2012,  3,  9,   VA,
                                                      "@2012/3/1 30 4BC 6 8" },
                { L_,  "@2012/3/1 30 4BC 6 8A", 2012,  3,  9,   VB,
                                                     "@2012/3/1 30 4BC 6 8A" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_spec_p;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   CODE  = DATA[i].d_code;
                const char *EXP   = DATA[i].d_exp_p;

                const bdlt::Date DATE(YEAR, MONTH, DAY);

                Obj mX;  const Obj& X = gg(&mX, SPEC);

                Obj mZ;  const Obj& Z = gg(&mZ, EXP);

                mX.removeHolidayCode(DATE, CODE);

                LOOP3_ASSERT(LINE, X, Z, X == Z);
            }
        }
      } break;
      case 15: {
        // --------------------------------------------------------------------
        // TESTING 'setValidRange'
        //   The manipulator operates as expected.
        //
        // Concerns:
        //: 1 That 'setValidRange' correctly modifies the value of the object.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each row in a table, create two objects using the generator
        //:   function; 'X' representing the initial value and 'Z' the
        //:   expected value after modifying 'X' with the method under test
        //:   and the parameters specified in the row.  Modify 'X' with the
        //:   method under test and parameters from the row and verify the
        //:   result.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //  void setValidRange(const Date& fd, const Date& ld);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'setValidRange'" << endl
                          << "=======================" << endl;

        static const struct {
            int         d_line;    // source line number
            const char *d_spec_p;  // initial specification
            int         d_year1;   // input year1
            int         d_month1;  // input month1
            int         d_day1;    // input day1
            int         d_year2;   // input year2
            int         d_month2;  // input month2
            int         d_day2;    // input day2
            const char *d_exp_p;   // expected specification
        } DATA[] = {
            //LN          SPEC            Y1   M1  D1   Y2   M2  D2    EXP
            //--  ---------------------  ----  --  --  ----  --  --  -------
            { L_,                    "", 2012,  1,  1, 2012,  1, 31,
                                                              "@2012/1/1 30" },

            { L_,          "@2012/1/15", 2012,  1,  1, 2012,  1, 31,
                                                              "@2012/1/1 30" },

            { L_,      "@2012/1/15 5 3", 2012,  1,  1, 2012,  1, 31,
                                                           "@2012/1/1 30 17" },
            { L_,     "@2012/1/15 5 3A", 2012,  1,  1, 2012,  1, 31,
                                                          "@2012/1/1 30 17A" },

            { L_,  "@2012/1/1 30 4A 9B", 2012,  1,  1, 2012,  1,  3,
                                                               "@2012/1/1 2" },
            { L_,  "@2012/1/1 30 4A 9B", 2012,  1,  1, 2012,  1,  6,
                                                            "@2012/1/1 5 4A" },
            { L_,  "@2012/1/1 30 4A 9B", 2012,  1,  1, 2012,  1, 16,
                                                        "@2012/1/1 15 4A 9B" },
            { L_,  "@2012/1/1 30 4A 9B", 2012,  1,  3, 2012,  1, 30,
                                                        "@2012/1/3 27 2A 7B" },
            { L_,  "@2012/1/1 30 4A 9B", 2012,  1,  6, 2012,  1, 30,
                                                           "@2012/1/6 24 4B" },
            { L_,  "@2012/1/1 30 4A 9B", 2012,  1, 16, 2012,  1, 30,
                                                             "@2012/1/16 14" },
        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        for (int i = 0; i < NUM_DATA; ++i) {
            const int   LINE   = DATA[i].d_line;
            const char *SPEC   = DATA[i].d_spec_p;
            const int   YEAR1  = DATA[i].d_year1;
            const int   MONTH1 = DATA[i].d_month1;
            const int   DAY1   = DATA[i].d_day1;
            const int   YEAR2  = DATA[i].d_year2;
            const int   MONTH2 = DATA[i].d_month2;
            const int   DAY2   = DATA[i].d_day2;
            const char *EXP    = DATA[i].d_exp_p;

            const bdlt::Date DATE1(YEAR1, MONTH1, DAY1);
            const bdlt::Date DATE2(YEAR2, MONTH2, DAY2);

            Obj mX;  const Obj& X = gg(&mX, SPEC);

            Obj mZ;  const Obj& Z = gg(&mZ, EXP);

            mX.setValidRange(DATE1, DATE2);
            LOOP3_ASSERT(LINE, X, Z, X == Z);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;

            ASSERT_PASS(mX.setValidRange(bdlt::Date(2012, 1,  1),
                                         bdlt::Date(2012, 1, 15)));
            ASSERT_FAIL(mX.setValidRange(bdlt::Date(2012, 1, 31),
                                         bdlt::Date(2012, 1, 15)));
        }
      } break;
      case 14: {
        // --------------------------------------------------------------------
        // TESTING 'addWeekendDays'
        //   The manipulator operates as expected.
        //
        // Concerns:
        //: 1 That 'addWeekendDays' correctly modifies the value of the object.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly test the method works as expected.  (C-1)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //  void addWeekendDays(const DayOfWeekSet& weekendDays);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'addWeekendDays'" << endl
                          << "========================" << endl;

        if (verbose) cout << "\nTesting 'addWeekendDays'." << endl;
        {
            Obj mX;  const Obj& X = mX;

            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_SUN) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_MON) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_TUE) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_WED) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_THU) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_FRI) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_SAT) == false);

            {
                bdlt::DayOfWeekSet wd;
                wd.add(bdlt::DayOfWeek::e_SUN);
                wd.add(bdlt::DayOfWeek::e_SAT);

                mX.addWeekendDays(wd);
            }

            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_SUN) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_MON) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_TUE) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_WED) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_THU) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_FRI) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_SAT) == true);

            {
                bdlt::DayOfWeekSet wd;
                wd.add(bdlt::DayOfWeek::e_MON);
                wd.add(bdlt::DayOfWeek::e_WED);
                wd.add(bdlt::DayOfWeek::e_FRI);

                mX.addWeekendDays(wd);
            }

            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_SUN) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_MON) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_TUE) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_WED) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_THU) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_FRI) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_SAT) == true);

            {
                bdlt::DayOfWeekSet wd;
                wd.add(bdlt::DayOfWeek::e_TUE);
                wd.add(bdlt::DayOfWeek::e_THU);

                mX.addWeekendDays(wd);
            }

            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_SUN) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_MON) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_TUE) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_WED) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_THU) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_FRI) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_SAT) == true);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            {
                Obj mX;

                bdlt::DayOfWeekSet wd;
                wd.add(bdlt::DayOfWeek::e_SUN);
                wd.add(bdlt::DayOfWeek::e_SAT);

                ASSERT_SAFE_PASS(mX.addWeekendDays(wd));
            }

            {
                Obj mX;

                bdlt::DayOfWeekSet wd;
                wd.add(bdlt::DayOfWeek::e_SUN);
                wd.add(bdlt::DayOfWeek::e_SAT);

                mX.addWeekendDaysTransition(bdlt::Date(2012, 3, 1), wd);

                ASSERT_SAFE_FAIL(mX.addWeekendDays(wd));
            }
        }
      } break;
      case 13: {
        // --------------------------------------------------------------------
        // TESTING 'addHolidayIfInRange' & 'addHolidayCodeIfInRange'
        //   The manipulators operate as expected.
        //
        // Concerns:
        //: 1 That 'addHolidayIfInRange' and 'addHolidayCodeIfInRange'
        //:   correctly check whether the supplied date 'isInRange', forward
        //:   to 'addHoliday' and 'addHolidayCode' as appropriate, and return
        //:   the correct value.
        //
        // Plan:
        //: 1 For each row in a table, create two objects using the generator
        //:   function; 'X' representing the initial value and 'Z' the
        //:   expected value after modifying 'X' with the method under test
        //:   and the parameters specified in the row.  Modify 'X' with the
        //:   method under test and parameters from the row and verify the
        //:   result and return value against the expected return value
        //:   supplied in the row.  (C-1)
        //
        // Testing:
        //   int addHolidayIfInRange(const Date& date);
        //   int addHolidayCodeIfInRange(const Date& date, int holidayCode);
        // --------------------------------------------------------------------

        if (verbose) {
            cout << endl
                 << "TESTING 'addHolidayIfInRange' & 'addHolidayCodeIfInRange'"
                 << endl
                 << "========================================================="
                 << endl;
        }

        if (verbose) cout << "\nTesting 'addHolidayIfInRange'." << endl;
        {
            static const struct {
                int         d_line;    // source line number
                const char *d_spec_p;  // initial specification
                int         d_year;    // input year
                int         d_month;   // input month
                int         d_day;     // input day
                int         d_rv;      // expected return value
                const char *d_exp_p;   // expected specification
            } DATA[] = {
                //LN       SPEC        YEAR  M   D   R         EXP
                //--  ---------------  ----  --  --  -  -----------------
                { L_,              "",    1,  1,  1, 1, ""                },
                { L_,              "", 9999, 12, 31, 1, ""                },
                { L_,              "", 2000,  1,  1, 1, ""                },

                { L_,     "@2012/3/1",    1,  1,  1, 1, "@2012/3/1"       },
                { L_,     "@2012/3/1", 9999, 12, 31, 1, "@2012/3/1"       },
                { L_,     "@2012/3/1", 2012,  2, 29, 1, "@2012/3/1"       },
                { L_,     "@2012/3/1", 2012,  3,  2, 1, "@2012/3/1"       },
                { L_,     "@2012/3/1", 2012,  3,  1, 0, "@2012/3/1 0 0"   },

                { L_,  "@2012/3/1 30",    1,  1,  1, 1, "@2012/3/1 30"    },
                { L_,  "@2012/3/1 30", 9999, 12, 31, 1, "@2012/3/1 30"    },
                { L_,  "@2012/3/1 30", 2012,  2, 29, 1, "@2012/3/1 30"    },
                { L_,  "@2012/3/1 30", 2012,  4,  1, 1, "@2012/3/1 30"    },
                { L_,  "@2012/3/1 30", 2012,  3,  1, 0, "@2012/3/1 30 0"  },
                { L_,  "@2012/3/1 30", 2012,  3, 31, 0, "@2012/3/1 30 30" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_spec_p;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   RV    = DATA[i].d_rv;
                const char *EXP   = DATA[i].d_exp_p;

                const bdlt::Date DATE(YEAR, MONTH, DAY);

                Obj mX;  const Obj& X = gg(&mX, SPEC);

                Obj mZ;  const Obj& Z = gg(&mZ, EXP);

                LOOP_ASSERT(LINE,
                             (0 == RV) == (0 == mX.addHolidayIfInRange(DATE)));

                LOOP3_ASSERT(LINE, X, Z, X == Z);
            }
        }

        if (verbose) cout << "\nTesting 'addHolidayCodeIfInRange'." << endl;
        {
            static const struct {
                int         d_line;    // source line number
                const char *d_spec_p;  // initial specification
                int         d_year;    // input year
                int         d_month;   // input month
                int         d_day;     // input day
                int         d_code;    // input holiday code
                int         d_rv;      // expected return value
                const char *d_exp_p;   // expected specification
            } DATA[] = {
                //LN       SPEC        YEAR  M   D   CODE  R       EXP
                //--  ---------------  ----  --  --  ----  -  --------------
                { L_,           "",       1,  1,  1,   VA, 1, ""             },
                { L_,           "",    9999, 12, 31,   VA, 1, ""             },
                { L_,           "",    2000,  1,  1,   VA, 1, ""             },

                { L_,  "@2012/3/1",       1,  1,  1,   VA, 1, "@2012/3/1"    },
                { L_,  "@2012/3/1",    9999, 12, 31,   VA, 1, "@2012/3/1"    },
                { L_,  "@2012/3/1",    2012,  2, 29,   VA, 1, "@2012/3/1"    },
                { L_,  "@2012/3/1",    2012,  3,  2,   VA, 1, "@2012/3/1"    },
                { L_,  "@2012/3/1",    2012,  3,  1,   VA, 0,
                                                            "@2012/3/1 0 0A" },

                { L_,  "@2012/3/1 30",    1,  1,  1,   VA, 1, "@2012/3/1 30" },
                { L_,  "@2012/3/1 30", 9999, 12, 31,   VA, 1, "@2012/3/1 30" },
                { L_,  "@2012/3/1 30", 2012,  2, 29,   VA, 1, "@2012/3/1 30" },
                { L_,  "@2012/3/1 30", 2012,  4,  1,   VA, 1, "@2012/3/1 30" },
                { L_,  "@2012/3/1 30", 2012,  3,  1,   VB, 0,
                                                           "@2012/3/1 30 0B" },
                { L_,  "@2012/3/1 30", 2012,  3, 31,   VC, 0,
                                                          "@2012/3/1 30 30C" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int   LINE  = DATA[i].d_line;
                const char *SPEC  = DATA[i].d_spec_p;
                const int   YEAR  = DATA[i].d_year;
                const int   MONTH = DATA[i].d_month;
                const int   DAY   = DATA[i].d_day;
                const int   CODE  = DATA[i].d_code;
                const int   RV    = DATA[i].d_rv;
                const char *EXP   = DATA[i].d_exp_p;

                const bdlt::Date DATE(YEAR, MONTH, DAY);

                Obj mX;  const Obj& X = gg(&mX, SPEC);

                Obj mZ;  const Obj& Z = gg(&mZ, EXP);

                LOOP_ASSERT(LINE,
                            (0 == RV) ==
                                (0 == mX.addHolidayCodeIfInRange(DATE, CODE)));

                LOOP3_ASSERT(LINE, X, Z, X == Z);
            }
        }
      } break;
      case 12: {
        // --------------------------------------------------------------------
        // RANGE CONSTRUCTOR
        //   The constructor operates as expected.
        //
        // Concerns:
        //: 1 The range constructor creates the correct initial value and has
        //:   the internal memory management system hooked up properly so that
        //:   *all* internally allocated memory draws from the same
        //:   user-supplied allocator whenever one is specified.
        //:
        //: 2 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 For each row in a table of initial values, create an object
        //:   using the range constructor with and without passing in an
        //:   allocator, verify the 'firstDate' and 'lastDate', and verify the
        //:   allocator is stored using the 'allocator' accessor.
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-2)
        //
        // Testing:
        //   PackedCalendar(const Date& firstDate, lastDate, ba = 0);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "RANGE CONSTRUCTOR" << endl
                          << "=================" << endl;

        {
            static const struct {
                int d_line;        // source line number
                int d_year1;       // input first year
                int d_month1;      // input first month
                int d_day1;        // input first day
                int d_year2;       // input last year
                int d_month2;      // input last month
                int d_day2;        // input last day
                int d_firstYear;   // output first year
                int d_firstMonth;  // output first month
                int d_firstDay;    // output first day
                int d_lastYear;    // output last year
                int d_lastMonth;   // output last month
                int d_lastDay;     // output last day
            } DATA[] = {
                //LN   Y1   M1  D1   Y2   M2  D2   FY   FM  FD   LY   LM  LD
                //--  ----  --  --  ----  --  --  ----  --  --  ----  --  --
                { L_, 2000,  1,  1, 2000,  1,  1, 2000,  1,  1, 2000,  1,  1 },
                { L_, 2000,  1,  1, 2001,  1,  1, 2000,  1,  1, 2001,  1,  1 },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE   = DATA[i].d_line;
                const int YEAR1  = DATA[i].d_year1;
                const int MONTH1 = DATA[i].d_month1;
                const int DAY1   = DATA[i].d_day1;
                const int YEAR2  = DATA[i].d_year2;
                const int MONTH2 = DATA[i].d_month2;
                const int DAY2   = DATA[i].d_day2;
                const int FYEAR  = DATA[i].d_firstYear;
                const int FMONTH = DATA[i].d_firstMonth;
                const int FDAY   = DATA[i].d_firstDay;
                const int LYEAR  = DATA[i].d_lastYear;
                const int LMONTH = DATA[i].d_lastMonth;
                const int LDAY   = DATA[i].d_lastDay;

                const bdlt::Date DATE1(YEAR1, MONTH1, DAY1);
                const bdlt::Date DATE2(YEAR2, MONTH2, DAY2);
                const bdlt::Date EXP_FIRST(FYEAR, FMONTH, FDAY);
                const bdlt::Date EXP_LAST(LYEAR, LMONTH, LDAY);

                {
                    bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                    Obj mX(DATE1, DATE2);  const Obj& X = mX;
                    LOOP_ASSERT(LINE, &defaultAllocator == X.allocator());
                    LOOP_ASSERT(LINE, EXP_FIRST == X.firstDate());
                    LOOP_ASSERT(LINE, EXP_LAST == X.lastDate());
                    LOOP_ASSERT(LINE, 0 == X.numHolidays());
                    LOOP_ASSERT(LINE, 0 == X.numHolidayCodesTotal());
                    LOOP_ASSERT(LINE, 0 == X.numWeekendDaysTransitions());
                    LOOP_ASSERT(LINE, allocations ==
                                            defaultAllocator.numAllocations());
                }
                {
                    bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                    Obj        mX(DATE1,
                                  DATE2,
                                  reinterpret_cast<bslma::TestAllocator *>(0));
                    const Obj& X = mX;
                    LOOP_ASSERT(LINE, &defaultAllocator == X.allocator());
                    LOOP_ASSERT(LINE, EXP_FIRST == X.firstDate());
                    LOOP_ASSERT(LINE, EXP_LAST == X.lastDate());
                    LOOP_ASSERT(LINE, 0 == X.numHolidays());
                    LOOP_ASSERT(LINE, 0 == X.numHolidayCodesTotal());
                    LOOP_ASSERT(LINE, 0 == X.numWeekendDaysTransitions());
                    LOOP_ASSERT(LINE, allocations ==
                                            defaultAllocator.numAllocations());
                }
                {
                    bsls::Types::Int64 allocations =
                                             defaultAllocator.numAllocations();

                    bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

                    Obj mX(DATE1, DATE2, &sa);  const Obj& X = mX;
                    LOOP_ASSERT(LINE, &sa == X.allocator());
                    LOOP_ASSERT(LINE, EXP_FIRST == X.firstDate());
                    LOOP_ASSERT(LINE, EXP_LAST == X.lastDate());
                    LOOP_ASSERT(LINE, 0 == X.numHolidays());
                    LOOP_ASSERT(LINE, 0 == X.numHolidayCodesTotal());
                    LOOP_ASSERT(LINE, 0 == X.numWeekendDaysTransitions());
                    LOOP_ASSERT(LINE, allocations ==
                                            defaultAllocator.numAllocations());
                    LOOP_ASSERT(LINE, 0 == sa.numAllocations());
                }
            }
        }

        // Negative testing.

        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            ASSERT_PASS(Obj(bdlt::Date(2012, 1, 5), bdlt::Date(2012, 1, 5)));
            ASSERT_FAIL(Obj(bdlt::Date(2012, 1, 5), bdlt::Date(2012, 1, 4)));
        }
      } break;
      case 11: {
        // -------------------------------------------------------------------
        // NON-BASIC ACCESSORS
        //   Ensure each non-basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each of the non-basic accessors returns the expected value.
        //:
        //: 2 Each non-basic accessor method is declared 'const'.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Directly test that each non-basic accessor, invoked on a set of
        //:   'const' objects created with the generator function, returns the
        //:   expected value as determined by an oracle implementation using
        //:   the basic accessors.  (C-1..2)
        //:
        //: 2 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   bool isBusinessDay(const Date& date) const;
        //   bool isNonBusinessDay(const Date& date) const;
        //   int length() const;
        //   int numBusinessDays() const;
        //   int numHolidayCodesTotal() const;
        //   int numNonBusinessDays() const;
        //   int numWeekendDaysInRange() const;
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "NON-BASIC ACCESSORS" << endl
                          << "===================" << endl;

        const char **SPECS = DEFAULT_SPECS;

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC = SPECS[ti];

            Obj mX;  const Obj& X = gg(&mX, SPEC);

            int LENGTH = 0;
            int NUM_NONBUSINESSDAY = 0;
            int NUM_WEEKENDDAY = 0;
            int NUM_HOLIDAYCODES = 0;

            bdlt::Date date;
            bool       notDone;
            if (X.length()) {
                date = X.firstDate();
                notDone = true;
            }
            else {
                notDone = false;
            }
            while (notDone) {
                const bool NONBUSINESSDAY = X.isHoliday(date)
                                         || X.isWeekendDay(date);

                LOOP2_ASSERT(ti,
                             date,
                             NONBUSINESSDAY == X.isNonBusinessDay(date));
                LOOP2_ASSERT(ti,
                             date,
                             !NONBUSINESSDAY == X.isBusinessDay(date));

                ++LENGTH;
                if (NONBUSINESSDAY) {
                    ++NUM_NONBUSINESSDAY;
                }
                if (X.isWeekendDay(date)) {
                    ++NUM_WEEKENDDAY;
                }
                if (X.isHoliday(date)) {
                    NUM_HOLIDAYCODES += X.numHolidayCodes(date);
                }

                if (date < bdlt::Date(9999, 12, 31)) {
                    ++date;
                    notDone = date <= X.lastDate();
                }
                else {
                    notDone = false;
                }
            }

            LOOP_ASSERT(ti, X.length()                == LENGTH);
            LOOP_ASSERT(ti, X.numBusinessDays()       ==
                                                  LENGTH - NUM_NONBUSINESSDAY);
            LOOP_ASSERT(ti, X.numNonBusinessDays()    == NUM_NONBUSINESSDAY);
            LOOP_ASSERT(ti, X.numWeekendDaysInRange() == NUM_WEEKENDDAY);
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
        //: 2 All calls to the 'bdexStreamOut' accessor are done from a 'const'
        //:   object or reference and all calls to the 'bdexStreamOut' free
        //:   function (provided by 'bslx') are supplied a 'const' object or
        //:   reference.  (C-2)
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
        Obj mVA;  const Obj& VA = gg(&mVA, DEFAULT_SPECS[0]);
        Obj mVB;  const Obj& VB = gg(&mVB, DEFAULT_SPECS[1]);
        Obj mVC;  const Obj& VC = gg(&mVC, DEFAULT_SPECS[2]);
        Obj mVD;  const Obj& VD = gg(&mVD, DEFAULT_SPECS[3]);
        Obj mVE;  const Obj& VE = gg(&mVE, DEFAULT_SPECS[4]);

        static const char **SPECS = DEFAULT_SPECS;

        if (verbose) {
            cout << "\nTesting 'maxSupportedBdexVersion'." << endl;
        }
        {
            ASSERT(2 == Obj::maxSupportedBdexVersion(0));
            ASSERT(3 == Obj::maxSupportedBdexVersion(20150612));
            ASSERT(3 == Obj::maxSupportedBdexVersion(VERSION_SELECTOR));

            using bslx::VersionFunctions::maxSupportedBdexVersion;

            ASSERT(2 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                0));
            ASSERT(3 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                20150612));
            ASSERT(3 == maxSupportedBdexVersion(reinterpret_cast<Obj *>(0),
                                                VERSION_SELECTOR));

        }

        const int VERSIONS[] = { 2, 3 };
        const int NUM_VERSIONS = static_cast<int>(  sizeof VERSIONS
                                                  / sizeof *VERSIONS);

        for (int versionIndex = 0;
             versionIndex < NUM_VERSIONS;
             ++versionIndex) {
            const int VERSION = VERSIONS[versionIndex];

            if (verbose) {
                cout << "\nTesting Version " << VERSION << "." << endl;
            }

            if (verbose) {
                cout << "\tDirect initial trial of 'bdexStreamOut' and "
                     << "(valid) 'bdexStreamIn'." << endl;
            }
            {
                const Obj X(VC);
                Out       out(VERSION_SELECTOR, &allocator);

                Out& rvOut = X.bdexStreamOut(out, VERSION);
                ASSERT(&out == &rvOut);

                const char *const OD  = out.data();
                const int         LOD = static_cast<int>(out.length());

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

            // We will use the stream free functions provided by 'bslx', as
            // opposed to the class member functions, since the 'bslx'
            // implementation gives priority to the free function
            // implementations; we want to test what will be used.
            // Furthermore, toward making this test case more reusable in other
            // components, from here on we generally use the 'bdexStreamIn' and
            // 'bdexStreamOut' free functions that are defined in the 'bslx'
            // package rather than call the like-named member functions
            // directly.

            if (verbose) {
                cout << "\tThorough test using stream free functions."
                     << endl;
            }
            {
                for (int i = 0; SPECS[i]; ++i) {
                    Obj mX;  const Obj& X = gg(&mX, SPECS[i]);

                    Out out(VERSION_SELECTOR, &allocator);

                    using bslx::OutStreamFunctions::bdexStreamOut;
                    using bslx::InStreamFunctions::bdexStreamIn;

                    Out& rvOut = bdexStreamOut(out, X, VERSION);
                    LOOP_ASSERT(i, &out == &rvOut);
                    const char *const OD  = out.data();
                    const int         LOD = static_cast<int>(out.length());

                    // Verify that each new value overwrites every old value
                    // and that the input stream is emptied, but remains valid.

                    for (int j = 0; SPECS[j]; ++j) {
                        In in(OD, LOD);
                        LOOP2_ASSERT(i, j, in);
                        LOOP2_ASSERT(i, j, !in.isEmpty());

                        Obj mT;  const Obj& T = gg(&mT, SPECS[j]);;
                        LOOP2_ASSERT(i, j, (X == T) == (i == j));

                        BSLX_TESTINSTREAM_EXCEPTION_TEST_BEGIN(in) {
                            in.reset();
                            In& rvIn = bdexStreamIn(in, mT, VERSION);
                            LOOP2_ASSERT(i, j, &in == &rvIn);
                        } BSLX_TESTINSTREAM_EXCEPTION_TEST_END

                        ASSERTV(i, j, X, T, X == T);
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
                const int         LOD = static_cast<int>(out.length());
                ASSERT(0 == LOD);

                for (int i = 0; SPECS[i]; ++i) {
                    In in(OD, LOD);
                    LOOP_ASSERT(i, in);
                    LOOP_ASSERT(i, in.isEmpty());

                    // Ensure that reading from an empty or invalid input
                    // stream leaves the stream invalid and the target object
                    // unchanged.

                    using bslx::InStreamFunctions::bdexStreamIn;

                    Obj mX;  const Obj& X = gg(&mX, SPECS[i]);

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
                const int         LOD = static_cast<int>(out.length());
                ASSERT(0 < LOD);

                for (int i = 0; SPECS[i]; ++i) {
                    In in(OD, LOD);
                    in.invalidate();
                    LOOP_ASSERT(i, !in);
                    LOOP_ASSERT(i, !in.isEmpty());

                    // Ensure that reading from a non-empty, invalid input
                    // stream leaves the stream invalid and the target object
                    // unchanged.

                    using bslx::InStreamFunctions::bdexStreamIn;

                    Obj mX;  const Obj& X = gg(&mX, SPECS[i]);

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
                const int         LOD1 = static_cast<int>(out.length());

                Out& rvOut2 = bdexStreamOut(out, X2, VERSION);
                ASSERT(&out == &rvOut2);
                const int         LOD2 = static_cast<int>(out.length());

                Out& rvOut3 = bdexStreamOut(out, X3, VERSION);
                ASSERT(&out == &rvOut3);
                const int         LOD3 = static_cast<int>(out.length());
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
                    } BSLX_TESTINSTREAM_EXCEPTION_TEST_END
                }
            }
        }

        if (verbose) {
             cout << "\nWire format direct tests." << endl;
        }
        {
            static const struct {
                int         d_lineNum;  // source line number
                const char *d_spec_p;   // specification
                int         d_version;  // version to stream with
                int         d_length;   // expect output length
                const char *d_fmt_p;    // expected output format
            } DATA[] = {
                //LINE      SPEC                              V  LEN  FORMAT
                //----  ------------------------------------  -  ---  ------
                { L_,   "",                                   1,  9,
                                      "\x37\xb9\xdd\x00\x00\x01\x00\x00\x00" },
                { L_,   "",                                   2,  9,
                                      "\x37\xb9\xdd\x00\x00\x01\x00\x00\x00" },
                { L_,   "",                                   3, 13,
                      "\x37\xb9\xdd\x00\x00\x01\x00\x01\x00\x01\x00\x01\x00" },

                { L_,   "@2000/1/1",                          1,  9,
                                      "\x0b\x24\x0a\x0b\x24\x0a\x00\x00\x00" },
                { L_,   "@2000/1/1",                          2,  9,
                                      "\x0b\x24\x0a\x0b\x24\x0a\x00\x00\x00" },
                { L_,   "@2000/1/1",                          3, 13,
                      "\x0b\x24\x0a\x0b\x24\x0a\x00\x01\x00\x01\x00\x01\x00" },

                { L_,   "@2000/1/1 32",                       1,  9,
                                      "\x0b\x24\x0a\x0b\x24\x2a\x00\x00\x00" },
                { L_,   "@2000/1/1 32",                       2,  9,
                                      "\x0b\x24\x0a\x0b\x24\x2a\x00\x00\x00" },
                { L_,   "@2000/1/1 32",                       3, 13,
                      "\x0b\x24\x0a\x0b\x24\x2a\x00\x01\x00\x01\x00\x01\x00" },

                { L_,   "@2000/1/1 0 0",                      1, 17,
      "\x0b\x24\x0a\x0b\x24\x0a\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00" },
                { L_,   "@2000/1/1 0 0",                      2, 17,
      "\x0b\x24\x0a\x0b\x24\x0a\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00" },
                { L_,   "@2000/1/1 0 0",                      3, 15,
              "\x0b\x24\x0a\x0b\x24\x0a\x00\x01\x01\x00\x01\x01\x00\x01\x00" },

                { L_,   "@2000/1/1 32 10",                    1, 17,
      "\x0b\x24\x0a\x0b\x24\x2a\x00\x01\x00\x00\x00\x00\x0a\x00\x00\x00\x00" },
                { L_,   "@2000/1/1 32 10",                    2, 17,
      "\x0b\x24\x0a\x0b\x24\x2a\x00\x01\x00\x00\x00\x00\x0a\x00\x00\x00\x00" },
                { L_,   "@2000/1/1 32 10",                    3, 15,
              "\x0b\x24\x0a\x0b\x24\x2a\x00\x01\x01\x0a\x01\x01\x00\x01\x00" },

                { L_,   "@2000/1/1 32 10AB",                  1, 25,
                          "\x0b\x24\x0a\x0b\x24\x2a"
                          "\x00\x01\x02\x00\x00\x00\x0a"
                          "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,   "@2000/1/1 32 10AB",                  2, 25,
                          "\x0b\x24\x0a\x0b\x24\x2a"
                          "\x00\x01\x02\x00\x00\x00\x0a"
                          "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,   "@2000/1/1 32 10AB",                  3, 17,
      "\x0b\x24\x0a\x0b\x24\x2a\x00\x01\x01\x0a\x01\x01\x00\x01\x02\x00\x01" },

                { L_,   "@2000/1/1ua",                        1,  9,
                                      "\x0b\x24\x0a\x0b\x24\x0a\x82\x00\x00" },
                { L_,   "@2000/1/1ua",                        2, 13,
                      "\x0b\x24\x0a\x0b\x24\x0a\x01\x00\x00\x00\x00\x01\x82" },
                { L_,   "@2000/1/1ua",                        3, 17,
      "\x0b\x24\x0a\x0b\x24\x0a\x01\x00\x00\x01\x82\x01\x00\x01\x00\x01\x00" },

                { L_,   "@2000/1/1ua 32 10AB",                1, 25,
                          "\x0b"
                          "\x24\x0a\x0b\x24\x2a\x82\x01\x02\x00\x00\x00\x0a"
                          "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,   "@2000/1/1ua 32 10AB",                2, 29,
                         "\x0b\x24\x0a\x0b\x24"
                          "\x2a\x01\x01\x02\x00\x00\x01\x82\x00\x00\x00\x0a"
                          "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,   "@2000/1/1ua 32 10AB",                3, 21,
                          "\x0b\x24\x0a\x0b\x24\x2a\x01\x00\x00"
                          "\x01\x82\x01\x01\x0a\x01\x01\x00\x01\x02\x00\x01" },

                { L_,   "@2000/1/1ua 32 10mwf",               2, 17,
      "\x0b\x24\x0a\x0b\x24\x2a\x02\x00\x00\x00\x00\x01\x82\x0b\x24\x14\x54" },
                { L_,   "@2000/1/1ua 32 10mwf",               3, 21,
                          "\x0b\x24\x0a\x0b\x24\x2a\x02\x00\x00"
                          "\x01\x82\x0b\x24\x14\x54\x01\x00\x01\x00\x01\x00" },

                { L_,   "@2000/1/1ua 32 10mwf 15AB",          2, 33,
                          "\x0b\x24\x0a\x0b\x24\x2a\x02\x01\x02"
                          "\x00\x00\x01\x82\x0b\x24\x14\x54\x00\x00\x00\x0f"
                          "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,   "@2000/1/1ua 32 10mwf 15AB", 3, 25,
                      "\x0b\x24\x0a\x0b\x24\x2a\x02\x00\x00\x01\x82\x0b\x24"
                          "\x14\x54\x01\x01\x0f\x01\x01\x00\x01\x02\x00\x01" },

                { L_,   "@2000/1/1 32 1ABC 2 3CDE 4ABCDE 5C", 1, 97,
                      "\x0b\x24\x0a\x0b\x24\x2a"
                      "\x00\x05\x0c\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00"
                      "\x00\x03\x00\x00\x00\x04\x00\x00\x00\x05\x00\x00\x00"
                      "\x00\x00\x00\x00\x03\x00\x00\x00\x03\x00\x00\x00\x06"
                      "\x00\x00\x00\x0b\x00\x00\x00\x00\x00\x00\x00\x01\x00"
                      "\x00\x00\x02\x00\x00\x00\x02\x00\x00\x00\x64\x00\x00"
                      "\x03\xe8\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00"
                      "\x02\x00\x00\x00\x64\x00\x00\x03\xe8\x00\x00\x00\x02" },
                { L_,   "@2000/1/1 32 1ABC 2 3CDE 4ABCDE 5C", 2, 97,
                      "\x0b\x24\x0a\x0b\x24\x2a"
                      "\x00\x05\x0c\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00"
                      "\x00\x03\x00\x00\x00\x04\x00\x00\x00\x05\x00\x00\x00"
                      "\x00\x00\x00\x00\x03\x00\x00\x00\x03\x00\x00\x00\x06"
                      "\x00\x00\x00\x0b\x00\x00\x00\x00\x00\x00\x00\x01\x00"
                      "\x00\x00\x02\x00\x00\x00\x02\x00\x00\x00\x64\x00\x00"
                      "\x03\xe8\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00"
                      "\x02\x00\x00\x00\x64\x00\x00\x03\xe8\x00\x00\x00\x02" },
                { L_,   "@2000/1/1 32 1ABC 2 3CDE 4ABCDE 5C", 3, 47,
                      "\x0b\x24\x0a\x0b\x24\x2a\x00\x01"
                      "\x05\x01\x02\x03\x04\x05\x01\x05\x00\x03\x03\x06\x0b"
                      "\x02\x0c\x00\x00\x00\x01\x00\x02\x00\x02\x00\x64\x03"
                      "\xe8\x00\x00\x00\x01\x00\x02\x00\x64\x03\xe8\x00\x02" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const char *const SPEC    = DATA[i].d_spec_p;
                const int         VERSION = DATA[i].d_version;
                const int         LEN     = DATA[i].d_length;
                const char *const FMT     = DATA[i].d_fmt_p;

                // Test using class methods.

                {
                    Obj mX;  const Obj& X = gg(&mX, SPEC);

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = X.bdexStreamOut(out, VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    ASSERTV(LINE,
                            LEN,
                            out.length(),
                            LEN == static_cast<int>(out.length()));
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0; j < static_cast<int>(out.length());
                                                                         ++j) {
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
                    ASSERTV(LINE, X, Y, X == Y);
                }

                // Test using free functions.

                {
                    Obj mX;  const Obj& X = gg(&mX, SPEC);

                    using bslx::OutStreamFunctions::bdexStreamOut;

                    bslx::ByteOutStream  out(VERSION_SELECTOR, &allocator);
                    bslx::ByteOutStream& rvOut = bdexStreamOut(out,
                                                               X,
                                                               VERSION);
                    LOOP_ASSERT(LINE, &out == &rvOut);

                    LOOP_ASSERT(LINE, LEN == static_cast<int>(out.length()));
                    LOOP_ASSERT(LINE, 0 == memcmp(out.data(), FMT, LEN));

                    if (verbose && memcmp(out.data(), FMT, LEN)) {
                        const char *hex = "0123456789abcdef";
                        P_(LINE);
                        for (int j = 0; j < static_cast<int>(out.length());
                                                                         ++j) {
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
                    ASSERTV(LINE, X, Y, X == Y);
                }
            }
        }

        if (verbose) {
            cout << "\nTesting with corrupted data." << endl;
        }

        const Obj W;                                        // default value

        Obj mX;  const Obj& X = gg(&mX, DEFAULT_SPECS[2]);  // control

        Obj mY;  const Obj& Y = gg(&mY, DEFAULT_SPECS[1]);  // streamed-out

        // Verify the three objects are distinct.
        ASSERT(W != X);
        ASSERT(W != Y);
        ASSERT(X != Y);

        if (verbose) {
            cout << "\tBad version." << endl;
        }
        {
            const char version = 0; // too small ('version' must be >= 1)

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            Y.firstDate().bdexStreamOut(out, 1);
            Y.lastDate().bdexStreamOut(out, 1);
            out.putLength(0);
            bdlc::PackedIntArray<int>().bdexStreamOut(out, 1);
            bdlc::PackedIntArray<int>().bdexStreamOut(out, 1);
            bdlc::PackedIntArray<int>().bdexStreamOut(out, 1);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

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

            using bslx::OutStreamFunctions::bdexStreamOut;

            Out& rvOut = bdexStreamOut(out, X, version);
            ASSERT(&out == &rvOut);
            ASSERT(!out);
        }
        {
            const char version = 4; // too large

            Out out(VERSION_SELECTOR, &allocator);

            // Stream out "new" value.
            Y.firstDate().bdexStreamOut(out, 1);
            Y.lastDate().bdexStreamOut(out, 1);
            out.putLength(0);
            bdlc::PackedIntArray<int>().bdexStreamOut(out, 1);
            bdlc::PackedIntArray<int>().bdexStreamOut(out, 1);
            bdlc::PackedIntArray<int>().bdexStreamOut(out, 1);

            const char *const OD  = out.data();
            const int         LOD = static_cast<int>(out.length());

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

            using bslx::OutStreamFunctions::bdexStreamOut;

            Out& rvOut = bdexStreamOut(out, X, version);
            ASSERT(&out == &rvOut);
            ASSERT(!out);
        }

        if (verbose) {
             cout << "\tValid (for control) and Invalid data." << endl;
        }
        {
            static const struct {
                int         d_lineNum;  // source line number
                bool        d_valid;    // valid flag
                int         d_version;  // version to stream with
                int         d_length;   // specification length
                const char *d_spec_p;   // specification
            } DATA[] = {
                //LINE  VALID  V  LEN       SPEC
                //----  -----  -  ---  ---------------

                // control

                { L_,   true,  1,  33,
                          "\x0b\x24\x0a\x0b\x24\x2a\x82\x02\x02"
                          "\x00\x00\x00\x09\x00\x00\x00\x0a\x00\x00\x00\x00"
                          "\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,   true,  2,  41,
                          "\x0b\x24\x0a\x0b\x24"
                          "\x2a\x02\x02\x02\x00\x00\x01\x82\x0b\x24\x14\x54"
                          "\x00\x00\x00\x09\x00\x00\x00\x0f\x00\x00\x00\x00"
                          "\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,   true,  3,  27,
                          "\x0b\x24\x0a"
                          "\x0b\x24\x2a\x02\x00\x00\x01\x82\x0b\x24\x14\x54"
                          "\x01\x02\x09\x0f\x01\x02\x00\x02\x01\x02\x00\x01" },

                // 'lastDate > firstDate' with invalid values

                { L_,  false,  1,  33,
                          "\x0b\x24\x2a\x0b\x24\x0a\x82\x02\x02"
                          "\x00\x00\x00\x09\x00\x00\x00\x0a\x00\x00\x00\x00"
                          "\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,  false,  2,  41,
                          "\x0b\x24\x2a\x0b\x24"
                          "\x0a\x02\x02\x02\x00\x00\x01\x82\x0b\x24\x14\x54"
                          "\x00\x00\x00\x09\x00\x00\x00\x0f\x00\x00\x00\x00"
                          "\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,  false,  3,  27,
                          "\x0b\x24\x2a"
                          "\x0b\x24\x0a\x02\x00\x00\x01\x82\x0b\x24\x14\x54"
                          "\x01\x02\x09\x0f\x01\x02\x00\x02\x01\x02\x00\x01" },

                // invalid 'DayOfWeekSet'

                { L_,  false,  1,   9,
                                      "\x0b\x24\x0a\x0b\x24\x2a\xff\x00\x00" },

                // non-zero 'offsetsLength' for empty calendar

                { L_,  false,  1,   9,
                                      "\x37\xb9\xdd\x00\x00\x01\x00\x01\x00" },
                { L_,  false,  2,   9,
                                      "\x37\xb9\xdd\x00\x00\x01\x00\x01\x00" },
                { L_,  false,  3,  14,
                  "\x37\xb9\xdd\x00\x00\x01\x00\x01\x01\x00\x01\x00\x01\x00" },

                // '0 == offsetsLength && codesLength != 0'

                { L_,  false,  1,   9,
                                      "\x37\xb9\xdd\x00\x00\x01\x00\x00\x01" },
                { L_,  false,  2,   9,
                                      "\x37\xb9\xdd\x00\x00\x01\x00\x00\x01" },
                { L_,  false,  3,  14,
                  "\x37\xb9\xdd\x00\x00\x01\x00\x01\x00\x01\x00\x01\x01\x00" },

                // weekend-days transitions not monotonically increasing

                { L_,  false,  2,  41,
                          "\x0b\x24\x0a\x0b\x24"
                          "\x2a\x02\x02\x02\x0b\x24\x14\x54\x00\x00\x01\x82"
                          "\x00\x00\x00\x09\x00\x00\x00\x0f\x00\x00\x00\x00"
                          "\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,  false,  3,  27,
                          "\x0b\x24\x0a"
                          "\x0b\x24\x2a\x02\x0b\x24\x14\x54\x00\x00\x01\x82"
                          "\x01\x02\x09\x0f\x01\x02\x00\x02\x01\x02\x00\x01" },

                // holiday offsets not monotonically increasing

                { L_,  false,  1,  33,
                          "\x0b\x24\x0a\x0b\x24\x2a\x82\x02\x02"
                          "\x00\x00\x00\x0a\x00\x00\x00\x0a\x00\x00\x00\x00"
                          "\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,  false,  2,  41,
                          "\x0b\x24\x0a\x0b\x24"
                          "\x2a\x02\x02\x02\x00\x00\x01\x82\x0b\x24\x14\x54"
                          "\x00\x00\x00\x0f\x00\x00\x00\x0f\x00\x00\x00\x00"
                          "\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,  false,  3,  27,
                          "\x0b\x24\x0a"
                          "\x0b\x24\x2a\x02\x00\x00\x01\x82\x0b\x24\x14\x54"
                          "\x01\x02\x0f\x0f\x01\x02\x00\x02\x01\x02\x00\x01" },

                // holiday codes index not monotonically non-decreasing

                { L_,  false,  1,  41,
                          "\x0b\x24\x0a\x0b\x24"
                          "\x2a\x82\x02\x03\x00\x00\x00\x09\x00\x00\x00\x0a"
                          "\x00\x00\x00\x0f\x00\x00\x00\x00\x00\x00\x00\x02"
                          "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,  false,  2,  49,
                          "\x0b"
                          "\x24\x0a\x0b\x24\x2a\x02\x03\x02\x00\x00\x01\x82"
                          "\x0b\x24\x14\x54\x00\x00\x00\x09\x00\x00\x00\x0a"
                          "\x00\x00\x00\x0f\x00\x00\x00\x00\x00\x00\x00\x02"
                          "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01" },
                { L_,  false,  3,  29,
                          "\x0b\x24\x0a\x0b\x24"
                          "\x2a\x02\x00\x00\x01\x82\x0b\x24\x14\x54\x01\x03"
                          "\x09\x0a\x0f\x01\x03\x00\x02\x00\x01\x02\x00\x01" },

                // holiday codes segments not monotonically increasing

                { L_,  false,  1,  33,
                          "\x0b\x24\x0a\x0b\x24\x2a\x82\x02\x02"
                          "\x00\x00\x00\x09\x00\x00\x00\x0a\x00\x00\x00\x00"
                          "\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00" },
                { L_,  false,  2,  41,
                          "\x0b\x24\x0a\x0b\x24"
                          "\x2a\x02\x02\x02\x00\x00\x01\x82\x0b\x24\x14\x54"
                          "\x00\x00\x00\x09\x00\x00\x00\x0f\x00\x00\x00\x00"
                          "\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00" },
                { L_,  false,  3,  27,
                          "\x0b\x24\x0a"
                          "\x0b\x24\x2a\x02\x00\x00\x01\x82\x0b\x24\x14\x54"
                          "\x01\x02\x09\x0f\x01\x02\x00\x02\x01\x02\x00\x00" },
            };
            const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

            for (int i = 0; i < NUM_DATA; ++i) {
                const int         LINE    = DATA[i].d_lineNum;
                const bool        VALID   = DATA[i].d_valid;
                const int         VERSION = DATA[i].d_version;
                const int         LEN     = DATA[i].d_length;
                const char *const SPEC    = DATA[i].d_spec_p;

                Obj mT(X);  const Obj& T = mT;
                LOOP_ASSERT(LINE, X == T);

                bslx::ByteInStream in(SPEC, LEN);
                LOOP_ASSERT(LINE, in);

                using bslx::InStreamFunctions::bdexStreamIn;

                bslx::ByteInStream& rvIn = bdexStreamIn(in, mT, VERSION);
                LOOP_ASSERT(LINE, &in == &rvIn);

                if (VALID) {
                    LOOP_ASSERT(LINE, in);
                }
                else {
                    LOOP_ASSERT(LINE, !in);
                    ASSERTV(LINE, X, T, X == T);
                }
            }
        }
      } break;
      case 9: {
        // --------------------------------------------------------------------
        // COPY-ASSIGNMENT OPERATOR
        //   Ensure that we can assign the value of any object of the class to
        //   any object of the class, such that the two objects subsequently
        //   have the same value.
        //
        // Concerns:
        //: 1 The assignment operator can change the value of any target object
        //:   to that of any source object.
        //:
        //: 2 The address of the target object's allocator is unchanged.
        //:
        //: 3 Any memory allocation is from the target object's allocator.
        //:
        //: 4 The signature and return type are standard.
        //:
        //: 5 The reference returned is to the target object (i.e., '*this').
        //:
        //: 6 The value of the source object is not modified.
        //:
        //: 7 The allocator of the source object is not modified.
        //:
        //: 8 Any memory allocation is exception neutral.
        //:
        //: 9 Assigning an object to itself behaves as expected (alias-safety).
        //
        // Plan:
        //: 1 Use the address of 'operator=' to initialize a function pointer
        //:   having the appropriate structure for the copy-assignment operator
        //:   defined in this component.  (C-4)
        //:
        //: 2 Construct a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of valid object values (one per row) in terms of
        //:     their individual attributes, including (a) first, the default
        //:     value, (b) boundary values corresponding to every range of
        //:     values that each individual attribute can independently attain,
        //:     and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 4 For each row 'R1' (representing a distinct object value, 'V') in
        //:   the table described in P-3: (C-1..3, 5..9)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop that iterates over each row 'R2'
        //:     (representing a distinct object value, 'W') in the table
        //:     described in P-3:
        //:
        //:   3 For each of the iterations (P-4.2):
        //:
        //:     1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:     2 Use the value constructor to create a modifiable 'Obj', 'mX',
        //:       using 'oa' and having the value 'W'.
        //:
        //:     3 Use the equality comparison operators to verify that the two
        //:       objects, 'mX' and 'Z', are initially equal if and only if the
        //:       table indices from P-4 and P-4.2 are the same.
        //:
        //:     4 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:       (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:
        //:     5 Verify that the address of the return value is the same as
        //:       that of 'mX'.  (C-5)
        //:
        //:     6 Use the equality comparison operators to verify that: (C-1,
        //:       6)
        //:
        //:       1 The target object, 'mX', now has the same value as 'Z'.
        //:
        //:       2 'Z' still has the same value as 'ZZ'.
        //:
        //:     7 Use the 'allocator' accessor of both 'mX' and 'Z' to verify
        //:       that both object allocators are unchanged.  (C-2, 7)
        //:
        //:     8 Use appropriate test allocators to verify that: (C-3, 8)
        //:
        //:       1 No additional memory is allocated by the source object.
        //:
        //:       2 All object memory is released when the object is destroyed.
        //:
        //: 5 Repeat steps similar to those described in P-2 except that, this
        //:   time, there is no inner loop (as in P-4.2); instead, the source
        //:   object, 'Z', is a reference to the target object, 'mX', and both
        //:   'mX' and 'ZZ' are initialized to have the value 'V'.  For each
        //:   row (representing a distinct object value, 'V') in the table
        //:   described in P-3: (C-1..3, 5..10)
        //:
        //:   1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj' 'mX'
        //:     using 'oa' and a 'const' 'Obj' 'ZZ' (using a distinct "scratch"
        //:     allocator).
        //:
        //:   3 Let 'Z' be a reference providing only 'const' access to 'mX'.
        //:
        //:   4 Use the equality comparison operators to verify that the target
        //:     object, 'mX', has the same value as 'ZZ'.
        //:
        //:   5 Assign 'mX' from 'Z' in the presence of injected exceptions
        //:     (using the 'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros).
        //:     (C-9)
        //:
        //:   6 Verify that the address of the return value is the same as that
        //:     of 'mX'.  (C-5)
        //:
        //:   7 Use the equality comparison operators to verify that the target
        //:     object, 'mX', still has the same value as 'ZZ'.  (C-10)
        //:
        //:   8 Use the 'allocator' accessor of 'mX' to verify that it is still
        //:     the object allocator.  (C-2)
        //:
        //:   9 Use appropriate test allocators to verify that: (C-3, 8)
        //:
        //:     1 Any memory that is allocated is from the object allocator.
        //:
        //:     2 No additional (e.g., temporary) object memory is allocated
        //:       when assigning an object value that did NOT initially require
        //:       allocated memory.
        //:
        //:     3 All object memory is released when the object is destroyed.
        //:
        //: 6 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //
        // Testing:
        //   PackedCalendar& operator=(const PackedCalendar& rhs);
        // --------------------------------------------------------------------


        if (verbose) cout << endl
                          << "COPY-ASSIGNMENT OPERATOR" << endl
                          << "========================" << endl;

        if (verbose) cout <<
                 "\nAssign the address of the operator to a variable." << endl;
        {
            typedef Obj& (Obj::*operatorPtr)(const Obj&);

            // Verify that the signature and return type are standard.

            operatorPtr operatorAssignment = &Obj::operator=;

            // Quash unused variable warning.

            Obj a;
            Obj b;
            (a.*operatorAssignment)(b);
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout <<
        "\nCreate a table of distinct object values and expected memory usage."
                                                                       << endl;

        static const char **SPECS = DEFAULT_SPECS;

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC1 = SPECS[ti];

            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj mZ(&scratch);   const Obj& Z = gg(&mZ,  SPEC1);

            Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC1);

            if (veryVerbose) { T_ P_(ti) P_(Z) P_(ZZ) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(ti, Z, Obj(&scratch), Z == Obj(&scratch));
                firstFlag = false;
            }

            for (int tj = 0; SPECS[tj]; ++tj) {
                const char *const SPEC2 = SPECS[tj];

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                {
                    Obj mX(&oa);  const Obj& X = gg(&mX, SPEC2);

                    if (veryVerbose) { T_ P_(tj) P_(X) }

                    LOOP4_ASSERT(ti, tj, X, Z,
                                 (X == Z) == (ti == tj));

                    bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                    Obj *mR = 0;
                    BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                        if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                        mR = &(mX = Z);
                    } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                    // Verify the address of the return value.

                    LOOP4_ASSERT(ti, tj, mR, &mX, mR == &mX);

                    LOOP4_ASSERT(ti, tj,  X, Z,  X == Z);
                    LOOP4_ASSERT(ti, tj, ZZ, Z, ZZ == Z);

                    LOOP4_ASSERT(ti, tj, &oa, X.allocator(),
                                 &oa == X.allocator());
                    LOOP4_ASSERT(ti, tj, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    LOOP2_ASSERT(ti, tj, sam.isInUseSame());

                    LOOP2_ASSERT(ti, tj, !da.numBlocksTotal());
                }

                // Verify all memory is released on object destruction.

                LOOP3_ASSERT(ti, tj, da.numBlocksInUse(),
                             0 == da.numBlocksInUse());
                LOOP3_ASSERT(ti, tj, oa.numBlocksInUse(),
                             0 == oa.numBlocksInUse());
            }

            // self-assignment

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            {
                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mX(&oa);   const Obj& X = gg(&mX, SPEC1);

                Obj mZZ(&oa);  const Obj& ZZ = gg(&mZZ, SPEC1);

                const Obj& Z = mX;

                LOOP3_ASSERT(ti, ZZ, Z, ZZ == Z);

                bslma::TestAllocatorMonitor oam(&oa), sam(&scratch);

                Obj *mR = 0;
                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    mR = &(mX = Z);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                // Verify the address of the return value.

                LOOP3_ASSERT(ti, mR, &mX, mR == &mX);

                LOOP3_ASSERT(ti, mR, &X, mR == &X);

                LOOP3_ASSERT(ti, Z, ZZ, ZZ == Z);

                LOOP3_ASSERT(ti, &oa, Z.allocator(), &oa == Z.allocator());

                if (0 == ti) {  // Empty, no allocation.
                    LOOP_ASSERT(ti, oam.isInUseSame());
                }

                LOOP_ASSERT(ti, sam.isInUseSame());

                LOOP_ASSERT(ti, !da.numBlocksTotal());
            }

            // Verify all memory is released on object destruction.

            LOOP2_ASSERT(ti, oa.numBlocksInUse(), 0 == oa.numBlocksInUse());
            LOOP2_ASSERT(ti, da.numBlocksInUse(), 0 == da.numBlocksInUse());
        }
      } break;
      case 8: {
        // --------------------------------------------------------------------
        // SWAP MEMBER AND FREE FUNCTION
        //   Ensure that, when member and free 'swap' are implemented, we can
        //   exchange the values of any two objects that use the same
        //   allocator.
        //
        // Concerns:
        //: 1 Both functions exchange the values of the (two) supplied objects.
        //:
        //: 2 The common object allocator address in both objects is unchanged.
        //:
        //: 3 Neither function allocates memory from any allocator.
        //:
        //: 4 Both functions have standard signatures.
        //:
        //: 5 Using either function to swap an object with itself does not
        //:   affect the value of the object (alias-safety).
        //:
        //: 6 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Use the addresses of the 'swap' member and free functions defined
        //:   in this component to initialize, respectively, pointers to member
        //:   and free functions having the appropriate structures.  (C-4)
        //:
        //: 2 Construct a 'bslma::TestAllocator' object and install it as the
        //:   default allocator (note that a ubiquitous test allocator is
        //:   already installed as the global allocator).
        //:
        //: 3 Using the table-driven technique:
        //:
        //:   1 Specify a set of widely varying object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every range
        //:     of values that each individual attribute can independently
        //:     attain, and (c) values that should require allocation from each
        //:     individual attribute that can independently allocate memory.
        //:
        //: 4 For each row 'R1' in the table of P-3: (C-1..3, 5)
        //:
        //:   1 Construct a 'bslma::TestAllocator' object, 'oa'.
        //:
        //:   2 Use the value constructor to create a modifiable 'Obj', 'mW',
        //:     using 'oa' and having the value described by 'R1'; also use the
        //:     copy constructor to create a 'const' 'Obj' 'XX' (using a
        //:     "scratch" allocator) from 'mW'.
        //:
        //:   3 Use the member and free 'swap' functions to swap the value of
        //:     'mW' with itself, and then verify: (C-2, 3, 5)
        //:
        //:     1 The value is unchanged.
        //:
        //:     2 The object allocator address is unchanged.
        //:
        //:     3 There was no additional object memory allocation.
        //:
        //:   4 For each row 'R2' in the table of P-3:
        //:
        //:     1 Use the copy constructor to create a modifiable 'Obj', 'mX',
        //:       (using 'oa') from 'XX' (P-4.2).
        //:
        //:     2 Use the value constructor to create a modifiable 'Obj', 'mY',
        //:       using 'oa' and having the value described by 'R2'; also use
        //:       the copy constructor to create a 'const' 'Obj' 'YY' (using a
        //:       "scratch" allocator) from 'Y'.
        //:
        //:     3 Use the member 'swap' function to swap the values of 'mX' and
        //:       'mY', and then verify: (C-1..3)
        //:
        //:       1 The values have been exchanged.
        //:
        //:       2 The object allocator addresses are unchanged.
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //:     4 Use the free 'swap' function to again swap the values of 'mX'
        //:       and 'mY', and then verify: (C-1..3)
        //:
        //:       1 The values have been exchanged.
        //:
        //:       2 The object allocator addresses are unchanged.
        //:
        //:       3 There was no additional object memory allocation.
        //:
        //: 5 Use the test allocator from P-2 to verify that no memory is ever
        //:   allocated from the default allocator.  (C-3)
        //:
        //: 6 Verify that, in appropriate build modes, defensive checks are
        //:   triggered when an attempt is made to swap objects that do not
        //:   refer to the same allocator, but not when the allocators are the
        //:   same (using the 'BSLS_ASSERTTEST_*' macros).  (C-6)
        //
        // Testing:
        //   void swap(PackedCalendar& other);
        //   void swap(PackedCalendar& a, PackedCalendar& b);
        // --------------------------------------------------------------------


        if (verbose) cout << endl
                          << "SWAP MEMBER AND FREE FUNCTION" << endl
                          << "=============================" << endl;

        if (verbose) cout <<
                "\nAssign the address of each function to a variable." << endl;
        {
            using namespace bdlc;

            typedef void (Obj::*funcPtr)(Obj&);

            typedef void (*freeFuncPtr)(Obj&, Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     memberSwap = &Obj::swap;
            freeFuncPtr freeSwap   = swap;

            // Quash unused variable warnings.

            Obj a;
            Obj b;
            (a.*memberSwap)(b);
            freeSwap(a, b);
        }

        if (verbose) cout <<
            "\nCreate a test allocator and install it as the default." << endl;

        bslma::TestAllocator da("default", veryVeryVeryVerbose);
        bslma::Default::setDefaultAllocatorRaw(&da);

        if (verbose) cout << "\nCreate a table of distinct object values."
                                                                       << endl;

        const char **SPECS = DEFAULT_SPECS;

        for (int ti = 0; SPECS[ti]; ++ti) {
            const char *const SPEC1 = SPECS[ti];

            bslma::TestAllocator oa("object",       veryVeryVeryVerbose);
            bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

            Obj        mW(&oa);
            const Obj& W = gg(&mW, SPEC1);
            const Obj  XX(W, &scratch);

            if (veryVerbose) { T_ P_(ti) P_(W) P_(XX) }

            // Ensure the first row of the table contains the
            // default-constructed value.

            static bool firstFlag = true;
            if (firstFlag) {
                LOOP3_ASSERT(ti, W, Obj(&scratch), W == Obj(&scratch));
                firstFlag = false;
            }

            // member 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                mW.swap(mW);

                LOOP3_ASSERT(ti, W, XX, W == XX);

                LOOP_ASSERT(ti, &oa == W.allocator());
                LOOP_ASSERT(ti, oam.isTotalSame());
            }

            // free function 'swap'
            {
                bslma::TestAllocatorMonitor oam(&oa);

                swap(mW, mW);

                LOOP3_ASSERT(ti, W, XX, W == XX);

                LOOP_ASSERT(ti, &oa == W.allocator());
                LOOP_ASSERT(ti, oam.isTotalSame());
            }

            for (int tj = 0; SPECS[tj]; ++tj) {
                const char *const SPEC2 = SPECS[tj];

                Obj        mX(XX, &oa);
                const Obj& X = mX;

                Obj        mY(&oa);
                const Obj& Y = gg(&mY, SPEC2);
                const Obj  YY(Y, &scratch);

                if (veryVerbose) { T_ P_(tj) P_(X) P_(Y) P_(YY) }

                // member 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    mX.swap(mY);

                    LOOP4_ASSERT(ti, tj, X, YY, X == YY);
                    LOOP4_ASSERT(ti, tj, Y, XX, Y == XX);
                    LOOP2_ASSERT(ti, tj, &oa == X.allocator());
                    LOOP2_ASSERT(ti, tj, &oa == Y.allocator());
                    LOOP2_ASSERT(ti, tj, oam.isTotalSame());
                }

                // free function 'swap'
                {
                    bslma::TestAllocatorMonitor oam(&oa);

                    swap(mX, mY);

                    LOOP4_ASSERT(ti, tj, X, XX, X == XX);
                    LOOP4_ASSERT(ti, tj, Y, YY, Y == YY);
                    LOOP2_ASSERT(ti, tj, &oa == X.allocator());
                    LOOP2_ASSERT(ti, tj, &oa == Y.allocator());
                    LOOP2_ASSERT(ti, tj, oam.isTotalSame());
                }
            }
        }

        // Verify no memory is allocated from the default allocator.

        LOOP_ASSERT(da.numBlocksTotal(), !da.numBlocksTotal());

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            if (veryVerbose) cout << "\t'swap' member function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);
                Obj mB(&oa1);
                Obj mC(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(mA.swap(mB));
                ASSERT_SAFE_FAIL(mC.swap(mZ));
            }

            if (veryVerbose) cout << "\t'swap' free function" << endl;
            {
                bslma::TestAllocator oa1("object1", veryVeryVeryVerbose);
                bslma::TestAllocator oa2("object2", veryVeryVeryVerbose);

                Obj mA(&oa1);
                Obj mB(&oa1);
                Obj mC(&oa1);
                Obj mZ(&oa2);

                ASSERT_SAFE_PASS(swap(mA, mB));
                ASSERT_SAFE_FAIL(swap(mC, mZ));
            }
        }
      } break;
      case 7: {
        // -------------------------------------------------------------------
        // COPY CONSTRUCTOR
        //   Ensure that we can create a distinct object of the class from any
        //   other one, such that the two objects have the same value.
        //
        // Concerns:
        //: 1 The copy constructor (with or without a supplied allocator)
        //:   creates an object having the same value as the supplied original
        //:   object.
        //:
        //: 2 If an allocator is *not* supplied to the copy constructor, the
        //:   default allocator in effect at the time of construction becomes
        //:   the object allocator for the resulting object (i.e., the
        //:   allocator of the original object is never copied).
        //:
        //: 3 If an allocator is supplied to the copy constructor, that
        //:   allocator becomes the object allocator for the resulting object.
        //:
        //: 4 Supplying a null allocator address has the same effect as not
        //:   supplying an allocator.
        //:
        //: 5 Supplying an allocator to the copy constructor has no effect on
        //:   subsequent object values.
        //:
        //: 6 Any memory allocation is from the object allocator, which is
        //:   returned by the 'allocator' accessor method.
        //:
        //: 7 There is no temporary memory allocation from any allocator.
        //:
        //: 8 Every object releases any allocated memory at destruction.
        //:
        //: 9 The original object is passed as a reference providing
        //:   non-modifiable access to the object.
        //:
        //:10 The value of the original object is unchanged.
        //:
        //:11 The allocator address of the original object is unchanged.
        //:
        //:12 Any memory allocation is exception neutral.
        //
        // Plan:
        //: 1 Using the table-driven technique:
        //:
        //:   1 Specify a set of widely varying object values (one per row) in
        //:     terms of their individual attributes, including (a) first, the
        //:     default value, (b) boundary values corresponding to every
        //:     range of values that each individual attribute can
        //:     independently attain, and (c) values that should require
        //:     allocation from each individual attribute that can
        //:     independently allocate memory.
        //:
        //: 2 For each row (representing a distinct object value, 'V') in the
        //:   table described in P-1: (C-1..12)
        //:
        //:   1 Use the value constructor to create two 'const' 'Obj', 'Z' and
        //:     'ZZ', each having the value 'V' and using a "scratch"
        //:     allocator.
        //:
        //:   2 Execute an inner loop creating three distinct objects in turn,
        //:     each using the copy constructor on 'Z' from P-2.1, but
        //:     configured differently: (a) without passing an allocator, (b)
        //:     passing a null allocator address explicitly, and (c) passing
        //:     the address of a test allocator distinct from the default.
        //:
        //:   3 For each of these three iterations (P-2.2): (C-1..12)
        //:
        //:     1 Construct three 'bslma::TestAllocator' objects and install
        //:       one as the current default allocator (note that a ubiquitous
        //:       test allocator is already installed as the global
        //:       allocator).
        //:
        //:     2 Use the copy constructor to dynamically create an object
        //:       'X', supplying it the 'const' object 'Z' (see P-2.1),
        //:       configured appropriately (see P-2.2) using a distinct test
        //:       allocator for the object's footprint.  (C-9)
        //:
        //:     3 Use the equality comparison operators to verify that:
        //:
        //:       1 The newly constructed object, 'X', has the same value as
        //:         'Z'.  (C-1, 5)
        //:
        //:       2 'Z' still has the same value as 'ZZ'.  (C-10)
        //:
        //:     4 Use the 'allocator' accessor of each underlying attribute
        //:       capable of allocating memory to ensure that its object
        //:       allocator is properly installed; also apply the object's
        //:       'allocator' accessor, as well as that of 'Z'.  (C-6, 11)
        //:
        //:     5 Use appropriate test allocators to verify that: (C-2..4, 7,
        //:       8, 12)
        //:
        //:       1 Memory is always allocated, and comes from the object
        //:         allocator only (irrespective of the specific number of
        //:         allocations or the total amount of memory allocated).
        //:         (C-2, 4)
        //:
        //:       2 If an allocator was supplied at construction (P-2.1c), the
        //:         current default allocator doesn't allocate any memory.
        //:         (C-3)
        //:
        //:       3 No temporary memory is allocated from the object
        //:         allocator.  (C-7)
        //:
        //:       4 All object memory is released when the object is
        //:         destroyed. (C-8)
        //:
        //: 3 Create an object as an automatic variable in the presence of
        //:   injected exceptions (using the
        //:   'BSLMA_TESTALLOCATOR_EXCEPTION_TEST_*' macros) and verify that
        //:   no memory is leaked.  (C-12)
        //
        // Testing:
        //   PackedCalendar(const PackedCalendar& original, ba = 0);
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "COPY CONSTRUCTOR" << endl
                          << "================" << endl;

        {
            static const char **SPECS = DEFAULT_SPECS;

            bool anyObjectMemoryAllocatedFlag = false;  // We later check that
                                                        // this test allocates
                                                        // some object memory.

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mZ(&scratch);   const Obj& Z = gg(&mZ, SPEC);

                Obj mZZ(&scratch);  const Obj& ZZ = gg(&mZZ, SPEC);

                if (veryVerbose) { T_ P_(Z) P(ZZ) }

                for (char cfg = 'a'; cfg <= 'c'; ++cfg) {

                    const char CONFIG = cfg;  // how we specify the allocator

                    bslma::TestAllocator fa("footprint", veryVeryVeryVerbose);
                    bslma::TestAllocator da("default",   veryVeryVeryVerbose);
                    bslma::TestAllocator sa("supplied",  veryVeryVeryVerbose);

                    bslma::Default::setDefaultAllocatorRaw(&da);

                    Obj                  *objPtr;
                    bslma::TestAllocator *objAllocatorPtr;

                    switch (CONFIG) {
                      case 'a': {
                        objPtr = new (fa) Obj(Z);
                        objAllocatorPtr = &da;
                      } break;
                      case 'b': {
                        objPtr = new (fa) Obj(Z, 0);
                        objAllocatorPtr = &da;
                      } break;
                      case 'c': {
                        objPtr = new (fa) Obj(Z, &sa);
                        objAllocatorPtr = &sa;
                      } break;
                      default: {
                        LOOP_ASSERT(CONFIG, !"Bad allocator config.");
                      } break;
                    }
                    LOOP2_ASSERT(ti, CONFIG,
                                 sizeof(Obj) == fa.numBytesInUse());

                    Obj& mX = *objPtr;  const Obj& X = mX;

                    if (veryVerbose) { T_ T_ P_(CONFIG) P(X) }

                    bslma::TestAllocator&  oa = *objAllocatorPtr;
                    bslma::TestAllocator& noa = 'c' != CONFIG ? sa : da;

                    // Ensure the first row of the table contains the
                    // default-constructed value.

                    static bool firstFlag = true;
                    if (firstFlag) {
                        LOOP4_ASSERT(ti, CONFIG, Obj(), *objPtr,
                                     Obj() == *objPtr)
                        firstFlag = false;
                    }

                    // Verify the value of the object.

                    LOOP4_ASSERT(ti, CONFIG,  X, Z,  X == Z);

                    // Verify that the value of 'Z' has not changed.

                    LOOP4_ASSERT(ti, CONFIG, ZZ, Z, ZZ == Z);

                    // Also apply the object's 'allocator' accessor, as well
                    // as that of 'Z'.

                    LOOP4_ASSERT(ti, CONFIG, &oa, X.allocator(),
                                 &oa == X.allocator());

                    LOOP4_ASSERT(ti, CONFIG, &scratch, Z.allocator(),
                                 &scratch == Z.allocator());

                    // Verify no allocation from the non-object allocator.

                    LOOP3_ASSERT(ti, CONFIG, noa.numBlocksTotal(),
                                 0 == noa.numBlocksTotal());

                    // Verify memory is always allocated except when no
                    // holidays and no weekend transitions are added.

                    if (   0 == X.numHolidays()
                        && 0 == X.numWeekendDaysTransitions()) {
                        LOOP3_ASSERT(ti, CONFIG, oa.numBlocksInUse(),
                                     0 == oa.numBlocksInUse());
                    }
                    else {
                        LOOP3_ASSERT(ti, CONFIG, oa.numBlocksInUse(),
                                     0 != oa.numBlocksInUse());
                    }

                    // Record if some object memory was allocated.

                    anyObjectMemoryAllocatedFlag |= !!oa.numBlocksInUse();

                    // Reclaim dynamically allocated object under test.

                    fa.deleteObject(objPtr);

                    // Verify all memory is released on object destruction.

                    LOOP3_ASSERT(ti, CONFIG, fa.numBlocksInUse(),
                                 0 == fa.numBlocksInUse());
                    LOOP3_ASSERT(ti, CONFIG, da.numBlocksInUse(),
                                 0 == da.numBlocksInUse());
                    LOOP3_ASSERT(ti, CONFIG, sa.numBlocksInUse(),
                                 0 == sa.numBlocksInUse());
                }  // end foreach configuration

            }  // end foreach row

            // Double check that at least some object memory got allocated.

            ASSERT(anyObjectMemoryAllocatedFlag);

            // Note that memory should be independently allocated for each
            // attribute capable of allocating memory.

            for (int ti = 0; SPECS[ti]; ++ti) {
                const char *const SPEC = SPECS[ti];

                bslma::TestAllocator scratch("scratch", veryVeryVeryVerbose);

                Obj mZ(&scratch);  const Obj& Z = gg(&mZ, SPEC);

                if (veryVerbose) { T_ P_(ti) P(Z) }

                bslma::TestAllocator da("default",  veryVeryVeryVerbose);
                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                bslma::Default::setDefaultAllocatorRaw(&da);

                BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(oa) {
                    if (veryVeryVerbose) { T_ T_ Q(ExceptionTestBody) }

                    Obj obj(Z, &oa);
                } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

                LOOP2_ASSERT(ti, da.numBlocksInUse(), !da.numBlocksInUse());
                LOOP2_ASSERT(ti, oa.numBlocksInUse(), !oa.numBlocksInUse());
            }
        }
      } break;
      case 6: {
        // -------------------------------------------------------------------
        // EQUALITY-COMPARISON OPERATORS
        //   Ensure that '==' and '!=' are the operational definition of
        //   value.
        //
        // Concerns:
        //: 1 Two objects, 'X' and 'Y', compare equal if and only if their
        //:   corresponding year/month/day representations compare equal.
        //:
        //: 2 'true  == (X == X)' (i.e., identity).
        //:
        //: 3 'false == (X != X)' (i.e., identity).
        //:
        //: 4 'X == Y' if and only if 'Y == X' (i.e., commutativity).
        //:
        //: 5 'X != Y' if and only if 'Y != X' (i.e., commutativity).
        //:
        //: 6 'X != Y' if and only if '!(X == Y)'.
        //:
        //: 7 Comparison is symmetric with respect to user-defined conversion
        //:   (i.e., both comparison operators are free functions).
        //:
        //: 8 Non-modifiable objects can be compared (i.e., objects or
        //:   references providing only non-modifiable access).
        //:
        //: 9 The equality-comparison operators' signatures and return types
        //:   are standard.
        //
        // Plan:
        //: 1 Use the respective addresses of 'operator==' and 'operator!=' to
        //:   initialize function pointers having the appropriate signatures
        //:   and return types for the two homogeneous, free equality-
        //:   comparison operators defined in this component.  (C-7..9)
        //:
        //: 2 Using the table-driven technique, specify a set of distinct
        //:   object values (one per row).
        //:
        //: 3 For each row 'R1' in the table of P-2:  (C-1..6)
        //:
        //:   1 Use the generator function to create a 'const' object, 'W',
        //:     having the value from 'R1'.
        //:
        //:   2 Using 'W', verify the reflexive (anti-reflexive) property of
        //:     equality (inequality) in the presence of aliasing.  (C-2..3)
        //:
        //:   3 For each row 'R2' in the table of P-2:  (C-1, 4..6)
        //:
        //:     1 Record, in 'EXP', whether or not distinct objects set to
        //:       values from 'R1' and 'R2', respectively, are expected to
        //:       have the same value.
        //:
        //:     2 Use the generator function to create a 'const' object, 'X',
        //:       having the value from 'R1', and a second 'const' object,
        //:       'Y', having the value from 'R2'.
        //:
        //:     3 Using 'X' and 'Y', verify the commutativity property and
        //:       expected return value for both '==' and '!='.  (C-1, 4..6)
        //
        // Testing:
        //   bool operator==(lhs, rhs);
        //   bool operator!=(lhs, rhs);
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "EQUALITY-COMPARISON OPERATORS" << endl
                          << "=============================" << endl;

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

        static const char **SPECS = DEFAULT_SPECS;

        if (verbose) cout << "\nCompare every value with every value."
                          << endl;

        for (int ti = 0; SPECS[ti]; ++ti) {
            // Ensure an object compares correctly with itself (alias test).
            {
                Obj mW;  const Obj W = gg(&mW, SPECS[ti]);

                LOOP2_ASSERT(ti, W,   W == W);
                LOOP2_ASSERT(ti, W, !(W != W));

                // Ensure the first row of the table contains the
                // default-constructed value.

                static bool firstFlag = true;
                if (firstFlag) {
                    LOOP3_ASSERT(ti, Obj(), W, Obj() == W)
                    firstFlag = false;
                }
            }

            for (int tj = 0; SPECS[tj]; ++tj) {
                const bool EXP = ti == tj;

                Obj mX;  const Obj& X = gg(&mX, SPECS[ti]);

                Obj mY;  const Obj  Y = gg(&mY, SPECS[tj]);

                if (veryVerbose) { T_ T_ T_ P_(EXP) P_(X) P(Y) }

                // Verify value and commutativity.

                LOOP4_ASSERT(ti, tj, X, Y,  EXP == (X == Y));
                LOOP4_ASSERT(ti, tj, Y, X,  EXP == (Y == X));

                LOOP4_ASSERT(ti, tj, X, Y, !EXP == (X != Y));
                LOOP4_ASSERT(ti, tj, Y, X, !EXP == (Y != X));
            }
        }
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // PRINT AND OUTPUT OPERATOR (<<)
        //   Ensure that the value of the object can be formatted appropriately
        //   on an 'ostream' in some standard, human-readable form.
        //
        // Concerns:
        //: 1 The 'print' method writes the value to the specified 'ostream'.
        //:
        //: 2 The 'print' method writes the value in the intended format.
        //:
        //: 3 The output using 's << obj' is the same as 'obj.print(s, 0, -1)'.
        //:
        //: 4 The 'print' method's signature and return type are standard.
        //:
        //: 5 The 'print' method returns the supplied 'ostream'.
        //:
        //: 6 The optional 'level' and 'spacesPerLevel' parameters have the
        //:   correct default values (0 and 4, respectively).
        //:
        //: 7 The output 'operator<<'s signature and return type are standard.
        //:
        //: 8 The output 'operator<<' returns the destination 'ostream'.
        //
        // Plan:
        //: 1 Use the addresses of the 'print' member function and 'operator<<'
        //:   free function defined in this component to initialize,
        //:   respectively, member-function and free-function pointers having
        //:   the appropriate signatures and return types.  (C-4, 7)
        //:
        //: 2 Using the table-driven technique:  (C-1..3, 5..6, 8)
        //:
        //:   1 Define fourteen carefully selected combinations of (two) object
        //:     values ('A' and 'B') and various values for the two formatting
        //:     parameters, along with the expected output.
        //:
        //:     ( 'value' x  'level'   x 'spacesPerLevel' ):
        //:     1 { A   } x {  0     } x {  0, 1, -1, -8 } --> 3 expected o/ps
        //:     2 { A   } x {  3, -3 } x {  0, 2, -2, -8 } --> 6 expected o/ps
        //:     3 { B   } x {  2     } x {  3            } --> 1 expected o/p
        //:     4 { A B } x { -8     } x { -8            } --> 2 expected o/ps
        //:     5 { A B } x { -9     } x { -9            } --> 2 expected o/ps
        //:
        //:   2 For each row in the table defined in P-2.1:  (C-1..3, 5..6, 8)
        //:
        //:     1 Using a 'const' 'Obj', supply each object value and pair of
        //:       formatting parameters to 'print', omitting the 'level' or
        //:       'spacesPerLevel' parameter if the value of that argument is
        //:       '-8'.  If the parameters are, arbitrarily, '(-9, -9)', then
        //:       invoke the 'operator<<' instead.
        //:
        //:     2 Use a standard 'ostringstream' to capture the actual output.
        //:
        //:     3 Verify the address of what is returned is that of the
        //:       supplied stream.  (C-5, 8)
        //:
        //:     4 Compare the contents captured in P-2.2.2 with what is
        //:       expected.  (C-1..3, 6)
        //
        // Testing:
        //   ostream& print(ostream& stream, int level = 0, int sPL = 4) const;
        //   ostream& operator<<(ostream&, const PackedCalendar&);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRINT AND OUTPUT OPERATOR (<<)" << endl
                          << "==============================" << endl;


        if (verbose) cout << "\nAssign the addresses of 'print' and "
                             "the output 'operator<<' to variables." << endl;
        {
            typedef ostream& (Obj::*funcPtr)(ostream&, int, int) const;
            typedef ostream& (*operatorPtr)(ostream&, const Obj&);

            // Verify that the signatures and return types are standard.

            funcPtr     printMember = &Obj::print;
            operatorPtr operatorOut = bdlt::operator<<;

            (void)printMember;  // quash potential compiler warnings
            (void)operatorOut;
        }

        if (verbose) cout <<
             "\nCreate a table of distinct value/format combinations." << endl;

        static const struct {
            int         d_line;            // source line number
            int         d_level;
            int         d_spacesPerLevel;

            const char *d_spec_p;

            const char *d_expected_p;
        } DATA[] = {

        // ------------------------------------------------------------------
        // P-2.1.1: { A } x { 0 }     x { 0, 1, -1, -8 } -->  4 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL           SPEC                          EXP
        //---- - ---  ----------------------  ------------------------------

        { L_,  0,  0,  "@2014/1/1au 30 3 5B",
                                             "{\n[ 01JAN2014, 31JAN2014 ]\n"
                                             "[ 01JAN0001 : [ SUN SAT ] ]\n"
                                         "04JAN2014\n06JAN2014 {\n1\n}\n}\n" },

        { L_,  0,  1,  "@2014/1/1au 30 3 5B",
                                            "{\n [ 01JAN2014, 31JAN2014 ]\n"
                                            " [ 01JAN0001 : [ SUN SAT ] ]\n"
                                    " 04JAN2014\n 06JAN2014 {\n  1\n }\n}\n" },

        { L_,  0, -1,  "@2014/1/1au 30 3 5B",   "{ [ 01JAN2014, 31JAN2014 ]"
                                              " [ 01JAN0001 : [ SUN SAT ] ]"
                                              " 04JAN2014 06JAN2014 { 1 } }" },

        { L_,  0, -8,  "@2014/1/1au 30 3 5B",
                                         "{\n    [ 01JAN2014, 31JAN2014 ]\n"
                          "    [ 01JAN0001 : [ SUN SAT ] ]\n    04JAN2014\n"
                                    "    06JAN2014 {\n        1\n    }\n}\n" },

        // ------------------------------------------------------------------
        // P-2.1.2: { A } x { 3, -3 } x { 0, 2, -2, -8 } -->  6 expected o/ps
        // ------------------------------------------------------------------

        //LINE L SPL           SPEC                          EXP
        //---- - ---  ----------------------  ------------------------------

        { L_,  3,  0,  "@2014/1/1au 30 3 5B",
                                             "{\n[ 01JAN2014, 31JAN2014 ]\n"
                                             "[ 01JAN0001 : [ SUN SAT ] ]\n"
                                         "04JAN2014\n06JAN2014 {\n1\n}\n}\n" },

        { L_,  3,  2,  "@2014/1/1au 30 3 5B",
                               "      {\n        [ 01JAN2014, 31JAN2014 ]\n"
                                     "        [ 01JAN0001 : [ SUN SAT ] ]\n"
                                  "        04JAN2014\n        06JAN2014 {\n"
                                         "          1\n        }\n      }\n" },

        { L_,  3, -2,  "@2014/1/1au 30 3 5B",
                                         "      { [ 01JAN2014, 31JAN2014 ] "
                                               "[ 01JAN0001 : [ SUN SAT ] ]"
                                              " 04JAN2014 06JAN2014 { 1 } }" },

        { L_,  3,  -8,  "@2014/1/1au 30 3 5B",             "            {\n"
                                "                [ 01JAN2014, 31JAN2014 ]\n"
                             "                [ 01JAN0001 : [ SUN SAT ] ]\n"
                  "                04JAN2014\n                06JAN2014 {\n"
                 "                    1\n                }\n            }\n" },

        { L_, -3,  0,  "@2014/1/1au 30 3 5B",
                                             "{\n[ 01JAN2014, 31JAN2014 ]\n"
                                             "[ 01JAN0001 : [ SUN SAT ] ]\n"
                                         "04JAN2014\n06JAN2014 {\n1\n}\n}\n" },

        { L_, -3,  2,  "@2014/1/1au 30 3 5B",
                                     "{\n        [ 01JAN2014, 31JAN2014 ]\n"
                                     "        [ 01JAN0001 : [ SUN SAT ] ]\n"
                                  "        04JAN2014\n        06JAN2014 {\n"
                                         "          1\n        }\n      }\n" },

        { L_, -3, -2,  "@2014/1/1au 30 3 5B",   "{ [ 01JAN2014, 31JAN2014 ]"
                                              " [ 01JAN0001 : [ SUN SAT ] ]"
                                              " 04JAN2014 06JAN2014 { 1 } }" },

        { L_, -3,  -8,  "@2014/1/1au 30 3 5B",
                             "{\n                [ 01JAN2014, 31JAN2014 ]\n"
                             "                [ 01JAN0001 : [ SUN SAT ] ]\n"
                  "                04JAN2014\n                06JAN2014 {\n"
                 "                    1\n                }\n            }\n" },

        // -----------------------------------------------------------------
        // P-2.1.3: { B } x { 2 }     x { 3 }            -->  1 expected o/p
        // -----------------------------------------------------------------

        //LINE L SPL           SPEC                          EXP
        //---- - ---  ----------------------  ------------------------------

        { L_,  2,  3,  "@2014/1/1au 30 3f 5",                    "      {\n"
                                       "         [ 01JAN2014, 31JAN2014 ]\n"
               "         [ 01JAN0001 : [ SUN SAT ], 04JAN2014 : [ FRI ] ]\n"
                                             "         06JAN2014\n      }\n" },

        // -----------------------------------------------------------------
        // P-2.1.4: { A B } x { -8 }   x { -8 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL           SPEC                          EXP
        //---- - ---  ----------------------  ------------------------------

        { L_, -8, -8,  "@2014/1/1au 30 3 5B",
                                         "{\n    [ 01JAN2014, 31JAN2014 ]\n"
                                         "    [ 01JAN0001 : [ SUN SAT ] ]\n"
                                          "    04JAN2014\n    06JAN2014 {\n"
                                                     "        1\n    }\n}\n" },

        { L_, -8, -8,  "@2014/1/1au 30 3f 5",
                                         "{\n    [ 01JAN2014, 31JAN2014 ]\n"
                    "    [ 01JAN0001 : [ SUN SAT ], 04JAN2014 : [ FRI ] ]\n"
                                                        "    06JAN2014\n}\n" },

        // -----------------------------------------------------------------
        // P-2.1.5: { A B } x { -9 }   x { -9 }         -->  2 expected o/ps
        // -----------------------------------------------------------------

        //LINE L SPL      Y   M   D  EXP
        //---- - ---   ----  --  --  ---

        { L_, -9, -9,  "@2014/1/1au 30 3 5B",   "{ [ 01JAN2014, 31JAN2014 ]"
                                              " [ 01JAN0001 : [ SUN SAT ] ]"
                                              " 04JAN2014 06JAN2014 { 1 } }" },

        { L_, -9, -9,  "@2014/1/1au 30 3f 5",   "{ [ 01JAN2014, 31JAN2014 ]"
                         " [ 01JAN0001 : [ SUN SAT ], 04JAN2014 : [ FRI ] ]"
                                                              " 06JAN2014 }" },

        };
        const int NUM_DATA = static_cast<int>(sizeof DATA / sizeof *DATA);

        if (verbose) cout << "\nTesting with various print specifications."
                          << endl;
        {
            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const int         LINE  = DATA[ti].d_line;
                const int         L     = DATA[ti].d_level;
                const int         SPL   = DATA[ti].d_spacesPerLevel;
                const char *const SPEC  = DATA[ti].d_spec_p;
                const char *const EXP   = DATA[ti].d_expected_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(L) P(SPL) P(SPEC);
                }

                if (veryVeryVerbose) { T_ T_ Q(EXPECTED) cout << EXP; }

                Obj mX;
                gg(&mX, SPEC);
                const Obj& X = mX;

                bslma::TestAllocator oa("object", veryVeryVeryVerbose);

                ostringstream os(&oa);

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

                {
                    bslma::TestAllocator da("default", veryVeryVeryVerbose);

                    bslma::DefaultAllocatorGuard dag(&da);

                    // Verify output is formatted as expected.

                    bsl::string osStr = os.str();

                    if (veryVeryVerbose) { P(osStr) }

                    LOOP3_ASSERT(LINE, EXP, osStr, EXP == osStr);
                }
            }
        }

        // Verify behavior when the stream is initially 'bad()'.

        {
            Obj mX;
            gg(&mX, "@2012/01/01");
            const Obj& X = mX;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            ostringstream os(&oa);

            os.setstate(bsl::ios::badbit);

            ASSERT(&os == &X.print(os));

            ASSERTV(os.str(), os.str() == "");
        }

        // Additional, ad-hoc tests.

        {
            Obj mX;
            gg(&mX, "@2012/01/01");
            const Obj& X = mX;

            bslma::TestAllocator oa("object", veryVeryVeryVerbose);

            ostringstream os(&oa);

            X.print(os, 1, -60);

            ASSERTV(os.str(), os.str() == "                                 "
                "                           { [ 01JAN2012, 01JAN2012 ] [ ] }");
        }

        {
            char R0[]  = "{ [ 31DEC9999, 01JAN0001 ] [ ] }";
            char R1[]  = "{ [ 31DEC9999, 01JAN0001 ] "
                         "[ 01JAN0001 : [ SUN SAT ] ] }";
            char R2[]  = "{ [ 01JAN0001, 01JAN0001 ] [ ] }";
            char R3[]  = "{ [ 01JAN0001, 19MAY0001 ] "
                         "[ 01JAN0001 : [ SUN SAT ] ] }";
            char R4[]  = "{ [ 01JAN0001, 19MAY0001 ] [ ] "
                         "02JAN0001 03JAN0001 { 0 1 } "
                         "}";
            char R5[]  = "{ [ 01JAN0001, 13JAN0001 ] [ ] "
                         "13JAN0001 { 0 1 2 100 1000 } }";
            char R6[]  = "{ [ 01JAN0001, 09FEB0001 ] "
                         "[ 01JAN0001 : [ SUN SAT ] ] "
                         "02JAN0001 03JAN0001 { 0 } 04JAN0001 { 1 } "
                         "08JAN0001 { 0 1 2 100 } }";
            char R7[]  = "  { [ 01JAN0001, 13JAN0001 ] [ ] "
                         "13JAN0001 { 0 1 2 100 1000 } }";
            char R8[]  = "      { [ 01JAN0001, 13JAN0001 ] [ ] "
                         "13JAN0001 { 0 1 2 100 1000 } }";
            char R9[]  = "  {\n"
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
            char R10[] = "{\n"
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
            char R11[] = "{ [ 01JAN0001, 13JAN0001 ] "
                         "[ 01JAN0001 : [ SUN FRI SAT ] ] "
                         "13JAN0001 { 0 1 2 100 1000 } }";
            char R12[] = "{ [ 01JAN0001, 11JAN0001 ] "
                         "[ 01JAN0001 : [ SUN FRI SAT ], "
                         "11JAN0001 : [ TUE ], "
                         "13JAN0001 : [ WED THU ] ] "
                         "02JAN0001 { 0 } 03JAN0001 { 1 } }";

            static const struct {
                const char *d_spec;
                int         d_level;
                int         d_spacesPerLevel;
                const char *d_result;
            } DATA[] = {
                //SPEC                        LEVEL SPACE PER LEVEL   RESULT
                {"",                          0,    -1,               R0 },
                {"au",                        0,    -1,               R1 },
                {"@1/1/1",                    0,    -1,               R2 },
                {"au@1/1/1 138",              0,    -1,               R3 },
                {"@1/1/1 138 1 2 2AB",        0,    -1,               R4 },
                {"@1/1/1 1 12ABCDE",          0,    -1,               R5 },
                {"au@1/1/1 39 1 2A 3B 7ABCD", 0,    -1,               R6 },
                {"@1/1/1 1 12ABCDE",          2,    -1,               R7 },
                {"@1/1/1 1 12ABCDE",          3,    -2,               R8 },
                {"au@1/1/1 1 12ABCDE",        1,     2,               R9 },
                {"au@1/1/1 1 12ABCDE",       -1,     2,               R10},
                {"fau@1/1/1 1 12ABCDE",      -1,    -2,               R11},
                {"fau@1/1/1 10t 1A 12wr 2B", -1,    -2,               R12},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int ti = 0; ti < NUM_DATA; ++ti) {
                const char        *SPEC = DATA[ti].d_spec;
                const int          LEVEL = DATA[ti].d_level;
                const int          SPACES = DATA[ti].d_spacesPerLevel;
                const bsl::string  RESULT = DATA[ti].d_result;

                bsl::ostringstream printStream;
                bsl::ostringstream operatorStream;
                {
                    bslma::TestAllocator a;  // specified allocator
                    LOOP_ASSERT( a.numBlocksTotal(), 0 ==  a.numBlocksTotal());

                    bslma::TestAllocator               da; // default allocator
                    const bslma::DefaultAllocatorGuard DAG(&da);
                    LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

                    Obj mX(&a); const Obj& X = mX;
                    LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                    gg(&mX, SPEC);

                    const bsls::Types::Int64 specifiedUsage =
                                                            a.numBlocksTotal();
                    const bsls::Types::Int64 defaultUsage   =
                                                           da.numBlocksTotal();

                    LOOP_ASSERT(ti, &printStream == &X.print(printStream,
                                                             LEVEL,
                                                             SPACES));
                    LOOP_ASSERT(ti, &operatorStream == &(operatorStream << X));

                    LOOP_ASSERT( a.numBlocksTotal(), specifiedUsage ==
                                                           a.numBlocksTotal());
                    LOOP_ASSERT(da.numBlocksTotal(), defaultUsage   ==
                                                          da.numBlocksTotal());
                }
                bsl::string cmp = printStream.str();
                LOOP3_ASSERT(ti, cmp.size(), RESULT.size(),
                                                  cmp.size() == RESULT.size());

                for (bsl::size_t i = 0; i < cmp.size(); ++i) {
                    LOOP4_ASSERT(ti,
                                 i,
                                 cmp[i],
                                 RESULT[i],
                                 cmp[i] == RESULT[i]);
                }
                if (LEVEL == 0 && SPACES == -1) {
                    bsl::string cmp2 = operatorStream.str();
                    LOOP3_ASSERT(ti, cmp.size(), cmp2.size(),
                                                    cmp.size() == cmp2.size());
                    for (bsl::size_t i = 0; i < cmp.size(); ++i) {
                        LOOP4_ASSERT(ti,
                                     i,
                                     cmp[i],
                                     cmp2[i],
                                     cmp[i] == cmp2[i]);
                    }
                }
            }
        }
      } break;
      case 4: {
        // -------------------------------------------------------------------
        // BASIC ACCESSORS
        //   Ensure each basic accessor properly interprets object state.
        //
        // Concerns:
        //: 1 Each of the basic accessors returns the expected value.
        //:
        //: 2 Each basic accessor method is declared 'const'.
        //:
        //: 3 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 To test 'allocator', create object with various allocators and
        //:   ensure the returned value matches the supplied allocator.
        //:
        //: 2 Directly test that each basic accessor, invoked on a set of
        //:   'const' objects created with the generator function, returns the
        //:   expected value.  (C-1..2)
        //:
        //: 3 Verify defensive checks are triggered for invalid values.  (C-3)
        //
        // Testing:
        //   bslma::Allocator *allocator() const;
        //   const Date& firstDate() const;
        //   bdlt::Date holiday(int index) const;
        //   int holidayCode(const Date& date, int index) const;
        //   const Date& lastDate() const;
        //   bool isHoliday(const Date& date) const;
        //   bool isInRange(const Date& date) const;
        //   bool isWeekendDay(const Date& date) const;
        //   bool isWeekendDay(DayOfWeek::Enum dayOfWeek) const;
        //   int numHolidayCodes(const Date& date) const;
        //   int numHolidays() const;
        //   int numWeekendDaysTransitions() const;
        //   WeekendDaysTransition weekendDaysTransition(int index) const;
        // -------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BASIC ACCESSORS" << endl
                          << "===============" << endl;

        if (verbose) cout << "\nTesting 'allocator'." << endl;
        {
            Obj mX;  const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
        }
        {
            Obj        mX(reinterpret_cast<bslma::TestAllocator *>(0));
            const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
        }
        {
            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            ASSERT(&sa == X.allocator());
        }

        if (verbose) cout << "\nTesting valid range methods." << endl;
        {
            Obj mX;  const Obj& X = gg(&mX, "");

            ASSERT(false == X.isInRange(bdlt::Date(   1,  1,  1)));
            ASSERT(false == X.isInRange(bdlt::Date(2014,  1,  1)));
            ASSERT(false == X.isInRange(bdlt::Date(9999, 12, 31)));
        }
        {
            Obj mX;  const Obj& X = gg(&mX, "@2007/01/01");

            ASSERT(X.firstDate() == bdlt::Date(2007,  1,  1));
            ASSERT(X.lastDate() ==  bdlt::Date(2007,  1,  1));

            ASSERT(false == X.isInRange(X.firstDate() - 1));
            ASSERT(true  == X.isInRange(X.firstDate()));
            ASSERT(false == X.isInRange(X.firstDate() + 1));
            ASSERT(false == X.isInRange(X.lastDate() - 1));
            ASSERT(true  == X.isInRange(X.lastDate()));
            ASSERT(false == X.isInRange(X.lastDate() + 1));
        }
        {
            Obj mX;  const Obj& X = gg(&mX, "@2007/01/01 30");

            ASSERT(X.firstDate() == bdlt::Date(2007,  1,  1));
            ASSERT(X.lastDate() ==  bdlt::Date(2007,  1, 31));

            ASSERT(false == X.isInRange(X.firstDate() - 1));
            ASSERT(true  == X.isInRange(X.firstDate()));
            ASSERT(true  == X.isInRange(X.firstDate() + 1));
            ASSERT(true  == X.isInRange(X.lastDate() - 1));
            ASSERT(true  == X.isInRange(X.lastDate()));
            ASSERT(false == X.isInRange(X.lastDate() + 1));
        }

        if (verbose) cout << "\nTesting holiday methods." << endl;
        {
            Obj mX;  const Obj& X = gg(&mX, "@2012/06/03 45 5 19BD");

            for (int i = 0; i <= 45; ++i) {
                ASSERT(X.isHoliday(bdlt::Date(2012, 6, 3) + i) ==
                                                          (i == 5 || i == 19));
            }

            ASSERT(X.numHolidays() == 2);
            ASSERT(X.numWeekendDaysTransitions() == 0);

            ASSERT(X.numHolidayCodes(bdlt::Date(2012, 6,  3)) == 0);
            ASSERT(X.numHolidayCodes(bdlt::Date(2012, 6,  8)) == 0);
            ASSERT(X.numHolidayCodes(bdlt::Date(2012, 6, 22)) == 2);

            ASSERT(bdlt::Date(2012, 6,  8) == X.holiday(0));
            ASSERT(bdlt::Date(2012, 6, 22) == X.holiday(1));

            ASSERT(X.holidayCode(bdlt::Date(2012, 6, 22), 0) == VB);
            ASSERT(X.holidayCode(bdlt::Date(2012, 6, 22), 1) == VD);
        }
        {
            Obj        mX;
            const Obj& X = gg(&mX, "@2014/01/01 90 10CE 14DE 17A 23ABE");

            for (int i = 0; i <= 90; ++i) {
                ASSERT(X.isHoliday(bdlt::Date(2014, 1, 1) + i) ==
                                   (i == 10 || i == 14 || i == 17 || i == 23));
            }

            ASSERT(X.numHolidays() == 4);
            ASSERT(X.numWeekendDaysTransitions() == 0);

            ASSERT(X.numHolidayCodes(bdlt::Date(2014, 1,  2)) == 0);
            ASSERT(X.numHolidayCodes(bdlt::Date(2014, 1, 11)) == 2);
            ASSERT(X.numHolidayCodes(bdlt::Date(2014, 1, 15)) == 2);
            ASSERT(X.numHolidayCodes(bdlt::Date(2014, 1, 18)) == 1);
            ASSERT(X.numHolidayCodes(bdlt::Date(2014, 1, 24)) == 3);

            ASSERT(bdlt::Date(2014, 1, 11) == X.holiday(0));
            ASSERT(bdlt::Date(2014, 1, 15) == X.holiday(1));
            ASSERT(bdlt::Date(2014, 1, 18) == X.holiday(2));
            ASSERT(bdlt::Date(2014, 1, 24) == X.holiday(3));

            ASSERT(X.holidayCode(bdlt::Date(2014, 1, 11), 0) == VC);
            ASSERT(X.holidayCode(bdlt::Date(2014, 1, 11), 1) == VE);

            ASSERT(X.holidayCode(bdlt::Date(2014, 1, 15), 0) == VD);
            ASSERT(X.holidayCode(bdlt::Date(2014, 1, 15), 1) == VE);

            ASSERT(X.holidayCode(bdlt::Date(2014, 1, 18), 0) == VA);

            ASSERT(X.holidayCode(bdlt::Date(2014, 1, 24), 0) == VA);
            ASSERT(X.holidayCode(bdlt::Date(2014, 1, 24), 1) == VB);
            ASSERT(X.holidayCode(bdlt::Date(2014, 1, 24), 2) == VE);
        }

        if (verbose) cout << "\nTesting weekend methods." << endl;
        {
            Obj mX;  const Obj& X = gg(&mX, "@2015/05/07au 45");

            ASSERT(X.numHolidays() == 0);
            ASSERT(X.numWeekendDaysTransitions() == 1);

            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_SUN) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_MON) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_TUE) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_WED) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_THU) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_FRI) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_SAT) == true);

            ASSERT(X.isWeekendDay(bdlt::Date(2015, 5, 15)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2015, 5, 16)) == true);
            ASSERT(X.isWeekendDay(bdlt::Date(2015, 5, 17)) == true);
            ASSERT(X.isWeekendDay(bdlt::Date(2015, 5, 18)) == false);

            bdlt::DayOfWeekSet wAU;
            wAU.add(bdlt::DayOfWeek::e_SUN);
            wAU.add(bdlt::DayOfWeek::e_SAT);

            ASSERT(bdlt::PackedCalendar::WeekendDaysTransition(
                                                      bdlt::Date(1, 1, 1), wAU)
                                                == X.weekendDaysTransition(0));
        }
        {
            Obj mX;  const Obj& X = gg(&mX, "");

            ASSERT(X.numHolidays() == 0);
            ASSERT(X.numWeekendDaysTransitions() == 0);

            ASSERT(X.isWeekendDay(bdlt::Date(2009, 11, 22)) == false);
        }
        {
            Obj mX;  const Obj& X = gg(&mX, "@2009/11/22mwf 45");

            ASSERT(X.numHolidays() == 0);
            ASSERT(X.numWeekendDaysTransitions() == 1);

            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_SUN) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_MON) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_TUE) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_WED) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_THU) == false);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_FRI) == true);
            ASSERT(X.isWeekendDay(bdlt::DayOfWeek::e_SAT) == false);

            ASSERT(X.isWeekendDay(bdlt::Date(2009, 11, 22)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2009, 11, 23)) == true);
            ASSERT(X.isWeekendDay(bdlt::Date(2009, 11, 24)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2009, 11, 25)) == true);
            ASSERT(X.isWeekendDay(bdlt::Date(2009, 11, 26)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2009, 11, 27)) == true);
            ASSERT(X.isWeekendDay(bdlt::Date(2009, 11, 28)) == false);

            bdlt::DayOfWeekSet wMWF;
            wMWF.add(bdlt::DayOfWeek::e_MON);
            wMWF.add(bdlt::DayOfWeek::e_WED);
            wMWF.add(bdlt::DayOfWeek::e_FRI);

            ASSERT(bdlt::PackedCalendar::WeekendDaysTransition(
                                                     bdlt::Date(1, 1, 1), wMWF)
                                                == X.weekendDaysTransition(0));
        }
        {
            Obj mX;  const Obj& X = gg(&mX, "@2014/03/01ua 45 14mwf 29t");

            ASSERT(X.numHolidays() == 0);
            ASSERT(X.numWeekendDaysTransitions() == 3);

            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3,  2)) == true);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3,  3)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3,  4)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3,  5)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3,  6)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3,  7)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3,  8)) == true);

            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3, 16)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3, 17)) == true);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3, 18)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3, 19)) == true);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3, 20)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3, 21)) == true);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 3, 22)) == false);

            ASSERT(X.isWeekendDay(bdlt::Date(2014, 4,  6)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 4,  7)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 4,  8)) == true);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 4,  9)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 4, 10)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 4, 11)) == false);
            ASSERT(X.isWeekendDay(bdlt::Date(2014, 4, 12)) == false);

            bdlt::DayOfWeekSet wAU;
            wAU.add(bdlt::DayOfWeek::e_SUN);
            wAU.add(bdlt::DayOfWeek::e_SAT);

            ASSERT(bdlt::PackedCalendar::WeekendDaysTransition(
                                                      bdlt::Date(1, 1, 1), wAU)
                                                == X.weekendDaysTransition(0));

            bdlt::DayOfWeekSet wMWF;
            wMWF.add(bdlt::DayOfWeek::e_MON);
            wMWF.add(bdlt::DayOfWeek::e_WED);
            wMWF.add(bdlt::DayOfWeek::e_FRI);

            ASSERT(bdlt::PackedCalendar::WeekendDaysTransition(
                                                 bdlt::Date(2014, 3, 15), wMWF)
                                                == X.weekendDaysTransition(1));

            bdlt::DayOfWeekSet wT;
            wT.add(bdlt::DayOfWeek::e_TUE);

            ASSERT(bdlt::PackedCalendar::WeekendDaysTransition(
                                                   bdlt::Date(2014, 3, 30), wT)
                                                == X.weekendDaysTransition(2));
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            Obj mX;  const Obj& X = gg(&mX, "@2012/01/01 30 5A 19");

            ASSERT_SAFE_FAIL(X.holiday(-1));
            ASSERT_SAFE_PASS(X.holiday( 0));
            ASSERT_SAFE_PASS(X.holiday( 1));
            ASSERT_SAFE_FAIL(X.holiday( 2));

            ASSERT_SAFE_PASS(X.holidayCode(bdlt::Date(2012, 1, 6),  0));
            ASSERT_SAFE_FAIL(X.holidayCode(bdlt::Date( 100, 1, 6),  0));
            ASSERT_SAFE_FAIL(X.holidayCode(bdlt::Date(2012, 1, 5),  0));
            ASSERT_SAFE_FAIL(X.holidayCode(bdlt::Date(2012, 1, 6), -1));
            ASSERT_SAFE_FAIL(X.holidayCode(bdlt::Date(2012, 1, 6),  1));

            ASSERT_SAFE_PASS(X.isHoliday(bdlt::Date(2012, 1, 6)));
            ASSERT_SAFE_FAIL(X.isHoliday(bdlt::Date( 100, 1, 6)));

            ASSERT_PASS(X.numHolidayCodes(bdlt::Date(2012, 1, 6)));
            ASSERT_FAIL(X.numHolidayCodes(bdlt::Date( 100, 1, 6)));

            Obj mY;  const Obj& Y = gg(&mY, "@2012/01/01ua 30 14mwf");

            ASSERT_SAFE_PASS(X.isWeekendDay(bdlt::DayOfWeek::e_SUN));
            ASSERT_SAFE_FAIL(Y.isWeekendDay(bdlt::DayOfWeek::e_SUN));
        }

      } break;
      case 3: {
        // --------------------------------------------------------------------
        // PRIMITIVE GENERATOR FUNCTIONS
        //   Ensure that the primitive generator functions are able to create
        //   an object in any state.
        //
        // Concerns:
        //: 1 Valid syntax produces the expected results.
        //:
        //: 2 Invalid syntax is detected and reported.
        //
        // Plan:
        //: 1 Evaluate a series of test strings of increasing complexity to
        //:   set the state of a newly created object and verify the returned
        //:   object using basic accessors.  (C-1)
        //:
        //: 2 Evaluate the 'ggg' function with a series of test strings of
        //:   increasing complexity and verify its return value.  (C-2)
        //
        // Testing:
        //   bdlt::PackedCalendar& gg(bdlt::PackedCalendar *o, const char *s);
        //   int ggg(bdlt::PackedCalendar *obj, const char *spec, bool vF);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "PRIMITIVE GENERATOR FUNCTIONS" << endl
                          << "=============================" << endl;

        const bdlt::Date xD1(2000, 1,  2); const bdlt::Date *D1 = &xD1;

        const bdlt::Date xD2(2000, 1,  3); const bdlt::Date *D2 = &xD2;

        const bdlt::Date xD3(2000, 1,  1); const bdlt::Date *D3 = &xD3;

        const bdlt::Date xD4(2000, 1, 15); const bdlt::Date *D4 = &xD4;

        const bdlt::Date xD5(2000, 1, 31); const bdlt::Date *D5 = &xD5;

        const bdlt::Date xD6(2000, 2, 15); const bdlt::Date *D6 = &xD6;

        const bdlt::Date xD7(2000, 2, 29); const bdlt::Date *D7 = &xD7;

        static struct {
            int               d_lineNumber;

            const char       *d_input;              // input command

            int               d_retCode;            // return code of ggg()

            int               d_length;             // length of the calendar

            int               d_businessDays;       // number of the business
                                                    // days in the calendar

            int               d_weekendDaysInWeek;  // number of WE days per
                                                    // week in the calendar

            int               d_holidays;           // number of holidays in
                                                    // the calendar

            const bdlt::Date *d_date_p;             // date to test holidays
                                                    // codes for

            int               d_holidayCodes;       // the number of holiday
                                                    // codes for d_date
        } DATA[] = {
//----------^
//Lin Input                                        RC  Len  BDs  WE Hol  D HC
//--  -------------------------------------------  --  ---  ---  -  --  --  -
{ L_, ""                                         , -1,   0,   0, 0,  0, D1, 0},
{ L_, "m"                                        , -1,   0,   0, 1,  0, D1, 0},
{ L_, "ua"                                       , -1,   0,   0, 2,  0, D1, 0},
{ L_, "@2000"                                    ,  5,   0,   0, 0,  0, D1, 0},
{ L_, "@2000/1/1"                                , -1,   1,   1, 0,  0, D1, 0},
{ L_, "u@2000/1/1a"                              , -1,   1,   0, 2,  0, D1, 0},
{ L_, "@2000/1/1sa"                              ,  9,   1,   1, 0,  0, D1, 0},
{ L_, "u@2000/1/1 @2000/1/10 @2000/1/1mt"        , -1,  10,   7, 1,  0, D1, 0},
{ L_, "rf@2000/1/1 @2000/1/3 @2000/1/1"          , -1,   3,   2, 2,  1, D1, 0},
{ L_, "rf@2000/1/1 2 0"                          , -1,   3,   2, 2,  1, D1, 0},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D4, 1},

{ L_, "@9999/12/31 1"                            , 13,   1,   1, 0,  0, D1, 0},

{ L_, "2000/1/1 3 2"                             ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "@2000/1/Asa"                              ,  8,   0,   0, 0,  0, D1, 0},
{ L_, "@2000/1/1sa"                              ,  9,   1,   1, 0,  0, D1, 0},

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

{ L_, "umtwrfa"                                  , -1,   0,   0, 7,  0, D1, 0},
{ L_, "@1/1/1umtwrfa"                            , -1,   1,   0, 7,  0, D1, 0},
{ L_, "afr@1/1/1wtmu"                            , -1,   1,   0, 7,  0, D1, 0},
{ L_, "umtwrfa@1/1/1"                            , -1,   1,   0, 7,  0, D1, 0},

{ L_, "uu"                                       ,  1,   0,   0, 1,  0, D1, 0},
{ L_, "uut"                                      ,  1,   0,   0, 1,  0, D1, 0},
{ L_, "utt"                                      ,  2,   0,   0, 2,  0, D1, 0},
{ L_, "utu"                                      ,  2,   0,   0, 2,  0, D1, 0},
{ L_, "a@2000/1/1a@2001/1/1 u"                   , 10,   1,   0, 1,  0, D1, 0},

{ L_, "zaf"                                      ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "azf"                                      ,  1,   0,   0, 1,  0, D1, 0},
{ L_, "afz"                                      ,  2,   0,   0, 2,  0, D1, 0},
{ L_, "z@2000/1/1 af"                            ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "a@2000/1/1  @2000/1/2zf"                  , 21,   2,   1, 1,  0, D1, 0},
{ L_, "@2000/1/1@2000/1/2 afz"                   , 21,   2,   2, 0,  0, D1, 0},

{ L_, "1"                                        ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "5"                                        ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "arf15"                                    ,  3,   0,   0, 3,  0, D1, 0},
{ L_, "15arf"                                    ,  0,   0,   0, 0,  0, D1, 0},
{ L_, "arf15"                                    ,  3,   0,   0, 3,  0, D1, 0},

{ L_, "@2000/1/1 @2000/1/1"                      , -1,   1,   1, 0,  0, D1, 0},
{ L_, "@2000/1/1 0"                              , -1,   1,   1, 0,  0, D1, 0},
{ L_, "@2000/1/1 @2000/1/3"                      , -1,   3,   3, 0,  0, D1, 0},
{ L_, "@2000/1/3 @2000/1/1"                      , -1,   3,   3, 0,  0, D1, 0},
{ L_, "@2000/1/1 2"                              , -1,   3,   3, 0,  0, D1, 0},

{ L_, "@2000/1/1 30 14"                          , -1,  31,  30, 0,  1, D1, 0},
{ L_, "@2000/1/1 30 @2000/1/15"                  , -1,  31,  30, 0,  1, D1, 0},
{ L_, "@2000/1/1 30 @2000/1/15 30"               , -1,  45,  43, 0,  2, D1, 0},
{ L_, "@2000/1/1 30 @2001/2/29"                  , 21,  31,  31, 0,  0, D1, 0},
{ L_, "@2000/1/1 30 14 @2001/2/29"               , 24,  31,  30, 0,  1, D1, 0},

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

{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D1, 0},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D2, 0},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D3, 1},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D4, 1},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D5, 1},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D6, 0},
{ L_, "w@2000/1/1 59 0A 14B 30C 45 59DE"         , -1,  60,  47, 1,  5, D7, 2},

{ L_, "au@2000/1/1 30 1AB 2DCE~au@2000/1/1 30 1A", -1,  31,  21, 2,  1, D1, 1},

{ L_, " @2000  /   1    /    1  30 1 A  B2  D CE", -1,  31,  29, 0,  2, D1, 2},
{ L_, " a  @   2000 /1 / 1 30 u1 AB2D C E"       , -1,  31,  24, 1,  2, D2, 3},
};
//----------v
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            if (veryVerbose) { T_; P(ti); }
            const int         LINE  =  DATA[ti].d_lineNumber;
            const char       *INPUT =  DATA[ti].d_input;
            const int         RC    =  DATA[ti].d_retCode;
            const int         LEN   =  DATA[ti].d_length;
            const int         BDAY  =  DATA[ti].d_businessDays;
            const int         WDAY  =  DATA[ti].d_weekendDaysInWeek;
            const int         HOLI  =  DATA[ti].d_holidays;
            const bdlt::Date  DCOD  = *DATA[ti].d_date_p;
            const int         HCOD  =  DATA[ti].d_holidayCodes;

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
            if (veryVerbose) { T_ P(ti) P(INPUT) P(X) }

            LOOP3_ASSERT(LINE, RC, retCode, RC == retCode);
            LOOP3_ASSERT(LINE, LEN, X.length(), LEN == X.length());
            LOOP3_ASSERT(LINE,
                         BDAY,
                         X.numBusinessDays(),
                         BDAY == X.numBusinessDays());
            LOOP3_ASSERT(LINE,
                         WDAY,
                         numWeekendDaysInFirstTransition(X),
                         WDAY == numWeekendDaysInFirstTransition(X));
            LOOP3_ASSERT(LINE,
                         HOLI,
                         X.numHolidays(),
                         HOLI == X.numHolidays());
            if (X.isInRange(DCOD)) {
                LOOP4_ASSERT(LINE,
                             DCOD,
                             HCOD,
                             X.numHolidayCodes(DCOD),
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

            // Test 'gg'.

            if (retCode == -1) {
                Obj mY; const Obj& Y = mY;

                Obj& R = gg(&mY, INPUT);
                LOOP_ASSERT(LINE, &R == &Y);

                LOOP3_ASSERT(LINE, LEN, Y.length(), LEN == Y.length());
                LOOP3_ASSERT(LINE,
                             BDAY,
                             Y.numBusinessDays(),
                             BDAY == Y.numBusinessDays());
                LOOP3_ASSERT(LINE,
                             WDAY,
                             numWeekendDaysInFirstTransition(Y),
                             WDAY == numWeekendDaysInFirstTransition(Y));
                LOOP3_ASSERT(LINE,
                             HOLI,
                             Y.numHolidays(),
                             HOLI == Y.numHolidays());
                if (Y.isInRange(DCOD)) {
                    LOOP4_ASSERT(LINE,
                                 DCOD,
                                 HCOD,
                                 Y.numHolidayCodes(DCOD),
                                 HCOD == Y.numHolidayCodes(DCOD));
                }
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

                Obj mX;  const Obj& X = mX;

                int retCode = ggg(&mX, spec, false);
                ASSERT(-1 == retCode);

                Obj::WeekendDaysTransitionConstIterator iter =
                                               X.beginWeekendDaysTransitions();
                ASSERT(sameWeekendDaysTransition(*iter,
                                                 bdlt::Date(2000,1,1),
                                                 "rw"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(2000,1,10),
                                                 "mt"));
                ASSERT(++iter == X.endWeekendDaysTransitions());
            }

            {
                const char* spec = "w@2000/1/1 60 0wu 15mt 30rfa";

                Obj mX;  const Obj& X = mX;

                int retCode = ggg(&mX, spec, false);
                ASSERT(-1 == retCode);

                Obj::WeekendDaysTransitionConstIterator iter =
                                               X.beginWeekendDaysTransitions();
                ASSERT(sameWeekendDaysTransition(*iter,
                                                 bdlt::Date(1,1,1),
                                                 "w"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(2000,1,1),
                                                 "wu"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(2000,1,16),
                                                 "mt"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(2000,1,31),
                                                 "rfa"));
                ASSERT(++iter == X.endWeekendDaysTransitions());
            }

            {
                const char* spec = "w@2000/1/1 @2000/3/1 @2000/1/1wu "
                                   "@2000/1/16mt @2000/1/18e @2000/1/31rfa";

                Obj mX;  const Obj& X = mX;

                int retCode = ggg(&mX, spec, false);
                ASSERT(-1 == retCode);

                Obj::WeekendDaysTransitionConstIterator iter =
                                               X.beginWeekendDaysTransitions();
                ASSERT(sameWeekendDaysTransition(*iter,
                                                 bdlt::Date(1,1,1),
                                                 "w"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(2000,1,1),
                                                 "wu"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(2000,1,16),
                                                 "mt"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(2000,1,18),
                                                 ""));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(2000,1,31),
                                                 "rfa"));
                ASSERT(++iter == X.endWeekendDaysTransitions());
            }
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // PRIMARY MANIPULATORS TEST
        //   The basic concern is that the default constructor, the destructor,
        //   and the primary manipulators:
        //      - addDay
        //      - addHoliday
        //      - addHolidayCode
        //      - addWeekendDay
        //      - addWeekendDaysTransition
        //      - removeAll
        //   operate as expected.
        //
        // Concerns:
        //: 1 Default constructor
        //:     a. creates an object with the expected value
        //:     b. properly wires the optionally-specified allocator
        //:
        //: 2 That 'addDay'
        //:     a. properly extends the object range if needed
        //:     b. properly handles duplicates
        //:     c. is exception-neutral
        //:
        //: 3 That 'addWeekendDay'
        //:     a. properly sets the day as a weekend day to the transition at
        //:        1/1/1
        //:     b. properly handles duplicates
        //:     c. is exception-neutral
        //:
        //: 4 That 'addHoliday'
        //:     a. increases the object range if needed
        //:     b. properly sets the specified day as a holiday
        //:     c. properly handles duplicates
        //:     d. is exception-neutral
        //:
        //: 5 That 'addHolidayCode'
        //:     a. increases the object range if needed
        //:     b. sets the specified date as a holiday if needed
        //:     c. properly sets a code for the specified holiday
        //:     d. properly handles multiple codes for one holiday
        //:     e. properly handles duplicates
        //:     f. is exception-neutral
        //:
        //: 6 That 'addWeekendDaysTransition'
        //:     a. properly adds a weekend-days transition
        //:     b. properly handles duplicates
        //:     c. is exception-neutral
        //:
        //: 7 That 'removeAll'
        //:     a. produces the expected value (empty)
        //:     b. leaves the object in a consistent state
        //:
        //: 8 Memory is not leaked by any method and the destructor properly
        //:   deallocates the residual allocated memory.
        //:
        //: 9 QoI: Asserted precondition violations are detected when enabled.
        //
        // Plan:
        //: 1 Create an object using the default constructor with and without
        //:   passing in an allocator, verify the allocator is stored using the
        //:   (untested) 'allocator' accessor, and verifying all allocations
        //:   are done from the allocator in future tests.  (C-1)
        //:
        //: 2 Create objects using the 'bslma::TestAllocator', use the primary
        //:   manipulator method with various values, and the (untested)
        //:   accessors to verify the value of the object.  Also vary the test
        //:   allocator's allocation limit to verify behavior in the presence
        //:   of exceptions.  (C-2..7)
        //:
        //: 3 Use a supplied 'bslma::TestAllocator' that goes out-of-scope
        //:   at the conclusion of each test to ensure all memory is returned
        //:   to the allocator.  (C-8)
        //:
        //: 4 Verify defensive checks are triggered for invalid values.  (C-9)
        //
        // Testing:
        //   PackedCalendar(bslma::Allocator *basicAllocator = 0);
        //   ~PackedCalendar();
        //   void addDay(const Date& date);
        //   void addHoliday(const Date& date);
        //   void addHolidayCode(const Date& date, int holidayCode);
        //   void addWeekendDay(DayOfWeek::Enum weekendDay);
        //   void addWeekendDaysTransition(date, weekendDays);
        //   void removeAll();
        // --------------------------------------------------------------------

        bslma::TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "PRIMARY MANIPULATORS TEST" << endl
                          << "=========================" << endl;

        if (verbose) cout << "\nTesting with various allocator configurations."
                          << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj mX;  const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
            ASSERT(0 == X.numHolidays());
            ASSERT(0 == X.numWeekendDaysTransitions());
            ASSERT(allocations == defaultAllocator.numAllocations());

            mX.addHoliday(bdlt::Date(2014, 12, 25));
            ASSERT(1 == X.numHolidays());
            ASSERT(0 == X.numWeekendDaysTransitions());
            ASSERT(allocations + 2 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            Obj        mX(reinterpret_cast<bslma::TestAllocator *>(0));
            const Obj& X = mX;
            ASSERT(&defaultAllocator == X.allocator());
            ASSERT(0 == X.numHolidays());
            ASSERT(0 == X.numWeekendDaysTransitions());
            ASSERT(allocations == defaultAllocator.numAllocations());

            mX.addHoliday(bdlt::Date(2014, 12, 25));
            ASSERT(1 == X.numHolidays());
            ASSERT(0 == X.numWeekendDaysTransitions());
            ASSERT(allocations + 2 == defaultAllocator.numAllocations());
        }
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            Obj mX(&sa);  const Obj& X = mX;
            ASSERT(&sa == X.allocator());
            ASSERT(0 == X.numHolidays());
            ASSERT(0 == X.numWeekendDaysTransitions());
            ASSERT(allocations == defaultAllocator.numAllocations());
            ASSERT(0 == sa.numAllocations());

            mX.addHoliday(bdlt::Date(2014, 12, 25));
            ASSERT(1 == X.numHolidays());
            ASSERT(0 == X.numWeekendDaysTransitions());
            ASSERT(allocations == defaultAllocator.numAllocations());
            ASSERT(2 == sa.numAllocations());
        }

        if (verbose) cout << "\nTesting 'addDay'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                Obj mX(&sa);  const Obj& X = mX;

                ASSERT(0 == X.isInRange(bdlt::Date(2000,1,1)));

                mX.addDay(bdlt::Date(2000,1,2));
                ASSERT(1 == X.length());
                ASSERT(0 == X.isInRange(bdlt::Date(2000,1,1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1,2)));
                ASSERT(0 == X.isInRange(bdlt::Date(2000,2,1)));
                ASSERT(0 == X.isInRange(bdlt::Date(2001,2,1)));

                mX.addDay(bdlt::Date(2000,1,2));
                ASSERT(1 == X.length());
                ASSERT(0 == X.isInRange(bdlt::Date(2000,1,1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1,2)));
                ASSERT(0 == X.isInRange(bdlt::Date(2000,2,1)));
                ASSERT(0 == X.isInRange(bdlt::Date(2001,2,1)));

                mX.addDay(bdlt::Date(2000,2,1));
                ASSERT(31 == X.length());
                ASSERT(0 == X.isInRange(bdlt::Date(2000,1,1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1,2)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,2,1)));
                ASSERT(0 == X.isInRange(bdlt::Date(2001,2,1)));

                mX.addDay(bdlt::Date(2000,1,1));
                ASSERT(32 == X.length());
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1,1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,2,1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,2,1)));
                ASSERT(0 == X.isInRange(bdlt::Date(2001,2,1)));

                mX.addDay(bdlt::Date(2001,2,1));
                ASSERT(398 == X.length());
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1,1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1,2)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,2,1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2001,2,1)));

                mX.addDay(bdlt::Date(2000,1,15));
                ASSERT(398 == X.length());
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1,1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1,2)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,2,1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2001,2,1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1,15)));
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nTesting 'addWeekendDay'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                Obj mX(&sa);  const Obj& X = mX;

                bdlt::DayOfWeekSet expected;
                ASSERT(0 == X.numWeekendDaysTransitions());

                mX.addWeekendDay(bdlt::DayOfWeek::e_SAT);
                expected.add(bdlt::DayOfWeek::e_SAT);
                ASSERT(1 == X.numWeekendDaysTransitions());
                ASSERT(bdlt::Date(1,1,1) ==
                                       X.beginWeekendDaysTransitions()->first);
                ASSERT(expected == X.beginWeekendDaysTransitions()->second);

                mX.addWeekendDay(bdlt::DayOfWeek::e_MON);
                expected.add(bdlt::DayOfWeek::e_MON);
                ASSERT(1 == X.numWeekendDaysTransitions());
                ASSERT(bdlt::Date(1,1,1) ==
                                       X.beginWeekendDaysTransitions()->first);
                ASSERT(expected == X.beginWeekendDaysTransitions()->second);

                mX.addWeekendDay(bdlt::DayOfWeek::e_SAT);
                ASSERT(1 == X.numWeekendDaysTransitions());
                ASSERT(bdlt::Date(1,1,1) ==
                                       X.beginWeekendDaysTransitions()->first);
                ASSERT(expected == X.beginWeekendDaysTransitions()->second);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nTesting 'addWeekendDaysTransition'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                Obj mX(&sa);  const Obj& X = mX;

                bdlt::Date         date;
                bdlt::DayOfWeekSet weekendDays;

                // default transition at 1/1/1
                Obj::WeekendDaysTransitionConstIterator iter =
                                               X.beginWeekendDaysTransitions();
                ASSERT(iter == X.endWeekendDaysTransitions());
                ASSERT(0 == X.numWeekendDaysTransitions());


                // new transition

                date.setYearMonthDay(2, 2, 2);
                weekendDays.removeAll();
                weekendDays.add(bdlt::DayOfWeek::e_MON);
                mX.addWeekendDaysTransition(date, weekendDays);

                iter = X.beginWeekendDaysTransitions();
                ASSERT(sameWeekendDaysTransition(*iter,
                                                 bdlt::Date(2,2,2), "m"));
                ASSERT(++iter == X.endWeekendDaysTransitions());

                // overwrite default transition

                date.setYearMonthDay(1, 1, 1);
                weekendDays.removeAll();
                weekendDays.add(bdlt::DayOfWeek::e_TUE);
                weekendDays.add(bdlt::DayOfWeek::e_SUN);;
                mX.addWeekendDaysTransition(date, weekendDays);

                iter = X.beginWeekendDaysTransitions();
                ASSERT(sameWeekendDaysTransition(*iter,
                                                 bdlt::Date(1,1,1), "tu"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(2,2,2), "m"));
                ASSERT(++iter == X.endWeekendDaysTransitions());

                // new transition

                date.setYearMonthDay(1, 2, 1);
                weekendDays.removeAll();
                weekendDays.add(bdlt::DayOfWeek::e_WED);
                weekendDays.add(bdlt::DayOfWeek::e_SUN);
                mX.addWeekendDaysTransition(date, weekendDays);

                iter = X.beginWeekendDaysTransitions();
                ASSERT(sameWeekendDaysTransition(*iter,
                                                 bdlt::Date(1,1,1), "tu"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(1,2,1), "wu"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(2,2,2), "m"));
                ASSERT(++iter == X.endWeekendDaysTransitions());

                // over-writing new transition

                date.setYearMonthDay(1, 2, 1);
                weekendDays.removeAll();
                weekendDays.add(bdlt::DayOfWeek::e_MON);
                weekendDays.add(bdlt::DayOfWeek::e_THU);
                mX.addWeekendDaysTransition(date, weekendDays);
                iter = X.beginWeekendDaysTransitions();
                ASSERT(sameWeekendDaysTransition(*iter,
                                                 bdlt::Date(1,1,1), "tu"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(1,2,1), "mr"));
                ASSERT(sameWeekendDaysTransition(*(++iter),
                                                 bdlt::Date(2,2,2), "m"));
                ASSERT(++iter == X.endWeekendDaysTransitions());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nTesting 'addHoliday'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                Obj mX(&sa);  const Obj& X = mX;

                ASSERT(0 == X.isInRange(bdlt::Date(1999,1, 1)));
                ASSERT(0 == X.isInRange(bdlt::Date(1999,7,15)));
                ASSERT(0 == X.isInRange(bdlt::Date(2000,1, 1)));
                ASSERT(0 == X.isInRange(bdlt::Date(2000,7,15)));
                ASSERT(0 == X.isInRange(bdlt::Date(2001,1, 1)));

                mX.addHoliday(bdlt::Date(2000,1,1));
                ASSERT(0 == X.isInRange(bdlt::Date(1999,1, 1)));
                ASSERT(0 == X.isInRange(bdlt::Date(1999,7,15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1, 1)));
                ASSERT(0 == X.isInRange(bdlt::Date(2000,7,15)));
                ASSERT(0 == X.isInRange(bdlt::Date(2001,1, 1)));
                Obj::HolidayConstIterator i = X.beginHolidays();
                ASSERT(i != X.endHolidays());
                ASSERT(bdlt::Date(2000,1,1) == *i);
                ASSERT(++i == X.endHolidays());

                mX.addHoliday(bdlt::Date(2000,1,1));
                ASSERT(0 == X.isInRange(bdlt::Date(1999,1, 1)));
                ASSERT(0 == X.isInRange(bdlt::Date(1999,7,15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1, 1)));
                ASSERT(0 == X.isInRange(bdlt::Date(2000,7,15)));
                ASSERT(0 == X.isInRange(bdlt::Date(2001,1, 1)));
                i = X.beginHolidays();
                ASSERT(i != X.endHolidays());
                ASSERT(2000 == i->year());
                ASSERT(bdlt::Date(2000,1,1) == *i);
                ASSERT(++i == X.endHolidays());

                mX.addHoliday(bdlt::Date(2001,1,1));
                ASSERT(367 == X.length());
                ASSERT(0 == X.isInRange(bdlt::Date(1999,1, 1)));
                ASSERT(0 == X.isInRange(bdlt::Date(1999,7,15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1, 1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,7,15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2001,1, 1)));
                i = X.beginHolidays();
                ASSERT(bdlt::Date(2000,1,1) == *i);
                ASSERT(bdlt::Date(2001,1,1) == *(++i));
                ASSERT(++i == X.endHolidays());

                mX.addHoliday(bdlt::Date(2000,7,15));
                ASSERT(367 == X.length());
                ASSERT(0 == X.isInRange(bdlt::Date(1999,1, 1)));
                ASSERT(0 == X.isInRange(bdlt::Date(1999,7,15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1, 1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,7,15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2001,1, 1)));
                i = X.beginHolidays();
                ASSERT(bdlt::Date(2000,1, 1) == *i);
                ASSERT(bdlt::Date(2000,7,15) == *(++i));
                ASSERT(bdlt::Date(2001,1, 1) == *(++i));
                ASSERT(++i == X.endHolidays());

                mX.addHoliday(bdlt::Date(2000,7,15));
                ASSERT(367 == X.length());
                ASSERT(0 == X.isInRange(bdlt::Date(1999,1, 1)));
                ASSERT(0 == X.isInRange(bdlt::Date(1999,7,15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1, 1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,7,15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2001,1, 1)));
                i = X.beginHolidays();
                ASSERT(bdlt::Date(2000,1, 1) == *i);
                ASSERT(bdlt::Date(2000,7,15) == *(++i));
                ASSERT(bdlt::Date(2001,1, 1) == *(++i));
                ASSERT(++i == X.endHolidays());

                mX.addHoliday(bdlt::Date(1999,1,1));
                ASSERT(732 == X.length());
                ASSERT(1 == X.isInRange(bdlt::Date(1999,1, 1)));
                ASSERT(1 == X.isInRange(bdlt::Date(1999,7,15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,1, 1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000,7,15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2001,1, 1)));
                i = X.beginHolidays();
                ASSERT(i != X.endHolidays());
                ASSERT(bdlt::Date(1999,1, 1) == *i);
                ASSERT(bdlt::Date(2000,1, 1) == *(++i));
                ASSERT(bdlt::Date(2000,7,15) == *(++i));
                ASSERT(bdlt::Date(2001,1, 1) == *(++i));
                ASSERT(++i == X.endHolidays());
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nTesting 'addHolidayCode'." << endl;
        {
            bsls::Types::Int64 allocations = defaultAllocator.numAllocations();

            bslma::TestAllocator sa("supplied", veryVeryVeryVerbose);

            BSLMA_TESTALLOCATOR_EXCEPTION_TEST_BEGIN(sa) {
                Obj mX(&sa);  const Obj& X = mX;

                ASSERT(0 == X.isInRange(bdlt::Date(2000,1, 1)));

                mX.addHoliday(bdlt::Date(2000,1,1));
                mX.addHolidayCode(bdlt::Date(2000,1,1), 1);
                Obj::HolidayCodeConstIterator k = X.beginHolidayCodes(
                                                         bdlt::Date(2000,1,1));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
                ASSERT(1 == *k);
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);

                mX.addHolidayCode(bdlt::Date(2000,1,1), 1);
                k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
                ASSERT(1 == *k);
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);

                mX.addHolidayCode(bdlt::Date(2000,1,1), 2);
                k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
                ASSERT(1 == *k);
                ASSERT(2 == *(++k));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);

                mX.addHolidayCode(bdlt::Date(2000,1,1), 2);
                k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
                ASSERT(1 == *k);
                ASSERT(2 == *(++k));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);

                mX.addHolidayCode(bdlt::Date(2000,2,1), 3);
                ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000, 2, 1)));
                Obj::HolidayConstIterator i = X.beginHolidays();
                ASSERT(bdlt::Date(2000,2,1) == *(++i));
                k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
                ASSERT(1 == *k);
                ASSERT(2 == *(++k));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);
                k = X.beginHolidayCodes(bdlt::Date(2000,2,1));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) != k);
                ASSERT(3 == *k);
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) == ++k);

                mX.addHolidayCode(bdlt::Date(1999,1,1), 1);
                mX.addHolidayCode(bdlt::Date(1999,1,1), 1);
                mX.addHolidayCode(bdlt::Date(1999,1,1), 2);
                mX.addHolidayCode(bdlt::Date(1999,1,1), 2);
                mX.addHolidayCode(bdlt::Date(1999,1,1), 3);
                mX.addHolidayCode(bdlt::Date(1999,1,1), 3);
                ASSERT(1 == X.isInRange(bdlt::Date(1999, 1, 1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000, 2, 1)));
                i = X.beginHolidays();
                ASSERT(bdlt::Date(1999,1,1) == *i);
                k = X.beginHolidayCodes(bdlt::Date(1999,1,1));
                ASSERT(1 == *k);
                ASSERT(2 == *(++k));
                ASSERT(3 == *(++k));
                ASSERT(X.endHolidayCodes(bdlt::Date(1999,1,1)) == ++k);
                k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
                ASSERT(1 == *k);
                ASSERT(2 == *(++k));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);
                k = X.beginHolidayCodes(bdlt::Date(2000,2,1));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) != k);
                ASSERT(3 == *k);
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) == ++k);

                mX.addHolidayCode(bdlt::Date(1999,1,1), INT_MAX);
                mX.addHolidayCode(bdlt::Date(1999,1,1), INT_MIN);
                ASSERT(1 == X.isInRange(bdlt::Date(1999, 1, 1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 1)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 15)));
                ASSERT(1 == X.isInRange(bdlt::Date(2000, 2, 1)));
                i = X.beginHolidays();
                ASSERT(bdlt::Date(1999,1,1) == *i);
                k = X.beginHolidayCodes(bdlt::Date(1999,1,1));
                ASSERT(INT_MIN == *k);
                ASSERT(1       == *(++k));
                ASSERT(2       == *(++k));
                ASSERT(3       == *(++k));
                ASSERT(INT_MAX == *(++k));
                ASSERT(X.endHolidayCodes(bdlt::Date(1999,1,1)) == ++k);
                k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
                ASSERT(1 == *k);
                ASSERT(2 == *(++k));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);
                k = X.beginHolidayCodes(bdlt::Date(2000,2,1));
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) != k);
                ASSERT(3 == *k);
                ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) == ++k);
            } BSLMA_TESTALLOCATOR_EXCEPTION_TEST_END

            ASSERT(allocations == defaultAllocator.numAllocations());
        }

        if (verbose) cout << "\nTesting 'removeAll'." << endl;
        {
            Obj mX;  const Obj& X = mX;
            ASSERT(0 == X.isInRange(bdlt::Date(2000,1, 1)));

            mX.addHoliday(bdlt::Date(2000,1,1));
            mX.addHolidayCode(bdlt::Date(2000,1,1), 1);
            Obj::HolidayCodeConstIterator k = X.beginHolidayCodes(
                                                 bdlt::Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdlt::Date(2000,1,1), 1);
            k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdlt::Date(2000,1,1), 2);
            k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdlt::Date(2000,1,1), 2);
            k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdlt::Date(2000,2,1), 3);
            ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 15)));
            ASSERT(1 == X.isInRange(bdlt::Date(2000, 2, 1)));
            Obj::HolidayConstIterator i = X.beginHolidays();
            ASSERT(bdlt::Date(2000,2,1) == *(++i));
            k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);
            k = X.beginHolidayCodes(bdlt::Date(2000,2,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) != k);
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) == ++k);

            mX.addHolidayCode(bdlt::Date(1999,1,1), 3);
            ASSERT(1 == X.isInRange(bdlt::Date(1999, 1, 1)));
            ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 15)));
            ASSERT(1 == X.isInRange(bdlt::Date(2000, 2, 1)));
            i = X.beginHolidays();
            ASSERT(bdlt::Date(1999,1,1) == *i);
            k = X.beginHolidayCodes(bdlt::Date(1999,1,1));
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdlt::Date(1999,1,1)) == ++k);
            k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);
            k = X.beginHolidayCodes(bdlt::Date(2000,2,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) != k);
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) == ++k);

            mX.removeAll();
            ASSERT(0 == X.isInRange(bdlt::Date(1999, 1, 1)));
            ASSERT(0 == X.isInRange(bdlt::Date(2000, 1, 1)));
            ASSERT(0 == X.isInRange(bdlt::Date(2000, 1, 15)));
            ASSERT(0 == X.isInRange(bdlt::Date(2000, 2, 1)));
            ASSERT(X.endHolidays() == X.beginHolidays());

            mX.addHoliday(bdlt::Date(2000,1,1));
            mX.addHolidayCode(bdlt::Date(2000,1,1), 1);
            k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdlt::Date(2000,1,1), 1);
            k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdlt::Date(2000,1,1), 2);
            k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdlt::Date(2000,1,1), 2);
            k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);

            mX.addHolidayCode(bdlt::Date(2000,2,1), 3);
            ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 15)));
            ASSERT(1 == X.isInRange(bdlt::Date(2000, 2, 1)));
            i = X.beginHolidays();
            ASSERT(bdlt::Date(2000,2,1) == *(++i));
            k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);
            k = X.beginHolidayCodes(bdlt::Date(2000,2,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) != k);
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) == ++k);

            mX.addHolidayCode(bdlt::Date(1999,1,1), 3);
            ASSERT(1 == X.isInRange(bdlt::Date(1999, 1, 1)));
            ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdlt::Date(2000, 1, 15)));
            ASSERT(1 == X.isInRange(bdlt::Date(2000, 2, 1)));
            i = X.beginHolidays();
            ASSERT(bdlt::Date(1999,1,1) == *i);
            k = X.beginHolidayCodes(bdlt::Date(1999,1,1));
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdlt::Date(1999,1,1)) == ++k);
            k = X.beginHolidayCodes(bdlt::Date(2000,1,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) != k);
            ASSERT(1 == *k);
            ASSERT(2 == *(++k));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,1,1)) == ++k);
            k = X.beginHolidayCodes(bdlt::Date(2000,2,1));
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) != k);
            ASSERT(3 == *k);
            ASSERT(X.endHolidayCodes(bdlt::Date(2000,2,1)) == ++k);
        }

        if (verbose) cout << "\nNegative Testing." << endl;
        {
            bsls::AssertFailureHandlerGuard hG(
                                             bsls::AssertTest::failTestDriver);

            {
                Obj mX;

                ASSERT_SAFE_PASS(mX.addWeekendDay(bdlt::DayOfWeek::e_SUN));
            }

            {
                Obj mX;

                bdlt::DayOfWeekSet wd;
                wd.add(bdlt::DayOfWeek::e_SUN);
                wd.add(bdlt::DayOfWeek::e_SAT);

                mX.addWeekendDaysTransition(bdlt::Date(2012, 3, 1), wd);

                ASSERT_SAFE_FAIL(mX.addWeekendDay(bdlt::DayOfWeek::e_SUN));
            }
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
        //: 1 Create four objects using both the default and copy constructors.
        //:
        //: 2 Exercise these objects using primary manipulators, basic
        //:   accessors, equality operators, and the assignment operator.
        //:
        //: 3 Verify expected values throughout.  (C-1)
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        const bdlt::Date VA(2000,  1,  1);
        const bdlt::Date VX(2000,  1,  2);
        const bdlt::Date VB(2000,  1,  3);

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
        if (verbose) cout << "\n 1. Create an object x1 (default ctor).       "
                             "          { x1: }" << endl;
        Obj mX1; const Obj& X1 = mX1;
        if (verbose) { cout << '\t'; P(X1); }

        if (verbose) cout <<
            "\ta. Check initial state of x1." << endl;
        ASSERT( 0 == X1.length());
        ASSERT( 0 == X1.numHolidays());

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
        //  - void addDay(const Date& date);
        //  - void addHoliday(const Date& date);
        //  - int addHolidayIfInRange(const Date& date);
        //  - void addHolidayCode(const Date& date, int holidayCode);
        //
        //  ACCESSORS
        //  - const Date& firstDate() const;
        //  - const Date& lastDate() const;
        //  - bool isInRange(const Date& date) const;
        //  - bool isBusinessDay(const Date& date) const;
        //  - bool isNonBusinessDay(const Date& date) const;
        //  - bool isHoliday(const Date& date) const;
        //  - bool isWeekendDay(const Date& date) const;
        //  - bool isWeekendDay(bdlt::DayOfWeek::Enum dayOfWeek) const;
        //  - int length() const;
        //  - int numBusinessDays() const;
        //  - int numNonBusinessDays() const;
        //  - int numHolidays() const;
        //  - int numHolidayCodes(const Date& date) const;
        //  - int numWeekendDaysInRange() const;
        //  - ostream& print(ostream& stream, int lev = 0, int spl = 4) const;
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

        ASSERT( 0 == cal.length());
        ASSERT( 0 == cal.numBusinessDays());
        ASSERT( 0 == cal.numNonBusinessDays());
        ASSERT( 0 == cal.numHolidays());
        ASSERT( 0 == cal.numWeekendDaysInRange());
        ASSERT( 0 == cal.numWeekendDaysTransitions());

        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_WED));
        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_SAT));

        ASSERT( 0 == cal.isInRange(bdlt::Date(   1, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(1999,12,31)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(9999,12,31)));

        // --------------------------------------------------------------------

        if (verbose) cout << "\nAdd weekend day: Sunday" << endl;
        cal.addWeekendDay(bdlt::DayOfWeek::e_SAT);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //         S  M Tu  W Th  F (S)

        ASSERT( 0 == cal.length());
        LOOP_ASSERT(cal.numBusinessDays(), 0 == cal.numBusinessDays());
        ASSERT( 0 == cal.numNonBusinessDays());
        ASSERT( 0 == cal.numHolidays());
        ASSERT( 0 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT(bdlt::Date(1,1,1) == cal.beginWeekendDaysTransitions()->first);

        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_WED));
        ASSERT( 1 == cal.isWeekendDay(bdlt::DayOfWeek::e_SAT));

        ASSERT( 0 == cal.isInRange(bdlt::Date(   1, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(1999,12,31)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(9999,12,31)));

        // --------------------------------------------------------------------

        if (verbose) cout << "\nAdd Day: Jan 1, 2000." << endl;
        cal.addDay(bdlt::Date(2000, 1, 1));
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //         S  M Tu  W Th  F (S)
        //                           1

        ASSERT(bdlt::Date(2000, 1, 1) == cal.firstDate());
        ASSERT(bdlt::Date(2000, 1, 1) == cal.lastDate());
        ASSERT( 1 == cal.length());
        ASSERT( 0 == cal.numBusinessDays());
        ASSERT( 1 == cal.numNonBusinessDays());
        ASSERT( 0 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_WED));
        ASSERT( 1 == cal.isWeekendDay(bdlt::DayOfWeek::e_SAT));

        ASSERT( 0 == cal.isInRange(bdlt::Date(   1, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(9999,12,31)));

        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 1)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 1)));
        ASSERT( 1 == cal.isWeekendDay(    bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 1)));

        // --------------------------------------------------------------------

        if (verbose) cout << "\nAdd Holiday: Jan 4, 2000." << endl;
        cal.addHoliday(bdlt::Date(2000, 1, 4));
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //         S  M Tu  W Th  F (S)
        //                           1
        //         2  3 (4)

        ASSERT(bdlt::Date(2000, 1, 1) == cal.firstDate());
        ASSERT(bdlt::Date(2000, 1, 4) == cal.lastDate());
        ASSERT( 4 == cal.length());
        ASSERT( 2 == cal.numBusinessDays());
        ASSERT( 2 == cal.numNonBusinessDays());
        ASSERT( 1 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_WED));
        ASSERT( 1 == cal.isWeekendDay(bdlt::DayOfWeek::e_SAT));

        ASSERT( 0 == cal.isInRange(bdlt::Date(   1, 1, 1)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(2000, 1, 1)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(2000, 1, 2)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 5)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(9999,12,31)));

        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 1)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.isWeekendDay(    bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 4)));

        if (verbose) cout << "\nAdd Day: Dec 30, 1999." << endl;
        cal.addDay(bdlt::Date(1999, 12, 30));
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //         S  M Tu  W Th  F (S)
        //                    30 31  1
        //         2  3 (4)

        ASSERT(bdlt::Date(1999,12,30) == cal.firstDate());
        ASSERT(bdlt::Date(2000, 1, 4) == cal.lastDate());
        ASSERT( 6 == cal.length());
        ASSERT( 4 == cal.numBusinessDays());
        ASSERT( 2 == cal.numNonBusinessDays());
        ASSERT( 1 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_WED));
        ASSERT( 1 == cal.isWeekendDay(bdlt::DayOfWeek::e_SAT));

        ASSERT( 0 == cal.isInRange(bdlt::Date(1999,12,29)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(1999,12,30)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 5)));

        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,30)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,31)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 1)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,31)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isWeekendDay(    bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,31)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 4)));

        if (verbose) cout << "\nAdd Holiday Code: Jan 1, 2000, 123." << endl;
        cal.addHolidayCode(bdlt::Date(2000, 1, 1), 123);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //         S  M Tu  W Th  F (S) ,123
        //                    30 31 (1)'
        //         2  3 (4)

        ASSERT(bdlt::Date(1999,12,30) == cal.firstDate());
        ASSERT(bdlt::Date(2000, 1, 4) == cal.lastDate());
        ASSERT( 6 == cal.length());
        ASSERT( 4 == cal.numBusinessDays());
        ASSERT( 2 == cal.numNonBusinessDays());
        ASSERT( 2 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_WED));
        ASSERT( 1 == cal.isWeekendDay(bdlt::DayOfWeek::e_SAT));

        ASSERT( 0 == cal.isInRange(bdlt::Date(1999,12,29)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(1999,12,30)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 5)));

        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,30)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,31)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 1)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isHoliday(       bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isWeekendDay(    bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.numHolidayCodes( bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 4)));

        if (verbose) cout << "\nAdd Holiday Code: Dec 26, 1999, 45." << endl;
        cal.addHolidayCode(bdlt::Date(1999, 12, 26), 45);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //   45.   S  M Tu  W Th  F (S) ,123
        //      `(26)27 28 29 30 31 (1)'
        //         2  3 (4)

        ASSERT(bdlt::Date(1999,12,26) == cal.firstDate());
        ASSERT(bdlt::Date(2000, 1, 4) == cal.lastDate());
        ASSERT(10 == cal.length());
        ASSERT( 7 == cal.numBusinessDays());
        ASSERT( 3 == cal.numNonBusinessDays());
        ASSERT( 3 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_WED));
        ASSERT( 1 == cal.isWeekendDay(bdlt::DayOfWeek::e_SAT));

        ASSERT( 0 == cal.isInRange(bdlt::Date(1999,12,25)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(1999,12,26)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 5)));

        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(1999,12,26)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,27)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,28)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,29)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,30)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,31)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 1)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.isHoliday(       bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isHoliday(       bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isWeekendDay(    bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.numHolidayCodes( bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.numHolidayCodes( bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 4)));

        if (verbose) cout << "\nAdd Holiday Code: Jan 1, 2000, 6." << endl;
        cal.addHolidayCode(bdlt::Date(2000, 1, 1), 6);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //   45.   S  M Tu  W Th  F (S) ,123,6
        //      `(26)27 28 29 30 31 (1)'
        //         2  3 (4)

        ASSERT(bdlt::Date(1999,12,26) == cal.firstDate());
        ASSERT(bdlt::Date(2000, 1, 4) == cal.lastDate());
        ASSERT(10 == cal.length());
        ASSERT( 7 == cal.numBusinessDays());
        ASSERT( 3 == cal.numNonBusinessDays());
        ASSERT( 3 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_WED));
        ASSERT( 1 == cal.isWeekendDay(bdlt::DayOfWeek::e_SAT));

        ASSERT( 0 == cal.isInRange(bdlt::Date(1999,12,25)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(1999,12,26)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 5)));

        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(1999,12,26)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,27)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,28)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,29)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,30)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,31)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 1)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.isHoliday(       bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isHoliday(       bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isWeekendDay(    bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.numHolidayCodes( bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,31)));
        ASSERT( 2 == cal.numHolidayCodes( bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 4)));

        if (verbose) cout << "\nAdd Holiday Code: Jan 1, 2000, 6. (again)"
                                                                       << endl;
        cal.addHolidayCode(bdlt::Date(2000, 1, 1), 6);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //   45.   S  M Tu  W Th  F (S) ,123,6!
        //      `(26)27 28 29 30 31 (1)'
        //         2  3 (4)

        ASSERT(bdlt::Date(1999,12,26) == cal.firstDate());
        ASSERT(bdlt::Date(2000, 1, 4) == cal.lastDate());
        ASSERT(10 == cal.length());
        ASSERT( 7 == cal.numBusinessDays());
        ASSERT( 3 == cal.numNonBusinessDays());
        ASSERT( 3 == cal.numHolidays());
        ASSERT( 1 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 1 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_WED));
        ASSERT( 1 == cal.isWeekendDay(bdlt::DayOfWeek::e_SAT));

        ASSERT( 0 == cal.isInRange(bdlt::Date(1999,12,25)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(1999,12,26)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 5)));

        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(1999,12,26)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,27)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,28)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,29)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,30)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,31)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 1)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.isHoliday(       bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isHoliday(       bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdlt::Date(2000, 1, 4)));

        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isWeekendDay(    bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.numHolidayCodes( bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,31)));

        ASSERT( 2 == cal.numHolidayCodes( bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 4)));

        if (verbose) cout << "\nAdd Weekend Day: Sunday." << endl;

        cal.addWeekendDay(bdlt::DayOfWeek::e_SUN);
        if (verbose) cout << cal << endl;
        if (veryVerbose) cal.print(cout) << endl;

        //          Dec 1999/Jan 2000
        //   45.  (S) M Tu  W Th  F (S) ,123,6!
        //      `(26)27 28 29 30 31 (1)'
        //         2  3 (4)

        ASSERT(bdlt::Date(1999,12,26) == cal.firstDate());
        ASSERT(bdlt::Date(2000, 1, 4) == cal.lastDate());
        ASSERT(10 == cal.length());
        ASSERT( 6 == cal.numBusinessDays());
        ASSERT( 4 == cal.numNonBusinessDays());
        ASSERT( 3 == cal.numHolidays());
        ASSERT( 3 == cal.numWeekendDaysInRange());
        ASSERT( 1 == cal.numWeekendDaysTransitions());
        ASSERT( 2 == numWeekendDaysInFirstTransition(cal));

        ASSERT( 1 == cal.isWeekendDay(bdlt::DayOfWeek::e_SUN));
        ASSERT( 0 == cal.isWeekendDay(bdlt::DayOfWeek::e_WED));
        ASSERT( 1 == cal.isWeekendDay(bdlt::DayOfWeek::e_SAT));

        ASSERT( 0 == cal.isInRange(bdlt::Date(1999,12,25)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(1999,12,26)));
        ASSERT( 1 == cal.isInRange(bdlt::Date(2000, 1, 4)));
        ASSERT( 0 == cal.isInRange(bdlt::Date(2000, 1, 5)));

        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(1999,12,26)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,27)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,28)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,29)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,30)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(1999,12,31)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 2)));
        ASSERT( 1 == cal.isBusinessDay(   bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isBusinessDay(   bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 1)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isNonBusinessDay(bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.isHoliday(       bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isHoliday(       bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isHoliday(       bdlt::Date(2000, 1, 3)));
        ASSERT( 1 == cal.isHoliday(       bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.isWeekendDay(    bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(1999,12,31)));
        ASSERT( 1 == cal.isWeekendDay(    bdlt::Date(2000, 1, 1)));
        ASSERT( 1 == cal.isWeekendDay(    bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.isWeekendDay(    bdlt::Date(2000, 1, 4)));

        ASSERT( 1 == cal.numHolidayCodes( bdlt::Date(1999,12,26)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,27)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,28)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,29)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,30)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(1999,12,31)));

        ASSERT( 2 == cal.numHolidayCodes( bdlt::Date(2000, 1, 1)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 2)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 3)));
        ASSERT( 0 == cal.numHolidayCodes( bdlt::Date(2000, 1, 4)));
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
