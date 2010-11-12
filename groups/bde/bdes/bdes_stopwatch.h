// bdes_stopwatch.h   -*-C++-*-
#ifndef INCLUDED_BDES_STOPWATCH
#define INCLUDED_BDES_STOPWATCH

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide access to user, system, and wall times of current process.
//
//@DEPRECATED: Use 'bsls_stopwatch' instead.
//
//@CLASSES:
//   bdes_Stopwatch: accumulates user, system, wall times of current process
//
//@AUTHOR: Thomas Marshall (tmarshall), Alexander Evdokimov (aevdokimov1)
//
//@DESCRIPTION: This component provides a class, 'bdes_Stopwatch', that
// implements real-time (system clock) interval timers for the system, user,
// and wall times of the current process.  A 'bdes_Stopwatch' object can
// accumulate the above values from multiple runs and always presents only the
// final total (zero if never started or reset to the initial state).
//
///Accuracy and Precision
///----------------------
// A 'bdes_Stopwatch' object returns its elapsed time intervals in seconds as
// 'double' values.  The precision is given by that of the 'bdes_TimeUtil'
// component, and, as such, strives to be as high as possible.  Monotonic
// behavior is platform-dependent, however, as are accuracy and useful
// precision.  The user is advised to determine the actual performance on each
// platform of interest.  In general, it is better to avoid stopping and
// restarting the stopwatch too often (e.g., inside a loop).  It is better to
// measure the overhead of the loop separately and subtract that time from
// the over-all time interval.
//
///Usage
///-----
// The following snippets of code illustrate basic use of a 'bdes_Stopwatch'
// object.  First we create a stopwatch and note that the accumulated times are
// all initially 0.0:
//..
//  bdes_Stopwatch s;
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
// Now stop the stopwatch and restart it so as to accumulate system or user
// times (by this time passing 'true' to the 'start' method):
//..
//  s.stop();
//  const double t2s = s.accumulatedSystemTime();  assert(t1s == t2s);
//  const double t2u = s.accumulatedUserTime();    assert(t1u == t2u);
//  const double t2w = s.accumulatedWallTime();    assert(t1w <= t2w);
//
//  s.start(true);
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

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>  // required by 'bdes'
#endif

#ifndef INCLUDED_BSLS_STOPWATCH
#include <bsls_stopwatch.h>
#endif

namespace BloombergLP {

typedef bsls_Stopwatch bdes_Stopwatch;
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

}  // close namespace BloombergLP

#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ----------------------------------
