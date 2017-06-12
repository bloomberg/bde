// balb_assertiontracker.h                                            -*-C++-*-

#ifndef INCLUDED_BALB_ASSERTIONTRACKER
#define INCLUDED_BALB_ASSERTIONTRACKER

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif

//@PURPOSE: Provide a means to track where failed assertion occur.
//
//@CLASSES:
//   balb::AssertionTracker: capture information about failed assertions
//
//@DESCRIPTION: The 'balb::AssertionTracker' component keeps track of failed
// assertions and the stack traces leading to them.  It provides means by which
// such collection can be limited so as not to overwhelm processes where
// assertions fail frequently, and ways to examine the accumulated data.
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
///Example 1:
/// - - - - -

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BDLF_BIND
#include <bdlf_bind.h>
#endif

#ifndef INCLUDED_BSLMA_ALLOCATOR
#include <bslma_allocator.h>
#endif

#ifndef INCLUDED_BSLMT_MUTEX
#include <bslmt_mutex.h>
#endif

#ifndef INCLUDED_BSLMT_THREADUTIL
#include <bslmt_threadutil.h>
#endif

#ifndef INCLUDED_BSLS_ASSERT
#include <bsls_assert.h>
#endif

#ifndef INCLUDED_BSLS_ATOMIC
#include <bsls_atomic.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_IOSTREAM
#include <bsl_ostream.h>
#endif

#ifndef INCLUDED_BSL_UTILITY
#include <bsl_utility.h>
#endif

#ifndef INCLUDED_BSL_UNORDERED_MAP
#include <bsl_unordered_map.h>
#endif

#ifndef INCLUDED_BSL_VECTOR
#include <bsl_vector.h>
#endif

namespace BloombergLP {
namespace balb {

                            // ======================
                            // class AssertionTracker
                            // ======================

class AssertionTracker {
  private:
    // PRIVATE TYPES
    typedef const char                              *Text;
    typedef const char                              *File;
    typedef int                                      Line;
    typedef bsl::pair<Text, bsl::pair<File, Line> >  AssertionLocation;
    typedef void                                    *Address;
    typedef bsl::vector<Address>                     StackTrace;
    typedef bsl::unordered_map<StackTrace, int>      AssertionCounts;
    typedef bsl::unordered_map<AssertionLocation, AssertionCounts>
        TrackingData;

  public:
    // PUBLIC TYPES
    typedef bsl::function<
        void(int, const char *, const char *, int, const bsl::vector<void *>&)>
        Callback;

  private:
    // PRIVATE DATA
    bsls::Assert::Handler   d_fallbackHandler;  // handler when limits exceeded
    bslma::Allocator       *d_allocator_p;      // allocator
    bsls::AtomicInt         d_maxAssertions;              // configured limit
    bsls::AtomicInt         d_maxLocations;               // configured limit
    bsls::AtomicInt         d_maxStackTracesPerLocation;  // configured limit
    bsls::AtomicInt         d_assertionCount;   // number of assertions seen
    TrackingData            d_trackingData;     // store of assertions seen
    bslmt::ThreadUtil::Key  d_recursionCheck;   // thread-local data key to
                                                // prevent recursive invocation
    mutable bslmt::Mutex    d_mutex;            // mutual exclusion lock
    Callback                d_callback;
    bsls::AtomicBool        d_onEachAssertion;
    bsls::AtomicBool        d_onNewLocation;
    bsls::AtomicBool        d_onNewStackTrace;

    // PRIVATE CREATORS
    AssertionTracker(const AssertionTracker&);             // = delete
        // Elided copy constructor.

    // PRIVATE MANIPULATORS
    AssertionTracker& operator=(const AssertionTracker&);  // = delete
        // Elided copy assignment operator.

  public:
    // CLASS METHODS
    static void failTracker(const char *text, const char *file, int line);
        // Track the failed assertion described by the specified 'text' that
        // occurred at the specified 'file' and 'line'.  The behavior is
        // undefined unless the lifetime of the pointers 'text' and 'file'
        // exceeds that of the 'AssertionTracker' singleton.  (In normal usage
        // those pointers will be string literals, trivially satisfying this.)
        // This function is intended to be installed as the assertion-handler
        // function for 'bsls::Assert'.  Note that unlike proper handlers, this
        // handler returns to its caller and may trigger warnings on such
        // behavior within 'bsls::Assert'.

