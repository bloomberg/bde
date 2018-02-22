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

#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_iostream.h>

#if !defined BSLS_PLATFORM_OS_WINDOWS
# include <unistd.h>    // getpid

# if defined BSLS_PLATFORM_OS_AIX
#   include <bslma_allocator.h>
#   include <bslma_deallocatorguard.h>
#   include <bslma_default.h>
#   include <bsl_algorithm.h>
#   include <sys/procfs.h>
#   include <sys/stat.h>
#   include <sys/types.h>
#   include <procinfo.h>
#   include <stdio.h>
# elif defined BSLS_PLATFORM_OS_CYGWIN
#   ifndef   _REENTRANT
#     define _REENTRANT
#   endif

#   include <bdlsb_memoutstreambuf.h>
#   include <bsl_fstream.h>
#   include <fcntl.h>
#   include <procfs.h>
# elif defined BSLS_PLATFORM_OS_DARWIN
#   include <libproc.h>
# elif defined BSLS_PLATFORM_OS_HPUX
#   include <sys/pstat.h>
# elif defined BSLS_PLATFORM_OS_LINUX
#   include <bslma_allocator.h>
#   include <bslma_deallocatorguard.h>
#   include <bslma_default.h>
#   include <bsls_types.h>
#   include <bsl_algorithm.h>
#   include <errno.h>
#   include <sys/stat.h>
#   include <sys/types.h>
# elif defined BSLS_PLATFORM_OS_SOLARIS
#   include <stdlib.h>
#   include <sys/stat.h>
#   include <sys/types.h>
#   include <stdio.h>
# else
#   error Unrecognized Platform
# endif
#else    // if defined BSLS_PLATFORM_OS_WINDOWS
# include <bdlde_charconvertutf16.h>
# include <bdlma_localsequentialallocator.h>
# include <windows.h>
#endif

namespace BloombergLP {
namespace bdls {
                             // ------------------
                             // struct ProcessUtil
                             // ------------------

// CLASS METHODS
int ProcessUtil::getProcessId()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return static_cast<int>(GetCurrentProcessId());
#else
    return static_cast<int>(::getpid());
#endif
}

int ProcessUtil::getProcessName(bsl::string *result)
{
    BSLS_ASSERT(result);

    result->clear();

#if defined BSLS_PLATFORM_OS_AIX
    struct procentry64  procbuffer;
    enum { k_BUF_LEN = 64 * 1024 };
    bslma::Allocator *allocator = bslma::Default::defaultAllocator();

    char *argsbuffer = static_cast<char *>(allocator->allocate(k_BUF_LEN));
    char *end = argsbuffer + k_BUF_LEN;
    bslma::DeallocatorGuard<bslma::Allocator> guard(argsbuffer, allocator);

    procbuffer.pi_pid = ProcessUtil::getProcessId();

    // '::getargs' should fill the beginning of 'argsbuffer' with
    // null-terminated 'argv[0]', which might be a relative path.

    if (0 == ::getargs(&procbuffer, sizeof(procbuffer), argsbuffer, k_BUF_LEN)
                                   && bsl::find(argsbuffer, end, '\0') < end) {
        result->assign(argsbuffer);
    }

    struct stat s;
    if (result->empty() ||
                   ('/' != (*result)[0] && 0 != ::stat(result->c_str(), &s))) {
        // Either 'getargs' failed, or the path is relative and the executable
        // is not there.  We have probably done a 'chdir' since program
        // start-up.  See if we can find the executable from "/proc".

        char fileNameBuf[50];
        snprintf(fileNameBuf,
                 sizeof(fileNameBuf),
                 "/proc/%d/object/a.out",
                 ProcessUtil::getProcessId());
        if (0 == ::stat(fileNameBuf, &s)) {
            result->assign(fileNameBuf);
        }
    }
#elif defined BSLS_PLATFORM_OS_CYGWIN
    enum { NUM_ELEMENTS = 14 + 16 };  // "/proc/<pid>/cmdline"

    bdlsb::MemOutStreamBuf osb(NUM_ELEMENTS);
    bsl::ostream           os(&osb);
    os << "/proc/" << ProcessUtil::getProcessId() << "/cmdline" << bsl::ends;
    const char *procfs = osb.data();

    bsl::ifstream ifs;
    ifs.open(procfs, bsl::ios_base::in | bsl::ios_base::binary);
    if (ifs.fail()) {
        return -1;                                                    // RETURN
    }
    ifs >> *result;

    bsl::string::size_type pos = result->find_first_of('\0');
    if (bsl::string::npos != pos) {
        result->resize(pos);
    }
#elif defined BSLS_PLATFORM_OS_DARWIN
    char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath(ProcessUtil::getProcessId(),
                     pathbuf,
                     sizeof(pathbuf)) <= 0) {
        return -1;
    }
    result->assign(pathbuf);
#elif defined BSLS_PLATFORM_OS_HPUX
    result->resize(1024);
    int rc = pstat_getcommandline(&(*result->begin()),
                                  result->size(),
                                  1,
                                  ProcessUtil::getProcessId());
    if (rc < 0) {
        return -1;
    }

