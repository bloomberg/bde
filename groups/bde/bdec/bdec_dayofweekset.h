// bdec_dayofweekset.h                                                -*-C++-*-
#ifndef INCLUDED_BDEC_DAYOFWEEKSET
#define INCLUDED_BDEC_DAYOFWEEKSET

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide an ordered set of (unique) 'bdet_DayOfWeek::Day' values.
//
//@CLASSES:
//  bdec_DayOfWeekSet: ordered set of (unique) 'bdet_DayOfWeek::Day' values
//
//@AUTHOR: Jeffrey Mendelsohn (jmendelsohn)
//
//@SEE_ALSO: bdecs_calendar
//
//@DESCRIPTION: This component implements an efficient value-semantic, ordered
// set class for elements of the enumerated 'bdet_DayOfWeek::Day'.  As there
// are only seven possible element values, asymptotic performance
// characterization is not appropriate; all operations implicitly run in
// constant time and provide the no-throw guarantee.
//
///Supplementary Overloaded Operators
///----------------------------------
// In addition to the standard value-semantic operators '=', '==', '!=', and
// '<<', the following canonical set of binary and unary (free) operators are
// defined on 'bdec_DayOfWeekSet' objects:
//..
//                   set S: { BDET_MON, BDET_TUE, BDET_WED }
//                   set T: { BDET_MON, BDET_WED, BDET_FRI }
//
//           Union:  S | T  { BDET_MON, BDET_TUE, BDET_WED, BDET_FRI }
//
//    Intersection:  S & T  { BDET_MON, BDET_WED }
//
//    Exclusive Or:  S ^ T  { BDET_TUE, BDET_FRI }
//
//     Subtraction:  S - T  { BDET_TUE }
//                   T - S  { BDET_FRI }
//
//  Unary Negation:     ~S  { BDET_SUN, BDET_THU, BDET_FRI, BDET_SAT }
//                      ~T  { BDET_SUN, BDET_TUE, BDET_THU, BDET_SAT }
//..
// The corresponding assignment (member) operators '|=' , '&=', '^=', and '-='
// (but not '~=') are also provided.
//
///Usage
///-----
// A 'bdec_DayOfWeekSet' is useful for recording recurring appointments,
// or special days (e.g., weekend days), in a calendar.  The following
// snippets of code illustrate how to create and use a 'bdec_DayOfWeek' set.
// We'll start by creating a couple of commonly useful sets.  First we
// define the 'bdec_dayOfWeekSet' 'weekendDays':
//..
//  bdec_DayOfWeekSet weekendDays;  assert(0 == weekendDays.length());
//..
// Notice that, this set is initially empty.  Next, let's add the days that
// characterize weekends:
//..
//  weekendDays.add(bdet_DayOfWeek::BDET_SUN);
//  assert(1 == weekendDays.length());
//
//  weekendDays.add(bdet_DayOfWeek::BDET_SAT);
//  assert(2 == weekendDays.length());
//..
// Observe that 'weekendDays' now contains precisely the days we expect it to
// contain:
//..
//  assert(true  == weekendDays.isMember(bdet_DayOfWeek::BDET_SUN));
//  assert(false == weekendDays.isMember(bdet_DayOfWeek::BDET_MON));
//  assert(false == weekendDays.isMember(bdet_DayOfWeek::BDET_TUE));
//  assert(false == weekendDays.isMember(bdet_DayOfWeek::BDET_WED));
//  assert(false == weekendDays.isMember(bdet_DayOfWeek::BDET_THU));
//  assert(false == weekendDays.isMember(bdet_DayOfWeek::BDET_FRI));
//  assert(true  == weekendDays.isMember(bdet_DayOfWeek::BDET_SAT));
//..
// Now let's create the complementary 'bdec_DayOfWeekSet' 'weekDays' directly
// from 'weekendDays' via a combination of unary negation and copy
// construction:
//..
//  bdec_DayOfWeekSet weekDays(~weekendDays);
//
//  assert(5 == weekDays.length());
//
//  assert(false == weekDays.isMember(bdet_DayOfWeek::BDET_SUN));
//  assert(true  == weekDays.isMember(bdet_DayOfWeek::BDET_MON));
//  assert(true  == weekDays.isMember(bdet_DayOfWeek::BDET_TUE));
//  assert(true  == weekDays.isMember(bdet_DayOfWeek::BDET_WED));
//  assert(true  == weekDays.isMember(bdet_DayOfWeek::BDET_THU));
//  assert(true  == weekDays.isMember(bdet_DayOfWeek::BDET_FRI));
//  assert(false == weekDays.isMember(bdet_DayOfWeek::BDET_SAT));
//..
// If we want to create a set containing all of the days in the week, we could
// do so via unary negation of the default constructed value:
//..
//  const bdec_DayOfWeekSet NO_DAYS;
//  const bdec_DayOfWeekSet ALL_DAYS(~NO_DAYS);
//
//  assert(7 == ALL_DAYS.length());
//..
// Observe that neither 'weekDays' nor 'weekendDays' represent the same value
// as 'ALL_DAYS', but their union does:
//..
//  assert(ALL_DAYS != weekendDays);
//  assert(ALL_DAYS != weekDays);
//  assert(ALL_DAYS == (weekDays | weekendDays));
//  assert(ALL_DAYS == (weekDays ^ weekendDays));
//
//  assert(weekendDays == ALL_DAYS - weekDays);
//
//  assert(weekDays    == ALL_DAYS - weekendDays);
//
//  assert(weekDays    == ALL_DAYS - weekendDays);
//..
// Similarly, neither 'weekDays' nor 'weekendDays' represents the same value as
// 'NO_DAYS', but their intersection does:
//..
//  assert(NO_DAYS != weekendDays);
//  assert(NO_DAYS != weekDays);
//  assert(NO_DAYS == (weekDays & weekendDays));
//
//  assert(weekendDays == weekendDays - weekDays);
//
//  assert(weekDays    == weekDays - weekendDays);
//..
// The only days of the week that have an 'E' in them are 'TUESDAY' and
// 'WEDNESDAY'.  Let's create the corresponding set 'eDays':
//..
//  bdec_DayOfWeekSet eDays;                 assert(0 == eDays.length());
//  eDays.add(bdet_DayOfWeek::BDET_TUE);     assert(1 == eDays.length());
//  eDays.add(bdet_DayOfWeek::BDET_WED);     assert(2 == eDays.length());
//
//  assert(false == eDays.isMember(bdet_DayOfWeek::BDET_SUN));
//  assert(false == eDays.isMember(bdet_DayOfWeek::BDET_MON));
//  assert(true  == eDays.isMember(bdet_DayOfWeek::BDET_TUE));
//  assert(true  == eDays.isMember(bdet_DayOfWeek::BDET_WED));
//  assert(false == eDays.isMember(bdet_DayOfWeek::BDET_THU));
//  assert(false == eDays.isMember(bdet_DayOfWeek::BDET_FRI));
//  assert(false == eDays.isMember(bdet_DayOfWeek::BDET_SAT));
//..
// The only days of the week that have an 'N' in them are 'MONDAY' and
// 'WEDNESDAY', and 'SUNDAY'.  Let's create the corresponding set 'nDays'
// starting with the value of 'eDays' by first removing 'TUESDAY',
// and then adding 'SUNDAY' and 'MONDAY':
//..
//  bdec_DayOfWeekSet nDays(eDays);          assert(2 == nDays.length());
//
//  nDays.remove(bdet_DayOfWeek::BDET_TUE);  assert(1 == nDays.length());
//
//  nDays.add(bdet_DayOfWeek::BDET_SUN);     assert(2 == nDays.length());
//  nDays.add(bdet_DayOfWeek::BDET_MON);     assert(3 == nDays.length());
//
//  assert(true  == nDays.isMember(bdet_DayOfWeek::BDET_SUN));
//  assert(true  == nDays.isMember(bdet_DayOfWeek::BDET_MON));
//  assert(false == nDays.isMember(bdet_DayOfWeek::BDET_TUE));
//  assert(true  == nDays.isMember(bdet_DayOfWeek::BDET_WED));
//  assert(false == nDays.isMember(bdet_DayOfWeek::BDET_THU));
//  assert(false == nDays.isMember(bdet_DayOfWeek::BDET_FRI));
//  assert(false == nDays.isMember(bdet_DayOfWeek::BDET_SAT));
//..
// Observe that all 'eDays' are 'weekDays', but that's not true of 'nDays':
//..
//  assert(true  == weekDays.areMembers(eDays));
//  assert(false == weekDays.areMembers(nDays));
//..
///Iterator Usage
/// - - - - - - -
// Finally observe that iteration order is defined by increasing enumerated
// 'bdet_DayOfWeek::Day' value '[ SUN .. SAT ]'.  The following use of the
// *forward* (bi-directional) iterator:
//..
//  for (bdec_DayOfWeekSet::iterator it  = ALL_DAYS.begin();
//                                   it != ALL_DAYS.end();
//                                 ++it) {
//      bsl::cout << *it << bsl::endl;
//  }
//..
// produces:
//..
//  SUN
//  MON
//  TUE
//  WED
//  THU
//  FRI
//  SAT
//..
// on standard output.  Similarly, the following use of the *reverse* iterator:
//..
//  for (bdec_DayOfWeekSet::reverse_iterator it  = weekDays.rbegin();
//                                           it != weekDays.rend();
//                                         ++it) {
//      bsl::cout << *it << bsl::endl;
//  }
//..
// produces:
//..
//  FRI
//  THU
//  WED
//  TUE
//  MON
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDES_BITUTIL
#include <bdes_bitutil.h>
#endif

