// bteso_eventtype.t.cpp         -*-C++-*-
#include <bteso_eventtype.h>
#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>

#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_iostream.h>
#include <bsl_strstream.h>

using namespace bsl;  // automatically added by script

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// There is nothing to test here, except that the socket event types are well
// defined and accessible with the proper names as advertised in the
// component-level documentation.
//-----------------------------------------------------------------------------
// [1] bteso_EventType::BTESO_ACCEPT
// [1] bteso_EventType::BTESO_CONNECT
// [1] bteso_EventType::BTESO_READ
// [1] bteso_EventType::BTESO_WRITE
// [1] bteso_EventType::BTESO_LENGTH
//=============================================================================

//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma_TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST:
        //
        // Plan:
        //   Simply access the enumerated values and assert their values.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "BREATHING TEST" << endl
                          << "==============" << endl;

        ASSERT( 0 == bteso_EventType::BTESO_ACCEPT );
        ASSERT( 1 == bteso_EventType::BTESO_CONNECT );
        ASSERT( 2 == bteso_EventType::BTESO_READ );
        ASSERT( 3 == bteso_EventType::BTESO_WRITE );
        ASSERT( 4 == bteso_EventType::BTESO_LENGTH );

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
//      Copyright (C) Bloomberg L.P., 2003
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
