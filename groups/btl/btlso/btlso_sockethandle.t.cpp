// btlso_sockethandle.t.cpp                                           -*-C++-*-

#include <btlso_sockethandle.h>

#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>
using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
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

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                            * Overview *
// Verify that the typedefs and enums are accessible and compile on this
// platform.
//-----------------------------------------------------------------------------
// [ 1] USAGE
//=============================================================================

// Verify that the Handle is accessible.
struct Assertion {
    btlso::SocketHandle::Handle handle;
};

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The invalid handle is appropriately initialized.
        //
        // Plan:
        //: 1 Confirm that the invalid handle is correctly initialized.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" <<
                             endl << "--------------" << endl;

        // Verify that the error codes are accessible.

        ASSERT(btlso::SocketHandle::e_ERROR_EOF !=
               btlso::SocketHandle::e_ERROR_UNCLASSIFIED);

        // Verify that the invalid is correctly set.

#ifdef BTLSO_PLATFORM_WIN_SOCKETS

        ASSERT(INVALID_SOCKET == btlso::SocketHandle::INVALID_SOCKET_HANDLE);

#else

        ASSERT(-1             == btlso::SocketHandle::INVALID_SOCKET_HANDLE);

#endif

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
