// bdet_datetimeinterval.h                                            -*-C++-*-
#ifndef INCLUDED_BDET_DATETIMEINTERVAL
#define INCLUDED_BDET_DATETIMEINTERVAL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a representation of an interval of time.
//
//@CLASSES:
//   bdet_DatetimeInterval: time interval representation (millisecond res.)
//
//@SEE_ALSO: bdet_date, bdet_time, bdet_datetime, bdet_timeinterval
//
//@AUTHOR: Tom Marshall (tmarshal)
//
//@DESCRIPTION: This component implements a time interval class capable of
// representing the (signed) difference between two arbitrary points in time.
//
///The Representation of a Time Interval
///-------------------------------------
// A time interval has a value that is independent of its representation.
// Conceptually, a time interval may be thought of as a signed,
// arbitrary-precision floating-point number denominated in seconds (or in
// days, or in fortnights, if one prefers).  A 'bdet_DatetimeInterval'
// represents this value as five fields: days, hours, minutes, seconds, and
// milliseconds.  In the "canonical representation" of a time interval, the
// 'days' field may have any valid 32-bit integer value, with the 'hours',
// 'minutes, 'seconds', and 'milliseconds' fields limited to the respective
// ranges '[ -23 .. 23 ]', '[ -59 .. 59 ]', '[ -59 .. 59 ]', and
// '[ -999 .. 999 ]' and with the additional constraint that the five fields
// are either all non-negative or all non-positive.  When setting the value of
// a time interval via its five-field representation, any integer value may be
// used in any field, with the constraint that the resulting number of days be
// representable as a 32-bit integer and the slightly-less-convenient condition
// that no one field or intermediate sum of fields can overflow a 64-bit
// integer of *milliseconds*.  Similarly, the field values may be accessed in
// the canonical representation using the 'days', 'hours', 'minutes',
// 'seconds', and 'milliseconds' methods.  The total value of the time interval
// (rounded towards zero) may be accessed in the respective field units via the
// 'totalDays', 'totalHours', 'totalMinutes', 'totalSeconds', and
// 'totalMilliseconds' methods.
//
// The output format for printing the value of a time interval is
// 'sD...D_HH:MM:SS.SSS' where 's' is the sign character ('+' or '-').
//
// The following summarizes the canonical representation of the value of a
// 'bdet_DatetimeInterval':
//..
//    Field Name     Max. Valid Range    Auxiliary Conditions Limiting Validity
//    ----------     ----------------    --------------------------------------
//    days           any 32-bit integer      all fields non-pos or all non-neg
//    hours          [ -23 .. 23 ]           all fields non-pos or all non-neg
//    minutes        [ -59 .. 59 ]           all fields non-pos or all non-neg
//    seconds        [ -59 .. 59 ]           all fields non-pos or all non-neg
//    milliseconds   [ -999 .. 999 ]         all fields non-pos or all non-neg
//..
///Usage
///-----
// The following snippets of code illustrate how to create and use a
// 'bdet_DatetimeInterval' object.  First create a default object 'i1'.
//..
//    bdet_DatetimeInterval i1;        assert(  0 == i1.days());
//                                     assert(  0 == i1.hours());
//                                     assert(  0 == i1.minutes());
//                                     assert(  0 == i1.seconds());
//                                     assert(  0 == i1.milliseconds());
//..
// Next set the value of 'i1' to -5 days and then add 16 hours to that value.
//..
//      i1.setTotalDays(-5);
//      i1.addHours(16);               assert( -4 == i1.days());
//                                     assert( -8 == i1.hours());
//                                     assert(  0 == i1.minutes());
//                                     assert(  0 == i1.seconds());
//                                     assert(  0 == i1.milliseconds());
//..
// Then create 'i2' as a copy of 'i1'.
//..
//      bdet_DatetimeInterval i2(i1);  assert( -4 == i2.days());
//                                     assert( -8 == i2.hours());
//                                     assert(  0 == i2.minutes());
//                                     assert(  0 == i2.seconds());
//                                     assert(  0 == i2.milliseconds());
//..
// Now add 2 days and 4 seconds to the value of 'i2' (in two steps), and
// confirm that 'i2' has a value that is greater than that of 'i1'.
//..
//      i2.addDays(2);
//      i2.addSeconds(4);              assert( -2 == i2.date().days());
//                                     assert( -7 == i2.time().hours());
//                                     assert(-59 == i2.time().minutes());
//                                     assert(-56 == i2.time().seconds());
//                                     assert(  0 == i2.time().milliseconds());
//      assert(i2 > i1);
//..
// Then add 2 days and 4 seconds to the value of 'i1' in one step by using the
// 'addInterval' method, and confirm that 'i1' now has the same value as 'i2'.
//..
//      i1.addInterval(2, 0, 0, 4);
//      assert(i2 == i1);
//..
// Finally write the value of 'i2' to 'stdout'.
//..
//      bsl::cout << i2 << bsl::endl;
//..
// The output operator produces the following format on 'stdout':
//..
//     -2_07:59:56.000
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

