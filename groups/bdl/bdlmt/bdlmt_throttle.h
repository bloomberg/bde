// bdlmt_throttle.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLMT_THROTTLE
#define INCLUDED_BDLMT_THROTTLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism for throttling actions.
//
//@CLASSES:
//   bdlmt::Throttle: throttle mechanism
//
//@SEE_ALSO:
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component defines 'bdlmt::Throttle', a 'class' which can
// be used to regulate the frequency with which actions occur.
//
///Usage
///-----
// Usage example should demonstrate making use of 'bdlt::TimeUnitRatio' to
// calculate nanoseconds.


#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BDLT_TIMEUNITRATIO
#include <bdlt_timeunitratio.h>
#endif

#ifndef INCLUDED_BSLMF_ASSERT
#include <bslmf_assert.h>
#endif

#ifndef INCLUDED_BSLMF_INTEGRALCONSTANT
#include <bslmf_integralconstant.h>
#endif

#ifndef INCLUDED_BSLS_ATOMICOPERATIONS
#include <bsls_atomicoperations.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_SYSTEMCLOCKTYPE
#include <bsls_systemclocktype.h>
#endif

#ifndef INCLUDED_BSLS_SYSTEMTIME
#include <bsls_systemtime.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

#ifndef INCLUDED_BSL_CLIMITS
#include <bsl_climits.h>
#endif

namespace BloombergLP {
namespace bdlmt {

struct Throttle {
  private:
    // PRIVATE TYPES
    typedef bsls::Types::Int64              Int64;
    typedef bsls::AtomicOperations          AtomicOps;
    typedef AtomicOps::AtomicTypes          AtomicTypes;

  public:
    template <int MAX_BURST_SIZE, Int64 NANOSECONDS_PER_ACTION_LEAK>
    struct ArgFilter_Imp;

    template <int clockType>
    struct ClockTypeFilter_Imp;

    // PUBLIC DATA
    mutable AtomicTypes::Int64    d_prevLeakTime;
    Int64                         d_nanosecondsPerActionLeak;
    Int64                         d_nanosecondsPerTotalReset;
    int                           d_maxBurstSize;
    bsls::SystemClockType::Enum   d_clockType;

    // CLASS DATA
    static const Int64            s_tenYearsNanoseconds  = 10 * 366 *
                                    bdlt::TimeUnitRatio::k_NANOSECONDS_PER_DAY;
    static const Int64            s_allowAllNanoseconds  = LLONG_MIN;
    static const Int64            s_allowNoneNanoseconds = LLONG_MAX;

    // No CREATORS, private or public -- must be aggregate initiailizable, and
    // some compilers have bugs with regard to aggregate initializing objects
    // with creators declared.

    // MANIPULATORS
    void initialize(int                         maxBurstSize,
                    Int64                       nanosecondsPerActionLeak,
                    bsls::SystemClockType::Enum clockType =
                                           bsls::SystemClockType::e_MONOTONIC);
        // Initialize this object with the specified 'maxBurstSize',
        // 'nanosecondsPerActionLeak', where 'maxBurstSize' is the maximum
        // number of actions that can occur in a short time, and
        // 'nanosecondsPerActionLeak' is the time that must elapse after the
        // bucket is full before another single event will be permitted.
        // Optionally specify 'clockType' to indicate whether a real time or
        // monotonic clock should be employed.  If 'clockType' is not provided,
        // a monotonic clock should be used.  A value of 'maxBurstSize == 0'
        // will result in no actions being permitted, a value of
        // 'nanosecondsPerActionLeak == 0' will result in all actions being
        // permitted.  The behavior is undefined unless
        // '0 <= nanosecondsPerActionLeak', unless
        // '0 < maxBurstSize || 0 < nanosecondsPerActionLeak', and unless
        // 'maxBurstSize * nanosecondsPerActionLeak <= LLONG_MAX'.  Note that
        // this method is to be called at most once, and before any other
        // methods are called on this object.

    bool requestPermission();
    bool requestPermission(const bsls::TimeInterval& now);
    bool requestPermission(int                       numActions);
    bool requestPermission(int                       numActions,
                           const bsls::TimeInterval& now);
        // Return 'true' to indicate that the client is permitted to perform
        // the optionally specified 'numActions' actions and 'false' otherwise.
        // Optionally specifiy 'now' to be used it for the current time,
        // otherwise call the clock.  If 'numActions' is not specified a value
        // of 1 is used.  The behavior is undefined unless '0 < numActions',
        // and unless 'numActions <= d_maxBurstSize || 0 == d_maxBurstSize'
        // where 'd_maxBurstSize' is the 'maxBurstSize' value with which this
        // object was configured.

    int requestPermissionIfValid(bool                      *result,
                                 int                        numActions);
    int requestPermissionIfValid(bool                      *result,
                                 int                        numActions,
                                 const bsls::TimeInterval&  now);
        // Set the specified '*result' to 'true' to indicate that the client is
        // permitted to perform the specified 'numActions' actions and 'false'
        // otherwise.  Optionally specifiy 'now' to be used it for the current
        // time, otherwise call the clock.  Return 0 to indicate that
        // '0 < numActions' and 'numActions <= d_maxBurstSize' and a non-zero
        // value otherwise, where 'd_maxBurstSize' is the 'maxBurstSize' value
        // this object was configured with.  Note that unless 0 is returned,
        // '*result' is unaffected.

