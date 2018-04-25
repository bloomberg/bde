// ball_logthrottle.cpp                                               -*-C++-*-
#include <ball_logthrottle.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ball_logthrottle_cpp,"$Id$ $CSID$")

///IMPLEMENTATION NOTES
///--------------------

                    // ---------------------------------
                    // BALL_LOGTHROTTLE_STREAM_CONST_IMP
                    // ---------------------------------

// The stream-style const throttled logging macro is reproduced here:
//..
//#define BALL_LOGTHROTTLE_STREAM_CONST_IMP(SEVERITY,                        \@
//                                          MAX_SIMULTANEOUS_MESSAGES,       \@
//                                          NANOSECONDS_PER_MESSAGE)         \@
//for (const BloombergLP::ball::CategoryHolder                               \@
//                                          *ball_logthrottle_cAtEgOrYhOlDeR \@
//             = BloombergLP::ball::Log::categoryHolderIfEnabled<(SEVERITY)>(\@
//                     ball_log_getCategoryHolder(BALL_LOG_CATEGORYHOLDER)); \@
//     ball_logthrottle_cAtEgOrYhOlDeR;                                      \@
//     ball_logthrottle_cAtEgOrYhOlDeR = 0)                                  \@
//for (static BloombergLP::bdlmt::Throttle ball_logthrottle_tHrOtTlE =       \@
//                          BDLMT_THROTTLE_INIT((MAX_SIMULTANEOUS_MESSAGES), \@
//                                              (NANOSECONDS_PER_MESSAGE));  \@
//     ball_logthrottle_cAtEgOrYhOlDeR                                       \@
//    && ball_logthrottle_tHrOtTlE.requestPermission();                      \@
//     )                                                                     \@
//for (BloombergLP::ball::Log_Stream ball_log_lOg_StReAm(                    \@
//                              ball_logthrottle_cAtEgOrYhOlDeR->category(), \@
//                              __FILE__,                                    \@
//                              __LINE__,                                    \@
//                              (SEVERITY));                                 \@
//     ball_logthrottle_cAtEgOrYhOlDeR;                                      \@
//     ball_logthrottle_cAtEgOrYhOlDeR = 0)
//..
// Where this would be used, for example, by 'BALL_LOGTHROTTLE_TRACE':
//..
//#define BALL_LOGTHROTTLE_TRACE(                                            \@
//                       MAX_SIMULTANEOUS_MESSAGES, NANOSECONDS_PER_MESSAGE) \@
//    BALL_LOGTHROTTLE_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_TRACE,\@
//                                      (MAX_SIMULTANEOUS_MESSAGES),         \@
//                                      (NANOSECONDS_PER_MESSAGE))           \@
//                                                       BALL_LOG_OUTPUT_STREAM
//..
// Note that '@' is appended to each line in the macro that ends with '\' to
// quell a diagnostic from gcc ("warning: multi-line comment").
//
// 'BALL_LOGTHROTTLE_STREAM_CONST_IMP' creates a static 'bdlt::Throttle' object
// with the specified 'maxSimultaneousMessages' and 'nanosecondsPerMessage',
// and, if the specified 'severity' is less than or equal to (lower values are
// more severe') the category threshold and the 'time debt' (see "Throttling
// Concepts" in component doc) registered in the throttle is not excessive,
// executes the statement or block that follows the macro.  If the severity is
// greater than (less severe than) the category threshold or the time debt is
// excessive, the statement or block following the macro is not executed.  Note
// that all 3 arguments of this macro must be compile-time constants containing
// no floating-point subexpressions.
//
// If you want to access the stream within the controlled block or statement,
// use 'BALL_LOG_OUTPUT_STREAM' as defined by 'ball_log.h'.  There is no
// 'BALL_LOGTHROTTLE_OUTPUT_STREAM'.
//
// The stream-style non-const throttled logging macro is reproduced here.  The
// only difference from the above is that it allows 'SEVERITY' to be a
// run-time, rather than a compile-time, constant:


                        // ---------------------------
                        // BALL_LOGTHROTTLE_STREAM_IMP
                        // ---------------------------

