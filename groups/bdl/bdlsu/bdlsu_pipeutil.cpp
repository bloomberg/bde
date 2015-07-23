// bdlsu_pipeutil.cpp                                                 -*-C++-*-
#include <bdlsu_pipeutil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(bdlsu_pipeutil_cpp,"$Id$ $CSID$")

#include <bdlsu_filedescriptorguard.h>
#include <bdlsu_xxxfileutil.h>
#include <bdlsu_pathutil.h>

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
        int rc = bdlsu::FileUtil::getWorkingDirectory(dir);
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

namespace bdlsu {
                              // ---------------------
                              // struct PipeUtil
                              // ---------------------

// CLASS METHODS
int
PipeUtil::makeCanonicalName(bsl::string           *pipeName,
                                  const bslstl::StringRef& baseName)
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

namespace bdlsu {
int
PipeUtil::send(const bslstl::StringRef& pipeName,
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

namespace bdlsu {
int
PipeUtil::send(const bslstl::StringRef& pipeName,
                     const bslstl::StringRef& message)
{
    int pipe = open(pipeName.data(), O_WRONLY);
    if (-1 == pipe) {
        return -1;
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
        return true;
    }
    return false;
}
}  // close package namespace

#endif

}  // close namespace BloombergLP

// ----------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2008
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ------------------------------ END-OF-FILE ---------------------------------
