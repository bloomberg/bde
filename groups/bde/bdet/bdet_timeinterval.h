// bdet_timeinterval.h            -*-C++-*-
#ifndef INCLUDED_BDET_TIMEINTERVAL
#define INCLUDED_BDET_TIMEINTERVAL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a representation of a time interval.
//
//@CLASSES:
//   bdet_TimeInterval: time interval with nanosecond resolution
//
//@SEE_ALSO: bdet_time, bdet_datetimeinterval
//
//@AUTHOR: Xinyu Xiang (xxiang)
//
//@DESCRIPTION: This component provides a time interval class,
// 'bdet_TimeInterval', that is capable of representing a signed time interval
// with nanosecond resolution.
//
///Representation
///--------------
// A time interval has a value that is independent of its representation.
// Conceptually, a time interval may be thought of as a signed,
// arbitrary-precision floating-point number denominated in seconds (or in
// days, or in fortnights, if one prefers).  A 'bdet_TimeInterval' represents
// this value as two fields: seconds and nanoseconds.  In the "canonical
// representation" of a time interval, the 'seconds' field may have any valid
// 64-bit integer value, with the 'nanoseconds' fields limited to the range
// '[ -999999999, 999999999 ]' and with the additional constraint that the two
// fields are either both non-negative or both non-positive.  When setting the
// value of a time interval via its two-field representation, any integer value
// may be used in either field, with the constraint that the resulting number
// of seconds be representable as a 64-bit integer.  Similarly, the two field
// values may be accessed in the canonical representation using the 'seconds'
// and 'nanoseconds' methods.
//
// Binary arithmetic and relational operators taking two 'bdet_TimeInterval'
// objects, or a 'bdet_TimeInterval' object and a 'double', are provided.  A
// 'double' operand, representing a real number of seconds, is first converted
// to a 'bdet_TimeInterval' object before performing the operation.  Under such
// circumstances, the fractional part of the 'double', if any, is rounded to
// the nearest whole number of nanoseconds.
//
///Usage 1
///-------
// The following snippets of code illustrate how to create and use a
// 'bdet_TimeInterval' object.  First create a default object 'i1':
//..
//    bdet_TimeInterval i1;
//    assert(0 == i1.seconds());
//    assert(0 == i1.nanoseconds());
//..
// Next, set the value of 'i1' to -5 seconds and -10 nanoseconds, and then
// add 4 seconds and 10 nanoseconds:
//..
//    i1.setInterval(-5, -10);
//    assert( -5 == i1.seconds());
//    assert(-10 == i1.nanoseconds());
//    i1.addInterval(4, 10);
//    assert( -1 == i1.seconds());
//    assert(  0 == i1.nanoseconds());
//..
// Then create 'i2' as a copy of 'i1':
//..
//    bdet_TimeInterval i2(i1);
//    assert(-1 == i2.seconds());
//    assert( 0 == i2.nanoseconds());
//..
// Now confirm that 'i2' has the same value as that of 'i1':
//..
//    assert(i2 == i1);
//..
// Then add to 'i2', in succession, 1 second, 2 milliseconds, 3 microseconds,
// and 4 nanoseconds:
//..
//    i2.addSeconds(1).addMilliseconds(2);
//    i2.addMicroseconds(3).addNanoseconds(4);
//    assert(      0 == i2.seconds());
//    assert(2003004 == i2.nanoseconds());
//..
// Next assign 3.5 seconds to 'i2', and then add 2.73 seconds:
//..
//    i2 = 3.5;
//    i2 += 2.73;
//    assert(        6 == i2.seconds());
//    assert(230000000 == i2.nanoseconds());
//..
// Finally, write the value of 'i2' to 'stdout':
//..
//    bsl::cout << i2 << bsl::endl;
//..
// The output operator produces the following (single-line) format:
//..
//    (6, 230000000)
//..
///Usage 2
///-------
// The following snippets of code show a convenient way to create a timeout
// value by adding 1500 milliseconds to the current system time.  We assume the
// existence of a utility interface 'my_SystemTime' with a static member
// function 'now()' that returns a 'bdet_TimeInterval' object containing the
// current system time:
//..
//    my_SystemTime::now().addMilliseconds(1500);
//..
// Alternatively, we could create the desired timeout value as follows:
//..
//    my_SystemTime::now() + 1.5;
//..
// Note that by specifying a constant such as 'TIME_LIMIT' as either an 'int'
// or a 'double', we are sure to define it at file scope without entailing a
// runtime initialization.  So, instead of using the following definition:
//..
//    // static const bdet_TimeInterval TIME_LIMIT(1, 500000000);
//..
// We could use a more generic definition such as:
//..
//    static const double TIME_LIMIT = 1.5;
//..
// If we need a 'TIME_LIMIT' value of 3 seconds, we would use the following
// definition instead:
//..
//    static const int TIME_LIMIT = 3;
//..
// Later, 'TIME_LIMIT' can be easily used to create a desired timeout value as
// follows:
//..
//    bdet_TimeInterval timeout;
//    timeout = my_SystemTime::now() + TIME_LIMIT;
//    bsl::cout << timeout << bsl::endl;
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


