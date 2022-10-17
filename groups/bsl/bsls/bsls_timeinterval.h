// bsls_timeinterval.h                                                -*-C++-*-
#ifndef INCLUDED_BSLS_TIMEINTERVAL
#define INCLUDED_BSLS_TIMEINTERVAL

#include <bsls_ident.h>
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
// '[ -999,999,999..999,999,999 ]', and with the additional constraint that the
// two fields are either both non-negative or both non-positive.  When setting
// the value of a time interval via its two-field representation, any integer
// value may be used in either field, with the constraint that the resulting
// number of seconds be representable as a 64-bit signed integer.  Similarly,
// the two field values may be accessed in the canonical representation using
// the 'seconds' and 'nanoseconds' methods.
//
// Binary arithmetic and relational operators taking two 'bsls::TimeInterval'
// objects, or a 'bsls::TimeInterval' object and a 'double', are provided.  A
// 'double' operand, representing a real number of seconds, is first converted
// to a 'bsls::TimeInterval' object before performing the operation.  Under
// such circumstances, the fractional part of the 'double', if any, is rounded
// to the nearest whole number of nanoseconds.
//
///User-Defined Literals
///---------------------
// The user-defined literal 'operator"" _h', 'operator"" _min',
// 'operator"" _s', 'operator"" _ms', 'operator"" _us' and 'operator"" _ns' are
// declared for the 'TimeInterval'.  These suffixes can be applied to integer
// literals and allow to create an object, representing the specified number of
// hours, minutes, seconds, milliseconds, microseconds or nanoseconds
// respectively:
//..
//  using namespace bsls::TimeIntervalLiterals;
//
//  bsls::TimeInterval i0 = 10_h;
//  assert(36000   == i0.seconds()    );
//  assert(0       == i0.nanoseconds());
//
//  bsls::TimeInterval i1 = 10001_ms;
//  assert(10      == i1.seconds()    );
//  assert(1000000 == i1.nanoseconds());
//
//  bsls::TimeInterval i2 = 100_ns;
//  assert(0       == i2.seconds()    );
//  assert(100     == i2.nanoseconds());
//..
// The operators providing literals are available in the
// 'BloombergLP::bsls::literals::TimeIntervalLiterals' namespace (where
// 'literals' and 'TimeIntervalLiterals' are both inline namespaces). Because
// of inline namespaces, there are several viable options for a using
// declaration, but *we* *recommend*
// 'using namespace bsls::TimeIntervalLiterals', which minimizes the scope of
// the using declaration.
//
// Note that user defined literals can be used only if the compiler supports
// the C++11 standard.
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

#include <bsls_assert.h>
#include <bsls_compilerfeatures.h>
#include <bsls_keyword.h>
#include <bsls_libraryfeatures.h>
#include <bsls_preconditions.h>
#include <bsls_types.h>

#if BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <chrono>
#include <type_traits>
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

#include <iosfwd>
#include <limits.h>  // 'LLONG_MIN', 'LLONG_MAX'

#ifndef BDE_DONT_ALLOW_TRANSITIVE_INCLUDES
#include <bsls_nativestd.h>
#endif // BDE_DONT_ALLOW_TRANSITIVE_INCLUDES

// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -FABC01  // 'add*' operations are ordered by time unit

#if BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#define BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
#endif  // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY

namespace BloombergLP {
namespace bsls {

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS

                  // =======================================
                  // template struct TimeInterval_IsDuration
                  // =======================================

template <class TYPE>
struct TimeInterval_IsDuration : std::false_type {
    // Template metafunction to determine if the specified 'TYPE' is a
    // 'std::chrono::duration'.
};

template <class REP, class PER>
struct TimeInterval_IsDuration<std::chrono::duration<REP, PER> >
                                                             : std::true_type {
    // Template metafunction specialization for 'std::chrono::duration' types.
};

                   // =============================
                   // struct TimeInterval_RepTraits
                   // =============================

template <class REP>
struct TimeInterval_RepTraits {
    // Trait metafunction that determines whether the specified 'REP' type is
    // considered a floating point type.

    static const bool k_IS_FLOAT =
                              std::is_floating_point<REP>::value
                           || std::chrono::treat_as_floating_point<REP>::value;
    // This compile time constant is 'true' if the 'REP' (template type
    // argument) is indicated to be a floating point type by the underlying
    // library.  Otherwise, if the underlying library does not consider 'REP'
    // floating point anywhere (see note), 'k_IS_FLOAT' is 'false'.  Note that
    // to cover all scenarios we need to examine *two* traits.  The first one
    // that tells if the representation type is a floating point (arithmetic)
    // type or not (from '<type_traits>'), and another that tells that
    // representation type is considered a floating point type from the
    // 'std::chrono' perspective.  Formally the second one just duplicates the
    // first one, but *in theory* either can be specialized to be true,
    // independently.  Also note that conversions from 'std::chrono::duration'
    // with a floating point representation to 'bsls::TimeInterval' are *not*
    // supported for now.
};

                   // ==================================
                   // struct TimeInterval_DurationTraits
                   // ==================================

template <class REP, class PERIOD>
struct TimeInterval_DurationTraits {
    // Trait metafunction that determines whether the
    // 'std::chrono::duration<REP, PERIOD>' object can be converted to
    // 'bsls::TimeInterval' either implicitly or explicitly.

