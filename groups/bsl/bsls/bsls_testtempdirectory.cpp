// bsls_testtempdirectory.cpp                                         -*-C++-*-
#include <bsls_testtempdirectory.h>

#include <bsls_ident.h>
BSLS_IDENT("$Id$ $CSID$")

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif
# include <windows.h>
# include <process.h>    // `_getpid`
#else
# include <dirent.h>     // `opendir`, `readdir`, `closedir`
# include <sys/stat.h>   // `lstat`
# include <sys/types.h>
# include <unistd.h>     // `getpid`, `rmdir`, `unlink`
#endif

namespace BloombergLP {
namespace {

enum { k_PATH_BUFFER_SIZE = bsls::TestTempDirectory::k_PATH_BUFFER_SIZE };

/// Print a message containing the specified `what` and `path`, and the last
/// system error (`GetLastError` on Windows, `errno` otherwise), to `stderr`,
/// and abort the program.
void fail(const char *what, const char *path)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    const DWORD error = GetLastError();
    fprintf(stderr,
            "bsls::TestTempDirectory: %s: \"%s\" (GetLastError %lu)\n",
            what,
            path,
            static_cast<unsigned long>(error));
#else
    const int error = errno;
    fprintf(stderr,
            "bsls::TestTempDirectory: %s: \"%s\" (errno %d: %s)\n",
            what,
            path,
            error,
            strerror(error));
#endif
    fflush(stderr);
    abort();
}

/// Return the name of the user running the process, or "unknown" if it is
/// not available.
const char *userName()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    const char *result = getenv("USERNAME");
#else
    const char *result = getenv("USER");
    if (!result || '\0' == *result) {
        result = getenv("LOGNAME");
    }
#endif
    return result && '\0' != *result ? result : "unknown";
}

/// Return the id of the current process.
int processId()
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return static_cast<int>(_getpid());
#else
    return static_cast<int>(getpid());
#endif
}

#ifdef BSLS_PLATFORM_OS_WINDOWS

/// Remove the directory having the specified `path`, and everything in it.
/// Return `true` on success, and `false` otherwise.
bool removeAll(const char *path)
{
    char pattern[k_PATH_BUFFER_SIZE];
    int  length = _snprintf(pattern, sizeof pattern, "%s\\*", path);
    if (length < 0 || static_cast<int>(sizeof pattern) <= length) {
        return false;                                                 // RETURN
    }

    bool             result = true;
    WIN32_FIND_DATAA data;
    HANDLE           handle = FindFirstFileA(pattern, &data);
    if (INVALID_HANDLE_VALUE != handle) {
        do {
            if (0 == strcmp(data.cFileName, ".") ||
                0 == strcmp(data.cFileName, "..")) {
                continue;
            }

            char child[k_PATH_BUFFER_SIZE];
            length = _snprintf(child,
                               sizeof child,
                               "%s\\%s",
                               path,
                               data.cFileName);
            if (length < 0 || static_cast<int>(sizeof child) <= length) {
                result = false;
                continue;
            }

            const DWORD attributes = data.dwFileAttributes;
            if ((attributes & FILE_ATTRIBUTE_DIRECTORY) &&
                !(attributes & FILE_ATTRIBUTE_REPARSE_POINT)) {
                result = removeAll(child) && result;
            }
            else {
                if (attributes & FILE_ATTRIBUTE_READONLY) {
                    SetFileAttributesA(child, FILE_ATTRIBUTE_NORMAL);
                }
                if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
                    result = RemoveDirectoryA(child) && result;
                }
                else {
                    result = DeleteFileA(child) && result;
                }
            }
        } while (FindNextFileA(handle, &data));
        FindClose(handle);
    }

    return RemoveDirectoryA(path) && result;
}

#else

/// Remove the directory having the specified `path`, and everything in it.
/// Return `true` on success, and `false` otherwise.  Symbolic links are
/// removed, not followed.
bool removeAll(const char *path)
{
    DIR *dir = opendir(path);
    if (dir) {
        // POSIX does not say whether `readdir` still returns every entry after
        // entries are removed from the directory, so make passes, from the
        // start, until a pass removes nothing.

        bool removed;
        do {
            removed = false;
            rewinddir(dir);
            while (struct dirent *entry = readdir(dir)) {
                if (0 == strcmp(entry->d_name, ".") ||
                    0 == strcmp(entry->d_name, "..")) {
                    continue;
                }

                char      child[k_PATH_BUFFER_SIZE];
                const int length = snprintf(child,
                                            sizeof child,
                                            "%s/%s",
                                            path,
                                            entry->d_name);
                if (length < 0 || static_cast<int>(sizeof child) <= length) {
                    continue;
                }

                struct stat info;
                if (0 == lstat(child, &info) && S_ISDIR(info.st_mode)) {
                    removed = removeAll(child) || removed;
                }
                else {
                    removed = 0 == unlink(child) || removed;
                }
            }
        } while (removed);
        closedir(dir);
    }

    // `rmdir` fails if anything is left in the directory.

    return 0 == rmdir(path);
}

