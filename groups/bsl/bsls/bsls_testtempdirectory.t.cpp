// bsls_testtempdirectory.t.cpp                                       -*-C++-*-
#include <bsls_testtempdirectory.h>

#include <bsls_platform.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif
# include <windows.h>
# include <direct.h>     // `_mkdir`
# include <process.h>    // `_getpid`
#else
# include <sys/stat.h>
# include <sys/statvfs.h>
# include <sys/types.h>
# include <unistd.h>
#endif

using namespace BloombergLP;

// ============================================================================
//                             TEST PLAN
// ----------------------------------------------------------------------------
//                             Overview
//                             --------
// The component under test is a scoped guard that creates a directory at
// construction and removes it, and its contents, at destruction.  We verify
// the name and location of the directory, that it is created, and that it and
// everything in it are removed.  This component is below `bsls_bsltestutil`,
// so this test driver defines its own test macros.
// ----------------------------------------------------------------------------
// CREATORS
// [ 2] TestTempDirectory(const char *testDriverName, int testCase);
// [ 2] ~TestTempDirectory();
//
// ACCESSORS
// [ 4] int makePath(char *result, size_t size, const char *name) const;
// [ 2] const char *path() const;
// ----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] CONCERN: `TMPDIR` NAMES THE PARENT DIRECTORY
// [ 5] USAGE EXAMPLE

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        printf("Error " __FILE__ "(%d): %s    (failed)\n", line, message);
        fflush(stdout);

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

#define ASSERT(X) aSsErT(!(X), #X, __LINE__)