    static const bool k_IS_FLOAT    = TimeInterval_RepTraits<REP>::k_IS_FLOAT;
        // This compile time constant is 'true' if the 'REP' (template type
        // argument) is indicated to be a floating point type by the underlying
        // library.  Otherwise, if the underlying library does not consider
        // 'REP' floating point anywhere (see 'TimeInterval_RepTraits'),
        // 'k_IS_FLOAT' is 'false'.

    static const bool k_IS_IMPLICIT = (std::nano::den % PERIOD::den == 0);
        // This compile time constant is 'true' if any possible value of an
        // 'std::chrono::duration<REP, PERIOD> object will be represented by
        // integer nanoseconds (fractions of nanoseconds are not required).
        // Otherwise this value is 'false'.

    static const bool k_IMPLICIT_CONVERSION_ENABLED =
                                                 !k_IS_FLOAT &&  k_IS_IMPLICIT;
        // This compile time constant is 'true' if
        // 'std::chrono::duration<REP, PERIOD>' objects will be implicitly
        // converted to 'TimeInterval', and 'false' otherwise.  This value is
        // intended to be used with 'enable_if' to enable implicitly converting
        // function overloads.  Note that this boolean value is mutually
        // exclusive with 'k_EXPLICIT_CONVERION_ENABLED' as in they will never
        // be both 'true' for the same 'REP" and 'PERIOD', but they may be both
        // 'false' for floats.

    static const bool k_EXPLICIT_CONVERSION_ENABLED =
                                                 !k_IS_FLOAT && !k_IS_IMPLICIT;
        // This compile time constant is 'true' if
        // 'std::chrono::duration<REP, PERIOD>' objects can be explicitly
        // converted to 'TimeInterval', and 'false' otherwise.  This value is
        // intended to be used with 'enable_if' to enable explicitly converting
        // function overloads.  Note that this boolean value is mutually
        // exclusive with 'k_IMPLICIT_CONVERION_ENABLED' as in they will never
        // be both 'true' for the same 'REP" and 'PERIOD', but they may be both
        // 'false' for floats.
};

#endif
                        // ==================
                        // class TimeInterval
                        // ==================

class TimeInterval {
    // Each instance of this value-semantic type represents a time interval
    // with nanosecond resolution.  In the "canonical representation" of a time
    // interval, the 'seconds' field may have any 64-bit signed integer value,
    // with the 'nanoseconds' field limited to the range
    // '[ -999,999,999..999,999,999 ]', and with the additional constraint that
    // the two fields are either both non-negative or both non-positive.

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
    BSLS_KEYWORD_CONSTEXPR
    static bool isSumValidInt64(bsls::Types::Int64 lhs,
                                bsls::Types::Int64 rhs);
        // Return 'true' if the sum of the specified 'lhs' and 'rhs' can be
        // represented using a 64-bit signed integer, and 'false' otherwise.

  public:
    // CLASS METHODS
    BSLS_KEYWORD_CONSTEXPR
    static bool isValid(bsls::Types::Int64 seconds,
                        int                nanoseconds);
        // Return 'true' if a 'TimeInterval' object can be constructed from the
        // specified 'seconds' and 'nanoseconds', and 'false' otherwise.  A
        // time interval can be constructed from 'seconds' and 'nanoseconds' if
        // their sum results in a time interval whose total number of seconds
        // can be represented with a 64-bit signed integer.

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
    template <class REP, class PERIOD>
    static bool isValid(const std::chrono::duration<REP, PERIOD>& duration);
        // Return 'true' if a 'TimeInterval' object can be constructed from the
        // specified 'duration', and 'false' otherwise.  A time interval can be
        // constructed from 'duration' if duration's value converted to seconds
        // can be represented with a 64-bit signed integer.
#endif

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
    BSLS_KEYWORD_CONSTEXPR
    TimeInterval();
        // Create a time interval having the value of 0 seconds and 0
        // nanoseconds.

    BSLS_KEYWORD_CONSTEXPR_CPP14
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

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
    template <class REP_TYPE, class PERIOD_TYPE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 TimeInterval(
        const std::chrono::duration<REP_TYPE, PERIOD_TYPE>& duration,
    typename std::enable_if<TimeInterval_DurationTraits<
                                   REP_TYPE,
                                   PERIOD_TYPE>::k_IMPLICIT_CONVERSION_ENABLED,
                            int>::type * = 0);
    template <class REP_TYPE, class PERIOD_TYPE>
    explicit BSLS_KEYWORD_CONSTEXPR_CPP14 TimeInterval(
        const std::chrono::duration<REP_TYPE, PERIOD_TYPE>& duration,
    typename std::enable_if<TimeInterval_DurationTraits<
                                   REP_TYPE,
                                   PERIOD_TYPE>::k_EXPLICIT_CONVERSION_ENABLED,
                            int>::type * = 0);
        // Create a time interval having the value represented by the specified
        // 'duration'.  Only integer representations of 'duration' are
        // supported (i.e. 'REP_TYPE' is integer). If the 'duration' can be
        // represented *exactly* by an integer nanoseconds this constructor
        // will be implicit.  Otherwise, the constructor will be explicit.  The
        // behavior is undefined unless the 'duration' can be converted to a
        // valid 'TimeInterval' object, whose 'seconds' field may have any
        // 64-bit signed integer value and 'nanoseconds' field limited to the
        // range '[ -999,999,999..999,999,999 ]'.  Note that the current
        // implementation of the lossy conversions (e.g., fractions of a
        // nanosecond) truncates the values towards zero, however this behavior
        // may change without notice in the future, so *do not* rely on this.
#endif

