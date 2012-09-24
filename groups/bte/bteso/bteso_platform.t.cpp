// bteso_platform.t.cpp    -*-C++-*-

#include <bteso_platform.h>

#include <bsls_platform.h>

#include <bsl_iostream.h>
#include <bsl_c_stdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script



//==========================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//--------------------------------------------------------------------------
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
//--------------------------------------------------------------------------

int typeTest(const bteso_Platform::BsdSockets&) { return 1; }
int typeTest(const bteso_Platform::WinSockets&) { return 2; }

//==========================================================================
//                             TEST PLAN
//--------------------------------------------------------------------------
//                            * Overview *
// Since this component implements CPP macro's and typedefs, which may or may
// not be defined, there is not too much to test in this driver.  Since
// correctness will be affected by compile-time switches during the build
// process, any compile-time tests we come up with should probably reside
// directly in the header or implementation file.
//--------------------------------------------------------------------------
// [ 1] Ensure that ThreadPolicy is set.
// [ 1] Ensure that exactly one of each THREADS type is set.
//==========================================================================

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
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
        bteso_Platform::SocketFamily implementation;
        ASSERT(1 == typeTest(implementation));
        #endif

        #ifdef BSLS_PLATFORM_OS_WINDOWS
        bteso_Platform::SocketFamily implementation;
        ASSERT(2 == typeTest(implementation));
        #endif

        if (verbose) {
            cout << endl << "Print defined symbols" << endl;

            #if defined(BTESO_PLATFORM_BSD_SOCKETS)
                ASSERT(0 <= BTESO_PLATFORM_BSD_SOCKETS);
                cout  << "\tBTESO_PLATFORM_BSD_SOCKETS = "
                      <<    BTESO_PLATFORM_BSD_SOCKETS << endl;
            #endif

            #if defined(BTESO_PLATFORM_WIN_SOCKETS)
                ASSERT(0 <= BTESO_PLATFORM_WIN_SOCKETS);
                cout  << "\tBTESO_PLATFORM_WIN_SOCKETS = "
                      <<    BTESO_PLATFORM_WIN_SOCKETS << endl;
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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2002
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
