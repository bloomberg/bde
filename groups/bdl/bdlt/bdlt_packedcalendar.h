// bdlt_packedcalendar.h                                              -*-C++-*-
#ifndef INCLUDED_BDLT_PACKEDCALENDAR
#define INCLUDED_BDLT_PACKEDCALENDAR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a compact repository for weekend/holiday information.
//
//@CLASSES:
//  bdlt::PackedCalendar: compact repository of weekend/holiday information
//
//@SEE_ALSO: bdlt_calendar
//
//@DESCRIPTION: This component provides a value-semantic class,
// 'bdlt::PackedCalendar', that represents weekend and holiday information over
// a *valid* *range* of dates.  A 'bdlt::PackedCalendar' is an approximation to
// the same *mathematical* type, and is capable of representing the same subset
// of *mathematical* values, as a 'bdlt::Calendar'.
//
// But unlike 'bdlt::Calendar', which is optimized for runtime efficiency,
// 'bdlt::PackedCalendar' is designed to minimize the amount of in-process
// memory required to represent that information.  For example, a packed
// calendar storing 250 holidays and holiday codes can consume as little as
// approximately 0.75K bytes (e.g., 2 bytes per holiday plus 1 byte per
// holiday code) to as much as approximately 3K bytes (e.g., 8 bytes per
// holiday plus 4 bytes per holiday code) depending upon the data of the
// calendar.  For typical calendars having a range of 40 years and 10 holidays
// per year, the expected size of the packed calendar is about half that of a
// similar implementation using a non-packed structure.
//
// Default-constructed calendars are empty, and have an empty valid range.
// Calendars can also be constructed with an initial (non-empty) valid range,
// implying that all dates within that range are business days.  The
// 'setValidRange' and 'addDay' methods modify the valid range of a calendar,
// and a suite of "add" methods can be used to populate a calendar with
// weekend days and holidays.
//
// The 'addHolidayCode' method associates an integer "holiday code" with a
// specific date, and can be called repeatedly with different integers and the
// same date to build up a set of holiday codes for that date.  Note that
// holiday codes are unique integers that, from the perspective of the
// calendar, have no particular meaning.  Typically, the user will choose
// holiday code values that are indices into an auxiliary collection (such as a
// 'bsl::vector<bsl::string>') to identify holiday names for end-user display.
//
// Once a calendar is populated, a rich set of accessor methods can be used to
// determine, e.g., if a given date is a business day, or the number of
// non-business days within some subrange of the calendar.  The holidays
// within a calendar can be obtained in increasing (chronological) order using
// an iterator identified by the nested 'HolidayConstIterator' 'typedef'.  The
// set of holiday codes associated with an arbitrary date in a
// 'bdlt::PackedCalendar' (or the current holiday referred to by a
// 'HolidayConstIterator') can be obtained in increasing (numerical) order
// using an iterator identified by the nested 'HolidayCodeConstIterator'
// 'typedef' (see below).
//
// Calendars are value-semantic objects, and, as such, necessarily support all
// of the standard value-semantic operations, such as default construction,
// copy construction and copy assignment, equality comparison, and
// externalization (BDEX streaming, in particular).  Calendars also support
// the notions of both union and intersection merging operations, whereby a
// calendar can change its value to contain the union or intersection of its
// own contained weekend days, holidays, and holiday codes with those of
// another calendar object.  Such merging operations will, in general, also
// alter the valid range of the resulting calendar.  Note that merged
// calendars can be significantly more efficient for certain repeated
// "is-common-business-day" determinations among two or more calendars.
//
///Weekend Days and Weekend-Days Transitions
///-----------------------------------------
// A calendar maintains a set of dates considered to be weekend days.
// Typically, a calendar's weekend days fall on the same days of the week for
// the entire range of a calendar.  For example, the weekend for United States
// has consisted of Saturday and Sunday since the year 1940.  The
// 'addWeekendDay' and 'addWeekendDays' methods can be used to specify the
// weekend days for these calendars.
//
// However, sometimes a calendar's weekend days changes over time.  For
// example, Bangladesh's weekend consisted of Friday until June 1, 1997 when
// Bangladesh changed its weekends to contain both Friday and Saturday.  Later,
// on October 1, 2001, Bangladesh reverted to a weekend of only Friday, until
// on September 9, 2009, Bangladesh again changed its weekends to include both
// Friday and Saturday.
//
// To optimize for space allocation while supporting both consistent and
// changing weekend days, a calendar represents weekend information using a
// sequence of "weekend-days transitions", each of which comprises a date and a
// set of days of the week considered to be the weekend on and after that
// date.  To represent the weekend days of Bangladesh, a calendar can use a
// sequence of four weekend-days transitions: (1) a transition on January 1,
// 0001 having a weekend day set containing only Friday, (2) a transition at
// June 1, 1997 having a weekend day set containing Friday and Saturday, (3) a
// transition at October 1, 2001 having a weekend day set containing only
// Friday, and (4) a transition at September 9, 2009 having a weekend day set
// containing Friday and Saturday.  To represent the weekend days of the United
// States, a calendar having a range after 1940 can use a single weekend-days
// transition on January 1, 0001 containing Saturday and Sunday.
//
// On construction, a calendar does not contain any weekend-days transitions.
// The 'addWeekendDaysTransition' method adds a new weekend-days transition or
// replaces an existing weekend-days transition.  The 'addWeekendDay' and
// 'addWeekendDays' methods create a weekend-days transition at January 1,
// 0001, if one doesn't already exist, and update the set of weekend days for
// that transition.  'addWeekendDay' and 'addWeekendDays' should only be used
// for calendars having a consistent set of weekend days throughout their
// entire range.  The use of 'addWeekendDay' and 'addWeekendDays' is intended
// to be *mutually* *exclusive* to the use of 'addWeekendDaysTransition'.  As
// such, the behavior of using these two methods together with
// 'addWeekendDaysTransition' is undefined.
//
///Nested Iterators
///----------------
// Also provided are several STL-style 'const' bidirectional iterators
// accessible as nested 'typedef's.  'HolidayConstIterator',
// 'HolidayCodeConstIterator', 'WeekendDaysTransitionConstIterator', and
// 'BusinessDayConstIterator', respectively, iterate over a chronologically
// ordered sequence of holidays, a numerically ordered sequence of holiday
// codes, a sequence of chronologically ordered weekend-days transitions, and a
// sequence of chronologically ordered business days.  Reverse iterators are
// also provided for each of these (forward) iterators.  As a general rule,
// calling a 'const' method will not invalidate any iterators, and calling a
// non-'const' method might invalidate all of them; it is, however, guaranteed
// that attempting to add *duplicate* holidays or holiday codes will have no
// effect, and therefore will not invalidate any iterators.  It is also
// guaranteed that adding a new code for an existing holiday will not
// invalidate any 'HolidayConstIterator' objects.
//
// Note that these iterators do *not* meet the requirements for a
// 'bsl::forward_iterator' and should not be used in standard algorithms (e.g.,
// 'bsl::lower_bound').
//
///Iterator Invalidation
///---------------------
// The modification of a 'bdlt::PackedCalendar' will invalidate iterators
// referring to the calendar.  The following table shows the relationship
// between a calendar manipulator and the types of iterators it will
// invalidate if the invocation of the manipulator modified the calendar (e.g.,
// using 'addHoliday' with a date that is not currently a holiday in the
// calendar):
//..
//          Manipulator                         Invalidates
//    --------------------------            --------------------
//    'operator='                           H    HC    WDT    BD
//    'addHoliday'                          H    HC           BD
//    'addHolidayCode'                           HC
//    'addHolidayCodeIfInRange'                  HC
//    'addHolidayIfInRange'                 H    HC           BD
//    'addWeekendDay'                                  WDT    BD
//    'addWeekendDays'                                 WDT    BD
//    'addWeekendDaysTransition'                       WDT    BD
//    'intersectBusinessDays'               H    HC    WDT    BD
//    'intersectNonBusinessDays'            H    HC    WDT    BD
//    'removeAll'                           H    HC    WDT    BD
//    'removeHoliday'                       H    HC           BD
//    'removeHolidayCode'                        HC
//    'setValidRange'                       H    HC           BD
//    'unionBusinessDays'                   H    HC    WDT    BD
//    'unionNonBusinessDays'                H    HC    WDT    BD
//
// where "H" represents the holiday iterators ('HolidayConstIterator' and
// 'HolidayConstReverseIterator'), "HC" represents the holiday code iterators
// ('HolidayCodeConstIterator' and 'HolidayCodeConstReverseIterator'), "WDT"
// represents the weekend-days transition iterators
// ('WeekendDaysTransitionConstIterator' and
// 'WeekendDaysTransitionConstReverseIterator'), and "BD" represents the
// business day iterators ('BusinessDayConstIterator' and
// 'BusinessDayConstReverseIterator').
//..
//
///Performance and Exception-Safety Guarantees
///-------------------------------------------
// The asymptotic worst-case performance of representative operations is
// characterized using big-O notation, 'O[f(N,M,W,V)]'.  'N' and 'M' each refer
// to the combined number ('H + C') of holidays 'H' (i.e., method
// 'numHolidays') and holiday codes 'C' (i.e., 'numHolidayCodesTotal') in the
// respective packed calendars.  'W' and 'V' each refer to the (likely small)
// number of weekend-days transitions in the respective packed calendars.  For
// clarity, the methods have abbreviated arguments: 'b', 'e', and 'd' are
// dates, 'c' is a holiday code, 'u' is a weekday, and 'w' is a set of
// weekdays.  Here, *Best* *Case* complexity, denoted by 'B[f(N)]', is loosely
// defined (for manipulators) as the worst-case cost, provided that (1) no
// additional internal capacity is required, (2) the start of the valid range
// does not change, and (3) that if a holiday (or holiday code) is being added,
// it is being appended *to* *the* *end* of the current sequence (of the latest
// holiday).
//..
//                                    Worst       Best    Exception-Safety
//  Operation                          Case       Case      Guarantee
//  ---------                         -----       ----    ----------------
//  DEFAULT CTOR                      O[1]                No-Throw
//  COPY CTOR(N)                      O[N]                Exception-Safe
//  N.DTOR()                          O[1]                No-Throw
//
//  N.OP=(M)                          O[M]                Basic <*>
//
//  N.reserveCapacity(H, C)           O[N]                Strong <*>
//
//  N.setValidRange(b, e)             O[N]        O[1]    Basic <*>
//  N.addDay(d)                       O[N]        O[1]    Basic <*>
//  N.addHoliday(d)                   O[N]        O[1]    Basic <*>
//  N.addHolidayCode(d,c)             O[N]        O[1]    Basic <*>
//
//  N.addWeekendDay(u)                O[1]                No-Throw
//  N.addWeekendDaysTransition(d,w)   O[W]                Basic <*>
//
//  N.intersectBusinessDays(M)        O[N+M+W+V]          Basic <*>
//  N.intersectNonBusinessDays(M)     O[N+M+W+V]          Basic <*>
//  N.unionBusinessDays(M)            O[N+M+W+V]          Basic <*>
//  N.unionNonBusinessDays(M)         O[N+M+W+V]          Basic <*>
//
//  N.removeHoliday(d)                O[N]                No-Throw
//  N.removeHolidayCode(d, c)         O[N]                No-Throw
//  N.removeAll();                    O[1]                No-Throw
//
//  N.swap(M)                         O[1]                No-Throw
//
//  N.firstDate()                     O[1]                No-Throw
//  N.lastDate()                      O[1]                No-Throw
//  N.length()                        O[1]                No-Throw
//
//  N.numHolidays()                   O[1]                No-Throw
//
//  N.numHolidayCodesTotal()          O[1]                No-Throw
//  N.numWeekendDaysInRange()         O[1]                No-Throw
//
//  N.isInRange(d);                   O[1]                No-Throw
//  N.isWeekendDay(w);                O[1]                No-Throw
//  N.isWeekendDay(d)                 O[log(W)]           No-Throw
//
//  N.isHoliday(d);                   O[log(N)]           No_Throw
//  N.isBusinessDay(d);               O[log(N)]           No_Throw
//  N.isNonBusinessDay(d);            O[log(N)]           No_Throw
//
//  N.numHolidayCodes(d)              O[log(N)]           No-Throw
//
//  N.numBusinessDays()               O[N]                No-Throw
//  N.numNonBusinessDays()            O[N]                No-Throw
//
//  other 'const' methods             O[1] .. O[N]        No-Throw
//
//
//  OP==(N,M)                         O[min(N,M)+min(W+V) No-Throw
//  OP!=(N,M)                         O[min(N,M)+min(W+V) No-Throw
//
//                          <*> No-Throw guarantee when capacity is sufficient.
//..
// Note that *all* of the non-creator methods of 'bdlt::PackedCalendar' provide
// the *No-Throw* guarantee whenever sufficient capacity is already available.
// Also note that these are largely the same as 'bdlt::Calendar' *except* that
// the accessors 'isBusinessDay' and 'isNonBusinessDay' are logarithmic in the
// number of holidays in 'bdlt::PackedCalendar'.
//
///Usage
///-----
// The two subsections below illustrate various aspects of populating and using
// packed calendars.
//
///Example 1: Populating Packed Calendars
/// - - - - - - - - - - - - - - - - - - -
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
//  int getNextHoliday(bsl::istream& input, bdlt::Date *holiday, int *numCodes)
//      // Load into the specified 'holiday' the date of the next holiday, and
//      // into the specified 'numCodes' the associated number of holiday codes
//      // for the holiday read from the specified 'input' stream.  Return 0 on
//      // success, and a non-zero value (with no effect on '*holiday' and
//      // '*numCodes') otherwise.
//  {
//      enum { SUCCESS = 0, FAILURE = 1 };
//
//      int year, month, day, codes;
//
//      if (input.good()) {
//          input >> year;
//      }
//      if (input.good()) {
//          input >> month;
//      }
//      if (input.good()) {
//          input >> day;
//      }
//      if (input.good()) {
//          input >> codes;
//      }
//
//      if (input.good()
//       && bdlt::Date::isValidYearMonthDay(year, month, day)) {
//          *holiday  = bdlt::Date(year, month, day);
//          *numCodes = codes;
//          return SUCCESS;                                           // RETURN
//      }
//
//      return FAILURE;                                               // RETURN
//  }
//..
// Then, we'll write a function that gets us an integer holiday code, or
// invalidates the stream if it cannot (note that negative holiday codes are
// not supported by this function, but negative holiday codes *are* supported
// by 'bdlt::PackedCalendar'):
//..
//  void getNextHolidayCode(bsl::istream& input, int *result)
//      // Load, into the specified 'result', the value read from the specified
//      // 'input' stream.  If the next token is not an integer, invalidate the
//      // stream with no effect on 'result'.
//  {
//      int holidayCode;
//
//      if (input.good()) {
//          input >> holidayCode;
//      }
//
//      if (input.good()) {
//          *result = holidayCode;
//      }
//  }
//..
// Now, with these helper functions, it is a simple matter to write a calendar
// loader function, 'load', that populates a given calendar with data in this
// "proprietary" format:
//..
//  void load(bsl::istream& input, bdlt::PackedCalendar *calendar)
//      // Populate the specified 'calendar' with holidays and corresponding
//      // codes read from the specified 'input' stream in our "proprietary"
//      // format (see above).  On success, 'input' will be empty, but valid;
//      // otherwise 'input' will be invalid.
//  {
//      bdlt::Date holiday;
//      int        numCodes;
//
//      while (0 == getNextHoliday(input, &holiday, &numCodes)) {
//          calendar->addHoliday(holiday);                       // add date
//          for (int i = 0; i < numCodes; ++i) {
//              int holidayCode;
//              getNextHolidayCode(input, &holidayCode);
//              if (input.good()) {
//                  // add codes
//
//                  calendar->addHolidayCode(holiday, holidayCode);
//              }
//          }
//          input.ignore(256, '\n');  // skip comments
//      }
//  }
//..
// Finally, we load a 'bdlt::PackedCalendar' and verify some values from the
// calendar.
//..
//  bsl::stringstream stream;
//  {
//      stream << "2010  9   6     1   44         ;Labor Day\n"
//             << "2010 10  11     1   19         ;Columbus Day\n"
//             << "2010 11   2     0              ;Election Day\n"
//             << "2010 11  25     1   14         ;Thanksgiving Day\n";
//  }
//
//  bdlt::PackedCalendar calendar;
//  load(stream, &calendar);
//
//  assert(bdlt::Date(2010,  9,  6) == calendar.firstDate());
//  assert(bdlt::Date(2010, 11, 25) == calendar.lastDate());
//  assert(true  == calendar.isBusinessDay(bdlt::Date(2010, 10, 12)));
//  assert(false == calendar.isBusinessDay(bdlt::Date(2010, 11,  2)));
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
//  void
//  printHolidayNamesForGivenDate(bsl::ostream&                   output,
//                                const bdlt::PackedCalendar&     calendar,
//                                const bdlt::Date&               date,
//                                const bsl::vector<bsl::string>& holidayNames)
//      // Write, to the specified 'output' stream, the elements in the
//      // specified 'holidayNames' associated, via holiday codes in the
//      // specified 'calendar', to the specified 'date'.  Each holiday name
//      // emitted is followed by a newline ('\n').  The behavior is undefined
//      // unless 'date' is within the valid range of 'calendar'.
//  {
//      for (bdlt::PackedCalendar::HolidayCodeConstIterator
//                                       it = calendar.beginHolidayCodes(date);
//                                       it != calendar.endHolidayCodes(date);
//                                     ++it) {
//          output << holidayNames[*it] << bsl::endl;
//      }
//  }
//..
// Then, since we can write the names of holidays for a given date, let's
// write a function that can write out all of the names associated with each
// holiday in the calendar:
//..
//  void
//  printHolidayDatesAndNames(bsl::ostream&                   output,
//                            const bdlt::PackedCalendar&     calendar,
//                            const bsl::vector<bsl::string>& holidayNames)
//      // Write, to the specified 'output' stream, each date associated with
//      // a holiday in the specified 'calendar' followed by any elements in
//      // the specified 'holidayNames' (associated via holiday codes in
//      // 'calendar') corresponding to that date.  Each date emitted is
//      // preceded and followed by a newline ('\n').  Each holiday name
//      // emitted is followed by a newline ('\n').
//  {
//      for (bdlt::PackedCalendar::HolidayConstIterator
//                        it = calendar.beginHolidays();
//                                      it != calendar.endHolidays(); ++it) {
//          output << '\n' << *it << '\n';
//          printHolidayNamesForGivenDate(output,
//                                        calendar,
//                                        *it,
//                                        holidayNames);
//      }
//  }
//..
// Next, we populate the 'holidayNames' vector:
//..
//  bsl::vector<bsl::string> holidayNames;
//  {
//      holidayNames.resize(45);
//
//      holidayNames[44] = "Labor Day";         // holiday code 44 is for
//                                              // Labor Day
//
//      holidayNames[14] = "Thanksgiving Day";  // holiday code 14 is for
//                                              // Thanksgiving Day
//  }
//..
// Now, using the 'calendar' populated in the previous example, we print the
// holiday information to a new 'bsl::stringstream':
//..
//  bsl::stringstream printStream;
//
//  printHolidayDatesAndNames(printStream, calendar, holidayNames);
//..
// Finally, we verify the output:
//..
//  assert(printStream.str() == "\n06SEP2010\nLabor Day\n\n11OCT2010\n\n\n"
//                               "02NOV2010\n\n25NOV2010\nThanksgiving Day\n");
//..

