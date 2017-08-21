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
// ********************************* WARNING *********************************
// IT IS CORPORATE POLICY THAT IN PRODUCTION CODE, ASSERTION FAILURE HANDLERS
// MUST NOT RETURN TO THEIR CALLERS.  THIS COMPONENT EXISTS IN CONTRAVENTION OF
// THAT POLICY, AND IS MEANT TO BE USED ONLY WITH SPECIAL PERMISSION OF
// MANAGEMENT, FOR THE PURPOSE OF ASCERTAINING THE CORRECTNESS OF FRESHLY
// WRITTEN OR ENABLED ASSERTIONS THAT ARE FAILING.
// ********************************* WARNING *********************************
//
// The singleton object will generally, and by default, be an object of type
// 'balb::AssertionTracker' but a different type may be used, as shown in the
// usage example below.
//
// 'balb::AssertionTrackerSingleton' is used by invoking its
// 'createAndInstallSingleton' static member function.  Doing so replaces the
// assert-handler installed in the 'bsls::Assert' system with the
// 'balb::AssertionTrackerSingleton::failTracker' static method and creates a
// singleton tracker object.  Should an assertion fail, the 'bals::Assert'
// system will invoke 'failTracker' and that in turn will forward the failure
// report to the singleton object.
//
///Configuration Callbacks
///-----------------------
// The tracker object constructor is passed a configuration callback function
// that is optionally supplied to 'createAndInstallSingleton()'.  The tracker
// object may (and 'balb::AssertionTracker' does) choose to invoke the
// configuration callback each time an assertion occurs so that control over
// assertion handling is dynamic - external settings may be used to inform the
// tracker how it should respond to assertions.  The tracker object constructor
// is also passed the assert-handler that was in place prior to its being
// replaced by 'failTracker()'.  Based on configuration, the tracker object may
// choose to allow this previous handler to handle an assertion failure.
//
// There is an overload of 'createAndInstallSingleton()' that takes a variant
// form of configuration callback intended to read the values of a trio of
// BREG values (see 'AssertionTrackerSingleton_BregCallback' below), for
// compatibility with a previous version of assertion tracking.
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
//  AssertionCounter *ac_p = balb::AssertionTrackerSingleton<AssertionCounter>
//      ::createAndInstallSingleton();
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

#ifndef INCLUDED_BSLMA_DEFAULT
#include <bslma_default.h>
#endif

#ifndef INCLUDED_BSLMT_ONCE
#include <bslmt_once.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
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

template <class TRACKER = AssertionTracker>
class AssertionTrackerSingleton {
    // This class template implements the management of a 'TRACKER' singleton
    // object that is used to monitor and report assertion failures.  Note that
    // this class template is meant to be used in an environment that has
    // freshly created or enabled assertions that are not yet known to be
    // assuredly correct and triggered only on "real" errors.  In such a case,
    // using an assertion monitor prevents programs from dying on triggered
    // assertions while data about them is being gathered.  See the warning
    // above about obtaining permission to use this component in production.
    //
    // For most purposes, 'TRACKER' should be 'balb::AssertionTracker'.  The
    // requirements on the 'TRACKER" class are
    //: 1 The 'TRACKER' type must have a member type named
    //:   'ConfigurationCallback'.
    //:
    //: 2 A 'TRACKER' object can be constructed from three arguments, the first
    //:   of type 'bsls::Assert::Handler', the second a configuration handler
    //:   of type 'TRACKER::ConfigurationCallback', and the third of type
    //:   'bslma::Allocator*'.
    //:
    //: 3 A 'TRACKER' object has an 'assertionDetected' function taking three
    //:   arguments, the first two of type 'const char *' and the third of type
    //:   'int'.
  private:
    // PRIVATE CLASS DATA
    static bsls::AtomicPointer<TRACKER> s_singleton_p;  // 'TRACKER' singleton

  public:
    // PUBLIC TYPES
    typedef AssertionTrackerSingleton_BregCallback BregCallback;
        // Incorporate the BREG callback type into the class.

    typedef typename TRACKER::ConfigurationCallback ConfigurationCallback;
        // The type of the configuration callback of the 'TRACKER'.

    // PUBLIC CLASS METHODS
    static void bregCallbackAdapter(BregCallback callback,
                                    int * /* maxAssertions             */,
                                    int * /* maxLocations              */,
                                    int * /* maxStackTracesPerLocation */,
                                    int * /* reportingSeverity         */,
                                    int * /* reportingFrequency        */);
        // This function is an adapter that maps the BREG values provided by
        // the specified 'callback' to the five integer parameters used by
        // 'balb::AssertionTracker'.

