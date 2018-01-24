// bdls_osutil.t.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdls_osutil.h>

#include <bslim_testutil.h>

#include <bslma_default.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>
#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_string.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <process.h>
#include <bsl_cstring.h>
#else
#include <unistd.h>
#include <sys/utsname.h>
#endif

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
//-----------------------------------------------------------------------------
// [2] int getOsInfo(bsl::string *os, bsl::string *v, bsl::string *ptch);
//-----------------------------------------------------------------------------
// [1] BREATHING TEST
// [3] USAGE EXAMPLE

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
//                     NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdls::OsUtil Obj;

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    (void)     veryVerbose;  // not currently used
    (void) veryVeryVerbose;  // not currently used

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    // CONCERN: In no case does memory come from the global allocator.

    bslma::TestAllocator globalAllocator("global", veryVeryVeryVerbose);
    bslma::Default::setGlobalAllocator(&globalAllocator);

    // CONCERN: The default allocator has not been locked, and in no case does
    // memory come from the global allocator.

    bslma::TestAllocator defaultAllocator("default", veryVeryVeryVerbose);
    ASSERT(0 == bslma::Default::setDefaultAllocator(&defaultAllocator));

    // CONCERN: The default allocator has not been locked, and there are no
    // leaks through the default allocator when the test driver exits.

    switch (test) { case 0:
      case 3: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

// The following example demonstrates using 'getOsInfo' to obtain information
// about the operating system at runtime and writing it to the console.
//
// First, we create strings for the operating system name ('osName'), version
// ('osVersion'), and patch ('osPatch'), and then call 'getOsInfo' to load
// these strings with values for the operating system the task is executing in:
//..
    bsl::string name;
    bsl::string version;
    bsl::string patch;
//..
// Then, we use the standard streams to write the operating system version
// information to the console, or an error message of failure:
//..
    const int rc = bdls::OsUtil::getOsInfo(&name, &version, &patch);
    if (rc == 0) {
        bsl::cout << "OS Name: " << name << "\n"
                  << "Version: " << version << "\n"
                  << "Patch:   " << patch << "\n";
    } else {
        bsl::cout << "Cannot determine OS name and version\n";
    }
//..
// Finally, the resulting console output on the Red Hat Enterprise Linux Server
// 5.5 would be
//..
//  OS Name: Linux
//  Version: 2.6.18-194.32.1.el5
//  Patch:   #1 SMP Mon Dec 20 10:52:42 EST 2010
//..
// On Windows 7 SP1, the display would be
//..
//  OS Name: Windows
//  Version: 6.1
//  Patch:   Service Pack 1
//..
      } break;
      case 2: {
        // --------------------------------------------------------------------
        // TESTING 'getOsInfo'
        //   Ensure that the operating system is identified correctly.
        //
        // Concerns:
        //: 1 The operating system is identified as defined in the contract.
        //: 2 The result is always zero, as there is no clear way to force an
        //:   OS to fail to report the expected data.
        //
        // Plan:
        //: 1 Verify the function returns zero
        //: 2 Verify OS Name is 'Windows' when built on MS Windows, and the
        //:   loaded version and patch match values returned by GetVersionEx().
        //: 3 On Posix systems verify that loaded 'name', 'version' and 'patch'
        //:   match sysname, release and version, respectively, as returned by
        //:   uname(2).
        //: 4 Negative testing: verify that passing a null pointer for any
        //:   argument is detected by a 'BSLS_ASSERT'.
        //
        // Testing:
        //   int getOsInfo(bsl::string *os, bsl::string *v, bsl::string *ptch);
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "TESTING 'getOsInfo'" << endl
                          << "===================" << endl;
        bsl::string name;
        bsl::string version;
        bsl::string patch;
        ASSERT(0 == Obj::getOsInfo(&name, &version, &patch));

#ifdef BSLS_PLATFORM_OS_WINDOWS
        ASSERTV(name, "Windows" == name);

        OSVERSIONINFOEX osvi;
        memset(&osvi, 0, sizeof(osvi));
        osvi.dwOSVersionInfoSize = sizeof(osvi);
        ASSERT(GetVersionEx(reinterpret_cast<LPOSVERSIONINFO>(&osvi)));

        ostringstream expectedVersion;
        expectedVersion << osvi.dwMajorVersion
                        << '.'
                        << osvi.dwMinorVersion;
        ASSERTV(version,   expectedVersion.str(),
                version == expectedVersion.str());

        ASSERTV(patch, osvi.szCSDVersion, 0 == patch.find(osvi.szCSDVersion));
#else
        struct utsname unameInfo;
        ASSERT(uname(&unameInfo) >= 0);
        ASSERTV(name, unameInfo.sysname, name == unameInfo.sysname);
        ASSERTV(version, unameInfo.release, version == unameInfo.release);
        ASSERTV(patch, unameInfo.version, patch == unameInfo.version);
#endif

        if (verbose) cout << "Negative testing" << endl;
        {
            bsls::AssertTestHandlerGuard hg;

            ASSERT_FAIL(Obj::getOsInfo(    0, &version, &patch));
            ASSERT_FAIL(Obj::getOsInfo(&name,        0, &patch));
            ASSERT_FAIL(Obj::getOsInfo(&name, &version,      0));
            ASSERT_FAIL(Obj::getOsInfo(    0,        0,      0));
            ASSERT_PASS(Obj::getOsInfo(&name, &version, &patch));
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
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;
        bsl::string name;
        bsl::string version;
        bsl::string patch;
        ASSERT(0 == Obj::getOsInfo(&name, &version, &patch));

      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    // CONCERN: In no case does memory come from the global allocator.

    LOOP_ASSERT(globalAllocator.numBlocksTotal(),
                0 == globalAllocator.numBlocksTotal());

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = " << testStatus << "." << endl;
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
