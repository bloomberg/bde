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

#include <bdls_filesystemutil.h>

#include <bslma_allocator.h>
#include <bslma_deallocatorguard.h>
#include <bslma_default.h>
#include <bslmf_assert.h>
#include <bsls_assert.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_iostream.h>

#include <sys/types.h>        // must precede 'stat.h'
#include <sys/stat.h>         // 'stat'

#if defined BSLS_PLATFORM_OS_UNIX
# include <unistd.h>           // 'getpid'

# if defined BSLS_PLATFORM_OS_AIX
#   include <bslmf_assert.h>
#   include <sys/procfs.h>
#   include <procinfo.h>
# elif defined BSLS_PLATFORM_OS_DARWIN
#   include <libproc.h>
# elif defined BSLS_PLATFORM_OS_LINUX
#   include <bslma_deallocatorguard.h>
#   include <bsls_types.h>
#   include <errno.h>            // ::program_invocation_name
# elif defined BSLS_PLATFORM_OS_SOLARIS
#   include <bsl_fstream.h>
# else
#   error Unrecognized Platform
# endif
#else    // if defined BSLS_PLATFORM_OS_WINDOWS
# include <bdlde_charconvertutf16.h>
# include <windows.h>
#endif

namespace {
namespace u {

inline
int getPid()
    // Return the process id.  Having this be separate from 'Obj::getProcessId'
    // allows us to call it inline within the component.
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return static_cast<int>(GetCurrentProcessId());
#else
    return static_cast<int>(::getpid());
#endif
}

inline
bool isExecutable(const char *path)
    // On Unix, return 'true' if the file at the specified 'path' exists, is
    // not a directory, and is executable (or is a symbolic link to such a
    // file) and 'false' otherwise.  On Windows, return 'true' if the file
    // exists and is not a directory.
{
#if defined BSLS_PLATFORM_OS_UNIX
    const int executableBits = S_IXUSR | S_IXGRP | S_IXOTH;

    struct stat s;
    int rc = ::stat(path, &s);
#else    // Windows
    const int executableBits = S_IEXEC;

    struct _stat s;
    int rc = ::_stat(path, &s);
#endif

    return 0 == rc && !(s.st_mode & S_IFDIR) && (s.st_mode & executableBits);
}

inline
bool isExecutable(const bsl::string& path)
{
    return isExecutable(path.c_str());
}

void appendInt(char *buf, int intValue)
    // Append the specified 'intValue', in decimal ascii, to the specified
    // 'buf';
{
    // The purpose of this function is to get around needing to use 'sprintf',
    // which calls 'malloc', to write the pid to a buffer.

    buf = buf + bsl::strlen(buf);

    bool sign = intValue < 0;
    if (sign) {
        *buf++ = '-';
    }

    int denominator = sign ? -1 : +1;
    BSLS_ASSERT(0 <= intValue / denominator);
    while (10 <= intValue / denominator) {
        denominator *= 10;
    }

    for (; 0 != denominator; denominator /= 10) {
        const int digit = intValue /  denominator;
        intValue                   %= denominator;

        BSLS_ASSERT(0 <= digit);
        BSLS_ASSERT(     digit < 10);

        *buf++ = static_cast<char>(digit + '0');
    }

    *buf = 0;
}

}  // close namespace u
}  // close unnamed namespace

namespace BloombergLP {
namespace bdls {

