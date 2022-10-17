// bdlt_calendar.h                                                    -*-C++-*-
#ifndef INCLUDED_BDLT_CALENDAR
#define INCLUDED_BDLT_CALENDAR

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide fast repository for accessing weekend/holiday information.
//
//@CLASSES:
//  bdlt::Calendar: fast repository for accessing weekend/holiday information
//
//@SEE_ALSO: bdlt_packedcalendar
//
//@DESCRIPTION: This component provides a value-semantic class,
// 'bdlt::Calendar', that represents weekend and holiday information over a
// *valid* *range* of dates.  A 'bdlt::Calendar' is an approximation to the
// same *mathematical* type, and is capable of representing the same subset of
// *mathematical* values, as a 'bdlt::PackedCalendar'.  A 'bdlt::Calendar'
// object (representing the same *mathematical* value) can be constructed
// directly from a reference to a non-modifiable 'bdlt::PackedCalendar' object,
// and a reference to a non-modifiable 'bdlt::PackedCalendar' is readily
// accessible from any 'bdlt::Calendar' object.
//
// But unlike 'bdlt::PackedCalendar', which is optimized for spatial
// efficiency, 'bdlt::Calendar' is designed to be especially efficient at
// determining whether a given 'bdlt::Date' value (within the valid range for a
// particular 'bdlt::Calendar' object) is a business day -- i.e., not a weekend
// day or holiday (see "Usage" below).  For example, the cost of determining
// whether a given 'bdlt::Date' is a business day, as opposed to a weekend or
// holiday, consists of only a few constant-time operations, compared to a
// binary search in a 'bdlt::PackedCalendar' representing the same calendar
// value.
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
// 'bdlt::Calendar' (or the current holiday referred to by a
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
// The modification of a 'bdlt::Calendar' will invalidate iterators referring
// to the calendar.  The following table shows the relationship between a
// calendar manipulator and the types of iterators it will invalidate if the
// invocation of the manipulator modified the calendar (e.g., using
// 'addHoliday' with a date that is not currently a holiday in the calendar):
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
// 'bdlt::Calendar' supports 'O[1]' (i.e., constant-time) determination of
// whether a given 'bdlt::Date' value is or is not a business day, which is
// accomplished by augmenting the implementation of a packed calendar with a
// supplementary cache.  The invariant that this cache and the data represented
// in the underlying 'bdlt::PackedCalendar' be maintained in a consistent
// state may add significantly to the cost of performing many manipulator
// operations, especially those that affect the calendar's valid range and add
// a new weekend-days transition.  Moreover, the cost of many of these
// operations will now be proportional to the length(s) of the valid range(s),
// as well as their respective numbers of holidays and associated holiday codes
// and weekend-days transitions.  Hence, when populating a calendar, it is
// recommended that the desired value be captured first as a
// 'bdlt::PackedCalendar', which can then be used efficiently to
// *value-construct* the desired 'bdlt::Calendar' object.  See the
// component-level doc for 'bdlt_packedcalendar' for its performance
// guarantees.
//
// All methods of the 'bdlt::Calendar' are exception-safe, but in general
// provide only the basic guarantee (i.e., no guarantee of rollback): If an
// exception occurs (i.e., while attempting to allocate memory), the calendar
// object is left in a coherent state, but (unless otherwise specified) its
// *value* is undefined.
//
///Usage
///-----
// The two subsections below illustrate various aspects of populating and using
// calendars.
//
///Example 1: Populating Calendars
///- - - - - - - - - - - - - - - -
// 'bdlt::Calendars' can be populated directly, but are often more efficiently
// created by first creating a corresponding 'bdlt::PackedCalendar', and then
// using that object to construct the calendar.  As an example, suppose we
// want to provide efficient access to a (high-performance) 'bdlt::Calendar'
// for a variety of locales, whose raw information comes from, say, a database.
// The latency associated with fetching data for individual calendars on
// demand from a typical database can be prohibitively expensive, so it may
// make sense to acquire data for all calendars in a single query at start-up.
//
// First, we declare a 'MyPackedCalendarCache' that, internally, is just a
// mapping from (typically short) character string names (such as "NYB",
// representing New York Bank settlement days) to 'bdlt::PackedCalendar'
// objects, containing densely packed calendar data:
//..
//  class MyPackedCalendarCache {
//      // This class maintains a space-efficient repository of calendar data
//      // associated with a (typically short) name.
//
//      // DATA
//      bsl::unordered_map<bsl::string, bdlt::PackedCalendar>  d_map;
//
//    public:
//      // CREATORS
//      MyPackedCalendarCache(bslma::Allocator *basicAllocator = 0);
//          // Create an empty 'MyPackedCalendarCache'.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//      // MANIPULATORS
//      void assign(const bsl::string&          name,
//                  const bdlt::PackedCalendar& calendar);
//          // Associate the value of the specified 'calendar' with the
//          // specified 'name'.
//
//      // ACCESSORS
//      const bdlt::PackedCalendar *lookup(const bsl::string& name) const;
//          // Return the address of calendar data associated with the
//          // specified 'name', or 0 if no such association exists.
//  };
//
//  // CREATORS
//  MyPackedCalendarCache::MyPackedCalendarCache(
//                                            bslma::Allocator *basicAllocator)
//  : d_map(basicAllocator)
//  {
//  }
//
//  // MANIPULATORS
//  void MyPackedCalendarCache::assign(const bsl::string&          name,
//                                     const bdlt::PackedCalendar& calendar)
//  {
//      d_map[name] = calendar;
//  }
//
//  // ACCESSORS
//  const bdlt::PackedCalendar *MyPackedCalendarCache::lookup(
//                                               const bsl::string& name) const
//  {
//      typedef bsl::unordered_map<bsl::string, bdlt::PackedCalendar> Cache;
//      Cache::const_iterator iter = d_map.find(name);
//
//      if (iter == d_map.end()) {
//          return 0;                                                 // RETURN
//      }
//      return &iter->second;
//  }
//..
// Then, we define an application function, 'loadMyPackedCalendarCache', that
// takes the address of a 'MyPackedCalendarCache' and populates it with
// up-to-date calendar data for all known locales (which, in the future, will
// be from a well-known database location):
//..
//  int loadMyPackedCalendarCache(MyPackedCalendarCache *result)
//      // Load, into the specified 'result', up-to-date calendar information
//      // for every known locale.  Return 0 on success, and a non-zero value
//      // otherwise.
//  {
//      bdlt::PackedCalendar calendar;
//      calendar.setValidRange(bdlt::Date(2000,  1,  1),
//                             bdlt::Date(2020, 12, 31));
//      result->assign("NYB", calendar);
//      return 0;
//  }
//..
// We can imagine that there might be dozens, even hundreds of different
// locales, and that most applications will not need efficient access to
// calendar data from many, let alone every locale; however, many long-running
// applications may well need to obtain efficient access to the same calendar
// data repeatedly.
//
// Next, we create a second-level of cache, 'MyCalendarCache', that maintains
// a repository of the more runtime-efficient, but also more space-intensive,
// 'bdlt::Calendar' objects, which are instantiated on demand from a
// packed-calendar-based data source:
//..
//  class MyCalendarCache {
//      // This class maintains a cache of runtime-efficient calendar objects
//      // created on demand from a compact packed-calendar-based data source,
//      // whose address is supplied at construction.
//
//      // DATA
//      MyPackedCalendarCache                           *d_datasource_p;
//      bsl::unordered_map<bsl::string, bdlt::Calendar>  d_map;
//
//    public:
//      // CREATORS
//      MyCalendarCache(MyPackedCalendarCache *dataSource,
//                      bslma::Allocator      *basicAllocator = 0);
//          // Create an empty 'MyCalendarCache' associated with the specified
//          // 'dataSource'.  Optionally specify a 'basicAllocator' used to
//          // supply memory.  If 'basicAllocator' is 0, the currently
//          // installed default allocator is used.
//
//      // MANIPULATORS
//      const bdlt::Calendar *lookup(const bsl::string& name);
//          // Return the address of calendar data associated with the
//          // specified 'name', or 0 if no such association exists in the data
//          // source whose address was supplied at construction.  Note that
//          // this method may alter the physical state of this object (and is
//          // therefore deliberately declared non-'const').
//  };
//
//  MyCalendarCache::MyCalendarCache(MyPackedCalendarCache *dataSource,
//                                   bslma::Allocator      *basicAllocator)
//  : d_datasource_p(dataSource)
//  , d_map(basicAllocator)
//  {
//  }
//
//  const bdlt::Calendar *MyCalendarCache::lookup(const bsl::string& name)
//  {
//      typedef bsl::unordered_map<bsl::string, bdlt::Calendar> Cache;
//      Cache::const_iterator iter = d_map.find(name);
//      if (iter == d_map.end()) {
//          const bdlt::PackedCalendar *pc = d_datasource_p->lookup(name);
//          if (!pc) {
//
//              // No such name in the data source.
//
//              return 0;                                             // RETURN
//          }
//
//          // Create new entry in calendar cache.
//
//          iter = d_map.insert(bsl::make_pair(name, *pc)).first;
//      }
//
//      // An efficient calendar either already existed or was created.
//
//      return &iter->second;
//   }
//..
// Now, we can create and populate the cache:
//..
//  MyPackedCalendarCache packedCalendarCache;
//  MyCalendarCache       calendarCache(&packedCalendarCache);
//
//  loadMyPackedCalendarCache(&packedCalendarCache);
//..
// Finally, we request the "NYB" calendar and verify the returned value:
//..
//  const bdlt::Calendar *calendarPtr = calendarCache.lookup("NYB");
//
//  assert(calendarPtr->firstDate() == bdlt::Date(2000,  1,  1));
//  assert(calendarPtr->lastDate()  == bdlt::Date(2020, 12, 31));
//..
//
///Example 2: Using Calendars
/// - - - - - - - - - - - - -
// What makes a 'bdlt::Calendar' substantially different from a
// 'bdlt::PackedCalendar' is the speed with which the 'bdlt::Calendar' can
// report whether a given date is or is not a business day.  An important use
// of high-performance calendar objects in financial applications is to quickly
// determine the settlement date of a financial instrument.  In some
// applications (e.g., those that explore the cross product of various
// portfolios over several horizons and scenarios), the settlement date may
// need to be calculated literally billions of times.  The settlement date
// will often be determined from a periodic target date, such as the 15th or
// 30th of the month, which is then perturbed in some way to arrive at a valid
// settlement date.
//
// One very common algorithm a security may prescribe for finding a valid
// settlement date is known as *modified* *following*: Given a target day, the
// settlement date for that month is defined as the first valid business day
// at or after the given target day in the same month; if no such date exists,
// then the settlement date is the closest valid business day before the target
// day in that month.
//
// First, we create a 'struct', 'MyCalendarUtil', that provides the
// 'modifiedFollowing' method:
//..
//  struct MyCalendarUtil {
//
//      // CLASS METHODS
//      static bdlt::Date modifiedFollowing(int                   targetDay,
//                                          int                   month,
//                                          int                   year,
//                                          const bdlt::Calendar& calendar)
//          // Return the date of the first business day at or after the
//          // specified 'targetDay' in the specified 'month' and 'year'
//          // according to the specified 'calendar', unless the resulting
//          // date would not fall within 'month', in which case return
//          // instead the date of the first business day before 'targetDay'
//          // in 'month'.  The behavior is undefined unless all candidate
//          // dates applied to 'calendar' are within its valid range and
//          // there exists at least one business day within 'month'.
//      {
//          BSLS_ASSERT(bdlt::Date::isValidYearMonthDay(year,
//                                                      month,
//                                                      targetDay));
//
//          // Efficiency is important so we will minimize the number of
//          // conversions between year/month/day and 'bdlt::Date' objects.
//
//          bdlt::Date date(year, month, targetDay);
//
//          if (0 == calendar.getNextBusinessDay(&date, date - 1)
//           && month == date.month()) {
//              return date;                                          // RETURN
//          }
//          while (calendar.isNonBusinessDay(--date)) {
//              // empty
//          }
//          return date;
//      }
//  };
//..
// Then, we create and populate two calendars, 'cal1' and 'cal2', for testing
// the 'modifiedFollowing' method:
//..
//  bdlt::Calendar cal1(bdlt::Date(2015, 1, 1), bdlt::Date(2015,12, 31));
//  cal1.addWeekendDay(bdlt::DayOfWeek::e_SUN);
//  cal1.addWeekendDay(bdlt::DayOfWeek::e_SAT);
//  cal1.addHoliday(bdlt::Date(2015, 7, 3));
//
//  bdlt::Calendar cal2(cal1);
//  cal2.addHoliday(bdlt::Date(2015, 7, 31));
//..
// Finally, we verify the 'modifiedFollowing' functionality:
//..
//  assert(bdlt::Date(2015, 7,  2) ==
//                       MyCalendarUtil::modifiedFollowing( 2, 7, 2015, cal1));
//  assert(bdlt::Date(2015, 7,  6) ==
//                       MyCalendarUtil::modifiedFollowing( 3, 7, 2015, cal1));
//  assert(bdlt::Date(2015, 7, 31) ==
//                       MyCalendarUtil::modifiedFollowing(31, 7, 2015, cal1));
//
//  assert(bdlt::Date(2015, 7,  2) ==
//                       MyCalendarUtil::modifiedFollowing( 2, 7, 2015, cal2));
//  assert(bdlt::Date(2015, 7,  6) ==
//                       MyCalendarUtil::modifiedFollowing( 3, 7, 2015, cal2));
//  assert(bdlt::Date(2015, 7, 30) ==
//                       MyCalendarUtil::modifiedFollowing(31, 7, 2015, cal2));
//..

