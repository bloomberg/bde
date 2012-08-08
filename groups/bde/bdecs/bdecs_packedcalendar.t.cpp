// bdecs_packedcalendar.t.cpp                                         -*-C++-*-

#include <bdecs_packedcalendar.h>

#include <bdema_bufferedsequentialallocator.h>  // for testing only
#include <bdex_instreamfunctions.h>             // for testing only
#include <bdex_outstreamfunctions.h>            // for testing only
#include <bdex_testoutstream.h>                 // for testing only
#include <bdex_testinstream.h>                  // for testing only
#include <bdex_testinstreamexception.h>         // for testing only

#include <bslma_defaultallocatorguard.h>        // for testing only
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cctype.h>      // isdigit() isupper() islower()
#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_vector.h>

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
// A 'bdecs_PackedCalendar' is a value-semantic type that represents a
// collection of weekend days and holidays over a given range of dates, and
// with each holiday associated with a possibly empty list of unique (integer)
// holiday codes).  Two calendars have the same 'value' if they (1) designate
// the same set of days as "weekend" days, (2) have the same valid range,
// (3) have the same days within that range designated as holidays, and (4)
// for each holiday, have the same set of associated integer codes.
//
// We have chosen the primary manipulators for 'bdecs_PackedCalendar' to be
// 'addDay', 'addHoliday', 'addHolidayCode', 'addWeekendDaysTransition', and
// 'removeAll'.
//
// We have chosen the basic accessors for 'bdecs_PackedCalendar' to be
// 'length', 'isInRange', 'firstDate', 'lastDate', 'isHoliday', and all the
// iterators except the business day iterators.
// --------------------------------------------------------------------------
// ITERATORS [16] BusinessDayConstIterator; [ 4]
// WeekendDaysTransitionConstIterator; [ 4] HolidayConstIterator; [ 4]
// HolidayCodeConstIterator; [16] BusinessDayConstReverseIterator; [ 4]
// HolidayConstReverseIterator; [ 4] HolidayCodeConstReverseIterator;
//
// CLASS METHODS
// [10] static int maxSupportedBdexVersion();
//
// CREATORS
// [ 2] bdecs_PackedCalendar(bslma_Allocator *basicAllocator = 0);
// [11] bdecs_PackedCalendar(const bdet_Date& firstDate, lastDate, ba = 0);
// [ 7] bdecs_PackedCalendar(const bdecs_PackedCalendar& original, ba = 0);
// [ 2] ~bdecs_PackedCalendar();
//
// MANIPULATORS
// [ 9] bdecs_PackedCalendar& operator=(const bdecs_PackedCalendar& rhs);
// [17] void setValidRange(const bdet_Date& fd, const bdet_Date& ld);
// [ 2] void addDay(const bdet_Date& date);
// [ 2] void addHoliday(const bdet_Date& date);
// [18] int addHolidayIfInRange(const bdet_Date& date);
// [ 2] void addHolidayCode(const bdet_Date& date, int holidayCode);
// [18] int addHolidayCodeIfInRange(const bdet_Date& date, int holidayCode);
// [12] void addWeekendDay(bdet_DayOfWeek::Day weekendDay);
// [12] void addWeekendDays(const bdec_DayOfWeekSet& weekendDays);
// [ 2] void addWeekendDaysTransition(date, weekendDays);
// [20] void intersectBusinessDays(const bdecs_PackedCalendar& calendar);
// [21] void intersectNonBusinessDays(const bdecs_PackedCalendar& calendar);
// [21] void unionBusinessDays(const bdecs_PackedCalendar& calendar);
// [20] void unionNonBusinessDays(const bdecs_PackedCalendar& calendar);
// [19] void removeHoliday(const bdet_Date& date);
// [19] void removeHolidayCode(const bdet_Date& date, int holidayCode);
// [ 2] void removeAll();
// [22] void swap(bdecs_PackedCalendar *rhs);
// [22] void swap(bdecs_PackedCalendar& other);
// [22] void swap(bdecs_PackedCalendar& lhs, bdecs_PackedCalendar& rhs);
// [10] template <class STREAM> STREAM& bdexStreamIn(STREAM& s, int version);
//
// ACCESSORS
// [16] BusinessDayConstIterator beginBusinessDays() const;
// [16] BusinessDayConstIterator beginBusinessDays(const bdet_Date&) const;
// [ 4] HolidayConstIterator beginHolidays() const;
// [ 4] HolidayConstIterator beginHolidays(const bdet_Date& date) const;
// [ 4] HolidayCodeConstIterator beginHolidayCodes(const HCI&) const;
// [ 4] HolidayCodeConstIterator beginHolidayCodes(const bdet_Date&) const;
// [16] BusinessDayConstIterator rbeginBusinessDays() const;
// [16] BusinessDayConstIterator rbeginBusinessDays(const bdet_Date&) const;
// [ 4] HolidayConstIterator rbeginHolidays() const;
// [ 4] HolidayConstIterator rbeginHolidays(const bdet_Date& date) const;
// [ 4] HolidayCodeConstIterator rbeginHolidayCodes(const HCI&) const;
// [ 4] HolidayCodeConstIterator rbeginHolidayCodes(const bdet_Date&) const;
// [ 4] const bdet_Date& firstDate() const;
// [ 4] const bdet_Date& lastDate() const;
// [16] BusinessDayConstIterator endBusinessDays() const;
// [16] BusinessDayConstIterator endBusinessDays(const bdet_Date& date) const;
// [ 4] HolidayConstIterator endHolidays() const;
// [ 4] HolidayConstIterator endHolidays(const bdet_Date& date) const;
// [ 4] HolidayCodeConstIterator endHolidayCodes(const HCI&) const;
// [ 4] HolidayCodeConstIterator endHolidayCodes(const bdet_Date& date) const;
// [16] BusinessDayConstIterator rendBusinessDays() const;
// [16] BusinessDayConstIterator rendBusinessDays(const bdet_Date& date) const;
// [ 4] HolidayConstIterator rendHolidays() const;
// [ 4] HolidayConstIterator rendHolidays(const bdet_Date& date) const;
// [ 4] HolidayCodeConstIterator rendHolidayCodes(const HCI&) const;
// [ 4] HolidayCodeConstIterator rendHolidayCodes(const bdet_Date& date) const;
// [ 4] bool isInRange(const bdet_Date& date) const;
// [15] bool isBusinessDay(const bdet_Date& date) const;
// [15] bool isNonBusinessDay(const bdet_Date& date) const;
// [ 4] bool isHoliday(const bdet_Date& date) const;
// [15] bool isWeekendDay(const bdet_Date& date) const;
// [ 4] bool isWeekendDay(bdet_DayOfWeek::Day dayOfWeek) const;
// [ 4] int length() const;
// [15] int numBusinessDays() const;
// [15] int numNonBusinessDays() const;
// [14] int numHolidays() const;
// [14] int numHolidayCodes(const bdet_Date& date) const;
// [13] int numWeekendDaysInRange() const;
// [ 4] WeekendDaysTransitionConstIterator beginWeekendDaysTransitions() const;
// [ 4] WeekendDaysTransitionConstIterator endWeekendDaysTransitions() const;
// [ 4] int numWeekendDaysTransitions() const;
// [10] template <class STREAM> STREAM& bdexStreamOut(STREAM&, int) const;
// [ 5] ostream& print(ostream& stream, int level = 0, int spl = 4) const;
//
// FREE OPERATORS
// [ 6] operator==(const bdecs_PackedCalendar& lhs, rhs);
// [ 6] operator!=(const bdecs_PackedCalendar& lhs, rhs);
// [ 5] operator<<(ostream&, const bdecs_PackedCalendar&);
//--------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [23] USAGE EXAMPLE
// [ 3] PRIMITIVE GENERATOR FUNCTION 'ggg'
// [ 8] GENERATOR FUNCTION 'g'
//=============================================================================

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

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_FAIL(expr) BSLS_ASSERTTEST_ASSERT_FAIL(expr)
#define ASSERT_PASS(expr) BSLS_ASSERTTEST_ASSERT_PASS(expr)
#define ASSERT_SAFE_FAIL(expr) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(expr)
#define ASSERT_SAFE_PASS(expr) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(expr)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef bdecs_PackedCalendar Obj;

//=============================================================================
//                          TEST UTILITY FUNCTIONS
// ----------------------------------------------------------------------------

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


namespace {

int numWeekendDaysInFirstTransition(const Obj& calendar)
    // Return the number of weekend days in the first weekend-days transition
    // of the specified 'calendar'.
{
    return calendar.beginWeekendDaysTransitions()->second.length();
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
//              SIMPLE GENERATOR FUNCTIONS 'h' and 'hh' FOR TESTING
//-----------------------------------------------------------------------------
// The following functions interpret the given 'spec' in order from left to
// right according to a simple custom language.
//
// In this simple approach, a handful of dates and integer indices will be
// chosen arbitrarily and mapped, respectively, to lower- and uppercase
// letters.  Holiday codes will be associated with the most recently specified
// date.  For example, let's assume the following arbitrary assignments of
// holiday codes:
//
//     int VA = 0, VB = 1, VC = 2, VD = 100, VE = 1000;
//
// In order to specify weekend days, the decimal digits 1-7 enumerate the days
// of the week beginning with SUNDAY = 1.  The first two lowercase characters
// of the spec, if present, will represent the boundary dates of the valid
// range, any additional lowercase letters will represent holidays within that
// range.  To aid readability, the lexicographic order of lowercase letters
// will coincide with the chronological order of dates:
//
//  a = 2000/1/1, b = 2000/1/2, c = 2000/1/3, d = 2000/2/29, e = 9999/12/31.
//
// Illustrative examples of this concise notation have the following meanings:
//
//   Spec        Description
//   ----        -----------
//   ""          Calendar is Empty (no weekend days, empty range).
//
//   "2"         [ MON ] is a weekend day, empty range.
//
//   "17"        [ SUN SAT ] are weekend days, empty range.
//
//   "a"         Range is 2000/1/1..2000/1/1, no holidays/weekend days.
//
//   "ad"        Range is 2000/1/1..2000/2/29, no holidays/weekend days.
//
//   "da"        Ill-formed: range top 'a' is less than range bottom ('d').
//
//   "ada"       Range is 2000/1/1..2000/2/29, 2000/1/1 is a holiday.
//
//   "adbd"      Range is 2000/1/1..2000/2/29, 2000/1/2 and 2000/2/29 are
//               holidays.
//
//   "adbd4a"    [ WED ] is a weekend day, Range is 2000/1/1..2000/2/29,
//               2000/1/1, 2000/1/2, and 2000/2/29 are holidays,
//
//   "abbd4a"    Ill-formed: holiday ('d') out of range ('a'..'b').
//
//   "adaA"      Range is 2000/1/1..2000/2/29, 2000/1/1{0} is a holiday with
//               associated holiday code 0.
//
//   "adaBC"     Range is 2000/1/1..2000/2/29, 2000/1/1{1,2} is a holiday with
//               associated holiday codes 1 and 2.
//
//   "aCdaB"     Same as above.
//
//   "adabCd"    Range is 2000/1/1..2000/2/29, holidays/codes are 2000/1/1,
//               2000/1/2{2}, 2000/2/29.
//
//   "21aebAcBCdA"
//               Weekend days are [ SUN MON ], Range is 2000/1/1..9999/12/31,
//               holidays/codes 2000/1/2{0}, 2000/1/3{1,2}), and 2000/2/29{0}
//
// A straight-forward implementation of a generator accepting this notation
// follows:

int VA = 0, VB = 1, VC = 2, VD = 100, VE = 1000; // Holiday codes.
const int HOLIDAY_CODES[] = {VA, VB, VC, VD, VE};
const int NUM_HOLIDAY_CODES = sizeof HOLIDAY_CODES / sizeof *HOLIDAY_CODES;

// Example dates

const bdet_Date Va(2000,  1,  1);
const bdet_Date Vb(2000,  1,  2);
const bdet_Date Vc(2000,  1,  3);
const bdet_Date Vd(2000,  2, 29);
const bdet_Date Ve(9999, 12, 31);

const bdet_Date *DAYS[] = { &Va, &Vb, &Vc, &Vd, &Ve };
const int NUM_DAYS = sizeof DAYS / sizeof *DAYS;

bdecs_PackedCalendar& hh(bdecs_PackedCalendar *object, const char *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec' according to the custom language
    // described above (in the test driver).
{

    ASSERT(object);
    ASSERT(spec);

    bdet_Date lastDate;
    int numDays = 0;

    for (const char *input = spec; *input; ++input) {
        if (isdigit(*input)) {                                // WEEKEND DAYS
            int idx = *input - '0';
            LOOP3_ASSERT(spec, input, idx, bdet_DayOfWeek::BDET_SUN <= idx);
            LOOP3_ASSERT(spec, input, idx, idx <= bdet_DayOfWeek::BDET_SAT);
            object->addWeekendDay(static_cast<bdet_DayOfWeek::Day>(idx));
        }
        else if (islower(*input)) {                        // DAYS/HOLIDAYS
            int idx = *input - 'a';
            LOOP3_ASSERT(spec, input, idx, 0 <= idx);
            LOOP3_ASSERT(spec, input, idx, idx < NUM_DAYS);
            lastDate = *DAYS[idx];
            if (numDays < 2) {
                object->addDay(lastDate);
            }
            else {
                LOOP3_ASSERT(spec, input, idx, object->isInRange(lastDate));
                object->addHoliday(lastDate);
            }
            ++numDays;
        }
        else if (isupper(*input)) {                        // HOLIDAY CODES
            int idx = *input - 'A';
            LOOP3_ASSERT(spec, input, idx, 0 <= idx);
            LOOP3_ASSERT(spec, input, idx, idx < NUM_HOLIDAY_CODES);
            LOOP3_ASSERT(spec, input, idx, numDays > 0);
            object->addHolidayCode(lastDate, HOLIDAY_CODES[idx]);
        }
        else if ('~' == *input) { // Undocumented Feature!
            object->removeAll();
        }
        else {
            LOOP2_ASSERT(spec, input, "Unrecognized Character" && 0);
        }

    } // end while not done

    return *object;
}

// Since the dates and indices are arbitrary, we are free to choose them in any
// way that suites our needs.  We might, for example choose 'a' to represent
// the earliest valid date (Saturday? 0001/01/01) and 'z' the latest one
// (Sunday? 9999/12/31).  In practice, however, tests that require specific
// values for tersely named symbolic elements such as (e.g., a, b, c) are
// highly unreadable and best avoided.  When specific values are relevant, it
// is preferable notation itself should accommodate them.
//
// The degree to which the assumption that the specific values for dates and
// indices are arbitrary and that only the relative values of dates matters
// will clearly depend on the implementation.  For most basic value-semantic
// operations, this assumption will turn out to be true even for a highly
// optimized implementation, provided that we implement and test that
// complexity separately (e.g., by encoding holiday offsets and holiday codes
// using 'bdea_UnsignedCharShortIntArray').  Unlike 'vector' and 'map', a
// calendar is not a generic, general-purpose container.  For more
// domain-specific queries (e.g., 'isBusinessDay'), it is likely that the
// interaction between the weekend days and holidays will matter and we will
// want to articulate arbitrary dates on the fly.

//=============================================================================
//              FLEXIBLE GENERATOR FUNCTIONS 'g' and 'gg' FOR TESTING
//-----------------------------------------------------------------------------
// The function 'g' and 'gg' interpret a specified 'spec' in order from left to
// right according to a complex custom language to bring the calendar to a
// range of possible states relevant for testing.
//
// These functions enables the explict specification of dates representing the
// first and last date in a calendar, holiday dates, and weekend-days
// transition dates.  An absolute date will be represented as @yyyy/mm/dd, (but
// leading 0's may be omitted).  A relative date will be represented as an
// unsigned integer offset from the start of the current range.  Note that
// relative dates should not be used on an empty calendar.
//
// Holiday codes are represented symbolically as uppercase letters.  Days of
// the week to be considered weekend days are identifier by lowercase letters
// {u, m, t, w, r, f, a}.
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
// For example, let's again assume the following arbitrary assignments of
// holiday codes:
//
//     int VA = 0, VB = 1, VC = 2, VD = 100, VE = 1000;
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
    ++input;
    SUCCEED;
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
                   bdecs_PackedCalendar *result,
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

    // Check for duplicates.

    int before = numWeekendDaysInFirstTransition(*result);
    result->addWeekendDay(dow);
    if (before >= numWeekendDaysInFirstTransition(*result)) {
        return FAILURE;
    }

    ++*endPosAddr;
    return SUCCESS;
}

int loadWeekendDaysTransition(const char **endPosAddr,
                              bdecs_PackedCalendar *result,
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
                    bdecs_PackedCalendar *result,
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

    // Check for duplicates.

    int before = result->numHolidayCodes(holiday);
    result->addHolidayCode(holiday, code);
    if (before >= result->numHolidayCodes(holiday)) {
        return FAILURE;
    }

    ++*endPosAddr;
    return SUCCESS;
}

int ggg(bdecs_PackedCalendar *object, const char *spec, bool verboseFlag =true)
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

bdecs_PackedCalendar gg(bdecs_PackedCalendar * object, const char *spec)
    // Return, by reference, the specified object with its value adjusted
    // according to the specified 'spec' according to the custom language
    // described above (in the test driver).
{
    ASSERT(object); ASSERT(spec);
    ASSERT(ggg(object, spec) < 0);
    return *object;
}

bdecs_PackedCalendar g(const char *spec)
    // Return, by value, an instance of this object with its value adjusted
    // according to the specified 'spec' according to the custom language
    // described above (in the test driver).
{
    ASSERT(spec);
    bdecs_PackedCalendar object;
    return gg(&object, spec);
}

//=============================================================================
//                                USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// The two subsections below illustrate various aspects of populating and using
// packed calendars.
//
///Populating Packed Calendars
///- - - - - - - - - - - - - -
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
//     2010  1  18     1   57         ;Martin Luther King Day
//     2010  2  15     1   51         ;Presidents Day
//     2010  4   2     2   9 105      ;Easter Sunday (Observed); Good Friday
//     2010  5  31     1   16         ;Memorial Day
//     2010  7   5     1   28         ;Independence Day (Observed)
//     2010  9   6     1   44         ;Labor Day
//     2010 10  11     1   19         ;Columbus Day
//     2010 11  02     0              ;Election Day
//     2010 11  25     1   14         ;Thanksgiving Day
//     2010 12  25     1    4         ;Christmas Day (Observed)
//     2010 12  31     1   22         ;New Year's Day (Observed)
//..
// Let's now create a couple of primitive helper functions to extract
// holiday and holiday-code counts from a given input stream.  First we'll
// create one that skips over headers and comments to get to the next valid
// holiday record:
//..
    int getNextHoliday(bsl::istream& input, bdet_Date *holiday, int *numCodes)
        // Load into the specified 'holiday' the date of the next holiday, and
        // into 'numCodes' the associated number of holiday codes for the
        // holiday read from the specified 'input' stream.  Return 0 on
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

        if (input.good() && bdet_Date::isValid(year, month, day)) {
            *holiday  = bdet_Date(year, month, day);
            *numCodes = codes;
            return SUCCESS;                                           // RETURN
        }

        return FAILURE;                                               // RETURN
    }
//..
// Then we'll write a function that gets us an integer holiday code, or
// invalidates the stream if it cannot (note that negative holiday codes are
// not supported):
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
// Now that we have these helper functions, it's a simple matter to write a
// calendar loader function, 'load', that populates a given calendar with data
// in this "proprietary" format:
//..
    void load(bsl::istream& input, bdecs_PackedCalendar *calendar)
        // Populate the specified 'calendar' with holidays and corresponding
        // codes read from the specified 'input' stream in our "proprietary"
        // format (see above).  On success, 'input' will be empty, but
        // valid; otherwise 'input' will be invalid.
    {
        bdet_Date holiday;
        int       numCodes;

        while (0 == getNextHoliday(input, &holiday, &numCodes)) {
            calendar->addHoliday(holiday);                       // add date
            for (int i = 0; i < numCodes; ++i) {
                int holidayCode;
                getNextHolidayCode(input, &holidayCode);
                calendar->addHolidayCode(holiday, holidayCode);  // add codes
            }
        }
    }
//..
// Note that different formats can easily be accommodated, while still using
// the same basic population strategy.  Also note that it may be substantially
// more efficient to populate calendars in increasing date order, compared
// to either reverse or random order.
//
///Using Packed Calendars
///- - - - - - - - - - -
// Higher-level clients (e.g., a GUI) may need to extract the holiday codes
// for a particular date, use them to look up their corresponding string names
// in a separate repository (e.g., a vector of strings) and to display these
// names to end users.  First let's create a function, that prints the
// names of holidays for a given date:
//..
    void
    printHolidayNamesForGivenDate(bsl::ostream&                   output,
                                  const bdecs_PackedCalendar&     calendar,
                                  const bdet_Date&                date,
                                  const bsl::vector<bsl::string>& holidayNames)
        // Write, to the specified 'output' stream, the elements in the
        // specified 'holidayNames' associated (via holiday codes in the
        // specified 'calendar') to the specified 'date'.  Each holiday
        // name emitted is followed by a newline ('\n').  The behavior is
        // undefined unless 'date' is within the valid range of 'calendar'.
    {
        for (bdecs_PackedCalendar::HolidayCodeConstIterator
                                         it = calendar.beginHolidayCodes(date);
                                         it != calendar.endHolidayCodes(date);
                                       ++it) {
            output << holidayNames[*it] << bsl::endl;
        }
    }
//..
// Now that we can write the names of holidays for a given date, let's
// write a function that can write out all of the names associated with each
// holiday in the calendar.
//..
    void
    printHolidayDatesAndNames(bsl::ostream&                   output,
                              const bdecs_PackedCalendar&     calendar,
                              const bsl::vector<bsl::string>& holidayNames)
        // Write, to the specified 'output' stream, each date associated with
        // a holiday in the specified 'calendar' followed by any elements in
        // the specified 'holidayNames' (associated via holiday codes in
        // 'calendar') corresponding to that date.  Each date emitted is
        // preceded and followed by a newline ('\n').  Each holiday name
        // emitted is followed by a newline ('\n').

    {
        for (bdecs_PackedCalendar::HolidayConstIterator
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
// Next, let's provide a way to write out the same information above, but limit
// it to the date values within a given range.
//..
    void
    printHolidaysInRange(bsl::ostream&                   output,
                         const bdecs_PackedCalendar&     calendar,
                         const bdet_Date&                beginDate,
                         const bdet_Date&                endDate,
                         const bsl::vector<bsl::string>& holidayNames)
        // Write, to the specified 'output' stream, each date associated
        // with a holiday in the specified 'calendar' within the (inclusive)
        // range indicated by the specified 'beginDate' and 'endDate',
        // followed by any elements in the specified 'holidayNames' (associated
        // via holiday codes in 'calendar') corresponding to that date.  Each
        // date emitted is preceded and followed by a newline ('\n').  Each
        // holiday name emitted is followed by a newline ('\n').  The behavior
        // is undefined unless both 'startDate' and 'endDate' are within the
        // valid range of 'calendar' and 'startDate <= endDate'.

    {
        for (bdecs_PackedCalendar::HolidayConstIterator
                                 it = calendar.beginHolidays(beginDate);
                                 it != calendar.endHolidays(endDate);
                               ++it) {
            output << '\n' << *it << '\n';
            printHolidayNamesForGivenDate(output,
                                          calendar,
                                          *it,
                                          holidayNames);
        }
    }
//..
// Note that we could now reimplement 'printHolidayDatesAndNames', albeit less
// efficiently, in terms of 'printHolidaysInRange':
//..
//  printHolidayDatesAndNames(bsl::ostream&                   output,
//                            const bdecs_PackedCalendar&     calendar,
//                            const bsl::vector<bsl::string>& holidayNames)
//  {
//      if (!calendar.isEmpty()) {
//          printHolidaysInRange(output,
//                               calendar,
//                               calendar.beginDate()
//                               calendar.endDate());
//      }
//  }
//
// Finally, low-level clients may also use a populated 'bdecs_PackedCalendar'
// object directly to determine whether a particular day is a valid business
// day; however, that operation, which here is logarithmic in the number of
// holidays, can be performed *much* more efficiently (see 'bdecs_Calendar'):
//..
    bdet_Date
    getNextBusinessDay(const bdecs_PackedCalendar& calendar,
                       const bdet_Date&            date)
        // Return the next business day in the specified 'calendar' after the
        // specified 'date'.  The behavior is undefined unless such a date
        // exists within the valid range of 'calendar'.
    {
        // Assume there is a business day in the valid range after date.

        bdet_Date candidate = date;
        do {
            ++candidate;
        } while (calendar.isNonBusinessDay(candidate));
                                                     // logarithmic complexity!
        return candidate;
    }
//..
//=============================================================================
//                              TEST CASES
//-----------------------------------------------------------------------------
#define DEFINE_TEST_CASE(NUMBER)                                              \
void testCase##NUMBER(bool verbose, bool veryVerbose, bool veryVeryVerbose)

DEFINE_TEST_CASE(23) {
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
        //   Furthermore, invoke the functions detailed in the usage example
        //   and verify proper functionality.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING USAGE EXAMPLE" << endl
                          << "=====================" << endl;

        if (verbose) cout << "\nTesting 'Populating Packed Calendar' "
                             "usage example" << endl;

        const char *inputFile =
            // HOLIDAY DATE   HOLIDAY CODES
            // ------------   -------------
            // Year Mon Day    #    Codes
            // ---- --- ---   --- ---------
              "2010  1  18     1   57\n"
              "2010  2  15     1   51\n"
              "2010  4   2     2   9 105\n"
              "2010  5  31     1   16\n"
              "2010  7   5     1   28\n"
              "2010  9   6     1   44\n"
              "2010 10  11     1   19\n"
              "2010 11   2     0\n"
              "2010 11  25     1   14\n"
              "2010 12  25     1    4\n"
              "2010 12  31     1   22\n";

        static struct {
            int d_line;              // line number
            int d_year;
            int d_month;
            int d_day;
            int d_numHolidayCodes;
        } DATA[] = {
            //LINE  YEAR   MONTH    DAY   NUMHCODE
            //----  ----   -----    ---   --------
            { L_,   2010,      1,   18,          1 },
            { L_,   2010,      2,   15,          1 },
            { L_,   2010,      4,    2,          2 },
            { L_,   2010,      5,   31,          1 },
            { L_,   2010,      7,    5,          1 },
            { L_,   2010,      9,    6,          1 },
            { L_,   2010,     10,   11,          1 },
            { L_,   2010,     11,    2,          0 },
            { L_,   2010,     11,   25,          1 },
            { L_,   2010,     12,   25,          1 },
            { L_,   2010,     12,   31,          1 },
        };
        const int NUM_DATA = sizeof DATA / sizeof *DATA;

        bsl::vector<bsl::string> holidayNames(106);
        holidayNames[57]  = "Martin Luther King Day";
        holidayNames[51]  = "Presidents Day";
        holidayNames[9]   = "Easter Sunday (Observed)";
        holidayNames[105] = "Good Friday";
        holidayNames[16]  = "Memorial Day";
        holidayNames[28]  = "Independence Day (Observed)";
        holidayNames[44]  = "Labor Day";
        holidayNames[19]  = "Columbus Day";
        holidayNames[14]  = "Thanksgiving Day";
        holidayNames[4]   = "Christmas Day (Observed)";
        holidayNames[22]  = "New Year's Day (Observed)";

        bsl::istringstream iss(inputFile, bsl::ios_base::in);

        bdecs_PackedCalendar calendar;

        // Make sure none of the holidays are loaded.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE  = DATA[ti].d_line;
            const int YEAR  = DATA[ti].d_year;
            const int MONTH = DATA[ti].d_month;
            const int DAY   = DATA[ti].d_day;

            if (veryVerbose) { T_ P_(LINE) P_(YEAR) P_(MONTH) P(DAY) }

            if (calendar.isInRange(bdet_Date(YEAR, MONTH, DAY))) {
                LOOP_ASSERT(LINE,
                            !calendar.isHoliday(bdet_Date(YEAR, MONTH, DAY)));
            }
        }

        load(iss, &calendar);

        // Make sure each holiday is properly loaded.
        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE     = DATA[ti].d_line;
            const int YEAR     = DATA[ti].d_year;
            const int MONTH    = DATA[ti].d_month;
            const int DAY      = DATA[ti].d_day;
            const int NUMHCODE = DATA[ti].d_numHolidayCodes;

            if (veryVerbose) {
                T_ P_(LINE) P_(YEAR) P_(MONTH) P_(DAY) P(NUMHCODE)
            }

            LOOP_ASSERT(LINE, calendar.isHoliday(bdet_Date(YEAR, MONTH, DAY)));

            LOOP_ASSERT(LINE, NUMHCODE == calendar.numHolidayCodes(
                                                 bdet_Date(YEAR, MONTH, DAY)));
        }

        if (verbose) cout << "\nTesting 'Using Packed Calendars' "
                             "usage example" << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const int LINE     = DATA[ti].d_line;
            const int YEAR     = DATA[ti].d_year;
            const int MONTH    = DATA[ti].d_month;
            const int DAY      = DATA[ti].d_day;

            if (veryVerbose) {
                printHolidayNamesForGivenDate(bsl::cout,
                                              calendar,
                                              bdet_Date(YEAR, MONTH, DAY),
                                              holidayNames);
            }
        }

