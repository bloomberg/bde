// bslmf_isarithmetic.t.cpp                                           -*-C++-*-
#include <bslmf_isarithmetic.h>

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
        ASSERT(is_arithmetic<bool>::value);
        ASSERT(is_arithmetic<char>::value);
        ASSERT(is_arithmetic<char volatile>::value);
        ASSERT(is_arithmetic<signed char>::value);
        ASSERT(is_arithmetic<int>::value);
        ASSERT(is_arithmetic<int const>::value);

        ASSERT(is_arithmetic<float>::value);
        ASSERT(is_arithmetic<float const>::value);
        ASSERT(is_arithmetic<double>::value);
        ASSERT(is_arithmetic<double volatile>::value);
        ASSERT(is_arithmetic<long double>::value);

        ASSERT(!is_arithmetic<int *>::value);
        ASSERT(!is_arithmetic<void *>::value);
        ASSERT(!is_arithmetic<void>::value);
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
