// balm_stopwatchscopedguard.h                                        -*-C++-*-

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALM_STOPWATCHSCOPEDGUARD
#define INCLUDED_BALM_STOPWATCHSCOPEDGUARD

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT("$Id: $")

//@PURPOSE: Provide a scoped guard for recording elapsed time.
//
//@CLASSES:
// balm::StopwatchScopedGuard: guard for recording a metric for elapsed time
//
//@SEE_ALSO: balm_metricsmanager, balm_defaultmetricsmanager, balm_metric
//
//@DESCRIPTION: This component provides a scoped guard class intended to
// simplify the task of recording (to a metric) the elapsed time of a block of
// code.  The 'balm::StopwatchScopedGuard' is supplied the identity of a metric
// on construction, and an optional enumerated constant indicating the time
// units to report values in (by default, values are reported in seconds).  The
// guard measures the elapsed time between its construction and destruction,
// and on destruction records that elapsed time, in the indicated time units,
// to the supplied metric.
//
///Choosing Between 'balm::StopwatchScopedGuard' and Macros
///--------------------------------------------------------
// The 'balm::StopwatchScopedGuard' class and the macros defined in the
// 'balm_metrics' component provide the same basic functionality.  Clients may
// find that using a 'balm::StopwatchScopedGuard' object (in coordination with
// a 'balm::Metric' object) is better suited to collecting metrics associated
// with a particular instance of a stateful object, while macros are better
// suited to collecting metrics associated with a particular code path (rather
// than an object instance).  In most instances, however, choosing between the
// two is a matter of taste.
//
///Thread Safety
///-------------
// 'balm::StopwatchScopedGuard' is *const* *thread-safe*, meaning that
// accessors may be invoked concurrently from different threads, but it is not
// safe to access or modify a 'balm::StopwatchScopedGuard' in one thread while
// thread modifies the same object.  Note however, that at this another time
// 'balm::StopwatchScopedGuard' provides no manipulator methods.
//
///Usage
///-----
// The following examples demonstrate how to record the elapsed time of a block
// of code using a 'balm::StopwatchScopedGuard'.
//
///Example 1: Create and Configure the Default 'balm::MetricsManager' Instance
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'balm::MetricManager'
// instance and perform a trivial configuration.
//
// First we create a 'balm::DefaultMetricsManagerScopedGuard', which manages
// the lifetime of the default metrics manager instance.  At construction, we
// provide the scoped guard an output stream ('stdout') that it will publish
// metrics to.  Note that the default metrics manager is intended to be created
// and destroyed by the *owner* of 'main'.  An instance of the manager should
// be created during the initialization of an application (while the task has a
// single thread) and destroyed just prior to termination (when there is
// similarly a single thread).
//..
//  int main(int argc, char *argv[])
//  {
//
//  // ...
//
//      balm::DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default instance has been created, it can be accessed using the
// 'instance' operation:
//..
//     balm::MetricsManager *manager = balm::DefaultMetricsManager::instance();
//     assert(0 != manager);
//..
// Note that the default metrics manager will be released when 'managerGuard'
// exits this scoped and is destroyed.  Clients that choose to explicitly call
// the 'balm::DefaultMetricsManager::create' method must also explicitly call
// the 'balm::DefaultMetricsManager::release' method.
//
///Example 2: Metric Collection with 'balm::StopwatchScopedGuard'
/// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Alternatively, we can use the 'balm::StopwatchScopedGuard' to record metric
// values.  In the following example we implement a hypothetical request
// processor similar to the one in example 3.  We use a 'balm::Metric'
// ('d_elapsedTime') and a 'balm::StopwatchScopedGuard' ('guard') to record the
// elapsed time of the request-processing function.
//..
//  class RequestProcessor {
//
//      // DATA
//      balm::Metric d_elapsedTime;
//
//    public:
//
//      // CREATORS
//      RequestProcessor()
//      : d_elapsedTime("MyCategory", "RequestProcessor/elapsedTime")
//      {}
//
//      // MANIPULATORS
//      int processRequest(const bsl::string& request)
//          // Process the specified 'request'.  Return 0 on success, and a
//          // non-zero value otherwise.
//      {
//         int returnCode = 0;
//
//         balm::StopwatchScopedGuard guard(&d_elapsedTime);
//
//  // ...
//
//         return returnCode;
//      }
//
//  // ...
//  };
//
//  // ...
//
//      RequestProcessor processor;
//
//      processor.processRequest("ab");
//      processor.processRequest("abc");
//      processor.processRequest("abc");
//      processor.processRequest("abdef");
//
//      manager->publishAll();
//
//      processor.processRequest("ab");
//      processor.processRequest("abc");
//      processor.processRequest("abc");
//      processor.processRequest("abdef");
//
//      processor.processRequest("a");
//      processor.processRequest("abc");
//      processor.processRequest("abc");
//      processor.processRequest("abdefg");
//
//      manager->publishAll();
//
//..

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BALM_COLLECTOR
#include <balm_collector.h>
#endif

