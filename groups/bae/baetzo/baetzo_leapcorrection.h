// baetzo_leapcorrection.h                                            -*-C++-*-
#ifndef INCLUDED_BAETZO_LEAPCORRECTION
#define INCLUDED_BAETZO_LEAPCORRECTION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide the number of accumulated leap seconds for a UTC time.
//
//@CLASSES:
//  baetzo_LeapCorrection: number of accumulated leap seconds for a UTC time
//
//@AUTHOR: Stefano Pacifico (spacifico1), Henry Verschell (hverschell)
//
//@SEE_ALSO: baetzo_zoneinfo
//
//@DESCRIPTION: This component provides a single, simply constrained
// (value-semantic) attribute class, 'baetzo_LeapCorrection', that is used to
// represent the accumulated number of leap seconds incorporated into a UTC
// time.
//
///Attributes
///----------
//..
//  Name                            Type           Default
//  ------------------------------  -------------  ----------
//  utcTime                         bdet_Datetime  epoch-time
//  accumulatedCorrectionInSeconds  int            0
//
//  Constraints
//  ------------------------------------------------------------------
//  'utcTime                        >= bdetu_Epoch::epoch()'
//  'accumulatedCorrectionInSeconds =< utcTime - bdetu_Epoch::epoch()'
//..
//: o utcTime: UTC date-time incorporating 'accumulatedCorrectionInSeconds'
//:   (i.e., accumulated leap seconds)
//:
//: o accumulatedCorrectionInSeconds: total number of leap-seconds that have
//:   been accumulated since January 1, 1970.
//
// Leap seconds are adjustments that are occasionally applied to keep UTC
// (Coordinated Universal Time) in sync with "mean solar time", where mean
// solar time is the time implied by Earth's motion.  Leap seconds are required
// because the definition of an SI (International System of Units) second
// results in a SI day that is (very slightly) less the time it takes the Earth
// to complete a rotation about its axis.  A 'baetzo_LeapCorrection' provides
// the cumulative total of leap seconds that have been applied to UTC between
// January 1, 1970 and the leap-correction's 'utcTime' attribute value (leap
// seconds were first introduced in 1972).  This correction value allows
// clients to accurately compute the actual number of seconds between two UTC
// times.  As of January 1, 2011 00:00 UTC, there were 24 accumulated leap
// seconds in UTC.  Note that TAI (International Atomic Time) -- a time scale
// strictly determined by the elapsed number of SI seconds as determined by an
// atomic clock -- is 'accumulatedCorrectionInSeconds() + 10' seconds ahead of
// UTC (the additional 10 seconds comes from initial difference between UTC and
// TAI at the start of 1972).
//
// Accounting for leap-seconds is a degree of accuracy that is not required, or
// even possible, in most applications.  Unless there is an explicit need for
// leap-correction information, clients are encouraged not to use it.  Note
// that, in many cases, system clocks are automatically updated to account for
// leap seconds, e.g., using the Network Time Protocol (NTP), or are not set
// with second accuracy to begin with.
//
///Usage
///-----
// In this section we demonstrate how to use a series of leap-corrections to
// more accurately determine the actual time interval between two UTC times.
//
///Example 1: Accurately Computing the Whole Seconds Between Two UTC Times
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// A common pattern for using 'baetzo_LeapCorrection' objects is to maintain an
// ordered sequence of corrections (sorted by 'utcTime'), where each correction
// indicates the start of a time interval where the associated leap-correction
// is applicable (and the end of that interval is indicated by the 'utcTime' of
// the subsequent leap correction in the sequence).
//
// First, we define a helper function 'determineLeapSeconds':
//..
//  int determineLeapSeconds(const bdet_Datetime&         utcTime,
//                           const baetzo_LeapCorrection *corrections,
//                           int                          numCorrections)
//      // Return the number of leap seconds that have been incorporated into
//      // to the specified 'utcTime', using the ordered sequence of specified
//      // 'corrections' of at least length 'numCorrections'.  The behavior is
//      // undefined unless 'corrections' contains at least the specified
//      // 'numCorrections', where each correction's 'utcTime' is less than the
//      // subsequents correction's 'utcTime', and each correction's 'utcTime'
//      // indicates the start of a time interval where the associated
//      // 'accumulatedCorrectionInSeconds' is applicable (and the end of that
//      // interval is indicated by the subsequent correction's 'utcTime').
//  {
//..
// We now construct a temporary 'baetzo_LeapCorrection' object that we will use
// to perform a binary search of 'corrections'.
//..
//      baetzo_LeapCorrection searchValue(utcTime, 0);
//..
// Next, we use 'upper_bound' to find the leap-correction that applies to
// 'utcTime' Note that 'baetzo_LeapCorrection' provides 'lessUtcTime', a static
// class method that returns 'true' if the first object's 'utcTime' attribute
// is less than the second's.
//..
//      const baetzo_LeapCorrection *result =
//                       bsl::upper_bound(corrections,
//                                        corrections + numCorrections,
//                                        searchValue,
//                                        &baetzo_LeapCorrection::lessUtcTime);
//..
// Note that either 'upper_bound' or 'lower_bound' will return the correction
// immediately after the correction that applies to 'utcTime' in the interior
// of the range.  We use 'upper_bound' because, in the unlikely case that the
// search value has the same 'utcTime' as an element in the sequence, it will
// return an iterator *after* that element (rather than referring to that
// element), so we do not need to explicitly handle that edge case.
//..
//      if (result == corrections) {
//          // 'utcTime' precedes all corrections.
//
//          return 0;                                                 // RETURN
//      }
//
//      --result;
//
//      return result->accumulatedCorrectionInSeconds();
//  }
//..
// Now, using the 'determineLeapSeconds' method, we can easily define a new
// function 'elapsedTime' that accurately determines the number of seconds
// between two UTC values:
//..
// bdet_DatetimeInterval elapsedTime(
//                               bdet_Datetime                startUtcTime,
//                               bdet_Datetime                endUtcTime,
//                               const baetzo_LeapCorrection *corrections,
//                               int                          numCorrections)
//     // Return the elapsed time between the specified 'startUtcTime' and the
//     // specified 'endUtcTime', using the specified ordered-sequence of
//     // 'numCorrections' 'corrections' to account for leap seconds.  The
//     // behavior is undefined unless 'startUtcTime <= endUtcTime',
//     // 'corrections' contains at least the specified 'numCorrections'
//     // objects, each correction's 'utcTime' is less than every subsequent
//     // correction's 'utcTime', and each correction's 'utcTime' indicates the
//     // start of a time period where the associated
//     // 'accumulatedCorrectionInSeconds' is in effect (and the inclusive end
//     // of that interval is indicated by the subsequent correction's
//     // 'utcTime').
// {
//     assert(startUtcTime <= endUtcTime);
//
//     bdet_DatetimeInterval interval = endUtcTime - startUtcTime;
//..
// We use the 'determineLeapSeconds' methods to find the number of leap-seconds
// that have been added between 'startUtcTime' and 'endUtcTime':
//..
//      int startCorrectionSec = determineLeapSeconds(startUtcTime,
//                                                    corrections,
//                                                    numCorrections);
//      int endCorrectionSec   = determineLeapSeconds(endUtcTime,
//                                                    corrections,
//                                                    numCorrections);
//      int numLeapSeconds     = endCorrectionSec - startCorrectionSec;
//..
// Finally, we adjust the computed 'interval' by the number of leap-seconds
// that have been added:
//..
//      interval.addSeconds(numLeapSeconds);
//      return interval;
//  }
//..
// To exercise our 'elapsedTime' function, we obtain an historical record:
//..
//  struct {
//      int d_year;
//      int d_month;
//      int d_day;
//      int d_correction;
//  } CORRECTIONS[] = {
//      { 1972, 1, 1,  0 },
//      { 1972, 7, 1,  1 },
//      { 1973, 1, 1,  2 },
//      { 1974, 1, 1,  3 },
//      { 1975, 1, 1,  4 },
//      { 1976, 1, 1,  5 },
//      { 1977, 1, 1,  6 },
//      { 1978, 1, 1,  7 },
//      { 1979, 1, 1,  8 },
//      { 1980, 1, 1,  9 },
//      { 1981, 7, 1, 10 },
//      { 1982, 7, 1, 11 },
//      { 1983, 7, 1, 12 },
//      { 1985, 7, 1, 13 },
//      { 1988, 1, 1, 14 },
//      { 1990, 1, 1, 15 },
//      { 1991, 1, 1, 16 },
//      { 1992, 7, 1, 17 },
//      { 1993, 7, 1, 18 },
//      { 1994, 7, 1, 19 },
//      { 1996, 1, 1, 20 },
//      { 1997, 7, 1, 21 },
//      { 1999, 1, 1, 22 },
//      { 2006, 1, 1, 23 },
//      { 2009, 1, 1, 24 }
//  };
//  const int SIZE = sizeof CORRECTIONS / sizeof *CORRECTIONS;
//..
// The historical record is used to create the requisite "array" of
// 'baetzo_LeapCorrection' objects.  In this example, a 'bsl::vector' is used
// as a dynamic array:
//..
//  bsl::vector<baetzo_LeapCorrection> corrections;
//
//  for (int ci = 0; ci < SIZE; ++ci) {
//      corrections.push_back(baetzo_LeapCorrection(
//                                       bdet_Datetime(CORRECTIONS[ci].d_year,
//                                                     CORRECTIONS[ci].d_month,
//                                                     CORRECTIONS[ci].d_day),
//                                       CORRECTIONS[ci].d_correction));
//  }
//..
// We can then compare the values returned from our 'elapsedTime' function with
// values obtained from inspection of the historical data.  For an interval
// that spans the entire history of leap-seconds, we expect the correction to
// match the largest accumulated offset (i.e., 24).
//..
//  bdet_Datetime start1(1970, 1, 1);
//  bdet_Datetime   end1(2010, 1, 1);
//
//  bdet_DatetimeInterval uncorrectedInterval1 = end1 - start1;
//  bdet_DatetimeInterval  calculatedInterval1 = elapsedTime(
//                                                     start1,
//                                                     end1,
//                                                     &corrections.front(),
//                                                     corrections.size());
//  assert(uncorrectedInterval1 + bdet_DatetimeInterval(0, 0, 0, 24)
//       == calculatedInterval1);
//..
// For an interval spanning the last three leap-seconds, we expect the
// correction to match the number of leap-seconds in that period (i.e., 3).
//..
//  bdet_Datetime start2(1998, 1, 1);
//  bdet_Datetime   end2(2010, 1, 1);
//
//  bdet_DatetimeInterval uncorrectedInterval2 = end2 - start2;
//  bdet_DatetimeInterval  calculatedInterval2 = elapsedTime(
//                                                        start2,
//                                                        end2,
//                                                        &corrections.front(),
//                                                        corrections.size());
//  assert(uncorrectedInterval2 + bdet_DatetimeInterval(0, 0, 0, 3)
//       == calculatedInterval2);
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BDET_DATETIME
#include <bdet_datetime.h>
#endif

