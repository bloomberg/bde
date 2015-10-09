// bdls_pipeutil.h                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BDLS_PIPEUTIL
#define INCLUDED_BDLS_PIPEUTIL

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide basic portable named-pipe utilities.
//
//@CLASSES:
//  bdls::PipeUtil: Portable utility methods for naming and accessing pipes
//
//@SEE_ALSO:
//
//@DESCRIPTION: This component, 'bdls::PipeUtil', provides portable utility
// methods for named pipes.

#ifndef INCLUDED_BDLSCM_VERSION
#include <bdlscm_version.h>
#endif

#ifndef INCLUDED_BSL_STRING
#include <bsl_string.h>
#endif


namespace BloombergLP {

namespace bdls {
                              // ===============
                              // struct PipeUtil
                              // ===============
struct PipeUtil {
    // This struct contains utility methods for platform-independent named pipe
    // operations.

    static int makeCanonicalName(bsl::string              *pipeName,
                                 const bslstl::StringRef&  baseName);
        // Load into the specified 'pipeName' the system-dependent canonical
        // pipe name corresponding to the specified 'baseName'.  On Unix
        // systems, attempts to load the environment variable TMPDIR to create
        // the pipe name.  Return 0 on success, and a nonzero value if
        // 'baseName' cannot be part of a pipe name on this system.

    static int send(const bslstl::StringRef& pipeName,
                    const bslstl::StringRef& message);
        // Send the specified 'message' to the pipe with the specified
        // 'pipeName'.  Return 0 on success, and a nonzero value otherwise.

    static bool isOpenForReading(const bslstl::StringRef& pipeName);
        // Return 'true' if the pipe with the specified 'pipeName' exists and
        // is currently open for reading by some process, and 'false'
        // otherwise.
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
