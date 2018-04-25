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
//         BALL_LOGTHROTTLEVA_TRACE,             BALL_LOGTHROTTLEVA_DEBUG,
//         BALL_LOGTHROTTLEVA_INFO,              BALL_LOGTHROTTLEVA_WARN,
//         BALL_LOGTHROTTLEVA_ERROR,             BALL_LOGTHROTTLEVA_FATAL,
//         BALL_LOGTHROTTLEVA,
//
//         BALL_LOGTHROTTLE_TRACE_BLOCK,         BALL_LOGTHROTTLE_DEBUG_BLOCK,
//         BALL_LOGTHROTTLE_INFO_BLOCK,          BALL_LOGTHROTTLE_WARN_BLOCK,
//         BALL_LOGTHROTTLE_ERROR_BLOCK,         BALL_LOGTHROTTLE_FATAL_BLOCK,
//         BALL_LOGTHROTTLE_BLOCK
//
//@SEE ALSO: ball_log, bdlmt_throttle
//
//@AUTHOR: Bill Chapman (bchapman2)
//
//@DESCRIPTION: This component provides numerous throttling macros analogous to
// their corresponding macros in 'ball_log'.  For example, the throtting
// version of 'BALL_LOG_INFO' is 'BALL_LOGTHROTTLE_INFO', or the throttling
// version of 'BALL_LOGVA' is 'BALL_LOGTHROTTLEVA'.
//
// Those macros that don't have a severity included in their names take 3
// arguments -- 'severity', 'maxSimulataneousMessages', and
// 'NANOSECONDS_PER_MESSAGE', where the meanings of the 'severity' values are
// defined in 'ball_severity'.
//
///Throttling Concepts: 'Leaky Bucket' and 'Time Debt'
///--------------------------------------------------
// As clients attemtp to output messages the throttle instantiated by each of
// these macros accumulates a time debt for those actions that dissipates over
// time.  The maximum value for this time debt is given by
// 'MAX_SIMULTANEOUS_MESSAGES * NANOSECONDS_PER_MESSAGE'.  The
// 'MAX_SIMULTANEOUS_MESSAGES' configuration parameter thereby limits the
// maximum number of actions that can be permitted in a very short time, and
// debt is 'paid off' at a rate of '1 / NANOSECONDS_PER_MESSAGE'.
//
// This behavior is known as a "leaky-bucket" algorithm: messages permitted
// place water in the bucket, the passage of time drains water from the bucket,
// and the bucket has a maximum capacity.  Messages are permitted when there is
// enough empty room in the bucket that the water placed won't overflow it.  A
// leaky bucket is an efficiently implementable approximation for allowing a
// certain number of actions over a window of time.
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
///Stream Logging Macros
///- - - - - - - - - - -
// The 'BALL_LOGTHROTTLE_*' macros are analogous to the 'BALL_LOG_*' macros,
// except that they take two throttle-related arguments,
// 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE'.  Use of these
// macros has the following pattern:
//..
//  static const int MSA = 5;             // max simultaneous actions
//  static const bsls::Types::Int64 NPA = 10LL * 1000 * 1000 * 1000;
//                                        // 10 seconds: nanoseconds per action
//
//  BALL_LOGTHROTTLE_TRACE(MSA, NPA) << X << Y ... ;
//  BALL_LOGTHROTTLE_DEBUG(MSA, NPA) << X << Y ... ;
//  BALL_LOGTHROTTLE_INFO( MSA, NPA) << X << Y ... ;
//  BALL_LOGTHROTTLE_WARN( MSA, NPA) << X << Y ... ;
//  BALL_LOGTHROTTLE_ERROR(MSA, NPA) << X << Y ... ;
//  BALL_LOGTHROTTLE_FATAL(MSA, NPA) << X << Y ... ;
//      where X, Y, ... represents any sequence of values for which
//      'operator<<' is defined.  The resulting formatted message string is
//      logged with the severity indicated by the name of the macro
//      (e.g., 'BALL_LOGTHROTTLE_TRACE' logs with severity
//      'ball::Severity::e_TRACE').
//..
// Note that the 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE'
// arguments must be compile-time constants and thus cannot contain any
// floating point subexpressions.
//
// A closely-related macro also based on C++ streams,
// 'BALL_LOGTHROTTLE_STREAM', requires that the severity be explicitly supplied
// as an argument:
//..
//  BALL_LOG_STREAM(severity, MSA, NPA) << X << Y ... ;
//      where X, Y, ... represents any sequence of values for which
//      'operator<<' is defined.  The resulting formatted message string is
//      logged with the specified 'severity'.
//..
// Here, 'severity' can be a variable, but 'MSA' and 'NPA' must still be
// compile-time constants.
//
///'printf'-Style Macros
///- - - - - - - - - - -
// The remaining macros are based on 'printf'-style format specifications,
// where each 'BALL_LOGTHROTTLEVA*' macro corresponds to a 'BALL_LOGVA*' macro
// in 'ball_log', with the 'BALL_LOGTHROTTLEVA' macro performing the same
// function, with throttling added.
//..
//  static const int MSA = 5;             // max simultaneous actions
//  static const bsls::Types::Int64 NPA = 10LL * 1000 * 1000 * 1000;
//                                        // 10 seconds: nanoseconds per action
//
//  BALL_LOGTHROTTLEVA_TRACE(MSA, NPA, msg, ...);
//  BALL_LOGTHROTTLEVA_DEBUG(MSA, NPA, msg, ...);
//  BALL_LOGTHROTTLEVA_INFO( MSA, NPA, msg, ...);
//  BALL_LOGTHROTTLEVA_WARN( MSA, NPA, msg, ...);
//  BALL_LOGTHROTTLEVA_ERROR(MSA, NPA, msg, ...);
//  BALL_LOGTHROTTLEVA_FATAL(MSA, NPA, msg, ...);
//      Format the specified '...' optional arguments, if any, according to the
//      'printf'-style format specification in the specified 'msg' (assumed to
//      be of type convertible to 'const char *') and log the resulting
//      formatted message string with the severity indicated by the name of the
//      macro (e.g., 'BALL_LOGVA_INFO' logs with severity
//      'ball::Severity::e_INFO') with the specified 'MSA' and 'NPA'
//      controlling the throttling.  'MSA' and 'NPA' must be compile-time
//      constants, though 'msg' can be variable.  The behavior is undefined
//      unless the number and types of optional arguments are compatible with
//      the format specification in 'msg'.  Note that each use of these macros
//      must be terminated by a ';'.
//..
// A closely related macro is 'BALL_LOGTHROTTLEVA' which takes the 'severity'
// as a variable, rather than being part of the macro name.
//..
//  BALL_LOGTHROTTLEVA(severity, MSA, NPA, msg, ...);
//..
// Here, 'severity' and 'msg' can be variables, but 'MSA' and 'NPA' must still
// be compile-time constants.
//
///Usage
///-----
///Example 1: A 'printf'-Style Example
///- - - - - - - - - - - - - - - - - -
// Suppose a computer is reading 'double's from a radio receiver, ten per
// second, which represent readings of radiation detected by a Geiger counter
// on a spacecraft and transmitted to a ground control at Jet Propulsion
// Laboratories in California.
//
// The readings are returned by the 'RaditionMeterReceiver' object, from a
// manipulator 'double yield();', which blocks until it returns.  If called in
// a tight loop, it will return ten readings from the spacecraft per second.
//
// Readings range from 0 to 100.
//: o Readings above 10 are a concern, but not very serious.  We will report
//:   those with an 'INFO' severity, and not more than one per hour.
//: o Readings above 30 are more of a worry.  We will report those with a a
//:   'WARN' severity, and not more than five per hour.
//: o Readings above 60 are very serious, which will be reported with 'ERROR'
//:   severity, and not more than twenty per hour.
//: o Readings above 90 are potentially catastrophic, will be reported with
//:   'FATAL' severity, with no limit on the number of readings reported.
//
// We are to write a deamon process, which will loop gathering readings until
// it reads an impossible value of -1.0, which indicates that the daemon is to
// terminate.
//..
//  void radiationMonitorPrintfDaemon()
//  {
//      BALL_LOG_SET_CATEGORY("RADIATION.MONITOR");
//
//      enum {
//          k_SECOND_NS = 1000 * 1000 * 1000,
//          k_NUM_ERROR = 20,
//          k_NUM_WARN  =  5,
//          k_NUM_INFO  =  1 };
//
//      static const bsls::Types::Int64 hour = 60LL * 60 * k_SECOND_NS;
//                                                    // an hour in nanoseconds
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
//              BALL_LOGVA_FATAL("Fatal radiation reading of %g", reading);
//          }
//..
// Next, we deal with 'error' messages that aren't as severe as 'fatal':
//..
//          else if (60 < reading) {
//              BALL_LOGTHROTTLEVA_ERROR(
//                                   k_NUM_ERROR,
//                                   hour,
//                                   "Error radiation reading of %g", reading);
//          }
//..
// Now, we deal wtih 'warning' messages less severe than 'error' readings:
//..
//          else if (30 < reading) {
//              BALL_LOGTHROTTLEVA_WARN(
//                                 k_NUM_WARN,
//                                 hour,
//                                 "Warning radiation reading of %g", reading);
//          }
//..
// Finally, we deal wtih 'info' messages less severe than 'warning' readings:
//..
//          else if (10 < reading) {
//              BALL_LOGTHROTTLEVA_INFO(
//                                    k_NUM_INFO,
//                                    hour,
//                                    "Info radiation reading of %g", reading);
//          }
//      }
//
//      BALL_LOGVA_DEBUG("Finished gathering data.");
//  }
//..
// Where the values returned by 'receiver.yield()' are:
//..
//  0 0 12.3 0 10.5 33.1 11.9 53.7 0 0 46.1 14.7 67.4 43.9 53.3 98.2 0 22.3
//  77.3 36.2 0 17.7 52.5 0 43.2 0 72.9 0 51.9 71.2 92.4 0 0 11.8 33.1 0 47.2
//  15.5 35.7 0 22.3 17.6 0 52.7 0 22.1 -1
//..
// Where
//: o 13 readings of '0.0', which don't produce output, occurred.
//: o 9 readings in the range '10.0 < x <= 30.0', which correspond to 'INFO'
//:   level traces, occurred.
//: o 13 readings in the range '30.0 < x <= 60.0' which correspond to 'WARN'
//:   level traces, occurred.
//: o 5 readings in the range '60.0 < x <= 90.0', which correspond to 'ERROR'
//:   level traces, occurred.
//: o 2 readings in the range '90.0 < x', which correspond to 'FATAL' level
//:   traces, occurred.
// Note that only 1 'INFO' trace and 5 'WARN' traces are permitted by the
// throttle within the (very long) time period of one hour, so the other traces
// at those levels will be suppressed.
//
// 'radiationMonitorPrintfDaemon' produces output like:
//..
//  23AUG2017_17:07:33.401 38510 140675422017312 DEBUG ball_logthrottle.t.cpp
//  438 RADIATION.MONITOR Start gathering data.
//
//  23AUG2017_17:07:33.704 38510 140675422017312 INFO ball_logthrottle.t.cpp
//  473 RADIATION.MONITOR Info radiation reading of 12.3
//
//  23AUG2017_17:07:34.007 38510 140675422017312 WARN ball_logthrottle.t.cpp
//  464 RADIATION.MONITOR Warning radiation reading of 33.1
//
//  23AUG2017_17:07:34.208 38510 140675422017312 WARN ball_logthrottle.t.cpp
//  464 RADIATION.MONITOR Warning radiation reading of 53.7
//
//  23AUG2017_17:07:34.511 38510 140675422017312 WARN ball_logthrottle.t.cpp
//  464 RADIATION.MONITOR Warning radiation reading of 46.1
//
//  23AUG2017_17:07:34.713 38510 140675422017312 ERROR ball_logthrottle.t.cpp
//  455 RADIATION.MONITOR Error radiation reading of 67.4
//
//  23AUG2017_17:07:34.814 38510 140675422017312 WARN ball_logthrottle.t.cpp
//  464 RADIATION.MONITOR Warning radiation reading of 43.9
//
//  23AUG2017_17:07:34.915 38510 140675422017312 WARN ball_logthrottle.t.cpp
//  464 RADIATION.MONITOR Warning radiation reading of 53.3
//
//  23AUG2017_17:07:35.016 38510 140675422017312 FATAL ball_logthrottle.t.cpp
//  446 RADIATION.MONITOR Fatal radiation reading of 98.2
//
//  23AUG2017_17:07:35.318 38510 140675422017312 ERROR ball_logthrottle.t.cpp
//  455 RADIATION.MONITOR Error radiation reading of 77.3
//
//  23AUG2017_17:07:36.124 38510 140675422017312 ERROR ball_logthrottle.t.cpp
//  455 RADIATION.MONITOR Error radiation reading of 72.9
//
//  23AUG2017_17:07:36.426 38510 140675422017312 ERROR ball_logthrottle.t.cpp
//  455 RADIATION.MONITOR Error radiation reading of 71.2
//
//  23AUG2017_17:07:36.529 38510 140675422017312 FATAL ball_logthrottle.t.cpp
//  446 RADIATION.MONITOR Fatal radiation reading of 92.4
//
//  23AUG2017_17:07:38.041 38510 140675422017312 DEBUG ball_logthrottle.t.cpp
//  477 RADIATION.MONITOR Finished gathering data.
//..
// Note that 8 'INFO' messages and 8 'WARN' messages were suppressed by
// the throttling.
//
///Example 2: A 'stream'-Style Example
///- - - - - - - - - - - - - - - - - -
// Here, we just repeat exactly the same code, using the 'stream'-style macros
// instead of the 'printf'-style macros:
//
//  void radiationMonitorStreamDaemon()
//  {
//      BALL_LOG_SET_CATEGORY("RADIATION.MONITOR");
//
//      enum {
//          k_NUM_ERROR = 20,
//          k_NUM_WARN  =  5,
//          k_NUM_INFO  =  1 };
//
//      const double hour = 60.0 * 60.0;    // an hour in seconds
//
//      RadiationMeterReceiver receiver;
//
//      BALL_LOG_DEBUG << "Start gathering data.";
//
//      double reading;
//      while (-1.0 != (reading = receiver.yield())) {
//..
// First, we deal with 'fatal' messages:
//..
//          if      (90 < reading) {
//              BALL_LOG_FATAL << "Fatal radiation reading of " << reading;
//          }
//..
// Next, we deal with 'error' messages that aren't as severe as 'fatal':
//..
//          else if (60 < reading) {
//              BALL_LOGTHROTTLE_ERROR(k_NUM_ERROR, hour) <<
//                                    "Error radiation reading of " << reading;
//          }
//..
// Now, we deal wtih 'warning' messages less severe than 'error' readings:
//..
//          else if (30 < reading) {
//              BALL_LOGTHROTTLE_WARN(k_NUM_WARN, hour) <<
//                                  "Warning radiation reading of " << reading;
//          }
//..
// Finally, we deal wtih 'info' messages less severe than 'warning' readings:
//..
//          else if (10 < reading) {
//              BALL_LOGTHROTTLE_INFO(k_NUM_INFO, hour) <<
//                                     "Info radiation reading of " << reading;
//          }
//      }
//
//      BALL_LOG_DEBUG << "Finished gathering data.";
//  }
//..
// The output would be identical to the output produced by usage example 1.

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

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

                           // =======================
                           // C++ stream-based macros
                           // =======================

