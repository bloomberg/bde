// bcemt_saturatedtimeconversionimputil.h                             -*-C++-*-
#ifndef INCLUDED_BCEMT_SATURATEDTIMECONVERSIONIMPUTIL
#define INCLUDED_BCEMT_SATURATEDTIMECONVERSIONIMPUTIL

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Conversions setting times out of a representable range to min/max.
//
//@CLASSES:
//   bcemt_SaturatedTimeConversionImpUtil: saturating time conversions.
//
//@SEE_ALSO:
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component defines a namespace containing static functions
// suitable for performing narrowing conversions of time-related types such
// that if an attempt is made to assign a value that is outside the range that
// is representable by the destination variable, the destination variable will
// take on its maximum or minimum value, whichever is closer to the value being
// assigned.
//
///Usage
///-----
// Suppose we need to assign a value held in a 'bdet_TimeInterval' to an
// 'unsigned int', where the 'unsigned int' is to contain an equilavent time
// interval expressed in milliseconds.  A 'bdet_TimeInterval' is able to
// represent intervals that are outside the range of intervals that can be
// represented by an 'unsigned int' number of milliseconds (e.g., any negative
// time interval).  'bcemt_SaturatedTimeConversionImpUtil' handles values
// outside the representable range of the destination type by "saturating",
// that is values outside the representable range of the destination type will
// be assigned the maximum or minimum representable value of the destination
// type (whichever is closer to the source value).
//
// First, we define variables of our source ('bdet_TimeInterval') and
// destination ('unsigned int') types:
//..
//  unsigned int destinationInterval;
//  bdet_TimeInterval sourceInterval;
//..
// Then, we try a value that does not require saturation and observe that
// 'toMillisec' converts it without modification (beyond loss of precision):
//..
//  sourceInterval.setInterval(4, 321000000);
//  bcemt_SaturatedTimeConversionImpUtil::toMillisec(
//                                       &destinationInterval, sourceInterval);
//  assert(4321 == destinationInterval);
//..
// Next, we calculate the maximum value that can be represented in an
// 'usngined int' number of milliseconds, and verify that converting an
// equivalent 'bdet_TimeInterval' does not modify the value:
//..
//  const unsigned int maxDestinationInterval =
//                                    bsl::numeric_limits<unsigned int>::max();
//  bdet_TimeInterval maxiumumTimeInterval(
//                              maxDestinationInterval / 1000,
//                              (maxDestinationInterval % 1000) * 1000 * 1000);
//  bcemt_SaturatedTimeConversionImpUtil::toMillisec(
//                                 &destinationInterval, maxiumumTimeInterval);
//  assert(maxDestinationInterval == destinationInterval);
//..
// Now, we attempt to convert a value greater than the maximum representable in
// an 'unsigned int' milliseconds and verify that the resulting value is the
// maximum representable 'unsigned int' value:
//..
//  bdet_TimeInterval aboveMaxInterval = maxiumumTimeInterval +
//                                           bdet_TimeInterval(0, 1000 * 1000);
//  bcemt_SaturatedTimeConversionImpUtil::toMillisec(
//                                     &destinationInterval, aboveMaxInterval);
//  assert(maxDestinationInterval == destinationInterval);
//..
// Finally, we try a value less than 0 and observe the result of the saturated
// conversion is 0 (the minimum representable value):
//..
//  bdet_TimeInterval belowMinimumInterval(-1, 0);
//  bcemt_SaturatedTimeConversionImpUtil::toMillisec(
//                                 &destinationInterval, belowMinimumInterval);
//  assert(0 == destinationInterval);
//..

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCES_PLATFORM
#include <bces_platform.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_ISSAME
#include <bslmf_issame.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_LIMITS
#include <bsl_limits.h>
#endif

#ifndef INCLUDED_BSL_CTIME
#include <bsl_ctime.h>
#endif

#ifdef BSLS_PLATFORM_OS_DARWIN

#ifndef INCLUDED_MACH_CLOCK_TYPES
#define INCLUDED_MACH_CLOCK_TYPES
#include <mach/clock_types.h>    // for 'mach_timespec_t'
#endif

#endif

#endif

namespace BloombergLP {

                 // ==========================================
                 // class bcemt_SaturatedTimeConversionImpUtil
                 // ==========================================

struct bcemt_SaturatedTimeConversionImpUtil {
    // This 'struct' provides a namespace for utility functions that convert
    // time values between different time representations, and "saturate" when
    // values are outside the range of values that may be represented in the
    // destination type (meaning that values above the maximum representable
    // value of the result type are set to the maximum value of the result
    // type, and values below the minimum representable value of the result
    // type are set to the minimum value fo the result type).

    // TYPES

    // Here we define type 'TimeSpec' -- an alias to 'timespec' on Unix, but
    // defined as a struct on Windows, to guarantee that 'TimeSpec' exists on
    // all platforms.

#ifdef BCES_PLATFORM_POSIX_THREADS
    typedef timespec TimeSpec;
#else
    struct TimeSpec {
        // Provide type for Windows platform

        bsls::Types::Int64 tv_sec;
        int                tv_nsec;
    };
#endif

    // CLASS METHODS

#if BSLS_PLATFORM_OS_DARWIN
    static void toTimeSpec(mach_timespec_t *dst, const bdet_TimeInterval& src);
#endif
    static void toTimeSpec(TimeSpec        *dst, const bdet_TimeInterval& src);
        // Assign to the specified 'dst' the value of the sepcified 'src', and
        // if 'src' is less than the lowest representable value of '*dst', set
        // '*dst' to the minimum value it can represent, and if 'src' is
        // greater than the highest representable value of '*dst', set '*dst'
        // to the maximum value that it can represent.

    static void toTimeT(bsl::time_t *dst, const bsls::Types::Int64 src);
        // Assign to the specified 'dst' the value of the sepcified 'src', and
        // if 'src' is less than the lowest representable 'time_t' value, set
        // 'dst' to the minimum 'time_t' value, and if 'src' is greater than
        // the highest representable 'time_t' value, set 'dst' to the maximum
        // 'time_t' value.

    static void toMillisec(unsigned int             *dst, 
                           const bdet_TimeInterval&  src);
    static void toMillisec(unsigned long            *dst, 
                           const bdet_TimeInterval&  src);
    static void toMillisec(bsls::Types::UInt64      *dst, 
                           const bdet_TimeInterval&  src);

        // Assign to the specified 'dst' the value of the specified 'src'
        // converted to milliseconds, and if that value is a negative time
        // interval, set 'dst' to 0, and if that value is greater than
        // the maximum representable value of 'dst' set 'dst' to its maximum
        // representable value.
};

//=============================================================================
//                        INLINE FUNCTION DEFINITIONS
//=============================================================================


}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