    //! TimeInterval(const TimeInterval& original) = default;
        // Create a time interval having the value of the specified 'original'
        // time interval.  Note that this trivial copy constructor is
        // generated by the compiler.

    //! ~TimeInterval() = default;
        // Destroy this time interval object.  Note that this trivial
        // destructor is generated by the compiler.

    // MANIPULATORS

                                  // Operator Overloads

    //! TimeInterval& operator=(const TimeInterval& rhs) = default;
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

    BSLS_KEYWORD_CONSTEXPR_CPP14
    TimeInterval& addDays(bsls::Types::Int64 days);
        // Add to this time interval the number of seconds represented by the
        // specified integral number of 'days', and return a reference
        // providing modifiable access to this object.  The behavior is
        // undefined unless the number of seconds in 'days', and the total
        // number of seconds in the resulting time interval, can both be
        // represented with 64-bit signed integers.  Note that 'days' may be
        // negative.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    TimeInterval& addHours(bsls::Types::Int64 hours);
        // Add to this time interval the number of seconds represented by the
        // specified integral number of 'hours', and return a reference
        // providing modifiable access to this object.  The behavior is
        // undefined unless the number of seconds in 'hours', and the total
        // number of seconds in the resulting time interval, can both be
        // represented with 64-bit signed integers.  Note that 'hours' may be
        // negative.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    TimeInterval& addMinutes(bsls::Types::Int64 minutes);
        // Add to this time interval the number of seconds represented by the
        // specified integral number of 'minutes', and return a reference
        // providing modifiable access to this object.  The behavior is
        // undefined unless the number of seconds in 'minutes', and the total
        // number of seconds in the resulting time interval, can both be
        // represented with 64-bit signed integers.  Note that 'minutes' may be
        // negative.

    BSLS_KEYWORD_CONSTEXPR_CPP14
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

    BSLS_KEYWORD_CONSTEXPR_CPP14
    void setTotalDays(bsls::Types::Int64 days);
        // Set the overall value of this object to indicate the specified
        // integral number of 'days'.  The behavior is undefined unless the
        // number of seconds in 'days' can be represented with a 64-bit signed
        // integer.  Note that 'days' may be negative.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    void setTotalHours(bsls::Types::Int64 hours);
        // Set the overall value of this object to indicate the specified
        // integral number of 'hours'.  The behavior is undefined unless the
        // number of seconds in 'hours' can be represented with a 64-bit signed
        // integer.  Note that 'hours' may be negative.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    void setTotalMinutes(bsls::Types::Int64 minutes);
        // Set the overall value of this object to indicate the specified
        // integral number of 'minutes'.  The behavior is undefined unless the
        // number of seconds in 'minutes' can be represented with a 64-bit
        // signed integer.  Note that 'minutes' may be negative.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    void setTotalSeconds(bsls::Types::Int64 seconds);
        // Set the overall value of this object to indicate the specified
        // integral number of 'seconds'.  Note that 'seconds' may be negative.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    void setTotalMilliseconds(bsls::Types::Int64 milliseconds);
        // Set the overall value of this object to indicate the specified
        // integral number of 'milliseconds'.  Note that 'milliseconds' may be
        // negative.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    void setTotalMicroseconds(bsls::Types::Int64 microseconds);
        // Set the overall value of this object to indicate the specified
        // integral number of 'microseconds'.  Note that 'microseconds' may be
        // negative.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    void setTotalNanoseconds(bsls::Types::Int64 nanoseconds);
        // Set the overall value of this object to indicate the specified
        // integral number of 'nanoseconds'.  Note that 'nanoseconds' may be
        // negative.

                                  // Time-Interval-Based Manipulators

    TimeInterval& addInterval(bsls::Types::Int64 seconds, int nanoseconds = 0);
        // Add to this time interval the specified integral number of
        // 'seconds', and the optionally specified integral number of
        // 'nanoseconds'.  If unspecified, 'nanoseconds' is 0.  Return a
        // reference providing modifiable access to this object.  The behavior
        // is undefined unless 'seconds() + seconds', and the total number of
        // seconds in the resulting time interval, can both be represented with
        // 64-bit signed integers.

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
    template <class REP_TYPE, class PERIOD_TYPE>
    BSLS_KEYWORD_CONSTEXPR_CPP14 TimeInterval&
    addDuration(const std::chrono::duration<REP_TYPE, PERIOD_TYPE>& duration,
                typename std::enable_if<
                    TimeInterval_DurationTraits<REP_TYPE, PERIOD_TYPE>::
                                                 k_IMPLICIT_CONVERSION_ENABLED,
                    int>::type * = 0);
        // Add to this time interval the specified 'duration'.  Return a
        // reference providing modifiable access to this object.  The behavior
        // is undefined unless the 'duration' can be converted to a valid
        // 'TimeInterval' object, whose 'seconds' field may have any
        // 64-bit signed integer value and 'nanoseconds' field limited to the
        // range '[ -999,999,999..999,999,999 ]'.  Also the behavior is
        // undefined unless the total number of seconds in the resulting time
        // interval can be represented with 64-bit signed integer.  Note that
        // this operation is allowed only if representation type of the
        // 'duration' is not a floating point type and the 'duration' itself
        // can be *exactly* represented by an integer nanoseconds.
#endif

