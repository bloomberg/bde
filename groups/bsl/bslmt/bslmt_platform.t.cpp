// bslmt_platform.t.cpp                                               -*-C++-*-

#include <bslmt_platform.h>
#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_cstdlib.h>     // 'atoi'

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
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

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }
//-----------------------------------------------------------------------------

int typeTest(const bslmt::Platform::PosixThreads&) { return 1; }
int typeTest(const bslmt::Platform::Win32Threads&) { return 2; }

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                            * Overview *
// Since this component implements CPP macro's and typedefs, which may or may
// not be defined, there is not too much to test in this driver.  Since
// correctness will be affected by compile-time switches during the build
// process, any compile-time tests we come up with should probably reside
// directly in the header or implementation file.
//-----------------------------------------------------------------------------
// [ 1] Ensure that ThreadPolicy is set.
// [ 1] Ensure that exactly one of each THREADS type is set.
//=============================================================================

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------
        // MINIMAL DEFINITION TEST:
        //   We want to make sure that the ThreadPolicy typedef is set and
        //   exactly one each of _THREADS macro is set.
        // --------------------------------------------------------

        if (verbose) cout << endl << "Minimal Definition Test" <<
                             endl << "-----------------------" << endl;

            // Compile-time tests for this case are located in the validation
            // section of this component's header file.

        if (verbose) cout << endl << "Verify typedefs are set" << endl;

        #ifdef BSLS_PLATFORM_OS_UNIX
        bslmt::Platform::ThreadPolicy policy;
        ASSERT(1 == typeTest(policy));
        #endif

        #ifdef BSLS_PLATFORM_OS_WINDOWS
        bslmt::Platform::ThreadPolicy policy;
        ASSERT(2 == typeTest(policy));
        #endif

        if (verbose) {
            cout << endl << "Print sefined symbols" << endl;

            #if defined(BSLMT_PLATFORM_POSIX_THREADS)
                ASSERT(0 <= BSLMT_PLATFORM_POSIX_THREADS);
                cout  << "\tBCES_PLATFORM_POSIX_THREADS = "
                      <<    BSLMT_PLATFORM_POSIX_THREADS << endl;
            #endif

            #if defined(BSLMT_PLATFORM_WIN32_THREADS)
                ASSERT(0 <= BSLMT_PLATFORM_WIN32_THREADS);
                cout  << "\tBCES_PLATFORM_WIN32_THREADS = "
                      <<    BSLMT_PLATFORM_WIN32_THREADS << endl;
            #endif

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
