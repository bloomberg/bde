// bsls_timeinterval.h                                                -*-C++-*-
#ifndef INCLUDED_BSLS_TIMEINTERVAL
#define INCLUDED_BSLS_TIMEINTERVAL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a representation of a time interval.
//
//@CLASSES:
//   bsls::TimeInterval: time interval with nanosecond resolution
//
//@SEE_ALSO: bdlt_time, bdlt_datetimeinterval
//
//@DESCRIPTION: This component provides a value-semantic type,
// 'bsls::TimeInterval', that is capable of representing a signed time
// interval with nanosecond resolution.
//
///Representation
///--------------
// A time interval has a value that is independent of its representation.
// Conceptually, a time interval may be thought of as a signed,
// arbitrary-precision floating-point number denominated in seconds (or in
// days, or in fortnights, if one prefers).  A 'bsls::TimeInterval' represents
// this value as two fields: seconds and nanoseconds.  In the "canonical
// representation" of a time interval, the 'seconds' field may have any 64-bit
// signed integer value, with the 'nanoseconds' field limited to the range
// '[ -999999999..999999999 ]', and with the additional constraint that the two
// fields are either both non-negative or both non-positive.  When setting the
// value of a time interval via its two-field representation, any integer value
// may be used in either field, with the constraint that the resulting number
// of seconds be representable as a 64-bit signed integer.  Similarly, the two
// field values may be accessed in the canonical representation using the
// 'seconds' and 'nanoseconds' methods.
//
// Binary arithmetic and relational operators taking two 'bsls::TimeInterval'
// objects, or a 'bsls::TimeInterval' object and a 'double', are provided.  A
// 'double' operand, representing a real number of seconds, is first converted
// to a 'bsls::TimeInterval' object before performing the operation.  Under
// such circumstances, the fractional part of the 'double', if any, is rounded
// to the nearest whole number of nanoseconds.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating and Modifying a 'bsls::TimeInterval'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example demonstrates how to create and manipulate a
// 'bsls::TimeInterval' object.
//
// First, we default construct a 'TimeInterval' object, 'interval':
//..
//  bsls::TimeInterval interval;
//
//  assert(0 == interval.seconds());
//  assert(0 == interval.nanoseconds());
//..
// Next, we set the value of 'interval' to 1 second and 10 nanoseconds (a time
// interval of 1000000010 nanoseconds):
//..
//  interval.setInterval(1, 10);
//
//  assert( 1 == interval.seconds());
//  assert(10 == interval.nanoseconds());
//..
// Then, we add 3 seconds to 'interval':
//..
//  interval.addInterval(3, 0);
//
//  assert( 4 == interval.seconds());
//  assert(10 == interval.nanoseconds());
//..
// Next, we create a copy of 'interval', 'intervalPrime':
//..
//  bsls::TimeInterval intervalPrime(interval);
//
//  assert(intervalPrime == interval);
//..
// Finally, we assign 3.14 seconds to 'intervalPrime', and then add 2.73
// seconds more:
//..
//  intervalPrime =  3.14;
//  intervalPrime += 2.73;
//
//  assert(        5 == intervalPrime.seconds());
//  assert(870000000 == intervalPrime.nanoseconds());
//..

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_NATIVESTD
#include <bsls_nativestd.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_IOSFWD
#include <iosfwd>
#define INCLUDED_IOSFWD
#endif

#ifndef INCLUDED_LIMITS_H
#include <limits.h>  // 'LLONG_MIN', 'LLONG_MAX'
#define INCLUDED_LIMITS_H
#endif

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01  // 'add*' operations are ordered by time unit

namespace BloombergLP {
namespace bsls {

                        // ==================
                        // class TimeInterval
                        // ==================

class TimeInterval {
    // Each instance of this value-semantic type represents a time interval
    // with nanosecond resolution.  In the "canonical representation" of a time
    // interval, the 'seconds' field may have any 64-bit signed integer value,
    // with the 'nanoseconds' field limited to the range
    // '[ -999999999..999999999 ]', and with the additional constraint that the
    // two fields are either both non-negative or both non-positive.

    // PRIVATE TYPES
    enum {
        k_MILLISECS_PER_SEC     = 1000,        // one thousand

        k_MICROSECS_PER_SEC     = 1000000,     // one million

        k_NANOSECS_PER_MICROSEC = 1000,        // one thousand

        k_NANOSECS_PER_MILLISEC = 1000000,     // one million

        k_NANOSECS_PER_SEC      = 1000000000,  // one billion

        k_SECONDS_PER_MINUTE    = 60,

        k_SECONDS_PER_HOUR      = 60 * k_SECONDS_PER_MINUTE,

