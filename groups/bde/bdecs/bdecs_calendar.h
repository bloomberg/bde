// bdecs_calendar.h                                                   -*-C++-*-
#ifndef INCLUDED_BDECS_CALENDAR
#define INCLUDED_BDECS_CALENDAR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide fast repository for accessing weekend/holiday information.
//
//@CLASSES:
//  bdecs_Calendar: fast repository for accessing weekend/holiday information
//
//@SEE_ALSO: bdecs_packedcalendar
//
//@AUTHOR: John Lakos (jlakos), Guillaume Morin (gmorin1)
//
//@SEE_ALSO: bdecs_packedcalendar
//
//@DESCRIPTION: This component provides a fully value-semantic class,
// 'bdecs_Calendar', that represents weekend and holiday information over a
// *valid* *range* of dates.  A 'bdecs_Calendar' is an approximation to the
// same *mathematical* type and is capable of representing the same subset of
// *mathematical* values as a 'bdecs_PackedCalendar'.  A 'bdecs_Calendar'
// object (representing the same *mathematical* value) can be constructed
// directly from a reference to a non-modifiable 'bdecs_PackedCalendar' object,
// and a reference to a non-modifiable 'bdecs_PackedCalendar' is readily
// accessible from any 'bdes_Calendar' object.
//
// But unlike 'bdecs_PackedCalendar', which is optimized for spatial
// efficiency, 'bdecs_Calendar' is designed to be especially efficient at
// determining whether a given 'bdet_Date' value (within the valid range
// for a particular 'bdecs_Calendar' object) is a business day -- i.e., not a
// weekend day or holiday (see "Usage" below).  For example, the cost of
// determining whether a given 'bdet_Date' is a business day, as opposed to a
// weekend or holiday, consists of only a few constant-time operations,
// compared to a binary search in a 'bdecs_PackedCalendar' representing the
// same calendar value.
//
// Default-constructed calendars are empty, and have no valid range.  Calendars
// can also be constructed with an initial valid range, implying that all dates
// within that range are business days.  The 'setValidRange' and 'addDay'
// methods, respectively, initialize or extend the valid range of a calendar,
// and a suite of 'add' methods can be used to populate a calendar with weekend
// days and holidays.
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
// set of holiday codes associated with an arbitrary date in a 'bdecs_Calendar'
// (or the current holiday referred to by a 'HolidayConstIterator') can be
// obtained in increasing (numerical) order using an iterator identified by
// the nested 'HolidayCodeConstIterator' 'typedef' (see below).
//
// Calendars are *fully* value-semantic objects, and, as such, necessarily
// support all of the standard value-semantic operations, such as default
// construction, copy construction and assignment, equality comparison,
// and externalization ('bdex' streaming, in particular).  Calendars also
// support the notions of both union and intersection merging operations,
// whereby a calendar can change its value to contain the union or intersection
// of its own contained weekend days, holidays, and holiday codes with those of
// another calendar object.  Such merging operations will, in general, also
// alter the valid range of the resulting calendar.  Note that merged calendars
// can be significantly more efficient for certain repeated
// "is-common-business-day" determinations among two or more calendars.
//
///Weekend Days and Weekend-Days Transitions
///-----------------------------------------
// A calendar maintain a set of dates considered to be weekend days.
// Typically, a calendar's weekend days falls on the same days of the week for
// the entire range of a calendar.  For example, the weekend for United States
// has consisted of Saturday and Sunday since the year 1940.  The
// 'addWeekendDay' and 'AddWeekendDays' methods can be used to specify the
// weekend days for these calendars.
//
// Sometimes, a calendar's weekend days changes over time.  For example,
// Bangladesh's weekend consists of Friday until June 1, 1997 when Bangladesh
// changed its weekends to be both Friday and Saturday.  Later, on October 1,
// 2001 Bangladesh reverted to a weekend of only Friday, until on September 9,
// 2009 Bangladesh again changed its weekends to be both Friday and Saturday.
//
// To optimize for space allocation while supporting both consistent and
// changing weekend days, a calendar represents weekend information using a
// sequence of weekend-days transitions, each of which comprises a date and a
// set of days of the week considered to the be the weekend on and after that
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
// The 'addWeekendDaysTransition' method adds a new weekend-days transition.
// The 'addWeekendDay' and 'addWeekendDays' methods create a weekend-days
// transition at January 1, 0001, if one doesn't already exist, and update the
// set of weekend days for that transition.  'addWeekendDay' and
// 'addWeekendDays' should be only used for calendars having a consistent set
// of weekend days throughout their entire range.  As such, using these methods
// together with 'addWeekendDaysTransition' is unspecified.
//
///Nested Iterators
///----------------
// Also provided are several STL-style 'const' bidirectional iterators
// accessible as nested 'typedef's.  'HolidayConstIterator',
// 'HolidayCodeConstIterator', and 'WeekendDaysTransitionConstIterator',
// respectively, iterate over a chronologically ordered sequence of holidays, a
// numerically ordered sequence of holiday codes, and a sequence of
// chronologically ordered weekend-days transitions.  As a general rule,
// calling a 'const' method will not invalidate any iterators, and calling a
// non-'const' method might invalidate any of them; it is, however, guaranteed
// that attempting to add *duplicate* holidays or holiday codes will have no
// effect, and therefore will not invalidate any iterators.  It is also
// guaranteed that adding a new code for an existing holiday will not
// invalidate any 'HolidayConstIterator'.
//
///Performance and Exception-Safety Guarantees
///-------------------------------------------
// 'bdecs_Calendar' supports 'O[1]' (i.e., constant-time) determination of
// whether a given 'bdet_Date' value is or is not a business day (or a
// holiday), which is accomplished by augmenting the implementation of a packed
// calendar with a supplementary cache.  The invariant that this cache and the
// data represented in the underlying 'bdecs_PackedCalendar' be maintained in a
// consistent state may add significantly to the cost of performing many
// manipulator operations, especially those that affect the calendar's valid
// range and add a new weekend-days transition.  Moreover, the cost of many of
// these operations will now be proportional to the length(s) of the valid
// range(s), as well as their respective numbers of holidays and associated
// holiday codes and weekend-days transitions.  Hence, when populating a
// calendar, it is recommended that the desired value be captured first as a
// 'bdecs_PackedCalendar', which can then be used efficiently to
// *value-construct* the desired 'bdecs_Calendar' object.  See the
// component-level doc for 'bdecs_packedcalendar' for its performance
// guarantees.
//
// All methods of the 'bdecs_Calendar' are exception-safe, but in general
// provide only the basic guarantee (i.e., no guarantee of rollback): If an
// exception occurs (i.e., while attempting to allocate memory), the calendar
// object will be left in a coherent state, but (unless otherwise specified)
// its *value* is undefined.
//
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
// Imagine we have a 'MyPackedCalendarCache' that, internally, is just a
// mapping from (typically short) character string names (such as "NYB",
// representing New York Bank settlement days) to 'bdecs_PackedCalendar'
// objects, containing densely packed calendar data:
//..
//  class MyPackedCalendarCache {
//      // This class maintains a space-efficient repository of calendar data
//      // associated with a (typically short) name.
//
//      // DATA
//      bsl::hash_map<bsl::string, bdecs_PackedCalendar *>  d_map;
//      bslma_Allocator                                    *d_allocator_p;
//
//     public:
//       // CREATORS
//       MyPackedCalendarCache(bslma_Allocator *basicAllocator = 0);
//          // Create an empty 'MyPackedCalendarCache'.  Optionally specify a
//          // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
//          // 0, the currently installed default allocator is used.
//
//       // ...
//
//       // ACCESSORS
//       const bdecs_PackedCalendar *lookup(const bsl::string& name) const;
//          // Return the address of calendar data associated with the
//          // specified 'name', or 0 if no such association exists.
//  };
//
//  MyPackedCalendarCache::MyPackedCalendarCache(
//                                            bslma_Allocator *basicAllocator)
//  : d_map(basicAllocator)
//  {
//  }
//
//  const bdecs_PackedCalendar *
//  MyPackedCalendarCache::lookup(const bsl::string& name) const
//  {
//      typedef bsl::hash_map<bsl::string, bdecs_PackedCalendar *> Cache;
//      Cache::const_iterator itr = d_map.find(name);
//
//      if (itr == d_map.end()) {
//          return 0;                                                 // RETURN
//      }
//      return itr->second;
//  }
//..
// Now imagine an application function, 'loadMyPackedCalendarCache', that takes
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
//  class MyCalendarCache {
//      // This class maintains a cache of runtime-efficient calendar objects
//      // created on demand from a compact packed-calendar-based data source,
//      // whose address is supplied at construction.
//
//      // DATA
//      MyPackedCalendarCache                      *d_datasource_p;
//      bsl::hash_map<bsl::string, bdecs_Calendar>  d_map;
//
//    public:
//      // CREATORS
//      MyCalendarCache(MyPackedCalendarCache *dataSource,
//                      bslma_Allocator       *basicAllocator = 0);
//         // Create an empty 'MyCalendarCache' associated with the specified
//         // 'dataSource'.  Optionally specify a 'basicAllocator' used to
//         // supply memory.  If 'basicAllocator' is 0, the currently installed
//         // default allocator is used.
//
//      // ...
//
//      // MANIPULATORS
//      const bdecs_Calendar *lookup(const bsl::string& name);
//         // Return the address of calendar data associated with the
//         // specified name, or 0 if no such association exists in
//         // the data source whose address was supplied at construction.
//         // Note that this method may alter the physical state of this
//         // object (and is therefore deliberately declared non-'const').
//
//      // ...
//
//  };
//
//  MyCalendarCache::MyCalendarCache(MyPackedCalendarCache *dataSource,
//                                   bslma_Allocator       *basicAllocator)
//  : d_datasource_p(dataSource)
//  , d_map(basicAllocator)
//  {
//  }
//
//  const bdecs_Calendar *MyCalendarCache::lookup(const bsl::string& name)
//  {
//      if (d_map.find(name) != d_map.end()) {
//          const bdecs_PackedCalendar *pc = d_datasource_p->lookup(name);
//          if (!pc) {
//
//              // No such name in the data source.
//
//              return 0;                                             // RETURN
//          }
//
//          // Create new entry in calendar cache.
//
//          d_map.insert(bsl::make_pair(name, *pc));
//      }
//
//      // An efficient calendar either already existed or was created.
//
//      return &d_map[name];
//   }
//..
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
//  struct MyCalendarUtil {
//
//      static bdet_Date modifiedFollowing(int                   targetDay,
//                                         int                   month,
//                                         int                   year,
//                                         const bdecs_Calendar& calendar)
//          // Return the date of the first business day at or after the
//          // specified 'targetDay' in the specified 'month' and 'year'
//          // according to the specified 'calendar', unless the resulting
//          // date would not fall within 'month', in which case return
//          // instead the date of the first business day before 'targetDay'
//          // in 'month'.  The behavior is undefined unless all candidate
//          // dates applied to 'calendar' are within its valid range and
//          // there exists at least one business day within 'month'.
//      {
//          BSLS_ASSERT(bdet_Date::isValid(year, month, targetDay));
//
//          // Efficiency is important so we will avoid converting to and
//          // from year/month/day to 'bdet_date' objects more than we must.
//
//          bdet_Date date(year, month, targetDay);
//          bdet_Date date2(date);
//
//          while (calendar.isNonBusinessDay(date)) {
//              ++date;
//          }
//          if (month == date.month()) {
//              return date;                                      // RETURN
//          }
//          do {
//              --date2;
//          } while (calendar.isNonBusinessDay(date2));
//          return date2;
//      }
// };
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDECS_PACKEDCALENDAR
#include <bdecs_packedcalendar.h>
#endif