    BSLS_KEYWORD_CONSTEXPR_CPP14
    void setInterval(bsls::Types::Int64 seconds, int nanoseconds = 0);
        // Set this time interval to have the value given by the sum of the
        // specified integral number of 'seconds', and the optionally specified
        // integral number of 'nanoseconds'.  If unspecified, 'nanoseconds' is
        // 0.  The behavior is undefined unless the total number of seconds in
        // the resulting time interval can be represented with a 64-bit signed
        // integer (see 'isValid').  Note that there is no restriction on the
        // sign or magnitude of either argument except that they must not
        // violate the method's preconditions.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    void setIntervalRaw(bsls::Types::Int64 seconds, int nanoseconds = 0);
        // Set this time interval to have the value given by the sum of the
        // specified integral number of 'seconds', and the optionally specified
        // integral number of 'nanoseconds', where 'seconds' and 'nanoseconds'
        // form a canonical representation of a time interval (see
        // {Representation}).  If unspecified, 'nanoseconds' is 0.  The
        // behavior is undefined unless
        // '-999,999,999 <= nanoseconds <= +999,999,999' and 'seconds' and
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
#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
    template <class DURATION_TYPE>
    bool isInDurationRange(
         typename std::enable_if<TimeInterval_IsDuration<DURATION_TYPE>::value,
                                 int>::type * = 0) const;
        // Return 'true' if the value of this time interval is within the valid
        // range of the parameterized 'DURATION_TYPE', and 'false' otherwise.
        // Note that this function does not participate in overload resolution
        // unless 'DURATION_TYPE' is an instantiation of
        // 'std::chrono::duration'.
#endif

    BSLS_KEYWORD_CONSTEXPR
    int nanoseconds() const;
        // Return the nanoseconds field in the canonical representation of the
        // value of this time interval.

    BSLS_KEYWORD_CONSTEXPR
    bsls::Types::Int64 seconds() const;
        // Return the seconds field in the canonical representation of the
        // value of this time interval.

    BSLS_KEYWORD_CONSTEXPR
    bsls::Types::Int64 totalDays() const;
        // Return the value of this time interval as an integral number of
        // days, rounded towards zero.  Note that the return value may be
        // negative.

    BSLS_KEYWORD_CONSTEXPR
    bsls::Types::Int64 totalHours() const;
        // Return the value of this time interval as an integral number of
        // hours, rounded towards zero.  Note that the return value may be
        // negative.

    BSLS_KEYWORD_CONSTEXPR
    bsls::Types::Int64 totalMinutes() const;
        // Return the value of this time interval as an integral number of
        // minutes, rounded towards zero.  Note that the return value may be
        // negative.

    BSLS_KEYWORD_CONSTEXPR
    bsls::Types::Int64 totalSeconds() const;
        // Return the value of this time interval as an integral number of
        // seconds, rounded towards zero.  Note that the return value may be
        // negative.  Also note that this method returns the same value as
        // 'seconds'.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    bsls::Types::Int64 totalMilliseconds() const;
        // Return the value of this time interval as an integral number of
        // milliseconds, rounded towards zero.  The behavior is undefined
        // unless the number of milliseconds can be represented with a 64-bit
        // signed integer.  Note that the return value may be negative.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    bsls::Types::Int64 totalMicroseconds() const;
        // Return the value of this time interval as an integral number of
        // microseconds, rounded towards zero.  The behavior is undefined
        // unless the number of microseconds can be represented with a 64-bit
        // signed integer.  Note that the return value may be negative.

    BSLS_KEYWORD_CONSTEXPR_CPP14
    bsls::Types::Int64 totalNanoseconds() const;
        // Return the value of this time interval as an integral number of
        // nanoseconds.  The behavior is undefined unless the number of
        // nanoseconds can be represented using a 64-bit signed integer.  Note
        // that the return value may be negative.

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
    template <class DURATION_TYPE>
    BSLS_KEYWORD_CONSTEXPR_CPP14
    typename std::enable_if<
              TimeInterval_IsDuration<DURATION_TYPE>::value &&
              !TimeInterval_RepTraits<typename DURATION_TYPE::rep>::k_IS_FLOAT,
              DURATION_TYPE>::type
    asDuration() const;
        // Return the value of this time interval as a 'std::chrono::duration'
        // object.   This function participates in overloading if
        // 'DURATION_TYPE' is actually an 'std::chrono::duration' instance, and
        // if it has *not* a floating point representation.  The behavior is
        // undefined unless the total number of nanoseconds can be represented
        // using a 'DURATION_TYPE'.  Note that the return value may be
        // negative.
#endif

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