        k_SECONDS_PER_DAY       = 24 * k_SECONDS_PER_HOUR
    };

    // DATA
    bsls::Types::Int64 d_seconds;      // field for seconds
    int                d_nanoseconds;  // field for nanoseconds

    // PRIVATE CLASS METHODS
    static bool isSumValidInt64(bsls::Types::Int64 lhs,
                                bsls::Types::Int64 rhs);
        // Return 'true' if the sum of the specified 'lhs' and 'rhs' can be
        // represented using a 64-bit signed integer, and 'false' otherwise.

  public:
    // CLASS METHODS
    static bool isValid(bsls::Types::Int64 seconds,
                        int                nanoseconds);
        // Return 'true' if a valid 'TimeInterval' can be constructed from the
        // specified 'seconds' and 'nanoseconds', and 'false' otherwise.  A
        // valid time interval can be constructed from 'seconds' and
        // 'nanoseconds' if their sum results in a time interval whose total
        // number of seconds can be represented with a 64-bit signed integer.

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
    TimeInterval();
        // Create a time interval having the value of 0 seconds and 0
        // nanoseconds.

    TimeInterval(bsls::Types::Int64 seconds, int nanoseconds);
        // Create a time interval having the value given by the sum of the
        // specified integral number of 'seconds' and 'nanoseconds'.  The
        // behavior is undefined unless the total number of seconds in the
        // resulting time interval can be represented with a 64-bit signed
        // integer (see 'isValid').  Note that there is no restriction on the
        // sign or magnitude of either argument except that they must not
        // violate the method's preconditions.

    explicit TimeInterval(double seconds);
        // Create a time interval having the value represented by the specified
        // real number of 'seconds'.  The fractional part of 'seconds', if any,
        // is rounded to the nearest whole number of nanoseconds.  The
        // behavior is undefined unless the total number of seconds in the
        // resulting time interval can be represented with a 64-bit signed
        // integer.

    // TimeInterval(const TimeInterval& original) = default;
        // Create a time interval having the value of the specified 'original'
        // time interval.  Note that this trivial copy constructor is
        // generated by the compiler.

    // ~TimeInterval() = default;
        // Destroy this time interval object.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS

                                  // Operator Overloads

    // TimeInterval& operator=(const TimeInterval& rhs) = default;
        // Assign to this time interval the value of the specified 'rhs' time
        // interval, and return a reference providing modifiable access to this
        // object.  Note that this trivial assignment operation is generated by
        // the compiler.


    TimeInterval& operator=(double rhs);
        // Assign to this time interval the value of the specified 'rhs' real
        // number of seconds, and return a reference providing modifiable
        // access to this object.  The fractional part of 'rhs', if any, is
        // rounded to the nearest whole number of nanoseconds.  The behavior
        // is undefined unless 'rhs' can be converted to a valid
        // 'TimeInterval' object.

    TimeInterval& operator+=(const TimeInterval& rhs);
        // Add to this time interval the value of the specified 'rhs' time
        // interval, and return a reference providing modifiable access to
        // this object.  The behavior is undefined unless the total number of
        // seconds in the resulting time interval can be represented with a
        // 64-bit signed integer.

    TimeInterval& operator+=(double rhs);
        // Add to this time interval the value of the specified 'rhs' real
        // number of seconds, and return a reference providing modifiable
        // access to this object.  The fractional part of 'rhs', if any, is
        // rounded to the nearest whole number of nanoseconds before being
        // added to this object.  The behavior is undefined unless 'rhs' can
        // be converted to a valid 'TimeInterval' object, and the total number
        // of seconds in the resulting time interval can be represented with a
        // 64-bit signed integer.

    TimeInterval& operator-=(const TimeInterval& rhs);
        // Subtract from this time interval the value of the specified 'rhs'
        // time interval, and return a reference providing modifiable access to
        // this object.  The behavior is undefined unless
        // 'LLONG_MIN != rhs.seconds()', and the total number of seconds in the
        // resulting time interval can be represented with a 64-bit signed
        // integer.

    TimeInterval& operator-=(double rhs);
        // Subtract from this time interval the value of the specified 'rhs'
        // real number of seconds, and return a reference providing modifiable
        // access to this object.  The fractional part of 'rhs', if any, is
        // rounded to the nearest whole number of nanoseconds before being
        // subtracted from this object.  The behavior is undefined unless
        // 'rhs' can be converted to a valid 'TimeInterval' object whose
        // 'seconds' field is greater than 'LLONG_MIN', and the total number of
        // seconds in the resulting time interval can be represented with a
        // 64-bit signed integer.

