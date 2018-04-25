// ball_logthrottle.h                                                 -*-C++-*-
#ifndef INCLUDED_BALL_LOGTHROTTLE
#define INCLUDED_BALL_LOGTHROTTLE

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a throttling equivalent of some of the 'ball_log' macros.
//
//@CLASSES:
//
//@MACROS: BALL_LOGTHROTTLE_TRACE,               BALL_LOGTHROTTLE_DEBUG,
//         BALL_LOGTHROTTLE_INFO,                BALL_LOGTHROTTLE_WARN,
//         BALL_LOGTHROTTLE_ERROR,               BALL_LOGTHROTTLE_FATAL,
//         BALL_LOGTHROTTLE_STREAM,
//
//         BALL_LOGTHROTTLE_TRACE_BLOCK,         BALL_LOGTHROTTLE_DEBUG_BLOCK,
//         BALL_LOGTHROTTLE_INFO_BLOCK,          BALL_LOGTHROTTLE_WARN_BLOCK,
//         BALL_LOGTHROTTLE_ERROR_BLOCK,         BALL_LOGTHROTTLE_FATAL_BLOCK,
//         BALL_LOGTHROTTLE_BLOCK,
//
//         BALL_LOGTHROTTLEVA_TRACE,             BALL_LOGTHROTTLEVA_DEBUG,
//         BALL_LOGTHROTTLEVA_INFO,              BALL_LOGTHROTTLEVA_WARN,
//         BALL_LOGTHROTTLEVA_ERROR,             BALL_LOGTHROTTLEVA_FATAL,
//         BALL_LOGTHROTTLEVA
//
//@SEE ALSO: ball_log, bdlmt_throttle
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides numerous macros for performing logging
// where the number of messages logged is 'throttled', meaning that the number
// of messages that will be logged within a given time is limited.  The macros
// in this component are all analogous to their corresponding macros in
// 'ball_log'.  For example, the throtting version of 'BALL_LOG_INFO' is
// 'BALL_LOGTHROTTLE_INFO', and the throttling version of 'BALL_LOGVA' is
// 'BALL_LOGTHROTTLEVA'.
//
// Each message has a 'severity' associated with it (see 'ball_severity.h' for
// the definitions of the six severities).  Those macros which don't contain a
// severity name in their title are passed a severity enum defined in
// 'ball::Severity' as their first argument.  The other two arguments,
// 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE' are described
// immediately below.
//
// Each invocation of any of the macros provided by this component instantiates
// it's own, statically declared, 'bdlmt::Throttle' object, shared by all
// threads.
//
///Throttling Concepts:
///-------------------
// As clients attempt to output messages the throttle instantiated by each of
// these macros accumulates a time debt for those messages that dissipates over
// time.  The maximum value for this time debt is given by
// 'MAX_SIMULTANEOUS_MESSAGES * NANOSECONDS_PER_MESSAGE'.  The
// 'MAX_SIMULTANEOUS_MESSAGES' configuration parameter thereby limits the
// maximum number of messages that can be permitted in a very short time, and
// debt is "paid off" at a rate of '1 / NANOSECONDS_PER_MESSAGE'.
//
// This behavior is known as a "leaky-bucket" algorithm: messages permitted
// place water in the bucket, the passage of time drains water from the bucket,
// and the bucket has a maximum capacity.  Messages are permitted only when
// there is enough empty room in the bucket that the water placed won't
// overflow it.  A leaky bucket is an efficiently implementable approximation
// for allowing a certain number of messages over a window of time.
//
///'Leaky Bucket' and 'Time Debt' Example:
/// - - - - - - - - - - - - - - - - - - -
// If 'MAX_SIMULTANEOUS_MESSAGES' is 4, and 'NANOSECONDS_PER_MESSAGE' is 10
// seconds (equals '10LL * 1000 * 1000 * 1000' nanoseconds), we could observe
// the following sequence of events, where each row represents one message
// attempted to be logged.  Note that the bucket capacity is
// 'MAX_SIMULTANEOUS_MESSAGES * NANOSECONDS_PER_MESSAGE == 40'.
// The threshold 'T' of time debt above which a message will not be permitted
// is '(MAX_SIMULTANEOUS_MESSAGES - 1) * NANOSECONDS_PER_MESSAGE == 30'
//..
//      +--------------+----------------+---------------+---------------+
//      |     Time     |   Time Debt    |     Message   |   Time Debt   |
//      |  in Seconds  |   in Seconds   |   Permitted?  |   in Seconds  |
//      | Since First  |   Before Log   |  T = (4-1)*10 |   After Log   |
//      |   Message    |                |         = 30  |               |
//      +--------------+----------------+---------------+---------------+
//      |       0      |            0   |   0 <= T Yes  |   0+10 -> 10  |
//      |       2      |  10- 2 ->  8   |   8 <= T Yes  |   8+10 -> 18  |
//      |       4      |  18- 2 -> 16   |  16 <= T Yes  |  18+10 -> 26  |
//      |       8      |  26- 4 -> 22   |  22 <= T Yes  |  22+10 -> 32  |
//      |       9      |  32- 1 -> 31   |  31 >  T  No  |  31+ 0 -> 31  |
//      |      10      |  31- 1 -> 30   |  30 <= T Yes  |  30+10 -> 40  |
//      |      11      |  40- 1 -> 39   |  39 >  T  No  |  39+ 0 -> 39  |
//      |      12      |  39- 1 -> 38   |  38 >  T  No  |  38+ 0 -> 38  |
//      |      13      |  38- 1 -> 37   |  37 >  T  No  |  37+ 0 -> 37  |
//      |      14      |  37- 1 -> 36   |  36 >  T  No  |  36+ 0 -> 36  |
//      |      15      |  36- 1 -> 35   |  35 >  T  No  |  35+ 0 -> 35  |
//      |      35      |  35-20 -> 15   |  15 <= T Yes  |  15+10 -> 25  |
//      |      75      |  25-40 ->  0*  |   0 <= T Yes  |   0+10 -> 10  |
//      +--------------+----------------+---------------+---------------+
//
// * -- At 'Time == 75', the time debt goes to zero, even though 25-40 is
//      negative -- the time debt can never go negative, and never above the
//      bucket capacity of 40.
//..
// Note that it is always impossible for more than 'MAX_SIMULTANEOUES_MESSAGES'
// to occur within any time span of less than 'NANOSECONDS_PER_MESSAGE'
// nanoseconds.
//
///Thread Safety
///-------------
// All macros defined in this component are thread-safe, and can be invoked
// concurrently by multiple threads.
//
///Macro Reference
///---------------
// This section documents the preprocessor macros defined in this component.
//
//  enum { k_MSM = MAX_SIMULTANEOUS_MESSAGES };
//  static const bsls::Types::Int64 k_NPM = k_NANOSECONDS_PER_MESSAGE;
//
///Stream-Based Throttling Macros
/// - - - - - - - - - - - - - - -
// The 'BALL_LOGTHROTTLE_*' macros are analogous to the 'BALL_LOG_*' macros,
// except that they take two throttle-related arguments,
// 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE', described below.
//..
//  BALL_LOGTHROTTLE_<SEVERITY>(MAX_SIMULTANEOUS_MESSAGES,
//                              NANOSECONDS_PER_MESSAGE);
//      // Throttle with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
//      // 'NANOSECONDS_PER_MESSAGE' as described in the 'Throttling Concepts'
//      // section above, and if the throttle permits the message to happen,
//      // create a 'ball::Log_Stream' with severity '<SEVERITY>', which is one
//      // of 'TRACE', 'DEBUG', 'INFO', 'WARN', 'ERROR', or 'FATAL' as defined
//      // in the 'ball_severity' component, which can then be streamed to
//      // using 'operator<<'.  Note that the 'MAX_SIMULTANEOUS_MESSAGES' and
//      // 'NANOSECONDS_PER_MESSAGE' arguments must be compile-time constants
//      // and thus cannot contain any floating-point subexpressions.  Also
//      // note that 'MAX_SIMULTANEOUS_MESSAGES' is of type 'int' and
//      // 'NANOSECONDS_PER_MESSAGE' is of type 'bsls::Types::Int64'.
//..
// For example,
//..
//  BALL_LOGTHROTTLE_TRACE(k_MSM, k_NPM);
//  BALL_LOGTHROTTLE_DEBUG(k_MSM, k_NPM);
//  BALL_LOGTHROTTLE_INFO( k_MSM, k_NPM);
//  BALL_LOGTHROTTLE_WARN( k_MSM, k_NPM);
//  BALL_LOGTHROTTLE_ERROR(k_MSM, k_NPM);
//  BALL_LOGTHROTTLE_FATAL(k_MSM, k_NPM);
//..
// A closely-related macro also based on C++ streams,
// 'BALL_LOGTHROTTLE_STREAM', requires that the severity be explicitly supplied
// as an argument:
//..
//  BALL_LOGTHROTTLE_STREAM(SEVERITY,
//                          MAX_SIMULTANEOUS_MESSAGES,
//                          NANOSECONDS_PER_MESSAGE);
//      // Throttle with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
//      // 'NANOSECONDS_PER_MESSAGE' as described in the 'Throttling Concepts'
//      // section above, and if the throttle permits the message to happen,
//      // create a 'ball::Log_Stream' with the specified 'SEVERITY', which can
//      // then be streamed to using 'operator<<'.  Note that the
//      // 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE' arguments
//      // must be compile-time constants and thus cannot contain any
//      // floating-point subexpressions, while 'severity' can be a run-time
//      // expression.  Also note that 'MAX_SIMULTANEOUS_MESSAGES' is of type
//      // 'int' and 'NANOSECONDS_PER_MESSAGE' is of type 'bsls::Types::Int64',
//      // and 'severity' is of type 'ball::Severity::Enum'.
//..
// For example,
//..
//  BALL_LOGTHROTTLE_STREAM(ball::Severity::e_WARN, k_MSM, k_NPM);
//..
//
///'BLOCK-Style' Throttling Macros
///- - - - - - - - - - - - - - - -
// The 'BALL_LOGTHROTTLE_*_BLOCK' macros are analogous to the
// 'BALL_LOG_*_BLOCK' macros, except that they take two throttle-related
// arguments, 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE', which
// are described below.
//..
//  BALL_LOGTHROTTLE_<SEVERITY>_BLOCK(MAX_SIMULTANEOUS_MESSAGES,
//                                    NANOSECONDS_PER_MESSAGE)
//                                                         <statement or block>
//      // Throttle with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
//      // 'NANOSECONDS_PER_MESSAGE' as described in the 'Throttling Concepts'
//      // section above, and if the throttle permits the message to happen,
//      // create a 'ball::Log_Stream' with severity ''<SEVERITY>', which is
//      // one of 'TRACE', 'DEBUG', 'INFO', 'WARN', 'ERROR', or 'FATAL' as
//      // defined in the 'ball_severity' component, and then execute the
//      // statement or block following the macro, within which the
//      // 'ball::Log_Stream' can be accessed via the 'BALL_LOG_OUTPUT_STREAM'
//      // identifier.  Note that the 'MAX_SIMULTANEOUS_MESSAGES' and
//      // 'NANOSECONDS_PER_MESSAGE' arguments must be compile-time constants
//      // and thus cannot contain any floating-point subexpressions.
//..
// For example,
//..
//  BALL_LOGTHROTTLE_ERROR_BLOCK(k_MSM, k_NPM) {
//      if (x < y(z) + 7.2) {
//          BALL_LOG_OUTPUT_STREAM << "Help, I'm being held prisoner in a"
//                                                          " microprocessor!";
//      }
//  }
//..
// A closely-related 'BLOCK'-based macro 'BALL_LOGTHROTTLE_BLOCK', requires
// that the severity be explicitly supplied as an argument:
//..
//  BALL_LOGTHROTTLE_BLOCK(SEVERITY,
//                         MAX_SIMULTANEOUS_MESSAGES,
//                         NANOSECONDS_PER_MESSAGE) <statement or block>
//      // Throttle with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
//      // 'NANOSECONDS_PER_MESSAGE' as described in the 'Throttling Concepts'
//      // section above, and if the throttle permits the message to happen,
//      // create a 'ball::Log_Stream' with the specified 'SEVERITY', and then
//      // execute the statement or block following the macro, within which the
//      // 'ball::Log_Stream' can be accessed via the 'BALL_LOG_OUTPUT_STREAM'
//      // identifier.  Note that the 'MAX_SIMULTANEOUS_MESSAGES' and
//      // 'NANOSECONDS_PER_MESSAGE' arguments must be compile-time constants
//      // and thus cannot contain any floating-point subexpressions, while
//      // 'severity' can be a run-time expression.  Also note that
//      // 'MAX_SIMULTANEOUS_MESSAGES' is of type 'int' and
//      // 'NANOSECONDS_PER_MESSAGE' is of type 'bsls::Types::Int64', and
//      // 'severity' is of type 'ball::Severity::Enum'.
//..
// For example,
//..
//  BALL_LOGTHROTTLE_BLOCK(ball::Severity::e_ERROR, k_MSM, k_NPM) {
//      if (x < y(z) + 7.2) {
//          BALL_LOG_OUTPUT_STREAM << "Help, I'm being held prisoner in a"
//                                                          " microprocessor!";
//      }
//  }
//..
// Here, 'severity' can be a variable, but 'MSA' and 'NPM' must still be
// compile-time constants.
//
///'printf-Style' Throttling Macros
/// - - - - - - - - - - - - - - - -
// The 'BALL_LOGTHROTTLEVA_*' macros are analogous to the 'BALL_LOGVA_*'
// macros, except that they take two throttle-related arguments,
// 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE', which are
// described below.
//..
//  BALL_LOGTHROTTLEVA_<SEVERITY>(MAX_SIMULTANEOUS_MESSAGE,
//                                NANOSECONDS_PER_MESSAGE,
//                                printfStyleFormatString,
//                                ...);
//      // Throttle with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
//      // 'NANOSECONDS_PER_MESSAGE' as described in the 'Throttling Concepts'
//      // section above, and if the throttle permits the message to happen,
//      // log a printf-style message with the zero or more '...' arguments
//      // formatted with the specified 'printfStyleFormatString' and the
//      // specified '<SEVERITY>', which is one of 'TRACE', 'DEBUG', 'INFO',
//      // 'WARN', 'ERROR', or 'FATAL' as defined in the 'ball_severity'
//      // component.  Note that the 'MAX_SIMULTANEOUS_MESSAGES' and
//      // 'NANOSECONDS_PER_MESSAGE' arguments must be compile-time constants
//      // and thus cannot contain any floating-point subexpressions, while all
//      // the following arguments can be run-time expressions.  Also note that
//      // 'MAX_SIMULTANEOUS_MESSAGES' is of type 'int' and
//      // 'NANOSECONDS_PER_MESSAGE' is of type 'bsls::Types::Int64'.
//..
// For example:
//..
//  int cpu = 6502;
//  BALL_LOGTHROTTLEVA_ERROR(k_MSM, k_NPM, "Help, I'm being held prisoner"
//                                            " in a %d microprocessor!", cpu);
//..
// A closely related macro is 'BALL_LOGTHROTTLEVA' which takes the 'severity'
// as a variable, rather than being part of the macro name.
//..
//  BALL_LOGTHROTTLEVA(SEVERITY,
//                     MAX_SIMULTANEOUS_MESSAGE,
//                     NANOSECONDS_PER_MESSAGE,
//                     printfStyleFormatString,
//                     ...);
//      // Throttle with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
//      // 'NANOSECONDS_PER_MESSAGE' as described in the 'Throttling Concepts'
//      // section above, and if the throttle permits the message to happen,
//      // log a printf-style message with the zero or more '...' arguments
//      // formatted with the specified 'printfStyleFormatString' and the
//      // specified 'SEVERITY'.  Note that the 'MAX_SIMULTANEOUS_MESSAGES' and
//      // 'NANOSECONDS_PER_MESSAGE' arguments must be compile-time constants
//      // and thus cannot contain any floating-point subexpressions, while
//      // 'SEVERITY' and all the other arguments can be run-time expressions.
//      // Also note that 'MAX_SIMULTANEOUS_MESSAGES' is of type 'int' and
//      // 'NANOSECONDS_PER_MESSAGE' is of type 'bsls::Types::Int64', and
//      // 'severity' is of type 'ball::Severity::Enum'.
//..
// For example:
//..
//  int cpu = 6502;
//  BALL_LOGTHROTTLEVA_ERROR(ball::Severity::e_ERROR,
//                           k_MSM,
//                           k_NPM,
//                           "Help, I'm being held prisoner"
//                                                  " in a %d microprocessor!",
//                           cpu);
//..
//
///Usage
///-----
/// Code global to all usage examples:
//..
//  enum {
//      k_NUM_INFO  = 20,
//      k_NUM_DEBUG =  5,
//      k_NUM_TRACE =  1 };
//
//  const Int64 k_HOUR = bdlt::TimeUnitRatio::k_NANOSECONDS_PER_HOUR;
//..
//
///Example 1: 'stream-Style' Throttling Macro Usage
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose a computer is reading 'double' values from a radio receiver, ten per
// second, which represent readings of radiation detected by a Geiger counter
// on a spacecraft and is transmitting them to a ground control at Jet
// Propulsion Laboratories in California.
//
// The readings are returned by the 'double yield()' manipulator of a
// 'RadiationMeterReceiver' object (the implementation of which is omitted).
// The 'yield' method blocks until it obtains a reading to return.  If called
// in a tight loop, 'yield' returns ten readings per second.
//
// Readings range from 0 to 100.
//: o Readings above 10 but not greater than 30 are a concern, but are not very
//:   serious.  We will report those with an 'e_TRACE' severity, and at most
//:   one per hour.
//: o Readings above 30 but not greater than 60 are more of a worry.  We will
//:   report those with an 'e_DEBUG' severity, and at most five per hour.
//: o Readings above 60 but not greater than 90 are very serious, which will be
//:   reported with an 'e_INFO' severity, and at most twenty per hour.
//: o Readings above 90 are potentially catastrophic, and will be reported with
//:   an 'e_WARN' severity, with no limit on the number of readings reported.
//
// We are to write a daemon process, which will loop gathering readings.  A
// reading of an impossible value of -1.0 will indicate termination.
//..
//  void radiationMonitorStreamDaemon()
//      // Daemon to run the radiation monitor for a finite period of time.
//  {
//      BALL_LOG_SET_CATEGORY("RADIATION.MONITOR");
//
//      RadiationMeterReceiver receiver;
//
//      BALL_LOG_DEBUG << "Start gathering data.";
//
//      double reading;
//      while (-1.0 != (reading = receiver.yield())) {
//..
// First, we deal with 'e_WARN' readings:
//..
//          if      (90 < reading) {
//              BALL_LOG_WARN << "Serious Radiation reading of " << reading;
//          }
//..
// Next, we deal with 'e_INFO' readings that aren't as severe as 'e_WARN':
//..
//          else if (60 < reading) {
//              BALL_LOGTHROTTLE_INFO(k_NUM_INFO, k_HOUR) <<
//                                          "Radiation reading of " << reading;
//          }
//..
// Now, we deal with 'warning' messages less severe than 'error' readings:
//..
//          else if (30 < reading) {
//              BALL_LOGTHROTTLE_DEBUG(k_NUM_DEBUG, k_HOUR) <<
//                                          "Radiation reading of " << reading;
//          }
//..
// Finally, we deal with 'info' messages less severe than 'warning' readings:
//..
//          else if (10 < reading) {
//              BALL_LOGTHROTTLE_TRACE(k_NUM_TRACE, k_HOUR) <<
//                                          "Radiation reading of " << reading;
//          }
//      }
//
//      BALL_LOG_DEBUG << "Finished gathering data.";
//  }
//..
// The values returned by 'receiver.yield()' are:
//..
//  0 0 12.3 0 10.5 33.1 11.9 53.7 0 0 46.1 14.7 67.4 43.9 53.3 98.2 0 22.3
//  77.3 36.2 0 17.7 52.5 0 43.2 0 72.9 0 51.9 71.2 92.4 0 0 11.8 33.1 0 47.2
//  15.5 35.7 0 22.3 17.6 0 52.7 0 22.1 -1
//..
// Where:
//: o 13 readings of 0.0, which don't produce output, occurred.
//: o 9 readings in the range '10.0 < reading <= 30.0', which correspond to
//:   'e_TRACE' level messages, occurred.
//: o 13 readings in the range '30.0 < reading <= 60.0', which correspond to
//:   'e_DEBUG' level messages, occurred.
//: o 5 readings in the range '60.0 < reading <= 90.0', which correspond to
//:   'e_INFO' level messages, occurred.
//: o 2 readings in the range '90.0 < reading', which correspond to 'e_WARN'
//:   level messages, occurred.
//
// Note that only 1 'e_TRACE' trace and 5 'e_DEBUG' messages are permitted by
// the throttle within the (very long) time period of one hour, so the other
// messages at those levels will be suppressed.
//
// 'radiationMonitorPrintfDaemon' produces output like:
//..
//  24APR2018_16:36:22.791 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  460 RADIATION.MONITOR Start gathering data.
//
//  24APR2018_16:36:23.094 61260 139907579877152 TRACE ball_logthrottle.t.cpp
//  488 RADIATION.MONITOR Radiation reading of 12.3
//
//  24APR2018_16:36:23.396 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  481 RADIATION.MONITOR Radiation reading of 33.1
//
//  24APR2018_16:36:23.597 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  481 RADIATION.MONITOR Radiation reading of 53.7
//
//  24APR2018_16:36:23.901 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  481 RADIATION.MONITOR Radiation reading of 46.1
//
//  24APR2018_16:36:24.102 61260 139907579877152 INFO ball_logthrottle.t.cpp
//  474 RADIATION.MONITOR Radiation reading of 67.4
//
//  24APR2018_16:36:24.203 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  481 RADIATION.MONITOR Radiation reading of 43.9
//
//  24APR2018_16:36:24.304 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  481 RADIATION.MONITOR Radiation reading of 53.3
//
//  24APR2018_16:36:24.404 61260 139907579877152 WARN ball_logthrottle.t.cpp
//  468 RADIATION.MONITOR Serious Radiation reading of 98.2
//
//  24APR2018_16:36:24.706 61260 139907579877152 INFO ball_logthrottle.t.cpp
//  474 RADIATION.MONITOR Radiation reading of 77.3
//
//  24APR2018_16:36:25.513 61260 139907579877152 INFO ball_logthrottle.t.cpp
//  474 RADIATION.MONITOR Radiation reading of 72.9
//
//  24APR2018_16:36:25.816 61260 139907579877152 INFO ball_logthrottle.t.cpp
//  474 RADIATION.MONITOR Radiation reading of 71.2
//
//  24APR2018_16:36:25.918 61260 139907579877152 WARN ball_logthrottle.t.cpp
//  468 RADIATION.MONITOR Serious Radiation reading of 92.4
//
//  24APR2018_16:36:27.429 61260 139907579877152 DEBUG ball_logthrottle.t.cpp
//  493 RADIATION.MONITOR Finished gathering data.
//..
// Note that 8 'TRACE' messages and 8 'DEBUG' messages were suppressed by the
// throttling.
//
///Example 2: 'BLOCK-Style' Throttling Macro Usage
///- - - - - - - - - - - - - - - - - - - - - - - -
// Here, we just repeat exactly the same code, using the 'BLOCK'-style
// throttling macros instead of the 'stream'-style throttling macros:
//..
//  void radiationMonitorBlockDaemon()
//      // Daemon to run the radiation monitor for a finite period of time.
//  {
//      BALL_LOG_SET_CATEGORY("RADIATION.MONITOR");
//
//      RadiationMeterReceiver receiver;
//
//      BALL_LOGVA_DEBUG("Start gathering data.");
//
//      double reading;
//      while (-1.0 != (reading = receiver.yield())) {
//..
// First, we deal with 'fatal' messages:
//..
//          if      (90 < reading) {
//              BALL_LOG_WARN << "Serious radiation reading of " << reading;
//          }
//..
// Next, we deal with 'error' messages that aren't as severe as 'fatal':
//..
//          else if (60 < reading) {
//              BALL_LOGTHROTTLE_INFO_BLOCK(k_NUM_INFO, k_HOUR) {
//                  BALL_LOG_OUTPUT_STREAM <<
//                                          "Radiation reading of " << reading;
//              }
//          }
//..
// Now, we deal with 'warning' messages less severe than 'error' readings:
//..
//          else if (30 < reading) {
//              BALL_LOGTHROTTLE_DEBUG_BLOCK(k_NUM_DEBUG, k_HOUR) {
//                  BALL_LOG_OUTPUT_STREAM <<
//                                          "Radiation reading of " << reading;
//              }
//          }
//..
// Finally, we deal with 'info' messages less severe than 'warning' readings.
// Note that in the above two output statements there was only one statement
// in the block, so we can just replace the block with a single statement,
// which we'll do this time.
//..
//          else if (10 < reading) {
//              BALL_LOGTHROTTLE_TRACE_BLOCK(k_NUM_TRACE, k_HOUR)
//                       BALL_LOG_OUTPUT_STREAM << "Radiation reading of "
//                                                                  << reading;
//          }
//      }
//
//      BALL_LOG_DEBUG << "Finished gathering data.";
//  }
//..
// If the values returned by 'receiver.yield()' match those from Usage Example
// 1, then the output will be identical to that example.
//..
///Example 3: 'printf-Style' Throttling Macro Usage
/// - - - - - - - - - - - - - - - - - - - - - - - -
// Here, we just repeat exactly the same code, using the 'printf'-style
// throttling macros:
//..
//  void radiationMonitorPrintfDaemon()
//      // Daemon to run the radiation monitor for a finite period of time.
//  {
//      BALL_LOG_SET_CATEGORY("RADIATION.MONITOR");
//
//      RadiationMeterReceiver receiver;
//
//      BALL_LOGVA_DEBUG("Start gathering data.");
//
//      double reading;
//      while (-1.0 != (reading = receiver.yield())) {
//..
// First, we deal with 'fatal' messages:
//..
//          if      (90 < reading) {
//              BALL_LOGVA_WARN("Serious radiation reading of %g", reading);
//          }
//..
// Next, we deal with 'error' messages that aren't as severe as 'fatal':
//..
//          else if (60 < reading) {
//              BALL_LOGTHROTTLEVA_INFO(
//                                   k_NUM_INFO,
//                                   k_HOUR,
//                                   "Radiation reading of %g", reading);
//          }
//..
// Now, we deal with 'warning' messages less severe than 'error' readings:
//..
//          else if (30 < reading) {
//              BALL_LOGTHROTTLEVA_DEBUG(
//                                 k_NUM_DEBUG,
//                                 k_HOUR,
//                                 "Radiation reading of %g", reading);
//          }
//..
// Finally, we deal with 'info' messages less severe than 'warning' readings:
//..
//          else if (10 < reading) {
//              BALL_LOGTHROTTLEVA_TRACE(
//                                    k_NUM_TRACE,
//                                    k_HOUR,
//                                    "Radiation reading of %g", reading);
//          }
//      }
//
//      BALL_LOGVA_DEBUG("Finished gathering data.");
//  }
//..
// If the values returned by 'receiver.yield()' match those from Usage Example
// 1, then the output will be identical to that example.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALL_CATEGORY
#include <ball_category.h>
#endif

