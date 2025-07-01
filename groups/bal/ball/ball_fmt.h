// ball_fmt.h                                                         -*-C++-*-
#ifndef INCLUDED_BALL_FMT
#define INCLUDED_BALL_FMT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide macros to facilitate `bsl::format` logging.
//
//@MACROS:
//  BALL_FMT: format a log record within a `*_BLOCK`
//  BALL_FMT_TRACE: format a log record with the `e_TRACE` level
//  BALL_FMT_DEBUG: format a log record with the `e_DEBUG` level
//  BALL_FMT_INFO: format a log record with the `e_INFO` level
//  BALL_FMT_WARN: format a log record with the `e_WARN` level
//  BALL_FMT_ERROR: format a log record with the `e_ERROR` level
//  BALL_FMT_FATAL: format a log record with the `e_FATAL` level
//
//@SEE_ALSO: ball_log, bslfmt_format, bslfmt_streamed
//
//@DESCRIPTION: This component provides preprocessor macros that facilitate
// logging using standard `format` format strings.  This component provides
// additional macros that build on those defined in `ball_log`, and this
// documentation assumes readers will be familiar with the terminology
// established by that component, such as log category, log record, log level,
// etc.
//
///Thread Safety
///-------------
// All macros defined in this component are thread-safe, and can be invoked
// concurrently by multiple threads.
//
// Additionally, each use of a logging macro will create a distinct log record,
// and `ball::Observer` implementations (like those in `ball`) generally
// guarantee that output for different log records are not interleaved.
//
///Macro Reference
///---------------
// This section documents the preprocessor macros defined in this component.
//
///Macros for Logging Records
/// - - - - - - - - - - - - -
// The macros defined in this subsection are the ones that are actually used to
// produce log records.  A use of any of the logging macros require a logging
// category be established in scope -- e.g., using `BALL_LOG_SET_CATEGORY`.
// For more information of setting the log category for a particular scope,
// see `ball_log`.  Note that the formatted string that is generated for the
// message attribute of each log record includes the category that is in scope
// and the filename as established by the standard `__FILE__` macro.
//
// The code within any logging statement/code block must not produce any side
// effects because it may or may not be executed based on run-time
// configuration of the `ball` logging subsystem:
//```
// BALL_FMT_INFO << ++i;    // (!) May or may not be incremented
//```
// The following `BALL_FMT_*` macros, are the simplest mechanisms to format
// (using a standard `format` specification) a single message to a log:
//```
// BALL_FMT_TRACE(format-string-literal, ARG1, ARG2, ...);
// BALL_FMT_DEBUG(format-string-literal, ARG1, ARG2, ...);
// BALL_FMT_INFO( format-string-literal, ARG1, ARG2, ...);
// BALL_FMT_WARN( format-string-literal, ARG1, ARG2, ...);
// BALL_FMT_ERROR(format-string-literal, ARG1, ARG2, ...);
// BALL_FMT_FATAL(format-string-literal, ARG1, ARG2, ...);
//```
// where `ARG1`, `ARG2`, ... represents any sequence of values for which a
// `bsl::format` formatter is defined.  The resulting formatted message string
// is logged with the severity indicated by the name of the macro (e.g.,
// `BALL_FMT_TRACE` logs with severity `ball::Severity::e_TRACE`).
//
///Macros for Formatted Logging Inside Code Blocks
///- - - - - - - - - - - - - - - - - - - - - - - -
// `ball_log` provides several kinds of macros to create potentially executed
// code blocks from which more complicated logging can be performed.  As a
// reminder the most commonly used such block macros are:
//```
// BALL_LOG_TRACE_BLOCK { ... }
// BALL_LOG_DEBUG_BLOCK { ... }
// BALL_LOG_INFO_BLOCK  { ... }
// BALL_LOG_WARN_BLOCK  { ... }
// BALL_LOG_ERROR_BLOCK { ... }
// BALL_LOG_FATAL_BLOCK { ... }
//```
// Please see `ball_log` for the other such block macros.
//
// Within logging code blocks the special macro, `BALL_FMT` provides
// standard `format`-style logging into the log record being built there.
//```
// BALL_FMT(format-string-literal, X, Y, ...)
//```
//
///Usage
///-----
// The following code fragments illustrate the standard pattern of macro usage.
//
///Example 1: A Basic Logging Example
/// - - - - - - - - - - - - - - - - -
// The following trivial example shows how to use the logging macros to log
// messages at various levels of severity.
//
// First, we initialize the log category within the context of this function.
// The logging macros such as `BALL_FMT_ERROR` will not compile unless a
// category has been specified in the current lexical scope:
//```
// BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");
//```
// Then, we record messages at various levels of severity.  These messages will
// be conditionally written to the log depending on the current logging
// threshold of the category (configured using the `ball::LoggerManager`
// singleton):
//```
// BALL_FMT_FATAL("Write this message to the log if the log threshold "
//                "is above 'ball::Severity::e_FATAL' (i.e., {}).", 32);
//
// BALL_FMT_TRACE("Write this message to the log if the log threshold "
//                "is above 'ball::Severity::e_TRACE' (i.e., {}).", 192);
//```
// Next, we demonstrate how to use proprietary code within logging macros.
// Suppose you want to add the content of a vector to the log trace:
//```
// bsl::vector<int> myVector(4, 328);
// BALL_LOG_TRACE_BLOCK {
//     BALL_FMT("myVector = [ ");
//     unsigned int position = 0;
//     for (bsl::vector<int>::const_iterator it  = myVector.begin(),
//                                           end = myVector.end();
//         it != end;
//         ++it, ++position) {
//         BALL_FMT("{}:{} ", position, *it);
//     }
//     BALL_FMT("]");
// }
//```
// Note that the code block will be conditionally executed depending on the
// current logging threshold of the category.  The code within the block must
// not produce any side effects, because its execution depends on the current
// logging configuration.  The special macro `BALL_FMT` provides the means to
// write to the log record from within the block.
//
///Example 2: Logging Types with `ostream` insert `operator<<`
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// The following example shows how to use the `bslfmt::streamed` facility with
// types that have no support for `bsl::format`ing, but have support for
// writing to an `ostream` to facilitate formatted logging.
//
// Suppose we have a type that (we do not own, and) is some sort of identifier
// that is capable of "printing" itself to an `ostream`:
//```
// namespace abc {
// class Identifier {
//   private:
//     // DATA
//     unsigned d_value;
//
//   public:
//     // CREATORS
//     Identifier(unsigned value) : d_value(value) {}
//
//     // ACCESSORS
//     unsigned value() const { return d_value; }
// };
//
// bsl::ostream& operator<<(bsl::ostream& os, const Identifier& obj)
// {
//     return os << obj.value();
// }
//
// }  // close namespace abc
//```
// First, we initialize the log category within the context of this function.
// The logging macros such as `BALL_FMT_ERROR` will not compile unless a
// category has been specified in the current lexical scope:
//```
// BALL_LOG_SET_CATEGORY("EXAMPLE.CATEGORY");
//```
// Then, we record a message containing identifiers using `bslfmt::streamed`:
//```
// const abc::Identifier id(12345);
// BALL_FMT_FATAL("Item {:>010} does not exist.", bslfmt::streamed(id));
// // Logs: `Item 0000012345 does not exist.`
//```
// Note that the wrapper created by `bslfmt::streamed` uses the `ostream`
// insert `operator<<` of `abc::Identifier` to get the characters to print and
// uses the syntax of string formatting for the format specification.