// Private macro, see doc in IMPLEMENTATION NOTES in .cpp file.

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
     ball_logthrottle_cAtEgOrYhOlDeR = 0)                                     \
for (BloombergLP::ball::Log_Stream ball_log_lOg_StReAm(                       \
                                 ball_logthrottle_cAtEgOrYhOlDeR->category(), \
                                 __FILE__,                                    \
                                 __LINE__,                                    \
                                 (SEVERITY));                                 \
     ball_logthrottle_cAtEgOrYhOlDeR;                                         \
     ball_logthrottle_cAtEgOrYhOlDeR = 0)

// Private macro, see doc in IMPLEMENTATION NOTES in .cpp file.

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
     ball_logthrottle_cAtEgOrYhOlDeR = 0)                                     \
for (BloombergLP::ball::Log_Stream ball_log_lOg_StReAm(                       \
                                 ball_logthrottle_cAtEgOrYhOlDeR->category(), \
                                 __FILE__,                                    \
                                 __LINE__,                                    \
                                 (SEVERITY));                                 \
     ball_logthrottle_cAtEgOrYhOlDeR;                                         \
     ball_logthrottle_cAtEgOrYhOlDeR = 0)

// 'BALL_LOGTHROTTLE_STREAM' creates a static 'bdlt::Throttle' object with the
// specified 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE', and, if
// the specified 'SEVERITY' is less than or equal to (lower values are more
// severe') the category threshold and the 'time debt' (see component doc)
// registered in the throttle is not execessive, creates a 'ball::Log_Stream'
// object, with any '<<' operators following this macro applied to that object.
// If the severity is greater than (less severe than) the category threshold or
// the time debt is excessive, the 'ball::Log_Stream' object is not created and
// the '<<' operators following it are not executed.  Note that, while
// 'SEVERITY' may be a variable, the other two arguments of this macro must be
// compile-time constants containing no floating-point subexpressions.