#ifndef INCLUDED_BDES_PLATFORMUTIL
#include <bdes_platformutil.h>
#endif

#endif

namespace BloombergLP {

                        // ===========================
                        // class bdet_DatetimeInterval
                        // ===========================

class bdet_DatetimeInterval {
    // Each object of this class represents a (signed) time interval.  More
    // generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  (A precise
    // operational definition of when two objects have the same value can be
    // found in the description of 'operator==' for the class.)  This class is
    // *exception* *neutral* with no guarantee of rollback: if an exception is
    // thrown during the invocation of a method on a pre-existing object, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // DATA
    bsls_PlatformUtil::Int64 d_milliseconds;  // interval in (signed) msec.

    // FRIENDS
    friend bdet_DatetimeInterval operator+(const bdet_DatetimeInterval&,
                                           const bdet_DatetimeInterval&);
    friend bdet_DatetimeInterval operator-(const bdet_DatetimeInterval&,
                                           const bdet_DatetimeInterval&);
    friend bdet_DatetimeInterval operator-(const bdet_DatetimeInterval&);
    friend bool operator==(const bdet_DatetimeInterval&,
                           const bdet_DatetimeInterval&);
    friend bool operator!=(const bdet_DatetimeInterval&,
                           const bdet_DatetimeInterval&);
    friend bool operator< (const bdet_DatetimeInterval&,
                           const bdet_DatetimeInterval&);
    friend bool operator<=(const bdet_DatetimeInterval&,
                           const bdet_DatetimeInterval&);
    friend bool operator>=(const bdet_DatetimeInterval&,
                           const bdet_DatetimeInterval&);
    friend bool operator> (const bdet_DatetimeInterval&,
                           const bdet_DatetimeInterval&);

  private:
    // PRIVATE TYPES
    enum {
        BDET_MSEC_PER_SEC  =  1000,
        BDET_MSEC_PER_MIN  =  BDET_MSEC_PER_SEC * 60,
        BDET_MSEC_PER_HR   =  BDET_MSEC_PER_MIN * 60,
        BDET_MSEC_PER_DAY  =  BDET_MSEC_PER_HR  * 24
    };

    // PRIVATE CREATORS
    explicit bdet_DatetimeInterval(bsls_PlatformUtil::Int64 milliseconds);
        // Create a time interval object having the value of the specified
        // 'milliseconds'.  Note that this private creator is an implementation
        // detail of 'operator+' and 'operator-', which would be unduly
        // expensive using the public interface.

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdet_DatetimeInterval,
                                 bslalg_TypeTraitBitwiseCopyable);

    // CLASS METHODS
    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    bdet_DatetimeInterval();
        // Create a time interval object having the value zero.

