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
// 'nanosecondsPerAction' and 'maxSimultaneousActions'.  Then clients request
// permission from this component to execute actions.  The component keeps
// track of the number of actions requested, and over time throttles the
// average number of actions permitted to a rate of '1 / nanosecondsPerAction'
// (actions-per-nanosecond).  So, for example, to limit the average rate of
// actions permitted to 10 actions per second (10 actions / one billion
// nanoseconds), the value for 'nanosecondsPerAction' would be 100000000 (which
// is one billion / 10).
//
// As clients request permission to perform actions the component accumulates a
// time debt for those actions that dissipates over time.  The maximum value
// for this time debt is given by
// 'maxSimultaneousActions * nanosecondsPerAction'.  The
// 'maxSimultaneousActions' configuration parameter thereby limits the maximum
// number of actions that can be simultaneously permitted.
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
//   BDLMT_THROTTLE_INIT(maxSimultaneousActions,
//                       nanosecondsPerAction)
//       Initialize this 'Throttle' to limit the average period of actions
//       permitted to the specified 'nanosecondsPerAction', and the maximum
//       number of actions allowed in a very short time to the specified
//       'maxSimultaneousActions', where time is measured according to the
//       monotonic system clock.  If 'maxSimultaneousActions' is 0, the
//       throttle will be configured to permit no actions.  If
//       'nanosecondsPerAction' is 0, the throttle will be configured to permit
//       all actions.  The behavior is undefined unless
//       '0 <= maxSimultaneousActions', '0 <= nanosecondsPerAction',
//       '0 < maxSimultaneousActions || 0 < nanosecondsPerAction', and
//       'maxSimultaneousActions * nanosecondsPerAction <= LLONG_MAX'.  Note
//       that floating-point expressions are not allowed in any of the
//       arguments, as they cannot be evaluated at compile-time on some
//       platforms.
//
//   BDLMT_THROTTLE_INIT_REALTIME(maxSimultaneousActions,
//                                nanosecondsPerAction)
//       Initialize this 'Throttle' to limit the average period of actions
//       permitted to the specified 'nanosecondsPerAction', and the maximum
//       number of actions allowed in a very short time to the specified
//       'maxSimultaneousActions', where time is measured according to the
//       realtime system clock.  If 'maxSimultaneousActions' is 0, the throttle
//       will be configured to permit no actions.  If 'nanosecondsPerAction' is
//       0, the throttle will be configured to permit all actions.  The
//       behavior is undefined unless '0 <= maxSimultaneousActions',
//       '0 <= nanosecondsPerAction',
//       '0 < maxSimultaneousActions || 0 < nanosecondsPerAction', and
//       'maxSimultaneousActions * nanosecondsPerAction <= LLONG_MAX'.  Note
//       that floating-point expressions are not allowed in any of the
//       arguments, as they cannot be evaluated at compile-time on some
//       platforms.
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
//   BDLMT_THROTTLE_IF(maxSimultaneousActions,
//                     nanosecondsPerAction)
//       Create a throttled 'if' statement limiting execution of the statement
//       following it (and controlling execution of any 'else' clause present).
//       Limit the average period of actions permitted to the specified
//       'nanosecondsPerAction', and the maximum number of actions allowed in a
//       very short time to the specified 'maxSimultaneousActions', where time
//       is measured according to the monotonic system clock.  If
//       'maxSimultaneousActions' is 0, the 'if' will be configured to permit
//       no actions.  If 'nanosecondsPerAction' is 0, the 'if' will be
//       configured to permit all actions.  The behavior is undefined unless
//       '0 <= maxSimultaneousActions', '0 <= nanosecondsPerAction',
//       '0 < maxSimultaneousActions || 0 < nanosecondsPerAction', and
//       'maxSimultaneousActions * nanosecondsPerAction <= LLONG_MAX'.  Note
//       that floating-point expressions are not allowed in any of the
//       arguments, as they cannot be evaluated at compile-time on some
//       platforms.
//
//   BDLMT_THROTTLE_IF_REALTIME(maxSimultaneousActions,
//                              nanosecondsPerAction)
//       Create a throttled 'if' statement limiting execution of the statement
//       following it (and controlling execution of any 'else' clause present).
//       Limit the average period of actions permitted to the specified
//       'nanosecondsPerAction', and the maximum number of actions allowed in a
//       very short time to the specified 'maxSimultaneousActions', where time
//       is measured according to the realtime system clock.  If
//       'maxSimultaneousActions' is 0, the 'if' will be configured to permit
//       no actions.  If 'nanosecondsPerAction' is 0, the 'if' will be
//       configured to permit all actions.  The behavior is undefined unless
//       '0 <= maxSimultaneousActions', '0 <= nanosecondsPerAction',
//       '0 < maxSimultaneousActions || 0 < nanosecondsPerAction', and
//       'maxSimultaneousActions * nanosecondsPerAction <= LLONG_MAX'.  Note
//       that floating-point expressions are not allowed in any of the
//       arguments, as they cannot be evaluated at compile-time on some
//       platforms.
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
// In this section we show intended usage of this component.
//
// Suppose we have an error reporting function 'reportError', that prints an
// error message.  There is a possibility that 'reportError' will be called
// very frequently and spew, so we want to throttle the error messages.  If
// many happen in a short time, we want to see the first 10, but we don't want
// them spewing at a sustained rate of more than one message every five
// seconds.
//
// First, we begin our routine:
//..
//  void reportError(bsl::ostream& stream)
//      // Report an error to the specified 'stream'.
//  {
//..
// Then, we define the maximum number of traces that can happen in a short
// time, provided that there have been no preceding traces for a long time:
//..
//      static const int                maxSimultaneousTraces = 10;
//..
// Next, we define the minimum nanoseconds per trace if sustained traces are
// being attempted to print as five seconds.  Note that since this is in
// nanoseconds, a 64-bit value is going to be needed to represent it:
//..
//      static const bsls::Types::Int64 nanosecondsPerSustainedTrace =
//                          5 * bdlt::TimeUnitRatio::k_NANOSECONDS_PER_SECOND;
//..
// Then, we declare our 'throttle' object and use the 'BDLMT_THROTTLE_INIT'
// macro to initialize it, using the two above constants.  Note that the two
// above constants *MUST* be calculated at compile-time, which means, among
// other things, that they can't contain any floating point sub-expressions:
//..
//      static bdlmt::Throttle throttle = BDLMT_THROTTLE_INIT(
//                        maxSimultaneousTraces, nanosecondsPerSustainedTrace);
//..
// Next, we call 'requestPermission' at run-time to determine whether we've
// been spewing too much to allow the next trace:
//..
//      if (throttle.requestPermission()) {
//..
// Then, we do the error message controlled by the throttle:
//..
//          stream << "Help!  I'm being held prisoner in a microprocessor!\n";
//      }
//  }
//..
// Next, in 'main', we create an output 'bsl::ostream' object that writes to a
// RAM buffer:
//..
//  char                        buffer[10 * 1024];
//  bdlsb::FixedMemOutStreamBuf streamBuf(buffer, sizeof(buffer));
//  bsl::ostream                ostr(&streamBuf);
//..
// Then, we create a stopwatch and start it running:
//..
//  bsls::Stopwatch stopwatch;
//  stopwatch.start();
//..
// Now, we cycle for seven seconds, calling the above-defined 'reportError'
// every hundredth of a second.  This should result in ten traces being logged
// in the first tenth of a second, and one more trace being logged five seconds
// later, with all other requests for permission from the throttle being
// refused:
//..
//  while (stopwatch.accumulatedWallTime() < 7.0) {
//      reportError(ostr);
//      bslmt::ThreadUtil::microSleep(10 * 1000);
//  }
//..
// Finally, we count the number of traces that were logged and verify that the
// number is eleven, as anticipated:
//..
//  const bsl::size_t numLines = bsl::count(streamBuf.data(),
//                                          streamBuf.data() +
//                                                          streamBuf.length(),
//                                          '\n');
//  assert(11 == numLines);
//..

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
    // This 'class' provides a mechanism that can be used by clients to
    // regulate the frequency at which actions can be taken.  The data members
    // of 'Throttle' are currently public to allow for compile-time (aggregate)
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
    int                         d_maxSimultaneousActions;
    bsls::SystemClockType::Enum d_clockType;

  private:
    // FRIENDS
    template <int                MAX_SIMULTANEOUS_ACTIONS,
              bsls::Types::Int64 NANOSECONDS_PER_ACTION>
    friend struct Throttle_ArgFilter_Imp;

  public:
    // MANIPULATORS
    void initialize(int                         maxSimultaneousActions,
                    Int64                       nanosecondsPerAction,
                    bsls::SystemClockType::Enum clockType =
                                           bsls::SystemClockType::e_MONOTONIC);
        // Initialize this 'Throttle' to limit the average period of actions
        // permitted to the specified 'nanosecondsPerAction', and the maximum
        // number of actions allowed in a very short timespan to the specified
        // 'maxSimultaneousActions'.  Optionally specify 'clockType' to
        // indicate which system clock will be used to measure time.  If
        // 'clockType' is not supplied the monotonic system clock is used.  The
        // configured throttle will over time limit the average number of
        // actions permitted to a rate of '1 / nanosecondsPerAction'.  If
        // 'maxSimultaneousActions' is 0, the throttle will be configured to
        // permit no actions, otherwise if 'nanosecondsPerAction' is 0, the
        // throttle will be configured to permit all actions.  The behavior is
        // undefined unless '0 <= nanosecondsPerAction',
        // '0 <= maxSimultaneousActions',
        // '0 < nanosecondsPerAction || 0 < maxSimultaneousActions', and
        // 'maxSimultaneousActions * nanosecondsPerActionLeak <= LLONG_MAX'.
        // Note that the behavior for other methods is undefined unless this
        // 'Throttle' is initialized (either using this function, or a
        // 'BDLMT_THROTTLE_INIT' macro) prior to being called.

    bool requestPermission();
    bool requestPermission(const bsls::TimeInterval& now);
    bool requestPermission(int                       numActions);
    bool requestPermission(int                       numActions,
                           const bsls::TimeInterval& now);
        // Return 'true' if the time debt incurred by taking the indicated
        // action(s) would *not* exceed the maximum allowed time debt
        // configured for this 'Throttle' object
        // ('nanosecondsPerAction * maxSimultaneousActions'), and 'false'
        // otherwise.  Optionally specify 'now' indicating the current time of
        // the system clock for which this object is configured ('now' is a
        // offset from that clocks epoch).  If 'now' is not supplied, the
        // current time is obtained from the configured system clock.
        // Optionally specify 'numActions' indicating the number of actions
        // requested.  If 'numActions' is not supplied, one action is
        // requested.  If this function returns 'true' then
        // 'numActions * nanosecondsPerActions' is added to the time debt
        // accumulated by this component.  The behavior is undefined unless
        // this throttle has been initialized (either by calling 'initialize'
        // or using one of the 'BDLMT_THROTTLE_INIT*' macros),
        // '0 < numActions', and ('numActions <= maxSimultaneousActions' or
        // '0 == maxSimultaneousActions').  Note that
        // 'requestPerimissionIfValid', unlike these methods, does not have any
        // preconditions on the value of 'numActions'.

    int requestPermissionIfValid(bool                      *result,
                                 int                        numActions);
    int requestPermissionIfValid(bool                      *result,
                                 int                        numActions,
                                 const bsls::TimeInterval&  now);
        // Set the specified '*result' to 'true' if the time debt incurred by
        // taking the specified 'numActions' would *not* exceed the maximum
        // allowed time debt configured for this 'Throttle' object
        // ('nanosecondsPerAction * maxSimultaneousActions'), and 'false'
        // otherwise.  Optionally specify 'now' indicating the current time of
        // the system clock for which this object is configured ('now' is a
        // offset from that clocks epoch).  If 'now' is not supplied, the
        // current time is obtained from the configured system clock.  If
        // '*result' is set to 'true' then 'numActions * nanosecondsPerActions'
        // is added to the time debt accumulated by this component.  Return 0
        // if '0 <= numActions' and ('numActions <= maxSimultaneousActions' or
        // '0 == maxSimultaneousActions'), and a non-zero value otherwise.  The
        // behavior is undefined unless this throttle has been initialized
        // (either by calling 'initialize' or using one of the
        // 'BDLMT_THROTTLE_INIT*' macros).  Note that unless 0 is returned,
        // '*result' is unaffected.

    // ACCESSOR
    bsls::SystemClockType::Enum clockType() const;
        // Return the system clock type with which this object was configured.

    int maxSimultaneousActions() const;
        // Return the 'maxSimultaneousActions' value with which this object was
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
        // return an error if 'numActions > maxSimultaneousActions').  The
        // returned 'result' is an offset from the epoch of the system clock
        // for which this throttle is configured.  The behavior is undefined
        // unless this throttle has been initialized (either by calling
        // 'initialize' or using a 'BDLMT_THROTTLE_INIT*' macro).  Note that
        // 'result' may be in the past, and this function does *not* obtain the
        // current time from the system clock.
};