    std::ostream& print(std::ostream& stream,
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

#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation
    // DEPRECATED
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

    template <class STREAM>
    STREAM& streamOut(STREAM& stream) const;
        // !DEPRECATED!: Use 'print' instead.
        //
        // Format this time to the specified output 'stream', and return a
        // reference to the modifiable 'stream'.

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22

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

std::ostream& operator<<(std::ostream&       stream,
                         const TimeInterval& timeInterval);
    // Write the value of the specified 'timeInterval' to the specified output
    // 'stream' in a single-line format, and return a reference providing
    // modifiable access to 'stream'.  If 'stream' is not valid on entry, this
    // operation has no effect.  Note that this human-readable format is not
    // fully specified and can change without notice.  Also note that this
    // method has the same behavior as 'object.print(stream, 0, -1)'.

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)
inline namespace literals {
inline namespace TimeIntervalLiterals {

BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval operator ""_h(  unsigned long long int hours);
    // This user defined literal operator converts the specified 'hours' value
    // to the respective 'TimeInterval' value.  The behavior is undefined
    // unless the specified number of hours can be converted to valid
    // 'TimeInterval' object.  (See the
    // "User-Defined Literals" section in the component-level documentation.)

BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval operator ""_min(unsigned long long int minutes);
    // This user defined literal operator converts the specified 'minutes'
    // value to the respective 'TimeInterval' value.  The behavior is undefined
    // unless the specified number of minutes can be converted to valid
    // 'TimeInterval' object.  (See the
    // "User-Defined Literals" section in the component-level documentation.)

BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval operator ""_s(  unsigned long long int seconds);
    // This user defined literal operator converts the specified 'seconds'
    // value to the respective 'TimeInterval' value.  The behavior is undefined
    // unless the specified number of seconds can be converted to valid
    // 'TimeInterval' object.  (See the
    // "User-Defined Literals" section in the component-level documentation.)

BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval operator ""_ms( unsigned long long int milliseconds);
    // This user defined literal operator converts the specified 'milliseconds'
    // value to the respective 'TimeInterval' value.  (See the
    // "User-Defined Literals" section in the component-level documentation.)

BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval operator ""_us( unsigned long long int microseconds);
    // This user defined literal operator converts the specified 'microseconds'
    // value to the respective 'TimeInterval' value.  (See the
    // "User-Defined Literals" section in the component-level documentation.)

BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval operator ""_ns( unsigned long long int nanoseconds);
    // This user defined literal operator converts the specified 'nanoseconds'
    // value to the respective 'TimeInterval' value.  (See the
    // "User-Defined Literals" section in the component-level documentation.)

}  // close TimeIntervalLiterals namespace
}  // close literals namespace
#endif  // BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE &&
        // BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS

// ============================================================================
//                         INLINE DEFINITIONS
// ============================================================================

                        // ------------------
                        // class TimeInterval
                        // ------------------

// PRIVATE CLASS METHODS
inline BSLS_KEYWORD_CONSTEXPR
bool TimeInterval::isSumValidInt64(bsls::Types::Int64 lhs,
                                   bsls::Types::Int64 rhs)
{
    // {DRQS 164912552} Sun CC miscomplies this ternary operator when the
    // function is invoked from the 'TimeInterval' constructor.
    // return lhs > 0 ? LLONG_MAX - lhs >= rhs : LLONG_MIN - lhs <= rhs;
    return (lhs >  0 && LLONG_MAX - lhs >= rhs) ||
           (lhs <= 0 && LLONG_MIN - lhs <= rhs);
}

// CLASS METHODS
inline
int TimeInterval::maxSupportedBdexVersion(int /* versionSelector */)
{
    return 1;
}

inline BSLS_KEYWORD_CONSTEXPR
bool TimeInterval::isValid(bsls::Types::Int64 seconds,
                           int                nanoseconds)
{
    return isSumValidInt64(seconds, nanoseconds / k_NANOSECS_PER_SEC);
}

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS

template <class REP, class PERIOD>
bool TimeInterval::isValid(const std::chrono::duration<REP, PERIOD>& duration)
{
    std::chrono::duration<long double> minValue =
                                 std::chrono::duration<long double>(LLONG_MIN);
    --minValue;
    std::chrono::duration<long double> maxValue =
                                 std::chrono::duration<long double>(LLONG_MAX);
    ++maxValue;
    const std::chrono::duration<long double> safeDuration =
               std::chrono::duration_cast<std::chrono::duration<long double> >(
                                                                     duration);

    return (safeDuration >= minValue && safeDuration <= maxValue);
}
#endif  // BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS

// CREATORS
inline BSLS_KEYWORD_CONSTEXPR
TimeInterval::TimeInterval()
: d_seconds(0)
, d_nanoseconds(0)
{
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval::TimeInterval(bsls::Types::Int64 seconds,
                           int                nanoseconds)
: d_seconds(0)
, d_nanoseconds(0)
{
    // A seemingly redundant initializer list is needed since members must be
    // initialized by mem-initializer in 'constexpr' constructor.

    setInterval(seconds, nanoseconds);
}

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
template <class REP_TYPE, class PERIOD_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval::TimeInterval(
    const std::chrono::duration<REP_TYPE, PERIOD_TYPE>& duration,
    typename std::enable_if<TimeInterval_DurationTraits<
                                   REP_TYPE,
                                   PERIOD_TYPE>::k_IMPLICIT_CONVERSION_ENABLED,
                            int>::type *)
{
    BSLS_ASSERT((isValid<REP_TYPE, PERIOD_TYPE>(duration)));
    using SecondsRatio = std::ratio<1>;
    using TimeIntervalSeconds =
                     std::chrono::duration<bsls::Types::Int64, SecondsRatio>;
    using TimeIntervalNanoseconds = std::chrono::duration<int, std::nano>;

    const bsls::Types::Int64 k_SECONDS     =
             std::chrono::duration_cast<TimeIntervalSeconds>(duration).count();
    const int                k_NANOSECONDS =
            std::chrono::duration_cast<TimeIntervalNanoseconds>(
                            duration - TimeIntervalSeconds(k_SECONDS)).count();
    setInterval(k_SECONDS, k_NANOSECONDS);
}

template <class REP_TYPE, class PERIOD_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval::TimeInterval(
    const std::chrono::duration<REP_TYPE, PERIOD_TYPE>& duration,
    typename std::enable_if<TimeInterval_DurationTraits<
                                   REP_TYPE,
                                   PERIOD_TYPE>::k_EXPLICIT_CONVERSION_ENABLED,
                            int>::type *)
{
    BSLS_ASSERT((isValid<REP_TYPE, PERIOD_TYPE>(duration)));
    const bsls::Types::Int64 k_SECONDS     =
            std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    const int                k_NANOSECONDS = static_cast<int>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
                          duration - std::chrono::seconds(k_SECONDS)).count());
    setInterval(k_SECONDS, k_NANOSECONDS);
}
#endif

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
    return addInterval(rhs.d_seconds, rhs.d_nanoseconds);
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

