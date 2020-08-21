// baltzo_zoneinfo.h                                                  -*-C++-*-
#ifndef INCLUDED_BALTZO_ZONEINFO
#define INCLUDED_BALTZO_ZONEINFO

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a value type to represent a time zone.
//
//@CLASSES:
//  baltzo::Zoneinfo: information about a time zone
//  baltzo::ZoneinfoTransition: attributes representing a time transition
//
//@SEE_ALSO: baltzo_localtimedescriptor, baltzo_zoneinfoutil
//
//@DESCRIPTION: This component provides a *value* *semantic* type,
// 'baltzo::Zoneinfo', that represents the information about a specific time
// zone contained in the Zoneinfo database.  In addition, this component
// provides an unconstrained *in-core* *value-semantic* type
// 'baltzo::ZoneinfoTransition' that can be used to characterize a transition
// for time zones.
//
///'baltzo::ZoneinfoTransition'
///----------------------------
// A 'baltzo::ZoneinfoTransition' contains:
//..
//  Name        Type
//  ----------  --------------------------
//  utcTime     bdlt::EpochUtil::TimeT64
//  descriptor  baltzo::LocalTimeDescriptor
//..
//: o 'utcTime': UTC time when a transition occurs
//:
//: o 'descriptor': local time value corresponding to the time transition
//
// For example, in New York on March 14, 2011, at the instant 1 a.m., clocks
// are set forward by an hour to mark the transition from Eastern Standard Time
// to Eastern Daylight Time.  This change can be represented by a
// 'baltzo::ZoneinfoTransition' object whose 'utcTime' refers to March 14,
// 2011, 1am and whose 'descriptor' describes Eastern Daylight Time (i.e.,
// description is "EDT", 'dstInEffectFlag' is 'true', and 'utcOffsetInSeconds'
// is -14,400 (-4 * 60 * 60)).
//
///'baltzo::Zoneinfo'
///------------------
// A 'baltzo::Zoneinfo' contains:
//
//: o the time zone identifier (e.g., "America/New_York" or "Asia/Tokyo")
//:
//: o the ordered sequence of 'baltzo::ZoneinfoTransition' objects,
//:   representing the various transitions from UTC for this time zone.
//:
//: o an optional POSIX-like TZ environment string used to represent
//:   far-reaching times past the end of the explicit time zone data.
//
// A 'baltzo::Zoneinfo' object also provides the method
// 'findTransitionForUtcTime' that allows a client to find, in the sequence of
// transitions, the appropriate transition whose local-time descriptor
// describes the properties of local time, at a specified UTC time, for the
// time zone in question.  Note that, even though this information is
// sufficient for converting local date and time, to their corresponding values
// in other time zones, clients are encouraged to use the utilities provided in
// 'baltzo_timezoneutil'.  Also note that, 'baltzo::Zoneinfo' objects are
// typically populated by the client through the 'baltzo::Loader' protocol, and
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
// found online at 'http://www.iana.org/time-zones/repository/tz-link.html'.
// This time zone information can be used to perform the conversion of dates
// and times from UTC to their corresponding dates and times in a given time
// zone and vice-versa.  (See 'baltzo_zoneinfobinaryreader' for more
// information about the binary file format.)
//
///posixExtendedRangeDescription
///-----------------------------
// This string may be populated with a POSIX-like TZ string that describes
// rules for local time are handled before the first and after the last
// local-time transitions maintained by this object.  Typically this is used
// for computing local time values far in the future.  The rules for the
// encoded string can be found online at
// 'http://www.ibm.com/developerworks/aix/library/au-aix-posix/'.
//
///Usage
///-----
// The following usage examples illustrate how to populate a 'baltzo::Zoneinfo'
// object and use it to transform a UTC time into a local time.
//
///Example 1: Populate a 'baltzo::Zoneinfo'
/// - - - - - - - - - - - - - - - - - - - -
// Suppose we want to represent the time-zone information for New York, in 2010
// using a 'baltzo::Zoneinfo' object.  In order to do so, we need to provide
// the UTC date-times (transitions) after which the time zone changes its
// offset from UTC, or daylight-saving Time starts or ends.
//
// First, we define two times "Mar 14, 2010 6:00 AM" and "Nov 07, 2010 7:00 AM"
// representing respectively the UTC time at which New York transitions to
// Eastern Daylight-saving Time (EDT) and Eastern Standard Time (EST) in 2010:
//..
//  bdlt::Datetime edtDatetime(2010, 03, 14, 2, 0);
//  bdlt::Datetime estDatetime(2010, 11, 07, 2, 0);
//..
// Then, we create two local-time descriptors that hold the offset from UTC and
// DST status for EST and EDT in New York in 2010, in terms of their
// 'offsetFromUtcInSeconds', 'dstInEffectFlag' and 'description' attributes:
//..
//  const baltzo::LocalTimeDescriptor est(-5 * 60 * 60, false, "EST");
//  const baltzo::LocalTimeDescriptor edt(-4 * 60 * 60, true,  "EDT");
//..
// Note that these descriptors will be associated with the created transition
// times, to reflect how local time in New York changes its offset from UTC and
// DST status after specific times.
//
// Next, we create an empty 'baltzo::Zoneinfo' object that will be populated
// with the information necessary to describe the time zone for New York:
//..
//  baltzo::Zoneinfo newYorkTimeZone;
//..
// Then, before being able to associate the transition times with their
// corresponding descriptors, we need translate the transition times to
// 'bdlt::EpochUtil::TimeT64':
//..
//  bdlt::EpochUtil::TimeT64 edtTransitionTime =
//                              bdlt::EpochUtil::convertToTimeT64(edtDatetime);
//  bdlt::EpochUtil::TimeT64 estTransitionTime =
//                              bdlt::EpochUtil::convertToTimeT64(estDatetime);
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
//      baltzo::Zoneinfo::TransitionConstIterator it =
//                                          newYorkTimeZone.beginTransitions();
//
//      assert(it->utcTime()    == edtTransitionTime);
//      assert(it->descriptor() == edt);
//      ++it;
//      assert(it->utcTime()    == estTransitionTime);
//      assert(it->descriptor() == est);
//  }
//..
// Notice that the iterator refers to a 'baltzo::ZoneinfoTransition' object.
//
///Example 2: Converting UTC to Local Time
///- - - - - - - - - - - - - - - - - - - -
// Suppose that we want to program a function, 'utcToLocalTime' to perform the
// conversion from a UTC time value to the corresponding local time value, in a
// given time zone, and return the computed local time:
//
// First we declare the function 'utcToLocalTime' and its contract:
//..
//  static bdlt::DatetimeTz utcToLocalTime(const bdlt::Datetime&   utcTime,
//                                        const baltzo::Zoneinfo& timeZone)
//  {
//      // Return the 'bdlt::DatetimeTz' value representing the date, time and
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
//                                 bdlt::EpochUtil::convertToTimeT64(utcTime));
//..
// Next, we obtain the appropriate 'baltzo::ZoneinfoTransition' object,
// invoking the method 'findTransitionForUtcTime' on 'timeZone':
//..
//      baltzo::Zoneinfo::TransitionConstIterator it =
//                                  timeZone.findTransitionForUtcTime(utcTime);
//..
// Then, we access the descriptor associated with the transition to which 'it'
// refers, and calculate the offset from UTC rounded to the minute:
//..
//      const baltzo::ZoneinfoTransition& transition = *it;
//      const int offsetInMinutes =
//                           transition.descriptor().utcOffsetInSeconds() / 60;
//..
// Now, we apply the obtained 'offsetInMinutes' to the originally specified
// 'utcTime' obtaining the corresponding local time in the specified
// 'timeZone':
//..
//      bdlt::Datetime temp(utcTime);
//      temp.addMinutes(offsetInMinutes);
//..
// Finally, return the local time value together with its offset from UTC:
//..
//      return bdlt::DatetimeTz(temp, offsetInMinutes);
//  }
//..
// Suppose, now, we want to convert UTC time to the corresponding local time in
// New York.  We can do so using the previously defined function
// 'utcToLocalTime' and reusing the 'baltzo::Zoneinfo' object,
// 'newYorkTimeZone' of Example 1.
//
// First, we define 'bdlt::Datetime' object representing the UTC time "Apr 10,
// 2010 12:00":
//..
//  bdlt::Datetime utcDatetime(2010, 04, 10, 12, 0, 0);
//..
// Then, we invoke 'utcToLocalTime' passing 'newYorkTimeZone' as a time zone
// and save the result:
//..
//  bdlt::DatetimeTz nyDatetime = utcToLocalTime(utcDatetime, newYorkTimeZone);
//..
// Finally, we compute the New York local time corresponding to 'utcDatetime',
// verify that "April 10, 2010 8:00" is the computed time:
//..
//  const bdlt::Datetime expectedTime(2010, 4, 10, 8, 0, 0);
//  assert(-4 * 60      == nyDatetime.offset());
//  assert(expectedTime == nyDatetime.localDatetime());
//..