#ifndef INCLUDED_BALL_LOG
#include <ball_log.h>
#endif

#ifndef INCLUDED_BALL_SEVERITY
#include <ball_severity.h>
#endif

#ifndef INCLUDED_BDLMT_THROTTLE
#include <bdlmt_throttle.h>
#endif

                 // ====================================
                 // Implementation Details: Do *NOT* Use
                 // ====================================

// Private macros, see doc in IMPLEMENTATION NOTES in .cpp file.

#define BALL_LOGTHROTTLE_STREAM_CONST_IMP(SEVERITY,                           \
                                          MAX_SIMULTANEOUS_MESSAGES,          \
                                          NANOSECONDS_PER_MESSAGE)            \
for (const BloombergLP::ball::CategoryHolder *ball_logthrottle_cAtEgOrYhOlDeR \
             = BloombergLP::ball::Log::categoryHolderIfEnabled<(SEVERITY)>(   \
                        ball_log_getCategoryHolder(BALL_LOG_CATEGORYHOLDER)); \
     ball_logthrottle_cAtEgOrYhOlDeR;                                         \
     ball_logthrottle_cAtEgOrYhOlDeR = 0)                                     \
for (static BloombergLP::bdlmt::Throttle ball_logthrottle_tHrOtTlE =          \
                             BDLMT_THROTTLE_INIT((MAX_SIMULTANEOUS_MESSAGES), \
                                                 (NANOSECONDS_PER_MESSAGE));  \
     ball_logthrottle_cAtEgOrYhOlDeR                                          \
    && ball_logthrottle_tHrOtTlE.requestPermission();                         \
     )                                                                        \
