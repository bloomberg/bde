// btlso_defaulteventmanagerimpl.t.cpp                                -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------


#include <btlso_defaulteventmanagerimpl.h>

#include <bsls_platform.h>
#include <btlso_platform.h>
#include <btlso_timemetrics.h>

#include <bslma_testallocator.h>                // for testing only
#include <bslmf_issame.h>                       // for testing only

#include <bsl_c_stdlib.h>     // atoi()
#include <bsl_iostream.h>

using namespace bsl;  // automatically added by script
using namespace BloombergLP;

//=============================================================================
//                             TEST PLAN
//-----------------------------------------------------------------------------
//                              Overview
//                              --------
//
// This component provides a forward declaration for the 'DefaultEventManager'
// class that specifies the default event manager on a certain platform.
// However, this component itself does not provide any functionality by itself.
// So we just verify that the typedefs are properly hooked up and an instance
// of default event manager can be created.
//-----------------------------------------------------------------------------
// [ 2] USAGE EXAMPLE
// [ 1] BREATHING TEST

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
//=============================================================================
//                  STANDARD BDE ASSERT TEST MACRO
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
//                  GLOBAL TYPES FOR TESTING
//-----------------------------------------------------------------------------

namespace BloombergLP {
namespace btlso {

// Dummy definitions of DefaultEventManager to allow testing that the default
// event manager on each platform is instantiated correctly.

template <class POLLING_MECHANISM>
class DefaultEventManager {
};

template <>
class DefaultEventManager<btlso::Platform::SELECT> {
  public:
    // TYPES
    typedef btlso::Platform::SELECT TYPE;

    // CREATORS
    explicit
    DefaultEventManager(TimeMetrics      *timeMetric     = 0,
                        bslma::Allocator *basicAllocator = 0)
        {
            (void) timeMetric; (void) basicAllocator;
        }

};

#if defined(BSLS_PLATFORM_OS_AIX)

template <>
class DefaultEventManager<btlso::Platform::POLLSET> {
  public:
    // TYPES
    typedef btlso::Platform::POLLSET TYPE;

    // CREATORS
    explicit
    DefaultEventManager(TimeMetrics      *timeMetric     = 0,
                        bslma::Allocator *basicAllocator = 0)
        {
            (void) timeMetric; (void) basicAllocator;
        }

};

#elif defined(BSLS_PLATFORM_OS_SOLARIS)

template <>
class DefaultEventManager<btlso::Platform::DEVPOLL> {
  public:
    // TYPES
    typedef btlso::Platform::DEVPOLL TYPE;

    // CREATORS
    explicit
    DefaultEventManager(TimeMetrics      *timeMetric     = 0,
                        bslma::Allocator *basicAllocator = 0)
        {
            (void) timeMetric; (void) basicAllocator;
        }

};

#elif defined(BSLS_PLATFORM_OS_LINUX)

template <>
class DefaultEventManager<btlso::Platform::EPOLL> {
  public:
    // TYPES
    typedef btlso::Platform::EPOLL TYPE;

    // CREATORS
    explicit
    DefaultEventManager(TimeMetrics      *timeMetric     = 0,
                        bslma::Allocator *basicAllocator = 0)
        {
            (void) timeMetric; (void) basicAllocator;
        }

};

#elif defined(BSLS_PLATFORM_OS_CYGWIN)  \
   || defined(BSLS_PLATFORM_OS_FREEBSD) \
   || defined(BSLS_PLATFORM_OS_DARWIN)  \
   || defined(BSLS_PLATFORM_OS_HPUX)

template <>
class DefaultEventManager<btlso::Platform::POLL> {
  public:
    // TYPES
    typedef btlso::Platform::POLL TYPE;

    // CREATORS
    explicit
    DefaultEventManager(TimeMetrics      *timeMetric     = 0,
                        bslma::Allocator *basicAllocator = 0)
        {
            (void) timeMetric; (void) basicAllocator;
        }

};

#endif

}
}

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    int test            = argc > 1 ? atoi(argv[1]) : 0;
    int verbose         = argc > 2;
    int veryVerbose     = argc > 3;
    int veryVeryVerbose = argc > 4;

    cout << "TEST " << __FILE__ << " CASE " << test << endl;;

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
// In the following usage example we show how to create an default instance of
// a default event manager.  First, we need to include this file (shown here
// for completeness):
//..
//  #include <btlso_defaulteventmanagerimpl.h>
//..
// Second, create a 'btlso::TimeMetrics' to give to the event manager:
//..
    btlso::TimeMetrics metrics(btlso::TimeMetrics::e_MIN_NUM_CATEGORIES,
                               btlso::TimeMetrics::e_CPU_BOUND);
//..
// Now, create a default event manager that uses this 'metrics':
//..
    btlso::DefaultEventManager<> eventManager(&metrics);
//..
// Note that the time metrics is optional.
//..
      } break;
      case 1: {
        // -----------------------------------------------------------------
        // BREATHING TEST
        //    Verify that
        //    o  a default instance can be created
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

            ASSERT(true == (bsl::is_same<
                             btlso::Platform::DEFAULT_POLLING_MECHANISM,
                            btlso::DefaultEventManager<>::TYPE>::value));
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