#ifndef INCLUDED_BALM_COLLECTORREPOSITORY
#include <balm_collectorrepository.h>
#endif

#ifndef INCLUDED_BALM_DEFAULTMETRICSMANAGER
#include <balm_defaultmetricsmanager.h>
#endif

#ifndef INCLUDED_BALM_METRIC
#include <balm_metric.h>
#endif

#ifndef INCLUDED_BALM_METRICSMANAGER
#include <balm_metricsmanager.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_STOPWATCH
#include <bsls_stopwatch.h>
#endif

namespace BloombergLP {

namespace balm {
                         // ==========================
                         // class StopwatchScopedGuard
                         // ==========================

class StopwatchScopedGuard {
    // This class provides a mechanism for recording, to a metric, the elapsed
    // time from the construction of an instance of the guard until that
    // instance goes out of scope (and is destroyed).  The constructor of this
    // class takes an optional argument indicating the time units in which to
    // report the elapsed time; by default a guard will report time in seconds.
    // The supplied time units determine the scale of the double value reported
    // by this guard, but does *not* affect the precision of the elapsed time
    // measurement.  Each instance of this class delegates to a 'Collector' for
    // the metric.  This 'Collector' is initialized on construction based on
    // the constructor arguments.  If this scoped guard is not initialized with
    // an active metric, or if the supplied metric becomes inactive before the
    // scoped guard is destroyed, then 'isActive()' will return 'false' and no
    // metric values will be recorded.  Note that if the metric supplied at
    // construction is not active when the scoped guard is constructed, the
    // scoped guard will not become active or record metric values regardless
    // of the future state of that supplied metric.

  public:
    // PUBLIC TYPES
    enum Units {
        // An enumeration of supported time units.

        k_NANOSECONDS   = 1000000000,
        k_MICROSECONDS  = 1000000,
        k_MILLISECONDS  = 1000,
        k_SECONDS       = 1
    };

  private:
    // DATA
    bsls::Stopwatch d_stopwatch;    // stopwatch

    Units           d_timeUnits;    // time units to record elapsed time in

    Collector *d_collector_p;  // metric collector (held, not owned); may
                                    // be 0, but cannot be invalid

    // NOT IMPLEMENTED
    StopwatchScopedGuard(const StopwatchScopedGuard&);
    StopwatchScopedGuard& operator=(const StopwatchScopedGuard&);

  public:
    // CREATORS
    explicit StopwatchScopedGuard(Metric *metric,
                                  Units   timeUnits = k_SECONDS);
        // Initialize this scoped guard to record elapsed time using the
        // specified 'metric'.  Optionally specify the 'timeUnits' in which to
        // report elapsed time.  If 'metric->isActive()' is 'false', this
        // object will also be inactive (i.e., will not record any values).
        // The behavior is undefined unless 'metric' is a valid address of a
        // 'Metric' object.  Note that 'timeUnits' indicates the scale of the
        // double value reported by this guard, but does *not* affect the
        // precision of the elapsed time measurement.

    explicit StopwatchScopedGuard(Collector *collector,
                                  Units      timeUnits = k_SECONDS);
        // Initialize this scoped guard to record elapsed time using the
        // specified 'collector'.  Optionally specify the 'timeUnits' in which
        // to report elapsed time.  If 'collector' is 0 or
        //'collector->category().enabled() == false', this object will be
        // inactive (i.e., will not record any values).  The behavior is
        // undefined unless
        // 'collector == 0 || collector->metricId().isValid()'.  Note that
        // 'timeUnits' indicates the scale of the double value reported by
        // this guard, but does *not* affect the precision of the elapsed time
        // measurement.

    StopwatchScopedGuard(const MetricId&  metricId,
                         MetricsManager  *manager = 0);
    StopwatchScopedGuard(const MetricId&  metricId,
                         Units            timeUnits,
                         MetricsManager  *manager = 0);
        // Initialize this scoped guard to record an elapsed time to the
        // specified 'metricId' from the optionally specified 'manager'.
        // Optionally specify the 'timeUnits' in which to report elapsed time.
        // If 'timeUnits' is not provided, the elapsed time will be reported in
        // seconds.  If 'manager' is 0, the 'DefaultMetricsManager' singleton
        // instance is used.  If no 'manager' is supplied and the default
        // instance has not been created, this object will be inactive (i.e.,
        // it will not record any values); similarly, if the metric's
        // associated category is disabled (i.e.,
        // 'metricId.category()->enabled()' is 'false'), then this object will
        // be inactive.  The behavior is undefined unless unless 'metricId' is
        // a valid id returned by the 'MetricRepository' object owned by the
        // indicated metrics manager.  Note that 'timeUnits' indicates the
        // scale of the double value reported by this guard, but does *not*
        // affect the precision of the elapsed time measurement.

