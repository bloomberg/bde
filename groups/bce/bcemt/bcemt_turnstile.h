// bcemt_turnstile.h                                                  -*-C++-*-
#ifndef INCLUDED_BCEMT_TURNSTILE
#define INCLUDED_BCEMT_TURNSTILE

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism to meter time.
//
//@CLASSES:
//  bcemt_Turnstile: mechanism to meter time
//
//@AUTHOR: David Rubin (drubin6@bloomberg.net)
//
//@DESCRIPTION: This component provides a mechanism, 'bcemt_Turnstile', to
// meter time.  A turnstile is configured with a rate that specified how many
// "events" per second the turnstile should allow.  After the rate is set (via
// the constructor or the 'reset' method), callers may execute the 'waitTurn'
// method, which blocks until the next interval arrives.  If the turnstile is
// not called at or above the configured rate (e.g., due to processing
// performed at each interval), the turnstile is said to be "lagging behind."
// The amount of lag time is obtained from the 'lagTime' method.
//
///Comparison With Sleep
///---------------------
// A straightforward implementation of metering is to call some form of sleep
// (e.g., 'bcemt_ThreadUtil::microSleep') with a computed rate after each
// processing step.  However, simply calling "sleep" accumulates errors since
// this implementation does not account for the time taken during the
// processing step.  For example, given two functions that take 'rate' (turns
// per second) and 'duration' (expected execution time in seconds), and execute
// 'rate * duration' calls to 'bsl::sqrt', calling 'waitTurn' on a turnstile or
// 'bcemt_ThreadUtil::microSleep' with duration '1000000 / rate', respectively;
// the elapsed time for each call results in the following table, showing that
// the 'bcemt_Turnstile' implementation maintains the correct rate while the
// 'microSleep' implementation accumulates errors.
//..
//                     Elapsed Time
//  Rate  Duration  Turnstile     Sleep
//  ----  --------  ---------     -----
//    10         1   0.900310  0.940390
//   100         1   0.980853  1.609041
//   500         1   1.000711  4.989093
//  1000         1   1.000103  9.988734
//..
//
///Thread Safety
///-------------
// This component is thread-safe and thread-enabled, meaning that multiple
// threads may safely use their own instances or a shared instance of a
// 'bcemt_Turnstile' object.
//
///Timer Resolution
///----------------
// The 'waitTurn' method has a resolution of 10 milliseconds.  Therefore,
// 'bcemt_Turnstile' cannot guarantee that all turns can be taken in each one
// second interval if a rate higher than 100 turns per second is specified.
//
///Usage
///-----
// The following example illustrates the use of 'bcemt_Turnstile' to control
// the rate of output being written to a specified output stream.  The example
// function, 'heartbeat', prints a specified message at a specified rate for a
// specified duration.  An instance of 'bsls_Stopwatch' is used to measure time
// against the specified duration.
//..
//  void heartbeat(bsl::ostream&      stream,
//                 const bsl::string& message,
//                 double             rate,
//                 double             duration)
//  {
//      // Write the specified 'message' to the specified 'stream' at the
//      // specified 'rate' (given in messages per second) for the specified
//      // 'duration'.
//
//      bsls_Stopwatch  timer;
//      timer.start();
//      bcemt_Turnstile turnstile(rate);
//
//      while (true) {
//          turnstile.waitTurn();
//          if (timer.elapsedTime() >= duration) {
//              break;
//          }
//          stream << message;
//      }
//  }
//..
// The benefits of using 'bcemt_Turnstile' in the above example, as opposed to
// simply calling 'sleep' in a loop, are twofold.  Firstly, 'bcemt_Turnstile'
// automatically accounts for drift caused by additional processing, so the
// loop is allowed to execute immediately if the program fails to execute the
// loop at the specified 'rate'.  Secondly, computing the sleep time and
// executing the sleep call, are encapsulated in the turnstile component, which
// improves the overall readability of the program.

#ifndef INCLUDED_BCESCM_VERSION
#include <bcescm_version.h>
#endif

#ifndef INCLUDED_BCEMT_THREAD
#include <bcemt_thread.h>
#endif

#ifndef INCLUDED_BCES_ATOMICTYPES
#include <bces_atomictypes.h>
#endif

#ifndef INCLUDED_BDET_TIMEINTERVAL
#include <bdet_timeinterval.h>
#endif

#ifndef INCLUDED_BDETU_SYSTEMTIME
#include <bdetu_systemtime.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORMUTIL
#include <bsls_platformutil.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {

                           // =====================
                           // class bcemt_Turnstile
                           // =====================

class bcemt_Turnstile {
    // This class provides a mechanism to meter time.  Using either the
    // constructor or the 'reset' method, the client specifies 'rate', the
    // frequency per second that events are to occur.  The client calls
    // 'waitTurn', which will sleep until the next event is to occur.  If
    // 'waitTurn' is not called until after the next event is due, the
    // turnstile is said to be 'lagging' behind, and calls to 'waitTurn' will
    // not sleep until the events have caught up with the schedule.  The amount
    // of lagging can be determined via the 'lagTime' method, which returns 0
    // if no lagging is occurring, or a positive lag time in micro seconds.

    // DATA
    bces_AtomicInt64         d_nextTurn;   // absolute time of next turn in
                                           // microseconds

    bces_AtomicInt64         d_interval;   // interval time in microseconds

    mutable bces_AtomicInt64 d_timestamp;  // time of last call to 'now' in
                                           // microseconds

    // PRIVATE TYPES
    typedef bsls_Types::Int64        Int64;

  private:
    // NOT IMPLEMENTED
    bcemt_Turnstile(const bcemt_Turnstile&);
    bcemt_Turnstile& operator=(const bcemt_Turnstile&);

  public:
    // CREATORS
    explicit
    bcemt_Turnstile(double                   rate,
                    const bdet_TimeInterval& startTime = bdet_TimeInterval(0));
        // Create a turnstile object that admits clients at the specified
        // 'rate', expressed as the number of turns per second.  Optionally
        // specify the (relative) 'startTime' of the first turn.  If
        // 'startTime' is not specified, the first turn may be taken
        // immediately.  The behavior is undefined unless '0 < rate'.

    // ~bcemt_Turnstile();
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    void reset(double                   rate,
               const bdet_TimeInterval& startTime = bdet_TimeInterval(0));
        // Reset the rate of this turnstile to the specified 'rate', expressed
        // as the number of turns per second.  Optionally specify the
        // (relative) 'startTime' of the first turn.  If 'startTime' is not
        // specified, the first turn may be taken immediately.  Note that
        // threads blocked on 'waitTurn' are not interrupted.

    bsls_Types::Int64 waitTurn();
        // Sleep until the next turn may be taken, and set the time of the
        // subsequent turn.  Return the non-negative number of microseconds
        // spent waiting.

    // ACCESSORS
    bsls_PlatformUtil::Int64 lagTime() const;
        // Return the positive number of microseconds difference between the
        // the current time and the next turn, indicating that the turnstile is
        // lagging behind the configured rate.  Otherwise, return 0.
};

}  // close namespace BloombergLP
#endif

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2007, 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------- END-OF-FILE --------------------------------
