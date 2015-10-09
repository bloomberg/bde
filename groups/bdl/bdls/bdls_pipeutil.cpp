// bdls_pipeutil.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_pipeutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdls_pipeutil_cpp,"$Id$ $CSID$")

#include <bdls_filedescriptorguard.h>
#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>

#include <bsls_assert.h>
#include <bsls_platform.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <winerror.h>
#else
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <bsl_cstdlib.h>
#include <bsl_cctype.h>
#include <bsl_algorithm.h>

namespace BloombergLP {

// STATIC HELPER FUNCTIONS

#ifdef BSLS_PLATFORM_OS_UNIX

static
void getPipeDir(bsl::string *dir)
    // open an appropriate working directory
{
    BSLS_ASSERT(dir);

    char *tmpdir = bsl::getenv("TMPDIR");
    if (0 != tmpdir) {
        dir->assign(tmpdir);
    }
    else {
        int rc = bdls::FilesystemUtil::getWorkingDirectory(dir);
        BSLS_ASSERT(0 == rc);
    }
}

#else  // Windows

static
void getPipeDir(bsl::string *dir)
{
    BSLS_ASSERT(dir);

    dir->assign("\\\\.\\pipe\\");
}

#endif

namespace bdls {
                              // ---------------
                              // struct PipeUtil
                              // ---------------

// CLASS METHODS
int PipeUtil::makeCanonicalName(bsl::string              *pipeName,
                                const bslstl::StringRef&  baseName)
{
    BSLS_ASSERT(pipeName);

    getPipeDir(pipeName);

    int rc = PathUtil::appendIfValid(pipeName, baseName);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    bsl::string::iterator where = pipeName->end() - baseName.length();

    bsl::transform(where, pipeName->end(), where, (int(*)(int))bsl::tolower);
    return 0;
}
}  // close package namespace

#ifdef BSLS_PLATFORM_OS_WINDOWS

namespace bdls {
int PipeUtil::send(const bslstl::StringRef& pipeName,
                   const bslstl::StringRef& message)
{
    HANDLE pipe;
    do {
        WaitNamedPipe(pipeName.data(), NMPWAIT_WAIT_FOREVER);
        pipe = CreateFile(pipeName.data(), GENERIC_WRITE, 0, NULL,
                          OPEN_EXISTING, 0, NULL);
    }
    while (INVALID_HANDLE_VALUE == pipe);

    DWORD mode = PIPE_READMODE_MESSAGE;
    SetNamedPipeHandleState(pipe, &mode, NULL, NULL);

    DWORD dummy;
    bool result = WriteFile(pipe, message.data(), message.length(), &dummy, 0);
    CloseHandle(pipe);
    return (!result);
}

bool
PipeUtil::isOpenForReading(const bslstl::StringRef& pipeName)
{
    HANDLE pipe = CreateFile(pipeName.data(),
                             GENERIC_WRITE, 0, NULL,
                             OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == pipe) {
        return false;                                                 // RETURN
    }

    FileDescriptorGuard guard(pipe);

    DWORD mode = PIPE_READMODE_MESSAGE;
    if (0 == SetNamedPipeHandleState(pipe, &mode, NULL, NULL)) {
        return false;                                                 // RETURN
    }

    DWORD dummy;
    if (0 == WriteFile(pipe, "\n", 1, &dummy, 0)) {
        return false;                                                 // RETURN
    }

    FlushFileBuffers(pipe);

    return true;
}
}  // close package namespace

#else // UNIX implementation

namespace bdls {
int PipeUtil::send(const bslstl::StringRef& pipeName,
                   const bslstl::StringRef& message)
{
    int pipe = open(pipeName.data(), O_WRONLY);
    if (-1 == pipe) {
        return -1;                                                    // RETURN
    }

    int rc = write(pipe, message.data(), message.length());
    close(pipe);
    return !(rc > 0);
}

bool
PipeUtil::isOpenForReading(const bslstl::StringRef& pipeName)
{
    int fd = open(pipeName.data(), O_WRONLY | O_NONBLOCK);
    if (-1 != fd) {
        close(fd);
        return true;                                                  // RETURN
    }
    return false;
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