#ifndef INCLUDED_BDETU_EPOCH
#include <bdetu_epoch.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSL_ALGORITHM
#include <bsl_algorithm.h>
#endif

#ifndef INCLUDED_BSL_CTIME
#include <bsl_ctime.h>
#endif

#ifndef INCLUDED_BSL_IOSFWD
#include <bsl_iosfwd.h>
#endif

namespace BloombergLP {

                            // ===========================
                            // class baetzo_LeapCorrection
                            // ===========================

class baetzo_LeapCorrection {
    // This simply constrained (value-semantic) attribute class describes the
    // accumulated number of leap seconds incorporated into a UTC time.  See
    // the Attributes section under @DESCRIPTION in the component-level
    // documentation.  This class contains one one attribute, 'utcTime', of
    // type 'bdet_DateTime'.  In addition to the intrinsic constraints of that
    // type, this also class requires 'bdetu_Epoch::epoch() <= utcTime'.
    //
    // This class:
    //: o supports a complete set of *value* *semantic* operations
    //:   o except for 'bdex' serialization
    //: o is *exception-neutral*
    //: o is *alias-safe*
    //: o is 'const' *thread-safe*
    // For terminology see 'bsldoc_glossary'.
    //
    // Note that a common use for this type is to create a ordered sequence of
    // leap-correction values where each correction indicates the start of a
    // time interval where the associated leap-correction is applicable (and
    // the inclusive end of that interval would be indicated by the 'utcTime'
    // of the subsequent leap correction in the sequence).