    // ACCESSOR
    int nextPermit(bsls::TimeInterval *result, int numActions) const;
        // If the specified 'numActions' is in the range
        // '0 < numActions <= d_maxBurstSize', where 'd_maxBurstSize' is the
        // 'maxBurstSize' with which this object was configured, set the
        // specified '*result' to the earliest absolute time when a burst of
        // 'numActions' actions will be permitted, and return 0, otherwise
        // return a non-zero value with no effect on '*result'.  Note that
        // '*result' may be set to a time in the past, and that this method
        // never calls the clock.
};

template <int MAX_BURST_SIZE, bsls::Types::Int64 NANOSECONDS_PER_ACTION_LEAK>
struct Throttle::ArgFilter_Imp {
    // Passing both args through this 'struct' achieves two goals:
    //: o It ensures that both are evaluated at compile time, which won't be
    //:   the case if either contains any floating point.
    //:
    //: o It enables us to do compile-time checks with 'BSLMF_ASSERT'.

    BSLMF_ASSERT(0 <= MAX_BURST_SIZE);
    BSLMF_ASSERT(0 <= NANOSECONDS_PER_ACTION_LEAK);
    BSLMF_ASSERT(MAX_BURST_SIZE || NANOSECONDS_PER_ACTION_LEAK);
    BSLMF_ASSERT(LLONG_MAX / (MAX_BURST_SIZE ? MAX_BURST_SIZE : 1) >=
                                                  NANOSECONDS_PER_ACTION_LEAK);

    static const Int64 value = 0 == MAX_BURST_SIZE
                             ? Throttle::s_allowNoneNanoseconds
                             : NANOSECONDS_PER_ACTION_LEAK
                             ? NANOSECONDS_PER_ACTION_LEAK
                             : Throttle::s_allowAllNanoseconds;
};

template <int clockType>
struct Throttle::ClockTypeFilter_Imp {
    // Passing 'clcokType' through this 'struct' achieves two goals:
    //: o It ensures that its value is computed at compile time.
    //:
    //: o It enables us to do a compile-time check of the validity of
    //:   'clockType'.

    BSLMF_ASSERT(bsls::SystemClockType::e_MONOTONIC == clockType ||
                 bsls::SystemClockType::e_REALTIME  == clockType);

    static const bsls::SystemClockType::Enum value =
                           static_cast<bsls::SystemClockType::Enum>(clockType);
};

//=============================================================================
//                               INLINE DEFINITIONS
//=============================================================================

// MANIPULATORS
inline
bool Throttle::requestPermission()
{
    return this->requestPermission(bsls::SystemTime::now(d_clockType));
}

inline
bool Throttle::requestPermission(int numActions)
{
    return this->requestPermission(numActions,
                                   bsls::SystemTime::now(d_clockType));
}

inline
int Throttle::requestPermissionIfValid(bool                      *result,
                                       int                        numActions)
{
    if (numActions <= 0 || d_maxBurstSize < numActions) {
        return -1;                                                    // RETURN
    }

    *result = this->requestPermission(numActions,
                                      bsls::SystemTime::now(d_clockType));
    return 0;
}

inline
int Throttle::requestPermissionIfValid(bool                      *result,
                                       int                        numActions,
                                       const bsls::TimeInterval&  now)
{
    if (numActions <= 0 || d_maxBurstSize < numActions) {
        return -1;                                                    // RETURN
    }

    *result = this->requestPermission(numActions, now);
    return 0;
}

//=============================================================================
//                              MACRO DEFINITIONS
//=============================================================================

// Aggregate initialization for a 'bdlmt::Throttle'.  The specified
// 'maxBurstSize' is to be an non-negative 'int' constant, the maximum number
// of actions permitted in a very short time.  The specified
// 'nanosecondsPerActionLeak' is to be a non-negative 'bsls::Types::Int64'
// constant indicating the minimum amount of time after 'maxBurstSize' actions
// have taken place before the next action will be permitted.  The specified
// 'clcokType' is a 'bsls::SystemClockType::Enum' indicating whether a
// monotonic or realtime clock to employed if the time is not passed to
// 'requestPermission'.  The behavior is undefined unless
// '0 <= nanosecondsPerActionLeak', unless
// '0 < maxBurstSize || 0 < nanosecondsPerActionLeak', unless
// 'maxBurstSize * nanosecondsPerActionLeak <= LLONG_MAX', unless 'clockType'
// is a valid 'bsls::SystemClockType::Enum' value, unless all of the arguments
// are compile-time constants.  Note that a value of '0 == maxBurstSize' will
// result in no actions being permitted, and a value of
// '0 == nanosecondsPerActionLeak' will result in all actions being permitted.

#define BDLMT_THROTTLE_INIT_RAW(maxBurstSize,                                 \
                                nanosecondsPerActionLeak,                     \
                                clockType) {                                  \
    -BloombergLP::bdlmt::Throttle::s_tenYearsNanoseconds,                     \
    BloombergLP::bdlmt::Throttle::ArgFilter_Imp<(maxBurstSize),               \
                                                (nanosecondsPerActionLeak)    \
                                                                    >::value, \
    bsl::integral_constant<BloombergLP::bsls::Types::Int64,                   \
                         (maxBurstSize) * (nanosecondsPerActionLeak)>::value, \
    bsl::integral_constant<int, (maxBurstSize)>::value,                       \
    BloombergLP::bdlmt::Throttle::ClockTypeFilter_Imp<(clockType)>::value     \
 }

#define BDLMT_THROTTLE_INIT_REALTIME(maxBurstSize,                            \
                                     nanosecondsPerActionLeak)                \
        BDLMT_THROTTLE_INIT_RAW((maxBurstSize),                               \
                                (nanosecondsPerActionLeak),                   \
                                bsls::SystemClockType::e_REALTIME)

