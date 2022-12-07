// bdlmt_throttle.h                                                   -*-C++-*-
#ifndef INCLUDED_BDLMT_THROTTLE
#define INCLUDED_BDLMT_THROTTLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide mechanism for limiting the rate at which actions may occur.
//
//@CLASSES:
//   bdlmt::Throttle: a mechanism for limiting the rate at which actions occur
//
//@MACROS:
//   BDLMT_THROTTLE_INIT, BDLMT_THROTTLE_INIT_REALTIME,
//   BDLMT_THROTTLE_INIT_ALLOW_ALL, BDLMT_THROTTLE_INIT_ALLOW_NONE,
//   BDLMT_THROTTLE_IF, BDLMT_THROTTLE_IF_REALTIME,
//   BDLMT_THROTTLE_IF_ALLOW_ALL, BDLMT_THROTTLE_IF_ALLOW_NONE
//
//@SEE_ALSO: bslmt_turnstile, btls_leakybucket
//
//@DESCRIPTION: This component provides a mechanism, 'bdlmt::Throttle', that
// can be used by clients to regulate the frequency at which actions can be
// taken.  Clients initialize a 'Throttle' with configuration values for
// 'nanosecondsPerAction', 'maxSimultaneousActions', and clock type.  Then
// clients request permission from this component to execute actions.  The
// component keeps track of the number of actions requested, and over time
// throttles the average number of actions permitted to a rate of
// '1 / nanosecondsPerAction' (actions-per-nanosecond).  So, for example, to
// limit the average rate of actions permitted to 10 actions per second
// (10 actions / one billion nanoseconds), the value for 'nanosecondsPerAction'
// would be 100000000 (which is one billion / 10).
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
// the system clock by which this component measures time.  By default, this
// component uses 'bsls::SystemClock::e_MONOTONIC'.  If the clock type
// indicated at initialization is 'bsls::SystemClockType::e_MONOTONIC', the
// timeout should be expressed as an absolute offset since the epoch of this
// clock (which matches the epoch used in
// 'bsls::SystemTime::now(bsls::SystemClockType::e_MONOTONIC)'.  If the clock
// type indicated at initialization is 'bsls::SystemClockType::e_REALTIME', the
// time should be expressed as an absolute offset since 00:00:00 UTC, January
// 1, 1970 (which matches the epoch used in
// 'bsls::SystemTime::now(bsls::SystemClockType::e_REALTIME)'.
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
///BLDMT_THROTTLE_INIT* macros
///- - - - - - - - - - - - - -
// One of these macros must be used to aggregate initialize 'bdlmt::Throttle'
// objects that have static storage duration -- the values are guaranteed to be
// evaluated at compile-time, avoiding race conditions.
//..
//   BDLMT_THROTTLE_INIT(maxSimultaneousActions,
//                       nanosecondsPerAction)
//   BDLMT_THROTTLE_INIT_REALTIME(maxSimultaneousActions,
//                                nanosecondsPerAction)
//       Initialize this 'Throttle' to limit the average period of actions
//       permitted to the specified 'nanosecondsPerAction', and the maximum
//       number of actions allowed at one time to the specified
//       'maxSimultaneousActions', where time is measured according to the
//       monotonic system clock.  These macros must be used for 'Throttle'
//       objects having static storage duration.  If 'maxSimultaneousActions'
//       is 0, the throttle will be configured to permit no actions.  If
//       'nanosecondsPerAction' is 0, the throttle will be configured to permit
//       all actions.  Use the '_REALTIME' variant of this macro to use the
//       real-time system clock to measure time, otherwise (by default) the
//       monotonic clock is used.  The behavior is undefined unless
//       '0 <= maxSimultaneousActions', '0 <= nanosecondsPerAction',
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
//..
///BDLMT_THROTTLE_IF* macros
///- - - - - - - - - - - - -
//..
//   BDLMT_THROTTLE_IF(maxSimultaneousActions,
//                     nanosecondsPerAction)
//   BDLMT_THROTTLE_IF_REALTIME(maxSimultaneousActions,
//                              nanosecondsPerAction)
//       This macro behaves like an 'if' clause, executing the subsequent
//       statement or block if the time debt incurred by taking a single action
//       would *not* exceed the maximum allowed time debt indicated by the
//       specified 'nanosecondsPerAction' and 'maxSimultaneousActions'.  If
//       this 'if' clause is 'true' (and the subsequent statement or block is
//       executed), then 'nanosecondsPerAction' is added to the time debt
//       accumulated by this macro instantiation.  'nanosecondsPerAction' is
//       the minimum average period between actions permitted by this macro
//       instantiation, and 'nanosecondsPerAction' is the maximum number of
//       simultaneous actions permitted by this macro instantiation.  If
//       'maxSimultaneousActions' is 0, the 'if' clause will evaluate to
//       'false'.  If 'nanosecondsPerAction' is 0, the 'if' clause will
//       evaluate to 'true'.  Use the '_REALTIME' variant of this macro to use
//       the real-time system clock to measure time, otherwise (by default) the
//       monotonic clock is used.  The behavior is undefined unless
//       '0 <= maxSimultaneousActions', '0 <= nanosecondsPerAction', and
//       '0 < maxSimultaneousActions || 0 < nanosecondsPerAction'.  Note that
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
//..
///Lack of 'bsl::chrono'-Based Overloads for 'requestPermission*'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 'bdlmt::Throttle' does not provide overloads for 'requestPermission' and
// 'requestPermissionIfValid' that take a 'bsl::chrono::time_point' as a
// representation for 'now'.  There are three reasons for this.  First,
// converting between different clocks is expensive, involving at least two
// calls to 'now' (one for the clock defined in the time point, and one for the
// clock used by the throttle).  This is supposed to be a performant component,
// allowing the caller to avoid the call to 'bsls::SystemTime::now' by passing
// in their own value of 'now'.  Second, it is inherently imprecise;
// conversions with the same input can return slightly different results,
// depending on the scheduling of the calls to 'now'.  Third, we have no way to
// support clocks that run at different rates.
//
///Usage
///-----
// In this section we show intended usage of this component.
//
///Example 1: Error Reporting
/// - - - - - - - - - - - - -
// Suppose we have an error reporting function 'reportError', that prints an
// error message to a log stream.  There is a possibility that 'reportError'
// will be called very frequently, and that reports of this error will
// overwhelm the other contents of the log, so we want to throttle the number
// of times this error will be reported.  For our application we decide that we
// want to see at most 10 reports of the error at any given time, and that if
// the error is occurring continuously, that we want a maximum sustained rate
// of one error report every five seconds.
//
// First, we declare the signature of our 'reportError' function:
//..
//  void reportError(bsl::ostream& stream)
//      // Report an error to the specified 'stream'.
//  {
//..
// Then, we define the maximum number of traces that can happen at a time to be
// 10:
//..
//      static const int maxSimultaneousTraces = 10;
//..
// Next, we define the minimum interval between subsequent reported errors, if
// errors are being continuously reported to be one report every 5 seconds.
// Note that the units are nanoseconds, which must be represented using a 64
// bit integral value:
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
// Now, we call 'requestPermission' at run-time to determine whether to report
// the next error to the log:
//..
//      if (throttle.requestPermission()) {
//..
// Finally, we write the message to the log:
//..
//          stream << "Help!  I'm being held prisoner in a microprocessor!\n";
//      }
//  }
//..

