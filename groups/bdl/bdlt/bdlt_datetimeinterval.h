// bdlt_datetimeinterval.h                                            -*-C++-*-
#ifndef INCLUDED_BDLT_DATETIMEINTERVAL
#define INCLUDED_BDLT_DATETIMEINTERVAL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a representation of an interval of time.
//
//@CLASSES:
//  bdlt::DatetimeInterval: time interval with microsecond resolution
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component implements a time interval class,
// 'bdlt::DatetimeInterval', capable of representing the (signed) difference
// between two arbitrary points in time.  The time interval represented by a
// 'bdlt::DatetimeInterval' object has microsecond resolution.
//
///The Representation of a Time Interval
///-------------------------------------
// A time interval has a value that is independent of its representation.
// Conceptually, the interval between two points in time could be described
// using a (signed) real number of seconds (or minutes, or hours, etc.).  A
// 'bdlt::DatetimeInterval' represents this value as six fields: days, hours,
// minutes, seconds, milliseconds, and microseconds.  In the "canonical
// representation" of a time interval, the days field may have any 32-bit
// signed integer value, with the hours, minutes, seconds, milliseconds, and
// microseconds fields limited to the respective ranges '[-23 .. 23]',
// '[-59 .. 59]', '[-59 .. 59]', '[-999 .. 999]', and '[-999 .. 999]', with the
// additional constraint that the six fields are either all non-negative or all
// non-positive.  When setting the value of a time interval via its six-field
// representation, any integer value may be used in any field, with the
// constraint that the resulting number of days be representable as a 32-bit
// signed integer.  Similarly, the field values may be accessed in the
// canonical representation using the 'days', 'hours', 'minutes', 'seconds',
// 'milliseconds', and 'microseconds' methods.
//
// The primary accessors for this type are 'days' and
// 'fractionalDayInMicroseconds'.  In combination, these two methods provide
// complete and succinct access to the value of a 'DatetimeInterval'.
// Furthermore, the total value of the interval may be accessed in the
// respective field units via the 'totalDays', 'totalHours', 'totalMinutes',
// 'totalSeconds', 'totalMilliseconds', and 'totalMicroseconds' methods.  Note
// that, with the exception of 'totalMicroseconds' (which returns an exact
// result), the other "total" accessors round toward 0.  Also note that the
// 'totalMicroseconds' accessor can fail for extreme 'DatetimeInterval' values.
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
//  microseconds   [-999 .. 999]         all fields non-pos. or all non-neg.
//..
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
//                                     assert(  0 == i1.microseconds());
//..
// Then, set the value of 'i1' to -5 days, and then add 16 hours to that value:
//..
//  i1.setTotalDays(-5);
//  i1.addHours(16);                   assert( -4 == i1.days());
//                                     assert( -8 == i1.hours());
//                                     assert(  0 == i1.minutes());
//                                     assert(  0 == i1.seconds());
//                                     assert(  0 == i1.milliseconds());
//                                     assert(  0 == i1.microseconds());
//..
// Next, create 'i2' as a copy of 'i1':
//..
//  bdlt::DatetimeInterval i2(i1);     assert( -4 == i2.days());
//                                     assert( -8 == i2.hours());
//                                     assert(  0 == i2.minutes());
//                                     assert(  0 == i2.seconds());
//                                     assert(  0 == i2.milliseconds());
//                                     assert(  0 == i2.microseconds());
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
//                                     assert(  0 == i2.microseconds());
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
//  -2_07:59:56.000000
//..

#include <bdlscm_version.h>

#include <bdlt_timeunitratio.h>

#include <bslh_hash.h>

#include <bslmf_integralconstant.h>
#include <bslmf_istriviallycopyable.h>

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_log.h>
#include <bsls_performancehint.h>
#include <bsls_platform.h>
#include <bsls_review.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#include <bsl_cstdint.h>
#include <bsl_limits.h>
#include <bsl_iosfwd.h>

namespace BloombergLP {
namespace bdlt {

                          // ======================
                          // class DatetimeInterval
                          // ======================

class DatetimeInterval {
    // Each object of this class represents a (signed) time interval with
    // microsecond resolution.  See {The Representation of a Time Interval} for
    // details.

    // PRIVATE TYPES
    typedef bsls::Types::Int64 Int64;

    enum {
        k_DEFAULT_FRACTIONAL_SECOND_PRECISION = 6
    };

    // DATA
    int32_t            d_days;          // field for days
    bsls::Types::Int64 d_microseconds;  // field for fractional day

    // FRIENDS
    friend DatetimeInterval operator-(const DatetimeInterval&);

    friend bool operator==(const DatetimeInterval&, const DatetimeInterval&);
    friend bool operator!=(const DatetimeInterval&, const DatetimeInterval&);
    friend bool operator< (const DatetimeInterval&, const DatetimeInterval&);
    friend bool operator<=(const DatetimeInterval&, const DatetimeInterval&);
    friend bool operator> (const DatetimeInterval&, const DatetimeInterval&);
    friend bool operator>=(const DatetimeInterval&, const DatetimeInterval&);

    template <class HASHALG>
    friend void hashAppend(HASHALG&, const DatetimeInterval&);

