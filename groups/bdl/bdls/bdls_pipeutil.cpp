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
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <winerror.h>
#include <bdlde_charconvertutf16.h>
#else
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <bdlde_utf8util.h>
#endif

#include <bsl_cstdlib.h>
#include <bsl_cctype.h>
#include <bsl_algorithm.h>

namespace BloombergLP {

// HELPER FUNCTIONS

namespace {

#ifdef BSLS_PLATFORM_OS_UNIX

template <class STRING_TYPE>
inline
void getPipeDir(STRING_TYPE *dir)
    // open an appropriate working directory
{
    BSLS_ASSERT(dir);

    char *tmpdir = bsl::getenv("SOCKDIR");
    if (0 == tmpdir) {
        tmpdir = bsl::getenv("TMPDIR");
    }
    if (0 != tmpdir) {
        dir->assign(tmpdir);
    }
    else {
        int rc = bdls::FilesystemUtil::getWorkingDirectory(dir);
        (void)rc;
        BSLS_ASSERT(0 == rc);
    }
}

#else  // Windows

template <class STRING_TYPE>
inline
void getPipeDir(STRING_TYPE *dir)
{
    BSLS_ASSERT(dir);

    dir->assign("\\\\.\\pipe\\");
}

#endif

template <class STRING_TYPE, class STRING_REF_TYPE>
int u_makeCanonicalName(STRING_TYPE *pipeName, const STRING_REF_TYPE& baseName)
{
    BSLS_ASSERT(pipeName);

    getPipeDir(pipeName);

    int rc = bdls::PathUtil::appendIfValid(pipeName, baseName);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    typename STRING_TYPE::iterator where = pipeName->end() - baseName.length();

    bsl::transform(where, pipeName->end(), where, (int(*)(int))bsl::tolower);
    return 0;
}


} // close unnamed namespace

namespace bdls {
                              // ---------------
                              // struct PipeUtil
                              // ---------------

// CLASS METHODS
int PipeUtil::makeCanonicalName(bsl::string             *pipeName,
                                const bsl::string_view&  baseName)
{
    return u_makeCanonicalName(pipeName, baseName);
}

int PipeUtil::makeCanonicalName(std::string             *pipeName,
                                const bsl::string_view&  baseName)
{
    return u_makeCanonicalName(pipeName, baseName);
}

#ifdef BSLS_LIBRARYFEATURES_HAS_CPP17_PMR
int PipeUtil::makeCanonicalName(std::pmr::string        *pipeName,
                                const bsl::string_view&  baseName)
{
    return u_makeCanonicalName(pipeName, baseName);
}
#endif

}  // close package namespace

#ifdef BSLS_PLATFORM_OS_WINDOWS

namespace bdls {
int PipeUtil::send(const bsl::string_view& pipeName,
                   const bsl::string_view& message)
{
    bsl::wstring wPipeName;

    if (0 != bdlde::CharConvertUtf16::utf8ToUtf16(&wPipeName, pipeName)) {
        BSLS_ASSERT(0 && "'pipeName' is an invalid UTF-8 string.");
        return -1;                                                    // RETURN
    }

    HANDLE pipe;
    do {
        WaitNamedPipeW(wPipeName.data(), NMPWAIT_WAIT_FOREVER);
        pipe = CreateFileW(wPipeName.data(), GENERIC_WRITE, 0, NULL,
                           OPEN_EXISTING, 0, NULL);
    }
    while (INVALID_HANDLE_VALUE == pipe);

    // NOTE: The below attempt to set the read mode to 'PIPE_READMODE_MESSAGE'
    // will silently fail if the named pipe was created in byte mode (e.g., by
    // 'balb::PipeControlChannel').  The message will then be sent as a
    // sequence of bytes.
    DWORD mode = PIPE_READMODE_MESSAGE;
    SetNamedPipeHandleState(pipe, &mode, NULL, NULL);

    DWORD dummy;
    bool result = WriteFile(pipe, message.data(), message.length(), &dummy, 0);
    CloseHandle(pipe);
    return (!result);
}

bool
PipeUtil::isOpenForReading(const bsl::string_view& pipeName)
{
    bsl::wstring wPipeName;

    if (0 != bdlde::CharConvertUtf16::utf8ToUtf16(&wPipeName, pipeName)) {
        BSLS_ASSERT(0 && "'pipeName' is an invalid UTF-8 string.");
        return false;                                                 // RETURN
    }

    HANDLE pipe = CreateFileW(wPipeName.data(),
                              GENERIC_WRITE, 0, NULL,
                              OPEN_EXISTING, 0, NULL);

    if (INVALID_HANDLE_VALUE == pipe) {
        return false;                                                 // RETURN
    }

    FileDescriptorGuard guard(pipe);

    DWORD mode = PIPE_READMODE_BYTE;
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
int PipeUtil::send(const bsl::string_view& pipeName,
                   const bsl::string_view& message)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(pipeName.data(), pipeName.length()));

    bsl::string safeName(pipeName);
    int pipe = open(safeName.c_str(), O_WRONLY);
    if (-1 == pipe) {
        return -1;                                                    // RETURN
    }

    int rc = static_cast<int>(write(pipe,
                                    message.data(),
                                    message.length()));
    (void)rc;
    close(pipe);
    return !(rc > 0);
}

bool
PipeUtil::isOpenForReading(const bsl::string_view& pipeName)
{
    BSLS_ASSERT(bdlde::Utf8Util::isValid(pipeName.data(), pipeName.length()));

    bsl::string safeName(pipeName);
    int fd = open(safeName.c_str(), O_WRONLY | O_NONBLOCK);
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
