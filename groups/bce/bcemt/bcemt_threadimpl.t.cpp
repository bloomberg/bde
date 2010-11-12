// bcemt_threadimpl.t.cpp             -*-C++-*-

#include <bcemt_threadimpl.h>

#include <iostream>
#include <cstring>  // strcmp()
#include <cstdlib>  // atoi()

using namespace BloombergLP;
using namespace std;

//=============================================================================
//                                TEST PLAN
//-----------------------------------------------------------------------------
//                                Overview
//                                --------
// This component only declares template classes and implements nothing.
// Therefore all that we can do is try to specialize the types for a dummy
// parameter.
//-----------------------------------------------------------------------------
//
// bcemt_AttributeImpl
// -------------------
// bcemt_ThreadUtilImpl
// --------------------
// bcemt_MutexImpl
// ---------------
// bcemt_RecursiveMutexImpl
// ------------------------
// bcemt_ConditionImpl
// -------------------
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {

struct TestThreadPolicy {};

template <>
class bcemt_AttributeImpl<TestThreadPolicy>
{
  public:
    bcemt_AttributeImpl<TestThreadPolicy>() {}
    ~bcemt_AttributeImpl<TestThreadPolicy>() {}
};

template <>
struct bcemt_ThreadUtilImpl<TestThreadPolicy>
{};

template <>
class bcemt_RecursiveMutexImpl<TestThreadPolicy>
{
  public:
    bcemt_RecursiveMutexImpl<TestThreadPolicy>() {}
    ~bcemt_RecursiveMutexImpl<TestThreadPolicy>() {}
};

template <>
class bcemt_MutexImpl<TestThreadPolicy>
{
  public:
    bcemt_MutexImpl<TestThreadPolicy>() {}
    ~bcemt_MutexImpl<TestThreadPolicy>() {}
};

template <>
class bcemt_ConditionImpl<TestThreadPolicy>
{
  public:
    bcemt_ConditionImpl<TestThreadPolicy>() {}
    ~bcemt_ConditionImpl<TestThreadPolicy>() {}
};

}

struct DeclarationTest {
    bcemt_AttributeImpl<TestThreadPolicy>      attribute;
    bcemt_ThreadUtilImpl<TestThreadPolicy>     threadUtil;
    bcemt_RecursiveMutexImpl<TestThreadPolicy> recursiveMutex;
    bcemt_MutexImpl<TestThreadPolicy>          mutex;
    bcemt_ConditionImpl<TestThreadPolicy>      condition;
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
      case 1: {
        // --------------------------------------------------------------------
        // Test Plan:
        //   Instantiate each template class with the test policy type and
        //   thereby instantiate the full specializations.  If this code
        //   compiles then that suffices as our test.  This verification is
        //   done by 'struct DeclarationTest' at file scope.
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Declaration Test" << endl
                          << "================" << endl;

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

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
