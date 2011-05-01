// baetzo_localtimeperiod.h                                           -*-C++-*-
#ifndef INCLUDED_BAETZO_LOCALTIMEPERIOD
#define INCLUDED_BAETZO_LOCALTIMEPERIOD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a type describing local time over a time period.
//
//@CLASSES:
//  baetzo_LocalTimePeriod: time descriptor and period of applicability
//
//@AUTHOR: Stefano Pacifico (spacifico1), Henry Verschell (hverschell)
//
//@SEE_ALSO:  baetzo_localtimedescriptor, baetzo_zoneinfoutil
//
//@DESCRIPTION: This component provides a single, complex-constrained
// (value-semantic) attribute class, 'baetzo_LocalTimerPeriod', that describes
// a period of time over which a local time description (UTC offset, DST
// status, and a descriptive string) is in effect.
//
///Attributes
///----------
//..
//  Name                 Type                   Default  Simple Constraints
//  -------------   -------------------         -------  ------------------
//  descriptor      baetzo_LocalTimeDescriptor  default  none
//  utcStartTime    bdet_Datetime               default  none
//  utcEndTime      bdet_Datetime               default  none
//
//  Complex Constraints
//  ----------------------------
//  'utcStartTime == utcEndTime ||
//  (utcStartTime != bdet_Datetime() && utcEndTime != bdet_Datetime()
//   && utcStartTime < utcEndTime)'
//..
//: o localTimeDescriptor: a description of local time that applies during the
//:   interval defined by 'startUtcTime' and 'endUtcTime'.
//:
//: o utcStartTime: UTC representation of the start of the time interval over
//:   which 'localTimeDescriptor' applies.
//:
//: o utcEndTime: UTC representation of the moment immediately after the end of
//:   the time interval over which 'localTimeDescriptor' applies.
//
// For example, in New York in 2010, the local time was Eastern Daylight Time
// ("EDT") from March 14, 2010 to November 7, 2010, and during Eastern Daylight
// Time, Daylight-Saving Time (DST) was in effect, and the offset from UTC was
// -4 hours.  We can represent this information using a
// 'baetzo_LocalTimePeriod' object whose 'utcStartTime' is "Mar 14, 2010 07:00
// UTC" (2AM EST), 'utcEndTime' is "Nov 7, 2010 06:00 UTC" (1AM EST, what would
// have been 2AM EDT), and 'localTimeDescriptor' has a 'description' of "EDT",
// 'dstInEffectFlag' of 'true', and a 'utcOffsetInSeconds' of -14,400
// (-4 * 60 * 60).
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Converting a UTC Time to a Local Time
/// - - - - - - - - - - - - - - - - - - - - - - - -
// In this example we illustrate how to use a local time period to convert a
// UTC time to the corresponding local time in some time zone.
//
// First, we define a function that performs a conversion from UTC time to
// local time:
//..
//  int convertUtcToLocalTime(bdet_Datetime                 *result,
//                            const bdet_Datetime&           utcTime,
//                            const baetzo_LocalTimePeriod&  period)
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
// Then, we create a 'baetzo_LocalTimePeriod' object, 'edt2010', that describes
// New York Daylight-Saving Time in 2010:
//..
//  enum { NEW_YORK_DST_OFFSET = -4 * 60 * 60 };  // -4 hours in seconds
//
//  baetzo_LocalTimeDescriptor edt(NEW_YORK_DST_OFFSET, true, "EDT");
//
//  baetzo_LocalTimePeriod edt2010(edt,
//                                 bdet_Datetime(2010,  3, 14, 7),
//                                 bdet_Datetime(2010, 11,  7, 6));
//
//  assert(bdet_Datetime(2010,  3, 14, 7) == edt2010.utcStartTime());
//  assert(bdet_Datetime(2010, 11,  7, 6) == edt2010.utcEndTime());
//  assert("EDT" == edt2010.descriptor().description());
//  assert(true  == edt2010.descriptor().dstInEffectFlag());
//  assert(NEW_YORK_DST_OFFSET == edt2010.descriptor().utcOffsetInSeconds());
//..
// Next, we create a 'bdet_Datetime', 'utcDateTime', representing the (UTC)
// time "Jul 20, 2010 11:00":
//..
//  bdet_Datetime utcDatetime(2010, 7, 20, 11, 0, 0);
//..
// Now, we use the 'convertUtcToLocalTime' function we defined earlier to
// convert 'utcDatetime' into its local time in Eastern Daylight Time (as
// described by 'edt2010'):
//..
//  bdet_Datetime localDatetime;
//  int status = convertUtcToLocalTime(&localDatetime, utcDatetime, edt2010);
//  if (0 != status) {
//      // The conversion failed so return an error code.
//
//      return 1;                                                     // RETURN
//  }
//..
// Finally, we verify that the result corresponds to the expected local time in
// New York, "Jul 20, 2010 7:00":
//..
//  assert(bdet_Datetime(2010, 7, 20, 7) == localDatetime);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAETZO_LOCALTIMEDESCRIPTOR
#include <baetzo_localtimedescriptor.h>
#endif

