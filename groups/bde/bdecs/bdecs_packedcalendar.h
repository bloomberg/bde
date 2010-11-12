// bdecs_packedcalendar.h                                             -*-C++-*-
#ifndef INCLUDED_BDECS_PACKEDCALENDAR
#define INCLUDED_BDECS_PACKEDCALENDAR

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a compact repository for weekend/holiday information.
//
//@CLASSES:
//  bdecs_PackedCalendar: compact repository of weekend/holiday information
//
//@SEE_ALSO: bdecs_calendar
//
//@AUTHOR: John Lakos (jlakos), Guillaume Morin (gmorin1)
//
//@DESCRIPTION: This component provides a fully value-semantic class,
// 'bdecs_PackedCalendar', that represents weekend and holiday information over
// a *valid* *range* of dates.  A 'bdecs_PackedCalendar' is an approximation to
// the same *mathematical* type, and is capable of representing the same subset
// of *mathematical* values, as a 'bdecs_Calendar'.
//
// But unlike 'bdecs_Calendar', which is optimized for runtime efficiency,
// 'bdecs_PackedCalendar' is designed to minimize the amount of in-process
// memory required to represent that information.  For example, a packed
// calendar having a valid range of 40 years, assuming roughly ten holidays
// per year, might consume just over 2K bytes (e.g., 4-8 bytes per holiday)
// plus an additional 4-16 bytes per holiday code (tending higher when each
// holiday code is for a unique holiday).
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
// set of holiday codes associated with an arbitrary date in a
// 'bdecs_PackedCalendar' (or the current holiday referred to by a
// 'HolidayConstIterator') can be obtained in increasing (numerical) order
// using an iterator identified by /the nested 'HolidayCodeConstIterator'
// 'typedef' (see below).
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
///Nested Iterators
///----------------
// Also provided are several STL-style 'const' bidirectional iterators
// accessible as nested 'typedef's.  'HolidayConstIterator',
// 'HolidayCodeConstIterator', and 'WeekendDayConstIterator', respectively,
// iterate over a chronologically ordered sequence of holidays, a numerically
// ordered sequence of holiday codes, and a sequence of weekend days, ordered
// from Sunday to Saturday.  As a general rule, calling a 'const' method will
// not invalidate any iterators, and calling a non-'const' method might
// invalidate any of them; it is, however, guaranteed that attempting to add
// *duplicate* holidays or holiday codes will have no effect, and therefore
// will not invalidate any iterators.  It is also guaranteed that adding a new
// code for an existing holiday will not invalidate any 'HolidayConstIterator'.
//
///Performance and Exception-Safety Guarantees
///-------------------------------------------
// The asymptotic worst-case performance of representative operations is
// characterized using big-O notation, 'O[f(N,M)]', where 'N' and 'M' each
// refer to the combined number ('H + C') of holidays 'H' (i.e., method
// 'numHolidays') and holiday codes 'C' (i.e., 'numHolidayCodesTotal') in
// the respective packed calendars.  Here, *Best* *Case* complexity,
// denoted by 'B[f(N)]', is loosely defined (for manipulators) as the
// worst-case cost, provided that (1) no additional internal capacity is
// required, (2) the bottom of the valid range does not change, and (3) that if
// a holiday (or holiday code) is being added, it is being appended *to* *the*
// *end* of the current sequence (of the latest holiday).
//..
//                                    Worst       Best   Exception-Safety
//  Operation                          Case       Case      Guarantee
//  ---------                         -----       ----   ---------------
//  DEFAULT CTOR                      O[1]                No-Throw
//  COPY CTOR(N)                      O[N]                Exception Safe
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
//  N.addWeekendDay(w)                O[1]                No-Throw
//
//  N.intersectBusinessDays(M)        O[N+M]              Basic <*>
//  N.intersectNonBusinessDays(M)     O[N+M]              Basic <*>
//  N.unionBusinessDays(M)            O[N+M]              Basic <*>
//  N.unionNonBusinessDays(M)         O[N+M]              Basic <*>
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
//  N.numWeekendDaysInWeek()          O[1]                No-Throw
//  N.numWeekendDaysInRange()         O[1]                No-Throw
//
//  N.isInRange(d);                   O[1]                No-Throw
//  N.isWeekendDay(w);                O[1]                No-Throw
//  N.isWeekendDay(d)                 O[1]                No-Throw
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
//  other const methods               O[1] .. O[N]        No-Throw
//
//
//  OP==(N,M)                         O[min(N,M)]         No-Throw
//  OP!=(N,M)                         O[min(N,M)]         No-Throw
//
//                                    <*> No-Throw guarantee when
//                                                      capacity is sufficient.
//..
// Note that *all* of the non-creator methods of 'bdecs_PackedCalendar' provide
// the *No-Throw* guarantee whenever sufficient capacity is already available.
//
// Note that these are largely the same as 'bdecs_Calendar' *except* that the
// accessors 'isBusinessDay' and 'isNonBusinessDay' are logarithmic in the
// number of holidays in 'bdecs_PackedCalendar'.
//
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
//     2010 11   2     0              ;Election Day
//     2010 11  25     1   14         ;Thanksgiving Day
//     2010 12  25     1    4         ;Christmas Day (Observed)
//     2010 12  31     1   22         ;New Year's Day (Observed)
//..
// Let's now create a couple of primitive helper functions to extract
// holiday and holiday-code counts from a given input stream.  First we'll
// create one that skips over headers and comments to get to the next valid
// holiday record:
//..
//  int getNextHoliday(bsl::istream& input, bdet_Date *holiday, int *numCodes)
//      // Load into the specified 'holiday' the date of the next holiday, and
//      // into 'numCodes' the associated number of holiday codes for the
//      // holiday read from the specified 'input' stream.  Return 0 on
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
//      if (input.good() && bdet_Date::isValid(year, month, day)) {
//          *holiday  = bdet_Date(year, month, day);
//          *numCodes = codes;
//          return SUCCESS;                                           // RETURN
//      }
//
//      return FAILURE;                                               // RETURN
//  }
//..
// Then we'll write a function that gets us an integer holiday code, or
// invalidates the stream if it cannot (note that negative holiday codes are
// not supported):
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
// Now that we have these helper functions, it's a simple matter to write a
// calendar loader function, 'load', that populates a given calendar with data
// in this "proprietary" format:
//..
//  void load(bsl::istream& input, bdecs_PackedCalendar *calendar)
//      // Populate the specified 'calendar' with holidays and corresponding
//      // codes read from the specified 'input' stream in our "proprietary"
//      // format (see above).  On success, 'input' will be empty, but
//      // valid; otherwise 'input' will be invalid.
//  {
//      bdet_Date holiday;
//      int       numCodes;
//
//      while (0 == getNextHoliday(input, &holiday, &numCodes)) {
//          calendar->addHoliday(holiday);                       // add date
//          for (int i = 0; i < numCodes; ++i) {
//              int holidayCode;
//              getNextHolidayCode(input, &holidayCode);
//              calendar->addHolidayCode(holiday, holidayCode);  // add codes
//              input.ignore(256, '\n');  // skip comments
//          }
//      }
//  }
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
//  void
//  printHolidayNamesForGivenDate(bsl::ostream&                   output,
//                                const bdecs_PackedCalendar&     calendar,
//                                const bdet_Date&                date,
//                                const bsl::vector<bsl::string>& holidayNames)
//      // Write, to the specified 'output' stream, the elements in the
//      // specified 'holidayNames' associated (via holiday codes in the
//      // specified 'calendar') to the specified 'date'.  Each holiday
//      // name emitted is followed by a newline ('\n').  The behavior is
//      // undefined unless 'date' is within the valid range of 'calendar'.
//  {
//      for (bdecs_PackedCalendar::HolidayCodeConstIterator
//                                       it = calendar.beginHolidayCodes(date);
//                                       it != calendar.endHolidayCodes(date);
//                                     ++it) {
//          output << holidayNames[*it] << bsl::endl;
//      }
//  }
//..
// Now that we can write the names of holidays for a given date, let's
// write a function that can write out all of the names associated with each
// holiday in the calendar.
//..
//  void
//  printHolidayDatesAndNames(bsl::ostream&                   output,
//                            const bdecs_PackedCalendar&     calendar,
//                            const bsl::vector<bsl::string>& holidayNames)
//      // Write, to the specified 'output' stream, each date associated with
//      // a holiday in the specified 'calendar' followed by any elements in
//      // the specified 'holidayNames' (associated via holiday codes in
//      // 'calendar') corresponding to that date.  Each date emitted is
//      // preceded and followed by a newline ('\n').  Each holiday name
//      // emitted is followed by a newline ('\n').
//
//  {
//      for (bdecs_PackedCalendar::HolidayConstIterator
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
// Next, let's provide a way to write out the same information above, but limit
// it to the date values within a given range.
//..
//  void
//  printHolidaysInRange(bsl::ostream&                   output,
//                       const bdecs_PackedCalendar&     calendar,
//                       const bdet_Date&                beginDate,
//                       const bdet_Date&                endDate,
//                       const bsl::vector<bsl::string>& holidayNames)
//      // Write, to the specified 'output' stream, each date associated
//      // with a holiday in the specified 'calendar' within the (inclusive)
//      // range indicated by the specified 'beginDate' and 'endDate',
//      // followed by any elements in the specified 'holidayNames' (associated
//      // via holiday codes in 'calendar') corresponding to that date.  Each
//      // date emitted is preceded and followed by a newline ('\n').  Each
//      // holiday name emitted is followed by a newline ('\n').  The behavior
//      // is undefined unless both 'startDate' and 'endDate' are within the
//      // valid range of 'calendar' and 'startDate <= endDate'.
//
//  {
//      for (bdecs_PackedCalendar::HolidayConstIterator
//                               it = calendar.beginHolidays(beginDate);
//                               it != calendar.endHolidays(endDate);
//                             ++it) {
//          output << '\n' << *it << '\n';
//          printHolidayNamesForGivenDate(output,
//                                        calendar,
//                                        *it,
//                                        holidayNames);
//      }
//  }
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
//..
// Finally, low-level clients may also use a populated 'bdecs_PackedCalendar'
// object directly to determine whether a particular day is a valid business
// day; however, that operation, which here is logarithmic in the number of
// holidays, can be performed *much* more efficiently (see 'bdecs_Calendar'):
//..
//  bdet_Date
//  getNextBusinessDay(const bdecs_PackedCalendar& calendar,
//                     const bdet_Date&            date)
//      // Return the next business day in the specified 'calendar' after the
//      // specified 'date'.  The behavior is undefined unless such a date
//      // exists within the valid range of 'calendar'.
//  {
//      // Assume there is a business day in the valid range after date.
//
//      bdet_Date candidate = date;
//      do {
//          ++candidate;
//      } while (calendar.isNonBusinessDay(candidate));
//                                                   // logarithmic complexity!
//      return candidate;
//  }
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDEC_DAYOFWEEKSET
#include <bdec_dayofweekset.h>
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

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_ITERATOR
#include <bsl_iterator.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bslma_Allocator;
class bdecs_PackedCalendar_BusinessDayConstIterator;
class bdecs_PackedCalendar_HolidayCodeConstIterator;
class bdecs_PackedCalendar_HolidayConstIterator;

                        // ==========================
                        // class bdecs_PackedCalendar
                        // ==========================

