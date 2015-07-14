// bdlmtt_saturatedtimeconversionimputil.cpp                           -*-C++-*-
#include <bdlmtt_saturatedtimeconversionimputil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlmtt_saturatedtimeconversionimputil_cpp,"$Id$ $CSID$")

#include <bslmf_conditional.h>

namespace BloombergLP {

// PRIVATE CONSTANTS
enum {
    NANOSEC_PER_MILLISEC = 1000000,
    MILLISEC_PER_SEC     = 1000
};


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
void toTimeTImp(unsigned long long *dst, bsls::Types::Int64 src)
    // Assign to the specified 'dst' the value of the sepcified 'src', and if
    // 'src' is less than the lowest representable 'time_t' value, set 'dst' to
    // the minimum 'time_t' value, and if 'src' is greater than the highest
    // representable 'time_t' value, set 'dst' to the maximum 'time_t' value.
{
    *dst = src < 0 ? 0 : (unsigned long long) src;
}

static inline
void toTimeTImp(long *dst, bsls::Types::Int64 src)
    // Assign to the specified 'dst' the value of the sepcified 'src', and if
    // 'src' is less than the lowest representable 'time_t' value, set 'dst' to
    // the minimum 'time_t' value, and if 'src' is greater than the highest
    // representable 'time_t' value, set 'dst' to the maximum 'time_t' value.
{
    typedef bsl::conditional<sizeof(int) == sizeof(long),
                            int,
                            bsls::Types::Int64>::type LongAlias;


    LongAlias result;

    toTimeTImp(&result, src);

    *dst = result;
}

static inline
void toTimeTImp(unsigned long *dst, bsls::Types::Int64 src)
    // Assign to the specified 'dst' the value of the sepcified 'src', and if
    // 'src' is less than the lowest representable 'time_t' value, set 'dst' to
    // the minimum 'time_t' value, and if 'src' is greater than the highest
    // representable 'time_t' value, set 'dst' to the maximum 'time_t' value.
{
    typedef bsl::conditional<sizeof(unsigned int) == sizeof(unsigned long),
                             unsigned int,
                             bsls::Types::Uint64>::type UnsignedLongAlias;

    UnsignedLongAlias result;

    toTimeTImp(&result, src);

    *dst = result;
}

namespace bdlmtt {
// PUBLIC CLASS METHODS
void SaturatedTimeConversionImpUtil::toTimeSpec(
                                                 TimeSpec                 *dst,
                                                 const bsls::TimeInterval&  src)
{
    BSLS_ASSERT(dst);

    enum { MAX_NANOSECONDS = 1000 * 1000 * 1000 - 1 };

    // In the test driver, in the test case "ASSERTS ABOUT 'TimeSpec' and
    // 'mach_timespec_t'" we verify that 'TimeSpec' can contain negative
    // values.

    toTimeTImp(&dst->tv_sec, src.seconds());

    dst->tv_nsec = src.seconds() > (bsls::Types::Int64) dst->tv_sec
                 ? MAX_NANOSECONDS
                 : src.seconds() < (bsls::Types::Int64) dst->tv_sec
                 ? -MAX_NANOSECONDS
                 : src.nanoseconds();
}
}  // close package namespace

#ifdef BSLS_PLATFORM_OS_DARWIN

namespace bdlmtt {void SaturatedTimeConversionImpUtil::toTimeSpec(
                                                 mach_timespec_t          *dst,
                                                 const bsls::TimeInterval&  src)
{
    BSLS_ASSERT(dst);

    enum { MAX_NANOSECONDS = 1000 * 1000 * 1000 - 1 };

    // 'dst->tv_sec' is unsigned, which is verified in the test driver in test
    // case "ASSERTS ABOUT 'TimeSpec' and 'mach_timespec_t'.".

    toTimeTImp(&dst->tv_sec, src.seconds());

    dst->tv_nsec = src.seconds() > (bsls::Types::Int64) dst->tv_sec
                 ? MAX_NANOSECONDS
                 : src.nanoseconds() < 0
                 ? 0
                 : src.nanoseconds();
}
}  // close package namespace
#endif

namespace bdlmtt {
void SaturatedTimeConversionImpUtil::toTimeT(
                                                 bsl::time_t              *dst,
                                                 const bsls::Types::Int64  src)
{
    BSLS_ASSERT(dst);

    toTimeTImp(dst, src);
}

void SaturatedTimeConversionImpUtil::toMillisec(
                                                 unsigned int             *dst,
                                                 const bsls::TimeInterval&  src)
{
    BSLS_ASSERT(dst);

    const int nanoMilliSeconds = src.nanoseconds() / NANOSEC_PER_MILLISEC;

    if (src.seconds() < 0 || (0 == src.seconds() && nanoMilliSeconds <= 0)) {
        *dst = 0;
    }
    else if (src.seconds() > maxOf(*dst)) {
        *dst = maxOf(*dst);
    }
    else {
        // 'src.seconds() < 2^32', therefore
        // 'src.seconds() * 1000 + 999 < 2^63', so this will work.  We also
        // know that 'src' is a poitive time interval.

        toTimeTImp(dst, src.seconds() * 1000 + nanoMilliSeconds);
    }
}

void SaturatedTimeConversionImpUtil::toMillisec(
                                                 unsigned long            *dst,
                                                 const bsls::TimeInterval&  src)
{
    BSLS_ASSERT_SAFE(dst);

    typedef bsl::conditional<sizeof(unsigned int) == sizeof(unsigned long),
                             unsigned int,
                             bsls::Types::Uint64>::type UnsignedLongAlias;

    UnsignedLongAlias result;

    toMillisec(&result, src);

    *dst = result;
}

void SaturatedTimeConversionImpUtil::toMillisec(
                                                 bsls::Types::Uint64      *dst,
                                                 const bsls::TimeInterval&  src)
{
    BSLS_ASSERT(dst);

    typedef bsls::Types::Uint64 Uint64;

    const int nanoMilliSeconds = src.nanoseconds() / NANOSEC_PER_MILLISEC;
    const Uint64 MAX_UINT64 = maxOf(*dst);
    const Uint64 MAX_SEC    = MAX_UINT64 / 1000;
    const Uint64 MAX_MILLI_FOR_MAX_SEC = 
                                      MAX_UINT64 - ((MAX_UINT64)/1000) * 1000;

    if (src.seconds() < 0 || (0 == src.seconds() && nanoMilliSeconds <= 0)) {
        *dst = 0;
    }
    else if ((Uint64)src.seconds() < MAX_SEC ||
               ((Uint64)src.seconds() == MAX_SEC &&
                (Uint64)nanoMilliSeconds < MAX_MILLI_FOR_MAX_SEC))  {
        *dst = (bsls::Types::Uint64)src.seconds() * 1000 + nanoMilliSeconds;
    }
    else {
        *dst = maxOf(*dst);
    }
}
}  // close package namespace

}  // close namespace BloombergLP

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