for (BloombergLP::ball::Log_Stream ball_log_lOg_StReAm(                       \
                                 ball_logthrottle_cAtEgOrYhOlDeR->category(), \
                                 __FILE__,                                    \
                                 __LINE__,                                    \
                                 (SEVERITY));                                 \
     ball_logthrottle_cAtEgOrYhOlDeR;                                         \
     ball_logthrottle_cAtEgOrYhOlDeR = 0)

#define BALL_LOGTHROTTLE_STREAM_IMP(SEVERITY,                                 \
                                    MAX_SIMULTANEOUS_MESSAGES,                \
                                    NANOSECONDS_PER_MESSAGE)                  \
for (const BloombergLP::ball::CategoryHolder *ball_logthrottle_cAtEgOrYhOlDeR \
                       = ball_log_getCategoryHolder(BALL_LOG_CATEGORYHOLDER); \
     ball_logthrottle_cAtEgOrYhOlDeR                                          \
    && ball_logthrottle_cAtEgOrYhOlDeR->threshold() >= (SEVERITY)             \
    && BloombergLP::ball::Log::isCategoryEnabled(                             \
                                          ball_logthrottle_cAtEgOrYhOlDeR,    \
                                          (SEVERITY));                        \
     ball_logthrottle_cAtEgOrYhOlDeR = 0)                                     \