#ifndef INCLUDED_BDEA_BITARRAY
#include <bdea_bitarray.h>
#endif

#ifndef INCLUDED_BDET_DATE
#include <bdet_date.h>
#endif

#ifndef INCLUDED_BDET_DAYOFWEEK
#include <bdet_dayofweek.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITUSESBSLMAALLOCATOR
#include <bslalg_typetraitusesbslmaallocator.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSLFWD_BSLMA_ALLOCATOR
#include <bslfwd_bslma_allocator.h>
#endif

namespace BloombergLP {

class bdec_DayOfWeekSet;
class bdecs_Calendar_BusinessDayConstIter;

                           // ====================
                           // class bdecs_Calendar
                           // ====================

class bdecs_Calendar {
    // This class implements a runtime-efficient, fully value-semantic
    // repository of weekend and holiday information over a *valid* *range* of
    // dates.  This valid range, '[ firstDate() .. lastDate() ]', spans the
    // first and last dates of a calendar's accessible contents.  A calendar
    // can be "populated" with weekend and holiday information via a suite of
    // 'add' methods.  Any subset of days of the week may be specified as
    // weekend (i.e., recurring non-business) days starting from a specified
    // date by adding a weekend-days transition; holidays within the valid
    // range are specified individually.  When adding a holiday, an arbitrary
    // integer "holiday code" may be associated with that date.  Additional
    // holiday codes for that date may subsequently be added.  Both the
    // holidays and the set of unique holiday codes associated with each
    // holiday date are maintained (internally) in order of increasing value.
    // Note that the behavior of requesting *any* calendar information for a
    // supplied date whose value is outside the current *valid* *range* for
    // that calendar (unless otherwise noted, e.g., 'isWeekendDay') is
    // undefined.
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  (A precise
    // operational definition of when two objects have the same value can be
    // found in the description of the homogeneous (free) 'operator==' for this
    // class.)  This class is *exception* *safe*, but provides no general
    // guarantee of rollback: If an exception is thrown during the invocation
    // of a method on a pre-existing object, the object will be left in a
    // coherent state, but (unless otherwise stated) its value is not defined.
    // In no event is memory leaked.  Finally, *aliasing* (e.g., using all or
    // part of an object as both source and destination) for the same
    // operation is supported in all cases.

    // DATA
    bdecs_PackedCalendar  d_packedCalendar;   // the packed calendar object,
                                              // which contains the weekend day
                                              // and holiday information

    bdea_BitArray         d_nonBusinessDays;  // cache of non-business days;
                                              // note that the end of the valid
                                              // range is defined by
                                              // 'd_startDate + length() - 1'

    bslma_Allocator      *d_allocator_p;      // memory allocator (not owned)

    // FRIENDS
    friend bool operator==(const bdecs_Calendar&, const bdecs_Calendar&);
    friend bool operator!=(const bdecs_Calendar&, const bdecs_Calendar&);

  private:
    // PRIVATE MANIPULATORS
    void synchronizeCache();
        // Synchronize this calendar's cache by first clearing the cache, then
        // repopulating it with holiday and weekend information.  Note that
        // this method is only *exception* *neutral*; exception safety and
        // rollback must be handled by the caller.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdecs_Calendar,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // PUBLIC TYPES
    typedef bdecs_Calendar_BusinessDayConstIter
                                               BusinessDayConstIterator;

    typedef bdecs_PackedCalendar::HolidayConstIterator
                                               HolidayConstIterator;

    typedef bdecs_PackedCalendar::HolidayCodeConstIterator
                                               HolidayCodeConstIterator;

    typedef bsl::reverse_iterator<BusinessDayConstIterator>
                                               BusinessDayConstReverseIterator;

    typedef bdecs_PackedCalendar::HolidayConstReverseIterator
                                               HolidayConstReverseIterator;

    typedef bdecs_PackedCalendar::HolidayCodeConstReverseIterator
                                               HolidayCodeConstReverseIterator;

    typedef bdecs_PackedCalendar::WeekendDaysTransition
                                            WeekendDaysTransition;

    typedef bdecs_PackedCalendar::WeekendDaysTransitionConstIterator
                                            WeekendDaysTransitionConstIterator;

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    typedef BusinessDayConstIterator        BusinessDayIterator;
    typedef HolidayConstIterator            HolidayIterator;
    typedef HolidayCodeConstIterator        HolidayCodeIterator;

    typedef BusinessDayConstReverseIterator BusinessDayReverseIterator;
    typedef HolidayConstReverseIterator     HolidayReverseIterator;
    typedef HolidayCodeConstReverseIterator HolidayCodeReverseIterator;
#endif

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // CREATORS
    explicit bdecs_Calendar(bslma_Allocator *basicAllocator = 0);
        // Create an empty calendar having no valid range.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    bdecs_Calendar(const bdet_Date&  firstDate,
                   const bdet_Date&  lastDate,
                   bslma_Allocator  *basicAllocator = 0);
        // Create a calendar having a valid range from the specified
        // 'firstDate' through the specified 'lastDate' if
        // 'firstDate <= lastDate'; otherwise, make the valid range empty.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    explicit bdecs_Calendar(const bdecs_PackedCalendar&  packedCalendar,
                            bslma_Allocator             *basicAllocator = 0);
        // Create a calendar having the same value as the specified
        // 'packedCalendar'.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    bdecs_Calendar(const bdecs_Calendar&  original,
                   bslma_Allocator       *basicAllocator = 0);
        // Create a calendar having the value of the specified 'original'
        // calendar.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~bdecs_Calendar();
        // Destroy this object.

    // MANIPULATORS
    bdecs_Calendar& operator=(const bdecs_Calendar& rhs);
    bdecs_Calendar& operator=(const bdecs_PackedCalendar& rhs);
        // Assign to this calendar the value of the specified 'rhs' calendar,
        // and return a reference to this modifiable calendar.

    void swap(bdecs_Calendar& other);
        // Swap the value of this object with the value of the specified
        // 'other' object.  This method provides the no-throw guarantee.  The
        // behavior is undefined if the two objects being swapped have
        // non-equal allocators.

    void addDay(const bdet_Date& date);
        // Extend the valid range (if necessary) to include the specified
        // 'date' value.

    void addHoliday(const bdet_Date& date);
        // Mark the specified 'date' as a holiday (i.e., a non-business day).
        // Extend the valid range of this calendar if necessary.  Note that
        // this method has no effect if 'date' is already marked as a holiday.

    void addHolidayCode(const bdet_Date& date, int holidayCode);
        // Mark the specified 'date' as a holiday (i.e., a non-business day)
        // and add the specified 'holidayCode' (if not already present) to the
        // ordered set of codes associated with 'date'.  Extend the valid
        // range of this calendar if necessary.  If 'holidayCode' is already a
        // code for 'date', this method has no effect (i.e., it will neither
        // change the state of this object nor invalidate any iterators).  If
        // 'date' is already marked as a holiday, this method will not
        // invalidate any 'HolidayConstIterator' or 'BusinessDayConstIterator'
        // iterators.

    void addWeekendDay(bdet_DayOfWeek::Day weekendDay);
        // Add the specified 'weekendDay' to the set of weekend days associated
        // with the weekend-days transition at January 1, 0001 maintained by
        // this calendar.  Create a transition at January 1, 0001 if one does
        // not exist.  The behavior is undefined if weekend-days transitions
        // were added to this calendar via the 'addWeekendDaysTransition'
        // method.

    void addWeekendDays(const bdec_DayOfWeekSet& weekendDays);
        // Add the specified 'weekendDays' to the set of weekend days
        // associated with the weekend-days transition at January 1, 0001
        // maintained by this calendar.  Create a transition at January 1, 0001
        // if one does not exist.  The behavior is undefined if weekend-days
        // transitions were added to this calendar via the
        // 'addWeekendDaysTransition' method.

    void addWeekendDaysTransition(const bdet_Date&         date,
                                  const bdec_DayOfWeekSet& weekendDays);
        // Add to this calendar a weekend-days transition on the specified
        // 'date' having the specified 'weekendDays' set.  If a weekend-days
        // transition already exists on 'date', replace the set of weekend days
        // of that transition with 'weekendDays'.  The behavior is undefined if
        // weekend days have been added to this calendar via either the
        // 'addWeekendDay' method or the 'addWeekendDays' method.

    void intersectBusinessDays(const bdecs_Calendar& other);
    void intersectBusinessDays(const bdecs_PackedCalendar& other);
        // Merge the specified 'other' calendar into this calendar such that
        // the valid range of this calendar will become the *intersection* of
        // the two calendars' ranges, and the weekend days and holidays for
        // this calendar become the union of those (non-business) days from
        // the two calendars -- i.e., the valid business days of this calendar
        // will become the intersection of those of the two original calendar
        // values.  For each holiday that remains, the resulting holiday codes
        // in this calendar will be the union of the corresponding original
        // holiday codes.

    void intersectNonBusinessDays(const bdecs_Calendar& other);
    void intersectNonBusinessDays(const bdecs_PackedCalendar& other);
        // Merge the specified 'other' calendar into this calendar such that
        // the valid range of this calendar will become the *intersection* of
        // the two calendars' ranges, and the weekend days and holidays for
        // this calendar are the intersection of those (non-business) days from
        // the two calendars -- i.e., the valid business days of this calendar
        // will become the union of those of the two original calendars, over
        // the *intersection* of their ranges.  For each holiday that remains,
        // the resulting holiday codes in this calendar will be the union of
        // the corresponding original holiday codes.

    void unionBusinessDays(const bdecs_Calendar& other);
    void unionBusinessDays(const bdecs_PackedCalendar& other);
        // Merge the specified 'other' calendar into this calendar such that
        // the valid range of this calendar will become the *union* of the two
        // calendars' ranges (or the minimal continuous range spanning the two
        // ranges, if the ranges are non-overlapping), and the weekend days
        // and holidays for this calendar are the intersection of those
        // (non-business) days from the two calendars -- i.e., the valid
        // business days of this calendar will become the union of those of the
        // two original calendar values.  For each holiday that remains, the
        // resulting holiday codes in this calendar will be the union of the
        // corresponding original holiday codes.

    void unionNonBusinessDays(const bdecs_Calendar& other);
    void unionNonBusinessDays(const bdecs_PackedCalendar& other);
        // Merge the specified 'other' calendar into this calendar such that
        // the valid range of this calendar will become the *union* of the two
        // calendars' ranges (or the minimal continuous range spanning the two
        // ranges, if the ranges are non-overlapping), and the weekend days
        // and holidays for this calendar will become the union of those
        // (non-business) days from the two calendars -- i.e., the valid
        // business days of this calendar will become the intersection of
        // those of the two calendars after each range is extended to cover
        // the resulting one.  For each holiday in either calendar, the
        // resulting holiday codes in this calendar will be the union of the
        // corresponding original holiday codes.

    void removeHoliday(const bdet_Date& date);
        // Remove from this calendar the holiday having the specified 'date' if
        // such a holiday exists.  Note that this operation has no effect if
        // 'date' is not a holiday in this calendar even if it is out of range.

    void removeHolidayCode(const bdet_Date& date, int holidayCode);
        // Remove from this calendar the specified 'holidayCode' for the
        // holiday having the specified 'date' if such a holiday having
        // 'holidayCode' exists.  Note that this operation has no effect if
        // 'date' is not a holiday in this calendar even if it is out of range,
        // or if the holiday at 'date' does not have 'holidayCode' associated
        // with it.

    void removeAll();
        // Remove all information from this calendar, leaving it with its
        // default constructed "empty" value.

    void setValidRange(const bdet_Date& firstDate,
                       const bdet_Date& lastDate);
        // Set the valid range of this calendar to be from the specified
        // 'firstDate' to the specified 'lastDate', if 'firstDate <= lastDate';
        // otherwise, set the valid range to be empty.  Any holiday (along with
        // any associated holiday codes) outside the new range will be removed.

    void swap(bdecs_Calendar *other);
        // Swap the value of this object with the specified 'other' object.
        // The behavior is undefined unless 'other' and this object hold the
        // same allocator.  Note that this method is logically equivalent to
        //..
        //  bdecs_Calendar tmp(*this);
        //  *this  = *other;
        //  *other = tmp;
        //..
        // except that this method is guaranteed never to throw.
        //
        // DEPRECATED: Use the 'swap' member function taking a reference
        // argument or the 'swap' free function taking two arguments of type
        // 'bdecs_Calendar&'.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation or if 'version' is not supported, this object is
        // unaltered.  Note that no version is read from 'stream'.  See the
        // 'bdex' package-level documentation for more information on 'bdex'
        // streaming of value-semantic types and containers.

    // ACCESSORS
    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

    WeekendDaysTransitionConstIterator beginWeekendDaysTransitions() const;
        // Return an iterator providing non-modifiable access to the first
        // weekend-day transition in the chronological sequence of weekend-day
        // transitions maintained by this calendar.

    WeekendDaysTransitionConstIterator endWeekendDaysTransitions() const;
        // Return an iterator providing non-modifiable access to the
        // past-the-end weekend-day transition in the chronological sequence of
        // weekend-day transitions maintained by this calendar.

    int numWeekendDaysTransitions() const;
        // Return the number of weekend-days transitions maintained by this
        // calendar.

    BusinessDayConstIterator beginBusinessDays() const;
        // Return an iterator that refers to the first business day in this
        // calendar.  If this calendar has no valid business days, the returned
        // iterator has the same value as that returned by 'endBusinessDays'.

    BusinessDayConstIterator beginBusinessDays(const bdet_Date& date) const;
        // Return an iterator that refers to the first business day that occurs
        // on or after the specified 'date' in this calendar.  If this calendar
        // has no such business day, the returned iterator has the same value
        // as that returned by 'endBusinessDays'.  The behavior is undefined
        // unless 'date' is within the valid range of this calendar.

    HolidayCodeConstIterator
    beginHolidayCodes(const HolidayConstIterator& iter) const;
        // Return an iterator that refers to the first holiday code for the
        // holiday referenced by the specified 'iter'.  If there is no holiday
        // code associated with the date referenced by the specified 'iter',
        // the returned iterator has the same value as that returned by
        // 'endHolidayCodes(iter)'.  The behavior is undefined unless 'iter'
        // refers to a valid holiday of this calendar.

    HolidayCodeConstIterator beginHolidayCodes(const bdet_Date& date) const;
        // Return an iterator that refers to the first holiday code for the
        // specified 'date'.  If there is no holiday code associated with
        // 'date', the returned iterator has the same value as that returned by
        // 'endHolidayCodes(date)'.  The behavior is undefined unless 'date' is
        // within the valid range of this calendar.  Note that 'date' need not
        // be marked as a holiday in this calendar.

    HolidayConstIterator beginHolidays() const;
        // Return an iterator that refers to the first holiday in this
        // calendar.  If this calendar has no holidays, the returned iterator
        // has the same value as that returned by 'endHolidays'.

    HolidayConstIterator beginHolidays(const bdet_Date& date) const;
        // Return an iterator that refers to the first holiday that occurs on
        // or after the specified 'date' in this calendar.  If this calendar
        // has no such holiday, the returned iterator has the same value as
        // that returned by 'endHolidays'.  The behavior is undefined unless
        // 'date' is within the valid range of this calendar.

    BusinessDayConstIterator endBusinessDays() const;
        // Return an iterator that indicates the element one past the last
        // business day in this calendar.  If this calendar has no valid
        // business days, the returned iterator has the same value as that
        // returned by 'beginBusinessDays'.

    BusinessDayConstIterator endBusinessDays(const bdet_Date& date) const;
        // Return an iterator that indicates the element one past the first
        // business day that occurs on or before the specified 'date' in this
        // calendar.  If this calendar has no such business day, the returned
        // iterator has the same value as that returned by
        // 'beginBusinessDays'.  The behavior is undefined unless 'date' is
        // within the valid range of this calendar.

    HolidayCodeConstIterator
    endHolidayCodes(const HolidayConstIterator& iter) const;
        // Return an iterator that indicates the element one past the last
        // holiday code associated with the date referenced by the specified
        // 'iter'.  If there are no holiday codes associated with the date
        // referenced by 'iter', the returned iterator has the same value as
        // that returned by 'beginHolidayCodes(iter)'.  The behavior is
        // undefined unless 'iter' references a valid holiday in this calendar.

    HolidayCodeConstIterator endHolidayCodes(const bdet_Date& date) const;
        // Return an iterator that indicates the element one past the last
        // holiday code associated with the specified 'date'.  If there are no
        // holiday codes associated with 'date', the returned iterator has the
        // same value as that returned by 'beginHolidayCodes(date)'.  The
        // behavior is undefined unless 'date' is within the valid range of
        // this calendar.  Note that 'date' need not be marked as a holiday in
        // this calendar.

    HolidayConstIterator endHolidays() const;
        // Return an iterator that indicates the element one past the last
        // holiday in this calendar.  If this calendar has no holidays, the
        // returned iterator has the same value as that returned by
        // 'beginHolidays'.

    HolidayConstIterator endHolidays(const bdet_Date& date) const;
        // Return an iterator that indicates the element one past the first
        // holiday that occurs on or before the specified 'date' in this
        // calendar.  If this calendar has no such holiday, the returned
        // iterator has the same value as that returned by 'beginHolidays'.
        // The behavior is undefined unless 'date' is within the valid range of
        // this calendar.

    const bdet_Date& firstDate() const;
        // Return a reference to the non-modifiable earliest date in the
        // valid range of this calendar.  The behavior is undefined unless
        // the calendar is non-empty -- i.e., unless '1 <= length()'.

    bdet_Date getNextBusinessDay(const bdet_Date& initialDate) const;
        // Return the date of the first business day following the specified
        // 'initialDate'.  For dates outside of the range of this calendar,
        // only weekend days are considered non-business days.  The behavior
        // is undefined if every day of the week is a weekend day, or if the
        // resulting date would otherwise exceed the value
        // 'bdet_Date(9999, 12, 31)'.

    bdet_Date getNextBusinessDay(const bdet_Date& initialDate, int nth) const;
        // Return the date of the specified 'nth' business day following the
        // specified 'initialDate'.  For dates outside of the range of this
        // calendar, only weekend days are considered non-business days.  The
        // behavior is undefined unless 'initialDate' is within the valid
        // range and '1 <= nth' (or if every day of the week is a weekend day,
        // or if the resulting date would otherwise exceed the value
        // 'bdet_Date(9999, 12, 31)').

    bool isBusinessDay(const bdet_Date& date) const;
        // Return 'true' if the specified 'date' is a business day (i.e.,
        // not a holiday or weekend day), and 'false' otherwise.  The behavior
        // is undefined unless 'date' is within the valid range of this
        // calendar.

    bool isHoliday(const bdet_Date& date) const;
        // Return 'true' if the specified 'date' is a holiday in this calendar,
        // and 'false' otherwise.  The behavior is undefined unless 'date' is
        // within the valid range of this calendar.

    bool isInRange(const bdet_Date& date) const;
        // Return 'true' if the specified 'date' is within the valid range of
        // this calendar (i.e., 'firstDate() <= date <= lastDate()'), and
        // 'false' otherwise.

    bool isNonBusinessDay(const bdet_Date& date) const;
        // Return 'true' if the specified 'date' is not a business day (i.e.,
        // a holiday or weekend day), and 'false' otherwise.  The behavior is
        // undefined unless 'date' is within the valid range of this calendar.
        // Note that:
        //..
        //  !isBusinessday(date)
        //..
        // returns the same result, but calling this method may be faster.

    bool isWeekendDay(const bdet_Date& date) const;
        // Return 'true' if the specified 'date' falls on a day of the week
        // that is considered a weekend day in this calendar, and 'false'
        // otherwise.  Note that this method is defined for all 'bdet_Date'
        // values, not just those that fall within the valid range, and may
        // be invoked on even an empty calendar (i.e., having '0 == length()').

    bool isWeekendDay(bdet_DayOfWeek::Day dayOfWeek) const;
        // Return 'true' if the specified 'dayOfWeek' is a weekend day in this
        // calendar, and 'false' otherwise.  The behavior is undefined if
        // weekend-days transitions were added to this calendar via the
        // 'addWeekendDaysTransition' method.

    const bdet_Date& lastDate() const;
        // Return a reference to the non-modifiable latest date in the valid
        // range of this calendar.  The behavior is undefined unless the
        // calendar is non-empty -- i.e., unless '1 <= length()'.

    int length() const;
        // Return the number of days in the valid range of this calendar,
        // which is defined to be 0 if this calendar is empty, and
        // 'lastDate() - firstDate() + 1' otherwise.

    int numBusinessDays() const;
        // Return the number of days in the valid range of this calendar that
        // are considered business days -- i.e., are neither holidays nor
        // weekend days.  Note that
        // 'numBusinessDays() == length() - numNonBusinessDays()'.

    int numHolidayCodes(const bdet_Date& date) const;
        // Return the number of (unique) holiday codes associated with the
        // specified 'date' in this calendar.  The behavior is undefined unless
        // 'date' is within the valid range of this calendar.

    int numHolidays() const;
        // Return the number of days in the valid range of this calendar that
        // are individually marked as holidays, irrespective of whether or
        // not the date is also considered a weekend day.

    int numNonBusinessDays() const;
        // Return the number of days in the valid range of this calendar that
        // are *not* considered business days -- i.e., are either holidays,
        // weekend days, or both.  Note that
        // 'numNonBusinessDays() == length() - numNonBusinessDays()'.

    int numWeekendDaysInRange() const;
        // Return the number of days in the valid range of this calendar that
        // are considered weekend days, irrespective of any designated
        // holidays.

    const bdecs_PackedCalendar& packedCalendar() const;
        // Return a reference to the underlying non-modifiable
        // 'bdecs_PackedCalendar', which represents the same (mathematical)
        // value as this calendar.

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

    BusinessDayConstReverseIterator rbeginBusinessDays() const;
        // Return an iterator that refers to the last business day in this
        // calendar.  If this calendar has no valid business days, the returned
        // iterator has the same value as that returned by
        // 'rendBusinessDays'.

    BusinessDayConstReverseIterator
    rbeginBusinessDays(const bdet_Date& date) const;
        // Return an iterator that refers to the first business day that occurs
        // on or before the specified 'date' in this calendar.  If this
        // calendar has no such business day, the returned iterator has the
        // same value as that returned by 'rendBusinessDays'.  The behavior
        // is undefined unless 'date' is within the valid range of this
        // calendar.

    HolidayCodeConstReverseIterator
    rbeginHolidayCodes(const HolidayConstIterator& iter) const;
        // Return an iterator that refers to the last holiday code associated
        // with the holiday referenced by the specified 'iter'.  If there are
        // no holiday codes associated with the date referenced by 'iter', the
        // returned iterator has the same value as that returned by
        // 'rendHolidayCodes(iter)'.  The behavior is undefined unless 'iter'
        // refers to a valid holiday of this calendar.

    HolidayCodeConstReverseIterator
    rbeginHolidayCodes(const bdet_Date& date) const;
        // Return an iterator that refers to the last holiday code associated
        // with the specified 'date' in this calendar.  If there are no holiday
        // codes associated with 'date', the returned iterator has the same
        // value as that returned by 'rendHolidayCodes(date)'.  The behavior is
        // undefined unless 'date' is within the valid range of this calendar.
        // Note that 'date' need not be marked as a holiday in this calendar.

    HolidayConstReverseIterator rbeginHolidays() const;
        // Return an iterator that refers to the last holiday in this calendar.
        // If this calendar has no holidays, the returned iterator has the same
        // value as that returned by 'rendHolidays'.

    HolidayConstReverseIterator rbeginHolidays(const bdet_Date& date) const;
        // Return an iterator that refers to the first holiday that occurs on
        // or before the specified 'date' in this calendar.  If this calendar
        // has no such holiday, the returned iterator has the same value as
        // that returned by 'rendHolidays'.  The behavior is undefined unless
        // 'date' is within the valid range of this calendar.

    BusinessDayConstReverseIterator rendBusinessDays() const;
        // Return an iterator that indicates the element one before the first
        // business day in this calendar.  If this calendar has no valid
        // business days, the returned iterator has the same value as that
        // returned by 'rbeginBusinessDays'.

    BusinessDayConstReverseIterator
    rendBusinessDays(const bdet_Date& date) const;
        // Return an iterator that indicates the element one before the first
        // business day that occurs on or before the specified 'date' in this
        // calendar.  If this calendar has no such business day, the returned
        // iterator has the same value as that returned by
        // 'rbeginBusinessDays'.  The behavior is undefined unless 'date' is
        // within the valid range of this calendar.

    HolidayCodeConstReverseIterator
    rendHolidayCodes(const HolidayConstIterator& iter) const;
        // Return an iterator that indicates the element one before the first
        // holiday code associated with the holiday referenced by the specified
        // 'iter'.  If there are no holiday codes associated with the date
        // referenced by 'iter', the returned iterator has the same value as
        // that returned by 'rbeginHolidayCodes(iter)'.  The behavior is
        // undefined unless 'iter' references a valid holiday in this calendar.

    HolidayCodeConstReverseIterator
    rendHolidayCodes(const bdet_Date& date) const;
        // Return an iterator that indicates the element one before the first
        // holiday code associated with the specified 'date'.  If there are no
        // holiday codes associated with 'date', the returned iterator has the
        // same value as that returned by 'rbeginHolidayCodes(date)'.  The
        // behavior is undefined unless 'date' is within the valid range of
        // this calendar.  Note that 'date' need not be marked as a holiday in
        // this calendar.

    HolidayConstReverseIterator rendHolidays() const;
        // Return an iterator that indicates the element one before the first
        // holiday in this calendar.  If this calendar has no holidays, the
        // returned iterator has the same value as that returned by
        // 'rbeginHolidays'.

    HolidayConstReverseIterator rendHolidays(const bdet_Date& date) const;
        // Return an iterator that indicates the element one before the first
        // holiday that occurs on or before the specified 'date' in this
        // calendar.  If this calendar has no such holiday, the returned
        // iterator has the same value as that returned by 'rbeginHolidays'.
        // The behavior is undefined unless 'date' is within the valid range of
        // this calendar.
};

