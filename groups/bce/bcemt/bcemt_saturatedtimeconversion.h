// bcemt_saturatedtimeconversion.h                                    -*-C++-*-
#ifndef INCLUDED_BCEMT_SATURATEDTIMECONVERSION
#define INCLUDED_BCEMT_oSATURATEDTIMECONVERSION

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide saturating converssions for time values.
//
//@CLASSES:
//   bcemt_SaturatedTimeConversion: namespace for saturating time conversions.
//
//@SEE_ALSO:
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component defines a namespace containing static functions
// suitable for performing narrowing conversions of numbers such that if an
// attempt is made to assign a value that is outside the range that is
// representable by the destination variable, the destination variable will
// take on its maximum or minimum value, whichever is closer to the value being
// assigned.
//
///Usage
///-----
// Suppose we have a 'timespec' which is a popular way to store a time quantify
// on Unix.  It has two fields, 'tv_sec' which indicates seconds, and
// 'tv_nsec', which indicates nanonseconds.  The size of these fields can vary
// depending up on the platform for the purposes of our usage example they are
// both 32 bit signed values.
//..
//  timespec tm;
//  assert(sizeof(tm.tv_sec) == sizeof(int));   // tm.tv_sec (on this
//                                              // platform) is 'int' or 32
//                                              // bit 'long'
//  tm.tv_sec = -1;
//  assert(tm.tv_sec < 0);                      // 'tm.tv_sec' is signed on
//                                              // this platform.
//  tm.tv_nsec = -1;
//  ASSERTV(tm.tv_nsec < 0);                    // 'tm.tv_nsec' is signed
//                                              // on this platform.
//..
// First, we take a 'bdet_TimeInterval' representing a time that can be
// represented in a 'timespec'.  Our 'toTimeSpec' will convert one to the other
// without modification.
//..
//  bdet_TimeInterval ti(12345678, 987654321);
//
//  assert(ti.seconds()     ==  12345678);
//  assert(ti.nanoseconds() == 987654321);
//
//  bcemt_SaturatedTimeConversion::toTimeSpec(&tm, ti);
//
//  assert((bsls::Types::Int64) tm.tv_sec == ti.seconds());
//  assert(tm.tv_nsec                     == ti.nanoseconds());
//..
// Then, we set our time interval to a value to high to be represented by the
// 'timespec':
//..
//  bsls::Types::Int64 usageMaxInt = bsl::numeric_limits<int>::max();
//  ti.setInterval(usageMaxInt + 100000, 500 * 1000 * 1000);
//
//  assert(ti.seconds()     == usageMaxInt + 100000);
//  assert(ti.nanoseconds() == 500 * 1000 * 1000);
//..
// Next, we use 'toTimeSpec' to assign its value to 'tm':
//..
//  bcemt_SaturatedTimeConversion::toTimeSpec(&tm, ti);
//..
// Then, we observe that 'tm' has been 'saturated' -- it has been set to the
// highest value that a 'timespec' is capable of representing:
//..
//  assert(tm.tv_sec  == usageMaxInt);
//  assert(tm.tv_nsec == 999999999);
//..
// Now, we set our time interval to a value too low to be represented by a
// 'timespec':
//..
//  bsls::Types::Int64 usageMinInt = bsl::numeric_limits<int>::min();
//  ti.setInterval(usageMinInt - 100000, -500 * 1000 * 1000);
//
//  assert(ti.seconds()     == usageMinInt - 100000);
//  assert(ti.nanoseconds() == -500 * 1000 * 1000);
//..
// Finally, we do the conversion and observe that 'tm' has been saturated and
// now is set to the lowest value that it can represent:
//..
//  bcemt_SaturatedTimeConversion::toTimeSpec(&tm, ti);
//
//  assert(tm.tv_sec  == usageMinInt);
//  assert(tm.tv_nsec == -999999999);
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

#ifdef BSLS_PLATFORM_CPU_64_BIT
#define BCEMT_SATURATEDTIMECONVERSION_LONG_IS_64_BIT 1
#endif

#ifndef INCLUDED_BSL_CTIME
#include <bsl_ctime.h>
#endif

#endif

#ifdef BCES_PLATFORM_WIN32_THREADS

// 'long' is 32 bit on 64 bit Windows