for (static BloombergLP::bdlmt::Throttle ball_logthrottle_tHrOtTlE =          \
                     BDLMT_THROTTLE_INIT((MAX_SIMULTANEOUS_MESSAGES),         \
                                                 (NANOSECONDS_PER_MESSAGE));  \
     ball_logthrottle_cAtEgOrYhOlDeR                                          \
    && ball_logthrottle_tHrOtTlE.requestPermission();                         \
     )                                                                        \
for (BloombergLP::ball::Log_Stream ball_log_lOg_StReAm(                       \
                                 ball_logthrottle_cAtEgOrYhOlDeR->category(), \
                                 __FILE__,                                    \
                                 __LINE__,                                    \
                                 (SEVERITY));                                 \
     ball_logthrottle_cAtEgOrYhOlDeR;                                         \
     ball_logthrottle_cAtEgOrYhOlDeR = 0)

#define BALL_LOGTHROTTLEVA_CONST_IMP(SEVERITY,                                \
                                     MAX_SIMULTANEOUS_MESSAGES,               \
                                     NANOSECONDS_PER_MESSAGE,                 \
                                     ...)                                     \
do {                                                                          \
    static BloombergLP::bdlmt::Throttle ball_logthrottle_tHrOtTlE =           \
                             BDLMT_THROTTLE_INIT((MAX_SIMULTANEOUS_MESSAGES), \
                                                 (NANOSECONDS_PER_MESSAGE));  \
    const BloombergLP::ball::CategoryHolder *ball_logthrottle_cAtEgOrYhOlDeR  \
             = BloombergLP::ball::Log::categoryHolderIfEnabled<(SEVERITY)>(   \
                        ball_log_getCategoryHolder(BALL_LOG_CATEGORYHOLDER)); \
    if (ball_logthrottle_cAtEgOrYhOlDeR &&                                    \
                            ball_logthrottle_tHrOtTlE.requestPermission()) {  \
        BloombergLP::ball::Log_Formatter ball_log_fOrMaTtEr(                  \
                                 ball_logthrottle_cAtEgOrYhOlDeR->category(), \
                                 __FILE__,                                    \
                                 __LINE__,                                    \
                                 (SEVERITY));                                 \
        BloombergLP::ball::Log::format(ball_log_fOrMaTtEr.messageBuffer(),    \
                                       ball_log_fOrMaTtEr.messageBufferLen(), \
                                       __VA_ARGS__);                          \
    }                                                                         \
} while(0)

                     // ====================================
                     // 'C++ Stream-Style' throttling macros
                     // ====================================