                                  // Add Operations

    TimeInterval& addDays(bsls::Types::Int64 days);
        // Add to this time interval the number of seconds represented by the
        // specified integral number of 'days', and return a reference
        // providing modifiable access to this object.  The behavior is
        // undefined unless the number of seconds in 'days', and the total
        // number of seconds in the resulting time interval, can both be
        // represented with 64-bit signed integers.  Note that 'days' may be
        // negative.

    TimeInterval& addHours(bsls::Types::Int64 hours);
        // Add to this time interval the number of seconds represented by the
        // specified integral number of 'hours', and return a reference
        // providing modifiable access to this object.  The behavior is
        // undefined unless the number of seconds in 'hours', and the total
        // number of seconds in the resulting time interval, can both be
        // represented with 64-bit signed integers.  Note that 'hours' may be
        // negative.

    TimeInterval& addMinutes(bsls::Types::Int64 minutes);
        // Add to this time interval the number of seconds represented by the
        // specified integral number of 'minutes', and return a reference
        // providing modifiable access to this object.  The behavior is
        // undefined unless the number of seconds in 'minutes', and the total
        // number of seconds in the resulting time interval, can both be
        // represented with 64-bit signed integers.  Note that 'minutes' may be
        // negative.

    TimeInterval& addSeconds(bsls::Types::Int64 seconds);
        // Add to this time interval the specified integral number of
        // 'seconds', and return a reference providing modifiable access to
        // this object.  The behavior is undefined unless the total number of
        // seconds in the resulting time interval can be represented with a
        // 64-bit signed integer.  Note that 'seconds' may be negative.

    TimeInterval& addMilliseconds(bsls::Types::Int64 milliseconds);
        // Add to this time interval the specified integral number of
        // 'milliseconds', and return a reference providing modifiable access
        // to this object.  The behavior is undefined unless the total number
        // of seconds in the resulting time interval can be represented with a
        // 64-bit signed integer.  Note that 'milliseconds' may be negative.

    TimeInterval& addMicroseconds(bsls::Types::Int64 microseconds);
        // Add to this time interval the specified integral number of
        // 'microseconds', and return a reference providing modifiable access
        // to this object.  The behavior is undefined unless the total number
        // of seconds in the resulting time interval can be represented with a
        // 64-bit signed integer.  Note that 'microseconds' may be negative.

    TimeInterval& addNanoseconds(bsls::Types::Int64 nanoseconds);
        // Add to this time interval the specified integral number of
        // 'nanoseconds', and return a reference providing modifiable access to
        // this object.  The behavior is undefined unless the total number of
        // seconds in the resulting time interval can be represented with a
        // 64-bit signed integer.  Note that 'nanoseconds' may be negative.

                                  // Set Operations

    void setTotalDays(bsls::Types::Int64 days);
        // Set the overall value of this object to indicate the specified
        // integral number of 'days'.  The behavior is undefined unless the
        // number of seconds in 'days' can be represented with a 64-bit signed
        // integer.  Note that 'days' may be negative.

    void setTotalHours(bsls::Types::Int64 hours);
        // Set the overall value of this object to indicate the specified
        // integral number of 'hours'.  The behavior is undefined unless the
        // number of seconds in 'hours' can be represented with a 64-bit signed
        // integer.  Note that 'hours' may be negative.

    void setTotalMinutes(bsls::Types::Int64 minutes);
        // Set the overall value of this object to indicate the specified
        // integral number of 'minutes'.  The behavior is undefined unless the
        // number of seconds in 'minutes' can be represented with a 64-bit
        // signed integer.  Note that 'minutes' may be negative.

    void setTotalSeconds(bsls::Types::Int64 seconds);
        // Set the overall value of this object to indicate the specified
        // integral number of 'seconds'.  Note that 'seconds' may be negative.

    void setTotalMilliseconds(bsls::Types::Int64 milliseconds);
        // Set the overall value of this object to indicate the specified
        // integral number of 'milliseconds'.  Note that 'milliseconds' may be
        // negative.

    void setTotalMicroseconds(bsls::Types::Int64 microseconds);
        // Set the overall value of this object to indicate the specified
        // integral number of 'microseconds'.  Note that 'microseconds' may be
        // negative.

    void setTotalNanoseconds(bsls::Types::Int64 nanoseconds);
        // Set the overall value of this object to indicate the specified
        // integral number of 'nanoseconds'.  Note that 'nanoseconds' may be
        // negative.

                                  // Time-Interval-Based Manipulators

