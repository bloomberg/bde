// baltzo_localtimeperiod.h                                           -*-C++-*-
#ifndef INCLUDED_BALTZO_LOCALTIMEPERIOD
#define INCLUDED_BALTZO_LOCALTIMEPERIOD

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a type describing local time over a time period.
//
//@CLASSES:
//  baltzo::LocalTimePeriod: time descriptor and period of applicability
//
//@SEE_ALSO:  baltzo_localtimedescriptor, baltzo_zoneinfoutil
//
//@DESCRIPTION: This component provides a single, complex-constrained
// (value-semantic) attribute class, 'baltzo::LocalTimePeriod', that describes
// a period of time over which a local time description (UTC offset, DST
// status, and a descriptive string) is in effect.
//
///Attributes
///----------
//..
//  Name            Type                        Default  Simple Constraints
//  -------------   --------------------------  -------  ------------------
//  descriptor      baltzo::LocalTimeDescriptor  default  none
//  utcStartTime    bdlt::Datetime               default  none
//  utcEndTime      bdlt::Datetime               default  none
//
//  Complex Constraints
//  -----------------------------------------------------------------
//  'utcStartTime == utcEndTime ||
//  (utcStartTime != bdlt::Datetime() && utcEndTime != bdlt::Datetime()
//   && utcStartTime < utcEndTime)'
//..
//: o 'localTimeDescriptor': a description of local time that applies during
//:   the interval defined by 'startUtcTime' and 'endUtcTime'.
//:
//: o 'utcStartTime': UTC representation of the start of the time interval over
//:   which 'localTimeDescriptor' applies.
//:
//: o 'utcEndTime': UTC representation of the moment immediately after the end
//:   of the time interval over which 'localTimeDescriptor' applies.
//
// For example, in New York in 2010, the local time was Eastern Daylight Time
// ("EDT") from March 14, 2010 to November 7, 2010, and during Eastern Daylight
// Time, Daylight-Saving Time (DST) was in effect, and the offset from UTC was
// -4 hours.  We can represent this information using a
// 'baltzo::LocalTimePeriod' object whose 'utcStartTime' is "Mar 14, 2010 07:00
// UTC" (2AM EST), 'utcEndTime' is "Nov 7, 2010 06:00 UTC" (1AM EST, what would
// have been 2AM EDT), and 'localTimeDescriptor' has a 'description' of "EDT",
// 'dstInEffectFlag' of 'true', and a 'utcOffsetInSeconds' of -14,400
// (-4 * 60 * 60).
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Converting a UTC Time to a Local Time
/// - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to use a local time period to convert a
// UTC time to the corresponding local time in some time zone.
//
// First, we define a function that performs a conversion from UTC time to
// local time:
//..
//  int convertUtcToLocalTime(bdlt::Datetime                 *result,
//                            const bdlt::Datetime&           utcTime,
//                            const baltzo::LocalTimePeriod&  period)
//      // Load into the specified 'result' the date-time value corresponding
//      // to the specified 'utcTime' in the local time described by the
//      // specified 'period'.  Return 0 on success, and a non-zero value if
//      // 'utcTime < period.utcStartTime()' or
//      // 'utcTime >= period.utcEndTime()'.
//  {
//      BSLS_ASSERT(result);
//
//      if (utcTime <  period.utcStartTime() ||
//          utcTime >= period.utcEndTime()) {
//          return 1;                                                 // RETURN
//      }
//
//      *result = utcTime;
//      result->addSeconds(period.descriptor().utcOffsetInSeconds());
//      return 0;
//  }
//..
// Then, we create a 'baltzo::LocalTimePeriod' object, 'edt2010', that
// describes New York Daylight-Saving Time in 2010:
//..
//  enum { NEW_YORK_DST_OFFSET = -4 * 60 * 60 };  // -4 hours in seconds
//
//  baltzo::LocalTimeDescriptor edt(NEW_YORK_DST_OFFSET, true, "EDT");
//
//  baltzo::LocalTimePeriod     edt2010(edt,
//                                      bdlt::Datetime(2010,  3, 14, 7),
//                                      bdlt::Datetime(2010, 11,  7, 6));
//
//  assert(bdlt::Datetime(2010,  3, 14, 7) == edt2010.utcStartTime());
//  assert(bdlt::Datetime(2010, 11,  7, 6) == edt2010.utcEndTime());
//  assert("EDT" == edt2010.descriptor().description());
//  assert(true  == edt2010.descriptor().dstInEffectFlag());
//  assert(NEW_YORK_DST_OFFSET == edt2010.descriptor().utcOffsetInSeconds());
//..
// Next, we create a 'bdlt::Datetime', 'utcDatetime', representing the (UTC)
// time "Jul 20, 2010 11:00":
//..
//  bdlt::Datetime utcDatetime(2010, 7, 20, 11, 0, 0);
//..
// Now, we use the 'convertUtcToLocalTime' function we defined earlier to
// convert 'utcDatetime' into its local time in Eastern Daylight Time (as
// described by 'edt2010'):
//..
//  bdlt::Datetime localDatetime;
//  int            status = convertUtcToLocalTime(&localDatetime,
//                                                utcDatetime,
//                                                edt2010);
//  if (0 != status) {
//      // The conversion failed so return an error code.
//
//      return 1;                                                     // RETURN
//  }
//..
// Finally, we verify that the result corresponds to the expected local time in
// New York, "Jul 20, 2010 7:00":
//..
//  assert(bdlt::Datetime(2010, 7, 20, 7) == localDatetime);
//..