class bdecs_PackedCalendar {
    // This class implements a space-efficient, fully value-semantic
    // repository of weekend and holiday information over a *valid* *range*
    // of dates.  This valid range, '[ firstDate() .. lastDate() ]', spans
    // the first and last dates of a calendar's accessible contents.  A
    // calendar can be "populated" with weekend and holiday information via a
    // suite of 'add' methods.  Any subset of days of the week may be
    // specified as weekend (i.e., recurring non-business) days for each
    // occurrence of that day-of-the-week within the valid range; holidays
    // within the valid range are specified individually.  When adding a
    // holiday, an arbitrary integer "holiday code" may be associated with
    // that date.  Additional holiday codes for that date may subsequently be
    // added.  Both the holidays and the set of unique holiday codes
    // associated with each holiday date are maintained (internally) in order
    // of increasing value.  Note that the behavior of requesting *any*
    // calendar information for a supplied date whose value is outside the
    // current *valid* *range* for that calendar (unless otherwise noted,
    // e.g., 'isWeekendDay') is undefined.
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
    bdet_Date         d_firstDate;          // first valid date of calendar
                                            // or (9999,12,31) if this calendar
                                            // is empty

    bdet_Date         d_lastDate;           // last valid date of calendar
                                            // or (0001,01,01) if this calendar
                                            // is empty

    bdec_DayOfWeekSet d_weekendDays;        // representation of weekend days

    bsl::vector<int>  d_holidayOffsets;     // ordered list of all holidays
                                            // in this calendar stored as
                                            // offsets from 'd_firstDate'

    bsl::vector<int>  d_holidayCodesIndex;  // parallel to 'd_holidayOffsets',
                                            // this is a list of indices into
                                            // 'd_holidayCodes'; note that the
                                            // end of each sequence can be
                                            // determined using the value of
                                            // the next entry in this vector
                                            // if it exists, or else the length
                                            // of 'd_holidayCodes', itself

    bsl::vector<int>  d_holidayCodes;       // sequences of holiday codes,
                                            // each partitioned into an ordered
                                            // "chunk" of codes per holiday in
                                            // 'd_holidayOffsets'; chunks are
                                            // stored in the same order as in
                                            // 'd_holidayOffsets'

    bslma_Allocator  *d_allocator_p;        // memory allocator (held, not
                                            // owned)

    // FRIENDS
    friend class bdecs_PackedCalendar_BusinessDayConstIterator;
    friend bool operator==(const bdecs_PackedCalendar&,
                           const bdecs_PackedCalendar&);
    friend bool operator!=(const bdecs_PackedCalendar&,
                           const bdecs_PackedCalendar&);

  private:
    // PRIVATE TYPES
    typedef bsl::vector<int>::const_iterator       OffsetsConstIterator;
    typedef bsl::vector<int>::const_iterator       CodesIndexConstIterator;
    typedef bsl::vector<int>::const_iterator       CodesConstIterator;
    typedef bsl::vector<int>::const_reverse_iterator
                                                   OffsetsConstReverseIterator;
    typedef bsl::vector<int>::const_reverse_iterator
                                                   CodesConstReverseIterator;
    typedef bsl::vector<int>::iterator             OffsetsIterator;
    typedef bsl::vector<int>::iterator             CodesIndexIterator;
    typedef bsl::vector<int>::iterator             CodesIterator;
    typedef bsl::vector<int>::reverse_iterator     OffsetsReverseIterator;
    typedef bsl::vector<int>::reverse_iterator     CodesIndexReverseIterator;
    typedef bsl::vector<int>::reverse_iterator     CodesReverseIterator;

    typedef bsl::vector<int>::size_type            OffsetsSizeType;
    typedef bsl::vector<int>::size_type            CodesIndexSizeType;
    typedef bsl::vector<int>::size_type            CodesSizeType;

    // PRIVATE MANIPULATORS
    CodesIterator beginHolidayCodes(const OffsetsIterator& iter);
        // Return an iterator that refers to the first modifiable holiday code
        // associated with the holiday referenced by the specified 'iter'.  If
        // there are no holiday codes associated with the date referenced by
        // 'iter', the returned iterator has the same value as that returned by
        // 'endHolidayCodes(iter)'.  The behavior is undefined unless 'iter'
        // refers to a valid holiday of this calendar.

    CodesIterator endHolidayCodes(const OffsetsIterator& iter);
        // Return an iterator that indicates the element one past the last
        // modifiable holiday code associated with the date referenced by the
        // specified 'iter'.  If there are no holiday codes associated with the
        // date referenced by 'iter', the returned iterator has the same value
        // as that returned by 'beginHolidayCodes(iter)'.  The behavior is
        // undefined unless 'iter' references a valid holiday in this calendar.

    void intersectNonBusinessDaysImp(
                               const bdecs_PackedCalendar& other,
                               bool                        fixIfDeltaPositive);
        // Intersect the offsets, indices, and weekend-days with those of the
        // specified 'other' calendar; unite the holiday codes of the
        // respective intersected holidays.  The first date of this calendar
        // (and thus the range) is changed following the specified
        // 'fixIfDeltaPositive'.

    void intersectBusinessDaysImp(
                               const bdecs_PackedCalendar& other,
                               bool                        fixIfDeltaPositive);
        // Unite the offsets, indices, holiday codes, and weekend-days with
        // those of the specified 'other' calendar.  The first date of this
        // calendar (and thus the range) is changed following the specified
        // 'fixIfDeltaPositive'.

    int addHolidayImp(const int offset);
        // Add the specified 'offset' as a holiday offset in this calendar.  If
        // the date represented by the specified 'offset' is already a
        // holiday, the method has no effect.

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
        // 'iter'.  If there are no holiday codes associated with the date
        // referenced by 'iter', the returned iterator has the same value as
        // that returned by 'beginHolidayCodes(iter)'.  The behavior is
        // undefined unless 'iter' references a valid holiday in this calendar.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdecs_PackedCalendar,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // TYPES
    typedef bdecs_PackedCalendar_BusinessDayConstIterator
                                               BusinessDayConstIterator;

    typedef bdecs_PackedCalendar_HolidayConstIterator
                                               HolidayConstIterator;

    typedef bdecs_PackedCalendar_HolidayCodeConstIterator
                                               HolidayCodeConstIterator;

    typedef bdec_DayOfWeekSet::const_iterator  WeekendDayConstIterator;

    typedef bsl::reverse_iterator<BusinessDayConstIterator>
                                               BusinessDayConstReverseIterator;

    typedef bsl::reverse_iterator<HolidayConstIterator>
                                               HolidayConstReverseIterator;

    typedef bsl::reverse_iterator<HolidayCodeConstIterator>
                                               HolidayCodeConstReverseIterator;

    typedef bdec_DayOfWeekSet::const_reverse_iterator
                                               WeekendDayConstReverseIterator;

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    typedef BusinessDayConstIterator        BusinessDayIterator;
    typedef HolidayConstIterator            HolidayIterator;
    typedef HolidayCodeConstIterator        HolidayCodeIterator;
    typedef WeekendDayConstIterator         WeekendDayIterator;