    void addInterval(bsls::Types::Int64 seconds, int nanoseconds = 0);
        // Add to this time interval the specified integral number of
        // 'seconds', and the optionally specified integral number of
        // 'nanoseconds'.  If unspecified, 'nanoseconds' is 0.  The behavior is
        // undefined unless 'this->seconds() + seconds', and the total number
        // of seconds in the resulting time interval, can both be represented
        // with 64-bit signed integers.

    void setInterval(bsls::Types::Int64 seconds, int nanoseconds = 0);
        // Set this time interval to have the value given by the sum of the
        // specified integral number of 'seconds', and the optionally specified
        // integral number of 'nanoseconds'.  If unspecified, 'nanoseconds' is
        // 0.  The behavior is undefined unless the total number of seconds in
        // the resulting time interval can be represented with a 64-bit signed
        // integer (see 'isValid').  Note that there is no restriction on the
        // sign or magnitude of either argument except that they must not
        // violate the method's preconditions.

    void setIntervalRaw(bsls::Types::Int64 seconds, int nanoseconds = 0);
        // Set this time interval to have the value given by the sum of the
        // specified integral number of 'seconds', and the optionally specified
        // integral number of 'nanoseconds', where 'seconds' and 'nanoseconds'
        // form a canonical representation of a time interval (see
        // {Representation}).  If unspecified, 'nanoseconds' is 0.  The
        // behavior is undefined unless
        // '-999999999 <= nanoseconds <= 999999999' and 'seconds' and
        // 'nanoseconds' are either both non-negative or both non-positive.
        // Note that this function provides a subset of the defined behavior of
        // 'setInterval' chosen to minimize runtime performance cost.

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
    int nanoseconds() const;
        // Return the nanoseconds field in the canonical representation of the
        // value of this time interval.

    bsls::Types::Int64 seconds() const;
        // Return the seconds field in the canonical representation of the
        // value of this time interval.

    bsls::Types::Int64 totalDays() const;
        // Return the value of this time interval as an integral number of
        // days, rounded towards zero.  Note that the return value may be
        // negative.

    bsls::Types::Int64 totalHours() const;
        // Return the value of this time interval as an integral number of
        // hours, rounded towards zero.  Note that the return value may be
        // negative.

    bsls::Types::Int64 totalMinutes() const;
        // Return the value of this time interval as an integral number of
        // minutes, rounded towards zero.  Note that the return value may be
        // negative.

    bsls::Types::Int64 totalSeconds() const;
        // Return the value of this time interval as an integral number of
        // seconds, rounded towards zero.  Note that the return value may be
        // negative.  Also note that this method returns the same value as
        // 'seconds'.

    bsls::Types::Int64 totalMilliseconds() const;
        // Return the value of this time interval as an integral number of
        // milliseconds, rounded towards zero.  The behavior is undefined
        // unless the number of milliseconds can be represented with a 64-bit
        // signed integer.  Note that the return value may be negative.

    bsls::Types::Int64 totalMicroseconds() const;
        // Return the value of this time interval as an integral number of
        // microseconds, rounded towards zero.  The behavior is undefined
        // unless the number of microseconds can be represented with a 64-bit
        // signed integer.  Note that the return value may be negative.

    bsls::Types::Int64 totalNanoseconds() const;
        // Return the value of this time interval as an integral number of
        // nanoseconds.  The behavior is undefined unless the number of
        // nanoseconds can be represented using a 64-bit signed integer.  Note
        // that the return value may be negative.

    double totalSecondsAsDouble() const;
        // Return the value of this time interval as a real number of seconds.
        // Note that the return value may be negative and may have a fractional
        // part (representing the nanosecond field of this object).  Also note
        // that the conversion from the internal representation to a 'double'
        // may *lose* precision.

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