namespace BloombergLP {

                        // =======================
                        // class bdet_TimeInterval
                        // =======================

class bdet_TimeInterval {
    // Each instance of this class represents a time interval.  In the
    // "canonical representation" of a time interval, the 'seconds' field may
    // have any valid 64-bit integer value, with the 'nanoseconds' field
    // limited to the range '[ -999999999, 999999999 ]' and with the additional
    // constraint that the two fields are either both non-negative or both
    // non-positive.  More generally, this class supports a complete set of
    // *value* *semantic* operations, including copy construction, assignment,
    // equality comparison, 'ostream' printing, and 'bdex' serialization.
    // (A precise operational definition of when two instances have the same
    // value can be found in the description of 'operator==' for this class.)
    // This class is *exception* *neutral* with no guarantee of rollback: if an
    // exception is thrown during the invocation of a method on a pre-existing
    // instance, the object is left in a valid state, but its value is
    // undefined.  In no event is memory leaked.  Finally, *aliasing* (e.g.,
    // using all or part of an object as both source and destination) is
    // supported in all cases.

    // PRIVATE TYPES
    enum {
        BDET_MILLISECS_PER_SEC     = 1000,        // one thousand
        BDET_MICROSECS_PER_SEC     = 1000000,     // one million
        BDET_NANOSECS_PER_MICROSEC = 1000,        // one thousand
        BDET_NANOSECS_PER_MILLISEC = 1000000,     // one million
        BDET_NANOSECS_PER_SEC      = 1000000000   // one billion
    };

    // DATA
    bsls_PlatformUtil::Int64 d_seconds;      // value field for seconds
    int                      d_nanoseconds;  // value field for nanoseconds

  public:
    // TRAITS
    BSLALG_DECLARE_NESTED_TRAITS(bdet_TimeInterval,
                                 bslalg_TypeTraitBitwiseCopyable);

    // CREATORS
    bdet_TimeInterval();
        // Create a time interval having the value of 0 seconds and 0
        // nanoseconds.

    bdet_TimeInterval(bsls_PlatformUtil::Int64 seconds, int nanoseconds);
        // Create a time interval having the value given by the sum of the
        // specified integral number of 'seconds' and 'nanoseconds'.  Note that
        // there is no restriction on the sign or magnitude of either argument,
        // except that the result must be representable by this object.

    explicit bdet_TimeInterval(double seconds);
        // Create a time interval having the value represented by the specified
        // real number of 'seconds'.  The fractional part of 'seconds', if any,
        // is rounded to the nearest whole number of nanoseconds before being
        // stored in this object.

    bdet_TimeInterval(const bdet_TimeInterval& original);
        // Create a time interval having the value of the specified 'original'
        // time interval.

    // ~bdet_TimeInterval();
        // Destroy this time interval object.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS
    bdet_TimeInterval& operator=(const bdet_TimeInterval& rhs);
        // Assign to this time interval the value of the specified 'rhs' time
        // interval and return a reference to this object.

    bdet_TimeInterval& operator=(double rhs);
        // Assign to this time interval the value of the specified 'rhs' real
        // number of seconds, and return a reference to this object.  The
        // fractional part of 'rhs', if any, is rounded to the nearest whole
        // number of nanoseconds before being assigned to this object.

