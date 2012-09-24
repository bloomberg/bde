// bdet_time.h                                                        -*-C++-*-
#ifndef INCLUDED_BDET_TIME
#define INCLUDED_BDET_TIME

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a value-semantic time-of-day type (millisecond resolution).
//
//@CLASSES:
//  bdet_Time: time-of-day type 'hh:mm:ss.sss' (having millisecond resolution)
//
//@AUTHOR: John Lakos (jlakos)
//
//@DESCRIPTION: This component implements a fully value-semantic time class,
// 'bdet_Time', that can represent the time of day to a resolution of one
// millisecond (using a 24-hour clock).  Valid time values range from
// "00:00:00.000" (i.e., midnight) through "23:59:59.999".  A time value
// can be specified via four separate integer field values denoting
// hours '[ 0 .. 23 ]', minutes '[ 0 .. 59 ]', seconds '[ 0 .. 59 ]', and
// milliseconds '[ 0 ..  999 ]'.  In addition, the 'bdet_Time' type has one
// more valid value, "24:00:00.000", which can be set explicitly and accessed,
// but behaves as if it were the value "00:00:00.000" with respect to all
// manipulators.  However, for all relational comparison operators,
// "24:00:00.000" is not a valid argument and results in undefined behavior.
// Each of the 'add' manipulators, along with modifying the value of the
// object, return the (signed) number of times that the
// "23:59:59.999" - "00:00:00.000" boundary was crossed in performing the
// addition.
//
///Usage
///-----
// The following snippets of code illustrate how to create and use a
// 'bdet_Time' object.  First we create a default-constructed object 't1':
//..
//  bdet_Time t1;                assert(24 == t1.hour());
//                               assert( 0 == t1.minute());
//                               assert( 0 == t1.second());
//                               assert( 0 == t1.millisecond());
//..
// Then we set 't1' to the value 2:34pm (14:34:00.000):
//..
//  t1.setTime(14, 34);          assert(14 == t1.hour());
//                               assert(34 == t1.minute());
//                               assert( 0 == t1.second());
//                               assert( 0 == t1.millisecond());
//..
// We can also use 'setTimeIfValid' if we are not sure whether the particular
// time we want to set to is a valid 'bdet_Time' value.  For example, if we
// want to set the time to 0:15am, but we mistakenly used 24 as the hour
// instead of 0:
//..
//  int ret = t1.setTimeIfValid(24, 15);
//                               assert( 0 != ret);          // "24:15" is not
//                                                           // valid
//
//                               assert(14 == t1.hour());    // no effect
//                               assert(34 == t1.minute());  // on the
//                               assert( 0 == t1.second());  // object
//                               assert( 0 == t1.millisecond());
//..
// Next we copy-construct 't2' from 't1':
//..
//  bdet_Time t2(t1);            assert(t1 == t2);
//..
// We can also add time to an existing time object:
//..
//  t2.addMinutes(5);
//  t2.addSeconds(7);
//                               assert(14 == t2.hour());
//                               assert(39 == t2.minute());
//                               assert( 7 == t2.second());
//                               assert( 0 == t2.millisecond());
//..
// Subtracting two 'bdet_Time' objects, 't1' from 't2', will yield a
// 'bdet_DatetimeInterval':
//..
//  bdet_DatetimeInterval dt = t2 - t1;
//                               assert(307 == dt.totalSeconds());
//..
// Finally we stream the value of 't2' to 'stdout':
//..
//  bsl::cout << t2 << bsl::endl;
//..
// The streaming operator produces the following output on 'stdout':
//..
//  14:39:07.000
//..

#ifndef INCLUDED_BDESCM_VERSION
#include <bdescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATETIMEINTERVAL
#include <bdet_datetimeinterval.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITBITWISECOPYABLE
#include <bslalg_typetraitbitwisecopyable.h>
#endif

#ifndef INCLUDED_BSLALG_TYPETRAITS
#include <bslalg_typetraits.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>  // TBD DEPRECATED
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                        // ===============
                        // class bdet_Time
                        // ===============

