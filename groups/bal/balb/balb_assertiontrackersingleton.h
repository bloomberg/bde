// balb_assertiontrackersingleton.h                                   -*-C++-*-

#ifndef INCLUDED_BALB_ASSERTIONTRACKERSINGLETON
#define INCLUDED_BALB_ASSERTIONTRACKERSINGLETON

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif

//@PURPOSE: Provide a means to install an assertion tracker singleton.
//
//@CLASSES:
//   balb::AssertionTrackerSingleton: manage an assertion tracker singleton
//
//@DESCRIPTION: The 'balb::AssertionTrackerSingleton' component creates and
// provides access to a singleton object that is responsible for accumulating
// and reporting on a sequence of failed assertions.
//
///Thread Safety
///-------------
// This component is thread-safe and thread-enabled: it is safe to access and
// manipulate multiple distinct instances from different threads, and it is
// safe to access and manipulate a single shared instance from different
// threads.
//
///Usage
///-----
// This section illustrates intended use of this component.
//
///Example 1: Count Number of Assertions Triggered
///- - - - - - - - - - - - - - - - - - - - - - - -
// We wish to count how many times assertions trigger, doing no other handling
// and allowing the program to continue running (note that this is normally
// forbidden in production).  We can use 'AssertionTrackerSingleton' to set up
// an assertion handler to do this.
//
// First, we create a class to do the counting.
//..
//  class AssertionCounter {
//      // PRIVATE DATA
//      int d_assertion_counter;  // Number of assertions seen.
//
//    public:
//      // PUBLIC CREATORS
//      explicit AssertionCounter(bsls::Assert::Handler   = 0,
//                                bslma::Allocator      * = 0);
//          // Create an 'AssertionCounter' object.  We ignore the fallback
//          // handler and optional allocator parameters.
//
//      // PUBLIC MANIPULATORS
//      void operator()(const char *, const char *, int);
//          // Function called when assertion failure occurs.  We ignore the
//          // text, file, and line parameters.
//
//      // PUBLIC ACCESSORS
//      int getAssertionCount() const;
//          // Return the number of assertions we have seen.
//  };
//..
// Then, we implement the member functions of the 'AssertionCounter' class.
//..
//  AssertionCounter::AssertionCounter(bsls::Assert::Handler,
//                                     bslma::Allocator *)
//  : d_assertion_counter(0)
//  {
//  }
//
//  void AssertionCounter::operator()(const char *, const char *, int)
//  {
//      ++d_assertion_counter;
//  }
//
//  int AssertionCounter::getAssertionCount() const
//  {
//      return d_assertion_counter;
//  }
//..
// Next, we set up an instance of this class to be the installed assertion
// using 'AssertionTrackerSingleton'.  Note that this needs to be done early in
// 'main()' before any other handlers are installed.  If another handler has
// already been installed, a null pointer will be returned, and we assert that
// this does not happen.
//..
//  AssertionCounter *ac_p =
//      balb::AssertionTrackerSingleton<AssertionCounter>::singleton();
//  assert(ac_p);
//..
// Finally, we will trigger some assertions and verify that we are counting
// them correctly.
//..
//  BSLS_ASSERT(0 && "assertion 1");
//  BSLS_ASSERT(0 && "assertion 2");
//  assert(ac_p->getAssertionCount() == 2);
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMT_ONCE
#include <bslmt_once.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

namespace BloombergLP {
namespace balb {

                        // ===============================
                        // class AssertionTrackerSingleton
                        // ===============================

template <class TRACKER>
class AssertionTrackerSingleton {
    // This class template implements the management of a 'TRACKER' singleton
    // object that is used to monitor and report assertion failures.  Note that
    // this is meant to be used in an environment that has freshly inserted or
    // exposed assertions that are not yet known to be assuredly correct and
    // triggered only on "real" errors.  In such a case, using an assertion
    // monitor prevents programs from dying on triggered assertions while data
    // about them is being gathered.
    //
    // The requirements on the 'TRACKER" class are
    //: 1 A 'TRACKER' object is constructible given two arguments, the first of
    //:   type 'bsls::Assert::Handler', and the second of type
    //:   'bslma::Allocator*'.
    //:
    //: 2 A 'TRACKER' object may be invoked as a function and passed three
    //:   arguments, the first two of type 'const char *' and the third of type
    //:   'int'.
  public:
    // PUBLIC CLASS METHODS
    static void failTracker(const char *text, const char *file, int line);
        // Forward the specified 'text', 'file', and 'line' corresponding to a
        // failed assertion to the singleton object managed by this class via
        // function-call operator.  The behavior is undefined unless the
        // lifetime of the pointers 'text' and 'file' exceeds that of the
        // singleton.  (In normal usage those pointers will be string literals,
        // trivially satisfying this.)  This function is intended to be
        // installed as the assertion-handler function for 'bsls::Assert'.
        // Note that unlike proper handlers, this handler returns to its caller
        // and may trigger warnings on such behavior within 'bsls::Assert'.

    static TRACKER *singleton(bslma::Allocator *basicAllocator = 0);
        // Return a pointer to the 'TRACKER' singleton object that will be used
        // to track assertion failures.  When the singleton is created,
        // 'failTracker' will be installed as the assertion-handler function
        // for 'bsls::Assert'.  If 'failTracker' could not be installed, return
        // a null pointer instead.  Optionally specify a 'basicAllocator' used
        // to supply memory.  The currently installed failure handler and this
        // allocator are supplied to the constructor of the singleton.
};

                        // -------------------------------
                        // class AssertionTrackerSingleton
                        // -------------------------------

// CLASS METHODS
template <class TRACKER>
void AssertionTrackerSingleton<TRACKER>::failTracker(const char *text,
                                                     const char *file,
                                                     int         line)
{
    (*singleton())(text, file, line);
}

template <class TRACKER>
TRACKER *
AssertionTrackerSingleton<TRACKER>::singleton(bslma::Allocator *basicAllocator)
{
    static TRACKER *theSingleton_p;

    BSLMT_ONCE_DO
    {
        bsls::Assert::setFailureHandler(&failTracker);
        if (bsls::Assert::failureHandler() == &failTracker) {
            static TRACKER theSingleton(bsls::Assert::failureHandler(),
                                        basicAllocator);
            theSingleton_p = &theSingleton;
        }
    }

    return theSingleton_p;
}

}  // close package namespace
}  // close enterprise namespace

#endif

// ----------------------------------------------------------------------------
// Copyright 2017 Bloomberg Finance L.P.
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
