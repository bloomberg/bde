// bcemt_saturatedtimeconversion.h                                    -*-C++-*-
#ifndef INCLUDED_BCEMT_SATURATEDTIMECONVERSION
#define INCLUDED_BCEMT_SATURATEDTIMECONVERSION

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
// 'unsigned int', where the 'unsigned int' is to contain the time in
// milliseconds.  A 'bdet_TimeInterval' is able to represent many values that
// cannot be represented in such an 'unsigned int', and what we want to do in
// the event of such values is 'saturation', that is, the value assigned will
// be the maximum or minimum value the destination can represent, whichever is
// closer to the value that should be assigned.
//..
//  typedef bcemt_SaturatedTimeConversion STC;
//  typedef bsls::Types::Int64            Int64;
//
//  unsigned int dest;
//  bdet_TimeInterval timeInt;
//..
// First, we try a value that does not require saturation and observe that
// 'toMillisec' converts it properly:
//..
//  timeInt.setInterval(4, 321000000);
//  STC::toMillisec(&dest, timeInt);
//  assert(4321 == dest);
//..
// Then, we calculate the max legal value that can be stored and verify that it
// translates to the maximum value:
//..
//  const unsigned int maxDest = bsl::numeric_limits<unsigned int>::max();
//  bdet_TimeInterval borderTimeInt(maxDest / 1000,
//                                  (maxDest % 1000) * 1000 * 1000);
//  STC::toMillisec(&dest, borderTimeInt);
//  assert(maxDest == dest);
//..
// Next, we translate a value that should translate to 'maxDest - 1':
//..
//  timeInt = borderTimeInt - bdet_TimeInterval(0, 1000 * 1000);
//  STC::toMillisec(&dest, timeInt);
//  assert(maxDest - 1 == dest);
//..
// Now, we try values higher than 'borderTimeInt' and observe saturation:
//..
//  timeInt = borderTimeInt + bdet_TimeInterval(0, 1000 * 1000);
//  STC::toMillisec(&dest, timeInt);
//  assert(maxDest == dest);
//
//  timeInt.setInterval(bsl::numeric_limits<Int64>::max(), 999999999);
//  STC::toMillisec(&dest, timeInt);
//  assert(maxDest == dest);
//
//  timeInt.setInterval(1000 * 1000 * 1000, 0);
//  STC::toMillisec(&dest, timeInt);
//  assert(maxDest == dest);
//..
// Finally, we try some negative values and observe our result is saturated to
// 0:
//..
//  timeInt.setInterval(0, - 1000 * 1000);
//  STC::toMillisec(&dest, timeInt);
//  assert(0 == dest);
//
//  timeInt.setInterval(-1000, 0);
//  STC::toMillisec(&dest, timeInt);
//  assert(0 == dest);
//
//  timeInt.setInterval(-1000 * 1000 * 1000, -999 * 1000 * 1000);
//  STC::toMillisec(&dest, timeInt);
//  assert(0 == dest);
//
//  timeInt.setInterval(bsl::numeric_limits<Int64>::min(), -999999999);
//  STC::toMillisec(&dest, timeInt);
//  assert(0 == dest);
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

#ifdef BCES_PLATFORM_POSIX_THREADS

#ifndef INCLUDED_BSL_CTIME
#include <bsl_ctime.h>
#endif

#endif

namespace BloombergLP {

                 // ==========================================
                 // class bcemt_SaturatedTimeConversionImpUtil
                 // ==========================================

class bcemt_SaturatedTimeConversion {
    // Namespace class for the platform-dependent class methods 'toTimeSpec',
    // 'toTimeT', and 'toMillisec', used for saturating conversions of
    // times stored as 'bdet_TimeInterval' to other types used by underlying
    // threading platforms.

  public:
    // PUBLIC TYPES

    // Here we define type 'TimeSpec' -- an alias to 'timespec' on Unix, but
    // defined as a struct on Windows, to guarantee that 'TimeSpec' exists on
    // all platforms.

#ifdef BCES_PLATFORM_POSIX_THREADS
    typedef timespec TimeSpec;
#else
    struct TimeSpec {
        // Provide type for Windows platform

        int tv_sec;
        int tv_nsec;
    };
#endif

  public:
    // CLASS METHODS
    static void toTimeSpec(TimeSpec *dst, const bdet_TimeInterval& src);
        // Assign to the specified 'dst' the value of the sepcified 'src', and
        // if 'src' is less than the lowest representable 'Timespec' value, set
        // 'dst' to the minimum 'Timespec' value, and if 'src' is greater than
        // the highest representable 'Timespec' value, set 'dst' to the maximum
        // 'Timespec' value.

#if BSLS_PLATFORM_OS_DARWIN
    static void toTimeSpec(mach_timespec_t *dst, const bdet_TimeInterval& src);
        // Assign to the specified 'dst' the value of the sepcified 'src', and
        // if 'src' is less than the lowest representable 'Timespec' value, set
        // 'dst' to the minimum 'Timespec' value, and if 'src' is greater than
        // the highest representable 'Timespec' value, set 'dst' to the maximum
        // 'Timespec' value.
#endif

    static void toTimeT(bsl::time_t *dst, const bsls::Types::Int64 src);
        // Assign to the specified 'dst' the value of the sepcified 'src', and
        // if 'src' is less than the lowest representable 'time_t' value, set
        // 'dst' to the minimum 'time_t' value, and if 'src' is greater than
        // the highest representable 'time_t' value, set 'dst' to the maximum
        // 'time_t' value.

    static void toMillisec(unsigned int *dst, const bdet_TimeInterval& src);
        // Assign to the specified '*dst' the value that it can represent that
        // is the closest possible value to the value of the specified 'src',
        // translated to milliseconds.

#ifdef BCES_PLATFORM_WIN32_THREADS
    static void toMillisec(unsigned long *dst, const bdet_TimeInterval& src);
        // Assign to the specified '*dst' (which is of type
        // 'DWORD == unsigned long' on Windows) the value that it can represent
        // that is the closest possible value to the value of the specified
        // 'src', translated to milliseconds.
#endif
};

//=============================================================================
//                        INLINE FUNCTION DEFINITIONS
//=============================================================================

#ifdef BCES_PLATFORM_WIN32_THREADS

inline
void bcemt_SaturatedTimeConversion::toMillisec(unsigned long            *dst,
                                               const bdet_TimeInterval&  src)
{
    // 'long' is always 32-bit on Windows, in 32 or 64 bit executables.

    BSLMF_ASSERT(sizeof(unsigned int) == sizeof(unsigned long));

    // See other asserts about 'DWORD' type in imp file.

    toMillisec((unsigned int *) dst, src);
}

#endif

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