#ifndef INCLUDED_BAET_LOCALDATETIME
#include <baet_localdatetime.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
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

namespace BloombergLP {

class bslma_Allocator;

                        // ============================
                        // class baetzo_LocalTimePeriod
                        // ============================

class baetzo_LocalTimePeriod {
    // This complex-constrained (value-semantic) attribute class describes a
    // range of time in which a particular local time description (offset, DST
    // status, and a descriptive string) is in effect.  See the Attributes
    // section under @DESCRIPTION in the component-level documentation.  Note
    // that the class invariants are identically the constraints on the
    // attributes.
    //
    // This class:
    //: o supports a complete set of *value* *semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.

    // DATA
    baetzo_LocalTimeDescriptor d_descriptor;    // local time descriptor for
                                                // this period

    bdet_Datetime              d_utcStartTime;  // start of this period

    bdet_Datetime              d_utcEndTime;    // end of this period

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS2(baetzo_LocalTimePeriod,
                                  bslalg_TypeTraitBitwiseMoveable,
                                  bslalg_TypeTraitUsesBslmaAllocator);

    // CLASS METHODS
    static bool isValidUtcStartAndEndTime(const bdet_Datetime& utcStartTime,
                                          const bdet_Datetime& utcEndTime);
        // Return 'true' if the specified 'utcStartTime' and 'utcEndTime' have
        // the same value, or if 'utcStartTime' and 'utcEndTime' are comparable
        // (i.e., neither has the value of a default-constructed
        // 'bdet_DateTime' object) and 'utcStartTime < utcEndTime'; return
        // 'false' otherwise.

    // CREATORS
    explicit baetzo_LocalTimePeriod(bslma_Allocator *basicAllocator = 0);
        // Create a 'baetzo_LocalTimePeriod' object having the (default)
        // attribute values:
        //: o 'descriptor()   == baetzo_LocalTimeDescriptor()'
        //: o 'utcStartTime() == bdet_Datetime()'
        //: o 'utcEndTime()   == bdet_Datetime()'
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    baetzo_LocalTimePeriod(
                        const baetzo_LocalTimeDescriptor&  descriptor,
                        const bdet_Datetime&               utcStartTime,
                        const bdet_Datetime&               utcEndTime,
                        bslma_Allocator                   *basicAllocator = 0);
        // Create a 'baetzo_LocalTimePeriod' object having the specified
        // 'descriptor', 'utcStartTime', and 'utcEndTime' attribute values.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The behavior is undefined unless
        // 'utcStartTime == utcEndTime', or if 'utcStartTime' and 'utcEndTime'
        // are comparable (i.e., neither equals the a default constructed
        // 'bdet_DateTime' object) unless 'utcStartTime < utcEndTime'.  (See
        // the 'isValidUtcStartAndEndTime' method.)

