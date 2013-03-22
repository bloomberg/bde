// bcemt_saturatedtimeconversion.cpp                                  -*-C++-*-
#include <bcemt_saturatedtimeconversion.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_barrier_cpp,"$Id$ $CSID$")

#ifdef BCES_PLATFORM_WIN32_THREADS
#include <windows.h>
#endif

#if defined(BCES_PLATFORM_POSIX_THREADS) && defined(BSLS_PLATFORM_CPU_64_BIT)
// Note 'long' is always 32 bit on 64 bit Windows.
#define BCEMT_SATURATEDTIMECONVERSION_LONG_IS_64_BIT 1
#endif

namespace BloombergLP {

#ifdef BCEMT_SATURATEDTIMECONVERSION_LONG_IS_64_BIT
BSLMF_ASSERT(sizeof(long) == sizeof(bsls::Types::Int64));
#else
BSLMF_ASSERT(sizeof(long) == sizeof(int));
#endif

BSLMF_ASSERT(sizeof(long long) == sizeof(bsls::Types::Int64));
BSLMF_ASSERT(sizeof(int)       <  sizeof(bsls::Types::Int64));

#ifdef BCES_PLATFORM_WIN32_THREADS

BSLMF_ASSERT((bsl::is_same<DWORD, unsigned long>::value));
BSLMF_ASSERT(sizeof(DWORD) == sizeof(int));     // 'long' is 4 bytes on
                                                // windows, 32 or 64 bit
BSLMF_ASSERT((DWORD) -1 > 0);

#endif

// PRIVATE CLASS METHODS
template <typename TYPE>
static inline
TYPE maxOf(const TYPE&)
    // Return the maximum representable value of 'TYPE'
{
    return bsl::numeric_limits<TYPE>::max();
};

template <typename TYPE>
static inline
TYPE minOf(const TYPE&)
    // Return the minimum representable value of 'TYPE'
{
    return bsl::numeric_limits<TYPE>::min();
};

static inline
void toTimeTImp(int *dst, bsls::Types::Int64 src)
    // Assign to the specified 'dst' the value of the sepcified 'src', and if
    // 'src' is less than the lowest representable 'time_t' value, set 'dst' to
    // the minimum 'time_t' value, and if 'src' is greater than the highest
    // representable 'time_t' value, set 'dst' to the maximum 'time_t' value.
{
    *dst = src > maxOf(*dst) ? maxOf(*dst)
                             : (src < minOf(*dst) ? minOf(*dst) : (int) src);
}

static inline
void toTimeTImp(long *dst, bsls::Types::Int64 src)
    // Assign to the specified 'dst' the value of the sepcified 'src', and if
    // 'src' is less than the lowest representable 'time_t' value, set 'dst' to
    // the minimum 'time_t' value, and if 'src' is greater than the highest
    // representable 'time_t' value, set 'dst' to the maximum 'time_t' value.
{
#ifdef BCEMT_SATURATEDTIMECONVERSION_LONG_IS_64_BIT
    *dst = src;
#else
    *dst = src > maxOf(*dst) ? maxOf(*dst)
                             : (src < minOf(*dst) ? minOf(*dst)
                                                  : (long) src);
#endif
}

static inline
void toTimeTImp(long long *dst, bsls::Types::Int64 src)
    // Assign to the specified 'dst' the value of the sepcified 'src', and if
    // 'src' is less than the lowest representable 'time_t' value, set 'dst' to
    // the minimum 'time_t' value, and if 'src' is greater than the highest
    // representable 'time_t' value, set 'dst' to the maximum 'time_t' value.
{
    *dst = src;
}

static inline
void toTimeTImp(unsigned int *dst, bsls::Types::Int64 src)
    // Assign to the specified 'dst' the value of the sepcified 'src', and if
    // 'src' is less than the lowest representable 'time_t' value, set 'dst' to
    // the minimum 'time_t' value, and if 'src' is greater than the highest
    // representable 'time_t' value, set 'dst' to the maximum 'time_t' value.
{
    *dst = src > maxOf(*dst) ? maxOf(*dst)
                             : (src < 0 ? 0 : (unsigned int) src);
}

static inline
void toTimeTImp(unsigned long *dst, bsls::Types::Int64 src)
    // Assign to the specified 'dst' the value of the sepcified 'src', and if
    // 'src' is less than the lowest representable 'time_t' value, set 'dst' to
    // the minimum 'time_t' value, and if 'src' is greater than the highest
    // representable 'time_t' value, set 'dst' to the maximum 'time_t' value.
{
#ifdef BCEMT_SATURATEDTIMECONVERSION_LONG_IS_64_BIT
    *dst = src < 0 ? 0 : src;
#else
    *dst = src > maxOf(*dst) ? maxOf(*dst)
                             : (src < 0 ? 0 : (unsigned long) src);
#endif
}

static inline
void toTimeTImp(unsigned long long *dst, bsls::Types::Int64 src)
    // Assign to the specified 'dst' the value of the sepcified 'src', and if
    // 'src' is less than the lowest representable 'time_t' value, set 'dst' to
    // the minimum 'time_t' value, and if 'src' is greater than the highest
    // representable 'time_t' value, set 'dst' to the maximum 'time_t' value.
{
    *dst = src < 0 ? 0 : (unsigned long long) src;
}

// PUBLIC CLASS METHODS
void bcemt_SaturatedTimeConversion::toTimeSpec(TimeSpec                 *dst,
                                               const bdet_TimeInterval&  src)
{
    enum { MAX_NANOSECONDS = 1000 * 1000 * 1000 - 1 };

#ifdef BDE_BUILD_TARGET_SAFE
    // On Windows, 'tv_sec' is a signed int.  On POSIX, it should be a
    // 'time_t', which should be some sort of signed int, but I can't find
    // anybody guaranteeing that.  Make sure.

    dst->tv_sec  = -1;
    BSLS_ASSERT_SAFE(dst->tv_sec  < 0);
    dst->tv_nsec = -1;
    BSLS_ASSERT_SAFE(dst->tv_nsec < 0);
#endif

    toTimeTImp(&dst->tv_sec, src.seconds());

    dst->tv_nsec = src.seconds() > (bsls::Types::Int64) dst->tv_sec
                 ? MAX_NANOSECONDS
                 : src.seconds() < (bsls::Types::Int64) dst->tv_sec
                 ? -MAX_NANOSECONDS
                 : src.nanoseconds();
}

#ifdef BSLS_PLATFORM_OS_DARWIN
void bcemt_SaturatedTimeConversion::toTimeSpec(mach_timespec_t          *dst,
                                               const bdet_TimeInterval&  src)
{
    enum { MAX_NANOSECONDS = 1000 * 1000 * 1000 - 1 };

    // 'dst->tv_sec' is unsigned, which may change.

    dst->tv_sec = -1;
    const bool secSigned = dst->tv_sec < 0;

    toTimeTImp(&dst->tv_sec, srcseconds());

    if (secSigned) {
#ifdef BDE_BUILD_TARGET_SAFE
        // tv_nsec must also be signed

        dst->tv_nsec = -1;
        BSLS_ASSERT_SAFE(dst->tv_nsec < 0);
#endif

        dst->tv_nsec = src.seconds() > (bsls::Types::Int64) dst->tv_sec
                     ? MAX_NANOSECONDS
                     : src.seconds() < (bsls::Types::Int64) dst->tv_sec
                     ? -MAX_NANOSECONDS
                     : src.nanoseconds();
    }
    else {
        // dst->tv_sec is unsigned.  Therefore, 'TIMESPEC' is not intended to
        // represent a negative quantity, so dst->nsec should never be -ve.
        // Note that 'bdet_TimeInterval' guarantees that 'src.seconds()' and
        // 'src.nanoseconds()' will never have opposite signs.

        dst->tv_nsec = src.seconds() > (bsls::Types::Int64) dst->tv_sec
                     ? MAX_NANOSECONDS
                     : src.nanoseconds() < 0
                     ? 0
                     : src.nanoseconds();
    }
}
#endif

void bcemt_SaturatedTimeConversion::toTimeT(bsl::time_t              *dst,
                                            const bsls::Types::Int64  src)
{
    toTimeTImp(dst, src);
}

void bcemt_SaturatedTimeConversion::toMillisec(unsigned int             *dst,
                                               const bdet_TimeInterval&  src)
{
    enum { MILLION = 1000 * 1000 };

    const int nanoMilliSeconds = src.nanoseconds() / MILLION;

    if (src.seconds() > maxOf(*dst)) {
        *dst = maxOf(*dst);
    }
    else if (src.seconds() < 0 ||
                               (0 == src.seconds() && nanoMilliSeconds <= 0)) {
        *dst = 0;
    }
    else {
        // 'sec < 2^32', therefore 'sec * 1000 + 999 < 2^63', so this will
        // work.  We also know that 'sec >= 1', so 'nanoMilliSeconds >= 0'.

        toTimeTImp(dst, src.seconds() * 1000 + nanoMilliSeconds);
    }
}

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
