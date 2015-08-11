// btlscm_version.t.cpp                                               -*-C++-*-

#include <btlscm_version.h>

#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_iostream.h>


using namespace BloombergLP;
using namespace bsl;  // automatically added by script


static int testStatus = 0;

//=============================================================================
//                  MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    bool verbose = (argc > 2);
    bool veryVerbose = (argc > 3);


    switch (test) { case 0:
      case 1: {
        //--------------------------------------------------------------------
        // TEST USAGE EXAMPLE
        //
        // Concern:
        //   That the usage example in the user documentation compiles and
        //   runs as expected.
        //
        // Plan:
        //   Use the exact text of the usage example from the user
        //   documentation, but change uses of 'assert' to 'ASSERT'.
        //
        // Testing:
        //   USAGE EXAMPLE
        //--------------------------------------------------------------------

        if (verbose) bsl::printf("\nTEST USAGE EXAMPLE"
                                 "\n==================\n");

// If a program wants to display the version of BTL used to build the
// current executable, it can simply print the version string returned by
// 'btlscm::Version::version()':
//..
    if (verbose) bsl::printf("BTL version: %s\n", btlscm::Version::version());
//..
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
//      Copyright (C) Bloomberg L.P., 2015
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