#include <bdlscm_version.h>

#include <bdlt_calendarreverseiteratoradapter.h>
#include <bdlt_date.h>
#include <bdlt_dayofweek.h>
#include <bdlt_dayofweekset.h>
#include <bdlt_packedcalendar.h>

#include <bdlc_bitarray.h>

#include <bslalg_swaputil.h>

#include <bslh_hash.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_integralconstant.h>

#include <bsls_assert.h>
#include <bsls_preconditions.h>
#include <bsls_review.h>

#include <bsl_iosfwd.h>
#include <bsl_iterator.h>

namespace BloombergLP {
namespace bdlt {

class Calendar_BusinessDayConstIter;

                             // ==============
                             // class Calendar
                             // ==============

class Calendar {
    // This class implements a runtime-efficient, value-semantic repository of
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

    // DATA
    PackedCalendar    d_packedCalendar;
                               // the packed calendar object, which contains
                               // the weekend day and holiday information

    bdlc::BitArray    d_nonBusinessDays;
                               // cache of non-business days; note that the end
                               // of the valid range is defined by
                               // 'd_packedCalendar.firstDate() + length() - 1'

    // FRIENDS
    friend bool operator==(const Calendar&, const Calendar&);
    friend bool operator!=(const Calendar&, const Calendar&);
    template <class HASHALG>
    friend void hashAppend(HASHALG& hashAlg, const Calendar&);