#include <bdlscm_version.h>

#include <bdlt_timeunitratio.h>

#include <bslmf_assert.h>
#include <bslmf_integralconstant.h>

#include <bsls_atomicoperations.h>
#include <bsls_libraryfeatures.h>
#include <bsls_systemclocktype.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>
#include <bsls_types.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <bsl_chrono.h>
#endif

#include <bsl_climits.h>

namespace BloombergLP {
namespace bdlmt {

                               // ==============
                               // class Throttle
                               // ==============

class Throttle {
    // This 'class' provides a mechanism that can be used by clients to
    // regulate the frequency at which actions can be taken.  The data members
    // of 'Throttle' are currently public to allow for compile-time (aggregate)
    // initialization of 'Throttle' objects having static storage duration (for
    // C++03 compilers that do not provide 'constexpr').

    // PRIVATE TYPES
    typedef bsls::Types::Int64       Int64;
    typedef bsls::AtomicOperations   AtomicOps;
    typedef AtomicOps::AtomicTypes   AtomicTypes;

    // PRIVATE CONSTANTS
    enum { k_BILLION = 1000 * 1000 * 1000 };

    static const Int64 k_ALLOW_ALL   = LLONG_MIN;
    static const Int64 k_ALLOW_NONE  = LLONG_MAX;
    static const Int64 k_MAX_SECONDS = LLONG_MAX / k_BILLION;
    static const Int64 k_MIN_SECONDS = LLONG_MIN / k_BILLION;

