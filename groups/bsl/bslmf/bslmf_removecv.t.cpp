// bslmf_removecv.t.cpp                                               -*-C++-*-
#include <bslmf_removecv.h>

#include <bslmf_issame.h>

#include <iostream>
#include <cstdlib>

using namespace bsl;
using namespace BloombergLP;

using std::cout;
using std::cerr;
using std::endl;
using std::atoi;

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

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        ASSERT((is_same<remove_cv<int const>::type, int>::value));
        ASSERT((is_same<remove_cv<int * const>::type, int *>::value));
        ASSERT((is_same<remove_cv<const int *>::type, const int *>::value));

        ASSERT((is_same<remove_cv<int volatile>::type, int>::value));
        ASSERT((is_same<remove_cv<int * volatile>::type, int *>::value));
        ASSERT((is_same<remove_cv<volatile int *>::type, volatile int *>::value));

        ASSERT((is_same<remove_cv<int const volatile>::type, int>::value));
        ASSERT((is_same<remove_cv<int * const volatile>::type, int *>::value));
        ASSERT((is_same<remove_cv<const volatile int *>::type, const volatile int *>::value));
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
