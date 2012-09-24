// bdesu_pipeutil.cpp                                                 -*-C++-*-
#include <bdesu_pipeutil.h>

#include <bdes_ident.h>
BDES_IDENT_RCSID(bdesu_pipeutil_cpp,"$Id$ $CSID$")

#include <bdesu_fileutil.h>
#include <bdesu_pathutil.h>

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
        int rc = bdesu_FileUtil::getWorkingDirectory(dir);
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

                              // ---------------------
                              // struct bdesu_PipeUtil
                              // ---------------------

// CLASS METHODS
int
bdesu_PipeUtil::makeCanonicalName(bsl::string           *pipeName,
                                  const bdeut_StringRef& baseName)
{
    BSLS_ASSERT(pipeName);

    getPipeDir(pipeName);

    int rc = bdesu_PathUtil::appendIfValid(pipeName, baseName);
    if (0 != rc) {
        return rc;                                                    // RETURN
    }

    bsl::string::iterator where = pipeName->end() - baseName.length();

    bsl::transform(where, pipeName->end(), where, (int(*)(int))bsl::tolower);
    return 0;
}

#ifdef BSLS_PLATFORM_OS_WINDOWS

int
bdesu_PipeUtil::send(const bdeut_StringRef& pipeName,
                     const bdeut_StringRef& message)
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
bdesu_PipeUtil::isOpenForReading(const bdeut_StringRef& pipeName)
{
    HANDLE pipe = CreateFile(pipeName.data(),
                             GENERIC_WRITE, 0, NULL,
                             OPEN_EXISTING, 0, NULL);
    if (INVALID_HANDLE_VALUE == pipe) {
        return false;                                                 // RETURN
    }

    DWORD mode = PIPE_READMODE_MESSAGE;
    if (0 == SetNamedPipeHandleState(pipe, &mode, NULL, NULL)) {
        return false;                                                 // RETURN
    }

    DWORD dummy;
    if (0 == WriteFile(pipe, "\n", 1, &dummy, 0)) {
        return false;                                                 // RETURN
    }

    FlushFileBuffers(pipe);
    CloseHandle(pipe);

    return true;
}

#else // UNIX implementation

int
bdesu_PipeUtil::send(const bdeut_StringRef& pipeName,
                     const bdeut_StringRef& message)
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
bdesu_PipeUtil::isOpenForReading(const bdeut_StringRef& pipeName)
{
    int fd = open(pipeName.data(), O_WRONLY | O_NONBLOCK);
    if (-1 != fd) {
        close(fd);
        return true;
    }
    return false;
}

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
