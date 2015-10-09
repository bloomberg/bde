// btlso_platform.t.cpp                                               -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_platform.h>

#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>     // atoi()

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

int typeTest(const btlso::Platform::BsdSockets&) { return 1; }
int typeTest(const btlso::Platform::WinSockets&) { return 2; }

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
// [ 1] 'BTLSO_PLATFORM_BSD_SOCKETS' and 'BTLSO_PLATFORM_WIN_SOCKETS'
//=============================================================================
// [ 2] USAGE EXAMPLE

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Defining a 'my_Socket' class
///- - - - - - - - - - - - - - - - - - - -
// Writing portable software sometimes involves specializing implementations
// to work with platform-specific interfaces.  For example, a socket-level
// communications framework would need to operate differently on a platform
// having a Windows operating system than on one having a Unix one (but it is
// probably unnecessary to distinguish between their respective versions):
//..
    // my_socket.h
//  #include <btlso_platform.h>
//
    #ifdef BSLS_PLATFORM_OS_WINDOWS
        #ifndef INCLUDED_WINSOCK2
        #include <winsock2.h>
        #define INCLUDED_WINSOCK2
        #endif
    #endif
//
    class my_Socket {
//
    #ifdef BTLSO_PLATFORM_WIN_SOCKETS
        SOCKET d_socketObject;  // Windows SOCKET handle
    #else
        int d_socketObject;     // Unix socket descriptor
    #endif
//
    // ...
//
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
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

        my_Socket socket;

      } break;
      case 1: {
        // --------------------------------------------------------
        // MINIMAL DEFINITION TEST:
        //   We want to make sure that the SocketFamily typedef is set and
        //   exactly one each of *_SOCKETS macro is set.
        // --------------------------------------------------------

        if (verbose) cout << endl << "Minimal Definition Test" <<
                             endl << "-----------------------" << endl;

            // Compile-time tests for this case are located in the validation
            // section of this component's header file.

        if (verbose) cout << endl << "Verify typedefs are set" << endl;

        #ifdef BSLS_PLATFORM_OS_UNIX
        btlso::Platform::SocketFamily implementation;
        ASSERT(1 == typeTest(implementation));
        #endif

        #ifdef BSLS_PLATFORM_OS_WINDOWS
        btlso::Platform::SocketFamily implementation;
        ASSERT(2 == typeTest(implementation));
        #endif

        if (verbose) {
            cout << endl << "Print defined symbols" << endl;

            #if defined(BTLSO_PLATFORM_BSD_SOCKETS)
                ASSERT(0 <= BTLSO_PLATFORM_BSD_SOCKETS);
                cout  << "\tBTESO_PLATFORM_BSD_SOCKETS = "
                      <<    BTLSO_PLATFORM_BSD_SOCKETS << endl;
            #endif

            #if defined(BTLSO_PLATFORM_WIN_SOCKETS)
                ASSERT(0 <= BTLSO_PLATFORM_WIN_SOCKETS);
                cout  << "\tBTESO_PLATFORM_WIN_SOCKETS = "
                      <<    BTLSO_PLATFORM_WIN_SOCKETS << endl;
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
