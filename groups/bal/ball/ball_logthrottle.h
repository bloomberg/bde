// ball_logthrottle.h                                                 -*-C++-*-
#ifndef INCLUDED_BALL_LOGTHROTTLE
#define INCLUDED_BALL_LOGTHROTTLE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide throttling equivalents of some of the 'ball_log' macros.
//
//@MACROS:
//  BALL_LOGTHROTTLE_TRACE(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//  BALL_LOGTHROTTLE_DEBUG(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//  BALL_LOGTHROTTLE_INFO(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//  BALL_LOGTHROTTLE_WARN(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//  BALL_LOGTHROTTLE_ERROR(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//  BALL_LOGTHROTTLE_FATAL(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//  BALL_LOGTHROTTLE_STREAM(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//
//  BALL_LOGTHROTTLE_TRACE_BLOCK(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//  BALL_LOGTHROTTLE_DEBUG_BLOCK(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//  BALL_LOGTHROTTLE_INFO_BLOCK(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//  BALL_LOGTHROTTLE_WARN_BLOCK(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//  BALL_LOGTHROTTLE_ERROR_BLOCK(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//  BALL_LOGTHROTTLE_FATAL_BLOCK(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//  BALL_LOGTHROTTLE_BLOCK(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE)
//
//  BALL_LOGTHROTTLEVA_TRACE(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE, ...)
//  BALL_LOGTHROTTLEVA_DEBUG(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE, ...)
//  BALL_LOGTHROTTLEVA_INFO(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE, ...)
//  BALL_LOGTHROTTLEVA_WARN(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE, ...)
//  BALL_LOGTHROTTLEVA_ERROR(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE, ...)
//  BALL_LOGTHROTTLEVA_FATAL(MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE, ...)
//  BALL_LOGTHROTTLEVA(SEV, MAX_SIMULTANEOUS_MSGS, NANOSEC_PER_MESSAGE, ...)
//
//@SEE_ALSO: ball_log, bdlmt_throttle
//
//@DESCRIPTION: This component provides numerous macros for performing logging
// where the number of messages logged is "throttled", meaning that the number
// of messages that will be logged within a given time interval is limited.
// The macros in this component are all analogous to corresponding macros in
// 'ball_log'.  For example, the throttling version of 'BALL_LOG_INFO' is
// 'BALL_LOGTHROTTLE_INFO', and the throttling version of 'BALL_LOGVA' is
// 'BALL_LOGTHROTTLEVA'.
//
// Each log message has a 'SEVERITY' associated with it (see 'ball_severity'
// for the definitions of the six standard severities).  Those macros that
// don't contain a 'SEVERITY' in their name are passed an integral severity
// value in the range '[0 .. 255]', with suggested values defined in
// 'ball::Severity', as their first argument.  The next two arguments,
// 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE', are described
// immediately below.
//
// Each invocation of any of the macros provided by this component instantiates
// its own 'bdlmt::Throttle' object to effect the throttling behavior.  Note
// that each throttle object is statically declared, so it is shared by all
// threads.
//
///Throttling Parameters
///---------------------
// Clients supply the throttling macros with configuration values for
// 'MAX_SIMULTANEOUS_MESSAGESS', and 'NANOSECONDS_PER_MESSAGE'.  The macros
// keep track of the number of logged messages, and over time throttles the
// average number of messages permitted to a rate of '1 / NANOSEC_PER_MESSAGE'
// (messages-per-nanosecond).  So, for example, to limit the average rate of
// messages permitted to 10 messages per second (10 actions / one billion
// nanoseconds), the value for 'NANOSECONDS_PER_MESSAGE' would be
// 100,000,000 (which is one billion / 10).
//
// As client code publishes a log message from a macro, the macro accumulates a
// time debt for each message, which dissipates over time.  The maximum value
// for this time debt is given by
// 'MAX_SIMULTANEOUS_MESSAGES * NANOSECONDS_PER_MESSAGE'.  The
// 'MAX_SIMULTANEOUS_MESSAGES' configuration parameter thereby limits the
// maximum number of messages that can be simultaneously published.
//
//: 'MAX_SIMULTANEOUS_MESSAGES': configures (an approximation of) the maximum
//:    number of messages that can be simultaneously logged.
//:
//: 'NANOSECONDS_PER_MESSAGE': configures (an approximation of) the minimum
//:    period between messages (and, by extension, the maximum rate).
//
// So, for example:
//..
//  static const bsls::Types::Int64 k_NS_PER_S =
//                           bdlt::TimeUnitRatio::k_NANOSECONDS_PER_SECOND;
//
//  // Log 1 WARN-level message every second on average:
//  BALL_LOGTHROTTLE_WARN(1, k_NS_PER_S) << "message 1";
//
//  // Log 1 WARN-level message every second on average, but allow a "burst"
//  // of up to 2 messages to be published simultaneously:
//  BALL_LOGTHROTTLE_WARN(2, k_NS_PER_S) << "message 2";
//..
// Notice that 'NANOSECONDS_PER_MESSAGE' controls an approximation for the
// average rate of messages to be published, and 'MAX_SIMULTANEOUS_MESSAGES'
// controls an approximation for the size of bursts of messages to be
// published.
//
// Note that this component is built on top of 'bdlmt_throttle', and mirrors
// its behavior.
//
///Throttling Concepts
///-------------------
// The behavior implemented by this component is known as a "leaky-bucket"
// algorithm: permitted actions place water in the bucket, the passage
// of time drains water from the bucket, and the bucket has a maximum capacity.
// Actions are permitted when there is enough empty room in the bucket that
// the water placed won't overflow it.  A leaky bucket is an efficiently
// implementable approximation for allowing a certain number of actions over a
// window of time.
//
///Thread Safety
///-------------
// All macros defined in this component are thread-safe, and can be invoked
// concurrently by multiple threads.
//
///Macro Reference
///---------------
// The following constraints pertain to all of the macros defined in this
// component.
//
//: o 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE' must be
//:   compile-time constants, and may not contain any floating-point
//:   subexpressions.
//:
//: o 'MAX_SIMULTANEOUS_MESSAGES', 'NANOSECONDS_PER_MESSAGE', and 'SEVERITY'
//:    are of types 'int', 'bsls::Types::Int64', and 'int', respectively.
//:
//: o The behavior is undefined unless 'SEVERITY' is in the range '[0 .. 255]',
//:   '0 <= MAX_SIMULTANEOUS_MESSAGES', '0 <= NANOSECONDS_PER_MESSAGE',
//:   '0 < MAX_SIMULTANEOUS_MESSAGES || 0 < NANOSECONDS_PER_MESSAGE', and
//:   'MAX_SIMULTANEOUS_MESSAGES * NANOSECONDS_PER_MESSAGE <= LLONG_MAX'.
//
// Each 'BALL_LOGTHROTTLE_*' is analogous to the corresponding 'BALL_LOG_*'
// macro, except that they take two additional throttle-related arguments,
// 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE', described above.
//
///Stream-Based Throttling Macros
/// - - - - - - - - - - - - - - -
// The following macros mirror the corresponding 'BALL_LOG_<SEVERITY>' macros:
//..
//  BALL_LOGTHROTTLE_<SEVERITY>(MAX_SIMULTANEOUS_MESSAGES,
//                              NANOSECONDS_PER_MESSAGE) << X << Y ... ;
//      Throttle logging with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
//      'NANOSECONDS_PER_MESSAGE' as described in {Throttling Parameters}, where
//      'X, Y, ...' represents any sequence of values for which 'operator<<' is
//      defined.  If the throttle permits a message to be logged, the resulting
//      formatted message is logged with the severity indicated by the name of
//      the macro (e.g., 'BALL_LOGTHROTTLE_ERROR' logs with severity
//      'ball::Severity::e_ERROR').
//
//  BALL_LOGTHROTTLE_STREAM(SEVERITY,
//                          MAX_SIMULTANEOUS_MESSAGES,
//                          NANOSECONDS_PER_MESSAGE) << X << Y ... ;
//      Throttle logging with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
//      'NANOSECONDS_PER_MESSAGE' as described in {Throttling Parameters}, where
//      'X, Y, ...' represents any sequence of values for which 'operator<<' is
//      defined.  If the throttle permits a message to be logged, the resulting
//      formatted message is logged with the specified 'SEVERITY'.
//..
//
///BLOCK-Style Throttling Macros
///- - - - - - - - - - - - - - -
// The following macros mirror the corresponding 'BALL_LOG_*_BLOCK' macros:
//..
//  BALL_LOGTHROTTLE_<SEVERITY>_BLOCK(MAX_SIMULTANEOUS_MESSAGES,
//                                    NANOSECONDS_PER_MESSAGE) <block>
//      Throttle logging with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
//      'NANOSECONDS_PER_MESSAGE' as described in {Throttling Parameters}, where
//      any sequence of values for which 'operator<<' is defined may be
//      streamed to 'BALL_LOG_OUTPUT_STREAM' within the controlled '<block>'.
//      If the throttle permits a message to be logged, the resulting formatted
//      message is logged with the severity indicated by the name of the macro
//      (e.g., 'BALL_LOGTHROTTLE_WARN_BLOCK' logs with severity
//      'ball::Severity::e_WARN').
//
//  BALL_LOGTHROTTLE_BLOCK(SEVERITY,
//                         MAX_SIMULTANEOUS_MESSAGES,
//                         NANOSECONDS_PER_MESSAGE) <block>
//      Throttle logging with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
//      'NANOSECONDS_PER_MESSAGE' as described in {Throttling Parameters}, where
//      any sequence of values for which 'operator<<' is defined may be
//      streamed to 'BALL_LOG_OUTPUT_STREAM' within the controlled '<block>'.
//      If the throttle permits a message to be logged, the resulting formatted
//      message is logged with the specified 'SEVERITY'.
//..
//
///'printf'-Style Throttling Macros
/// - - - - - - - - - - - - - - - -
// The following macros mirror the corresponding 'BALL_LOGVA_*' macros:
//..
//  BALL_LOGTHROTTLEVA_<SEVERITY>(MAX_SIMULTANEOUS_MESSAGE,
//                                NANOSECONDS_PER_MESSAGE,
//                                MSG,
//                                ...);
//      Throttle logging with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
//      'NANOSECONDS_PER_MESSAGE' as described in {Throttling Parameters}.  If
//      the throttle permits a message to be logged, format the specified '...'
//      optional arguments, if any, according to the 'printf'-style format
//      specification in the specified 'MSG' (assumed to be of type convertible
//      to 'const char *'), and log the resulting formatted message with the
//      severity indicated by the name of the macro (e.g.,
//      'BALL_LOGTHROTTLEVA_INFO' logs with severity 'ball::Severity::e_INFO').
//      The behavior is undefined unless the number and types of the optional
//      arguments are compatible with the format specification in 'MSG'.  Note
//      that each use of these macros must be terminated by a ';'.
//
//  BALL_LOGTHROTTLEVA(SEVERITY,
//                     MAX_SIMULTANEOUS_MESSAGES,
//                     NANOSECONDS_PER_MESSAGE,
//                     MSG,
//                     ...);
//      Throttle logging with the specified 'MAX_SIMULTANEOUS_MESSAGES' and
//      'NANOSECONDS_PER_MESSAGE' as described in {Throttling Parameters}.  If
//      the throttle permits a message to be logged, format the specified '...'
//      optional arguments, if any, according to the 'printf'-style format
//      specification in the specified 'MSG' (assumed to be of type convertible
//      to 'const char *'), and log the resulting formatted message with the
//      specified 'SEVERITY'.  The behavior is undefined unless the number and
//      types of the optional arguments are compatible with the format
//      specification in 'MSG'.  Note that each use of this macro must be
//      terminated by a ';'.
//..
//
///Usage
///-----
// This section illustrates the intended use of this component.
//
///Example 1: C++ Stream-Style Throttling Macro Usage
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose a computer is reading 'double' values from a radio receiver, ten per
// second, which represent readings of radiation detected by a Geiger counter
// on a spacecraft, and is transmitting them to a ground control at Jet
// Propulsion Laboratories in California.
//
// The readings are returned by the 'double yield()' manipulator of a
// 'RadiationMeterReceiver' object (the implementation of which is omitted).
// The 'yield' method blocks until it obtains a reading to return.  If called
// in a tight loop, 'yield' returns ten readings per second.
//
// Readings range from 0 to 100.
//
//: o Readings above 10 but not greater than 30 are a concern, but are not very
//:   serious.  We will report those with an 'e_TRACE' severity, and at most
//:   one per hour (i.e., messages will be throttled).
//:
//: o Readings above 30 but not greater than 60 are more of a worry.  We will
//:   report those with an 'e_DEBUG' severity, and at most five per hour.
//:
//: o Readings above 60 but not greater than 90 are very serious.  They will be
//:   reported with an 'e_INFO' severity, and at most twenty per hour.
//:
//: o Readings above 90 are potentially catastrophic, and will be reported with
//:   an 'e_WARN' severity, with no limit on the number of readings reported
//:   (i.e., no throttling).
//
// We are to write a daemon process, which will loop gathering readings.  A
// reading of an impossible value of -1.0 will indicate termination.
//
//  First we define a set of useful constants:
//..
//  enum {
//      k_NUM_INFO  = 20,       // max # of info messages in a very short time
//      k_NUM_DEBUG =  5,       // max # of debug messages in a very short time
//      k_NUM_TRACE =  1        // max # of trace messages in a very short time
//  };
//
//  const Int64 k_NS_PER_HOUR =
//                    BloombergLP::bdlt::TimeUnitRatio::k_NANOSECONDS_PER_HOUR;
//
//  const Int64 k_NS_PER_INFO  = k_NS_PER_HOUR / k_NUM_INFO;
//                 // long-term minimum nanoseconds per info message permitted
//  const Int64 k_NS_PER_DEBUG = k_NS_PER_HOUR / k_NUM_DEBUG;
//                 // long-term minimum nanoseconds per debug message permitted
//  const Int64 k_NS_PER_TRACE = k_NS_PER_HOUR / k_NUM_TRACE;
//                 // long-term minimum nanoseconds per trace message permitted
//..
// Then we implement the radiation monitor using the log-throttle macros to
// throttle the number of log records being published:
//..
//  void radiationMonitorStreamDaemon()
//      // Daemon to run the radiation monitor.
//  {
//      BALL_LOG_SET_CATEGORY("RADIATION.MONITOR");
//
//      RadiationMeterReceiver receiver;
//
//      BALL_LOG_DEBUG << "Start gathering data.";
//
//      double reading;
//      while (-1.0 != (reading = receiver.yield())) {
//
//          if (90 < reading) {
//              BALL_LOG_WARN << "Serious Radiation reading of " << reading;
//          }
//
//          else if (60 < reading) {
//              BALL_LOGTHROTTLE_INFO(k_NUM_INFO, k_NS_PER_INFO) <<
//                                          "Radiation reading of " << reading;
//          }
//
//          else if (30 < reading) {
//              BALL_LOGTHROTTLE_DEBUG(k_NUM_DEBUG, k_NS_PER_DEBUG) <<
//                                          "Radiation reading of " << reading;
//          }
//
//          else if (10 < reading) {
//              BALL_LOGTHROTTLE_TRACE(k_NUM_TRACE, k_NS_PER_TRACE) <<
//                                          "Radiation reading of " << reading;
//          }
//      }
//
//      BALL_LOG_DEBUG << "Finished gathering data.";
//  }
//..
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
//..