template <int                MAX_SIMULTANEOUS_ACTIONS,
          bsls::Types::Int64 NANOSECONDS_PER_ACTION>
class Throttle_ArgFilter_Imp {
    // Passing both args through this 'struct' achieves two goals:
    //: o It ensures that both are evaluated at compile time, which won't be
    //:   the case if either contains any floating point.
    //:
    //: o It enables us to do compile-time checks with 'BSLMF_ASSERT'.
    //:
    //: o If 0 is passed to 'NANOSECONDS_PER_ACTION', that indicates
    //:   'allow all', in which case the 'd_maxSimultaneousActions' (or
    //:   'msaValue') should be 'INT_MAX' and the 'd_nanosecondsPerAction' (or
    //:   'npaValue') should be 'k_ALLOW_ALL', regardless of the value of
    //:   'max_simultaneous_actions'.
    //:
    //: o If 0 is passed to 'MAX_SIMULTANEOUS_ACTIONS', that indicates
    //:   'allow none', in which case 'd_nanosecondsPerAction' (or 'nsaValue')
    //:   should be 'k_ALLOW_NONE' and 'd_maxSimultaneousActions' (or
    //:   'msaValue') should be 0.

    BSLMF_ASSERT(0 <= MAX_SIMULTANEOUS_ACTIONS);
    BSLMF_ASSERT(0 <= NANOSECONDS_PER_ACTION);
    BSLMF_ASSERT(MAX_SIMULTANEOUS_ACTIONS || NANOSECONDS_PER_ACTION);
    BSLMF_ASSERT(LLONG_MAX /
                   (MAX_SIMULTANEOUS_ACTIONS ? MAX_SIMULTANEOUS_ACTIONS : 1) >=
                                                       NANOSECONDS_PER_ACTION);

