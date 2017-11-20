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
//@MACROS:
//   BDLMT_THROTTLE_INIT, BDLMT_THROTTLE_INIT_REALTIME,
//   BDLMT_THROTTLE_INIT_ALLOW_ALL, BDLMT_THROTTLE_INIT_ALLOW_NONE,
//   BDLMT_THROTTLE_IF, BDLMT_THROTTLE_IF_REALTIME,
//   BDLMT_THROTTLE_IF_ALLOW_ALL, BDLMT_THROTTLE_IF_ALLOW_NONE
//
//@SEE_ALSO:
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides a mechanism, 'bdlmt::Throttle', that
// can be used by clients to regulate the frequency at which actions can be
// taken.  Clients initialize a 'Throttle' with configuration values for
// 'nanosecondsPerAction' and 'maxBurstSize'.  Then clients request permission
// from this component to execute actions.  The component keeps track of the
// number of actions requested, and over time throttles the average number of
// actions permitted to a rate of '1 / nanosecondsPerAction'
// (actions-per-nanosecond).  So, for example, to limit the average rate of
// actions permitted to 10 actions per second (10 actions / one billion
// nanoseconds), the value for 'nanosecondsPerAction' would be 100000000 (which
// is one billion / 10).
//
// As clients request permission to perform actions the component accumulates a
// time debt for those actions that dissipates over time.  The maximum value
// for this time debt is given by 'maxBurstSize * nanosecondsPerAction'.  The
// 'maxBurstSize' configuration parameter thereby limits the maximum number of
// actions that can be simultaneously permitted.
//
// This behavior is known as a "leaky-bucket" algorithm: actions permitted
// place water in the bucket, the passage of time drains water from the bucket,
// and the bucket has a maximum capacity.  Actions are permitted when there is
// enough empty room in the bucket that the water placed won't overflow it.  A
// leaky bucket is an efficiently implementable approximation for allowing a
// certain number of actions over a window of time.
//
///Supported Clock-Types
///---------------------
// The component 'bsls::SystemClockType' supplies the enumeration indicating
// the system clock by which this component measures time.  There are two clock
// types, real time and monotonic, where real time time is an absolute offset
// since 00:00:00 UTC, January 1, 1970 (which matches the epoch used in
// 'bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME)'.  The default
// clock type used is 'bsls::SystemClockType::e_MONOTONIC', the timeout should
// be expressed as an absolute offset since the epoch of this clock (which
// matches the epoch used in
// 'bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC)', usually the
// absolute time since system boot time.
//
///Thread Safety
///-------------
// 'bdlmt::Throttle' is fully *thread-safe*, meaning that all
// non-initialization operations on a given instance instance can be safely
// invoked simultaneously from multiple threads.
//
///Static 'Throttle' Objects
///-------------------------
// 'Throttle' objects declared with static storage duration must be initialized
// using one of the 'BDLMT_THROTTLE_INIT*' macros.  In order to provide thread
// safety on C++03 compilers (which do not have 'constexpr'), these macros
// perform aggregate initialization that can be evaluated at compile time.
//
///Macro Reference
///---------------
//
//                      // ---------------------------
//                      // BLDMT_THROTTLE_INIT* macros
//                      // ---------------------------
//
// One of these macros must be used to aggregate initialize 'bdlmt::Throttle'
// objects that have static storage duration -- the values are guaranteed to be
// evaluated at compile-time, avoiding race conditions.
//
//   BDLMT_THROTTLE_INIT(maxBurstSize,
//                       nanosecondsPerAction)
//       Initialize this 'Throttle' to limit the average period of actions
//       permitted to the specified 'nanosecondsPerAction', and the maximum
//       number of actions allowed in a very short time to the specified
//       'maxBurstSize', where time is measured according to the monotonic
//       system clock.  If 'maxBurstSize' is 0, the throttle will be
//       configured to permit no actions.  If 'nanosecondsPerAction' is 0, the
//       throttle will be configured to permit all actions.  The behavior is
//       undefined unless '0 <= maxBurstSize', '0 <= nanosecondsPerAction',
//       '0 < maxBurstSize || 0 < nanosecondsPerAction', and
//       'maxBurstSize * nanosecondsPerAction <= LLONG_MAX'.  Note that
//       floating-point expressions are not allowed in any of the arguments, as
//       they cannot be evaluated at compile-time on some platforms.
//
//   BDLMT_THROTTLE_INIT_REALTIME(maxBurstSize,
//                                nanosecondsPerAction)
//       Initialize this 'Throttle' to limit the average period of actions
//       permitted to the specified 'nanosecondsPerAction', and the maximum
//       number of actions allowed in a very short time to the specified
//       'maxBurstSize', where time is measured according to the realtime
//       system clock.  If 'maxBurstSize' is 0, the throttle will be
//       configured to permit no actions.  If 'nanosecondsPerAction' is 0, the
//       throttle will be configured to permit all actions.  The behavior is
//       undefined unless '0 <= maxBurstSize', '0 <= nanosecondsPerAction',
//       '0 < maxBurstSize || 0 < nanosecondsPerAction', and
//       'maxBurstSize * nanosecondsPerAction <= LLONG_MAX'.  Note that
//       floating-point expressions are not allowed in any of the arguments, as
//       they cannot be evaluated at compile-time on some platforms.
//
//   BDLMT_THROTTLE_INIT_ALLOW_ALL
//       Initialize this 'Throttle' to allow all actions.
//
//   BDLMT_THROTTLE_INIT_ALLOW_NONE
//       Initialize this 'Throttle' to allow no actions.
//
//                          // -------------------------
//                          // BDLMT_THROTTLE_IF* macros
//                          // -------------------------
//
//   BDLMT_THROTTLE_IF(maxBurstSize,
//                     nanosecondsPerAction)
//       Create a throttled 'if' statement limiting execution of the statement
//       following it (and controlling execution of any 'else' clause present).
//       Limit the average period of actions permitted to the specified
//       'nanosecondsPerAction', and the maximum number of actions allowed in a
//       very short time to the specified 'maxBurstSize', where time is
//       measured according to the monotonic system clock.  If 'maxBurstSize'
//       is 0, the 'if' will be configured to permit no actions.  If
//       'nanosecondsPerAction' is 0, the 'if' will be configured to permit all
//       actions.  The behavior is undefined unless '0 <= maxBurstSize',
//       '0 <= nanosecondsPerAction',
//       '0 < maxBurstSize || 0 < nanosecondsPerAction', and
//       'maxBurstSize * nanosecondsPerAction <= LLONG_MAX'.  Note that
//       floating-point expressions are not allowed in any of the arguments, as
//       they cannot be evaluated at compile-time on some platforms.
//
//   BDLMT_THROTTLE_IF_REALTIME(maxBurstSize,
//                              nanosecondsPerAction)
//       Create a throttled 'if' statement limiting execution of the statement
//       following it (and controlling execution of any 'else' clause present).
//       Limit the average period of actions permitted to the specified
//       'nanosecondsPerAction', and the maximum number of actions allowed in a
//       very short time to the specified 'maxBurstSize', where time is
//       measured according to the realtime system clock.  If 'maxBurstSize' is
//       0, the 'if' will be configured to permit no actions.  If
//       'nanosecondsPerAction' is 0, the 'if' will be configured to permit all
//       actions.  The behavior is undefined unless '0 <= maxBurstSize',
//       '0 <= nanosecondsPerAction',
//       '0 < maxBurstSize || 0 < nanosecondsPerAction', and
//       'maxBurstSize * nanosecondsPerAction <= LLONG_MAX'.  Note that
//       floating-point expressions are not allowed in any of the arguments, as
//       they cannot be evaluated at compile-time on some platforms.
//
//   BDLMT_THROTTLE_IF_ALLOW_ALL
//       Create an 'if' statement whose condition is always 'true', always
//       allowing execution of the statement controlled by it and never
//       allowing execution of any 'else' clause present.
//
//   BDLMT_THROTTLE_IF_ALLOW_NONE
//       Create an 'if' statement whose condition is always 'false', never
//       allowing execution of the statement controlled by it and always
//       allowing execution of any 'else' clause present.
//
///Usage
///-----
// Ensure the usage example makes use of bdlt::TimeUnitRatio.

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

