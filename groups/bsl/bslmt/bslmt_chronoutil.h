// bslmt_chronoutil.h                                                 -*-C++-*-
#ifndef INCLUDED_BSLMT_CHRONOUTIL
#define INCLUDED_BSLMT_CHRONOUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide utilities related to threading with C++11-style clocks.
//
//@CLASSES:
//  bslmt::ChronoUtil: namespace for `bsl::chrono`-related operations
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component defines a utility `struct`, `bslmt::ChronoUtil`,
// that serves as a namespace for a suite of classes and functions for
// interfacing C++11-style clocks with the clocks that BDE provides.
//
// `bslmt::ChronoUtil` defines a `durationToTimeInterval` function for
// converting an arbitrary `bsl::chrono::duration` to a `bsls::TimeInterval`.
//
// `bslmt::ChronoUtil` also defines a `timedWait` function for waiting on a
// synchronization primitive that uses a `bsls` system clock type internally
// (see {`bsls_systemclocktype`}), while allowing the user to specify the
// timeout using a `bsl::chrono::time_point`.
//
// `ChronoUtil::timedWait` requires several things from the underlying
// primitive:
//
// (1) an enumeration containing `e_TIMED_OUT`.
//
// (2) a member function, `timedWait`, that takes a `const bsls::TimeInterval&`
// denoting the timeout value, and possibly an additional pointer argument.
// This function should return 0 upon success, `e_TIMED_OUT` on a timeout, and
// some other value on failure.  `ChronoUtil::timedWait` will convert the
// `bsl::chrono::time_point` that is passed to it into a `bsls::TimeInterval`
// that can be used by the synchronization primitive (`ARG_TYPE` is just a
// placeholder name; the primitive will have its own type):
// ```
// int timedWait(const bsls::TimeInterval&);
// int timedWait(ARG_TYPE *argument, const bsls::TimeInterval&);
// ```
//
// (3) a `const` member function, `clockType`, that takes no parameters and
// returns the underlying clock that the primitive uses -- either
// `bsls::SystemClockType::e_REALTIME` or `bsls::SystemClockType::e_MONOTONIC`:
// ```
// bsls::SystemClockType::Enum clockType() const;
// ```
//
// Note that if the clock associated with the time point does not correspond to
// the clock used by the underlying synchronization primitive, then the
// `timedWait` function of the primitive may be called more than once, so the
// method is potentially more efficient if the clocks match.
//
// Finally, `bslmt::ChronoUtil` defines an `isMatchingClock` function that
// checks to see if a C++11-style clock matches a `bsls` system clock.  See
// {`bsls_systemclocktype`}.
//
///Usage
///-----
// This example illustrates intended use of this component.
//
// We first define a synchronization primitive that is compliant with the
// requirements of `bslmt::ChronoUtil::timedWait` and then demonstrate use of
// that primitive with `timedWait`.
//
///Prologue: Create a Synchronization Primitive
/// - - - - - - - - - - - - - - - - - - - - - -
// The `TimedWaitSuccess` class, defined below, is a synchronization primitive
// that complies with the requirements of `bslmt::ChronoUtil::timedWait` (see
// the component-level documentation for information).
//
// First, we define the interface of `TimedWaitSuccess`:
// ```
// /// `TimedWaitSuccess` is a synchronization primitive that always
// /// succeeds.
// class TimedWaitSuccess {
//
//   private:
//     // DATA
//     bsls::SystemClockType::Enum d_clockType;
//
//   public:
//     // TYPES
//     enum { e_TIMED_OUT = 1 };
//
//     // CREATORS
//
//     /// Create a `TimedWaitSuccess` object.  Optionally specify a
//     /// `clockType` indicating the type of the system clock against
//     /// which the `bsls::TimeInterval` `absTime` timeouts passed to the
//     /// `timedWait` method are to be interpreted.  If `clockType` is not
//     /// specified then the realtime system clock is used.
//     explicit
//     TimedWaitSuccess(bsls::SystemClockType::Enum clockType
//                                       = bsls::SystemClockType::e_REALTIME);
//
//     // MANIPULATORS
//
//     /// Return 0 immediately.  Note that this is for demonstration and
//     /// testing purposes only.
//     int timedWait(const bsls::TimeInterval&);
//
//     // ACCESSORS
//
//     /// Return the clock type used for timeouts.
//     bsls::SystemClockType::Enum clockType() const;
// };
// ```
// Then, we implement the creator.  All it has to do is remember the
// `clockType` that was passed to it:
// ```
// inline
// TimedWaitSuccess::TimedWaitSuccess(bsls::SystemClockType::Enum clockType)
// : d_clockType(clockType)
// {
// }
//
// ```
// Next, we implement the `timedWait` function.  In this simplistic primitive,
// this function always succeeds:
// ```
// // MANIPULATORS
// inline
// int TimedWaitSuccess::timedWait(const bsls::TimeInterval&)
// {
//     return 0;
// }
//
// ```
// Next, we implement the `clockType` function, which returns the underlying
// `bsls::SystemClockType::Enum` that this primitive uses:
// ```
// // ACCESSORS
// inline
// bsls::SystemClockType::Enum TimedWaitSuccess::clockType() const
// {
//     return d_clockType;
// }
// ```
//
///Example 1: Using `bslmt::ChronoUtil::timedWait`
///- - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates use of `bslmt::ChronoUtil::timedWait` to block on
// a synchronization primitive until either a condition is satisfied or a
// specified amount of time has elapsed.  We use a `bsl::chrono::time_point` to
// specify the amount of time to wait.  To do this, we call
// `bslmt::ChronoUtil::timedWait`, passing in the timeout as an *absolute* time
// point.  In this example, we're using `TimedWaitSuccess` as the
// synchronization primitive, and specifying the timeout using
// `bsl::chrono::steady_clock`.
//
// First, we construct the `TimedWaitSuccess` primitive; by default it uses the
// `bsls` realtime system clock to measure time:
// ```
// TimedWaitSuccess aPrimitive;
// ```
// Then, we call `bslmt::ChronoUtil::timedWait` to block on `aPrimitive`, while
// passing a timeout of "10 seconds from now", measured on the
// `bsl::chrono::steady_clock`:
// ```
// int rc = bslmt::ChronoUtil::timedWait(
//          &aPrimitive,
//          bsl::chrono::steady_clock::now() + bsl::chrono::seconds(10));
// ```
// When this call returns, one of three things will be true: (a) `rc == 0`,
// which means that the call succeeded before the timeout expired, (b)
// `rc == TimedWaitSuccess::e_TIMED_OUT`, which means that the call did not
// succeed before the timeout expired, or (c) `rc` equals some other value,
// which means that an error occurred.
//
// If the call to `bslmt::ChronoUtil::timedWait` returned `e_TIMED_OUT` then we
// are guaranteed that the current time *on the clock that the time point was
// defined on* is greater than the timeout value that was passed in.

