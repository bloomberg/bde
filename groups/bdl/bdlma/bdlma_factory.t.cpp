// bdlma_factory.t.cpp                                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#include <bdlma_factory.h>

#include <bslim_testutil.h>

#include <bsls_assert.h>
#include <bsls_asserttest.h>

#include <bsl_cstdlib.h>     // 'atoi'
#include <bsl_cstring.h>     // 'memcpy'
#include <bsl_iostream.h>
#include <bsl_memory.h>

using namespace BloombergLP;
using namespace bsl;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// We are testing a pure protocol class as well as a set of overloaded
// operators.  We need to verify that (1) a concrete derived class compiles and
// links, and (2) that the overloaded new operator correctly forwards the call
// to the allocate method of the supplied deleter.
//-----------------------------------------------------------------------------
// [ 1] virtual ~bdlma::Factory();
// [ 1] virtual void delete(TYPE *instance) = 0;
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
//=============================================================================

// ============================================================================
//                     STANDARD BDE ASSERT TEST FUNCTION
// ----------------------------------------------------------------------------

namespace {

int testStatus = 0;

void aSsErT(bool condition, const char *message, int line)
{
    if (condition) {
        cout << "Error " __FILE__ "(" << line << "): " << message
             << "    (failed)" << endl;

        if (0 <= testStatus && testStatus <= 100) {
            ++testStatus;
        }
    }
}

}  // close unnamed namespace

// ============================================================================
//               STANDARD BDE TEST DRIVER MACRO ABBREVIATIONS
// ----------------------------------------------------------------------------

#define ASSERT       BSLIM_TESTUTIL_ASSERT
#define ASSERTV      BSLIM_TESTUTIL_ASSERTV

#define LOOP_ASSERT  BSLIM_TESTUTIL_LOOP_ASSERT
#define LOOP0_ASSERT BSLIM_TESTUTIL_LOOP0_ASSERT
#define LOOP1_ASSERT BSLIM_TESTUTIL_LOOP1_ASSERT
#define LOOP2_ASSERT BSLIM_TESTUTIL_LOOP2_ASSERT
#define LOOP3_ASSERT BSLIM_TESTUTIL_LOOP3_ASSERT
#define LOOP4_ASSERT BSLIM_TESTUTIL_LOOP4_ASSERT
#define LOOP5_ASSERT BSLIM_TESTUTIL_LOOP5_ASSERT
#define LOOP6_ASSERT BSLIM_TESTUTIL_LOOP6_ASSERT

#define Q            BSLIM_TESTUTIL_Q   // Quote identifier literally.
#define P            BSLIM_TESTUTIL_P   // Print identifier and value.
#define P_           BSLIM_TESTUTIL_P_  // P(X) without '\n'.
#define T_           BSLIM_TESTUTIL_T_  // Print a tab (w/o newline).
#define L_           BSLIM_TESTUTIL_L_  // current Line number

//=============================================================================
//                      CONCRETE DERIVED TYPES
//-----------------------------------------------------------------------------
class my_Obj {
};

class my_Factory : public bdlma::Factory<my_Obj> {
  // Test class used to verify protocol.
    int *d_destructorFlag_p;
    int  d_fun;  // holds code describing function:
                 //   + 1 delete
                 //   + 2 create
    my_Obj d_obj;
  public:
    my_Factory() : d_destructorFlag_p(0) { }

    my_Factory(int *destructorFlag) : d_destructorFlag_p(destructorFlag) { }

    virtual ~my_Factory() { if (d_destructorFlag_p) *d_destructorFlag_p = 1; }

    virtual my_Obj *createObject()         { d_fun = 2; return  &d_obj; }
    virtual void deleteObject(my_Obj *X)   { d_fun = 1; ASSERT(X == &d_obj); }

    int fun() const { return d_fun; }
        // Return descriptive code for the function called.
};

//=============================================================================
//                                   MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int  test    = argc > 1 ? atoi(argv[1]) : 0;
    bool verbose = argc > 2;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
///Usage
///-----
// Suppose that we would like to transfer an object between threads using
// 'bsl::shared_ptr'.  For the sake of discussion, the type of this object is
// 'my_Obj' and we will suppose that it is created using a concrete
// implementation of 'bdlma::Factory', say, 'my_Factory', the implementation of
// which is assumed:
//..
    my_Factory  factory;
    my_Obj     *object = factory.createObject();
//..
// Next we create a shared pointer passing to it 'object' and the 'factory'
// that was used to create 'object':
//..
    bsl::shared_ptr<my_Obj> handle(object, &factory);
//..
// Now the 'handle' can be passed to another thread or enqueued efficiently.
// Once the reference count of 'handle' goes to 0, 'object' is automatically
// deleted via the 'deleteObject' method of 'factory', which in turn will
// invoke the destructor of 'object'.  Note that since the type of the factory
// used to both create the object under management and to instantiate 'handle'
// is 'bdlma::Factory<my_Obj>', any kind of creator/deleter that implements
// this protocol can be passed.  Also note, on the downside, that the lifetime
// of 'factory' must be longer than the lifetime of all associated object
// instances.
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // PROTOCOL TEST:
        //   All we need to do is make sure that a subclass of the
        //   'bdlma::Factory' class compiles and links when all virtual
        //   functions are defined.
        //
        // Plan:
        //   Construct an object of a class derived from 'bdlma::Factory'.
        //   Cast a reference to the object to the base class 'bdlma::Factory'.
        //   Using the base class reference invoke both 'delete' method and
        //   verify that the correct implementations of the methods are called.
        //
        // Testing:
        //   virtual ~bdlma::Factory();
        //   virtual void deleteObject(my_Obj *object) = 0;
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;
        int destructorFlag = 0;
        my_Factory myA(&destructorFlag);
        bdlma::Factory<my_Obj>& a = myA;
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
        if (verbose) cout << "\tTesting '~bdlma::Factory'" << endl;
        {
            bdlma::Factory<my_Obj> *mX = new my_Factory(&destructorFlag);
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

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
