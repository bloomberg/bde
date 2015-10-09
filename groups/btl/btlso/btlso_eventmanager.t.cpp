// btlso_eventmanager.t.cpp                                           -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_eventmanager.h>

#include <btlso_event.h>

#include <bsls_timeinterval.h>
#include <bdlt_currenttime.h>
#include <bdlf_bind.h>

#include <bsl_iostream.h>

#include <bsl_c_stdlib.h>     // atoi
#include <bsl_functional.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

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

//-----------------------------------------------------------------------------

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
// We are testing a pure protocol class.  We need to verify that a concrete
// derived class compiles and links and that the example compiles and works as
// advertised.
//
// Note that some types in the Test Plan below are abbreviations for types
// qualified by 'btlso::SocketHandle::'.
//-----------------------------------------------------------------------------
// [ 1] ~btlso::EventManager()
// [ 1] int dispatch(const bsls::TimeInterval& timeout, int flags = 0);
// [ 1] int dispatch(int flags = 0);
// [ 1] int registerSocketEvent(const Handle&, const Type, const Callback&);
// [ 1] void deregisterSocketEvent(const Handle& handle, Type event);
// [ 1] void deregisterSocket(const btlso::SocketHandle::Handle& handle);
// [ 1] void deregisterAll();
// [ 1] bool hasLimitedSocketCapacity() const;
// [ 1] int numSocketEvents();
// [ 1] int numEvents();
// [ 1] int isRegistered(const Handle& handle, const Type event);
//-----------------------------------------------------------------------------
// [ 1] PROTOCOL TEST - Make sure derived class compiles and links.
// [ 2] USAGE TEST - Make sure usage example compiles and works as advertised.
//=============================================================================

// ============================================================================
//                        GLOBAL TYPEDEFS FOR TESTING
// ----------------------------------------------------------------------------

typedef btlso::SocketHandle::Handle Handle;
typedef btlso::EventType::Type      EventType;

//=============================================================================
//                              USAGE EXAMPLE
//-----------------------------------------------------------------------------

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Monitoring a set of sockets
///- - - - - - - - - - - - - - - - - - -
int monitorSet(btlso::SocketHandle::Handle      *sockets,
               int                               numSockets,
               void (*function)(btlso::SocketHandle::Handle),
               btlso::EventManager              *manager)
    // Invoke the specified 'function' when incoming data is detected on a
    // subset of the specified 'sockets' of the specified 'numSockets' length.
    // Use the specified 'manager' for monitoring.  Return a positive number of
    // callbacks invoked on success and a non-positive value otherwise.  The
    // behavior is undefined unless 0 < numSockets.
{
    ASSERT(sockets);
    ASSERT(manager);
    ASSERT(0 < numSockets);

    // Create a callback associated with 'function' for each socket and
    // register this callback to be invoked when associated socket is
    // ready for reading.

    for (int i = 0; i < numSockets; ++i) {
        bsl::function<void()> callback(bdlf::BindUtil::bind(function,
                                                            sockets[i]));

        const int rc = manager->registerSocketEvent(sockets[i],
                                                    btlso::EventType::e_READ,
                                                    callback);

        if (rc) {

            // For cleanliness, when a registration fails, we will all
            // previous registrations

            while(--i >= 0) {
                manager->deregisterSocket(sockets[i]);
            }

            return -1;                                                // RETURN
        }
    }
    return manager->dispatch();
}

static void dummyFunction(btlso::SocketHandle::Handle handle)
{

}

//=============================================================================
//                      CONCRETE DERIVED TYPE
//-----------------------------------------------------------------------------

class my_EventManager : public btlso::EventManager {
    // Test class used to verify protocol.

    int *d_fun; // For storing code for last called function:
                //  1: ~my_EventManager()
                //  2: dispatch with timeout
                //  3: dispatch without timeout
                //  4: registerSocketEvent
                //  5: deregisterSocketEvent
                //  6: deregisterSocket
                //  7: deregisterAll
                //  8: numSocketEvents
                //  9: numEvents
                // 10: isRegistered
                // 11: hasLimitedSocketCapacity

  public:
    my_EventManager(int *fun) : d_fun(fun) { }
    ~my_EventManager()
        { *d_fun = 1; }

    virtual int dispatch(const bsls::TimeInterval& timeout, int flags)
        { *d_fun = 2; return -1; }

