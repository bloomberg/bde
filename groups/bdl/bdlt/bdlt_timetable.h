// bdlt_timetable.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLT_TIMETABLE
#define INCLUDED_BDLT_TIMETABLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a repository for accessing timetable information.
//
//@CLASSES:
//  bdlt::Timetable: repository for accessing timetable information
//  bdlt::TimetableTransition: datetime and transition code value
//
//@DESCRIPTION: This component provides a value-semantic class,
// 'bdlt::Timetable', that represents a timetable of state transitions over a
// *valid* *range* of dates, an associated iterator,
// 'bdlt::Timetable::const_iterator', that provides non-modifiable access to
// the timetable's state transitions, and a class, 'bdlt::TimetableTransition',
// that represents a change of state at a datetime.
//
// 'bdlt::Timetable' is designed to be especially efficient at determining the
// state in effect at a given 'bdlt::Datetime' value (within the valid range
// for a particular 'bdlt::Timetable' object), and iterating through the state
// transitions.
//
// 'bdlt::TimetableTransition' consists of a 'bdlt::Datetime' and a (single)
// non-negative integral code (of type 'int') that defines the "state" that
// becomes effective at that datetime.  The meaning of the integral code
// ascribed to each transition is defined by the client.  There can be at most
// one 'bdlt::TimetableTransition' defined for any datetime value within the
// range of a 'bdlt::Timetable'.  Consequently, there is at most one
// (client-defined) state in effect at any datetime in a timetable.
//
// Default-constructed timetables are empty, and have an empty valid range.
// Timetables can also be constructed with an initial (non-empty) valid range.
// The 'setValidRange' method modifies the valid range of a timetable, and a
// suite of "add" methods can be used to populate a timetable with state
// transitions.
//
// Timetables are value-semantic objects, and, as such, necessarily support all
// of the standard value-semantic operations, such as default construction,
// copy construction and copy assignment, and equality comparison.
//
///Exception-Safety Guarantees
///---------------------------
// All methods of 'bdlt::Timetable' are exception-safe, but in general provide
// only the basic guarantee (i.e., no guarantee of rollback): If an exception
// occurs (i.e., while attempting to allocate memory), the timetable object is
// left in a coherent state, but (unless otherwise specified) its *value* is
// undefined.
//
// All methods of 'bdlt::TimetableTransition' are exception-safe.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: 'Exchange Schedule'
/// - - - - - - - - - - - - - - -
// Suppose we want to track the open and close times for an exchange.  Most
// Mondays (and Tuesdays, Wednesdays, etc.) will have the same schedule,
// although some may differ.  We can use 'bdlt::Timetable' to efficiently store
// this data.
//
// First, we create an instance of 'bdlt::Timetable' with the desired valid
// range:
//..
//  bdlt::Timetable timetable(bdlt::Date(2018, 1, 1),
//                            bdlt::Date(2018, 12, 31));
//..
// Then, we define the codes for start-of-trading and end-of-trading and
// populate the typical transitions into the timetable:
//..
//  const int k_TRADING    = 0;
//  const int k_NO_TRADING = 1;
//
//  timetable.setInitialTransitionCode(k_NO_TRADING);
//
//  for (int i = 0; i < 5; ++ i) {
//      timetable.addTransitions(static_cast<bdlt::DayOfWeek::Enum>(
//                                                 bdlt::DayOfWeek::e_MON + i),
//                               bdlt::Time(8, 30),
//                               k_TRADING,
//                               timetable.firstDate(),
//                               timetable.lastDate());
//
//      timetable.addTransitions(static_cast<bdlt::DayOfWeek::Enum>(
//                                                 bdlt::DayOfWeek::e_MON + i),
//                               bdlt::Time(16, 30),
//                               k_NO_TRADING,
//                               timetable.firstDate(),
//                               timetable.lastDate());
//  }
//..
// Next, we add a holiday on January 19, 2018:
//..
//  timetable.removeTransitions(bdlt::Date(2018, 1, 19));
//..
// Then, we add a half-day on November 23, 2018:
//..
//  timetable.addTransition(bdlt::Datetime(2018, 11, 23, 12, 30),
//                          k_NO_TRADING);
//
//  timetable.removeTransition(bdlt::Datetime(2018, 11, 23, 16, 30));
//..
// Finally, we verify the transition code in effect at a few datetimes.
//..
//  assert(k_NO_TRADING == timetable.transitionCodeInEffect(
//                                      bdlt::Datetime(2018,  1, 15,  8,  0)));
//
//  assert(k_TRADING    == timetable.transitionCodeInEffect(
//                                      bdlt::Datetime(2018,  1, 15,  8, 30)));
//
//  assert(k_TRADING    == timetable.transitionCodeInEffect(
//                                      bdlt::Datetime(2018,  1, 15, 16,  0)));
//
//  assert(k_NO_TRADING == timetable.transitionCodeInEffect(
//                                      bdlt::Datetime(2018,  1, 15, 16, 30)));
//
//  assert(k_NO_TRADING == timetable.transitionCodeInEffect(
//                                      bdlt::Datetime(2018, 11, 23,  8,  0)));
//
//  assert(k_TRADING    == timetable.transitionCodeInEffect(
//                                      bdlt::Datetime(2018, 11, 23,  8, 30)));
//
//  assert(k_TRADING    == timetable.transitionCodeInEffect(
//                                      bdlt::Datetime(2018, 11, 23, 12,  0)));
//
//  assert(k_NO_TRADING == timetable.transitionCodeInEffect(
//                                      bdlt::Datetime(2018, 11, 23, 12, 30)));
//..

#include <bdlscm_version.h>

#include <bdlt_date.h>
#include <bdlt_datetime.h>
#include <bdlt_dayofweek.h>
#include <bdlt_time.h>

#include <bdlc_compactedarray.h>

#include <bslalg_swaputil.h>

#include <bslh_hash.h>

#include <bslma_allocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bsls_assert.h>
#include <bsls_review.h>

#include <bsl_algorithm.h>
#include <bsl_cstddef.h>
#include <bsl_iosfwd.h>
#include <bsl_vector.h>

namespace BloombergLP {
namespace bdlt {

// FORWARD DECLARATIONS
class Timetable;
class Timetable_Day;
class Timetable_ConstIterator;

                        // =========================
                        // class TimetableTransition
                        // =========================

class TimetableTransition {
    // This simply-constrained attribute class represents a state transition,
    // implemented as a datetime for when the transition occurs, and a code to
    // indicate the new state.

    // DATA
    Datetime d_datetime;  // datetime of the transition
    int      d_code;      // code in effect at, and after, 'd_datetime'