    typedef BusinessDayConstReverseIterator BusinessDayReverseIterator;
    typedef HolidayConstReverseIterator     HolidayReverseIterator;
    typedef HolidayCodeConstReverseIterator HolidayCodeReverseIterator;
    typedef WeekendDayConstReverseIterator  WeekendDayReverseIterator;
#endif

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.

    // CREATORS
    explicit bdecs_PackedCalendar(bslma_Allocator *basicAllocator = 0);
        // Create an empty calendar having no valid range.  Optionally specify
        // a 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    bdecs_PackedCalendar(const bdet_Date&  firstDate,
                         const bdet_Date&  lastDate,
                         bslma_Allocator  *basicAllocator = 0);
        // Create a calendar having a valid range from the specified
        // 'firstDate' through the specified 'lastDate' if
        // 'firstDate' <= lastDate'; otherwise, make the valid range empty.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    bdecs_PackedCalendar(const bdecs_PackedCalendar&  original,
                         bslma_Allocator             *basicAllocator = 0);
        // Create a calendar having the value of the specified 'original'
        // calendar.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    ~bdecs_PackedCalendar();
        // Destroy this object.

    // MANIPULATORS
    bdecs_PackedCalendar& operator=(const bdecs_PackedCalendar& rhs);
        // Assign to this calendar the value of the specified 'rhs' calendar,
        // and return a reference to this modifiable calendar.

    void setValidRange(const bdet_Date& firstDate,
                       const bdet_Date& lastDate);
        // Set the range of this calendar using the specified 'firstDate' and
        // 'lastDate' as, respectively, the first date and the last date of the
        // calendar if 'firstDate <= lastDate'.  Otherwise, the range is made
        // empty.  Any holiday that is outside the new range and its holiday
        // codes will be removed.

    void addDay(const bdet_Date& date);
        // Extend the valid range (if necessary) to include the specified
        // 'date' value.

    void addHoliday(const bdet_Date& date);
        // Mark the specified 'date' as a holiday (i.e., a non-business day).
        // The behavior is undefined unless 'date' is within the valid range of
        // this calendar.  Note that this method has no effect if 'date' is
        // already marked as a holiday.

    int addHolidayIfInRange(const bdet_Date& date);
        // Mark the specified 'date' as a holiday (i.e., a non-business day) if
        // 'date' is within the valid range of this calendar.  Return 0 if
        // 'date' is in range, and a non-zero value (with no effect on this
        // calendar) otherwise.  This method has no effect if 'date' is already
        // marked as a holiday or is not in the valid range.

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

    int addHolidayCodeIfInRange(const bdet_Date& date, int holidayCode);
        // Mark the specified 'date' as a holiday (i.e., a non-business day)
        // and add the specified 'holidayCode' (if not already present) to the
        // set of codes associated with 'date' if 'date' is within the valid
        // range of this calendar.  Return 0 if 'date' is in range, and a
        // non-zero value otherwise.  If 'holidayCode' is already a code for
        // 'date' or if 'date' is not in the valid range, this method will
        // neither change the value of the object nor invalidate any iterators.
        // If 'date' is already marked as a holiday, this method will not
        // invalidate any 'HolidayConstIterator' iterators.  Note that this
        // method may be called repeatedly with the same value for 'date' to
        // build up a set of holiday codes for that date.

    void addWeekendDay(bdet_DayOfWeek::Day weekendDay);
        // Add to this calendar the specified 'weekendDay' (i.e., a recurring
        // non-business day).  All dates within the valid range
        // '[ firstDate() .. lastDate() ]' that fall on this day of the week
        // will cease to be business days.  Note that every occurrence of
        // 'weekendDay' will continue be a non-business day, even if the
        // valid range of this calendar is subsequently increased.

    void addWeekendDays(const bdec_DayOfWeekSet& weekendDays);
        // Add to this calendar the specified 'weekendDays' (i.e., recurring
        // non-business days).  All dates within the valid range
        // '[ firstDate() .. lastDate() ]' that fall on any day in
        // 'weekendDays' cease to be business days.  Note that every occurrence
        // of every day of the week in 'weekendDays' will continue to be a
        // non-business day, even if the valid range of this calendar is
        // subsequently increased.

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
        // 'date' is not a holiday in this calendar even if it is out of
        // range, or if the holiday at 'date' does not have 'holidayCode'
        // associated with it.

    void removeAll();
        // Remove all information from this calendar, leaving it with its
        // default constructed "empty" value.

    void reserveHolidayCapacity(int numHolidays);
        // Reserve enough space to store exactly the specified
        // 'numHolidays' within this calendar.  This method has no effect if
        // 'numHolidays < numHolidays()'.

    void reserveHolidayCodeCapacity(int numHolidayCodes);
        // Reserve enough space to store exactly the specified
        // 'numHolidayCodes' within this calendar.  This method has no effect
        // if 'numHolidayCodes <= numHolidayCodesTotal()'.

    void swap(bdecs_PackedCalendar *other);
        // Swap the value of this object with the specified 'other' object.
        // The behavior is undefined unless 'other' and this object hold the
        // same allocator.  Note that this method is logically equivalent to
        //..
        //  bdecs_Calendar tmp(*this);
        //  *this  = *other;
        //  *other = tmp;
        //..
        // except that this method is guaranteed never to throw.

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

    WeekendDayConstIterator beginWeekendDays() const;
        // Return an iterator that refers to the first weekend day of this
        // calendar.  If this calendar has no weekend days, the returned
        // iterator has the same value as that returned by 'endWeekendDays'.

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

    WeekendDayConstIterator endWeekendDays() const;
        // Return an iterator that indicates the element one past the last
        // weekend day in this calendar.  If this calendar has no weekend days,
        // the returned iterator has the same value as that returned by
        // 'beginWeekendDays'.

    const bdet_Date& firstDate() const;
        // Return a reference to the non-modifiable earliest date in the
        // valid range of this calendar.  The behavior is undefined unless
        // the calendar is non-empty -- i.e., unless '1 <= length()'.

    bool isBusinessDay(const bdet_Date& date) const;
        // Return 'true' if the specified 'date' is a business day (i.e.,
        // not a holiday or weekend day), and 'false' otherwise.  The behavior
        // is undefined unless 'true == isInRange(date)'.

    bool isHoliday(const bdet_Date& date) const;
        // Return 'true' if the specified 'date' is a holiday in this calendar,
        // and 'false' otherwise.  The behavior is undefined unless
        // 'true == isInRange(date)'.

    bool isInRange(const bdet_Date& date) const;
        // Return 'true' if the specified 'date' is within the valid range of
        // this calendar (i.e., 'firstDate() <= date <= lastDate()'), and
        // 'false' otherwise.  Note that the valid range for a
        // 'bdecs_PackedCalendar' is empty if its length is 0.

    bool isNonBusinessDay(const bdet_Date& date) const;
        // Return 'true' if the specified 'date' is not a business day (i.e.,
        // a holiday or weekend day), and 'false' otherwise.  The behavior is
        // undefined unless 'true == isInRange(date)'.  Note that:
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
        // calendar, and 'false' otherwise.

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
        // specified 'date' in this calendar.  The behavior is undefined
        // unless 'true == isInRange(date)'.

    int numHolidayCodesTotal() const;
        // Return the total number of holiday codes for all holidays in this
        // calendar.  Note that this function is used primarily in conjunction
        // with 'reserveHolidayCodeCapacity'.

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

    int numWeekendDaysInWeek() const;
        // Return the number of days of the week that are designated as weekend
        // days in this calendar.  Note that the value returned will be in the
        // range '[ 0 .. 7 ]'.

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

    WeekendDayConstReverseIterator rbeginWeekendDays() const;
        // Return an iterator that refers to the last weekend day of this
        // calendar.  If this calendar has no weekend days, the returned
        // iterator has the same value as that returned by 'rendWeekendDays'.

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

    WeekendDayConstReverseIterator rendWeekendDays() const;
        // Return an iterator that indicates the element one before the first
        // weekend day in this calendar.  If this calendar has no weekend days,
        // the returned iterator has the same value as that returned by
        // 'rbeginWeekendDays'.

    const bdec_DayOfWeekSet& weekendDays() const;
        // Return a reference to the non-modifiable set of weekend days
        // associated with this calendar.
};