    // PRIVATE MANIPULATORS
    void assign(bsls::Types::Int64 days, bsls::Types::Int64 microseconds);
        // Set this datetime interval to have the value given by the sum of the
        // specified 'days' and 'microseconds'.  The behavior is undefined
        // unless the total number of days, after converting to the canonical
        // representation, can be represented as an 'int'.  Note that it is
        // impossible for an 'Int64' to represent more than a day in
        // microseconds.  Also note that the arguments may be supplied using a
        // mixture of positive, negative, and 0 values.

    int assignIfValid(bsls::Types::Int64 days,
                      bsls::Types::Int64 microseconds);
        // Set this datetime interval to have the value given by the sum of the
        // specified 'days' and 'microseconds'.  Return 0 if the total number
        // of days, after converting to the canonical representation, can be
        // represented as an 'int' and a non-zero value (with no effect)
        // otherwise.  Note that it is impossible for an 'Int64' to represent
        // more than a day in microseconds.  Also note that the arguments may
        // be supplied using a mixture of positive, negative, and 0 values.

  public:
    // PUBLIC CLASS DATA
    static const bsls::Types::Int64 k_MILLISECONDS_MAX = 185542587187199999LL;
        // The maximum interval that is representable by a 'DatetimeInterval',
        // in milliseconds.

    static const bsls::Types::Int64 k_MILLISECONDS_MIN =
                   -k_MILLISECONDS_MAX - TimeUnitRatio::k_MILLISECONDS_PER_DAY;
        // The minimum interval that is representable by a 'DatetimeInterval',
        // in milliseconds.

    // CLASS METHODS
    static
    bool isValid(int                days,
                 bsls::Types::Int64 hours = 0,
                 bsls::Types::Int64 minutes = 0,
                 bsls::Types::Int64 seconds = 0,
                 bsls::Types::Int64 milliseconds = 0,
                 bsls::Types::Int64 microseconds = 0);
        // Return 'true' if a time interval object having the value given by
        // the specified 'days', and the optionally specified 'hours',
        // 'minutes', 'seconds', 'milliseconds', and 'microseconds' can be
        // represented as a 'DatetimeInterval' and 'false' otherwise.
        // Unspecified arguments default to 0.  The resulting time interval
        // value is valid if the days field does not overflow a 32-bit integer.
        // Note that the arguments may be supplied using a mixture of positive,
        // negative, and 0 values.

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
                     bsls::Types::Int64 hours = 0,
                     bsls::Types::Int64 minutes = 0,
                     bsls::Types::Int64 seconds = 0,
                     bsls::Types::Int64 milliseconds = 0,
                     bsls::Types::Int64 microseconds = 0);
        // Create a time interval object having the value given by the
        // specified 'days', and the optionally specified 'hours', 'minutes',
        // 'seconds', 'milliseconds', and 'microseconds'.  Unspecified
        // arguments default to 0.  The behavior is undefined unless the
        // resulting time interval value is valid (i.e., the days field must
        // not overflow a 32-bit integer).  Note that the arguments may be
        // supplied using a mixture of positive, negative, and 0 values.

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
                     bsls::Types::Int64 hours = 0,
                     bsls::Types::Int64 minutes = 0,
                     bsls::Types::Int64 seconds = 0,
                     bsls::Types::Int64 milliseconds = 0,
                     bsls::Types::Int64 microseconds = 0);
        // Set the time interval represented by this object to the value given
        // by the specified 'days', and the optionally specified 'hours',
        // 'minutes', 'seconds', 'milliseconds', and 'microseconds'.
        // Unspecified arguments default to 0.  The behavior is undefined
        // unless the resulting time interval value is valid (i.e., the days
        // field must not overflow a 32-bit integer).  Note that the arguments
        // may be supplied using a mixture of positive, negative, and 0 values.

    int setIntervalIfValid(int                days,
                           bsls::Types::Int64 hours = 0,
                           bsls::Types::Int64 minutes = 0,
                           bsls::Types::Int64 seconds = 0,
                           bsls::Types::Int64 milliseconds = 0,
                           bsls::Types::Int64 microseconds = 0);
        // Set the time interval represented by this object to the value given
        // by the specified 'days', and the optionally specified 'hours',
        // 'minutes', 'seconds', 'milliseconds', and 'microseconds'.
        // Unspecified arguments default to 0.  Return 0 if the resulting time
        // interval value is valid (i.e., the 'days' field must not overflow an
        // 'int') and a non-zero value (with no effect) otherwise.  Note that
        // the arguments may be supplied using a mixture of positive, negative,
        // and 0 values.

    void setTotalDays(int days);
        // Set the overall value of this object to indicate the specified
        // number of 'days'.

    void setTotalHours(bsls::Types::Int64 hours);
        // Set the overall value of this object to indicate the specified
        // number of 'hours'.  The behavior is undefined unless the resulting
        // time interval value is valid (i.e., the days field must not overflow
        // a 32-bit integer).

    int setTotalHoursIfValid(bsls::Types::Int64 hours);
        // Set the overall value of this object to indicate the specified
        // number of 'hours'.  Return 0 if the resulting time interval value is
        // valid (i.e., the 'days' field must not overflow an 'int') and a
        // non-zero value (with no effect) otherwise.