    native_std::ostream& print(native_std::ostream& stream,
                               int                  level          = 0,
                               int                  spacesPerLevel = 4) const;
        // Write the value of this object to the specified output 'stream' in a
        // human-readable format, and return a reference providing modifiable
        // access to 'stream'.  Optionally specify an initial indentation
        // 'level', whose absolute value is incremented recursively for nested
        // objects.  If 'level' is specified, optionally specify
        // 'spacesPerLevel', whose absolute value indicates the number of
        // spaces per indentation level for this and all of its nested objects.
        // If 'level' is negative, suppress indentation of the first line.  If
        // 'spacesPerLevel' is negative, format the entire output on one line,
        // suppressing all but the initial indentation (as governed by
        // 'level').  If 'stream' is not valid on entry, this operation has no
        // effect.  Note that the format is not fully specified, and can change
        // without notice.


};

// FREE OPERATORS
TimeInterval operator+(const TimeInterval& lhs, const TimeInterval& rhs);
TimeInterval operator+(const TimeInterval& lhs, double rhs);
TimeInterval operator+(double lhs, const TimeInterval& rhs);
    // Return a 'TimeInterval' value that is the sum of the specified 'lhs' and
    // 'rhs' time intervals.  The behavior is undefined unless (1) operands of
    // type 'double' can be converted to valid 'TimeInterval' objects, and (2)
    // the resulting time interval can be represented with a 64-bit signed
    // integer.

TimeInterval operator-(const TimeInterval& lhs, const TimeInterval& rhs);
TimeInterval operator-(const TimeInterval& lhs, double rhs);
TimeInterval operator-(double lhs, const TimeInterval& rhs);
    // Return a 'TimeInterval' value that is the difference between the
    // specified 'lhs' and 'rhs' time intervals.  The behavior is undefined
    // unless (1) operands of type 'double' can be converted to valid
    // 'TimeInterval' objects, (2) the value on the right-hand side
    // (potentially after conversion to a 'TimeInterval') has a number of
    // seconds that is not 'LLONG_MIN', and (3) the resulting time interval can
    // be represented with a 64-bit signed integer.

TimeInterval operator-(const TimeInterval& rhs);
    // Return a 'TimeInterval' value that is the negative of the specified
    // 'rhs' time interval.  The behavior is undefined unless
    // 'LLONG_MIN != rhs.seconds()'.

bool operator==(const TimeInterval& lhs, const TimeInterval& rhs);
bool operator==(const TimeInterval& lhs, double rhs);
bool operator==(double lhs, const TimeInterval& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time intervals have the
    // same value, and 'false' otherwise.  Two time intervals have the same
    // value if their respective second and nanosecond fields have the same
    // value.  The behavior is undefined unless operands of type 'double' can
    // be converted to valid 'TimeInterval' objects.

bool operator!=(const TimeInterval& lhs, const TimeInterval& rhs);
bool operator!=(const TimeInterval& lhs, double rhs);
bool operator!=(double lhs, const TimeInterval& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time intervals do not
    // have the same value, and 'false' otherwise.  Two time intervals do not
    // have the same value if their respective second or nanosecond fields
    // differ in value.  The behavior is undefined unless operands of type
    // 'double' can be converted to valid 'TimeInterval' objects.

bool operator< (const TimeInterval& lhs, const TimeInterval& rhs);
bool operator< (const TimeInterval& lhs, double rhs);
bool operator< (double lhs, const TimeInterval& rhs);
bool operator<=(const TimeInterval& lhs, const TimeInterval& rhs);
bool operator<=(const TimeInterval& lhs, double rhs);
bool operator<=(double lhs, const TimeInterval& rhs);
bool operator> (const TimeInterval& lhs, const TimeInterval& rhs);
bool operator> (const TimeInterval& lhs, double rhs);
bool operator> (double lhs, const TimeInterval& rhs);
bool operator>=(const TimeInterval& lhs, const TimeInterval& rhs);
bool operator>=(const TimeInterval& lhs, double rhs);
bool operator>=(double lhs, const TimeInterval& rhs);
    // Return 'true' if the nominal relation between the specified 'lhs' and
    // 'rhs' time interval values holds, and 'false' otherwise.  The behavior
    // is undefined unless operands of type 'double' can be converted to valid
    // 'TimeInterval' objects.

native_std::ostream& operator<<(native_std::ostream& stream,
                                const TimeInterval&  timeInterval);
    // Write the value of the specified 'timeInterval' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.  Also note that this
    // method has the same behavior as 'object.print(stream, 0, -1)'.

// ============================================================================
//                         INLINE DEFINITIONS
// ============================================================================

                        // ------------------
                        // class TimeInterval
                        // ------------------

// CLASS METHODS
inline
int TimeInterval::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

inline
bool TimeInterval::isValid(bsls::Types::Int64 seconds,
                           int                nanoseconds)
{
    return isSumValidInt64(seconds, nanoseconds / k_NANOSECS_PER_SEC);
}

// CREATORS
inline
TimeInterval::TimeInterval()
: d_seconds(0)
, d_nanoseconds(0)
{
}

inline
TimeInterval::TimeInterval(bsls::Types::Int64 seconds,
                           int                nanoseconds)
{
    setInterval(seconds, nanoseconds);
}

// MANIPULATORS
inline
TimeInterval& TimeInterval::operator=(double rhs)
{
    *this = TimeInterval(rhs);
    return *this;
}

inline
TimeInterval& TimeInterval::operator+=(const TimeInterval& rhs)
{
    addInterval(rhs.d_seconds, rhs.d_nanoseconds);
    return *this;
}

inline
TimeInterval& TimeInterval::operator+=(double rhs)
{
    *this += TimeInterval(rhs);
    return *this;
}


inline
TimeInterval& TimeInterval::operator-=(const TimeInterval& rhs)
{
    BSLS_ASSERT_SAFE(LLONG_MIN < rhs.seconds());

    addInterval(-rhs.d_seconds, -rhs.d_nanoseconds);
    return *this;
}

inline
TimeInterval& TimeInterval::operator-=(double rhs)
{
    *this -= TimeInterval(rhs);
    return *this;
}

                                  // Add Operations

inline
TimeInterval& TimeInterval::addDays(bsls::Types::Int64 days)
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_SECONDS_PER_DAY >= days &&
                     LLONG_MIN / k_SECONDS_PER_DAY <= days);

    return addSeconds(days * k_SECONDS_PER_DAY);
}

inline
TimeInterval& TimeInterval::addHours(bsls::Types::Int64 hours)
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_SECONDS_PER_HOUR >= hours &&
                     LLONG_MIN / k_SECONDS_PER_HOUR <= hours);