    bdet_TimeInterval& operator+=(const bdet_TimeInterval& rhs);
        // Add to this time interval the value of the specified 'rhs' time
        // interval and return a reference to this object.

    bdet_TimeInterval& operator+=(double rhs);
        // Add to this time interval the value of the specified 'rhs' real
        // number of seconds, and return a reference to this object.  The
        // fractional part of 'rhs', if any, is rounded to the nearest whole
        // number of nanoseconds before being added to this object.

    bdet_TimeInterval& operator-=(const bdet_TimeInterval& rhs);
        // Subtract from this time interval the value of the specified 'rhs'
        // time interval and return a reference to this object.

    bdet_TimeInterval& operator-=(double rhs);
        // Subtract from this time interval the value of the specified 'rhs'
        // real number of seconds, and return a reference to this object.  The
        // fractional part of 'rhs', if any, is rounded to the nearest whole
        // number of nanoseconds before being subtracted from this object.

    bdet_TimeInterval& addSeconds(bsls_PlatformUtil::Int64 seconds);
        // Add to this time interval the time interval value represented by the
        // specified integral number of 'seconds' and return a reference to
        // this object.

    bdet_TimeInterval& addMilliseconds(bsls_PlatformUtil::Int64 milliseconds);
        // Add to this time interval the time interval value represented by the
        // specified integral number of 'milliseconds' and return a reference
        // to this object.

    bdet_TimeInterval& addMicroseconds(bsls_PlatformUtil::Int64 microseconds);
        // Add to this time interval the time interval value represented by the
        // specified integral number of 'microseconds' and return a reference
        // to this object.

    bdet_TimeInterval& addNanoseconds(bsls_PlatformUtil::Int64 nanoseconds);
        // Add to this time interval the time interval value represented by the
        // specified integral number of 'nanoseconds' and return a reference to
        // this object.

    void addInterval(bsls_PlatformUtil::Int64 seconds, int nanoseconds);
        // Add to this time interval the time interval value represented by the
        // specified integral number of 'seconds' and 'nanoseconds'.

    void setInterval(bsls_PlatformUtil::Int64 seconds, int nanoseconds);
        // Set this time interval to have the value given by the sum of the
        // specified integral number of 'seconds' and 'nanoseconds'.  Note that
        // there is no restriction on the sign or magnitude of either argument,
        // except that the result must be representable by this object.

    template <class STREAM>
    STREAM& bdexStreamIn(STREAM& stream, int version);
        // Assign to this time interval the value read from the specified input
        // 'stream' using the specified 'version' format and return a reference
        // to the modifiable 'stream'.  If 'stream' is initially invalid, this
        // operation has no effect.  If 'stream' becomes invalid during this
        // operation, this object is valid, but its value is undefined.  If
        // 'version' is not supported, 'stream' is marked invalid and this
        // object is unaltered.  Note that no version is read from 'stream'.
        // (See the 'bdex' package-level documentation for more information on
        // 'bdex' streaming of value-semantic types and containers.)

    // ACCESSORS
    bsls_PlatformUtil::Int64 seconds() const;
        // Return the seconds field in the canonical representation of the
        // value of this time interval.

    int nanoseconds() const;
        // Return the nanoseconds field in the canonical representation of the
        // value of this time interval.

    bsls_PlatformUtil::Int64 totalMilliseconds() const;
        // Return the value of this time interval as an integral number of
        // milliseconds, rounded towards zero.  The behavior is undefined
        // unless the number of milliseconds is small enough to fit into a
        // 64-bit integer.  Note that the return value may be negative.

    bsls_PlatformUtil::Int64 totalMicroseconds() const;
        // Return the value of this time interval as an integral number of
        // microseconds, rounded towards zero.  The behavior is undefined
        // unless the number of microseconds is small enough to fit into a
        // 64-bit integer.  Note that the return value may be negative.

    double totalSecondsAsDouble() const;
        // Return the value of this time interval as a real number of seconds.
        // Note that the return value may be negative and may have a fractional
        // part (representing the nanosecond field of this object).  Also note
        // that the conversion from the internal representation to a 'double'
        // may *lose* precision.

    static int maxSupportedBdexVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.)

