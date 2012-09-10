// bslmf_isfloatingpoint.t.cpp                                        -*-C++-*-
#include <bslmf_isfloatingpoint.h>

#include <iostream>
#include <cstdlib>

using namespace std;
using namespace bsl;
using namespace BloombergLP;

//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;

static void aSsErT(int c, const char *s, int i) {
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << "    (failed)" << endl;
        if (testStatus >= 0 && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    int veryVerbose = argc > 3;

    (void) verbose;
    (void) veryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        ASSERT(!is_floating_point<bool>::value);
        ASSERT(!is_floating_point<int>::value);
        ASSERT(!is_floating_point<void>::value);
        ASSERT(!is_floating_point<int *>::value);
        ASSERT(is_floating_point<float>::value);
        ASSERT(is_floating_point<float const>::value);
        ASSERT(is_floating_point<double>::value);
        ASSERT(is_floating_point<double volatile>::value);
        ASSERT(is_floating_point<long double>::value);
      } break;
      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      }
    }

    if (testStatus > 0) {
        cerr << "Error, non-zero test status = "
             << testStatus << "." << endl;
    }

    return testStatus;
}