#include <balscm_version.h>

#include <baltzo_localtimedescriptor.h>

#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>

#include <bslalg_swaputil.h>

#include <bslma_allocator.h>
#include <bslma_stdallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_isbitwisemoveable.h>
#include <bslmf_movableref.h>
#include <bslmf_nestedtraitdeclaration.h>

#include <bsls_assert.h>
#include <bsls_keyword.h>
#include <bsls_review.h>
#include <bsls_types.h>

#include <bsl_cstring.h>
#include <bsl_iosfwd.h>
#include <bsl_set.h>
#include <bsl_string.h>
#include <bsl_vector.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsl_algorithm.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace baltzo {

                          // ========================
                          // class ZoneinfoTransition
                          // ========================

class ZoneinfoTransition {
    // This class is an unconstrained *in-core* value-semantic class that
    // characterizes a transition when the local time value of a time-zone
    // changes.  The salient attributes of this type are the 'utcTime'
    // (representing seconds from UTC), and 'descriptor' representing the local
    // time value after the transition.

    // DATA
    bdlt::EpochUtil::TimeT64   d_utcTime;       // UTC time (representing in
                                                // seconds from epoch) when the
                                                // time transition occurs

    const LocalTimeDescriptor *d_descriptor_p;  // pointer to the descriptor
                                                // associated with this
                                                // transition (held, not owned)