  public:
    // PUBLIC CONSTANTS
    static const Int64 k_TEN_YEARS_NANOSECONDS = 10 * 366 *
                                    bdlt::TimeUnitRatio::k_NANOSECONDS_PER_DAY;

    // PUBLIC DATA
    AtomicTypes::Int64 d_prevLeakTime;                  // effective time of
                                                        // previous leak

    Int64              d_nanosecondsPerAction;          // nanoseconds per
                                                        // sustained action

    Int64              d_nanosecondsPerTotalReset;      // total bucket
                                                        // capacity in time

    int                d_maxSimultaneousActions;        // total bucket
                                                        // capacity in actions

    bsls::SystemClockType::Enum
                       d_clockType;                     // clock type --
                                                        // monotonic or
                                                        // realtime

  private:
    // FRIENDS
    template <int                t_MAX_SIMULTANEOUS_ACTIONS,
              bsls::Types::Int64 t_NANOSECONDS_PER_ACTION>
    friend class Throttle_InitHelper;

  public:
    // MANIPULATORS
    void initialize(int                         maxSimultaneousActions,
                    Int64                       nanosecondsPerAction,
                    bsls::SystemClockType::Enum clockType =
                                           bsls::SystemClockType::e_MONOTONIC);
        // Initialize this 'Throttle' to limit the average period of actions
        // permitted to the specified 'nanosecondsPerAction', and the maximum
        // number of simultaneous actions allowed to the specified
        // 'maxSimultaneousActions'.  Optionally specify 'clockType' to
        // indicate the system clock that will be used to measure time (see
        // {Supported Clock-Types} in the component documentation).  If
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
        // 'Throttle' is initialized (either using one of the overloads of this
        // function, or a 'BDLMT_THROTTLE_INIT' macro) prior to being called.

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
    void initialize(int                              maxSimultaneousActions,
                    Int64                            nanosecondsPerAction,
                    const bsl::chrono::system_clock&);
        // Initialize this 'Throttle' to limit the average period of actions
        // permitted to the specified 'nanosecondsPerAction', and the maximum
        // number of simultaneous actions allowed to the specified
        // 'maxSimultaneousActions'.  Use the realtime system clock to measure
        // time (see {Supported Clock-Types} in the component documentation).
        // The configured throttle will over time limit the average number of
        // actions permitted to a rate of '1 / nanosecondsPerAction'.  If
        // 'maxSimultaneousActions' is 0, the throttle will be configured to
        // permit no actions, otherwise if 'nanosecondsPerAction' is 0, the
        // throttle will be configured to permit all actions.  The behavior is
        // undefined unless '0 <= nanosecondsPerAction',
        // '0 <= maxSimultaneousActions',
        // '0 < nanosecondsPerAction || 0 < maxSimultaneousActions', and
        // 'maxSimultaneousActions * nanosecondsPerActionLeak <= LLONG_MAX'.
        // Note that the behavior for other methods is undefined unless this
        // 'Throttle' is initialized (either using one of the overloads of this
        // function, or a 'BDLMT_THROTTLE_INIT' macro) prior to being called.