    template <class STREAM>
    STREAM& bdexStreamOut(STREAM& stream, int version) const;
        // Write this value to the specified output 'stream' using the
        // specified 'version' format and return a reference to the modifiable
        // 'stream'.  If 'version' is not supported, 'stream' is unmodified.
        // Note that 'version' is not written to 'stream'.  (See the 'bdex'
        // package-level documentation for more information on 'bdex' streaming
        // of value-semantic types and containers.)

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

    static int maxSupportedVersion();
        // Return the most current 'bdex' streaming version number supported by
        // this class.  (See the 'bdex' package-level documentation for more
        // information on 'bdex' streaming of value-semantic types and
        // containers.)
        //
        // DEPRECATED: use 'maxSupportedBdexVersion' instead.

    bsl::ostream& streamOut(bsl::ostream& stream) const;
        // Format this time interval to the specified output 'stream' and
        // return a reference to the modifiable 'stream'.
        //
        // DEPRECATED: use 'streamOut' instead.

#endif

};

// FREE OPERATORS
inline
bdet_TimeInterval operator+(const bdet_TimeInterval& lhs,
                            const bdet_TimeInterval& rhs);
inline
bdet_TimeInterval operator+(const bdet_TimeInterval& lhs, double rhs);
inline
bdet_TimeInterval operator+(double lhs, const bdet_TimeInterval& rhs);
    // Return a 'bdet_TimeInterval' value that is the sum of the specified
    // 'lhs' and 'rhs' time intervals.

inline
bdet_TimeInterval operator-(const bdet_TimeInterval& lhs,
                            const bdet_TimeInterval& rhs);
inline
bdet_TimeInterval operator-(const bdet_TimeInterval& lhs, double rhs);
inline
bdet_TimeInterval operator-(double lhs, const bdet_TimeInterval& rhs);
    // Return a 'bdet_TimeInterval' value that is the difference between the
    // specified 'lhs' and 'rhs' time intervals.

inline
bdet_TimeInterval operator-(const bdet_TimeInterval& rhs);
    // Return a 'bdet_TimeInterval' value that is the negative of the specified
    // 'rhs' time interval.

