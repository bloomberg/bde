// balb_filecleanerutil.t.cpp                                         -*-C++-*-
#include <balb_filecleanerutil.h>

#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>

#include <bdlt_datetime.h>
#include <bdlt_epochutil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_threadutil.h>

#include <bsls_asserttest.h>
#include <bsls_log.h>
#include <bsls_platform.h>
#include <bsls_timeinterval.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>
#include <bsl_iostream.h>
#include <bsl_fstream.h>
#include <bsl_string.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//-----------------------------------------------------------------------------
// CLASS METHODS
// [ 2] void removeFiles(const balb::FileCleanerConfiguration& config);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 3] USAGE EXAMPLE

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
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

#define ASSERT_FAIL_RAW(EXPR)  BSLS_ASSERTTEST_ASSERT_FAIL_RAW(EXPR)

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef balb::FileCleanerUtil          Obj;
typedef balb::FileCleanerConfiguration ObjConfig;

// ============================================================================
//                  GLOBAL HELPER FUNCTIONS FOR TESTING
// ----------------------------------------------------------------------------

namespace {

class TempDirectoryGuard {
    // This class implements a scoped temporary directory guard.  The guard
    // tries to create a temporary directory in the system-wide temp directory
    // and falls back to the current directory.

    // DATA
    bsl::string       d_dirName;      // path to the created directory
    bslma::Allocator *d_allocator_p;  // memory allocator (held, not owned)

    // NOT IMPLEMENTED
    TempDirectoryGuard(const TempDirectoryGuard&);
    TempDirectoryGuard& operator=(const TempDirectoryGuard&);

  public:
    // TRAITS
    BSLMF_NESTED_TRAIT_DECLARATION(TempDirectoryGuard,
                                   bslma::UsesBslmaAllocator);

    // CREATORS
    explicit TempDirectoryGuard(bslma::Allocator *basicAllocator = 0)
        // Create temporary directory in the system-wide temp or current
        // directory.  Optionally specify a 'basicAllocator' used to supply
        // memory.  If 'basicAllocator' is 0, the currently installed default
        // allocator is used.
    : d_dirName(bslma::Default::allocator(basicAllocator))
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        bsl::string tmpPath(d_allocator_p);
#ifdef BSLS_PLATFORM_OS_WINDOWS
        char tmpPathBuf[MAX_PATH];
        GetTempPath(MAX_PATH, tmpPathBuf);
        tmpPath.assign(tmpPathBuf);
#else
        const char *envTmpPath = bsl::getenv("TMPDIR");
        if (envTmpPath) {
            tmpPath.assign(envTmpPath);
        }
#endif

        int res = bdls::PathUtil::appendIfValid(&tmpPath, "ball_");
        ASSERTV(tmpPath, 0 == res);

        res = bdls::FilesystemUtil::createTemporaryDirectory(&d_dirName,
                                                             tmpPath);
        ASSERTV(tmpPath, 0 == res);
    }

    ~TempDirectoryGuard()
        // Destroy this object and remove the temporary directory (recursively)
        // created at construction.
    {
        bdls::FilesystemUtil::remove(d_dirName, true);
    }

    // ACCESSORS
    const bsl::string& getTempDirName() const
        // Return a 'const' reference to the name of the created temporary
        // directory.
    {
        return d_dirName;
    }
};

