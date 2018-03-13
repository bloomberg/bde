// bdls_processutil.h                                                 -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLS_PROCESSUTIL
#define INCLUDED_BDLS_PROCESSUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")


//@PURPOSE: Provide basic platform-independent utilities related to processes.
//
//@CLASSES:
//  bdls::ProcessUtil: portable utility methods related to processes
//
//@AUTHOR: Bruce Szablak (bszablak)
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component, 'bdls::ProcessUtil', defines a
// platform-independent interface for processes.  Currently, it provides a
// utility to get the current process ID, and a utility to get the name of the
// process executable.
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
//  assert(0 == rc);
//  assert(!processName.empty());
//..
// All calls to 'getProcessName' will yield the same value:
//..
//  bsl::string processNameB;
//  rc = bdls::ProcessUtil::getProcessName(&processNameB);
//  assert(0 == rc);
//  assert(processNameB == processName);
//..

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif

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
        // failure, '*result' will not be modified.  Note that the purpose of
        // this method is to return a path that, when opened, will reliably
        // access this process's executable.  Also note that the value of
        // 'argv[0]' passed to 'main' may be invalid if it is a relative path
        // and the working directory has been changed since the process began,
        // so absolute paths are preferred, even if they bear no resemblance to
        // the command line (e.g. some Unix platforms provide links to the
        // executable under the "/proc" virtual file space).
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