// FREE OPERATORS
bool operator==(const bdecs_PackedCalendar& lhs,
                const bdecs_PackedCalendar& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' calendars have the same
    // value, and 'false' otherwise.  Two calendars have the same value if they
    // have the same valid range (or are both empty), the same weekend days,
    // the same holidays, and each corresponding pair of holidays, has the same
    // (ordered) set of associated holiday codes.

bool operator!=(const bdecs_PackedCalendar& lhs,
                const bdecs_PackedCalendar& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' calendars do not have the
    // same value, and 'false' otherwise.  Two calendars do not have the same
    // value if they do not have the same valid range (and are not both empty),
    // do not have the same weekend days, do not have the same holidays, or,
    // for at least one corresponding pair of holidays, do not have the same
    // (ordered) set of associated holiday codes.

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const bdecs_PackedCalendar& calendar);
    // Write the value of the specified 'calendar' to the specified output
    // 'stream', and return a reference to the modifiable 'stream'.

                 // ============================================
                 // class bdecs_PackedCalendar_IteratorDateProxy
                 // ============================================

class bdecs_PackedCalendar_IteratorDateProxy {
   // This object is a proxy class for 'bdet_Date' for use by the arrow
   // operator of 'bdecs_PackedCalendar_HolidayConstIterator',
   // 'bdecs_Calendar_BusinessDayConstIter', and
   // 'bdecs_PackedCalendar_BusinessDayConstIterator'.  An object of this class
   // behaves as the 'bdet_Date' object with which it was constructed.

    // DATA
    bdet_Date d_date;  // proxied date

  private:
    // NOT IMPLEMENTED
    bdecs_PackedCalendar_IteratorDateProxy&
    operator=(const bdecs_PackedCalendar_IteratorDateProxy&);

  public:
    // CREATORS
    bdecs_PackedCalendar_IteratorDateProxy(const bdet_Date& date);
        // Create a proxy object for the specified 'date'.

   ~bdecs_PackedCalendar_IteratorDateProxy();
        // Destroy this object.

    bdecs_PackedCalendar_IteratorDateProxy(
                       const bdecs_PackedCalendar_IteratorDateProxy& original);
        // Create a proxy object referencing the same 'bdet_Date' value as the
        // specified 'original' proxy.

    // ACCESSORS
    const bdet_Date *operator->() const;
        // Return the address of the proxied date object.
};

                        // ==================================
                        // class bdecs_PackedCalendar_DateRef
                        // ==================================

struct bdecs_PackedCalendar_DateRef : bdet_Date {
    // This private class is used by the arrow operator of holiday iterator and
    // business day iterator classes.  The objects instantiated from this class
    // serve as references to 'bdet_Date' objects.

  private:
    // NOT IMPLEMENTED
    bdecs_PackedCalendar_DateRef& operator=(
                                          const bdecs_PackedCalendar_DateRef&);

  public:
    // CREATORS
    explicit bdecs_PackedCalendar_DateRef(const bdet_Date& date);
        // Create a new object using the specified 'date'.

    bdecs_PackedCalendar_DateRef(const bdecs_PackedCalendar_DateRef& original);
        // Create a new object having the value of the specified 'original'
        // object.

    ~bdecs_PackedCalendar_DateRef();
        // Destroy this object.

    // ACCESSORS
    bdecs_PackedCalendar_IteratorDateProxy operator&() const;
        // Return this reference object.
};

                    // ===============================================
                    // class bdecs_PackedCalendar_HolidayConstIterator
                    // ===============================================

class bdecs_PackedCalendar_HolidayConstIterator {
    // Provide read-only, sequential access in increasing (chronological) order
    // to the holidays in a 'bdecs_PackedCalendar' object.

    // DATA
    bsl::vector<int>::const_iterator d_iterator;    // vector's iterator
    bdet_Date                        d_firstDate;   // offset date.  Note that
                                                    // dates are only 4-byte
                                                    // objects, so keeping a
                                                    // reference is not
                                                    // interesting
                                                    // performance-wise and
                                                    // size-wise.

    // FRIENDS
    friend class bdecs_PackedCalendar;
    friend bool operator==(const bdecs_PackedCalendar_HolidayConstIterator&,
                           const bdecs_PackedCalendar_HolidayConstIterator&);
    friend bool operator!=(const bdecs_PackedCalendar_HolidayConstIterator&,
                           const bdecs_PackedCalendar_HolidayConstIterator&);

  private:
    // PRIVATE TYPES
    typedef bsl::vector<int>::const_iterator OffsetsConstIterator;

    // PRIVATE CREATORS
    bdecs_PackedCalendar_HolidayConstIterator(
                                        const OffsetsConstIterator& iter,
                                        const bdet_Date             firstDate);
        // Create a holiday iterator using the specified 'iter' and
        // 'firstDate'.

  public:
    // TYPES
    typedef bdet_Date                              value_type;
    typedef int                                    difference_type;
    typedef bdecs_PackedCalendar_IteratorDateProxy pointer;
    typedef bdecs_PackedCalendar_DateRef           reference;
        // The star operator returns a 'bdecs_PackedCalendar_DateRef' *by*
        // *value*.

    typedef bsl::bidirectional_iterator_tag iterator_category;

    // CREATORS
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    bdecs_PackedCalendar_HolidayConstIterator();
        // Create an uninitialized iterator.
#endif

    bdecs_PackedCalendar_HolidayConstIterator(
                    const bdecs_PackedCalendar_HolidayConstIterator& original);
        // Create an iterator having the value of the specified 'original'
        // iterator.

    ~bdecs_PackedCalendar_HolidayConstIterator();
        // Destroy this object.

    // MANIPULATORS
    bdecs_PackedCalendar_HolidayConstIterator& operator=(
                         const bdecs_PackedCalendar_HolidayConstIterator& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator,
        // and return a reference to this modifiable iterator.

    bdecs_PackedCalendar_HolidayConstIterator& operator++();
        // Advance this iterator to refer to the next holiday in the calendar,
        // and return a reference to this modifiable object.  The behavior is
        // undefined unless, on entry, this iterator references a valid
        // holiday.

    bdecs_PackedCalendar_HolidayConstIterator& operator--();
        // Regress this iterator to refer to the previous holiday in the
        // calendar and return a reference to this modifiable object.  The
        // behavior is undefined unless, on entry, this iterator references a
        // valid holiday.

    // ACCESSORS
    bdecs_PackedCalendar_DateRef operator*() const;
        // Return a 'bdecs_PackedCalendar_DateRef' object which contains the
        // date value of the holiday referenced by this iterator.

    bdecs_PackedCalendar_IteratorDateProxy operator->() const;
        // Return a date value proxy for the current holiday.
};

// FREE OPERATORS
bool operator==(const bdecs_PackedCalendar_HolidayConstIterator& lhs,
                const bdecs_PackedCalendar_HolidayConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators point to the
    // same entry in the same calendar, and 'false' otherwise.  The behavior is
    // undefined unless 'lhs' and 'rhs' both iterate over the same calendar.

bool operator!=(const bdecs_PackedCalendar_HolidayConstIterator& lhs,
                const bdecs_PackedCalendar_HolidayConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not point to
    // the same entry in the same calendar, and 'false' otherwise.  The
    // behavior is undefined unless 'lhs' and 'rhs' both iterate over the same
    // calendar.

bdecs_PackedCalendar_HolidayConstIterator
operator++(bdecs_PackedCalendar_HolidayConstIterator& iterator, int);
    // Advance the specified 'iterator' to refer to the next holiday in the
    // calendar, and return the previous value of 'iterator'.  The behavior is
    // undefined unless, on entry, 'iterator' references a valid business day.

bdecs_PackedCalendar_HolidayConstIterator
operator--(bdecs_PackedCalendar_HolidayConstIterator& iterator, int);
    // Regress the specified 'iterator' to refer to the previous holiday in the
    // calendar, and return the previous value of 'iterator'.  The behavior is
    // undefined unless, on entry, 'iterator' references a valid holiday.

                 // ===================================================
                 // class bdecs_PackedCalendar_HolidayCodeConstIterator
                 // ===================================================

class bdecs_PackedCalendar_HolidayCodeConstIterator {
    // Provide read-only, sequential access in increasing (numerical) order to
    // the holiday codes in a 'bdecs_PackedCalendar' object.

    // DATA
    bsl::vector<int>::const_iterator d_iterator;  // the vector's iterator

    // FRIENDS
    friend class bdecs_PackedCalendar;
    friend bool operator==(
                         const bdecs_PackedCalendar_HolidayCodeConstIterator&,
                         const bdecs_PackedCalendar_HolidayCodeConstIterator&);
    friend bool operator!=(
                         const bdecs_PackedCalendar_HolidayCodeConstIterator&,
                         const bdecs_PackedCalendar_HolidayCodeConstIterator&);

  private:
    // PRIVATE TYPES
    typedef bsl::vector<int>::const_iterator CodesConstIterator;

    // PRIVATE CREATORS
    bdecs_PackedCalendar_HolidayCodeConstIterator(
                                               const CodesConstIterator& iter);
        // Create a holiday-code iterator using the specified 'iter'.

  public:
    // TYPES
    typedef int  value_type;
    typedef int  difference_type;
    typedef int *pointer;
    typedef int  reference; // The star operators returns an int *by value*.
    typedef bsl::bidirectional_iterator_tag iterator_category;

    // CREATORS
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    bdecs_PackedCalendar_HolidayCodeConstIterator();
        // Create an uninitialized iterator.
#endif

    bdecs_PackedCalendar_HolidayCodeConstIterator(
                const bdecs_PackedCalendar_HolidayCodeConstIterator& original);
        // Create an object having the value of the specified 'original'
        // iterator.

    ~bdecs_PackedCalendar_HolidayCodeConstIterator();
        // Destroy this object.

    // MANIPULATORS
    bdecs_PackedCalendar_HolidayCodeConstIterator& operator=(
                     const bdecs_PackedCalendar_HolidayCodeConstIterator& rhs);
        // Assign to this object the value of the specified 'rhs' iterator, and
        // return a reference to this modifiable iterator.

    bdecs_PackedCalendar_HolidayCodeConstIterator& operator++();
        // Advance this iterator to refer to the next holiday code for the
        // associated date in the calendar, and return a reference to this
        // modifiable object.  The behavior is undefined unless, on entry, this
        // iterator references a valid holiday code.

    bdecs_PackedCalendar_HolidayCodeConstIterator& operator--();
        // Regress this iterator to refer to the previous holiday code for the
        // associated date in the calendar, and return a reference to this
        // modifiable object.  The behavior is undefined unless, on entry, this
        // iterator references a valid holiday code.

    // ACCESSORS
    int operator*() const;
        // Return the holiday code referenced by this iterator.
};

// FREE OPERATORS
bool operator==(const bdecs_PackedCalendar_HolidayCodeConstIterator& lhs,
                const bdecs_PackedCalendar_HolidayCodeConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators points to the
    // same entry, and 'false' otherwise.  The behavior is undefined unless
    // 'lhs' and 'rhs' both reference the same holiday in the same calendar.

bool operator!=(const bdecs_PackedCalendar_HolidayCodeConstIterator& lhs,
                const bdecs_PackedCalendar_HolidayCodeConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not point to
    // the same entry, and 'false' otherwise.  The behavior is undefined unless
    // 'lhs' and 'rhs' both reference the same holiday in the same calendar.

bdecs_PackedCalendar_HolidayCodeConstIterator
operator++(bdecs_PackedCalendar_HolidayCodeConstIterator& iterator, int);
    // Advance the specified 'iterator' to refer to the next holiday code for
    // the associated date in the calendar, and return the previous value of
    // 'iterator'.  The behavior is undefined unless, on entry, 'iterator'
    // references a valid holiday code.

bdecs_PackedCalendar_HolidayCodeConstIterator
operator--(bdecs_PackedCalendar_HolidayCodeConstIterator& iterator, int);
    // Regress the specified 'iterator' to refer to the previous holiday code
    // for the associated date in the calendar, and return the previous value
    // of 'iterator'.  The behavior is undefined unless, on entry, 'iterator'
    // references a valid holiday code.

                // ===================================================
                // class bdecs_PackedCalendar_BusinessDayConstIterator
                // ===================================================

class bdecs_PackedCalendar_BusinessDayConstIterator {
    // Provide read-only, sequential access in increasing (chronological) order
    // to the business days in a 'bdecs_PackedCalendar' object.

    // DATA
    bsl::vector<int>::const_iterator  d_offsetIter;     // iterator for the
                                                        // holiday offsets
    const bdecs_PackedCalendar       *d_calendar_p;     // pointer to the
                                                        // calendar

    bdet_DayOfWeek::Day               d_dayOfWeek;      // the day-of-week
                                                        // value for the date
                                                        // referenced by this
                                                        // iterator
    int                               d_currentOffset;  // offset of the date
                                                        // referenced by this
                                                        // iterator
    bool                              d_endFlag;        // indicates an 'end'
                                                        // iterator if set to
                                                        // true

    // FRIENDS
    friend class bdecs_PackedCalendar;
    friend bool operator==(
                         const bdecs_PackedCalendar_BusinessDayConstIterator&,
                         const bdecs_PackedCalendar_BusinessDayConstIterator&);
    friend bool operator!=(
                         const bdecs_PackedCalendar_BusinessDayConstIterator&,
                         const bdecs_PackedCalendar_BusinessDayConstIterator&);

  private:
    // PRIVATE TYPES
    typedef bsl::vector<int>::const_iterator OffsetsConstIterator;

    // PRIVATE CREATORS
    bdecs_PackedCalendar_BusinessDayConstIterator(
                                      const bdecs_PackedCalendar& calendar,
                                      const bdet_Date&            startDate,
                                      bool                        endIterFlag);
        // Create a business day iterator for the specified 'calendar'.
        // If the specified 'endIterFlag' is 'false', then this iterator
        // references the first business day on or after the specified
        // 'startDate'; otherwise, this iterator references one business day
        // *past* the first business day on or after 'startDate'.  If no
        // business day matching the above specification exists, then this
        // iterator will reference one day past the end of its range.

    // PRIVATE MANIPULATORS
    void nextBusinessDay();
        // Advance this iterator to the next business day.  The behavior is
        // undefined if this method is called when 'd_endFlag' is already true.

    void previousBusinessDay();
        // Regress this iterator to the previous business day.  The behavior is
        // undefined if the iterator is already at 'beginBusinessDay'.

  public:
    // TYPES
    typedef bdet_Date                              value_type;
    typedef int                                    difference_type;
    typedef bdecs_PackedCalendar_IteratorDateProxy pointer;
    typedef bdecs_PackedCalendar_DateRef           reference;
        // The star operator returns a 'bdecs_PackedCalendar_DateRef' *by*
        // *value*.

    typedef bsl::bidirectional_iterator_tag iterator_category;

    // CREATORS
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
    bdecs_PackedCalendar_BusinessDayConstIterator();
        // Create an uninitialized iterator.
#endif

    bdecs_PackedCalendar_BusinessDayConstIterator(
               const bdecs_PackedCalendar_BusinessDayConstIterator& original);
        // Create an iterator having the value of the specified 'original'
        // iterator.

    ~bdecs_PackedCalendar_BusinessDayConstIterator();
        // Destroy this object.

    // MANIPULATORS
    bdecs_PackedCalendar_BusinessDayConstIterator& operator=(
                     const bdecs_PackedCalendar_BusinessDayConstIterator& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator,
        // and return a reference to this modifiable iterator.

    bdecs_PackedCalendar_BusinessDayConstIterator& operator++();
        // Advance this iterator to refer to the next business day in the
        // calendar and return a reference to this modifiable object.  The
        // behavior is undefined unless, on entry, this iterator references a
        // valid business day.

    bdecs_PackedCalendar_BusinessDayConstIterator& operator--();
        // Regress this iterator to refer to the previous business day in the
        // calendar, and return a reference to this modifiable object.  The
        // behavior is undefined unless, on entry, this iterator references a
        // valid business day.

    // ACCESSORS
    bdecs_PackedCalendar_DateRef operator*() const;
        // Return a 'bdecs_PackedCalendar_DateRef' object which contains the
        // date value of the business day referenced by this iterator.

    bdecs_PackedCalendar_IteratorDateProxy operator->() const;
        // Return a date value proxy for the current business day.
};

// FREE OPERATORS
inline
bool operator==(const bdecs_PackedCalendar_BusinessDayConstIterator& lhs,
                const bdecs_PackedCalendar_BusinessDayConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators point to the
    // same entry in the same calendar, and 'false' otherwise.  The behavior is
    // undefined unless 'lhs' and 'rhs' both iterate over the same calendar.

inline
bool operator!=(const bdecs_PackedCalendar_BusinessDayConstIterator& lhs,
                const bdecs_PackedCalendar_BusinessDayConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not point to
    // the same entry in the same calendar, and 'false' otherwise.  The
    // behavior is undefined unless 'lhs' and 'rhs' both iterate over the same
    // calendar.

inline
bdecs_PackedCalendar_BusinessDayConstIterator operator++(
                 bdecs_PackedCalendar_BusinessDayConstIterator& iterator, int);
    // Advance the specified 'iterator' to refer to the next business day in
    // the calendar, and return the previous value of 'iterator'.  The behavior
    // is undefined unless, on entry, 'iterator' references a valid business
    // day.

inline
bdecs_PackedCalendar_BusinessDayConstIterator operator--(
                 bdecs_PackedCalendar_BusinessDayConstIterator& iterator, int);
    // Regress the specified 'iterator' to refer to the previous business day
    // in the calendar, and return the previous value of 'iterator'.  The
    // behavior is undefined unless, on entry, 'iterator' references a valid
    // business day.

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                   // --------------------------------------------
                   // class bdecs_PackedCalendar_IteratorDateProxy
                   // --------------------------------------------

// CREATORS
inline
bdecs_PackedCalendar_IteratorDateProxy::
bdecs_PackedCalendar_IteratorDateProxy(const bdet_Date& date)
: d_date(date)
{
}

inline
bdecs_PackedCalendar_IteratorDateProxy::
~bdecs_PackedCalendar_IteratorDateProxy()
{
}

inline
bdecs_PackedCalendar_IteratorDateProxy::
bdecs_PackedCalendar_IteratorDateProxy(
                        const bdecs_PackedCalendar_IteratorDateProxy& original)
: d_date(original.d_date)
{
}

// ACCESSORS
inline
const bdet_Date *bdecs_PackedCalendar_IteratorDateProxy::operator->() const
{
    return &d_date;
}

                        // ----------------------------------
                        // class bdecs_PackedCalendar_DateRef
                        // ----------------------------------

// CREATORS
inline
bdecs_PackedCalendar_DateRef::
bdecs_PackedCalendar_DateRef(const bdet_Date& date)
: bdet_Date(date)
{
}

inline
bdecs_PackedCalendar_DateRef::
bdecs_PackedCalendar_DateRef(const bdecs_PackedCalendar_DateRef& original)
: bdet_Date(original)
{
}

inline
bdecs_PackedCalendar_DateRef::~bdecs_PackedCalendar_DateRef()
{
}

// ACCESSORS
inline
bdecs_PackedCalendar_IteratorDateProxy
bdecs_PackedCalendar_DateRef::operator&() const
{
    return *this;
}

                    // -----------------------------------------------
                    // class bdecs_PackedCalendar_HolidayConstIterator
                    // -----------------------------------------------

// CREATORS
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
inline
bdecs_PackedCalendar_HolidayConstIterator::
bdecs_PackedCalendar_HolidayConstIterator()
{
}
#endif

inline
bdecs_PackedCalendar_HolidayConstIterator::
bdecs_PackedCalendar_HolidayConstIterator(
                                         const OffsetsConstIterator& iter,
                                         const bdet_Date             firstDate)
: d_iterator(iter)
, d_firstDate(firstDate)
{
}

inline
bdecs_PackedCalendar_HolidayConstIterator::
bdecs_PackedCalendar_HolidayConstIterator(
                     const bdecs_PackedCalendar_HolidayConstIterator& original)
: d_iterator(original.d_iterator)
, d_firstDate(original.d_firstDate)
{
}

inline
bdecs_PackedCalendar_HolidayConstIterator::
~bdecs_PackedCalendar_HolidayConstIterator()
{
}

// MANIPULATORS
inline
bdecs_PackedCalendar_HolidayConstIterator&
bdecs_PackedCalendar_HolidayConstIterator::
operator=(const bdecs_PackedCalendar_HolidayConstIterator& rhs)
{
    d_iterator  = rhs.d_iterator;
    d_firstDate = rhs.d_firstDate;
    return *this;
}

inline
bdecs_PackedCalendar_HolidayConstIterator&
bdecs_PackedCalendar_HolidayConstIterator::operator++()
{
    ++d_iterator;
    return *this;
}

inline
bdecs_PackedCalendar_HolidayConstIterator&
bdecs_PackedCalendar_HolidayConstIterator::operator--()
{
    --d_iterator;
    return *this;
}

inline
bdecs_PackedCalendar_HolidayConstIterator
operator++(bdecs_PackedCalendar_HolidayConstIterator& iterator, int)
{
    bdecs_PackedCalendar_HolidayConstIterator tmp(iterator);
    ++iterator;
    return tmp;
}

inline
bdecs_PackedCalendar_HolidayConstIterator
operator--(bdecs_PackedCalendar_HolidayConstIterator& iterator, int)
{
    bdecs_PackedCalendar_HolidayConstIterator tmp(iterator);
    --iterator;
    return tmp;
}

// ACCESSORS
inline
bdecs_PackedCalendar_DateRef
bdecs_PackedCalendar_HolidayConstIterator::operator*() const
{
    return bdecs_PackedCalendar_DateRef(d_firstDate + *d_iterator);
}

inline
bdecs_PackedCalendar_IteratorDateProxy
bdecs_PackedCalendar_HolidayConstIterator::operator->() const
{
    return bdecs_PackedCalendar_IteratorDateProxy(this->operator*());
}

// FREE OPERATORS
inline
bool operator==(const bdecs_PackedCalendar_HolidayConstIterator& lhs,
                const bdecs_PackedCalendar_HolidayConstIterator& rhs)
{
    return lhs.d_iterator == rhs.d_iterator;
}

inline
bool operator!=(const bdecs_PackedCalendar_HolidayConstIterator& lhs,
                const bdecs_PackedCalendar_HolidayConstIterator& rhs)
{
    return lhs.d_iterator != rhs.d_iterator;
}

                // ---------------------------------------------------
                // class bdecs_PackedCalendar_HolidayCodeConstIterator
                // ---------------------------------------------------

// CREATORS
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
inline
bdecs_PackedCalendar_HolidayCodeConstIterator::
bdecs_PackedCalendar_HolidayCodeConstIterator()
{
}
#endif

inline
bdecs_PackedCalendar_HolidayCodeConstIterator::
bdecs_PackedCalendar_HolidayCodeConstIterator(const CodesConstIterator& iter)
: d_iterator(iter)
{
}

inline
bdecs_PackedCalendar_HolidayCodeConstIterator::
bdecs_PackedCalendar_HolidayCodeConstIterator(
                 const bdecs_PackedCalendar_HolidayCodeConstIterator& original)
: d_iterator(original.d_iterator)
{
}

inline
bdecs_PackedCalendar_HolidayCodeConstIterator::
~bdecs_PackedCalendar_HolidayCodeConstIterator()
{
}

// MANIPULATORS
inline
bdecs_PackedCalendar_HolidayCodeConstIterator&
bdecs_PackedCalendar_HolidayCodeConstIterator::
operator=(const bdecs_PackedCalendar_HolidayCodeConstIterator& rhs)
{
    d_iterator = rhs.d_iterator;
    return *this;
}

inline
bdecs_PackedCalendar_HolidayCodeConstIterator&
bdecs_PackedCalendar_HolidayCodeConstIterator::operator++()
{
    ++d_iterator;
    return *this;
}

inline
bdecs_PackedCalendar_HolidayCodeConstIterator&
bdecs_PackedCalendar_HolidayCodeConstIterator::operator--()
{
    --d_iterator;
    return *this;
}

inline
bdecs_PackedCalendar_HolidayCodeConstIterator
operator++(bdecs_PackedCalendar_HolidayCodeConstIterator& iterator, int)
{
    bdecs_PackedCalendar_HolidayCodeConstIterator tmp(iterator);
    ++iterator;
    return tmp;
}

inline
bdecs_PackedCalendar_HolidayCodeConstIterator
operator--(bdecs_PackedCalendar_HolidayCodeConstIterator& iterator, int)
{
    bdecs_PackedCalendar_HolidayCodeConstIterator tmp(iterator);
    --iterator;
    return tmp;
}

// ACCESSORS
inline
int bdecs_PackedCalendar_HolidayCodeConstIterator::operator*() const
{
    return *d_iterator;
}

// FREE OPERATORS
inline
bool operator==(const bdecs_PackedCalendar_HolidayCodeConstIterator& lhs,
                const bdecs_PackedCalendar_HolidayCodeConstIterator& rhs)
{
    return lhs.d_iterator == rhs.d_iterator;
}

inline
bool operator!=(const bdecs_PackedCalendar_HolidayCodeConstIterator& lhs,
                const bdecs_PackedCalendar_HolidayCodeConstIterator& rhs)
{
    return lhs.d_iterator != rhs.d_iterator;
}

              // ---------------------------------------------------
              // class bdecs_PackedCalendar_BusinessDayConstIterator
              // ---------------------------------------------------

// CREATORS
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
inline
bdecs_PackedCalendar_BusinessDayConstIterator::
bdecs_PackedCalendar_BusinessDayConstIterator()
{
}
#endif

inline
bdecs_PackedCalendar_BusinessDayConstIterator::
~bdecs_PackedCalendar_BusinessDayConstIterator()
{
}

inline
bdecs_PackedCalendar_BusinessDayConstIterator::
bdecs_PackedCalendar_BusinessDayConstIterator(
                 const bdecs_PackedCalendar_BusinessDayConstIterator& original)
: d_offsetIter(original.d_offsetIter)
, d_calendar_p(original.d_calendar_p)
, d_dayOfWeek(original.d_dayOfWeek)
, d_currentOffset(original.d_currentOffset)
, d_endFlag(original.d_endFlag)
{
    BSLS_ASSERT_SAFE(
     (d_calendar_p->firstDate() + d_currentOffset).dayOfWeek() == d_dayOfWeek);
}

// MANIPULATORS
inline
bdecs_PackedCalendar_BusinessDayConstIterator&
bdecs_PackedCalendar_BusinessDayConstIterator::operator++()
{
    nextBusinessDay();
    return *this;
}

inline
bdecs_PackedCalendar_BusinessDayConstIterator&
bdecs_PackedCalendar_BusinessDayConstIterator::operator--()
{
    previousBusinessDay();
    return *this;
}

inline
bdecs_PackedCalendar_BusinessDayConstIterator operator++(
                  bdecs_PackedCalendar_BusinessDayConstIterator& iterator, int)
{
    bdecs_PackedCalendar_BusinessDayConstIterator tmp(iterator);
    ++iterator;
    return tmp;
}

inline
bdecs_PackedCalendar_BusinessDayConstIterator operator--(
                  bdecs_PackedCalendar_BusinessDayConstIterator& iterator, int)
{
    bdecs_PackedCalendar_BusinessDayConstIterator tmp(iterator);
    --iterator;
    return tmp;
}

// ACCESSORS
inline
bdecs_PackedCalendar_DateRef
bdecs_PackedCalendar_BusinessDayConstIterator::operator*() const
{
    return bdecs_PackedCalendar_DateRef(d_calendar_p->firstDate() +
                                        d_currentOffset);
}

inline
bdecs_PackedCalendar_IteratorDateProxy
bdecs_PackedCalendar_BusinessDayConstIterator::operator->() const
{
    return bdecs_PackedCalendar_IteratorDateProxy(this->operator*());
}

// FREE OPERATORS
inline
bool
operator==(const bdecs_PackedCalendar_BusinessDayConstIterator& lhs,
           const bdecs_PackedCalendar_BusinessDayConstIterator& rhs)
{
    return lhs.d_calendar_p     == rhs.d_calendar_p
        && lhs.d_endFlag        == rhs.d_endFlag
        && (lhs.d_currentOffset == rhs.d_currentOffset
         || lhs.d_endFlag == true);
}

inline
bool
operator!=(const bdecs_PackedCalendar_BusinessDayConstIterator& lhs,
           const bdecs_PackedCalendar_BusinessDayConstIterator& rhs)
{
    return lhs.d_calendar_p != rhs.d_calendar_p
        || lhs.d_endFlag    != rhs.d_endFlag
        || (lhs.d_endFlag   == false
         && lhs.d_currentOffset != rhs.d_currentOffset);
}

                       // --------------------------
                       // class bdecs_PackedCalendar
                       // --------------------------

// CLASS METHODS
inline
int bdecs_PackedCalendar::maxSupportedBdexVersion()
{
    return 1;
}

// PRIVATE MANIPULATORS
inline
bdecs_PackedCalendar::CodesIterator
bdecs_PackedCalendar::beginHolidayCodes(const OffsetsIterator& iter)
{
    const int indexOffset = iter - d_holidayOffsets.begin();
    const int codeOffset = d_holidayCodesIndex[indexOffset];
    return d_holidayCodes.begin() + codeOffset;
}

inline
bdecs_PackedCalendar::CodesIterator
bdecs_PackedCalendar::endHolidayCodes(const OffsetsIterator& iter)
{
    // Use 'OffsetsSizeType' instead of 'int' to avoid a gcc warning.

    const OffsetsSizeType endIndexOffset = iter - d_holidayOffsets.begin() + 1;

    const int iterIndex = endIndexOffset == d_holidayCodesIndex.size()
                        ? d_holidayCodes.size()
                        : d_holidayCodesIndex[endIndexOffset];
    return d_holidayCodes.begin() + iterIndex;
}

// PRIVATE ACCESSORS
inline
bdecs_PackedCalendar::CodesConstIterator
bdecs_PackedCalendar::beginHolidayCodes(const OffsetsConstIterator& iter) const
{
    const int indexOffset = iter - d_holidayOffsets.begin();
    const int codeOffset  = d_holidayCodesIndex[indexOffset];
    return d_holidayCodes.begin() + codeOffset;
}

inline
bdecs_PackedCalendar::CodesConstIterator
bdecs_PackedCalendar::endHolidayCodes(const OffsetsConstIterator& iter) const
{
    // Use 'OffsetsSizeType' instead of 'int' to avoid a gcc warning.

    const OffsetsSizeType endIndexOffset = iter - d_holidayOffsets.begin() + 1;

    const int iterIndex = endIndexOffset == d_holidayCodesIndex.size()
                        ? d_holidayCodes.size()
                        : d_holidayCodesIndex[endIndexOffset];
    return d_holidayCodes.begin() + iterIndex;
}

// MANIPULATORS
inline
void bdecs_PackedCalendar::addWeekendDay(bdet_DayOfWeek::Day weekendDay)
{
    d_weekendDays.add(weekendDay);
}

inline
void bdecs_PackedCalendar::addWeekendDays(const bdec_DayOfWeekSet& weekendDays)
{
    d_weekendDays |= weekendDays;
}

template <class STREAM>
STREAM& bdecs_PackedCalendar::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // Switch on the schema version (starting with 1).
          case 1: {
            bdecs_PackedCalendar inCal(d_allocator_p);
            inCal.d_firstDate.bdexStreamIn(stream, 1);
            if (!stream) {
                return stream;
            }

            inCal.d_lastDate.bdexStreamIn(stream, 1);
            if (!stream ||
                   (inCal.d_firstDate > inCal.d_lastDate
                 && (   inCal.d_firstDate != bdet_Date(9999,12,31)
                     || inCal.d_lastDate  != bdet_Date(1,1,1)))) {
                stream.invalidate();
                return stream;
            }
            int length = inCal.d_lastDate - inCal.d_firstDate + 1;

            inCal.d_weekendDays.bdexStreamIn(stream, 1);
            if (!stream) {
                return stream;
            }

            int offsetsLength;
            stream.getLength(offsetsLength);
            if (   !stream
                ||    (inCal.d_firstDate >= inCal.d_lastDate
                   && offsetsLength != 0)
                ||    (inCal.d_firstDate <  inCal.d_lastDate
                   && (offsetsLength < 0 || offsetsLength > length))) {
                stream.invalidate();
                return stream;
            }
            BSLS_ASSERT_SAFE(offsetsLength >= 0);

            int codesLength;
            stream.getLength(codesLength);
            if (!stream || (0 == offsetsLength && codesLength != 0)) {
                stream.invalidate();
                return stream;
            }

            // 'd_holidayCodesIndex' and 'd_holidayOffsets' have the same size.

            inCal.d_holidayOffsets.reserve(offsetsLength);
            inCal.d_holidayCodesIndex.reserve(offsetsLength);

            inCal.d_holidayCodes.resize(codesLength);
            inCal.d_holidayOffsets.resize(offsetsLength);
            inCal.d_holidayCodesIndex.resize(offsetsLength);

            int previousValue = -1;
            int i = 0;
            for (OffsetsIterator it = inCal.d_holidayOffsets.begin();
                    i < offsetsLength; ++i, ++it) {
                stream.getInt32(*it);
                if (   !stream || *it < 0 || *it >= length
                    || *it <= previousValue) {
                    stream.invalidate();
                    return stream;
                }
                previousValue = *it;
            }

            previousValue = -1;
            i = 0;
            for (CodesIndexIterator it = inCal.d_holidayCodesIndex.begin();
                    i < offsetsLength; ++i, ++it) {
                stream.getInt32(*it);

                // This vector is ordered but duplicates are allowed.  The
                // first element must be 0.

                if (!stream || *it < 0 || *it < previousValue
                 || *it > codesLength  || (0 == i && 0 != *it)) {

                    // If we get here, some of the code indices could
                    // potentially be greater than 'codesLength'.  That would
                    // trigger an assertion in the destructor.  So call
                    // 'removeAll' to clean up.

                    inCal.removeAll();
                    stream.invalidate();
                    return stream;
                }
                previousValue = *it;
            }

            CodesIndexIterator it = inCal.d_holidayCodesIndex.begin();
            CodesIndexIterator end = inCal.d_holidayCodesIndex.end();

            // Skip the holidays that have no codes.

            while (it != end && *it == 0) {
                ++it;
            }

            // 'it' is now positioned at the first holiday with one or more
            // codes or at the end.

            bool previousValueFlag = false; // This flag will be used
                                            // to determine if we
                                            // are inside an ordered
                                            // sequence of codes
                                            // (i.e 'previousValue'
                                            // refers to a code
                                            // for the same holiday
                                            // as 'value').

            i = 0;
            for (CodesIterator jt = inCal.d_holidayCodes.begin();
                    i < codesLength; ++i, ++jt) {
                stream.getInt32(*jt);
                if (   !stream
                    || (previousValueFlag && *jt <= previousValue)) {
                    stream.invalidate();
                    return stream;
                }

                // Regardless of whether or not there is more data, advance the
                // index iterator as needed and update 'previousValueFlag' if
                // 'it' moves.

                if (it != end && i == (*it - 1)) {
                    previousValueFlag = false;

                    while (it != end && i == (*it - 1)) {
                        ++it; // Skip the holidays that have no codes.
                    }
                }
                else {
                    previousValueFlag = true;
                }
                previousValue = *jt;
            }
            BSLS_ASSERT_SAFE(it == end);

            swap(&inCal); // This cannot throw any exceptions.
          } break;
          default: {
               stream.invalidate();
          }
        }
    }
    return stream;
}

inline
void bdecs_PackedCalendar::reserveHolidayCapacity(int numHolidays)
{
    d_holidayOffsets.reserve(numHolidays);
}

inline
void bdecs_PackedCalendar::reserveHolidayCodeCapacity(int numHolidayCodes)
{
    d_holidayCodes.reserve(numHolidayCodes);
}

// ACCESSORS
template <class STREAM>
STREAM& bdecs_PackedCalendar::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {  // Switch on the schema version (starting with 1).
      case 1: {
          d_firstDate.bdexStreamOut(stream, 1);
          d_lastDate.bdexStreamOut(stream, 1);
          d_weekendDays.bdexStreamOut(stream, 1);

          stream.putLength(d_holidayOffsets.size());
          stream.putLength(d_holidayCodes.size());

          for (OffsetsSizeType i = 0; i < d_holidayOffsets.size(); ++i) {
              stream.putInt32(d_holidayOffsets[i]);
          }
          for (CodesIndexSizeType i = 0; i < d_holidayCodesIndex.size(); ++i) {
              stream.putInt32(d_holidayCodesIndex[i]);
          }

          for (CodesSizeType i = 0; i < d_holidayCodes.size(); ++i) {
              stream.putInt32(d_holidayCodes[i]);
          }
      } break;
    }
    return stream;
}

inline
bdecs_PackedCalendar::BusinessDayConstIterator
bdecs_PackedCalendar::beginBusinessDays() const
{
    return BusinessDayConstIterator(*this, d_firstDate, false);
}

inline
bdecs_PackedCalendar::BusinessDayConstIterator
bdecs_PackedCalendar::beginBusinessDays(const bdet_Date& date) const
{
    return BusinessDayConstIterator(*this, date, false);
}

inline
bdecs_PackedCalendar::HolidayCodeConstIterator
bdecs_PackedCalendar::beginHolidayCodes(const HolidayConstIterator& iter) const
{
    return HolidayCodeConstIterator(beginHolidayCodes(iter.d_iterator));
}

inline
bdecs_PackedCalendar::HolidayConstIterator
bdecs_PackedCalendar::beginHolidays() const
{
    return HolidayConstIterator(d_holidayOffsets.begin(), d_firstDate);
}

inline
bdecs_PackedCalendar::HolidayConstIterator
bdecs_PackedCalendar::beginHolidays(const bdet_Date& date) const
{
    OffsetsConstIterator i = bsl::lower_bound(d_holidayOffsets.begin(),
                                              d_holidayOffsets.end(),
                                              date - d_firstDate);
    return HolidayConstIterator(i, d_firstDate);
}

inline
bdecs_PackedCalendar::WeekendDayConstIterator
bdecs_PackedCalendar::beginWeekendDays() const
{
    return d_weekendDays.begin();
}

inline
bdecs_PackedCalendar::BusinessDayConstIterator
bdecs_PackedCalendar::endBusinessDays() const
{
    return BusinessDayConstIterator(*this, d_lastDate, true);
}

inline
bdecs_PackedCalendar::BusinessDayConstIterator
bdecs_PackedCalendar::endBusinessDays(const bdet_Date& date) const
{
    return BusinessDayConstIterator(*this, date, true);
}

inline
bdecs_PackedCalendar::HolidayCodeConstIterator
bdecs_PackedCalendar::endHolidayCodes(const HolidayConstIterator& iter) const
{
    return endHolidayCodes(iter.d_iterator);
}

inline
bdecs_PackedCalendar::HolidayConstIterator
bdecs_PackedCalendar::endHolidays() const
{
    return HolidayConstIterator(d_holidayOffsets.end(), d_firstDate);
}

inline
bdecs_PackedCalendar::HolidayConstIterator
bdecs_PackedCalendar::endHolidays(const bdet_Date& date) const
{
    OffsetsConstIterator i = bsl::lower_bound(d_holidayOffsets.begin(),
                                              d_holidayOffsets.end(),
                                              date - d_firstDate + 1);
    return HolidayConstIterator(i, d_firstDate);
}

inline
bdecs_PackedCalendar::WeekendDayConstIterator
bdecs_PackedCalendar::endWeekendDays() const
{
    return d_weekendDays.end();
}

inline
const bdet_Date& bdecs_PackedCalendar::firstDate() const
{
    return d_firstDate;
}

inline
bool bdecs_PackedCalendar::isBusinessDay(const bdet_Date& date) const
{
    return !isNonBusinessDay(date);
}

inline
bool bdecs_PackedCalendar::isHoliday(const bdet_Date& date) const
{
    const int offset = date - d_firstDate;
    const OffsetsConstIterator offsetEnd = d_holidayOffsets.end();
    const OffsetsConstIterator i = bsl::lower_bound(d_holidayOffsets.begin(),
                                                    offsetEnd,
                                                    offset);
    if (i == offsetEnd || *i != offset) {
        return false;
    }
    return true;
}

inline
bool bdecs_PackedCalendar::isInRange(const bdet_Date& date) const
{
    return d_firstDate <= date && date <= d_lastDate;
}

inline
bool bdecs_PackedCalendar::isNonBusinessDay(const bdet_Date& date) const
{
    return isWeekendDay(date) || isHoliday(date);
}

inline
bool bdecs_PackedCalendar::isWeekendDay(bdet_DayOfWeek::Day dayOfWeek) const
{
    return d_weekendDays.isMember(dayOfWeek);
}

inline
bool bdecs_PackedCalendar::isWeekendDay(const bdet_Date& date) const
{
   return isWeekendDay(date.dayOfWeek());
}

inline
const bdet_Date& bdecs_PackedCalendar::lastDate() const
{
    return d_lastDate;
}

inline
int bdecs_PackedCalendar::length() const
{
    return d_firstDate <= d_lastDate ? d_lastDate - d_firstDate + 1 : 0;
}

inline
int bdecs_PackedCalendar::numBusinessDays() const
{
    return length() - numNonBusinessDays();
}

inline
int bdecs_PackedCalendar::numHolidayCodesTotal() const
{
    return d_holidayCodes.size();
}

inline
int bdecs_PackedCalendar::numHolidays() const
{
    return d_holidayOffsets.size();
}

inline
int bdecs_PackedCalendar::numWeekendDaysInWeek() const
{
    return d_weekendDays.length();
}

inline
bdecs_PackedCalendar::BusinessDayConstReverseIterator
bdecs_PackedCalendar::rbeginBusinessDays() const
{
    return BusinessDayConstReverseIterator(endBusinessDays());
}

inline
bdecs_PackedCalendar::BusinessDayConstReverseIterator
bdecs_PackedCalendar::rbeginBusinessDays(const bdet_Date& date) const
{
    return BusinessDayConstReverseIterator(endBusinessDays(date));
}

inline
bdecs_PackedCalendar::HolidayCodeConstReverseIterator
bdecs_PackedCalendar::rbeginHolidayCodes(const bdet_Date& date) const
{
    return HolidayCodeConstReverseIterator(endHolidayCodes(date));
}

inline
bdecs_PackedCalendar::HolidayCodeConstReverseIterator
bdecs_PackedCalendar::
rbeginHolidayCodes(const HolidayConstIterator& iter) const
{
    return HolidayCodeConstReverseIterator(endHolidayCodes(iter));
}

inline
bdecs_PackedCalendar::HolidayConstReverseIterator
bdecs_PackedCalendar::rbeginHolidays() const
{
    return HolidayConstReverseIterator(endHolidays());
}

inline
bdecs_PackedCalendar::HolidayConstReverseIterator
bdecs_PackedCalendar::rbeginHolidays(const bdet_Date& date) const
{
    return HolidayConstReverseIterator(endHolidays(date));
}

inline
bdecs_PackedCalendar::WeekendDayConstReverseIterator
bdecs_PackedCalendar::rbeginWeekendDays() const
{
    return d_weekendDays.rbegin();
}

inline
bdecs_PackedCalendar::BusinessDayConstReverseIterator
bdecs_PackedCalendar::rendBusinessDays() const
{
    return BusinessDayConstReverseIterator(beginBusinessDays());
}

inline
bdecs_PackedCalendar::BusinessDayConstReverseIterator
bdecs_PackedCalendar::rendBusinessDays(const bdet_Date& date) const
{
    return BusinessDayConstReverseIterator(beginBusinessDays(date));
}

inline
bdecs_PackedCalendar::HolidayCodeConstReverseIterator
bdecs_PackedCalendar::rendHolidayCodes(const bdet_Date& date) const
{
    return HolidayCodeConstReverseIterator(beginHolidayCodes(date));
}

inline
bdecs_PackedCalendar::HolidayCodeConstReverseIterator
bdecs_PackedCalendar::rendHolidayCodes(const HolidayConstIterator& iter) const
{
    return HolidayCodeConstReverseIterator(beginHolidayCodes(iter));
}

inline
bdecs_PackedCalendar::HolidayConstReverseIterator
bdecs_PackedCalendar::rendHolidays() const
{
    return HolidayConstReverseIterator(beginHolidays());
}

inline
bdecs_PackedCalendar::HolidayConstReverseIterator
bdecs_PackedCalendar::rendHolidays(const bdet_Date& date) const
{
    return HolidayConstReverseIterator(beginHolidays(date));
}

inline
bdecs_PackedCalendar::WeekendDayConstReverseIterator
bdecs_PackedCalendar::rendWeekendDays() const
{
    return d_weekendDays.rend();
}

inline
const bdec_DayOfWeekSet& bdecs_PackedCalendar::weekendDays() const
{
    return d_weekendDays;
}

// Backwards Compatibility Types
#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY
typedef bdecs_PackedCalendar_BusinessDayConstIterator
                                       bdecs_PackedCalendarBusinessDayIterator;
typedef bdecs_PackedCalendar_IteratorDateProxy
                                       bdecs_Iterator_DateProxy;
typedef bdecs_PackedCalendar_HolidayConstIterator
                                       bdecs_HolidayIter;
typedef bdecs_PackedCalendar_HolidayCodeConstIterator
                                       bdecs_HolidayCodeIter;
#endif

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2005
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