#include <bslscm_version.h>

#include <bsls_libraryfeatures.h>
#include <bsls_systemclocktype.h>
#include <bsls_systemtime.h>
#include <bsls_timeinterval.h>

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#include <bsl_chrono.h>

namespace BloombergLP {
namespace bslmt {

                           // =================
                           // struct ChronoUtil
                           // =================

/// This `struct` provides a namespace for utility functions that operate on
/// `bsl::chrono` facilities.
struct ChronoUtil {

  public:
    // CLASS METHODS

    /// Return a `bsls::TimeInterval` having the value represented by the
    /// specified `duration`.  Unlike the implicit conversion defined from
    /// duration defined in `bsls::TimeInterval`, this conversion handles
    /// floating-point-based durations as well as integral ones.
    template <class REP_TYPE, class PERIOD_TYPE>
    static bsls::TimeInterval durationToTimeInterval(
                 const bsl::chrono::duration<REP_TYPE, PERIOD_TYPE>& duration);

    /// Return `true` if the specified (template parameter) type `CLOCK`
    /// matches the specified `clockType`, and `false` otherwise.
    template <class CLOCK>
    static
    bool isMatchingClock(bsls::SystemClockType::Enum clockType);

    /// Block on the specified `primitive` object of the (template
    /// parameter) `PRIMITIVE` type by calling its `timedWait` method,
    /// passing a timeout calculated from the specified `absTime`.
    /// `absTime` is an *absolute* time represented by a time point with
    /// respect to some epoch, which is determined by the clock associated
    /// with the time point.  Return 0 on success, `PRIMITIVE::e_TIMED_OUT`
    /// if the `absTime` timeout expired, and other return values on error.
    /// The `timedWait` method of `primitive` is called only once if the
    /// clock type specified by the (template parameter) type `CLOCK`
    /// corresponds to the clock used by `primitive`, and may be called more
    /// than once otherwise.  Note that error codes returned from this
    /// method, necessarily distinct from 0 and `PRIMITIVE::e_TIMED_OUT`,
    /// are defined by `PRIMITIVE`.
    template <class PRIMITIVE, class CLOCK, class DURATION>
    static
    int timedWait(PRIMITIVE                                       *primitive,
                  const bsl::chrono::time_point<CLOCK, DURATION>&  absTime);

