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
//@SEE_ALSO: balb_assertiontrackersingleton
//
//@DESCRIPTION: The 'balb::AssertionTracker' component keeps track of failed
// assertions and the stack traces leading to them.  It provides means by which
// such collection can be limited so as not to overwhelm processes where
// assertions fail frequently, and ways to examine the accumulated data.
//
// ********************************* WARNING *********************************
// IT IS CORPORATE POLICY THAT IN PRODUCTION CODE, ASSERTION FAILURE HANDLERS
// MUST NOT RETURN TO THEIR CALLERS.  THIS COMPONENT EXISTS IN CONTRAVENTION OF
// THAT POLICY, AND IS MEANT TO BE USED ONLY WITH SPECIAL PERMISSION OF
// MANAGEMENT, FOR THE PURPOSE OF ASCERTAINING THE CORRECTNESS OF FRESHLY
// WRITTEN OR ENABLED ASSERTIONS THAT ARE FAILING.
// ********************************* WARNING *********************************
//
// For purposes of this component, an assertion is considered to have a
// location, represented by a triple of assertion text, source file name, and
// source line number, and a stack trace, consisting of a vector of addresses
// representing the call stack from 'main()' to the assertion location.  (Note
// that an assertion that appears to have a single location and stack trace in
// the source code may nevertheless appear as having multiple stack traces when
// they occur.  This is because compiler optimizations such as inlining, loop
// unrolling, and frame-pointer suppression can generate duplicates of the
// assertion line or disguise the call stack.)
//
// This component serves as a means whereby assertion failures can be noted,
// tracked, and reported, but does not itself provide the facility for being
// attached to the global 'bsls::Assert' system (and indeed the usage example
// below shows how this component may be used independently of that).  Rather,
// this component considers an assertion to have occurred when its
// 'assertionDetected' method is invoked.  The separate
// 'balb::AssertionTrackerSingleton' component mediates between this component
// and the 'bsls::Assert' system.
//
// The behavior of this component involves the actions of three function object
// callbacks:
//: o Fallback Handler
//:   When the number of assertions exceeds configured parameters (or in the
//:   unlikely case that one of these callbacks itself fails an assertion), the
//:   assertion is reported to this handler instead.
//:
//:   By default, the failure handler installed in the 'bsls::Assert' system is
//:   used for this.
//:
//: o Reporting Callback
//:   When an assertion failure occurs and the component configuration requires
//:   the failure to be reported, the reporting callback is invoked and passed
//:   information about the assertion, including the number of times it has
//:   occurred and the stack trace (as a vector of addresses, without symbolic
//:   information) to the place where it was triggered.  The reporting callback
//:   is also used to report the accumulated set of recorded assertion traces
//:   when this is requested through a call to 'reportAllRecordedStackTraces'.
//:
//:   By default, reporting is done using the platform default message handler
//:   of the 'bsls::Log' system.
//:
//: o Configuration Callback
//:   Each time an assertion failure occurs, this component first calls the
//:   configuration callback and resets the component configuration based on
//:   its results.  This callback can, for example, query 'BREG' settings and
//:   thereby allow reporting to be dynamically enabled or disabled.
//:
//:   By default, configuration is left unchanged.
//
// Configuration of this component is mediated through five properties:
//: o 'maxAssertions' (default -1)
//:   The maximum number of assertion occurrences this object will handle,
//:   unlimited if set to -1.  If more assertions occur, they will be
//:   reported to the fallback handler and their stack traces will not be
//:   stored.
//:
//: o 'maxLocations' (default -1)
//:   The maximum number of assertion locations (i.e., pairs of file name
//:   and line number) this object will handle, unlimited if set to -1.  If
//:   assertions at more locations occur, they will be reported to the
//:   fallback handler and their stack traces will not be stored.
//:
//: o 'maxStackTracesPerLocation' (default -1)
//:   The maximum number of different stack traces that will be stored per
//:   assertion location, unlimited if -1.  (A stack trace is the path of
//:   function calls leading to the assertion location.  A given assertion
//:   location may be reached through different paths.)  If more stack traces
//:   for a location occur, the assertion will be reported to the fallback
//:   handler and the stack traces will not be stored.
//:
//: o 'reportingSeverity' (default 'bsls::LogSeverity::e_FATAL')
//:   This severity value is passed to the reporting callback.
//:
//: o 'reportingFrequency' (default 'e_onNewStackTrace')
//:   This parameter controls whether an assertion occurrence is reported via
//:   the reporting callback or whether it is only counted but not reported.
//:   The possible values are:
//:   o 'e_onEachAssertion'
//:     All assertion occurrences are reported.
//:
//:   o 'e_onNewStackTrace'
//:     The first time a new stack trace is seen, it is reported.  Subsequent
//:     instances of the same stack trace are counted but not reported.
//:
//:   o 'e_onNewLocation'
//:     The first time a new location (i.e., a pair of file and line) is seen,
//:     it is reported.  Subsequent instances of the same location, even if
//:     they have different stack traces, are counted (by stack trace) but not
//:     reported.
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
///Example 1: Historic Broken Assertions
///- - - - - - - - - - - - - - - - - - -
// We have a function that has been running without assertions enabled.  We
// have reason to believe that the assertions will trigger if enabled, but that
// the effects of the violations will mostly be benignly erased (which is why
// the problems have not been detected so far).  We want to enable the
// assertions and fix the places that are causing them to trigger, but we do
// not want the program to abort each time an assertion occurs because it will
// slow down the task of information gathering, and because we need to gather
// the information in production.  We can use 'bsls::AssertionTracker' for
// this purpose.
//
// First, we will place a local static 'AssertionTracker' object ahead of the
// function we want to instrument, and create custom assertion macros just for
// that function.
//..
//  namespace {
//  balb::AssertionTracker theTracker;
//  #define TRACK_ASSERT(condition) do { if (!(condition)) { \  /*Squash Warn*/
//  theTracker.assertionDetected(#condition, __FILE__, __LINE__); } } while (0)
//  }  // close unnamed namespace
//..
// Then, we define the function to be traced, and use the modified assertions.
//..
//   void foo(int percentage)
//       // Receive the specified 'percentage'.  The behavior is undefined
//       // unless '0 < percentage < 100'.
//   {
//       TRACK_ASSERT(  0 < percentage);
//       TRACK_ASSERT(100 > percentage);
//   }
//..
// Next, we create some uses of the function that may trigger the assertions.
//..
//   void useFoo()
//       // Trigger assertions by passing invalid values to 'foo'.
//   {
//       for (int i = 0; i < 100; ++i) {
//           foo(i);
//       }
//       for (int i = 100; i > 0; --i) {
//           foo(i);
//       }
//   }
//..
// Finally, we prepare to track the assertion failures.  We will have the
// tracker report into a string stream object so that we can examine it.  We
// configure the tracker, trigger the assertions, and verify that they have
// been correctly discovered.
//..
//   bsl::ostringstream os;
//   theTracker.setReportingCallback(
//       bdlf::BindUtil::bind(balb::AssertionTracker::reportAssertion,
//                            &os, _1, _2, _3, _4, _5, _6));
//   theTracker.setReportingFrequency(
//                                  balb::AssertionTracker::e_onEachAssertion);
//   useFoo();
//   bsl::string report = os.str();
//   assert(report.npos != report.find("0 < percentage"));
//   assert(report.npos != report.find("100 > percentage"));
//..

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

