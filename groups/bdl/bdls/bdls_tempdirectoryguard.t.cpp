// bdls_tempdirectoryguard.t.cpp                                      -*-C++-*-

#include <bdls_tempdirectoryguard.h>

#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>

#include <bdlb_stringviewutil.h>

#include <bslim_testutil.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//
// [ 2] USAGE EXAMPLE
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
//                   GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
// ----------------------------------------------------------------------------

// ============================================================================
//                               USAGE EXAMPLE
// ----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Basic Usage
/// - - - - - - - - - - -
// Suppose an algorithm requires writing data to a temporary file on disk
// during processing:
//..
void testAlgorithm(const bsl::string &testFileName);
    // Do "algorithm" using the specified 'testFileName' for intermidiate state
    // storage.
//..
// A function looking to use this algorithm can obtain a directory in which to
// put this file, guaranteed to not be used by other processes and to be
// cleaned up on normal exit, using an instance of 'bdls::TempDirectoryGuard':
//..
void usesTestAlgorithm()
{
    bdls::TempDirectoryGuard tempDirGuard("my_algo_");

    bsl::string tmpFileName(tempDirGuard.getTempDirName());
    bdls::PathUtil::appendRaw(&tmpFileName,"algorithm.scratch");

    testAlgorithm(tmpFileName);
}
//..
// After exiting, the scratch file (named "algorithm.scratch") and the
// temporary directory (with an unspecified name starting with "my_algo_"),
// possibly in the system temp directory or the current working directory, will
// be removed.

void testAlgorithm(const bsl::string &)
{
}

// ============================================================================
//                               MAIN PROGRAM
// ----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    (void)verbose;
    (void)veryVerbose;
    (void)veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
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

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

        usesTestAlgorithm();

      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //
        // Concerns:
        //: 1 A temporary directory gets created by an instance of the guard.
        //: 2 That directory is empty.
        //: 3 That directory's name starts with the right prefix.
        //: 4 A second guard creats a distinct directory name.
        //: 5 The directory is removed when the guard is destroyed.
        //
        // Plan:
        //: 1 Create a guard, check all concerns
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        {
            const bsl::string prefix = "test_";

            bsl::string g1dir;
            bsl::string tmpfile;
            bsl::string g2dir;

            {
                bdls::TempDirectoryGuard guard1(prefix);
                g1dir = guard1.getTempDirName();

                ASSERTV(g1dir,bdls::FilesystemUtil::exists(g1dir));
                ASSERTV(g1dir,bdls::FilesystemUtil::isDirectory(g1dir));

                bslstl::StringRef head,tail;
                bdls::PathUtil::splitFilename(&head,&tail,g1dir);

                ASSERTV(head,tail,
                        bdlb::StringViewUtil::startsWith(tail,prefix));

                {
                    bdls::TempDirectoryGuard guard2(prefix);
                    g2dir = guard2.getTempDirName();

                    ASSERTV(g2dir,bdls::FilesystemUtil::exists(g2dir));
                    ASSERTV(g2dir,bdls::FilesystemUtil::isDirectory(g2dir));

                    ASSERTV(g1dir,g2dir,g1dir != g2dir);
                }
            }

            ASSERTV(g1dir,!bdls::FilesystemUtil::exists(g1dir));
            ASSERTV(g1dir,!bdls::FilesystemUtil::isDirectory(g2dir));
            ASSERTV(g2dir,!bdls::FilesystemUtil::exists(g1dir));
            ASSERTV(g2dir,!bdls::FilesystemUtil::isDirectory(g2dir));
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
// Copyright 2019 Bloomberg Finance L.P.
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