#include <bdlscm_version.h>

#include <bdlt_calendarreverseiteratoradapter.h>
#include <bdlt_date.h>
#include <bdlt_dayofweek.h>
#include <bdlt_dayofweekset.h>

#include <bdlc_packedintarray.h>
#include <bdlc_packedintarrayutil.h>

#include <bslalg_swaputil.h>

#include <bslh_hash.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_integralconstant.h>

#include <bsls_assert.h>
#include <bsls_preconditions.h>
#include <bsls_review.h>

#include <bsl_cstddef.h>
#include <bsl_iosfwd.h>
#include <bsl_iterator.h>
#include <bsl_utility.h>      // 'bsl::pair'
#include <bsl_vector.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bslalg_typetraits.h>

#include <bsl_algorithm.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace bdlt {

class PackedCalendar_BusinessDayConstIterator;
class PackedCalendar_HolidayCodeConstIterator;
class PackedCalendar_HolidayConstIterator;

                          // ====================
                          // class PackedCalendar
                          // ====================

class PackedCalendar {
    // This class implements a space-efficient, value-semantic repository of
    // weekend and holiday information over a *valid* *range* of dates.  This
    // valid range, '[firstDate() .. lastDate()]', spans the first and last
    // dates of a calendar's accessible contents.  A calendar can be
    // "populated" with weekend and holiday information via a suite of "add"
    // methods.  Any subset of days of the week may be specified as weekend
    // (i.e., recurring non-business) days starting from a specified date by
    // adding a weekend-days transition; holidays within the valid range are
    // specified individually.  When adding a holiday, an arbitrary integer
    // "holiday code" may be associated with that date.  Additional holiday
    // codes for that date may subsequently be added.  Both the holidays and
    // the set of unique holiday codes associated with each holiday date are
    // maintained (internally) in order of increasing value.  Note that the
    // behavior of requesting *any* calendar information for a supplied date
    // whose value is outside the current *valid* *range* for that calendar
    // (unless otherwise noted, e.g., 'isWeekendDay') is undefined.

  private:
    // PRIVATE TYPES
    typedef bsl::pair<Date, DayOfWeekSet> WeekendDaysTransitionPrivate;

    typedef bdlc::PackedIntArray<int>::const_iterator OffsetsConstIterator;
    typedef bdlc::PackedIntArray<int>::const_iterator CodesIndexConstIterator;
    typedef bdlc::PackedIntArray<int>::const_iterator CodesConstIterator;

    typedef bsl::vector<WeekendDaysTransitionPrivate>
                                                 WeekendDaysTransitionSequence;

    struct WeekendDaysTransitionLess {
        // This 'struct' provides a comparator predicate for the type
        // 'WeekendDaysTransition' to enable the use of standard algorithms
        // (such as 'bsl::lower_bound') on ranges of objects of that type.

        // ACCESSORS
        bool operator()(const WeekendDaysTransitionPrivate& lhs,
                        const WeekendDaysTransitionPrivate& rhs) const
            // Return 'true' if the value of the specified 'lhs' is less than
            // (ordered before) the value of the specified 'rhs', and 'false'
            // otherwise.  The value of 'lhs' is less than the value of 'rhs'
            // if the date represented by the data member 'first' of 'lhs' is
            // earlier than the date represented by the data member 'first' of
            // 'rhs'.
        {
            return lhs.first < rhs.first;
        }
    };

  public:
    // TYPES
    typedef WeekendDaysTransitionPrivate WeekendDaysTransition;

    typedef PackedCalendar_BusinessDayConstIterator   BusinessDayConstIterator;

    typedef PackedCalendar_HolidayConstIterator       HolidayConstIterator;

    typedef PackedCalendar_HolidayCodeConstIterator   HolidayCodeConstIterator;

    typedef bdlt::CalendarReverseIteratorAdapter<BusinessDayConstIterator>
                                               BusinessDayConstReverseIterator;

    typedef bdlt::CalendarReverseIteratorAdapter<HolidayConstIterator>
                                                   HolidayConstReverseIterator;

    typedef bdlt::CalendarReverseIteratorAdapter<HolidayCodeConstIterator>
                                               HolidayCodeConstReverseIterator;

    typedef WeekendDaysTransitionSequence::const_iterator
                                            WeekendDaysTransitionConstIterator;

    typedef
    bsl::reverse_iterator<WeekendDaysTransitionSequence::const_iterator>
                                     WeekendDaysTransitionConstReverseIterator;

  private:
    // DATA
    Date                       d_firstDate;  // first valid date of calendar or
                                             // 9999/12/31 if this calendar is
                                             // empty

    Date                       d_lastDate;   // last valid date of calendar or
                                             // 0001/01/01 if this calendar is
                                             // empty

    bsl::vector<WeekendDaysTransition>
                               d_weekendDaysTransitions;
                                             // chronological list of weekend-
                                             // days transitions

    bdlc::PackedIntArray<int>  d_holidayOffsets;
                                             // ordered list of all holidays in
                                             // this calendar stored as offsets
                                             // from 'd_firstDate'

    bdlc::PackedIntArray<int>  d_holidayCodesIndex;
                                             // parallel to 'd_holidayOffsets',
                                             // this is a list of indices into
                                             // 'd_holidayCodes'; note that the
                                             // end of each sequence can be
                                             // determined using the value of
                                             // the next entry in this array if
                                             // it exists, or else the length
                                             // of 'd_holidayCodes' itself

    bdlc::PackedIntArray<int>  d_holidayCodes;
                                             // sequences of holiday codes,
                                             // each partitioned into an
                                             // ordered "chunk" of codes per
                                             // holiday in 'd_holidayOffsets';
                                             // chunks are stored in the same
                                             // order as in 'd_holidayOffsets'

    bslma::Allocator          *d_allocator_p;
                                             // memory allocator (held, not
                                             // owned)

    // FRIENDS
    friend class PackedCalendar_BusinessDayConstIterator;

    friend bool operator==(const PackedCalendar&, const PackedCalendar&);
    friend bool operator!=(const PackedCalendar&, const PackedCalendar&);
    template <class HASHALG>
    friend void hashAppend(HASHALG& hashAlg, const PackedCalendar&);

  private:
    // PRIVATE CLASS METHODS
    static void intersectHolidays(
                               bdlc::PackedIntArray<int> *resHolidayOffsets,
                               bdlc::PackedIntArray<int> *resHolidayCodesIndex,
                               bdlc::PackedIntArray<int> *resHolidayCodes,
                               const PackedCalendar&      lhs,
                               const PackedCalendar&      rhs,
                               const bdlt::Date           firstDate,
                               const bdlt::Date           lastDate);
        // Append, onto the specified 'resHolidayOffsets',
        // 'resHolidayCodesIndex', and 'resHolidayCodes', the intersection of
        // the holidays from the specified 'lhs' and 'rhs' calendars
        // restricted to the date range of the specified 'firstDate' and
        // 'lastDate'.  The behavior is undefined unless
        // 'firstDate <= lastDate'.

    static void unionHolidays(bdlc::PackedIntArray<int> *resHolidayOffsets,
                              bdlc::PackedIntArray<int> *resHolidayCodesIndex,
                              bdlc::PackedIntArray<int> *resHolidayCodes,
                              const PackedCalendar&      lhs,
                              const PackedCalendar&      rhs,
                              const bdlt::Date           firstDate,
                              const bdlt::Date           lastDate);
        // Append, onto the specified 'resHolidayOffsets',
        // 'resHolidayCodesIndex', and 'resHolidayCodes', the union of the
        // holidays from the specified 'lhs' and 'rhs' calendars restricted to
        // the date range of the specified 'firstDate' and 'lastDate'.  The
        // behavior is undefined unless 'firstDate <= lastDate'.

    // PRIVATE MANIPULATORS
    int addHolidayImp(int offset);
        // Add the specified 'offset' as a holiday offset in this calendar.  If
        // the date represented by 'offset' is already a holiday, this method
        // has no effect.  Return the index of 'd_holidayOffsets' where
        // 'offset' is stored.  The behavior is undefined unless '0 <= offset'
        // and the 'offset' represents a date within the valid range of the
        // calendar.

    // PRIVATE ACCESSORS
    CodesConstIterator beginHolidayCodes(
                                       const OffsetsConstIterator& iter) const;
        // Return an iterator that refers to the first non-modifiable holiday
        // code for the holiday referenced by the specified 'iter'.  If there
        // are no holiday codes associated with the date referenced by 'iter',
        // the returned iterator has the same value as that returned by
        // 'endHolidayCodes(iter)'.  The behavior is undefined unless 'iter'
        // refers to a valid holiday of this calendar.

    CodesConstIterator endHolidayCodes(const OffsetsConstIterator& iter) const;
        // Return an iterator that indicates the element one past the last
        // holiday code associated with the date referenced by the specified
        // 'iter'.  The behavior is undefined unless 'iter' references a valid
        // holiday in this calendar.