#ifndef INCLUDED_WINDOWS
#include <windows.h>
#endif

#endif

namespace BloombergLP {

                             // ===================
                             // class bcemt_Barrier
                             // ===================

class bcemt_SaturatedTimeConversion {
    // Namespace class for the platform-dependent class methods 'toTimeSpec',
    // 'toTimeT', and 'toMillisec', used for saturating conversions of
    // times stored as 'bdet_TimeInterval' to other types used by underlying
    // threading platforms.

    // PRIVATE TYPE
    typedef bsls::Types::Int64 Int64;

    // COMPILE-TIME ASSERTS

#ifdef BCEMT_SATURATEDTIMECONVERSION_LONG_IS_64_BIT
    BSLMF_ASSERT(sizeof(long) == sizeof(Int64));
#else
    BSLMF_ASSERT(sizeof(long) == sizeof(int));
#endif

    BSLMF_ASSERT(sizeof(long long) == sizeof(Int64));
    BSLMF_ASSERT(sizeof(int)       <  sizeof(Int64));

    // PRIVATE CLASS METHODS
    template <typename TYPE>
    static TYPE maxOf(const TYPE& value);
        // Return the maximum value that can be represented by the type of the
        // specifired 'value'.

    template <typename TYPE>
    static TYPE minOf(const TYPE& value);
        // Return the minimum value that can be represented by the type of the
        // specifired 'value'.

    static void toTimeTImp(int                *dst, Int64 src);
    static void toTimeTImp(long               *dst, Int64 src);
    static void toTimeTImp(long long          *dst, Int64 src);
    static void toTimeTImp(unsigned int       *dst, Int64 src);
    static void toTimeTImp(unsigned long      *dst, Int64 src);
    static void toTimeTImp(unsigned long long *dst, Int64 src);
        // Assign to the specified '*dst' the value that it can represent that
        // is the closest possible value to the value of the specified 'src'.

  public:
    // CLASS METHODS
#ifdef BCES_PLATFORM_POSIX_THREADS
    template <typename TIMESPEC>
    static void toTimeSpec(TIMESPEC *dst, const bdet_TimeInterval& src);
        // Assign to the specified '*dst' the value that it can represent that
        // is the closest possible value to the value of the specified 'src'.
        // This is a template function to also accommodate alternatively named
        // timespec types such as 'mach_timespec_t' on Darwn.  The type
        // 'TIMESPEC' must be a 'struct' containing an integral 'tv_sec'
        // representing seconds, and 'tv_nsec' representing nanoseconds.  If
        // both fields constain non-zero value, they will both have the same
        // sign.