    return addInterval(-rhs.d_seconds, -rhs.d_nanoseconds);
}

inline
TimeInterval& TimeInterval::operator-=(double rhs)
{
    *this -= TimeInterval(rhs);
    return *this;
}

                                  // Add Operations

inline BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval& TimeInterval::addDays(bsls::Types::Int64 days)
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_SECONDS_PER_DAY >= days &&
                     LLONG_MIN / k_SECONDS_PER_DAY <= days);

    return addSeconds(days * k_SECONDS_PER_DAY);
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval& TimeInterval::addHours(bsls::Types::Int64 hours)
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_SECONDS_PER_HOUR >= hours &&
                     LLONG_MIN / k_SECONDS_PER_HOUR <= hours);

    return addSeconds(hours * k_SECONDS_PER_HOUR);
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval& TimeInterval::addMinutes(bsls::Types::Int64 minutes)
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_SECONDS_PER_MINUTE >= minutes &&
                     LLONG_MIN / k_SECONDS_PER_MINUTE <= minutes);

    return addSeconds(minutes * k_SECONDS_PER_MINUTE);
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
TimeInterval& TimeInterval::addSeconds(bsls::Types::Int64 seconds)
{
    BSLS_ASSERT_SAFE(isSumValidInt64(seconds, d_seconds));

    d_seconds += seconds;
    if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += k_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= k_NANOSECS_PER_SEC;
    }

    return *this;
}

inline
TimeInterval& TimeInterval::addMilliseconds(bsls::Types::Int64 milliseconds)
{
    return addInterval(          milliseconds / k_MILLISECS_PER_SEC,
               static_cast<int>((milliseconds % k_MILLISECS_PER_SEC) *
                                                     k_NANOSECS_PER_MILLISEC));
}

inline
TimeInterval& TimeInterval::addMicroseconds(bsls::Types::Int64 microseconds)
{
    return addInterval(          microseconds / k_MICROSECS_PER_SEC,
               static_cast<int>((microseconds % k_MICROSECS_PER_SEC) *
                                                     k_NANOSECS_PER_MICROSEC));
}

inline
TimeInterval& TimeInterval::addNanoseconds(bsls::Types::Int64 nanoseconds)
{
    return addInterval(                 nanoseconds / k_NANOSECS_PER_SEC,
                       static_cast<int>(nanoseconds % k_NANOSECS_PER_SEC));
}

                                  // Set Operations