        bsl::ostringstream oss1;
        bsl::ostringstream oss2;

        printHolidayDatesAndNames(oss1, calendar, holidayNames);
        printHolidaysInRange(oss2,
                             calendar,
                             calendar.firstDate(),
                             calendar.lastDate(),
                             holidayNames);

        const bsl::string &str1 = oss1.str();
        const bsl::string &str2 = oss2.str();

        if (veryVerbose) {
            cout << "printHolidayDatesAndNames: \n" << str1 << endl;
            cout << "printHolidaysInRange: \n" << str2 << endl;
        }

        LOOP2_ASSERT(str1, str2, str1 == str2);

      }

DEFINE_TEST_CASE(22) {
        // --------------------------------------------------------------------
        // TESTING 'swap':
        //
        // Concerns:
        //  1. This method properly swaps all data members of the two
        //     calendars, including first date, last day, weekend days,
        //     holidays, and holiday codes.
        //  2. This method is exception-safe with a guarantee of rollback.
        //
        // Plans:
        //  To address concern 1, we use a set of calendars of arbitrarily
        //  determined values and exercise 'swap' on them.  Then we verify
        //  their values are properly swapped.
        //
        //  To address concern 2, we use the 'BEGIN_EXCEPTION_SAFE_TEST' and
        //  'END_EXCEPTION_SAFE_TEST' macros to generate memory exceptions and
        //  verify that the value of the calendar object is rolled back when an
        //  exception is thrown.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Array-Based Implementation Technique
        //
        // Testing:
        //      void swap(bdecs_PackedCalendar *);
        //      void swap(bdecs_PackedCalendar&);
        //      void swap(bdecs_PackedCalendar&, bdecs_PackedCalendar&);
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing 'swap'" << endl
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
            for (int j = 0; SPECS[j]; ++j) {
                Obj mX(&testAllocator); const Obj& X = mX;
                gg(&mX, SPECS[i]);
                const Obj XX(mX);

                Obj mY(&testAllocator); const Obj& Y = mY;
                gg(&mY, SPECS[j]);
                const Obj YY(mY);

                // 'swap' taking pointer (DEPRECATED)
                BEGIN_EXCEPTION_SAFE_TEST {
                    mX.swap(&mY);
                } END_EXCEPTION_SAFE_TEST(X == XX && Y == YY);

                LOOP2_ASSERT(i, j, X == YY && Y == XX);

                // 'swap' taking reference
                mX = XX;
                mY = YY;

                BEGIN_EXCEPTION_SAFE_TEST {
                    mX.swap(mY);
                } END_EXCEPTION_SAFE_TEST(X == XX && Y == YY);

                LOOP2_ASSERT(i, j, X == YY && Y == XX);

                // 'swap' free function
                mX = XX;
                mY = YY;

                BEGIN_EXCEPTION_SAFE_TEST {
                    swap(mX, mY);
                } END_EXCEPTION_SAFE_TEST(X == XX && Y == YY);

                LOOP2_ASSERT(i, j, X == YY && Y == XX);
            }
        }
      }

DEFINE_TEST_CASE(21) {
        // --------------------------------------------------------------------
        // TESTING 'intersectNonBusinessDays' and 'unionBusinessDays':
        //
        // Concerns:
        //  1. Both methods properly set the range of the calendar.
        //  2. The set of weekend days and holidays for the resulting calendar
        //     is the intersection of those from the two original calendars.
        //     All holidays are within the valid range.  The new holiday codes
        //     are the union of holiday codes of the two calendars' common
        //     holidays.
        //  3. Both methods are exception-safe with a guarantee of rollback.
        //
        // Plans:
        //  To address concerns 1 and 2, We create a set of calendar objects
        //  and invoke these methods on them.  We then verify the range,
        //  weekend days, holidays, and associated holiday codes of the
        //  resulting calendars by comparing them with the original calendars.
        //
        //  To address concern 3, we use the 'BEGIN_EXCEPTION_SAFE_TEST' and
        //  'END_EXCEPTION_SAFE_TEST' macros to generate memory exceptions and
        //  verify that the value of the calendar object is rolled back when an
        //  exception is thrown.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Array-Based Implementation Technique
        //
        // Testing:
        //      void intersectNonBusinessDays(const bdecs_PackedCalendar&)
        //      void unionBusinessDays(const bdecs_PackedCalendar&)
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing 'intersectNonBusinessDays' and" << endl
                          << "        'unionBusinessDays'"            << endl
                          << "======================================" << endl;

        static const char *SPECS[] = {
            "", "a", "um", "@1/1/1", "@9999/12/31 0 0B",
            "@1/1/1 100",
            "m@1/1/1 100 0      1      2C 3D    4AB 5B     50DE 100A",
            "w@1/1/1 100 0ABCDE 1ABCDE    3ABCE     5ABCDE      100ABCDE",
            "a@1999/1/3 1000     23     36  232AB        414",
            "mtwfau@1999/1/1 800    23B 38C        406A",
            "u@2000/1/1 100",
            "@2000/2/1 200                         10D   20AE 190",
            "u@2004/1/2 10 1B    2A 3 4B 5ABCDE   10",
            "f@2004/1/1 10 2ABCD 3B 4 5B 6      10",
            "u@2000/1/1 200 10A 5mt 100wr 150f",
            "m@1990/3/1 100 20tr 50rf 100f",
            "m@1990/3/1 100 0mf 20mt 60rf",
            "m@1990/3/1 100 0mf 20mt 30e 60rf",
        0};

        // Test 'intersectNonBusinessDays' Method.

        for (int i = 0; SPECS[i]; ++i) {
            Obj mX(&testAllocator); const Obj& X = mX;
            gg(&mX, SPECS[i]);
            for (int j = 0; SPECS[j]; ++j) {
                Obj mY(&testAllocator); const Obj& Y = mY;
                gg(&mY, SPECS[j]);
                Obj mZ(X, &testAllocator); const Obj& Z = mZ;

                BEGIN_EXCEPTION_SAFE_TEST {
                    mZ.intersectNonBusinessDays(Y);
                } END_EXCEPTION_SAFE_TEST(Z == X);

                if (verbose) { P(X) P(Y) P(Z); cout << endl;}

                LOOP4_ASSERT(i, j, X, Z, (i != j) || (Y == Z));

                // Verifying the new range of 'Z'.

                if (X.firstDate() > Y.lastDate() || X.lastDate() <
                        Y.firstDate()) {
                    LOOP2_ASSERT(i, j, bdet_Date(9999,12,31) == Z.firstDate());
                    LOOP2_ASSERT(i, j, bdet_Date(1,1,1) == Z.lastDate());
                }
                else {
                    LOOP4_ASSERT(i, j, X, Z, X.isInRange(Z.firstDate()));
                    LOOP4_ASSERT(i, j, X, Z, X.isInRange(Z.lastDate()));
                    LOOP4_ASSERT(i, j, Y, Z, Y.isInRange(Z.firstDate()));
                    LOOP4_ASSERT(i, j, Y, Z, Y.isInRange(Z.lastDate()));
                    LOOP4_ASSERT(i, j, X, Y, Z.firstDate() == X.firstDate() ||
                                             Z.firstDate() == Y.firstDate());
                    LOOP4_ASSERT(i, j, X, Y, Z.lastDate() == X.lastDate() ||
                                             Z.lastDate() == Y.lastDate());
                }

                // Verifying the new weekend days in 'Z'.
                for (bdet_Date d = Z.firstDate(); d <= Z.lastDate(); ++d) {
                    bool xWeekendDayFlag = X.isWeekendDay(d);
                    bool yWeekendDayFlag = Y.isWeekendDay(d);
                    bool zWeekendDayFlag = Z.isWeekendDay(d);
                    LOOP3_ASSERT(zWeekendDayFlag,
                                 xWeekendDayFlag,
                                 yWeekendDayFlag,
                                 zWeekendDayFlag ==
                                         (xWeekendDayFlag && yWeekendDayFlag));
                }

                // Verifying the new holidays in 'Z'.

                Obj::HolidayConstIterator it    = X.beginHolidays();
                Obj::HolidayConstIterator itEnd = X.endHolidays();
                Obj::HolidayConstIterator jt    = Y.beginHolidays();
                Obj::HolidayConstIterator jtEnd = Y.endHolidays();
                Obj::HolidayConstIterator kt    = Z.beginHolidays();

                // Iterating through all holidays in 'Z'.

                while (kt != Z.endHolidays()) {

                    if (it != itEnd && *it == *kt) {
                        while (jt != jtEnd && *jt < *it) {
                            ++jt;
                        }
                    }
                    else if (jt != jtEnd && *jt == *kt) {
                        while (it != itEnd && *it < *jt) {
                            ++it;
                        }
                    }

                    // Check consistency.

                    LOOP2_ASSERT(i, j, Z.isInRange(*kt));

                    if (((it != itEnd) && (jt != jtEnd) && *it == *jt)) {

                        // If this holiday exists in both 'X' and 'Y', it must
                        // exist in 'Z'.

                        LOOP4_ASSERT(i, j, *it, *kt, *kt == *it);
                        LOOP4_ASSERT(i, j, *jt, *kt, *kt == *jt);

                        // Verifying the new holiday codes in 'Z'.

                        Obj::HolidayCodeConstIterator itCode =
                                                       X.beginHolidayCodes(it);
                        Obj::HolidayCodeConstIterator jtCode =
                                                       Y.beginHolidayCodes(jt);
                        Obj::HolidayCodeConstIterator ktCode =
                                                       Z.beginHolidayCodes(kt);
                        Obj::HolidayCodeConstIterator itCodeEnd =
                                                         X.endHolidayCodes(it);
                        Obj::HolidayCodeConstIterator jtCodeEnd =
                                                         Y.endHolidayCodes(jt);
                        Obj::HolidayCodeConstIterator ktCodeEnd =
                                                         Z.endHolidayCodes(kt);
                        while (ktCode != ktCodeEnd) {
                            bool incrementItCodeFlag = false;
                            bool incrementJtCodeFlag = false;

                            if (itCode != itCodeEnd && *itCode == *ktCode) {
                                LOOP2_ASSERT(i, j, (jtCode == jtCodeEnd) ||
                                                   *itCode <= *jtCode);

                                // Found the holiday code in 'X'.

                                incrementItCodeFlag = true;
                            }
                            if (jtCode != jtCodeEnd && *jtCode == *ktCode) {
                                LOOP2_ASSERT(i, j, (itCode == itCodeEnd) ||
                                                   *itCode >= *jtCode);

                                // Found the holiday code in 'Y'.

                                incrementJtCodeFlag = true;
                            }
                            if (incrementItCodeFlag) {
                                ++itCode;
                            }
                            if (incrementJtCodeFlag) {
                                ++jtCode;
                            }

                            // Verify that the holiday code in 'Z' referenced
                            // by 'ktCode' is found in either 'X' or 'Y' (or
                            // both).

                            LOOP6_ASSERT(i, j, X, Y, Z, *ktCode,
                                   incrementItCodeFlag || incrementJtCodeFlag);
                            ++ktCode;
                        }
                        LOOP3_ASSERT(i, j, *itCode, itCode == itCodeEnd);
                        LOOP3_ASSERT(i, j, *jtCode, jtCode == jtCodeEnd);

                        ++kt;
                        ++jt;
                        ++it;
                        continue;
                    }
                    else if (it != itEnd && *kt == *it) {
                        LOOP3_ASSERT(i, j, *it, Y.isWeekendDay(*it));
                        LOOP3_ASSERT(i, j, *it, *kt == *it);

                        Obj::HolidayCodeConstIterator itCode =
                                                       X.beginHolidayCodes(it);
                        Obj::HolidayCodeConstIterator ktCode =
                                                       Z.beginHolidayCodes(kt);
                        Obj::HolidayCodeConstIterator itCodeEnd =
                                                         X.endHolidayCodes(it);
                        Obj::HolidayCodeConstIterator ktCodeEnd =
                                                         Z.endHolidayCodes(kt);
                        while (itCode != itCodeEnd) {
                            LOOP3_ASSERT(i, j, *jt, ktCode  != ktCodeEnd);
                            LOOP3_ASSERT(i, j, *it, *ktCode == *itCode);
                            ++itCode; ++ktCode;
                        }
                        LOOP3_ASSERT(i, j, *it, ktCodeEnd == ktCodeEnd);

                        ++it;
                        ++kt;
                        continue;
                    }
                    else if (jt != jtEnd && *kt == *jt) {
                        LOOP3_ASSERT(i, j, *jt, X.isWeekendDay(*jt));
                        LOOP3_ASSERT(i, j, *jt, *kt == *jt);

                        Obj::HolidayCodeConstIterator jtCode =
                                                       Y.beginHolidayCodes(jt);
                        Obj::HolidayCodeConstIterator ktCode =
                                                       Z.beginHolidayCodes(kt);
                        Obj::HolidayCodeConstIterator jtCodeEnd =
                                                         Y.endHolidayCodes(jt);
                        Obj::HolidayCodeConstIterator ktCodeEnd =
                                                         Z.endHolidayCodes(kt);
                        while (jtCode != jtCodeEnd) {
                            LOOP3_ASSERT(i, j, *jt, ktCode  != ktCodeEnd);
                            LOOP3_ASSERT(i, j, *jt, *ktCode == *jtCode);
                            ++jtCode; ++ktCode;
                        }
                        LOOP3_ASSERT(i, j, *jt, ktCodeEnd == ktCodeEnd);

                        ++jt;
                        ++kt;
                        continue;
                    }

                    if (it != itEnd && jt != jtEnd) {
                        LOOP2_ASSERT(i, j, *it != *jt);
                    }

                    LOOP2_ASSERT(i, j, it != itEnd || jt != jtEnd);

                    if (it != itEnd && jt != jtEnd) {
                        if (*it < *jt)
                            ++it;
                        else
                            ++jt;
                    }
                    else if (it != itEnd) {
                        ++it;
                    }
                    else if (jt != jtEnd) {
                        ++jt;
                    }

                }

                // Verify that 'X' and 'Y' has no more common holiday after
                // this point since we have gone through all the holidays in
                // 'Z'.

                for (Obj::HolidayConstIterator itx = it; itx != itEnd; ++itx) {
                    for (Obj::HolidayConstIterator ity = jt; ity != jtEnd;
                                                                       ++ity) {
                        LOOP4_ASSERT(i, j, *itx, *ity, *itx != *ity);
                    }
                }
            }
        }

        // Testing 'unionBusinessDays'.

        for (int i = 0; SPECS[i]; ++i) {
            Obj mX(&testAllocator); const Obj& X = mX;
            gg(&mX, SPECS[i]);
            for (int j = 0; SPECS[j]; ++j) {
                Obj mY(&testAllocator); const Obj& Y = mY;
                gg(&mY, SPECS[j]);
                Obj mZ(X, &testAllocator); const Obj& Z = mZ;

                BEGIN_EXCEPTION_SAFE_TEST {
                    mZ.unionBusinessDays(Y);
                } END_EXCEPTION_SAFE_TEST(Z == X);

                if (verbose) { P(X) P(Y) P(Z); cout << endl;}

                LOOP4_ASSERT(i, j, X, Z, (i != j) || (Y == Z));

                // Verifying the new range of 'Z'.

                if (X.firstDate() <= X.lastDate()) {
                    LOOP4_ASSERT(i, j, X, Z, Z.isInRange(X.firstDate()));
                    LOOP4_ASSERT(i, j, X, Z, Z.isInRange(X.lastDate()));
                }
                if (Y.firstDate() <= Y.lastDate()) {
                    LOOP4_ASSERT(i, j, Y, Z, Z.isInRange(Y.firstDate()));
                    LOOP4_ASSERT(i, j, Y, Z, Z.isInRange(Y.lastDate()));
                }

                // Verifying the new weekend days in 'Z'.
                for (bdet_Date d = X.firstDate(); d <= X.lastDate(); ++d) {
                    bool xWeekendDayFlag = X.isWeekendDay(d);
                    bool yWeekendDayFlag = Y.isWeekendDay(d);
                    bool zWeekendDayFlag = Z.isWeekendDay(d);
                    LOOP3_ASSERT(zWeekendDayFlag,
                                 xWeekendDayFlag,
                                 yWeekendDayFlag,
                                 zWeekendDayFlag ==
                                         (xWeekendDayFlag && yWeekendDayFlag));
                }

                for (bdet_Date d = Y.firstDate(); d <= Y.lastDate(); ++d) {
                    bool xWeekendDayFlag = X.isWeekendDay(d);
                    bool yWeekendDayFlag = Y.isWeekendDay(d);
                    bool zWeekendDayFlag = Z.isWeekendDay(d);
                    LOOP3_ASSERT(zWeekendDayFlag,
                                 xWeekendDayFlag,
                                 yWeekendDayFlag,
                                 zWeekendDayFlag ==
                                         (xWeekendDayFlag && yWeekendDayFlag));
                }

                // Verifying the new holidays in 'Z'.

                Obj::HolidayConstIterator it    = X.beginHolidays();
                Obj::HolidayConstIterator itEnd = X.endHolidays();
                Obj::HolidayConstIterator jt    = Y.beginHolidays();
                Obj::HolidayConstIterator jtEnd = Y.endHolidays();
                Obj::HolidayConstIterator kt    = Z.beginHolidays();

                // Iterating through all holidays in 'Z'.

                while (kt != Z.endHolidays()) {

                    if (it != itEnd && *it == *kt) {
                        while (jt != jtEnd && *jt < *it) {
                            ++jt;
                        }
                    }
                    else if (jt != jtEnd && *jt == *kt) {
                        while (it != itEnd && *it < *jt) {
                            ++it;
                        }
                    }

                    // Check consistency.

                    LOOP2_ASSERT(i, j, Z.isInRange(*kt));

                    if (((it != itEnd) && (jt != jtEnd) && *it == *jt)) {

                        // If this holiday exists in both 'X' and 'Y', it must
                        // exist in 'Z'.

                        LOOP4_ASSERT(i, j, *it, *kt, *kt == *it);
                        LOOP4_ASSERT(i, j, *jt, *kt, *kt == *jt);

                        // Verifying the new holiday codes in 'Z'.

                        Obj::HolidayCodeConstIterator itCode =
                                                       X.beginHolidayCodes(it);
                        Obj::HolidayCodeConstIterator jtCode =
                                                       Y.beginHolidayCodes(jt);
                        Obj::HolidayCodeConstIterator ktCode =
                                                       Z.beginHolidayCodes(kt);
                        Obj::HolidayCodeConstIterator itCodeEnd =
                                                         X.endHolidayCodes(it);
                        Obj::HolidayCodeConstIterator jtCodeEnd =
                                                         Y.endHolidayCodes(jt);
                        Obj::HolidayCodeConstIterator ktCodeEnd =
                                                         Z.endHolidayCodes(kt);
                        while (ktCode != ktCodeEnd) {
                            bool incrementItCodeFlag = false;
                            bool incrementJtCodeFlag = false;

                            if (itCode != itCodeEnd && *itCode == *ktCode) {
                                LOOP2_ASSERT(i, j, (jtCode == jtCodeEnd) ||
                                                   *itCode <= *jtCode);

                                // Found the holiday code in 'X'.

                                incrementItCodeFlag = true;
                            }
                            if (jtCode != jtCodeEnd && *jtCode == *ktCode) {
                                LOOP2_ASSERT(i, j, (itCode == itCodeEnd) ||
                                                   *itCode >= *jtCode);

                                // Found the holiday code in 'Y'.

                                incrementJtCodeFlag = true;
                            }
                            if (incrementItCodeFlag) {
                                ++itCode;
                            }
                            if (incrementJtCodeFlag) {
                                ++jtCode;
                            }

                            // Verify that the holiday code in 'Z' referenced
                            // by 'ktCode' is found in either 'X' or 'Y' (or
                            // both).

                            LOOP6_ASSERT(i, j, X, Y, Z, *ktCode,
                                   incrementItCodeFlag || incrementJtCodeFlag);
                            ++ktCode;
                        }
                        LOOP3_ASSERT(i, j, *itCode, itCode == itCodeEnd);
                        LOOP3_ASSERT(i, j, *jtCode, jtCode == jtCodeEnd);

                        ++kt;
                        ++jt;
                        ++it;
                        continue;
                    }
                    else if (it != itEnd && *kt == *it) {
                        LOOP3_ASSERT(i, j, *it, Y.isWeekendDay(*it));
                        LOOP3_ASSERT(i, j, *it, *kt == *it);

                        Obj::HolidayCodeConstIterator itCode =
                                                       X.beginHolidayCodes(it);
                        Obj::HolidayCodeConstIterator ktCode =
                                                       Z.beginHolidayCodes(kt);
                        Obj::HolidayCodeConstIterator itCodeEnd =
                                                         X.endHolidayCodes(it);
                        Obj::HolidayCodeConstIterator ktCodeEnd =
                                                         Z.endHolidayCodes(kt);
                        while (itCode != itCodeEnd) {
                            LOOP3_ASSERT(i, j, *jt, ktCode  != ktCodeEnd);
                            LOOP3_ASSERT(i, j, *it, *ktCode == *itCode);
                            ++itCode; ++ktCode;
                        }
                        LOOP3_ASSERT(i, j, *it, ktCodeEnd == ktCodeEnd);

                        ++it;
                        ++kt;
                        continue;
                    }
                    else if (jt != jtEnd && *kt == *jt) {
                        LOOP3_ASSERT(i, j, *jt, X.isWeekendDay(*jt));
                        LOOP3_ASSERT(i, j, *jt, *kt == *jt);

                        Obj::HolidayCodeConstIterator jtCode =
                                                       Y.beginHolidayCodes(jt);
                        Obj::HolidayCodeConstIterator ktCode =
                                                       Z.beginHolidayCodes(kt);
                        Obj::HolidayCodeConstIterator jtCodeEnd =
                                                         Y.endHolidayCodes(jt);
                        Obj::HolidayCodeConstIterator ktCodeEnd =
                                                         Z.endHolidayCodes(kt);
                        while (jtCode != jtCodeEnd) {
                            LOOP3_ASSERT(i, j, *jt, ktCode  != ktCodeEnd);
                            LOOP3_ASSERT(i, j, *jt, *ktCode == *jtCode);
                            ++jtCode; ++ktCode;
                        }
                        LOOP3_ASSERT(i, j, *jt, ktCodeEnd == ktCodeEnd);

                        ++jt;
                        ++kt;
                        continue;
                    }

                    if (it != itEnd && jt != jtEnd) {
                        LOOP2_ASSERT(i, j, *it != *jt);
                    }

                    LOOP2_ASSERT(i, j, it != itEnd || jt != jtEnd);

                    if (it != itEnd && jt != jtEnd) {
                        if (*it < *jt)
                            ++it;
                        else
                            ++jt;
                    }
                    else if (it != itEnd) {
                        ++it;
                    }
                    else if (jt != jtEnd) {
                        ++jt;
                    }
                }

                // Verify that 'X' and 'Y' has no more common holiday after
                // this point since we have gone through all the holidays in
                // 'Z'.

                for (Obj::HolidayConstIterator itx = it; itx != itEnd; ++itx) {
                    for (Obj::HolidayConstIterator ity = jt; ity != jtEnd;
                                                                       ++ity) {
                        LOOP4_ASSERT(i, j, *itx, *ity, *itx != *ity);
                    }
                }
            }
        }
      }