    void setTotalMinutes(bsls::Types::Int64 minutes);
        // Set the overall value of this object to indicate the specified
        // number of 'minutes'.  The behavior is undefined unless the resulting
        // time interval value is valid (i.e., the days field must not overflow
        // a 32-bit integer).

    int setTotalMinutesIfValid(bsls::Types::Int64 minutes);
        // Set the overall value of this object to indicate the specified
        // number of 'minutes'.  Return 0 if the resulting time interval value
        // is valid (i.e., the 'days' field must not overflow an 'int') and a
        // non-zero value (with no effect) otherwise.

    void setTotalSeconds(bsls::Types::Int64 seconds);
        // Set the overall value of this object to indicate the specified
        // number of 'seconds'.  The behavior is undefined unless the resulting
        // time interval value is valid (i.e., the days field must not overflow
        // a 32-bit integer).

    int setTotalSecondsIfValid(bsls::Types::Int64 seconds);
        // Set the overall value of this object to indicate the specified
        // number of 'seconds'.  Return 0 if the resulting time interval value
        // is valid (i.e., the 'days' field must not overflow an 'int') and a
        // non-zero value (with no effect) otherwise.

    void setTotalSecondsFromDouble(double seconds);
        // Set the overall value of this object to indicate the specified
        // number of 'seconds'.  The fractional part of 'seconds', if any, is
        // rounded to the nearest whole number of microseconds.  The behavior
        // is undefined unless the resulting time interval value is valid
        // (i.e., the days field must not overflow a 32-bit integer).

    int setTotalSecondsFromDoubleIfValid(double seconds);
        // Set the overall value of this object to indicate the specified
        // number of 'seconds'.  The fractional part of 'seconds', if any, is
        // rounded to the nearest whole number of microseconds.  Return 0 if
        // the resulting time interval value is valid (i.e., the 'days' field
        // must not overflow an 'int') and a non-zero value (with no effect)
        // otherwise.

    void setTotalMilliseconds(bsls::Types::Int64 milliseconds);
        // Set the overall value of this object to indicate the specified
        // number of 'milliseconds'.  The behavior is undefined unless the
        // resulting time interval value is valid (i.e., the days field must
        // not overflow a 32-bit integer).

    int setTotalMillisecondsIfValid(bsls::Types::Int64 milliseconds);
        // Set the overall value of this object to indicate the specified
        // number of 'milliseconds'.  Return 0 if the resulting time interval
        // value is valid (i.e., the days field must not overflow an 'int') and
        // a non-zero value (with no effect) otherwise.

    void setTotalMicroseconds(bsls::Types::Int64 microseconds);
        // Set the overall value of this object to indicate the specified
        // number of 'microseconds'.  Note that there is no
        // 'setTotalMicrosecondsIfValid' because no value of 'microseconds' can
        // cause the number of days to overflow.

    DatetimeInterval& addInterval(int                days,
                                  bsls::Types::Int64 hours = 0,
                                  bsls::Types::Int64 minutes = 0,
                                  bsls::Types::Int64 seconds = 0,
                                  bsls::Types::Int64 milliseconds = 0,
                                  bsls::Types::Int64 microseconds = 0);
        // Add to this time interval the specified number of 'days', and the
        // optionally specified number of 'hours', 'minutes', 'seconds',
        // 'milliseconds', and 'microseconds', and return a reference providing
        // modifiable access to this object.  Unspecified arguments default to
        // 0.  The behavior is undefined unless the resulting time interval
        // value is valid (i.e., the days field must not overflow a 32-bit
        // integer).  Note that the arguments may be supplied using a mixture
        // of positive, negative, and 0 values.

    int addIntervalIfValid(int                days,
                           bsls::Types::Int64 hours = 0,
                           bsls::Types::Int64 minutes = 0,
                           bsls::Types::Int64 seconds = 0,
                           bsls::Types::Int64 milliseconds = 0,
                           bsls::Types::Int64 microseconds = 0);
        // Add to this time interval the specified number of 'days', and the
        // optionally specified number of 'hours', 'minutes', 'seconds',
        // 'milliseconds', and 'microseconds'.  Return 0 if the resulting time
        // interval value is valid (i.e., the days field must not overflow an
        // 'int') and a non-zero value (with no effect) otherwise.  Note that
        // the arguments may be supplied using a mixture of positive, negative,
        // and 0 values.

    DatetimeInterval& addDays(int days);
        // Add to this time interval the specified number of 'days', and return
        // a reference providing modifiable access to this object.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the days field must not overflow a 32-bit integer).

    int addDaysIfValid(int days);
        // Add to this time interval the specified number of 'days'.  Return 0
        // if the resulting time interval value is valid (i.e., the days field
        // must not overflow an 'int') and a non-zero value (with no effect)
        // otherwise.

    DatetimeInterval& addHours(bsls::Types::Int64 hours);
        // Add to this time interval the specified number of 'hours', and
        // return a reference providing modifiable access to this object.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the days field must not overflow a 32-bit integer).