#endif

}  // close unnamed namespace

namespace bsls {

                          // -----------------------
                          // class TestTempDirectory
                          // -----------------------

// CREATORS
TestTempDirectory::TestTempDirectory(const char *testDriverName,
                                     int         testCase)
{
    d_path[0] = '\0';

    if (!testDriverName || '\0' == *testDriverName) {
        fail("test driver name is empty", "");
    }

#ifdef BSLS_PLATFORM_OS_WINDOWS
    char  tmpDir[MAX_PATH + 1];
    const DWORD tmpDirSize   = static_cast<DWORD>(sizeof tmpDir);
    DWORD       tmpDirLength = GetTempPathA(tmpDirSize, tmpDir);
    if (0 == tmpDirLength || tmpDirSize <= tmpDirLength) {
        fail("cannot get the temporary directory", "");
    }

    // Remove the trailing separator returned by `GetTempPathA`.

    while (0 < tmpDirLength && ('\\' == tmpDir[tmpDirLength - 1] ||
                                '/'  == tmpDir[tmpDirLength - 1])) {
        tmpDir[--tmpDirLength] = '\0';
    }

    const int pid = processId();

    static volatile LONG counter = 0;
    for (int attempt = 0; attempt < 100; ++attempt) {
        const unsigned suffix = static_cast<unsigned>(GetTickCount())
                              ^ (static_cast<unsigned>(
                                       InterlockedIncrement(&counter)) << 20);

        const int length = _snprintf(d_path,
                                     sizeof d_path,
                                     "%s\\%s-%s-%d-%d-%08x",
                                     tmpDir,
                                     userName(),
                                     testDriverName,
                                     testCase,
                                     pid,
                                     suffix);
        if (length < 0 || static_cast<int>(sizeof d_path) <= length) {
            d_path[sizeof d_path - 1] = '\0';
            fail("temporary directory path is too long", d_path);
        }

        if (CreateDirectoryA(d_path, 0)) {
            return;                                                   // RETURN
        }
        if (ERROR_ALREADY_EXISTS != GetLastError()) {
            fail("cannot create temporary directory", d_path);
        }
    }
    fail("cannot create a uniquely named temporary directory", d_path);
#else
    const char *tmpDir = getenv("TMPDIR");
    if (!tmpDir || '\0' == *tmpDir) {
        tmpDir = "/tmp";
    }

    // Drop trailing separators, so the format below adds exactly one.  If
    // `TMPDIR` is `/`, this leaves an empty prefix, and the path is
    // `/<name>`.

    int tmpDirLength = static_cast<int>(strlen(tmpDir));
    while (0 < tmpDirLength && '/' == tmpDir[tmpDirLength - 1]) {
        --tmpDirLength;
    }

    const int length = snprintf(d_path,
                                sizeof d_path,
                                "%.*s/%s-%s-%d-%d-XXXXXX",
                                tmpDirLength,
                                tmpDir,
                                userName(),
                                testDriverName,
                                testCase,
                                processId());
    if (length < 0 || static_cast<int>(sizeof d_path) <= length) {
        d_path[sizeof d_path - 1] = '\0';
        fail("temporary directory path is too long", d_path);
    }

    if (!mkdtemp(d_path)) {
        fail("cannot create temporary directory", d_path);
    }
#endif
}

TestTempDirectory::~TestTempDirectory()
{
    if ('\0' != d_path[0] && !removeAll(d_path)) {
        fprintf(stderr,
                "bsls::TestTempDirectory: warning: cannot remove \"%s\"\n",
                d_path);
    }
}

// ACCESSORS
int TestTempDirectory::makePath(char       *result,
                                size_t      size,
                                const char *name) const
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    const char k_SEPARATOR = '\\';
#else
    const char k_SEPARATOR = '/';
#endif

    const size_t pathLength = strlen(d_path);
    const size_t nameLength = strlen(name);

    // Room for the path, the separator, the name, and the null character.

    if (size < pathLength + nameLength + 2) {
        if (0 < size) {
            result[0] = '\0';
        }
        return -1;                                                    // RETURN
    }

    memcpy(result, d_path, pathLength);
    result[pathLength] = k_SEPARATOR;
    memcpy(result + pathLength + 1, name, nameLength + 1);
    return 0;
}

}  // close package namespace
}  // close enterprise namespace

// ----------------------------------------------------------------------------
// Copyright 2026 Bloomberg Finance L.P.
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