#define BALL_LOGTHROTTLE_STREAM(SEVERITY,                                     \
                                MAX_SIMULTANEOUS_MESSAGES,                    \
                                NANOSECONDS_PER_MESSAGE)                      \
    BALL_LOGTHROTTLE_STREAM_IMP((SEVERITY),                                   \
                                (MAX_SIMULTANEOUS_MESSAGES),                  \
                                (NANOSECONDS_PER_MESSAGE))                    \
                                                         BALL_LOG_OUTPUT_STREAM

#define BALL_LOGTHROTTLE_TRACE(                                               \
                          MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \
    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_TRACE,   \
                                      (MAX_SIMULTANEOUS_MESSAGES),            \
                                      (NANOSECONDS_PER_MESSAGE))              \
                                                         BALL_LOG_OUTPUT_STREAM

#define BALL_LOGTHROTTLE_DEBUG(                                               \
                          MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \
    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_DEBUG,   \
                                      (MAX_SIMULTANEOUS_MESSAGES),            \
                                      (NANOSECONDS_PER_MESSAGE))              \
                                                         BALL_LOG_OUTPUT_STREAM

#define BALL_LOGTHROTTLE_INFO(                                                \
                          MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \
    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_INFO,    \
                                      (MAX_SIMULTANEOUS_MESSAGES),            \
                                      (NANOSECONDS_PER_MESSAGE))              \
                                                         BALL_LOG_OUTPUT_STREAM