    int addHoursIfValid(bsls::Types::Int64 hours);
        // Add to this time interval the specified number of 'hours'.  Return 0
        // if the resulting time interval value is valid (i.e., the days field
        // must not overflow an 'int') and a non-zero value (with no effect)
        // otherwise.

    DatetimeInterval& addMinutes(bsls::Types::Int64 minutes);
        // Add to this time interval the specified number of 'minutes', and
        // return a reference providing modifiable access to this object.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the days field must not overflow a 32-bit integer).

    int addMinutesIfValid(bsls::Types::Int64 minutes);
        // Add to this time interval the specified number of 'minutes'.  Return
        // 0 if the resulting time interval value is valid (i.e., the days
        // field must not overflow an 'int') and a non-zero value (with no
        // effect) otherwise.

    DatetimeInterval& addSeconds(bsls::Types::Int64 seconds);
        // Add to this time interval the specified number of 'seconds', and
        // return a reference providing modifiable access to this object.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the days field must not overflow a 32-bit integer).

    int addSecondsIfValid(bsls::Types::Int64 seconds);
        // Add to this time interval the specified number of 'seconds'.  Return
        // 0 if the resulting time interval value is valid (i.e., the days
        // field must not overflow an 'int') and a non-zero value (with no
        // effect) otherwise.

    DatetimeInterval& addMilliseconds(bsls::Types::Int64 milliseconds);
        // Add to this time interval the specified number of 'milliseconds',
        // and return a reference providing modifiable access to this object.
        // The behavior is undefined unless the resulting time interval value
        // is valid (i.e., the days field must not overflow a 32-bit integer).

    int addMillisecondsIfValid(bsls::Types::Int64 milliseconds);
        // Add to this time interval the specified number of 'milliseconds'.
        // Return 0 if the resulting time interval value is valid (i.e., the
        // days field must not overflow an 'int') and a non-zero value (with no
        // effect) otherwise.

    DatetimeInterval& addMicroseconds(bsls::Types::Int64 microseconds);
        // Add to this time interval the specified number of 'microseconds',
        // and return a reference providing modifiable access to this object.
        // The behavior is undefined unless the resulting time interval value
        // is valid (i.e., the days field must not overflow a 32-bit integer).

    int addMicrosecondsIfValid(bsls::Types::Int64 microseconds);
        // Add to this time interval the specified number of 'microseconds'.
        // Return 0 if the resulting time interval value is valid (i.e., the
        // days field must not overflow an 'int') and a non-zero value (with no
        // effect) otherwise.

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

    bsls::Types::Int64 fractionalDayInMicroseconds() const;
        // Return the value of this time interval as an integral number of
        // microseconds modulo the number of microseconds in a day.  Note that
        // the return value may be negative.

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

    int microseconds() const;
        // Return the microseconds field in the canonical representation of the
        // value of this time interval.  Note that the return value may be
        // negative.

    int printToBuffer(char *result,
                      int   numBytes,
                      int   fractionalSecondPrecision = 6) const;
        // Efficiently write to the specified 'result' buffer no more than the
        // specified 'numBytes' of a representation of the value of this
        // object.  Optionally specify 'fractionalSecondPrecision' digits to
        // indicate how many fractional second digits to output.  If
        // 'fractionalSecondPrecision' is not specified then 6 fractional
        // second digits will be output (3 digits for milliseconds and 3 digits
        // for microseconds).  Return the number of characters (not including
        // the null character) that would have been written if the limit due to
        // 'numBytes' were not imposed.  'result' is null-terminated unless
        // 'numBytes' is 0.  The behavior is undefined unless '0 <= numBytes',
        // '0 <= fractionalSecondPrecision <= 6', and 'result' refers to at
        // least 'numBytes' contiguous bytes.  Note that the return value is
        // greater than or equal to 'numBytes' if the output representation was
        // truncated to avoid 'result' overrun.

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
        // Return the value of this time interval in integral milliseconds,
        // rounded towards zero.  Note that the return value may be negative.

    bsls::Types::Int64 totalMicroseconds() const;
        // Return the value of this time interval as an integral number of
        // microseconds.  The behavior is undefined unless the number of
        // microseconds can be represented with a 64-bit signed integer.  Note
        // that the return value may be negative.

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

#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation

    // DEPRECATED METHODS
    static int maxSupportedBdexVersion();
        // !DEPRECATED!: Use 'maxSupportedBdexVersion(int)' instead.
        //
        // Return the most current BDEX streaming version number supported by
        // this class.

#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
    static int maxSupportedVersion();
        // !DEPRECATED!: Use 'maxSupportedBdexVersion(int)' instead.
        //
        // Return the most current BDEX streaming version number supported by
        // this class.