    static TRACKER *createAndInstallSingleton(
                    ConfigurationCallback  configure = ConfigurationCallback(),
                    bslma::Allocator      *basicAllocator = 0);
        // Create a 'TRACKER' singleton object that will be used to track
        // assertion failures, replace the installed assertion-handler function
        // for 'bsls::Assert' with
        // 'AssertionTrackerSingleton<TRACKER>::failTracker', and return a
        // pointer to the newly-created singleton.
        //
        // Three arguments are passed to the constructor of the 'TRACKER'
        // object - the previously-installed assertion-handler function that
        // is replaced by 'failTracker', a configuration callback object, and
        // an allocator pointer.
        //
        // Optionally specify 'configure', an object of type
        // 'TRACKER::ConfigurationCallback' to be passed as the configuration
        // callback 'TRACKER' constructor argument.  If 'configure' is not
        // provided, a default-constructed object of type
        // 'TRACKER::ConfigurationCallback' is passed instead.
        //
        // Optionally specify a 'basicAllocator' used to supply memory for the
        // 'TRACKER' singleton that will be passed as the allocator 'TRACKER'
        // constructor argument.  If 'basicAllocator' is not provided, a null
        // pointer is passed instead.

    static TRACKER *
    createAndInstallSingleton(BregCallback      callback,
                              bslma::Allocator *basicAllocator = 0);
        // Create a 'TRACKER' singleton object that will be used to track
        // assertion failures, replace the installed assertion-handler function
        // for 'bsls::Assert' with
        // 'AssertionTrackerSingleton<TRACKER>::failTracker', and return a
        // pointer to the newly-created singleton.
        //
        // Three arguments are passed to the constructor of the 'TRACKER'
        // object - the previously-installed assertion-handler function that
        // is replaced by 'failTracker', a configuration callback object, and
        // an allocator pointer.
        //
        // The configuration callback object passed to the constructor adapts
        // the specified 'callback' to the callback type required by
        // 'balb::AssertionTracker'.  (That is, it is a function object that
        // invokes 'bregCallbackAdapter' with its first argument bound to
        // 'callback'.)
        //
        // Optionally specify a 'basicAllocator' used to supply memory for the
        // 'TRACKER' singleton that will be passed as the allocator 'TRACKER'
        // constructor argument.  If 'basicAllocator' is not provided, a null
        // pointer is passed instead.

    static void failTracker(const char *text, const char *file, int line);
        // Forward the specified 'text', 'file', and 'line' corresponding to a
        // failed assertion to the singleton object managed by this class via
        // function-call operator.  The behavior is undefined unless the
        // singleton has been created and installed (via one of the
        // 'createAndInstallSingleton' methods) and unless the lifetime of the
        // pointers 'text' and 'file' exceeds that of the singleton.  (In
        // normal usage those pointers will be string literals, trivially
        // satisfying this.)  This function is intended to be installed as the
        // assertion-handler function for 'bsls::Assert'.  Note that this
        // handler, unlike proper handlers, returns to its caller, potentially
        // triggering warnings within 'bsls::Assert'.  Such a handler may be
        // used in production code only with management permission; see the
        // warning above.

    static TRACKER *singleton();
        // Return a pointer to the 'TRACKER' singleton object that will be used
        // to track assertion failures if the singleton object has been
        // created, and a null pointer otherwise.
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                        // -------------------------------
                        // class AssertionTrackerSingleton
                        // -------------------------------

// CLASS DATA
template <class TRACKER>
bsls::AtomicPointer<TRACKER>
AssertionTrackerSingleton<TRACKER>::s_singleton_p;

// CLASS METHODS
template <class TRACKER>
void AssertionTrackerSingleton<TRACKER>::bregCallbackAdapter(
                                       BregCallback  callback,
                                       int          *,
                                       int          *,
                                       int          *maxStackTracesPerLocation,
                                       int          *reportingSeverity,
                                       int          *)
{
    int trace_enable, max_callstack_count, trace_severity_level;
    callback(&trace_enable, &trace_severity_level, &max_callstack_count);
    if (!trace_enable) {
        *maxStackTracesPerLocation = 0;
    }
    else {
        *maxStackTracesPerLocation = max_callstack_count;
        *reportingSeverity         = trace_severity_level;
    }
}

template <class TRACKER>
TRACKER *AssertionTrackerSingleton<TRACKER>::createAndInstallSingleton(
                                         ConfigurationCallback  configure,
                                         bslma::Allocator      *basicAllocator)
{
    BSLMT_ONCE_DO
    {
        static TRACKER theSingleton(
            bsls::Assert::failureHandler(),
            configure,
            bslma::Default::globalAllocator(basicAllocator));
        s_singleton_p = &theSingleton;
        bsls::Assert::setFailureHandler(&failTracker);
    }

    return s_singleton_p;
}

template <class TRACKER>
TRACKER *AssertionTrackerSingleton<TRACKER>::createAndInstallSingleton(
                                              BregCallback      callback,
                                              bslma::Allocator *basicAllocator)
{
    using namespace bdlf::PlaceHolders;
    return createAndInstallSingleton(
        bdlf::BindUtil::bind(
            bregCallbackAdapter, callback, _1, _2, _3, _4, _5),
        basicAllocator);
}

template <class TRACKER>
void AssertionTrackerSingleton<TRACKER>::failTracker(const char *text,
                                                     const char *file,
                                                     int         line)
{
    s_singleton_p->assertionDetected(text, file, line);
}

template <class TRACKER>
TRACKER *AssertionTrackerSingleton<TRACKER>::singleton()
{
    return s_singleton_p;
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
