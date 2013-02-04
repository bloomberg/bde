// bsldoc_glossary.t.cpp                                              -*-C++-*-
#include <bsldoc_glossary.h>

#include <cstdlib>     // 'atoi'
#include <iostream>

using namespace std;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
// This is a documentation component with no implementation and no tests.

static int testStatus = 0;

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    switch (test) {
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
//      Copyright (C) Bloomberg L.P., 2011
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
