// btlso_event.t.cpp                                                  -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_event.h>

#include <bdlf_bind.h>                          // for testing only

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only
#include <bsls_platform.h>

#include <bsl_unordered_map.h>

#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_functional.h>
#include <bsl_iostream.h>
#include <bsl_utility.h>      // make_pair()

using namespace bsl;  // automatically added by script

using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
// The class under test is a very simple unconstrained two-attribute class.
// This test plan follows the standard approach for components implementing
// such classes.
//-----------------------------------------------------------------------------
// CREATORS
// [ 1] btlso::Event(const btlso::Event& original);
// [ 1] btlso::Event(const btlso::SocketHandle::Handle& handle,
//                   const btlso::EventType::Type       type);
// [ 1] ~btlso::Event();
//
// MANIPULATORS
// [ 1] void setHandle(const btlso::SocketHandle::Handle& handle);
// [ 1] void setType(btlso::EventType::Type type);
//
// ACCESSORS
// [ 1] btlso::SocketHandle::Handle handle() const;
// [ 1] btlso::EventType::Type type() const;
//
// FREE OPERATORS
// [ 1] bool operator==(const Event& lhs, const Event& rhs);
// [ 1] bool operator!=(const Event& lhs, const Event& rhs);
//-----------------------------------------------------------------------------
// [ 1] BREATHING TEST
// [ 2] USAGE EXAMPLE

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char *s, int i)
{
    if (c) {
        cout << "Error " << __FILE__ << "(" << i << "): " << s
             << " (failed)" << endl;
        if (0 <= testStatus && testStatus <= 100) ++testStatus;
    }
}