#define BALL_LOGTHROTTLE_WARN(                                                \
                          MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \
    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_WARN,    \
                                      (MAX_SIMULTANEOUS_MESSAGES),            \
                                      (NANOSECONDS_PER_MESSAGE))              \
                                                         BALL_LOG_OUTPUT_STREAM

#define BALL_LOGTHROTTLE_ERROR(                                               \
                          MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \
    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_ERROR,   \
                                      (MAX_SIMULTANEOUS_MESSAGES),            \
                                      (NANOSECONDS_PER_MESSAGE))              \
                                                         BALL_LOG_OUTPUT_STREAM

#define BALL_LOGTHROTTLE_FATAL(                                               \
                          MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \
    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_FATAL,   \
                                      (MAX_SIMULTANEOUS_MESSAGES),            \
                                      (NANOSECONDS_PER_MESSAGE))              \
                                                         BALL_LOG_OUTPUT_STREAM

                        // ===============================
                        // 'Block-Style' throttling macros
                        // ===============================

#define BALL_LOGTHROTTLE_BLOCK(SEVERITY,                                      \
                               MAX_SIMULTANEOUS_MESSAGES,                     \
                               NANOSECONDS_PER_MESSAGE)                       \
    BALL_LOGTHROTTLE_STREAM_IMP((SEVERITY),                                   \
                                (MAX_SIMULTANEOUS_MESSAGES),                  \
                                (NANOSECONDS_PER_MESSAGE))

