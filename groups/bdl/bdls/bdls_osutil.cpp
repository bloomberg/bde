// bdls_osutil.cpp                                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_osutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdls_osutil_cpp, "$Id$ $CSID$")

#include <bdls_processutil.h>

#include <bsl_cstring.h>
#include <bsl_sstream.h>

#include <bslmf_assert.h>

#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
# include <bsl_limits.h>

# include "windows.h"
# include "VersionHelpers.h"
#else
# include <unistd.h>
# include <sys/utsname.h>
#endif


namespace BloombergLP {

                            // --------------------
                            // struct bdls::OsUtil
                            // --------------------

// CLASS METHODS
#ifdef BSLS_PLATFORM_OS_WINDOWS

namespace bdls {
int OsUtil::getOsInfo(bsl::string *osName,
                      bsl::string *osVersion,
                      bsl::string *osPatch)
{
    BSLS_ASSERT(osName);
    BSLS_ASSERT(osVersion);
    BSLS_ASSERT(osPatch);

    *osName = "Windows";

    // On Windows, 'WORD' means a 16-bit unsigned int.

    WORD major = 0;
    WORD minor = 0;
    WORD servicePackMajor = 0;

    const WORD maxWord = bsl::numeric_limits<WORD>::max();

    while (IsWindowsVersionOrGreater(major, minor, servicePackMajor)) {
        if (major >= maxWord) {
            return -1;                                                // RETURN
	}
        ++major;
    }
    --major;
    while (IsWindowsVersionOrGreater(major, minor, servicePackMajor)) {
        if (minor >= maxWord) {
            return -1;                                                // RETURN
	}
        ++minor;
    }
    --minor;
    while (IsWindowsVersionOrGreater(major, minor, servicePackMajor)) {
        if (servicePackMajor >= maxWord) {
            return -1;                                                // RETURN
	}
        ++servicePackMajor;
    }
    --servicePackMajor;

    // Os version
    bsl::ostringstream version;
    version << major << '.' << minor;
    *osVersion = version.str();
    version.str("");

    // Service pack number
    if (servicePackMajor) {
        version << "Service Pack " << servicePackMajor << ".0";
    }
    *osPatch = version.str();

    return 0;
}
}  // close package namespace

#elif defined(BSLS_PLATFORM_OS_UNIX)

namespace bdls {
int OsUtil::getOsInfo(bsl::string *osName,
                      bsl::string *osVersion,
                      bsl::string *osPatch)
{
    BSLS_ASSERT(osName);
    BSLS_ASSERT(osVersion);
    BSLS_ASSERT(osPatch);

    struct utsname unameInfo;
    if (-1 == uname(&unameInfo)) {
        return -1;                                                    // RETURN
    }
    *osName = unameInfo.sysname;
    *osVersion = unameInfo.release;
    *osPatch = unameInfo.version;
    return 0;
}
}  // close package namespace

#else

BSLMF_ASSERT("Unsupported operating system", false);

#endif
}  // close enterprise namespace

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