class Throttle {
    // This class provides a mechanism that can be used by clients to regulate
    // the frequency at which actions can be taken.  The data members of
    // 'Throttle' are currently public to allow for compile-time (aggregate)
    // initialization of 'Throttle' objects having static storage duration (for
    // C++03 compilers that do not provide 'constexpr').

    // PRIVATE TYPES
    typedef bsls::Types::Int64              Int64;
    typedef bsls::AtomicOperations          AtomicOps;
    typedef AtomicOps::AtomicTypes          AtomicTypes;

    // PRIVATE CONSTANTS
    static const Int64          k_ALLOW_ALL  = LLONG_MIN;
    static const Int64          k_ALLOW_NONE = LLONG_MAX;

  public:
    // PUBLIC CONSTANTS
    static const Int64          k_TEN_YEARS_NANOSECONDS = 10 * 366 *
                                    bdlt::TimeUnitRatio::k_NANOSECONDS_PER_DAY;

    // PUBLIC DATA
    AtomicTypes::Int64          d_prevLeakTime;
    Int64                       d_nanosecondsPerAction;
    Int64                       d_nanosecondsPerTotalReset;
    int                         d_maxBurstSize;
    bsls::SystemClockType::Enum d_clockType;

  private:
    // FRIENDS
    template <int MAX_BURST_SIZE, bsls::Types::Int64 NANOSECONDS_PER_ACTION>
    friend struct Throttle_ArgFilter_Imp;