    // DATA
    bdet_Datetime d_utcTime;            // UTC time to which this correction
                                        // applies

    int           d_accumulatedCorrectionInSeconds;
                                        // number of accumulated leap-seconds
  public:

    // CLASS METHODS
    static bool lessUtcTime(const baetzo_LeapCorrection& first,
                            const baetzo_LeapCorrection& second);
        // Return 'true' if the specified 'first' leap correction's 'utcTime'
        // is less than that of the specified 'second' leap correction's one,
        // and 'false' otherwise.

    // CREATORS
    baetzo_LeapCorrection();
        // Create a 'baetzo_LeapCorrection' object having the (default)
        // attribute values:
        //: o 'utcTime()                      == bdetu_Epoch::epoch()'
        //: o 'accumulatedCorrectionInSeconds == 0'

    baetzo_LeapCorrection(const bdet_Datetime& utcTime,
                          int                  accumulatedCorrectionInSeconds);
        // Create a 'baetzo_LeapCorrection' object having the specified
        // 'utcTime' and 'accumulatedCorrectionInSeconds' attribute values.
        // The behavior is undefined unless 'bdetu_Epoch::epoch() <= utcTime'.

    baetzo_LeapCorrection(const baetzo_LeapCorrection& original);
        // Create a 'baetzo_LeapCorrection' object with the same value as the
        // specified 'original'.

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
    // The following destructor is generated by the compiler, except in "SAFE"
    // build modes (e.g., to enable the checking of class invariants).