  private:
    // PRIVATE MANIPULATORS
    void synchronizeCache();
        // Synchronize this calendar's cache by first clearing the cache, then
        // repopulating it with the holiday and weekend information from this
        // calendar's 'd_packedCalendar'.  Note that this method is only
        // *exception*-*neutral*; exception safety and rollback must be
        // handled by the caller.

    // PRIVATE ACCESSORS
    bool isCacheSynchronized() const;
        // Return 'true' if this calendar's cache correctly represents the
        // holiday and weekend information stored in this calendar's
        // 'd_packedCalendar', and 'false' otherwise.

  public:
    // TYPES
    typedef Calendar_BusinessDayConstIter            BusinessDayConstIterator;

    typedef PackedCalendar::HolidayConstIterator     HolidayConstIterator;

    typedef PackedCalendar::HolidayCodeConstIterator HolidayCodeConstIterator;

    typedef CalendarReverseIteratorAdapter<BusinessDayConstIterator>
                                               BusinessDayConstReverseIterator;

    typedef PackedCalendar::HolidayConstReverseIterator
                                                   HolidayConstReverseIterator;

    typedef PackedCalendar::HolidayCodeConstReverseIterator
                                               HolidayCodeConstReverseIterator;

    typedef PackedCalendar::WeekendDaysTransition WeekendDaysTransition;

    typedef PackedCalendar::WeekendDaysTransitionConstIterator
                                            WeekendDaysTransitionConstIterator;

