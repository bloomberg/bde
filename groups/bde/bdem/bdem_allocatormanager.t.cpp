// bdem_allocatormanager.t.cpp                                        -*-C++-*-

#include <bdem_allocatormanager.h>

#include <bsl_iostream.h>

#include <bsl_cstdlib.h>     // atoi()

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

// ============================================================================
//                                   TESTPLAN
//
// ----------------------------------------------------------------------------
// TBD
// ----------------------------------------------------------------------------
// [ 1] USAGE example
// [ 1] bdem_AggregateOption::AllocationStrategy
// [ 1] bdem_AggregateOption::AllocationStrategyBit
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACROS
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
//                  TEST PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:

      case 1: {
        // --------------------------------------------------------------------
        // TESTING USAGE EXAMPLE
        //   The usage example provided in the component header file must
        //   compile, link, and run on all platforms as shown.  This usage
        //   test also happens to exhaustively test the entire component
        //   and is thus the only test in the suite.
        //
        // Plan:
        //   Incorporate usage example from header into driver, remove leading
        //   comment characters, and replace 'assert' with 'ASSERT'.
        //   Test each enumeration type by assigning a variable the value
        //   of each enumeration constant and verifying that the integral
        //   value of the variable after assignment is as expected.
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    return testStatus;
}

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2010
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