inline BSLS_KEYWORD_CONSTEXPR_CPP14
void TimeInterval::setTotalDays(bsls::Types::Int64 days)
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_SECONDS_PER_DAY >= days &&
                     LLONG_MIN / k_SECONDS_PER_DAY <= days);

    return setTotalSeconds(days * k_SECONDS_PER_DAY);
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
void TimeInterval::setTotalHours(bsls::Types::Int64 hours)
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_SECONDS_PER_HOUR >= hours &&
                     LLONG_MIN / k_SECONDS_PER_HOUR <= hours);

    return setTotalSeconds(hours * k_SECONDS_PER_HOUR);
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
void TimeInterval::setTotalMinutes(bsls::Types::Int64 minutes)
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_SECONDS_PER_MINUTE >= minutes &&
                     LLONG_MIN / k_SECONDS_PER_MINUTE <= minutes);

    return setTotalSeconds(minutes * k_SECONDS_PER_MINUTE);
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
void TimeInterval::setTotalSeconds(bsls::Types::Int64 seconds)
{
    d_seconds     = seconds;
    d_nanoseconds = 0;
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
void TimeInterval::setTotalMilliseconds(bsls::Types::Int64 milliseconds)
{
    setInterval(                  milliseconds / k_MILLISECS_PER_SEC,
                static_cast<int>((milliseconds % k_MILLISECS_PER_SEC) *
                                                     k_NANOSECS_PER_MILLISEC));
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
void TimeInterval::setTotalMicroseconds(bsls::Types::Int64 microseconds)
{
    setInterval(                  microseconds / k_MICROSECS_PER_SEC,
                static_cast<int>((microseconds % k_MICROSECS_PER_SEC) *
                                                     k_NANOSECS_PER_MICROSEC));

}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
void TimeInterval::setTotalNanoseconds(bsls::Types::Int64 nanoseconds)
{
    setInterval(                 nanoseconds / k_NANOSECS_PER_SEC,
                static_cast<int>(nanoseconds % k_NANOSECS_PER_SEC));
}

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
template <class REP_TYPE, class PERIOD_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14 TimeInterval&
TimeInterval::addDuration(
       const std::chrono::duration<REP_TYPE, PERIOD_TYPE>& duration,
       typename std::enable_if<TimeInterval_DurationTraits<
                                   REP_TYPE,
                                   PERIOD_TYPE>::k_IMPLICIT_CONVERSION_ENABLED,
                               int>::type *)
{
    BSLS_ASSERT((isValid<REP_TYPE, PERIOD_TYPE>(duration)));

    const bsls::Types::Int64 k_SECONDS     =
            std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    const int                k_NANOSECONDS = static_cast<int>(
                          std::chrono::duration_cast<std::chrono::nanoseconds>(
                          duration - std::chrono::seconds(k_SECONDS)).count());
    return addInterval(k_SECONDS, k_NANOSECONDS);
}
#endif

inline BSLS_KEYWORD_CONSTEXPR_CPP14
void TimeInterval::setInterval(bsls::Types::Int64 seconds,
                               int                nanoseconds)
{
    BSLS_PRECONDITIONS_BEGIN();
    BSLS_ASSERT(isValid(seconds, nanoseconds));
    BSLS_PRECONDITIONS_END();

    d_seconds = seconds;
    if (nanoseconds >= k_NANOSECS_PER_SEC
     || nanoseconds <= -k_NANOSECS_PER_SEC) {
        d_seconds += nanoseconds / k_NANOSECS_PER_SEC;
        d_nanoseconds = static_cast<int>(nanoseconds % k_NANOSECS_PER_SEC);
    }
    else {
        d_nanoseconds = static_cast<int>(nanoseconds);
    }

    if (d_seconds > 0 && d_nanoseconds < 0) {
        --d_seconds;
        d_nanoseconds += k_NANOSECS_PER_SEC;
    }
    else if (d_seconds < 0 && d_nanoseconds > 0) {
        ++d_seconds;
        d_nanoseconds -= k_NANOSECS_PER_SEC;
    }

}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
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
#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
template <class DURATION_TYPE>
bool TimeInterval::isInDurationRange(
         typename std::enable_if<TimeInterval_IsDuration<DURATION_TYPE>::value,
                                 int>::type *) const
{
    using SecondsRatio = std::ratio<1>;
    using TimeIntervalSeconds =
                       std::chrono::duration<bsls::Types::Int64, SecondsRatio>;
    using TimeIntervalNanoseconds = std::chrono::duration<int, std::nano>;
    using Period = typename DURATION_TYPE::period;
    using LongDoubleTo = std::chrono::duration<long double, Period>;

    const LongDoubleTo MIN_VALUE =
                std::chrono::duration_cast<LongDoubleTo>(DURATION_TYPE::min());

    const LongDoubleTo MAX_VALUE =
                std::chrono::duration_cast<LongDoubleTo>(DURATION_TYPE::max());

    const LongDoubleTo value = std::chrono::duration_cast<LongDoubleTo>(
                                                TimeIntervalSeconds(seconds()))
                             + std::chrono::duration_cast<LongDoubleTo>(
                                       TimeIntervalNanoseconds(nanoseconds()));

    return (MIN_VALUE <= value && value <= MAX_VALUE);
}
#endif  // BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS

inline BSLS_KEYWORD_CONSTEXPR
int TimeInterval::nanoseconds() const
{
    return d_nanoseconds;
}

inline BSLS_KEYWORD_CONSTEXPR
bsls::Types::Int64 TimeInterval::seconds() const
{
    return d_seconds;
}

inline BSLS_KEYWORD_CONSTEXPR
bsls::Types::Int64 TimeInterval::totalDays() const
{
    return d_seconds / k_SECONDS_PER_DAY;
}

inline BSLS_KEYWORD_CONSTEXPR
bsls::Types::Int64 TimeInterval::totalHours() const
{
    return d_seconds / k_SECONDS_PER_HOUR;
}

inline BSLS_KEYWORD_CONSTEXPR
bsls::Types::Int64 TimeInterval::totalMinutes() const
{
    return d_seconds / k_SECONDS_PER_MINUTE;
}

inline BSLS_KEYWORD_CONSTEXPR
bsls::Types::Int64 TimeInterval::totalSeconds() const
{
    return d_seconds;
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
bsls::Types::Int64 TimeInterval::totalMilliseconds() const
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_MILLISECS_PER_SEC >= d_seconds &&
                     LLONG_MIN / k_MILLISECS_PER_SEC <= d_seconds);
    BSLS_ASSERT_SAFE(isSumValidInt64(d_seconds * k_MILLISECS_PER_SEC,
                                     d_nanoseconds / k_NANOSECS_PER_MILLISEC));


    return d_seconds     * k_MILLISECS_PER_SEC
         + d_nanoseconds / k_NANOSECS_PER_MILLISEC;
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
bsls::Types::Int64 TimeInterval::totalMicroseconds() const
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_MICROSECS_PER_SEC >= d_seconds &&
                     LLONG_MIN / k_MICROSECS_PER_SEC <= d_seconds);
    BSLS_ASSERT_SAFE(isSumValidInt64(d_seconds     * k_MICROSECS_PER_SEC,
                                     d_nanoseconds / k_NANOSECS_PER_MICROSEC));

    return d_seconds     * k_MICROSECS_PER_SEC
         + d_nanoseconds / k_NANOSECS_PER_MICROSEC;
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
bsls::Types::Int64 TimeInterval::totalNanoseconds() const
{
    BSLS_ASSERT_SAFE(LLONG_MAX / k_NANOSECS_PER_SEC >= d_seconds &&
                     LLONG_MIN / k_NANOSECS_PER_SEC <= d_seconds);
    BSLS_ASSERT_SAFE(isSumValidInt64(d_seconds * k_NANOSECS_PER_SEC,
                                     d_nanoseconds));

    return d_seconds * k_NANOSECS_PER_SEC + d_nanoseconds;
}

#ifdef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS
template <class DURATION_TYPE>
inline
BSLS_KEYWORD_CONSTEXPR_CPP14
typename std::enable_if<
              TimeInterval_IsDuration<DURATION_TYPE>::value &&
              !TimeInterval_RepTraits<typename DURATION_TYPE::rep>::k_IS_FLOAT,
              DURATION_TYPE>::type
TimeInterval::asDuration() const
{
    using SecondsRatio = std::ratio<1>;
    using TimeIntervalSeconds =
                       std::chrono::duration<bsls::Types::Int64, SecondsRatio>;
    using TimeIntervalNanoseconds = std::chrono::duration<int, std::nano>;

    BSLS_ASSERT(isInDurationRange<DURATION_TYPE>());

    return (std::chrono::duration_cast<DURATION_TYPE>(TimeIntervalSeconds(
                                                                    d_seconds))
          + std::chrono::duration_cast<DURATION_TYPE>(TimeIntervalNanoseconds(
                                                              d_nanoseconds)));
}
#endif

inline
double TimeInterval::totalSecondsAsDouble() const
{
    return static_cast<double>(d_seconds) + d_nanoseconds /
                                       static_cast<double>(k_NANOSECS_PER_SEC);
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


#ifndef BDE_OPENSOURCE_PUBLICATION  // pending deprecation

// DEPRECATED METHODS
inline
int TimeInterval::maxSupportedBdexVersion()
{
    return maxSupportedBdexVersion(0);
}

#endif // BDE_OPENSOURCE_PUBLICATION -- pending deprecation
#ifndef BDE_OMIT_INTERNAL_DEPRECATED  // BDE2.22
inline
int TimeInterval::maxSupportedVersion()
{
    return maxSupportedBdexVersion(0);
}

template <class STREAM>
inline
STREAM& TimeInterval::streamOut(STREAM& stream) const
{
    return print(stream, 0, -1);
}

#endif  // BDE_OMIT_INTERNAL_DEPRECATED -- BDE2.22

}  // close package namespace

// FREE OPERATORS
inline
bsls::TimeInterval bsls::operator+(const TimeInterval& lhs,
                                   const TimeInterval& rhs)
{
    TimeInterval result(lhs);
    return result.addInterval(rhs.seconds(), rhs.nanoseconds());
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

#if defined(BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE)  && \
    defined(BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS)

inline BSLS_KEYWORD_CONSTEXPR_CPP14
bsls::TimeInterval bsls::TimeIntervalLiterals::operator"" _h(
                                                  unsigned long long int hours)
{
    BSLS_ASSERT((LLONG_MAX/3600) >= hours);
    return TimeInterval(static_cast<bsls::Types::Int64>(hours*3600), 0);
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
bsls::TimeInterval bsls::TimeIntervalLiterals::operator"" _min(
                                                unsigned long long int minutes)
{
    BSLS_ASSERT((LLONG_MAX/60) >= minutes);
    return TimeInterval(static_cast<bsls::Types::Int64>(minutes*60), 0);
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
bsls::TimeInterval bsls::TimeIntervalLiterals::operator"" _s(
                                                unsigned long long int seconds)
{
    BSLS_ASSERT(LLONG_MAX > seconds);
    return TimeInterval(static_cast<bsls::Types::Int64>(seconds), 0);
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
bsls::TimeInterval bsls::TimeIntervalLiterals::operator"" _ms(
                                           unsigned long long int milliseconds)
{
    const bsls::Types::Int64 k_MILLISECS_PER_SEC     = 1000;
    const bsls::Types::Int64 k_NANOSECS_PER_MILLISEC = 1000000;

    return TimeInterval(milliseconds / k_MILLISECS_PER_SEC,
                        static_cast<int>((milliseconds % k_MILLISECS_PER_SEC) *
                                         k_NANOSECS_PER_MILLISEC));
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
bsls::TimeInterval bsls::TimeIntervalLiterals::operator"" _us(
                                           unsigned long long int microseconds)
{
    const bsls::Types::Int64 k_MICROSECS_PER_SEC     = 1000000;
    const bsls::Types::Int64 k_NANOSECS_PER_MICROSEC = 1000;

    return TimeInterval(microseconds / k_MICROSECS_PER_SEC,
                        static_cast<int>((microseconds % k_MICROSECS_PER_SEC) *
                                         k_NANOSECS_PER_MICROSEC));
}

inline BSLS_KEYWORD_CONSTEXPR_CPP14
bsls::TimeInterval bsls::TimeIntervalLiterals::operator"" _ns(
                                            unsigned long long int nanoseconds)
{
    const bsls::Types::Int64 k_NANOSECS_PER_SEC = 1000000000;

    return TimeInterval(nanoseconds / k_NANOSECS_PER_SEC,
                        static_cast<int>(nanoseconds % k_NANOSECS_PER_SEC));
}

#endif  // BSLS_COMPILERFEATURES_SUPPORT_INLINE_NAMESPACE &&
        // BSLS_COMPILERFEATURES_SUPPORT_USER_DEFINED_LITERALS

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

#undef BSLS_TIMEINTERVAL_PROVIDES_CHRONO_CONVERSIONS

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