#include <balscm_version.h>

#include <baltzo_localdatetime.h>
#include <baltzo_localtimedescriptor.h>

#include <bdlt_datetime.h>

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

#include <bsl_iosfwd.h>

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsl_algorithm.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

namespace BloombergLP {
namespace baltzo {

                           // =====================
                           // class LocalTimePeriod
                           // =====================

class LocalTimePeriod {
    // This complex-constrained (value-semantic) attribute class describes a
    // range of time in which a particular local time description (offset, DST
    // status, and a descriptive string) is in effect.  See the Attributes
    // section under @DESCRIPTION in the component-level documentation.  Note
    // that the class invariants are identically the constraints on the
    // attributes.

    // DATA
    LocalTimeDescriptor d_descriptor;    // local time descriptor for this
                                         // period

    bdlt::Datetime      d_utcStartTime;  // start of this period

    bdlt::Datetime      d_utcEndTime;    // end of this period

  public:
    // TYPES
    typedef bsl::allocator<char> allocator_type;

    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(LocalTimePeriod, bslma::UsesBslmaAllocator);
    BSLMF_NESTED_TRAIT_DECLARATION(LocalTimePeriod, bslmf::IsBitwiseMoveable);

    // CLASS METHODS
    static bool isValidUtcStartAndEndTime(const bdlt::Datetime& utcStartTime,
                                          const bdlt::Datetime& utcEndTime);
        // Return 'true' if the specified 'utcStartTime' and 'utcEndTime' have
        // the same value, or if 'utcStartTime' and 'utcEndTime' are comparable
        // (i.e., neither has the value of a default-constructed
        // 'bdlt::DateTime' object) and 'utcStartTime < utcEndTime'; return
        // 'false' otherwise.

    // CREATORS
    LocalTimePeriod();
    explicit LocalTimePeriod(const allocator_type& allocator);
        // Create a 'LocalTimePeriod' object having the (default) attribute
        // values:
        //..
        //  descriptor()   == LocalTimeDescriptor()
        //  utcStartTime() == bdlt::Datetime()
        //  utcEndTime()   == bdlt::Datetime()
        //..
        // Optionally specify an 'allocator' (e.g., the address of a
        // 'bslma::Allocator' object) to supply memory; otherwise, the default
        // allocator is used.