// FREE OPERATORS
bool operator==(const bdecs_Calendar& lhs, const bdecs_Calendar& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' calendars have the same
    // value, and 'false' otherwise.  Two calendars have the same value if they
    // have the same valid range (or are both empty), the same weekend days,
    // the same holidays, and each corresponding pair of holidays, has the same
    // (ordered) set of associated holiday codes.

bool operator!=(const bdecs_Calendar& lhs, const bdecs_Calendar& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' calendars do not have the
    // same value, and 'false' otherwise.  Two calendars do not have the same
    // value if they do not have the same valid range (and are not both empty),
    // do not have the same weekend days, do not have the same holidays, or,
    // for at least one corresponding pair of holidays, do not have the same
    // (ordered) set of associated holiday codes.

bsl::ostream& operator<<(bsl::ostream& stream, const bdecs_Calendar& calendar);
    // Write the value of the specified 'calendar' to the specified output
    // 'stream', and return a reference to the modifiable 'stream'.

// FREE FUNCTIONS
void swap(bdecs_Calendar& a, bdecs_Calendar& b);
    // Swap the values of the specified 'a' and 'b' objects.  This method
    // provides the no-throw guarantee.  The behavior is undefined if the two
    // objects being swapped have non-equal allocators.

                    // =========================================
                    // class bdecs_Calendar_BusinessDayConstIter
                    // =========================================

class bdecs_Calendar_BusinessDayConstIter {
    // Provide read-only, sequential access in increasing (chronological) order
    // to the business days in a 'bdecs_Calendar' object.

    // DATA
    const bdea_BitArray *d_nonBusinessDays_p;  // pointer to the non-business
                                               // day bit array in the calendar

    bdet_Date            d_firstDate;          // first valid date of the
                                               // calendar

    int                  d_currentOffset;      // offset of the date to which
                                               // this iterator is pointing (-1
                                               // indicates an 'end' iterator)

    // FRIENDS
    friend class bdecs_Calendar;
    friend bool operator==(const bdecs_Calendar_BusinessDayConstIter&,
                           const bdecs_Calendar_BusinessDayConstIter&);
    friend bool operator!=(const bdecs_Calendar_BusinessDayConstIter&,
                           const bdecs_Calendar_BusinessDayConstIter&);

  private:
    // PRIVATE CREATORS
    bdecs_Calendar_BusinessDayConstIter(
                                    const bdea_BitArray& nonBusinessDays,
                                    const bdet_Date&     firstDateOfCalendar,
                                    const bdet_Date&     startDate,
                                    bool                 endIterFlag);
        // Create a business day iterator for a calendar defined by the
        // specified 'nonBusinessDays' and 'firstDateOfCalendar'.  If the
        // specified 'endIterFlag' is 'false', then this iterator references
        // the first business day on or after the specified 'startDate';
        // otherwise, this iterator references one business day *past* the
        // first business day on or after 'startDate'.  If no business day
        // matching the above specification exists, then this iterator will
        // reference one day past the end of its range.  The behavior is
        // undefined unless 'startDate' is within the valid range of the
        // calendar defined by 'nonBusinessDays' and 'firstDateOfCalendar'.

  public:
    // PUBLIC TYPES
    typedef bdet_Date                              value_type;
    typedef int                                    difference_type;
    typedef bdecs_PackedCalendar_IteratorDateProxy pointer;
    typedef bdecs_PackedCalendar_DateRef           reference;
        // The star operator returns a 'bdecs_PackedCalendar_DateRef' *by*
        // *value*.

    typedef bsl::bidirectional_iterator_tag iterator_category;

    // CREATORS
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    bdecs_Calendar_BusinessDayConstIter();
        // Create an uninitialized iterator.
#endif

    bdecs_Calendar_BusinessDayConstIter(
                          const bdecs_Calendar_BusinessDayConstIter& original);
        // Create an iterator having the value of the specified 'original'
        // iterator.

    ~bdecs_Calendar_BusinessDayConstIter();
        // Destroy this object.

    // MANIPULATORS
    bdecs_Calendar_BusinessDayConstIter& operator=(
                               const bdecs_Calendar_BusinessDayConstIter& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator,
        // and return a reference to this modifiable iterator.

    bdecs_Calendar_BusinessDayConstIter& operator++();
        // Advance this iterator to refer to the next business day in the
        // calendar and return a reference to this modifiable object.  The
        // behavior is undefined unless, on entry, this iterator references a
        // valid business day.

    bdecs_Calendar_BusinessDayConstIter& operator--();
        // Regress this iterator to refer to the previous business day in the
        // calendar, and return a reference to this modifiable object.  The
        // behavior is undefined unless, on entry, this iterator references a
        // valid business day.

    // ACCESSORS
    bdecs_PackedCalendar_DateRef operator*() const;
        // Return a 'bdecs_PackedCalendar_DateRef' object that contains the
        // date value of the business day referenced by this iterator.

    bdecs_PackedCalendar_IteratorDateProxy operator->() const;
        // Return a date value proxy for the business day referenced by this
        // iterator.
};

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
typedef bdecs_Calendar_BusinessDayConstIter bdecs_CalendarBusinessDayIter;
#endif

// FREE OPERATORS
bool operator==(const bdecs_Calendar_BusinessDayConstIter& lhs,
                const bdecs_Calendar_BusinessDayConstIter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators point to the
    // same entry in the same calendar, and 'false' otherwise.  The behavior is
    // undefined unless 'lhs' and 'rhs' both iterate over the same calendar.

bool operator!=(const bdecs_Calendar_BusinessDayConstIter& lhs,
                const bdecs_Calendar_BusinessDayConstIter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not point to
    // the same entry in the same calendar, and 'false' otherwise.  The
    // behavior is undefined unless 'lhs' and 'rhs' both iterate over the same
    // calendar.

bdecs_Calendar_BusinessDayConstIter operator++(
                           bdecs_Calendar_BusinessDayConstIter& iterator, int);
    // Advance the specified 'iterator' to refer to the next business day in
    // the calendar, and return the previous value of 'iterator'.  The behavior
    // is undefined unless, on entry, 'iterator' references a valid business
    // day.

bdecs_Calendar_BusinessDayConstIter operator--(
                           bdecs_Calendar_BusinessDayConstIter& iterator, int);
    // Regress the specified 'iterator' to refer to the previous business day
    // in the calendar, and return the previous value of 'iterator'.  The
    // behavior is undefined unless, on entry, 'iterator' references a valid
    // business day.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // --------------------
                          // class bdecs_Calendar
                          // --------------------

                            // -----------------
                            // Level-0 Functions
                            // -----------------

// ACCESSORS
inline
bool bdecs_Calendar::isInRange(const bdet_Date& date) const
{
    return d_packedCalendar.isInRange(date);
}

// CLASS METHODS
inline
int bdecs_Calendar::maxSupportedBdexVersion()
{
    return bdecs_PackedCalendar::maxSupportedBdexVersion();
}

// MANIPULATORS
inline
void bdecs_Calendar::intersectBusinessDays(const bdecs_PackedCalendar& other)
{
    d_packedCalendar.intersectBusinessDays(other);
    synchronizeCache();
}

inline
void bdecs_Calendar::intersectBusinessDays(const bdecs_Calendar& other)
{
    intersectBusinessDays(other.d_packedCalendar);
}

inline
void
bdecs_Calendar::intersectNonBusinessDays(const bdecs_PackedCalendar& other)
{
    d_packedCalendar.intersectNonBusinessDays(other);
    synchronizeCache();
}

inline
void bdecs_Calendar::intersectNonBusinessDays(const bdecs_Calendar& other)
{
    intersectNonBusinessDays(other.d_packedCalendar);
}

inline
void bdecs_Calendar::unionBusinessDays(const bdecs_Calendar& other)
{
    unionBusinessDays(other.d_packedCalendar);
}

inline
void bdecs_Calendar::unionNonBusinessDays(const bdecs_Calendar& other)
{
    unionNonBusinessDays(other.d_packedCalendar);
}

inline
void bdecs_Calendar::removeAll()
{
    d_packedCalendar.removeAll();
    d_nonBusinessDays.removeAll();
}

inline
void bdecs_Calendar::removeHolidayCode(const bdet_Date& date,
                                       int              holidayCode)
{
    d_packedCalendar.removeHolidayCode(date, holidayCode);
}

template <class STREAM>
STREAM& bdecs_Calendar::bdexStreamIn(STREAM& stream, int version)
{

    // The 'bdecs_Calendar' delegates its streaming operations (and the format
    // version) to 'bdecs_PackedCalendar' as both types can represent the exact
    // same set of mathematical values.

    if (stream) {

        // Check whether 'version' is less than the maximum supported version.
        // The version is hard-coded here as 2, because
        // 'maxSupportedBdexVersion()' is kept at 1 for
        // backwards-compatibility.  We should change the following condition
        // to compare with 'maxSupportedBdexVersion()' once the class method is
        // updated to return 2.

        if (version <= 2) {

            bdecs_PackedCalendar inCal(d_allocator_p);
            inCal.bdexStreamIn(stream, version);
            if (!stream) {
                return stream;
            }
            d_nonBusinessDays.reserveCapacity(inCal.length());

            // At this point, no exceptions can be thrown.

            d_packedCalendar.swap(inCal);
            synchronizeCache();
        }
        else {
            stream.invalidate();
        }
    }
    return stream;
}

// ACCESSORS
template <class STREAM>
STREAM& bdecs_Calendar::bdexStreamOut(STREAM& stream, int version) const
{

    // The 'bdecs_Calendar' delegates its streaming operations (and the format
    // version) to 'bdecs_PackedCalendar' as both types can represent the exact
    // same set of mathematical values.

    // Check whether 'version' is less than the maximum supported version.  The
    // version is hard-coded here as 2, because 'maxSupportedBdexVersion()' is
    // kept at 1 for backwards-compatibility.  We should change the following
    // condition to compare with 'maxSupportedBdexVersion()' once the class
    // method is updated to return 2.

    if (version <= 2) {
        d_packedCalendar.bdexStreamOut(stream, version);
    }
    else {
        stream.invalidate();
    }
    return stream;
}


inline
bdecs_Calendar::WeekendDaysTransitionConstIterator
bdecs_Calendar::beginWeekendDaysTransitions() const
{
    return d_packedCalendar.beginWeekendDaysTransitions();
}

inline
bdecs_Calendar::WeekendDaysTransitionConstIterator
bdecs_Calendar::endWeekendDaysTransitions() const
{
    return d_packedCalendar.endWeekendDaysTransitions();
}

inline
int bdecs_Calendar::numWeekendDaysTransitions() const
{
    return d_packedCalendar.numWeekendDaysTransitions();
}

inline
bdecs_Calendar::BusinessDayConstIterator
bdecs_Calendar::beginBusinessDays(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return bdecs_Calendar_BusinessDayConstIter(d_nonBusinessDays,
                                               firstDate(),
                                               date,
                                               false);
}

inline
bdecs_Calendar::HolidayCodeConstIterator
bdecs_Calendar::beginHolidayCodes(const HolidayConstIterator& iter) const
{
    return d_packedCalendar.beginHolidayCodes(iter);
}

inline
bdecs_Calendar::HolidayCodeConstIterator
bdecs_Calendar::beginHolidayCodes(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return d_packedCalendar.beginHolidayCodes(date);
}

inline
bdecs_Calendar::HolidayConstIterator bdecs_Calendar::beginHolidays() const
{
    return d_packedCalendar.beginHolidays();
}

inline
bdecs_Calendar::HolidayConstIterator
bdecs_Calendar::beginHolidays(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return d_packedCalendar.beginHolidays(date);
}

inline
bdecs_Calendar::BusinessDayConstIterator
bdecs_Calendar::endBusinessDays() const
{
    return BusinessDayConstIterator(d_nonBusinessDays, firstDate(),
                                    lastDate(), true);
}

inline
bdecs_Calendar::BusinessDayConstIterator
bdecs_Calendar::endBusinessDays(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return BusinessDayConstIterator(d_nonBusinessDays, firstDate(),
                                    date, true);
}

inline
bdecs_Calendar::HolidayCodeConstIterator
bdecs_Calendar::endHolidayCodes(const HolidayConstIterator& iter) const
{
    return d_packedCalendar.endHolidayCodes(iter);
}

inline
bdecs_Calendar::HolidayCodeConstIterator
bdecs_Calendar::endHolidayCodes(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return d_packedCalendar.endHolidayCodes(date);
}

inline
bdecs_Calendar::HolidayConstIterator bdecs_Calendar::endHolidays() const
{
    return d_packedCalendar.endHolidays();
}

inline
bdecs_Calendar::HolidayConstIterator
bdecs_Calendar::endHolidays(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return d_packedCalendar.endHolidays(date);
}

inline
const bdet_Date& bdecs_Calendar::firstDate() const
{
    return d_packedCalendar.firstDate();
}

inline
bool bdecs_Calendar::isBusinessDay(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return !isNonBusinessDay(date);
}

inline
bool bdecs_Calendar::isHoliday(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return d_packedCalendar.isHoliday(date);
}

inline
bool bdecs_Calendar::isNonBusinessDay(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return d_nonBusinessDays[date - d_packedCalendar.firstDate()];
}

inline
bool bdecs_Calendar::isWeekendDay(bdet_DayOfWeek::Day dayOfWeek) const
{
    return d_packedCalendar.isWeekendDay(dayOfWeek);
}

inline
bool bdecs_Calendar::isWeekendDay(const bdet_Date& date) const
{
    return d_packedCalendar.isWeekendDay(date);
}

inline
const bdet_Date& bdecs_Calendar::lastDate() const
{
    return d_packedCalendar.lastDate();
}

inline
int bdecs_Calendar::length() const
{
    return d_nonBusinessDays.length();
}

inline
int bdecs_Calendar::numBusinessDays() const
{
    return d_nonBusinessDays.numSet0();
}

inline
int bdecs_Calendar::numHolidayCodes(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return d_packedCalendar.numHolidayCodes(date);
}

inline
int bdecs_Calendar::numHolidays() const
{
    return d_packedCalendar.numHolidays();
}

inline
int bdecs_Calendar::numNonBusinessDays() const
{
    return d_nonBusinessDays.numSet1();
}

inline
int bdecs_Calendar::numWeekendDaysInRange() const
{
    return d_packedCalendar.numWeekendDaysInRange();
}

inline
const bdecs_PackedCalendar& bdecs_Calendar::packedCalendar() const
{
    return d_packedCalendar;
}

inline
bsl::ostream& bdecs_Calendar::print(bsl::ostream& stream,
                                    int           level,
                                    int           spacesPerLevel) const
{
    return d_packedCalendar.print(stream, level, spacesPerLevel);
}

inline
bdecs_Calendar::BusinessDayConstReverseIterator
bdecs_Calendar::rbeginBusinessDays() const
{
    return BusinessDayConstReverseIterator(endBusinessDays());
}

inline
bdecs_Calendar::BusinessDayConstReverseIterator
bdecs_Calendar::rbeginBusinessDays(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return BusinessDayConstReverseIterator(endBusinessDays(date));
}

inline
bdecs_Calendar::HolidayCodeConstReverseIterator
bdecs_Calendar::rbeginHolidayCodes(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return d_packedCalendar.rbeginHolidayCodes(date);
}

inline
bdecs_Calendar::HolidayCodeConstReverseIterator
bdecs_Calendar::rbeginHolidayCodes(const HolidayConstIterator& iter) const
{
    return d_packedCalendar.rbeginHolidayCodes(iter);
}

inline
bdecs_Calendar::HolidayConstReverseIterator
bdecs_Calendar::rbeginHolidays() const
{
    return d_packedCalendar.rbeginHolidays();
}

inline
bdecs_Calendar::HolidayConstReverseIterator
bdecs_Calendar::rbeginHolidays(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return d_packedCalendar.rbeginHolidays(date);
}

inline
bdecs_Calendar::BusinessDayConstReverseIterator
bdecs_Calendar::rendBusinessDays() const
{
    return BusinessDayConstReverseIterator(beginBusinessDays());
}

inline
bdecs_Calendar::BusinessDayConstReverseIterator
bdecs_Calendar::rendBusinessDays(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return BusinessDayConstReverseIterator(beginBusinessDays(date));
}

inline
bdecs_Calendar::HolidayCodeConstReverseIterator
bdecs_Calendar::rendHolidayCodes(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return d_packedCalendar.rendHolidayCodes(date);
}

inline
bdecs_Calendar::HolidayCodeConstReverseIterator
bdecs_Calendar::rendHolidayCodes(const HolidayConstIterator& iter) const
{
    return d_packedCalendar.rendHolidayCodes(iter);
}

inline
bdecs_Calendar::HolidayConstReverseIterator
bdecs_Calendar::rendHolidays() const
{
    return d_packedCalendar.rendHolidays();
}

inline
bdecs_Calendar::HolidayConstReverseIterator
bdecs_Calendar::rendHolidays(const bdet_Date& date) const
{
    BSLS_ASSERT_SAFE(isInRange(date));

    return d_packedCalendar.rendHolidays(date);
}

// FREE OPERATORS
inline
bool operator==(const bdecs_Calendar& lhs, const bdecs_Calendar& rhs)
{
    const bool result = lhs.d_packedCalendar == rhs.d_packedCalendar;
    BSLS_ASSERT_SAFE(!result ||
                               lhs.d_nonBusinessDays == rhs.d_nonBusinessDays);
    return result;
}

inline
bool operator!=(const bdecs_Calendar& lhs, const bdecs_Calendar& rhs)
{
    const bool result = lhs.d_packedCalendar != rhs.d_packedCalendar;
    BSLS_ASSERT_SAFE(result || lhs.d_nonBusinessDays == rhs.d_nonBusinessDays);
    return result;
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdecs_Calendar& calendar)
{
    calendar.print(stream, 0, -1);
    return stream;
}

// FREE FUNCTIONS
inline
void swap(bdecs_Calendar& a, bdecs_Calendar& b)
{
    a.swap(b);
}

                    // -----------------------------------------
                    // class bdecs_Calendar_BusinessDayConstIter
                    // -----------------------------------------

// CREATORS
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
inline
bdecs_Calendar_BusinessDayConstIter::bdecs_Calendar_BusinessDayConstIter()
{
}
#endif

inline
bdecs_Calendar_BusinessDayConstIter::bdecs_Calendar_BusinessDayConstIter(
                           const bdecs_Calendar_BusinessDayConstIter& original)
: d_nonBusinessDays_p(original.d_nonBusinessDays_p)
, d_firstDate(original.d_firstDate)
, d_currentOffset(original.d_currentOffset)
{
}

inline
bdecs_Calendar_BusinessDayConstIter::~bdecs_Calendar_BusinessDayConstIter()
{
}

// MANIPULATORS
inline
bdecs_Calendar_BusinessDayConstIter&
bdecs_Calendar_BusinessDayConstIter::operator=(
                                const bdecs_Calendar_BusinessDayConstIter& rhs)
{
    d_nonBusinessDays_p = rhs.d_nonBusinessDays_p;
    d_firstDate         = rhs.d_firstDate;
    d_currentOffset     = rhs.d_currentOffset;
    return *this;
}

inline
bdecs_Calendar_BusinessDayConstIter&
bdecs_Calendar_BusinessDayConstIter::operator++()
{
    BSLS_ASSERT_SAFE(d_currentOffset >= 0);

    d_currentOffset =
                  d_nonBusinessDays_p->find0AtSmallestIndexGT(d_currentOffset);
    return *this;
}

inline
bdecs_Calendar_BusinessDayConstIter&
bdecs_Calendar_BusinessDayConstIter::operator--()
{
    if (-1 == d_currentOffset) {
        d_currentOffset = d_nonBusinessDays_p->find0AtLargestIndexLE(
                                            d_nonBusinessDays_p->length() - 1);
    }
    else {
        d_currentOffset = d_nonBusinessDays_p->find0AtLargestIndexLT(
                                                              d_currentOffset);
    }
    return *this;
}

inline
bdecs_Calendar_BusinessDayConstIter operator++(
                            bdecs_Calendar_BusinessDayConstIter& iterator, int)
{
    bdecs_Calendar_BusinessDayConstIter tmp(iterator);
    ++iterator;
    return tmp;
}

inline
bdecs_Calendar_BusinessDayConstIter operator--(
                            bdecs_Calendar_BusinessDayConstIter& iterator, int)
{
    bdecs_Calendar_BusinessDayConstIter tmp(iterator);
    --iterator;
    return tmp;
}

// ACCESSORS
inline
bdecs_PackedCalendar_DateRef
bdecs_Calendar_BusinessDayConstIter::operator*() const
{
    return bdecs_PackedCalendar_DateRef(d_firstDate + d_currentOffset);
}

inline
bdecs_PackedCalendar_IteratorDateProxy
bdecs_Calendar_BusinessDayConstIter::operator->() const
{
    return bdecs_PackedCalendar_IteratorDateProxy(this->operator*());
}

// FREE OPERATORS
inline
bool
operator==(const bdecs_Calendar_BusinessDayConstIter& lhs,
           const bdecs_Calendar_BusinessDayConstIter& rhs)
{
    return lhs.d_nonBusinessDays_p == rhs.d_nonBusinessDays_p
        && lhs.d_firstDate         == rhs.d_firstDate
        && lhs.d_currentOffset     == rhs.d_currentOffset;
}

inline
bool
operator!=(const bdecs_Calendar_BusinessDayConstIter& lhs,
           const bdecs_Calendar_BusinessDayConstIter& rhs)
{
    return lhs.d_nonBusinessDays_p != rhs.d_nonBusinessDays_p
        || lhs.d_firstDate         != rhs.d_firstDate
        || lhs.d_currentOffset     != rhs.d_currentOffset;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
