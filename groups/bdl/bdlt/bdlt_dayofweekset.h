// bdlt_dayofweekset.h                                                -*-C++-*-
#ifndef INCLUDED_BDLT_DAYOFWEEKSET
#define INCLUDED_BDLT_DAYOFWEEKSET

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide an ordered set of (unique) 'bdlt::DayOfWeek::Enum' values.
//
//@CLASSES:
//  bdlt::DayOfWeekSet: ordered set of (unique) 'bdlt::DayOfWeek::Enum' values
//
//@SEE_ALSO: bdlt_dayofweek
//
//@DESCRIPTION: This component implements an efficient value-semantic, ordered
// set class, 'bdlt::DayOfWeekSet', for elements of the 'bdlt::DayOfWeek::Enum'
// enumeration.  As there are only seven possible element values, asymptotic
// performance characterization is not appropriate; all operations implicitly
// run in constant time and provide the no-throw guarantee.
//
///Supplementary Overloaded Operators
///----------------------------------
// In addition to the standard value-semantic operators '=', '==', '!=', and
// '<<', the following canonical set of binary and unary (free) operators are
// defined on 'bdlt::DayOfWeekSet' objects:
//..
//                   set S: { e_MON, e_TUE, e_WED }
//                   set T: { e_MON, e_WED, e_FRI }
//
//           Union:  S | T  { e_MON, e_TUE, e_WED, e_FRI }
//
//    Intersection:  S & T  { e_MON, e_WED }
//
//    Exclusive Or:  S ^ T  { e_TUE, e_FRI }
//
//     Subtraction:  S - T  { e_TUE }
//                   T - S  { e_FRI }
//
//  Unary Negation:     ~S  { e_SUN, e_THU, e_FRI, e_SAT }
//                      ~T  { e_SUN, e_TUE, e_THU, e_SAT }
//..
// The corresponding assignment (member) operators '|=' , '&=', '^=', and '-='
// (but not '~=') are also provided.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Manipulation and Traversal of Day of Week Sets
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A 'bdlt::DayOfWeekSet' is useful for recording recurring appointments, or
// special days (e.g., weekend days), in a calendar.  The following snippets of
// code illustrate how to create and use a 'bdlt::DayOfWeek' set.
//
// First, we create a couple of commonly useful sets.  First we define the
// 'bdlt::DayOfWeekSet' 'weekendDays':
//..
//  bdlt::DayOfWeekSet weekendDays;
//..
// Then, we notice that this set is initially empty.
//..
//  assert(0 == weekendDays.length());
//..
// Next, we add the days that characterize weekends:
//..
//  weekendDays.add(bdlt::DayOfWeek::e_SUN);
//  assert(1 == weekendDays.length());
//
//  weekendDays.add(bdlt::DayOfWeek::e_SAT);
//  assert(2 == weekendDays.length());
//..
// Then, we observe that 'weekendDays' now contains precisely the days we
// expect it to contain:
//..
//  assert(true  == weekendDays.isMember(bdlt::DayOfWeek::e_SUN));
//  assert(false == weekendDays.isMember(bdlt::DayOfWeek::e_MON));
//  assert(false == weekendDays.isMember(bdlt::DayOfWeek::e_TUE));
//  assert(false == weekendDays.isMember(bdlt::DayOfWeek::e_WED));
//  assert(false == weekendDays.isMember(bdlt::DayOfWeek::e_THU));
//  assert(false == weekendDays.isMember(bdlt::DayOfWeek::e_FRI));
//  assert(true  == weekendDays.isMember(bdlt::DayOfWeek::e_SAT));
//..
// Next, we create the complementary 'bdlt::DayOfWeekSet' 'weekDays' directly
// from 'weekendDays' via a combination of unary negation and copy
// construction:
//..
//  bdlt::DayOfWeekSet weekDays(~weekendDays);
//
//  assert(5 == weekDays.length());
//
//  assert(false == weekDays.isMember(bdlt::DayOfWeek::e_SUN));
//  assert(true  == weekDays.isMember(bdlt::DayOfWeek::e_MON));
//  assert(true  == weekDays.isMember(bdlt::DayOfWeek::e_TUE));
//  assert(true  == weekDays.isMember(bdlt::DayOfWeek::e_WED));
//  assert(true  == weekDays.isMember(bdlt::DayOfWeek::e_THU));
//  assert(true  == weekDays.isMember(bdlt::DayOfWeek::e_FRI));
//  assert(false == weekDays.isMember(bdlt::DayOfWeek::e_SAT));
//..
// Then, to create a set containing all of the days in the week, we do so via
// unary negation of the default constructed value:
//..
//  const bdlt::DayOfWeekSet NO_DAYS;
//  const bdlt::DayOfWeekSet ALL_DAYS(~NO_DAYS);
//
//  assert(7 == ALL_DAYS.length());
//..
// Next, we observe that neither 'weekDays' nor 'weekendDays' represent the
// same value as 'ALL_DAYS', but their union does:
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
// Then, we observe that similarly, neither 'weekDays' nor 'weekendDays'
// represents the same value as 'NO_DAYS', but their intersection does:
//..
//  assert(NO_DAYS != weekendDays);
//  assert(NO_DAYS != weekDays);
//  assert(NO_DAYS == (weekDays & weekendDays));
//
//  assert(weekendDays == weekendDays - weekDays);
//
//  assert(weekDays    == weekDays - weekendDays);
//..
// Next, we create the corresponding set 'eDays' consisting of the only days of
// the week that have an 'E' in them: 'TUESDAY' and 'WEDNESDAY':
//..
//  bdlt::DayOfWeekSet eDays;                 assert(0 == eDays.length());
//  eDays.add(bdlt::DayOfWeek::e_TUE);     assert(1 == eDays.length());
//  eDays.add(bdlt::DayOfWeek::e_WED);     assert(2 == eDays.length());
//
//  assert(false == eDays.isMember(bdlt::DayOfWeek::e_SUN));
//  assert(false == eDays.isMember(bdlt::DayOfWeek::e_MON));
//  assert(true  == eDays.isMember(bdlt::DayOfWeek::e_TUE));
//  assert(true  == eDays.isMember(bdlt::DayOfWeek::e_WED));
//  assert(false == eDays.isMember(bdlt::DayOfWeek::e_THU));
//  assert(false == eDays.isMember(bdlt::DayOfWeek::e_FRI));
//  assert(false == eDays.isMember(bdlt::DayOfWeek::e_SAT));
//..
// Then, we create a set consisting of days that have an 'n' in them: 'MONDAY',
// 'WEDNESDAY', and 'SUNDAY'.  We create the corresponding set 'nDays' starting
// with the value of 'eDays' by first removing 'TUESDAY', and then adding
// 'SUNDAY' and 'MONDAY':
//..
//  bdlt::DayOfWeekSet nDays(eDays);          assert(2 == nDays.length());
//
//  nDays.remove(bdlt::DayOfWeek::e_TUE);  assert(1 == nDays.length());
//
//  nDays.add(bdlt::DayOfWeek::e_SUN);     assert(2 == nDays.length());
//  nDays.add(bdlt::DayOfWeek::e_MON);     assert(3 == nDays.length());
//
//  assert(true  == nDays.isMember(bdlt::DayOfWeek::e_SUN));
//  assert(true  == nDays.isMember(bdlt::DayOfWeek::e_MON));
//  assert(false == nDays.isMember(bdlt::DayOfWeek::e_TUE));
//  assert(true  == nDays.isMember(bdlt::DayOfWeek::e_WED));
//  assert(false == nDays.isMember(bdlt::DayOfWeek::e_THU));
//  assert(false == nDays.isMember(bdlt::DayOfWeek::e_FRI));
//  assert(false == nDays.isMember(bdlt::DayOfWeek::e_SAT));
//..
// Next, we observe that all 'eDays' are 'weekDays', but that's not true of
// 'nDays':
//..
//  assert(true  == weekDays.areMembers(eDays));
//  assert(false == weekDays.areMembers(nDays));
//..
// Now, we observe that iteration order is defined by increasing enumerated
// 'bdlt::DayOfWeek::Day' value '[ SUN .. SAT ]'.  The following use of the
// *forward* (bi-directional) iterator:
//..
//  for (bdlt::DayOfWeekSet::iterator it  = ALL_DAYS.begin();
//                                    it != ALL_DAYS.end();
//                                    ++it) {
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
// on standard output.
//
// Finally, we observe that, similarly, the following use of the *reverse*
// iterator:
//..
//  for (bdlt::DayOfWeekSet::reverse_iterator it  = weekDays.rbegin();
//                                            it != weekDays.rend();
//                                            ++it) {
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

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_DAYOFWEEK
#include <bdlt_dayofweek.h>
#endif

