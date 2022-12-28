// bsls_stopwatch.h                                                   -*-C++-*-
#ifndef INCLUDED_BSLS_STOPWATCH
#define INCLUDED_BSLS_STOPWATCH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide access to user, system, and wall times of current process.
//
//@CLASSES:
//  bsls::Stopwatch: accumulates user, system, wall times of current process
//
//@DESCRIPTION: This component provides a class, 'bsls::Stopwatch', that
// implements real-time (system clock) interval timers for the system, user,
// and wall times of the current process.  A 'bsls::Stopwatch' object can
// accumulate the above values from multiple runs and always presents only the
// final total (zero if never started or reset to the initial state).
//
///Accuracy and Precision
///----------------------
// A 'bsls::Stopwatch' object returns its elapsed time intervals in seconds as
// 'double' values.  The precision is given by that of the 'bsls::TimeUtil'
// component, and, as such, strives to be as high as possible.  Monotonic
// behavior is platform-dependent, however, as are accuracy and useful
// precision.  The user is advised to determine the actual performance on each
// platform of interest.  In general, it is better to avoid stopping and
// restarting the stopwatch too often (e.g., inside a loop).  It is better to
// measure the overhead of the loop separately and subtract that time from the
// over-all time interval.
//
///Accuracy on Windows
///- - - - - - - - - -
// 'bsls::Stopwatch' may be slow or inconsistent on some Windows machines.  See
// the 'Accuracy and Precision' section of 'bsls_timeutil.h'.
//
///Usage Examples
///--------------
// The following snippets of code illustrate basic use of a 'bsls::Stopwatch'
// object.  First we create a stopwatch and note that the accumulated times are
// all initially 0.0:
//..
//  bsls::Stopwatch s;
//  const double t0s = s.accumulatedSystemTime();  assert(0.0 == t0s);
//  const double t0u = s.accumulatedUserTime();    assert(0.0 == t0u);
//  const double t0w = s.accumulatedWallTime();    assert(0.0 == t0w);
//..
// Next we start the stopwatch such that it does not accumulate system or user
// times.  Note that a stopwatch always accumulates wall time (i.e., as long as
// it is in the RUNNING state):
//..
//  s.start();
//  const double t1s = s.accumulatedSystemTime();  assert(0.0 == t1s);
//  const double t1u = s.accumulatedUserTime();    assert(0.0 == t1u);
//  const double t1w = s.accumulatedWallTime();    assert(0.0 <= t1w);
//..
// Now stop the stopwatch and restart it so as to accumulate system and user
// times (i.e., by passing 'true' to the 'start' method):
//..
//  s.stop();
//  const double t2s = s.accumulatedSystemTime();  assert(t1s == t2s);
//  const double t2u = s.accumulatedUserTime();    assert(t1u == t2u);
//  const double t2w = s.accumulatedWallTime();    assert(t1w <= t2w);
//
//  s.start(bsls::Stopwatch::k_COLLECT_ALSO_CPU_TIMES);
//  const double t3s = s.accumulatedSystemTime();  assert(t2s <= t3s);
//  const double t3u = s.accumulatedUserTime();    assert(t2u <= t3u);
//  const double t3w = s.accumulatedWallTime();    assert(t2w <= t3w);
//..
// Finally, we reset the stopwatch, which both puts it into the STOPPED state
// and resets all accumulated times back to their initial state (i.e., 0.0):
//..
//  s.reset();
//  const double t4s = s.accumulatedSystemTime();  assert(0.0 == t4s);
//  const double t4u = s.accumulatedUserTime();    assert(0.0 == t4u);
//  const double t4w = s.accumulatedWallTime();    assert(0.0 == t4w);
//  const double t5s = s.accumulatedSystemTime();  assert(0.0 == t5s);
//  const double t5u = s.accumulatedUserTime();    assert(0.0 == t5u);
//  const double t5w = s.accumulatedWallTime();    assert(0.0 == t5w);
//..

#include <bsls_keyword.h>
#include <bsls_timeutil.h>
#include <bsls_types.h>

#include <string.h>

namespace BloombergLP {
namespace bsls {

                             // ===============
                             // class Stopwatch
                             // ===============

class Stopwatch {
    // The 'class' provides an accumulator for the system, user, and wall times
    // of the current process.  A stopwatch can be in either the STOPPED
    // (initial) state or the RUNNING state.  It potentially tracks three
    // values: the accumulated system time, the accumulated user time, and the
    // accumulated wall time (all in seconds and all initially set to zero).
    // Whether or not system and user times are accumulated is conditional on
    // how the stopwatch is started (see the 'start' method).  While in the
    // RUNNING state, a stopwatch accumulates the above values and it retains
    // the values if put into the STOPPED state (unless 'reset' is called).
    // The accumulated times can be accessed at any time and in either state
    // (RUNNING or STOPPED).

    // DATA
    Types::Int64 d_startSystemTime;        // system time when started
                                           // (nanoseconds)