    LocalTimePeriod(const LocalTimeDescriptor& descriptor,
                    const bdlt::Datetime&      utcStartTime,
                    const bdlt::Datetime&      utcEndTime,
                    const allocator_type&      allocator = allocator_type());
        // Create a 'LocalTimePeriod' object having the specified 'descriptor',
        // 'utcStartTime', and 'utcEndTime' attribute values.  Optionally
        // specify an 'allocator' (e.g., the address of a 'bslma::Allocator'
        // object) to supply memory; otherwise, the default allocator is used.
        // The behavior is undefined unless 'utcStartTime == utcEndTime', or if
        // 'utcStartTime' and 'utcEndTime' are comparable (i.e., neither equals
        // the a default constructed 'bdlt::DateTime' object) unless
        // 'utcStartTime < utcEndTime'.  (See the 'isValidUtcStartAndEndTime'
        // method.)

    LocalTimePeriod(const LocalTimePeriod& original,
                    const allocator_type&  allocator = allocator_type());
        // Create a 'LocalTimePeriod' object with the same value as the
        // specified 'original' object.  Optionally specify an 'allocator'
        // (e.g., the address of a 'bslma::Allocator' object) to supply memory;
        // otherwise, the default allocator is used.

    LocalTimePeriod(bslmf::MovableRef<LocalTimePeriod> original)
                                                         BSLS_KEYWORD_NOEXCEPT;
        // Create a 'LocalTimePeriod' object having the same value and the same
        // allocator as the specified 'original' object.  The value of
        // 'original' becomes unspecified but valid, and its allocator remains
        // unchanged.

    LocalTimePeriod(bslmf::MovableRef<LocalTimePeriod> original,
                    const allocator_type&              allocator);
        // Create a 'LocalTimePeriod' object having the same value as the
        // specified 'original' object, using the specified 'allocator' (e.g.,
        // the address of a 'bslma::Allocator' object) to supply memory.  The
        // allocator of 'original' remains unchanged.  If 'original' and the
        // newly created object have the same allocator then the value of
        // 'original' becomes unspecified but valid, and no exceptions will be
        // thrown; otherwise 'original' is unchanged and an exception may be
        // thrown.

    ~LocalTimePeriod();
        // Destroy this object.