#ifndef INCLUDED_BDLB_BITUTIL
#include <bdlb_bitutil.h>
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
namespace bdlt {

                          // =======================
                          // class DayOfWeekSet_Iter
                          // =======================

class DayOfWeekSet_Iter : public bsl::iterator<bsl::bidirectional_iterator_tag,
                                               const DayOfWeek::Enum> {
    // Implementation of standard bidirectional iterator for 'DayOfWeekSet'.
    // Any modification of a 'DayOfWeekSet' will invalidate any iterators
    // referring to that 'DayOfWeekSet'.

    // CLASS DATA
    static const DayOfWeek::Enum s_dayOfWeekArray[9];  // = { ???, SUN, ...

    // DATA
    unsigned char d_data;       // copy of days of the week from the original
                                // container
    signed char   d_index;      // current position in the iteration; value
                                // '[0 .. 8]'

    // FRIENDS
    friend bool operator==(const DayOfWeekSet_Iter&, const DayOfWeekSet_Iter&);

  public:
    // CREATORS
    DayOfWeekSet_Iter();
        // Create a default (invalid) iterator.

    DayOfWeekSet_Iter(int data, int index);
        // Create an iterator using the specified 'data' and 'index'.  If
        // 'index' is 1, this iterator references the first valid element of
        // 'data'; if 'index' is 8, then this iterator references one past the
        // last possible element in 'data'.  The behavior is undefined unless
        // '0 == (data & 1)', 'index >= 0', and 'index <= 8'.

    DayOfWeekSet_Iter(const DayOfWeekSet_Iter& original);
        // Create an iterator having the value of the specified 'original'
        // iterator.

    ~DayOfWeekSet_Iter();
        // Destroy this iterator.

    // MANIPULATORS
    DayOfWeekSet_Iter& operator=(const DayOfWeekSet_Iter& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator,
        // and return a reference providing modifiable access to this iterator.

    DayOfWeekSet_Iter& operator++();
        // Advance this iterator to the next valid data element, and return a
        // reference providing modifiable access to this iterator.  If there is
        // no next valid data element, this iterator will be set equal to
        // 'end()'.

    DayOfWeekSet_Iter operator++(int);
        // Advance this iterator to the next valid data element, and return by
        // value the value of this iterator before it was incremented.  If
        // there is no next valid data element, this iterator will be set equal
        // to 'end()'.

    DayOfWeekSet_Iter& operator--();
        // Regress this iterator to the previous valid data element, and return
        // a reference providing modifiable access to this iterator.  If there
        // is no preceding data element, the value of 'reverse_iterator(*this)'
        // will be 'rend()'.

    DayOfWeekSet_Iter operator--(int);
        // Regress this iterator to the previous valid data element, and return
        // by value the value of this iterator before it was decremented.  If
        // there is no preceding data element, the value of
        // 'reverse_iterator(*this)' will be 'rend()'.

    // ACCESSORS
    const DayOfWeek::Enum& operator*() const;
        // Return a reference providing non-modifiable access to the day of
        // week value referenced by this iterator.  The behavior is undefined
        // unless the iterator refers to a valid day of the week, specifically,
        // the behavior is undefined if '*this == end()'.
};

// FREE OPERATORS
bool operator==(const DayOfWeekSet_Iter& lhs, const DayOfWeekSet_Iter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators have the same
    // value, and 'false' otherwise.  Two iterators have the same value if they
    // refer to data at the same index position.  The behavior is undefined
    // unless 'lhs' and 'rhs' both reference into the same set of data.

bool operator!=(const DayOfWeekSet_Iter& lhs, const DayOfWeekSet_Iter& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not have the
    // same value, and 'false' otherwise.  Two iterators do not have the same
    // value if they do not refer to data at the same index position.  The
    // behavior is undefined unless 'lhs' and 'rhs' both reference into the
    // same set of data.

                          // ==================
                          // class DayOfWeekSet
                          // ==================

class DayOfWeekSet {
    // This class implements an efficient value-semantic, ordered set of
    // 'DayOfWeek' values.  This set requires a fixed capacity, and all
    // operations operate in constant time, and provide the no-throw guarantee.

    // DATA
    unsigned char d_days;  // bits '1 .. 7' reflect '[ SUN, MON, ..., SAT ]';
                           // bit 0 is unused

    // FRIENDS
    friend bool operator==(const DayOfWeekSet&, const DayOfWeekSet&);
    friend bool operator!=(const DayOfWeekSet&, const DayOfWeekSet&);
    friend DayOfWeekSet operator~(const DayOfWeekSet&);

  public:
    // TYPES
    typedef DayOfWeekSet_Iter iterator;
        // Standard nested alias for set container's iterator.

    typedef iterator const_iterator;
        // Standard nested alias for set container's constant iterator.

    typedef bsl::reverse_iterator<iterator> reverse_iterator;
        // Standard nested alias for set container's reverse iterator.

    typedef reverse_iterator const_reverse_iterator;
        // Standard nested alias for set container's constant reverse iterator.

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
    DayOfWeekSet();
        // Create an empty set.

    DayOfWeekSet(const DayOfWeekSet& original);
        // Create a set initialized to the value of the specified 'original'
        // set.

    ~DayOfWeekSet();
        // Destroy this object.

    // MANIPULATORS
    DayOfWeekSet& operator=(const DayOfWeekSet& rhs);
        // Assign to this set the value of the specified 'rhs' set, and return
        // a reference providing modifiable access to this set.

    DayOfWeekSet& operator|=(const DayOfWeekSet& rhs);
        // Assign to this set the union of this set with the specified 'rhs'
        // set (i.e., a set containing elements that are in either this set or
        // the 'rhs' set, or in both sets), and return a reference providing
        // modifiable access to this set.

    DayOfWeekSet& operator&=(const DayOfWeekSet& rhs);
        // Assign to this set the intersection of this set with the specified
        // 'rhs' set (i.e., a set containing elements that are in both this
        // set and the 'rhs' set), and return a reference providing modifiable
        // access to this set.

    DayOfWeekSet& operator^=(const DayOfWeekSet& rhs);
        // Assign to this set the exclusive-or of this set with the specified
        // 'rhs' set (i.e., a set containing elements that are either in this
        // set, but not 'rhs', or in 'rhs', but not in this set), and return a
        // reference providing modifiable access to this set.

    DayOfWeekSet& operator-=(const DayOfWeekSet& rhs);
        // Assign to this set the subtraction of the specified 'rhs' set from
        // this set (i.e., a set containing elements that are in this set, but
        // not in the 'rhs' set), and return a reference providing modifiable
        // access to this set.

    void add(DayOfWeek::Enum value);
        // Add the specified 'value' to this set.

    bool remove(DayOfWeek::Enum value);
        // Remove the specified 'value' from this set.  Return 'true' if
        // 'value' was a member of this set, and 'false' otherwise.

    void removeAll();
        // Remove all members of this set.

                                  // Aspects

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this object the value read from the specified input
        // 'stream' using the specified 'version' format, and return a
        // reference to 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'version' is not supported, this object
        // is unaltered and 'stream' is invalidated, but otherwise unmodified.
        // If 'version' is supported but 'stream' becomes invalid during this
        // operation, this object has an undefined, but valid, state.  Note
        // that no version is read from 'stream'.  See the 'bslx' package-level
        // documentation for more information on BDEX streaming of
        // value-semantic types and containers.

    // ACCESSORS
    bool areMembers(const DayOfWeekSet& set) const;
        // Return 'true' if this set contains all elements of the specified
        // 'set', and 'false' otherwise.

    iterator begin() const;
        // Return an iterator referencing the first valid element in this set.

    iterator end() const;
        // Return an iterator indicating one position past the last possible
        // element in this set.

    bool isEmpty() const;
        // Return 'true' if there are no elements in this set, and 'false'
        // otherwise.

    bool isMember(DayOfWeek::Enum value) const;
        // Return 'true' if the specified 'value' is an element of this set,
        // and 'false' otherwise.

    int length() const;
        // Return the number of elements in this set.

    reverse_iterator rbegin() const;
        // Return a reverse iterator referencing the last valid element in this
        // set.

    reverse_iterator rend() const;
        // Return a reverse iterator indicating one position before the first
        // possible element in this set.

                                  // Aspects

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write the value of this object, using the specified 'version'
        // format, to the specified output 'stream', and return a reference to
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported, 'stream' is invalidated, but
        // otherwise unmodified.  Note that 'version' is not written to
        // 'stream'.  See the 'bslx' package-level documentation for more
        // information on BDEX streaming of value-semantic types and
        // containers.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Format this object to the specified output 'stream' at the (absolute
        // value of) the optionally specified indentation 'level', and return a
        // reference to 'stream'.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', the number of spaces per indentation level for
        // this and all of its nested objects.  If 'level' is negative,
        // suppress indentation of the first line.  If 'spacesPerLevel' is
        // negative, format the entire output on one line, suppressing all but
        // the initial indentation (as governed by 'level').  If 'stream' is
        // not valid on entry, this operation has no effect.


};

// FREE OPERATORS
DayOfWeekSet operator~(const DayOfWeekSet& set);
    // Return a set containing the complement of the specified 'set' (i.e.,
    // those members *not* contained in 'set').

DayOfWeekSet operator|(const DayOfWeekSet& lhs, const DayOfWeekSet& rhs);
    // Return a set containing the union of the specified 'lhs' and 'rhs' sets
    // (i.e., a set containing elements that are in either 'lhs' or 'rhs' or
    // both).

DayOfWeekSet operator&(const DayOfWeekSet& lhs, const DayOfWeekSet& rhs);
    // Return a set containing the intersection of the specified 'lhs' and
    // 'rhs' sets (i.e., a set containing elements that are in both 'lhs' and
    // 'rhs').

DayOfWeekSet operator^(const DayOfWeekSet& lhs, const DayOfWeekSet& rhs);
    // Return a set containing the exclusive-or of the specified 'lhs' and
    // 'rhs' sets (i.e., a set containing elements that are either in 'lhs',
    // but not 'rhs', or in 'rhs', but not 'lhs').

DayOfWeekSet operator-(const DayOfWeekSet& lhs, const DayOfWeekSet& rhs);
    // Return a set containing the subtraction of the specified 'rhs' set from
    // the specified 'lhs' set (i.e., a set containing elements that are in
    // 'lhs', but not in 'rhs').

bool operator==(const DayOfWeekSet& lhs, const DayOfWeekSet& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' sets have the same value,
    // and 'false' otherwise.  Two sets have the same value if they have the
    // same length and all the elements of one set are members of the other
    // set.

bool operator!=(const DayOfWeekSet& lhs, const DayOfWeekSet& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' sets do not have the same
    // value, and 'false' otherwise.  Two sets do not have the same value if
    // they differ in length or there exists an element of one set that is not
    // a member of the other set.

bsl::ostream& operator<<(bsl::ostream& stream, const DayOfWeekSet& rhs);
    // Write the specified 'rhs' set to the specified output 'stream' in some
    // reasonable (single-line) format, and return a reference to 'stream'.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // -----------------------
                          // class DayOfWeekSet_Iter
                          // -----------------------

// CREATORS
inline
DayOfWeekSet_Iter::DayOfWeekSet_Iter()
: d_data(0)
, d_index(8)
{
}

inline
DayOfWeekSet_Iter::DayOfWeekSet_Iter(const DayOfWeekSet_Iter& original)
: d_data(original.d_data)
, d_index(original.d_index)
{
}

inline
DayOfWeekSet_Iter::~DayOfWeekSet_Iter()
{
    BSLS_ASSERT_SAFE(0 == (d_data & 1));  // lsb is unused and always 0
    BSLS_ASSERT_SAFE(d_index >= 0);
    BSLS_ASSERT_SAFE(d_index <= 8);
}

// MANIPULATORS
inline
DayOfWeekSet_Iter&
DayOfWeekSet_Iter::operator=(const DayOfWeekSet_Iter& rhs)
{
    d_data  = rhs.d_data;
    d_index = rhs.d_index;
    return *this;
}

inline
DayOfWeekSet_Iter DayOfWeekSet_Iter::operator++(int)
{
    DayOfWeekSet_Iter tmp(*this);
    this->operator++();
    return tmp;
}

inline
DayOfWeekSet_Iter DayOfWeekSet_Iter::operator--(int)
{
    DayOfWeekSet_Iter tmp(*this);
    this->operator--();
    return tmp;
}

// ACCESSORS
inline
const DayOfWeek::Enum& DayOfWeekSet_Iter::operator*() const
{
    BSLS_ASSERT_SAFE(d_index >= 1);
    BSLS_ASSERT_SAFE(d_index <= 7);

    return s_dayOfWeekArray[d_index];
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const DayOfWeekSet_Iter& lhs,
                      const DayOfWeekSet_Iter& rhs)
{
    BSLS_ASSERT_SAFE(lhs.d_data == rhs.d_data);

    // If the data is not the same, either the objects were not initially the
    // same, or one has subsequently been modified.

    return lhs.d_index == rhs.d_index;
}

inline
bool bdlt::operator!=(const DayOfWeekSet_Iter& lhs,
                      const DayOfWeekSet_Iter& rhs)
{
    return !(lhs == rhs);
}

namespace bdlt {

                             // ------------------
                             // class DayOfWeekSet
                             // ------------------

// CLASS METHODS

                                  // Aspects

inline
int DayOfWeekSet::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// CREATORS
inline
DayOfWeekSet::DayOfWeekSet()
: d_days(0)
{
}

inline
DayOfWeekSet::DayOfWeekSet(const DayOfWeekSet& original)
: d_days(original.d_days)
{
}

inline
DayOfWeekSet::~DayOfWeekSet()
{
    BSLS_ASSERT_SAFE(!(d_days & 1));
}

// MANIPULATORS
inline
DayOfWeekSet& DayOfWeekSet::operator=(const DayOfWeekSet& rhs)
{
    d_days = rhs.d_days;
    return *this;
}

inline
DayOfWeekSet& DayOfWeekSet::operator|=(const DayOfWeekSet& rhs)
{
    d_days = static_cast<unsigned char>(d_days | rhs.d_days);
    return *this;
}

inline
DayOfWeekSet& DayOfWeekSet::operator&=(const DayOfWeekSet& rhs)
{
    d_days = static_cast<unsigned char>(d_days & rhs.d_days);
    return *this;
}

inline
DayOfWeekSet& DayOfWeekSet::operator^=(const DayOfWeekSet& rhs)
{
    d_days = static_cast<unsigned char>(d_days ^ rhs.d_days);
    return *this;
}

inline
DayOfWeekSet& DayOfWeekSet::operator-=(const DayOfWeekSet& rhs)
{
    const int mask = d_days & rhs.d_days;
    d_days = static_cast<unsigned char>(d_days - mask);
    return *this;
}

inline
void DayOfWeekSet::add(DayOfWeek::Enum value)
{
    d_days = static_cast<unsigned char>(d_days | (1 << value));
}

inline
bool DayOfWeekSet::remove(DayOfWeek::Enum value)
{
    const int  mask = 1 << value;
    const bool rv   = d_days & mask;
    d_days &= static_cast<unsigned char>(~mask);
    return rv;
}

inline
void DayOfWeekSet::removeAll()
{
    d_days = 0;
}

                                  // Aspects

template <class STREAM>
STREAM& DayOfWeekSet::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            unsigned char tmp;
            stream.getUint8(tmp);

            if (stream && 0 == (tmp & 1)) {
                d_days = tmp;
            }
            else {
                stream.invalidate();
            }
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

// ACCESSORS
inline
bool DayOfWeekSet::areMembers(const DayOfWeekSet& set) const
{
    return set.d_days == (d_days & set.d_days);
}

inline
DayOfWeekSet::iterator DayOfWeekSet::begin() const
{
    return DayOfWeekSet::iterator(DayOfWeekSet_Iter(d_days, 1));
}

inline
DayOfWeekSet::iterator DayOfWeekSet::end() const
{
    return DayOfWeekSet::iterator(DayOfWeekSet_Iter(d_days, 8));
}

inline
bool DayOfWeekSet::isEmpty() const
{
    return 0 == d_days;
}

inline
bool DayOfWeekSet::isMember(DayOfWeek::Enum value) const
{
    const int mask = 1 << value;
    return mask == (d_days & mask);
}

inline
int DayOfWeekSet::length() const
{
    return bdlb::BitUtil::numBitsSet(static_cast<unsigned int>(d_days));
}

inline
DayOfWeekSet::reverse_iterator DayOfWeekSet::rbegin() const
{
    return DayOfWeekSet::reverse_iterator(end());
}

inline
DayOfWeekSet::reverse_iterator DayOfWeekSet::rend() const
{
    return DayOfWeekSet::reverse_iterator(begin());
}

                                  // Aspects

template <class STREAM>
STREAM& DayOfWeekSet::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            stream.putUint8(d_days);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}


}  // close package namespace

// FREE OPERATORS
inline
bdlt::DayOfWeekSet bdlt::operator~(const DayOfWeekSet& set)
{
    DayOfWeekSet tmp(set);
    tmp.d_days = static_cast<unsigned char>(~tmp.d_days & 0xfe);
    return tmp;
}

inline
bool bdlt::operator==(const DayOfWeekSet& lhs, const DayOfWeekSet& rhs)
{
    return lhs.d_days == rhs.d_days;
}

inline
bool bdlt::operator!=(const DayOfWeekSet& lhs, const DayOfWeekSet& rhs)
{
    return lhs.d_days != rhs.d_days;
}

inline
bdlt::DayOfWeekSet bdlt::operator|(const DayOfWeekSet& lhs,
                                   const DayOfWeekSet& rhs)
{
    return DayOfWeekSet(lhs) |= rhs;
}

inline
bdlt::DayOfWeekSet bdlt::operator&(const DayOfWeekSet& lhs,
                                   const DayOfWeekSet& rhs)
{
    return DayOfWeekSet(lhs) &= rhs;
}

inline
bdlt::DayOfWeekSet bdlt::operator^(const DayOfWeekSet& lhs,
                                   const DayOfWeekSet& rhs)
{
    return DayOfWeekSet(lhs) ^= rhs;
}

inline
bdlt::DayOfWeekSet bdlt::operator-(const DayOfWeekSet& lhs,
                                   const DayOfWeekSet& rhs)
{
    return DayOfWeekSet(lhs) -= rhs;
}

inline
bsl::ostream& bdlt::operator<<(bsl::ostream& stream, const DayOfWeekSet& rhs)
{
    return rhs.print(stream, 0, -1);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2014 Bloomberg Finance L.P.
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