int changeModificationTime(const bsl::string& fileName,
                           int                delta)
    // Change the modification time of the file with the specified 'fileName'
    // by the specified 'delta' seconds.  Return '0' if the file exists and
    // the modification time was changed and non-zero status otherwise.
{
    if (false == bdls::FilesystemUtil::exists(fileName)) {
        return -1;                                                    // RETURN
    }

#ifndef BSLS_PLATFORM_OS_WINDOWS
    struct stat fileStats;
    int         rc = stat(fileName.c_str(), &fileStats);

    ASSERT(0 == rc);

    struct utimbuf fileTimes;
    fileTimes.actime = fileStats.st_atime;
    fileTimes.modtime = fileStats.st_mtime + delta;

    rc = utime(fileName.c_str(), &fileTimes);

    return rc;
#else
    // Quick and dirty.
    typedef bdls::FilesystemUtil FsUtil;

    bdlt::Datetime fileDateTime;
    int            rc;

    rc = FsUtil::getLastModificationTime(&fileDateTime, fileName);

    ASSERT(0 == rc);

    fileDateTime.addSeconds(delta);

    FILETIME   fileTime;
    SYSTEMTIME stUTC;

    stUTC.wYear         = fileDateTime.year();
    stUTC.wMonth        = fileDateTime.month();
    stUTC.wDay          = fileDateTime.day();
    stUTC.wHour         = fileDateTime.hour();
    stUTC.wMinute       = fileDateTime.minute();
    stUTC.wSecond       = fileDateTime.second();
    stUTC.wMilliseconds = fileDateTime.millisecond();

    SystemTimeToFileTime(&stUTC, &fileTime);

    FsUtil::FileDescriptor hFile = FsUtil::open(
                                           fileName,
                                           FsUtil::FileOpenPolicy::e_OPEN,
                                           FsUtil::FileIOPolicy::e_READ_WRITE);

    if (FsUtil::k_INVALID_FD == hFile) {
        return -1;                                                    // RETURN
    }

    rc = SetFileTime(hFile,
                     (LPFILETIME) NULL,
                     (LPFILETIME) NULL,
                     &fileTime);


    FsUtil::close(hFile);

    return rc;
#endif
}

