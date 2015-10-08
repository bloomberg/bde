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

#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <process.h>
#include <cstring>
#else
#include <unistd.h>
#include <sys/utsname.h>
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

    OSVERSIONINFOEX osvi;

    memset(&osvi, 0, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (!GetVersionEx((OSVERSIONINFO *)&osvi)) {
        return -1;
    }

    // Os version
    bsl::ostringstream version;
    version << osvi.dwMajorVersion << '.' << osvi.dwMinorVersion;
    *osVersion = version.str();

    // Service pack number
    *osPatch = osvi.szCSDVersion;
    return 0;
}
}  // close package namespace

#else

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