    baetzo_LocalTimePeriod(const baetzo_LocalTimePeriod&  original,
                           bslma_Allocator               *basicAllocator = 0);
        // Create a 'baetzo_LocalTimePeriod' object with the same value as the
        // specified 'original' object.  Optionally specify a 'basicAllocator'
        // used to supply memory.  If 'basicAllocator' is 0, the currently
        // installed default allocator is used.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~baetzo_LocalTimePeriod();
        // Destroy this object.
#endif

    // MANIPULATORS
    baetzo_LocalTimePeriod& operator=(const baetzo_LocalTimePeriod& rhs);
        // Assign to this object the value of the specified 'rhs' object, and
        // return a reference providing modifiable access to this object.

    void setDescriptor(const baetzo_LocalTimeDescriptor& value);
        // Set the 'descriptor' attribute to the specified 'value'.

    void setUtcStartAndEndTime(const bdet_Datetime& utcStartTime,
                               const bdet_Datetime& utcEndTime);
        // Set the 'utcStartTime' and 'utcEndTime' attributes of this object to
        // the specified 'utcStartTime' and 'utcEndTime', respectively.  The
        // behavior is undefined unless 'utcStartTime == utcEndTime', or if
        // 'utcStartTime' and 'utcEndTime' are comparable (i.e., neither equals
        // a default constructed 'bdet_DateTime' object) unless
        // 'utcStartTime < utcEndTime'.  (See the 'isValidUtcStartAndEndTime'
        // method.)

