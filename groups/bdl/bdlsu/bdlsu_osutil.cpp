// bdlsu_osutil.cpp                                                   -*-C++-*-
#include <bdlsu_osutil.h>

#include <bdlsu_processutil.h>

#include <bsl_cstring.h>
#include <bsl_sstream.h>

#include <bsls_platform.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlsu_osutil_cpp, "$Id$ $CSID$")

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <process.h>
#include <cstring>
#else
#include <unistd.h>
#include <sys/utsname.h>
#endif


namespace BloombergLP {

                        // -------------------
                        // struct bdlsu::OsUtil
                        // -------------------

// CLASS METHODS
#ifdef BSLS_PLATFORM_OS_WINDOWS

namespace bdlsu {
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

namespace bdlsu {
int OsUtil::getOsInfo(bsl::string *osName,
                            bsl::string *osVersion,
                            bsl::string *osPatch)
{
    BSLS_ASSERT(osName);
    BSLS_ASSERT(osVersion);
    BSLS_ASSERT(osPatch);

    struct utsname unameInfo;
    if (-1 == uname(&unameInfo)) {
        return -1;
    }
    *osName = unameInfo.sysname;
    *osVersion = unameInfo.release;
    *osPatch = unameInfo.version;
    return 0;
}
}  // close package namespace

#endif
}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
