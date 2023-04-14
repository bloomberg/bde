// ball_logfilecleanerutil.h                                          -*-C++-*-
#ifndef INCLUDED_BALL_LOGFILECLEANERUTIL
#define INCLUDED_BALL_LOGFILECLEANERUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility class for removing log files.
//
//@CLASSES:
//  ball::LogFileCleanerUtil: utility class for removing log files
//
//@SEE_ALSO: ball_fileobserver2, balb_filecleanerconfiguration
//
//@DESCRIPTION: This component defines a 'struct', 'ball::LogFileCleanerUtil',
// that provides utility functions for converting log file patterns used by
// 'ball' file observers into filesystem patterns and installing a custom
// rotation callback into file observers to perform log file cleanup.
//
///Log Filename Pattern
///--------------------
// The 'ball' file observers allow the use of '%'-escape sequences to specify
// log filenames.  The recognized sequences are as follows:
//..
//  %Y - current year (four digits with leading zeros)
//  %M - current month (two digits with leading zeros)
//  %D - current day (two digits with leading zeros)
//  %h - current hour (two digits with leading zeros)
//  %m - current minute (two digits with leading zeros)
//  %s - current second (two digits with leading zeros)
//  %T - current datetime, equivalent to "%Y%M%D_%h%m%s"
//  %p - process Id
//..
//
///Log Filename Pattern Conversion Rules
///-------------------------------------
// The 'logPatternToFilePattern' conversion function does the following:
//
// * Every recognized '%'-escape sequence is converted to '*'.
//
// * Successive '%'-escape sequences without any separator between them are
//   collapsed into a single '*'.
//
// * "%%" is converted into a single '%'.
//
// * All unrecognized '%'-escape sequences and characters are passed to
//   output as-is.
//
// * A single '*' is appended to the converted pattern when it does not
//   terminate with '*'.  (This is necessary for capturing rotated log files.)
//
//..
//  -------------------+---------------
//   Log File Pattern  | File Pattern
//  -------------------+---------------
//   "a.log"           | "a.log*"
//   "a.log.%T"        | "a.log.*"
//   "a.log.%Y"        | "a.log.*"
//   "a.log.%Y%M%D"    | "a.log.*"
//   "a.%T.log"        | "a.*.log*"
//  -------------------+---------------
//..
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following snippets of code illustrate the basic usage of
// 'ball::LogFileCleanerUtil'.
//
// Suppose that the application was set up to do its logging using one of the
// 'ball' file observers (see 'ball_fileobserver2') with the following log
// pattern:
//..
//  const char *appLogPattern = "/var/log/myApp/log%T";
//..
// First, we need to convert the log filename pattern to the pattern that can
// be used for filename matching on the filesystem:
//..
//  bsl::string fileNamePattern;
//  ball::LogFileCleanerUtil::logPatternToFilePattern(&fileNamePattern,
//                                                    appLogPattern);
//..
// Finally, we test the resulting file pattern:
//..
//  assert("/var/log/myApp/log*" == fileNamePattern);
//..
///Example 2: Cleaning Log Files On File Rotation
/// - - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how the application can implement
// automatic log file cleanup from the observer's file rotation callback.
//
// Suppose that the application was set up to do its logging using one of the
// file observers (see 'ball_fileobserver2') with the following log pattern:
//..
//  const char *appLogPattern = "/var/log/myApp/log%T";
//..
// First, we need to convert the log filename pattern to the pattern that can
// be used for filename matching on the filesystem:
//..
//  bsl::string fileNamePattern;
//  ball::LogFileCleanerUtil::logPatternToFilePattern(&fileNamePattern,
//                                                    appLogPattern);
//..
// Then, we create a configuration for the file cleaner utility.  The sample
// configuration below instructs the file cleaner to remove all log files that
// match the specified file pattern and are older than a week, but to keep at
// least 4 most recent log files:
//..
//  balb::FileCleanerConfiguration config(
//               fileNamePattern.c_str(),
//               bsls::TimeInterval(7 * bdlt::TimeUnitRatio::k_SECONDS_PER_DAY),
//               4);
//..
// Next, we create a file observer and enable file logging:
//..
//  ball::FileObserver2 observer;
//  observer.enableFileLogging(appLogPattern);
//..
// Finally, we use the utility function to install the file rotation callback
// that will invoke file cleanup with the specified configuration:
//..
//  ball::LogFileCleanerUtil::enableLogFileCleanup(&observer, config);
//..
// Note that the file cleanup will be performed immediately and on every log
// file rotation performed by the file observer.  Also note that this method
// overrides the file rotation callback currently installed in the file
// observer.

#include <balscm_version.h>

#include <balb_filecleanerconfiguration.h>
#include <balb_filecleanerutil.h>

#include <bdlf_bind.h>
#include <bdlf_placeholder.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace ball {

                           // =========================
                           // struct LogFileCleanerUtil
                           // =========================

struct LogFileCleanerUtil {
    // This utility class provides functions for converting log file patterns
    // and for cleaning up log files based on a configuration.
  private:
    // PRIVATE CLASS METHODS
                        // ** default callback function **
    static
    void logFileCleanupOnRotationDefault(
                                 int,
                                 const bsl::string&,
                                 const balb::FileCleanerConfiguration& config);
        // Immediately call 'balb::FileCleanerUtil::removeFiles' with the
        // specified 'config'.

  public:
    // CLASS METHODS
    template <class t_OBSERVER>
    static
    void enableLogFileCleanup(
                               t_OBSERVER                            *observer,
                               const balb::FileCleanerConfiguration&  config);
        // Immediately call 'balb::FileCleanerUtil::removeFiles' with the
        // specified 'config' and then install an
        // 't_OBSERVER::OnFileRotationCallback' function into the specified
        // 'observer' that invokes 'removeFiles' synchronously on every log
        // file rotation.  The (template parameter) 't_OBSERVER' type must
        // provide concrete implementation of the 'ball::Observer' protocol,
        // have a 'setOnFileRotationCallback' method (see 'ball_fileobserver',
        // 'ball_fileobserver2', and 'ball_asyncfileobserver'), and define an
        // 'OnFileRotationCallback' type alias.  This method overrides the file
        // rotation callback currently installed in the observer (if any).

    static
    void logPatternToFilePattern(bsl::string             *filePattern,
                                 const bsl::string_view&  logPattern);
        // Substitute all occurrences of valid '%'-escape sequences in the
        // specified 'logPattern' with '*' and load the specified 'filePattern'
        // with the resulting string.  This utility function converts the log
        // patterns used by various file observers and supplied to their
        // 'enableFileLogging' method (see 'ball_fileobserver',
        // 'ball_fileobserver2', and 'ball_asyncfileobserver') to the file
        // pattern used by various utility functions to find the related
        // file(s) on the filesystem (see 'balb_filecleanerutil' and
        // 'balb_filecleanerconfiguration').
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                           // -------------------------
                           // struct LogFileCleanerUtil
                           // -------------------------

// CLASS METHOD
template <class t_OBSERVER>
inline
void LogFileCleanerUtil::enableLogFileCleanup(
                               t_OBSERVER                            *observer,
                               const balb::FileCleanerConfiguration&  config)
{
    balb::FileCleanerUtil::removeFiles(config);

    typename t_OBSERVER::OnFileRotationCallback  rotationCallback =
        bdlf::BindUtil::bind(&logFileCleanupOnRotationDefault,
                             bdlf::PlaceHolders::_1,
                             bdlf::PlaceHolders::_2,
                             config);

    observer->setOnFileRotationCallback(rotationCallback);
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