    bsl::ostream& streamOut(bsl::ostream& stream) const;
        // !DEPRECATED!: use 'operator<<' or 'print' instead.
        //
        // Format this datetime interval to the specified output 'stream', and
        // return a reference to 'stream'.

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22

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
    // seconds, milliseconds, and microseconds fields are the same.

bool operator!=(const DatetimeInterval& lhs, const DatetimeInterval& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time intervals do not
    // have the same value, and 'false' otherwise.  Two time intervals do not
    // have the same value if any of the corresponding values of their days,
    // hours, minutes, seconds, milliseconds, or microseconds fields is not
    // the same.

bool operator< (const DatetimeInterval& lhs, const DatetimeInterval& rhs);
bool operator<=(const DatetimeInterval& lhs, const DatetimeInterval& rhs);
bool operator> (const DatetimeInterval& lhs, const DatetimeInterval& rhs);
bool operator>=(const DatetimeInterval& lhs, const DatetimeInterval& rhs);
    // Return 'true' if the nominal relation between the specified 'lhs' and
    // 'rhs' time interval values holds, and 'false' otherwise.  'lhs' is less
    // than 'rhs' if the following expression evaluates to 'true':
    //..
    //     lhs.days() < rhs.days()
    //  || (lhs.days() == rhs.days() && lhs.fractionalDayInMicroseconds()
    //                                     < rhs.fractionalDayInMicroseconds())
    //..
    // The other relationships are defined similarly.

bsl::ostream& operator<<(bsl::ostream& stream, const DatetimeInterval& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.  Also note that this
    // method has the same behavior as 'object.print(stream, 0, -1)'.

// FREE FUNCTIONS
template <class HASHALG>
void hashAppend(HASHALG& hashAlg, const DatetimeInterval& object);
    // Pass the specified 'object' to the specified 'hashAlg'.  This function
    // integrates with the 'bslh' modular hashing system and effectively
    // provides a 'bsl::hash' specialization for 'DatetimeInterval'.

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                          // ----------------------
                          // class DatetimeInterval
                          // ----------------------

// CLASS METHODS

                                  // Aspects

inline
int DatetimeInterval::maxSupportedBdexVersion(int versionSelector)
{
    if (versionSelector >= 20170401) {
        return 2;                                                     // RETURN
    }
    return 1;
}

// CREATORS
inline
DatetimeInterval::DatetimeInterval()
: d_days(0)
, d_microseconds(0)
{
}

inline
DatetimeInterval::DatetimeInterval(int                days,
                                   bsls::Types::Int64 hours,
                                   bsls::Types::Int64 minutes,
                                   bsls::Types::Int64 seconds,
                                   bsls::Types::Int64 milliseconds,
                                   bsls::Types::Int64 microseconds)
{
    setInterval(days,
                hours,
                minutes,
                seconds,
                milliseconds,
                microseconds);
}

inline
DatetimeInterval::DatetimeInterval(const DatetimeInterval& original)
: d_days(original.d_days)
, d_microseconds(original.d_microseconds)
{
}

// MANIPULATORS
inline
DatetimeInterval& DatetimeInterval::operator=(const DatetimeInterval& rhs)
{
    d_days = rhs.d_days;
    d_microseconds = rhs.d_microseconds;
    return *this;
}

inline
DatetimeInterval& DatetimeInterval::operator+=(const DatetimeInterval& rhs)
{
#ifdef BSLS_ASSERT_IS_USED
    int rc = addIntervalIfValid(rhs.d_days, 0, 0, 0, 0, rhs.d_microseconds);
    BSLS_ASSERT(0 == rc && "operator+= over/under flow");  (void) rc;
#else
    addInterval(rhs.d_days, 0, 0, 0, 0, rhs.d_microseconds);
#endif

    return *this;
}

inline
DatetimeInterval& DatetimeInterval::operator-=(const DatetimeInterval& rhs)
{
    Int64 rhsDays         = rhs.d_days;
    Int64 rhsMicroseconds = rhs.d_microseconds;
    if (INT_MIN == rhsDays) {
        ++rhsDays;
        rhsMicroseconds -= TimeUnitRatio::k_US_PER_D;
    }
    rhsDays         = -rhsDays;
    rhsMicroseconds = -rhsMicroseconds;
    BSLS_ASSERT_SAFE(rhsDays <= INT_MAX && INT_MIN < rhsDays);   // always true

#ifdef BSLS_ASSERT_IS_USED
    int rc = addIntervalIfValid(
                       static_cast<int>(rhsDays), 0, 0, 0, 0, rhsMicroseconds);
    BSLS_ASSERT(0 == rc && "operator-= over/under flow"); (void) rc;
#else
    addInterval(static_cast<int>(rhsDays), 0, 0, 0, 0, rhsMicroseconds);
#endif

    return *this;
}

inline
void DatetimeInterval::setTotalDays(int days)
{
    d_days         = days;
    d_microseconds = 0;
}

inline
void DatetimeInterval::setTotalHours(bsls::Types::Int64 hours)
{
    assign(hours / TimeUnitRatio::k_H_PER_D,
           hours % TimeUnitRatio::k_H_PER_D * TimeUnitRatio::k_US_PER_H);
}

inline
int DatetimeInterval::setTotalHoursIfValid(bsls::Types::Int64 hours)
{
    return assignIfValid(hours / TimeUnitRatio::k_H_PER_D,
                        (hours % TimeUnitRatio::k_H_PER_D) *
                                                    TimeUnitRatio::k_US_PER_H);
}

inline
void DatetimeInterval::setTotalMinutes(bsls::Types::Int64 minutes)
{
    assign(minutes / TimeUnitRatio::k_M_PER_D,
           minutes % TimeUnitRatio::k_M_PER_D * TimeUnitRatio::k_US_PER_M);
}

inline
int DatetimeInterval::setTotalMinutesIfValid(bsls::Types::Int64 minutes)
{
    return assignIfValid(minutes / TimeUnitRatio::k_M_PER_D,
                        (minutes % TimeUnitRatio::k_M_PER_D) *
                                                    TimeUnitRatio::k_US_PER_M);
}

inline
void DatetimeInterval::setTotalSeconds(bsls::Types::Int64 seconds)
{
    assign(seconds / TimeUnitRatio::k_S_PER_D,
           seconds % TimeUnitRatio::k_S_PER_D * TimeUnitRatio::k_US_PER_S);
}

inline
int DatetimeInterval::setTotalSecondsIfValid(bsls::Types::Int64 seconds)
{
    return assignIfValid(seconds / TimeUnitRatio::k_S_PER_D,
                        (seconds % TimeUnitRatio::k_S_PER_D) *
                                                    TimeUnitRatio::k_US_PER_S);
}

inline
void DatetimeInterval::setTotalMilliseconds(bsls::Types::Int64 milliseconds)
{
    assign(milliseconds / TimeUnitRatio::k_MS_PER_D,
           milliseconds % TimeUnitRatio::k_MS_PER_D
                                                 * TimeUnitRatio::k_US_PER_MS);
}

inline
int DatetimeInterval::setTotalMillisecondsIfValid(
                                               bsls::Types::Int64 milliseconds)
{
    return assignIfValid(milliseconds / TimeUnitRatio::k_MS_PER_D,
                        (milliseconds % TimeUnitRatio::k_MS_PER_D) *
                                                   TimeUnitRatio::k_US_PER_MS);
}

inline
void DatetimeInterval::setTotalMicroseconds(bsls::Types::Int64 microseconds)
{
    assign(microseconds / TimeUnitRatio::k_US_PER_D,
           microseconds % TimeUnitRatio::k_US_PER_D);
}

inline
DatetimeInterval& DatetimeInterval::addDays(int days)
{
    assign(static_cast<bsls::Types::Int64>(d_days)
                                       + static_cast<bsls::Types::Int64>(days),
           d_microseconds);
    return *this;
}

inline
int DatetimeInterval::addDaysIfValid(int days)
{
    return assignIfValid(static_cast<bsls::Types::Int64>(d_days)
                                       + static_cast<bsls::Types::Int64>(days),
                         d_microseconds);
}

inline
DatetimeInterval& DatetimeInterval::addHours(bsls::Types::Int64 hours)
{
    assign(static_cast<bsls::Types::Int64>(d_days)
                                            + hours / TimeUnitRatio::k_H_PER_D,
           d_microseconds +
                 hours % TimeUnitRatio::k_H_PER_D * TimeUnitRatio::k_US_PER_H);
    return *this;
}

inline
int DatetimeInterval::addHoursIfValid(bsls::Types::Int64 hours)
{
    return assignIfValid(static_cast<bsls::Types::Int64>(d_days)
                                            + hours / TimeUnitRatio::k_H_PER_D,
                         d_microseconds + (hours % TimeUnitRatio::k_H_PER_D) *
                                                    TimeUnitRatio::k_US_PER_H);
}

inline
DatetimeInterval& DatetimeInterval::addMinutes(bsls::Types::Int64 minutes)
{
    assign(static_cast<bsls::Types::Int64>(d_days)
                                          + minutes / TimeUnitRatio::k_M_PER_D,
           d_microseconds +
               minutes % TimeUnitRatio::k_M_PER_D * TimeUnitRatio::k_US_PER_M);
    return *this;
}

inline
int DatetimeInterval::addMinutesIfValid(bsls::Types::Int64 minutes)
{
    return assignIfValid(static_cast<bsls::Types::Int64>(d_days)
                                          + minutes / TimeUnitRatio::k_M_PER_D,
                         d_microseconds +
                                 (minutes % TimeUnitRatio::k_M_PER_D) *
                                                    TimeUnitRatio::k_US_PER_M);
}

inline
DatetimeInterval& DatetimeInterval::addSeconds(bsls::Types::Int64 seconds)
{
    assign(static_cast<bsls::Types::Int64>(d_days)
                                          + seconds / TimeUnitRatio::k_S_PER_D,
           d_microseconds +
               seconds % TimeUnitRatio::k_S_PER_D * TimeUnitRatio::k_US_PER_S);
    return *this;
}

inline
int DatetimeInterval::addSecondsIfValid(bsls::Types::Int64 seconds)
{
    return assignIfValid(static_cast<bsls::Types::Int64>(d_days)
                                          + seconds / TimeUnitRatio::k_S_PER_D,
                         d_microseconds +
                                 (seconds % TimeUnitRatio::k_S_PER_D) *
                                                    TimeUnitRatio::k_US_PER_S);
}

inline
DatetimeInterval&
DatetimeInterval::addMilliseconds(bsls::Types::Int64 milliseconds)
{
    assign(static_cast<bsls::Types::Int64>(d_days)
                                    + milliseconds / TimeUnitRatio::k_MS_PER_D,
           d_microseconds + milliseconds % TimeUnitRatio::k_MS_PER_D
                                                 * TimeUnitRatio::k_US_PER_MS);
    return *this;
}

inline
int DatetimeInterval::addMillisecondsIfValid(bsls::Types::Int64 milliseconds)
{
    return assignIfValid(static_cast<bsls::Types::Int64>(d_days)
                                    + milliseconds / TimeUnitRatio::k_MS_PER_D,
                         d_microseconds +
                               (milliseconds % TimeUnitRatio::k_MS_PER_D) *
                                                   TimeUnitRatio::k_US_PER_MS);
}

inline
DatetimeInterval&
DatetimeInterval::addMicroseconds(bsls::Types::Int64 microseconds)
{
    assign(static_cast<bsls::Types::Int64>(d_days)
                                    + microseconds / TimeUnitRatio::k_US_PER_D,
           d_microseconds + microseconds % TimeUnitRatio::k_US_PER_D);
    return *this;
}

inline
int DatetimeInterval::addMicrosecondsIfValid(bsls::Types::Int64 microseconds)
{
    return assignIfValid(static_cast<bsls::Types::Int64>(d_days)
                                    + microseconds / TimeUnitRatio::k_US_PER_D,
                         d_microseconds +
                                     microseconds % TimeUnitRatio::k_US_PER_D);
}

                                  // Aspects

template <class STREAM>
STREAM& DatetimeInterval::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 2: {
            int tmpDays;
            stream.getInt32(tmpDays);

            bsls::Types::Int64 tmpMicroseconds;
            stream.getInt64(tmpMicroseconds);

            if (   stream
                && (   (0 <= tmpDays && 0 <= tmpMicroseconds)
                    || (0 >= tmpDays && 0 >= tmpMicroseconds))
                &&  TimeUnitRatio::k_US_PER_D > tmpMicroseconds
                && -TimeUnitRatio::k_US_PER_D < tmpMicroseconds) {
                assign(tmpDays, tmpMicroseconds);
            }
            else {
                stream.invalidate();
            }
          } break;
          case 1: {
            bsls::Types::Int64 tmp;
            stream.getInt64(tmp);

            if (   stream
                && k_MILLISECONDS_MIN <= tmp && k_MILLISECONDS_MAX >= tmp) {
                setTotalMilliseconds(tmp);
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
    return d_days;
}

inline
bsls::Types::Int64 DatetimeInterval::fractionalDayInMicroseconds() const
{
    return d_microseconds;
}

inline
int DatetimeInterval::hours() const
{
    return static_cast<int>(d_microseconds / TimeUnitRatio::k_US_PER_H);
}

inline
int DatetimeInterval::minutes() const
{
    return static_cast<int>(d_microseconds / TimeUnitRatio::k_US_PER_M
                                                   % TimeUnitRatio::k_M_PER_H);
}

inline
int DatetimeInterval::seconds() const
{
    return static_cast<int>(d_microseconds / TimeUnitRatio::k_US_PER_S
                                                   % TimeUnitRatio::k_S_PER_M);
}

inline
int DatetimeInterval::milliseconds() const
{
    return static_cast<int>(d_microseconds / TimeUnitRatio::k_US_PER_MS
                                                  % TimeUnitRatio::k_MS_PER_S);
}

inline
int DatetimeInterval::microseconds() const
{
    return static_cast<int>(d_microseconds % TimeUnitRatio::k_US_PER_MS);
}

inline
int DatetimeInterval::totalDays() const
{
    return d_days;
}

inline
bsls::Types::Int64 DatetimeInterval::totalHours() const
{
    return static_cast<bsls::Types::Int64>(d_days) * TimeUnitRatio::k_H_PER_D
                                  + d_microseconds / TimeUnitRatio::k_US_PER_H;
}

inline
bsls::Types::Int64 DatetimeInterval::totalMinutes() const
{
    return static_cast<bsls::Types::Int64>(d_days) * TimeUnitRatio::k_M_PER_D
                                  + d_microseconds / TimeUnitRatio::k_US_PER_M;
}

inline
bsls::Types::Int64 DatetimeInterval::totalSeconds() const
{
    return static_cast<bsls::Types::Int64>(d_days) * TimeUnitRatio::k_S_PER_D
                                  + d_microseconds / TimeUnitRatio::k_US_PER_S;
}

inline
double DatetimeInterval::totalSecondsAsDouble() const
{
    return d_days * static_cast<double>(TimeUnitRatio::k_S_PER_D) +
                         static_cast<double>(d_microseconds) /
                                static_cast<double>(TimeUnitRatio::k_US_PER_S);
}

inline
bsls::Types::Int64 DatetimeInterval::totalMilliseconds() const
{
    return static_cast<bsls::Types::Int64>(d_days) * TimeUnitRatio::k_MS_PER_D
                                 + d_microseconds / TimeUnitRatio::k_US_PER_MS;
}

inline
bsls::Types::Int64 DatetimeInterval::totalMicroseconds() const
{
    BSLS_REVIEW(   0 >= d_days
                     || (bsl::numeric_limits<bsls::Types::Int64>::max() -
                        d_microseconds) / TimeUnitRatio::k_US_PER_D >= d_days);

#if !defined(BSLS_PLATFORM_CMP_SUN) \
 || !defined(BDE_BUILD_TARGET_OPT) \
 || BSLS_PLATFORM_CMP_VERSION >= 0x5140

    // Older versions of the Sun compiler (e.g., 5.12.3 and 5.12.4) fail to
    // compile the following 'BSLS_REVIEW' correctly in optimized builds.

    BSLS_REVIEW(   0 <= d_days
                     || (bsl::numeric_limits<bsls::Types::Int64>::min() -
                        d_microseconds) / TimeUnitRatio::k_US_PER_D <= d_days);

#endif

    return static_cast<bsls::Types::Int64>(d_days) * TimeUnitRatio::k_US_PER_D
                                                              + d_microseconds;
}

                                  // Aspects

template <class STREAM>
STREAM& DatetimeInterval::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 2: {
            stream.putInt32(d_days);
            stream.putInt64(d_microseconds);
          } break;
          case 1: {
            stream.putInt64(totalMilliseconds());
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation

// DEPRECATED METHODS
inline
int DatetimeInterval::maxSupportedBdexVersion()
{
    return maxSupportedBdexVersion(0);
}

#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
inline
int DatetimeInterval::maxSupportedVersion()
{
    return maxSupportedBdexVersion(0);
}

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22

}  // close package namespace

// FREE OPERATORS
inline
bdlt::DatetimeInterval bdlt::operator+(const DatetimeInterval& lhs,
                                       const DatetimeInterval& rhs)
{
    DatetimeInterval ret(lhs);

#ifdef BSLS_ASSERT_IS_USED
    int rc = ret.addIntervalIfValid(0, 0, 0, 0,
                                  rhs.totalMilliseconds(), rhs.microseconds());
    BSLS_ASSERT(0 == rc && "operator+ over/under flow");    (void) rc;
#else
    ret.addInterval(0, 0, 0, 0, rhs.totalMilliseconds(), rhs.microseconds());
#endif

    return ret;
}

inline
bdlt::DatetimeInterval bdlt::operator-(const DatetimeInterval& lhs,
                                       const DatetimeInterval& rhs)
{
    DatetimeInterval ret(lhs);

#ifdef BSLS_ASSERT_IS_USED
    int rc = ret.addIntervalIfValid(0, 0, 0, 0,
                                -rhs.totalMilliseconds(), -rhs.microseconds());
    BSLS_ASSERT(0 == rc && "operator- over/under flow");    (void) rc;
#else
    ret.addInterval(0, 0, 0, 0, -rhs.totalMilliseconds(), -rhs.microseconds());
#endif

    return ret;
}

inline
bdlt::DatetimeInterval bdlt::operator-(const DatetimeInterval& value)
{
    BSLS_REVIEW(value.d_days > bsl::numeric_limits<int32_t>::min());

    DatetimeInterval interval;

    interval.d_days = -value.d_days;
    interval.d_microseconds = -value.d_microseconds;

    return interval;
}

inline
bool bdlt::operator==(const DatetimeInterval& lhs, const DatetimeInterval& rhs)
{
    return lhs.d_days == rhs.d_days
        && lhs.d_microseconds == rhs.d_microseconds;
}

inline
bool bdlt::operator!=(const DatetimeInterval& lhs, const DatetimeInterval& rhs)
{
    return lhs.d_days != rhs.d_days
        || lhs.d_microseconds != rhs.d_microseconds;
}

inline
bool bdlt::operator< (const DatetimeInterval& lhs,
                      const DatetimeInterval& rhs)
{
    return lhs.d_days < rhs.d_days
        || (   lhs.d_days == rhs.d_days
            && lhs.d_microseconds < rhs.d_microseconds);
}

inline
bool bdlt::operator<=(const DatetimeInterval& lhs,
                      const DatetimeInterval& rhs)
{
    return lhs.d_days < rhs.d_days
        || (   lhs.d_days == rhs.d_days
            && lhs.d_microseconds <= rhs.d_microseconds);
}

inline
bool bdlt::operator> (const DatetimeInterval& lhs, const DatetimeInterval& rhs)
{
    return lhs.d_days > rhs.d_days
        || (   lhs.d_days == rhs.d_days
            && lhs.d_microseconds > rhs.d_microseconds);
}

inline
bool bdlt::operator>=(const DatetimeInterval& lhs, const DatetimeInterval& rhs)
{
    return lhs.d_days > rhs.d_days
        || (   lhs.d_days == rhs.d_days
            && lhs.d_microseconds >= rhs.d_microseconds);
}

inline
bsl::ostream& bdlt::operator<<(bsl::ostream&           stream,
                               const DatetimeInterval& object)
{
    return object.print(stream, 0, -1);
}

// FREE FUNCTIONS
template <class HASHALG>
void bdlt::hashAppend(HASHALG& hashAlg, const DatetimeInterval& object)
{
    using ::BloombergLP::bslh::hashAppend;
    hashAppend(hashAlg, object.d_days);
    hashAppend(hashAlg, object.d_microseconds);
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
// Copyright 2017 Bloomberg Finance L.P.
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