inline
bool operator==(const bdet_TimeInterval& lhs, const bdet_TimeInterval& rhs);
inline
bool operator==(const bdet_TimeInterval& lhs, double rhs);
inline
bool operator==(double lhs, const bdet_TimeInterval& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time intervals have the
    // same value, and 'false' otherwise.  Two time intervals have the same
    // value if their respective second and nanosecond fields have the same
    // value.

inline
bool operator!=(const bdet_TimeInterval& lhs, const bdet_TimeInterval& rhs);
inline
bool operator!=(const bdet_TimeInterval& lhs, double rhs);
inline
bool operator!=(double lhs, const bdet_TimeInterval& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' time intervals do not
    // have the same value, and 'false' otherwise.  Two time intervals do not
    // have the same value if their respective second or nanosecond fields
    // differ in value.

inline
bool operator< (const bdet_TimeInterval& lhs, const bdet_TimeInterval& rhs);
inline
bool operator< (const bdet_TimeInterval& lhs, double rhs);
inline
bool operator< (double lhs, const bdet_TimeInterval& rhs);
inline
bool operator<=(const bdet_TimeInterval& lhs, const bdet_TimeInterval& rhs);
inline
bool operator<=(const bdet_TimeInterval& lhs, double rhs);
inline
bool operator<=(double lhs, const bdet_TimeInterval& rhs);
inline
bool operator> (const bdet_TimeInterval& lhs, const bdet_TimeInterval& rhs);
inline
bool operator> (const bdet_TimeInterval& lhs, double rhs);
inline
bool operator> (double lhs, const bdet_TimeInterval& rhs);
inline
bool operator>=(const bdet_TimeInterval& lhs, const bdet_TimeInterval& rhs);
inline
bool operator>=(const bdet_TimeInterval& lhs, double rhs);
inline
bool operator>=(double lhs, const bdet_TimeInterval& rhs);
    // Return 'true' if the nominal relation between the specified 'lhs' and
    // 'rhs' time interval values holds, and 'false' otherwise.

bsl::ostream& operator<<(bsl::ostream&            stream,
                         const bdet_TimeInterval& timeInterval);
    // Write the specified 'timeInterval' value to the specified 'stream' in
    // the format of '(seconds, nanoseconds)', e.g., '(-5, -10)', and return a
    // reference to the modifiable 'stream'.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                        // -----------------------
                        // class bdet_TimeInterval
                        // -----------------------

// CREATORS
inline
bdet_TimeInterval::bdet_TimeInterval()
: d_seconds(0)
, d_nanoseconds(0)
{
}

inline
bdet_TimeInterval::bdet_TimeInterval(const bdet_TimeInterval& original)
: d_seconds(original.d_seconds)
, d_nanoseconds(original.d_nanoseconds)
{
}

// MANIPULATORS
inline
bdet_TimeInterval& bdet_TimeInterval::operator=(const bdet_TimeInterval& rhs)
{
    d_seconds     = rhs.d_seconds;
    d_nanoseconds = rhs.d_nanoseconds;
    return *this;
}

inline
bdet_TimeInterval& bdet_TimeInterval::operator=(double rhs)
{
    *this = bdet_TimeInterval(rhs);
    return *this;
}

inline
bdet_TimeInterval& bdet_TimeInterval::operator+=(double rhs)
{
    *this += bdet_TimeInterval(rhs);
    return *this;
}

inline
bdet_TimeInterval& bdet_TimeInterval::operator-=(double rhs)
{
    *this -= bdet_TimeInterval(rhs);
    return *this;
}

template <class STREAM>
STREAM& bdet_TimeInterval::bdexStreamIn(STREAM& stream, int version)
{
    if (stream) {
        switch (version) {  // switch on the version
          case 1: {
            bsls_PlatformUtil::Int64 seconds;
            stream.getInt64(seconds);
            if (!stream) {
                return stream;
            }

            int nanoseconds;
            stream.getInt32(nanoseconds);
            if (!stream) {
                return stream;
            }

            if ((seconds > 0 && nanoseconds < 0)
             || (seconds < 0 && nanoseconds > 0)) {
                stream.invalidate();
                return stream;
            }

            d_seconds     = seconds;
            d_nanoseconds = nanoseconds;
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
bsls_PlatformUtil::Int64 bdet_TimeInterval::seconds() const
{
    return d_seconds;
}

inline
int bdet_TimeInterval::nanoseconds() const
{
    return d_nanoseconds;
}

inline
bsls_PlatformUtil::Int64 bdet_TimeInterval::totalMilliseconds() const
{
    return d_seconds * BDET_MILLISECS_PER_SEC
         + d_nanoseconds / BDET_NANOSECS_PER_MILLISEC;
}

inline
bsls_PlatformUtil::Int64 bdet_TimeInterval::totalMicroseconds() const
{
    return d_seconds * BDET_MICROSECS_PER_SEC
         + d_nanoseconds / BDET_NANOSECS_PER_MICROSEC;
}

inline
double bdet_TimeInterval::totalSecondsAsDouble() const
{
    return (double)d_seconds + d_nanoseconds / (1.0 * BDET_NANOSECS_PER_SEC);
}

inline
int bdet_TimeInterval::maxSupportedBdexVersion()
{
    return 1;
}

template <class STREAM>
STREAM& bdet_TimeInterval::bdexStreamOut(STREAM& stream, int version) const
{
    switch (version) {
      case 1: {
        stream.putInt64(d_seconds);
        stream.putInt32(d_nanoseconds);
      } break;
    }
    return stream;
}

#if !defined(BSL_LEGACY) || 1 == BSL_LEGACY

inline
int bdet_TimeInterval::maxSupportedVersion()
{
    return maxSupportedBdexVersion();
}

inline
bsl::ostream& bdet_TimeInterval::streamOut(bsl::ostream& stream) const
{
    return stream << *this;
}

#endif

// FREE OPERATORS
inline
bdet_TimeInterval operator+(const bdet_TimeInterval& lhs,
                            const bdet_TimeInterval& rhs)
{
    return bdet_TimeInterval(lhs.seconds()     + rhs.seconds(),
                             lhs.nanoseconds() + rhs.nanoseconds());
}

inline
bdet_TimeInterval operator+(const bdet_TimeInterval& lhs, double rhs)
{
    return lhs + bdet_TimeInterval(rhs);
}

inline
bdet_TimeInterval operator+(double lhs, const bdet_TimeInterval& rhs)
{
    return bdet_TimeInterval(lhs) + rhs;
}

inline
bdet_TimeInterval operator-(const bdet_TimeInterval& lhs,
                            const bdet_TimeInterval& rhs)
{
    return bdet_TimeInterval(lhs.seconds()     - rhs.seconds(),
                             lhs.nanoseconds() - rhs.nanoseconds());
}

inline
bdet_TimeInterval operator-(const bdet_TimeInterval& lhs, double rhs)
{
    return lhs - bdet_TimeInterval(rhs);
}

inline
bdet_TimeInterval operator-(double lhs, const bdet_TimeInterval& rhs)
{
    return bdet_TimeInterval(lhs) - rhs;
}

inline
bdet_TimeInterval operator-(const bdet_TimeInterval& rhs)
{
    return bdet_TimeInterval(-rhs.seconds(), -rhs.nanoseconds());
}

inline
bool operator==(const bdet_TimeInterval& lhs, const bdet_TimeInterval& rhs)
{
    return lhs.seconds()     == rhs.seconds()
        && lhs.nanoseconds() == rhs.nanoseconds();
}

inline
bool operator==(const bdet_TimeInterval& lhs, double rhs)
{
    return lhs == bdet_TimeInterval(rhs);
}

inline
bool operator==(double lhs, const bdet_TimeInterval& rhs)
{
    return bdet_TimeInterval(lhs) == rhs;
}

inline
bool operator!=(const bdet_TimeInterval& lhs, const bdet_TimeInterval& rhs)
{
    return lhs.seconds()     != rhs.seconds()
        || lhs.nanoseconds() != rhs.nanoseconds();
}

inline
bool operator!=(const bdet_TimeInterval& lhs, double rhs)
{
    return lhs != bdet_TimeInterval(rhs);
}

inline
bool operator!=(double lhs, const bdet_TimeInterval& rhs)
{
    return bdet_TimeInterval(lhs) != rhs;
}

inline
bool operator< (const bdet_TimeInterval& lhs, const bdet_TimeInterval& rhs)
{
    return lhs.seconds() < rhs.seconds()
        || (lhs.seconds() == rhs.seconds()
            && lhs.nanoseconds() < rhs.nanoseconds());
}

inline
bool operator< (const bdet_TimeInterval& lhs, double rhs)
{
    return lhs < bdet_TimeInterval(rhs);
}

inline
bool operator< (double lhs, const bdet_TimeInterval& rhs)
{
    return bdet_TimeInterval(lhs) < rhs;
}

inline
bool operator<=(const bdet_TimeInterval& lhs, const bdet_TimeInterval& rhs)
{
    return lhs.seconds() < rhs.seconds()
        || (lhs.seconds() == rhs.seconds()
            && lhs.nanoseconds() <= rhs.nanoseconds());
}

inline
bool operator<=(const bdet_TimeInterval& lhs, double rhs)
{
    return lhs <= bdet_TimeInterval(rhs);
}

inline
bool operator<=(double lhs, const bdet_TimeInterval& rhs)
{
    return bdet_TimeInterval(lhs) <= rhs;
}

inline
bool operator> (const bdet_TimeInterval& lhs, const bdet_TimeInterval& rhs)
{
    return lhs.seconds() > rhs.seconds()
        || (lhs.seconds() == rhs.seconds()
            && lhs.nanoseconds() > rhs.nanoseconds());
}

inline
bool operator> (const bdet_TimeInterval& lhs, double rhs)
{
    return lhs > bdet_TimeInterval(rhs);
}

inline
bool operator> (double lhs, const bdet_TimeInterval& rhs)
{
    return bdet_TimeInterval(lhs) > rhs;
}

inline
bool operator>=(const bdet_TimeInterval& lhs, const bdet_TimeInterval& rhs)
{
    return lhs.seconds() > rhs.seconds()
        || (lhs.seconds() == rhs.seconds()
            && lhs.nanoseconds() >= rhs.nanoseconds());
}

inline
bool operator>=(const bdet_TimeInterval& lhs, double rhs)
{
    return lhs >= bdet_TimeInterval(rhs);
}

inline
bool operator>=(double lhs, const bdet_TimeInterval& rhs)
{
    return bdet_TimeInterval(lhs) >= rhs;
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
