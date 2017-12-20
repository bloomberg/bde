// btlso_sockethandle.t.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_sockethandle.h>

#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// Verify that the typedefs and enums are accessible and compile on this
// platform.
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
// [ 1] BREATHING TEST
//=============================================================================


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
//                  GLOBAL METHODS FOR TESTING
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing a socket class
///- - - - - - - - - - - - - - - - - - -
// Writing portable software involves providing a generic interface while
// specializing implementations to work with platform-provided interfaces.  As
// an example, we provide a platform-independent socket 'class', 'my_Socket'
// below.  The 'my_Socket' 'class' provides functionality to establish a
// connection and to read and write data on it.  Internally, the 'class' uses a
// 'btlso::SocketHandle::Handle' object to store the socket handle.  Note that
// for brevity only portions of this 'class' that are relevant to this example
// are shown:
//..
    class my_Socket {
        // This 'class' provides a platform-independent socket class.
//
        // DATA
        btlso::SocketHandle::Handle d_handle;
//
        // CREATORS
        my_Socket();
            // Create an unconnected 'my_Socket' object.
//
        // Rest of the interface elided for brevity
    };
//..
// The default-constructor can then be implemented as follows:
//..
    my_Socket::my_Socket()
    : d_handle(btlso::SocketHandle::INVALID_SOCKET_HANDLE)
    {
    }
//..


// Verify that the Handle is accessible.
struct Assertion {
    btlso::SocketHandle::Handle handle;
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
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

        // The usage example is a class declaration and definition.

      } break;
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