  public:
    // PUBLIC CONSTANTS
    static const bsls::Types::Int64 k_npaValue = 0 == MAX_SIMULTANEOUS_ACTIONS
                                               ? Throttle::k_ALLOW_NONE
                                               : NANOSECONDS_PER_ACTION
                                               ? NANOSECONDS_PER_ACTION
                                               : Throttle::k_ALLOW_ALL;

    static const int                k_msaValue = 0 == NANOSECONDS_PER_ACTION
                                               ? INT_MAX
                                               : MAX_SIMULTANEOUS_ACTIONS;
};

template <int clockType>
class Throttle_ClockTypeFilter_Imp {
    // Passing 'clcokType' through this 'struct' achieves two goals:
    //: o It ensures that its value is computed at compile time.
    //:
    //: o It enables us to do a compile-time check of the validity of
    //:   'clockType'.

    // PRIVATE TYPES
    typedef bsls::SystemClockType  SystemClockType;
    typedef SystemClockType::Enum  ClockEnum;

    BSLMF_ASSERT(SystemClockType::e_MONOTONIC == clockType ||
                 SystemClockType::e_REALTIME  == clockType);

  public:
    // PUBLIC CONSTANTS
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
    if (numActions <= 0 || (d_maxSimultaneousActions < numActions &&
                                              0 != d_maxSimultaneousActions)) {
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
    if (numActions <= 0 || (d_maxSimultaneousActions < numActions &&
                                              0 != d_maxSimultaneousActions)) {
        return -1;                                                    // RETURN
    }

    *result = this->requestPermission(numActions, now);
    return 0;
}

// ACCESSORS
inline
bsls::SystemClockType::Enum Throttle::clockType() const
{
    return d_clockType;
}

inline
int Throttle::maxSimultaneousActions() const
{
    return d_maxSimultaneousActions;
}

inline
bsls::Types::Int64 Throttle::nanosecondsPerAction() const
{
    return d_nanosecondsPerAction;
}

