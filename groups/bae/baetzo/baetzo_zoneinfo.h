// baetzo_zoneinfo.h                                                  -*-C++-*-
#ifndef INCLUDED_BAETZO_ZONEINFO
#define INCLUDED_BAETZO_ZONEINFO

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a value type to represent a time zone.
//
//@CLASSES:
//  baetzo_Zoneinfo: information about a time zone
//  baetzo_ZoneinfoTransition: attributes representing a time transition
//
//@AUTHOR: Stefano Pacifico (spacifico1)
//
//@SEE_ALSO: baetzo_localtimedescriptor, baetzo_zoneinfoutil
//
//@DESCRIPTION: This component provides a *value* *semantic* type,
// 'baetzo_Zoneinfo', that represents the information about a specific time
// zone contained in the Zoneinfo database.  In addition, this component
// provides an unconstrained *in-core* *value-semantic* type
// 'baetzo_ZoneinfoTransition' that can be used to characterize a transition
// for time zones.
//
///'baetzo_ZoneinfoTransition'
///---------------------------
// A 'baetzo_ZoneinfoTransition' contains:
//..
//  Name        Type
//  ----------  --------------------------
//  utcTime     bdetu_Epoch::TimeT64
//  descriptor  baetzo_LocalTimeDescriptor
//..
//: o utcTime: UTC time when a transition occurs
//:
//: o descriptor: local time value corresponding to the time transition
//
// For example, in New York on March 14, 2011, at the instant 1 a.m., clocks
// are set forward by an hour to mark the transition from Eastern Standard Time
// to Eastern Daylight Time.  This change can be represented by a
// 'baetzo_ZoneinfoTransition' object whose 'utcTime' refers to March 14, 2011,
// 1 a.m. and whose 'descriptor' describes Eastern Daylight Time (i.e.,
// description is "EDT", 'dstInEffectFlag' is 'true', and 'utcOffsetInSeconds'
// is -14,400 (-4 * 60 * 60)).
//
///'baetzo_Zoneinfo'
///-----------------
// A 'baetzo_Zoneinfo' contains:
//: o the time zone identifier (e.g., "America/New_York" or "Asia/Tokyo")
//:
//: o the ordered sequence of 'baetzo_ZoneinfoTransition' objects,
//:   representing the various transitions from UTC for this time zone.
//
// A 'baetzo_Zoneinfo' object also provides the method
// 'findTransitionForUtcTime' that allows a client to find, in the sequence of
// transitions, the appropriate transition whose local-time descriptor
// describes the properties of local time, at a specified UTC time, for the
// time zone in question.  Note that, even though this information is
// sufficient for converting local date and time, to their corresponding values
// in other time zones, clients are encouraged to use the utilities provided in
// 'baetzo_timezoneutil'.  Also note that, 'baetzo_Zoneinfo' objects are
// typically populated by the client through the 'baetzo_Loader' protocol, and
// not directly.
//
///Zoneinfo Database
///-----------------
// This database, also referred to as either the TZ database or the Olson
// database (after its creator, Arthur Olson), is a standard, public-domain
// time-zone information distribution used by many software systems (including
// a number of Unix variants and the Java Runtime Environment).  Information
// about the Zoneinfo database -- including the time zone rules for the
// supported time zones, and source code for the 'zic' compiler (for compiling
// those rules into the binary representation used by this component) -- can be
// found online at 'http://www.twinsun.com/tz/tz-link.htm'.  This time-zone
// information can be used to perform the conversion of dates and times from
// UTC to their corresponding dates and times in a given time zone and
// vice-versa.  (See 'baetzo_zoneinfobinaryreader' for more information about
// the binary file format.)
//
///Usage
///-----
// The following usage examples illustrate how to populate a 'baetzo_Zoneinfo'
// object and use it to transform a UTC time into a local time.
//
///Example 1: Populate a 'baetzo_Zoneinfo'
///- - - - - - - - - - - - - - - - - - - -
// Suppose we want to represent the time-zone information for New York, in 2010
// using a 'baetzo_Zoneinfo' object.  In order to do so,  we need to provide
// the UTC date-times (transitions) after which the time zone changes its
// offset from UTC, or daylight-saving Time starts or ends.
//
// First, we define two times "Mar 14, 2010 6:00 AM" and "Nov 07, 2010 7:00 AM"
// representing respectively the UTC time at which New York transitions to
// Eastern Daylight-saving Time (EDT) and Eastern Standard Time (EST) in 2010:
//..
//  bdet_Datetime edtDatetime(2010, 03, 14, 2, 0);
//  bdet_Datetime estDatetime(2010, 11, 07, 2, 0);
//..
// Then, we create two local-time descriptors that hold the offset from UTC and
// DST status for EST and EDT in New York in 2010, in terms of their
// 'offsetFromUtcInSeconds', 'dstInEffectFlag' and 'description' attributes:
//..
//  const baetzo_LocalTimeDescriptor est(-5 * 60 * 60, false, "EST");
//  const baetzo_LocalTimeDescriptor edt(-4 * 60 * 60, true,  "EDT");
//..
// Note that these descriptors will be associated with the created transition
// times, to reflect how local time in New York changes its offset from UTC and
// DST status after specific times.
//
// Next, we create an empty 'baetzo_Zoneinfo' object that will be populated
// with the information necessary to describe the time zone for New York:
//..
//  baetzo_Zoneinfo newYorkTimeZone;
//..
// Then, before being able to associate the transition times with their
// corresponding descriptors, we need translate the transition times to
// 'bdetu_Epoch::TimeT64':
//..
//  bdetu_Epoch::TimeT64 edtTransitionTime =
//                                  bdetu_Epoch::convertToTimeT64(edtDatetime);
//  bdetu_Epoch::TimeT64 estTransitionTime =
//                                  bdetu_Epoch::convertToTimeT64(estDatetime);
//..
// Now, we associate the created descriptors with the transitions we indicated
// previously and add them to 'newYorkTimeZone' using the 'addTransition'
// method:
//..
//  newYorkTimeZone.addTransition(edtTransitionTime, edt);
//  newYorkTimeZone.addTransition(estTransitionTime, est);
//..
// Note that this insertion operation maintains the transitions in order of
// transition time, and therefore inserting transitions out-of-order, while not
// illegal, is very inefficient.
//
// Finally we verify that the 'newYorkTimeZone' contains the transitions we
// indicated:
//..
//  {
//      baetzo_Zoneinfo::TransitionConstIterator it =
//                                          newYorkTimeZone.beginTransitions();
//
//      assert(it->utcTime()    == edtTransitionTime);
//      assert(it->descriptor() == edt);
//      ++it;
//      assert(it->utcTime()    == estTransitionTime);
//      assert(it->descriptor() == est);
//  }
//..
// Notice that the iterator refers to a 'baetzo_ZoneinfoTransition' object.
//
///Example 2: Converting UTC to Local Time
///- - - - - - - - - - - - - - - - - - - -
// Suppose that we want to program a function, 'utcToLocalTime' to perform the
// conversion from a UTC time value to the corresponding local time value, in a
// given time zone, and return the computed local time:
//
// First we declare the function 'utcToLocalTime' and its contract:
//..
//  static bdet_DatetimeTz utcToLocalTime(const bdet_Datetime&   utcTime,
//                                        const baetzo_Zoneinfo& timeZone)
//  {
//      // Return the 'bdet_DatetimeTz' value representing the date, time and
//      // offset from UTC (rounded to the minute) value of the local time,
//      // corresponding to the specified 'utcTime' in the specified
//      // 'timeZone'.  The behavior is undefined if the 'utcTime' precedes the
//      // time of the first transition contained in 'timeZone' and
//      // '0 < timeZone.numTransitions()'.
//..
// Then, we check the precondition of 'utcToLocalTime', by checking that
// 'timeZone' contains at least one transitions and comparing 'utcTime' to the
// first transition time in 'timeZone':
//..
//      BSLS_ASSERT(0 < timeZone.numTransitions());
//      BSLS_ASSERT(timeZone.firstTransition().utcTime() <=
//                                     bdetu_Epoch::convertToTimeT64(utcTime));
//..
// Next, we obtain the appropriate 'baetzo_ZoneinfoTransition' object,
// invoking the method 'findTransitionForUtcTime' on 'timeZone':
//..
//      baetzo_Zoneinfo::TransitionConstIterator it =
//                                  timeZone.findTransitionForUtcTime(utcTime);
//..
// Then, we access the descriptor associated with the transition to which 'it'
// refers, and calculate the offset from UTC rounded to the minute:
//..
//      const baetzo_ZoneinfoTransition& transition = *it;
//      const int offsetInMinutes =
//                           transition.descriptor().utcOffsetInSeconds() / 60;
//..
// Now, we apply the obtained 'offsetInMinutes' to the originally specified
// 'utcTime' obtaining the corresponding local time in the specified
// 'timeZone':
//..
//      bdet_Datetime temp(utcTime);
//      temp.addMinutes(offsetInMinutes);
//..
// Finally, return the local time value together with its offset from UTC:
//..
//      return bdet_DatetimeTz(temp, offsetInMinutes);
//  }
//..
// Suppose, now, we want to convert UTC time to the corresponding local time in
// New York.  We can do so using the previously defined function
// 'utcToLocalTime' and reusing the 'baetzo_Zoneinfo' object, 'newYorkTimeZone'
// of Example 1.
//
// First, we define 'bdet_Datetime' object representing the UTC time "Apr 10,
// 2010 12:00":
//..
//  bdet_Datetime utcDatetime(2010, 04, 10, 12, 0, 0);
//..
// Then, we invoke 'utcToLocalTime' passing 'newYorkTimeZone' as a time zone
// and save the result:
//..
//  bdet_DatetimeTz nyDatetime = utcToLocalTime(utcDatetime, newYorkTimeZone);
//..
// Finally, we compute the New York local time corresponding to 'utcDatetime',
// verify that "April 10, 2010 8:00" is the computed time:
//..
//  const bdet_Datetime expectedTime(2010, 4, 10, 8, 0, 0);
//  assert(-4 * 60      == nyDatetime.offset());
//  assert(expectedTime == nyDatetime.localDatetime());
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAETZO_LOCALTIMEDESCRIPTOR
#include <baetzo_localtimedescriptor.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDETU_EPOCH
#include <bdetu_epoch.h>
#endif