    // FRIENDS
    friend class Zoneinfo;

    // PRIVATE CREATORS
    ZoneinfoTransition(bdlt::EpochUtil::TimeT64   utcTime,
                       const LocalTimeDescriptor *descriptor);
        // Create a 'ZoneinfoTransition' object having the specified 'utcTime'
        // and, 'descriptor' attribute values.  The behavior is undefined
        // unless 'descriptor' remains valid for the lifetime of this object.

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(ZoneinfoTransition,
                                   bslmf::IsBitwiseMoveable);

    // CREATORS
    ~ZoneinfoTransition();
        // Destroy this object.

    // ACCESSORS
    const LocalTimeDescriptor& descriptor() const;
        // Return a reference providing non-modifiable access to the
        // 'descriptor' attribute of this object.

    bdlt::EpochUtil::TimeT64 utcTime() const;
        // Return the value of the 'utcTime' attribute of this object.

                                // Aspects

    bsl::ostream& print(bsl::ostream& stream,
                        int           level = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the format
        // is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const ZoneinfoTransition& lhs, const ZoneinfoTransition& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'ZoneinfoTransition' objects have the
    // same value if the corresponding value of their 'utcTime' attribute is
    // the same and both refer to the same 'descriptor' address.

bool operator!=(const ZoneinfoTransition& lhs, const ZoneinfoTransition& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'ZoneinfoTransition' objects do
    // not have the same value if the corresponding value of their 'utcTime' is
    // not the same or if they do not refer to the same 'descriptor' address.

bool operator<(const ZoneinfoTransition& lhs, const ZoneinfoTransition& rhs);
    // Return 'true' if the value of the specified 'lhs' is less than (ordered
    // before) the value of the specified 'rhs'.  Note that the value of 'lhs'
    // is less than the value of 'rhs' if the value of the 'utcTime' attribute
    // of 'lhs' is less than the value of the 'utcTime' attribute of 'rhs'.

bsl::ostream& operator<<(bsl::ostream&             stream,
                         const ZoneinfoTransition& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)' with the attribute names elided.

                               // ==============
                               // class Zoneinfo
                               // ==============

class Zoneinfo {
    // This class is a value-semantic type holding a structured representation
    // of the information contained in an Zoneinfo (or "Olson") time zone
    // database for a *single* locale (e.g., "America/New_York").  The salient
    // attributes of this type are the string identifier and the ordered
    // sequence of 'ZoneinfoTransition' objects.

    // PRIVATE TYPES
    class DescriptorLess {
        // This 'class' is a private functor that provides a comparator
        // predicate for the type 'LocalTimeDescriptor', so that it can be
        // stored in associative containers such as 'bsl::set'.

      public:
        bool operator()(const LocalTimeDescriptor& lhs,
                        const LocalTimeDescriptor& rhs) const;
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

    typedef bsl::vector<ZoneinfoTransition> TransitionSequence;
        // Alias for the sequence of transitions that characterize a 'Zoneinfo'
        // object.

    typedef bsl::set<LocalTimeDescriptor, DescriptorLess> DescriptorSet;
        // Alias for the set of unique local-time descriptors that are managed
        // by a 'Zoneinfo' object.

    // DATA
    bsl::string         d_identifier;
                          // this time zone's id

    DescriptorSet       d_descriptors;
                          // set of local time descriptors for this time zone
                          // (e.g., 'EST')

    TransitionSequence  d_transitions;
                          // transitions, from one local time descriptor to
                          // another (e.g., 'EST' to 'EDT'), ordered by the
                          // time the transition occurred (or will occur)

    bsl::string         d_posixExtendedRangeDescription;
                          // optional POSIX-like TZ environment string
                          // representing far-reaching times

    // FRIENDS
    friend bool operator==(const Zoneinfo&, const Zoneinfo&);

  public:
#ifndef BDE_OPENSOURCE_PUBLICATION
    // CLASS METHODS
    static bdlt::EpochUtil::TimeT64 convertToTimeT64(
                                               const bdlt::Datetime& datetime);
        // [!DEPRECATED!]: Do not use.  This method is provided in BDE 2.23 to
        // allow the conversion to 'TimeT64' without generating 'bsls_log'
        // output.
        //
        // Return the relative time computed as the difference between the
        // specified absolute 'datetime' and the epoch.


    static int convertFromTimeT64(bdlt::Datetime           *result,
                                  bdlt::EpochUtil::TimeT64  time);
        // [!DEPRECATED!]: Do not use.  This method is provided in BDE 2.23 to
        // allow the conversion from 'TimeT64' without generating 'bsls_log'
        // output.
        //
        // Load into the specified 'result' the absolute datetime computed as
        // the sum of the specified relative 'time' and the epoch.  Return 0 on
        // success, and a non-zero value otherwise.
#endif

    // TYPES
    typedef bsl::allocator<char> allocator_type;

    typedef TransitionSequence::const_iterator TransitionConstIterator;
        // Alias for a bi-directional 'const' iterator over the sequence of
        // transitions maintained by a 'Zoneinfo' object.

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(Zoneinfo, bslma::UsesBslmaAllocator);

    // CREATORS
    Zoneinfo();
    explicit Zoneinfo(const allocator_type& allocator);
        // Create a 'Zoneinfo' object having the values:
        //..
        //  numTransitions() == 0
        //  identifier()     == ""
        //..
        // Optionally specify an 'allocator' (e.g., the address of a
        // 'bslma::Allocator' object) to supply memory; otherwise, the default
        // allocator is used.

    Zoneinfo(const Zoneinfo&       original,
             const allocator_type& allocator = allocator_type());
        // Create a 'Zoneinfo' object having the same value as the specified
        // 'original' object.  Optionally specify an 'allocator' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory; otherwise,
        // the default allocator is used.

    Zoneinfo(bslmf::MovableRef<Zoneinfo> original) BSLS_KEYWORD_NOEXCEPT;
        // Create a 'Zoneinfo' object having the same value and the same
        // allocator as the specified 'original' object.  The value of
        // 'original' becomes unspecified but valid, and its allocator remains
        // unchanged.

    Zoneinfo(bslmf::MovableRef<Zoneinfo> original,
             const allocator_type&       allocator);
        // Create a 'Zoneinfo' object having the same value as the specified
        // 'original' object, using the specified 'allocator' (e.g., the
        // address of a 'bslma::Allocator' object) to supply memory.  The
        // allocator of 'original' remains unchanged.  If 'original' and the
        // newly created object have the same allocator then the value of
        // 'original' becomes unspecified but valid, and no exceptions will be
        // thrown; otherwise 'original' is unchanged and an exception may be
        // thrown.

    // MANIPULATORS
    Zoneinfo& operator=(const Zoneinfo& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    Zoneinfo& operator=(bslmf::MovableRef<Zoneinfo> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.  The allocators of
        // this object and 'rhs' both remain unchanged.  If 'rhs' and this
        // object have the same allocator then the value of 'rhs' becomes
        // unspecified but valid, and no exceptions will be thrown; otherwise
        // 'rhs' is unchanged (and an exception may be thrown).

    void addTransition(bdlt::EpochUtil::TimeT64   utcTime,
                       const LocalTimeDescriptor& descriptor);
        // Add to this object a transition occurring at the specified 'utcTime'
        // when the local time in the described time-zone adopts the
        // characteristics of the specified 'descriptor'.  If a transition at
        // 'utcTime' is already present, replace it's local-time descriptor
        // with 'descriptor'.

    void setIdentifier(const bslstl::StringRef&  value);
    void setIdentifier(const char               *value);
        // Set the 'identifier' attribute of this object to the specified
        // 'value'.

    void setPosixExtendedRangeDescription(const bslstl::StringRef&  value);
    void setPosixExtendedRangeDescription(const char               *value);
        // Set the 'posixExtendedRangeDescription' attribute of this object,
        // used to describe local time transitions far in the future, to the
        // specified 'value' (see {posixExtendedRangeDescription}).

    void swap(Zoneinfo& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // !DEPRECATED!: Use 'get_allocator()' instead.
        //
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the currently
        // installed default allocator is used.

    TransitionConstIterator findTransitionForUtcTime(
                                          const bdlt::Datetime& utcTime) const;
        // Return an iterator providing non-modifiable access to the transition
        // that holds the local-time descriptor associated with the specified
        // 'utcTime'.  The behavior is undefined unless 'numTransitions() > 0'
        // and 'utcTime' is at or after the transition returned by
        // 'firstTransition'.

    const ZoneinfoTransition& firstTransition() const;
        // Return a reference providing non-modifiable access to the first
        // transition contained in this object.  The behavior is undefined
        // unless 'numTransitions() > 0'.

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.

    const bsl::string& identifier() const;
        // Return a reference providing non-modifiable access to the
        // 'identifier' attribute of this object.

    const bsl::string& posixExtendedRangeDescription() const;
        // Return a reference providing non-modifiable access to the
        // 'posixExtendedRangeDescription' attribute of this object, used to
        // describe local time transitions far in the future (see
        // {posixExtendedRangeDescription}).

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
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference to 'stream'.
        // Optionally specify an initial indentation 'level', whose absolute
        // value is incremented recursively for nested objects.  If 'level' is
        // specified, optionally specify 'spacesPerLevel', whose absolute value
        // indicates the number of spaces per indentation level for this and
        // all of its nested objects.  If 'level' is negative, suppress
        // indentation of the first line.  If 'spacesPerLevel' is negative,
        // format the entire output on one line, suppressing all but the
        // initial indentation (as governed by 'level').  If 'stream' is not
        // valid on entry, this operation has no effect.  Note that the format
        // is not fully specified, and can change without notice.
};

// FREE OPERATORS
bool operator==(const Zoneinfo& lhs, const Zoneinfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'Zoneinfo' objects have the same
    // value if the corresponding value of their 'identifier' attribute is the
    // same and if both store the same sequence of transitions, ordered by
    // time.

bool operator!=(const Zoneinfo& lhs, const Zoneinfo& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'Zoneinfo' objects do not have
    // the same value if their corresponding 'identifier' attribute does not
    // have the same value, or if both do *not* store the same sequence of
    // transitions, ordered by time.

bsl::ostream& operator<<(bsl::ostream& stream, const Zoneinfo& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// FREE FUNCTIONS
void swap(Zoneinfo& a, Zoneinfo& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee if the two objects were
    // created with the same allocator and the basic guarantee otherwise.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                          // ------------------------
                          // class ZoneinfoTransition
                          // ------------------------

// PRIVATE CREATORS
inline
ZoneinfoTransition::ZoneinfoTransition(bdlt::EpochUtil::TimeT64   utcTime,
                                       const LocalTimeDescriptor *descriptor)
: d_utcTime(utcTime)
, d_descriptor_p(descriptor)
{
    BSLS_ASSERT(descriptor);
}

// CREATORS
inline
ZoneinfoTransition::~ZoneinfoTransition()
{
    BSLS_ASSERT(d_descriptor_p);
}

// ACCESSORS
inline
const LocalTimeDescriptor& ZoneinfoTransition::descriptor() const
{
    return *d_descriptor_p;
}

inline
bdlt::EpochUtil::TimeT64 ZoneinfoTransition::utcTime() const
{
    return d_utcTime;
}

}  // close package namespace

// FREE OPERATORS
inline
bool baltzo::operator==(const ZoneinfoTransition& lhs,
                        const ZoneinfoTransition& rhs)
{
    return  lhs.utcTime()    == rhs.utcTime()
         && lhs.descriptor() == rhs.descriptor();
}

inline
bool baltzo::operator!=(const ZoneinfoTransition& lhs,
                        const ZoneinfoTransition& rhs)
{
    return  !(lhs == rhs);
}

inline
bool baltzo::operator<(const ZoneinfoTransition& lhs,
                       const ZoneinfoTransition& rhs)
{
    return lhs.utcTime() < rhs.utcTime();
}

namespace baltzo {

                               // --------------
                               // class Zoneinfo
                               // --------------

// CREATORS
inline
Zoneinfo::Zoneinfo()
: d_identifier()
, d_descriptors()
, d_transitions()
, d_posixExtendedRangeDescription()
{
}

inline
Zoneinfo::Zoneinfo(const allocator_type& allocator)
: d_identifier(allocator)
, d_descriptors(allocator)
, d_transitions(allocator)
, d_posixExtendedRangeDescription(allocator)
{
}

// MANIPULATORS
inline
Zoneinfo& Zoneinfo::operator=(const Zoneinfo& rhs)
{
    Zoneinfo(rhs, get_allocator()).swap(*this);
    return *this;
}

inline
void Zoneinfo::setIdentifier(const bslstl::StringRef& value)
{
    BSLS_ASSERT(0 != value.data());

    d_identifier.assign(value.begin(), value.end());
}

inline
void Zoneinfo::setIdentifier(const char *value)
{
    BSLS_ASSERT(value);

    bsl::string(value, get_allocator()).swap(d_identifier);
}

inline
void Zoneinfo::setPosixExtendedRangeDescription(const bslstl::StringRef& value)
{
    BSLS_ASSERT(0 != value.data());

    d_posixExtendedRangeDescription.assign(value.begin(), value.end());
}

inline
void Zoneinfo::setPosixExtendedRangeDescription(const char *value)
{
    BSLS_ASSERT(value);

    d_posixExtendedRangeDescription.assign(value, value + bsl::strlen(value));
}

inline
void Zoneinfo::swap(Zoneinfo& other)
{
    BSLS_ASSERT(get_allocator() == other.get_allocator());

    bslalg::SwapUtil::swap(&d_identifier,  &other.d_identifier);
    bslalg::SwapUtil::swap(&d_descriptors, &other.d_descriptors);
    bslalg::SwapUtil::swap(&d_transitions, &other.d_transitions);
    bslalg::SwapUtil::swap(&d_posixExtendedRangeDescription,
                           &other.d_posixExtendedRangeDescription);
}

// ACCESSORS
inline
bslma::Allocator *Zoneinfo::allocator() const
{
    return get_allocator().mechanism();
}

inline
const ZoneinfoTransition& Zoneinfo::firstTransition() const
{
    BSLS_ASSERT(numTransitions() > 0);

    return d_transitions.front();
}

inline
Zoneinfo::allocator_type Zoneinfo::get_allocator() const
{
    return d_identifier.get_allocator();
}

inline
const bsl::string& Zoneinfo::identifier() const
{
    return d_identifier;
}

inline
const bsl::string& Zoneinfo::posixExtendedRangeDescription() const
{
    return d_posixExtendedRangeDescription;
}

inline
bsl::size_t Zoneinfo::numTransitions() const
{
    return d_transitions.size();
}

inline
Zoneinfo::TransitionConstIterator Zoneinfo::beginTransitions() const
{
    return d_transitions.begin();
}

inline
Zoneinfo::TransitionConstIterator Zoneinfo::endTransitions() const
{
    return d_transitions.end();
}

}  // close package namespace

// FREE OPERATORS
inline
bool baltzo::operator==(const Zoneinfo& lhs, const Zoneinfo& rhs)
{
    return lhs.identifier() == rhs.identifier()
        && lhs.posixExtendedRangeDescription() ==
                                            rhs.posixExtendedRangeDescription()
        && lhs.numTransitions() == rhs.numTransitions()
        && bsl::equal(lhs.d_transitions.begin(),
                      lhs.d_transitions.end(),
                      rhs.d_transitions.begin());
}

inline
bool baltzo::operator!=(const Zoneinfo& lhs, const Zoneinfo& rhs)
{
    return !(lhs == rhs);
}

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2020 Bloomberg Finance L.P.
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
