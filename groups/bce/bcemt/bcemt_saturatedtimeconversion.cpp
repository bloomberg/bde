// bcemt_saturatedtimeconversion.cpp                                  -*-C++-*-
#include <bcemt_saturatedtimeconversion.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bcemt_barrier_cpp,"$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_types.h>

#include <bsl_climits.h>

template <typename TYPE>
inline static
TYPE maxOf(const TYPE&) {
    return bsl::numeric_limits<TYPE>::max();
}

template <typename TYPE>
inline static
TYPE minOf(const TYPE&) {
    return bsl::numeric_limits<TYPE>::min();
}

namespace BloombergLP {

#ifdef BCES_PLATFORM_POSIX_THREADS

void bcemt_SaturatedTimeConversion::toTimeSpec(timespec                 *dst,
                                               const bdet_TimeInterval&  src)
{
    BSLS_ASSERT_SAFE(minOf(dst->tv_nsec) <= -999999999);

    dst->tv_sec  = src.seconds() > maxOf(dst->tv_sec)
                 ? maxOf(dst->tv_sec)
                 : src.seconds() < minOf(dst->tv_sec)
                 ? minOf(dst->tv_sec)
                 : src.seconds();
    dst->tv_nsec = src.nanoseconds();
}

void bcemt_SaturatedTimeConversion::toTimeT(bsl::time_t              *dst,
                                            const bsls::Types::Int64  src)
{
    *dst = src > maxOf(*dst)
         ? maxOf(*dst)
         : src < minOf(*dst)
         ? minOf(*dst)
         : src;
}

#endif

#ifdef bces_platform_WIN32_THREADS

#if 0

// This is the slower, but more perfect imp (note that divide is a very slow
// operation on some platforms).

void bcemt_SaturatedTimeConversion::toMillesec(DWORD                    *dst,
                                               const bdet_TimeInterval&  src)
{
    enum { MILLION = 1000 * 1000 };

    BSLS_ASSERT_SAFE(0 == minOf(*dst));

    const bsls::Types::Int64 sec = src.seconds();
    const int nanoMilliSeconds = src.nanoseconds() / MILLION;
    const unsigned maxSec = (maxOf(*dst) - nanoMilliSeconds) / 1000;

    *dst = src > maxSec
         ? maxOf(*dst)
         : sec <= 0
         ? (sec < 0 || nanoMilliSeconds < 0 ? 0 : nanoMilliSeconds)
         : sec * 1000 + nanoMilliSeconds;
}

#else

// This is riskier because the calculation of 'milliSec' might overflow
// and wrap, but it's much faster.  The likelihood of an Int64 overflowing
// is remote, even in milliseconds -- any value below 2e18 years (+1970) is
// safe, even in milliseconds.

void bcemt_SaturatedTimeConversion::toMillisec(DWORD                    *dst,
                                               const bdet_TimeInterval&  src)
{
    enum { MILLION = 1000 * 1000 };

    BSLS_ASSERT_SAFE(0 == minOf(*dst));

    bsls::Types::Int64 milliSec = src.seconds() * 1000 +
				  src.nanoseconds() / MILLION;

    *dst = millSec > maxOf(*dst)
	 ? maxOf(*dst)
	 : milliSec < 0
	 ? 0
	 : milliSec;
}

#endif

#endif

} // namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
