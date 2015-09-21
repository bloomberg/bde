// bdlt_datetimeinterval.h                                            -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIMEINTERVAL
#define INCLUDED_BDLT_DATETIMEINTERVAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a representation of an interval of time.
//
//@CLASSES:
//  bdlt::DatetimeInterval: time interval with millisecond resolution
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component implements a time interval class,
// 'bdlt::DatetimeInterval', capable of representing the (signed) difference
// between two arbitrary points in time.  The time interval represented by a
// 'bdlt::DatetimeInterval' object has millisecond resolution.
//
///The Representation of a Time Interval
///-------------------------------------
// A time interval has a value that is independent of its representation.
// Conceptually, the interval between two points in time could be described
// using a (signed) real number of seconds (or minutes, or hours, etc.).  A
// 'bdlt::DatetimeInterval' represents this value as five fields: days, hours,
// minutes, seconds, and milliseconds.  In the "canonical representation" of a
// time interval, the days field may have any 32-bit integer value, with the
// hours, minutes, seconds, and milliseconds fields limited to the respective
// ranges '[-23 .. 23]', '[-59 .. 59]', '[-59 .. 59]', and '[-999 .. 999]',
// with the additional constraint that the five fields are either all
// non-negative or all non-positive.  When setting the value of a time interval
// via its five-field representation, any integer value may be used in any
// field, with the constraint that the resulting number of days be
// representable as a 32-bit integer and the slightly-less-convenient condition
// that no one field, or intermediate sum of fields, can overflow a 64-bit
// integer of *milliseconds*.  Similarly, the field values may be accessed in
// the canonical representation using the 'days', 'hours', 'minutes',
// 'seconds', and 'milliseconds' methods.  The total value of the time interval
// may be accessed in the respective field units via the 'totalDays',
// 'totalHours', 'totalMinutes', 'totalSeconds', and 'totalMilliseconds'
// methods.  Note that, with the exception of 'totalMilliseconds' (which
// returns an exact result), the other "total" accessors round toward 0.
//
// The following summarizes the canonical representation of the value of a
// 'bdlt::DatetimeInterval':
//..
//  Field Name     Max. Valid Range     Auxiliary Conditions Limiting Validity
//  ----------     ------------------   --------------------------------------
//  days           any 32-bit integer    all fields non-pos. or all non-neg.
//  hours          [ -23 ..  23]         all fields non-pos. or all non-neg.
//  minutes        [ -59 ..  59]         all fields non-pos. or all non-neg.
//  seconds        [ -59 ..  59]         all fields non-pos. or all non-neg.
//  milliseconds   [-999 .. 999]         all fields non-pos. or all non-neg.
//..
///Extremal Time Intervals
///-----------------------
// The 'bdlt::DatetimeInterval' class provides two integral constants,
// 'k_MILLISECONDS_MAX' and 'k_MILLISECONDS_MIN', that define (respectively)
// the maximum and minimum intervals representable by 'DatetimeInterval'
// objects.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic 'bdlt::DatetimeInterval' Usage
///- - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create and use a 'bdlt::DatetimeInterval'
// object.
//
// First, create an object 'i1' having the default value:
//..
//  bdlt::DatetimeInterval i1;         assert(  0 == i1.days());
//                                     assert(  0 == i1.hours());
//                                     assert(  0 == i1.minutes());
//                                     assert(  0 == i1.seconds());
//                                     assert(  0 == i1.milliseconds());
//..
// Then, set the value of 'i1' to -5 days, and then add 16 hours to that value:
//..
//  i1.setTotalDays(-5);
//  i1.addHours(16);                   assert( -4 == i1.days());
//                                     assert( -8 == i1.hours());
//                                     assert(  0 == i1.minutes());
//                                     assert(  0 == i1.seconds());
//                                     assert(  0 == i1.milliseconds());
//..
// Next, create 'i2' as a copy of 'i1':
//..
//  bdlt::DatetimeInterval i2(i1);     assert( -4 == i2.days());
//                                     assert( -8 == i2.hours());
//                                     assert(  0 == i2.minutes());
//                                     assert(  0 == i2.seconds());
//                                     assert(  0 == i2.milliseconds());
//..
// Then, add 2 days and 4 seconds to the value of 'i2' (in two steps), and
// confirm that 'i2' has a value that is greater than that of 'i1':
//..
//  i2.addDays(2);
//  i2.addSeconds(4);                  assert( -2 == i2.days());
//                                     assert( -7 == i2.hours());
//                                     assert(-59 == i2.minutes());
//                                     assert(-56 == i2.seconds());
//                                     assert(  0 == i2.milliseconds());
//                                     assert(i2 > i1);
//..
// Next, add 2 days and 4 seconds to the value of 'i1' in one step by using the
// 'addInterval' method, and confirm that 'i1' now has the same value as 'i2':
//..
//  i1.addInterval(2, 0, 0, 4);        assert(i2 == i1);
//..
// Finally, write the value of 'i2' to 'stdout':
//..
//  bsl::cout << i2 << bsl::endl;
//..
// The output operator produces the following format on 'stdout':
//..
//  -2_07:59:56.000
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_TIMEUNITRATIO
#include <bdlt_timeunitratio.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLMF_ISTRIVIALLYCOPYABLE
#include <bslmf_istriviallycopyable.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>       // 'INT_MIN'
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {
namespace bdlt {

                          // ======================
                          // class DatetimeInterval
                          // ======================

class DatetimeInterval {
    // Each object of this class represents a (signed) time interval with
    // millisecond resolution.  See {The Representation of a Time Interval} for
    // details.

    // DATA
    bsls::Types::Int64 d_milliseconds;  // interval in (signed) milliseconds

  public:
    // PUBLIC CLASS DATA
    static const bsls::Types::Int64 k_MILLISECONDS_MAX = 185542587187199999LL;
        // The maximum interval that is representable by a 'DatetimeInterval',
        // in milliseconds.

    static const bsls::Types::Int64 k_MILLISECONDS_MIN =
                   -k_MILLISECONDS_MAX - TimeUnitRatio::k_MILLISECONDS_PER_DAY;
        // The minimum interval that is representable by a 'DatetimeInterval,
        // in milliseconds'.


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
    DatetimeInterval();
        // Create a time interval object having the value 0.

    explicit
    DatetimeInterval(int                days,
                     bsls::Types::Int64 hours        = 0,
                     bsls::Types::Int64 minutes      = 0,
                     bsls::Types::Int64 seconds      = 0,
                     bsls::Types::Int64 milliseconds = 0);
        // Create a time interval object having the value given by the
        // specified 'days', and the optionally specified 'hours', 'minutes',
        // 'seconds', and 'milliseconds'.  Unspecified arguments default to 0.
        // The behavior is undefined unless: (1) the resulting time interval
        // value is valid (i.e., the days field must not overflow a 32-bit
        // integer), and (2) no intermediate sum of the arguments (converted to
        // milliseconds) overflows a 64-bit integer, regardless of the order of
        // summation.  Note that the arguments may be supplied using a mixture
        // of positive, negative, and 0 values.

    DatetimeInterval(const DatetimeInterval& original);
        // Create a time interval object having the value of the specified
        // 'original' time interval.

    //! ~DatetimeInterval() = default;
        // Destroy this time interval object.  Note that this method's
        // definition is generated by the compiler.

    // MANIPULATORS
    DatetimeInterval& operator=(const DatetimeInterval& rhs);
        // Assign to this object the value of the specified 'rhs' time
        // interval, and return a reference providing modifiable access to this
        // object.

    DatetimeInterval& operator+=(const DatetimeInterval& rhs);
        // Add to this time interval the value of the specified 'rhs' time
        // interval, and return a reference providing modifiable access to this
        // object.  The behavior is undefined unless the resulting time
        // interval value is valid (i.e., the days field must not overflow a
        // 32-bit integer).

    DatetimeInterval& operator-=(const DatetimeInterval& rhs);
        // Subtract from this time interval the value of the specified 'rhs'
        // time interval, and return a reference providing modifiable access to
        // this object.  The behavior is undefined unless the resulting time
        // interval value is valid (i.e., the days field must not overflow a
        // 32-bit integer).

    void setInterval(int                days,
                     bsls::Types::Int64 hours        = 0,
                     bsls::Types::Int64 minutes      = 0,
                     bsls::Types::Int64 seconds      = 0,
                     bsls::Types::Int64 milliseconds = 0);
        // Set the time interval represented by this object to the value given
        // by the specified 'days', and the optionally specified 'hours',
        // 'minutes', 'seconds', and 'milliseconds'.  Unspecified arguments
        // default to 0.  The behavior is undefined unless: (1) the resulting
        // time interval value is valid (i.e., the days field must not overflow
        // a 32-bit integer), and (2) no intermediate sum of the arguments
        // (converted to milliseconds) overflows a 64-bit integer, regardless
        // of the order of summation.  Note that the arguments may be supplied
        // using a mixture of positive, negative, and 0 values.

    void setTotalDays(int days);
        // Set the overall value of this object to indicate the specified
        // number of 'days'.

    void setTotalHours(bsls::Types::Int64 hours);
        // Set the overall value of this object to indicate the specified
        // number of 'hours'.  The behavior is undefined unless the resulting
        // time interval value is valid (i.e., the days field must not overflow
        // a 32-bit integer).

    void setTotalMinutes(bsls::Types::Int64 minutes);
        // Set the overall value of this object to indicate the specified
        // number of 'minutes'.  The behavior is undefined unless the resulting
        // time interval value is valid (i.e., the days field must not overflow
        // a 32-bit integer).

    void setTotalSeconds(bsls::Types::Int64 seconds);
        // Set the overall value of this object to indicate the specified
        // number of 'seconds'.  The behavior is undefined unless the resulting
        // time interval value is valid (i.e., the days field must not overflow
        // a 32-bit integer).

    void setTotalMilliseconds(bsls::Types::Int64 milliseconds);
        // Set the overall value of this object to indicate the specified
        // number of 'milliseconds'.  The behavior is undefined unless the
        // resulting time interval value is valid (i.e., the days field must
        // not overflow a 32-bit integer).

    void addInterval(int                days,
                     bsls::Types::Int64 hours        = 0,
                     bsls::Types::Int64 minutes      = 0,
                     bsls::Types::Int64 seconds      = 0,
                     bsls::Types::Int64 milliseconds = 0);
        // Add to this time interval the specified number of 'days', and the
        // optionally specified number of 'hours', 'minutes', 'seconds', and
        // 'milliseconds'.  Unspecified arguments default to 0.  The behavior
        // is undefined unless: (1) the resulting time interval value is valid
        // (i.e., the days field must not overflow a 32-bit integer), and (2)
        // no intermediate sum of this time interval and the arguments
        // (converted to milliseconds) overflows a 64-bit integer, regardless
        // of the order of summation.  Note that the arguments may be supplied
        // using a mixture of positive, negative, and 0 values.

    void addDays(int days);
        // Add to this time interval the specified number of 'days'.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the days field must not overflow a 32-bit integer).

    void addHours(bsls::Types::Int64 hours);
        // Add to this time interval the specified number of 'hours'.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the days field must not overflow a 32-bit integer).

    void addMinutes(bsls::Types::Int64 minutes);
        // Add to this time interval the specified number of 'minutes'.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the days field must not overflow a 32-bit integer).

    void addSeconds(bsls::Types::Int64 seconds);
        // Add to this time interval the specified number of 'seconds'.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the days field must not overflow a 32-bit integer).

    void addMilliseconds(bsls::Types::Int64 milliseconds);
        // Add to this time interval the specified number of 'milliseconds'.
        // The behavior is undefined unless the resulting time interval value
        // is valid (i.e., the days field must not overflow a 32-bit integer).

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
    int days() const;
        // Return the days field in the canonical representation of the value
        // of this time interval.  Note that the return value may be negative.
        // Also note that the return value is the same as that returned by
        // 'totalDays'.

    int hours() const;
        // Return the hours field in the canonical representation of the value
        // of this time interval.  Note that the return value may be negative.

    int minutes() const;
        // Return the minutes field in the canonical representation of the
        // value of this time interval.  Note that the return value may be
        // negative.

    int seconds() const;
        // Return the seconds field in the canonical representation of the
        // value of this time interval.  Note that the return value may be
        // negative.

    int milliseconds() const;
        // Return the milliseconds field in the canonical representation of the
        // value of this time interval.  Note that the return value may be
        // negative.

    int totalDays() const;
        // Return the value of this time interval in integral days, rounded
        // toward 0.  Note that the return value may be negative.  Also note
        // that the return value is the same as that returned by 'days'.

    bsls::Types::Int64 totalHours() const;
        // Return the value of this time interval in integral hours, rounded
        // toward 0.  Note that the return value may be negative.

    bsls::Types::Int64 totalMinutes() const;
        // Return the value of this time interval in integral minutes, rounded
        // toward 0.  Note that the return value may be negative.

    bsls::Types::Int64 totalSeconds() const;
        // Return the value of this time interval in integral seconds, rounded
        // toward 0.  Note that the return value may be negative.

    double totalSecondsAsDouble() const;
        // Return the value of this time interval in seconds as a 'double',
        // potentially with a fractional part.  Note that the return value may
        // be negative.  Also note that the conversion from the internal
        // representation to 'double' may *lose* precision.

    bsls::Types::Int64 totalMilliseconds() const;
        // Return the value of this time interval in integral milliseconds.
        // Note that the return value may be negative.

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
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in
        // a human-readable format, and return a reference to 'stream'.
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
DatetimeInterval operator+(const DatetimeInterval& lhs,
                           const DatetimeInterval& rhs);
    // Return a 'DatetimeInterval' object whose value is the sum of the
    // specified 'lhs' and 'rhs' time intervals.  The behavior is undefined
    // unless the resulting time interval value is valid (i.e., the days field
    // must not overflow a 32-bit integer).

DatetimeInterval operator-(const DatetimeInterval& lhs,
                           const DatetimeInterval& rhs);
    // Return a 'DatetimeInterval' object whose value is the difference between
    // the specified 'lhs' and 'rhs' time intervals.  The behavior is undefined
    // unless the resulting time interval value is valid (i.e., the days field
    // must not overflow a 32-bit integer).

DatetimeInterval operator-(const DatetimeInterval& value);
    // Return a 'DatetimeInterval' object whose value is the negative of the
    // specified time interval 'value'.  The behavior is undefined unless
    // 'INT_MIN < value.days()'.

bool operator==(const DatetimeInterval& lhs, const DatetimeInterval& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time intervals have the
    // same value, and 'false' otherwise.  Two time intervals have the same
    // value if all of the corresponding values of their days, hours, minutes,
    // seconds, and milliseconds fields are the same.

bool operator!=(const DatetimeInterval& lhs, const DatetimeInterval& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time intervals do not
    // have the same value, and 'false' otherwise.  Two time intervals do not
    // have the same value if any of the corresponding values of their days,
    // hours, minutes, seconds, or milliseconds fields is not the same.

bool operator< (const DatetimeInterval& lhs, const DatetimeInterval& rhs);
bool operator<=(const DatetimeInterval& lhs, const DatetimeInterval& rhs);
bool operator> (const DatetimeInterval& lhs, const DatetimeInterval& rhs);
bool operator>=(const DatetimeInterval& lhs, const DatetimeInterval& rhs);
    // Return 'true' if the nominal relation between the specified 'lhs' and
    // 'rhs' time interval values holds, and 'false' otherwise.  'lhs' is less
    // than 'rhs' if the following expression evaluates to 'true':
    //..
    //  lhs.totalMilliseconds() < rhs.totalMilliseconds()
    //..
    // The other relationships are defined similarly.

bsl::ostream& operator<<(bsl::ostream& stream, const DatetimeInterval& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.  Also note that this
    // method has the same behavior as 'object.print(stream, 0, -1)'.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                          // ----------------------
                          // class DatetimeInterval
                          // ----------------------

// CLASS METHODS

                                  // Aspects

inline
int DatetimeInterval::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

// CREATORS
inline
DatetimeInterval::DatetimeInterval()
: d_milliseconds(0)
{
}

inline
DatetimeInterval::DatetimeInterval(int                days,
                                   bsls::Types::Int64 hours,
                                   bsls::Types::Int64 minutes,
                                   bsls::Types::Int64 seconds,
                                   bsls::Types::Int64 milliseconds)
: d_milliseconds(milliseconds
           + seconds * TimeUnitRatio::k_MS_PER_S
           + minutes * TimeUnitRatio::k_MS_PER_M
           + hours   * TimeUnitRatio::k_MS_PER_H
           + static_cast<bsls::Types::Int64>(days) * TimeUnitRatio::k_MS_PER_D)
{
    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);
}

inline
DatetimeInterval::DatetimeInterval(const DatetimeInterval& original)
: d_milliseconds(original.d_milliseconds)
{
}

// MANIPULATORS
inline
DatetimeInterval& DatetimeInterval::operator=(const DatetimeInterval& rhs)
{
    d_milliseconds = rhs.d_milliseconds;
    return *this;
}

inline
DatetimeInterval& DatetimeInterval::operator+=(const DatetimeInterval& rhs)
{
    d_milliseconds += rhs.d_milliseconds;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);

    return *this;
}

inline
DatetimeInterval& DatetimeInterval::operator-=(const DatetimeInterval& rhs)
{
    d_milliseconds -= rhs.d_milliseconds;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);

    return *this;
}

inline
void DatetimeInterval::setInterval(int                days,
                                   bsls::Types::Int64 hours,
                                   bsls::Types::Int64 minutes,
                                   bsls::Types::Int64 seconds,
                                   bsls::Types::Int64 milliseconds)
{
    d_milliseconds = milliseconds
           + seconds * TimeUnitRatio::k_MS_PER_S
           + minutes * TimeUnitRatio::k_MS_PER_M
           + hours   * TimeUnitRatio::k_MS_PER_H
           + static_cast<bsls::Types::Int64>(days) * TimeUnitRatio::k_MS_PER_D;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);
}

inline
void DatetimeInterval::setTotalDays(int days)
{
    d_milliseconds =
             static_cast<bsls::Types::Int64>(days) * TimeUnitRatio::k_MS_PER_D;
}

inline
void DatetimeInterval::setTotalHours(bsls::Types::Int64 hours)
{
    d_milliseconds = hours * TimeUnitRatio::k_MS_PER_H;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);
}

inline
void DatetimeInterval::setTotalMinutes(bsls::Types::Int64 minutes)
{
    d_milliseconds = minutes * TimeUnitRatio::k_MS_PER_M;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);
}