class bdet_Time {
    // This class implements a fully value semantic type that represents the
    // time of day to a resolution of one millisecond.  Each object of this
    // (almost) simply constrained attribute class *always* represents a valid
    // time value to a resolution of one millisecond.  The valid range for
    // times is "00:00:00.000" through "23:59:59.999", except that
    // "24:00:00.000" represents the default-constructed value.  The value
    // "24:00:00.000" behaves, in most cases, as if it were the value
    // "00:00:00.000"; however, for all relational comparison operators,
    // "24:00:00.000" is not a valid argument and, therefore, would results in
    // undefined behavior.  Each add operation on a 'bdet_Time' object will
    // return the (signed) number of times that the
    // "23:59:59.999" - "00:00:00.000" boundary was crossed while performing
    // the operation.  Attempting to construct a 'bdet_Time' with any field
    // outside its valid range (or with an hour field value of 24, and any
    // other field non-zero) will result in undefined behavior.
    //
    // More generally, this class supports a complete set of *value* *semantic*
    // operations, including copy construction, assignment, equality
    // comparison, 'ostream' printing, and 'bdex' serialization.  A precise
    // operational definition of when two objects have the same value can be
    // found in the description of 'operator==' for the class.  This class is
    // *exception* *neutral* with no guarantee of rollback: If an exception is
    // thrown during the invocation of a method on a pre-existing object, the
    // object is left in a valid state, but its value is undefined.  In no
    // event is memory leaked.  Finally, *aliasing* (e.g., using all or part of
    // an object as both source and destination) is supported in all cases.

    // PRIVATE TYPES
    enum {
        BDET_HOURS_PER_DAY           = 24,

        BDET_MINUTES_PER_HOUR        = 60,

        BDET_SECONDS_PER_MINUTE      = 60,

        BDET_MILLISECONDS_PER_SECOND = 1000,

        BDET_MINUTES_PER_DAY         = BDET_HOURS_PER_DAY
                                     * BDET_MINUTES_PER_HOUR,

        BDET_SECONDS_PER_HOUR        = BDET_MINUTES_PER_HOUR
                                     * BDET_SECONDS_PER_MINUTE,

        BDET_SECONDS_PER_DAY         = BDET_HOURS_PER_DAY
                                     * BDET_SECONDS_PER_HOUR,

        BDET_MILLISECONDS_PER_MINUTE = BDET_MILLISECONDS_PER_SECOND
                                     * BDET_SECONDS_PER_MINUTE,

        BDET_MILLISECONDS_PER_HOUR   = BDET_MILLISECONDS_PER_MINUTE
                                     * BDET_MINUTES_PER_HOUR,

        BDET_MILLISECONDS_PER_DAY    = BDET_MILLISECONDS_PER_HOUR
                                     * BDET_HOURS_PER_DAY
    };

    // DATA
    int d_milliseconds;  // offset from "00:00:00.000"; 86,400 is the default

    // FRIENDS
    friend bdet_DatetimeInterval operator-(const bdet_Time&, const bdet_Time&);
    friend bool operator==(const bdet_Time&, const bdet_Time&);
    friend bool operator!=(const bdet_Time&, const bdet_Time&);
    friend bool operator< (const bdet_Time&, const bdet_Time&);
    friend bool operator<=(const bdet_Time&, const bdet_Time&);
    friend bool operator>=(const bdet_Time&, const bdet_Time&);
    friend bool operator> (const bdet_Time&, const bdet_Time&);

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdet_Time, bslalg_TypeTraitBitwiseCopyable);

    // CLASS METHODS
    static bool isValid(int hour,
                        int minute      = 0,
                        int second      = 0,
                        int millisecond = 0);
        // Return 'true' if the specified 'hour', and the optionally specified
        // 'minute', 'second', and 'millisecond', represent a valid 'bdet_Time'
        // value, and 'false' otherwise.  Unspecified arguments default to 0.
        // The 'hour', 'minute', 'second', and 'millisecond' fields comprise a
        // valid 'bdet_Time' value if '0 <= hour < 24', '0 <= minute < 60',
        // '0 <= second < 60', and '0 <= millisecond < 1000'.  Additionally,
        // "24:00:00.000" also represents a valid 'bdet_Time' value.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)

    // CREATORS
    bdet_Time();
        // Create a 'bdet_Time' object having the value "24:00:00.000".

    explicit
    bdet_Time(int hour, int minute = 0, int second = 0, int millisecond = 0);
        // Create a 'bdet_Time' object having the (valid) value represented by
        // the specified 'hour', and the optionally specified 'minute',
        // 'second', and 'millisecond'.  Unspecified arguments default to 0.
        // The behavior is undefined unless the arguments comprise a valid
        // 'bdet_Time' value (see 'isValid').

    bdet_Time(const bdet_Time& original);
        // Create a 'bdet_Time' object having the value of the specified
        // 'original' time.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    ~bdet_Time();
        // Destroy this time object.  Note that this trivial destructor is, in
        // some build modes, generated by the compiler.