#define BALL_LOGTHROTTLE_STREAM(SEVERITY,                                     \
                                MAX_SIMULTANEOUS_MESSAGES,                    \
                                NANOSECONDS_PER_MESSAGE)                      \
    BALL_LOGTHROTTLE_STREAM_IMP((SEVERITY),                                   \
                                (MAX_SIMULTANEOUS_MESSAGES),                  \
                                (NANOSECONDS_PER_MESSAGE))                    \
                                                         BALL_LOG_OUTPUT_STREAM

// The 'BALL_LOGTHROTTLE_{severity}' macros create a static 'bdlt::Throttle'
// object with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
// 'NANOSECONDS_PER_MESSAGE', and, if 'severity' is less than or equal to
// (lower values are more severe') than the category threshold and the
// 'time debt' (see component doc) registered in the throttle is not
// execessive, a 'ball::Log_Stream' object is created and any '<<' operators
// following the macro are executed and applied to it.  If the severity is
// greater than (less severe than) the category threshold or the time debt is
// excessive, the 'ball::Log_Stream' object is not created and the '<<'
// operators following the macro are not executed.  Note that the two arguments
// of these macros must be compile-time constants containing no floating-point
// subexpressions.

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

// 'BALL_LOGTHROTTLE_STREAM_BLOCK' creates a static 'bdlt::Throttle' object
// with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
// 'NANOSECONDS_PER_MESSAGE', and, if the specified 'SEVERITY' is less than or
// equal to (lower values are more severe') then the category threshold and the
// 'time debt' (see component doc) registered in the throttle is not
// execessive, executes the statement or block that follows the macro.  If the
// severity is greater than (less severe than) the category threshold or the
// time debt is excessive, the statement or block following the macro is not
// executed.  Note that, while 'SEVERITY' may be a variable, the other two
// arguments of this macro must be compile-time constants containing no
// floating-point subexpressions.
//
// If you want to access the stream within the controlled block or statement,
// use 'BALL_LOG_OUTPUT_STREAM' as defined by 'ball_log.h'.  There is no
// 'BALL_LOGTHROTTLE_OUTPUT_STREAM'.

