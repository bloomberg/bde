// bdls_processutil.t.cpp                                             -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_processutil.h>

#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>

#include <bslim_testutil.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bsls_platform.h>

#include <bsl_algorithm.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

#if defined BSLS_PLATFORM_OS_UNIX
# include <sys/types.h>
# include <sys/stat.h>
# include <limits.h>
# include <unistd.h>
#else
# include <direct.h>
#endif

using namespace BloombergLP;
using bsl::cout;
using bsl::cerr;
using bsl::endl;
using bsl::flush;

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

// ============================================================================
//                 GLOBAL HELPER TYPES & CLASSES FOR TESTING
// ----------------------------------------------------------------------------

typedef bdls::FilesystemUtil  FUtil;
typedef bdls::PathUtil        PathUtil;
typedef bdls::ProcessUtil     Obj;

namespace {
namespace u {

bool isExecutable(const bsl::string& path)
{
#if defined BSLS_PLATFORM_OS_UNIX
    const int executableBits = S_IXUSR | S_IXGRP | S_IXOTH;

    struct stat s;
    int rc = ::lstat(path.c_str(), &s);
    return 0 == rc && !S_ISLNK(s.st_mode) && (s.st_mode & executableBits);
#else
    return FUtil::isRegularFile(path);
#endif
}

bool isRelative(const bsl::string& path)
{
    ASSERT(!path.empty());
    const char C = path.empty() ? 0 : path[0];

    return '/' != C && '\\' != C && bsl::string::npos == path.find(':');
}

int resolvePath(bsl::string *result, const char *origPath)
{
#if defined BSLS_PLATFORM_OS_UNIX
#   ifndef PATH_MAX
    enum { PATH_MAX = 4 * 1024 };
#   endif

    result->resize(PATH_MAX);
    const char *ret = ::realpath(origPath, &(*result)[0]);
    if (!ret) {
        return -1;                                                    // RETURN
    }
    bsl::size_t n = result->find('\0');
    if (bsl::string::npos == n || PATH_MAX <= n) {
        return -1;                                                    // RETURN
    }
    result->resize(n);
#else
    result->assign(origPath);
#endif

    return 0;
}

inline
int resolvePath(bsl::string *result, const bsl::string& origPath)
{
    return resolvePath(result, origPath.c_str());
}

}  // close namespace u
}  // close unnamed namespace

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // The component under test and the test driver may use the default
    // allocator, but must not leak from it.

    bslma::TestAllocator ta("test",    veryVeryVeryVerbose);
    bslma::TestAllocator da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultGuard(&da);

    switch(test) { case 0:
      case 4: {
        // --------------------------------------------------------------------
        // 'getExecutablePath' RELATIVE ARGV[0] TEST
        //
        // Concerns:
        //: 1 Run test case 2 under circumstances where the executable is
        //:   specified by a relative path, with spaces in it if we expect that
        //:   the platform can handle it, and ensure that the test passes where
        //:   we expect it to.
        //
        // Plan:
        //: 1 'argv[0]' is always an absolute path on Windows, so there is no
        //:   point in running this test there.
        //:
        //: 2 Create a temp file that is to be a unix shell script.  In it:
        //:   o Create a test directory.
        //:   o Copy 'argv[0]' into a file in the test directory.  If we expect
        //:     to be able to cope with spaces in the file name, choose a file
        //:     name with spaces.
        //:   o chdir into that directory.
        //:   o Run test case 2, with the same verbosity flags passed to this
        //:     test case.  Do this via an 'exec' so that the return value
        //:     returned by the test case will be returned by the shell script.
        //:
        //: 3 Use 'system' to run the shell script and observe the return
        //:   value, which will indicate whether TC 2 passed.
        // --------------------------------------------------------------------

#if defined BSLS_PLATFORM_OS_UNIX
        if (verbose) cout << "'getExecutablePath' RELATIVE ARGV[0] TEST\n"
                             "=========================================\n";

        char dirNameBuf[1024];
        bsl::sprintf(dirNameBuf,"tmp.bdls_processutil.t.case4.%s.%d.dir",
                                 bsl::getenv("HOSTNAME"), Obj::getProcessId());
        const bsl::string dirName(dirNameBuf, &ta);

        (void) FUtil::remove(dirName, true);
        ASSERT(!FUtil::exists(dirName));
        int rc = FUtil::createDirectories(dirName, true);
        ASSERT(0 == rc);

#if defined BSLS_PLATFORM_OS_CYGWIN || defined BSLS_PLATFORM_OS_HPUX
        // These two platforms can't deal with spaces in the executable name.

        const bsl::string execName("case4.exec.t", &ta);
#else
        const bsl::string execName("case4.exec.a b   c.t", &ta);
#endif
        bsl::string cpDst(dirName, &ta);
        cpDst += '/';
        cpDst += execName;

        const bsl::string scriptName(dirName + "/case4script.sh", &ta);
        FILE *fp = bsl::fopen(scriptName.c_str(), "w");
        ASSERT(fp);
        bsl::fprintf(fp, ":\n");
        bsl::fprintf(fp, "cp '%s' '%s'\n", argv[0], cpDst.c_str());
        bsl::fprintf(fp, "chmod a+rwx '%s'\n", cpDst.c_str());
        bsl::fprintf(fp, "cd %s >/dev/null 2>&1\n", dirName.c_str());
        bsl::fprintf(fp, "exec '%s' 2%s%s%s%s\n",
                                              execName.c_str(),
                                              verbose ? " v" : "",
                                              veryVerbose ? " v" : "",
                                              veryVeryVerbose ? " v" : "",
                                              veryVeryVeryVerbose ? " v" : "");
        rc = bsl::fclose(fp);
        ASSERT(0 == rc);

        bsl::string systemStr("bash ", &ta);
        if (veryVerbose) {
            systemStr += "-v ";
        }
        systemStr += scriptName;
        rc = bsl::system(systemStr.c_str());
        if (0 != rc) {
            ASSERTV(rc, 0 < rc);
            if (0 < rc) {
                testStatus = bsl::min(101, testStatus + rc);
            }
        }

        rc = FUtil::remove(dirName, true);
        ASSERT(0 == rc);
        ASSERT(!FUtil::exists(dirName));
#else
        if (verbose) cout << "Test 4 skipped on Windows.\n";
#endif
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // LEAKING FILE DESCRIPTORS TEST
        //
        // Concern:
        //: 1 Reproduce bug where 'getProcessName' is leaking file descriptors.
        //
        // Plan:
        //: 1 Call it many times and see if we run out of file descriptors.
        // --------------------------------------------------------------------

        if (verbose) cout << "LEAKING FILE DESCRIPTORS TEST\n"
                             "=============================\n";

        const int firstId = Obj::getProcessId();
#ifdef BSLS_PLATFORM_OS_UNIX
        ASSERT(firstId > 0);

        const char *tmpFileName = "tmp.bdls_processutil.case3.eraseMe.txt";
        FUtil::remove(tmpFileName);
#endif
        bsl::string firstName;
        ASSERT(0 == Obj::getProcessName(&firstName));

        for (int i = 0; i < 100; ++i) {
            ASSERT(Obj::getProcessId() == firstId);
            bsl::string name;
            ASSERT(0 == Obj::getProcessName(&name));
            ASSERT(name == firstName);

#ifdef BSLS_PLATFORM_OS_UNIX
            FUtil::FileDescriptor fd = FUtil::open(tmpFileName,
                                 FUtil::e_OPEN_OR_CREATE, FUtil::e_READ_WRITE);
            LOOP2_ASSERT(i, fd, fd < 40);
            FUtil::close(fd);
            FUtil::remove(tmpFileName);
#endif
        }
      }  break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'getExecutablePath'
        //
        // Concerns:
        //: 1 Ensure that 'getExecutablePath' can find the executable, before
        //:   and after changing the current dirrectory.  Note that some
        //:   platforms will not be able to cope with this.
        //
        // Plan:
        //: 1 Determine if 'argv[0]' is relative.  This is still a useful
        //:   test if 'argv[0]' is absolute, but it's a better test if it's
        //:   relative, so test case 4 runs this test case with a relative
        //:   'argv[0]'.
        //:
        //: 2 Get the current working directory.
        //:
        //: 3 Read the executable name with 'getExecutablePath'.  Verify that
        //:   it's a valid, executable file.
        //:
        //: 4 Change to the parent of the current directory.
        //:
        //: 5 Determine, based upon the platform and whether the directory
        //:   "/proc" is available, whether we expect 'getExecutablePath' to
        //:   now be able to give us a usable path to the executable, and set
        //:   the 'bool' 'expFindExec' accordingly.
        //:
        //: 6 Call 'getExecutablePath' again.
        //:
        //: 7 If we expect the post-chdir tests for the current platform, check
        //:   whether the return code was 0, whether a file exists at the
        //:   returned path, whether the file at the returned path was
        //:   executable, and whether the observed size of the file at the
        //:   returned path matches that taken of 'argv[0]' before changing
        //:   directories.
        //:
        //: 8 If we don't expect the post-chdir tests to pass and they do, emit
        //:   traces accordingly but don't fail.
        //
        // Testing:
        //   int getExecutablePath(bsl::string *);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING 'getExecutablePath'\n"
                             "===========================\n";

        if (verbose) P(argv[0]);
        ASSERTV(argv[0], FUtil::exists(argv[0]));
        const bsls::Types::Int64 execSize = FUtil::getFileSize(argv[0]);
        ASSERTV(execSize, 8 * 1024 < execSize);
        if (verbose) P(execSize);

        // Detect relative path on Unix or Windows

        const bool argv0IsRelative = u::isRelative(argv[0]);
#if defined BSLS_PLATFORM_OS_WINDOWS
        ASSERT(!argv0IsRelative);    // 'arvg[0]' is always translated to an
                                     // absolute path on Windows.
#endif

        bsl::string origCwd(&ta);
        int rc = FUtil::getWorkingDirectory(&origCwd);
        ASSERT(0 == rc);
        ASSERT("/" != origCwd && 3 < origCwd.length());    // not root
        if (verbose) P(origCwd);

        bsl::string execName("meow", &ta);
        rc = Obj::getExecutablePath(&execName);
        ASSERT(0 == rc);
        if (verbose) cout << "Before 'cd ..': exec: '" << execName << "'\n";

        ASSERTV(execName, FUtil::exists(execName));
        ASSERTV(execName, u::isExecutable(execName));
        ASSERTV(execSize, FUtil::getFileSize(execName),
                                     execSize == FUtil::getFileSize(execName));

        rc = FUtil::setWorkingDirectory("..");
        ASSERT(0 == rc);

        bsl::string cwd(&ta);
        rc = FUtil::getWorkingDirectory(&cwd);
        ASSERT(0 == rc);
        if (verbose) P(cwd);
        ASSERTV(origCwd, cwd, origCwd != cwd);

        ASSERTV(cwd, argv[0], FUtil::exists(argv[0]), argv0IsRelative,
                                   FUtil::exists(argv[0]) == !argv0IsRelative);

        const bool procExists = FUtil::isDirectory("/proc", true);
        if (verbose) P(procExists);

#if defined BSLS_PLATFORM_OS_AIX || defined BSLS_PLATFORM_OS_LINUX
        const bool expFindExec = procExists || !argv0IsRelative;
#elif defined BSLS_PLATFORM_OS_CYGWIN || defined BSLS_PLATFORM_OS_HPUX
        const bool expFindExec = !argv0IsRelative;
#else
        const bool expFindExec = true;
#endif
        if (verbose) P(expFindExec);

        execName = "woof";
        int geprc = Obj::getExecutablePath(&execName);
        ASSERT(!execName.empty());
        const bool execNameIsRelative = u::isRelative(execName);
        const bool execNameExists = FUtil::exists(execName);
        const bool execNameIsExec = u::isExecutable(execName);
        const bool sizeMatches = execSize == FUtil::getFileSize(execName);

        if (verbose) cout << "After  'cd ..': exec: '" << execName <<
                       "' is " << (execNameIsExec ? "" : "not ") << "valid.\n";

        if (verbose) {
            P_(execNameIsRelative);
            P_(execNameExists);
            P_(execNameIsExec);
            P(sizeMatches);
        }

        if (expFindExec) {
            ASSERTV(execName, expFindExec, geprc, 0 == geprc);
            ASSERTV(execName, !execNameIsRelative);
            ASSERTV(execName, expFindExec, execNameExists, execNameExists);
            ASSERTV(execName, expFindExec, execNameIsExec, execNameIsExec);
            ASSERTV(execSize, sizeMatches);
        }
        else {
            if (0 == geprc)          cout << "Unexpected success rc!\n";
            if (!execNameIsRelative) cout << "Unexpectedly not relative!\n";
            if (execNameExists)      cout << "Unexpectedly existed!\n";
            if (execNameIsExec)      cout << "Unexpectedly executable!\n";
            if (sizeMatches)         cout << "Unexpected matching size!\n";
        }
      } break;
      case 1: {
        // ------------------------------------------------------------
        // BREATHING TEST
        //
        // Testing:
        //   int getProcessId();
        //   int getProcessName(bsl::string *);
        // ------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        ASSERT(0 != bdls::ProcessUtil::getProcessId());

        if (verbose) P(argv[0]);
        const bool argv0IsRelative = u::isRelative(argv[0]);

        ASSERTV(argv[0], FUtil::exists(argv[0]));
        bsl::string resArgv0(&ta);
        int rc = u::resolvePath(&resArgv0, argv[0]);
        ASSERT(0 == rc);

        bsl::string name(&ta);
        ASSERT(0 == bdls::ProcessUtil::getProcessName(&name));
        ASSERTV(name, FUtil::exists(name));
        const bool nameIsRelative = u::isRelative(name);

        bsl::string resName(&ta);
        rc = u::resolvePath(&resName, name);
        ASSERT(0 == rc);

        ASSERTV(resArgv0, resName, name, argv[0],
                     argv0IsRelative != nameIsRelative || resArgv0 == resName);

        bsl::string baseArgv0(&ta);
        rc = PathUtil::getBasename(&baseArgv0, resArgv0);
        ASSERT(0 == rc);

        bsl::string basename(&ta);
        rc = PathUtil::getBasename(&basename, resName);
        ASSERT(0 == rc);

        ASSERTV(resArgv0, resName, baseArgv0, basename, baseArgv0 == basename);

        if (verbose) {
            P(argv[0]);
            P(name);
            P(resArgv0);
            P(resName);
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // MANUAL TEST: LONG PROCESS NAME (> 128 CHARACTERS)
        //
        // Concern:
        //: 1 Reproduce bug where 'getProcessName' truncates the process name
        //:   without returning an error (negative value).
        //
        // Plan:
        //: 1 Rename the test driver by hand.  Then run it with -1 test case
        //:   and see if 'getProcessName' reports the same as 'argv[0]'.
        // --------------------------------------------------------------------

        if (verbose) cout <<
                         "MANUAL TEST: LONG PROCESS NAME (> 128 CHARACTERS)\n"
                         "=================================================\n";

        bsl::string name;
        ASSERT(0 == bdls::ProcessUtil::getProcessName(&name));
        ASSERT(name == argv[0]);
        P(name);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "."
             << endl;
    }
    return testStatus;
}

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
