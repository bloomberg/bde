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

#ifndef INCLUDED_BSLS_LOG
#include <bsls_log.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
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
        ReportingCallback;

    enum ReportingFrequency {
        e_onNewLocation,    // Report once per distinct file/line pair
        e_onNewStackTrace,  // Report once per distinct stack trace
        e_onEachAssertion   // Report every assertion occurrence
    };

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
    ReportingCallback       d_reportingCallback;
    bsls::AtomicInt         d_reportingFrequency;

    // PRIVATE CREATORS
    AssertionTracker(const AssertionTracker&);             // = delete
        // Elided copy constructor.

    // PRIVATE MANIPULATORS
    AssertionTracker& operator=(const AssertionTracker&);  // = delete
        // Elided copy assignment operator.

  public:
    // CLASS METHODS
    static void logAssertion(int                         count,
                             const char                 *text,
                             const char                 *file,
                             int                         line,
                             const bsl::vector<void *>&  stack);
        // Log the specified 'count', 'text', 'file', 'line', and 'stack'
        // through the 'bsls::Log' facility.  This function is the default
        // callback function for reporting.

    static void reportAssertion(bsl::ostream               *out,
                                int                         count,
                                const char                 *text,
                                const char                 *file,
                                int                         line,
                                const bsl::vector<void *>&  stack);
        // Report the specified 'count', 'text', 'file', 'line', and 'stack' to
        // the specified stream 'out'.

    // CREATORS
    explicit AssertionTracker(bsls::Assert::Handler  fallbackHandler =
                                                bsls::Assert::failureHandler(),
                              bslma::Allocator      *basicAllocator  = 0);
        // Create an object of this type.  Optionally specify a
        // 'fallbackHandler' used to handle assertions that exceed configured
        // limits.  If 'fallbackHandler' is not given, the currently installed
        // failure handler is used.  Optionally specify a 'basicAllocator' used
        // to supply memory.  If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    // MANIPULATORS
    void operator()(const char *text, const char *file, int line);
        // Implement the required tracking behavior for the specified 'text',
        // 'file', and 'line'.  In typical use, this method will be invoked on
        // a singleton object of this type by an installed assertion failure
        // handler.

    void setMaxAssertions(int value);
        // Set the maximum number of assertions that this object will handle to
        // the specified 'value'.  If 'value' is negative, an unlimited number
        // of assertions can be handled.  If there is an assertion failure
        // beyond the limit, the assertion will be passed to the saved handler.

    void setMaxLocations(int value);
        // Set the maximum number of assertion locations that this object will
        // handle to the specified 'value'.  If 'value' is negative, an
        // unlimited number of locations can be handled.  If there is an
        // assertion failure beyond the limit, the assertion will be passed to
        // the saved handler.

    void setMaxStackTracesPerLocation(int value);
        // Set the maximum number of stack traces for a given location that
        // this object will handle to the specified 'value'.  If 'value' is
        // negative, an unlimited number of stack traces can be handled.  If
        // there is an assertion failure beyond the limit, the assertion will
        // be passed to the saved handler.

    void setReportingFrequency(ReportingFrequency frequency);
        // Set the frequency with which assertions are reported to the
        // specified 'frequency'.  If 'frequency' is 'e_onNewLocation', an
        // assertion will be reported if it is the first time that file/line
        // assertion location has been seen.  If 'frequency' is
        // 'e_onNewStackTrace', an assertion will be reported if it is the
        // first time a particular stack trace has been seen (that is, an
        // assertion at a particular file/line location may have multiple stack
        // traces because the function in which it appears is called from a
        // variety of call paths).  Finally, if 'frequency' is
        // 'e_onEachAssertion', every assertion occurrence will be reported.
        // This reporting frequency is initially 'e_onNewStackTrace'.

    void setReportingCallback(ReportingCallback cb);
        // Set the callback function invoked when an assertion occurs to the
        // specified 'cb'.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.

    ReportingCallback reportingCallback() const;
        // Return the callback functor used to report assertions.

    int maxAssertions() const;
        // Return the maximum number of assertions that this object will handle
        // or -1 if the number is unlimited.

    int maxLocations() const;
        // Return the maximum number of locations that this object will handle
        // or -1 if the number is unlimited.

    int maxStackTracesPerLocation() const;
        // Return the maximum number of stack traces for a given location that
        // this object will handle or -1 if the number is unlimited.

    ReportingFrequency reportingFrequency() const;
        // Return the frequency with which assertions are reported.

    bool onNewLocation() const;
        // Return whether the callback is invoked on each new assertion
        // location.

    bool onNewStackTrace() const;
        // Return whether the callback is invoked on each new assertion stack
        // trace;

    void reportAllStackTraces() const;
        // This method invokes the callback for each saved stack trace.
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