#define BALL_LOGTHROTTLE_TRACE_BLOCK(                                         \
                          MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \
    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_TRACE,   \
                                      (MAX_SIMULTANEOUS_MESSAGES),            \
                                      (NANOSECONDS_PER_MESSAGE))

#define BALL_LOGTHROTTLE_DEBUG_BLOCK(                                         \
                          MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \
    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_DEBUG,   \
                                      (MAX_SIMULTANEOUS_MESSAGES),            \
                                      (NANOSECONDS_PER_MESSAGE))

#define BALL_LOGTHROTTLE_INFO_BLOCK(                                          \
                          MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \
    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_INFO,    \
                                      (MAX_SIMULTANEOUS_MESSAGES),            \
                                      (NANOSECONDS_PER_MESSAGE))

#define BALL_LOGTHROTTLE_WARN_BLOCK(                                          \
                          MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \
    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_WARN,    \
                                      (MAX_SIMULTANEOUS_MESSAGES),            \
                                      (NANOSECONDS_PER_MESSAGE))

#define BALL_LOGTHROTTLE_ERROR_BLOCK(                                         \
                          MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \
    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_ERROR,   \
                                      (MAX_SIMULTANEOUS_MESSAGES),            \
                                      (NANOSECONDS_PER_MESSAGE))

