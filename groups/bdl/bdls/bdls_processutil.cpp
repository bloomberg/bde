// bdls_processutil.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_processutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdls_processutil_cpp,"$Id$ $CSID$")

#include <bdlsb_memoutstreambuf.h>
#include <bdls_fdstreambuf.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_iostream.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_SOLARIS)
#include <procfs.h>
#include <fcntl.h>
#elif defined(BSLS_PLATFORM_OS_AIX)
#include <sys/procfs.h>
#include <fcntl.h>
#elif defined(BSLS_PLATFORM_OS_LINUX) || defined(BSLS_PLATFORM_OS_CYGWIN)
#include <fcntl.h>
#elif defined(BSLS_PLATFORM_OS_HPUX)
#include <sys/pstat.h>
#elif defined(BSLS_PLATFORM_OS_DARWIN)
#include <libproc.h>
#endif

namespace BloombergLP {

namespace bdls {
                             // ------------------
                             // struct ProcessUtil
                             // ------------------

// CLASS METHODS
int ProcessUtil::getProcessId() {
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return static_cast<int>(GetCurrentProcessId());
#else
    return static_cast<int>(getpid());
#endif
}

int ProcessUtil::getProcessName(bsl::string *result)
{
    BSLS_ASSERT(result);

    result->clear();

#if defined BSLS_PLATFORM_OS_WINDOWS
    int  rc = 0;

    result->resize(MAX_PATH);
    DWORD length = GetModuleFileName(0, &(*result->begin()), result->size());
    if (length > 0) {
        result->resize(length);
    }

    return length <= 0;
#else
# if defined BSLS_PLATFORM_OS_HPUX
    result->resize(256);
    int rc = pstat_getcommandline(&(*result->begin()),
                                  result->size(), 1,
                                  getpid());
    if (rc < 0)
    {
        return -1;
    }

    bsl::string::size_type pos = result->find_first_of(' ');
    if (bsl::string::npos != pos) {
        result->resize(pos);
    }
# elif defined BSLS_PLATFORM_OS_LINUX || defined BSLS_PLATFORM_OS_CYGWIN
    enum { NUM_ELEMENTS = 14 + 16 };  // "/proc/<pid>/cmdline"

    bdlsb::MemOutStreamBuf osb(NUM_ELEMENTS);
    bsl::ostream          os(&osb);
    os << "/proc/" << getpid() << "/cmdline" << bsl::ends;
    const char *procfs = osb.data();

    int fd = open(procfs, O_RDONLY);
    if (fd == -1) {
        return -1;                                                    // RETURN
    }

    FdStreamBuf isb(fd, true, true, true);
    bsl::istream      is(&isb);
    is >> *result;

    bsl::string::size_type pos = result->find_first_of('\0');
    if (bsl::string::npos != pos) {
        result->resize(pos);
    }
# elif defined BSLS_PLATFORM_OS_DARWIN
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath (getpid(), pathbuf, sizeof(pathbuf)) <= 0) {
        return -1;
    }
    result->assign(pathbuf);
# else
#  if defined BSLS_PLATFORM_OS_AIX
    enum { NUM_ELEMENTS = 14 + 16 };  // "/proc/<pid>/psinfo"

    bdlsb::MemOutStreamBuf osb(NUM_ELEMENTS);
    bsl::ostream          os(&osb);
    os << "/proc/" << getpid() << "/psinfo" << bsl::ends;
    const char *procfs = osb.data();
#  else
    const char *procfs = "/proc/self/psinfo";
#  endif
    int fd = open(procfs, O_RDONLY);
    if (fd == -1) {
        return -1;
    }

    psinfo_t psinfo;
    bool readFailed = (sizeof psinfo != read(fd, &psinfo, sizeof psinfo));

    int rc = close(fd);
    if (readFailed || 0 != rc) {
        return -1;
    }

    result->assign(psinfo.pr_psargs);

    bsl::string::size_type pos = result->find_first_of(' ');
    if (bsl::string::npos != pos) {
        result->resize(pos);
    }
# endif
#endif
    return result->empty();
}
}  // close package namespace

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