    virtual int dispatch(int flags)
        { *d_fun = 3; return -1; }

    virtual int registerSocketEvent(const Handle&                 handle,
                                    const EventType               event,
                                    const EventManager::Callback& callback)
        { *d_fun = 4; return -1; }

    virtual void deregisterSocketEvent(const Handle& handle, EventType event)
        { *d_fun = 5; }

    virtual int deregisterSocket(const Handle& handle)
        { *d_fun = 6; return 0; }

    virtual void deregisterAll()
        { *d_fun = 7; }

    virtual int numSocketEvents(const Handle& handle) const
        { *d_fun = 8; return 0; }

    virtual int numEvents() const
        { *d_fun = 9; return 0; }

    virtual int isRegistered(const Handle& handle, const EventType event) const
        { *d_fun = 10; return 0; }

    virtual bool hasLimitedSocketCapacity() const
        { *d_fun = 11; return true; }
};

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;
    // int veryVerbose = argc > 3;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:
      case 2: {
        // --------------------------------------------------------------------
        // USAGE EXAMPLE
        //
        // Concerns:
        //: 1 The usage example provided in the component header file must
        //:   compile, link, and run as shown.
        //
        // Plan:
        //: 1 Incorporate usage example from header into test driver, replace
        //:   leading comment characters with spaces, replace 'assert' with
        //:   'ASSERT', and insert 'if (veryVerbose)' before all output
        //:   operations.  (C-1)
        //
        // Testing:
        //   USAGE EXAMPLE
        // --------------------------------------------------------------------

        if (verbose) cout << endl << "USAGE TEST" << endl
                                  << "==========" << endl;
        enum { NUM_SOCKETS = 3 };

        btlso::SocketHandle::Handle sockets[NUM_SOCKETS];

        int function = -1;

        my_EventManager mEm(&function);

        btlso::EventManager& mX = mEm;
        ASSERT(-1 == monitorSet(sockets, NUM_SOCKETS, dummyFunction, &mX));

      } break;
      case 1: {
        // -----------------------------------------------------------------
        // PROTOCOL TEST:
        //   We need to make sure that a subclass of the
        //   'btlso::EventManager' class compiles and links when
        //   all virtual functions are defined.
        //
        // Testing:
        //   ~btlso::EventManager()
        //   int dispatch(const bsls::TimeInterval& timeout, int flags = 0);
        //   int dispatch(int flags = 0);
        //   void registerSocketEvent(const Handle&, const Type, callback);
        //   void deregisterSocketEvent(handle, event);
        //   void deregisterSocket(const btlso::SocketHandle::Handle& handle);
        //   void deregisterAll();
        //   bool hasLimitedSocketCapacity() const;
        //   bool isRegistered() const;
        //   int numSocketEvents(const btlso::SocketHandle::Handle& handle);
        //   int numEvents();
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "PROTOCOL TEST" << endl
                                  << "=============" << endl;

        int function = -1;

        {
            my_EventManager               mEm(&function);
            btlso::EventManager&          m = mEm;
            btlso::SocketHandle::Handle   h;
            btlso::EventType::Type        e = btlso::EventType::Type(1);
            btlso::EventManager::Callback cb;
            bsls::TimeInterval            ti;

            if (verbose) cout << "\nTesting dispatch function." << endl;

            int val = m.dispatch(ti, 1);
            ASSERT(2 == function); ASSERT(-1 == val);

            val = m.dispatch(1);
            ASSERT(3 == function); ASSERT(-1 == val)

            if (verbose) cout << "\nTesting register functions." << endl;

            val = m.registerSocketEvent(h, e, cb);
            ASSERT(4 == function); ASSERT(-1 == val);

            if (verbose) cout << "\nTesting deregister functions." << endl;

            m.deregisterSocketEvent(h, e);
            ASSERT(5 == function);

            m.deregisterSocket(h);
            ASSERT(6 == function);

            m.deregisterAll();
            ASSERT(7 == function);

            if (verbose) cout << "\nTesting accessor functions." << endl;

            m.numSocketEvents(h);
            ASSERT(8 == function);

            m.numEvents();
            ASSERT(9 == function);

            m.isRegistered(h, e);
            ASSERT(10 == function);

            m.hasLimitedSocketCapacity();
            ASSERT(11 == function);
        }

        // Destructor should have been invoked.

        ASSERT(1 == function);
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