#define BALL_LOGTHROTTLE_FATAL_BLOCK(                                         \
                          MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \
    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_FATAL,   \
                                      (MAX_SIMULTANEOUS_MESSAGES),            \
                                      (NANOSECONDS_PER_MESSAGE))

                      // ================================
                      // 'printf-style' throttling macros
                      // ================================

#define BALL_LOGTHROTTLEVA(SEVERITY,                                          \
                           MAX_SIMULTANEOUS_MESSAGES,                         \
                           NANOSECONDS_PER_MESSAGE,                           \
                           ...)                                               \
do {                                                                          \
    static BloombergLP::bdlmt::Throttle ball_logthrottle_tHrOtTlE =           \
                             BDLMT_THROTTLE_INIT((MAX_SIMULTANEOUS_MESSAGES), \
                                                 (NANOSECONDS_PER_MESSAGE));  \
    const BloombergLP::ball::CategoryHolder *ball_logthrottle_cAtEgOrYhOlDeR  \
                      = ball_log_getCategoryHolder(BALL_LOG_CATEGORYHOLDER);  \
    if (ball_logthrottle_cAtEgOrYhOlDeR->threshold() >= (SEVERITY)            \
       && BloombergLP::ball::Log::isCategoryEnabled(                          \
                                            ball_logthrottle_cAtEgOrYhOlDeR,  \
                                            (SEVERITY))                       \
       && ball_logthrottle_tHrOtTlE.requestPermission()) {                    \
        BloombergLP::ball::Log_Formatter ball_log_fOrMaTtEr(                  \
                                 ball_logthrottle_cAtEgOrYhOlDeR->category(), \
                                 __FILE__,                                    \
                                 __LINE__,                                    \
                                 (SEVERITY));                                 \
        BloombergLP::ball::Log::format(ball_log_fOrMaTtEr.messageBuffer(),    \
                                       ball_log_fOrMaTtEr.messageBufferLen(), \
                                       __VA_ARGS__);                          \
    }                                                                         \
} while(0)

#define BALL_LOGTHROTTLEVA_TRACE(MAX_SIMULTANEOUS_MESSAGES,                   \
                                 NANOSECONDS_PER_MESSAGE,                     \
                                 ...)                                         \
    BALL_LOGTHROTTLEVA_CONST_IMP(BloombergLP::ball::Severity::e_TRACE,        \
                                 (MAX_SIMULTANEOUS_MESSAGES),                 \
                                 (NANOSECONDS_PER_MESSAGE),                   \
                                 __VA_ARGS__)

#define BALL_LOGTHROTTLEVA_DEBUG(MAX_SIMULTANEOUS_MESSAGES,                   \
                                 NANOSECONDS_PER_MESSAGE,                     \
                                 ...)                                         \
    BALL_LOGTHROTTLEVA_CONST_IMP(BloombergLP::ball::Severity::e_DEBUG,        \
                                 (MAX_SIMULTANEOUS_MESSAGES),                 \
                                 (NANOSECONDS_PER_MESSAGE),                   \
                                 __VA_ARGS__)

#define BALL_LOGTHROTTLEVA_INFO( MAX_SIMULTANEOUS_MESSAGES,                   \
                                 NANOSECONDS_PER_MESSAGE,                     \
                                 ...)                                         \
    BALL_LOGTHROTTLEVA_CONST_IMP(BloombergLP::ball::Severity::e_INFO,         \
                                 (MAX_SIMULTANEOUS_MESSAGES),                 \
                                 (NANOSECONDS_PER_MESSAGE),                   \
                                 __VA_ARGS__)

#define BALL_LOGTHROTTLEVA_WARN( MAX_SIMULTANEOUS_MESSAGES,                   \
                                 NANOSECONDS_PER_MESSAGE,                     \
                                 ...)                                         \
    BALL_LOGTHROTTLEVA_CONST_IMP(BloombergLP::ball::Severity::e_WARN,         \
                                 (MAX_SIMULTANEOUS_MESSAGES),                 \
                                 (NANOSECONDS_PER_MESSAGE),                   \
                                 __VA_ARGS__)

#define BALL_LOGTHROTTLEVA_ERROR(MAX_SIMULTANEOUS_MESSAGES,                   \
                                 NANOSECONDS_PER_MESSAGE,                     \
                                 ...)                                         \
    BALL_LOGTHROTTLEVA_CONST_IMP(BloombergLP::ball::Severity::e_ERROR,        \
                                 (MAX_SIMULTANEOUS_MESSAGES),                 \
                                 (NANOSECONDS_PER_MESSAGE),                   \
                                 __VA_ARGS__)

#define BALL_LOGTHROTTLEVA_FATAL(MAX_SIMULTANEOUS_MESSAGES,                   \
                                 NANOSECONDS_PER_MESSAGE,                     \
                                 ...)                                         \
    BALL_LOGTHROTTLEVA_CONST_IMP(BloombergLP::ball::Severity::e_FATAL,        \
                                 (MAX_SIMULTANEOUS_MESSAGES),                 \
                                 (NANOSECONDS_PER_MESSAGE),                   \
                                 __VA_ARGS__)

#endif

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
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
