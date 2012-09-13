// baem_stopwatchscopedguard.h              -*-C++-*-
#ifndef INCLUDED_BAEM_STOPWATCHSCOPEDGUARD
#define INCLUDED_BAEM_STOPWATCHSCOPEDGUARD

#ifndef INCLUDED_BDES_IDENT
#include <bdes_ident.h>
#endif
BDES_IDENT("$Id: $")

//@PURPOSE: Provide a scoped guard for recording elapsed time.
//
//@CLASSES:
// baem_StopwatchScopedGuard: guard for recording a metric for elapsed time
//
//@SEE_ALSO: baem_metricsmanager, baem_defaultmetricsmanager, baem_metric
//
//@AUTHOR: Henry Verschell (hverschell)
//
//@DESCRIPTION: This component provides a scoped guard class intended to
// simplify the task of recording (to a metric) the elapsed time of a block of
// code.  The 'baem_StopwatchScopedGuard' is supplied the identity
// of a metric on construction, and an optional enumerated constant
// indicating the time units to report values in (by default, values are
// reported in seconds).  The guard measures the elapsed time between its
// construction and destruction, and on destruction records that elapsed time,
// in the indicated time units, to the supplied metric.
//
///Choosing Between 'baem_StopwatchScopedGuard' and Macros
///-------------------------------------------------------
// The 'baem_StopwatchScopedGuard' class and the macros defined in the
// 'baem_metrics' component provide the same basic functionality.  Clients may
// find that using a 'baem_StopwatchScopedGuard' object (in coordination with
// a 'baem_Metric' object) is better suited to collecting metrics associated
// with a particular instance of a stateful object, while macros are better
// suited to collecting metrics associated with a particular code path (rather
// than an object instance).  In most instances, however, choosing between the
// two is a matter of taste.
//
///Thread Safety
///-------------
// 'baem_StopwatchScopedGuard' is *const* *thread-safe*, meaning that
// accessors may be invoked concurrently from different threads, but it is
// not safe to access or modify a 'baem_StopwatchScopedGuard' in one thread
// while another thread modifies the same object.  Note however, that at this
// time 'baem_StopwatchScopedGuard' provides no manipulator methods.
//
///Usage
///-----
// The following examples demonstrate how to record the elapsed time of a block
// of code using a 'baem_StopwatchScopedGuard'.
//
///Example 1 - Create and Configure the Default 'baem_MetricsManager' Instance
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// This example demonstrates how to create the default 'baem_MetricManager'
// instance and perform a trivial configuration.
//
// First we create a 'baem_DefaultMetricsManagerScopedGuard', which manages the
// lifetime of the default metrics manager instance.  At construction, we
// provide the scoped guard an output stream ('stdout') to which it will
// publish metrics.  Note that the default metrics manager is intended to be
// created and destroyed by the *owner* of 'main'.  An instance of the manager
// should be created during the initialization of an application (while the
// task has a single thread) and destroyed just prior to termination (when
// there is similarly a single thread).
//..
//  int main(int argc, char *argv[])
//  {
//
//  // ...
//
//      baem_DefaultMetricsManagerScopedGuard managerGuard(bsl::cout);
//..
// Once the default instance has been created, it can be accessed using the
// 'instance' operation:
//..
//      baem_MetricsManager *manager = baem_DefaultMetricsManager::instance();
//      assert(0 != manager);
//..
// Note that the default metrics manager will be destroyed when 'managerGuard'
// goes out of scope.  Clients that choose to call
// 'baem_DefaultMetricsManager::create()' explicitly must also explicitly call
// 'baem_DefaultMetricsManager::destroy()'.
//
///Example 2 - Metric Collection with 'baem_StopwatchScopedGuard'
///- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// Alternatively, we can use the 'baem_StopwatchScopedGuard' to record metric
// values.  In the following example we implement a hypothetical request
// processor similar to the one in example 2.  We use a 'baem_Metric'
// ('d_elapsedTime') and a 'baem_StopwatchScopedGuard' ('guard') to record the
// elapsed time of the request-processing function.
//..
//  class RequestProcessor {
//
//      // DATA
//      baem_Metric d_elapsedTime;
//
//    public:
//      // CREATORS
//      RequestProcessor()
//      : d_elapsedTime("MyCategory", "RequestProcessor/elapsedTime")
//      {}
//
//      // MANIPULATORS
//      int processRequest(const bsl::string& request)
//          // Process the specified 'request'; return 0 on success, and a
//          // non-zero value otherwise.
//      {
//         int returnCode = 0;
//
//         baem_StopwatchScopedGuard guard(&d_elapsedTime);
//
//         // Perform some task.
//
//         return returnCode;
//      }
//
//  // ...
//  };
//..

#ifndef INCLUDED_BAESCM_VERSION
#include <baescm_version.h>
#endif

#ifndef INCLUDED_BAEM_COLLECTOR
#include <baem_collector.h>
#endif

