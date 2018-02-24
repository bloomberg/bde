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
//  int pid = bdls::ProcessUtil::getProcessId();
//..
// Get the current process name:
//..
//  bsl::string processName;
//  bdls::ProcessUtil::getProcessName(&processName);
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
    static int getExecutablePath(bsl::string *result);
        // Return a path with which the executable can be accessed.  Return 0
        // if '*result' is set to a path which does open any file, which may
        // be the case if the path is relative and 'chdir' has been called
        // since process start up.  Note that the highest priority in this
        // function is to return a path which, when opened, will access the
        // executable, which may not resemble the 'argv[0]' passed to 'main'.

    static int getProcessId();
        // Return the system specific process identifier for the currently
        // running process.

    static int getProcessName(bsl::string *result);
        // Load the system specific process name for the currently running
        // process into the specified 'result'.  The process name may be in
        // any language on all supported platforms.  To provide that support,
        // 'result' will be encoded as UTF-8, but it might not be normalized.
        // Return 0 on success, and a non-zero value otherwise.  Note that the
        // behavior varies with platform -- the implementation attempts to
        // return a pathname through which the executable can be accessed,
        // which may be completely unlike the value of 'argv[0]' specified to
        // 'main', especially if 'argv[0]' was specified as a relative path.
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
