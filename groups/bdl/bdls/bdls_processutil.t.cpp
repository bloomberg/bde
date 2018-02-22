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
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_iostream.h>
#include <bsl_string.h>

#if defined BSLS_PLATFORM_OS_UNIX
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
typedef bdls::ProcessUtil     Obj;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? bsl::atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
//  bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // The component under test may use the default allocator, but must not
    // leak from it.

    bslma::TestAllocator ta("test", veryVeryVeryVerbose);
    bslma::DefaultAllocatorGuard defaultGuard(&ta);

    switch(test) { case 0:
      case 2: {
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

        const char *tmpFileName = "tmp.processutil.case2.eraseMe.txt";
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

        ASSERTV(argv[0], FUtil::exists(argv[0]));

        bsl::string name;
        ASSERT(0 == bdls::ProcessUtil::getProcessName(&name));

        ASSERTV(name, FUtil::exists(name));

        bsl::string nameBasename, argv0Basename;
        ASSERT(0 == bdls::PathUtil::getBasename(&nameBasename,  name));
        ASSERT(0 == bdls::PathUtil::getBasename(&argv0Basename, argv[0]));

        ASSERTV(nameBasename, argv0Basename, name, argv[0],
                                                nameBasename == argv0Basename);

        if (veryVerbose) {
            P_(nameBasename);    P(argv0Basename);
            P(argv[0]);
            P(name);
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
      case -2: {
        // --------------------------------------------------------------------
        // 'getProcessName': CHDIR TEST
        //
        // Concerns:
        //: 1 Ensure that we are able to find the executable after a 'chdir'
        //:   after process start up.
        //:
        //: 2 This is a negative test case because it does not pass on all
        //:   platforms, and it also requires the test to be set up so that
        //:   the executable will be called from a relative path.
        //
        // Plan:
        //: 1 Call 'chdir'.
        //:
        //: 2 Call 'getProcessName' and verify that the file name it returns
        //:   still exists.
        // --------------------------------------------------------------------

        if (verbose) cout << "'getProcessName': CHDIR TEST\n"
                             "============================\n";

#undef CHDIR_CALL
#undef GETCWD_CALL
#if defined BSLS_PLATFORM_OS_UNIX 
# define CHDIR_CALL    ::chdir
# define GETCWD_CALL   ::getcwd

const bool expRelative = true;
#else
# define CHDIR_CALL    ::_chdir
# define GETCWD_CALL   ::_getcwd

// Note that, on Windows, 'argv[0]' is always absolute, never relative, even if
// it was specified on the cmdline as relative.

const bool expRelative = false;
#endif

        if (verbose) P(argv[0]);
        ASSERTV(argv[0], FUtil::exists(argv[0]));

        // Detect relative path on Unix or Windows

        const bool argv0IsRelative = '/' != argv[0][0] && '\\' != argv[0][0] &&
                                                    !bsl::strchr(argv[0], ':');
        ASSERTV(expRelative, argv0IsRelative, expRelative == argv0IsRelative);

        CHDIR_CALL("..");

        enum { k_CWD_BUF_LEN = 1024 };
        char cwdBuf[k_CWD_BUF_LEN];
        GETCWD_CALL(cwdBuf, k_CWD_BUF_LEN);
        if (verbose) P(cwdBuf);

        ASSERTV(cwdBuf, argv[0], FUtil::exists(argv[0]), argv0IsRelative,
                                    FUtil::exists(argv[0]) == !argv0IsRelative)

        bsl::string name;
        int rc = Obj::getProcessName(&name);
        ASSERT(0 == rc);
        ASSERT(FUtil::exists(name));
        if (verbose) P(name);

#undef CHDIR_CALL
#undef GETCWD_CALL
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