    typedef PackedCalendar::WeekendDaysTransitionConstReverseIterator
                                     WeekendDaysTransitionConstReverseIterator;

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
    explicit Calendar(bslma::Allocator *basicAllocator = 0);
        // Create an empty calendar, i.e., a calendar having an empty valid
        // range.  Optionally specify a 'basicAllocator' used to supply memory.
        // If 'basicAllocator' is 0, the currently installed default allocator
        // is used.

    Calendar(const Date&       firstDate,
             const Date&       lastDate,
             bslma::Allocator *basicAllocator = 0);
        // Create a calendar having a valid range from the specified
        // 'firstDate' through the specified 'lastDate'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'firstDate <= lastDate'.

    explicit Calendar(const bdlt::PackedCalendar&  packedCalendar,
                      bslma::Allocator            *basicAllocator = 0);
        // Create a calendar having the same value as the specified
        // 'packedCalendar'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    Calendar(const Calendar& original, bslma::Allocator *basicAllocator = 0);
        // Create a calendar having the value of the specified 'original'
        // calendar.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~Calendar();
        // Destroy this object.

    // MANIPULATORS
    Calendar& operator=(const Calendar&       rhs);
    Calendar& operator=(const PackedCalendar& rhs);
        // Assign to this calendar the value of the specified 'rhs' calendar,
        // and return a reference providing modifiable access to this calendar.
        // See {Iterator Invalidation} for information regarding which
        // iterators are affected by the use of these methods.

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
        // which iterators are affected by the use of this method.  Note that
        // this method may be called repeatedly with the same value for 'date'
        // to build up a set of holiday codes for that date.

    int addHolidayCodeIfInRange(const Date& date, int holidayCode);
        // Mark the specified 'date' as a holiday (i.e., a non-business day)
        // in this calendar and add the specified 'holidayCode' (if not
        // already present) to the set of codes associated with 'date', if
        // 'date' is within the valid range of this calendar.  Return 0 if
        // 'date' is in range, and a non-zero value otherwise.  If
        // 'holidayCode' is already a code for 'date', or if 'date' is not in
        // the valid range, this method has no effect.  See
        // {Iterator Invalidation} for information regarding which iterators
        // are affected by the use of this method.  Note that this method may,
        // at the expense of verifying 'isInRange(date)' for each invocation,
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

    void intersectBusinessDays(const Calendar&       other);
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

    void intersectNonBusinessDays(const Calendar&       other);
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

    void unionBusinessDays(const Calendar&       other);
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

    void unionNonBusinessDays(const Calendar&       other);
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

    void swap(Calendar& other);
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

    Date holiday(int index) const;
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
        // 'false' otherwise.  Note that the valid range for a 'Calendar' is
        // empty if its length is 0.

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

    const PackedCalendar& packedCalendar() const;
        // Return a reference providing non-modifiable access to the underlying
        // 'PackedCalendar', which represents the same (mathematical) value as
        // this calendar.

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

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE3.0

    Date getNextBusinessDay(const Date& initialDate) const;
        // Return the date of the first business day following the specified
        // 'initialDate'.  For dates outside of the range of this calendar,
        // only weekend days are considered non-business days.  The behavior
        // is undefined if every day of the week is a weekend day, or if the
        // resulting date would otherwise exceed the value
        // 'Date(9999, 12, 31)'.