DEFINE_TEST_CASE(20) {
        // --------------------------------------------------------------------
        // TESTING 'intersectBusinessDays' and 'unionNonBusinessDays':
        //
        // Concerns:
        //  1. Both methods properly set the range of the calendar.
        //  2. The set of weekend days, holidays, and associated holiday codes
        //     for the resulting calendar is the union of those from the two
        //     original calendars.  All holidays are within the valid range.
        //  3. Both methods are exception-safe with a guarantee of rollback.
        //
        // Plans:
        //  To address concerns 1 and 2, We create a set of calendar objects
        //  and invoke these methods on them.  We then verify the range,
        //  weekend days, holidays, and associated holiday codes of the
        //  resulting calendars by comparing them with the original calendars.
        //
        //  To address concern 3, we use the 'BEGIN_EXCEPTION_SAFE_TEST' and
        //  'END_EXCEPTION_SAFE_TEST' macros to generate memory exceptions and
        //  verify that the value of the calendar object is rolled back when an
        //  exception is thrown.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Array-Based Implementation Technique
        //
        // Testing:
        //      void intersectBusinessDays(const bdecs_PackedCalendar&)
        //      void unionNonBusinessDays(const bdecs_PackedCalendar&)
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing 'intersectBusinessDays' and" << endl
                          << "        'unionNonBusinessDays'"      << endl
                          << "===================================" << endl;

        static const char *SPECS[] = {
            "", "a", "um", "@1/1/1", "@9999/12/31 0 0B",

            "@1/1/1 100",
            "m@1/1/1 100 0      1      2C 3D    4AB 5B     50DE 100A",
            "w@1/1/1 100 0ABCDE 1ABCDE    3ABCE     5ABCDE      100ABCDE",

            "a@1999/1/3 1000     23     36  232AB        414",
            "mtwfau@1999/1/1 800    23B 38C        406A",
            "u@2000/1/1 100",
            "@2000/2/1 200                         10D   20AE 190",

            "u@2004/1/2 10 1B    2A 3 4B 5ABCDE   10",
            "f@2004/1/1 10 2ABCD 3B 4 5B 6      10",
            "u@2000/1/1 200 10A 5mt 100wr 150f",
            "m@1990/3/1 100 20tr 50rf 100f",
            "m@1990/3/1 100 0mf 20mt 60rf",
            "m@1990/3/1 100 0mf 20mt 30e 60rf",
        0};

        // Test 'intersectBusinessDays'.

        for (int i = 0; SPECS[i]; ++i) {
            Obj mX(&testAllocator); const Obj& X = mX;
            gg(&mX, SPECS[i]);
            for (int j = 0; SPECS[j]; ++j) {
                Obj mY(&testAllocator); const Obj& Y = mY;
                gg(&mY, SPECS[j]);
                Obj mZ(X, &testAllocator); const Obj& Z = mZ;

                BEGIN_EXCEPTION_SAFE_TEST {
                    mZ.intersectBusinessDays(Y);
                } END_EXCEPTION_SAFE_TEST(Z == X);

                LOOP4_ASSERT(i, j, X, Z, (i != j) || (Y == Z));

                // Verifying the new range of 'Z'.

                if (   X.firstDate() > Y.lastDate()
                    || X.lastDate()  < Y.firstDate()) {
                    LOOP2_ASSERT(i, j, bdet_Date(9999,12,31) == Z.firstDate());
                    LOOP2_ASSERT(i, j, bdet_Date(1,1,1)      == Z.lastDate());
                }
                else {
                    LOOP4_ASSERT(i, j, X, Z, X.isInRange(Z.firstDate()));
                    LOOP4_ASSERT(i, j, X, Z, X.isInRange(Z.lastDate()));
                    LOOP4_ASSERT(i, j, Y, Z, Y.isInRange(Z.firstDate()));
                    LOOP4_ASSERT(i, j, Y, Z, Y.isInRange(Z.lastDate()));
                    LOOP4_ASSERT(i, j, X, Y, Z.firstDate() == X.firstDate() ||
                                             Z.firstDate() == Y.firstDate());
                    LOOP4_ASSERT(i, j, X, Y, Z.lastDate() == X.lastDate() ||
                                             Z.lastDate() == Y.lastDate());
                }

                // Verifying the new weekend days in 'Z'.

                for (bdet_Date d = Z.firstDate(); d <= Z.lastDate(); ++d) {
                    bool xWeekendDayFlag = X.isWeekendDay(d);
                    bool yWeekendDayFlag = Y.isWeekendDay(d);
                    bool zWeekendDayFlag = Z.isWeekendDay(d);
                    LOOP3_ASSERT(zWeekendDayFlag,
                                 xWeekendDayFlag,
                                 yWeekendDayFlag,
                                 zWeekendDayFlag ==
                                         (xWeekendDayFlag || yWeekendDayFlag));
                }

                // Verifying the new holidays in 'Z'.

                Obj::HolidayConstIterator it = X.beginHolidays();
                Obj::HolidayConstIterator jt = Y.beginHolidays();
                Obj::HolidayConstIterator kt = Z.beginHolidays();
                Obj::HolidayConstIterator itEnd = X.endHolidays();
                Obj::HolidayConstIterator jtEnd = Y.endHolidays();
                Obj::HolidayConstIterator ktEnd = Z.endHolidays();

                // Position the holiday iterators of 'X' and 'Y' to within the
                // valid range of 'Z'.

                if (X.firstDate() < Z.firstDate()) {
                    while (it != itEnd && *it < Z.firstDate()) {
                        ++it;
                    }
                }
                if (Y.firstDate() < Z.firstDate()) {
                    while(jt != jtEnd && *jt < Z.firstDate()) {
                        ++jt;
                    }
                }

                // Iterating through all holidays in 'Z'.

                while (kt != ktEnd) {
                    LOOP2_ASSERT(i, j, Z.isInRange(*kt));
                    bool incrementItFlag = false;
                    bool incrementJtFlag = false;

                    if (it != itEnd && *it == *kt) {
                        LOOP5_ASSERT(i, j, X, Y, Z,
                                                  (jt == jtEnd) || *it <= *jt);
                        incrementItFlag = true;  // found the holiday in 'X'
                    }
                    if (jt != jtEnd && *jt == *kt) {
                        LOOP5_ASSERT(i, j, X, Y, Z,
                                                  (it == itEnd) || *it >= *jt);
                        incrementJtFlag = true;  // found the holiday in 'Y'
                    }

                    // Verifying the new holiday codes for the holiday
                    // referenced by 'kt' in 'Z'.

                    Obj::HolidayCodeConstIterator itCodeEnd =
                                                         Z.endHolidayCodes(kt);
                    Obj::HolidayCodeConstIterator itCode = itCodeEnd;
                    Obj::HolidayCodeConstIterator jtCodeEnd =
                                                         Z.endHolidayCodes(kt);
                    Obj::HolidayCodeConstIterator jtCode = jtCodeEnd;

                    if (incrementItFlag) {
                        itCode = X.beginHolidayCodes(it);
                        itCodeEnd = X.endHolidayCodes(it);
                    }

                    if (incrementJtFlag) {
                        jtCode = Y.beginHolidayCodes(jt);
                        jtCodeEnd = Y.endHolidayCodes(jt);
                    }

                    Obj::HolidayCodeConstIterator ktCode =
                                                      Z.beginHolidayCodes(*kt);
                    Obj::HolidayCodeConstIterator ktCodeEnd =
                                                      Z.endHolidayCodes(*kt);

                    // Iterating through all holiday codes for the holiday
                    // referenced by 'kt' in 'Z'.

                    while (ktCode != ktCodeEnd) {
                        bool incrementItCodeFlag = false;
                        bool incrementJtCodeFlag = false;

                        if (itCode != itCodeEnd && *itCode == *ktCode) {
                            LOOP2_ASSERT(i, j,
                                  (jtCode == jtCodeEnd) || *itCode <= *jtCode);

                            // Found the holiday code in 'X'.

                            incrementItCodeFlag = true;
                        }
                        if (jtCode != jtCodeEnd && *jtCode == *ktCode) {
                            LOOP2_ASSERT(i, j,
                                  (itCode == itCodeEnd) || *itCode >= *jtCode);

                            // Found the holiday code in 'Y'.

                            incrementJtCodeFlag = true;
                        }
                        if (incrementItCodeFlag) {
                            ++itCode;
                        }
                        if (incrementJtCodeFlag) {
                            ++jtCode;
                        }

                        // Verify that the holiday code in 'Z' referenced by
                        // 'ktCode' is found in either 'X' or 'Y' (or both).

                        LOOP6_ASSERT(i, j, X, Y, Z, *ktCode,
                                   incrementItCodeFlag || incrementJtCodeFlag);

                        ++ktCode;
                    }

                    // We have gone through all the holiday codes for this date
                    // in both 'X' or 'Y' because the codes in 'Z' is a union
                    // of those in 'X' and 'Y'.  Therefore both 'itCode' and
                    // 'jtCode' should have reached the end here.

                    LOOP3_ASSERT(i, j, *itCode, itCode == itCodeEnd);
                    LOOP3_ASSERT(i, j, *jtCode, jtCode == jtCodeEnd);

                    if (incrementItFlag) {
                        ++it;
                    }
                    if (incrementJtFlag) {
                        ++jt;
                    }

                    // Verify that the holiday in 'Z' referenced by 'kt' is
                    // found in either 'Y' or 'Z' (or both).

                    LOOP6_ASSERT(i, j, X, Y, Z, *kt,
                                           incrementItFlag || incrementJtFlag);

                    ++kt;
                }

                // Between 'X' and 'Y', the one with the smaller last date
                // should have all its holidays included in 'Z'.  So its
                // holiday iterator should have reached the end here.

                LOOP2_ASSERT(i, j, it == itEnd || jt == jtEnd);

                // At most one of the two loops below will execute for the
                // reason above.  These two loops verify that any holidays that
                // we have not gone through in either 'X' or 'Y' must be out of
                // the valid range of 'Z'.

                while (it != itEnd) {
                    LOOP2_ASSERT(i, j, Z.isInRange(*it) == false);
                    ++it;
                }
                while (jt != jtEnd) {
                    LOOP2_ASSERT(i, j, Z.isInRange(*jt) == false);
                    ++jt;
                }
            }
        }

        // Test 'unionNonBusinessDays'.

        for (int i = 0; SPECS[i]; ++i) {
            Obj mX(&testAllocator); const Obj& X = mX;
            gg(&mX, SPECS[i]);
            for (int j = 0; SPECS[j]; ++j) {
                Obj mY(&testAllocator); const Obj& Y = mY;
                gg(&mY, SPECS[j]);
                Obj mZ(X, &testAllocator); const Obj& Z = mZ;

                BEGIN_EXCEPTION_SAFE_TEST {
                    mZ.unionNonBusinessDays(Y);
                } END_EXCEPTION_SAFE_TEST(Z == X);

                LOOP4_ASSERT(i, j, X, Z, (i != j) || (Y == Z));

                // Verifying the new range of 'Z'.

                if (X.firstDate() <= X.lastDate()) {
                    LOOP4_ASSERT(i, j, X, Z, Z.isInRange(X.firstDate()));
                    LOOP4_ASSERT(i, j, X, Z, Z.isInRange(X.lastDate()));
                }
                if (Y.firstDate() <= Y.lastDate()) {
                    LOOP4_ASSERT(i, j, Y, Z, Z.isInRange(Y.firstDate()));
                    LOOP4_ASSERT(i, j, Y, Z, Z.isInRange(Y.lastDate()));
                }

                // Verifying the new weekend days in 'Z'.

                for (bdet_Date d = X.firstDate(); d <= X.lastDate(); ++d) {
                    bool xWeekendDayFlag = X.isWeekendDay(d);
                    bool yWeekendDayFlag = Y.isWeekendDay(d);
                    bool zWeekendDayFlag = Z.isWeekendDay(d);
                    LOOP3_ASSERT(zWeekendDayFlag,
                                 xWeekendDayFlag,
                                 yWeekendDayFlag,
                                 zWeekendDayFlag ==
                                         (xWeekendDayFlag || yWeekendDayFlag));
                }

                for (bdet_Date d = Y.firstDate(); d <= Y.lastDate(); ++d) {
                    bool xWeekendDayFlag = X.isWeekendDay(d);
                    bool yWeekendDayFlag = Y.isWeekendDay(d);
                    bool zWeekendDayFlag = Z.isWeekendDay(d);
                    LOOP6_ASSERT(X, Y, Z, zWeekendDayFlag,
                                 xWeekendDayFlag,
                                 yWeekendDayFlag,
                                 zWeekendDayFlag ==
                                         (xWeekendDayFlag || yWeekendDayFlag));
                }

                // Verifying the new holidays in 'Z'.

                Obj::HolidayConstIterator it = X.beginHolidays();
                Obj::HolidayConstIterator jt = Y.beginHolidays();
                Obj::HolidayConstIterator kt = Z.beginHolidays();
                Obj::HolidayConstIterator itEnd = X.endHolidays();
                Obj::HolidayConstIterator jtEnd = Y.endHolidays();
                Obj::HolidayConstIterator ktEnd = Z.endHolidays();

                // Iterating through all holidays in 'Z'.

                while (kt != ktEnd) {

                    LOOP2_ASSERT(i, j, Z.isInRange(*kt));

                    bool incrementItFlag = false;
                    bool incrementJtFlag = false;
                    if (it != itEnd && *it == *kt) {
                        LOOP2_ASSERT(i, j, (jt == jtEnd) || *it <= *jt);
                        incrementItFlag = true;  // found the holiday in 'X'
                    }
                    if (jt != jtEnd && *jt == *kt) {
                        LOOP2_ASSERT(i, j, (it == itEnd) || *it >= *jt);
                        incrementJtFlag = true;  // found the holiday in 'Y'
                    }

                    // Verifying the new holiday codes for the holiday
                    // referenced by 'kt' in 'Z'.

                    Obj::HolidayCodeConstIterator itCodeEnd =
                                                         Z.endHolidayCodes(kt);
                    Obj::HolidayCodeConstIterator itCode = itCodeEnd;
                    Obj::HolidayCodeConstIterator jtCodeEnd =
                                                         Z.endHolidayCodes(kt);
                    Obj::HolidayCodeConstIterator jtCode = jtCodeEnd;

                    if (incrementItFlag) {
                        itCode = X.beginHolidayCodes(it);
                        itCodeEnd = X.endHolidayCodes(it);
                    }

                    if (incrementJtFlag) {
                        jtCode = Y.beginHolidayCodes(jt);
                        jtCodeEnd = Y.endHolidayCodes(jt);
                    }

                    Obj::HolidayCodeConstIterator ktCode =
                                                      Z.beginHolidayCodes(*kt);
                    Obj::HolidayCodeConstIterator ktCodeEnd =
                                                        Z.endHolidayCodes(*kt);

                    // Iterating through all holiday codes for the holiday
                    // referenced by 'kt' in 'Z'.

                    while (ktCode != ktCodeEnd) {
                        bool incrementItCodeFlag = false;
                        bool incrementJtCodeFlag = false;
                        if (   incrementItFlag
                               && itCode != itCodeEnd
                               && *itCode == *ktCode) {
                            LOOP2_ASSERT(i, j,
                                      (!incrementJtFlag || jtCode == jtCodeEnd)
                                      || *itCode <= *jtCode);

                            // Found the holiday code in 'X'.
                            incrementItCodeFlag = true;
                        }
                        if (   incrementJtFlag
                               && jtCode != jtCodeEnd
                               && *jtCode == *ktCode) {
                            LOOP2_ASSERT(i, j,
                                      (!incrementItFlag || itCode == itCodeEnd)
                                      || *itCode >= *jtCode);

                            // Found the holiday code in 'Y'.

                            incrementJtCodeFlag = true;
                        }
                        if (incrementItCodeFlag) {
                            ++itCode;
                        }
                        if (incrementJtCodeFlag) {
                            ++jtCode;
                        }

                        // Verify that the holiday code in 'Z' referenced by
                        // 'ktCode' is found in either 'X' or 'Y' (or both).

                        LOOP6_ASSERT(i, j, X, Y, Z, *ktCode,
                                   incrementItCodeFlag || incrementJtCodeFlag);

                        ++ktCode;
                    }

                    // We have gone through all the holiday codes for this
                    // date in both 'X' or 'Y' because the codes in 'Z' is a
                    // union of those in 'X' and 'Y'.  Therefore both 'itCode'
                    // and 'jtCode' should have reached the end here.

                    LOOP3_ASSERT(i, j, *itCode,
                                      !incrementItFlag || itCode == itCodeEnd);
                    LOOP3_ASSERT(i, j, *jtCode,
                                      !incrementJtFlag || jtCode == jtCodeEnd);

                    if (incrementItFlag) {
                        ++it;
                    }
                    if (incrementJtFlag) {
                        ++jt;
                    }

                    // Verify that the holiday in 'Z' referenced by 'kt' is
                    // found in either 'Y' or 'Z' (or both).

                    LOOP6_ASSERT(i, j, X, Y, Z, *kt,
                                           incrementItFlag || incrementJtFlag);

                    ++kt;
                }

                // Since 'Z' has all the holidays in 'X' and 'Y', the iterators
                // for holidays in 'X' and 'Y' should have reached the end
                // here.

                LOOP3_ASSERT(i, j, *it, it == itEnd);
                LOOP3_ASSERT(i, j, *jt, jt == jtEnd);
            }
        }
      }

DEFINE_TEST_CASE(19) {
        // --------------------------------------------------------------------
        // TESTING 'removeHoliday' and 'removeHolidayCode':
        //
        // Concerns:
        //  1. These methods remove the specified holiday/holiday code if it
        //     exists in the calendar.
        //  2. These methods have no effect on the calendar object if the
        //     specified holiday/holiday code does not exist.
        //  3. These methods has no effect on other aspects of the calendar
        //     such as its first date, last date, or weekend days.
        //
        // Plan:
        //  To address concern 1, we individually remove each holiday/holiday
        //  code in the calendar and verify that only the specified
        //  holiday/holiday code is removed by comparing the result with the
        //  original object.
        //
        //  To address concern 2, we try to remove non-existing or out-of-range
        //  holidays/holiday codes and verify that the calendar object is
        //  unchanged.
        //
        //  To address concern 3, we add the removed holiday/holiday code back
        //  to the calendar object and verify it has the same value as before.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Array-Based Implementation Technique
        //
        // Testing:
        //      void removeHoliday(const bdet_Date&)
        //      void removeHolidayCode(const bdet_Date&, int)
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing 'removeHoliday' and 'removeHolidayCode'"
                          << endl
                          << "==============================================="
                          << endl;

        static const char *SPECS[] = {
            "@1/1/1 0 0",
            "@2005/6/7 365 180B",
            "au@2008/1/1 34124 1 2AB 3C 234C 2323 9786ADE",
            "a@9999/12/1 30 15A 30BC",
            "a@2005/6/7 30 10w 15A 30BC",
            "a@9999/12/1 30 10fu 15mt 15A 30BC",
            0
        };

        // Trivial tests on an empty calendar.

        Obj emptyCal(&testAllocator);
        Obj emptyCal2(&testAllocator);
        emptyCal.removeHoliday(bdet_Date(   1,  1,  1));
        ASSERT(emptyCal == emptyCal2);
        emptyCal.removeHoliday(bdet_Date(9999, 12, 31));
        ASSERT(emptyCal == emptyCal2);
        emptyCal.removeHolidayCode(bdet_Date(   1,  1,  1), 1);
        ASSERT(emptyCal == emptyCal2);
        emptyCal.removeHolidayCode(bdet_Date(9999, 12, 31), 1);
        ASSERT(emptyCal == emptyCal2);

        // Test 'removeHoliday'.

        for (int i = 0; SPECS[i]; ++i) {
            Obj mX(&testAllocator); const Obj& X = mX;
            gg(&mX, SPECS[i]);
            for (int j = 0; j < X.numHolidays(); ++j) {
                Obj mY(X, &testAllocator); const Obj& Y = mY;
                bdet_Date holidayToRemove;
                bsl::vector<int> codes;

                // Look for the 'j'-th holiday and save its holiday codes in
                // 'codes'.

                int k = 0;
                for (Obj::HolidayConstIterator it = Y.beginHolidays();
                                                 it != Y.endHolidays(); ++it) {
                    if (k == j) {
                        holidayToRemove = *it;
                        Obj::HolidayCodeConstIterator jt =
                                                       Y.beginHolidayCodes(it);
                        while (jt != Y.endHolidayCodes(it)) {
                            codes.push_back(*jt);
                            ++jt;
                        }
                        break;
                    }
                    ++k;
                }

                // Remove the holiday and verify the holiday count is one less
                // than before.

                int oldHolidayCount = mY.numHolidays();
                mY.removeHoliday(holidayToRemove);
                LOOP2_ASSERT(i, j, oldHolidayCount - 1 == mY.numHolidays());

                // Verify that removing a non-existing holiday has no effect on
                // the calendar object.

                Obj mZ(Y, &testAllocator); const Obj& Z = mZ;
                mZ.removeHoliday(holidayToRemove);
                LOOP2_ASSERT(i, j, Z == Y);

                // Verify that removing an out-of-range day has no effect on
                // the calendar object.

                if (Y.lastDate() < bdet_Date(9999, 12, 31)) {
                    mZ.removeHoliday(Y.lastDate() + 1);
                    LOOP2_ASSERT(i, j, Z == Y);
                }

                // Add the holiday back to 'Y' along with its original holiday
                // codes.  Verify that the result has the same value as the
                // original object.

                mY.addHoliday(holidayToRemove);
                for (bsl::vector<int>::const_iterator kt = codes.begin();
                        kt != codes.end(); ++kt) {
                   mY.addHolidayCode(holidayToRemove, *kt);
                }
                LOOP4_ASSERT(i, j, X, Y, X == Y);

                // Use 'removeHoliday' to remove every holiday in 'Y'.  Verify
                // that the calendar is in a valid state.

                for (Obj::HolidayConstIterator it = X.beginHolidays();
                                                 it != X.endHolidays(); ++it) {
                    mY.removeHoliday(*it);
                }
                mZ = X;
                mZ.removeAll();
                mZ.setValidRange(X.firstDate(), X.lastDate());

                for (Obj::WeekendDaysTransitionConstIterator itr =
                                               X.beginWeekendDaysTransitions();
                     itr != X.endWeekendDaysTransitions();
                     ++itr) {
                    mZ.addWeekendDaysTransition(itr->first, itr->second);
                }

                LOOP2_ASSERT(i, j, Y == Z);
            }
        }

        // Test 'removeHolidayCode'.

        for (int i = 0; SPECS[i]; ++i) {
            Obj mX(&testAllocator); const Obj& X = mX;
            gg(&mX, SPECS[i]);
            for (int j = 0; 1; ++j) {
                Obj mY(X, &testAllocator); const Obj& Y = mY;
                int codesFound = 0;
                bool found = false;
                int codeToRemove;
                bdet_Date atDate;

                // Search for the 'j'-th holiday code.

                for (Obj::HolidayConstIterator it = Y.beginHolidays();
                        it != Y.endHolidays(); ++it) {
                    for (Obj::HolidayCodeConstIterator jt =
                                                       Y.beginHolidayCodes(it);
                            jt != Y.endHolidayCodes(it); ++jt) {
                        if (codesFound == j) {
                            codeToRemove = *jt;
                            atDate = *it;
                            found = true;
                            break;
                        }
                        ++codesFound;
                    }
                    if (found) {
                        break;
                    }
                }
                if (!found) {
                    break; // end of loop, there is no more holiday codes in
                           // the calendar.
                }

                // Remove the holiday code and verify the holiday codes count
                // is one less than before.

                int numCodes = mY.numHolidayCodes(atDate);
                mY.removeHolidayCode(atDate, codeToRemove);
                LOOP2_ASSERT(i, j, numCodes - 1 == mY.numHolidayCodes(atDate));

                // Verify that removing a non-existing holiday code has no
                // effect on the calendar object.

                Obj mZ(Y, &testAllocator); const Obj& Z = mZ;
                mZ.removeHolidayCode(atDate, codeToRemove);
                LOOP2_ASSERT(i, j, Y == Z);

                // Verify that removing a holiday code on an out-of range day
                // has no effect on the calendar object.

                if (Y.lastDate() < bdet_Date(9999, 12, 31)) {
                    mZ.removeHolidayCode(Y.lastDate() + 1, codeToRemove);
                    LOOP2_ASSERT(i, j, Y == Z);
                }

                // Verify that removing a holiday code on a non-holiday has no
                // effect on the calendar object.

                Obj::BusinessDayConstIterator iter = Z.beginBusinessDays();
                if (iter != Z.endBusinessDays()) {
                    mZ.removeHolidayCode(*iter, codeToRemove);
                    LOOP2_ASSERT(i, j, Y == Z);
                }

                // Add the holiday code back and verify the result has the same
                // value as the original object.

                mY.addHolidayCode(atDate, codeToRemove);
                LOOP4_ASSERT(i, j, X, Y, X == Y);
            }
        }
      }

