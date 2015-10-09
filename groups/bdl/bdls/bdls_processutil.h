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
//@SEE_ALSO:
//
//@DESCRIPTION: This component, 'bdls::ProcessUtil', defines a
// platform-independent interface for processes.  Currently, it provides (only)
// a utility to get the current process ID.
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
    static int getProcessId();
        // Return the system specific process identifier for the currently
        // running process.

    static int getProcessName(bsl::string *result);
        // Load the system specific process name for the currently running
        // process into the specified 'result'.  Return 0 on success, and a
        // non-zero value otherwise.  Note that on many systems, this is the
        // fully qualified path name of the current executable.
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