    ~baetzo_LeapCorrection();
        // Destroy this object.
#endif

    // MANIPULATORS
    baetzo_LeapCorrection& operator=(const baetzo_LeapCorrection& rhs);
        // Assign to this object the value of the specified 'rhs' object and
        // return a reference providing modifiable access to this object.

    void setUtcTime(const bdet_Datetime& value);
        // Set the 'utcTime' attribute to the specified 'value'.  The behavior
        // is undefined unless 'bdetu_Epoch::epoch() <= utcTime'.

    void setAccumulatedCorrection(int value);
        // Set the 'accumulatedCorrectionInSeconds' attribute to the specified
        // 'value' (seconds).

    void swap(baetzo_LeapCorrection& other);
        // Efficiently exchange the value of this object with the value of the
        // specified 'other' object.  This method provides the no-throw
        // exception-safety guarantee.

    // ACCESSORS
    const bdet_Datetime& utcTime() const;
        // Return a reference providing non-modifiable access to the 'utcTime'
        // attribute of this object.

    int accumulatedCorrectionInSeconds() const;
        // Return the value of the 'accumulatedCorrectionInSeconds' attribute
        // of this object.

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
bool operator==(const baetzo_LeapCorrection& lhs,
                const baetzo_LeapCorrection& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects have the same
    // value, and 'false' otherwise.  Two 'baetzo_LeapCorrection' objects have
    // the same value if the corresponding values of their 'utcTime' and
    // 'accumulatedCorrectionInSeconds' attributes are the same.

bool operator!=(const baetzo_LeapCorrection& lhs,
                const baetzo_LeapCorrection& rhs);
    // Return 'true' if the specified 'lhs' and 'rhs' objects do not have the
    // same value, and 'false' otherwise.  Two 'baetzo_LeapCorrection' objects
    // do not have the same value if the corresponding values of their
    // 'utcTime' or 'accumulatedCorrectionInSeconds' attributes are not the
    // same.

bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baetzo_LeapCorrection& object);
    // Write the value of the specified 'object' to the specified output
    // 'stream' in a single-line format, and return a reference to 'stream'.
    // If 'stream' is not valid on entry, this operation has no effect.  Note
    // that this human-readable format is not fully specified and can change
    // without notice.  Also note that this method has the same behavior as
    // 'object.print(stream, 0, -1)'.

// FREE FUNCTIONS
void swap(baetzo_LeapCorrection& a, baetzo_LeapCorrection& b);
    // Efficiently exchange the values of the specified 'a' and 'b' objects.
    // This function provides the no-throw exception-safety guarantee.

// ============================================================================
//                      INLINE FUNCTION DEFINITIONS
// ============================================================================