#ifndef INCLUDED_BDEUT_STRINGREF
#include <bdeut_stringref.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef INCLUDED_BSL_SET
#include <bsl_set.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {

class bslma_Allocator;

                        // ===============================
                        // class baetzo_ZoneinfoTransition
                        // ===============================

class baetzo_ZoneinfoTransition {
    // This class is an unconstrained *in-core* value-semantic class that
    // characterizes a transition when the local time value of a time-zone
    // changes.  The salient attributes of this type are the 'utcTime'
    // (representing seconds from UTC), and 'descriptor' representing the local
    // time value after the transition.
    //
    // This class:
    //: o supports a complete set of *value* *semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology: see 'bsldoc_glossary'.

    // DATA
    bdetu_Epoch::TimeT64              d_utcTime;       // UTC time
                                                       // (representing in
                                                       // seconds from epoch)
                                                       // when the time
                                                       // transition occurs

    const baetzo_LocalTimeDescriptor *d_descriptor_p;  // pointer to the
                                                       // descriptor associated
                                                       // with this transition
                                                       // (held, not owned)

    // FRIENDS
    friend class baetzo_Zoneinfo;

    // PRIVATE CREATORS
    baetzo_ZoneinfoTransition(bdetu_Epoch::TimeT64              utcTime,
                              const baetzo_LocalTimeDescriptor *descriptor);
        // Create a 'baetzo_ZoneinfoTransition' object having the specified
        // 'utcTime' and, 'descriptor' attribute values.  The behavior is
        // undefined unless 'descriptor' remains valid for the lifetime of
        // this object.