#ifndef INCLUDED_BDET_DAYOFWEEK
#include <bdet_dayofweek.h>
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


namespace BloombergLP {

                          // ============================
                          // class bdec_DayOfWeekSet_Iter
                          // ============================

class bdec_DayOfWeekSet_Iter : public bsl::iterator<
                                               bsl::bidirectional_iterator_tag,
                                               const bdet_DayOfWeek::Day> {
    // Implementation of standard bidirectional iterator for
    // 'bdec_DayOfWeekSet'.

    // CLASS DATA
    static const bdet_DayOfWeek::Day *s_dayOfWeekArray_p;  // = { ???, SUN, ...

    // DATA
    char d_data;   // copy of days of the week from the original container
    char d_index;  // current position in the iteration; value '[1 .. 7]'

    // FRIENDS
    friend bool operator==(const bdec_DayOfWeekSet_Iter&,
                           const bdec_DayOfWeekSet_Iter&);

  public:
    // CREATORS
    bdec_DayOfWeekSet_Iter();
        // Create a default (invalid) iterator.

    bdec_DayOfWeekSet_Iter(int data, int index);
        // Create an iterator using the specified 'data' and 'index'.  If
        // 'index' is 1, this iterator references the first valid element of
        // 'data'; if 'index' is 8, then this iterator references one past the
        // last possible element in 'data'.

    bdec_DayOfWeekSet_Iter(const bdec_DayOfWeekSet_Iter& original);
        // Create an iterator having the value of the specified 'original'
        // iterator.

    ~bdec_DayOfWeekSet_Iter();
        // Destroy this iterator.

    // MANIPULATORS
    bdec_DayOfWeekSet_Iter& operator=(const bdec_DayOfWeekSet_Iter& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator.

    bdec_DayOfWeekSet_Iter& operator++();
        // Advance this iterator to the next valid data element.

    bdec_DayOfWeekSet_Iter& operator--();
        // Regress this iterator to the previous valid data element.

    // ACCESSORS
    const bdet_DayOfWeek::Day& operator*() const;
        // Return a reference to the day of week value referenced by this
        // iterator.
};

// FREE OPERATORS
bool operator==(const bdec_DayOfWeekSet_Iter& lhs,
                const bdec_DayOfWeekSet_Iter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators have the same
    // value, and 'false' otherwise.  Two iterators have the same value if they
    // refer to data at the same index position.  The behavior is undefined
    // unless 'lhs' and 'rhs' both reference into the same set of data.

bool operator!=(const bdec_DayOfWeekSet_Iter& lhs,
                const bdec_DayOfWeekSet_Iter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not have the
    // same value, and 'false' otherwise.  Two iterators do not have the same
    // value if they do not refer to data at the same index position.  The
    // behavior is undefined unless 'lhs' and 'rhs' both reference into the
    // same set of data.

bdec_DayOfWeekSet_Iter operator++(bdec_DayOfWeekSet_Iter& iterator, int);
    // Advance the specified 'iterator' to the next valid data element and
    // return the original 'iterator'.

bdec_DayOfWeekSet_Iter operator--(bdec_DayOfWeekSet_Iter& iterator, int);
    // Regress the specified 'iterator' to the previous valid data element and
    // return the original 'iterator'.

                          // =======================
                          // class bdec_DayOfWeekSet
                          // =======================

class bdec_DayOfWeekSet {
    // This class implements an efficient value-semantic, ordered set of
    // 'bdet_DayOfWeek' values.  This set requires a fixed capacity, and
    // all operations can be assumed to operate in constant time, and
    // provide the no-throw guarantee.

    // DATA
    unsigned char d_days;  // bits '1 .. 7' reflect '[ SUN, MON, ..., SAT ]';
                           // bit 0 is unused

    // FRIENDS
    friend bool operator==(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
    friend bool operator!=(const bdec_DayOfWeekSet&, const bdec_DayOfWeekSet&);
    friend bdec_DayOfWeekSet operator~(const bdec_DayOfWeekSet&);

  public:
    // TYPES
    typedef bdec_DayOfWeekSet_Iter iterator;
        // Standard nested alias for set container's iterator.

    typedef iterator const_iterator;
        // Standard nested alias for set container's constant iterator.

    typedef bsl::reverse_iterator<iterator> reverse_iterator;
        // Standard nested alias for set container's reverse iterator.

    typedef reverse_iterator const_reverse_iterator;
        // Standard nested alias for set container's constant reverse iterator.

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of container types.

    // CREATORS
    bdec_DayOfWeekSet();
        // Create an empty set.

    bdec_DayOfWeekSet(const bdec_DayOfWeekSet& original);
        // Create a set initialized to the value of the specified 'original'
        // set.

    ~bdec_DayOfWeekSet();
        // Destroy this object.

    // MANIPULATORS
    bdec_DayOfWeekSet& operator=(const bdec_DayOfWeekSet& rhs);
        // Assign to this set the value of the specified 'rhs' set.  Return a
        // reference to this modifiable set.

    bdec_DayOfWeekSet& operator|=(const bdec_DayOfWeekSet& rhs);
        // Assign to this set the union of this set with the specified 'rhs'
        // set (i.e., a set containing elements that are in either this set or
        // the 'rhs' set, or in both sets).  Return a reference to this
        // modifiable set.

    bdec_DayOfWeekSet& operator&=(const bdec_DayOfWeekSet& rhs);
        // Assign to this set the intersection of this set with the specified
        // 'rhs' set (i.e., a set containing elements that are in both this
        // set and the 'rhs' set).  Return a reference to this modifiable set.

    bdec_DayOfWeekSet& operator^=(const bdec_DayOfWeekSet& rhs);
        // Assign to this set the exclusive-or of this set with the specified
        // 'rhs' set (i.e., a set containing elements that are either in this
        // set, but not 'rhs' or in 'rhs', but not in this set).  Return a
        // reference to this modifiable set.

    bdec_DayOfWeekSet& operator-=(const bdec_DayOfWeekSet& rhs);
        // Assign to this set the subtraction of the specified 'rhs' set from
        // this set (i.e., a set containing elements that are in this set, but
        // not in the 'rhs' set).  Return a reference to this modifiable set.

    void add(bdet_DayOfWeek::Day value);
        // Add the specified 'value' to this set.

    int remove(bdet_DayOfWeek::Day value);
        // Remove the specified 'value' from this set.  Return 1 if the
        // 'value' was already a member of this set, and 0 otherwise.

    void removeAll();
        // Remove all members of this set.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.

    // ACCESSORS
    bool areMembers(const bdec_DayOfWeekSet& set) const;
        // Return 'true' if this set contains all elements of the specified
        // 'set', and 'false' otherwise.

    iterator begin() const;
        // Return an iterator referencing the first valid element in this set.

    iterator end() const;
        // Return an iterator indicating one position past the last possible
        // element in this set.

    bool isMember(bdet_DayOfWeek::Day value) const;
        // Return 'true' if the specified 'value' is an element of this set,
        // and 'false' otherwise.

    int length() const;
        // Return the number of elements in this set.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level' and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.

    reverse_iterator rbegin() const;
        // Return a reverse iterator referencing the last valid element in this
        // set.

    reverse_iterator rend() const;
        // Return a reverse iterator indicating one position before the first
        // possible element in this set.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.
};

// FREE OPERATORS
bdec_DayOfWeekSet operator~(const bdec_DayOfWeekSet& set);
    // Return a set containing the complement of the specified 'set'
    // (i.e., those members *not* contained in 'set').

bdec_DayOfWeekSet operator|(const bdec_DayOfWeekSet& lhs,
                            const bdec_DayOfWeekSet& rhs);
    // Return a set containing the union of the specified 'lhs' and 'rhs' sets
    // (i.e., a set containing elements that are in either 'lhs' or 'rhs' or
    // both).

bdec_DayOfWeekSet operator&(const bdec_DayOfWeekSet& lhs,
                            const bdec_DayOfWeekSet& rhs);
    // Return a set containing the intersection of the specified 'lhs' and
    // 'rhs' sets (i.e., a set containing elements that are in both 'lhs' and
    // 'rhs').

bdec_DayOfWeekSet operator^(const bdec_DayOfWeekSet& lhs,
                            const bdec_DayOfWeekSet& rhs);
    // Return a set containing the exclusive-or of the specified 'lhs' and
    // 'rhs' sets (i.e., a set containing elements that are either in 'lhs',
    // but not 'rhs' or in 'rhs', but not 'lhs').

bdec_DayOfWeekSet operator-(const bdec_DayOfWeekSet& lhs,
                            const bdec_DayOfWeekSet& rhs);
    // Return a set containing the subtraction of the specified 'rhs' from the
    // specified 'lhs' set (i.e., a set containing elements that are in 'lhs',
    // but not in 'rhs').

bool operator==(const bdec_DayOfWeekSet& lhs, const bdec_DayOfWeekSet& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' sets have the same value,
    // and 'false' otherwise.  Two sets have the same value if they have the
    // same length and all the elements of one set are members of the other
    // set.

bool operator!=(const bdec_DayOfWeekSet& lhs, const bdec_DayOfWeekSet& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' sets do not have the same
    // value, and 'false' otherwise.  Two sets do not have the same value if
    // they differ in length or there exists an element of one set that is not
    // a member of the other set.

bsl::ostream& operator<<(bsl::ostream& stream, const bdec_DayOfWeekSet& rhs);
    // Write the specified 'rhs' set to the specified output 'stream' in some
    // reasonable (single-line) format and return a reference to 'stream'.
    // Note that the order of the elements is implementation dependent.

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

                          // ----------------------------
                          // class bdec_DayOfWeekSet_Iter
                          // ----------------------------

// CREATORS
inline
bdec_DayOfWeekSet_Iter::bdec_DayOfWeekSet_Iter()
: d_data(0)
, d_index(8)
{
}

inline
bdec_DayOfWeekSet_Iter::bdec_DayOfWeekSet_Iter(
                                        const bdec_DayOfWeekSet_Iter& original)
: d_data(original.d_data)
, d_index(original.d_index)
{
}

inline
bdec_DayOfWeekSet_Iter::~bdec_DayOfWeekSet_Iter()
{
    BSLS_ASSERT_SAFE(0 == (d_data & 1));  // lsb is unused and always 0
}

// MANIPULATORS
inline
bdec_DayOfWeekSet_Iter&
bdec_DayOfWeekSet_Iter::operator=(const bdec_DayOfWeekSet_Iter& rhs)
{
    d_data  = rhs.d_data;
    d_index = rhs.d_index;
    return *this;
}

// ACCESSORS
inline
const bdet_DayOfWeek::Day& bdec_DayOfWeekSet_Iter::operator*() const
{
    return s_dayOfWeekArray_p[static_cast<int>(d_index)];
}

// FREE OPERATORS
inline
bool operator==(const bdec_DayOfWeekSet_Iter& lhs,
                const bdec_DayOfWeekSet_Iter& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_data == rhs.d_data);

    // If the data is not the same, either the objects were not initially
    // the same, or one has subsequently been modified.

    return lhs.d_index == rhs.d_index;
}

inline
bool operator!=(const bdec_DayOfWeekSet_Iter& lhs,
                const bdec_DayOfWeekSet_Iter& rhs)
{
    return !(lhs == rhs);
}

inline
bdec_DayOfWeekSet_Iter operator++(bdec_DayOfWeekSet_Iter& iterator, int)
{
    bdec_DayOfWeekSet_Iter tmp(iterator);
    iterator.operator++();
    return tmp;
}

inline
bdec_DayOfWeekSet_Iter operator--(bdec_DayOfWeekSet_Iter& iterator, int)
{
    bdec_DayOfWeekSet_Iter tmp(iterator);
    iterator.operator--();
    return tmp;
}

                          // -----------------------
                          // class bdec_DayOfWeekSet
                          // -----------------------

// CLASS METHODS
inline
int bdec_DayOfWeekSet::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
inline
bdec_DayOfWeekSet::bdec_DayOfWeekSet()
: d_days(0)
{
}

inline
bdec_DayOfWeekSet::bdec_DayOfWeekSet(const bdec_DayOfWeekSet& original)
: d_days(original.d_days)
{
}

inline
bdec_DayOfWeekSet::~bdec_DayOfWeekSet()
{
    BSLS_ASSERT_SAFE(!(d_days & 1));
}

// MANIPULATORS
inline
bdec_DayOfWeekSet& bdec_DayOfWeekSet::operator=(const bdec_DayOfWeekSet& rhs)
{
    d_days = rhs.d_days;
    return *this;
}

inline
bdec_DayOfWeekSet& bdec_DayOfWeekSet::operator|=(const bdec_DayOfWeekSet& rhs)
{
    d_days = static_cast<unsigned char>(d_days | rhs.d_days);
    return *this;
}

inline
bdec_DayOfWeekSet& bdec_DayOfWeekSet::operator&=(const bdec_DayOfWeekSet& rhs)
{
    d_days = static_cast<unsigned char>(d_days & rhs.d_days);
    return *this;
}

inline
bdec_DayOfWeekSet& bdec_DayOfWeekSet::operator^=(const bdec_DayOfWeekSet& rhs)
{
    d_days = static_cast<unsigned char>(d_days ^ rhs.d_days);
    return *this;
}

inline
bdec_DayOfWeekSet& bdec_DayOfWeekSet::operator-=(const bdec_DayOfWeekSet& rhs)
{
    const int mask = d_days & rhs.d_days;
    d_days = static_cast<unsigned char>(d_days - mask);
    return *this;
}

inline
void bdec_DayOfWeekSet::add(bdet_DayOfWeek::Day value)
{
    d_days = static_cast<unsigned char>(d_days | (1 << value));
}

inline
int bdec_DayOfWeekSet::remove(bdet_DayOfWeek::Day value)
{
    const int mask = 1 << value;
    const int rv   = mask == (d_days & mask);
    d_days = static_cast<unsigned char>(d_days & ~mask);
    return rv;
}

inline
void bdec_DayOfWeekSet::removeAll()
{
    d_days = 0;
}

template <class STREAM>
STREAM& bdec_DayOfWeekSet::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            unsigned char tmp;
            stream.getUint8(tmp);
            if (! stream) {
                break;
            }
            else if (tmp & 1) {
                stream.invalidate();  // corrupt data
            }
            else {
                d_days = tmp;
            }
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
bool bdec_DayOfWeekSet::areMembers(const bdec_DayOfWeekSet& set) const
{
    return set.d_days == (d_days & set.d_days);
}

inline
bdec_DayOfWeekSet::iterator bdec_DayOfWeekSet::begin() const
{
    return bdec_DayOfWeekSet::iterator(bdec_DayOfWeekSet_Iter(d_days, 1));
}

inline
bdec_DayOfWeekSet::iterator bdec_DayOfWeekSet::end() const
{
    return bdec_DayOfWeekSet::iterator(bdec_DayOfWeekSet_Iter(d_days, 8));
}

inline
bool bdec_DayOfWeekSet::isMember(bdet_DayOfWeek::Day value) const
{
    const int mask = 1 << value;
    return mask == (d_days & mask);
}

inline
int bdec_DayOfWeekSet::length() const
{
    return bdes_BitUtil::numSetOne(d_days);
}

inline
bdec_DayOfWeekSet::reverse_iterator bdec_DayOfWeekSet::rbegin() const
{
    return bdec_DayOfWeekSet::reverse_iterator(end());
}

inline
bdec_DayOfWeekSet::reverse_iterator bdec_DayOfWeekSet::rend() const
{
    return bdec_DayOfWeekSet::reverse_iterator(begin());
}

template <class STREAM>
STREAM& bdec_DayOfWeekSet::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putUint8(d_days);
      } break;
    }
    return stream;
}

// FREE OPERATORS
inline
bdec_DayOfWeekSet operator~(const bdec_DayOfWeekSet& set)
{
    bdec_DayOfWeekSet tmp(set);
    tmp.d_days =
         static_cast<unsigned char>(~tmp.d_days & bdes_BitUtil::oneMask(1, 7));
    return tmp;
}

inline
bool operator==(const bdec_DayOfWeekSet& lhs, const bdec_DayOfWeekSet& rhs)
{
    return lhs.d_days == rhs.d_days;
}

inline
bool operator!=(const bdec_DayOfWeekSet& lhs, const bdec_DayOfWeekSet& rhs)
{
    return lhs.d_days != rhs.d_days;
}

inline
bdec_DayOfWeekSet operator|(const bdec_DayOfWeekSet& lhs,
                            const bdec_DayOfWeekSet& rhs)
{
    return bdec_DayOfWeekSet(lhs) |= rhs;
}

inline
bdec_DayOfWeekSet operator&(const bdec_DayOfWeekSet& lhs,
                            const bdec_DayOfWeekSet& rhs)
{
    return bdec_DayOfWeekSet(lhs) &= rhs;
}

inline
bdec_DayOfWeekSet operator^(const bdec_DayOfWeekSet& lhs,
                            const bdec_DayOfWeekSet& rhs)
{
    return bdec_DayOfWeekSet(lhs) ^= rhs;
}

inline
bdec_DayOfWeekSet operator-(const bdec_DayOfWeekSet& lhs,
                            const bdec_DayOfWeekSet& rhs)
{
    return bdec_DayOfWeekSet(lhs) -= rhs;
}

inline
bsl::ostream& operator<<(bsl::ostream& stream, const bdec_DayOfWeekSet& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