DEFINE_TEST_CASE(18) {
        // --------------------------------------------------------------------
        // TESTING 'addHolidayIfInRange' and 'addHolidayCodeIfInRange':
        //
        // Concerns:
        //  1. 'addHolidayIfInRange' adds a new holiday only if the holiday is
        //     within the valid range.
        //  2. 'addHolidayCodeIfInRange' adds a new holiday code to the holiday
        //     only if this holiday is within the valid range.  It also adds
        //     the holiday to the calendar if the holiday is in range and was
        //     not previously in the calendar.
        //
        // Plan:
        //  To address concerns 1 and 2, invoke these two methods on holidays
        //  inside and outside of the valid ranges and verify that a new
        //  holiday/holiday code is added if and only if the holiday is within
        //  the valid range.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Array-Based Implementation Technique
        //
        // Testing:
        //      int addHolidayIfInRange(const bdet_Date&)
        //      int addHolidayCodeIfInRange(const bdet_Date&, int)
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing 'addHolidayIfInRange' and" << endl
                          << "        'addHolidayCodeIfInRange'" << endl
                          << "=================================" << endl;

        static const bdet_Date DATES[] = {
            bdet_Date(   1,  1,  1), bdet_Date(2004,  2, 28),
            bdet_Date(2004,  2, 29), bdet_Date(2004, 12,  1),
            bdet_Date(2004, 12, 31), bdet_Date(2005,  1,  1),
            bdet_Date(9999, 12, 31),
        };
        const int NUM_DATES = sizeof DATES / sizeof *DATES;

        Obj mX(&testAllocator); const Obj& X = mX;

        // Set the range to be [2004/2/29, 2004/12/31].

        mX.setValidRange(DATES[2], DATES[NUM_DATES-3]);
        for (int i = 0; i < NUM_DATES; ++i) {
            mX.addHolidayIfInRange(DATES[i]);
            if (mX.isInRange(DATES[i])) {
                LOOP_ASSERT(i, mX.isHoliday(DATES[i]));
            }
        }

        mX.removeAll();
        mX.setValidRange(DATES[2], DATES[NUM_DATES-3]);
        for (int i = 0; i < NUM_DATES; ++i) {
            int code = 1;
            mX.addHolidayCodeIfInRange(DATES[i], code);
            if (mX.isInRange(DATES[i])) {
                LOOP_ASSERT(i, mX.isHoliday(DATES[i]));
                Obj::HolidayCodeConstIterator it =
                                                mX.beginHolidayCodes(DATES[i]);
                LOOP_ASSERT(i, it != mX.endHolidayCodes(DATES[i]));
                LOOP_ASSERT(i, *it == code);
                ++it;
                LOOP_ASSERT(i, it == mX.endHolidayCodes(DATES[i]));
            }
        }
      }

DEFINE_TEST_CASE(17) {
        // --------------------------------------------------------------------
        // TESTING 'setValidRange':
        //
        // Concerns:
        //  1. 'setValidRange' properly sets the valid range of a calendar
        //     without affecting the weekend days of the calendar.
        //  2. 'setValidRange' removes the holidays that are outside of the
        //     valid range of the calendar when the range shrinks.
        //
        // Plan:
        //  To address concerns 1 and 2, divide the range [1/1/1, 9999/12/31]
        //  into several subranges.  Specify a set of holidays (or no holiday)
        //  for each of the subranges.  Then, for each of these subranges, use
        //  'setValidRange' to first expand then shrink it.  Then we verify
        //  that during this process the weekend days of the calendar
        //  are not affected and that the holidays that fall outside of the
        //  calendar during a shrink are removed.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Loop-Based Implementation Technique
        //
        // Testing:
        //      void setValidRange(const bdet_Date&, const bdet_Date&)
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing 'setValidRange'" << endl
                          << "=======================" << endl;

        // The array below defined N ranges.  They are: [ RANGES[0],
        // RANGES[1] ), [ RANGES[1], RANGES[2] ), ..., [ RANGES[N-2],
        // RANGES[N-1] ), and [ RANGES[N-1], bdet_Date(9999, 12, 31) ].  Here N
        // is the number of dates in this array.

        static const bdet_Date RANGES[] = {
            bdet_Date(   1,  1,  1), bdet_Date(1999,  1,  1),
            bdet_Date(2005,  1,  1), bdet_Date(2010,  1,  1),
            bdet_Date(5678,  1,  1), bdet_Date(5678,  1,  2),
        };
        const int NUM_RANGES = sizeof RANGES / sizeof *RANGES;

        static const bdet_Date HOLIDAYS[] = {
            // Holidays for range #1

            bdet_Date(   1,  1,  1), bdet_Date(   1,  1,  7),
            bdet_Date(1234,  5,  6), bdet_Date(1997,  7,  1),
            bdet_Date(1998,  12, 31), bdet_Date(1,  1,  2),

            // Holidays for range #2

            bdet_Date(1999,  3,  1), bdet_Date(2004, 12, 31),

            // Holidays for range #3

            bdet_Date(2006, 10,  1), bdet_Date(2008,  7,  4),
            bdet_Date(2009,  9, 10), bdet_Date(2009,  9, 11),

            // Holidays for range #4

            bdet_Date(5555,  5,  5), bdet_Date(5566,  5,  6),

            // No holidays for range #5

            // Holidays for range #6

            bdet_Date(8888,  8,  8), bdet_Date(9999, 12, 31),
        };
        const int NUM_HOLIDAYS = sizeof HOLIDAYS / sizeof *HOLIDAYS;

        static const bdet_DayOfWeek::Day WEEKENDDAYS[] = {
            bdet_DayOfWeek::BDET_SUN,
            bdet_DayOfWeek::BDET_WED,
            bdet_DayOfWeek::BDET_SAT,
        };

        bsl::vector<Obj::WeekendDaysTransition> weekendDaysTransitions;
        {
            bdet_Date date(1, 1, 1);
            bdec_DayOfWeekSet weekendDays;
            weekendDays.add(bdet_DayOfWeek::BDET_SUN);
            weekendDaysTransitions.push_back(Obj::WeekendDaysTransition(
                                                           date, weekendDays));

            date.setYearMonthDay(1000, 1, 1);
            weekendDays.removeAll();
            weekendDays.add(bdet_DayOfWeek::BDET_SAT);
            weekendDays.add(bdet_DayOfWeek::BDET_MON);
            weekendDaysTransitions.push_back(Obj::WeekendDaysTransition(
                                                           date, weekendDays));

            date.setYearMonthDay(2000, 1, 1);
            weekendDays.removeAll();
            weekendDays.add(bdet_DayOfWeek::BDET_WED);
            weekendDays.add(bdet_DayOfWeek::BDET_THU);
            weekendDaysTransitions.push_back(Obj::WeekendDaysTransition(
                                                           date, weekendDays));
        }

        Obj mX(&testAllocator); const Obj& X = mX;

        for (bsl::vector<Obj::WeekendDaysTransition>::const_iterator itr =
                                                weekendDaysTransitions.begin();
             itr != weekendDaysTransitions.end();
             ++itr) {
            mX.addWeekendDaysTransition(itr->first, itr->second);
        }

        for (int i = 0; i <= NUM_RANGES; ++i) {
            // Vary the end date to expand/shrink the range.

            // Here 'i' is pointing at the beginning of the range, and 'j' is
            // pointer at one day past the end of the range.  We start with an
            // empty range, so let 'i = j'.

            int j = i;

            bool shrinkRangeFlag = false;
                // 'true' if shrinking the valid range of the calendar, 'false'
                // if expanding depending on this flag.  We will add/remove one
                // of 'RANGES[j]' to/from the valid range of the calendar.

            int startDateIndex = i;
            do {
                bdet_Date startDate;
                bdet_Date endDate;

                // Load the '[RANGES[startDateIndex] ... RANGES[j] - 1]'
                // boundaries into 'startDate' and 'endDate'.

                if (j != 0) {
                    startDate =  startDateIndex == NUM_RANGES
                               ? bdet_Date(9999, 12, 31)
                               : RANGES[startDateIndex];
                    endDate   =  j == NUM_RANGES
                               ? bdet_Date(9999, 12, 31)
                               : RANGES[j] - 1;
                }
                else {
                    // when 'j == 0', 'RANGES[j]' is bdet_Date(1, 1, 1).  We
                    // cannot subtract 1 from it.  So we handle it as a special
                    // case here.

                    startDate = bdet_Date(9999, 12, 31);
                    endDate   = bdet_Date(   1,  1,  1);
                }

                if (veryVerbose) {
                    P(startDate);
                    P(endDate);
                    P(mX);
                }
                mX.setValidRange(startDate, endDate);

                if (startDate <= endDate) {
                    LOOP2_ASSERT(startDateIndex, j,
                                                   startDate == X.firstDate());
                    LOOP2_ASSERT(startDateIndex, j, endDate   == X.lastDate());
                }
                else {
                    LOOP2_ASSERT(startDateIndex, j, bdet_Date(9999,12,31) ==
                                                                X.firstDate());
                    LOOP2_ASSERT(startDateIndex, j, bdet_Date(1, 1, 1) ==
                                                                 X.lastDate());
                }

                // Verify that weekend-days transitions have not been modified
                LOOP2_ASSERT(weekendDaysTransitions.size(),
                             X.numWeekendDaysTransitions(),
                             weekendDaysTransitions.size() ==
                                                X.numWeekendDaysTransitions());
                ASSERT(bsl::equal(X.beginWeekendDaysTransitions(),
                                  X.endWeekendDaysTransitions(),
                                  weekendDaysTransitions.begin()));

                // Verify at least all the expected holidays are present.

                int holidayCount = 0;
                for (int k = 0; k < NUM_HOLIDAYS; ++k) {
                    if (shrinkRangeFlag) {
                        // If we are shrinking the valid range of the calendar,
                        // then all the holidays that are within
                        // '[RANGES[startDateIndex] ... RANGES[j] - 1' should
                        // be in the valid range of the calendar.

                        if (X.isInRange(HOLIDAYS[k])) {
                            LOOP3_ASSERT(startDateIndex, j, k,
                                         X.isHoliday(HOLIDAYS[k]));
                        }
                    }
                    if (X.isInRange(HOLIDAYS[k]) && X.isHoliday(HOLIDAYS[k])) {
                        ++holidayCount;  // count expected holidays
                    }
                }

                // Verify there is no more than the expected holidays in the
                // calendar.

                LOOP2_ASSERT(startDateIndex, j,
                                              holidayCount == X.numHolidays());

                if (shrinkRangeFlag == false) {
                    // If we are expanding, add new holidays to it.

                    for (int k = 0; k < NUM_HOLIDAYS; ++k) {
                        if (X.isInRange(HOLIDAYS[k])) {
                            mX.addHoliday(HOLIDAYS[k]);
                        }
                    }
                }

                if (j == NUM_RANGES) {
                    // We are done expanding the valid range of the calendar.
                    // Now shrink it.

                    shrinkRangeFlag = true;
                }
                j += (shrinkRangeFlag ? -1 : 1);
                startDateIndex += (shrinkRangeFlag ? 1 : 0);

            } while (j > startDateIndex);
        }
      }

DEFINE_TEST_CASE(16) {
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
      }

DEFINE_TEST_CASE(15) {
        // --------------------------------------------------------------------
        // TESTING  'isWeekendDay(bdet_Date&)', 'isBusinessDay',
        //          'isNonBusinessDay', 'numWeekendDaysInRange',
        //          'numBusinessDays', and 'numNonBusinessDays'
        //
        // Concerns:
        //: 1 'isWeekendDay' properly identifies weekend days.
        //:
        //: 2 'isBusinessDay' properly identifies business days.
        //:
        //: 3 'isNonBusinessDay' properly identifies non-business days.
        //:
        //: 4 'numWeekendDaysInRange' returns the total number of weekend days
        //:   in the entire range of the calendar.
        //:
        //: 5 'numBusinessDays' returns the total number of business days in
        //:   the entire range of the calendar.
        //:
        //: 6 'numNonBusinessDays' returns the total number of non-business
        //:   days in the entire range of the calendar.
        //
        // Plan:
        //: 1 Using a table-driven approach, specify a list of weekend-days
        //:   transitions each comprising a transition date and a (varied and
        //:   substantially different) set of weekend days in ascending
        //:   transition date order.
        //:
        //: 2 For each row in the table described in P-1:
        //:
        //:   1 Create a calendar having a range that encompases all the
        //:     transition dates in the table.  Using the
        //:     'addWeekendDaysTransition' method, add all transitions
        //:     represented from the first row in the table up to the current
        //:     row.
        //:
        //:   2 Call 'isWeekendDay', 'isBusinessDay', and 'isNonBusinessDay'
        //:     for each date in the calendar and verify the correct values are
        //:     returned.  (C-1..3)
        //:
        //:   3 Verify that 'numWeekendDaysInRange', 'numBusinessDays', and
        //:     'numNonBusinessDays' returns the correct values for the
        //:     calendar.  (C-4..6)
        //:
        //:   4 Add a hard coded list of holiday dates to the calendar.
        //:
        //:   5 Repeat P-2 and P-3.  (C-1..6)
        //:
        //:   6 Iterate using a 'BusinessDayConstIterator' object from
        //:     'beginBusinessDays()' up to 'endBusinessDays()' and ensure that
        //:     both the holiday dates and weekend days are properly skipped by
        //:     the iterator.
        //
        // Testing:
        //    int isWeekendDay(bdet_Date&) const
        //    int numBusinessDays() const
        //    int numNonBusinessDays() const
        //    bool isBusinessDay(const bdet_Date& date) const
        //    bool isNonBusinessDay(const bdet_Date& date) const
        //    bool isWeekendDay(const bdet_Date& date) const
        // --------------------------------------------------------------------

        if (verbose) cout << endl
            << "TESTING  'isWeekendDay(bdet_Date&)', 'isBusinessDay'," << endl
            << "         'isNonBusinessDay', 'numWeekendDaysInRange'," << endl
            << "         'numBusinessDays', and 'numNonBusinessDays'," << endl;


        static const struct {
            int d_line;
            int d_year;
            int d_month;
            int d_day;
            const char *d_weekendDays;
        } DATA[] = {
            //LINE  YYYY  MM  DD   SMTWTFS
            //----  ----  --  --   -------
            // default transition at 1/1/1 (not included in the calendar range)
            { L_,      1,  1,  1, "1000001" },

            { L_,   2000,  1,  1, "0000000" },
            { L_,   2000,  1, 10, "0000001" },
            { L_,   2000,  2, 10, "0000010" },
            { L_,   2000,  3, 10, "0000100" },
            { L_,   2000,  4, 10, "0001000" },
            { L_,   2000,  5, 10, "0010000" },
            { L_,   2000,  6, 10, "0100000" },
            { L_,   2000,  6, 20, "0000000" },
            { L_,   2000,  7, 10, "1000000" },
            { L_,   2000,  8, 10, "1000001" },
            { L_,   2000,  9, 10, "1100000" },
            { L_,   2000, 10, 10, "0110000" },
            { L_,   2000, 11, 10, "0011000" },
            { L_,   2001,  1, 10, "0001100" },
            { L_,   2001,  2, 10, "0000110" },
            { L_,   2001,  3, 10, "0000011" },
            { L_,   2001,  4, 10, "1111111" },
        };

        const int NUM_DATA = sizeof DATA/sizeof *DATA;

        const bdet_Date FIRST_DATE(bdet_Date(DATA[1].d_year,
                                             DATA[1].d_month,
                                             DATA[1].d_day) - 20);
        const bdet_Date LAST_DATE(
                                bdet_Date(DATA[NUM_DATA - 1].d_year,
                                          DATA[NUM_DATA - 1].d_month,
                                          DATA[NUM_DATA - 1].d_day + 10) + 20);

        bslma_TestAllocator oa("oa", veryVeryVerbose);

        for (int ti = 0; ti < NUM_DATA; ++ti) {

            Obj mX(FIRST_DATE, LAST_DATE, &oa); const Obj& X = mX;

            typedef bsl::vector<bsl::pair<bdet_Date, bdec_DayOfWeekSet> >
                                                              TransitionVector;
            TransitionVector transitions;

            for (int tj = 0; tj <= ti; ++tj)
            {
                bdet_Date date(DATA[tj].d_year,
                               DATA[tj].d_month,
                               DATA[tj].d_day);
                bdec_DayOfWeekSet weekendDays;
                int wdIndex = 1;
                for(const char *wkStr = DATA[tj].d_weekendDays;
                    *wkStr; ++wkStr, ++wdIndex) {
                    if (*wkStr == '1') {
                        if (veryVeryVerbose) {
                            P(wdIndex)
                        }
                        weekendDays.add(
                                    static_cast<bdet_DayOfWeek::Day>(wdIndex));
                    }
                }

                if (veryVerbose) {
                    P_(date) P(weekendDays)
                }
                mX.addWeekendDaysTransition(date, weekendDays);
                transitions.push_back(bsl::make_pair(date, weekendDays));
            }

            // verify methods
            {
                int curTransI = 0;
                int numWeekendDays = 0;
                for (bdet_Date date = FIRST_DATE; date <= LAST_DATE; ++date) {
                    if (curTransI < transitions.size() - 1 &&
                        date >= transitions[curTransI + 1].first) {
                        ++curTransI;
                    }
                    bool isWeekend =
                        transitions[curTransI].second.isMember(
                                                             date.dayOfWeek());

                    if (isWeekend) {
                        ++numWeekendDays;
                    }

                    LOOP2_ASSERT(isWeekend,
                                 date,
                                 isWeekend == X.isWeekendDay(date));
                    LOOP2_ASSERT(isWeekend,
                                 date,
                                 isWeekend == X.isNonBusinessDay(date));
                    LOOP2_ASSERT(isWeekend,
                                 date,
                                 isWeekend != X.isBusinessDay(date));
                }

                LOOP_ASSERT(numWeekendDays,
                            numWeekendDays == X.numWeekendDaysInRange());
                LOOP_ASSERT(numWeekendDays,
                            numWeekendDays == X.numNonBusinessDays());

                LOOP_ASSERT(numWeekendDays,
                            X.length() - numWeekendDays ==
                                                          X.numBusinessDays());
            }

            bsl::vector<bdet_Date> holidays(&oa);
            holidays.push_back(bdet_Date(2000, 1,17));
            holidays.push_back(bdet_Date(2000, 2,21));
            holidays.push_back(bdet_Date(2000, 2,22));
            holidays.push_back(bdet_Date(2000, 4,21));
            holidays.push_back(bdet_Date(2000, 5,14));
            holidays.push_back(bdet_Date(2000, 7, 4));
            holidays.push_back(bdet_Date(2000, 9, 4));
            holidays.push_back(bdet_Date(2000,11,23));
            holidays.push_back(bdet_Date(2000,12,25));

            for (bsl::vector<bdet_Date>::const_iterator itr = holidays.begin();
                 itr != holidays.end();
                 ++itr) {
                mX.addHoliday(*itr);
            }

            // verify methods
            {
                int curTransI = 0;
                int numWeekendDays = 0;
                int numNonBusinessDays = 0;
                for (bdet_Date date = FIRST_DATE; date <= LAST_DATE; ++date) {
                    if (curTransI < transitions.size() - 1 &&
                        date >= transitions[curTransI + 1].first) {
                        ++curTransI;
                    }
                    bool isWeekend =
                        transitions[curTransI].second.isMember(
                                                             date.dayOfWeek());

                    bool isHoliday = 0 < std::count(holidays.begin(),
                                                    holidays.end(),
                                                    date);
                    bool isNonBusinessDay = isWeekend || isHoliday;

                    if (isWeekend) {
                        ++numWeekendDays;
                    }
                    if (isNonBusinessDay) {
                        ++numNonBusinessDays;
                    }

                    LOOP2_ASSERT(isWeekend,
                                 date,
                                 isWeekend == X.isWeekendDay(date));
                    LOOP2_ASSERT(isWeekend,
                                 date,
                                 isNonBusinessDay == X.isNonBusinessDay(date));
                    LOOP2_ASSERT(isWeekend,
                                 date,
                                 isNonBusinessDay != X.isBusinessDay(date));
                }

                LOOP2_ASSERT(numWeekendDays, numNonBusinessDays,
                             numWeekendDays != numNonBusinessDays);
                LOOP_ASSERT(numWeekendDays,
                            numWeekendDays == X.numWeekendDaysInRange());
                LOOP_ASSERT(numNonBusinessDays,
                            numNonBusinessDays == X.numNonBusinessDays());

                LOOP_ASSERT(numWeekendDays,
                            X.length() - numNonBusinessDays ==
                                                          X.numBusinessDays());
            }

            // verify business day iterator skips holidays and weekend days
            {
                Obj::BusinessDayConstIterator itr = X.beginBusinessDays();
                bdet_Date prevDate = X.firstDate();

                while (itr != X.endBusinessDays()) {

                    bdet_Date curDate = *itr;
                    if ((prevDate + 1) != curDate) {
                        // ensure that the skipped days are weekend days or
                        // holidays
                        for (bdet_Date d = prevDate + 1; d < curDate; ++d) {
                            LOOP_ASSERT(d,
                                        X.isWeekendDay(d) ||
                                        0 < std::count(holidays.begin(),
                                                       holidays.end(),
                                                       d));
                        }
                    }

                    // ensure that current date is not a weekend day or a
                    // holiday
                    LOOP_ASSERT(curDate,
                                !(X.isWeekendDay(curDate) ||
                                  0 < std::count(holidays.begin(),
                                                 holidays.end(),
                                                 curDate)));

                    prevDate = curDate;
                    ++itr;
                }

                if (prevDate != LAST_DATE) {
                // ensure that the skipped days are weekend days or holidays
                    for (bdet_Date d = prevDate + 1; d <= LAST_DATE; ++d) {
                        LOOP_ASSERT(d,
                                    X.isWeekendDay(d) ||
                                    0 < std::count(holidays.begin(),
                                                   holidays.end(),
                                                   d));
                    }
                }
            }
        }
}

DEFINE_TEST_CASE(14) {
        // --------------------------------------------------------------------
        // TESTING 'numHolidays' and 'numHolidayCodes':
        //
        // Concerns:
        //  1. 'numHolidays' reports the number of holidays in a calendar.
        //  2. 'numHolidayCodes' reports the number of holiday codes associated
        //     with a holiday.  It returns 0 if the date supplied is not a
        //     holiday.
        //
        // Plan:
        //  To address concern 1, we create a calendar object and add each day
        //  of an arbitrarily-selected month as a holiday to the calendar.  We
        //  check the results reported by 'numHolidays' after each holiday is
        //  added to make sure that they agree with the number of holidays we
        //  have added.
        //
        //  To address concern 2, we first make sure that 0 is returned when we
        //  invoke 'numHolidayCodes' on a non-holiday.  Then we add between 0
        //  and 38 holiday codes to a holiday and verify the result returned
        //  equals the number of codes we have added.
        //
        // Tactics:
        //      - Ad-Hoc/Enumeration Data Selection Methods
        //      - Loop-Based Implementation Technique
        //
        // Testing:
        //      int numHolidays() const
        //      int numHolidayCodes(const bdet_Date& date) const
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing numHolidays and numHolidayCodes" << endl
                          << "=======================================" << endl;

        Obj mX(&testAllocator); const Obj& X = mX;
        for (int day = 1; day <= 31; ++day) {
            bdet_Date holiday(2004, 1, day);
            mX.addDay(holiday);
            LOOP_ASSERT(day, 0 == X.numHolidayCodes(holiday));
            mX.addHoliday(holiday);
            LOOP_ASSERT(day, X.numHolidays() == day);
            LOOP_ASSERT(day, 0 == X.numHolidayCodes(holiday));
            for (int code = 1; code <= 38; ++code) {
                mX.addHolidayCode(holiday, code);
                LOOP2_ASSERT(day, code, X.numHolidayCodes(holiday) == code);
            }
        }
      }

DEFINE_TEST_CASE(13) {
        // --------------------------------------------------------------------
        // TESTING 'numWeekendDaysInRange':
        //
        // Concerns:
        //  1. 'numWeekendDaysInRange' returns the total number of weekend days
        //     within the valid range of the calendar regardless of the length
        //     or the start date of the calendar.
        //
        // Plan:
        //  To address concern 1, we create calendar objects with lengths
        //  varying from 0 to 15 and start dates on every day of the week.
        //  The number 15 was chosen because we want to test
        //  'numWeekendDaysInRange' with lengths up to at least two complete
        //  weeks to ensure that all possible cases are covered.  For each of
        //  these objects, we loop through all possible 128 weekend days
        //  combinations (using the method described in concern 1) and verify
        //  that the return values of 'numWeekendDaysInRange' agree with the
        //  number of weekend days we obtained by going through all dates
        //  within the valid range.
        //
        // Tactics:
        //      - Enumeration Data Selection Method
        //      - Loop-Based Implementation Technique
        //
        // Testing:
        //      int numWeekendDaysInRange() const
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing numWeekendDaysInRange" << endl
                          << "=============================" << endl;

        static const bdet_DayOfWeek::Day DAYS[] = {
            bdet_DayOfWeek::BDET_SUN,
            bdet_DayOfWeek::BDET_MON,
            bdet_DayOfWeek::BDET_TUE,
            bdet_DayOfWeek::BDET_WED,
            bdet_DayOfWeek::BDET_THU,
            bdet_DayOfWeek::BDET_FRI,
            bdet_DayOfWeek::BDET_SAT,
        };

        // test 'numWeekendDaysInFirstTransition' test apparatus,
        // also test 'numWeekendDaysInRange' with an empty calendar.

        for (int i = 0; i <= 0x7F; ++i) {
            bdecs_PackedCalendar obj(&testAllocator);

            int bitCount = 0;
            for (int dow = 0; dow < 7; ++ dow) {
                if (i & (1 << dow)) {
                    obj.addWeekendDay(DAYS[dow]);
                    ++bitCount;
                }
            }
            LOOP_ASSERT(i, numWeekendDaysInFirstTransition(obj) == bitCount);
            LOOP_ASSERT(i, 0 == obj.numWeekendDaysInRange());
        }

        // Testing 'numWeekendDaysInRange' with non-empty calendars.

        for (int day = 0; day < 7; ++day) {
            for (int len = 0; len <= 14; ++len) {
                for (int i = 0; i <= 0x7F; ++i) {
                    bdecs_PackedCalendar obj(&testAllocator);
                    obj.addDay(bdet_Date(2000, 1, 1) + day);
                    obj.addDay(obj.firstDate() + len);

                    for (int dow = 0; dow < 7; ++ dow) {
                        if (i & (1 << dow)) {
                            obj.addWeekendDay(DAYS[dow]);
                        }
                    }

                    // Counting weekend days within the range of this calendar.

                    int weekendDayCount = 0;
                    for (bdet_Date d = obj.firstDate(); d <= obj.lastDate();
                                                                         ++d) {
                        if (obj.isWeekendDay(d.dayOfWeek())) {
                            ++weekendDayCount;
                        }
                    }

                    LOOP3_ASSERT(day, len, i,
                               weekendDayCount == obj.numWeekendDaysInRange());
                }
            }
        }
      }