#ifndef INCLUDED_BSLS_LOGSEVERITY
#include <bsls_logseverity.h>
#endif

#ifndef INCLUDED_BSL_FUNCTIONAL
#include <bsl_functional.h>
#endif

#ifndef INCLUDED_BSL_MAP
#include <bsl_map.h>
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
    typedef const char *Text;  // assertion text type

    typedef const char *File;  // source file name type

    typedef int Line;          // source line number type

    typedef bsl::pair<Text, bsl::pair<File, Line> >  AssertionLocation;
                               // assertion location type, consisting of its
                               // text, source file, and line number

    typedef void *Address;     // type of one element of a stack trace

    typedef bsl::vector<Address> StackTrace;
                               // type of a stack trace, consisting of a
                               // sequence of addresses

    // The following two types can use unordered_map instead of map once
    // DRQS 102622326 (compilation failure on AIX) is fixed.
    typedef bsl::map<StackTrace, int> AssertionCounts;
                               // a type for keeping a count per stack trace

    typedef bsl::map<AssertionLocation, AssertionCounts> TrackingData;
                               // a type for keeping a set of stack traces and
                               // their counts per assertion location

  public:
    // PUBLIC TYPES
    typedef bsl::function<void(int *maxAssertions,
                               int *maxLocations,
                               int *maxStackTracesPerLocation,
                               int *reportingSeverity,
                               int *reportingFrequency)>
        ConfigurationCallback;
                               // a configuration callback is a functor that
                               // receives pointers to five integer parameters
                               // that it can modify

    typedef bsl::function<void(int                         ,
                               int                         ,
                               const char                 *,
                               const char                 *,
                               int                         ,
                               const bsl::vector<void *>&  )>
                                           ReportingCallback;
                               // a reporting callback is a functor that
                               // receives a count of how many times an
                               // assertion stack trace has occurred, followed
                               // by severity, assertion text, file name, line
                               // number, and stack trace

    // This enumeration defines the possible values for how often assertions
    // are reported.
    enum ReportingFrequency {
        e_onNewLocation,    // Report once per distinct file/line pair
        e_onNewStackTrace,  // Report once per distinct stack trace
        e_onEachAssertion   // Report every assertion occurrence
    };

  private:
    // PRIVATE DATA
    bsls::Assert::Handler   d_fallbackHandler;
        // handler invoked when assertions occur beyond configured limits

    bslma::Allocator       *d_allocator_p;
        // allocator used by this object

    bsls::AtomicInt         d_maxAssertions;
        // configured limit of assertions that will be processed

    bsls::AtomicInt         d_maxLocations;
        // configured limit of assertion locations that will be processed

    bsls::AtomicInt         d_maxStackTracesPerLocation;
        // configured limit of per-location stack traces that will be processed

    bsls::AtomicInt         d_reportingSeverity;
        // configured severity, passed to reporting callback

    bsls::AtomicInt         d_assertionCount;
        // number of assertions seen, including unprocessed ones

    TrackingData            d_trackingData;
        // database tracking all processed assertions so far seen

    bslmt::ThreadUtil::Key  d_recursionCheck;
        // key for thread-local data, used to prevent recursive invocation of
        // the assertion handler if a callback itself triggers an assertion

    mutable bslmt::Mutex    d_mutex;
        // lock used to serialize concurrent access to the callback members
        // 'd_configurationCallback' and 'd_reportingCallback' and to the
        // accumulated assertion data 'd_trackingData' and 'd_assertionCount'

    ConfigurationCallback   d_configurationCallback;
        // callback invoked to reconfigure reporting parameters

    ReportingCallback       d_reportingCallback;
        // callback invoked to report an assertion

    bsls::AtomicInt         d_reportingFrequency;
        // configured reporting frequency

    // PRIVATE CREATORS
    AssertionTracker(const AssertionTracker&);             // = delete
        // Elided copy constructor.

    // PRIVATE MANIPULATORS
    AssertionTracker& operator=(const AssertionTracker&);  // = delete
        // Elided copy assignment operator.

  public:
    // CLASS METHODS
    static void preserveConfiguration(int * /* maxAssertions             */,
                                      int * /* maxLocations              */,
                                      int * /* maxStackTracesPerLocation */,
                                      int * /* reportingSeverity         */,
                                      int * /* reportingFrequency        */);
        // This function can be installed as a configuration callback.  It
        // leaves the configuration unchanged.  This function is used as the
        // default configuration callback.

    static void reportAssertion(bsl::ostream               *out,
                                int                         count,
                                int                         severity,
                                const char                 *text,
                                const char                 *file,
                                int                         line,
                                const bsl::vector<void *>&  stack);
        // Report the specified 'count', 'text', 'file', 'line', and 'stack' to
        // the specified stream 'out' at the specified 'severity' in an
        // implementation-defined format.  If 'out' is null, the assertion will
        // be reported via 'bsls::Log::platformDefaultMessageHandler'.  A
        // functor of this function with 'out' bound to a particular stream
        // pointer can be installed as a reporting callback.  Such a functor
        // with 'out' bound to a null pointer is used as the default reporting
        // callback.
        //
        // While the format is subject to change, it is currently
        // 'severity:file:line:count:message:[ addresses... ]\n'.  Note that
        // 'file' and 'message' may contain embedded colons.  The addresses are
        // written as a sequence of space-separated hexadecimal values without
        // a leading '0x' tag.

    // CREATORS
    explicit AssertionTracker(
        bsls::Assert::Handler  fallback       = bsls::Assert::failureHandler(),
        ConfigurationCallback  configure      = preserveConfiguration,
        bslma::Allocator      *basicAllocator = 0);
        // Create an object of this type.  Optionally specify a 'fallback' used
        // to handle assertions that exceed configured limits.  If 'fallback'
        // is not given, the currently installed failure handler is used.
        // Optionally specify 'configure' to be used to reset configuration
        // each time an assertion occurs.  If 'configure' is not specified, the
        // configuration is left unchanged.  Optionally specify a
        // 'basicAllocator' used to supply memory.  If 'basicAllocator' is 0,
        // the currently installed default allocator is used.

    // MANIPULATORS
    void assertionDetected(const char *text, const char *file, int line);
        // This function is invoked to inform this object that an assertion
        // described by the specified 'text', 'file', and 'line' has occurred.
        // This object will refresh its configuration via its configuration
        // callback.  Depending on the configuration and the number and types
        // of assertions that have already been reported previously, this
        // assertion and its stack trace may be stored, and it may be reported
        // through the reporting callback, the fallback handler, or not at all.
        // See the description above for how configuration controls this
        // behavior.  The behavior is undefined unless 'text' and 'file' have a
        // lifetime exceeding this object and their contents do not change once
        // this method has been called.  (In typical use, both are string
        // constants, and this requirement is trivially met.)

    void setConfigurationCallback(ConfigurationCallback cb);
        // Set the configuration callback function, invoked when an assertion
        // is reported to 'assertionDetected', to the specified 'cb'.
        // Configuration values supplied by the callback replace those
        // configured on this object using this object's other manipulators.
        // The default configured callback retains this object's configuration
        // with no changes.

    void setMaxAssertions(int value);
        // Set the maximum number of assertions that this object will handle to
        // the specified 'value'.  If 'value' is negative, an unlimited number
        // of assertions can be handled.  If there is an assertion failure
        // beyond the limit, the assertion will be passed to the fallback
        // handler supplied at construction.

    void setMaxLocations(int value);
        // Set the maximum number of assertion locations that this object will
        // handle to the specified 'value'.  If 'value' is negative, an
        // unlimited number of locations can be handled.  If there is an
        // assertion failure beyond the limit, the assertion will be passed to
        // the fallback handler supplied at construction.

    void setMaxStackTracesPerLocation(int value);
        // Set the maximum number of stack traces for a given location that
        // this object will handle to the specified 'value'.  If 'value' is
        // negative, an unlimited number of stack traces can be handled.  If
        // there is an assertion failure beyond the limit, the assertion will
        // be passed to the fallback handler supplied at construction.

    void setReportingCallback(ReportingCallback cb);
        // Set the callback function, invoked when an assertion reported to
        // 'assertionDetected' meets the configured criteria for reporting, to
        // the specified 'cb'.

    void setReportingFrequency(ReportingFrequency value);
        // Set the frequency with which assertions are reported to the
        // specified 'value'.  If 'value' is 'e_onNewLocation', an assertion
        // will be reported if it is the first time that file/line assertion
        // location has been seen.  If 'value' is 'e_onNewStackTrace', an
        // assertion will be reported if it is the first time a particular
        // stack trace has been seen (that is, an assertion at a particular
        // file/line location may have multiple stack traces because the
        // function in which it appears is called from a variety of call
        // paths).  Finally, if 'value' is 'e_onEachAssertion', every assertion
        // occurrence will be reported (resulting in multiple reports if the
        // same assertion is triggered multiple times).  This reporting
        // frequency is initially 'e_onNewStackTrace'.

    void setReportingSeverity(bsls::LogSeverity::Enum value);
        // Set the severity level at which assertions will be reported to the
        // specified 'value'.  See {'bsls_logseverity'} for details.  When this
        // object reports assertions using
        // 'bsls::Log::platformDefaultMessageHandler', the severity level is
        // passed to that function.  The severity level is also encoded as a
        // string into the formatted message prepared by 'reportAssertion'.

    // ACCESSORS
    bslma::Allocator *allocator() const;
        // Return the allocator used by this object to supply memory.

    ConfigurationCallback configurationCallback() const;
        // Return the configuration callback used to configure this object.

    int maxAssertions() const;
        // Return the maximum number of assertions that this object will handle
        // or a negative value if the number is unlimited.

    int maxLocations() const;
        // Return the maximum number of locations that this object will handle
        // or a negative value if the number is unlimited.

    int maxStackTracesPerLocation() const;
        // Return the maximum number of stack traces for a given location that
        // this object will handle or a negative value if the number is
        // unlimited.

    void reportAllRecordedStackTraces() const;
        // This method invokes the reporting callback for each saved stack
        // trace, in arbitrary order.

    ReportingCallback reportingCallback() const;
        // Return the callback functor used to report assertions.

    ReportingFrequency reportingFrequency() const;
        // Return the frequency with which assertions are reported.  The
        // possible values are
        //: o e_onNewLocation (0)   - report first time for each location
        //: o e_onNewStackTrace (1) - report first time for each stack trace
        //: o e_onEachAssertion (2) - report every assertion occurrence

    bsls::LogSeverity::Enum reportingSeverity() const;
        // Return the severity with which assertions are reported.  See
        // {'bsls_logseverity'} for details.
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