    static void toTimeT(bsl::time_t *dst, const bsls::Types::Int64 src);
        // Assign to the specified '*dst' the value that it can represent that
        // is the closest possible value to the value of the specified 'src'.
#endif

#ifdef BCES_PLATFORM_WIN32_THREADS
    static void toMillisec(DWORD *dst, const bdet_TimeInterval& src);
        // Assign to the specified '*dst' the value that it can represent that
        // is the closest possible value to the value of the specified 'src',
        // translated to milliseconds.
#endif
};

// ===========================================================================
//                        INLINE FUNCTION DEFINITIONS
// ===========================================================================

// PRIVATE CLASS METHODS
template <typename TYPE>
inline
TYPE bcemt_SaturatedTimeConversion::maxOf(const TYPE&)
{
    return bsl::numeric_limits<TYPE>::max();
};

template <typename TYPE>
inline
TYPE bcemt_SaturatedTimeConversion::minOf(const TYPE&)
{
    return bsl::numeric_limits<TYPE>::min();
};

inline
void bcemt_SaturatedTimeConversion::toTimeTImp(int                *dst,
                                               bsls::Types::Int64  src)
{
    *dst = src > maxOf(*dst) ? maxOf(*dst)
                             : (src < minOf(*dst) ? minOf(*dst) : (int) src);
}
inline
void bcemt_SaturatedTimeConversion::toTimeTImp(long               *dst,
                                               bsls::Types::Int64  src)
{
#ifdef BCEMT_SATURATEDTIMECONVERSION_LONG_IS_64_BIT
    *dst = src;
#else
    *dst = src > maxOf(*dst) ? maxOf(*dst)
                             : (src < minOf(*dst) ? minOf(*dst)
                                                  : (long) src);
#endif
}

inline
void bcemt_SaturatedTimeConversion::toTimeTImp(long long          *dst,
                                               bsls::Types::Int64  src)
{
    *dst = src;
}

inline
void bcemt_SaturatedTimeConversion::toTimeTImp(unsigned int       *dst,
                                               bsls::Types::Int64  src)
{
    *dst = src > maxOf(*dst) ? maxOf(*dst)
                             : (src < 0 ? 0 : (unsigned int) src);
}

inline
void bcemt_SaturatedTimeConversion::toTimeTImp(unsigned long      *dst,
                                               bsls::Types::Int64  src)
{
#ifdef BCEMT_SATURATEDTIMECONVERSION_LONG_IS_64_BIT
    *dst = src < 0 ? 0 : src;
#else
    *dst = src > maxOf(*dst) ? maxOf(*dst)
                             : (src < 0 ? 0 : (unsigned long) src);
#endif
}

inline
void bcemt_SaturatedTimeConversion::toTimeTImp(unsigned long long *dst,
                                               bsls::Types::Int64  src)
{
    *dst = src < 0 ? 0 : (unsigned long long) src;
}

#ifdef BCES_PLATFORM_POSIX_THREADS

template <typename TIMESPEC>
inline
void bcemt_SaturatedTimeConversion::toTimeSpec(TIMESPEC                 *dst,
                                               const bdet_TimeInterval&  src)
{
    enum { BILLION = 1000 * 1000 * 1000 };

    dst->tv_sec = -1;
    const bool secSigned = dst->tv_sec < 0;

    Int64 sec = src.seconds();
    toTimeTImp(&dst->tv_sec, sec);
    Int64 tvSec = dst->tv_sec;    // we want to compare 'tv_sec' to 'src',
                                  // convert to an 'Int64' to avoid weirdness
                                  // arising from compares with 'dst->tv_sec'
                                  // which has an unknown type

    if (secSigned) {
#ifdef BDE_BUILD_TARGET_SAFE
        // tv_nxec must also be signed

        dst->tv_nsec = -1;
        BSLS_ASSERT_SAFE(dst->tv_nsec < 0);
#endif

        dst->tv_nsec = sec > tvSec ? BILLION - 1
                                   : sec < tvSec ? - (BILLION - 1)
                                                 : src.nanoseconds();
    }
    else {
        // dst->tv_sec is unsigned.  Therefore, 'TIMESPEC' is not intended to
        // represent a negative quantity, so dst->nsec should never be -ve.
        // Note that 'bdet_TimeInterval' guarantees that 'src.seconds()' and
        // 'src.nanoseconds()' will never have opposite signs.

        dst->tv_nsec = sec > tvSec ? BILLION - 1
                                   : src.nanoseconds() < 0 ? 0
                                                           : src.nanoseconds();
    }
}

inline
void bcemt_SaturatedTimeConversion::toTimeT(bsl::time_t              *dst,
                                            const bsls::Types::Int64  src)
{
    toTimeTImp(dst, src);
}

#endif

#ifdef BCES_PLATFORM_WIN32_THREADS

inline
void bcemt_SaturatedTimeConversion::toMillisec(DWORD                    *dst,
                                               const bdet_TimeInterval&  src)
{
    enum { MILLION = 1000 * 1000 };

    BSLMF_ASSERT((bsl::is_same<DWORD, unsigned long>::value));
    BSLMF_ASSERT(sizeof(DWORD) == sizeof(int));     // 'long' is 4 bytes on
                                                    // windows, 32 or 64 bit
    BSLMF_ASSERT((DWORD) -1 > 0);

    const Int64 sec = src.seconds();
    const int nanoMilliSeconds = src.nanoseconds() / MILLION;

    if (sec > maxOf(*dst)) {
        *dst = maxOf(*dst);
    }
    else if (sec <= 0) {
        *dst = sec < 0 || nanoMilliSeconds < 0 ? 0 : nanoMilliSeconds;
    }
    else {
        // 'sec < 2^32', therefore 'sec * 1000 + 999 < 2^63', so this will
        // work.  We also know that 'sec >= 1', so 'nanoMilliSeconds >= 0'.

        toTimeTImp(dst, sec * 1000 + nanoMilliSeconds);
    }
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
