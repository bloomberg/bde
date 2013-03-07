// bdesu_osutil.cpp                                                   -*-C++-*-
#include <bdesu_osutil.h>

#include <bdesu_processutil.h>

#include <bsl_cstring.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesu_osutil_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace bdesu {

                        // -------------
                        // struct OsUtil
                        // -------------

// CLASS METHODS
#ifdef BSLS_PLATFORM__OS_WINDOWS
#include <windows.h>
#include <process.h>
#include <cstring>

#define snprintf _snprintf

int OsUtil::getOsInfo(bsl::string *osName_p,
                            bsl::string *osVersion_p,
                            bsl::string *osPatch_p)
{
    BSLS_ASSERT(osName_p);
    BSLS_ASSERT(osVersion_p);
    BSLS_ASSERT(osPatch_p);

    bsl::string& osName    = *osName_p;
    bsl::string& osVersion = *osVersion_p;
    bsl::string& osPatch   = *osPatch_p;

    osName = "Windows";

    OSVERSIONINFOEX osvi;

    memset(&osvi, 0, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (!GetVersionEx((OSVERSIONINFO *)&osvi)) {
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        if (!GetVersionEx((OSVERSIONINFO *)&osvi)) {
            return -1;
        }
    }

    // Os version
    char tmpBuf[1024];
    snprintf(tmpBuf,
             sizeof(tmpBuf),
             "%d.%d",
             osvi.dwMajorVersion,
             osvi.dwMinorVersion);
    osVersion = tmpBuf;

    // Service pack number
    osPatch = osvi.szCSDVersion;
    return 0;
}

#else

#include <unistd.h>
#include <libgen.h>
#include <sys/utsname.h>

int OsUtil::getOsInfo(bsl::string *osName_p,
                            bsl::string *osVersion_p,
                            bsl::string *osPatch_p)
{
    BSLS_ASSERT(osName_p);
    BSLS_ASSERT(osVersion_p);
    BSLS_ASSERT(osPatch_p);

    struct utsname unameInfo;
    if (-1 == uname(&unameInfo)) {
        return -1;
    }
    *osName_p = unameInfo.sysname;
    *osVersion_p = unameInfo.release;
    *osPatch_p = unameInfo.version;
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