#include <balscm_version.h>

#include <ball_category.h>
#include <ball_log.h>
#include <ball_severity.h>

#include <bdlmt_throttle.h>

                 // ====================================
                 // Implementation Details: Do *NOT* Use
                 // ====================================

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
                                         (NANOSECONDS_PER_MESSAGE));          \
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
                           BDLMT_THROTTLE_INIT((MAX_SIMULTANEOUS_MESSAGES),   \
                                               (NANOSECONDS_PER_MESSAGE));    \
    const BloombergLP::ball::CategoryHolder                                   \
                                          *ball_logthrottle_cAtEgOrYhOlDeR    \
            = BloombergLP::ball::Log::categoryHolderIfEnabled<(SEVERITY)>(    \
                      ball_log_getCategoryHolder(BALL_LOG_CATEGORYHOLDER));   \
    if (ball_logthrottle_cAtEgOrYhOlDeR &&                                    \
                           ball_logthrottle_tHrOtTlE.requestPermission()) {   \
        BloombergLP::ball::Log_Formatter ball_logthrottle_fOrMaTtEr(          \
                               ball_logthrottle_cAtEgOrYhOlDeR->category(),   \
                               __FILE__,                                      \
                               __LINE__,                                      \
                               (SEVERITY));                                   \
        BloombergLP::ball::Log::format(                                       \
                             ball_logthrottle_fOrMaTtEr.messageBuffer(),      \
                             ball_logthrottle_fOrMaTtEr.messageBufferLen(),   \
                             __VA_ARGS__);                                    \
    }                                                                         \
} while(0)

                      // ==================================
                      // C++ Stream-Style throttling macros
                      // ==================================

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

                         // =============================
                         // BLOCK-Style throttling macros
                         // =============================

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
                       // 'printf'-style throttling macros
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
        BloombergLP::ball::Log_Formatter ball_logthrottle_fOrMaTtEr(          \
                                 ball_logthrottle_cAtEgOrYhOlDeR->category(), \
                                 __FILE__,                                    \
                                 __LINE__,                                    \
                                 (SEVERITY));                                 \
        BloombergLP::ball::Log::format(                                       \
                               ball_logthrottle_fOrMaTtEr.messageBuffer(),    \
                               ball_logthrottle_fOrMaTtEr.messageBufferLen(), \
                               __VA_ARGS__);                                  \
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