    explicit bdet_DatetimeInterval(int                      days,
                                   bsls_PlatformUtil::Int64 hours        = 0,
                                   bsls_PlatformUtil::Int64 minutes      = 0,
                                   bsls_PlatformUtil::Int64 seconds      = 0,
                                   bsls_PlatformUtil::Int64 milliseconds = 0);
        // Create a time interval object having the value given by the
        // specified 'days' and the optionally specified 'hours', 'minutes',
        // 'seconds' and 'milliseconds'.  Unspecified fields default to zero.
        // The behavior is undefined unless (1) the resulting time interval
        // value is valid (i.e., the expressed interval must not overflow a
        // 32-bit integer number of days), and (2) every time argument
        // separately can be represented as a *62-bit* integer number of
        // milliseconds.  Note that the latter restriction ensures that an
        // overflow cannot occur in an intermediate sum when computing an
        // otherwise valid result, as may occur, for example, given a very
        // large positive number of hours and a large very negative number of
        // minutes.

    bdet_DatetimeInterval(const bdet_DatetimeInterval& original);
        // Create a time interval object having the value of the specified
        // 'original' time interval.

    //! ~bdet_DatetimeInterval();
        // Destroy this time interval object.  Note that this method's
        // definition is compiler generated.

    // MANIPULATORS
    bdet_DatetimeInterval& operator=(const bdet_DatetimeInterval& rhs);
        // Assign to this object the value of the specified 'rhs' time
        // interval, and return a reference to this modifiable object.

    bdet_DatetimeInterval& operator+=(const bdet_DatetimeInterval& rhs);
        // Add to this time interval the value of the specified 'rhs' time
        // interval and return a reference to this object.

    bdet_DatetimeInterval& operator-=(const bdet_DatetimeInterval& rhs);
        // Subtract from this time interval the value of the specified 'rhs'
        // time interval and return a reference to this object.

    void setInterval(int                      days,
                     bsls_PlatformUtil::Int64 hours        = 0,
                     bsls_PlatformUtil::Int64 minutes      = 0,
                     bsls_PlatformUtil::Int64 seconds      = 0,
                     bsls_PlatformUtil::Int64 milliseconds = 0);
        // Set the field values of this object to the specified 'days'
        // and the optionally specified 'hours', 'minutes', 'seconds', and
        // 'milliseconds' values.  Unspecified fields default to zero.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the 'days' field must not overflow a 32-bit integer),
        // but otherwise each argument may independently have any value that
        // is representable as a *62-bit* integer number of milliseconds.

    void setTotalDays(int days);
        // Set the overall value of this object to indicate the specified
        // number of 'days'.

    void setTotalHours(bsls_PlatformUtil::Int64 hours);
        // Set the overall value of this object to indicate the specified
        // number of 'hours'.  The behavior is undefined unless the resulting
        // time interval value is valid (i.e., the 'days' field must not
        // overflow a 32-bit integer).

    void setTotalMinutes(bsls_PlatformUtil::Int64 minutes);
        // Set the overall value of this object to indicate the specified
        // number of 'minutes'.  The behavior is undefined unless the resulting
        // time interval value is valid (i.e., the 'days' field must not
        // overflow a 32-bit integer).

    void setTotalSeconds(bsls_PlatformUtil::Int64 seconds);
        // Set the overall value of this object to indicate the specified
        // number of 'seconds'.  The behavior is undefined unless the resulting
        // time interval value is valid (i.e., the 'days' field must not
        // overflow a 32-bit integer).

    void setTotalMilliseconds(bsls_PlatformUtil::Int64 milliseconds);
        // Set the overall value of this object to indicate the specified
        // number of 'milliseconds'.  The behavior is undefined unless the
        // resulting time interval value is valid (i.e., the 'days' field must
        // not overflow a 32-bit integer).

    void addInterval(int                      days,
                     bsls_PlatformUtil::Int64 hours        = 0,
                     bsls_PlatformUtil::Int64 minutes      = 0,
                     bsls_PlatformUtil::Int64 seconds      = 0,
                     bsls_PlatformUtil::Int64 milliseconds = 0);
        // Add to this time interval the specified number of 'days', and the
        // optionally specified number of 'hours', 'minutes', 'seconds', and
        // 'milliseconds'.  The behavior is undefined unless the resulting time
        // interval value is valid (i.e., the 'days' field must not overflow a
        // 32-bit integer), but otherwise each argument may independently have
        // any value that is representable as a *62-bit* integer number of
        // milliseconds.