    void initialize(int                              maxSimultaneousActions,
                    Int64                            nanosecondsPerAction,
                    const bsl::chrono::steady_clock&);
        // Initialize this 'Throttle' to limit the average period of actions
        // permitted to the specified 'nanosecondsPerAction', and the maximum
        // number of simultaneous actions allowed to the specified
        // 'maxSimultaneousActions'.  Use the monotonic system clock to measure
        // time (see {Supported Clock-Types} in the component documentation).
        // The configured throttle will over time limit the average number of
        // actions permitted to a rate of '1 / nanosecondsPerAction'.  If
        // 'maxSimultaneousActions' is 0, the throttle will be configured to
        // permit no actions, otherwise if 'nanosecondsPerAction' is 0, the
        // throttle will be configured to permit all actions.  The behavior is
        // undefined unless '0 <= nanosecondsPerAction',
        // '0 <= maxSimultaneousActions',
        // '0 < nanosecondsPerAction || 0 < maxSimultaneousActions', and
        // 'maxSimultaneousActions * nanosecondsPerActionLeak <= LLONG_MAX'.
        // Note that the behavior for other methods is undefined unless this
        // 'Throttle' is initialized (either using one of the overloads of this
        // function, or a 'BDLMT_THROTTLE_INIT' macro) prior to being called.
#endif

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
        // 'numActions * nanosecondsPerAction' is added to the time debt
        // accumulated by this component.  The behavior is undefined unless
        // this throttle has been initialized (either by calling an overload of
        // 'initialize' or using one of the 'BDLMT_THROTTLE_INIT*' macros),
        // '0 < numActions', ('numActions <= maxSimultaneousActions' or
        // '0 == maxSimultaneousActions'), and the value of 'now', if
        // specified, can be expressed in nanoseconds as a 64-bit signed
        // integer.  Note that 'requestPermissionIfValid', unlike these
        // methods, does not have any preconditions on the value of
        // 'numActions'.

    int requestPermissionIfValid(bool                      *result,
                                 int                        numActions);
    int requestPermissionIfValid(bool                      *result,
                                 int                        numActions,
                                 const bsls::TimeInterval&  now);
        // Set the specified '*result' to 'true' if the time debt incurred by
        // taking the specified 'numActions' would *not* exceed the maximum
        // allowed time debt configured for this 'Throttle' object
        // ('nanosecondsPerAction * maxSimultaneousActions'), and set '*result'
        // to 'false' otherwise.  Optionally specify 'now' indicating the
        // current time of the system clock for which this object is configured
        // ('now' is a offset from that clocks epoch).  If 'now' is not
        // supplied, the current time is obtained from the configured system
        // clock.  If '*result' is set to 'true' then
        // 'numActions * nanosecondsPerAction' is added to the time debt
        // accumulated by this component.  Return 0 if '0 <= numActions',
        // ('numActions <= maxSimultaneousActions' or
        // '0 == maxSimultaneousActions'), and the value of 'now', if
        // specified, can be expressed in nanoseconds as a 64-bit signed
        // integer, and a non-zero value otherwise.  The behavior is undefined
        // unless this throttle has been initialized (either by calling an
        // overload of 'initialize' or using one of the 'BDLMT_THROTTLE_INIT*'
        // macros).  Note that unless 0 is returned, '*result' is unaffected.

    // ACCESSOR
    bsls::SystemClockType::Enum clockType() const;
        // Return the system clock type with which this 'Throttle' is
        // configured to observe the passage of time.

    int maxSimultaneousActions() const;
        // Return the maximum number of simultaneous actions for which this
        // 'Throttle' is configured to permit.

    Int64 nanosecondsPerAction() const;
        // Return the time debt, in nanoseconds, that this 'Throttle' is
        // configured to incur for each action permitted.

    int nextPermit(bsls::TimeInterval *result, int numActions) const;
        // Load into the specified 'result' the earliest *absolute* *time*
        // (according to system clock configured at initialization) when the
        // specified 'numActions' will next be permitted.  Return 0 on success,
        // and a non-zero value (with no effect on 'result') if this throttle
        // is configured such that 'numActions' will never be permitted (i.e.,
        // return an error if 'numActions > maxSimultaneousActions') or if
        // 'numActions <= 0'.  The returned 'result' is an offset from the
        // epoch of the system clock for which this throttle is configured.
        // The behavior is undefined unless this throttle has been initialized
        // (either by calling 'initialize' or using a 'BDLMT_THROTTLE_INIT*'
        // macro).  Note that 'result' may be in the past, and this function
        // does *not* obtain the current time from the system clock.
};