    Date getNextBusinessDay(const Date& initialDate, int nth) const;
        // Return the date of the specified 'nth' business day following the
        // specified 'initialDate'.  For dates outside of the range of this
        // calendar, only weekend days are considered non-business days.  The
        // behavior is undefined unless 'initialDate' is within the valid
        // range and '1 <= nth' (or if every day of the week is a weekend day,
        // or if the resulting date would otherwise exceed the value
        // 'Date(9999, 12, 31)').

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE3.0

};

// FREE OPERATORS
bool operator==(const Calendar& lhs, const Calendar& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' calendars have the same
    // value, and 'false' otherwise.  Two calendars have the same value if they
    // have the same valid range (or are both empty), the same weekend days,
    // the same holidays, and each corresponding pair of holidays has the same
    // (ordered) set of associated holiday codes.

bool operator!=(const Calendar& lhs, const Calendar& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' calendars do not have the
    // same value, and 'false' otherwise.  Two calendars do not have the same
    // value if they do not have the same valid range (and are not both empty),
    // do not have the same weekend days, do not have the same holidays, or,
    // for at least one corresponding pair of holidays, do not have the same
    // (ordered) set of associated holiday codes.

bsl::ostream& operator<<(bsl::ostream& stream, const Calendar& calendar);
    // Write the value of the specified 'calendar' to the specified output
    // 'stream', and return a reference to the modifiable 'stream'.

// FREE FUNCTIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const Calendar& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'Calendar'.

void swap(Calendar& a, Calendar& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee if the two objects were
    // created with the same allocator and the basic guarantee otherwise.

                    // ===================================
                    // class Calendar_BusinessDayConstIter
                    // ===================================

class Calendar_BusinessDayConstIter {
    // Provide read-only, sequential access in increasing (chronological) order
    // to the business days in a 'Calendar' object.

    // DATA
    const bdlc::BitArray *d_nonBusinessDays_p;  // pointer to the non-business
                                                // day bit array in the
                                                // calendar

    Date                  d_firstDate;          // first valid date of the
                                                // calendar

    int                   d_currentOffset;      // offset of the date to which
                                                // this iterator is pointing
                                                // (-1 indicates an 'end'
                                                // iterator)

    // FRIENDS
    friend class Calendar;
    friend bool operator==(const Calendar_BusinessDayConstIter&,
                           const Calendar_BusinessDayConstIter&);
    friend bool operator!=(const Calendar_BusinessDayConstIter&,
                           const Calendar_BusinessDayConstIter&);

  private:
    // PRIVATE CREATORS
    Calendar_BusinessDayConstIter(const bdlc::BitArray& nonBusinessDays,
                                  const Date&           firstDateOfCalendar,
                                  const Date&           startDate,
                                  bool                  endIterFlag);
        // Create a business day iterator for a calendar defined by the
        // specified 'nonBusinessDays' and 'firstDateOfCalendar'.  If the
        // specified 'endIterFlag' is 'false', then this iterator references
        // the first business day on or after the specified 'startDate';
        // otherwise, this iterator references one business day *past* the
        // first business day on or after 'startDate'.  If no business day
        // matching the above specification exists, then this iterator
        // references one day past the end of its range.  The behavior is
        // undefined unless 'startDate' is within the valid range of the
        // calendar defined by 'nonBusinessDays' and 'firstDateOfCalendar'.

  public:
    // PUBLIC TYPES
    typedef Date                     value_type;
    typedef int                      difference_type;
    typedef PackedCalendar_DateProxy pointer;
    typedef PackedCalendar_DateRef   reference;
        // The star operator returns a 'PackedCalendar_DateRef' *by* *value*.

    typedef bsl::bidirectional_iterator_tag iterator_category;

    // CREATORS
    Calendar_BusinessDayConstIter(
                                const Calendar_BusinessDayConstIter& original);
        // Create an iterator having the value of the specified 'original'
        // iterator.

    ~Calendar_BusinessDayConstIter();
        // Destroy this object.

    // MANIPULATORS
    Calendar_BusinessDayConstIter& operator=(
                                     const Calendar_BusinessDayConstIter& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator,
        // and return a reference providing modifiable access to this object.

    Calendar_BusinessDayConstIter& operator++();
        // Advance this iterator to refer to the next business day in the
        // associated calendar, and return a reference providing modifiable
        // access to this object.  The behavior is undefined unless, on entry,
        // this iterator references a valid business day.

    Calendar_BusinessDayConstIter& operator--();
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
bool operator==(const Calendar_BusinessDayConstIter& lhs,
                const Calendar_BusinessDayConstIter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators refer to the
    // same element in the same calendar, and 'false' otherwise.  The behavior
    // is undefined unless 'lhs' and 'rhs' both iterate over the same calendar.

bool operator!=(const Calendar_BusinessDayConstIter& lhs,
                const Calendar_BusinessDayConstIter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not refer to
    // the same element in the same calendar, and 'false' otherwise.  The
    // behavior is undefined unless 'lhs' and 'rhs' both iterate over the same
    // calendar.

Calendar_BusinessDayConstIter operator++(
                                 Calendar_BusinessDayConstIter& iterator, int);
    // Advance the specified 'iterator' to refer to the next business day in
    // the associated calendar, and return the previous value of 'iterator'.
    // The behavior is undefined unless, on entry, 'iterator' references a
    // valid business day.

Calendar_BusinessDayConstIter operator--(
                                 Calendar_BusinessDayConstIter& iterator, int);
    // Regress the specified 'iterator' to refer to the previous business day
    // in the associated calendar, and return the previous value of 'iterator'.
    // The behavior is undefined unless, on entry, 'iterator' references a
    // valid business day that is not the first business day for the associated
    // calendar.

// ============================================================================
//                          INLINE DEFINITIONS
// ============================================================================

                            // --------------
                            // class Calendar
                            // --------------

// CLASS METHODS

                                  // Aspects

inline
int Calendar::maxSupportedBdexVersion(int versionSelector)
{
    return PackedCalendar::maxSupportedBdexVersion(versionSelector);
}

// MANIPULATORS
inline
Calendar& Calendar::operator=(const Calendar& rhs)
{
    Calendar(rhs, d_packedCalendar.allocator()).swap(*this);
    return *this;
}

inline
Calendar& Calendar::operator=(const PackedCalendar& rhs)
{
    Calendar(rhs, d_packedCalendar.allocator()).swap(*this);
    return *this;
}

inline
int Calendar::addHolidayCodeIfInRange(const Date& date, int holidayCode)
{
    if (isInRange(date)) {
        addHolidayCode(date, holidayCode);
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
int Calendar::addHolidayIfInRange(const Date& date)
{
    if (isInRange(date)) {
        addHoliday(date);
        return 0;                                                     // RETURN
    }
    return -1;
}

inline
void Calendar::addWeekendDaysTransition(const Date&         startDate,
                                        const DayOfWeekSet& weekendDays)
{
    d_packedCalendar.addWeekendDaysTransition(startDate, weekendDays);
    synchronizeCache();
}

inline
void Calendar::intersectBusinessDays(const PackedCalendar& other)
{
    d_packedCalendar.intersectBusinessDays(other);
    synchronizeCache();
}

inline
void Calendar::intersectBusinessDays(const Calendar& other)
{
    intersectBusinessDays(other.d_packedCalendar);
}

inline
void Calendar::intersectNonBusinessDays(const PackedCalendar& other)
{
    d_packedCalendar.intersectNonBusinessDays(other);
    synchronizeCache();
}

inline
void Calendar::intersectNonBusinessDays(const Calendar& other)
{
    intersectNonBusinessDays(other.d_packedCalendar);
}

inline
void Calendar::removeAll()
{
    d_packedCalendar.removeAll();
    d_nonBusinessDays.removeAll();
}

inline
void Calendar::removeHoliday(const Date& date)
{
    d_packedCalendar.removeHoliday(date);

    if (true == isInRange(date) && false == isWeekendDay(date)) {
        d_nonBusinessDays.assign0(date - firstDate());
    }
}

inline
void Calendar::removeHolidayCode(const Date& date, int holidayCode)
{
    d_packedCalendar.removeHolidayCode(date, holidayCode);
}

inline
void Calendar::reserveHolidayCapacity(int numHolidays)
{
    d_packedCalendar.reserveHolidayCapacity(numHolidays);
}

inline
void Calendar::reserveHolidayCodeCapacity(int numHolidayCodes)
{
    d_packedCalendar.reserveHolidayCodeCapacity(numHolidayCodes);
}

inline
void Calendar::setValidRange(const Date& firstDate, const Date& lastDate)
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT(firstDate <= lastDate);
    BSLS_PRECONDITIONS_END();

    if (firstDate <= lastDate) {
        // For backwards compatibility, 'firstDate > lastDate' results in an
        // empty calendar (when asserts are not enabled).

        d_nonBusinessDays.reserveCapacity(lastDate - firstDate + 1);
    }

    d_packedCalendar.setValidRange(firstDate, lastDate);

    synchronizeCache();
}

inline
void Calendar::unionBusinessDays(const Calendar& other)
{
    unionBusinessDays(other.d_packedCalendar);
}

inline
void Calendar::unionNonBusinessDays(const Calendar& other)
{
    unionNonBusinessDays(other.d_packedCalendar);
}

                                  // Aspects

template <class STREAM>
STREAM& Calendar::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        PackedCalendar inCal(allocator());
        inCal.bdexStreamIn(stream, version);
        if (!stream) {
            return stream;                                            // RETURN
        }
        d_nonBusinessDays.reserveCapacity(inCal.length());
        d_packedCalendar.swap(inCal);
        synchronizeCache();
    }
    return stream;
}

inline
void Calendar::swap(Calendar& other)
{
    // 'swap' is undefined for objects with non-equal allocators.
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT(d_packedCalendar.allocator() ==
                                           other.d_packedCalendar.allocator());
    BSLS_PRECONDITIONS_END();
    bslalg::SwapUtil::swap(&d_packedCalendar,  &other.d_packedCalendar);
    bslalg::SwapUtil::swap(&d_nonBusinessDays, &other.d_nonBusinessDays);
}

// ACCESSORS
inline
Calendar::BusinessDayConstIterator Calendar::beginBusinessDays() const
{
    if (length()) {
        return Calendar_BusinessDayConstIter(d_nonBusinessDays,
                                             firstDate(),
                                             firstDate(),
                                             false);                  // RETURN
    }

    return endBusinessDays();
}

inline
Calendar::BusinessDayConstIterator
                            Calendar::beginBusinessDays(const Date& date) const
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT_SAFE(isInRange(date));
    BSLS_PRECONDITIONS_END();

    return Calendar_BusinessDayConstIter(d_nonBusinessDays,
                                         firstDate(),
                                         date,
                                         false);
}

inline
Calendar::HolidayCodeConstIterator
                            Calendar::beginHolidayCodes(const Date& date) const
{
    return d_packedCalendar.beginHolidayCodes(date);
}

inline
Calendar::HolidayCodeConstIterator
            Calendar::beginHolidayCodes(const HolidayConstIterator& iter) const
{
    return d_packedCalendar.beginHolidayCodes(iter);
}

inline
Calendar::HolidayConstIterator Calendar::beginHolidays() const
{
    return d_packedCalendar.beginHolidays();
}

inline
Calendar::HolidayConstIterator Calendar::beginHolidays(const Date& date) const
{
    return d_packedCalendar.beginHolidays(date);
}

inline
Calendar::WeekendDaysTransitionConstIterator
                                  Calendar::beginWeekendDaysTransitions() const
{
    return d_packedCalendar.beginWeekendDaysTransitions();
}

inline
Calendar::BusinessDayConstIterator Calendar::endBusinessDays() const
{
    if (length()) {
        return BusinessDayConstIterator(d_nonBusinessDays,
                                        firstDate(),
                                        lastDate(),
                                        true);                        // RETURN
    }
    return BusinessDayConstIterator(d_nonBusinessDays,
                                    Date() + 1,
                                    Date(),
                                    true);
}

inline
Calendar::BusinessDayConstIterator
                              Calendar::endBusinessDays(const Date& date) const
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT_SAFE(isInRange(date));
    BSLS_PRECONDITIONS_END();

    return BusinessDayConstIterator(d_nonBusinessDays,
                                    firstDate(),
                                    date,
                                    true);
}

inline
Calendar::HolidayCodeConstIterator
                              Calendar::endHolidayCodes(const Date& date) const
{
    return d_packedCalendar.endHolidayCodes(date);
}

inline
Calendar::HolidayCodeConstIterator
              Calendar::endHolidayCodes(const HolidayConstIterator& iter) const
{
    return d_packedCalendar.endHolidayCodes(iter);
}

inline
Calendar::HolidayConstIterator Calendar::endHolidays() const
{
    return d_packedCalendar.endHolidays();
}

inline
Calendar::HolidayConstIterator Calendar::endHolidays(const Date& date) const
{
    return d_packedCalendar.endHolidays(date);
}

inline
Calendar::WeekendDaysTransitionConstIterator
                                    Calendar::endWeekendDaysTransitions() const
{
    return d_packedCalendar.endWeekendDaysTransitions();
}

inline
const Date& Calendar::firstDate() const
{
    return d_packedCalendar.firstDate();
}

inline
int Calendar::getNextBusinessDay(Date *nextBusinessDay, const Date& date) const
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT_SAFE(nextBusinessDay);
    BSLS_ASSERT_SAFE(Date(9999, 12, 31) > date);
    BSLS_ASSERT_SAFE(isInRange(date + 1));
    BSLS_PRECONDITIONS_END();

    enum { e_SUCCESS = 0, e_FAILURE = 1 };

    int offset = static_cast<int>(
                    d_nonBusinessDays.find0AtMinIndex(date + 1 - firstDate()));
    if (0 <= offset) {
        *nextBusinessDay = firstDate() + offset;
        return e_SUCCESS;                                             // RETURN
    }

    return e_FAILURE;
}


inline
Date Calendar::holiday(int index) const
{
    return d_packedCalendar.holiday(index);
}

inline
int Calendar::holidayCode(const Date& date, int index) const
{
    return d_packedCalendar.holidayCode(date, index);
}

inline
bool Calendar::isBusinessDay(const Date& date) const
{
    return !isNonBusinessDay(date);
}

inline
bool Calendar::isHoliday(const Date& date) const
{
    return d_packedCalendar.isHoliday(date);
}

inline
bool Calendar::isInRange(const Date& date) const
{
    return d_packedCalendar.isInRange(date);
}

inline
bool Calendar::isNonBusinessDay(const Date& date) const
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT_SAFE(isInRange(date));
    BSLS_PRECONDITIONS_END();

    return d_nonBusinessDays[date - firstDate()];
}

inline
bool Calendar::isWeekendDay(const Date& date) const
{
    return d_packedCalendar.isWeekendDay(date);
}

inline
bool Calendar::isWeekendDay(DayOfWeek::Enum dayOfWeek) const
{
    return d_packedCalendar.isWeekendDay(dayOfWeek);
}

inline
const Date& Calendar::lastDate() const
{
    return d_packedCalendar.lastDate();
}

inline
int Calendar::length() const
{
    return static_cast<int>(d_nonBusinessDays.length());
}

inline
int Calendar::numBusinessDays() const
{
    return static_cast<int>(d_nonBusinessDays.num0());
}

inline
int Calendar::numBusinessDays(const Date& beginDate, const Date& endDate) const
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT_SAFE(isInRange(beginDate));
    BSLS_ASSERT_SAFE(isInRange(endDate));
    BSLS_ASSERT_SAFE(beginDate <= endDate);
    BSLS_PRECONDITIONS_END();

    return static_cast<int>(d_nonBusinessDays.num0(beginDate - firstDate(),
                                                   endDate - firstDate() + 1));
}

inline
int Calendar::numHolidayCodes(const Date& date) const
{
    return d_packedCalendar.numHolidayCodes(date);
}

inline
int Calendar::numHolidayCodesTotal() const
{
    return d_packedCalendar.numHolidayCodesTotal();
}

inline
int Calendar::numHolidays() const
{
    return d_packedCalendar.numHolidays();
}

inline
int Calendar::numNonBusinessDays() const
{
    return static_cast<int>(d_nonBusinessDays.num1());
}

inline
int Calendar::numWeekendDaysInRange() const
{
    return d_packedCalendar.numWeekendDaysInRange();
}

inline
int Calendar::numWeekendDaysTransitions() const
{
    return d_packedCalendar.numWeekendDaysTransitions();
}

inline
const PackedCalendar& Calendar::packedCalendar() const
{
    return d_packedCalendar;
}

inline
Calendar::BusinessDayConstReverseIterator Calendar::rbeginBusinessDays() const
{
    return BusinessDayConstReverseIterator(endBusinessDays());
}

inline
Calendar::BusinessDayConstReverseIterator
                           Calendar::rbeginBusinessDays(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return BusinessDayConstReverseIterator(endBusinessDays(date));
}

inline
Calendar::HolidayCodeConstReverseIterator
                           Calendar::rbeginHolidayCodes(const Date& date) const
{
    return d_packedCalendar.rbeginHolidayCodes(date);
}

inline
Calendar::HolidayCodeConstReverseIterator
           Calendar::rbeginHolidayCodes(const HolidayConstIterator& iter) const
{
    return d_packedCalendar.rbeginHolidayCodes(iter);
}

inline
Calendar::HolidayConstReverseIterator Calendar::rbeginHolidays() const
{
    return d_packedCalendar.rbeginHolidays();
}

inline
Calendar::HolidayConstReverseIterator
                               Calendar::rbeginHolidays(const Date& date) const
{
    return d_packedCalendar.rbeginHolidays(date);
}

inline
Calendar::WeekendDaysTransitionConstReverseIterator
                                 Calendar::rbeginWeekendDaysTransitions() const
{
    return d_packedCalendar.rbeginWeekendDaysTransitions();
}

inline
Calendar::BusinessDayConstReverseIterator Calendar::rendBusinessDays() const
{
    return BusinessDayConstReverseIterator(beginBusinessDays());
}

inline
Calendar::BusinessDayConstReverseIterator
                             Calendar::rendBusinessDays(const Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return BusinessDayConstReverseIterator(beginBusinessDays(date));
}

inline
Calendar::HolidayCodeConstReverseIterator
                             Calendar::rendHolidayCodes(const Date& date) const
{
    return d_packedCalendar.rendHolidayCodes(date);
}

inline
Calendar::HolidayCodeConstReverseIterator
             Calendar::rendHolidayCodes(const HolidayConstIterator& iter) const
{
    return d_packedCalendar.rendHolidayCodes(iter);
}

inline
Calendar::HolidayConstReverseIterator Calendar::rendHolidays() const
{
    return d_packedCalendar.rendHolidays();
}

inline
Calendar::HolidayConstReverseIterator
                                 Calendar::rendHolidays(const Date& date) const
{
    return d_packedCalendar.rendHolidays(date);
}

inline
Calendar::WeekendDaysTransitionConstReverseIterator
                                   Calendar::rendWeekendDaysTransitions() const
{
    return d_packedCalendar.rendWeekendDaysTransitions();
}

inline
Calendar::WeekendDaysTransition
                               Calendar::weekendDaysTransition(int index) const
{
    return d_packedCalendar.weekendDaysTransition(index);
}

                                  // Aspects

inline
bslma::Allocator *Calendar::allocator() const
{
    return d_packedCalendar.allocator();
}

template <class STREAM>
inline
STREAM& Calendar::bdexStreamOut(STREAM& stream, int version) const
{

    d_packedCalendar.bdexStreamOut(stream, version);
    return stream;
}

inline
bsl::ostream& Calendar::print(bsl::ostream& stream,
                              int           level,
                              int           spacesPerLevel) const
{
    return d_packedCalendar.print(stream, level, spacesPerLevel);
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22

// DEPRECATED METHODS
inline
int Calendar::maxSupportedBdexVersion()
{
    return 1;
}

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const Calendar& lhs, const Calendar& rhs)
{
    return lhs.d_packedCalendar == rhs.d_packedCalendar;
}

inline
bool bdlt::operator!=(const Calendar& lhs, const Calendar& rhs)
{
    return lhs.d_packedCalendar != rhs.d_packedCalendar;
}

inline
bsl::ostream& bdlt::operator<<(bsl::ostream& stream, const Calendar& calendar)
{
    calendar.print(stream, 0, -1);
    return stream;
}

// FREE FUNCTIONS
template <class HASHALG>
inline
void bdlt::hashAppend(HASHALG& hashAlg, const Calendar& object)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlg, object.d_packedCalendar);
}

namespace bdlt {

