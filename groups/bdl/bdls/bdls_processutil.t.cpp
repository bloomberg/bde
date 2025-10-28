// bdls_processutil.t.cpp                                             -*-C++-*-
#include <bdls_processutil.h>

#include <bdlma_sequentialallocator.h>
#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>

#include <bslim_testutil.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>
#include <bslmt_threadutil.h>
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

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
// typedef bsl::function<void (const char *path)> Func;
//
// CLASS METHODS
// [ 3] int getPathToExecutable(bsl::string *);
// [ 2] int getProcessName(bsl::string *);
// [ 1] int getProcessId();
//-----------------------------------------------------------------------------
// [ 6] USAGE EXAMPLE
// [ 4] CONCERN: RELATIVE PATH, SYMLINKS, SPACES
// [ 1] BREATHING TEST
//-----------------------------------------------------------------------------

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

bslma::TestAllocator uda("u_default");

bsl::size_t npos = bsl::string::npos;

#if defined BSLS_PLATFORM_OS_UNIX
enum { e_UNIX = true };

const char slash = '/';
#else
enum { e_UNIX = false };

const char slash = '\\';
#endif

/// On Unix, return `true` if the file at the specified `path` exists, is
/// not a directory, and is executable (or is a symbolic link to such a
/// file) and `false` otherwise.  On Windows, return `true` if the file
/// exists and is not a directory.
bool isExecutable(const bsl::string& path)
{
#if defined BSLS_PLATFORM_OS_UNIX
    const int executableBits = S_IXUSR | S_IXGRP | S_IXOTH;

    struct stat s;
    int rc = ::stat(path.c_str(), &s);
    return 0 == rc && !(s.st_mode & S_IFDIR) && (s.st_mode & executableBits);
#else
    return FUtil::isRegularFile(path);
#endif
}

bool isRelative(const char *path)
{
    const char C = *path;
    ASSERT('\0' != C);

#if defined BSLS_PLATFORM_OS_UNIX
    return '/' != C;
#else
    return '\\' != C && !(C && ':' == path[1]);
#endif
}

bool isRelative(const bsl::string& path)
{
    return isRelative(path.c_str());
}