#include <balscm_version.h>

#include <ball_log.h>

#include <bsl_format.h>
#include <bsl_iterator.h>

                         // =========================
                         // Logging Macro Definitions
                         // =========================

#define BALL_FMT(...)                                                         \
    bsl::format_to(                                                           \
        bsl::ostreambuf_iterator<char>(                                       \
            &BALL_LOG_RECORD->fixedFields().messageStreamBuf()),              \
        __VA_ARGS__)

#define BALL_FMT_TRACE(...)                                                   \
    BALL_LOG_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_TRACE)           \
    BALL_FMT(__VA_ARGS__)

#define BALL_FMT_DEBUG(...)                                                   \
    BALL_LOG_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_DEBUG)           \
    BALL_FMT(__VA_ARGS__)

#define BALL_FMT_INFO(...)                                                    \
    BALL_LOG_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_INFO)            \
    BALL_FMT(__VA_ARGS__)

#define BALL_FMT_WARN(...)                                                    \
    BALL_LOG_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_WARN)            \
    BALL_FMT(__VA_ARGS__)

#define BALL_FMT_ERROR(...)                                                   \
    BALL_LOG_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_ERROR)           \
    BALL_FMT(__VA_ARGS__)

#define BALL_FMT_FATAL(...)                                                   \
    BALL_LOG_STREAM_CONST_IMP(BloombergLP::ball::Severity::e_FATAL)           \
    BALL_FMT(__VA_ARGS__)

#endif  // INCLUDED_BALL_FMT

// ----------------------------------------------------------------------------
// Copyright 2025 Bloomberg Finance L.P.
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