  public:
    // MANIPULATORS
    void initialize(int                         maxBurstSize,
                    Int64                       nanosecondsPerAction,
                    bsls::SystemClockType::Enum clockType =
                                           bsls::SystemClockType::e_MONOTONIC);
        // Initialize this 'Throttle' to limit the average period of actions
        // permitted to the specified 'nanosecondsPerAction', and the maximum
        // number of actions allowed in a very short timespan to
        // 'maxBurstSize'.  Optionally specify 'clockType' to indicate which
        // system clock will be used to measure time.  If 'clockType' is not
        // supplied the monotonic system clock is used.  The configured
        // throttle will over time limit the average number of actions
        // permitted to a rate of '1 / nanosecondsPerAction'.  If
        // 'maxBurstSize' is 0, the throttle will be configured to permit no
        // actions, otherwise if 'nanosecondsPerAction' is 0, the throttle will
        // be configured to permit all actions.  The behavior is undefined
        // unless '0 <= nanosecondsPerAction', '0 <= maxBurstSize', unless
        // '0 < nanosecondsPerAction || 0 < maxBurstSize', and unless
        // 'maxBurstSize * nanosecondsPerActionLeak <= LLONG_MAX'.  Note that
        // the behavior for other methods is undefined unless this 'Throttle'
        // is initialized (either using this function, or a
        // 'BDLMT_THROTTLE_INIT' macro) prior to being called.

    bool requestPermission();
    bool requestPermission(const bsls::TimeInterval& now);
    bool requestPermission(int                       numActions);
    bool requestPermission(int                       numActions,
                           const bsls::TimeInterval& now);
        // Return 'true' if the time debt incurred by taking the indicated
        // action(s) would *not* exceed the maximum allowed time debt
        // configured for this 'Throttle' object
        // ('nanosecondsPerAction * maxBurstSize'), and 'false' otherwise.
        // Optionally specificy 'now' indicating the current time of the system
        // clock for which this object is configured ('now' is a offset from
        // that clocks epoch).  If 'now' is not supplied, the current time is
        // obtained from the configured system clock.  Optionally specify
        // 'numActions' indicating the number of actions requested.  If
        // 'numActions' is not supplied, one action is requested.  If this
        // function returns 'true' then 'numActions * nanosecondsPerActions' is
        // added to the time debt accumulated by this component.  The behavior
        // is undefined unless this throttle has been initialized (either by
        // calling 'initialize' or using one of the 'BDLMT_THROTTLE_INIT*'
        // macros), '0 < numActions', and
        // 'numActions <= maxBurstSize || 0 == maxBurstSize'.  Note that
        // 'requestPerimissionIfValid', unlike these methods, does not have any
        // preconditions on the value of 'numActions'.