                             // ------------------
                             // struct ProcessUtil
                             // ------------------

// CLASS METHODS
int ProcessUtil::getProcessId()
{
    return u::getPid();
}

int ProcessUtil::getProcessName(bsl::string *result)
{
    BSLS_ASSERT(result);

    // The goal of 'getProcessName' is to return something like 'argv[0]', it
    // would be nice if it were a path to the executable, but failing that,
    // prefer to return something as similar to 'argv[0]' as possible.  If
    // 'argv[0]' is a symlink, prefer to return it unresolved.

#if defined BSLS_PLATFORM_OS_AIX

    enum { k_BUF_LEN = 8 * 1024 };

    bsl::string argsBuf(k_BUF_LEN, '\0');
    char *buf = &argsBuf[0];

    struct procentry64  procBuf;
    procBuf.pi_pid = u::getPid();

    // '::getargs' should fill the beginning of 'argsBuf' with null-terminated
    // 'argv[0]', which might be a relative path.

    if (0 != ::getargs(&procBuf, sizeof(procBuf), buf, k_BUF_LEN - 1)) {
        return -1;                                                    // RETURN
    }

    // 'assign' here, not 'swap' since 'argsBuf' is irreversibly large and
    // probably uses a different allocator than '*result'.

    result->assign(argsBuf.c_str());
    return 0;

#elif defined BSLS_PLATFORM_OS_DARWIN

    bsl::string pidPathBuf(PROC_PIDPATHINFO_MAXSIZE, '\0');

    int numChars = ::proc_pidpath(u::getPid(),
                                  &pidPathBuf[0],
                                  PROC_PIDPATHINFO_MAXSIZE);
    if (numChars <= 0) {
        return -1;                                                    // RETURN
    }
    BSLS_ASSERT(numChars <= PROC_PIDPATHINFO_MAXSIZE);

    result->assign(pidPathBuf.c_str(), numChars);
    return 0;

#elif defined BSLS_PLATFORM_OS_LINUX

    // We read '::program_invocation_name', which will just yield 'argv[0]',
    // which may or may not be a relative path.  Note that this will still
    // correctly handle the case where the path of the executable contains
    // spaces.

    const char *argv0 = ::program_invocation_name;
    if (!argv0) {
        return -1;                                                    // RETURN
    }
    result->assign(argv0);
    return 0;

#elif defined BSLS_PLATFORM_OS_SOLARIS

    //: o If 'argv0' is obtained and is executable, use that.
    //: o Else, if 'execName' is non-empty and executable, use 'execName'.
    //: o Else, If 'execName' is non-empty and 'argv0' is empty, use
    //:   'execName'.
    //: o Else, if 'execName' and 'argv0' are both non-empty and neither is
    //:   executable, use 'argv0' since that will not have symlinks resolved.

    char cmdLineFileName[100];
    bsl::strcpy( cmdLineFileName, "/proc/");
    u::appendInt(cmdLineFileName, u::getPid());
    bsl::strcat( cmdLineFileName, "/cmdline");
    bsl::ifstream cmdLineIstream;
    cmdLineIstream.open(cmdLineFileName,
                        bsl::ios_base::in | bsl::ios_base::binary);

    bsl::string argv0;
    if (cmdLineIstream.is_open()) {
        bsl::getline(cmdLineIstream, argv0, '\0');

        if (!argv0.empty() && u::isExecutable(argv0)) {
            result->assign(argv0);
            return 0;                                                 // RETURN
        }
    }

    // OK, most likely either '/proc' is unavailable, or 'argv0' was a relative
    // path and we've changed the current directory since task startup.

    // '::getexecname' will return 'argv[0]' with symlinks resolved (which
    // might be a full path when 'argv[0]' was relative), or 0 if it fails.  No
    // known limit on length.

    const char *execName = ::getexecname();
    if (execName && *execName &&
                                (argv0.empty() || u::isExecutable(execName))) {
        result->assign(execName);
        return 0;                                                     // RETURN
    }
    if (!argv0.empty()) {
        result->assign(argv0);
        return 0;                                                     // RETURN
    }

    return -1;

#elif defined BSLS_PLATFORM_OS_WINDOWS

    // This code seems to be returning 'argv[0]' except that, on Windows,
    // 'argv[0]' is always an absolute path, even if it was specified on the
    // command line as a relative path.  The following code yields the full
    // path.

    bsl::wstring wResult(64 * 1024, '\0');
    DWORD length = GetModuleFileNameW(0, &wResult[0], wResult.length());
    if (0 < length && length < wResult.length()) {
        wResult.resize(length);

        // Use '*result's allocator for 'utf8Path', since 'utf8Path' will be
        // perfectly sized and we can then swap it into '*result'.

        bsl::string utf8Path(result->allocator().mechanism());
        int rc = bdlde::CharConvertUtf16::utf16ToUtf8(&utf8Path, wResult);
        if (0 != rc) {
            return -1;                                                // RETURN
        }

        result->swap(utf8Path);
        return 0;                                                     // RETURN
    }

    // Error

    return -1;
#else
# error    Unrecognized Platform
#endif
}

int ProcessUtil::getPathToExecutable(bsl::string *result)
{
    BSLS_ASSERT(result);

    int rc;

    // Try 'getProcessName' and see if it gives a path to a file.  This method
    // is prone to yielding relative paths, but if there's an executable file
    // at the yielded path, that is good enough.

    bsl::string processName;
    rc = getProcessName(&processName);
    if (0 == rc && u::isExecutable(processName)) {
        result->assign(processName);
        return 0;                                                     // RETURN
    }

    // Probably 'argv[0]' was a relative path and we've changed the current
    // directory since task startup.  Employ some alternatives to get the
    // executable.

#if  defined BSLS_PLATFORM_OS_LINUX || defined BSLS_PLATFORM_OS_AIX || \
                                               defined BSLS_PLATFORM_OS_SOLARIS
# if defined BSLS_PLATFORM_OS_LINUX
    const char *suffix = "/exe";
# else
    const char *suffix = "/object/a.out";
# endif

    char linkName[100];
    bsl::strcpy( linkName, "/proc/");
    u::appendInt(linkName, u::getPid());
    bsl::strcat( linkName, suffix);

    // 'linkName' may be a symlink or hardlink.  If it's a symlink, resolve it,
    // otherwise return 'linkName', which won't look anything like 'argv[0]'
    // but will at least allow the caller to access the executable.  But if
    // '/proc' is unavailable (sometimes the case) it won't work.

    if (u::isExecutable(linkName)) {
        struct stat s;
        rc = ::lstat(linkName, &s);
        if (0 == rc && S_ISLNK(s.st_mode)) {
            // It's a symbolic link.  Resolve it.

            enum { k_BUF_LEN = 64 * 1024 };

            bslma::Allocator *alloc = bslma::Default::defaultAllocator();
            char *linkBuf = static_cast<char *>(alloc->allocate(k_BUF_LEN));
            bslma::DeallocatorGuard<bslma::Allocator> guard(linkBuf, alloc);

            // If successful, 'readlink' returns the number of bytes in the
            // result.

            int numChars = static_cast<int>(::readlink(linkName,
                                                       linkBuf,
                                                       k_BUF_LEN));
            if (0 < numChars && numChars < k_BUF_LEN) {
                linkBuf[numChars] = 0;
                if (u::isExecutable(linkBuf)) {
                    result->assign(linkBuf, numChars);
                    return 0;                                         // RETURN
                }
            }
        }

        // 'linkName' is either a hard link, or 'readlink' failed or the
        // executable name was too long for the buffer.  At least we know that
        // 'linkName' will give a path to the executable file, though it
        // doesn't look anything like 'argv[0]'.

        result->assign(linkName);
        return 0;                                                     // RETURN
    }

#endif

    return -1;
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
