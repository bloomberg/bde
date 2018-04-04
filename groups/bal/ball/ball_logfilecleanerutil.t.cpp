// ball_logfilecleanerutil.t.cpp                                      -*-C++-*-
#include <ball_logfilecleanerutil.h>

#include <ball_fileobserver2.h>

#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>

#include <bdlt_datetime.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>
#include <bslma_usesbslmaallocator.h>

#include <bslmf_nestedtraitdeclaration.h>

#include <bslmt_threadutil.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsls_timeinterval.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // strlen(), memset(), memcpy(), memcmp()
#include <bsl_ctime.h>
#include <bsl_iostream.h>
#include <bsl_fstream.h>

#ifndef BSLS_PLATFORM_OS_WINDOWS
#include <sys/stat.h>
#include <sys/types.h>
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
// [ 2] void logPatternToFilePattern(filePattern, logPattern);
// [ 3] enableLogFileCleanup(OBSERVER *observer, const FCConfiguration&);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 4] USAGE EXAMPLE 1
// [ 5] USAGE EXAMPLE 2

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

typedef ball::LogFileCleanerUtil Obj;

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
      case 5: {
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
        //   USAGE EXAMPLE 2
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

///Example 2: Cleaning Log Files On File Rotation
///- - - - - - - - - - - - - - - - - - - - - - -
// The following snippets of code illustrate how the application can implement
// automatic log file cleanup from the observer's file rotation callback.
//..
    TempDirectoryGuard tempDirGuard;
    bsl::string        baseName(tempDirGuard.getTempDirName());
    bdls::PathUtil::appendRaw(&baseName, "logFile%T");
//..
// Suppose that the application was set up to do its logging using one of the
// 'ball' file observers (see 'ball_fileobserver2') with the following log
// pattern:
//..
    const char *appLogPattern = baseName.c_str();
//..
// First, we need to convert the log filename pattern to the pattern that can
// be used for filename matching on the filesystem:
//..
    bsl::string fileNamePattern;
    ball::LogFileCleanerUtil::logPatternToFilePattern(&fileNamePattern,
                                                      appLogPattern);
//..
// Then, we create a configuration for the file cleaner utility.  The sample
// configuration below instructs the file cleaner to remove all log files that
// match the specified file pattern and are older than a week, but to keep at
// least 4 most recent log files:
//..
    balb::FileCleanerConfiguration config(fileNamePattern.c_str(),
                                          bsls::TimeInterval(7 * 60 * 60 * 24),
                                          4);
//..
// Next, we create a file observer and enable file logging:
//..
    ball::FileObserver2 observer;
    observer.enableFileLogging(appLogPattern);
//..
// Finally, we use the utility function to install the file rotation callback
// that will invoke file cleanup with the specified configuration:
//..
    ball::LogFileCleanerUtil::enableLogFileCleanup(&observer, config);
//..
// Note that the file cleanup will be performed immediately and on every log
// file rotation performed by the file observer.  Also note that this method
// overrides the file rotation callback currently installed in the file
// observer.
      } break;
      case 4: {
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
        //   USAGE EXAMPLE 1
        // --------------------------------------------------------------------

        if (verbose) cout << "\nUSAGE EXAMPLE"
                          << "\n=============" << endl;

///Example 1: Basic Usage
/// - - - - - - - - - - -
// The following snippets of code illustrate the basic usage of
// 'ball::LogFileCleanerUtil'.
//
// Suppose that the application was set up to do its logging using one of the
// 'ball' file observers (see 'ball_fileobserver2') with the following log
// pattern:
//..
    const char *appLogPattern = "/var/log/myApp/log%T";
//..
// First, we need to convert the log filename pattern to the pattern that can
// be used for filename matching on the filesystem:
//..
    bsl::string fileNamePattern;
    ball::LogFileCleanerUtil::logPatternToFilePattern(&fileNamePattern,
                                                      appLogPattern);
//..
// Finally, we test the resulting file pattern:
//..
    ASSERT("/var/log/myApp/log*" == fileNamePattern);
//..
      } break;
      case 3: {
        // --------------------------------------------------------------------
        // TESTING 'enableLogFileCleanup' METHOD
        //
        // Concerns:
        //
        // Plan:
        //
        // Testing:
        //   enableLogFileCleanup(OBSERVER *observer, const FCConfiguration&);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'enableLogFileCleanup' METHOD"
                          << "\n====================================="
                          << endl;

        {
            TempDirectoryGuard tempDirGuard;
            bsl::string        baseName(tempDirGuard.getTempDirName());
            bdls::PathUtil::appendRaw(&baseName, "logFile");

            createFile(baseName + "1");
            createFile(baseName + "2");
            createFile(baseName + "3");
            createFile(baseName + "4");

            changeModificationTime(baseName + "1", -10);
            changeModificationTime(baseName + "2", -22);  // <- important
            changeModificationTime(baseName + "3", -30);
            changeModificationTime(baseName + "4", -40);

            ball::FileObserver2 observer;
            balb::FileCleanerConfiguration config((baseName + "*").c_str(),
                                                  bsls::TimeInterval(25),
                                                  0);

            observer.enableFileLogging(baseName.c_str());

            Obj::enableLogFileCleanup(&observer, config);

            // The cleanup is called immediately.
            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "4"));

            bslmt::ThreadUtil::microSleep(0, 5);
            observer.forceRotation();

            // Forced rotation should call installed callback.
            ASSERT(true  == bdls::FilesystemUtil::exists(baseName + "1"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "2"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "3"));
            ASSERT(false == bdls::FilesystemUtil::exists(baseName + "4"));
        }
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'logPatternToFilePattern' METHOD
        //
        // Concerns:
        //:  1 All valid '%'-escape sequences are recognized.
        //:
        //:  2 All other input pattern symbols passed into output intact.
        //
        // Plan:
        //:  1 Using the table-driven technique, test function on various input
        //:    patterns.  (C-1..2)
        //
        // Testing:
        //   void logPatternToFilePattern(filePattern, logPattern);
        // --------------------------------------------------------------------
        if (verbose) cout << "\nTESTING 'logPatternToFilePattern' METHOD"
                          << "\n========================================"
                          << endl;
        {
            static const struct {
                int         d_line;           // line number
                const char *d_logPattern_p;   // input log pattern
                const char *d_filePattern_p;  // expected file pattern
            } DATA[] = {
                {L_,  "",              "*"         },
                {L_,  "%",             "%*"        },
                {L_,  "%%",            "%*"        },
                {L_,  "%%%",           "%%*"       },
                {L_,  "%%%%",          "%%*"       },
                {L_,  "%%%%%",         "%%%*"      },
                {L_,  "%%%%%",         "%%%*"      },

                // Valid '%'-escape sequences
                {L_,  "%D",            "*"         },
                {L_,  "%M",            "*"         },
                {L_,  "%Y",            "*"         },
                {L_,  "%h",            "*"         },
                {L_,  "%m",            "*"         },
                {L_,  "%s",            "*"         },
                {L_,  "%p",            "*"         },
                {L_,  "%T",            "*"         },

                {L_,  "a%D",           "a*"        },
                {L_,  "b%M",           "b*"        },
                {L_,  "c%Y",           "c*"        },
                {L_,  "d%h",           "d*"        },
                {L_,  "e%m",           "e*"        },
                {L_,  "f%s",           "f*"        },
                {L_,  "g%p",           "g*"        },
                {L_,  "h%T",           "h*"        },

                {L_,  "%DD",           "*D*"       },
                {L_,  "%MM",           "*M*"       },
                {L_,  "%YY",           "*Y*"       },
                {L_,  "%hh",           "*h*"       },
                {L_,  "%mm",           "*m*"       },
                {L_,  "%ss",           "*s*"       },
                {L_,  "%pp",           "*p*"       },
                {L_,  "%TT",           "*T*"       },

                {L_,  "a%Dz",          "a*z*"      },
                {L_,  "b%Mx",          "b*x*"      },
                {L_,  "c%Yy",          "c*y*"      },
                {L_,  "d%hr",          "d*r*"      },
                {L_,  "e%mq",          "e*q*"      },
                {L_,  "f%st",          "f*t*"      },
                {L_,  "g%ps",          "g*s*"      },
                {L_,  "h%Tv",          "h*v*"      },

                // Sequence of 2+ valid '%'-escape sequences
                {L_,  "%D%Y",          "*"         },
                {L_,  "%T%D",          "*"         },
                {L_,  "%T%m%p",        "*"         },
                {L_,  "z%T%m%p",       "z*"        },
                {L_,  "%Ta%m%p",       "*a*"       },
                {L_,  "%Y:%h%m",       "*:*"       },

                // Random strings with random mix of % and chars
                {L_,  "*",             "*"         },
                {L_,  "a*",            "a*"        },
                {L_,  "%*",            "%*"        },
                {L_,  "%d",            "%d*"       },
                {L_,  "%z",            "%z*"       },
                {L_,  "A%",            "A%*"       },
                {L_,  "T%",            "T%*"       },
                {L_,  "%D%",           "*%*"       },
                {L_,  "a%d",           "a%d*"      },
                {L_,  "z%xd",          "z%xd*"     },
                {L_,  "s%c%",          "s%c%*"     },
                {L_,  "%Y%%",          "*%*"       },
                {L_,  "%Y%.",          "*%.*"      },
                {L_,  "%s*",           "*"         },
                {L_,  "%Y%.%h",        "*%.*"      },
                {L_,  "%%h%%",         "%h%*"      },
                {L_,  "%%%s",          "%*"        },
                {L_,  "%%%D%",         "%*%*"      },
                {L_,  "x%T%m%pd",      "x*d*"      },
                {L_,  "x%Y%M%m*",      "x*"        },
                {L_,  "/log/mA",       "/log/mA*"  },

                // Patterns from the documentation
                {L_,  "a.log",         "a.log*"    },
                {L_,  "a.log.%T",      "a.log.*"   },
                {L_,  "a.log.%Y",      "a.log.*"   },
                {L_,  "a.log.%Y%M%D",  "a.log.*"   },
                {L_,  "a.%T.log",      "a.*.log*"  },
            };  // DATA

            enum { DATA_LEN = sizeof DATA / sizeof *DATA };

            for (int i = 0; i < DATA_LEN; ++i) {
                const int   LINE         = DATA[i].d_line;
                const char *LOG_PATTERN  = DATA[i].d_logPattern_p;
                const char *FILE_PATTERN = DATA[i].d_filePattern_p;

                if (veryVerbose) {
                    T_ P_(LINE) P_(LOG_PATTERN) P(FILE_PATTERN)
                }

                bsl::string result;
                Obj::logPatternToFilePattern(&result, LOG_PATTERN);

                ASSERTV(result, FILE_PATTERN == result);
            }
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
