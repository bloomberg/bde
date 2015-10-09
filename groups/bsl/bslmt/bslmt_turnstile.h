// bslmt_turnstile.h                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BSLMT_TURNSTILE
#define INCLUDED_BSLMT_TURNSTILE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a mechanism to meter time.
//
//@CLASSES:
//  bslmt::Turnstile: mechanism to meter time
//
//@DESCRIPTION: This component provides a mechanism, 'bslmt::Turnstile', to
// meter time.  A turnstile is configured with a rate that specified how many
// "events" per second the turnstile should allow.  After the rate is set (via
// the constructor or the 'reset' method), callers may execute the 'waitTurn'
// method, which blocks until the next interval arrives.  If the turnstile is
// not called at or above the configured rate (e.g., due to processing
// performed at each interval), the turnstile is said to be "lagging behind."
// The amount of lag time is obtained from the 'lagTime' method.
//
///Comparison with Sleep
///---------------------
// A straightforward implementation of metering is to call some form of sleep
// (e.g., 'bslmt::ThreadUtil::microSleep') with a computed rate after each
// processing step.  However, simply calling "sleep" accumulates errors since
// this implementation does not account for the time taken during the
// processing step.  For example, given two functions that take 'rate' (turns
// per second) and 'duration' (expected execution time in seconds), and execute
// 'rate * duration' calls to 'bsl::sqrt', calling 'waitTurn' on a turnstile or
// 'bslmt::ThreadUtil::microSleep' with duration '1000000 / rate',
// respectively; the elapsed time for each call results in the following table,
// showing that the 'bslmt::Turnstile' implementation maintains the correct
// rate while the 'microSleep' implementation accumulates errors.
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
// Except for the 'reset' method, this component is thread-safe and
// thread-aware, meaning that multiple threads may safely use their own
// instances or a shared instance of a 'bslmt::Turnstile' object, provided that
// 'reset' is not called on a turnstile object while another thread is
// accessing or modifying the same object.
//
///Timer Resolution
///----------------
// The 'waitTurn' method has a resolution of 10 milliseconds.  Therefore,
// 'bslmt::Turnstile' cannot guarantee that all turns can be taken in each one
// second interval if a rate higher than 100 turns per second is specified.
//
///Usage
///-----
// The following example illustrates the use of 'bslmt::Turnstile' to control
// the rate of output being written to a specified output stream.  The example
// function, 'heartbeat', prints a specified message at a specified rate for a
// specified duration.  An instance of 'bsls::Stopwatch' is used to measure
// time against the specified duration.
//..
//  static void heartbeat(bsl::ostream&       stream,
//                        const bsl::string&  message,
//                        double              rate,
//                        double              duration)
//  {
//      // Write the specified 'message' to the specified 'stream' at the
//      // specified 'rate' (given in messages per second) for the specified
//      // 'duration'.
//
//      bsls::Stopwatch  timer;
//      timer.start();
//      bslmt::Turnstile turnstile(rate);
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
// The benefits of using 'bslmt::Turnstile' in the above example, as opposed to
// simply calling 'sleep' in a loop, are twofold.  Firstly, 'bslmt::Turnstile'
// automatically accounts for drift caused by additional processing, so the
// loop is allowed to execute immediately if the program fails to execute the
// loop at the specified 'rate'.  Secondly, computing the sleep time and
// executing the sleep call, are encapsulated in the turnstile component, which
// improves the overall readability of the program.

#ifndef INCLUDED_BSLSCM_VERSION
#include <bslscm_version.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSLS_TIMEINTERVAL
#include <bsls_timeinterval.h>
#endif

#ifndef INCLUDED_BSLS_TYPES
#include <bsls_types.h>
#endif

namespace BloombergLP {
namespace bslmt {

                             // ===============
                             // class Turnstile
                             // ===============

class Turnstile {
    // This class provides a mechanism to meter time.  Using either the
    // constructor or the 'reset' method, the client specifies 'rate',
    // indicating the number of events per second that the turnstile will
    // allow.  The client then calls 'waitTurn', which will either sleep until
    // the next event is to occur, or return immediately if 'waitTurn' was
    // called after the next event is due.  If 'waitTurn' is not called until
    // after the next event is due, the turnstile is said to be 'lagging'
    // behind, and calls to 'waitTurn' will not sleep until the events have
    // caught up with the schedule.  Note that calling 'waitTurn' a single time
    // does not bring a turnstile back on schedule.  For example, if a
    // turnstile's configured frequency is one event per second, and the client
    // is 10 seconds behind schedule, if 'waitTurn' were subsequently called
    // once per second, the turnstile will remain at 10 seconds behind
    // schedule.  The amount by which events are lagging behind the schedule
    // can be determined via the 'lagTime' method, which returns the positive
    // number of microseconds by which the turnstile is lagging, or 0 if the
    // turnstile is not behind schedule.

    // DATA
    bsls::AtomicInt64         d_nextTurn;   // absolute time of next turn in
                                           // microseconds

    bsls::AtomicInt64         d_interval;   // interval time in microseconds

    mutable bsls::AtomicInt64 d_timestamp;  // time of last call to 'now' in
                                           // microseconds

    // PRIVATE TYPES
    typedef bsls::Types::Int64 Int64;

  private:
    // NOT IMPLEMENTED
    Turnstile(const Turnstile&);
    Turnstile& operator=(const Turnstile&);

  public:
    // CREATORS
    explicit
    Turnstile(double                    rate,
              const bsls::TimeInterval& startTime = bsls::TimeInterval(0));
        // Create a turnstile object that admits clients at the specified
        // 'rate', expressed as the number of turns per second.  Optionally
        // specify the (relative) 'startTime' of the first turn.  If
        // 'startTime' is not specified, the first turn may be taken
        // immediately.  The behavior is undefined unless '0 < rate'.

    // ~Turnstile();
        // Destroy this object.  Note that this trivial destructor is generated
        // by the compiler.

    // MANIPULATORS
    void reset(double                    rate,
               const bsls::TimeInterval& startTime = bsls::TimeInterval(0));
        // Reset the rate of this turnstile to the specified 'rate', expressed
        // as the number of turns per second.  Optionally specify the
        // (relative) 'startTime' of the first turn.  If 'startTime' is not
        // specified, the first turn may be taken immediately.  Note that
        // threads blocked on 'waitTurn' are not interrupted.

    bsls::Types::Int64 waitTurn();
        // Sleep until the next turn may be taken or return immediately if the
        // turnstile is lagging behind schedule.  Return the non-negative
        // number of microseconds spent waiting.

    // ACCESSORS
    bsls::Types::Int64 lagTime() const;
        // Return the positive number of microseconds difference between the
        // the current time and the next turn, indicating that the turnstile is
        // lagging behind the configured rate.  Otherwise, return 0.
};

}  // close package namespace
}  // close enterprise namespace
#endif

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