inline
void DatetimeInterval::setTotalSeconds(bsls::Types::Int64 seconds)
{
    d_milliseconds = seconds * TimeUnitRatio::k_MS_PER_S;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);
}

inline
void DatetimeInterval::setTotalMilliseconds(bsls::Types::Int64 milliseconds)
{
    d_milliseconds = milliseconds;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);
}

inline
void DatetimeInterval::addInterval(int                days,
                                   bsls::Types::Int64 hours,
                                   bsls::Types::Int64 minutes,
                                   bsls::Types::Int64 seconds,
                                   bsls::Types::Int64 milliseconds)
{
    d_milliseconds += milliseconds
           + seconds * TimeUnitRatio::k_MS_PER_S
           + minutes * TimeUnitRatio::k_MS_PER_M
           + hours   * TimeUnitRatio::k_MS_PER_H
           + static_cast<bsls::Types::Int64>(days) * TimeUnitRatio::k_MS_PER_D;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);
}

inline
void DatetimeInterval::addDays(int days)
{
    d_milliseconds +=
             static_cast<bsls::Types::Int64>(days) * TimeUnitRatio::k_MS_PER_D;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);
}

inline
void DatetimeInterval::addHours(bsls::Types::Int64 hours)
{
    d_milliseconds += hours * TimeUnitRatio::k_MS_PER_H;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);
}

