// balb_filecleanerutil.h                                             -*-C++-*-
#ifndef INCLUDED_BALB_FILECLEANERUTIL
#define INCLUDED_BALB_FILECLEANERUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a utility class for configuration-based file removal.
//
//@CLASSES:
//  balb::FileCleanerUtil: utility class for removing selected files
//
//@SEE_ALSO: balb_filecleanerconfiguration
//
//@AUTHOR: Oleg Subbotin
//
//@DESCRIPTION: This component defines a 'struct', 'balb::FileCleanerUtil',
// that provides a utility function for removing files based on a
// configuration.
//
///General Features and Behavior
///-----------------------------
// The 'removeFiles' function implements generic filename pattern matching and
// removal logic.  The user is responsible for calling this function with
// parameters that do not cause any interference with the application logic.
// Specifically, the user must pay attention to the supplied pattern to avoid
// removal of important application data files.  See
// 'balb_filecleanerconfiguration' for further information.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following snippets of code illustrate the basic usage of
// 'balb::FileCleanerUtil'.
//
// Let's assume that the application was set up to log to files having the
// following pattern:
//..
//  const char *appLogFilePattern = "/var/log/myApp/log*";
//..
// We want to clean up all the files older then a week, but leave at least 4
// latest log files.  First, we create a cleanup configuration object that will
// capture our parameters:
//..
//  balb::FileCleanerConfiguration config(
//              appLogFilePattern,
//              bsls::TimeInterval(bdlt::TimeUnitRatio::k_SECONDS_PER_DAY * 7),
//              4);
//..
// Then, we use this configuration to do a file cleanup:
//..
//  balb::FileCleanerUtil::removeFiles(config);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALB_FILECLEANERCONFIGURATION
#include <balb_filecleanerconfiguration.h>
#endif

namespace BloombergLP {
namespace balb {

                           // ======================
                           // struct FileCleanerUtil
                           // ======================

struct FileCleanerUtil {
    // This utility class provides functions relating to file clean up.
  public:
    // CLASS METHODS
    static
    void removeFiles(const FileCleanerConfiguration& config);
        // Remove files based on the criteria given by the specified 'config'.
        // Only those files for which 'bdls::FilesystemUtil::isRegularFile'
        // returns 'true' are considered for removal (symbolic links are not
        // followed).
};

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
