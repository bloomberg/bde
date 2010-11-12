// bcema_defaultdeleter.t.cpp        -*-C++-*-
#include <bcema_defaultdeleter.h>
#include <bcema_testallocator.h>
#include <bcema_sharedptr.h>

#include <bslma_default.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// [TBD]
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

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

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define PS(X) cout << #X " = \n" << (X) << endl; // Print identifier and value.
#define T_()  cout << "\t" << flush;          // Print a tab (w/o newline)

//=============================================================================
//                      CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------
class my_Obj {
    int *d_destructorFlag_p;
  public:
    my_Obj(int *destructorFlag) : d_destructorFlag_p(destructorFlag) {
        *d_destructorFlag_p = 0;
    }
    ~my_Obj() { *d_destructorFlag_p = 1; }
};

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // TEST USAGE
        // --------------------------------------------------------------------

        bcema_TestAllocator ta;

        int destructorFlag;
        my_Obj *object = new(ta) my_Obj(&destructorFlag);

        bcema_DefaultDeleter<my_Obj> deleter(&ta);

        bcema_SharedPtr<my_Obj> handle(object, &deleter, &ta);
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // TESTING bcema_DefaultDeleter
        // Concerns:
        //   o bcema_DefaultDeleter works with global 'new' and 'delete'
        //   o bcema_DefaultDeleter works with user-installed allocator
        // Plan:
        //   Create 'my_Obj' using different allocators
        // Testing:
        //   bcema_DefaultDeleter(bcem_Allocator *);
        //   deleteObject(my_Obj *);
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "TESTING bcema_DefaultDeleter" << endl
                                  << "============================" << endl;

        if (verbose) cout << "\tUsing global operators 'new' and 'delete"
                          << endl;
        {
            int destructorFlag;
            my_Obj *object = new (*bslma_Default::defaultAllocator())
                                                       my_Obj(&destructorFlag);
            bcema_DefaultDeleter<my_Obj> deleter;
            bcema_Deleter<my_Obj> *base = &deleter;
            base->deleteObject(object);
            ASSERT(1 == destructorFlag);
        }
        if (verbose) cout << "\tUsing user-installed allocator" << endl;
        {
            bcema_TestAllocator testAllocator(veryVeryVerbose);
            bslma_Allocator *allocator = &testAllocator;
            int destructorFlag;
            my_Obj *object =
                new (testAllocator) my_Obj(&destructorFlag);
            bcema_DefaultDeleter<my_Obj> deleter(allocator);
            deleter.deleteObject(object);
            ASSERT(1 == destructorFlag);
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
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