#ifndef INCLUDED_BAEM_COLLECTORREPOSITORY
#include <baem_collectorrepository.h>
#endif

#ifndef INCLUDED_BAEM_DEFAULTMETRICSMANAGER
#include <baem_defaultmetricsmanager.h>
#endif

#ifndef INCLUDED_BAEM_METRIC
#include <baem_metric.h>
#endif

#ifndef INCLUDED_BAEM_METRICSMANAGER
#include <baem_metricsmanager.h>
#endif

#ifndef INCLUDED_BSLS_PLATFORM
#include <bsls_platform.h>
#endif

#ifndef INCLUDED_BSLS_STOPWATCH
#include <bsls_stopwatch.h>
#endif

namespace BloombergLP {

                     // ===============================
                     // class baem_StopwatchScopedGuard
                     // ===============================

class baem_StopwatchScopedGuard {
    // This class provides a mechanism for recording, to a metric, the elapsed
    // time from the construction of an instance of the guard until that
    // instance goes out of scope (and is destroyed).  The constructor of this
    // class takes an optional argument indicating the time units in which to
    // report the elapsed time; by default a guard will report time in seconds.
    // The supplied time units determine the scale of the double value reported
    // by this guard, but does *not* affect the precision of the elapsed time
    // measurement.  Each instance of this class delegates to a
    // 'baem_Collector' for the metric.  This 'baem_Collector' is initialized
    // on construction based on the constructor arguments.  If this scoped
    // guard is not initialized with an active metric, or if the supplied
    // metric becomes inactive before the scoped guard is destroyed, then
    // 'isActive()' will return 'false' and no metric values will be recorded.
    // Note that if the metric supplied at construction is not active when the
    // scoped guard is constructed, the scoped guard will not become active or
    // record metric values regardless of the future state of that supplied
    // metric.

  public:
    // PUBLIC TYPES
    enum Units {
        // An enumeration of supported time units.

        BAEM_NANOSECONDS   = 1000000000,
        BAEM_MICROSECONDS  = 1000000,
        BAEM_MILLISECONDS  = 1000,
        BAEM_SECONDS       = 1
#ifndef BDE_OMIT_INTERNAL_DEPRECATED
      , NANOSECONDS  = BAEM_NANOSECONDS
      , MICROSECONDS = BAEM_MICROSECONDS
      , MILLISECONDS = BAEM_MILLISECONDS
      , SECONDS      = BAEM_SECONDS
#endif // BDE_OMIT_INTERNAL_DEPRECATED
    };

  private:
    // DATA
    bsls_Stopwatch  d_stopwatch;    // stopwatch

    Units           d_timeUnits;    // time units to record elapsed time in

    baem_Collector *d_collector_p;  // metric collector (held, not owned); may
                                    // be 0, but cannot be invalid

    // NOT IMPLEMENTED
    baem_StopwatchScopedGuard(const baem_StopwatchScopedGuard&);
    baem_StopwatchScopedGuard& operator=(const baem_StopwatchScopedGuard&);

  public:
    // CREATORS
    explicit baem_StopwatchScopedGuard(baem_Metric *metric,
                                       Units        timeUnits = BAEM_SECONDS);
        // Initialize this scoped guard to record elapsed time using the
        // specified 'metric'.  Optionally specify the 'timeUnits' in which to
        // report elapsed time.  If 'metric->isActive()' is 'false', this
        // object will also be inactive (i.e., will not record any values).
        // The behavior is undefined unless 'metric' is a valid address of a
        // 'baem_Metric' object.  Note that 'timeUnits' indicates the scale of
        // the double value reported by this guard, but does *not* affect the
        // precision of the elapsed time measurement.

    explicit baem_StopwatchScopedGuard(
                                     baem_Collector *collector,
                                     Units           timeUnits = BAEM_SECONDS);
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

    baem_StopwatchScopedGuard(const baem_MetricId&  metricId,
                              baem_MetricsManager  *manager = 0);
    baem_StopwatchScopedGuard(const baem_MetricId&  metricId,
                              Units                 timeUnit,
                              baem_MetricsManager  *manager = 0);
        // Initialize this scoped guard to record an elapsed time to the
        // specified 'metricId' from the optionally specified
        // 'manager'.  Optionally specify the 'timeUnits' in which to report
        // elapsed time.  If 'timeUnits' is not provided, the elapsed time
        // will be reported in seconds.  If 'manager' is 0, the
        // 'baem_DefaultMetricsManager' singleton instance is used.  If no
        // 'manager' is supplied and the default instance has not been
        // created, this object will be inactive (i.e., it will not record any
        // values); similarly, if the metric's associated category
        // is disabled (i.e., 'metricId.category()->enabled()' is 'false'),
        // then this object will be inactive.  The behavior is undefined
        // unless unless 'metricId' is a valid id returned by the
        // 'baem_MetricRepository' object owned by the indicated metrics
        // manager.  Note that 'timeUnits' indicates the scale of the double
        // value reported by this guard, but does *not* affect the precision
        // of the elapsed time measurement.