    return addSeconds(hours * k_SECONDS_PER_HOUR);
}

inline
TimeInterval& TimeInterval::addMinutes(bsls::Types::Int64 minutes)
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_SECONDS_PER_MINUTE >= minutes &&
                     LLONG_MIN / k_SECONDS_PER_MINUTE <= minutes);

    return addSeconds(minutes * k_SECONDS_PER_MINUTE);
}

inline
TimeInterval& TimeInterval::addMilliseconds(bsls::Types::Int64 milliseconds)
{
    addInterval(          milliseconds / k_MILLISECS_PER_SEC,
        static_cast<int>((milliseconds % k_MILLISECS_PER_SEC) *
                                                     k_NANOSECS_PER_MILLISEC));
    return *this;
}

inline
TimeInterval& TimeInterval::addMicroseconds(bsls::Types::Int64 microseconds)
{
    addInterval(          microseconds / k_MICROSECS_PER_SEC,
        static_cast<int>((microseconds % k_MICROSECS_PER_SEC) *
                                                     k_NANOSECS_PER_MICROSEC));
    return *this;

}

inline
TimeInterval& TimeInterval::addNanoseconds(bsls::Types::Int64 nanoseconds)
{
    addInterval(                 nanoseconds / k_NANOSECS_PER_SEC,
                static_cast<int>(nanoseconds % k_NANOSECS_PER_SEC));
    return *this;
}

                                  // Set Operations

inline
void TimeInterval::setTotalDays(bsls::Types::Int64 days)
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_SECONDS_PER_DAY >= days &&
                     LLONG_MIN / k_SECONDS_PER_DAY <= days);

    return setTotalSeconds(days * k_SECONDS_PER_DAY);
}

inline
void TimeInterval::setTotalHours(bsls::Types::Int64 hours)
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_SECONDS_PER_HOUR >= hours &&
                     LLONG_MIN / k_SECONDS_PER_HOUR <= hours);

    return setTotalSeconds(hours * k_SECONDS_PER_HOUR);
}

inline
void TimeInterval::setTotalMinutes(bsls::Types::Int64 minutes)
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_SECONDS_PER_MINUTE >= minutes &&
                     LLONG_MIN / k_SECONDS_PER_MINUTE <= minutes);

    return setTotalSeconds(minutes * k_SECONDS_PER_MINUTE);
}

inline
void TimeInterval::setTotalSeconds(bsls::Types::Int64 seconds)
{
    d_seconds     = seconds;
    d_nanoseconds = 0;
}

inline
void TimeInterval::setTotalMilliseconds(bsls::Types::Int64 milliseconds)
{
    setInterval(                  milliseconds / k_MILLISECS_PER_SEC,
                static_cast<int>((milliseconds % k_MILLISECS_PER_SEC) *
                                                     k_NANOSECS_PER_MILLISEC));
}

inline
void TimeInterval::setTotalMicroseconds(bsls::Types::Int64 microseconds)
{
    setInterval(                  microseconds / k_MICROSECS_PER_SEC,
                static_cast<int>((microseconds % k_MICROSECS_PER_SEC) *
                                                     k_NANOSECS_PER_MICROSEC));

}

inline
void TimeInterval::setTotalNanoseconds(bsls::Types::Int64 nanoseconds)
{
    setInterval(                 nanoseconds / k_NANOSECS_PER_SEC,
                static_cast<int>(nanoseconds % k_NANOSECS_PER_SEC));
}