    public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baetzo_ZoneinfoTransition,
                                 bslalg_TypeTraitBitwiseMoveable);

    // CREATORS
#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~baetzo_ZoneinfoTransition();
        // Destroy this object.
#endif

    // ACCESSORS
    const baetzo_LocalTimeDescriptor& descriptor() const;
        // Return a reference providing non-modifiable access to the
        // 'descriptor' attribute of this object.

    bdetu_Epoch::TimeT64 utcTime() const;
        // Return the value of the 'utcTime' attribute of this object.

                                // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the
        // format is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const baetzo_ZoneinfoTransition& lhs,
                const baetzo_ZoneinfoTransition& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'baetzo_ZoneinfoTransition' objects
    // have the same value if the corresponding value of their 'utcTime'
    // attribute is the same and both refer to the same 'descriptor' address.

bool operator!=(const baetzo_ZoneinfoTransition& lhs,
                const baetzo_ZoneinfoTransition& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baetzo_ZoneinfoTransition'
    // objects do not have the same value if the corresponding value of their
    // 'utcTime' is not the same or if they do not refer to the same
    // 'descriptor' address.

bool operator<(const baetzo_ZoneinfoTransition& lhs,
               const baetzo_ZoneinfoTransition& rhs);
     // Return 'true' if the value of the specified 'lhs' is less than (ordered
     // before) the value of the specified 'rhs'.  Note that the value of 'lhs'
     // is less than the value of 'rhs' if the value of the 'utcTime' attribute
     // of 'lhs' is less than the value of the 'utcTime' attribute of 'rhs'.

bsl::ostream& operator<<(bsl::ostream&                    stream,
                         const baetzo_ZoneinfoTransition& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)' with the attribute names elided.

                        // =====================
                        // class baetzo_Zoneinfo
                        // =====================

class baetzo_Zoneinfo {
    // This class is a value-semantic type holding a structured representation
    // of the information contained in an Zoneinfo (or "Olson") time zone
    // database for a *single* locale (e.g., "America/New_York").  The salient
    // attributes of this type are the string identifier and the ordered
    // sequence of 'baetzo_ZoneinfoTransition' objects.
    //
    // This class:
    //: o supports a complete set of *value* *semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology: 'see bsldoc_glossary'.

    // PRIVATE TYPES
    class DescriptorLess {
        // This 'class' is a private functor that provides a comparator
        // predicate for the type 'baetzo_LocalTimeDescriptor', so that it can
        // be stored in associative containers such as 'bsl::set'.

      public:
        bool operator()(const baetzo_LocalTimeDescriptor& lhs,
                        const baetzo_LocalTimeDescriptor& rhs) const;
            // Return 'true' if the value of the specified 'lhs' is less than
            // (ordered before) the value of the specified 'rhs'.  Note that
            // the value of 'lhs' is less than the value of 'rhs' if the value
            // of the corresponding 'utcOffsetInSeconds', 'description', and
            // 'dstInEffectFlag' attributes of 'lhs' when incrementally
            // compared one at a time in that order is less than the attribute
            // value of 'rhs'.  Also note that the comparison moves to the next
            // attribute only when the corresponding attribute values compare
            // equal.
    };

    typedef bsl::vector<baetzo_ZoneinfoTransition> TransitionSequence;
        // Alias for the sequence of transitions that characterize a
        // 'baetzo_Zoneinfo' object.

    typedef bsl::set<baetzo_LocalTimeDescriptor, DescriptorLess> DescriptorSet;
        // Alias for the set of unique local-time descriptors that are managed
        // by a 'baetzo_Zoneinfo' object.

    // DATA
    bsl::string         d_identifier;   // this time zone's id

    DescriptorSet       d_descriptors;  // set of local time descriptors for
                                        // this time zone (e.g., 'EST')

    TransitionSequence  d_transitions;  // transitions, from one local time
                                        // descriptor to another (e.g., 'EST'
                                        // to 'EDT'), ordered by the time the
                                        // transition occurred (or will occur)

    bslma_Allocator    *d_allocator_p;  // allocator used to supply memory
                                        // (held, not owned)

    // FRIENDS
    friend bool operator==(const baetzo_Zoneinfo&, const baetzo_Zoneinfo&);

  public:
    // TYPES
    typedef TransitionSequence::const_iterator TransitionConstIterator;
        // Alias for a bi-directional 'const' iterator over the sequence of
        // transitions maintained by a 'baetzo_Zoneinfo' object.

    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(baetzo_Zoneinfo,
                                 bslalg_TypeTraitUsesBslmaAllocator);

    // CREATORS
    explicit baetzo_Zoneinfo(bslma_Allocator *basicAllocator = 0);
        // Create a 'baetzo_Zoneinfo' object having the values:
        //: o 'numTransitions() == 0'
        //: o 'identifier()     == ""'
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    baetzo_Zoneinfo(const baetzo_Zoneinfo&  original,
                    bslma_Allocator        *basicAllocator = 0);
        // Create a 'baetzo_Zoneinfo' object having the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

    // MANIPULATORS
    baetzo_Zoneinfo& operator=(const baetzo_Zoneinfo& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void addTransition(bdetu_Epoch::TimeT64              utcTime,
                       const baetzo_LocalTimeDescriptor& descriptor);
        // Add to this object a transition occurring at the specified 'utcTime'
        // when the local time in the described time-zone adopts the
        // characteristics of the specified 'descriptor'.  If a transition at
        // 'utcTime' is already present, replace it's local-time descriptor
        // with 'descriptor'.

    void setIdentifier(const bdeut_StringRef& value);
        // Set the 'identifier' attribute of this object to the specified
        // 'value'.

    void swap(baetzo_Zoneinfo& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    bslma_Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.

    TransitionConstIterator findTransitionForUtcTime(
                                           const bdet_Datetime& utcTime) const;
        // Return an iterator providing non-modifiable access to the transition
        // that holds the local-time descriptor associated with the specified
        // 'utcTime'.  The behavior is undefined unless 'numTransitions() > 0'
        // and the specified 'utcTime' is later than the transition returned by
        // 'firstTransition'.

    const baetzo_ZoneinfoTransition& firstTransition() const;
        // Return a reference providing non-modifiable access to the first
        // transition contained in this object.  The behavior is undefined
        // unless 'numTransitions() > 0'.

    const bsl::string& identifier() const;
        // Return a reference providing non-modifiable access to the
        // 'identifier' attribute of this object.

    bsl::size_t numTransitions() const;
       // Return the number of transitions maintained by this zone info.

    TransitionConstIterator beginTransitions() const;
       // Return an iterator providing non-modifiable access to the first
       // transition in the ordered sequence of transitions maintained by this
       // object.  Note that if 'beginTransitions() == endTransitions()' then
       // there are no transitions stored by this object.

    TransitionConstIterator endTransitions() const;
       // Return an iterator providing non-modifiable access to the one-past
       // the last transition in the ordered sequence of transitions that is
       // associated with this object.  Note that if
       // 'beginTransitions() == endTransitions()' then there are no
       // transitions stored by this object.

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute
        // value indicates the number of spaces per indentation level for this
        // and all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the
        // format is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const baetzo_Zoneinfo& lhs, const baetzo_Zoneinfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'baetzo_Zoneinfo' objects have the
    // same value if the corresponding value of their 'identifier' attribute is
    // the same and if both store the same sequence of transitions, ordered by
    // time.

bool operator!=(const baetzo_Zoneinfo& lhs, const baetzo_Zoneinfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baetzo_Zoneinfo' objects do not
    // have the same value if their corresponding 'identifier' attribute does
    // not have the same value, or if both do *not* store the same sequence of
    // transitions, ordered by time.

bsl::ostream& operator<<(bsl::ostream&          stream,
                         const baetzo_Zoneinfo& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// FREE FUNCTIONS
void swap(baetzo_Zoneinfo& a, baetzo_Zoneinfo& b);
    // Swap the value of the specified 'a' object with the value of the
    // specified 'b' object.  This method provides the no-throw guarantee.  The
    // behavior is undefined if the two objects being swapped have non-equal
    // allocators.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -------------------------------
                        // class baetzo_ZoneinfoTransition
                        // -------------------------------

// PRIVATE CREATORS
inline
baetzo_ZoneinfoTransition::baetzo_ZoneinfoTransition(
                                  bdetu_Epoch::TimeT64              utcTime,
                                  const baetzo_LocalTimeDescriptor *descriptor)
: d_utcTime(utcTime)
, d_descriptor_p(descriptor)
{
    BSLS_ASSERT_SAFE(descriptor);
}

// CREATORS
#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
inline
baetzo_ZoneinfoTransition::~baetzo_ZoneinfoTransition()
{
    BSLS_ASSERT_SAFE(d_descriptor_p);
}
#endif

// ACCESSORS
inline
const baetzo_LocalTimeDescriptor& baetzo_ZoneinfoTransition::descriptor() const
{
    return *d_descriptor_p;
}

inline
bdetu_Epoch::TimeT64 baetzo_ZoneinfoTransition::utcTime() const
{
    return d_utcTime;
}

// FREE OPERATORS
inline
bool operator==(const baetzo_ZoneinfoTransition& lhs,
                const baetzo_ZoneinfoTransition& rhs)
{
    return  lhs.utcTime()    == rhs.utcTime()
         && lhs.descriptor() == rhs.descriptor();
}

inline
bool operator!=(const baetzo_ZoneinfoTransition& lhs,
                const baetzo_ZoneinfoTransition& rhs)
{
    return  !(lhs == rhs);
}

inline
bool operator<(const baetzo_ZoneinfoTransition& lhs,
               const baetzo_ZoneinfoTransition& rhs)
{
    return lhs.utcTime() < rhs.utcTime();
}

                        // ---------------------
                        // class baetzo_Zoneinfo
                        // ---------------------


// CREATORS
inline
baetzo_Zoneinfo::baetzo_Zoneinfo(bslma_Allocator *basicAllocator)
: d_identifier(basicAllocator)
, d_descriptors(basicAllocator)
, d_transitions(basicAllocator)
, d_allocator_p(bslma_Default::allocator(basicAllocator))
{
}

// MANIPULATORS
inline
baetzo_Zoneinfo& baetzo_Zoneinfo::operator=(const baetzo_Zoneinfo& rhs)
{
     baetzo_Zoneinfo(rhs, d_allocator_p).swap(*this);
     return *this;
}

inline
void baetzo_Zoneinfo::setIdentifier(const bdeut_StringRef& identifier)
{
    BSLS_ASSERT_SAFE(identifier.isBound());

    d_identifier.assign(identifier.begin(), identifier.end());
}

inline
void baetzo_Zoneinfo::swap(baetzo_Zoneinfo& other)
{
    BSLS_ASSERT_SAFE(allocator() == other.allocator());

    bsl::swap(d_identifier,  other.d_identifier);
    bsl::swap(d_descriptors, other.d_descriptors);
    bsl::swap(d_transitions, other.d_transitions);
}

// ACCESSORS
inline
bslma_Allocator *baetzo_Zoneinfo::allocator() const
{
    return d_allocator_p;
}

inline
const baetzo_ZoneinfoTransition& baetzo_Zoneinfo::firstTransition() const
{
    BSLS_ASSERT_SAFE(numTransitions() > 0);

    return d_transitions.front();
}

inline
const bsl::string& baetzo_Zoneinfo::identifier() const
{
    return d_identifier;
}

inline
bsl::size_t baetzo_Zoneinfo::numTransitions() const
{
    return d_transitions.size();
}

inline
baetzo_Zoneinfo::TransitionConstIterator
baetzo_Zoneinfo::beginTransitions() const
{
    return d_transitions.begin();
}

inline
baetzo_Zoneinfo::TransitionConstIterator
baetzo_Zoneinfo::endTransitions() const
{
    return d_transitions.end();
}

// FREE OPERATORS
inline
bool operator==(const baetzo_Zoneinfo& lhs, const baetzo_Zoneinfo& rhs)
{
    return lhs.identifier()     == rhs.identifier()
        && lhs.numTransitions() == rhs.numTransitions()
        && bsl::equal(lhs.d_transitions.begin(),
                      lhs.d_transitions.end(),
                      rhs.d_transitions.begin());
}

inline
bool operator!=(const baetzo_Zoneinfo& lhs, const baetzo_Zoneinfo& rhs)
{
    return !(lhs == rhs);
}

// FREE FUNCTIONS
inline
void swap(baetzo_Zoneinfo& a, baetzo_Zoneinfo& b)
{
    a.swap(b);
}

}  // close namespace BloombergLP


#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P.  (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