    void swap(baetzo_LocalTimePeriod& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.  The behavior is undefined unless this
        // object was created with the same allocator as 'other'.

    // ACCESSORS
    const baetzo_LocalTimeDescriptor& descriptor() const;
        // Return a reference providing non-modifiable access to the
        // 'descriptor' attribute of this object.

    const bdet_Datetime& utcStartTime() const;
        // Return a reference providing non-modifiable access to the
        // 'utcStartTime' attribute of this object.

    const bdet_Datetime& utcEndTime() const;
        // Return a reference providing non-modifiable access to the
        // 'utcEndTime' attribute of this object.

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
bool operator==(const baetzo_LocalTimePeriod& lhs,
                const baetzo_LocalTimePeriod& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'baetzo_LocalTimePeriod' objects have
    // the same value if each of their corresponding 'descriptor',
    // 'utcStartTime', and 'utcEndTime' attributes have the same value.

bool operator!=(const baetzo_LocalTimePeriod& lhs,
                const baetzo_LocalTimePeriod& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baetzo_LocalTimePeriod' objects
    // do not have the same value if the corresponding values of their
    // 'descriptor', 'utcStartTime', or 'utcEndTime' attributes are not the
    // same.

std::ostream& operator<<(std::ostream&                 stream,
                         const baetzo_LocalTimePeriod& localTimePeriod);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)'.

// FREE FUNCTIONS
void swap(baetzo_LocalTimePeriod& a, baetzo_LocalTimePeriod& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.  The
    // behavior is undefined unless the two objects were created with the same
    // allocator.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // ----------------------------
                        // class baetzo_LocalTimePeriod
                        // ----------------------------

// CLASS METHODS
inline
bool baetzo_LocalTimePeriod::isValidUtcStartAndEndTime(
                                             const bdet_Datetime& utcStartTime,
                                             const bdet_Datetime& utcEndTime)
{
    bdet_Datetime defaultObj;
    return utcStartTime == utcEndTime
        || (utcStartTime != defaultObj && utcEndTime != defaultObj
            && utcStartTime < utcEndTime);
}

// CREATORS
inline
baetzo_LocalTimePeriod::baetzo_LocalTimePeriod(bslma_Allocator *basicAllocator)
: d_descriptor(basicAllocator)
, d_utcStartTime()
, d_utcEndTime()
{
}

inline
baetzo_LocalTimePeriod::baetzo_LocalTimePeriod(
                             const baetzo_LocalTimeDescriptor&  descriptor,
                             const bdet_Datetime&               utcStartTime,
                             const bdet_Datetime&               utcEndTime,
                             bslma_Allocator                   *basicAllocator)
: d_descriptor(descriptor, basicAllocator)
, d_utcStartTime(utcStartTime)
, d_utcEndTime(utcEndTime)
{
    BSLS_ASSERT_SAFE(isValidUtcStartAndEndTime(d_utcStartTime, d_utcEndTime));
}

inline
baetzo_LocalTimePeriod::baetzo_LocalTimePeriod(
                                 const baetzo_LocalTimePeriod&  original,
                                 bslma_Allocator               *basicAllocator)
: d_descriptor(original.d_descriptor, basicAllocator)
, d_utcStartTime(original.d_utcStartTime)
, d_utcEndTime(original.d_utcEndTime)
{
    BSLS_ASSERT_SAFE(isValidUtcStartAndEndTime(d_utcStartTime, d_utcEndTime));
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
inline
baetzo_LocalTimePeriod::~baetzo_LocalTimePeriod()
{
    BSLS_ASSERT_SAFE(isValidUtcStartAndEndTime(d_utcStartTime, d_utcEndTime));
}
#endif

// MANIPULATORS
inline
baetzo_LocalTimePeriod& baetzo_LocalTimePeriod::operator=(
                                             const baetzo_LocalTimePeriod& rhs)
{
    d_descriptor   = rhs.d_descriptor;    // must be first
    d_utcStartTime = rhs.d_utcStartTime;
    d_utcEndTime   = rhs.d_utcEndTime;
    return *this;
}

inline
void baetzo_LocalTimePeriod::setUtcStartAndEndTime(
                                             const bdet_Datetime& utcStartTime,
                                             const bdet_Datetime& utcEndTime)
{
    BSLS_ASSERT_SAFE(isValidUtcStartAndEndTime(utcStartTime, utcEndTime));

    d_utcStartTime = utcStartTime;
    d_utcEndTime   = utcEndTime;
}

inline
void baetzo_LocalTimePeriod::setDescriptor(
                                       const baetzo_LocalTimeDescriptor& value)
{
    d_descriptor = value;
}

inline
void baetzo_LocalTimePeriod::swap(baetzo_LocalTimePeriod& other)
{
    // 'swap' is undefined for objects with non-equal allocators.

    BSLS_ASSERT_SAFE(d_descriptor.description().get_allocator() ==
                             other.d_descriptor.description().get_allocator());

    using bsl::swap;
    swap(d_descriptor,   other.d_descriptor);
    swap(d_utcStartTime, other.d_utcStartTime);
    swap(d_utcEndTime,   other.d_utcEndTime);
}

// ACCESSORS
inline
const baetzo_LocalTimeDescriptor& baetzo_LocalTimePeriod::descriptor() const
{
    return d_descriptor;
}

inline
const bdet_Datetime& baetzo_LocalTimePeriod::utcStartTime() const
{
    return d_utcStartTime;
}

inline
const bdet_Datetime& baetzo_LocalTimePeriod::utcEndTime() const
{
    return d_utcEndTime;
}

// FREE OPERATORS
inline
bool operator==(const baetzo_LocalTimePeriod& lhs,
                const baetzo_LocalTimePeriod& rhs)
{
    return lhs.descriptor()   == rhs.descriptor()
        && lhs.utcStartTime() == rhs.utcStartTime()
        && lhs.utcEndTime()   == rhs.utcEndTime();
}

inline
bool operator!=(const baetzo_LocalTimePeriod& lhs,
                const baetzo_LocalTimePeriod& rhs)
{
    return lhs.descriptor()   != rhs.descriptor()
        || lhs.utcStartTime() != rhs.utcStartTime()
        || lhs.utcEndTime()   != rhs.utcEndTime();
}

inline
std::ostream& operator<<(std::ostream&                 stream,
                         const baetzo_LocalTimePeriod& localTimePeriod)
{
    return localTimePeriod.print(stream, 0, -1);
}

// FREE FUNCTIONS
inline
void swap(baetzo_LocalTimePeriod& a, baetzo_LocalTimePeriod& b)
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