int resolvePath(bsl::string *result, const char *origPath)
{
    result->assign(origPath);

#if defined BSLS_PLATFORM_OS_UNIX
#   ifndef PATH_MAX
    enum { PATH_MAX = 4 * 1024 };
#   endif

    bsl::string buf(&uda);
    buf.resize(PATH_MAX);
    const char *ret = ::realpath(origPath, &buf[0]);
    if (!ret) {
        return -1;                                                    // RETURN
    }
    bsl::size_t n = buf.find('\0');
    if (npos == n || PATH_MAX <= n) {
        return -1;                                                    // RETURN
    }
    buf.resize(n);

    *result = buf;
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

    bslma::TestAllocator         ta("test",    veryVeryVeryVerbose);
    bslma::TestAllocator         da("default", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard daGuard(&da);

    // `hostName` is used in multiple test cases in combination with process id
    // to make reliably unique temporary file names.  `$HOSTNAME` is not set in
    // matrix builds, so we can't access the hostname through `getenv`.

#if BSLS_PLATFORM_OS_WINDOWS
    const char * const hostName = "windowshost";
#else
    enum { k_HOST_NAME_BUF_LEN = 256 };
    static char hostName[k_HOST_NAME_BUF_LEN];
    ASSERT(0 == ::gethostname(hostName, k_HOST_NAME_BUF_LEN - 1));
#endif

    bsl::string argv0(argv[0], &ta);
    bsl::string baseArgv0(     &ta);
    {
#if defined BSLS_PLATFORM_OS_WINDOWS
        for (char *pc = &argv0[0]; *pc; ++pc) {
            if ('/' == *pc) {
            *pc = '\\';
            }
        }
#endif
        int rc = PathUtil::getBasename(&baseArgv0, argv0);
        ASSERT(0 == rc);
    }

    switch(test) { case 0:
      case 6: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concern:
        // 1. That the usage example compiles and works.
        //
        // Plan:
        // 1. Run the usage example.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "USAGE EXAMPLE\n"
                             "=============\n";

        bslma::DefaultAllocatorGuard defaultGuard(&ta);     // Strings use
                                                            // default alloc.

// Get the current process ID:
// ```
    const int pid = bdls::ProcessUtil::getProcessId();
// ```
// All calls to `getProcessId` will yield the same value:
// ```
    ASSERT(bdls::ProcessUtil::getProcessId() == pid);
// ```
// Get the current process name:
// ```
    bsl::string processName;
    int rc = bdls::ProcessUtil::getProcessName(&processName);
    if (0 != rc) {
        processName = "unknown";
    }
// ```
// All calls to `getProcessName` will yield the same value.  Note that if
// the call does not succeed, `processNameB` will not be modified.
// ```
    bsl::string processNameB("unknown");
    (void) bdls::ProcessUtil::getProcessName(&processNameB);
//
    ASSERT(processNameB == processName);
// ```
      } break;
      case 5: {
        // --------------------------------------------------------------------
        // LEAKING FILE DESCRIPTORS TEST
        //
        // Concern:
        // 1. Reproduce bug where `getProcessName` is leaking file descriptors.
        //
        // Plan:
        // 1. Call all functions in this component many times and see if we run
        //    out of file descriptors.
        // --------------------------------------------------------------------

        if (verbose) cout << "LEAKING FILE DESCRIPTORS TEST\n"
                             "=============================\n";

#ifdef BSLS_PLATFORM_OS_UNIX
        char tmpFileName[1024];
        bsl::snprintf(tmpFileName,
                      sizeof tmpFileName,
                      "tmp.bdls_processutil.case4.%s.%d.txt",
                      hostName,
                      Obj::getProcessId());
        FUtil::remove(tmpFileName);

        enum { k_ITERATIONS  = 30,
               k_MAX_FILE_ID = k_ITERATIONS / 2 };
#else
        enum { k_ITERATIONS  = 520 };
#endif

        const int firstId = Obj::getProcessId();
        bsl::string firstName(&ta);
        ASSERT(0 == Obj::getProcessName(&firstName));
        bsl::string firstExecName(&ta);
        ASSERT(0 == Obj::getPathToExecutable(&firstExecName));

        for (int i = 0; i < k_ITERATIONS; ++i) {
            ASSERT(Obj::getProcessId() == firstId);
            bsl::string name(&ta);
            ASSERT(0 == Obj::getProcessName(&name));
            ASSERT(name == firstName);
            bsl::string execName(&ta);
            ASSERT(0 == Obj::getPathToExecutable(&execName));
            ASSERT(execName == firstExecName);

#ifdef BSLS_PLATFORM_OS_UNIX
            FUtil::FileDescriptor fd = FUtil::open(tmpFileName,
                                 FUtil::e_OPEN_OR_CREATE, FUtil::e_READ_WRITE);
            LOOP2_ASSERT(i, fd, fd < k_MAX_FILE_ID);
            FUtil::close(fd);
            FUtil::remove(tmpFileName);
#endif
        }
      }  break;
      case 4: {
        // --------------------------------------------------------------------
        // CONCERN: RELATIVE PATH, SYMLINKS, SPACES
        //
        // This test case is concerned with difficult cases for
        // `getProcessName` and `getExecutableName`.  In order to test
        // situations where, for example, an executable name has a space, this
        // test case uses `system` to run a shell script that copies the test
        // driver to a new file, and runs case -1, and captures the result.
        // Case -1 contains the body of the test performed.
        //
        // Concerns:
        // 1. That `getPathToExecutable` will reliably deliver a path through
        //    which the executable can be accessed, even under difficult
        //    conditions.
        //    - The executable is invoked with a relative path
        //    - When the working directory has been changed since task startup.
        //    - When the executable name contains spaces.
        //
        // Plan:
        // 1. Create a temp file that is to be a unix or DOS shell script.  In
        //    it:
        //    - Create a test directory.
        //    - Use `getPathToExecutable` to get the name of the currently
        //      running process (the test driver).  Copy this executable into
        //      a file in the test directory.  If we expect to be able to cope
        //      with spaces in the file name, choose a file name with spaces.
        //    - chdir into that directory.
        //    - Run the previous test case (which tests `getPathToExecutable`,
        //      with the same verbosity flags passed to this test case.
        //
        // 2. Use `system` to run the shell script and observe the return
        //    value, which will indicate whether that test case passed, and
        //    incorporate that value into the value of `testStatus` for this
        //    test driver.
        //
        // Testing:
        //   CONCERN: RELATIVE PATH, SYMLINKS, SPACES
        // --------------------------------------------------------------------

        if (verbose) cout << "CONCERN: RELATIVE PATH, SYMLINKS, SPACES\n"
                             "========================================\n";

        int rc;
        char directoryName[1024];
        bsl::snprintf(directoryName,
                      sizeof directoryName,
                      "tmp.bdls_processutil.t.case%d.%s.%d.dir",
                      test,
                      hostName,
                      Obj::getProcessId());

        {
            // `FUtil::remove` uses the default allocator on directories, and
            // `FUtil::createDirectories` uses the default allocator.

            bslma::DefaultAllocatorGuard guard(&ta);
            (void) FUtil::remove(directoryName, true);
            ASSERT(!FUtil::exists(directoryName));
            rc = FUtil::createDirectories(directoryName, true);
            ASSERT(0 == rc);
        }

#if defined BSLS_PLATFORM_OS_CYGWIN
        // This platform can't handle spaces in the executable name.

        const char * const executableName = "case4.exec.t";
        const char * const linkName       = "case4.link.t";
#elif defined BSLS_PLATFORM_OS_WINDOWS
        const char * const executableName = "case4.exec.a  b  c.t.exe";
#else
        const char * const executableName = "case4.exec.a  b  c.t";
        const char * const linkName       = "case4.link.a  b  c.t";
#endif

        bsl::string copiedExecutablePath(directoryName, &ta);    // copy destination of exec
        copiedExecutablePath += u::slash;
        copiedExecutablePath += executableName;

        bsl::string testDriverPath(&ta);
        ASSERT(0 == Obj::getPathToExecutable(&testDriverPath));
        if (veryVerbose) P(testDriverPath);

#if defined BSLS_PLATFORM_OS_UNIX
        bsl::string scriptName(directoryName, &ta);
        scriptName += "/case4.script.sh";

        const char *path = copiedExecutablePath.c_str();

        FILE *fp = bsl::fopen(scriptName.c_str(), "w");
        ASSERT(fp);
        bsl::fprintf(fp, ":\n");
        bsl::fprintf(fp, "cp '%s' '%s'\n", testDriverPath.c_str(), path);
        bsl::fprintf(fp, "chmod a+rwx '%s'\n", path);
        bsl::fprintf(fp, "cd %s >/dev/null 2>&1\n", directoryName);
        bsl::fprintf(fp, "ln -s '%s' '%s'\n", executableName, linkName);
        bsl::fprintf(fp, "%s", veryVerbose ? "pwd; ls -l\n" : "");
        bsl::fprintf(fp, "exec './%s' -1 %s%s%s%s\n",
                                              linkName,
                                              verbose ? " v" : "",
                                              veryVerbose ? " v" : "",
                                              veryVeryVerbose ? " v" : "",
                                              veryVeryVeryVerbose ? " v" : "");
        rc = bsl::fclose(fp);
        ASSERT(0 == rc);

        scriptName.insert(bsl::size_t(0), veryVerbose ? "sh -v "
                                                      : "sh ");

#else   // Windows

        bsl::string scriptName(directoryName, &ta);
        scriptName += "\\case4.script.bat";
        FILE *fp = bsl::fopen(scriptName.c_str(), "w");

        const char *path = copiedExecutablePath.c_str();

        ASSERT(fp);
        bsl::fprintf(fp, "%s", !veryVerbose ?  "@echo off\n" : "");
        bsl::fprintf(fp, "copy \"%s\" \"%s\"\n", testDriverPath.c_str(), path);
        bsl::fprintf(fp, "cd %s\n", directoryName);
        bsl::fprintf(fp, "%s", veryVerbose ? "echo %cd%\ndir /o\n" : "");
        bsl::fprintf(fp, "\".\\%s\" -1 %s%s%s%s\n",
                                              executableName,
                                              verbose ? " v" : "",
                                              veryVerbose ? " v" : "",
                                              veryVeryVerbose ? " v" : "",
                                              veryVeryVeryVerbose ? " v" : "");
        rc = bsl::fclose(fp);
        ASSERT(0 == rc);

#endif

        rc = bsl::system(scriptName.c_str());
        ASSERTV(rc, 0 == rc);
        if (0 < rc) {
            testStatus = bsl::min(101, testStatus + rc);
        }

        ASSERT(FUtil::isDirectory(directoryName));

        if (u::e_UNIX) {
            rc = FUtil::remove(directoryName, true);
        } else {
            // Windows needs a few seconds after the script finishes to be
            // allowed to delete the script and the executable at
            // `copiedExecutablePath` and the directory `directoryName`
            // containing them.
            bslmt::ThreadUtil::microSleep(0, 5);  // 5 seconds
            for (int removeTry = 0; removeTry < 5; ++removeTry) {
                bslmt::ThreadUtil::microSleep(0, 1);
                rc = FUtil::remove(directoryName, true);
                if (0 == rc) break;
            }
        }

        if (0 == rc) {
            ASSERT(!FUtil::exists(directoryName));
            ASSERT(!FUtil::exists(copiedExecutablePath));
        } else {
            if (verbose) {
                cout << "Failed to clean up " << directoryName << endl;
            }
        }
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING `getPathToExecutable`
        //
        // Concerns:
        // 1. Ensure that `getPathToExecutable` can find the executable, before
        //    and after changing the current dirrectory.  Note that some
        //    platforms will not be able to cope with this.
        //
        // Plan:
        // 1. Determine if `argv[0]` is relative.  This is still a useful test
        //    if `argv[0]` is absolute, but it's a better test if it's
        //    relative, so the next test case runs this test case with a
        //    relative `argv[0]`.
        //
        // 2. Get the current working directory.
        //
        // 3. Read the executable name with `getPathToExecutable`.  Verify that
        //    it's a valid, executable file.
        //
        // 4. Change to the parent of the current directory.
        //
        // 5. Determine, based upon the platform and whether the directory
        //    "/proc" is available, whether we expect `getPathToExecutable` to
        //    now be able to give us a usable path to the executable, and set
        //    the `bool` `expFindExec` accordingly.
        //
        // 6. Call `getPathToExecutable` again.
        //
        // 7. If we expect the post-chdir tests for the current platform, check
        //    whether the return code was 0, whether a file exists at the
        //    returned path, whether the file at the returned path was
        //    executable, and whether the observed size of the file at the
        //    returned path matches that taken of `argv[0]` before changing
        //    directories.
        //
        // 8. If we don't expect the post-chdir tests to pass and they do, emit
        //    traces accordingly but don't fail.
        //
        // Testing:
        //   int getPathToExecutable(bsl::string *);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `getPathToExecutable`\n"
                             "===========================\n";

        ASSERTV(argv0, FUtil::exists(argv0));

        const bsls::Types::Int64 execSize = FUtil::getFileSize(argv0);
        ASSERTV(execSize, 8 * 1024 < execSize);

        // Detect relative path on Unix or Windows

        const bool argv0IsRelative = u::isRelative(argv0);

        bsl::string origCwd(&ta);
        int rc = FUtil::getWorkingDirectory(&origCwd);
        ASSERT(0 == rc);
        ASSERT(3 < origCwd.length());    // not root

        bsl::string processNameBeforeCd;;
        int gpnbRc = Obj::getProcessName(&processNameBeforeCd);
        ASSERTV(gpnbRc, processNameBeforeCd, 0 == gpnbRc);
        ASSERT(!processNameBeforeCd.empty());

        bsl::string executableNameBeforeCd;
        rc = Obj::getPathToExecutable(&executableNameBeforeCd);
        ASSERT(0 == rc);

        ASSERTV(executableNameBeforeCd, FUtil::exists(executableNameBeforeCd));
        ASSERTV(executableNameBeforeCd,
                u::isExecutable(executableNameBeforeCd));
        ASSERTV(execSize,
                FUtil::getFileSize(executableNameBeforeCd),
                execSize == FUtil::getFileSize(executableNameBeforeCd));

        rc = FUtil::setWorkingDirectory("..");
        ASSERT(0 == rc);

        bsl::string cwd;
        rc = FUtil::getWorkingDirectory(&cwd);
        ASSERT(0 == rc);
        ASSERTV(origCwd, cwd, origCwd != cwd);

        ASSERTV(cwd, argv0, FUtil::exists(argv0), argv0IsRelative,
                FUtil::exists(argv0) == !argv0IsRelative);

        const bool procExists = FUtil::isDirectory("/proc", true);

#if defined BSLS_PLATFORM_OS_AIX || defined BSLS_PLATFORM_OS_LINUX
        const bool expFindExec = procExists || !argv0IsRelative;
#elif defined BSLS_PLATFORM_OS_CYGWIN
        const bool expFindExec = !argv0IsRelative;
#else
        const bool expFindExec = true;
#endif

        bsl::string executableName;
        int gpteRc = Obj::getPathToExecutable(&executableName);
        ASSERTV(gpteRc, executableName, 0 == gpteRc);
        ASSERT(!executableName.empty());
        ASSERT(bsl::strlen(executableName.c_str()) == executableName.length());
        const bool executableNameIsRelative = u::isRelative(executableName);
        const bool executableNameExists = FUtil::exists(executableName);
        const bool executableNameIsExec = u::isExecutable(executableName);
        const bool sizeMatches =
            execSize == FUtil::getFileSize(executableName);

        bsl::string processNameAfterCd("meow", &ta);
        int gpnRc = Obj::getProcessName(&processNameAfterCd);
        ASSERTV(gpnRc, processNameAfterCd, 0 == gpnRc);
        ASSERT(!processNameAfterCd.empty());

        ASSERTV(processNameAfterCd, baseArgv0, u::npos !=
                processNameAfterCd.find(baseArgv0));

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_DARWIN)
        // On windows and MacOS getProcessName and getExecutableName return
        // the same value.

        ASSERTV(executableName, processNameAfterCd,
                executableName == processNameAfterCd);
#endif

        if (verbose) {
            P_(origCwd); P_(cwd); P(procExists);
            P_(processNameBeforeCd);      P(processNameAfterCd);
            P_(executableNameBeforeCd);   P(executableName);

            P_(executableNameIsRelative);
            P_(executableNameExists);
            P(executableNameIsExec);

            P_(sizeMatches); P(processNameAfterCd);
        }

        if (expFindExec) {
            ASSERTV(executableName, expFindExec, gpteRc, 0 == gpteRc);
            ASSERTV(executableName, !executableNameIsRelative);
            ASSERTV(executableName, expFindExec, executableNameExists,
                    executableNameExists);
            ASSERTV(executableName, expFindExec, executableNameIsExec,
                    executableNameIsExec);
            ASSERTV(execSize, sizeMatches);
        }
        else {
            // If we did better than expected, issue warning, but not error.

            if (0 == gpteRc)
                cout << "Unexpected success rc!\n";

            if (!executableNameIsRelative)
                cout << "Unexpectedly not relative!\n";

            if (executableNameExists)
                cout << "Unexpectedly existed!\n";

            if (executableNameIsExec)
                cout << "Unexpectedly executable!\n";

            if (sizeMatches)
                cout << "Unexpected matching size!\n";
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING `getProcessName`
        //
        // Concerns:
        // 1. That `getProcessName` always succeeds.
        //
        // 2. That the component name (which will be part of the executable
        //    file name under normal testing) is part of the found process
        //    name.
        //
        // Plan:
        // 1. Call `getProcessName`.
        //
        // 2. Verify that the value returned includes the component name as
        //    a substring.
        //
        // Testing:
        //   int getProcessName(bsl::string *);
        // --------------------------------------------------------------------

        if (verbose) cout << "TESTING `getProcessName`\n"
                             "========================\n";

        bsl::string name(&ta);
        int rc = Obj::getProcessName(&name);
        ASSERTV(rc, 0 == rc);

        ASSERTV(name, bsl::string::npos != name.find(baseArgv0));

        if (verbose) P(name);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Testing:
        //   int getProcessId();
        // --------------------------------------------------------------------

        if (verbose) cout << "BREATHING TEST\n"
                             "==============\n";

        const int pid = bdls::ProcessUtil::getProcessId();
        ASSERT(0 != pid);

#if defined BSLS_PLATFORM_OS_UNIX
        ASSERTV(pid, 0 < pid);
#endif

        if (verbose) P(argv0);
        const bool argv0IsRelative = u::isRelative(argv0.c_str());

        ASSERTV(argv0, FUtil::exists(argv0));
        bsl::string resArgv0(&ta);
        int rc = u::resolvePath(&resArgv0, argv0.c_str());
        ASSERT(0 == rc);

        bsl::string name(&ta);
        ASSERT(0 == bdls::ProcessUtil::getProcessName(&name));
        ASSERTV(name, FUtil::exists(name));
        const bool nameIsRelative = u::isRelative(name);

        bsl::string resName(&ta);
        rc = u::resolvePath(&resName, name);
        ASSERT(0 == rc);

        ASSERTV(resArgv0, resName, name, argv0,
                     argv0IsRelative != nameIsRelative || resArgv0 == resName);

        bsl::string baseArgv0(&ta);
        rc = PathUtil::getBasename(&baseArgv0, resArgv0);
        ASSERT(0 == rc);

        bsl::string basename(&ta);
        rc = PathUtil::getBasename(&basename, resName);
        ASSERT(0 == rc);

        ASSERTV(resArgv0, resName, baseArgv0, basename, baseArgv0 == basename);

        if (verbose || 0 != testStatus) {
            P(pid);
            P(argv[0]);
            P(argv0);
            P(name);
            P(resArgv0);
            P(resName);
        }
      } break;
      case -1: {
        // --------------------------------------------------------------------
        // CONCERN: RELATIVE PATH, SYMLINKS, SPACES (SUB TEST)
        //
        // This test case is executed via `system` from test case 4 to verify
        // that the renamed test-driver executable works correct.  See case
        // 4 for concerns and plan.
        // --------------------------------------------------------------------

        if (verbose)
            cout << "CONCERN: RELATIVE PATH, SYMLINKS, SPACES (SUB TEST)\n"
                 << "===================================================\n";

        int rc;

        ASSERTV(argv0, FUtil::exists(argv0));
        const bsls::Types::Int64 execSize = FUtil::getFileSize(argv0);
        const bool argv0IsRelative = u::isRelative(argv0);
        const bool procExists = FUtil::isDirectory("/proc", true);
        (void) argv0IsRelative;
        (void) procExists;

        rc = FUtil::setWorkingDirectory("..");
        ASSERT(0 == rc);

        bsl::string cwd;
        rc = FUtil::getWorkingDirectory(&cwd);
        ASSERT(0 == rc);


        bsl::string executableName;
        int gpteRc = Obj::getPathToExecutable(&executableName);
        ASSERTV(gpteRc, executableName, 0 == gpteRc);
        ASSERT(!executableName.empty());
        ASSERT(bsl::strlen(executableName.c_str()) == executableName.length());

        const bool executableNameIsRelative = u::isRelative(executableName);
        const bool executableNameExists = FUtil::exists(executableName);
        const bool executableNameIsExec = u::isExecutable(executableName);
        const bool sizeMatches =
            execSize == FUtil::getFileSize(executableName);

#if defined BSLS_PLATFORM_OS_AIX || defined BSLS_PLATFORM_OS_LINUX
        const bool expectSuccess = procExists || !argv0IsRelative;
#elif defined BSLS_PLATFORM_OS_CYGWIN
        const bool expectSuccess = !argv0IsRelative;
#else
        const bool expectSuccess = true;
#endif
        if (expectSuccess) {
            ASSERTV(executableName, expectSuccess, gpteRc, 0 == gpteRc);
            ASSERTV(executableName, !executableNameIsRelative);
            ASSERTV(executableName, expectSuccess, executableNameExists,
                    executableNameExists);
            ASSERTV(executableName, expectSuccess, executableNameIsExec,
                    executableNameIsExec);
            ASSERTV(execSize, sizeMatches);
        }


        bsl::string processName;
        int gpnRc = Obj::getProcessName(&processName);
        ASSERTV(gpnRc, processName, 0 == gpnRc);
        ASSERT(!processName.empty());

        // Whether the process named returned is exected to be the name of a
        // symlink.  This is `true` on most UNIX platforms, except MacOS.
        // This is false on Windows (no symlinks) and Darwin (where the
        // proc_getpidpath function returns the path of the process the
        // symlink referred to).



        const char *expectedPathElement =
#if defined(BSLS_PLATFORM_OS_UNIX) && !defined(BSLS_PLATFORM_OS_DARWIN)
            "case4.link";   // name of the symlink
#else
            "case4.exec";   // name of the executable
#endif

        ASSERTV(processName, expectedPathElement,
               u::npos != processName.find(expectedPathElement));

#if defined(BSLS_PLATFORM_OS_WINDOWS) || defined(BSLS_PLATFORM_OS_DARWIN)
        // On windows and MacOS getProcessName and getExecutableName return
        // the same value.

        ASSERTV(executableName, processName, executableName == processName);
#endif

      } break;
      case -2: {
        // --------------------------------------------------------------------
        // MANUAL TEST: LONG PROCESS NAME (> 128 CHARACTERS)
        //
        // Concern:
        // 1. Reproduce bug where `getProcessName` truncates the process name
        //    without returning an error (negative value).
        //
        // Plan:
        // 1. Rename the test driver by hand.  Then run it with -1 test case
        //    and see if `getProcessName` reports the same as `argv[0]`.
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