    int requestPermissionIfValid(bool                      *result,
                                 int                        numActions);
    int requestPermissionIfValid(bool                      *result,
                                 int                        numActions,
                                 const bsls::TimeInterval&  now);
        // Set '*result' to 'true' if the time debt incurred by taking the
        // specified 'numActions' would *not* exceed the maximum allowed time
        // debt configured for this 'Throttle' object
        // ('nanosecondsPerAction * maxBurstSize'), and 'false' otherwise.
        // Optionally specificy 'now' indicating the current time of the system
        // clock for which this object is configured ('now' is a offset from
        // that clocks epoch).  If 'now' is not supplied, the current time is
        // obtained from the configured system clock.  If '*result' is set to
        // 'true' then 'numActions * nanosecondsPerActions' is added to the
        // time debt accumulated by this component.  Return 0 if
        // '0 <= numActions' and 'numActions <= maxBurstSize' or
        // '0 == maxBurstSize', and a non-zero value otherwise.  The behavior
        // is undefined unless this throttle has been initialized (either by
        // calling 'initialize' or using one of the 'BDLMT_THROTTLE_INIT*'
        // macros).  Note that unless 0 is returned, '*result' is unaffected.

    // ACCESSOR
    int maxBurstSize() const;
        // Return the 'maxBurstSize' value with which this object was
        // configured.

    Int64 nanosecondsPerAction() const;
        // Return the 'nanosecondsPerAction' value with which this object was
        // configured.

    int nextPermit(bsls::TimeInterval *result, int numActions) const;
        // Load into the specified 'result' the earliest *absolute* *time*
        // (according to system clock configured at initialization) when the
        // specified 'numActions' will next be permitted.  Return 0 on success,
        // and a non-zero value (with no effect on 'result') if this throttle
        // is configured such that 'numActions' will never be permitted (i.e.,
        // return an error if 'numActions > maxBurstSize').  The returned
        // 'result' is an offset from the epoch of the system clock for which
        // this throttle is configured.  The behavior is undefined unless this
        // throttle has been initialized (either by calling 'initialize' or
        // using a 'BDLMT_THROTTLE_INIT*' macro).  Note that 'result' may be in
        // the past, and this function does *not* obtain the current time from
        // the system clock.
};

template <int MAX_BURST_SIZE, bsls::Types::Int64 NANOSECONDS_PER_ACTION>
class Throttle_ArgFilter_Imp {
    // Passing both args through this 'struct' achieves two goals:
    //: o It ensures that both are evaluated at compile time, which won't be
    //:   the case if either contains any floating point.
    //:
    //: o It enables us to do compile-time checks with 'BSLMF_ASSERT'.

    BSLMF_ASSERT(0 <= MAX_BURST_SIZE);
    BSLMF_ASSERT(0 <= NANOSECONDS_PER_ACTION);
    BSLMF_ASSERT(MAX_BURST_SIZE || NANOSECONDS_PER_ACTION);
    BSLMF_ASSERT(LLONG_MAX / (MAX_BURST_SIZE ? MAX_BURST_SIZE : 1) >=
                                                  NANOSECONDS_PER_ACTION);

  public:
    static const bsls::Types::Int64 value = 0 == MAX_BURST_SIZE
                                          ? Throttle::k_ALLOW_NONE
                                          : NANOSECONDS_PER_ACTION
                                          ? NANOSECONDS_PER_ACTION
                                          : Throttle::k_ALLOW_ALL;
};

template <int clockType>
class Throttle_ClockTypeFilter_Imp {
    // Passing 'clcokType' through this 'struct' achieves two goals:
    //: o It ensures that its value is computed at compile time.
    //:
    //: o It enables us to do a compile-time check of the validity of
    //:   'clockType'.

    typedef bsls::SystemClockType  SystemClockType;
    typedef SystemClockType::Enum  ClockEnum;