inline
void TimeInterval::setIntervalRaw(bsls::Types::Int64 seconds,
                                  int                nanoseconds)
{
    BSLS_ASSERT_SAFE(-k_NANOSECS_PER_SEC < nanoseconds &&
                      k_NANOSECS_PER_SEC > nanoseconds);
    BSLS_ASSERT_SAFE((seconds >= 0 && nanoseconds >= 0) ||
                     (seconds <= 0 && nanoseconds <= 0));

    d_seconds     = seconds;
    d_nanoseconds = nanoseconds;
}

                                  // Aspects

template <class STREAM>
STREAM& TimeInterval::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            bsls::Types::Int64 seconds;
            int                nanoseconds;
            stream.getInt64(seconds);
            stream.getInt32(nanoseconds);

            if (stream && (   (seconds >= 0 && nanoseconds >= 0)
                           || (seconds <= 0 && nanoseconds <= 0))
                       && nanoseconds > -k_NANOSECS_PER_SEC
                       && nanoseconds <  k_NANOSECS_PER_SEC) {
                d_seconds     = seconds;
                d_nanoseconds = nanoseconds;
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
int TimeInterval::nanoseconds() const
{
    return d_nanoseconds;
}

inline
bsls::Types::Int64 TimeInterval::seconds() const
{
    return d_seconds;
}

inline
bsls::Types::Int64 TimeInterval::totalDays() const
{
    return d_seconds / k_SECONDS_PER_DAY;
}

inline
bsls::Types::Int64 TimeInterval::totalHours() const
{
    return d_seconds / k_SECONDS_PER_HOUR;
}

inline
bsls::Types::Int64 TimeInterval::totalMinutes() const
{
    return d_seconds / k_SECONDS_PER_MINUTE;
}

inline
bsls::Types::Int64 TimeInterval::totalSeconds() const
{
    return d_seconds;
}

inline
bsls::Types::Int64 TimeInterval::totalMilliseconds() const
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_MILLISECS_PER_SEC >= d_seconds &&
                     LLONG_MIN / k_MILLISECS_PER_SEC <= d_seconds);
    BSLS_ASSERT_SAFE(isSumValidInt64(d_seconds * k_MILLISECS_PER_SEC,
                                     d_nanoseconds / k_NANOSECS_PER_MILLISEC));


    return d_seconds     * k_MILLISECS_PER_SEC
         + d_nanoseconds / k_NANOSECS_PER_MILLISEC;
}

inline
bsls::Types::Int64 TimeInterval::totalMicroseconds() const
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_MICROSECS_PER_SEC >= d_seconds &&
                     LLONG_MIN / k_MICROSECS_PER_SEC <= d_seconds);
    BSLS_ASSERT_SAFE(isSumValidInt64(d_seconds     * k_MICROSECS_PER_SEC,
                                     d_nanoseconds / k_NANOSECS_PER_MICROSEC));

    return d_seconds     * k_MICROSECS_PER_SEC
         + d_nanoseconds / k_NANOSECS_PER_MICROSEC;
}

inline
bsls::Types::Int64 TimeInterval::totalNanoseconds() const
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_NANOSECS_PER_SEC >= d_seconds &&
                     LLONG_MIN / k_NANOSECS_PER_SEC <= d_seconds);
    BSLS_ASSERT_SAFE(isSumValidInt64(d_seconds * k_NANOSECS_PER_SEC,
                                     d_nanoseconds));

    return d_seconds * k_NANOSECS_PER_SEC + d_nanoseconds;
}

inline
double TimeInterval::totalSecondsAsDouble() const
{
    return static_cast<double>(d_seconds) + d_nanoseconds /
                                                    (1.0 * k_NANOSECS_PER_SEC);
}

                                  // Aspects

template <class STREAM>
STREAM& TimeInterval::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            stream.putInt64(d_seconds);
            stream.putInt32(d_nanoseconds);
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
bsls::TimeInterval bsls::operator+(const TimeInterval& lhs,
                                   const TimeInterval& rhs)
{
    TimeInterval result(lhs);
    result.addInterval(rhs.seconds(), rhs.nanoseconds());
    return result;
}

inline
bsls::TimeInterval bsls::operator+(const TimeInterval& lhs, double rhs)
{
    return lhs + TimeInterval(rhs);
}

inline
bsls::TimeInterval bsls::operator+(double lhs, const TimeInterval& rhs)
{
    return TimeInterval(lhs) + rhs;
}

inline
bsls::TimeInterval bsls::operator-(const TimeInterval& lhs,
                                   const TimeInterval& rhs)