    void addDays(int days);
        // Add to this time interval the specified number of 'days'.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the 'days' field must not overflow a 32-bit integer),
        // but otherwise 'days' may have any valid integer value.

    void addHours(bsls_PlatformUtil::Int64 hours);
        // Add to this time interval the specified number of 'hours'.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the 'days' field must not overflow a 32-bit integer),
        // but otherwise 'hours' may have any value.

    void addMinutes(bsls_PlatformUtil::Int64 minutes);
        // Add to this time interval the specified number of 'minutes'.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the 'days' field must not overflow a 32-bit integer),
        // but otherwise 'minutes' may have any value.

    void addSeconds(bsls_PlatformUtil::Int64 seconds);
        // Add to this time interval the specified number of 'seconds'.  The
        // behavior is undefined unless the resulting time interval value is
        // valid (i.e., the 'days' field must not overflow a 32-bit integer),
        // but otherwise 'seconds' may have any valid integer value.

    void addMilliseconds(bsls_PlatformUtil::Int64 milliseconds);
        // Add to this time interval the specified number of 'milliseconds'.
        // The behavior is undefined unless the resulting time interval value
        // is valid (i.e., the 'days' field must not overflow a 32-bit
        // integer), but otherwise 'milliseconds' may have any valid integer
        // value.

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
    int days() const;
        // Return the days field in the canonical representation of the value
        // of this time interval.

    int hours() const;
        // Return the hours field in the canonical representation of the value
        // of this time interval.

    int minutes() const;
        // Return the minutes field in the canonical representation of the
        // value of this time interval.

    int seconds() const;
        // Return the seconds field in the canonical representation of the
        // value of this time interval.

    int milliseconds() const;
        // Return the milliseconds field in the canonical representation of the
        // value of this time interval.

    int totalDays() const;
        // Return the value of this time interval in integer days, rounded
        // towards zero.  Note that the return value may be negative.

    bsls_PlatformUtil::Int64 totalHours() const;
        // Return the value of this time interval in integer hours, rounded
        // towards zero.  Note that the return value may be negative.

    bsls_PlatformUtil::Int64 totalMinutes() const;
        // Return the value of this time interval in integer minutes, rounded
        // towards zero.  Note that the return value may be negative.

    bsls_PlatformUtil::Int64 totalSeconds() const;
        // Return the value of this time interval in integer seconds, rounded
        // towards zero.  Note that the return value may be negative.

    double totalSecondsAsDouble() const;
        // Return the value of this time interval in seconds as a double,
        // potentially with a fractional part.  Note that the return value
        // may be negative.  Also note that the conversion from the internal
        // representation to double may *lose* precision.

    bsls_PlatformUtil::Int64 totalMilliseconds() const;
        // Return the value of this time interval in integer milliseconds,
        // rounded towards zero.  Note that the return value may be negative.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the
        // modifiable 'stream'.  If 'version' is not supported, 'stream' is
        // unmodified.  Note that 'version' is not written to 'stream'.
        // See the 'bdex' package-level documentation for more information
        // on 'bdex' streaming of value-semantic types and containers.

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: use 'maxSupportedBdexVersion()' instead.

    bsl::ostream& streamOut(bsl::ostream& stream) const;
        // Format this time interval to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
        //
        // DEPRECATED: use 'operator<<' instead.

#endif // BDE_OMIT_INTERNAL_DEPRECATED

};

// FREE OPERATORS
inline
bdet_DatetimeInterval operator+(const bdet_DatetimeInterval& lhs,
                                const bdet_DatetimeInterval& rhs);
    // Return a 'bdetTimeInterval' value that is the sum of the specified 'lhs'
    // and 'rhs' time intervals.