  public:
    // CLASS METHODS

                                  // Aspects

    static int maxSupportedBdexVersion(int versionSelector);
        // Return the maximum valid BDEX format version, as indicated by the
        // specified 'versionSelector', to be passed to the 'bdexStreamOut'
        // method.  Note that it is highly recommended that 'versionSelector'
        // be formatted as "YYYYMMDD", a date representation.  Also note that
        // 'versionSelector' should be a *compile*-time-chosen value that
        // selects a format version supported by both externalizer and
        // unexternalizer.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    // CREATORS
    explicit PackedCalendar(bslma::Allocator *basicAllocator = 0);
        // Create an empty calendar, i.e., a calendar having an empty valid
        // range.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    PackedCalendar(const Date&       firstDate,
                   const Date&       lastDate,
                   bslma::Allocator *basicAllocator = 0);
        // Create a calendar having a valid range from the specified
        // 'firstDate' through the specified 'lastDate'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'firstDate <= lastDate'.

    PackedCalendar(const PackedCalendar&  original,
                   bslma::Allocator      *basicAllocator = 0);
        // Create a calendar having the value of the specified 'original'
        // calendar.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~PackedCalendar();
        // Destroy this object.

    // MANIPULATORS
    PackedCalendar& operator=(const PackedCalendar& rhs);
        // Assign to this calendar the value of the specified 'rhs' calendar,
        // and return a reference providing modifiable access to this calendar.
        // See {Iterator Invalidation} for information regarding which
        // iterators are affected by the use of this method.

    void addDay(const Date& date);
        // Extend the valid range (if necessary) of this calendar to include
        // the specified 'date' value.

    void addHoliday(const Date& date);
        // Mark the specified 'date' as a holiday (i.e., a non-business day)
        // in this calendar.  Extend the valid range of this calendar if
        // necessary.  If 'date' is already marked as a holiday, this method
        // has no effect.  See {Iterator Invalidation} for information
        // regarding which iterators are affected by the use of this method.

    void addHolidayCode(const Date& date, int holidayCode);
        // Mark the specified 'date' as a holiday (i.e., a non-business day) in
        // this calendar and add the specified 'holidayCode' (if not already
        // present) to the ordered set of codes associated with 'date'.
        // Extend the valid range of this calendar if necessary.  If
        // 'holidayCode' is already a code for 'date', this method has no
        // effect.  See {Iterator Invalidation} for information regarding
        // which iterators are affected by the use of this method.

    int addHolidayCodeIfInRange(const Date& date, int holidayCode);
        // Mark the specified 'date' as a holiday (i.e., a non-business day)
        // in this calendar and add the specified 'holidayCode' (if not
        // already present) to the set of codes associated with 'date', if
        // 'date' is within the valid range of this calendar.  Return 0 if
        // 'date' is in range, and a non-zero value otherwise.  If
        // 'holidayCode' is already a code for 'date', or if 'date' is not in
        // the valid range, this method has no effect.  See
        // {Iterator Invalidation} for information regarding which iterators
        // are affected by the use of this method.  Note that this method may
        // be called repeatedly with the same value for 'date' to build up a
        // set of holiday codes for that date.

    int addHolidayIfInRange(const Date& date);
        // Mark the specified 'date' as a holiday (i.e., a non-business day) in
        // this calendar if 'date' is within the valid range.  Return 0 if
        // 'date' is in range, and a non-zero value otherwise.  This method has
        // no effect if 'date' is already marked as a holiday or is not in the
        // valid range.  See {Iterator Invalidation} for information regarding
        // which iterators are affected by the use of this method.

    void addWeekendDay(DayOfWeek::Enum weekendDay);
        // Add the specified 'weekendDay' to the set of weekend days associated
        // with the weekend-days transition at January 1, 0001 maintained by
        // this calendar.  Create a transition at January 1, 0001 if one does
        // not exist.  See {Iterator Invalidation} for information regarding
        // which iterators are affected by the use of this method.  The
        // behavior is undefined unless no weekend-days transitions were added
        // to this calendar via the 'addWeekendDaysTransition' method.

    void addWeekendDays(const DayOfWeekSet& weekendDays);
        // Add the specified 'weekendDays' to the set of weekend days
        // associated with the weekend-days transition at January 1, 0001
        // maintained by this calendar.  Create a transition at January 1, 0001
        // if one does not exist.  See {Iterator Invalidation} for information
        // regarding which iterators are affected by the use of this method.
        // The behavior is undefined unless no weekend-days transitions were
        // added to this calendar via the 'addWeekendDaysTransition' method.

    void addWeekendDaysTransition(const Date&         startDate,
                                  const DayOfWeekSet& weekendDays);
        // Add to this calendar a weekend-days transition on the specified
        // 'startDate' having the specified 'weekendDays' set.  If a
        // weekend-days transition already exists on 'startDate', replace the
        // set of weekend days of that transition with 'weekendDays'.  See
        // {Iterator Invalidation} for information regarding which iterators
        // are affected by the use of this method.  Note that this method does
        // not extend the valid range of the calendar.

    void intersectBusinessDays(const PackedCalendar& other);
        // Merge the specified 'other' calendar into this calendar such that
        // the valid range of this calendar becomes the *intersection* of the
        // two calendars' ranges, and the weekend days and holidays for this
        // calendar become the union of those (non-business) days from the two
        // calendars -- i.e., the valid business days of this calendar become
        // the intersection of those of the two original calendar values over
        // the *intersection* of their ranges.  For each holiday that remains,
        // the resulting holiday codes in this calendar will be the union of
        // the corresponding original holiday codes.  See
        // {Iterator Invalidation} for information regarding which iterators
        // are affected by the use of this method.

    void intersectNonBusinessDays(const PackedCalendar& other);
        // Merge the specified 'other' calendar into this calendar such that
        // the valid range of this calendar becomes the *intersection* of the
        // two calendars' ranges, the weekend days for this calendar become the
        // intersection of those days from the two calendars, and the holidays
        // for this calendar become the set of days that are a holiday in one
        // of the calendars and a non-business day in the other calendar --
        // i.e., the valid business days of this calendar become the union of
        // those of the two original calendars over the *intersection* of their
        // ranges.  For each holiday that remains, the resulting holiday codes
        // in this calendar will be the union of the corresponding original
        // holiday codes.  See {Iterator Invalidation} for information
        // regarding which iterators are affected by the use of this method.

    void removeAll();
        // Remove all information from this calendar, leaving it with its
        // default constructed "empty" value.  See {Iterator Invalidation} for
        // information regarding which iterators are affected by the use of
        // this method.

    void removeHoliday(const Date& date);
        // Remove from this calendar the holiday having the specified 'date' if
        // such a holiday exists.  This operation has no effect if 'date' is
        // not a holiday in this calendar.  See {Iterator Invalidation} for
        // information regarding which iterators are affected by the use of
        // this method.  Note that this method is defined for all 'date'
        // values, not just those that fall within the valid range, and may be
        // invoked even on an empty calendar (i.e., having '0 == length()').

    void removeHolidayCode(const Date& date, int holidayCode);
        // Remove from this calendar the specified 'holidayCode' for the
        // holiday having the specified 'date' if such a holiday having
        // 'holidayCode' exists.  This operation has no effect if 'date' is not
        // a holiday in this calendar, or if the holiday at 'date' does not
        // have 'holidayCode' associated with it.  See {Iterator Invalidation}
        // for information regarding which iterators are affected by the use of
        // this method.  Note that this method is defined for all 'date'
        // values, not just those that fall within the valid range, and may be
        // invoked even on an empty calendar (i.e., having '0 == length()').

    void reserveHolidayCapacity(int numHolidays);
        // Reserve enough space to store at least the specified 'numHolidays'
        // within this calendar.  This method has no effect if
        // 'numHolidays <= numHolidays()'.

    void reserveHolidayCodeCapacity(int numHolidayCodes);
        // Reserve enough space to store at least the specified
        // 'numHolidayCodes' within this calendar assuming no additional
        // holidays are added to this calendar.  This method has no effect if
        // 'numHolidayCodes <= numHolidayCodesTotal()'.

    void setValidRange(const Date& firstDate, const Date& lastDate);
        // Set the range of this calendar using the specified 'firstDate' and
        // 'lastDate' as, respectively, the first date and the last date of the
        // calendar.  Any holiday that is outside the new range and its holiday
        // codes are removed.  See {Iterator Invalidation} for information
        // regarding which iterators are affected by the use of this method.
        // The behavior is undefined unless 'firstDate <= lastDate'.

    void unionBusinessDays(const PackedCalendar& other);
        // Merge the specified 'other' calendar into this calendar such that
        // the valid range of this calendar becomes the *union* of the two
        // calendars' ranges (or the minimal continuous range spanning the two
        // ranges, if the ranges are non-overlapping), the weekend days for
        // this calendar become the intersection of those days from the two
        // calendars, and the holidays for this calendar become the set of days
        // that are a holiday in one of the calendars and a non-business day in
        // the other calendar -- i.e., the valid business days of this calendar
        // become the union of those of the two original calendar values.  For
        // each holiday that remains, the resulting holiday codes in this
        // calendar will be the union of the corresponding original holiday
        // codes.  See {Iterator Invalidation} for information regarding which
        // iterators are affected by the use of this method.

    void unionNonBusinessDays(const PackedCalendar& other);
        // Merge the specified 'other' calendar into this calendar such that
        // the valid range of this calendar becomes the *union* of the two
        // calendars' ranges (or the minimal continuous range spanning the two
        // ranges, if the ranges are non-overlapping), and the weekend days
        // and holidays for this calendar become the union of those
        // (non-business) days from the two calendars -- i.e., the valid
        // business days of this calendar become the intersection of those of
        // the two calendars after each range is extended to cover the
        // resulting one.  For each holiday in either calendar, the resulting
        // holiday codes in this calendar will be the union of the
        // corresponding original holiday codes.  See {Iterator Invalidation}
        // for information regarding which iterators are affected by the use of
        // this method.

                                  // Aspects

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation or if 'version' is not supported, this object is
        // unaltered.  Note that no version is read from 'stream'.  See the
        // 'bslx' package-level documentation for more information on BDEX
        // streaming of value-semantic types and containers.

    void swap(PackedCalendar& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    BusinessDayConstIterator beginBusinessDays() const;
        // Return an iterator providing non-modifiable access to the first
        // business day in this calendar.  If this calendar has no valid
        // business days, the returned iterator has the same value as that
        // returned by 'endBusinessDays()'.

    BusinessDayConstIterator beginBusinessDays(const Date& date) const;
        // Return an iterator providing non-modifiable access to the first
        // business day that occurs on or after the specified 'date' in this
        // calendar.  If this calendar has no such business day, the returned
        // iterator has the same value as that returned by
        // 'endBusinessDays(date)'.  The behavior is undefined unless 'date' is
        // within the valid range of this calendar.

    HolidayCodeConstIterator beginHolidayCodes(const Date& date) const;
        // Return an iterator providing non-modifiable access to the first
        // holiday code for the specified 'date' in this calendar.  If there is
        // no holiday code associated with 'date', the returned iterator has
        // the same value as that returned by 'endHolidayCodes(date)'.  The
        // behavior is undefined unless 'date' is marked as a holiday in this
        // calendar.

    HolidayCodeConstIterator beginHolidayCodes(
                                       const HolidayConstIterator& iter) const;
        // Return an iterator providing non-modifiable access to the first
        // holiday code for the holiday referenced by the specified 'iter'.  If
        // there is no holiday code associated with the date referenced by
        // 'iter', the returned iterator has the same value as that returned by
        // 'endHolidayCodes(iter)'.  The behavior is undefined unless 'iter'
        // refers to a valid holiday of this calendar.

    HolidayConstIterator beginHolidays() const;
        // Return an iterator providing non-modifiable access to the first
        // holiday in this calendar.  If this calendar has no holidays, the
        // returned iterator has the same value as that returned by
        // 'endHolidays()'.

    HolidayConstIterator beginHolidays(const Date& date) const;
        // Return an iterator providing non-modifiable access to the first
        // holiday that occurs on or after the specified 'date' in this
        // calendar.  If this calendar has no such holiday, the returned
        // iterator has the same value as that returned by 'endHolidays(date)'.
        // The behavior is undefined unless 'date' is within the valid range
        // of this calendar.

    WeekendDaysTransitionConstIterator beginWeekendDaysTransitions() const;
        // Return an iterator providing non-modifiable access to the first
        // weekend-days transition in the chronological sequence of
        // weekend-days transitions maintained by this calendar.  If this
        // calendar has no weekend-days transitions, the returned iterator has
        // the same value as that returned by 'endWeekendDaysTransitions()'.

    BusinessDayConstIterator endBusinessDays() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end business day in this calendar.

    BusinessDayConstIterator endBusinessDays(const Date& date) const;
        // Return an iterator providing non-modifiable access to the first
        // business day that occurs after the specified 'date' in this
        // calendar.  If this calendar has no such business day, the returned
        // iterator has the same value as that returned by
        // 'endBusinessDays()'.  The behavior is undefined unless 'date' is
        // within the valid range of this calendar.

    HolidayCodeConstIterator endHolidayCodes(const Date& date) const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end holiday code associated with the specified 'date'.  The
        // behavior is undefined unless 'date' is marked as a holiday in this
        // calendar.

    HolidayCodeConstIterator
                       endHolidayCodes(const HolidayConstIterator& iter) const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end holiday code associated with the date referenced by
        // the specified 'iter'.  The behavior is undefined unless 'iter'
        // references a valid holiday in this calendar.

    HolidayConstIterator endHolidays() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end holiday in the chronological sequence of holidays
        // maintained by this calendar.

    HolidayConstIterator endHolidays(const Date& date) const;
        // Return an iterator providing non-modifiable access to the first
        // holiday that occurs after the specified 'date' in this calendar.
        // If this calendar has no such holiday, the returned iterator has the
        // same value as that returned by 'endHolidays()'.  The behavior is
        // undefined unless 'date' is within the valid range of this calendar.

    WeekendDaysTransitionConstIterator endWeekendDaysTransitions() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end weekend-days transition in the chronological sequence
        // of weekend-days transitions maintained by this calendar.

    const Date& firstDate() const;
        // Return a reference providing non-modifiable access to the earliest
        // date in the valid range of this calendar.  The behavior is
        // undefined unless this calendar is non-empty -- i.e., unless
        // '1 <= length()'.

    int getNextBusinessDay(Date *nextBusinessDay, const Date& date) const;
        // Load, into the specified 'nextBusinessDay', the date of the first
        // business day in this calendar following the specified 'date'.
        // Return 0 on success -- i.e., if such a business day exists, and a
        // non-zero value (with no effect on 'nextBusinessDay') otherwise.  The
        // behavior is undefined unless 'date + 1' is both a valid 'bdlt::Date'
        // and within the valid range of this calendar.

    int getNextBusinessDay(Date        *nextBusinessDay,
                           const Date&  date,
                           int          nth) const;
        // Load, into the specified 'nextBusinessDay', the date of the
        // specified 'nth' business day in this calendar following the
        // specified 'date'.  Return 0 on success -- i.e., if such a business
        // day exists, and a non-zero value (with no effect on
        // 'nextBusinessDay') otherwise.  The behavior is undefined unless
        // 'date + 1' is both a valid 'bdlt::Date' and within the valid range
        // of this calendar, and '0 < nth'.

    bdlt::Date holiday(int index) const;
        // Return the holiday at the specified 'index' in this calendar.  For
        // all 'index' values from 0 to 'numHolidays() - 1' (inclusive), a
        // unique holiday is returned.  The mapping of 'index' to holiday is
        // invalidated when the set of holidays is modified by an invocation of
        // 'addHoliday', 'addHolidayIfInRange', 'intersectBusinessDays',
        // 'intersectNonBusinessDays', 'removeAll', 'removeHoliday',
        // 'setValidRange', 'unionBusinessDays', or 'unionNonBusinessDays'.
        // The behavior is undefined unless '0 <= index < numHolidays()'.

    int holidayCode(const Date& date, int index) const;
        // Return, for the holiday at the specified 'date' in this calendar,
        // the holiday code at the specified 'index'.  For all 'index' values
        // from 0 to 'numHolidayCodes(date) - 1' (inclusive), a unique holiday
        // code is returned.  The mapping of 'index' to holiday code is
        // invalidated when the set of holidays or holiday codes is modified by
        // an invocation of 'addHoliday', 'addHolidayCode',
        // 'addHolidayCodeIfInRange', 'addHolidayIfInRange',
        // 'intersectBusinessDays', 'intersectNonBusinessDays', 'removeAll',
        // 'removeHoliday', 'removeHolidayCode', 'setValidRange',
        // 'unionBusinessDays', or 'unionNonBusinessDays'.  The behavior is
        // undefined unless 'date' is a holiday in this calendar and
        // '0 <= index < numHolidayCodes(date)'.  Note that this method
        // facilitates testing and generally should not be used by clients; in
        // particular, using this method to iterate over the holiday codes for
        // 'date' is less efficient than using a 'HolidayCodeConstIterator'.

    bool isBusinessDay(const Date& date) const;
        // Return 'true' if the specified 'date' is a business day (i.e., not
        // a holiday or weekend day) in this calendar, and 'false' otherwise.
        // The behavior is undefined unless 'date' is within the valid range of
        // this calendar.

    bool isHoliday(const Date& date) const;
        // Return 'true' if the specified 'date' is a holiday in this calendar,
        // and 'false' otherwise.  The behavior is undefined unless 'date' is
        // within the valid range of this calendar.

    bool isInRange(const Date& date) const;
        // Return 'true' if the specified 'date' is within the valid range of
        // this calendar (i.e., 'firstDate() <= date <= lastDate()'), and
        // 'false' otherwise.  Note that the valid range for a
        // 'PackedCalendar' is empty if its length is 0.

    bool isNonBusinessDay(const Date& date) const;
        // Return 'true' if the specified 'date' is not a business day (i.e.,
        // is either a holiday or weekend day) in this calendar, and 'false'
        // otherwise.  The behavior is undefined unless 'date' is within the
        // valid range of this calendar.  Note that:
        //..
        //  !isBusinessDay(date)
        //..
        // returns the same result.

    bool isWeekendDay(const Date& date) const;
        // Return 'true' if the specified 'date' falls on a day of the week
        // that is considered a weekend day in this calendar, and 'false'
        // otherwise.  Note that this method is defined for all 'date' values,
        // not just those that fall within the valid range, and may be invoked
        // even on an empty calendar (i.e., having '0 == length()').

    bool isWeekendDay(DayOfWeek::Enum dayOfWeek) const;
        // Return 'true' if the specified 'dayOfWeek' is a weekend day in this
        // calendar, and 'false' otherwise.  The behavior is undefined unless
        // no weekend-days transitions were added to this calendar via the
        // 'addWeekendDaysTransition' method.

    const Date& lastDate() const;
        // Return a reference providing non-modifiable access to the latest
        // date in the valid range of this calendar.  The behavior is
        // undefined unless this calendar is non-empty -- i.e., unless
        // '1 <= length()'.

    int length() const;
        // Return the number of days in the valid range of this calendar,
        // which is defined to be 0 if this calendar is empty, and
        // 'lastDate() - firstDate() + 1' otherwise.

    int numBusinessDays() const;
        // Return the number of days in the valid range of this calendar that
        // are considered business days -- i.e., are neither holidays nor
        // weekend days.  Note that
        // 'numBusinessDays() == length() - numNonBusinessDays()'.

    int numBusinessDays(const Date& beginDate, const Date& endDate) const;
        // Return the number of days in the specified range
        // '[beginDate .. endDate]' of this calendar that are considered
        // business days -- i.e., are neither holidays nor weekend days.  The
        // behavior is undefined unless 'beginDate' and 'endDate' are within
        // the valid range of this calendar, and 'beginDate <= endDate'.

    int numHolidayCodes(const Date& date) const;
        // Return the number of (unique) holiday codes associated with the
        // specified 'date' in this calendar if 'date' is a holiday in this
        // calendar, and 0 otherwise.  The behavior is undefined unless 'date'
        // is within the valid range of this calendar.

    int numHolidayCodesTotal() const;
        // Return the total number of holiday codes for all holidays in this
        // calendar.  Note that this function is used primarily in conjunction
        // with 'reserveHolidayCodeCapacity'.

    int numHolidays() const;
        // Return the number of days in the valid range of this calendar that
        // are individually marked as holidays, irrespective of whether or not
        // the date is also considered a weekend day.

    int numNonBusinessDays() const;
        // Return the number of days in the valid range of this calendar that
        // are *not* considered business days -- i.e., are either holidays,
        // weekend days, or both.  Note that
        // 'numNonBusinessDays() == length() - numBusinessDays()'.

    int numWeekendDaysInRange() const;
        // Return the number of days in the valid range of this calendar that
        // are considered weekend days, irrespective of any designated
        // holidays.

    int numWeekendDaysTransitions() const;
        // Return the number of weekend-days transitions maintained by this
        // calendar.

    BusinessDayConstReverseIterator rbeginBusinessDays() const;
        // Return an iterator providing non-modifiable access to the last
        // business day in this calendar.  If this calendar has no valid
        // business days, the returned iterator has the same value as that
        // returned by 'rendBusinessDays()'.

    BusinessDayConstReverseIterator rbeginBusinessDays(const Date& date) const;
        // Return an iterator providing non-modifiable access to the first
        // business day that occurs on or before the specified 'date' in this
        // calendar.  If this calendar has no such business day, the returned
        // iterator has the same value as that returned by
        // 'rendBusinessDays(date)'.  The behavior is undefined unless 'date'
        // is within the valid range of this calendar.

    HolidayCodeConstReverseIterator rbeginHolidayCodes(const Date& date) const;
        // Return an iterator providing non-modifiable access to the last
        // holiday code associated with the specified 'date' in this calendar.
        // If there are no holiday codes associated with 'date', the returned
        // iterator has the same value as that returned by
        // 'rendHolidayCodes(date)'.  The behavior is undefined unless 'date'
        // is marked as a holiday in this calendar.

    HolidayCodeConstReverseIterator
                    rbeginHolidayCodes(const HolidayConstIterator& iter) const;
        // Return an iterator providing non-modifiable access to the last
        // holiday code associated with the holiday referenced by the specified
        // 'iter'.  If there are no holiday codes associated with the date
        // referenced by 'iter', the returned iterator has the same value as
        // that returned by 'rendHolidayCodes(iter)'.  The behavior is
        // undefined unless 'iter' refers to a valid holiday of this calendar.

    HolidayConstReverseIterator rbeginHolidays() const;
        // Return an iterator providing non-modifiable access to the last
        // holiday in this calendar.  If this calendar has no holidays, the
        // returned iterator has the same value as that returned by
        // 'rendHolidays()'.

    HolidayConstReverseIterator rbeginHolidays(const Date& date) const;
        // Return an iterator providing non-modifiable access to the first
        // holiday that occurs on or before the specified 'date' in this
        // calendar.  If this calendar has no such holiday, the returned
        // iterator has the same value as that returned by
        // 'rendHolidays(date)'.  The behavior is undefined unless 'date' is
        // within the valid range of this calendar.

    WeekendDaysTransitionConstReverseIterator
                                          rbeginWeekendDaysTransitions() const;
        // Return an iterator providing non-modifiable access to the last
        // weekend-days transition in the chronological sequence of
        // weekend-days transitions maintained by this calendar.  If this
        // calendar has no weekend-days transitions, the returned iterator has
        // the same value as that returned by 'rendWeekendDaysTransitions()'.

    BusinessDayConstReverseIterator rendBusinessDays() const;
        // Return an iterator providing non-modifiable access to the element
        // one before the first business day in this calendar.

    BusinessDayConstReverseIterator rendBusinessDays(const Date& date) const;
        // Return an iterator providing non-modifiable access to the first
        // business day that occurs before the specified 'date' in this
        // calendar.  If this calendar has no such business day, the returned
        // iterator has the same value as that returned by
        // 'rendBusinessDays()'.  The behavior is undefined unless 'date' is
        // within the valid range of this calendar.

    HolidayCodeConstReverseIterator rendHolidayCodes(const Date& date) const;
        // Return an iterator providing non-modifiable access to the element
        // one before the first holiday code associated with the specified
        // 'date'.  The behavior is undefined unless 'date' is marked as a
        // holiday in this calendar.

    HolidayCodeConstReverseIterator
                      rendHolidayCodes(const HolidayConstIterator& iter) const;
        // Return an iterator providing non-modifiable access to the element
        // one before the first holiday code associated with the holiday
        // referenced by the specified 'iter'.  The behavior is undefined
        // unless 'iter' references a valid holiday in this calendar.

    HolidayConstReverseIterator rendHolidays() const;
        // Return an iterator providing non-modifiable access to the element
        // one before the first holiday in this calendar.

    HolidayConstReverseIterator rendHolidays(const Date& date) const;
        // Return an iterator providing non-modifiable access to the first
        // holiday that occurs before the specified 'date' in this calendar.
        // If this calendar has no such holiday, the returned iterator has the
        // same value as that returned by 'rendHolidays()'.  The behavior is
        // undefined unless 'date' is within the valid range of this calendar.

    WeekendDaysTransitionConstReverseIterator
                                            rendWeekendDaysTransitions() const;
        // Return an iterator providing non-modifiable access to the element
        // one before the first weekend-days transition in the chronological
        // sequence of weekend-days transitions maintained by this calendar.

    WeekendDaysTransition weekendDaysTransition(int index) const;
        // Return the weekend-days transition at the specified 'index' in this
        // calendar.  For all 'index' values from 0 to
        // 'numWeekendDaysTransitions() - 1' (inclusive), a unique weekend-days
        // transition is returned.  The mapping of 'index' to weekend-days
        // transition is invalidated when the set of weekend-days transitions
        // is modified by an invocation of 'addWeekendDay', 'addWeekendDays',
        // 'addWeekendDaysTransition', 'intersectBusinessDays',
        // 'intersectNonBusinessDays', 'removeAll', 'unionBusinessDays', or
        // 'unionNonBusinessDays'.  The behavior is undefined unless
        // '0 <= index < numWeekendDaysTransitions()'.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.  See the 'bslx'
        // package-level documentation for more information on BDEX streaming
        // of value-semantic types and containers.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to the modifiable 'stream'.  If 'level' is specified,
        // optionally specify 'spacesPerLevel', the number of spaces per
        // indentation level for this and all of its nested objects.  If
        // 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22

    // DEPRECATED METHODS
    static int maxSupportedBdexVersion();
        // !DEPRECATED!: Use 'maxSupportedBdexVersion(int)' instead.
        //
        // Return the most current BDEX streaming version number supported by
        // this class.

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22
};

