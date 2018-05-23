// bdls_processutil.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLS_PROCESSUTIL
#define INCLUDED_BDLS_PROCESSUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide basic platform-independent utilities related to processes.
//
//@CLASSES:
//  bdls::ProcessUtil: portable utility methods related to processes
//
//@AUTHOR: Bruce Szablak (bszablak), Bill Chapman (bchapman2)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component, 'bdls::ProcessUtil', defines a
// platform-independent interface for processes.  Currently, it provides a
// utility to get the current process ID, a utility to get the name of the
// process, and a utility to get a filename through which the executable can be
// accessed.
//
///'getProcessName' vs 'getPathToExecutable'
///-----------------------------------------
// The 'getProcessName' function is intended to yield a process name that will
// be meaningful to the programmer, recognizably related to the value of
// 'argv[0]' passed to the application, and usually a valid path to the
// executable file.  However, in some cases, especially when 'argv[0]' was a
// relative path and the working directory has changed since 'main' was called,
// this will not be a usable path for accessing the executable file, and
// 'getPathToExecutable' should be used.  Note that the return value of
// 'getPathToExecutable' may be completely unrelated to the value of 'argv[0]'
// passed to main, and the value returned by that function may vary when called
// multiple times in the same program.  'getPathToExecutable' will not succeed
// unless it returns a valid path to the executable file.
//
///Usage
///-----
// Get the current process ID:
//..
//  const int pid = bdls::ProcessUtil::getProcessId();
//..
// All calls to 'getProcessId' will yield the same value:
//..
//  assert(bdls::ProcessUtil::getProcessId() == pid);
//..
// Get the current process name:
//..
//  bsl::string processName;
//  int rc = bdls::ProcessUtil::getProcessName(&processName);
//  if (0 != rc) {
//      processName = "unknown";
//  }
//..
// All calls to 'getProcessName' will yield the same value.  Note that if
// the call does not succeed, 'processNameB' will not be modified.
//..
//  bsl::string processNameB("unknown");
//  (void) bdls::ProcessUtil::getProcessName(&processNameB);
//
//  assert(processNameB == processName);
//..

#include <bdlscm_version.h>

#include <bsl_string.h>

namespace BloombergLP {
namespace bdls {

                             // ==================
                             // struct ProcessUtil
                             // ==================

struct ProcessUtil {
    // This 'struct' contains utility methods for platform-independent process
    // operations.

    // CLASS METHODS
    static int getProcessId();
        // Return the system specific process identifier for the currently
        // running process.

    static int getProcessName(bsl::string *result);
        // Load the system specific process name for the currently running
        // process into the specified '*result'.  Return 0 on success, and a
        // non-zero value otherwise.  The language in which '*result' is
        // provided is unspecified; '*result' will be encoded as UTF-8, but
        // might not be normalized.  On failure, '*result' will be unmodified.
        // Note that the primary purpose of this method is to provide an
        // identifier for the current process, and '*result' may not be a valid
        // path to the executable; to access the actual task file for the
        // process use 'getPathToExecutable' below.

    static int getPathToExecutable(bsl::string *result);
        // Set '*result' to a path with which the executable can be accessed
        // (which may bear no relation to the command line used to begin this
        // process).  Return 0 on success, and a non-zero value otherwise.  On
        // failure, '*result' will not be modified.  Note that the returned
        // value of '*result' may not correspond to the value of 'argv[0]'
        // passed to 'main'.  Some systems provide more reliable alternatives,
        // such as through the "/proc" file system.
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