#define ASSERT(X) { aSsErT(!(X), #X, __LINE__); }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
    if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP2_ASSERT(I,J,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
                     << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
    if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"  \
                     << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) cout << #X " = " << (X) << endl; // Print identifier and value.
#define Q(X) cout << "<| " #X " |>" << endl;  // Quote identifier literally.
#define P_(X) cout << #X " = " << (X) << ", "<< flush; // P(X) without '\n'
#define L_ __LINE__                           // current Line number
#define NL "\n"

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

typedef btlso::Event        Obj;
typedef btlso::EventHash    Hash;

const btlso::SocketHandle::Handle   H0 = 0;
const btlso::SocketHandle::Handle   H1 = 1;

const btlso::EventType::Type ACCEPT  = btlso::EventType::e_ACCEPT;
const btlso::EventType::Type READ    = btlso::EventType::e_READ;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Implementing an event manager
/// - - - - - - - - - - - - - - - - - - - -
// First, we define a class, 'my_EventManager', that allows clients to register
// an asynchronous callback that is invoked when a certain event on a socket
// handle is complete.  As an example, objects of this class would allow users
// to register a read callback that is invoked when there is data available on
// a socket.
//
// The definition of the 'my_EventManager' class is provided below with
// non-relevant portions elided for brevity:
//..
    class my_EventManager {
        // This class allows clients to register asynchronous callbacks that
        // are invoked when certain events fire on a socket handle.
//
      public:
        // TYPES
        typedef bsl::function<void()> Callback;
//
      private:
        typedef bsl::unordered_map<btlso::Event,
                                   Callback,
                                   btlso::EventHash> CallbacksMap;
//
        // DATA
        CallbacksMap d_callbacks;
//
//      . . .
//
      public:
        // CREATORS
        my_EventManager(bslma::Allocator *basicAllocator = 0);
            // Create a 'my_EventManager' object.  Optionally specify a
            // 'basicAllocator' used to supply memory.  If 'basicAllocator' is
            // 0, the currently installed default allocator is used.
//
        ~my_EventManager();
//
        // MANIPULATORS
        int registerEvent(btlso::SocketHandle::Handle handle,
                          btlso::EventType::Type      eventType,
                          Callback&                   callback);
            // Register the specified 'callback' to be invoked when the
            // specified 'eventType' fires on the specified 'handle'.  Return
            // 0 on success and non-zero value otherwise.
//
//      . . .
    };
//..
// Then, we provide the function definitions for the 'my_EventManager' class:
//..
    // CREATORS
    my_EventManager::my_EventManager(bslma::Allocator *basicAllocator)
    : d_callbacks(basicAllocator)
    {
    }
//
    my_EventManager::~my_EventManager()
    {
    }
//
    // MANIPULATORS
    int my_EventManager::registerEvent(btlso::SocketHandle::Handle handle,
                                       btlso::EventType::Type      eventType,
                                       Callback&                   callback)
    {
//..
// Next, we create a 'btlso_Event' object
//..
        btlso::Event event(handle, eventType);
//..
// Then, we confirm if a callback is already registered for 'event'.  If so we
// simply update the callback:
//..
        CallbacksMap::iterator iter = d_callbacks.find(event);
        if (d_callbacks.end() != iter) {
            iter->second = callback;
            return 0;
        }
//..
// Next, we add the event to the callbacks map:
//..
        bool insertedEvent = d_callbacks.insert(
                                       bsl::make_pair(event, callback)).second;
        ASSERT(insertedEvent);

        return 0;
    }
//..
// Now, we specify a read callback:
//..
    void readCallback(btlso::SocketHandle::Handle handle)
        // Process data read from the specified 'handle'.
    {
        // . . .
    }
//..

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

    bslma::TestAllocator testAllocator(veryVeryVerbose);

    switch (test) { case 0:  // Zero is always the leading case.
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file compiles,
        //:   links, and runs as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, remove
        //:   leading comment characters, and replace 'assert' with 'ASSERT'.
        //:   (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "USAGE EXAMPLE" << endl
                          << "=============" << endl;

//..
// Finally, we construct a 'my_EventManager' object and register an event:
//..
        my_EventManager             manager;
//
        btlso::SocketHandle::Handle handle;

        // Open the socket 'handle'
//
        // . . .
//
        btlso::EventType::Type    eventType = btlso::EventType::e_READ;
        my_EventManager::Callback callback  = bdlf::BindUtil::bind(
                                                                 &readCallback,
                                                                 handle);
//
        const int rc = manager.registerEvent(handle, eventType, callback);
        ASSERT(0 == rc);
//..
      } break;
      case 1: {
        // --------------------------------------------------------------------
        // BREATHING TEST
        //   This case exercises (but does not fully test) basic functionality.
        //
        // Concerns:
        //: 1 The class is sufficiently functional to enable comprehensive
        //:   testing in subsequent test cases.
        //
        // Plan:
        //: 1 Perform and ad-hoc test of the primary modifiers and accessors.
        //
        // Testing:
        //   BREATHING TEST
        // --------------------------------------------------------------------

        if (verbose) cout << endl
                          << "Basic Attribute Test" << endl
                          << "====================" << endl;

        Obj mX1(H0, ACCEPT);  const Obj& X1 = mX1;
        Obj mY1(H0, ACCEPT);  const Obj& Y1 = mY1;
        Obj mZ1(H0, ACCEPT);  const Obj& Z1 = mZ1; // Z1 is the control

        if (verbose) { P_(X1.handle()); P_(X1.type()); }

        if (verbose) cout << "\nCheck ctor. " << endl;

        ASSERT(H0     == X1.handle());
        ASSERT(ACCEPT == X1.type());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        if (verbose) cout << "\tSetting default values explicitly." <<endl;

        mX1.setHandle(H0);
        mX1.setType(ACCEPT);

        ASSERT(H0      == X1.handle());
        ASSERT(ACCEPT  == X1.type());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Z1 == Y1));          ASSERT(0 == (Z1 != Y1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\tTesting set/get methods."
                          << "\n\t\tChange attribute 0." << endl;

        mX1.setHandle(H1);

        if (verbose) { P_(X1.handle()); P_(X1.type()); }

        ASSERT(H1      == X1.handle());
        ASSERT(ACCEPT  == X1.type());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(1 == (C == X1));       ASSERT(0 == (C != X1));
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1.setHandle(H0);
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

        // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

        if (verbose) cout << "\t\tChange attribute 1." << endl;

        mX1.setType(READ);

        if (verbose) { P_(X1.handle()); P_(X1.type()); }

        ASSERT(H0      == X1.handle());
        ASSERT(READ    == X1.type());

        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(0 == (X1 == Z1));          ASSERT(1 == (X1 != Z1));
        ASSERT(0 == (Z1 == X1));          ASSERT(1 == (Z1 != X1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));
        {
            Obj C(X1);
            ASSERT(1 == (C == X1));       ASSERT(0 == (C != X1));
        }

        mY1 = X1;
        ASSERT(1 == (Y1 == Y1));          ASSERT(0 == (Y1 != Y1));
        ASSERT(1 == (Y1 == X1));          ASSERT(0 == (Y1 != X1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1.setType(ACCEPT);
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(0 == (Y1 == Z1));          ASSERT(1 == (Y1 != Z1));

        mX1 = mY1 = Z1;
        ASSERT(1 == (X1 == X1));          ASSERT(0 == (X1 != X1));
        ASSERT(1 == (X1 == Z1));          ASSERT(0 == (X1 != Z1));
        ASSERT(1 == (Y1 == Z1));          ASSERT(0 == (Y1 != Z1));

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
