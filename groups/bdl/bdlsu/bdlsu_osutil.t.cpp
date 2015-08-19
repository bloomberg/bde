// bdlsu_osutil.t.cpp                                                 -*-C++-*-

#include <bdlsu_osutil.h>

#include <bsls_platform.h>

#include <bslma_default.h>
#include <bslma_defaultallocatorguard.h>
#include <bslma_testallocator.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_iostream.h>
#include <bsl_sstream.h>

#ifdef BSLS_PLATFORM_OS_WINDOWS
#include <windows.h>
#include <process.h>
#include <cstring>
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
// [ ]
//-----------------------------------------------------------------------------
// [1] BREATHING TEST

// ============================================================================
//                    STANDARD BDE ASSERT TEST MACROS
// ----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}
# define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

// ============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
// ----------------------------------------------------------------------------

#define LOOP_ASSERT(I,X) {                                                    \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__);}}

#define LOOP2_ASSERT(I,J,X) {                                                 \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": "                 \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) {                                               \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"     \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP4_ASSERT(I,J,K,L,X) {                                             \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\n";                    \
       aSsErT(1, #X, __LINE__); } }

#define LOOP5_ASSERT(I,J,K,L,M,X) {                                           \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" <<  \
       #K << ": " << K << "\t" << #L << ": " << L << "\t" <<                  \
       #M << ": " << M << "\n";                                               \
       aSsErT(1, #X, __LINE__); } }

// ============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
// ----------------------------------------------------------------------------

#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", " << flush; // 'P(X)' without '\n'
#define T_ cout << "\t" << flush;             // Print tab w/o newline.
#define L_ __LINE__                           // current Line number

// ============================================================================
//                  NEGATIVE-TEST MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT_SAFE_PASS(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_PASS(EXPR)
#define ASSERT_SAFE_FAIL(EXPR) BSLS_ASSERTTEST_ASSERT_SAFE_FAIL(EXPR)
#define ASSERT_PASS(EXPR)      BSLS_ASSERTTEST_ASSERT_PASS(EXPR)
#define ASSERT_FAIL(EXPR)      BSLS_ASSERTTEST_ASSERT_FAIL(EXPR)
#define ASSERT_OPT_PASS(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_PASS(EXPR)
#define ASSERT_OPT_FAIL(EXPR)  BSLS_ASSERTTEST_ASSERT_OPT_FAIL(EXPR)

// ============================================================================
//                     GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef bdlsu::OsUtil Obj;

// ============================================================================
//                            MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int                 test = argc > 1 ? atoi(argv[1]) : 0;
    bool             verbose = argc > 2;
    bool         veryVerbose = argc > 3;
    bool     veryVeryVerbose = argc > 4;
    bool veryVeryVeryVerbose = argc > 5;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

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

        // The following example demonstrates using 'getOsInfo' to obtain
        // information about the operating system at runtime and writing it to
        // the console.

        // First, we create strings for the operating system name ('osName'),
        // version ('osVersion'), and patch ('osPatch'), and then call
        // 'getOsInfo' to load these strings with values for the operating
        // system the task is executing in:

        bsl::string name;
        bsl::string version;
        bsl::string patch;
        int rc = bdlsu::OsUtil::getOsInfo(&name, &version, &patch);

        // Then, we use the standard streams to write the operating system
        // version information to the console, or an error message of failure:

        if (0 == rc) {
            cout << "OS Name: " << name << "\n"
                      << "Version: " << version << "\n"
                      << "Patch:   " << patch << "\n";
        } else {
            cout << "Cannot determine OS name and version\n";
        }

        // Finally, the resulting console output on the
        // Red Hat Enterprise Linux Server 5.5 would be
        //..
        // OS Name: Linux
        // Version: 2.6.18-194.32.1.el5
        // Patch:   #1 SMP Mon Dec 20 10:52:42 EST 2010
        //..

      } break;
      case 2: {
        // --------------------------------------------------------------------
        // OS IDENTIFICATION
        //   Ensure that the operating system is identified correctly.
        //
        // Concerns:
        //: 1 The operating system is identified as defined in the contract.
        //
        // Plan:
        //: 1 Verify OS Name is 'Windows' when built on MS Windows, and the
        //:   loaded version and patch match values returned by GetVersionEx().
        //: 2 On Posix systems verify that loaded 'name', 'version' and 'patch'
        //:   match sysname, release and version, respectively, as returned by
        //:   uname(2).
        //
        // Testing:
        //   getOsInfo
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "OS IDENTIFICATION TEST" << endl
                          << "======================" << endl;
        bsl::string name;
        bsl::string version;
        bsl::string patch;
        ASSERT(0 == Obj::getOsInfo(&name, &version, &patch));
#ifdef BSLS_PLATFORM_OS_WINDOWS
        ASSERT("Windows" == name);

        OSVERSIONINFOEX osvi;
        memset(&osvi, 0, sizeof(osvi));
        osvi.dwOSVersionInfoSize = sizeof(osvi);
        ASSERT(GetVersionEx(reinterpret_cast<LPOSVERSIONINFO>(&osvi)));

        ostringstream expectedVersion;
        expectedVersion << osvi.dwMajorVersion
                        << '.'
                        << osvi.dwMinorVersion;
        ASSERT(version == expectedVersion.str());

        ASSERT(patch == osvi.szCSDVersion);
#else
        struct utsname unameInfo;
        ASSERT(uname(&unameInfo) >= 0);
        ASSERT(name == unameInfo.sysname);
        ASSERT(version == unameInfo.release);
        ASSERT(patch == unameInfo.version);
#endif
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