#define BALL_LOGTHROTTLE_BLOCK(SEVERITY,                                      \
                               MAX_SIMULTANEOUS_MESSAGES,                     \
                               NANOSECONDS_PER_MESSAGE)                       \
    BALL_LOGTHROTTLE_STREAM_IMP((SEVERITY),                                   \
                                (MAX_SIMULTANEOUS_MESSAGES),                  \
                                (NANOSECONDS_PER_MESSAGE))

// 'BALL_LOGTHROTTLE_STREAM_{severity}_BLOCK' creates a static 'bdlt::Throttle'
// object with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
// 'NANOSECONDS_PER_MESSAGE', and, if the specified 'severity' is less than or
// equal to (lower values are more severe') than the category threshold and the
// 'time debt' (see component doc) registered in the throttle is not
// execessive, executes the statement or block that follows the macro.  If the
// severity is greater than (less severe than) the category threshold or the
// time debt is excessive, the statement or block following the macro is not
// executed.  Note that both arguments of this macro must be compile-time
// constants containing no floating-point subexpressions.
//
// If you want to access the stream within the controlled block or statement,
// use 'BALL_LOG_OUTPUT_STREAM' as defined by 'ball_log.h'.  There is no
// 'BALL_LOGTHROTTLE_OUTPUT_STREAM'.

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

                           // =====================
                           // 'printf'-style macros
                           // =====================