                         // =========================
                         // class Throttle_InitHelper
                         // =========================

template <int                t_MAX_SIMULTANEOUS_ACTIONS,
          bsls::Types::Int64 t_NANOSECONDS_PER_ACTION>
class Throttle_InitHelper {
    // [!PRIVATE!] This component private meta-function is used to implement
    // the initialization macros.  This type provides the following:
    //: o Ensures arguments are evaluated at compile time (which won't be
    //:    the case for floating point arguments)
    //:
    //: o Enables compile time checks with BSLMF_ASSERT
    //:
    //: o Handles special cases if 0 is passed for
    //:   't_MAX_SIMULTANEOUS_ACTIONS' or t_NANOSECONDS_PER_ACTION'

    BSLMF_ASSERT(0 <= t_MAX_SIMULTANEOUS_ACTIONS);
    BSLMF_ASSERT(0 <= t_NANOSECONDS_PER_ACTION);
    BSLMF_ASSERT(t_MAX_SIMULTANEOUS_ACTIONS || t_NANOSECONDS_PER_ACTION);
    BSLMF_ASSERT(LLONG_MAX / (t_MAX_SIMULTANEOUS_ACTIONS
                                  ? t_MAX_SIMULTANEOUS_ACTIONS
                                  : 1) >=
                 t_NANOSECONDS_PER_ACTION);

  public:
    // PUBLIC CONSTANTS
    static const bsls::Types::Int64 k_npaValue =
                     0 == t_MAX_SIMULTANEOUS_ACTIONS ? Throttle::k_ALLOW_NONE
                     : t_NANOSECONDS_PER_ACTION      ? t_NANOSECONDS_PER_ACTION
                                                     : Throttle::k_ALLOW_ALL;

    static const int k_msaValue = 0 == t_NANOSECONDS_PER_ACTION
                                      ? INT_MAX
                                      : t_MAX_SIMULTANEOUS_ACTIONS;
};

//=============================================================================
//                               INLINE DEFINITIONS
//=============================================================================

                               // --------------
                               // class Throttle
                               // --------------

// MANIPULATORS
#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
inline
void Throttle::initialize(
                       int                              maxSimultaneousActions,
                       Int64                            nanosecondsPerAction,
                       const bsl::chrono::system_clock&)
{
    initialize(maxSimultaneousActions,
               nanosecondsPerAction,
               bsls::SystemClockType::e_REALTIME);
}

inline
void Throttle::initialize(
                       int                              maxSimultaneousActions,
                       Int64                            nanosecondsPerAction,
                       const bsl::chrono::steady_clock&)
{
    initialize(maxSimultaneousActions,
               nanosecondsPerAction,
               bsls::SystemClockType::e_MONOTONIC);
}
#endif

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
    { -BloombergLP::bdlmt::Throttle::k_TEN_YEARS_NANOSECONDS },               \
    BloombergLP::bdlmt::Throttle_InitHelper<                                  \
                                         (maxSimultaneousActions),            \
                                         (nanosecondsPerAction)>::k_npaValue, \
    bsl::integral_constant<BloombergLP::bsls::Types::Int64,                   \
         1LL * (maxSimultaneousActions) * (nanosecondsPerAction)>::value,     \
    BloombergLP::bdlmt::Throttle_InitHelper<                                  \
                                         (maxSimultaneousActions),            \
                                         (nanosecondsPerAction)>::k_msaValue, \
    BloombergLP::bsls::SystemClockType::e_MONOTONIC                           \
 }

#define BDLMT_THROTTLE_INIT_REALTIME(maxSimultaneousActions,                  \
                                     nanosecondsPerAction) {                  \
    { -BloombergLP::bdlmt::Throttle::k_TEN_YEARS_NANOSECONDS },               \
    BloombergLP::bdlmt::Throttle_InitHelper<                                  \
                                         (maxSimultaneousActions),            \
                                         (nanosecondsPerAction)>::k_npaValue, \
    bsl::integral_constant<BloombergLP::bsls::Types::Int64,                   \
         1LL * (maxSimultaneousActions) * (nanosecondsPerAction)>::value,     \
    BloombergLP::bdlmt::Throttle_InitHelper<                                  \
                                         (maxSimultaneousActions),            \
                                         (nanosecondsPerAction)>::k_msaValue, \
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

