// bbescm_version.t.cpp    -*-C++-*-

#include <bbescm_version.h>

#include <stdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace std;
using namespace bsl;  // automatically added by script


static int testStatus = 0;

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
