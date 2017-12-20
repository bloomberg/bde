// bslmt_saturatedtimeconversionimputil.cpp                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bslmt_saturatedtimeconversionimputil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bslmt_saturatedtimeconversionimputil_cpp,"$Id$ $CSID$")

#include <bslmf_conditional.h>

#include <bsls_assert.h>

namespace BloombergLP {

// PRIVATE CONSTANTS
enum {
    k_NANOSEC_PER_MILLISEC = 1000000,
    k_MILLISEC_PER_SEC     = 1000
};

// PRIVATE CLASS METHODS
template <class TYPE>
static inline
TYPE maxOf(const TYPE&)
    // Return the maximum representable value of 'TYPE'
{
    return bsl::numeric_limits<TYPE>::max();
};

template <class TYPE>
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

// PUBLIC CLASS METHODS
void bslmt::SaturatedTimeConversionImpUtil::toTimeSpec(
                                                TimeSpec                  *dst,
                                                const bsls::TimeInterval&  src)
{
    BSLS_ASSERT(dst);

    enum { k_MAX_NANOSECONDS = 1000 * 1000 * 1000 - 1 };

    // In the test driver, in the test case "ASSERTS ABOUT 'TimeSpec' and
    // 'mach_timespec_t'" we verify that 'TimeSpec' can contain negative
    // values.

    toTimeTImp(&dst->tv_sec, src.seconds());

    dst->tv_nsec = src.seconds() > (bsls::Types::Int64) dst->tv_sec
                 ? k_MAX_NANOSECONDS
                 : src.seconds() < (bsls::Types::Int64) dst->tv_sec
                 ? -k_MAX_NANOSECONDS
                 : src.nanoseconds();
}

#ifdef BSLS_PLATFORM_OS_DARWIN

void bslmt::SaturatedTimeConversionImpUtil::toTimeSpec(
                                                mach_timespec_t           *dst,
                                                const bsls::TimeInterval&  src)
{
    BSLS_ASSERT(dst);

    enum { k_MAX_NANOSECONDS = 1000 * 1000 * 1000 - 1 };

    // 'dst->tv_sec' is unsigned, which is verified in the test driver in test
    // case "ASSERTS ABOUT 'TimeSpec' and 'mach_timespec_t'.".

    toTimeTImp(&dst->tv_sec, src.seconds());

    dst->tv_nsec = src.seconds() > (bsls::Types::Int64) dst->tv_sec
                 ? k_MAX_NANOSECONDS
                 : src.nanoseconds() < 0
                 ? 0
                 : src.nanoseconds();
}
#endif

void bslmt::SaturatedTimeConversionImpUtil::toTimeT(
                                                 bsl::time_t              *dst,
                                                 const bsls::Types::Int64  src)
{
    BSLS_ASSERT(dst);

    toTimeTImp(dst, src);
}

void bslmt::SaturatedTimeConversionImpUtil::toMillisec(
                                                unsigned int              *dst,
                                                const bsls::TimeInterval&  src)
{
    BSLS_ASSERT(dst);

    const int nanoMilliSeconds = src.nanoseconds() / k_NANOSEC_PER_MILLISEC;

    if (src.seconds() < 0 || (0 == src.seconds() && nanoMilliSeconds <= 0)) {
        *dst = 0;
    }
    else if (src.seconds() > maxOf(*dst)) {
        *dst = maxOf(*dst);
    }
    else {
        // 'src.seconds() < 2^32', therefore
        // 'src.seconds() * 1000 + 999 < 2^63', so this will work.  We also
        // know that 'src' is a positive time interval.

        toTimeTImp(dst, src.seconds() * 1000 + nanoMilliSeconds);
    }
}

void bslmt::SaturatedTimeConversionImpUtil::toMillisec(
                                                unsigned long             *dst,
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

void bslmt::SaturatedTimeConversionImpUtil::toMillisec(
                                                bsls::Types::Uint64       *dst,
                                                const bsls::TimeInterval&  src)
{
    BSLS_ASSERT(dst);

    typedef bsls::Types::Uint64 Uint64;

    const int nanoMilliSeconds = src.nanoseconds() / k_NANOSEC_PER_MILLISEC;
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

}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
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
