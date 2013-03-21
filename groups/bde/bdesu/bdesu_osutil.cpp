// bdesu_osutil.cpp                                                   -*-C++-*-
#include <bdesu_osutil.h>

#include <bdesu_processutil.h>

#include <bsl_cstring.h>
#include <bsl_sstream.h>

#include <bsls_platform.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesu_osutil_cpp, "$Id$ $CSID$")

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <process.h>
#include <cstring>
#else
#include <unistd.h>
#include <sys/utsname.h>
#endif


namespace BloombergLP {
namespace bdesu {

                        // -------------
                        // struct OsUtil
                        // -------------

// CLASS METHODS
#ifdef BSLS_PLATFORM_OS_WINDOWS

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

#else

int OsUtil::getOsInfo(bsl::string *osName,
                      bsl::string *osVersion,
                      bsl::string *osPatch)
{
    BSLS_ASSERT(osName);
    BSLS_ASSERT(osVersion);
    BSLS_ASSERT(osPatch);

    utsname unameInfo;
    if (-1 == uname(&unameInfo)) {
        return -1;
    }
    *osName = unameInfo.sysname;
    *osVersion = unameInfo.release;
    *osPatch = unameInfo.version;
    return 0;
}

#endif
}  // close package namespace
}  // close enterprise namespace

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2013
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