    // FRIENDS
    friend class TimetableTransition_Ref;
    friend class Timetable_ConstIterator;

  private:
    // PRIVATE CREATORS
    TimetableTransition();
        // Create a 'TimetableTransition' having datetime value
        // 'Datetime(Date())' and code 'k_UNSET_TRANSITION_CODE'.

    TimetableTransition(const Datetime& datetime, int code);
        // Create a 'TimetableTransition' having the specified 'datetime' and
        // 'code'.  The behavior is undefined unless '24 > datetime.hour()' and
        // '0 <= code || k_UNSET_TRANSITION_CODE == code'.

  public:
    // CONSTANTS
    enum { k_UNSET_TRANSITION_CODE = -1 };  // value representing an unset
                                            // transition code

    // CREATORS
    TimetableTransition(const TimetableTransition& original);
        // Create a 'TimetableTransition' having the same value as the
        // specified 'original' object.

    //! ~TimetableTransition() = default;
        // Destroy this object.

    // MANIPULATORS
    TimetableTransition& operator=(const TimetableTransition& rhs);
        // Assign to this object the value of the specified 'rhs' timetable
        // transition, and return a reference providing modifiable access to
        // this object.

    // ACCESSORS
    const Datetime& datetime() const;
        // Return the datetime of this transition.

    int code() const;
        // Return the code of this transition.

