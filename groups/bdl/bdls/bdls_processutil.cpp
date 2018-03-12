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

#if !defined BSLS_PLATFORM_OS_WINDOWS
# include <unistd.h>           // 'getpid'
# include <sys/stat.h>         // 'stat'
# include <sys/types.h>

# if defined BSLS_PLATFORM_OS_AIX
#   include <bslmf_assert.h>
#   include <sys/procfs.h>
#   include <procinfo.h>
# elif defined BSLS_PLATFORM_OS_CYGWIN
#   ifndef   _REENTRANT
#     define _REENTRANT
#   endif

#   include <bsl_fstream.h>
#   include <fcntl.h>
#   include <procfs.h>
# elif defined BSLS_PLATFORM_OS_DARWIN
#   include <libproc.h>
# elif defined BSLS_PLATFORM_OS_HPUX
#   include <sys/pstat.h>
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
    return 0 == rc && !(s.st_mode & S_IFDIR) && (s.st_mode & executableBits);
#else
    return BloombergLP::bdls::FilesystemUtil::isRegularFile(path);
#endif
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

    bslma::Allocator *allocator = result->allocator().mechanism();
    (void) allocator;    // suppress 'unused' warnings

#if defined BSLS_PLATFORM_OS_AIX

    enum { k_BUF_LEN = 64 * 1024 };

    // Fill 'argsBuf' with non-'\0' so our 'find' later will detect if
    // '::getargs' failed to null-terminate its result.

    bsl::string argsBuf(k_BUF_LEN, '*', allocator);

    struct procentry64  procBuf;
    procBuf.pi_pid = getProcessId();

    // '::getargs' should fill the beginning of 'argsBuf' with null-terminated
    // 'argv[0]', which might be a relative path.

    if (0 != ::getargs(&procBuf, sizeof(procBuf), &argsBuf[0], k_BUF_LEN)) {
        return -1;                                                    // RETURN
    }
    const bsl::size_t pos = argsBuf.find('\0');
    BSLMF_ASSERT(k_BUF_LEN < bsl::string::npos);
    if (k_BUF_LEN <= pos) {
        return -1;                                                    // RETURN
    }

    // 'assign' here, not 'swap' since the buffer for 'argsBuf' is irreversibly
    // extremely large and the returned value will nearly always be much
    // shorter.

    result->assign(argsBuf.c_str(), pos);
    return 0;

#elif defined BSLS_PLATFORM_OS_CYGWIN

    enum { NUM_ELEMENTS = 6 + 30 + 8 };  // "/proc/<pid>/cmdline" -- way more
                                         // than enough

    char procfs[NUM_ELEMENTS];
    bsl::sprintf(procfs, "/proc/%d/cmdline", getProcessId());

    bsl::ifstream ifs;
    ifs.open(procfs, bsl::ios_base::in | bsl::ios_base::binary);
    if (ifs.fail()) {
        return -1;                                                    // RETURN
    }
    ifs >> *result;

    bsl::string::size_type pos = result->find('\0');
    if (bsl::string::npos != pos) {
        result->resize(pos);
    }

    return 0;

#elif defined BSLS_PLATFORM_OS_DARWIN

    // We empirically determined that 'proc_pidpath' won't tolerate a buffer
    // much longer than 4K, and 4K is short enough not to be worth dynamically
    // allocating.

    enum { k_BUF_LEN = 4 * 1024 };
    char buf[k_BUF_LEN];

    // Fill 'buf' with non-'\0' so our 'find' later will detect if
    // 'proc_pidpath' failed to null-terminate its result.

    bsl::fill(buf + 0, buf + k_BUF_LEN, '*');

    if (proc_pidpath(getProcessId(), buf, k_BUF_LEN) <= 0) {
        return -1;                                                    // RETURN
    }
    if (buf + k_BUF_LEN == bsl::find(buf + 0, buf + k_BUF_LEN, 0)) {
        return -1;                                                    // RETURN
    }

    result->assign(buf);
    return 0;

#elif defined BSLS_PLATFORM_OS_HPUX

    bsl::string path(allocator);
    path.resize(1024);
    int rc = pstat_getcommandline(&path[0],
                                  path.size(),
                                  1,
                                  getProcessId());
    if (rc < 0) {
        return -1;                                                    // RETURN
    }

    bsl::string::size_type pos = path.find_first_of(' ');
    if (bsl::string::npos != pos) {
        path.resize(pos);
    }

    // Using 'assign' not 'swap', as 'path' has already irreversibly grown to
    // a 1024 byte buffer, and the final value will probably be much shorter.

    result->assign(path);
    return 0;

#elif defined BSLS_PLATFORM_OS_LINUX

    // We read '::program_invocation_name', which will just yield 'argv[0]',
    // which may or may not be a relative path.  Note that this will still
    // correctly handle the case where the path of the executable contains
    // spaces.

    result->assign(::program_invocation_name);
    return 0;

#elif defined BSLS_PLATFORM_OS_SOLARIS

    // '::getexecname' will return an absolute path with symlinks resolved.

    result->assign(::getexecname());
    return 0;

#elif defined BSLS_PLATFORM_OS_WINDOWS

    // On Windows, 'argv[0]' is always an absolute path, even if it was
    // specified on the command line as a relative path.  The following code
    // yields the full path.

    bsl::wstring wResult(allocator);
    for (unsigned wSize = MAX_PATH; wSize <= 4 * MAX_PATH; wSize *= 2) {
        wResult.resize(wSize, L'\0');
        DWORD length = GetModuleFileNameW(0, &wResult[0], wResult.length());
        if (length == 0) {  // Error
            return 1;                                                 // RETURN
        }
        else if (length < wResult.length()) {  // Success
            wResult.resize(length);

            bsl::string utf8Path(allocator);
            int rc = bdlde::CharConvertUtf16::utf16ToUtf8(&utf8Path, wResult);
            if (0 != rc) {
                return -1;                                            // RETURN
            }

            result->swap(utf8Path);
            return 0;                                                 // RETURN
        }
    }

    return -1;
#else
# error    Unrecognized Platform
#endif
}

int ProcessUtil::getPathToExecutable(bsl::string *result)
{
    BSLS_ASSERT(result);

    bslma::Allocator *allocator = result->allocator().mechanism();
    (void) allocator;    // suppress 'unused' warnings
    int rc;

    // Our first priority is to give a path to the executable file.  Our second
    // priority is to give a path that looks something like 'argv[0]'.  First,
    // we attempt methods that give us a nice looking result.

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

        bsl::string linkBuf(k_BUF_LEN, '\0', allocator);

        // If successful, 'readlink' returns the number of bytes in the result.

        bsls::Types::Int64 numChars = ::readlink(linkName,
                                                 &linkBuf[0],
                                                 k_BUF_LEN);
        if (0 < numChars && numChars < k_BUF_LEN) {
            linkBuf.resize(numChars);
            if (u::isExecutable(linkBuf.c_str())) {
                result->assign(linkBuf);
                return 0;                                             // RETURN
            }
        }
    }

#endif

    // Try 'getProcessName' and see if it gives a path to a file.  This method
    // is prone to yielding relative paths, but if there's an executable file
    // at the yielded path, that is good enough.

    bsl::string processName(allocator);
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
