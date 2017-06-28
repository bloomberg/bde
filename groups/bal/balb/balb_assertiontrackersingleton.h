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

#ifndef INCLUDED_BALB_ASSERTIONTRACKER
#include <balb_assertiontracker.h>
#endif

#ifndef INCLUDED_BDLF_BIND
#include <bdlf_bind.h>
#endif

#ifndef INCLUDED_BDLF_PLACEHOLDER
#include <bdlf_placeholder.h>
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

extern "C" {
typedef void (*AssertionTrackerSingleton_BregCallback)(int *, int *, int *);
    // This is the type of a callback function that fetches three BREG values.
    // These are intended to be the ones found in
    // <bbit/201310/bbit_assert_trace_enable.h>
    // <bbit/201310/bbit_assert_trace_severity_level.h>
    // <bbit/201411/bbit_assert_max_callstack_count.h> and represent the legacy
    // BREG meant to control assertion logging.  (Note that linkage
    // specifications must appear at namespace scope, forcing the placement of
    // this declaration.)
}

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
    //: 1 The 'TRACKER' type must have a member type named
    //:   'ConfigurationCallback'.
    //:
    //: 2 A 'TRACKER' object can be constructed from three arguments, the first
    //:   of type 'bsls::Assert::Handler', the second a configuration handler
    //:   of type 'TRACKER::ConfigurationCallback', and the third of type
    //:   'bslma::Allocator*'.
    //:
    //: 2 A 'TRACKER' object has an 'assertionDetected' function taking three
    //:   arguments, the first two of type 'const char *' and the third of type
    //:   'int'.
  public:
    // PUBLIC TYPES
    typedef AssertionTrackerSingleton_BregCallback BregCallback;
        // Incorporate the BREG callback type into the class.

    typedef typename TRACKER::ConfigurationCallback ConfigurationCallback;
        // The type of the configuration callback of the 'TRACKER'.

    // PUBLIC CLASS METHODS
    static void bregCallbackAdapter(BregCallback callback, int *configure);
        // This function is an adapter that maps the BREG values provided by
        // the specified 'callback' to the specified 'configure' array used by
        // 'balb::AssertionTracker'.

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

    static TRACKER *
    singleton(ConfigurationCallback  configure      = ConfigurationCallback(),
              bslma::Allocator      *basicAllocator = 0);
        // Return a pointer to the 'TRACKER' singleton object that will be used
        // to track assertion failures.  When the singleton is created,
        // 'failTracker' will be installed as the assertion-handler function
        // for 'bsls::Assert'.  If 'failTracker' could not be installed, return
        // a null pointer instead.  Optionally specify a 'basicAllocator' used
        // to supply memory.  The currently installed failure handler, the
        // optionally specified 'configure' callback, and 'basicAallocator' are
        // supplied to the constructor of the singleton.

    static TRACKER *singleton(BregCallback      callback,
                              bslma::Allocator *basicAllocator = 0);
        // Return a pointer to the 'TRACKER' singleton object that will be used
        // to track assertion failures.  The created singleton will have a
        // configuration callback that adapts the specified 'callback' to the
        // form required by 'balb::AssertionTracker'.  Optionally specify a
        // 'basicAllocator' used to supply memory.  The currently installed
        // failure handler, the adapted 'callback', and 'basicAallocator' are
        // supplied to the constructor of the singleton.
};

                        // -------------------------------
                        // class AssertionTrackerSingleton
                        // -------------------------------

// CLASS METHODS
template <class TRACKER>
void AssertionTrackerSingleton<TRACKER>::bregCallbackAdapter(
                                                       BregCallback  callback,
                                                       int          *configure)
{
    int trace_enable, max_callstack_count, trace_severity_level;
    callback(&trace_enable, &max_callstack_count, &trace_severity_level);
    if (!trace_enable || !trace_severity_level) {
        configure[0] = 0;
    }
    else {
        configure[0] = -1;
        configure[1] = -1;
        configure[2] = max_callstack_count;
        // Align the BREG value with the 'bsls::LogSeverity::Enum' value.
        configure[3] = trace_severity_level - 1;
        configure[4] = 1;
    }
}

template <class TRACKER>
void AssertionTrackerSingleton<TRACKER>::failTracker(const char *text,
                                                     const char *file,
                                                     int         line)
{
    singleton()->assertionDetected(text, file, line);
}

template <class TRACKER>
TRACKER *AssertionTrackerSingleton<TRACKER>::singleton(
                                         ConfigurationCallback  configure,
                                         bslma::Allocator      *basicAllocator)
{
    static TRACKER *theSingleton_p;

    BSLMT_ONCE_DO
    {
        bsls::Assert::setFailureHandler(&failTracker);
        if (bsls::Assert::failureHandler() == &failTracker) {
            static TRACKER theSingleton(
                    bsls::Assert::failureHandler(), configure, basicAllocator);
            theSingleton_p = &theSingleton;
        }
    }

    return theSingleton_p;
}

template <class TRACKER>
TRACKER *
AssertionTrackerSingleton<TRACKER>::singleton(BregCallback      callback,
                                              bslma::Allocator *basicAllocator)
{
    using namespace bdlf::PlaceHolders;
    return singleton(bdlf::BindUtil::bind(bregCallbackAdapter, callback, _1));
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