    static void reportAssertion(bsl::ostream               *out,
                                int                         count,
                                const char                 *text,
                                const char                 *file,
                                int                         line,
                                const bsl::vector<void *>&  stack);
        // Report the specified 'count', 'text', 'file', 'line', and 'stack' to
        // the specified stream 'out'.  This function, with 'out' bound to
        // 'bsl::cout', is the default callback function for reporting.

    static AssertionTracker *
    singleton(bsls::Assert::Handler  fallbackHandler = bsls::Assert::failAbort,
              bslma::Allocator      *basicAllocator  = 0);
        // Return a pointer to the 'AssertionTracker' singleton object that
        // will be used to track assertion failures.  When the singleton is
        // created, 'failTracker' will be installed as the assertion-handler
        // function for 'bsls::Assert'.  If 'failTracker' could not be
        // installed, return a null pointer instead.  Optionally specify a
        // 'fallbackHandler' to be used if assertions beyond configured limits
        // occur.  On the first call when the singleton is created, the
        // behavior is undefined unless 'fallbackHandler' is specified and not
        // null.  On subsequent calls 'fallbackHandler' has no effect.
        // Optionally specify a 'basicAllocator' used to supply memory.  If
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.  The allocator used by this object is set on the first call
        // when the singleton is created.  On subsequent calls 'basicAllocator'
        // has no effect.

    // CREATORS
    AssertionTracker(bsls::Assert::Handler  fallbackHandler,
                     bslma::Allocator      *basicAllocator);
        // Create an object of this type.  The specified 'fallbackHandler' is
        // used to handle assertions that exceed configured limits.  The
        // specified 'basicAllocator' is used to supply memory.  Note that the
        // 'singleton' class method creates a special instance of this class.

    // MANIPULATORS
    void callback(Callback cb);
        // Set the callback function invoked when an assertion occurs and a
        // callback invocation is requested to the specified 'cb'.

    void failTrackerImpl(const char *text, const char *file, int line);
        // Implement the required tracking behavior for the specified 'text',
        // 'file', and 'line', that will have been passed to this method of the
        // singleton by the installed 'failTracker' handler.

    void maxAssertions(int value);
        // Set the maximum number of assertions that this object will handle to
        // the specified 'value'.  If 'value' is negative, an unlimited number
        // of assertions can be handled.  If there is an assertion failure
        // beyond the limit, the assertion will be passed to the saved handler.

    void maxLocations(int value);
        // Set the maximum number of assertion locations that this object will
        // handle to the specified 'value'.  If 'value' is negative, an
        // unlimited number of locations can be handled.  If there is an
        // assertion failure beyond the limit, the assertion will be passed to
        // the saved handler.

    void maxStackTracesPerLocation(int value);
        // Set the maximum number of stack traces for a given location that
        // this object will handle to the specified 'value'.  If 'value' is
        // negative, an unlimited number of stack traces can be handled.  If
        // there is an assertion failure beyond the limit, the assertion will
        // be passed to the saved handler.

    void onEachAssertion(bool value);
        // Set whether the callback is invoked on each assertion occurrence to
        // the specified 'value'.

    void onNewLocation(bool value);
        // Set whether the callback is invoked on each new assertion location
        // to the specified 'value'.

    void onNewStackTrace(bool value);
        // Set whether the callback is invoked on each new assertion stack
        // trace to the specified 'value'.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.

    Callback callback() const;
        // Return the callback functor used to report assertions.

    virtual void iterateAll() const;
        // This method calls 'onNewStackTrace' for each saved stack trace.

    int maxAssertions() const;
        // Return the maximum number of assertions that this object will handle
        // or -1 if the number is unlimited.

    int maxLocations() const;
        // Return the maximum number of locations that this object will handle
        // or -1 if the number is unlimited.

    int maxStackTracesPerLocation() const;
        // Return the maximum number of stack traces for a given location that
        // this object will handle or -1 if the number is unlimited.

    bool onEachAssertion() const;
        // Return whether the callback is invoked on each assertion occurrence.

    bool onNewLocation() const;
        // Return whether the callback is invoked on each new assertion
        // location.

    bool onNewStackTrace() const;
        // Return whether the callback is invoked on each new assertion stack
        // trace;
};

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