    baem_StopwatchScopedGuard(const char          *category,
                              const char          *name,
                              baem_MetricsManager *manager = 0);
    baem_StopwatchScopedGuard(const char          *category,
                              const char          *name,
                              Units                timeUnit,
                              baem_MetricsManager *manager = 0);
        // Initialize this scoped guard to record an elapsed time to the
        // metric, identified by the specified 'category' and 'name', from the
        // optionally specified 'manager'.  Optionally specify the 'timeUnits'
        // in which to report elapsed time.  If 'timeUnit' is not provided, the
        // elapsed time will be reported in seconds.  If 'manager' is 0, use
        // the 'baem_DefaultMetricsManager' instance.  If no 'manager' is
        // supplied, and the default instance has not been created, this
        // object will be inactive (i.e., it will not record any values);
        // similarly, if the identified 'category' is disabled, then this
        // object will be inactive.  The behavior is undefined unless
        // 'category' and 'name' are null-terminated.  Note that 'timeUnits'
        // indicates the scale of the double value reported by this guard, but
        // does *not* affect the precision of the elapsed time measurement.

    ~baem_StopwatchScopedGuard();
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
        // the 'baem_MetricsManager' method 'setCategoryEnabled').
};

// ===========================================================================
//                      INLINE FUNCTION DEFINITIONS
// ===========================================================================

                    // -------------------------------
                    // class baem_StopwatchScopedGuard
                    // -------------------------------

// CREATORS
inline
baem_StopwatchScopedGuard::baem_StopwatchScopedGuard(baem_Metric *metric,
                                                     Units        timeUnits)
: d_stopwatch()
, d_timeUnits(timeUnits)
, d_collector_p(metric->isActive() ? metric->collector() : 0)
{
    if (d_collector_p) {
        d_stopwatch.start();
    }
}

inline
baem_StopwatchScopedGuard::baem_StopwatchScopedGuard(baem_Collector *collector,
                                                     Units           timeUnits)
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
baem_StopwatchScopedGuard::baem_StopwatchScopedGuard(
                                               const baem_MetricId&  metricId,
                                               baem_MetricsManager  *manager)
: d_stopwatch()
, d_timeUnits(BAEM_SECONDS)
, d_collector_p(0)
{
    baem_Collector *collector = baem_Metric::lookupCollector(metricId,
                                                             manager);
    d_collector_p = (collector &&
                     collector->metricId().category()->enabled())
                    ? collector : 0;
    if (d_collector_p) {
        d_stopwatch.start();
    }
}

inline
baem_StopwatchScopedGuard::baem_StopwatchScopedGuard(
                                               const baem_MetricId&  metricId,
                                               Units                 timeUnits,
                                               baem_MetricsManager  *manager)
: d_stopwatch()
, d_timeUnits(timeUnits)
, d_collector_p(0)
{
    baem_Collector *collector = baem_Metric::lookupCollector(metricId,
                                                             manager);
    d_collector_p = (collector &&
                     collector->metricId().category()->enabled())
                    ? collector : 0;
    if (d_collector_p) {
        d_stopwatch.start();
    }
}

inline
baem_StopwatchScopedGuard::baem_StopwatchScopedGuard(
                                            const char          *category,
                                            const char          *name,
                                            baem_MetricsManager *manager)
: d_stopwatch()
, d_timeUnits(BAEM_SECONDS)
, d_collector_p(0)
{
    baem_Collector *collector =
                         baem_Metric::lookupCollector(category, name, manager);

    d_collector_p = (collector &&
                     collector->metricId().category()->enabled())
                    ? collector : 0;

    if (d_collector_p) {
        d_stopwatch.start();
    }
}

inline
baem_StopwatchScopedGuard::baem_StopwatchScopedGuard(
                                            const char          *category,
                                            const char          *name,
                                            Units                timeUnits,
                                            baem_MetricsManager *manager)
: d_stopwatch()
, d_timeUnits(timeUnits)
, d_collector_p(0)
{
    baem_Collector *collector =
                         baem_Metric::lookupCollector(category, name, manager);

    d_collector_p = (collector &&
                     collector->metricId().category()->enabled())
                    ? collector : 0;

    if (d_collector_p) {
        d_stopwatch.start();
    }
}

inline
baem_StopwatchScopedGuard::~baem_StopwatchScopedGuard()
{
    if (d_collector_p && d_collector_p->metricId().category()->enabled()) {
        d_collector_p->update(d_stopwatch.elapsedTime() * d_timeUnits);
    }
}

// ACCESSORS
inline
bool baem_StopwatchScopedGuard::isActive() const
{
    return 0 != d_collector_p
        && d_collector_p->metricId().category()->enabled();
}

}  // close namespace BloombergLP

#endif

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2009
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