    // MANIPULATORS
    LocalTimePeriod& operator=(const LocalTimePeriod& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    LocalTimePeriod& operator=(bslmf::MovableRef<LocalTimePeriod> rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a non-'const' reference to this object.  The allocators of
        // this object and 'rhs' both remain unchanged.  If 'rhs' and this
        // object have the same allocator then the value of 'rhs' becomes
        // unspecified but valid, and no exceptions will be thrown; otherwise
        // 'rhs' is unchanged (and an exception may be thrown).

    void setDescriptor(const LocalTimeDescriptor& value);
        // Set the 'descriptor' attribute to the specified 'value'.

    void setUtcStartAndEndTime(const bdlt::Datetime& utcStartTime,
                               const bdlt::Datetime& utcEndTime);
        // Use the specified 'utcStartTime' and 'utcEndTime' to set the
        // 'utcStartTime' and 'utcEndTime' attributes of this object
        // respectively.  The behavior is undefined unless
        // 'utcStartTime == utcEndTime', or if 'utcStartTime' and 'utcEndTime'
        // are comparable (i.e., neither equals a default constructed
        // 'bdlt::DateTime' object) unless 'utcStartTime < utcEndTime'.  (See
        // the 'isValidUtcStartAndEndTime' method.)

    void swap(LocalTimePeriod& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // !DEPRECATED!: Use 'get_allocator()' instead.
        //
        // Return 'get_allocator().mechanism()'.

    const LocalTimeDescriptor& descriptor() const;
        // Return a reference providing non-modifiable access to the
        // 'descriptor' attribute of this object.

    allocator_type get_allocator() const;
        // Return the allocator used by this object to supply memory.  Note
        // that if no allocator was supplied at construction the default
        // allocator in effect at construction is used.

    const bdlt::Datetime& utcEndTime() const;
        // Return a reference providing non-modifiable access to the
        // 'utcEndTime' attribute of this object.

    const bdlt::Datetime& utcStartTime() const;
        // Return a reference providing non-modifiable access to the
        // 'utcStartTime' attribute of this object.

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
bool operator==(const LocalTimePeriod& lhs, const LocalTimePeriod& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'LocalTimePeriod' objects have the
    // same value if each of their corresponding 'descriptor', 'utcStartTime',
    // and 'utcEndTime' attributes have the same value.

bool operator!=(const LocalTimePeriod& lhs, const LocalTimePeriod& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'LocalTimePeriod' objects do not
    // have the same value if the corresponding values of their 'descriptor',
    // 'utcStartTime', or 'utcEndTime' attributes are not the same.

std::ostream& operator<<(bsl::ostream&          stream,
                         const LocalTimePeriod& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)'.

// FREE FUNCTIONS
void swap(LocalTimePeriod& a, LocalTimePeriod& b);
    // Exchange the values of the specified 'a' and 'b' objects.  This function
    // provides the no-throw exception-safety guarantee if the two objects were
    // created with the same allocator and the basic guarantee otherwise.

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // ---------------------
                           // class LocalTimePeriod
                           // ---------------------

// CLASS METHODS
inline
bool LocalTimePeriod::isValidUtcStartAndEndTime(
                                            const bdlt::Datetime& utcStartTime,
                                            const bdlt::Datetime& utcEndTime)
{
    bdlt::Datetime defaultObj;
    return utcStartTime == utcEndTime
        || (utcStartTime != defaultObj && utcEndTime != defaultObj
            && utcStartTime < utcEndTime);
}

// CREATORS
inline
LocalTimePeriod::LocalTimePeriod()
: d_descriptor()
, d_utcStartTime()
, d_utcEndTime()
{
}

inline
LocalTimePeriod::LocalTimePeriod(const allocator_type& allocator)
: d_descriptor(allocator)
, d_utcStartTime()
, d_utcEndTime()
{
}

inline
LocalTimePeriod::LocalTimePeriod(const LocalTimeDescriptor& descriptor,
                                 const bdlt::Datetime&      utcStartTime,
                                 const bdlt::Datetime&      utcEndTime,
                                 const allocator_type&      allocator)
: d_descriptor(descriptor, allocator)
, d_utcStartTime(utcStartTime)
, d_utcEndTime(utcEndTime)
{
    BSLS_ASSERT(isValidUtcStartAndEndTime(d_utcStartTime, d_utcEndTime));
}

inline
LocalTimePeriod::LocalTimePeriod(const LocalTimePeriod& original,
                                 const allocator_type&  allocator)
: d_descriptor(original.d_descriptor, allocator)
, d_utcStartTime(original.d_utcStartTime)
, d_utcEndTime(original.d_utcEndTime)
{
}

inline
LocalTimePeriod::LocalTimePeriod(bslmf::MovableRef<LocalTimePeriod> original)
                                                          BSLS_KEYWORD_NOEXCEPT
: d_descriptor(bslmf::MovableRefUtil::move(
                         bslmf::MovableRefUtil::access(original).d_descriptor))
, d_utcStartTime(bslmf::MovableRefUtil::access(original).d_utcStartTime)
, d_utcEndTime(bslmf::MovableRefUtil::access(original).d_utcEndTime)
{
}

inline
LocalTimePeriod::LocalTimePeriod(bslmf::MovableRef<LocalTimePeriod> original,
                                 const allocator_type&              allocator)
: d_descriptor(bslmf::MovableRefUtil::move(
                         bslmf::MovableRefUtil::access(original).d_descriptor),
                         allocator)
, d_utcStartTime(bslmf::MovableRefUtil::access(original).d_utcStartTime)
, d_utcEndTime(bslmf::MovableRefUtil::access(original).d_utcEndTime)
{
}

inline
LocalTimePeriod::~LocalTimePeriod()
{
    BSLS_ASSERT(isValidUtcStartAndEndTime(d_utcStartTime, d_utcEndTime));
}

// MANIPULATORS
inline
LocalTimePeriod& LocalTimePeriod::operator=(const LocalTimePeriod& rhs)
{
    d_descriptor   = rhs.d_descriptor;    // must be first
    d_utcStartTime = rhs.d_utcStartTime;
    d_utcEndTime   = rhs.d_utcEndTime;
    return *this;
}

inline
LocalTimePeriod& LocalTimePeriod::operator=(
                                        bslmf::MovableRef<LocalTimePeriod> rhs)
{
    LocalTimePeriod& lvalue = rhs;

    // Move 'd_descriptor' first for strong exception guarantee.

    d_descriptor   = bslmf::MovableRefUtil::move(lvalue.d_descriptor);
    d_utcStartTime = bslmf::MovableRefUtil::move(lvalue.d_utcStartTime);
    d_utcEndTime   = bslmf::MovableRefUtil::move(lvalue.d_utcEndTime);

    return *this;
}

inline
void LocalTimePeriod::setDescriptor(const LocalTimeDescriptor& value)
{
    d_descriptor = value;
}

inline
void LocalTimePeriod::setUtcStartAndEndTime(const bdlt::Datetime& utcStartTime,
                                            const bdlt::Datetime& utcEndTime)
{
    BSLS_ASSERT_SAFE(isValidUtcStartAndEndTime(utcStartTime, utcEndTime));

    d_utcStartTime = utcStartTime;
    d_utcEndTime   = utcEndTime;
}

inline
void LocalTimePeriod::swap(LocalTimePeriod& other)
{
    // 'swap' is undefined for objects with non-equal allocators.

    BSLS_ASSERT(get_allocator() == other.get_allocator());

    bslalg::SwapUtil::swap(&d_descriptor,   &other.d_descriptor);
    bslalg::SwapUtil::swap(&d_utcStartTime, &other.d_utcStartTime);
    bslalg::SwapUtil::swap(&d_utcEndTime,   &other.d_utcEndTime);
}

// ACCESSORS
inline
bslma::Allocator *LocalTimePeriod::allocator() const
{
    return get_allocator().mechanism();
}

inline
const LocalTimeDescriptor& LocalTimePeriod::descriptor() const
{
    return d_descriptor;
}

inline
LocalTimePeriod::allocator_type LocalTimePeriod::get_allocator() const
{
    return d_descriptor.description().get_allocator();
}

inline
const bdlt::Datetime& LocalTimePeriod::utcEndTime() const
{
    return d_utcEndTime;
}

inline
const bdlt::Datetime& LocalTimePeriod::utcStartTime() const
{
    return d_utcStartTime;
}

}  // close package namespace

// FREE OPERATORS
inline
bool baltzo::operator==(const LocalTimePeriod& lhs, const LocalTimePeriod& rhs)
{
    return lhs.descriptor()   == rhs.descriptor()
        && lhs.utcStartTime() == rhs.utcStartTime()
        && lhs.utcEndTime()   == rhs.utcEndTime();
}

inline
bool baltzo::operator!=(const LocalTimePeriod& lhs, const LocalTimePeriod& rhs)
{
    return lhs.descriptor()   != rhs.descriptor()
        || lhs.utcStartTime() != rhs.utcStartTime()
        || lhs.utcEndTime()   != rhs.utcEndTime();
}

inline
std::ostream& baltzo::operator<<(bsl::ostream&          stream,
                                 const LocalTimePeriod& object)
{
    return object.print(stream, 0, -1);
}

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