DEFINE_TEST_CASE(12) {
        // --------------------------------------------------------------------
        // TESTING 'addWeekendDays' and 'addWeekendDay':
        //
        // Concerns:
        //  1. 'addWeekendDays' properly adds the weekend days in the
        //     'bdec_DayOfWeekSet' to the calendar object.  If the calendar
        //     object already had weekend days before, it should now have the
        //     union of its original weekend days and the new weekend days
        //     after this method is invoked.
        //
        //  2. 'isWeekendDay(bdet_Date&)' properly identifies weekend days.
        //
        //  3.  Assert precondition violation are detected when enabled.
        //
        //  4. 'addWeekendDays' and 'addWeekendDay' logically adds to the
        //     default weekend-days transition at 1/1/1.
        //
        // Plan:
        //  To address concern 1 and 4, we use the first 7 bits of two integers
        //  to represent all possible combinations of the weekend days that
        //  exist in the calendar and the weekend days that are being added,
        //  respectively.  For each of the 128x128 cases, we generate the
        //  weekend days set, add it to the calendar object, and verify the
        //  resulting weekend days are the combined set of old and new weekend
        //  days and the weekend days been added is in the first weekend-days
        //  transition.
        //
        //  To address concern 2, we use the tested
        //  'isWeekendDay(bdet_DayOfWeek::Day)' method to verify the results of
        //  'isWeekendDay(bdet_Date&)' by invoking both methods on all the days
        //  of an arbitrarily-selected month and compare their results.
        //
        //  To address concern 3, we use the 'BSLS_ASSERTTEST_*' macros to
        //  verify defensive checks are triggered when an attempt is made to
        //  call 'addWeekendDays' when a weekend-days transition at a start
        //  date that is not 1/1/1 have already been added to this calendar.
        //
        // Tactics:
        //      - Ad-Hoc/Enumeration Data Selection Methods
        //      - Loop-Based Implementation Technique
        //
        // Testing:
        //      void addWeekendDays(const bdec_DayOfWeekSet& weekendDays)
        //      bool isWeekendDay(const bdet_Date& date) const
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

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

                // Set up the current weekend days before adding new weekend
                // days
                ASSERT(X.numWeekendDaysTransitions() == 1);
                ASSERT(X.beginWeekendDaysTransitions()->first ==
                                                           bdet_Date(1, 1, 1));
                ASSERT(X.beginWeekendDaysTransitions()->second.length() == 0);

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
        {
            bslma_TestAllocator oa("oa", veryVeryVerbose);
            Obj Y(bdet_Date(2012, 1, 1), bdet_Date(2012, 12, 31), &oa);
            bdec_DayOfWeekSet weekendDays;
            weekendDays.add(bdet_DayOfWeek::BDET_SUN);
            Y.addWeekendDaysTransition(bdet_Date(2012, 1, 1),
                                       weekendDays);

            bsls_AssertFailureHandlerGuard hG(bsls_AssertTest::failTestDriver);
            ASSERT_SAFE_FAIL(Y.addWeekendDay(bdet_DayOfWeek::BDET_SUN));
        }
      }


DEFINE_TEST_CASE(11) {
        // --------------------------------------------------------------------
        // TESTING INITIAL-VALUE CONSTRUCTOR:
        //
        // Concerns:
        //  1. The initial value is correct.
        //  2. The constructor is exception neutral w.r.t. memory allocation.
        //  3. The internal memory management system is hooked up properly so
        //     that *all* internally allocated memory (if any) draws from a
        //     user-supplied allocator whenever one is specified.
        //
        // Plan:
        //  For each constructor we will create objects
        //    - Without passing in an allocator.  We will install an allocator
        //      as the default and verify that memory is drawn from it.
        //    - With an allocator.  We will install an allocator as the default
        //      and verify that memory is drawn from the allocator passed in
        //      and not from the default allocator.
        //    - In the presence of exceptions during memory allocations using a
        //      'bslma_TestAllocator' and varying its *allocation* *limit*.
        //    - Where the object is constructed entirely in static memory
        //      (using a 'bdema_BufferedSequentialAllocator') and never
        //      destroyed.
        //
        //  We will use basic accessors to verify the first and last date of
        //  this object.  We will also verify that this object has no holiday
        //  or weekend day.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Loop-Based Implementation Technique
        //
        // Testing:
        //      bdecs_PackedCalendar::bdecs_PackedCalendar(
        //                                     const bdet_Date& firstDate,
        //                                     const bdet_Date& lastDate,
        //                                     bslma_Allocator *basicAllocator)
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing Initial-Value Constructor" << endl
                          << "=================================" << endl;

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

                int blocks = da.numBlocksTotal();
                Obj mX(dateFirst, dateLast);  const Obj& X = mX;

                // Verify that memory is drawn from the default allocator.

                LOOP_ASSERT(i, blocks <= da.numBlocksTotal());

                if (veryVerbose) P(X);
                if (dateFirst > dateLast) {
                    LOOP_ASSERT(i, X.firstDate() == bdet_Date(9999, 12, 31));
                    LOOP_ASSERT(i, X.lastDate()  == bdet_Date(   1,  1,  1));
                }
                else {
                    LOOP_ASSERT(i, X.firstDate() == dateFirst);
                    LOOP_ASSERT(i, X.lastDate()  == dateLast );
                }
                LOOP_ASSERT(i, X.beginHolidays() == X.endHolidays());
                LOOP_ASSERT(i, X.numWeekendDaysTransitions() == 1);
                LOOP_ASSERT(i, X.beginWeekendDaysTransitions()->first
                                                        == bdet_Date(1, 1, 1));
                LOOP_ASSERT(i,X.beginWeekendDaysTransitions()->second.length()
                                                                         == 0);
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
                int blocks = testAllocator.numBlocksTotal();
                int blocksDefault = da.numBlocksTotal();
                Obj mX(dateFirst, dateLast, &testAllocator);

                // Verify that memory is drawn from the specified allocator and
                // not from the default allocator.

                LOOP_ASSERT(i, blocks <= testAllocator.numBlocksTotal());
                LOOP_ASSERT(i, blocksDefault == da.numBlocksTotal());

                const Obj& X = mX;
                if (veryVerbose) P(X);
                if (dateFirst > dateLast) {
                    LOOP_ASSERT(i, X.firstDate() == bdet_Date(9999, 12, 31));
                    LOOP_ASSERT(i, X.lastDate()  == bdet_Date(   1,  1,  1));
                }
                else {
                    LOOP_ASSERT(i, X.firstDate() == dateFirst);
                    LOOP_ASSERT(i, X.lastDate()  == dateLast );
                }
                LOOP_ASSERT(i, X.beginHolidays() == X.endHolidays());
                LOOP_ASSERT(i, X.numWeekendDaysTransitions() == 1);
                LOOP_ASSERT(i, X.beginWeekendDaysTransitions()->first
                                                        == bdet_Date(1, 1, 1));
                LOOP_ASSERT(i,X.beginWeekendDaysTransitions()->second.length()
                                                                         == 0);
            }
        }

        if (verbose) cout << "\t\tWith exceptions." << endl;
        {
            for (int i = 0; i < NUM_DATA; ++i) {
              BEGIN_BSLMA_EXCEPTION_TEST {
                const int DFIRST = DATA[i].d_first;
                const int DLAST  = DATA[i].d_last;

                bdet_Date dateFirst(DFIRST / 10000, DFIRST / 100 % 100,
                                    DFIRST % 100);
                bdet_Date dateLast (DLAST  / 10000, DLAST  / 100 % 100,
                                    DLAST  % 100);
                Obj mX(dateFirst, dateLast, &testAllocator);
                const Obj& X = mX;
                if (veryVerbose) P(X);
                if (dateFirst > dateLast) {
                    LOOP_ASSERT(i, X.firstDate() == bdet_Date(9999, 12, 31));
                    LOOP_ASSERT(i, X.lastDate()  == bdet_Date(   1,  1,  1));
                }
                else {
                    LOOP_ASSERT(i, X.firstDate() == dateFirst);
                    LOOP_ASSERT(i, X.lastDate()  == dateLast );
                }
                LOOP_ASSERT(i, X.beginHolidays() == X.endHolidays());
                LOOP_ASSERT(i, X.numWeekendDaysTransitions() == 1);
                LOOP_ASSERT(i, X.beginWeekendDaysTransitions()->first
                                                        == bdet_Date(1, 1, 1));
                LOOP_ASSERT(i,X.beginWeekendDaysTransitions()->second.length()
                                                                         == 0);
              } END_BSLMA_EXCEPTION_TEST
            }
        }
        if (verbose) cout << "\tIn place using a buffer allocator." << endl;
        {
            char memory[1024];
            bdema_BufferedSequentialAllocator a(memory, sizeof memory);
            for (int i = 0; i < NUM_DATA; ++i) {
                const int DFIRST = DATA[i].d_first;
                const int DLAST  = DATA[i].d_last;

                bdet_Date dateFirst(DFIRST / 10000, DFIRST / 100 % 100,
                                    DFIRST % 100);
                bdet_Date dateLast (DLAST  / 10000, DLAST  / 100 % 100,
                                    DLAST  % 100);
                Obj *doNotDelete = new(a.allocate(sizeof(Obj)))
                                                  Obj(dateFirst, dateLast, &a);
                Obj& mX = *doNotDelete;  const Obj& X = mX;
                if (veryVerbose) P(X);
                if (dateFirst > dateLast) {
                    LOOP_ASSERT(i, X.firstDate() == bdet_Date(9999, 12, 31));
                    LOOP_ASSERT(i, X.lastDate()  == bdet_Date(   1,  1,  1));
                }
                else {
                    LOOP_ASSERT(i, X.firstDate() == dateFirst);
                    LOOP_ASSERT(i, X.lastDate()  == dateLast );
                }
                LOOP_ASSERT(i, X.beginHolidays() == X.endHolidays());
                LOOP_ASSERT(i, X.numWeekendDaysTransitions() == 1);
                LOOP_ASSERT(i, X.beginWeekendDaysTransitions()->first
                                                        == bdet_Date(1, 1, 1));
                LOOP_ASSERT(i,X.beginWeekendDaysTransitions()->second.length()
                                                                         == 0);
            }

            // No destructor is called; will produce memory leak in purify
            // if internal allocators are not hooked up properly.
        }
      }


namespace {

bool testCase10VerifyInvalidDataTest(const Obj Xs[], const Obj YYs[],
                                     const int LODs[],
                                     const int NUM_DATA,
                                     const Obj& XX, const int i)
{
    for (int di = 0; di < NUM_DATA; ++di) {
        if (i < LODs[di]) {
            if (Xs[di] != XX) return false;
        }
        else {
            if (Xs[di] != YYs[di]) return false;
        }
    }
    return true;
}

}  // close unnamed namespace

DEFINE_TEST_CASE(10) {
        // --------------------------------------------------------------------
        // TESTING STREAMING FUNCTIONALITY:
        //
        // Concerns:
        //  1. The available bdex stream functions are implemented
        //     using the respective member functions 'bdexStreamOut' and
        //     'bdexStreamIn'.
        //  2. Streaming must be exception safe with a guarantee of rollback
        //     with respect to memory allocation or streaming in values.
        //  3. Ensure that streaming works under the following conditions:
        //      VALID - may contain any sequence of valid values.
        //      EMPTY - valid, but contains no data.
        //      INVALID - may or may not be empty.
        //      INCOMPLETE - the stream is truncated, but otherwise valid.
        //      CORRUPTED - the data contains explicitly inconsistent fields.
        //  4. both version 1 and version 2 of the streaming format are
        //     available. Version 2 supports calendars with more than one
        //     weekend-days transition,
        //
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
            ASSERT(2 == X.maxSupportedBdexVersion());
            if (verbose) cout << "\tusing class method syntax:" << endl;
            ASSERT(2 == Obj::maxSupportedBdexVersion());
        }

        const int VERSION = Obj::maxSupportedBdexVersion();
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
                ASSERT(X == t);
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

                            LOOP2_ASSERT(U_SPEC, V_SPEC, UU == U);
                            LOOP2_ASSERT(U_SPEC, V_SPEC, VV == V);
                            LOOP2_ASSERT(U_SPEC,
                                         V_SPEC,
                                         (ui == vi) == (U == V));

                            // Test stream-in operator here.
                            bdex_InStreamFunctions::streamIn(testInStream,
                                                             mV,
                                                             version);

                            LOOP2_ASSERT(U_SPEC, V_SPEC, UU == U);
                            LOOP4_ASSERT(U_SPEC, V_SPEC, UU, V, UU == V);
                            LOOP2_ASSERT(U_SPEC, V_SPEC,  U == V);
                        } END_EXCEPTION_SAFE_TEST(VV == V);
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
                        LOOP_ASSERT(ti, X == t);

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         t,
                                                         version);
                        LOOP_ASSERT(ti, !testInStream);
                        LOOP_ASSERT(ti, X == t);

                        bdex_InStreamFunctions::streamIn(testInStream,
                                                         t,
                                                         version);
                        LOOP_ASSERT(ti, !testInStream);
                        LOOP_ASSERT(ti, X == t);

                    } END_EXCEPTION_SAFE_TEST(t == EMPTYCAL);
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
                                LOOP4_ASSERT(i, di, Xs[di], XX, Xs[di] == XX);
                            }
                            else {
                                LOOP2_ASSERT(i, di, in);
                                LOOP4_ASSERT(i, di, Xs[di], YYs[di],
                                             Xs[di] == YYs[di]);
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
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
                bdex_TestInStream in(OD, LOD); ASSERT(in);
                in.setSuppressVersionCheck(1);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(in);
                ASSERT(W != t);    ASSERT(X != t);      ASSERT(Y == t);
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
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, streamInVersion);
                ASSERT(!in);
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
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
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, streamInVersion);
                ASSERT(!in);
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
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
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
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
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
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
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
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
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
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
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
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
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            }
        }
        if (verbose)
            cout << "\t\tFirst weekend-days transition not at 1/1/1."
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

                int weekendDaysTransitionsLength = 1;
                out.putLength(weekendDaysTransitionsLength);

                // Writing offsets length and codes length into the stream.

                int OffsetsLength = 5;
                int codesLength   = 0;

                out.putLength(OffsetsLength);
                out.putLength(codesLength);


                // Write more weekend-days transitions in some random
                // (non-ascending) time order
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
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);

                bdex_TestInStream in(OD, LOD); in.setQuiet(!veryVerbose);
                in.setSuppressVersionCheck(1);
                ASSERT(in);
                bdex_InStreamFunctions::streamIn(in, t, version); ASSERT(!in);
                ASSERT(W != t);    ASSERT(X == t);      ASSERT(Y != t);
            }
        }
      }