// The stream-style non-const throttled logging macro is reproduced here:
//..
//#define BALL_LOGTHROTTLE_STREAM_IMP(SEVERITY,                              \@
//                                    MAX_SIMULTANEOUS_MESSAGES,             \@
//                                    NANOSECONDS_PER_MESSAGE)               \@
//for (const BloombergLP::ball::CategoryHolder                               \@
//                                          *ball_logthrottle_cAtEgOrYhOlDeR \@
//                    = ball_log_getCategoryHolder(BALL_LOG_CATEGORYHOLDER); \@
//     ball_logthrottle_cAtEgOrYhOlDeR                                       \@
//    && ball_logthrottle_cAtEgOrYhOlDeR->threshold() >= (SEVERITY)          \@
//    && BloombergLP::ball::Log::isCategoryEnabled(                          \@
//                                          ball_logthrottle_cAtEgOrYhOlDeR, \@
//                                          (SEVERITY));                     \@
//     ball_logthrottle_cAtEgOrYhOlDeR = 0)                                  \@
//for (static BloombergLP::bdlmt::Throttle ball_logthrottle_tHrOtTlE =       \@
//                     BDLMT_THROTTLE_INIT((MAX_SIMULTANEOUS_MESSAGES),      \@
//                                         (NANOSECONDS_PER_MESSAGE));       \@
//     ball_logthrottle_cAtEgOrYhOlDeR                                       \@
//    && ball_logthrottle_tHrOtTlE.requestPermission();                      \@
//     )                                                                     \@
//for (BloombergLP::ball::Log_Stream ball_log_lOg_StReAm(                    \@
//                              ball_logthrottle_cAtEgOrYhOlDeR->category(), \@
//                              __FILE__,                                    \@
//                              __LINE__,                                    \@
//                              (SEVERITY));                                 \@
//     ball_logthrottle_cAtEgOrYhOlDeR;                                      \@
//     ball_logthrottle_cAtEgOrYhOlDeR = 0)
//..
// Where this is only used for 'BALL_LOGTHROTTLE_STREAM':
//..
//#define BALL_LOGTHROTTLE_STREAM(SEVERITY,                                  \@
//                                MAX_SIMULTANEOUS_MESSAGES,                 \@
//                                NANOSECONDS_PER_MESSAGE)                   \@
//    BALL_LOGTHROTTLE_STREAM_IMP((SEVERITY),                                \@
//                                (MAX_SIMULTANEOUS_MESSAGES),               \@
//                                (NANOSECONDS_PER_MESSAGE))                 \@
//                                                    BALL_LOG_OUTPUT_STREAM
//..
// 'BALL_LOGTHROTTLE_STREAM_IMP' is equivalent to
// 'BALL_LOGTHROTTLE_STREAM_CONST_IMP' except that the 'SEVERITY' argument does
// not have to be a compile-time constant and many be a run-time expression.
//
// If you want to access the stream within the controlled block or statement,
// use 'BALL_LOG_OUTPUT_STREAM' as defined by 'ball_log.h'.  There is no
// 'BALL_LOGTHROTTLE_OUTPUT_STREAM'.

                        // ----------------------------
                        // BALL_LOGTHROTTLEVA_CONST_IMP
                        // ----------------------------

// The 'printf'-style const throttled logging macro is reproduced here:
//..
//#define BALL_LOGTHROTTLEVA_CONST_IMP(SEVERITY,                             \@
//                                     MAX_SIMULTANEOUS_MESSAGES,            \@
//                                     NANOSECONDS_PER_MESSAGE,              \@
//                                     ...)                                  \@
//do {                                                                       \@
//    static BloombergLP::bdlmt::Throttle ball_logthrottle_tHrOtTlE =        \@
//                          BDLMT_THROTTLE_INIT((MAX_SIMULTANEOUS_MESSAGES), \@
//                                              (NANOSECONDS_PER_MESSAGE));  \@
//    const BloombergLP::ball::CategoryHolder                                \@
//                                         *ball_logthrottle_cAtEgOrYhOlDeR  \@
//          = BloombergLP::ball::Log::categoryHolderIfEnabled<(SEVERITY)>(   \@
//                     ball_log_getCategoryHolder(BALL_LOG_CATEGORYHOLDER)); \@
//    if (ball_logthrottle_cAtEgOrYhOlDeR &&                                 \@
//                         ball_logthrottle_tHrOtTlE.requestPermission()) {  \@
//        BloombergLP::ball::Log_Formatter ball_log_fOrMaTtEr(               \@
//                              ball_logthrottle_cAtEgOrYhOlDeR->category(), \@
//                              __FILE__,                                    \@
//                              __LINE__,                                    \@
//                              (SEVERITY));                                 \@
//        BloombergLP::ball::Log::format(                                    \@
//                                    ball_log_fOrMaTtEr.messageBuffer(),    \@
//                                    ball_log_fOrMaTtEr.messageBufferLen(), \@
//                                    __VA_ARGS__);                          \@
//    }                                                                      \@
//} while(0)
//..
// Which is used, for example, by
//..
//#define BALL_LOGTHROTTLEVA_TRACE(MAX_SIMULTANEOUS_MESSAGES,                \@
//                                 NANOSECONDS_PER_MESSAGE,                  \@
//                                 ...)                                      \@
//    BALL_LOGTHROTTLEVA_CONST_IMP(BloombergLP::ball::Severity::e_TRACE,     \@
//                                 (MAX_SIMULTANEOUS_MESSAGES),              \@
//                                 (NANOSECONDS_PER_MESSAGE),                \@
//                                 __VA_ARGS__)
//..
// The specified 'SEVERITY' is from the enum 'ball::Severity::Level'.  This
// macro creates a static 'bdlmt::Throttle' object configured with the
// specified 'MAX_SIMULTANEOUS_MESSAGES' and 'NANOSECONDS_PER_MESSAGE'.  If
// 'SEVERITY' is less than (lower severity values are more severe) the category
// threshold and the time debt (see "Throttling Concepts" in component doc) of
// the throttle is not excessive, log a message with the 'printf'-style format
// string msg and arguments 'arg1, ... argN'.  Note that this macro requires
// that the first three arguments to be compile-time constants, noner of which
// may contain any floating-point subexpressions, but there is no such
// requirement on any of the other arguments.

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