                   // -----------------------------------
                   // class Calendar_BusinessDayConstIter
                   // -----------------------------------

// CREATORS
inline
Calendar_BusinessDayConstIter::Calendar_BusinessDayConstIter(
                                 const Calendar_BusinessDayConstIter& original)
: d_nonBusinessDays_p(original.d_nonBusinessDays_p)
, d_firstDate(original.d_firstDate)
, d_currentOffset(original.d_currentOffset)
{
}

inline
Calendar_BusinessDayConstIter::~Calendar_BusinessDayConstIter()
{
}

// MANIPULATORS
inline
Calendar_BusinessDayConstIter& Calendar_BusinessDayConstIter::operator=(
                                      const Calendar_BusinessDayConstIter& rhs)
{
    d_nonBusinessDays_p = rhs.d_nonBusinessDays_p;
    d_firstDate         = rhs.d_firstDate;
    d_currentOffset     = rhs.d_currentOffset;
    return *this;
}

inline
Calendar_BusinessDayConstIter& Calendar_BusinessDayConstIter::operator++()
{
    BSLS_ASSERT_SAFE(d_currentOffset >= 0);

    d_currentOffset = static_cast<int>(
                    d_nonBusinessDays_p->find0AtMinIndex(d_currentOffset + 1));
    return *this;
}

inline
Calendar_BusinessDayConstIter& Calendar_BusinessDayConstIter::operator--()
{
    if (-1 == d_currentOffset) {
        d_currentOffset = static_cast<int>(d_nonBusinessDays_p->
                            find0AtMaxIndex(0, d_nonBusinessDays_p->length()));
    }
    else {
        d_currentOffset = static_cast<int>(d_nonBusinessDays_p->
                                          find0AtMaxIndex(0, d_currentOffset));
    }

    BSLS_ASSERT_SAFE(0 <= d_currentOffset);

    return *this;
}

inline
Calendar_BusinessDayConstIter operator++(
                                  Calendar_BusinessDayConstIter& iterator, int)
{
    Calendar_BusinessDayConstIter tmp(iterator);
    ++iterator;
    return tmp;
}

inline
Calendar_BusinessDayConstIter operator--(
                                  Calendar_BusinessDayConstIter& iterator, int)
{
    Calendar_BusinessDayConstIter tmp(iterator);
    --iterator;
    return tmp;
}

// ACCESSORS
inline
PackedCalendar_DateRef Calendar_BusinessDayConstIter::operator*() const
{
    return PackedCalendar_DateRef(d_firstDate + d_currentOffset);
}

inline
PackedCalendar_DateProxy Calendar_BusinessDayConstIter::operator->() const
{
    return PackedCalendar_DateProxy(this->operator*());
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const Calendar_BusinessDayConstIter& lhs,
                      const Calendar_BusinessDayConstIter& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_nonBusinessDays_p == rhs.d_nonBusinessDays_p);

    return lhs.d_firstDate     == rhs.d_firstDate
        && lhs.d_currentOffset == rhs.d_currentOffset;
}

inline
bool bdlt::operator!=(const Calendar_BusinessDayConstIter& lhs,
                      const Calendar_BusinessDayConstIter& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_nonBusinessDays_p == rhs.d_nonBusinessDays_p);

    return lhs.d_firstDate     != rhs.d_firstDate
        || lhs.d_currentOffset != rhs.d_currentOffset;
}

}  // close enterprise namespace

// TRAITS
namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<bdlt::Calendar> : bsl::true_type {};

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