    bsl::string::size_type pos = result->find_first_of(' ');
    if (bsl::string::npos != pos) {
        result->resize(pos);
    }
#elif defined BSLS_PLATFORM_OS_LINUX
    // First, we attempt to get the executable name through the symlink
    // "/proc/self/exe", because, if successful, that will yield a full path
    // with symlinks resolved.

    const char linkName[] = { "/proc/self/exe" };
    struct stat s;
    bsls::Types::Int64 rc = ::lstat(linkName, &s);
    if (0 == rc && S_ISLNK(s.st_mode)) {
        enum { k_BUF_LEN = 64 * 1024 };
        bslma::Allocator *allocator = bslma::Default::defaultAllocator();

        char *linkBuf = static_cast<char *>(allocator->allocate(k_BUF_LEN));
        bslma::DeallocatorGuard<bslma::Allocator> guard(linkBuf, allocator);

        rc = ::readlink(linkName, linkBuf, k_BUF_LEN);
        if (0 < rc && rc < k_BUF_LEN && linkBuf + rc ==
                                     bsl::find(linkBuf, linkBuf + rc + 1, 0)) {
            result->assign(linkBuf);
        }
    }

    if (result->empty()) {
        // Reading the symlink failed, probably because "/proc" is sometimes
        // unavailable.  We fall back on reading '::program_invocation_name',
        // which will just yield 'argv[0]', which may or may not be a relative
        // path, a less optimal result.  Note that this can still cope with the
        // case where the path of the executable contains spaces.

        result->assign(::program_invocation_name);
    }
#elif defined BSLS_PLATFORM_OS_SOLARIS
    // '::getexecname' will return an absolute path with symlinks resolved.

    result->assign(::getexecname());
#elif defined BSLS_PLATFORM_OS_WINDOWS
    // On Windows, 'argv[0]' is always an absolute path, even if it was
    // specified on the cmd line as a relative path.  The following code yields
    // the full path.

    static const size_t k_INITIAL_SIZE = MAX_PATH + 1;
    bdlma::LocalSequentialAllocator<sizeof(wchar_t) * k_INITIAL_SIZE> la;
    bsl::wstring wResult(MAX_PATH, L'\0', &la);
    while (wResult.length() <= 4 * k_INITIAL_SIZE) {
        DWORD length = GetModuleFileNameW(0, &wResult[0], wResult.length());
        if (length == 0) {  // Error
            return 1;                                                 // RETURN
        }
        else if (length < wResult.length()) {  // Success
            wResult.resize(length);
            return bdlde::CharConvertUtf16::utf16ToUtf8(result, wResult);
                                                                      // RETURN
        }
        else { // Not enough space for the process name in 'wResult'
            wResult.resize(wResult.length() * 2); // Make more space
        }
    }

    return -1; // The path name is too long (more than 4 * k_INITIAL_SIZE)
#else
# error    Unrecognized Platform
#endif

    return result->empty();    // 'false' == 0 if success
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