inline
bdet_DatetimeInterval operator-(const bdet_DatetimeInterval& lhs,
                                const bdet_DatetimeInterval& rhs);
    // Return a 'bdetTimeInterval' value that is the difference between the
    // specified 'lhs' and 'rhs' time intervals.

inline
bdet_DatetimeInterval operator-(const bdet_DatetimeInterval& value);
    // Return a 'bdet_DatetimeInterval' value that is the negative of the
    // specified interval 'value'.

inline
bool operator==(const bdet_DatetimeInterval& lhs,
                const bdet_DatetimeInterval& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time intervals have the
    // same value, and 'false' otherwise.  Two time intervals have the same
    // value if they have the same days, hours, minutes, seconds, and
    // milliseconds field values.

inline
bool operator!=(const bdet_DatetimeInterval& lhs,
                const bdet_DatetimeInterval& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time intervals do not
    // have the same value, and 'false' otherwise.  Two time intervals differ
    // in value if they differ in one or more of their respective field values.

inline
bool operator< (const bdet_DatetimeInterval& lhs,
                const bdet_DatetimeInterval& rhs);
inline
bool operator<=(const bdet_DatetimeInterval& lhs,
                const bdet_DatetimeInterval& rhs);
inline
bool operator> (const bdet_DatetimeInterval& lhs,
                const bdet_DatetimeInterval& rhs);
inline
bool operator>=(const bdet_DatetimeInterval& lhs,
                const bdet_DatetimeInterval& rhs);
    // Return 'true' if the nominal relation between the specified 'lhs' and
    // 'rhs' time interval values holds, and 'false' otherwise.  'lhs' is less
    // than 'rhs' if the following expression evaluates to logical true:
    //..
    //      lhs.days()    <  rhs.days()
    //   || lhs.days()    == rhs.days()    && lhs.hours()   < rhs.hours()
    //   || lhs.hours()   == rhs.hours()   && lhs.minutes() < rhs.minutes()
    //   || lhs.minutes() == rhs.minutes() && lhs.seconds() < rhs.seconds()
    //   || lhs.seconds() == rhs.seconds() && lhs.milliseconds()
    //                                                     < rhs.milliseconds()
    //..
    // The other relationships are defined similarly.

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const bdet_DatetimeInterval& timeInterval);
    // Write the specified 'timeInterval' value to the specified output
    // 'stream' in the following format: "sD...D_HH:MM:SS.SSS" where 's' is a
    // sign character ('+' or '-'), and return a reference to the modifiable
    // 'stream'.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

// PRIVATE CREATORS
inline
bdet_DatetimeInterval::bdet_DatetimeInterval(
                                         bsls_PlatformUtil::Int64 milliseconds)
: d_milliseconds(milliseconds)
{
}

// CLASS METHODS
inline
int bdet_DatetimeInterval::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
inline
bdet_DatetimeInterval::bdet_DatetimeInterval()
: d_milliseconds(0)
{
}

inline
bdet_DatetimeInterval::bdet_DatetimeInterval(
                                         int                      days,
                                         bsls_PlatformUtil::Int64 hours,
                                         bsls_PlatformUtil::Int64 minutes,
                                         bsls_PlatformUtil::Int64 seconds,
                                         bsls_PlatformUtil::Int64 milliseconds)
: d_milliseconds(milliseconds
               + seconds * BDET_MSEC_PER_SEC
               + minutes * BDET_MSEC_PER_MIN
               + hours   * BDET_MSEC_PER_HR
               + (bsls_PlatformUtil::Int64)days * BDET_MSEC_PER_DAY)
{
}

inline
bdet_DatetimeInterval::bdet_DatetimeInterval(
                                         const bdet_DatetimeInterval& original)
: d_milliseconds(original.d_milliseconds)
{
}

// MANIPULATORS
inline
bdet_DatetimeInterval&
bdet_DatetimeInterval::operator=(const bdet_DatetimeInterval& rhs)
{
    d_milliseconds = rhs.d_milliseconds;
    return *this;
}

inline
bdet_DatetimeInterval&
bdet_DatetimeInterval::operator+=(const bdet_DatetimeInterval& rhs)
{
    d_milliseconds += rhs.d_milliseconds;
    return *this;
}

inline
bdet_DatetimeInterval&
bdet_DatetimeInterval::operator-=(const bdet_DatetimeInterval& rhs)
{
    d_milliseconds -= rhs.d_milliseconds;
    return *this;
}

inline
void bdet_DatetimeInterval::setInterval(int                      days,
                                        bsls_PlatformUtil::Int64 hours,
                                        bsls_PlatformUtil::Int64 minutes,
                                        bsls_PlatformUtil::Int64 seconds,
                                        bsls_PlatformUtil::Int64 milliseconds)
{
    d_milliseconds = milliseconds
                   + seconds * BDET_MSEC_PER_SEC
                   + minutes * BDET_MSEC_PER_MIN
                   + hours   * BDET_MSEC_PER_HR
                   + (bsls_PlatformUtil::Int64)days * BDET_MSEC_PER_DAY;
}

inline
void bdet_DatetimeInterval::setTotalDays(int days)
{
    d_milliseconds = (bsls_PlatformUtil::Int64)days * BDET_MSEC_PER_DAY;
}

inline
void bdet_DatetimeInterval::setTotalHours(bsls_PlatformUtil::Int64 hours)
{
    d_milliseconds = hours * BDET_MSEC_PER_HR;
}

inline
void bdet_DatetimeInterval::setTotalMinutes(bsls_PlatformUtil::Int64 minutes)
{
    d_milliseconds = minutes * BDET_MSEC_PER_MIN;
}

inline
void bdet_DatetimeInterval::setTotalSeconds(bsls_PlatformUtil::Int64 seconds)
{
    d_milliseconds = seconds * BDET_MSEC_PER_SEC;
}

inline
void bdet_DatetimeInterval::setTotalMilliseconds(
                                         bsls_PlatformUtil::Int64 milliseconds)
{
    d_milliseconds = milliseconds;
}

inline
void bdet_DatetimeInterval::addInterval(int                      days,
                                        bsls_PlatformUtil::Int64 hours,
                                        bsls_PlatformUtil::Int64 minutes,
                                        bsls_PlatformUtil::Int64 seconds,
                                        bsls_PlatformUtil::Int64 milliseconds)
{
    d_milliseconds += milliseconds
                    + seconds * BDET_MSEC_PER_SEC
                    + minutes * BDET_MSEC_PER_MIN
                    + hours   * BDET_MSEC_PER_HR
                    + (bsls_PlatformUtil::Int64)days * BDET_MSEC_PER_DAY;
}

inline
void bdet_DatetimeInterval::addDays(int days)
{
    d_milliseconds += (bsls_PlatformUtil::Int64)days * BDET_MSEC_PER_DAY;
}

inline
void bdet_DatetimeInterval::addHours(bsls_PlatformUtil::Int64 hours)
{
    d_milliseconds += hours * BDET_MSEC_PER_HR;
}

inline
void bdet_DatetimeInterval::addMinutes(bsls_PlatformUtil::Int64 minutes)
{
    d_milliseconds += minutes * BDET_MSEC_PER_MIN;
}

inline
void bdet_DatetimeInterval::addSeconds(bsls_PlatformUtil::Int64 seconds)
{
    d_milliseconds += seconds * BDET_MSEC_PER_SEC;
}

inline
void
bdet_DatetimeInterval::addMilliseconds(bsls_PlatformUtil::Int64 milliseconds)
{
    d_milliseconds += milliseconds;
}

template <class STREAM>
STREAM& bdet_DatetimeInterval::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the version
          case 1: {
            stream.getInt64(d_milliseconds);
            if (!stream) {
                return stream;                                        // RETURN
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
int bdet_DatetimeInterval::days() const
{
    return (int)(d_milliseconds / BDET_MSEC_PER_DAY);
}

inline
int bdet_DatetimeInterval::hours() const
{
    return (int)(d_milliseconds % BDET_MSEC_PER_DAY / BDET_MSEC_PER_HR);
}

inline
int bdet_DatetimeInterval::minutes() const
{
    return (int)(d_milliseconds % BDET_MSEC_PER_HR / BDET_MSEC_PER_MIN);
}

inline
int bdet_DatetimeInterval::seconds() const
{
    return (int)(d_milliseconds % BDET_MSEC_PER_MIN / BDET_MSEC_PER_SEC);
}

inline
int bdet_DatetimeInterval::milliseconds() const
{
    return (int)(d_milliseconds % BDET_MSEC_PER_SEC);
}

inline
int bdet_DatetimeInterval::totalDays() const
{
    return (int)(d_milliseconds / BDET_MSEC_PER_DAY);
}

inline
bsls_PlatformUtil::Int64 bdet_DatetimeInterval::totalHours() const
{
    return d_milliseconds / BDET_MSEC_PER_HR;
}

inline
bsls_PlatformUtil::Int64 bdet_DatetimeInterval::totalMinutes() const
{
    return d_milliseconds / BDET_MSEC_PER_MIN;
}

inline
bsls_PlatformUtil::Int64 bdet_DatetimeInterval::totalSeconds() const
{
    return d_milliseconds / BDET_MSEC_PER_SEC;
}

inline
double bdet_DatetimeInterval::totalSecondsAsDouble() const
{
    return (double)d_milliseconds / (1.0 * BDET_MSEC_PER_SEC);
}

inline
bsls_PlatformUtil::Int64 bdet_DatetimeInterval::totalMilliseconds() const
{
    return d_milliseconds;
}

template <class STREAM>
STREAM& bdet_DatetimeInterval::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putInt64(d_milliseconds);
      } break;
    }
    return stream;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

inline
int bdet_DatetimeInterval::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

inline
bsl::ostream& bdet_DatetimeInterval::streamOut(bsl::ostream& stream) const
{
    return stream << *this;
}

#endif // BDE_OMIT_INTERNAL_DEPRECATED

// FREE OPERATORS
inline
bdet_DatetimeInterval operator+(const bdet_DatetimeInterval& lhs,
                                const bdet_DatetimeInterval& rhs)
{
    return bdet_DatetimeInterval(lhs.d_milliseconds + rhs.d_milliseconds);
}

inline
bdet_DatetimeInterval operator-(const bdet_DatetimeInterval& lhs,
                                const bdet_DatetimeInterval& rhs)
{
    return bdet_DatetimeInterval(lhs.d_milliseconds - rhs.d_milliseconds);
}

inline
bdet_DatetimeInterval operator-(const bdet_DatetimeInterval& value)
{
    return bdet_DatetimeInterval(-value.d_milliseconds);
}

inline
bool operator< (const bdet_DatetimeInterval& lhs,
                const bdet_DatetimeInterval& rhs)
{
    return lhs.d_milliseconds < rhs.d_milliseconds;
}

inline
bool operator<=(const bdet_DatetimeInterval& lhs,
                const bdet_DatetimeInterval& rhs)
{
    return lhs.d_milliseconds <= rhs.d_milliseconds;
}

inline
bool operator==(const bdet_DatetimeInterval& lhs,
                const bdet_DatetimeInterval& rhs)
{
    return lhs.d_milliseconds == rhs.d_milliseconds;
}

inline
bool operator!=(const bdet_DatetimeInterval& lhs,
                const bdet_DatetimeInterval& rhs)
{
    return lhs.d_milliseconds != rhs.d_milliseconds;
}

inline
bool operator> (const bdet_DatetimeInterval& lhs,
                const bdet_DatetimeInterval& rhs)
{
    return lhs.d_milliseconds > rhs.d_milliseconds;
}

inline
bool operator>=(const bdet_DatetimeInterval& lhs,
                const bdet_DatetimeInterval& rhs)
{
    return lhs.d_milliseconds >= rhs.d_milliseconds;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