                          // ---------------------------
                          // class baetzo_LeapCorrection
                          // ---------------------------

// CLASS METHODS
inline
bool baetzo_LeapCorrection::lessUtcTime(const baetzo_LeapCorrection& first,
                                        const baetzo_LeapCorrection& second)
{
    return first.utcTime() < second.utcTime();
}

// CREATORS
inline
baetzo_LeapCorrection::baetzo_LeapCorrection()
: d_utcTime(bdetu_Epoch::epoch())
, d_accumulatedCorrectionInSeconds(0)
{
}

inline
baetzo_LeapCorrection::baetzo_LeapCorrection(
                           const bdet_Datetime& utcTime,
                           int                  accumulatedCorrectionInSeconds)
: d_utcTime(utcTime)
, d_accumulatedCorrectionInSeconds(accumulatedCorrectionInSeconds)
{
    BSLS_ASSERT_SAFE(bdetu_Epoch::epoch() <= utcTime);
}

inline
baetzo_LeapCorrection::baetzo_LeapCorrection(
                                         const baetzo_LeapCorrection& original)
: d_utcTime(original.d_utcTime)
, d_accumulatedCorrectionInSeconds(original.d_accumulatedCorrectionInSeconds)
{
}

#if defined(BSLS_ASSERT_SAFE_IS_ACTIVE)
inline
baetzo_LeapCorrection::~baetzo_LeapCorrection()
{
    BSLS_ASSERT_SAFE(bdetu_Epoch::epoch() <= d_utcTime);
}
#endif

// MANIPULATORS
inline
baetzo_LeapCorrection& baetzo_LeapCorrection::operator=(
                                              const baetzo_LeapCorrection& rhs)
{
    d_utcTime                        = rhs.d_utcTime;
    d_accumulatedCorrectionInSeconds = rhs.d_accumulatedCorrectionInSeconds;
    return *this;
}

inline
void baetzo_LeapCorrection::setUtcTime(const bdet_Datetime& value)
{
    BSLS_ASSERT_SAFE(bdetu_Epoch::epoch() <= value);

    d_utcTime = value;
}

inline
void baetzo_LeapCorrection::setAccumulatedCorrection(int value)
{
    d_accumulatedCorrectionInSeconds = value;
}

inline
void baetzo_LeapCorrection::swap(baetzo_LeapCorrection& other)
{
    using bsl::swap;
    swap(d_utcTime, other.d_utcTime);
    swap(d_accumulatedCorrectionInSeconds,
         other.d_accumulatedCorrectionInSeconds);
}

// ACCESSORS
inline
const bdet_Datetime& baetzo_LeapCorrection::utcTime() const
{
    return d_utcTime;
}

inline
int baetzo_LeapCorrection::accumulatedCorrectionInSeconds() const
{
    return d_accumulatedCorrectionInSeconds;
}

inline
bool operator==(const baetzo_LeapCorrection& lhs,
                const baetzo_LeapCorrection& rhs)
{
    return lhs.utcTime()                        == rhs.utcTime()
        && lhs.accumulatedCorrectionInSeconds() ==
                                          rhs.accumulatedCorrectionInSeconds();
}

inline
bool operator!=(const baetzo_LeapCorrection& lhs,
                const baetzo_LeapCorrection& rhs)
{
    return !(lhs == rhs);
}

inline
bsl::ostream& operator<<(bsl::ostream&                stream,
                         const baetzo_LeapCorrection& object)
{
    return object.print(stream, 0, -1);
}

// FREE FUNCTIONS
inline
void swap(baetzo_LeapCorrection& a, baetzo_LeapCorrection& b)
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