#endif

    // MANIPULATORS
    bdet_Time& operator=(const bdet_Time& rhs);
        // Assign to this time object the value of the specified 'rhs' object,
        // and return a reference to this modifiable time.

    bdet_Time& operator+=(const bdet_DatetimeInterval& rhs);
        // Add to this time object the value of the specified 'rhs' datetime
        // interval, and return a reference to this modifiable time.  The
        // behavior is undefined unless the resulting time represents a valid
        // 'bdet_Time' value (see 'isValid').

    bdet_Time& operator-=(const bdet_DatetimeInterval& rhs);
        // Subtract from this time object the value of the specified 'rhs'
        // datetime interval, and return a reference to this modifiable time.
        // The behavior is undefined unless the resulting time represents a
        // valid 'bdet_Time' value (see 'isValid').

    void setTime(int hour,
                 int minute      = 0,
                 int second      = 0,
                 int millisecond = 0);
        // Set the value of this time object to the specified 'hour', and the
        // optionally specified 'minute', 'second', and 'millisecond'.
        // Unspecified arguments default to 0.  The behavior is undefined if
        // any of the specified values, or the resulting time value, is outside
        // its valid range (see 'isValid').

    int setTimeIfValid(int hour,
                       int minute      = 0,
                       int second      = 0,
                       int millisecond = 0);
        // Set the value of this time object to the specified 'hour', and the
        // optionally specified 'minute', 'second', and 'millisecond', if they
        // would comprise a valid 'bdet_Time' value (see 'isValid').  Return 0
        // on success, and a non-zero value (with no effect) otherwise.
        // Unspecified arguments default to 0.

    void setHour(int hour);
        // Modify the value of this time object to have the specified 'hour'
        // field; if 'hour' is 24, set remaining fields of this object to 0.
        // The behavior is undefined unless '0 <= hour <= 24'.

    void setMinute(int minute);
        // Modify the value of this time object to have the specified 'minute'
        // field; if the "hour" field of this object is currently 24, set it to
        // 0.  The behavior is undefined unless '0 <= minute <= 59'.

    void setSecond(int second);
        // Modify the value of this time object to have the specified 'second'
        // field; if the "hour" field of this object is currently 24, set it to
        // 0.  The behavior is undefined unless '0 <= second <= 59'.

    void setMillisecond(int millisecond);
        // Modify the value of this time object to have the specified
        // 'millisecond' field; if the "hour" field of this object is currently
        // 24, set it to 0.  The behavior is undefined unless
        // '0 <= millisecond <= 999'.

    int addTime(int hours,
                int minutes      = 0,
                int seconds      = 0,
                int milliseconds = 0);
        // Add to the value of this time object the specified (signed) number
        // of 'hours' and optionally specified (signed) numbers of 'minutes',
        // 'seconds', and 'milliseconds'; return the (signed) number of times
        // that the "23:59:59.999" - "00:00:00.000" boundary was crossed in
        // performing the operation.  Unspecified arguments default to 0.

    int addInterval(const bdet_DatetimeInterval& interval);
        // Increase the value of this time object by the specified 'interval'
        // of time, and return the (signed) number of times that the
        // "23:59:59.999" - "00:00:00.000" boundary was crossed in performing
        // the operation.  The behavior is undefined unless the number of
        // crossings returned can be represented by an 'int'.

    int addHours(int hours);
        // Increase the value of this time object by the specified number of
        // 'hours', and return the (signed) number of times that the
        // "23:59:59.999" - "00:00:00.000" boundary was crossed in performing
        // the operation.

    int addMinutes(int minutes);
        // Increase the value of this time object by the specified number of
        // 'minutes', and return the (signed) number of times that the
        // "23:59:59.999" - "00:00:00.000" boundary was crossed in performing
        // the operation.

    int addSeconds(int seconds);
        // Increase the value of this time object by the specified number of
        // 'seconds', and return the (signed) number of times that the
        // "23:59:59.999" - "00:00:00.000" boundary was crossed in performing
        // the operation.

    int addMilliseconds(int milliseconds);
        // Increase the value of this time object by the specified number of
        // 'milliseconds', and return the (signed) number of times that the
        // "23:59:59.999" - "00:00:00.000" boundary was crossed in performing
        // the operation.

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
    void getTime(int *hour,
                 int *minute = 0,
                 int *second = 0,
                 int *millisecond = 0) const;
        // Load, into the specified 'hour', and the optionally specified
        // 'minute', 'second' and 'millisecond', the representation of the
        // value of this time object.  Unspecified arguments default to 0.
        // Supplying 0 for an address argument suppresses the loading of the
        // value for the corresponding field, but has no effect on the loading
        // of other field values.

    int hour() const;
        // Return the value of the "hour" field of this time object.

    int minute() const;
        // Return the value of the "minute" field of this time object.

    int second() const;
        // Return the value of the "second" field of this time object.

    int millisecond() const;
        // Return the value of the "millisecond" field of this time object.

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
         // valid on entry, this operation has no effect.  Note that this
         // human-readable format is not fully specified, and can change
         // without notice.

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format, and return a reference to the modifiable
        // 'stream'.  If 'stream' is initially invalid, this operation has no
        // effect.  If 'version' is not supported, 'stream' is invalidated.
        // Note that in no event is 'version' written to 'stream'.  (See the
        // 'bdex' package-level documentation for more information on 'bdex'
        // streaming of value-semantic types and containers.)

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the package-group-level documentation for more
        // information on 'bdex' streaming of container types.)
        //
        // DEPRECATED: Use 'maxSupportedBdexVersion' instead.

    int validateAndSetTime(int hour,
                           int minute      = 0,
                           int second      = 0,
                           int millisecond = 0);
        // Set the value of this time object to the specified 'hour', and the
        // optionally specified 'minute', 'second', and 'millisecond', if they
        // would comprise a valid 'bdet_Time' value (see 'isValid').  Return 0
        // on success, and a non-zero value (with no effect) otherwise.
        // Unspecified arguments default to 0.
        //
        // DEPRECATED: Use 'setTimeIfValid' instead.

    bsl::ostream& streamOut(bsl::ostream& stream) const;
        // Format this time to the specified output 'stream', and return a
        // reference to the modifiable 'stream'.
        //
        // DEPRECATED: Use 'print' instead.

