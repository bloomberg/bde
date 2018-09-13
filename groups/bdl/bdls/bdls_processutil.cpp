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
#   include <stdlib.h>
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
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return static_cast<int>(GetCurrentProcessId());
#else
    return static_cast<int>(::getpid());
#endif
}

int ProcessUtil::getProcessName(bsl::string *result)
{
    BSLS_ASSERT(result);

#if defined BSLS_PLATFORM_OS_AIX

    enum { k_BUF_LEN = 64 * 1024 };

    // Fill 'argsBuf' with non-'\0' so our 'find' later will detect if
    // '::getargs' failed to null-terminate its result.

    bsl::string argsBuf(k_BUF_LEN, '*');
    char *buf = &argsBuf[0], *bufEnd = buf + k_BUF_LEN;

    struct procentry64  procBuf;
    procBuf.pi_pid = getProcessId();

    // '::getargs' should fill the beginning of 'argsBuf' with null-terminated
    // 'argv[0]', which might be a relative path.

    if (0 != ::getargs(&procBuf, sizeof(procBuf), buf, k_BUF_LEN)) {
        return -1;                                                    // RETURN
    }
    if (bufEnd == bsl::find(buf, bufEnd, '\0')) {
        return -1;                                                    // RETURN
    }

    // 'assign' here, not 'swap' since the buffer for 'argsBuf' is irreversibly
    // extremely large and the returned value will nearly always be much
    // shorter.

    result->assign(argsBuf.c_str());
    return 0;

#elif defined BSLS_PLATFORM_OS_DARWIN

    bsl::string pidPathBuf(PROC_PIDPATHINFO_MAXSIZE, '\0');

    int numChars = ::proc_pidpath(getProcessId(),
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

    result->assign(::program_invocation_name);
    return 0;

#elif defined BSLS_PLATFORM_OS_SOLARIS

    // '::getexecname' will return 'argv[0]' with symlinks resolved, or 0 if it
    // fails.

    const char *execName = ::getexecname();
    if (!execName) {
        return -1;                                                    // RETURN
    }
    result->assign(execName);
    return 0;

#elif defined BSLS_PLATFORM_OS_WINDOWS

    // This code seems to be returning 'argv[0]' except that, on Windows,
    // 'argv[0]' is always an absolute path, even if it was specified on the
    // command line as a relative path.  The following code yields the full
    // path.

    bsl::wstring wResult(64 * 1024, '*');
    DWORD length = GetModuleFileNameW(0, &wResult[0], wResult.length());
    if (0 < length && length < wResult.length()) {
        wResult.resize(length);

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

    // Our first priority is to give a path to the executable file.  Our second
    // priority is to give a path that looks something like 'argv[0]'.  First,
    // we attempt methods that give us a nice looking result.

    int rc;

#if defined BSLS_PLATFORM_OS_LINUX

    // We attempt to get the executable name through the symlink
    // "/proc/self/exe", because, if successful, that will yield a full path
    // with symlinks resolved, but it will normally look something like
    // 'argv[0]', the best of all possible worlds.  But if '/proc' is
    // unavailable (sometimes the case) it won't work.

    const char linkName[] = { "/proc/self/exe" };
    struct stat s;
    rc = ::lstat(linkName, &s);
    if (0 == rc && S_ISLNK(s.st_mode)) {
        enum { k_BUF_LEN = 64 * 1024 };

        bsl::string linkBuf(k_BUF_LEN, '\0');

        // If successful, 'readlink' returns the number of bytes in the result.

        bsls::Types::Int64 numChars = ::readlink(linkName,
                                                 &linkBuf[0],
                                                 k_BUF_LEN);

        if (0 < numChars && numChars < k_BUF_LEN) {
            linkBuf.resize(static_cast<bsl::string::size_type>(numChars));
            if (u::isExecutable(linkBuf.c_str())) {
                // Use 'assign', not 'swap' is the buffer of 'linkBuf' has
                // irreversibly grown to 64K long.

                result->assign(linkBuf);
                return 0;                                             // RETURN
            }
        }
    }

#endif

    // Try 'getProcessName' and see if it gives a path to a file.  This method
    // is prone to yielding relative paths, but if there's an executable file
    // at the yielded path, that is good enough.

    bsl::string processName(result->allocator().mechanism());
    rc = getProcessName(&processName);
    if (0 == rc && u::isExecutable(processName)) {
        result->swap(processName);
        return 0;                                                     // RETURN
    }

#if defined BSLS_PLATFORM_OS_AIX || defined BSLS_PLATFORM_OS_SOLARIS

    // If we got here, it probably means that 'getProcessName' yielded a
    // relative path and we've changed the working directory since task
    // startup, or, less likely, someone unlinked the executable file from the
    // file system.  If "/proc" is available, this will give us an ugly, but
    // usable path to a hard link to the executable.

    char fileNameBuf[6 + 30 + 7 + 6];    // way more than enough
    bsl::sprintf(fileNameBuf,
                 "/proc/%d/object/a.out",
                 getProcessId());
    if (u::isExecutable(fileNameBuf)) {
        result->assign(fileNameBuf);
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
