// btlso_defaulteventmanager.t.cpp                                    -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_defaulteventmanager.h>

#include <btlso_platform.h>
#include <btlso_timemetrics.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslma_testallocatorexception.h>       // for testing only

#include <bsl_iostream.h>
#include <bsl_c_stdio.h>
#include <bsl_c_stdlib.h>

using namespace BloombergLP;
using namespace bsl;  // automatically added by script

//=============================================================================
//                              TEST PLAN
//-----------------------------------------------------------------------------
//                              OVERVIEW
//                              --------
//
// This component provides an interface that is implemented by all the concrete
// event managers.  However, this component itself does not implement any of
// the methods.  So we just verify that the typedefs are properly hooked up and
// an instance of default event manager can be created.  We also verify that an
// instance of specialized event manager can be created.
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
// [ 1] BREATHING TEST

//=============================================================================
//                    STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------

static int testStatus = 0;
void aSsErT(int c, const char *s, int i)
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

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I,X) { \
   if (!(X)) { cout << #I << ": " << I << "\n"; aSsErT(1, #X, __LINE__); }}

#define LOOP2_ASSERT(I,J,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " \
              << J << "\n"; aSsErT(1, #X, __LINE__); } }

#define LOOP3_ASSERT(I,J,K,X) { \
   if (!(X)) { cout << #I << ": " << I << "\t" << #J << ": " << J << "\t" \
              << #K << ": " << K << "\n"; aSsErT(1, #X, __LINE__); } }

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

int          globalVerbose = 0,
         globalVeryVerbose = 0,
     globalVeryVeryVerbose = 0;

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------
int main(int argc, char *argv[]) {
    int test = argc > 1 ? atoi(argv[1]) : 0;
    int verbose = argc > 2;                 globalVerbose = verbose;
    int veryVerbose = argc > 3;         globalVeryVerbose = veryVerbose;
    int veryVeryVerbose = argc > 4; globalVeryVeryVerbose = veryVeryVerbose;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;

    switch (test) { case 0:  // Zero is always the leading case.
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

        if (verbose) cout << endl << "USAGE EXAMPLE" << endl
                                  << "=============" << endl;

///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Creating a default event manager
///- - - - - - - - - - - - - - - - - - - - - -
// In the following usage example we show how to create an instance of a
// default event manager.  First, we need to include this file (shown here for
// completeness):
//..
//  #include <btlso_defaulteventmanager.h>
//..
// Second, create a 'btlso::TimeMetrics' to give to the event manager:
//..
    btlso::TimeMetrics metrics(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
                               btlso::TimeMetrics::e_CPU_BOUND);
//..
// Now, create a default event manager that uses this 'metrics':
//..
    typedef btlso::Platform::DEFAULT_POLLING_MECHANISM PollMechanism;
    btlso::DefaultEventManager<PollMechanism> eventManager(&metrics);
//..
// Note that the time metrics is optional.  Using the same component, we can
// create an event manager that uses a particular mechanism (for example,
// '/dev/poll') as follows:
//..
//  btlso::DefaultEventManager<btlso::Platform::DEVPOLL> fastEventManager;
//..
// Note that '/dev/poll' is available only on Solaris and this instantiation
// fails (at compile time) on other platforms.
//..
      } break;
      case 1: {
        // -----------------------------------------------------------------
        // BREATHING TEST
        //    Verify that
        //      o  a default instance can be created
        //      o  a specialized instance can be created
        //
        // Testing:
        //   Create an object of this event manager under test.
        // -----------------------------------------------------------------

        if (verbose) cout << endl << "BREATHING TEST" << endl
                                  << "==============" << endl;

        bslma::TestAllocator testAllocator(veryVeryVerbose);

        if (veryVerbose) cout << "\tCreating a default instance." << endl;
        {
            btlso::TimeMetrics metrics(
                                      btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
                                      btlso::TimeMetrics::e_CPU_BOUND,
                                      &testAllocator);
            btlso::DefaultEventManager<> mX(&metrics, &testAllocator);
            btlso::DefaultEventManager<> mY(&metrics);
            btlso::DefaultEventManager<> mZ;
        }

        if (veryVerbose) cout << "\tCreating another instance." << endl;
        {
            btlso::TimeMetrics metrics(
                                   btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
                                   btlso::TimeMetrics::e_CPU_BOUND,
                                   &testAllocator);

            typedef btlso::Platform::DEFAULT_POLLING_MECHANISM PollMech;

            btlso::DefaultEventManager<PollMech> mX(&metrics, &testAllocator);
            btlso::DefaultEventManager<PollMech> mY(&metrics);
            btlso::DefaultEventManager<PollMech> mZ;
        }

        if (veryVerbose) cout << "\tCreating an instance explicitly."
                              << endl;
        {
            btlso::DefaultEventManager<btlso::Platform::SELECT>
                                    mX((btlso::TimeMetrics*)0, &testAllocator);
            btlso::DefaultEventManager<btlso::Platform::SELECT>
                                                    mY((btlso::TimeMetrics*)0);
            btlso::DefaultEventManager<btlso::Platform::SELECT> mZ;
        }
      } break;

      default: {
        cerr << "WARNING: CASE `" << test << "' NOT FOUND." << endl;
        testStatus = -1;
      } break;
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