#define BDLMT_THROTTLE_INIT_MONOTONIC(maxBurstSize,                           \
                                      nanosecondsPerActionLeak)               \
        BDLMT_THROTTLE_INIT_RAW((maxBurstSize),                               \
                                (nanosecondsPerActionLeak),                   \
                                bsls::SystemClockType::e_MONOTONIC)

#define BDLMT_THROTTLE_INIT(maxBurstSize,                                     \
                            nanosecondsPerActionLeak)                         \
        BDLMT_THROTTLE_INIT_RAW((maxBurstSize),                               \
                                (nanosecondsPerActionLeak),                   \
                                bsls::SystemClockType::e_MONOTONIC)

#define BDLMT_THROTTLE_INIT_ALLOW_ALL  BDLMT_THROTTLE_INIT(INT_MAX, 0)
#define BDLMT_THROTTLE_INIT_ALLOW_NONE BDLMT_THROTTLE_INIT(0,       1)

                        // ----------------------------
                        // 'BDLMT_THROTTLE_IF_*' macros
                        // ----------------------------

// The following macros throttle execution of the statement or block following
// them.  Note that they control the statement as an 'if', meaning that an
// 'else' clause can be used to catch rejected events.  The specified
// 'maxBurstSize' is to be a non-negative 'int' constant, and the specified
// 'nanosecondsPerActionLeak' is to be a non-negative 'bsls::Types::Int64'
// constant, the duration of each period.  If '0 == maxBurstSize', then no
// actions will be permitted, if '0 == nanosecondsPerActionLeak', then all
// actions will be permitted.  The behavior is undefined unless
// '0 <= maxBurstSize', unless '0 <= nanosecondsPerActionLeak', unless
// '0 < maxBurstSize || 0 < nanosecondsPerActionLeak', unless
// 'maxBurstSize * nanosecondsPerActionLeak <= LLONG_MAX', and unless all
// arguments are compile-time constants.

#define BDLMT_THROTTLE_IF_RAW(maxBurstSize,                                   \
                              nanosecondsPerActionLeak,                       \
                              clockType)                                      \
    if (bool bdlmt_throttle_iFsToP = false) {}                                \
    else                                                                      \
        for (static BloombergLP::bdlmt::Throttle bdlmt_throttle_iFtHrOtTlE =  \
                         BDLMT_THROTTLE_INIT_RAW((maxBurstSize),              \
                                                 (nanosecondsPerActionLeak),  \
                                                 (clockType));                \
             !bdlmt_throttle_iFsToP;                                          \
             bdlmt_throttle_iFsToP = true)                                    \
            if (bdlmt_throttle_iFtHrOtTlE.requestPermission())

#define BDLMT_THROTTLE_IF_MONOTONIC(maxBurstSize,                             \
                                    nanosecondsPerActionLeak)                 \
    BDLMT_THROTTLE_IF_RAW((maxBurstSize),                                     \
                          (nanosecondsPerActionLeak),                         \
                          bsls::SystemClockType::e_MONOTONIC)

#define BDLMT_THROTTLE_IF_REALTIME(maxBurstSize,                              \
                                   nanosecondsPerActionLeak)                  \
    BDLMT_THROTTLE_IF_RAW((maxBurstSize),                                     \
                          (nanosecondsPerActionLeak),                         \
                          bsls::SystemClockType::e_REALTIME)

#define BDLMT_THROTTLE_IF(maxBurstSize,                                       \
                          nanosecondsPerActionLeak)                           \
    BDLMT_THROTTLE_IF_RAW((maxBurstSize),                                     \
                          (nanosecondsPerActionLeak),                         \
                          bsls::SystemClockType::e_MONOTONIC)


#define BDLMT_THROTTLE_IF_ALLOW_ALL    BDLMT_THROTTLE_IF(INT_MAX, 0)
#define BDLMT_THROTTLE_IF_ALLOW_NONE   BDLMT_THROTTLE_IF(0,       1)

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