inline
void DatetimeInterval::addMinutes(bsls::Types::Int64 minutes)
{
    d_milliseconds += minutes * TimeUnitRatio::k_MS_PER_M;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);
}

inline
void DatetimeInterval::addSeconds(bsls::Types::Int64 seconds)
{
    d_milliseconds += seconds * TimeUnitRatio::k_MS_PER_S;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);
}

inline
void
DatetimeInterval::addMilliseconds(bsls::Types::Int64 milliseconds)
{
    d_milliseconds += milliseconds;

    BSLS_ASSERT_SAFE(k_MILLISECONDS_MIN <= d_milliseconds);
    BSLS_ASSERT_SAFE(    d_milliseconds <= k_MILLISECONDS_MAX);
}

                                  // Aspects

template <class STREAM>
STREAM& DatetimeInterval::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            bsls::Types::Int64 tmp;
            stream.getInt64(tmp);

            if (stream
             && k_MILLISECONDS_MIN <= tmp && k_MILLISECONDS_MAX >= tmp) {
                d_milliseconds = tmp;
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
int DatetimeInterval::days() const
{
    return static_cast<int>(d_milliseconds / TimeUnitRatio::k_MS_PER_D);
}

inline
int DatetimeInterval::hours() const
{
    return static_cast<int>(
       d_milliseconds % TimeUnitRatio::k_MS_PER_D / TimeUnitRatio::k_MS_PER_H);
}

inline
int DatetimeInterval::minutes() const
{
    return static_cast<int>(
       d_milliseconds % TimeUnitRatio::k_MS_PER_H / TimeUnitRatio::k_MS_PER_M);
}

inline
int DatetimeInterval::seconds() const
{
    return static_cast<int>(
       d_milliseconds % TimeUnitRatio::k_MS_PER_M / TimeUnitRatio::k_MS_PER_S);
}

inline
int DatetimeInterval::milliseconds() const
{
    return static_cast<int>(d_milliseconds % TimeUnitRatio::k_MS_PER_S);
}

inline
int DatetimeInterval::totalDays() const
{
    return static_cast<int>(d_milliseconds / TimeUnitRatio::k_MS_PER_D);
}

inline
bsls::Types::Int64 DatetimeInterval::totalHours() const
{
    return d_milliseconds / TimeUnitRatio::k_MS_PER_H;
}

inline
bsls::Types::Int64 DatetimeInterval::totalMinutes() const
{
    return d_milliseconds / TimeUnitRatio::k_MS_PER_M;
}

inline
bsls::Types::Int64 DatetimeInterval::totalSeconds() const
{
    return d_milliseconds / TimeUnitRatio::k_MS_PER_S;
}

inline
double DatetimeInterval::totalSecondsAsDouble() const
{
    return static_cast<double>(d_milliseconds) /
                                             (1.0 * TimeUnitRatio::k_MS_PER_S);
}

inline
bsls::Types::Int64 DatetimeInterval::totalMilliseconds() const
{
    return d_milliseconds;
}

                                  // Aspects

template <class STREAM>
STREAM& DatetimeInterval::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            stream.putInt64(d_milliseconds);
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
bdlt::DatetimeInterval bdlt::operator+(const DatetimeInterval& lhs,
                                       const DatetimeInterval& rhs)
{
    DatetimeInterval interval;

    interval.setTotalMilliseconds(
                            lhs.totalMilliseconds() + rhs.totalMilliseconds());

    return interval;
}

inline
bdlt::DatetimeInterval bdlt::operator-(const DatetimeInterval& lhs,
                                       const DatetimeInterval& rhs)
{
    DatetimeInterval interval;

    interval.setTotalMilliseconds(
                            lhs.totalMilliseconds() - rhs.totalMilliseconds());

    return interval;
}

inline
bdlt::DatetimeInterval bdlt::operator-(const DatetimeInterval& value)
{
    BSLS_ASSERT_SAFE(INT_MIN < value.days());

    DatetimeInterval interval;

    interval.setTotalMilliseconds(-value.totalMilliseconds());

    return interval;
}

inline
bool bdlt::operator==(const DatetimeInterval& lhs,
                      const DatetimeInterval& rhs)
{
    return lhs.totalMilliseconds() == rhs.totalMilliseconds();
}

inline
bool bdlt::operator!=(const DatetimeInterval& lhs,
                      const DatetimeInterval& rhs)
{
    return lhs.totalMilliseconds() != rhs.totalMilliseconds();
}

inline
bool bdlt::operator< (const DatetimeInterval& lhs,
                      const DatetimeInterval& rhs)
{
    return lhs.totalMilliseconds() < rhs.totalMilliseconds();
}

inline
bool bdlt::operator<=(const DatetimeInterval& lhs,
                      const DatetimeInterval& rhs)
{
    return lhs.totalMilliseconds() <= rhs.totalMilliseconds();
}

inline
bool bdlt::operator> (const DatetimeInterval& lhs,
                      const DatetimeInterval& rhs)
{
    return lhs.totalMilliseconds() > rhs.totalMilliseconds();
}

inline
bool bdlt::operator>=(const DatetimeInterval& lhs,
                      const DatetimeInterval& rhs)
{
    return lhs.totalMilliseconds() >= rhs.totalMilliseconds();
}

inline
bsl::ostream& bdlt::operator<<(bsl::ostream&           stream,
                               const DatetimeInterval& object)
{
    return object.print(stream, 0, -1);
}

}  // close enterprise namespace

namespace bsl {

// TRAITS
template <>
struct is_trivially_copyable<BloombergLP::bdlt::DatetimeInterval> :
                                                               bsl::true_type {
    // This template specialization for 'is_trivially_copyable' indicates that
    // 'DatetimeInterval' is a trivially copyable type.
};

}  // close namespace bsl

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