// ----------------------------------------------------------------------------
// Usage: BALL_LOGTHROTTLEVA_CONST_IMP(SEVERITY,
//                                     MAX_SIMULTANEOUS_MESSAGES,
//                                     NANOSECONDS_PER_MESSAGE,
//                                     msg, arg1, arg2, ... argN);
//
// Where the specified 'SEVERITY' is from the enum 'ball::Severity::Level'.
// This macro creates a static 'bdlmt::Throttle' object configured with the
// specified 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE'.  If
// 'SEVERITY' is less than (lower severity values are more severe) the category
// threshold and the time debt (see component doc) of the throttle is not
// excessive, log a message with the 'printf'-style format string msg and
// arguments 'arg1, ... argN'.  Note that this macro requires the all three of
// the first arguments to be compile-time constants, none of which may contain
// any floating-point subexpressions, but there is no such requirement on any
// of the other arguments.

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

// ----------------------------------------------------------------------------
// Usage: BALL_LOGTHROTTLEVA(SEVERITY,
//                           MAX_SIMULTANEOUS_MESSAGES,
//                           NANOSECONDS_PER_MESSAGE,
//                           msg, arg1, arg2, ... argN);
//
// Where the specified 'SEVERITY' is from the enum 'ball::Severity::Level'.
// This macro creates a static 'bdlmt::Throttle' object configured with the
// specified 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE'.  If
// 'SEVERITY' is less than (lower severity values are more severe) the category
// threshold and the time debt (see component doc) of the throttle is not
// excessive, log a message with the 'printf'-style format string msg and
// arguments 'arg1, ... argN'.  Note that this macro requires that the second
// and third arguments to be compile-time constants, neither of which may
// contain any floating-point subexpressions, but there is no such requirement
// on any of the other arguments.

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

// ----------------------------------------------------------------------------
// Usage: BALL_LOGTHROTTLEVA_{SEVERITY}(MAX_SIMULTANEOUS_MESSAGES,
//                                      NANOSECONDS_PER_MESSAGE,
//                                      msg,
//                                      arg1, arg2, ... argN);
//
// Where the specified 'SEVERITY' is from the enum
// 'ball::Severity::e_{SEVERITY}'.  This macro creates a static
// 'bdlmt::Throttle' object configured with the specified
// 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE'.  If 'SEVERITY' is
// less than (lower severity values are more severe) the category threshold and
// the time debt (see component doc) of the throttle is not excessive, log a
// message with the 'printf'-style format string msg and arguments
// 'arg1, ... argN'.  Note that this macro requires that both of the first
// arguments to be compile-time constants, neither of which may contain any
// floating-point subexpressions, but there is no such requirement on any of
// the other arguments.

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