                          // ---------------------------
                          // BDLMT_THROTTLE_INIT* macros
                          // ---------------------------

#define BDLMT_THROTTLE_INIT(maxSimultaneousActions,                           \
                            nanosecondsPerAction) {                           \
    -BloombergLP::bdlmt::Throttle::k_TEN_YEARS_NANOSECONDS,                   \
    BloombergLP::bdlmt::Throttle_ArgFilter_Imp<(maxSimultaneousActions),      \
                                               (nanosecondsPerAction)         \
                                                               >::k_npaValue, \
    bsl::integral_constant<BloombergLP::bsls::Types::Int64,                   \
         1LL * (maxSimultaneousActions) * (nanosecondsPerAction)>::value,     \
    BloombergLP::bdlmt::Throttle_ArgFilter_Imp<(maxSimultaneousActions),      \
                                               (nanosecondsPerAction)         \
                                                               >::k_msaValue, \
    BloombergLP::bsls::SystemClockType::e_MONOTONIC                           \
 }

#define BDLMT_THROTTLE_INIT_REALTIME(maxSimultaneousActions,                  \
                                     nanosecondsPerAction) {                  \
    -BloombergLP::bdlmt::Throttle::k_TEN_YEARS_NANOSECONDS,                   \
    BloombergLP::bdlmt::Throttle_ArgFilter_Imp<(maxSimultaneousActions),      \
                                               (nanosecondsPerAction)         \
                                                               >::k_npaValue, \
    bsl::integral_constant<BloombergLP::bsls::Types::Int64,                   \
         1LL * (maxSimultaneousActions) * (nanosecondsPerAction)>::value,     \
    BloombergLP::bdlmt::Throttle_ArgFilter_Imp<(maxSimultaneousActions),      \
                                               (nanosecondsPerAction)         \
                                                               >::k_msaValue, \
    BloombergLP::bsls::SystemClockType::e_REALTIME                            \
 }