// FREE OPERATORS
bool operator==(const PackedCalendar& lhs, const PackedCalendar& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' calendars have the same
    // value, and 'false' otherwise.  Two calendars have the same value if they
    // have the same valid range (or are both empty), the same weekend days,
    // the same holidays, and each corresponding pair of holidays has the same
    // (ordered) set of associated holiday codes.

bool operator!=(const PackedCalendar& lhs, const PackedCalendar& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' calendars do not have the
    // same value, and 'false' otherwise.  Two calendars do not have the same
    // value if they do not have the same valid range (and are not both empty),
    // do not have the same weekend days, do not have the same holidays, or,
    // for at least one corresponding pair of holidays, do not have the same
    // (ordered) set of associated holiday codes.

bsl::ostream& operator<<(bsl::ostream&         stream,
                         const PackedCalendar& calendar);
    // Write the value of the specified 'calendar' to the specified output
    // 'stream', and return a reference to the modifiable 'stream'.

// FREE FUNCTIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const PackedCalendar& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'PackedCalendar'.

void swap(PackedCalendar& a, PackedCalendar& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee if the two objects were
    // created with the same allocator and the basic guarantee otherwise.

                       // ==============================
                       // class PackedCalendar_DateProxy
                       // ==============================

class PackedCalendar_DateProxy {
    // This class serves as a proxy for 'Date' for use by the arrow operator of
    // calendar iterators (e.g., 'PackedCalendar_HolidayConstIterator').  An
    // object of this class behaves as the 'Date' object with which it was
    // constructed.

    // DATA
    Date d_date;  // proxied date

  private:
    // NOT IMPLEMENTED
    PackedCalendar_DateProxy& operator=(const PackedCalendar_DateProxy&);

  public:
    // CREATORS
    PackedCalendar_DateProxy(const Date& date);                     // IMPLICIT
        // Create a proxy object for the specified 'date'.

    ~PackedCalendar_DateProxy();
        // Destroy this object.

    PackedCalendar_DateProxy(const PackedCalendar_DateProxy& original);
        // Create a proxy object referencing the same 'Date' value as the
        // specified 'original' proxy.

    // ACCESSORS
    const Date *operator->() const;
        // Return the address providing non-modifiable access to the proxied
        // date object.
};

                      // ============================
                      // class PackedCalendar_DateRef
                      // ============================

class PackedCalendar_DateRef : public Date {
    // This private class is used by the arrow operator of the holiday
    // iterator and business day iterator classes.  The objects instantiated
    // from this class serve as references to 'Date' objects.

  private:
    // NOT IMPLEMENTED
    PackedCalendar_DateRef& operator=(const PackedCalendar_DateRef&);

  public:
    // CREATORS
    explicit PackedCalendar_DateRef(const Date& date);
        // Create a date reference object using the specified 'date'.

    PackedCalendar_DateRef(const PackedCalendar_DateRef& original);
        // Create a date reference object having the value of the specified
        // 'original' object.

    ~PackedCalendar_DateRef();
        // Destroy this object.

    // ACCESSORS
    PackedCalendar_DateProxy operator&() const;
        // Return a proxy object to this object's referenced date.
};

                  // =========================================
                  // class PackedCalendar_HolidayConstIterator
                  // =========================================

class PackedCalendar_HolidayConstIterator {
    // Provide read-only, sequential access in increasing (chronological) order
    // to the holidays in a 'PackedCalendar' object.  This class owns an
    // iterator into the 'bdlc::PackedIntArray' storing the holiday offsets of
    // the referenced 'bdlt::PackedCalendar' and a date, 'd_firstDate', to
    // convert the offsets into dates corresponding to holidays.

    // DATA
    bdlc::PackedIntArray<int>::const_iterator d_iterator;   // array's iterator

    Date                                      d_firstDate;  // offset date

    // FRIENDS
    friend class PackedCalendar;
    friend bool operator==(const PackedCalendar_HolidayConstIterator&,
                           const PackedCalendar_HolidayConstIterator&);
    friend bool operator!=(const PackedCalendar_HolidayConstIterator&,
                           const PackedCalendar_HolidayConstIterator&);

  private:
    // PRIVATE TYPES
    typedef bdlc::PackedIntArray<int>::const_iterator OffsetsConstIterator;

    // PRIVATE CREATORS
    PackedCalendar_HolidayConstIterator(const OffsetsConstIterator& iter,
                                        const Date                  firstDate);
        // Create a holiday iterator using the specified 'iter' and
        // 'firstDate'.

  public:
    // TYPES
    typedef Date                     value_type;
    typedef int                      difference_type;
    typedef PackedCalendar_DateProxy pointer;
    typedef PackedCalendar_DateRef   reference;
        // The star operator returns a 'PackedCalendar_DateRef' *by* *value*.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE3.0

    typedef bsl::bidirectional_iterator_tag iterator_category;

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE3.0


    // CREATORS
    PackedCalendar_HolidayConstIterator(
                          const PackedCalendar_HolidayConstIterator& original);
        // Create an iterator having the value of the specified 'original'
        // iterator.

    ~PackedCalendar_HolidayConstIterator();
        // Destroy this object.

    // MANIPULATORS
    PackedCalendar_HolidayConstIterator& operator=(
                               const PackedCalendar_HolidayConstIterator& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator,
        // and return a reference providing modifiable access to this object.

    PackedCalendar_HolidayConstIterator& operator++();
        // Advance this iterator to refer to the next holiday in the calendar,
        // and return a reference providing modifiable access to this object.
        // The behavior is undefined unless, on entry, this iterator
        // references a valid holiday.

    PackedCalendar_HolidayConstIterator& operator--();
        // Regress this iterator to refer to the previous holiday in the
        // calendar, and return a reference providing modifiable access to this
        // object.  The behavior is undefined unless, on entry, this iterator
        // references a valid holiday that is not the first holiday in the
        // associated calendar.

    // ACCESSORS
    PackedCalendar_DateRef operator*() const;
        // Return a 'PackedCalendar_DateRef' object that contains the date
        // value of the holiday referenced by this iterator.

    PackedCalendar_DateProxy operator->() const;
        // Return a date proxy for the holiday referenced by this iterator.
};

// FREE OPERATORS
bool operator==(const PackedCalendar_HolidayConstIterator& lhs,
                const PackedCalendar_HolidayConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators refer to the
    // same element in the same calendar, and 'false' otherwise.  The behavior
    // is undefined unless 'lhs' and 'rhs' both iterate over the same calendar.

bool operator!=(const PackedCalendar_HolidayConstIterator& lhs,
                const PackedCalendar_HolidayConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not refer to
    // the same element in the same calendar, and 'false' otherwise.  The
    // behavior is undefined unless 'lhs' and 'rhs' both iterate over the same
    // calendar.

PackedCalendar_HolidayConstIterator
                operator++(PackedCalendar_HolidayConstIterator& iterator, int);
    // Advance the specified 'iterator' to refer to the next holiday in the
    // associated calendar, and return the previous value of 'iterator'.  The
    // behavior is undefined unless, on entry, 'iterator' references a valid
    // holiday.

PackedCalendar_HolidayConstIterator
                operator--(PackedCalendar_HolidayConstIterator& iterator, int);
    // Regress the specified 'iterator' to refer to the previous holiday in the
    // associated calendar, and return the previous value of 'iterator'.  The
    // behavior is undefined unless, on entry, 'iterator' references a valid
    // holiday that is not the first holiday in the associated calendar.

               // =============================================
               // class PackedCalendar_HolidayCodeConstIterator
               // =============================================

class PackedCalendar_HolidayCodeConstIterator {
    // Provide read-only, sequential access in increasing (numerical) order to
    // the holiday codes in a 'PackedCalendar' object.

    // DATA
    bdlc::PackedIntArray<int>::const_iterator d_iterator;  // array's iterator

    // FRIENDS
    friend class PackedCalendar;
    friend bool operator==(const PackedCalendar_HolidayCodeConstIterator&,
                           const PackedCalendar_HolidayCodeConstIterator&);
    friend bool operator!=(const PackedCalendar_HolidayCodeConstIterator&,
                           const PackedCalendar_HolidayCodeConstIterator&);
    friend bsl::ptrdiff_t operator-(
                               const PackedCalendar_HolidayCodeConstIterator&,
                               const PackedCalendar_HolidayCodeConstIterator&);

  private:
    // PRIVATE TYPES
    typedef bdlc::PackedIntArray<int>::const_iterator CodesConstIterator;

    // PRIVATE CREATORS
    PackedCalendar_HolidayCodeConstIterator(const CodesConstIterator& iter);
                                                                    // IMPLICIT
        // Create a holiday-code iterator referencing the holiday code
        // referred to by the specified 'iter'.

  public:
    // TYPES
    typedef int  value_type;
    typedef int  difference_type;
    typedef int *pointer;
    typedef int  reference;
        // The star operator returns an 'int' *by* *value*.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE3.0

    typedef bsl::bidirectional_iterator_tag iterator_category;

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE3.0

    // CREATORS
    PackedCalendar_HolidayCodeConstIterator(
                      const PackedCalendar_HolidayCodeConstIterator& original);
        // Create an object having the value of the specified 'original'
        // iterator.

    ~PackedCalendar_HolidayCodeConstIterator();
        // Destroy this object.

    // MANIPULATORS
    PackedCalendar_HolidayCodeConstIterator& operator=(
                           const PackedCalendar_HolidayCodeConstIterator& rhs);
        // Assign to this object the value of the specified 'rhs' iterator, and
        // return a reference providing modifiable access to this object.

    PackedCalendar_HolidayCodeConstIterator& operator++();
        // Advance this iterator to refer to the next holiday code for the
        // associated date in the associated calendar, and return a reference
        // providing modifiable access to this object.  The behavior is
        // undefined unless, on entry, this iterator references a valid
        // holiday code.

    PackedCalendar_HolidayCodeConstIterator& operator--();
        // Regress this iterator to refer to the previous holiday code for the
        // associated date in the associated calendar, and return a reference
        // providing modifiable access to this object.  The behavior is
        // undefined unless, on entry, this iterator references a valid
        // holiday code that is not the first holiday code for the associated
        // date in the calendar.

    // ACCESSORS
    int operator*() const;
        // Return the holiday code referenced by this iterator.
};

// FREE OPERATORS
bool operator==(const PackedCalendar_HolidayCodeConstIterator& lhs,
                const PackedCalendar_HolidayCodeConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators refers to the
    // same element, and 'false' otherwise.  The behavior is undefined unless
    // 'lhs' and 'rhs' both reference the same holiday in the same calendar.

bool operator!=(const PackedCalendar_HolidayCodeConstIterator& lhs,
                const PackedCalendar_HolidayCodeConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not refer to
    // the same element, and 'false' otherwise.  The behavior is undefined
    // unless 'lhs' and 'rhs' both reference the same holiday in the same
    // calendar.

PackedCalendar_HolidayCodeConstIterator
            operator++(PackedCalendar_HolidayCodeConstIterator& iterator, int);
    // Advance the specified 'iterator' to refer to the next holiday code for
    // the associated date in the associated calendar, and return the previous
    // value of 'iterator'.  The behavior is undefined unless, on entry,
    // 'iterator' references a valid holiday code.

PackedCalendar_HolidayCodeConstIterator
            operator--(PackedCalendar_HolidayCodeConstIterator& iterator, int);
    // Regress the specified 'iterator' to refer to the previous holiday code
    // for the associated date in the associated calendar, and return the
    // previous value of 'iterator'.  The behavior is undefined unless, on
    // entry, 'iterator' references a valid holiday code that is not the first
    // holiday code for the associated date in the calendar.

bsl::ptrdiff_t operator-(const PackedCalendar_HolidayCodeConstIterator& lhs,
                         const PackedCalendar_HolidayCodeConstIterator& rhs);
    // Return the number of elements between specified 'lhs' and 'rhs'.  The
    // behavior is undefined unless 'lhs' and 'rhs' refer to codes associated
    // with the same holiday in the same calendar.

              // =============================================
              // class PackedCalendar_BusinessDayConstIterator
              // =============================================

class PackedCalendar_BusinessDayConstIterator {
    // Provide read-only, sequential access in increasing (chronological) order
    // to the business days in a 'PackedCalendar' object.

    // DATA
    bdlc::PackedIntArray<int>::const_iterator  d_offsetIter;
                     // iterator for the holiday offsets

    const PackedCalendar                      *d_calendar_p;
                     // pointer to the calendar

    int                                        d_currentOffset;
                     // offset of the date referenced by this iterator from
                     // 'd_calendar_p->firstDate()'; if
                     // 'd_endFlag && 0 < d_calendar_p->length()', must equal
                     // 'd_calendar_p->lastDate() - d_calendar_p->firstDate()';
                     // if 'd_endFlag && 0 == d_calendar_p->length()', must
                     // equal 0

    bool                                       d_endFlag;
                     // indicates an 'end' iterator if set to 'true'

    // FRIENDS
    friend class PackedCalendar;
    friend bool operator==(const PackedCalendar_BusinessDayConstIterator&,
                           const PackedCalendar_BusinessDayConstIterator&);
    friend bool operator!=(const PackedCalendar_BusinessDayConstIterator&,
                           const PackedCalendar_BusinessDayConstIterator&);

  private:
    // PRIVATE TYPES
    typedef bdlc::PackedIntArray<int>::const_iterator OffsetsConstIterator;

    // PRIVATE CREATORS
    PackedCalendar_BusinessDayConstIterator(const PackedCalendar& calendar,
                                            const Date&           startDate,
                                            bool                  endIterFlag);
        // Create a business day iterator for the specified 'calendar'.  If the
        // specified 'endIterFlag' is 'false', then this iterator references
        // the first business day on or after the specified 'startDate';
        // otherwise, this iterator references one business day *past* the
        // first business day on or after 'startDate'.  If no business day
        // matching the above specification exists, then this iterator will
        // reference one day past the end of its range.

    // PRIVATE MANIPULATORS
    void nextBusinessDay();
        // Advance this iterator to the next business day.  The behavior is
        // undefined unless 'false == d_endFlag'.

    void previousBusinessDay();
        // Regress this iterator to the previous business day.  The behavior is
        // undefined unless '*this != d_calendar_p->beginBusinessDays()'.

  public:
    // TYPES
    typedef Date                     value_type;
    typedef int                      difference_type;
    typedef PackedCalendar_DateProxy pointer;
    typedef PackedCalendar_DateRef   reference;
        // The star operator returns a 'PackedCalendar_DateRef' *by* *value*.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE3.0

    typedef bsl::bidirectional_iterator_tag iterator_category;

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE3.0

    // CREATORS
    PackedCalendar_BusinessDayConstIterator(
                      const PackedCalendar_BusinessDayConstIterator& original);
        // Create an iterator having the value of the specified 'original'
        // iterator.

    ~PackedCalendar_BusinessDayConstIterator();
        // Destroy this object.

    // MANIPULATORS
    PackedCalendar_BusinessDayConstIterator& operator=(
                           const PackedCalendar_BusinessDayConstIterator& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator,
        // and return a reference providing modifiable access to this object.

    PackedCalendar_BusinessDayConstIterator& operator++();
        // Advance this iterator to refer to the next business day in the
        // associated calendar, and return a reference providing modifiable
        // access to this object.  The behavior is undefined unless, on entry,
        // this iterator references a valid business day.

    PackedCalendar_BusinessDayConstIterator& operator--();
        // Regress this iterator to refer to the previous business day in the
        // associated calendar, and return a reference providing modifiable
        // access to this object.  The behavior is undefined unless, on entry,
        // this iterator references a valid business day that is not the first
        // business day for the associated calendar.

    // ACCESSORS
    PackedCalendar_DateRef operator*() const;
        // Return a 'PackedCalendar_DateRef' object that contains the date
        // value of the business day referenced by this iterator.

    PackedCalendar_DateProxy operator->() const;
        // Return a date proxy for the business day referenced by this
        // iterator.
};

// FREE OPERATORS
bool operator==(const PackedCalendar_BusinessDayConstIterator& lhs,
                const PackedCalendar_BusinessDayConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators refer to the
    // same element in the same calendar, and 'false' otherwise.  The behavior
    // is undefined unless 'lhs' and 'rhs' both iterate over the same calendar.

bool operator!=(const PackedCalendar_BusinessDayConstIterator& lhs,
                const PackedCalendar_BusinessDayConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not refer to
    // the same element in the same calendar, and 'false' otherwise.  The
    // behavior is undefined unless 'lhs' and 'rhs' both iterate over the same
    // calendar.

PackedCalendar_BusinessDayConstIterator operator++(
                       PackedCalendar_BusinessDayConstIterator& iterator, int);
    // Advance the specified 'iterator' to refer to the next business day in
    // the associated calendar, and return the previous value of 'iterator'.
    // The behavior is undefined unless, on entry, 'iterator' references a
    // valid business day.

PackedCalendar_BusinessDayConstIterator operator--(
                       PackedCalendar_BusinessDayConstIterator& iterator, int);
    // Regress the specified 'iterator' to refer to the previous business day
    // in the associated calendar, and return the previous value of 'iterator'.
    // The behavior is undefined unless, on entry, 'iterator' references a
    // valid business day that is not the first business day for the associated
    // calendar.

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                     // ------------------------------
                     // class PackedCalendar_DateProxy
                     // ------------------------------

// CREATORS
inline
PackedCalendar_DateProxy::PackedCalendar_DateProxy(const Date& date)
: d_date(date)
{
}

inline
PackedCalendar_DateProxy::~PackedCalendar_DateProxy()
{
}

inline
PackedCalendar_DateProxy::PackedCalendar_DateProxy(
                                      const PackedCalendar_DateProxy& original)
: d_date(original.d_date)
{
}

// ACCESSORS
inline
const Date *PackedCalendar_DateProxy::operator->() const
{
    return &d_date;
}

                        // ----------------------------
                        // class PackedCalendar_DateRef
                        // ----------------------------

// CREATORS
inline
PackedCalendar_DateRef::PackedCalendar_DateRef(const Date& date)
: Date(date)
{
}

inline
PackedCalendar_DateRef::PackedCalendar_DateRef(
                                        const PackedCalendar_DateRef& original)
: Date(original)
{
}

inline
PackedCalendar_DateRef::~PackedCalendar_DateRef()
{
}

// ACCESSORS
inline
PackedCalendar_DateProxy PackedCalendar_DateRef::operator&() const
{
    return *this;
}

                    // -----------------------------------------
                    // class PackedCalendar_HolidayConstIterator
                    // -----------------------------------------

// PRIVATE CREATORS
inline
PackedCalendar_HolidayConstIterator::
     PackedCalendar_HolidayConstIterator(const OffsetsConstIterator& iter,
                                         const Date                  firstDate)
: d_iterator(iter)
, d_firstDate(firstDate)
{
}

// CREATORS
inline
PackedCalendar_HolidayConstIterator::PackedCalendar_HolidayConstIterator(
                           const PackedCalendar_HolidayConstIterator& original)
: d_iterator(original.d_iterator)
, d_firstDate(original.d_firstDate)
{
}

inline
PackedCalendar_HolidayConstIterator::~PackedCalendar_HolidayConstIterator()
{
}

// MANIPULATORS
inline
PackedCalendar_HolidayConstIterator& PackedCalendar_HolidayConstIterator::
                      operator=(const PackedCalendar_HolidayConstIterator& rhs)
{
    d_iterator  = rhs.d_iterator;
    d_firstDate = rhs.d_firstDate;
    return *this;
}

inline
PackedCalendar_HolidayConstIterator&
                              PackedCalendar_HolidayConstIterator::operator++()
{
    ++d_iterator;
    return *this;
}

inline
PackedCalendar_HolidayConstIterator&
                              PackedCalendar_HolidayConstIterator::operator--()
{
    --d_iterator;
    return *this;
}

// ACCESSORS
inline
PackedCalendar_DateRef PackedCalendar_HolidayConstIterator::operator*() const
{
    return PackedCalendar_DateRef(d_firstDate + *d_iterator);
}

inline
PackedCalendar_DateProxy
                        PackedCalendar_HolidayConstIterator::operator->() const
{
    return PackedCalendar_DateProxy(this->operator*());
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const PackedCalendar_HolidayConstIterator& lhs,
                      const PackedCalendar_HolidayConstIterator& rhs)
{
    return lhs.d_iterator == rhs.d_iterator;
}

inline
bool bdlt::operator!=(const PackedCalendar_HolidayConstIterator& lhs,
                      const PackedCalendar_HolidayConstIterator& rhs)
{
    return lhs.d_iterator != rhs.d_iterator;
}

inline
bdlt::PackedCalendar_HolidayConstIterator
           bdlt::operator++(PackedCalendar_HolidayConstIterator& iterator, int)
{
    PackedCalendar_HolidayConstIterator tmp(iterator);
    ++iterator;
    return tmp;
}

inline
bdlt::PackedCalendar_HolidayConstIterator
           bdlt::operator--(PackedCalendar_HolidayConstIterator& iterator, int)
{
    PackedCalendar_HolidayConstIterator tmp(iterator);
    --iterator;
    return tmp;
}

namespace bdlt {

                // ---------------------------------------------
                // class PackedCalendar_HolidayCodeConstIterator
                // ---------------------------------------------

// PRIVATE CREATORS
inline
PackedCalendar_HolidayCodeConstIterator::
        PackedCalendar_HolidayCodeConstIterator(const CodesConstIterator& iter)
: d_iterator(iter)
{
}

// CREATORS
inline
PackedCalendar_HolidayCodeConstIterator::
PackedCalendar_HolidayCodeConstIterator(
                       const PackedCalendar_HolidayCodeConstIterator& original)
: d_iterator(original.d_iterator)
{
}

inline
PackedCalendar_HolidayCodeConstIterator::
                                     ~PackedCalendar_HolidayCodeConstIterator()
{
}

// MANIPULATORS
inline
PackedCalendar_HolidayCodeConstIterator&
PackedCalendar_HolidayCodeConstIterator::
                  operator=(const PackedCalendar_HolidayCodeConstIterator& rhs)
{
    d_iterator = rhs.d_iterator;
    return *this;
}

inline
PackedCalendar_HolidayCodeConstIterator&
                          PackedCalendar_HolidayCodeConstIterator::operator++()
{
    ++d_iterator;
    return *this;
}

inline
PackedCalendar_HolidayCodeConstIterator&
                          PackedCalendar_HolidayCodeConstIterator::operator--()
{
    --d_iterator;
    return *this;
}

// ACCESSORS
inline
int PackedCalendar_HolidayCodeConstIterator::operator*() const
{
    return *d_iterator;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const PackedCalendar_HolidayCodeConstIterator& lhs,
                      const PackedCalendar_HolidayCodeConstIterator& rhs)
{
    return lhs.d_iterator == rhs.d_iterator;
}

inline
bool bdlt::operator!=(const PackedCalendar_HolidayCodeConstIterator& lhs,
                      const PackedCalendar_HolidayCodeConstIterator& rhs)
{
    return lhs.d_iterator != rhs.d_iterator;
}

inline
bdlt::PackedCalendar_HolidayCodeConstIterator
       bdlt::operator++(PackedCalendar_HolidayCodeConstIterator& iterator, int)
{
    bdlt::PackedCalendar_HolidayCodeConstIterator tmp(iterator);
    ++iterator;
    return tmp;
}

inline
bdlt::PackedCalendar_HolidayCodeConstIterator
       bdlt::operator--(PackedCalendar_HolidayCodeConstIterator& iterator, int)
{
    PackedCalendar_HolidayCodeConstIterator tmp(iterator);
    --iterator;
    return tmp;
}

inline
bsl::ptrdiff_t
            bdlt::operator-(const PackedCalendar_HolidayCodeConstIterator& lhs,
                            const PackedCalendar_HolidayCodeConstIterator& rhs)
{
    return lhs.d_iterator - rhs.d_iterator;
}

namespace bdlt {

               // ---------------------------------------------
               // class PackedCalendar_BusinessDayConstIterator
               // ---------------------------------------------

// CREATORS
inline
PackedCalendar_BusinessDayConstIterator::
PackedCalendar_BusinessDayConstIterator(
                       const PackedCalendar_BusinessDayConstIterator& original)
: d_offsetIter(original.d_offsetIter)
, d_calendar_p(original.d_calendar_p)
, d_currentOffset(original.d_currentOffset)
, d_endFlag(original.d_endFlag)
{
}

inline
PackedCalendar_BusinessDayConstIterator::
                                     ~PackedCalendar_BusinessDayConstIterator()
{
}

// MANIPULATORS
inline
PackedCalendar_BusinessDayConstIterator&
                          PackedCalendar_BusinessDayConstIterator::operator++()
{
    BSLS_ASSERT_SAFE(false == d_endFlag);

    nextBusinessDay();
    return *this;
}

inline
PackedCalendar_BusinessDayConstIterator&
                          PackedCalendar_BusinessDayConstIterator::operator--()
{
    BSLS_ASSERT_SAFE(*this != d_calendar_p->beginBusinessDays());

    previousBusinessDay();
    return *this;
}

// ACCESSORS
inline
PackedCalendar_DateRef
                     PackedCalendar_BusinessDayConstIterator::operator*() const
{
    return PackedCalendar_DateRef(d_calendar_p->firstDate() + d_currentOffset);
}

inline
PackedCalendar_DateProxy
                    PackedCalendar_BusinessDayConstIterator::operator->() const
{
    return PackedCalendar_DateProxy(this->operator*());
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const PackedCalendar_BusinessDayConstIterator& lhs,
                      const PackedCalendar_BusinessDayConstIterator& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_calendar_p == rhs.d_calendar_p);

    return lhs.d_endFlag       == rhs.d_endFlag
        && lhs.d_currentOffset == rhs.d_currentOffset;
}

inline
bool bdlt::operator!=(const PackedCalendar_BusinessDayConstIterator& lhs,
                      const PackedCalendar_BusinessDayConstIterator& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_calendar_p == rhs.d_calendar_p);

    return lhs.d_endFlag       != rhs.d_endFlag
        || lhs.d_currentOffset != rhs.d_currentOffset;
}

inline
bdlt::PackedCalendar_BusinessDayConstIterator bdlt::operator++(
                        PackedCalendar_BusinessDayConstIterator& iterator, int)
{
    PackedCalendar_BusinessDayConstIterator tmp(iterator);
    ++iterator;
    return tmp;
}

inline
bdlt::PackedCalendar_BusinessDayConstIterator bdlt::operator--(
                        PackedCalendar_BusinessDayConstIterator& iterator, int)
{
    PackedCalendar_BusinessDayConstIterator tmp(iterator);
    --iterator;
    return tmp;
}

namespace bdlt {

                         // --------------------
                         // class PackedCalendar
                         // --------------------

// PRIVATE ACCESSORS
inline
PackedCalendar::CodesConstIterator
      PackedCalendar::beginHolidayCodes(const OffsetsConstIterator& iter) const
{
    BSLS_ASSERT_SAFE(isHoliday(d_firstDate + *iter));

    const int indexOffset = static_cast<int>(iter - d_holidayOffsets.begin());

    const bsl::size_t codeOffset = d_holidayCodesIndex[indexOffset];

    return d_holidayCodes.begin() + codeOffset;
}

inline
PackedCalendar::CodesConstIterator
        PackedCalendar::endHolidayCodes(const OffsetsConstIterator& iter) const
{
    BSLS_ASSERT_SAFE(isHoliday(d_firstDate + *iter));

    const bsl::size_t endIndexOffset = iter - d_holidayOffsets.begin() + 1;

    const bsl::size_t iterIndex = endIndexOffset ==
                                                   d_holidayCodesIndex.length()
                                  ? d_holidayCodes.length()
                                  : d_holidayCodesIndex[endIndexOffset];
    return d_holidayCodes.begin() + iterIndex;
}

// CLASS METHODS

                                  // Aspects

inline
int PackedCalendar::maxSupportedBdexVersion(int versionSelector)
{
    if (versionSelector >= 20150612) {
        return 3;                                                     // RETURN
    }
    return 2;
}

// MANIPULATORS
inline
int PackedCalendar::addHolidayCodeIfInRange(const Date& date, int holidayCode)
{
    if (isInRange(date)) {
        addHolidayCode(date, holidayCode);
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int PackedCalendar::addHolidayIfInRange(const Date& date)
{
    if (isInRange(date)) {
        addHoliday(date);
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
void PackedCalendar::reserveHolidayCapacity(int numHolidays)
{
    BSLS_ASSERT(0 <= numHolidays);

    d_holidayOffsets.reserveCapacity(numHolidays);
    d_holidayCodesIndex.reserveCapacity(numHolidays);
}

inline
void PackedCalendar::reserveHolidayCodeCapacity(int numHolidayCodes)
{
    BSLS_ASSERT(0 <= numHolidayCodes);

    d_holidayCodes.reserveCapacity(numHolidayCodes);
}

                                  // Aspects

template <class STREAM>
STREAM& PackedCalendar::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 3: {
            bdlt::Date firstDate;
            firstDate.bdexStreamIn(stream, 1);

            if (!stream) {
                return stream;                                        // RETURN
            }

            bdlt::Date lastDate;
            lastDate.bdexStreamIn(stream, 1);

            if (!stream
             || (firstDate > lastDate
              && (firstDate != Date(9999,12,31)
                  || lastDate  != Date(1,1,1)))) {
                stream.invalidate();
                return stream;                                        // RETURN
            }

            int length = lastDate - firstDate + 1;

            int transitionsLength = 0;
            stream.getLength(transitionsLength);

            if (!stream || transitionsLength < 0) {
                stream.invalidate();
                return stream;                                        // RETURN
            }

            bsl::vector<WeekendDaysTransition>
                      weekendDaysTransitions(transitionsLength, d_allocator_p);
            for (int i = 0; i < transitionsLength; ++i) {
                WeekendDaysTransition& wdt = weekendDaysTransitions[i];

                wdt.first.bdexStreamIn(stream, 1);

                if (!stream) {
                    return stream;                                    // RETURN
                }

                // The data must be monotonically increasing.

                if (i && wdt.first <= weekendDaysTransitions[i - 1].first) {
                    stream.invalidate();
                    return stream;                                    // RETURN
                }

                wdt.second.bdexStreamIn(stream, 1);

                if (!stream) {
                    return stream;                                    // RETURN
                }
            }

            bdlc::PackedIntArray<int> holidayOffsets(d_allocator_p);
            holidayOffsets.bdexStreamIn(stream, 1);

            if (!stream
             || (firstDate >  lastDate && !holidayOffsets.isEmpty())
             || (firstDate <= lastDate
              && (static_cast<int>(holidayOffsets.length()) > length
               || (!holidayOffsets.isEmpty()
                && (holidayOffsets[0] < 0
                 || holidayOffsets[0] > length))))) {
                stream.invalidate();
                return stream;                                        // RETURN
            }

            // The values in 'holidayOffsets' must be monotonically increasing
            // and represent dates within the valid range.

            for (bsl::size_t i = 1; i < holidayOffsets.length(); ++i) {
                if (holidayOffsets[i - 1] >= holidayOffsets[i]
                 || holidayOffsets[i] > length) {
                    stream.invalidate();
                    return stream;                                    // RETURN
                }
            }

            bdlc::PackedIntArray<int> holidayCodesIndex(d_allocator_p);
            holidayCodesIndex.bdexStreamIn(stream, 1);

            if (!stream
             || holidayCodesIndex.length() != holidayOffsets.length()
             || (!holidayCodesIndex.isEmpty() && holidayCodesIndex[0] != 0)) {
                stream.invalidate();
                return stream;                                        // RETURN
            }

            // The values in 'holidayCodesIndex' must be monotonically
            // non-decreasing.

            for (bsl::size_t i = 1; i < holidayCodesIndex.length(); ++i) {
                if (holidayCodesIndex[i - 1] > holidayCodesIndex[i]) {
                    stream.invalidate();
                    return stream;                                    // RETURN
                }
            }

            bdlc::PackedIntArray<int> holidayCodes(d_allocator_p);
            holidayCodes.bdexStreamIn(stream, 1);

            if (!stream
             || (holidayOffsets.isEmpty() && !holidayCodes.isEmpty())) {
                stream.invalidate();
                return stream;                                        // RETURN
            }

            // The values in the segments of 'holidayOffsets' must be
            // monotonically increasing.

            bsl::size_t index = 0;
            for (int i = 0; i < static_cast<int>(holidayCodes.length()); ++i) {
                if (index < holidayCodesIndex.length()
                 && i == holidayCodesIndex[index]) {
                    ++index;
                    while (index < holidayCodesIndex.length()
                        && i == holidayCodesIndex[index]) {
                        ++index;
                    }
                }
                else if (holidayCodes[i - 1] >= holidayCodes[i]) {
                    stream.invalidate();
                    return stream;                                    // RETURN
                }
            }

            bslalg::SwapUtil::swap(&d_firstDate,         &firstDate);
            bslalg::SwapUtil::swap(&d_lastDate,          &lastDate);
            bslalg::SwapUtil::swap(&d_weekendDaysTransitions,
                                   &weekendDaysTransitions);
            bslalg::SwapUtil::swap(&d_holidayOffsets,    &holidayOffsets);
            bslalg::SwapUtil::swap(&d_holidayCodesIndex, &holidayCodesIndex);
            bslalg::SwapUtil::swap(&d_holidayCodes,      &holidayCodes);
          } break;
          case 2: {
            PackedCalendar inCal(d_allocator_p);
            inCal.d_firstDate.bdexStreamIn(stream, 1);
            if (!stream) {
                return stream;                                        // RETURN
            }

            inCal.d_lastDate.bdexStreamIn(stream, 1);
            if (!stream
             || (inCal.d_firstDate > inCal.d_lastDate
              && (inCal.d_firstDate != Date(9999,12,31)
               || inCal.d_lastDate  != Date(1,1,1)))) {
                stream.invalidate();
                return stream;                                        // RETURN
            }
            int length = inCal.d_lastDate - inCal.d_firstDate + 1;

            int transitionsLength = 0;
            stream.getLength(transitionsLength);
            if (!stream || transitionsLength < 0)
            {
                stream.invalidate();
                return stream;                                        // RETURN
            }

            int offsetsLength = 0;
            stream.getLength(offsetsLength);
            if (!stream
             || (inCal.d_firstDate >  inCal.d_lastDate
              && offsetsLength != 0)
             || (inCal.d_firstDate <= inCal.d_lastDate
              && (offsetsLength < 0 || offsetsLength > length))) {
                stream.invalidate();
                return stream;                                        // RETURN
            }
            BSLS_ASSERT(offsetsLength >= 0);

            int codesLength = 0;
            stream.getLength(codesLength);
            if (!stream || (0 == offsetsLength && codesLength != 0)) {
                stream.invalidate();
                return stream;                                        // RETURN
            }

            inCal.d_weekendDaysTransitions.resize(transitionsLength);

            // To ensure 'inCal' cannot be in an invalid state, we must first
            // reserve capacity for the three 'PackedIntArray' and then resize
            // them to guarantee the operations can complete without
            // re-allocation *and* the object invariants (see the destructor
            // implementation) are maintained.

            inCal.d_holidayOffsets.reserveCapacity(offsetsLength);
            inCal.d_holidayCodesIndex.reserveCapacity(offsetsLength);
            inCal.d_holidayCodes.reserveCapacity(codesLength);

            inCal.d_holidayOffsets.resize(offsetsLength);
            inCal.d_holidayCodesIndex.resize(offsetsLength);
            inCal.d_holidayCodes.resize(codesLength);

            for (WeekendDaysTransitionSequence::iterator it =
                     inCal.d_weekendDaysTransitions.begin();
                 it != inCal.d_weekendDaysTransitions.end();
                 ++it) {
                it->first.bdexStreamIn(stream, 1);
                if (!stream) {
                    return stream;                                    // RETURN
                }

                if (it        != inCal.d_weekendDaysTransitions.begin()
                 && it->first <= (it - 1)->first) {
                    stream.invalidate();
                    return stream;                                    // RETURN
                }

                it->second.bdexStreamIn(stream, 1);
                if (!stream) {
                    return stream;                                    // RETURN
                }
            }

            int previousValue = -1;
            for (int i = 0; i < offsetsLength; ++i) {
                int tmp;
                stream.getInt32(tmp);
                if (!stream
                 || tmp < 0
                 || tmp >= length
                 || tmp <= previousValue) {
                    stream.invalidate();
                    return stream;                                    // RETURN
                }
                inCal.d_holidayOffsets.replace(i, tmp);
                previousValue = tmp;
            }

            previousValue = -1;
            for (int i = 0; i < offsetsLength; ++i) {
                int tmp;
                stream.getInt32(tmp);

                // This vector is ordered but duplicates are allowed.  The
                // first element must be 0.

                if (!stream
                 || tmp < 0
                 || tmp < previousValue
                 || tmp > codesLength
                 || (0 == i && 0 != tmp)) {

                    // If we get here, some of the code indices could
                    // potentially be greater than 'codesLength'.  That would
                    // trigger an assertion in the destructor.  So call
                    // 'removeAll' to clean up.

                    inCal.removeAll();
                    stream.invalidate();
                    return stream;                                    // RETURN
                }
                inCal.d_holidayCodesIndex.replace(i, tmp);
                previousValue = tmp;
            }

            CodesIndexConstIterator it = inCal.d_holidayCodesIndex.begin();
            CodesIndexConstIterator end = inCal.d_holidayCodesIndex.end();

            // Skip the holidays that have no codes.

            while (it != end && 0 == *it) {
                ++it;
            }

            // 'it' is now positioned at the first holiday with one or more
            // codes or at the end.

            bool previousValueFlag = false;  // This flag is used to determine
                                             // if we are inside an ordered
                                             // sequence of codes (i.e.,
                                             // 'previousValue' refers to a
                                             // code for the same holiday as
                                             // 'value').

            for (int i = 0; i < codesLength; ++i) {
                int tmp;
                stream.getInt32(tmp);
                if (!stream
                 || (previousValueFlag && tmp <= previousValue)) {
                    stream.invalidate();
                    return stream;                                    // RETURN
                }
                inCal.d_holidayCodes.replace(i, tmp);

                // Regardless of whether or not there is more data, advance the
                // index iterator as needed and update 'previousValueFlag' if
                // 'it' moves.

                if (it != end && i == (*it - 1)) {
                    previousValueFlag = false;

                    while (it != end && i == (*it - 1)) {
                        ++it;  // Skip the holidays that have no codes.
                    }
                }
                else {
                    previousValueFlag = true;
                }
                previousValue = tmp;
            }
            BSLS_ASSERT(it == end);

            swap(inCal);  // This cannot throw.
          } break;
          case 1: {
            PackedCalendar inCal(d_allocator_p);
            inCal.d_firstDate.bdexStreamIn(stream, 1);
            if (!stream) {
                return stream;                                        // RETURN
            }

            inCal.d_lastDate.bdexStreamIn(stream, 1);
            if (!stream
             || (inCal.d_firstDate > inCal.d_lastDate
              && (inCal.d_firstDate != Date(9999, 12, 31)
               || inCal.d_lastDate  != Date(1, 1, 1)))) {
                stream.invalidate();
                return stream;                                        // RETURN
            }
            int length = inCal.d_lastDate - inCal.d_firstDate + 1;

            DayOfWeekSet weekendDays;
            weekendDays.bdexStreamIn(stream, 1);
            if (!stream) {
                return stream;                                        // RETURN
            }

            if (weekendDays.length() > 0) {
                inCal.addWeekendDays(weekendDays);
            }

            int offsetsLength = 0;
            stream.getLength(offsetsLength);
            if (!stream
             || (inCal.d_firstDate >  inCal.d_lastDate
              && offsetsLength != 0)
             || (inCal.d_firstDate <= inCal.d_lastDate
              && (offsetsLength < 0 || offsetsLength > length))) {
                stream.invalidate();
                return stream;                                        // RETURN
            }
            BSLS_ASSERT(offsetsLength >= 0);

            int codesLength = 0;
            stream.getLength(codesLength);
            if (!stream || (0 == offsetsLength && codesLength != 0)) {
                stream.invalidate();
                return stream;                                        // RETURN
            }

            // To ensure 'inCal' cannot be in an invalid state, we must first
            // reserve capacity for the three 'PackedIntArray' and then resize
            // them to guarantee the operations can complete without
            // re-allocation *and* the object invariants (see the destructor
            // implementation) are maintained.

            inCal.d_holidayOffsets.reserveCapacity(offsetsLength);
            inCal.d_holidayCodesIndex.reserveCapacity(offsetsLength);
            inCal.d_holidayCodes.reserveCapacity(codesLength);

            inCal.d_holidayOffsets.resize(offsetsLength);
            inCal.d_holidayCodesIndex.resize(offsetsLength);
            inCal.d_holidayCodes.resize(codesLength);

            int previousValue = -1;
            for (int i = 0; i < offsetsLength; ++i) {
                int tmp;
                stream.getInt32(tmp);
                if (!stream
                 || tmp < 0
                 || tmp >= length
                 || tmp <= previousValue) {
                    stream.invalidate();
                    return stream;                                    // RETURN
                }
                inCal.d_holidayOffsets.replace(i, tmp);
                previousValue = tmp;
            }

            previousValue = -1;
            for (int i = 0; i < offsetsLength; ++i) {
                int tmp;
                stream.getInt32(tmp);

                // This vector is ordered but duplicates are allowed.  The
                // first element must be 0.

                if (!stream
                 || tmp < 0
                 || tmp < previousValue
                 || tmp > codesLength
                 || (0 == i && 0 != tmp)) {

                    // If we get here, some of the code indices could
                    // potentially be greater than 'codesLength'.  That would
                    // trigger an assertion in the destructor.  So call
                    // 'removeAll' to clean up.

                    inCal.removeAll();
                    stream.invalidate();
                    return stream;                                    // RETURN
                }
                inCal.d_holidayCodesIndex.replace(i, tmp);
                previousValue = tmp;
            }

            CodesIndexConstIterator it = inCal.d_holidayCodesIndex.begin();
            CodesIndexConstIterator end = inCal.d_holidayCodesIndex.end();

            // Skip the holidays that have no codes.

            while (it != end && 0 == *it) {
                ++it;
            }

            // 'it' is now positioned at the first holiday with one or more
            // codes or at the end.

            bool previousValueFlag = false;  // This flag is used to determine
                                             // if we are inside an ordered
                                             // sequence of codes (i.e.,
                                             // 'previousValue' refers to a
                                             // code for the same holiday as
                                             // 'value').

            for (int i = 0; i < codesLength; ++i) {
                int tmp;
                stream.getInt32(tmp);
                if (!stream
                 || (previousValueFlag && tmp <= previousValue)) {
                    stream.invalidate();
                    return stream;                                    // RETURN
                }
                inCal.d_holidayCodes.replace(i, tmp);

                // Regardless of whether or not there is more data, advance the
                // index iterator as needed and update 'previousValueFlag' if
                // 'it' moves.

                if (it != end && i == (*it - 1)) {
                    previousValueFlag = false;

                    while (it != end && i == (*it - 1)) {
                        ++it;  // Skip the holidays that have no codes.
                    }
                }
                else {
                    previousValueFlag = true;
                }
                previousValue = tmp;
            }
            BSLS_ASSERT(it == end);

            swap(inCal);  // This cannot throw.
          } break;
          default: {
            stream.invalidate();
          }
        }
    }
    return stream;
}

// ACCESSORS
inline
PackedCalendar::BusinessDayConstIterator
                                      PackedCalendar::beginBusinessDays() const
{
    return BusinessDayConstIterator(*this, d_firstDate, false);
}

inline
PackedCalendar::BusinessDayConstIterator
                      PackedCalendar::beginBusinessDays(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return BusinessDayConstIterator(*this, date, false);
}

inline
PackedCalendar::HolidayCodeConstIterator
      PackedCalendar::beginHolidayCodes(const HolidayConstIterator& iter) const
{
    BSLS_ASSERT_SAFE(isHoliday(*iter));

    return HolidayCodeConstIterator(beginHolidayCodes(iter.d_iterator));
}

inline
PackedCalendar::HolidayConstIterator PackedCalendar::beginHolidays() const
{
    return HolidayConstIterator(d_holidayOffsets.begin(), d_firstDate);
}

inline
PackedCalendar::HolidayConstIterator
                          PackedCalendar::beginHolidays(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    OffsetsConstIterator i = bdlc::PackedIntArrayUtil::lowerBound(
                                                      d_holidayOffsets.begin(),
                                                      d_holidayOffsets.end(),
                                                      date - d_firstDate);
    return HolidayConstIterator(i, d_firstDate);
}

inline
PackedCalendar::WeekendDaysTransitionConstIterator
                            PackedCalendar::beginWeekendDaysTransitions() const
{
    return d_weekendDaysTransitions.begin();
}

inline
PackedCalendar::BusinessDayConstIterator
                                        PackedCalendar::endBusinessDays() const
{
    return BusinessDayConstIterator(*this, d_lastDate, true);
}

inline
PackedCalendar::BusinessDayConstIterator
                        PackedCalendar::endBusinessDays(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return BusinessDayConstIterator(*this, date, true);
}

inline
PackedCalendar::HolidayCodeConstIterator
        PackedCalendar::endHolidayCodes(const HolidayConstIterator& iter) const
{
    BSLS_ASSERT_SAFE(isHoliday(*iter));

    return endHolidayCodes(iter.d_iterator);
}

inline
PackedCalendar::HolidayConstIterator PackedCalendar::endHolidays() const
{
    return HolidayConstIterator(d_holidayOffsets.end(), d_firstDate);
}

inline
PackedCalendar::HolidayConstIterator
                            PackedCalendar::endHolidays(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    OffsetsConstIterator i = bdlc::PackedIntArrayUtil::upperBound(
                                                      d_holidayOffsets.begin(),
                                                      d_holidayOffsets.end(),
                                                      date - d_firstDate);
    return HolidayConstIterator(i, d_firstDate);
}

inline
PackedCalendar::WeekendDaysTransitionConstIterator
                              PackedCalendar::endWeekendDaysTransitions() const
{
    return d_weekendDaysTransitions.end();
}

inline
const Date& PackedCalendar::firstDate() const
{
    BSLS_ASSERT_SAFE(d_firstDate <= d_lastDate);

    return d_firstDate;
}

inline
int PackedCalendar::getNextBusinessDay(Date        *nextBusinessDay,
                                       const Date&  date) const
{
    BSLS_ASSERT_SAFE(nextBusinessDay);
    BSLS_ASSERT_SAFE(Date(9999, 12, 31) > date);
    BSLS_ASSERT_SAFE(isInRange(date + 1));

    enum { e_SUCCESS = 0, e_FAILURE = 1 };

    BusinessDayConstIterator iter = beginBusinessDays(date + 1);
    if (iter == endBusinessDays()) {
        return e_FAILURE;                                             // RETURN
    }
    *nextBusinessDay = *iter;
    return e_SUCCESS;
}

inline
bdlt::Date PackedCalendar::holiday(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < numHolidays());

    return d_firstDate + d_holidayOffsets[index];
}

inline
int PackedCalendar::holidayCode(const Date& date, int index) const
{
    BSLS_ASSERT_SAFE(isInRange(date));
    BSLS_ASSERT_SAFE(isHoliday(date));
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < numHolidayCodes(date));

    const int                  offset = date - d_firstDate;
    const OffsetsConstIterator offsetBegin = d_holidayOffsets.begin();
    const OffsetsConstIterator offsetEnd   = d_holidayOffsets.end();
    const OffsetsConstIterator i = bdlc::PackedIntArrayUtil::lowerBound(
                                                                   offsetBegin,
                                                                   offsetEnd,
                                                                   offset);
    return d_holidayCodes[d_holidayCodesIndex[i - offsetBegin] + index];
}

inline
bool PackedCalendar::isBusinessDay(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return !isNonBusinessDay(date);
}

inline
bool PackedCalendar::isHoliday(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    const int                  offset = date - d_firstDate;
    const OffsetsConstIterator offsetEnd = d_holidayOffsets.end();
    const OffsetsConstIterator i = bdlc::PackedIntArrayUtil::lowerBound(
                                                      d_holidayOffsets.begin(),
                                                      offsetEnd,
                                                      offset);

    return i != offsetEnd && *i == offset;
}

inline
bool PackedCalendar::isInRange(const Date& date) const
{
    return d_firstDate <= date && date <= d_lastDate;
}

inline
bool PackedCalendar::isNonBusinessDay(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return isWeekendDay(date) || isHoliday(date);
}

inline
bool PackedCalendar::isWeekendDay(DayOfWeek::Enum dayOfWeek) const
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT_SAFE(d_weekendDaysTransitions.size() <= 1);
    BSLS_PRECONDITIONS_END();

    if (d_weekendDaysTransitions.empty()) {
        return false;                                                 // RETURN
    }

    BSLS_ASSERT_SAFE(d_weekendDaysTransitions[0].first == Date(1,1,1));

    return d_weekendDaysTransitions[0].second.isMember(dayOfWeek);
}

inline
const Date& PackedCalendar::lastDate() const
{
    BSLS_ASSERT_SAFE(d_firstDate <= d_lastDate);

    return d_lastDate;
}

inline
int PackedCalendar::length() const
{
    return d_firstDate <= d_lastDate ? d_lastDate - d_firstDate + 1 : 0;
}

inline
int PackedCalendar::numBusinessDays() const
{
    return length() - numNonBusinessDays();
}

inline
int PackedCalendar::numHolidayCodesTotal() const
{
    return static_cast<int>(d_holidayCodes.length());
}

inline
int PackedCalendar::numHolidays() const
{
    return static_cast<int>(d_holidayOffsets.length());
}

inline
int PackedCalendar::numWeekendDaysTransitions() const
{
    return static_cast<int>(d_weekendDaysTransitions.size());
}

inline
PackedCalendar::BusinessDayConstReverseIterator
                                     PackedCalendar::rbeginBusinessDays() const
{
    return BusinessDayConstReverseIterator(endBusinessDays());
}

inline
PackedCalendar::BusinessDayConstReverseIterator
                     PackedCalendar::rbeginBusinessDays(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return BusinessDayConstReverseIterator(endBusinessDays(date));
}

inline
PackedCalendar::HolidayCodeConstReverseIterator
                     PackedCalendar::rbeginHolidayCodes(const Date& date) const
{
    BSLS_ASSERT_SAFE(isHoliday(date));

    return HolidayCodeConstReverseIterator(endHolidayCodes(date));
}

inline
PackedCalendar::HolidayCodeConstReverseIterator
     PackedCalendar::rbeginHolidayCodes(const HolidayConstIterator& iter) const
{
    BSLS_ASSERT_SAFE(isHoliday(*iter));

    return HolidayCodeConstReverseIterator(endHolidayCodes(iter));
}

inline
PackedCalendar::HolidayConstReverseIterator
                                         PackedCalendar::rbeginHolidays() const
{
    return HolidayConstReverseIterator(endHolidays());
}

inline
PackedCalendar::HolidayConstReverseIterator
                         PackedCalendar::rbeginHolidays(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return HolidayConstReverseIterator(endHolidays(date));
}

inline
PackedCalendar::WeekendDaysTransitionConstReverseIterator
                           PackedCalendar::rbeginWeekendDaysTransitions() const
{
    return WeekendDaysTransitionConstReverseIterator(
                                               d_weekendDaysTransitions.end());
}

inline
PackedCalendar::BusinessDayConstReverseIterator
                                       PackedCalendar::rendBusinessDays() const
{
    return BusinessDayConstReverseIterator(beginBusinessDays());
}

inline
PackedCalendar::BusinessDayConstReverseIterator
                       PackedCalendar::rendBusinessDays(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return BusinessDayConstReverseIterator(beginBusinessDays(date));
}

inline
PackedCalendar::HolidayCodeConstReverseIterator
                       PackedCalendar::rendHolidayCodes(const Date& date) const
{
    BSLS_ASSERT_SAFE(isHoliday(date));

    return HolidayCodeConstReverseIterator(beginHolidayCodes(date));
}

inline
PackedCalendar::HolidayCodeConstReverseIterator
       PackedCalendar::rendHolidayCodes(const HolidayConstIterator& iter) const
{
    BSLS_ASSERT_SAFE(isHoliday(*iter));

    return HolidayCodeConstReverseIterator(beginHolidayCodes(iter));
}

inline
PackedCalendar::HolidayConstReverseIterator
                                           PackedCalendar::rendHolidays() const
{
    return HolidayConstReverseIterator(beginHolidays());
}

inline
PackedCalendar::HolidayConstReverseIterator
                           PackedCalendar::rendHolidays(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return HolidayConstReverseIterator(beginHolidays(date));
}

inline
PackedCalendar::WeekendDaysTransitionConstReverseIterator
                             PackedCalendar::rendWeekendDaysTransitions() const
{
    return WeekendDaysTransitionConstReverseIterator(
                                             d_weekendDaysTransitions.begin());
}

inline
PackedCalendar::WeekendDaysTransition
                         PackedCalendar::weekendDaysTransition(int index) const
{
    BSLS_ASSERT_SAFE(0 <= index);
    BSLS_ASSERT_SAFE(     index < numWeekendDaysTransitions());

    return d_weekendDaysTransitions[index];
}

                                  // Aspects

inline
bslma::Allocator *PackedCalendar::allocator() const
{
    return d_allocator_p;
}

template <class STREAM>
STREAM& PackedCalendar::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {  // Switch on the schema version (starting with 1).
      case 3: {
        d_firstDate.bdexStreamOut(stream, 1);
        d_lastDate.bdexStreamOut(stream, 1);

        stream.putLength(static_cast<int>(d_weekendDaysTransitions.size()));
        for (WeekendDaysTransitionSequence::size_type i = 0;
             i < d_weekendDaysTransitions.size();
             ++i) {
            d_weekendDaysTransitions[i].first.bdexStreamOut(stream, 1);
            d_weekendDaysTransitions[i].second.bdexStreamOut(stream, 1);
        }

        d_holidayOffsets.bdexStreamOut(stream, 1);
        d_holidayCodesIndex.bdexStreamOut(stream, 1);
        d_holidayCodes.bdexStreamOut(stream, 1);
      } break;
      case 2: {
        d_firstDate.bdexStreamOut(stream, 1);
        d_lastDate.bdexStreamOut(stream, 1);

        stream.putLength(static_cast<int>(d_weekendDaysTransitions.size()));
        stream.putLength(static_cast<int>(d_holidayOffsets.length()));
        stream.putLength(static_cast<int>(d_holidayCodes.length()));

        for (WeekendDaysTransitionSequence::size_type i = 0;
             i < d_weekendDaysTransitions.size();
             ++i) {
            d_weekendDaysTransitions[i].first.bdexStreamOut(stream, 1);
            d_weekendDaysTransitions[i].second.bdexStreamOut(stream, 1);
        }

        for (bsl::size_t i = 0; i < d_holidayOffsets.length(); ++i) {
            stream.putInt32(d_holidayOffsets[i]);
        }
        for (bsl::size_t i = 0; i < d_holidayCodesIndex.length(); ++i) {
            stream.putInt32(d_holidayCodesIndex[i]);
        }
        for (bsl::size_t i = 0; i < d_holidayCodes.length(); ++i) {
            stream.putInt32(d_holidayCodes[i]);
        }
      } break;
      case 1: {
        d_firstDate.bdexStreamOut(stream, 1);
        d_lastDate.bdexStreamOut(stream, 1);

        if (!d_weekendDaysTransitions.empty() &&
            d_weekendDaysTransitions[0].first == Date(1, 1, 1)) {
            d_weekendDaysTransitions[0].second.bdexStreamOut(stream, 1);
        }
        else {
            DayOfWeekSet tempSet;
            tempSet.bdexStreamOut(stream, 1);
        }

        stream.putLength(static_cast<int>(d_holidayOffsets.length()));
        stream.putLength(static_cast<int>(d_holidayCodes.length()));

        for (bsl::size_t i = 0; i < d_holidayOffsets.length(); ++i) {
            stream.putInt32(d_holidayOffsets[i]);
        }
        for (bsl::size_t i = 0; i < d_holidayCodesIndex.length(); ++i) {
            stream.putInt32(d_holidayCodesIndex[i]);
        }
        for (bsl::size_t i = 0; i < d_holidayCodes.length(); ++i) {
            stream.putInt32(d_holidayCodes[i]);
        }
      } break;
      default: {
        stream.invalidate();
      }

    }
    return stream;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22

// DEPRECATED METHODS
inline
int PackedCalendar::maxSupportedBdexVersion()
{
    return 1;
}

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator!=(const PackedCalendar& lhs, const PackedCalendar& rhs)
{
    return !(lhs == rhs);
}

// FREE FUNCTIONS
template <class HASHALG>
inline
void bdlt::hashAppend(HASHALG& hashAlg, const PackedCalendar& object)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlg, object.d_firstDate);
    hashAppend(hashAlg, object.d_lastDate);
    hashAppend(hashAlg, object.d_weekendDaysTransitions);
    hashAppend(hashAlg, object.d_holidayOffsets);
    hashAppend(hashAlg, object.d_holidayCodesIndex);
    hashAppend(hashAlg, object.d_holidayCodes);
}

}  // close enterprise namespace

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<bdlt::PackedCalendar> : bsl::true_type {};

}  // close namespace bslma
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