    Types::Int64 d_startUserTime;          // user time when started
                                           // (nanoseconds)

    TimeUtil::OpaqueNativeTime d_startWallTime;
                                           // wall time when started
                                           // (nanoseconds)

    Types::Int64 d_accumulatedSystemTime;  // accumulated system time
                                           // (nanoseconds)

    Types::Int64 d_accumulatedUserTime;    // accumulated user time
                                           // (nanoseconds)

    Types::Int64 d_accumulatedWallTime;    // accumulated wall time
                                           // (nanoseconds)

    bool         d_isRunning;              // state flag ('true' if RUNNING,
                                           // 'false' if STOPPED)

    bool         d_collectCpuTimesFlag;    // 'true' if cpu times are being
                                           // collected

    // CLASS DATA
    static const double      s_nanosecondsPerSecond;   // conversion factor
                                                       // (for nanoseconds to
                                                       // seconds)

  private:
    // NOT IMPLEMENTED
    Stopwatch& operator=(const Stopwatch&) BSLS_KEYWORD_DELETED;

  private:
    // PRIVATE MANIPULATORS
    void updateTimes();
        // Update the CPU times accumulated but this stopwatch.

    // PRIVATE ACCESSORS
    void accumulatedTimesRaw(Types::Int64               *systemTime,
                             Types::Int64               *userTime,
                             TimeUtil::OpaqueNativeTime *wallTime) const;
        // Load into the specified 'systemTime', 'userTime', and 'wallTime' the
        // values of the system time, user time, and wall time (in
        // nanoseconds), respectively, as provided by 'TimeUtil'.

    Types::Int64 elapsedWallTime(TimeUtil::OpaqueNativeTime rawWallTime) const;
        // Return the elapsed time, in nanoseconds, between the current
        // 'd_startWallTime' and the specified 'rawWallTime'.

  public:
    // PUBLIC CONSTANTS
    static const bool k_COLLECT_WALL_TIME_ONLY     = false;
    static const bool k_COLLECT_WALL_AND_CPU_TIMES = true;
        // For readability/ease of understanding of code that calls the
        // 'start(bool)' method use these constants as arguments.

    // CREATORS
    Stopwatch();
        // Create a stopwatch in the STOPPED state having total accumulated
        // system, user, and wall times all equal to 0.0.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_DEFAULTED_FUNCTIONS
    // To avoid warnings about future incompatibility due to the deleted copy
    // assignment operator we declare the copy constructor as implicitly
    // generated.  For consistency the destructor was also placed here and
    // declared to be explicitly generated.

    Stopwatch(const Stopwatch& other) = default;
        // Create a stopwatch having the state and total accumulated system,
        // user, and wall time of the specified 'other' object.  Note that this
        // method's definition is compiler generated.

    ~Stopwatch() = default;
        // Destroy this stopwatch.  Note that this method's definition is
        // compiler generated.
#endif

    // MANIPULATORS
    void reset();
        // Place this stopwatch in the STOPPED state, unconditionally stopping
        // the accumulation of elapsed times, and set the quiescent elapsed
        // times to 0.0.

    void start(bool collectCpuTimes = false);
        // Place this stopwatch in the RUNNING state and begin accumulating
        // elapsed times if this object was in the STOPPED state.  Optionally
        // specify a 'collectCpuTimes' flag indicating whether CPU times should
        // be collected.  If 'collectCpuTimes' is not specified, then CPU times
        // are *not* collected.  Note that the instantaneous total elapsed
        // times are available from the RUNNING state.  Also note that
        // disabling collection of CPU times will result in fewer systems calls
        // and therefore faster measurements.  Also note that 'collectCpuTimes'
        // may be expressed using the one of the class level constants
        // 'k_COLLECT_WALL_TIME_ONLY', and 'k_COLLECT_WALL_AND_CPU_TIMES' for
        // easier immediate understanding of the meaning of the client code.

    void stop();
        // Place this stopwatch in the STOPPED state, unconditionally stopping
        // the accumulation of elapsed times.  Note that the quiescent
        // accumulated elapsed times are available while in the STOPPED state.

    // ACCESSORS
    double accumulatedSystemTime() const;
        // Return the total (instantaneous and quiescent) elapsed system time
        // (in seconds) accumulated by this stopwatch, or 0 if the collection
        // of CPU times is disabled.

    void accumulatedTimes(double *systemTime,
                          double *userTime,
                          double *wallTime) const;
        // Load into the specified 'systemTime', 'userTime' and 'wallTime' the
        // total (instantaneous and quiescent) elapsed system, user, and wall
        // times (all in seconds) accumulated by this stopwatch.  Note that
        // this method attempts to retrieve all of the values at the same time
        // (atomically), if the underlying platform supports it.

    double accumulatedUserTime() const;
        // Return the total (instantaneous and quiescent) elapsed user time (in
        // seconds) accumulated by this stopwatch, or 0 if the collection of
        // CPU times is disabled.