void createFile(const bsl::string& fileName)
    // Create a file with the specified 'fileName' and writes some data to it.
{
    bsl::ofstream fs(fileName.c_str());
    fs << "0xdeadbeef";
    fs.close();

    ASSERT(true  == bdls::FilesystemUtil::exists(fileName));
}

}  // close unnamed namespace

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    const int  test                = argc > 1 ? atoi(argv[1]) : 0;
    const bool verbose             = argc > 2;
    const bool veryVerbose         = argc > 3;
    const bool veryVeryVerbose     = argc > 4;
    const bool veryVeryVeryVerbose = argc > 5;

    (void) veryVerbose;      // Suppress compiler warning.
    (void) veryVeryVerbose;
    (void) veryVeryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) { case 0:  // Zero is always the leading case.
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //   Extracted from component header file.
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following snippets of code illustrate the basic usage of
// 'balb::LogCleanerUtil'.
//
// Let's assume that the application was set up to log to files having the
// following pattern:
//..
    const char *appLogFilePattern = "/var/log/myApp/log*";
//..
// We want to clean up all the files older then a week, but leave at least 4
// latest log files.  First, we create a cleanup configuration object that will
// capture our parameters:
//..
    balb::FileCleanerConfiguration config(
                appLogFilePattern,
                bsls::TimeInterval(bdlt::TimeUnitRatio::k_SECONDS_PER_DAY * 7),
                4);
//..
// Then, we use this configuration to do a file cleanup:
//..
    balb::FileCleanerUtil::removeFiles(config);
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'removeFiles' METHOD
        //
        // Concerns:
        //:  1 Only files matching the pattern are subjects to removal.
        //:
        //:  2 The specified minimum number of files matching the pattern is
        //:    preserved independent of their modification time.
        //
        // Plan:
        //:  1 Populate the temporary directory with a set of files with
        //:    different modification times.
        //:
        //:  2 Execute the 'removeFiles' function with various set of input
        //:    parameters and verify that only expected files are removed.
        //:    (C-1..2)
        //
        // Testing:
        //   void removeFiles(const balb::FileCleanerConfiguration& config);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'removeFiles' METHOD"
                          << "\n============================" << endl;

        if (veryVeryVerbose) {
            bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_TRACE);
        }

        if (verbose) cout << "\tTesting removal on time only." << endl;
        {
            TempDirectoryGuard tempDirGuard;
            bsl::string        baseName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&baseName, "logFile");

            createFile(baseName + "1");
            createFile(baseName + "2");
            createFile(baseName + "3");
            createFile(baseName + "4");

            changeModificationTime(baseName + "1", -10);
            changeModificationTime(baseName + "2", -20);
            changeModificationTime(baseName + "3", -30);
            changeModificationTime(baseName + "4", -40);

            ObjConfig config((baseName + "*").c_str(),
                             bsls::TimeInterval(25),
                             0);
            // Remove all logs older than 25 seconds.
            Obj::removeFiles(config);

            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "4"));

            // Remove all logs older than 15 seconds.
            config.setMaxFileAge(bsls::TimeInterval(15));
            Obj::removeFiles(config);

            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "4"));

            // Remove all logs older than 5 seconds.
            config.setMaxFileAge(bsls::TimeInterval(5));
            Obj::removeFiles(config);

            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "4"));
        }

        if (verbose) cout << "\tTesting removal on time and number." << endl;
        {
            TempDirectoryGuard tempDirGuard;
            bsl::string        baseName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&baseName, "logFile");

            createFile(baseName + "1");
            createFile(baseName + "2");
            createFile(baseName + "3");
            createFile(baseName + "4");

            changeModificationTime(baseName + "1", -10);
            changeModificationTime(baseName + "2", -20);
            changeModificationTime(baseName + "3", -30);
            changeModificationTime(baseName + "4", -40);

            // Remove all but 4 logs older than 25 seconds.
            ObjConfig config((baseName + "*").c_str(),
                             bsls::TimeInterval(25),
                             4);
            Obj::removeFiles(config);

            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "4"));

            // Remove all but 4 logs older than 15 seconds.
            config.setMaxFileAge(bsls::TimeInterval(15));
            Obj::removeFiles(config);

            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "4"));

            // Remove all but 4 logs older than 5 seconds.
            config.setMaxFileAge(bsls::TimeInterval(5));
            Obj::removeFiles(config);

            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "4"));

            // Remove all but 3 logs older than 5 seconds.
            config.setMaxFileAge(bsls::TimeInterval(5));
            config.setMinNumFiles(3);
            Obj::removeFiles(config);

            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "4"));

            // Remove all but 1 log older than 5 seconds.
            config.setMaxFileAge(bsls::TimeInterval(5));
            config.setMinNumFiles(1);
            Obj::removeFiles(config);

            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "4"));
        }

        if (verbose)
            cout << "\tTesting removal of files with mod time in future."
                 << endl;
        {
            TempDirectoryGuard tempDirGuard;
            bsl::string        baseName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&baseName, "logFile");

            createFile(baseName + "1");
            createFile(baseName + "2");
            createFile(baseName + "3");
            createFile(baseName + "4");

            changeModificationTime(baseName + "1", 10);
            changeModificationTime(baseName + "2", 20);
            changeModificationTime(baseName + "3", 30);
            changeModificationTime(baseName + "4", 40);

            // Remove all but 4 logs older than 0 seconds.
            ObjConfig config((baseName + "*").c_str(),
                             bsls::TimeInterval(0),
                             0);
            Obj::removeFiles(config);

            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(true == bdls::FilesystemUtil::exists(baseName + "4"));
        }
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Run each method with arbitrary inputs and verify the behavior is
        //:   as expected.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << "\nBREATHING TEST"
                          << "\n==============" << endl;

        // Using 'bsls' logging in this test.

        if (veryVeryVerbose) {
            bsls::Log::setSeverityThreshold(bsls::LogSeverity::e_TRACE);
        }

        {
            TempDirectoryGuard tempDirGuard;
            bsl::string        baseName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&baseName, "logFile");

            createFile(baseName + "1");
            createFile(baseName + "2");
            createFile(baseName + "3");
            createFile(baseName + "4");

            changeModificationTime(baseName + "1", -10);
            changeModificationTime(baseName + "2", -20);
            changeModificationTime(baseName + "3", -30);
            changeModificationTime(baseName + "4", -40);

            // Remove all logs older than 35 seconds.
            ObjConfig config((baseName + "*").c_str(),
                             bsls::TimeInterval(35),
                             0);
            Obj::removeFiles(config);

            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "4"));

            // Remove all logs older than 15 seconds.
            config.setMaxFileAge(bsls::TimeInterval(15));
            Obj::removeFiles(config);

            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "4"));
        }
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
    }
    return testStatus;
}

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