    BSLMF_ASSERT(SystemClockType::e_MONOTONIC == clockType ||
                 SystemClockType::e_REALTIME  == clockType);

  public:
    static const ClockEnum value = static_cast<ClockEnum>(clockType);
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
    if (numActions <= 0 ||
                        (d_maxBurstSize < numActions && 0 != d_maxBurstSize)) {
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
    if (numActions <= 0 ||
                        (d_maxBurstSize < numActions && 0 != d_maxBurstSize)) {
        return -1;                                                    // RETURN
    }

    *result = this->requestPermission(numActions, now);
    return 0;
}

// ACCESSORS
inline
int Throttle::maxBurstSize() const
{
    return d_maxBurstSize;
}

inline
bsls::Types::Int64 Throttle::nanosecondsPerAction() const
{
    return d_nanosecondsPerAction;
}

                          // ---------------------------
                          // BDLMT_THROTTLE_INIT* macros
                          // ---------------------------

#define BDLMT_THROTTLE_INIT(maxBurstSize,                                     \
                            nanosecondsPerAction) {                           \
    -BloombergLP::bdlmt::Throttle::k_TEN_YEARS_NANOSECONDS,                   \
    BloombergLP::bdlmt::Throttle_ArgFilter_Imp<(maxBurstSize),                \
                                               (nanosecondsPerAction)         \
                                                                    >::value, \
    bsl::integral_constant<BloombergLP::bsls::Types::Int64,                   \
                   1LL * (maxBurstSize) * (nanosecondsPerAction)>::value,     \
    bsl::integral_constant<int, (maxBurstSize)>::value,                       \
    BloombergLP::bsls::SystemClockType::e_MONOTONIC                           \
 }

#define BDLMT_THROTTLE_INIT_REALTIME(maxBurstSize,                            \
                                     nanosecondsPerAction) {                  \
    -BloombergLP::bdlmt::Throttle::k_TEN_YEARS_NANOSECONDS,                   \
    BloombergLP::bdlmt::Throttle_ArgFilter_Imp<(maxBurstSize),                \
                                               (nanosecondsPerAction)         \
                                                                    >::value, \
    bsl::integral_constant<BloombergLP::bsls::Types::Int64,                   \
                   1LL * (maxBurstSize) * (nanosecondsPerAction)>::value,     \
    bsl::integral_constant<int, (maxBurstSize)>::value,                       \
    BloombergLP::bsls::SystemClockType::e_REALTIME                            \
 }

#define BDLMT_THROTTLE_INIT_ALLOW_ALL  BDLMT_THROTTLE_INIT(INT_MAX, 0)
#define BDLMT_THROTTLE_INIT_ALLOW_NONE BDLMT_THROTTLE_INIT(0,       1)

                        // ---------------------------
                        // 'BDLMT_THROTTLE_IF*' macros
                        // ---------------------------

#define BDLMT_THROTTLE_IF(maxBurstSize,                                       \
                          nanosecondsPerAction)                               \
    if (bool bdlmt_throttle_iFsToP = false) {}                                \
    else                                                                      \
        for (static BloombergLP::bdlmt::Throttle bdlmt_throttle_iFtHrOtTlE =  \
                                BDLMT_THROTTLE_INIT((maxBurstSize),           \
                                                    (nanosecondsPerAction));  \
             !bdlmt_throttle_iFsToP;                                          \
             bdlmt_throttle_iFsToP = true)                                    \
            if (bdlmt_throttle_iFtHrOtTlE.requestPermission())

#define BDLMT_THROTTLE_IF_REALTIME(maxBurstSize,                              \
                                   nanosecondsPerAction)                      \
    if (bool bdlmt_throttle_iFsToP = false) {}                                \
    else                                                                      \
        for (static BloombergLP::bdlmt::Throttle bdlmt_throttle_iFtHrOtTlE =  \
                       BDLMT_THROTTLE_INIT_REALTIME((maxBurstSize),           \
                                                    (nanosecondsPerAction));  \
             !bdlmt_throttle_iFsToP;                                          \
             bdlmt_throttle_iFsToP = true)                                    \
            if (bdlmt_throttle_iFtHrOtTlE.requestPermission())

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