#define BDLMT_THROTTLE_INIT_ALLOW_ALL  BDLMT_THROTTLE_INIT(1, 0)
#define BDLMT_THROTTLE_INIT_ALLOW_NONE BDLMT_THROTTLE_INIT(0, 1)

                        // ---------------------------
                        // 'BDLMT_THROTTLE_IF*' macros
                        // ---------------------------

#define BDLMT_THROTTLE_IF(maxSimultaneousActions,                             \
                          nanosecondsPerAction)                               \
    if (bool bdlmt_throttle_iFsToP = false) {}                                \
    else                                                                      \
        for (static BloombergLP::bdlmt::Throttle bdlmt_throttle_iFtHrOtTlE =  \
                                BDLMT_THROTTLE_INIT((maxSimultaneousActions), \
                                                    (nanosecondsPerAction));  \
             !bdlmt_throttle_iFsToP;                                          \
             bdlmt_throttle_iFsToP = true)                                    \
            if (bdlmt_throttle_iFtHrOtTlE.requestPermission())

#define BDLMT_THROTTLE_IF_REALTIME(maxSimultaneousActions,                    \
                                   nanosecondsPerAction)                      \
    if (bool bdlmt_throttle_iFsToP = false) {}                                \
    else                                                                      \
        for (static BloombergLP::bdlmt::Throttle bdlmt_throttle_iFtHrOtTlE =  \
                       BDLMT_THROTTLE_INIT_REALTIME((maxSimultaneousActions), \
                                                    (nanosecondsPerAction));  \
             !bdlmt_throttle_iFsToP;                                          \
             bdlmt_throttle_iFsToP = true)                                    \
            if (bdlmt_throttle_iFtHrOtTlE.requestPermission())

#define BDLMT_THROTTLE_IF_ALLOW_ALL    BDLMT_THROTTLE_IF(1, 0)
#define BDLMT_THROTTLE_IF_ALLOW_NONE   BDLMT_THROTTLE_IF(0, 1)

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