    /// Block on the specified `primitive` object of the (template
    /// parameter) `PRIMITIVE` type by calling its `timedWait` method,
    /// passing the specified `argument` of the (template parameter)
    /// `ARG_TYPE` and a timeout calculated from the specified `absTime`.
    /// `absTime` is an *absolute* time represented by a time point with
    /// respect to some epoch, which is determined by the clock associated
    /// with the time point.  Return 0 on success, `PRIMITIVE::e_TIMED_OUT`
    /// if the `absTime` timeout expired, and other return values on error.
    /// The `timedWait` method of `primitive` is called only once if the
    /// clock type specified by the (template parameter) type `CLOCK`
    /// corresponds to the clock used by `primitive`, and may be called more
    /// than once otherwise.  Note that error codes returned from this
    /// method, necessarily distinct from 0 and `PRIMITIVE::e_TIMED_OUT`,
    /// are defined by `PRIMITIVE`.
    template <class PRIMITIVE, class ARG_TYPE, class CLOCK, class DURATION>
    static
    int timedWait(PRIMITIVE                                       *primitive,
                  ARG_TYPE                                        *argument,
                  const bsl::chrono::time_point<CLOCK, DURATION>&  absTime);
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                           // -----------------
                           // struct ChronoUtil
                           // -----------------

// CLASS METHODS
template <class REP_TYPE, class PERIOD_TYPE>
inline
bsls::TimeInterval ChronoUtil::durationToTimeInterval(
                  const bsl::chrono::duration<REP_TYPE, PERIOD_TYPE>& duration)
{
    using namespace bsl::chrono;

    // Explicit scope needed due to legacy code using 'seconds' at BloombergLP
    // namespace scope.
    bsl::chrono::seconds wholeSecs =
        duration_cast<bsl::chrono::seconds>(duration);
    nanoseconds nanoSecs  = duration_cast<nanoseconds>(duration - wholeSecs);
    return bsls::TimeInterval(wholeSecs.count(),
                              static_cast<int>(nanoSecs.count()));
}

template <class CLOCK>
inline
bool ChronoUtil::isMatchingClock(bsls::SystemClockType::Enum)
{
    return false;
}

template <>
inline
bool ChronoUtil::isMatchingClock<bsl::chrono::system_clock>(
                                         bsls::SystemClockType::Enum clockType)
{
    return bsls::SystemClockType::e_REALTIME == clockType;
}

template <>
inline
bool ChronoUtil::isMatchingClock<bsl::chrono::steady_clock>(
                                         bsls::SystemClockType::Enum clockType)
{
    return bsls::SystemClockType::e_MONOTONIC == clockType;
}

template <class PRIMITIVE, class CLOCK, class DURATION>
int ChronoUtil::timedWait(
                    PRIMITIVE                                       *primitive,
                    const bsl::chrono::time_point<CLOCK, DURATION>&  absTime)
{
    if (ChronoUtil::isMatchingClock<CLOCK>(primitive->clockType())) {
        return primitive->timedWait(
                     bsls::TimeInterval(absTime.time_since_epoch())); // RETURN
    }
    else {
        typename CLOCK::time_point now = CLOCK::now();
        int                        ret;

        // Iteration is necessary because the specified 'CLOCK' type is known
        // to be different from that used internally by 'primitive'.

        while (absTime > now) {
            bsls::TimeInterval ti =
                                  bsls::SystemTime::now(primitive->clockType())
                                                   .addDuration(absTime - now);
            ret = primitive->timedWait(ti);
            if (PRIMITIVE::e_TIMED_OUT != ret) {
                return ret;                                           // RETURN
            }
            now = CLOCK::now();
        }
        return PRIMITIVE::e_TIMED_OUT;                                // RETURN
    }
}

template <class PRIMITIVE, class ARG_TYPE, class CLOCK, class DURATION>
int ChronoUtil::timedWait(
                    PRIMITIVE                                       *primitive,
                    ARG_TYPE                                        *argument,
                    const bsl::chrono::time_point<CLOCK, DURATION>&  absTime)
{
    if (ChronoUtil::isMatchingClock<CLOCK>(primitive->clockType())) {
        return primitive->timedWait(
                     argument,
                     bsls::TimeInterval(absTime.time_since_epoch())); // RETURN
    }
    else {
        typename CLOCK::time_point now = CLOCK::now();
        int                        ret;

        // Iteration is necessary because the specified 'CLOCK' type is known
        // to be different from that used internally by 'primitive'.

        while (absTime > now) {
            bsls::TimeInterval ti =
                                  bsls::SystemTime::now(primitive->clockType())
                                                   .addDuration(absTime - now);
            ret = primitive->timedWait(argument, ti);
            if (PRIMITIVE::e_TIMED_OUT != ret) {
                return ret;                                           // RETURN
            }
            now = CLOCK::now();
        }
        return PRIMITIVE::e_TIMED_OUT;                                // RETURN
    }
}

}  // close package namespace
}  // close enterprise namespace

#endif // BSLS_LIBRARYFEATURES_HAS_CPP11_BASELINE_LIBRARY
#endif

// ----------------------------------------------------------------------------
// Copyright 2021 Bloomberg Finance L.P.
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