#endif // BDE_OMIT_INTERNAL_DEPRECATED

};

// FREE OPERATORS
bdet_Time operator+(const bdet_Time&             lhs,
                    const bdet_DatetimeInterval& rhs);
    // Return a 'bdet_Time' value that is the sum of the specified 'lhs'
    // time and the specified 'rhs' datetime interval.

bdet_Time operator+(const bdet_DatetimeInterval& lhs,
                    const bdet_Time&             rhs);
    // Return a 'bdet_Time' value that is the sum of the specified 'lhs'
    // datetime interval and the specified 'rhs' time.

bdet_Time operator-(const bdet_Time&             lhs,
                    const bdet_DatetimeInterval& rhs);
    // Return a 'bdet_Time' value that is the difference between the
    // specified 'lhs' time and the specified 'rhs' datetime interval.

bdet_DatetimeInterval operator-(const bdet_Time& lhs,
                                const bdet_Time& rhs);
    // Return a 'bdet_DatetimeInterval' object initialized with the difference
    // between the specified 'lhs' and 'rhs' time values.  The value is
    // undefined if 'lhs' or 'rhs' have the (default) value "24:00:00.000".

bool operator==(const bdet_Time& lhs, const bdet_Time& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time objects have the
    // same value, and 'false' otherwise.  Two time objects have the same value
    // if each of their corresponding "hour", "minute", "second", and
    // "millisecond" fields respectively have the same value.

bool operator!=(const bdet_Time& lhs, const bdet_Time& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time objects do not have
    // the same value, and 'false' otherwise.  Two time objects do not have the
    // same value if any of their corresponding "hour", "minute", "second", and
    // "millisecond" fields respectively do not have the same value.

bool operator<(const bdet_Time& lhs, const bdet_Time& rhs);
    // Return 'true' if the specified 'lhs' time value is less than the
    // specified 'rhs' time value, and 'false' otherwise.  The behavior is
    // undefined if 'lhs' or 'rhs' has the value "24:00:00.000".

bool operator<=(const bdet_Time& lhs, const bdet_Time& rhs);
    // Return 'true' if the specified 'lhs' time value is less than or equal to
    // the specified 'rhs' time value, and 'false' otherwise.  The behavior is
    // undefined if 'lhs' or 'rhs' has the value "24:00:00.000".

bool operator>(const bdet_Time& lhs, const bdet_Time& rhs);
    // Return 'true' if the specified 'lhs' time value is greater than the
    // specified 'rhs' time value, and 'false' otherwise.  The behavior is
    // undefined if 'lhs' or 'rhs' has the value "24:00:00.000".

bool operator>=(const bdet_Time& lhs, const bdet_Time& rhs);
    // Return 'true' if the specified 'lhs' time value is greater than or equal
    // to the specified 'rhs' time value, and 'false' otherwise.  The behavior
    // is undefined if 'lhs' or 'rhs' has the value "24:00:00.000".

bsl::ostream&  operator<<(bsl::ostream& stream, const bdet_Time& time);
    // Write the value of the specified 'time' object to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified, can change
    // without notice, and is logically equivalent to:
    //..
    //  print(stream, 0, -1);
    //..

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                        // ---------------
                        // class bdet_Time
                        // ---------------

// CLASS METHODS
inline
bool bdet_Time::isValid(int hour, int minute, int second, int millisecond)
{
    return (0 <= hour        &&   24 > hour     &&
            0 <= minute      &&   60 > minute   &&
            0 <= second      &&   60 > second   &&
            0 <= millisecond && 1000 > millisecond)
        || (24 == hour && 0 == minute && 0 == second && 0 == millisecond);
}

inline
int bdet_Time::maxSupportedBdexVersion()
{
    return 1;
}

// CREATORS
inline
bdet_Time::bdet_Time()
: d_milliseconds(BDET_MILLISECONDS_PER_DAY)
{
}

inline
bdet_Time::bdet_Time(int hour, int minute, int second, int millisecond)
{
    BSLS_ASSERT_SAFE(isValid(hour, minute, second, millisecond));

    d_milliseconds = millisecond + BDET_MILLISECONDS_PER_SECOND *
                         (second + BDET_SECONDS_PER_MINUTE      *
                         (minute + BDET_MINUTES_PER_HOUR        * hour));
}

inline
bdet_Time::bdet_Time(const bdet_Time& original)
: d_milliseconds(original.d_milliseconds)
{
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)

inline
bdet_Time::~bdet_Time()
{
    BSLS_ASSERT_SAFE(0 <= d_milliseconds);
    BSLS_ASSERT_SAFE(     d_milliseconds <= BDET_MILLISECONDS_PER_DAY);
}

#endif

// MANIPULATORS
inline
bdet_Time& bdet_Time::operator=(const bdet_Time& rhs)
{
    d_milliseconds = rhs.d_milliseconds;
    return *this;
}

inline
bdet_Time& bdet_Time::operator+=(const bdet_DatetimeInterval& rhs)
{
    // IMPLEMENTATION NOTE: Although 'totalMilliseconds' is an 'Int64', a value
    // requiring more than 32-bits would guarantee that this object's value
    // would become invalid, which falls into undefined behavior.  Hence, the
    // cast is safe.

    addMilliseconds(static_cast<int>(rhs.totalMilliseconds()));
    return *this;
}

inline
bdet_Time& bdet_Time::operator-=(const bdet_DatetimeInterval& rhs)
{
    // IMPLEMENTATION NOTE: as above.

    addMilliseconds(static_cast<int>(-rhs.totalMilliseconds()));
    return *this;
}

inline
int bdet_Time::setTimeIfValid(int hour,
                              int minute,
                              int second,
                              int millisecond)
{
    enum { BDET_SUCCESS = 0, BDET_FAILURE = -1 };

    if (isValid(hour, minute, second, millisecond)) {
        setTime(hour, minute, second, millisecond);
        return BDET_SUCCESS;                                          // RETURN
    }

    return BDET_FAILURE;
}

template <class STREAM>
STREAM& bdet_Time::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) { // switch on the schema version
          case 1: {
            int tmp;
            stream.getInt32(tmp);

            if (!stream
             || static_cast<unsigned>(tmp) > BDET_MILLISECONDS_PER_DAY) {
                stream.invalidate();
            }
            else {
                d_milliseconds = tmp;
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
int bdet_Time::hour() const
{
    return d_milliseconds / BDET_MILLISECONDS_PER_HOUR;
}

inline
int bdet_Time::minute() const
{
    return d_milliseconds % BDET_MILLISECONDS_PER_HOUR
                          / BDET_MILLISECONDS_PER_MINUTE;
}

inline
int bdet_Time::second() const
{
    return d_milliseconds % BDET_MILLISECONDS_PER_MINUTE
                          / BDET_MILLISECONDS_PER_SECOND;
}

inline
int bdet_Time::millisecond() const
{
    return d_milliseconds % BDET_MILLISECONDS_PER_SECOND;
}

template <class STREAM>
STREAM& bdet_Time::bdexStreamOut(STREAM& stream, int version) const
{
    if (stream) {
        switch (version) {
          case 1: {
            stream.putInt32(d_milliseconds);
          } break;
          default: {
            stream.invalidate();  // unrecognized version number
          }
        }
    }
    return stream;
}

#ifndef BDE_OMIT_INTERNAL_DEPRECATED

inline
int bdet_Time::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

inline
int bdet_Time::validateAndSetTime(int hour,
                                  int minute,
                                  int second,
                                  int millisecond)
{
    return setTimeIfValid(hour, minute, second, millisecond);
}

inline
bsl::ostream& bdet_Time::streamOut(bsl::ostream& stream) const
{
    return stream << *this;
}

#endif // BDE_OMIT_INTERNAL_DEPRECATED

// FREE OPERATORS
inline
bdet_Time operator+(const bdet_Time& lhs, const bdet_DatetimeInterval& rhs)
{
    bdet_Time result(lhs);
    return result += rhs;
}

inline
bdet_Time operator+(const bdet_DatetimeInterval& lhs, const bdet_Time& rhs)
{
    bdet_Time result(rhs);
    return result += lhs;
}

inline
bdet_Time operator-(const bdet_Time& lhs, const bdet_DatetimeInterval& rhs)
{
    bdet_Time result(lhs);
    return result -= rhs;
}

inline
bdet_DatetimeInterval operator-(const bdet_Time& lhs, const bdet_Time& rhs)
{
    bdet_DatetimeInterval timeInterval;
    timeInterval.setTotalMilliseconds(
                    lhs.d_milliseconds % bdet_Time::BDET_MILLISECONDS_PER_DAY
                  - rhs.d_milliseconds % bdet_Time::BDET_MILLISECONDS_PER_DAY);
    return timeInterval;
}

inline
bool operator==(const bdet_Time& lhs, const bdet_Time& rhs)
{
    return lhs.d_milliseconds == rhs.d_milliseconds;
}

inline
bool operator!=(const bdet_Time& lhs, const bdet_Time& rhs)
{
    return lhs.d_milliseconds != rhs.d_milliseconds;
}

inline
bool operator<(const bdet_Time& lhs, const bdet_Time& rhs)
{
    BSLS_ASSERT_SAFE(bdet_Time::BDET_MILLISECONDS_PER_DAY !=
                                                           lhs.d_milliseconds);
    BSLS_ASSERT_SAFE(bdet_Time::BDET_MILLISECONDS_PER_DAY !=
                                                           rhs.d_milliseconds);

    return lhs.d_milliseconds < rhs.d_milliseconds;
}

inline
bool operator<=(const bdet_Time& lhs, const bdet_Time& rhs)
{
    BSLS_ASSERT_SAFE(bdet_Time::BDET_MILLISECONDS_PER_DAY !=
                                                           lhs.d_milliseconds);
    BSLS_ASSERT_SAFE(bdet_Time::BDET_MILLISECONDS_PER_DAY !=
                                                           rhs.d_milliseconds);

    return lhs.d_milliseconds <= rhs.d_milliseconds;
}

inline
bool operator>(const bdet_Time& lhs, const bdet_Time& rhs)
{
    BSLS_ASSERT_SAFE(bdet_Time::BDET_MILLISECONDS_PER_DAY !=
                                                           lhs.d_milliseconds);
    BSLS_ASSERT_SAFE(bdet_Time::BDET_MILLISECONDS_PER_DAY !=
                                                           rhs.d_milliseconds);

    return lhs.d_milliseconds > rhs.d_milliseconds;
}

inline
bool operator>=(const bdet_Time& lhs, const bdet_Time& rhs)
{
    BSLS_ASSERT_SAFE(bdet_Time::BDET_MILLISECONDS_PER_DAY !=
                                                           lhs.d_milliseconds);
    BSLS_ASSERT_SAFE(bdet_Time::BDET_MILLISECONDS_PER_DAY !=
                                                           rhs.d_milliseconds);

    return lhs.d_milliseconds >= rhs.d_milliseconds;
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