                             // Aspects

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
};

// FREE OPERATORS
bool operator==(const TimetableTransition& lhs,
                const TimetableTransition& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' timetable transitions
    // have the same value, and 'false' otherwise.  Two timetable transitions
    // have the same value if they have the same datetime and code.

bool operator!=(const TimetableTransition& lhs,
                const TimetableTransition& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' timetable transitions do
    // not have the same value, and 'false' otherwise.  Two timetable
    // transitions do not have the same value if they do not have the same
    // datetime or the same code.

bool operator<(const TimetableTransition& lhs,
               const TimetableTransition& rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs', and 'false' otherwise.  Timetable transition 'lhs' has a value
    // less than timetable transition 'rhs' if
    // 'lhs.datetime() < rhs.datetime()', or 'lhs.datetime() == rhs.datetime()'
    // and 'lhs.code() < rhs.code()'.

bool operator<(const TimetableTransition& lhs, const Datetime& rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs', and 'false' otherwise.  Timetable transition 'lhs' has a value
    // less than datetime 'rhs' if 'lhs.datetime() < rhs'.  The behavior is
    // undefined unless '24 > rhs.hour()'.

bool operator<(const Datetime& lhs, const TimetableTransition& rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs', and 'false' otherwise.  Datetime 'lhs' has a value less than
    // timetable transition 'rhs' if 'lhs < rhs.datetime()'.  The behavior is
    // undefined unless '24 > lhs.hour()'.

// HASH SPECIALIZATIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const TimetableTransition& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'TimetableTransition'.

                      // =============================
                      // class TimetableTransition_Ref
                      // =============================

class TimetableTransition_Ref : public TimetableTransition {
    // This private class is used by the arrow operator of the timetable
    // iterator class.  The objects instantiated from this class serve as
    // references to 'TimetableTransition' objects.

    friend class Timetable_ConstIterator;

  private:
    // NOT IMPLEMENTED
    TimetableTransition_Ref& operator=(const TimetableTransition_Ref&);

    // PRIVATE CREATORS
    TimetableTransition_Ref();
        // Create a timetable transition reference object using the default
        // 'TimetableTransition' constructor.

  public:
    // CREATORS
    explicit TimetableTransition_Ref(const TimetableTransition& transition);
        // Create a timetable transition reference object using the specified
        // 'transition'.

    TimetableTransition_Ref(const TimetableTransition_Ref& original);
        // Create a timetable transition reference object having the value of
        // the specified 'original' object.

    //! ~TimetableTransition_Ref() = default;
        // Destroy this object.

    // MANIPULATORS
    TimetableTransition_Ref& operator=(const TimetableTransition& rhs);
        // Assign to this object the value of the specified 'rhs' timetable
        // transition, and return a reference providing modifiable access to
        // this 'TimetableTransition_Ref'.
};

                  // =====================================
                  // class Timetable_CompactableTransition
                  // =====================================

class Timetable_CompactableTransition {
    // This simply-constrained attribute class represents a state transition,
    // implemented as a time for when the transition occurs, and a code to
    // indicate the new state.

    // DATA
    Time d_time;  // time of the transition
    int  d_code;  // code in effect at, and after, 'd_time'

    // FRIENDS
    friend class Timetable;
    friend class Timetable_Day;

  public:
    // CONSTANTS
    enum { k_UNSET_TRANSITION_CODE =
                                TimetableTransition::k_UNSET_TRANSITION_CODE };
                                            // value representing an unset
                                            // transition code

    // CREATORS
    Timetable_CompactableTransition();
        // Create a 'Timetable_CompactableTransition' having time value
        // 'Time(0)' and code 'k_UNSET_TRANSITION_CODE'.

    Timetable_CompactableTransition(const Time& time, int code);
        // Create a 'Timetable_CompactableTransition' having the specified
        // 'time' and 'code'.  The behavior is undefined unless
        // '24 > time.hour()' and
        // '0 <= code || k_UNSET_TRANSITION_CODE == code'.

    Timetable_CompactableTransition(
                              const Timetable_CompactableTransition& original);
        // Create a 'Timetable_CompactableTransition' having the same value as
        // the specified 'original' object.

    //! ~Timetable_CompactableTransition() = default;
        // Destroy this object.

    // MANIPULATORS
    Timetable_CompactableTransition& operator=(
                                   const Timetable_CompactableTransition& rhs);
        // Assign to this object the value of the specified 'rhs' compactable
        // transition, and return a reference providing modifiable access to
        // this object.

    // ACCESSORS
    const Time& time() const;
        // Return the time of this compactable transition.

    int code() const;
        // Return the code of this compactable transition.

                             // Aspects

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
};

// FREE OPERATORS
bool operator==(const Timetable_CompactableTransition& lhs,
                const Timetable_CompactableTransition& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' compactable transitions
    // have the same value, and 'false' otherwise.  Two compactable transitions
    // have the same value if they have the same time and code.

bool operator!=(const Timetable_CompactableTransition& lhs,
                const Timetable_CompactableTransition& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' compactable transitions
    // do not have the same value, and 'false' otherwise.  Two compactable
    // transitions do not have the same value if they do not have the same time
    // or the same code.

bool operator<(const Timetable_CompactableTransition& lhs,
               const Timetable_CompactableTransition& rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs', and 'false' otherwise.  Compactable transition 'lhs' has a value
    // less than compactable transition 'rhs' if 'lhs.time() < rhs.time()', or
    // 'lhs.time() == rhs.time()' and 'lhs.code() < rhs.code()'.

bool operator<(const Timetable_CompactableTransition& lhs,
               const Time&                            rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs', and 'false' otherwise.  Compactable transition 'lhs' has a value
    // less than time 'rhs' if 'lhs.time() < rhs'.  The behavior is undefined
    // unless '24 > rhs.hour()'.

bool operator<(const Time&                            lhs,
               const Timetable_CompactableTransition& rhs);
    // Return 'true' if the specified 'lhs' has a value less than the specified
    // 'rhs', and 'false' otherwise.  Time 'lhs' has a value less than
    // compactable transition 'rhs' if 'lhs < rhs.time()'.  The behavior is
    // undefined unless '24 > lhs.hour()'.

// HASH SPECIALIZATIONS
template <class HASHALG>
void hashAppend(HASHALG&                               hashAlg,
                const Timetable_CompactableTransition& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for
    // 'Timetable_CompactableTransition'.

                           // ===================
                           // class Timetable_Day
                           // ===================

class Timetable_Day {
    // This class implements a value-semantic repository of time-indexed state
    // transitions over one date (this class implements one day of a
    // timetable).  A 'Timetable_Day' can be "populated" with state transitions
    // via the 'addTransition' method, and queried for the transition code in
    // effect at a specified time via the 'transitionCodeInEffect' method.
    // Note that, as an optimization for the 'transitionCodeInEffect' method,
    // the transition code in effect before the first possible transition is
    // stored in 'd_initialTransitionCode'.

    // DATA
    int                                          d_initialTransitionCode;
                                             // transition code in effect at
                                             // the start of this daily
                                             // timetable

    bsl::vector<Timetable_CompactableTransition> d_transitions;
                                             // ordered vector of transitions

    // FRIENDS
    friend class Timetable_ConstIterator;

    friend bool operator==(const Timetable_Day&, const Timetable_Day&);
    friend bool operator!=(const Timetable_Day&, const Timetable_Day&);
    friend bool operator< (const Timetable_Day&, const Timetable_Day&);

    template <class HASHALG>
    friend void hashAppend(HASHALG&, const Timetable_Day&);

  public:
    // CONSTANTS
    enum { k_UNSET_TRANSITION_CODE =
                                TimetableTransition::k_UNSET_TRANSITION_CODE };
                                            // value representing an unset
                                            // transition code

    // CREATORS
    explicit
    Timetable_Day(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'Timetable_Day' (i.e., a daily timetable having no
        // transitions) whose initial transition code is
        // 'k_UNSET_TRANSITION_CODE'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    Timetable_Day(const Timetable_Day&  original,
                  bslma::Allocator     *basicAllocator = 0);
        // Create a 'Timetable_Day' having the same value as the specified
        // 'original' object.  Optionally specify a 'basicAllocator' used to
        // supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    //! ~Timetable_Day() = default;
        // Destroy this object.

    // MANIPULATORS
    Timetable_Day& operator=(const Timetable_Day& rhs);
        // Assign to this object the value of the specified 'rhs' daily
        // timetable, and return a reference providing modifiable access to
        // this object.

    bool addTransition(const Time& time, int code);
        // Add a transition to this daily timetable at the specified 'time'
        // having the specified 'code'.  If 'time' is already a transition
        // point, replace the existing code with 'code'.  Return 'true' if the
        // value returned by 'finalTransitionCode()' prior to this operation is
        // not equal to the value returned by 'finalTransitionCode()' after
        // this operation, and 'false' otherwise.  The behavior is undefined
        // unless '24 > time.hour()' and
        // '0 <= code || k_UNSET_TRANSITION_CODE == code'.

    bool removeAllTransitions();
        // Remove all transitions from this daily timetable.  Return 'true' if
        // the value returned by 'finalTransitionCode()' prior to this
        // operation is not equal to the value returned by
        // 'finalTransitionCode()' after this operation, and 'false' otherwise.

    bool removeTransition(const Time& time);
        // If a transition occurs at the specified 'time', remove the
        // transition from this daily timetable.  Otherwise, return without
        // modifying this daily timetable.  Return 'true' if the value returned
        // by 'finalTransitionCode()' prior to this operation is not equal to
        // the value returned by 'finalTransitionCode()' after this operation,
        // and 'false' otherwise.  The behavior is undefined unless
        // '24 > time.hour()'.

    bool setInitialTransitionCode(int code);
        // Set the transition code in effect prior to the start of this daily
        // timetable to the specified 'code'.  Return 'true' if the value
        // returned by 'finalTransitionCode()' prior to this operation is not
        // equal to the value returned by 'finalTransitionCode()' after this
        // operation, and 'false' otherwise.  The behavior is undefined unless
        // '0 <= code || k_UNSET_TRANSITION_CODE == code'.

    // ACCESSORS
    int finalTransitionCode() const;
        // Return the transition code that is in effect at the end of this
        // daily timetable.  Note that if this daily timetable has no
        // transitions, 'initialTransitionCode()' is returned.

    int initialTransitionCode() const;
        // Return the transition code in effect prior to the start of this
        // daily timetable.

    bsl::size_t size() const;
        // Return the number of transitions in this daily timetable.

    int transitionCodeInEffect(const Time& time) const;
        // Return the transition code associated with the latest transition
        // that occurs on or before the specified 'time' in this daily
        // timetable.  If this daily timetable has no such transition, return
        // 'initialTransitionCode()'.  The behavior is undefined unless
        // '24 > time.hour()'.
};

// FREE OPERATORS
bool operator==(const Timetable_Day& lhs, const Timetable_Day& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' daily timetables have the
    // same value, and 'false' otherwise.  Two daily timetables have the same
    // value if they have the same initial transition code, the same number of
    // transitions, and each corresponding pair of transitions has the same
    // value.

bool operator!=(const Timetable_Day& lhs, const Timetable_Day& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' daily timetables do not
    // have the same value, and 'false' otherwise.  Two daily timetables do not
    // have the same value if they do not have the same initial transition
    // code, they do not have the same number of transitions, or there is a
    // corresponding pair of transitions that do not have the same value.

bool operator<(const Timetable_Day& lhs, const Timetable_Day& rhs);
    // Return 'true' if the specified 'lhs' daily timetable is less than the
    // specified 'rhs' daily timetable, and 'false' otherwise.  The 'lhs' daily
    // timetable is less than the 'rhs' daily timetable if
    // 'lhs.initialTransitionCode() < rhs.initialTransitionCode()', or
    // 'lhs.initialTransitionCode() == rhs.initialTransitionCode()' and
    // 'lhs.d_transitions < rhs.d_transitions'.

// HASH SPECIALIZATIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const Timetable_Day& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'Timetable_Day'.

                             // ===============
                             // class Timetable
                             // ===============

class Timetable {
    // This class implements a value-semantic repository of datetime-indexed
    // state transitions over a *valid* *range* of dates.  This valid range,
    // '[firstDate() .. lastDate()]', spans the first and last dates of a
    // timetable's accessible contents.  A timetable can be "populated" with
    // state transitions via a suite of "add" methods.  Note that the behavior
    // of requesting *any* timetable information for a supplied date whose
    // value is outside the current *valid* *range* for that timetable is
    // undefined.

    // DATA
    Date                                d_firstDate;  // start of valid range

    Date                                d_lastDate;   // end of valid range

    int                                 d_initialTransitionCode;
                                                      // transition code in
                                                      // effect *before* the
                                                      // valid range

    bdlc::CompactedArray<Timetable_Day> d_timetable;  // daily timetables

    // FRIENDS
    friend class Timetable_ConstIterator;

    friend bool operator==(const Timetable&, const Timetable&);
    friend bool operator!=(const Timetable&, const Timetable&);

    template <class HASHALG>
    friend void hashAppend(HASHALG&, const Timetable&);

  public:
    // CONSTANTS
    enum { k_UNSET_TRANSITION_CODE =
                                TimetableTransition::k_UNSET_TRANSITION_CODE };
                                            // value representing an unset
                                            // transition code

    // TYPES
    typedef Timetable_ConstIterator const_iterator;

    // CREATORS
    explicit Timetable(bslma::Allocator *basicAllocator = 0);
        // Create an empty 'Timetable' (i.e., a timetable having no
        // transitions) whose initial transition code is
        // 'k_UNSET_TRANSITION_CODE'.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    Timetable(
             const Date&       firstDate,
             const Date&       lastDate,
             int               initialTransitionCode = k_UNSET_TRANSITION_CODE,
             bslma::Allocator *basicAllocator = 0);
        // Create a timetable having a valid range from the specified
        // 'firstDate' through the specified 'lastDate' and having the
        // optionally specified 'initialTransitionCode'.  If
        // 'initialTransitionCode' is not specified, the initial transition
        // code is set to 'k_UNSET_TRANSITION_CODE'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.  The behavior is
        // undefined unless 'firstDate <= lastDate', and
        // '0 <= initialTransitionCode' or
        // 'k_UNSET_TRANSITION_CODE == initialTransitionCode'.

    Timetable(const Timetable& original, bslma::Allocator *basicAllocator = 0);
        // Create a timetable having the value of the specified 'original'
        // timetable.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.

    //! ~Timetable() = default;
        // Destroy this object.

    // MANIPULATORS
    Timetable& operator=(const Timetable& rhs);
        // Assign to this timetable the value of the specified 'rhs' timetable,
        // and return a reference providing modifiable access to this
        // timetable.  This operation invalidates all iterators.

    void addTransition(const Date& date, const Time& time, int code);
        // Add a transition to this timetable on the specified 'date' at the
        // specified 'time' having the specified 'code'.  If 'time' is already
        // a transition point on 'date', replace the existing code with 'code'.
        // The addition of a transition, but not the replacement of the code of
        // an existing transition, invalidates all iterators.  The behavior is
        // undefined unless '24 > time.hour()', 'date' is within the valid
        // range of this timetable, and
        // '0 <= code || k_UNSET_TRANSITION_CODE == code'.

    void addTransition(const Datetime& datetime, int code);
        // Add a transition to this timetable at the specified 'datetime'
        // having the specified 'code'.  If 'datetime' is already a transition
        // point, replace the existing code with 'code'.  The addition of a
        // transition, but not the replacement of the code of an existing
        // transition, invalidates all iterators.  The behavior is undefined
        // unless '24 > datetime.hour()', 'datetime.date()' is within the valid
        // range of this timetable, and
        // '0 <= code || k_UNSET_TRANSITION_CODE == code'.

    void addTransitions(const DayOfWeek::Enum& dayOfWeek,
                        const Time&            time,
                        int                    code,
                        const Date&            firstDate,
                        const Date&            lastDate);
        // Add transitions to this timetable that occur at the specified
        // 'time', having the specified 'code', on all dates that are of the
        // specified 'dayOfWeek' within the closed interval of dates from the
        // specified 'firstDate' to the specified 'lastDate'.  For every date
        // on which this transition will occur, if 'time' is already a
        // transition point, replace the existing code with 'code'.  The
        // addition of a transition, but not the replacement of the code of an
        // existing transition, invalidates all iterators.  The behavior is
        // undefined unless '24 > time.hour()', 'firstDate <= lastDate',
        // 'firstDate' and 'lastDate' are within the valid range of this
        // timetable, and '0 <= code || k_UNSET_TRANSITION_CODE == code'.

    void removeAllTransitions();
        // Remove all transitions from this timetable.  The removal of a
        // transition invalidates all iterators.

    void removeTransition(const Date& date, const Time& time);
        // If a transition occurs on the specified 'date' at the specified
        // 'time', remove the transition from this timetable.  Otherwise,
        // return without modifying this timetable.  The removal of a
        // transition invalidates all iterators.  The behavior is undefined
        // unless '24 > time.hour()' and 'date' is within the valid range of
        // this timetable.

    void removeTransition(const Datetime& datetime);
        // If a transition occurs at the specified 'datetime', remove the
        // transition from this timetable.  Otherwise, return without modifying
        // this timetable.  The removal of a transition invalidates all
        // iterators.  The behavior is undefined unless '24 > datetime.hour()'
        // and 'datetime.date()' is within the valid range of this timetable.

    void removeTransitions(const Date& date);
        // Remove all transitions from this timetable that occur on the
        // specified 'date'.  The removal of a transition invalidates all
        // iterators.  The behavior is undefined unless 'date' is within the
        // valid range of this timetable.

    void removeTransitions(const DayOfWeek::Enum& dayOfWeek,
                           const Time&            time,
                           const Date&            firstDate,
                           const Date&            lastDate);
        // Remove all transitions from this timetable that occur at the
        // specified 'time' on all dates that are of the specified 'dayOfWeek'
        // within the closed interval of dates from the specified 'firstDate'
        // to the specified 'lastDate'.  The removal of a transition
        // invalidates all iterators.  The behavior is undefined unless
        // '24 > time.hour()', 'firstDate <= lastDate', and 'firstDate' and
        // 'lastDate' are within the valid range of this timetable.

    void reset();
        // Reset this timetable to the default constructed (empty) state.  All
        // associated iterators are invalidated.

    void setInitialTransitionCode(int code);
        // Set the transition code in effect at the start of this timetable to
        // the specified 'code'.  The behavior is undefined unless
        // '0 <= code || k_UNSET_TRANSITION_CODE == code'.

    void setValidRange(const Date& firstDate, const Date& lastDate);
        // Set the range of this timetable using the specified 'firstDate' and
        // 'lastDate' as, respectively, the first date and the last date of the
        // timetable.  Any transitions, and associated transition codes, that
        // are outside of the new range are removed.  The removal of a
        // transition invalidates all iterators.  The behavior is undefined
        // unless 'firstDate <= lastDate'.

                                  // Aspects

    void swap(Timetable& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const_iterator begin() const;
        // Return an iterator referring to the first transition in this
        // timetable, or the past-the-end iterator if this timetable is empty.
        // The iterator remains valid as long as this timetable exists, and the
        // number of transitions within this timetable does not change.

    const_iterator end() const;
        // Return the past-the-end iterator for this timetable.  The iterator
        // remains valid as long as this timetable exists, and the number of
        // transitions within this timetable does not change.

    const Date& firstDate() const;
        // Return a 'const' reference to the earliest date in the valid range
        // of this timetable.  The behavior is undefined if this timetable does
        // not have a valid range (i.e., it is in the default constructed
        // (empty) state).

    int initialTransitionCode() const;
        // Return the transition code that is in effect at the start of this
        // timetable (see 'setInitialTransitionCode').

    bool isInRange(const Date& date) const;
        // Return 'true' if the specified 'date' is within the valid range of
        // this timetable, and 'false' otherwise.

    const Date& lastDate() const;
        // Return a 'const' reference to the latest date in the valid range of
        // this timetable.  The behavior is undefined if this timetable does
        // not have a valid range (i.e., it is in the default constructed
        // (empty) state).

    int length() const;
        // Return the number of days in the valid range of this timetable,
        // which is defined to be 0 if this timetable is empty, and
        // 'lastDate() - firstDate() + 1' otherwise.

    int transitionCodeInEffect(const Date& date, const Time& time) const;
        // Return the transition code associated with the latest transition
        // that occurs on or before the specified 'date' and 'time' in this
        // timetable.  If this timetable has no such transition, return
        // 'initialTransitionCode()'.  The behavior is undefined unless
        // '24 > time.hour()' and 'date' is within the valid range of this
        // timetable.

    int transitionCodeInEffect(const Datetime& datetime) const;
        // Return the transition code associated with the latest transition
        // that occurs on or before the specified 'datetime' in this timetable.
        // If this timetable has no such transition, return
        // 'initialTransitionCode()'.  The behavior is undefined unless
        // '24 > datetime.hour()' and 'datetime.date()' is within the valid
        // range of this timetable.

                                  // Aspects

    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.

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
};

// FREE OPERATORS
bool operator==(const Timetable& lhs, const Timetable& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' timetables have the same
    // value, and 'false' otherwise.  Two timetables have the same value if
    // they have the same initial transition code, the same valid range (or are
    // both empty), the same number of transitions, and each corresponding pair
    // of transitions have the same value.

bool operator!=(const Timetable& lhs, const Timetable& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' timetables do not have
    // the same value, and 'false' otherwise.  Two timetables do not have the
    // same value if they do not have the same initial transition code, do not
    // have the same valid range (and are not both empty), do not have the same
    // number of transitions, or, for at least one corresponding pair of
    // transitions, do not have the same value.

bsl::ostream& operator<<(bsl::ostream& stream, const Timetable& timetable);
    // Write the value of the specified 'timetable' to the specified output
    // 'stream', and return a reference to the modifiable 'stream'.

// FREE FUNCTIONS
void swap(Timetable& a, Timetable& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee if the two objects were
    // created with the same allocator and the basic guarantee otherwise.

// HASH SPECIALIZATIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const Timetable& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'Timetable'.

                      // =============================
                      // class Timetable_ConstIterator
                      // =============================

class Timetable_ConstIterator {
    // Provide read-only, sequential access in increasing (chronological) order
    // to the transitions in a 'Timetable' object.

    // DATA
    const Timetable                 *d_timetable_p;      // pointer to the
                                                         // 'Timetable' into
                                                         // which this iterator
                                                         // references

    bsl::size_t                      d_dayIndex;         // index of the
                                                         // referenced daily
                                                         // timetable

    bsl::size_t                      d_transitionIndex;  // index of the
                                                         // referenced
                                                         // transition in the
                                                         // referenced daily
                                                         // timetable

    mutable TimetableTransition_Ref  d_ref;              // cached value used
                                                         // for the return
                                                         // value of
                                                         // 'operator->()'

    // FRIENDS
    friend class Timetable;

    friend bool operator==(const Timetable_ConstIterator&,
                           const Timetable_ConstIterator&);
    friend bool operator!=(const Timetable_ConstIterator&,
                           const Timetable_ConstIterator&);

  private:
    // PRIVATE CREATORS
    Timetable_ConstIterator(const Timetable& timetable,
                            bsl::size_t      dayIndex,
                            bsl::size_t      transitionIndex);
        // Create a transition iterator for the specified 'timetable' that
        // refers to the transition at the specified 'transitionIndex' on the
        // day at the specified 'dayIndex' in 'timetable'.

  public:
    // TYPES
    typedef TimetableTransition      value_type;
    typedef TimetableTransition_Ref *pointer;
    typedef TimetableTransition      reference;
        // The star operator returns a 'TimetableTransition' *by* *value*.

    // CREATORS
    Timetable_ConstIterator();
        // Create a default iterator.  Note that the behavior of most methods
        // is undefined when used on a default-constructed iterator.

    Timetable_ConstIterator(const Timetable_ConstIterator& original);
        // Create an iterator having the value of the specified 'original'
        // iterator.

    //! ~Timetable_ConstIterator() = default;
        // Destroy this object.

    // MANIPULATORS
    Timetable_ConstIterator& operator=(const Timetable_ConstIterator& rhs);
        // Assign to this iterator the value of the specified 'rhs' iterator,
        // and return a reference providing modifiable access to this object.

    Timetable_ConstIterator& operator++();
        // Advance this iterator to refer to the next transition in the
        // associated timetable, and return a reference providing modifiable
        // access to this object.  The behavior is undefined unless, on entry,
        // this iterator references a valid transition.

    Timetable_ConstIterator& operator--();
        // Regress this iterator to refer to the previous transition in the
        // associated timetable, and return a reference providing modifiable
        // access to this object.  The behavior is undefined unless, on entry,
        // this iterator references a valid transition that is not the first
        // transition of the associated timetable.

    // ACCESSORS
    TimetableTransition operator*() const;
        // Return, *by* *value*, a 'TimetableTransition' object representing
        // the transition referenced by this iterator.  The behavior is
        // undefined unless this iterator references a valid transition in the
        // associated timetable.

    const TimetableTransition_Ref *operator->() const;
        // Return a proxy to the transition referenced by this iterator.  The
        // behavior is undefined unless this iterator references a valid
        // transition in the associated timetable.
};

// FREE OPERATORS
Timetable_ConstIterator operator++(Timetable_ConstIterator& iterator, int);
    // Advance the specified 'iterator' to refer to the next transition in the
    // referenced timetable, and return an iterator referring to the original
    // element (*before* the advancement).  The behavior is undefined unless,
    // on entry, 'iterator' references a valid transition.

Timetable_ConstIterator operator--(Timetable_ConstIterator& iterator, int);
    // Regress the specified 'iterator' to refer to the previous transition in
    // the referenced timetable, and return an iterator referring to the
    // original element (*before* the decrementation).  The behavior is
    // undefined unless, on entry, 'iterator' references a valid transition
    // that is not the first transition of the associated timetable.

bool operator==(const Timetable_ConstIterator& lhs,
                const Timetable_ConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators have the same
    // value, and 'false' otherwise.  Two 'Timetable_ConstIterator' iterators
    // have the same value if they refer to the same timetable and the same
    // transition.

bool operator!=(const Timetable_ConstIterator& lhs,
                const Timetable_ConstIterator& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' iterators do not have the
    // same value, and 'false' otherwise.  Two 'Timetable_ConstIterator'
    // iterators do not have the same value if they do not refer to the same
    // timetable, or do not refer to the same transition.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                        // -------------------------
                        // class TimetableTransition
                        // -------------------------

// PRIVATE CREATORS
inline
TimetableTransition::TimetableTransition()
: d_datetime(Date())
, d_code(k_UNSET_TRANSITION_CODE)
{
}

inline
TimetableTransition::TimetableTransition(const Datetime& datetime, int code)
: d_datetime(datetime)
, d_code(code)
{
    BSLS_ASSERT(24 > datetime.hour());

    BSLS_ASSERT(0 <= code || k_UNSET_TRANSITION_CODE == code);
}

// CREATORS
inline
TimetableTransition::TimetableTransition(const TimetableTransition& original)
: d_datetime(original.d_datetime)
, d_code(original.d_code)
{
}

// MANIPULATORS
inline
TimetableTransition& TimetableTransition::operator=(
                                                const TimetableTransition& rhs)
{
    d_datetime = rhs.d_datetime;
    d_code     = rhs.d_code;

    return *this;
}

// ACCESSORS
inline
const Datetime& TimetableTransition::datetime() const
{
    return d_datetime;
}

inline
int TimetableTransition::code() const
{
    return d_code;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const TimetableTransition& lhs,
                      const TimetableTransition& rhs)
{
    return lhs.datetime() == rhs.datetime() && lhs.code() == rhs.code();
}

inline
bool bdlt::operator!=(const TimetableTransition& lhs,
                      const TimetableTransition& rhs)
{
    return lhs.datetime() != rhs.datetime() || lhs.code() != rhs.code();
}

inline
bool bdlt::operator<(const TimetableTransition& lhs,
                     const TimetableTransition& rhs)
{
    return lhs.datetime() < rhs.datetime()
        || (lhs.datetime() == rhs.datetime() && lhs.code() < rhs.code());
}

inline
bool bdlt::operator<(const TimetableTransition& lhs, const Datetime& rhs)
{
    BSLS_ASSERT(24 > rhs.hour());

    return lhs.datetime() < rhs;
}

inline
bool bdlt::operator<(const Datetime& lhs, const TimetableTransition& rhs)
{
    BSLS_ASSERT(24 > lhs.hour());

    return lhs < rhs.datetime();
}

// HASH SPECIALIZATIONS
template <class HASHALG>
inline
void bdlt::hashAppend(HASHALG& hashAlg, const TimetableTransition& object)
{
    using ::BloombergLP::bslh::hashAppend;

    hashAppend(hashAlg, object.datetime());
    hashAppend(hashAlg, object.code());
}

namespace bdlt {

                      // -----------------------------
                      // class TimetableTransition_Ref
                      // -----------------------------

// PRIVATE CREATORS
inline
TimetableTransition_Ref::TimetableTransition_Ref()
: TimetableTransition()
{
}

// CREATORS
inline
TimetableTransition_Ref::TimetableTransition_Ref(
                                         const TimetableTransition& transition)
: TimetableTransition(transition)
{
}

inline
TimetableTransition_Ref::TimetableTransition_Ref(
                                       const TimetableTransition_Ref& original)
: TimetableTransition(original)
{
}

// MANIPULATORS
inline
TimetableTransition_Ref& TimetableTransition_Ref::operator=(
                                                const TimetableTransition& rhs)
{
    d_datetime = rhs.d_datetime;
    d_code     = rhs.d_code;

    return *this;
}

                  // -------------------------------------
                  // class Timetable_CompactableTransition
                  // -------------------------------------

// CREATORS
inline
Timetable_CompactableTransition::Timetable_CompactableTransition()
: d_time(0)
, d_code(k_UNSET_TRANSITION_CODE)
{
}

inline
Timetable_CompactableTransition::Timetable_CompactableTransition(
                                                              const Time& time,
                                                              int         code)
: d_time(time)
, d_code(code)
{
    BSLS_ASSERT(24 > time.hour());

    BSLS_ASSERT(0 <= code || k_UNSET_TRANSITION_CODE == code);
}

inline
Timetable_CompactableTransition::Timetable_CompactableTransition(
                               const Timetable_CompactableTransition& original)
: d_time(original.d_time)
, d_code(original.d_code)
{
}

// MANIPULATORS
inline
Timetable_CompactableTransition& Timetable_CompactableTransition::operator=(
                                    const Timetable_CompactableTransition& rhs)
{
    d_time = rhs.d_time;
    d_code = rhs.d_code;

    return *this;
}

// ACCESSORS
inline
const Time& Timetable_CompactableTransition::time() const
{
    return d_time;
}

inline
int Timetable_CompactableTransition::code() const
{
    return d_code;
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const Timetable_CompactableTransition& lhs,
                      const Timetable_CompactableTransition& rhs)
{
    return lhs.time() == rhs.time() && lhs.code() == rhs.code();
}

inline
bool bdlt::operator!=(const Timetable_CompactableTransition& lhs,
                      const Timetable_CompactableTransition& rhs)
{
    return lhs.time() != rhs.time() || lhs.code() != rhs.code();
}

inline
bool bdlt::operator<(const Timetable_CompactableTransition& lhs,
                     const Timetable_CompactableTransition& rhs)
{
    return lhs.time() < rhs.time()
        || (lhs.time() == rhs.time() && lhs.code() < rhs.code());
}

inline
bool bdlt::operator<(const Timetable_CompactableTransition& lhs,
                     const Time&                            rhs)
{
    BSLS_ASSERT(24 > rhs.hour());

    return lhs.time() < rhs;
}

inline
bool bdlt::operator<(const Time&                            lhs,
                     const Timetable_CompactableTransition& rhs)
{
    BSLS_ASSERT(24 > lhs.hour());

    return lhs < rhs.time();
}

// HASH SPECIALIZATIONS
template <class HASHALG>
inline
void bdlt::hashAppend(HASHALG&                               hashAlg,
                      const Timetable_CompactableTransition& object)
{
    using ::BloombergLP::bslh::hashAppend;

    hashAppend(hashAlg, object.time());
    hashAppend(hashAlg, object.code());
}

namespace bdlt {

                           // -------------------
                           // class Timetable_Day
                           // -------------------

// CREATORS
inline
Timetable_Day::Timetable_Day(bslma::Allocator *basicAllocator)
: d_initialTransitionCode(k_UNSET_TRANSITION_CODE)
, d_transitions(basicAllocator)
{
}

inline
Timetable_Day::Timetable_Day(const Timetable_Day&  original,
                             bslma::Allocator     *basicAllocator)
: d_initialTransitionCode(original.d_initialTransitionCode)
, d_transitions(original.d_transitions, basicAllocator)
{
}

// MANIPULATORS
inline
Timetable_Day& Timetable_Day::operator=(const Timetable_Day& rhs)
{
    d_initialTransitionCode = rhs.d_initialTransitionCode;
    d_transitions           = rhs.d_transitions;

    return *this;
}

inline
bool Timetable_Day::removeAllTransitions()
{
    int code = finalTransitionCode();

    d_transitions.clear();

    return code != d_initialTransitionCode;
}

inline
bool Timetable_Day::setInitialTransitionCode(int code)
{
    BSLS_ASSERT(0 <= code || k_UNSET_TRANSITION_CODE == code);

    bool rv = d_initialTransitionCode != code && d_transitions.empty();

    d_initialTransitionCode = code;

    return rv;
}

// ACCESSORS
inline
int Timetable_Day::finalTransitionCode() const
{
    bsl::vector<Timetable_CompactableTransition>::const_reverse_iterator iter =
                                                        d_transitions.rbegin();

    return iter != d_transitions.rend()
         ? iter->d_code
         : d_initialTransitionCode;
}

inline
int Timetable_Day::initialTransitionCode() const
{
    return d_initialTransitionCode;
}

inline
bsl::size_t Timetable_Day::size() const
{
    return d_transitions.size();
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const Timetable_Day& lhs, const Timetable_Day& rhs)
{
    return lhs.d_initialTransitionCode == rhs.d_initialTransitionCode
        && lhs.d_transitions           == rhs.d_transitions;
}

inline
bool bdlt::operator!=(const Timetable_Day& lhs, const Timetable_Day& rhs)
{
    return lhs.d_initialTransitionCode != rhs.d_initialTransitionCode
        || lhs.d_transitions           != rhs.d_transitions;
}

inline
bool bdlt::operator<(const Timetable_Day& lhs, const Timetable_Day& rhs)
{
    return lhs.d_initialTransitionCode < rhs.d_initialTransitionCode
        || (   lhs.d_initialTransitionCode == rhs.d_initialTransitionCode
            && lhs.d_transitions < rhs.d_transitions);
}

// HASH SPECIALIZATIONS
template <class HASHALG>
inline
void bdlt::hashAppend(HASHALG& hashAlg, const Timetable_Day& object)
{
    using ::BloombergLP::bslh::hashAppend;

    hashAppend(hashAlg, object.d_initialTransitionCode);
    hashAppend(hashAlg, object.d_transitions);
}

namespace bdlt {

                             // ---------------
                             // class Timetable
                             // ---------------

// MANIPULATORS
inline
Timetable& Timetable::operator=(const Timetable& rhs)
{
    Timetable(rhs, allocator()).swap(*this);

    return *this;
}

inline
void Timetable::addTransition(const Datetime& datetime, int code)
{
    addTransition(datetime.date(), datetime.time(), code);
}

inline
void Timetable::removeAllTransitions()
{
    Date firstDate = d_firstDate;
    Date lastDate  = d_lastDate;

    d_firstDate = Date(9999, 12, 31);
    d_lastDate  = Date(   1,  1,  1);

    d_timetable.removeAll();

    setValidRange(firstDate, lastDate);
}

inline
void Timetable::removeTransition(const Datetime& datetime)
{
    removeTransition(datetime.date(), datetime.time());
}

inline
void Timetable::reset()
{
    d_initialTransitionCode = k_UNSET_TRANSITION_CODE;

    d_firstDate = Date(9999, 12, 31);
    d_lastDate  = Date(   1,  1,  1);

    d_timetable.removeAll();
}

                                  // Aspects

inline
void Timetable::swap(Timetable& other)
{
    // Member 'swap' is undefined for objects with non-equal allocators.

    BSLS_ASSERT(allocator() == other.allocator());

    bslalg::SwapUtil::swap(&d_initialTransitionCode,
                           &other.d_initialTransitionCode);

    bslalg::SwapUtil::swap(&d_firstDate, &other.d_firstDate);
    bslalg::SwapUtil::swap(&d_lastDate,  &other.d_lastDate);
    bslalg::SwapUtil::swap(&d_timetable, &other.d_timetable);
}

// ACCESSORS
inline
Timetable::const_iterator Timetable::end() const
{
    return Timetable_ConstIterator(*this, d_timetable.length(), 0);
}

inline
const Date& Timetable::firstDate() const
{
    BSLS_ASSERT(0 < length());

    return d_firstDate;
}

inline
int Timetable::initialTransitionCode() const
{
    return d_initialTransitionCode;
}

inline
bool Timetable::isInRange(const Date& date) const
{
    return date >= d_firstDate && date <= d_lastDate;
}

inline
const Date& Timetable::lastDate() const
{
    BSLS_ASSERT(0 < length());

    return d_lastDate;
}

inline
int Timetable::length() const
{
    return d_firstDate <= d_lastDate ? d_lastDate - d_firstDate + 1 : 0;
}

inline
int Timetable::transitionCodeInEffect(const Date& date, const Time& time) const
{
    BSLS_ASSERT(24 > time.hour());
    BSLS_ASSERT(isInRange(date));

    bsl::size_t          index = date - d_firstDate;
    const Timetable_Day& daily = d_timetable[index];

    return daily.transitionCodeInEffect(time);
}

inline
int Timetable::transitionCodeInEffect(const Datetime& datetime) const
{
    return transitionCodeInEffect(datetime.date(), datetime.time());
}

                                  // Aspects

inline
bslma::Allocator *Timetable::allocator() const
{
    return d_timetable.allocator();
}

}  // close package namespace

// FREE OPERATORS
inline
bool bdlt::operator==(const Timetable& lhs, const Timetable& rhs)
{
    return lhs.d_initialTransitionCode == rhs.d_initialTransitionCode
        && lhs.d_firstDate             == rhs.d_firstDate
        && lhs.d_lastDate              == rhs.d_lastDate
        && lhs.d_timetable             == rhs.d_timetable;
}

inline
bool bdlt::operator!=(const Timetable& lhs, const Timetable& rhs)
{
    return lhs.d_initialTransitionCode != rhs.d_initialTransitionCode
        || lhs.d_firstDate             != rhs.d_firstDate
        || lhs.d_lastDate              != rhs.d_lastDate
        || lhs.d_timetable             != rhs.d_timetable;
}

inline
bsl::ostream& bdlt::operator<<(bsl::ostream&    stream,
                               const Timetable& timetable)
{
    return timetable.print(stream, 0, -1);
}

// FREE FUNCTIONS
inline
void bdlt::swap(Timetable& a, Timetable& b)
{
    if (a.allocator() == b.allocator()) {
        a.swap(b);

        return;                                                       // RETURN
    }

    Timetable futureA(b, a.allocator());
    Timetable futureB(a, b.allocator());

    futureA.swap(a);
    futureB.swap(b);
}

// HASH SPECIALIZATIONS
template <class HASHALG>
inline
void bdlt::hashAppend(HASHALG& hashAlg, const Timetable& object)
{
    using ::BloombergLP::bslh::hashAppend;

    hashAppend(hashAlg, object.d_firstDate);
    hashAppend(hashAlg, object.d_lastDate);
    hashAppend(hashAlg, object.d_initialTransitionCode);
    hashAppend(hashAlg, object.d_timetable);
}

namespace bdlt {

                      // -----------------------------
                      // class Timetable_ConstIterator
                      // -----------------------------

// PRIVATE CREATORS
inline
Timetable_ConstIterator::Timetable_ConstIterator(
                                              const Timetable& timetable,
                                              bsl::size_t      dayIndex,
                                              bsl::size_t      transitionIndex)
: d_timetable_p(&timetable)
, d_dayIndex(dayIndex)
, d_transitionIndex(transitionIndex)
{
}

// CREATORS
inline
Timetable_ConstIterator::Timetable_ConstIterator()
: d_timetable_p(0)
, d_dayIndex(0)
, d_transitionIndex(0)
{
}

inline
Timetable_ConstIterator::Timetable_ConstIterator(
                                       const Timetable_ConstIterator& original)
: d_timetable_p(original.d_timetable_p)
, d_dayIndex(original.d_dayIndex)
, d_transitionIndex(original.d_transitionIndex)
{
}

// MANIPULATORS
inline
Timetable_ConstIterator& Timetable_ConstIterator::
                                  operator=(const Timetable_ConstIterator& rhs)
{
    d_timetable_p     = rhs.d_timetable_p;
    d_dayIndex        = rhs.d_dayIndex;
    d_transitionIndex = rhs.d_transitionIndex;

    return *this;
}

// ACCESSORS
inline
TimetableTransition Timetable_ConstIterator::operator*() const
{
    BSLS_ASSERT(d_timetable_p);
    BSLS_ASSERT(d_dayIndex
                          < static_cast<bsl::size_t>(d_timetable_p->length()));
    BSLS_ASSERT(d_transitionIndex
                              < d_timetable_p->d_timetable[d_dayIndex].size());

    const Timetable_CompactableTransition& transition =
                          d_timetable_p->d_timetable[d_dayIndex].d_transitions[
                                                            d_transitionIndex];
    return TimetableTransition(
            Datetime(d_timetable_p->firstDate() + static_cast<int>(d_dayIndex),
                     transition.time()),
            transition.code());
}

inline
const TimetableTransition_Ref *Timetable_ConstIterator::operator->() const
{
    BSLS_ASSERT(d_timetable_p);
    BSLS_ASSERT(d_dayIndex
                          < static_cast<bsl::size_t>(d_timetable_p->length()));

    d_ref = this->operator*();
    return &d_ref;
}

}  // close package namespace

// FREE OPERATORS
inline
bdlt::Timetable_ConstIterator bdlt::operator++(
                                             Timetable_ConstIterator& iterator,
                                             int)
{
    const Timetable_ConstIterator curr = iterator;
    ++iterator;
    return curr;
}

inline
bdlt::Timetable_ConstIterator bdlt::operator--(
                                             Timetable_ConstIterator& iterator,
                                             int)
{
    const Timetable_ConstIterator curr = iterator;
    --iterator;
    return curr;
}

inline
bool bdlt::operator==(const Timetable_ConstIterator& lhs,
                      const Timetable_ConstIterator& rhs)
{
    return lhs.d_timetable_p     == rhs.d_timetable_p
        && lhs.d_dayIndex        == rhs.d_dayIndex
        && lhs.d_transitionIndex == rhs.d_transitionIndex;
}

inline
bool bdlt::operator!=(const Timetable_ConstIterator& lhs,
                      const Timetable_ConstIterator& rhs)
{
    return lhs.d_timetable_p     != rhs.d_timetable_p
        || lhs.d_dayIndex        != rhs.d_dayIndex
        || lhs.d_transitionIndex != rhs.d_transitionIndex;
}

}  // close enterprise namespace

// TRAITS

namespace BloombergLP {
namespace bslma {

template <>
struct UsesBslmaAllocator<bdlt::Timetable_Day> : bsl::true_type {};

template <>
struct UsesBslmaAllocator<bdlt::Timetable> : bsl::true_type {};

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