DEFINE_TEST_CASE(9) {
        // --------------------------------------------------------------------
        // TESTING ASSIGNMENT OPERATOR:
        //
        // Concerns:
        //  1. The value represented by any instance can be assigned to any
        //     other instance.  And the assignment operator returns a reference
        //     to the destination object.
        //  2. The 'rhs' value must not be affected by the operation.
        //  3. 'rhs' going out of scope has no effect on the value of 'lhs'
        //     after the assignment.
        //  4. Aliasing (x = x): The assignment operator must always work --
        //     even when the 'lhs' and 'rhs' are identically the same object.
        //  5. The allocator value is not part of the assignment.
        //  6. The assignment operator must be exception safe with a guarantee
        //     of rollback with respect to memory allocation.
        //
        // Plan:
        //  To address concerns 1, 2, and 3, specify a set S of unique object
        //  values with substantial and varied differences.  Construct tests
        //  X = Y for all (X, Y) in S x S.  For each of these test, generate Y
        //  and YY using the same 'SPEC'.  After the assignment, assert that
        //  X, Y, and the value returned by the assignment operator all equal
        //  YY.  Let Y go out of scope and confirm that YY == X.
        //
        //  To address concern 4, we perform Y = Y and verify that both the
        //  return value of the assignment and Y itself equal YY.
        //
        //  To address concern 5, we create an object with the default
        //  allocator and assigned to it an object with a test allocator.  Then
        //  we verify that the memory of the new object is drawn from the
        //  default allocator.
        //
        //  To address concern 6, we use a test allocator and initially set the
        //  allocation limit to 0.  Then we perform assignments while gradually
        //  increasing the allocation limit.  This will trigger exceptions when
        //  the assignments need to obtain memory.  Then in the exception
        //  handler we verify that the values of the objects involved in the
        //  assignment are unchanged.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Array-Based Implementation Technique
        //
        // Testing:
        //      bdecs_PackedCalendar& operator=(
        //                                     const bdecs_PackedCalendar& rhs)
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing Assignment Operator" << endl
                          << "===========================" << endl;

        static const char *SPECS[] = {
            "",       "a",    "@2000/1/1",        "@2000/1/1 a",
            "@2000/1/1 2",    "@2000/1/1 2au",    "au@2000/1/1 2 1",
            "au@2000/1/1 1A", "au@2000/1/1 1A 2", "@2000/1/1 1 2A",
            "@2000/1/1 100",  "au@2000/1/1 365 2 20A 30 40AB 50DE",
            "au@2000/1/1 0au",  "@2000/1/1 0au 30tw 100rf",
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
                    Obj mX(&testAllocator);  const Obj& X  = mX;
                    gg(&mX, SPECS[i]);
                    {
                        Obj mY(&testAllocator); const Obj& Y = mY;
                        Obj mZ(&testAllocator); const Obj& Z = mZ;
                        gg(&mY, SPECS[j]);
                        gg(&mZ, SPECS[i]);

                        LOOP_ASSERT(i, YY == (mY = Y));
                        LOOP_ASSERT(i, YY == Y);
                        LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                        LOOP2_ASSERT(i, j, YY == (mX = Y));
                        LOOP2_ASSERT(i, j, YY == (mZ = Y));
                        LOOP2_ASSERT(i, j, YY == Y);
                        LOOP2_ASSERT(i, j, YY == X);
                        LOOP2_ASSERT(i, j, YY == Z);
                    }
                    LOOP2_ASSERT(i, j, YY == X);
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
                            LOOP2_ASSERT(i, j, XX == X);
                            LOOP2_ASSERT(i, j, YY == Y);
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
                            LOOP2_ASSERT(i, j, YY == Y);
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
      }

DEFINE_TEST_CASE(8) {
        // --------------------------------------------------------------------
        // TESTING GENERATOR FUNCTION, g:
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
        //   bdecs_PackedCalendar g(const char *spec)
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing Generator Function 'g'" << endl
                          << "==============================" << endl;

        static const char *SPECS[] = {
            "",              "au",                   "@2005/1/1 0",
            "w@1900/3/4 30", "au@1/1/1 30 2 4A 30B", "fau@9999/12/31 0 0A",
            "au@2005/5/1 30 1A 3B 11C 12D 13E 29",
            "au@2008/1/1 365 15 40A 120B 280C 300D 365E",
            "au@2000/1/1 0au",  "@2000/1/1 0au 30tw 100rf",
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
      }

DEFINE_TEST_CASE(7) {
        // --------------------------------------------------------------------
        // TESTING COPY CONSTRUCTOR:
        //
        // Concerns:
        //  1. The new object's value is the same as that of the original
        //     object.
        //  2. The value of the original object is left unaffected.
        //  3. Subsequent changes in or destruction of the source object have
        //     no effect on the copy-constructed object.
        //  4. The object has its internal memory management system hooked up
        //     properly so that *all* internally allocated memory draws from a
        //     user-supplied allocator whenever one is specified, and otherwise
        //     the default one.
        //
        // Plan:
        //  To address concern 1 - 3, specify a set S of object values with
        //  substantial and varied differences.  For each value in S,
        //  initialize objects W and X, copy construct Y from X and use
        //  'operator==()' to verify that both X and Y subsequently have the
        //  same value as W.  Let X go out of scope and again verify that
        //  W == Y.
        //
        //  To address concern 4, we will install a test allocator as the
        //  default and also supply a separate test allocator explicitly.  We
        //  will measure the total usage of both allocators before and after
        //  calling the copy constructor to ensure that no memory is allocated
        //  from the default allocator.  Then we will call the copy constructor
        //  to create another object using the default allocator and verify
        //  that the amount of memory used is the same as that with the
        //  supplied test allocator.
        //
        //  It is not necessary to perform exception-safety test for the copy
        //  constructor because it is using only the class members' copy
        //  constructors in the initialization list to create the object.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Loop-Based Implementation Technique
        //
        // Testing:
        //      bdecs_PackedCalendar::bdecs_PackedCalendar(
        //                         const bdecs_PackedCalendar&  original,
        //                         bslma_Allocator             *basicAllocator)
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing Copy Constructor" << endl
                          << "========================" << endl;

        static const char *SPECS[] = {
            "",       "a",    "@2000/1/1",        "@2000/1/1 a",
            "@2000/1/1 2",    "@2000/1/1 2au",    "au@2000/1/1 2 1",
            "au@2000/1/1 1A", "au@2000/1/1 1A 2", "@2000/1/1 1 2A",
            "@2000/1/1 100",  "au@2000/1/1 365 2 20A 30 40AB 50DE",
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
      }

DEFINE_TEST_CASE(6) {
        // --------------------------------------------------------------------
        // TESTING EQUALITY OPERATORS: '==' and '!='
        //
        // Concerns:
        //   1. 'operator==' returns true if two 'bdecs_packedCalendar' objects
        //      have the same value and false if they are only partially equal
        //      or not equal at all.  'operator!=' behaves exactly the opposite
        //      of 'operator=='.
        //   2. 'operator==' reports true when applied to any two objects whose
        //      internal representations may be different yet still represent
        //      the same (logical) value.
        //   3. 'operator==' and 'operator!=' do no trigger any memory
        //      allocation.
        //
        // Plan:
        //  To address concern 1, we will select a set S of unique object
        //  values having various differences.  These differences include:
        //      - Same except different weekend-days transitions;
        //      - Same except different holidays;
        //      - Same except different holiday codes;
        //      - same except different end day;
        //      - Same except different start day;
        //      - Any combinations of the above.
        //  We will verify the correctness of 'operator==' and 'operator!='
        //  (returning either true or false) using all elements (u, v) of the
        //  cross product S X S.
        //
        //  To address concern 2, we will implement the following two tests.
        //  First, we will create two calendar objects for the same spec: one
        //  with a specified allocator and the other one with the default
        //  allocator.  For each pair of specs, we will do a total of 4
        //  comparisons since there are two calendar objects for each spec.
        //  Through these comparisons We will verify that different allocator
        //  values does not affect the result of 'operator==' and 'operator!='.
        //  Second, we will call 'removeAll' for both u and v in (u, v) of the
        //  cross product S X S.  We will verify that u == v and both u and v
        //  have the same value as an empty calendar, even though their
        //  internal representation might be different.
        //
        //  To address concern 3, we will install a default allocator and
        //  verify after each round of comparisons that no allocation is
        //  triggered in either thr specified allocator or the default
        //  allocator.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Array-Based Implementation Technique
        //
        // Testing:
        //      bool operator==(const bdecs_PackedCalendar& lhs,
        //                      const bdecs_PackedCalendar& rhs)
        //      bool operator!=(const bdecs_PackedCalendar& lhs,
        //                      const bdecs_PackedCalendar& rhs)
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "Testing Equality Operators" << endl
                          << "==========================" << endl;

        static const char *SPECS[] = {
            "",       "a",     "@2000/1/1",        "@2000/1/1 a",
            "@2000/1/1 2",     "@2000/1/1 2au",    "au@2000/1/1 2 1",
            "au@2000/1/1 1A",  "au@2000/1/1 1A 2", "@2000/1/1 1 2A",
            "@2000/1/1 100",   "@1999/12/31 1",    "@0001/1/1 364",
            "@9999/01/01 364", "@9999/12/31",      "fau@2004/2/29 2",
            "fau@2004/2/28 3", "au@2000/1/1 365 2 20A 30 40AB 50DE",
            "au@2000/1/1 0au",  "@2000/1/1 0au 30tw 100rf",
        0}; // Null string required as last element.

        Obj emptyCal(&testAllocator); const Obj& EMPTYCAL = emptyCal;
        gg(&emptyCal, SPECS[0]);

        bslma_TestAllocator da; // default allocator
        const bslma_DefaultAllocatorGuard DAG(&da);

        for (int i = 0; SPECS[i]; ++i) {
            for (int j = 0; SPECS[j]; ++j) {
                Obj mX(&testAllocator); const Obj& X = mX;
                gg(&mX, SPECS[i]);

                // Perform a self comparison.

                LOOP_ASSERT(i,   X == X);
                LOOP_ASSERT(i, !(X != X));

                // Create 'XX' and 'YY' which have the same values as 'X' and
                // 'Y' respectively but with the default allocator.

                Obj mXX; const Obj& XX = mXX;
                gg(&mXX, SPECS[i]);

                Obj mY(&testAllocator); const Obj& Y = mY;
                gg(&mY, SPECS[j]);
                Obj mYY; const Obj& YY = mYY;
                gg(&mYY, SPECS[j]);

                // Compare the calendars agains each other.  Since for each of
                // the 'SPECS' we create two calendar objects: one with a
                // specified allocator and the other with the default
                // allocator, we need to do 4 comparisons: 'X' vs 'Y', 'XX' vs
                // 'Y', 'X' vs 'YY', and 'XX' vs 'YY'.

                int blocks        = testAllocator.numBlocksTotal();
                int blocksDefault = da.numBlocksTotal();

                LOOP2_ASSERT(i, j, (i == j) == (X == Y));
                LOOP2_ASSERT(i, j, (i == j) != (X != Y));

                LOOP2_ASSERT(i, j, (i == j) == (XX == Y));
                LOOP2_ASSERT(i, j, (i == j) != (XX != Y));

                LOOP2_ASSERT(i, j, (i == j) == (X == YY));
                LOOP2_ASSERT(i, j, (i == j) != (X != YY));

                LOOP2_ASSERT(i, j, (i == j) == (XX == YY));
                LOOP2_ASSERT(i, j, (i == j) != (XX != YY));

                // Verify that no allocation is triggered by these two methods.

                LOOP2_ASSERT(i, j, testAllocator.numBlocksTotal() == blocks);
                LOOP2_ASSERT(i, j, da.numBlocksTotal() == blocksDefault);

                // Compare the calendars with an empty calendar after all their
                // holidays and weekend days are removed to make sure the
                // internal vectors' sizes do not affect the result of
                // 'operator==' and 'operator!='.

                mX.removeAll();
                LOOP2_ASSERT(i, j,   X == EMPTYCAL);
                LOOP2_ASSERT(i, j, !(X != EMPTYCAL));

                mY.removeAll();
                LOOP2_ASSERT(i, j,   Y == EMPTYCAL);
                LOOP2_ASSERT(i, j, !(Y != EMPTYCAL));
                LOOP2_ASSERT(i, j,   Y == X);
                LOOP2_ASSERT(i, j, !(Y != X));
            }
        }
      }

DEFINE_TEST_CASE(5) {
        // --------------------------------------------------------------------
        // TESTING OUTPUT (<<) OPERATOR and 'print' method:
        //
        // Concerns:
        //  We want to ensure that the 'print' method correctly formats a
        //  'bdecs_packedCalendar' object for output with any valid 'level' and
        //  'spacesPerLevel' values and returns the specified stream.  We also
        //  want to verify that if an allocator is supplied, the default
        //  allocator is not used.
        //
        // Plan:
        //  We will set up a set of 'bdecs_packedCalendar' objects with
        //  different values.  We will use these objects combined with
        //  different values of 'level' and 'spacesPerLevel' to test the output
        //  of the 'print' method.  The test will start with fixed 'level' and
        //  'spacePerLevel' values and with simpler objects, then move on to
        //  larger and more complex ones.  Then we will continue the test by
        //  varying 'level' and 'spacePerLevel'.  Since 'operator<<' is
        //  implemented based upon the 'print' method, we will check its output
        //  against the 'print' method when the values of 'level' and
        //  'spacesPerLevel' match those used to implement 'operator<<'.  We
        //  will also verify that 'operator<<' returns the specified stream.
        //  To ensure that no memory is allocated from the default allocator
        //  during printing, we will install a test allocator as the default
        //  and also supply a separate test allocator explicitly.  We will
        //  measure the total usage of both allocators before and after
        //  printing.
        //
        // Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Table-Driven Implementation Technique
        //
        // Testing:
        //      bsl::ostream& bdecs_PackedCalendar::print(
        //                                  bsl::ostream& stream,
        //                                  int           level,
        //                                  int           spacesPerLevel) const
        //      bsl::ostream& operator<<(bsl::ostream&               stream,
        //                               const bdecs_PackedCalendar& calendar)
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        char R0[]="{ [ 31DEC9999, 01JAN0001 ] [ 01JAN0001 : [ ] ] }";
        char R1[]="{ [ 31DEC9999, 01JAN0001 ] [ 01JAN0001 : [ SUN SAT ] ] }";
        char R2[]="{ [ 01JAN0001, 01JAN0001 ] [ 01JAN0001 : [ ] ] }";
        char R3[]="{ [ 01JAN0001, 19MAY0001 ] [ 01JAN0001 : [ SUN SAT ] ] }";
        char R4[]="{ [ 01JAN0001, 19MAY0001 ] [ 01JAN0001 : [ ] ] "
                  "02JAN0001 03JAN0001 { 0 1 } "
                  "}";
        char R5[]="{ [ 01JAN0001, 13JAN0001 ] [ 01JAN0001 : [ ] ] "
                  "13JAN0001 { 0 1 2 100 1000 } }";
        char R6[]="{ [ 01JAN0001, 09FEB0001 ] [ 01JAN0001 : [ SUN SAT ] ] "
                  "02JAN0001 03JAN0001 { 0 } 04JAN0001 { 1 } "
                  "08JAN0001 { 0 1 2 100 } }";

        char R7[]="  { [ 01JAN0001, 13JAN0001 ] [ 01JAN0001 : [ ] ] "
                  "13JAN0001 { 0 1 2 100 1000 } }";
        char R8[]="      { [ 01JAN0001, 13JAN0001 ] [ 01JAN0001 : [ ] ] "
                  "13JAN0001 { 0 1 2 100 1000 } }";
        char R9[]="  {\n"
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
        char R10[]="{\n"
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
        char R11[]="{ [ 01JAN0001, 13JAN0001 ] "
                   "[ 01JAN0001 : [ SUN FRI SAT ] ] "
                   "13JAN0001 { 0 1 2 100 1000 } }";
        char R12[]="{ [ 01JAN0001, 11JAN0001 ] "
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

        if (verbose) cout << endl
                          << "Testing 'print' and 'operator<<'." << endl
                          << "=================================" << endl;

        for (int ti = 0; ti < NUM_DATA; ++ti) {
            const char *SPEC = DATA[ti].d_spec;
            const int LEVEL = DATA[ti].d_level;
            const int SPACES = DATA[ti].d_spacesPerLevel;
            const bsl::string RESULT = DATA[ti].d_result;

            bsl::ostringstream printStream;
            bsl::ostringstream operatorStream;
            {
                bslma_TestAllocator a;  // specified allocator
                LOOP_ASSERT( a.numBlocksTotal(), 0 ==  a.numBlocksTotal());

                bslma_TestAllocator da; // default allocator
                const bslma_DefaultAllocatorGuard DAG(&da);
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());

                Obj mX(&a); const Obj& X = mX;
                LOOP_ASSERT(da.numBlocksTotal(), 0 == da.numBlocksTotal());
                gg(&mX, SPEC);
                const int specifiedUsage  =  a.numBlocksTotal();
                const int defaultUsage    = da.numBlocksTotal();

                LOOP_ASSERT(ti, printStream == X.print(printStream, LEVEL,
                                                       SPACES));
                LOOP_ASSERT(ti, operatorStream == (operatorStream << X));

                LOOP_ASSERT( a.numBlocksTotal(), specifiedUsage ==
                                                           a.numBlocksTotal());
                LOOP_ASSERT(da.numBlocksTotal(), defaultUsage   ==
                                                          da.numBlocksTotal());
            }
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
      }

DEFINE_TEST_CASE(4) {
        // --------------------------------------------------------------------
        // TESTING BASIC ACCESSORS
        //
        // We want to first test the 'bdecs_PackedCalendar_IteratorDateProxy'
        // class to make sure it is working properly.  Second, we want to
        // verify that 'HolidayConstIterator', 'HolidayCodeConstIterator', and
        // 'WeekendDaysTransitionConstIterator', along with their corresponding
        // reverse iterators ('HolidayConstReverseIterator' and
        // 'HolidayCodeConstReverseIterator'), are working properly.  Third, we
        // also want to test 'beginHolidays', 'beginHolidayCodes',
        // 'beginWeekendDaysTransition', 'endHolidays', 'endHolidayCodes', and
        // 'endWeekendDaysTransition' and verify that these methods return the
        // expected iterators under all circumstances.  Finally, we want to
        // test the rest of the basic accessors, which include 'length',
        // 'isInRange', 'firstDate', 'lastDate', 'isHoliday', 'weekendDays',
        // 'numWeekendDaysTransitions', and
        // 'isWeekendDay(bdet_DayOfWeek::Day)'.
        //
        // Concerns:
        //  1. The class 'bdecs_PackedCalendar_IteratorDateProxy' properly
        //     proxies a 'bdet_Date' object.
        //
        //  For each iterator,
        //
        //  2. 'beginXXXX' returns the first element if it exists and 'endXXXX'
        //     returns one element past the last one.  If the container is
        //     empty, 'beginXXXX' must be equal to 'endXXXX'.  In addition, for
        //     'HolidayConstIterator', 'beginHolidays(const bdet_Date&)' and
        //     'endHolidays(const bdet_Date&)' return the first and one past
        //     the last element within the range specified by the date.
        //
        //  For 'HolidayConstIterator' and 'HolidayCodeConstIterator',
        //
        //  3. 'operator++' and 'operator--' move the iterator forward/backward
        //     by one element.  'operator=' assigns an iterator to another one
        //     of the same type and returns a reference to the iterator.
        //  4. 'operator*' and 'operator->' return the actual value associated
        //     with the iterator.
        //  5. 'operator==' and 'operator!=' compare the values of the two
        //     iterators and return the expected result.
        //  6. The reverse iterators are working properly.  Specifically for
        //     'HolidayConstIterator', 'rbeginHolidays(const bdet_Date&)' and
        //     'rendHolidays(const bdet_Date&)' are working properly.
        //
        //  For 'WeekendDaysTransitionConstIterator',
        //
        //  7. 'operator++' and 'operator--' move the iterator forward/backward
        //     by one element.  'operator=' assigns an iterator to another one
        //     of the same type and returns a reference to the iterator.
        //  8. 'operator*' and 'operator->' return the actual value associated
        //     with the iterator.
        //  9. 'operator==' and 'operator!=' compare the values of the two
        //     iterators and return the expected result.
        //
        //  For 'bdecs_PackedCalendar',
        //
        // 10. 'length', 'isInRange', 'firstDate', and 'lastDate' return the
        //     expected results for empty calendars, calendars that include
        //     regular years, and calendars that include leap years.
        // 11. 'isHoliday', and 'isWeekendDay(bdet_DayOfWeek::Day)' properly
        //     identify holidays and weekend days.
        // 12. 'weekendDays' properly returns the set of weekend days
        //     associated with this calendar.
        //
        // Plan:
        //
        //  To address concern 1, we will construct a
        //  'bdecs_PackedCalendar_IteratorDateProxy' object based on a
        //  pre-selected date in order to test the constructor
        //  'bdecs_PackedCalendar_IteratorDateProxy(const bdet_Date& date)'.
        //  Then we will create another date proxy object based on the first
        //  object in order to test the copy constructor.  We will then compare
        //  the pre-selected date with the 'bdet_Date' objects returned by
        //  'operator->' of these two objects and make sure they all match.  We
        //  will also use 'operator->' to call one of the methods of the
        //  'bdet_Date' class and verify that the return value matches the
        //  value obtained by calling this method through 'bdet_Date'.
        //
        //  To address concern 2, we will first call 'beginXXXX' and 'endXXXX'
        //  on an empty container and make sure they are equal.  Then we will
        //  add/remove elements to/from the container, in arbitrary order, and
        //  make sure that 'beginXXXX' always return the first element if it
        //  exists and 'endXXXX' always return the one past the last element.
        //  To test 'beginHolidays(const bdet_Date&)' and 'endHolidays(const
        //  bdet_Date&)' for 'HolidayConstIterator', we will set up a calendar
        //  with a fixed set of holidays.  We will then make sure the two
        //  methods above return the expected iterators for the following two
        //  cases: when a (1) holiday and (2) non-holiday from the calendar is
        //  supplied as the input parameter.
        //
        //  To address concern 3, we will call the iterator's 'operator++' and
        //  'operator--' method and make sure it moves forward/backward as
        //  expected.
        //
        //  To address concern 4, we will call 'operator*' and 'operator->' on
        //  an iterator pointing to a valid entry and verify that the return
        //  value matches the value of the entry.
        //
        //  To address concern 5, we will first compare an iterator to itself.
        //  Then we will compare it with another iterator that points to the
        //  same entry and make sure they are equal.  Finally we will compare
        //  the iterator with an iterator that points to a different entry and
        //  make sure they are not equal.
        //
        //  To address concern 6, we will compare the results returned by the
        //  reverse iterators with the results returned by the forward
        //  iterators moving backwards and make sure they are identical.  It is
        //  not necessary to apply all the tests for the forward iterators to
        //  these reverse iterators because they are implemented as the
        //  'bsl::reverse_iterator<>' version of the forward iterators.  To
        //  test 'rbeginHolidays(const bdet_Date&)' and
        //  'rendHolidays(const bdet_Date&)', we will set up a calendar with
        //  a fixed set of holidays.  We will then make sure the two methods
        //  above return the expected iterators for the following two cases:
        //  when a (1) holiday and (2) non-holiday from the calendar is
        //  supplied as the input parameter.
        //
        //  To address concerns 7 - 9, we perform a similar set of test done
        //  for concerns 4 - 6.
        //
        //  To address concern 10, we will arbitrarily select a set of date
        //  ranges and create calendar objects based on them.  Then we verify
        //  the return values of 'length', 'isInRange', 'firstDate', and
        //  'lastDate' against the expected values.
        //
        //  To address concern 11, we will first test 'isHoliday' by adding a
        //  few pre-selected dates plus all the dates in a pre-selected month
        //  into a calendar object.  We will verify that 'isHoliday' returns
        //  'false' before these days were added and 'true' afterwards.  Then
        //  we will test 'isWeekendDay' using the following method: let bit 0
        //  indicate whether Sunday is a weekend day, let bit 1 indicate
        //  whether Monday is a weekend day, ..., let bit 6 indicate whether
        //  Saturday is a weekend day.  Then all the possible combinations of
        //  weekend days can be represented by an integer between 0 and 127.
        //  For each of the combinations, we add the specified days as weekend
        //  days and verify 'isWeekendDay' returns the correct result for each
        //  weekend day and non-weekend day.
        //
        //  To address concern 12, we will add a few weekend days to a calendar
        //  and verify that 'weekendDays' returns the correct set of weekend
        //  days.  Then we will remove all weekend days from the calendar and
        //  verify that 'weekendDays' returns an empty weekend day set.
        //
        //  Tactics:
        //      - Ad Hoc test data selection method
        //      - Brute-Force/Loop-based/Table-Driven test case implementation
        //        techniques
        //
        //  Testing:
        //      HolidayConstIterator
        //      HolidayCodeConstIterator
        //      WeekendDaysTransitionConstIterator
        //      int length()
        //      bool isInRange(const bdet_Date&)
        //      const bdet_Date& firstDate()
        //      const bdet_Date& lastDate()
        //      bool isHoliday(const bdet_Date&)
        //      bool isWeekendDay(bdet_DayOfWeek::Day)
        //      int numWeekendDaysTransitions() const
        //  -------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

        if (verbose) cout << endl
                          << "TESTING BASIC ACCESSORS" << endl
                          << "=======================" << endl;

        {
            // Testing the 'bdecs_PackedCalendar_IteratorDateProxy' class

            if (verbose) cout << endl
                    << "Testing 'bdecs_PackedCalendar_IteratorDateProxy' class"
                    << endl
                    << "========================================"
                    << endl;

            // Do the same test on a the smallest and the largest dates
            {
                const bdet_Date date(1, 1, 1);

                bdecs_PackedCalendar_IteratorDateProxy proxy1(date);
                bdecs_PackedCalendar_IteratorDateProxy proxy2(proxy1);

                ASSERT(date == *(proxy1.operator->()));
                ASSERT(date == *(proxy2.operator->()));

                ASSERT(date.year() == proxy1->year());
                ASSERT(date.year() == proxy2->year());
            }
            {
                const bdet_Date date(9999,12,31);

                bdecs_PackedCalendar_IteratorDateProxy proxy1(date);
                bdecs_PackedCalendar_IteratorDateProxy proxy2(proxy1);

                ASSERT(date == *(proxy1.operator->()));
                ASSERT(date == *(proxy2.operator->()));

                ASSERT(date.year() == proxy1->year());
                ASSERT(date.year() == proxy2->year());
            }

        }

        {
            // TESTING 'HolidayIter'.

            if (verbose) cout << endl
                              << "Testing HolidayIter" << endl
                              << "===================" << endl;

            typedef Obj::HolidayConstIterator Iterator;

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
                // TESTING 'beginHolidays' and 'endHolidays'

                enum {
                    ACTION_ADD,     // add associated date
                    ACTION_DEL      // remove associated date
                };

                static const struct {
                    int d_lineNumber;
                    int d_action;
                    int d_date;
                    int d_begin;
                    int d_last;
                } HDATA[] = {
                    //LINE, ACTION,         DATE,     BEGIN,    LAST
                    { L_,   ACTION_ADD,     20000101, 20000101, 20000101},
                    { L_,   ACTION_ADD,      2171119,  2171119, 20000101},
                    { L_,   ACTION_DEL,     20000101,  2171119,  2171119},
                    { L_,   ACTION_DEL,      2171119,        0,        0},
                    { L_,   ACTION_ADD,     20000101, 20000101, 20000101},
                    { L_,   ACTION_ADD,      2171119,  2171119, 20000101},
                    { L_,   ACTION_ADD,     20000229,  2171119, 20000229},
                    { L_,   ACTION_DEL,      2171119, 20000101, 20000229},
                    { L_,   ACTION_ADD,     60320425, 20000101, 60320425},
                    { L_,   ACTION_ADD,     30180717, 20000101, 60320425},
                    { L_,   ACTION_DEL,     60320425, 20000101, 30180717},
                    { L_,   ACTION_ADD,     99991231, 20000101, 99991231},
                    { L_,   ACTION_DEL,     20000229, 20000101, 99991231},
                    { L_,   ACTION_ADD,        10101,    10101, 99991231},
                };
                const int NUM_HDATA = sizeof HDATA / sizeof *HDATA;

                Obj mX(&testAllocator);
                const Obj& X = mX;
                for (int nb = 0; nb < NUM_HDATA; ++nb) {
                    const int LINE = HDATA[nb].d_lineNumber;
                    const int ACTION = HDATA[nb].d_action;
                    bdet_Date mDATE;  const bdet_Date& DATE  = mDATE;
                    bdet_Date mBEGIN; const bdet_Date& BEGIN = mBEGIN;
                    bdet_Date mLAST;  const bdet_Date& LAST  = mLAST;

                    if (veryVerbose) P(LINE)

                    mDATE.setYearMonthDay(HDATA[nb].d_date / 10000,
                                          HDATA[nb].d_date % 10000 / 100,
                                          HDATA[nb].d_date % 100);
                    if (HDATA[nb].d_begin) {
                        mBEGIN.setYearMonthDay(HDATA[nb].d_begin / 10000,
                                               HDATA[nb].d_begin % 10000 / 100,
                                               HDATA[nb].d_begin % 100);
                        mLAST.setYearMonthDay (HDATA[nb].d_last / 10000,
                                               HDATA[nb].d_last % 10000 / 100,
                                               HDATA[nb].d_last % 100);
                    }

                    if (ACTION == ACTION_ADD) {
                        mX.addHoliday(DATE);
                    }
                    else {
                        mX.removeHoliday(DATE);
                    }
                    if (HDATA[nb].d_begin == 0) {
                        LOOP_ASSERT(nb, X.beginHolidays() == X.endHolidays());
                    }
                    else {
                        LOOP3_ASSERT(nb, *X.beginHolidays(), BEGIN,
                                         *X.beginHolidays() == BEGIN);
                        LOOP_ASSERT(nb, X.endHolidays() ==
                                                          X.endHolidays(LAST));
                        Iterator it = X.endHolidays();
                        --it;
                        LOOP3_ASSERT(nb, *it, LAST, *it == LAST);
                    }
                }
            }
            {
                // TESTING 'beginHolidays(const bdet_Date&)' and
                // 'endHolidays(const bdet_Date&)'

                // The order does matter.  Up to two holidays may be adjacent.

                const IteratorValue T1(   1, 1, 4);
                const IteratorValue T2( 217,11,19);
                const IteratorValue T3(2000, 1, 1);
                const IteratorValue T4(2000, 2,28);
                const IteratorValue T5(2000, 2,29);
                const IteratorValue T6(6032, 4,25);
                const IteratorValue T7(9999,12,29);
                const IteratorValue *DATA[] = {&T1, &T2, &T3, &T4, &T5, &T6,
                                               &T7};
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                Obj mX(&testAllocator);
                const Obj& X = mX;
                ASSERT(X.beginHolidays() == X.beginHolidays());
                ASSERT(X.endHolidays()   == X.endHolidays());
                ASSERT(X.beginHolidays() == X.endHolidays());

                for (int j = 0; j < NUM_DATA; ++j) {
                    mX.addHoliday(*DATA[j]);
                }

                mX.setValidRange(bdet_Date(1, 1, 3), bdet_Date(9999, 12, 30));

                // Test 'beginHolidays' using a date before the first holiday.

                ASSERT(*X.beginHolidays(T1-1) == T1);

                for (int nb = 0; nb < NUM_DATA; ++nb) {
                    bdet_Date date1 = *DATA[nb];

                    // Test 'beginHolidays' using a holiday.

                    LOOP3_ASSERT(nb, *X.beginHolidays(date1), date1,
                                     *X.beginHolidays(date1) == date1);

                    if (nb < NUM_DATA - 1) {
                        bdet_Date date2 = *DATA[nb+1];

                        // Test 'beginHolidays' using a non-holiday.

                        if (date1 + 1 != date2) {
                            LOOP3_ASSERT(nb, *X.beginHolidays(date1+1), date2,
                                         *X.beginHolidays(date1+1) == date2);
                        }
                        else if (nb < NUM_DATA - 2) {
                            bdet_Date date3 = *DATA[nb+2];
                            LOOP3_ASSERT(nb, *X.beginHolidays(date1+2), date3,
                                         *X.beginHolidays(date1+2) == date3);
                        }

                        // Test 'endHolidays' using a holiday.

                        LOOP3_ASSERT(nb, *X.endHolidays(date1), date2,
                                         *X.endHolidays(date1) == date2);

                        // Test 'endHolidays' using a non-holiday.

                        if (date1 + 1 != date2) {
                            LOOP3_ASSERT(nb, *X.endHolidays(date1+1), date2,
                                             *X.endHolidays(date1+1) == date2);
                        }
                        else if (nb < NUM_DATA - 2) {
                            bdet_Date date3 =  *DATA[nb+2];
                            LOOP3_ASSERT(nb, *X.endHolidays(date1+1), date3,
                                             *X.endHolidays(date1+1) == date3);
                        }
                    }
                }

                // Test 'endHolidays' using the last holiday.

                ASSERT(X.endHolidays(T7) == X.endHolidays());

                // Test 'endHolidays' using a date after the last holiday.

                ASSERT(X.endHolidays(T7+1) == X.endHolidays());
            }
            {
                // TESTING 'rbeginHolidays(const bdet_Date&)' and
                // 'rendHolidays(const bdet_Date&)'

                // The order does matter.  Up to two holidays may be adjacent.

                const IteratorValue T1(   1, 1, 4);
                const IteratorValue T2( 217,11,19);
                const IteratorValue T3(2000, 1, 1);
                const IteratorValue T4(2000, 2,28);
                const IteratorValue T5(2000, 2,29);
                const IteratorValue T6(6032, 4,25);
                const IteratorValue T7(9999,12,29);
                const IteratorValue *DATA[] = {&T1, &T2, &T3, &T4, &T5, &T6,
                                               &T7};
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                Obj mX(&testAllocator);
                const Obj& X = mX;
                ASSERT(X.rbeginHolidays() == X.rbeginHolidays());
                ASSERT(X.rendHolidays()   == X.rendHolidays());
                ASSERT(X.rbeginHolidays() == X.rendHolidays());

                for (int j = 0; j < NUM_DATA; ++j) {
                    mX.addHoliday(*DATA[j]);
                }

                mX.setValidRange(bdet_Date(1, 1, 3), bdet_Date(9999, 12, 30));

                // Test 'rbeginHolidays' using a date after the last holiday.

                ASSERT(*X.rbeginHolidays(T7+1) == T7);

                for (int nb = NUM_DATA - 1; nb >= 0; --nb) {
                    bdet_Date date1 = *DATA[nb];

                    // Test 'rbeginHolidays' using a holiday.

                    LOOP3_ASSERT(nb, *X.rbeginHolidays(date1), date1,
                                     *X.rbeginHolidays(date1) == date1);

                    if (nb >= 1) {
                        bdet_Date date2 = *DATA[nb-1];

                        // Test 'rbeginHolidays' using a non-holiday.

                        if (date1 - 1 != date2) {
                            LOOP3_ASSERT(nb, *X.rbeginHolidays(date1-1), date2,
                                          *X.rbeginHolidays(date1-1) == date2);
                        }
                        else if (nb >= 2) {
                            bdet_Date date3 = *DATA[nb-2];
                            LOOP3_ASSERT(nb, *X.rbeginHolidays(date1-2), date3,
                                          *X.rbeginHolidays(date1-2) == date3);
                        }

                        // Test 'rendHolidays' using a holiday.

                        LOOP3_ASSERT(nb, *X.rendHolidays(date1), date2,
                                         *X.rendHolidays(date1) == date2);

                        // Test 'rendHolidays' using a non-holiday.

                        if (date1 - 1 != date2) {
                            LOOP3_ASSERT(nb, *X.rendHolidays(date1-1), date2,
                                            *X.rendHolidays(date1-1) == date2);
                        }
                        else if (nb >= 2) {
                            bdet_Date date3 =  *DATA[nb-2];
                            LOOP3_ASSERT(nb, *X.rendHolidays(date1-1), date3,
                                            *X.rendHolidays(date1-1) == date3);
                        }
                    }
                }

                // Test 'rendHolidays' using the last holiday.

                ASSERT(X.rendHolidays(T1) == X.rendHolidays());

                // Test 'rendHolidays' using a date after the last holiday.

                ASSERT(X.rendHolidays(T1-1) == X.rendHolidays());
            }
            const IteratorValue T1(   1, 1, 1);
            const IteratorValue T2( 217,11,19);
            const IteratorValue T3(2000, 1, 1);
            const IteratorValue T4(2000, 2,29);
            const IteratorValue T5(3018, 7,17);
            const IteratorValue T6(6032, 4,25);
            const IteratorValue T7(9999,12,31);
            const IteratorValue *DATA[] = {&T1, &T2, &T3, &T4, &T5, &T6, &T7};
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // In the for-loop below 'nb' represents the number of holidays we
            // add to the calendar object.  During the first loop we do not add
            // any holiday to the calendar in order to test the case of a
            // calendar having no holidays.  In the subsequent loops we will
            // add between 1 and 'NUM_DATA' holiday(s) to the calendar.  Hence
            // 'nb' starts with a value of 0 and ends with a value of
            // 'NUM_DATA'.

            for (int nb = 0; nb <= NUM_DATA; ++nb) {
                Obj mX(&testAllocator);
                const Obj& X = mX;
                if (verbose) cout << "\nTesting with a size of " << nb << "."
                                  << endl;

                if (veryVerbose) cout << "\tgenerating object" << endl;

                for (int j = 0; j < nb; ++j) {
                    mX.addHoliday(*DATA[j]);

                    if (veryVeryVerbose) {
                        T_; P_(*DATA[j]);
                    }
                }
                if (veryVeryVerbose) cout << endl;

                // *** NON-MUTATING FORWARD ITERATOR TESTS ***

                if (veryVerbose) cout << "\tnon-mutating forward iterator "
                                      << "tests" << endl;

                // Breathing test.

                LOOP_ASSERT(nb, X.beginHolidays() == X.beginHolidays());
                LOOP_ASSERT(nb, X.endHolidays() == X.endHolidays());
                LOOP_ASSERT(nb, !nb || X.beginHolidays() != X.endHolidays());
                LOOP_ASSERT(nb,  nb || X.beginHolidays() == X.endHolidays());

                // Primary manipulators and basic accessors test pre-increment,
                // deference, copy constructor, 'operator==' test
                //
                // Note that 'operator*' is the basic accessor for all
                // values except 'X.endHolidays', in that case
                // 'operator==' is the basic accessor.

                {
                    Iterator i = X.beginHolidays();
                    for (int c = 0; c < nb; ++c) {
                        LOOP2_ASSERT(nb, c, !(i == X.endHolidays()));
                        LOOP4_ASSERT(nb, c, *DATA[c], *i, *DATA[c] == *i);

                        // copy construction

                        Iterator j(i);
                        LOOP2_ASSERT(nb, c, j != X.endHolidays());
                        LOOP4_ASSERT(nb, c, *DATA[c], *j, *DATA[c] == *j);

                        // 'operator++'

                        Iterator k(++i);
                        if (c < (nb - 1)) {
                            LOOP4_ASSERT(nb, c, *i, *k, *i == *k);
                            LOOP4_ASSERT(nb, c, *i, *j, *i != *j);
                        }
                        else {
                            LOOP2_ASSERT(nb, c, i == X.endHolidays());
                            LOOP2_ASSERT(nb, c, k == X.endHolidays());
                        }
                    }
                    LOOP_ASSERT(nb, i == X.endHolidays());
                }

                // 'operator==' and 'operator!='

                {
                    // Note that in the double loop below, each loop runs 'nb'
                    // + 1 times because we compare the 'nb' iterators
                    // referencing the 'nb' holidays plus the end iterator.

                    Iterator i = X.beginHolidays();
                    for (int c = 0; c <= nb; ++c) {
                        Iterator j = X.beginHolidays();
                        for (int d = 0; d <= nb; ++d) {

                            // The two tests are necessary because
                            // 'operator!=' is NOT implemented as "return
                            // !(lhs == rhs);".

                            if (c == d) {
                                LOOP3_ASSERT(nb, c, d, i == j);
                                LOOP3_ASSERT(nb, c, d, !(i != j));
                            }
                            else {
                                LOOP3_ASSERT(nb, c, d, i != j);
                                LOOP3_ASSERT(nb, c, d, !(i == j));
                            }
                            if (d < nb) {
                                ++j;
                            }
                        }
                        if (c < nb) {
                            ++i;
                        }
                    }
                }

                // assignment operator

                {
                    Iterator i = X.beginHolidays();
                    for (int c = 0; c < nb; ++c) {
                        Iterator j = X.beginHolidays();
                        for (int d = 0; d < nb; ++d) {
                            Iterator k(i);

                            i = i;
                            LOOP3_ASSERT(nb, c, d, k == i);

                            k = j;
                            LOOP3_ASSERT(nb, c, d, k == j);
                            ++j;
                        }
                        ++i;
                    }
                }

                // 'operator->'

                Iterator i = X.beginHolidays();
                for (; i != X.endHolidays(); ++i) {
                    LOOP_ASSERT(nb, (*i).month() == i->month());
                }

                // post-increment operator test

                i = X.beginHolidays();
                for (int c = 0; c < nb; ++c) {
                    LOOP2_ASSERT(nb, c, i != X.endHolidays());
                    LOOP4_ASSERT(nb, c, *DATA[c], *i, *DATA[c] == *(i++));
                }
                LOOP_ASSERT(nb, i == X.endHolidays());

                // *** NON-MUTATING BIDIRECTIONAL ITERATOR TESTS ***

                if (veryVerbose) cout << "\tnon-mutating bidirectional "
                                      << "iterator tests" << endl;

                // pre-decrement operator test

                i = X.endHolidays();
                for (int c = (nb - 1); c >= 0; --c) {
                    Iterator j = --i;
                    LOOP4_ASSERT(nb, c, *DATA[c], *i, *DATA[c] == *i);
                    LOOP2_ASSERT(nb, c, j == i);
                }

                // post-decrement operator test

                i = X.endHolidays();
                if (i != X.beginHolidays())
                    --i;
                for (int c = (nb - 1); c > 0; --c) {
                    LOOP4_ASSERT(nb, c, *DATA[c], *i, *DATA[c] == *(i--));
                }

                LOOP_ASSERT(nb, !nb || *i == *DATA[0]); // Test last value,
                                                        // if size > 0.

                // *** REVERSE ITERATOR TESTS ***
                // The following tests are very simple because
                // 'HolidayConstReverseIterator' is implemented using a TESTED
                // bsl::reserse_iterator<Iterator>-like template.

                if (veryVerbose) cout << "\treverse iterator tests" << endl;

                typedef Obj::HolidayConstReverseIterator ReverseIterator;

                if (nb == 0) {
                    LOOP_ASSERT(nb, X.rbeginHolidays() == X.rendHolidays());
                }
                else {
                    ReverseIterator ri = X.rbeginHolidays();

                    for (i = X.endHolidays(); ri != X.rendHolidays(); ++ri) {
                        --i;
                        LOOP_ASSERT(nb, *i == *ri);
                        LOOP_ASSERT(nb, i->year() == ri->year());
                    }
                    LOOP_ASSERT(nb, X.beginHolidays() == i);

                    for (ri = X.rendHolidays(), i = X.beginHolidays();
                                                   i != X.endHolidays(); ++i) {
                        --ri;
                        LOOP_ASSERT(nb, *i == *ri);
                        LOOP_ASSERT(nb, i->year() == ri->year());
                    }
                    LOOP_ASSERT(nb, X.rbeginHolidays() == ri);
                }
            }
        }
        {
            // TESTING 'HolidayCodeIter'

            typedef Obj::HolidayCodeConstIterator Iterator;

            typedef int IteratorValue;      // iterator's value_type typedef
            typedef int IteratorDifference; // iterator's difference_type
                                            // typedef
            typedef int *IteratorPointer;   // iterator's pointer typedef
            typedef int IteratorReference;  // iterator's reference typedef
            typedef bsl::bidirectional_iterator_tag IteratorCategory;
                // iterator's iterator_category typedef

            if (verbose) cout << endl
                              << "Testing HolidayCodeIter" << endl
                              << "=======================" << endl;

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

            // TESTING 'beginHolidayCodes', 'endHolidayCodes'.

            const int TEST_DATES[] = {10101, 20000101, 20000229, 30180717,
                                      99991231};
            const int NUM_DATES = sizeof (TEST_DATES) / sizeof(int);
            for (int nb = 0; nb < NUM_DATES; ++nb) {
                enum {
                    ACTION_ADD,     // add associated holiday code
                    ACTION_DEL      // remove associated holiday code
                };
                static const struct {
                    int d_lineNumber;
                    int d_action;
                    int d_hcode;
                    int d_begin;
                    int d_last;
                } HCDATA[] = {
                    //LINE, ACTION,         HCODE,   BEGIN,   LAST
                    { L_,   ACTION_ADD,     0      , 0,       0      },
                    { L_,   ACTION_ADD,     INT_MIN, INT_MIN, 0      },
                    { L_,   ACTION_DEL,     0,       INT_MIN, INT_MIN},
                    { L_,   ACTION_DEL,     INT_MIN, 0,       0      },
                    { L_,   ACTION_ADD,     0,       0,       0      },
                    { L_,   ACTION_ADD,     -6373,   -6373,   0      },
                    { L_,   ACTION_ADD,     23,      -6373,   23     },
                    { L_,   ACTION_DEL,     -6373,   0,       23     },
                    { L_,   ACTION_ADD,     INT_MAX, 0,       INT_MAX},
                    { L_,   ACTION_ADD,     2232,    0,       INT_MAX},
                    { L_,   ACTION_DEL,     INT_MAX, 0,       2232   },
                };
                const int NUM_HCDATA = sizeof HCDATA / sizeof *HCDATA;

                Obj mX(&testAllocator);
                const Obj& X = mX;
                bdet_Date D(TEST_DATES[nb] / 10000,
                            TEST_DATES[nb] % 10000 / 100,
                            TEST_DATES[nb] % 100);
                mX.addHoliday(D);
                Obj::HolidayConstIterator iter = X.beginHolidays();

                // Since any integer is a valid holiday code, we don't have a
                // value to indicate "no holiday code" for 'd_begin'.
                // Therefore we must use a variable to keep track of the number
                // of holiday codes.

                int numCodes = 0;

                // Add/delete holiday codes one at a time in arbitrary order
                // and check the return values of 'beginHolidayCodes' and
                // 'endHolidayCodes' after each addition/deletion.

                ASSERT(X.beginHolidayCodes(D) == X.beginHolidayCodes(iter));
                for (int nb = 0; nb < NUM_HCDATA; ++nb) {
                    const int LINE = HCDATA[nb].d_lineNumber;
                    const int ACTION = HCDATA[nb].d_action;
                    const int HCODE = HCDATA[nb].d_hcode;
                    const int BEGIN = HCDATA[nb].d_begin;
                    const int LAST = HCDATA[nb].d_last;

                    if (ACTION == ACTION_ADD) {
                        mX.addHolidayCode(D, HCODE);
                        ++numCodes;
                    }
                    else {
                        mX.removeHolidayCode(D, HCODE);
                        --numCodes;
                    }
                    if (numCodes == 0) {
                        LOOP_ASSERT(nb, X.beginHolidayCodes(D) ==
                                        X.endHolidayCodes(D));
                    }
                    else {
                        LOOP3_ASSERT(nb, *X.beginHolidayCodes(D), BEGIN,
                                         *X.beginHolidayCodes(D) == BEGIN);
                        LOOP3_ASSERT(nb, *X.beginHolidayCodes(iter), BEGIN,
                                         *X.beginHolidayCodes(iter) == BEGIN);
                        LOOP_ASSERT(nb, X.endHolidayCodes(D) ==
                                        X.endHolidayCodes(iter));
                        Iterator it = X.endHolidayCodes(D);
                        --it;
                        LOOP3_ASSERT(nb, *it, LAST, *it == LAST);
                    }
                }
            }

            // The order does matter.

            const IteratorValue T1(INT_MIN);
            const IteratorValue T2(-6373);
            const IteratorValue T3(0);
            const IteratorValue T4(23);
            const IteratorValue T5(2232);
            const IteratorValue T6(INT_MAX);
            const IteratorValue *DATA[] = {&T1, &T2, &T3, &T4, &T5, &T6};
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            // In the for-loop below 'nb' represents the number of holiday
            // codes we add to the holiday.  During the first loop we do not
            // want to add any holiday codes in order to test the case of a
            // holiday with no codes.  In the subsequent loops we will add
            // between 1 and 'NUM_DATA' code(s) to the holiday.  Hence 'nb'
            // starts with a value of 0 and ends with a value of 'NUM_DATA'.

            for (int nb = 0; nb <= NUM_DATA; ++nb) {
                Obj mX(&testAllocator);
                const Obj& X = mX;
                bdet_Date D(2000, 1, 1);

                if (verbose) cout << "\nTesting with a size of " << nb << "."
                                  << endl;

                if (veryVerbose) cout << "\tgenerating object" << endl;
                for (int j = 0; j < nb; ++j) {
                    mX.addHolidayCode(D, *DATA[j]);

                    if (veryVeryVerbose) {
                        T_; P_(*DATA[j]);
                    }
                }
                if (veryVeryVerbose) cout << endl;

                // *** NON-MUTATING FORWARD ITERATOR TESTS ***

                if (veryVerbose) cout << "\tnon-mutating forward iterator "
                                      << "tests" << endl;

                // Breathing test.

                if (!X.isInRange(D)) {
                    continue;
                }

                LOOP_ASSERT(nb, X.beginHolidayCodes(D) ==
                                                       X.beginHolidayCodes(D));
                LOOP_ASSERT(nb, X.endHolidayCodes(D) ==
                                                         X.endHolidayCodes(D));
                LOOP_ASSERT(nb, !nb || X.beginHolidayCodes(D) !=
                                                         X.endHolidayCodes(D));
                LOOP_ASSERT(nb,  nb || X.beginHolidayCodes(D) ==
                                                         X.endHolidayCodes(D));

                // Primary manipulators and accessors test pre-increment,
                // deference, copy constructor, 'operator==' test
                //
                // Note that 'operator*' is the primary accessor for all
                // values of except 'X.endHolidayCodes(D)', in that case
                // 'operator==' is the primary accessor.

                {
                    Iterator i = X.beginHolidayCodes(D);
                    for (int c = 0; c < nb; ++c) {
                        LOOP2_ASSERT(nb, c, !(i == X.endHolidayCodes(D)));
                        LOOP4_ASSERT(nb, c, *DATA[c], *i, *DATA[c] == *i);

                        // copy construction

                        Iterator j(i);
                        LOOP2_ASSERT(nb, c, j != X.endHolidayCodes(D));
                        LOOP4_ASSERT(nb, c, *DATA[c], *j, *DATA[c] == *j);

                        // 'operator++'

                        Iterator k(++i);
                        if (c < (nb - 1)) {
                            LOOP4_ASSERT(nb, c, *i, *k, *i == *k);
                            LOOP4_ASSERT(nb, c, *i, *j, *i != *j);
                        }
                        else {
                            LOOP2_ASSERT(nb, c, i == X.endHolidayCodes(D));
                            LOOP2_ASSERT(nb, c, k == X.endHolidayCodes(D));
                        }
                    }
                    LOOP_ASSERT(nb, i == X.endHolidayCodes(D));
                }

                // 'operator==' and 'operator!='

                {
                    // Note that in the double loop below, each loop runs 'nb'
                    // + 1 times because we compare the 'nb' iterators
                    // referencing the 'nb' holiday codes plus the end
                    // iterator.

                    Iterator i = X.beginHolidayCodes(D);
                    for (int c = 0; c <= nb; ++c) {
                        Iterator j = X.beginHolidayCodes(D);
                        for (int d = 0; d <= nb; ++d) {

                            // The two tests are necessary if 'operator!='
                            // is NOT implemented as "return !(lhs == rhs);".

                            if (c == d) {
                                LOOP3_ASSERT(nb, c, d, i == j);
                                LOOP3_ASSERT(nb, c, d, !(i != j));
                            }
                            else {
                                LOOP3_ASSERT(nb, c, d, i != j);
                                LOOP3_ASSERT(nb, c, d, !(i == j));
                            }
                            if (d < nb) {
                                ++j;
                            }
                        }
                        if (c < nb) {
                            ++i;
                        }
                    }
                }

                // assignment operator

                {
                    Iterator i = X.beginHolidayCodes(D);
                    for (int c = 0; c < nb; ++c) {
                        Iterator j = X.beginHolidayCodes(D);
                        for (int d = 0; d < nb; ++d) {
                            Iterator k(i);

                            i = i;
                            LOOP3_ASSERT(nb, c, d, k == i);

                            k = j;
                            LOOP3_ASSERT(nb, c, d, k == j);
                            ++j;
                        }
                        ++i;
                    }
                }

                // post-increment operator test

                Iterator i = X.beginHolidayCodes(D);
                for (int c = 0; c < nb; ++c) {
                    LOOP2_ASSERT(nb, c, i != X.endHolidayCodes(D));
                    LOOP4_ASSERT(nb, c, *DATA[c], *i, *DATA[c] == *(i++));
                }
                LOOP_ASSERT(nb, i == X.endHolidayCodes(D));

                // *** NON-MUTATING BIDIRECTIONAL ITERATOR TESTS ***

                if (veryVerbose) cout << "\tnon-mutating bidirectional "
                                      << "iterator tests" << endl;

                // pre-decrement operator test

                i = X.endHolidayCodes(D);
                for (int c = (nb - 1); c >= 0; --c) {
                    Iterator j = --i;
                    LOOP4_ASSERT(nb, c, *DATA[c], *i, *DATA[c] == *i);
                    LOOP2_ASSERT(nb, c, j == i);
                }

                // post-decrement operator test

                i = X.endHolidayCodes(D);
                if (i != X.beginHolidayCodes(D))
                    --i;
                for (int c = (nb - 1); c > 0; --c) {
                    LOOP4_ASSERT(nb, c, *DATA[c], *i, *DATA[c] == *(i--));
                }

                // Test last value, if size > 0.

                LOOP_ASSERT(nb, !nb || *i == *DATA[0]);

                // *** REVERSE ITERATOR TESTS ***
                // These tests are very simple because
                // HolidayConstReverseIterator is implemented using a TESTED
                // bsl::reverse_iterator<Iterator>-like template.

                if (veryVerbose) cout << "\treverse iterator tests" << endl;

                typedef Obj::HolidayCodeConstReverseIterator ReverseIterator;

                if (nb == 0) {
                    LOOP_ASSERT(nb, X.rbeginHolidayCodes(D) ==
                                                        X.rendHolidayCodes(D));
                }
                else {
                    ReverseIterator ri = X.rbeginHolidayCodes(D);

                    for (i = X.endHolidayCodes(D);
                                           ri != X.rendHolidayCodes(D); ++ri) {
                        --i;
                        LOOP_ASSERT(nb, *i == *ri);
                    }
                    LOOP_ASSERT(nb, X.beginHolidayCodes(D) == i);

                    for (ri = X.rendHolidayCodes(D),
                         i = X.beginHolidayCodes(D);
                                              i != X.endHolidayCodes(D); ++i) {
                        --ri;
                        LOOP_ASSERT(nb, *i == *ri);
                    }
                    LOOP_ASSERT(nb, X.rbeginHolidayCodes(D) == ri);
                }
            }
        }
        {
            {
                // Testing 'firstDate', 'lastDate', 'isInRange', 'length'

                if (verbose) {
                    cout << endl
                         << "Testing 'firstDate', 'lastDate', " << endl
                         << "        'isInRange', 'length'"     << endl
                         << "=================================" << endl;
                }
                static const struct {
                    int d_lineNum;     // source line number
                    int d_firstDate;   // first date
                    int d_lastDate;    // last date
                    int d_length;      // expected length
                } DATA[] = {
                    //LINE  FIRST DATE  LAST DATE LEN
                    //----  ----------  --------- ---
                    { L_,   99991231,      10101,   0 },
                    { L_,      10101,      10101,   1 },
                    { L_,      10101,      10102,   2 },
                    { L_,      10315,      10415,  32 },
                    { L_,      10101,      20101, 366 },
                    { L_,   20070201,   20070301,  29 },
                    { L_,   20070301,   20070401,  32 },
                    { L_,   20070409,   20070509,  31 },
                    { L_,   20070101,   20080101, 366 },
                    { L_,   20080207,   20080307,  30 },
                    { L_,   20080229,   20080329,  30 },
                    { L_,   20080101,   20090101, 367 },
                    { L_,   21000201,   21000301,  29 },
                    { L_,   21030101,   21050101, 732 },
                    { L_,   99990207,   99990307,  29 },
                    { L_,   99990320,   99990420,  32 },
                    { L_,   99990101,   99991231, 365 },
                };
                const int NUM_DATA = sizeof DATA / sizeof *DATA;

                const bdet_Date BEGINNING_OF_TIME(1,     1,  1);
                const bdet_Date END_OF_TIME(      9999, 12, 31);

                for (int ti = 0; ti < NUM_DATA; ++ti) {
                    const int       LINE   = DATA[ti].d_lineNum;
                    const bdet_Date START(DATA[ti].d_firstDate / 10000,
                                          DATA[ti].d_firstDate % 10000 / 100,
                                          DATA[ti].d_firstDate % 100);
                    const bdet_Date END(DATA[ti].d_lastDate / 10000,
                                        DATA[ti].d_lastDate % 10000 / 100,
                                        DATA[ti].d_lastDate % 100);
                    const int       LENGTH = DATA[ti].d_length;

                    bdecs_PackedCalendar obj(&testAllocator);

                    if (LENGTH > 0) {
                        obj.addDay(START);
                        obj.addDay(END);
                    }

                    LOOP_ASSERT(LINE, START == obj.firstDate());
                    LOOP_ASSERT(LINE, END == obj.lastDate());
                    LOOP_ASSERT(LINE, LENGTH == obj.length());

                    if (BEGINNING_OF_TIME != START) {
                        LOOP_ASSERT(LINE,
                                 false == obj.isInRange(bdet_Date(START) - 1));
                    }
                    LOOP_ASSERT(LINE, (LENGTH > 0) == obj.isInRange(START));
                    LOOP_ASSERT(LINE, (LENGTH > 0) == obj.isInRange(END));
                    if (END_OF_TIME != END) {
                        LOOP_ASSERT(LINE,
                                   false == obj.isInRange(bdet_Date(END) + 1));
                    }
                }
            }
            {
                // Testing 'isHoliday' and 'isWeekendDay'

                if (verbose) {
                    cout << endl
                         << "Testing 'isHoliday' and 'isWeekendDay'" << endl
                         << "======================================" << endl;
                }

                // Testing 'isHoliday'

                bdecs_PackedCalendar obj(bdet_Date(1, 1, 1),
                                         bdet_Date(9999, 12, 31),
                                         &testAllocator);

                ASSERT(false == obj.isHoliday(bdet_Date(2004, 8, 1)));
                for (int day = 1; day <= 31; ++day) {
                    obj.addHoliday(bdet_Date(2004, 8, day));
                    for (int j = 1; j <= 31; ++j) {
                        LOOP2_ASSERT(day, j,
                           (j <= day) == obj.isHoliday(bdet_Date(2004, 8, j)));
                    }
                }

                static const bdet_Date DATES[] = {
                    bdet_Date(   1,  1,  1), bdet_Date(1999,  1,  1),
                    bdet_Date(2000,  2, 29), bdet_Date(2003, 12, 31),
                    bdet_Date(3005,  7,  1), bdet_Date(9999, 12, 31)
                };
                const int NUM_DATES = sizeof(DATES)/ sizeof(DATES[0]);

                ASSERT(false == obj.isHoliday(DATES[0]));
                for (int i = 0; i < NUM_DATES; ++i) {
                    obj.addHoliday(DATES[i]);
                    for (int j = 0; j < NUM_DATES; ++j) {
                        LOOP2_ASSERT(i, j,
                                          (j <= i) == obj.isHoliday(DATES[j]));
                    }
                }

                // Testing isWeekendDay(bdet_DayOfWeek::Day)

                const int SUN  = 1;
                const int MON  = 1 << 1;
                const int TUE  = 1 << 2;
                const int WED  = 1 << 3;
                const int THU  = 1 << 4;
                const int FRI  = 1 << 5;
                const int SAT  = 1 << 6;
                for (int ti = 0; ti <= 0x7F; ++ti) {

                    bdecs_PackedCalendar obj(&testAllocator);

                    if (ti & SUN) {
                        obj.addWeekendDay(bdet_DayOfWeek::BDET_SUN);
                    }
                    if (ti & MON) {
                        obj.addWeekendDay(bdet_DayOfWeek::BDET_MON);
                    }
                    if (ti & TUE) {
                        obj.addWeekendDay(bdet_DayOfWeek::BDET_TUE);
                    }
                    if (ti & WED) {
                        obj.addWeekendDay(bdet_DayOfWeek::BDET_WED);
                    }
                    if (ti & THU) {
                        obj.addWeekendDay(bdet_DayOfWeek::BDET_THU);
                    }
                    if (ti & FRI) {
                        obj.addWeekendDay(bdet_DayOfWeek::BDET_FRI);
                    }
                    if (ti & SAT) {
                        obj.addWeekendDay(bdet_DayOfWeek::BDET_SAT);
                    }

                    LOOP_ASSERT(ti, ((ti & SUN) != 0)
                                == obj.isWeekendDay(bdet_DayOfWeek::BDET_SUN));
                    LOOP_ASSERT(ti, ((ti & MON) != 0)
                                == obj.isWeekendDay(bdet_DayOfWeek::BDET_MON));
                    LOOP_ASSERT(ti, ((ti & TUE) != 0)
                                == obj.isWeekendDay(bdet_DayOfWeek::BDET_TUE));
                    LOOP_ASSERT(ti, ((ti & WED) != 0)
                                == obj.isWeekendDay(bdet_DayOfWeek::BDET_WED));
                    LOOP_ASSERT(ti, ((ti & THU) != 0)
                                == obj.isWeekendDay(bdet_DayOfWeek::BDET_THU));
                    LOOP_ASSERT(ti, ((ti & FRI) != 0)
                                == obj.isWeekendDay(bdet_DayOfWeek::BDET_FRI));
                    LOOP_ASSERT(ti, ((ti & SAT) != 0)
                                == obj.isWeekendDay(bdet_DayOfWeek::BDET_SAT));
                }
            }
            {
                // Testing 'weekendDays'

                if (verbose) {
                    cout << endl
                         << "Testing 'weekendDays'" << endl
                         << "=====================" << endl;
                }

                Obj mX(&testAllocator); const Obj& X = mX;
                bdec_DayOfWeekSet set;

                ASSERT(X.numWeekendDaysTransitions() == 1);
                ASSERT(X.beginWeekendDaysTransitions()->second == set);

                set.add(bdet_DayOfWeek::BDET_SUN);
                mX.addWeekendDay(bdet_DayOfWeek::BDET_SUN);
                ASSERT(X.numWeekendDaysTransitions() == 1);
                ASSERT(X.beginWeekendDaysTransitions()->second == set);

                set.add(bdet_DayOfWeek::BDET_MON);
                mX.addWeekendDay(bdet_DayOfWeek::BDET_MON);
                ASSERT(X.numWeekendDaysTransitions() == 1);
                ASSERT(X.beginWeekendDaysTransitions()->second == set);

                set.add(bdet_DayOfWeek::BDET_WED);
                mX.addWeekendDay(bdet_DayOfWeek::BDET_WED);
                ASSERT(X.numWeekendDaysTransitions() == 1);
                ASSERT(X.beginWeekendDaysTransitions()->second == set);

                set.removeAll();
                mX.removeAll();
                ASSERT(X.numWeekendDaysTransitions() == 1);
                ASSERT(X.beginWeekendDaysTransitions()->second == set);
            }
        }

        if (verbose)
            cout << endl << "Test 'WeekendDaysTransitionConstIterator'"
                 << endl;

        // Test typedefs
        {

            typedef Obj::WeekendDaysTransitionConstIterator Iterator;
            typedef Obj::WeekendDaysTransition              IteratorValue;
            typedef int                                 IteratorDifference;
            typedef Obj::WeekendDaysTransition*         IteratorPointer;
            typedef Obj::WeekendDaysTransition&         IteratorReference;
            typedef bsl::bidirectional_iterator_tag IteratorCategory;

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

        }

        // Test 'WeekendDaysTransitionConstIterator' type, the
        // 'beginWeekendDaysTransition' method, the 'endWeekendDaysTransition'
        // method, and the 'numWeekendDaysTransitions' method
        {
            static const struct {
                int d_line;
                int d_year;
                int d_month;
                int d_day;
                const char *d_weekendDays;
            } DATA[] = {
                //LINE  YYYY  MM  DD   SMTWTFS
                //----  ----  --  --   -------
                // default transition at 1/1/1 (not manually added)
                { L_,      1,  1,  1, "0000000" },

                { L_,   2000,  1,  1, "0000000" },
                { L_,   2000,  1, 10, "0000001" },
                { L_,   2000,  2, 10, "0000010" },
                { L_,   2000,  3, 10, "0000100" },
                { L_,   2000, 11, 10, "0011000" },
                { L_,   2001,  2, 10, "0000110" },
                { L_,   2001,  3, 10, "0000011" },
                { L_,   2001,  4, 10, "1111111" },
            };

            const int NUM_DATA = sizeof DATA/sizeof *DATA;

            bslma_TestAllocator oa("oa", veryVeryVerbose);

            for (int ti = 0; ti < NUM_DATA; ++ti) {

                Obj mX; const Obj& X = mX;

                typedef Obj::WeekendDaysTransitionConstIterator Iterator;
                typedef bsl::vector<Obj::WeekendDaysTransition>
                                                              TransitionVector;
                TransitionVector transitions;

                for (int tj = 0; tj <= ti; ++tj)
                {
                    bdet_Date date(DATA[tj].d_year,
                                   DATA[tj].d_month,
                                   DATA[tj].d_day);
                    bdec_DayOfWeekSet weekendDays;
                    int wdIndex = 1;
                    for(const char *wkStr = DATA[tj].d_weekendDays;
                        *wkStr; ++wkStr, ++wdIndex) {
                        if (*wkStr == '1') {
                            if (veryVeryVerbose) {
                                P(wdIndex)
                                    }
                            weekendDays.add(
                                static_cast<bdet_DayOfWeek::Day>(wdIndex));
                        }
                    }

                    if (veryVerbose) {
                        P_(date) P(weekendDays)
                    }
                    transitions.push_back(bsl::make_pair(date, weekendDays));
                    if (ti != 0) {
                        mX.addWeekendDaysTransition(date, weekendDays);
                    }
                }

                ASSERT(transitions.size() == X.numWeekendDaysTransitions());

                // operator++, operator*(), and operator->();
                {
                    Iterator iter = X.beginWeekendDaysTransitions();
                    for (int tt = 0; tt < transitions.size(); ++tt) {
                        ASSERT(*iter == transitions[tt]);
                        ASSERT(*(iter.operator->()) == transitions[tt]);
                        ++iter;
                    }

                    ASSERT(iter == X.endWeekendDaysTransitions());
                }

                // operator== and operator!=
                {
                    Iterator iter1 = X.beginWeekendDaysTransitions();

                    // loop from begin() up to and including end()
                    for (int ii = 0; ii <= transitions.size(); ++ii) {
                        Iterator iter2 = X.beginWeekendDaysTransitions();
                        for (int jj = 0; jj <= transitions.size(); ++jj) {

                            bool sameFlag = ii == jj;
                            LOOP3_ASSERT(ii, jj, transitions.size(),
                                         sameFlag == (iter1 == iter2));
                            LOOP3_ASSERT(ii, jj, transitions.size(),
                                         !sameFlag == (iter1 != iter2));

                            if (jj != transitions.size()) {
                                ++iter2;
                            }
                        }

                        if (ii != transitions.size()) {
                            ++iter1;
                        }
                    }
                }

                // copy constructor
                {
                    Iterator Z = X.beginWeekendDaysTransitions();
                    Iterator ZZ = X.beginWeekendDaysTransitions();

                    for (int ii = 0; ii <= transitions.size(); ++ii) {

                        const Iterator Y(Z);
                        ASSERT(Z == ZZ);
                        ASSERT(Y == ZZ);
                        ASSERT(Y == Z);

                        if (ii != transitions.size()) {
                            ++Z;
                            ++ZZ;
                        }
                    }
                }

                // copy-assignment operator
                {
                    Iterator Z = X.beginWeekendDaysTransitions();
                    Iterator ZZ = X.beginWeekendDaysTransitions();

                    Obj temp;
                    for (int ii = 0; ii <= transitions.size(); ++ii) {

                        Iterator mY = temp.endWeekendDaysTransitions();
                        const Iterator& Y = mY;

                        ASSERT(Y != ZZ);
                        Iterator *mR = &(mY = Z);
                        ASSERT(mR == &mY);
                        ASSERT(Z == ZZ);
                        ASSERT(Y == ZZ);
                        ASSERT(Y == Z);

                        if (ii != transitions.size()) {
                            ++Z;
                            ++ZZ;
                        }
                    }
                }

                // operator++()
                {
                    Iterator mZZ = X.beginWeekendDaysTransitions();
                    const Iterator& ZZ = mZZ;

                    for (int ii = 0; ii < transitions.size(); ++ii) {

                        Iterator Y(ZZ);
                        Iterator* mR = &(++Y);
                        ASSERT(mR == &Y);

                        if (ii == transitions.size() - 1) {
                            ASSERT(Y == X.endWeekendDaysTransitions());
                            break;
                        }

                        ASSERT(*Y == transitions[ii + 1]);
                        ++mZZ;
                    }
                }

                // operator++(int)
                {
                    Iterator mZZ = X.beginWeekendDaysTransitions();
                    const Iterator& ZZ = mZZ;

                    for (int ii = 0; ii < transitions.size(); ++ii) {

                        Iterator Y(ZZ);
                        Iterator mR = Y++;
                        ASSERT(mR == ZZ);

                        if (ii == transitions.size() - 1) {
                            ASSERT(Y == X.endWeekendDaysTransitions());
                            break;
                        }

                        ASSERT(*Y == transitions[ii + 1]);
                        ++mZZ;
                    }
                }

                // operator--()
                {
                    Iterator mZZ = X.beginWeekendDaysTransitions();
                    const Iterator& ZZ = mZZ;
                    ++mZZ;

                    for (int ii = 1; ii < transitions.size(); ++ii) {

                        Iterator Y(ZZ);
                        Iterator* mR = &(--Y);
                        ASSERT(mR == &Y);

                        if (ii == 1) {
                            ASSERT(Y == X.beginWeekendDaysTransitions());
                            break;
                        }

                        ASSERT(*Y == transitions[ii - 1]);
                        ++mZZ;
                    }
                }

                // operator--(int)
                {
                    Iterator mZZ = X.beginWeekendDaysTransitions();
                    const Iterator& ZZ = mZZ;
                    ++mZZ;

                    for (int ii = 1; ii < transitions.size(); ++ii) {

                        Iterator Y(ZZ);
                        Iterator mR = Y--;
                        ASSERT(mR == ZZ);

                        if (ii == 1) {
                            ASSERT(Y == X.beginWeekendDaysTransitions());
                            break;
                        }

                        ASSERT(*Y == transitions[ii - 1]);
                        ++mZZ;
                    }
                }

            }
        }
      }

DEFINE_TEST_CASE(3) {
        // --------------------------------------------------------------------
        // TESTING SIMPLE GENERATOR FUNCTION 'hh' AND PRIMITIVE GENERATOR
        // FUNCTIONS 'gg' and 'ggg':
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
        //      bdecs_PackedCalendar& hh(bdecs_PackedCalendar *, const char *)
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'hh', 'gg', and 'ggg'" << endl
                          << "=============================" << endl;

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
            if (veryVerbose) { T_ P(ti) P(INPUT) P(X) }

            LOOP3_ASSERT(LINE, RC, retCode, RC == retCode);
            LOOP3_ASSERT(LINE, LEN, X.length(), LEN == X.length());
            LOOP3_ASSERT(LINE, BDAY, X.numBusinessDays(),
                         BDAY == X.numBusinessDays());
            LOOP3_ASSERT(LINE, WDAY, numWeekendDaysInFirstTransition(X),
                         WDAY == numWeekendDaysInFirstTransition(X));
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
                ASSERT(sameWeekendDaysTransition(*iter, bdet_Date(1,1,1), ""));
                ASSERT(sameWeekendDaysTransition(*(++iter),
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

        {
            if (verbose) cout << endl
                              << "\tTesting SIMPLE GENERATOR 'hh'" << endl
                              << "\t=============================" << endl;

            static struct {
                int d_lineNumber;
                const char *d_input;  // input command
                const char *d_gg;     // command for 'gg' to generate a
                                      // calendar of the same value as that
                                      // generated by 'hh'
            } DATA[] = {
                //LINE, HH COMMAND,         GG COMMAND
                { L_,   "",                 ""                              },
                { L_,   "2",                "m"                             },
                { L_,   "17",               "au"                            },
                { L_,   "a",                "@2000/1/1"                     },
                { L_,   "ad",               "@2000/1/1 59"                  },
                { L_,   "ada",              "@2000/1/1 59 0"                },
                { L_,   "adbd",             "@2000/1/1 59 1 59"             },
                { L_,   "adbd4a",           "w@2000/1/1 59 0 1 59"          },
                { L_,   "adaA",             "@2000/1/1 59 0A"               },
                { L_,   "adaBC",            "@2000/1/1 59 0BC"              },
                { L_,   "aCdaB",            "@2000/1/1 59 0BC"              },
                { L_,   "adabCd",           "@2000/1/1 59 0 1C 59"          },
                { L_,   "21aebAcBCdA",      "um@2000/1/1 2921939 1A 2BC 59A"},
            };
            const int NUM_DATA = sizeof DATA / sizeof *DATA;

            for (int i = 0; i < NUM_DATA; ++i) {
                const int LINE = DATA[i].d_lineNumber;
                const char *hhCommand = DATA[i].d_input;
                const char *ggCommand = DATA[i].d_gg;
                Obj mX; const Obj& X = mX;
                Obj mY; const Obj& Y = mY;
                gg(&mX, ggCommand);
                hh(&mY, hhCommand);
                LOOP3_ASSERT(i, X, Y, X == Y);
            }
        }
      }


DEFINE_TEST_CASE(2) {
        // --------------------------------------------------------------------
        // TESTING PRIMARY MANIPULATORS
        //
        // We want to exercise the set of primary manipulators, which can put
        // the object in any state.
        //
        // Concerns:
        //  1. Default constructor
        //      a. creates an object with the expected value
        //      b. is exception-neutral
        //      c. properly wires the optionally-specified allocator
        //
        //  2. That 'addDay'
        //      a. properly extends the object range if needed
        //      b. properly handles duplicates
        //
        //  3. That 'addWeekendDay'
        //      a. properly sets the day as a weekend day to the transition at
        //         1/1/1
        //      b. properly handles duplicates
        //
        //  4. That 'addHoliday'
        //      a. increases the object range if needed
        //      b. properly sets the specified day as a holiday
        //      b. properly handles duplicates
        //
        //  5. That 'addHolidayCode'
        //      a. increases the object range if needed
        //      b. sets the specified date as a holiday if needed
        //      c. properly sets a code for the specified holiday
        //      d. properly handles multiple codes for one holiday
        //      e. properly handles duplicates
        //
        //  6. That 'removeAll'
        //      a. produces the expected value (empty)
        //      b. leaves the object in a consistent state
        //
        //  7. That 'addWeekendDaysTransition'
        //      a. properly adds a weekend-days transition
        //      b. properly handles duplicates
        //
        //  Note that there is no "stretching" in this object.  We are adopting
        //  a black-box attitude while testing this function with regard to the
        //  containers used by the object.
        //
        // Plan:
        //  To address concerns for 1, create an object using the default
        //  constructor
        //      - without passing an allocator, in which case the object will
        //        allocate memory using the default allocator.
        //      - with an allocator, in which case the object will allocate
        //        memory using the specified allocator.
        //      - in the presence of exceptions during memory allocations using
        //        a 'bslma_TestAllocator' and varying its allocation limit.
        //  Use 'length' to check the value.
        //
        //  To address concerns for 2, create an object, use 'addDay' to add
        //  several days including duplicates and check if the range has been
        //  properly extended with 'isInRange' and 'length'.
        //
        //  To address concerns for 3, create an object, exercise
        //  'addWeekendDay' to set weekend days and check if it is working as
        //  expected using 'WeekendDayConstIterator'.
        //
        //  To address concerns for 4, create an object, use 'addHoliday' to
        //  add several days including duplicates and check if the range has
        //  been properly extended with 'isInRange', if the holidays were
        //  correctly added using 'HolidayConstIterator'.
        //
        //  To address concerns for 5, create an object, use 'addHolidayCode'
        //  to add several days and codes including duplicates and check if the
        //  range has been properly extended with 'isInRange', if the holiday
        //  codes were correctly added using 'HolidayCodeConstIterator'.
        //
        //  To address concerns for 6, create an object, exercise 'addDay',
        //  'addHoliday', 'addHolidayCode' at will, then call 'removeAll',
        //  check the value and then exercise 'addDay', 'addHoliday' and
        //  'addHolidayCode' to check consistency.
        //
        //   To address concerns for 7, create an object, add a set of
        //   weekend-days transitions using the 'addWeekendDayTransition'
        //   method.  Verify using the 'numWeekendDaysTransitions',
        //   'beginWeekendDaysTransitions', 'endWeekendDaysTransitions' methods
        //   that the transitions have been added correctly.
        //
        //  Tactics:
        //      - Ad-Hoc Data Selection Method
        //      - Brute-Force Implementation technique
        //
        //  Testing:
        //      void addDay(const bdet_Date& date)
        //      void addWeekendDay(bdet_DayOfWeek::Day weekendDay)
        //      void addWeekendDaysTransition(date, weekendDays)
        //      void addHoliday(const bdet_Date& date)
        //      void addHolidayCode(const bdet_Date& date, int holidayCode)
        //      void removeAll()
        //      ~bdecs_PackedCalendar()
        // --------------------------------------------------------------------

        bslma_TestAllocator testAllocator(veryVeryVerbose);

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
        }

        if (verbose) cout << "\nTesting 'addWeekendDay'." << endl;
        {
            Obj mX;
            const Obj& X = mX;
            bdec_DayOfWeekSet expected;
            ASSERT(1 == X.numWeekendDaysTransitions());
            ASSERT(X.beginWeekendDaysTransitions()->second == expected);

            mX.addWeekendDay(bdet_DayOfWeek::BDET_SAT);
            expected.add(bdet_DayOfWeek::BDET_SAT);
            ASSERT(1 == X.numWeekendDaysTransitions());
            ASSERT(X.beginWeekendDaysTransitions()->second == expected);

            mX.addWeekendDay(bdet_DayOfWeek::BDET_MON);
            expected.add(bdet_DayOfWeek::BDET_MON);
            ASSERT(1 == X.numWeekendDaysTransitions());
            ASSERT(X.beginWeekendDaysTransitions()->second == expected);

            mX.addWeekendDay(bdet_DayOfWeek::BDET_SAT);
            ASSERT(1 == X.numWeekendDaysTransitions());
            ASSERT(X.beginWeekendDaysTransitions()->second == expected);
        }

        if (verbose) cout << "\nTesting 'addWeekendDaysTransition'." << endl;
        {
            Obj mX; const Obj& X = mX;
            bdet_Date date;
            bdec_DayOfWeekSet weekendDays;

            // default transition at 1/1/1
            Obj::WeekendDaysTransitionConstIterator iter =
                                               X.beginWeekendDaysTransitions();
            ASSERT(sameWeekendDaysTransition(*iter,
                                             bdet_Date(1,1,1), ""));
            ASSERT(++iter == X.endWeekendDaysTransitions());


            // new transition
            date.setYearMonthDay(2, 2, 2);
            weekendDays.removeAll();
            weekendDays.add(bdet_DayOfWeek::BDET_MON);
            mX.addWeekendDaysTransition(date, weekendDays);

            iter = X.beginWeekendDaysTransitions();
            ASSERT(sameWeekendDaysTransition(*iter,
                                             bdet_Date(1,1,1), ""));
            ASSERT(sameWeekendDaysTransition(*(++iter),
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
        }

        if (verbose) cout << "\nTesting 'addHoliday'." << endl;
        {
            Obj mX;
            const Obj& X = mX;
            ASSERT(0 == X.isInRange(bdet_Date(1999,1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(1999,7,15)));
            ASSERT(0 == X.isInRange(bdet_Date(2000,1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(2000,7,15)));
            ASSERT(0 == X.isInRange(bdet_Date(2001,1, 1)));

            mX.addHoliday(bdet_Date(2000,1,1));
            ASSERT(0 == X.isInRange(bdet_Date(1999,1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(1999,7,15)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(2000,7,15)));
            ASSERT(0 == X.isInRange(bdet_Date(2001,1, 1)));
            Obj::HolidayConstIterator i = X.beginHolidays();
            ASSERT(i != X.endHolidays());
            ASSERT(bdet_Date(2000,1,1) == *i);
            ASSERT(++i == X.endHolidays());

            mX.addHoliday(bdet_Date(2000,1,1));
            ASSERT(0 == X.isInRange(bdet_Date(1999,1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(1999,7,15)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(2000,7,15)));
            ASSERT(0 == X.isInRange(bdet_Date(2001,1, 1)));
            i = X.beginHolidays();
            ASSERT(i != X.endHolidays());
            ASSERT(2000 == i->year());
            ASSERT(bdet_Date(2000,1,1) == *i);
            ASSERT(++i == X.endHolidays());

            mX.addHoliday(bdet_Date(2001,1,1));
            ASSERT(367 == X.length());
            ASSERT(0 == X.isInRange(bdet_Date(1999,1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(1999,7,15)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,7,15)));
            ASSERT(1 == X.isInRange(bdet_Date(2001,1, 1)));
            i = X.beginHolidays();
            ASSERT(bdet_Date(2000,1,1) == *i);
            ASSERT(bdet_Date(2001,1,1) == *(++i));
            ASSERT(++i == X.endHolidays());

            mX.addHoliday(bdet_Date(2000,7,15));
            ASSERT(367 == X.length());
            ASSERT(0 == X.isInRange(bdet_Date(1999,1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(1999,7,15)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,7,15)));
            ASSERT(1 == X.isInRange(bdet_Date(2001,1, 1)));
            i = X.beginHolidays();
            ASSERT(bdet_Date(2000,1, 1) == *i);
            ASSERT(bdet_Date(2000,7,15) == *(++i));
            ASSERT(bdet_Date(2001,1, 1) == *(++i));
            ASSERT(++i == X.endHolidays());

            mX.addHoliday(bdet_Date(2000,7,15));
            ASSERT(367 == X.length());
            ASSERT(0 == X.isInRange(bdet_Date(1999,1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(1999,7,15)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,7,15)));
            ASSERT(1 == X.isInRange(bdet_Date(2001,1, 1)));
            i = X.beginHolidays();
            ASSERT(bdet_Date(2000,1, 1) == *i);
            ASSERT(bdet_Date(2000,7,15) == *(++i));
            ASSERT(bdet_Date(2001,1, 1) == *(++i));
            ASSERT(++i == X.endHolidays());

            mX.addHoliday(bdet_Date(1999,1,1));
            ASSERT(732 == X.length());
            ASSERT(1 == X.isInRange(bdet_Date(1999,1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(1999,7,15)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000,7,15)));
            ASSERT(1 == X.isInRange(bdet_Date(2001,1, 1)));
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
            ASSERT(0 == X.isInRange(bdet_Date(2000,1, 1)));

            mX.addHoliday(bdet_Date(2000,1,1));
            mX.addHolidayCode(bdet_Date(2000,1,1), 1);
            Obj::HolidayCodeConstIterator k = X.beginHolidayCodes(
                                                 bdet_Date(2000,1,1));
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

            mX.addHolidayCode(bdet_Date(2000,2,1), 3);
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 15)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 2, 1)));
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
            mX.addHolidayCode(bdet_Date(1999,1,1), 1);
            mX.addHolidayCode(bdet_Date(1999,1,1), 2);
            mX.addHolidayCode(bdet_Date(1999,1,1), 2);
            mX.addHolidayCode(bdet_Date(1999,1,1), 3);
            mX.addHolidayCode(bdet_Date(1999,1,1), 3);
            ASSERT(1 == X.isInRange(bdet_Date(1999, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 15)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 2, 1)));
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
            mX.addHolidayCode(bdet_Date(1999,1,1), INT_MIN);
            ASSERT(1 == X.isInRange(bdet_Date(1999, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 15)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 2, 1)));
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
            ASSERT(0 == X.isInRange(bdet_Date(2000,1, 1)));

            mX.addHoliday(bdet_Date(2000,1,1));
            mX.addHolidayCode(bdet_Date(2000,1,1), 1);
            Obj::HolidayCodeConstIterator k = X.beginHolidayCodes(
                                                 bdet_Date(2000,1,1));
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

            mX.addHolidayCode(bdet_Date(2000,2,1), 3);
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 15)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 2, 1)));
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
            ASSERT(1 == X.isInRange(bdet_Date(1999, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 15)));
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

            mX.removeAll();
            ASSERT(0 == X.isInRange(bdet_Date(1999, 1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(2000, 1, 1)));
            ASSERT(0 == X.isInRange(bdet_Date(2000, 1, 15)));
            ASSERT(0 == X.isInRange(bdet_Date(2000, 2, 1)));
            ASSERT(X.endHolidays() == X.beginHolidays());

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

            mX.addHolidayCode(bdet_Date(2000,2,1), 3);
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 15)));
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

            mX.addHolidayCode(bdet_Date(1999,1,1), 3);
            ASSERT(1 == X.isInRange(bdet_Date(1999, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 1)));
            ASSERT(1 == X.isInRange(bdet_Date(2000, 1, 15)));
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
      }