{
    BSLS_ASSERT_SAFE(LLONG_MIN != rhs.seconds());

    TimeInterval result(lhs);
    result.addInterval(-rhs.seconds(), -rhs.nanoseconds());
    return result;
}

inline
bsls::TimeInterval bsls::operator-(const TimeInterval& lhs, double rhs)
{
    return lhs - TimeInterval(rhs);
}

inline
bsls::TimeInterval bsls::operator-(double lhs, const TimeInterval& rhs)
{
    return TimeInterval(lhs) - rhs;
}

inline
bsls::TimeInterval bsls::operator-(const TimeInterval& rhs)
{
    BSLS_ASSERT_SAFE(LLONG_MIN != rhs.seconds());

    return TimeInterval(-rhs.seconds(), -rhs.nanoseconds());
}

inline
bool bsls::operator==(const TimeInterval& lhs, const TimeInterval& rhs)
{
    return lhs.seconds()     == rhs.seconds()
        && lhs.nanoseconds() == rhs.nanoseconds();
}

inline
bool bsls::operator==(const TimeInterval& lhs, double rhs)
{
    return lhs == TimeInterval(rhs);
}

inline
bool bsls::operator==(double lhs, const TimeInterval& rhs)
{
    return TimeInterval(lhs) == rhs;
}

inline
bool bsls::operator!=(const TimeInterval& lhs, const TimeInterval& rhs)
{
    return lhs.seconds()     != rhs.seconds()
        || lhs.nanoseconds() != rhs.nanoseconds();
}

inline
bool bsls::operator!=(const TimeInterval& lhs, double rhs)
{
    return lhs != TimeInterval(rhs);
}

inline
bool bsls::operator!=(double lhs, const TimeInterval& rhs)
{
    return TimeInterval(lhs) != rhs;
}

inline
bool bsls::operator< (const TimeInterval& lhs, const TimeInterval& rhs)
{
    return lhs.seconds() < rhs.seconds()
        || (lhs.seconds() == rhs.seconds()
            && lhs.nanoseconds() < rhs.nanoseconds());
}

inline
bool bsls::operator< (const TimeInterval& lhs, double rhs)
{
    return lhs < TimeInterval(rhs);
}

inline
bool bsls::operator< (double lhs, const TimeInterval& rhs)
{
    return TimeInterval(lhs) < rhs;
}

inline
bool bsls::operator<=(const TimeInterval& lhs, const TimeInterval& rhs)
{
    return lhs.seconds() < rhs.seconds()
        || (lhs.seconds() == rhs.seconds()
            && lhs.nanoseconds() <= rhs.nanoseconds());
}

inline
bool bsls::operator<=(const TimeInterval& lhs, double rhs)
{
    return lhs <= TimeInterval(rhs);
}

inline
bool bsls::operator<=(double lhs, const TimeInterval& rhs)
{
    return TimeInterval(lhs) <= rhs;
}

inline
bool bsls::operator> (const TimeInterval& lhs, const TimeInterval& rhs)
{
    return lhs.seconds() > rhs.seconds()
        || (lhs.seconds() == rhs.seconds()
            && lhs.nanoseconds() > rhs.nanoseconds());
}

inline
bool bsls::operator> (const TimeInterval& lhs, double rhs)
{
    return lhs > TimeInterval(rhs);
}

inline
bool bsls::operator> (double lhs, const TimeInterval& rhs)
{
    return TimeInterval(lhs) > rhs;
}

inline
bool bsls::operator>=(const TimeInterval& lhs, const TimeInterval& rhs)
{
    return lhs.seconds() > rhs.seconds()
        || (lhs.seconds() == rhs.seconds()
            && lhs.nanoseconds() >= rhs.nanoseconds());
}

inline
bool bsls::operator>=(const TimeInterval& lhs, double rhs)
{
    return lhs >= TimeInterval(rhs);
}

inline
bool bsls::operator>=(double lhs, const TimeInterval& rhs)
{
    return TimeInterval(lhs) >= rhs;
}

// BDE_VERIFY pragma: pop

// IMPLEMENTATION NOTE: A 'is_trivially_copyable' trait declaration has been
// moved to 'bslmf_istriviallycopyable.h' to work around issues on the Sun CC
// 5.13 compiler.  We had previously forward declared
// 'bsl::is_trivially_copyable' and specialized it for 'TimeInterval' here (see
// the 2.24 release tags).
//..
//  namespace bsl {
//  template <>
//  struct is_trivially_copyable<BloombergLP::bsls::TimeInterval> :
//                                                            bsl::true_type {
//      // This template specialization for 'is_trivially_copyable' indicates
//      // that 'Date' is a trivially copyable type.
//  };
//  }
//..

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
