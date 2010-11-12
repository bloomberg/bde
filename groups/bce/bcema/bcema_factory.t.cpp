// bcema_factory.t.cpp        -*-C++-*-

#include <bcema_factory.h>

#include <bsl_cstdlib.h>     // atoi()
#include <bsl_cstring.h>     // memcpy()
#include <bsl_iostream.h>
using namespace BloombergLP;
using namespace bsl;  // automatically added by script



//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class as well as a set of overloaded
// operators.  We need to verify that (1) a concrete derived class compiles
// and links, and (2) that the overloaded new operator correctly forwards
// the call to the allocate method of the supplied deleter.
//-----------------------------------------------------------------------------
// [ 1] virtual ~bcema_Factory();
// [ 1] virtual void delete(TYPE *instance) = 0;
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
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
};

class my_Factory : public bcema_Factory<my_Obj> {
  // Test class used to verify protocol.
    int *d_destructorFlag_p;
    int  d_fun;  // holds code describing function:
                 //   + 1 delete
                 //   + 2 create
    my_Obj d_obj;
  public:
    my_Factory(int *destructorFlag) : d_destructorFlag_p(destructorFlag) { }
    virtual ~my_Factory() { *d_destructorFlag_p = 1; }

    virtual my_Obj *createObject()         { d_fun = 2; return  &d_obj; }
    virtual void deleteObject(my_Obj *X)   { d_fun = 1; ASSERT(X == &d_obj); }

    int fun() const { return d_fun; }
        // Return descriptive code for the function called.
};

//=============================================================================
//                      MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   All we need to do is make sure that a subclass of the
        //   'bcema_Factory' class compiles and links when all virtual
        //   functions are defined.
        //
        // Plan:
        //   Construct an object of a class derived from 'bcema_Factory'.
        //   Upcast a reference to the object to the base class
        //   'bcema_Factory'.  Using the base class reference invoke both
        //   'delete' method and verify that the correct implementations of the
        //   methods are called.
        //
        // Testing:
        //   virtual ~bcema_Factory();
        //   virtual void deleteObject(my_Obj *object) = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;
        int destructorFlag = 0;
        my_Factory myA(&destructorFlag);
        bcema_Factory<my_Obj>& a = myA;
        my_Obj *X = (my_Obj*)NULL;

        if (verbose) cout << "\tTesting 'createObject'" << endl;
        {
            X = a.createObject();
            ASSERT(2 == myA.fun()); ASSERT(X);
        }

        if (verbose) cout << "\tTesting 'deleteObject'" << endl;
        {
            a.deleteObject(X);  ASSERT(1 == myA.fun());
        }
        if (verbose) cout << "\tTesting '~bcema_Factory'" << endl;
        {
            bcema_Factory<my_Obj> *mX = new my_Factory(&destructorFlag);
            delete mX;
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