#define ASSERTV(V, X)                                                         \
    do {                                                                      \
        if (!(X)) {                                                           \
            printf("%s: %s\n", #V, (V));                                      \
            aSsErT(true, #X, __LINE__);                                       \
        }                                                                     \
    } while (false)

// ============================================================================
//                     GLOBAL TYPEDEFS AND HELPERS FOR TESTING
// ----------------------------------------------------------------------------

typedef bsls::TestTempDirectory Obj;

namespace {
namespace u {

#ifdef BSLS_PLATFORM_OS_WINDOWS
const char k_SEPARATOR = '\\';
#else
const char k_SEPARATOR = '/';
#endif

/// Return `true` if the specified `path` names an existing directory, and
/// `false` otherwise.
bool isDirectory(const char *path)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    const DWORD attributes = GetFileAttributesA(path);
    return INVALID_FILE_ATTRIBUTES != attributes &&
           (attributes & FILE_ATTRIBUTE_DIRECTORY);
#else
    struct stat info;
    return 0 == stat(path, &info) && S_ISDIR(info.st_mode);
#endif
}

/// Return `true` if the specified `path` names an existing file system
/// entry, and `false` otherwise.  Symbolic links are not followed.
bool exists(const char *path)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return INVALID_FILE_ATTRIBUTES != GetFileAttributesA(path);
#else
    struct stat info;
    return 0 == lstat(path, &info);
#endif
}

/// Create a directory having the specified `path`.  Return `true` on
/// success, and `false` otherwise.
bool makeDirectory(const char *path)
{
#ifdef BSLS_PLATFORM_OS_WINDOWS
    return 0 == _mkdir(path);
#else
    return 0 == mkdir(path, 0700);
#endif
}

/// Create a file having the specified `path` and containing the specified
/// `text`.  Return `true` on success, and `false` otherwise.
bool writeFile(const char *path, const char *text)
{
    FILE *fp = fopen(path, "w");
    if (!fp) {
        return false;                                                 // RETURN
    }
    const bool result = EOF != fputs(text, fp);
    return 0 == fclose(fp) && result;
}

/// Load into the specified `result` of the specified `size` the specified
/// `parent` and `name` joined by a directory separator.
void join(char *result, size_t size, const char *parent, const char *name)
{
    const int length = snprintf(result,
                                size,
                                "%s%c%s",
                                parent,
                                k_SEPARATOR,
                                name);
    ASSERT(0 <= length && length < static_cast<int>(size));
}

/// Return the name of the user, computed as `bsls::TestTempDirectory` does.
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

/// Return the last component of the specified `path`.
const char *baseName(const char *path)
{
    const char *result = strrchr(path, k_SEPARATOR);
    return result ? result + 1 : path;
}

#ifndef BSLS_PLATFORM_OS_WINDOWS
/// Set the `TMPDIR` environment variable to the specified `value`, or unset
/// it if `value` is 0.
void setTmpDir(const char *value)
{
    if (value) {
        ASSERT(0 == setenv("TMPDIR", value, 1));
    }
    else {
        ASSERT(0 == unsetenv("TMPDIR"));
    }
}
#endif

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test    = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose = argc > 2;

    printf("TEST " __FILE__ " CASE %d\n", test);

    switch (test) { case 0:  // Zero is always the leading case.
      case 5: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        // 1. The usage example provided in the component header file compiles,
        //    links, and runs as shown.
        //
        // Plan:
        // 1. Incorporate usage example from header into test driver, remove
        //    leading comment characters, and replace `assert` with `ASSERT`.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) printf("\nUSAGE EXAMPLE"
                            "\n=============\n");

        char savedPath[Obj::k_PATH_BUFFER_SIZE];
        {

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Writing a Temporary File in a Test Case
/// - - - - - - - - - - - - - - - - - - - - - - - - -
// Suppose test case 3 of the test driver for `xyza_widget` must write data to
// a file and read it back.  First, we create a guard for the temporary
// directory of the test case:
// ```
    const int testCase = 3;

    bsls::TestTempDirectory tempDir("xyza_widget", testCase);
// ```
// Then, we build the name of a file in that directory:
// ```
    char fileName[bsls::TestTempDirectory::k_PATH_BUFFER_SIZE];
    ASSERT(0 == tempDir.makePath(fileName, sizeof fileName, "data.txt"));
// ```
// Next, we write the file and read it back:
// ```
    FILE *fp = fopen(fileName, "w+");
    ASSERT(fp);

    fputs("Hello, world!\n", fp);
    rewind(fp);

    char buffer[32];
    ASSERT(fgets(buffer, sizeof buffer, fp));
    ASSERT(0 == strcmp("Hello, world!\n", buffer));

    fclose(fp);
// ```
// Finally, when `tempDir` goes out of scope, the directory and `data.txt` are
// removed.

            strcpy(savedPath, tempDir.path());
        }
        ASSERTV(savedPath, !u::exists(savedPath));
      } break;
      case 4: {
        // --------------------------------------------------------------------
        // `makePath`
        //
        // Concerns:
        // 1. `makePath` loads `path()`, a directory separator, and the name,
        //    and returns 0, when the buffer is large enough.
        //
        // 2. A buffer exactly large enough for the path and its null
        //    character succeeds; one byte smaller fails.
        //
        // 3. On failure, `makePath` returns non-zero and loads the empty
        //    string, and writes nothing when the size is 0.
        //
        // 4. An empty name yields `path()` followed by a separator.
        //
        // Plan:
        // 1. Call `makePath` with a large buffer, an exact buffer, a buffer
        //    one byte short, and a size of 0, and check the result and the
        //    buffer each time.  (C-1..4)
        //
        // Testing:
        //   int makePath(char *result, size_t size, const char *name) const;
        // --------------------------------------------------------------------

        if (verbose) printf("\n`makePath`"
                            "\n==========\n");

        Obj mX("bsls_testtempdirectory", test);  const Obj& X = mX;

        char expected[Obj::k_PATH_BUFFER_SIZE];
        u::join(expected, sizeof expected, X.path(), "name.txt");
        const size_t expectedLength = strlen(expected);

        char buffer[Obj::k_PATH_BUFFER_SIZE];

        // Large buffer.

        ASSERT(0 == X.makePath(buffer, sizeof buffer, "name.txt"));
        ASSERTV(buffer, 0 == strcmp(expected, buffer));

        // Exact fit, and one byte short.

        memset(buffer, 'x', sizeof buffer);
        ASSERT(0 == X.makePath(buffer, expectedLength + 1, "name.txt"));
        ASSERTV(buffer, 0 == strcmp(expected, buffer));

        memset(buffer, 'x', sizeof buffer);
        ASSERT(0 != X.makePath(buffer, expectedLength, "name.txt"));
        ASSERT('\0' == buffer[0]);

        // Size 0 writes nothing.

        memset(buffer, 'x', sizeof buffer);
        ASSERT(0 != X.makePath(buffer, 0, "name.txt"));
        ASSERT('x' == buffer[0]);

        // Empty name.

        ASSERT(0 == X.makePath(buffer, sizeof buffer, ""));
        ASSERTV(buffer, strlen(X.path()) + 1 == strlen(buffer));
        ASSERTV(buffer, 0 == strncmp(X.path(), buffer, strlen(X.path())));
        ASSERTV(buffer, u::k_SEPARATOR == buffer[strlen(buffer) - 1]);
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // CONCERN: `TMPDIR` NAMES THE PARENT DIRECTORY
        //
        // Concerns:
        // 1. On Unix, the directory is created in the directory named by
        //    `TMPDIR`.
        //
        // 2. Trailing `/`s in `TMPDIR` do not produce `//` in the path.
        //
        // 3. If `TMPDIR` is unset or empty, the directory is created in
        //    `/tmp`.
        //
        // Plan:
        // 1. Create an outer guard, and set `TMPDIR` to its path, with no,
        //    one, and two trailing `/`s.  Create an inner guard and verify its
        //    path is the outer path, a `/`, and its base name.  (C-1..2)
        //
        // 2. Unset `TMPDIR`, then set it to the empty string.  Create a guard
        //    and verify its path begins with `/tmp/`.  Skip this step if
        //    `/tmp` is not writable or has no free blocks, since the guard
        //    would abort.  (C-3)
        //
        // Testing:
        //   CONCERN: `TMPDIR` NAMES THE PARENT DIRECTORY
        // --------------------------------------------------------------------

        if (verbose) printf(
                         "\nCONCERN: `TMPDIR` NAMES THE PARENT DIRECTORY"
                         "\n============================================\n");

#ifdef BSLS_PLATFORM_OS_WINDOWS
        if (verbose) printf("Not applicable on Windows.\n");
#else
        const char *savedTmpDir = getenv("TMPDIR");
        char        saved[Obj::k_PATH_BUFFER_SIZE];
        if (savedTmpDir) {
            strcpy(saved, savedTmpDir);
        }

        {
            Obj outer("bsls_testtempdirectory", test);

            char withSlash[Obj::k_PATH_BUFFER_SIZE];
            ASSERT(0 == outer.makePath(withSlash, sizeof withSlash, ""));

            char withSlashes[Obj::k_PATH_BUFFER_SIZE];
            ASSERT(0 == outer.makePath(withSlashes, sizeof withSlashes, "/"));

            const char *VALUES[] = { outer.path(), withSlash, withSlashes };

            for (size_t i = 0; i < sizeof VALUES / sizeof *VALUES; ++i) {
                u::setTmpDir(VALUES[i]);

                Obj inner("bsls_testtempdirectory", test);

                char expected[Obj::k_PATH_BUFFER_SIZE];
                u::join(expected,
                        sizeof expected,
                        outer.path(),
                        u::baseName(inner.path()));

                ASSERTV(inner.path(), 0 == strcmp(expected, inner.path()));
                ASSERTV(inner.path(), u::isDirectory(inner.path()));
            }
        }

        // The fallback creates a real directory in `/tmp`, and the
        // constructor aborts if it cannot.  Skip this check, rather than
        // abort, when `/tmp` is not writable or has no free blocks, as on the
        // CI host this component was written for.

        struct statvfs tmpStat;
        const bool     tmpIsUsable = 0 == access("/tmp", W_OK | X_OK)
                                  && 0 == statvfs("/tmp", &tmpStat)
                                  && 0 < tmpStat.f_bavail;
        if (!tmpIsUsable && verbose) {
            printf("Skipping the `/tmp` fallback: `/tmp` is not usable.\n");
        }

        const char *EMPTY_VALUES[] = { 0, "" };
        for (size_t i = 0;
             tmpIsUsable && i < sizeof EMPTY_VALUES / sizeof *EMPTY_VALUES;
             ++i) {
            u::setTmpDir(EMPTY_VALUES[i]);

            Obj mX("bsls_testtempdirectory", test);

            ASSERTV(mX.path(), 0 == strncmp("/tmp/", mX.path(), 5));
            ASSERTV(mX.path(), u::isDirectory(mX.path()));
        }

        u::setTmpDir(savedTmpDir ? saved : 0);
#endif
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // CREATORS AND `path`
        //
        // Concerns:
        // 1. The constructor creates a directory, and `path` returns its path.
        //
        // 2. The base name of the directory is the user name, the test driver
        //    name, the test case, and the process id, separated by `-`, then
        //    `-` and a suffix.
        //
        // 3. The path does not end with a directory separator.
        //
        // 4. Two guards for the same test driver and case have different
        //    paths.
        //
        // 5. The destructor removes the directory, and the files (including
        //    a directory of many files), nested directories, and (on Unix)
        //    symbolic links in it.
        //
        // 6. The destructor does not follow symbolic links.
        //
        // Plan:
        // 1. Create two guards, and verify their paths name distinct
        //    directories with the expected base name prefix.  (C-1..4)
        //
        // 2. Populate one directory with a file, a nested directory holding a
        //    file, a nested directory holding 500 files, and (on Unix) a
        //    symbolic link to a file outside the
        //    directory.  Destroy the guard and verify the directory is gone,
        //    and the file outside the directory remains.  (C-5..6)
        //
        // Testing:
        //   TestTempDirectory(const char *testDriverName, int testCase);
        //   ~TestTempDirectory();
        //   const char *path() const;
        // --------------------------------------------------------------------

        if (verbose) printf("\nCREATORS AND `path`"
                            "\n===================\n");

        char prefix[Obj::k_PATH_BUFFER_SIZE];
        snprintf(prefix,
                 sizeof prefix,
                 "%s-bsls_testtempdirectory-%d-%d-",
                 u::userName(),
                 test,
                 u::processId());
        const size_t prefixLength = strlen(prefix);

        Obj  outside("bsls_testtempdirectory", test);
        char outsideFile[Obj::k_PATH_BUFFER_SIZE];
        u::join(outsideFile, sizeof outsideFile, outside.path(), "keep.txt");
        ASSERT(u::writeFile(outsideFile, "keep"));

        char savedPath[Obj::k_PATH_BUFFER_SIZE];
        {
            Obj mX("bsls_testtempdirectory", test);
            Obj mY("bsls_testtempdirectory", test);

            const char *PATH = mX.path();
            const char *BASE = u::baseName(PATH);

            if (verbose) printf("path: %s\n", PATH);

            ASSERTV(PATH, u::isDirectory(PATH));
            ASSERTV(BASE, 0 == strncmp(prefix, BASE, prefixLength));
            ASSERTV(BASE, prefixLength < strlen(BASE));
            ASSERTV(PATH, u::k_SEPARATOR != PATH[strlen(PATH) - 1]);

            ASSERTV(mY.path(), u::isDirectory(mY.path()));
            ASSERTV(mY.path(), 0 != strcmp(PATH, mY.path()));

            char file[Obj::k_PATH_BUFFER_SIZE];
            u::join(file, sizeof file, PATH, "file.txt");
            ASSERT(u::writeFile(file, "file"));

            char nested[Obj::k_PATH_BUFFER_SIZE];
            u::join(nested, sizeof nested, PATH, "nested");
            ASSERT(u::makeDirectory(nested));

            char nestedFile[Obj::k_PATH_BUFFER_SIZE];
            u::join(nestedFile, sizeof nestedFile, nested, "nested.txt");
            ASSERT(u::writeFile(nestedFile, "nested"));

            // Many files, so that removal cannot finish in one pass of a
            // directory stream that skips entries after removals.

            char many[Obj::k_PATH_BUFFER_SIZE];
            u::join(many, sizeof many, PATH, "many");
            ASSERT(u::makeDirectory(many));

            for (int i = 0; i < 500; ++i) {
                char name[32];
                snprintf(name, sizeof name, "file%d.txt", i);

                char manyFile[Obj::k_PATH_BUFFER_SIZE];
                u::join(manyFile, sizeof manyFile, many, name);
                ASSERTV(manyFile, u::writeFile(manyFile, "many"));
            }

#ifndef BSLS_PLATFORM_OS_WINDOWS
            char link[Obj::k_PATH_BUFFER_SIZE];
            u::join(link, sizeof link, PATH, "link");
            ASSERT(0 == symlink(outsideFile, link));

            char dirLink[Obj::k_PATH_BUFFER_SIZE];
            u::join(dirLink, sizeof dirLink, PATH, "dirlink");
            ASSERT(0 == symlink(outside.path(), dirLink));
#endif

            strcpy(savedPath, PATH);
        }

        ASSERTV(savedPath, !u::exists(savedPath));
        ASSERTV(outsideFile, u::exists(outsideFile));
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        // 1. The class is sufficiently functional to enable comprehensive
        //    testing in subsequent test cases.
        //
        // Plan:
        // 1. Create a guard, verify the directory exists, write a file in it,
        //    destroy the guard, and verify the directory is gone.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) printf("\nBREATHING TEST"
                            "\n==============\n");

        char savedPath[Obj::k_PATH_BUFFER_SIZE];
        {
            Obj mX("bsls_testtempdirectory", test);

            ASSERTV(mX.path(), u::isDirectory(mX.path()));

            char file[Obj::k_PATH_BUFFER_SIZE];
            u::join(file, sizeof file, mX.path(), "breathing.txt");
            ASSERT(u::writeFile(file, "breathing"));

            strcpy(savedPath, mX.path());
        }
        ASSERTV(savedPath, !u::exists(savedPath));
      } break;
      default: {
        fprintf(stderr, "WARNING: CASE `%d' NOT FOUND.\n", test);
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        fprintf(stderr, "Error, non-zero test status = %d.\n", testStatus);
    }

    return testStatus;
}

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