    StopwatchScopedGuard(const char     *category,
                         const char     *name,
                         MetricsManager *manager = 0);
    StopwatchScopedGuard(const char     *category,
                         const char     *name,
                         Units           timeUnits,
                         MetricsManager *manager = 0);
        // Initialize this scoped guard to record an elapsed time to the
        // metric, identified by the specified 'category' and 'name', from the
        // optionally specified 'manager'.  Optionally specify the 'timeUnits'
        // in which to report elapsed time.  If 'timeUnits' is not provided,
        // the elapsed time will be reported in seconds.  If 'manager' is 0,
        // use the 'DefaultMetricsManager' instance.  If no 'manager' is
        // supplied, and the default instance has not been created, this
        // object will be inactive (i.e., it will not record any values);
        // similarly, if the identified 'category' is disabled, then this
        // object will be inactive.  The behavior is undefined unless
        // 'category' and 'name' are null-terminated.  Note that 'timeUnits'
        // indicates the scale of the double value reported by this guard, but
        // does *not* affect the precision of the elapsed time measurement.

    ~StopwatchScopedGuard();
        // Destroy this scoped guard and, if the scoped guard is active,
        // record the accumulated elapsed time from its creation..

    // ACCESSORS
    bool isActive() const;
        // Return 'true' if this scoped guard will actively record metrics, and
        // 'false' otherwise.  If the returned value is 'false' the destructor
        // will not record a value to the metric.  A scoped guard will be
        // inactive if either (1) it was not initialized with a valid metric,
        // (2) the metric it was initialized with was not active at the time
        // of construction, or (3) the metric supplied at construction is
        // currently inactive, meaning the category of metrics this metric
        // belongs to has been disabled since this object's construction (see
        // the 'MetricsManager' method 'setCategoryEnabled').
};

// ============================================================================
//                            INLINE DEFINITIONS
// ============================================================================

                         // --------------------------
                         // class StopwatchScopedGuard
                         // --------------------------

// CREATORS
inline
StopwatchScopedGuard::StopwatchScopedGuard(Metric *metric,
                                           Units   timeUnits)
: d_stopwatch()
, d_timeUnits(timeUnits)
, d_collector_p(metric->isActive() ? metric->collector() : 0)
{
    if (d_collector_p) {
        d_stopwatch.start();
    }
}

inline
StopwatchScopedGuard::StopwatchScopedGuard(Collector *collector,
                                           Units      timeUnits)
: d_stopwatch()
, d_timeUnits(timeUnits)
, d_collector_p((collector && collector->metricId().category()->enabled())
                ? collector
                : 0)
{
    if (d_collector_p) {
        d_stopwatch.start();
    }
}

inline
StopwatchScopedGuard::StopwatchScopedGuard(const MetricId&  metricId,
                                           MetricsManager  *manager)
: d_stopwatch()
, d_timeUnits(k_SECONDS)
, d_collector_p(0)
{
    Collector *collector = Metric::lookupCollector(metricId, manager);
    d_collector_p = (collector &&
                     collector->metricId().category()->enabled())
                    ? collector : 0;
    if (d_collector_p) {
        d_stopwatch.start();
    }
}

inline
StopwatchScopedGuard::StopwatchScopedGuard(const MetricId&  metricId,
                                           Units            timeUnits,
                                           MetricsManager  *manager)
: d_stopwatch()
, d_timeUnits(timeUnits)
, d_collector_p(0)
{
    Collector *collector = Metric::lookupCollector(metricId, manager);
    d_collector_p = (collector &&
                     collector->metricId().category()->enabled())
                    ? collector : 0;
    if (d_collector_p) {
        d_stopwatch.start();
    }
}

inline
StopwatchScopedGuard::StopwatchScopedGuard(const char     *category,
                                           const char     *name,
                                           MetricsManager *manager)
: d_stopwatch()
, d_timeUnits(k_SECONDS)
, d_collector_p(0)
{
    Collector *collector = Metric::lookupCollector(category, name, manager);

    d_collector_p = (collector &&
                     collector->metricId().category()->enabled())
                    ? collector : 0;

    if (d_collector_p) {
        d_stopwatch.start();
    }
}

inline
StopwatchScopedGuard::StopwatchScopedGuard(const char     *category,
                                           const char     *name,
                                           Units           timeUnits,
                                           MetricsManager *manager)
: d_stopwatch()
, d_timeUnits(timeUnits)
, d_collector_p(0)
{
    Collector *collector = Metric::lookupCollector(category, name, manager);
    d_collector_p = (collector && collector->metricId().category()->enabled())
                    ? collector : 0;
    if (d_collector_p) {
        d_stopwatch.start();
    }
}

inline
StopwatchScopedGuard::~StopwatchScopedGuard()
{
    if (isActive()) {
        d_collector_p->update(d_stopwatch.elapsedTime() * d_timeUnits);
    }
}

// ACCESSORS
inline
bool StopwatchScopedGuard::isActive() const
{
    return 0 != d_collector_p
        && d_collector_p->metricId().category()->enabled();
}

}  // close package namespace
}  // close enterprise namespace

#endif

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