    double accumulatedWallTime() const;
        // Return the total (instantaneous and quiescent) elapsed wall time (in
        // seconds) accumulated by this stopwatch.

    double elapsedTime() const;
        // Return the total (instantaneous and quiescent) elapsed wall time (in
        // seconds) accumulated by this stopwatch.  Note that this method is
        // equivalent to 'accumulatedWallTime'.

    bool isRunning() const;
        // Return 'true' if this stopwatch is in the RUNNING state, and 'false'
        // otherwise.
};

// ============================================================================
//                             INLINE DEFINITIONS
// ============================================================================

                             // ---------------
                             // class Stopwatch
                             // ---------------

// PRIVATE ACCESSORS
inline
void Stopwatch::accumulatedTimesRaw(Types::Int64               *systemTime,
                                    Types::Int64               *userTime,
                                    TimeUtil::OpaqueNativeTime *wallTime) const
{
    TimeUtil::getProcessTimers(systemTime, userTime);
    TimeUtil::getTimerRaw(wallTime);
}

inline
Types::Int64 Stopwatch::elapsedWallTime(
                                  TimeUtil::OpaqueNativeTime rawWallTime) const
{
    return TimeUtil::convertRawTime(rawWallTime)
         - TimeUtil::convertRawTime(d_startWallTime);
}

// CREATORS
inline
Stopwatch::Stopwatch()
: d_startSystemTime(0)
, d_startUserTime(0)
// , d_startWallTime(0)  // opaque type, no default ctor from 0.
, d_accumulatedSystemTime(0)
, d_accumulatedUserTime(0)
, d_accumulatedWallTime(0)
, d_isRunning(false)
, d_collectCpuTimesFlag(false)
{
    TimeUtil::initialize();
    memset(&d_startWallTime, 0, sizeof(d_startWallTime));
}

// MANIPULATORS
inline
void Stopwatch::reset()
{
    d_isRunning             = false;
    d_accumulatedSystemTime = 0;
    d_accumulatedUserTime   = 0;
    d_accumulatedWallTime   = 0;
}

inline
void Stopwatch::start(bool collectCpuTimes)
{
    if (!d_isRunning) {
        d_collectCpuTimesFlag = collectCpuTimes;
        if (d_collectCpuTimesFlag) {
            accumulatedTimesRaw(&d_startSystemTime,
                                &d_startUserTime,
                                &d_startWallTime);
        }
        else {
            TimeUtil::getTimerRaw(&d_startWallTime);
        }
        d_isRunning = true;
    }
}

inline
void Stopwatch::stop()
{
    if (d_isRunning) {
        if (d_collectCpuTimesFlag) {
            updateTimes();
        }
        else {
            TimeUtil::OpaqueNativeTime now;
            TimeUtil::getTimerRaw(&now);
            d_accumulatedWallTime += elapsedWallTime(now);
        }
        d_isRunning = false;
    }
}

// ACCESSORS
inline
double Stopwatch::accumulatedSystemTime() const
{
    if (!d_collectCpuTimesFlag) {
        return 0.0;                                                   // RETURN
    }

    if (d_isRunning) {
        return (double)(d_accumulatedSystemTime
                  + TimeUtil::getProcessSystemTimer() - d_startSystemTime)
                                                      / s_nanosecondsPerSecond;
                                                                      // RETURN
    }
    return (double)d_accumulatedSystemTime / s_nanosecondsPerSecond;
}

inline
double Stopwatch::accumulatedUserTime() const
{
    if (!d_collectCpuTimesFlag) {
        return 0.0;                                                   // RETURN
    }

    if (d_isRunning) {
        return (double)(d_accumulatedUserTime
                      + TimeUtil::getProcessUserTimer() - d_startUserTime)
                                                      / s_nanosecondsPerSecond;
                                                                      // RETURN
    }
    return (double)d_accumulatedUserTime / s_nanosecondsPerSecond;
}

inline
double Stopwatch::accumulatedWallTime() const
{
    if (d_isRunning) {
        TimeUtil::OpaqueNativeTime now;
        TimeUtil::getTimerRaw(&now);
        return (double)(d_accumulatedWallTime + elapsedWallTime(now))
                                                      / s_nanosecondsPerSecond;
                                                                      // RETURN
    }
    return (double)d_accumulatedWallTime / s_nanosecondsPerSecond;
}

inline
double Stopwatch::elapsedTime() const
{
    return accumulatedWallTime();
}

inline
bool Stopwatch::isRunning() const
{
    return d_isRunning;
}

}  // close package namespace

#ifndef BDE_OPENSOURCE_PUBLICATION  // BACKWARD_COMPATIBILITY
// ============================================================================
//                           BACKWARD COMPATIBILITY
// ============================================================================

typedef bsls::Stopwatch bsls_Stopwatch;
    // This alias is defined for backward compatibility.
#endif  // BDE_OPENSOURCE_PUBLICATION -- BACKWARD_COMPATIBILITY

}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2016 Bloomberg Finance L.P.
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