DEFINE_TEST_CASE(1) {
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
        //      - equality operators: 'operator==()' and 'operator!=()'
        //      - the (test-driver supplied) output operator: 'operator<<()'
        //      - primary manipulators: 'push_back' and 'clear' methods
        //      - basic accessors: 'size' and 'operator[]()'
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
        // Tactics:
        //   - Ad Hoc Test Data Selection Method
        //   - Brute Force Implementation Technique
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

        ASSERT(FE == cal.firstDate());
        ASSERT(LE == cal.lastDate());
        ASSERT( 0 == cal.length());
        ASSERT( 0 == cal.numBusinessDays());
        ASSERT( 0 == cal.numNonBusinessDays());
        ASSERT( 0 == cal.numHolidays());
        ASSERT( 0 == cal.numWeekendDaysInRange());
        ASSERT( 0 == numWeekendDaysInFirstTransition(cal));

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
        LOOP_ASSERT(cal.numBusinessDays(), 0 == cal.numBusinessDays());
        ASSERT( 0 == cal.numNonBusinessDays());
        ASSERT( 0 == cal.numHolidays());
        ASSERT( 0 == cal.numWeekendDaysInRange());
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
        cal.addDay(bdet_Date(2000, 1, 1));
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

        if (verbose) cout << "\nAdd Holiday: Jan 4, 2000." << endl;
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

      }
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

    switch (test) { case 0:  // Zero is always the leading case.
#define CASE(NUMBER)                                                          \
  case NUMBER: testCase##NUMBER(verbose, veryVerbose, veryVeryVerbose); break
        /*
        */
        CASE(23);
        CASE(22);
        CASE(21);
        CASE(20);
        CASE(19);
        CASE(18);
        CASE(17);
        CASE(16);
        CASE(15);
        CASE(14);
        CASE(13);
        CASE(12);
        CASE(11);
        CASE(10);
        CASE(9);
        CASE(8);
        CASE(7);
        CASE(6);
        CASE(5);
        CASE(4);
        CASE(3);
        CASE(2);
        CASE(1);
#undef CASE

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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
